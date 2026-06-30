#!/usr/bin/env python3
"""Single source of truth for the decomp.me byte-match queue.

Ledger:   build_pc/decompme/scratches.json  (fn -> {url, tu, compiler, local_pct,
          scratch_pct, status, notes})
Rendered: build_pc/decompme/queue.md        (human-readable table, regenerated)

Commands:
  add <fn> <url> [--tu T] [--compiler C] [--status S] [--notes ...]   create/replace entry
  set <fn> <field> <value>                                            update one field
  render                                                              rewrite queue.md
  list                                                                print ledger

status vocabulary: queued | wip | near-miss | MATCH | DONE | wall | already-matched
(MATCH = 100% on scratch but not yet integrated; DONE = integrated into src + gate-verified)
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
LEDGER = ROOT / "build_pc" / "decompme" / "scratches.json"
QUEUE = ROOT / "build_pc" / "decompme" / "queue.md"


def load():
    return json.loads(LEDGER.read_text(encoding="utf-8")) if LEDGER.exists() else {}


def save(d):
    LEDGER.write_text(json.dumps(d, indent=2, sort_keys=True) + "\n")


def render(d):
    order = {"wip": 0, "near-miss": 1, "MATCH": 2, "queued": 3,
             "wall": 4, "already-matched": 5, "DONE": 6}
    rows = sorted(d.items(), key=lambda kv: (order.get(kv[1].get("status"), 9), kv[0]))
    out = []
    out.append("# decomp.me scratch queue -- GC6E01 (Pokemon Colosseum)\n")
    out.append("Byte-match decomp pipeline. **Single source of truth = scratches.json** "
               "(regenerate this file with `python tools/decomp_work/scratch_track.py render`).\n")
    out.append("Real targets = ACTIVE asm-wrappers (`asm fn(){#include .inc}` NOT under `#if 0`); "
               "list them with `tools/decomp_work/find_active_asm.py` / `find_leaf_asm.py`. "
               "NOTE: a `.inc` file existing does NOT mean unmatched -- most have active C already.\n")
    out.append("Workflow per fn: extract_target.py -> local mwcc -S search -> create_scratch.js "
               "(auto-logs here) -> configure_scratch.js/drive.js push -> readmatch.js -> at 100%, "
               "flip `#if 1`->`#if 0` in src to activate the C (NEVER the reverse = fraud) -> verify gate -> DONE.\n")
    out.append("status: queued | wip | near-miss | MATCH(100% on scratch, not integrated) | "
               "DONE(integrated+gate-verified) | wall | already-matched\n")
    out.append("| status | local% | scratch% | fn | TU | compiler | scratch | notes |")
    out.append("|---|---|---|---|---|---|---|---|")
    for fn, e in rows:
        url = e.get("url", "")
        sid = url.rsplit("/", 1)[-1] if url else "-"
        link = f"[{sid}]({url})" if url else "-"
        out.append(f"| {e.get('status','?')} | {e.get('local_pct','-')} | "
                   f"{e.get('scratch_pct','-')} | {fn} | {e.get('tu','-')} | "
                   f"{e.get('compiler','-')} | {link} | {e.get('notes','')} |")
    QUEUE.write_text("\n".join(out) + "\n")
    print(f"rendered {len(rows)} entries -> {QUEUE}")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return
    cmd = sys.argv[1]
    d = load()
    if cmd == "add":
        fn, url = sys.argv[2], sys.argv[3]
        e = d.get(fn, {})
        e["url"] = url
        args = sys.argv[4:]
        i = 0
        while i < len(args):
            if args[i] == "--notes":
                e["notes"] = " ".join(args[i + 1:])
                break
            key = args[i].lstrip("-")
            e[key] = args[i + 1]
            i += 2
        d[fn] = e
        save(d)
        render(d)
    elif cmd == "set":
        fn, field, value = sys.argv[2], sys.argv[3], sys.argv[4]
        if field in ("local_pct", "scratch_pct"):
            value = float(value) if "." in value else int(value)
        d.setdefault(fn, {})[field] = value
        save(d)
        render(d)
    elif cmd == "render":
        render(d)
    elif cmd == "list":
        print(json.dumps(d, indent=2, sort_keys=True))
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
