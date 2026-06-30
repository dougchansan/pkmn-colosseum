import json, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FULL = ROOT / "tools/decomp_work/oscache_full.json"
j = json.load(open(FULL, encoding="utf-8"))
fn = sys.argv[1]

def find(side):
    for s in j[side]["symbols"]:
        if s.get("name") == fn:
            return s
    return None

ls = find("left")   # target
rs = find("right")  # mine
print("match=", rs.get("match_percent") if rs else None,
      " target_found=", ls is not None)
li = (ls or {}).get("instructions", [])
ri = (rs or {}).get("instructions", [])
n = max(len(li), len(ri))
for idx in range(n):
    a = li[idx] if idx < len(li) else {}
    b = ri[idx] if idx < len(ri) else {}
    k = a.get("diff_kind") or b.get("diff_kind") or ""
    af = (a.get("instruction") or {}).get("formatted") or ""
    bf = (b.get("instruction") or {}).get("formatted") or ""
    mark = " " if (k in ("", "DIFF_NONE")) else "*"
    print("%s %3d  %-36s | %-36s  %s" % (mark, idx, af, bf, k if mark == "*" else ""))
