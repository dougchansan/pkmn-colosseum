#!/usr/bin/env python3
"""automatch.py - zero-token deterministic match sweeper.

For every near-miss function in a source file, mechanically try a curated
catalog of function-local `#pragma push`/`pop` variants, recompile, and
measure that one symbol with objdiff-cli. Keep the best variant; revert the
rest. No LLM in the loop.

This is the mandatory PRE-PASS before any LLM agent touches a file: it
auto-lands the mechanical wins (peephole/scheduling/opt-level/fp_contract
permutations) and PROVES which functions are genuinely blocked, so agent
tokens are only spent where deterministic sweeps plateau.

Usage:
    python tools/automatch.py src/game/scene_init.c
    python tools/automatch.py src/game/scene_init.c --band 85 99.99
    python tools/automatch.py src/game/scene_init.c --symbol fn_80037180
    python tools/automatch.py src/game/scene_init.c --apply
    python tools/automatch.py src/game/scene_init.c --apply --report out.md

Without --apply it is a dry run (reports what WOULD improve). With --apply
it writes the winning pragma stack for each improved function, then
re-measures the whole file to guarantee no net regression before keeping.

Safety:
  - Never edits `#if 0` asm blocks or `*_fn_*.inc` files.
  - Only inserts `#pragma push` ... `#pragma pop` around the active C body.
  - Verifies whole-file matched-count is monotonic before accepting --apply.
"""

import os
import argparse
import json
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")


def read_src(p):
    """Read preserving exact line endings (no CRLF<->LF translation)."""
    with open(p, "r", encoding="utf-8", errors="replace", newline="") as f:
        return f.read()


def write_src(p, s):
    """Write verbatim — no newline translation, so round-trips are
    byte-exact and win-commits aren't polluted with CRLF->LF noise."""
    with open(p, "w", encoding="utf-8", newline="") as f:
        f.write(s)

sys.path.insert(0, str(ROOT / "tools"))
import compile_check  # noqa: E402  (reuse its compile + path logic)

# Curated variant catalog. Each entry is the list of pragma lines injected
# between `#pragma push` and the function signature. Order matters: the
# documented winners (see memory feedback_*) come first so we stop early.
PRAGMA_VARIANTS = [
    [],  # baseline (no push/pop) — establishes the reference match%
    ["#pragma peephole off"],
    ["#pragma scheduling off"],
    ["#pragma scheduling on"],
    ["#pragma optimization_level 1"],
    ["#pragma optimization_level 2"],
    ["#pragma optimization_level 3"],
    ["#pragma fp_contract on"],
    ["#pragma peephole off", "#pragma scheduling on"],
    ["#pragma peephole off", "#pragma scheduling off"],
    ["#pragma peephole off", "#pragma optimization_level 2"],
    ["#pragma scheduling on", "#pragma fp_contract on"],
    ["#pragma optimization_level 2", "#pragma fp_contract on"],
    ["#pragma optimization_level 2", "#pragma peephole off",
     "#pragma scheduling on"],
]

