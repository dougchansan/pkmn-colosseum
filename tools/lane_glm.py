#!/usr/bin/env python3
"""lane_glm.py — GLM-driven matching-decomp lane (cockpit → pipeline).

Architecture: REMOTE generation, LOCAL gated verification — the same hardened
loop the other lanes use, with GLM (z.ai coding plan, the cockpit's model) as the
candidate generator instead of the retired local Ollama.

  target PPC asm + current C  --HTTPS-->  z.ai GLM (glm-5.2 / glm-4.6)
                              <--cand C--
  replace the fn body in the .c -> compile via the local gate (compile_check,
  mwcceppc through wibo on mac/linux) -> objdiff measure -> keep ONLY if that
  function strictly improves AND the file's fully-matched count does not regress
  -> otherwise revert. GLM only proposes; the local gate decides.

Credentials: ZAI_API_KEY (exported by the cockpit's
~/.config/decomp-keys/cockpit.env). Zero risk to the tree — every non-improving
candidate is reverted.

Usage:
    python tools/lane_glm.py src/game/colosseum_battle.c --max-pct 60 --limit 5 --apply
    python tools/lane_glm.py src/game/colosseum_battle.c --symbol fn_80240454 --apply
    python tools/lane_glm.py src/game/foo.c --model glm-4.6 --limit 8
"""

import argparse
import json
import os
import re
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import compile_check as cc  # noqa: E402  compile + objdiff gate (mac-ready via wibo)

ZAI_URL = "https://api.z.ai/api/coding/paas/v4/chat/completions"
DEFAULT_MODEL = "glm-5.2"  # the cockpit's GLM; --model glm-4.6 for faster bulk codegen
COCKPIT_ENV = Path(os.environ.get(
    "DECOMP_COCKPIT_ENV", str(Path.home() / ".config" / "decomp-keys" / "cockpit.env")))

CW_CRIB = """\
Target compiler: Metrowerks CodeWarrior for GameCube PowerPC (Gekko), flags
-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int. Reproduce
its codegen EXACTLY. Known quirks:
- C89 ONLY: every declaration before any statement in a block.
- `subf rD,rA,rB` = rB - rA (operands reversed).
- Signed vs unsigned: `*(s32*)` -> cmpwi, `*(u32*)` -> cmplwi. Pick to match.
- Use block scope { } when the asm loads the SAME global from r13 twice.
- NEVER use float literals (0.0f); declare `extern f32 lbl_XXXXXXXX;`.
- A leaf fn (no bl) needs no pragmas; a thunk is `extern void t(void); void f(void){ t(); }`.
Output ONLY the single C function definition (no prose, no markdown fences, no
asm, no #if). Keep the EXACT signature shown in the current C."""


def load_key():
    key = os.environ.get("ZAI_API_KEY")
    if key:
        return key
    if COCKPIT_ENV.is_file():
        for ln in COCKPIT_ENV.read_text().splitlines():
            m = re.match(r'\s*export\s+ZAI_API_KEY=(.+)', ln)
            if m:
                # take the bare token (ignore quotes and any trailing inline comment)
                return m.group(1).strip().strip('"').strip("'").split()[0]
    sys.exit(f"no ZAI_API_KEY (set it or populate {COCKPIT_ENV})")


def ask_glm(prompt, model, key, timeout=420):
    body = json.dumps({
        "model": model,
        "messages": [{"role": "user", "content": prompt}],
        "temperature": 0.2,
        "max_tokens": 16384,  # room for glm-5.2 reasoning + the full answer
    }).encode()
    req = urllib.request.Request(
        ZAI_URL, data=body,
        headers={"Content-Type": "application/json",
                 "Authorization": f"Bearer {key}"})
    with urllib.request.urlopen(req, timeout=timeout) as r:
        d = json.loads(r.read())
    return d["choices"][0]["message"].get("content", "") or ""


def extract_c(text, fn):
    """Pull the C definition for `fn` out of a (possibly fenced) GLM reply."""
    text = re.sub(r"```[a-zA-Z]*", "", text).replace("```", "")
    i = text.find(fn + "(")
    if i == -1:
        return None
    # start of the line that holds the signature (return type is on it for these files)
    start = text.rfind("\n", 0, i) + 1
    k = text.find("{", i)
    if k == -1:
        return None
    depth = 0
    for p in range(k, len(text)):
        if text[p] == "{":
            depth += 1
        elif text[p] == "}":
            depth -= 1
            if depth == 0:
                return text[start:p + 1].strip()
    return None


def find_fn_span(text, fn):
    """Return (start, end, body) of the fn DEFINITION (not a forward decl)."""
    for m in re.finditer(re.escape(fn) + r"\s*\(", text):
        # find the matching ) then require a { (definition), not ; (decl)
        p = m.end() - 1
        depth = 0
        while p < len(text):
            if text[p] == "(":
                depth += 1
            elif text[p] == ")":
                depth -= 1
                if depth == 0:
                    break
            p += 1
        q = p + 1
        while q < len(text) and text[q] in " \t\r\n":
            q += 1
        if q >= len(text) or text[q] != "{":
            continue  # forward declaration
        # back up to the start of the return type / line
        ls = text.rfind("\n", 0, m.start()) + 1
        # brace-match the body
        depth = 0
        r = q
        while r < len(text):
            if text[r] == "{":
                depth += 1
            elif text[r] == "}":
                depth -= 1
                if depth == 0:
                    return ls, r + 1, text[ls:r + 1]
            r += 1
    return None


