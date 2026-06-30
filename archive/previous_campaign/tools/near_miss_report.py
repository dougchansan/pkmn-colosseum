#!/usr/bin/env python3
"""List functions at 80-99.99% match, sorted by file. Easy-win candidates."""
import os
import json
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE = ROOT / "build" / "GC6E01" / "base"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
sys.path.insert(0, str(ROOT / "tools"))
from headless_subprocess import run as run_tool  # noqa: E402

near = defaultdict(list)
for o in sorted(BASE.rglob("*.o")):
    rel = o.relative_to(BASE).as_posix()
    r = run_tool(
        [str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(o), "-o", "-",
         "--format", "json", "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True,
    )
    if r.returncode != 0:
        continue
    j = json.loads(r.stdout)
    for s in j.get("right", {}).get("symbols", []):
        if s.get("kind") != "SYMBOL_FUNCTION":
            continue
        n = s.get("name", "")
        if not n.startswith("fn_"):
            continue
        pct = s.get("match_percent")
        if pct is None or pct >= 100 or pct < 80:
            continue
        near[rel].append((pct, n))

ranked = sorted(near.items(), key=lambda kv: -len(kv[1]))
for rel, fns in ranked:
    if len(fns) < 3:
        continue
    print(f"=== {rel} ({len(fns)} near-misses) ===")
    for pct, name in sorted(fns, key=lambda x: -x[0])[:8]:
        print(f"  {pct:6.2f}%  {name}")