def find_fn_def(lines, name):
    """Locate a function's C definition by name, format-agnostic.

    Returns (sig_idx, close_idx): the line index of the signature and of the
    line holding the body's final closing brace. Works whether the file uses
    `/* fn_X - 0x.. */` headers, `/* Address: 0x.. | Ghidra import */`
    headers, or no headers at all; signature and `{` may be on separate
    lines (K&R style). Skips `#if 0 .. #else` asm-wrapper regions and
    `;`-terminated prototypes. Returns None if no plain C body exists.
    """
    pat = re.compile(r"\b" + re.escape(name) + r"\s*\(")
    in_if0 = False
    n = len(lines)
    i = 0
    while i < n:
        s = lines[i].strip()
        if s.startswith("#if 0"):
            in_if0 = True
            i += 1
            continue
        if s.startswith("#else") or s.startswith("#endif"):
            in_if0 = False
            i += 1
            continue
        if (not in_if0 and pat.search(lines[i])
                and "asm " not in lines[i]
                and "#include" not in lines[i]
                and not lines[i].lstrip().startswith(("/*", "*", "//"))
                and re.match(r"^[A-Za-z_][\w \t\*]*$",
                             lines[i].split(name)[0])):
            # Accumulate from the signature line until the first top-level
            # '{' (definition) or ';' (prototype), whichever comes first.
            sig = ""
            end_line = i
            decided = None
            for t in range(i, n):
                sig += lines[t]
                end_line = t
                bpos = sig.find("{")
                spos = sig.find(";")
                if bpos != -1 and (spos == -1 or bpos < spos):
                    decided = "def"
                    break
                if spos != -1 and (bpos == -1 or spos < bpos):
                    decided = "proto"
                    break
            if decided != "def":
                i = end_line + 1
                continue
            # brace-match from the line containing the first '{'
            open_line = end_line
            for t in range(i, n):
                if "{" in lines[t]:
                    open_line = t
                    break
            depth = 0
            seen = False
            for t in range(open_line, n):
                for ch in lines[t]:
                    if ch == "{":
                        depth += 1
                        seen = True
                    elif ch == "}":
                        depth -= 1
                if seen and depth == 0:
                    return (i, t)
            return None
        i += 1
    return None


def list_fn_names(text):
    """All fn_XXXXXXXX names that have a C definition in the file."""
    return sorted(set(re.findall(r"\b(fn_[0-9A-Fa-f]{8})\s*\(", text)))


def measure(src_path, symbols):
    """Compile src and return {symbol: match_percent} for the given symbols."""
    try:
        base_o = compile_check.compile_source(src_path)
    except SystemExit:
        return None  # compile failed
    cmd = [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
           "-o", "-", "--format", "json",
           "-c", "ppc.calculatePoolRelocations=false"]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    if r.returncode != 0:
        return None
    j = json.loads(r.stdout)
    out = {}
    for s in j.get("right", {}).get("symbols", []):
        if s.get("kind") == "SYMBOL_FUNCTION":
            out[s.get("name", "")] = s.get("match_percent", 0.0)
    return {s: out.get(s, 0.0) for s in symbols} if symbols else out


_ISO_CACHE = {}


def measure_isolated(content, src_path, symbols, tag):
    """Compile `content` (full file text) to a private temp .o and objdiff
    just `symbols`. No shared state — safe to run from many processes at
    once. Returns {sym: pct} or None on compile failure."""
    import tempfile
    sp = Path(src_path).resolve()
    key = str(sp)
    if key not in _ISO_CACHE:
        ver = compile_check.get_file_compiler_version(sp)
        _ISO_CACHE[key] = (str(compile_check.get_compiler(ver)),
                           compile_check.get_file_cflags(sp))
    compiler, cflags = _ISO_CACHE[key]
    td = Path(tempfile.gettempdir())
    tc = td / f"am_{tag}.c"
    to = td / f"am_{tag}.o"
    try:
        with open(tc, "w", encoding="utf-8", newline="") as f:
            f.write(content)
        r = subprocess.run([compiler, "-c", "-o", str(to), *cflags, str(tc)],
                           capture_output=True, text=True, cwd=str(ROOT))
        if r.returncode != 0 or not to.exists():
            return None
        d = subprocess.run(
            [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(to),
             "-o", "-", "--format", "json",
             "-c", "ppc.calculatePoolRelocations=false"],
            capture_output=True, text=True, cwd=str(ROOT))
        if d.returncode != 0:
            return None
        j = json.loads(d.stdout)
        out = {}
        for s in j.get("right", {}).get("symbols", []):
            if s.get("kind") == "SYMBOL_FUNCTION":
                out[s.get("name", "")] = s.get("match_percent", 0.0)
        return {s: out.get(s, 0.0) for s in symbols} if symbols else out
    finally:
        for p in (tc, to):
            try:
                p.unlink()
            except OSError:
                pass


