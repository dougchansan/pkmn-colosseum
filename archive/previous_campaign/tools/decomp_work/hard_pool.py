#!/usr/bin/env python3
"""Hard-function pool for a triage workflow: active asm-wrappers in the colosseum_* wall TUs
(smallest first -- most likely to have a non-stmw tractable case) + large (>45) non-wall fns."""
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
SRC = ROOT / "src"
WALL_TUS = ('colosseum_battle', 'colosseum_event', 'colosseum_script')
asm_re = re.compile(r'^\s*asm\s+\w+\s+(fn_[0-9A-Fa-f]+)\s*\(')
inc_re = re.compile(r'#include\s+"([^"]*_fn_[0-9A-Fa-f]+\.inc)"')


def instr_count(p):
    return sum(1 for x in p.read_text(errors='replace').splitlines()
               if x.strip() and not x.strip().startswith('.') and 'nofralloc' not in x)


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
                            out.append((m.group(1), instr_count(ip), path.relative_to(ROOT).as_posix()))
                        break
    return out


wall, large = [], []
for c in SRC.rglob('*.c'):
    rel = c.relative_to(ROOT).as_posix()
    is_wall = any(w in rel for w in WALL_TUS)
    for fn, n, tu in active_asm(c):
        if is_wall:
            wall.append((n, fn, tu))
        elif n > 45:
            large.append((n, fn, tu))
wall.sort()
large.sort()
print(f"colosseum_* wall-TU asm-wrappers: {len(wall)} | large(>45) non-wall: {len(large)}")
print("\n--- WALL-TU smallest 24 ---")
for n, fn, tu in wall[:24]:
    print(f"  {n:3d}  {fn}  {tu}")
print("\n--- LARGE non-wall (>45) first 20 ---")
for n, fn, tu in large[:20]:
    print(f"  {n:3d}  {fn}  {tu}")
