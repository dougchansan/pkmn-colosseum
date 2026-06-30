#!/bin/bash
# classify.sh — for each "fn src" line given on stdin, build the permuter dir (if
# needed), score base.c, and classify the near-miss by penalty composition:
#   PURE_REGALLOC  : score = k*5, only Register Differences  -> annealer-hopeless
#   REGALLOC+SCHED : has Reorderings (k*60)                  -> partly scheduling
#   STRUCTURAL     : has Insertions/Deletions (k*100)        -> wrong shape; tractable
# Reads only; writes under permuter/ only.
set -uo pipefail
PDIR="/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/permuter"
while read -r FN SRC; do
  [ -z "${FN:-}" ] && continue
  DIR="$PDIR/dirs/$FN"
  if [ ! -f "$DIR/base.c" ] || [ ! -f "$DIR/target.o" ]; then
    bash "$PDIR/build_dir.sh" "$FN" "$SRC" >/dev/null 2>&1 || { printf '%-14s BUILD_FAIL\n' "$FN"; continue; }
  fi
  OUT=$(python3 "$PDIR/score_breakdown.py" "$DIR" 2>/dev/null)
  reg=$(echo "$OUT" | grep -oE 'Register Differences: *[0-9]+' | grep -oE '[0-9]+$')
  reo=$(echo "$OUT" | grep -oE 'Reorderings: *[0-9]+' | grep -oE '[0-9]+$')
  ins=$(echo "$OUT" | grep -oE 'Insertions: *[0-9]+' | grep -oE '[0-9]+$')
  del=$(echo "$OUT" | grep -oE 'Deletions: *[0-9]+' | grep -oE '[0-9]+$')
  fin=$(echo "$OUT" | grep -oE 'FINAL_SCORE = [0-9]+' | grep -oE '[0-9]+$')
  reg=${reg:-?}; reo=${reo:-?}; ins=${ins:-?}; del=${del:-?}; fin=${fin:-FAIL}
  cls="?"
  if [ "$fin" = "FAIL" ]; then cls="COMPILE_FAIL"
  elif [ "$ins" != "0" ] || [ "$del" != "0" ]; then cls="STRUCTURAL"
  elif [ "$reo" != "0" ]; then cls="REGALLOC+SCHED"
  elif [ "$reg" != "0" ]; then cls="PURE_REGALLOC"
  elif [ "$fin" = "0" ]; then cls="MATCH(0)"
  fi
  printf '%-14s score=%-5s reg=%-3s reorder=%-3s ins=%-3s del=%-3s -> %s\n' \
    "$FN" "$fin" "$reg" "$reo" "$ins" "$del" "$cls"
done
