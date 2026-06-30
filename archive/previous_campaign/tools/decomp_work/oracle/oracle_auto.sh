#!/usr/bin/env bash
# oracle_auto.sh - AUTO oracle: no hand-written per-function harness.
#
#   oracle_auto.sh <fn_name> <hex_addr> <src.c> [extra_callee ...]
#
# Resolves ABI from src, extracts the verbatim src body (#if0-aware), and runs
# the generic auto_harness.c against the FULL-program image (libcolo.a) so deep
# call trees execute their real callees.
#
# Emits one line: PASS / MISMATCH / UNREACHABLE(Tier-2) / AUTOHARNESS-FAILED.
#
# Env defaults target the 3090 box /tmp layout:
#   DOLRECOMP_SRC, LIBCOLO (libcolo.a), GEN_DIR, ORACLE_DOL, WORK
set -uo pipefail

FN="${1:?fn name}"; ADDR="${2:?hex addr}"; SRC="${3:?src.c}"; shift 3 || true
CALLEES=("$@")

DOLRECOMP_SRC="${DOLRECOMP_SRC:-/tmp/DolRecomp/src}"
LIBCOLO="${LIBCOLO:-/tmp/colo_full/libcolo.a}"
GEN_DIR="${GEN_DIR:-/tmp/colo_out/generated}"
ORACLE_DOL="${ORACLE_DOL:-/tmp/start.dol}"
WORK="${WORK:-/tmp/oracle}"
HERE="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$WORK"

# chunk table + protos for the full driver (generated once; reuse if present)
[ -f "$WORK/oracle_chunk_table.inc" ] || cp /tmp/colo_full/chunk_table.inc "$WORK/oracle_chunk_table.inc"
[ -f "$WORK/oracle_chunk_protos.inc" ] || cp /tmp/colo_full/chunk_protos.inc "$WORK/oracle_chunk_protos.inc"

fail(){ echo "AUTOHARNESS-FAILED $FN @ $ADDR : $*"; exit 3; }

# 1) Find the active (#else) signature line for FN in SRC.
SIG="$(python3 - "$SRC" "$FN" <<'PY'
import sys,re
src,fn=sys.argv[1],sys.argv[2]
# strip #if 0..#else..#endif to active branch (reuse extractor logic inline)
import importlib.util, os
spec=importlib.util.spec_from_file_location("e", os.path.join(os.path.dirname(os.path.abspath(__file__)) if False else os.path.dirname(src) or '.', ""))
text=open(src,encoding='utf-8',errors='replace').read()
# minimal if0 strip
out=[]; lines=text.split('\n'); i=0
while i<len(lines):
    s=lines[i].strip().replace(' ','')
    if s=='#if0':
        d=1; j=i+1; el=None
        while j<len(lines) and d>0:
            t=lines[j].strip()
            if t.startswith('#if'): d+=1
            elif t.startswith('#endif'):
                d-=1
                if d==0: break
            elif t.startswith('#else') and d==1: el=j
            j+=1
        if el is not None: out.extend(lines[el+1:j])
        i=j+1
    else: out.append(lines[i]); i+=1
text='\n'.join(out)
m=re.search(r'(?m)^([A-Za-z_][\w\s\*]*?\b'+re.escape(fn)+r'\s*\([^;{]*\))\s*\{', text)
print(m.group(1).strip() if m else "")
PY
)"
[ -n "$SIG" ] || fail "no active C definition found for $FN in $SRC"
echo "  sig: $SIG" >&2

# 2) Resolve ABI
ABI_OUT="$WORK/abi.inc"
ABI_LINE="$(python3 "$HERE/resolve_abi.py" "$SIG" "$ABI_OUT")"
echo "  abi: $ABI_LINE" >&2
case "$ABI_LINE" in
  ABI_FAIL*) fail "ABI unresolved: ${ABI_LINE#ABI_FAIL }";;
esac

