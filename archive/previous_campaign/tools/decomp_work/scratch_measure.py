#!/usr/bin/env python3
"""scratch_measure.py - collision-free per-function match measurement.

Each parallel decomp agent works on its OWN copy of a 40k-line source file and
compiles it to its OWN object, so many agents can measure concurrently in the
shared working tree without clobbering src/game/colosseum_battle.c or the
canonical build/GC6E01/base/game/colosseum_battle.o.

Usage:
  python tools/decomp_work/scratch_measure.py <scratch_src.c> <fn> [<fn> ...] [--tag NAME]

It compiles <scratch_src.c> with the SAME CW version + flags the real TU uses
(read from config/GC6E01/compile_config.json, default = GC/1.3 -O4,p) to a unique
temp object, then objdiffs each requested symbol against the canonical target
object. Prints "<match%>  <fn>" per symbol. Match% is the authoritative number
(same objdiff invocation match_scan_file.py uses).
"""
import sys, os, re, json, subprocess, tempfile, argparse
from pathlib import Path

ROOT = Path("C:/Users/douglaswhittingham/pkmn-colosseum")
TOOLS = ROOT / "tools"
OBJDIFF = TOOLS / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
MWCC = TOOLS / "mwcc_compiler" / "GC"
INCLUDE = ROOT / "include"
CFG = json.loads((ROOT / "config" / "GC6E01" / "compile_config.json").read_text())


def flags_for(rel_src):
    ov = CFG.get("overrides", {})
    if rel_src in ov and "flags" in ov[rel_src]:
        flags = ov[rel_src]["flags"].split()
        ver = ov[rel_src].get("compiler", CFG["default"]["compiler"])
    else:
        flags = CFG["default"]["flags"].split()
        ver = CFG["default"]["compiler"]
    flags += ["-i", str(INCLUDE)]
    ver = ver.split("/")[-1]  # "GC/1.3" -> "1.3"
    return ver, flags


def compile_scratch(src, out_obj, rel_src):
    ver, flags = flags_for(rel_src)
    mwcc = MWCC / ver / "mwcceppc.exe"
    if not mwcc.exists():
        return False, f"no mwcceppc for {ver}"
    cmd = [str(mwcc), "-c", "-o", str(out_obj)] + flags + [str(src)]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    if r.returncode != 0:
        return False, (r.stdout + r.stderr)[-2000:]
    return True, ""


DTK = ROOT / "tools" / ("dtk.exe" if os.name == "nt" else "dtk")


def emit_asm(out_obj, fn):
    """Disasm YOUR just-built scratch object with dtk and return fn's asm.
    Same clean format as tools/decomp_work/show_target_asm.py, so you can eyeball
    your codegen vs the target line by line."""
    s_out = out_obj.with_suffix(".dis.txt")
    subprocess.run([str(DTK), "elf", "disasm", str(out_obj), str(s_out)],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, cwd=str(ROOT))
    if not s_out.exists():
        return "(disasm failed)"
    out, grab = [], False
    for ln in s_out.read_text(errors="replace").splitlines():
        if re.match(rf"^\.fn\s+{re.escape(fn)}\b", ln):
            grab = True
        if grab:
            out.append(ln)
            if re.match(rf"^\.endfn\s+{re.escape(fn)}\b", ln):
                break
    return "\n".join(out) if out else "(fn not found in scratch object)"


def measure(out_obj, fn):
    r = subprocess.run(
        [str(OBJDIFF), "diff", "-1", str(TARGET), "-2", str(out_obj), "-o", "-",
         "--format", "json", "-c", "ppc.calculatePoolRelocations=false", fn],
        capture_output=True, text=True)
    try:
        d = json.loads(r.stdout)
    except Exception:
        return None
    for sym in d.get("right", {}).get("symbols", []):
        if sym.get("name") == fn:
            return sym.get("match_percent")
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("fns", nargs="+")
    ap.add_argument("--tag", default=None)
    ap.add_argument("--rel", default="src/game/colosseum_battle.c",
                    help="repo-relative path used to pick CW flags")
    ap.add_argument("--asm", action="store_true",
                    help="also print YOUR produced asm for the first fn (compare vs show_target_asm.py)")
    a = ap.parse_args()
    src = Path(a.src).resolve()
    tag = a.tag or src.stem
    out_dir = Path(tempfile.gettempdir()) / "cb_scratch_obj"
    out_dir.mkdir(parents=True, exist_ok=True)
    out_obj = out_dir / f"{tag}.o"
    ok, err = compile_scratch(src, out_obj, a.rel)
    if not ok:
        print(f"COMPILE_FAIL:\n{err}")
        return 2
    for fn in a.fns:
        p = measure(out_obj, fn)
        print(f"{(p if p is not None else float('nan')):6.2f}  {fn}")
    if a.asm:
        print(f"\n=== YOUR produced asm for {a.fns[0]} (dtk disasm of scratch obj) ===")
        print(emit_asm(out_obj, a.fns[0]))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
