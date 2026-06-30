#!/usr/bin/env python3
"""pcport_harvest.py — self-iterating flip-harvest driver for campaign-tail TUs.

Per TU: pcport_gen (auto-unify on) -> clang compile -> classify errors:
  * arity-class errors ("arguments to function call") in a FLIPPED body -> FLIP_AS_STUB
  * arity-class errors in an ACTIVE body -> STUB_BODY
  * everything else -> reported as residue for manual fix-entries
Loops until 0 errors or no progress. Table additions persist in
tools/pcport_stub_tables.json, which pcport_gen.py overlays onto its in-file tables.

Usage: python tools/pcport_harvest.py src/game/pokemon.c [more TUs...]
"""
import json, re, subprocess, sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CLANG = r"C:\Program Files\LLVM\bin\clang.exe"
TBL = ROOT / "tools" / "pcport_stub_tables.json"
INC = ["-Iinclude", "-Iinclude/dolphin", "-Iinclude/game", "-Iinclude/hsd",
       "-Ipcport", "-Ithird_party", "-Ithird_party/glad/include",
       "-Ibuild_pc/_deps/glfw-src/include"]
CFLAGS = ["-m32", "-c", "-DPCPORT=1", "-w", "-O1", "-ferror-limit=3000",
          "-fms-compatibility", "-fms-extensions",
          "-Wno-error=int-conversion", "-Wno-error=incompatible-pointer-types",
          "-Wno-error=implicit-function-declaration", "-Wno-error=return-mismatch",
          "-include", "pcport/pcport_compat.h"] + INC

DEF_RE = re.compile(r'^(?:static\s+)?(?:const\s+)?[A-Za-z_][\w\s]*?[\s\*]+\**'
                    r'([A-Za-z_]\w*)\s*\(([^;{}()]*)\)\s*\{?\s*$')
KW = {"return", "if", "while", "for", "switch", "goto", "else", "do", "case",
      "sizeof", "typedef"}
ASM_FN_RE = re.compile(r'^\s*asm\s+[\w\s\*]*?\b(fn_[0-9A-Fa-f]+)\s*\(')
IF1_RE = re.compile(r'^\s*#\s*if\s+1\b')


def load_tbl():
    if TBL.exists():
        t = json.loads(TBL.read_text())
        t.setdefault("kr_def", {})
        return t
    return {"auto_unify": [], "flip_as_stub": {}, "stub_body": {}, "kr_def": {}}


def save_tbl(t):
    TBL.write_text(json.dumps(
        {"auto_unify": sorted(set(t["auto_unify"])),
         "flip_as_stub": {k: sorted(set(v)) for k, v in t["flip_as_stub"].items()},
         "stub_body": {k: sorted(set(v)) for k, v in t["stub_body"].items()},
         "kr_def": {k: sorted(set(v)) for k, v in t.get("kr_def", {}).items()}},
        indent=1))


def flipped_fns(src_path):
    """fn names of ACTIVE (#if 1) asm wrappers in the ORIGINAL source."""
    out, lines = set(), src_path.read_text(errors="replace").splitlines()
    active = False
    for ln in lines:
        if IF1_RE.match(ln):
            active = True
        elif ln.lstrip().startswith(("#else", "#endif")):
            active = False
        elif active:
            m = ASM_FN_RE.match(ln)
            if m:
                out.add(m.group(1))
    return out


def file_defs(gen_lines):
    defs = []
    for i, ln in enumerate(gen_lines):
        if not ln or ln[0].isspace() or ln.rstrip().endswith(';'):
            continue
        m = DEF_RE.match(ln)
        if m and m.group(1) not in KW:
            brace = ln.rstrip().endswith('{')
            if not brace:
                j = i + 1
                while j < len(gen_lines) and not gen_lines[j].strip():
                    j += 1
                brace = j < len(gen_lines) and gen_lines[j].lstrip().startswith('{')
            if brace:
                defs.append((i + 1, m.group(1)))
    return defs


