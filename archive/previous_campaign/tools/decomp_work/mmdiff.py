#!/usr/bin/env python3
"""Show oriented target-vs-ours diff for one function in menu_middle."""
import os
import sys, json, itertools, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
TARGET = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
BASE = ROOT / "build/GC6E01/base/game/menu/menu_middle.o"
CLI = ROOT / ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli")

fn = sys.argv[1]
r = subprocess.run([str(CLI), "diff", "-1", str(TARGET), "-2", str(BASE),
                    "-o", "-", "--format", "json",
                    "-c", "ppc.calculatePoolRelocations=false", fn],
                   capture_output=True, text=True, cwd=str(ROOT))
d = json.loads(r.stdout)

def get(side):
    for s in d[side]["symbols"]:
        if s.get("name") == fn:
            return s
    return None

L = get("left"); R = get("right")
if not L or not R:
    print("missing", L is not None, R is not None); sys.exit(1)
li = L["instructions"]; ri = R["instructions"]
print(f"== {fn} ==  target(left) match%={R.get('match_percent')}")
for a, b in itertools.zip_longest(li, ri):
    af = (a or {}).get("instruction", {}).get("formatted", "") if a else ""
    bf = (b or {}).get("instruction", {}).get("formatted", "") if b else ""
    k = (a or {}).get("diff_kind") or (b or {}).get("diff_kind") or ""
    mark = "" if k in ("", "DIFF_NONE") else "  <== " + k
    print(f"{af:36} | {bf:36}{mark}")
