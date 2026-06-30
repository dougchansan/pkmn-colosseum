#!/usr/bin/env python3
"""queue_attack.py — Drain the deepseek-tier queue using the proven Apr 25
attack pipeline (parallel model drafts + iterative repair via 3090 reward).

Per-function flow (= attack_function.py.attack()):
  Phase 1: parallel cloud drafts (V4-flash, plus optional V4-pro / Kimi K2.6)
  Phase 2: score each via remote 3090 reward (compile + match%)
  Phase 3: N repair rounds — best candidate becomes basis for revision prompt
  Phase 4 (LOCAL): apply winning candidate to file in master, parent re-scan,
                   commit if ≥90% with attribution-friendly subject

Env vars:
  MODELS              comma-separated provider/model specs (default: v4-flash only)
  REPAIR_ROUNDS       default 2 (= 3 attack passes per fn)
  MAX_TOKENS          default 4096
  FILE_FILTER         comma-separated stems (default: all non-codex/sisyphus lanes)
  MAX_FN_ASM_LINES    default 60
"""
from __future__ import annotations

import json
import os
import re
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
os.chdir(REPO)
sys.path.insert(0, str(REPO / "tools/decomp_work/benchmark"))

# Re-use the proven Apr 25 plumbing — same module, same providers, same scoring.
import attack_function as af  # type: ignore  # noqa: E402

LOG_DIR = Path(r"G:\decomp-worktrees\logs")
LOG_DIR.mkdir(parents=True, exist_ok=True)
LOG_FILE = LOG_DIR / os.environ.get("LOG_FILE_BASENAME", "queue_attack.log")
TASKS_FILE = REPO / "tools/decomp_work/coordination/tasks.json"
TOKENS_FILE = REPO / ".omc/agent_tokens.json"
LOG_HELPER = REPO / "tools/decomp_work/overnight/log_to_dashboard.sh"
EXPERT_HEADER_FILE = Path(r"G:\decomp-worktrees\expert_prompt_header.md")
PARTIAL_POOL = REPO / ".omc/deepseek_pool"

# Override CLOUD_MODELS in attack_function with our cost-conscious selection.
# Default: V4-flash only (cheapest). Set MODELS env var to add more.
DEFAULT_MODELS = ["deepseek/deepseek-v4-flash"]
MODELS = [m.strip() for m in os.environ.get("MODELS", ",".join(DEFAULT_MODELS)).split(",") if m.strip()]
af.CLOUD_MODELS = MODELS  # type: ignore

REPAIR_ROUNDS = int(os.environ.get("REPAIR_ROUNDS", "5"))
PLATEAU_BREAK_AFTER = int(os.environ.get("PLATEAU_BREAK_AFTER", "3"))
MAX_TOKENS = int(os.environ.get("MAX_TOKENS", "4096"))
MAX_FN_ASM_LINES = int(os.environ.get("MAX_FN_ASM_LINES", "80"))
MIN_PRIOR_PCT = float(os.environ.get("MIN_PRIOR_PCT", "0"))
REQUIRE_SEED = os.environ.get("REQUIRE_SEED", "0") not in ("", "0", "false", "False")
MIN_PCT_KEEP = 90.0
AGENT_NAME = os.environ.get("AGENT_NAME", "deepseek-v4-flash" if MODELS == ["deepseek/deepseek-v4-flash"] else MODELS[0].replace("/", "-"))

FILE_FILTER = set(filter(None, (
    s.strip() for s in os.environ.get(
        "FILE_FILTER",
        "gs_thread,hsd_lobj,hsd_class,hsd_dobj,gs_event_exec,hsd_cobj,gs_texture,gs_material,fsys_file,effect_util",
    ).split(",")
)))
TIER_FILTER = set(filter(None, (
    s.strip() for s in os.environ.get("TIER_FILTER", "deepseek,kimi").split(",")
)))


