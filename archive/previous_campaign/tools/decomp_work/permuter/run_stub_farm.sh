#!/bin/bash
# run_stub_farm.sh — autonomous stub -> permuter farm.
#
# Reads crackable_stubs.txt (ranked stub candidates) and, for each function:
#   1. m2c_draft.py <fn> <src> --no-context     -> m2c_out/<fn>.c
#   2. m2c_clean.py                              -> compilable-ish C body
#   3. build_dir.sh <fn> <src>                   -> dirs/<fn>/ (base.c empty body + target.o)
#   4. inject_body.py                            -> real body into base.c
#   5. compile.sh base.c                         -> base.o (skip fn if this fails)
#   6. scorer (refs/.../src/scorer.py)           -> base_score (skip if INF / non-finite)
#   7. permuter.py . -j JOBS --stop-on-zero      -> timeboxed search (BUDGET sec)
#   8. harvest a score-0 output to wins/<fn>_stub.c
#   9. append one line to logs/stub_farm.log:
#        <fn>  compiled=Y/N  base_score=N  best_score=N  WIN=Y/N
#
# IDEMPOTENT: a fn already logged WIN=Y is skipped. Re-running re-attempts only
# fns that have not yet won (so you can grow BUDGET over successive passes).
#
# CPU-ONLY. Never touches the GPU. Runs compiles/search niced + ionice-idle so a
# co-resident training job keeps priority. Default JOBS=4 (cap to leave headroom).
#
# Only ever writes under tools/decomp_work/permuter/ (dirs/, wins/, logs/) and
# tools/decomp_work/m2c_out/. NEVER edits src/, the .inc truth files, or commits.
# A score-0 "WIN" here is UNVERIFIED — re-measure independently before integ.
#
# Usage:
#   ./run_stub_farm.sh [LIST] [N]
#     LIST  candidate list file        (default: crackable_stubs.txt next to this script)
#     N     process only the first N candidates (default: all)
#   env:
#     BUDGET  per-fn permuter seconds   (default 600)
#     JOBS    permuter worker threads   (default 4, hard-capped to 8)
#     ONLY    space-separated fn allowlist (process only these, in list order)
set -uo pipefail

SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SELF/../../.." && pwd)"
PERM="$REPO/tools/decomp_work/refs/decomp-permuter"
PDIR="$SELF"
DW="$REPO/tools/decomp_work"

LIST="${1:-$PDIR/crackable_stubs.txt}"
LIMIT="${2:-0}"
BUDGET="${BUDGET:-600}"
JOBS="${JOBS:-4}"
[ "$JOBS" -gt 8 ] && JOBS=8           # leave CPU headroom for the training box
ONLY="${ONLY:-}"

LOG="$PDIR/logs/stub_farm.log"
mkdir -p "$PDIR/wins" "$PDIR/logs" "$DW/m2c_out"
touch "$LOG"

OBJDUMP="powerpc-linux-gnu-objdump -dr -EB -mpowerpc -M broadway"

run_lowprio() {
  local -a cmd=("$@")
  command -v ionice >/dev/null 2>&1 && cmd=(ionice -c3 "${cmd[@]}")
  command -v nice   >/dev/null 2>&1 && cmd=(nice -n "${FARM_NICE:-15}" "${cmd[@]}")
  "${cmd[@]}"
}

# score <target.o> <cand.o> -> prints integer score (or 1000000000 on failure)
score_o() {
  local target="$1" cand="$2"
  PYTHONPATH="$PERM" python3 - "$target" "$cand" "$OBJDUMP" <<'PY'
import sys
from src.scorer import Scorer
target, cand, objdump = sys.argv[1], sys.argv[2], sys.argv[3]
try:
    s = Scorer(target, stack_differences=False, algorithm="difflib",
               debug_mode=False, ign_branch_targets=True, objdump_command=objdump)
    sc, _ = s.score(cand)
except Exception as e:
    sys.stderr.write(f"score error: {e}\n"); sc = Scorer.PENALTY_INF
print(sc)
PY
}

logline() { echo "$1" | tee -a "$LOG"; }

already_won() { grep -qE "^$1[[:space:]].*WIN=Y" "$LOG" 2>/dev/null; }

