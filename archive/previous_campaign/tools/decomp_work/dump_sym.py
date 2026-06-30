#!/usr/bin/env python3
"""Dump aligned target-vs-mine instruction diff for a symbol using objdiff JSON.

-1 = TARGET (original, aim-for), -2 = BASE (my compiled code).
Prints rows where left=target, right=mine, marking mismatches with '*'.
"""
import os
import json, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
CLI = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")
TARGET = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
BASE = ROOT / "build" / "GC6E01" / "base" / "game" / "gs_render.o"

def get(sym):
    cmd = [str(CLI), "diff", "-1", str(TARGET), "-2", str(BASE),
           "-o", "-", "--format", "json", "-c",
           "ppc.calculatePoolRelocations=false", sym]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(ROOT))
    return json.loads(r.stdout)

def fmt_instr(ins):
    if ins is None:
        return ""
    row = ins.get("row")
    parts = []
    for a in (ins.get("arguments") or []):
        for k in ("opcode", "plain", "value", "signed", "unsigned",
                  "branch_dest", "relocation", "literal"):
            if k in a:
                parts.append(str(a[k]))
                break
        else:
            parts.append(str(a))
    seg = ins.get("segments")
    text = ""
    if seg:
        text = "".join(s.get("text", "") for s in seg)
    else:
        text = " ".join(parts)
    return text.strip()

def dump(side, sym):
    j = get(sym)
    entries = j.get(side, {}).get("symbols", [])
    for e in entries:
        if e.get("name") == sym:
            return e.get("instructions", [])
    return []

def main():
    sym = sys.argv[1]
    j = get(sym)
    left = None; right = None
    for e in j.get("left", {}).get("symbols", []):
        if e.get("name") == sym:
            left = e.get("instructions", [])
    for e in j.get("right", {}).get("symbols", []):
        if e.get("name") == sym:
            right = e.get("instructions", [])
    left = left or []
    right = right or []
    n = max(len(left), len(right))
    print(f"=== {sym}  TARGET(left)={len(left)}  MINE(right)={len(right)} ===")
    for i in range(n):
        li = left[i] if i < len(left) else None
        ri = right[i] if i < len(right) else None
        lt = render(li)
        rt = render(ri)
        ldk = (li or {}).get("diff_kind")
        rdk = (ri or {}).get("diff_kind")
        mark = " " if (ldk in (None, "DIFF_NONE") and rdk in (None, "DIFF_NONE")) else "*"
        print(f"{mark} {lt:<40} | {rt}")

def render(ins):
    if ins is None:
        return ""
    segs = ins.get("segments")
    if segs:
        return "".join(s.get("text", "") for s in segs).strip()
    # fallback: mnemonic + args
    m = ins.get("mnemonic", ins.get("opcode", ""))
    args = []
    for a in (ins.get("arguments") or []):
        if isinstance(a, dict):
            for k in ("opcode","plain","value","branch_dest","relocation"):
                if k in a:
                    args.append(str(a[k])); break
        else:
            args.append(str(a))
    return (str(m) + " " + ", ".join(args)).strip()

if __name__ == "__main__":
    main()
