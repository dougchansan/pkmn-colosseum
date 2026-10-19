#!/usr/bin/env python3
"""Emit a guest-function table (address,size) for a co-op DOL.

The recomp's `patches/GC6E01-functions.csv` lists retail addresses and sizes.
A co-op build keeps `.text` the same total size but still moves individual
symbols: the first edited function grows and everything after it shifts, so
heroReadStick is 0x434 rather than 0x430 and moveLeader starts at 0x8012E7BC
rather than 0x8012E7B8.

`scripts/make-hook-aware-module.py` resolves each hooked address to a function
range through that table, so a mod that hooks a moved function needs a table
built from the co-op ELF instead:

    python tools/coop/gen_functions_csv.py --coop-elf build-coop/GC6E01/main.elf \\
        > build-coop/GC6E01/functions.csv
"""

from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SHT_SYMTAB = 2
STT_FUNC = 2


def functions(path: Path) -> dict[int, int]:
    data = path.read_bytes()
    if data[:4] != b"\x7fELF":
        raise SystemExit(f"{path}: not an ELF")
    (section_offset,) = struct.unpack_from(">I", data, 0x20)
    entry_size, count = struct.unpack_from(">HH", data, 0x2E)

    sections = []
    for index in range(count):
        offset = section_offset + index * entry_size
        fields = struct.unpack_from(">10I", data, offset)
        sections.append(
            {"type": fields[1], "offset": fields[4], "size": fields[5],
             "link": fields[6], "entsize": fields[9]}
        )

    out: dict[int, int] = {}
    for section in sections:
        if section["type"] != SHT_SYMTAB or not section["entsize"]:
            continue
        for index in range(section["size"] // section["entsize"]):
            offset = section["offset"] + index * section["entsize"]
            _, value, size, info, _, _ = struct.unpack_from(">IIIBBH", data, offset)
            if (info & 0xF) != STT_FUNC or not size or not value:
                continue
            # A symbol may be listed more than once; the sizes agree.
            out[value] = size
    return out


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--coop-elf", type=Path,
                    default=ROOT / "build-coop/GC6E01/main.elf")
    ap.add_argument("--base", type=Path,
                    help="the recomp's retail functions CSV to merge over; the"
                         " co-op ELF wins where both name an address")
    args = ap.parse_args()

    table = functions(args.coop_elf)
    if not table:
        raise SystemExit(f"{args.coop_elf}: no sized function symbols")

    # The ELF only carries sized STT_FUNC symbols, which is fewer entries than
    # the decomp's symbol table: other mods hook addresses that are functions
    # in the retail table and absent here. Merge rather than replace, so a
    # co-op table is a superset and never loses a hookable address.
    if args.base:
        text = args.base.read_text(encoding="utf-8-sig").replace("\r\n", "\n")
        base: dict[int, int] = {}
        for line in text.splitlines():
            line = line.strip()
            if not line or line.startswith("#") or line.startswith("address"):
                continue
            address, _, size = line.partition(",")
            base[int(address, 16)] = int(size, 16)
        base.update(table)
        table = base

    lines = [
        "# GC6E01 guest functions: start address, size (hex), from the co-op",
        f"# build's ELF ({args.coop_elf.name})"
        + (f" merged over {args.base.name}" if args.base else "") + ". Read by",
        "# scripts/make-hook-aware-module.py.",
        "address,size",
    ]
    lines += [f"{address:08X},{size:X}" for address, size in sorted(table.items())]
    sys.stdout.write("\n".join(lines) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
