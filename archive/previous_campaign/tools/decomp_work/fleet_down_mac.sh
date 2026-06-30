#!/usr/bin/env bash
# fleet_down_mac.sh — stop the Mac decomp fleet (kill lane workers + their agents +
# the gate loop). Leaves the cockpit tmux session up and all wins preserved.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
pkill -f 'lane_worker.sh' 2>/dev/null || true
pkill -f 'data_lane_worker.sh' 2>/dev/null || true
pkill -f 'lane_seed.py' 2>/dev/null || true
pkill -f 'data_seed_v3_probe.py' 2>/dev/null || true
pkill -f 'claude -p' 2>/dev/null || true
pkill -f 'codex exec' 2>/dev/null || true
pkill -f 'opencode run' 2>/dev/null || true
pkill -f 'fleet_dashboard.py' 2>/dev/null || true
pkill -f 'fleet_monitor.sh' 2>/dev/null || true
pkill -f 'permuter_poll.sh' 2>/dev/null || true
pkill -f 'permuter_poll_3090.sh' 2>/dev/null || true
[ -f build/.fleet_gate.pid ] && { kill "$(cat build/.fleet_gate.pid)" 2>/dev/null || true; rm -f build/.fleet_gate.pid; }
[ -f build/.fleet_dashboard.pid ] && { kill "$(cat build/.fleet_dashboard.pid)" 2>/dev/null || true; rm -f build/.fleet_dashboard.pid; }
[ -f build/.permuter_poll.pid ] && { kill "$(cat build/.permuter_poll.pid)" 2>/dev/null || true; rm -f build/.permuter_poll.pid; }
[ -f build/.permuter_poll_3090.pid ] && { kill "$(cat build/.permuter_poll_3090.pid)" 2>/dev/null || true; rm -f build/.permuter_poll_3090.pid; }
rm -rf build/fleet_locks build/data_fleet_locks build/locks
echo "[fleet] stopped — workers/agents/gate killed; cockpit session + band_wins preserved."
