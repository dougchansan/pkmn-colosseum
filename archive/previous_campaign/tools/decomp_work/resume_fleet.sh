#!/bin/bash
# resume_fleet.sh — rebuild the decomp + PC-port fleet in tmux session 0 after a crash.
# Panes: %0 claude/orchestrator | %1 quantum | %2 pipeline | %3 decomp-codex | %4 GLM
#        %5 research | %6 deepseek | %7 activity-matrix | %8 pc-port codex
# Run from WSL. See docs/ORCHESTRATOR_HANDOFF.md for the operating playbook.
set -uo pipefail
REPO=/mnt/c/Users/douglaswhittingham/pkmn-colosseum
PYW="/mnt/c/Users/douglaswhittingham/AppData/Local/Programs/Python/Python312/python.exe"
cd "$REPO"

echo "[resume] building panes (8 alongside %0)..."
for i in 1 2 3 4 5 6 7 8; do
  tmux split-window -t 0:0 "bash -lc 'cd $REPO; exec bash'" 2>/dev/null || true
  tmux select-layout -t 0:0 even-horizontal >/dev/null 2>&1 || true
done
sleep 2
tmux list-panes -t 0:0 -F "#{pane_index} #{pane_id}"

echo "[resume] backends (annealer throttled + research daemon)..."
GRIND_WORKERS=2 GRIND_JOBS=2 setsid nohup bash tools/decomp_work/permuter/anneal_supervisor.sh \
  >tools/decomp_work/permuter/logs/anneal_supervisor.out 2>&1 </dev/null &
setsid nohup python3 tools/decomp_work/permuter/research_daemon.py \
  >tools/decomp_work/permuter/logs/research_daemon.out 2>&1 </dev/null &

# Pane indices are positional after even-horizontal; map by index 1..8.
P() { tmux list-panes -t 0:0 -F "#{pane_index} #{pane_id}" | awk -v i="$1" '$1==i{print $2}'; }
for i in 1 2 3 4 5 6 7 8; do tmux send-keys -t "$(P $i)" "cd $REPO" Enter; done
sleep 1

echo "[resume] dashboards..."
tmux send-keys -t "$(P 1)" "clear; python3 tools/decomp_work/permuter/quantum_dash.py" Enter
tmux send-keys -t "$(P 2)" "clear; python3 tools/decomp_work/permuter/pipeline_dash.py" Enter
tmux send-keys -t "$(P 5)" "clear; python3 tools/decomp_work/permuter/research_log_view.py" Enter
tmux send-keys -t "$(P 7)" "clear; bash tools/decomp_work/activity_matrix.sh" Enter

echo "[resume] agents..."
tmux send-keys -t "$(P 3)" "codex --model gpt-5.5 -c model_reasoning_effort=high --dangerously-bypass-approvals-and-sandbox" Enter
tmux send-keys -t "$(P 8)" "codex --model gpt-5.5 -c model_reasoning_effort=high --dangerously-bypass-approvals-and-sandbox" Enter
tmux send-keys -t "$(P 4)" "opencode --model ollama-cloud/glm-5.1" Enter
tmux send-keys -t "$(P 6)" "bash tools/decomp_work/overnight/run_deepseek_pane.sh" Enter

echo "[resume] DONE. Dismiss any codex/opencode update prompts, then dispatch tasks per"
echo "         docs/ORCHESTRATOR_HANDOFF.md. (3090/qwen stays OFF — energy policy.)"
echo "         Requeue if deepseek queue-empty: flip failed<=400 -> queued in tasks.json."