process_fn() {
  local FN="$1" SRC="$2"
  local DIR="$PDIR/dirs/$FN"

  if already_won "$FN"; then
    echo "skip $FN (already WIN in log)"; return
  fi

  echo "=== $FN  ($SRC)  BUDGET=${BUDGET}s JOBS=$JOBS ==="

  # 1+2. m2c draft -> clean body
  local draft="$DW/m2c_out/$FN.c" body="$DW/m2c_out/$FN.clean.c"
  if ! run_lowprio python3 "$DW/m2c_draft.py" "$FN" "$SRC" --no-context --quiet \
        > "$PDIR/logs/m2c_$FN.log" 2>&1; then
    logline "$FN  compiled=N  base_score=-  best_score=-  WIN=N  (m2c_draft_failed)"; return
  fi
  python3 "$PDIR/m2c_clean.py" "$draft" -o "$body" >/dev/null 2>&1 || {
    logline "$FN  compiled=N  base_score=-  best_score=-  WIN=N  (m2c_clean_failed)"; return; }

  # 3. build the permuter dir (scaffold: base.c empty body + target.o)
  if ! run_lowprio bash "$PDIR/build_dir.sh" "$FN" "$SRC" \
        > "$PDIR/logs/build_$FN.log" 2>&1; then
    logline "$FN  compiled=N  base_score=-  best_score=-  WIN=N  (build_dir_failed)"; return
  fi
  [ -f "$DIR/base.c" ] && [ -f "$DIR/target.o" ] || {
    logline "$FN  compiled=N  base_score=-  best_score=-  WIN=N  (no_base_or_target)"; return; }

  # 4. inject the cleaned body in place of the scrubbed empty body
  if ! python3 "$PDIR/inject_body.py" "$DIR/base.c" "$FN" "$body" \
        > "$PDIR/logs/inject_$FN.log" 2>&1; then
    logline "$FN  compiled=N  base_score=-  best_score=-  WIN=N  (inject_failed)"; return
  fi

  # 5. compile base.c -> base.o
  if ! ( cd "$DIR" && run_lowprio ./compile.sh base.c -o "$DIR/base.o" ) \
        > "$PDIR/logs/compile_$FN.log" 2>&1 || [ ! -s "$DIR/base.o" ]; then
    logline "$FN  compiled=N  base_score=-  best_score=-  WIN=N  (compile_failed)"; return
  fi

  # 6. baseline score; skip if non-finite (isolated harness can't see this fn)
  local BASE
  BASE="$(score_o "$DIR/target.o" "$DIR/base.o")"
  if [ -z "$BASE" ] || [ "$BASE" -ge 1000000000 ] 2>/dev/null; then
    logline "$FN  compiled=Y  base_score=INF  best_score=INF  WIN=N  (non_finite_base)"; return
  fi
  echo "$FN base_score=$BASE"

  # If the base already byte-matches, that's an immediate (unverified) win.
  if [ "$BASE" -eq 0 ] 2>/dev/null; then
    cp "$DIR/base.c" "$PDIR/wins/${FN}_stub.c"
    logline "$FN  compiled=Y  base_score=0  best_score=0  WIN=Y  (base_already_match)"; return
  fi

  # 7. timeboxed permuter search
  rm -rf "$DIR"/output-* 2>/dev/null
  ( cd "$DIR" && run_lowprio timeout "$BUDGET" python3 "$PERM/permuter.py" . \
       -j "$JOBS" --stop-on-zero --best-only ) \
       > "$PDIR/logs/run_$FN.log" 2>&1 || true

  # 8. harvest best. output-<score>-<ctr> dirs hold improved candidates.
  local BEST WINDIR
  BEST="$(ls -d "$DIR"/output-* 2>/dev/null \
          | sed -E 's#.*/output-(-?[0-9]+)-[0-9]+#\1#' | sort -n | head -1)"
  [ -z "$BEST" ] && BEST="$BASE"          # no improvement written -> base is best
  WINDIR="$(ls -d "$DIR"/output-0-* 2>/dev/null | head -1)"

  if [ -n "$WINDIR" ] && [ -f "$WINDIR/source.c" ]; then
    cp "$WINDIR/source.c" "$PDIR/wins/${FN}_stub.c"
    logline "$FN  compiled=Y  base_score=$BASE  best_score=0  WIN=Y"
  else
    logline "$FN  compiled=Y  base_score=$BASE  best_score=$BEST  WIN=N"
  fi
}

# ---- iterate the candidate list (skip comments / blanks) ----
count=0
while IFS=$'\t' read -r FN SRC _rest; do
  case "$FN" in ''|\#*) continue;; esac
  SRC="${SRC%%[[:space:]]*}"            # strip any trailing comment columns
  [ -n "$ONLY" ] && { case " $ONLY " in *" $FN "*) ;; *) continue;; esac; }
  process_fn "$FN" "$SRC"
  count=$((count + 1))
  [ "$LIMIT" -gt 0 ] && [ "$count" -ge "$LIMIT" ] && break
done < "$LIST"

echo "=== farm pass done ($count fns processed). log: $LOG ==="
