#!/usr/bin/env python3
"""splice_fn.py <dest.c> <src_scratch.c> <fn> - replace fn's definition in dest
with the version from src_scratch, matching by function-boundary brace counting.

Composes multiple independent agent wins into one file cleanly (functions are
disjoint, so repeated splices never conflict). Edits dest IN PLACE.
Prints the line span replaced. Exit 1 if the fn can't be uniquely located.
"""
import sys, re
from pathlib import Path


def find_span(lines, fn):
    """Return (start_idx, end_idx_inclusive) of the ACTIVE top-level definition of fn.
    Skips dead `#if 0` regions and `asm`-qualified wrapper definitions (the .inc
    wrappers), and skips bare prototypes (signature ending in `;` before any `{`)."""
    sig = re.compile(rf'^([A-Za-z_][^;{{]*?)\b{re.escape(fn)}\s*\(')
    if_depth0 = 0  # nesting depth of dead `#if 0` (not in #else)
    if_stack = []  # ('if0'|'other', in_else)
    for i, ln in enumerate(lines):
        s = ln.strip()
        if re.match(r'#\s*if\s+0\b', s):
            if_stack.append(['if0', False]);
            if True: if_depth0 += 1
            continue
        if re.match(r'#\s*if', s):
            if_stack.append(['other', False]); continue
        if re.match(r'#\s*else', s):
            if if_stack:
                if if_stack[-1][0] == 'if0' and not if_stack[-1][1]:
                    if_depth0 -= 1
                if_stack[-1][1] = True
            continue
        if re.match(r'#\s*endif', s):
            if if_stack:
                k = if_stack.pop()
                if k[0] == 'if0' and not k[1]:
                    if_depth0 -= 1
            continue
        if if_depth0 > 0:
            continue  # inside a dead #if 0 (target asm wrapper etc.)
        m = sig.match(ln)
        if m:
            if 'asm' in m.group(1).split():
                continue  # asm wrapper, not the compiled C
            # find opening brace; if a ';' appears first it's a prototype -> skip
            bo = i
            while bo < len(lines) and '{' not in lines[bo] and bo < i + 6:
                if ';' in lines[bo]:
                    bo = None; break
                bo += 1
            if bo is None or bo >= len(lines) or '{' not in lines[bo]:
                continue
            depth = 0
            j = bo
            while j < len(lines):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    return i, j
                j += 1
    return None


def main():
    dest = Path(sys.argv[1])
    src = Path(sys.argv[2])
    fn = sys.argv[3]
    dlines = dest.read_text(errors="replace").splitlines(keepends=True)
    slines = src.read_text(errors="replace").splitlines(keepends=True)
    ds = find_span(dlines, fn)
    ss = find_span(slines, fn)
    if not ds:
        print(f"ERROR: {fn} not found in dest {dest}"); return 1
    if not ss:
        print(f"ERROR: {fn} not found in src {src}"); return 1
    # normalize inserted lines to dest's line ending (real file is CRLF; scratch may be LF)
    dest_eol = "\r\n" if (dlines and dlines[0].endswith("\r\n")) else "\n"
    inserted = []
    for ln in slines[ss[0]:ss[1] + 1]:
        body = ln.rstrip("\r\n")
        had_nl = ln.endswith("\n")
        inserted.append(body + dest_eol if had_nl else body)
    new = dlines[:ds[0]] + inserted + dlines[ds[1] + 1:]
    dest.write_text("".join(new), errors="replace")
    print(f"spliced {fn}: dest lines {ds[0]+1}-{ds[1]+1} <- src lines {ss[0]+1}-{ss[1]+1} "
          f"({ds[1]-ds[0]+1} -> {ss[1]-ss[0]+1} lines)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
