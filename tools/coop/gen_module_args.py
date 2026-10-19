#!/usr/bin/env python3
"""Emit moderngekko-port build arguments for a co-op DOL.

The recomp's patch manifests are keyed to retail addresses. A co-op DOL shifts
every symbol at or after the edit point, and the shift is NOT uniform (observed
+192 for most symbols but +196 for HSD_MtxScaledAdd, because per-function
alignment padding differs). So the mapping has to go through the symbol tables
rather than arithmetic.

For each manifest entry this resolves retail address -> symbol name -> co-op
address by diffing the matching build's main.elf against the co-op build's.

    python tools/coop/gen_module_args.py --recomp-root <path> > args.txt
"""

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from elfread import Elf32BE  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--recomp-root", type=Path, required=True)
    ap.add_argument("--retail-elf", type=Path, default=ROOT / "build/GC6E01/main.elf")
    ap.add_argument("--coop-elf", type=Path, default=ROOT / "build-coop/GC6E01/main.elf")
    args = ap.parse_args()

    retail = Elf32BE.load(args.retail_elf).symbols()
    coop = Elf32BE.load(args.coop_elf).symbols()
    by_addr: dict[int, list[str]] = {}
    for name, addr in retail.items():
        by_addr.setdefault(addr, []).append(name)

    out: list[str] = []

    ws = args.recomp_root / "patches/GC6E01-native-widescreen.csv"
    text = ws.read_text(encoding="utf-8-sig").replace("\r\n", "\n")
    for row in csv.DictReader(text.splitlines()):
        addr = int(row["address"], 16)
        # These all sit below the edit point; assert rather than assume.
        names = by_addr.get(addr, [])
        moved = [n for n in names if n in coop and coop[n] != addr]
        if moved:
            print(
                f"warning: widescreen patch {row['address']} targets moved symbol "
                f"{moved[0]}",
                file=sys.stderr,
            )
        out += ["--dol-patch", f"{row['address']}={row['replacement']}"]

    ns = args.recomp_root / "patches/GC6E01-native-substitutions.csv"
    text = ns.read_text(encoding="utf-8-sig").replace("\r\n", "\n")
    shifted = 0
    for row in csv.DictReader(text.splitlines()):
        addr = int(row["guest"], 16)
        names = by_addr.get(addr, [])
        new = None
        for n in names:
            if n in coop:
                new = coop[n]
                break
        if new is None:
            print(
                f"warning: no symbol for substitution {row['guest']} "
                f"({row['symbol']}); keeping retail address",
                file=sys.stderr,
            )
            new = addr
        if new != addr:
            shifted += 1
            print(
                f"remapped {row['guest']} -> {new:08X}  {names[0]} "
                f"({new - addr:+d})",
                file=sys.stderr,
            )
        out += ["--native-subst", f"{new:08X}:{row['symbol']}"]

    print(f"{shifted} substitution(s) remapped", file=sys.stderr)
    sys.stdout.write("\n".join(out) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
