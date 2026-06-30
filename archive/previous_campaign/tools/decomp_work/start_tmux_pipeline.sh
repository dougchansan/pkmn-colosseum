#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
NPM_BIN="/c/Users/douglaswhittingham/AppData/Roaming/npm"
GIT_BASH="/c/Program Files/Git/bin/bash.exe"
TMUX_BIN="$LOCALAPPDATA/Microsoft/WinGet/Packages/marlocarlo.psmux_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"

echo "Starting decomp pipeline..."
echo "Root: $ROOT"
echo "tmux: $TMUX_BIN"
echo "bash: $GIT_BASH"

"$TMUX_BIN" kill-server 2>/dev/null || true

"$TMUX_BIN" new-session -d -s decomp -n pipeline

# Force all panes/windows to use Git Bash
"$TMUX_BIN" set-option -t decomp default-shell "$GIT_BASH"
"$TMUX_BIN" set-option -t decomp default-command "\"$GIT_BASH\" -li"

# Top pane: status
"$TMUX_BIN" send-keys -t decomp:pipeline.0 \
  "cd \"$ROOT\" && export PATH=\"$NPM_BIN:\$PATH\" && while true; do clear; echo '=== DECOMP PIPELINE STATUS ==='; python tools/decomp_scheduler.py --status 2>/dev/null; echo; printf 'Candidates: %s | Review: %s | Applied: %s\n' \"\$(find tools/decomp_work/candidates -maxdepth 1 -name '*.json' 2>/dev/null | wc -l)\" \"\$(find tools/decomp_work/review -maxdepth 1 -name '*.json' 2>/dev/null | wc -l)\" \"\$(find tools/decomp_work/applied -maxdepth 1 -name '*.json' 2>/dev/null | wc -l)\"; sleep 8; done" Enter

# Bottom left: Claude
"$TMUX_BIN" split-window -t decomp:pipeline -v -p 70
"$TMUX_BIN" send-keys -t decomp:pipeline.1 \
  "cd \"$ROOT\" && export PATH=\"$NPM_BIN:\$PATH\" && claude --dangerously-skip-permissions" Enter

# Bottom right: Codex-ready bash shell
"$TMUX_BIN" split-window -t decomp:pipeline.1 -h -p 50
"$TMUX_BIN" send-keys -t decomp:pipeline.2 \
  "cd \"$ROOT\" && export PATH=\"$NPM_BIN:\$PATH\" && echo '=== CODEX PANE (ready) ==='" Enter

"$TMUX_BIN" select-layout -t decomp:pipeline main-horizontal
"$TMUX_BIN" select-pane -t decomp:pipeline.1

echo
echo "Done. Attach with:"
echo "\"$TMUX_BIN\" attach -t decomp"