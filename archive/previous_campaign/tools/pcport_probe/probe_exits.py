#!/usr/bin/env python3
"""Hunt for the per-room exit/door trigger records in a field-map .fsys.

The RE'd exit record (from the "people" subsystem accessor fns, see the
MAP WARPS scope plan) is claimed to be 0x2C bytes:
  +0x00 u8   active flag
  +0x01..08  s8 target floor IDs per state
  +0x09 s8   exit direction/type
  +0x0C u32  associated-data match key (pointer)
  +0x14 f32  approach distance/radius
  +0x1C f32  half-angle-width   (* sdata2 const)
  +0x20 f32  half-angle-height  (* sdata2 const)

These offsets are RE-derived and need confirming against real bytes.  We scan
every member of the archive for a window that *looks like* one or more such
records: a small active byte, small signed target IDs, and three plausible
float fields (positive, finite, room-scale) at +0x14/+0x1C/+0x20, repeated at
stride 0x2C.  We also dump candidate windows in both BE and LE for the floats
since endianness of the scene_data is uncertain.

Usage: python tools/pcport_probe/probe_exits.py [D1_garage_1F] [more maps...]
"""
import os, sys, struct, math
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
# logo_decode.py lives in the main checkout's probe dir (untracked helper).
sys.path.insert(0, r"C:\Users\douglaswhittingham\pkmn-colosseum\tools\pcport_probe")
from logo_decode import parse_fsys, classify, be32

FILES = r"C:\Users\douglaswhittingham\pkmn-colosseum\orig\GC6E01\disc\files"


def f32(b, o, le):
    fmt = "<f" if le else ">f"
    if o + 4 > len(b):
        return None
    return struct.unpack_from(fmt, b, o)[0]


def plausible_radius(v):
    return v is not None and math.isfinite(v) and 0.5 < v < 5000.0


def plausible_angle(v):
    # half-angle in radians-ish or a cosine factor; allow a generous band
    return v is not None and math.isfinite(v) and 0.0 < v < 1.0e4


def scan_member(name, raw):
    if raw is None or len(raw) < 0x2C:
        return []
    n = len(raw)
    hits = []
    for le in (False, True):
        base = 0
        while base + 0x2C <= n:
            r14 = f32(raw, base + 0x14, le)
            r1c = f32(raw, base + 0x1C, le)
            r20 = f32(raw, base + 0x20, le)
            active = raw[base + 0x00]
            dirb = raw[base + 0x09]
            tgt0 = raw[base + 0x01]
            ok = (plausible_radius(r14) and plausible_angle(r1c)
                  and plausible_angle(r20) and active in (0, 1)
                  and tgt0 < 0xC0)
            if ok:
                count = 0
                p = base
                while p + 0x2C <= n:
                    rr14 = f32(raw, p + 0x14, le)
                    rr1c = f32(raw, p + 0x1C, le)
                    rr20 = f32(raw, p + 0x20, le)
                    aa = raw[p + 0x00]
                    if (plausible_radius(rr14) and plausible_angle(rr1c)
                            and plausible_angle(rr20) and aa in (0, 1)):
                        count += 1
                        p += 0x2C
                    else:
                        break
                if count >= 1:
                    hits.append((base, count, le, r14, r1c, r20,
                                 active, dirb, tgt0))
                base = p
            else:
                base += 4
    return hits


def dump_record(raw, off, le):
    fmt = "<f" if le else ">f"
    ifmt = "<I" if le else ">I"
    def F(o):
        return struct.unpack_from(fmt, raw, off + o)[0]
    mk = struct.unpack_from(ifmt, raw, off + 0x0C)[0]
    print(f"    rec@0x{off:X} ({'LE' if le else 'BE'}):")
    print(f"      active=0x{raw[off+0]:02X} targets={[raw[off+1+i] for i in range(8)]} "
          f"dir=0x{raw[off+9]:02X} matchKey=0x{mk:08X}")
    print(f"      f@0x14={F(0x14):.3f}  f@0x1C={F(0x1C):.3f}  f@0x20={F(0x20):.3f}")
    print(f"      raw: " + " ".join(f"{raw[off+i]:02X}" for i in range(0x2C)))


def main():
    stems = sys.argv[1:] or ["D1_garage_1F"]
    for stem in stems:
        path = os.path.join(FILES, stem + ".fsys")
        if not os.path.exists(path):
            print(f"!! missing {path}")
            continue
        data = open(path, "rb").read()
        print(f"\n=========== {stem}.fsys ({len(data)} bytes) ===========")
        for mem in parse_fsys(data):
            raw = mem["raw"]
            kind = classify(raw)
            rsz = len(raw) if raw is not None else 0
            hits = scan_member(mem["name"], raw)
            note = ""
            if hits:
                note = f"  <<< {len(hits)} exit-rec candidate window(s)"
            print(f"  {mem['name']:<26} kind={kind:<12} sz=0x{rsz:<7X}{note}")
            shown = 0
            for (base, count, le, r14, r1c, r20, active, dirb, tgt0) in \
                    sorted(hits, key=lambda h: -h[1]):
                if shown >= 4:
                    break
                print(f"      window @0x{base:X} count={count} "
                      f"({'LE' if le else 'BE'}) r14={r14:.2f} r1c={r1c:.2f} r20={r20:.2f}")
                dump_record(raw, base, le)
                shown += 1


if __name__ == "__main__":
    main()
