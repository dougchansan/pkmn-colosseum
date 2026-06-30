#!/bin/bash
# codex_asm_driver.sh — dedicated Codex-lane driver for FROM-SCRATCH asm->C decomp of
# the largest active asm-wrappers (build/asm_codex_queue.txt, built from
# active_asm_targets.py). Runs ALONGSIDE the main fleet_driver, which keeps Opus+Sonnet
# on the near-miss/LOW buckets. Codex xhigh reasoning is spent here (higher value than
# LOW near-misses) because each win is a real asm->C conversion that advances the
# decompiled-to-C metric. Per-file lock aware (shares coordination/locks.py with the
# main driver); marks attempted in the wall ledger. Codex lanes only.
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
source tools/decomp_work/tmux_control/panes.env 2>/dev/null
export MSYS_NO_PATHCONV=1
# SINGLETON GUARD: TaskStop kills the Monitor's grep but NOT this bash loop, so naive
# restarts pile up zombie drivers that all dispatch onto the same panes at once (this
# caused 5 concurrent codex drivers). Refuse to start a 2nd instance. Stale pidfile
# (after SIGKILL) is harmless — kill -0 on a dead pid returns false.
PIDF="build/.codex_asm_driver.pid"
if [ -f "$PIDF" ] && kill -0 "$(cat "$PIDF" 2>/dev/null)" 2>/dev/null; then
  echo "[codex_asm_driver] another instance ($(cat "$PIDF")) already running — exiting"; exit 0
