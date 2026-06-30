#!/bin/bash
# Launch multi-agent decomp pipeline in tmux main-horizontal layout
# Top pane: status monitor
# Bottom left: Ollama/Kimi generator (Lane 1)
# Bottom center: Verify/Apply (Lane 2)
# Bottom right: Review queue watcher (Lane 3)

TMUX="$LOCALAPPDATA/Microsoft/WinGet/Packages/arndawg.tmux-windows_Microsoft.Winget.Source_8wekyb3d8bbwe/tmux.exe"
SESSION="decomp"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

# Kill existing session
"$TMUX" kill-session -t "$SESSION" 2>/dev/null

# Create session with status monitor (top pane)
"$TMUX" new-session -d -s "$SESSION" -x 200 -y 50 \
  "cd '$ROOT' && echo '=== DECOMP STATUS MONITOR ===' && watch -n 10 'python tools/decomp_scheduler.py --status 2>/dev/null; echo ---; echo Candidates: $(ls tools/decomp_work/candidates/*.json 2>/dev/null | wc -l); echo Review: $(ls tools/decomp_work/review/*.json 2>/dev/null | wc -l); echo Applied: $(ls tools/decomp_work/applied/*.json 2>/dev/null | wc -l)'"

# Split bottom for Lane 1: Ollama generator
"$TMUX" split-window -t "$SESSION" -v -l 60% \
  "cd '$ROOT' && echo '=== LANE 1: OLLAMA + KIMI GENERATOR ===' && python tools/decomp_work/lane_ollama.py; echo 'Lane 1 finished. Press enter.'; read"

# Split Lane 2: Verify/Apply
"$TMUX" split-window -t "$SESSION" -h -l 50% \
  "cd '$ROOT' && echo '=== LANE 2: VERIFY + APPLY ===' && python tools/decomp_work/lane_verify.py; echo 'Lane 2 finished. Press enter.'; read"

# Split Lane 3: Review watcher
"$TMUX" split-window -t "$SESSION" -h -l 50% \
  "cd '$ROOT' && echo '=== LANE 3: REVIEW QUEUE ===' && watch -n 5 'echo Review items:; for f in tools/decomp_work/review/*.json; do [ -f \"\$f\" ] && python -c \"import json; d=json.load(open(\\\"\$f\\\")); print(f\\\"  {d[\\\\\"function\\\"]:20s} {d.get(\\\\\"status\\\\\",\\\\\"?\\\\\"):12s} {d[\\\\\"file\\\\\"]}\\\")\" 2>/dev/null; done; echo ---; echo \"These need Codex/Claude fixes\"'"

# Select main layout
"$TMUX" select-layout -t "$SESSION" main-horizontal

# Attach
"$TMUX" attach -t "$SESSION"
