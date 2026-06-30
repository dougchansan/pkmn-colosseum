#!/usr/bin/env python3
"""Compare symbol order between our compiled .o and the target .o.

A function can match 100% on bytes but still break linking if its data/bss
symbols land in the wrong order. objdiff-cli only checks .text contents, so
weak ordering bugs slip through. This script catches them by listing the
symbols in declaration order from both objects and diffing.

Inspired by zeldaret/tp tools/utilities/weak_order_diff.py, but reworked to
use objdiff-cli's JSON output instead of binutils readelf (we don't ship
binutils in our toolchain).

Usage:
    python3 tools/weak_order_diff.py src/game/gs_title.c
    python3 tools/weak_order_diff.py src/game/gs_title.c --section data
    python3 tools/weak_order_diff.py src/game/gs_title.c --section text  # default

Exits 1 if symbol order differs, 0 if it matches.
"""

import os
import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
sys.path.insert(0, str(ROOT / "tools"))
from headless_subprocess import run as run_tool  # noqa: E402


def src_to_obj(src: str) -> Path:
    """src/game/gs_title.c -> build/GC6E01/base/game/gs_title.o"""
    p = Path(src).resolve()
    rel = p.relative_to(ROOT / "src").with_suffix(".o")
    return ROOT / "build" / "GC6E01" / "base" / rel


def dump_symbols(target_o: Path, base_o: Path) -> dict:
    out = run_tool(
        [
            str(OBJDIFF), "diff",
            "-1", str(target_o),
            "-2", str(base_o),
            "-o", "-",
            "--format", "json",
            "-c", "ppc.calculatePoolRelocations=false",
        ],
        capture_output=True, text=True, check=True,
    )
    return json.loads(out.stdout)


def extract_order(side: dict, section_filter: str | None) -> list[str]:
    """Return symbol names in the order they appear in the object."""
    syms = []
    for s in side.get("symbols", []):
        name = s.get("name", "")
        kind = s.get("kind", "")
        sec = s.get("section", "")
        if not name or name.startswith("@"):
            continue
        if section_filter == "text" and kind != "SYMBOL_FUNCTION":
            continue
        if section_filter == "data" and kind == "SYMBOL_FUNCTION":
            continue
        if section_filter and sec and section_filter not in sec.lower():
            continue
        syms.append(name)
    return syms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src", help="Source .c file (resolves to its built .o)")
    ap.add_argument("--section", default="text", choices=["text", "data", "all"],
                    help="Which symbol class to compare (default: text)")
    ap.add_argument("--target", default=str(TARGET_O), help="Target .o path")
    args = ap.parse_args()

    base_o = src_to_obj(args.src)
    target_o = Path(args.target)
    if not base_o.exists():
        sys.exit(f"base .o missing: {base_o} — run compile_check.py first")
    if not target_o.exists():
        sys.exit(f"target .o missing: {target_o}")

    section = None if args.section == "all" else args.section
    diff = dump_symbols(target_o, base_o)
    left = extract_order(diff.get("left", {}), section)
    right = extract_order(diff.get("right", {}), section)

    # Only compare symbols present on both sides (target may carry many more).
    target_set = set(left)
    right_pruned = [s for s in right if s in target_set]
    left_pruned = [s for s in left if s in set(right)]

    if left_pruned == right_pruned:
        print(f"OK: {len(right_pruned)} {args.section} symbols match target order")
        return 0

    # Show first diverging position.
    print(f"MISMATCH in {args.section} symbol order:")
    print(f"  target ({target_o.name}): {len(left_pruned)} symbols")
    print(f"  ours   ({base_o.name}): {len(right_pruned)} symbols")
    for i, (a, b) in enumerate(zip(left_pruned, right_pruned)):
        if a != b:
            print(f"  first divergence at index {i}:")
            print(f"    target: {a}")
            print(f"    ours:   {b}")
            # Show a few neighbors for context
            lo, hi = max(0, i - 2), min(len(right_pruned), i + 4)
            print(f"  context (target | ours):")
            for j in range(lo, hi):
                la = left_pruned[j] if j < len(left_pruned) else "<end>"
                rb = right_pruned[j] if j < len(right_pruned) else "<end>"
                marker = " <--" if j == i else ""
                print(f"    [{j}] {la:<40s} | {rb}{marker}")
            return 1
    # Length mismatch
    if len(left_pruned) != len(right_pruned):
        print(f"  length mismatch: target {len(left_pruned)} vs ours {len(right_pruned)}")
        return 1
    return 1


if __name__ == "__main__":
    sys.exit(main())
