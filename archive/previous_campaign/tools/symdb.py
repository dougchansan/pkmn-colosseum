#!/usr/bin/env python3
"""
symdb.py - Project-wide symbol / type / signature database for the Pokemon
Colosseum (GC6E01) matching decompilation.

Purpose
=======
Every function used to be decompiled in a vacuum: each agent invented its own
externs, struct field offsets, and signatures locally. That produced two
recurring failures:

  1. "conflicting externs break CodeWarrior compilation" - two .c files declare
     the same symbol with different types/signatures.
  2. unmatchable large functions - a single wrong struct field offset cascades
     into many register/scheduling diffs that get misdiagnosed as needing a
     compiler pragma.

This tool is the single source of truth that every agent reads and writes
through. The high-value piece is `--check-conflicts`, which scans all src/**.c
for `extern` declarations of the same symbol with DIFFERENT types/signatures and
reports every conflict (file:line pairs).

Database file: config/GC6E01/symdb.json

Usage
=====
    # Look up a symbol (address / type / signature / struct info)
    python tools/symdb.py lookup fn_800F07A8
    python tools/symdb.py lookup BattlePokemon

    # Record / update a symbol's canonical signature or type
    python tools/symdb.py set-sig fn_800F07A8 \
        "void* fn_800F07A8(s32 priority, void* parent, s32 stackSize)"
    python tools/symdb.py set-type lbl_80406A20 "GSglobals" --kind object

    # Record a struct's field offsets/types
    python tools/symdb.py set-field BattlePokemon 0x00 species u16
    python tools/symdb.py set-field BattlePokemon 0x04 hp u16

    # Scan the live src tree for conflicting extern declarations
    python tools/symdb.py check-conflicts
    python tools/symdb.py check-conflicts --report logs/extern_conflicts.txt

    # Seed the database from the existing symbol map (addresses -> names)
    python tools/symdb.py seed

    # Sanity-check the database file
    python tools/symdb.py validate

The pre-1.0 argparse subcommand UI mirrors the other tools in tools/.
"""
import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
SYMBOLS_TXT = PROJECT_ROOT / "config" / "GC6E01" / "symbols.txt"
DB_PATH = PROJECT_ROOT / "config" / "GC6E01" / "symdb.json"
SRC_DIR = PROJECT_ROOT / "src"

# ---------------------------------------------------------------------------
# symbols.txt parsing
# ---------------------------------------------------------------------------

# e.g. fn_800056C4 = .text:0x800056C4; // type:function size:0x10 // Proposed: GS_MainLoopInit
SYM_RE = re.compile(
    r"^\s*(?P<name>[A-Za-z_$][\w$]*)\s*=\s*"
    r"(?P<section>[.\w]+):0x(?P<addr>[0-9A-Fa-f]+)\s*;"
    r"(?P<rest>.*)$"
)
TYPE_RE = re.compile(r"type:(?P<type>\w+)")
SIZE_RE = re.compile(r"size:0x(?P<size>[0-9A-Fa-f]+)")
SCOPE_RE = re.compile(r"scope:(?P<scope>\w+)")
PROPOSED_RE = re.compile(r"Proposed:\s*(?P<proposed>[\w$]+)")


def parse_symbols_txt(path=SYMBOLS_TXT):
    """Yield dicts describing every entry in the splits-style symbols file."""
    if not path.exists():
        return
    with open(path, encoding="utf-8", errors="replace") as f:
        for lineno, line in enumerate(f, 1):
            m = SYM_RE.match(line)
            if not m:
                continue
            rest = m.group("rest")
            entry = {
                "name": m.group("name"),
                "addr": "0x" + m.group("addr").upper().rjust(8, "0"),
                "section": m.group("section"),
                "line": lineno,
            }
            tm = TYPE_RE.search(rest)
            if tm:
                entry["kind"] = tm.group("type")
            sm = SIZE_RE.search(rest)
            if sm:
                entry["size"] = "0x" + sm.group("size").upper()
            scm = SCOPE_RE.search(rest)
            if scm:
                entry["scope"] = scm.group("scope")
            pm = PROPOSED_RE.search(rest)
            if pm:
                entry["proposed"] = pm.group("proposed")
            yield entry


