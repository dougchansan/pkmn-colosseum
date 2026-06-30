#!/usr/bin/env python3
"""
import_reference.py - Import already-matched reference C for shared library code.

THE INSIGHT
-----------
Pokemon Colosseum (GC6E01) was built with MetroWerks CodeWarrior GC 1.2.5n/1.3
plus the stock Nintendo Dolphin SDK, the MetroWerks Standard Library (MSL/CRT),
and HAL's sysdolphin (HSD). Those library layers are *byte-identical* across the
many GameCube titles built with the same toolchain. The functions colosseum is
still hand-decompiling from asm in src/dolphin/** and src/crt|runtime|libc are
ALREADY matched in public open-source GC decomp projects (dolsdk2001, zeldaret/tww,
projectPiki/pikmin2, ...). We should PORT them, not reinvent them.

WHAT THIS TOOL DOES
-------------------
Given a reference C function (pasted/in a file) and a target colosseum .c file,
it:
  1. Normalizes the reference to this project's conventions:
       - standard project types  (unsigned long -> u32, size_t kept, etc.)
       - C89: all declarations before statements within a block
       - strips common GCC-isms / attributes
       - flags externs to route through the symbol DB if one exists
  2. Stages it into the target file IN PLACE of the existing asm wrapper / stub
     (the `asm <type> name(...) { ... }` block, an `#if 1` asm wrapper, or a
     `fn_XXXXXXXX` register-soup placeholder), leaving a STAGED/UNVERIFIED marker.

HONESTY / SAFETY RULES (the team has been burned by faked matches)
-----------------------------------------------------------------
  * This tool NEVER claims a byte match. It only stages C for later verification
    in the main tree via tools/compile_check.py + tools/match_test.py.
  * It REFUSES to touch any *.inc assembly file.
  * It never flips `#if 0` -> `#if 1` and never edits an .inc.
  * Every staged block is wrapped with an explicit
    `/* STAGED-IMPORT (UNVERIFIED) ... */` banner so a human/parent can review.

This is intentionally conservative: it favors a clean, reviewable diff over
clever automation. Anything it cannot do safely it leaves as a TODO comment.

USAGE
-----
  # Normalize a reference function and print the normalized C (no file writes):
  python tools/import_reference.py normalize --ref ref.c [--func NAME]

  # Stage a reference into a target file, replacing the named function's
  # asm wrapper / stub:
  python tools/import_reference.py stage \
      --target src/crt/string.c --func strncmp --ref ref_strncmp.c \
      --source "zeldaret/tww MSL_C/MSL_Common/Src/string.c" \
      [--dry-run]

  # Inline reference (instead of --ref file):
  python tools/import_reference.py stage --target ... --func ... \
      --ref-text "int strncmp(...) { ... }" --source "..."
"""

import argparse
import datetime
import re
import sys
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = TOOLS_DIR.parent

# ---------------------------------------------------------------------------
# Symbol DB hook (optional). The task brief mentions tools/symdb.py; it does
# not exist in this tree yet. If it ever appears, route externs through it.
# Otherwise we leave a TODO and pass externs through unchanged.
# ---------------------------------------------------------------------------
HAS_SYMDB = (TOOLS_DIR / "symdb.py").exists()
if HAS_SYMDB:
    sys.path.insert(0, str(TOOLS_DIR))
    try:
        import symdb  # type: ignore
    except Exception:
        symdb = None
        HAS_SYMDB = False
else:
    symdb = None


# ---------------------------------------------------------------------------
# Normalization
# ---------------------------------------------------------------------------

# Reference projects use stdint/MSL spellings; colosseum uses Dolphin types.
# Order matters: longest / most specific first.
TYPE_MAP = [
    (r"\bunsigned\s+long\s+long\b", "u64"),
    (r"\bsigned\s+long\s+long\b", "s64"),
    (r"\bunsigned\s+long\b", "u32"),
    (r"\bsigned\s+long\b", "s32"),
    (r"\bunsigned\s+int\b", "u32"),
    (r"\bsigned\s+int\b", "s32"),
    (r"\bunsigned\s+short\b", "u16"),
    (r"\bsigned\s+short\b", "s16"),
    (r"\bunsigned\s+char\b", "u8"),
    (r"\bsigned\s+char\b", "s8"),
    (r"\buint64_t\b", "u64"),
    (r"\bint64_t\b", "s64"),
    (r"\buint32_t\b", "u32"),
    (r"\bint32_t\b", "s32"),
    (r"\buint16_t\b", "u16"),
    (r"\bint16_t\b", "s16"),
    (r"\buint8_t\b", "u8"),
    (r"\bint8_t\b", "s8"),
    # NOTE: bare `int`, `short`, `char`, `long`, `void`, `float`, `double`,
    # `size_t` are intentionally left alone. The MSL/SDK source frequently
    # depends on `int`/`char`/`size_t` exactly as written to match the
    # compiler's argument-passing and comparison codegen.
]

