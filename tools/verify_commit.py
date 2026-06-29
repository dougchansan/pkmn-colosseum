#!/usr/bin/env python3
"""verify_commit.py - anti-fraud gate for decomp commits.

Every wave this session produced the same recurring fraud/error modes that
had to be caught by hand before cherry-picking:

  - editing `*_fn_*.inc` (the ROM-truth bytes objdiff measures against)
  - editing other ROM-truth files (symbols.txt / target .o)
  - adding `#include "*.inc"` or inline asm inside source
  - flipping `#if 0` -> `#if 1` to re-activate an asm wrapper and forge 100%
  - adding raw cast-plus-offset pointer arithmetic as a "finished" decompilation
  - claiming a match% that does not re-measure on a clean build

This makes that vigilance a one-shot tool. Run it before cherry-picking a
subagent commit, and in CI.

Usage:
    # check the last commit on the current branch (vs its parent)
    python tools/verify_commit.py

    # check a range / specific commit
    python tools/verify_commit.py --range master..HEAD
    python tools/verify_commit.py --commit <sha>

    # also re-measure claimed results (compile + objdiff)
    python tools/verify_commit.py --measure src/game/gs_render.c:fn_800D56C0:100

Exit code 0 = clean, 1 = violation (prints what + why).
"""

import os
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
_EXE = ".exe" if os.name == "nt" else ""


def _first_existing(paths):
    for path in paths:
        if path.exists():
            return path
    return None


OBJDIFF = _first_existing([
    ROOT / "tools" / f"objdiff-cli{_EXE}",
    ROOT / "build" / "tools" / f"objdiff-cli{_EXE}",
]) or (ROOT / "tools" / f"objdiff-cli{_EXE}")
sys.path.insert(0, str(ROOT / "tools"))
from headless_subprocess import run as run_tool  # noqa: E402

# Files a match-improvement commit may NEVER modify. `splits.txt` and
# `symbols.build.txt` are dtk-owned build metadata and may change when running
# the normal dtk split/update workflow.
TRUTH_DENY = [
    re.compile(r"_fn_[0-9A-Fa-f]+\.inc$"),
    re.compile(r"\.inc$"),
    re.compile(r"config/GC6E01/symbols\.txt$"),
    re.compile(r"config/GC6E01/link_order\.txt$"),
    re.compile(r"build/GC6E01/obj/.*\.o$"),
    re.compile(r"\.dol$"),
]


def git(*args):
    # bytes + utf-8/replace: source diffs contain non-cp1252 bytes that
    # would crash text=True decoding on Windows and silently pass the gate.
    r = subprocess.run(["git", "-C", str(ROOT), *args], capture_output=True)
    return r.stdout.decode("utf-8", errors="replace")


def changed_files(rng):
    out = git("diff", "--name-only", rng)
    return [f.strip() for f in out.splitlines() if f.strip()]


def diff_text(rng):
    return git("diff", rng)


def check_truth_files(rng):
    """Flag truth files that are MODIFIED or ADDED — the forge case (editing the
    ROM-truth bytes to fake a match).

    DELETIONS and RENAMES are allowed: removing or moving a truth file cannot
    forge a match. In particular the dtk-template migration intentionally DELETES
    the committed per-function *.inc (copyright; regenerated from the ROM into
    gitignored build/ instead) — that is not tampering. `git diff --name-status`
    yields A / M / D / R<score> / C<score>; we reject only A and M.
    """
    out = git("diff", "--name-status", rng)
    bad = []
    for line in out.splitlines():
        parts = line.split("\t")
        if len(parts) < 2:
            continue
        status, path = parts[0], parts[-1]  # parts[-1] = new path for renames
        if status[:1] not in ("A", "M"):    # allow D (delete), R (rename), C (copy)
            continue
        for rx in TRUTH_DENY:
            if rx.search(path):
                bad.append(f"{status} {path}")
                break
    return bad


def _active_wrapper_re(fn):
    return re.compile(
        r"#if\s+1\s*(?://[^\n]*)?\n\s*(?:#[^\n]*\n\s*)*"
        r"asm\s+void\s+" + re.escape(fn) + r"\b",
        re.MULTILINE,
    )


def _parent_had_active_wrapper(parent, path, fn):
    if parent is None or path is None or fn is None:
        return False
    content = _file_at(parent, path)
    return bool(content and _active_wrapper_re(fn).search(content))


