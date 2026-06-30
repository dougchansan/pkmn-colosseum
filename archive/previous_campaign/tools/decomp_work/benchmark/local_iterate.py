#!/usr/bin/env python3
"""local_iterate.py — Local iterate-and-diff helper for one function.

Replaces the #else body of <fn> in src/game/<stem>.c with the candidate text,
flips #if 1 to #if 0, builds locally, and dumps a per-instruction objdiff
between the new .o and the target .o.

Usage:
    python tools/decomp_work/benchmark/local_iterate.py \
        --stem gs_title --fn fn_80020F54 \
        --candidate /tmp/cand.c \
        [--diff-out /tmp/diff.json]
"""
from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
COMPILE_CHECK = REPO / "tools" / "compile_check.py"
OBJDIFF = REPO / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")


def swap_else(stem: str, fn: str, c_body: str) -> tuple[bool, str]:
    c_path = REPO / "src" / "game" / f"{stem}.c"
    text = c_path.read_text(encoding="latin-1")
    pattern = re.compile(
        r"(#if\s+1\s*\nasm void " + re.escape(fn) + r"\([^)]*\)\s*\{[^}]*\})\s*\n"
        r"(#else\s*\n)(.*?)(#endif)",
        re.S,
    )
    m = pattern.search(text)
    if not m:
        pattern2 = re.compile(
            r"(#if\s+0\s*\nasm void " + re.escape(fn) + r"\([^)]*\)\s*\{[^}]*\})\s*\n"
            r"(#else\s*\n)(.*?)(#endif)",
            re.S,
        )
        m = pattern2.search(text)
        if not m:
            return False, text
        new_text = (
            text[: m.start()] + m.group(1) + "\n" + m.group(2) + c_body.rstrip() + "\n" + m.group(4) + text[m.end():]
        )
    else:
        flipped = m.group(1).replace("#if 1", "#if 0", 1)
        new_text = (
            text[: m.start()] + flipped + "\n" + m.group(2) + c_body.rstrip() + "\n" + m.group(4) + text[m.end():]
        )
    c_path.write_text(new_text, encoding="latin-1")
    return True, text


def restore(stem: str, original: str) -> None:
    c_path = REPO / "src" / "game" / f"{stem}.c"
    c_path.write_text(original, encoding="latin-1")


def compile_and_diff(stem: str, fn: str, diff_out: Path | None) -> dict:
    obj_built = REPO / "build" / "GC6E01" / "base" / "game" / f"{stem}.o"
    obj_built.unlink(missing_ok=True)
    cc = subprocess.run(
        ["python", str(COMPILE_CHECK), f"src/game/{stem}.c"],
        cwd=REPO, capture_output=True, text=True, timeout=180,
    )
    compile_ok = cc.returncode == 0 and "OK:" in cc.stdout
    if not compile_ok:
        return {"compile_ok": False, "compile_err": (cc.stdout + cc.stderr)[-2000:]}
    target_o = REPO / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
    if not diff_out:
        diff_out = REPO / ".omc" / "decomp_attack" / f"{fn}_diff.json"
    diff_out.parent.mkdir(parents=True, exist_ok=True)
    od = subprocess.run(
        [str(OBJDIFF), "diff",
         "-1", str(target_o),
         "-2", str(obj_built),
         "-o", str(diff_out),
         "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false",
         fn],
        cwd=REPO, capture_output=True, text=True, timeout=60,
    )
    return {"compile_ok": True, "diff_path": str(diff_out), "rc": od.returncode}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--stem", required=True)
    ap.add_argument("--fn", required=True)
    ap.add_argument("--candidate", required=True)
    ap.add_argument("--diff-out", default=None)
    args = ap.parse_args()

    cand = Path(args.candidate).read_text(encoding="utf-8")

    swapped, original = swap_else(args.stem, args.fn, cand)
    if not swapped:
        print(f"ERROR: could not find #if/#else block for {args.fn}")
        return 1
    try:
        result = compile_and_diff(args.stem, args.fn, Path(args.diff_out) if args.diff_out else None)
        if not result["compile_ok"]:
            print("COMPILE FAILED:")
            print(result["compile_err"])
            return 2
        print(f"compiled OK -> {result['diff_path']}")
        # Summarize diff: count matched/mismatched
        diff = json.loads(Path(result["diff_path"]).read_text(encoding="utf-8"))
        for fn_obj in diff.get("functions", []):
            if fn_obj.get("name") == args.fn:
                ins = fn_obj.get("instructions", [])
                tot = len(ins)
                match = sum(1 for i in ins if i.get("kind") == "match")
                pct = (match / tot * 100) if tot else 0
                print(f"function: {tot} instr, {match} match -> {pct:.1f}%")
                # Show first 30 mismatched
                shown = 0
                for i in ins:
                    if i.get("kind") != "match":
                        l = i.get("left") or {}
                        r = i.get("right") or {}
                        print(f"  {l.get('formatted','')[:60]!s:60} | {r.get('formatted','')[:60]!s}")
                        shown += 1
                        if shown >= 30:
                            print(f"  ... (showing first 30 of {tot - match} mismatches)")
                            break
                break
    finally:
        restore(args.stem, original)
        print(f"restored src/game/{args.stem}.c")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
