#!/usr/bin/env bash
# pane_io.sh — the SOLE owner of psmux. The ONLY process that ever runs a tmux
# client during autonomous operation. Single-threaded; every tmux call goes through
# txk (SIGKILL-bounded + serialized), so a wedged client dies in <=TXK_T seconds and
# the loop survives — nothing can pile up or block the machine.
#
# Each pass:
#   1. capture every agent pane -> build/hb/<NAME>.live, classify -> build/hb/<NAME>.state
#        idle  = screen byte-static across the pass AND no "esc to interrupt" AND not rate-limited
#        busy  = changing, or showing the interrupt indicator
#        rate  = rate-limit / usage-cap text on screen (idle but cannot work)
#        capfail = capture timed out / empty (treat as not-dispatchable)
#   2. drain build/dispatch/<NAME>.req -> send-keys the prompt + Enter, archive the req,
#      and force that pane's state to busy so the driver won't double-send.
#   writes build/hb/.alive (epoch) each pass so the driver can detect a dead owner.
#
# Modes:  --once       one pass then exit (testing)
#         --no-send    capture/classify only, never send (dry run against a live cockpit)
#         INTERVAL=N   seconds between passes (default 8)
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
export MSYS_NO_PATHCONV=1
source tools/decomp_work/tmux_control/txk.sh
source tools/decomp_work/tmux_control/panes.env 2>/dev/null

ONCE=0; NOSEND=0
for a in "$@"; do case "$a" in --once) ONCE=1;; --no-send) NOSEND=1;; esac; done
INTERVAL="${INTERVAL:-8}"
HB=build/hb; REQ=build/dispatch; mkdir -p "$HB" "$REQ" "$REQ/sent"

PIDF="build/.pane_io.pid"
if [ -f "$PIDF" ] && kill -0 "$(cat "$PIDF" 2>/dev/null)" 2>/dev/null; then
  echo "[pane_io] another instance ($(cat "$PIDF")) already running — exiting"; exit 0
fi
echo $$ > "$PIDF"; trap 'rm -f "$PIDF"' EXIT

# NAME -> pane id. Matches the names auto_rebatch dispatches to, so a dispatch req
# named build/dispatch/<NAME>.req lands on the right pane. Empty entries are skipped.
declare -A PANE=(
  [OPUS]="${WORKER_PANE:-}" [SON]="${SONNET_PANE:-}" [OPUS3]="${CODEX3_PANE:-}"
  [GLM]="${GLM_PANE:-}"
  [C1]="${CODEX_PANE:-}" [C2]="${CODEX2_PANE:-}" [C3]="${CODEX3_PANE:-}" [C4]="${CODEX4_PANE:-}"
  [C5]="${CODEX5_PANE:-}" [C6]="${CODEX6_PANE:-}" [C7]="${CODEX7_PANE:-}" [C8]="${CODEX8_PANE:-}"
)

# --- per-function token ledger ------------------------------------------------
# Attribute each completed task's token spend to its target function(s). pane_io
# /clears every agent before each dispatch, so the TUI token counter measures just
# that task. Codex only shows its count in the live spinner (gone once idle), so we
# sample on every busy pass and finalize the last seen value at the busy->idle edge.
LEDGER="$HB/token_by_fn.jsonl"

lane_provider() {  # lane name -> provider bucket (matches dashboard SOURCE_KEYS)
  case "$1" in
    OPUS|SON|OPUS3) echo claude ;;
    GLM)            echo glm ;;
    C[1-8])         echo codex ;;
    *)              echo other ;;
  esac
}

_num() {  # "155.2k"/"7"/"1.2M" -> integer
  awk -v s="$1" 'BEGIN{ g=tolower(s); u=1; if(g ~ /k/)u=1000; if(g ~ /m/)u=1000000;
    gsub(/[^0-9.]/,"",g); if(g=="")g=0; printf "%d", g*u + 0.5 }'
}

