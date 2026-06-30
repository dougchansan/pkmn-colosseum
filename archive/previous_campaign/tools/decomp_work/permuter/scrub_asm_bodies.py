#!/usr/bin/env python3
# scrub_asm_bodies.py — make a preprocessed-and-isolated permuter base.c ALWAYS
# parse with pycparser (C99) by reducing every function whose body still
# contains raw PowerPC assembly to a clean forward declaration.
#
# Why this exists
# ---------------
# build_dir.sh preprocesses the whole TU with `mwcceppc -E`, then runs
# strip_other_fns.py to reduce all-but-<target> to forward decls. But that is
# not enough: any sibling (or even the target) that is still an *asm wrapper*
# leaks raw PPC asm into base.c, e.g.
#
#     void fn_801174F4(void) {
#     nofralloc
#     stwu r1, -0x30(r1)
#     ...
#     }
#
# or an inline `__asm { li r3, 0x4 ... }` block inside an otherwise-C function.
# pycparser cannot parse `stwu`, `li r3`, `nofralloc`, `mtspr`, etc., so the
# permuter reports "Syntax error in base.c", evaluates zero candidates, and
# prints a degenerate NOWIN.
#
# This pass guarantees the end state required by the harness:
#   * base.c parses, and
#   * the target function is the ONLY real definition (every other definition
#     whose body still contains asm becomes `;`).
#
# Strategy
# --------
# Walk top-level function DEFINITIONS (a signature immediately followed by a
# `{...}` body). For each, brace-match its body. If the body text contains any
# PPC asm marker (a mnemonic at statement position, `nofralloc`, or an `__asm`
# block), collapse the whole definition to its signature + `;` (a forward
# declaration). The target function is kept intact UNLESS --force-target is
# given (used when the target itself has no usable C candidate, so that base.c
# can still parse and report a clean baseline instead of a syntax error).
#
# Usage:
#   scrub_asm_bodies.py <file.c> <target_fn> [--force-target]
# Rewrites <file.c> in place. Prints a one-line summary to stderr.

import re
import sys
from pathlib import Path

# PPC mnemonics that, when appearing as the first token of a statement line
# inside a function body, mark that body as raw assembly. Kept conservative so
# we never misfire on real C identifiers (these are matched only at the very
# start of a stripped line and followed by a register/immediate/label operand).
_ASM_MNEMONICS = (
    "stwu", "stw", "stmw", "sth", "stb", "stfd", "stfs", "stwx", "sthx",
    "lwz", "lwzu", "lwzx", "lhz", "lha", "lbz", "lmw", "lfd", "lfs", "lis",
    "li", "la", "lwzx",
    "addi", "addic", "add", "addis", "subf", "subc", "subic", "neg", "mullw",
    "mr", "mflr", "mtlr", "mtctr", "mfctr", "mtspr", "mfspr", "mffs", "mtfsf",
    "rlwinm", "rlwimi", "clrlwi", "extsh", "extsb", "extrwi", "slwi", "srwi",
    "or", "ori", "oris", "and", "andi", "xor", "xoris", "nor",
    "cmpwi", "cmplwi", "cmpw", "cmplw", "crset", "crclr", "crxor",
    "b", "bl", "blr", "beq", "bne", "bge", "blt", "bgt", "ble", "bdnz",
    "bctrl", "bctr", "nofralloc", "fmr", "fadds", "fmadds", "fmuls", "fsubs",
    "frsp", "fctiwz", "psq_l", "psq_st", "dcbf", "dcbz", "sync", "isync",
)
# A line is an asm statement if, after stripping leading whitespace, it begins
# with one of the mnemonics followed by whitespace and then a register/number/
# label operand (r0-r31, f0-f31, a digit, '-', or '@'/'.L' local label) OR is a
# bare `nofralloc`/`blr`/`isync`/`sync`. This avoids matching C like `b = ...;`.
_OPERAND = r"(?:[rf]\d|\d|-|0x|@|\.L|cr|GQR|sp|GPR|\w+\s*[,(])"
_ASM_LINE_RE = re.compile(
    r"^\s*(?:"
    + "|".join(re.escape(m) for m in _ASM_MNEMONICS)
    + r")\b(?:\s+" + _OPERAND + r"|\s*$)",
    re.M,
)
# `__asm` / `asm` inline-asm block markers and the MWCC `nofralloc` directive.
_INLINE_ASM_RE = re.compile(r"(^|[^\w])(?:__asm|nofralloc)\b")


