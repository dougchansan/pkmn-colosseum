#!/usr/bin/env python3
"""resolve_abi.py - resolve a function's ABI from its src/ signature so the
auto-harness can place arguments in the right registers.

Emits a small C header (abi.inc) describing:
  - ORC_NARGS, and for each arg: kind (INT/PTR/FLT), its GPR or FPR slot
  - ORC_RET kind (INT in r3 / FLT in f1 / VOID)
plus a PASS/FAIL flag in stdout:
  ABI_OK   <ret_kind> <argspec>
  ABI_FAIL <reason>

ABI mapping (GameCube EABI):
  - integer/pointer args consume GPRs r3,r4,r5,...   (each one GPR; we treat
    64-bit as unsupported -> ABI_FAIL)
  - float/double args consume FPRs f1,f2,...          (separate file)
  NOTE: arg->register order follows declaration order; ints and floats advance
  their own register files independently (standard PPC EABI).

We REFUSE (ABI_FAIL) when we cannot place inputs deterministically:
  - `void` parameter list with a non-void-ABI body (args come from globals/regs
    implicitly) -> can't fuzz -> AUTOHARNESS-FAILED
  - varargs (...) ; struct-BY-VALUE params ; unknown typedef'd struct returns
"""
import re, sys

INT_TYPES = {'int','s32','u32','s16','u16','s8','u8','char','short','long',
             'unsigned','signed','bool','bool8','BOOL','size_t','bool32'}
FLT_TYPES = {'float','f32','double','f64'}

def classify_type(t):
    t = t.strip()
    if '*' in t:
        return 'PTR'
    base = t.replace('const','').strip().split()[-1] if t.split() else t
    # pointer via typedef like 'HSD_CObj*' already caught; bare word:
    if base in FLT_TYPES:
        return 'FLT'
    if base in INT_TYPES:
        return 'INT'
    # typedef'd scalar (e.g. GXBool=u8) -> treat as INT (1 GPR) conservatively
    # but unknown aggregate by value is unsafe:
    return 'INT?'   # uncertain scalar; caller decides

def parse_sig(sig):
    # sig like: 'u32 fn_8023CFDC(u32 r3, u32 r4, u32 r5, u32 r6)'
    m = re.match(r'\s*([A-Za-z_][\w\s\*]*?)\s+([A-Za-z_]\w*)\s*\((.*)\)\s*$', sig, re.S)
    if not m:
        return None
    ret, name, params = m.group(1).strip(), m.group(2), m.group(3).strip()
    return ret, name, params

def main():
    if len(sys.argv) != 3:
        print("usage: resolve_abi.py <signature-string> <out.inc>", file=sys.stderr)
        print("ABI_FAIL bad-invocation"); sys.exit(2)
    sig, out = sys.argv[1], sys.argv[2]
    p = parse_sig(sig)
    if not p:
        print("ABI_FAIL cannot-parse-signature"); sys.exit(1)
    ret, name, params = p

    if '...' in params:
        print("ABI_FAIL varargs"); sys.exit(1)

    # return kind
    rk = classify_type(ret)
    if ret.strip() == 'void':
        ret_kind = 'VOID'
    elif rk == 'FLT':
        ret_kind = 'FLT'
    elif rk in ('INT','PTR','INT?'):
        ret_kind = 'INT'
    else:
        print(f"ABI_FAIL unknown-return-type:{ret}"); sys.exit(1)

    # params
    plist = [x.strip() for x in params.split(',')] if params and params != 'void' else []
    if params == 'void' or params == '':
        # void parameter list: cannot fuzz inputs (implicit args). REFUSE.
        print("ABI_FAIL void-param-list-implicit-args"); sys.exit(1)

    args = []   # (kind, type)
    gpr = 3; fpr = 1
    specs = []
    for prm in plist:
        # strip the param name; keep the type
        mm = re.match(r'(.*?)([A-Za-z_]\w*)?\s*$', prm.strip())
        typ = prm
        # remove trailing identifier
        toks = prm.split()
        if len(toks) >= 2 and '*' not in toks[-1]:
            typ = ' '.join(toks[:-1]) if not prm.rstrip().endswith('*') else prm
        k = classify_type(typ)
        if k == 'FLT':
            specs.append(('FLT', fpr)); fpr += 1
        elif k in ('INT','PTR','INT?'):
            specs.append(('PTR' if k=='PTR' else 'INT', gpr)); gpr += 1
        else:
            print(f"ABI_FAIL unsupported-param-type:{typ}"); sys.exit(1)

    with open(out,'w') as f:
        f.write(f"/* auto ABI for {name}: ret={ret_kind} nargs={len(specs)} */\n")
        f.write(f"#define ORC_RET_{ret_kind} 1\n")
        f.write(f"#define ORC_NARGS {len(specs)}\n")
        f.write("static const struct {{ int kind; int slot; }} ORC_ARGS[] = {{ {} }};\n".format(
            ", ".join("{%s, %d}" % ({'INT':'0','PTR':'1','FLT':'2'}[k], s) for k,s in specs)
            or "{0,3}"))
        # kind: 0=INT,1=PTR,2=FLT
    spec_str = " ".join(f"{k}@{'f' if k=='FLT' else 'r'}{s}" for k,s in specs)
    print(f"ABI_OK {ret_kind} {spec_str}")

if __name__ == "__main__":
    main()
