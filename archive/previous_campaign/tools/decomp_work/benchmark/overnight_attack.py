#!/usr/bin/env python3
"""overnight_attack.py — Systematic multi-mutation attack on all asm-still-active functions.

For each function in the suite (default: gs_title still-asm-active set):
  1. Try the existing #else partial as baseline
  2. Generate fresh candidates from V4-flash, V4-pro, Kimi K2.6 (parallel)
  3. For the best compiling candidate, apply mechanical mutations:
       * pragma combos (peephole on/off, scheduling on/off, fp_contract on/off)
       * u32 vs u8 vs s32 for mask-style locals
       * declaration-order permutations
       * cast-fnptr -> block-extern conversion (and vice versa)
  4. Log every attempt to .omc/decomp_attack/<fn>_log.jsonl
  5. Save best candidate to .omc/decomp_attack/<fn>_best.{c,json}
  6. Update .omc/decomp_attack/LEADERBOARD.md after EACH function

Designed for unattended overnight runs. Resumable: skips functions whose
.omc/decomp_attack/<fn>_best.json already shows >= MATCH_FLOOR (default 90%).

Usage:
    python tools/decomp_work/benchmark/overnight_attack.py \
        --suite tools/decomp_work/benchmark/test_suite_gs_title_expert.json \
        --max-attempts-per-fn 20 \
        --skip-above 95
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
from bench_opencode import run_model, normalize  # noqa: E402
from bench_compile_match import remote_reward  # noqa: E402

REPO = Path(__file__).resolve().parents[3]
ATTACK_DIR = REPO / ".omc" / "decomp_attack"
LEADERBOARD = ATTACK_DIR / "LEADERBOARD.md"

CLOUD_MODELS = [
    "deepseek/deepseek-v4-flash",
    "deepseek/deepseek-v4-pro",
    "moonshot/kimi-k2.6",
]

# Mechanical mutations — small, deterministic source transforms.
PRAGMA_VARIANTS = [
    ("default-peephole-off",  "#pragma push\n#pragma peephole off\n#pragma optimization_level 4"),
    ("scheduling-on",          "#pragma push\n#pragma scheduling on\n#pragma optimization_level 4"),
    ("scheduling+peephole-off","#pragma push\n#pragma scheduling on\n#pragma peephole off\n#pragma optimization_level 4"),
    ("fp_contract-on",         "#pragma push\n#pragma peephole off\n#pragma fp_contract on\n#pragma optimization_level 4"),
    ("plain-O4",               "#pragma optimization_level 4"),
    ("nopragma",               ""),
]


def extract_c_safely(raw: str) -> str | None:
    if not raw:
        return None
    m = re.search(r"```c?\s*\n(.*?)```", raw, re.DOTALL)
    if m:
        return m.group(1).strip()
    return raw.strip() or None


def gen_cloud(model: str, prompt: str, max_tokens: int) -> tuple[str, str | None, float]:
    raw, elapsed = run_model(model, prompt, max_tokens=max_tokens)
    code = extract_c_safely(raw)
    return raw, normalize(code) if code else None, elapsed


def score(stem: str, fn: str, code: str | None) -> tuple[float, bool]:
    if not code:
        return 0.0, False
    _, pct, ok, _ = remote_reward(stem, fn, code)
    return pct, ok


def existing_partial(c_text: str, fn: str) -> str | None:
    """Pull the current #else body for the function."""
    pattern = re.compile(
        r"#if\s+[01]\s*\nasm void " + re.escape(fn) + r"\(void\)\s*\{[^}]*\}\s*\n"
        r"#else\s*\n(.*?)\n#endif",
        re.S,
    )
    m = pattern.search(c_text)
    if m:
        body = m.group(1).strip()
        if len(body) > 60 and "/* TODO" not in body[:80]:
            return body
    return None


