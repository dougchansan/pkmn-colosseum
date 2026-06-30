#!/usr/bin/env python3
"""Extract the ACTUAL submitted vertex positions + loaded matrices for skinned
draws from a Dolphin FIFO log, to diff against our HSD decode.

Builds the memory map from the frame's memory-updates, walks the GX stream
tracking CP array base/stride + VCD/VAT + XF position matrices, and for draws
with target vertex counts dumps each vertex's (PNMTXIDX slot, indexed position
read from the game's vertex array) plus the per-slot loaded matrix.
"""
import struct, sys

PATH = sys.argv[1] if len(sys.argv) > 1 else "fifo1.dff"
TARGETS = set(int(x) for x in sys.argv[2:]) or {17, 84, 522, 352}
d = open(PATH, "rb").read(); sz = len(d)

# frame info @0x80
fifoOff, = struct.unpack_from("<Q", d, 0x80)
fifoSz,  = struct.unpack_from("<I", d, 0x88)
muOff,   = struct.unpack_from("<Q", d, 0x80 + 20)
nmu,     = struct.unpack_from("<I", d, 0x80 + 28)
fifo = d[fifoOff:fifoOff + fifoSz]; n = len(fifo)

# memory map: list of (addr, bytes); last write wins
segs = []
for i in range(nmu):
    e = muOff + i * 24
    addr, = struct.unpack_from("<I", d, e + 4)
    doff, = struct.unpack_from("<Q", d, e + 8)
    dsz,  = struct.unpack_from("<I", d, e + 16)
    segs.append((addr, d[doff:doff + dsz]))

def memread(addr, length):
    for a, b in reversed(segs):  # later updates win
        if a <= addr and addr + length <= a + len(b):
            return b[addr - a: addr - a + length]
    return None

def be_f32(buf, o):
    return struct.unpack_from(">f", buf, o)[0]

# GX state
vcd_lo = vcd_hi = 0
vat = [[0, 0, 0] for _ in range(8)]
bases = {}; strides = {}
xfmem = [0.0] * 0x100
loaded = {}

def vtx_attrs(v):
    """Return list of (attr_name, kind, size) in order to parse one vertex.
    kind: 'mtx'(1B), 'idx8'(1B), 'idx16'(2B), 'dir'(size)."""
    a, b, c = vat[v]
    out = []
    if vcd_lo & 1: out.append(("PNMTXIDX", "mtx", 1))
    for i in range(8):
        if (vcd_lo >> (1 + i)) & 1: out.append((f"TEX{i}MTXIDX", "mtx", 1))
    def addattr(name, mode, dirsize):
        if mode == 0: return
        if mode == 2: out.append((name, "idx8", 1))
        elif mode == 3: out.append((name, "idx16", 2))
        else: out.append((name, "dir", dirsize))
    poscnt = 3 if (a & 1) else 2; postyp = (a >> 1) & 7
    addattr("POS", (vcd_lo >> 9) & 3, {0:1,1:1,2:2,3:2,4:4}[postyp]*poscnt)
    addattr("NRM", (vcd_lo >> 11) & 3, 12)
    addattr("CLR0", (vcd_lo >> 13) & 3, 4)
    addattr("CLR1", (vcd_lo >> 15) & 3, 4)
    for t in range(8):
        addattr(f"TEX{t}", (vcd_hi >> (t*2)) & 3, 8)
    return out

draws = []
p = 0
while p < n:
    op = fifo[p]; p += 1
    if op == 0: continue
    if op == 0x08:
        a = fifo[p]; val, = struct.unpack_from(">I", fifo, p+1); p += 5
        if a == 0x50: vcd_lo = val
        elif a == 0x60: vcd_hi = val
        elif 0x70 <= a <= 0x77: vat[a&7][0] = val
        elif 0x80 <= a <= 0x87: vat[a&7][1] = val
        elif 0x90 <= a <= 0x97: vat[a&7][2] = val
        elif 0xA0 <= a <= 0xAF: bases[a] = val
        elif 0xB0 <= a <= 0xBF: strides[a] = val
    elif op == 0x10:
        h, = struct.unpack_from(">I", fifo, p); p += 4
        cnt = ((h>>16)&0xF)+1; addr = h & 0xFFFF
        vals = struct.unpack_from(">%df"%cnt, fifo, p); p += cnt*4
        if addr+cnt <= 0x100:
            for i in range(cnt): xfmem[addr+i] = vals[i]
            loaded[(addr//12)*12] = True
    elif op == 0x61: p += 4
    elif op in (0x20,0x28,0x30,0x38): p += 4
    elif 0x80 <= op <= 0xBF:
        vat_i = op & 7
        cnt, = struct.unpack_from(">H", fifo, p); p += 2
        attrs = vtx_attrs(vat_i)
        # parse vertices
        verts = []
        start = p
        ok = True
        posBase = bases.get(0xA0); posStride = strides.get(0xB0, 12)
        for _ in range(cnt):
            slot = 0; pos = None
            for (name, kind, size) in attrs:
                if p + size > n: ok = False; break
                if kind == "mtx":
                    if name == "PNMTXIDX": slot = fifo[p] // 3
                elif name == "POS":
                    if kind == "idx16": idx = (fifo[p] << 8) | fifo[p+1]
                    elif kind == "idx8": idx = fifo[p]
                    else: idx = None
                    if idx is not None and posBase is not None:
                        buf = memread(posBase + idx*posStride, 12)
                        if buf: pos = (be_f32(buf,0), be_f32(buf,4), be_f32(buf,8))
                p += size
            if not ok: break
            verts.append((slot, pos))
        if not ok: break
        mats = {s: [xfmem[s:s+4], xfmem[s+4:s+8], xfmem[s+8:s+12]] for s in sorted(loaded)}
        draws.append((op&0xF8, cnt, verts, mats, posBase))
        loaded = {}
    else: break

print(f"parsed {len(draws)} draws; targets={sorted(TARGETS)}")
for i,(prim,cnt,verts,mats,posBase) in enumerate(draws):
    if cnt in TARGETS:
        print(f"\n=== draw[{i}] prim=0x{prim:02x} verts={cnt} posBase=0x{(posBase or 0):x} matsLoaded={len(mats)} ===")
        # slot histogram
        from collections import Counter
        sh = Counter(s for s,_ in verts)
        print("  slot hist:", dict(sorted(sh.items())))
        # show first 8 verts: slot + game position
        for j,(slot,pos) in enumerate(verts[:8]):
            ps = f"({pos[0]:.2f},{pos[1]:.2f},{pos[2]:.2f})" if pos else "None"
            print(f"    v{j}: slot={slot} gamePos={ps}")
        # position bbox over all verts
        ps = [p for _,p in verts if p]
        if ps:
            mn=[min(c[k] for c in ps) for k in range(3)]
            mx=[max(c[k] for c in ps) for k in range(3)]
            print(f"  gamePos bbox=[{mn[0]:.1f},{mn[1]:.1f},{mn[2]:.1f} .. {mx[0]:.1f},{mx[1]:.1f},{mx[2]:.1f}]")
