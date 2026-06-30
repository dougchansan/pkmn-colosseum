#!/usr/bin/env python3
"""test_verify_gate.py - synthetic self-test for the fraud gate.

Proves the gate REJECTS the two structural fraud classes and PASSES a clean
.c-only change. Runs offline: no git, no build artifacts required. It drives
verify_gate.run_gate() with explicit file lists and writes synthetic source
files into a temp dir under the repo (so the reactivation scanner can read
them via repo-relative paths).

Run:
    python tools/test_verify_gate.py
Exit 0 = all assertions held; exit 1 = a check regressed (the gate is unsafe).
"""

import os
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

import verify_gate  # noqa: E402

REACTIVATED_WRAPPER = """\
#pragma push
#if 1
asm void fn_DEADBEEF(void) {
    nofralloc
#include "asm/fn_DEADBEEF.inc"
}
#else
void fn_DEADBEEF(void) {}
#endif
#pragma pop
"""

CLEAN_DECOMP = """\
#include "global.h"

extern f32 lbl_804836A0;

f32 fn_DEADBEEF(void) {
    return lbl_804836A0;
}

#if 0
asm void fn_OLD(void) {
    nofralloc
#include "asm/fn_OLD.inc"
}
#endif
"""

PASSED = []
FAILED = []


def check(name, cond, detail=""):
    (PASSED if cond else FAILED).append(name)
    mark = "ok  " if cond else "FAIL"
    print(f"  [{mark}] {name}" + (f"  -- {detail}" if detail and not cond else ""))


FILE_WITH_PENDING = """\
#if 1
asm void fn_PENDING(void) {
    nofralloc
#include "asm/fn_PENDING.inc"
}
#endif

void fn_REAL(void) {
}
"""

FILE_FLIPPED = """\
#if 1
asm void fn_PENDING(void) {
    nofralloc
#include "asm/fn_PENDING.inc"
}
#endif

#if 1
asm void fn_FLIPPED(void) {
    nofralloc
#include "asm/fn_FLIPPED.inc"
}
#endif
"""


def _diff_aware_test():
    """Verify the diff-aware reactivation logic against a real temp git repo."""
    if not _git_available():
        print("  [skip] diff-aware test (git unavailable)")
        return
    import verify_gate as vg

    saved_root, saved_git = vg.ROOT, vg._git
    with tempfile.TemporaryDirectory() as gd:
        repo = Path(gd)
        _run(["git", "init", "-q"], repo)
        _run(["git", "config", "user.email", "t@t"], repo)
        _run(["git", "config", "user.name", "t"], repo)
        _run(["git", "config", "commit.gpgsign", "false"], repo)
        target = repo / "src" / "mod.c"
        target.parent.mkdir(parents=True, exist_ok=True)
        # Commit a file that already has a pending #if 1 wrapper + a real C fn.
        target.write_text(FILE_WITH_PENDING, encoding="utf-8")
        _run(["git", "add", "-A"], repo)
        _run(["git", "commit", "-q", "-m", "base"], repo)
        # Now flip fn_REAL into an asm wrapper (add a NEW #if 1 + asm void).
        target.write_text(FILE_FLIPPED, encoding="utf-8")
        _run(["git", "add", "-A"], repo)
        _run(["git", "commit", "-q", "-m", "flip"], repo)

        # Point verify_gate at the throwaway repo.
        vg.ROOT = repo
        vg._git = lambda a: _git_in(repo, a)
        try:
            rc, lines = vg.run_gate("HEAD~1..HEAD", None, None)
            out = "\n".join(lines)
            check("diff-aware: newly flipped wrapper is REJECTED",
                  rc == 1 and "fn_FLIPPED" in out, f"rc={rc}: {out}")
            check("diff-aware: pre-existing pending wrapper NOT flagged",
                  "fn_PENDING" not in out, out)
        finally:
            vg.ROOT, vg._git = saved_root, saved_git


def _git_available() -> bool:
    try:
        subprocess.run(["git", "--version"], capture_output=True)
        return True
    except OSError:
        return False


