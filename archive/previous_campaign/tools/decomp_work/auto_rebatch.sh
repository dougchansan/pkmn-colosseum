#!/bin/bash
# auto_rebatch.sh — autonomous lane refill, LOCK-AWARE, DUAL-QUEUE, TMUX-FREE.
#
# 2026-06-20 rearchitecture: this script no longer touches psmux at all. It reads
# idle state from build/hb/<NAME>.state (written by pane_io.sh, the sole tmux owner)
# and dispatches by writing build/dispatch/<NAME>.req (drained + sent by pane_io).
# That removes the 4-deep nested-bash pipeline that drove native-PE tmux clients and
# wedged the loop. The driver can no longer hang on a tmux call.
#
# Feeds idle lanes from two queues, highest-value first:
#   1. build/wall_queue.txt  — NEARWALL fns (real C @95-99.95%): CRACK with CW levers.
#   2. build/asm_queue.txt   — active asm-wrappers: from-scratch decomp.
# Each lane gets a DISTINCT file that is neither already-assigned nor band-locked.
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
export MSYS_NO_PATHCONV=1
HB=build/hb; REQ=build/dispatch; mkdir -p "$HB" "$REQ"

LANES="${ASM_LANES:-OPUS SON C1 C2}"
# Lanes that run Sonnet/Haiku (weaker/cheaper models): they get the SMALL-fn sonnet_queue
# instead of the hard-grind crack/scratch queues. One name per line in build/sonnet_lanes.txt.
SONNET_LANES=$(tr -d '\r' < build/sonnet_lanes.txt 2>/dev/null | tr '\n' ' ' | sed 's/  */ /g')
# Farming lanes: draw from build/farm_queue.txt and get a PORT-FROM-MELEE packet (port
# matched C from decomp-refs/melee for shared SDK/HSD TUs) instead of grind-from-scratch.
# One lane name per line in build/farm_lanes.txt; ramp up by adding lanes.
FARM_LANES=$(tr -d '\r' < build/farm_lanes.txt 2>/dev/null | tr '\n' ' ' | sed 's/  */ /g')
FRESH="${HB_FRESH:-300}"  # a state/alive file older than this many seconds is not trusted.
# Raised 45->300: under heavy host load pane_io's capture pass can take minutes (observed
# ~234s), so 45s rejected every lane as "stale" and the fleet stopped dispatching entirely.
# The dispatch cooldown below still prevents re-tasking a lane too soon, so a looser window
# is safe; it just lets the driver keep working through a sluggish control loop.
# Dispatch cooldown: after a lane is tasked, do not re-task it for this many seconds even if
# it reads idle. A freshly-prompted agent briefly looks idle while it starts (one static
# pane_io pass, no "esc to interrupt"); without this it gets re-prompted before finishing its
# first assignment. An actively-working agent reads busy well within this window, so the only
# cost is a lane that genuinely finished fast waiting a bit for its next packet.
COOLDOWN="${DISPATCH_COOLDOWN:-45}"   # was 150: a lane finishing fast then idled the rest
# of the window for no reason. 45s still covers the dispatch->busy race (pane_io marks a lane
# busy within ~8-16s of the agent showing its first work marker), with the IDLE_MIN debounce
# as backstop, so we never re-prompt an agent that is merely still starting its packet.

# The tmux owner must be alive and recently active, else states are stale and reqs we
# write would never be sent. Refuse to dispatch — safer than queuing into the void.
now=$(date +%s)
alive=$(cat "$HB/.alive" 2>/dev/null || echo 0)
if [ $(( now - ${alive:-0} )) -gt "$FRESH" ]; then
  echo "PANE-IO-DOWN (alive=${alive}, now=${now}) — not dispatching this cycle"; exit 0
fi

locked_files() { timeout 15 python tools/decomp_work/coordination/locks.py list 2>/dev/null | awk '{print $2}'; }

# idle = state file says idle, is fresh, AND no dispatch req already pending for it
declare -A IDLE
for name in $LANES; do
  IDLE[$name]=0
  [ -f "$REQ/$name.req" ] && continue                       # a prompt is already queued/unsent
  # cooldown: skip a lane that was tasked within the last COOLDOWN seconds (its .task mtime is
  # the last-dispatch time) so we never re-prompt an agent still starting/working its packet.
  tf="$REQ/$name.task"
  if [ -f "$tf" ]; then
    tmt=$(stat -c %Y "$tf" 2>/dev/null || echo 0)
    [ $(( now - ${tmt:-0} )) -lt "$COOLDOWN" ] && continue
  fi
  read -r st ts _ < "$HB/$name.state" 2>/dev/null || continue
  [ "$st" = idle ] || continue
  [ $(( now - ${ts:-0} )) -le "$FRESH" ] && IDLE[$name]=1