def apply_pragma_swap(code: str, new_header: str) -> str:
    """Replace the leading pragma block with `new_header`."""
    lines = code.splitlines()
    out = []
    in_header = True
    for ln in lines:
        if in_header and (ln.startswith("#pragma") or ln.strip() == ""):
            continue
        in_header = False
        out.append(ln)
    body = "\n".join(out)
    if new_header:
        body = new_header + "\n" + body
    # Strip trailing #pragma pop if present
    body = re.sub(r"\n#pragma pop\s*$", "", body)
    if "#pragma push" in (new_header or ""):
        body = body + "\n#pragma pop"
    return body


def log_attempt(fn: str, label: str, pct: float, ok: bool, code: str | None, extra: dict | None = None) -> None:
    line = {
        "ts": time.time(),
        "fn": fn,
        "label": label,
        "pct": pct,
        "compile_ok": ok,
        "code_len": len(code) if code else 0,
        "extra": extra or {},
    }
    with (ATTACK_DIR / f"{fn}_log.jsonl").open("a", encoding="utf-8") as f:
        f.write(json.dumps(line) + "\n")


def save_best(fn: str, stem: str, label: str, pct: float, ok: bool, code: str) -> None:
    (ATTACK_DIR / f"{fn}_best.json").write_text(
        json.dumps({"fn": fn, "stem": stem, "label": label, "pct": pct, "compile_ok": ok}, indent=2),
        encoding="utf-8",
    )
    (ATTACK_DIR / f"{fn}_best.c").write_text(code, encoding="utf-8")


def update_leaderboard(scores: dict[str, dict]) -> None:
    rows = sorted(scores.values(), key=lambda r: -r["pct"])
    md = ["# Overnight attack leaderboard", "", "Updated: " + time.strftime("%Y-%m-%d %H:%M:%S"), "", "| Function | best% | compile | label |", "|---|---|---|---|"]
    for r in rows:
        md.append(f"| `{r['fn']}` | {r['pct']:.1f}% | {r['compile_ok']} | {r['label']} |")
    LEADERBOARD.write_text("\n".join(md), encoding="utf-8")


