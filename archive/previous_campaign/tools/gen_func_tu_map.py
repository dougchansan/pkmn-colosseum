#!/usr/bin/env python3
"""gen_func_tu_map.py - materialize the function -> source-TU map.

The data already exists implicitly in config/GC6E01/:
  - symbols.txt        every symbol: `name = .sec:0xADDR; // type:function size:0xN`
  - splits_refined.txt address-range -> src file (`STATUS start end src cnt # note`)

This joins them into a single per-function lookup so downstream tools
(process_ghidra_output.py, a future per-TU Ghidra import, tu_split work)
can group functions by their real translation unit instead of dumping
everything into __unassigned__.

Output: config/GC6E01/func_tu_map.json
  { "fn_800055E0": {"addr":"0x800055E0","size":"0x...","section":".text",
                     "src":"src/game/main.c","status":"KNOWN"}, ... }

GAP ranges yield src=null, status="GAP" (still positioned by address so a
later pass can attribute them). Functions outside every split range are
tagged by their section (e.g. .init) so they are never silently lost.

Usage:
    python tools/gen_func_tu_map.py            # write the map + summary
    python tools/gen_func_tu_map.py --check     # summary only, no write
"""

import argparse
import bisect
import json
import re
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CFG = ROOT / "config" / "GC6E01"
SYMBOLS = CFG / "symbols.txt"
SPLITS = CFG / "splits_refined.txt"
OUT = CFG / "func_tu_map.json"

SYM_RE = re.compile(
    r"^(?P<name>\S+)\s*=\s*\.(?P<sec>\w+):0x(?P<addr>[0-9A-Fa-f]+);"
    r".*?type:function(?:.*?size:0x(?P<size>[0-9A-Fa-f]+))?"
)
SPLIT_RE = re.compile(
    r"^(?P<status>KNOWN|LIKELY|GAP)\s+0x(?P<start>[0-9A-Fa-f]+)\s+"
    r"0x(?P<end>[0-9A-Fa-f]+)\s+(?P<src>\S+)"
)


def parse_symbols():
    fns = []
    for ln in SYMBOLS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SYM_RE.match(ln.strip())
        if not m:
            continue
        fns.append({
            "name": m.group("name"),
            "addr": int(m.group("addr"), 16),
            "section": m.group("sec"),
            "size": int(m.group("size"), 16) if m.group("size") else 0,
        })
    fns.sort(key=lambda f: f["addr"])
    return fns


def parse_splits():
    ranges = []
    for ln in SPLITS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SPLIT_RE.match(ln.strip())
        if not m:
            continue
        src = m.group("src")
        ranges.append({
            "start": int(m.group("start"), 16),
            "end": int(m.group("end"), 16),
            "src": None if (m.group("status") == "GAP" or
                            src.strip("?") == "") else src,
            "status": m.group("status"),
        })
    ranges.sort(key=lambda r: r["start"])
    return ranges


def build():
    fns = parse_symbols()
    ranges = parse_splits()
    starts = [r["start"] for r in ranges]

    out = {}
    for f in fns:
        a = f["addr"]
        i = bisect.bisect_right(starts, a) - 1
        src, status = None, "UNMAPPED"
        if 0 <= i < len(ranges) and ranges[i]["start"] <= a < ranges[i]["end"]:
            src = ranges[i]["src"]
            status = ranges[i]["status"]
        elif f["section"] != "text":
            status = f"SECTION:{f['section']}"
        out[f["name"]] = {
            "addr": f"0x{a:08X}",
            "size": f"0x{f['size']:X}",
            "section": f".{f['section']}",
            "src": src,
            "status": status,
        }
    return out, ranges


_RANGES_CACHE = None


def tu_for_address(addr):
    """Return src_path for a .text address using splits_refined ranges,
    or None if it falls in a GAP / outside every range. Importable by
    process_ghidra_output.py so Ghidra functions group by real TU instead
    of __unassigned__. Cached after first call."""
    global _RANGES_CACHE
    if _RANGES_CACHE is None:
        rs = parse_splits()
        _RANGES_CACHE = (rs, [r["start"] for r in rs])
    ranges, starts = _RANGES_CACHE
    i = bisect.bisect_right(starts, addr) - 1
    if 0 <= i < len(ranges) and ranges[i]["start"] <= addr < ranges[i]["end"]:
        return ranges[i]["src"]
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true",
                    help="print summary only, do not write the map")
    args = ap.parse_args()

    if not SYMBOLS.exists() or not SPLITS.exists():
        raise SystemExit(f"missing {SYMBOLS} or {SPLITS}")

    fmap, ranges = build()

    by_status = Counter(v["status"].split(":")[0] for v in fmap.values())
    by_src = Counter(v["src"] for v in fmap.values() if v["src"])
    mapped = sum(1 for v in fmap.values() if v["src"])
    total = len(fmap)

    print(f"[tu-map] {total} functions, {len(ranges)} split ranges")
    print(f"[tu-map] {mapped} attributed to a source TU "
          f"({100.0*mapped/total:.1f}%), {total-mapped} unattributed")
    for st, n in by_status.most_common():
        print(f"   {st:<12} {n}")
    print(f"[tu-map] {len(by_src)} distinct source files; top 8 by fn count:")
    for src, n in by_src.most_common(8):
        print(f"   {n:>4}  {src}")

    if not args.check:
        OUT.write_text(json.dumps(fmap, indent=1, sort_keys=True),
                        encoding="utf-8")
        print(f"[tu-map] wrote {OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
