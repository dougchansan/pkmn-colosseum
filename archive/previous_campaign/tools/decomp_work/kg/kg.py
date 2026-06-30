#!/usr/bin/env python3
"""
kg.py — the decomp Knowledge Graph (single-file SQLite).

The "sudoku" thesis: these games were written by ~10-20 people, so wins propagate.
A lever that cracks one function very likely cracks its siblings; a struct layout
discovered in one TU constrains every function that touches it. This DB collapses
the project's scattered knowledge (scratches.json, equivalent.txt, WALLS.md, the git
log of 1700+ commits, CW_QUIRKS.md) into ONE queryable graph so parallel workers can
ask "what's the highest-value next move, and what lever applies?" instead of guessing.

Stdlib only. Usage:
    python kg.py init                 # create / migrate the schema
    python kg.py backfill             # (re)ingest all sources; idempotent
    python kg.py q <name> [arg]       # run a named "sudoku" query (see q list)
    python kg.py q list               # list named queries
    python kg.py sql "<SELECT ...>"   # raw read-only query
    python kg.py stats                # row counts per table
    python kg.py record-crack <addr> <lever_slug> [--commit SHA --delta TXT]
    python kg.py record-lever <slug> --title T [--desc D --opt-gated --source S]

DB lives at tools/decomp_work/kg/kg.db (next to coordination/locks.db convention).
"""
import argparse
import json
import os
import re
import sqlite3
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
DB_PATH = os.path.join(HERE, "kg.db")

SCRATCHES = os.path.join(REPO, "build_pc", "decompme", "scratches.json")
EQUIVALENT = os.path.join(REPO, "tools", "decomp_work", "equivalent.txt")
WALLS_MD = os.path.join(REPO, "WALLS.md")

FN_RE = re.compile(r"\bfn_[0-9A-Fa-f]{8}\b")
TU_RE = re.compile(r"\b([\w/]+\.c)\b")
PCT_RE = re.compile(r"(\d{1,3}(?:\.\d+)?)\s*%")
WALLCLASS_RE = re.compile(r"\bW[-\w]+")  # W1, W2, W-operand-order, W-peephole-...

