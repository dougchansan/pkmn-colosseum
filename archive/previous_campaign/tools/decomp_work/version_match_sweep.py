#!/usr/bin/env python3
"""For a big 0-match active-C file, compile it under each CW version and count
how many of its functions byte-match. If a version dramatically beats the
default, the TU was built with that version -> a reviewed per-file compiler
override could unlock the whole file (the library-import recipe applied to
game mega-files)."""
import re, subprocess, sys, os

ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"
FILES = sys.argv[1:] or ["src/game/colosseum_battle.c"]
VERS = ["1.2.5n", "1.3", "1.3.2", "1.3.2r", "2.0", "2.5", "2.6", "2.7"]

def count_matches(srcfile, ver):
    cmd = [sys.executable, "tools/compile_check.py", srcfile, "--diff",
           "--compiler-version", ver]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT, timeout=900)
    out = r.stdout + r.stderr
    if "COMPILE FAILED" in out or "Errors caused tool to abort" in out:
        return None, None, out.strip().splitlines()[-1] if out.strip() else "compile fail"
    # lines like: fn_XXXX   NN.NNNN%   a/b   m
    pcts = re.findall(r"fn_[0-9A-Fa-f]+\s+([0-9.]+)%", out)
    if not pcts:
        return None, None, "no fn rows parsed"
    total = len(pcts)
    matched = sum(1 for p in pcts if p == "100.0000")
    return matched, total, ""

for srcfile in FILES:
    print(f"\n==== {srcfile} : matches per CW version ====")
    best = (-1, None)
    for v in VERS:
        try:
            m, t, err = count_matches(srcfile, v)
        except subprocess.TimeoutExpired:
            print(f"  {v:8s}  TIMEOUT"); continue
        if m is None:
            print(f"  {v:8s}  -- ({err[:60]})")
        else:
            print(f"  {v:8s}  {m:>4}/{t} matched ({100.0*m/t:.1f}%)")
            if m > best[0]:
                best = (m, v)
    print(f"  BEST: {best[1]} with {best[0]} matches")
