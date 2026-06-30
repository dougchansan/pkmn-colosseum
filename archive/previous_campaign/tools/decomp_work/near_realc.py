#!/usr/bin/env python3
"""Large near-miss functions that are GENUINE permuter targets: active branch is REAL_C
(NOT an asm-wrapper, NOT a TODO/empty stub) AND objdiff match% in [min,max).

Fixes near_large.py's flaw: active asm-wrappers score 99.x% (reloc/pool artifacts) and
masquerade as near-misses, but they have no C to permute.

Usage: python tools/decomp_work/near_realc.py [min_pct] [max_pct] [min_bytes]
  defaults: 88 99.99 0xC0
"""
import os
import json
import re
import subprocess
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE = ROOT / "build" / "GC6E01" / "base"
SRC = ROOT / "src"

min_pct = float(sys.argv[1]) if len(sys.argv) > 1 else 88.0
max_pct = float(sys.argv[2]) if len(sys.argv) > 2 else 99.99
min_bytes = int(sys.argv[3], 0) if len(sys.argv) > 3 else 0xC0

FUNC_DEF = re.compile(r'^\s*(?:static\s+|asm\s+|inline\s+)*[A-Za-z_][\w ]*[\s*]+(fn_[0-9A-Fa-f]+)\s*\([^;]*\)\s*\{?\s*$')
ASM_DEF = re.compile(r'^\s*asm\s+\w+\s+(fn_[0-9A-Fa-f]+)\s*\(')
STUB_RE = re.compile(r'/\*\s*TODO|/\*\s*stub|^\s*\{\s*\}\s*$', re.I)

# classify every fn by its ACTIVE branch
cls = {}   # fn -> 'ASM' | 'STUB' | 'REAL_C'
for c in SRC.rglob('*.c'):
    lines = c.read_text(errors='replace').splitlines()
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
        elif stack[-1]:
            am = ASM_DEF.match(l)
            if am:
                cls[am.group(1)] = 'ASM'
                continue
            fm = FUNC_DEF.match(l)
            if fm:
                fn = fm.group(1)
                # peek body for stub
                body = ' '.join(lines[i:i + 3])
                cls[fn] = 'STUB' if STUB_RE.search(body) else 'REAL_C'

rows = {}
for o in sorted(BASE.rglob("*.o")):
    try:
        p = subprocess.run([str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(o), "-o", "-",
                            "--format", "json", "-c", "ppc.calculatePoolRelocations=false"],
                           capture_output=True, text=True, timeout=60)
        d = json.loads(p.stdout)
    except Exception:
        continue
    tu = o.relative_to(BASE).as_posix()

    def walk(x):
        if isinstance(x, dict):
            n = x.get('name', '')
            if n.startswith('fn_') and 'match_percent' in x and 'size' in x:
                try:
                    sz = int(str(x['size']), 0)
                except Exception:
                    sz = int(x['size']) if str(x['size']).isdigit() else 0
                mp = x['match_percent']
                if n not in rows or mp > rows[n][1]:
                    rows[n] = (sz, mp, tu)
            for v in x.values():
                walk(v)
        elif isinstance(x, list):
            for v in x:
                walk(v)
    walk(d)

hits = [(n, sz, mp, tu) for n, (sz, mp, tu) in rows.items()
        if sz >= min_bytes and min_pct <= mp < max_pct and cls.get(n) == 'REAL_C']
hits.sort(key=lambda r: -r[2])
print(f"GENUINE REAL_C large near-misses (>= {min_bytes}B, {min_pct}-{max_pct}%): {len(hits)}")
print(f"{'match%':>7} {'bytes':>5}  {'fn':<14} TU")
for n, sz, mp, tu in hits[:60]:
    print(f"{mp:7.2f} {sz:5d}  {n:<14} {tu}")
