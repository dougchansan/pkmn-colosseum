#!/usr/bin/env python3
"""Deterministic function-body splicer for colosseum_script.c integration.

Replaces named top-level function definitions in a source file with new bodies,
keyed by function name. Used to integrate per-agent winning function bodies into
a single file without text-merge ambiguity.

Each function in colosseum_script.c is a top-level definition: the signature
line starts at column 0 with a return type, the opening `{` is on that line or a
few lines below, and the body closes with `}` at column 0.

Usage:
  python cs_splice.py <src.c> <patch.json> <out.c>

patch.json: {"fn_80217220": "<full replacement definition text>", ...}
The replacement text MUST be the complete definition (signature .. closing brace).

Exits nonzero if any named function is not found exactly once (safety: a missing
or ambiguous target means the splice would silently corrupt the file).
"""
import json
import re
import sys
from pathlib import Path


# A signature where the return type precedes the name on the same line
# (e.g. `void fn_X(`, `static u8 *fn_X(`). Restricted to type-ish chars so it
# does not match calls/expressions like `a = fn_X(`.
_TYPED = r'^[A-Za-z_][\w \t\*]*\b{n}\s*\('
# The name at column 0 (return type is on the preceding line — Ghidra imports).
_COL0 = r'^{n}\s*\('
# A line that is purely a return type / qualifiers (the line above a col-0 name).
TYPE_LINE = re.compile(
    r'^(?:static|const|volatile|unsigned|signed|inline|register)?'
    r'(?:\s+(?:static|const|volatile|unsigned|signed))*\s*'
    r'[A-Za-z_]\w*\s*\**\s*$')


def detect_newline(text):
    """Return the dominant newline style.

    A source file can contain one stray CRLF in an otherwise LF file. Splitting
    on CRLF just because it appears once collapses most of the file into one
    line and makes function lookup fail.
    """
    crlf = text.count("\r\n")
    lf = text.count("\n")
    bare_lf = lf - crlf
    return "\r\n" if crlf > bare_lf else "\n"


def _active_lines(lines):
    """Boolean per line: is it in a LIVE preprocessor branch? Only a literal
    `#if 0` (and its pre-`#else` body) is treated as dead; every other #if form
    is assumed live (conservative — never hides real code). Used to disambiguate
    a still-wrapped `#if 0 asm void fn(){...} #else <real C> #endif` where both
    the dead asm stub and the live real-C body match the same fn name."""
    active = [True] * len(lines)
    stack = []
    for i, raw in enumerate(lines):
        s = raw.strip()
        if s.startswith("#if"):
            toks = s[3:].split()
            # `#if 0` -> dead. The PCPORT guard is also dead in the matching
            # build: PCPORT is only defined for the PC port, never for byte
            # matching, so `#ifdef PCPORT` is dead (its `#else` holds the real
            # C) and `#ifndef PCPORT` is live. Without this, every still-wrapped
            # near-miss in a PCPORT-guarded TU is mis-marked dead and cannot be
            # disambiguated for save/integrate.
            if s.startswith("#ifdef") and "PCPORT" in toks:
                dead = True
            elif s.startswith("#ifndef") and "PCPORT" in toks:
                dead = False
            else:
                dead = bool(toks) and toks[0] == "0"
            stack.append(not dead)
        elif s.startswith("#elif"):
            if stack:
                stack[-1] = True                          # conservatively live
        elif s.startswith("#else"):
            if stack:
                stack[-1] = not stack[-1]
        elif s.startswith("#endif"):
            if stack:
                stack.pop()
        active[i] = all(stack) if stack else True
    return active


