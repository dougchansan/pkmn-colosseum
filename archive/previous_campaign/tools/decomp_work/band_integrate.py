#!/usr/bin/env python3
"""Deterministic integration + RE-VERIFICATION of band_wins back into canon.

Generalization of cs_integrate.py. Every band wins file
(build/band_wins/<tag>.json) records the source file it belongs to (the `_src`
key, written by band.py save). This merges all wins per source file, splices
them into a fresh copy of that canonical source, recompiles the integrated
file via a private band, and RE-MEASURES every spliced function via objdiff.

This is the GROUND-TRUTH parent gate: it trusts no agent claim. Only functions
that still measure >=100% in the freshly-compiled integrated file are kept; any
that dropped below 100% after integration are reported and DROPPED (the canonical
source is left untouched for them).

Usage:
  band.py band_integrate.py            integrate every tag in build/band_wins/
  band_integrate.py <tag> [<tag> ...]  integrate only these tags
  band_integrate.py --apply            also overwrite the canonical src in place
                                       (default: write build/band_<stem>_integrated.c
                                        only; never edits src/ without --apply)

Outputs (per source file):
  build/band_<stem>_patch.json         merged {fn: body} that re-verified 100%
  build/band_<stem>_integrated.c       canonical with verified wins spliced in
Exit 0 if every saved win re-verified (or was cleanly dropped); 1 on splice error.
"""
import json
import os
import re
import signal
import subprocess
import sys
import datetime
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
WINS = ROOT / "build" / "band_wins"
NEARMISS = ROOT / "build" / "band_nearmiss"   # real-C near-misses banked by band.py bank
BUILD = ROOT / "build"
EQUIV_FILE = HERE / "equivalent.txt"
WALLS_FILE = ROOT / "WALLS.md"
PY = sys.executable
M = 99.9999
META_KEYS = {"_src", "_srcs", "_pct"}
INTEGRATE_TIMEOUT_SECS = float(os.environ.get("BAND_INTEGRATE_TIMEOUT_SECS", "300"))


STATUS_LOG = HERE / "coordination" / "status.md"
MATCHED_SEEN = ROOT / "build" / "matched_fns.txt"


def _run_capture(cmd, timeout=INTEGRATE_TIMEOUT_SECS):
    """subprocess.run(..., capture_output=True) with process-group timeout."""
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            text=True, start_new_session=True)
    try:
        out, err = proc.communicate(timeout=timeout)
        return subprocess.CompletedProcess(cmd, proc.returncode, out, err)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(proc.pid, signal.SIGTERM)
        except OSError:
            pass
        try:
            out, err = proc.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            try:
                os.killpg(proc.pid, signal.SIGKILL)
            except OSError:
                pass
            out, err = proc.communicate()
        err = (err or "") + (
            f"\nTIMEOUT after {timeout:g}s running: "
            + " ".join(str(x) for x in cmd)
            + "\n"
        )
        return subprocess.CompletedProcess(cmd, 124, out or "", err)


def _log_matches(fns, src_rel):
    """Append a per-fn 'MATCH!' line to coordination/status.md so the dashboard
    activity log lists the ACTUAL functions matched (not just a 'batch harvest'
    commit message). Deduped via build/matched_fns.txt so re-gating the same wins
    doesn't spam duplicate entries."""
    if not fns:
        return
    seen = set()
    try:
        seen = set(MATCHED_SEEN.read_text(encoding="utf-8").split())
    except OSError:
        pass
    new = [fn for fn in fns if fn not in seen]
    if not new:
        return
    ts = datetime.datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
    try:
        with STATUS_LOG.open("a", encoding="utf-8") as f:
            for fn in new:
                f.write(f"- **{ts}** `gate` MATCH! {fn} 100.00% in {src_rel}\n")
        MATCHED_SEEN.parent.mkdir(parents=True, exist_ok=True)
        with MATCHED_SEEN.open("a", encoding="utf-8") as f:
            for fn in new:
                f.write(fn + "\n")
    except OSError:
        pass


