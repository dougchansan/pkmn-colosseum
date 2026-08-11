#!/usr/bin/env python3
"""Show retail-vs-ours `bl` order for one function.

The block-order detector says *that* a block moved; this says *which* calls
are out of place, which is what you need to pick the arm to invert.

usage: callorder.py <unit> <fn>
"""
import json
import os
import re
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

# Same normalisation the detector uses. Retail and our object frequently give
# the SAME address two different names (msgctrlSetValue vs fn_80132A38), which
# compares as a mismatch on raw text and buries the real displacement.
sym = {}
for line in open('config/GC6E01/symbols.txt'):
    m = re.match(r'(\S+) = \.text:0x([0-9A-Fa-f]{8});', line)
    if m:
        sym[m.group(1)] = int(m.group(2), 16)


def addr(name):
    m = re.fullmatch(r'fn_([0-9A-Fa-f]{8})', name)
    return int(m.group(1), 16) if m else sym.get(name, ('NAME', name))


def calls(side):
    for s in d[side]['symbols']:
        if s.get('name') == fn:
            out = []
            for i in s['instructions']:
                t = i.get('instruction', {}).get('formatted', '')
                if t.startswith('bl '):
                    out.append(t.split()[1])
            return out
    return None


L, R = calls('left'), calls('right')
if L is None or R is None:
    sys.exit('%s not found in %s' % (fn, unit))
LA, RA = [addr(x) for x in L], [addr(x) for x in R]
print('%-38s | %s' % ('TARGET (retail)', 'OURS'))
for i in range(max(len(L), len(R))):
    a = L[i] if i < len(L) else ''
    b = R[i] if i < len(R) else ''
    aa = LA[i] if i < len(LA) else None
    ba = RA[i] if i < len(RA) else None
    # flag only genuine address differences; note pure aliasing separately
    mark = '  ' if aa == ba else '!='
    alias = ' (alias)' if aa == ba and a != b else ''
    print('%-38s %s %s%s' % (a, mark, b, alias))
print('\nmatch: %s   (%d retail calls, %d ours)'
      % (LA == RA, len(L), len(R)))