SCHEMA = """
CREATE TABLE IF NOT EXISTS functions (
    addr        TEXT PRIMARY KEY,      -- fn_XXXXXXXX or named symbol
    name        TEXT,                  -- friendly name if known
    tu          TEXT,                  -- owning .c file
    byte_pct    REAL,                  -- best known byte-match %
    compiler    TEXT,                  -- e.g. GC/1.3, -O4,s
    status      TEXT,                  -- DONE / WIP / NEARMISS / WALL / UNKNOWN
    is_equivalent INTEGER DEFAULT 0,   -- 1 = real correct C, blocked by a wall
    wall_class  TEXT,                  -- W1..W6 / W-* if blocked
    scratch_url TEXT,
    notes       TEXT,
    updated_at  REAL
);
CREATE INDEX IF NOT EXISTS idx_fn_tu   ON functions(tu);
CREATE INDEX IF NOT EXISTS idx_fn_wall ON functions(wall_class);
CREATE INDEX IF NOT EXISTS idx_fn_pct  ON functions(byte_pct);

CREATE TABLE IF NOT EXISTS levers (
    slug        TEXT PRIMARY KEY,      -- kebab-case id
    title       TEXT,
    description TEXT,
    opt_gated   INTEGER DEFAULT 0,     -- 1 = only bites at certain opt levels
    source      TEXT                   -- CW_QUIRKS.md / memory / git / agent
);

CREATE TABLE IF NOT EXISTS walls (
    class           TEXT PRIMARY KEY,  -- W1, W2, W-operand-order, ...
    title           TEXT,
    c_controllable  INTEGER DEFAULT 0, -- 0 = not reachable from C
    description     TEXT
);

-- Edge: a commit/agent cracked a function using a lever (the money table).
CREATE TABLE IF NOT EXISTS cracked_by (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    addr        TEXT,
    lever_slug  TEXT,
    commit_sha  TEXT,
    delta       TEXT,                  -- "+1 byte-exact", "-> 100%"
    ts          REAL,
    UNIQUE(addr, lever_slug, commit_sha)
);
CREATE INDEX IF NOT EXISTS idx_crack_lever ON cracked_by(lever_slug);
CREATE INDEX IF NOT EXISTS idx_crack_addr  ON cracked_by(addr);

-- Technique timeline scraped from git (TU-level when no fn named).
CREATE TABLE IF NOT EXISTS commits (
    sha         TEXT PRIMARY KEY,
    ts          REAL,
    subject     TEXT,
    tu          TEXT,
    fn_addr     TEXT,
    delta       INTEGER,               -- +N byte-exact when parseable
    lever_hint  TEXT                   -- raw parenthetical, e.g. "(peephole-off pragma swap)"
);
CREATE INDEX IF NOT EXISTS idx_commit_tu ON commits(tu);

-- New methods harvested from other decomp projects (tww/melee/prime/xd...).
CREATE TABLE IF NOT EXISTS externals (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    project     TEXT,
    commit_sha  TEXT,
    ts          REAL,
    summary     TEXT,
    url         TEXT,
    lever_slug  TEXT,                  -- mapped to one of ours, if any
    UNIQUE(project, commit_sha)
);

-- Direct function relationship edges. Source rows are deterministic evidence
-- (src C parser, Ghidra p-code bundles, later AI-reviewed imports), not guesses.
CREATE TABLE IF NOT EXISTS function_calls (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    caller      TEXT NOT NULL,
    callee      TEXT NOT NULL,
    caller_tu   TEXT,
    callee_tu   TEXT,
    callsite    TEXT,
    source      TEXT,                  -- src-c / ghidra-pcode / ai-reviewed
    confidence  TEXT,                  -- low / medium / high
    evidence    TEXT,
    updated_at  REAL,
    UNIQUE(caller, callee, callsite, source)
);
CREATE INDEX IF NOT EXISTS idx_calls_caller ON function_calls(caller);
CREATE INDEX IF NOT EXISTS idx_calls_callee ON function_calls(callee);
CREATE INDEX IF NOT EXISTS idx_calls_tu     ON function_calls(caller_tu, callee_tu);

-- Candidate names and provenance, including call-neighborhood derived evidence.
CREATE TABLE IF NOT EXISTS name_evidence (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol      TEXT NOT NULL,
    candidate   TEXT NOT NULL,
    source      TEXT NOT NULL,          -- current-symbol / symbolmap / calltag / ai-reviewed
    confidence  TEXT,
    score       REAL,
    evidence    TEXT,
    updated_at  REAL,
    UNIQUE(symbol, candidate, source)
);
CREATE INDEX IF NOT EXISTS idx_name_evidence_symbol ON name_evidence(symbol);
CREATE INDEX IF NOT EXISTS idx_name_evidence_score  ON name_evidence(score);

-- Lightweight "this belongs with X" labels. These are intentionally separate
-- from renames so low/medium-confidence context can accumulate without
-- prematurely changing source or symbols.
CREATE TABLE IF NOT EXISTS function_tags (
    symbol      TEXT NOT NULL,
    tag         TEXT NOT NULL,
    kind        TEXT DEFAULT 'calltag',
    confidence  TEXT,
    score       REAL,
    evidence    TEXT,
    updated_at  REAL,
    UNIQUE(symbol, tag, kind)
);
CREATE INDEX IF NOT EXISTS idx_function_tags_symbol ON function_tags(symbol);
CREATE INDEX IF NOT EXISTS idx_function_tags_tag    ON function_tags(tag);

CREATE TABLE IF NOT EXISTS meta (k TEXT PRIMARY KEY, v TEXT);
"""

# ---- Seed data ---------------------------------------------------------------

