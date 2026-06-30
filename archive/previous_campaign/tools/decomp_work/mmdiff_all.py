#!/usr/bin/env python3
"""Show oriented target-vs-ours diffs for several functions in menu_middle."""
import os
import sys, json, itertools, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from headless_subprocess import run as hrun
TARGET = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
BASE = ROOT / "build/GC6E01/base/game/menu/menu_middle.o"
CLI = ROOT / ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli")

fns = sys.argv[1:]
for fn in fns:
    r = hrun([str(CLI), "diff", "-1", str(TARGET), "-2", str(BASE),
                        "-o", "-", "--format", "json",
                        "-c", "ppc.calculatePoolRelocations=false", fn],
                       capture_output=True, text=True, cwd=str(ROOT))
    try:
        d = json.loads(r.stdout)
    except Exception:
        print(f"== {fn} == FAILED: {r.stderr[:200]}")
        continue

    def get(side):
        for s in d[side]["symbols"]:
            if s.get("name") == fn:
                return s
        return None

    L = get("left"); R = get("right")
    if not L or not R:
        print(f"== {fn} == missing L={L is not None} R={R is not None}")
        continue
    li = L["instructions"]; ri = R["instructions"]
    print(f"\n===== {fn} =====  match%={R.get('match_percent')}")
    for a, b in itertools.zip_longest(li, ri):
        af = (a or {}).get("instruction", {}).get("formatted", "") if a else ""
        bf = (b or {}).get("instruction", {}).get("formatted", "") if b else ""
        k = (a or {}).get("diff_kind") or (b or {}).get("diff_kind") or ""
        mark = "" if k in ("", "DIFF_NONE") else "  <== " + k
        if mark:
            print(f"{af:36} | {bf:36}{mark}")
        else:
            print(f"{af:36} | {bf:36}")
