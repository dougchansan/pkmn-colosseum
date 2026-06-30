#!/usr/bin/env python3
"""status_truth.py - THE ONE authoritative match number for this decomp.

This is the ONLY metric anyone should quote. Three numbers have historically
floated around this project and disagreed with each other:

  * the real objdiff measurement (what this tool prints),
  * an inflated "dashboard" figure (~77.8%) that nobody can reproduce, and
  * a stale README figure (46.5%) that was never updated.

If you are about to write a percentage into a README, a PR description, a chat
message, or a status doc: run THIS, quote THIS, link THIS. Anything else is
unverified and must not be trusted.

How it stays honest: it reuses the exact same objdiff path as
``tools/progress.py`` (which goes through ``tools/measure_cache.py`` ->
``objdiff-cli.exe`` against the unified target object). objdiff is ground
truth; it compares produced bytes against the target, so it cannot be inflated
by editing source files, comments, or dashboards. The only way to move this
number is to actually produce matching object code.

Usage:
    python tools/status_truth.py            # print THE number, write status_truth.json
    python tools/status_truth.py --quiet    # write JSON only, minimal stdout
    python tools/status_truth.py --json PATH # also write a copy to PATH

Output JSON (build/status_truth.json):
    {
      "schema": "pkmn-colosseum/status_truth/v1",
      "measured_at": "2026-05-28T12:34:56",
      "source": "objdiff-cli via tools/measure_cache.py (ground truth)",
      "match_pct": 52.38,                  # functions matched / total
      "bytes_pct": 48.38,                  # bytes matched / total
      "matched_functions": 4341,
      "total_functions": 8287,
      "matched_bytes": 753612,
      "total_bytes": 1557676
    }

Exit codes:
    0  measured successfully
    2  build artifacts missing (actionable message printed, NOT a crash)
"""

import argparse
import datetime
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

OUT_JSON = ROOT / "build" / "status_truth.json"


def _build_hint(target_o: Path, base_dir: Path) -> str:
    """Return an actionable message about what is missing and how to fix it."""
    missing = []
    if not target_o.exists():
        missing.append(f"  - target object: {target_o}")
    if not base_dir.exists():
        missing.append(f"  - base build dir: {base_dir}")
    elif not any(base_dir.rglob("*.o")):
        missing.append(f"  - no compiled .o files under: {base_dir}")
    return (
        "Cannot measure status: build artifacts are missing.\n"
        + "\n".join(missing)
        + "\n\nThis tool only reads existing build output; it does not build.\n"
        "Build the project first (produce the target + base objects), then\n"
        "re-run:  python tools/status_truth.py\n"
        "If the build lives elsewhere, run tools/progress.py the same way you\n"
        "normally do, then re-run this."
    )


def measure():
    """Compute the authoritative numbers via progress.py's objdiff path.

    Returns the overall dict, or None if build artifacts are missing.
    Reuses progress.collect()/summarize() verbatim so this number can never
    drift from `python tools/progress.py`.
    """
    import progress  # reuse the exact same objdiff path; never reimplement it

    if not progress.TARGET_O.exists() or not progress.BASE_DIR.exists():
        return None
    if not any(progress.BASE_DIR.rglob("*.o")):
        return None

    files = progress.collect()
    if not files:
        return None
    overall = progress.summarize(files)
    try:
        import measure_cache
        measure_cache.flush()
    except Exception:
        pass
    return overall


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--quiet", action="store_true",
                    help="Write JSON only; minimal stdout")
    ap.add_argument("--json", metavar="PATH",
                    help="Also write a copy of the report to PATH")
    args = ap.parse_args()

    import progress

    overall = measure()
    if overall is None:
        print(_build_hint(progress.TARGET_O, progress.BASE_DIR), file=sys.stderr)
        return 2

    report = {
        "schema": "pkmn-colosseum/status_truth/v1",
        "measured_at": datetime.datetime.now().isoformat(timespec="seconds"),
        "source": "objdiff-cli via tools/measure_cache.py (ground truth)",
        "match_pct": round(overall["match_pct"], 2),
        "bytes_pct": round(overall["bytes_pct"], 2),
        "matched_functions": overall["matched_functions"],
        "total_functions": overall["total_functions"],
        "matched_bytes": overall["matched_bytes"],
        "total_bytes": overall["total_bytes"],
    }

    OUT_JSON.parent.mkdir(parents=True, exist_ok=True)
    OUT_JSON.write_text(json.dumps(report, indent=2), encoding="utf-8")
    if args.json:
        extra = Path(args.json)
        if not extra.is_absolute():
            extra = ROOT / extra
        extra.parent.mkdir(parents=True, exist_ok=True)
        extra.write_text(json.dumps(report, indent=2), encoding="utf-8")

    if not args.quiet:
        print("=" * 64)
        print("AUTHORITATIVE DECOMP STATUS  (the only number to quote)")
        print("=" * 64)
        print(f"  Functions: {report['matched_functions']:,}/"
              f"{report['total_functions']:,}  ({report['match_pct']:.2f}%)")
        print(f"  Bytes:     {report['matched_bytes']:,}/"
              f"{report['total_bytes']:,}  ({report['bytes_pct']:.2f}%)")
        print(f"  Measured:  {report['measured_at']}")
        print(f"  Source:    {report['source']}")
        print(f"  Written:   {OUT_JSON}")
        print("=" * 64)
        print("Do NOT quote dashboard or README numbers. Quote THIS.")
    else:
        print(f"{report['match_pct']:.2f}% functions / "
              f"{report['bytes_pct']:.2f}% bytes  -> {OUT_JSON}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
