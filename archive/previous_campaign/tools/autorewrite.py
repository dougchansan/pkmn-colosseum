#!/usr/bin/env python3
"""autorewrite.py - zero-token mechanical C-transform sweeper.

Stage 2.5 of the pipeline (see docs/decomp_notes/WORKFLOW.md). Consumes
diffclass's AUTO classifications and applies the *targeted* C-source
transform for that class to the single function, recompiles, and measures.
Keeps the transform only if that function's match% strictly improves;
otherwise reverts. No LLM.

Where automatch sweeps pragmas, autorewrite sweeps source rewrites that
this session proved repeatedly land matches but which an LLM otherwise
rediscovers by trial-and-error (~5-30K tokens each):

  signed-compare    flip *(u32*)<->*(s32*) (and u16/u8) at the deref site
  redundant-extend  collapse (s16)(s32)x -> (s16)x  (and s8 / short/int)

Each transform is applied only within the target function's line span, so
neighbouring functions are never touched. Whole-function application is
tried first; if that regresses or no-ops, a per-occurrence search flips
one site at a time.

Usage:
    python tools/autorewrite.py src/game/gs_render.c
    python tools/autorewrite.py src/game/gs_render.c --band 90 99.99
    python tools/autorewrite.py src/game/gs_render.c --symbol fn_800D56C0
    python tools/autorewrite.py src/game/gs_render.c --apply --report r.md
"""

import argparse
import re
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import automatch  # noqa: E402  (find_fn_def, measure, matched_count)
import diffclass  # noqa: E402  (fetch, classify)

# Each transform: (label, function(body_text) -> new_body_text). They are
# pure text substitutions scoped to one function's span.
PTR_FLIPS = [
    ("*(u32*)", "*(s32*)"), ("*(s32*)", "*(u32*)"),
    ("*(u16*)", "*(s16*)"), ("*(s16*)", "*(u16*)"),
    ("*(u8*)", "*(s8*)"), ("*(s8*)", "*(u8*)"),
]

EXTEND_COLLAPSE = [
    (re.compile(r"\(s16\)\s*\(s32\)"), "(s16)"),
    (re.compile(r"\(s16\)\s*\(u32\)"), "(s16)"),
    (re.compile(r"\(s8\)\s*\(s32\)"), "(s8)"),
    (re.compile(r"\(s8\)\s*\(u32\)"), "(s8)"),
    (re.compile(r"\(short\)\s*\(int\)"), "(short)"),
    (re.compile(r"\(char\)\s*\(int\)"), "(char)"),
]

# Explicit cast flips (NOT pointer-deref — `(s32)x`, not `*(s32*)x`).
# `(s32)` is not a substring of `*(s32*)` so these never collide with
# PTR_FLIPS.
CAST_FLIPS = [
    ("(u32)", "(s32)"), ("(s32)", "(u32)"),
    ("(u16)", "(s16)"), ("(s16)", "(u16)"),
    ("(u8)", "(s8)"), ("(s8)", "(u8)"),
]

# Signedness flip of a single declaration / parameter / return type:
# a type keyword immediately followed by an identifier (decl context).
DECL_FLIP = re.compile(
    r"\b(s32|u32|s16|u16|s8|u8)(\s+[A-Za-z_]\w*\s*[;,=)])")
_DECL_SWAP = {"s32": "u32", "u32": "s32", "s16": "u16", "u16": "s16",
              "s8": "u8", "u8": "s8"}


def variants_for(category, body):
    """Yield (label, new_body) candidates for a diffclass category."""
    if category == "signed-compare":
        # 1. pointer-deref flips (whole-fn then per-site) — proven path
        for a, b in PTR_FLIPS:
            if a in body:
                yield (f"all {a}->{b}", body.replace(a, b))
        for a, b in PTR_FLIPS:
            n = body.count(a)
            if n > 1:
                for i in range(n):
                    yield (f"{a}->{b} #site{i+1}",
                           _replace_nth(body, a, b, i))
        # 2. explicit cast flips — per-occurrence (non-pointer subclass)
        for a, b in CAST_FLIPS:
            n = body.count(a)
            for i in range(n):
                yield (f"cast {a}->{b} #site{i+1}",
                       _replace_nth(body, a, b, i))
        # 3. declaration / return-type signedness flips — per-site
        decls = list(DECL_FLIP.finditer(body))
        for i, m in enumerate(decls):
            t = m.group(1)
            new = (body[:m.start()] + _DECL_SWAP[t] + m.group(2)
                   + body[m.end():])
            yield (f"decl {t}->{_DECL_SWAP[t]}{m.group(2).strip()} "
                   f"#site{i+1}", new)
    elif category == "redundant-extend":
        for rx, repl in EXTEND_COLLAPSE:
            if rx.search(body):
                yield (f"collapse {rx.pattern}", rx.sub(repl, body))


