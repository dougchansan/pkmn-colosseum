#!/bin/bash
# auto_gate.sh — overnight autonomous gating. For every pending band-win tag:
# dry-run through the hardened band_integrate plug; if anything HELD, apply,
# reject asm/#if-flip fraud, skip redundant no-ops, and commit the real wins
# scoped (NEVER the 3 pre-existing WIP files). Prints a per-tag verdict + a total.
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
PY=python3
BI=tools/decomp_work/band_integrate.py
TO=tools/decomp_work/timeout_run.py
GATE_TIMEOUT_SECS=${GATE_TIMEOUT_SECS:-1800}
# the 3 pre-existing WIP files that are NOT this session's work — never commit them
WIP_RE='src/game/fsys/fsys_file.c|src/game/gs_pokemon_summary.c|src/hsd/hsd_dobj.c'
total=0
MARK=build/.last_gate
NEXT_MARK=build/.last_gate.next
PRE_DIRTY_SRC=$(git status --short -- src/ 2>/dev/null | awk '{print $2}')
# Only consider tags banked/updated since the last pass (first pass: last 3h),
# so we don't re-dry-run dozens of already-gated historical tags every cycle.
touch "$NEXT_MARK"
if [ -f "$MARK" ]; then FILES=$(find build/band_wins -name 'pl_*.json' -newer "$MARK" 2>/dev/null)
else FILES=$(find build/band_wins -name 'pl_*.json' -mmin -180 2>/dev/null); fi
if [ -n "$FILES" ]; then
  # The lane bundle files are long-lived ledgers. Prune already-committed exact
  # wins and cross-lane duplicate fn/source entries before the expensive parent
  # integration pass, otherwise a single new save re-gates stale historical rows.
  $PY tools/decomp_work/prune_band_wins.py --write --quiet $FILES >/dev/null 2>&1 || true
