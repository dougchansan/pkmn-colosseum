#!/usr/bin/env python3
"""lane_3090.py - offload candidate-C generation to the local 3090 GPU.

Architecture: remote generation, LOCAL gated verification. The 3090
(Ollama @ $DECOMP_GPU_HOST, default 192.168.50.101, qwen2.5-coder:32b — the
only model that produced
usable CW-matchable C on that box per the prior benchmark) generates a
candidate C body for an undecompiled / low-% function. EVERYTHING after
that is the same hardened pipeline used everywhere else:

  ghidra seed + target asm  --HTTP-->  3090 qwen2.5-coder:32b
                            <--cand C--
  splice into fn span -> isolated compile -> objdiff measure ->
  keep only if that fn strictly improves AND whole-file matched-count
  does NOT regress -> commit.

Zero API tokens. Targets the ~4000 low-% asm-style functions the
deterministic loop can't touch and that are too token-expensive for
Claude agents at scale. Safe by construction: the 3090 only proposes;
the local gate decides.

Usage:
    python tools/lane_3090.py src/game/battle/battle_grid.c --apply
    python tools/lane_3090.py src/game/gs_floor.c --max-pct 60 --limit 8 --apply
    python tools/lane_3090.py src/game/foo.c --symbol fn_80012345
"""

import argparse
import json
import os
import re
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OLLAMA_HOST = os.environ.get("DECOMP_GPU_HOST", "192.168.50.101")
OLLAMA_PORT = os.environ.get("DECOMP_OLLAMA_PORT", "11434")
OLLAMA = f"http://{OLLAMA_HOST}:{OLLAMA_PORT}/api/generate"
MODEL = "qwen2.5-coder:32b"
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
sys.path.insert(0, str(ROOT / "tools"))
import automatch          # noqa: E402  find_fn_def/measure_isolated/read_src
import compile_check      # noqa: E402
try:
    import ghidra_seed   # noqa: E402  raw_index/ghidra_block
except Exception:
    ghidra_seed = None

CW_CRIB = """\
Target compiler: Metrowerks CodeWarrior 1.3 for GameCube PowerPC, flags
-O4,p. Match its codegen EXACTLY. Known quirks:
- Prefer `#pragma push`/`#pragma peephole off`/.../`#pragma pop` around
  the function if record-form compares (cmpwi vs clrlwi.) differ.
- Signed vs unsigned compare: `*(s32*)` gives cmpwi, `*(u32*)` gives
  cmplwi. Pick to match the target.
- Direct `(s16)f` cast (NOT `(s16)(s32)f`) avoids a spurious extsh.
- Demote u16/u8 params to s32 to keep clrlslwi on the original reg.
- Ghidra's unaff_rN / _DAT_ / undefinedN are NOT reliable — derive the
  real signature and types from the TARGET DISASSEMBLY below.
Output ONLY the C function definition (no prose, no markdown fences,
no #if/#else, no asm). It must compile under the above flags."""


