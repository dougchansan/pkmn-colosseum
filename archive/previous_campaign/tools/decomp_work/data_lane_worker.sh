#!/usr/bin/env bash
# data_lane_worker.sh <role> — claim source-data chunks for the data campaign.
#
# Codex lanes may edit source for one claimed chunk at a time. GLM lanes are
# research-only: write evidence/proposed constants, but do not edit tracked files.
set -uo pipefail

ROLE="${1:?usage: data_lane_worker.sh <codex|codex2|glm>}"
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
[ -f "$HOME/.config/decomp-keys/cockpit.env" ] && { set -a; . "$HOME/.config/decomp-keys/cockpit.env"; set +a; }

QUEUE="${DATA_QUEUE:-tools/decomp_work/data_campaign_queue.json}"
LOCKD="build/data_fleet_locks"
DONED="build/data_fleet_done"
PROMPTD="build/agent_training/data_prompts"
MODE="${DATA_MODE:-research}"
if [ "$ROLE" = "codex" ] || [ "$ROLE" = "codex2" ]; then
  MODE="${DATA_MODE:-edit}"
fi
mkdir -p "$LOCKD" "$DONED" "$PROMPTD" build/agent_training/data_research

run_agent() {
  local task="$1"
  case "$ROLE" in
    codex|codex2) codex exec --dangerously-bypass-approvals-and-sandbox "$task" ;;
    glm)          opencode run --model "${DECOMP_GLM_MODEL:-glm/glm-5.2}" "$task" ;;
    *)            echo "unknown role $ROLE" >&2; return 1 ;;
  esac
}

claim_chunk() {
  python3 - "$QUEUE" "$LOCKD" "$DONED" "$ROLE" "${DATA_PREFER_IDS:-}" <<'PY'
import json
import os
import sys
from pathlib import Path

queue_path = Path(sys.argv[1])
lock_dir = Path(sys.argv[2])
done_dir = Path(sys.argv[3])
role = sys.argv[4]
prefer = [item for item in sys.argv[5].split(",") if item]
queue = json.loads(queue_path.read_text(encoding="utf-8")).get("queue", [])

def score(item):
    try:
        pref = prefer.index(item.get("id"))
    except ValueError:
        pref = 999
    lane_rank = {"NUMERIC": 0, "STRING": 1, "LAYOUT": 2, "RESEARCH": 3}
    return (pref, lane_rank.get(item.get("lane"), 9), item.get("priority_score", 9999), item.get("start", ""))

for item in sorted(queue, key=score):
    chunk_id = item.get("id")
    if not chunk_id:
        continue
    if (done_dir / f"{chunk_id}.{role}").exists():
        continue
    lock = lock_dir / chunk_id
    try:
        os.mkdir(lock)
    except FileExistsError:
        continue
    (lock / "owner").write_text(role + "\n", encoding="utf-8")
    (lock / "item.json").write_text(json.dumps(item, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(item, separators=(",", ":")))
    raise SystemExit(0)
raise SystemExit(1)
PY
}

make_task() {
  local item_json="$1"
  python3 - "$item_json" "$ROLE" "$MODE" <<'PY'
import json
import sys

item = json.loads(sys.argv[1])
role = sys.argv[2]
mode = sys.argv[3]
chunk_id = item["id"]
symbols = item.get("symbols") or []
symbol_names = ", ".join(sym.get("name", "") for sym in symbols[:40])
print(f"""You are working on the Pokémon Colosseum source-data matching campaign.

Hard rules:
- NO .inc files, incbin, raw target byte arrays, asm wrappers, inline asm, or pasted ROM bytes.
- Only count data after `python3 tools/verify_data_progress.py` passes.
- Prefer typed C source: `const f32`, `const f64`, strings, pointer tables, structs, enums.
- If evidence is insufficient, write a research note and stop. Do not fake progress.
- Do not commit or push. Parent will review/commit.

Role: {role}
Mode: {mode}
Claimed chunk:
- id: {chunk_id}
- lane: {item.get("lane")}
- range: {item.get("start")}..{item.get("end")} ({item.get("size")} bytes)
- target object: {item.get("target_object")}
- reason: {item.get("reason")}
- symbols: {symbol_names}

Context to read first:
- AGENTS.md
- tools/decomp_work/DATA_CAMPAIGN.md
- config/GC6E01/data_progress.json
- tools/decomp_work/data_campaign_queue.json
- src/crt/extras.c if the symbols are CRT math `.sdata2` constants.

Recommended workflow:
1. Find all references to this chunk's labels in `src/` and `config/GC6E01/symbols.build.txt`.
2. Identify source ownership and typed data shape.
3. For contiguous CRT math chunks, extend `crt/sdata2_math.c` split/source only with typed `f64` constants.
4. Regenerate split/build as needed:
   `ninja build/GC6E01/config.json && ninja build/GC6E01/ok`
5. If source bytes match, update `config/GC6E01/data_progress.json`, then run:
   `python3 tools/verify_data_progress.py`
   `python3 tools/apply_data_progress_to_report.py report.json`
   `python3 tools/sync_progress_metadata.py --sync`
   `python3 tools/decomp_work/build_data_worklist.py --section .sdata2 --chunk-bytes 256 --output tools/decomp_work/data_sdata2_worklist.json`
   `python3 tools/decomp_work/prioritize_data_worklist.py`
   `python3 tools/decomp_work/snapshot_metrics.py`

Mode-specific instruction:
""")
if mode == "edit":
    print("""Make the smallest safe source edit for this one chunk if evidence is strong.
If the chunk is not sourceable in one pass, write `build/agent_training/data_research/{chunk_id}_codex.md` with blockers and leave tracked files unchanged.""")
else:
    print(f"""Research only. Do not edit tracked files.
Write `build/agent_training/data_research/{chunk_id}_{role}.md` with:
- source ownership evidence
- label -> proposed typed C declaration
- whether it is safe for Codex edit mode
- blockers/gaps""")
print("""
Before exit, print one line prefixed with DATA_TRAINING_SUMMARY containing compact JSON:
role, mode, chunk_id, range, source_owner, safe_to_edit, matched_bytes_added,
files_changed, evidence, blockers.
""")
PY
}

echo "[$ROLE:data] lane up — mode=$MODE queue=$QUEUE"
while :; do
  item="$(claim_chunk || true)"
  if [ -z "$item" ]; then
    echo "[$ROLE:data] queue drained/locked — idle 120s"
    sleep 120
    continue
  fi
  chunk_id="$(python3 -c 'import json,sys; print(json.loads(sys.argv[1])["id"])' "$item")"
  prompt="$PROMPTD/$(date -u +%Y%m%dT%H%M%SZ)_${ROLE}_${chunk_id}.md"
  task="$(make_task "$item")"
  printf '%s\n' "$task" > "$prompt"
  echo "[$ROLE:data] ===== claim $chunk_id $(date +%H:%M:%S) prompt=$prompt ====="
  if run_agent "$task" 2>&1; then
    rc=0
  else
    rc=$?
    echo "[$ROLE:data] agent exited non-zero rc=$rc"
  fi
  touch "$DONED/$chunk_id.$ROLE"
  echo "[$ROLE:data] ===== done $chunk_id $(date +%H:%M:%S) ====="
  rm -rf "$LOCKD/$chunk_id"
done
