#!/usr/bin/env python3
"""
bench_compile_match.py - Benchmark models with real compile+match scoring.

This uses the existing provider/model routing from bench_opencode.py, but scores
responses by sending extracted C to the remote Linux repo's reward_fn.py.

Usage:
    python bench_compile_match.py [--suite path/to/suite.json] model1 [model2 ...]

The default suite is the compact CW-focused suite. Only entries whose source file
stem matches the remote reward path shape (src/game/<stem>.c) are scored.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
import time
from pathlib import Path

from bench_opencode import extract_c, normalize, run_model


BENCH_DIR = Path(__file__).resolve().parent
DEFAULT_SUITE = BENCH_DIR / "test_suite_cw_focus_compact.json"
RESULTS_DIR = BENCH_DIR / "results"

REMOTE_HOST = os.environ.get(
    "DECOMP_GPU_SSH",
    f"douglaswhittingham@{os.environ.get('DECOMP_GPU_HOST', '192.168.50.101')}",
)
REMOTE_REPO = "/storage/finetune/pkmn-colosseum"
REMOTE_TMP_DIR = "/storage/finetune/tmp/compile_match_bench"


def parse_reward_stdout(stdout: str) -> tuple[float, float, bool]:
    match = re.search(r"reward=([0-9.]+)\s+pct=([0-9.]+)%\s+ok=(True|False)", stdout)
    if not match:
        return 0.0, 0.0, False
    reward = float(match.group(1))
    pct = float(match.group(2))
    ok = match.group(3) == "True"
    return reward, pct, ok


def remote_reward(stem: str, fn: str, c_code: str) -> tuple[float, float, bool, str]:
    with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False, encoding="utf-8") as tmp:
        tmp.write(c_code)
        local_path = Path(tmp.name)

    remote_path = f"{REMOTE_TMP_DIR}/{local_path.name}"
    try:
        mkdir_cmd = ["ssh", REMOTE_HOST, f"mkdir -p {REMOTE_TMP_DIR}"]
        mkdir_proc = subprocess.run(mkdir_cmd, capture_output=True, text=True, timeout=30)
        if mkdir_proc.returncode != 0:
            return 0.0, 0.0, False, mkdir_proc.stderr.strip() or mkdir_proc.stdout.strip()

        scp_cmd = ["scp", str(local_path), f"{REMOTE_HOST}:{remote_path}"]
        scp_proc = subprocess.run(scp_cmd, capture_output=True, text=True, timeout=60)
        if scp_proc.returncode != 0:
            return 0.0, 0.0, False, scp_proc.stderr.strip() or scp_proc.stdout.strip()

        ssh_cmd = [
            "ssh",
            REMOTE_HOST,
            (
                f"cd {REMOTE_REPO} && "
                f"python3 tools/decomp_work/rl/reward_fn.py {stem} {fn} {remote_path}"
            ),
        ]
        reward_proc = subprocess.run(ssh_cmd, capture_output=True, text=True, timeout=240)
        stdout = reward_proc.stdout.strip()
        stderr = reward_proc.stderr.strip()
        reward, pct, ok = parse_reward_stdout(stdout)
        details = stdout if stdout else stderr
        return reward, pct, ok, details
    finally:
        cleanup_cmd = ["ssh", REMOTE_HOST, f"rm -f {remote_path}"]
        subprocess.run(cleanup_cmd, capture_output=True, text=True, timeout=30)
        local_path.unlink(missing_ok=True)


def benchmark_model(model_spec: str, suite: list[dict], max_tokens: int = 1024) -> dict:
    print(f"\n{'=' * 60}")
    print(f"Compile+match benchmarking: {model_spec}  (max_tokens={max_tokens})")
    print(f"{'=' * 60}")

    results = []
    total_time = 0.0

    for i, test in enumerate(suite):
        fn = test["function"]
        stem = Path(test["file"]).stem
        prompt = test["prompt"]

        print(f"  [{i + 1}/{len(suite)}] {stem}:{fn}...", end=" ", flush=True)
        start = time.time()
        response, elapsed = run_model(model_spec, prompt, max_tokens=max_tokens)
        total_time += elapsed

        c_code = extract_c(response)
        result = {
            "function": fn,
            "stem": stem,
            "time": round(elapsed, 3),
            "status": "FAIL",
            "extracted": False,
            "compile_ok": False,
            "match_pct": 0.0,
            "reward": 0.0,
            "response_preview": response[:500] if response else "",
        }

        if c_code:
            result["extracted"] = True
            c_code = normalize(c_code)
            reward, pct, ok, details = remote_reward(stem, fn, c_code)
            result["compile_ok"] = ok
            result["match_pct"] = pct
            result["reward"] = reward
            result["details"] = details[:500] if details else ""
            result["code"] = c_code
            if ok and pct > 0.0:
                result["status"] = "PASS"

        results.append(result)
        if result["extracted"]:
            print(f"{result['match_pct']:.1f}% (compile={result['compile_ok']}, {elapsed:.1f}s)")
        else:
            print(f"no-code ({elapsed:.1f}s)")

    return {
        "model": model_spec,
        "results": results,
        "total_time": round(total_time, 3),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Benchmark models with compile+match scoring")
    parser.add_argument("--suite", default=str(DEFAULT_SUITE), help="Suite JSON to use")
    parser.add_argument("--only-stem", help="Only score functions whose source file stem matches this value")
    parser.add_argument("--max-tokens", type=int, default=1024, help="Max output tokens per call (default 1024). Bump for large/complex functions.")
    parser.add_argument("models", nargs="+", help="provider/model specs")
    args = parser.parse_args()

    suite_path = Path(args.suite).resolve()
    suite = json.loads(suite_path.read_text(encoding="utf-8"))
    suite = [item for item in suite if item["file"].startswith("src/game/")]
    if args.only_stem:
        suite = [item for item in suite if Path(item["file"]).stem == args.only_stem]

    RESULTS_DIR.mkdir(exist_ok=True)
    print(f"Compile+match suite: {suite_path} ({len(suite)} functions)")

    for model_spec in args.models:
        result = benchmark_model(model_spec, suite, max_tokens=args.max_tokens)
        safe_name = model_spec.replace("/", "_").replace(":", "_")
        out_path = RESULTS_DIR / f"compile_match__{suite_path.stem}__{safe_name}.json"
        out_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        passes = sum(1 for r in result["results"] if r["status"] == "PASS")
        print(f"\n  Result: {passes}/{len(result['results'])} PASS, total={result['total_time']:.1f}s")
        print(f"  Saved: {out_path}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
