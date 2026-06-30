#!/usr/bin/env bash
# fleet_monitor.sh — live terminal dashboard for code lanes, data lanes, and remotes.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
LANES="${FLEET_LANES:-glm codex codex2 seed}"
DATA_LANES="${DATA_LANES:-glm codex codex2}"
QUEUE="${FLEET_QUEUE:-build/low_attack_queue.txt}"
START_HEAD="$(git rev-parse HEAD 2>/dev/null)"
INTERVAL="${MONITOR_INTERVAL:-8}"
DASH_PORT="${DASH_PORT:-8770}"
SEED_HOST="${DECOMP_GPU_HOST:-192.168.50.101}"
SEED_SERVER="${SEED_SERVER:-http://$SEED_HOST:8780/gen}"
printf '\033[?25l\033[2J'
cleanup() { printf '\033[?25h'; }
trap cleanup EXIT
trap 'cleanup; exit 130' INT TERM

alive_code() {
  local p
  [ "$1" = seed ] && p="lane_seed.py" || p="(^|[[:space:]/])lane_worker\.sh $1($|[[:space:]])"
  pgrep -f "$p" >/dev/null 2>&1 && echo up || echo "--"
}

alive_data() {
  pgrep -f "(^|[[:space:]/])data_lane_worker\.sh $1($|[[:space:]])" >/dev/null 2>&1 && echo up || echo "--"
}

json_summary() {
  python3 - "$DASH_PORT" <<'PY'
import json
import os
import sys
from pathlib import Path

root = Path.cwd()
port = sys.argv[1]

def load(path, default):
    try:
        return json.loads((root / path).read_text(encoding="utf-8"))
    except Exception:
        return default

report = load("report.json", {}).get("measures", {})
work = load("tools/decomp_work/data_sdata2_worklist.json", {}).get("metadata", {})
queue = load("tools/decomp_work/data_campaign_queue.json", {})
progress = load("config/GC6E01/data_progress.json", {})
win = load("build/permuter_status.json", {})
gpu = load("build/permuter_status_3090.json", {})

matched = int(report.get("matched_data") or 0)
total = int(report.get("total_data") or 0)
pct = (100.0 * matched / total) if total else 0.0
chunk_bytes = int(work.get("chunk_bytes") or 256)
done_chunks = 0
for item in progress.get("matched", []) or []:
    if item.get("section") == (work.get("section") or ".sdata2"):
        done_chunks += (int(item.get("size") or 0) + chunk_bytes - 1) // chunk_bytes
remaining_chunks = int(work.get("chunk_count") or 0)
lanes = (queue.get("metadata", {}) or {}).get("lanes", {}) or {}
top = []
for item in (queue.get("queue", []) or [])[:4]:
    top.append(f"{item.get('id')}:{item.get('lane')}:{item.get('start')}")

def perm_line(name, p):
    alive = "up" if p.get("alive") else "--"
    active = ",".join((p.get("active_targets") or [])[:4])
    gpu_txt = ""
    if p.get("gpu_mem_total"):
        gpu_txt = f" · gpu {p.get('gpu_util','?')}% {p.get('gpu_mem_used','?')}/{p.get('gpu_mem_total','?')} MiB"
    return (f"{name:<8} [{alive}] workers {p.get('workers','?')} j {p.get('jobs','?')} "
            f"active {p.get('active','?')} queued {p.get('queued','?')} done {p.get('done','?')} "
            f"wins {p.get('wins','?')}{gpu_txt} :: {active or str(p.get('last',''))[:78]}")

print(f"data    {matched:,}/{total:,} bytes ({pct:.4f}%) · entries {len(progress.get('matched', []) or [])} · "
      f".sdata2 {done_chunks}/{done_chunks + remaining_chunks} chunks · queue "
      f"N{int(lanes.get('NUMERIC',0))} S{int(lanes.get('STRING',0))} "
      f"L{int(lanes.get('LAYOUT',0))} R{int(lanes.get('RESEARCH',0))}")
print("next    " + ("  ".join(top) if top else "(queue empty)"))
print(perm_line("win", win))
print(perm_line("3090", gpu))
print(f"dash    http://127.0.0.1:{port}/  api={bool(report)}")
PY
}

