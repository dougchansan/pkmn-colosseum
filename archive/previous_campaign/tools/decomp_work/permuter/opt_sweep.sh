#!/bin/bash
# opt_sweep.sh — systematic per-fn optimization-level sweep (the KG
# `per-fn-opt-level-sweep` lever: permuter-INVISIBLE, can crack reg-alloc /
# scheduling walls in one pass). Builds the isolated permuter dir for <fn> (its
# REAL source C), compiles base.c at every -O level x {p,s}, scores each vs the
# .inc truth (target.o), and prints the best. A score of 0 = byte-exact at that
# opt level (an UNVERIFIED win — re-measure in-context before integrating).
#
# usage: opt_sweep.sh <fn_name> <src/rel/path.c>
# output (one line): <fn>  cur=<baseScore@O4,p>  best=<score>  opt=<level>  ZERO=<Y/N>
set -uo pipefail
SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SELF/../../.." && pwd)"
PERM="$REPO/tools/decomp_work/refs/decomp-permuter"
MWCC="$HOME/.cache/decomp-mwcc/GC/1.3/mwcceppc.exe"; [ -f "$MWCC" ] || MWCC="$REPO/tools/mwcc_compiler/GC/1.3/mwcceppc.exe"
WIBO="${WIBO:-$HOME/.local/bin/wibo}"; command -v wibo >/dev/null 2>&1 && WIBO="$(command -v wibo)"
OD="powerpc-linux-gnu-objdump -dr -EB -mpowerpc -M broadway"
FN="$1"; SRC="$2"; DIR="$SELF/dirs/$FN"

score(){ PYTHONPATH="$PERM" python3 - "$DIR/target.o" "$1" "$OD" <<'PY' 2>/dev/null
import sys
from src.scorer import Scorer
try:
    s=Scorer(sys.argv[1],stack_differences=False,algorithm="difflib",debug_mode=False,
             ign_branch_targets=True,objdump_command=sys.argv[3])
    print(s.score(sys.argv[2])[0])
except Exception: print(10**9)
PY
}

# build the isolated dir (real C body + target.o) — skip if already built
if [ ! -f "$DIR/base.c" ] || [ ! -f "$DIR/target.o" ]; then
  bash "$SELF/build_dir.sh" "$FN" "$SRC" >/dev/null 2>&1 || { echo "$FN  BUILD_DIR_FAILED"; exit 0; }
fi
[ -f "$DIR/base.c" ] && [ -f "$DIR/target.o" ] || { echo "$FN  NO_BASE_OR_TARGET"; exit 0; }

best=1000000000; bestopt="-"; cur="-"
for OPT in 4,p 0,p 1,p 2,p 3,p 0,s 1,s 2,s 3,s 4,s; do
  "$WIBO" "$MWCC" -O$OPT -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off \
     -use_lmw_stmw on -sdata 8 -sdata2 8 -i "$REPO/include" -c -o /tmp/osw_$FN.o "$DIR/base.c" 2>/dev/null || continue
  sc="$(score /tmp/osw_$FN.o)"
  [ "$OPT" = "4,p" ] && cur="$sc"
  if [ -n "$sc" ] && [ "$sc" -lt "$best" ] 2>/dev/null; then best="$sc"; bestopt="$OPT"; fi
done
rm -f /tmp/osw_$FN.o 2>/dev/null
Z=N; [ "$best" = 0 ] 2>/dev/null && Z=Y
echo "$FN  cur=$cur  best=$best  opt=$bestopt  ZERO=$Z"
