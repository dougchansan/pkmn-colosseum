#!/bin/bash
# anneal_pragma.sh — run the decomp-permuter for ONE function under a SWEEP of
# CodeWarrior file-scope PRAGMA configurations, splitting a total time budget.
#
# WHY: pragmas (scheduling / peephole / fp_contract) are the single biggest
# missing CW lever in the permuter — they cannot be expressed as pycparser AST
# mutations (pycparser strips `#pragma`). We inject them at COMPILE time instead:
# a per-config compile.sh variant prepends the chosen `#pragma ...` line(s) to the
# C file before invoking mwcceppc. File-scope pragmas apply to the function body.
#
# usage (from WSL):
#   ./anneal_pragma.sh <fn_name> <src/game/file.c> [total_budget_s] [jobs]
#
# Configs swept (budget is split evenly across all five):
#   1. (none — baseline)
#   2. #pragma scheduling on
#   3. #pragma peephole off
#   4. #pragma scheduling on + #pragma peephole off
#   5. #pragma fp_contract on
#
# Emits one of:
#   WIN <fn> <config>          score-0 exact match; winning C copied to wins/<fn>.c
#                              and a line `<fn> <src> <config>` appended to
#                              wins/MANIFEST.txt. STOPS the sweep on first win.
#   NOWIN <fn> <best> <config> best score reached across all configs, no exact match
#   FAIL <fn> <reason>         dir build failed, or no config produced a valid object
#
# Only writes under tools/decomp_work/permuter/. Never edits src/ or .inc.
set -uo pipefail
REPO="/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
PERM="$REPO/tools/decomp_work/refs/decomp-permuter"
PDIR="$REPO/tools/decomp_work/permuter"
FN="$1"; SRC="$2"; TOTAL="${3:-900}"; JOBS="${4:-8}"
DIR="$PDIR/dirs/$FN"
mkdir -p "$PDIR/wins" "$PDIR/logs"

# 1. build the dir (preprocess + isolate + assemble target.o + tuned settings).
#    build_dir.sh now writes the [weight_overrides] block into settings.toml.
if ! bash "$PDIR/build_dir.sh" "$FN" "$SRC" > "$PDIR/logs/build_$FN.log" 2>&1; then
  echo "FAIL $FN build_dir"; exit 0
fi
[ -f "$DIR/base.c" ] && [ -f "$DIR/target.o" ] || { echo "FAIL $FN missing_base_or_target"; exit 0; }

# Preserve the un-pragma'd compile.sh so we can restore it on exit.
ORIG_CC="$DIR/compile.sh"
SAVED_CC="$DIR/compile.orig.sh"
cp "$ORIG_CC" "$SAVED_CC"
restore() { cp "$SAVED_CC" "$ORIG_CC"; chmod +x "$ORIG_CC"; }
trap restore EXIT

# Per-config pragma lines. Index-aligned with CFG_NAME.
CFG_NAME=(none sched_on peephole_off sched_on_peephole_off fp_contract_on)
CFG_PRAG=(
  ""
  '#pragma scheduling on'
  '#pragma peephole off'
  '#pragma scheduling on\n#pragma peephole off'
  '#pragma fp_contract on'
)

