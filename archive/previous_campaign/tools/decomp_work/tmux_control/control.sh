#!/usr/bin/env bash
set -euo pipefail
# control.sh — High-level dispatcher for tmux multi-agent control.
#
# Usage:
#   ./control.sh list                          List panes
#   ./control.sh registry                      Show pane registry
#   ./control.sh refresh                       Refresh pane registry
#   ./control.sh dashboard                     Show full dashboard
#   ./control.sh capture <name> [lines]        Capture pane contents
#   ./control.sh focus <name>                  Focus a pane
#   ./control.sh clear <name>                  Clear a pane
#   ./control.sh interrupt <name>              Send Ctrl-C to pane
#   ./control.sh send <name> "<text>"          Send text + Enter
#   ./control.sh send-codex "<prompt>"         Send prompt to Codex TUI
#   ./control.sh send-codex-safe "<prompt>"    Capture first, then send
#   ./control.sh codex-interrupt               Soft-interrupt Codex
#   ./control.sh codex-capture [lines]         Capture Codex output
#   ./control.sh codex-reset                   Clear Codex input line
#   ./control.sh codex-idle                    Check if Codex is idle
#   ./control.sh send-glm "<prompt>"           Send prompt to GLM TUI
#   ./control.sh send-glm-safe "<prompt>"      Capture first, then send to GLM
#   ./control.sh glm-capture [lines]           Capture GLM output
#   ./control.sh glm-idle                      Check if GLM is idle
#   ./control.sh glm-interrupt                 Soft-interrupt GLM
#   ./control.sh send-worker "<prompt>"        Send prompt to the worker Opus TUI
#   ./control.sh send-worker-safe "<prompt>"   Capture first, then send to worker
#   ./control.sh worker-capture [lines]        Capture worker Opus output
#   ./control.sh worker-idle                   Check if worker Opus is idle
#   ./control.sh worker-interrupt              Soft-interrupt worker Opus
#   ./control.sh send-tui <name> "<prompt>"    Send prompt to any TUI pane by name
#   ./control.sh tui-idle <name>               Check if a named TUI pane is idle
#   ./control.sh shell <name> "<cmd>"          Run command in shell pane
#   ./control.sh restart watcher               Restart watcher pane
#   ./control.sh restart status                Restart status pane
#   ./control.sh start watcher                 Start watcher pane
#   ./control.sh start pokedex                 Start Pokédex dashboard
#   ./control.sh restart pokedex               Restart Pokédex dashboard
#   ./control.sh start status                  Start status pane
#   ./control.sh bootstrap                     Create 4-pane layout
#   ./control.sh repair                        Fix missing panes
#   ./control.sh check                         Run self-test
#   ./control.sh help                          Show this help

DIR="$(cd "$(dirname "$0")" && pwd)"

cmd="${1:-help}"
shift 2>/dev/null || true