def _find_brace_end(s: str, open_idx: int) -> int:
    """Return index of the matching '}' for the '{' at open_idx."""
    assert s[open_idx] == "{"
    level = 0
    i = open_idx
    n = len(s)
    while i < n:
        c = s[i]
        if c == "{":
            level += 1
        elif c == "}":
            level -= 1
            if level == 0:
                return i
        i += 1
    raise ValueError("unbalanced braces")


# A function header: optional leading whitespace, then a type/qualifier run that
# is NOT a control keyword, then `name(...)` and a `{`. We locate candidate '{'
# at column 0-ish by scanning for "<ident>(" ... ")" "{" patterns.
_HEADER_RE = re.compile(
    r"(?P<head>(?:^|\n)[ \t]*"          # line start
    r"(?:[A-Za-z_][\w\s\*]*?\s|\*)?"     # return type / qualifiers (optional)
    r"\*?(?P<name>[A-Za-z_]\w*)\s*"      # function name
    r"\([^;{}]*\)\s*)"                   # parameter list (no ; { } inside)
    r"\{",                               # opening brace
)

_CONTROL_KW = {"if", "for", "while", "switch", "do", "else", "return", "sizeof"}


def body_is_asm(body: str) -> bool:
    if _INLINE_ASM_RE.search(body):
        return True
    return bool(_ASM_LINE_RE.search(body))


def scrub(source: str, target: str, force_target: bool) -> tuple[str, int, bool]:
    out = []
    i = 0
    n = len(source)
    reduced = 0
    target_reduced = False
    while i < n:
        m = _HEADER_RE.search(source, i)
        if m is None:
            out.append(source[i:])
            break
        name = m.group("name")
        if name in _CONTROL_KW:
            # not a function definition (e.g. `if (...) {`) — emit up to and
            # including this brace, continue scanning after it.
            out.append(source[i : m.end()])
            i = m.end()
            continue
        brace_idx = m.end() - 1
        try:
            end = _find_brace_end(source, brace_idx)
        except ValueError:
            out.append(source[i:])
            break
        head = m.group("head")
        body = source[brace_idx : end + 1]
        # Emit verbatim everything between the previous cursor and the start of
        # this definition's header (externs, type decls, forward decls, etc.).
        out.append(source[i : m.start("head")])
        is_target = name == target
        # Keep the target as a real definition ONLY if its body is real C. If
        # the target body still contains asm (no C candidate exists in source,
        # or it is an inline __asm{} block), reduce it too so base.c at least
        # parses and the permuter reports a clean baseline instead of a syntax
        # error. `force_target` forces reduction even of a C target (unused by
        # the normal pipeline; handy for diagnostics).
        keep_body = is_target and not force_target and not body_is_asm(body)
        if keep_body:
            # Target stays a real definition. Drop a leading `asm` qualifier in
            # case the kept body is itself a wrapper the caller chose to keep.
            out.append(re.sub(r"\basm\b\s*", "", head, count=1) + body)
        else:
            # Reduce to a forward declaration: signature + ';'. Strip a leading
            # `asm ` qualifier so pycparser (C99) accepts the decl.
            decl = re.sub(r"\basm\b\s*", "", head, count=1).rstrip()
            if decl.endswith("{"):
                decl = decl[:-1].rstrip()
            out.append(decl + ";")
            if body_is_asm(body):
                reduced += 1
                if is_target:
                    target_reduced = True
        i = end + 1
    return "".join(out), reduced, target_reduced


def main() -> None:
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    flags = {a for a in sys.argv[1:] if a.startswith("--")}
    if len(args) != 2:
        sys.exit("usage: scrub_asm_bodies.py <file.c> <target_fn> [--force-target]")
    path = Path(args[0])
    target = args[1]
    force = "--force-target" in flags
    src = path.read_text(encoding="utf-8", errors="replace")
    out, reduced, target_reduced = scrub(src, target, force)
    path.write_text(out, encoding="utf-8")
    note = f"scrub_asm_bodies: reduced {reduced} asm-bodied fn(s) to forward decls"
    if target_reduced:
        note += f"; WARNING target {target} had no usable C body (reduced too)"
    print(note, file=sys.stderr)


if __name__ == "__main__":
    main()
