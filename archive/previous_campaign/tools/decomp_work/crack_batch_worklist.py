#!/usr/bin/env python3
"""crack_batch_worklist.py — pick the closest real-C near-miss functions that are NOT
currently locked by the fleet, for the lever-crack-batch workflow. Prints a JSON array of
[fn, src] pairs (closest-first).

Mechanical REG-COLORING walls are the target: directed lever-application (named locals +
declaration-order) cracks them, which is cheaper than the permuter's blind search. We DON'T
pre-classify here (that needs a compile) — the workflow's per-fn agent runs classify_residual
and only applies levers if it's REG-COLORING, banking the rest for the permuter.

Sources, in order:
  1. .omc/permuter_queue.json   (already-curated real-C near-misses, closest-first), else
  2. wall_ledger.json NEARWALL  (real C 95-99.95%, unattempted).
Skips any fn whose src file is locked in build/locks/ so the batch never fights the fleet.

usage: crack_batch_worklist.py [--n 12]
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]   # tools/decomp_work/<file> -> repo
sys.path.insert(0, str(ROOT / "tools" / "decomp_work" / "coordination"))


def locked_srcs():
    """Set of repo-relative src paths currently locked by the fleet (file-based locks)."""
    out = set()
    try:
        import locks
        for r in locks.list_locks():
            if r.get("scope") == "file":
                out.add(r.get("key"))
            elif r.get("file"):
                out.add(r.get("file"))
    except Exception:
        pass
    return {s for s in out if s}


def main():
    a = sys.argv[1:]
    n = int(a[a.index("--n") + 1]) if "--n" in a else 12
    locked = locked_srcs()
    work, seen = [], set()

    pq = ROOT / ".omc" / "permuter_queue.json"
    if pq.exists():
        try:
            for fn, src in json.load(open(pq, encoding="utf-8")):
                if src in locked or fn in seen:
                    continue
                if (ROOT / src).exists():
                    work.append([fn, src]); seen.add(fn)
        except (ValueError, OSError):
            pass

    if len(work) < n:
        led_path = ROOT / "build" / "wall_ledger.json"
        try:
            led = json.load(open(led_path, encoding="utf-8"))
            rows = [(v.get("pct", 0.0), fn, "src/" + v["file"] + ".c")
                    for fn, v in led.items()
                    if isinstance(v, dict) and v.get("bucket") == "NEARWALL"
                    and not v.get("attempted")]
            rows.sort(reverse=True)
            for _pct, fn, src in rows:
                if src in locked or fn in seen:
                    continue
                if (ROOT / src).exists():
                    work.append([fn, src]); seen.add(fn)
        except (ValueError, OSError, KeyError):
            pass

    print(json.dumps(work[:n]))


if __name__ == "__main__":
    main()
