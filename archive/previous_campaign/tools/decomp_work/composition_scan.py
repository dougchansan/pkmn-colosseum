#!/usr/bin/env python3
"""Composition of the decomp: classify every function as active-asm-stub
(undecompiled) vs active-C, by tracking #if/#else/#endif branches in each
src/**/*.c. Active-asm = the function's `asm void fn_X` wrapper sits in a
live preprocessor branch (#if 1 or unguarded). Active-C = a non-asm
definition of fn_X sits in a live branch.
"""
import re, os
from collections import defaultdict

ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
SRC = os.path.join(ROOT, "src")

asm_re = re.compile(r"^\s*asm\s+(?:void|u32|s32|int)\s+(fn_[0-9A-Fa-f]+)\s*\(")
# active-C function definition: <type> fn_X( ... at column start, not 'asm', not 'extern'
cdef_re = re.compile(r"^\s*(?!extern|asm)([A-Za-z_][\w *]*?)\b(fn_[0-9A-Fa-f]+)\s*\(")
if0_re = re.compile(r"^\s*#if\s+0\b")
if1_re = re.compile(r"^\s*#if\s+1\b")
ifx_re = re.compile(r"^\s*#if(?:def|ndef)?\b")
else_re = re.compile(r"^\s*#else\b")
endif_re = re.compile(r"^\s*#endif\b")

asm_active = defaultdict(set)   # file -> {fn}  (undecompiled, asm wrapper live)
c_active = defaultdict(set)     # file -> {fn}  (active C definition live)

for dp, _, files in os.walk(SRC):
    for fn in files:
        if not fn.endswith(".c"):
            continue
        path = os.path.join(dp, fn)
        rel = os.path.relpath(path, ROOT).replace("\\", "/")
        stack = []  # each: ['active'|'inactive', seen_else]
        try:
            lines = open(path, encoding="utf-8", errors="replace").read().splitlines()
        except Exception:
            continue
        def live():
            return all(s[0] == "active" for s in stack)
        for ln in lines:
            if if0_re.match(ln):
                stack.append(["inactive", False])
            elif if1_re.match(ln):
                stack.append(["active", False])
            elif ifx_re.match(ln):  # #ifdef/#ifndef/other #if -> treat as active (conservative)
                stack.append(["active", False])
            elif else_re.match(ln):
                if stack:
                    stack[-1][0] = "inactive" if stack[-1][0] == "active" else "active"
                    stack[-1][1] = True
            elif endif_re.match(ln):
                if stack:
                    stack.pop()
            else:
                m = asm_re.match(ln)
                if m and live():
                    asm_active[rel].add(m.group(1))
                    continue
                m = cdef_re.match(ln)
                if m and live():
                    c_active[rel].add(m.group(2))

all_asm = set()
all_c = set()
for f, s in asm_active.items(): all_asm |= {(f, x) for x in s}
for f, s in c_active.items(): all_c |= {(f, x) for x in s}

n_asm = sum(len(s) for s in asm_active.values())
n_c = sum(len(s) for s in c_active.values())
print("==== COMPOSITION (source-parsed, src/**/*.c) ====")
print(f"active asm-stub functions (UNDECOMPILED): {n_asm}")
print(f"active-C function definitions:            {n_c}")
print()
print("Top files by undecompiled asm-stub count:")
for f, s in sorted(asm_active.items(), key=lambda kv: -len(kv[1]))[:25]:
    print(f"  {len(s):>4}  {f}   (active-C here: {len(c_active.get(f,()))})")
print()
print("Top files by active-C count (decomp attempted):")
for f, s in sorted(c_active.items(), key=lambda kv: -len(kv[1]))[:15]:
    print(f"  {len(s):>4}  {f}   (asm-stub here: {len(asm_active.get(f,()))})")
