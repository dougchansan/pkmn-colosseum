#!/usr/bin/env bash
# After run_headless.sh has imported all sections, this script:
#   1. Seeds known fn_XXXXXXXX symbols from src/game/gs_title.c
#   2. Exports Ghidra-decompiled C for each address in function_list.txt
#
# Output: tools/decomp_work/ghidra_out/fn_XXXXXXXX_ghidra.c

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd -W 2>/dev/null || pwd)"
GHIDRA_DIR="${GHIDRA_DIR:-C:/Users/douglaswhittingham/gamecube-decomp/ghidra_12.0.4_PUBLIC}"
ANALYZE="$GHIDRA_DIR/support/analyzeHeadless.bat"
PROJECT_DIR="$REPO_ROOT/tools/ghidra/project"
PROJECT_NAME="colosseum"
SCRIPT_DIR="$REPO_ROOT/tools/ghidra/scripts"
GS_TITLE_C="$REPO_ROOT/src/game/gs_title.c"
FN_LIST="$REPO_ROOT/tools/ghidra/scripts/function_list.txt"
OUT_DIR="$REPO_ROOT/tools/decomp_work/ghidra_out"

mkdir -p "$OUT_DIR"

# Run on the text1 section (contains all game code)
# First analyze (auto-analysis wasn't run during import), then seed+export
echo "=== Analyzing text1 + seeding symbols + exporting decomp ==="
"$ANALYZE" "$PROJECT_DIR" "$PROJECT_NAME" \
  -process "text1.bin" \
  -scriptPath "$SCRIPT_DIR" \
  -preScript SeedSymbols.java "$GS_TITLE_C" \
  -postScript ExportDecomp.java "$FN_LIST" "$OUT_DIR" \
  -analysisTimeoutPerFile 1800 < /dev/null

echo "=== Export complete. Output: $OUT_DIR ==="
ls "$OUT_DIR" | head -10
