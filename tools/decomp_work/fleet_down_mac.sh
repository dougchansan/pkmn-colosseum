#!/usr/bin/env bash
# fleet_down_mac.sh — stop the Mac decomp fleet (kill lane workers + their agents +
# the gate loop). Leaves the cockpit tmux session up and all wins preserved.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
pkill -f 'lane_worker.sh' 2>/dev/null || true
pkill -f 'lane_seed.py' 2>/dev/null || true
pkill -f 'claude -p' 2>/dev/null || true
pkill -f 'codex exec' 2>/dev/null || true
pkill -f 'opencode run' 2>/dev/null || true
pkill -f 'fleet_dashboard.py' 2>/dev/null || true
pkill -f 'fleet_monitor.sh' 2>/dev/null || true
[ -f build/.fleet_gate.pid ] && { kill "$(cat build/.fleet_gate.pid)" 2>/dev/null || true; rm -f build/.fleet_gate.pid; }
echo "[fleet] stopped — workers/agents/gate killed; cockpit session + band_wins preserved."
