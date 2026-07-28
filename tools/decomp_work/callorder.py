#!/usr/bin/env python3
"""Show retail-vs-ours `bl` order for one function.

The block-order detector says *that* a block moved; this says *which* calls
are out of place, which is what you need to pick the arm to invert.

usage: callorder.py <unit> <fn>
"""
import json
import os
import subprocess
import sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
os.chdir(ROOT)
OBJDIFF = os.path.join('build', 'tools',
                       'objdiff-cli.exe' if os.name == 'nt' else 'objdiff-cli')
if len(sys.argv) != 3:
    sys.exit(__doc__)
unit, fn = sys.argv[1], sys.argv[2]

proc = subprocess.run(
    [OBJDIFF, 'diff', '-p', '.', '-u', unit, '-o', '-', '--format', 'json'],
    capture_output=True, text=True, timeout=180)
if proc.returncode != 0 or not proc.stdout.strip():
    sys.exit('objdiff-cli failed: %s' % proc.stderr.strip()[:400])
d = json.loads(proc.stdout)


def calls(side):
    for s in d[side]['symbols']:
        if s.get('name') == fn:
            return [i['instruction']['formatted']
                    for i in s['instructions']
                    if i.get('instruction', {}).get('formatted', '')
                    .startswith('bl ')]
    return None


L, R = calls('left'), calls('right')
if L is None or R is None:
    sys.exit('%s not found in %s' % (fn, unit))
print('%-38s | %s' % ('TARGET (retail)', 'OURS'))
for i in range(max(len(L), len(R))):
    a = L[i] if i < len(L) else ''
    b = R[i] if i < len(R) else ''
    print('%-38s %s %s' % (a, '!=' if a != b else '  ', b))
print('\nmatch: %s   (%d retail calls, %d ours)' % (L == R, len(L), len(R)))
