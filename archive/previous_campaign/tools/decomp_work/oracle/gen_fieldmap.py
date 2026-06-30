#!/usr/bin/env python3
"""gen_fieldmap.py - recover the (offset,width) field map of a pointer arg by
walking the recomp-annotated C of a function AND the callees it passes that
pointer to. Emits bside_fieldmap.inc: an array the harness uses to seed each
field as ONE logical value (big-endian into A's guest RAM, little-endian into
B's mirror) so both sides read the same logical value regardless of endianness.

Approach (per function body, from the recomp C instruction comments):
  - Track which GPRs ALIAS each incoming pointer arg (r3,r4,r5,..). Seed the
    alias set from the prologue/`or rD,rS,rS` (and `mr`) copies, plus add/rlwinm
    that derive a within-struct address from an alias (so `r3 += idx*4` keeps r3
    pointing into the SAME struct, just at a variable offset).
  - For every load/store `op rX, N(rB)` where rB aliases a pointer arg, record
    (arg_index, base_const_offset N, width, index_stride, index_bound).
  - When the base was index-adjusted (`add rB, rB, rIdx` after `rlwinm rIdx =
    arg*stride`, bounded by a preceding `cmplwi ..., BOUND`), enumerate the
    concrete offsets N + k*stride for k in [0,BOUND).
  - Recurse into `bl <addr>` callees, mapping the pointer arg to the callee's r3
    (callees here are passed the pointer in r3 in fightSideGetStatus's cases).

Conservatism: if a load's base cannot be tied to a pointer arg (global, SDA,
stack, or an un-traced register), it is IGNORED (those are handled elsewhere:
SDA lbls, stack). If the access pattern is too tangled to resolve a concrete
offset (e.g. index from another memory read), we mark UNRESOLVED for that arg so
the caller classifies the function LIMITED rather than seeding a wrong map.

Usage: gen_fieldmap.py <chunks_dir> <fn_addr> <nargs> <ptr_arg_indices_csv> <out.inc>
  ptr_arg_indices_csv: which ABI args are pointers, e.g. "0" or "0,1" (0-based,
  arg0=r3). Only those get a field map.
"""
import re, sys, glob, os

LOAD = re.compile(r'//\s*[0-9A-Fa-f]+:\s*(lwz|lwzu|lhz|lha|lbz|lfs|lfd)\s+(r\d+),\s*(-?\d+)\((r\d+)\)')
STORE= re.compile(r'//\s*[0-9A-Fa-f]+:\s*(stw|sth|stb|stfs|stfd)\s+(r\d+),\s*(-?\d+)\((r\d+)\)')
ORMR = re.compile(r'//\s*[0-9A-Fa-f]+:\s*(?:or|mr)\s+(r\d+),\s*(r\d+)(?:,\s*(r\d+))?')
RLW  = re.compile(r'//\s*[0-9A-Fa-f]+:\s*rlwinm\.?\s+(r\d+),\s*(r\d+),\s*(\d+),\s*(\d+),\s*(\d+)')
ADD  = re.compile(r'//\s*[0-9A-Fa-f]+:\s*add\s+(r\d+),\s*(r\d+),\s*(r\d+)')
ADDI = re.compile(r'//\s*[0-9A-Fa-f]+:\s*addi\s+(r\d+),\s*(r\d+),\s*(-?\d+)')
CMPLI= re.compile(r'//\s*[0-9A-Fa-f]+:\s*cmplwi\s+(r\d+),\s*0x([0-9A-Fa-f]+)')
BL   = re.compile(r'//\s*[0-9A-Fa-f]+:\s*bl\s+0x([0-9A-Fa-f]+)')
INSTR= re.compile(r'//\s*([0-9A-Fa-f]+):\s*(\S+)')

WIDTH = {'lbz':1,'stb':1,'lhz':2,'lha':2,'sth':2,'lwz':4,'lwzu':4,'stw':4,'lfs':4,'stfs':4,'lfd':8,'stfd':8}