def extract_current_candidate(src: Path, fn: str) -> str:
    try:
        text = src.read_text(encoding="utf-8")
    except OSError:
        return ""
    pat = re.compile(
        rf'#if\s+[01]\s*\n\s*asm\s+\w+\s+{re.escape(fn)}\s*\([^)]*\)\s*\{{\s*\n'
        rf'\s*#include\s+"[^"]+"\s*\n\s*\}}\s*\n'
        rf'#else\s*\n(.*?)#endif',
        re.DOTALL,
    )
    m = pat.search(text)
    if not m:
        return ""
    code = m.group(1).strip()
    if fn not in code or "TODO" in code or re.search(rf'{re.escape(fn)}\s*\([^)]*\)\s*\{{\s*\}}', code):
        return ""
    return code


def task_has_seed(task: dict) -> bool:
    fn = task.get("function", "")
    meta = task.get("meta", {})
    if (PARTIAL_POOL / f"{fn}.c").exists():
        return True
    file_path = meta.get("file")
    return bool(file_path and extract_current_candidate(REPO / file_path, fn))


def _log(msg: str) -> None:
    ts = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    line = f"[{ts}] {msg}\n"
    LOG_FILE.open("a", encoding="utf-8").write(line)
    print(line.rstrip(), flush=True)


def _dashboard(event: str, msg: str) -> None:
    try:
        subprocess.run(
            ["bash", str(LOG_HELPER), AGENT_NAME, event, msg],
            check=False, capture_output=True, timeout=5,
        )
    except Exception:
        pass


def update_tokens(used_delta: int, current_fn: str, status: str) -> None:
    try:
        data = json.loads(TOKENS_FILE.read_text(encoding="utf-8"))
        agent_key = AGENT_NAME
        a = data.setdefault("agents", {}).setdefault(agent_key, {
            "tokens_used": 0,
            "limit": 10_000_000,
            "since": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
            "current_fn": current_fn,
            "status": status,
        })
        a["tokens_used"] = a.get("tokens_used", 0) + max(0, used_delta)
        a["current_fn"] = current_fn
        a["status"] = status
        TOKENS_FILE.write_text(json.dumps(data, indent=2), encoding="utf-8")
    except Exception as e:
        _log(f"WARN: token update failed: {e}")


def claim_next_task() -> dict | None:
    try:
        tasks = json.loads(TASKS_FILE.read_text(encoding="utf-8"))
    except Exception as e:
        _log(f"queue read error: {e}")
        return None
    def _prior_score(t: dict) -> float:
        result = t.get("result") or {}
        try:
            return float(result.get("pct", result.get("best_pct", 0.0)) or 0.0)
        except Exception:
            return 0.0

    candidates = [
        t for t in tasks
        if t.get("status") == "queued"
        and t.get("meta", {}).get("model_tier") in TIER_FILTER
        and t.get("meta", {}).get("asm_lines", 999) <= MAX_FN_ASM_LINES
        and _prior_score(t) >= MIN_PRIOR_PCT
        and (not REQUIRE_SEED or task_has_seed(t))
        and (not FILE_FILTER or t.get("meta", {}).get("stem") in FILE_FILTER)
    ]
    if not candidates:
        return None

    prio_order = {"high": 0, "normal": 1, "low": 2}
    candidates.sort(key=lambda t: (
        -_prior_score(t),
        prio_order.get(t.get("priority", "normal"), 1),
        t.get("meta", {}).get("asm_lines", 999),
        t["created"],
    ))
    target = candidates[0]
    target["status"] = "claimed"
    target["claimed_by"] = AGENT_NAME
    TASKS_FILE.write_text(json.dumps(tasks, indent=2), encoding="utf-8")
    return target


def mark_task(task_id: str, status: str, extra: dict | None = None) -> None:
    try:
        tasks = json.loads(TASKS_FILE.read_text(encoding="utf-8"))
    except Exception as e:
        _log(f"mark_task read error: {e}")
        return
    for t in tasks:
        if t["id"] == task_id:
            t["status"] = status
            if status == "completed":
                t["completed_at"] = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
            if extra:
                t.setdefault("result", {}).update(extra)
            break
    TASKS_FILE.write_text(json.dumps(tasks, indent=2), encoding="utf-8")


