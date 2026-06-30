#!/usr/bin/env python3
"""Parse a grind-workflow output JSON; print MATCH summary + stage each candidate_c to
build_pc/decompme/grind/<fn>/cand_full.c for the parent to integrate."""
import json
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
out = Path(sys.argv[1])
d = json.loads(out.read_text(encoding='utf-8', errors='replace'))
res = d.get('result', d)
matches = res.get('matches', [])
near = res.get('near', [])
print(f"MATCH={len(matches)}  NEAR={len(near)}")
print("\n=== MATCH (claimed 100%) ===")
for m in matches:
    fn = m['fn']
    dd = ROOT / 'build_pc' / 'decompme' / 'grind' / fn
    dd.mkdir(parents=True, exist_ok=True)
    (dd / 'cand_full.c').write_text(m.get('candidate_c', ''), encoding='utf-8')
    note = (m.get('notes', '') or '')[:140].replace('\n', ' ')
    print(f"  {fn}  -> {dd.name}/cand_full.c")
    print(f"       {note}")
print("\n=== NEAR ===")
for m in near:
    print(f"  {m['fn']}  {m.get('match_pct','?')}%  {(m.get('notes','') or '')[:90]}")
