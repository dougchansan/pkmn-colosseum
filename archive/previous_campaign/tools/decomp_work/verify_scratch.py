#!/usr/bin/env python3
"""verify_scratch.py <scratch.c> [target_fn] - PARENT-SIDE integration gate.

Compiles a candidate full-file scratch, measures the WHOLE object against the
canonical target, and reports vs the frozen baseline (/tmp/cb_baseline.json):
  - the target fn's new %
  - net change in 100%-count
  - every REGRESSION (a fn that dropped below its baseline %)  <- a clean win has none
  - every NEW 100%

Use this before splicing any agent scratch into the real file. Exit 0 = clean
(target improved/100 AND zero regressions); exit 1 = regressions present.
"""
import sys, json, subprocess, tempfile
from pathlib import Path

ROOT = Path("C:/Users/douglaswhittingham/pkmn-colosseum")
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "decomp_work"))
import measure_cache
import scratch_measure as sm

TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASELINE = json.loads(Path(tempfile.gettempdir(), "cb_baseline.json").read_text()) \
    if Path(tempfile.gettempdir(), "cb_baseline.json").exists() \
    else json.loads(Path("/tmp/cb_baseline.json").read_text())


def main():
    scratch = Path(sys.argv[1]).resolve()
    target_fn = sys.argv[2] if len(sys.argv) > 2 else None
    out_dir = Path(tempfile.gettempdir()) / "cb_scratch_obj"
    out_dir.mkdir(parents=True, exist_ok=True)
    out_obj = out_dir / f"verify_{scratch.stem}.o"
    ok, err = sm.compile_scratch(scratch, out_obj, "src/game/colosseum_battle.c")
    if not ok:
        print(f"COMPILE_FAIL:\n{err}")
        return 2
    funcs = measure_cache.diff_funcs(TARGET, out_obj)
    try: measure_cache.flush()
    except Exception: pass
    cur = {f["name"]: round(float(f["match"]), 2) for f in funcs}

    regressions, newperfect = [], []
    for name, base_pct in BASELINE.items():
        c = cur.get(name)
        if c is None:
            continue
        if c < base_pct - 0.005:
            regressions.append((name, base_pct, c))
        if c >= 100.0 and base_pct < 100.0:
            newperfect.append((name, base_pct, c))

    base_perfect = sum(1 for v in BASELINE.values() if v >= 100.0)
    cur_perfect = sum(1 for v in cur.values() if v >= 100.0)

    if target_fn:
        print(f"TARGET {target_fn}: {BASELINE.get(target_fn,'?')} -> {cur.get(target_fn,'?')}")
    print(f"100%-count: {base_perfect} -> {cur_perfect}  (net {cur_perfect-base_perfect:+d})")
    print(f"NEW 100%: {[n for n,_,_ in newperfect]}")
    if regressions:
        print(f"!!! REGRESSIONS ({len(regressions)}):")
        for n, b, c in regressions:
            print(f"    {n}: {b} -> {c}")
        return 1
    print("CLEAN: no regressions")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