def _run(args, cwd):
    p = subprocess.run(args, cwd=str(cwd), capture_output=True, text=True)
    if p.returncode != 0:
        raise RuntimeError(
            f"{' '.join(args)} failed with {p.returncode}: {p.stderr or p.stdout}"
        )
    return p


def _git_in(repo, args):
    p = subprocess.run(["git", "-C", str(repo)] + args,
                       capture_output=True, text=True)
    return p.returncode, p.stdout, p.stderr


def main() -> int:
    print("verify_gate self-test")
    print("-" * 60)

    with tempfile.TemporaryDirectory(dir=str(ROOT)) as td:
        tmp = Path(td)
        reldir = tmp.relative_to(ROOT).as_posix()

        # --- Case 1: tampered .inc diff must REJECT (scope check) -----------
        inc_rel = f"src/{reldir}/fake_fn_800A4D28.inc"
        rc, lines = verify_gate.run_gate(None, None, [inc_rel])
        out = "\n".join(lines)
        check("tampered .inc file is REJECTED", rc == 1, f"rc={rc}")
        check("reason names assembly truth file",
              "assembly truth file" in out, out)

        # objdiff config edit must REJECT
        rc, _ = verify_gate.run_gate(None, None, ["config/objdiff.json"])
        check("objdiff config edit is REJECTED", rc == 1, f"rc={rc}")

        # build script edit must REJECT
        rc, _ = verify_gate.run_gate(None, None, ["configure.py"])
        check("build script edit is REJECTED", rc == 1, f"rc={rc}")

        # header edit must REJECT
        rc, _ = verify_gate.run_gate(None, None, ["include/global.h"])
        check("header edit is REJECTED", rc == 1, f"rc={rc}")

        # --- Case 2: #if0 -> #if1 wrapper reactivation must REJECT ----------
        react_path = tmp / "reactivated.c"
        react_path.write_text(REACTIVATED_WRAPPER, encoding="utf-8")
        react_rel = f"src/{reldir}/reactivated.c"
        # Mirror it at the path the gate will read (must be under src/).
        # The scope check uses the path string; the scanner reads ROOT/path.
        real_react = ROOT / react_rel
        real_react.parent.mkdir(parents=True, exist_ok=True)
        real_react.write_text(REACTIVATED_WRAPPER, encoding="utf-8")
        try:
            rc, lines = verify_gate.run_gate(None, None, [react_rel])
            out = "\n".join(lines)
            check("#if0->#if1 reactivated wrapper is REJECTED", rc == 1, f"rc={rc}")
            check("reason names reactivated asm wrapper",
                  "reactivated asm wrapper" in out, out)

            # --- Case 3: clean .c-only change PASSES structural check -------
            clean_rel = f"src/{reldir}/clean.c"
            real_clean = ROOT / clean_rel
            real_clean.write_text(CLEAN_DECOMP, encoding="utf-8")
            try:
                # No --function, explicit files => structural-only, no measure.
                rc, lines = verify_gate.run_gate(None, None, [clean_rel])
                out = "\n".join(lines)
                check("clean src/**.c change PASSES structural check",
                      rc == 0, f"rc={rc}: {out}")
                check("clean change reports PASS (structural)",
                      "PASS (structural)" in out, out)
                check("clean change keeps #if 0 wrapper without tripping gate",
                      "REJECT" not in out, out)
            finally:
                real_clean.unlink(missing_ok=True)
        finally:
            real_react.unlink(missing_ok=True)

    # --- Case 4: diff-aware path -- a NEW #if0->#if1 flip is REJECTED, but a
    # PRE-EXISTING pending wrapper in a touched file is NOT (it is the normal
    # in-progress state). Uses a throwaway git repo with verify_gate pointed
    # at it. Skipped gracefully if git is unavailable.
    _diff_aware_test()

    print("-" * 60)
    print(f"{len(PASSED)} passed, {len(FAILED)} failed")
    if FAILED:
        print("FAILED:", ", ".join(FAILED))
        return 1
    print("ALL CHECKS PASSED -- fraud classes are structurally unmergeable.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