def _register_equivalent(equiv_fns, src_rel):
    """Append salvaged functional-C functions to equivalent.txt (the C-converted
    axis registry read by progress2.py) and a stub line to WALLS.md. The asm DOL
    build is unaffected (it builds from the dtk split, not the C source), so this
    only moves the C-CONVERTED axis, never the byte-exact DOL. Idempotent."""
    if not equiv_fns:
        return
    existing = set()
    if EQUIV_FILE.exists():
        existing = {l.split("#")[0].strip()
                    for l in EQUIV_FILE.read_text(encoding="utf-8").splitlines()
                    if l.split("#")[0].strip()}
    stem = Path(src_rel).stem
    eq_lines, wall_lines = [], []
    for fn, pct in sorted(equiv_fns):
        if fn in existing:
            continue
        eq_lines.append(f"{fn}   # {stem} — {pct:.2f}% functional real C, salvaged "
                        f"(asm active for byte-match; counts on C-converted axis)\n")
        wall_lines.append(f"- `{fn}` ({stem}, {pct:.2f}%): SALVAGE — faithful real C "
                          f"active, did not reach byte-exact; registered Equivalent.\n")
    if eq_lines:
        with EQUIV_FILE.open("a", encoding="utf-8") as f:
            f.writelines(eq_lines)
        with WALLS_FILE.open("a", encoding="utf-8") as f:
            f.write("\n")
            f.writelines(wall_lines)
        print(f"  REGISTERED {len(eq_lines)} fn(s) as Equivalent (equivalent.txt + WALLS.md)")


def _scratch_json(tag, integrated_c, config_from):
    """init a private band from the integrated file, compile + measure it,
    return {fn: pct}. Resolves flags/version/target from the REAL canon source
    (config_from) — the integrated temp file's stem isn't in compile_config.json,
    so without this it would fall back to default -O4,p and falsely drop wins on
    -O4,s TUs (trainer.c, pokemon.c, ...)."""
    inttag = f"_int_{tag}"
    init = _run_capture([PY, str(HERE / "band.py"), "init", inttag, str(integrated_c),
                         "--config-from", str(config_from)])
    if init.returncode == 124:
        return None, init.stdout + init.stderr
    r = _run_capture([PY, str(HERE / "band.py"), "json", inttag])
    line = [l for l in r.stdout.splitlines() if l.strip().startswith("{")]
    if not line:
        return None, (r.stdout + r.stderr)
    return json.loads(line[-1]), None


# Signatures of the decl-order splice failure: a win body spliced into canon can
# land ABOVE canon's file-scope extern / SDA decl block (band_integrate splices
# function BODIES only, not the scratch's surrounding decls), so the first use of
# an extern fn or lbl_* global precedes its declaration -> implicit-int, then a
# redeclaration conflict, or a bare undefined-identifier.
_DECLORDER_RE = re.compile(
    r"implicit|undefined identifier|redeclared|was declared as|declared as:\s*'int",
    re.IGNORECASE)
# A single-line, column-0 (file-scope) extern declaration: `extern <...> name;`
# or `extern <...> name(args);`. Anchored at start so INDENTED in-body externs
# are left untouched; excludes `{` so it never grabs a definition.
_EXTERN_DECL_RE = re.compile(r"^extern\b[^;{]*;\s*$")