WALL_SEED = [
    ("W1", "Data-flow-locked register allocation", 0,
     "Survivor-register coloring fixed by data-flow shape; allocator-inversion can't reach it."),
    ("W2", "Instruction scheduling", 0,
     "CW issues loads/ops in an order C can't steer (e.g. SDA-load-first)."),
    ("W3", "Scheduling/peephole interplay", 0, "Schedule + peephole fusion not C-controllable."),
    ("W6", "Jumptable NAME-only mismatch", 0,
     "Bytes link identical; sole diff is anonymous local jumptable symbol vs dtk-named one."),
    ("W-operand-order", "Commutative operand order to fresh register", 0,
     "Target emits addr/value store operands in an order CW won't reproduce (all CW versions swept)."),
    ("W-peephole-branch-merge", "Peephole branch fusion", 0,
     "CW fuses bne L;blr -> beqlr; target keeps the inner null-check un-merged."),
    ("W-sda-reloc-c", "SDA magic-double literal-pool reloc", 0,
     "Auto int->float @NNNN pool ref vs target's named lbl_XXXX@sda21."),
    ("W-fpr-rotation", "Volatile FPR rotation + conversion-literal reloc", 0,
     "fXX vs fYY temp coloring driven by first-use; conversion-literal reloc artifact."),
]

# Curated from CW_QUIRKS.md + the feedback_* memory ledger. These are the reusable
# C-side transforms; cracked_by edges link them to the functions they actually moved.
LEVER_SEED = [
    ("ctr-loop-inline", "Inline CTR count into for-init", 0,
     "for(i=*(u32*)lbl;i!=0;i--) keeps mtctr/bdnz; a separate local + if-guard suppresses it."),
    ("volatile-cast-reload", "volatile cast defeats byte/SDA CSE", 0,
     "*(volatile u8*)p / *(volatile T*)&lbl forces a reload where CW would CSE two reads."),
    ("peephole-off", "#pragma peephole off", 1,
     "Defeats branchless-ternary / branch-fusion peephole on a single function."),
    ("scheduling-on", "#pragma scheduling on", 1, "Forces CW instruction scheduling to match target order."),
    ("scheduling-peephole-combo", "scheduling on + peephole off", 1,
     "COMBO unlocks load-batching where neither pragma alone matches."),
    ("fp-contract", "#pragma fp_contract on", 1, "Fuses a*b+c into fmadds (sticky pragma)."),
    ("block-scope-sda-reload", "Block-scope decl forces SDA reload", 1,
     "Separate { } blocks defeat CSE of a twice-loaded SDA symbol; opt-level gated."),
    ("comma-progressive-chain", "Comma operator progressive index", 0,
     "(0,expr) / (iVar=N, key!=tbl[N]) chain produces sequential li rN,i for small-table lookup."),
    ("switch-case0-default", "case 0 + default for dead cmpwi", 0,
     "Adding case 0: sharing default body emits target's dead cmpwi r0,0 prologue."),
    ("switch-one-case-beq-b", "One-case switch forces beq;b;L shape", 0,
     "switch(x){case 0:...;break;} emits beq L; b end; L: form no if/goto reproduces."),
    ("s16-cast-at-use", "s32 local + (s16) cast at use site", 0,
     "Declare s32, cast (s16) only at the arithmetic use to place extsh where target wants it."),
    ("s16-float-direct-cast", "(s16)f32 direct cast", 0,
     "Direct (s16)f32 (not (s16)(s32)f32) avoids a spurious extsh before sth; pairs w/ peephole off."),
    ("f32-array-frame", "f32 buf[N] to force frame size", 0,
     "Convert scalar f32 locals to an array to reserve the target's larger stack frame."),
    ("bit-shift-vs-signed-compare", "((u32)*p>>N)&1 over (s8)*p<0", 0,
     "Bit-shift form emits extrwi+cmplwi matching target instead of extsb+cmpwi."),
    ("nested-call-r3", "Nest call into arg to keep r3", 0,
     "Inlining a call into the next call's arg avoids a non-volatile spill (smaller prologue)."),
    ("sda21-scalar-amp", "extern scalar + &lbl for SDA array", 0,
     "extern u16 lbl_X; (&lbl_X)[idx] emits li rN,lbl@sda21 + offset; array decl emits wrong lis+addi."),
    ("goto-ret0-inversion", "goto _ret0 flips branch inversion", 0,
     "if(ptr==NULL) goto _ret0; flips CW's bne load-path to target's beq null-path for getters."),
    ("named-struct-block-copy", "Named-struct cast for block copy", 0,
     "*(struct S*)dst=*(struct S*)src matches CW lwzu/stwu/bdnz interleave; anon casts fail."),
    ("per-fn-opt-level", "Per-function #pragma optimization_level", 1,
     "In-body opt-level override matches functions compiled at a different level than the TU."),
    ("o4s-flag", "-O4,s TU flag", 1, "Size-opt flag; correct for some TUs (script/event), net-negative for others (battle)."),
    ("real-signature", "Narrow K&R extern to real signature", 0,
     "extern void fn() -> extern u32 fn(u32,s32) fixes param-move/clrlslwi instruction choice."),
    ("u16-s32-param-demotion", "Demote u16 param to s32", 0,
     "Emits clrlslwi on the original param reg instead of an extra clrlwi/mr."),
    ("dropped-float-vararg", "Restore dropped float vararg", 0,
     "crset cr1eq vs crclr before a varargs call = a dropped FLOAT arg; restore it (use a fr1 param)."),
    ("dropped-int-vararg", "Restore dropped leading int vararg", 0,
     "mr rN,r3 before a varargs call = a dropped leading int arg; restore it."),
    ("inner-parens-assoc", "Inner parens flip FP add order", 0,
     "Parenthesizing (a±b) reorders CW's FP add evaluation to match target."),
    ("dead-compare-const", "Dead bool-materialize compare", 0,
     "if(x==0)return 0; if(x==-1)return 1; return 1; cracks some bool-materialize walls."),
    ("dowhile-break-lwzu", "do{...break}while(0) + compound-assign", 0,
     "Reaches lwzu/update-form via do/while(0) with break and p+=K compound assignment."),
    ("struct-copy-contiguous", "Named-struct copy for store order", 0,
     "Matches CW contiguous-word store interleave when a plain copy mismatches on order."),
]


