#!/usr/bin/env python3
"""Match our decoded pobjs (build_pc/our_verts.txt) to the FIFO draws by vertex
position, then report each pobj's per-slot loaded matrices (the game's
view*skinMtx). Uniform matrices across slots => model-space pobj; varied =>
bone-local. Also derives skinMtx = view^-1 * M using the room view.
"""
import struct, sys, re

DFF = sys.argv[1] if len(sys.argv) > 1 else "fifo1.dff"
d = open(DFF, "rb").read()
fifoOff, = struct.unpack_from("<Q", d, 0x80); fifoSz, = struct.unpack_from("<I", d, 0x88)
muOff, = struct.unpack_from("<Q", d, 0x80+20); nmu, = struct.unpack_from("<I", d, 0x80+28)
fifo = d[fifoOff:fifoOff+fifoSz]; n = len(fifo)
segs = []
for i in range(nmu):
    e = muOff+i*24; addr, = struct.unpack_from("<I", d, e+4)
    doff, = struct.unpack_from("<Q", d, e+8); dsz, = struct.unpack_from("<I", d, e+16)
    segs.append((addr, d[doff:doff+dsz]))
def rd(a, l):
    for ad, b in reversed(segs):
        if ad <= a and a+l <= ad+len(b): return b[a-ad:a-ad+l]
    return None

# --- parse FIFO draws: (positions list, slot->mtx rows) ---
vcd_lo = vcd_hi = 0; vat = [[0,0,0] for _ in range(8)]; bases = {}; strides = {}
xfmem = [0.0]*0x100; loaded = {}
def attrs(v):
    a = vat[v][0]; out = []
    if vcd_lo & 1: out.append(("M","mtx",1))
    for i in range(8):
        if (vcd_lo>>(1+i))&1: out.append(("TM","mtx",1))
    def add(nm, mode, ds):
        if mode == 0: return
        out.append((nm, {2:"i8",3:"i16"}.get(mode,"dir"), {2:1,3:2}.get(mode,ds)))
    pc = 3 if vat[v][0]&1 else 2; pt = (vat[v][0]>>1)&7
    add("POS",(vcd_lo>>9)&3,{0:1,1:1,2:2,3:2,4:4}[pt]*pc)
    add("NRM",(vcd_lo>>11)&3,12); add("C0",(vcd_lo>>13)&3,4); add("C1",(vcd_lo>>15)&3,4)
    for t in range(8): add("T%d"%t,(vcd_hi>>(t*2))&3,8)
    return out
draws = []; p = 0
while p < n:
    op = fifo[p]; p += 1
    if op == 0: continue
    if op == 0x08:
        a = fifo[p]; v, = struct.unpack_from(">I", fifo, p+1); p += 5
        if a == 0x50: vcd_lo = v
        elif a == 0x60: vcd_hi = v
        elif 0x70<=a<=0x77: vat[a&7][0] = v
        elif 0xA0<=a<=0xAF: bases[a] = v
        elif 0xB0<=a<=0xBF: strides[a] = v
    elif op == 0x10:
        h, = struct.unpack_from(">I", fifo, p); p += 4; cnt = ((h>>16)&0xF)+1; addr = h&0xFFFF
        vals = struct.unpack_from(">%df"%cnt, fifo, p); p += cnt*4
        if addr+cnt <= 0x100:
            for i in range(cnt): xfmem[addr+i] = vals[i]
            loaded[(addr//12)*12] = True
    elif op == 0x61: p += 4
    elif op in (0x20,0x28,0x30,0x38): p += 4
    elif 0x80 <= op <= 0xBF:
        at = attrs(op&7); cnt, = struct.unpack_from(">H", fifo, p); p += 2
        pb = bases.get(0xA0); ps = strides.get(0xB0,12); verts = []
        ok = True
        for _ in range(cnt):
            sl = 0; pos = None
            for (nm,k,szb) in at:
                if nm == "M": sl = fifo[p]//3
                if nm == "POS":
                    idx = (fifo[p]<<8|fifo[p+1]) if k=="i16" else (fifo[p] if k=="i8" else None)
                    if idx is not None and pb is not None:
                        b = rd(pb+idx*ps,12)
                        if b: pos = struct.unpack(">fff",b)
                p += szb
            verts.append((sl,pos))
        mats = {s//12: [xfmem[s:s+4],xfmem[s+4:s+8],xfmem[s+8:s+12]] for s in sorted(loaded)}
        draws.append((verts, mats)); loaded = {}
    else: break

# build a position->draw index lookup (rounded positions)
def key(pos): return (round(pos[0],1),round(pos[1],1),round(pos[2],1))
posToDraw = {}
for di,(verts,mats) in enumerate(draws):
    for sl,pos in verts:
        if pos: posToDraw.setdefault(key(pos),[]).append((di,sl))

# --- parse our verts into pobjs ---
our = []  # list of pobjs; each = list of (slot, local)
cur = []
for line in open("build_pc/our_verts.txt"):
    if "bbox" in line:
        if cur: our.append(cur); cur = []
        continue
    m = re.search(r"slot=(\d+).*local=\(([-\d.]+),([-\d.]+),([-\d.]+)\)", line)
    if m:
        cur.append((int(m.group(1)), (float(m.group(2)),float(m.group(3)),float(m.group(4)))))
if cur: our.append(cur)

print(f"our pobjs: {[len(p) for p in our]}; FIFO draws: {len(draws)}")
for pi, pobj in enumerate(our):
    # find which FIFO draws our verts map to
    drawhits = {}
    for sl, loc in pobj:
        for di, fsl in posToDraw.get(key(loc), []):
            drawhits.setdefault(di, 0); drawhits[di] += 1
    best = sorted(drawhits.items(), key=lambda x:-x[1])[:6]
    print(f"\n=== our pobj#{pi+1} ({len(pobj)} verts) -> FIFO draws {best} ===")
    # collect loaded matrices from the matched draws
    matched = set(di for di,_ in best if drawhits[di] >= 3)
    allmats = {}
    for di in matched:
        for sl, m in draws[di][1].items():
            allmats[sl] = m
    if not allmats:
        print("  (no confident match)"); continue
    # report per-slot translate; uniform => model-space, varied => bone-local
    ts = []
    for sl in sorted(allmats):
        t = (allmats[sl][0][3], allmats[sl][1][3], allmats[sl][2][3])
        ts.append(t)
        print(f"    slot {sl}: M translate=({t[0]:.1f},{t[1]:.1f},{t[2]:.1f})")
    if len(ts) >= 2:
        spread = max(abs(a[i]-b[i]) for a in ts for b in ts for i in range(3))
        print(f"    => translate spread across slots = {spread:.2f}  ({'UNIFORM/model-space' if spread<3 else 'VARIED/bone-local'})")
