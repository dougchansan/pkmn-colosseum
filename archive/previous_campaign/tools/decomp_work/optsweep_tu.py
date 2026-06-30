#!/usr/bin/env python3
"""optsweep_tu.py <src/file.c> [--apply]
Per-function #pragma optimization_level sweep for ONE TU.

For every function (matched via its `/* 0xADDR | */` marker + the `asm <ret> NAME(`
wrapper name) whose current byte-match < 100%, try opt levels 0..4 on ITS pragma
line and measure that fn via band json. Reports any level that beats the current
match (100% = byte-exact win). With --apply, writes each fn's match-maximizing
level back to canon (pragma-only; verifies the TU byte-exact count does not drop).

Self-contained: band-inits its own per-TU tag to resolve the correct compiler+flags,
then drives measurement by writing the private scratch and calling `band json`.
"""
import re, sys, subprocess, json, os, argparse

ROOT = r"C:\Users\douglaswhittingham\pkmn-colosseum"

def band(args):
    return subprocess.run([sys.executable, "tools/decomp_work/band.py", *args],
                          cwd=ROOT, capture_output=True, text=True)

def band_json(tag):
    r = band(["json", tag])
    try:
        return json.loads(r.stdout)
    except Exception:
        return None

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file")
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()
    rel = a.file.replace("\\", "/")
    tag = "sw_" + re.sub(r"[^a-z0-9]", "", os.path.basename(rel).lower()[:-2])
    band(["init", tag, rel])
    scratch = os.path.join(ROOT, "tools", "decomp_work", "scratch", f"band_{tag}.c")
    canon   = os.path.join(ROOT, *rel.split("/"))
    if not os.path.exists(scratch):
        print(f"SKIP {rel}: band init produced no scratch"); return

    lines = open(scratch, encoding="utf-8", newline="").read().splitlines(keepends=True)
    addr_re = re.compile(r"/\*\s*0x([0-9A-Fa-f]{8})\s*\|")
    opt_re  = re.compile(r"^#pragma optimization_level\s+(\d+)")
    asm_re  = re.compile(r"^\s*asm\s+[\w\s\*]+?\b(\w+)\s*\(")
    # a C definition opener: optional static/inline, a type, NAME, (args, not ending in ';'
    def_re  = re.compile(r"^\s*(?!extern\b|return\b|if\b|for\b|while\b|switch\b)"
                         r"(?:static\s+|inline\s+|asm\s+)*[A-Za-z_]\w*[\s\*]+([A-Za-z_]\w*)\s*\(")
    n = len(lines)
    sym_pragma = {}   # symbol -> (pragma_line_idx, current_level)
    for i in range(n):
        m_addr = addr_re.search(lines[i])
        if not m_addr:
            continue
        addr = m_addr.group(1).upper()
        pidx = lvl = sym = None
        for j in range(i+1, min(i+16, n)):
            mo = opt_re.match(lines[j])
            if mo and pidx is None:
                pidx, lvl = j, int(mo.group(1))
            if sym is None:
                ma = asm_re.match(lines[j])     # asm wrapper name (most reliable)
                if ma:
                    sym = ma.group(1)
                else:
                    md = def_re.match(lines[j])  # else first real C definition opener
                    if md and ";" not in lines[j]:
                        sym = md.group(1)
            if pidx is not None and sym:
                break
        if pidx is None:
            continue
        # default to fn_<addr> if no name parsed
        if not sym:
            sym = f"fn_{addr}"
        sym_pragma[sym] = (pidx, lvl)

    def measure():
        open(scratch, "w", encoding="utf-8", newline="").write("".join(lines))
        return band_json(tag) or {}

    base = measure()
    if not base:
        print(f"SKIP {rel}: band json empty (unmeasurable TU)"); return
    base_count = sum(1 for v in base.values() if v == 100.0)
    targets = [s for s, (p, l) in sym_pragma.items()
               if isinstance(base.get(s), (int, float)) and base[s] < 100.0]

    # Batched sweep: pragmas are per-fn independent, so set EVERY target to level X
    # at once and one compile yields each target's standalone match at X. 5 compiles total.
    at = {s: {} for s in targets}   # sym -> {level: pct}
    for lvl in (0, 1, 2, 3, 4):
        for s in targets:
            p, _ = sym_pragma[s]
            lines[p] = re.sub(r"optimization_level\s+\d+", f"optimization_level {lvl}", lines[p])
        d = measure()
        for s in targets:
            v = d.get(s)
            if isinstance(v, (int, float)):
                at[s][lvl] = v
    # restore current levels
    for s in targets:
        p, cur = sym_pragma[s]
        lines[p] = re.sub(r"optimization_level\s+\d+", f"optimization_level {cur}", lines[p])

    wins, improves = [], []
    for sym in targets:
        cur = sym_pragma[sym][1]; cur_pct = base[sym]
        best_lvl, best_pct = cur, cur_pct
        for lvl, pct in at[sym].items():
            if pct > best_pct + 1e-6:
                best_pct, best_lvl = pct, lvl
        if best_lvl != cur:
            (wins if best_pct >= 100.0 else improves).append((sym, cur, best_lvl, best_pct))

    print(f"FILE {rel}  fns_with_pragma={len(sym_pragma)} targets={len(targets)} "
          f"WINS={len(wins)} improves={len(improves)} base_byteexact={base_count}")
    for s, c, b, p in wins:     print(f"  WIN {s}: L{c}->L{b} (100%)")
    for s, c, b, p in improves: print(f"  imp {s}: L{c}->L{b} ({p:.1f}%)")

    if a.apply and (wins or improves):
        for s, c, b, p in wins + improves:
            pidx, _ = sym_pragma[s]
            lines[pidx] = re.sub(r"optimization_level\s+\d+", f"optimization_level {b}", lines[pidx])
        after = measure(); after_count = sum(1 for v in after.values() if v == 100.0)
        if after_count >= base_count:
            open(canon, "w", encoding="utf-8", newline="").write("".join(lines))
            print(f"APPLIED {rel}: byte-exact {base_count}->{after_count} (+{after_count-base_count})")
        else:
            print(f"NOAPPLY {rel}: count would drop {base_count}->{after_count} — skipped")

if __name__ == "__main__":
    main()
