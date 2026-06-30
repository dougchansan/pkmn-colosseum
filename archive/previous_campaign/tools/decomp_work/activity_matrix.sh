#!/bin/bash
# activity_matrix.sh — RE-agent activity feed. Scrolling history shows REAL
# EVENTS only (new git commits, worker WIN/FAIL/NEW-BEST lines); the annealer
# heartbeat is compressed into ONE in-place-updating status line (carriage-
# return refresh) that only repaints when the state actually CHANGES.
REPO=/mnt/c/Users/douglaswhittingham/pkmn-colosseum
cd "$REPO" 2>/dev/null || exit 1
B=$'\e[1m'; G=$'\e[38;5;46m'; C=$'\e[38;5;51m'; Y=$'\e[38;5;226m'
M=$'\e[38;5;201m'; RD=$'\e[38;5;196m'; D=$'\e[38;5;240m'; R=$'\e[0m'

cols() { tput cols 2>/dev/null || echo "${COLUMNS:-80}"; }

# Scroll one event line into history: clear the status line first, then print.
# $1 = color, $2 = 8-char tag, $3 = plain payload (truncated to pane width)
event() {
  local w max payload
  w=$(cols); max=$(( w - 20 )); [ "$max" -lt 16 ] && max=16
  payload=$(printf '%s' "$3" | tr -d '\n\r' | cut -c1-"$max")
  printf '\r\e[K%s%s %s%-8s%s %s\n' "$D" "$(date +%H:%M:%S)" "$1" "$2" "$R" "$payload"
  status_dirty=1
}

# Repaint the single in-place annealer status line (no newline).
paint_status() {
  local w s
  w=$(cols)
  s="ANNEAL ${last_ann:-idle}"
  s=$(printf '%s' "$s" | cut -c1-$(( w - 2 )))
  printf '\r\e[K%s%s%s' "$D" "$s" "$R"
  status_dirty=0
}

printf '%s%s ACTIVITY MATRIX %s%s\n' "$C" "$B" "$R" "$D"
printf '%scommits + WIN/FAIL events scroll; annealer state updates in place%s\n' "$D" "$R"

last_commit=$(git log --oneline -1 2>/dev/null | awk '{print $1}')
last_deepseek=""
last_research=""
last_ann=""
status_dirty=1

while true; do
  # ── EVENTS: new git commits since last tick (each scrolls into history)
  cur=$(git log --oneline -1 2>/dev/null | awk '{print $1}')
  if [ -n "$cur" ] && [ "$cur" != "$last_commit" ]; then
    if [ -n "$last_commit" ]; then
      mapfile -t newc < <(git log --oneline "${last_commit}..HEAD" 2>/dev/null | head -8)
      for ((i=${#newc[@]}-1; i>=0; i--)); do
        case "${newc[$i]}" in
          *[Mm]atch\ fn_*) event "$G$B" "MATCH" "${newc[$i]}" ;;
          *)               event "$M"    "COMMIT" "${newc[$i]}" ;;
        esac
      done
    fi
    last_commit=$cur
  fi

  # ── EVENTS: deepseek worker log (only on change)
  ds=$(tail -1 tools/decomp_work/overnight/logs/deepseek_v4_live.log 2>/dev/null | grep -oE "START fn_[0-9A-F]+|REJECT fn_[0-9A-F]+.*|[0-9.]+% via" | head -1)
  if [ -n "$ds" ] && [ "$ds" != "$last_deepseek" ]; then
    event "$C" "DEEPSEEK" "$ds"
    last_deepseek="$ds"
  fi

  # ── EVENTS: research log — WIN / FAIL / NEW BEST only (no info chatter)
  raw_rl=$(tail -1 .omc/research_log.jsonl 2>/dev/null)
  if [ -n "$raw_rl" ] && [ "$raw_rl" != "$last_research" ]; then
    rl=$(printf '%s' "$raw_rl" | python3 -c "import sys,json
try:
 d=json.loads(sys.stdin.read())
 lvl=d.get('level','info'); msg=d.get('msg','')
 mu=msg.upper()
 if lvl=='win' or 'WIN' in mu.split(): k='WIN'
 elif 'FAIL' in mu or lvl=='warn': k='FAIL'
 elif lvl=='best' or 'NEW BEST' in mu: k='BEST'
 else: k=''
 if k: print(k+'|'+d.get('stage','?')+' '+d.get('fn','')+' '+msg[:80])
except: pass" 2>/dev/null)
    case "${rl%%|*}" in
      WIN)  event "$G$B" "WIN"  "${rl#*|}" ;;
      FAIL) event "$RD"  "FAIL" "${rl#*|}" ;;
      BEST) event "$C"   "BEST" "${rl#*|}" ;;
    esac
    last_research="$raw_rl"
  fi

  # ── STATUS: one-line annealer state, repainted ONLY on change
  ann=$(python3 -c "import json
try:
 d=json.load(open('.omc/permuter_state.json')); a=d.get('active',{})
 print(' '.join(f\"{v.get('fn','')}=E{v.get('score','?')}/best{v.get('best','?')}\" for v in list(a.values())[:4]))
except: pass" 2>/dev/null)
  if [ "$ann" != "$last_ann" ] || [ "$status_dirty" = 1 ]; then
    last_ann="$ann"
    paint_status
  fi
  sleep 4
done
