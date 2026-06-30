#!/usr/bin/env python3
"""build_prompt.py — Assemble a single prompt file for any model (opencode / ollama / api).

Combines: CW_QUIRKS.md + pattern detector output + existing externs in the target .c file
          + the raw .inc asm. Write it to stdout or a file.

Usage:
    python3 tools/decomp_work/build_prompt.py <stem> <fn>
    # Example:
    python3 tools/decomp_work/build_prompt.py gs_field_world fn_80115C48

Then pipe the output to your model of choice:
    python3 tools/decomp_work/build_prompt.py gs_field_world fn_80115C48 > /tmp/prompt.md
    # Then paste into opencode / codex / etc., or:
    ollama run qwen2.5-coder:32b-instruct-q4_K_M < /tmp/prompt.md
    ollama run deepseek-coder-v2:16b < /tmp/prompt.md
    ollama run gpt-oss:20b < /tmp/prompt.md
"""
from __future__ import annotations

import io
import re
import subprocess
import sys
from pathlib import Path

# Force UTF-8 stdout on Windows (default is cp1252 which chokes on ≥, →, etc.)
if sys.stdout.encoding and sys.stdout.encoding.lower() != "utf-8":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

ROOT = Path(__file__).resolve().parent.parent.parent  # repo root
QUIRKS = ROOT / "tools" / "decomp_work" / "CW_QUIRKS.md"
HELPER = ROOT / "tools" / "decomp_work" / "match_helper.py"


def existing_externs(c_path: Path, calls: list[str], labels: list[str]) -> list[str]:
    """Grep the target .c file for existing decls of referenced symbols."""
    if not c_path.exists():
        return []
    text = c_path.read_text(encoding="latin-1")
    out = []
    for sym in set(calls) | set(labels):
        for line in text.splitlines():
            if sym in line and ("extern" in line or "typedef" in line):
                out.append(f"  {sym}: {line.strip()}")
                break
        else:
            out.append(f"  {sym}: (NOT YET DECLARED — add `extern ...`)")
    return out


def main(argv: list[str]) -> int:
    if len(argv) < 3:
        print(__doc__, file=sys.stderr)
        return 2
    stem = argv[1]
    fn = argv[2]
    if not fn.startswith("fn_"):
        fn = "fn_" + fn

    inc_path = ROOT / "src" / "game" / f"{stem}_{fn}.inc"
    c_path = ROOT / "src" / "game" / f"{stem}.c"

    if not inc_path.exists():
        print(f"ERROR: {inc_path} not found", file=sys.stderr)
        return 1

    # Run match_helper to get pattern analysis
    result = subprocess.run(
        ["python3", str(HELPER), str(inc_path)],
        capture_output=True, text=True
    )
    if result.returncode != 0:
        print(f"match_helper failed: {result.stderr}", file=sys.stderr)
        return 1
    analysis = result.stdout

    # Extract calls and labels from analysis for extern grep
    calls = re.findall(r"`(fn_[0-9A-Fa-f]+)`", analysis)
    labels = re.findall(r"`(lbl_[0-9A-Fa-f]+)`", analysis) + re.findall(r"`(jumptable_[0-9A-Fa-f]+)`", analysis)
    existing = existing_externs(c_path, calls, labels)

    # Read CW_QUIRKS.md
    quirks = QUIRKS.read_text(encoding="utf-8") if QUIRKS.exists() else "(CW_QUIRKS.md missing)"

    out = []
    out.append(f"# Decomp task: match `{fn}` in `src/game/{stem}.c` to byte-identical PowerPC")
    out.append("")
    out.append("## Your job")
    out.append(f"Read the target asm below and write C that, when compiled with CodeWarrior 1.3")
    out.append(f"(flags `-O4,p -fp hard`), produces byte-identical output. Target floor: ≥90% match.")
    out.append("")
    out.append("## Output format (strict)")
    out.append("Respond with ONLY the complete `#else` block content for this function. Start with")
    out.append("`#pragma push` and end with `#pragma pop`. Include the function signature, body, and")
    out.append("any pragmas. Do NOT write commentary, explanations, or markdown outside the block.")
    out.append("Example:")
    out.append("```")
    out.append("#pragma push")
    out.append("#pragma peephole off")
    out.append(f"s32 {fn}(u32 key) {{")
    out.append("    // body")
    out.append("    return 0;")
    out.append("}")
    out.append("#pragma pop")
    out.append("```")
    out.append("")
    out.append("## Existing externs in " + str(c_path.name))
    out.append("**Use these EXACT signatures. Never add a duplicate extern.**")
    out.append("")
    for line in existing:
        out.append(line)
    out.append("")
    out.append("## Pattern analysis + skeleton")
    out.append("")
    out.append(analysis)
    out.append("")
    out.append("## CW 1.3 quirk cheatsheet (MUST follow)")
    out.append("")
    out.append(quirks)

    sys.stdout.write("\n".join(out))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