def find_def_span(lines, name):
    """Return (start_idx, end_idx) inclusive for the top-level definition of
    `name`, or None. Handles both single-line signatures (`type name(`) and
    multi-line ones where the return type sits on its own line above a
    column-0 name. A definition's opening `{` must precede any `;` (else it's a
    forward decl / call). Brace-matched to the closing `}`."""
    typed_re = re.compile(_TYPED.format(n=re.escape(name)))
    col0_re = re.compile(_COL0.format(n=re.escape(name)))
    spans = []
    n = len(lines)
    i = 0
    while i < n:
        is_typed = bool(typed_re.match(lines[i]))
        is_col0 = (not is_typed) and bool(col0_re.match(lines[i]))
        if is_typed or is_col0:
            # Scan forward for the opening brace, bailing if we hit `;` first
            # (forward declaration / prototype / call).
            j = i
            saw_brace = False
            decl_only = False
            while j < n and j < i + 6:
                if "{" in lines[j]:
                    saw_brace = True
                    break
                if ";" in lines[j]:
                    decl_only = True
                    break
                j += 1
            if saw_brace and not decl_only:
                start = i
                # Column-0 name: absorb the preceding pure-return-type line(s)
                # so the splice overwrites them instead of orphaning the type.
                if is_col0:
                    p = i - 1
                    while p >= 0 and lines[p].strip() and TYPE_LINE.match(lines[p].strip()):
                        start = p
                        p -= 1
                # brace-match from j to the closing `}`
                depth = 0
                k = j
                started = False
                while k < n:
                    depth += lines[k].count("{") - lines[k].count("}")
                    if "{" in lines[k]:
                        started = True
                    if started and depth <= 0:
                        spans.append((start, k))
                        break
                    k += 1
                i = k + 1
                continue
        i += 1
    if len(spans) > 1:
        # Prefer the definition in a LIVE branch (resolves the still-wrapped
        # `#if 0 asm stub #else real-C #endif` collision).
        active = _active_lines(lines)
        live = [sp for sp in spans if active[sp[0]]]
        if live:
            spans = live
    if len(spans) != 1:
        return None if not spans else spans  # caller handles ambiguity
    return spans[0]


def main():
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    src = Path(sys.argv[1])
    patch = json.loads(Path(sys.argv[2]).read_text(encoding="utf-8"))
    out = Path(sys.argv[3])

    raw = src.read_bytes()
    text = raw.decode("utf-8", errors="replace")
    nl = detect_newline(text)
    lines = text.split(nl)

    errors = []
    # Apply bottom-up so earlier indices stay valid as we splice.
    targets = []
    for name, body in patch.items():
        span = find_def_span(lines, name)
        if span is None:
            errors.append(f"NOT FOUND: {name}")
            continue
        if isinstance(span, list):
            errors.append(f"AMBIGUOUS ({len(span)} defs): {name}")
            continue
        targets.append((span[0], span[1], name, body))

    if errors:
        for e in errors:
            print(e, file=sys.stderr)
        sys.exit(1)

    targets.sort(key=lambda t: -t[0])
    for start, end, name, body in targets:
        body_lines = body.replace("\r\n", "\n").split("\n")
        # strip leading/trailing blank lines from the replacement body
        while body_lines and body_lines[0].strip() == "":
            body_lines = body_lines[1:]
        while body_lines and body_lines[-1].strip() == "":
            body_lines = body_lines[:-1]
        # De-dupe a `#pragma peephole` wrapper. Canon often already wraps this span
        # with `#pragma peephole off` (line above) and `... on` (line below). If the
        # replacement body carries its OWN copies, splicing would emit the pragma
        # TWICE — and a doubled `#pragma peephole` corrupts CodeWarrior's parse of
        # the NEXT asm block downstream (a "declaration syntax error" cascade on the
        # adjacent `asm void fn(){#include .inc}` that aborts the whole batch). Drop
        # the body's copy when canon already supplies the wrapper.
        above = lines[start - 1].strip() if start > 0 else ""
        below = lines[end + 1].strip() if end + 1 < len(lines) else ""
        if (above.startswith("#pragma peephole off") and body_lines
                and body_lines[0].strip().startswith("#pragma peephole off")):
            body_lines = body_lines[1:]
        if (below.startswith("#pragma peephole on") and body_lines
                and body_lines[-1].strip().startswith("#pragma peephole on")):
            body_lines = body_lines[:-1]
        lines[start:end + 1] = body_lines

    out.write_bytes(nl.join(lines).encode("utf-8"))
    print(f"spliced {len(targets)} function(s) -> {out}")


if __name__ == "__main__":
    main()
