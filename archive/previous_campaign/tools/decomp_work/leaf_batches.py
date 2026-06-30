#!/usr/bin/env python3
"""List UN-ATTEMPTED leaf active-asm functions, partitioned small/medium, for grind workflows."""
import re
import json
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
SRC = ROOT / "src"
WALLS = ('colosseum_battle', 'colosseum_event', 'colosseum_script')
asm_re = re.compile(r'^\s*asm\s+\w+\s+(fn_[0-9A-Fa-f]+)\s*\(')
inc_re = re.compile(r'#include\s+"([^"]*_fn_[0-9A-Fa-f]+\.inc)"')

# everything already tried this/prior sessions (DONE, walls, near-miss, NEAR, special-skip)
ATTEMPTED = {
    # DONE
    'fn_80135C58', 'fn_801629FC', 'fn_80163BE4', 'fn_80163CA8', 'fn_80163DE8', 'fn_8005D798',
    # walls / reverted near-miss
    'fn_8019C128', 'fn_80191788', 'fn_8011BA0C', 'fn_8016292C', 'fn_80161D20', 'fn_80061B74',
    # prior-workflow NEAR (input.c family + others)
    'fn_800F7DE4', 'fn_800F7E40', 'fn_800F7E9C', 'fn_800F7BC4', 'fn_800F7C28', 'fn_800F7B5C',
    'fn_800F7AF0', 'fn_800F7A08', 'fn_800F7A7C', 'fn_800F7EF8', 'fn_800F80B0', 'fn_80162E14',
    'fn_800F7108', 'fn_80017790', 'fn_80068F84', 'fn_80069944', 'fn_801A6C34',
    # special-form, not normal C
    'fn_8019C6FC', 'fn_800F02F4', 'fn_8009A0C0',
}


def instr_count(p):
    n = 0
    for x in p.read_text(errors='replace').splitlines():
        t = x.strip()
        if not t or t.startswith('.') or 'nofralloc' in t:
            continue
        n += 1
    return n


def leaf(p):
    instrs = []
    for x in p.read_text(errors='replace').splitlines():
        t = re.sub(r'^/\*.*?\*/\s*', '', x.strip())
        if not t or t.startswith('.') or 'nofralloc' in t:
            continue
        instrs.append(t)
    return not any(re.match(r'(bl|mflr|stwu|b |bla|bctrl)\b', i) for i in instrs)


def active_asm(path):
    out = []
    lines = path.read_text(errors='replace').splitlines()
    stack = [True]
    for i, l in enumerate(lines):
        s = l.strip()
        if s.startswith('#if'):
            stack.append(stack[-1] and s[3:].strip() != '0')
        elif s.startswith('#else'):
            if len(stack) > 1:
                stack[-1] = stack[-2] and not stack[-1]
        elif s.startswith('#endif'):
            if len(stack) > 1:
                stack.pop()
        else:
            m = asm_re.match(l)
            if m and stack[-1]:
                for j in range(i, min(i + 4, len(lines))):
                    im = inc_re.search(lines[j])
                    if im:
                        ip = ROOT / im.group(1)
                        if ip.exists():
                            out.append((m.group(1), instr_count(ip), leaf(ip), path.relative_to(ROOT).as_posix()))
                        break
    return out


res = []
for c in SRC.rglob('*.c'):
    rel = c.relative_to(ROOT).as_posix()
    if any(w in rel for w in WALLS):
        continue
    for fn, n, isleaf, tu in active_asm(c):
        if fn not in ATTEMPTED:
            res.append((n, fn, isleaf, tu))
res.sort()
small = [r for r in res if r[0] <= 20]
medium = [r for r in res if 20 < r[0] <= 45]
print(f"UN-ATTEMPTED active-asm (non-wall): {len(res)} | small(<=20): {len(small)} | medium(21-45): {len(medium)}")
print("\n--- SMALL (<=20 instr), L=leaf ---")
for n, fn, isleaf, tu in small[:34]:
    print(f"  {n:3d} {'L' if isleaf else ' '}  {fn}  {tu}")
print("\n--- MEDIUM (21-45 instr), L=leaf ---")
for n, fn, isleaf, tu in medium[:34]:
    print(f"  {n:3d} {'L' if isleaf else ' '}  {fn}  {tu}")
