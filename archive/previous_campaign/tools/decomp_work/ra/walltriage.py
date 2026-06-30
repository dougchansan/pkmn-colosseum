#!/usr/bin/env python3
"""walltriage.py - classify high-mismatch (>N instr) near-misses as
PURE register-allocation walls vs STRUCTURAL mismatches.

For each wall-heavy file we compile, run objdiff JSON, then for every
function with mismatches >= MIN_MM we walk the aligned target(left)/ours(right)
instruction streams and bucket each non-matching slot:

  PURE-REG  : DIFF_REPLACE, same mnemonic, identical once register numbers
              are normalized (only rN/fN numbers differ).
  REG-IMM   : DIFF_REPLACE, same mnemonic, differs only in a numeric immediate
              / sda offset (allocator-adjacent, often stack slot moves).
  STRUCT    : DIFF_INSERT / DIFF_DELETE (instruction count differs) OR
              DIFF_REPLACE where the mnemonic itself differs.
  OTHER     : anything else (arg/branch-target relocs etc).

A function is called PURE-REG-ALLOC if it has >=1 mismatch and
  STRUCT == 0  and  PURE-REG >= 1  and PURE-REG dominates (>=80% of non-OTHER).
We also record how many DISTINCT physical registers are permuted (the size of
the target<->ours register remap), which is the knob count an allocator-
inversion method would need to control.

Usage:
    python tools/decomp_work/ra/walltriage.py            # default file set
    python tools/decomp_work/ra/walltriage.py --min-mm 8 --json out.json
"""
import os
import argparse
import json
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools"))
import compile_check  # noqa: E402
from headless_subprocess import run as run_tool  # noqa: E402

TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")

REG = re.compile(r"\b([rf])(\d+)\b")
NUM = re.compile(r"-?0x[0-9A-Fa-f]+|-?\b\d+\b")

DEFAULT_FILES = [
    "src/game/colosseum_script.c",
    "src/game/colosseum_battle.c",
    "src/game/battle/battle_logic.c",
    "src/game/gs_event_exec.c",
    "src/game/gs_render.c",
    "src/game/gs_material.c",
]


def mnem(f):
    return f.split()[0].rstrip(".").lower() if f else ""


def norm_regs(f):
    return REG.sub(lambda m: m.group(1) + "R", f or "")


def norm_regs_and_num(f):
    return NUM.sub("N", norm_regs(f))


def reg_list(f):
    """Ordered list of (kind,num) register tokens in an instruction."""
    return [(m.group(1), int(m.group(2))) for m in REG.finditer(f or "")]


def fmt(slot):
    return (slot.get("instruction") or {}).get("formatted") or ""


def analyze_symbol(left, right):
    """left = target instrs, right = our instrs (aligned by objdiff)."""
    buckets = Counter()
    reg_remap = Counter()      # (target_reg -> our_reg) frequency
    distinct_pairs = set()
    examples = []
    n_mismatch = 0

    for li, ri in zip(left, right):
        k = li.get("diff_kind") or ri.get("diff_kind")
        if not k or k == "DIFF_NONE":
            continue
        n_mismatch += 1
        lf, rf = fmt(li), fmt(ri)
        lm, rm = mnem(lf), mnem(rf)

        if k in ("DIFF_INSERT", "DIFF_DELETE"):
            buckets["STRUCT"] += 1
            continue

        # DIFF_REPLACE (or arg/op mismatch on an aligned pair)
        if lm != rm:
            buckets["STRUCT"] += 1
            continue

        # same mnemonic from here
        if norm_regs(lf) == norm_regs(rf):
            # identical once register numbers normalized -> pure reg permutation
            buckets["PURE-REG"] += 1
            lr, rr = reg_list(lf), reg_list(rf)
            for (lk, ln), (rk, rn) in zip(lr, rr):
                if lk == rk and ln != rn:
                    reg_remap[(f"{lk}{ln}", f"{rk}{rn}")] += 1
                    distinct_pairs.add((f"{lk}{ln}", f"{rk}{rn}"))
            if len(examples) < 3:
                examples.append((lf, rf))
        elif norm_regs_and_num(lf) == norm_regs_and_num(rf):
            # same after also wiping immediates -> stack-slot / offset move
            buckets["REG-IMM"] += 1
            if len(examples) < 3:
                examples.append((lf, rf))
        else:
            buckets["OTHER"] += 1

    return {
        "mismatches": n_mismatch,
        "buckets": dict(buckets),
        "distinct_reg_pairs": len(distinct_pairs),
        "reg_remap": dict(Counter({f"{a}->{b}": c
                                   for (a, b), c in reg_remap.items()})),
        "examples": examples,
    }


def classify(info):
    """Tiered classification.

    PURE-REG-ALLOC : STRUCT==0, REG-IMM==0  -> ONLY register numbers differ.
                     The textbook addressable target.
    REG-ALLOC+IMM  : STRUCT==0 -> register numbers and/or stack-slot/immediate
                     offsets differ (allocator + stack-frame layout), no
                     inserted/deleted/different instructions.
    REG-DOMINANT   : a few STRUCT (<=15% of non-other) but the residual is
                     dominated by reg permutation; allocator-inversion would
                     remove the bulk, leaving a tiny structural tail.
    STRUCTURAL     : STRUCT >= 40% of non-other -> a different problem.
    MIXED          : in between.
    """
    b = info["buckets"]
    pure = b.get("PURE-REG", 0)
    regimm = b.get("REG-IMM", 0)
    struct = b.get("STRUCT", 0)
    non_other = pure + regimm + struct
    if non_other == 0:
        return "OTHER-ONLY"
    reg_core = pure + regimm
    if struct == 0 and regimm == 0 and pure >= 1:
        return "PURE-REG-ALLOC"
    if struct == 0 and reg_core >= 1:
        return "REG-ALLOC+IMM"
    if reg_core >= 1 and struct <= 0.15 * non_other:
        return "REG-DOMINANT"
    if struct >= 0.4 * non_other:
        return "STRUCTURAL"
    return "MIXED"


