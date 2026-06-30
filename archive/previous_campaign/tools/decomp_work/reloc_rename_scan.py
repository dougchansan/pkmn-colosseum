#!/usr/bin/env python3
"""reloc_rename_scan.py — find near-miss fns whose residual includes a
reloc-NAME mismatch (`bl fn_XXXXXXXX` / `lis fn_X@ha` on our side vs the real
symbols.build.txt name on the target side). Renaming the C identifier to the
build name is byte-neutral and correct-by-construction (the target objects are
built FROM symbols.build.txt). Discovered 2026-06-10 (Wave1-E: 24 fns -> 100).

usage:
  python tools/decomp_work/reloc_rename_scan.py <tu-json> [...]
where <tu-json> is a full-TU objdiff JSON (no symbol filter):
  ./tools/objdiff-cli.exe diff -1 build/.../auto_01_..._text.o -2 build/.../<stem>.o \
      -o - --format json -c ppc.calculatePoolRelocations=false > tu.json
Prints per-fn rename candidates: fn_OLDADDR -> RealName  (and the fn % it blocks).
"""
import json, re, sys, os

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SYMS = os.path.join(REPO, "config", "GC6E01", "symbols.build.txt")

def load_symmap():
    m = {}
    pat = re.compile(r"^(\S+)\s*=\s*\S+:0x([0-9A-Fa-f]{8})")
    with open(SYMS, encoding="utf-8", errors="replace") as f:
        for line in f:
            g = pat.match(line)
            if g:
                m[g.group(2).upper()] = g.group(1)
    return m

TOKEN = re.compile(r"[A-Za-z_$@][A-Za-z0-9_$@]*")

def names(s):
    return set(t for t in TOKEN.findall(s) if not t.startswith("0x"))

def scan(path, symmap):
    d = json.load(open(path))
    def side(k):
        return {s["name"]: s for s in d[k]["symbols"] if s.get("name", "").startswith("fn_") and "match_percent" in s}
    L = side("left")
    out = {}
    for nm, ls in L.items():
        m = ls["match_percent"]
        if not (80 <= m < 100):
            continue
        rs = None
        for s in d["right"]["symbols"]:
            if s.get("name") == nm:
                rs = s
                break
        if rs is None:
            continue
        renames = set()
        for a, b in zip(ls.get("instructions", []), rs.get("instructions", [])):
            ia = a.get("instruction"); ib = b.get("instruction")
            if not ia or not ib:
                continue
            ta = ia.get("formatted", ""); tb = ib.get("formatted", "")
            if ta == tb:
                continue
            na, nb = names(ta), names(tb)
            ours_fn = {t for t in nb - na if re.fullmatch(r"fn_[0-9A-Fa-f]{8}", t)}
            theirs = {t for t in na - nb if not t.startswith(("fn_", "lbl_", "@"))}
            for of in ours_fn:
                addr = of[3:].upper()
                real = symmap.get(addr)
                if real and real in theirs:
                    renames.add((of, real))
        if renames:
            out[nm] = (m, sorted(renames))
    return out

def main():
    symmap = load_symmap()
    for path in sys.argv[1:]:
        res = scan(path, symmap)
        if res:
            print(f"== {path}")
            for fn, (m, rn) in sorted(res.items(), key=lambda kv: -kv[1][0]):
                for old, new in rn:
                    print(f"  {fn} ({m:.2f}%): {old} -> {new}")

if __name__ == "__main__":
    main()