parse_tokens() {  # <name> -> "IN OUT" (best-effort from $HB/<name>.live; 0 0 if none)
  local live="$HB/$1.live" cap m inn out
  [ -f "$live" ] || { echo "0 0"; return; }
  cap=$(cat "$live")
  # Claude/GLM TUI persistent footer: "tokens: 155200in/7.2kout"
  m=$(printf '%s\n' "$cap" | grep -oiE "tokens:[[:space:]]*[0-9.]+[km]?[[:space:]]*in/[[:space:]]*[0-9.]+[km]?[[:space:]]*out" | tail -1)
  if [ -n "$m" ]; then
    inn=$(printf '%s' "$m" | grep -oiE "[0-9.]+[km]?[[:space:]]*in" | grep -oiE "[0-9.]+[km]?")
    out=$(printf '%s' "$m" | grep -oiE "[0-9.]+[km]?[[:space:]]*out" | grep -oiE "[0-9.]+[km]?")
    echo "$(_num "$inn") $(_num "$out")"; return
  fi
  # Truncated footer on a narrow pane ("tokens: 155200in/7…") -> input only.
  m=$(printf '%s\n' "$cap" | grep -oiE "tokens:[[:space:]]*[0-9.]+[km]?[[:space:]]*in" | tail -1 | grep -oiE "[0-9.]+[km]?")
  if [ -n "$m" ]; then echo "$(_num "$m") 0"; return; fi
  # Codex/GLM spinner total: "↑ 61.0k tokens" / "↓ 15.8k tokens"
  m=$(printf '%s\n' "$cap" | grep -oiE "[0-9.]+[km]?[[:space:]]*tokens" | tail -1 | grep -oiE "[0-9.]+[km]?")
  if [ -n "$m" ]; then echo "$(_num "$m") 0"; return; fi
  echo "0 0"
}

finalize_task() {  # <name> : append a ledger row for the just-completed task
  local name="$1" meta="$HB/$name.curtask"
  [ -f "$meta" ] || return 0
  local ts tstart tag file fns prov tin tout
  ts=$(date +%s)
  tstart=$(sed -n 's/^tstart=//p' "$meta"); [ -n "$tstart" ] || tstart=0
  tag=$(sed -n 's/^tag=//p' "$meta")
  file=$(sed -n 's/^file=//p' "$meta")
  fns=$(sed -n 's/^fns=//p' "$meta")
  prov=$(sed -n 's/^prov=//p' "$meta")
  read -r tin tout < "$HB/$name.tok.last" 2>/dev/null || { tin=0; tout=0; }
  [ -n "$tin" ] || tin=0; [ -n "$tout" ] || tout=0
  local fns_json
  fns_json=$(printf '%s' "$fns" | tr ' ' '\n' | grep -E '^fn_' \
    | awk 'BEGIN{ORS="";print "["} {printf "%s\"%s\"",(NR>1?",":""),$0} END{print "]"}')
  [ -n "$fns_json" ] || fns_json="[]"
  # ts = task end, tstart = dispatch time -> the [tstart,ts] window for journal-based
  # token attribution (footer counts are lossy on narrow panes / absent for codex).
  printf '{"ts":%s,"tstart":%s,"lane":"%s","provider":"%s","tag":"%s","file":"%s","fns":%s,"tokens_in":%s,"tokens_out":%s}\n' \
    "$ts" "$tstart" "$name" "$prov" "$tag" "$file" "$fns_json" "$tin" "$tout" >> "$LEDGER"
  rm -f "$meta" "$HB/$name.tok.last"
}