REG_CLASSES = ("PURE-REG-ALLOC", "REG-ALLOC+IMM", "REG-DOMINANT")


def fetch(src):
    src = Path(src)
    if not src.is_absolute():
        src = ROOT / src
    try:
        compile_check.compile_source(src)
    except SystemExit:
        return None, f"compile failed: {src}"
    base_o = compile_check.source_to_base_obj(src.resolve())
    if not base_o.exists():
        return None, f"base .o missing: {base_o}"
    r = run_tool(
        [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True, cwd=str(ROOT), timeout=300,
    )
    if r.returncode != 0:
        return None, f"objdiff failed: {r.stderr[:200]}"
    try:
        return json.loads(r.stdout), None
    except json.JSONDecodeError:
        return None, "objdiff json parse failed"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--files", nargs="*", default=DEFAULT_FILES)
    ap.add_argument("--min-mm", type=int, default=8,
                    help="minimum instruction mismatches to consider")
    ap.add_argument("--json", default=None)
    args = ap.parse_args()

    all_rows = []
    totals = Counter()
    for src in args.files:
        print(f"\n########## {src} ##########", flush=True)
        j, err = fetch(src)
        if err:
            print(f"  SKIP: {err}", flush=True)
            continue
        left = {s["name"]: s for s in j["left"]["symbols"]
                if s.get("kind") == "SYMBOL_FUNCTION"}
        right = {s["name"]: s for s in j["right"]["symbols"]
                 if s.get("kind") == "SYMBOL_FUNCTION"}
        file_rows = []
        for name, rs in right.items():
            if not name.startswith("fn_"):
                continue
            pct = rs.get("match_percent", 0.0)
            if pct >= 100.0 or pct <= 0.0:
                continue
            ls = left.get(name)
            if not ls:
                continue
            info = analyze_symbol(ls.get("instructions", []),
                                  rs.get("instructions", []))
            if info["mismatches"] < args.min_mm:
                continue
            cls = classify(info)
            row = {"file": src, "fn": name, "match": round(pct, 2),
                   "mismatches": info["mismatches"], "class": cls,
                   "buckets": info["buckets"],
                   "distinct_reg_pairs": info["distinct_reg_pairs"],
                   "reg_remap": info["reg_remap"],
                   "examples": info["examples"]}
            file_rows.append(row)
            all_rows.append(row)
            totals[cls] += 1
        file_rows.sort(key=lambda r: (r["class"] not in REG_CLASSES,
                                      -r["match"]))
        for r in file_rows:
            print(f"  {r['match']:6.2f}%  mm={r['mismatches']:<4} "
                  f"{r['class']:<15} {r['fn']}  "
                  f"buckets={r['buckets']} regpairs={r['distinct_reg_pairs']}",
                  flush=True)
        print(f"  [{src}] {len(file_rows)} fns >= {args.min_mm} mm", flush=True)

    n = len(all_rows)
    print("\n\n==================== SUMMARY ====================")
    print(f"Total functions with >= {args.min_mm} instr mismatches: {n}")
    for cls in ("PURE-REG-ALLOC", "REG-ALLOC+IMM", "REG-DOMINANT",
                "MIXED", "STRUCTURAL", "OTHER-ONLY"):
        c = totals.get(cls, 0)
        if n:
            print(f"  {cls:<16} {c:>4}  ({100.0*c/n:.1f}%)")

    reg_addr = [r for r in all_rows if r["class"] in REG_CLASSES]
    print(f"\nREG-ALLOC-ADDRESSABLE (pure/+imm/dominant): {len(reg_addr)} "
          f"({100.0*len(reg_addr)/n:.1f}% of >= {args.min_mm}mm)" if n else "")

    # cleanest examples: reg-addressable, fewest distinct reg pairs (smallest
    # permutation = easiest to invert), then highest match% (closest).
    clean = sorted(reg_addr,
                   key=lambda r: (r["distinct_reg_pairs"], -r["match"]))
    print("\n--- Cleanest reg-alloc examples (fewest distinct reg pairs) ---")
    for r in clean[:20]:
        print(f"  {r['fn']}  [{Path(r['file']).name}]  {r['class']}  "
              f"match={r['match']}%  mm={r['mismatches']}  "
              f"distinct_reg_pairs={r['distinct_reg_pairs']}  "
              f"buckets={r['buckets']}")
        print(f"      remap={r['reg_remap']}")
        for lf, rf in r["examples"][:2]:
            print(f"      target: {lf}")
            print(f"      ours  : {rf}")

    if args.json:
        Path(args.json).write_text(json.dumps(all_rows, indent=2),
                                   encoding="utf-8")
        print(f"\n[walltriage] wrote {args.json}")


if __name__ == "__main__":
    main()
