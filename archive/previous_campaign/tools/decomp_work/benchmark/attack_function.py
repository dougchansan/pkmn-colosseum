#!/usr/bin/env python3
"""attack_function.py — Iterative multi-model attack on a single function.

Pipeline per function:
  1. Generate initial candidates from V4-flash, V4-pro, Kimi K2.6 in parallel
     (and optionally Codex via tmux — pass --use-codex)
  2. Score all candidates against the 3090 reward function
  3. Save full candidate list + scores to .omc/decomp_attack/<fn>.json
  4. (Optional) Build repair prompt from best, send to one or more models, score
  5. Print final leaderboard + best code

Doesn't auto-commit; intended to feed an interactive Opus loop.

Usage:
    python tools/decomp_work/benchmark/attack_function.py \
        --suite tools/decomp_work/benchmark/test_suite_gs_title_expert.json \
        --fn fn_80020F54
        [--use-codex] [--repair-rounds 2]
"""
from __future__ import annotations

import argparse
import json
import re
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

THIS_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(THIS_DIR))
from bench_opencode import run_model, normalize, normalize_model_spec  # noqa: E402
from bench_compile_match import remote_reward  # noqa: E402
from bench_via_codex import (  # noqa: E402
    INBOX, OUTBOX, ensure_dirs, send_codex, wait_for_outbox,
    build_codex_instruction,
)

REPO = Path(__file__).resolve().parents[3]
ATTACK_DIR = REPO / ".omc" / "decomp_attack"


CLOUD_MODELS = [
    "deepseek/deepseek-v4-flash",
    "deepseek/deepseek-v4-pro",
    "moonshot/kimi-k2.6",
]


def _extract_c_safely(raw: str) -> str:
    """Extract C from response — markdown-aware, doesn't truncate nested braces."""
    if raw.startswith(("HTTP_ERROR_", "URL_ERROR:", "CONFIG_ERROR:", "TIMEOUT", "ERROR:")):
        return ""
    m = re.search(r"```c?\s*\n(.*?)```", raw, re.DOTALL)
    if m:
        return m.group(1).strip()
    return raw.strip()


def gen_cloud(model: str, prompt: str, max_tokens: int) -> dict:
    start = time.time()
    routed_model = normalize_model_spec(model)
    raw, elapsed = run_model(routed_model, prompt, max_tokens=max_tokens)
    code = _extract_c_safely(raw)
    result = {
        "model": routed_model,
        "raw": raw,
        "code": normalize(code) if code else "",
        "elapsed": elapsed,
    }
    if routed_model != model:
        result["requested_model"] = model
    return result


def gen_codex(prompt: str, fn_label: str) -> dict:
    """Send prompt to Codex pane via inbox/outbox handoff."""
    ensure_dirs()
    in_path = INBOX / f"attack_{fn_label}.md"
    out_path = OUTBOX / f"attack_{fn_label}.c"
    in_path.write_text(prompt, encoding="utf-8")
    out_path.unlink(missing_ok=True)
    in_rel = in_path.relative_to(REPO).as_posix()
    out_rel = out_path.relative_to(REPO).as_posix()
    send_codex(build_codex_instruction(fn_label, in_rel, out_rel), verbose=False)
    start = time.time()
    ok = wait_for_outbox(out_path, timeout_s=300, stable_s=4)
    elapsed = time.time() - start
    raw = out_path.read_text(encoding="utf-8", errors="replace") if ok else ""
    code = _extract_c_safely(raw) if raw else ""
    return {
        "model": "codex/gpt-5.5-xhigh",
        "raw": raw,
        "code": normalize(code) if code else "",
        "elapsed": elapsed,
    }


def score_candidate(candidate: dict, stem: str, fn: str) -> dict:
    if not candidate.get("code"):
        return {**candidate, "match_pct": 0.0, "compile_ok": False, "details": "no-code"}
    reward, pct, ok, details = remote_reward(stem, fn, candidate["code"])
    return {
        **candidate,
        "match_pct": pct,
        "compile_ok": ok,
        "reward": reward,
        "details": details[:300] if details else "",
    }


def build_repair_prompt(test: dict, current_code: str, current_pct: float, hint: str = "") -> str:
    asm = test["asm"].strip()
    fn = test["function"]
    extra = f"\n\nADDITIONAL HINT: {hint}\n" if hint else ""
    return (
        "You are debugging a CodeWarrior 1.3 byte-match decompilation. The current\n"
        f"candidate compiles but only matches {current_pct:.1f}%. Your job: produce a\n"
        "REVISED full-function C body that pushes the match closer to 100%.\n\n"
        "Common CW 1.3 fix paths to try (in order):\n"
        "1. Wrap with `#pragma push / #pragma peephole off / #pragma pop` if not already.\n"
        "2. Add `#pragma scheduling on` for instruction scheduling matches.\n"
        "3. Inline CTR loop counts into for(;;) init — never extract to a local.\n"
        "4. Use `(s32)(s16)x` or `(u16)*(u32*)ptr` casts to control extsh/clrlwi placement.\n"
        "5. Block-scope `{ }` to defeat CSE on duplicate SDA loads.\n"
        "6. For the cmpwi/beq/bge dead-prologue switch pattern: include both `case 0:` and `default:`.\n"
        "7. Use `(s8)*ptr < 0` OR `((u32)*ptr >> 7) & 1` — pick to match target's instructions.\n"
        f"{extra}\n"
        f"FUNCTION: {fn}\n\n"
        f"TARGET ASM:\n```\n{asm}\n```\n\n"
        f"CURRENT C CANDIDATE (matches {current_pct:.1f}%):\n```c\n{current_code}\n```\n\n"
        "Output ONLY the revised full C function with all needed externs, pragmas, and braces.\n"
        "Wrap in a single ```c ... ``` block. No explanation."
    )


