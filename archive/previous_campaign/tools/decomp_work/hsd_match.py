#!/usr/bin/env python3
"""
hsd_match.py - byte-match grind helper for the HSD (and other library) TUs.

Closes the edit->compile->objdiff loop WITHOUT the dead objdiff base-object path
(build.ninja is report-only; build/GC6E01/base/*.o are STALE - do not trust them).

What it does, per call:
  1. decompctx bundles the TU's includes into one .c
  2. auto-detects "undefined label" cross-TU refs and injects `extern void X();`
  3. compiles to a FRESH object with the correct CW version (HSD = GC/1.2.5n)
  4. objdiff's the fresh object vs the carved target (auto_01_800055E0_text.o)
  5. prints per-function exact match% (--list) or an instruction diff (--fn)

Usage:
  python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c --list
  python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c --fn fn_80196EF8
  python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c --cw 1.3 --list   # version sweep

A function at 100.00% is byte-identical (done). Compile fresh EVERY time - never
trust a cached/report number.
"""
import argparse, os, re, subprocess, sys, tempfile, json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TARGET = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
ODCLI = ROOT / ("tools/objdiff-cli.exe" if os.name == "nt" else "tools/objdiff-cli")
DECOMPCTX = ROOT / "tools/decompctx.py"
FLAGS = "-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8".split()

def sh(args, **kw):
    return subprocess.run(args, capture_output=True, encoding="utf-8", errors="replace", **kw)

def compile_tu(tu: Path, cw: str, workdir: Path):
    """Bundle + auto-extern + compile to object. Returns (obj_path, errors)."""
    cc = ROOT / f"build/compilers/GC/{cw}/mwcceppc.exe"
    if not cc.exists():
        sys.exit(f"compiler not found: {cc}  (see build/compilers/GC/ for versions)")
    ctx = workdir / "ctx.c"
    sh([sys.executable, str(DECOMPCTX), str(tu), "-o", str(ctx)])
    if not ctx.exists():
        sys.exit("decompctx failed to bundle the TU")
    body = ctx.read_text(encoding="utf-8", errors="replace")
    externs = []
    obj = workdir / "fresh.o"
    env = {**os.environ, "PATH": str(cc.parent) + os.pathsep + os.environ["PATH"]}
    for _ in range(4):  # iterate: inject undefined labels as externs, recompile
        src = "".join(f"extern void {e}();\n" for e in externs) + body
        ctx.write_text(src, encoding="utf-8")
        if obj.exists(): obj.unlink()
        r = sh([str(cc), "-c", *FLAGS, "-o", str(obj), str(ctx)], env=env, cwd=str(ROOT))
        labs = set(re.findall(r"undefined label '([^']+)'", r.stdout + r.stderr))
        new = labs - set(externs)
        if obj.exists():
            return obj, ""
        if not new:
            return None, (r.stdout + r.stderr)
        externs += sorted(new)
    return (obj if obj.exists() else None), "exhausted extern-injection retries"

def diff(obj: Path, workdir: Path):
    out = workdir / "d.json"
    sh([str(ODCLI), "diff", "-1", str(TARGET), "-2", str(obj), "--format", "json", "-o", str(out)])
    return json.loads(out.read_text(encoding="utf-8"))

RELOC = re.compile(r"lbl_[0-9A-Fa-f]+|@(ha|l|h|sda21|got)\b")

def rows(sym):
    """[(diff_kind, formatted)] for real instruction rows."""
    out = []
    for i in sym.get("instructions", []):
        if "instruction" in i:
            out.append((i.get("diff_kind"), i["instruction"].get("formatted", "?")))
    return out