def check_asm_wrapper_flip(diff, parent=None):
    """Detect `#if 0` -> `#if 1` re-activation of an asm wrapper.

    Fraud signature: an added `#if 1` (or `#if 0` removed and `#if 1`
    added) within a few lines of an `asm ` wrapper or a `_fn_*.inc`
    include. If the parent already had the same active wrapper for the
    same function, do not flag it: large #else-body rewrites can make
    unchanged context lines appear as additions in Git's chosen hunk split.
    """
    lines = diff.splitlines()
    violations = []
    window = []
    path = None
    for i, ln in enumerate(lines):
        if ln.startswith("+++ b/"):
            path = ln[6:]
            continue
        if ln.startswith("+++ /dev/null"):
            path = None
            continue
        window = lines[max(0, i - 4):i + 5]
        if re.match(r"^\+\s*#if\s+1\b", ln):
            ctx = "\n".join(window)
            if ("asm " in ctx or re.search(r"_fn_[0-9A-Fa-f]+\.inc", ctx)
                    or re.search(r"^\+.*asm\s+\w+\s+fn_", ctx, re.M)):
                m = re.search(r"^[ +]\s*asm\s+void\s+(\w+)\b", ctx, re.M)
                fn = m.group(1) if m else None
                if _parent_had_active_wrapper(parent, path, fn):
                    continue
                where = f"{path}:{fn}: " if path and fn else ""
                violations.append(where + ln.strip())
        # also: removed `#if 0` paired with added `#if 1`
        if re.match(r"^-\s*#if\s+0\b", ln):
            nxt = "\n".join(lines[i:i + 6])
            if re.search(r"^\+\s*#if\s+1\b", nxt, re.M):
                violations.append(ln.strip() + "  -> #if 1")
    return violations


ADDED_INC_INCLUDE_RE = re.compile(r"^\+\s*#include\s+[<\"][^>\"]*\.inc[>\"]",
                                  re.IGNORECASE)
ADDED_INLINE_ASM_RE = re.compile(r"^\+.*\b(?:__asm|asm\s*\{|asm\s+void)\b")
RAW_POINTER_OFFSET_RE = [
    re.compile(
        r"\*\s*\(\s*(?:volatile\s+|const\s+)*[A-Za-z_]\w*(?:\s*\*|\s+)\s*\)"
        r"\s*\([^;\n]*\+\s*(?:0x[0-9A-Fa-f]+|\d+)"
    ),
    re.compile(
        r"\(\s*(?:u8|s8|char|void)\s*\*\s*\)\s*[A-Za-z_]\w*"
        r"\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)"
    ),
]


def check_added_source_fraud(diff):
    """Reject newly-added source lines that are not real readable C.

    Existing debt is tracked separately; this gate only inspects added lines in
    changed src/**/*.c files so it prevents new cheating/debt without making the
    current project unmergeable.
    """
    path = None
    violations = []
    for ln in diff.splitlines():
        if ln.startswith("+++ b/"):
            path = ln[6:]
            continue
        if ln.startswith("+++ /dev/null"):
            path = None
            continue
        if not (path and path.startswith("src/") and path.endswith(".c")):
            continue
        if not ln.startswith("+") or ln.startswith("+++"):
            continue
        body = ln[1:]
        stripped = body.strip()
        if not stripped or stripped.startswith("*") or stripped.startswith("//"):
            continue
        if ADDED_INC_INCLUDE_RE.match(ln):
            violations.append(f"{path}: added .inc include: {stripped}")
        elif ADDED_INLINE_ASM_RE.match(ln):
            violations.append(f"{path}: added inline/asm wrapper code: {stripped}")
        elif any(rx.search(body) for rx in RAW_POINTER_OFFSET_RE):
            violations.append(f"{path}: added raw pointer-offset C: {stripped}")
    return violations


