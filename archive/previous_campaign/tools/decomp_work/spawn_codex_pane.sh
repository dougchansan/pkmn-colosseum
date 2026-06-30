#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./spawn_codex_pane.sh "your codex exec prompt here"

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 \"codex task prompt\""
  exit 1
fi

PROMPT="$*"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
NPM_BIN="/c/Users/douglaswhittingham/AppData/Roaming/npm"

find_tmux() {
  local candidates=(
    "$LOCALAPPDATA/Microsoft/WinGet/Packages/marlocarlo.psmux_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "$LOCALAPPDATA/Microsoft/WinGet/Packages/arndawg.tmux-windows_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "/c/Users/douglaswhittingham/AppData/Local/Microsoft/WinGet/Packages/marlocarlo.psmux_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
    "/c/Users/douglaswhittingham/AppData/Local/Microsoft/WinGet/Packages/arndawg.tmux-windows_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
  )
  local p
  for p in "${candidates[@]}"; do
    if [ -f "$p" ]; then
      printf '%s\n' "$p"
      return 0
    fi
  done
  return 1
}

find_git_bash() {
  local candidates=(
    "/c/Program Files/Git/bin/bash.exe"
    "/c/Program Files/Git/usr/bin/bash.exe"
  )
  local p
  for p in "${candidates[@]}"; do
    if [ -f "$p" ]; then
      printf '%s\n' "$p"
      return 0
    fi
  done
  return 1
}

TMUX_BIN="$(find_tmux || true)"
GIT_BASH="$(find_git_bash || true)"

if [ -z "${TMUX_BIN:-}" ] || [ -z "${GIT_BASH:-}" ]; then
  echo "Could not find tmux.exe or Git Bash."
  exit 1
fi

"$TMUX_BIN" split-window -t decomp:pipeline.1 -h \
  "\"$GIT_BASH\" -lc 'cd \"$ROOT\" && export PATH=\"$NPM_BIN:\$PATH\" && echo \"=== CODEX EXEC ===\" && codex exec \"$PROMPT\"; ec=\$?; echo; echo \"codex exit code: \$ec\"; echo \"Press Enter to close pane...\"; read'"

"$TMUX_BIN" select-layout -t decomp:pipeline main-horizontal