def enclosing(defs, line_no):
    name = None
    for s, n in defs:
        if s <= line_no:
            name = n
        else:
            break
    return name


def run(rel_src):
    src = ROOT / rel_src
    rel = Path(rel_src).as_posix().removeprefix("src/")
    gen_path = ROOT / "build_pc" / "gen" / rel
    flips = flipped_fns(src)

    tbl = load_tbl()
    if rel not in tbl["auto_unify"]:
        tbl["auto_unify"].append(rel)
        save_tbl(tbl)

    prev_errs = None
    for rnd in range(1, 9):
        subprocess.run([sys.executable, "tools/pcport_gen.py", rel_src],
                       cwd=ROOT, capture_output=True)
        r = subprocess.run([CLANG, *CFLAGS, str(gen_path), "-o",
                            str(ROOT / "build_pc" / "harvest_t.o")],
                           cwd=ROOT, capture_output=True, text=True)
        all_lines = r.stderr.splitlines()
        errs = [ln for ln in all_lines if " error: " in ln]
        # pair each error with the callee named in its following note line
        callee_of = {}
        for li, ln in enumerate(all_lines):
            if " error: " not in ln:
                continue
            for lj in range(li + 1, min(li + 8, len(all_lines))):
                nm = re.search(r"note: '([A-Za-z_]\w*)' declared here", all_lines[lj])
                if nm:
                    callee_of[ln] = nm.group(1)
                    break
                if " error: " in all_lines[lj]:
                    break
        if not errs:
            print(f"  {rel}: CLEAN after round {rnd - 1}")
            return True
        prev_errs = len(errs)
        gen_lines = gen_path.read_text(errors="replace").splitlines()
        defs = file_defs(gen_lines)
        new_flip, new_body, new_kr, other = set(), set(), set(), defaultdict(list)
        def_names = {n for _, n in defs}
        for ln in errs:
            m = re.search(r':(\d+):\d+: error: (.*)', ln)
            if not m:
                continue
            fn = enclosing(defs, int(m.group(1)))
            if fn is None:
                continue
            msg = m.group(2)
            if "too many arguments to function call" in msg and \
                    callee_of.get(ln) in def_names:
                # callers pass MORE args than the in-TU definition declares:
                # K&R-ify the CALLEE definition (keeps its real body)
                new_kr.add(callee_of[ln])
            elif "arguments to function call" in msg:
                (new_flip if fn in flips else new_body).add(fn)
            else:
                other[fn].append(msg[:70])
        # only count entries not already in the tables (else we'd loop forever)
        new_flip -= set(tbl["flip_as_stub"].get(rel, []))
        new_body -= set(tbl["stub_body"].get(rel, []))
        new_kr -= set(tbl["kr_def"].get(rel, []))
        if not new_flip and not new_body and not new_kr:
            print(f"  {rel}: {len(errs)} errors, no new arity-class entries; residue:")
            for fn, msgs in list(other.items())[:10]:
                print(f"    {fn}: {msgs[0]} x{len(msgs)}")
            for ln in errs[:6]:
                print("    " + ln.split("error: ")[-1][:90])
            return False
        tbl["flip_as_stub"].setdefault(rel, []).extend(sorted(new_flip))
        tbl["stub_body"].setdefault(rel, []).extend(sorted(new_body))
        tbl["kr_def"].setdefault(rel, []).extend(sorted(new_kr))
        save_tbl(tbl)
        print(f"  {rel}: round {rnd}: {len(errs)} errors -> "
              f"+{len(new_flip)} flip-stub, +{len(new_body)} body-stub, "
              f"+{len(new_kr)} kr-def")
    print(f"  {rel}: did not converge in 8 rounds")
    return False


if __name__ == "__main__":
    ok = []
    bad = []
    for f in sys.argv[1:]:
        print(f"== {f}")
        (ok if run(f) else bad).append(f)
    print(f"\nCLEAN: {len(ok)}  RESIDUE: {len(bad)}")
    for f in bad:
        print("  needs manual entries:", f)
