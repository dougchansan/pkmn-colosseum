#!/usr/bin/env python3
"""Find + parse the WZX collision mesh inside a field-map .fsys archive.

Floor FSYS archives contain typed resources (GFL geometry, WZX collision,
PKX, Tex, Camera, Map, Script...). The WZX format (per include/game/gs_colsys.h):

  header  0x00: u32 vertexDataOffset  (self-relative; relocated to absolute)
          0x04: u32 triangleCount
  tris    each record = 0x40 bytes:
            0x00: Vec3f v0
            0x0C: Vec3f v1
            0x18: Vec3f v2
            0x24..0x3F: attributes (surface type, normal, debug color,
                        child-mesh pointers)

We identify the WZX member by content signature (no name dependency): a raw
member whose header {off, count} satisfies 0 < off < size and
off + count*0x40 <= size, count plausible, and whose vertices are sane floats.
Then we dump triangle count + XZ/Y world bounds so they can be sanity-checked
against the visible room geometry.

Usage:  python tools/pcport_probe/probe_wzx.py [D1_garage_1F]
"""
import os, sys, struct
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from logo_decode import parse_fsys, classify, be32

FILES = r"C:\Users\douglaswhittingham\pkmn-colosseum\orig\GC6E01\disc\files"


def f32be(b, o):
    return struct.unpack_from(">f", b, o)[0]


def looks_like_wzx(raw):
    """Return (triCount, vertOff) if raw plausibly is a WZX mesh, else None."""
    if raw is None or len(raw) < 8 + 0x40:
        return None
    n = len(raw)
    vert_off = be32(raw, 0)
    tri_count = be32(raw, 4)
    if not (0 < vert_off < n) or not (0 < tri_count < 200000):
        return None
    need = vert_off + tri_count * 0x40
    # allow a little trailing slack/alignment
    if need > n or need < n - 0x800:
        return None
    # sanity-check the first few triangles' verts are finite, bounded floats
    import math
    for t in range(min(tri_count, 8)):
        base = vert_off + t * 0x40
        for k in range(9):  # 3 verts * 3 comps
            v = f32be(raw, base + k * 4)
            if not math.isfinite(v) or abs(v) > 1e6:
                return None
    return (tri_count, vert_off)


def mesh_bounds(raw, tri_count, vert_off):
    import math
    lo = [math.inf] * 3
    hi = [-math.inf] * 3
    for t in range(tri_count):
        base = vert_off + t * 0x40
        for vtx in range(3):
            for c in range(3):
                v = f32be(raw, base + vtx * 0x0C + c * 4)
                lo[c] = min(lo[c], v)
                hi[c] = max(hi[c], v)
    return lo, hi


def main():
    stem = sys.argv[1] if len(sys.argv) > 1 else "D1_garage_1F"
    path = os.path.join(FILES, stem + ".fsys")
    data = open(path, "rb").read()
    print(f"== {stem}.fsys ({len(data)} bytes) ==")
    wzx_hits = []
    for mem in parse_fsys(data):
        raw = mem["raw"]
        kind = classify(raw)
        rsz = len(raw) if raw is not None else 0
        sig = looks_like_wzx(raw)
        tag = ""
        if sig:
            tag = f"  <<< WZX? tris={sig[0]} vertOff=0x{sig[1]:X}"
            wzx_hits.append((mem["name"], raw, sig))
        print(f"  {mem['name']:<24} kind={kind:<13} rawsz=0x{rsz:<7X}{tag}")

    print(f"\n-- {len(wzx_hits)} WZX candidate(s) --")
    for name, raw, (tri_count, vert_off) in wzx_hits:
        lo, hi = mesh_bounds(raw, tri_count, vert_off)
        print(f"\n[{name}] {tri_count} triangles, vertOff=0x{vert_off:X}")
        print(f"  X: {lo[0]:9.2f} .. {hi[0]:9.2f}  (span {hi[0]-lo[0]:.2f})")
        print(f"  Y: {lo[1]:9.2f} .. {hi[1]:9.2f}  (span {hi[1]-lo[1]:.2f})")
        print(f"  Z: {lo[2]:9.2f} .. {hi[2]:9.2f}  (span {hi[2]-lo[2]:.2f})")
        # peek at first triangle attribute words
        base = vert_off
        attrs = [be32(raw, base + 0x24 + i * 4) for i in range(7)]
        print(f"  tri0 attr words: " + " ".join(f"0x{a:08X}" for a in attrs))


if __name__ == "__main__":
    main()