def remeasure(spec):
    """spec = 'src/path.c:fn_NAME:CLAIMED' -> (ok, msg)."""
    import compile_check
    src, fn, claimed = spec.rsplit(":", 2)
    claimed = float(claimed)
    src_path = ROOT / src
    try:
        base_o = compile_check.compile_source(src_path)
    except SystemExit:
        return (False, f"{fn}: compile FAILED (claimed {claimed}%)")
    r = run_tool(
        [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True, cwd=str(ROOT))
    if r.returncode != 0:
        return (False, f"{fn}: objdiff failed")
    j = json.loads(r.stdout)
    actual = None
    for s in j.get("right", {}).get("symbols", []):
        if s.get("name") == fn:
            actual = s.get("match_percent", 0.0)
            break
    if actual is None:
        return (False, f"{fn}: not found in object")
    if actual + 0.01 < claimed:
        return (False, f"{fn}: claimed {claimed}% but measured "
                        f"{actual:.2f}% (HALLUCINATION)")
    return (True, f"{fn}: {actual:.2f}% >= claimed {claimed}% OK")


def _file_at(ref, path):
    """File contents at a git ref, or None if absent. Reads bytes and
    decodes utf-8/replace — source files contain non-cp1252 bytes
    (e.g. 0x81), and text=True would crash the decode on Windows,
    silently making every file un-checkable."""
    r = subprocess.run(["git", "-C", str(ROOT), "show", f"{ref}:{path}"],
                        capture_output=True)
    if r.returncode != 0:
        return None
    return r.stdout.decode("utf-8", errors="replace")


def _matched_in(content, path):
    """Isolated-compile `content` as `path`, return #fn_ symbols @100%
    (or None if it doesn't compile)."""
    import automatch
    tag = "vc_" + re.sub(r"\W", "_", path)
    m = automatch.measure_isolated(content, ROOT / path, None, tag)
    if m is None:
        return None
    return sum(1 for k, v in m.items()
               if k.startswith("fn_") and v >= 100.0)


def check_file_regression(parent, head):
    """For every changed src/**/*.c, recompile parent vs head and reject
    on a whole-file matched-count drop. This catches localized edits that
    fix the target fn but regress neighbours (the w2/scene_init class) —
    which --measure (claimed-fns-only) cannot see."""
    out = git("diff", "--name-only", parent, head)
    msgs = []
    for f in out.splitlines():
        f = f.strip()
        if not (f.startswith("src/") and f.endswith(".c")):
            continue
        pc = _file_at(parent, f)
        hc = _file_at(head, f)
        if pc is None or hc is None:
            continue  # added/removed file — nothing to regress
        pm = _matched_in(pc, f)
        hm = _matched_in(hc, f)
        if pm is None or hm is None:
            continue  # compile failure handled elsewhere
        tag = "OK" if hm >= pm else "REGRESSED"
        print(f"  [{tag}] {f}: {pm} -> {hm} @100%")
        if hm < pm:
            msgs.append(f"{f}: whole-file matched {pm}->{hm} "
                        f"(net -{pm - hm})")
    return msgs


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--range", help="git diff range (e.g. master..HEAD)")
    g.add_argument("--commit", help="single commit sha")
    ap.add_argument("--measure", action="append", default=[],
                    help="src.c:fn:CLAIMED%% - re-measure claim (repeatable)")
    ap.add_argument("--no-regression-check", action="store_true",
                    help="skip the whole-file non-regression rebuild "
                         "(default: enabled — recompiles each changed .c)")
    args = ap.parse_args()

    if args.commit:
        rng = f"{args.commit}~1..{args.commit}"
        parent, head = f"{args.commit}~1", args.commit
    elif args.range:
        rng = args.range
        parent, head = (rng.split("..", 1) + ["HEAD"])[:2]
    else:
        rng = "HEAD~1..HEAD"
        parent, head = "HEAD~1", "HEAD"

    files = changed_files(rng)
    diff = diff_text(rng)
    violations = []

    if not args.no_regression_check:
        violations += ["WHOLE-FILE REGRESSION: " + m
                       for m in check_file_regression(parent, head)]

    bad_truth = check_truth_files(rng)
    if bad_truth:
        violations.append("TRUTH-FILE EDIT (forbidden): "
                          + ", ".join(bad_truth))

    flips = check_asm_wrapper_flip(diff, parent)
    if flips:
        violations.append("ASM-WRAPPER #if 0->#if 1 FLIP: "
                          + " | ".join(flips[:5]))

    source_fraud = check_added_source_fraud(diff)
    if source_fraud:
        violations.append("SOURCE-FRAUD/READABILITY VIOLATION: "
                          + " | ".join(source_fraud[:8]))

    for spec in args.measure:
        ok, msg = remeasure(spec)
        print(("  OK   " if ok else "  FAIL ") + msg)
        if not ok:
            violations.append("MEASURE: " + msg)

    print(f"\n[verify] range {rng}: {len(files)} files changed")
    if violations:
        print("[verify] REJECTED:")
        for v in violations:
            print("  - " + v)
        return 1
    print("[verify] clean — no truth edits, no asm-flip, claims hold")
    return 0


if __name__ == "__main__":
    sys.exit(main())