def target_asm(tgt_obj, base_obj, symbol):
    """Compact target-side instruction listing for `symbol`."""
    dj = cc._run_objdiff_json(tgt_obj, base_obj, symbol)
    if not dj:
        return ""
    for s in dj.get("left", {}).get("symbols", []):
        if s.get("name") == symbol:
            ins = [i.get("instruction", {}).get("formatted", "")
                   for i in s.get("instructions", [])]
            return "\n".join(x for x in ins if x)[:6000]
    return ""


def measure_file(src):
    """Compile `src` and return (matched_count, {sym: pct}). None on compile fail."""
    try:
        obj = cc.compile_source(src)
    except SystemExit:
        return None
    tgt = cc.find_target_obj(src)
    dj = cc._run_objdiff_json(tgt, obj)
    out = {}
    for name in cc._list_function_symbols(dj):
        s = cc._summarize_symbol(dj, name)
        if s["symbol_found"] and s["total_instructions"] > 0:
            out[name] = s["match_percent"]
    matched = sum(1 for v in out.values() if v >= 100.0)
    return matched, out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--symbol")
    ap.add_argument("--min-pct", type=float, default=0.0,
                    help="only attempt fns at/above this match%% (e.g. 50 to focus near-misses)")
    ap.add_argument("--max-pct", type=float, default=99.9,
                    help="only attempt fns below this match%%")
    ap.add_argument("--best-first", action="store_true",
                    help="attempt closest-to-100%% first (higher ROI) instead of worst-first")
    ap.add_argument("--limit", type=int, default=5)
    ap.add_argument("--model", default=DEFAULT_MODEL)
    ap.add_argument("--apply", action="store_true",
                    help="keep improvements (default: dry-run, revert all)")
    args = ap.parse_args()

    key = load_key()
    src = Path(args.source)
    if not src.is_absolute():
        src = ROOT / src
    original = src.read_text(encoding="latin-1")
    tgt_obj = cc.find_target_obj(src)
    base_obj = cc.source_to_base_obj(src)

    base = measure_file(src)
    if base is None:
        sys.exit("baseline compile failed")
    base_matched, base_pcts = base
    print(f"[glm] {src.name} baseline {base_matched}/{len(base_pcts)} fns @100  (model {args.model})")

    if args.symbol:
        targets = [args.symbol]
        print(f"[glm] 1 target: {args.symbol} (currently {base_pcts.get(args.symbol, 0.0):.1f}%)\n")
    else:
        band = [(p, n) for n, p in base_pcts.items()
                if args.min_pct <= p < args.max_pct]
        band.sort(reverse=args.best_first)
        targets = [n for _, n in band][:args.limit]
        order = "best-first" if args.best_first else "worst-first"
        print(f"[glm] {len(targets)} target(s) in [{args.min_pct},{args.max_pct})% ({order})\n")

    wins = []
    cur = original
    for idx, fn in enumerate(targets, 1):
        span = find_fn_span(cur, fn)
        if not span:
            print(f"  [{idx}/{len(targets)}] {fn}: no C definition in file, skip")
            continue
        s0, e0, body = span
        asm = target_asm(tgt_obj, base_obj, fn)
        if not asm:
            print(f"  [{idx}/{len(targets)}] {fn}: no target asm, skip")
            continue
        prompt = (f"{CW_CRIB}\n\nFUNCTION: {fn}\n\nTARGET PPC ASSEMBLY:\n```\n{asm}\n```\n\n"
                  f"CURRENT C (rewrite to match the asm byte-for-byte):\n```c\n{body}\n```")
        before = base_pcts.get(fn, 0.0)
        try:
            reply = ask_glm(prompt, args.model, key)
        except Exception as ex:
            print(f"  [{idx}/{len(targets)}] {fn}: GLM error {ex}")
            continue
        cand = extract_c(reply, fn)
        if not cand:
            print(f"  [{idx}/{len(targets)}] {fn}: no C extracted from reply")
            continue

        trial = cur[:s0] + cand + cur[e0:]
        src.write_text(trial, encoding="latin-1")
        res = measure_file(src)
        if res is None:
            print(f"  [{idx}/{len(targets)}] {fn}: {before:.1f}% -> candidate FAILED to compile")
            src.write_text(cur, encoding="latin-1")
            continue
        new_matched, new_pcts = res
        after = new_pcts.get(fn, 0.0)
        if after > before and new_matched >= base_matched:
            print(f"  [{idx}/{len(targets)}] {fn}: {before:.1f}% -> {after:.1f}%  KEEP"
                  + ("  *100%*" if after >= 100.0 else ""))
            cur = trial
            base_matched, base_pcts = new_matched, new_pcts
            wins.append((fn, before, after))
        else:
            why = "no gain" if after <= before else f"file regressed {new_matched}<{base_matched}"
            print(f"  [{idx}/{len(targets)}] {fn}: {before:.1f}% -> {after:.1f}%  revert ({why})")
            src.write_text(cur, encoding="latin-1")

    # Final state
    if args.apply and wins:
        src.write_text(cur, encoding="latin-1")
        print(f"\n[glm] APPLIED {len(wins)} win(s) to {src.name}: "
              + ", ".join(f"{f}({b:.0f}->{a:.0f})" for f, b, a in wins))
    else:
        src.write_text(original, encoding="latin-1")
        tag = "dry-run, reverted" if not args.apply else "no wins"
        print(f"\n[glm] {tag}; {src.name} unchanged"
              + (f" (would-win: {len(wins)})" if wins else ""))


if __name__ == "__main__":
    main()
