#!/usr/bin/env python3
"""List small ACTIVE asm-wrappers that are LEAF (no bl/mflr/stwu) -> clean match candidates."""
import re
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent.parent
SRC = ROOT / "src"
WALLS = ('colosseum_battle', 'colosseum_event', 'colosseum_script')
asm_re = re.compile(r'^\s*asm\s+\w+\s+(fn_[0-9A-Fa-f]+)\s*\(')
inc_re = re.compile(r'#include\s+"([^"]*_fn_[0-9A-Fa-f]+\.inc)"')

def analyze_inc(p):
    instrs=[]
    for x in p.read_text(errors='replace').splitlines():
        t=x.strip()
        if not t or t.startswith('.') or 'nofralloc' in t: continue
        # strip dtk comment prefix
        t=re.sub(r'^/\*.*?\*/\s*','',t)
        instrs.append(t)
    leaf = not any(re.match(r'(bl|mflr|stwu|b |bla|bctrl)\b', i) for i in instrs)
    return len(instrs), leaf, instrs

def active_asm_in(path):
    out=[]
    lines=path.read_text(errors='replace').splitlines()
    stack=[True]
    for i,l in enumerate(lines):
        s=l.strip()
        if s.startswith('#if'): stack.append(stack[-1] and s[3:].strip()!='0')
        elif s.startswith('#else'):
            if len(stack)>1: stack[-1]=stack[-2] and not stack[-1]
        elif s.startswith('#endif'):
            if len(stack)>1: stack.pop()
        else:
            m=asm_re.match(l)
            if m and stack[-1]:
                for j in range(i,min(i+4,len(lines))):
                    im=inc_re.search(lines[j])
                    if im:
                        ip=ROOT/im.group(1)
                        if ip.exists():
                            n,leaf,_=analyze_inc(ip)
                            out.append((m.group(1),n,leaf,im.group(1)))
                        break
    return out

res=[]
for c in SRC.rglob('*.c'):
    rel=c.relative_to(ROOT).as_posix()
    if any(w in rel for w in WALLS): continue
    for fn,n,leaf,inc in active_asm_in(c):
        if leaf: res.append((n,fn,rel,inc))
res.sort()
print(f"small LEAF active asm-wrappers: {len(res)} total")
for n,fn,rel,inc in res[:35]:
    print(f"  {n:3d}  {fn}  {rel}")
