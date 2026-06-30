#!/usr/bin/env python3
"""gen_bucket_queue.py — bucket-by-bucket queue. Picks the HIGHEST-priority bucket
that still has unattempted fns and writes build/wall_queue.txt from ONLY that
bucket, so the fleet completes one bucket before moving to the next. Auto-advances
when a bucket is fully attempted. Run before each auto_rebatch pass.

Priority (value-descending): NEARWALL -> STRUCT -> ASM -> LOW
"""
import json, os, re, sys
from collections import defaultdict
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
LED = os.path.join(ROOT, "build", "wall_ledger.json")
WINS_DIR = os.path.join(ROOT, "build", "band_wins")
_FN_RE = re.compile(r"fn_[0-9A-Fa-f]{8}")


def load_saved():
    """Every fn already SAVED (>=100%) lands in build/band_wins/<tag>.json. Excluding
    these stops the queue from re-handing already-matched work to agents (which burns
    tokens re-verifying done fns). Structure-agnostic: scan the JSON text for fn ids."""
    saved = set()
    if os.path.isdir(WINS_DIR):
        for f in os.listdir(WINS_DIR):
            if f.endswith(".json"):
                try:
                    saved.update(_FN_RE.findall(open(os.path.join(WINS_DIR, f), encoding="utf-8").read()))
                except OSError:
                    pass
    return saved


SAVED = load_saved()


def load_reground():
    """auto_rebatch appends every fn in a dispatched packet to build/wall_attempts.txt on
    EACH dispatch (via `wall_ledger.py mark`). A fn appearing >=2 times has therefore been
    RE-dispatched — we already attempted it (almost always to a wall) and are burning tokens
    grinding it again. The wall_ledger.json `attempted` flag the queue reads is only
    refreshed by a full `wall_ledger.py build` (which doesn't run each cycle), so the queue
    re-offers these walls every session (e.g. gs_model.c / SI.c fns dispatched 6x). Exclude
    anything dispatched >=2 times. A count of 1 stays in the pool — that's a fn's legitimate
    single attempt (or one merely bundled into a packet but never worked), so this does NOT
    starve the queue the way excluding every-ever-dispatched fn would. To force a re-attempt
    after a NEW lever appears, delete the fn's lines from build/wall_attempts.txt."""
    from collections import Counter
    p = os.path.join(ROOT, "build", "wall_attempts.txt")
    c = Counter()
    if os.path.exists(p):
        try:
            c.update(_FN_RE.findall(open(p, encoding="utf-8", errors="replace").read()))
        except OSError:
            pass
    return {fn for fn, n in c.items() if n >= int(os.environ.get("REGROUND_MIN", "4"))}


REGROUND = load_reground()


def load_codex_files():
    """Files the Codex ASM lanes own (build/asm_codex_queue.txt). Exclude them from the
    Claude queue so an Opus lane never lands on a file a Codex lane is converting (they
    share the per-tag band scratch -> would clobber each other)."""
    p = os.path.join(ROOT, "build", "asm_codex_queue.txt")
    files = set()
    if os.path.exists(p):
        for ln in open(p, encoding="utf-8"):
            ln = ln.strip()
            if ln:
                files.add(ln.split()[0])
    return files


CODEX_FILES = load_codex_files()
QUEUE = os.path.join(ROOT, "build", "wall_queue.txt")
ASSIGNED = os.path.join(ROOT, "build", "wall_assigned.txt")
BAD = ("effect_util", "hsd_", "ui_core", "fsys_file", "gs_material", "pokemon", "gs_pokemon_summary",
       # 2026-06-22: mega Ghidra-import TUs whose remaining fns are hard LOW drafts (need
       # from-scratch reshape, NOT near-miss CRACK levers). They flooded the queue top and
       # lanes punted "no action / unchanged" on them in a tight loop. Excluded from the
       # auto-queue; work them via explicit from-scratch packets, not the CRACK rebatcher.
       "colosseum_battle", "gba_misc")
# For the ASM (from-scratch) queue the crack-tuned BAD list is too broad — it drops EVERY
# remaining ASM target. Exclude only files that genuinely can't be band-decompiled: the
# W-SDA-WRAPPER TUs (hsd_*) and the 3 WIP files auto_gate never commits. Everything else
# (ui_core, effect_util, gs_material, pokemon, ...) is valid from-scratch ASM work.
ASM_BAD = ("hsd_", "fsys_file", "gs_pokemon_summary")
# For the RESHAPE queue (LOW <70% real-C DRAFTS that need structural rework, not near-miss
# CRACK levers) exclude only the genuinely un-decompilable: effect_util (band-unmeasurable —
# its .inc overflows the harness) and hsd_ (W-SDA-WRAPPER). The mega-Ghidra LOW drafts
# (colosseum_battle, gba_misc, gs_material, ...) ARE the target here — the crack rebatcher
# punts on them, so they get from-scratch m2c-draft reshape packets instead.
RESHAPE_BAD = ("effect_util", "hsd_")
# Goal order for the CRACK queue (wall_queue.txt). ASM is intentionally NOT here:
# asm-wrappers have no real C to "crack" — they need from-scratch decomp (scratch mode), so
# they go to build/asm_queue.txt via write_asm_queue(), not the crack queue. wall_queue holds
# only real-C near-miss CRACK targets (LOW/STRUCT/NEARWALL).
PRIORITY = ["LOW", "STRUCT", "NEARWALL"]
# per-bucket minimum match% (skip the truly-hopeless within a bucket). LOW kept at 0
# so the file pool is deep enough to feed every lane (band locks per-file = one lane
# per file); the bucket PRIORITY still works the higher-quality fns first.
MINPCT = {"NEARWALL": 0.0, "STRUCT": 0.0, "ASM": 0.0, "LOW": 0.0}


