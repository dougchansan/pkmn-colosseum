#!/usr/bin/env python3
"""verify_gate.py - tamper-evident merge gate for a candidate decomp match.

The decomp has a recurring FRAUD class. Agents (and humans) have forged 100%
matches by:

  (a) EDITING THE TRUTH: hand-editing the per-function ``*_fn_*.inc`` assembly
      files (or objdiff config / build scripts) so the comparison passes;
  (b) REACTIVATING THE WRAPPER: flipping ``#if 0`` -> ``#if 1`` so the original
      ``asm void`` wrapper compiles back in -- the function then "matches"
      because it literally *is* the target assembly, not real C;
  (c) hallucinating a match percentage that nobody re-measures.

This gate makes (a) and (b) STRUCTURALLY unmergeable instead of relying on a
reviewer to notice. It is meant to run on a candidate change (a git ref range
and/or a specific function) BEFORE the change is trusted or merged.

Order of checks (fail fast, fraud first):
  1. DIFF SCOPE: ``git diff --name-only`` over the range. HARD-FAIL if ANY
     changed file is outside ``src/**.c``. This explicitly rejects edits to
     ``*.inc`` (assembly truth), ``objdiff`` configs, build scripts, and
     headers -- the surfaces used to forge matches.
  2. WRAPPER REACTIVATION: scan the touched ``.c`` files for an ``#if 1``
     immediately followed by an ``asm void`` wrapper. HARD-FAIL on any hit.
  3. MATCH MEASUREMENT: only if 1 and 2 pass, report the objdiff match% (the
     ground-truth number) for the named function, or overall status.

A REJECT at step 1 or 2 means the change is fraudulent or out-of-scope by
construction; the match% is not even computed, because it would be meaningless.

Usage:
    python tools/verify_gate.py --range HEAD~1..HEAD
    python tools/verify_gate.py --range origin/master..HEAD --function fn_800A4D28
    python tools/verify_gate.py --function fn_800A4D28        # scope = uncommitted working tree
    python tools/verify_gate.py --files src/game/foo.c        # explicit file list (for tests)

Exit codes:
    0  PASS  (scope clean, no reactivated wrapper; match% reported if asked)
    1  REJECT (out-of-scope file, or reactivated asm wrapper detected)
    2  ERROR  (could not run git / build artifacts missing for measurement)
"""

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

# Only hand-written C under src/ may change in a real decomp match.
ALLOWED_RE = re.compile(r"^src/.+\.c$")

# Explicitly-named fraud surfaces, reported with a specific reason when hit.
INC_RE = re.compile(r".*\.inc$", re.IGNORECASE)
HEADER_RE = re.compile(r".*\.(h|hpp|inc\.h)$", re.IGNORECASE)
OBJDIFF_RE = re.compile(r"(^|/)(objdiff[^/]*\.(ya?ml|json|toml)|.*objdiff.*config.*)$",
                        re.IGNORECASE)
BUILDSCRIPT_RE = re.compile(
    r"(^|/)(configure\.py|build\.py|.*\.mk|[Mm]akefile|.*\.ninja|"
    r"config\.json|.*\.lcf|.*\.ld)$")

# #if 1 immediately (next non-blank line) followed by an asm void wrapper.
# This is the signature of a reactivated assembly wrapper.
REACTIVATED_RE = re.compile(
    r"#if\s+1\s*(?://[^\n]*)?\n\s*(?:#[^\n]*\n\s*)*asm\s+void\s+\w+",
    re.MULTILINE,
)


def _git(args: list[str]) -> tuple[int, str, str]:
    """Run a git command, retrying once (git on PATH is flaky on this box)."""
    cmd = ["git", "-C", str(ROOT)] + args
    last = None
    for _ in range(2):
        try:
            p = subprocess.run(cmd, capture_output=True, text=True)
            return p.returncode, p.stdout, p.stderr
        except OSError as e:  # git not found / spawn failure
            last = e
    return 127, "", f"git invocation failed: {last}"


