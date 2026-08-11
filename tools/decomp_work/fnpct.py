#!/usr/bin/env python3
"""Report per-function fuzzy match% for a unit, straight from objdiff-cli.

`measure_fn.py` is pinned to one object from the old gs_title workflow; this
works for any unit. Names come from `build/GC6E01/report.json`.

usage: fnpct.py <unit> [fn ...]      # no fn args => every unmatched fn in unit
"""
import json
import os
import subprocess
import sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
os.chdir(ROOT)

OBJDIFF = os.path.join('build', 'tools',
                       'objdiff-cli.exe' if os.name == 'nt' else 'objdiff-cli')
if not os.path.exists(OBJDIFF):
    sys.exit('objdiff-cli not found at %s — run `ninja` first' % OBJDIFF)

if len(sys.argv) < 2:
    sys.exit(__doc__)
unit, want = sys.argv[1], set(sys.argv[2:])

proc = subprocess.run(
    [OBJDIFF, 'diff', '-p', '.', '-u', unit, '-o', '-', '--format', 'json'],
    capture_output=True, text=True, timeout=180)
if proc.returncode != 0 or not proc.stdout.strip():
    sys.exit('objdiff-cli failed for %s: %s' % (unit, proc.stderr.strip()[:400]))

d = json.loads(proc.stdout)
rows = []
for sym in d['right']['symbols']:
    name = sym.get('name', '')
    pct = sym.get('match_percent')
    if pct is None:
        continue
    if want and name not in want:
        continue
    if not want and pct >= 99.995:
        continue
    rows.append((pct, name))

for pct, name in sorted(rows):
    print('%7.2f%%  %s' % (pct, name))
if want:
    for missing in sorted(want - {n for _, n in rows}):
        print('%7s   %s  (not found in unit)' % ('NA', missing))