def fresh_by_file(led, bucket):
    bf = defaultdict(list)
    for fn, v in led.items():
        if v["attempted"] or v["bucket"] != bucket:
            continue
        if fn in SAVED or fn in REGROUND:   # already matched, or already re-dispatched (>=2x) — don't re-grind
            continue
        if any(b in v["file"] for b in BAD):
            continue
        if v["pct"] < MINPCT.get(bucket, 0.0):
            continue
        src = "src/" + v["file"] + ".c"
        if src in CODEX_FILES:        # owned by a Codex ASM lane -> don't double-assign
            continue
        if os.path.exists(os.path.join(ROOT, src)):
            bf[src].append((v["pct"], fn))
    return bf


def write_asm_queue(led):
    """Emit build/asm_queue.txt — the from-scratch (asm-wrapper) targets auto_rebatch
    dispatches in 'scratch' mode. Fresh ASM-bucket fns only (not attempted / SAVED / reground
    / BAD). Nothing else regenerates this file, so without this it stayed days-stale and the
    scratch fallback re-ground a dead list. CODEX_FILES are NOT excluded here: the fleet is
    all-Opus now (the Codex ASM lanes were retired), so those files are valid Opus work."""
    bf = defaultdict(list)
    for fn, v in led.items():
        if v["attempted"] or v["bucket"] != "ASM":
            continue
        if fn in SAVED or fn in REGROUND:
            continue
        if any(b in v["file"] for b in ASM_BAD):
            continue
        src = "src/" + v["file"] + ".c"
        if os.path.exists(os.path.join(ROOT, src)):
            bf[src].append((v.get("size", 0), fn))
    lines = []
    for src, fns in sorted(bf.items(), key=lambda kv: -len(kv[1])):
        names = [fn for _, fn in sorted(fns, reverse=True)][:6]
        lines.append(src + " " + " ".join(names))
    open(os.path.join(ROOT, "build", "asm_queue.txt"), "w").write("\n".join(lines) + "\n")
    return len(lines)


def write_reshape_queue(led):
    """Emit build/reshape_queue.txt — LOW (<70%) real-C DRAFTS that need STRUCTURAL reshape
    (m2c-draft -> faithful real C: correct control-flow / types / signature), NOT near-miss
    CRACK levers. These are the mega-Ghidra-import drafts (colosseum_battle, gba_misc,
    gs_material, ...) the crack rebatcher punts on (it tries reg-coloring/decl-order levers on
    fns that need a whole reshape). Fresh LOW only (not attempted / SAVED / reground /
    RESHAPE_BAD-unmeasurable), grouped by file, MOST-fns-first so a TU gets closed out."""
    bf = defaultdict(list)
    for fn, v in led.items():
        if v["attempted"] or v["bucket"] != "LOW":
            continue
        if fn in SAVED or fn in REGROUND:
            continue
        if any(b in v["file"] for b in RESHAPE_BAD):
            continue
        src = "src/" + v["file"] + ".c"
        if os.path.exists(os.path.join(ROOT, src)):
            bf[src].append((v.get("size", 0), fn))
    lines = []
    for src, fns in sorted(bf.items(), key=lambda kv: -len(kv[1])):
        names = [fn for _, fn in sorted(fns, reverse=True)][:6]
        lines.append(src + " " + " ".join(names))
    open(os.path.join(ROOT, "build", "reshape_queue.txt"), "w").write("\n".join(lines) + "\n")
    return len(lines)


# Need enough DISTINCT files to feed every lane (band locks per-file = one lane per
# file). With ~14 lanes a single concentrated bucket (e.g. STRUCT in 2 files) would
# starve most lanes, so we fill the queue with the current bucket's files FIRST and
# overflow into the next bucket(s) — in priority order — only as far as needed.
MIN_FILES = int(os.environ.get("BUCKET_MIN_FILES", "16"))

# Max function size (bytes; 4 bytes/instr) for the Sonnet/Haiku queue. These weaker/cheaper
# models choke on the big hard-grind targets, so they get SMALL functions only — small
# near-misses, small LOW/STRUCT drafts, small ASM wrappers — capability-matched, smallest-first.
SONNET_MAX_BYTES = int(os.environ.get("SONNET_MAX_BYTES", "320"))


