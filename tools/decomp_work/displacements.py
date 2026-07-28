#!/usr/bin/env python3
"""Show the exact bl displacement for many functions at once.

callorder.py prints one function's full call list; this prints only the moved
calls, for a whole worklist, resolving each function's unit from report.json.
Names are normalised to addresses, so symbol aliases do not show as diffs.

usage: displacements.py <fn> [fn ...]
"""
import difflib
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
os.chdir(ROOT)
OBJDIFF = os.path.join('build', 'tools',
                       'objdiff-cli.exe' if os.name == 'nt' else 'objdiff-cli')
if len(sys.argv) < 2:
    sys.exit(__doc__)
want = sys.argv[1:]

sym = {}
for line in open('config/GC6E01/symbols.txt'):
    m = re.match(r'(\S+) = \.text:0x([0-9A-Fa-f]{8});', line)
    if m:
        sym[m.group(1)] = int(m.group(2), 16)


def addr(name):
    m = re.fullmatch(r'fn_([0-9A-Fa-f]{8})', name)
    return int(m.group(1), 16) if m else sym.get(name, ('NAME', name))


# fn -> unit, from the live report
report = json.load(open('build/GC6E01/report.json'))
unit_of, pct_of = {}, {}
for u in report['units']:
    fns = list(u.get('functions') or [])
    for s in (u.get('sections') or []):
        fns += list(s.get('functions') or [])
    for f in fns:
        if f['name'] in want:
            unit_of[f['name']] = u['name']
            pct_of[f['name']] = f.get('fuzzy_match_percent', 0.0)


def scan(fn):
    unit = unit_of.get(fn)
    if not unit:
        return fn, unit, ['not found in report.json']
    p = subprocess.run([OBJDIFF, 'diff', '-p', '.', '-u', unit, '-o', '-',
                        '--format', 'json'],
                       capture_output=True, text=True, timeout=180)
    if p.returncode != 0 or not p.stdout.strip():
        return fn, unit, ['objdiff failed: %s' % p.stderr.strip()[:120]]
    d = json.loads(p.stdout)

    def calls(side):
        for s in d[side]['symbols']:
            if s.get('name') == fn:
                return [t.split()[1] for t in
                        (i.get('instruction', {}).get('formatted', '')
                         for i in s['instructions']) if t.startswith('bl ')]
        return []

    ln, rn = calls('left'), calls('right')
    L, R = [addr(x) for x in ln], [addr(x) for x in rn]
    if L == R:
        return fn, unit, ['ORDER ALREADY MATCHES (residual is not block order)']
    out = []
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(
            None, L, R, autojunk=False).get_opcodes():
        if tag != 'equal':
            out.append('%-7s retail[%d:%d]=%s  ours[%d:%d]=%s'
                       % (tag, i1, i2, ln[i1:i2], j1, j2, rn[j1:j2]))
    return fn, unit, out


with ThreadPoolExecutor(max_workers=8) as ex:
    for fn, unit, lines in ex.map(scan, want):
        print('== %s  %.2f%%  [%s]' % (fn, pct_of.get(fn, 0.0), unit))
        for line in lines:
            print('   ' + line)
