#!/usr/bin/env python3
"""Prioritize data worklist chunks by source-recovery risk."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
WORKLIST = ROOT / "tools" / "decomp_work" / "data_sdata2_worklist.json"
OUTPUT = ROOT / "tools" / "decomp_work" / "data_campaign_queue.json"


def classify(chunk: dict) -> tuple[str, int, str]:
    kinds = chunk.get("kind_counts", {}) or {}
    unknown = int(kinds.get("unknown", 0) or 0)
    strings = int(kinds.get("string", 0) or 0)
    gap = int(chunk.get("gap_bytes", 0) or 0)
    size = int(chunk.get("size", 0) or 0)
    numeric = sum(int(kinds.get(k, 0) or 0) for k in ("float", "double", "4byte", "2byte", "byte"))

    score = unknown * 100 + strings * 20 + gap * 5
    if numeric == 0:
        score += 50
    if size > 256:
        score += size - 256

    if unknown:
        lane = "RESEARCH"
        reason = "unknown symbols need type/source attribution"
    elif gap > 16:
        lane = "LAYOUT"
        reason = "padding/alignment must be explained by source layout"
    elif strings:
        lane = "STRING"
        reason = "string literals need source ownership evidence"
    else:
        lane = "NUMERIC"
        reason = "numeric constants only; best first data-matching target"
    return lane, score, reason


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--worklist", default=str(WORKLIST))
    parser.add_argument("--output", default=str(OUTPUT))
    args = parser.parse_args()

    work = json.loads(Path(args.worklist).read_text(encoding="utf-8"))
    queue = []
    for index, chunk in enumerate(work.get("chunks", []) or []):
        lane, score, reason = classify(chunk)
        queue.append(
            {
                "id": chunk.get("id") or f"sdata2-{index:03d}",
                "status": "queued",
                "lane": lane,
                "priority_score": score,
                "reason": reason,
                "section": chunk.get("section"),
                "start": chunk.get("start"),
                "end": chunk.get("end"),
                "size": chunk.get("size"),
                "gap_bytes": chunk.get("gap_bytes"),
                "kind_counts": chunk.get("kind_counts"),
                "target_object": chunk.get("target_object"),
                "symbol_count": len(chunk.get("symbols", []) or []),
                "symbols": chunk.get("symbols"),
            }
        )
    queue.sort(key=lambda item: (item["priority_score"], item["start"]))
    out = {
        "metadata": {
            "source": str(Path(args.worklist).relative_to(ROOT)),
            "section": (work.get("metadata", {}) or {}).get("section"),
            "chunk_count": len(queue),
            "lanes": {
                lane: sum(1 for item in queue if item["lane"] == lane)
                for lane in ("NUMERIC", "STRING", "LAYOUT", "RESEARCH")
            },
            "policy": "source expressions only; no incbin/raw target bytes; verify with tools/verify_data_progress.py before counting",
        },
        "queue": queue,
    }
    Path(args.output).write_text(json.dumps(out, indent=2) + "\n", encoding="utf-8")
    print(
        f"wrote {Path(args.output).relative_to(ROOT)}: "
        + ", ".join(f"{lane}={count}" for lane, count in out["metadata"]["lanes"].items())
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