# ---------------------------------------------------------------------------
# Database load / save
# ---------------------------------------------------------------------------

def empty_db():
    return {
        "_meta": {
            "version": 1,
            "description": "Pokemon Colosseum (GC6E01) symbol/type/signature DB. "
                           "Single source of truth - see docs/type_database.md.",
        },
        # name -> {addr, kind, size, scope, section, type, signature, note}
        "symbols": {},
        # struct name -> {size, fields: {"0xNN": {name, type, note}}, note}
        "structs": {},
    }


def load_db(path=DB_PATH):
    if not path.exists():
        return empty_db()
    with open(path, encoding="utf-8") as f:
        db = json.load(f)
    # forward-compat: ensure top-level keys exist
    db.setdefault("symbols", {})
    db.setdefault("structs", {})
    db.setdefault("_meta", empty_db()["_meta"])
    return db


def save_db(db, path=DB_PATH):
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(db, f, indent=2, sort_keys=True)
        f.write("\n")


# ---------------------------------------------------------------------------
# extern declaration scanning (the conflict detector)
# ---------------------------------------------------------------------------

# Strip C comments so we never parse an extern that lives inside one.
BLOCK_COMMENT_RE = re.compile(r"/\*.*?\*/", re.DOTALL)
LINE_COMMENT_RE = re.compile(r"//[^\n]*")

# An extern declaration: the keyword, everything up to (and including) the ';'.
EXTERN_RE = re.compile(r"\bextern\b(?P<body>[^;{]*);", re.DOTALL)

# Pull the declared identifier out of an extern body. We accept the common
# shapes seen in this tree:
#   extern void fn_X(args);
#   extern u32  GStaskRegister(args);
#   extern FuncPtr _ctors[];
#   extern const char* __DVDVersion;
#   extern DVDCommandBlock DummyCommandBlock_803FC3A0;
# The identifier is the last C identifier that is immediately followed by '('
# (a function) or by '[', '=' or end-of-decl (an object).
DECL_NAME_RE = re.compile(r"([A-Za-z_$][\w$]*)\s*(?:\(|\[|=|$)")


def _strip_comments_keep_lines(text):
    """Remove comments but preserve newline count so line numbers stay correct."""
    def repl(m):
        return "\n" * m.group(0).count("\n")
    text = BLOCK_COMMENT_RE.sub(repl, text)
    text = LINE_COMMENT_RE.sub("", text)
    return text


def _normalize_sig(body, name):
    """
    Produce a canonical, whitespace- and param-name-insensitive signature
    string for an extern body so that two declarations that differ only in
    parameter names or spacing are treated as identical.

    For a function `<ret> name(<params>)` we keep the return type and the list
    of parameter *types* (param names dropped). For an object we keep the full
    type spelling. Returns (kind, normalized_string).
    """
    body = " ".join(body.split())  # collapse whitespace

    paren = body.find("(")
    if paren != -1 and body.rstrip().endswith(")"):
        ret = body[:paren]
        params = body[paren + 1:body.rfind(")")]
        # Split top-level params on commas (no nested parens expected here).
        parts = [p.strip() for p in params.split(",")] if params.strip() else []
        norm_params = []
        for p in parts:
            if p in ("void", "", "..."):
                norm_params.append(p)
                continue
            # Drop a trailing identifier that is the parameter name.
            p2 = re.sub(r"\b[A-Za-z_$][\w$]*\s*$", "", p).strip()
            # If stripping ate the whole thing (e.g. bare "int"), keep original.
            norm_params.append(p2 if p2 else p)
        ret = ret.replace(name, "").strip()
        return "func", "%s(%s)" % (ret, ", ".join(norm_params))

    # object / variable: drop the name, keep the type spelling (incl. [] / *).
    spelled = body.replace(name, "", 1).strip()
    spelled = " ".join(spelled.split())
    return "object", spelled


