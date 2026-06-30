#!/usr/bin/env bash
# batch_run.sh - run oracle_auto.sh across a list of equivalent.txt functions
# and emit a per-function status report.
#
# Each line of the batch list: <fn> <hexaddr> <src_basename> [callee...]
# src files must be present in $SRCDIR (pushed to the box).
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="${SRCDIR:-/tmp/oracle_repo}"
export DOLRECOMP_SRC="${DOLRECOMP_SRC:-/tmp/DolRecomp/src}"
export LIBCOLO="${LIBCOLO:-/tmp/colo_full/libcolo.a}"
export GEN_DIR="${GEN_DIR:-/tmp/colo_out/generated}"
export ORACLE_DOL="${ORACLE_DOL:-/tmp/start.dol}"
export WORK="${WORK:-/tmp/oracle}"

LIST="${1:?batch list file}"
printf '%-16s %-10s %-22s %s\n' "FN" "ADDR" "FILE" "STATUS"
printf '%-16s %-10s %-22s %s\n' "----------------" "--------" "----------------------" "------"
while read -r fn addr file rest; do
  [ -z "${fn:-}" ] && continue
  case "$fn" in \#*) continue;; esac
  rm -f "$WORK"/bside_*.inc "$WORK"/abi.inc
  out="$("$HERE/oracle_auto.sh" "$fn" "$addr" "$SRCDIR/$file" $rest 2>"$WORK/batch_$fn.err")"
  rc=$?
  # extract the headline status token
  if echo "$out" | grep -q "^AUTO .* -> "; then
     st="$(echo "$out" | sed -n 's/.*-> //p')"
     extra="$(echo "$out" | grep -oE '[0-9]+ run, [0-9]+ mismatch, [0-9]+ unreachable, [0-9]+ nonzero-ret')"
     st="$st  ($extra)"
  elif echo "$out" | grep -q "AUTOHARNESS-LIMITED"; then st="AUTOHARNESS-LIMITED ($(echo "$out"|sed -n 's/.*AUTOHARNESS-LIMITED[^:]*: //p'|cut -c1-48))"
  elif echo "$out" | grep -q "AUTOHARNESS-FAILED"; then st="AUTOHARNESS-FAILED ($(echo "$out"|sed -n 's/.*AUTOHARNESS-FAILED[^:]*: //p'))"
  elif [ "$rc" -ge 128 ]; then st="AUTOHARNESS-FAILED (crash signal $((rc-128)) — callee tree deref'd unmapped pointer; NOT a verification)"
  else st="ERROR rc=$rc: $(echo "$out" | tail -1)"; fi
  printf '%-16s %-10s %-22s %s\n' "$fn" "$addr" "$file" "$st"
done < "$LIST"