def db(create=False):
    if not create and not os.path.exists(DB_PATH):
        sys.exit(f"kg.db not found at {DB_PATH} — run `python kg.py init` first.")
    con = sqlite3.connect(DB_PATH)
    con.row_factory = sqlite3.Row
    con.execute("PRAGMA foreign_keys=ON")
    con.executescript(SCHEMA)
    return con


def cmd_init(_args):
    con = db(create=True)
    con.executescript(SCHEMA)
    for cls, title, ctrl, desc in WALL_SEED:
        con.execute("INSERT OR IGNORE INTO walls(class,title,c_controllable,description) VALUES(?,?,?,?)",
                    (cls, title, ctrl, desc))
    for slug, title, og, desc in LEVER_SEED:
        con.execute("INSERT OR IGNORE INTO levers(slug,title,opt_gated,description,source) VALUES(?,?,?,?,?)",
                    (slug, title, og, desc, "CW_QUIRKS.md/memory"))
    con.execute("INSERT OR REPLACE INTO meta(k,v) VALUES('schema_version','1')")
    con.commit()
    print(f"initialized {DB_PATH}")
    print(f"  seeded {len(WALL_SEED)} wall classes, {len(LEVER_SEED)} levers")


def _norm_pct(s):
    try:
        return float(s)
    except (TypeError, ValueError):
        return None


