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

echo "[fleet] launching lane workers (streaming live): $LANES"
for r in $LANES; do
  "$CK" send "$r" "bash tools/decomp_work/lane_worker.sh $r 2>&1 | tee -a build/lane_$r.log"
done

# 6th lane: the v3-model flywheel proposer (free local GPU). Ensure the inference
# server is up on the 3090, then run the propose-gate loop in the 'seed' pane.
SEED_HOST="${DECOMP_GPU_HOST:-192.168.50.101}"
if ! curl -s -m4 "http://$SEED_HOST:8780/" 2>/dev/null | grep -q seedcoder; then
  echo "[fleet] starting v3 inference server on $SEED_HOST ..."
  ssh -o ConnectTimeout=15 -i ~/.ssh/id_ed25519 "douglaswhittingham@$SEED_HOST" \
    "setsid bash /storage/finetune/start_v3.sh > /storage/finetune/serve_v3.log 2>&1 < /dev/null & disown" 2>/dev/null || true
fi
"$CK" send seed "python3 -u tools/decomp_work/lane_seed.py 2>&1 | tee -a build/lane_seed.log"
export FLEET_LANES="$LANES seed"

echo "[fleet] starting auto-gate loop (every ${GATE_EVERY}s) in the background"
( while :; do sleep "$GATE_EVERY"; bash tools/decomp_work/auto_gate.sh >> build/gate.log 2>&1; done ) >/dev/null 2>&1 &
echo $! > build/.fleet_gate.pid

echo "[fleet] starting web dashboard as a background daemon (port ${DASH_PORT:-8770}, 0.0.0.0)"
pkill -f fleet_dashboard.py 2>/dev/null || true
( python3 tools/decomp_work/fleet_dashboard.py --port "${DASH_PORT:-8770}" >build/dashboard.log 2>&1 & )
# 6th pane: terminal monitor (local quick-glance; the web dashboard is the main view)
"$CK" send shell "bash tools/decomp_work/fleet_monitor.sh"

TS_IP="$(/Applications/Tailscale.app/Contents/MacOS/Tailscale ip -4 2>/dev/null | head -1 || echo '<mac-tailscale-ip>')"
echo "[fleet] UP — $(echo $LANES | wc -w | tr -d ' ') lanes grinding the low bucket; agent output streams live."
echo "[fleet] DASHBOARD: http://${TS_IP}:${DASH_PORT:-8770}/   (open from Windows over Tailscale)"
echo "[fleet] attach: $CK attach   ·   stop: bash tools/decomp_work/fleet_down_mac.sh"
echo "[fleet] terminal dashboard alternative: bash tools/decomp_work/fleet_monitor.sh"
