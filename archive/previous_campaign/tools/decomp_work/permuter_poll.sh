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
LOG=tools/decomp_work/permuter/logs/anneal_supervisor.out
last=$(tail -1 "$LOG" 2>/dev/null | tr -d '"\\' | tr -s ' ')
# liveness via log freshness (robust vs pgrep name quirks + the 5s inter-cycle gap)
now=$(date +%s); m=$(stat -c %Y "$LOG" 2>/dev/null || echo 0)
python3 - "$last" "$now" "$m" <<'PY'
import json
import os
import subprocess
import sys

last, now_s, m_s = sys.argv[1], sys.argv[2], sys.argv[3]

def pgrep_count(pattern):
    try:
        out = subprocess.check_output(["pgrep", "-fc", pattern], text=True).strip()
        return int(out or "0")
    except Exception:
        return 0

def pgrep_first(pattern):
    try:
        out = subprocess.check_output(["pgrep", "-fo", pattern], text=True).strip()
        return int(out or "0")
    except Exception:
        return 0

def read_environ(pid):
    if not pid:
        return {}
    try:
        raw = open(f"/proc/{pid}/environ", "rb").read().split(b"\0")
    except Exception:
        return {}
    env = {}
    for item in raw:
        if b"=" in item:
            k, v = item.split(b"=", 1)
            env[k.decode(errors="replace")] = v.decode(errors="replace")
    return env

def load(path, default):
    try:
        with open(path, encoding="utf-8") as fh:
            return json.load(fh)
    except Exception:
        return default

state = load(".omc/permuter_state.json", {})
queue_file = load(".omc/permuter_queue.json", [])
active = state.get("active") or {}
done = state.get("done") or []
wins = state.get("wins") or []
grind_processes = pgrep_count("grind2.py")
permuter_processes = pgrep_count("permuter.py")
grind_env = read_environ(pgrep_first("grind2.py"))
alive = bool(grind_processes or permuter_processes)
try:
    alive = alive or (int(now_s) - int(m_s) < 300)
except Exception:
    pass
active_targets = sorted({v.get("fn") for v in active.values() if v.get("fn")})
workers = state.get("workers") or int(grind_env.get("GRIND_WORKERS") or grind_processes or 0)
jobs = state.get("jobs") or int(grind_env.get("GRIND_JOBS") or 0)
replicas = state.get("replicas") or int(grind_env.get("GRIND_REPLICAS") or 1)
budget = state.get("budget") or grind_env.get("GRIND_BUDGET")
try:
    budget = int(budget) if budget is not None else None
except Exception:
    budget = None
effective_slots = state.get("effective_slots")
if effective_slots is None and workers and jobs and replicas:
    effective_slots = workers * jobs * replicas
summary = {
    "machine": "windows",
    "alive": alive,
    "cores": state.get("cores"),
    "profile": state.get("profile"),
    "workers": workers,
    "jobs": jobs,
    "replicas": replicas,
    "budget": budget,
    "effective_slots": effective_slots,
    "active": len(active_targets),
    "active_targets": active_targets,
    "queued": len(state.get("queue") or []),
    "targets": len(queue_file),
    "done": len(done),
    "wins": len(wins),
    "grind_processes": grind_processes,
    "permuter_processes": permuter_processes,
    "last": last,
}
print(json.dumps(summary, separators=(",", ":")))
PY
EOF
B64=$(printf '%s' "$REMOTE" | base64 | tr -d '\n')

echo "[permuter_poll] polling $WIN every ${INTERVAL}s -> $OUT"
printf '\033[?25l\033[2J'
cleanup() { printf '\033[?25h'; }
trap cleanup EXIT
trap 'cleanup; exit 130' INT TERM
draw_status() {
  python3 - "$OUT" "$WIN" <<'PY'
import json
import sys
from pathlib import Path

path = Path(sys.argv[1])
name = sys.argv[2]
try:
    data = json.loads(path.read_text(encoding="utf-8"))
except Exception as exc:
    data = {"alive": False, "last": f"status read failed: {exc}"}

alive = "up" if data.get("alive") else "--"
targets = data.get("active_targets") or []
def val(key):
    value = data.get(key)
    return "?" if value is None else value
print(f"PERMUTER {name}  [{alive}]")
print("=" * 48)
print(f"workers {val('workers')}  jobs {val('jobs')}  slots {val('effective_slots')}  budget {val('budget')}s")
print(f"active {val('active')}  queued {val('queued')}/{val('targets')}  done {val('done')}  wins {val('wins')}")
print(f"processes grind={val('grind_processes')} permuter={val('permuter_processes')}  cores={val('cores')}")
print("targets " + (", ".join(targets[:5]) if targets else "(none)"))
print(str(data.get("last", ""))[:220])
PY
}
while :; do
  js=$(ssh -o ConnectTimeout=20 -o ServerAliveInterval=5 -i "$KEY" "$WIN" \
        "C:\\Windows\\System32\\wsl.exe -e bash -c \"echo $B64 | base64 -d | bash\"" 2>/dev/null | tail -1)
  if printf '%s' "$js" | python3 -c "import sys,json;json.load(sys.stdin)" 2>/dev/null; then
    printf '%s' "$js" > "$OUT"
  else
    printf '{"alive":false,"workers":0,"targets":0,"last":"(unreachable)"}\n' > "$OUT"
  fi
  printf '\033[H'
  draw_status
  printf '\033[J'
  sleep "$INTERVAL"
done
