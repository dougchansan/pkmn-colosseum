#!/usr/bin/env python3
"""diffclass.py - classify WHY a near-miss function misses, from objdiff JSON.

Pairs the target/base instruction streams and classifies the dominant
mismatch into a known category. Each category maps to one of:

  - AUTO   : a deterministic rewrite/pragma automatch should try
  - BLOCKED: a compiler-level wall no source change fixes (skip, 0 compiles)
  - HINT   : needs an LLM agent, but with a precise starting hypothesis

This is the force multiplier: it lets automatch try only the relevant
variant instead of all 13, instantly triages BLOCKED classes without any
compile, and upgrades LLM agents from "explore" to "test this hypothesis".

Usage:
    python tools/diffclass.py src/game/colosseum_script.c
    python tools/diffclass.py src/game/colosseum_script.c --band 90 99.99
    python tools/diffclass.py src/game/scene_init.c --symbol fn_80037180
    python tools/diffclass.py src/game/gs_render.c --json classes.json

Reads the *currently compiled* base .o (run compile_check first, or pass
--compile to build it here).
"""

import os
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET_O = ROOT / "build" / "GC6E01" / "obj" / "auto_01_800055E0_text.o"
OBJDIFF = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")

sys.path.insert(0, str(ROOT / "tools"))
import compile_check  # noqa: E402
from headless_subprocess import run as run_tool  # noqa: E402

REG = re.compile(r"\b[rf]\d+\b")
ANON_SDA = re.compile(r"@\d+@sda21|@\d+@(ha|l)\b")


def mnem(formatted):
    if not formatted:
        return ""
    return formatted.split()[0].rstrip(".").lower()


def norm_regs(formatted):
    """Replace register numbers so only structure remains."""
    return REG.sub("R", formatted or "")


def diff_pairs(left, right):
    """Yield (l_fmt, r_fmt, kind) for every non-matching aligned slot."""
    for li, ri in zip(left, right):
        k = li.get("diff_kind") or ri.get("diff_kind")
        if not k or k in ("DIFF_NONE",):
            continue
        lf = (li.get("instruction") or {}).get("formatted")
        rf = (ri.get("instruction") or {}).get("formatted")
        yield lf, rf, k


def classify(left, right):
    """Return (category, action_kind, detail). action_kind in
    {AUTO, BLOCKED, HINT}."""
    pairs = list(diff_pairs(left, right))
    if not pairs:
        return ("clean", "AUTO", "no diffs (already 100%?)")

    lmnems = [mnem(l) for l, r, k in pairs]
    rmnems = [mnem(r) for l, r, k in pairs]
    lall = [mnem((i.get("instruction") or {}).get("formatted"))
            for i in left]
    rall = [mnem((i.get("instruction") or {}).get("formatted"))
            for i in right]

    # 1. stmw/lmw emission gap -- CW won't emit multi-reg load/store here.
    if ("stmw" in lall or "lmw" in lall) and \
       ("stmw" not in rall and "lmw" not in rall):
        return ("stmw-emission", "BLOCKED",
                "target uses stmw/lmw; CW emits stw/lwz runs for this TU "
                "(needs TU split -- see docs/tu_split.md)")

    # 2. anonymous sda21 / unresolved relocation in an arg.
    for lf, rf, k in pairs:
        if k in ("DIFF_ARG_MISMATCH",):
            if ANON_SDA.search(lf or "") or ANON_SDA.search(rf or ""):
                return ("anonymous-sda21", "BLOCKED",
                        "named lbl_X@sda21 vs anonymous @N@sda21 -- "
                        "objdiff symbol-identity mismatch, no source fix")
            # symbol@ha/@l on target vs raw 0xNNNN immediate on ours ->
            # a missing extern/decl makes CW lose the relocation.
            lt, rt = lf or "", rf or ""
            if ("@ha" in lt or "@l" in lt) and re.search(r"0x[0-9A-Fa-f]+",
                                                          rt) \
                    and "@" not in rt:
                return ("reloc-immediate", "HINT",
                        "target has symbol@ha/@l, ours has raw immediate -- "
                        "add the missing extern/symbol declaration so CW "
                        "emits a relocation")

    # 3. signed vs unsigned compare.
    for lm, rm in zip(lmnems, rmnems):
        if {lm, rm} in ({"cmpw", "cmplw"}, {"cmpwi", "cmplwi"}):
            return ("signed-compare", "AUTO",
                    "cmpw<->cmplw: flip *(u32*)->*(s32*) or add (s32)/(u32) "
                    "cast at the compare site")

    # 4. redundant sign/zero-extend next to a store (the (s16)(s32) case).
    ext = {"extsh", "extsb", "clrlwi", "rlwinm"}
    lext = sum(1 for m in lmnems if m in ext)
    rext = sum(1 for m in rmnems if m in ext)
    if abs(lext - rext) >= 1 and ("sth" in lall + rall or
                                  "stb" in lall + rall):
        return ("redundant-extend", "AUTO",
                "extra extsh/extsb/clrlwi by a store: use direct (s16)/(s8) "
                "cast (not (s16)(s32)) or add the cast the target has")

    # 5. branch-shape peephole (bne;b vs beq family).
    if ({"bne", "beq"} & set(lmnems + rmnems)) and \
       ("b" in lmnems + rmnems):
        return ("peephole-bne-b", "AUTO",
                "branch-shape differs (bne;b vs beq) -- try #pragma peephole")

    # 6. pure register-allocation permutation.
    repl = [(l, r) for l, r, k in pairs if k == "DIFF_REPLACE"]
    ins_del = [k for l, r, k in pairs if k in ("DIFF_INSERT", "DIFF_DELETE")]
    if repl and not ins_del and all(
        mnem(l) == mnem(r) and norm_regs(l) == norm_regs(r)
        for l, r in repl
    ):
        return ("reg-alloc-permutation", "HINT",
                "same instructions, only register numbers differ -- try "
                "declaration reorder or -O2; CW allocator-driven")

    # 7. scheduling: same instruction multiset, different order.
    if sorted(m for m in lall if m) == sorted(m for m in rall if m) \
            and lall != rall:
        return ("instr-scheduling-order", "AUTO",
                "same instructions reordered -- try #pragma scheduling on/off")

    return ("unclassified", "HINT",
            f"{len(pairs)} diffs, no known pattern -- needs LLM analysis")