def attack(test: dict, *, use_codex: bool, repair_rounds: int, max_tokens: int) -> dict:
    fn = test["function"]
    stem = Path(test["file"]).stem
    print(f"\n{'=' * 70}\nAttacking {stem}:{fn}\n{'=' * 70}\n")

    history = []

    # Phase 1 — parallel cloud generation
    print("Phase 1: parallel cloud drafts")
    candidates = []
    with ThreadPoolExecutor(max_workers=len(CLOUD_MODELS)) as ex:
        futures = [ex.submit(gen_cloud, m, test["prompt"], max_tokens) for m in CLOUD_MODELS]
        for fut in futures:
            candidates.append(fut.result())

    if use_codex:
        print("Phase 1b: codex draft")
        candidates.append(gen_codex(test["prompt"], fn))

    # Phase 2 — score all
    print("Phase 2: scoring")
    scored = []
    for c in candidates:
        s = score_candidate(c, stem, fn)
        scored.append(s)
        print(f"  {s['model']:36}  {s['match_pct']:5.1f}%  compile={str(s['compile_ok']):5}  ({s['elapsed']:.0f}s)")

    history.append({"phase": "initial", "candidates": scored})
    best = max(scored, key=lambda x: (x["match_pct"], x["compile_ok"]))
    best_pct = best["match_pct"]
    best_code = best["code"]
    print(f"\nBest after Phase 1: {best_pct:.1f}% from {best['model']}")

    # Phase 3 — repair rounds
    for rnd in range(repair_rounds):
        if best_pct >= 100.0:
            break
        if not best_code:
            print("Best has no code — skipping repair.")
            break
        print(f"\nPhase 3.{rnd + 1}: repair (current {best_pct:.1f}%)")
        repair_prompt = build_repair_prompt(test, best_code, best_pct)
        round_candidates = []
        with ThreadPoolExecutor(max_workers=len(CLOUD_MODELS)) as ex:
            futures = [ex.submit(gen_cloud, m, repair_prompt, max_tokens) for m in CLOUD_MODELS]
            for fut in futures:
                round_candidates.append(fut.result())
        if use_codex:
            round_candidates.append(gen_codex(repair_prompt, f"{fn}_r{rnd + 1}"))
        round_scored = []
        for c in round_candidates:
            s = score_candidate(c, stem, fn)
            round_scored.append(s)
            print(f"  {s['model']:36}  {s['match_pct']:5.1f}%  compile={str(s['compile_ok']):5}  ({s['elapsed']:.0f}s)")
        history.append({"phase": f"repair_{rnd + 1}", "candidates": round_scored})
        round_best = max(round_scored, key=lambda x: (x["match_pct"], x["compile_ok"]))
        if round_best["match_pct"] > best_pct:
            best = round_best
            best_pct = best["match_pct"]
            best_code = best["code"]
            print(f"  -> IMPROVED to {best_pct:.1f}% via {best['model']}")
        else:
            print(f"  -> no improvement (best of round: {round_best['match_pct']:.1f}%)")

    return {
        "function": fn,
        "stem": stem,
        "best_pct": best_pct,
        "best_compile_ok": best["compile_ok"],
        "best_model": best["model"],
        "best_code": best_code,
        "history": history,
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--suite", required=True)
    ap.add_argument("--fn", required=True, help="fn_XXXXXXXX target")
    ap.add_argument("--use-codex", action="store_true")
    ap.add_argument("--repair-rounds", type=int, default=1)
    ap.add_argument("--max-tokens", type=int, default=4096)
    args = ap.parse_args()

    suite = json.loads(Path(args.suite).read_text(encoding="utf-8"))
    test = next((t for t in suite if t["function"] == args.fn), None)
    if test is None:
        print(f"ERROR: {args.fn} not found in suite", file=sys.stderr)
        return 1

    result = attack(test, use_codex=args.use_codex, repair_rounds=args.repair_rounds, max_tokens=args.max_tokens)

    ATTACK_DIR.mkdir(parents=True, exist_ok=True)
    out_path = ATTACK_DIR / f"{args.fn}.json"
    out_path.write_text(json.dumps(result, indent=2), encoding="utf-8")

    print(f"\n{'=' * 70}")
    print(f"FINAL: {result['best_pct']:.1f}% via {result['best_model']} (compile={result['best_compile_ok']})")
    print(f"Saved: {out_path}")
    print(f"{'=' * 70}\n")
    if result["best_code"]:
        print("--- BEST CANDIDATE ---")
        print(result["best_code"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