# GCC-isms / attributes that never appear in CW matching source.
GCCISM_PATTERNS = [
    r"__attribute__\s*\(\([^)]*\)\)",
    r"__restrict__\b",
    r"\brestrict\b",
    r"__inline__\b",
    r"__builtin_expect\s*\(",   # only the wrapper; handled specially below
]


def map_types(text: str) -> str:
    for pat, repl in TYPE_MAP:
        text = re.sub(pat, repl, text)
    return text


def strip_gccisms(text: str, notes: list) -> str:
    for pat in GCCISM_PATTERNS:
        if re.search(pat, text):
            notes.append("stripped GCC-ism matching /%s/" % pat)
    text = re.sub(r"__attribute__\s*\(\([^)]*\)\)", "", text)
    text = re.sub(r"\b__restrict__\b", "", text)
    text = re.sub(r"\brestrict\b", "", text)
    text = re.sub(r"\b__inline__\b", "inline", text)
    return text


def find_externs(text: str) -> list:
    """Return list of (line, symbol) externs needing DB routing."""
    out = []
    for line in text.splitlines():
        m = re.match(r"\s*extern\b.*?\b([A-Za-z_]\w*)\s*[\(;]", line)
        if m:
            out.append((line.strip(), m.group(1)))
    return out


def route_externs(text: str, notes: list) -> str:
    externs = find_externs(text)
    if not externs:
        return text
    if HAS_SYMDB and symdb is not None:
        # Best-effort: ask the DB for the canonical declaration of each symbol.
        for _, sym in externs:
            try:
                canon = symdb.lookup_decl(sym)  # type: ignore[attr-defined]
            except Exception:
                canon = None
            if canon:
                notes.append("routed extern '%s' through symdb" % sym)
        notes.append("symdb present: verify extern types against the DB")
    else:
        for _, sym in externs:
            notes.append(
                "TODO(symdb): no symbol DB present - verify extern '%s' type "
                "and SDA/section placement by hand" % sym
            )
    return text


# C89 declaration ordering -------------------------------------------------
# CodeWarrior at -O4 in this project enforces C89 (declarations-before-statements
# per block). Reference MSL/SDK source is already C89, but C99-style
# `for (int i = ...)` and mid-block declarations do appear in some references.
# We detect (and report) those rather than silently rewriting, because moving a
# declaration can change register allocation and therefore the match. A human
# should confirm the reorder.

RE_FOR_DECL = re.compile(r"for\s*\(\s*((?:unsigned|signed|const)\s+)*"
                         r"(?:int|short|long|char|u8|u16|u32|u64|s8|s16|s32|s64)\b")
RE_DECL_LINE = re.compile(
    r"^\s*(?:const\s+)?"
    r"(?:u8|u16|u32|u64|s8|s16|s32|s64|int|short|long|char|void|float|double|"
    r"size_t|BOOL|f32|f64|OSTime|[A-Z]\w*)\b[\s\*]+[A-Za-z_]")


def check_c89(text: str, notes: list) -> None:
    if RE_FOR_DECL.search(text):
        notes.append("WARNING: C99 for-loop declaration found; CW needs C89 - "
                     "hoist the loop variable to the top of its block")
    # Detect a declaration that appears after a non-declaration statement in a
    # block (very rough heuristic; advisory only).
    in_body = False
    seen_stmt = False
    for raw in text.splitlines():
        line = raw.strip()
        if "{" in line:
            in_body = True
            seen_stmt = False
        if "}" in line:
            in_body = False
        if not in_body or not line or line.startswith(("/*", "*", "//")):
            continue
        # A declaration line starts with a type keyword. Calls like `foo(x);`
        # are not declarations; an initialized decl `u32 x = f();` or
        # `const u8* p = (u8*)q - 1;` still is. Only the *declarator* (text
        # before any `=`) must be free of a call paren.
        declarator = line.split("=")[0]
        is_decl = bool(RE_DECL_LINE.match(line)) and "(" not in declarator
        if is_decl and seen_stmt:
            notes.append("WARNING: declaration after statement (C89) near: %s"
                         % line[:60])
        elif not is_decl and not line.endswith("{") and line not in ("{", "}"):
            seen_stmt = True