def _sweep_fn_task(payload):
    """Top-level (picklable) worker: sweep one function's pragma variants
    in isolation. payload = (orig_text, name, sig_idx, close_idx, b0,
    src_path, variants). Returns (name, b0, best_pct, best_variant)."""
    orig_text, name, si, ci, b0, src_path, variants = payload
    lines = orig_text.splitlines(keepends=True)
    best_pct, best_var = b0, None
    for k, variant in enumerate(variants):
        if not variant:
            continue
        trial = "".join(wrap_block(lines, si, ci, variant))
        m = measure_isolated(trial, src_path, [name],
                             f"{name}_{k}")
        if m is None:
            continue
        pct = m.get(name, 0.0)
        if pct > best_pct + 1e-6:
            best_pct, best_var = pct, variant
        if best_pct >= 100.0:
            break
    return (name, b0, best_pct, best_var)


def matched_count(pcts):
    return sum(1 for v in pcts.values() if v >= 100.0)


def wrap_block(lines, sig_idx, close_idx, pragmas):
    """Return a new line list with push/pragmas around [sig_idx, close_idx]."""
    if not pragmas:
        return list(lines)
    nl = "\r\n" if (lines and lines[0].endswith("\r\n")) else "\n"
    new = list(lines)
    block = [f"#pragma push{nl}"] + [p + nl for p in pragmas]
    new[sig_idx:sig_idx] = block
    # close_idx shifts by len(block)
    ins_at = close_idx + len(block) + 1
    new[ins_at:ins_at] = [f"#pragma pop{nl}"]
    return new


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--band", nargs=2, type=float, default=[80.0, 99.99],
                    metavar=("LO", "HI"),
                    help="only sweep functions with LO <= match%% < HI")
    ap.add_argument("--symbol", help="sweep just this one function")
    ap.add_argument("--apply", action="store_true",
                    help="write winning pragmas (default: dry run)")
    ap.add_argument("--report", help="write a markdown report here")
    ap.add_argument("--jobs", "-j", type=int, default=1,
                    help="parallel workers (isolated temp builds). "
                         "N>1 sweeps functions concurrently.")
    ap.add_argument("--max-fns", type=int, default=0,
                    help="cap functions swept this run (0 = no cap). "
                         "Bounds wall-time so a loop iteration finishes "
                         "inside the background-task cap.")
    args = ap.parse_args()

    src = Path(args.source)
    if not src.is_absolute():
        src = ROOT / src
    original = read_src(src)

    # Baseline measurement of every function in the file.
    print(f"[automatch] baseline compile of {src.name} ...")
    base = measure(src, None)
    if base is None:
        sys.exit("baseline compile failed — fix the file first")
    base_matched = matched_count(base)
    print(f"[automatch] baseline: {base_matched}/{len(base)} @ 100%")

    if args.symbol:
        targets = [args.symbol]
    else:
        lo, hi = args.band
        targets = sorted(
            (n for n, p in base.items() if lo <= p < hi),
            key=lambda n: -base[n],
        )
    if args.max_fns and len(targets) > args.max_fns:
        targets = targets[:args.max_fns]
        print(f"[automatch] capped to {args.max_fns} functions this run")
    print(f"[automatch] {len(targets)} near-miss targets in band "
          f"{args.band[0]}-{args.band[1]}%")

    lines = original.splitlines(keepends=True)

    results = []  # (name, base%, best%, best_variant)
    t0 = time.time()

    # Build the work list (skip no-body functions up front).
    work = []
    for name in targets:
        loc = find_fn_def(lines, name)
        if loc is None:
            print(f"  {name} — no C body found, skip")
            results.append((name, base.get(name, 0.0),
                            base.get(name, 0.0), None))
            continue
        work.append((name, loc[0], loc[1], base[name]))

    if args.jobs > 1 and work:
        from concurrent.futures import ProcessPoolExecutor, as_completed
        payloads = [(original, n, si, ci, b0, str(src), PRAGMA_VARIANTS)
                    for (n, si, ci, b0) in work]
        done = 0
        with ProcessPoolExecutor(max_workers=args.jobs) as ex:
            futs = {ex.submit(_sweep_fn_task, p): p[1] for p in payloads}
            for fut in as_completed(futs):
                name, b0, best_pct, best_var = fut.result()
                done += 1
                tag = ("=100" if best_pct >= 100
                       else f"+{best_pct - b0:.2f}") \
                    if best_var else "no change"
                print(f"  [{done}/{len(payloads)}] {name} {b0:.2f}% -> "
                      f"{best_pct:.2f}%  ({tag})"
                      + (f"  {best_var}" if best_var else ""))
                results.append((name, b0, best_pct, best_var))
    else:
        for idx, (name, sig_idx, close_idx, b0) in enumerate(work, 1):
            best_pct, best_var = b0, None
            for variant in PRAGMA_VARIANTS:
                if not variant:
                    continue
                trial = wrap_block(lines, sig_idx, close_idx, variant)
                write_src(src, "".join(trial))
                m = measure(src, [name])
                if m is None:
                    continue
                pct = m.get(name, 0.0)
                if pct > best_pct + 1e-6:
                    best_pct, best_var = pct, variant
                if best_pct >= 100.0:
                    break
            write_src(src, original)  # restore pristine
            tag = ("=100" if best_pct >= 100 else f"+{best_pct - b0:.2f}") \
                if best_var else "no change"
            print(f"  [{idx}/{len(work)}] {name} {b0:.2f}% -> "
                  f"{best_pct:.2f}%  ({tag})"
                  + (f"  {best_var}" if best_var else ""))
            results.append((name, b0, best_pct, best_var))

    # Apply phase: re-insert every winning variant, then verify no regression.
    if args.apply:
        wins = [(n, v) for (n, b, p, v) in results if v and p > b + 1e-6]
        if wins:
            cur_text = original
            for name, variant in wins:
                ls = cur_text.splitlines(keepends=True)
                loc = find_fn_def(ls, name)
                if loc is None:
                    continue
                si, ci = loc
                cur_text = "".join(wrap_block(ls, si, ci, variant))
            write_src(src, cur_text)
            final = measure(src, None)
            if final is None or matched_count(final) < base_matched:
                write_src(src, original)
                print("[automatch] APPLY REVERTED — net regression detected")
            else:
                print(f"[automatch] APPLIED {len(wins)} wins. "
                      f"matched {base_matched} -> {matched_count(final)}")
        else:
            print("[automatch] no improvements to apply")

    dt = time.time() - t0
    improved = [r for r in results if r[3] and r[2] > r[1] + 1e-6]
    solved = [r for r in results if r[2] >= 100.0]
    print(f"\n[automatch] done in {dt:.0f}s — "
          f"{len(solved)} reached 100%, {len(improved)} improved, "
          f"{len(targets) - len(improved)} unchanged (deterministically "
          f"blocked — escalate THOSE to an LLM agent)")

    if args.report:
        with open(args.report, "w", encoding="utf-8") as f:
            f.write(f"# automatch report: {src.name}\n\n")
            f.write(f"baseline {base_matched}/{len(base)} @ 100%, "
                    f"swept {len(targets)} near-misses in {dt:.0f}s\n\n")
            f.write("## Solved (deterministic, zero-token)\n\n")
            for n, b, p, v in solved:
                f.write(f"- **{n}** {b:.2f}% -> 100% via `{' '.join(v)}`\n")
            f.write("\n## Improved but not 100%\n\n")
            for n, b, p, v in improved:
                if p < 100:
                    f.write(f"- {n} {b:.2f}% -> {p:.2f}% "
                            f"via `{' '.join(v)}`\n")
            f.write("\n## Blocked — escalate to LLM agent\n\n")
            for n, b, p, v in results:
                if not v or p <= b + 1e-6:
                    f.write(f"- {n} stuck at {b:.2f}% "
                            f"(no pragma variant helped)\n")
        print(f"[automatch] report -> {args.report}")


if __name__ == "__main__":
    main()
