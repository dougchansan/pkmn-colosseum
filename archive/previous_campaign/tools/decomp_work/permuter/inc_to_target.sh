#!/bin/bash
# inc_to_target.sh — assemble a project *_fn_<ADDR>.inc truth file into a
# single-function target.o for decomp-permuter.
#
# usage: ./inc_to_target.sh <fn_name> <path/to/file.inc> <out_target.o>
#
# The .inc files are MetroWerks-flavoured PPC asm using:
#   - a leading `nofralloc` directive (CW inline-asm only; drop for GNU as)
#   - local labels written as `@L_xxxx:` and referenced as `@L_xxxx`
#   - external calls `bl fn_xxxx` (left as relocations against undefined syms)
# GNU `as` wants `.L`-style locals, so we rewrite `@L_` -> `.L_`.
set -euo pipefail

FN="$1"
INC="$2"
OUT="$3"

AS=powerpc-linux-gnu-as
ASFLAGS="-mbroadway -mregnames -be"

WORK="$(mktemp -d)"
SFILE="$WORK/$FN.s"

{
  echo "    .section .text"
  echo "    .globl $FN"
  echo "    .type $FN, @function"
  echo "$FN:"
  # Drop `nofralloc`, rewrite @L_ local labels to GNU-friendly .L_ form.
  sed -e '/^[[:space:]]*nofralloc[[:space:]]*$/d' \
      -e 's/@L_/.L_/g' \
      "$INC"
} > "$SFILE"

$AS $ASFLAGS -o "$OUT" "$SFILE"
rm -rf "$WORK"