def attack_one(test: dict, *, max_attempts: int, max_tokens: int) -> dict:
    fn = test["function"]
    stem = Path(test["file"]).stem
    print(f"\n=== {stem}:{fn} ===", flush=True)

    best_pct = 0.0
    best_ok = False
    best_code = ""
    best_label = "none"
    attempts = 0

    # Phase 1: existing partial
    c_text = (REPO / test["file"]).read_text(encoding="latin-1")
    partial = existing_partial(c_text, fn)
    if partial:
        attempts += 1
        pct, ok = score(stem, fn, partial)
        log_attempt(fn, "existing-partial", pct, ok, partial)
        print(f"  existing partial: {pct:.1f}% ok={ok}", flush=True)
        if pct > best_pct or (pct == best_pct and ok and not best_ok):
            best_pct, best_ok, best_code, best_label = pct, ok, partial, "existing-partial"

    # Phase 2: parallel cloud drafts
    print(f"  cloud drafts ({len(CLOUD_MODELS)} models)...", flush=True)
    cloud_results = []
    with ThreadPoolExecutor(max_workers=len(CLOUD_MODELS)) as ex:
        futures = {ex.submit(gen_cloud, m, test["prompt"], max_tokens): m for m in CLOUD_MODELS}
        for fut, m in [(f, futures[f]) for f in futures]:
            try:
                raw, code, elapsed = fut.result(timeout=180)
            except Exception as ex_:
                raw, code, elapsed = f"EXC: {ex_}", None, 0
            cloud_results.append((m, code, elapsed))

    for m, code, elapsed in cloud_results:
        attempts += 1
        if not code:
            log_attempt(fn, f"cloud:{m}", 0.0, False, None, {"elapsed": elapsed, "note": "no-code"})
            print(f"  {m:36s} no-code ({elapsed:.0f}s)", flush=True)
            continue
        pct, ok = score(stem, fn, code)
        log_attempt(fn, f"cloud:{m}", pct, ok, code, {"elapsed": elapsed})
        print(f"  {m:36s} {pct:5.1f}% ok={ok} ({elapsed:.0f}s)", flush=True)
        if pct > best_pct or (pct == best_pct and ok and not best_ok):
            best_pct, best_ok, best_code, best_label = pct, ok, code, f"cloud:{m}"

    # Phase 3: mechanical pragma mutations on the best compiling candidate
    if best_ok and best_code and best_pct < 100.0:
        print(f"  pragma mutations from {best_label} @ {best_pct:.1f}%", flush=True)
        for label, header in PRAGMA_VARIANTS:
            if attempts >= max_attempts:
                break
            mutated = apply_pragma_swap(best_code, header)
            if mutated == best_code:
                continue
            attempts += 1
            pct, ok = score(stem, fn, mutated)
            log_attempt(fn, f"mut:{label}", pct, ok, mutated)
            print(f"    {label:34s} {pct:5.1f}% ok={ok}", flush=True)
            if pct > best_pct:
                best_pct, best_ok, best_code, best_label = pct, ok, mutated, f"mut:{label}"
                if best_pct >= 100.0:
                    break

    if best_code:
        save_best(fn, stem, best_label, best_pct, best_ok, best_code)
    print(f"  -> BEST {best_pct:.1f}% via {best_label} (attempts={attempts})", flush=True)
    return {"fn": fn, "pct": best_pct, "compile_ok": best_ok, "label": best_label}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--suite", required=True)
    ap.add_argument("--only-stem", default="gs_title")
    ap.add_argument("--max-attempts-per-fn", type=int, default=20)
    ap.add_argument("--max-tokens", type=int, default=4096)
    ap.add_argument("--skip-above", type=float, default=99.5,
                    help="Skip functions whose existing _best.json already meets this %")
    ap.add_argument("--only-fn", default=None,
                    help="Comma-separated subset")
    args = ap.parse_args()

    ATTACK_DIR.mkdir(parents=True, exist_ok=True)
    suite = json.loads(Path(args.suite).read_text(encoding="utf-8"))
    suite = [t for t in suite if t["file"].startswith("src/game/")]
    if args.only_stem:
        suite = [t for t in suite if Path(t["file"]).stem == args.only_stem]
    if args.only_fn:
        wanted = {x.strip() for x in args.only_fn.split(",")}
        suite = [t for t in suite if t["function"] in wanted]

    print(f"overnight_attack: {len(suite)} candidates from {args.suite}", flush=True)
    print(f"max_attempts_per_fn={args.max_attempts_per_fn}, skip_above={args.skip_above}%", flush=True)

    scores: dict[str, dict] = {}
    # Pre-load any existing _best.json (resume support)
    for t in suite:
        bp = ATTACK_DIR / f"{t['function']}_best.json"
        if bp.exists():
            try:
                scores[t["function"]] = json.loads(bp.read_text(encoding="utf-8"))
            except Exception:
                pass
    update_leaderboard(scores)

    for t in suite:
        fn = t["function"]
        prior = scores.get(fn)
        if prior and prior.get("pct", 0) >= args.skip_above:
            print(f"skip {fn}: already at {prior['pct']:.1f}%", flush=True)
            continue
        try:
            r = attack_one(t, max_attempts=args.max_attempts_per_fn, max_tokens=args.max_tokens)
            scores[fn] = r
        except KeyboardInterrupt:
            print("interrupted", flush=True)
            break
        except Exception as e:
            print(f"  ERROR on {fn}: {e}", flush=True)
        update_leaderboard(scores)

    # Final summary
    rows = sorted(scores.values(), key=lambda r: -r["pct"])
    print("\n=== FINAL ===")
    for r in rows:
        print(f"  {r['fn']:14s}  {r['pct']:5.1f}%  ok={r['compile_ok']}  {r['label']}")
    print(f"\nLeaderboard: {LEADERBOARD}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