def disasm(symbol, base_o):
    """Target-side instruction list for `symbol`, compact text."""
    r = subprocess.run(
        [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false", symbol],
        capture_output=True, text=True, cwd=str(ROOT))
    if r.returncode != 0:
        return ""
    try:
        j = json.loads(r.stdout)
    except ValueError:
        return ""
    for s in j.get("left", {}).get("symbols", []):
        if s.get("name") == symbol:
            ins = [i.get("instruction", {}).get("formatted", "")
                   for i in s.get("instructions", [])]
            return "\n".join(x for x in ins if x)[:6000]
    return ""


def ask_3090(prompt, timeout=240):
    body = json.dumps({
        "model": MODEL, "prompt": prompt, "stream": False,
        "options": {"temperature": 0.1, "num_predict": 2048,
                    "num_ctx": 8192},
    }).encode()
    req = urllib.request.Request(OLLAMA, data=body,
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        return json.loads(r.read()).get("response", "")


def extract_c(text, fn):
    """Pull a plausible C function definition for `fn` out of the model
    reply (strip fences/prose)."""
    text = re.sub(r"```[a-zA-Z]*", "", text).replace("```", "")
    i = text.find(fn + "(")
    if i == -1:
        return None
    # walk back to the start of the return type / line
    j = text.rfind("\n", 0, i)
    start = j + 1 if j != -1 else 0
    # brace-match forward
    depth = 0
    seen = False
    k = text.find("{", i)
    if k == -1:
        return None
    for p in range(k, len(text)):
        if text[p] == "{":
            depth += 1
            seen = True
        elif text[p] == "}":
            depth -= 1
            if seen and depth == 0:
                return text[start:p + 1].strip()
    return None


def main():
    global MODEL
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--symbol")
    ap.add_argument("--max-pct", type=float, default=60.0,
                    help="only attempt fns below this match%% (bulk focus)")
    ap.add_argument("--min-pct", type=float, default=0.0,
                    help="only attempt fns at/above this match%% "
                         "(set high to refine near-misses instead of "
                         "from-scratch asm bulk)")
    ap.add_argument("--best-first", action="store_true",
                    help="attempt closest-to-100%% first (refinement ROI) "
                         "instead of worst-first")
    ap.add_argument("--limit", type=int, default=8)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--model", default=MODEL)
    args = ap.parse_args()
    MODEL = args.model

    src = Path(args.source)
    if not src.is_absolute():
        src = ROOT / src
    original = automatch.read_src(src)
    base_o = compile_check.source_to_base_obj(src.resolve())
    if not base_o.exists():
        compile_check.compile_source(src)

    base = automatch.measure(src, None)
    if base is None:
        sys.exit("baseline compile failed")
    base_matched = automatch.matched_count(base)
    print(f"[3090] {src.name} baseline {base_matched}/{len(base)} @100")

    if args.symbol:
        targets = [(base.get(args.symbol, 0.0), args.symbol)]
    else:
        targets = sorted(
            ((p, n) for n, p in base.items()
             if n.startswith("fn_")
             and args.min_pct <= p < args.max_pct),
            reverse=args.best_first)
        targets = targets[:args.limit]
    print(f"[3090] {len(targets)} targets "
          f"[{args.min_pct},{args.max_pct})% "
          f"{'best' if args.best_first else 'worst'}-first via {MODEL}")

    lines = original.splitlines(keepends=True)
    g_lines = g_idx = None
    if ghidra_seed and ghidra_seed.RAW.exists():
        g_lines, g_idx = ghidra_seed.raw_index()

    wins = []
    t0 = time.time()
    for k, (pct, fn) in enumerate(targets, 1):
        loc = automatch.find_fn_def(lines, fn)
        if loc is None:
            print(f"  [{k}/{len(targets)}] {fn} no C body, skip")
            continue
        si, ci = loc
        gh = ""
        if g_lines is not None:
            try:
                from dol_addr import va_to_off
                off = va_to_off(int(fn[3:], 16))
                if off is not None:
                    blk = ghidra_seed.ghidra_block(g_lines, g_idx,
                                                   int(fn[3:], 16))
                    gh = blk or ""
            except Exception:
                pass
        asm = disasm(fn, base_o)
        prompt = (f"{CW_CRIB}\n\n=== FUNCTION: {fn} (currently {pct:.1f}% "
                  f"match) ===\n\n--- Ghidra decompile (hints only) ---\n"
                  f"{gh[:4000]}\n\n--- TARGET DISASSEMBLY (authoritative) "
                  f"---\n{asm}\n\n--- Current C in tree ---\n"
                  f"{''.join(lines[si:ci+1])[:3000]}\n\n"
                  f"Rewrite {fn} to match the target disassembly exactly.")
        try:
            reply = ask_3090(prompt)
        except Exception as e:
            print(f"  [{k}/{len(targets)}] {fn} 3090 error: {e}")
            continue
        cand = extract_c(reply, fn)
        if not cand:
            print(f"  [{k}/{len(targets)}] {fn} no parseable C in reply")
            continue
        trial = lines[:si] + [cand + "\n"] + lines[ci + 1:]
        m = automatch.measure_isolated("".join(trial), src, [fn],
                                       f"l3090_{fn}_{k}")
        np = (m or {}).get(fn, -1.0)
        tag = ("=100" if np >= 100 else f"+{np-pct:.1f}") \
            if np > pct + 1e-6 else "no gain"
        print(f"  [{k}/{len(targets)}] {fn} {pct:.1f}% -> "
              f"{np:.1f}%  ({tag})")
        if np > pct + 1e-6:
            wins.append((fn, si, ci, cand, pct, np))

    if args.apply and wins:
        cur = original
        for fn, _si, _ci, cand, _p, _n in wins:
            ls = cur.splitlines(keepends=True)
            loc = automatch.find_fn_def(ls, fn)
            if loc is None:
                continue
            s2, c2 = loc
            cur = "".join(ls[:s2] + [cand + "\n"] + ls[c2 + 1:])
        automatch.write_src(src, cur)
        final = automatch.measure(src, None)
        if final is None or automatch.matched_count(final) < base_matched:
            automatch.write_src(src, original)
            print("[3090] APPLY REVERTED — net regression / compile fail")
        else:
            rel = src.resolve().relative_to(ROOT).as_posix()
            subprocess.run(["git", "-C", str(ROOT), "add", rel],
                           capture_output=True)
            subprocess.run(
                ["git", "-C", str(ROOT), "commit", "-q", "-m",
                 f"{src.stem}: lane_3090 candidates ({len(wins)} fns, "
                 f"matched {base_matched}->{automatch.matched_count(final)})"
                 "\n\nGenerated on local 3090 (qwen2.5-coder:32b), "
                 "zero API tokens; gated by isolated compile + objdiff + "
                 "whole-file non-regression.\n\nCo-Authored-By: Claude "
                 "Opus 4.7 (1M context) <noreply@anthropic.com>"],
                capture_output=True)
            print(f"[3090] APPLIED {len(wins)} wins, matched "
                  f"{base_matched}->{automatch.matched_count(final)}")
    dt = time.time() - t0
    print(f"[3090] done {dt:.0f}s — {sum(1 for w in wins if w[5]>=100)} "
          f"@100, {len(wins)} improved, zero API tokens")


if __name__ == "__main__":
    main()