def changed_files(ref_range: str | None) -> tuple[list[str], str | None]:
    """Return (posix-relative paths, error). No range => uncommitted working tree."""
    if ref_range:
        args = ["diff", "--name-only", ref_range]
    else:
        # Working tree vs HEAD, including staged; covers an in-progress change.
        args = ["diff", "--name-only", "HEAD"]
    rc, out, err = _git(args)
    if rc != 0:
        return [], err.strip() or f"git diff failed (rc={rc})"
    files = [ln.strip().replace("\\", "/") for ln in out.splitlines() if ln.strip()]
    return files, None


def classify_offender(path: str) -> str:
    """Give a precise reason a non-allowed path is rejected."""
    if INC_RE.match(path):
        return "assembly truth file (*.inc) -- editing this forges the comparison"
    if OBJDIFF_RE.search(path):
        return "objdiff config -- editing this forges the comparison"
    if BUILDSCRIPT_RE.search(path):
        return "build script -- editing this can forge the comparison"
    if HEADER_RE.match(path):
        return "header file -- a real match changes only src/**.c"
    return "outside src/**.c -- a real match changes only hand-written C"


def check_scope(files: list[str]) -> tuple[bool, list[str], list[str]]:
    """Return (ok, c_files, reasons). ok=False if any file is out of scope."""
    c_files, reasons = [], []
    for f in files:
        if ALLOWED_RE.match(f):
            c_files.append(f)
        else:
            reasons.append(f"  REJECT  {f}\n            -> {classify_offender(f)}")
    return (len(reasons) == 0), c_files, reasons


def _added_if1_linenos(rel: str, ref_range: str | None) -> set[int] | None:
    """Return post-image line numbers of `#if 1` lines ADDED by the diff.

    Parses unified-diff hunk headers (@@ -a,b +c,d @@) and walks added (`+`)
    lines, tracking the new-file line number. Because the literal text of an
    `#if 1` line is identical for every wrapper, we must match on POSITION,
    not content -- otherwise a newly flipped wrapper would tar every
    pre-existing pending wrapper in the same file.

    Returns None when no diff is available (e.g. explicit --files in tests),
    signalling the caller to fall back to a whole-file scan.
    """
    args = ["diff", "--unified=0"]
    args += [ref_range] if ref_range else ["HEAD"]
    args += ["--", rel]
    rc, out, _ = _git(args)
    if rc != 0:
        return None
    added = set()
    new_ln = 0
    for ln in out.splitlines():
        if ln.startswith("@@"):
            m = re.search(r"\+(\d+)", ln)
            new_ln = int(m.group(1)) if m else 0
            continue
        if ln.startswith("+++") or ln.startswith("---"):
            continue
        if ln.startswith("+"):
            body = ln[1:].strip()
            if re.match(r"#if\s+1\b", body):
                added.add(new_ln)
            new_ln += 1
        elif ln.startswith("-"):
            continue  # removed line: does not advance new-file counter
        else:
            new_ln += 1  # context line (rare with -U0)
    return added


def check_reactivation(c_files: list[str], ref_range: str | None,
                       diff_aware: bool) -> list[str]:
    """Return REJECT reasons for reactivated asm wrappers.

    When diff_aware, an `#if 1 + asm void` pair is only flagged if the diff
    actually ADDED that `#if 1` line -- i.e. a wrapper was flipped back on.
    Pre-existing, still-pending wrappers in a touched file are NOT flagged
    (they are the normal state of an in-progress decomp). When not diff_aware
    (explicit file list with no git history), every reactivated pair in file
    content is flagged.
    """
    hits = []
    for rel in c_files:
        p = ROOT / rel
        if not p.exists():
            continue
        added_if1 = _added_if1_linenos(rel, ref_range) if diff_aware else None
        text = p.read_text(encoding="utf-8", errors="replace")
        for m in REACTIVATED_RE.finditer(text):
            block = m.group(0)
            # Line number of the `#if 1` line itself (start of the match).
            if1_line = text[: m.start()].count("\n") + 1
            if added_if1 is not None and if1_line not in added_if1:
                # Pre-existing pending wrapper, not flipped by this diff: skip.
                continue
            name = re.search(r"asm\s+void\s+(\w+)", block)
            fn = name.group(1) if name else "?"
            hits.append(f"  REJECT  {rel}:{if1_line}  "
                        f"reactivated asm wrapper (#if 1 + asm void {fn})")
    return hits


