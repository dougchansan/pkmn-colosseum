#!/usr/bin/env bash
# cockpit.sh — Mac/Linux-native multi-agent tmux cockpit for the decomp pipeline.
#
# Unlike the Windows control.sh/txk.sh setup (which exists only to wrangle a
# native-PE psmux binary), this uses real tmux directly — no SIGKILL wrapper,
# no WinGet path hunting, no registry file. Panes are resolved by a tmux pane
# user-option (@role) that TUIs cannot overwrite, so there is nothing to sync.
#
# Panes (window "cockpit" of session "$DECOMP_SESSION", default: decomp):
#   orchestrator  shell in the repo root — you drive from here
#   glm           opencode TUI on the z.ai GLM coding plan (glm/glm-5.2)
#   codex         codex TUI (ChatGPT-authed)
#   shell         spare shell for permuter / status / scratch
#
# Usage:
#   cockpit.sh bootstrap              create the layout + launch the TUIs
#   cockpit.sh attach                 attach to the session
#   cockpit.sh list                   list panes and their titles
#   cockpit.sh status                 panes + idle/busy per agent
#   cockpit.sh send <name> "<text>"   type text + Enter into a pane
#   cockpit.sh send-glm "<text>"      -> send glm  (types into the attached TUI)
#   cockpit.sh send-codex "<text>"    -> send codex
#   cockpit.sh ask-glm "<prompt>"     drive GLM headlessly, print its answer (reliable)
#   cockpit.sh ask-codex "<prompt>"   drive Codex headlessly, print its answer (needs codex login)
#   cockpit.sh capture <name> [lines] print the last [lines] (default 80) of a pane
#   cockpit.sh idle <name>            exit 0 if the pane looks idle, 1 if busy
#   cockpit.sh glm-idle | codex-idle  -> idle glm / codex
#   cockpit.sh restart <name>         relaunch the TUI in a pane (glm|codex)
#   cockpit.sh kill                   kill the whole session
#   cockpit.sh help
#
# Config (env overrides):
#   DECOMP_SESSION        tmux session name           (default: decomp)
#   DECOMP_COCKPIT_ENV    file sourced for API keys   (default: ~/.config/decomp-keys/cockpit.env)
#   DECOMP_GLM_MODEL      opencode model for the glm pane (default: glm/glm-5.2)
#   DECOMP_REPO           repo root                   (default: auto-detected)

set -uo pipefail

TMUX="${TMUX_BIN:-tmux}"
SESSION="${DECOMP_SESSION:-decomp}"
WINDOW="cockpit"
COCKPIT_ENV="${DECOMP_COCKPIT_ENV:-$HOME/.config/decomp-keys/cockpit.env}"
GLM_MODEL="${DECOMP_GLM_MODEL:-glm/glm-5.2}"

# repo root = three levels up from this script (tools/decomp_work/tmux_control/)
SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${DECOMP_REPO:-$(cd "$SELF/../../.." && pwd)}"

die() { echo "cockpit: $*" >&2; exit 1; }
have_session() { "$TMUX" has-session -t "$SESSION" 2>/dev/null; }

# Resolve a semantic pane name -> tmux pane id, via the pane user-option @role.
# (TUIs like opencode rewrite pane_title via OSC escapes, so titles are unreliable;
# @role is a tmux-side option the app cannot touch.)
pane_for() {
  local name="$1"
  "$TMUX" list-panes -t "$SESSION:$WINDOW" -F '#{pane_id} #{@role}' 2>/dev/null \
    | awk -v n="$name" '$2==n {print $1; exit}'
}

require_pane() {
  local id; id="$(pane_for "$1")"
  [ -n "$id" ] || die "no pane named '$1' (is the cockpit bootstrapped? run: cockpit.sh bootstrap)"
  printf '%s' "$id"
}

# Launch command for a TUI pane: source keys, cd to repo, run the TUI, then
# fall back to a shell so a crash/exit doesn't destroy the pane.
launch_cmd() {
  local what="$1" inner
  case "$what" in
    glm)          inner="opencode --model '$GLM_MODEL'" ;;
    codex|codex2) inner="codex" ;;
    opus)         inner="claude --model opus" ;;
    sonnet)       inner="claude --model sonnet" ;;
    *)            inner="$SHELL" ;;
  esac
  printf 'cd %q; [ -f %q ] && { set -a; . %q; set +a; }; %s; exec %q' \
    "$REPO" "$COCKPIT_ENV" "$COCKPIT_ENV" "$inner" "${SHELL:-/bin/bash}"
}

# Roles that run an agent TUI (the rest are plain shells).
AGENT_ROLES="opus glm codex codex2 sonnet"
ALL_ROLES="opus glm codex codex2 sonnet shell"

