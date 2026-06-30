#!/usr/bin/env python3
"""List LARGE functions at a near-miss match% (the best permuter/manual targets).

Objdiffs every built base .o against the monolithic target text object, collects
(fn, size_bytes, match%), filters large + near, sorts by % desc.

Usage: python tools/decomp_work/near_large.py [min_pct] [max_pct] [min_bytes]
  defaults: 88 99.99 0xC0  (>=192 bytes ~48 instr, 88%<=pct<100%)
"""
import os
import json
import subprocess
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE = ROOT / "build" / "GC6E01" / "base"

min_pct = float(sys.argv[1]) if len(sys.argv) > 1 else 88.0
max_pct = float(sys.argv[2]) if len(sys.argv) > 2 else 99.99
min_bytes = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0xC0

rows = {}
objs = sorted(BASE.rglob("*.o"))
for i, o in enumerate(objs):
    try:
        p = subprocess.run(
            [str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(o), "-o", "-",
             "--format", "json", "-c", "ppc.calculatePoolRelocations=false"],
            capture_output=True, text=True, timeout=60)
        d = json.loads(p.stdout)
    except Exception:
        continue
    tu = o.relative_to(BASE).as_posix()

    def walk(x):
        if isinstance(x, dict):
            n = x.get('name', '')
            if n.startswith('fn_') and 'match_percent' in x and 'size' in x:
                try:
                    sz = int(str(x['size']), 0) if not str(x['size']).isdigit() else int(x['size'])
                except Exception:
                    sz = 0
                mp = x['match_percent']
                # keep the best (highest) sighting per fn
                if n not in rows or mp > rows[n][1]:
                    rows[n] = (sz, mp, tu)
            for v in x.values():
                walk(v)
        elif isinstance(x, list):
            for v in x:
                walk(v)
    walk(d)

hits = [(n, sz, mp, tu) for n, (sz, mp, tu) in rows.items()
        if sz >= min_bytes and min_pct <= mp < max_pct]
hits.sort(key=lambda r: -r[2])
print(f"LARGE near-miss functions (>= {min_bytes} bytes, {min_pct}% <= match < {max_pct}%): {len(hits)}")
print(f"{'match%':>7}  {'bytes':>5}  {'fn':<14} TU")
for n, sz, mp, tu in hits[:60]:
    print(f"{mp:7.2f}  {sz:5d}  {n:<14} {tu}")
