#!/bin/bash
# baselines.sh — print the permuter base score for each built function dir.
PERM="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/refs/decomp-permuter"
DIRS="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/permuter/dirs"
cd "$PERM"
for FN in fn_80117D14 fn_8011A280 fn_8011A9EC fn_8011AB50 fn_80119BD0; do
  line=$(python3 permuter.py --debug "$DIRS/$FN" 2>/dev/null | grep "base score")
  if [ -z "$line" ]; then
    err=$(python3 permuter.py --debug "$DIRS/$FN" 2>&1 | grep -iE "error|syntax|traceback" | head -1)
    echo "$FN : FAILED ($err)"
  else
    echo "$FN : $line"
  fi
done