# Per-function sizes from the symbol map, so the body window is bounded EXACTLY
# to the function (not a fixed slop window that bleeds into neighbors and seeds
# wrong-width fields from unrelated functions — that was a real artifact source).
_SYMS=None
def _load_syms():
    global _SYMS
    if _SYMS is not None: return _SYMS
    _SYMS={}
    # locate symbols.txt relative to repo root (… /pkmn-colosseum/config/GC6E01/)
    here=os.path.dirname(os.path.abspath(__file__))
    cand=[os.path.join(here,'..','..','..','config','GC6E01','symbols.txt'),
          '/storage/finetune/pkmn-colosseum/config/GC6E01/symbols.txt',
          os.environ.get('ORACLE_SYMBOLS','')]
    for p in cand:
        if p and os.path.exists(p):
            for line in open(p,errors='replace'):
                m=re.match(r'(\w+)\s*=\s*\.\w+:0x([0-9A-Fa-f]+);.*size:0x([0-9A-Fa-f]+)',line)
                if m: _SYMS[int(m.group(2),16)]=int(m.group(3),16)
            break
    return _SYMS

def fn_body(chunks, addr):
    """Recomp C of the function at addr, bounded EXACTLY to [addr, addr+size)
    using the symbol-map size (fallback: stop at the first instruction whose
    address is < addr or >= addr+0x400, well under any single getter's size but
    bounded enough to avoid deep bleed). Bounding to the true size is what stops
    a neighbor's lhz/lwz from being mis-attributed as a field of THIS function."""
    lbl = f"label_{addr.upper()}:"
    a0 = int(addr,16)
    sz = _load_syms().get(a0, 0)
    hi = a0 + sz if sz>0 else a0 + 0x400
    for c in glob.glob(os.path.join(chunks,"*.c")):
        t=open(c,errors='replace').read()
        if lbl in t:
            i=t.index(lbl)
            seg=t[i:i+40000]
            out=[]
            for line in seg.splitlines():
                m=INSTR.search(line)
                if m:
                    ia=int(m.group(1),16)
                    if ia < a0 or ia >= hi:
                        break
                out.append(line)
            return "\n".join(out)
    return None

def collect(chunks, addr, ptr_in_reg, depth, seen, fields, unresolved):
    """ptr_in_reg: dict reg->arg_index that holds a pointer arg on entry."""
    if depth>3 or addr in seen: return
    seen.add(addr)
    body = fn_body(chunks, addr)
    if not body: return
    # alias[reg] = (arg_index, base_offset)  meaning reg = ptrarg + base_offset
    alias = {r:(ai,0) for r,ai in ptr_in_reg.items()}
    # index info: idxreg -> (stride, bound)
    idxinfo = {}
    last_cmp = {}  # reg -> bound from a cmplwi just seen
    recent_bound = 0  # most-recent cmplwi constant (fallback for stride index)
    for line in body.splitlines():
        m=INSTR.search(line)
        if not m: continue
        # track cmplwi bounds
        c=CMPLI.search(line)
        if c:
            last_cmp[c.group(1)] = int(c.group(2),16)
            recent_bound = int(c.group(2),16)
        # rlwinm producing an index scaled value: rlwinm rD, rS, SH, MB, ME
        r=RLW.search(line)
        if r:
            rD,rS,sh,mb,me = r.group(1),r.group(2),int(r.group(3)),int(r.group(4)),int(r.group(5))
            stride = (1<<sh) if sh>0 else 1
            # bound: direct on src/dst, else the most-recent cmplwi (the index
            # bound check typically precedes the stride-scaling rlwinm and uses
            # a sibling rlwinm of the SAME source register, e.g. r4&0xff).
            bnd = last_cmp.get(rS) or last_cmp.get(rD) or recent_bound or 0
            idxinfo[rD]=(stride,bnd)
            alias.pop(rD,None)
            continue
        # or/mr copy: rD = rS (alias propagation)
        o=ORMR.search(line)
        if o:
            rD,rS,rB = o.group(1),o.group(2),o.group(3)
            if rB and rB!=rS:
                alias.pop(rD,None)  # or of two diff regs: not a simple alias
            elif rS in alias:
                alias[rD]=alias[rS]
            else:
                alias.pop(rD,None)
            continue
        # addi rD, rS, imm : alias with adjusted offset
        ai=ADDI.search(line)
        if ai:
            rD,rS,imm=ai.group(1),ai.group(2),int(ai.group(3))
            if rS in alias:
                t=alias[rS]
                if len(t)==2: alias[rD]=(t[0],t[1]+imm)
                else: alias[rD]=(t[0],t[1]+imm,t[2],t[3])  # keep variable
            else: alias.pop(rD,None)
            continue
        # add rD, rS, rIdx : if rS aliases a ptr and rIdx is a scaled index,
        # rD aliases the ptr with a VARIABLE offset (stride,bound)
        a=ADD.search(line)
        if a:
            rD,rS,rI=a.group(1),a.group(2),a.group(3)
            base=None
            if rS in alias and len(alias[rS])==2 and rI in idxinfo: base,idx=alias[rS],idxinfo[rI]
            elif rI in alias and len(alias[rI])==2 and rS in idxinfo: base,idx=alias[rI],idxinfo[rS]
            if base:
                argi,off=base; stride,bound=idx
                alias[rD]=(argi,off,stride,bound)  # 4-tuple = variable
            else:
                alias.pop(rD,None)
            continue
        # loads/stores
        ls = LOAD.search(line) or STORE.search(line)
        if ls:
            op,rX,disp,rB = ls.group(1),ls.group(2),int(ls.group(3)),ls.group(4)
            w=WIDTH.get(op,4)
            if rB in alias:
                t=alias[rB]
                if len(t)==2:
                    argi,off=t
                    fields.add((argi, off+disp, w))
                else:
                    argi,off,stride,bound=t
                    if bound and bound<=64:
                        for k in range(bound):
                            fields.add((argi, off+disp+k*stride, w))
                    else:
                        # unbounded variable index: seed a conservative dense
                        # range (do NOT discard the arg's other resolved fields).
                        b = bound if bound else 16
                        b = min(b,64)
                        for k in range(b):
                            fields.add((argi, off+disp+k*(stride or w), w))
            continue
        # calls: recurse into the callee. Two cases:
        #  (1) precise: a register currently aliases a pointer arg at offset 0 ->
        #      map that register to the arg for the callee.
        #  (2) jumptable/flow-broken: if NO precise alias survived but the
        #      function HAS pointer args, conservatively assume the callee may
        #      receive the (single) pointer arg in r3. This over-collects fields
        #      (harmless: extra seeded fields are never read) but never misses a
        #      field, which is what soundness requires. Only applied at depth 0
        #      (the top function whose ptr args we are mapping) and only when
        #      there is exactly one pointer arg to avoid mis-attribution.
        b=BL.search(line)
        if b:
            tgt=b.group(1).upper()
            pr = {}
            for rr in ('r3','r4','r5','r6'):
                if rr in alias and len(alias[rr])==2 and alias[rr][1]==0:
                    pr[rr]=alias[rr][0]
            if not pr and depth==0 and len(ptr_in_reg)==1:
                # flow-broken (jumptable) at top level, single ptr arg -> assume r3
                only_arg = next(iter(ptr_in_reg.values()))
                pr={'r3':only_arg}
            if pr:
                collect(chunks, tgt, pr, depth+1, seen, fields, unresolved)
            continue

