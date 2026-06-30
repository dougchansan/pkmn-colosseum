import json, sys
from pathlib import Path

p = Path(r"C:\Users\douglaswhittingham\pkmn-colosseum\tools\decomp_work\one.json")
j = json.loads(p.read_text(encoding="utf-8-sig"))
target = sys.argv[1]

left = {s["name"]: s for s in j["left"]["symbols"]}
right = {s["name"]: s for s in j["right"]["symbols"]}

ls = left.get(target, {})
rs = right.get(target, {})
li = ls.get("instructions", [])
ri = rs.get("instructions", [])
print(f"== {target} : match {rs.get('match_percent',0):.4f}% ==")
print(f"{'TARGET (aim-for)':45} | {'OURS (compiled C)':45} | kind")
print("-"*120)
n = max(len(li), len(ri))
for idx in range(n):
    L = li[idx] if idx < len(li) else None
    R = ri[idx] if idx < len(ri) else None
    lf = ((L or {}).get("instruction") or {}).get("formatted") or ""
    rf = ((R or {}).get("instruction") or {}).get("formatted") or ""
    k = (L or {}).get("diff_kind") or (R or {}).get("diff_kind") or ""
    mark = "" if k in ("", "DIFF_NONE") else "  <<<"
    print(f"{lf:45} | {rf:45} | {k}{mark}")