# 2b) VOID-return pre-flight. A void function has NO return value to compare;
# its only observable is side-effects (often writes to SDA globals, which this
# harness does not track — it only diffs RAM windows around pointer ARGS).
# Comparing the leftover r3 of a void function yields garbage -> a FALSE
# MISMATCH. Refuse rather than mis-report. (Confirmed: 7 of 8 sweep mismatches
# were exactly this — void setters whose r3 is undefined.)
case "$ABI_LINE" in
  "ABI_OK VOID"*)
    echo "AUTOHARNESS-LIMITED $FN @ $ADDR : void return — no comparable return value; side-effects are (mostly global) writes the auto-harness does not track (use a hand-harness that snapshots the touched globals/RAM)"
    exit 4;;
esac

# 3) Extract verbatim src (target + any same-file callees the caller listed)
INC="$WORK/bside_real_src.inc"
python3 "$HERE/extract_src_fns.py" "$SRC" "$INC" "$FN" "${CALLEES[@]}" >&2 || fail "extract failed"

# 3b) Generate thunks for every fn_ callee NOT defined in the .inc -> recomp.
THUNKS="$WORK/bside_thunks.inc"
THUNK_LINE="$(python3 "$HERE/gen_thunks.py" "$INC" "$THUNKS" "$SRC")"
echo "  $THUNK_LINE" >&2

# 3c) Generate SDA/global lbl_ storage + RAM loaders.
python3 "$HERE/gen_lbls.py" "$INC" "$SRC" "$WORK/bside_lbls.inc" "$WORK/bside_lbls_load.inc" >&2

# 3d) Endianness pre-flight. The B-side runs little-endian on a mirror of the
# big-endian guest RAM. For POINTER-DATA reads from FUZZED structs there is no
# single byte layout that matches the guest across widths/offsets, so a fuzzed-
# data deref can yield a FALSE mismatch (a harness artifact, never a decomp bug).
# The auto-harness is fully trustworthy for:
#   - scalar-arg functions (no pointer data), and
#   - pointer functions doing ARITHMETIC ONLY (return ptr+offset, no deref).
# Any dereference of pointer data -> AUTOHARNESS-LIMITED (hand-harness needed).
DEREF="$(python3 - "$INC" "$FN" "$SIG" <<'PY'
import re,sys
b=open(sys.argv[1],encoding='utf-8',errors='replace').read()
fn=sys.argv[2]; sig=sys.argv[3]
# isolate the TARGET function body: the DEFINITION (sig followed by '{'), not a
# forward declaration (sig followed by ';'). Match the first '<sig> {'.
defs=list(re.finditer(r'(?ms)^[A-Za-z_][\w\s\*]*?\b'+re.escape(fn)+r'\s*\([^;{]*\)\s*\{', b))
if defs:
    start=defs[0].end()
    # brace-match to the closing }
    depth=1; i=start
    while i<len(b) and depth>0:
        if b[i]=='{':depth+=1
        elif b[i]=='}':depth-=1
        i+=1
    body=b[start:i-1]
else:
    body=b
# (a) direct pointer-data dereference: *(T*)(...) read, ptr[...] indexing, ->field
deref = bool(re.search(r'\*\s*\(\s*[A-Za-z_][\w\s]*\*\s*\)\s*\(', body)) or \
        bool(re.search(r'\*\s*\(\s*[A-Za-z_][\w\s]*\*\s*\)\s*[A-Za-z_]', body)) or \
        bool(re.search(r'\b[A-Za-z_]\w*\s*\[\s*0x?[0-9A-Fa-f]', body)) or \
        bool(re.search(r'->', body))
# (b) INDIRECT: a POINTER arg is passed to a callee, which may deref it on the
# endianness-fragile mirror (e.g. fightSideGetStatus passes ptr1 to getters).
# Extract pointer-typed param names from the signature, then see if any is used
# as a call argument inside the body.
params = sig[sig.find('(')+1:sig.rfind(')')]
ptr_names=[]
for p in params.split(','):
    p=p.strip()
    if '*' in p:
        nm=re.findall(r'([A-Za-z_]\w*)\s*$', p)
        if nm: ptr_names.append(nm[0])
for nm in ptr_names:
    # a pointer param passed as an argument to a fn_ callee (which may deref it)
    if re.search(r'\bfn_[0-9A-Fa-f]{8}\s*\([^)]*\b'+re.escape(nm)+r'\b', body):
        deref=True; break
