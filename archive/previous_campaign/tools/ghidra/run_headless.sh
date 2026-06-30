#!/usr/bin/env bash
# Import main.dol sections into a Ghidra project and auto-analyze.
#
# Prerequisites:
#   - split_dol.py has been run (tools/ghidra/dol_split/*.bin + manifest.json exist)
#   - Ghidra 12.0.4 installed at GHIDRA_DIR
#   - Java 21 on PATH
#
# Usage:
#   tools/ghidra/run_headless.sh
#
# Outputs:
#   tools/ghidra/project/colosseum.gpr (+ .rep directory)

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd -W 2>/dev/null || pwd)"
GHIDRA_DIR="${GHIDRA_DIR:-C:/Users/douglaswhittingham/gamecube-decomp/ghidra_12.0.4_PUBLIC}"
ANALYZE="$GHIDRA_DIR/support/analyzeHeadless.bat"
PROJECT_DIR="$REPO_ROOT/tools/ghidra/project"
PROJECT_NAME="colosseum"
SPLIT_DIR="$REPO_ROOT/tools/ghidra/dol_split"
SCRIPT_DIR="$REPO_ROOT/tools/ghidra/scripts"
MANIFEST="$SPLIT_DIR/manifest.json"

if [ ! -f "$MANIFEST" ]; then
  echo "Manifest not found: $MANIFEST"
  echo "Run: python3 tools/ghidra/split_dol.py <path/to/main.dol> $SPLIT_DIR"
  exit 1
fi

mkdir -p "$PROJECT_DIR"

# Emit sections as newline-separated "name base file" triples to a tmpfile.
TMPLIST="$(mktemp)"
python3 -c "
import json
m = json.load(open(r'$MANIFEST'))
for s in m['sections']:
    print(s['name'], hex(s['base_addr']), s['file'])
" > "$TMPLIST"

COUNT=$(wc -l < "$TMPLIST")
IDX=0
while read -r NAME BASE FILE; do
  IDX=$((IDX+1))
  FLAGS=(-noanalysis)
  if [ "$IDX" -eq "$COUNT" ]; then
    FLAGS=()  # analyze on the final import
  fi

  echo "=== Importing $NAME @ $BASE (${IDX}/${COUNT}) ==="
  "$ANALYZE" "$PROJECT_DIR" "$PROJECT_NAME" \
    -import "$SPLIT_DIR/$FILE" \
    -loader BinaryLoader \
    -loader-baseAddr "$BASE" \
    -processor "PowerPC:BE:32:default" \
    -overwrite \
    -analysisTimeoutPerFile 1800 \
    -scriptPath "$SCRIPT_DIR" \
    "${FLAGS[@]}" < /dev/null
done < "$TMPLIST"
rm -f "$TMPLIST"

echo "=== Import + analysis complete ==="
echo "Project: $PROJECT_DIR/$PROJECT_NAME.gpr"
