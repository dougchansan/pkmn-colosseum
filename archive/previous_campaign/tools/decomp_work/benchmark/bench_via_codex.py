#!/usr/bin/env python3
"""bench_via_codex.py — Drive the Codex TUI pane through a benchmark suite.

Codex (configured for any backend model — currently gpt-5.5 xhigh) runs in tmux
pane `codex`. We can't easily call its API directly, so this driver uses a
**file-based inbox/outbox handoff**:

  1. For each test, write the prompt to .omc/codex_bench/inbox/<fn>.md
  2. Send Codex an instruction telling it to read inbox + write outbox
  3. Poll for outbox/<fn>.c to exist + be stable
  4. Read the answer, extract the C, score via remote_reward
  5. Save results in the same JSON shape as bench_compile_match.py

Usage:
    python tools/decomp_work/benchmark/bench_via_codex.py \
        --suite tools/decomp_work/benchmark/test_suite_cw_focus_expert.json \
        --only-stem gs_field_world \
        --label codex-gpt-5.5-xhigh
"""
from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import time
from pathlib import Path


def find_bash() -> str:
    """Locate a bash interpreter the Windows-native Python can actually exec."""
    for env_var in ("BASH", "GIT_BASH"):
        v = os.environ.get(env_var)
        if v and Path(v).exists():
            return v
    candidates = [
        r"C:\Program Files\Git\usr\bin\bash.exe",
        r"C:\Program Files\Git\bin\bash.exe",
        r"C:\Program Files (x86)\Git\bin\bash.exe",
    ]
    for c in candidates:
        if Path(c).exists():
            return c
    found = shutil.which("bash") or shutil.which("bash.exe")
    if found:
        return found
    raise RuntimeError("Could not locate bash.exe — set BASH=C:\\path\\to\\bash.exe in env")


BASH = find_bash()

# Imports from sibling bench scripts
import sys
sys.path.insert(0, str(Path(__file__).resolve().parent))
from bench_opencode import extract_c, normalize  # noqa: E402
from bench_compile_match import remote_reward, RESULTS_DIR  # noqa: E402


REPO = Path(__file__).resolve().parents[3]
DECOMP_WORK = REPO / "tools" / "decomp_work"
TMUX_CONTROL = DECOMP_WORK / "tmux_control"
IO_ROOT = REPO / ".omc" / "codex_bench"
INBOX = IO_ROOT / "inbox"
OUTBOX = IO_ROOT / "outbox"


def ensure_dirs() -> None:
    INBOX.mkdir(parents=True, exist_ok=True)
    OUTBOX.mkdir(parents=True, exist_ok=True)


def send_codex(text: str, verbose: bool = True) -> bool:
    """Send a prompt to the Codex TUI pane. Returns True if confirmation succeeded."""
    script = TMUX_CONTROL / "send_to_codex_tui.sh"
    r = subprocess.run(
        [BASH, str(script), text],
        capture_output=True, text=True, check=False,
    )
    ok = r.returncode == 0 and "Sent to Codex TUI:" in (r.stdout or "")
    if verbose:
        if ok:
            print(f"      send-codex OK ({len(text)} chars): {r.stdout.strip().splitlines()[-1]}")
        else:
            print(f"      send-codex FAIL rc={r.returncode}")
            print(f"        stdout: {(r.stdout or '').strip()[:200]}")
            print(f"        stderr: {(r.stderr or '').strip()[:200]}")
    return ok


def codex_idle() -> bool:
    r = subprocess.run(
        [BASH, str(TMUX_CONTROL / "codex_is_idle.sh"), "--quiet"],
        capture_output=True,
    )
    return r.returncode == 0


def wait_for_outbox(out_path: Path, timeout_s: int = 600, stable_s: int = 3) -> bool:
    """Wait for `out_path` to exist + have non-zero size + size stable for N seconds."""
    deadline = time.time() + timeout_s
    last_size = -1
    last_change = time.time()
    while time.time() < deadline:
        if out_path.exists():
            sz = out_path.stat().st_size
            if sz > 0 and sz == last_size and (time.time() - last_change) >= stable_s:
                return True
            if sz != last_size:
                last_size = sz
                last_change = time.time()
        time.sleep(2)
    return False


