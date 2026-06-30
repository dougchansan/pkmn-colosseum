#!/usr/bin/env python3
"""Mine call relationships and naming evidence into the decomp KG.

This is intentionally deterministic. It gives agents and the dashboard a shared
relationship context before any AI naming pass weighs in.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import sqlite3
import sys
import time
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[2]
DB_PATH = HERE / "kg.db"
SRC_DIR = REPO / "src"
SYMBOLS = REPO / "config" / "GC6E01" / "symbols.txt"
SM_DIR = REPO / "config" / "GC6E01" / "symbolmap"
BUNDLES_DIR = HERE / "bundles"

FN_RE = re.compile(r"\bfn_[0-9A-Fa-f]{8}\b")
SYMBOL_LINE_RE = re.compile(
    r"^([A-Za-z_.$][\w.$:@?]*)\s*=\s*\.text:(0x[0-9A-Fa-f]+);"
    r"\s*//\s*type:function\b"
)
CALL_TOKEN_RE = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")
PCODE_CALL_RE = re.compile(r"\bCALL\s+\(ram,\s*(0x[0-9A-Fa-f]+),\s*8\)")

C_KEYWORDS = {
    "if", "for", "while", "switch", "return", "sizeof", "do", "case",
    "break", "continue", "typedef", "struct", "union", "enum",
}

REL_SCHEMA = """
CREATE TABLE IF NOT EXISTS function_calls (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    caller      TEXT NOT NULL,
    callee      TEXT NOT NULL,
    caller_tu   TEXT,
    callee_tu   TEXT,
    callsite    TEXT,
    source      TEXT,
    confidence  TEXT,
    evidence    TEXT,
    updated_at  REAL,
    UNIQUE(caller, callee, callsite, source)
);
CREATE INDEX IF NOT EXISTS idx_calls_caller ON function_calls(caller);
CREATE INDEX IF NOT EXISTS idx_calls_callee ON function_calls(callee);
CREATE INDEX IF NOT EXISTS idx_calls_tu     ON function_calls(caller_tu, callee_tu);