def _hoist_externs(path):
    """Move every file-scope `extern ...;` declaration to a block right after the
    file's #include section. externs emit no code (byte-neutral) and only need to
    precede their first use, so hoisting them all to the top resolves the
    decl-order class without touching codegen. Deduped, first-seen order.
    Returns True iff it rewrote the file. Safe: only ever called on an
    already-failed compile, so it can only recover or stay neutral."""
    try:
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    except OSError:
        return False
    externs, rest = [], []
    for ln in lines:
        (externs if _EXTERN_DECL_RE.match(ln) else rest).append(ln)
    if not externs:
        return False
    seen, uniq = set(), []
    for e in externs:
        key = " ".join(e.split())
        if key not in seen:
            seen.add(key)
            uniq.append(e.rstrip())
    ins = 0
    for i, ln in enumerate(rest[:300]):
        if ln.lstrip().startswith("#include"):
            ins = i + 1
    block = ["", "/* hoisted file-scope externs: decl-before-use for spliced wins */"]
    block += uniq + [""]
    out = rest[:ins] + block + rest[ins:]
    try:
        path.write_text("\n".join(out) + "\n", encoding="utf-8")
    except OSError:
        return False
    return True


def _compilable_subset(canon, fn_bodies, stem):
    """Fallback for a full-batch compile abort: splice each fn on its own and keep
    only those whose lone splice compiles. One pathological win (a redundant
    already-decompiled fn, a dropped in-body extern that triggers the implicit-decl
    asm-block cascade, a malformed body) can then never sink the whole batch — it's
    quarantined and the rest still gate. Returns (good_bodies, quarantined) where
    quarantined is a list of (fn, reason)."""
    good, quarantined = {}, []
    ppatch = BUILD / f"band_{stem}_iso_patch.json"
    pout = BUILD / f"band_{stem}_iso.c"
    for fn, body in fn_bodies.items():
        ppatch.write_bytes(json.dumps({fn: body}, indent=1).encode("utf-8"))
        r = subprocess.run([PY, str(HERE / "cs_splice.py"), str(canon),
                            str(ppatch), str(pout)], capture_output=True, text=True)
        if r.returncode != 0:
            quarantined.append((fn, "splice"))
            continue
        m, _ = _scratch_json(f"{stem}_iso", pout, canon)
        if m is None:
            quarantined.append((fn, "compile"))
            continue
        good[fn] = body
    return good, quarantined


