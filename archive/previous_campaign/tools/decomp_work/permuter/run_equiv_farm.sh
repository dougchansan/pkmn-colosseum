#!/bin/bash
# run_equiv_farm.sh — idle-CPU permuter farm over equivalent.txt reg-alloc/
# scheduling walls (correct C, tiny residual, mostly never permuter-tested).
# Unlike the stub farm there is NO m2c step: build_dir.sh extracts the function's
# REAL source C. Per fn: build_dir -> opt-sweep (instant-win screen) -> timeboxed
# permuter -> harvest score-0. A score-0 is UNVERIFIED (re-measure in-context).
#
# usage: ./run_equiv_farm.sh [LIST] [N]   env: BUDGET (permuter s, def 1200) JOBS (def 6, cap 8)
set -uo pipefail
SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SELF/../../.." && pwd)"
PERM="$REPO/tools/decomp_work/refs/decomp-permuter"
LIST="${1:-$SELF/optsweep_candidates.txt}"; LIMIT="${2:-0}"
BUDGET="${BUDGET:-1200}"; JOBS="${JOBS:-6}"; [ "$JOBS" -gt 8 ] && JOBS=8
LOG="$SELF/logs/equiv_farm.log"; mkdir -p "$SELF/wins" "$SELF/logs"; touch "$LOG"
OD="powerpc-linux-gnu-objdump -dr -EB -mpowerpc -M broadway"
lp(){ command -v nice>/dev/null&&set -- nice -n15 "$@"; command -v ionice>/dev/null&&set -- ionice -c3 "$@"; "$@"; }
already(){ grep -qE "^$1[[:space:]].*WIN=Y" "$LOG" 2>/dev/null; }

proc(){
  local FN="$1" SRC="$2" PCT="$3" DIR="$SELF/dirs/$FN"
  already "$FN" && { echo "skip $FN (won)"; return; }
  echo "=== $FN ($SRC) ${PCT}% BUDGET=${BUDGET}s ==="
  # 1. build the isolated dir (real source C)
  if [ ! -f "$DIR/base.c" ] || [ ! -f "$DIR/target.o" ]; then
    lp bash "$SELF/build_dir.sh" "$FN" "$SRC" >"$SELF/logs/build_$FN.log" 2>&1 || true
  fi
  [ -f "$DIR/base.c" ] && [ -f "$DIR/target.o" ] || { echo "$FN  pct=$PCT  build=N  opt=-  best=-  WIN=N (build_failed)"|tee -a "$LOG"; return; }
  # 2. opt-sweep instant-win screen
  local SW; SW="$(lp bash "$SELF/opt_sweep.sh" "$FN" "$SRC" 2>/dev/null | tail -1)"
  local base="$(echo "$SW"|grep -oE 'cur=[0-9]+'|cut -d= -f2)"
  local oz="$(echo "$SW"|grep -oE 'ZERO=[YN]'|cut -d= -f2)"
  if [ "$oz" = Y ]; then
    cp "$DIR/base.c" "$SELF/wins/${FN}_equiv_optsweep.c"
    echo "$FN  pct=$PCT  build=Y  opt=$(echo "$SW"|grep -oE 'opt=[0-9,ps]+')  best=0  WIN=Y (opt-sweep)"|tee -a "$LOG"; return
  fi
  # 3. timeboxed permuter at default -O4,p
  rm -rf "$DIR"/output-* 2>/dev/null
  ( cd "$DIR" && lp timeout "$BUDGET" python3 "$PERM/permuter.py" . -j "$JOBS" --stop-on-zero --best-only ) \
     >"$SELF/logs/run_$FN.log" 2>&1 || true
  local BEST WIN
  BEST="$(ls -d "$DIR"/output-* 2>/dev/null|sed -E 's#.*/output-(-?[0-9]+)-[0-9]+#\1#'|sort -n|head -1)"
  [ -z "$BEST" ] && BEST="${base:-?}"
  WIN="$(ls -d "$DIR"/output-0-* 2>/dev/null|head -1)"
  if [ -n "$WIN" ] && [ -f "$WIN/source.c" ]; then
    cp "$WIN/source.c" "$SELF/wins/${FN}_equiv.c"
    echo "$FN  pct=$PCT  build=Y  optbase=$base  best=0  WIN=Y (permuter)"|tee -a "$LOG"
  else
    echo "$FN  pct=$PCT  build=Y  optbase=$base  best=$BEST  WIN=N"|tee -a "$LOG"
  fi
}

n=0
while IFS=$'\t' read -r FN SRC PCT _; do
  case "$FN" in ''|\#*) continue;; esac
  proc "$FN" "$SRC" "$PCT"
  n=$((n+1)); [ "$LIMIT" -gt 0 ] && [ "$n" -ge "$LIMIT" ] && break
done < "$LIST"
echo "=== equiv farm done ($n fns) ==="
