import json, sys
from pathlib import Path

p = Path(r"C:\Users\douglaswhittingham\pkmn-colosseum\tools\decomp_work\dvd_diff.json")
j = json.loads(p.read_text(encoding="utf-8-sig"))

# right = our compiled DVD.o
right = {s["name"]: s for s in j["right"]["symbols"]
         if s.get("kind") == "SYMBOL_FUNCTION"}
left = {s["name"]: s for s in j["left"]["symbols"]
        if s.get("kind") == "SYMBOL_FUNCTION"}

rows = []
for name, rs in right.items():
    pct = rs.get("match_percent", 0.0)
    rows.append((pct, name))
rows.sort()
for pct, name in rows:
    print(f"{pct:8.4f}%  {name}")
print(f"\nTotal functions in DVD.o (right): {len(rows)}")
