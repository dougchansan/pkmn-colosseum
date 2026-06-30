#!/usr/bin/env python3
"""Generate a ctx.c with all #includes expanded — usable as "Context" on https://decomp.me.

Adapted from zeldaret/tp tools/decompctx.py (originally encounter/dtk-template).

Usage:
    python3 tools/decompctx.py src/game/gs_title.c

By default this resolves includes against `include/` (the project's only
`-i` directory in compile_config.json). Pass extra `-I dir` flags to add more.

The output `ctx.c` (default) can be pasted into the "Context" field on
decomp.me to get a scratch with all the typedefs/externs the function needs.
"""

import argparse
import fnmatch
import os
import re
import sys
from typing import List

script_dir = os.path.dirname(os.path.realpath(__file__))
root_dir = os.path.abspath(os.path.join(script_dir, ".."))
src_dir = os.path.join(root_dir, "src")
default_include = os.path.join(root_dir, "include")
include_dirs: List[str] = []
exclude_globs: List[str] = []

include_pattern = re.compile(r'^#\s*include\s*[<"](.+?)[>"]')
guard_pattern = re.compile(r"^#\s*ifndef\s+(.*)$")
once_pattern = re.compile(r"^#\s*pragma\s+once$")

defines = set()
deps: List[str] = []


def generate_prelude(prelude_defs) -> str:
    if not prelude_defs:
        return ""
    out = "/* decompctx prelude */\n"
    for d in prelude_defs:
        parts = d.split("=", 1)
        if len(parts) == 2:
            out += f"#define {parts[0]} {parts[1]}\n"
        else:
            out += f"#define {parts[0]}\n"
    out += "/* end decompctx prelude */\n\n"
    return out


def import_h_file(in_file: str, r_path: str) -> str:
    # Try relative to the current file first, then the include dirs.
    candidate = os.path.join(root_dir, r_path, in_file)
    if os.path.exists(candidate):
        return import_c_file(candidate)
    for inc_dir in include_dirs:
        candidate = os.path.join(inc_dir, in_file)
        if os.path.exists(candidate):
            return import_c_file(candidate)
    print(f"  warning: could not resolve include '{in_file}'", file=sys.stderr)
    return f'/* unresolved: #include "{in_file}" */\n'


def import_c_file(in_file: str) -> str:
    rel = os.path.relpath(in_file, root_dir)
    deps.append(rel)
    try:
        with open(in_file, encoding="utf-8") as f:
            return process_file(rel, list(f))
    except UnicodeDecodeError:
        with open(in_file, errors="replace") as f:
            return process_file(rel, list(f))


def process_file(in_file: str, lines) -> str:
    out = ""
    for idx, line in enumerate(lines):
        if idx == 0:
            g = guard_pattern.match(line.strip())
            if g:
                if g.group(1) in defines:
                    return ""
                defines.add(g.group(1))
            else:
                o = once_pattern.match(line.strip())
                if o:
                    if in_file in defines:
                        return ""
                    defines.add(in_file)
        m = include_pattern.match(line.strip())
        if m and not m.group(1).endswith(".s") and not m.group(1).endswith(".inc"):
            inc = m.group(1)
            excluded = any(fnmatch.fnmatch(inc, g) for g in exclude_globs)
            out += f'/* "{in_file}" line {idx} include "{inc}" */\n'
            if excluded:
                out += "/* excluded */\n"
            else:
                out += import_h_file(inc, os.path.dirname(in_file))
            out += f'/* end "{inc}" */\n'
        else:
            out += line
    return out


def sanitize_path(p: str) -> str:
    return p.replace("\\", "/").replace(" ", r"\ ")


def main() -> None:
    ap = argparse.ArgumentParser(description="Build a ctx.c for decomp.me Context.")
    ap.add_argument("c_file", help="Source file (.c/.cpp) to build context for")
    ap.add_argument("-o", "--output", default="ctx.c", help="Output path (default: ctx.c)")
    ap.add_argument("-I", "--include", action="append", help="Extra include dir (repeatable)")
    ap.add_argument("-x", "--exclude", action="append", help="Glob to skip (repeatable)")
    ap.add_argument("-D", "--define", action="append", help="Prelude #define (repeatable)")
    ap.add_argument("-d", "--depfile", help="Optional Makefile-style depfile output")
    args = ap.parse_args()

    global include_dirs, exclude_globs
    include_dirs = [default_include] + (args.include or [])
    exclude_globs = args.exclude or []

    out = generate_prelude(args.define or [])
    out += import_c_file(os.path.abspath(args.c_file))

    out_path = os.path.join(root_dir, args.output) if not os.path.isabs(args.output) else args.output
    with open(out_path, "w", encoding="utf-8") as f:
        f.write(out)
    print(f"wrote {out_path} ({len(deps)} files inlined)")

    if args.depfile:
        with open(args.depfile, "w", encoding="utf-8") as f:
            f.write(sanitize_path(args.output) + ":")
            for d in deps:
                f.write(f" \\\n\t{sanitize_path(d)}")


if __name__ == "__main__":
    main()