def scan_externs(src_dir=SRC_DIR):
    """
    Return {symbol_name: [ {file, line, kind, sig, raw} ... ]} for every extern
    declaration found under src_dir.
    """
    out = defaultdict(list)
    for cfile in sorted(src_dir.rglob("*.c")):
        try:
            raw = cfile.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        clean = _strip_comments_keep_lines(raw)
        for m in EXTERN_RE.finditer(clean):
            body = m.group("body").strip()
            if not body:
                continue
            nm = DECL_NAME_RE.search(body + " ")
            if not nm:
                continue
            name = nm.group(1)
            # Skip pure type keywords that can sneak through (typedefs etc.).
            if name in ("struct", "union", "enum", "const", "volatile"):
                continue
            kind, sig = _normalize_sig(body, name)
            # line number = newlines before the match start, +1
            line = clean.count("\n", 0, m.start()) + 1
            rel = cfile.relative_to(PROJECT_ROOT).as_posix()
            out[name].append({
                "file": rel,
                "line": line,
                "kind": kind,
                "sig": sig,
                "raw": " ".join(("extern " + body).split()),
            })
    return out


def find_conflicts(externs):
    """
    Given the scan_externs() map, return a sorted list of conflicts. A conflict
    is a symbol declared extern in 2+ places with 2+ distinct normalized
    signatures. Each conflict is (name, {sig: [decls...]}).
    """
    conflicts = []
    for name, decls in externs.items():
        by_sig = defaultdict(list)
        for d in decls:
            by_sig[(d["kind"], d["sig"])].append(d)
        if len(by_sig) > 1:
            conflicts.append((name, by_sig))
    conflicts.sort(key=lambda c: c[0])
    return conflicts


# ---------------------------------------------------------------------------
# Programmatic API (used by import_reference.py and batch drivers)
# ---------------------------------------------------------------------------

def lookup_decl(name, db=None):
    """Return the curated canonical signature string for `name`, or None.

    import_reference.py routes externs through this so a staged import reuses
    the project's agreed declaration instead of inventing a fresh one.
    """
    db = db if db is not None else load_db()
    sym = db.get("symbols", {}).get(name)
    if sym and sym.get("signature"):
        return sym["signature"]
    return None


def name_to_addr(name, db=None):
    """Return the '0x........' address for a symbol name (DB first, then map)."""
    db = db if db is not None else load_db()
    sym = db.get("symbols", {}).get(name)
    if sym and sym.get("addr"):
        return sym["addr"]
    for e in parse_symbols_txt():
        if e["name"] == name or e.get("proposed") == name:
            return e["addr"]
    return None


def addr_to_name(addr, db=None):
    """Return the symbol name defined at a given address ('0xXXXXXXXX')."""
    want = "0x" + str(addr).lower().replace("0x", "").rjust(8, "0").upper()
    db = db if db is not None else load_db()
    for nm, sym in db.get("symbols", {}).items():
        if sym.get("addr") == want:
            return nm
    return None


def funcs_in_range(lo, hi, db=None):
    """Return [(addr_int, name, size_int)] for function symbols in [lo, hi)."""
    db = db if db is not None else load_db()
    out = []
    for nm, sym in db.get("symbols", {}).items():
        if sym.get("kind") != "function":
            continue
        a = sym.get("addr")
        if not a:
            continue
        ai = int(a, 16)
        if lo <= ai < hi:
            sz = int(sym["size"], 16) if sym.get("size") else 0
            out.append((ai, nm, sz))
    out.sort()
    return out


# ---------------------------------------------------------------------------
# Subcommands
# ---------------------------------------------------------------------------

def cmd_lookup(args):
    db = load_db()
    key = args.name
    found = False
    if key in db["symbols"]:
        found = True
        print("symbol: %s" % key)
        for k, v in sorted(db["symbols"][key].items()):
            print("  %-10s %s" % (k, v))
    if key in db["structs"]:
        found = True
        s = db["structs"][key]
        print("struct: %s  (size=%s)" % (key, s.get("size", "?")))
        for off in sorted(s.get("fields", {}), key=lambda x: int(x, 16)):
            fld = s["fields"][off]
            print("  %-6s %-10s %s%s" % (
                off, fld.get("type", "?"), fld.get("name", "?"),
                ("  // " + fld["note"]) if fld.get("note") else ""))
        if s.get("note"):
            print("  note: %s" % s["note"])
    if not found:
        # fall back to the raw symbol map so lookups are useful pre-seed
        for e in parse_symbols_txt():
            if e["name"] == key or e.get("proposed") == key:
                found = True
                print("symbols.txt: %s @ %s  %s" % (
                    e["name"], e["addr"],
                    " ".join("%s=%s" % (k, v) for k, v in e.items()
                             if k not in ("name", "addr", "line"))))
        if not found:
            print("not found: %s" % key)
            return 1
    return 0