def _isize(x):
    try:
        return int(x)
    except (TypeError, ValueError):
        return 0


def write_sonnet_queue(led):
    """Emit build/sonnet_queue.txt — SMALL functions (<= SONNET_MAX_BYTES) for the Sonnet/Haiku
    lanes. Excludes SAVED / re-ground (>=REGROUND_MIN) / BAD; any work bucket. Smallest-first so
    the cheap models knock out trivial stubs and 1-instruction near-misses instead of grinding
    the marathon targets the Opus lanes handle."""
    bf = defaultdict(list)
    for fn, v in led.items():
        # real-C buckets only (LOW/STRUCT/NEARWALL) — ASM wrappers need from-scratch, which
        # stays on the Opus/GLM scratch lanes; these get the CRACK prompt.
        if v["bucket"] not in ("LOW", "STRUCT", "NEARWALL"):
            continue
        if fn in SAVED or fn in REGROUND:
            continue
        if any(b in v["file"] for b in BAD):
            continue
        sz = _isize(v.get("size", 0))
        if sz == 0 or sz > SONNET_MAX_BYTES:
            continue
        src = "src/" + v["file"] + ".c"
        if os.path.exists(os.path.join(ROOT, src)):
            bf[src].append((sz, fn))
    lines = []
    for src, fns in sorted(bf.items(), key=lambda kv: -len(kv[1])):
        names = [fn for _, fn in sorted(fns)][:6]   # smallest fns first
        lines.append(src + " " + " ".join(names))
    open(os.path.join(ROOT, "build", "sonnet_queue.txt"), "w").write("\n".join(lines) + "\n")
    return len(lines)


def main():
    led = json.load(open(LED))
    asm_n = write_asm_queue(led)   # keep the from-scratch (scratch-mode) queue fresh each cycle
    son_n = write_sonnet_queue(led)   # small-fn queue for the Sonnet/Haiku lanes
    rs_n = write_reshape_queue(led)   # LOW real-C drafts needing structural reshape (reshape mode)
    active = None
    lines = []
    seen = set()
    # Pinned targets (build/pin_queue.txt) are forced to the FRONT of the crack queue
    # regardless of bucket / attempted / reground state, so an operator can task the fleet
    # on specific closeouts. Picked first as lanes go idle (never interrupting an in-flight
    # task). Format per line: "src/file.c fn_X [fn_Y ...]"; '#' / blank lines ignored.
    n_pin = 0
    pin_path = os.path.join(ROOT, "build", "pin_queue.txt")
    if os.path.exists(pin_path):
        for ln in open(pin_path, encoding="utf-8", errors="replace"):
            ln = ln.strip()
            if not ln or ln.startswith("#"):
                continue
            parts = ln.split()
            if len(parts) < 2 or parts[0] in seen:
                continue
            seen.add(parts[0])
            lines.append(ln)
            n_pin += 1
    # TU-focus (build/.low_focus): order the LOW bucket FEWEST-fresh-fns-first so the
    # fleet closes out near-finished TUs one at a time (the queue auto-advances as a TU's
    # fns get SAVED and drop out of fresh_by_file). Other buckets keep highest-quality-first.
    low_focus = os.path.exists(os.path.join(ROOT, "build", ".low_focus"))
    total_fresh = 0
    for bucket in PRIORITY:
        bf = fresh_by_file(led, bucket)
        nfn = sum(len(v) for v in bf.values())
        if nfn == 0:
            continue
        if active is None:
            active = bucket
        total_fresh += nfn
        if low_focus and bucket == "LOW":
            order = sorted(bf.items(), key=lambda kv: (len(kv[1]), -max(f[0] for f in kv[1])))
        else:
            order = sorted(bf.items(), key=lambda kv: (-max(f[0] for f in kv[1]), -len(kv[1])))
        for src, fns in order:
            if src in seen:
                continue
            seen.add(src)
            names = [fn for _, fn in sorted(fns, reverse=True)][:6]
            lines.append(src + " " + " ".join(names))
        if len(lines) >= MIN_FILES:
            break   # enough files to feed the lanes; current bucket is prioritized at the top
    if not active and not lines:
        open(QUEUE, "w").write("")
        print(f"ALL-BUCKETS-COMPLETE (crack queue empty; asm_queue files={asm_n}; sonnet_queue files={son_n})")
        return
    open(QUEUE, "w").write("\n".join(lines) + "\n")
    if active:
        marker = os.path.join(ROOT, "build", ".active_bucket")
        prev = open(marker).read().strip() if os.path.exists(marker) else ""
        if prev != active:
            open(ASSIGNED, "w").write("")
            open(marker, "w").write(active)
    print(f"ACTIVE-BUCKET={active or 'PINS-ONLY'} files={len(lines)} pins={n_pin}{' low-focus' if low_focus else ''} | asm_queue files={asm_n} | sonnet_queue files={son_n} | reshape_queue files={rs_n}")


if __name__ == "__main__":
    main()