def build_per_fn_prompt(fn: str, stem: str, src_path: Path, inc_text: str) -> str:
    """Reproduce the Apr 25 expert prompt: 21KB CW header + per-fn asm + sister ctx."""
    try:
        header = EXPERT_HEADER_FILE.read_text(encoding="utf-8")
    except OSError:
        header = "# CW 1.3 byte-match decomp\n"

    # Sister context (~80 lines around the fn definition) for type/extern hints.
    src_text = src_path.read_text(encoding="utf-8", errors="replace")
    line_idx = 0
    m = re.search(rf'(?:asm\s+\w+\s+|void\s+|u\d+\s+|s\d+\s+){re.escape(fn)}\s*\(', src_text)
    if m:
        line_idx = src_text[: m.start()].count("\n")
    lines = src_text.splitlines()
    start = max(0, line_idx - 40)
    end = min(len(lines), line_idx + 60)
    sister = "\n".join(lines[start:end])

    # Current stub signature for explicit guidance
    stub_match = re.search(
        rf'#else\s*\n([^}}]*?{re.escape(fn)}\s*\([^)]*\)\s*\{{[^}}]*\}})',
        src_text,
    )
    stub = stub_match.group(1).strip() if stub_match else f"void {fn}(void)"

    return f"""{header}

## Target function: {fn} in {stem}.c

CURRENT STUB SIGNATURE (use exactly):
```c
{stub}
```

ASM (.inc):
```
{inc_text}
```

NEARBY SOURCE CONTEXT from {stem}.c (lines {start+1}-{end}):
```c
{sister}
```

Output one ```c``` block only.
"""


_OBJDIFF_TARGET = REPO / "build/GC6E01/obj/auto_01_800055E0_text.o"


def _resolve_base_o(stem: str) -> Path:
    """Find the per-file .o under build/GC6E01/base/."""
    for sub in ("game", "hsd", "game/effect", "game/ui", "game/fsys",
                "game/battle", "game/people", "dolphin/os", "dolphin/vi",
                "dolphin/dvd", "dolphin/exi", "init", "crt"):
        p = REPO / "build/GC6E01/base" / sub / f"{stem}.o"
        if p.exists():
            return p
    return REPO / "build/GC6E01/base/game" / f"{stem}.o"