def build_codex_instruction(fn: str, in_rel: str, out_rel: str) -> str:
    return (
        f"Decomp task: read the prompt at `{in_rel}` and write your C answer to `{out_rel}`.\n"
        "Output ONLY the matching C function with required externs and pragma wrappers; do "
        "not output assembly, do not output explanations, do not output #if/#else blocks. "
        f"After writing the file, reply with the single word 'DONE-{fn}'."
    )


def benchmark_codex(suite: list[dict], label: str) -> dict:
    ensure_dirs()
    results = []
    total_time = 0.0

    print(f"\n{'=' * 60}\nCodex benchmarking: {label}\n{'=' * 60}")

    for i, test in enumerate(suite):
        fn = test["function"]
        stem = Path(test["file"]).stem
        prompt = test["prompt"]

        in_path = INBOX / f"{stem}_{fn}.md"
        out_path = OUTBOX / f"{stem}_{fn}.c"
        in_rel = in_path.relative_to(REPO).as_posix()
        out_rel = out_path.relative_to(REPO).as_posix()

        out_path.unlink(missing_ok=True)
        in_path.write_text(prompt, encoding="utf-8")

        print(f"  [{i + 1}/{len(suite)}] {stem}:{fn}...", end=" ", flush=True)

        send_codex(build_codex_instruction(fn, in_rel, out_rel))
        start = time.time()
        ok_outbox = wait_for_outbox(out_path, timeout_s=600, stable_s=4)
        elapsed = time.time() - start
        total_time += elapsed

        result = {
            "function": fn,
            "stem": stem,
            "time": round(elapsed, 3),
            "status": "FAIL",
            "extracted": False,
            "compile_ok": False,
            "match_pct": 0.0,
            "reward": 0.0,
            "response_preview": "",
        }

        if not ok_outbox:
            result["response_preview"] = "TIMEOUT — outbox file never appeared"
            results.append(result)
            print(f"timeout ({elapsed:.0f}s)")
            continue

        raw = out_path.read_text(encoding="utf-8", errors="replace")
        result["response_preview"] = raw[:500]
        # Codex writes a pure .c file — only run extract_c if the output is
        # accidentally markdown-wrapped. Never use the second regex (it has a
        # non-greedy `{.*?}` that truncates at the first nested `}` in switch
        # statements, producing unbalanced code).
        code = None
        if "```" in raw:
            m = re.search(r"```c?\s*\n(.*?)```", raw, re.DOTALL)
            if m:
                code = m.group(1).strip()
        if code is None:
            code = raw.strip()
        if code:
            code = normalize(code)
            result["extracted"] = True
            reward, pct, ok, details = remote_reward(stem, fn, code)
            result["compile_ok"] = ok
            result["match_pct"] = pct
            result["reward"] = reward
            result["details"] = details[:500] if details else ""
            result["code"] = code
            if ok and pct > 0.0:
                result["status"] = "PASS"

        results.append(result)
        print(f"{result['match_pct']:.1f}% (compile={result['compile_ok']}, {elapsed:.0f}s)")

    return {"model": label, "results": results, "total_time": round(total_time, 3)}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--suite", required=True)
    ap.add_argument("--only-stem", default=None)
    ap.add_argument("--only-fn", default=None, help="Comma-separated list of fn_XXXX to filter to")
    ap.add_argument("--label", default="codex/gpt-5.5-xhigh")
    args = ap.parse_args()

    suite_path = Path(args.suite).resolve()
    suite = json.loads(suite_path.read_text(encoding="utf-8"))
    suite = [t for t in suite if t["file"].startswith("src/game/")]
    if args.only_stem:
        suite = [t for t in suite if Path(t["file"]).stem == args.only_stem]
    if args.only_fn:
        wanted = {f.strip() for f in args.only_fn.split(",") if f.strip()}
        suite = [t for t in suite if t["function"] in wanted]

    print(f"Codex bench suite: {suite_path} ({len(suite)} tests)")
    print(f"Label: {args.label}")

    out = benchmark_codex(suite, args.label)
    safe_label = args.label.replace("/", "_").replace(":", "_")
    out_path = RESULTS_DIR / f"compile_match__{suite_path.stem}__{safe_label}.json"
    out_path.write_text(json.dumps(out, indent=2) + "\n", encoding="utf-8")
    passes = sum(1 for r in out["results"] if r["status"] == "PASS")
    print(f"\nResult: {passes}/{len(out['results'])} PASS, total={out['total_time']:.0f}s")
    print(f"Saved: {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
