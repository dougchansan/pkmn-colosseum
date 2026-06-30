#!/usr/bin/env python3
"""reward_fn.py — Compile a proposed C body + measure CW match%.

Called by the RL training loop. Takes: (stem, fn, candidate_c) → (reward in [0,1], match_pct, ok_bool).

- Swaps candidate_c into src/game/<stem>.c inside the correct #else block
- Forces fresh compile: rm build/GC6E01/base/game/<stem>.o
- Compiles via tools/compile_check.py
- Measures via tools/match_scan_file.py
- Reverts the file (git checkout) regardless of outcome
- Returns reward = match_pct / 100.0, or 0.0 on compile fail

Uses a lock to serialize access to the shared src tree so parallel rollouts
don't clobber each other.
"""
from __future__ import annotations

import fcntl
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import Tuple

REPO_ROOT = Path(__file__).resolve().parent.parent.parent.parent
LOCK_PATH = REPO_ROOT / "tools" / "decomp_work" / "rl" / ".reward.lock"
LOCK_PATH.parent.mkdir(parents=True, exist_ok=True)


def _acquire_lock():
    f = open(LOCK_PATH, "w")
    fcntl.flock(f.fileno(), fcntl.LOCK_EX)
    return f


def _release_lock(f):
    fcntl.flock(f.fileno(), fcntl.LOCK_UN)
    f.close()


def _swap_else_block(stem: str, fn: str, c_body: str) -> bool:
    """Replace the #else block for a given fn with c_body. Returns True if swapped."""
    c_path = REPO_ROOT / "src" / "game" / f"{stem}.c"
    text = c_path.read_text(encoding="latin-1")

    pattern = re.compile(
        r"(#if 1\s*\nasm void " + re.escape(fn) + r"\(void\)\s*\{[^}]*\})\s*\n"
        r"(#else\n)(.*?)(#endif)",
        re.S
    )
    m = pattern.search(text)
    if not m:
        # Already-flipped: #if 0 active, #else block has real C
        pattern2 = re.compile(
            r"(#if 0\s*\nasm void " + re.escape(fn) + r"\(void\)\s*\{[^}]*\})\s*\n"
            r"(#else\n)(.*?)(#endif)",
            re.S
        )
        m = pattern2.search(text)
        if not m:
            return False
        new_text = text[:m.start()] + m.group(1) + "\n" + m.group(2) + c_body.rstrip() + "\n" + m.group(4) + text[m.end():]
    else:
        # Flip the #if 1 to #if 0 AND replace body
        flipped = m.group(1).replace("#if 1", "#if 0", 1)
        new_text = text[:m.start()] + flipped + "\n" + m.group(2) + c_body.rstrip() + "\n" + m.group(4) + text[m.end():]
    c_path.write_text(new_text, encoding="latin-1")
    return True


def _revert(stem: str, original_text: str | None = None) -> None:
    """Restore src/game/<stem>.c to its prior state.

    Prefers `git checkout` when the working tree is git-tracked; falls back
    to writing back the captured snapshot. The 3090 reward host is NOT a
    git repo, so the snapshot path is the durable one.
    """
    c_path = REPO_ROOT / "src" / "game" / f"{stem}.c"
    if (REPO_ROOT / ".git").exists():
        result = subprocess.run(
            ["git", "checkout", f"src/game/{stem}.c"],
            cwd=REPO_ROOT, capture_output=True
        )
        if result.returncode == 0:
            return
    if original_text is not None:
        c_path.write_text(original_text, encoding="latin-1")


def _compile(stem: str) -> bool:
    build_path = REPO_ROOT / "build" / "GC6E01" / "base" / "game" / f"{stem}.o"
    build_path.unlink(missing_ok=True)
    result = subprocess.run(
        ["python3", "tools/compile_check.py", f"src/game/{stem}.c"],
        cwd=REPO_ROOT, capture_output=True, text=True, timeout=120
    )
    return result.returncode == 0 and "OK:" in result.stdout


def _match_pct(stem: str, fn: str) -> float:
    result = subprocess.run(
        ["python3", "tools/match_scan_file.py", stem, fn],
        cwd=REPO_ROOT, capture_output=True, text=True, timeout=60
    )
    for line in result.stdout.splitlines():
        m = re.match(r"\s*([\d.]+)%\s+" + re.escape(fn), line)
        if m:
            return float(m.group(1))
    return 0.0


def compute_reward(stem: str, fn: str, c_body: str) -> Tuple[float, float, bool]:
    """Returns (reward, match_pct, ok). Reward in [0,1]. ok=False on compile failure."""
    lock = _acquire_lock()
    c_path = REPO_ROOT / "src" / "game" / f"{stem}.c"
    original_text = c_path.read_text(encoding="latin-1") if c_path.exists() else None
    try:
        try:
            swapped = _swap_else_block(stem, fn, c_body)
            if not swapped:
                return 0.0, 0.0, False
            ok = _compile(stem)
            if not ok:
                return 0.0, 0.0, False
            pct = _match_pct(stem, fn)
            return pct / 100.0, pct, True
        finally:
            # Always restore the .c file regardless of compile timeout/crash.
            # Without this, subprocess.TimeoutExpired (e.g. on a stuck CW
            # compile) leaves the working tree mutated and breaks the next
            # reward eval.
            _revert(stem, original_text)
    finally:
        _release_lock(lock)


if __name__ == "__main__":
    # CLI test: python3 reward_fn.py <stem> <fn> <c_body_file>
    if len(sys.argv) < 4:
        print(__doc__); sys.exit(2)
    body = Path(sys.argv[3]).read_text(encoding="utf-8")
    reward, pct, ok = compute_reward(sys.argv[1], sys.argv[2], body)
    print(f"reward={reward:.3f} pct={pct:.1f}% ok={ok}")
