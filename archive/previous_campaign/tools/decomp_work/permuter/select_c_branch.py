#!/usr/bin/env python3
# select_c_branch.py <src.c> <target_fn>
#
# Prints to stdout a copy of <src.c> in which the TARGET function's active asm
# wrapper is de-selected so that the `#else` C decomposition candidate is the
# one the preprocessor keeps. Concretely, the project stores each function as:
#
#     #if 1                       <-- asm wrapper active
#     asm void fn_TARGET(void) {
#     #include "..._fn_TARGET.inc"
#     }
#     #else
#     void fn_TARGET(...) { ...C candidate... }   <-- what we want to permute
#     #endif
#
# When the wrapper guard is `#if 1`, `mwcceppc -E` keeps the raw asm and the
# permuter cannot parse it. This helper rewrites ONLY the `#if 1` that guards
# the target's `asm void fn_TARGET` wrapper to `#if 0`, so preprocessing keeps
# the `#else` C body instead. Every other function (and the original repo
# source) is left untouched — the output is written to a permuter-owned temp,
# never back to the tree.
#
# If the target is already `#if 0` (asm wrapper inactive, C branch already
# selected) or has no such guarded wrapper, the source is emitted unchanged.
#
# Exit status is always 0; a one-line note goes to stderr.

import re
import sys
from pathlib import Path


def select_c_branch(source: str, target: str) -> tuple[str, bool]:
    lines = source.split("\n")
    # Find `asm <ret> <target>(` wrapper line, then walk upward to the nearest
    # `#if 1` / `#ifdef`-style guard that opens its block and flip it to `#if 0`.
    wrapper_re = re.compile(r"^\s*asm\b.*\b" + re.escape(target) + r"\s*\(")
    flipped = False
    for idx, line in enumerate(lines):
        if not wrapper_re.search(line):
            continue
        # Walk upward over blank/comment lines to the guard directive.
        j = idx - 1
        while j >= 0 and lines[j].strip() == "":
            j -= 1
        if j >= 0 and re.match(r"^\s*#if\s+1\s*$", lines[j]):
            lines[j] = "#if 0"
            flipped = True
        break
    return "\n".join(lines), flipped


def main() -> None:
    if len(sys.argv) != 3:
        sys.exit("usage: select_c_branch.py <src.c> <target_fn>")
    src = Path(sys.argv[1]).read_text(encoding="utf-8", errors="replace")
    out, flipped = select_c_branch(src, sys.argv[2])
    sys.stdout.write(out)
    note = (
        f"select_c_branch: flipped target {sys.argv[2]} asm wrapper #if 1 -> #if 0"
        if flipped
        else f"select_c_branch: target {sys.argv[2]} already on C branch (no flip)"
    )
    print(note, file=sys.stderr)


if __name__ == "__main__":
    main()
