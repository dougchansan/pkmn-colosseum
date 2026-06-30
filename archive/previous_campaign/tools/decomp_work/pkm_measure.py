#!/usr/bin/env python3
"""Fast compile+measure loop for src/game/pokemon.c.

Usage:
  python tools/decomp_work/pkm_measure.py                 # compile + full summary
  python tools/decomp_work/pkm_measure.py fn_801F1888 ... # compile + report named fns
  python tools/decomp_work/pkm_measure.py --no-compile    # measure only
"""
import sys, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(ROOT / "tools"))

def main():
    args = [a for a in sys.argv[1:]]
    do_compile = "--no-compile" not in args
    args = [a for a in args if not a.startswith("--")]
    if do_compile:
        r = subprocess.run([sys.executable, str(ROOT / "tools" / "compile_check.py"),
                            str(ROOT / "src" / "game" / "pokemon.c")],
                           capture_output=True, text=True)
        if "OK:" not in r.stdout:
            print("COMPILE FAILED")
            print(r.stdout[-3000:])
            print(r.stderr[-3000:])
            sys.exit(1)
    import measure_cache
    target = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
    base = ROOT / "build" / "GC6E01" / "base" / "game" / "pokemon.o"
    funcs = measure_cache.diff_funcs(target, base)
    try:
        measure_cache.flush()
    except Exception:
        pass
    exact = sum(1 for f in funcs if float(f["match"]) >= 100.0 - 1e-6)
    print(f"pokemon.o: {exact}/{len(funcs)} byte-exact")
    if args:
        want = set(args)
        for f in funcs:
            if f["name"] in want:
                print(f"  {f['name']}  {float(f['match']):.2f}%")
    else:
        for f in sorted(funcs, key=lambda f: float(f["match"])):
            m = float(f["match"])
            if m < 100.0 - 1e-6:
                print(f"  {f['name']}  {m:.2f}%")

if __name__ == "__main__":
    main()
