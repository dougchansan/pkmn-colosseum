#!/usr/bin/env python3
"""phased_demo.py <src/file.c> <fn> — demonstrate the real-world iterative loop with
the v3 model: draft -> see the exact objdiff residual -> repair that band -> re-measure,
phase by phase. Prints the match% progression. Read-only (uses a private band scratch).

  python tools/decomp_work/rl/phased_demo.py src/game/gs_title.c fn_80023E60
"""
import hashlib
import json
import subprocess
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools"))
import lane_glm as L   # noqa: E402
BAND = ROOT / "tools" / "decomp_work" / "band.py"
SCR = ROOT / "tools" / "decomp_work" / "scratch"
V3 = "http://192.168.50.101:8780/gen"
PHASES = 4
N_INIT, N_REPAIR = 3, 2


def band(*a, timeout=180):
    return subprocess.run([sys.executable, str(BAND), *a], cwd=str(ROOT),
                          capture_output=True, text=True, timeout=timeout)


def gen(asm, n, temp=0.5, draft=None, diff=None):
    body = {"asm": asm, "n": n, "temp": temp}
    if draft and diff:
        body.update(draft=draft, diff=diff)
    req = urllib.request.Request(V3, data=json.dumps(body).encode(),
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=400) as r:
        return json.loads(r.read()).get("candidates", [])


def rename_def(c, fn):
    import re
    m = re.search(r"\b([A-Za-z_]\w*)\s*\([^;{]*\)\s*\{", c)
    return None if not m else (c if m.group(1) == fn else c.replace(m.group(1), fn))


def main():
    file, fn = sys.argv[1], sys.argv[2]
    tag = "demo_" + hashlib.md5(file.encode()).hexdigest()[:8]
    import compile_check as cc
    src = (ROOT / file).resolve()
    cc.compile_source(src)
    asm = L.target_asm(cc.find_target_obj(src), cc.source_to_base_obj(src), fn)
    scratch = SCR / f"band_{tag}.c"
    band("init", tag, file)
    orig = scratch.read_text(encoding="latin-1")
    s0, e0, _ = L.find_fn_span(orig, fn)

    def measure(c):
        scratch.write_text(orig[:s0] + c + orig[e0:], encoding="latin-1")
        try:
            return json.loads(band("json", tag).stdout).get(fn)
        except Exception:
            return None

    print(f"=== phased decomp: {fn} in {file} ===")
    # Phase 0: initial drafts
    best_pct, best_c = -1.0, None
    for c in gen(asm, N_INIT):
        rc = rename_def(c, fn)
        if not rc:
            continue
        p = measure(rc)
        if p is not None and p > best_pct:
            best_pct, best_c = p, rc
    print(f"phase 0 (draft):   {best_pct:.1f}%")
    if best_c is None:
        print("no compilable draft"); return

    # Phases 1..K: diff-repair
    for ph in range(1, PHASES + 1):
        if best_pct >= 100.0:
            break
        scratch.write_text(orig[:s0] + best_c + orig[e0:], encoding="latin-1")  # best in scratch
        diff = band("diff", tag, fn).stdout
        improved = False
        for c in gen(asm, N_REPAIR, draft=best_c, diff=diff):
            rc = rename_def(c, fn)
            if not rc:
                continue
            p = measure(rc)
            if p is not None and p > best_pct:
                best_pct, best_c, improved = p, rc, True
        print(f"phase {ph} (repair): {best_pct:.1f}%" + ("  *100%*" if best_pct >= 100 else ("" if improved else "  (no gain — wall/handoff)")))
        if not improved:
            break

    scratch.write_text(orig, encoding="latin-1")  # restore
    print(f"=== final: {best_pct:.1f}% {'WIN' if best_pct>=100 else '(near-miss -> permuter/levers)'} ===")


if __name__ == "__main__":
    main()
