#!/usr/bin/env bash
# codex_to_opus.sh — migrate the codex lanes to Opus to conserve codex usage.
# Watches each codex pane via pane_io's build/hb/<NAME>.state; the moment a pane goes
# idle (its LAST codex run finished), respawns it as claude opus[1m] and adds it back
# to build/fleet_lanes.txt so the driver feeds it Opus work. Never interrupts a run.
# Uses txk (SIGKILL + mutex) so it shares the single psmux owner lock with pane_io.
# Exits when all listed panes are converted. Launch detached like fleet_up.
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
export MSYS_NO_PATHCONV=1
source tools/decomp_work/tmux_control/txk.sh

PIDF="build/.codex_to_opus.pid"
if [ -f "$PIDF" ] && kill -0 "$(cat "$PIDF" 2>/dev/null)" 2>/dev/null; then
  echo "[conv] already running ($(cat "$PIDF")) — exiting"; exit 0
fi
echo $$ > "$PIDF"; trap 'rm -f "$PIDF"' EXIT

LANES_FILE=build/fleet_lanes.txt
CD='cd /d C:\Users\douglaswhittingham\pkmn-colosseum'
OPUS_CMD="$CD && claude --model \"opus[1m]\" --dangerously-skip-permissions"
declare -A PANE=( [C1]=%4 [C2]=%10 [C4]=%11 [C5]=%12 [C6]=%13 [C7]=%14 [C8]=%15 )
PENDING="C1 C2 C4 C5 C6 C7 C8"
GRACE="${GRACE:-30}"   # require idle AND state fresh within this many seconds

echo "[conv] up — will convert to opus as each finishes its last run: $PENDING"
while [ -n "$(echo "$PENDING" | tr -d ' ')" ]; do
  NEXT=""
  now=$(date +%s)
  for n in $PENDING; do
    read -r st ts _ < "build/hb/$n.state" 2>/dev/null || { NEXT="$NEXT $n"; continue; }
    age=$(( now - ${ts:-0} ))
    if [ "$st" = idle ] && [ "$age" -le "$GRACE" ]; then
      txk respawn-pane -k -t "${PANE[$n]}" "cmd.exe"
      sleep 2
      txk send-keys -t "${PANE[$n]}" "$OPUS_CMD" Enter
      # add the lane back (dedup, atomic write so the driver never reads a half file)
      cur=$(cat "$LANES_FILE" 2>/dev/null)
      printf '%s %s\n' "$cur" "$n" | tr ' ' '\n' | awk 'NF && !seen[$0]++' | tr '\n' ' ' | sed 's/ *$/\n/' > "$LANES_FILE.tmp"
      mv -f "$LANES_FILE.tmp" "$LANES_FILE"
      echo "[conv] $n (${PANE[$n]}) -> opus[1m], re-added to lanes: $(cat "$LANES_FILE")"
    else
      NEXT="$NEXT $n"
    fi
  done
  PENDING=$(echo "$NEXT" | xargs)
  [ -n "$PENDING" ] && sleep 15
done
echo "[conv] DONE — all codex panes converted to opus[1m]"
