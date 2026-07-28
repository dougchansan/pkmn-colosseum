#!/usr/bin/env python3
"""Compact framediff across many functions: what is in the extra register?

framediff.py is the full read for one function. This is the triage pass: for
each function it prints the frame/saved-register delta and, for every
callee-saved register present on only ONE side, the instruction that first
defines it — the value that register holds. That is the only fact that makes
a frame delta actionable.

usage: framescan.py <fn> [fn ...]
"""
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

CALLEE = {'r%d' % n for n in range(13, 32)} | {'f%d' % n for n in range(14, 32)}
FRAME = re.compile(r'stwu\s+r1,\s*-(0x[0-9a-fA-F]+|\d+)\(r1\)')
SPAN = re.compile(r'(stmw|stw|stfd|psq_st)\s+([rf]\d+),')


def firstdefs(seq):
    out = {}
    for i in seq:
        m = re.match(r'^(\w+)\s+([rf]\d+)\s*,\s*(.*)$', i)
        if not m:
            continue
        op, dst = m.group(1), m.group(2)
        if dst in CALLEE and dst not in out and not op.startswith(
                ('stw', 'stmw', 'stfd', 'psq_st', 'stfs', 'sth', 'stb')):
            out[dst] = i
    return out


def frame(seq):
    for i in seq[:8]:
        m = FRAME.search(i)
        if m:
            v = m.group(1)
            return int(v, 16) if v.startswith('0x') else int(v)
    return None


def saved(seq):
    """Callee-saved regs written to the frame in the prologue."""
    out = set()
    for i in seq[:16]:
        m = SPAN.match(i)
        if m and '(r1)' in i:
            reg = m.group(2)
            if m.group(1) == 'stmw':
                n = int(reg[1:])
                out |= {'r%d' % k for k in range(n, 32)}
            elif reg in CALLEE:
                out.add(reg)
    return out


def scan(fn):
    unit = unit_of.get(fn)
    if not unit:
        return fn, ['not in report.json']
    p = subprocess.run([OBJDIFF, 'diff', '-p', '.', '-u', unit, '-o', '-',
                        '--format', 'json'], capture_output=True, text=True,
                       timeout=180)
    if p.returncode != 0 or not p.stdout.strip():
        return fn, ['objdiff failed']
    d = json.loads(p.stdout)

    def ins(side):
        for s in d[side]['symbols']:
            if s.get('name') == fn:
                return [i.get('instruction', {}).get('formatted', '')
                        for i in s['instructions']]
        return None

    L, R = ins('left'), ins('right')
    if L is None or R is None:
        return fn, ['not found in unit']
    lf, rf = frame(L), frame(R)
    lsv, rsv = saved(L), saved(R)
    ldef, rdef = firstdefs(L), firstdefs(R)
    lines = ['frame retail=%s ours=%s (%+d)   saved retail=%d ours=%d (%+d)'
             % (hex(lf) if lf else '?', hex(rf) if rf else '?',
                (rf - lf) if (lf and rf) else 0,
                len(lsv), len(rsv), len(rsv) - len(lsv))]
    for reg in sorted(rsv - lsv, key=lambda r: (r[0], int(r[1:]))):
        lines.append('  OURS-ONLY  %-4s = %s' % (reg, rdef.get(reg, '<no def>')))
    for reg in sorted(lsv - rsv, key=lambda r: (r[0], int(r[1:]))):
        lines.append('  RETAIL-ONLY %-4s = %s' % (reg, ldef.get(reg, '<no def>')))
    # same register, different value = a coloring difference, not a count one
    for reg in sorted(lsv & rsv, key=lambda r: (r[0], int(r[1:]))):
        a, b = ldef.get(reg, ''), rdef.get(reg, '')
        if a and b and a != b:
            lines.append('  DIFFERS    %-4s retail: %-32s ours: %s'
                         % (reg, a, b))
    return fn, lines


with ThreadPoolExecutor(max_workers=6) as ex:
    for fn, lines in ex.map(scan, want):
        print('== %s  %.2f%%' % (fn, pct_of.get(fn, 0.0)))
        for l in lines:
            print('   ' + l)