def ingest_scratches(con):
    if not os.path.exists(SCRATCHES):
        print("  scratches.json: not found, skipping")
        return 0
    data = json.load(open(SCRATCHES, encoding="utf-8", errors="replace"))
    n = 0
    now = time.time()
    for addr, rec in data.items():
        pct = _norm_pct(rec.get("local_pct"))
        if pct is None:
            pct = _norm_pct(rec.get("scratch_pct"))
        status = (rec.get("status") or "UNKNOWN").upper()
        con.execute("""
            INSERT INTO functions(addr,tu,byte_pct,compiler,status,scratch_url,notes,updated_at)
            VALUES(?,?,?,?,?,?,?,?)
            ON CONFLICT(addr) DO UPDATE SET
              tu=COALESCE(excluded.tu,functions.tu),
              byte_pct=COALESCE(excluded.byte_pct,functions.byte_pct),
              compiler=COALESCE(excluded.compiler,functions.compiler),
              status=excluded.status,
              scratch_url=COALESCE(excluded.scratch_url,functions.scratch_url),
              notes=COALESCE(excluded.notes,functions.notes),
              updated_at=excluded.updated_at
        """, (addr, rec.get("tu"), pct, rec.get("compiler"), status,
              rec.get("url"), rec.get("notes"), now))
        n += 1
    return n


def ingest_equivalent(con):
    if not os.path.exists(EQUIVALENT):
        print("  equivalent.txt: not found, skipping")
        return 0
    n = 0
    now = time.time()
    for raw in open(EQUIVALENT, encoding="utf-8", errors="replace"):
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        # token before any whitespace/# is the symbol; rest (after # or — ) is comment
        m = re.match(r"^(\S+)\s*(.*)$", line)
        if not m:
            continue
        addr, rest = m.group(1), m.group(2)
        comment = rest.lstrip("#").strip()
        pcts = PCT_RE.findall(comment)
        pct = _norm_pct(pcts[0]) if pcts else None
        wcm = WALLCLASS_RE.search(comment)
        wall = wcm.group(0) if wcm else None
        # normalize W-Operand-Order-ish to our seeded slugs (best-effort)
        tu = None
        tum = TU_RE.search(comment)
        if tum:
            tu = tum.group(1)
        con.execute("""
            INSERT INTO functions(addr,tu,byte_pct,status,is_equivalent,wall_class,notes,updated_at)
            VALUES(?,?,?,?,1,?,?,?)
            ON CONFLICT(addr) DO UPDATE SET
              tu=COALESCE(functions.tu,excluded.tu),
              byte_pct=COALESCE(excluded.byte_pct,functions.byte_pct),
              status='WALL', is_equivalent=1,
              wall_class=COALESCE(excluded.wall_class,functions.wall_class),
              notes=COALESCE(functions.notes,excluded.notes),
              updated_at=excluded.updated_at
        """, (addr, tu, pct, "WALL", wall, comment[:400], now))
        n += 1
    return n


def ingest_walls_md(con):
    if not os.path.exists(WALLS_MD):
        print("  WALLS.md: not found, skipping")
        return 0
    text = open(WALLS_MD, encoding="utf-8", errors="replace").read()
    n = 0
    now = time.time()
    # per-function wall entries: any line that names a fn_ AND a W-class
    for line in text.splitlines():
        if "fn_" not in line:
            continue
        fns = FN_RE.findall(line)
        if not fns:
            continue
        wcm = WALLCLASS_RE.search(line)
        wall = wcm.group(0) if wcm else None
        pcts = PCT_RE.findall(line)
        pct = _norm_pct(pcts[0]) if pcts else None
        for addr in fns:
            con.execute("""
                INSERT INTO functions(addr,byte_pct,wall_class,status,updated_at)
                VALUES(?,?,?,?,?)
                ON CONFLICT(addr) DO UPDATE SET
                  wall_class=COALESCE(excluded.wall_class,functions.wall_class),
                  byte_pct=COALESCE(functions.byte_pct,excluded.byte_pct),
                  updated_at=excluded.updated_at
            """, (addr, pct, wall, "WALL", now))
            n += 1
    return n


def _git(*args):
    return subprocess.run(["git", "-C", REPO, *args],
                          capture_output=True, text=True, encoding="utf-8",
                          errors="replace").stdout


