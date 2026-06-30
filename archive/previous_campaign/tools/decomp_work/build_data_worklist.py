#!/usr/bin/env python3
"""Build a source-data matching worklist from symbol metadata."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SYMBOLS = ROOT / "config" / "GC6E01" / "symbols.build.txt"
BUILD_CFG = ROOT / "build" / "GC6E01" / "config.json"
DATA_PROGRESS = ROOT / "config" / "GC6E01" / "data_progress.json"
AUTO_BASE_RE = re.compile(r"auto_\d+_([0-9A-Fa-f]{8})_")
SYMBOL_RE = re.compile(
    r"^(\S+)\s*=\s*(\.\w+):0x([0-9A-Fa-f]+);.*?size:0x([0-9A-Fa-f]+)(.*)$"
)


def parse_int(value, default: int = 0) -> int:
    try:
        return int(value, 0) if isinstance(value, str) else int(value)
    except (TypeError, ValueError):
        return default


def data_kind(tail: str) -> str:
    if "data:" not in tail:
        return "unknown"
    return tail.split("data:", 1)[1].split()[0]


def load_symbols(path: Path) -> list[dict]:
    rows: list[dict] = []
    for line in path.read_text(encoding="utf-8").splitlines():
        match = SYMBOL_RE.match(line.strip())
        if not match:
            continue
        name, section, addr_s, size_s, tail = match.groups()
        size = int(size_s, 16)
        if size <= 0:
            continue
        rows.append(
            {
                "name": name,
                "section": section,
                "addr": int(addr_s, 16),
                "size": size,
                "kind": data_kind(tail),
                "local": "scope:local" in tail,
            }
        )
    return sorted(rows, key=lambda row: (row["section"], row["addr"], row["name"]))


def load_section_units(path: Path) -> dict[str, list[dict]]:
    if not path.exists():
        return {}
    cfg = json.loads(path.read_text(encoding="utf-8"))
    units: dict[str, list[dict]] = {}
    for unit in cfg.get("units", []) or []:
        obj = str(unit.get("object") or "")
        match = AUTO_BASE_RE.search(Path(obj).name)
        if not match:
            continue
        section = "." + Path(obj).stem.rsplit("_", 1)[-1]
        if section in {".bss", ".sbss", ".sbss2"}:
            continue
        start = int(match.group(1), 16)
        size = parse_int(unit.get("data_size"))
        units.setdefault(section, []).append(
            {
                "name": unit.get("name"),
                "object": obj,
                "start": start,
                "end": start + size,
                "size": size,
            }
        )
    for section_units in units.values():
        section_units.sort(key=lambda row: row["start"])
    return units


def load_matched(path: Path) -> list[tuple[str, int, int]]:
    if not path.exists():
        return []
    data = json.loads(path.read_text(encoding="utf-8"))
    ranges = []
    for item in data.get("matched", []) or []:
        section = str(item.get("section") or "")
        start = parse_int(item.get("start"))
        size = parse_int(item.get("size"))
        if section and start and size > 0:
            ranges.append((section, start, start + size))
    return ranges


def is_matched(section: str, start: int, end: int, matched: list[tuple[str, int, int]]) -> bool:
    for m_section, m_start, m_end in matched:
        if section == m_section and m_start <= start and end <= m_end:
            return True
    return False


def make_chunks(
    symbols: list[dict],
    *,
    section: str,
    units: list[dict],
    matched: list[tuple[str, int, int]],
    max_bytes: int,
) -> list[dict]:
    rows = [row for row in symbols if row["section"] == section]
    chunks: list[dict] = []
    idx = 0
    chunk_id = 0
    while idx < len(rows):
        start = rows[idx]["addr"]
        chunk_symbols = []
        symbol_bytes = 0
        end = start
        while idx < len(rows):
            row = rows[idx]
            row_end = row["addr"] + row["size"]
            proposed_end = max(end, row_end)
            if chunk_symbols and proposed_end - start > max_bytes:
                break
            chunk_symbols.append(row)
            symbol_bytes += row["size"]
            end = proposed_end
            idx += 1
        current_id = chunk_id
        chunk_id += 1
        if is_matched(section, start, end, matched):
            continue
        target_unit = next(
            (
                unit
                for unit in units
                if unit["start"] <= start and end <= unit["end"]
            ),
            None,
        )
        kinds = Counter(row["kind"] for row in chunk_symbols)
        chunks.append(
            {
                "id": f"{section.lstrip('.')}-{current_id:03d}",
                "section": section,
                "start": f"0x{start:08X}",
                "end": f"0x{end:08X}",
                "size": end - start,
                "symbol_bytes": symbol_bytes,
                "gap_bytes": (end - start) - symbol_bytes,
                "target_object": target_unit.get("object") if target_unit else None,
                "kind_counts": dict(sorted(kinds.items())),
                "symbols": [
                    {
                        "name": row["name"],
                        "addr": f"0x{row['addr']:08X}",
                        "size": row["size"],
                        "kind": row["kind"],
                    }
                    for row in chunk_symbols
                ],
            }
        )
    return chunks


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--section", default=".sdata2")
    parser.add_argument("--chunk-bytes", type=int, default=256)
    parser.add_argument("--symbols", default=str(SYMBOLS))
    parser.add_argument("--build-config", default=str(BUILD_CFG))
    parser.add_argument("--data-progress", default=str(DATA_PROGRESS))
    parser.add_argument("--output")
    args = parser.parse_args()

    symbols = load_symbols(Path(args.symbols))
    units = load_section_units(Path(args.build_config))
    matched = load_matched(Path(args.data_progress))
    section_units = units.get(args.section, [])
    section_symbols = [row for row in symbols if row["section"] == args.section]
    chunks = make_chunks(
        symbols,
        section=args.section,
        units=section_units,
        matched=matched,
        max_bytes=args.chunk_bytes,
    )
    kind_counts = Counter(row["kind"] for row in section_symbols)
    symbol_bytes = sum(row["size"] for row in section_symbols)
    section_start = min(
        [row["addr"] for row in section_symbols]
        + [unit["start"] for unit in section_units],
        default=None,
    )
    section_end = max(
        [row["addr"] + row["size"] for row in section_symbols]
        + [unit["end"] for unit in section_units],
        default=None,
    )
    section_size = (section_end - section_start) if section_start is not None and section_end is not None else symbol_bytes
    result = {
        "metadata": {
            "source": str(Path(args.symbols).relative_to(ROOT)),
            "section": args.section,
            "target_objects": [unit["object"] for unit in section_units],
            "section_start": f"0x{section_start:08X}" if section_start is not None else None,
            "section_end": f"0x{section_end:08X}" if section_end is not None else None,
            "section_size": section_size,
            "symbol_count": len(section_symbols),
            "symbol_bytes": symbol_bytes,
            "unattributed_or_padding_bytes": section_size - symbol_bytes,
            "kind_counts": dict(sorted(kind_counts.items())),
            "chunk_bytes": args.chunk_bytes,
            "chunk_count": len(chunks),
        },
        "chunks": chunks,
    }

    text = json.dumps(result, indent=2) + "\n"
    if args.output:
        Path(args.output).write_text(text, encoding="utf-8")
    else:
        print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
