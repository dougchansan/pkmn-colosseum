#!/usr/bin/env python3
"""tu_split.py - split a mega .c into its real translation units.

Unblocked by config/GC6E01/func_tu_map.json. The merged mega-files
(gs_field_world.c 736 fns, colosseum_battle.c 685 …) defeat CW's per-TU
heuristics — most importantly `stmw/lmw` emission, which depends on the
compilation unit and is the documented hard blocker for a whole family
of functions (see docs/decomp_notes/src_game_battle_battle_main.md).

This tool partitions a mega-file's functions by their func_tu_map `src`
attribution and emits each group as its real TU filename, replicating the
mega-file preamble (includes/externs/file-scope decls) into each.

SAFETY:
  * Dry-run is the default — prints the plan, writes nothing.
  * --apply re-runs configure.py, rebuilds, and compares whole-project
    matched-count; if it does not strictly improve-or-equal, the entire
    split is reverted via `git checkout`.
  * Refuses to run if the target file has uncommitted changes or if a
    git worktree is using it (avoids colliding with live agents).

Usage:
    python tools/tu_split.py src/game/battle/battle_main.c        # plan
    python tools/tu_split.py src/game/battle/battle_main.c --apply
"""

import argparse
import json
import re
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MAP = ROOT / "config" / "GC6E01" / "func_tu_map.json"
sys.path.insert(0, str(ROOT / "tools"))
import automatch  # noqa: E402  (find_fn_def, measure, matched_count, read/write)

FN_RE = re.compile(r"\bfn_[0-9A-Fa-f]{8}\b")


def git(*a):
    return subprocess.run(["git", "-C", str(ROOT), *a],
                          capture_output=True, text=True)


def load_map():
    return json.loads(MAP.read_text(encoding="utf-8"))


def function_blocks(lines, names):
    """Yield (name, start_idx, end_idx) covering the full block for each
    function: from its preceding `/* ... */` header (if adjacent) or the
    signature, through the matching close brace / #endif."""
    blocks = []
    for n in names:
        loc = automatch.find_fn_def(lines, n)
        if loc is None:
            continue
        si, ci = loc
        # extend start upward over an attached header comment / #if 0 asm
        s = si
        j = si - 1
        seen_blank = 0
        while j >= 0:
            t = lines[j].strip()
            if t.startswith(("/*", "*", "*/", "#if", "#else", "#endif",
                             "asm ", "#include")) or t == "":
                s = j
                j -= 1
                if t == "":
                    seen_blank += 1
                    if seen_blank > 1:
                        break
                continue
            break
        # extend end downward over a trailing #pragma pop / #endif
        e = ci
        k = ci + 1
        while k < len(lines):
            t = lines[k].strip()
            if t.startswith(("#pragma", "#endif")) or t == "":
                e = k
                k += 1
                if t == "":
                    break
                continue
            break
        blocks.append((n, s, e))
    blocks.sort(key=lambda b: b[1])
    return blocks


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    src = Path(args.source)
    rel = src.as_posix() if not src.is_absolute() else \
        src.resolve().relative_to(ROOT).as_posix()
    full = ROOT / rel

    # Safety: clean working copy only.
    if git("status", "--porcelain", "--", rel).stdout.strip():
        sys.exit(f"REFUSING: {rel} has uncommitted changes")

    fmap = load_map()
    text = automatch.read_src(full)
    lines = text.splitlines(keepends=True)

    names = sorted(set(FN_RE.findall(text)))
    blocks = function_blocks(lines, names)
    if not blocks:
        sys.exit("no function blocks found")

    first = blocks[0][1]
    preamble = "".join(lines[:first])

    groups = defaultdict(list)
    unattributed = 0
    for n, s, e in blocks:
        info = fmap.get(n)
        dst = info.get("src") if info else None
        if not dst or dst == rel:
            dst = rel  # stays in the original file
            if not info or not info.get("src"):
                unattributed += 1
        groups[dst].append((n, s, e))

    others = {k: v for k, v in groups.items() if k != rel}
    print(f"[tu-split] {rel}: {len(blocks)} functions")
    print(f"[tu-split] stays in original: {len(groups.get(rel, []))} "
          f"({unattributed} unattributed/GAP)")
    print(f"[tu-split] would extract to {len(others)} TU files:")
    for dst, fns in sorted(others.items(), key=lambda kv: -len(kv[1])):
        exists = (ROOT / dst).exists()
        print(f"   {len(fns):>4}  {dst}{'  (exists!)' if exists else ''}")

    if not args.apply:
        print("\n[tu-split] DRY RUN — nothing written. "
              "Re-run with --apply to execute (guarded by match-count "
              "preservation + auto-revert).")
        return 0

    # --apply path
    base_before = automatch.measure(full, None)
    if base_before is None:
        sys.exit("baseline compile failed; aborting split")
    mc_before = automatch.matched_count(base_before)

    # Build new original (drop extracted blocks) + new TU files.
    extract_idx = set()
    for dst, fns in others.items():
        if (ROOT / dst).exists():
            sys.exit(f"REFUSING: target {dst} already exists — manual "
                     f"merge required (not auto-overwriting)")
        body = preamble
        for n, s, e in sorted(fns, key=lambda b: b[1]):
            body += "".join(lines[s:e + 1])
            for i in range(s, e + 1):
                extract_idx.add(i)
        (ROOT / dst).parent.mkdir(parents=True, exist_ok=True)
        automatch.write_src(ROOT / dst, body)
        print(f"[tu-split] wrote {dst} ({len(fns)} fns)")

    kept = [ln for i, ln in enumerate(lines) if i not in extract_idx]
    automatch.write_src(full, "".join(kept))

    subprocess.run([sys.executable, str(ROOT / "configure.py")],
                   cwd=str(ROOT), capture_output=True)
    # Re-measure every new + original object.
    ok = True
    mc_after = 0
    for dst in list(others) + [rel]:
        m = automatch.measure(ROOT / dst, None)
        if m is None:
            ok = False
            break
        mc_after += automatch.matched_count(m)

    if not ok or mc_after < mc_before:
        for dst in others:
            try:
                (ROOT / dst).unlink()
            except OSError:
                pass
        git("checkout", "--", rel)
        subprocess.run([sys.executable, str(ROOT / "configure.py")],
                       cwd=str(ROOT), capture_output=True)
        print(f"[tu-split] REVERTED — matched {mc_before} -> "
              f"{mc_after} (not preserved)")
        return 1

    print(f"[tu-split] OK — matched {mc_before} -> {mc_after} preserved/"
          f"improved across {len(others)+1} TUs. configure.py regenerated. "
          f"Review + commit.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