def ingest_git(con):
    # %H<US>%ct<US>%s
    out = _git("log", "--no-merges", "--pretty=format:%H\x1f%ct\x1f%s")
    n_commits = 0
    n_cracks = 0
    lever_slugs = {r["slug"] for r in con.execute("SELECT slug FROM levers")}
    # quick keyword->slug map for lever-hint mining
    hint_map = {
        "peephole": "peephole-off", "scheduling": "scheduling-on", "fp_contract": "fp-contract",
        "fmadds": "fp-contract", "-o4,s": "o4s-flag", "o4,s": "o4s-flag",
        "opt-level": "per-fn-opt-level", "optimization_level": "per-fn-opt-level",
        "per-fn opt": "per-fn-opt-level", "block-scope": "block-scope-sda-reload",
        "block scope": "block-scope-sda-reload", "comma": "comma-progressive-chain",
        "volatile": "volatile-cast-reload", "vararg": "dropped-float-vararg",
        "signature": "real-signature", "struct copy": "named-struct-block-copy",
        "named-struct": "named-struct-block-copy", "ctr": "ctr-loop-inline",
    }
    for raw in out.splitlines():
        parts = raw.split("\x1f")
        if len(parts) != 3:
            continue
        sha, ts, subj = parts
        low = subj.lower()
        is_technique = any(k in low for k in
                           ("byte-exact", "100%", "->", "decomp:", "match", "pragma", "wall", "lever"))
        if not is_technique:
            continue
        tum = TU_RE.search(subj)
        tu = tum.group(1) if tum else None
        fns = FN_RE.findall(subj)
        fn_addr = fns[0] if fns else None
        dm = re.search(r"\+(\d+)\s*byte-exact", subj)
        delta = int(dm.group(1)) if dm else None
        paren = re.search(r"\(([^)]*)\)", subj)
        lever_hint = paren.group(1) if paren else None
        try:
            tsf = float(ts)
        except ValueError:
            tsf = None
        con.execute("""INSERT OR REPLACE INTO commits(sha,ts,subject,tu,fn_addr,delta,lever_hint)
                       VALUES(?,?,?,?,?,?,?)""", (sha, tsf, subj, tu, fn_addr, delta, lever_hint))
        n_commits += 1
        # mine cracked_by edges when a fn AND a lever hint are both present
        if fn_addr and lever_hint:
            hl = lever_hint.lower()
            matched = None
            for kw, slug in hint_map.items():
                if kw in hl and slug in lever_slugs:
                    matched = slug
                    break
            if matched:
                con.execute("""INSERT OR IGNORE INTO cracked_by(addr,lever_slug,commit_sha,delta,ts)
                               VALUES(?,?,?,?,?)""",
                            (fn_addr, matched, sha, f"+{delta}" if delta else "-> 100%", tsf))
                n_cracks += 1
    return n_commits, n_cracks


def cmd_backfill(_args):
    con = db()
    print("backfilling knowledge graph...")
    s = ingest_scratches(con); print(f"  scratches.json: {s} functions")
    e = ingest_equivalent(con); print(f"  equivalent.txt: {e} wall/equivalent functions")
    w = ingest_walls_md(con);   print(f"  WALLS.md:       {w} per-fn wall entries")
    c, cr = ingest_git(con);    print(f"  git log:        {c} technique commits, {cr} fn-level crack edges")
    con.execute("INSERT OR REPLACE INTO meta(k,v) VALUES('last_backfill',?)", (str(time.time()),))
    con.commit()
    cmd_stats(_args)


# ---- Named "sudoku" queries --------------------------------------------------