print("DEREF" if deref else "ARITH")
PY
)"
FIELDMAP_FLAG=""
if [ "$DEREF" = "DEREF" ]; then
  # STRUCT-AWARE SEEDING: try to recover the (offset,width) field map of the
  # pointer args from the recomp C call tree. If it resolves, seed each field as
  # ONE logical value (BE in A, LE in B) — removing the endianness artifact — and
  # proceed. Only fall back to LIMITED if the map can't be parsed.
  PTR_IDX="$(python3 - "$ABI_OUT" <<'PY'
import re,sys
t=open(sys.argv[1]).read()
# grab the ORC_ARGS initializer line, then all {kind, slot} pairs in order
line=[l for l in t.splitlines() if 'ORC_ARGS[]' in l]
pairs=re.findall(r'\{\s*(\d+)\s*,\s*(\d+)\s*\}', line[0]) if line else []
idx=[str(j) for j,(kind,slot) in enumerate(pairs) if kind=='1']  # kind 1 = PTR
print(','.join(idx))
PY
)"
  if [ -n "$PTR_IDX" ]; then
    FM_LINE="$(python3 "$HERE/gen_fieldmap.py" "$GEN_DIR/chunks" "$ADDR" "$(grep -oE 'ORC_NARGS [0-9]+' "$ABI_OUT"|awk '{print $2}')" "$PTR_IDX" "$WORK/bside_fieldmap.inc" 2>/dev/null)"
    NF="$(grep -oE 'ORC_NFIELDS [0-9]+' "$WORK/bside_fieldmap.inc" 2>/dev/null | awk '{print $2}')"
    echo "  fieldmap: $FM_LINE" >&2
    # Flat-seeding is valid only for a bounded field set. A very large field map
    # (>40) means the function reaches deep into a call tree that almost
    # certainly dereferences POINTER-typed fields (nested structs / child
    # pointers) — flat seeding would put random bytes where a valid pointer must
    # be, causing the recomp to read wild memory (crash) or diverge. Refuse and
    # classify LIMITED rather than risk a crash or a false result.
    if [ -n "$NF" ] && [ "$NF" -gt 0 ] && [ "$NF" -le 40 ]; then
      FIELDMAP_FLAG="-DORC_USE_FIELDMAP"
    elif [ -n "$NF" ] && [ "$NF" -gt 40 ]; then
      echo "AUTOHARNESS-LIMITED $FN @ $ADDR : field map too large ($NF fields) — deep call tree with probable nested/child-pointer fields that flat seeding cannot satisfy; use the hand-harness"
      exit 4
    fi
  fi
  if [ -z "$FIELDMAP_FLAG" ]; then
    echo "AUTOHARNESS-LIMITED $FN @ $ADDR : dereferences fuzzed pointer data and the field map could not be recovered (nested/child-pointer or index-from-memory); use the hand-harness with logical-field seeding"
    exit 4
  fi
fi

# 4) Compile generic harness against the FULL image
OUT="$WORK/auto_$FN"
RETKIND=""
grep -q 'ORC_RET_FLT' "$ABI_OUT" && RETKIND="-DORC_RET_FLT_BUILD"
cc -O1 $FIELDMAP_FLAG -Wno-int-conversion -Wno-implicit-function-declaration \
   -I"$DOLRECOMP_SRC" -I"$WORK" -I"$HERE" \
   -DORC_FN_ADDR=0x${ADDR#0x}u -DORC_FN_NAME=$FN \
   -o "$OUT" "$HERE/auto_harness.c" "$LIBCOLO" -lm 2>"$WORK/cc_$FN.log"
if [ $? -ne 0 ]; then
   echo "  (compile errors -> AUTOHARNESS-FAILED; see $WORK/cc_$FN.log)" >&2
   head -8 "$WORK/cc_$FN.log" >&2
   fail "B-side compile error (likely struct-typed args / header deps)"
fi

# 5) Run
ORACLE_DOL="$ORACLE_DOL" "$OUT" "$ORACLE_DOL"