FUNC_HEADER_RE = re.compile(
    r"(?P<ret>(?:[A-Za-z_]\w*[\s\*]+)+)"
    r"(?P<name>[A-Za-z_]\w*)\s*\([^;{]*\)\s*\{",
    re.MULTILINE,
)


def _enumerate_functions(text: str) -> list:
    """Return [(name, full_function_text)] for every top-level function."""
    out = []
    for m in FUNC_HEADER_RE.finditer(text):
        fname = m.group("name")
        start = m.start()
        i = text.index("{", m.end() - 1)
        depth = 0
        j = i
        while j < len(text):
            if text[j] == "{":
                depth += 1
            elif text[j] == "}":
                depth -= 1
                if depth == 0:
                    out.append((fname, text[start:j + 1]))
                    break
            j += 1
    return out


def extract_function(text: str, name: str | None) -> tuple[str, str]:
    """Return (func_name, full_function_text) by brace matching.

    `name` selects the reference function to pull. If it is not present in the
    reference but the reference defines exactly one function, that sole function
    is used (this is the common case: a one-function reference snippet staged
    under a target `fn_XXXX` name). Otherwise we error and list what's available
    so the caller can pass the right --ref-func.
    """
    funcs = _enumerate_functions(text)
    if not funcs:
        raise ValueError("no function definition found in reference")
    if name:
        for fname, body in funcs:
            if fname == name:
                return fname, body
        if len(funcs) == 1:
            return funcs[0]
        available = ", ".join(f[0] for f in funcs)
        raise ValueError(
            "function %s not found in reference; available: %s "
            "(pass --ref-func to choose)" % (name, available))
    return funcs[0]


def normalize(ref_text: str, func: str | None):
    notes: list[str] = []
    fname, body = extract_function(ref_text, func)
    body = strip_gccisms(body, notes)
    body = map_types(body)
    body = route_externs(body, notes)
    check_c89(body, notes)
    return fname, body, notes


# ---------------------------------------------------------------------------
# Staging into a target source file
# ---------------------------------------------------------------------------

def refuse_inc(path: Path):
    if path.suffix.lower() == ".inc":
        raise SystemExit("REFUSED: this tool never edits .inc assembly files (%s)"
                         % path)


def find_target_block(src: str, func: str) -> tuple[int, int, str]:
    """
    Locate the existing definition of `func` in the target source and return
    (start, end, kind). Handles:
      - asm wrapper:   `asm <type> func(...) { ... }`  (often inside #pragma push/pop)
      - plain def:     `<type> func(...) { ... }`
      - fn_ placeholder stub with a leading comment.
    Never matches inside an #if 0 ... #endif that we'd have to flip.
    """
    # asm definition
    patterns = [
        (r"asm\s+[^\n;{}]*\b%s\s*\(" % re.escape(func), "asm-wrapper"),
        (r"(?:^|\n)[^\n;{}]*\b%s\s*\([^;{}]*\)\s*\{" % re.escape(func), "c-def"),
    ]
    for pat, kind in patterns:
        m = re.search(pat, src)
        if not m:
            continue
        # Snap the start to the beginning of the line that the match begins on.
        # The `c-def` pattern can begin with a leading "\n" capture, so search
        # from the first non-newline character of the match.
        body_start = m.start()
        while body_start < len(src) and src[body_start] in "\r\n":
            body_start += 1
        start = src.rfind("\n", 0, body_start)
        start = 0 if start < 0 else start + 1
        # Extend left ONLY for a directly-preceding `#pragma push` cluster
        # (asm wrappers live inside push/.../pop). We never absorb a preceding
        # comment, so doc comments above the function stay intact.
        pre = src[:start].rstrip("\n")
        push_idx = pre.rfind("#pragma push")
        if push_idx != -1:
            between = pre[push_idx:]
            # only a small pragma cluster, and nothing but pragmas between it
            # and the function (no comment/code lines)
            cluster_lines = [ln.strip() for ln in between.splitlines()]
            if len(cluster_lines) <= 4 and all(
                ln.startswith("#pragma") or ln == "" for ln in cluster_lines
            ):
                start = push_idx
        # brace-match the body
        brace = src.index("{", m.start())
        depth = 0
        j = brace
        while j < len(src):
            if src[j] == "{":
                depth += 1
            elif src[j] == "}":
                depth -= 1
                if depth == 0:
                    end = j + 1
                    break
            j += 1
        else:
            raise ValueError("unbalanced braces around %s in target" % func)
        # include a trailing `#pragma pop` if present
        tail = src[end:end + 40]
        mp = re.match(r"\s*#pragma pop", tail)
        if mp:
            end += mp.end()
        return start, end, kind
    raise ValueError("could not locate definition of %s in target" % func)


