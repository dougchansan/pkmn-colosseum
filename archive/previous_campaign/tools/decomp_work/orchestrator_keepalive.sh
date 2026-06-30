#!/bin/bash
# orchestrator_keepalive.sh — keeps the Codex orchestrator (pane %9) cycling.
# A Codex agent idles after each turn; this detached loop re-pokes it with an
# "orchestration round" whenever it goes idle. Run detached (setsid nohup or a
# dedicated tmux session).
#
# Cadence is deliberately SLOW and rate-limit aware: the 3 codex panes (%3/%8/%9)
# share ONE Codex usage pool, so hammering %9 every couple minutes just burns the
# shared quota and, once the cap is hit, slams a wall over and over. Instead we:
#   - only POKE every POKE_INTERVAL seconds (default 900 = 15 min)
#   - GLANCE every CHECK_INTERVAL seconds (default 180) to notice busy/limit state
#   - on "usage limit / try again at H:MM" -> sleep until the parsed reset (+buffer),
#     so we go quiet for the whole cooldown instead of re-poking.
# Tune via env: KEEPALIVE_POKE_INTERVAL, KEEPALIVE_CHECK_INTERVAL, KEEPALIVE_RL_BACKOFF.
ORCH="${1:-%9}"
POKE_INTERVAL="${KEEPALIVE_POKE_INTERVAL:-900}"
CHECK_INTERVAL="${KEEPALIVE_CHECK_INTERVAL:-180}"
RL_BACKOFF_DEFAULT="${KEEPALIVE_RL_BACKOFF:-1800}"
LOG=/mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/overnight/logs/orchestrator_keepalive.log
ROUND="Orchestration round (per docs/ORCHESTRATOR_HANDOFF.md): (1) capture %3 %4 %6 %8; re-dispatch ANY idle agent via tmux send-keys with its next function/task and 'keep grinding, do not idle'. (2) If %8 (PC-port) rebuilt a probe exe, run the batch via PowerShell (edit build_pc/_runbatch.ps1 \$exe to newest pcport_motion_probe_headless_*.exe; powershell.exe -NoProfile -ExecutionPolicy Bypass -File C:\\Users\\douglaswhittingham\\pkmn-colosseum\\build_pc\\_runbatch.ps1; read build_pc/_batch_out.txt) and paste pkx+char counts to %8. (3) Keep DeepSeek %6 fed (requeue failed<=400 to queued in tasks.json if queue-empty). (4) Periodically commit+push checkpoints to origin. (5) Work the priority TODO: finish pkx batch -> battle Colosseum -> fix non-winning annealer + DeepSeek. Then finish this turn; you'll be poked for the next round."

log() { echo "$(date +%H:%M:%S) $*" >> "$LOG"; }
log "keepalive start: orch=$ORCH poke=${POKE_INTERVAL}s check=${CHECK_INTERVAL}s rl_backoff=${RL_BACKOFF_DEFAULT}s"

last_poke=0
while true; do
  now=$(date +%s)
  cap=$(tmux capture-pane -t "$ORCH" -p -S -25 2>/dev/null)
  tail8=$(echo "$cap" | grep -v '^$' | tail -8)

  # 1. Busy? Leave it alone.
  if echo "$tail8" | grep -q "esc to interrupt"; then
    sleep "$CHECK_INTERVAL"; continue
  fi

  # 2. Rate-limited / usage cap? Back off until the reset time the CLI prints.
  if echo "$cap" | grep -qiE "usage limit|hit your usage|try again at|rate.?limit|429|quota"; then
    when=$(echo "$cap" | grep -oiE "try again at [0-9]{1,2}:[0-9]{2} ?(am|pm)?" | tail -1 | sed -E 's/.*try again at //I')
    backoff="$RL_BACKOFF_DEFAULT"
    if [ -n "$when" ]; then
      tgt=$(date -d "$when" +%s 2>/dev/null)
      if [ -n "$tgt" ]; then
        [ "$tgt" -le "$now" ] && tgt=$((tgt + 86400))   # parsed time already passed today -> tomorrow
        diff=$((tgt - now + 120))                        # +2 min buffer past the reset
        [ "$diff" -gt 0 ] && backoff="$diff"
      fi
    fi
    log "RATE-LIMITED (reset~'${when:-unknown}') -> quiet for ${backoff}s"
    sleep "$backoff"
    continue
  fi

  # 3. Idle + healthy: poke, but only every POKE_INTERVAL.
  if [ $((now - last_poke)) -ge "$POKE_INTERVAL" ]; then
    log "orchestrator idle -> poking a round"
    tmux send-keys -t "$ORCH" C-u; sleep 1
    tmux send-keys -t "$ORCH" -l "$ROUND"; sleep 1
    tmux send-keys -t "$ORCH" Enter; sleep 2; tmux send-keys -t "$ORCH" Enter
    last_poke="$now"
  fi
  sleep "$CHECK_INTERVAL"
done
