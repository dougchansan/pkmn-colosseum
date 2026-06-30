#!/usr/bin/env python3
"""Apply data_progress.json totals to an existing decomp.dev report."""

import argparse
import json
from pathlib import Path


def as_int(value) -> int:
    if value is None:
        return 0
    return int(value)


def load_data_totals(path: Path) -> tuple[int, int]:
    data = json.loads(path.read_text(encoding="utf-8"))
    matched = 0
    complete = 0
    seen = set()
    for item in data.get("matched", []):
        key = (
            item.get("section"),
            item.get("start"),
            item.get("object"),
            item.get("source_path"),
        )
        if key in seen:
            continue
        seen.add(key)
        size = int(item.get("size", 0) or 0)
        if size <= 0:
            continue
        matched += size
        complete += size
    return matched, complete


def load_total_data(path: Path) -> int:
    if not path.exists():
        return 0

    config = json.loads(path.read_text(encoding="utf-8"))
    total = 0
    for unit in config.get("units", []):
        data_size = as_int(unit.get("data_size"))
        if data_size <= 0:
            continue
        name = str(unit.get("name", "")).lower()
        obj = str(unit.get("object", "")).lower()
        if "bss" in name or "bss" in obj:
            continue
        total += data_size
    return total


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("report", nargs="?", default="report.json")
    parser.add_argument(
        "--data-progress",
        default="config/GC6E01/data_progress.json",
    )
    parser.add_argument(
        "--build-config",
        default="build/GC6E01/config.json",
    )
    args = parser.parse_args()

    report_path = Path(args.report)
    data_progress_path = Path(args.data_progress)
    build_config_path = Path(args.build_config)
    report = json.loads(report_path.read_text(encoding="utf-8"))
    measures = report.setdefault("measures", {})

    total_code = as_int(measures.get("total_code"))
    old_total_data = as_int(measures.get("total_data"))
    total_data = load_total_data(build_config_path) or old_total_data
    old_matched_data = as_int(measures.get("matched_data"))
    matched_data, complete_data = load_data_totals(data_progress_path)
    if matched_data > total_data or complete_data > total_data:
        raise SystemExit("data progress exceeds report total_data")

    old_total_weight = total_code + old_total_data
    total_weight = total_code + total_data
    old_fuzzy = float(measures.get("fuzzy_match_percent", 0.0) or 0.0)
    fuzzy_without_data = (old_fuzzy * old_total_weight) - (100.0 * old_matched_data)
    measures["fuzzy_match_percent"] = (
        (fuzzy_without_data + (100.0 * matched_data)) / total_weight
        if total_weight else 0.0
    )
    measures["total_data"] = str(total_data)
    measures["matched_data"] = str(matched_data)
    measures["matched_data_percent"] = (
        (100.0 * matched_data / total_data) if total_data else 0.0
    )
    measures["complete_data"] = str(complete_data)
    measures["complete_data_percent"] = (
        (100.0 * complete_data / total_data) if total_data else 0.0
    )

    report_path.write_text(
        json.dumps(report, separators=(",", ":")) + "\n",
        encoding="utf-8",
    )
    print(
        f"updated {report_path}: data={matched_data}/{total_data}, "
        f"complete_data={complete_data}/{total_data}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
