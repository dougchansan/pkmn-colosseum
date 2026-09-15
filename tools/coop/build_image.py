#!/usr/bin/env python3
"""Build a bootable co-op ISO from the original image plus a patched DOL.

Why the DOL is relocated rather than replaced in place:

The disc reserves exactly `fst_offset - dol_offset` bytes for the executable.
On this image that is 0x39AD00, and the original DOL is 0x39ACE0 — only 32
bytes of slack. A patched DOL carrying an extra loaded section is larger than
that, so splicing it at the original offset would run into the FST, and a
loader that clamps the DOL to its on-disc extent silently drops the appended
section (observed: the co-op payload was absent from memory while the patched
text1 bytes were present).

So instead the patched DOL is written to a fresh 32 KB-aligned offset past the
existing data and `boot.bin`'s dol_offset (disc offset 0x420) is repointed at
it. Nothing else on the disc moves, so the FST and every file offset stay
valid.

Note this makes the image larger than a retail disc. That is fine for Dolphin,
which reads the image as an ordinary file, but the result is a local test image
and not a faithful disc reproduction. Both LAN machines must use the identical
file.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ALIGN = 0x8000
BOOT_DOL_OFFSET_FIELD = 0x420


def sha1_file(path: Path, limit: int | None = None) -> str:
    h = hashlib.sha1()
    with path.open("rb") as f:
        remaining = limit
        while True:
            chunk = f.read(1 << 20)
            if not chunk:
                break
            if remaining is not None:
                if remaining <= 0:
                    break
                chunk = chunk[:remaining]
                remaining -= len(chunk)
            h.update(chunk)
    return h.hexdigest()


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--iso", type=Path, required=True, help="original, untouched game image"
    )
    ap.add_argument("--dol", type=Path, default=ROOT / "build/coop/main.dol")
    ap.add_argument("--out", type=Path, default=ROOT / "build/coop/coop.iso")
    ap.add_argument(
        "--manifest", type=Path, default=ROOT / "build/coop/image.manifest.json"
    )
    args = ap.parse_args()

    for p in (args.iso, args.dol):
        if not p.is_file():
            sys.exit(f"missing input: {p}")

    with args.iso.open("rb") as f:
        f.seek(BOOT_DOL_OFFSET_FIELD)
        old_dol_off, fst_off, fst_size, _ = struct.unpack(">IIII", f.read(16))

    dol_bytes = args.dol.read_bytes()
    orig_size = args.iso.stat().st_size
    extent = fst_off - old_dol_off

    print(f"original image   {args.iso}  ({orig_size} bytes)")
    print(f"  dol_offset     0x{old_dol_off:08X}")
    print(f"  fst_offset     0x{fst_off:08X}")
    print(f"  dol extent     0x{extent:X} ({extent} bytes)")
    print(f"patched dol      {len(dol_bytes)} bytes")
    # Preferred path: the patched DOL still fits the space the disc reserves
    # for it, so it can be spliced at the original offset. Nothing else on the
    # disc moves and the image stays exactly retail-sized.
    in_place = len(dol_bytes) <= extent
    new_dol_off = (
        old_dol_off if in_place else (orig_size + ALIGN - 1) & ~(ALIGN - 1)
    )
    print(f"  placement      {'in place' if in_place else 'relocated'}")

    args.out.parent.mkdir(parents=True, exist_ok=True)
    print(f"copying image -> {args.out} ...")
    shutil.copyfile(args.iso, args.out)

    with args.out.open("r+b") as f:
        f.seek(new_dol_off)
        f.write(dol_bytes)
        if not in_place:
            pad = (-len(dol_bytes)) % ALIGN
            if pad:
                f.write(b"\x00" * pad)
            f.seek(BOOT_DOL_OFFSET_FIELD)
            f.write(struct.pack(">I", new_dol_off))

    # Read back and verify rather than trusting the writes.
    with args.out.open("rb") as f:
        f.seek(BOOT_DOL_OFFSET_FIELD)
        (check_off,) = struct.unpack(">I", f.read(4))
        f.seek(check_off)
        check_dol = f.read(len(dol_bytes))
    if check_off != new_dol_off:
        sys.exit("verification failed: dol_offset did not take")
    if check_dol != dol_bytes:
        sys.exit("verification failed: DOL bytes at the new offset differ")

    out_size = args.out.stat().st_size
    manifest = {
        "original_image": {
            "path": str(args.iso),
            "size": orig_size,
            "sha1": sha1_file(args.iso),
        },
        "patched_dol": {
            "path": str(args.dol),
            "size": len(dol_bytes),
            "sha1": hashlib.sha1(dol_bytes).hexdigest(),
        },
        "output_image": {
            "path": str(args.out),
            "size": out_size,
            "sha1": sha1_file(args.out),
        },
        "relocation": {
            "old_dol_offset": f"0x{old_dol_off:08X}",
            "new_dol_offset": f"0x{new_dol_off:08X}",
            "original_dol_extent": f"0x{extent:X}",
            "fst_offset": f"0x{fst_off:08X}",
            "fst_size": f"0x{fst_size:08X}",
        },
    }
    args.manifest.write_text(json.dumps(manifest, indent=2) + "\n")

    print(f"  new dol_offset 0x{new_dol_off:08X}  (verified)")
    print(f"output image     {args.out}  ({out_size} bytes)")
    print(f"  sha1           {manifest['output_image']['sha1']}")
    print(f"manifest         {args.manifest}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
