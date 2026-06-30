#!/usr/bin/env python3
"""extract_pikmin.py — Extract matched C/C++ function bodies from projectPiki repos.

Pikmin 1 / Pikmin 2 layout:
- src/**/*.cpp — full matched C++ source (no #if asm wrappers; these
  projects ship matched source directly when the function is solved)
- asm/**/*.s — contains leftover unmatched functions + .ctors/.data/.rodata
  sections (one .s per translation unit)
- configure.py — compile flags per library/TU

A function in src/ that does NOT have a corresponding asm section in
asm/<lib>/<file>.s (under the .text region) is considered matched.

Simpler heuristic (what we actually use): extract ALL function bodies
from src/. Pikmin projects use the policy of only committing matched
source — if it's in src/, it matches. The .s files are holdovers, not
active unmatched source.

Usage:
    python3 extract_pikmin.py /storage/finetune/decomps/pikmin2 > pikmin2.jsonl
    python3 extract_pikmin.py /storage/finetune/decomps/pikmin  > pikmin1.jsonl

Output format (per line):
    {
      "project": "pikmin2",
      "file": "src/plugProjectKandoU/kanban.cpp",
      "lib": "plugProjectKandoU",
      "fn": "bar_TitleSection__Q24User4kandoF",
      "cflags_name": "cflags_pikmin",
      "function_body": "<full extracted function including signature + braces>"
    }
"""
from __future__ import annotations

import io
import json
import re
import sys
from pathlib import Path

if sys.stdout.encoding and sys.stdout.encoding.lower() != "utf-8":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")


# Rough function extractor. Targets typical CW-style C/C++ functions:
#     [template<...>] return_type [Class::]name(args) [const] { body }
# or:
#     extern "C" return_type name(args) { body }
#
# We use brace counting to find the matching close brace. Skip:
# - function prototypes (ending in `;` before `{`)
# - inline member functions inside class bodies (those are harder)
# - macros expanding to functions
FN_HEADER_RE = re.compile(
    r"""
    ^                               # start of line
    (?P<prefix>                     # prefix modifiers
        (?:
            static\s+ |
            inline\s+ |
            virtual\s+ |
            extern\s+ |
            \"C\"\s+ |
            template\s*<[^>]*>\s* |
            __attribute__\s*\([^)]*\)\s*
        )*
    )
    (?P<rettype>                    # return type
        (?:
            (?:const\s+)?
            (?:unsigned\s+|signed\s+)?
            [\w:<>*&\s]+?
        )
    )
    \s+
    (?P<name>
        (?:[\w~]+::)*[\w~]+         # possibly Class::name or dtor
    )
    \s*
    \(                              # open paren of args
    """,
    re.M | re.X
)


