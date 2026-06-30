#!/usr/bin/env python3
"""Probe SeedCoder v3 on a source-data matching chunk.

The v3 server is trained for asm->C, so this is deliberately research-only:
it asks for typed source-data declarations, saves the response under build/,
and never edits tracked source.
"""
from __future__ import annotations

import argparse
import json
import os
import time
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
QUEUE = ROOT / "tools" / "decomp_work" / "data_campaign_queue.json"
RESEARCH = ROOT / "build" / "agent_training" / "data_research"
SERVER = os.environ.get("SEED_SERVER", "http://192.168.50.101:8780/gen")


def load_queue() -> list[dict]:
    data = json.loads(QUEUE.read_text(encoding="utf-8"))
    return data.get("queue", []) or []


def choose_chunk(chunks: list[dict], chunk_id: str | None) -> dict:
    if chunk_id:
        for item in chunks:
            if item.get("id") == chunk_id:
                return item
        raise SystemExit(f"chunk not found: {chunk_id}")
    for item in chunks:
        if item.get("lane") == "NUMERIC":
            return item
    if not chunks:
        raise SystemExit("data queue is empty")
    return chunks[0]


def prompt_for(item: dict) -> str:
    symbols = item.get("symbols") or []
    symbol_lines = "\n".join(
        f"- {sym.get('name')} @ {sym.get('addr')} size={sym.get('size')} kind={sym.get('kind')}"
        for sym in symbols[:80]
    )
    note = ""
    for path in sorted(RESEARCH.glob(f"{item['id']}_*.md")):
        if path.name.endswith("_seedv3.md"):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if text.strip():
            if len(text) > 9000:
                text = text[:5200] + "\n\n[...middle clipped...]\n\n" + text[-3600:]
            note += f"\n--- {path.name} ---\n{text}\n"
    if note:
        note = "\nPrior agent research to critique/refine:\n" + note[-9500:]

    return f"""DATA MATCHING TASK, NOT A FUNCTION.

Project: Pokemon Colosseum matching decompilation.
Goal: propose readable, typed C declarations for one source-owned data chunk.

Hard rules:
- Do not use .inc files, incbin, inline asm, raw byte arrays, or pasted ROM bytes.
- Prefer typed C declarations: const f32, const f64, strings, structs, pointer tables.
- If a label cannot be justified from type/reference evidence, mark it blocked.
- Output only proposed source declarations and a short evidence/blocker list.
- This is research only; do not claim a verified match.

Chunk:
- id: {item.get('id')}
- section: {item.get('section')}
- range: {item.get('start')}..{item.get('end')} ({item.get('size')} bytes)
- lane: {item.get('lane')}
- target object: {item.get('target_object')}
- reason: {item.get('reason')}

Labels:
{symbol_lines}
{note}

Existing successful pattern:
- src/crt_data/sdata2_math.c owns contiguous fdlibm-style .sdata2 constants as SDATA2 const f64.
- verified data entries must pass python3 tools/verify_data_progress.py.

Return:
1. Proposed source owner file.
2. Typed declarations for safe labels only.
3. Labels/chunk ranges that should remain blocked.
4. Verification commands.
"""


def request(server: str, prompt: str, n: int, temp: float, max_new: int) -> list[str]:
    body = json.dumps({
        "asm": prompt,
        "fn": "data_chunk",
        "n": n,
        "temp": temp,
        "max_new": max_new,
    }).encode()
    req = urllib.request.Request(server, data=body, headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=500) as resp:
        data = json.loads(resp.read())
    return data.get("candidates", [])


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--chunk-id")
    ap.add_argument("--server", default=SERVER)
    ap.add_argument("--n", type=int, default=1)
    ap.add_argument("--temp", type=float, default=0.25)
    ap.add_argument("--max-new", type=int, default=900)
    args = ap.parse_args()

    item = choose_chunk(load_queue(), args.chunk_id)
    prompt = prompt_for(item)
    RESEARCH.mkdir(parents=True, exist_ok=True)
    out_path = RESEARCH / f"{item['id']}_seedv3.md"

    started = time.time()
    try:
        candidates = request(args.server, prompt, args.n, args.temp, args.max_new)
        ok = True
        error = ""
    except Exception as exc:
        candidates = []
        ok = False
        error = str(exc)

    with out_path.open("w", encoding="utf-8") as fh:
        fh.write(f"# SeedCoder v3 data probe: {item['id']}\n\n")
        fh.write(f"server: `{args.server}`\n\n")
        fh.write("## Prompt\n\n```text\n")
        fh.write(prompt)
        fh.write("\n```\n\n")
        if error:
            fh.write(f"## Error\n\n`{error}`\n")
        for idx, cand in enumerate(candidates):
            fh.write(f"## Candidate {idx}\n\n")
            fh.write(cand.strip())
            fh.write("\n\n")

    summary = {
        "ok": ok,
        "chunk_id": item.get("id"),
        "server": args.server,
        "candidates": len(candidates),
        "seconds": round(time.time() - started, 2),
        "output": str(out_path.relative_to(ROOT)),
        "error": error,
    }
    print("DATA_SEED_SUMMARY " + json.dumps(summary, separators=(",", ":")))
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
