#!/usr/bin/env python3
"""pcport_step0_triage.py — flip-harvest triage for the PC port priority TUs.

For each ACTIVE asm wrapper (`#if 1 asm {...inc} #else <C> #endif` or raw asm block)
in the priority TUs, classify the inactive C alternative:

  REAL  — #else branch contains a substantive C body (control flow / calls / stores)
  STUB  — #else branch exists but is empty/trivial (return 0; or {})
  NONE  — raw asm block or #if without #else: no C alternative at all

REAL ones are free wins for the PC build (pcport_gen flips them automatically);
STUB/NONE are the genuine functional-decomp backlog.
"""
import re, sys, json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

PRIORITY_TUS = [
    "src/game/gs_field_world.c",
    "src/game/gs_worldmap.c",
    "src/hsd/hsd_jobj.c",
    "src/hsd/hsd_cobj.c",
    "src/hsd/hsd_mobj.c",
    "src/hsd/hsd_dobj.c",
    "src/game/battle/battle_scene.c",
    "src/game/gs_event_exec.c",
    "src/game/gs_texture.c",
    "src/game/effect/effect_util.c",
]

ASM_RE = re.compile(r'^\s*asm\s+[\w\*\s]+?(fn_[0-9A-Fa-f]{8})\s*\(')
INC_RE = re.compile(r'#include\s+"([^"]*fn_[0-9A-Fa-f]{8}\.(?:inc|s))"')
IF_RE = re.compile(r'^\s*#\s*if\b')
IF1_RE = re.compile(r'^\s*#\s*if\s+1\b')
ELSE_RE = re.compile(r'^\s*#\s*else\b')
ENDIF_RE = re.compile(r'^\s*#\s*endif\b')


def inc_instr_count(rel):
    p = ROOT / rel
    if not p.exists():
        # asm includes are sometimes rooted differently; try asm/ prefix as-is
        return None
    n = 0
    for line in p.read_text(errors="replace").splitlines():
        s = line.strip()
        if s and not s.startswith((".", "/", "#")) and "nofralloc" not in s:
            n += 1
    return n


def body_class(lines):
    """Classify a C body: REAL if it has substantive statements."""
    code = "\n".join(lines)
    # strip comments and string literals
    code = re.sub(r'/\*.*?\*/', '', code, flags=re.S)
    code = re.sub(r'//[^\n]*', '', code)
    if re.search(r'\bTODO\b', "\n".join(lines)):
        todo = True
    else:
        todo = False
    stmts = [s.strip() for s in re.split(r'[;{}]', code) if s.strip()]
    # drop function signatures, declarations, bare returns
    substantive = []
    for s in stmts:
        if re.match(r'^(extern|static)?\s*(const\s+)?[\w\*]+[\s\*]+\w+\s*\([^)]*\)$', s):
            continue  # signature / prototype
        if re.match(r'^(extern|static)?\s*(const\s+)?(void|s32|u32|s16|u16|s8|u8|f32|f64|int|char|short|long|float|double|struct|union|enum)\b[\w\s\*\[\],]*$', s):
            continue  # plain declaration
        if re.match(r'^return(\s+(0|NULL|0\.0f?))?$', s):
            continue  # bare/trivial return
        substantive.append(s)
    if len(substantive) >= 2:
        return "REAL"
    if len(substantive) == 1 and not todo:
        return "REAL"
    return "STUB"


