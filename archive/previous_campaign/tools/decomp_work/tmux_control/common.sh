#!/usr/bin/env bash
# common.sh — Shared functions for tmux multi-agent control.
# Source this file; do not execute directly.

_COMMON_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$_COMMON_DIR/config.sh"

# --- tmux binary discovery ---
find_tmux() {
  local local_app="${LOCALAPPDATA:-}"
  local candidates=(
    "${local_app}/Microsoft/WinGet/Packages/marlocarlo.psmux_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "${local_app}/Microsoft/WinGet/Packages/arndawg.tmux-windows_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "/mnt/c/Users/douglaswhittingham/AppData/Local/Microsoft/WinGet/Packages/marlocarlo.psmux_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "/mnt/c/Users/douglaswhittingham/AppData/Local/Microsoft/WinGet/Packages/arndawg.tmux-windows_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "/c/Users/douglaswhittingham/AppData/Local/Microsoft/WinGet/Packages/marlocarlo.psmux_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "/c/Users/douglaswhittingham/AppData/Local/Microsoft/WinGet/Packages/arndawg.tmux-windows_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
  )
  for p in "${candidates[@]}"; do
    if [ -f "$p" ]; then printf '%s\n' "$p"; return 0; fi
  done
  return 1
}

TMUX_BIN="${TMUX_BIN:-$(find_tmux 2>/dev/null || true)}"
if [ -z "$TMUX_BIN" ]; then
  echo "FATAL: tmux binary not found" >&2
  exit 1
fi

# --- Logging ---
mkdir -p "$LOG_DIR"

log_event() {
  # log_event <category> <message>
  local cat="${1:-general}" msg="${2:-}"
  local ts
  ts="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
  printf '%s [%s] %s\n' "$ts" "$cat" "$msg" >> "$LOG_DIR/control.log"
}

# --- Pane registry ---
load_registry() {
  if [ -f "$REGISTRY_FILE" ]; then
    source "$REGISTRY_FILE"
  fi
}

# Call once on load
load_registry

# resolve_pane_target <semantic_name>
# Returns the tmux pane target string.
resolve_pane_target() {
  local name="${1,,}" # lowercase
  case "$name" in
    claude)  echo "${CLAUDE_PANE:-$DEFAULT_CLAUDE_PANE}" ;;
    codex)   echo "${CODEX_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex2)  echo "${CODEX2_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex3)  echo "${CODEX3_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex4)  echo "${CODEX4_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex5)  echo "${CODEX5_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex6)  echo "${CODEX6_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex7)  echo "${CODEX7_PANE:-$DEFAULT_CODEX_PANE}" ;;
    codex8)  echo "${CODEX8_PANE:-$DEFAULT_CODEX_PANE}" ;;
    glm)     echo "${GLM_PANE:-$DEFAULT_GLM_PANE}" ;;
    sonnet)  echo "${SONNET_PANE:-$DEFAULT_WORKER_PANE}" ;;
    worker)  echo "${WORKER_PANE:-$DEFAULT_WORKER_PANE}" ;;
    worker2) echo "${WORKER2_PANE:-$DEFAULT_WORKER_PANE}" ;;
    status)  echo "${STATUS_PANE:-$DEFAULT_STATUS_PANE}" ;;
    watcher) echo "${WATCHER_PANE:-$DEFAULT_WATCHER_PANE}" ;;
    pokedex) echo "${POKEDEX_PANE:-$DEFAULT_POKEDEX_PANE}" ;;
    *)
      echo "ERROR: unknown pane name: $name (use: claude, codex, codex2, codex3, codex4, glm, sonnet, worker, worker2, status, watcher, pokedex)" >&2
      return 1
      ;;
  esac
}

# get_pane_type <semantic_name>
get_pane_type() {
  local name="${1,,}"
  case "$name" in
    claude)  echo "$PANE_TYPE_CLAUDE" ;;
    codex)   echo "$PANE_TYPE_CODEX" ;;
    codex2)  echo "$PANE_TYPE_CODEX" ;;
    codex3)  echo "$PANE_TYPE_CODEX" ;;
    codex4)  echo "$PANE_TYPE_CODEX" ;;
    codex5)  echo "$PANE_TYPE_CODEX" ;;
    codex6)  echo "$PANE_TYPE_CODEX" ;;
    codex7)  echo "$PANE_TYPE_CODEX" ;;
    codex8)  echo "$PANE_TYPE_CODEX" ;;
    glm)     echo "$PANE_TYPE_GLM" ;;
    sonnet)  echo "$PANE_TYPE_WORKER" ;;
    worker)  echo "$PANE_TYPE_WORKER" ;;
    worker2) echo "$PANE_TYPE_WORKER" ;;
    status)  echo "$PANE_TYPE_STATUS" ;;
    watcher) echo "$PANE_TYPE_WATCHER" ;;
    pokedex) echo "$PANE_TYPE_POKEDEX" ;;
    *)       echo "shell" ;;
  esac
}

# --- Session validation ---
assert_session() {
  if ! "$TMUX_BIN" has-session -t "$TMUX_SESSION" 2>/dev/null; then
    echo "ERROR: tmux session '$TMUX_SESSION' does not exist" >&2
    return 1
  fi
}

# --- Safe quoting for send-keys ---
# tmux send-keys treats certain chars specially; this wraps for literal text.
safe_send_text() {
  local target="$1" text="$2"
  # Use send-keys -l for literal text (no key lookup)
  "$TMUX_BIN" send-keys -t "$target" -l "$text"
}

# --- Die helper ---
die() {
  echo "ERROR: $*" >&2
  exit 1
}