def integrate_source(src_rel, fn_bodies, apply, min_pct=M, equivalent=False,
                     bank=False, committed=None):
    """Splice fn_bodies into a fresh copy of src_rel, recompile, re-verify each
    fn. Byte-exact mode (default): keep only fns that re-measure >=100%.
    Equivalent mode (min_pct<100): also keep faithful real-C fns that re-measure
    >=min_pct (they get registered in equivalent.txt). Bank mode (bank=True,
    committed=<fn->pct>): keep faithful real-C near-misses in [min_pct,100) that
    STRICTLY BEAT the committed src pct (no-regress — protects byte-matching asm
    wrappers, whose committed pct is higher than any <100 real-C) and apply them
    WITHOUT registering Equivalent, so refill_queue still queues them for the
    permuter. Returns (held, dropped)."""
    canon = ROOT / src_rel
    if not canon.exists():
        print(f"  SKIP: canonical source missing: {src_rel}")
        return [], []
    stem = Path(src_rel).stem
    patch = BUILD / f"band_{stem}_patch.json"
    integrated = BUILD / f"band_{stem}_integrated.c"

    patch.write_bytes(json.dumps(fn_bodies, indent=1).encode("utf-8"))
    r = subprocess.run([PY, str(HERE / "cs_splice.py"), str(canon), str(patch),
                        str(integrated)], capture_output=True, text=True)
    print("  " + (r.stdout.strip() or r.stderr.strip()))
    if r.returncode != 0:
        print("  SPLICE FAILED:\n" + r.stderr)
        return None, None

    newm, err = _scratch_json(stem, integrated, canon)
    if newm is None and err and _DECLORDER_RE.search(err):
        # Auto-recover from the decl-order splice failure (see _hoist_externs):
        # a spliced win body landed above canon's file-scope extern/SDA block.
        if _hoist_externs(integrated):
            print("  decl-order abort detected -> hoisted file-scope externs, retrying compile ...")
            newm, err = _scratch_json(stem, integrated, canon)
            if newm is not None:
                print("  recovered: hoisted externs resolved decl-before-use")
    if newm is None and len(fn_bodies) > 1:
        # Isolation fallback (harden the splice plug): a single pathological fn can
        # abort the whole integrated compile. Quarantine the offenders, gate the rest.
        good, quarantined = _compilable_subset(canon, fn_bodies, stem)
        if good and quarantined:
            for fn, why in sorted(quarantined):
                print(f"    QUARANTINE {fn}  (lone splice {why} abort — kept out of batch)")
            patch.write_bytes(json.dumps(good, indent=1).encode("utf-8"))
            subprocess.run([PY, str(HERE / "cs_splice.py"), str(canon), str(patch),
                            str(integrated)], capture_output=True, text=True)
            newm, err = _scratch_json(stem, integrated, canon)
            if newm is not None:
                print(f"  recovered: quarantined {len(quarantined)}, gating {len(good)} compilable fn(s)")
                fn_bodies = dict(good)
    if newm is None:
        print("  MEASURE FAILED:\n" + (err or ""))
        return None, None

    # FRAUD GUARD: a byte-match win must be DECOMPILED C, not inline assembly.
    # Agents have gamed the metric by hand-transcribing the .inc disassembly into
    # `asm void fn(){ stwu...; lwz...; }` blocks (or keeping the `#include .inc`
    # wrapper) to force a 100% match without producing any C. Reject any saved body
    # that is an `asm`-storage-class function or contains an inline asm{} / __asm
    # block — real C never does. (The legitimate un-decompiled baseline is also an
    # `asm` wrapper, and must never be re-saved as a "win" either.)
    ASM_FN = re.compile(r"\basm\b\s+[\w*]+\s+" + r"\w+\s*\(")   # `asm <type> fn(`
    ASM_BLOCK = re.compile(r"\basm\b\s*\{|__asm\b|#include\s+\"[^\"]*\.inc\"")
    held, equiv_held, bank_held, dropped, rejected = [], [], [], [], []
    committed = committed or {}
    for fn in fn_bodies:
        body = fn_bodies[fn] or ""
        if ASM_FN.search(body) or ASM_BLOCK.search(body):
            rejected.append((fn, newm.get(fn, 0.0)))
            continue
        pct = newm.get(fn, 0.0)
        if pct >= M:
            held.append((fn, pct))
        elif bank:
            cm = committed.get(fn, 0.0)
            if min_pct <= pct < M and pct > cm + 1e-6:
                bank_held.append((fn, pct))   # real-C improvement -> bank + keep permuter-eligible
            else:
                dropped.append((fn, pct))     # below floor OR would regress committed src
        elif equivalent and pct >= min_pct:
            equiv_held.append((fn, pct))      # faithful real C, salvaged
        else:
            dropped.append((fn, pct))

    if rejected:
        print(f"  !! REJECTED {len(rejected)} inline-asm/wrapper 'wins' (NOT real C — fraud):")
        for fn, pct in sorted(rejected):
            print(f"    REJECT-ASM  {fn}  {pct:.2f}%  (inline assembly, not decompilation)")
    print(f"  re-verified: {len(held)} byte-exact, {len(equiv_held)} equivalent, "
          f"{len(bank_held)} banked near-miss, {len(dropped)} dropped")
    for fn, pct in sorted(held):
        print(f"    HELD   {fn}  {pct:.2f}%  (byte-exact)")
    for fn, pct in sorted(equiv_held):
        print(f"    EQUIV  {fn}  {pct:.2f}%  (functional real C -> Equivalent)")
    for fn, pct in sorted(bank_held):
        print(f"    BANK   {fn}  {pct:.2f}%  (real-C improvement -> canon + permuter)")
    for fn, pct in sorted(dropped):
        print(f"    DROP   {fn}  {pct:.2f}%  (below floor or would regress committed)")

    keep_set = dict(held + equiv_held + bank_held)
    if dropped or rejected:
        # Re-splice with only the kept fns so the integrated file is clean
        # (excludes sub-threshold drops and rejected inline-asm/wrapper fraud).
        kept = {fn: b for fn, b in fn_bodies.items() if fn in keep_set}
        if kept:
            patch.write_bytes(json.dumps(kept, indent=1).encode("utf-8"))
            subprocess.run([PY, str(HERE / "cs_splice.py"), str(canon),
                            str(patch), str(integrated)],
                           capture_output=True, text=True)
        else:
            integrated.write_bytes(canon.read_bytes())

    print(f"  integrated -> {integrated.relative_to(ROOT)}")
    if apply and (held or equiv_held or bank_held):
        canon.write_bytes(integrated.read_bytes())
        print(f"  APPLIED to {src_rel}")
        _log_matches([fn for fn, _ in held], src_rel)   # per-fn MATCH! -> activity log
        if equiv_held:
            _register_equivalent(equiv_held, src_rel)
        # bank_held are deliberately NOT registered Equivalent: they are unfinished
        # real-C improvements the permuter (refill_queue) should still pick up.
    return held + equiv_held + bank_held, dropped


