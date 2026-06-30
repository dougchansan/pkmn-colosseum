#!/usr/bin/env python3
"""Summarize what's left to decompile: active asm-wrappers by TU + small-leaf count."""
import re
import subprocess
import collections
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent


def run(script):
    return subprocess.run(["python", str(ROOT / "tools" / "decomp_work" / script)],
                          capture_output=True, text=True).stdout


print("=== active asm-wrappers (genuinely undecompiled, non-wall) by TU ===")
out = run("find_active_asm.py")
tu = collections.Counter()
total = 0
for l in out.splitlines():
    m = re.search(r'fn_[0-9A-Fa-f]+\s+(\S+)', l)
    if m:
        tu[m.group(1)] += 1
        total += 1
for f, c in tu.most_common(25):
    print(f"  {c:4d}  {f}")
print(f"  ... {len(tu)} TUs, {total} active asm-wrappers total")

print("\n=== small LEAF candidates (cleanest next targets) ===")
leaf = run("find_leaf_asm.py")
print(leaf.splitlines()[0] if leaf else "(none)")
