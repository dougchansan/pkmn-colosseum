#!/usr/bin/env python3
"""run.py - one-command driver for the string-based symbol-map pipeline.

Runs, in order:
  0. mine_xrefs       dtk asm -> strings.json + fn_strings.json
  1. attribute_tus    __FILE__ xrefs -> tu_attribution.md / tu_evidence.json
  2. propose_names    log/assert strings -> name_proposals.*
  3. port_xd          XD (GXXE01) real names (skipped without --xd-asm)
  4. build_symbol_map merge -> proposed_symbols.txt / symbols.with_proposals.txt

Defaults assume invocation from the repo root. Build the Colosseum asm first
with `python configure.py && ninja` (or it already exists in build/GC6E01/asm).
To enable the XD port, extract XD's main.dol and split it (see README), then
pass --xd-asm.
"""

import argparse
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent.parent


def run(script: str, *a) -> None:
    cmd = [sys.executable, str(HERE / script), *map(str, a)]
    print(f"\n$ {' '.join(cmd)}")
    subprocess.run(cmd, check=True)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--asm-dir", type=Path, default=REPO / "build/GC6E01/asm")
    ap.add_argument("--out-dir", type=Path,
                    default=REPO / "config/GC6E01/symbolmap")
    ap.add_argument("--symbols", type=Path,
                    default=REPO / "config/GC6E01/symbols.txt")
    ap.add_argument("--splits", type=Path,
                    default=REPO / "config/GC6E01/splits_refined.txt")
    ap.add_argument("--xd-symbols", type=Path, default=REPO /
                    "tools/decomp_work/refs/xd-decomp/config/GXXE01/symbols.txt")
    ap.add_argument("--xd-asm", type=Path, default=None,
                    help="XD asm dir from `dtk split` (enables the XD port)")
    args = ap.parse_args()

    run("mine_xrefs.py", "--asm-dir", args.asm_dir, "--out-dir", args.out_dir)
    run("attribute_tus.py", "--sm-dir", args.out_dir, "--splits", args.splits)
    run("propose_names.py", "--sm-dir", args.out_dir, "--symbols", args.symbols)
    if args.xd_asm and Path(args.xd_asm).is_dir():
        run("port_xd.py", "--sm-dir", args.out_dir,
            "--xd-symbols", args.xd_symbols,
            "--col-asm", args.asm_dir, "--xd-asm", args.xd_asm)
    else:
        run("port_xd.py", "--sm-dir", args.out_dir,
            "--xd-symbols", args.xd_symbols)
    run("build_symbol_map.py", "--sm-dir", args.out_dir,
        "--symbols", args.symbols)
    print("\n[done] see", args.out_dir)


if __name__ == "__main__":
    main()
