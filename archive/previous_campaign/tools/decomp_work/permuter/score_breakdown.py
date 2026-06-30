#!/usr/bin/env python3
# score_breakdown.py — compile a permuter dir's base.c (or a given .c) with the
# project flags and print the Scorer penalty breakdown (regalloc vs reordering vs
# ins/del) against that dir's target.o. Used to characterize a near-miss as a
# PURE register-allocation wall (score = k*5, all REGALLOC) vs a scheduling wall
# (reordering, k*60) vs a structural miss (ins/del, k*100).
#
# usage:  python3 score_breakdown.py <dir> [src.c]
import os, sys, subprocess, tempfile

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PERM = f"{REPO}/tools/decomp_work/refs/decomp-permuter"
MWCC = f"{REPO}/tools/mwcc_compiler/GC/1.3/mwcceppc.exe"
INCLUDE = f"{REPO}/include"
sys.path.insert(0, PERM)
from src.scorer import Scorer

DIRP = sys.argv[1]
SRC = sys.argv[2] if len(sys.argv) > 2 else os.path.join(DIRP, "base.c")
TARGET_O = os.path.join(DIRP, "target.o")

def win(p):
    return subprocess.check_output(["wslpath", "-w", p]).decode().strip()

with tempfile.NamedTemporaryFile(suffix=".o", delete=False) as tf:
    out_o = tf.name
flags = ["-O4,p","-nodefaults","-proc","gekko","-fp","hard","-Cpp_exceptions","off",
         "-enum","int","-warn","off","-use_lmw_stmw","on","-sdata","8","-sdata2","8"]
cmd = [MWCC, *flags, "-i", win(INCLUDE), "-c", "-o", win(out_o), win(SRC)]
r = subprocess.run(cmd, capture_output=True)
if r.returncode != 0 or not os.path.exists(out_o) or os.path.getsize(out_o) == 0:
    print("COMPILE_FAIL")
    sys.stderr.write(r.stderr.decode(errors="replace"))
    sys.exit(1)

scorer = Scorer(
    TARGET_O,
    stack_differences=False,
    algorithm="difflib",
    debug_mode=True,
    ign_branch_targets=True,
    objdump_command="powerpc-linux-gnu-objdump -dr -EB -mpowerpc -M broadway",
)
score, _ = scorer.score(out_o)
print(f"\nFINAL_SCORE = {score}")
os.unlink(out_o)
