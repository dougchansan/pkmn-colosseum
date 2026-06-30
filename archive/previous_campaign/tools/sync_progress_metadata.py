#!/usr/bin/env python3
"""Keep committed progress metadata synchronized.

This repository has two progress consumers:

* ``report.json`` is the decomp.dev/README objdiff report.
* ``tools/decomp_work/progress.json`` is the agent-lane skip list.

The agent progress file must not blindly mirror every 100% report entry because
objdiff can count active asm wrappers and trivial stubs as matched. For campaign
coordination, "completed" means active real C that is byte-exact, so this script
derives ``progress.json.completed`` from the intersection of:

* functions at >=99.995% in ``report.json``
* functions classified as ``REAL_C`` by ``tools/decomp_work/progress2.py``

It also keeps the README headline numbers aligned with ``report.json``.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
REPORT = ROOT / "report.json"
README = ROOT / "README.md"
PROGRESS = ROOT / "tools" / "decomp_work" / "progress.json"
MATCH = 99.995


def load_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def report_measures(report: dict) -> dict:
    return report.get("measures", {})


def fl(m: dict, key: str) -> float:
    try:
        return float(m.get(key, 0) or 0)
    except (TypeError, ValueError):
        return 0.0


def report_matched_names(report: dict) -> set[str]:
    names: set[str] = set()
    for unit in report.get("units", []):
        for fn in unit.get("functions", []):
            if fl(fn, "fuzzy_match_percent") >= MATCH:
                name = fn.get("name")
                if name:
                    names.add(name)
    return names


def real_c_names() -> set[str]:
    sys.path.insert(0, str(ROOT / "tools" / "decomp_work"))
    import progress2  # type: ignore

    _, fn2class = progress2.classify_all()
    return {name for name, cls in fn2class.items() if cls == "REAL_C"}


def unique_list(values) -> list[str]:
    return sorted({str(v) for v in values if str(v).strip()})


def expected_progress(existing: dict, report: dict) -> dict:
    matched = report_matched_names(report)
    real_c = real_c_names()
    completed = sorted(matched & real_c)

    failed = unique_list(existing.get("failed", []))
    in_progress = unique_list(existing.get("in_progress", []))
    completed_set = set(completed)
    failed = [fn for fn in failed if fn not in completed_set]
    in_progress = [fn for fn in in_progress if fn not in completed_set]

    measures = report_measures(report)
    report_hash = hashlib.sha256(REPORT.read_bytes()).hexdigest()
    return {
        "completed": completed,
        "failed": failed,
        "in_progress": in_progress,
        "metadata": {
            "source": "report.json",
            "policy": (
                "completed = active REAL_C functions with "
                "fuzzy_match_percent >= 99.995; asm wrappers/stubs excluded"
            ),
            "report_sha256": report_hash,
            "report_matched_functions": int(fl(measures, "matched_functions")),
            "report_total_functions": int(fl(measures, "total_functions")),
            "report_matched_data": int(fl(measures, "matched_data")),
            "report_total_data": int(fl(measures, "total_data")),
            "report_complete_data": int(fl(measures, "complete_data")),
            "completed_real_c_functions": len(completed),
        },
    }


def expected_readme_text(existing: str, report: dict) -> str:
    measures = report_measures(report)
    fm = int(fl(measures, "matched_functions"))
    ft = int(fl(measures, "total_functions"))
    cm = int(fl(measures, "matched_code"))
    ct = int(fl(measures, "total_code"))
    dm = int(fl(measures, "matched_data"))
    dt = int(fl(measures, "total_data"))
    if not ft or not ct:
        return existing

    fpct = 100.0 * fm / ft
    cpct = 100.0 * cm / ct
    dpct = (100.0 * dm / dt) if dt else 0.0
    out = re.sub(
        r"\| Function match \| [^|]*\|",
        f"| Function match | ~{fpct:.1f}% ({fm:,} / {ft:,} functions) |",
        existing,
    )
    out = re.sub(
        r"\| Code match \| [^|]*\|",
        f"| Code match | ~{cpct:.1f}% ({cm:,} / {ct:,} matched code bytes) |",
        out,
    )
    data_row = f"| Data match | ~{dpct:.1f}% ({dm:,} / {dt:,} matched data bytes) |"
    if re.search(r"\| Data match \| [^|]*\|", out):
        out = re.sub(r"\| Data match \| [^|]*\|", data_row, out)
    elif dt:
        out = re.sub(
            r"(\| Code match \| [^\n]*\n)",
            r"\1" + data_row + "\n",
            out,
            count=1,
        )
    return out


def write_json(path: Path, data: dict) -> None:
    path.write_text(json.dumps(data, indent=2, sort_keys=False) + "\n", encoding="utf-8")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="fail if files are not synchronized")
    ap.add_argument("--sync", action="store_true", help="write synchronized README/progress.json")
    args = ap.parse_args()

    if not REPORT.exists():
        print("sync-progress: report.json missing", file=sys.stderr)
        return 1
    report = load_json(REPORT)

    existing_progress = load_json(PROGRESS) if PROGRESS.exists() else {}
    want_progress = expected_progress(existing_progress, report)
    have_progress = existing_progress

    have_readme = README.read_text(encoding="utf-8") if README.exists() else ""
    want_readme = expected_readme_text(have_readme, report)

    progress_ok = have_progress == want_progress
    readme_ok = have_readme == want_readme

    if args.check:
        if not readme_ok:
            print("sync-progress: README.md is stale relative to report.json", file=sys.stderr)
        if not progress_ok:
            hc = len(have_progress.get("completed", [])) if isinstance(have_progress, dict) else 0
            wc = len(want_progress.get("completed", []))
            print(
                "sync-progress: tools/decomp_work/progress.json is stale "
                f"(completed {hc} -> {wc})",
                file=sys.stderr,
            )
        if readme_ok and progress_ok:
            print(
                "sync-progress: OK "
                f"({len(want_progress['completed'])} real-C completed functions)"
            )
            return 0
        print("sync-progress: run python3 tools/sync_progress_metadata.py --sync", file=sys.stderr)
        return 1

    if not args.sync:
        ap.error("choose --check or --sync")

    changed = []
    if not readme_ok:
        README.write_text(want_readme, encoding="utf-8")
        changed.append("README.md")
    if not progress_ok:
        write_json(PROGRESS, want_progress)
        changed.append("tools/decomp_work/progress.json")

    if changed:
        print(
            "sync-progress: updated "
            + ", ".join(changed)
            + f" ({len(want_progress['completed'])} real-C completed functions)"
        )
    else:
        print(
            "sync-progress: already current "
            f"({len(want_progress['completed'])} real-C completed functions)"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