def report_match(function: str | None) -> tuple[int, str]:
    """Report ground-truth objdiff match%. Returns (rc, message)."""
    try:
        import progress
        import measure_cache
    except Exception as e:  # pragma: no cover
        return 2, f"could not import measure path: {e}"

    if not progress.TARGET_O.exists() or not progress.BASE_DIR.exists():
        return 2, ("build artifacts missing; cannot measure match%. "
                   "Build the project, then re-run.")

    if function:
        found = None
        for o in sorted(progress.BASE_DIR.rglob("*.o")):
            funcs = measure_cache.diff_funcs(progress.TARGET_O, o)
            if not funcs:
                continue
            for fdata in funcs:
                if fdata["name"] == function:
                    found = fdata
                    break
            if found:
                break
        measure_cache.flush()
        if found is None:
            return 2, f"function {function} not found in any base object"
        pct = found["match"]
        verdict = "100% MATCH" if pct >= 100 else f"{pct:.1f}% (NOT matching)"
        return 0, f"objdiff match for {function}: {verdict}"

    files = progress.collect()
    overall = progress.summarize(files)
    measure_cache.flush()
    return 0, (f"overall objdiff: "
               f"{overall['matched_functions']}/{overall['total_functions']} "
               f"functions ({overall['match_pct']:.2f}%), "
               f"{overall['bytes_pct']:.2f}% bytes")


def run_gate(ref_range, function, explicit_files):
    """Core gate logic, returns (exit_code, lines). Importable for tests."""
    lines = []

    if explicit_files is not None:
        files = [f.replace("\\", "/") for f in explicit_files]
        err = None
    else:
        files, err = changed_files(ref_range)
    if err:
        return 2, [f"ERROR: {err}"]

    if not files:
        lines.append("No changed files in scope.")

    # Step 1: diff scope.
    scope_ok, c_files, scope_reasons = check_scope(files)
    if not scope_ok:
        lines.append("REJECT: change touches files outside src/**.c")
        lines.extend(scope_reasons)
        lines.append("\nA legitimate decomp match modifies ONLY hand-written "
                     "C under src/. The above surfaces are how matches get "
                     "forged. Rejected without measuring.")
        return 1, lines

    # Step 2: wrapper reactivation. Diff-aware in git mode (flag only newly
    # flipped #if 1 wrappers, not pre-existing pending ones); full-content
    # scan when given an explicit file list with no diff (tests).
    diff_aware = explicit_files is None
    react = check_reactivation(c_files, ref_range, diff_aware)
    if react:
        lines.append("REJECT: reactivated assembly wrapper(s) detected")
        lines.extend(react)
        lines.append("\n#if 1 directly enabling an `asm void` wrapper makes the "
                     "function 'match' because it IS the original assembly. "
                     "This is fraud, not a decompilation. Rejected without "
                     "measuring.")
        return 1, lines

    lines.append("PASS (structural): "
                 f"{len(c_files)} src/**.c file(s), no out-of-scope edits, "
                 "no reactivated wrappers.")

    # Step 3: measure (only reached when structural checks pass).
    if function or explicit_files is None:
        rc, msg = report_match(function)
        lines.append(msg)
        if rc == 2:
            # Measurement unavailable is not a fraud REJECT; surface as note,
            # keep overall PASS for the structural gate.
            lines.append("(note: structural checks PASSED; match% unavailable)")
            return 0, lines
    return 0, lines


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--range", dest="ref_range",
                    help="git ref range, e.g. origin/master..HEAD")
    ap.add_argument("--function", help="function name to measure after gating")
    ap.add_argument("--files", nargs="+",
                    help="explicit file list instead of git diff (for tests)")
    args = ap.parse_args()

    rc, lines = run_gate(args.ref_range, args.function, args.files)
    print("\n".join(lines))
    verdict = {0: "PASS", 1: "REJECT", 2: "ERROR"}[rc]
    print(f"\n=== verify_gate: {verdict} ===")
    return rc


if __name__ == "__main__":
    sys.exit(main())
