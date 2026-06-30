#!/usr/bin/env python3
"""
regen_incs.py - Regenerate .inc files for already-wrapped asm functions.

Uses the asm_wrapper_sweep.py logic (symbolic SDA references, @L_ labels, etc.)

Usage:
    python tools/regen_incs.py src/game/gs_thread.c gs_thread_
    python tools/regen_incs.py src/dolphin/pad/Pad.c Pad_
"""
import re
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_ROOT / 'tools'))

from asm_wrapper_sweep import load_asm_index, extract_inc_lines


def main():
    if len(sys.argv) < 3:
        print("Usage: python tools/regen_incs.py <source.c> <prefix_>")
        sys.exit(1)

    src_file = Path(sys.argv[1])
    prefix = sys.argv[2]

    if not src_file.exists():
        src_file = PROJECT_ROOT / sys.argv[1]
    if not src_file.exists():
        print(f"ERROR: Source file not found: {src_file}")
        sys.exit(1)

    src_text = src_file.read_text(encoding='utf-8', errors='replace')

    # Find all asm-wrapped function names
    asm_fns = re.findall(r'asm \w+ (fn_8[0-9A-Fa-f]+)\s*\(', src_text)
    print(f"Found {len(asm_fns)} asm-wrapped functions in {src_file.name}")

    asm_lines, asm_index = load_asm_index()

    src_dir = src_file.parent
    regenerated = 0
    missing = 0

    for fn_name in asm_fns:
        inc_lines = extract_inc_lines(fn_name, asm_lines, asm_index)
        if inc_lines is None:
            print(f"  MISSING in asm: {fn_name}")
            missing += 1
            continue

        inc_path = src_dir / f'{prefix}{fn_name}.inc'
        content = '\n'.join(inc_lines) + '\n'
        inc_path.write_text(content, encoding='utf-8')
        regenerated += 1
        if regenerated % 10 == 0:
            print(f"  Regenerated {regenerated}/{len(asm_fns)}...")

    print(f"\nDone: regenerated {regenerated}, missing from asm: {missing}")


if __name__ == '__main__':
    main()