def compute_instr_diff(stem: str, fn: str, max_rows: int = 60) -> str:
    """Run objdiff and return a compact side-by-side instruction diff for the
    LLM. Empty string if anything fails (worker continues without diff feedback).
    """
    base_o = _resolve_base_o(stem)
    if not base_o.exists() or not _OBJDIFF_TARGET.exists():
        return ""
    try:
        r = subprocess.run(
            [str(REPO / ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli")), "diff",
             "-1", str(_OBJDIFF_TARGET), "-2", str(base_o),
             "-o", "-", "--format", "json",
             "-c", "ppc.calculatePoolRelocations=false", fn],
            capture_output=True, text=True, timeout=60,
        )
        if r.returncode != 0 or not r.stdout.strip():
            return ""
        d = json.loads(r.stdout)
    except Exception:
        return ""

    def _grab(side: str) -> list[str]:
        for sym in d.get(side, {}).get("symbols", []):
            if sym.get("name") == fn and sym.get("kind") == "SYMBOL_FUNCTION":
                return [
                    (i.get("instruction", {}) or {}).get("formatted", "")
                    for i in sym.get("instructions", [])
                ]
        return []

    tgt = _grab("left")
    cand = _grab("right")
    if not tgt or not cand:
        return ""

    rows = []
    n = max(len(tgt), len(cand))
    for i in range(min(n, max_rows)):
        t = tgt[i] if i < len(tgt) else "<-- missing -->"
        c = cand[i] if i < len(cand) else "<-- missing -->"
        marker = "" if t == c else "  *"
        rows.append(f"  {t:<38}  | {c:<38}{marker}")

    if n > max_rows:
        rows.append(f"  ... ({n - max_rows} more rows truncated) ...")

    return (
        "INSTRUCTION-LEVEL DIFF (target | candidate; '*' = mismatch):\n"
        + "\n".join(rows)
    )


def build_repair_prompt_with_diff(test: dict, current_code: str,
                                  current_pct: float, instr_diff: str) -> str:
    """Augment af.build_repair_prompt with real per-instruction diff feedback."""
    base = af.build_repair_prompt(test, current_code, current_pct)
    if not instr_diff:
        return base
    extra = (
        "\n\n## INSTRUCTION-LEVEL DIFF (most actionable feedback)\n"
        "Below are the FIRST instructions of target vs your last candidate.\n"
        "Each `*`-marked row is a mismatch you need to fix. Look at the\n"
        "specific opcode/operand difference and adjust the C accordingly.\n\n"
        f"{instr_diff}\n"
    )
    # Insert before the final "Output ONLY the revised..." instruction
    if "Output ONLY" in base:
        return base.replace("Output ONLY", extra + "\nOutput ONLY")
    return base + extra


def score_local(candidate_code: str, src: Path, stem: str, fn: str, backup: bytes) -> tuple[float, bool, str]:
    """Apply candidate to file, compile, match-scan, revert. Return (pct, compile_ok, reason)."""
    if not candidate_code or fn not in candidate_code:
        return 0.0, False, "empty-or-no-fn"
    ok, why = apply_candidate(src, fn, candidate_code)
    if not ok:
        src.write_bytes(backup)
        return 0.0, False, f"apply-{why}"
    try:
        success, pct, reason = verify_local(stem, fn)
        # Revert no matter what — we only commit at the very end on overall best
        src.write_bytes(backup)
        if reason == "compile-or-no-match":
            return pct, False, reason
        return pct, True, reason
    except Exception as e:
        src.write_bytes(backup)
        return 0.0, False, f"score-err: {e}"


def local_attack(test: dict, src: Path, stem: str, fn: str, backup: bytes,
                 *, repair_rounds: int, max_tokens: int) -> dict:
    """Apr 25-style attack pipeline with LOCAL scoring (no 3090 dependency)."""
    from concurrent.futures import ThreadPoolExecutor

    history = []
    print(f"\nAttacking {stem}:{fn} (local-scored)")

    # Phase 1: parallel cloud drafts (one model = no parallelism, but keeps shape)
    print("  Phase 1: drafts")
    candidates = []
    pool_path = PARTIAL_POOL / f"{fn}.c"
    if pool_path.exists():
        try:
            candidates.append({
                "model": "deepseek-pool",
                "code": pool_path.read_text(encoding="utf-8"),
                "elapsed": 0,
            })
            print(f"    seeded from {pool_path.relative_to(REPO)}")
        except OSError:
            pass
    current_code = extract_current_candidate(src, fn)
    if current_code:
        candidates.append({
            "model": "current-source",
            "code": current_code,
            "elapsed": 0,
        })
        print("    seeded from current source")
    if REQUIRE_SEED and not candidates:
        print("  no saved/current seed candidate; skipping token spend")
        return {
            "best_pct": 0.0,
            "best_code": "",
            "best_model": "no-seed",
            "history": history,
        }
    if REQUIRE_SEED:
        print("  Phase 1.5: seed precheck")
        seed_scored = []
        for c in candidates:
            pct, ok, reason = score_local(c.get("code", ""), src, stem, fn, backup)
            seed_scored.append({**c, "match_pct": pct, "compile_ok": ok, "reason": reason})
            print(f"    {c['model']:36}  {pct:5.1f}%  compile={ok}  reason={reason}  seed")
        history.append({"phase": "seed_precheck", "candidates": [
            {k: v for k, v in s.items() if k != "raw"} for s in seed_scored
        ]})
        best_seed = max(seed_scored, key=lambda x: (x["match_pct"], x["compile_ok"]))
        if best_seed["match_pct"] < MIN_PRIOR_PCT or not best_seed.get("compile_ok"):
            print(
                f"  seed best {best_seed['match_pct']:.1f}% below min_prior "
                f"{MIN_PRIOR_PCT:.1f}%; skipping token spend"
            )
            return {
                "best_pct": best_seed["match_pct"],
                "best_code": best_seed.get("code", ""),
                "best_model": best_seed.get("model", "seed-precheck"),
                "history": history,
            }
    if len(MODELS) > 1:
        with ThreadPoolExecutor(max_workers=len(MODELS)) as ex:
            futs = [ex.submit(af.gen_cloud, m, test["prompt"], max_tokens) for m in MODELS]
            for f in futs:
                candidates.append(f.result())
    else:
        candidates.append(af.gen_cloud(MODELS[0], test["prompt"], max_tokens))

    # Phase 2: local scoring
    print("  Phase 2: local scoring")
    scored = []
    for c in candidates:
        pct, ok, reason = score_local(c.get("code", ""), src, stem, fn, backup)
        scored.append({**c, "match_pct": pct, "compile_ok": ok, "reason": reason})
        print(f"    {c['model']:36}  {pct:5.1f}%  compile={ok}  reason={reason}  ({c.get('elapsed',0):.0f}s)")

    history.append({"phase": "initial", "candidates": [
        {k: v for k, v in s.items() if k != "raw"} for s in scored
    ]})
    best = max(scored, key=lambda x: (x["match_pct"], x["compile_ok"]))
    best_pct = best["match_pct"]
    best_code = best["code"]
    best_model = best["model"]

    if best_pct <= 0.0 and not best.get("compile_ok"):
        print("  initial candidate did not compile/score; skipping repairs from 0.0%")
        return {
            "best_pct": best_pct,
            "best_code": best_code,
            "best_model": best_model,
            "history": history,
        }

    def _save_partial() -> None:
        if best_pct <= 0.0 or not best_code:
            return
        try:
            PARTIAL_POOL.mkdir(parents=True, exist_ok=True)
            pool_path.write_text(best_code.rstrip() + "\n", encoding="utf-8")
        except OSError:
            pass

    _save_partial()

    # Phase 3: repair rounds w/ plateau detection
    plateau_count = 0
    for rnd in range(repair_rounds):
        if best_pct >= 100.0:
            break
        if not best_code:
            print("  no candidate to repair from; stopping")
            break
        if plateau_count >= PLATEAU_BREAK_AFTER:
            print(f"  plateau ({plateau_count} rounds no improvement) — stopping at {best_pct:.1f}%")
            break
        # Compute real instruction-level diff for the BEST candidate so the
        # repair prompt has actionable feedback (the Apr 25 attack pipeline
        # was missing this; should help V4 Flash push 70-80% candidates over).
        ok, why = apply_candidate(src, fn, best_code)
        instr_diff = ""
        if ok:
            try:
                # Need to compile so the .o reflects the candidate
                subprocess.run(
                    ["python", "tools/compile_check.py", str(src.relative_to(REPO)).replace("\\", "/")],
                    capture_output=True, text=True, timeout=120,
                )
                instr_diff = compute_instr_diff(stem, fn)
            except Exception:
                instr_diff = ""
            src.write_bytes(backup)  # always restore
        print(f"  Phase 3.{rnd+1}: repair from {best_pct:.1f}% (diff_rows={instr_diff.count(chr(10))})")
        repair_prompt = build_repair_prompt_with_diff(test, best_code, best_pct, instr_diff)
        round_cands = []
        if len(MODELS) > 1:
            with ThreadPoolExecutor(max_workers=len(MODELS)) as ex:
                futs = [ex.submit(af.gen_cloud, m, repair_prompt, max_tokens) for m in MODELS]
                for f in futs:
                    round_cands.append(f.result())
        else:
            round_cands.append(af.gen_cloud(MODELS[0], repair_prompt, max_tokens))
        round_scored = []
        for c in round_cands:
            pct, ok, reason = score_local(c.get("code", ""), src, stem, fn, backup)
            round_scored.append({**c, "match_pct": pct, "compile_ok": ok, "reason": reason})
            print(f"    {c['model']:36}  {pct:5.1f}%  compile={ok}  reason={reason}")
        history.append({"phase": f"repair_{rnd+1}", "candidates": [
            {k: v for k, v in s.items() if k != "raw"} for s in round_scored
        ]})
        round_best = max(round_scored, key=lambda x: (x["match_pct"], x["compile_ok"]))
        if round_best["match_pct"] > best_pct:
            best_pct = round_best["match_pct"]
            best_code = round_best["code"]
            best_model = round_best["model"]
            plateau_count = 0
            _save_partial()
            print(f"    -> IMPROVED to {best_pct:.1f}% via {best_model}")
        else:
            plateau_count += 1
            print(f"    -> no improvement ({plateau_count}/{PLATEAU_BREAK_AFTER}; best of round: {round_best['match_pct']:.1f}%)")

    return {
        "best_pct": best_pct,
        "best_code": best_code,
        "best_model": best_model,
        "history": history,
    }


def build_test_dict(task: dict) -> dict:
    """Build the 'test' dict that attack_function.attack() expects."""
    meta = task["meta"]
    fn = task["function"]
    stem = meta["stem"]
    src = REPO / meta["file"]
    inc = REPO / meta["inc"]

    inc_text = inc.read_text(encoding="utf-8", errors="replace")
    prompt = build_per_fn_prompt(fn, stem, src, inc_text)

    return {
        "function": fn,
        "file": meta["file"],
        "inc_file": meta["inc"],
        "asm": inc_text,
        "asm_lines": meta["asm_lines"],
        "prompt": prompt,
    }


def apply_candidate(src: Path, fn: str, candidate: str) -> tuple[bool, str]:
    """Same pattern flip as the original worker: #if 1 → #if 0 with C body in #else."""
    text = src.read_text(encoding="utf-8")
    pat = re.compile(
        rf'(#if\s+1\s*\n\s*asm\s+\w+\s+{re.escape(fn)}\s*\([^)]*\)\s*\{{\s*\n'
        rf'\s*#include\s+"[^"]+"\s*\n\s*\}}\s*\n)'
        rf'(#else\s*\n)(.*?)(#endif)',
        re.DOTALL,
    )
    m = pat.search(text)
    if not m:
        return False, "no-pattern"
    new = (
        m.group(1).replace("#if 1", "#if 0", 1)
        + m.group(2)
        + candidate.rstrip() + "\n"
        + m.group(4)
    )
    new_text = text[: m.start()] + new + text[m.end():]
    src.write_text(new_text, encoding="utf-8")
    return True, "applied"


def _parse_pct(out: str, fn: str) -> float | None:
    """Parse the match% from verify_match.py stdout. It prints an 'OK: <obj-path>'
    line BEFORE the percentage line (e.g. ' 100.0%  fn_X'), so splitting the whole
    blob on '%' mis-parses ('OK: ...o\\n 100.0' -> float() throws). Parse per-line:
    prefer the line that also names fn, else the first line carrying a percentage."""
    pct = None
    for line in out.splitlines():
        if "%" not in line:
            continue
        try:
            cand = float(line.split("%")[0].strip().split()[-1])
        except Exception:
            continue
        if fn in line:
            return cand
        if pct is None:
            pct = cand
    return pct


def verify_local(stem: str, fn: str) -> tuple[bool, float, str]:
    r = subprocess.run(
        ["python", "tools/decomp_work/overnight/verify_match.py", stem, fn],
        capture_output=True, text=True, timeout=180,
    )
    out = (r.stdout or "").strip()
    if r.returncode == 0:
        pct = _parse_pct(out, fn)
        return (True, pct, "ok") if pct is not None else (False, 0.0, "parse")
    if r.returncode == 1:
        pct = _parse_pct(out, fn)
        return False, (pct if pct is not None else 0.0), "low"
    return False, 0.0, "compile-or-no-match"


def process_one(task: dict) -> None:
    fn = task["function"]
    meta = task.get("meta", {})
    stem = meta.get("stem")
    src = REPO / meta["file"]

    update_tokens(0, fn, "busy")
    _log(f"START {fn} ({stem}.c, {meta.get('asm_lines')} asm lines, models={MODELS}, repairs={REPAIR_ROUNDS})")
    _dashboard("STARTED", f"{fn} ({stem}.c) attack-pipeline")

    backup = src.read_bytes()

    # Build attack_function.attack() input
    test = build_test_dict(task)

    # Run a LOCAL-scored version of the Apr 25 pipeline. We can't use 3090
    # reward because that compiler is hardcoded to gs_title.c; for arbitrary
    # files we score by apply→compile→match_scan→revert in this process.
    try:
        result = local_attack(test, src, stem, fn, backup,
                              repair_rounds=REPAIR_ROUNDS, max_tokens=MAX_TOKENS)
    except Exception as e:
        _log(f"FAIL {fn}: attack pipeline exception: {e}")
        src.write_bytes(backup)  # safety
        mark_task(task["id"], "failed", {"reason": f"attack-error: {e}"})
        update_tokens(0, fn, "idle")
        return

    best_pct = result.get("best_pct", 0.0)
    best_code = result.get("best_code") or ""
    best_model = result.get("best_model", "?")

    # Token estimate: prompt size × (1 initial + N repair rounds) for input,
    # plus avg ~2KB output per round. local_attack drops 'raw' from history
    # so we estimate from structure: count phases in history, multiply.
    n_attempts = sum(len(p.get("candidates", [])) for p in result.get("history", []))
    if n_attempts == 0:
        n_attempts = 1
    prompt_tokens = (len(test["prompt"]) // 4) * n_attempts
    output_tokens = 500 * n_attempts  # ~500 tokens per response
    used = prompt_tokens + output_tokens
    update_tokens(used, fn, "busy")

    if best_pct < MIN_PCT_KEEP or not best_code:
        _log(f"REJECT {fn}: best={best_pct:.1f}% via {best_model} (<{MIN_PCT_KEEP}%)")
        mark_task(task["id"], "failed", {
            "attempt_pct": best_pct, "attempt_model": best_model,
            "reason": "below-threshold",
        })
        update_tokens(0, fn, "idle")
        return

    # Apply the winning candidate locally and re-verify in the parent process.
    ok, why = apply_candidate(src, fn, best_code)
    if not ok:
        _log(f"FAIL {fn}: cannot apply ({why})")
        src.write_bytes(backup)
        mark_task(task["id"], "failed", {"reason": f"apply-{why}", "best_pct": best_pct})
        update_tokens(0, fn, "idle")
        return

    success, local_pct, reason = verify_local(stem, fn)
    if success and local_pct >= MIN_PCT_KEEP:
        _log(f"MATCH {fn}: 3090={best_pct:.1f}% local={local_pct:.1f}% via {best_model} [keep]")
        _dashboard("MATCH", f"{fn} {local_pct:.1f}% in {stem}.c by deepseek-v4-flash")
        mark_task(task["id"], "completed", {
            "best_pct": best_pct, "local_pct": local_pct,
            "best_model": best_model,
        })
    else:
        # 3090 said good, local says low — common with multi-fn .c files where
        # other unmatched fns drift the diff. Trust local for commit gating.
        _log(f"REVERT {fn}: 3090={best_pct:.1f}% but local={local_pct:.1f}% reason={reason}")
        src.write_bytes(backup)
        mark_task(task["id"], "failed", {
            "best_pct": best_pct, "local_pct": local_pct,
            "reason": f"local-disagrees-{reason}",
        })

    update_tokens(0, fn, "idle")


def main():
    _log(f"queue_attack started; models={MODELS}; repair_rounds={REPAIR_ROUNDS}; cap={MAX_FN_ASM_LINES}; min_prior={MIN_PRIOR_PCT}")
    _dashboard("INFO", f"queue_attack online (models={','.join(MODELS)}, repairs={REPAIR_ROUNDS})")

    n_done = 0
    while True:
        task = claim_next_task()
        if task is None:
            _log("queue empty for filter; sleeping 60s")
            update_tokens(0, "idle", "idle")
            time.sleep(60)
            continue
        try:
            process_one(task)
        except Exception as e:
            _log(f"process_one fatal for {task.get('function')}: {e}")
            mark_task(task["id"], "failed", {"reason": f"fatal: {e}"})
            update_tokens(0, "idle", "idle")
        n_done += 1
        if n_done % 5 == 0:
            try:
                tasks = json.loads(TASKS_FILE.read_text(encoding="utf-8"))
                ok = sum(1 for t in tasks if t.get("claimed_by") == AGENT_NAME and t.get("status") == "completed")
                _log(f"--- session progress: {ok} matched / {n_done} attempted")
            except Exception:
                pass
        time.sleep(2)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        _log("interrupted")
        update_tokens(0, "idle", "idle")
