#!/bin/bash
# build_all.sh — build permuter dirs for the 5 target gs_field_world.c funcs.
set -uo pipefail
PDIR="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/permuter"
SRC="src/game/gs_field_world.c"
for FN in fn_80117D14 fn_8011A280 fn_8011A9EC fn_8011AB50 fn_80119BD0; do
  echo "==== building $FN ===="
  "$PDIR/build_dir.sh" "$FN" "$SRC" 2>&1 | grep -v "mwcceppc.exe Driver Error\|Cannot find my executable" | tail -2
done
echo "==== done ===="
