#!/usr/bin/env python3
"""compare_results.py — Consolidate benchmark JSONs into a leaderboard.

Scans tools/decomp_work/benchmark/results/ for compile_match_* and bestofn_* JSONs
and emits:
  - A per-model summary (PASS count, mean match%, total time, mean match% on extracted-only)
  - A function × model matrix (match% per cell)
  - Top performer per function

Usage:
    python tools/decomp_work/benchmark/compare_results.py
    python tools/decomp_work/benchmark/compare_results.py --suite test_suite_cw_focus_expert
    python tools/decomp_work/benchmark/compare_results.py --kind bestofn
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from collections import defaultdict

RESULTS_DIR = Path(__file__).resolve().parent / "results"


def collect(prefix: str, suite_filter: str | None) -> list[dict]:
    out = []
    for p in sorted(RESULTS_DIR.glob(f"{prefix}__*.json")):
        if suite_filter and suite_filter not in p.stem:
            continue
        try:
            data = json.loads(p.read_text(encoding="utf-8"))
        except Exception:
            continue
        out.append({"path": p, "data": data})
    return out


def summarize_compile_match(rows: list[dict]) -> str:
    out = []
    out.append("# Compile+match leaderboard")
    out.append("")
    out.append("| Model | PASS | mean% | mean% (extracted) | total time |")
    out.append("|---|---|---|---|---|")
    matrix = defaultdict(dict)  # fn -> model -> pct
    for row in rows:
        d = row["data"]
        model = d["model"]
        results = d["results"]
        passes = sum(1 for r in results if r.get("status") == "PASS")
        pcts = [r.get("match_pct", 0.0) for r in results]
        extracted_pcts = [r.get("match_pct", 0.0) for r in results if r.get("extracted")]
        mean_pct = sum(pcts) / len(pcts) if pcts else 0.0
        mean_ex = sum(extracted_pcts) / len(extracted_pcts) if extracted_pcts else 0.0
        out.append(
            f"| `{model}` | {passes}/{len(results)} | {mean_pct:.1f}% | "
            f"{mean_ex:.1f}% | {d.get('total_time', 0):.0f}s |"
        )
        for r in results:
            matrix[r["function"]][model] = r.get("match_pct", 0.0)

    out.append("")
    out.append("# Per-function match% matrix")
    out.append("")
    models = sorted({m for funcs in matrix.values() for m in funcs.keys()})
    header = "| Function | " + " | ".join(models) + " | best |"
    sep = "|---|" + "|".join(["---"] * (len(models) + 1)) + "|"
    out.append(header)
    out.append(sep)
    for fn in sorted(matrix.keys()):
        row = matrix[fn]
        cells = []
        best = max(row.values()) if row else 0.0
        for m in models:
            v = row.get(m)
            cells.append(f"{v:.1f}%" if v is not None else "-")
        winner = ", ".join(m for m in models if row.get(m) == best and best > 0)
        out.append(f"| `{fn}` | " + " | ".join(cells) + f" | {best:.1f}% ({winner}) |")
    return "\n".join(out)


def summarize_bestofn(rows: list[dict]) -> str:
    out = []
    out.append("# Best-of-N leaderboard")
    out.append("")
    out.append("| Model | PASS | mean best% | best stage breakdown | total time |")
    out.append("|---|---|---|---|---|")
    for row in rows:
        d = row["data"]
        model = d["model"]
        results = d["results"]
        passes = sum(1 for r in results if r.get("status") == "PASS")
        pcts = [r.get("best_match_pct", 0.0) for r in results]
        mean_pct = sum(pcts) / len(pcts) if pcts else 0.0
        stages = defaultdict(int)
        for r in results:
            stages[r.get("best_stage") or "none"] += 1
        breakdown = ", ".join(f"{s}={c}" for s, c in sorted(stages.items()))
        out.append(
            f"| `{model}` | {passes}/{len(results)} | {mean_pct:.1f}% | "
            f"{breakdown} | {d.get('total_time', 0):.0f}s |"
        )
    return "\n".join(out)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--suite", default=None, help="Substring to filter result files (e.g. test_suite_cw_focus_expert)")
    ap.add_argument("--kind", choices=["compile_match", "bestofn", "both"], default="both")
    args = ap.parse_args()

    if args.kind in ("compile_match", "both"):
        rows = collect("compile_match", args.suite)
        if rows:
            print(summarize_compile_match(rows))
            print()
    if args.kind in ("bestofn", "both"):
        rows = collect("bestofn_compile_match", args.suite)
        if rows:
            print(summarize_bestofn(rows))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