done

LOCKS=$(locked_files)
# Spawn-tax-immune contention guard: files already owned by another busy/pending lane.
# locks.py (python+sqlite) is slow / times out under the MSYS process-spawn tax, so its
# list silently empties and two lanes draw the same file. This pure-file check is the
# reliable backstop: derive each lane's file from its pending .req or active .task
# ("File: <path>"), count it taken only while that lane is busy or has an unsent req
# (an idle lane's last file is finished and free to reassign).
BUSY_FILES=""
for _o in $LANES; do
  _of=""
  for _f in "$REQ/$_o.req" "$REQ/$_o.task"; do
    [ -f "$_f" ] || continue
    _of=$(sed -n 's/.*File: \([^ ]*\).*/\1/p' "$_f" | head -1); [ -n "$_of" ] && break
  done
  [ -n "$_of" ] || continue
  read -r _ost _ < "$HB/$_o.state" 2>/dev/null || _ost=""
  if [ "$_ost" = busy ] || [ -f "$REQ/$_o.req" ]; then BUSY_FILES="${BUSY_FILES}"$'\n'"${_of}"; fi
done
RUN_PICKED=""
pick_line() {  # pick_line <queue> -> first line whose file is free + unlocked + unowned + unpicked-this-run
  local queue="$1" line f
  [ -f "$queue" ] || return 1
  while IFS= read -r line; do
    [ -n "$line" ] || continue
    f=$(echo "$line" | awk '{print $1}')
    echo "$LOCKS" | grep -qxF "$f" && continue
    printf '%s\n' "$BUSY_FILES" | grep -qxF "$f" && continue   # owned by another busy/pending lane
    printf '%s\n' "$RUN_PICKED" | grep -qxF "$f" && continue
    echo "$line"; return 0
  done < "$queue"
  return 1
}

