#!/usr/bin/env python3
"""Find ACTIVE asm-wrapper functions (genuinely undecompiled) with instr counts."""
import re, os, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
SRC = ROOT / "src"
WALLS = ('colosseum_battle','colosseum_event','colosseum_script')
asm_re = re.compile(r'^\s*asm\s+\w+\s+(fn_[0-9A-Fa-f]+)\s*\(')
inc_re = re.compile(r'#include\s+"([^"]*_fn_[0-9A-Fa-f]+\.inc)"')

def active_asm_in(path):
    out=[]
    # track #if 0 inactivity via a simple stack: each #if pushes whether branch active
    # We approximate: detect `#if 0` ... `#else` ... `#endif`. asm wrapper is the
    # if-branch. If `#if 0`, asm is INACTIVE. Otherwise (#if 1 / no guard) active.
    lines=path.read_text(errors='replace').splitlines()
    # stack of active-state
    stack=[True]
    i=0
    while i < len(lines):
        l=lines[i]
        s=l.strip()
        if s.startswith('#if'):
            cond = s[3:].strip()
            active = stack[-1] and (cond not in ('0',))
            stack.append(active)
        elif s.startswith('#elif'):
            pass
        elif s.startswith('#else'):
            if len(stack)>1:
                parent=stack[-2]; stack[-1]= parent and not stack[-1]
        elif s.startswith('#endif'):
            if len(stack)>1: stack.pop()
        else:
            m=asm_re.match(l)
            if m and stack[-1]:
                # find the .inc on the next few lines for size
                size=None; fn=m.group(1)
                for j in range(i, min(i+4,len(lines))):
                    im=inc_re.search(lines[j])
                    if im:
                        ip=ROOT/im.group(1)
                        if ip.exists():
                            size=sum(1 for x in ip.read_text(errors='replace').splitlines() if x.strip() and not x.strip().startswith('.') and 'nofralloc' not in x)
                        break
                out.append((fn,size))
        i+=1
    return out

results=[]
for c in SRC.rglob('*.c'):
    rel=c.relative_to(ROOT).as_posix()
    if any(w in rel for w in WALLS): continue
    for fn,size in active_asm_in(c):
        results.append((size if size is not None else 999, fn, rel))
results.sort(key=lambda x:(x[0], x[2]))
LIM=int(sys.argv[1]) if len(sys.argv)>1 and sys.argv[1].isdigit() else 40
print(f"total active asm-wrappers (non-wall): {len(results)}")
print(f"smallest {LIM} (instr-count, fn, tu):")
for sz,fn,tu in results[:LIM]:
    print(f"  {sz:3d}  {fn}  {tu}")
