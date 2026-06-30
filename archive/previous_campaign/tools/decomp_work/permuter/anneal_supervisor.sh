#!/bin/bash
# anneal_supervisor.sh — keeps the quantum-annealer swarm continuously fed.
# grind2.py drains its queue then all workers exit together ("one ends -> all
# end"). This loop refills the swarm queue with the closest winnable near-misses
# (refill_queue.py) and relaunches the swarm, so it never stays dead.
cd /mnt/c/Users/douglaswhittingham/pkmn-colosseum || exit 1

GRIND_PROFILE="${GRIND_PROFILE:-default}"
if [ "$GRIND_PROFILE" = "desktop" ] || [ "$GRIND_PROFILE" = "low" ] || \
   [ "$GRIND_PROFILE" = "low-impact" ] || [ "$GRIND_PROFILE" = "friendly" ]; then
  export GRIND_WORKERS="${GRIND_WORKERS:-1}"
  export GRIND_JOBS="${GRIND_JOBS:-1}"
  export GRIND_STAGGER="${GRIND_STAGGER:-15}"
  GRIND_NICE="${GRIND_NICE:-15}"
else
  GRIND_NICE="${GRIND_NICE:-0}"
fi

run_lowprio() {
  local -a cmd=("$@")
  if command -v ionice >/dev/null 2>&1; then
    cmd=(ionice -c3 "${cmd[@]}")
  fi
  if [ "${GRIND_NICE:-0}" != "0" ] && command -v nice >/dev/null 2>&1; then
    cmd=(nice -n "$GRIND_NICE" "${cmd[@]}")
  fi
  "${cmd[@]}"
}

# ---- pretty, width-aware output -------------------------------------------
# Recompute width each draw so resizing the pane reflows the frame. Clamp to a
# sane band so a giant or tiny terminal still looks right.
term_width() {
  local w
  w=$(tput cols 2>/dev/null) || w=${COLUMNS:-100}
  [ -z "$w" ] || [ "$w" -lt 8 ] 2>/dev/null && w=100
  [ "$w" -gt 120 ] 2>/dev/null && w=120
  [ "$w" -lt 48  ] 2>/dev/null && w=48
  echo "$w"
}
# ANSI palette (disabled when stdout is not a tty)
if [ -t 1 ]; then
  C_RST=$'\033[0m'; C_DIM=$'\033[2m'; C_BLD=$'\033[1m'
  C_CYN=$'\033[36m'; C_GRN=$'\033[32m'; C_YEL=$'\033[33m'; C_RED=$'\033[31m'; C_MAG=$'\033[35m'
else
  C_RST=; C_DIM=; C_BLD=; C_CYN=; C_GRN=; C_YEL=; C_RED=; C_MAG=
fi
repeat() { local n=$1 ch=$2 out=; while [ "$n" -gt 0 ]; do out="$out$ch"; n=$((n-1)); done; printf '%s' "$out"; }

# rule "char" [color] — full-width horizontal rule
rule() { local w; w=$(term_width); printf '%s%s%s\n' "${2:-$C_DIM}" "$(repeat "$w" "${1:-─}")" "$C_RST"; }

# banner "TITLE" — boxed, centred, full width
banner() {
  local w pad title="$1"; w=$(term_width)
  local inner=$((w - 2)) tlen=${#title}
  [ "$tlen" -gt "$inner" ] && { title="${title:0:$inner}"; tlen=$inner; }
  pad=$(( (inner - tlen) / 2 ))
  printf '%s┌%s┐%s\n'  "$C_CYN" "$(repeat "$inner" ─)" "$C_RST"
  printf '%s│%s%s%s%s%s│%s\n' "$C_CYN" "$(repeat "$pad" ' ')" "$C_BLD" "$title" "$C_RST$C_CYN" "$(repeat "$((inner - pad - tlen))" ' ')" "$C_RST"
  printf '%s└%s┘%s\n'  "$C_CYN" "$(repeat "$inner" ─)" "$C_RST"
}

# step COLOR GLYPH "message" — one timestamped status line, padded to width
step() {
  local color="$1" glyph="$2" msg="$3" w stamp prefix plain
  w=$(term_width); stamp=$(date +%H:%M:%S)
  prefix="${C_DIM}${stamp}${C_RST} ${color}${glyph}${C_RST} "
  plain="$stamp $glyph $msg"                       # for length accounting
  [ ${#plain} -gt "$w" ] && msg="${msg:0:$((w - ${#stamp} - 4))}…"
  printf '%s%s\n' "$prefix" "$msg"
}

preflight() {
  local t=/tmp/_mwcc_preflight
  rm -f "$t.o"
  echo 'int preflight_fn(int x) { return x + 1; }' > "$t.c"
  if [ -x tools/decomp_work/permuter/wibo ]; then
    tools/decomp_work/permuter/wibo tools/mwcc_compiler/GC/1.3/mwcceppc.exe \
        -nodefaults -proc gekko -c -o "$t.o" "$t.c" >/dev/null 2>&1
  else
    ./tools/mwcc_compiler/GC/1.3/mwcceppc.exe -nodefaults -proc gekko \
        -c -o "$(wslpath -w "$t.o")" "$(wslpath -w "$t.c")" >/dev/null 2>&1
  fi
  [ -s "$t.o" ]
}

cycle=0
banner "QUANTUM-ANNEALER SWARM SUPERVISOR"
step "$C_DIM" "·" "permuter swarm · refills closest near-misses each cycle · Ctrl-C to stop"
while true; do
  cycle=$((cycle + 1))
  until preflight; do
    step "$C_RED" "✗" "mwcc-via-interop BROKEN (WSL interop down?) — retry in 60s"
    sleep 60
  done
  rule
  banner "CYCLE ${cycle}"
  step "$C_CYN" "↻" "refilling swarm queue (closest winnable near-misses) ..."
  python3 tools/decomp_work/permuter/refill_queue.py --n 24 --min 90 --max 99.999 2>&1 | sed "s/^/   ${C_DIM}│${C_RST} /"
  step "$C_GRN" "▶" "launching grind2 swarm ..."
  rule "·"
  run_lowprio python3 tools/decomp_work/permuter/grind2.py 2>&1
  rule "·"
  step "$C_YEL" "■" "cycle ${cycle} finished — refill + restart in 5s"
  sleep 5
done