def cmd_set_sig(args):
    db = load_db()
    sym = db["symbols"].setdefault(args.name, {})
    sym["signature"] = " ".join(args.signature.split())
    if args.note:
        sym["note"] = args.note
    save_db(db)
    print("set signature for %s" % args.name)
    return 0


def cmd_set_type(args):
    db = load_db()
    sym = db["symbols"].setdefault(args.name, {})
    sym["type"] = args.type
    if args.kind:
        sym["kind"] = args.kind
    if args.note:
        sym["note"] = args.note
    save_db(db)
    print("set type for %s -> %s" % (args.name, args.type))
    return 0


def cmd_set_field(args):
    db = load_db()
    s = db["structs"].setdefault(args.struct, {"fields": {}})
    s.setdefault("fields", {})
    off = args.offset.lower()
    if not off.startswith("0x"):
        off = "0x%X" % int(off, 0)
    else:
        off = "0x%X" % int(off, 16)
    s["fields"][off] = {"name": args.field, "type": args.type}
    if args.note:
        s["fields"][off]["note"] = args.note
    save_db(db)
    print("set %s.%s @ %s : %s" % (args.struct, args.field, off, args.type))
    return 0


def cmd_set_struct_size(args):
    db = load_db()
    s = db["structs"].setdefault(args.struct, {"fields": {}})
    s["size"] = args.size if args.size.startswith("0x") else "0x%X" % int(args.size, 0)
    save_db(db)
    print("set %s size = %s" % (args.struct, s["size"]))
    return 0


def cmd_seed(args):
    db = load_db()
    n_new = 0
    n_upd = 0
    for e in parse_symbols_txt():
        sym = db["symbols"].get(e["name"])
        record = {
            "addr": e["addr"],
            "section": e["section"],
            "kind": e.get("kind"),
        }
        if "size" in e:
            record["size"] = e["size"]
        if "scope" in e:
            record["scope"] = e["scope"]
        if "proposed" in e:
            record["proposed"] = e["proposed"]
        record = {k: v for k, v in record.items() if v is not None}
        if sym is None:
            db["symbols"][e["name"]] = record
            n_new += 1
        else:
            # Never clobber human-curated signature/type/note fields; only
            # refresh the address-map-derived facts.
            for k, v in record.items():
                if sym.get(k) != v:
                    sym[k] = v
                    n_upd += 1
    save_db(db)
    print("seeded %d new symbols, refreshed %d fields from %s"
          % (n_new, n_upd, SYMBOLS_TXT.relative_to(PROJECT_ROOT).as_posix()))
    print("database: %s (%d symbols total)"
          % (DB_PATH.relative_to(PROJECT_ROOT).as_posix(), len(db["symbols"])))
    return 0


def cmd_check_conflicts(args):
    externs = scan_externs()
    conflicts = find_conflicts(externs)

    lines = []
    lines.append("# Conflicting extern declarations - Pokemon Colosseum (GC6E01)")
    lines.append("#")
    lines.append("# A conflict = one symbol declared `extern` in 2+ .c files with")
    lines.append("# 2+ DISTINCT normalized signatures (param NAMES and whitespace")
    lines.append("# are ignored; only types / arities / return types matter).")
    lines.append("# These are the declarations that break CodeWarrior compilation")
    lines.append("# when their translation units are linked together.")
    lines.append("#")
    total_syms = len(externs)
    lines.append("# extern symbols scanned : %d" % total_syms)
    lines.append("# conflicting symbols     : %d" % len(conflicts))
    lines.append("")

    for name, by_sig in conflicts:
        lines.append("=" * 72)
        lines.append("%s  (%d distinct signatures)" % (name, len(by_sig)))
        for (kind, sig), decls in sorted(by_sig.items()):
            lines.append("  [%s] %s" % (kind, sig if sig else "(empty)"))
            for d in sorted(decls, key=lambda x: (x["file"], x["line"])):
                lines.append("       %s:%d   %s" % (d["file"], d["line"], d["raw"]))
        lines.append("")

    report = "\n".join(lines)
    print(report)

    if args.report:
        rp = Path(args.report)
        if not rp.is_absolute():
            rp = PROJECT_ROOT / rp
        rp.parent.mkdir(parents=True, exist_ok=True)
        rp.write_text(report + "\n", encoding="utf-8")
        print("\n[written to %s]" % rp)

    # Non-zero exit when conflicts exist, so CI / agents can gate on it.
    return 1 if conflicts else 0


