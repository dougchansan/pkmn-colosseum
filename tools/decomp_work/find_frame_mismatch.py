#!/usr/bin/env python3
"""Find functions whose stack frame differs from retail's.

A frame-size mismatch is a cheap, unambiguous signal that the *shape* of the
source is wrong, not just its register allocation, and it names the problem
before you read a single diff line. Three causes seen so far:

  * a local we hold in a callee-saved register that retail spills to the
    stack and reloads -- fix by declaring that one local `volatile`
    (_cameraLoadCameraMatrix 0xe0 -> 0xc0, fightActionFlowKaisiPre's u8 flag)
  * the same locals declared in two separate blocks where retail shares one
    set at function scope (two sets of slots = exactly the extra frame)
  * a scratch buffer declared larger than retail's whole frame
    (fn_800F8268 had u8 pad[0x50] when retail's frame *is* 0x50)

Also reports the saved-register span from `stmw`, since "we save fewer/more
registers than retail" is the other half of the same story.

Sign convention: delta = ours - retail. Positive means our frame is bigger.

usage: find_frame_mismatch.py [min_fuzzy] [min_size]
"""
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..')
os.chdir(ROOT)
MIN_FUZZY = float(sys.argv[1]) if len(sys.argv) > 1 else 40.0
MIN_SIZE = int(sys.argv[2]) if len(sys.argv) > 2 else 200

# See find_reordered_blocks.py: the extensionless binary is a Linux ELF and
# fails with "Exec format error" on Windows, which a bare except turns into a
# clean-looking empty result.
OBJDIFF = os.path.join('build', 'tools',
                       'objdiff-cli.exe' if os.name == 'nt' else 'objdiff-cli')
if not os.path.exists(OBJDIFF):
    sys.exit('objdiff-cli not found at %s — run `ninja` first' % OBJDIFF)

report = json.load(open('build/GC6E01/report.json'))
units = []
for u in report['units']:
    fns = list(u.get('functions', []))
    for s in u.get('sections', []):
        fns += s.get('functions', [])
    sel = [(f['name'], f.get('fuzzy_match_percent', 0.0), int(f['size']))
           for f in fns
           if MIN_FUZZY <= f.get('fuzzy_match_percent', 0.0) < 99.5
           and int(f['size']) >= MIN_SIZE]
    if sel:
        units.append((u['name'], sel))

STWU = re.compile(r'stwu\s+r1,\s*-(0x[0-9a-fA-F]+|\d+)\(r1\)')
STMW = re.compile(r'stmw\s+r(\d+),')
FSAVE = re.compile(r'stfd\s+f(\d+),')


def prologue(symbol):
    """(frame_size, first_saved_gpr, first_saved_fpr) from the first 12 insns."""
    frame = gpr = fpr = None
    for i in symbol['instructions'][:12]:
        t = i.get('instruction', {}).get('formatted', '')
        m = STWU.search(t)
        if m and frame is None:
            frame = int(m.group(1), 0)
        m = STMW.search(t)
        if m and gpr is None:
            gpr = int(m.group(1))
        m = FSAVE.search(t)
        if m and fpr is None:
            fpr = int(m.group(1))
    return frame, gpr, fpr


FAILURES = []


def scan(item):
    unit, sel = item
    try:
        out = subprocess.run(
            [OBJDIFF, 'diff', '-p', '.', '-u', unit,
             '-o', '-', '--format', 'json'],
            capture_output=True, text=True, timeout=120).stdout
        d = json.loads(out)
        idx = {s: {x['name']: x for x in d[s]['symbols']}
               for s in ('left', 'right')}
    except Exception as e:
        FAILURES.append((unit, '%s: %s' % (type(e).__name__, e)))
        return []
    rows = []
    for fn, pct, size in sel:
        left, right = idx['left'].get(fn), idx['right'].get(fn)
        if not left or not right:
            continue
        lf, lg, lfp = prologue(left)
        rf, rg, rfp = prologue(right)
        if lf is None or rf is None:
            continue
        dframe = rf - lf
        dgpr = (lg - rg) if (lg is not None and rg is not None) else 0
        # an extra saved FPR on our side is the volatile-lever signature
        extra_fpr = (rfp is not None and lfp is None)
        if dframe == 0 and dgpr == 0 and not extra_fpr:
            continue
        # Fraction of instructions that already line up. A frame mismatch on a
        # body that otherwise matches is a shape bug worth one targeted edit;
        # the same mismatch at 5% aligned is a register transliteration and
        # wants a rewrite, not a lever. All four extra-FPR hits on the first
        # run were the latter, which is why this column exists.
        # objdiff omits diff_kind entirely on instructions that match.
        ins = right['instructions']
        aligned = sum(1 for i in ins if 'diff_kind' not in i)
        align = 100.0 * aligned / max(len(ins), 1)
        gap = size * (100 - pct) / 100
        rows.append((abs(dframe) * gap, dframe, dgpr, extra_fpr,
                     gap, pct, size, fn, unit, align))
    return rows


rows = []
with ThreadPoolExecutor(max_workers=8) as ex:
    for r in ex.map(scan, units):
        rows += r
# Rank by gap weighted by how much of the body already matches: a frame bug in
# otherwise-aligned code is one edit away, soup is not.
rows.sort(key=lambda r: -(r[4] * r[9] / 100.0))

MIN_ALIGN = 50.0
if FAILURES:
    print(f'WARNING: {len(FAILURES)} of {len(units)} units failed to extract; '
          f'results are INCOMPLETE')
    for unit, err in FAILURES[:3]:
        print(f'  {unit} -> {err}')
print(f'{len(rows)} functions whose frame or saved-register span differs')
print(f'{"dframe":>7} {"dgpr":>5} {"fpr":>4} {"gap":>7} {"fuzzy":>7} '
      f'{"algn":>6} {"size":>6}  fn')
shown = [r for r in rows if r[9] >= MIN_ALIGN]
for _, dframe, dgpr, extra_fpr, gap, pct, size, fn, unit, align in shown[:30]:
    print(f'{dframe:>+7} {dgpr:>+5} {"yes" if extra_fpr else "":>4} '
          f'{gap:>7.0f} {pct:>6.2f}% {align:>5.1f}% {size:>6}  {fn}')
print(f'\n({len(rows) - len(shown)} more below {MIN_ALIGN:.0f}% aligned '
      f'suppressed: those are transliterations, not shape bugs)')

# The volatile lever's exact signature: we save a callee-saved FPR that retail
# does not, and our frame is correspondingly larger. Highest-confidence subset.
hot = [r for r in rows if r[3] and r[1] > 0 and r[9] >= MIN_ALIGN]
print(f'\n-- extra callee-saved FPR on our side ({len(hot)}): '
      f'declare the reloaded local volatile --')
for _, dframe, dgpr, _, gap, pct, size, fn, unit, align in hot:
    print(f'{dframe:>+7} {dgpr:>+5} {gap:>7.0f} {pct:>6.2f}% {align:>5.1f}% '
          f'{size:>6}  {fn}  [{unit}]')