for name in $LANES; do
  [ "${IDLE[$name]:-0}" = 1 ] || continue
  # Mode order: crack (wall_queue, real-C near-misses) first by default. When build/.scratch_first
  # exists, do from-scratch ASM (asm_queue) FIRST instead — set when the near-miss crack pool is
  # mined out and the fleet should grind asm-wrappers into C. Either mode falls back to the other
  # when its queue is empty, so a lane is never left idle while work of any kind remains.
  if printf '%s\n' $FARM_LANES | grep -qxF "$name"; then
    # Farming lane: port matched C from decomp-refs/melee for shared SDK/HSD TUs.
    # Falls back to the crack queue when the farm queue is dry.
    mode=farm; line=$(pick_line build/farm_queue.txt)
    [ -z "$line" ] && { mode=crack; line=$(pick_line build/wall_queue.txt); }
  elif printf '%s\n' $SONNET_LANES | grep -qxF "$name"; then
    # Sonnet/Haiku lane: SMALL real-C fns only (capability-matched). Fall back to the crack
    # queue only if the small-fn queue is dry.
    mode=crack; line=$(pick_line build/sonnet_queue.txt)
    [ -z "$line" ] && line=$(pick_line build/wall_queue.txt)
  elif [ -f build/.reshape_first ]; then
    # LOW Ghidra-draft reshape focus (operator goal: close the LOW bucket). From-scratch
    # structural rework FIRST; fall back to the small-fn queue (then crack) so a lane is
    # never idle when the reshape queue thins.
    mode=reshape; line=$(pick_line build/reshape_queue.txt)
    [ -z "$line" ] && { mode=crack; line=$(pick_line build/sonnet_queue.txt); }
    [ -z "$line" ] && { mode=crack; line=$(pick_line build/wall_queue.txt); }
  elif [ -f build/.scratch_first ]; then
    mode=scratch; line=$(pick_line build/asm_queue.txt)
    [ -z "$line" ] && { mode=crack; line=$(pick_line build/wall_queue.txt); }
  else
    mode=crack; line=$(pick_line build/wall_queue.txt)
    [ -z "$line" ] && { mode=scratch; line=$(pick_line build/asm_queue.txt); }
    # Final fallback for crack lanes: the SMALL real-C fn queue (fresh, capability-safe,
    # never a walled-TU re-grind). Without this, once the pins/crack/asm queues drain the
    # 4 non-SON lanes (OPUS GLM C3 C4) hit QUEUE-EXHAUSTED and idle overnight — only SON
    # could reach sonnet_queue. Crack lanes are at least as capable on these small fns.
    [ -z "$line" ] && { mode=crack; line=$(pick_line build/sonnet_queue.txt); }
  fi
  if [ -z "$line" ]; then echo "QUEUE-EXHAUSTED — $name idle, no free unlocked target"; continue; fi
  file=$(echo "$line" | awk '{print $1}')
  # Small packets (default 2 fns) keep each agent turn short so it CANNOT grind one packet for
  # hours (we saw 3h / 290k-token single turns). A fn that resists is re-offered on a later
  # cycle rather than burning a marathon turn; combined with the hard-stop prompt below.
  # Bounded packets. Farm gets a slightly larger packet (porting is fast) but NOT the
  # whole TU — whole-TU ports overwrote already-matched fns and regressed match%.
  if [ "$mode" = farm ]; then
    fns=$(echo "$line" | cut -d' ' -f2-$(( ${FARM_FNS:-4} + 1 )))
  elif [ "$mode" = reshape ]; then
    fns=$(echo "$line" | cut -d' ' -f2-2)   # ONE big draft per packet — reshape is heavy
  else
    fns=$(echo "$line" | cut -d' ' -f2-$(( ${PACKET_FNS:-2} + 1 )))
  fi
  stem=$(basename "$file" .c); tag="pl_${stem}"
  RUN_PICKED="${RUN_PICKED}"$'\n'"${file}"
  # Background the ledger marks: under the MSYS python-spawn tax (~8s/spawn observed under
  # full load) a synchronous per-fn mark loop blows past the driver's auto_rebatch timeout,
  # so the dispatch is KILLED before any .req is written and idle lanes never refill. The
  # marks only need to land before the next cycle's gen (~30s), so fire-and-forget keeps the
  # .req write — the thing that actually feeds the lane — off the slow critical path.
  ( for fn in $fns; do timeout 10 python tools/decomp_work/wall_ledger.py mark "$fn" "$name/$mode" >/dev/null 2>&1; done ) &
  gate="BEFORE any WALL you MUST run: python tools/decomp_work/classify_residual.py $tag <fn>. If it prints REG-COLORING (exit 0) you may NOT wall it — rewrite with NAMED locals (never raw rNN locals, they pin the coloring) + declaration-order lever until 100. Only RELOC/SCHEDULING/SHAPE verdicts may WALL/REWORK."
  ff=""; case "$name" in C1|C2|C3|C4) ff="If a fn still resists after the classifier says REG-COLORING and ~4 decl-order attempts, leave it in scratch and report WALL <fn> <%> + the classifier verdict, then move on.";; esac
  hardstop="HARD STOP — do NOT over-grind: at most ~4 attempts / ~15 min PER FN. If a fn still resists, report WALL <fn> <%> and MOVE ON immediately. Do ONLY this packet ($fns) then END YOUR TURN — do not chain to other files or keep iterating a wall for hours."
  bankstep="BANK NEAR-MISSES: if a fn reaches a real-C near-miss (>=90%, genuine C — NO asm/.inc) that you cannot push to 100%, run \`python tools/decomp_work/band.py bank $tag <fn>\` BEFORE you report WALL/STRUCT. That hands your near-miss to the permuter to finish instead of discarding it when the scratch is reused. (bank rejects <90% and 100%; keep using band.py save for true 100% wins.)"
  bankfilestep="BANK THE FILE: if you improved this file (raised one or more fns) but cannot hit 100%, run \`python tools/decomp_work/band.py bank-file $tag\` BEFORE reporting STRUCT/WALL — that snapshots your WHOLE-file reshape (its added externs/helpers/sibling edits) so the permuter can finish it; a single-fn bank would lose that context and collapse. The strict all-fns no-regress verify happens later at integrate time, so banking is always safe."
  if [ "$mode" = farm ]; then
    prompt="FARM (port matched C from the Melee decomp; read tools/decomp_work/SDK_FARMING.md). File: $file  TAG: $tag  fns: $fns. These are shared SDK/HSD functions already decompiled upstream. *** SCOPE — CRITICAL: edit ONLY the listed fns ($fns). Do NOT touch, reformat, or re-port ANY other function in $file — every already-decompiled function must stay byte-for-byte unchanged. (A prior whole-TU farm overwrote already-matched fns and regressed the match%.) NO-REGRESS: before editing, run python tools/compile_check.py $file --compiler-version 1.2.5n --diff to note each listed fn's current %. Keep your port if it 100%-matches OR raises the fn's %; if the fn was an asm-wrapper/stub, keep the C port even if still partial (that is real-C progress — a later crack pass finishes it). REVERT a port ONLY if it would make a fn LOWER than it already was. *** RECIPE (proven on hsd_mtx): (1) for EACH fn, dump its target asm and list its bl-targets; MAP it to the matching upstream function by call-fingerprint + signature, searching RECURSIVELY in BOTH refs: HSD -> decomp-refs/melee/src/sysdolphin/baselib/ ; Dolphin SDK (incl gx/ vi/ os/ dvd/ pad/ subdirs) -> decomp-refs/melee/extern/dolphin/src/dolphin/ AND decomp-refs/sms/src/dolphin/ (Sunshine's GX library is more complete). Aggregated TUs like GXInit pull fns from many GX source files (GXInit.c/GXTransform.c/GXLight.c/GXTexture.c/...), so grep the fn's call-targets across the whole gx/ dir. DISTRUST the existing stub comments — they are often wrong. (2) Port melee's C verbatim, naming the fn by its canonical symbol (HSD_*/OS*/GX*/etc., NOT fn_), and add a LOCAL typedef preamble for any missing types (e.g. 'typedef f32 Mtx[3][4];' a 3-f32 Vec3, math/MTX/VEC externs by name) — do NOT #include melee headers. (3) Measure: python tools/compile_check.py $file --compiler-version 1.2.5n --diff --symbols <canonical names>. HSD compiles at 1.2.5n; if a fn near-misses on prologue/epilogue, retry --compiler-version 1.2.5. (4) If a fn is NOT present in melee's matching source (aggregated from another upstream TU), report NOT-FARMABLE <fn> and MOVE ON — do not grind it. Real C only (no asm/.inc/rNN-locals); git diff must touch only .c/.h. AFTER each match run python tools/decomp_work/kg/kg.py record-crack <fn> sdk-farm-melee. $hardstop SAVED <fn> 100.00 / NOT-FARMABLE <fn>."
  elif [ "$mode" = crack ]; then
    prompt="CRACK (levers: read docs/CRACK_LEVERS.md). File: $file  TAG: $tag  fns: $fns. These are real C in canon <100%. For EACH fn: band.py diff, then classify_residual.py to pick the fix (REG-COLORING=named-locals+decl-order; SHAPE=m2c_draft reshape). Fix scratch, band.py check, save at 100. Real C only (no asm/.inc/rNN-locals). KG (shared lever memory): BEFORE each fn run python tools/decomp_work/kg/kg.py q lever-targets <fn> for proven levers; AFTER each save run python tools/decomp_work/kg/kg.py record-crack <fn> <lever-slug> (and python tools/decomp_work/kg/kg.py record-lever <slug> --title \"...\" --desc \"...\" if you found a NEW lever) so every lane reuses it. $gate $ff $bankstep $hardstop SAVED <fn> 100.00 / WALL <fn> <%>."
  elif [ "$mode" = reshape ]; then
    prompt="RESHAPE (LOW real-C DRAFT -> faithful real C). File: $file  TAG: $tag  fns: $fns. This is a <70% Ghidra/m2c DRAFT that needs STRUCTURAL rework, NOT near-miss crack levers. STEPS: (1) read/regenerate the m2c draft (python tools/decomp_work/m2c_draft.py if present) AND dump the target asm so you know the real control-flow; (2) REWRITE the body as faithful real C — correct SIGNATURE, control-flow (loops/switches/if-else), and types, with NAMED locals (in-body externs; NEVER raw rNN register-locals); (3) band.py diff to measure; iterate on SHAPE (control-flow + types) FIRST, then decl-order. Real C only (no asm/.inc/rNN-locals). KG (shared lever memory): BEFORE run python tools/decomp_work/kg/kg.py q lever-targets <fn>; AFTER a save run python tools/decomp_work/kg/kg.py record-crack <fn> <lever-slug>. $gate $bankfilestep HARD STOP — do ONLY this ONE fn ($fns), at most ~30 min, then END YOUR TURN. Report exactly one of: SAVED <fn> 100.00 / STRUCT <fn> <%> (improved >=70%, hand off for crack) / WALL <fn> <%>. Do NOT chain to other fns or grind for hours."
  else
    prompt="FROM-SCRATCH asm->C (levers: docs/CRACK_LEVERS.md). File: $file  TAG: $tag  fns: $fns. m2c_draft each, then REWRITE into faithful REAL C with NAMED locals (in-body externs) — no raw rNN register-locals. band.py check, save at 100. No asm/.inc fraud. KG (shared lever memory): BEFORE each fn run python tools/decomp_work/kg/kg.py q lever-targets <fn> for proven levers; AFTER each save run python tools/decomp_work/kg/kg.py record-crack <fn> <lever-slug> (and python tools/decomp_work/kg/kg.py record-lever <slug> --title \"...\" --desc \"...\" if you found a NEW lever) so every lane reuses it. $gate $ff $bankstep $hardstop SAVED/WALL/SKIP per fn."
  fi
  printf '%s' "$prompt" > "$REQ/$name.req"        # pane_io sends this; never touch tmux here
  echo "REBATCH $name [$mode] -> $stem [$fns]"
done
