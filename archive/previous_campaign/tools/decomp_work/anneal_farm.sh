#!/usr/bin/env bash
# anneal_farm.sh — path-agnostic parallel annealer swarm. Reads a near-miss queue
# (build/anneal_targets.txt, lines "src/file.c fn"), and runs permute_match.py over
# it, partitioned BY FILE so parallel jobs never edit the same source file (the one
# collision permute_match can't tolerate). Runs anywhere the toolchain works
# (compile_check + mwcc + wibo) — Mac, the 3090, or Windows/WSL — no hardcoded paths.
#
#   ANNEAL_JOBS  (default: nproc/2)   how many FILES to anneal at once
#   ANNEAL_ITERS (default: 1500)      anneal budget per function
#   ANNEAL_QUEUE (default: build/anneal_targets.txt)
#   WINS -> tools/decomp_work/permuter/wins/<fn>.c ; log -> build/anneal_farm.log
set -uo pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
JOBS="${ANNEAL_JOBS:-$(( $( (nproc 2>/dev/null || sysctl -n hw.ncpu) ) / 2 ))}"
ITERS="${ANNEAL_ITERS:-1500}"
QUEUE="${ANNEAL_QUEUE:-build/anneal_targets.txt}"
PY="${ANNEAL_PY:-python3}"
LOG="build/anneal_farm.log"
mkdir -p tools/decomp_work/permuter/wins build
: > "$LOG"
[ -f "$QUEUE" ] || { echo "no queue at $QUEUE"; exit 1; }

# FUNCTION-level parallelism: each fn anneals on its OWN same-dir copy of the file
# (so the per-fn .inc includes still resolve and find_target_obj -> the monolithic
# target obj), so up to $JOBS functions anneal at once with no source collision.
nfn=$(grep -vE '^\s*#|^\s*$' "$QUEUE" | awk '{print NF-1}' | paste -sd+ - | bc 2>/dev/null || echo '?')
echo "[anneal_farm] $nfn functions, $JOBS in parallel, $ITERS iters/fn  ($(date +%H:%M:%S))" | tee -a "$LOG"

# expand "file fn1 fn2..." -> "file fn" lines, anneal each on a private copy
grep -vE '^\s*#|^\s*$' "$QUEUE" | awk '{for(i=2;i<=NF;i++) print $1, $i}' | \
  xargs -P "$JOBS" -L1 bash -c '
    file="$1"; fn="$2"; py="'"$PY"'"; iters="'"$ITERS"'"; log="'"$LOG"'"
    cp="${file%.c}__a_${fn}.c"
    cp -f "$file" "$cp" 2>/dev/null || exit 0
    out=$("$py" tools/decomp_work/permute_match.py "$cp" "$fn" --iters "$iters" 2>&1)
    rm -f "$cp" "${cp%.c}".o 2>/dev/null
    if printf "%s" "$out" | grep -qiE "WIN|score 0|\"exact\": *true|100\.0+%"; then
      echo "WIN  $fn  ($file)" | tee -a "$log"
      printf "%s" "$out" > "tools/decomp_work/permuter/wins/$fn.out"
    else
      echo "....  $fn  ($(printf "%s" "$out" | grep -oE "match=[0-9.]+%?|best[^0-9]*[0-9.]+" | tail -1))" >> "$log"
    fi
  ' _

echo "[anneal_farm] done $(date +%H:%M:%S) — wins: $(grep -c '^WIN' "$LOG")" | tee -a "$LOG"
