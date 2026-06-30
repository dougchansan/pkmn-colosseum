#!/usr/bin/env bash
# permuter_poll_3090.sh — poll the Linux 3090 permuter swarm and GPU state.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
HOST="${PERMUTER_3090_HOST:-${DECOMP_GPU_HOST:-192.168.50.101}}"
USER_HOST="${PERMUTER_3090_USER_HOST:-douglaswhittingham@$HOST}"
KEY="${PERMUTER_3090_KEY:-$HOME/.ssh/id_ed25519}"
REPO="${PERMUTER_3090_REPO:-/storage/finetune/pkmn-colosseum}"
OUT="build/permuter_status_3090.json"
INTERVAL="${PERMUTER_3090_POLL_INTERVAL:-60}"

read -r -d '' REMOTE <<'EOF'
cd "${REPO:-/storage/finetune/pkmn-colosseum}" 2>/dev/null || exit 0
LOG=tools/decomp_work/permuter/logs/anneal_supervisor.out
[ -f "$LOG" ] || LOG=tools/decomp_work/permuter/logs/anneal_3090_grind2.out
last=$(tail -1 "$LOG" 2>/dev/null | tr -d '"\\' | tr -s ' ')
now=$(date +%s); m=$(stat -c %Y "$LOG" 2>/dev/null || echo 0)
python3 - "$last" "$now" "$m" <<'PY'
import json
import subprocess
import sys

last, now_s, m_s = sys.argv[1], sys.argv[2], sys.argv[3]

def pgrep_count(pattern):
    try:
        out = subprocess.check_output(["pgrep", "-fc", pattern], text=True).strip()
        return int(out or "0")
    except Exception:
        return 0

def load(path, default):
    try:
        with open(path, encoding="utf-8") as fh:
            return json.load(fh)
    except Exception:
        return default

def gpu_status():
    try:
        out = subprocess.check_output([
            "nvidia-smi",
            "--query-gpu=utilization.gpu,memory.used,memory.total",
            "--format=csv,noheader,nounits",
        ], text=True, timeout=5).strip().splitlines()[0]
        util, used, total = [int(part.strip()) for part in out.split(",")[:3]]
        return {"gpu_util": util, "gpu_mem_used": used, "gpu_mem_total": total}
    except Exception:
        return {}

state = load(".omc/permuter_state.json", {})
queue_file = load(".omc/permuter_queue.json", [])
active = state.get("active") or {}
done = state.get("done") or []
wins = state.get("wins") or []
grind_processes = pgrep_count("grind2.py")
permuter_processes = pgrep_count("permuter.py")
serve_v3_processes = pgrep_count("serve_v3.py")
alive = bool(grind_processes or permuter_processes)
try:
    alive = alive or (int(now_s) - int(m_s) < 300)
except Exception:
    pass
active_targets = sorted({v.get("fn") for v in active.values() if v.get("fn")})
workers = state.get("workers") or grind_processes
jobs = state.get("jobs")
replicas = state.get("replicas")
effective_slots = state.get("effective_slots")
if effective_slots is None and workers and jobs and replicas:
    effective_slots = workers * jobs * replicas
summary = {
    "machine": "3090",
    "alive": alive,
    "cores": state.get("cores"),
    "profile": state.get("profile"),
    "workers": workers,
    "jobs": jobs,
    "replicas": replicas,
    "budget": state.get("budget"),
    "effective_slots": effective_slots,
    "active": len(active_targets),
    "active_targets": active_targets,
    "queued": len(state.get("queue") or []),
    "targets": len(queue_file),
    "done": len(done),
    "wins": len(wins),
    "grind_processes": grind_processes,
    "permuter_processes": permuter_processes,
    "serve_v3_processes": serve_v3_processes,
    "last": last,
}
summary.update(gpu_status())
print(json.dumps(summary, separators=(",", ":")))
PY
EOF
B64=$(printf '%s' "$REMOTE" | base64 | tr -d '\n')

echo "[permuter_poll_3090] polling $USER_HOST every ${INTERVAL}s -> $OUT"
printf '\033[?25l\033[2J'
cleanup() { printf '\033[?25h'; }
trap cleanup EXIT
trap 'cleanup; exit 130' INT TERM
draw_status() {
  python3 - "$OUT" "$USER_HOST" <<'PY'
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
gpu = ""
if data.get("gpu_mem_total"):
    gpu = f"  gpu {data.get('gpu_util','?')}%  vram {data.get('gpu_mem_used','?')}/{data.get('gpu_mem_total','?')} MiB"
print(f"PERMUTER 3090  [{alive}]{gpu}")
print("=" * 48)
print(f"host {name}")
print(f"workers {val('workers')}  jobs {val('jobs')}  slots {val('effective_slots')}  budget {val('budget')}s")
print(f"active {val('active')}  queued {val('queued')}/{val('targets')}  done {val('done')}  wins {val('wins')}")
print(f"processes grind={val('grind_processes')} permuter={val('permuter_processes')} seedv3={val('serve_v3_processes')}  cores={val('cores')}")
print("targets " + (", ".join(targets[:5]) if targets else "(none)"))
print(str(data.get("last", ""))[:240])
PY
}
while :; do
  quoted_repo=$(printf '%q' "$REPO")
  js=$(ssh -o ConnectTimeout=20 -o ServerAliveInterval=5 -i "$KEY" "$USER_HOST" \
        "export REPO=$quoted_repo; echo $B64 | base64 -d | bash" 2>/dev/null | tail -1)
  if printf '%s' "$js" | python3 -c "import sys,json;json.load(sys.stdin)" 2>/dev/null; then
    printf '%s\n' "$js" > "$OUT"
  else
    printf '{"machine":"3090","alive":false,"workers":0,"targets":0,"last":"(unreachable)"}\n' > "$OUT"
  fi
  printf '\033[H'
  draw_status
  printf '\033[J'
  sleep "$INTERVAL"
done