case "$cmd" in
  list)              "$DIR/list_panes.sh" ;;
  registry)          "$DIR/show_registry.sh" ;;
  refresh)           "$DIR/refresh_registry.sh" ;;
  dashboard)         "$DIR/show_dashboard.sh" ;;
  capture)           "$DIR/capture_pane.sh" "$@" ;;
  focus)             "$DIR/focus_pane.sh" "$@" ;;
  clear)             "$DIR/clear_pane.sh" "$@" ;;
  interrupt)         "$DIR/interrupt_pane.sh" "$@" ;;
  send)              name="$1"; shift; "$DIR/send_text.sh" "$name" "$*"; "$DIR/send_enter.sh" "$name" ;;
  send-codex)        "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex-safe)   "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  # Codex lanes 2-4 (CODEX_NAME selects the pane the codex sender resolves)
  send-codex2)       CODEX_NAME=codex2 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex2-safe)  CODEX_NAME=codex2 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  send-codex3)       CODEX_NAME=codex3 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex3-safe)  CODEX_NAME=codex3 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  send-codex4)       CODEX_NAME=codex4 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex4-safe)  CODEX_NAME=codex4 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  send-codex5)       CODEX_NAME=codex5 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex5-safe)  CODEX_NAME=codex5 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  send-codex6)       CODEX_NAME=codex6 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex6-safe)  CODEX_NAME=codex6 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  send-codex7)       CODEX_NAME=codex7 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex7-safe)  CODEX_NAME=codex7 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  send-codex8)       CODEX_NAME=codex8 "$DIR/send_to_codex_tui.sh" "$@" ;;
  send-codex8-safe)  CODEX_NAME=codex8 "$DIR/send_to_codex_tui.sh" --capture-first "$@" ;;
  codex2-capture)    "$DIR/codex_capture.sh" --pane codex2 "$@" 2>/dev/null || "$DIR/capture_pane.sh" codex2 "${1:-80}" ;;
  codex3-capture)    "$DIR/capture_pane.sh" codex3 "${1:-80}" ;;
  codex4-capture)    "$DIR/capture_pane.sh" codex4 "${1:-80}" ;;
  codex-interrupt)   "$DIR/codex_soft_interrupt.sh" ;;
  codex-capture)     "$DIR/codex_capture.sh" "$@" ;;
  codex-reset)       "$DIR/codex_reset_prompt.sh" ;;
  codex-idle)        "$DIR/codex_is_idle.sh" "$@" ;;
  # Sonnet worker (Claude Code TUI, direct)
  send-sonnet)       "$DIR/send_to_tui.sh" sonnet "$@" ;;
  send-sonnet-safe)  "$DIR/send_to_tui.sh" sonnet --capture-first "$@" ;;
  sonnet-capture)    "$DIR/capture_pane.sh" sonnet "${1:-80}" ;;
  sonnet-idle)       "$DIR/tui_is_idle.sh" sonnet "$@" ;;
  # GLM worker (Claude Code TUI)
  send-glm)          "$DIR/send_to_tui.sh" glm "$@" ;;
  send-glm-safe)     "$DIR/send_to_tui.sh" glm --capture-first "$@" ;;
  glm-capture)       "$DIR/capture_pane.sh" glm "${1:-80}" ;;
  glm-idle)          "$DIR/tui_is_idle.sh" glm "$@" ;;
  glm-interrupt)     "$DIR/tui_soft_interrupt.sh" glm ;;
  # Worker Opus (second Opus pane, dispatched by the orchestrator)
  send-worker)       "$DIR/send_to_tui.sh" worker "$@" ;;
  send-worker-safe)  "$DIR/send_to_tui.sh" worker --capture-first "$@" ;;
  worker-capture)    "$DIR/capture_pane.sh" worker "${1:-80}" ;;
  worker-idle)       "$DIR/tui_is_idle.sh" worker "$@" ;;
  worker-interrupt)  "$DIR/tui_soft_interrupt.sh" worker ;;
  # Generic TUI dispatch by name (claude/codex/glm/worker)
  send-tui)          name="$1"; shift; "$DIR/send_to_tui.sh" "$name" "$@" ;;
  send-tui-safe)     name="$1"; shift; "$DIR/send_to_tui.sh" "$name" --capture-first "$@" ;;
  tui-idle)          "$DIR/tui_is_idle.sh" "$@" ;;
  tui-interrupt)     "$DIR/tui_soft_interrupt.sh" "$@" ;;
  shell)             "$DIR/run_in_shell_pane.sh" "$@" ;;
  restart)
    case "${1:-}" in
      watcher) "$DIR/restart_watcher_pane.sh" ;;
      status)  "$DIR/restart_status_pane.sh" ;;
      pokedex) "$DIR/restart_pokedex_pane.sh" ;;
      *)       echo "Usage: control.sh restart <watcher|status|pokedex>" >&2; exit 1 ;;
    esac
    ;;
  start)
    case "${1:-}" in
      watcher) "$DIR/start_watcher_pane.sh" ;;
      status)  "$DIR/start_status_pane.sh" ;;
      pokedex) "$DIR/start_pokedex_pane.sh" ;;
      *)       echo "Usage: control.sh start <watcher|status|pokedex>" >&2; exit 1 ;;
    esac
    ;;
  bootstrap)         "$DIR/bootstrap_layout.sh" ;;
  repair)            "$DIR/repair_layout.sh" ;;
  check)             "$DIR/check.sh" ;;
  help|--help|-h)
    head -n 45 "$0" | grep '^#' | sed 's/^# \?//'
    ;;
  *)
    echo "Unknown command: $cmd" >&2
    echo "Run: ./control.sh help" >&2
    exit 1
    ;;
esac
