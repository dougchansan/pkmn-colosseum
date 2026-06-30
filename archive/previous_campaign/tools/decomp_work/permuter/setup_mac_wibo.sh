#!/usr/bin/env bash
# setup_mac_wibo.sh — provision an Apple-Silicon Mac to run MetroWerks mwcceppc
# (a 32-bit Windows PE) via wibo-macos under Rosetta 2, then PROVE the output is
# byte-identical to a native x86 build before trusting any match.
#
# Why this is the right shape: mwcceppc cannot be natively ported (closed-source,
# and reproducing its exact codegen *is* the decomp-matching problem). wibo is a PE
# *loader*, not an emulator — Rosetta 2 translates the x86_64 wibo, which then runs
# the i386 mwcceppc. Verified: 1.3 and 1.2.5n produce byte-identical .o vs native.
#
# The Mac's role is the arm64 ORCHESTRATOR (objdiff/m2c/scoring/editing); the high-
# throughput permuter farm stays on the native-x86 3090 box. Use the Mac wrapper for
# occasional / off-hours compiles only.
#
# Run from the repo root on a machine that has SSH access to the Mac:
#   MAC=douglaswhittingham@douglass-macbook-pro bash tools/decomp_work/permuter/setup_mac_wibo.sh
set -uo pipefail
MAC="${MAC:?set MAC=user@host}"
SSH=(ssh -o BatchMode=yes -i "$HOME/.ssh/id_ed25519" "$MAC")
WIBO_VER="1.1.0"
REPO_ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
FLAGS="-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8 -c"

echo "== 1. wibo-macos (loader) =="
"${SSH[@]}" "mkdir -p ~/.local/bin && curl -fsSL -o ~/.local/bin/wibo-macos \
  https://github.com/decompals/wibo/releases/download/${WIBO_VER}/wibo-macos && \
  chmod +x ~/.local/bin/wibo-macos && xattr -c ~/.local/bin/wibo-macos 2>/dev/null; \
  ~/.local/bin/wibo-macos 2>&1 | head -1"

echo "== 2. compiler cache (mirror box convention: ~/.cache/decomp-mwcc) =="
tar -czf - -C "$REPO_ROOT/tools/mwcc_compiler" GC | \
  "${SSH[@]}" 'mkdir -p ~/.cache/decomp-mwcc && tar -xzf - -C ~/.cache/decomp-mwcc && \
    echo "  versions: $(ls ~/.cache/decomp-mwcc/GC | tr "\n" " ")"'

echo "== 3. PATH wrapper mwcc-cw (version via MWCC_VER, default 1.3) =="
"${SSH[@]}" 'cat > ~/.local/bin/mwcc-cw <<"EOF"
#!/bin/bash
exec "$HOME/.local/bin/wibo-macos" "$HOME/.cache/decomp-mwcc/GC/${MWCC_VER:-1.3}/mwcceppc.exe" "$@"
EOF
chmod +x ~/.local/bin/mwcc-cw
grep -q ".local/bin" ~/.zshrc 2>/dev/null || echo "export PATH=\"\$HOME/.local/bin:\$PATH\"" >> ~/.zshrc'

echo "== 4. BYTE-IDENTICAL GATE (mandatory — never trust an unverified Mac compile) =="
printf 'typedef unsigned int u32; typedef int s32; typedef float f32;\n'\
'typedef struct { f32 x, y, z; } Vec;\n'\
'f32 dot(Vec* a, Vec* b) { return a->x*b->x + a->y*b->y + a->z*b->z; }\n'\
's32 sum(s32* arr, u32 n) { u32 i; s32 s = 0; for (i = 0; i < n; i++) { if (arr[i] > 0) s += arr[i]; } return s; }\n' \
  > /tmp/_macgate.c
fail=0
for V in 1.3 1.2.5n; do
  # native x86 reference (mwcceppc.exe runs natively on Windows; on Linux use wibo)
  rm -rf /tmp/_gate && mkdir -p /tmp/_gate && cp /tmp/_macgate.c /tmp/_gate/test.c && ( cd /tmp/_gate
    MWCC="$REPO_ROOT/tools/mwcc_compiler/GC/$V/mwcceppc.exe"
    if command -v wibo >/dev/null 2>&1; then wibo "$MWCC" $FLAGS -o ref.o test.c >/dev/null 2>&1
    else "$MWCC" $FLAGS -o ref.o test.c >/dev/null 2>&1; fi )
  REF=$(md5sum /tmp/_gate/ref.o 2>/dev/null | cut -d' ' -f1)
  # Mac via wibo-macos (same filename -> byte-identical incl. metadata)
  "${SSH[@]}" "mkdir -p ~/_gate && cd ~/_gate" >/dev/null 2>&1
  scp -o BatchMode=yes -i "$HOME/.ssh/id_ed25519" /tmp/_macgate.c "$MAC:_gate_test_$V.c" >/dev/null 2>&1
  MAC_MD5=$("${SSH[@]}" "cd ~ && cp _gate_test_$V.c test.c && MWCC_VER=$V ~/.local/bin/mwcc-cw $FLAGS -o w_$V.o test.c 2>/dev/null && md5 -q w_$V.o")
  if [ -n "$REF" ] && [ "$REF" = "$MAC_MD5" ]; then echo "  $V: BYTE-IDENTICAL  $REF ✓"
  else echo "  $V: MISMATCH  native=$REF mac=$MAC_MD5 ✗"; fail=1; fi
done
[ "$fail" = 0 ] && echo "== GATE PASSED — Mac mwcceppc is trustworthy ==" || { echo "== GATE FAILED — do NOT trust Mac matches =="; exit 1; }
