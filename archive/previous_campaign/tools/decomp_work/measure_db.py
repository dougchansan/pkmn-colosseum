#!/usr/bin/env python3
"""measure_db.py — the single FRESH source of per-function match%.

Every band.py measurement (_rows) appends one line here, so the dashboard and the queue
builders can read the LATEST measured pct instead of the periodically-rebuilt — and
therefore stale — wall_ledger.json / report.json. This session we watched the dashboard
DONE count freeze for 1.5h and 400+ real wins sit in the wrong bucket precisely because
membership only refreshed on a manual `wall_ledger.py build`. A write-on-every-measure
cache removes that whole class of staleness.

Design: APPEND-ONLY JSONL — safe under concurrent lanes (no read-modify-write race; each
band compile appends one line and readers fold). One line per band COMPILE:
    {"ts": <epoch>, "src": "src/game/foo.c", "compiler": "1.2.5n", "pcts": {fn: pct, ...}}
Readers fold to the latest line per src. `compact` rewrites the log to one line per src.

CLI:
    python measure_db.py fresh [--fn fn_X]   # dump folded {fn: {pct,src,ts,compiler}} (or one fn)
    python measure_db.py compact             # bound growth: keep only the latest line per src
    python measure_db.py age                 # seconds since the newest measurement
"""
import json
import os
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]                       # tools/decomp_work -> repo
LOG = Path(os.environ.get("MEASURE_CACHE", ROOT / "build" / "measure_cache.jsonl"))


def record(src, pcts, compiler=""):
    """Append one measurement line. No-op + swallow all errors so it can NEVER break a
    measurement. Only real sources (src/...) are cached — build/ integrate temps are skipped."""
    try:
        s = str(src or "").replace("\\", "/")
        if not s.startswith("src/") or not pcts:
            return
        LOG.parent.mkdir(parents=True, exist_ok=True)
        rec = {"ts": time.time(), "src": s, "compiler": compiler,
               "pcts": {k: round(float(v), 3) for k, v in pcts.items()}}
        with LOG.open("a", encoding="utf-8") as f:
            f.write(json.dumps(rec) + "\n")
    except Exception:
        pass


def _fold():
    """src -> latest record."""
    latest = {}
    try:
        with LOG.open(encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                try:
                    rec = json.loads(line)
                except ValueError:
                    continue
                s = rec.get("src")
                if not s:
                    continue
                prev = latest.get(s)
                if prev is None or rec.get("ts", 0) >= prev.get("ts", 0):
                    latest[s] = rec
    except OSError:
        return {}
    return latest


def load_fresh():
    """Folded view: {fn: {pct, src, ts, compiler}} from the latest measurement per src."""
    out = {}
    for s, rec in _fold().items():
        ts, comp = rec.get("ts", 0.0), rec.get("compiler", "")
        for fn, pct in (rec.get("pcts") or {}).items():
            cur = out.get(fn)
            if cur is None or ts >= cur["ts"]:    # newest wins if a fn appears in >1 src
                out[fn] = {"pct": float(pct), "src": s, "ts": ts, "compiler": comp}
    return out


def newest_ts():
    return max((r.get("ts", 0.0) for r in _fold().values()), default=0.0)


def compact():
    latest = _fold()
    try:
        LOG.parent.mkdir(parents=True, exist_ok=True)
        tmp = LOG.with_suffix(".jsonl.tmp")
        tmp.write_text("".join(json.dumps(r) + "\n" for r in latest.values()), encoding="utf-8")
        os.replace(str(tmp), str(LOG))
    except OSError:
        pass
    return len(latest)


def main(argv=None):
    a = (argv if argv is not None else sys.argv[1:])
    cmd = a[0] if a else "fresh"
    if cmd == "compact":
        n = compact()
        print(f"compacted measure_cache to {n} source line(s)")
    elif cmd == "age":
        t = newest_ts()
        print(f"{round(time.time() - t)}s since newest measurement" if t else "no measurements yet")
    else:  # fresh
        fresh = load_fresh()
        if "--fn" in a:
            fn = a[a.index("--fn") + 1]
            print(json.dumps(fresh.get(fn, {}), indent=1))
        else:
            print(json.dumps(fresh, indent=1))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