QUERIES = {
    "nearmiss-by-tu": (
        "Near-miss functions (90-99%) grouped by TU - best ROI targets.",
        """SELECT tu, COUNT(*) n, ROUND(AVG(byte_pct),1) avg_pct
           FROM functions WHERE byte_pct>=90 AND byte_pct<100 AND COALESCE(is_equivalent,0)=0
           GROUP BY tu ORDER BY n DESC LIMIT 40"""),
    "siblings": (
        "Unmatched functions in the same TU as a given fn (sudoku propagation). arg=fn_addr",
        """SELECT f2.addr, f2.byte_pct, f2.status, f2.wall_class
           FROM functions f1 JOIN functions f2 ON f1.tu=f2.tu
           WHERE f1.addr=:arg AND f2.addr<>:arg AND (f2.byte_pct IS NULL OR f2.byte_pct<100)
           ORDER BY f2.byte_pct DESC"""),
    "top-levers": (
        "Levers ranked by how many functions they cracked (git-mined).",
        """SELECT l.slug, l.title, l.opt_gated, COUNT(c.id) cracks
           FROM levers l LEFT JOIN cracked_by c ON c.lever_slug=l.slug
           GROUP BY l.slug ORDER BY cracks DESC, l.slug"""),
    "wall-load": (
        "Wall classes ranked by how many functions are stuck on them.",
        """SELECT COALESCE(wall_class,'(none)') wall, COUNT(*) n, ROUND(AVG(byte_pct),1) avg_pct
           FROM functions WHERE status='WALL' OR is_equivalent=1
           GROUP BY wall_class ORDER BY n DESC"""),
    "lever-targets": (
        "Near-miss fns in TUs where a given lever already worked - try it on the siblings. arg=lever_slug",
        """SELECT DISTINCT f.addr, f.tu, f.byte_pct
           FROM cracked_by c
           JOIN functions cf ON cf.addr=c.addr
           JOIN functions f  ON f.tu=cf.tu
           WHERE c.lever_slug=:arg AND f.byte_pct>=90 AND f.byte_pct<100
             AND COALESCE(f.is_equivalent,0)=0
           ORDER BY f.byte_pct DESC LIMIT 50"""),
    "tu-progress": (
        "Per-TU rollup: done / nearmiss / wall counts.",
        """SELECT tu,
                  SUM(CASE WHEN byte_pct>=100 THEN 1 ELSE 0 END) done,
                  SUM(CASE WHEN byte_pct>=90 AND byte_pct<100 THEN 1 ELSE 0 END) nearmiss,
                  SUM(CASE WHEN is_equivalent=1 OR status='WALL' THEN 1 ELSE 0 END) walls,
                  COUNT(*) total
           FROM functions WHERE tu IS NOT NULL
           GROUP BY tu ORDER BY nearmiss DESC LIMIT 40"""),
    "recent-techniques": (
        "Most recent technique commits with a parsed lever hint.",
        """SELECT date(ts,'unixepoch') d, fn_addr, tu, delta, lever_hint
           FROM commits WHERE lever_hint IS NOT NULL ORDER BY ts DESC LIMIT 30"""),
    "externals": (
        "Recent technique commits harvested from other decomp projects.",
        """SELECT project, date(ts,'unixepoch') d, COALESCE(lever_slug,'-') lever, summary
           FROM externals ORDER BY ts DESC LIMIT 40"""),
    "external-levers": (
        "Cross-project: which of our levers other projects are actively using.",
        """SELECT lever_slug, COUNT(*) n, GROUP_CONCAT(DISTINCT project) projects
           FROM externals WHERE lever_slug IS NOT NULL
           GROUP BY lever_slug ORDER BY n DESC"""),
    "call-neighborhood": (
        "Direct callers/callees for a function, with TU context and confidence. arg=fn_addr/name",
        """SELECT 'calls' AS dir, callee AS fn, callee_tu AS tu, confidence, source, evidence
           FROM function_calls WHERE caller=:arg
           UNION ALL
           SELECT 'called-by' AS dir, caller AS fn, caller_tu AS tu, confidence, source, evidence
           FROM function_calls WHERE callee=:arg
           ORDER BY dir, confidence DESC, fn LIMIT 80"""),
    "calltags": (
        "Function calltags ranked by accumulated relationship evidence.",
        """SELECT tag, confidence, COUNT(*) functions, ROUND(AVG(score), 2) avg_score
           FROM function_tags
           WHERE kind='calltag'
           GROUP BY tag, confidence
           ORDER BY avg_score DESC, functions DESC, tag LIMIT 50"""),
    "name-evidence": (
        "Name candidates and calltag evidence for one function. arg=fn_addr/name",
        """SELECT symbol, candidate, source, confidence, ROUND(score, 2) score, evidence
           FROM name_evidence
           WHERE symbol=:arg
           ORDER BY score DESC, confidence DESC, source LIMIT 50"""),
}


