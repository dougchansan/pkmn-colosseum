import os
import json, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CLI = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
ORIG = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
BASE = ROOT / "build/GC6E01/base/dolphin/os/OSCache.o"

fn = sys.argv[1]
out = subprocess.run(
    [str(CLI), "diff", "-1", str(ORIG), "-2", str(BASE), "-o", "-",
     "--format", "json", "-c", "ppc.calculatePoolRelocations=false", fn],
    cwd=str(ROOT), capture_output=True, text=True)
if out.returncode != 0:
    print("objdiff failed:", out.stderr[:500]); sys.exit(1)
j = json.loads(out.stdout)

def by_name(side, name):
    for s in j[side]["symbols"]:
        if s.get("name") == name:
            return s
    return None

rs = by_name("right", fn)
ls = by_name("left", fn)
print("right match=", rs.get("match_percent") if rs else None,
      " left_found=", ls is not None)
li = (ls or {}).get("instructions", [])
ri = (rs or {}).get("instructions", [])
n = max(len(li), len(ri))
nmm = 0
for idx in range(n):
    a = li[idx] if idx < len(li) else {}
    b = ri[idx] if idx < len(ri) else {}
    k = a.get("diff_kind") or b.get("diff_kind") or ""
    af = (a.get("instruction") or {}).get("formatted") or ""
    bf = (b.get("instruction") or {}).get("formatted") or ""
    bad = k not in ("", "DIFF_NONE")
    if bad: nmm += 1
    mark = "*" if bad else " "
    print("%s %3d  %-34s | %-34s  %s" % (mark, idx, af, bf, k if bad else ""))
print("RIGHT-side mismatch rows:", nmm)
