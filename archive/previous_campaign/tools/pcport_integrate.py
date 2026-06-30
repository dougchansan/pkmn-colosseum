#!/usr/bin/env python3
"""pcport_integrate.py — replace the inactive #else C body of an asm wrapper with a
new functional decompilation. The #if 1 asm branch (byte-match truth) is untouched;
the #else branch is never compiled by CodeWarrior, so the GC build is unaffected.

Usage: python tools/pcport_integrate.py <tu.c> <fn_name> <body_file.c>
"""
import re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main():
    tu, fn, body_file = sys.argv[1], sys.argv[2], sys.argv[3]
    path = ROOT / tu
    lines = path.read_text(errors="replace").splitlines(keepends=True)
    body = Path(body_file).read_text().rstrip("\n")

    asm_re = re.compile(r'^\s*asm\s+[\w\s\*]*?\b' + re.escape(fn) + r'\s*\(')
    i = 0
    n = len(lines)
    while i < n:
        if re.match(r'^\s*#\s*if\s+1\b', lines[i]):
            # wrapper for our fn?
            k, depth, else_idx, end_idx, found = i + 1, 1, None, None, False
            while k < n:
                s = lines[k]
                if re.match(r'^\s*#\s*if', s):
                    depth += 1
                elif re.match(r'^\s*#\s*endif', s):
                    depth -= 1
                    if depth == 0:
                        end_idx = k
                        break
                elif re.match(r'^\s*#\s*else', s) and depth == 1:
                    else_idx = k
                if depth == 1 and else_idx is None and asm_re.match(s):
                    found = True
                k += 1
            if found and else_idx is not None and end_idx is not None:
                new = lines[:else_idx + 1] + [body + "\n"] + lines[end_idx:]
                path.write_text("".join(new))
                print(f"replaced #else body of {fn} in {tu} "
                      f"(was {end_idx - else_idx - 1} lines, now {body.count(chr(10)) + 1})")
                return 0
            i = end_idx if end_idx is not None else k
        i += 1
    print(f"ERROR: wrapper for {fn} not found in {tu}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