classify() {  # classify <name> <pane> ; echoes idle|busy|rate|capfail
  local name="$1" pane="$2" cap rc streak
  cap=$(txk capture-pane -p -t "$pane" 2>/dev/null); rc=$?
  if [ $rc -ne 0 ] || [ -z "$cap" ]; then echo capfail; return; fi
  printf '%s' "$cap" > "$HB/$name.live"
  # ACTIVE-WORK markers -> busy immediately, and reset the idle streak. These are the
  # RELIABLE "still working" signals (spinner gerund "… for Ns", elapsed "(Nm Ns ·",
  # "esc to interrupt", "thinking"); a genuinely idle prompt shows none of them. A
  # working agent always shows one, so it's never misread as idle and /cleared mid-task.
  # NB: use "esctoint" (= "esc to interrupt", the ACTIVE indicator), NOT bare "interrupt"
  # — codex's idle "Conversation interrupted" message contains "interrupt" and would
  # falsely pin the lane busy forever.
  if printf '%s' "$cap" | tr -d ' ' | grep -qiE "esctoint|thinking|workingfor|for[0-9]+s|[0-9]+s·|\([0-9]+m[0-9]+s"; then
    echo 0 > "$HB/$name.scount"; echo busy; return
  fi
  # Codex prints a benign welcome line "You have N usage limit reset available. Run /usage"
  # at startup — NOT a rate-limit. Strip it before the rate check so codex lanes aren't
  # falsely gated as rate-limited and skipped by the dispatcher.
  if printf '%s' "$cap" | grep -ivE "usage limit reset|run /usage" | grep -qiE "rate.?limit|usage limit|limit reached|too many request|try again (in|at|later)|resets? (at|in)|reached your|429 "; then
    echo 0 > "$HB/$name.scount"; echo rate; return
  fi
  # No busy/rate markers -> idle prompt. Debounce IDLE_MIN passes (guards a momentary
  # marker gap between tool calls) before declaring idle. We do NOT require a byte-static
  # screen — the TUI footer's token counter/cursor changes every pass, which would
  # otherwise keep a genuinely-idle lane "busy" forever and starve it of dispatch.
  streak=0; [ -f "$HB/$name.scount" ] && streak=$(cat "$HB/$name.scount" 2>/dev/null)
  streak=$((streak + 1)); echo "$streak" > "$HB/$name.scount"
  if [ "$streak" -ge "${IDLE_MIN:-2}" ]; then echo idle; else echo busy; fi
}

drain_one() {  # drain_one <name> <pane> : point the agent at a task FILE (no long-prompt send)
  local name="$1" pane="$2" req="$REQ/$name.req" task="$REQ/$name.task"
  [ -f "$req" ] || return 0
  [ -s "$req" ] || { rm -f "$req"; return 0; }
  if [ "$NOSEND" = 1 ]; then echo "[pane_io] (--no-send) would dispatch -> $name"; return 0; fi
  # Long-prompt delivery on native psmux is fundamentally unreliable under load:
  #   - send-keys -l truncates (drops the head, leaving "...AVED <fn> <%>" fragments).
  #   - paste-buffer -t is ignored (hits the active pane) AND truncates when the loaded
  #     server makes the paste slower than the SIGKILL timeout.
  # So DON'T send the prompt at all: stage the full task in a file and send a SHORT, reliable
  # command (~50 chars, well under any truncation threshold) telling the agent to read it.
  mv -f "$req" "$task" 2>/dev/null || { cp -f "$req" "$task" && rm -f "$req"; }
  # record per-task meta for the token ledger (provider, tag, file, target fns).
  # /clear below resets the agent's token footer, so the next busy->idle window
  # measures exactly this task's spend (finalized in pass()).
  {
    echo "tstart=$(date +%s)"
    echo "prov=$(lane_provider "$name")"
    echo "tag=$(grep -oiE 'TAG:[[:space:]]*[A-Za-z0-9_]+' "$task" | head -1 | sed -E 's/.*[Tt][Aa][Gg]:[[:space:]]*//')"
    echo "file=$(grep -oiE 'File:[[:space:]]*[^[:space:]]+' "$task" | head -1 | sed -E 's/.*[Ff]ile:[[:space:]]*//')"
    echo "fns=$(grep -oiE 'fns:[[:space:]]*(fn_[0-9A-Fa-f]+[[:space:]]*)+' "$task" | grep -oiE 'fn_[0-9A-Fa-f]{8}' | tr '\n' ' ' | sed 's/ *$//')"
  } > "$HB/$name.curtask"
  echo "0 0" > "$HB/$name.tok.last"
  local cmd="Read build/dispatch/$name.task and do exactly what it says."
  _txk_acquire
  timeout -s KILL 10 "$TMUX_BIN" send-keys -t "$pane" C-u 2>/dev/null            # clear stale/truncated input
  # /clear the agent's context BEFORE each new task so every assignment starts fresh (no
  # carryover bloat across tasks). The KG (tools/decomp_work/kg/kg.db) is the durable shared
  # memory, so cracked levers survive the clear; the band scratch on disk is resume-safe too.
  # Only reached when the lane is idle (cooldown + idle-gated by auto_rebatch), so this never
  # wipes an in-progress task. Set NO_CLEAR_ON_DISPATCH=1 to skip.
  if [ "${NO_CLEAR_ON_DISPATCH:-0}" != 1 ]; then
    timeout -s KILL 10 "$TMUX_BIN" send-keys -t "$pane" -l "/clear" 2>/dev/null
    sleep 0.3
    timeout -s KILL 10 "$TMUX_BIN" send-keys -t "$pane" Enter 2>/dev/null
    sleep 1.0                                                                     # let /clear take effect
    timeout -s KILL 10 "$TMUX_BIN" send-keys -t "$pane" C-u 2>/dev/null          # clear residual input post-/clear
  fi
  timeout -s KILL 10 "$TMUX_BIN" send-keys -t "$pane" -l "$cmd" 2>/dev/null
  sleep 0.3
  timeout -s KILL 10 "$TMUX_BIN" send-keys -t "$pane" Enter 2>/dev/null
  _txk_release
  echo "busy $(date +%s)" > "$HB/$name.state"; rm -f "$HB/$name.prev"
  echo "[pane_io] dispatched -> $name"
}

