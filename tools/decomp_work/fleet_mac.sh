#!/usr/bin/env bash
# fleet_mac.sh — launch the Mac low-bucket decomp fleet.
#   - brings up the 6-pane cockpit (panes as shells, no interactive TUIs)
#   - runs lane_worker.sh in each of the 5 agent panes (opus glm codex codex2 sonnet)
#   - runs auto_gate.sh on a loop in the shell pane (apply + commit verified wins)
# Each lane claims its own file from build/low_attack_queue.txt (atomic lock), so
# the 5 agents grind in parallel without collisions. Attach to watch:
#   tools/decomp_work/tmux_control/cockpit.sh attach
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
CK=tools/decomp_work/tmux_control/cockpit.sh
GATE_EVERY="${GATE_EVERY:-300}"   # seconds between auto-gate passes
LANES="${FLEET_LANES:-opus glm codex codex2 sonnet}"

echo "[fleet] regenerating bucket queue..."
python3 tools/decomp_work/gen_bucket_queue.py >/dev/null 2>&1 || true
nfiles=$(grep -vcE '^\s*#|^\s*$' build/low_attack_queue.txt 2>/dev/null || echo 0)
echo "[fleet] low_attack_queue: $nfiles files"

# fresh locks each run (a prior run's claims shouldn't block this one)
rm -rf build/fleet_locks; mkdir -p build/fleet_locks build/band_wins

echo "[fleet] bringing up cockpit (panes as shells)..."
"$CK" kill >/dev/null 2>&1 || true
DECOMP_NO_AGENTS=1 "$CK" bootstrap
sleep 3

echo "[fleet] launching lane workers: $LANES"
for r in $LANES; do
  "$CK" send "$r" "bash tools/decomp_work/lane_worker.sh $r 2>&1 | tee -a build/lane_$r.log"
done

echo "[fleet] starting auto-gate loop (every ${GATE_EVERY}s) in the shell pane"
"$CK" send shell "while :; do sleep $GATE_EVERY; bash tools/decomp_work/auto_gate.sh 2>&1 | tee -a build/gate.log; done"

echo "[fleet] UP — $(echo $LANES | wc -w | tr -d ' ') lanes grinding the low bucket, auto-gate every ${GATE_EVERY}s."
echo "[fleet] attach:  $CK attach     logs: build/lane_<role>.log, build/gate.log"