def _replace_nth(text, old, new, n):
    """Replace only the n-th (0-based) occurrence of old with new."""
    idx = -1
    for _ in range(n + 1):
        idx = text.find(old, idx + 1)
        if idx == -1:
            return text
    return text[:idx] + new + text[idx + len(old):]


def span_text(lines, sig_idx, close_idx):
    return "".join(lines[sig_idx:close_idx + 1])


def splice(lines, sig_idx, close_idx, new_text):
    out = list(lines[:sig_idx])
    out += new_text.splitlines(keepends=True)
    out += lines[close_idx + 1:]
    return out


def _rewrite_fn_task(payload):
    """Top-level picklable worker: sweep one function's rewrite variants
    in isolation. payload = (orig_text, name, cat, si, ci, b0, src_path).
    Returns (name, cat, b0, best_pct, best_label, best_text, si, ci)."""
    orig_text, name, cat, si, ci, b0, src_path = payload
    lines = orig_text.splitlines(keepends=True)
    body = span_text(lines, si, ci)
    best_pct, best_label, best_text = b0, None, None
    for k, (label, new_body) in enumerate(variants_for(cat, body)):
        if new_body == body:
            continue
        trial = "".join(splice(lines, si, ci, new_body))
        m = automatch.measure_isolated(trial, src_path, [name],
                                       f"ar_{name}_{k}")
        if m is None:
            continue
        pct = m.get(name, 0.0)
        if pct > best_pct + 1e-6:
            best_pct, best_label, best_text = pct, label, new_body
        if best_pct >= 100.0:
            break
    return (name, cat, b0, best_pct, best_label, best_text, si, ci)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--band", nargs=2, type=float, default=[85.0, 99.99],
                    metavar=("LO", "HI"))
    ap.add_argument("--symbol")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--report")
    ap.add_argument("--jobs", "-j", type=int, default=1,
                    help="parallel workers (isolated temp builds)")
    ap.add_argument("--max-fns", type=int, default=0,
                    help="cap rewrite-class targets this run (0 = no cap)")
    args = ap.parse_args()

    src = Path(args.source)
    if not src.is_absolute():
        src = ROOT / src
    original = automatch.read_src(src)

    print(f"[autorewrite] baseline compile of {src.name} ...")
    base = automatch.measure(src, None)
    if base is None:
        sys.exit("baseline compile failed")
    base_matched = automatch.matched_count(base)
    print(f"[autorewrite] baseline {base_matched}/{len(base)} @ 100%")

    # diffclass on the freshly-built baseline .o
    j = diffclass.fetch(src, compile_first=False)
    left = {s["name"]: s for s in j["left"]["symbols"]
            if s.get("kind") == "SYMBOL_FUNCTION"}
    right = {s["name"]: s for s in j["right"]["symbols"]
             if s.get("kind") == "SYMBOL_FUNCTION"}

    lo, hi = args.band
    targets = []
    for name, rs in right.items():
        if not name.startswith("fn_"):
            continue
        pct = rs.get("match_percent", 0.0)
        if args.symbol:
            if name != args.symbol:
                continue
        elif not (lo <= pct < hi):
            continue
        cat, act, _ = diffclass.classify(
            left.get(name, {}).get("instructions", []),
            rs.get("instructions", []))
        if cat in ("signed-compare", "redundant-extend"):
            targets.append((pct, name, cat))
    targets.sort(key=lambda t: -t[0])
    if args.max_fns and len(targets) > args.max_fns:
        targets = targets[:args.max_fns]
        print(f"[autorewrite] capped to {args.max_fns} this run")
    print(f"[autorewrite] {len(targets)} rewrite-class targets "
          f"(signed-compare / redundant-extend)")

    lines = original.splitlines(keepends=True)
    results = []
    t0 = time.time()

    work = []
    for (b0, name, cat) in targets:
        loc = automatch.find_fn_def(lines, name)
        if loc is None:
            print(f"  {name} -- no C body, skip")
            continue
        work.append((b0, name, cat, loc[0], loc[1]))

    if args.jobs > 1 and work:
        from concurrent.futures import ProcessPoolExecutor, as_completed
        payloads = [(original, n, cat, si, ci, b0, str(src))
                    for (b0, n, cat, si, ci) in work]
        done = 0
        with ProcessPoolExecutor(max_workers=args.jobs) as ex:
            futs = [ex.submit(_rewrite_fn_task, p) for p in payloads]
            for fut in as_completed(futs):
                r = fut.result()
                name, cat, b0, bp, lbl = r[0], r[1], r[2], r[3], r[4]
                done += 1
                tag = ("=100" if bp >= 100 else f"+{bp - b0:.2f}") \
                    if lbl else "no change"
                print(f"  [{done}/{len(payloads)}] {name} [{cat}] "
                      f"{b0:.2f}% -> {bp:.2f}%  ({tag})"
                      + (f"  via {lbl}" if lbl else ""))
                results.append(r)
    else:
        for idx, (b0, name, cat, si, ci) in enumerate(work, 1):
            body = span_text(lines, si, ci)
            best_pct, best_label, best_text = b0, None, None
            for label, new_body in variants_for(cat, body):
                if new_body == body:
                    continue
                trial = splice(lines, si, ci, new_body)
                automatch.write_src(src, "".join(trial))
                m = automatch.measure(src, [name])
                if m is None:
                    continue
                pct = m.get(name, 0.0)
                if pct > best_pct + 1e-6:
                    best_pct, best_label, best_text = pct, label, new_body
                if best_pct >= 100.0:
                    break
            automatch.write_src(src, original)  # restore pristine
            tag = ("=100" if best_pct >= 100 else f"+{best_pct - b0:.2f}") \
                if best_label else "no change"
            print(f"  [{idx}/{len(work)}] {name} [{cat}] {b0:.2f}% -> "
                  f"{best_pct:.2f}%  ({tag})"
                  + (f"  via {best_label}" if best_label else ""))
            results.append((name, cat, b0, best_pct, best_label, best_text,
                            si, ci))

    if args.apply:
        wins = [r for r in results if r[4] and r[3] > r[2] + 1e-6]
        if wins:
            cur = original
            for name, cat, b0, bp, lbl, txt, si, ci in wins:
                ls = cur.splitlines(keepends=True)
                loc = automatch.find_fn_def(ls, name)
                if loc is None:
                    continue
                s2, c2 = loc
                cur = "".join(splice(ls, s2, c2, txt))
            automatch.write_src(src, cur)
            final = automatch.measure(src, None)
            if final is None or automatch.matched_count(final) < base_matched:
                automatch.write_src(src, original)
                print("[autorewrite] APPLY REVERTED -- net regression")
            else:
                print(f"[autorewrite] APPLIED {len(wins)} wins. "
                      f"matched {base_matched} -> "
                      f"{automatch.matched_count(final)}")
        else:
            print("[autorewrite] no improvements to apply")

    dt = time.time() - t0
    solved = [r for r in results if r[3] >= 100.0]
    improved = [r for r in results if r[4] and r[3] > r[2] + 1e-6]
    print(f"\n[autorewrite] done in {dt:.0f}s -- {len(solved)} reached "
          f"100%, {len(improved)} improved, "
          f"{len(targets) - len(improved)} unchanged")

    if args.report:
        with open(args.report, "w", encoding="utf-8") as f:
            f.write(f"# autorewrite report: {src.name}\n\n")
            f.write(f"baseline {base_matched}/{len(base)}, "
                    f"{len(targets)} targets, {dt:.0f}s\n\n")
            f.write("## Solved (zero-token mechanical rewrite)\n\n")
            for n, c, b, p, l, _t, _s, _e in solved:
                f.write(f"- **{n}** [{c}] {b:.2f}% -> 100% via `{l}`\n")
            f.write("\n## Improved but not 100%\n\n")
            for n, c, b, p, l, _t, _s, _e in improved:
                if p < 100:
                    f.write(f"- {n} [{c}] {b:.2f}% -> {p:.2f}% "
                            f"via `{l}`\n")
        print(f"[autorewrite] report -> {args.report}")


if __name__ == "__main__":
    main()