fi
echo $$ > "$PIDF"; trap 'rm -f "$PIDF"' EXIT
INTERVAL="${INTERVAL:-45}"
LANES="${CODEX_ASM_LANES:-C1 C2 C3 C4}"
QUEUE="build/asm_codex_queue.txt"
CONT_CAP="${CONT_CAP:-8}"           # max CONTINUE turns on one file before rotating
STATE="build/codex_lane_state"; mkdir -p "$STATE"
declare -A SEND=( [C1]=send-codex-safe [C2]=send-codex2-safe [C3]=send-codex3-safe [C4]=send-codex4-safe )
declare -A PANE=( [C1]=$CODEX_PANE [C2]=$CODEX2_PANE [C3]=$CODEX3_PANE [C4]=$CODEX4_PANE )
# a file is COMPLETE for a lane when every target fn is saved in band_wins, OR the lane
# has spent CONT_CAP continue-turns on it (walled/stuck — move on, partial work persists).
file_complete() {  # <stem> <fns...>
  local stem="$1"; shift; local wins="build/band_wins/pl_${stem}.json"
  [ -f "$wins" ] || return 1
  local fn; for fn in "$@"; do grep -q "$fn" "$wins" 2>/dev/null || return 1; done
  return 0
}
# echo only fns NOT already saved in band_wins for this stem — so we never hand an agent
# already-matched work to re-verify (token waste). Empty output => the whole file is done.
pending_fns() {  # <stem> <fns...>
  local stem="$1"; shift; local wins="build/band_wins/pl_${stem}.json" fn out=""
  for fn in "$@"; do
    [ -f "$wins" ] && grep -q "$fn" "$wins" 2>/dev/null && continue
    out="$out $fn"
  done
  echo "${out# }"
}
echo "[codex_asm_driver] up — lanes:[$LANES] from $QUEUE, interval ${INTERVAL}s, sticky (cap ${CONT_CAP}) from-scratch asm->C"
while true; do
  if [ ! -s "$QUEUE" ]; then echo "[$(date +%H:%M)] ASM queue empty — regenerate build/asm_codex_queue.txt"; sleep "$INTERVAL"; continue; fi
  LOCKS=$(python tools/decomp_work/coordination/locks.py list 2>/dev/null | awk '{print $2}')
  declare -A SNAP IDLE; RUN_PICKED=""
  # Protect EVERY lane's currently-owned file (busy lanes included) so an idle lane
  # can't grab a file another lane is mid-conversion on (would clobber the shared band
  # scratch). Seed the dedup set from all persisted lane state before any assignment.
  for n in $LANES; do
    sf=$(cat "$STATE/$n.file" 2>/dev/null)
    [ -n "$sf" ] && RUN_PICKED="${RUN_PICKED}"$'\n'"${sf}"
  done
  # timeout-guard captures: an unbounded capture that hangs (degraded tmux) freezes the
  # whole loop indefinitely. A timed-out capture just skips that lane this cycle.
  for n in $LANES; do SNAP[$n]=$(timeout 8 tmux capture-pane -p -t "${PANE[$n]}" 2>/dev/null | md5sum); done
  sleep 2
  for n in $LANES; do
    cap=$(timeout 8 tmux capture-pane -p -t "${PANE[$n]}" 2>/dev/null)
    # A lane RUNNING a turn shows "esc to interrupt" (both Claude & Codex TUIs). Never
    # treat such a pane as idle even if its screen was briefly static (slow compile /
    # thinking) — that false-positive is what made the driver fire a 2nd prompt onto a
    # working lane. Idle = NO interrupt indicator AND byte-static over the 2s window.
    if echo "$cap" | tr -d ' ' | grep -qiE "esctoint"; then IDLE[$n]=0; continue; fi
    # A RATE-LIMITED / usage-capped pane is "idle" (no turn running) but CANNOT do work —
    # dispatching onto it just queues dead prompts that retry forever. Skip such lanes.
    if echo "$cap" | grep -qiE "rate.?limit|usage limit|limit reached|too many request|try again (in|at|later)|resets? (at|in)|reached your|x402|429 "; then
      echo "[$(date +%H:%M)] RATE-LIMITED $n — skipping (no dispatch)"; IDLE[$n]=0; continue
    fi
    [ "${SNAP[$n]}" = "$(echo "$cap" | md5sum)" ] && IDLE[$n]=1 || IDLE[$n]=0
  done
  for n in $LANES; do
    [ "${IDLE[$n]:-0}" = 1 ] || continue
    curfile=$(cat "$STATE/$n.file" 2>/dev/null); curfns=$(cat "$STATE/$n.fns" 2>/dev/null)
    cnt=$(cat "$STATE/$n.cnt" 2>/dev/null || echo 0)
    # STICKY: stay on the current file (CONTINUE) until all targets saved or cap hit.
    if [ -n "$curfile" ]; then
      stem=$(basename "$curfile" .c); tag="pl_${stem}"
      if file_complete "$stem" $curfns; then
        echo "[$(date +%H:%M)] ASM-ROTATE $n off $stem (all targets saved)"; rm -f "$STATE/$n".{file,fns,cnt}; curfile=""
      elif [ "$cnt" -ge "$CONT_CAP" ]; then
        echo "[$(date +%H:%M)] ASM-ROTATE $n off $stem (cap ${CONT_CAP} reached)"; rm -f "$STATE/$n".{file,fns,cnt}; curfile=""
      else
        cnt=$((cnt+1)); echo "$cnt" > "$STATE/$n.cnt"
        RUN_PICKED="${RUN_PICKED}"$'\n'"${curfile}"
        pend=$(pending_fns "$stem" $curfns)   # drop already-saved fns from the ask
        prompt="CONTINUE iterating $curfile (TAG $tag). Your prior C in the band scratch is PRESERVED — build ON it, do NOT re-draft from m2c. band.py check to see current per-fn match%; for each fn below not yet 100%, diff the residual, run classify_residual.py $tag <fn>, apply the lever, and push the EXISTING scratch higher (named locals, no rNN). Each turn must raise the %, not reset it. At 100% band.py save. KG: kg.py q lever-targets <fn> BEFORE; kg.py record-crack <fn> <lever-slug> AFTER each save (record-lever if NEW). fns (NOT-yet-saved only): $pend. Report SAVED/WALL/SKIP per fn; say FILE-DONE when all resolved."
        ./tools/decomp_work/tmux_control/control.sh "${SEND[$n]}" "$prompt" >/dev/null 2>&1
        echo "[$(date +%H:%M)] ASM-CONTINUE $n -> $stem (turn $cnt/$CONT_CAP)"
        continue
      fi
    fi
    # assign a NEW file (lane has none, or just rotated off). Skip files whose target fns
    # are ALL already saved (band_wins) — re-handing matched work just burns tokens.
    file=""; fns=""
    while IFS= read -r l; do
      [ -n "$l" ] || continue
      f=$(echo "$l" | awk '{print $1}')
      echo "$LOCKS" | grep -qxF "$f" && continue
      printf '%s\n' "$RUN_PICKED" | grep -qxF "$f" && continue
      cstem=$(basename "$f" .c); cand=$(echo "$l" | cut -d' ' -f2-)
      pend=$(pending_fns "$cstem" $cand)
      [ -z "$pend" ] && continue          # whole file already saved -> skip
      file="$f"; fns="$pend"; break
    done < "$QUEUE"
    [ -n "$file" ] || { echo "[$(date +%H:%M)] ASM-QUEUE: $n idle, no file with unsaved fns (locked/done)"; continue; }
    stem=$(basename "$file" .c); tag="pl_${stem}"
    RUN_PICKED="${RUN_PICKED}"$'\n'"${file}"
    echo "$file" > "$STATE/$n.file"; echo "$fns" > "$STATE/$n.fns"; echo 0 > "$STATE/$n.cnt"
    for fn in $fns; do python tools/decomp_work/wall_ledger.py mark "$fn" "$n/asm" >/dev/null 2>&1; done
    prompt="ASM->C (read docs/CRACK_LEVERS.md). File: $file  TAG: $tag  fns: $fns. Run python tools/decomp_work/band.py init $tag $file — it is RESUME-SAFE: any prior C already in the scratch is PRESERVED (not wiped), so you build on accumulated progress across visits. Then band.py check $tag for current per-fn match%. For EACH target fn: if the scratch fn is still the raw asm-wrapper (~0%), m2c_draft + write faithful REAL C with NAMED locals + in-body externs (no rNN register-locals, no asm/.inc fraud); if it ALREADY has partial C (>0%), ITERATE it toward 100% — diff the residual, run classify_residual.py $tag <fn>, apply the lever. Every turn must RAISE the scratch %, never restart it. At 100% band.py save (flips #if1->#if0). KG (shared lever memory): BEFORE each fn run python tools/decomp_work/kg/kg.py q lever-targets <fn> for proven levers; AFTER each SAVE run python tools/decomp_work/kg/kg.py record-crack <fn> <lever-slug> (and kg.py record-lever <slug> --title \"...\" --desc \"...\" if you found a NEW lever) so every lane reuses it. Report SAVED/WALL/SKIP per fn; say FILE-DONE when all targets resolved."
    ./tools/decomp_work/tmux_control/control.sh "${SEND[$n]}" "$prompt" >/dev/null 2>&1
    echo "[$(date +%H:%M)] ASM-DISPATCH $n -> $stem (${fns%% *} +$(($(echo $fns | wc -w)-1)) more)"
  done
  unset SNAP IDLE
  sleep "$INTERVAL"
done
