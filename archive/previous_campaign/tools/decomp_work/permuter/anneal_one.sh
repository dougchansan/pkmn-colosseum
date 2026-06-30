#!/bin/bash
# anneal_one.sh — build a permuter dir for ONE function and run the search.
# usage (from WSL):  ./anneal_one.sh <fn_name> <src/game/file.c> [budget_s] [jobs]
# Emits one of:
#   WIN <fn>            (score-0 exact match found; winning C copied to wins/<fn>.c)
#   NOWIN <fn> <score>  (best score reached, no exact match)
#   FAIL <fn> <reason>  (dir build or compile baseline failed)
# Only writes under tools/decomp_work/permuter/. Never edits src/ or .inc.
set -uo pipefail
REPO="/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PERM="$REPO/tools/decomp_work/refs/decomp-permuter"
PDIR="$REPO/tools/decomp_work/permuter"
FN="$1"; SRC="$2"; BUDGET="${3:-360}"; JOBS="${4:-2}"
DIR="$PDIR/dirs/$FN"
mkdir -p "$PDIR/wins" "$PDIR/logs"

run_lowprio() {
  local -a cmd=("$@")
  if command -v ionice >/dev/null 2>&1; then
    cmd=(ionice -c3 "${cmd[@]}")
  fi
  if [ "${GRIND_NICE:-0}" != "0" ] && command -v nice >/dev/null 2>&1; then
    cmd=(nice -n "$GRIND_NICE" "${cmd[@]}")
  fi
  "${cmd[@]}"
}

# 1. build the dir (preprocess + isolate + assemble target.o + settings)
if ! run_lowprio bash "$PDIR/build_dir.sh" "$FN" "$SRC" > "$PDIR/logs/build_$FN.log" 2>&1; then
  echo "FAIL $FN build_dir"; exit 0
fi
[ -f "$DIR/base.c" ] && [ -f "$DIR/target.o" ] || { echo "FAIL $FN missing_base_or_target"; exit 0; }

# 2. run the permuter (time-boxed). --stop-on-zero halts on exact match.
( cd "$DIR" && run_lowprio timeout "$BUDGET" python3 "$PERM/permuter.py" . -j "$JOBS" \
    --stop-on-zero --best-only > "$PDIR/logs/run_$FN.log" 2>&1 )

# 3. harvest: a score-0 match is normally saved by the permuter as
# output-0-*/source.c. If the original base already scores 0, permuter exits
# before writing an output dir; in that case base.c is the exact match.
WIN=$(ls -d "$DIR"/output-0-* 2>/dev/null | head -1)
if [ -n "$WIN" ] && [ -f "$WIN/source.c" ]; then
  cp "$WIN/source.c" "$PDIR/wins/$FN.c"
  echo "WIN $FN"
elif grep -qE '(^|[^0-9-])(base score = 0|score = 0)([^0-9]|$)' "$PDIR/logs/run_$FN.log" 2>/dev/null; then
  cp "$DIR/base.c" "$PDIR/wins/$FN.c"
  echo "WIN $FN"
else
  BEST=$(grep -oE 'score = [0-9-]+' "$PDIR/logs/run_$FN.log" 2>/dev/null | grep -oE '[0-9-]+$' | sort -n | head -1)
  echo "NOWIN $FN ${BEST:-?}"
fi
