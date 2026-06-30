#!/usr/bin/env bash
# oracle_diff.sh - behavioral-equivalence oracle driver (reusable).
#
# Builds and runs a differential between:
#   A-side: the ORIGINAL function's machine code, statically recompiled by
#           DolRecomp from orig/GC6E01/start.dol (flat big-endian guest RAM,
#           DOL code+data preloaded so jumptables/rodata/SDA2 resolve).
#   B-side: OUR decompiled C, extracted VERBATIM from a src/ .c file
#           (#if 0 asm stubs are skipped; the active #else C is used).
#
# Each target has a per-function harness (harness_<fn>.c) holding the memory
# model, input sweep and comparison logic. This script wires verbatim src
# extraction + the DolRecomp generated chunk(s) + the core runtime + the DOL
# loader into one executable and runs it.
#
# Usage:
#   oracle_diff.sh <harness.c> <src.c> <fn1> [fn2 ...]
#       <harness.c>  per-function harness (e.g. harness_fightSideGetStatus.c)
#       <src.c>      source file to extract the B-side from (e.g. .../pokemon.c)
#       <fnN>        function(s) to extract VERBATIM into bside_real_src.inc
#                    (list the target + any same-file callees it needs; omit
#                    callees the harness provides itself via a host shim)
#
# The harness names the DolRecomp chunk symbol(s) it needs (func_<chunkbase>);
# this script discovers and compiles the chunk file(s) that own every label_
# address the harness references via a marker comment:
#       // ORACLE_CHUNK 0x801F76B8   (one per needed entry address)
# Falls back to scanning the harness for hex addresses if no markers present.
#
# Env (defaults target the 3090 box /tmp layout):
#   DOLRECOMP_SRC  - DolRecomp src/ (core/cpu.{c,h}, frontend/dol.{c,h})
#   GEN_DIR        - dolrecomp generated/ output (chunks + header)
#   SRC_FILE_HOST  - if set, copy of src.c already on this host (else use $2)
#   ORACLE_DOL     - path to original start.dol (default /tmp/start.dol)
#   WORK           - scratch build dir (default /tmp/oracle)
set -euo pipefail

HARNESS="${1:?harness .c}"; SRC="${2:?src .c}"; shift 2 || true
FNS=("$@")
[ ${#FNS[@]} -ge 1 ] || { echo "need >=1 fn to extract"; exit 2; }

DOLRECOMP_SRC="${DOLRECOMP_SRC:-/tmp/DolRecomp/src}"
GEN_DIR="${GEN_DIR:-/tmp/colo_out/generated}"
ORACLE_DOL="${ORACLE_DOL:-/tmp/start.dol}"
WORK="${WORK:-/tmp/oracle}"
HERE="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$WORK"

# 1) Extract verbatim B-side src -> .inc (active #else C; dead asm skipped)
INC="$WORK/bside_real_src.inc"
python3 "$HERE/extract_src_fns.py" "$SRC" "$INC" "${FNS[@]}"

# 2) Discover the generated chunk file(s) the harness needs.
#    Prefer explicit "// ORACLE_CHUNK 0xADDR" markers; else scan func_ symbols.
CHUNK_ARGS=()
mapfile -t ADDRS < <(grep -oE 'ORACLE_CHUNK 0x[0-9A-Fa-f]+' "$HARNESS" | awk '{print $2}')
if [ ${#ADDRS[@]} -eq 0 ]; then
    # fallback: any func_XXXXXXXX( referenced in the harness
    mapfile -t FUNCS < <(grep -oE 'func_[0-9A-Fa-f]{8}' "$HARNESS" | sort -u)
    for f in "${FUNCS[@]}"; do
        addr="0x${f#func_}"
        ADDRS+=("$addr")
    done
fi
declare -A SEEN
for addr in "${ADDRS[@]}"; do
    lbl="label_$(printf '%08X' "$addr")"
    chunk="$(grep -l "func_$(printf '%08X' "$addr")(CPUState" "$GEN_DIR"/chunks/*.c 2>/dev/null | head -1)"
    [ -z "$chunk" ] && chunk="$(grep -l "${lbl}:" "$GEN_DIR"/chunks/*.c | head -1)"
    [ -z "$chunk" ] && { echo "ERROR: no chunk owns $addr"; exit 1; }
    if [ -z "${SEEN[$chunk]:-}" ]; then SEEN[$chunk]=1; CHUNK_ARGS+=("$chunk"); fi
done
echo "chunks: ${CHUNK_ARGS[*]}"

# 3) Compile harness + chunk(s) + core runtime + DOL loader
OUT="$WORK/oracle_$(basename "$HARNESS" .c)"
# NOTE: -I"$WORK" MUST precede -I"$HERE" so the freshly-generated
# bside_real_src.inc in $WORK wins over any stale copy beside the harness.
cc -O1 -Wno-int-conversion -I"$DOLRECOMP_SRC" -I"$WORK" -I"$HERE" \
   -o "$OUT" \
   "$HARNESS" "${CHUNK_ARGS[@]}" \
   "$DOLRECOMP_SRC/core/cpu.c" "$DOLRECOMP_SRC/frontend/dol.c" -lm

# 4) Run
echo "== running $OUT (DOL=$ORACLE_DOL) =="
ORACLE_DOL="$ORACLE_DOL" "$OUT" "$ORACLE_DOL"
