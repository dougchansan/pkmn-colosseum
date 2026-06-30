#!/usr/bin/env python3
"""Map compile-error line numbers in a generated TU to their enclosing function,
and report which flipped bodies are pseudo-register transcriptions (declare rN locals).
Usage: python tools/pcport_map_errors.py build_pc/gen/game/gs_field_world.c /tmp/errlines.txt
"""
import re, sys
from pathlib import Path

gen = Path(sys.argv[1]).read_text(errors="replace").splitlines()
errlines = sorted({int(x) for x in Path(sys.argv[2]).read_text().split()})

DEF_RE = re.compile(r'^(?:static\s+)?(?:const\s+)?[A-Za-z_][\w\s]*?[\s\*]+\**([A-Za-z_]\w*)\s*\(([^;{}()]*)\)\s*\{?\s*$')
KW = {"return","if","while","for","switch","goto","else","do","case","sizeof","typedef"}

# file-scope function spans: (start_line_1based, name)
defs = []
for i, ln in enumerate(gen):
    if not ln or ln[0].isspace() or ln.rstrip().endswith(';'):
        continue
    m = DEF_RE.match(ln)
    if m and m.group(1) not in KW:
        if ln.rstrip().endswith('{') or (i+1 < len(gen) and gen[i+1].lstrip().startswith('{')):
            defs.append((i+1, m.group(1)))

def enclosing(line_no):
    name = None
    for start, n in defs:
        if start <= line_no:
            name = n
        else:
            break
    return name

from collections import Counter
hits = Counter()
for e in errlines:
    n = enclosing(e)
    if n:
        hits[n] += 1

print("functions containing error sites:")
for n, c in hits.most_common():
    print(f"  {c:4d}  {n}")
print(f"total distinct: {len(hits)}")

# pseudo-register flipped bodies: declare rN locals
RVAR = re.compile(r'^\s*(?:register\s+)?(?:u32|s32|u16|s16|u8|s8|f32|int|void\s*\*)\s+r\d+\b')
spans = [(s, (defs[i+1][0]-1 if i+1 < len(defs) else len(gen)), n) for i, (s, n) in enumerate(defs)]
rfuncs = []
for s, e, n in spans:
    if any(RVAR.match(gen[j]) for j in range(s, min(e, len(gen)))):
        rfuncs.append(n)
print(f"\npseudo-register (rN-local) bodies: {len(rfuncs)}")
print(" ".join(rfuncs))
