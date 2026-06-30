#!/usr/bin/env python3
"""
flag_sweep.py - sweep CodeWarrior codegen flags on a TU and score by objdiff.

Supersedes the earlier flag_sweep (which compiled via compile_check's wrong-flags
path, MUTATED the base build, and -- critically -- never tested `-use_lmw_stmw off`,
so its "flag sweep REFUTED" conclusion did NOT cover the stmw lever). This version:
  * baselines on the EXACT flags build.ninja uses (from compile_config.json), so the
    baseline reproduces the real build (see feedback_compile_check_wrong_flags);
  * compiles each variant to a TEMP object -- never edits config/build or base/*.o;
  * tests `-use_lmw_stmw off` (the W3 stmw-threshold suspect, see WALLS.md) + `-inline`
    modes + opt levels, and reports the per-FUNCTION match% delta for each.

Applying a winning flag is a separate, human-reviewed step (a per-file override in
config/GC6E01/compile_config.json) -- this harness only measures.

Usage:
  python tools/decomp_work/flag_sweep.py src/game/colosseum_battle.c
  python tools/decomp_work/flag_sweep.py src/game/gs_material.c --fns fn_800E3604
  python tools/decomp_work/flag_sweep.py <file> --variants baseline,lmw_off --only-changed
"""
import os
import argparse, json, sys, subprocess, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
INCLUDE = ROOT / "include"
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
MWCC_GC = ROOT / "tools" / "mwcc_compiler" / "GC"
CONFIG = ROOT / "config" / "GC6E01" / "compile_config.json"


def _replace_tok(flags, a, b):
    return [b if t == a else t for t in flags]


def _replace_seq(flags, seq, repl):
    out, i = [], 0
    while i < len(flags):
        if flags[i:i + len(seq)] == seq:
            out.extend(repl); i += len(seq)
        else:
            out.append(flags[i]); i += 1
    return out


def _set_inline(flags, mode):
    out, i = [], 0
    while i < len(flags):
        if flags[i] == "-inline":
            i += 2; continue
        out.append(flags[i]); i += 1
    out.extend(["-inline", mode])
    return out


VARIANTS = {
    "baseline":        lambda f: f,
    "lmw_off":         lambda f: _replace_seq(f, ["-use_lmw_stmw", "on"], ["-use_lmw_stmw", "off"]),
    "inline_off":      lambda f: _set_inline(f, "off"),
    "inline_noauto":   lambda f: _set_inline(f, "noauto"),
    "inline_deferred": lambda f: _set_inline(f, "deferred"),
    "inline_auto":     lambda f: _set_inline(f, "auto"),
    "O3p":             lambda f: _replace_tok(f, "-O4,p", "-O3,p"),
    "O4s":             lambda f: _replace_tok(f, "-O4,p", "-O4,s"),
}
DEFAULT_VARIANTS = ["baseline", "lmw_off", "inline_off", "inline_noauto", "inline_deferred"]


def file_compiler_and_flags(src_rel):
    cfg = json.loads(CONFIG.read_text())
    ent = cfg.get("overrides", {}).get(src_rel, {})
    comp = ent.get("compiler") or cfg["default"]["compiler"]
    flags_str = ent.get("flags") or cfg["default"]["flags"]
    ver = comp.split("/", 1)[1] if "/" in comp else comp
    return ver, flags_str.split()


def compile_variant(src, ver, flags, out_o):
    mwcc = MWCC_GC / ver / "mwcceppc.exe"
    cmd = [str(mwcc), "-c", *flags, "-i", "include", "-o", str(out_o), str(src)]
    r = subprocess.run(cmd, cwd=str(ROOT), capture_output=True, text=True)
    return r.returncode == 0, (r.stdout + r.stderr)