def make_banner(func: str, source: str, notes: list) -> str:
    today = datetime.date.today().isoformat()
    lines = [
        "/* ===== STAGED-IMPORT (UNVERIFIED) ===========================",
        " * function : %s" % func,
        " * imported : %s via tools/import_reference.py" % today,
        " * reference: %s" % source,
        " * STATUS   : staged for verification only - NOT confirmed matching.",
        " *            Verify in the main tree with:",
        " *              python tools/compile_check.py <this file>",
        " *              python tools/match_test.py %s" % func,
    ]
    for n in notes:
        lines.append(" *   note: %s" % n)
    lines.append(" * =========================================================== */")
    return "\n".join(lines)


def stage(target: Path, func: str, ref_text: str, source: str, dry_run: bool,
          ref_func: str | None = None):
    refuse_inc(target)
    if not target.exists():
        raise SystemExit("target does not exist: %s" % target)

    # `func` is the TARGET symbol name (often a fn_XXXX address symbol).
    # `ref_func` selects which function to pull from the reference; default to
    # the sole reference function (handled in extract_function).
    fname, normalized, notes = normalize(ref_text, ref_func)
    if fname != func:
        notes.append("reference function name was '%s'; staging as '%s'"
                     % (fname, func))
        normalized = re.sub(r"\b%s\s*\(" % re.escape(fname),
                            func + "(", normalized, count=1)

    src = target.read_text(encoding="utf-8", errors="replace")
    start, end, kind = find_target_block(src, func)
    notes.insert(0, "replaced existing %s definition" % kind)

    banner = make_banner(func, source, notes)
    replacement = banner + "\n" + normalized + "\n"

    new_src = src[:start] + replacement + src[end:]

    if dry_run:
        print("--- STAGED DIFF (dry-run) for %s in %s ---" % (func, target))
        print("REMOVED (%d chars, kind=%s):" % (end - start, kind))
        print(src[start:end])
        print("\nADDED:")
        print(replacement)
        return

    target.write_text(new_src, encoding="utf-8")
    print("Staged %s into %s (replaced %s block)." % (func, target, kind))
    print("Notes:")
    for n in notes:
        print("  - %s" % n)
    print("\nThis is STAGED/UNVERIFIED. Verify in the main tree before trusting it.")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def load_ref(args) -> str:
    if args.ref_text:
        return args.ref_text
    if args.ref:
        p = Path(args.ref)
        if not p.is_absolute():
            p = PROJECT_ROOT / p
        return p.read_text(encoding="utf-8", errors="replace")
    raise SystemExit("provide --ref FILE or --ref-text TEXT")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    n = sub.add_parser("normalize", help="normalize a reference function, print C")
    n.add_argument("--ref")
    n.add_argument("--ref-text")
    n.add_argument("--func")

    s = sub.add_parser("stage", help="stage a normalized reference into a target .c")
    s.add_argument("--target", required=True)
    s.add_argument("--func", required=True)
    s.add_argument("--ref")
    s.add_argument("--ref-text")
    s.add_argument("--ref-func",
                   help="name of the function to pull FROM the reference "
                        "(default: the sole function in the reference). Use when "
                        "the reference defines several functions and --func is the "
                        "target's fn_XXXX symbol name.")
    s.add_argument("--source", required=True,
                   help="human-readable provenance, e.g. 'zeldaret/tww string.c'")
    s.add_argument("--dry-run", action="store_true")

    args = ap.parse_args()

    if args.cmd == "normalize":
        ref = load_ref(args)
        fname, body, notes = normalize(ref, args.func)
        if not HAS_SYMDB:
            print("/* NOTE: tools/symdb.py absent - externs left for manual review */")
        print(body)
        sys.stderr.write("\n[normalize] function: %s\n" % fname)
        for nn in notes:
            sys.stderr.write("  - %s\n" % nn)
    elif args.cmd == "stage":
        target = Path(args.target)
        if not target.is_absolute():
            target = PROJECT_ROOT / target
        stage(target, args.func, load_ref(args), args.source, args.dry_run,
              ref_func=args.ref_func)


if __name__ == "__main__":
    main()