data_claim_for() {
  local role="$1"
  for d in build/data_fleet_locks/*/; do
    [ -f "$d/owner" ] || continue
    [ "$(cat "$d/owner" 2>/dev/null)" = "$role" ] || continue
    python3 - "$d/item.json" <<'PY'
import json, sys
try:
    item = json.load(open(sys.argv[1]))
    print(f"{item.get('id')} {item.get('start')}..{item.get('end')} {item.get('lane')}")
except Exception:
    print("(claim)")
PY
  done 2>/dev/null | tail -1
}

while :; do
  total=$(grep -vcE '^\s*#|^\s*$' "$QUEUE" 2>/dev/null || echo 0)
  claimed=$(ls build/fleet_locks 2>/dev/null | wc -l | tr -d ' ')
  wins=$(ls build/band_wins/pl_*.json 2>/dev/null | wc -l | tr -d ' ')
  commits=$(git rev-list --count "${START_HEAD}..HEAD" 2>/dev/null || echo 0)
  byte_exact=$(git log "${START_HEAD}..HEAD" --oneline 2>/dev/null | grep -oE '\+[0-9]+ byte-exact' | grep -oE '[0-9]+' | awk '{s+=$1} END{print s+0}')
  pane_lines=$(tput lines 2>/dev/null || echo 24)

  printf '\033[H'
  if [ "${FLEET_MONITOR_COMPACT:-0}" = "1" ] || [ "$pane_lines" -lt 24 ]; then
    {
      printf 'DECOMP FLEET %s · %s · data/data-lanes/permuters\n' "$(date +%H:%M:%S)" "$(git branch --show-current 2>/dev/null)"
      json_summary | sed 's/^/  /'
      printf 'data lanes:\n'
      for r in $DATA_LANES; do
        claim="$(data_claim_for "$r")"
        last="$(tail -1 "build/data_lane_$r.log" 2>/dev/null | cut -c1-92)"
        printf '  %-6s [%2s] %-42s\n' "$r" "$(alive_data "$r")" "${claim:-(idle)}"
        printf '        %s\n' "${last:-...}"
      done
      printf 'commits: '
      git log -3 --oneline 2>/dev/null | awk 'NR==1 {printf "%s", $0; next} {printf " | %s", $0} END {print ""}'
      printf '\nrefresh %ss · dashboard 0.0.0.0:%s · stop: tools/decomp_work/fleet_down_mac.sh\n' "$INTERVAL" "$DASH_PORT"
    }
  else
    {
    printf '╔══ DECOMP FLEET ════════════════════════  %s  ══╗\n' "$(date +%H:%M:%S)"
    printf '  branch %s · commits this monitor %s · byte-exact +%s · band_wins tags %s\n' \
      "$(git branch --show-current 2>/dev/null)" "$commits" "$byte_exact" "$wins"
    printf '  low-bucket queue: %s files total · %s claimed · %s remaining\n' "$total" "$claimed" "$((total - claimed))"
    json_summary | sed 's/^/  /'

    printf '╠══ CODE LANES ══════════════════════════════════════════════╣\n'
    for r in $LANES; do
      file="$(for d in build/fleet_locks/*/; do [ -f "$d/owner" ] && [ "$(cat "$d/owner")" = "$r" ] && cat "$d/file"; done 2>/dev/null | tail -1)"
      last="$(tail -1 "build/lane_$r.log" 2>/dev/null | cut -c1-86)"
      printf '  %-7s [%2s] %-46s\n' "$r" "$(alive_code "$r")" "${file:-(idle)}"
      printf '          └ %s\n' "${last:-...}"
    done

    printf '╠══ DATA LANES ══════════════════════════════════════════════╣\n'
    for r in $DATA_LANES; do
      claim="$(data_claim_for "$r")"
      last="$(tail -1 "build/data_lane_$r.log" 2>/dev/null | cut -c1-86)"
      printf '  %-7s [%2s] %-46s\n' "$r" "$(alive_data "$r")" "${claim:-(idle)}"
      printf '          └ %s\n' "${last:-...}"
    done

    printf '╠══ RECENT GATE COMMITS ═════════════════════════════════════╣\n'
    git log -6 --oneline 2>/dev/null | sed 's/^/  /' || true
    printf '╚════════════════════════════════════════════════════════════╝\n'
    printf '  refresh %ss · web dashboard 0.0.0.0:%s · stop fleet: tools/decomp_work/fleet_down_mac.sh\n' "$INTERVAL" "$DASH_PORT"
    }
  fi | while IFS= read -r line; do
    printf '\033[2K%s\n' "$line"
  done
  printf '\033[J'
  sleep "$INTERVAL"
done
