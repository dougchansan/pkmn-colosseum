#!/usr/bin/env bash
# permuter_poll.sh — poll the Windows WSL permuter swarm over Tailscale and write
# build/permuter_status.json for the dashboard. The permuter (anneal_supervisor +
# grind2.py) runs on the Windows box's CPU; this just reports its liveness/load.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
WIN="${PERMUTER_HOST:-win}"
KEY="${PERMUTER_KEY:-$HOME/.ssh/id_ed25519}"
OUT="build/permuter_status.json"
INTERVAL="${PERMUTER_POLL_INTERVAL:-60}"

# remote status script (base64'd to dodge ssh/wsl/bash quoting)
read -r -d '' REMOTE <<'EOF'
cd /mnt/c/Users/douglaswhittingham/pkmn-colosseum 2>/dev/null || exit 0
w=$(pgrep -fc grind2.py 2>/dev/null); : "${w:=0}"          # pgrep -c prints 0 itself; no ||echo (it would double up)
t=$(python3 -c "import json;print(len(json.load(open('.omc/permuter_queue.json'))))" 2>/dev/null || echo 0)
LOG=tools/decomp_work/permuter/logs/anneal_supervisor.out
last=$(tail -1 "$LOG" 2>/dev/null | tr -d '"\\' | tr -s ' ')
# liveness via log freshness (robust vs pgrep name quirks + the 5s inter-cycle gap)
now=$(date +%s); m=$(stat -c %Y "$LOG" 2>/dev/null || echo 0)
alive=false; { [ "${w:-0}" -gt 0 ] || [ $((now - m)) -lt 300 ]; } && alive=true
printf '{"alive":%s,"workers":%s,"targets":%s,"last":"%s"}\n' "$alive" "${w:-0}" "${t:-0}" "$last"
EOF
B64=$(printf '%s' "$REMOTE" | base64 | tr -d '\n')

echo "[permuter_poll] polling $WIN every ${INTERVAL}s -> $OUT"
while :; do
  js=$(ssh -o ConnectTimeout=20 -o ServerAliveInterval=5 -i "$KEY" "$WIN" \
        "C:\\Windows\\System32\\wsl.exe -e bash -c \"echo $B64 | base64 -d | bash\"" 2>/dev/null | tail -1)
  if printf '%s' "$js" | python3 -c "import sys,json;json.load(sys.stdin)" 2>/dev/null; then
    printf '%s' "$js" > "$OUT"
  else
    printf '{"alive":false,"workers":0,"targets":0,"last":"(unreachable)"}\n' > "$OUT"
  fi
  sleep "$INTERVAL"
done
