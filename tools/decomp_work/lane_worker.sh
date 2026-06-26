#!/usr/bin/env bash
# lane_worker.sh <role> — one fleet lane. Claims an unlocked file from the
# low-bucket queue, hands it to this lane's agent to grind to byte-match via the
# band harness, then loops to the next file. An atomic per-file lock (mkdir)
# guarantees two lanes never work the same file. Wins land in
# build/band_wins/pl_<role>.json; auto_gate.sh applies + commits them.
#
# Agent per role: opus/sonnet -> claude -p (autonomous edits); codex/codex2 ->
# codex exec; glm -> opencode run (z.ai glm-5.2). Keys from cockpit.env.
set -uo pipefail

ROLE="${1:?usage: lane_worker.sh <opus|sonnet|codex|codex2|glm>}"
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
[ -f "$HOME/.config/decomp-keys/cockpit.env" ] && { set -a; . "$HOME/.config/decomp-keys/cockpit.env"; set +a; }

QUEUE="${FLEET_QUEUE:-build/low_attack_queue.txt}"
LOCKD="build/fleet_locks"
TAG="pl_$ROLE"
mkdir -p "$LOCKD"

_hash() { printf '%s' "$1" | (md5 -q 2>/dev/null || md5sum | awk '{print $1}'); }

run_agent() {
  local task="$1"
  case "$ROLE" in
    opus)         claude -p --model opus   --dangerously-skip-permissions "$task" ;;
    sonnet)       claude -p --model sonnet --dangerously-skip-permissions "$task" ;;
    codex|codex2) codex exec --dangerously-bypass-approvals-and-sandbox "$task" ;;
    glm)          opencode run --model "${DECOMP_GLM_MODEL:-glm/glm-5.2}" "$task" ;;
    *)            echo "unknown role $ROLE" >&2; return 1 ;;
  esac
}

make_task() {
  local file="$1" fns="$2"
  cat <<EOF
You are decompiling Pokémon Colosseum (GameCube, Metrowerks CodeWarrior 1.2.5n/1.3).
First read AGENT_ONBOARDING.md and skim WALLS.md (known walls — don't refight them).

Work ONLY on $file. Your private band tag is $TAG (never use another tag).
Make these LOW-bucket functions byte-match the target object:
  $fns

Use the band harness — it gives you a PRIVATE scratch copy so you never collide
with other lanes. Do NOT edit any other file, do NOT touch *.inc files, and NEVER
paste assembly or flip #if 0/#if 1 (that is fraud and will be rejected):

  python3 tools/decomp_work/band.py init  $TAG $file
  # for each target fn:
  python3 tools/decomp_work/band.py check $TAG <fn>      # per-fn match%
  python3 tools/decomp_work/band.py diff  $TAG <fn>      # target-vs-ours asm
  #   ...edit tools/decomp_work/scratch/band_$TAG.c to fix the codegen mismatch,
  #   re-run check until the fn is 100% byte-exact...
  python3 tools/decomp_work/band.py save  $TAG <fns that reached 100%>

Write only real, idiomatic C89 (CW conventions: declarations first, no float
literals, correct signed/unsigned, block scope for repeated r13 loads). Save every
function that reaches 100%. When the listed fns are done or genuinely walled, stop.
EOF
}

echo "[$ROLE] lane up — queue=$QUEUE tag=$TAG"
while :; do
  claimed=""; cfns=""
  while read -r file fns; do
    [ -z "$file" ] && continue
    case "$file" in \#*) continue ;; esac
    [ -f "$file" ] || continue
    h="$(_hash "$file")"
    if mkdir "$LOCKD/$h" 2>/dev/null; then
      printf '%s\n' "$ROLE" > "$LOCKD/$h/owner"
      printf '%s\n' "$file" > "$LOCKD/$h/file"
      claimed="$file"; cfns="$fns"; break
    fi
  done < <(grep -vE '^\s*#|^\s*$' "$QUEUE")

  if [ -z "$claimed" ]; then
    echo "[$ROLE] queue drained — idle 90s"; sleep 90; continue
  fi
  echo "[$ROLE] ===== grind $claimed ($(printf '%s' "$cfns" | wc -w | tr -d ' ') fns)  $(date +%H:%M:%S) ====="
  # stream the agent's work live into the pane (no tail buffering)
  run_agent "$(make_task "$claimed" "$cfns")" 2>&1 || echo "[$ROLE] agent exited non-zero"
  echo "[$ROLE] ===== done $claimed  $(date +%H:%M:%S) ====="
done