def extract_functions(src: str) -> list[tuple[str, str]]:
    """Scan source text for function definitions, return [(name, body_including_signature)]."""
    out: list[tuple[str, str]] = []
    pos = 0
    while pos < len(src):
        # find next "{" on a line after a )
        m = re.search(r"^[^\n]*\)\s*(?:const\s*)?(?:throw\s*\([^)]*\)\s*)?\{", src[pos:], re.M)
        if not m:
            break
        paren_close = pos + m.start() + m.group().rindex(")")
        brace_open = pos + m.end() - 1
        # walk back from paren_close to find function name (skip whitespace + ()
        # we want the identifier just before the args
        # Find the preceding ')' or start of this function
        # Simpler: find start of this "block" by backtracking to prev `}` or `;` or start
        back_from = paren_close
        # Look for bounded start — prev `}` or `;` at column 0 or top-level `\n`
        # For CW source, most free functions are at column 0
        # Scan line start before this signature
        line_start = src.rfind("\n", 0, paren_close) + 1
        # Maybe signature spans multiple lines; widen back up to 5 lines
        for _ in range(5):
            if line_start <= 1: break
            prev_line_end = src.rfind("\n", 0, line_start - 1)
            if prev_line_end < 0: break
            prev_line = src[prev_line_end + 1:line_start - 1].rstrip()
            if not prev_line or prev_line.endswith(";") or prev_line.endswith("}"):
                break
            line_start = prev_line_end + 1

        signature = src[line_start:brace_open].strip()
        # Skip control statements that LOOK like functions
        if re.match(r"^(?:if|for|while|switch|do|return|else|catch)\b", signature):
            pos = brace_open + 1
            continue
        # Skip struct/class definitions (they have `struct X {` not function call)
        if re.match(r"^(?:struct|class|union|enum|namespace)\b", signature):
            pos = brace_open + 1
            continue
        # Skip function-pointer typedefs
        if "typedef" in signature:
            pos = brace_open + 1
            continue

        # Extract function name (last identifier before '(')
        m2 = re.search(r"([\w~]+(?:::[\w~]+)*)\s*\(", signature)
        if not m2:
            pos = brace_open + 1
            continue
        name = m2.group(1)

        # Walk forward to find matching close brace
        depth = 1
        i = brace_open + 1
        in_string = False
        in_char = False
        in_comment_line = False
        in_comment_block = False
        while i < len(src) and depth > 0:
            c = src[i]
            n = src[i + 1] if i + 1 < len(src) else ""
            if in_comment_line:
                if c == "\n": in_comment_line = False
            elif in_comment_block:
                if c == "*" and n == "/":
                    in_comment_block = False; i += 1
            elif in_string:
                if c == "\\": i += 1
                elif c == '"': in_string = False
            elif in_char:
                if c == "\\": i += 1
                elif c == "'": in_char = False
            else:
                if c == "/" and n == "/": in_comment_line = True; i += 1
                elif c == "/" and n == "*": in_comment_block = True; i += 1
                elif c == '"': in_string = True
                elif c == "'": in_char = True
                elif c == "{": depth += 1
                elif c == "}": depth -= 1
            i += 1
        brace_close = i
        body = src[line_start:brace_close].rstrip()
        out.append((name, body))
        pos = brace_close
    return out


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__, file=sys.stderr)
        return 2
    repo = Path(sys.argv[1])
    if not repo.is_dir():
        print(f"ERROR: {repo} not a directory", file=sys.stderr)
        return 1
    project = repo.name

    src_dir = repo / "src"
    if not src_dir.is_dir():
        print(f"ERROR: {repo}/src not found", file=sys.stderr)
        return 1

    # Build lib → cflags map from configure.py (best-effort — defaults to cflags_pikmin)
    cflags_per_lib: dict[str, str] = {}
    cfg_path = repo / "configure.py"
    if cfg_path.exists():
        cfg = cfg_path.read_text(encoding="utf-8", errors="replace")
        # Match each Library(name="X", cflags=Y_name, ...) block
        for m in re.finditer(
            r"(?:Library|LibBuilder)\(\s*name=['\"](?P<n>[^'\"]+)['\"].*?cflags=(?P<c>\w+)",
            cfg, re.S
        ):
            cflags_per_lib[m.group("n")] = m.group("c")
        # Alt syntax seen in pikmin configure.py
        for m in re.finditer(r"\{[^}]*?['\"]lib['\"]:\s*['\"](?P<n>[^'\"]+)['\"][^}]*?['\"]cflags['\"]:\s*(?P<c>\w+)", cfg, re.S):
            cflags_per_lib[m.group("n")] = m.group("c")

    count = 0
    for cpp in sorted(list(src_dir.rglob("*.cpp")) + list(src_dir.rglob("*.c"))):
        rel = cpp.relative_to(repo).as_posix()
        # lib is first component under src/ (e.g., src/plugProjectKandoU/... → lib = plugProjectKandoU)
        parts = rel.split("/")
        lib = parts[1] if len(parts) > 2 else parts[0]
        cflags_name = cflags_per_lib.get(lib, "cflags_base")
        try:
            src = cpp.read_text(encoding="utf-8", errors="replace")
        except Exception:
            continue
        fns = extract_functions(src)
        for name, body in fns:
            # Skip trivial stubs
            if len(body) < 40: continue
            if "{ }" in body.replace("\t", "").replace("\n", "") or body.count("\n") < 2:
                continue
            record = {
                "project": project,
                "file": rel,
                "lib": lib,
                "fn": name,
                "cflags_name": cflags_name,
                "function_body": body,
            }
            print(json.dumps(record, ensure_ascii=False), flush=True)
            count += 1
            if count % 500 == 0:
                print(f"  [{count:5d}] {rel}  {name}", file=sys.stderr, flush=True)
    print(f"Emitted {count} function pairs from {project}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
