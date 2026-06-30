#!/usr/bin/env python3
"""Parent-side selective merge for the pokemon.c band campaign.

Replaces the named functions' source spans in src/game/pokemon.c with the
versions from a band's scratch copy, then recompiles the canonical TU and
reports before/after match% for those functions. Refuses to apply a function
whose canonical match% would regress.

Usage:
  python tools/decomp_work/pkm_merge.py <band> <fn> [fn ...]
  python tools/decomp_work/pkm_merge.py <band> --dry <fn> [...]   # show spans only

A function's span runs from the line following the previous function's
closing brace (so address comments / pragmas attach to the function they
precede) through the function's own closing brace.
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
CANON = ROOT / "src" / "game" / "pokemon.c"
SCRATCH = ROOT / "tools" / "decomp_work" / "scratch"

FUNC_DEF = re.compile(r'^(?:static\s+|asm\s+)*(?:[A-Za-z_]\w*[\s*]+)+((?:fn_[0-9A-Fa-f]{8}|_?[A-Za-z]\w*__F\w*|fight\w+))\s*\([^;]*\)\s*(?:\{.*)?$')


def spans(text):
    """Map fn name -> (start_line, end_line) inclusive, 0-based."""
    lines = text.splitlines()
    out = {}
    prev_end = -1
    i = 0
    n = len(lines)
    while i < n:
        m = FUNC_DEF.match(lines[i])
        if m:
            name = m.group(1)
            # find opening brace
            bo = i
            while bo < n and "{" not in lines[bo]:
                bo += 1
            depth = 0
            j = bo
            while j < n:
                depth += lines[j].count("{") - lines[j].count("}")
                if depth <= 0 and j >= bo:
                    break
                j += 1
            out[name] = (prev_end + 1, j)
            prev_end = j
            i = j + 1
        else:
            i += 1
    return out, lines


def measure(fns):
    r = subprocess.run([sys.executable, str(ROOT / "tools" / "decomp_work" / "pkm_measure.py"), *fns],
                       capture_output=True, text=True, cwd=str(ROOT))
    print(r.stdout.strip())
    if "COMPILE FAILED" in r.stdout:
        return None
    vals = {}
    for ln in r.stdout.splitlines():
        m = re.match(r'\s+(fn_\w+)\s+([\d.]+)%', ln)
        if m:
            vals[m.group(1)] = float(m.group(2))
    return vals


def main():
    band = sys.argv[1]
    args = sys.argv[2:]
    dry = "--dry" in args
    fns = [a for a in args if not a.startswith("--")]
    scr = SCRATCH / f"pokemon_{band}.c"
    canon_text = CANON.read_text(errors="replace")
    scr_text = scr.read_text(errors="replace")
    cspans, clines = spans(canon_text)
    sspans, slines = spans(scr_text)

    print("=== BEFORE ===")
    before = measure(fns)
    if before is None:
        sys.exit("canonical TU does not compile; aborting")

    # apply bottom-up so line numbers stay valid
    order = sorted(fns, key=lambda f: cspans[f][0], reverse=True)
    newlines = list(clines)
    for fn in order:
        if fn not in cspans:
            sys.exit(f"{fn} not found in canonical")
        if fn not in sspans:
            sys.exit(f"{fn} not found in scratch {band}")
        cs, ce = cspans[fn]
        ss, se = sspans[fn]
        if dry:
            print(f"--- {fn}: canonical L{cs+1}-{ce+1} <- scratch L{ss+1}-{se+1}")
            continue
        newlines[cs:ce + 1] = slines[ss:se + 1]
    if dry:
        return
    CANON.write_text("\n".join(newlines) + "\n")

    print("=== AFTER ===")
    after = measure(fns)
    if after is None:
        print("REGRESSION: TU no longer compiles -- restoring pre-merge content")
        CANON.write_text(canon_text)
        sys.exit(1)
    bad = [f for f in fns if after.get(f, 0) < before.get(f, 0) - 1e-4]
    for f in fns:
        b, a = before.get(f, 0), after.get(f, 0)
        tag = "MATCH" if a >= 100.0 - 1e-6 else ("REGRESS" if f in bad else "")
        print(f"  {f}: {b:.2f}% -> {a:.2f}%  {tag}")
    if bad:
        print("NOTE: regressions present; revert manually if unwanted (git diff src/game/pokemon.c)")


if __name__ == "__main__":
    main()