NCFG=${#CFG_NAME[@]}
# Split the total budget evenly; floor at 30s so a config always gets a real run.
PER=$(( TOTAL / NCFG ))
[ "$PER" -lt 30 ] && PER=30

# Build a pragma-injecting compile.sh variant for config index $1 (prag in $2).
# The permuter calls `compile.sh <C_FILE> -o <OUT_O>`; we prepend the pragma to a
# scratch copy of the C file, then hand THAT to the real compile.sh ($1=C,$3=.o).
write_cc() {
  local prag="$1" out="$2"
  cat > "$out" <<EOF
#!/bin/bash
set -euo pipefail
IN_C="\$1"
PRAG_C="\$IN_C.prag.c"
( printf '${prag}\n'; cat "\$IN_C" ) > "\$PRAG_C"
exec bash "$SAVED_CC" "\$PRAG_C" "\$2" "\$3"
EOF
  chmod +x "$out"
}

BEST=999999
BEST_CFG="none"
WON=""

for ((i=0; i<NCFG; i++)); do
  NAME="${CFG_NAME[$i]}"
  PRAG="${CFG_PRAG[$i]}"
  CCV="$DIR/compile_${NAME}.sh"
  RUNLOG="$PDIR/logs/run_${FN}_${NAME}.log"

  if [ -z "$PRAG" ]; then
    # baseline: just the saved (un-pragma'd) compile wrapper.
    cp "$SAVED_CC" "$CCV"; chmod +x "$CCV"
  else
    write_cc "$PRAG" "$CCV"
  fi

  # Point the dir's compile.sh (what the permuter invokes) at this variant.
  cp "$CCV" "$ORIG_CC"; chmod +x "$ORIG_CC"

  # 2a. SANITY: confirm the pragma-injected baseline still produces a valid object
  #     before trusting any scores from this config. A broken pragma (compile
  #     error) must not silently poison the sweep.
  PROBE_O="$DIR/_probe_${NAME}.o"
  if ! bash "$ORIG_CC" "$DIR/base.c" -o "$PROBE_O" > "$PDIR/logs/probe_${FN}_${NAME}.log" 2>&1; then
    echo "  [$NAME] SKIP — pragma-injected compile failed (see logs/probe_${FN}_${NAME}.log)"
    rm -f "$PROBE_O"
    continue
  fi
  rm -f "$PROBE_O"

  # Clear any output-* dirs from a previous config so the post-run harvest only
  # ever sees THIS config's outputs (the permuter never deletes them itself).
  rm -rf "$DIR"/output-* 2>/dev/null || true

  # 2b. run the permuter for this config's slice of the budget.
  #     PROVEN CAPTURE (recover_one.sh): --stop-on-zero WITHOUT --best-only. With
  #     --best-only the score-0 candidate's output-0-* dir was not retained where
  #     the harvest could find it (it printed NOWIN ... 0 and LOST the win for
  #     fn_800084C0). Without --best-only, every improving result — including the
  #     score-0 — is written to output-<score>-<ctr>/source.c, so we can harvest.
  ( cd "$DIR" && timeout "$PER" python3 "$PERM/permuter.py" . -j "$JOBS" \
      --stop-on-zero > "$RUNLOG" 2>&1 )

  # 3. harvest this config. TWO distinct score-0 paths must BOTH be caught:
  #
  #   (a) MUTATION win: a permuted candidate reached score 0. post_score() wrote
  #       it to output-0-<ctr>/source.c. Harvest that file.
  #
  #   (b) BASE win: the UNMUTATED base.c already scores 0 under this pragma. The
  #       permuter prints "base score = 0" then "Found zero score! Exiting" but
  #       NEVER calls write_candidate for the base, so NO output-0-* dir is ever
  #       created. This is exactly the bug that lost fn_800084C0 (peephole_off):
  #       the old harvest only looked for output-0-* and printed NOWIN 0. Here we
  #       also detect "base score = 0" / "score = 0" in the run log and harvest
  #       the dir's own base.c — which, under this config's pragma, is the match.
  WINDIR=$(ls -d "$DIR"/output-0-* 2>/dev/null | head -1)
  if [ -n "$WINDIR" ] && [ -f "$WINDIR/source.c" ]; then
    cp "$WINDIR/source.c" "$PDIR/wins/$FN.c"
    echo "$FN $SRC $NAME" >> "$PDIR/wins/MANIFEST.txt"
    WON="$NAME"
    BEST=0; BEST_CFG="$NAME"
    echo "  [$NAME] score 0 — WIN (mutation; wins/$FN.c via $NAME)"
    break
  fi
  # base-already-zero: log shows a zero score but no output dir was produced.
  if grep -qE '(base score = 0|score = 0\b|Found zero score)' "$RUNLOG" 2>/dev/null; then
    cp "$DIR/base.c" "$PDIR/wins/$FN.c"
    echo "$FN $SRC $NAME (base)" >> "$PDIR/wins/MANIFEST.txt"
    WON="$NAME"
    BEST=0; BEST_CFG="$NAME"
    echo "  [$NAME] score 0 — WIN (base already matches; wins/$FN.c via $NAME)"
    break
  fi

  # otherwise record this config's best (lowest) score for the floor comparison.
  CBEST=$(grep -oE 'score = [0-9-]+' "$RUNLOG" 2>/dev/null | grep -oE '[0-9-]+$' | sort -n | head -1)
  CBEST=${CBEST:-?}
  echo "  [$NAME] best score = $CBEST"
  if [ "$CBEST" != "?" ] && [ "$CBEST" -lt "$BEST" ] 2>/dev/null; then
    BEST="$CBEST"; BEST_CFG="$NAME"
  fi
done

if [ -n "$WON" ]; then
  echo "WIN $FN $WON"
else
  echo "NOWIN $FN $BEST $BEST_CFG"
fi