bootstrap() {
  if have_session; then
    echo "cockpit: session '$SESSION' already exists — attach with: cockpit.sh attach"
    return 0
  fi
  [ -f "$COCKPIT_ENV" ] || echo "cockpit: WARNING: $COCKPIT_ENV not found — GLM pane may lack ZAI_API_KEY"
  command -v opencode >/dev/null || echo "cockpit: WARNING: opencode not on PATH (glm pane)"
  command -v codex    >/dev/null || echo "cockpit: WARNING: codex not on PATH (codex pane)"

  # 6 panes in a tiled grid (opus | glm | codex | codex2 | sonnet | shell)
  "$TMUX" new-session -d -s "$SESSION" -n "$WINDOW" -c "$REPO" -x 250 -y 60
  for _ in 1 2 3 4 5; do
    "$TMUX" split-window -t "$SESSION:$WINDOW" -c "$REPO"
    "$TMUX" select-layout -t "$SESSION:$WINDOW" tiled >/dev/null 2>&1 || true
  done

  # tag each pane with a stable @role (creation order)
  local roles=($ALL_ROLES) i=0 id
  while read -r id; do
    "$TMUX" set -p -t "$id" @role "${roles[$i]}"
    i=$((i + 1))
  done < <("$TMUX" list-panes -t "$SESSION:$WINDOW" -F '#{pane_id}')

  # show the role on each pane border
  "$TMUX" set -t "$SESSION" pane-border-status top  >/dev/null 2>&1 || true
  "$TMUX" set -t "$SESSION" pane-border-format ' #{@role} #{?#{==:#{@role},},,| }#{pane_current_command} ' >/dev/null 2>&1 || true

  # launch the agent TUIs (shell stays a plain shell). Skip with DECOMP_NO_AGENTS=1
  # so a fleet driver can run a worker loop in each pane instead of an interactive TUI.
  if [ -z "${DECOMP_NO_AGENTS:-}" ]; then
    for r in $AGENT_ROLES; do
      local p; p="$(pane_for "$r")"
      "$TMUX" send-keys -t "$p" -l -- "$(launch_cmd "$r")"
      "$TMUX" send-keys -t "$p" Enter
    done
  fi
  "$TMUX" select-pane -t "$(pane_for opus)"

  echo "cockpit: session '$SESSION' up (opus | glm | codex | codex2 | sonnet | shell)"
  echo "         attach: tools/decomp_work/tmux_control/cockpit.sh attach"
}

cmd_send() {
  local id; id="$(require_pane "$1")"; shift
  "$TMUX" send-keys -t "$id" -l -- "$*"
  "$TMUX" send-keys -t "$id" Enter
}

# Non-interactive dispatch — the RELIABLE way to drive an agent (no tmux/TUI
# needed). Returns the agent's answer on stdout. Use these from the orchestrator.
_source_keys() { [ -f "$COCKPIT_ENV" ] && { set -a; . "$COCKPIT_ENV"; set +a; }; }

cmd_ask_glm() {
  [ $# -ge 1 ] || die "usage: ask-glm \"<prompt>\""
  ( cd "$REPO"; _source_keys; opencode run --model "$GLM_MODEL" "$*" )
}

cmd_ask_codex() {
  [ $# -ge 1 ] || die "usage: ask-codex \"<prompt>\""
  command -v codex >/dev/null || die "codex not on PATH (brew reinstall --cask codex)"
  ( cd "$REPO"; _source_keys; codex exec --skip-git-repo-check "$*" )
}

cmd_capture() {
  local id; id="$(require_pane "$1")"; local lines="${2:-80}"
  "$TMUX" capture-pane -p -t "$id" -S "-$lines"
}

# Idle heuristic: capture twice ~1.6s apart; identical tail => idle (exit 0).
cmd_idle() {
  local id; id="$(require_pane "$1")"
  local a b
  a="$("$TMUX" capture-pane -p -t "$id" -S -12 2>/dev/null)"
  sleep 1.6
  b="$("$TMUX" capture-pane -p -t "$id" -S -12 2>/dev/null)"
  if [ "$a" = "$b" ]; then echo "idle"; return 0; else echo "busy"; return 1; fi
}

cmd_restart() {
  local what="$1" id; id="$(require_pane "$what")"
  "$TMUX" send-keys -t "$id" C-c 2>/dev/null || true
  sleep 0.3
  "$TMUX" send-keys -t "$id" -l -- "$(launch_cmd "$what")"
  "$TMUX" send-keys -t "$id" Enter
  echo "cockpit: restarted '$what'"
}

cmd_status() {
  have_session || die "no session '$SESSION'"
  printf '%-14s %-8s %s\n' PANE STATE ID
  for n in $ALL_ROLES; do
    local id state; id="$(pane_for "$n")"
    [ -z "$id" ] && { printf '%-14s %-8s %s\n' "$n" "-" "(missing)"; continue; }
    if cmd_idle "$n" >/dev/null 2>&1; then state=idle; else state=busy; fi
    printf '%-14s %-8s %s\n' "$n" "$state" "$id"
  done
}

case "${1:-help}" in
  bootstrap) bootstrap ;;
  attach)    have_session || die "no session — run: cockpit.sh bootstrap"; exec "$TMUX" attach -t "$SESSION" ;;
  list)      have_session || die "no session"; "$TMUX" list-panes -t "$SESSION:$WINDOW" -F '#{@role}\t#{pane_id}\t#{pane_current_command}' ;;
  status)    cmd_status ;;
  send)      shift; [ $# -ge 2 ] || die "usage: send <name> \"<text>\""; cmd_send "$@" ;;
  send-glm)  shift; cmd_send glm "$*" ;;
  send-codex) shift; cmd_send codex "$*" ;;
  ask-glm)   shift; cmd_ask_glm "$*" ;;
  ask-codex) shift; cmd_ask_codex "$*" ;;
  capture)   shift; [ $# -ge 1 ] || die "usage: capture <name> [lines]"; cmd_capture "$@" ;;
  idle)      shift; [ $# -ge 1 ] || die "usage: idle <name>"; cmd_idle "$1" ;;
  glm-idle)  cmd_idle glm ;;
  codex-idle) cmd_idle codex ;;
  restart)   shift; [ $# -ge 1 ] || die "usage: restart <glm|codex>"; cmd_restart "$1" ;;
  kill)      have_session && "$TMUX" kill-session -t "$SESSION" && echo "cockpit: killed '$SESSION'" || echo "cockpit: no session" ;;
  help|--help|-h) sed -n '2,40p' "$0" | sed 's/^# \{0,1\}//' ;;
  *) die "unknown command '$1' (run: cockpit.sh help)" ;;
esac
