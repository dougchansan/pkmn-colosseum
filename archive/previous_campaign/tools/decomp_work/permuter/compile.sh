#!/bin/bash
# compile.sh — decomp-permuter compile wrapper for Pokemon Colosseum (GPXE01).
#
# decomp-permuter invokes: ./compile.sh <C_FILE> -o <OUT_O>
#   $1 = input .c    $2 = "-o"    $3 = output .o
#
# Runs the vendored MetroWerks compiler mwcceppc.exe (a Windows PE) NATIVELY in
# Linux via `wibo` (https://github.com/decompals/wibo) — NOT through WSL<->Windows
# interop. All paths stay in Linux form (no wslpath). This keeps the entire
# permuter loop inside the WSL VM: no per-compile Windows process launch and no
# 9p round-trips, which is dramatically cheaper on the Windows host (the old
# interop path is what pinned `wsl`/`vmmemwsl` at hundreds of percent).
#
# Requires: wibo on PATH or at ~/.local/bin/wibo, and powerpc-linux-gnu-objdump
# (the scorer's disassembler) available natively under WSL.
set -euo pipefail

# Resolve the repo root from this script's own location. build_dir.sh copies
# this wrapper to tools/decomp_work/permuter/dirs/<fn>/compile.sh, so the repo
# root is five directories up. Deriving it (instead of hardcoding /mnt/c) means
# the tree can live anywhere — /mnt/c or native ext4 — with no edit here.
SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SELF/../../../../.." && pwd)"
# Prefer a compiler copy on native ext4 — avoids re-reading the multi-MB PE
# (and its lmgr326b.dll license manager) over the 9p /mnt/c mount on every
# single compile (~2x faster per compile, and keeps that I/O off the Windows
# host). Falls back to the in-repo copy if the ext4 cache is absent.
MWCC_CACHE="$HOME/.cache/decomp-mwcc/GC/1.3/mwcceppc.exe"
if [ -f "$MWCC_CACHE" ]; then MWCC="$MWCC_CACHE"; else MWCC="$REPO/tools/mwcc_compiler/GC/1.3/mwcceppc.exe"; fi
INCLUDE="$REPO/include"

# Locate wibo (the Win32 PE loader that runs mwcceppc.exe under Linux).
WIBO="${WIBO:-}"
if [ -z "$WIBO" ]; then
  if command -v wibo >/dev/null 2>&1; then WIBO="$(command -v wibo)"
  elif [ -x "$HOME/.local/bin/wibo" ]; then WIBO="$HOME/.local/bin/wibo"
  else echo "compile.sh: wibo not found on PATH or at ~/.local/bin/wibo" >&2; exit 127; fi
fi

IN_C="$1"
OUT_O="$3"

# Project's exact compile flags (must match compile_config.json default for
# gs_field_world.c, which uses GC/1.3).
FLAGS=(-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off \
       -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8)

# -c compile only; emit the object directly. wibo feeds the PE Linux paths.
exec "$WIBO" "$MWCC" "${FLAGS[@]}" -i "$INCLUDE" -c -o "$OUT_O" "$IN_C"