pass() {
  local name pane st prevst tok
  echo "$(date +%s)" > "$HB/.alive"
  for name in "${!PANE[@]}"; do
    pane="${PANE[$name]}"; [ -n "$pane" ] || continue
    prevst=""; [ -f "$HB/$name.state" ] && prevst=$(cut -d' ' -f1 < "$HB/$name.state" 2>/dev/null)
    st=$(classify "$name" "$pane")
    # token ledger: sample the footer while busy; finalize the task on busy->idle.
    if [ "$st" = busy ]; then
      tok=$(parse_tokens "$name")
      [ "$tok" != "0 0" ] && echo "$tok" > "$HB/$name.tok.last"
    elif [ "$st" = idle ] && { [ "$prevst" = busy ] || [ "$prevst" = rate ]; }; then
      finalize_task "$name"
    fi
    echo "$st $(date +%s)" > "$HB/$name.state"
    echo "$(date +%s)" > "$HB/.alive"      # keep heartbeat fresh during the capture loop
  done
  for name in "${!PANE[@]}"; do
    pane="${PANE[$name]}"; [ -n "$pane" ] || continue
    # CRITICAL: only drain (send /clear + task) when the lane is CURRENTLY idle. A .req is
    # written by auto_rebatch from a PRIOR pass's classification; if the lane has since gone
    # busy (started its next task, or was briefly misclassified idle), draining now would
    # /clear a WORKING agent and wipe its in-progress task. Re-check the state just written
    # above (this pass's classification). If busy/rate, leave the .req queued for a later
    # pass when the lane is genuinely idle.
    cur=$(cut -d' ' -f1 < "$HB/$name.state" 2>/dev/null)
    [ "$cur" = idle ] || continue
    drain_one "$name" "$pane"
    echo "$(date +%s)" > "$HB/.alive"      # ...and during (slower) dispatch drains
  done
}

echo "[pane_io] up — sole tmux owner, interval ${INTERVAL}s, txk_kill=${TXK_T}s$([ "$NOSEND" = 1 ] && echo ' [NO-SEND]')"
if [ "$ONCE" = 1 ]; then pass; exit 0; fi
while true; do pass; sleep "$INTERVAL"; done
