#!/usr/bin/env python3
"""build_suite_from_source.py — Build a benchmark suite from any src/game/<file>.c.

Scans the .c file for `#if 1\nasm void fn_XXXXXXXX(void) { #include "<inc>"; }\n#else\n...\n#endif`
patterns (i.e. functions still in asm-only state), extracts the corresponding raw asm,
and emits a test_suite_*.json compatible with make_expert_suite.py + bench_compile_match.py.

Usage:
    python tools/decomp_work/benchmark/build_suite_from_source.py \
        --source src/game/gs_title.c \
        --out tools/decomp_work/benchmark/test_suite_gs_title.json

Pipe through make_expert_suite.py afterwards to add the rich expert prompt:
    python tools/decomp_work/benchmark/make_expert_suite.py \
        --in  tools/decomp_work/benchmark/test_suite_gs_title.json \
        --out tools/decomp_work/benchmark/test_suite_gs_title_expert.json
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]


# Per-test compact prompt template (lifted from test_suite_cw_focus_compact.json
# entries — same structure so the suite is interchangeable).
COMPACT_PROMPT_TEMPLATE = """You are decompiling a GameCube (PowerPC) function for Pokemon Colosseum.
Convert the following PPC assembly into byte-matching C89 code.

CRITICAL RULES:
- C89 only: ALL declarations before statements in each block
- Use block scoping {{ }} when the asm loads the SAME global from r13 twice
- NEVER use float literals (0.0f) — use `extern f32 lbl_XXXXXXXX;`
- SDA_BASE = 0x80480820 (r13), SDA2_BASE = 0x804836A0 (r2)
- Leaf functions (nofralloc, no bl) need NO pragmas — default O4
- Thunks (stwu/mflr/bl/epilogue) = `extern void target(); void fn() {{ target(); }}`
- `subf rD, rA, rB` = rB - rA (reversed!)
- `clrrwi rD, rS, N` = rS & ~((1<<N)-1)
- Output ONLY the C function + needed externs. No asm, no #if blocks, no explanation.

FUNCTION: {fn}

ASSEMBLY:
```
{asm}
```

CONTEXT:
```c
{context}
```

Write the C89 replacement function."""


def find_asm_blocks(c_text: str) -> list[tuple[str, str, int]]:
    """Find every still-asm-wrapped function. Returns [(fn, inc_path, char_offset), ...]."""
    pattern = re.compile(
        r"#if\s+1\s*\nasm void (fn_[0-9A-Fa-f]{8})\(void\) \{\s*\n"
        r'#include "([^"]+)"\s*\n\}',
        re.MULTILINE,
    )
    return [(m.group(1), m.group(2), m.start()) for m in pattern.finditer(c_text)]


def slice_context(c_text: str, offset: int, lines_before: int = 8, lines_after: int = 8) -> str:
    """Return ~16 lines of source around the offset."""
    line_start = c_text.rfind("\n", 0, offset)
    if line_start < 0:
        line_start = 0
    # Walk back N lines
    pos = line_start
    for _ in range(lines_before):
        prev = c_text.rfind("\n", 0, pos)
        if prev < 0:
            pos = 0
            break
        pos = prev
    start = pos + 1 if pos > 0 else 0
    # Walk forward N lines
    pos = line_start
    for _ in range(lines_after + 4):  # show through #endif and a bit more
        nxt = c_text.find("\n", pos + 1)
        if nxt < 0:
            pos = len(c_text)
            break
        pos = nxt
    end = pos
    return c_text[start:end]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--source", required=True, help="Path to src/game/X.c file")
    ap.add_argument("--out", required=True, help="Path to write the suite JSON")
    args = ap.parse_args()

    src_path = Path(args.source).resolve()
    c_text = src_path.read_text(encoding="latin-1")
    blocks = find_asm_blocks(c_text)
    print(f"{src_path.name}: found {len(blocks)} still-asm-wrapped functions")

    suite = []
    rel_source = src_path.relative_to(REPO).as_posix()
    for fn, inc_rel, offset in blocks:
        inc_path = REPO / inc_rel
        if not inc_path.exists():
            print(f"  WARN: missing inc {inc_rel} for {fn}, skipping")
            continue
        asm = inc_path.read_text(encoding="latin-1")
        context = slice_context(c_text, offset, 6, 6)
        suite.append({
            "function": fn,
            "file": rel_source,
            "inc_file": inc_rel,
            "asm": asm,
            "correct_c": "",  # unknown — reward function will measure against the .inc
            "prompt": COMPACT_PROMPT_TEMPLATE.format(fn=fn, asm=asm.strip(), context=context),
            "asm_lines": sum(1 for ln in asm.splitlines() if ln.strip() and not ln.strip().startswith("/*")),
        })

    Path(args.out).write_text(json.dumps(suite, indent=2), encoding="utf-8")
    print(f"wrote {args.out}: {len(suite)} tests")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
