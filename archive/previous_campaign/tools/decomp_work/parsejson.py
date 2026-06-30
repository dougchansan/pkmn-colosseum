#!/usr/bin/env python3
"""Parse a saved objdiff JSON file and print aligned diffs for given symbols."""
import json
import sys

path = sys.argv[1]
syms = sys.argv[2:]
with open(path, "r", encoding="utf-8", errors="ignore") as f:
    d = json.load(f)
left = {s["name"]: s for s in d["left"]["symbols"]}
right = {s["name"]: s for s in d["right"]["symbols"]}
for sym in syms:
    ls = left.get(sym, {})
    rs = right.get(sym, {})
    li = ls.get("instructions", [])
    ri = rs.get("instructions", [])
    print(f"\n=== {sym}  target%={ls.get('match_percent')}  "
          f"ours%={rs.get('match_percent')} ===")
    n = max(len(li), len(ri))
    for idx in range(n):
        lf = rf = lk = rk = ""
        if idx < len(li):
            lf = ((li[idx].get("instruction") or {}).get("formatted")) or ""
            lk = li[idx].get("diff_kind") or ""
        if idx < len(ri):
            rf = ((ri[idx].get("instruction") or {}).get("formatted")) or ""
            rk = ri[idx].get("diff_kind") or ""
        mark = "X" if ((lk and lk != "DIFF_NONE") or
                       (rk and rk != "DIFF_NONE")) else " "
        print(f"{mark} {lf:<44} | {rf}")
