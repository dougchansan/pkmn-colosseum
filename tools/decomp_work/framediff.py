#!/usr/bin/env python3
"""Show retail-vs-ours prologue, epilogue and stack-slot usage for one function.

find_frame_mismatch.py says a frame differs; this says WHICH slot or register
is extra, which is what you need before touching source. Guessing the local
from the C and applying `volatile` to it lost 0.19pp on fn_800411FC.

usage: framediff.py <unit> <fn>
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

p = subprocess.run([OBJDIFF, 'diff', '-p', '.', '-u', unit, '-o', '-',
                    '--format', 'json'], capture_output=True, text=True,
                   timeout=180)
if p.returncode != 0 or not p.stdout.strip():
    sys.exit('objdiff-cli failed: %s' % p.stderr.strip()[:300])
d = json.loads(p.stdout)


def insns(side):
    for s in d[side]['symbols']:
        if s.get('name') == fn:
            return [i.get('instruction', {}).get('formatted', '')
                    for i in s['instructions']]
    return None


L, R = insns('left'), insns('right')
if L is None or R is None:
    sys.exit('%s not found in %s' % (fn, unit))

SAVE = re.compile(r'^(stwu|stmw|stw|stfd|psq_st|mflr|mfspr|addi\s+r1)')
REST = re.compile(r'^(lmw|lwz|lfd|psq_l|mtlr|mtspr|addi\s+r1|blr)')


def head(seq, n=14):
    return [i for i in seq[:n] if SAVE.match(i)]


def tail(seq, n=16):
    return [i for i in seq[-n:] if REST.match(i)]


print('%-40s | %s' % ('TARGET (retail) PROLOGUE', 'OURS'))
a, b = head(L), head(R)
for i in range(max(len(a), len(b))):
    x = a[i] if i < len(a) else ''
    y = b[i] if i < len(b) else ''
    print('%-40s %s %s' % (x, '  ' if x == y else '!=', y))

print('\n%-40s | %s' % ('TARGET EPILOGUE', 'OURS'))
a, b = tail(L), tail(R)
for i in range(max(len(a), len(b))):
    x = a[i] if i < len(a) else ''
    y = b[i] if i < len(b) else ''
    print('%-40s %s %s' % (x, '  ' if x == y else '!=', y))

# Which stack slots does each side touch, and how often? A slot present on one
# side only is the extra local; a slot RELOADED after a bl on retail but held
# in a register by us is the volatile-lever candidate.
slot = re.compile(r'\b(stw|lwz|stfs|lfs|stfd|lfd|sth|lhz|stb|lbz)\s+'
                  r'([rf]\d+),\s*(-?(?:0x)?[0-9a-fA-F]+)\(r1\)')


def slots(seq):
    out = {}
    for i in seq:
        m = slot.match(i)
        if m:
            off = int(m.group(3), 16) if m.group(3).startswith('0x') \
                else int(m.group(3))
            out.setdefault(off, []).append(m.group(1))
    return out


def firstdefs(seq, regs):
    """Where each callee-saved register is first written — i.e. WHAT it holds.

    Knowing the frame has one extra GPR is not actionable; knowing that GPR
    receives `mr r29, r4` (the parameter) or a specific load is. Guessing the
    local from the C instead cost -0.19pp (volatile on fn_800411FC) and
    -4.00pp (de-hoisting staticData on fn_800A6BD4).
    """
    out = {}
    for idx, i in enumerate(seq):
        m = re.match(r'^(\w+)\s+(r\d+|f\d+)\s*,\s*(.*)$', i)
        if not m:
            continue
        dst, op = m.group(2), m.group(1)
        if dst in regs and dst not in out and not op.startswith(('stw', 'stmw',
                                                                'stfd', 'psq_st')):
            out[dst] = (idx, i)
    return out


CALLEE = {'r%d' % n for n in range(13, 32)} | {'f%d' % n for n in range(14, 32)}
lf, rf = firstdefs(L, CALLEE), firstdefs(R, CALLEE)
print('\n%-6s %-34s | %s' % ('reg', 'TARGET first def', 'OURS first def'))
for reg in sorted(set(lf) | set(rf), key=lambda r: (r[0], int(r[1:]))):
    x = lf.get(reg, (0, '-'))[1]
    y = rf.get(reg, (0, '-'))[1]
    note = '   <- ours only' if reg in rf and reg not in lf else (
        '   <- retail only' if reg in lf and reg not in rf else '')
    print('%-6s %-34s %s %s%s' % (reg, x, '  ' if x == y else '!=', y, note))

ls, rs = slots(L), slots(R)
print('\n%-8s %-22s | %s' % ('r1 slot', 'TARGET (n ld/st)', 'OURS'))
for off in sorted(set(ls) | set(rs)):
    lv = ls.get(off, [])
    rv = rs.get(off, [])
    mark = '  ' if len(lv) == len(rv) else '!='
    note = ''
    if lv and not rv:
        note = '   <- retail-only slot'
    elif rv and not lv:
        note = '   <- ours-only slot'
    print('%-8s %-22s %s %s%s'
          % (hex(off), '%d %s' % (len(lv), ','.join(sorted(set(lv)))) if lv
             else '-', mark,
             '%d %s' % (len(rv), ','.join(sorted(set(rv)))) if rv else '-',
             note))
