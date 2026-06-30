#!/usr/bin/env python3
"""Scan match percentage for selected functions in one source file.

Usage:
  python tools/match_scan_file.py <file_stem_or_path> fn_XXXXX [fn_YYYYY ...]
  python tools/match_scan_file.py src/game/gs_render.c fn_800DF21C --report out.json

The scanner compiles the source file once, then runs non-interactive objdiff
JSON for the requested symbols. This avoids stale object reads and avoids the
interactive objdiff TUI.
"""

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC_DIR = ROOT / "src"
REPORTS_DIR = ROOT / "tools" / "decomp_work" / "reports"

sys.path.insert(0, str(ROOT / "tools"))
import compile_check  # noqa: E402


def resolve_source(name: str) -> Path:
    """Resolve a file stem or source path to a tracked source file."""
    p = Path(name)
    if not p.is_absolute():
        p = ROOT / p
    if p.exists():
        return p

    stem = Path(name).stem
    matches = sorted(SRC_DIR.rglob(f"{stem}.c"))
    if matches:
        return matches[0]

    raise SystemExit(f"ERROR: cannot resolve source file from {name!r}")


def default_report_path(src_path: Path) -> Path:
    rel = src_path.relative_to(ROOT).with_suffix("")
    safe = "_".join(rel.parts)
    return REPORTS_DIR / f"match_scan_{safe}.json"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", help="Source path or file stem")
    parser.add_argument("symbols", nargs="+", help="Function symbols to scan")
    parser.add_argument(
        "--report", nargs="?", const="AUTO",
        help="Write a JSON report. Omit value for tools/decomp_work/reports/."
    )
    parser.add_argument(
        "--whole-file", action="store_true",
        help="Use one full-object objdiff JSON pass instead of per-symbol passes"
    )
    parser.add_argument(
        "--timeout", type=int, default=120,
        help="Objdiff timeout in seconds"
    )
    args = parser.parse_args()

    src_path = resolve_source(args.source)
    result = compile_check.run_diff_symbols(
        src_path, symbols=args.symbols, whole_file=args.whole_file,
        timeout=args.timeout,
    )

    for symbol in args.symbols:
        info = result["symbols"].get(symbol, {})
        if result.get("error"):
            print(f"  ERR {symbol}: {result['error']}")
        elif not info.get("symbol_found"):
            print(f"  NO-MATCH {symbol}")
        else:
            print(f"{info['match_percent']:6.1f}%  {symbol}")

    if args.report:
        report_path = default_report_path(src_path) if args.report == "AUTO" else Path(args.report)
        if not report_path.is_absolute():
            report_path = ROOT / report_path
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(result, indent=2), encoding="utf-8")
        print(f"report: {report_path.relative_to(ROOT)}")

    return 1 if result.get("error") else 0


if __name__ == "__main__":
    raise SystemExit(main())
