#!/bin/bash
# run_deepseek_pane.sh — DeepSeek cloud lane in a visible WSL pane.
# Forwards config to the Windows python via WSLENV (WSL->Win env bridge).
cd /mnt/c/Users/douglaswhittingham/pkmn-colosseum || exit 1
PYW="/mnt/c/Users/douglaswhittingham/AppData/Local/Programs/Python/Python312/python.exe"
export MODELS='deepseek/deepseek-v4-flash'
export AGENT_NAME='deepseek-v4-flash'
export TIER_FILTER='codex,deepseek,kimi'
export MAX_FN_ASM_LINES='240'
# Broad-drain mode: attack any eligible queued fn (not just >=60% near-misses with
# a seed). Set MIN_PRIOR_PCT=60 / REQUIRE_SEED=1 to return to marginal-gains cracker mode.
export MIN_PRIOR_PCT='0'
export REQUIRE_SEED='0'
export REPAIR_ROUNDS='8'
export PLATEAU_BREAK_AFTER='3'
# FILE_FILTER: queue_attack.py defaults to a narrow hsd/gs whitelist with no queued
# deepseek/kimi work -> "queue empty for filter". Point it at the stems that actually
# have eligible queued tasks (regenerate this list if the lane drains).
export FILE_FILTER='fsys_file,gs_field_world,gs_npc_interact,gs_pcbox,gs_pokemon_summary,gs_render,gs_scene,gs_thread,gs_worldmap,hsd_lobj,people,scene_init,ui_core'
export WSLENV='MODELS:AGENT_NAME:TIER_FILTER:MAX_FN_ASM_LINES:MIN_PRIOR_PCT:REQUIRE_SEED:REPAIR_ROUNDS:PLATEAU_BREAK_AFTER:FILE_FILTER'
echo "[deepseek] starting (model=$MODELS tiers=$TIER_FILTER cap=$MAX_FN_ASM_LINES min_prior=$MIN_PRIOR_PCT require_seed=$REQUIRE_SEED) ..."
while true; do
  "$PYW" tools/decomp_work/overnight/queue_attack.py 2>&1
  echo "[deepseek] worker exited rc=$? — restarting in 10s (Ctrl-C to stop)"
  sleep 10
done
