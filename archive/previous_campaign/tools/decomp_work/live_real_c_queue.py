#!/usr/bin/env python3
"""Build a fleet queue from live local objdiff results.

report.json can be stale and can include active asm-wrapper artifacts. This script
compiles candidate TUs, measures the actual local object, excludes active .inc
wrappers and logged equivalents, then writes a queue of real-C non-100 functions.

Default output:
  build/pin_queue.txt

Examples:
  python3 tools/decomp_work/live_real_c_queue.py --max-files 24
  python3 tools/decomp_work/live_real_c_queue.py --sources src/game/foo.c src/hsd/bar.c
"""
from __future__ import annotations

import argparse
import contextlib
import io
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "decomp_work"))

import band as bandmod  # noqa: E402
import compile_check as cc  # noqa: E402
import lane_glm as lane  # noqa: E402

FN_RE = re.compile(r"fn_[0-9A-Fa-f]{8}")


def _num(x) -> float:
    try:
        return float(x)
    except (TypeError, ValueError):
        return 0.0


def _equiv_fns() -> set[str]:
    p = ROOT / "tools" / "decomp_work" / "equivalent.txt"
    if not p.exists():
        return set()
    return set(FN_RE.findall(p.read_text(errors="replace")))


def _report_sources(max_report_missing: int) -> list[Path]:
    rpath = ROOT / "report.json"
    if not rpath.exists():
        return []
    data = json.loads(rpath.read_text())
    rows = []
    for unit in data.get("units", []):
        name = unit.get("name")
        if not name:
            continue
        src = ROOT / "src" / f"{name}.c"
        if not src.exists():
            continue
        miss = []
        for fn in unit.get("functions", []):
            pct = _num(fn.get("fuzzy_match_percent"))
            if pct < 99.995:
                miss.append((pct, _num(fn.get("size") or fn.get("size_bytes")), fn.get("name")))
        if 0 < len(miss) <= max_report_missing:
            rows.append((len(miss), -sum(x[1] for x in miss), src))
    return [src for _, __, src in sorted(rows)]


def _objdiff_pcts(src: Path) -> dict[str, float] | None:
    try:
        with contextlib.redirect_stdout(io.StringIO()):
            base = cc.compile_source(src, verbose=False)
    except BaseException:
        return None
    target = cc.find_target_obj(src)
    diff = cc._run_objdiff_json(target, base)
    if not diff:
        return None
    out = {}
    for fn in cc._list_function_symbols(diff):
        s = cc._summarize_symbol(diff, fn)
        if s.get("symbol_found") and s.get("total_instructions", 0) > 0:
            out[fn] = float(s.get("match_percent", 0.0))
    return out


def live_line(src: Path, max_fns: int, min_pct: float, max_pct: float, equiv: set[str]) -> tuple[str, list[str], str] | None:
    pcts = _objdiff_pcts(src)
    if not pcts:
        return None
    text = src.read_text(encoding="latin-1", errors="replace")
    candidates = {fn for fn, pct in pcts.items() if min_pct <= pct < max_pct and fn not in equiv}
    active_inc = bandmod.active_asm_fns(src, candidates)
    keep = []
    for fn in sorted(candidates, key=lambda f: (-pcts[f], f)):
        if fn in active_inc:
            continue
        if not lane.find_fn_span(text, fn):
            continue
        keep.append(fn)
    if not keep:
        return None
    rel = str(src.relative_to(ROOT)).replace("\\", "/")
    return rel, keep[:max_fns], f"{rel}: " + " ".join(f"{fn}={pcts[fn]:.2f}%" for fn in keep[:max_fns])


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default="build/pin_queue.txt")
    ap.add_argument("--max-files", type=int, default=24)
    ap.add_argument("--max-fns", type=int, default=6)
    ap.add_argument("--min-pct", type=float, default=0.0)
    ap.add_argument("--max-pct", type=float, default=99.9999)
    ap.add_argument("--max-report-missing", type=int, default=8)
    ap.add_argument("--sources", nargs="*")
    args = ap.parse_args()

    if args.sources:
        sources = [ROOT / s for s in args.sources]
    else:
        sources = _report_sources(args.max_report_missing)

    equiv = _equiv_fns()
    lines = ["# Live-measured real-C queue. No active .inc wrappers; excludes equivalent.txt."]
    notes = []
    for src in sources:
        if len(lines) - 1 >= args.max_files:
            break
        item = live_line(src, args.max_fns, args.min_pct, args.max_pct, equiv)
        if item is None:
            continue
        rel, fns, note = item
        lines.append(rel + " " + " ".join(fns))
        notes.append(note)

    out = ROOT / args.out
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text("\n".join(lines) + "\n")
    print(f"wrote {len(lines) - 1} queue lines -> {out.relative_to(ROOT)}")
    for note in notes:
        print(note)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
