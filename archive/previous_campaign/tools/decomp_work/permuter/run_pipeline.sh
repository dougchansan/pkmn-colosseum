#!/bin/bash
# run_pipeline.sh — orchestrate STEP1 (find near-misses) + STEP2 (anneal top3)
# for ONE source file. Writes a machine-readable result block to RESULTS file.
# Only writes under tools/decomp_work/permuter/.
set -uo pipefail
REPO="/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PDIR="$REPO/tools/decomp_work/permuter"
SRC="src/game/gs_field_world.c"
BUDGET=420
JOBS=2
RES="$PDIR/pipeline_results.txt"
DIFF="$PDIR/diff_out.txt"
: > "$RES"

echo "STEP1 start $(date)" >> "$RES"
# Run compile_check --diff once, capture full output.
( cd "$REPO" && python tools/compile_check.py --diff "$SRC" ) > "$DIFF" 2>&1
echo "STEP1 exit=$? $(date)" >> "$RES"

# Parse: find lines containing fn_XXXXXXXX and a percentage. Keep 80<=p<100.
# We accept formats like "fn_8011XXXX  93.50%" or "... fn_8011XXXX ... 93.5% ...".
python3 - "$DIFF" >> "$RES" <<'PY'
import re,sys
txt=open(sys.argv[1],encoding="utf-8",errors="replace").read()
pat=re.compile(r'(fn_[0-9A-Fa-f]{8}).*?([0-9]+(?:\.[0-9]+)?)\s*%')
best={}
for line in txt.splitlines():
    for fn,p in pat.findall(line):
        p=float(p)
        # keep the max pct seen per fn
        if fn not in best or p>best[fn]:
            best[fn]=p
cands=[(fn,p) for fn,p in best.items() if 80.0<=p<100.0]
cands.sort(key=lambda x:-x[1])
print("CANDIDATES "+str(len(cands)))
for fn,p in cands:
    print(f"CAND {fn} {p}")
top=[fn for fn,_ in cands[:3]]
print("TOP "+(" ".join(top) if top else "NONE"))
open("/tmp/_top.txt","w").write("\n".join(top))
PY

TOP=$(cat /tmp/_top.txt 2>/dev/null)
if [ -z "$TOP" ]; then
  echo "NO_NEARMISSES" >> "$RES"
  echo "DONE $(date)" >> "$RES"
  exit 0
fi

echo "STEP2 start $(date)" >> "$RES"
for FN in $TOP; do
  echo "ANNEAL_START $FN $(date)" >> "$RES"
  OUT=$(cd "$PDIR" && bash anneal_one.sh "$FN" "$SRC" "$BUDGET" "$JOBS" 2>>"$PDIR/logs/orch_$FN.log")
  echo "ANNEAL_RESULT $OUT" >> "$RES"
  echo "ANNEAL_END $FN $(date)" >> "$RES"
done
echo "DONE $(date)" >> "$RES"
