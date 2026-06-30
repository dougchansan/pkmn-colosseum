#!/usr/bin/env python3
"""refill_queue.py — the daily-gains loop. Pull the closest WINNABLE near-misses
from the current objdiff report and write them as the permuter swarm queue.

A "winnable" near-miss is a function whose ACTIVE source branch is real C (NOT an
asm-wrapper — those score 99.x% from reloc/pool artifacts but have no C to
permute) and whose fuzzy match is in [min,max). The closest ones (99.9%+) are
typically 1-3 instructions off — exactly where the annealer has a gradient and
the LLM agents can hand-fix. (Contrast the reg-alloc / structural WALLS, which
no source change reaches — see triage_gate.py / coloring_oracle.py.)

usage:
  python3 refill_queue.py [--min 95] [--max 99.999] [--n 14] [--min-bytes 160]
  python3 refill_queue.py --list        # just print, don't write the queue
Writes .omc/permuter_queue.json (grind2.py reads it). Run with WSL python3."""
import json, os, re, sys
from pathlib import Path

REPO = Path("/mnt/c/Users/douglaswhittingham/pkmn-colosseum")
REPORT = REPO / "report.json"
QUEUE = REPO / ".omc" / "permuter_queue.json"
SRC = REPO / "src"
EQUIV = REPO / "tools" / "decomp_work" / "equivalent.txt"


def equivalent_fns():
    """Functions registered Equivalent (faithful real C, confirmed C-uncontrollable
    wall — jumptable/sdata2/reloc-name artifacts that score frame-0 in the permuter
    but never byte-match the project objdiff). The annealer must NOT 'win' and stop on
    these; exclude them from the queue so it keeps grinding genuinely-crackable fns."""
    out = set()
    try:
        for line in EQUIV.read_text(encoding="utf-8", errors="replace").splitlines():
            nm = line.split("#", 1)[0].strip()
            if nm.startswith("fn_"):
                out.add(nm)
    except OSError:
        pass
    return out


def opt(flag, default, cast=float):
    if flag in sys.argv:
        return cast(sys.argv[sys.argv.index(flag) + 1])
    return default


def classify_sources():
    """fn -> source path, restricted to functions that are a REAL-C near-miss:
    the decomp pattern wraps each function as

        #if 1            #if 0
        asm ... fn(){    asm ... fn(){          <- asm wrapper
        #include .inc    #include .inc
        }                }
        #else            #else
        void fn(){TODO}  void fn(){ ...real C }  <- C body
        #endif           #endif

    The reliable signal is the `#if` guarding the `asm` wrapper:
      #if 1 -> asm ACTIVE -> it's a wrapper (no real C); SKIP.
      #if 0 -> C (#else) ACTIVE -> a real near-miss; INCLUDE — unless the C body
               is a `{ /* TODO */ }` / empty stub.
    (The previous stack+loose-regex approach matched forward declarations in
     other files and ignored stub bodies, letting asm/stub fns through —
     caught on fn_800D848C.)"""
    real, fnfile = set(), {}
    asm_re = re.compile(r"^\s*asm\s+\w[\w \*]*\s+(fn_[0-9A-Fa-f]{8})\s*\(")
    stub_re = re.compile(r"\{\s*(/\*\s*TODO.*?\*/\s*)?\}\s*$", re.I)
    for c in SRC.rglob("*.c"):
        try:
            lines = c.read_text(errors="replace").splitlines()
        except OSError:
            continue
        last_if = None   # numeric literal of the most recent #if, else None
        for i, l in enumerate(lines):
            s = l.strip()
            m = re.match(r"#if\s+(\d+)\s*$", s)
            if m:
                last_if = int(m.group(1)); continue
            if s.startswith("#if"):
                last_if = None; continue       # complex condition -> unknown
            am = asm_re.match(l)
            if am and last_if == 0:
                fn = am.group(1)
                # the C body lives after the matching #else; find it and reject stubs
                body = ""
                for j in range(i, min(i + 40, len(lines))):
                    if lines[j].strip().startswith("#else"):
                        body = "\n".join(lines[j + 1:j + 4]); break
                if stub_re.search(body) or "/* TODO" in body or "/*stub" in body.lower():
                    continue
                real.add(fn)
                fnfile.setdefault(fn, str(c.relative_to(REPO)).replace("\\", "/"))
    return real, set(), fnfile


def main():
    lo = opt("--min", 95.0)
    hi = opt("--max", 99.999)
    n = opt("--n", 14, int)
    min_bytes = opt("--min-bytes", 160, int)
    list_only = "--list" in sys.argv

    rep = json.load(open(REPORT))
    fns = {}
    for u in rep["units"]:
        for f in u.get("functions") or []:
            fm = f.get("fuzzy_match_percent")
            if fm is not None and lo <= fm < hi:
                fns[f["name"]] = {"fuzzy": fm, "size": int(f.get("size", 0))}

    real, asmw, fnfile = classify_sources()
    equiv = equivalent_fns()
    cand = [(name, d, fnfile[name]) for name, d in fns.items()
            if name in real and name not in asmw and name in fnfile
            and name not in equiv and d["size"] >= min_bytes]
    cand.sort(key=lambda x: -x[1]["fuzzy"])

    n_eq = sum(1 for name in fns if name in equiv)
    if n_eq:
        print(f"(excluded {n_eq} Equivalent-registered fn(s) — reloc-name artifacts, not annealable)")
    print(f"{len(cand)} winnable real-C near-misses in [{lo},{hi})%, >={min_bytes}B — closest first:")
    for name, d, f in cand[:n]:
        print(f"  {name}  {d['fuzzy']:6.2f}%  {d['size']:>5}B  {f}")
    if list_only:
        return
    queue = [[name, f] for name, d, f in cand[:n]]
    os.makedirs(QUEUE.parent, exist_ok=True)
    json.dump(queue, open(QUEUE, "w"), indent=1)
    print(f"\nwrote {QUEUE} with {len(queue)} targets — restart grind2.py to grind them")


if __name__ == "__main__":
    main()