def fetch(src_path, compile_first):
    if compile_first:
        compile_check.compile_source(src_path)
    base_o = compile_check.source_to_base_obj(Path(src_path).resolve())
    if not base_o.exists():
        sys.exit(f"base .o missing: {base_o} -- run with --compile")
    r = run_tool(
        [str(OBJDIFF), "diff", "-1", str(TARGET_O), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True, cwd=str(ROOT),
    )
    if r.returncode != 0:
        sys.exit(f"objdiff failed: {r.stderr[:300]}")
    return json.loads(r.stdout)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--band", nargs=2, type=float, default=[1.0, 99.99],
                    metavar=("LO", "HI"))
    ap.add_argument("--symbol")
    ap.add_argument("--compile", action="store_true",
                    help="compile the source before classifying")
    ap.add_argument("--json", help="write {symbol: {category, action,...}}")
    args = ap.parse_args()

    src = Path(args.source)
    if not src.is_absolute():
        src = ROOT / src
    j = fetch(src, args.compile)

    left = {s["name"]: s for s in j["left"]["symbols"]
            if s.get("kind") == "SYMBOL_FUNCTION"}
    right = {s["name"]: s for s in j["right"]["symbols"]
             if s.get("kind") == "SYMBOL_FUNCTION"}

    lo, hi = args.band
    out = {}
    rows = []
    for name, rs in right.items():
        if not name.startswith("fn_"):
            continue
        pct = rs.get("match_percent", 0.0)
        if args.symbol:
            if name != args.symbol:
                continue
        elif not (lo <= pct < hi):
            continue
        ls = left.get(name, {})
        cat, act, detail = classify(ls.get("instructions", []),
                                    rs.get("instructions", []))
        out[name] = {"match": round(pct, 2), "category": cat,
                     "action": act, "detail": detail}
        rows.append((pct, name, cat, act, detail))

    rows.sort(key=lambda r: (-["AUTO", "HINT", "BLOCKED"].index(r[3]), -r[0]))
    by_act = {}
    for pct, name, cat, act, detail in rows:
        by_act.setdefault(act, []).append((pct, name, cat, detail))

    for act in ("AUTO", "HINT", "BLOCKED"):
        items = by_act.get(act, [])
        if not items:
            continue
        print(f"\n=== {act} ({len(items)}) ===")
        for pct, name, cat, detail in items:
            print(f"  {pct:6.2f}%  {name}  [{cat}]")
            print(f"           {detail}")

    n = len(rows)
    a = len(by_act.get("AUTO", []))
    b = len(by_act.get("BLOCKED", []))
    h = len(by_act.get("HINT", []))
    print(f"\n[diffclass] {n} near-misses: {a} AUTO-fixable, {h} need LLM "
          f"hint, {b} compiler-blocked (skip -- saves {b} agent dispatches)")

    if args.json:
        Path(args.json).write_text(json.dumps(out, indent=2),
                                   encoding="utf-8")
        print(f"[diffclass] -> {args.json}")


if __name__ == "__main__":
    main()