def main():
    raw = sys.argv[1:]
    apply = "--apply" in raw
    equivalent = "--equivalent" in raw
    min_pct = M
    if "--min-pct" in raw:
        i = raw.index("--min-pct")
        try:
            min_pct = float(raw[i + 1])
        except (IndexError, ValueError):
            print("--min-pct needs a number, e.g. --min-pct 90")
            return 2
    if equivalent and min_pct >= M:
        min_pct = 90.0  # sensible default salvage floor
    skip = {"--apply", "--equivalent", "--min-pct", str(min_pct)}
    args = [a for a in raw if a not in skip]
    if equivalent:
        print(f"[equivalent mode] salvaging faithful real C with pct >= {min_pct:.1f}% "
              f"(byte-exact still requires 100%)")
    if not WINS.exists():
        print("no build/band_wins/ — nothing to integrate.")
        return 0

    files = sorted(WINS.glob("*.json"))
    if args:
        wanted = {a if a.endswith(".json") else f"{a}.json" for a in args}
        files = [f for f in files if f.name in wanted]
    if not files:
        print("no matching band wins files.")
        return 0

    # Group wins by source file. A tag may hold wins from MULTIPLE files, so we
    # group by PER-FUNCTION source recorded in `_srcs` (band.py save). Legacy
    # single-`_src` files fall back to that for every fn.
    by_src = {}
    for wf in files:
        try:
            d = json.loads(wf.read_text(encoding="utf-8"))
        except ValueError:
            print(f"WARN: unparseable {wf.name}; skipping")
            continue
        srcs = d.get("_srcs", {})            # per-fn source map (new format)
        default_src = d.get("_src")          # legacy single source / fallback
        for fn, body in d.items():
            if fn in META_KEYS:
                continue
            src_rel = srcs.get(fn, default_src)
            if not src_rel:
                print(f"WARN: {wf.name}:{fn} has no source (old format?); skipping")
                continue
            by_src.setdefault(src_rel, {})[fn] = body  # disjoint by design

    rc = 0
    total_held = total_dropped = 0
    for src_rel, fn_bodies in sorted(by_src.items()):
        print(f"\n=== {src_rel}  ({len(fn_bodies)} saved win(s)) ===")
        held, dropped = integrate_source(src_rel, fn_bodies, apply, min_pct, equivalent)
        if held is None:
            rc = 1
            continue
        total_held += len(held)
        total_dropped += len(dropped)

    print(f"\n=== TOTAL: {total_held} held, {total_dropped} dropped "
          f"across {len(by_src)} file(s) ===")
    if not apply:
        print("(dry run — pass --apply to overwrite canonical src files)")
    return rc


if __name__ == "__main__":
    sys.exit(main())