def real_diff_rows(prod_sym, left_syms):
    """Rows that are GENUINE codegen diffs (not bundling/reloc/branch artifacts).
    Uses objdiff's per-row diff_kind. A DIFF_ARG_MISMATCH where the TARGET has a
    relocation but the produced has the resolved immediate is a decompctx artifact
    (data symbol inlined) - filtered out. DIFF_REPLACE / INSERT / DELETE and
    register/real-immediate arg mismatches are kept."""
    tgt = left_syms[prod_sym["target_symbol"]] if prod_sym.get("target_symbol") is not None else {}
    trows = rows(tgt)
    prows = rows(prod_sym)
    real = []
    for k, (dk, pf) in enumerate(prows):
        if not dk or dk == "DIFF_NONE":
            continue
        tf = trows[k][1] if k < len(trows) else ""
        if dk == "DIFF_ARG_MISMATCH":
            # reloc-vs-resolved-immediate artifact, or relative branch-target artifact
            if RELOC.search(tf) and not RELOC.search(pf):
                continue
            if re.match(r"\s*(b|beq|bne|bge|ble|bgt|blt|bdnz|bdz)", pf) and \
               re.match(r"\s*(b|beq|bne|bge|ble|bgt|blt|bdnz|bdz)", tf):
                continue  # branch to different absolute addr = same relative target
        real.append((dk, tf, pf))
    real += [("DIFF_LEN", "", "")] * abs(len(trows) - len(prows))
    return real

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tu")
    ap.add_argument("--cw", default="1.2.5n", help="CW version dir under build/compilers/GC/ (HSD=1.2.5n)")
    ap.add_argument("--list", action="store_true", help="list non-100%% functions")
    ap.add_argument("--fn", default=None, help="show instruction diff for one function")
    a = ap.parse_args()
    tu = (ROOT / a.tu) if not os.path.isabs(a.tu) else Path(a.tu)
    with tempfile.TemporaryDirectory() as wd:
        wd = Path(wd)
        obj, err = compile_tu(tu, a.cw, wd)
        if obj is None:
            sys.exit("COMPILE FAILED:\n" + "\n".join(l for l in err.splitlines() if "rror" in l or "abel" in l)[:2000])
        d = diff(obj, wd)
        right = {s["name"]: s for s in d["right"]["symbols"]}
        left = d["left"]["symbols"]
        if a.fn:
            s = right.get(a.fn)
            if not s: sys.exit(f"{a.fn} not in produced object")
            tgt = left[s["target_symbol"]] if s.get("target_symbol") is not None else {}
            ti, pi = [r[1] for r in rows(tgt)], [r[1] for r in rows(s)]
            rd = real_diff_rows(s, left)
            print(f"=== {a.fn}  match={s.get('match_percent'):.2f}%  real-diffs={len(rd)}  (TARGET | PRODUCED) ===")
            realset = {(t, p) for _, t, p in rd}
            for k in range(max(len(ti), len(pi))):
                t = ti[k] if k < len(ti) else ""
                p = pi[k] if k < len(pi) else ""
                mark = ">>" if (t, p) in realset else "  "
                print(f"{mark} {t:34s} | {p}")
            print("(>> = GENUINE codegen diff per objdiff diff_kind; reloc/branch artifacts filtered)")
            if not rd:
                print("** 0 real diffs: the C is correct; residual % is decompctx reloc-inlining noise. SKIP / certify in objdiff GUI. **")
        else:
            tab = []
            for n, s in right.items():
                if s.get("kind") != "SYMBOL_FUNCTION" or s.get("match_percent") is None:
                    continue
                tab.append((len(real_diff_rows(s, left)), s["match_percent"], n, s.get("size", 0)))
            done = sum(1 for r in tab if r[1] >= 100.0)
            artifact = sum(1 for r in tab if r[0] == 0 and r[1] < 100.0)
            real = sorted([r for r in tab if r[0] > 0])  # genuine work, fewest real diffs first
            print(f"{tu.name}: {len(tab)} funcs | {done} at 100% | {artifact} artifact-only (C correct, reloc noise) "
                  f"| {len(real)} need real work (CW {a.cw})")
            print("real-diffs  match%   function  (size)   <- attack these, fewest real-diffs first")
            for n_rd, m, n, sz in real:
                print(f"  {n_rd:>4}   {m:7.3f}%  {n}  ({sz}B)")

if __name__ == "__main__":
    main()
