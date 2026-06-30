#!/bin/bash
# permute_box_solo.sh — solo permuter run for fn_80044630 ON THE 3090 BOX
# (independent of the dev-box run; both seeded from the shared best). No
# orchestrator loop -> nothing respawns. usage: permute_box_solo.sh [budget_s] [jobs]
set -uo pipefail
RB=/storage/finetune/pkmn-colosseum
PERM=$RB/tools/decomp_work/refs/decomp-permuter
DIR=$RB/tools/decomp_work/permuter/dirs/fn_80044630
LOG=$RB/tools/decomp_work/permuter/logs/run_box_solo.log
WINS=$RB/tools/decomp_work/permuter/wins
BUDGET=${1:-28800}; JOBS=${2:-22}
mkdir -p "$(dirname "$LOG")" "$WINS"
cd "$DIR"
grep -q 'pragma peephole off' base.c || python3 - <<'PY'
s=open("base.c").read().split("\n");o=[]
for ln in s:
    if ln.startswith("void fn_80044630(void)"): o.append("#pragma peephole off")
    o.append(ln)
o.append("#pragma peephole on");open("base.c","w").write("\n".join(o))
PY
rm -rf output-* 2>/dev/null || true
echo "=== box solo permuter start $(date -u +%FT%TZ) budget=${BUDGET}s jobs=${JOBS} ===" > "$LOG"
timeout "$BUDGET" python3 "$PERM/permuter.py" . -j "$JOBS" --best-only --stop-on-zero >> "$LOG" 2>&1
BEST=$(ls -d output-*-* 2>/dev/null | sed -E 's#.*output-([0-9]+)-.*#\1#' | sort -n | head -1)
WD=$(ls -d output-${BEST}-* 2>/dev/null | head -1)
if [ -n "${WD:-}" ] && [ -f "$WD/source.c" ]; then
  cp "$WD/source.c" "$WINS/fn_80044630_box_${BEST}.c"
  if [ "$BEST" = "0" ]; then echo "RESULT: WIN score 0 -> wins/fn_80044630_box_0.c" >> "$LOG"
  else echo "RESULT: BEST score=$BEST" >> "$LOG"; fi
else echo "RESULT: NOWIN" >> "$LOG"; fi
echo "=== box solo done $(date -u +%FT%TZ) ===" >> "$LOG"
