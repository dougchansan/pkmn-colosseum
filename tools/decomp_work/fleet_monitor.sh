#!/usr/bin/env bash
# fleet_monitor.sh — live dashboard for the Mac decomp fleet. Refreshes in place:
#   - committed wins on the campaign branch (this run)
#   - low-bucket queue progress (claimed / remaining / total)
#   - per-lane: current file, agent liveness, last log line
#   - recent auto-gate commits
# Run in a cockpit pane:  bash tools/decomp_work/fleet_monitor.sh
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
LANES="${FLEET_LANES:-opus glm codex codex2 sonnet}"
QUEUE="${FLEET_QUEUE:-build/low_attack_queue.txt}"
START_HEAD="$(git rev-parse HEAD 2>/dev/null)"
INTERVAL="${MONITOR_INTERVAL:-8}"

alive() { pgrep -f "lane_worker\.sh $1\$" >/dev/null 2>&1 && echo up || echo "--"; }

while :; do
  total=$(grep -vcE '^\s*#|^\s*$' "$QUEUE" 2>/dev/null || echo 0)
  claimed=$(ls build/fleet_locks 2>/dev/null | wc -l | tr -d ' ')
  wins=$(ls build/band_wins/pl_*.json 2>/dev/null | wc -l | tr -d ' ')
  commits=$(git rev-list --count "${START_HEAD}..HEAD" 2>/dev/null || echo 0)
  byte_exact=$(git log "${START_HEAD}..HEAD" --oneline 2>/dev/null | grep -oE '\+[0-9]+ byte-exact' | grep -oE '[0-9]+' | awk '{s+=$1} END{print s+0}')

  clear
  printf '╔══ DECOMP FLEET ════════════════════════  %s  ══╗\n' "$(date +%H:%M:%S)"
  printf '  branch %s\n' "$(git branch --show-current 2>/dev/null)"
  printf '  WINS committed this run: %s commit(s)  (+%s byte-exact)   band_wins tags: %s\n' "$commits" "$byte_exact" "$wins"
  printf '  low-bucket queue: %s files total · %s claimed · %s remaining\n' "$total" "$claimed" "$((total - claimed))"
  printf '╠══ LANES ═══════════════════════════════════════════════════╣\n'
  for r in $LANES; do
    file="$(for d in build/fleet_locks/*/; do [ -f "$d/owner" ] && [ "$(cat "$d/owner")" = "$r" ] && cat "$d/file"; done 2>/dev/null | tail -1)"
    last="$(tail -1 "build/lane_$r.log" 2>/dev/null | cut -c1-58)"
    printf '  %-7s [%2s] %-34s\n' "$r" "$(alive "$r")" "${file:-(idle)}"
    printf '          └ %s\n' "${last:-...}"
  done
  printf '╠══ RECENT GATE COMMITS ═════════════════════════════════════╣\n'
  git log "${START_HEAD}..HEAD" --oneline 2>/dev/null | head -5 | sed 's/^/  /' || true
  printf '╚════════════════════════════════════════════════════════════╝\n'
  printf '  (refresh %ss · attach-detach: Ctrl-b d · stop fleet: tools/decomp_work/fleet_down_mac.sh)\n' "$INTERVAL"
  sleep "$INTERVAL"
done