CREATE TABLE IF NOT EXISTS name_evidence (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    symbol      TEXT NOT NULL,
    candidate   TEXT NOT NULL,
    source      TEXT NOT NULL,
    confidence  TEXT,
    score       REAL,
    evidence    TEXT,
    updated_at  REAL,
    UNIQUE(symbol, candidate, source)
);
CREATE INDEX IF NOT EXISTS idx_name_evidence_symbol ON name_evidence(symbol);
CREATE INDEX IF NOT EXISTS idx_name_evidence_score  ON name_evidence(score);

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
"""


def connect(create: bool = False) -> sqlite3.Connection:
    if not DB_PATH.exists() and not create:
        sys.exit(f"kg.db not found at {DB_PATH}; run `python tools/decomp_work/kg/kg.py init` first")
    con = sqlite3.connect(str(DB_PATH))
    con.row_factory = sqlite3.Row
    con.executescript(REL_SCHEMA)
    return con


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def repo_rel(path: Path) -> str:
    try:
        return path.relative_to(REPO).as_posix()
    except ValueError:
        return path.as_posix()


def normalize_tu(path: Path | str | None) -> str:
    if not path:
        return ""
    text = path.as_posix() if isinstance(path, Path) else str(path).replace("\\", "/")
    if text.startswith("src/"):
        return text
    if text.endswith(".c"):
        return f"src/{text}" if not text.startswith("src/") else text
    return text


def load_symbols() -> tuple[set[str], dict[int, str], dict[str, int]]:
    names: set[str] = set()
    by_addr: dict[int, str] = {}
    addr_by_name: dict[str, int] = {}
    if not SYMBOLS.exists():
        return names, by_addr, addr_by_name
    for raw in read_text(SYMBOLS).splitlines():
        match = SYMBOL_LINE_RE.match(raw.strip())
        if not match:
            continue
        name = match.group(1)
        addr = int(match.group(2), 16)
        names.add(name)
        by_addr[addr] = name
        addr_by_name[name] = addr
    return names, by_addr, addr_by_name


def load_function_tus(con: sqlite3.Connection) -> dict[str, str]:
    tus: dict[str, str] = {}
    try:
        rows = con.execute("SELECT addr, tu FROM functions WHERE tu IS NOT NULL")
    except sqlite3.Error:
        return tus
    for row in rows:
        addr = str(row["addr"])
        tu = normalize_tu(row["tu"])
        if addr and tu:
            tus[addr] = tu
    return tus


def strip_comments_and_strings(line: str, block_comment: bool) -> tuple[str, bool]:
    out: list[str] = []
    i = 0
    in_string = False
    quote = ""
    while i < len(line):
        ch = line[i]
        nxt = line[i + 1] if i + 1 < len(line) else ""
        if block_comment:
            if ch == "*" and nxt == "/":
                block_comment = False
                i += 2
            else:
                i += 1
            continue
        if in_string:
            if ch == "\\":
                i += 2
                continue
            if ch == quote:
                in_string = False
            i += 1
            continue
        if ch == "/" and nxt == "*":
            block_comment = True
            i += 2
            continue
        if ch == "/" and nxt == "/":
            break
        if ch in ("\"", "'"):
            in_string = True
            quote = ch
            out.append(" ")
            i += 1
            continue
        out.append(ch)
        i += 1
    return "".join(out), block_comment


def definition_name(signature: str, known_symbols: set[str]) -> str | None:
    text = " ".join(signature.strip().split())
    if not text or text.startswith("#"):
        return None
    if text.startswith(("extern ", "typedef ", "return ", "if ", "for ", "while ", "switch ")):
        return None
    if " asm " in f" {text} " or text.startswith("asm "):
        return None
    if ";" in text and ("{" not in text or text.index(";") < text.index("{")):
        return None
    match = re.search(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(", text)
    if not match:
        return None
    name = match.group(1)
    if name in C_KEYWORDS:
        return None
    if name in known_symbols or FN_RE.fullmatch(name):
        return name
    return None


def mine_source_calls(known_symbols: set[str], function_tus: dict[str, str],
                      max_files: int | None = None) -> tuple[list[dict[str, object]], dict[str, str]]:
    edges: dict[tuple[str, str, str], dict[str, object]] = {}
    source_defs: dict[str, str] = {}
    files = sorted(p for p in SRC_DIR.rglob("*.c") if "_fn_" not in p.name)
    if max_files:
        files = files[:max_files]
    for path in files:
        tu = repo_rel(path)
        current = ""
        depth = 0
        pending = ""
        pending_start = 0
        block_comment = False
        try:
            lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        except OSError:
            continue
        for lineno, raw in enumerate(lines, 1):
            line, block_comment = strip_comments_and_strings(raw, block_comment)
            if current:
                for token in CALL_TOKEN_RE.findall(line):
                    if token == current or token in C_KEYWORDS or token not in known_symbols:
                        continue
                    callsite = f"{tu}:{lineno}"
                    key = (current, token, "src-c")
                    if key not in edges:
                        edges[key] = {
                            "caller": current,
                            "callee": token,
                            "caller_tu": tu,
                            "callee_tu": function_tus.get(token, ""),
                            "callsite": callsite,
                            "source": "src-c",
                            "confidence": "high",
                            "evidence": "direct C call token",
                            "count": 1,
                        }
                    else:
                        edges[key]["count"] = int(edges[key].get("count", 1)) + 1
                depth += line.count("{") - line.count("}")
                if depth <= 0:
                    current = ""
                    depth = 0
                continue

            if pending:
                pending += " " + line.strip()
                if ";" in line and ("{" not in line or line.index(";") < line.index("{")):
                    pending = ""
                    pending_start = 0
                    continue
                if "{" in line:
                    name = definition_name(pending, known_symbols)
                    if name:
                        current = name
                        source_defs[name] = tu
                        function_tus.setdefault(name, tu)
                        depth = pending.count("{") - pending.count("}")
                    pending = ""
                    pending_start = 0
                continue

            if "(" not in line:
                continue
            if "{" in line:
                name = definition_name(line, known_symbols)
                if name:
                    current = name
                    source_defs[name] = tu
                    function_tus.setdefault(name, tu)
                    depth = line.count("{") - line.count("}")
                continue
            name = definition_name(line, known_symbols)
            if name:
                pending = line.strip()
                pending_start = lineno
                if pending_start == 0:
                    pending_start = lineno

    for edge in edges.values():
        count = int(edge.pop("count", 1))
        if count > 1:
            edge["evidence"] = f"direct C call token ({count} callsites)"
    return list(edges.values()), source_defs


def mine_pcode_calls(addr_symbols: dict[int, str],
                     function_tus: dict[str, str]) -> list[dict[str, object]]:
    edges: dict[tuple[str, str, str], dict[str, object]] = {}
    if not BUNDLES_DIR.exists():
        return []
    for path in sorted(BUNDLES_DIR.glob("fn_*.md")):
        caller = path.stem
        try:
            lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        except OSError:
            continue
        for lineno, line in enumerate(lines, 1):
            for match in PCODE_CALL_RE.finditer(line):
                callee = addr_symbols.get(int(match.group(1), 16))
                if not callee or callee == caller:
                    continue
                key = (caller, callee, "ghidra-pcode")
                if key not in edges:
                    edges[key] = {
                        "caller": caller,
                        "callee": callee,
                        "caller_tu": function_tus.get(caller, ""),
                        "callee_tu": function_tus.get(callee, ""),
                        "callsite": f"{repo_rel(path)}:{lineno}",
                        "source": "ghidra-pcode",
                        "confidence": "medium",
                        "evidence": "direct CALL in Ghidra p-code bundle",
                        "count": 1,
                    }
                else:
                    edges[key]["count"] = int(edges[key].get("count", 1)) + 1
    for edge in edges.values():
        count = int(edge.pop("count", 1))
        if count > 1:
            edge["evidence"] = f"direct CALL in Ghidra p-code bundle ({count} callsites)"
    return list(edges.values())


def parse_symbolmap_file(path: Path) -> list[tuple[str, str, str]]:
    rows: list[tuple[str, str, str]] = []
    if not path.exists():
        return rows
    for raw in read_text(path).splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        match = re.match(r"^(fn_[0-9A-Fa-f]{8})\s*->\s*([A-Za-z_][A-Za-z0-9_]*)\s*(?://\s*(.*))?$", line)
        if match:
            rows.append((match.group(1), match.group(2), (match.group(3) or "").strip()))
    return rows


def confidence_for_score(score: float) -> str:
    if score >= 0.75:
        return "high"
    if score >= 0.55:
        return "medium"
    return "low"


def tag_prefix(name: str) -> str | None:
    if not name or name.startswith(("fn_", "lbl_", "@")):
        return None
    clean = name.strip("_")
    if "_" in clean:
        first = clean.split("_", 1)[0]
        return first if len(first) >= 2 else None
    match = re.match(r"^([A-Z][A-Za-z0-9]{2,}?)(?:[A-Z][a-z]|$)", clean)
    if match:
        return match.group(1)
    return None


def insert_name_evidence(con: sqlite3.Connection, known_symbols: set[str],
                         edges: list[dict[str, object]], now: float) -> tuple[int, int]:
    n_evidence = 0
    n_tags = 0
    function_tus = {
        str(row["addr"]): normalize_tu(row["tu"])
        for row in con.execute("SELECT addr, tu FROM functions WHERE tu IS NOT NULL")
        if row["addr"] and row["tu"]
    }

    # Current non-address names are already accepted evidence.
    for symbol in sorted(known_symbols):
        if symbol.startswith("fn_"):
            continue
        tag = tag_prefix(symbol)
        con.execute(
            """INSERT OR REPLACE INTO name_evidence
               (symbol,candidate,source,confidence,score,evidence,updated_at)
               VALUES(?,?,?,?,?,?,?)""",
            (symbol, symbol, "current-symbol", "high", 0.95,
             "current symbols.txt function name", now),
        )
        n_evidence += 1
        if tag:
            con.execute(
                """INSERT OR REPLACE INTO function_tags
                   (symbol,tag,kind,confidence,score,evidence,updated_at)
                   VALUES(?,?,?,?,?,?,?)""",
                (symbol, tag, "self", "high", 0.95, "current function name prefix", now),
            )
            n_tags += 1

    # File/class membership is weak naming evidence, but it gives isolated
    # functions a context node until call-neighbor evidence accumulates.
    for symbol, tu in function_tus.items():
        if not tu:
            continue
        stem = Path(tu).stem
        if not stem:
            continue
        con.execute(
            """INSERT OR REPLACE INTO function_tags
               (symbol,tag,kind,confidence,score,evidence,updated_at)
               VALUES(?,?,?,?,?,?,?)""",
            (symbol, stem, "tu", "low", 0.40, f"owning translation unit: {tu}", now),
        )
        n_tags += 1
        if symbol.startswith("fn_"):
            con.execute(
                """INSERT OR REPLACE INTO name_evidence
                   (symbol,candidate,source,confidence,score,evidence,updated_at)
                   VALUES(?,?,?,?,?,?,?)""",
                (symbol, f"{stem}_*", "tu", "low", 0.40, f"owning translation unit: {tu}", now),
            )
            n_evidence += 1

    named_by_tu: dict[str, Counter[str]] = defaultdict(Counter)
    for symbol, tu in function_tus.items():
        tag = tag_prefix(symbol)
        if tag and tu:
            named_by_tu[tu][tag] += 1

    for symbol, tu in function_tus.items():
        if not symbol.startswith("fn_") or not tu:
            continue
        for tag, count in named_by_tu.get(tu, Counter()).most_common(3):
            score = min(0.85, 0.36 + count * 0.08)
            confidence = confidence_for_score(score)
            evidence = f"{count} named function(s) in owning TU {tu}"
            con.execute(
                """INSERT OR REPLACE INTO function_tags
                   (symbol,tag,kind,confidence,score,evidence,updated_at)
                   VALUES(?,?,?,?,?,?,?)""",
                (symbol, tag, "tu-neighbor", confidence, score, evidence, now),
            )
            n_tags += 1
            con.execute(
                """INSERT OR REPLACE INTO name_evidence
                   (symbol,candidate,source,confidence,score,evidence,updated_at)
                   VALUES(?,?,?,?,?,?,?)""",
                (symbol, f"{tag}_*", "tu-neighbor", confidence, score, evidence, now),
            )
            n_evidence += 1

    for source_name, path, confidence, score in (
        ("symbolmap-proposed", SM_DIR / "proposed_symbols.txt", "high", 0.90),
        ("symbolmap-applied", SM_DIR / "applied_symbols.txt", "high", 0.98),
    ):
        for old, new, provenance in parse_symbolmap_file(path):
            con.execute(
                """INSERT OR REPLACE INTO name_evidence
                   (symbol,candidate,source,confidence,score,evidence,updated_at)
                   VALUES(?,?,?,?,?,?,?)""",
                (old, new, source_name, confidence, score, provenance, now),
            )
            n_evidence += 1
            tag = tag_prefix(new)
            if tag:
                con.execute(
                    """INSERT OR REPLACE INTO function_tags
                       (symbol,tag,kind,confidence,score,evidence,updated_at)
                       VALUES(?,?,?,?,?,?,?)""",
                    (old, tag, "symbolmap", confidence, score, f"{source_name}: {new}", now),
                )
                n_tags += 1

    neighbor_tags: dict[str, Counter[str]] = defaultdict(Counter)
    neighbor_sources: dict[tuple[str, str], set[str]] = defaultdict(set)
    for edge in edges:
        caller = str(edge["caller"])
        callee = str(edge["callee"])
        src = str(edge.get("source") or "")
        for symbol, other in ((caller, callee), (callee, caller)):
            tag = tag_prefix(other)
            if not tag:
                continue
            neighbor_tags[symbol][tag] += 1
            neighbor_sources[(symbol, tag)].add(src)

    for symbol, counts in neighbor_tags.items():
        for tag, count in counts.items():
            diversity = len(neighbor_sources[(symbol, tag)])
            score = min(0.95, 0.35 + count * 0.12 + diversity * 0.10)
            confidence = confidence_for_score(score)
            evidence = f"{count} named call-neighbor(s), {diversity} evidence source(s)"
            con.execute(
                """INSERT OR REPLACE INTO function_tags
                   (symbol,tag,kind,confidence,score,evidence,updated_at)
                   VALUES(?,?,?,?,?,?,?)""",
                (symbol, tag, "calltag", confidence, score, evidence, now),
            )
            n_tags += 1
            if symbol.startswith("fn_"):
                con.execute(
                    """INSERT OR REPLACE INTO name_evidence
                       (symbol,candidate,source,confidence,score,evidence,updated_at)
                       VALUES(?,?,?,?,?,?,?)""",
                    (symbol, f"{tag}_*", "calltag", confidence, score, evidence, now),
                )
                n_evidence += 1
    return n_evidence, n_tags


def replace_mined_rows(con: sqlite3.Connection) -> None:
    con.execute("DELETE FROM function_calls WHERE source IN ('src-c','ghidra-pcode')")
    con.execute(
        "DELETE FROM name_evidence WHERE source IN "
        "('current-symbol','symbolmap-proposed','symbolmap-applied','calltag','tu','tu-neighbor')"
    )
    con.execute("DELETE FROM function_tags WHERE kind IN ('self','symbolmap','calltag','tu','tu-neighbor')")


def insert_edges(con: sqlite3.Connection, edges: list[dict[str, object]], now: float) -> int:
    n = 0
    for edge in edges:
        con.execute(
            """INSERT OR REPLACE INTO function_calls
               (caller,callee,caller_tu,callee_tu,callsite,source,confidence,evidence,updated_at)
               VALUES(?,?,?,?,?,?,?,?,?)""",
            (
                edge.get("caller"), edge.get("callee"), edge.get("caller_tu"),
                edge.get("callee_tu"), edge.get("callsite"), edge.get("source"),
                edge.get("confidence"), edge.get("evidence"), now,
            ),
        )
        n += 1
    return n


def cmd_build(args: argparse.Namespace) -> None:
    con = connect(create=args.create_db)
    known_symbols, addr_symbols, _addr_by_name = load_symbols()
    if not known_symbols:
        sys.exit(f"no function symbols loaded from {SYMBOLS}")
    function_tus = load_function_tus(con)
    now = time.time()
    replace_mined_rows(con)

    edges: list[dict[str, object]] = []
    source_defs: dict[str, str] = {}
    if not args.no_source:
        src_edges, source_defs = mine_source_calls(known_symbols, function_tus, args.max_source_files)
        edges.extend(src_edges)
        for symbol, tu in source_defs.items():
            con.execute(
                """INSERT OR IGNORE INTO functions(addr,tu,status,updated_at)
                   VALUES(?,?,?,?)""",
                (symbol, tu, "UNKNOWN", now),
            )
    if not args.no_bundles:
        edges.extend(mine_pcode_calls(addr_symbols, function_tus))
    edge_count = insert_edges(con, edges, now)
    evidence_count, tag_count = insert_name_evidence(con, known_symbols, edges, now)
    con.execute("INSERT OR REPLACE INTO meta(k,v) VALUES('last_callgraph_build',?)", (str(now),))
    con.commit()
    payload = {
        "edges": edge_count,
        "name_evidence": evidence_count,
        "function_tags": tag_count,
        "source_defs": len(source_defs),
    }
    if args.json:
        print(json.dumps(payload, indent=2, sort_keys=True))
    else:
        print(
            f"callgraph: {edge_count} calls, {tag_count} tags, "
            f"{evidence_count} name-evidence rows"
        )


def rows_to_dicts(rows: list[sqlite3.Row]) -> list[dict[str, object]]:
    return [dict(row) for row in rows]


def cmd_context(args: argparse.Namespace) -> None:
    con = connect()
    fn = args.symbol
    function = con.execute("SELECT * FROM functions WHERE addr=?", (fn,)).fetchone()
    callees = rows_to_dicts(list(con.execute(
        """SELECT callee AS fn, callee_tu AS tu, confidence, source, evidence, callsite
           FROM function_calls WHERE caller=?
           ORDER BY confidence DESC, fn LIMIT ?""",
        (fn, args.limit),
    )))
    callers = rows_to_dicts(list(con.execute(
        """SELECT caller AS fn, caller_tu AS tu, confidence, source, evidence, callsite
           FROM function_calls WHERE callee=?
           ORDER BY confidence DESC, fn LIMIT ?""",
        (fn, args.limit),
    )))
    tags = rows_to_dicts(list(con.execute(
        """SELECT tag, kind, confidence, ROUND(score,2) AS score, evidence
           FROM function_tags WHERE symbol=?
           ORDER BY score DESC, tag LIMIT ?""",
        (fn, args.limit),
    )))
    evidence = rows_to_dicts(list(con.execute(
        """SELECT candidate, source, confidence, ROUND(score,2) AS score, evidence
           FROM name_evidence WHERE symbol=?
           ORDER BY score DESC, candidate LIMIT ?""",
        (fn, args.limit),
    )))
    payload = {
        "function": dict(function) if function else {"addr": fn},
        "callees": callees,
        "callers": callers,
        "tags": tags,
        "name_evidence": evidence,
    }
    if args.markdown:
        print(f"# Relationship context for `{fn}`\n")
        if function:
            print(f"- TU: `{function['tu'] or ''}`")
            print(f"- Byte match: `{function['byte_pct']}`")
            print(f"- Status: `{function['status'] or ''}`")
        print("\n## Calltags")
        for row in tags or [{"tag": "(none)", "confidence": "", "score": "", "evidence": ""}]:
            print(f"- `{row['tag']}` {row.get('confidence','')} score={row.get('score','')} - {row.get('evidence','')}")
        print("\n## Name Evidence")
        for row in evidence or [{"candidate": "(none)", "source": "", "confidence": "", "score": "", "evidence": ""}]:
            print(f"- `{row['candidate']}` from {row.get('source','')} {row.get('confidence','')} score={row.get('score','')} - {row.get('evidence','')}")
        print("\n## Calls")
        for row in callees or [{"fn": "(none)", "tu": "", "confidence": "", "source": "", "evidence": ""}]:
            print(f"- `{fn}` -> `{row['fn']}` ({row.get('tu','')}) {row.get('confidence','')} via {row.get('source','')}: {row.get('evidence','')}")
        print("\n## Called By")
        for row in callers or [{"fn": "(none)", "tu": "", "confidence": "", "source": "", "evidence": ""}]:
            print(f"- `{row['fn']}` -> `{fn}` ({row.get('tu','')}) {row.get('confidence','')} via {row.get('source','')}: {row.get('evidence','')}")
    else:
        print(json.dumps(payload, indent=2, sort_keys=True))


def cmd_stats(_args: argparse.Namespace) -> None:
    con = connect()
    for table in ("function_calls", "name_evidence", "function_tags"):
        print(f"{table:16s} {con.execute(f'SELECT COUNT(*) FROM {table}').fetchone()[0]}")


def main() -> None:
    parser = argparse.ArgumentParser(description="mine KG call relationships and name evidence")
    sub = parser.add_subparsers(dest="cmd", required=True)

    build = sub.add_parser("build")
    build.add_argument("--create-db", action="store_true", help="allow creating kg.db if missing")
    build.add_argument("--no-source", action="store_true", help="skip src/**/*.c call mining")
    build.add_argument("--no-bundles", action="store_true", help="skip kg/bundles p-code call mining")
    build.add_argument("--max-source-files", type=int, default=None, help="debug throttle")
    build.add_argument("--json", action="store_true")
    build.set_defaults(handler=cmd_build)

    ctx = sub.add_parser("context")
    ctx.add_argument("symbol")
    ctx.add_argument("--limit", type=int, default=40)
    ctx.add_argument("--markdown", action="store_true")
    ctx.set_defaults(handler=cmd_context)

    stats = sub.add_parser("stats")
    stats.set_defaults(handler=cmd_stats)

    args = parser.parse_args()
    args.handler(args)


if __name__ == "__main__":
    main()
