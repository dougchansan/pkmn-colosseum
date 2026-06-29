#!/usr/bin/env bash
# Byte-match DOL build for Pokémon Colosseum (GC6E01).
# Reproduces orig/GC6E01/sys/main.dol byte-for-byte from the dtk split.
#
# Pipeline: dtk dol split -> verify ldscript -> mwldeppc link -> dtk elf2dol -> sha1 verify.
#
# Notes captured from the pilot (do not lose these):
#   * symbols.build.txt is the build-only sanitized symbols file (trailing "// Proposed:"
#     comments stripped; 232 lines). The canonical config/GC6E01/symbols.txt is the truth
#     file and is NEVER edited.
#   * config.build.yml points at symbols.build.txt and the tracked ldscript template.
#   * dtk's default linker script uses a 0x2000 debug-stack gap, but the original game
#     uses _db_stack_addr = _stack_addr + 0x8000. config/GC6E01/ldscript.tpl records
#     that one game-specific linker setting without post-split patching.
#   * Original layout (verified byte-exact): stack 0x10000, debug-stack gap 0x8000.
set -euo pipefail
cd "$(dirname "$0")/../.."   # repo root

CFG=config/GC6E01/config.build.yml
OUT=build/dol
ORIG=orig/GC6E01/sys/main.dol
LD=tools/decomp_work/ra/mwldeppc.exe

echo "[1/5] dtk dol split ($CFG -> $OUT)"
rm -rf "$OUT"
tools/dtk.exe dol split --no-update "$CFG" "$OUT" >/dev/null 2>&1

echo "[2/5] verify ldscript debug-stack gap is 0x8000"
grep -q 'stack_addr + 0x8000' "$OUT/ldscript.lcf" || { echo "FATAL: ldscript template did not set 0x8000"; exit 1; }

echo "[3/5] link asm objects -> main.elf"
OBJS=$(find "$OUT/obj" -name '*.o' | sort | tr '\n' ' ')
PATH="tools/decomp_work/ra:$PATH" "$LD" -o "$OUT/main.elf" -lcf "$OUT/ldscript.lcf" $OBJS 2>&1 \
  | grep -ivE "Warning|MWLibrar|Floating|does not match|settings|^#|^$" || true

echo "[4/5] elf2dol -> main.dol"
tools/dtk.exe elf2dol "$OUT/main.elf" "$OUT/main.dol" >/dev/null 2>&1

echo "[5/5] verify sha1 vs orig"
A=$(sha1sum "$ORIG" | cut -d' ' -f1)
B=$(sha1sum "$OUT/main.dol" | cut -d' ' -f1)
echo "  orig:  $A"
echo "  built: $B"
if [ "$A" = "$B" ]; then
  echo "  ==> OK: byte-identical main.dol"
else
  echo "  ==> MISMATCH: $(cmp -l "$ORIG" "$OUT/main.dol" | wc -l) differing bytes"
  exit 1
fi
