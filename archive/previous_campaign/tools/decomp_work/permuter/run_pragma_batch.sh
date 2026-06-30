#!/bin/bash
# run_pragma_batch.sh — drive anneal_pragma.sh across the validation target set.
# Each function gets the full pragma sweep (5 configs) within TOTAL budget.
# Writes a single master log line per function; wins land in wins/<fn>.c.
set -uo pipefail
PDIR="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/permuter"
TOTAL="${1:-900}"
JOBS="${2:-8}"
MASTER="$PDIR/logs/pragma_batch_master.log"
: > "$MASTER"

# fn  src
TARGETS=(
  "fn_8011A280 src/game/gs_field_world.c"
  "fn_8011A9EC src/game/gs_field_world.c"
  "fn_8011AB50 src/game/gs_field_world.c"
  "fn_800DCD98 src/game/gs_render.c"
  "fn_8000DD5C src/game/gs_npc_interact.c"
  "fn_8000DDE8 src/game/gs_npc_interact.c"
  "fn_800084C0 src/game/gs_task.c"
  "fn_80140A9C src/game/people/people_data.c"
)

echo "=== pragma batch start $(date) TOTAL=$TOTAL JOBS=$JOBS ===" | tee -a "$MASTER"
for t in "${TARGETS[@]}"; do
  set -- $t
  FN="$1"; SRC="$2"
  echo "==== $FN ($SRC) $(date +%H:%M:%S) ====" | tee -a "$MASTER"
  RES=$(bash "$PDIR/anneal_pragma.sh" "$FN" "$SRC" "$TOTAL" "$JOBS" 2>&1)
  echo "$RES" | tee -a "$MASTER"
  # final verdict line (WIN/NOWIN/FAIL) to the master for quick scanning
  echo "$RES" | grep -E '^(WIN|NOWIN|FAIL) ' | tail -1 >> "$MASTER"
done
echo "=== pragma batch done $(date) ===" | tee -a "$MASTER"
