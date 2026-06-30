#!/usr/bin/env python3
"""prepare_dataset.py — Build (prompt, ground_truth_C) pairs from matched functions.

Scans all src/game/*.c files for active C bodies (inside #if 0 / #else blocks
that replaced asm wrappers) that match ≥95% against target. Emits a JSONL
dataset suitable for RL or SFT training.

Output format (one JSON per line):
    {
      "fn": "fn_80115C48",
      "stem": "gs_field_world",
      "match_pct": 100.0,
      "prompt": "<build_prompt output>",
      "completion": "<the #else block content, just the C>",
      "asm": "<raw .inc contents>"
    }

Usage:
    python3 tools/decomp_work/rl/prepare_dataset.py > /storage/finetune/rl/dataset.jsonl

The output becomes the SFT warm-start for the RL loop. Each matched pair is
a known-good example the model must reproduce, then RL explores variants.
"""
from __future__ import annotations

import io
import json
import os
import re
import subprocess
import sys
from pathlib import Path

if sys.stdout.encoding and sys.stdout.encoding.lower() != "utf-8":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

ROOT = Path(__file__).resolve().parent.parent.parent.parent
BUILD_PROMPT = ROOT / "tools" / "decomp_work" / "build_prompt.py"
MATCH_SCAN = ROOT / "tools" / "match_scan_file.py"


def extract_else_block(text: str, fn: str) -> str | None:
    """Find the #else block content for a given fn_XXXXXXXX."""
    # Match: `#if 0\n asm void fn_XXX ... #else\n <CONTENT> \n #endif`
    pattern = re.compile(
        r"#if 0\s*\nasm void " + re.escape(fn) + r"\(void\)\s*\{[^}]*\}\s*\n#else\n(.*?)#endif",
        re.S
    )
    m = pattern.search(text)
    if m:
        body = m.group(1).rstrip()
        # Skip TODO stubs
        if "/* TODO" in body or len(body.strip()) < 20:
            return None
        return body
    return None


def list_matched(stem: str) -> list[tuple[str, float]]:
    """Return [(fn_name, match_pct)] for all C-active functions in the file."""
    c_path = ROOT / "src" / "game" / f"{stem}.c"
    if not c_path.exists():
        return []
    text = c_path.read_text(encoding="latin-1")
    # Find all functions where #if 0 asm is followed by a real C body
    fns = re.findall(r"#if 0\s*\nasm void (fn_[0-9A-Fa-f]+)\(void\)", text)
    if not fns:
        return []
    # Measure match%
    result = subprocess.run(
        ["python3", str(MATCH_SCAN), stem] + fns,
        capture_output=True, text=True, cwd=ROOT
    )
    out: list[tuple[str, float]] = []
    for line in result.stdout.splitlines():
        m = re.match(r"\s*([\d.]+)%\s+(fn_[0-9A-Fa-f]+)", line)
        if m:
            out.append((m.group(2), float(m.group(1))))
    return out


def build_prompt(stem: str, fn: str) -> str:
    result = subprocess.run(
        ["python3", str(BUILD_PROMPT), stem, fn],
        capture_output=True, text=True, cwd=ROOT
    )
    return result.stdout


def main() -> int:
    stems = sorted({
        f.stem for f in (ROOT / "src" / "game").glob("*.c")
        if f.stem not in {"__pycache__"}
    })
    count = 0
    for stem in stems:
        matched = list_matched(stem)
        if not matched:
            continue
        c_text = (ROOT / "src" / "game" / f"{stem}.c").read_text(encoding="latin-1")
        for fn, pct in matched:
            if pct < 95.0:
                continue
            body = extract_else_block(c_text, fn)
            if not body:
                continue
            prompt = build_prompt(stem, fn)
            inc_path = ROOT / "src" / "game" / f"{stem}_{fn}.inc"
            asm = inc_path.read_text(encoding="latin-1") if inc_path.exists() else ""
            record = {
                "fn": fn,
                "stem": stem,
                "match_pct": pct,
                "prompt": prompt,
                "completion": body.strip(),
                "asm": asm,
            }
            print(json.dumps(record), flush=True)
            count += 1
            print(f"  [{count:3d}] {stem}  {fn}  {pct:.1f}%", file=sys.stderr, flush=True)
    print(f"Emitted {count} training pairs", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