def scan_tu(rel):
    path = ROOT / rel
    lines = path.read_text(errors="replace").splitlines()
    results = []
    i, n = 0, len(lines)
    done_c = 0  # `#if 0 asm #else C #endif` — C already active, no work needed
    while i < n:
        if re.match(r'^\s*#\s*if\s+0\b', lines[i]):
            # skip the whole dead-asm/live-C block; if it wraps an fn_ asm, count as done
            k, depth, saw_asm = i + 1, 1, False
            while k < n:
                if IF_RE.match(lines[k]):
                    depth += 1
                elif ENDIF_RE.match(lines[k]):
                    depth -= 1
                    if depth == 0:
                        break
                if depth == 1 and ASM_RE.match(lines[k]):
                    saw_asm = True
                k += 1
            if saw_asm:
                done_c += 1
            i = k + 1
            continue
        if IF1_RE.match(lines[i]) or (IF_RE.match(lines[i]) and not re.match(r'^\s*#\s*if(def|ndef)\b', lines[i])):
            # scan the active #if branch to its depth-1 #else
            k, depth, else_idx, end_idx = i + 1, 1, None, None
            fn, inc = None, None
            while k < n:
                if IF_RE.match(lines[k]):
                    depth += 1
                elif ENDIF_RE.match(lines[k]):
                    depth -= 1
                    if depth == 0:
                        end_idx = k
                        break
                elif ELSE_RE.match(lines[k]) and depth == 1:
                    else_idx = k
                if depth == 1 and else_idx is None:
                    m = ASM_RE.match(lines[k])
                    if m:
                        fn = m.group(1)
                    im = INC_RE.search(lines[k])
                    if im:
                        inc = im.group(1)
                k += 1
            if fn is not None:
                if else_idx is not None and end_idx is not None:
                    cls = body_class(lines[else_idx + 1:end_idx])
                    c_lines = end_idx - else_idx - 1
                else:
                    cls, c_lines = "NONE", 0
                results.append(dict(fn=fn, cls=cls, c_lines=c_lines,
                                    instrs=inc_instr_count(inc) if inc else None,
                                    line=i + 1))
                i = end_idx if end_idx is not None else k
            i += 1
            continue
        # raw asm block outside any #if
        m = ASM_RE.match(lines[i])
        if m:
            inc = None
            for j in range(i, min(i + 4, n)):
                im = INC_RE.search(lines[j])
                if im:
                    inc = im.group(1)
                    break
            results.append(dict(fn=m.group(1), cls="NONE", c_lines=0,
                                instrs=inc_instr_count(inc) if inc else None,
                                line=i + 1))
        i += 1
    return results, done_c


def main():
    equiv = set()
    eq = ROOT / "tools/decomp_work/equivalent.txt"
    if eq.exists():
        for line in eq.read_text().splitlines():
            line = line.split("#")[0].strip()
            if line.startswith("fn_"):
                equiv.add(line)

    grand = {"REAL": 0, "STUB": 0, "NONE": 0}
    out = {}
    for rel in PRIORITY_TUS:
        if not (ROOT / rel).exists():
            print(f"!! missing TU: {rel}")
            continue
        rows, done_c = scan_tu(rel)
        out[rel] = {"wrappers": rows, "already_c": done_c}
        counts = {"REAL": 0, "STUB": 0, "NONE": 0}
        for r in rows:
            counts[r["cls"]] += 1
            grand[r["cls"]] += 1
        print(f"\n== {rel}  ({len(rows)} ACTIVE asm wrappers: "
              f"{counts['REAL']} flippable-C / {counts['STUB']} stub / {counts['NONE']} no-C"
              f"  |  {done_c} fns already active-C via #if0)")
        for r in sorted(rows, key=lambda r: (r["cls"] != "REAL", -(r["instrs"] or 0))):
            tag = " EQUIV" if r["fn"] in equiv else ""
            print(f"   {r['cls']:4} {r['fn']}  instrs={r['instrs'] or '?':>4}  "
                  f"c_lines={r['c_lines']:>4}  @{rel}:{r['line']}{tag}")
    print(f"\nTOTAL: {sum(grand.values())} wrappers — "
          f"{grand['REAL']} REAL-C (free wins) / {grand['STUB']} stub / {grand['NONE']} no-C")
    (ROOT / "build_pc").mkdir(exist_ok=True)
    (ROOT / "build_pc/step0_triage.json").write_text(json.dumps(out, indent=1))
    print("json: build_pc/step0_triage.json")


if __name__ == "__main__":
    main()
