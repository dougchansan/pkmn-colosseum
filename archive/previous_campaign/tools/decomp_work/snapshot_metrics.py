#!/usr/bin/env python3
"""Snapshot public/debt/data/fleet metrics for day-by-day tracking."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
REPORT = ROOT / "report.json"
DEBT = ROOT / "build" / "real_c_debt_audit.json"
DATA_PROGRESS = ROOT / "config" / "GC6E01" / "data_progress.json"
DATA_WORKLIST = ROOT / "tools" / "decomp_work" / "data_sdata2_worklist.json"
DATA_QUEUE = ROOT / "tools" / "decomp_work" / "data_campaign_queue.json"
HISTORY = ROOT / "tools" / "decomp_work" / "metrics_history.jsonl"
PERMUTER = ROOT / "build" / "permuter_status.json"
WINS = ROOT / "build" / "band_wins"
META_KEYS = {"_src", "_srcs", "_pct"}


def load_json(path: Path, default):
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception:
        return default


def sh(args: list[str]) -> str:
    try:
        return subprocess.run(
            args,
            cwd=ROOT,
            text=True,
            capture_output=True,
            timeout=5,
        ).stdout.strip()
    except Exception:
        return ""


def fl(mapping: dict, key: str) -> float:
    try:
        return float(mapping.get(key, 0) or 0)
    except (TypeError, ValueError):
        return 0.0


def pct(done: float, total: float) -> float:
    return (100.0 * done / total) if total else 0.0


def report_metrics() -> dict:
    report = load_json(REPORT, {})
    m = report.get("measures", {}) or {}
    return {
        "matched_functions": int(fl(m, "matched_functions")),
        "total_functions": int(fl(m, "total_functions")),
        "matched_functions_percent": fl(m, "matched_functions_percent") or pct(fl(m, "matched_functions"), fl(m, "total_functions")),
        "matched_code": int(fl(m, "matched_code")),
        "total_code": int(fl(m, "total_code")),
        "matched_code_percent": fl(m, "matched_code_percent") or pct(fl(m, "matched_code"), fl(m, "total_code")),
        "complete_code": int(fl(m, "complete_code")),
        "complete_code_percent": fl(m, "complete_code_percent") or pct(fl(m, "complete_code"), fl(m, "total_code")),
        "matched_data": int(fl(m, "matched_data")),
        "total_data": int(fl(m, "total_data")),
        "matched_data_percent": fl(m, "matched_data_percent") or pct(fl(m, "matched_data"), fl(m, "total_data")),
        "complete_data": int(fl(m, "complete_data")),
        "complete_data_percent": fl(m, "complete_data_percent") or pct(fl(m, "complete_data"), fl(m, "total_data")),
        "fuzzy_match_percent": fl(m, "fuzzy_match_percent"),
        "complete_units": int(fl(m, "complete_units")),
        "total_units": int(fl(m, "total_units")),
        "complete_units_percent": pct(fl(m, "complete_units"), fl(m, "total_units")),
    }


def source_debt_metrics() -> dict:
    debt = load_json(DEBT, {})
    totals = debt.get("totals", {}) or {}
    keys = [
        "source_functions",
        "real_c_functions",
        "asm_wrapper_functions",
        "stub_functions",
        "inc_include_lines",
        "inline_asm_token_lines",
        "raw_pointer_offset_lines",
        "register_named_lines",
        "fn_named_definitions",
        "fn_named_references",
        "files_with_inc",
        "files_with_asm_wrappers",
        "files_with_pointer_offsets",
    ]
    out = {key: int(totals.get(key, 0) or 0) for key in keys}
    out["real_c_percent"] = pct(out["real_c_functions"], out["source_functions"])
    out["generated_at"] = debt.get("generated_at")
    return out


def ranges_from_data_progress() -> list[tuple[str, int, int]]:
    progress = load_json(DATA_PROGRESS, {})
    ranges = []
    for item in progress.get("matched", []) or []:
        try:
            section = str(item.get("section") or "")
            start = int(item.get("start"), 0)
            size = int(item.get("size") or 0)
        except (TypeError, ValueError):
            continue
        if section and size > 0:
            ranges.append((section, start, start + size))
    return ranges


def covered(section: str, start_s: str, end_s: str, ranges: list[tuple[str, int, int]]) -> bool:
    try:
        start = int(start_s, 0)
        end = int(end_s, 0)
    except (TypeError, ValueError):
        return False
    return any(m_section == section and m_start <= start and end <= m_end for m_section, m_start, m_end in ranges)


def data_campaign_metrics(report: dict) -> dict:
    progress = load_json(DATA_PROGRESS, {})
    work = load_json(DATA_WORKLIST, {})
    queue = load_json(DATA_QUEUE, {})
    ranges = ranges_from_data_progress()
    chunks = work.get("chunks", []) or []
    section = (work.get("metadata", {}) or {}).get("section", ".sdata2")
    done_chunks = sum(1 for chunk in chunks if covered(section, chunk.get("start"), chunk.get("end"), ranges))
    meta = work.get("metadata", {}) or {}
    chunk_bytes = int(meta.get("chunk_bytes") or 256)
    done_chunks += sum(
        ((end - start + chunk_bytes - 1) // chunk_bytes)
        for m_section, start, end in ranges
        if m_section == section
    )
    remaining_chunks = int(meta.get("chunk_count") or len(chunks))
    lanes = (queue.get("metadata", {}) or {}).get("lanes", {}) or {}
    return {
        "verified_entries": len(progress.get("matched", []) or []),
        "verified_bytes": report.get("matched_data", 0),
        "total_bytes": report.get("total_data", 0),
        "verified_percent": report.get("matched_data_percent", 0.0),
        "sdata2_chunks_done": done_chunks,
        "sdata2_chunks_total": remaining_chunks + done_chunks,
        "sdata2_bytes": int(meta.get("section_size") or 0),
        "sdata2_symbols": int(meta.get("symbol_count") or 0),
        "sdata2_padding": int(meta.get("unattributed_or_padding_bytes") or 0),
        "queue_numeric": int(lanes.get("NUMERIC") or 0),
        "queue_string": int(lanes.get("STRING") or 0),
        "queue_layout": int(lanes.get("LAYOUT") or 0),
        "queue_research": int(lanes.get("RESEARCH") or 0),
    }


def band_win_metrics() -> dict:
    entries = []
    if WINS.is_dir():
        for path in WINS.glob("*.json"):
            data = load_json(path, {})
            entries.extend(k for k in data if k not in META_KEYS)
    unique = sorted(set(entries))
    return {
        "band_win_entries": len(entries),
        "band_win_unique": len(unique),
        "band_win_duplicates": max(0, len(entries) - len(unique)),
    }


def fleet_metrics() -> dict:
    perm = load_json(PERMUTER, {})
    out = {
        "permuter_alive": bool(perm.get("alive")),
        "permuter_workers": int(perm.get("workers") or 0),
        "permuter_active": int(perm.get("active") or 0),
        "permuter_queued": int(perm.get("queued") or 0),
        "permuter_wins": int(perm.get("wins") or 0),
    }
    out.update(band_win_metrics())
    return out


def make_snapshot() -> dict:
    now = time.time()
    report = report_metrics()
    return {
        "date": time.strftime("%Y-%m-%d", time.localtime(now)),
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S%z", time.localtime(now)),
        "git": {
            "branch": sh(["git", "branch", "--show-current"]),
            "commit": sh(["git", "rev-parse", "--short", "HEAD"]),
            "dirty": bool(sh(["git", "status", "--short"])),
        },
        "report": report,
        "source_debt": source_debt_metrics(),
        "data_campaign": data_campaign_metrics(report),
        "fleet": fleet_metrics(),
    }


def read_history(path: Path) -> list[dict]:
    if not path.exists():
        return []
    rows = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        try:
            rows.append(json.loads(line))
        except ValueError:
            pass
    return rows


def write_history(path: Path, rows: list[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        "".join(json.dumps(row, sort_keys=True, separators=(",", ":")) + "\n" for row in rows),
        encoding="utf-8",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--history", default=str(HISTORY))
    parser.add_argument("--append", action="store_true", help="append even if today's date already exists")
    parser.add_argument("--print", action="store_true", help="print the snapshot JSON")
    args = parser.parse_args()

    path = Path(args.history)
    snapshot = make_snapshot()
    rows = read_history(path)
    if args.append:
        rows.append(snapshot)
    else:
        rows = [row for row in rows if row.get("date") != snapshot["date"]]
        rows.append(snapshot)
        rows.sort(key=lambda row: (row.get("date", ""), row.get("timestamp", "")))
    write_history(path, rows)
    if args.print:
        print(json.dumps(snapshot, indent=2, sort_keys=True))
    else:
        r = snapshot["report"]
        d = snapshot["data_campaign"]
        print(
            f"snapshot {snapshot['date']}: "
            f"fn={r['matched_functions']}/{r['total_functions']} "
            f"code={r['matched_code']}/{r['total_code']} "
            f"data={d['verified_bytes']}/{d['total_bytes']} "
            f"sdata2_chunks={d['sdata2_chunks_done']}/{d['sdata2_chunks_total']}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
