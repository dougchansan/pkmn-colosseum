#!/usr/bin/env python3
"""Parse a Dolphin FIFO log (.dff v6) and extract, per draw call, the GX
position matrices the game loaded (ground truth for our skinning).

We walk the GX command stream: track the CP vertex descriptor (VCD) + VAT so we
can compute each draw's per-vertex byte size (to skip vertex data), track XF
position-matrix-memory writes (GXLoadPosMtxImm -> XF addr 0..0xFF), and on each
draw record (primitive, vertexCount, matrices loaded since the previous draw).

Goal: find the draws whose vertex counts match Wes's PObjs (17/84/522/352) and
dump the matrices the game used, to diff against our jointWorld/skin palette.
"""
import struct, sys

PATH = sys.argv[1] if len(sys.argv) > 1 else "fifo1.dff"
d = open(PATH, "rb").read()

# Frame info at file offset 0x80 (fifoDataOffset u64, fifoDataSize u32, ...)
fifoOff, = struct.unpack_from("<Q", d, 0x80)
fifoSz,  = struct.unpack_from("<I", d, 0x88)
print(f"fifoDataOffset=0x{fifoOff:x} size={fifoSz} (0x{fifoSz:x})")
fifo = d[fifoOff:fifoOff + fifoSz]
n = len(fifo)

# ---- GX state we must track to compute vertex size -------------------------
vcd_lo = 0  # CP reg 0x50
vcd_hi = 0  # CP reg 0x60
vat_a = [0] * 8
vat_b = [0] * 8
vat_c = [0] * 8

def comp_bytes(cnt, typ):
    # typ: 0=u8,1=s8,2=u16,3=s16,4=f32 ; cnt is component count
    sz = {0: 1, 1: 1, 2: 2, 3: 2, 4: 4}.get(typ, 4)
    return sz * cnt

def vtx_size(vat):
    """Bytes per vertex for the current VCD using VAT group `vat`."""
    a, b, c = vat_a[vat], vat_b[vat], vat_c[vat]
    sz = 0
    # matrix-index attrs (1 bit each in vcd_lo): PNMTXIDX bit0, TEXnMTXIDX bits1..8
    if vcd_lo & 1: sz += 1
    for i in range(8):
        if (vcd_lo >> (1 + i)) & 1: sz += 1
    def idxsz(v): return {0: 0, 1: -1, 2: 1, 3: 2}[v]  # -1 == direct (size from VAT)
    # POS (vcd_lo bits 9-10)
    pos = (vcd_lo >> 9) & 3
    if pos:
        if pos >= 2: sz += (pos - 1)  # index8=1, index16=2
        else:
            poscnt = (a >> 0) & 1; postyp = (a >> 1) & 7
            sz += comp_bytes(3 if poscnt else 2, postyp)  # 1->XYZ(3),0->XY(2)
    # NRM (bits 11-12)
    nrm = (vcd_lo >> 11) & 3
    if nrm:
        if nrm >= 2: sz += (nrm - 1)
        else:
            nrmtyp = (a >> 13) & 7
            sz += comp_bytes(3, nrmtyp)
    # CLR0 (bits 13-14), CLR1 (bits 15-16)
    for ci, shift, cntbit, typbit in [(0, 13, 4, 5), (1, 15, 8, 9)]:
        clr = (vcd_lo >> shift) & 3
        if clr:
            if clr >= 2: sz += (clr - 1)
            else:
                ctyp = (a >> typbit) & 7
                # rgb565/rgba8 etc: 16 or 32 bit; approximate by type
                sz += {0:2,1:3,2:4,3:2,4:3,5:4}.get(ctyp,4)
    # TEX0..7 (vcd_hi 2 bits each)
    for t in range(8):
        tx = (vcd_hi >> (t * 2)) & 3
        if tx:
            if tx >= 2: sz += (tx - 1)
            else:
                # tex coord comp/type live in VAT a/b/c; approximate f32 ST=8
                sz += 8
    return sz

# ---- XF position matrix memory (addr 0x000..0x0FF = 256 floats = pos mtx) ---
xfmem = [0.0] * 0x100
loaded_slots = {}   # slot(addr/4 over 0..63) -> 3x4 matrix, since last draw

def read_mtx(slot_addr):
    base = slot_addr  # in floats
    m = []
    for r in range(3):
        row = xfmem[base + r * 4: base + r * 4 + 4]
        m.append(row)
    return m

draws = []
p = 0
TARGET = {17, 84, 522, 352, 9, 99, 388, 100, 19, 5, 16, 39}

while p < n:
    op = fifo[p]; p += 1
    if op == 0x00:
        continue
    elif op == 0x08:  # CP load: u8 addr, u32 val
        addr = fifo[p]; val, = struct.unpack_from(">I", fifo, p + 1); p += 5
        if addr == 0x50: vcd_lo = val
        elif addr == 0x60: vcd_hi = val
        elif 0x70 <= addr <= 0x77: vat_a[addr & 7] = val
        elif 0x80 <= addr <= 0x87: vat_b[addr & 7] = val
        elif 0x90 <= addr <= 0x97: vat_c[addr & 7] = val
    elif op == 0x10:  # XF load: u32 (count-1<<16 | addr16), then count*4 bytes
        hdr, = struct.unpack_from(">I", fifo, p); p += 4
        cnt = ((hdr >> 16) & 0xF) + 1
        addr = hdr & 0xFFFF
        vals = struct.unpack_from(">%df" % cnt, fifo, p); p += cnt * 4
        if addr + cnt <= 0x100:  # position-matrix memory
            for i in range(cnt):
                xfmem[addr + i] = vals[i]
            # record the matrix slot (addr is multiple of 4 for a row-start)
            base = (addr // 4) * 4
            loaded_slots[base] = True
    elif op == 0x61:  # BP load: u32
        p += 4
    elif op in (0x20, 0x28, 0x30, 0x38):  # INDX load A/B/C/D: u32
        p += 4
    elif 0x80 <= op <= 0xBF:  # DRAW: primitive|vat ; u16 count ; vtx data
        vat = op & 7
        cnt, = struct.unpack_from(">H", fifo, p); p += 2
        vs = vtx_size(vat)
        if cnt and vs:
            mats = {s: read_mtx(s) for s in sorted(loaded_slots)}
            draws.append((op & 0xF8, cnt, vs, mats))
            loaded_slots = {}
            p += cnt * vs
        else:
            # can't size; bail to avoid desync
            break
    else:
        # unknown opcode -> stop (avoid runaway)
        break

print(f"parsed {len(draws)} draws (stopped at byte {p}/{n})")
print("=== draws with Wes-like vertex counts ===")
for i, (prim, cnt, vs, mats) in enumerate(draws):
    if cnt in TARGET:
        print(f"draw[{i}] prim=0x{prim:02x} verts={cnt} vtxsize={vs} matsLoaded={len(mats)}")
        for s, m in list(mats.items())[:12]:
            t = (m[0][3], m[1][3], m[2][3])
            print(f"    posmtx@xf{s:3d} (slot {s//4}): translate=({t[0]:.2f},{t[1]:.2f},{t[2]:.2f})")
print("=== vertex-count histogram of all draws ===")
from collections import Counter
h = Counter(c for _, c, _, _ in draws)
for c, k in sorted(h.items()):
    print(f"  verts={c}: {k} draws")