fi
# LOCK-AWARE GATING (the driver's documented intent, now actually implemented).
# Never commit a file a lane is still band-locked on: that gate-vs-lane race is what
# produced the repeated byte-identical "+N byte-exact" churn commits — the gate kept
# re-applying a lane's in-flight file every pass. Held file-locks are read once per pass;
# a tag whose changed file is locked is reverted + skipped, leaving the win banked to
# gate cleanly the first cycle the lock is released. timeout-guarded: if locks.py is slow
# the list is empty and we fall back to the old (lock-blind) behaviour rather than stall.
LOCKED_FILES=$($PY "$TO" 20 -- $PY tools/decomp_work/coordination/locks.py list --scope file 2>/dev/null | awk '{print $2}')
for f in $FILES; do
  [ -f "$f" ] || continue
  tag=$(basename "$f" .json)
  nwins=$($PY -c "import json;print(sum(1 for k in json.load(open('$f')) if k not in {'_src','_srcs','_pct'}))" 2>/dev/null || echo 0)
  [ "${nwins:-0}" -ge 1 ] || continue
  tag_srcs=$($PY - "$f" <<'PY' 2>/dev/null
import json, sys
d = json.load(open(sys.argv[1]))
srcs = set(d.get("_srcs", {}).values())
if d.get("_src"):
    srcs.add(d["_src"])
for src in sorted(s for s in srcs if s):
    print(src)
PY
)
  pre_dirty=""
  for src in $tag_srcs; do printf '%s\n' "$PRE_DIRTY_SRC" | grep -qxF "$src" && pre_dirty="$pre_dirty $src"; done
  if [ -n "$pre_dirty" ]; then
    echo "  $tag: DIRTY ($pre_dirty) -> skip, will gate after parent reviews dirty src"
    touch "$f"
    continue
  fi
  out=$($PY "$TO" "$GATE_TIMEOUT_SECS" -- $PY "$BI" "$tag" 2>&1)
  rc=$?
  if [ "$rc" = 124 ]; then
    echo "  $tag: TIMEOUT dry-run gate after ${GATE_TIMEOUT_SECS}s"
    touch "$f"
    continue
  fi
  echo "$out" | grep -q "HELD" || { echo "  $tag: no-hold (walls/abort)"; continue; }
  $PY "$TO" "$GATE_TIMEOUT_SECS" -- $PY "$BI" "$tag" --apply >/dev/null 2>&1
  rc=$?
  if [ "$rc" = 124 ]; then
    echo "  $tag: TIMEOUT apply gate after ${GATE_TIMEOUT_SECS}s"
    touch "$f"
    continue
  fi
  # changed src files from the accumulated applies, minus the WIP trio
  changed_all=$(git diff --name-only -- src/ 2>/dev/null | grep -vE "$WIP_RE")
  changed=""
  for src in $changed_all; do
    printf '%s\n' "$PRE_DIRTY_SRC" | grep -qxF "$src" || changed="$changed $src"
  done
  [ -n "$changed" ] || { echo "  $tag: redundant no-op (already in canon)"; continue; }
  # anti-churn: if a lane still band-locks any changed file, don't race it — revert the
  # speculative apply for the locked file(s) and skip; the win stays banked for a later pass.
  raced=""
  for src in $changed; do printf '%s\n' "$LOCKED_FILES" | grep -qxF "$src" && raced="$raced $src"; done
  if [ -n "$raced" ]; then
    echo "  $tag: LOCKED ($raced) -> revert+skip, will gate when lane releases"
    for src in $raced; do git checkout -- "$src" 2>/dev/null; done
    touch "$f"
    continue
  fi
  # fraud guard: reject if any added line is asm storage / inline asm / .inc include
  fraud=0
  for src in $changed; do
    if git diff -- "$src" | grep -qiE "^\+.*(asm void |asm[[:space:]]*\{|__asm|#include[[:space:]]+\"[^\"]*\.inc\")"; then
      echo "  $tag: FRAUD in $src -> reverting"; git checkout -- "$src"; fraud=1
    fi
  done
  [ "$fraud" = 1 ] && continue
  changed_all=$(git diff --name-only -- src/ 2>/dev/null | grep -vE "$WIP_RE")
  changed=""
  for src in $changed_all; do
    printf '%s\n' "$PRE_DIRTY_SRC" | grep -qxF "$src" || changed="$changed $src"
  done
  [ -n "$changed" ] || continue
  held=$(echo "$out" | grep -c "HELD")
  committed=0
  for attempt in 1 2 3 4 5 6 7 8; do
    git add $changed build/matched_fns.txt tools/decomp_work/equivalent.txt 2>/dev/null
    if git commit -q -m "decomp: gate $tag (+$held byte-exact) [auto-overnight]

Co-Authored-By: Claude Opus 4.8 (1M context) <noreply@anthropic.com>"; then
      echo "  $tag: COMMITTED +$held"; total=$((total+held)); committed=1
      # The HELD fns are now byte-exact in canon. Mark them attempted AND bump them to
      # 100% in report.json so the ledger/bucket queues stop re-offering finished work
      # (instead of waiting hours for the dashboard's periodic report regen).
      heldfns=$(echo "$out" | grep -oE "HELD[[:space:]]+fn_[0-9A-Fa-f]+" | awk '{print $2}' | tr '\n' ' ')
      for cfn in $heldfns; do $PY tools/decomp_work/wall_ledger.py mark "$cfn" committed >/dev/null 2>&1; done
      [ -n "$heldfns" ] && $PY tools/decomp_work/update_report.py $heldfns >/dev/null 2>&1
      break
    fi
    sleep 4   # transient .git/objects permission race (AV scan) — back off and retry
  done
  [ "$committed" = 0 ] && echo "  $tag: COMMIT-FAILED after retries (win applied to canon, staged, NOT committed — manual commit needed)"
done
mv "$NEXT_MARK" "$MARK"
echo "=== auto_gate: +$total byte-exact committed this pass ==="
