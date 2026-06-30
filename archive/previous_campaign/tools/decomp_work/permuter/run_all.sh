#!/bin/bash
# run_all.sh — time-boxed permuter runs over the 5 gs_field_world.c near-misses.
#
# For each function: run permuter.py -j <J> --stop-on-zero for <BUDGET> seconds.
# The permuter writes any improved/zero candidate to dirs/<fn>/output-<score>-<n>/.
# A score-0 directory means an exact match; we copy its source.c to wins/<fn>.c.
PERM="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/refs/decomp-permuter"
ROOT="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/permuter"
DIRS="$ROOT/dirs"
WINS="$ROOT/wins"
LOGS="$ROOT/logs"
mkdir -p "$WINS" "$LOGS"

J="${J:-16}"
BUDGET="${BUDGET:-480}"   # seconds per function

cd "$PERM"
for FN in fn_80117D14 fn_8011A280 fn_8011A9EC fn_8011AB50 fn_80119BD0; do
  echo "===================================================================="
  echo "RUN $FN  (j=$J budget=${BUDGET}s)  $(date +%H:%M:%S)"
  # Clean stale outputs so we only see this run's wins.
  rm -rf "$DIRS/$FN"/output-* 2>/dev/null
  timeout "$BUDGET" python3 permuter.py -j "$J" --stop-on-zero --best-only \
      "$DIRS/$FN" > "$LOGS/$FN.log" 2>&1
  # Report best score seen + any score-0 win.
  best=$(ls -d "$DIRS/$FN"/output-* 2>/dev/null | sed -E 's/.*output-([0-9]+)-.*/\1/' | sort -n | head -1)
  echo "  best output score this run: ${best:-none}"
  if [ -d "$DIRS/$FN/output-0-1" ]; then
    cp "$DIRS/$FN/output-0-1/source.c" "$WINS/$FN.c"
    echo "  *** SCORE-0 WIN -> $WINS/$FN.c ***"
  fi
  grep -iE "Found zero|base score" "$LOGS/$FN.log" | head -2 | sed 's/^/  /'
done
echo "===================================================================="
echo "ALL RUNS DONE $(date +%H:%M:%S)"
echo "Win files:"; ls -la "$WINS" 2>/dev/null
