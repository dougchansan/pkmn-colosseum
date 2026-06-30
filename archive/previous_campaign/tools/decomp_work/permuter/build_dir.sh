#!/bin/bash
# build_dir.sh — build a decomp-permuter working directory for one function of
# Pokemon Colosseum's gs_field_world.c (extensible to any TU).
#
# usage: ./build_dir.sh <fn_name> <src/game/file.c>
#
# Produces tools/decomp_work/permuter/dirs/<fn_name>/ containing:
#   base.c        — preprocessed whole-TU source, reduced to just <fn_name>
#   target.o      — single-function object assembled from the *_fn_<addr>.inc
#   compile.sh    — symlink/copy of the shared compile wrapper
#   settings.toml — compiler_type=mwcc, func_name=<fn_name>, objdump_command
set -euo pipefail

# Derive the repo root from this script's location (tools/decomp_work/permuter/
# build_dir.sh -> three dirs up). No hardcoded /mnt/c, so the tree can live on
# native ext4 or the Windows mount without editing this file.
SELF="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$SELF/../../.." && pwd)"
PERM="$REPO/tools/decomp_work/refs/decomp-permuter"
PDIR="$REPO/tools/decomp_work/permuter"
# Prefer the ext4 compiler cache (see compile.sh) to avoid 9p reads; fall back
# to the in-repo copy.
MWCC_CACHE="$HOME/.cache/decomp-mwcc/GC/1.3/mwcceppc.exe"
if [ -f "$MWCC_CACHE" ]; then MWCC="$MWCC_CACHE"; else MWCC="$REPO/tools/mwcc_compiler/GC/1.3/mwcceppc.exe"; fi
INCLUDE="$REPO/include"

# wibo runs mwcceppc.exe natively under Linux (no WSL<->Windows interop).
WIBO="${WIBO:-}"
if [ -z "$WIBO" ]; then
  if command -v wibo >/dev/null 2>&1; then WIBO="$(command -v wibo)"
  elif [ -x "$HOME/.local/bin/wibo" ]; then WIBO="$HOME/.local/bin/wibo"
  else echo "build_dir.sh: wibo not found on PATH or at ~/.local/bin/wibo" >&2; exit 127; fi
fi

FN="$1"
SRC="$REPO/$2"              # e.g. src/game/gs_field_world.c
STEM="$(basename "$SRC" .c)"
INC="$(dirname "$SRC")/${STEM}_${FN/fn_/fn_}.inc"   # src/game/<stem>_<fn>.inc

OUTDIR="$PDIR/dirs/$FN"
mkdir -p "$OUTDIR"

# --- 1. base.c: preprocess the whole TU, then isolate the one function -------
# Pipeline (all output written ONLY under the permuter dir, never the tree):
#   a. select_c_branch.py: if the TARGET is stored as an active `#if 1` asm
#      wrapper, flip that guard to `#if 0` in a TEMP copy so the preprocessor
#      keeps the `#else` C decomp candidate (the thing we actually permute).
#   b. mwcceppc -E on that temp copy.
#   c. strip `/* #line */` markers + strip_other_fns.py (isolate the target).
#   d. scrub_asm_bodies.py: reduce EVERY function whose body still contains raw
#      PPC asm (sibling asm wrappers, or the target itself if it has no C
#      candidate / is an inline __asm{} block) to a clean forward declaration,
#      so pycparser (C99) can always parse base.c. The target keeps its C body.
PP="$OUTDIR/_pp.c"
TMPSRC="$OUTDIR/_src.c"

# a. select the target's C branch (no-op if it is already on the C branch).
python3 "$PDIR/select_c_branch.py" "$SRC" "$FN" > "$TMPSRC"

# b. preprocess the temp copy via wibo (native, no interop, Linux paths).
#    cd "$REPO" so relative "src/game/..._fn_*.inc" #includes still resolve
#    (only referenced by the asm wrappers we have de-selected, but keep valid).
( cd "$REPO" && "$WIBO" "$MWCC" -O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off \
    -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8 \
    -i "$INCLUDE" -E -o "$PP" "$TMPSRC" ) >/dev/null 2>&1 || true

# c. Drop mwcc's "/* #line ... */" markers (confuse strip_other_fns / re-parse)
sed -i '/^\/\* #line /d' "$PP"

# strip_other_fns rewrites the file in place when given a file arg:
python3 "$PERM/strip_other_fns.py" "$PP" "$FN"

# d. Reduce any remaining asm-bodied function (incl. the target if it has no C
#    candidate) to a forward declaration so base.c is guaranteed to parse.
python3 "$PDIR/scrub_asm_bodies.py" "$PP" "$FN"

# Belt-and-suspenders: drop any leading `asm ` qualifier the scrubber may have
# left on a forward declaration so pycparser (C99) accepts it.
sed -i -E 's/^[[:space:]]*asm[[:space:]]+/    /' "$PP"

cp "$PP" "$OUTDIR/base.c"

# --- 2. target.o: assemble the .inc truth file -------------------------------
"$PDIR/inc_to_target.sh" "$FN" "$INC" "$OUTDIR/target.o"

# --- 3. compile.sh -----------------------------------------------------------
cp "$PDIR/compile.sh" "$OUTDIR/compile.sh"
chmod +x "$OUTDIR/compile.sh"

# --- 4. settings.toml --------------------------------------------------------
# The [weight_overrides] block boosts the mutation passes that target CodeWarrior's
# register-allocation / instruction-scheduling / stack-frame walls (the dominant
# near-miss class for this codebase). Each key MERGES on top of the mwcc default
# weights (src/main.py: merge_randomization_weights). perm_temp_for_expr already
# defaults to 100, so it is left untouched. perm_dummy_comma_expr is boosted hard
# because the (0, expr) comma idiom produced our only confirmed score-0 win
# (fn_8011B67C). perm_pad_var_decl shifts the stack frame for CW frame-size match.
cat > "$OUTDIR/settings.toml" <<EOF
func_name = "$FN"
compiler_type = "mwcc"
objdump_command = "powerpc-linux-gnu-objdump -dr -EB -mpowerpc -M broadway"

[weight_overrides]
perm_reorder_decls = 40
perm_reorder_stmts = 40
perm_dummy_comma_expr = 30
perm_commutative = 20
perm_pad_var_decl = 10
perm_split_assignment = 20
perm_duplicate_assignment = 15
perm_add_self_assignment = 15
perm_refer_to_var = 15
EOF

echo "Built $OUTDIR"
ls -la "$OUTDIR"