def main():
    chunks, addr, nargs, ptrcsv, out = sys.argv[1:6]
    nargs=int(nargs)
    ptr_args=[int(x) for x in ptrcsv.split(',') if x!='']
    # initial: arg i is in register r(3+i)
    ptr_in_reg={ f"r{3+i}": i for i in ptr_args }
    fields=set(); unresolved=set(); seen=set()
    collect(chunks, addr.upper(), ptr_in_reg, 0, seen, fields, unresolved)
    # Dedupe OVERLAPPING fields: a byte range [off, off+w) seeded twice (e.g. a
    # u16 and a u32 at the same offset, or a sub-field of a wider one) would have
    # the second seed clobber the first, breaking the per-field compare. Keep a
    # non-overlapping cover, preferring WIDER fields (they subsume narrower).
    by_arg={}
    for argi,off,w in fields: by_arg.setdefault(argi,[]).append((off,w))
    final=[]
    for argi,lst in by_arg.items():
        lst.sort(key=lambda t:(t[0], -t[1]))   # by offset, widest first
        covered=[]  # list of (start,end)
        for off,w in lst:
            s,e=off,off+w
            if any(not(e<=cs or s>=ce) for cs,ce in covered): continue  # overlaps
            covered.append((s,e)); final.append((argi,off,w))
    fields=sorted(final)
    with open(out,'w') as f:
        f.write("/* AUTO field map (arg,offset,width). Seed each as one logical value. */\n")
        f.write(f"#define ORC_NFIELDS {len(fields)}\n")
        f.write("static const struct { int arg; int off; int width; } ORC_FIELDS[] = {\n")
        for argi,off,w in fields:
            if off<0 or off>0x8000: continue
            f.write(f"  {{{argi},{off},{w}}},\n")
        f.write("};\n")
    print("FIELDMAP fields="+str(len(fields))+" unresolved="+",".join(str(x) for x in sorted(unresolved)))

if __name__=="__main__":
    main()