def cmd_validate(args):
    problems = []
    db = load_db()

    # 1. Internal DB sanity.
    for name, sym in db["symbols"].items():
        addr = sym.get("addr")
        if addr and not re.fullmatch(r"0x[0-9A-Fa-f]+", addr):
            problems.append("symbol %s: bad addr %r" % (name, addr))
    for sname, s in db["structs"].items():
        for off in s.get("fields", {}):
            if not re.fullmatch(r"0x[0-9A-Fa-f]+", off):
                problems.append("struct %s: bad field offset %r" % (sname, off))

    # 2. Curated signatures must not contradict the live extern declarations.
    externs = scan_externs()
    for name, sym in db["symbols"].items():
        sig = sym.get("signature")
        if not sig:
            continue
        _, norm = _normalize_sig(sig.replace("extern", "", 1).strip().rstrip(";"), name)
        decls = externs.get(name, [])
        live = {d["sig"] for d in decls if d["kind"] == "func"}
        if live and norm not in live:
            problems.append(
                "symbol %s: DB signature %r not matched by any live extern %s"
                % (name, norm, sorted(live)))

    # 3. Re-run the conflict scan as part of validation.
    conflicts = find_conflicts(externs)
    if conflicts:
        problems.append("%d extern conflict(s) still present in src tree "
                        "(run check-conflicts)" % len(conflicts))

    if problems:
        print("VALIDATION FAILED (%d problem(s)):" % len(problems))
        for p in problems:
            print("  - %s" % p)
        return 1
    print("validation OK: %d symbols, %d structs, no extern conflicts"
          % (len(db["symbols"]), len(db["structs"])))
    return 0


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def build_parser():
    p = argparse.ArgumentParser(
        description="Symbol/type/signature database for the GC6E01 decomp.")
    sub = p.add_subparsers(dest="cmd", required=True)

    sp = sub.add_parser("lookup", help="look up a symbol or struct")
    sp.add_argument("name")
    sp.set_defaults(func=cmd_lookup)

    sp = sub.add_parser("set-sig", help="record a function's canonical signature")
    sp.add_argument("name")
    sp.add_argument("signature")
    sp.add_argument("--note")
    sp.set_defaults(func=cmd_set_sig)

    sp = sub.add_parser("set-type", help="record a symbol's type")
    sp.add_argument("name")
    sp.add_argument("type")
    sp.add_argument("--kind", help="function|object|label")
    sp.add_argument("--note")
    sp.set_defaults(func=cmd_set_type)

    sp = sub.add_parser("set-field", help="record a struct field offset/type")
    sp.add_argument("struct")
    sp.add_argument("offset", help="e.g. 0x04 or 4")
    sp.add_argument("field")
    sp.add_argument("type")
    sp.add_argument("--note")
    sp.set_defaults(func=cmd_set_field)

    sp = sub.add_parser("set-struct-size", help="record a struct's total size")
    sp.add_argument("struct")
    sp.add_argument("size", help="e.g. 0x58")
    sp.set_defaults(func=cmd_set_struct_size)

    sp = sub.add_parser("seed", help="seed DB from config/GC6E01/symbols.txt")
    sp.set_defaults(func=cmd_seed)

    sp = sub.add_parser("check-conflicts",
                        help="scan src/**.c for conflicting extern decls")
    sp.add_argument("--report", help="also write the report to this path")
    sp.set_defaults(func=cmd_check_conflicts)

    sp = sub.add_parser("validate", help="sanity-check the DB and src tree")
    sp.set_defaults(func=cmd_validate)

    return p


def main(argv=None):
    args = build_parser().parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