def cmd_query(args):
    name = args.name
    if name == "list" or name not in QUERIES:
        if name != "list":
            print(f"unknown query '{name}'.\n")
        print("named queries:")
        for k, (desc, _) in QUERIES.items():
            print(f"  {k:20s} {desc}")
        return
    desc, sql = QUERIES[name]
    con = db()
    print(f"# {desc}\n")
    params = {"arg": args.arg} if ":arg" in sql else {}
    if ":arg" in sql and not args.arg:
        sys.exit(f"query '{name}' needs an argument: python kg.py q {name} <arg>")
    rows = list(con.execute(sql, params))
    _print_rows(rows)


def cmd_sql(args):
    q = args.query.strip()
    if not re.match(r"(?is)^\s*(select|with)\b", q):
        sys.exit("sql: read-only — only SELECT/WITH queries allowed.")
    con = db()
    _print_rows(list(con.execute(q)))


def _print_rows(rows):
    if not rows:
        print("(no rows)")
        return
    cols = rows[0].keys()
    widths = {c: max(len(c), *(len(str(r[c])) for r in rows)) for c in cols}
    print("  ".join(c.ljust(widths[c]) for c in cols))
    print("  ".join("-" * widths[c] for c in cols))
    for r in rows:
        print("  ".join(str(r[c]).ljust(widths[c]) for c in cols))
    print(f"\n({len(rows)} rows)")


def cmd_stats(_args):
    con = db()
    for t in (
        "functions", "levers", "walls", "cracked_by", "commits", "externals",
        "function_calls", "name_evidence", "function_tags",
    ):
        n = con.execute(f"SELECT COUNT(*) FROM {t}").fetchone()[0]
        print(f"  {t:12s} {n}")


def cmd_record_crack(args):
    con = db()
    con.execute("""INSERT OR IGNORE INTO cracked_by(addr,lever_slug,commit_sha,delta,ts)
                   VALUES(?,?,?,?,?)""",
                (args.addr, args.lever_slug, args.commit, args.delta, time.time()))
    con.commit()
    print(f"recorded crack: {args.addr} via {args.lever_slug}")


def cmd_record_lever(args):
    con = db()
    con.execute("""INSERT INTO levers(slug,title,description,opt_gated,source)
                   VALUES(?,?,?,?,?)
                   ON CONFLICT(slug) DO UPDATE SET
                     title=excluded.title, description=COALESCE(excluded.description,levers.description),
                     opt_gated=excluded.opt_gated""",
                (args.slug, args.title, args.desc, 1 if args.opt_gated else 0, args.source))
    con.commit()
    print(f"recorded lever: {args.slug}")


def main():
    p = argparse.ArgumentParser(description="decomp Knowledge Graph")
    sub = p.add_subparsers(dest="cmd", required=True)
    sub.add_parser("init").set_defaults(fn=cmd_init)
    sub.add_parser("backfill").set_defaults(fn=cmd_backfill)
    sub.add_parser("stats").set_defaults(fn=cmd_stats)
    q = sub.add_parser("q"); q.add_argument("name"); q.add_argument("arg", nargs="?"); q.set_defaults(fn=cmd_query)
    s = sub.add_parser("sql"); s.add_argument("query"); s.set_defaults(fn=cmd_sql)
    rc = sub.add_parser("record-crack")
    rc.add_argument("addr"); rc.add_argument("lever_slug")
    rc.add_argument("--commit", default=None); rc.add_argument("--delta", default=None)
    rc.set_defaults(fn=cmd_record_crack)
    rl = sub.add_parser("record-lever")
    rl.add_argument("slug"); rl.add_argument("--title", required=True)
    rl.add_argument("--desc", default=None); rl.add_argument("--opt-gated", action="store_true")
    rl.add_argument("--source", default="agent")
    rl.set_defaults(fn=cmd_record_lever)
    args = p.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
