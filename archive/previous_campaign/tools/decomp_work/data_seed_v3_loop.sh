#!/usr/bin/env bash
# data_seed_v3_loop.sh — readable SeedCoder v3 data-research pane.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1

INTERVAL="${SEED_DATA_INTERVAL:-900}"
SERVER="${SEED_SERVER:-http://${DECOMP_GPU_HOST:-192.168.50.101}:8780/gen}"
DONE="build/data_fleet_done"
LOG="build/data_seed_v3.log"
mkdir -p "$DONE" "$(dirname "$LOG")"

next_chunk() {
  python3 - "$DONE" <<'PY'
import json
import sys
from pathlib import Path

done = {p.name.split(".")[0] for p in Path(sys.argv[1]).glob("*.seedv3")}
queue = json.load(open("tools/decomp_work/data_campaign_queue.json", encoding="utf-8")).get("queue", [])
for item in queue:
    chunk_id = item.get("id")
    if chunk_id and chunk_id not in done:
        print(chunk_id)
        raise SystemExit(0)
print("")
PY
}

draw_header() {
  local chunk="$1"
  printf '\033[H'
  {
    printf 'SEEDCODER V3 DATA PROBE\n'
    printf '=======================\n'
    printf 'server  %s\n' "$SERVER"
    printf 'chunk   %s\n' "${chunk:-(none)}"
    printf 'policy  research-only; writes *_seedv3.md\n'
    printf 'sleep   %ss\n' "$INTERVAL"
    printf '\nrecent summaries:\n'
    grep 'DATA_SEED_SUMMARY' "$LOG" 2>/dev/null | tail -5 | python3 -c '
import json, sys
for line in sys.stdin:
    try:
        rec = json.loads(line.split(" ", 1)[1])
        print("  {0} {1}s {2} cand".format(
            rec.get("chunk_id", "?"),
            rec.get("seconds", "?"),
            rec.get("candidates", "?"),
        ))
    except Exception:
        print("  " + line.strip()[:70])
' || true
  } | while IFS= read -r line; do
    printf '\033[2K%s\n' "$line"
  done
  printf '\033[J'
}

printf '\033[?25l\033[2J'
cleanup() { printf '\033[?25h'; }
trap cleanup EXIT
trap 'cleanup; exit 130' INT TERM

while :; do
  chunk="$(next_chunk)"
  draw_header "$chunk"
  if [ -z "$chunk" ]; then
    sleep "$INTERVAL"
    continue
  fi
  {
    printf '[seed-v3:data] probing %s at %s\n' "$chunk" "$(date +%H:%M:%S)"
    python3 tools/decomp_work/data_seed_v3_probe.py --server "$SERVER" --chunk-id "$chunk" --n 1 --temp 0.2 --max-new 1200
    touch "$DONE/$chunk.seedv3"
  } 2>&1 | tee -a "$LOG"
  draw_header "$chunk"
  sleep "$INTERVAL"
done
