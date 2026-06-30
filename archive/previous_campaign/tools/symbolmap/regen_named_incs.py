#!/usr/bin/env python3
"""regen_named_incs.py - regenerate .inc bodies for asm-wrappers of ANY name.

The stock tools (convert_to_asm_wrappers.py, regen_incs.py) only recognise
`fn_8XXXXXXX` wrappers, so once a function is renamed to its real name they can
no longer regenerate its `.inc`. This tool matches `asm <type> <NAME>(...)` for
any identifier and extracts the body from the (renamed) dtk asm by that name -
so objdiff works on renamed TUs without committing the gitignored `.inc`.

Run after `ninja` has produced build/GC6E01/asm (the symbol-renamed asm):
    python tools/symbolmap/regen_named_incs.py src/hsd/hsd_fog.c
    python tools/symbolmap/regen_named_incs.py --all      # every tracked .c
"""
import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from asm_wrapper_sweep import load_asm_index, extract_inc_lines  # noqa: E402

ASM_WRAPPER = re.compile(r"^asm\s+[\w*]+\s+([A-Za-z_]\w*)\s*\(", re.M)
INCLUDE = re.compile(r'#include\s+"([^"]*/)?([^"/]+)\.inc"')


def regen_file(src: Path, asm_lines, asm_index) -> tuple:
    text = src.read_text(encoding="utf-8", errors="replace")
    names = ASM_WRAPPER.findall(text)
    # Map each wrapper name to the .inc basename used in its #include.
    inc_for = {}
    for m in re.finditer(
            r"asm\s+[\w*]+\s+([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{\s*\n\s*"
            r'#include\s+"([^"]+)"', text):
        inc_for[m.group(1)] = ROOT / m.group(2)
    done = missing = 0
    for name in names:
        lines = extract_inc_lines(name, asm_lines, asm_index)
        if lines is None:
            print(f"    MISSING in asm: {name}")
            missing += 1
            continue
        path = inc_for.get(name, src.with_name(f"{src.stem}_{name}.inc"))
        path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
        done += 1
    return done, missing


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("files", nargs="*", type=Path)
    ap.add_argument("--all", action="store_true", help="every tracked src/*.c")
    args = ap.parse_args()

    files = list(args.files)
    if args.all:
        files = sorted((ROOT / "src").rglob("*.c"))
    if not files:
        ap.error("pass source files or --all")

    asm_lines, asm_index = load_asm_index()
    total = miss = 0
    for f in files:
        f = f if f.is_absolute() else ROOT / f
        d, m = regen_file(f, asm_lines, asm_index)
        total += d
        miss += m
        if d:
            print(f"  {f.name}: regenerated {d} .inc")
    print(f"[regen] {total} .inc regenerated, {miss} missing from asm")


if __name__ == "__main__":
    main()