def objdiff_match(out_o):
    cmd = [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(out_o),
           "-o", "-", "--format", "json", "-c", "ppc.calculatePoolRelocations=false"]
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return None
    try:
        j = json.loads(r.stdout)
    except ValueError:
        return None
    out = {}
    for s in j.get("right", {}).get("symbols", []):
        if s.get("kind") != "SYMBOL_FUNCTION":
            continue
        nm = s.get("name", "")
        if nm.startswith("fn_") and s.get("match_percent") is not None:
            out[nm] = round(float(s["match_percent"]), 4)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file")
    ap.add_argument("--fns", default=None, help="comma-separated fn_ names (default: all)")
    ap.add_argument("--variants", default=",".join(DEFAULT_VARIANTS))
    ap.add_argument("--only-changed", action="store_true")
    a = ap.parse_args()

    src = (ROOT / a.file).resolve() if not Path(a.file).is_absolute() else Path(a.file)
    if not src.exists():
        sys.exit(f"no such file: {src}")
    src_rel = str(src.relative_to(ROOT)).replace("\\", "/")
    want = set(x.strip() for x in a.fns.split(",")) if a.fns else None
    variants = [v.strip() for v in a.variants.split(",") if v.strip() in VARIANTS]

    ver, base_flags = file_compiler_and_flags(src_rel)
    print(f"file: {src_rel}   compiler: GC/{ver}")
    print(f"base flags: {' '.join(base_flags)}")
    print(f"variants: {', '.join(variants)}\n")

    results = {}
    tmpdir = Path(tempfile.mkdtemp(prefix="flagsweep_"))
    try:
        for v in variants:
            flags = VARIANTS[v](list(base_flags))
            out_o = tmpdir / f"{v}.o"
            ok, log = compile_variant(src, ver, flags, out_o)
            if not ok:
                errs = "; ".join(l.strip() for l in log.splitlines() if "rror" in l)[:300]
                print(f"  [{v}] COMPILE FAILED  {errs}")
                results[v] = {}; continue
            m = objdiff_match(out_o)
            results[v] = m or {}
            if m is None:
                print(f"  [{v}] objdiff failed")
    finally:
        for f in tmpdir.glob("*"):
            try: f.unlink()
            except OSError: pass
        try: tmpdir.rmdir()
        except OSError: pass

    allfns = set()
    for v in variants:
        allfns |= set(results[v].keys())
    if want:
        allfns &= want
    base = results.get("baseline", {})
    w = 15
    print(f"  {'function':<14} " + " ".join(f"{v[:w]:>{w}}" for v in variants))
    improved = []
    for fn in sorted(allfns):
        vals = [results[v].get(fn) for v in variants]
        if a.only_changed:
            present = [x for x in vals if x is not None]
            if len(set(present)) <= 1:
                continue
        cells = []
        b = base.get(fn)
        for v, x in zip(variants, vals):
            if x is None:
                cells.append(f"{'-':>{w}}"); continue
            tag = "  "
            if v != "baseline" and b is not None:
                if x > b + 1e-4: tag = " +"
                elif x < b - 1e-4: tag = " -"
            cells.append(f"{x:>{w-2}.2f}{tag}")
            if v != "baseline" and b is not None and x >= 99.9999 > b:
                improved.append((fn, v, b, x))
        print(f"  {fn:<14} " + " ".join(cells))

    print()
    if improved:
        print("=== NEW 100% from a flag (candidate per-file override) ===")
        for fn, v, b, x in improved:
            print(f"  {fn}: {b:.2f}% -> 100% with [{v}]")
    print("=== net effect vs baseline (per variant) ===")
    for v in variants:
        if v == "baseline":
            continue
        up = dn = 0
        for fn in allfns:
            b = base.get(fn); x = results[v].get(fn)
            if b is None or x is None:
                continue
            if x > b + 1e-4: up += 1
            elif x < b - 1e-4: dn += 1
        n100b = sum(1 for fn in allfns if (base.get(fn) or 0) >= 99.9999)
        n100v = sum(1 for fn in allfns if (results[v].get(fn) or 0) >= 99.9999)
        print(f"  {v:<16} improved {up:>4}  regressed {dn:>4}   100%: {n100b}->{n100v}")


if __name__ == "__main__":
    main()
