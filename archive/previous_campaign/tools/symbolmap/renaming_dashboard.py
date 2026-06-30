#!/usr/bin/env python3
"""Live dashboard for symbolmap renaming targets."""

from __future__ import annotations

import argparse
import io
import json
import os
import re
import shutil
import subprocess
import sys
import threading
import time
import uuid
import zipfile
from collections import Counter
from datetime import datetime, timedelta, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlparse


ROOT = Path(__file__).resolve().parents[2]
SM_DIR = ROOT / "config" / "GC6E01" / "symbolmap"
SYMBOLS = ROOT / "config" / "GC6E01" / "symbols.txt"
FUNC_TU_MAP = ROOT / "config" / "GC6E01" / "func_tu_map.json"
DECOMP_REPORT = ROOT / "report.json"
DECOMP_STATUS_LOG = ROOT / "tools" / "decomp_work" / "coordination" / "status.md"
# --- v9: reader / wall / agent / token / lockout data sources --------------------
DECOMP_WORK = ROOT / "tools" / "decomp_work"
COORD_DIR = DECOMP_WORK / "coordination"
PROGRESS2 = DECOMP_WORK / "progress2.py"
SCRATCH_DIR = DECOMP_WORK / "scratch"  # band_<tag>.c / .src live here (#5 lease fn derivation)
WALLS_MD = ROOT / "WALLS.md"
EQUIVALENT_TXT = DECOMP_WORK / "equivalent.txt"
CS_WALLS_JSON = ROOT / "build" / "cs_walls.json"
AGENT_STATUS_TXT = COORD_DIR / "agent_status.txt"
CLAIMS_JSON = COORD_DIR / "claims.json"
TASKS_JSON = COORD_DIR / "tasks.json"
CRACK_QUEUE_JSON = COORD_DIR / "crack_queue.json"
HARD_TARGETS_MD = ROOT / "docs" / "hardest_regalloc_functions.md"
TMUX_CONTROL = DECOMP_WORK / "tmux_control"
LOCKS_DB = Path(os.environ.get("DECOMP_LOCKS_DB", COORD_DIR / "locks.db"))
AGENT_TOKENS_JSON = ROOT / ".omc" / "agent_tokens.json"
AGENT_LIMITS_JSON = DECOMP_WORK / "agent_limits.json"
PROXY_USAGE_LIMITS_JSON = ROOT / "tools" / "llm-proxy" / "usage_limits.json"
OPENCODE_STORAGE = Path(
    os.environ.get(
        "OPENCODE_STORAGE",
        str(Path.home() / ".local" / "share" / "opencode" / "storage"),
    )
)
HISTORY_FILE = ROOT / ".omx" / "state" / "renaming_dashboard_history.json"
UNIT_HISTORY_FILE = ROOT / ".omx" / "state" / "renaming_dashboard_unit_history.json"
FN_HISTORY_FILE = ROOT / ".omx" / "state" / "renaming_dashboard_fn_history.json"
# Per-function token ledger (Phase 2): pane_io appends one JSONL row per completed
# task to FN_TOKEN_LEDGER; update_fn_token_history() folds new rows into a per-fn
# time-series (offset-tracked so rows are ingested exactly once).
FN_TOKEN_HISTORY_FILE = ROOT / ".omx" / "state" / "renaming_dashboard_fn_token_history.json"
FN_TOKEN_LEDGER = ROOT / "build" / "hb" / "token_by_fn.jsonl"
FN_TOKEN_HISTORY_CAP = 200
HISTORY_INTERVAL_SECONDS = 60
# Ring cap for the global match-progress history. Raised from 500 -> 2000 so the
# months-long git backfill (one row per report.json commit) is not evicted.
HISTORY_CAP = 2000
UNIT_HISTORY_CAP = 300
FN_HISTORY_CAP = 200
STATE_CACHE_TTL_SECONDS = 1.8
DASHBOARD_VERSION = 15
# --- v10: token-history collector sources ----------------------------------------
CLAUDE_PROJECT_DIR = (
    Path.home()
    / ".claude"
    / "projects"
    / "C--Users-douglaswhittingham-pkmn-colosseum"
)
# GLM runs Claude Code under an isolated config dir (~/.claude-glm) via the proxy,
# so its session journals (same JSONL `usage` schema) are NOT under ~/.claude — they
# must be collected separately to attribute GLM tokens.
GLM_PROJECT_DIR = (
    Path.home()
    / ".claude-glm"
    / "projects"
    / "C--Users-douglaswhittingham-pkmn-colosseum"
)
CODEX_HISTORY = Path.home() / ".codex" / "history.jsonl"
CODEX_SESSIONS_DIR = Path.home() / ".codex" / "sessions"
TOKEN_HISTORY_FILE = DECOMP_WORK / "token_history.json"
KG_DB = DECOMP_WORK / "kg" / "kg.db"
# Artifacts served privately over Tailscale (the user's own ROM-derived inputs).
MAIN_DOL = ROOT / "orig" / "GC6E01" / "sys" / "main.dol"
TARGET_OBJ_DIR = ROOT / "build" / "GC6E01" / "obj"

# In-process TTL cache for build_state(). The full rebuild runs git x4, reparses
# the ~646KB report.json, and shells out to rg per Proposed/Needs-wiring row, so
# it can take ~19s. The 5s front-end auto-refresh would otherwise stack rebuilds.
_STATE_CACHE: dict[str, object] = {"value": None, "expires": 0.0}

MAP_RE = re.compile(
    r"^(fn_[0-9A-Fa-f]{8})\s*->\s*([A-Za-z_.$][\w.$:@?]*)\s*//\s*(.*?)\s*$"
)
LEAD_RE = re.compile(
    r"^\|\s*`?(fn_[0-9A-Fa-f]{8})`?\s*\|\s*`?([^|`]+?)`?\s*\|\s*([^|]+?)\s*\|\s*([^|]+?)\s*\|"
)
SYMBOL_RE = re.compile(
    r"^([A-Za-z_.$][\w.$:@?]*)\s*=\s*(\.\w+):(0x[0-9A-Fa-f]+);"
    r"\s*//\s*type:(\w+)\s*size:(0x[0-9A-Fa-f]+)(.*)$"
)
FUNCTION_NAME_RE = re.compile(r"^[A-Za-z_.$][\w.$:@?]*$")
CRACK_LANES = [
    {
        "id": "codex-gpt55-high",
        "label": "Codex 5.5 high",
        "provider": "codex",
        "model": "gpt-5.5",
        "launch": "tmux-codex",
        "note": "interactive Codex TUI lane",
    },
    {
        "id": "claude-opus",
        "label": "Claude Opus",
        "provider": "claude-code",
        "model": "opus",
        "launch": "queue",
        "note": "queue until Claude launcher is connected",
    },
    {
        "id": "glm-52",
        "label": "GLM 5.2",
        "provider": "glm",
        "model": "glm-5.2",
        "launch": "queue",
        "note": "queue until GLM launcher is connected",
    },
    {
        "id": "opencode-deepseek-v4-flash",
        "label": "OpenCode DeepSeek V4 Flash",
        "provider": "opencode",
        "model": "deepseek-v4-flash",
        "launch": "queue",
        "note": "queue for low-cost OpenCode/DeepSeek drain",
    },
]
CRACK_STRATEGIES = [
    {
        "id": "contenders",
        "label": "Contenders",
        "note": "independent attempts; best default for one function",
    },
    {
        "id": "split",
        "label": "Split giant",
        "note": "coordinated slice plan; use only for structural giants",
    },
    {
        "id": "repair",
        "label": "Repair best",
        "note": "start from current/previous near-match C and iterate",
    },
]


def read_text(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def load_simple_map(path: Path) -> dict[str, dict[str, str]]:
    rows: dict[str, dict[str, str]] = {}
    for line in read_text(path).splitlines():
        match = MAP_RE.match(line.strip())
        if not match:
            continue
        fn, name, provenance = match.groups()
        rows[fn] = {"fn": fn, "name": name, "provenance": provenance.strip()}
    return rows


def load_leads(path: Path) -> dict[str, dict[str, str]]:
    rows: dict[str, dict[str, str]] = {}
    for line in read_text(path).splitlines():
        match = LEAD_RE.match(line.strip())
        if not match:
            continue
        fn, name, provenance, header = match.groups()
        if fn == "addr (fn_)":
            continue
        rows[fn] = {
            "fn": fn,
            "name": name.strip(),
            "provenance": provenance.strip(),
            "header": header.strip(),
        }
    return rows


def load_symbols(path: Path) -> tuple[dict[str, dict[str, str]], dict[str, dict[str, str]]]:
    by_name: dict[str, dict[str, str]] = {}
    by_addr: dict[str, dict[str, str]] = {}
    for line in read_text(path).splitlines():
        match = SYMBOL_RE.match(line.strip())
        if not match:
            continue
        name, section, addr, kind, size, comment = match.groups()
        row = {
            "name": name,
            "section": section,
            "addr": addr.lower(),
            "kind": kind,
            "size": size,
            "comment": comment.strip(),
        }
        by_name[name] = row
        by_addr[addr.lower()] = row
    return by_name, by_addr


def load_tu_map(path: Path) -> dict[str, dict[str, str]]:
    if not path.exists():
        return {}
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return {}
    return data if isinstance(data, dict) else {}


def fn_to_addr(fn: str) -> str:
    return "0x" + fn.removeprefix("fn_").lower()


def git_value(args: list[str]) -> str:
    try:
        proc = subprocess.run(
            ["git", *args],
            cwd=ROOT,
            check=False,
            capture_output=True,
            text=True,
            timeout=3,
        )
    except (OSError, subprocess.TimeoutExpired):
        return ""
    if proc.returncode != 0:
        return ""
    return proc.stdout.strip()


def recent_commits(limit: int = 10) -> list[dict[str, object]]:
    text = git_value(
        [
            "log",
            f"-n{limit}",
            "--date=format:%b %d, %H:%M",
            "--pretty=format:%h%x09%cd%x09%ct%x09%s",
        ]
    )
    rows: list[dict[str, object]] = []
    for line in text.splitlines():
        parts = line.split("\t", 3)
        if len(parts) != 4:
            continue
        sha, when, ct, subject = parts
        try:
            unix = int(ct)
        except ValueError:
            unix = 0
        # `when` retained for backwards-compat; `unix` lets the front-end format
        # the commit time in HST via hstTime().
        rows.append({"sha": sha, "when": when, "unix": unix, "subject": subject})
    return rows


def recent_commit_attempts(limit: int = 40) -> list[dict[str, object]]:
    """Derive attempt-log entries from recent git commits that touched src/*.c.

    Codex commits its per-file decomp work to git (not status.md), so without
    this its progress (e.g. "Advance menu_middle matching" on menu_middle.c)
    never shows up in the Attempt Log. We emit one entry per changed .c file
    (capped per commit) in the same shape load_attempt_log() produces so the
    front-end's reverse-sort and relatedAttempts(unit) filtering both work.
    """
    text = git_value(
        [
            "log",
            f"-n{limit}",
            "--name-only",
            "--pretty=format:%x01%H%x09%ct%x09%s",
            "--",
            "src",
        ]
    )
    rows: list[dict[str, object]] = []
    # Records are separated by the \x01 we injected at the start of each header.
    for record in text.split("\x01"):
        record = record.strip("\n")
        if not record:
            continue
        lines = record.split("\n")
        header = lines[0].split("\t", 2)
        if len(header) != 3:
            continue
        _sha, ct, subject = header
        try:
            unix = int(ct)
        except ValueError:
            continue
        iso = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(unix))
        emitted = 0
        for path in lines[1:]:
            path = path.strip()
            if not path or not path.endswith(".c"):
                continue
            if emitted >= 3:
                break
            emitted += 1
            base = path.replace("\\", "/").rsplit("/", 1)[-1]
            lower_subject = subject.lower()
            kind = "match" if (
                "byte-exact" in lower_subject
                or "byte-match" in lower_subject
                or "100%" in lower_subject
                or "match " in lower_subject
                or "matched" in lower_subject
                or re.search(r"\+\d+\s+(?:byte|fn|function|match)", lower_subject)
            ) else "commit"
            rows.append(
                {
                    "timestamp": iso,
                    "unix": unix,
                    "agent": "git",
                    "kind": kind,
                    "function": "",
                    "file": base,
                    "percent": None,
                    "message": subject,
                }
            )
    return rows


def source_label(source: str) -> str:
    if not source:
        return "unknown"
    path = source.replace("\\", "/")
    name = path.rsplit("/", 1)[-1]
    return name.removesuffix(".c")


def source_refs(symbol: str) -> int:
    try:
        proc = subprocess.run(
            [
                "rg",
                "-n",
                "--fixed-strings",
                symbol,
                "src",
                "include",
                "config/GC6E01/symbols.txt",
                "config/GC6E01/symbols.build.txt",
                "config/GC6E01/func_tu_map.json",
                "config/GC6E01/symdb.json",
                "--glob",
                "!*.inc",
            ],
            cwd=ROOT,
            check=False,
            capture_output=True,
            text=True,
            timeout=3,
        )
    except (OSError, subprocess.TimeoutExpired):
        return -1
    if proc.returncode not in (0, 1):
        return -1
    return len([line for line in proc.stdout.splitlines() if line.strip()])


def int_value(value: object) -> int:
    try:
        return int(str(value), 0)
    except (TypeError, ValueError):
        return 0


def pct(done: int, total: int) -> float:
    if total <= 0:
        return 0.0
    return round(done * 100.0 / total, 1)


def float_pct(value: object) -> float:
    try:
        return round(float(value), 2)
    except (TypeError, ValueError):
        return 0.0


def timestamp_unix(value: object) -> int:
    """Parse dashboard timestamps to unix seconds for stable merge sorting."""
    if isinstance(value, (int, float)):
        return int(value)
    if not isinstance(value, str) or not value:
        return 0
    text = value.strip()
    if text.isdigit():
        return int(text)
    # Most coordination rows are UTC ISO strings. Some historical rows are plain
    # local dashboard snapshots; keep those sortable best-effort.
    for fmt, utc in (
        ("%Y-%m-%dT%H:%M:%SZ", True),
        ("%Y-%m-%dT%H:%M:%S", True),
        ("%Y-%m-%d %H:%M:%S", False),
    ):
        try:
            parsed = datetime.strptime(text.split(".")[0].replace("Z", ""), fmt.replace("Z", ""))
        except ValueError:
            continue
        if utc:
            return int(parsed.replace(tzinfo=timezone.utc).timestamp())
        return int(time.mktime(parsed.timetuple()))
    return 0


_HARD_TARGET_CACHE: dict[str, object] = {"mtime": None, "entries": None, "by_fn": None}


def _clean_md_cell(value: str) -> str:
    return re.sub(r"`([^`]*)`", r"\1", value).strip()


def _parse_size_value(value: str) -> int:
    text = _clean_md_cell(value).replace(",", "").strip()
    if not text or text.lower() == "n/a":
        return 0
    try:
        return int(text, 16) if text.lower().startswith("0x") else int(text)
    except ValueError:
        return 0


def _section_key(section: str) -> str:
    lower = section.lower()
    if "confirmed" in lower:
        return "wall"
    if "asm" in lower:
        return "asm"
    if "real-c" in lower or "near" in lower:
        return "near"
    if "structural" in lower:
        return "giant"
    return re.sub(r"[^a-z0-9]+", "-", lower).strip("-") or "other"


def load_hard_targets() -> dict[str, object]:
    """Parse docs/hardest_regalloc_functions.md into dashboard metadata."""
    if not HARD_TARGETS_MD.exists():
        return {"available": False, "entries": [], "by_fn": {}, "counts": {}, "source": str(HARD_TARGETS_MD)}
    try:
        mtime = HARD_TARGETS_MD.stat().st_mtime
    except OSError:
        mtime = 0.0
    if _HARD_TARGET_CACHE.get("mtime") == mtime and _HARD_TARGET_CACHE.get("entries") is not None:
        return {
            "available": True,
            "entries": _HARD_TARGET_CACHE["entries"],
            "by_fn": _HARD_TARGET_CACHE["by_fn"],
            "counts": dict(Counter(str(e.get("section_key", "other")) for e in _HARD_TARGET_CACHE["entries"])),  # type: ignore[arg-type]
            "source": str(HARD_TARGETS_MD),
        }

    entries: list[dict[str, object]] = []
    section = ""
    for raw in read_text(HARD_TARGETS_MD).splitlines():
        line = raw.strip()
        if line.startswith("## "):
            section = line.removeprefix("## ").strip()
            continue
        if not line.startswith("|") or "---" in line or "File / TU" in line:
            continue
        cells = [_clean_md_cell(c) for c in line.strip("|").split("|")]
        if len(cells) < 5 or not section:
            continue
        section_key = _section_key(section)
        rank = None
        if cells[0].strip().isdigit() and len(cells) >= 6:
            rank = int(cells[0].strip())
            file_cell, fn, size_cell, match_cell, note_cell = cells[1], cells[2], cells[3], cells[4], cells[5]
        else:
            file_cell, fn, size_cell, match_cell, note_cell = cells[0], cells[1], cells[2], cells[3], cells[4]
        file_match = re.search(r"(src/[^\s|]+\.c)", file_cell.replace("\\", "/"))
        source = file_match.group(1) if file_match else ""
        match_pct = None
        pct_match = re.search(r"([0-9]+(?:\.[0-9]+)?)", match_cell)
        if pct_match:
            match_pct = float_pct(pct_match.group(1))
        entries.append({
            "section": section,
            "section_key": section_key,
            "rank": rank,
            "source": source,
            "function": fn.strip(),
            "size": _parse_size_value(size_cell),
            "match_pct": match_pct,
            "note": note_cell.strip(),
        })

    def entry_sort(entry: dict[str, object]) -> tuple[int, int, int, str]:
        section_weight = {"wall": 0, "asm": 1, "near": 2, "giant": 3}.get(str(entry.get("section_key")), 9)
        rank_value = int_value(entry.get("rank")) or 9999
        size_value = int_value(entry.get("size"))
        return (section_weight, rank_value, -size_value, str(entry.get("function") or ""))

    entries.sort(key=entry_sort)
    by_fn: dict[str, dict[str, object]] = {}
    for entry in entries:
        fn = str(entry.get("function") or "")
        if fn and fn not in by_fn:
            by_fn[fn] = entry
    counts = Counter(str(e.get("section_key") or "other") for e in entries)
    _HARD_TARGET_CACHE["mtime"] = mtime
    _HARD_TARGET_CACHE["entries"] = entries
    _HARD_TARGET_CACHE["by_fn"] = by_fn
    return {"available": True, "entries": entries, "by_fn": by_fn, "counts": dict(counts), "source": str(HARD_TARGETS_MD)}


def hard_target_for(fn: str) -> dict[str, object] | None:
    data = load_hard_targets()
    by_fn = data.get("by_fn") if isinstance(data, dict) else {}
    return by_fn.get(fn) if isinstance(by_fn, dict) else None


def match_status(value: object) -> str:
    score = float_pct(value)
    if score >= 99.95:
        return "matched"
    if score >= 90.0:
        return "near"
    if score > 0:
        return "partial"
    return "missing"


def classify_provenance(provenance: str) -> str:
    text = provenance.lower()
    if "xd" in text:
        return "XD port"
    if "string" in text:
        return "String evidence"
    if "structural" in text:
        return "Structural"
    return "Other"


def load_decomp_report(path: Path) -> dict[str, object]:
    empty = {
        "available": False,
        "fuzzy_pct": 0.0,
        "code_pct": 0.0,
        "functions_pct": 0.0,
        "matched_functions": 0,
        "total_functions": 0,
        "matched_code": 0,
        "total_code": 0,
        "complete_units": 0,
        "total_units": 0,
        "units": [],
        "source": str(path),
    }
    if not path.exists():
        return empty
    try:
        report = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return empty
    measures = report.get("measures", {})
    if not isinstance(measures, dict):
        return empty

    units = []
    for unit in report.get("units", []):
        if not isinstance(unit, dict):
            continue
        unit_measures = unit.get("measures", {})
        if not isinstance(unit_measures, dict):
            unit_measures = {}
        metadata = unit.get("metadata", {})
        if not isinstance(metadata, dict):
            metadata = {}
        total_functions = int_value(unit_measures.get("total_functions", 0))
        matched_functions = int_value(unit_measures.get("matched_functions", 0))
        functions = []
        status_counts: Counter[str] = Counter()
        for index, function in enumerate(unit.get("functions", [])):
            if not isinstance(function, dict):
                continue
            fn_pct = float_pct(function.get("fuzzy_match_percent", 0))
            status = match_status(fn_pct)
            status_counts[status] += 1
            functions.append(
                {
                    "index": index,
                    "name": function.get("name", ""),
                    "size": int_value(function.get("size", 0)),
                    "fuzzy_pct": fn_pct,
                    "status": status,
                }
            )
        functions.sort(
            key=lambda row: (
                float(row.get("fuzzy_pct", 0)) >= 99.95,
                -int(row.get("size", 0)),
                str(row.get("name", "")),
            )
        )
        units.append(
            {
                "name": unit.get("name", ""),
                "source": metadata.get("source_path", ""),
                "fuzzy_pct": float_pct(unit_measures.get("fuzzy_match_percent", 0)),
                "code_pct": float_pct(unit_measures.get("matched_code_percent", 0)),
                "functions_pct": float_pct(
                    unit_measures.get("matched_functions_percent", 0)
                ),
                "matched_functions": matched_functions,
                "total_functions": total_functions,
                "matched_code": int_value(unit_measures.get("matched_code", 0)),
                "total_code": int_value(unit_measures.get("total_code", 0)),
                "complete": bool(metadata.get("complete"))
                or float_pct(unit_measures.get("matched_functions_percent", 0)) >= 100.0,
                "functions": functions,
                "function_status": dict(status_counts),
            }
        )
    units.sort(
        key=lambda row: (
            -int(row.get("total_functions", 0)),
            str(row.get("name", "")),
        )
    )

    return {
        "available": True,
        "fuzzy_pct": float_pct(measures.get("fuzzy_match_percent", 0)),
        "code_pct": float_pct(measures.get("matched_code_percent", 0)),
        "functions_pct": float_pct(measures.get("matched_functions_percent", 0)),
        "matched_functions": int_value(measures.get("matched_functions", 0)),
        "total_functions": int_value(measures.get("total_functions", 0)),
        "matched_code": int_value(measures.get("matched_code", 0)),
        "total_code": int_value(measures.get("total_code", 0)),
        "complete_units": int_value(measures.get("complete_units", 0)),
        "total_units": int_value(measures.get("total_units", 0)),
        "units": units,
        "source": str(path),
        "updated_at": time.strftime(
            "%Y-%m-%d %H:%M:%S", time.localtime(path.stat().st_mtime)
        ),
    }


_CONVERSION_CACHE: dict[str, object] = {"value": None, "expires": 0.0}
_CONVERSION_CACHE_TTL = 60.0


def load_conversion_scan() -> dict[str, object]:
    """Fast progress2 scan axis: real C vs asm wrappers vs stubs."""
    now = time.monotonic()
    cached = _CONVERSION_CACHE.get("value")
    if cached is not None and now < float(_CONVERSION_CACHE.get("expires", 0)):
        return cached  # type: ignore[return-value]
    empty = {
        "available": False,
        "real_c": 0,
        "asm_wrappers": 0,
        "stubs": 0,
        "source_total": 0,
        "converted_pct": 0.0,
    }
    if not PROGRESS2.exists():
        return empty
    try:
        proc = subprocess.run(
            [sys.executable, str(PROGRESS2)],
            cwd=str(ROOT),
            check=False,
            capture_output=True,
            text=True,
            timeout=30,
        )
    except (OSError, subprocess.TimeoutExpired):
        return empty
    if proc.returncode != 0:
        return empty
    text = proc.stdout

    def grab(pattern: str) -> int:
        match = re.search(pattern, text)
        return int(match.group(1)) if match else 0

    real_c = grab(r"REAL_C \(decompiled\):\s+(\d+)")
    asm = grab(r"ASM-wrapper \(ROM-only\):\s+(\d+)")
    stubs = grab(r"STUB \(TODO/empty\):\s+(\d+)")
    total = grab(r"source functions total:\s+(\d+)")
    converted = 100.0 * real_c / total if total else 0.0
    payload = {
        "available": True,
        "real_c": real_c,
        "asm_wrappers": asm,
        "stubs": stubs,
        "source_total": total,
        "converted_pct": round(converted, 2),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }
    _CONVERSION_CACHE["value"] = payload
    _CONVERSION_CACHE["expires"] = now + _CONVERSION_CACHE_TTL
    return payload


def load_attempt_log(path: Path, limit: int = 1000) -> list[dict[str, object]]:
    rows: list[dict[str, object]] = []
    line_re = re.compile(
        r"^- \*\*(?P<timestamp>[^*]+)\*\*\s+`(?P<agent>[^`]+)`\s+(?P<message>.*)$"
    )
    fn_re = re.compile(r"fn_[0-9A-Fa-f]{8}")
    pct_re = re.compile(r"([0-9]+(?:\.[0-9]+)?)%")
    # `in foo.c` (older format) OR a bare TU path like `game/gs_render_util`
    # (the auto-report writer emits "<unit> a->b/c"; capture its trailing stem so
    # the front-end's per-unit `relatedAttempts(unit)` filter still matches it).
    file_re = re.compile(r"\bin\s+([^()\s]+\.c|\?\.c)")
    unit_re = re.compile(r"^([A-Za-z0-9_./-]+/)?([A-Za-z0-9_]+)\s+\d+->\d+/\d+")
    markers = (
        "MATCH!",
        "COMMIT",
        "REGRESSION",
        "Claimed",
        "Completed",
        "Decision",
        "Enqueued",
    )
    for line in read_text(path).splitlines():
        match = line_re.match(line.strip())
        if not match:
            continue
        raw_message = match.group("message").strip()
        # Strip the leading "- " markdown artifact the auto-report writer emits
        # ("- **ts** `report` - game/foo ...") so the message reads cleanly.
        raw_message = re.sub(r"^[-*]\s+", "", raw_message)
        message = raw_message
        for marker in markers:
            index = raw_message.find(marker)
            if index >= 0:
                message = raw_message[index:].strip()
                break
        agent = match.group("agent")
        kind = "note"
        upper = message.upper()
        if "REGRESSION" in upper:
            kind = "regression"
        elif "MATCH!" in upper:
            kind = "match"
        elif "COMMIT" in upper:
            kind = "commit"
        elif "CLAIMED" in upper:
            kind = "claim"
        elif agent in ("report", "auto-report"):
            kind = "report"
        fn_match = fn_re.search(message)
        pct_match = pct_re.search(message)
        file_match = file_re.search(message)
        file_attr = file_match.group(1) if file_match else ""
        # Attribute auto-report unit lines ("game/gs_render_util 9->10/21") to a
        # synthetic "<stem>.c" so relatedAttempts(unit) groups them per file.
        if not file_attr:
            unit_match = unit_re.match(message)
            if unit_match:
                file_attr = unit_match.group(2) + ".c"
        rows.append(
            {
                "timestamp": match.group("timestamp"),
                "unix": timestamp_unix(match.group("timestamp")),
                "agent": agent,
                "kind": kind,
                "function": fn_match.group(0) if fn_match else "",
                "file": file_attr,
                "percent": float_pct(pct_match.group(1)) if pct_match else None,
                "message": message,
            }
        )
    return rows[-limit:]


def attempt_sort_key(row: dict[str, object]) -> tuple[int, str, str]:
    unix = int_value(row.get("unix")) or timestamp_unix(row.get("timestamp"))
    return (unix, str(row.get("timestamp") or ""), str(row.get("message") or ""))


def merged_attempt_log(limit: int | None = 1000) -> list[dict[str, object]]:
    """Merge status.md and git-derived attempts newest-first.

    Returning a mixed or chronological concat is why old April rows could
    surface above newer match work in the Activity Log match filter.
    """
    status_limit = 100000 if limit is None else max(limit * 4, 1000)
    commit_limit = 240 if limit is None or limit > 1000 else 80
    merged = load_attempt_log(DECOMP_STATUS_LOG, limit=status_limit) + recent_commit_attempts(limit=commit_limit)
    deduped: dict[tuple[object, object, object, object, object], dict[str, object]] = {}
    for row in merged:
        row.setdefault("unix", timestamp_unix(row.get("timestamp")))
        key = (
            row.get("timestamp"),
            row.get("agent"),
            row.get("kind"),
            row.get("file"),
            row.get("message"),
        )
        deduped[key] = row
    rows = sorted(deduped.values(), key=attempt_sort_key, reverse=True)
    if limit is not None and limit > 0:
        rows = rows[:limit]
    return rows


def load_history() -> list[dict[str, object]]:
    if not HISTORY_FILE.exists():
        return []
    try:
        data = json.loads(HISTORY_FILE.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return []
    if not isinstance(data, list):
        return []
    return [row for row in data if isinstance(row, dict)]


# The %-keys the commit-history view tracks. A commit is plotted only when one of
# these actually moves, so the line steps at real milestones, not every minute.
_HISTORY_PCT_KEYS = (
    "decomp_code_pct",
    "decomp_fuzzy_pct",
    "decomp_functions_pct",
    "c_converted_pct",
)


def collapse_history_to_commits(
    history: list[dict[str, object]] | None = None,
) -> list[dict[str, object]]:
    """Collapse the per-minute history ring into one point per COMMIT.

    The raw history (`load_history`) is time-sampled roughly every minute, so the
    "match progress over time" line is noisy and the x-axis isn't tied to real
    checkpoints. Each history row already carries the `head` (commit) it was taken
    at, so we:

      1. keep, for each consecutive run of the same `head`, the LAST sample — that
         row holds the final measures recorded for that commit (the within-commit
         minute-samples collapse to a single milestone point);
      2. drop any commit whose tracked %-values are identical to the previously
         kept commit, so a no-op commit (or a re-measure that didn't move the
         numbers) doesn't add a flat step.

    The returned rows keep their real `unix`/`timestamp`, so the chart still plots
    against wall-clock time — it just steps at commits instead of at minutes.
    """
    rows = history if history is not None else load_history()
    rows = [r for r in rows if isinstance(r, dict)]
    if not rows:
        return []

    # 1) collapse consecutive same-head runs to that run's last (newest) sample.
    per_commit: list[dict[str, object]] = []
    for row in rows:
        head = str(row.get("head") or "")
        if per_commit and str(per_commit[-1].get("head") or "") == head and head:
            per_commit[-1] = row  # newer sample of the same commit wins
        else:
            per_commit.append(row)

    # 2) drop commits whose tracked %s match the previously kept one.
    def pct_sig(row: dict[str, object]) -> tuple:
        return tuple(round(float_pct(row.get(k, 0)), 4) for k in _HISTORY_PCT_KEYS)

    collapsed: list[dict[str, object]] = []
    last_sig: tuple | None = None
    for row in per_commit:
        sig = pct_sig(row)
        if last_sig is not None and sig == last_sig:
            continue
        collapsed.append(row)
        last_sig = sig
    return collapsed


def write_history(history: list[dict[str, object]]) -> None:
    HISTORY_FILE.parent.mkdir(parents=True, exist_ok=True)
    tmp = HISTORY_FILE.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(history[-HISTORY_CAP:], indent=2), encoding="utf-8")
    tmp.replace(HISTORY_FILE)


def snapshot_for_history(state: dict[str, object]) -> dict[str, object]:
    counts = state.get("counts", {})
    if not isinstance(counts, dict):
        counts = {}
    metrics = state.get("metrics", {})
    if not isinstance(metrics, dict):
        metrics = {}
    decomp = state.get("decomp", {})
    if not isinstance(decomp, dict):
        decomp = {}
    conversion = decomp.get("conversion", {})
    if not isinstance(conversion, dict):
        conversion = {}
    now = time.time()
    return {
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "unix": int(now),
        "head": state.get("head", ""),
        "branch": state.get("branch", ""),
        "targets": counts.get("targets", 0),
        "needs_wiring": counts.get("by_status", {}).get("Needs wiring", 0)
        if isinstance(counts.get("by_status"), dict)
        else 0,
        "proposed": counts.get("by_status", {}).get("Proposed", 0)
        if isinstance(counts.get("by_status"), dict)
        else 0,
        "recorded": counts.get("by_status", {}).get("Recorded", 0)
        if isinstance(counts.get("by_status"), dict)
        else 0,
        "renamed": counts.get("by_status", {}).get("Renamed", 0)
        if isinstance(counts.get("by_status"), dict)
        else 0,
        "completion_pct": metrics.get("completion_pct", 0),
        "decomp_fuzzy_pct": decomp.get("fuzzy_pct", 0),
        "decomp_code_pct": decomp.get("code_pct", 0),
        "decomp_functions_pct": decomp.get("functions_pct", 0),
        "c_converted_pct": conversion.get("converted_pct", 0),
        "decomp_matched_functions": decomp.get("matched_functions", 0),
        "decomp_total_functions": decomp.get("total_functions", 0),
        "old_ref_total": metrics.get("old_ref_total", 0),
    }


def should_record_history(history: list[dict[str, object]], snapshot: dict[str, object]) -> bool:
    if not history:
        return True
    last = history[-1]
    tracked = [
        "targets",
        "needs_wiring",
        "proposed",
        "recorded",
        "renamed",
        "completion_pct",
        "decomp_fuzzy_pct",
        "decomp_code_pct",
        "decomp_functions_pct",
        "c_converted_pct",
        "decomp_matched_functions",
        "old_ref_total",
        "head",
    ]
    if any(last.get(key) != snapshot.get(key) for key in tracked):
        return True
    return int(snapshot["unix"]) - int(last.get("unix", 0)) >= HISTORY_INTERVAL_SECONDS


def update_history(state: dict[str, object]) -> list[dict[str, object]]:
    history = load_history()
    snapshot = snapshot_for_history(state)
    if should_record_history(history, snapshot):
        history.append(snapshot)
        write_history(history)
    return history[-HISTORY_CAP:]


def _load_json_obj(path: Path) -> dict[str, object]:
    if not path.exists():
        return {}
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {}
    return data if isinstance(data, dict) else {}


def _write_json_obj(path: Path, data: dict[str, object]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + ".tmp")
    tmp.write_text(json.dumps(data), encoding="utf-8")
    tmp.replace(path)


def load_unit_history(source: str) -> list[dict[str, object]]:
    data = _load_json_obj(UNIT_HISTORY_FILE)
    rows = data.get(source)
    if not isinstance(rows, list):
        return []
    return [row for row in rows if isinstance(row, dict)]


def load_fn_history(name: str) -> list[dict[str, object]]:
    data = _load_json_obj(FN_HISTORY_FILE)
    rows = data.get(name)
    if not isinstance(rows, list):
        return []
    return [row for row in rows if isinstance(row, dict)]


def update_unit_history(decomp: dict[str, object]) -> None:
    """Append a per-unit sample only when that unit's functions_pct or code_pct
    changed (change-gated, like should_record_history but per unit)."""
    units = decomp.get("units")
    if not isinstance(units, list) or not units:
        return
    store = _load_json_obj(UNIT_HISTORY_FILE)
    now = int(time.time())
    dirty = False
    for unit in units:
        if not isinstance(unit, dict):
            continue
        source = str(unit.get("source") or unit.get("name") or "")
        if not source:
            continue
        fp = float_pct(unit.get("functions_pct", 0))
        cp = float_pct(unit.get("code_pct", 0))
        rows = store.get(source)
        if not isinstance(rows, list):
            rows = []
        last = rows[-1] if rows else None
        if (
            last is not None
            and float_pct(last.get("fp", 0)) == fp
            and float_pct(last.get("cp", 0)) == cp
        ):
            continue
        rows.append(
            {
                "unix": now,
                "fp": fp,
                "cp": cp,
                "mc": int_value(unit.get("matched_code", 0)),
                "mf": int_value(unit.get("matched_functions", 0)),
                "tf": int_value(unit.get("total_functions", 0)),
            }
        )
        store[source] = rows[-UNIT_HISTORY_CAP:]
        dirty = True
    if dirty:
        _write_json_obj(UNIT_HISTORY_FILE, store)


def update_fn_history(decomp: dict[str, object]) -> None:
    """Append a per-function sample for unmatched (<100%) functions only, when
    that function's fuzzy_pct changed. Keyed by function name. Capped per fn."""
    units = decomp.get("units")
    if not isinstance(units, list) or not units:
        return
    store = _load_json_obj(FN_HISTORY_FILE)
    now = int(time.time())
    dirty = False
    seen: set[str] = set()
    for unit in units:
        if not isinstance(unit, dict):
            continue
        for fn in unit.get("functions", []):
            if not isinstance(fn, dict):
                continue
            name = str(fn.get("name") or "")
            if not name or name in seen:
                continue
            seen.add(name)
            fp = float_pct(fn.get("fuzzy_pct", 0))
            if fp >= 99.95:
                # Drop history for now-matched fns so the file stays small.
                if name in store:
                    del store[name]
                    dirty = True
                continue
            rows = store.get(name)
            if not isinstance(rows, list):
                rows = []
            last = rows[-1] if rows else None
            if last is not None and float_pct(last.get("fuzzy_pct", 0)) == fp:
                continue
            rows.append({"unix": now, "fuzzy_pct": fp})
            store[name] = rows[-FN_HISTORY_CAP:]
            dirty = True
    if dirty:
        _write_json_obj(FN_HISTORY_FILE, store)


# --- Phase 3: journal-window token attribution -------------------------------
# Footer scraping is lossy (narrow panes truncate claude's in/out) and absent for
# codex (its TUI shows duration, not tokens). The accurate source is each agent's
# session journal, which records exact per-message token usage + timestamps. Each
# journal FILE belongs to one lane — identifiable by the dispatch marker pane_io
# sends ("Read build/dispatch/<LANE>.task ..."). So a ledger row's [tstart, ts]
# window summed over that lane's journal gives accurate per-task tokens.
_JFILE_LANE_CACHE: dict[str, tuple[int, str]] = {}
_DISPATCH_MARKER = re.compile(r"dispatch/([A-Za-z0-9]+)\.task")


def _journal_file_lane(path: Path) -> str:
    """Which lane owns this journal file (via the dispatch-command marker). Cached
    by (path, size); files only grow so size is a sufficient invalidation key."""
    try:
        size = path.stat().st_size
    except OSError:
        return ""
    key = str(path)
    cached = _JFILE_LANE_CACHE.get(key)
    if cached and cached[0] == size:
        return cached[1]
    lane = ""
    try:
        with open(path, encoding="utf-8", errors="replace") as fh:
            for line in fh:
                if "dispatch/" not in line:
                    continue
                m = _DISPATCH_MARKER.search(line)
                if m:
                    lane = m.group(1)
                    break
    except OSError:
        lane = ""
    _JFILE_LANE_CACHE[key] = (size, lane)
    return lane


def _claude_glm_window(root: Path, lane: str, t0: float, t1: float,
                       lane_filter: bool = True) -> tuple[int, int]:
    """Sum input+output tokens from a Claude-format journal (Claude or GLM) over
    [t0, t1]. lane_filter restricts to the lane's files via the dispatch marker —
    needed when lanes share a config dir (OPUS/SON in ~/.claude); skip it for the
    single-lane GLM dir (~/.claude-glm has no dispatch marker and one owner)."""
    tin = tout = 0
    if not root.exists():
        return (0, 0)
    for path in root.rglob("*.jsonl"):
        try:
            if path.stat().st_mtime < t0:
                continue
        except OSError:
            continue
        if lane_filter and _journal_file_lane(path) != lane:
            continue
        try:
            with open(path, encoding="utf-8", errors="replace") as fh:
                for line in fh:
                    if '"usage"' not in line:
                        continue
                    try:
                        d = json.loads(line)
                    except json.JSONDecodeError:
                        continue
                    msg = d.get("message")
                    if not isinstance(msg, dict):
                        continue
                    u = msg.get("usage")
                    if not isinstance(u, dict):
                        continue
                    ts = _parse_iso(d.get("timestamp"))
                    if not ts or ts < t0 or ts > t1:
                        continue
                    tin += int_value(u.get("input_tokens"))
                    tout += int_value(u.get("output_tokens"))
        except OSError:
            continue
    return (tin, tout)


def _codex_window(lane: str, t0: float, t1: float) -> tuple[int, int]:
    """Sum codex tokens for the lane's rollout files over [t0, t1]. Per turn the
    billable count is output + (input - cached_input), excluding the reused context."""
    tin = tout = 0
    if not CODEX_SESSIONS_DIR.exists():
        return (0, 0)
    for path in CODEX_SESSIONS_DIR.rglob("*.jsonl"):
        try:
            if path.stat().st_mtime < t0 - 86400:
                continue
        except OSError:
            continue
        if _journal_file_lane(path) != lane:
            continue
        try:
            with open(path, encoding="utf-8", errors="replace") as fh:
                for line in fh:
                    if '"last_token_usage"' not in line:
                        continue
                    try:
                        d = json.loads(line)
                    except json.JSONDecodeError:
                        continue
                    payload = d.get("payload")
                    if not isinstance(payload, dict):
                        continue
                    info = payload.get("info")
                    if not isinstance(info, dict):
                        continue
                    u = info.get("last_token_usage")
                    if not isinstance(u, dict):
                        continue
                    ts = _parse_iso(d.get("timestamp"))
                    if not ts or ts < t0 or ts > t1:
                        continue
                    inp = int_value(u.get("input_tokens"))
                    cached = int_value(u.get("cached_input_tokens"))
                    tin += max(0, inp - cached)
                    tout += int_value(u.get("output_tokens"))
        except OSError:
            continue
    return (tin, tout)


def journal_window_tokens(provider: str, lane: str, t0: float, t1: float) -> tuple[int, int]:
    """Accurate (in, out) tokens for a task's [t0, t1] window from the lane's journal."""
    if t0 <= 0 or t1 <= 0 or t1 < t0 or not lane:
        return (0, 0)
    if provider == "claude":
        return _claude_glm_window(CLAUDE_PROJECT_DIR, lane, t0, t1, lane_filter=True)
    if provider == "glm":
        return _claude_glm_window(GLM_PROJECT_DIR, lane, t0, t1, lane_filter=False)
    if provider == "codex":
        return _codex_window(lane, t0, t1)
    return (0, 0)


def update_fn_token_history() -> None:
    """Fold newly-appended rows of the pane_io token ledger (FN_TOKEN_LEDGER) into a
    per-function token time-series. Offset-tracked so each row is ingested once; resets
    if the ledger is rotated/truncated. Each fn keeps the last FN_TOKEN_HISTORY_CAP rows.
    Token counts come from the lane's journal over [tstart, ts] (accurate), falling back
    to the row's footer counts when the journal join yields nothing."""
    try:
        if not FN_TOKEN_LEDGER.exists():
            return
        size = FN_TOKEN_LEDGER.stat().st_size
    except OSError:
        return
    store = _load_json_obj(FN_TOKEN_HISTORY_FILE)
    fns = store.get("fns") if isinstance(store.get("fns"), dict) else {}
    off = int_value(store.get("_offset"))
    if off > size:
        off = 0  # ledger rotated/truncated -> re-read from start
    try:
        with open(FN_TOKEN_LEDGER, "rb") as fh:
            fh.seek(off)
            chunk = fh.read()
    except OSError:
        return
    last_nl = chunk.rfind(b"\n")
    if last_nl < 0:
        return  # no complete line yet (mid-write)
    complete = chunk[:last_nl + 1]
    new_off = off + len(complete)
    dirty = False
    for line in complete.decode("utf-8", "replace").splitlines():
        line = line.strip()
        if not line:
            continue
        try:
            r = json.loads(line)
        except json.JSONDecodeError:
            continue
        ts = int_value(r.get("ts"))
        tstart = int_value(r.get("tstart"))
        prov = str(r.get("provider") or "")
        lane = str(r.get("lane") or "")
        tin = int_value(r.get("tokens_in"))
        tout = int_value(r.get("tokens_out"))
        src = "footer"
        # Prefer accurate journal-window tokens; fall back to footer counts.
        if tstart > 0:
            jin, jout = journal_window_tokens(prov, lane, tstart, ts)
            if jin + jout > 0:
                tin, tout, src = jin, jout, "journal"
        row_fns = r.get("fns") if isinstance(r.get("fns"), list) else []
        for fn in row_fns:
            fn = str(fn)
            if not fn:
                continue
            rows = fns.get(fn)
            if not isinstance(rows, list):
                rows = []
            rows.append({"unix": ts, "provider": prov, "in": tin, "out": tout,
                         "src": src, "tag": r.get("tag", ""), "file": r.get("file", "")})
            fns[fn] = rows[-FN_TOKEN_HISTORY_CAP:]
            dirty = True
    store["fns"] = fns
    store["_offset"] = new_off
    if dirty or new_off != off:
        _write_json_obj(FN_TOKEN_HISTORY_FILE, store)


def load_fn_token_history(name: str) -> dict[str, object]:
    """Per-function token time-series for one fn (backs /api/history/fn_tokens)."""
    store = _load_json_obj(FN_TOKEN_HISTORY_FILE)
    fns = store.get("fns") if isinstance(store.get("fns"), dict) else {}
    rows = fns.get(name) if isinstance(fns.get(name), list) else []
    return {"available": bool(rows), "fn": name, "rows": rows}


def load_fn_token_summary(limit: int = 100) -> dict[str, object]:
    """Aggregate per-fn token totals (by provider) for the 'Token spend by function'
    table — sorted by total tokens desc."""
    store = _load_json_obj(FN_TOKEN_HISTORY_FILE)
    fns = store.get("fns") if isinstance(store.get("fns"), dict) else {}
    out = []
    for fn, rows in fns.items():
        if not isinstance(rows, list) or not rows:
            continue
        by_prov: dict[str, int] = {}
        total = 0
        last_ts = 0
        tag = ""
        file = ""
        for r in rows:
            prov = str(r.get("provider") or "?")
            tok = int_value(r.get("in")) + int_value(r.get("out"))
            by_prov[prov] = by_prov.get(prov, 0) + tok
            total += tok
            ts = int_value(r.get("unix"))
            if ts >= last_ts:
                last_ts = ts
                tag = r.get("tag", "") or tag
                file = r.get("file", "") or file
        out.append({
            "fn": fn, "total": total, "by_provider": by_prov,
            "tasks": len(rows), "last_ts": last_ts, "tag": tag, "file": file,
        })
    out.sort(key=lambda x: -x["total"])
    return {"available": bool(out), "count": len(out), "fns": out[:max(1, limit)]}


def load_unit_functions(source: str) -> dict[str, object]:
    """Lazy endpoint backing: return one unit's functions[] without shipping all
    units. Matches on metadata.source_path (preferred) or unit name."""
    if not DECOMP_REPORT.exists() or not source:
        return {"available": False, "source": source, "functions": []}
    try:
        report = json.loads(DECOMP_REPORT.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return {"available": False, "source": source, "functions": []}
    norm = source.replace("\\", "/")
    for unit in report.get("units", []):
        if not isinstance(unit, dict):
            continue
        metadata = unit.get("metadata", {})
        if not isinstance(metadata, dict):
            metadata = {}
        unit_source = str(metadata.get("source_path", "")).replace("\\", "/")
        if unit_source != norm and str(unit.get("name", "")) != source:
            continue
        functions = []
        for index, function in enumerate(unit.get("functions", [])):
            if not isinstance(function, dict):
                continue
            fn_pct = float_pct(function.get("fuzzy_match_percent", 0))
            name = str(function.get("name", ""))
            row = {
                "index": index,
                "name": name,
                "size": int_value(function.get("size", 0)),
                "fuzzy_pct": fn_pct,
                "status": match_status(fn_pct),
            }
            hard = hard_target_for(name)
            if hard:
                row["difficulty"] = hard
            functions.append(row)
        functions.sort(
            key=lambda row: (
                float(row.get("fuzzy_pct", 0)) >= 99.95,
                -int(row.get("size", 0)),
                str(row.get("name", "")),
            )
        )
        return {
            "available": True,
            "source": unit_source or source,
            "name": unit.get("name", ""),
            "functions": functions,
        }
    return {"available": False, "source": source, "functions": []}


def get_state(force: bool = False) -> dict[str, object]:
    """Return build_state() through a short TTL cache so the 5s auto-refresh and
    the slow rebuild stop colliding."""
    now = time.monotonic()
    cached = _STATE_CACHE.get("value")
    if not force and cached is not None and now < float(_STATE_CACHE.get("expires", 0)):
        return cached  # type: ignore[return-value]
    state = build_state()
    _STATE_CACHE["value"] = state
    _STATE_CACHE["expires"] = now + STATE_CACHE_TTL_SECONDS
    return state


def build_state() -> dict[str, object]:
    proposed = load_simple_map(SM_DIR / "proposed_symbols.txt")
    applied = load_simple_map(SM_DIR / "applied_symbols.txt")
    structural = load_simple_map(SM_DIR / "structural_applied.txt")
    leads = load_leads(SM_DIR / "leads_needs_wiring.md")
    lead_order = {fn: idx for idx, fn in enumerate(leads)}
    symbols_by_name, symbols_by_addr = load_symbols(SYMBOLS)
    tu_map = load_tu_map(FUNC_TU_MAP)

    all_fns = set(proposed) | set(applied) | set(structural) | set(leads)
    rows = []
    for fn in sorted(all_fns, key=lambda item: int(item.removeprefix("fn_"), 16)):
        base = proposed.get(fn) or applied.get(fn) or structural.get(fn) or leads.get(fn)
        if not base:
            continue

        name = leads.get(fn, {}).get("name", base["name"])
        addr = fn_to_addr(fn)
        current = symbols_by_addr.get(addr)
        tu = tu_map.get(name) or tu_map.get(fn) or {}

        status = "Proposed"
        if fn in leads:
            status = "Needs wiring"
        if current and current["name"] == name:
            status = "Renamed"
        elif fn in applied or fn in structural:
            status = "Recorded"

        old_refs = source_refs(fn) if status in ("Needs wiring", "Proposed") else 0
        if old_refs == -1:
            old_refs_label = "unknown"
        else:
            old_refs_label = str(old_refs)

        rows.append(
            {
                "fn": fn,
                "name": name,
                "status": status,
                "addr": addr,
                "size": (current or {}).get("size") or tu.get("size", ""),
                "source": tu.get("src") or "",
                "provenance": leads.get(fn, {}).get(
                    "provenance", base.get("provenance", "")
                ),
                "header": leads.get(fn, {}).get("header", ""),
                "current_symbol": (current or {}).get("name", ""),
                "old_refs": old_refs_label,
            }
        )

    rank = {"Needs wiring": 0, "Proposed": 1, "Recorded": 2, "Renamed": 3}
    rows.sort(
        key=lambda row: (
            rank.get(str(row["status"]), 9),
            lead_order.get(str(row["fn"]), int(str(row["fn"])[3:], 16)),
        )
    )
    next_target = next((row for row in rows if row["status"] == "Needs wiring"), None)

    status_counts: dict[str, int] = {}
    source_counts: Counter[str] = Counter()
    provenance_counts: Counter[str] = Counter()
    source_summary: dict[str, dict[str, object]] = {}
    old_ref_total = 0
    known_size_total = 0
    for row in rows:
        key = str(row["status"])
        status_counts[key] = status_counts.get(key, 0) + 1
        source = str(row["source"] or "unknown")
        source_counts[source] += 1
        provenance_counts[classify_provenance(str(row["provenance"]))] += 1
        if str(row["old_refs"]).isdigit():
            old_ref_total += int(str(row["old_refs"]))
        size = int_value(row["size"])
        known_size_total += size
        summary = source_summary.setdefault(
            source,
            {
                "source": source,
                "label": source_label(source),
                "targets": 0,
                "wired": 0,
                "renamed": 0,
                "recorded": 0,
                "needs_wiring": 0,
                "proposed": 0,
                "size_bytes": 0,
            },
        )
        summary["targets"] = int(summary["targets"]) + 1
        summary["size_bytes"] = int(summary["size_bytes"]) + size
        if key in ("Recorded", "Renamed"):
            summary["wired"] = int(summary["wired"]) + 1
        if key == "Renamed":
            summary["renamed"] = int(summary["renamed"]) + 1
        elif key == "Recorded":
            summary["recorded"] = int(summary["recorded"]) + 1
        elif key == "Needs wiring":
            summary["needs_wiring"] = int(summary["needs_wiring"]) + 1
        elif key == "Proposed":
            summary["proposed"] = int(summary["proposed"]) + 1

    total_targets = len(rows)
    wired_targets = status_counts.get("Recorded", 0) + status_counts.get("Renamed", 0)
    active_targets = status_counts.get("Needs wiring", 0) + status_counts.get("Proposed", 0)
    tu_tiles = []
    for summary in source_summary.values():
        targets = int(summary["targets"])
        wired = int(summary["wired"])
        summary["completion_pct"] = pct(wired, targets)
        tu_tiles.append(summary)
    tu_tiles.sort(
        key=lambda row: (
            -int(row.get("targets", 0)),
            str(row.get("source", "")),
        )
    )
    decomp = load_decomp_report(DECOMP_REPORT)
    if isinstance(decomp, dict):
        decomp["conversion"] = load_conversion_scan()
    # Merge status.md attempt log with git-commit-derived entries so codex's
    # per-file work (committed to git, not status.md) appears in the log and in
    # per-unit drill-downs. The front-end reverse-sorts and filters by `file`.
    attempt_log = merged_attempt_log(limit=1000)
    attack_matrix = load_attack_matrix(decomp if isinstance(decomp, dict) else {})
    return {
        "version": DASHBOARD_VERSION,
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
        "repo": str(ROOT),
        "head": git_value(["rev-parse", "--short", "HEAD"]),
        "branch": git_value(["branch", "--show-current"]),
        "recent_commits": recent_commits(),
        "decomp": decomp,
        "attempt_log": attempt_log,
        "attack_matrix": attack_matrix,
        "next_target": next_target,
        "counts": {
            "leads": len(leads),
            "proposed": len(proposed),
            "applied": len(applied),
            "structural_recorded": len(structural),
            "targets": len(rows),
            "by_status": status_counts,
        },
        "metrics": {
            "completion_pct": pct(wired_targets, total_targets),
            "rename_pct": pct(status_counts.get("Renamed", 0), total_targets),
            "active_pct": pct(active_targets, total_targets),
            "wired_targets": wired_targets,
            "active_targets": active_targets,
            "old_ref_total": old_ref_total,
            "known_size_bytes": known_size_total,
        },
        "charts": {
            "status": [
                {"label": key, "value": status_counts.get(key, 0)}
                for key in ("Needs wiring", "Proposed", "Recorded", "Renamed")
            ],
            "sources": [
                {"label": key, "value": value}
                for key, value in source_counts.most_common(8)
            ],
            "provenance": [
                {"label": key, "value": value}
                for key, value in provenance_counts.most_common()
            ],
        },
        "targets": rows,
        "tu_tiles": tu_tiles,
        "files": {
            "leads": str(SM_DIR / "leads_needs_wiring.md"),
            "proposed": str(SM_DIR / "proposed_symbols.txt"),
            "applied": str(SM_DIR / "applied_symbols.txt"),
            "structural": str(SM_DIR / "structural_applied.txt"),
            "symbols": str(SYMBOLS),
            "tu_map": str(FUNC_TU_MAP),
            "decomp_report": str(DECOMP_REPORT),
            "decomp_status_log": str(DECOMP_STATUS_LOG),
        },
    }


# =========================================================================== #
# v9: decomp.me-style function reader (compile + objdiff, per-instruction)     #
# =========================================================================== #
# band.py already encodes the exact compile+objdiff invocation against the
# immutable target object. We reuse compile_check (per-file flags/version/target)
# the same way band.py does, then run objdiff-cli in JSON mode and slice out the
# requested fn's left(TARGET)/right(CURRENT) instruction rows.
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "decomp_work"))
try:  # imported lazily-safe: the reader endpoint degrades to an error payload
    import compile_check as _compile_check  # type: ignore
except Exception:  # noqa: BLE001
    _compile_check = None
_OBJDIFF_CLI = ROOT / "tools" / ("objdiff-cli.exe" if os.name == "nt" else "objdiff-cli")

# TTL cache keyed by (source, fn). The compile is slow (~seconds), so a hit
# within the TTL or while the source file is unchanged returns instantly.
_ASM_CACHE: dict[str, dict[str, object]] = {}
_ASM_CACHE_TTL_SECONDS = 90.0
_ASM_LOCK = threading.Lock()


def _source_defines_fn(path: Path, fn: str) -> bool:
    """Return true when `path` contains a likely C definition for `fn`.

    Generated band rows can name non-existent files, and many symbols are also
    present as externs or calls in unrelated TUs. Prefer the source file with a
    function body so the reader compiles the unit that owns the selected symbol.
    """
    if not fn:
        return False
    try:
        lines = path.read_text(errors="ignore").splitlines()
    except OSError:
        return False

    needle = f"{fn}("
    spaced_needle = f"{fn} ("
    for idx, line in enumerate(lines):
        if needle not in line and spaced_needle not in line:
            continue
        stripped = line.strip()
        if not stripped or stripped.startswith("*") or stripped.startswith("//"):
            continue
        if stripped.startswith("extern ") or stripped.endswith(";"):
            continue

        sig = stripped
        for extra in lines[idx + 1:idx + 8]:
            if "{" in sig or ";" in sig:
                break
            sig += " " + extra.strip()

        brace_at = sig.find("{")
        semi_at = sig.find(";")
        if brace_at >= 0 and (semi_at < 0 or brace_at < semi_at):
            return True
    return False


def _resolve_source_path(name: str, fn: str = "") -> Path | None:
    """Resolve a stem/report path to a tracked src/**.c.

    report.json can contain generated band units such as `src/band_mtool.c`.
    Those do not exist on disk, so function-reader clicks need a final fallback:
    find the real source file that contains the selected fn symbol.
    """
    if not name:
        name = ""
    norm = name.replace("\\", "/")
    cand = ROOT / norm
    if norm and cand.exists() and cand.is_file():
        return cand.resolve()
    stem = Path(norm).stem
    if stem:
        matches = sorted((ROOT / "src").rglob(f"{stem}.c"))
        if matches:
            return matches[0].resolve()
    if fn:
        found_paths: list[Path] = []
        try:
            proc = subprocess.run(
                ["rg", "-l", "--fixed-strings", fn, "src", "--glob", "*.c", "--glob", "!*.inc"],
                cwd=str(ROOT),
                check=False,
                capture_output=True,
                text=True,
                timeout=5,
            )
        except (OSError, subprocess.TimeoutExpired):
            proc = None
        if proc is not None and proc.returncode == 0:
            for line in proc.stdout.splitlines():
                found = (ROOT / line.strip()).resolve()
                if found.exists():
                    found_paths.append(found)
            for found in found_paths:
                if _source_defines_fn(found, fn):
                    return found
            if found_paths:
                return found_paths[0]
    return None


def _instr_text(ins: object) -> str:
    if not isinstance(ins, dict):
        return "---"
    inner = ins.get("instruction")
    if isinstance(inner, dict):
        return str(inner.get("formatted") or inner.get("mnemonic") or "?")
    return "---"


def _row_state(lk: str, rk: str) -> str:
    """Map objdiff diff_kind pair -> a coarse row colour class."""
    if lk in ("DIFF_NONE", "") and rk in ("DIFF_NONE", ""):
        return "same"
    if lk in ("DIFF_DELETE",) or rk in ("DIFF_INSERT",):
        return "addrm"
    return "diff"


def compute_asm_diff(source: str, fn: str) -> dict[str, object]:
    """Compile `source` to its base .o and objdiff vs the target; return the
    aligned per-instruction rows for `fn`. Shape mirrors band.py cmd_diff."""
    if _compile_check is None:
        return {"available": False, "error": "compile_check import failed", "fn": fn, "source": source}
    src_path = _resolve_source_path(source, fn)
    if src_path is None:
        return {"available": False, "error": f"source not found: {source}", "fn": fn, "source": source}
    if not _OBJDIFF_CLI.exists():
        return {"available": False, "error": "objdiff-cli not found", "fn": fn, "source": source}

    cache_key = f"{src_path}|{fn}"
    try:
        mtime = src_path.stat().st_mtime
    except OSError:
        mtime = 0.0
    now = time.monotonic()
    with _ASM_LOCK:
        hit = _ASM_CACHE.get(cache_key)
        if hit is not None and hit.get("_mtime") == mtime and now < float(hit.get("_expires", 0)):
            return hit["payload"]  # type: ignore[return-value]

    try:
        target_o = _compile_check.find_target_obj(src_path)
        if not Path(target_o).exists():
            return {"available": False, "error": f"target object missing: {target_o}", "fn": fn, "source": source}
        base_o = _compile_check.compile_source(src_path, verbose=False)
    except SystemExit as exc:  # compile_source exits on failure
        return {"available": False, "error": f"compile failed: {exc}", "fn": fn, "source": source}
    except Exception as exc:  # noqa: BLE001
        return {"available": False, "error": f"compile error: {exc}", "fn": fn, "source": source}

    try:
        proc = subprocess.run(
            [str(_OBJDIFF_CLI), "diff", "-1", str(target_o), "-2", str(base_o),
             "-o", "-", "--format", "json",
             "-c", "ppc.calculatePoolRelocations=false"],
            cwd=str(ROOT), capture_output=True, text=True, timeout=180,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return {"available": False, "error": f"objdiff failed: {exc}", "fn": fn, "source": source}
    if proc.returncode != 0:
        return {"available": False, "error": "objdiff failed: " + proc.stderr[:300], "fn": fn, "source": source}
    try:
        diff = json.loads(proc.stdout)
    except json.JSONDecodeError:
        return {"available": False, "error": "objdiff returned non-JSON", "fn": fn, "source": source}

    def _side(side: str) -> list:
        for sym in diff.get(side, {}).get("symbols", []):
            if sym.get("name") == fn:
                return sym.get("instructions", []) or []
        return []

    left, right = _side("left"), _side("right")
    rows = []
    matched = 0
    total = 0
    for idx in range(max(len(left), len(right))):
        li = left[idx] if idx < len(left) else None
        ri = right[idx] if idx < len(right) else None
        lk = (li.get("diff_kind") if isinstance(li, dict) else None) or ("X" if li is None else "DIFF_NONE")
        rk = (ri.get("diff_kind") if isinstance(ri, dict) else None) or ("X" if ri is None else "DIFF_NONE")
        state = _row_state(lk, rk)
        if li is not None and ri is not None:
            total += 1
            if state == "same":
                matched += 1
        rows.append({"l": _instr_text(li), "r": _instr_text(ri), "state": state})
    # fuzzy_pct from objdiff symbol match_percent (right side), fallback to ratio
    fuzzy = 0.0
    for sym in diff.get("right", {}).get("symbols", []):
        if sym.get("name") == fn:
            fuzzy = float(sym.get("match_percent") or 0.0)
            break
    if fuzzy == 0.0 and total:
        fuzzy = round(100.0 * matched / total, 2)

    payload = {
        "available": True,
        "fn": fn,
        "source": str(src_path.relative_to(ROOT)).replace("\\", "/"),
        "target_obj": Path(target_o).name,
        "fuzzy_pct": round(fuzzy, 2),
        "rows": rows,
        "row_count": len(rows),
        "matched": matched,
        "total": total,
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }
    with _ASM_LOCK:
        _ASM_CACHE[cache_key] = {"_mtime": mtime, "_expires": now + _ASM_CACHE_TTL_SECONDS, "payload": payload}
    return payload


# =========================================================================== #
# v9: per-function wall / equivalent / attempt info                           #
# =========================================================================== #
_WALL_CLASS_RE = re.compile(r"\b(W-[A-Za-z0-9-]+|W[0-9])\b")


def _parse_equivalent(fn: str) -> tuple[bool, str]:
    """Return (is_equivalent, note) by scanning equivalent.txt for `fn`."""
    text = read_text(EQUIVALENT_TXT)
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        # `fn_XXXX   # reason`  (whitespace or tab separated)
        head = re.split(r"[\s#]", stripped, 1)[0]
        if head == fn:
            note = ""
            if "#" in stripped:
                note = stripped.split("#", 1)[1].strip()
            return True, note
    return False, ""


def _parse_walls_md(fn: str) -> tuple[str, str]:
    """Return (wall_class, note) for `fn` from WALLS.md table/bullet entries."""
    text = read_text(WALLS_MD)
    for raw in text.splitlines():
        # match the fn name wrapped in backticks or bare, anywhere on the line
        if fn not in raw:
            continue
        # Skip lines that are pure cross-references in prose (keep table rows + bullets)
        stripped = raw.strip()
        if not (stripped.startswith("|") or stripped.startswith("- ")):
            continue
        cls_match = _WALL_CLASS_RE.search(raw)
        wall_class = cls_match.group(1) if cls_match else ""
        # Trim markdown table pipes / leading bullet for a readable note.
        note = stripped.strip("|").strip()
        note = re.sub(r"^[-*]\s*", "", note)
        # Collapse the leading "`fn_XXXX`" token out of the note for brevity.
        note = note.replace(f"`{fn}`", "").replace(fn, "").strip(" |-")
        if len(note) > 360:
            note = note[:357] + "..."
        return wall_class, note
    return "", ""


def _parse_cs_walls(fn: str) -> bool:
    """True if `fn` is listed in build/cs_walls.json (a flat list of fn names)."""
    if not CS_WALLS_JSON.exists():
        return False
    try:
        data = json.loads(CS_WALLS_JSON.read_text(encoding="utf-8", errors="replace"))
    except (OSError, json.JSONDecodeError):
        return False
    if isinstance(data, list):
        return fn in data
    if isinstance(data, dict):
        return fn in data
    return False


def _fn_attempts(fn: str) -> list[dict[str, object]]:
    """Recent status.md attempt-log lines that name `fn`."""
    out = []
    for row in merged_attempt_log(limit=1500):
        if row.get("function") == fn or (fn and fn in str(row.get("message", ""))):
            out.append({
                "timestamp": row.get("timestamp"),
                "unix": row.get("unix"),
                "kind": row.get("kind"),
                "percent": row.get("percent"),
                "message": row.get("message"),
            })
    return out[-25:]


def load_fn_kg_context(fn: str) -> dict[str, object]:
    if not KG_DB.exists():
        return {"available": False, "error": "kg.db not found"}
    import sqlite3
    try:
        conn = sqlite3.connect(str(KG_DB), timeout=5.0)
        conn.row_factory = sqlite3.Row
        def has_table(name: str) -> bool:
            return conn.execute(
                "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?",
                (name,),
            ).fetchone() is not None
        if not (has_table("function_calls") and has_table("function_tags") and has_table("name_evidence")):
            conn.close()
            return {"available": False, "error": "callgraph tables not built"}
        tags = [
            dict(row) for row in conn.execute(
                """
                SELECT tag, kind, confidence, round(score, 2) AS score, evidence
                FROM function_tags
                WHERE symbol=?
                ORDER BY score DESC, kind, tag
                LIMIT 10
                """,
                (fn,),
            )
        ]
        evidence = [
            dict(row) for row in conn.execute(
                """
                SELECT candidate, source, confidence, round(score, 2) AS score, evidence
                FROM name_evidence
                WHERE symbol=?
                ORDER BY score DESC, source, candidate
                LIMIT 10
                """,
                (fn,),
            )
        ]
        callees = [
            dict(row) for row in conn.execute(
                """
                SELECT callee AS fn, callee_tu AS tu, confidence, source, evidence
                FROM function_calls
                WHERE caller=?
                ORDER BY CASE confidence WHEN 'high' THEN 0 WHEN 'medium' THEN 1 ELSE 2 END,
                         source, fn
                LIMIT 10
                """,
                (fn,),
            )
        ]
        callers = [
            dict(row) for row in conn.execute(
                """
                SELECT caller AS fn, caller_tu AS tu, confidence, source, evidence
                FROM function_calls
                WHERE callee=?
                ORDER BY CASE confidence WHEN 'high' THEN 0 WHEN 'medium' THEN 1 ELSE 2 END,
                         source, fn
                LIMIT 10
                """,
                (fn,),
            )
        ]
        conn.close()
    except sqlite3.Error as exc:
        return {"available": False, "error": str(exc)}
    return {
        "available": True,
        "tags": tags,
        "name_evidence": evidence,
        "callees": callees,
        "callers": callers,
    }


def load_fn_info(fn: str) -> dict[str, object]:
    is_equiv, equiv_note = _parse_equivalent(fn)
    wall_class, wall_note = _parse_walls_md(fn)
    in_cs_walls = _parse_cs_walls(fn)
    attempts = _fn_attempts(fn)
    jobs = [job for job in load_crack_jobs(limit=200).get("jobs", []) if isinstance(job, dict) and job.get("fn") == fn]
    return {
        "fn": fn,
        "wall_class": wall_class,
        "note": wall_note or equiv_note,
        "is_equivalent": is_equiv,
        "in_cs_walls": in_cs_walls,
        "difficulty": hard_target_for(fn),
        "crack_jobs": jobs[:8],
        "crack_job_count": len(jobs),
        "attempts": attempts,
        "attempt_count": len(attempts),
        "kg": load_fn_kg_context(fn),
    }


# =========================================================================== #
# v9: agent-activity panel (merge shell dashboards into the web)              #
# =========================================================================== #
def load_agents() -> dict[str, object]:
    """Parse coordination/{agent_status.txt, claims.json, tasks.json} into a
    who-is-working-on-what view."""
    agents: dict[str, dict[str, object]] = {}

    # agent_status.txt: e.g. "20:22:58 codex=false opencode=true" (latest line)
    status_line = ""
    flags: dict[str, bool] = {}
    for line in read_text(AGENT_STATUS_TXT).splitlines():
        line = line.strip()
        if line:
            status_line = line
    if status_line:
        for tok in status_line.split():
            if "=" in tok:
                key, _, val = tok.partition("=")
                flags[key] = val.strip().lower() in ("true", "1", "yes", "busy", "active")

    # In-flight task lookup: latest 'claimed' status task per function.
    inflight: dict[str, dict[str, object]] = {}
    try:
        tasks = json.loads(TASKS_JSON.read_text(encoding="utf-8", errors="replace"))
    except (OSError, json.JSONDecodeError):
        tasks = []
    if isinstance(tasks, list):
        for t in tasks:
            if not isinstance(t, dict):
                continue
            if t.get("status") == "claimed":
                by = str(t.get("claimed_by") or "")
                if by:
                    inflight.setdefault(by, t)

    # claims.json: [{agent, function, claimed_at, task_id}]
    try:
        claims = json.loads(CLAIMS_JSON.read_text(encoding="utf-8", errors="replace"))
    except (OSError, json.JSONDecodeError):
        claims = []
    rows: list[dict[str, object]] = []
    if isinstance(claims, list):
        # newest claim per agent wins for the live table
        latest: dict[str, dict[str, object]] = {}
        for c in claims:
            if not isinstance(c, dict):
                continue
            agent = str(c.get("agent") or "")
            if not agent:
                continue
            prev = latest.get(agent)
            if prev is None or str(c.get("claimed_at", "")) >= str(prev.get("claimed_at", "")):
                latest[agent] = c
        for agent, c in latest.items():
            task = inflight.get(agent) or {}
            meta = task.get("meta", {}) if isinstance(task.get("meta"), dict) else {}
            # status flag: prefer the agent_status busy flags, else infer from inflight
            busy = flags.get(agent.split("-")[0], flags.get(agent, bool(task)))
            rows.append({
                "agent": agent,
                "function": str(c.get("function") or task.get("function") or ""),
                "file": str(meta.get("file") or ""),
                "claimed_at": c.get("claimed_at"),
                "task_status": str(task.get("status") or ""),
                "busy": bool(busy),
            })
    rows.sort(key=lambda r: str(r.get("claimed_at") or ""), reverse=True)

    # --- union: make ALL known agents visible, not just those with a live claim.
    # Old behaviour derived the agent list from claims.json (-> only glm/codex).
    # Merge in (a) live lock owners, (b) agent_limits roster, (c) token-tracked
    # agents, (d) a static roster, so claude/deepseek/mimo/qwen also appear.
    def base_name(a: str) -> str:
        a = (a or "").lower()
        for stem in AGENT_ROSTER:
            if a.startswith(stem):
                return stem
        return a.split("-")[0]

    present = {base_name(str(r.get("agent"))) for r in rows}
    # (a) lock owners — show what file/fn they're holding as current work
    try:
        for lk in load_locks().get("locks", []):
            owner = str(lk.get("owner") or "")
            if not owner or base_name(owner) in present:
                continue
            rows.append({
                "agent": owner,
                "function": lk.get("key") if lk.get("scope") == "fn" else "",
                "file": lk.get("file") or (lk.get("key") if lk.get("scope") == "file" else ""),
                "claimed_at": None, "task_status": "lease", "busy": True,
            })
            present.add(base_name(owner))
    except Exception:
        pass
    # (b/c/d) idle roster members so the fleet is always fully represented
    extra_names: list[str] = list(AGENT_ROSTER)
    try:
        lim = _load_json_obj(AGENT_LIMITS_JSON).get("agents", [])
        extra_names += [str(a.get("name")) for a in lim if isinstance(a, dict)]
    except Exception:
        pass
    extra_names += list((_load_json_obj(AGENT_TOKENS_JSON).get("agents", {}) or {}).keys())
    for name in extra_names:
        bn = base_name(name)
        if bn and bn not in present:
            rows.append({"agent": bn, "function": "", "file": "",
                         "claimed_at": None, "task_status": "idle",
                         "busy": flags.get(bn, False)})
            present.add(bn)

    # token usage (.omc/agent_tokens.json) merged in per agent when names match
    tokens = _load_json_obj(AGENT_TOKENS_JSON).get("agents", {})
    if isinstance(tokens, dict):
        for r in rows:
            tk = tokens.get(r["agent"])
            if isinstance(tk, dict):
                r["tokens_used"] = int_value(tk.get("tokens_used", 0))
                r["token_limit"] = int_value(tk.get("limit", 0))
                r["token_status"] = tk.get("status", "")

    return {
        "available": bool(rows) or bool(flags),
        "status_line": status_line,
        "flags": flags,
        "agents": rows,
        "queued": sum(1 for t in tasks if isinstance(t, dict) and t.get("status") == "queued") if isinstance(tasks, list) else 0,
        "claimed": sum(1 for t in tasks if isinstance(t, dict) and t.get("status") == "claimed") if isinstance(tasks, list) else 0,
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def load_locks() -> dict[str, object]:
    """Read the SQLite fleet-lock DB (coordination/locks.db) for the live who-owns-what
    view. Read-only and best-effort: missing DB / no sqlite3 just yields an empty table,
    never an error. Expired rows are filtered out (and dropped) like the CLI does."""
    import sqlite3  # stdlib; local import keeps the dashboard importable if ever absent

    if not LOCKS_DB.exists():
        return {"available": False, "locks": [], "files": [], "fns": [],
                "generated_at": time.strftime("%Y-%m-%d %H:%M:%S")}
    now = time.time()
    rows: list[dict[str, object]] = []
    try:
        conn = sqlite3.connect(str(LOCKS_DB), timeout=5.0)
        conn.row_factory = sqlite3.Row
        conn.execute("PRAGMA busy_timeout=5000")
        # reclaim expired rows opportunistically so the dashboard never shows ghosts
        try:
            conn.execute("DELETE FROM locks WHERE expires_at != 0 AND expires_at <= ?", (now,))
            conn.commit()
        except sqlite3.Error:
            pass
        cur = conn.execute("SELECT * FROM locks ORDER BY scope, acquired_at DESC")
        for r in cur.fetchall():
            d = dict(r)
            exp = float(d.get("expires_at") or 0)
            d["ttl_remaining"] = None if exp == 0 else max(0, round(exp - now))
            d["age"] = round(now - float(d.get("acquired_at") or now))
            rows.append(d)
        conn.close()
    except sqlite3.Error as exc:
        return {"available": False, "error": str(exc), "locks": [],
                "files": [], "fns": [], "generated_at": time.strftime("%Y-%m-%d %H:%M:%S")}

    files = [r for r in rows if r.get("scope") == "file"]
    fns = [r for r in rows if r.get("scope") == "fn"]
    owners = sorted({str(r.get("owner") or "") for r in rows if r.get("owner")})
    return {
        "available": True,
        "locks": rows,
        "files": files,
        "fns": fns,
        "owners": owners,
        "file_count": len(files),
        "fn_count": len(fns),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


_LOCKS_MOD = None
_CRACK_QUEUE_LOCK = threading.Lock()


def locks_module():
    """Import coordination/locks.py once (cached) so the dashboard's POST controls
    reuse the exact same atomic acquire/release/renew/gc logic as the CLI."""
    global _LOCKS_MOD
    if _LOCKS_MOD is None:
        import importlib.util
        spec = importlib.util.spec_from_file_location("decomp_locks", COORD_DIR / "locks.py")
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        _LOCKS_MOD = mod
    return _LOCKS_MOD


def _read_json_list(path: Path) -> list[dict[str, object]]:
    if not path.exists():
        return []
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return []
    if isinstance(data, list):
        return [row for row in data if isinstance(row, dict)]
    if isinstance(data, dict) and isinstance(data.get("jobs"), list):
        return [row for row in data["jobs"] if isinstance(row, dict)]
    return []


def _write_json_list(path: Path, rows: list[dict[str, object]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(path.suffix + ".tmp")
    tmp.write_text(json.dumps(rows, indent=2), encoding="utf-8")
    tmp.replace(path)


def load_crack_jobs(limit: int = 80) -> dict[str, object]:
    rows = _read_json_list(CRACK_QUEUE_JSON)
    rows.sort(key=lambda row: int_value(row.get("created_unix")) or timestamp_unix(row.get("created")), reverse=True)
    counts = Counter(str(row.get("status") or "queued") for row in rows)
    return {
        "available": True,
        "source": str(CRACK_QUEUE_JSON),
        "lanes": CRACK_LANES,
        "strategies": CRACK_STRATEGIES,
        "counts": dict(counts),
        "jobs": rows[:limit],
        "total": len(rows),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def _find_bash() -> str | None:
    for env_name in ("BASH", "GIT_BASH"):
        value = os.environ.get(env_name)
        if value and Path(value).exists():
            return value
    for candidate in (
        r"C:\Program Files\Git\usr\bin\bash.exe",
        r"C:\Program Files\Git\bin\bash.exe",
        r"C:\Program Files (x86)\Git\bin\bash.exe",
    ):
        if Path(candidate).exists():
            return candidate
    found = shutil.which("bash") or shutil.which("bash.exe")
    return found


def _run_tmux_script(script: str, args: list[str], timeout: int = 20) -> dict[str, object]:
    bash = _find_bash()
    script_path = TMUX_CONTROL / script
    if not bash:
        return {"ok": False, "error": "bash.exe not found"}
    if not script_path.exists():
        return {"ok": False, "error": f"tmux control script not found: {script}"}
    try:
        proc = subprocess.run(
            [bash, str(script_path), *args],
            cwd=str(ROOT),
            check=False,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return {"ok": False, "error": str(exc)}
    return {
        "ok": proc.returncode == 0,
        "returncode": proc.returncode,
        "stdout": (proc.stdout or "")[-8000:],
        "stderr": (proc.stderr or "")[-3000:],
    }


def capture_tmux_pane(pane: str = "codex", lines: int = 80) -> dict[str, object]:
    if pane not in {"codex", "claude", "status", "watcher", "pokedex"}:
        return {"available": False, "error": "unknown pane", "pane": pane}
    result = _run_tmux_script("capture_pane.sh", [pane, str(max(10, min(lines, 400)))], timeout=10)
    return {
        "available": bool(result.get("ok")),
        "pane": pane,
        "text": result.get("stdout", ""),
        "error": result.get("error") or result.get("stderr", ""),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def _crack_lane(lane_id: str) -> dict[str, object]:
    for lane in CRACK_LANES:
        if lane["id"] == lane_id:
            return lane
    return CRACK_LANES[0]


def _crack_strategy(strategy_id: str) -> dict[str, object]:
    for strategy in CRACK_STRATEGIES:
        if strategy["id"] == strategy_id:
            return strategy
    return CRACK_STRATEGIES[0]


def _find_fn_inc(src_path: Path, fn: str) -> str:
    try:
        text = src_path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        text = ""
    include_re = re.compile(rf'#include\s+"([^"]*{re.escape(fn)}[^"]*)"')
    match = include_re.search(text)
    if match:
        inc = (src_path.parent / match.group(1)).resolve()
        try:
            return inc.relative_to(ROOT).as_posix()
        except ValueError:
            return str(inc)
    guess = src_path.with_name(f"{src_path.stem}_fn_{fn.removeprefix('fn_')}.inc")
    if guess.exists():
        return guess.relative_to(ROOT).as_posix()
    return ""


def build_crack_prompt(job: dict[str, object]) -> str:
    difficulty = job.get("difficulty") if isinstance(job.get("difficulty"), dict) else {}
    diff_note = ""
    if isinstance(difficulty, dict) and difficulty:
        diff_note = (
            f"\nDifficulty catalog: {difficulty.get('section')} rank {difficulty.get('rank') or 'n/a'}, "
            f"size 0x{int_value(difficulty.get('size')):X}, current match {difficulty.get('match_pct')}%.\n"
            f"Catalog note: {difficulty.get('note')}\n"
        )
    strategy = job.get("strategy_label") or job.get("strategy")
    source = str(job.get("source") or "")
    fn = str(job.get("fn") or "")
    inc = str(job.get("inc") or "")
    gs_field_warning = ""
    if source.replace("\\", "/") == "src/game/gs_field_world.c":
        gs_field_warning = "\nWARNING: AGENTS says gs_field_world.c is owned by a live codex session. Do not edit it unless ownership has changed; report the conflict instead.\n"
    return (
        f"Dashboard crack-lab job {job.get('id')}: attack {fn} in {source}.\n"
        f"Lane: {job.get('lane_label')} ({job.get('lane_model')}). Strategy: {strategy}.\n"
        f"{diff_note}{gs_field_warning}\n"
        "Goal: improve honest decompilation/matching. Real C beats asm wrappers; never edit *_fn_*.inc; never fake-match by flipping real C back to an asm wrapper.\n\n"
        "Suggested workflow:\n"
        f"1. Inspect `{source}` around `{fn}` and sibling functions. Use existing externs/signatures first.\n"
        f"2. If present, read target include `{inc}` only as truth material; do not edit it.\n"
        f"3. Use `python tools/decompctx.py {source}` if context is needed.\n"
        f"4. Measure with `python tools/match_scan_file.py {source} {fn}`; for claimed wins, run it twice.\n"
        "5. Keep edits scoped to the source file and matching pragmas/types needed for this function.\n"
        "6. Report raw match output, whether the function compiled, and any wall evidence.\n\n"
        "If the strategy is `split giant`, first produce a slice/ownership plan and only edit after the plan is clear. "
        "If the strategy is `contenders`, make an independent full-function attempt and preserve your reasoning in the final report."
    )


def send_crack_to_codex(job: dict[str, object]) -> dict[str, object]:
    prompt = build_crack_prompt(job)
    result = _run_tmux_script("send_to_codex_tui.sh", [prompt, "--capture-first"], timeout=25)
    return result


def open_tmux_window(pane: str = "codex") -> dict[str, object]:
    if pane not in {"codex", "claude", "status", "watcher", "pokedex"}:
        return {"ok": False, "error": "unknown pane"}
    bash = _find_bash()
    if not bash:
        return {"ok": False, "error": "bash.exe not found"}
    wt = shutil.which("wt") or shutil.which("wt.exe")
    if not wt:
        for candidate in (
            Path(os.environ.get("LOCALAPPDATA", "")) / "Microsoft" / "WindowsApps" / "wt.exe",
            Path(os.environ.get("ProgramFiles", r"C:\Program Files")) / "WindowsApps" / "wt.exe",
        ):
            if candidate.exists():
                wt = str(candidate)
                break
    if not wt:
        return {"ok": False, "error": "Windows Terminal (wt.exe) not found"}
    attach_script = TMUX_CONTROL / "attach_pane.sh"
    if not attach_script.exists():
        return {"ok": False, "error": "attach_pane.sh not found"}
    try:
        subprocess.Popen(
            [wt, "new-tab", "--title", f"decomp {pane}", "-d", str(ROOT), bash, str(attach_script), pane],
            cwd=str(ROOT),
        )
    except OSError as exc:
        return {"ok": False, "error": str(exc)}
    return {"ok": True, "pane": pane}


def enqueue_crack_job(body: dict[str, object]) -> dict[str, object]:
    fn = str(body.get("fn") or body.get("function") or "").strip()
    if not fn or not FUNCTION_NAME_RE.match(fn):
        return {"ok": False, "error": "valid function name required"}
    source_in = str(body.get("source") or "").strip()
    src_path = _resolve_source_path(source_in, fn)
    if src_path is None:
        return {"ok": False, "error": f"source not found for {fn}: {source_in}"}
    try:
        source = src_path.relative_to(ROOT).as_posix()
    except ValueError:
        source = str(src_path)
    lane = _crack_lane(str(body.get("lane") or CRACK_LANES[0]["id"]))
    strategy = _crack_strategy(str(body.get("strategy") or CRACK_STRATEGIES[0]["id"]))
    now = time.time()
    hard = hard_target_for(fn)
    job = {
        "id": f"crack-{time.strftime('%Y%m%d-%H%M%S', time.gmtime(now))}-{fn}-{uuid.uuid4().hex[:6]}",
        "fn": fn,
        "source": source,
        "stem": src_path.stem,
        "inc": _find_fn_inc(src_path, fn),
        "lane": lane["id"],
        "lane_label": lane["label"],
        "lane_provider": lane["provider"],
        "lane_model": lane["model"],
        "strategy": strategy["id"],
        "strategy_label": strategy["label"],
        "status": "queued",
        "created": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime(now)),
        "created_unix": int(now),
        "difficulty": hard,
        "note": str(body.get("note") or ""),
        "origin": "dashboard",
    }
    dry_run = bool(body.get("dry_run"))
    if not dry_run:
        with _CRACK_QUEUE_LOCK:
            rows = _read_json_list(CRACK_QUEUE_JSON)
            rows.append(job)
            rows = rows[-500:]
            _write_json_list(CRACK_QUEUE_JSON, rows)
    launch = bool(body.get("launch"))
    launch_result = None
    if launch and not dry_run:
        if lane.get("launch") == "tmux-codex":
            launch_result = send_crack_to_codex(job)
            job["status"] = "launched" if launch_result.get("ok") else "launch_error"
            job["launch"] = {
                "target": "codex",
                "ok": launch_result.get("ok"),
                "returncode": launch_result.get("returncode"),
                "stderr": launch_result.get("stderr"),
                "stdout": str(launch_result.get("stdout") or "")[-1200:],
                "at": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
            }
        else:
            job["status"] = "queued"
            job["launch"] = {"ok": False, "error": f"{lane['label']} launcher is not wired yet"}
        with _CRACK_QUEUE_LOCK:
            rows = _read_json_list(CRACK_QUEUE_JSON)
            for idx, row in enumerate(rows):
                if row.get("id") == job["id"]:
                    rows[idx] = job
                    break
            _write_json_list(CRACK_QUEUE_JSON, rows)
    return {"ok": True, "job": job, "dry_run": dry_run, "launch": launch_result}


# =========================================================================== #
# Orchestrator data: SYNC / RUN / SHIP / PRS / leases / worker reports / quantum #
# =========================================================================== #
BAND_WINS_DIR = ROOT / "build" / "band_wins"
PERMUTER_STATE = ROOT / ".omc" / "permuter_state.json"

# Roster: agents that should ALWAYS appear in Agent Activity even with no live
# claim (the old view only showed agents present in claims.json -> glm/codex).
AGENT_ROSTER = ["claude", "codex", "deepseek", "glm", "mimo", "qwen", "opencode"]


def _find_gh() -> str | None:
    """Locate gh.exe — it's installed but not on PATH in this environment."""
    import shutil as _sh
    cand = _sh.which("gh")
    if cand:
        return cand
    for p in (
        Path(os.environ.get("ProgramFiles", r"C:\Program Files")) / "GitHub CLI" / "gh.exe",
        Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / "GitHub CLI" / "gh.exe",
        Path(os.environ.get("LOCALAPPDATA", "")) / "GitHubCLI" / "gh.exe",
    ):
        if p.exists():
            return str(p)
    return None


GH_EXE = _find_gh()


def _find_git() -> str:
    """Resolve git.exe absolutely — the dashboard server is launched without git
    on PATH (PowerShell Start-Process), unlike an interactive shell."""
    import shutil as _sh
    cand = _sh.which("git")
    if cand:
        return cand
    for p in (
        Path(os.environ.get("ProgramFiles", r"C:\Program Files")) / "Git" / "cmd" / "git.exe",
        Path(os.environ.get("ProgramFiles", r"C:\Program Files")) / "Git" / "bin" / "git.exe",
        Path(os.environ.get("LOCALAPPDATA", "")) / "Programs" / "Git" / "cmd" / "git.exe",
    ):
        if p.exists():
            return str(p)
    return "git"  # last resort: hope it's on PATH


GIT_EXE = _find_git()
# Put git's dir on PATH so gh (and any child) can find git even when the server
# was launched without it on PATH.
if GIT_EXE not in ("git", None) and os.path.dirname(GIT_EXE):
    os.environ["PATH"] = os.path.dirname(GIT_EXE) + os.pathsep + os.environ.get("PATH", "")

_CMD_CACHE: dict[str, tuple[float, object]] = {}


def _cached(key: str, ttl: float, producer):
    """Tiny TTL cache so per-request git/gh shell-outs don't slow the dashboard."""
    now = time.time()
    hit = _CMD_CACHE.get(key)
    if hit and hit[0] > now:
        return hit[1]
    try:
        val = producer()
    except Exception as exc:  # never let a shell-out break a panel
        val = {"available": False, "error": str(exc)}
    _CMD_CACHE[key] = (now + ttl, val)
    return val


def _git(*args: str, timeout: int = 8) -> str:
    proc = subprocess.run([GIT_EXE, *args], cwd=str(ROOT), capture_output=True,
                          text=True, timeout=timeout)
    return proc.stdout.strip()


def _gh_json(args: list[str], timeout: int = 12):
    if not GH_EXE:
        raise RuntimeError("gh CLI not found")
    env = dict(os.environ, GH_PROMPT_DISABLED="1", GH_PAGER="cat")
    proc = subprocess.run([GH_EXE, *args], cwd=str(ROOT), capture_output=True,
                          text=True, timeout=timeout, env=env)
    if proc.returncode != 0:
        raise RuntimeError((proc.stderr or "gh failed").strip()[:200])
    return json.loads(proc.stdout or "[]")


def load_sync() -> dict:
    """Git posture vs origin/master: branch, ahead/behind, dirty, last fetch."""
    def producer():
        branch = _git("rev-parse", "--abbrev-ref", "HEAD") or "detached"
        head = _git("rev-parse", "--short", "HEAD")
        # ahead/behind vs the tracked remote master
        ab = _git("rev-list", "--left-right", "--count", "origin/master...HEAD") or "0\t0"
        try:
            behind, ahead = (int(x) for x in ab.split())
        except ValueError:
            behind, ahead = 0, 0
        dirty = len([l for l in _git("status", "--porcelain").splitlines() if l.strip()])
        fetch_head = ROOT / ".git" / "FETCH_HEAD"
        last_fetch = None
        if fetch_head.exists():
            last_fetch = int(now_minus(fetch_head.stat().st_mtime))
        on_master = branch == "master"
        up = "up to date with origin/master" if (ahead == 0 and behind == 0) \
            else f"{ahead} ahead, {behind} behind origin/master"
        return {
            "available": True, "branch": branch, "head": head,
            "ahead": ahead, "behind": behind, "dirty": dirty,
            "on_master": on_master, "summary": up,
            "last_fetch_secs": last_fetch,
        }
    return _cached("sync", 20, producer)


def now_minus(ts: float) -> float:
    return time.time() - ts


def load_prs() -> dict:
    """Open + recently-merged GitHub PRs via gh."""
    def producer():
        if not GH_EXE:
            return {"available": False, "error": "gh CLI not found", "open": [], "merged": []}
        fields = "number,title,state,headRefName,isDraft,reviewDecision,updatedAt,url"
        opn = _gh_json(["pr", "list", "--state", "open", "--limit", "30", "--json", fields])
        mrg = _gh_json(["pr", "list", "--state", "merged", "--limit", "10", "--json", fields])
        repo = _cached("ghrepo", 600, lambda: _gh_json(["repo", "view", "--json", "nameWithOwner"]))
        return {
            "available": True,
            "repo": (repo or {}).get("nameWithOwner", ""),
            "open": opn, "merged": mrg,
            "open_count": len(opn), "merged_count": len(mrg),
            "draft_count": sum(1 for p in opn if p.get("isDraft")),
        }
    return _cached("prs", 45, producer)


def load_band_wins() -> tuple[int, list[dict[str, object]]]:
    confirmed = 0
    files = []
    if BAND_WINS_DIR.exists():
        for f in sorted(BAND_WINS_DIR.glob("*.json")):
            try:
                d = json.loads(f.read_text(encoding="utf-8"))
            except (OSError, ValueError):
                continue
            if not isinstance(d, dict):
                continue
            fns = [k for k in d if k != "_src"]
            confirmed += len(fns)
            if fns:
                files.append({
                    "file": f.name,
                    "source": d.get("_src", ""),
                    "count": len(fns),
                    "functions": fns[:12],
                })
    return confirmed, files


def load_ship() -> dict:
    """Handoff readiness: confirmed band wins, regressions, branch posture."""
    def producer():
        confirmed, files = load_band_wins()
        log = load_attempt_log(DECOMP_STATUS_LOG, limit=400)
        regressions = sum(1 for r in log if r.get("kind") == "regression")
        sync = load_sync()
        ready = (not sync.get("on_master")) and confirmed > 0 and regressions == 0
        if sync.get("on_master"):
            state = "blocked: master"
        elif regressions:
            state = "blocked: regressions"
        else:
            state = "pr_ready" if ready else "no wins yet"
        return {
            "available": True, "confirmed": confirmed, "regressions": regressions,
            "branch": sync.get("branch"), "on_master": sync.get("on_master"),
            "state": state, "ready": ready, "files": files,
        }
    return _cached("ship", 25, producer)


_BAND_TAG_RE = re.compile(r"band\s+([A-Za-z0-9_]+)")


def _band_tag_for(owner: str, note: str) -> str:
    """The band scratch tag for a lock. The note (e.g. "band cdx3") names it
    explicitly; otherwise the owner IS the tag (the band harness sets owner=tag)."""
    m = _BAND_TAG_RE.search(str(note or ""))
    if m:
        return m.group(1)
    return str(owner or "").strip()


def _attempt_log_cache() -> list[dict[str, object]]:
    """Cached newest-first attempt-log rows for cheap per-lease fn lookups (#5).
    25s TTL mirrors the other lease-adjacent caches so a refresh doesn't reparse
    status.md once per lock row."""
    def producer():
        return load_attempt_log(DECOMP_STATUS_LOG, limit=4000)
    return _cached("attempt_log_for_leases", 25, producer)


def derive_active_work(owner: str, note: str, file: str) -> dict[str, object]:
    """Best-effort, READ-ONLY derivation of WHICH function/file a lease is working.

    The lock row only carries owner + file + note ("band <tag>") — the active fn
    isn't recorded there. We derive it without touching band.py:

      * src_file  — from the band scratch sidecar tools/decomp_work/scratch/
                    band_<tag>.src (its "src" key, the TU the harness is banding).
                    Falls back to the lock's own `file`.
      * fn        — the most recent attempt-log line by this owner/tag that names
                    an fn_XXXXXXXX. status.md is the only place the live fn surfaces;
                    if the owner hasn't logged an fn yet this stays "" (and the
                    caller shows file+tag+age with a note instead).
      * fresh     — mtime of band_<tag>.c, so the UI can show how fresh the scratch
                    is even when no fn was logged.
    """
    tag = _band_tag_for(owner, note)
    out: dict[str, object] = {"tag": tag, "src_file": "", "fn": "", "fresh": None}
    if not tag:
        out["src_file"] = file or ""
        return out
    # source file + scratch freshness from the sidecar / scratch copy
    src_path = SCRATCH_DIR / f"band_{tag}.src"
    if src_path.exists():
        try:
            meta = json.loads(src_path.read_text(encoding="utf-8", errors="replace"))
            if isinstance(meta, dict) and meta.get("src"):
                out["src_file"] = str(meta.get("src"))
        except (OSError, json.JSONDecodeError):
            pass
    scratch_c = SCRATCH_DIR / f"band_{tag}.c"
    if scratch_c.exists():
        try:
            out["fresh"] = round(scratch_c.stat().st_mtime)
        except OSError:
            pass
    if not out["src_file"]:
        out["src_file"] = file or ""
    # active fn: newest attempt-log line for this owner/tag that names an fn_.
    log = _attempt_log_cache()
    for row in reversed(log):
        if str(row.get("agent") or "") != tag:
            continue
        fn = str(row.get("function") or "")
        if fn:
            out["fn"] = fn
            break
    return out


def load_leases() -> dict:
    """Active leases (live locks) + queued work (coordination tasks.json)."""
    locks = load_locks()
    # Per-function attempt counts from the attempt log (every `band check <fn>` logs
    # one row) — the "how much we waste per fn / tokens spent" signal.
    fn_attempts: dict[str, int] = {}
    for r in _attempt_log_cache():
        fnm = str(r.get("function") or "")
        if fnm:
            fn_attempts[fnm] = fn_attempts.get(fnm, 0) + 1
    active = []
    for lk in locks.get("locks", []):
        # #5: surface WHICH function/file the lease is working, derived read-only
        # from the band scratch sidecar + attempt log (the lock row lacks the fn).
        work = derive_active_work(lk.get("owner") or "", lk.get("note") or "", lk.get("file") or "")
        active.append({
            "scope": lk.get("scope"), "key": lk.get("key"), "owner": lk.get("owner"),
            "file": lk.get("file"), "elapsed": lk.get("age"),
            "ttl_remaining": lk.get("ttl_remaining"), "note": lk.get("note"),
            "tag": work.get("tag"), "active_src": work.get("src_file"),
            "active_fn": work.get("fn"), "scratch_mtime": work.get("fresh"),
            "attempts": fn_attempts.get(work.get("fn") or "", 0),
        })
    queued = []
    try:
        tasks = json.loads(TASKS_JSON.read_text(encoding="utf-8", errors="replace"))
    except (OSError, ValueError):
        tasks = []
    if isinstance(tasks, list):
        for t in tasks:
            if isinstance(t, dict) and t.get("status") == "queued":
                queued.append({
                    "function": t.get("function"), "description": t.get("description"),
                    "priority": t.get("priority", "normal"), "created": t.get("created"),
                })
    prio = {"high": 0, "normal": 1, "low": 2}
    queued.sort(key=lambda q: (prio.get(q.get("priority"), 1), str(q.get("created") or "")))
    return {
        "available": True, "active": active, "queued": queued,
        "active_count": len(active), "queued_count": len(queued),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def _norm_src(path: str) -> str:
    """Normalize a source path to repo-relative forward-slash form for keying."""
    p = str(path or "").replace("\\", "/").strip()
    if not p:
        return ""
    # strip an absolute prefix down to the src/... segment when present
    idx = p.find("src/")
    if idx > 0:
        p = p[idx:]
    return p


def _unit_pct_index() -> dict[str, dict[str, object]]:
    """Map normalized source_path -> live unit metrics from report.json (#3/#4).
    Cached briefly so the per-file live-% lookups don't reparse the report."""
    def producer():
        report = load_decomp_report(DECOMP_REPORT)
        index: dict[str, dict[str, object]] = {}
        for unit in report.get("units", []) if isinstance(report, dict) else []:
            if not isinstance(unit, dict):
                continue
            key = _norm_src(str(unit.get("source") or ""))
            if not key:
                continue
            index[key] = {
                "name": unit.get("name", ""),
                "source": unit.get("source", ""),
                "fuzzy_pct": unit.get("fuzzy_pct", 0),
                "code_pct": unit.get("code_pct", 0),
                "functions_pct": unit.get("functions_pct", 0),
                "matched_functions": unit.get("matched_functions", 0),
                "total_functions": unit.get("total_functions", 0),
            }
        return index
    return _cached("unit_pct_index", 20, producer)


def _scratch_active(max_age_s: int = 3 * 3600) -> list[dict[str, object]]:
    """Band scratch sidecars touched within max_age_s -> active-work signals.
    Each band_<tag>.c mtime is the freshness; band_<tag>.src "src" key is the TU
    being worked. Surfaces functional-decomp agents that skip `band init` and so
    never take a lock (the gap the brief calls out for wrk6/glm6)."""
    out: list[dict[str, object]] = []
    if not SCRATCH_DIR.exists():
        return out
    now = time.time()
    try:
        entries = list(SCRATCH_DIR.glob("band_*.c"))
    except OSError:
        return out
    for c in entries:
        try:
            mtime = c.stat().st_mtime
        except OSError:
            continue
        if now - mtime > max_age_s:
            continue
        tag = c.stem[len("band_"):]
        src = ""
        sidecar = c.with_suffix(".src")
        if sidecar.exists():
            try:
                meta = json.loads(sidecar.read_text(encoding="utf-8", errors="replace"))
                if isinstance(meta, dict):
                    src = str(meta.get("src") or "")
            except (OSError, json.JSONDecodeError):
                pass
        # Only surface scratches that map to a real src/ TU; skip integration
        # blobs / sidecar-less scratch copies that have no report unit.
        if not _norm_src(src).startswith("src/"):
            continue
        out.append({"tag": tag, "src": src, "mtime": round(mtime)})
    return out


def _recent_commit_files(limit: int = 25, max_age_s: int = 12 * 3600) -> list[dict[str, object]]:
    """Recent commits' touched src/*.c files -> active-work signals (file+author+time)."""
    text = git_value([
        "log", f"-n{limit}", "--name-only",
        "--pretty=format:%x01%H%x09%ct%x09%an%x09%s", "--", "src",
    ])
    out: list[dict[str, object]] = []
    now = time.time()
    for record in text.split("\x01"):
        record = record.strip("\n")
        if not record:
            continue
        lines = record.split("\n")
        header = lines[0].split("\t", 3)
        if len(header) != 4:
            continue
        _sha, ct, author, subject = header
        try:
            unix = int(ct)
        except ValueError:
            continue
        if now - unix > max_age_s:
            continue
        for path in lines[1:]:
            path = path.strip()
            if path.endswith(".c"):
                out.append({"src": _norm_src(path), "author": author, "unix": unix, "subject": subject})
    return out


def load_active_work() -> dict:
    """Unified 'who is working on what' view (#3 + #4).

    Unions three independent live signals so an agent shows up even when it skips
    `band init` (and so holds no lock):
      1. locks.db leases       -> owner + file + note + age   (load_leases active)
      2. band scratch sidecars -> tag + src TU + scratch mtime (recently touched)
      3. recent src commits    -> file + author + commit time

    Entries are keyed by normalized source file and decorated with the live unit
    match% from report.json (matched/total fns, fuzzy/code %), so the front-end
    can show a per-file LIVE percentage that moves as the agents land matches.
    """
    pct_index = _unit_pct_index()
    by_src: dict[str, dict[str, object]] = {}

    def touch(src: str) -> dict[str, object] | None:
        key = _norm_src(src)
        if not key:
            return None
        ent = by_src.get(key)
        if ent is None:
            metrics = pct_index.get(key, {})
            ent = {
                "src": key,
                "label": source_label(key),
                "signals": [],       # which evidence sources flagged this file
                "owners": [],        # tags/agents/authors seen working it
                "fn": "",            # best-effort active function
                "fresh": 0,          # newest unix across all signals
                "lease": False,
                "unit": metrics or None,
            }
            by_src[key] = ent
        return ent

    # 1) live leases (band-lock holders)
    leases = load_leases()
    for lk in leases.get("active", []):
        src = lk.get("active_src") or lk.get("file") or ""
        ent = touch(src)
        if ent is None:
            continue
        ent["lease"] = True
        if "lease" not in ent["signals"]:
            ent["signals"].append("lease")
        owner = str(lk.get("owner") or lk.get("tag") or "")
        if owner and owner not in ent["owners"]:
            ent["owners"].append(owner)
        if lk.get("active_fn") and not ent["fn"]:
            ent["fn"] = lk.get("active_fn")
        mt = int_value(lk.get("scratch_mtime"))
        ent["fresh"] = max(int_value(ent["fresh"]), mt)

    # 2) band scratch sidecars (catches lock-less functional-decomp agents)
    for s in _scratch_active():
        ent = touch(s.get("src") or "")
        if ent is None:
            continue
        if "scratch" not in ent["signals"]:
            ent["signals"].append("scratch")
        tag = str(s.get("tag") or "")
        if tag and tag not in ent["owners"]:
            ent["owners"].append(tag)
        ent["fresh"] = max(int_value(ent["fresh"]), int_value(s.get("mtime")))

    # 3) recent commits' touched files
    for c in _recent_commit_files():
        ent = touch(c.get("src") or "")
        if ent is None:
            continue
        if "commit" not in ent["signals"]:
            ent["signals"].append("commit")
        author = str(c.get("author") or "")
        if author and author not in ent["owners"]:
            ent["owners"].append(author)
        ent["fresh"] = max(int_value(ent["fresh"]), int_value(c.get("unix")))

    items = list(by_src.values())
    # leases first, then freshest; lets the decomp-detail "active files" list lead
    # with what's being worked RIGHT NOW.
    items.sort(key=lambda e: (0 if e.get("lease") else 1, -int_value(e.get("fresh"))))
    return {
        "available": bool(items),
        "active": items,
        "count": len(items),
        "lease_count": sum(1 for e in items if e.get("lease")),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def _report_status(row: dict) -> str:
    """Classify an attempt-log entry into an orchestrator-style worker status."""
    kind = row.get("kind")
    msg = str(row.get("message", "")).upper()
    pct = row.get("percent")
    if kind == "regression" or "REGRESSION" in msg:
        return "needs rework"
    if kind == "match" or "MATCH!" in msg or (pct is not None and pct >= 100):
        return "exact"
    if kind == "commit" or "COMMIT" in msg:
        return "committed"
    if any(w in msg for w in ("ERROR", "FAILED", "TIMEOUT")):
        return "tool error"
    if "->" in msg or (pct is not None and pct > 0):
        return "improved"
    return "no progress"


def load_reports(limit: int = 60) -> dict:
    """Worker reports: most-recent attempt per (agent, function/unit) with a status.

    Surfaces genuinely-recent activity. merged_attempt_log() already returns rows
    newest-first (status.md + git commits, sorted by timestamp desc), so:
      * per (agent, fn/unit) key we keep the NEWEST row — the FIRST occurrence in a
        newest-first list — instead of the old code's last-wins (which kept the
        OLDEST and let stale April "dashboard REGRESSION" rows survive);
      * the deduped reports are then sorted STRICTLY by timestamp desc before the
        [:limit] slice, so the newest rows are guaranteed to be the ones shown
        (the old [-limit:] over dict-insertion order let old unique keys leak in).
    """
    merged = merged_attempt_log(limit=1500)
    seen: dict[tuple, dict] = {}
    for row in merged:
        agent = str(row.get("agent") or "?")
        fn = str(row.get("function") or row.get("file") or "")
        key = (agent, fn or row.get("message", "")[:40])
        row_unix = int_value(row.get("unix")) or timestamp_unix(row.get("timestamp"))
        rep = {
            "agent": agent,
            "function": row.get("function") or "",
            "file": row.get("file") or "",
            "percent": row.get("percent"),
            "status": _report_status(row),
            "message": row.get("message", ""),
            "timestamp": row.get("timestamp"),
            "unix": row_unix,
        }
        prev = seen.get(key)
        # Keep the newest row for each key. merged is newest-first so the first
        # seen is usually newest, but guard explicitly on unix in case of ties.
        if prev is None or row_unix > int_value(prev.get("unix")):
            seen[key] = rep
    # Strict newest-first ordering BEFORE the limit slice (this is the actual fix
    # for the April-data bug: don't let dict-insertion order pick the survivors).
    reports = sorted(
        seen.values(),
        key=lambda r: (int_value(r.get("unix")), str(r.get("timestamp") or "")),
        reverse=True,
    )[:limit]
    counts: dict[str, int] = {}
    for r in reports:
        counts[r["status"]] = counts.get(r["status"], 0) + 1
    return {"available": bool(reports), "reports": reports, "counts": counts,
            "total": len(reports), "generated_at": time.strftime("%Y-%m-%d %H:%M:%S")}


def load_quantum() -> dict:
    """Surface the decomp-permuter annealing swarm state (quantum_dash data)."""
    if not PERMUTER_STATE.exists():
        return {"available": False}
    try:
        d = json.loads(PERMUTER_STATE.read_text(encoding="utf-8", errors="replace"))
    except (OSError, ValueError) as exc:
        return {"available": False, "error": str(exc)}
    if not isinstance(d, dict):
        return {"available": False}
    return {"available": True, "state": d, "generated_at": time.strftime("%Y-%m-%d %H:%M:%S")}


MEASURE_CACHE = ROOT / "build" / "measure_cache.jsonl"


def load_measure_cache():
    """FRESH per-fn pct from build/measure_cache.jsonl (band.py appends every measurement).
    Folds the append-only log to the latest line per src. Returns ({fn: pct}, newest_ts).
    This is the single fresh source that lets buckets reflect reality between the slow
    `wall_ledger.py build` rebuilds (the staleness that froze the DONE count this session)."""
    latest = {}
    try:
        with open(MEASURE_CACHE, encoding="utf-8", errors="replace") as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                try:
                    rec = json.loads(line)
                except ValueError:
                    continue
                s = rec.get("src")
                if s and (s not in latest or rec.get("ts", 0) >= latest[s].get("ts", 0)):
                    latest[s] = rec
    except OSError:
        return {}, 0.0
    fresh, newest = {}, 0.0
    for rec in latest.values():
        ts = rec.get("ts", 0.0)
        newest = max(newest, ts)
        for fn, pct in (rec.get("pcts") or {}).items():
            if fn not in fresh or ts >= fresh[fn][1]:
                fresh[fn] = (float(pct), ts)
    return {fn: p for fn, (p, _ts) in fresh.items()}, newest


def _pct_bucket(p):
    if p >= 99.95:
        return "DONE"
    if p >= 95.0:
        return "NEARWALL"
    if p >= 70.0:
        return "STRUCT"
    return "LOW"


def load_buckets() -> dict:
    """Wall-ledger bucket coverage: per-bucket totals + how many functions we've
    ATTACKED (attempted), for the campaign progress bar chart."""
    led_path = ROOT / "build" / "wall_ledger.json"
    if not led_path.exists():
        return {"available": False}
    try:
        led = json.loads(led_path.read_text(encoding="utf-8", errors="replace"))
    except (OSError, ValueError) as exc:
        return {"available": False, "error": str(exc)}
    order = ["DONE", "EQUIV", "NEARWALL", "STRUCT", "ASM", "LOW"]
    meta = {
        "DONE": "byte-exact real C (achieved)",
        "EQUIV": "correct C, wall-accepted",
        "NEARWALL": "95-99.95% reg-alloc walls (crack targets)",
        "STRUCT": "70-95% wrong-shape (rework)",
        "ASM": "undecompiled asm-wrappers",
        "LOW": "<70% early/wrong",
    }
    # "Attacked" must be LIVE: the ledger `attempted` flag only refreshes on a full
    # `wall_ledger.py build` (which doesn't run each cycle), so recompute the same way the
    # queue (gen_bucket_queue) decides a fn is no longer fresh — attempted OR already SAVED
    # (band_wins) OR re-ground (dispatched >=2x in wall_attempts.txt). Recomputed every call
    # so the bars track reality instead of freezing at the last build.
    _FN = re.compile(r"fn_[0-9A-Fa-f]+")
    saved: set = set()
    _bw = ROOT / "build" / "band_wins"
    if _bw.is_dir():
        for _f in _bw.glob("*.json"):
            try:
                saved.update(_FN.findall(_f.read_text(encoding="utf-8", errors="replace")))
            except OSError:
                pass
    reground: set = set()
    _wap = ROOT / "build" / "wall_attempts.txt"
    if _wap.exists():
        try:
            _c = Counter(_FN.findall(_wap.read_text(encoding="utf-8", errors="replace")))
            reground = {fn for fn, n in _c.items() if n >= 2}
        except OSError:
            pass
    fresh, fresh_ts = load_measure_cache()
    agg = {b: {"total": 0, "attempted": 0} for b in order}
    for fn, v in led.items():
        if not isinstance(v, dict):
            continue
        b = v.get("bucket")
        # LIVE re-bucketing of wins: a fn with a band_wins record is a byte-exact 100%
        # real-C win, so it belongs in DONE even when the ledger's stored bucket (which
        # only refreshes on a full `wall_ledger.py build`) still lists it under its
        # pre-win bucket. Without this the DONE count freezes between rebuilds and
        # hundreds of already-won fns look stuck in NEARWALL/STRUCT/LOW.
        if fn in saved and b in agg and b not in ("DONE", "EQUIV"):
            b = "DONE"
        elif b in ("NEARWALL", "STRUCT", "LOW") and fn in fresh:
            # FRESH re-bucket from the measure cache (band.py writes every measurement):
            # reflect the BEST measured pct (committed OR latest scratch) immediately
            # instead of waiting for the next ledger rebuild. Up-only (max) so a transient
            # mid-edit scratch dip never regresses the display; a >=99.95% scratch that is
            # NOT a confirmed saved win caps at NEARWALL — only a band_win earns DONE, so
            # unsaved/transient 100%s can't inflate the headline.
            best = max(float(v.get("pct") or 0.0), fresh[fn])
            nb = _pct_bucket(best)
            b = "NEARWALL" if (nb == "DONE" and fn not in saved) else nb
        if b in agg:
            agg[b]["total"] += 1
            if v.get("attempted") or fn in saved or fn in reground:
                agg[b]["attempted"] += 1
    buckets = []
    for b in order:
        t, a = agg[b]["total"], agg[b]["attempted"]
        if b in ("DONE", "EQUIV"):
            # these are finished — the bar is "% complete" (100), not "% attacked".
            buckets.append({"name": b, "total": t, "attempted": t, "remaining": 0,
                            "pct": 100.0 if t else 0.0, "desc": meta[b]})
        else:
            buckets.append({"name": b, "total": t, "attempted": a, "remaining": t - a,
                            "pct": round(100.0 * a / t, 1) if t else 0.0, "desc": meta[b]})
    total = sum(agg[b]["total"] for b in order)
    done = agg["DONE"]["total"] + agg["EQUIV"]["total"]
    # also surface the ROM function-MATCH headline (report.json), which counts
    # asm-wrappers that match the target bytes — so the two numbers are explained.
    match_fns = match_total = 0
    try:
        m = json.loads(DECOMP_REPORT.read_text(encoding="utf-8", errors="replace")).get("measures", {})
        match_fns = int(float(m.get("matched_functions", 0) or 0))
        match_total = int(float(m.get("total_functions", 0) or 0))
    except (OSError, ValueError, AttributeError):
        pass
    return {
        "available": True, "buckets": buckets, "total_fns": total, "done_fns": done,
        "overall_pct": round(100.0 * done / total, 1) if total else 0.0,
        "match_fns": match_fns, "match_total": match_total,
        "match_pct": round(100.0 * match_fns / match_total, 1) if match_total else 0.0,
        "measure_fresh_age": round(time.time() - fresh_ts) if fresh_ts else None,
        "measure_fresh_fns": len(fresh),
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def load_attack_matrix(decomp: dict[str, object]) -> dict[str, object]:
    """Rank work by unit so the dashboard directs the next attack, not just observes."""
    units = decomp.get("units") if isinstance(decomp, dict) else []
    if not isinstance(units, list):
        units = []
    enriched = []
    for unit in units:
        if not isinstance(unit, dict):
            continue
        counts = unit.get("function_status", {})
        if not isinstance(counts, dict):
            counts = {}
        total_fns = int_value(unit.get("total_functions", 0))
        matched_fns = int_value(unit.get("matched_functions", 0))
        open_fns = max(0, total_fns - matched_fns)
        open_code = max(0, int_value(unit.get("total_code", 0)) - int_value(unit.get("matched_code", 0)))
        near = int_value(counts.get("near", 0))
        partial = int_value(counts.get("partial", 0))
        missing = int_value(counts.get("missing", 0))
        if open_fns <= 0 and open_code <= 0:
            continue
        score = near * 12 + partial * 5 + missing * 2 + min(open_code / 2000.0, 20.0)
        enriched.append({
            "name": unit.get("name", ""),
            "source": unit.get("source", ""),
            "label": source_label(str(unit.get("source") or unit.get("name") or "")),
            "code_pct": float_pct(unit.get("code_pct", 0)),
            "fuzzy_pct": float_pct(unit.get("fuzzy_pct", 0)),
            "functions_pct": float_pct(unit.get("functions_pct", 0)),
            "open_fns": open_fns,
            "open_code": open_code,
            "near": near,
            "partial": partial,
            "missing": missing,
            "score": round(score, 1),
        })

    def top_rows(predicate, key, limit=5):
        return sorted([r for r in enriched if predicate(r)], key=key)[:limit]

    near_targets = top_rows(lambda r: r["near"] > 0, lambda r: (-int(r["near"]), -float(r["fuzzy_pct"]), -int(r["open_code"])))
    bulk_targets = top_rows(lambda r: r["missing"] + r["partial"] > 0, lambda r: (-int(r["open_code"]), -int(r["open_fns"])))
    finish_targets = top_rows(lambda r: r["functions_pct"] >= 80 and r["open_fns"] > 0, lambda r: (int(r["open_fns"]), -float(r["functions_pct"])))
    lanes = [
        {
            "id": "near",
            "title": "Near-match finishers",
            "count": sum(int(r["near"]) for r in enriched),
            "note": "90-99.95% functions; highest ROI for byte-exact wins",
            "command": "python tools/decomp_work/kg/bestof.py targets 12 --real-c-only",
            "targets": near_targets,
        },
        {
            "id": "bulk",
            "title": "Bulk code-percent files",
            "count": sum(int(r["open_code"]) for r in enriched),
            "note": "largest unmatched byte pools; improves code percent fastest",
            "command": "python tools/decomp_work/progress2.py --measure",
            "targets": bulk_targets,
        },
        {
            "id": "close",
            "title": "Unit closeouts",
            "count": len(finish_targets),
            "note": "fewest functions left in already-strong units",
            "command": "python tools/match_scan_file.py <src/file.c>",
            "targets": finish_targets,
        },
    ]
    try:
        leases = load_leases()
    except Exception:
        leases = {}
    try:
        reports = load_reports(limit=30)
    except Exception:
        reports = {}
    try:
        ship = load_ship()
    except Exception:
        ship = {}
    pipeline = [
        {"stage": "Scout", "count": sum(1 for r in enriched if int(r["near"]) > 0), "note": "KG / report targets"},
        {"stage": "Claim", "count": int_value(leases.get("queued_count", 0)), "note": "queued work"},
        {"stage": "Work", "count": int_value(leases.get("active_count", 0)), "note": "active leases"},
        {"stage": "Verify", "count": int_value((reports.get("counts") or {}).get("exact", 0)) if isinstance(reports.get("counts"), dict) else 0, "note": "exact reports"},
        {"stage": "Ship", "count": int_value(ship.get("confirmed", 0)), "note": ship.get("state", "")},
    ]
    return {
        "available": bool(enriched),
        "lanes": lanes,
        "pipeline": pipeline,
        "top_units": sorted(enriched, key=lambda r: (-float(r["score"]), -int(r["open_code"])))[:12],
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def load_kg() -> dict[str, object]:
    """Summarize the SQLite decomp knowledge graph for the web cockpit."""
    import sqlite3

    if not KG_DB.exists():
        return {"available": False, "error": f"not found: {KG_DB}"}
    try:
        conn = sqlite3.connect(str(KG_DB), timeout=5.0)
        conn.row_factory = sqlite3.Row
        def has_table(name: str) -> bool:
            return conn.execute(
                "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?",
                (name,),
            ).fetchone() is not None

        def scalar(sql: str, default: int = 0) -> int:
            try:
                row = conn.execute(sql).fetchone()
            except sqlite3.Error:
                return default
            return int(row[0]) if row and row[0] is not None else default

        counts = {
            "functions": scalar("SELECT count(*) FROM functions"),
            "levers": scalar("SELECT count(*) FROM levers"),
            "cracked_edges": scalar("SELECT count(*) FROM cracked_by"),
            "externals": scalar("SELECT count(*) FROM externals"),
            "walls": scalar("SELECT count(*) FROM walls"),
            "near": scalar("SELECT count(*) FROM functions WHERE byte_pct >= 90 AND byte_pct < 99.95"),
            "calls": scalar("SELECT count(*) FROM function_calls") if has_table("function_calls") else 0,
            "calltags": scalar("SELECT count(*) FROM function_tags") if has_table("function_tags") else 0,
            "name_evidence": scalar("SELECT count(*) FROM name_evidence") if has_table("name_evidence") else 0,
        }
        top_levers = [
            dict(row) for row in conn.execute(
                """
                SELECT l.slug, l.title, l.opt_gated, count(c.id) AS cracks
                FROM levers l
                LEFT JOIN cracked_by c ON c.lever_slug = l.slug
                GROUP BY l.slug
                ORDER BY cracks DESC, l.slug
                LIMIT 8
                """
            )
        ]
        wall_load = [
            dict(row) for row in conn.execute(
                """
                SELECT coalesce(nullif(wall_class, ''), '(none)') AS wall,
                       count(*) AS n,
                       round(avg(byte_pct), 1) AS avg_pct
                FROM functions
                WHERE status != 'DONE'
                GROUP BY wall
                ORDER BY n DESC, wall
                LIMIT 8
                """
            )
        ]
        targets = [
            dict(row) for row in conn.execute(
                """
                SELECT addr, tu, round(byte_pct, 2) AS pct,
                       status, coalesce(wall_class, '') AS wall_class
                FROM functions
                WHERE byte_pct >= 90 AND byte_pct < 99.95
                ORDER BY byte_pct DESC, addr
                LIMIT 12
                """
            )
        ]
        calltags = []
        name_evidence = []
        call_edges = []
        relationship_graph = {"nodes": [], "edges": []}
        if has_table("function_tags"):
            calltags = [
                dict(row) for row in conn.execute(
                    """
                    SELECT tag, confidence, count(*) AS functions,
                           round(avg(score), 2) AS avg_score
                    FROM function_tags
                    WHERE kind='calltag'
                    GROUP BY tag, confidence
                    ORDER BY avg_score DESC, functions DESC, tag
                    LIMIT 10
                    """
                )
            ]
        if has_table("name_evidence"):
            name_evidence = [
                dict(row) for row in conn.execute(
                    """
                    SELECT symbol, candidate, source, confidence,
                           round(score, 2) AS score, evidence
                    FROM name_evidence
                    WHERE source IN ('calltag','tu','tu-neighbor','symbolmap-proposed','symbolmap-applied')
                    ORDER BY score DESC, confidence DESC, symbol
                    LIMIT 12
                    """
                )
            ]
        if has_table("function_calls"):
            call_edges = [
                dict(row) for row in conn.execute(
                    """
                    SELECT caller, callee, caller_tu, callee_tu,
                           confidence, source, evidence
                    FROM function_calls
                    ORDER BY
                      CASE confidence WHEN 'high' THEN 0 WHEN 'medium' THEN 1 ELSE 2 END,
                      CASE source WHEN 'src-c' THEN 0 ELSE 1 END,
                      caller, callee
                    LIMIT 16
                    """
                )
            ]
            graph_edges = [
                dict(row) for row in conn.execute(
                    """
                    SELECT caller, callee, caller_tu, callee_tu,
                           confidence, source
                    FROM function_calls
                    WHERE caller_tu IS NOT NULL AND caller_tu != ''
                    ORDER BY
                      CASE confidence WHEN 'high' THEN 0 WHEN 'medium' THEN 1 ELSE 2 END,
                      caller, callee
                    LIMIT 24
                    """
                )
            ]
            nodes: dict[str, dict[str, object]] = {}
            graph: list[dict[str, object]] = []
            for row in graph_edges:
                caller = str(row.get("caller") or "")
                callee = str(row.get("callee") or "")
                caller_tu = str(row.get("caller_tu") or "")
                callee_tu = str(row.get("callee_tu") or "")
                for tu, fn in ((caller_tu, caller), (callee_tu, callee)):
                    if tu:
                        nodes.setdefault(f"tu:{tu}", {
                            "id": f"tu:{tu}", "label": tu.rsplit("/", 1)[-1],
                            "kind": "class", "tu": tu,
                        })
                    if fn:
                        nodes.setdefault(fn, {
                            "id": fn, "label": fn,
                            "kind": "named" if not fn.startswith("fn_") else "function",
                            "tu": tu,
                        })
                    if tu and fn:
                        graph.append({"from": f"tu:{tu}", "to": fn, "kind": "contains", "confidence": "high"})
                if caller and callee:
                    graph.append({
                        "from": caller, "to": callee, "kind": "calls",
                        "confidence": row.get("confidence") or "", "source": row.get("source") or "",
                    })
            relationship_graph = {"nodes": list(nodes.values())[:48], "edges": graph[:72]}
        tu_rollup = [
            dict(row) for row in conn.execute(
                """
                SELECT tu,
                       count(*) AS total,
                       sum(CASE WHEN status = 'DONE' THEN 1 ELSE 0 END) AS done,
                       sum(CASE WHEN byte_pct >= 90 AND byte_pct < 99.95 THEN 1 ELSE 0 END) AS near,
                       sum(CASE WHEN status = 'WALL' THEN 1 ELSE 0 END) AS walls,
                       round(avg(byte_pct), 1) AS avg_pct
                FROM functions
                GROUP BY tu
                ORDER BY near DESC, total DESC
                LIMIT 8
                """
            )
        ]
        conn.close()
    except sqlite3.Error as exc:
        return {"available": False, "error": str(exc)}
    return {
        "available": True,
        "counts": counts,
        "top_levers": top_levers,
        "wall_load": wall_load,
        "targets": targets,
        "calltags": calltags,
        "name_evidence": name_evidence,
        "call_edges": call_edges,
        "relationship_graph": relationship_graph,
        "tu_rollup": tu_rollup,
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def prepare_handoff() -> dict:
    """Push the current (non-master) branch and open/show its GitHub PR.

    Enforces the PR workflow: refuses to operate from master (agents must branch).
    Commits nothing — only pushes existing commits and opens the PR for review."""
    branch = _git("rev-parse", "--abbrev-ref", "HEAD")
    if branch == "master":
        return {"ok": False, "error": "On master — create a feature branch first; "
                "agents must commit to a branch and open a PR, not master."}
    if not GH_EXE:
        return {"ok": False, "error": "gh CLI not found"}
    try:
        ahead = int(_git("rev-list", "--count", "origin/master..HEAD") or "0")
    except ValueError:
        ahead = 0
    if ahead == 0:
        return {"ok": False, "error": f"Branch '{branch}' has no commits ahead of origin/master."}
    env = dict(os.environ, GH_PROMPT_DISABLED="1", GH_PAGER="cat")
    try:
        push = subprocess.run([GIT_EXE, "push", "-u", "origin", branch], cwd=str(ROOT),
                              capture_output=True, text=True, timeout=120)
        if push.returncode != 0:
            return {"ok": False, "error": ("git push failed: " + (push.stderr or ""))[:240]}
        view = subprocess.run([GH_EXE, "pr", "view", "--json", "url", "-q", ".url"],
                              cwd=str(ROOT), capture_output=True, text=True, timeout=30, env=env)
        url = (view.stdout or "").strip()
        if not url:
            create = subprocess.run([GH_EXE, "pr", "create", "--fill", "--head", branch],
                                    cwd=str(ROOT), capture_output=True, text=True, timeout=60, env=env)
            url = ""
            for line in (create.stdout or "").splitlines():
                if line.startswith("http"):
                    url = line.strip()
            if create.returncode != 0 and not url:
                return {"ok": False, "error": (create.stderr or "pr create failed")[:240]}
        for k in ("prs", "sync", "ship"):
            _CMD_CACHE.pop(k, None)
        return {"ok": True, "branch": branch, "ahead": ahead, "url": url}
    except Exception as exc:
        return {"ok": False, "error": str(exc)}


# =========================================================================== #
# v10: token-expense-over-time — multi-source hourly collector                #
#   (a) CLAUDE   ~/.claude/projects/<repo>/**/*.jsonl  (primary, richest)     #
#   (b) OPENCODE WSL sqlite (glm/qwen/kimi/deepseek/...) tagged by model      #
#   (c) CODEX    ~/.codex/history.jsonl  (activity event count, no tokens)    #
#   + .omc/agent_tokens.json cumulative fallback totals                       #
# Aggregated buckets persist to tools/decomp_work/token_history.json so the   #
# chart survives a source rotating; refreshed by the background auto-loop.    #
# =========================================================================== #
SOURCE_KEYS = ("claude", "opencode", "codex", "glm")
# Model -> chart source bucket. Anything unmatched falls through to "opencode"
# (every model in the WSL db is an opencode-run worker).
_OPENCODE_MODEL_TAGS = ("mimo", "deepseek", "glm", "qwen", "kimi", "nemotron")
try:
    OPENCODE_PROBE_TIMEOUT_SECONDS = float(os.environ.get("DASH_OPENCODE_TIMEOUT_SECONDS", "8"))
except ValueError:
    OPENCODE_PROBE_TIMEOUT_SECONDS = 8.0


def _new_bucket() -> dict[str, float]:
    b: dict[str, float] = {k: 0.0 for k in SOURCE_KEYS}
    b["claude_cache_read"] = 0.0
    b["codex_cache_read"] = 0.0
    b["codex_events"] = 0.0
    b["glm_cache_read"] = 0.0
    return b


def _collect_claude(buckets: dict[int, dict[str, float]], cutoff: float,
                    by_model: dict[str, float]) -> dict[str, object]:
    """Sum input+output (and cache_read separately) per hour from the Claude
    project journals. One JSONL line per turn; only lines with `"usage"` count."""
    if not CLAUDE_PROJECT_DIR.exists():
        return {"available": False, "files": 0, "total": 0,
                "reason": f"claude dir not found: {CLAUDE_PROJECT_DIR}"}
    files = 0
    total = 0
    for path in CLAUDE_PROJECT_DIR.rglob("*.jsonl"):
        files += 1
        try:
            with open(path, encoding="utf-8", errors="replace") as fh:
                for line in fh:
                    if '"usage"' not in line:
                        continue
                    try:
                        d = json.loads(line)
                    except json.JSONDecodeError:
                        continue
                    msg = d.get("message")
                    if not isinstance(msg, dict):
                        continue
                    usage = msg.get("usage")
                    if not isinstance(usage, dict):
                        continue
                    ts = _parse_iso(d.get("timestamp"))
                    if not ts or ts < cutoff:
                        continue
                    inp = int(usage.get("input_tokens") or 0)
                    out = int(usage.get("output_tokens") or 0)
                    cr = int(usage.get("cache_read_input_tokens") or 0)
                    hour = int(ts // 3600) * 3600
                    b = buckets.setdefault(hour, _new_bucket())
                    b["claude"] += inp + out
                    b["claude_cache_read"] += cr
                    by_model[str(msg.get("model") or "?")] += inp + out
                    total += inp + out
        except OSError:
            continue
    return {"available": total > 0, "files": files, "total": total}


def _collect_glm(buckets: dict[int, dict[str, float]], cutoff: float,
                 by_model: dict[str, float]) -> dict[str, object]:
    """Same as _collect_claude but for the GLM lane's isolated config dir
    (~/.claude-glm). GLM is Claude Code pinned to glm-5.2 via the proxy, so its
    journals use the identical JSONL `usage` schema. Bucketed into the `glm` source."""
    if not GLM_PROJECT_DIR.exists():
        return {"available": False, "files": 0, "total": 0,
                "reason": f"glm dir not found: {GLM_PROJECT_DIR}"}
    files = 0
    total = 0
    for path in GLM_PROJECT_DIR.rglob("*.jsonl"):
        files += 1
        try:
            with open(path, encoding="utf-8", errors="replace") as fh:
                for line in fh:
                    if '"usage"' not in line:
                        continue
                    try:
                        d = json.loads(line)
                    except json.JSONDecodeError:
                        continue
                    msg = d.get("message")
                    if not isinstance(msg, dict):
                        continue
                    usage = msg.get("usage")
                    if not isinstance(usage, dict):
                        continue
                    ts = _parse_iso(d.get("timestamp"))
                    if not ts or ts < cutoff:
                        continue
                    inp = int(usage.get("input_tokens") or 0)
                    out = int(usage.get("output_tokens") or 0)
                    cr = int(usage.get("cache_read_input_tokens") or 0)
                    hour = int(ts // 3600) * 3600
                    b = buckets.setdefault(hour, _new_bucket())
                    b["glm"] += inp + out
                    b["glm_cache_read"] += cr
                    by_model[str(msg.get("model") or "glm")] += inp + out
                    total += inp + out
        except OSError:
            continue
    return {"available": total > 0, "files": files, "total": total}


def _collect_opencode(buckets: dict[int, dict[str, float]], cutoff: float,
                      by_model: dict[str, float]) -> dict[str, object]:
    """Query the WSL opencode.db from WSL's own python3 (avoids WAL/lock issues).
    Each assistant message's `data` blob carries tokens.{input,output} + modelID
    and time.created (unix ms). Tag by model; bucket by hour."""
    script = (
        "import sqlite3,os,json,sys\n"
        "p=os.path.expanduser('~/.local/share/opencode/opencode.db')\n"
        "if not os.path.exists(p):\n"
        "    print(json.dumps({'available':False,'reason':'opencode.db not found'}));sys.exit()\n"
        "con=sqlite3.connect('file:%s?mode=ro'%p,uri=True)\n"
        "rows=[]\n"
        "for tc,data in con.execute('SELECT time_created,data FROM message'):\n"
        "    try: dd=json.loads(data)\n"
        "    except Exception: continue\n"
        "    if dd.get('role')!='assistant': continue\n"
        "    tk=dd.get('tokens') or {}\n"
        "    inp=int(tk.get('input') or 0); out=int(tk.get('output') or 0)\n"
        "    if inp+out<=0: continue\n"
        "    t=(dd.get('time') or {}).get('created') or tc\n"
        "    rows.append({'created':t,'model':dd.get('modelID') or '?','tok':inp+out})\n"
        "print(json.dumps({'available':True,'rows':rows}))\n"
    )
    try:
        proc = subprocess.run(
            ["wsl", "bash", "-lc", "python3 -"],
            input=script, capture_output=True, text=True, timeout=OPENCODE_PROBE_TIMEOUT_SECONDS,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return {"available": False, "reason": f"wsl call failed: {exc}"}
    if proc.returncode != 0:
        return {"available": False, "reason": "wsl python3 failed: " + proc.stderr[:160].strip()}
    try:
        data = json.loads(proc.stdout.strip().splitlines()[-1])
    except (json.JSONDecodeError, IndexError):
        return {"available": False, "reason": "opencode probe returned non-JSON"}
    if not data.get("available"):
        return data
    total = 0
    rows = data.get("rows") or []
    for r in rows:
        created = r.get("created") or 0
        if created > 1e12:  # ms -> s
            created = created / 1000.0
        if not created or created < cutoff:
            continue
        hour = int(created // 3600) * 3600
        b = buckets.setdefault(hour, _new_bucket())
        b["opencode"] += int(r.get("tok") or 0)
        by_model[str(r.get("model") or "?")] += int(r.get("tok") or 0)
        total += int(r.get("tok") or 0)
    return {"available": total > 0, "total": total, "rows": len(rows)}


def _path_is_in_repo(value: object) -> bool:
    if not isinstance(value, str) or not value:
        return False
    try:
        root = os.path.normcase(str(ROOT.resolve()))
        candidate = os.path.normcase(os.path.abspath(value))
    except (OSError, ValueError):
        return False
    return candidate == root or candidate.startswith(root + os.sep)


def _codex_payload_matches_repo(payload: dict[str, object]) -> bool:
    if _path_is_in_repo(payload.get("cwd")):
        return True
    roots = payload.get("workspace_roots")
    if not isinstance(roots, list):
        return False
    for item in roots:
        if isinstance(item, str) and _path_is_in_repo(item):
            return True
        if isinstance(item, dict):
            for key in ("root", "path", "cwd"):
                if _path_is_in_repo(item.get(key)):
                    return True
    return False


def _codex_payload_model(payload: dict[str, object]) -> str:
    model = payload.get("model")
    if isinstance(model, str) and model:
        return model
    mode = payload.get("collaboration_mode")
    if isinstance(mode, dict):
        settings = mode.get("settings")
        if isinstance(settings, dict):
            model = settings.get("model")
            if isinstance(model, str) and model:
                return model
    return ""


def _usage_total_tokens(usage: dict[str, object]) -> int:
    total = int_value(usage.get("total_tokens"))
    if total > 0:
        return total
    return int_value(usage.get("input_tokens")) + int_value(usage.get("output_tokens"))


def _collect_codex(buckets: dict[int, dict[str, float]], cutoff: float,
                   by_model: dict[str, float]) -> dict[str, object]:
    """Bucket Codex prompt activity plus real token usage from session JSONL logs.

    history.jsonl still only has prompt events; recent session rollouts also emit
    payload.info.last_token_usage after each model response, which is the per-turn
    token count we can safely sum without double-counting cumulative totals.
    """
    events = 0
    history_reason = ""
    if CODEX_HISTORY.exists():
        try:
            with open(CODEX_HISTORY, encoding="utf-8", errors="replace") as fh:
                for line in fh:
                    try:
                        d = json.loads(line)
                    except json.JSONDecodeError:
                        continue
                    ts = d.get("ts")
                    if not isinstance(ts, (int, float)) or ts < cutoff:
                        continue
                    hour = int(ts // 3600) * 3600
                    b = buckets.setdefault(hour, _new_bucket())
                    b["codex_events"] += 1
                    events += 1
        except OSError:
            history_reason = "codex history unreadable"
    else:
        history_reason = "codex history not found"

    files = 0
    project_files = 0
    token_events = 0
    total = 0
    if CODEX_SESSIONS_DIR.exists():
        for path in CODEX_SESSIONS_DIR.rglob("*.jsonl"):
            try:
                stat = path.stat()
            except OSError:
                continue
            if stat.st_mtime < cutoff - 86400:
                continue
            files += 1
            project_session = None
            counted_project_file = False
            current_model = "codex"
            try:
                with open(path, encoding="utf-8", errors="replace") as fh:
                    for line in fh:
                        try:
                            d = json.loads(line)
                        except json.JSONDecodeError:
                            continue
                        payload = d.get("payload")
                        if not isinstance(payload, dict):
                            continue
                        if d.get("type") in ("session_meta", "turn_context"):
                            if _codex_payload_matches_repo(payload):
                                project_session = True
                                if not counted_project_file:
                                    project_files += 1
                                    counted_project_file = True
                            elif project_session is None and payload.get("cwd"):
                                project_session = False
                            model = _codex_payload_model(payload)
                            if model:
                                current_model = model
                        if project_session is False:
                            continue
                        info = payload.get("info")
                        if not isinstance(info, dict):
                            continue
                        usage = info.get("last_token_usage")
                        if not isinstance(usage, dict):
                            continue
                        ts = _parse_iso(d.get("timestamp"))
                        if not ts or ts < cutoff:
                            continue
                        tok = _usage_total_tokens(usage)
                        if tok <= 0:
                            continue
                        if project_session is None:
                            project_session = True
                            if not counted_project_file:
                                project_files += 1
                                counted_project_file = True
                        hour = int(ts // 3600) * 3600
                        b = buckets.setdefault(hour, _new_bucket())
                        b["codex"] += tok
                        b["codex_cache_read"] += int_value(usage.get("cached_input_tokens"))
                        by_model[current_model] += tok
                        token_events += 1
                        total += tok
            except OSError:
                continue

    meta: dict[str, object] = {
        "available": total > 0 or events > 0,
        "events": events,
        "token_events": token_events,
        "total": total,
        "files": files,
        "project_files": project_files,
    }
    if history_reason and not meta["available"]:
        meta["reason"] = history_reason
    if not CODEX_SESSIONS_DIR.exists():
        meta["sessions_reason"] = "codex sessions dir not found"
    return meta


_TOKENS_CACHE: dict[str, object] = {"value": None, "expires": 0.0}
_TOKENS_CACHE_TTL = 120.0
_TOKENS_CACHE_LOCK = threading.Lock()


def collect_tokens(hours: int = 168) -> dict[str, object]:
    """Aggregate all three live sources into hourly buckets tagged by source.
    Merges agent_tokens.json cumulative counts into totals as a fallback stat."""
    cutoff = time.time() - hours * 3600
    buckets: dict[int, dict[str, float]] = {}
    by_model: dict[str, float] = Counter()

    claude_meta = _collect_claude(buckets, cutoff, by_model)
    opencode_meta = _collect_opencode(buckets, cutoff, by_model)
    codex_meta = _collect_codex(buckets, cutoff, by_model)
    glm_meta = _collect_glm(buckets, cutoff, by_model)

    series = []
    for hour in sorted(buckets):
        b = buckets[hour]
        claude = int(b["claude"])
        opencode = int(b["opencode"])
        codex = int(b["codex"])
        glm = int(b["glm"])
        codex_ev = int(b["codex_events"])
        total = claude + opencode + codex + glm
        series.append({
            "unix": hour,
            "by_source": {"claude": claude, "opencode": opencode, "codex": codex, "glm": glm},
            "claude": claude,
            "opencode": opencode,
            "codex": codex,
            "glm": glm,
            "codex_events": codex_ev,
            "cache_read": int(b["claude_cache_read"]),
            "codex_cache_read": int(b["codex_cache_read"]),
            "glm_cache_read": int(b["glm_cache_read"]),
            "input": total,   # legacy field for older clients
            "output": 0,
            "total": total,
        })

    totals = {
        "claude": int(claude_meta.get("total", 0)),
        "opencode": int(opencode_meta.get("total", 0)),
        "codex": int(codex_meta.get("total", 0)),
        "glm": int(glm_meta.get("total", 0)),
        "codex_events": int(codex_meta.get("events", 0)),
        "all_tokens": (
            int(claude_meta.get("total", 0))
            + int(opencode_meta.get("total", 0))
            + int(codex_meta.get("total", 0))
            + int(glm_meta.get("total", 0))
        ),
    }
    # agent_tokens.json cumulative fallback (per-agent lifetime totals).
    agent_tokens = _load_json_obj(AGENT_TOKENS_JSON).get("agents", {})
    if isinstance(agent_tokens, dict):
        totals["agent_tokens_cumulative"] = sum(
            int_value(v.get("tokens_used", 0))
            for v in agent_tokens.values() if isinstance(v, dict)
        )

    top_models = sorted(by_model.items(), key=lambda kv: -kv[1])[:10]
    return {
        "available": bool(series),
        "window_hours": hours,
        "buckets": series,
        "totals": totals,
        "grand_total": totals["all_tokens"],
        "by_model": [{"model": m, "tokens": int(v)} for m, v in top_models],
        "sources": {
            "claude": claude_meta,
            "opencode": opencode_meta,
            "codex": codex_meta,
            "glm": glm_meta,
        },
        "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
    }


def _load_token_history() -> dict[str, object]:
    return _load_json_obj(TOKEN_HISTORY_FILE)


def refresh_token_history() -> dict[str, object]:
    """Recompute the full window and persist to token_history.json (change-gated
    on the grand total + bucket count so we don't churn the file every loop)."""
    payload = collect_tokens(hours=24 * 30)  # 30-day persisted window
    prev = _load_token_history()
    changed = (
        int(prev.get("grand_total", -1)) != int(payload.get("grand_total", 0))
        or len(prev.get("buckets", []) or []) != len(payload.get("buckets", []))
    )
    if changed:
        try:
            _write_json_obj(TOKEN_HISTORY_FILE, payload)
        except OSError:
            pass
    return payload


def load_tokens(hours: int = 168) -> dict[str, object]:
    """Front-end entry. Serves the live collector through a short TTL cache,
    falling back to the persisted token_history.json if collection fails (e.g.
    WSL unreachable). Slices the persisted window down to the requested hours."""
    now = time.monotonic()
    cached = _TOKENS_CACHE.get("value")
    if cached is not None and now < float(_TOKENS_CACHE.get("expires", 0)):
        full = cached  # type: ignore[assignment]
    else:
        with _TOKENS_CACHE_LOCK:
            now = time.monotonic()
            cached = _TOKENS_CACHE.get("value")
            if cached is not None and now < float(_TOKENS_CACHE.get("expires", 0)):
                full = cached  # type: ignore[assignment]
            else:
                try:
                    full = collect_tokens(hours=24 * 30)
                except Exception:  # noqa: BLE001
                    full = None
                if not full or not full.get("available"):
                    persisted = _load_token_history()
                    if persisted.get("available"):
                        full = persisted
                if full is None:
                    full = {"available": False, "buckets": [], "reason": "no token sources reachable"}
                _TOKENS_CACHE["value"] = full
                _TOKENS_CACHE["expires"] = time.monotonic() + _TOKENS_CACHE_TTL

    cutoff = time.time() - hours * 3600
    buckets = [b for b in (full.get("buckets") or []) if Number_ge(b.get("unix"), cutoff)]
    out = dict(full)
    out["buckets"] = buckets
    out["window_hours"] = hours
    out["totals"] = _series_token_totals(buckets, out.get("totals") if isinstance(out.get("totals"), dict) else None)
    out["grand_total"] = out["totals"]["all_tokens"]
    return out


def Number_ge(v: object, cutoff: float) -> bool:
    try:
        return float(v) >= cutoff
    except (TypeError, ValueError):
        return False


def _series_token_totals(series: list[dict[str, object]],
                         base_totals: dict[str, object] | None = None) -> dict[str, int]:
    totals = dict(base_totals or {})
    claude = sum(int_value(b.get("claude")) for b in series)
    opencode = sum(int_value(b.get("opencode")) for b in series)
    codex = sum(int_value(b.get("codex")) for b in series)
    codex_events = sum(int_value(b.get("codex_events")) for b in series)
    window_totals = {
        "claude": claude,
        "opencode": opencode,
        "codex": codex,
        "codex_events": codex_events,
        "all_tokens": claude + opencode + codex,
    }
    if "agent_tokens_cumulative" in totals:
        window_totals["agent_tokens_cumulative"] = int_value(totals.get("agent_tokens_cumulative"))
    return window_totals


def _parse_iso(ts: object) -> float:
    """Parse an ISO-8601 UTC string (trailing Z optional) -> unix seconds, or 0."""
    if not isinstance(ts, str) or not ts:
        return 0.0
    try:
        return time.mktime(time.strptime(ts.replace("Z", "").split(".")[0], "%Y-%m-%dT%H:%M:%S")) - time.timezone
    except (ValueError, OverflowError):
        return 0.0


HST = timezone(timedelta(hours=-10))


def next_hst_reset(hour: int, minute: int = 0) -> int:
    now_utc = datetime.now(timezone.utc)
    now_hst = now_utc.astimezone(HST)
    candidate = now_hst.replace(hour=hour, minute=minute, second=0, microsecond=0)
    if candidate <= now_hst:
        candidate += timedelta(days=1)
    return int(candidate.astimezone(timezone.utc).timestamp())


def next_utc_reset(hour: int, minute: int = 0) -> int:
    now_utc = datetime.now(timezone.utc)
    candidate = now_utc.replace(hour=hour, minute=minute, second=0, microsecond=0)
    if candidate <= now_utc:
        candidate += timedelta(days=1)
    return int(candidate.timestamp())


def default_limit_agents() -> list[dict[str, object]]:
    return [
        {
            "name": "claude-code",
            "label": "Claude Code",
            "kind": "daily",
            "next_reset_unix": next_hst_reset(7),
            "note": "Project plan expects Claude to refresh around 07:00 HST.",
            "models": ["claude-opus", "claude-sonnet", "claude-fable"],
            "assumed": True,
        },
        {
            "name": "glm-coding-plan",
            "label": "GLM Coding Plan",
            "kind": "daily",
            "next_reset_unix": next_utc_reset(0),
            "note": "Assumed daily coding-plan reset at 00:00 UTC; edit agent_limits.json if provider cadence differs.",
            "models": ["glm-5.1", "glm-5.1:cloud", "glm-5.2"],
            "assumed": True,
        },
    ]


def load_limits() -> dict[str, object]:
    """Read the user-maintained agent_limits.json and compute next-reset unix
    timestamps for the countdown panel."""
    data = _load_json_obj(AGENT_LIMITS_JSON)
    agents_in = data.get("agents", []) if isinstance(data.get("agents"), list) else []
    now = time.time()
    out = []
    for a in agents_in:
        if not isinstance(a, dict):
            continue
        next_unix = _parse_iso(a.get("next_reset"))
        if not next_unix:
            interval = a.get("reset_interval_hours")
            last = _parse_iso(a.get("last_reset"))
            if isinstance(interval, (int, float)) and interval and last:
                step = interval * 3600
                # roll forward from last_reset to the first reset strictly in the future
                k = max(0, int((now - last) // step) + 1)
                next_unix = last + k * step
        if not next_unix:
            name = str(a.get("name") or "").lower()
            if name == "claude-code":
                next_unix = next_hst_reset(7)
            elif name == "glm-coding-plan":
                next_unix = next_utc_reset(0)
        out.append({
            "name": a.get("name", ""),
            "label": a.get("label", a.get("name", "")),
            "kind": a.get("kind", ""),
            "reset_interval_hours": a.get("reset_interval_hours"),
            "next_reset_unix": int(next_unix) if next_unix else 0,
            "seconds_until": int(next_unix - now) if next_unix else 0,
            "note": a.get("note", ""),
            "models": a.get("models", []),
            "assumed": bool(a.get("assumed", False)),
        })
    present = {str(a.get("name") or "").lower() for a in out}
    for a in default_limit_agents():
        name = str(a.get("name") or "").lower()
        family = name.split("-", 1)[0]
        if name in present or family in present:
            continue
        next_unix = int_value(a.get("next_reset_unix"))
        out.append({
            "name": a.get("name", ""),
            "label": a.get("label", a.get("name", "")),
            "kind": a.get("kind", ""),
            "reset_interval_hours": a.get("reset_interval_hours"),
            "next_reset_unix": next_unix,
            "seconds_until": int(next_unix - now) if next_unix else 0,
            "note": a.get("note", ""),
            "models": a.get("models", []),
            "assumed": bool(a.get("assumed", False)),
        })
    return {
        "available": bool(out),
        "source": str(AGENT_LIMITS_JSON),
        "agents": out,
        "now_unix": int(now),
    }


def _rolling_token_sum(buckets: list, provider: str, window_seconds: float, now: float) -> int:
    """Sum a provider's tokens across the hourly buckets within the rolling window."""
    cut = now - window_seconds
    total = 0
    for b in buckets:
        if not isinstance(b, dict):
            continue
        if int_value(b.get("unix")) < cut:
            continue
        total += int_value(b.get(provider))
    return total


def load_agent_usage() -> dict[str, object]:
    """Per-provider (claude / codex / glm) token spend in the rolling 5h and weekly
    windows vs optional caps (agent_limits.json cap_tokens), plus reset countdown and
    any live proxy rate-limit status. Backs the 'Agent limits' panel — at a glance,
    who is approaching a 5h / weekly cap."""
    now = time.time()
    tokens = load_tokens(hours=24 * 7 + 1)  # cover the weekly window; reuses the cached collector
    buckets = tokens.get("buckets", []) if isinstance(tokens, dict) else []

    # cap + reset info keyed by (provider, window) from agent_limits.json
    lim = _load_json_obj(AGENT_LIMITS_JSON)
    agents_in = lim.get("agents", []) if isinstance(lim.get("agents"), list) else []
    cap_by: dict[tuple[str, str], dict[str, object]] = {}
    for a in agents_in:
        if not isinstance(a, dict):
            continue
        prov = str(a.get("provider") or "").lower()
        win = str(a.get("window") or "").lower()
        if not prov or not win:
            continue
        next_unix = _parse_iso(a.get("next_reset"))
        if not next_unix:
            interval = a.get("reset_interval_hours")
            last = _parse_iso(a.get("last_reset"))
            if isinstance(interval, (int, float)) and interval and last:
                step = interval * 3600
                k = max(0, int((now - last) // step) + 1)
                next_unix = last + k * step
        cap_raw = a.get("cap_tokens")
        cap_by[(prov, win)] = {
            "cap": int_value(cap_raw) if cap_raw else None,
            "next_reset_unix": int(next_unix) if next_unix else 0,
            "seconds_until": int(next_unix - now) if next_unix else 0,
            "note": a.get("note", ""),
            "assumed": bool(a.get("assumed", False)),
        }

    live = _load_json_obj(PROXY_USAGE_LIMITS_JSON)
    WINDOWS = {"5h": 5 * 3600, "weekly": 7 * 24 * 3600}
    LIVE_KEY = {"glm": "GLM", "claude": "Anthropic", "codex": "Codex"}
    LABEL = {"claude": "Claude", "codex": "Codex", "glm": "GLM"}

    providers = []
    for prov in ("claude", "codex", "glm"):
        windows = {}
        for win, secs in WINDOWS.items():
            meta = cap_by.get((prov, win))
            # GLM's plan only defines a 5h reset window, but still show its rolling
            # weekly token sum (no configured cap/reset) so its usage is visible.
            used = _rolling_token_sum(buckets, prov, secs, now)
            # Codex re-reads its full (cached) context each turn, so the raw sum is
            # dominated by cached input. Subtract cached_input to get a billable-ish
            # figure comparable to Claude/GLM (which already exclude cache reads).
            if prov == "codex":
                used = max(0, used - _rolling_token_sum(buckets, "codex_cache_read", secs, now))
            cap = (meta or {}).get("cap")
            pct = round(100.0 * used / cap, 1) if cap else None
            windows[win] = {
                "used": used,
                "cap": cap,
                "pct": pct,
                "next_reset_unix": (meta or {}).get("next_reset_unix", 0),
                "seconds_until": (meta or {}).get("seconds_until", 0),
                "assumed": (meta or {}).get("assumed", False),
            }
        status = None
        if isinstance(live, dict):
            ent = live.get(LIVE_KEY.get(prov, ""))
            if isinstance(ent, dict):
                hdrs = ent.get("headers") if isinstance(ent.get("headers"), dict) else {}
                status = hdrs.get("anthropic-ratelimit-unified-status") or ent.get("status")
        providers.append({
            "provider": prov,
            "label": LABEL[prov],
            "windows": windows,
            "live_status": status,
        })

    return {
        "available": bool(buckets),
        "now_unix": int(now),
        "providers": providers,
        "source": str(TOKEN_HISTORY_FILE),
    }


HTML = r"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>GC6E01 Progress Control</title>
  <style>
    :root {
      color-scheme: dark;
      --bg: #0d1118;
      --band: #151c28;
      --panel: #182231;
      --panel-2: #111925;
      --ink: #eef4fb;
      --muted: #a8b4c4;
      --quiet: #728095;
      --line: #2d3a4b;
      --line-strong: #46556a;
      --amber: #f0b35a;
      --teal: #38b995;
      --cobalt: #5c91df;
      --red: #e07171;
      --violet: #a98ee6;
      --steel: #8da0b8;
      --shadow: 0 1px 2px rgba(0, 0, 0, .22);
      --radius: 4px;
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      font-family: "Segoe UI", Arial, sans-serif;
      font-size: 14px;
      line-height: 1.35;
      letter-spacing: 0;
      background: var(--bg);
      color: var(--ink);
    }
    button, input, select {
      font: inherit;
      letter-spacing: 0;
    }
    button {
      cursor: pointer;
    }
    .topbar {
      display: grid;
      grid-template-columns: minmax(280px, 1fr) auto;
      gap: 18px;
      align-items: center;
      padding: 18px 24px;
      background: rgba(13, 17, 24, .9);
      border-bottom: 1px solid var(--line);
      position: sticky;
      top: 0;
      z-index: 20;
      backdrop-filter: blur(12px);
    }
    .hud-strip {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      gap: 12px;
      align-items: center;
      margin-bottom: 12px;
      padding: 9px 12px;
      border: 1px solid var(--line);
      border-radius: var(--radius);
      background: #0f1722;
      box-shadow: var(--shadow);
    }
    .hud-stats {
      display: flex;
      flex-wrap: wrap;
      gap: 14px;
      align-items: baseline;
      color: var(--muted);
      font-family: Consolas, "Courier New", monospace;
      font-weight: 700;
      min-width: 0;
    }
    .hud-project {
      color: #f3f7fb;
      font-size: 15px;
    }
    .hud-value {
      color: #dfe8f4;
      font-size: 18px;
    }
    .hud-good { color: #58d889; }
    .hud-warn { color: #f0b35a; }
    .hud-bad { color: #e07171; }
    .hud-controls {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
      align-items: center;
      justify-content: flex-end;
      color: var(--muted);
      font-size: 12px;
    }
    .hud-controls label {
      display: inline-flex;
      gap: 6px;
      align-items: center;
      white-space: nowrap;
    }
    .hud-controls input[type="checkbox"] {
      width: 14px;
      height: 14px;
      min-width: 14px;
      padding: 0;
      accent-color: #38b995;
    }
    .hud-controls select {
      height: 28px;
      width: 74px;
      padding: 0 7px;
    }
    h1 {
      margin: 0;
      font-size: 22px;
      font-weight: 760;
    }
    .subtitle {
      margin-top: 4px;
      color: var(--muted);
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      font-size: 12px;
    }
    .actions {
      display: flex;
      flex-wrap: wrap;
      gap: 8px;
      justify-content: flex-end;
    }
    .btn {
      height: 36px;
      border-radius: 7px;
      border: 1px solid var(--line-strong);
      color: var(--ink);
      background: #1c2939;
      padding: 0 12px;
      font-weight: 700;
    }
    .btn.primary {
      background: #23684f;
      border-color: #2f9874;
    }
    .btn.ghost {
      background: transparent;
    }
    .btn.xs {
      height: 24px;
      padding: 0 8px;
      font-size: 11px;
      font-weight: 700;
      border-radius: 4px;
    }
    .btn.danger {
      border-color: #6e3030;
      color: #f0b4b4;
    }
    .btn.danger:hover { background: #3a1e1e; }
    .btn.panel-action {
      height: 24px;
      padding: 0 9px;
      font-size: 11px;
      font-weight: 700;
      margin-left: auto;
    }
    .lock-scope {
      display: inline-block;
      min-width: 34px;
      text-align: center;
      padding: 1px 6px;
      border-radius: 4px;
      font-size: 10px;
      font-weight: 800;
      text-transform: uppercase;
      letter-spacing: .04em;
      border: 1px solid var(--line-strong);
    }
    .lock-scope.file { color: #cfe0ff; background: rgba(92, 145, 223, .16); border-color: #3a567f; }
    .lock-scope.fn { color: #cdebde; background: rgba(56, 185, 149, .15); border-color: #2f6e5a; }
    /* #5: subtle "fn pending in log" annotation in the lease key cell */
    .lease-hint { color: var(--quiet); font-size: 10px; font-style: italic; }
    .lock-actions { white-space: nowrap; text-align: right; }
    .lock-actions .btn + .btn { margin-left: 5px; }
    .lock-table td { vertical-align: middle; }
    main {
      padding: 18px 24px 30px;
      max-width: 1680px;
      margin: 0 auto;
    }
    .metric-grid {
      display: grid;
      grid-template-columns: repeat(6, minmax(130px, 1fr));
      gap: 10px;
      margin-bottom: 12px;
    }
    .metric, .panel, .detail-panel {
      background: var(--panel);
      border: 1px solid var(--line);
      border-radius: var(--radius);
      box-shadow: var(--shadow);
    }
    .metric {
      min-height: 92px;
      padding: 12px;
      position: relative;
      overflow: hidden;
    }
    .metric::after {
      content: "";
      position: absolute;
      inset: auto 0 0 0;
      height: 3px;
      background: var(--accent, var(--steel));
    }
    .metric-label {
      color: var(--muted);
      font-size: 11px;
      font-weight: 760;
      text-transform: uppercase;
    }
    .metric-value {
      margin-top: 8px;
      font-size: 28px;
      font-weight: 780;
    }
    .metric-note {
      margin-top: 3px;
      color: var(--quiet);
      font-size: 12px;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .overview {
      display: grid;
      grid-template-columns: minmax(300px, 1.15fr) minmax(300px, .85fr);
      gap: 12px;
      align-items: stretch;
      margin-bottom: 12px;
    }
    .panel {
      padding: 14px;
      min-width: 0;
    }
    .panel-title {
      display: flex;
      align-items: baseline;
      justify-content: space-between;
      gap: 10px;
      margin-bottom: 10px;
    }
    h2 {
      margin: 0;
      font-size: 14px;
      font-weight: 760;
      text-transform: uppercase;
      color: #dfe8f4;
    }
    .panel-note {
      color: var(--quiet);
      font-size: 12px;
    }
    .target-line {
      display: grid;
      grid-template-columns: auto 1fr auto;
      gap: 12px;
      align-items: center;
      padding: 12px;
      border-radius: var(--radius);
      background: #101824;
      border: 1px solid var(--line);
      margin-bottom: 12px;
    }
    .target-badge {
      display: grid;
      place-items: center;
      width: 50px;
      height: 50px;
      border-radius: 7px;
      color: #1a1204;
      background: var(--amber);
      font-weight: 800;
    }
    .target-name {
      font-size: 18px;
      font-weight: 780;
      overflow-wrap: anywhere;
    }
    .target-meta {
      margin-top: 3px;
      color: var(--muted);
      overflow-wrap: anywhere;
    }
    .progress-shell {
      height: 12px;
      border-radius: 999px;
      background: #0d131d;
      border: 1px solid #253243;
      overflow: hidden;
    }
    .progress-fill {
      height: 100%;
      width: 0;
      background: linear-gradient(90deg, var(--teal), var(--amber));
      transition: width .25s ease;
    }
    .chart-grid {
      display: grid;
      grid-template-columns: 1fr 1.2fr;
      gap: 12px;
      margin-bottom: 12px;
    }
    .ops-grid {
      display: grid;
      grid-template-columns: minmax(0, 1fr) minmax(320px, .85fr);
      gap: 12px;
      margin-bottom: 12px;
      align-items: stretch;
    }
    .feed {
      display: grid;
      gap: 8px;
      max-height: 265px;
      overflow: auto;
      padding-right: 4px;
    }
    .feed-row {
      display: grid;
      grid-template-columns: 92px 1fr auto;
      gap: 10px;
      align-items: start;
      padding: 8px 9px;
      border: 1px solid #263244;
      background: #101824;
      border-radius: 7px;
    }
    .feed-time {
      color: var(--quiet);
      font-size: 12px;
      font-family: Consolas, "Courier New", monospace;
      white-space: nowrap;
    }
    .feed-main {
      color: #dce7f3;
      overflow-wrap: anywhere;
    }
    .feed-tag {
      border-radius: 999px;
      padding: 2px 7px;
      background: rgba(56, 185, 149, .14);
      color: #93f0d2;
      font-size: 11px;
      font-weight: 780;
      text-transform: uppercase;
      white-space: nowrap;
    }
    .commit-list {
      display: grid;
      gap: 7px;
      max-height: 265px;
      overflow: auto;
      padding-right: 4px;
    }
    .commit-row {
      display: grid;
      grid-template-columns: 72px 92px 1fr;
      gap: 10px;
      align-items: start;
      padding: 8px 9px;
      border: 1px solid #263244;
      background: #101824;
      border-radius: 7px;
    }
    .commit-sha {
      color: #76a9ff;
      font-family: Consolas, "Courier New", monospace;
      font-weight: 780;
    }
    .commit-when {
      color: var(--quiet);
      font-family: Consolas, "Courier New", monospace;
      font-size: 12px;
      white-space: nowrap;
    }
    .commit-subject {
      color: #cdd8e6;
      overflow-wrap: anywhere;
    }
    .tu-panel {
      margin-bottom: 12px;
    }
    .tu-toolbar {
      display: grid;
      grid-template-columns: minmax(180px, 1fr) auto auto;
      gap: 8px;
      margin-bottom: 10px;
      align-items: center;
    }
    .tu-map {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(92px, 1fr));
      grid-auto-rows: 42px;
      grid-auto-flow: dense;
      gap: 5px;
      min-height: 240px;
      max-height: 430px;
      overflow: auto;
      border: 1px solid #263244;
      border-radius: var(--radius);
      background: #0f1722;
      padding: 8px;
    }
    .tu-tile {
      display: grid;
      align-content: start;
      gap: 3px;
      min-width: 0;
      border: 1px solid rgba(255, 255, 255, .14);
      border-radius: 4px;
      padding: 6px;
      color: #06100b;
      overflow: hidden;
      cursor: pointer;
      box-shadow: inset 0 0 0 1px rgba(0, 0, 0, .16);
    }
    .tu-name {
      font-family: Consolas, "Courier New", monospace;
      font-size: 12px;
      font-weight: 800;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .tu-pct {
      font-size: 12px;
      font-weight: 780;
    }
    .tu-meta {
      color: rgba(6, 16, 11, .72);
      font-size: 11px;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .chart-card {
      min-height: 260px;
    }
    canvas {
      width: 100%;
      height: 205px;
      display: block;
    }
    .legend {
      display: flex;
      flex-wrap: wrap;
      gap: 8px 14px;
      margin-top: 10px;
      color: var(--muted);
      font-size: 12px;
    }
    .legend-item {
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }
    .swatch {
      width: 10px;
      height: 10px;
      border-radius: 2px;
      background: var(--swatch);
    }
    .workbench {
      display: grid;
      grid-template-columns: minmax(0, 1fr) 360px;
      gap: 12px;
      align-items: start;
    }
    .toolbar {
      display: grid;
      grid-template-columns: minmax(180px, 1fr) 170px minmax(140px, 220px) 132px;
      gap: 8px;
      margin-bottom: 10px;
    }
    input, select {
      height: 36px;
      min-width: 0;
      color: var(--ink);
      background: #101824;
      border: 1px solid var(--line);
      border-radius: 7px;
      padding: 0 10px;
    }
    input::placeholder {
      color: #78869a;
    }
    .table-wrap {
      overflow: auto;
      border: 1px solid var(--line);
      border-radius: var(--radius);
      background: var(--panel-2);
    }
    table {
      width: 100%;
      min-width: 1040px;
      border-collapse: collapse;
    }
    th, td {
      padding: 9px 10px;
      border-bottom: 1px solid #263244;
      text-align: left;
      vertical-align: middle;
      white-space: nowrap;
    }
    th {
      position: sticky;
      top: 0;
      z-index: 4;
      background: #202b3b;
      color: #c4cfdd;
      font-size: 11px;
      font-weight: 760;
      text-transform: uppercase;
      user-select: none;
    }
    th[data-sort] {
      cursor: pointer;
    }
    td.wrap {
      white-space: normal;
      min-width: 210px;
      overflow-wrap: anywhere;
    }
    tr {
      transition: background .12s ease;
    }
    tbody tr:hover {
      background: #1d2939;
    }
    tbody tr.selected {
      background: #26364a;
      outline: 1px solid #547298;
      outline-offset: -1px;
    }
    .mono {
      font-family: Consolas, "Courier New", monospace;
    }
    .pill {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      min-width: 104px;
      border-radius: 999px;
      padding: 3px 9px;
      font-size: 12px;
      font-weight: 780;
      border: 1px solid transparent;
    }
    .needs { background: rgba(240, 179, 90, .16); color: #ffd28a; border-color: rgba(240, 179, 90, .45); }
    .proposed { background: rgba(92, 145, 223, .16); color: #a9caff; border-color: rgba(92, 145, 223, .45); }
    .recorded { background: rgba(141, 160, 184, .16); color: #d1d9e4; border-color: rgba(141, 160, 184, .35); }
    .renamed { background: rgba(56, 185, 149, .16); color: #93f0d2; border-color: rgba(56, 185, 149, .48); }
    .bad {
      color: #ff9a9a;
      font-weight: 780;
    }
    .detail-panel {
      padding: 14px;
      position: sticky;
      top: 82px;
    }
    .detail-title {
      font-size: 16px;
      font-weight: 780;
      overflow-wrap: anywhere;
    }
    .detail-subtitle {
      margin: 4px 0 12px;
      color: var(--muted);
      overflow-wrap: anywhere;
    }
    .kv {
      display: grid;
      grid-template-columns: 112px 1fr;
      gap: 8px;
      padding: 7px 0;
      border-top: 1px solid #263244;
    }
    .kv:first-of-type {
      border-top: 0;
    }
    .kv .k {
      color: var(--quiet);
      font-size: 12px;
      text-transform: uppercase;
      font-weight: 760;
    }
    .kv .v {
      overflow-wrap: anywhere;
    }
    code {
      color: #dce8f5;
      background: #0c131d;
      border: 1px solid #253143;
      border-radius: 5px;
      padding: 2px 5px;
      font-family: Consolas, "Courier New", monospace;
      font-size: 12px;
    }
    .command-list {
      display: grid;
      gap: 6px;
      margin-top: 8px;
    }
    .source-bars {
      display: grid;
      gap: 8px;
      margin-top: 8px;
    }
    .bar-row {
      display: grid;
      grid-template-columns: minmax(90px, 1fr) 3fr 34px;
      gap: 8px;
      align-items: center;
      color: var(--muted);
      font-size: 12px;
    }
    .bar-name {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .bar-track {
      height: 8px;
      background: #0d131d;
      border-radius: 999px;
      overflow: hidden;
    }
    .bar-fill {
      height: 100%;
      background: var(--cobalt);
      border-radius: 999px;
    }
    .empty-state {
      color: var(--quiet);
      padding: 16px;
    }
    .tabs {
      display: inline-flex;
      gap: 6px;
      padding: 4px;
      margin-bottom: 12px;
      border: 1px solid var(--line);
      border-radius: var(--radius);
      background: #0f1722;
    }
    .tab-btn {
      height: 34px;
      min-width: 124px;
      padding: 0 14px;
      border: 0;
      border-radius: 6px;
      color: var(--muted);
      background: transparent;
      font-weight: 780;
    }
    .tab-btn.active {
      color: #06100b;
      background: linear-gradient(90deg, #39c95e, #f0b35a);
    }
    .view {
      display: none;
    }
    .view.active {
      display: block;
    }
    .decomp-workspace {
      display: grid;
      grid-template-columns: minmax(0, 1fr) minmax(360px, .42fr);
      gap: 12px;
      align-items: start;
      margin-bottom: 12px;
    }
    .decomp-map {
      max-height: 620px;
      grid-auto-rows: 48px;
    }
    .tu-tile.selected {
      outline: 2px solid #eef4fb;
      outline-offset: 1px;
    }
    .decomp-detail {
      max-height: 620px;
      overflow: auto;
    }
    .treemap-toolbar {
      display: grid;
      gap: 8px;
      margin-bottom: 10px;
    }
    .treemap-legend { display: flex; flex-wrap: wrap; gap: 14px; align-items: center; font: 600 10px "Cascadia Mono", monospace; color: var(--muted); }
    .treemap-legend .tm-leg { display: inline-flex; gap: 5px; align-items: center; }
    .treemap-legend .sw { width: 11px; height: 11px; border-radius: 2px; display: inline-block; box-sizing: border-box; }
    .treemap-legend .sw.glow { background: rgba(110,231,168,.85); box-shadow: 0 0 6px rgba(110,231,168,.9); }
    .treemap-legend .sw.agent { border: 2px solid rgba(127,209,255,.95); }
    .treemap-legend .sw.anneal { border: 2px dashed rgba(255,176,72,.95); }
    .treemap-controls {
      display: grid;
      grid-template-columns: minmax(160px, 1fr) minmax(150px, 210px) auto auto auto;
      gap: 8px;
      align-items: center;
    }
    .area-toggle {
      display: inline-flex;
      gap: 6px;
      align-items: center;
      color: var(--muted);
      font-size: 12px;
      white-space: nowrap;
    }
    .area-toggle input {
      width: 14px;
      height: 14px;
      min-width: 14px;
      accent-color: #38b995;
    }
    .crumbs {
      display: flex;
      flex-wrap: wrap;
      align-items: center;
      gap: 4px;
      color: var(--muted);
      font-size: 13px;
      min-height: 22px;
    }
    .crumbs button {
      border: 0;
      background: transparent;
      color: #76a9ff;
      font-weight: 760;
      padding: 0;
    }
    .crumbs button:disabled {
      color: #cdd8e6;
      cursor: default;
    }
    .crumbs .sep {
      color: var(--quiet);
    }
    .treemap-shell {
      position: relative;
      border: 1px solid #263244;
      border-radius: var(--radius);
      background: #0f1722;
      overflow: hidden;
    }
    #decomp-treemap {
      width: 100%;
      height: 560px;
      display: block;
      cursor: pointer;
    }
    .treemap-tip {
      position: absolute;
      z-index: 9;
      pointer-events: none;
      max-width: 280px;
      padding: 7px 9px;
      border: 1px solid var(--line-strong);
      border-radius: 6px;
      background: rgba(10, 15, 22, .96);
      color: var(--ink);
      font-size: 12px;
      box-shadow: var(--shadow);
    }
    .treemap-tip b {
      color: #fff;
      font-family: Consolas, "Courier New", monospace;
    }
    .files-toolbar {
      display: grid;
      grid-template-columns: minmax(180px, 1fr) auto auto;
      gap: 8px;
      margin-bottom: 10px;
      align-items: center;
    }
    th[data-fsort] { cursor: pointer; }
    th.num, td.num { text-align: right; font-family: Consolas, "Courier New", monospace; }
    tfoot td {
      font-weight: 800;
      background: #1a2433;
      border-top: 2px solid var(--line-strong);
      position: sticky;
      bottom: 0;
    }
    .mini-bar {
      display: inline-block;
      vertical-align: middle;
      width: 46px;
      height: 7px;
      margin-left: 6px;
      border-radius: 999px;
      background: #0d131d;
      overflow: hidden;
    }
    .mini-bar > span {
      display: block;
      height: 100%;
      background: var(--teal);
    }
    .decomp-title {
      font-size: 17px;
      font-weight: 800;
      overflow-wrap: anywhere;
    }
    .decomp-subtitle {
      margin: 4px 0 12px;
      color: var(--muted);
      overflow-wrap: anywhere;
    }
    .mini-stats {
      display: grid;
      grid-template-columns: repeat(4, minmax(0, 1fr));
      gap: 7px;
      margin-bottom: 12px;
    }
    .mini-stat {
      padding: 8px;
      border: 1px solid #263244;
      border-radius: 6px;
      background: #101824;
    }
    .mini-label {
      color: var(--quiet);
      font-size: 11px;
      font-weight: 760;
      text-transform: uppercase;
    }
    .mini-value {
      margin-top: 3px;
      font-weight: 800;
      font-family: Consolas, "Courier New", monospace;
    }
    .function-list,
    .attempt-list {
      display: grid;
      gap: 6px;
      max-height: 300px;
      overflow: auto;
      padding-right: 4px;
    }
    .function-row {
      display: grid;
      grid-template-columns: minmax(150px, 1fr) 72px 72px 82px minmax(70px, .55fr);
      gap: 8px;
      align-items: center;
      padding: 7px 8px;
      border: 1px solid #263244;
      border-radius: 6px;
      background: #101824;
    }
    .function-name {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
      font-family: Consolas, "Courier New", monospace;
      font-weight: 760;
    }
    .function-pct,
    .function-size {
      font-family: Consolas, "Courier New", monospace;
      color: #dce7f3;
      text-align: right;
    }
    .status-chip {
      justify-self: end;
      border-radius: 999px;
      padding: 2px 7px;
      font-size: 11px;
      font-weight: 780;
      text-transform: uppercase;
      border: 1px solid rgba(255, 255, 255, .16);
    }
    .status-chip.matched { background: rgba(56, 185, 149, .18); color: #93f0d2; }
    .status-chip.near { background: rgba(240, 179, 90, .18); color: #ffd28a; }
    .status-chip.partial { background: rgba(92, 145, 223, .18); color: #a9caff; }
    .status-chip.missing { background: rgba(224, 113, 113, .18); color: #ffaaaa; }
    .attempt-row {
      display: grid;
      grid-template-columns: 162px 78px 1fr;
      gap: 9px;
      align-items: start;
      padding: 8px 9px;
      border: 1px solid #263244;
      background: #101824;
      border-radius: 7px;
    }
    .attempt-time,
    .attempt-kind {
      color: var(--quiet);
      font-family: Consolas, "Courier New", monospace;
      font-size: 12px;
      white-space: nowrap;
    }
    .attempt-message {
      color: #dce7f3;
      overflow-wrap: anywhere;
    }
    .history-layout {
      display: grid;
      grid-template-columns: minmax(0, 1fr);
      gap: 12px;
      margin-bottom: 12px;
    }
    /* right column of the treemap workspace: file-stats card + the moved
       Selected File Progress chart, stacked vertically under each other. */
    .decomp-side {
      display: flex;
      flex-direction: column;
      gap: 12px;
      min-width: 0;
    }
    .decomp-side .detail-panel { position: static; top: auto; }
    .decomp-side #file-progress-card { margin: 0; }
    /* ---- v9: decomp.me-style function reader ------------------------------ */
    .reader-overlay {
      display: none;
      margin-bottom: 12px;
    }
    .reader-overlay.active { display: block; }
    .reader-head {
      display: flex;
      flex-wrap: wrap;
      align-items: center;
      gap: 12px;
      margin-bottom: 12px;
    }
    .reader-fn {
      font-family: Consolas, "Courier New", monospace;
      font-size: 18px;
      font-weight: 800;
      color: #eef4fb;
    }
    .reader-pct {
      font-family: Consolas, "Courier New", monospace;
      font-size: 22px;
      font-weight: 800;
    }
    .reader-pct.good { color: #58d889; }
    .reader-pct.near { color: #f0b35a; }
    .reader-pct.bad { color: #e07171; }
    .reader-meta {
      color: var(--muted);
      font-size: 12px;
      font-family: Consolas, "Courier New", monospace;
    }
    .reader-grid {
      display: grid;
      grid-template-columns: minmax(0, 1.6fr) minmax(260px, .6fr);
      gap: 12px;
      align-items: start;
    }
    .asm-pane {
      border: 1px solid var(--line);
      border-radius: var(--radius);
      background: #0b1018;
      overflow: hidden;
    }
    .asm-colhead {
      display: grid;
      grid-template-columns: 36px 1fr 1fr;
      gap: 0;
      position: sticky;
      top: 0;
      z-index: 2;
      background: #1b2535;
      border-bottom: 1px solid var(--line-strong);
    }
    .asm-colhead span {
      padding: 7px 10px;
      font-size: 11px;
      font-weight: 800;
      text-transform: uppercase;
      color: #c4cfdd;
      letter-spacing: .04em;
    }
    .asm-colhead .target { border-right: 1px solid var(--line); }
    .asm-body {
      max-height: 560px;
      overflow: auto;
      font-family: Consolas, "Courier New", monospace;
      font-size: 12.5px;
      line-height: 1.5;
    }
    .asm-line {
      display: grid;
      grid-template-columns: 36px 1fr 1fr;
      border-bottom: 1px solid rgba(38, 50, 68, .5);
    }
    .asm-line:hover { background: rgba(92, 145, 223, .07); }
    .asm-num {
      color: #4d5b70;
      text-align: right;
      padding: 1px 6px;
      user-select: none;
      background: rgba(0, 0, 0, .18);
    }
    .asm-cell {
      padding: 1px 10px;
      white-space: pre;
      overflow: hidden;
      text-overflow: ellipsis;
    }
    .asm-cell.target { border-right: 1px solid rgba(45, 58, 75, .8); }
    .asm-line.same .asm-cell { color: #b9c6d6; }
    .asm-line.diff { background: rgba(224, 113, 113, .10); }
    .asm-line.diff .asm-cell.current { color: #ff9a9a; }
    .asm-line.diff .asm-cell.target { color: #ffd28a; }
    .asm-line.addrm { background: rgba(240, 179, 90, .09); }
    .asm-line.addrm .asm-cell { color: #c69152; }
    .asm-legend {
      display: flex;
      flex-wrap: wrap;
      gap: 8px 14px;
      margin-top: 10px;
      color: var(--muted);
      font-size: 12px;
    }
    .reader-side { display: grid; gap: 12px; }
    .wall-card {
      border: 1px solid var(--line);
      border-radius: var(--radius);
      background: var(--panel-2);
      padding: 12px;
    }
    .crack-card {
      border-color: #31513e;
      background:
        linear-gradient(135deg, rgba(63,185,80,.10), rgba(13,20,31,.20) 42%),
        var(--panel-2);
    }
    .crack-title {
      display: flex;
      align-items: baseline;
      justify-content: space-between;
      gap: 8px;
      margin-bottom: 8px;
    }
    .crack-title b {
      color: #eef4fb;
      font-size: 12px;
      text-transform: uppercase;
      letter-spacing: .06em;
    }
    .crack-title span {
      color: var(--quiet);
      font: 10.5px Consolas, "Courier New", monospace;
    }
    .crack-field {
      margin-top: 9px;
    }
    .crack-field label {
      display: block;
      color: var(--quiet);
      font-size: 10.5px;
      font-weight: 800;
      text-transform: uppercase;
      margin-bottom: 4px;
    }
    .crack-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 5px;
    }
    .crack-choice {
      min-height: 34px;
      border: 1px solid var(--line);
      border-radius: 4px;
      background: #0d141f;
      color: #cbd7e6;
      font: 700 10.5px Consolas, "Courier New", monospace;
      cursor: pointer;
      overflow: hidden;
      text-overflow: ellipsis;
    }
    .crack-choice.active {
      border-color: var(--accent-dim);
      color: var(--accent);
      background: rgba(63,185,80,.12);
      box-shadow: inset 0 0 0 1px rgba(63,185,80,.15);
    }
    .crack-actions {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 6px;
      margin-top: 10px;
    }
    .crack-status {
      min-height: 18px;
      margin-top: 8px;
      color: var(--muted);
      font-size: 11px;
      overflow-wrap: anywhere;
    }
    .crack-job-list {
      display: grid;
      gap: 5px;
      max-height: 150px;
      overflow: auto;
      margin-top: 8px;
    }
    .crack-job {
      display: grid;
      grid-template-columns: 72px 1fr;
      gap: 6px;
      border-top: 1px solid var(--line);
      padding-top: 5px;
      font-size: 11px;
    }
    .crack-job span:first-child {
      color: var(--accent);
      font-family: Consolas, "Courier New", monospace;
    }
    .terminal-card pre {
      max-height: 220px;
      overflow: auto;
      margin: 8px 0 0;
      padding: 9px;
      border: 1px solid #263244;
      border-radius: 4px;
      background: #05080d;
      color: #9ee6ba;
      font: 11px/1.35 Consolas, "Courier New", monospace;
      white-space: pre-wrap;
    }
    .difficulty-chip {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      min-width: 0;
      border: 1px solid #314157;
      border-radius: 4px;
      padding: 2px 5px;
      color: #cbd7e6;
      font: 10px Consolas, "Courier New", monospace;
      text-transform: uppercase;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .difficulty-chip.wall { color: #ffaaaa; border-color: rgba(224,113,113,.45); }
    .difficulty-chip.asm { color: #ffd28a; border-color: rgba(240,179,90,.45); }
    .difficulty-chip.near { color: #a9caff; border-color: rgba(92,145,223,.45); }
    .difficulty-chip.giant { color: #d6c6ff; border-color: rgba(169,142,230,.45); }
    .wall-banner {
      display: inline-flex;
      align-items: center;
      gap: 7px;
      padding: 5px 10px;
      border-radius: 999px;
      font-size: 12px;
      font-weight: 800;
      margin-bottom: 8px;
    }
    .wall-banner.wall { background: rgba(224, 113, 113, .16); color: #ffaaaa; border: 1px solid rgba(224, 113, 113, .4); }
    .wall-banner.equiv { background: rgba(169, 142, 230, .16); color: #d6c6ff; border: 1px solid rgba(169, 142, 230, .4); }
    .wall-banner.clear { background: rgba(56, 185, 149, .14); color: #93f0d2; border: 1px solid rgba(56, 185, 149, .35); }
    .wall-note {
      color: #cdd8e6;
      font-size: 12.5px;
      line-height: 1.5;
      overflow-wrap: anywhere;
    }
    .agent-table {
      width: 100%;
      border-collapse: collapse;
      font-size: 12.5px;
    }
    .agent-table th, .agent-table td {
      padding: 7px 9px;
      border-bottom: 1px solid #263244;
      text-align: left;
      white-space: nowrap;
    }
    .agent-table th { color: #c4cfdd; font-size: 11px; text-transform: uppercase; }
    .agent-dot {
      display: inline-block;
      width: 8px; height: 8px;
      border-radius: 999px;
      margin-right: 6px;
      vertical-align: middle;
    }
    .agent-dot.busy { background: #58d889; box-shadow: 0 0 6px #58d889; }
    .agent-dot.idle { background: #5b6a80; }
    .limit-grid {
      display: grid;
      grid-template-columns: repeat(auto-fill, minmax(160px, 1fr));
      gap: 10px;
    }
    .limit-card {
      border: 1px solid #263244;
      border-radius: 7px;
      background: #101824;
      padding: 11px;
    }
    .limit-label { color: var(--muted); font-size: 11px; font-weight: 760; text-transform: uppercase; }
    .limit-countdown {
      margin-top: 6px;
      font-family: Consolas, "Courier New", monospace;
      font-size: 22px;
      font-weight: 800;
      color: #f0b35a;
    }
    .limit-countdown.soon { color: #e07171; }
    .limit-note { margin-top: 4px; color: var(--quiet); font-size: 11px; }
    .ops3-grid {
      display: grid;
      grid-template-columns: minmax(0, 1.3fr) minmax(280px, .7fr);
      gap: 12px;
      margin-bottom: 12px;
      align-items: start;
    }
    /* ---- v10: token totals stat strip ------------------------------------- */
    .token-stats {
      display: flex;
      flex-wrap: wrap;
      gap: 8px 18px;
      margin-bottom: 10px;
    }
    .token-stat {
      padding: 6px 12px;
      border: 1px solid #263244;
      border-radius: 6px;
      background: #101824;
      min-width: 90px;
    }
    .token-stat-value {
      font-family: Consolas, "Courier New", monospace;
      font-size: 18px;
      font-weight: 800;
    }
    .token-stat-label {
      color: var(--quiet);
      font-size: 11px;
      font-weight: 760;
      text-transform: uppercase;
    }
    /* ---- v10: pinned (tap-to-show) treemap detail panel for touch devices -- */
    .treemap-pin {
      position: fixed;
      left: 50%;
      bottom: 16px;
      transform: translateX(-50%);
      z-index: 70;
      width: min(94vw, 460px);
      max-height: 60vh;
      overflow: auto;
      padding: 14px 16px 16px;
      border: 1px solid var(--line-strong);
      border-radius: 12px;
      background: rgba(13, 18, 26, .98);
      box-shadow: 0 -8px 40px rgba(0, 0, 0, .6);
      display: none;
      font-size: 15px;
      line-height: 1.55;
    }
    .treemap-pin.active { display: block; }
    .treemap-pin .pin-body b {
      color: #fff;
      font-family: Consolas, "Courier New", monospace;
      font-size: 16px;
    }
    .treemap-pin-actions {
      display: flex;
      gap: 10px;
      margin-top: 14px;
    }
    .treemap-pin-actions .btn {
      flex: 1 1 auto;
      height: 44px;
      font-size: 15px;
    }
    .treemap-pin-actions .btn.open {
      background: #23684f;
      border-color: #2f9874;
      color: #eef4fb;
    }
    @media (max-width: 1180px) {
      .metric-grid { grid-template-columns: repeat(3, minmax(130px, 1fr)); }
      .overview, .chart-grid, .ops-grid, .ops3-grid, .workbench, .decomp-workspace, .history-layout, .reader-grid { grid-template-columns: 1fr; }
      .detail-panel { position: static; }
    }
    @media (max-width: 760px) {
      html, body { overflow-x: hidden; }
      .topbar { grid-template-columns: 1fr; padding: 14px 12px; }
      .hud-strip { grid-template-columns: 1fr; }
      .actions { justify-content: stretch; }
      .actions .btn { flex: 1 1 auto; }
      main { padding: 12px; }
      .metric-grid { grid-template-columns: repeat(2, minmax(120px, 1fr)); }
      .toolbar { grid-template-columns: 1fr; }
      .tu-toolbar { grid-template-columns: 1fr; }
      .treemap-controls { grid-template-columns: 1fr; }
      .files-toolbar { grid-template-columns: 1fr; }
      .target-line { grid-template-columns: 1fr; }
      .feed-row, .commit-row, .attempt-row, .function-row { grid-template-columns: 1fr; }
      .mini-stats { grid-template-columns: repeat(2, minmax(0, 1fr)); }
      .tabs { display: flex; width: 100%; }
      .tab-btn { flex: 1 1 0; min-width: 0; padding: 0 8px; }
      /* Wide data tables: shrink the min-width so the .table-wrap scroller fits
         the viewport, and drop low-value columns on phones. */
      table { min-width: 560px; }
      #files-table { min-width: 640px; }
      /* Targets table: hide Size, Current, Old Refs (kept in the detail panel). */
      #view-symbols table th:nth-child(4),
      #view-symbols table td:nth-child(4),
      #view-symbols table th:nth-child(6),
      #view-symbols table td:nth-child(6),
      #view-symbols table th:nth-child(7),
      #view-symbols table td:nth-child(7) { display: none; }
      /* Files table: hide Matched bytes, Matched Fns, Fuzzy %, Complete. */
      #files-table th:nth-child(3), #files-table td:nth-child(3),
      #files-table th:nth-child(6), #files-table td:nth-child(6),
      #files-table th:nth-child(8), #files-table td:nth-child(8),
      #files-table th:nth-child(9), #files-table td:nth-child(9) { display: none; }
      /* Agent table: hide Claimed timestamp column. */
      .agent-table th:nth-child(4), .agent-table td:nth-child(4) { display: none; }
      .token-stat { flex: 1 1 40%; }
    }

    /* ===================================================================== */
    /* ORCHESTRATOR RESKIN — near-black, monospace, single-green accent,      */
    /* sharp rectangular cards, 3-column app shell. Appended last so these    */
    /* rules win over the originals without editing each in place.           */
    /* ===================================================================== */
    :root {
      --bg: #080b10;
      --band: #0c111a;
      --panel: #0e141d;
      --panel-2: #0a0e15;
      --ink: #d4dee9;
      --muted: #8593a6;
      --quiet: #5d6b7d;
      --line: #1a2330;
      --line-strong: #2a3644;
      --accent: #3fb950;
      --accent-dim: #2ea043;
      --shadow: none;
      --radius: 3px;
    }
    body { background: var(--bg); font-size: 13px; }
    h1, h2, .rail-card-head, .rail-head, .panel-title h2, .metric-label,
    .stat-k, .stat-v, .agent-table th, .tab-btn, .lock-scope, .panel-note {
      font-family: "Cascadia Mono", Consolas, "Courier New", monospace;
    }
    h2, .panel-title h2 { letter-spacing: .07em; font-size: 12px; color: var(--ink); }

    /* ---- app shell ---- */
    .app {
      display: grid;
      grid-template-columns: 296px minmax(0, 1fr) 360px;
      align-items: start;
      min-height: 100vh;
    }
    .rail {
      position: sticky;
      top: 0;
      align-self: start;
      max-height: 100vh;
      overflow-y: auto;
      display: flex;
      flex-direction: column;
      gap: 11px;
      padding: 13px;
      background: var(--panel-2);
      border-right: 1px solid var(--line);
    }
    .rail-right { border-right: 0; border-left: 1px solid var(--line); }
    main.work { max-width: none; margin: 0; padding: 14px 16px 26px; min-width: 0; }

    /* ---- left rail cards ---- */
    .rail-brand {
      display: flex; align-items: center; gap: 10px;
      padding-bottom: 11px; border-bottom: 1px solid var(--line);
    }
    .rail-brand-mark {
      width: 26px; height: 26px; flex: 0 0 26px;
      display: grid; place-items: center;
      border: 1px solid var(--accent-dim); border-radius: 3px;
      color: var(--accent); font-size: 14px;
    }
    .rail-brand h1 { font-size: 12px; letter-spacing: .1em; text-transform: uppercase; margin: 0; }
    .rail-brand .subtitle { font-size: 10px; color: var(--quiet); display: block; }
    .rail-brand .subtitle span { display: inline; margin-right: 6px; }
    .rail-card {
      border: 1px solid var(--line);
      background: var(--panel);
      border-radius: var(--radius);
      padding: 10px;
    }
    .rail-card-head {
      display: flex; align-items: center; gap: 7px;
      font-size: 10.5px; font-weight: 700; letter-spacing: .1em;
      text-transform: uppercase; color: var(--muted);
      margin-bottom: 9px;
    }
    .rail-num {
      display: inline-grid; place-items: center;
      width: 15px; height: 15px; flex: 0 0 15px;
      border: 1px solid var(--line-strong); border-radius: 3px;
      font-size: 9px; color: var(--accent);
    }
    .stat-list { display: flex; flex-direction: column; gap: 4px; }
    .stat-row {
      display: flex; justify-content: space-between; align-items: baseline;
      gap: 10px; padding: 2px 0;
      border-bottom: 1px dotted var(--line);
    }
    .stat-row:last-child { border-bottom: 0; }
    .stat-k { color: var(--quiet); font-size: 10.5px; text-transform: uppercase; letter-spacing: .03em; }
    .stat-v { color: var(--ink); font-weight: 700; font-size: 13px; }
    .stat-v.hud-good { color: var(--accent); }
    .stat-v.hud-warn { color: #d2a24a; }
    .stat-v.hud-bad  { color: #d56a6a; }
    .ctl-row { display: flex; align-items: center; gap: 8px; color: var(--muted); font-size: 12px; margin-top: 8px; }
    .ctl-row select { height: 26px; }
    .btn.block { width: 100%; justify-content: center; display: inline-flex; align-items: center; }
    .proj-name { font: 700 14px "Cascadia Mono", Consolas, monospace; color: var(--ink); }
    .proj-sub { font-size: 11px; color: var(--quiet); margin-top: 3px; }

    /* vertical tabs inside the VIEW card */
    .rail .tabs { display: flex; flex-direction: column; gap: 4px; }
    .rail .tab-btn {
      width: 100%; text-align: left; height: 30px; border-radius: 3px;
      border: 1px solid transparent; background: transparent; color: var(--muted);
      padding: 0 10px; font-size: 12px; font-weight: 700; text-transform: uppercase; letter-spacing: .04em;
    }
    .rail .tab-btn:hover { background: #121a24; color: var(--ink); }
    .rail .tab-btn.active {
      background: rgba(63, 185, 80, .12);
      border-color: var(--accent-dim);
      color: var(--accent);
      box-shadow: none;
    }

    /* ---- right rail ---- */
    .rail-head {
      display: flex; align-items: baseline; justify-content: space-between;
      font-size: 10.5px; font-weight: 700; letter-spacing: .1em; text-transform: uppercase;
      color: var(--muted); padding: 2px 2px 9px; border-bottom: 1px solid var(--line);
    }
    .rail-head-note { color: var(--quiet); letter-spacing: .02em; }
    .rail-right .panel { border-radius: var(--radius); }

    /* ---- cards / panels: sharp + flat ---- */
    .metric, .panel, .detail-panel, .hud-strip, .rail-card {
      box-shadow: none;
      border-radius: var(--radius);
    }
    .panel { border-color: var(--line); }
    #decomp-metrics { display: none; }   /* numbers live in the left STATUS card now */
    .btn { border-radius: 3px; }
    .btn.primary { background: var(--accent-dim); border-color: var(--accent); color: #06140a; }
    .agent-table th { color: var(--muted); }

    @media (max-width: 1200px) {
      .app { grid-template-columns: 1fr; }
      .rail { position: static; max-height: none; border-right: 0; border-bottom: 1px solid var(--line); }
      .rail-right { border-left: 0; }
    }

    /* ---- rail kv rows / sync / run / ship / prs ---- */
    .kv { display: flex; justify-content: space-between; align-items: baseline; gap: 8px; padding: 2px 0; font: 11.5px "Cascadia Mono", Consolas, monospace; border-bottom: 1px dotted var(--line); }
    .kv:last-of-type { border-bottom: 0; }
    .kv-k { color: var(--quiet); text-transform: uppercase; font-size: 10px; letter-spacing: .03em; }
    .kv-v { color: var(--ink); font-weight: 700; }
    .rc-note { margin-left: auto; font-size: 10px; font-weight: 700; color: var(--quiet); text-transform: none; letter-spacing: 0; }
    .rc-note.good { color: var(--accent); }
    .rc-note.warn { color: #d2a24a; }
    .rc-hint { margin-top: 7px; font-size: 10.5px; color: var(--quiet); line-height: 1.35; }
    .btn.sm { height: 26px; font-size: 11px; padding: 0 9px; margin-top: 8px; }
    .run-controls { display: flex; gap: 5px; margin-bottom: 8px; }
    .run-controls .btn { flex: 1 1 0; margin-top: 0; padding: 0 4px; font-size: 10.5px; }
    .btn[disabled] { opacity: .42; cursor: not-allowed; }
    .rail-link { display: inline-block; margin-top: 8px; color: var(--accent); font-size: 11px; text-decoration: none; }
    .rail-link:hover { text-decoration: underline; }
    .pr-list { display: flex; flex-direction: column; gap: 4px; }
    .pr-row { display: grid; grid-template-columns: auto 1fr auto; gap: 7px; align-items: baseline; padding: 5px 6px; border: 1px solid var(--line); border-radius: 3px; background: var(--panel-2); text-decoration: none; color: var(--ink); }
    .pr-row:hover { border-color: var(--line-strong); }
    .pr-num { color: var(--quiet); font-size: 11px; }
    .pr-title { font-size: 11px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    .pr-state { font: 700 9px "Cascadia Mono", monospace; text-transform: uppercase; letter-spacing: .04em; padding: 1px 5px; border-radius: 3px; border: 1px solid var(--line-strong); color: var(--muted); }
    .pr-state.approved { color: var(--accent); border-color: var(--accent-dim); }
    .pr-state.draft { color: #8a8f98; }
    .pr-state.open { color: #c7d4ff; border-color: #2c4a7f; }

    /* ---- leases + quantum center row ---- */
    .leases-quantum { display: grid; grid-template-columns: minmax(0, 2fr) minmax(0, 1fr); gap: 12px; margin-bottom: 12px; }
    @media (max-width: 1000px) { .leases-quantum { grid-template-columns: 1fr; } }
    .bucket-section { margin-bottom: 12px; }
    .bucket-bars { display: flex; flex-direction: column; gap: 9px; padding: 4px 2px; }
    .bucket-row { display: grid; grid-template-columns: 92px 1fr 120px; align-items: center; gap: 12px; }
    .bucket-label { font: 700 12px "Cascadia Mono", monospace; color: #cbd5e3; letter-spacing: .3px; }
    .bucket-track { position: relative; height: 20px; background: rgba(141,160,184,.14); border-radius: 5px; overflow: hidden; }
    .bucket-fill { position: absolute; left: 0; top: 0; height: 100%; border-radius: 5px; transition: width .5s ease; min-width: 2px; }
    .bucket-pct { position: absolute; right: 7px; top: 2px; font: 700 11px "Cascadia Mono", monospace; color: #e6edf5; text-shadow: 0 1px 2px rgba(0,0,0,.6); }
    .bucket-counts { font: 600 11px "Cascadia Mono", monospace; color: #aebdd0; text-align: right; }
    .bucket-counts .muted { color: #6b7a8d; }
    .lease-tabs { display: flex; gap: 4px; margin-left: auto; }
    .lease-tab { height: 24px; padding: 0 9px; border: 1px solid var(--line); background: transparent; color: var(--muted); border-radius: 3px; font: 700 10.5px "Cascadia Mono", monospace; text-transform: uppercase; letter-spacing: .04em; }
    .lease-tab.active { color: var(--accent); border-color: var(--accent-dim); background: rgba(63,185,80,.1); }
    .lease-tab span { color: inherit; opacity: .8; }
    .prio { font: 700 9.5px "Cascadia Mono", monospace; text-transform: uppercase; padding: 1px 5px; border-radius: 3px; border: 1px solid var(--line-strong); color: var(--muted); }
    .prio-high { color: #d56a6a; border-color: #6e3030; }
    .prio-normal { color: #cbd6e4; }
    .prio-low { color: var(--quiet); }
    .quantum-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    .q-cell { border: 1px solid var(--line); border-radius: 3px; padding: 8px; background: var(--panel-2); }
    .q-k { font-size: 10px; text-transform: uppercase; letter-spacing: .04em; color: var(--quiet); }
    .q-v { font-size: 18px; font-weight: 800; color: var(--accent); margin-top: 3px; }

    /* ---- worker reports ---- */
    .report-filters { display: flex; flex-wrap: wrap; gap: 4px; margin-bottom: 8px; }
    .chip-btn { height: 21px; padding: 0 7px; border: 1px solid var(--line); background: transparent; color: var(--muted); border-radius: 3px; font: 700 10px "Cascadia Mono", monospace; text-transform: uppercase; letter-spacing: .03em; }
    .chip-btn.active { color: var(--accent); border-color: var(--accent-dim); background: rgba(63,185,80,.1); }
    .report-list { display: flex; flex-direction: column; gap: 5px; max-height: 460px; overflow-y: auto; }
    .report-card { border: 1px solid var(--line); border-left: 2px solid var(--line-strong); border-radius: 3px; padding: 7px 8px; background: var(--panel-2); }
    .report-top { display: flex; align-items: baseline; justify-content: space-between; gap: 8px; }
    .report-fn { font-size: 12px; font-weight: 700; color: var(--ink); white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    .report-chip { font: 700 9px "Cascadia Mono", monospace; text-transform: uppercase; letter-spacing: .04em; padding: 1px 6px; border-radius: 3px; white-space: nowrap; border: 1px solid var(--line-strong); color: var(--muted); }
    .report-chip.s-exact { color: var(--accent); border-color: var(--accent-dim); background: rgba(63,185,80,.1); }
    .report-chip.s-improved { color: #cbe6ff; border-color: #2c4a7f; background: rgba(92,145,223,.1); }
    .report-chip.s-committed { color: #b7c4d4; }
    .report-chip.s-needs-rework { color: #e0b24a; border-color: #6e5a26; background: rgba(224,178,74,.08); }
    .report-chip.s-no-progress { color: #98a3b3; }
    .report-chip.s-tool-error { color: #d56a6a; border-color: #6e3030; background: rgba(213,106,106,.08); }
    .report-card.s-exact { border-left-color: var(--accent); }
    .report-meta { font-size: 10.5px; color: var(--quiet); margin-top: 3px; font-family: "Cascadia Mono", Consolas, monospace; }
    .report-msg { font-size: 11px; color: var(--muted); margin-top: 3px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    /* #4: inline report timestamp + click-to-expand detail panel */
    .report-ts { font: 600 10px "Cascadia Mono", monospace; color: var(--quiet); white-space: nowrap; margin-left: auto; }
    .report-clickable { cursor: pointer; }
    .report-clickable:hover { border-left-color: var(--accent-dim); background: var(--panel); }
    .report-detail { display: none; margin-top: 6px; padding-top: 6px; border-top: 1px dashed var(--line); }
    .report-card.expanded .report-detail { display: block; }
    .report-card.expanded .report-msg { white-space: normal; }
    .report-dl { display: flex; gap: 8px; font-size: 10.5px; line-height: 1.5; }
    .report-dk { color: var(--quiet); min-width: 64px; text-transform: uppercase; letter-spacing: .03em; font-size: 9.5px; padding-top: 1px; }
    .report-dv { color: var(--muted); font-family: "Cascadia Mono", Consolas, monospace; word-break: break-word; }
    .report-full { white-space: pre-wrap; }

    /* ---- comprehensive log ---- */
    .log-toolbar { display: flex; flex-wrap: wrap; gap: 8px; align-items: center; margin-bottom: 9px; }
    .log-toolbar input[type="search"] { height: 28px; flex: 1 1 200px; min-width: 140px; }
    .log-kinds { display: flex; flex-wrap: wrap; gap: 4px; }
    .log-toolbar select { height: 28px; }
    .attempt-list.comprehensive .attempt-row { grid-template-columns: 78px 96px 92px 1fr; }
    .attempt-agent { color: var(--muted); font-size: 11px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    .attempt-row.k-match .attempt-kind { color: var(--accent); }
    .attempt-row.k-regression .attempt-kind { color: #d56a6a; }
    .attempt-row.k-commit .attempt-kind { color: #cbe6ff; }

    /* ---- match-progress range buttons ---- */
    .range-group { display: flex; gap: 3px; margin-left: auto; }
    .range-btn { height: 22px; padding: 0 8px; border: 1px solid var(--line); background: transparent; color: var(--muted); border-radius: 3px; font: 700 10px "Cascadia Mono", monospace; }
    .range-btn.active { color: var(--accent); border-color: var(--accent-dim); background: rgba(63,185,80,.1); }
    /* #2: pan/zoom affordances on the history chart */
    #history-chart { cursor: grab; }
    #history-chart.grabbing { cursor: grabbing; }
    .hist-reset { cursor: pointer; color: #e0b24a; border-color: #6e5a26; background: rgba(224,178,74,.08); margin-left: 6px; }
    /* #3/#4: active-work table + decomp-detail active-files block */
    .active-work-row.is-lease { box-shadow: inset 2px 0 0 var(--accent); }
    .active-work-click { cursor: pointer; }
    .active-work-click:hover { background: rgba(92,145,223,.10); }
    .sig-chip { display: inline-block; font: 700 9px "Cascadia Mono", monospace; text-transform: uppercase; letter-spacing: .03em; padding: 1px 5px; margin: 0 3px 0 0; border-radius: 3px; border: 1px solid var(--line-strong); color: var(--quiet); }
    .sig-lease { color: #cdebde; border-color: #2f6e5a; background: rgba(56,185,149,.12); }
    .sig-scratch { color: #cbe6ff; border-color: #2c4a7f; background: rgba(92,145,223,.12); }
    .sig-commit { color: #e0c08a; border-color: #6e5a26; background: rgba(224,178,74,.10); }
    .decomp-active-files { margin-top: 12px; }
    .active-files-list { display: flex; flex-direction: column; gap: 2px; margin-top: 6px; }
    .active-file-row { display: grid; grid-template-columns: minmax(0,1fr) 120px 96px 64px; gap: 8px; align-items: center; padding: 4px 6px; border-radius: 3px; font-size: 11px; border: 1px solid transparent; }
    .active-file-row:hover { border-color: var(--line); background: var(--panel-2); }
    .af-name { overflow: hidden; text-overflow: ellipsis; white-space: nowrap; color: var(--ink); }
    .af-pct { color: var(--muted); display: flex; align-items: center; gap: 6px; }
    .af-who { color: var(--quiet); overflow: hidden; text-overflow: ellipsis; white-space: nowrap; font-size: 10.5px; }
    .af-fresh { color: var(--quiet); text-align: right; font-size: 10px; }
    .attack-board,
    .kg-layout {
      display: grid;
      grid-template-columns: minmax(0, 1.25fr) minmax(320px, .75fr);
      gap: 12px;
      margin-bottom: 12px;
      align-items: stretch;
    }
    .attack-lanes {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 10px;
    }
    .attack-lane {
      min-width: 0;
      border: 1px solid #263244;
      border-radius: 7px;
      background: #101824;
      padding: 10px;
    }
    .attack-lane h3 {
      margin: 0 0 5px;
      color: #e8f1fb;
      font-size: 13px;
      line-height: 1.25;
    }
    .attack-count {
      font: 800 22px Consolas, "Courier New", monospace;
      color: var(--amber);
    }
    .attack-note {
      min-height: 32px;
      color: var(--quiet);
      font-size: 11px;
      line-height: 1.35;
    }
    .attack-targets {
      display: grid;
      gap: 5px;
      margin-top: 8px;
    }
    .attack-target {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      gap: 8px;
      color: #cbd7e6;
      font-size: 11px;
      border-top: 1px solid rgba(38, 50, 68, .72);
      padding-top: 5px;
    }
    .attack-target .mono {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    /* #1: clickable Attack Matrix cells drill into the unit close-out detail */
    .attack-target-click { cursor: pointer; border-radius: 3px; transition: background .12s ease, color .12s ease; }
    .attack-target-click:hover { background: rgba(92, 145, 223, .14); color: #eaf2ff; }
    .attack-target-click:hover .mono { color: #bcd6ff; }
    .attack-command {
      margin-top: 9px;
      color: #8da0b8;
      font: 11px Consolas, "Courier New", monospace;
      white-space: nowrap;
      overflow: hidden;
      text-overflow: ellipsis;
    }
    .pipeline-flow {
      display: grid;
      grid-template-columns: repeat(5, minmax(0, 1fr));
      gap: 8px;
    }
    .pipe-step {
      position: relative;
      min-height: 92px;
      border: 1px solid #263244;
      border-radius: 7px;
      background: linear-gradient(180deg, #111b28, #0d141f);
      padding: 9px;
      overflow: hidden;
    }
    .pipe-step::after {
      content: "";
      position: absolute;
      left: 0; right: 0; bottom: 0;
      height: 3px;
      background: var(--accent, var(--cobalt));
    }
    .pipe-k {
      color: var(--muted);
      font-size: 10px;
      font-weight: 800;
      text-transform: uppercase;
    }
    .pipe-v {
      margin-top: 7px;
      color: #eef4fb;
      font: 800 26px Consolas, "Courier New", monospace;
    }
    .pipe-note {
      margin-top: 2px;
      color: var(--quiet);
      font-size: 11px;
      overflow-wrap: anywhere;
    }
    .ship-wins {
      display: grid;
      gap: 6px;
      margin-top: 8px;
      max-height: 180px;
      overflow: auto;
      padding-right: 4px;
    }
    .ship-win {
      border-top: 1px solid #263244;
      padding-top: 6px;
      font-size: 11px;
      color: var(--muted);
    }
    .ship-win b {
      color: #dfe8f4;
      font-family: Consolas, "Courier New", monospace;
    }
    .ship-win .mono {
      display: block;
      margin-top: 2px;
      color: #8da0b8;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .limit-models,
    .model-strip {
      display: flex;
      flex-wrap: wrap;
      gap: 4px;
      margin-top: 7px;
    }
    .model-chip {
      border: 1px solid #314157;
      border-radius: 999px;
      padding: 2px 6px;
      color: #cbd7e6;
      background: #0d141f;
      font: 10.5px Consolas, "Courier New", monospace;
      max-width: 100%;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    .model-chip.assumed { border-color: rgba(240,179,90,.55); color: #ffd28a; }
    #kg-graph {
      height: 300px;
      background: radial-gradient(circle at 50% 50%, rgba(56,185,149,.10), rgba(13,17,24,0) 58%);
      border: 1px solid #263244;
      border-radius: 7px;
    }
    .kg-side {
      display: grid;
      gap: 10px;
    }
    .kg-stats {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 7px;
    }
    .kg-stat {
      border: 1px solid #263244;
      border-radius: 6px;
      background: #101824;
      padding: 8px;
    }
    .kg-stat b {
      display: block;
      color: #eef4fb;
      font: 800 18px Consolas, "Courier New", monospace;
    }
    .kg-stat span {
      color: var(--quiet);
      font-size: 10.5px;
      text-transform: uppercase;
      font-weight: 760;
    }
    .kg-list {
      display: grid;
      gap: 5px;
      max-height: 190px;
      overflow: auto;
    }
    .kg-row {
      display: grid;
      grid-template-columns: minmax(0, 1fr) auto;
      gap: 8px;
      padding: 5px 0;
      border-top: 1px solid #263244;
      color: #cbd7e6;
      font-size: 11px;
    }
    .kg-row:first-child { border-top: 0; }
    .kg-row .mono {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
    @media (max-width: 1180px) {
      .attack-board,
      .kg-layout { grid-template-columns: 1fr; }
      .attack-lanes { grid-template-columns: repeat(2, minmax(0, 1fr)); }
      .pipeline-flow { grid-template-columns: repeat(3, minmax(0, 1fr)); }
    }
    @media (max-width: 720px) {
      .attack-lanes,
      .pipeline-flow,
      .kg-stats { grid-template-columns: 1fr; }
      #kg-graph { height: 240px; }
    }
  </style>
</head>
<body>
  <div class="app">
    <aside class="rail rail-left">
      <div class="rail-brand">
        <span class="rail-brand-mark">&#9707;</span>
        <div>
          <h1>Decomp Control</h1>
          <div class="subtitle">
            <span id="repo"></span>
            <span id="head"></span>
            <span id="updated"></span>
          </div>
        </div>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">1</span> Status</div>
        <div class="stat-list">
          <div class="stat-row"><span class="stat-k">code</span><span class="stat-v hud-good" id="hud-code">0%</span></div>
          <div class="stat-row"><span class="stat-k">fuzzy</span><span class="stat-v" id="hud-fuzzy">0%</span></div>
          <div class="stat-row"><span class="stat-k">fns</span><span class="stat-v" id="hud-fns">0%</span></div>
          <div class="stat-row"><span class="stat-k">C converted</span><span class="stat-v" id="hud-converted">0%</span></div>
          <div class="stat-row"><span class="stat-k">open fns</span><span class="stat-v hud-warn" id="hud-openfns">0</span></div>
          <div class="stat-row"><span class="stat-k">units</span><span class="stat-v" id="hud-units">0/0</span></div>
          <div class="stat-row"><span class="stat-k">leases</span><span class="stat-v" id="hud-active-leases">0</span></div>
          <div class="stat-row"><span class="stat-k">queued</span><span class="stat-v" id="hud-queued">0</span></div>
        </div>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">2</span> Controls</div>
        <button class="btn primary block" id="refresh" type="button">Refresh now</button>
        <label class="ctl-row"><input id="auto-refresh" type="checkbox" checked> live auto-refresh</label>
        <label class="ctl-row">every
          <select id="refresh-rate">
            <option value="5000">5 sec</option>
            <option value="15000">15 sec</option>
            <option value="30000">30 sec</option>
          </select>
        </label>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">3</span> View</div>
        <nav class="tabs" aria-label="Dashboard views">
          <button class="tab-btn active" id="tab-decomp" type="button" data-view="decomp">Decomp</button>
          <button class="tab-btn" id="tab-files" type="button" data-view="files">Files</button>
          <button class="tab-btn" id="tab-symbols" type="button" data-view="symbols">Symbol Map</button>
        </nav>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">1</span> Sync <span class="rc-note" id="sync-summary"></span></div>
        <div class="kv"><span class="kv-k">branch</span><span class="kv-v mono" id="sync-branch">-</span></div>
        <div class="kv"><span class="kv-k">head</span><span class="kv-v mono" id="sync-head">-</span></div>
        <div class="kv"><span class="kv-k">ahead/behind</span><span class="kv-v mono" id="sync-ab">-</span></div>
        <div class="kv"><span class="kv-k">dirty files</span><span class="kv-v mono" id="sync-dirty">-</span></div>
        <button class="btn ghost block sm" id="sync-fetch" type="button">Fetch origin</button>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">2</span> Run <span class="rc-note" id="run-note">display-only</span></div>
        <div class="run-controls">
          <button class="btn ghost sm" type="button" disabled title="Wire your fleet launcher to enable">&#9654; Resume</button>
          <button class="btn ghost sm" type="button" disabled title="Wire your fleet launcher to enable">&#10073;&#10073; Pause</button>
          <button class="btn ghost sm" type="button" disabled title="Wire your fleet launcher to enable">&#9211; Kill</button>
        </div>
        <div class="kv"><span class="kv-k">active leases</span><span class="kv-v mono" id="run-active">0</span></div>
        <div class="kv"><span class="kv-k">queued</span><span class="kv-v mono" id="run-queued">0</span></div>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">3</span> Ship <span class="rc-note" id="ship-state"></span></div>
        <div class="kv"><span class="kv-k">confirmed wins</span><span class="kv-v mono" id="ship-confirmed">0</span></div>
        <div class="kv"><span class="kv-k">regressions</span><span class="kv-v mono" id="ship-regress">0</span></div>
        <button class="btn primary block sm" id="ship-handoff" type="button" title="Push current branch and open a PR">&#9094; Prepare handoff (PR)</button>
        <div class="rc-hint" id="ship-hint"></div>
        <div class="ship-wins" id="ship-wins"></div>
      </div>

      <div class="rail-card">
        <div class="rail-card-head"><span class="rail-num">4</span> PRs <span class="rc-note" id="prs-note"></span></div>
        <div class="pr-list" id="pr-list"></div>
        <button class="btn ghost block sm" id="prs-refresh" type="button">Sync PR status</button>
      </div>

      <div class="rail-card">
        <div class="rail-card-head">Project</div>
        <div class="proj-name" id="hud-project">GC6E01/DECOMP</div>
        <div class="proj-sub">Pok&eacute;mon Colosseum &middot; CodeWarrior 1.3 &middot; -O4,p</div>
        <a class="rail-link" id="link-pokedex" href="#" target="_blank" rel="noopener">&#9656; Pok&eacute;dex dashboard</a>
      </div>
    </aside>

    <main class="work">
    <section class="view active" id="view-decomp">
      <section class="metric-grid" id="decomp-metrics"></section>

      <section class="attack-board">
        <div class="panel">
          <div class="panel-title">
            <h2>Attack Matrix</h2>
            <span class="panel-note" id="attack-note"></span>
          </div>
          <div class="attack-lanes" id="attack-lanes"></div>
        </div>
        <div class="panel">
          <div class="panel-title">
            <h2>Pipeline</h2>
            <span class="panel-note" id="pipeline-note"></span>
          </div>
          <div class="pipeline-flow" id="pipeline-flow"></div>
        </div>
      </section>

      <section class="history-layout">
        <div class="panel chart-card">
          <div class="panel-title">
            <h2>Match Progress Over Time</h2>
            <span class="panel-note" id="timeline-range"></span>
            <div class="range-group" id="history-range" role="group" aria-label="Time range">
              <button class="range-btn" type="button" data-days="0.003472">5m</button>
              <button class="range-btn" type="button" data-days="0.010417">15m</button>
              <button class="range-btn" type="button" data-days="0.020833">30m</button>
              <button class="range-btn" type="button" data-days="0.041667">1h</button>
              <button class="range-btn" type="button" data-days="0.166667">4h</button>
              <button class="range-btn" type="button" data-days="1">24h</button>
              <button class="range-btn" type="button" data-days="3">3d</button>
              <button class="range-btn" type="button" data-days="7">7d</button>
              <button class="range-btn active" type="button" data-days="14">14d</button>
              <button class="range-btn" type="button" data-days="0">All</button>
            </div>
          </div>
          <canvas id="history-chart" height="205"></canvas>
        </div>
      </section>

      <section class="bucket-section">
        <div class="panel" id="bucket-panel">
          <div class="panel-title">
            <h2>Campaign Buckets — coverage</h2>
            <span class="panel-note" id="bucket-note"></span>
          </div>
          <div id="bucket-bars" class="bucket-bars"></div>
        </div>
      </section>

      <section class="leases-quantum">
        <div class="panel">
          <div class="panel-title">
            <h2>Leases</h2>
            <div class="lease-tabs" id="lease-tabs" role="tablist">
              <button class="lease-tab active" type="button" data-lease="active">Active <span id="lease-active-n">0</span></button>
              <button class="lease-tab" type="button" data-lease="queued">Queued <span id="lease-queued-n">0</span></button>
            </div>
          </div>
          <div class="table-wrap lease-pane" id="lease-active-pane">
            <table class="agent-table">
              <thead><tr><th>Scope</th><th>Agent</th><th>File</th><th>Function</th><th>Att</th><th>Elapsed</th><th>TTL</th></tr></thead>
              <tbody id="lease-active-body"></tbody>
            </table>
          </div>
          <div class="table-wrap lease-pane" id="lease-queued-pane" hidden>
            <table class="agent-table">
              <thead><tr><th>Function</th><th>Priority</th><th>Description</th></tr></thead>
              <tbody id="lease-queued-body"></tbody>
            </table>
          </div>
        </div>

        <!-- #4: robust who-is-working-on-what (locks + scratch + commits union) -->
        <div class="panel" id="active-work-panel">
          <div class="panel-title">
            <h2>Active Work</h2>
            <span class="panel-note" id="active-work-note"></span>
          </div>
          <div class="table-wrap">
            <table class="agent-table">
              <thead><tr><th>File</th><th>Live %</th><th>Who</th><th>Signal</th><th>Fresh</th></tr></thead>
              <tbody id="active-work-body"></tbody>
            </table>
          </div>
        </div>

        <div class="panel" id="quantum-panel">
          <div class="panel-title">
            <h2>Annealer</h2>
            <span class="panel-note" id="quantum-note">permuter swarm</span>
          </div>
          <div class="quantum-grid" id="quantum-grid"></div>
        </div>
      </section>

      <section class="decomp-workspace">
        <div class="panel tu-panel">
          <div class="panel-title">
            <h2>Treemap</h2>
            <span class="panel-note" id="decomp-note"></span>
          </div>
          <div class="treemap-toolbar">
            <nav class="crumbs" id="decomp-crumbs" aria-label="Treemap breadcrumb"></nav>
            <div class="treemap-controls">
              <input id="decomp-query" type="search" placeholder="Filter file or source">
              <select id="decomp-difficulty" title="Filter function rows by hardest-target catalog">
                <option value="all">All difficulty</option>
                <option value="hard">Hardest catalog</option>
                <option value="asm">Asm giants</option>
                <option value="near">Real-C near misses</option>
                <option value="wall">Confirmed walls</option>
                <option value="giant">Structural giants</option>
              </select>
              <label class="area-toggle"><input id="decomp-area-fns" type="checkbox"> area by fn count</label>
              <button class="btn" id="decomp-near" type="button">Near Match</button>
              <button class="btn" id="decomp-clear" type="button">Clear</button>
            </div>
            <div class="treemap-legend" aria-label="treemap animation legend">
              <span class="tm-leg"><i class="sw glow"></i>new match</span>
              <span class="tm-leg"><i class="sw agent"></i>agent working</span>
              <span class="tm-leg"><i class="sw anneal"></i>permuter annealing</span>
            </div>
          </div>
          <div class="treemap-shell">
            <canvas id="decomp-treemap"></canvas>
            <div class="treemap-tip" id="decomp-tip" hidden></div>
          </div>
        </div>

        <div class="decomp-side">
          <aside class="detail-panel decomp-detail" id="decomp-details"></aside>
          <div class="panel chart-card" id="file-progress-card">
            <div class="panel-title">
              <h2>Selected File Progress</h2>
              <span class="panel-note" id="file-history-note"></span>
            </div>
            <canvas id="file-history-chart" height="205"></canvas>
          </div>
        </div>
      </section>

      <div class="treemap-pin" id="treemap-pin" role="dialog" aria-label="Tile detail">
        <div class="pin-body" id="treemap-pin-body"></div>
        <div class="treemap-pin-actions">
          <button class="btn open" id="treemap-pin-open" type="button">Open &#9656;</button>
          <button class="btn ghost" id="treemap-pin-close" type="button">Dismiss</button>
        </div>
      </div>

      <section class="panel reader-overlay" id="reader-overlay">
        <div class="panel-title">
          <h2>Function Reader</h2>
          <button class="btn ghost" id="reader-back" type="button">&#8592; Back to treemap</button>
        </div>
        <div class="reader-head">
          <span class="reader-fn" id="reader-fn">fn_</span>
          <span class="reader-pct" id="reader-pct">--%</span>
          <span class="reader-meta" id="reader-meta"></span>
        </div>
        <div class="reader-grid">
          <div>
            <div class="asm-pane">
              <div class="asm-colhead">
                <span></span>
                <span class="target">Target (aim for)</span>
                <span class="current">Current build</span>
              </div>
              <div class="asm-body" id="asm-body"></div>
            </div>
            <div class="asm-legend">
              <span class="legend-item"><span class="swatch" style="--swatch:#5c91df"></span>match</span>
              <span class="legend-item"><span class="swatch" style="--swatch:#e07171"></span>differs</span>
              <span class="legend-item"><span class="swatch" style="--swatch:#f0b35a"></span>insert / delete</span>
            </div>
          </div>
          <aside class="reader-side" id="reader-side"></aside>
        </div>
      </section>

      <section class="panel">
        <div class="panel-title">
          <h2>Activity Log</h2>
          <span class="panel-note" id="decomp-log-note"></span>
        </div>
        <div class="log-toolbar">
          <input id="log-query" type="search" placeholder="Filter message, fn, agent">
          <div class="log-kinds" id="log-kinds">
            <button class="chip-btn active" type="button" data-kind="all">all</button>
            <button class="chip-btn" type="button" data-kind="match">match</button>
            <button class="chip-btn" type="button" data-kind="commit">commit</button>
            <button class="chip-btn" type="button" data-kind="regression">regression</button>
            <button class="chip-btn" type="button" data-kind="report">report</button>
            <button class="chip-btn" type="button" data-kind="claim">claim</button>
          </div>
          <select id="log-limit" title="Rows shown">
            <option value="80">80 rows</option>
            <option value="200">200 rows</option>
            <option value="500">500 rows</option>
            <option value="100000">all</option>
          </select>
        </div>
        <div class="attempt-list comprehensive" id="decomp-log"></div>
      </section>

      <section class="panel chart-card">
        <div class="panel-title">
          <h2>Token Expense Over Time</h2>
          <span class="panel-note" id="tokens-note"></span>
        </div>
        <div class="token-stats" id="tokens-stats"></div>
        <canvas id="tokens-chart" height="205"></canvas>
      </section>

      <section class="panel">
        <div class="panel-title">
          <h2>Token Spend by Function</h2>
          <span class="panel-note" id="fn-tokens-note"></span>
        </div>
        <div id="fn-tokens-body"></div>
      </section>

      <section class="kg-layout">
        <div class="panel chart-card">
          <div class="panel-title">
            <h2>Knowledge Graph</h2>
            <span class="panel-note" id="kg-note"></span>
          </div>
          <canvas id="kg-graph" height="300"></canvas>
        </div>
        <div class="panel kg-side">
          <div>
            <div class="panel-title">
              <h2>KG Targets</h2>
              <span class="panel-note" id="kg-target-note"></span>
            </div>
            <div class="kg-stats" id="kg-stats"></div>
          </div>
          <div>
            <div class="limit-label">Top reusable levers</div>
            <div class="kg-list" id="kg-levers"></div>
          </div>
          <div>
            <div class="limit-label">Calltags / name evidence</div>
            <div class="kg-list" id="kg-tags"></div>
          </div>
          <div>
            <div class="limit-label">Near-match targets</div>
            <div class="kg-list" id="kg-targets"></div>
          </div>
          <div>
            <div class="limit-label">Call relationships</div>
            <div class="kg-list" id="kg-edges"></div>
          </div>
        </div>
      </section>
    </section>

    <section class="view" id="view-files">
      <section class="panel">
        <div class="panel-title">
          <h2>Translation Units &amp; File Sizes</h2>
          <span class="panel-note" id="files-note"></span>
        </div>
        <div class="files-toolbar">
          <input id="files-query" type="search" placeholder="Filter source path">
          <label class="area-toggle"><input id="files-incomplete" type="checkbox"> only incomplete</label>
          <button class="btn" id="files-clear" type="button">Clear</button>
        </div>
        <div class="table-wrap">
          <table id="files-table">
            <thead>
              <tr>
                <th data-fsort="source">Source Path</th>
                <th data-fsort="total_code" class="num">Bytes</th>
                <th data-fsort="matched_code" class="num">Matched</th>
                <th data-fsort="code_pct" class="num">Code %</th>
                <th data-fsort="total_functions" class="num">Fns</th>
                <th data-fsort="matched_functions" class="num">Matched Fns</th>
                <th data-fsort="functions_pct" class="num">Fns %</th>
                <th data-fsort="fuzzy_pct" class="num">Fuzzy %</th>
                <th data-fsort="complete">Complete</th>
              </tr>
            </thead>
            <tbody id="files-body"></tbody>
            <tfoot><tr id="files-foot"></tr></tfoot>
          </table>
        </div>
      </section>
    </section>

    <section class="view" id="view-symbols">
      <section class="metric-grid" id="symbol-metrics"></section>

      <section class="overview">
        <div class="panel">
          <div class="panel-title">
            <h2>Current Target</h2>
            <span class="panel-note" id="progress-label"></span>
          </div>
          <div class="target-line">
            <div class="target-badge" id="next-rank">NEXT</div>
            <div>
              <div class="target-name mono" id="next-name"></div>
              <div class="target-meta" id="next-detail"></div>
            </div>
            <span class="pill needs" id="next-status">Needs wiring</span>
          </div>
          <div class="progress-shell"><div class="progress-fill" id="progress-fill"></div></div>
          <div class="legend" id="status-legend"></div>
        </div>
        <div class="panel">
          <div class="panel-title">
            <h2>Evidence Mix</h2>
            <span class="panel-note" id="history-points"></span>
          </div>
          <canvas id="provenance-chart" height="205"></canvas>
          <div class="source-bars" id="source-bars"></div>
        </div>
      </section>

      <section class="panel chart-card">
        <div class="panel-title">
          <h2>Status Distribution</h2>
          <span class="panel-note" id="status-total"></span>
        </div>
        <canvas id="status-chart" height="205"></canvas>
      </section>

      <section class="ops-grid">
        <div class="panel">
          <div class="panel-title">
            <h2>Live Activity</h2>
            <span class="panel-note" id="activity-note"></span>
          </div>
          <div class="feed" id="activity-feed"></div>
        </div>
        <div class="panel">
          <div class="panel-title">
            <h2>Recent Commits</h2>
            <span class="panel-note" id="commit-note"></span>
          </div>
          <div class="commit-list" id="commit-list"></div>
        </div>
      </section>

      <section class="panel tu-panel">
        <div class="panel-title">
          <h2>Translation Units</h2>
          <span class="panel-note" id="tu-note"></span>
        </div>
        <div class="tu-toolbar">
          <input id="tu-query" type="search" placeholder="Filter TU name or source">
          <button class="btn" id="tu-needs" type="button">Needs Wiring</button>
          <button class="btn" id="tu-clear" type="button">Clear</button>
        </div>
        <div class="tu-map" id="tu-map"></div>
      </section>

      <section class="workbench">
        <div class="panel">
          <div class="panel-title">
            <h2>Targets</h2>
            <span class="panel-note" id="row-count"></span>
          </div>
          <div class="toolbar">
            <input id="query" type="search" placeholder="Filter symbol, source, evidence">
            <select id="status-filter">
              <option value="">All statuses</option>
              <option value="Needs wiring">Needs wiring</option>
              <option value="Proposed">Proposed</option>
              <option value="Recorded">Recorded</option>
              <option value="Renamed">Renamed</option>
            </select>
            <select id="source-filter">
              <option value="">All sources</option>
            </select>
            <button class="btn" id="clear" type="button">Clear</button>
          </div>
          <div class="table-wrap">
            <table>
              <thead>
                <tr>
                  <th data-sort="status">Status</th>
                  <th data-sort="fn">Function</th>
                  <th data-sort="name">Name</th>
                  <th data-sort="size">Size</th>
                  <th data-sort="source">Source</th>
                  <th data-sort="current_symbol">Current</th>
                  <th data-sort="old_refs">Old Refs</th>
                  <th data-sort="provenance">Evidence</th>
                </tr>
              </thead>
              <tbody id="targets"></tbody>
            </table>
          </div>
        </div>

        <aside class="detail-panel" id="details"></aside>
      </section>
    </section>
    </main>

    <aside class="rail rail-right">
      <div class="rail-head"><span>Details</span><span class="rail-head-note">worker reports</span></div>

      <div class="panel">
        <div class="panel-title">
          <h2>Worker Reports</h2>
          <span class="panel-note" id="reports-note"></span>
        </div>
        <div class="report-filters" id="report-filters"></div>
        <div class="report-list" id="reports-body"></div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <h2>Agent Activity</h2>
          <span class="panel-note" id="agents-note"></span>
        </div>
        <div class="table-wrap">
          <table class="agent-table">
            <thead>
              <tr><th>Agent</th><th>Function</th><th>File</th><th>Claimed</th><th>State</th></tr>
            </thead>
            <tbody id="agents-body"></tbody>
          </table>
        </div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <h2>Fleet Locks</h2>
          <span class="panel-note" id="locks-note"></span>
          <button class="btn ghost panel-action" id="locks-gc" type="button"
                  title="Purge expired locks from the DB">Purge expired</button>
        </div>
        <div class="table-wrap">
          <table class="agent-table lock-table">
            <thead>
              <tr><th>Scope</th><th>Key</th><th>Owner</th><th>TTL</th><th></th></tr>
            </thead>
            <tbody id="locks-body"></tbody>
          </table>
        </div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <h2>Agent Token Usage</h2>
          <span class="panel-note" id="agent-usage-note"></span>
        </div>
        <div class="bucket-bars" id="agent-usage-bars"></div>
      </div>

      <div class="panel">
        <div class="panel-title">
          <h2>Lockout Resets</h2>
          <span class="panel-note" id="limits-note"></span>
        </div>
        <div class="limit-grid" id="limits-grid"></div>
      </div>
    </aside>
  </div>
  <script>
    const store = {
      data: null,
      rows: [],
      filtered: [],
      selectedFn: null,
      sortKey: "",
      sortDir: 1,
      attention: false,
      tuNeedsOnly: false,
      decompNearOnly: false,
      selectedUnitKey: "",
      activeView: "decomp",
      refreshTimer: null,
      // Treemap drill state. level: "files" -> "unit" -> "fn".
      tm: {
        level: "files",
        rects: [],
        unitSource: "",
        unitName: "",
        unitFns: null,
        unitFnsSource: "",
        selectedFn: "",
        areaByFns: false,
        difficultyFilter: "all"
      },
      filesSort: "total_code",
      filesDir: -1,
      fnHistoryCache: {},
      // v9: function reader + agent/lockout/token panels
      reader: { open: false, fn: "", source: "" },
      crack: { lane: "codex-gpt55-high", strategy: "contenders", jobs: [], terminalPane: "codex" },
      limits: [],
      agentsTimer: null,
      limitsTimer: null,
      crackTimer: null,
      kgTimer: null,
      // v10: touch two-step treemap + live attempt-log poll
      isTouch: false,
      pinnedItem: null,
      _logData: null,
      _logUnit: null,
      logTimer: null
    };
    // Touch devices have no hover, so a single tap must NOT immediately drill in
    // (the user could never read the tile detail). Detect coarse pointers.
    store.isTouch = (typeof window.matchMedia === "function" && window.matchMedia("(pointer: coarse)").matches)
      || ("ontouchstart" in window) || (navigator.maxTouchPoints > 0);
    const statusClass = {
      "Needs wiring": "needs",
      "Proposed": "proposed",
      "Recorded": "recorded",
      "Renamed": "renamed"
    };
    const palette = {
      "Needs wiring": "#f0b35a",
      "Proposed": "#5c91df",
      "Recorded": "#8da0b8",
      "Renamed": "#38b995",
      "XD port": "#38b995",
      "String evidence": "#f0b35a",
      "Structural": "#5c91df",
      "Other": "#a98ee6"
    };
    function $(id) {
      return document.getElementById(id);
    }
    function setText(node, value) {
      node.textContent = value == null || value === "" ? "-" : String(value);
    }
    function number(value) {
      const text = String(value || "0");
      const parsed = text.toLowerCase().startsWith("0x") ? parseInt(text, 16) : Number(text);
      return Number.isFinite(parsed) ? parsed : 0;
    }
    function shortSource(source) {
      if (!source) return "unknown";
      const normalized = String(source).replaceAll("\\", "/");
      return normalized.split("/").pop().replace(/\.c$/, "");
    }
    function tileColor(percent) {
      const pct = Number(percent || 0);
      if (pct >= 99.95) return "#39c95e";
      if (pct >= 90) return "#2f9b48";
      if (pct >= 70) return "#78901b";
      if (pct > 0) return "#c68b25";
      return "#8a2525";
    }
    function pctText(value) {
      return `${Number(value || 0).toFixed(1)}%`;
    }
    function unitKey(row) {
      return `${row.name || ""}|${row.source || ""}`;
    }
    function fileName(source) {
      if (!source) return "unknown";
      return String(source).replaceAll("\\", "/").split("/").pop();
    }
    function unitDisplayName(row) {
      return fileName(row.source) !== "unknown" ? fileName(row.source) : (row.name || "unknown");
    }
    function functionStatusLabel(status) {
      if (status === "matched") return "100";
      if (status === "near") return "near";
      if (status === "partial") return "partial";
      return "missing";
    }
    function statusChip(status) {
      const chip = document.createElement("span");
      chip.className = `status-chip ${status || "missing"}`;
      setText(chip, functionStatusLabel(status));
      return chip;
    }
    function metric(label, value, note, color) {
      const box = document.createElement("div");
      box.className = "metric";
      box.style.setProperty("--accent", color);
      const labelNode = document.createElement("div");
      labelNode.className = "metric-label";
      setText(labelNode, label);
      const valueNode = document.createElement("div");
      valueNode.className = "metric-value";
      setText(valueNode, value);
      const noteNode = document.createElement("div");
      noteNode.className = "metric-note";
      setText(noteNode, note);
      box.append(labelNode, valueNode, noteNode);
      return box;
    }
    function pill(status) {
      const el = document.createElement("span");
      el.className = `pill ${statusClass[status] || "recorded"}`;
      setText(el, status);
      return el;
    }
    function td(value, className) {
      const cell = document.createElement("td");
      if (className) cell.className = className;
      setText(cell, value);
      return cell;
    }
    function code(value) {
      const node = document.createElement("code");
      setText(node, value);
      return node;
    }
    function fitCanvas(canvas) {
      const rect = canvas.getBoundingClientRect();
      const ratio = window.devicePixelRatio || 1;
      const width = Math.max(240, Math.floor(rect.width * ratio));
      const height = Math.max(180, Math.floor(rect.height * ratio));
      if (canvas.width !== width || canvas.height !== height) {
        canvas.width = width;
        canvas.height = height;
      }
      const ctx = canvas.getContext("2d");
      ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
      return { ctx, w: width / ratio, h: height / ratio };
    }
    function drawEmpty(ctx, w, h, label) {
      ctx.clearRect(0, 0, w, h);
      ctx.fillStyle = "#728095";
      ctx.font = "12px Segoe UI, Arial";
      ctx.textAlign = "center";
      ctx.fillText(label, w / 2, h / 2);
    }
    function drawDonut(canvas, items) {
      const { ctx, w, h } = fitCanvas(canvas);
      ctx.clearRect(0, 0, w, h);
      const total = items.reduce((sum, row) => sum + Number(row.value || 0), 0);
      if (!total) return drawEmpty(ctx, w, h, "No target data");
      const cx = w * .38;
      const cy = h * .52;
      const radius = Math.min(w, h) * .34;
      const inner = radius * .58;
      let angle = -Math.PI / 2;
      for (const item of items) {
        const value = Number(item.value || 0);
        if (!value) continue;
        const next = angle + (Math.PI * 2 * value / total);
        ctx.beginPath();
        ctx.moveTo(cx, cy);
        ctx.arc(cx, cy, radius, angle, next);
        ctx.closePath();
        ctx.fillStyle = palette[item.label] || "#a98ee6";
        ctx.fill();
        angle = next;
      }
      ctx.globalCompositeOperation = "destination-out";
      ctx.beginPath();
      ctx.arc(cx, cy, inner, 0, Math.PI * 2);
      ctx.fill();
      ctx.globalCompositeOperation = "source-over";
      ctx.fillStyle = "#eef4fb";
      ctx.font = "700 24px Segoe UI, Arial";
      ctx.textAlign = "center";
      ctx.fillText(String(total), cx, cy + 5);
      ctx.fillStyle = "#a8b4c4";
      ctx.font = "12px Segoe UI, Arial";
      ctx.fillText("targets", cx, cy + 25);
      let y = 26;
      ctx.textAlign = "left";
      for (const item of items) {
        ctx.fillStyle = palette[item.label] || "#a98ee6";
        ctx.fillRect(w * .68, y - 9, 10, 10);
        ctx.fillStyle = "#cbd5e3";
        ctx.font = "12px Segoe UI, Arial";
        ctx.fillText(`${item.label}: ${item.value}`, w * .68 + 16, y);
        y += 22;
      }
    }
    function drawBars(canvas, items) {
      const { ctx, w, h } = fitCanvas(canvas);
      ctx.clearRect(0, 0, w, h);
      const max = Math.max(...items.map(row => Number(row.value || 0)), 0);
      if (!max) return drawEmpty(ctx, w, h, "No evidence data");
      const left = 108;
      const right = 20;
      const top = 18;
      const barH = 18;
      const gap = 12;
      ctx.font = "12px Segoe UI, Arial";
      items.forEach((item, idx) => {
        const y = top + idx * (barH + gap);
        const width = (w - left - right) * Number(item.value || 0) / max;
        ctx.fillStyle = "#a8b4c4";
        ctx.textAlign = "right";
        ctx.fillText(item.label, left - 10, y + 13);
        ctx.fillStyle = "#0d131d";
        ctx.fillRect(left, y, w - left - right, barH);
        ctx.fillStyle = palette[item.label] || "#a98ee6";
        ctx.fillRect(left, y, width, barH);
        ctx.fillStyle = "#eef4fb";
        ctx.textAlign = "left";
        ctx.fillText(String(item.value), left + width + 6, y + 13);
      });
    }
    // ---- Unified time-series chart: fixed elapsed-hours x-axis (anchored at 0,
    //      does not scroll), % gridline labels on the Y axis, and a hover
    //      crosshair + tooltip. Drives BOTH the global match-progress chart and
    //      the per-file/per-fn selected-file chart so they read the same. ------
    function _chartTip() {
      let t = document.getElementById("chart-tip");
      if (!t) {
        t = document.createElement("div");
        t.id = "chart-tip";
        t.style.cssText = "position:fixed;z-index:60;pointer-events:none;display:none;" +
          "background:#0d131d;border:1px solid #2d3a4b;border-radius:6px;padding:6px 9px;" +
          "font:12px Segoe UI,Arial;color:#eef4fb;box-shadow:0 4px 14px rgba(0,0,0,.5)";
        document.body.appendChild(t);
      }
      return t;
    }
    function _bindChartHover(canvas) {
      if (canvas._hoverBound) return;
      canvas._hoverBound = true;
      canvas.addEventListener("mousemove", evt => {
        const c = canvas._chart;
        const tip = _chartTip();
        if (!c || !c.rows.length) { tip.style.display = "none"; return; }
        const rect = canvas.getBoundingClientRect();
        const mx = evt.clientX - rect.left;
        if (mx < c.pad.l - 6 || mx > c.w - c.pad.r + 6) { tip.style.display = "none"; return; }
        let bi = 0, bd = 1e9;
        c.rows.forEach((r, i) => { const d = Math.abs(c.x(Number(r.unix)) - mx); if (d < bd) { bd = d; bi = i; } });
        const r = c.rows[bi];
        const lines = c.active.map(it => {
          const v = _seriesVal(r, it.key);
          return `<span style="color:${it.color}">&#9632;</span> ${it.label}: ${Number.isFinite(v) ? v.toFixed(2) + "%" : "-"}`;
        }).join("<br>");
        tip.innerHTML = `<b>${c.fmtH(r.unix)}</b> &middot; ${fmtTime(r.unix)}<br>${lines}`;
        tip.style.display = "block";
        tip.style.left = (evt.clientX + 14) + "px";
        tip.style.top = (evt.clientY + 14) + "px";
      });
      canvas.addEventListener("mouseleave", () => { _chartTip().style.display = "none"; });
    }
    // A row's value for a series is finite-or-GAP: backfilled history rows only
    // carry decomp_* keys, so completion_pct/symbols are missing there. Treat a
    // missing/non-finite raw value as a gap (NaN) -> skip the point, break the
    // line -- never plot it as 0 (which would drag the line down to the axis).
    function _seriesVal(row, key) {
      const raw = row == null ? undefined : row[key];
      const n = Number(raw);
      return (raw === null || raw === undefined || !Number.isFinite(n)) ? NaN : n;
    }
    // `domain` (optional) = [xMin,xMax] unix window override for pan/zoom (#2).
    // When given, the x-scale uses it instead of the data's own min/max, so the
    // caller can slide/zoom the time axis while the full row set stays available
    // for hover. Rows are still all drawn; points outside the window just map off
    // the plot area and are clipped below.
    function _drawTimeChart(canvas, rows, series, emptyLabel, domain) {
      const { ctx, w, h } = fitCanvas(canvas);
      ctx.clearRect(0, 0, w, h);
      rows = (rows || []).filter(r => r && Number.isFinite(Number(r.unix)));
      // Keep a series only if it has at least one finite point somewhere.
      const active = series.filter(it => rows.some(r => Number.isFinite(_seriesVal(r, it.key))));
      if (rows.length < 2 || !active.length) { canvas._chart = null; return drawEmpty(ctx, w, h, emptyLabel || "No history yet"); }
      const pad = { l: 46, r: 16, t: 18, b: 30 };
      const xs = rows.map(r => Number(r.unix || 0));
      const ys = rows.flatMap(r => active.map(it => _seriesVal(r, it.key))).filter(Number.isFinite);
      const dataMinX = Math.min(...xs), dataMaxX = Math.max(...xs);
      // Window: explicit domain (pan/zoom) clamped to a sane span, else full data.
      let minX = dataMinX, maxX = dataMaxX;
      if (Array.isArray(domain) && Number.isFinite(domain[0]) && Number.isFinite(domain[1]) && domain[1] > domain[0]) {
        minX = domain[0]; maxX = domain[1];
      }
      // Auto-zoom Y to the visible data range so the slope is readable (a fixed
      // 0-100 axis flattens 40-60% lines). Use only the points inside the current
      // [minX,maxX] window so panning/zooming re-fits the vertical scale too.
      const yvis = [];
      active.forEach(it => rows.forEach(r => {
        const xv = Number(r.unix || 0); if (xv < minX || xv > maxX) return;
        const vv = _seriesVal(r, it.key); if (Number.isFinite(vv) && vv > 0) yvis.push(vv);
      }));
      let dLo = yvis.length ? Math.min(...yvis) : 0;
      let dHi = yvis.length ? Math.max(...yvis) : 100;
      const span = Math.max(0.05, dHi - dLo);
      // finer steps + tight padding so even a sub-1% move fills the chart vertically
      const ystep = span > 40 ? 10 : span > 16 ? 5 : span > 6 ? 2 : span > 2 ? 1
                  : span > 0.8 ? 0.5 : span > 0.3 ? 0.2 : span > 0.12 ? 0.1 : span > 0.05 ? 0.05 : 0.02;
      const padY = Math.max(ystep * 0.5, span * 0.08);
      let yLo = Math.max(0, Math.floor((dLo - padY) / ystep) * ystep);
      let yHi = Math.min(100, Math.ceil((dHi + padY) / ystep) * ystep);
      if (yHi - yLo < ystep) { yHi = Math.min(100, yLo + ystep * 2); }
      // Scale over the (possibly windowed) [minX,maxX]; clip drawing to the plot box.
      const x = v => pad.l + (w - pad.l - pad.r) * (v - minX) / Math.max(1, maxX - minX);
      const y = v => h - pad.b - (h - pad.t - pad.b) * (v - yLo) / Math.max(0.001, yHi - yLo);
      // Y gridlines + percent labels at every step (read the % off the axis)
      const ydec = ystep < 0.1 ? 2 : ystep < 1 ? 1 : 0;
      ctx.lineWidth = 1;
      for (let v = yLo; v <= yHi + 0.001; v += ystep) {
        const gy = y(v);
        ctx.strokeStyle = "#2d3a4b";
        ctx.beginPath(); ctx.moveTo(pad.l, gy); ctx.lineTo(w - pad.r, gy); ctx.stroke();
        ctx.fillStyle = "#8da0b8"; ctx.font = "11px Segoe UI, Arial"; ctx.textAlign = "right";
        ctx.fillText(v.toFixed(ydec) + "%", pad.l - 6, gy + 4);
      }
      // series lines + points -- break the line at gaps (NaN), don't plot 0.
      // Clip to the plot box so a zoomed/panned window doesn't draw past the axes.
      ctx.save();
      ctx.beginPath(); ctx.rect(pad.l, pad.t - 4, w - pad.l - pad.r, h - pad.t - pad.b + 8); ctx.clip();
      active.forEach((it, si) => {
        ctx.strokeStyle = it.color; ctx.lineWidth = si === 0 ? 2.5 : 2; ctx.beginPath();
        let pen = false;
        rows.forEach(r => {
          const val = _seriesVal(r, it.key);
          if (!Number.isFinite(val)) { pen = false; return; }
          const px = x(Number(r.unix)), py = y(val);
          if (pen) ctx.lineTo(px, py); else { ctx.moveTo(px, py); pen = true; }
        });
        ctx.stroke(); ctx.fillStyle = it.color;
        rows.forEach(r => {
          const val = _seriesVal(r, it.key);
          if (!Number.isFinite(val)) return;
          ctx.beginPath(); ctx.arc(x(Number(r.unix)), y(val), 2.2, 0, 6.2832); ctx.fill();
        });
      });
      ctx.restore();
      // legend
      let lx = pad.l; ctx.textAlign = "left"; ctx.font = "12px Segoe UI, Arial";
      active.forEach(it => { ctx.fillStyle = it.color; ctx.fillRect(lx, pad.t - 12, 9, 9); ctx.fillStyle = "#cbd5e3"; ctx.fillText(it.label, lx + 13, pad.t - 3); lx += 64; });
      // x-axis: 6-hour-aligned ticks. <=48h span -> elapsed-hours labels; longer
      // (days/months) -> HST date labels at a readable cadence (~8-12 max).
      const fmtH = ux => { const hr = (Number(ux) - minX) / 3600; return hr < 1 ? Math.round(hr * 60) + "m" : (hr < 10 ? hr.toFixed(1) : String(Math.round(hr))) + "h"; };
      const SIXH = 6 * 3600;
      const spanH = (maxX - minX) / 3600;
      ctx.fillStyle = "#7c8aa0"; ctx.font = "11px Segoe UI, Arial"; ctx.textAlign = "center";
      if (spanH <= 48) {
        // Elapsed hours, ticks every 6h from the origin.
        for (let ux = minX; ux <= maxX + 1; ux += SIXH) ctx.fillText(fmtH(ux), x(ux), h - 8);
        ctx.fillStyle = "#6b7686"; ctx.font = "10px Segoe UI, Arial"; ctx.textAlign = "right";
        ctx.fillText("hours since " + fmtTime(rows[0].unix), w - pad.r, pad.t - 3);
      } else {
        // Date labels in HST. Choose a 6h-multiple step so we get <=12 ticks.
        const slots = Math.ceil((maxX - minX) / SIXH);
        const stepSlots = Math.max(1, Math.ceil(slots / 11));
        const step = stepSlots * SIXH;
        // Snap the first tick up to the next 6h UTC boundary so ticks align.
        const first = Math.ceil(minX / SIXH) * SIXH;
        const dateLbl = ux => new Date(Number(ux) * 1000).toLocaleString("en-US", { timeZone: "Pacific/Honolulu", month: "numeric", day: "numeric" });
        for (let ux = first; ux <= maxX + 1; ux += step) ctx.fillText(dateLbl(ux), x(ux), h - 8);
        ctx.fillStyle = "#6b7686"; ctx.font = "10px Segoe UI, Arial"; ctx.textAlign = "right";
        ctx.fillText(dateLbl(minX) + " -> " + dateLbl(maxX) + " HST", w - pad.r, pad.t - 3);
      }
      canvas._chart = { rows, active, x, y, pad, w, h, fmtH, minX, maxX, dataMinX, dataMaxX };
      _bindChartHover(canvas);
    }
    function drawHistory(canvas, history, domain) {
      _drawTimeChart(canvas, history, [
        { key: "decomp_code_pct", label: "code", color: "#5c91df" },
        { key: "decomp_fuzzy_pct", label: "fuzzy", color: "#a98ee6" },
        { key: "decomp_functions_pct", label: "fns", color: "#f0b35a" },
        { key: "c_converted_pct", label: "real C", color: "#38b995" }
      ], "Timeline starts with the next snapshot", domain);
    }
    function relatedAttempts(data, unit) {
      if (!unit) return data.attempt_log || [];
      const sourceFile = fileName(unit.source);
      const shortName = shortSource(unit.source || unit.name);
      const fnSet = new Set((unit.functions || []).map(row => row.name).filter(Boolean));
      return (data.attempt_log || []).filter(row => {
        const rowFile = fileName(row.file || "");
        if (rowFile !== "unknown" && sourceFile !== "unknown" && rowFile === sourceFile) return true;
        if (rowFile !== "unknown" && rowFile.replace(/\.c$/, "") === shortName) return true;
        if (row.function && fnSet.has(row.function)) return true;
        return false;
      });
    }
    // HST formatting (Pacific/Honolulu, UTC-10). Accepts a unix-seconds NUMBER
    // or an ISO-UTC STRING ("2026-06-14T11:00:06Z"). Returns "" for empty input.
    function hstTime(v) {
      let ms = NaN;
      if (typeof v === "number") {
        ms = v * 1000;
      } else if (typeof v === "string" && v) {
        const n = Number(v);
        ms = Number.isFinite(n) && /^\d+$/.test(v.trim()) ? n * 1000 : Date.parse(v);
      }
      if (!Number.isFinite(ms) || !ms) return typeof v === "string" ? v : "";
      return new Date(ms).toLocaleString("en-US", {
        timeZone: "Pacific/Honolulu", month: "numeric", day: "numeric",
        hour: "2-digit", minute: "2-digit", hour12: false
      });
    }
    function fmtTime(unix) {
      const ms = Number(unix || 0) * 1000;
      if (!ms) return "";
      return new Date(ms).toLocaleString("en-US", {
        timeZone: "Pacific/Honolulu", month: "numeric", day: "numeric",
        hour: "2-digit", minute: "2-digit", hour12: false
      });
    }
    // Real time-series over unix time. series = [{key,label,color}], rows from
    // the new /api/history/unit or /api/history/fn endpoints.
    function drawTimeSeries(canvas, rows, series, emptyLabel) {
      _drawTimeChart(canvas, rows, series, emptyLabel || "No history recorded yet");
    }
    function renderMetrics(data) {
      if (!data || !data.metrics) return;   // skip the cold-start stub
      const decomp = data.decomp || {};
      const conv = decomp.conversion || {};
      const openFns = Math.max(0, Number(decomp.total_functions || 0) - Number(decomp.matched_functions || 0));
      $("decomp-metrics").replaceChildren(
        metric("Byte-Exact Code", pctText(decomp.code_pct), `${(decomp.matched_code || 0).toLocaleString()}/${(decomp.total_code || 0).toLocaleString()} bytes`, "#5c91df"),
        metric("C-Converted", pctText(conv.converted_pct), `${(conv.real_c || 0).toLocaleString()}/${(conv.source_total || 0).toLocaleString()} source fns`, "#38b995"),
        metric("Fuzzy Match", pctText(decomp.fuzzy_pct), "weighted instruction similarity", "#a98ee6"),
        metric("Byte-Exact Fns", pctText(decomp.functions_pct), `${decomp.matched_functions || 0}/${decomp.total_functions || 0} functions`, "#f0b35a"),
        metric("Open Fns", openFns.toLocaleString(), `${(conv.asm_wrappers || 0).toLocaleString()} asm wrappers, ${(conv.stubs || 0).toLocaleString()} stubs`, "#e07171"),
        metric("Complete Units", `${decomp.complete_units || 0}/${decomp.total_units || 0}`, "report.json decomp units", "#8da0b8")
      );
      $("symbol-metrics").replaceChildren(
        metric("Completion", `${data.metrics.completion_pct}%`, `${data.metrics.wired_targets}/${data.counts.targets} recorded or renamed`, "#38b995"),
        metric("Needs Wiring", data.counts.by_status["Needs wiring"] || 0, "confirmed leads blocking clean rename", "#f0b35a"),
        metric("Renamed", data.counts.by_status.Renamed || 0, `${data.metrics.rename_pct}% fully wired`, "#5c91df"),
        metric("Recorded", data.counts.by_status.Recorded || 0, "evidence captured", "#8da0b8"),
        metric("Old Fn Refs", data.metrics.old_ref_total, "live references to unresolved fn_ names", "#e07171"),
        metric("Known Bytes", data.metrics.known_size_bytes.toLocaleString(), "tracked symbol sizes", "#a98ee6")
      );
    }
    function renderHud(data) {
      if (!data || !data.metrics) return;   // skip the cold-start stub (no /api/state yet)
      const decomp = data.decomp || {};
      const conv = decomp.conversion || {};
      const openFns = Math.max(0, Number(decomp.total_functions || 0) - Number(decomp.matched_functions || 0));
      setText($("hud-code"), pctText(decomp.code_pct));
      setText($("hud-fuzzy"), pctText(decomp.fuzzy_pct));
      setText($("hud-fns"), pctText(decomp.functions_pct));
      setText($("hud-converted"), pctText(conv.converted_pct));
      setText($("hud-openfns"), openFns.toLocaleString());
      setText($("hud-units"), `${decomp.complete_units || 0}/${decomp.total_units || 0}`);
    }
    // #1/#2: shared drill-in used by the Attack Matrix cards. Mirrors the units
    // table click (switchView -> gotoFiles -> enterUnit -> scroll) so a matrix
    // cell lands in the exact same unit close-out detail the treemap drill uses.
    function drillIntoUnit(unitRef) {
      if (!unitRef || (!unitRef.source && !unitRef.name)) return;
      try { switchView("decomp"); } catch (e) {}
      try { gotoFiles(); } catch (e) {}
      enterUnit(unitRef);
      const ws = document.querySelector(".decomp-workspace");
      if (ws) ws.scrollIntoView({ behavior: "smooth", block: "start" });
    }
    function renderAttackMatrix(data) {
      const matrix = (data && data.attack_matrix) || {};
      const lanes = matrix.lanes || [];
      const laneWrap = $("attack-lanes");
      const pipeWrap = $("pipeline-flow");
      if (!laneWrap || !pipeWrap) return;
      laneWrap.replaceChildren();
      pipeWrap.replaceChildren();
      setText($("attack-note"), matrix.available ? `${(matrix.top_units || []).length} ranked units` : "waiting for report.json");
      if (!lanes.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No attack lanes available");
        laneWrap.append(empty);
      }
      for (const lane of lanes) {
        const card = document.createElement("div");
        card.className = "attack-lane";
        const title = document.createElement("h3");
        setText(title, lane.title || "");
        const count = document.createElement("div");
        count.className = "attack-count";
        const countValue = lane.id === "bulk" ? Number(lane.count || 0).toLocaleString() : lane.count || 0;
        setText(count, countValue);
        const note = document.createElement("div");
        note.className = "attack-note";
        setText(note, lane.note || "");
        const targets = document.createElement("div");
        targets.className = "attack-targets";
        for (const t of (lane.targets || []).slice(0, 5)) {
          const row = document.createElement("div");
          row.className = "attack-target";
          const name = document.createElement("span");
          name.className = "mono";
          setText(name, t.label || t.source || t.name || "");
          const val = document.createElement("span");
          const bits = lane.id === "near"
            ? `${t.near || 0} near`
            : lane.id === "close"
              ? `${t.open_fns || 0} open`
              : `${Number(t.open_code || 0).toLocaleString()}b`;
          setText(val, bits);
          row.append(name, val);
          // #1/#2: drill into this unit's close-out detail on click, reusing the
          // SAME path the treemap/units-table use (switchView+gotoFiles+enterUnit).
          if (t.source || t.name) {
            row.classList.add("attack-target-click");
            row.title = "Open " + (t.source || t.name) + " close-out detail";
            row.addEventListener("click", () => drillIntoUnit({ source: t.source || "", name: t.name || "" }));
          }
          targets.append(row);
        }
        const cmd = document.createElement("div");
        cmd.className = "attack-command";
        setText(cmd, lane.command || "");
        card.append(title, count, note, targets, cmd);
        laneWrap.append(card);
      }
      const colors = ["#38b995", "#5c91df", "#a98ee6", "#f0b35a", "#e07171"];
      for (const [idx, step] of (matrix.pipeline || []).entries()) {
        const card = document.createElement("div");
        card.className = "pipe-step";
        card.style.setProperty("--accent", colors[idx % colors.length]);
        const k = document.createElement("div");
        k.className = "pipe-k";
        setText(k, step.stage || "");
        const v = document.createElement("div");
        v.className = "pipe-v";
        setText(v, Number(step.count || 0).toLocaleString());
        const n = document.createElement("div");
        n.className = "pipe-note";
        setText(n, step.note || "");
        card.append(k, v, n);
        pipeWrap.append(card);
      }
      setText($("pipeline-note"), matrix.generated_at ? `updated ${matrix.generated_at}` : "");
    }
    function renderTop(data) {
      if (!data || !data.metrics) return;   // skip the cold-start stub
      const row = data.next_target;
      setText($("repo"), data.repo);
      setText($("head"), `${data.branch || "detached"} @ ${data.head || "unknown"}`);
      setText($("updated"), `Updated ${data.generated_at}`);
      setText($("next-name"), row ? `${row.fn} -> ${row.name}` : "No pending wiring leads");
      setText($("next-detail"), row ? `${row.source || "unknown source"} | ${row.size || "unknown size"} | ${row.provenance || "no provenance"}` : "All confirmed leads have been wired");
      setText($("progress-label"), `${data.metrics.completion_pct}% complete`);
      $("progress-fill").style.width = `${Math.max(0, Math.min(100, Number(data.metrics.completion_pct || 0)))}%`;
      $("next-status").replaceWith(pill(row ? row.status : "Renamed"));
      const nextStatus = document.querySelector(".target-line .pill");
      nextStatus.id = "next-status";
    }
    function renderLegend(data) {
      if (!data || !data.charts || !data.charts.status) return;   // skip the cold-start stub
      const legend = $("status-legend");
      legend.replaceChildren();
      for (const item of data.charts.status) {
        const entry = document.createElement("span");
        entry.className = "legend-item";
        const swatch = document.createElement("span");
        swatch.className = "swatch";
        swatch.style.setProperty("--swatch", palette[item.label] || "#a98ee6");
        entry.append(swatch, document.createTextNode(`${item.label}: ${item.value}`));
        legend.append(entry);
      }
      setText($("status-total"), `${data.counts.targets} tracked rows`);
      // #3: report commit-milestone count (what the chart plots), not raw samples.
      const commitPts = collapseHistoryToCommits(data.history || []).length;
      setText($("history-points"), `${commitPts} commit milestones`);
      // #2: the timeline-range readout (visible window + pan/zoom hint + reset
      // button) is now owned by updateHistHint() inside drawHistoryRanged(), so
      // renderLegend no longer overwrites it with a static full-range string.
    }
    function renderSourceFilters(rows) {
      const select = $("source-filter");
      const previous = select.value;
      const sources = [...new Set(rows.map(row => row.source || "unknown"))].sort();
      select.replaceChildren(new Option("All sources", ""));
      for (const source of sources) {
        select.append(new Option(source, source));
      }
      if (sources.includes(previous)) {
        select.value = previous;
      }
    }
    function filteredRows() {
      const q = $("query").value.trim().toLowerCase();
      const status = store.attention ? "Needs wiring" : $("status-filter").value;
      const source = $("source-filter").value;
      let rows = store.rows.filter(row => {
        const haystack = `${row.fn} ${row.name} ${row.source} ${row.provenance} ${row.header} ${row.current_symbol}`.toLowerCase();
        if (q && !haystack.includes(q)) return false;
        if (status && row.status !== status) return false;
        if (source && (row.source || "unknown") !== source) return false;
        return true;
      });
      if (store.sortKey) {
        const key = store.sortKey;
        const dir = store.sortDir;
        rows = rows.slice().sort((a, b) => {
          const av = key === "size" || key === "old_refs" ? number(a[key]) : String(a[key] || "").toLowerCase();
          const bv = key === "size" || key === "old_refs" ? number(b[key]) : String(b[key] || "").toLowerCase();
          if (av < bv) return -1 * dir;
          if (av > bv) return 1 * dir;
          return 0;
        });
      }
      return rows;
    }
    function renderRows() {
      const body = $("targets");
      const rows = filteredRows();
      store.filtered = rows;
      setText($("row-count"), `${rows.length}/${store.rows.length} visible`);
      body.replaceChildren();
      if (!rows.length) {
        const tr = document.createElement("tr");
        const cell = document.createElement("td");
        cell.colSpan = 8;
        cell.className = "empty-state";
        setText(cell, "No targets match the current filters");
        tr.append(cell);
        body.append(tr);
        return;
      }
      if (!store.selectedFn || !store.rows.some(row => row.fn === store.selectedFn)) {
        store.selectedFn = rows[0].fn;
      }
      for (const row of rows) {
        const tr = document.createElement("tr");
        tr.tabIndex = 0;
        tr.dataset.fn = row.fn;
        if (row.fn === store.selectedFn) tr.className = "selected";
        const statusCell = document.createElement("td");
        statusCell.append(pill(row.status));
        tr.append(
          statusCell,
          td(row.fn, "mono"),
          td(row.name, "mono"),
          td(row.size, "mono"),
          td(row.source, "wrap"),
          td(row.current_symbol, "mono"),
          td(row.old_refs, row.old_refs !== "0" && row.status !== "Renamed" ? "bad mono" : "mono"),
          td(row.provenance, "wrap")
        );
        tr.addEventListener("click", () => {
          store.selectedFn = row.fn;
          renderRows();
          renderDetails(row);
        });
        tr.addEventListener("keydown", event => {
          if (event.key === "Enter" || event.key === " ") {
            event.preventDefault();
            store.selectedFn = row.fn;
            renderRows();
            renderDetails(row);
          }
        });
        body.append(tr);
      }
      renderDetails(rows.find(row => row.fn === store.selectedFn) || rows[0]);
    }
    function kv(label, value) {
      const row = document.createElement("div");
      row.className = "kv";
      const k = document.createElement("div");
      k.className = "k";
      setText(k, label);
      const v = document.createElement("div");
      v.className = "v";
      if (value instanceof Node) v.append(value);
      else setText(v, value);
      row.append(k, v);
      return row;
    }
    function renderDetails(row) {
      const panel = $("details");
      panel.replaceChildren();
      if (!row) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No selected target");
        panel.append(empty);
        return;
      }
      const title = document.createElement("div");
      title.className = "detail-title mono";
      setText(title, `${row.fn} -> ${row.name}`);
      const subtitle = document.createElement("div");
      subtitle.className = "detail-subtitle";
      setText(subtitle, row.source || "unknown source");
      const status = document.createElement("div");
      status.append(pill(row.status));
      const commands = document.createElement("div");
      commands.className = "command-list";
      commands.append(
        code(`python tools\\match_test.py ${row.fn} --verbose`),
        code(`rg -n "${row.fn}|${row.name}" src include config\\GC6E01`)
      );
      panel.append(
        title,
        subtitle,
        kv("Status", status),
        kv("Address", row.addr),
        kv("Size", row.size || "unknown"),
        kv("Current", row.current_symbol || "not in symbols.txt"),
        kv("Old refs", row.old_refs),
        kv("Evidence", row.provenance),
        kv("Header", row.header),
        kv("Commands", commands)
      );
    }
    function renderSourceBars(data) {
      const bars = $("source-bars");
      bars.replaceChildren();
      const sources = data.charts.sources || [];
      const max = Math.max(...sources.map(row => Number(row.value || 0)), 1);
      for (const row of sources) {
        const line = document.createElement("div");
        line.className = "bar-row";
        const name = document.createElement("div");
        name.className = "bar-name";
        setText(name, row.label);
        const track = document.createElement("div");
        track.className = "bar-track";
        const fill = document.createElement("div");
        fill.className = "bar-fill";
        fill.style.width = `${Number(row.value || 0) * 100 / max}%`;
        track.append(fill);
        const value = document.createElement("div");
        setText(value, row.value);
        line.append(name, track, value);
        bars.append(line);
      }
    }
    function renderActivity(data) {
      const feed = $("activity-feed");
      feed.replaceChildren();
      const history = data.history || [];
      const rows = [];
      const next = data.next_target;
      if (next) {
        rows.push({
          time: data.generated_at,
          tag: "focus",
          text: `${next.fn} -> ${next.name} | ${next.source || "unknown source"}`
        });
      } else {
        rows.push({ time: data.generated_at, tag: "clear", text: "No confirmed-name leads are currently waiting for wiring." });
      }
      for (let idx = history.length - 1; idx >= 0 && rows.length < 8; idx--) {
        const row = history[idx];
        const prev = idx > 0 ? history[idx - 1] : null;
        const delta = prev ? Number(row.completion_pct || 0) - Number(prev.completion_pct || 0) : 0;
        const tag = delta > 0 ? "gain" : "tick";
        rows.push({
          time: Number.isFinite(Number(row.unix)) ? hstTime(Number(row.unix)) : (row.timestamp || ""),
          tag,
          text: `${pctText(row.completion_pct)} complete, ${row.renamed || 0} renamed, ${row.needs_wiring || 0} needs wiring${delta > 0 ? `, +${delta.toFixed(1)}%` : ""}`
        });
      }
      for (const row of rows) {
        const item = document.createElement("div");
        item.className = "feed-row";
        const time = document.createElement("div");
        time.className = "feed-time";
        setText(time, row.time);
        const main = document.createElement("div");
        main.className = "feed-main";
        setText(main, row.text);
        const tag = document.createElement("div");
        tag.className = "feed-tag";
        setText(tag, row.tag);
        item.append(time, main, tag);
        feed.append(item);
      }
      setText($("activity-note"), history.length ? `${history.length} retained snapshots` : "waiting for snapshots");
    }
    function renderCommits(data) {
      const list = $("commit-list");
      list.replaceChildren();
      const commits = data.recent_commits || [];
      if (!commits.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No recent commits available");
        list.append(empty);
      }
      for (const commit of commits) {
        const row = document.createElement("div");
        row.className = "commit-row";
        const sha = document.createElement("div");
        sha.className = "commit-sha";
        setText(sha, commit.sha);
        const when = document.createElement("div");
        when.className = "commit-when";
        setText(when, Number.isFinite(Number(commit.unix)) && Number(commit.unix) > 0 ? hstTime(Number(commit.unix)) : commit.when);
        const subject = document.createElement("div");
        subject.className = "commit-subject";
        setText(subject, commit.subject);
        row.append(sha, when, subject);
        list.append(row);
      }
      setText($("commit-note"), `${commits.length} commits`);
    }
    function filteredTuTiles(data) {
      const q = $("tu-query").value.trim().toLowerCase();
      return (data.tu_tiles || []).filter(row => {
        const haystack = `${row.label} ${row.source}`.toLowerCase();
        if (q && !haystack.includes(q)) return false;
        if (store.tuNeedsOnly && Number(row.needs_wiring || 0) <= 0) return false;
        return true;
      });
    }
    function renderSymbolTreemap(data) {
      const map = $("tu-map");
      map.replaceChildren();
      const tiles = filteredTuTiles(data);
      setText($("tu-note"), `${tiles.length}/${(data.tu_tiles || []).length} source groups`);
      if (!tiles.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No translation units match the current filter");
        map.append(empty);
        return;
      }
      const maxTargets = Math.max(...tiles.map(row => Number(row.targets || 0)), 1);
      for (const row of tiles) {
        const targets = Number(row.targets || 0);
        const pct = Number(row.completion_pct || 0);
        const span = Math.max(1, Math.min(5, Math.ceil(Math.sqrt(targets / maxTargets) * 5)));
        const tile = document.createElement("button");
        tile.type = "button";
        tile.className = "tu-tile";
        tile.style.background = tileColor(pct);
        tile.style.gridColumnEnd = `span ${span}`;
        tile.style.gridRowEnd = `span ${Math.max(1, Math.min(3, Math.ceil(span * .72)))}`;
        tile.title = `${row.source} | ${row.wired}/${row.targets} wired`;
        const name = document.createElement("div");
        name.className = "tu-name";
        setText(name, row.label || shortSource(row.source));
        const pctNode = document.createElement("div");
        pctNode.className = "tu-pct";
        setText(pctNode, pctText(pct));
        const meta = document.createElement("div");
        meta.className = "tu-meta";
        setText(meta, `${row.wired}/${row.targets} wired | ${row.needs_wiring || 0} needs`);
        tile.append(name, pctNode, meta);
        tile.addEventListener("click", () => {
          $("source-filter").value = row.source || "unknown";
          store.attention = false;
          $("attention")?.classList.remove("primary");
          renderRows();
          document.querySelector(".workbench").scrollIntoView({ behavior: "smooth", block: "start" });
        });
        map.append(tile);
      }
    }
    // ---- Squarified treemap (decomp.dev-style, single canvas) ----------------
    function lerp(a, b, t) { return a + (b - a) * t; }
    // Green = matched fraction, blue = unmatched. Continuous interpolation keyed
    // on match%, matching the user's decomp.dev screenshot palette.
    function tmColor(pct) {
      const t = Math.max(0, Math.min(1, Number(pct || 0) / 100));
      // unmatched (blue #3a5fa8) -> matched (green #2f9b48)
      const r = Math.round(lerp(0x3a, 0x2f, t));
      const g = Math.round(lerp(0x5f, 0x9b, t));
      const b = Math.round(lerp(0xa8, 0x48, t));
      return `rgb(${r},${g},${b})`;
    }
    // Squarified treemap layout. items: [{value, ...}] -> sets item._rect.
    function squarify(items, x, y, w, h) {
      items = items.filter(it => Number(it.value || 0) > 0);
      const total = items.reduce((s, it) => s + Number(it.value || 0), 0);
      if (total <= 0 || w <= 0 || h <= 0) return;
      const scale = (w * h) / total;
      const nodes = items.map(it => ({ ref: it, area: Number(it.value || 0) * scale }))
        .sort((a, b) => b.area - a.area);
      let rx = x, ry = y, rw = w, rh = h;
      let idx = 0;
      const worst = (row, side) => {
        const sum = row.reduce((s, n) => s + n.area, 0);
        const maxA = Math.max(...row.map(n => n.area));
        const minA = Math.min(...row.map(n => n.area));
        const s2 = side * side;
        const sum2 = sum * sum;
        return Math.max((s2 * maxA) / sum2, sum2 / (s2 * minA));
      };
      while (idx < nodes.length) {
        const vertical = rw >= rh;
        const side = vertical ? rh : rw;
        const row = [nodes[idx]];
        let j = idx + 1;
        while (j < nodes.length) {
          const test = row.concat([nodes[j]]);
          if (worst(test, side) > worst(row, side)) break;
          row.push(nodes[j]);
          j += 1;
        }
        const rowArea = row.reduce((s, n) => s + n.area, 0);
        const thick = rowArea / side;
        let off = vertical ? ry : rx;
        for (const n of row) {
          const len = n.area / thick;
          if (vertical) {
            n.ref._rect = { x: rx, y: off, w: thick, h: len };
            off += len;
          } else {
            n.ref._rect = { x: off, y: ry, w: len, h: thick };
            off += len;
          }
        }
        if (vertical) { rx += thick; rw -= thick; } else { ry += thick; rh -= thick; }
        idx = j;
      }
    }
    function treemapItems() {
      const tm = store.tm;
      const data = store.data || {};
      const q = $("decomp-query").value.trim().toLowerCase();
      const areaFns = tm.areaByFns;
      if (tm.level === "files") {
        let units = ((data.decomp || {}).units || []).filter(row => {
          const haystack = `${row.name} ${row.source}`.toLowerCase();
          if (q && !haystack.includes(q)) return false;
          if (store.decompNearOnly && !(Number(row.functions_pct || 0) >= 90 && Number(row.functions_pct || 0) < 100)) return false;
          return true;
        });
        return units.map(u => ({
          kind: "unit",
          ref: u,
          label: unitDisplayName(u),
          value: areaFns ? Number(u.total_functions || 0) : Number(u.total_code || 0),
          pct: Number(u.code_pct || 0),
          tip: `<b>${unitDisplayName(u)}</b><br>${pctText(u.code_pct)} code | ${u.matched_functions || 0}/${u.total_functions || 0} fns<br>${Number(u.total_code || 0).toLocaleString()} bytes`
        }));
      }
      // unit level -> functions
      const fns = (tm.unitFns || []).filter(fn => {
        if (q && !String(fn.name || "").toLowerCase().includes(q)) return false;
        if (store.decompNearOnly && !(Number(fn.fuzzy_pct || 0) >= 90 && Number(fn.fuzzy_pct || 0) < 100)) return false;
        return true;
      });
      return fns.map(fn => ({
        kind: "fn",
        ref: fn,
        label: fn.name || "",
        value: Math.max(1, Number(fn.size || 0)),
        pct: Number(fn.fuzzy_pct || 0),
        tip: `<b>${fn.name || ""}</b><br>${pctText(fn.fuzzy_pct)} fuzzy | ${Number(fn.size || 0).toLocaleString()} bytes`
      }));
    }
    // Wrap a tile label (file name / fn_XXXX, usually no spaces) to fit maxW: break
    // after separators (_ . /) first, then hard-break by character so it never clips.
    function wrapLabel(ctx, text, maxW) {
      text = String(text || "");
      if (maxW <= 4 || ctx.measureText(text).width <= maxW) return [text];
      const toks = text.split(/(?<=[_./-])/);
      const lines = []; let cur = "";
      const pushChars = (tok) => {
        let piece = "";
        for (const ch of tok) {
          if (piece && ctx.measureText(piece + ch).width > maxW) { lines.push(piece); piece = ch; }
          else piece += ch;
        }
        return piece;
      };
      for (const tok of toks) {
        if (!cur) {
          cur = (ctx.measureText(tok).width > maxW) ? pushChars(tok) : tok;
        } else if (ctx.measureText(cur + tok).width <= maxW) {
          cur += tok;
        } else {
          lines.push(cur);
          cur = (ctx.measureText(tok).width > maxW) ? pushChars(tok) : tok;
        }
      }
      if (cur) lines.push(cur);
      return lines;
    }
    function renderTreemap() {
      const canvas = $("decomp-treemap");
      const decomp = (store.data || {}).decomp || {};
      const tm = store.tm;
      const items = treemapItems();
      if (tm.level === "files") {
        setText($("decomp-note"), decomp.available ? `${items.length}/${((decomp.units) || []).length} files | ${pctText(decomp.code_pct)} code` : "report.json not available");
      } else {
        const matched = items.filter(it => it.pct >= 99.95).length;
        setText($("decomp-note"), `${items.length} fns | ${matched} at 100%`);
      }
      const { ctx, w, h } = fitCanvas(canvas);
      ctx.clearRect(0, 0, w, h);
      tm.rects = [];
      if (!items.length) {
        return drawEmpty(ctx, w, h, decomp.available ? "No items match the current filter" : "report.json not available");
      }
      squarify(items, 1, 1, w - 2, h - 2);
      ctx.font = "11px Consolas, 'Courier New', monospace";
      ctx.textBaseline = "top";
      // Animation state: glow a tile when its byte-exact fn count rises (new match),
      // and pulse a tile whose file an agent is actively working (from leases).
      store.tmPrev = store.tmPrev || {};
      store.tmGlow = store.tmGlow || {};
      const nowMs = Date.now();
      for (const it of items) {
        const r = it._rect;
        if (!r || r.w < 0.5 || r.h < 0.5) continue;
        tm.rects.push({ x: r.x, y: r.y, w: r.w, h: r.h, item: it });
        // detect fresh progress -> arm a glow. unit: byte-exact fn count rises;
        // fn (drilled-in view): the function newly crosses to 100% byte-exact.
        if (it.ref) {
          const nm = it.ref.name || it.label;
          if (it.kind === "unit") {
            const cur = Number(it.ref.matched_functions || 0);
            const prev = store.tmPrev[nm];
            if (prev !== undefined && cur > prev) store.tmGlow[nm] = nowMs + 2200;
            store.tmPrev[nm] = cur;
          } else {
            const cur = Number(it.pct || 0);
            const prev = store.tmPrev[nm];
            if (prev !== undefined && cur >= 99.95 && prev < 99.95) store.tmGlow[nm] = nowMs + 2600;
            store.tmPrev[nm] = cur;
          }
        }
        ctx.fillStyle = tmColor(it.pct);
        ctx.fillRect(r.x, r.y, r.w, r.h);
        ctx.strokeStyle = "rgba(13,17,24,.75)";
        ctx.lineWidth = 1;
        ctx.strokeRect(r.x + 0.5, r.y + 0.5, Math.max(0, r.w - 1), Math.max(0, r.h - 1));
        if (r.w > 30 && r.h > 13) {
          ctx.save();
          ctx.beginPath();
          ctx.rect(r.x + 4, r.y + 1, r.w - 7, r.h - 2);
          ctx.clip();
          // Light label on the bluer (low-pct) end, dark on the greener end.
          ctx.fillStyle = it.pct >= 55 ? "rgba(8,14,11,.92)" : "rgba(238,244,251,.96)";
          const lineH = 12, maxW = r.w - 11;
          const lines = wrapLabel(ctx, it.label, maxW);     // wrap, don't clip
          const showPct = r.h > 26;
          const avail = Math.max(1, Math.floor((r.h - 6) / lineH) - (showPct ? 1 : 0));
          let ty = r.y + 4;
          for (let i = 0; i < Math.min(lines.length, avail); i++) { ctx.fillText(lines[i], r.x + 6, ty); ty += lineH; }
          if (showPct) ctx.fillText(pctText(it.pct), r.x + 6, ty);
          ctx.restore();
        }
        // --- match glow (green, fading) ---
        const nm = it.ref && (it.ref.name || it.label);
        const gExp = nm && store.tmGlow[nm];
        if (gExp && nowMs < gExp) {
          const a = (gExp - nowMs) / 2200;            // 1 -> 0
          ctx.save();
          ctx.strokeStyle = `rgba(110,231,168,${0.95 * a})`;
          ctx.lineWidth = 3;
          ctx.shadowColor = "rgba(110,231,168,0.95)";
          ctx.shadowBlur = 18 * a;
          ctx.strokeRect(r.x + 1.5, r.y + 1.5, Math.max(0, r.w - 3), Math.max(0, r.h - 3));
          ctx.restore();
        }
        // resolve agent / permuter for this tile (unit -> by file, fn -> by fn name)
        let agent, pinfo;
        if (it.kind === "unit" && it.ref) {
          const src = String(it.ref.source || "").replace(/\\/g, "/");
          agent = store.tmAgents && store.tmAgents[src];
          pinfo = store.tmPermuter && store.tmPermuter[src];
        } else if (it.kind === "fn" && it.ref) {
          agent = store.tmAgentFns && store.tmAgentFns[it.ref.name];
          const pf = store.tmPermuterFns && store.tmPermuterFns[it.ref.name];
          if (pf) pinfo = { fn: it.ref.name, iter: pf.iter, best: pf.best };
        }
        // --- active-agent pulse (blue, oscillating) + agent badge ---
        if (agent) {
          const p = 0.5 + 0.5 * Math.sin(nowMs / 280);   // 0..1 pulse
          ctx.save();
          ctx.strokeStyle = `rgba(127,209,255,${0.40 + 0.5 * p})`;
          ctx.lineWidth = 2.5;
          ctx.strokeRect(r.x + 1.5, r.y + 1.5, Math.max(0, r.w - 3), Math.max(0, r.h - 3));
          if (r.w > 44 && r.h > 14) {
            ctx.fillStyle = `rgba(127,209,255,${0.7 + 0.3 * p})`;
            ctx.font = "bold 10px Consolas, monospace";
            ctx.textAlign = "right";
            ctx.fillText("● " + agent, r.x + r.w - 6, r.y + r.h - 10);
            ctx.textAlign = "left";
          }
          ctx.restore();
        }
        // --- permuter annealing (amber marching-ants, distinct from the agent pulse) ---
        if (pinfo) {
          ctx.save();
          ctx.strokeStyle = "rgba(255,176,72,0.95)";
          ctx.lineWidth = 2.5;
          ctx.setLineDash([6, 4]);
          ctx.lineDashOffset = -((nowMs / 40) % 10);   // marching ants = annealing
          ctx.strokeRect(r.x + 1.5, r.y + 1.5, Math.max(0, r.w - 3), Math.max(0, r.h - 3));
          ctx.setLineDash([]);
          if (r.w > 52 && r.h > 28) {
            ctx.fillStyle = "rgba(255,176,72,0.96)";
            ctx.font = "bold 10px Consolas, monospace";
            ctx.textAlign = "left";
            ctx.fillText("⚛ " + (pinfo.fn || "anneal") + " best " + (pinfo.best == null ? "?" : pinfo.best), r.x + 7, r.y + r.h - 10);
          }
          ctx.restore();
        }
      }
      // keep animating while a glow is fading, an agent is working, or the permuter
      // is annealing a tile (throttled ~11fps).
      const glowing = Object.values(store.tmGlow).some(t => t > nowMs);
      const working = store.tmAgents && Object.keys(store.tmAgents).length > 0;
      const annealing = store.tmPermuter && Object.keys(store.tmPermuter).length > 0;
      if ((glowing || working || annealing) && !store._tmTimer) {
        store._tmTimer = setTimeout(() => { store._tmTimer = 0; renderTreemap(); }, 90);
      }
    }
    function treemapHit(evt) {
      const canvas = $("decomp-treemap");
      const rect = canvas.getBoundingClientRect();
      const px = (evt.clientX - rect.left);
      const py = (evt.clientY - rect.top);
      for (const r of store.tm.rects) {
        if (px >= r.x && px <= r.x + r.w && py >= r.y && py <= r.y + r.h) return r;
      }
      return null;
    }
    function onTreemapMove(evt) {
      const tip = $("decomp-tip");
      const hit = treemapHit(evt);
      if (!hit) { tip.hidden = true; return; }
      tip.hidden = false;
      tip.innerHTML = hit.item.tip;
      const shell = $("decomp-treemap").parentElement.getBoundingClientRect();
      // Measure the tip now that its content is set, then flip it above/left of
      // the cursor when it would overflow the (overflow:hidden) shell — without
      // this, tooltips on bottom/right tiles render off-canvas and stay invisible.
      const tw = tip.offsetWidth || 280;
      const th = tip.offsetHeight || 60;
      let left = evt.clientX - shell.left + 14;
      let top = evt.clientY - shell.top + 14;
      if (left + tw > shell.width) left = evt.clientX - shell.left - tw - 14;
      if (top + th > shell.height) top = evt.clientY - shell.top - th - 14;
      tip.style.left = `${Math.max(0, Math.min(left, shell.width - tw))}px`;
      tip.style.top = `${Math.max(0, Math.min(top, shell.height - th))}px`;
    }
    function drillItem(it) {
      if (it.kind === "unit") enterUnit(it.ref);
      else if (it.kind === "fn") enterFn(it.ref);
    }
    function onTreemapClick(evt) {
      const hit = treemapHit(evt);
      if (store.isTouch) {
        // Two-step: first tap pins the detail, second tap on the SAME tile drills.
        if (!hit) { closePin(); return; }
        const it = hit.item;
        const sameAsPinned = store.pinnedItem
          && store.pinnedItem.kind === it.kind
          && (it.ref && store.pinnedItem.ref && (it.ref.name === store.pinnedItem.ref.name && it.ref.source === store.pinnedItem.ref.source));
        if (sameAsPinned) {
          closePin();
          drillItem(it);
        } else {
          openPin(it);
        }
        return;
      }
      // Desktop: unchanged single-click drill.
      if (!hit) return;
      drillItem(hit.item);
    }
    // ---- v10: pinned (tap-to-show) treemap detail for touch -----------------
    function openPin(it) {
      store.pinnedItem = it;
      const pin = $("treemap-pin");
      $("treemap-pin-body").innerHTML = it.tip || "";
      const openBtn = $("treemap-pin-open");
      setText(openBtn, it.kind === "unit" ? "Open file ▸" : "Open function ▸");
      pin.classList.add("active");
    }
    function closePin() {
      store.pinnedItem = null;
      $("treemap-pin").classList.remove("active");
    }
    function enterUnit(unit) {
      const tm = store.tm;
      tm.level = "unit";
      tm.unitSource = unit.source || "";
      tm.unitName = unitDisplayName(unit);
      tm.selectedFn = "";
      tm.unitFns = null;
      const wantSource = unit.source || unit.name || "";
      const url = `/api/unit?source=${encodeURIComponent(wantSource)}`;
      fetch(url, { cache: "no-store" }).then(r => r.json()).then(payload => {
        if (store.tm.level !== "unit" || store.tm.unitSource !== (unit.source || "")) return;
        store.tm.unitFns = (payload.functions || []);
        store.tm.unitFnsSource = wantSource;
        renderTreemap();
        renderDecompDetail(store.data);
      }).catch(() => {
        store.tm.unitFns = (unit.functions || []);
        renderTreemap();
        renderDecompDetail(store.data);
      });
      renderCrumbs();
      renderDecompDetail(store.data);
      renderTreemap();
    }
    function enterFn(fn) {
      store.tm.selectedFn = fn.name || "";
      renderCrumbs();
      renderDecompDetail(store.data);
      openReader(fn.name || "");
    }
    // ---- v9: decomp.me-style function reader --------------------------------
    function closeReader() {
      store.reader.open = false;
      $("reader-overlay").classList.remove("active");
    }
    function openReader(fnName) {
      if (!fnName) return;
      const source = store.tm.unitFnsSource || store.tm.unitSource || store.tm.unitName || "";
      store.reader = { open: true, fn: fnName, source };
      const overlay = $("reader-overlay");
      overlay.classList.add("active");
      overlay.scrollIntoView({ behavior: "smooth", block: "start" });
      setText($("reader-fn"), fnName);
      setText($("reader-pct"), "...");
      $("reader-pct").className = "reader-pct";
      setText($("reader-meta"), "compiling + diffing (first load ~5-8s)...");
      const body = $("asm-body");
      body.replaceChildren();
      const wait = document.createElement("div");
      wait.className = "empty-state";
      setText(wait, "Compiling " + shortSource(source) + ".c and running objdiff...");
      body.append(wait);
      $("reader-side").replaceChildren();
      // Fire wall-info and asm-diff in parallel.
      loadFnInfo(fnName);
      const url = `/api/asm?source=${encodeURIComponent(source)}&fn=${encodeURIComponent(fnName)}`;
      fetch(url, { cache: "no-store" }).then(r => r.json()).then(payload => {
        if (!store.reader.open || store.reader.fn !== fnName) return;
        renderReaderAsm(payload);
      }).catch(() => {
        if (!store.reader.open || store.reader.fn !== fnName) return;
        renderReaderAsm({ available: false, error: "request failed" });
      });
    }
    function renderReaderAsm(payload) {
      const body = $("asm-body");
      body.replaceChildren();
      if (!payload || !payload.available) {
        const err = document.createElement("div");
        err.className = "empty-state";
        setText(err, (payload && payload.error) ? `Reader unavailable: ${payload.error}` : "No diff data available.");
        body.append(err);
        setText($("reader-pct"), "--%");
        $("reader-pct").className = "reader-pct bad";
        setText($("reader-meta"), payload && payload.source ? payload.source : "");
        return;
      }
      const pct = Number(payload.fuzzy_pct || 0);
      setText($("reader-pct"), pct.toFixed(2) + "%");
      $("reader-pct").className = "reader-pct " + (pct >= 99.95 ? "good" : pct >= 90 ? "near" : "bad");
      setText($("reader-meta"), `${payload.source} | ${payload.target_obj || ""} | ${payload.matched || 0}/${payload.total || 0} instr match`);
      const rows = payload.rows || [];
      if (!rows.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "objdiff returned no instructions for this symbol.");
        body.append(empty);
        return;
      }
      const frag = document.createDocumentFragment();
      rows.forEach((row, idx) => {
        const line = document.createElement("div");
        line.className = `asm-line ${row.state || "same"}`;
        const num = document.createElement("div");
        num.className = "asm-num";
        num.textContent = String(idx + 1);
        const target = document.createElement("div");
        target.className = "asm-cell target";
        target.textContent = row.l || "";
        const current = document.createElement("div");
        current.className = "asm-cell current";
        current.textContent = row.r || "";
        line.append(num, target, current);
        frag.append(line);
      });
      body.append(frag);
    }
    function difficultyChip(diff) {
      const chip = document.createElement("span");
      const key = diff && diff.section_key ? String(diff.section_key) : "";
      chip.className = `difficulty-chip ${key || "none"}`;
      if (!diff) {
        setText(chip, "-");
        return chip;
      }
      const rank = diff.rank ? `#${diff.rank}` : key;
      setText(chip, `${key || "hard"} ${rank}`);
      chip.title = `${diff.section || "hard target"}: ${diff.note || ""}`;
      return chip;
    }
    function functionMatchesDifficulty(fn) {
      const f = store.tm.difficultyFilter || "all";
      const diff = fn && fn.difficulty;
      if (f === "all") return true;
      if (f === "hard") return !!diff;
      return !!diff && diff.section_key === f;
    }
    function selectedCrackPayload(launch) {
      return {
        fn: store.reader.fn,
        source: store.reader.source,
        lane: store.crack.lane,
        strategy: store.crack.strategy,
        launch: !!launch
      };
    }
    function setCrackStatus(text, good) {
      const el = $("crack-status");
      if (!el) return;
      setText(el, text || "");
      el.style.color = good ? "var(--accent)" : "";
    }
    function crackAction(launch) {
      const label = launch ? "Launching in Codex tmux..." : "Queued.";
      setCrackStatus(label, false);
      return fetch("/api/crack/enqueue", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(selectedCrackPayload(launch)),
        cache: "no-store"
      }).then(r => r.json()).then(payload => {
        if (!payload || !payload.ok) {
          setCrackStatus((payload && payload.error) || "crack request failed", false);
          return;
        }
        const job = payload.job || {};
        const launchInfo = job.launch || {};
        if (launch && launchInfo.ok === false) {
          setCrackStatus(`Queued; launch not available: ${launchInfo.error || "adapter error"}`, false);
        } else {
          setCrackStatus(`${job.status || "queued"} ${job.id || ""}`, true);
        }
        pollCrackJobs();
        loadFnInfo(store.reader.fn);
      }).catch(() => setCrackStatus("crack request failed", false));
    }
    function renderTerminalCapture(payload) {
      const pre = $("terminal-capture");
      if (!pre) return;
      if (!payload || !payload.available) {
        setText(pre, (payload && payload.error) || "terminal capture unavailable");
        return;
      }
      setText(pre, payload.text || "(no terminal output)");
    }
    function pollTerminal() {
      fetch(`/api/crack/terminal?pane=${encodeURIComponent(store.crack.terminalPane || "codex")}&lines=120`, { cache: "no-store" })
        .then(r => r.json()).then(renderTerminalCapture).catch(() => renderTerminalCapture({ available: false, error: "terminal endpoint failed" }));
    }
    function pollCrackJobs() {
      fetch("/api/crack/jobs?limit=80", { cache: "no-store" })
        .then(r => r.json()).then(payload => {
          store.crack.jobs = (payload && payload.jobs) || [];
          if (store.reader.open) {
            const info = { fn: store.reader.fn, crack_jobs: store.crack.jobs.filter(j => j.fn === store.reader.fn), crack_job_count: store.crack.jobs.filter(j => j.fn === store.reader.fn).length };
            const list = $("crack-job-list");
            if (list) renderCrackJobList(list, info.crack_jobs);
          }
        }).catch(() => {});
    }
    function renderCrackJobList(list, jobs) {
      list.replaceChildren();
      if (!jobs || !jobs.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No crack jobs for this function yet");
        list.append(empty);
        return;
      }
      for (const job of jobs.slice(0, 8)) {
        const row = document.createElement("div");
        row.className = "crack-job";
        const state = document.createElement("span");
        setText(state, job.status || "queued");
        const desc = document.createElement("div");
        setText(desc, `${job.lane_label || job.lane} / ${job.strategy_label || job.strategy}`);
        row.append(state, desc);
        list.append(row);
      }
    }
    function renderCrackLab(info, side) {
      const card = document.createElement("div");
      card.className = "wall-card crack-card";
      const title = document.createElement("div");
      title.className = "crack-title";
      const b = document.createElement("b");
      setText(b, "Crack Lab");
      const sub = document.createElement("span");
      setText(sub, "queue / tmux");
      title.append(b, sub);
      card.append(title);
      const diff = info && info.difficulty;
      if (diff) {
        const note = document.createElement("div");
        note.className = "wall-note";
        setText(note, `${diff.section || "Hard target"} ${diff.rank ? "#" + diff.rank : ""}: ${diff.note || ""}`);
        card.append(note);
      }
      const laneField = document.createElement("div");
      laneField.className = "crack-field";
      const laneLabel = document.createElement("label");
      setText(laneLabel, "Model lane");
      const laneGrid = document.createElement("div");
      laneGrid.className = "crack-grid";
      for (const lane of [
        ["codex-gpt55-high", "Codex 5.5 high"],
        ["claude-opus", "Claude Opus"],
        ["glm-52", "GLM 5.2"],
        ["opencode-deepseek-v4-flash", "DeepSeek V4 Flash"]
      ]) {
        const btn = document.createElement("button");
        btn.type = "button";
        btn.className = "crack-choice" + (store.crack.lane === lane[0] ? " active" : "");
        btn.dataset.lane = lane[0];
        setText(btn, lane[1]);
        btn.onclick = () => {
          store.crack.lane = lane[0];
          renderFnInfo(info);
        };
        laneGrid.append(btn);
      }
      laneField.append(laneLabel, laneGrid);
      card.append(laneField);
      const stratField = document.createElement("div");
      stratField.className = "crack-field";
      const stratLabel = document.createElement("label");
      setText(stratLabel, "Strategy");
      const stratGrid = document.createElement("div");
      stratGrid.className = "crack-grid";
      for (const strat of [
        ["contenders", "Contenders"],
        ["repair", "Repair best"],
        ["split", "Split giant"]
      ]) {
        const btn = document.createElement("button");
        btn.type = "button";
        btn.className = "crack-choice" + (store.crack.strategy === strat[0] ? " active" : "");
        btn.dataset.strategy = strat[0];
        setText(btn, strat[1]);
        btn.onclick = () => {
          store.crack.strategy = strat[0];
          renderFnInfo(info);
        };
        stratGrid.append(btn);
      }
      stratField.append(stratLabel, stratGrid);
      card.append(stratField);
      const canTmux = store.crack.lane === "codex-gpt55-high";
      const actions = document.createElement("div");
      actions.className = "crack-actions";
      const queue = document.createElement("button");
      queue.className = "btn ghost";
      queue.type = "button";
      setText(queue, "Queue");
      queue.onclick = () => crackAction(false);
      const launch = document.createElement("button");
      launch.className = "btn primary";
      launch.type = "button";
      setText(launch, canTmux ? "Send to tmux" : "Queue lane");
      launch.title = canTmux ? "Queue and send to the Codex tmux pane" : "This lane is queued; its direct launcher is not wired yet";
      launch.onclick = () => crackAction(canTmux);
      actions.append(queue, launch);
      card.append(actions);
      const status = document.createElement("div");
      status.id = "crack-status";
      status.className = "crack-status";
      setText(status, canTmux
        ? "Independent contenders are the default; split only for structural giants."
        : "This lane queues the job for now; direct launch adapter is not wired yet.");
      card.append(status);
      const jobs = document.createElement("div");
      jobs.id = "crack-job-list";
      jobs.className = "crack-job-list";
      renderCrackJobList(jobs, (info && info.crack_jobs) || []);
      card.append(jobs);
      side.append(card);

      const term = document.createElement("div");
      term.className = "wall-card terminal-card";
      const termTitle = document.createElement("div");
      termTitle.className = "crack-title";
      const tb = document.createElement("b");
      setText(tb, "Codex terminal");
      const refresh = document.createElement("button");
      refresh.type = "button";
      refresh.className = "btn ghost xs";
      setText(refresh, "Refresh");
      refresh.onclick = pollTerminal;
      termTitle.append(tb, refresh);
      const open = document.createElement("button");
      open.type = "button";
      open.className = "btn ghost block sm";
      setText(open, "Open tmux window");
      open.disabled = !canTmux;
      open.title = canTmux ? "Open/attach the Codex tmux pane" : "Only the Codex lane has a tmux pane adapter right now";
      open.onclick = () => postJSON("/api/crack/open-terminal", { pane: "codex" }).then(r => {
        setCrackStatus(r && r.ok ? "Opened tmux window" : ((r && r.error) || "Could not open tmux window"), !!(r && r.ok));
      }).catch(() => setCrackStatus("Could not open tmux window", false));
      const pre = document.createElement("pre");
      pre.id = "terminal-capture";
      setText(pre, canTmux ? "Terminal capture not loaded" : "Terminal is Codex-only for now; this lane will stay queued.");
      term.append(termTitle, open, pre);
      side.append(term);
      if (canTmux) pollTerminal();
    }
    function loadFnInfo(fnName) {
      fetch(`/api/fninfo?fn=${encodeURIComponent(fnName)}`, { cache: "no-store" })
        .then(r => r.json())
        .then(info => {
          if (!store.reader.open || store.reader.fn !== fnName) return;
          renderFnInfo(info);
        })
        .catch(() => {});
    }
    function renderFnInfo(info) {
      const side = $("reader-side");
      side.replaceChildren();
      const card = document.createElement("div");
      card.className = "wall-card";
      const banner = document.createElement("div");
      const hasWall = info.wall_class || info.in_cs_walls;
      if (info.is_equivalent) {
        banner.className = "wall-banner equiv";
        setText(banner, "Logged Equivalent");
      } else if (hasWall) {
        banner.className = "wall-banner wall";
        setText(banner, `Known wall${info.wall_class ? ": " + info.wall_class : ""}`);
      } else {
        banner.className = "wall-banner clear";
        setText(banner, "No wall logged");
      }
      card.append(banner);
      if (info.wall_class || info.in_cs_walls) {
        const tags = document.createElement("div");
        tags.className = "reader-meta";
        const bits = [];
        if (info.wall_class) bits.push(info.wall_class);
        if (info.in_cs_walls) bits.push("cs_walls.json");
        if (info.is_equivalent) bits.push("equivalent.txt");
        setText(tags, bits.join(" | "));
        tags.style.marginBottom = "8px";
        card.append(tags);
      }
      if (info.note) {
        const note = document.createElement("div");
        note.className = "wall-note";
        setText(note, info.note);
        card.append(note);
      } else if (!hasWall && !info.is_equivalent) {
        const note = document.createElement("div");
        note.className = "wall-note";
        setText(note, "Not in WALLS.md, equivalent.txt, or cs_walls.json. If this fn is below 100%, it is an open target.");
        card.append(note);
      }
      side.append(card);
      const kg = (info && info.kg) || {};
      if (kg.available) {
        const kgCard = document.createElement("div");
        kgCard.className = "wall-card";
        const kgTitle = document.createElement("div");
        kgTitle.className = "limit-label";
        setText(kgTitle, "KG relationships");
        kgCard.append(kgTitle);
        const kgList = document.createElement("div");
        kgList.className = "kg-list";
        kgList.style.maxHeight = "240px";
        kgList.style.marginTop = "8px";
        const addKgRow = (left, right) => {
          const row = document.createElement("div");
          row.className = "kg-row";
          const a = document.createElement("span");
          a.className = "mono";
          setText(a, left);
          const b = document.createElement("span");
          setText(b, right || "");
          row.append(a, b);
          kgList.append(row);
        };
        for (const row of (kg.tags || []).slice(0, 5)) {
          addKgRow(`#${row.tag || ""}`, `${row.confidence || ""} ${row.score || ""}`);
        }
        for (const row of (kg.name_evidence || []).slice(0, 4)) {
          addKgRow(`name ${row.candidate || ""}`, `${row.source || ""} ${row.score || ""}`);
        }
        for (const row of (kg.callees || []).slice(0, 5)) {
          addKgRow(`calls ${row.fn || ""}`, row.confidence || "");
        }
        for (const row of (kg.callers || []).slice(0, 5)) {
          addKgRow(`from ${row.fn || ""}`, row.confidence || "");
        }
        if (!kgList.children.length) {
          const empty = document.createElement("div");
          empty.className = "empty-state";
          setText(empty, "No KG relationships mined for this function yet");
          kgList.append(empty);
        }
        kgCard.append(kgList);
        side.append(kgCard);
      }
      renderCrackLab(info, side);
      // history chart for the fn (reuse existing per-fn endpoint via canvas)
      const histCard = document.createElement("div");
      histCard.className = "wall-card";
      const histTitle = document.createElement("div");
      histTitle.className = "limit-label";
      setText(histTitle, `${info.attempt_count || 0} logged attempts`);
      histCard.append(histTitle);
      if ((info.attempts || []).length) {
        const list = document.createElement("div");
        list.className = "function-list";
        list.style.maxHeight = "180px";
        list.style.marginTop = "8px";
        for (const a of info.attempts.slice().reverse().slice(0, 12)) {
          const row = document.createElement("div");
          row.className = "attempt-row";
          row.style.gridTemplateColumns = "120px 1fr";
          const t = document.createElement("div");
          t.className = "attempt-time";
          setText(t, hstTime(a.timestamp));
          const m = document.createElement("div");
          m.className = "attempt-message";
          setText(m, a.message || a.kind || "");
          row.append(t, m);
          list.append(row);
        }
        histCard.append(list);
      }
      side.append(histCard);
    }
    function gotoFiles() {
      const tm = store.tm;
      tm.level = "files";
      tm.unitSource = "";
      tm.unitName = "";
      tm.unitFns = null;
      tm.selectedFn = "";
      renderCrumbs();
      renderTreemap();
      renderDecompDetail(store.data);
    }
    function gotoUnit() {
      store.tm.selectedFn = "";
      renderCrumbs();
      renderTreemap();
      renderDecompDetail(store.data);
    }
    function renderCrumbs() {
      const nav = $("decomp-crumbs");
      nav.replaceChildren();
      const tm = store.tm;
      const mk = (label, handler, active) => {
        const b = document.createElement("button");
        b.type = "button";
        setText(b, label);
        if (active) b.disabled = true;
        else b.addEventListener("click", handler);
        return b;
      };
      const sep = () => {
        const s = document.createElement("span");
        s.className = "sep";
        s.textContent = "›";
        return s;
      };
      nav.append(mk("All files", gotoFiles, tm.level === "files"));
      if (tm.level !== "files") {
        nav.append(sep(), mk(tm.unitName || "unit", gotoUnit, !tm.selectedFn));
      }
      if (tm.selectedFn) {
        nav.append(sep(), mk(tm.selectedFn, () => {}, true));
      }
    }
    function renderDecompDetail(data) {
      const panel = $("decomp-details");
      panel.replaceChildren();
      const decomp = (data || {}).decomp || {};
      const tm = store.tm;
      if (!decomp.available) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No report.json decomp metrics found");
        panel.append(empty);
        renderDecompAttemptLog(data, null);
        drawTimeSeries($("file-history-chart"), [], [], "No report.json decomp metrics");
        setText($("file-history-note"), "no report.json");
        return;
      }
      if (tm.level === "files") {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "Click a file in the treemap to drill into its functions.");
        panel.append(empty);
        // #3: at the files level, surface the files agents are ACTIVELY working
        // (locks + fresh scratch + recent commits) with their live match%, so the
        // detail panel isn't blank and the user can jump straight to live work.
        const activeHost = document.createElement("div");
        activeHost.id = "decomp-active-files";
        activeHost.className = "decomp-active-files";
        panel.append(activeHost);
        renderActiveFilesInDetail(store._activeWork);
        renderDecompAttemptLog(data, null);
        drawTimeSeries($("file-history-chart"), [], [], "Select a file for its progress history");
        setText($("file-history-note"), "no file selected");
        return;
      }
      const units = decomp.units || [];
      const unit = units.find(u => (u.source || "") === tm.unitSource) || units.find(u => unitDisplayName(u) === tm.unitName);
      if (!unit) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "Unit not found in current report.");
        panel.append(empty);
        return;
      }
      const title = document.createElement("div");
      title.className = "decomp-title mono";
      setText(title, unitDisplayName(unit));
      const subtitle = document.createElement("div");
      subtitle.className = "decomp-subtitle";
      setText(subtitle, unit.source || unit.name || "unknown source");
      const stats = document.createElement("div");
      stats.className = "mini-stats";
      const matched = (unit.function_status || {}).matched || 0;
      const near = (unit.function_status || {}).near || 0;
      const partial = (unit.function_status || {}).partial || 0;
      const missing = (unit.function_status || {}).missing || 0;
      stats.append(
        miniStat("Functions", `${unit.matched_functions || 0}/${unit.total_functions || 0}`),
        miniStat("Fuzzy", pctText(unit.fuzzy_pct)),
        miniStat("Code", pctText(unit.code_pct)),
        miniStat("Bytes", Number(unit.total_code || 0).toLocaleString())
      );
      const mix = document.createElement("div");
      mix.className = "legend";
      for (const [label, value, color] of [
        ["100", matched, "#38b995"],
        ["near", near, "#f0b35a"],
        ["partial", partial, "#5c91df"],
        ["missing", missing, "#e07171"]
      ]) {
        const entry = document.createElement("span");
        entry.className = "legend-item";
        const swatch = document.createElement("span");
        swatch.className = "swatch";
        swatch.style.setProperty("--swatch", color);
        entry.append(swatch, document.createTextNode(`${label}: ${value}`));
        mix.append(entry);
      }
      const listTitle = document.createElement("div");
      listTitle.className = "panel-title";
      const h = document.createElement("h2");
      setText(h, "Functions");
      const note = document.createElement("span");
      note.className = "panel-note";
      const fnSource = tm.unitFns || (unit.functions || []);
      const diffFilter = store.tm.difficultyFilter || "all";
      setText(note, tm.unitFns ? `${fnSource.length} rows (lazy /api/unit)` : `${fnSource.length} rows`);
      listTitle.append(h, note);
      const list = document.createElement("div");
      list.className = "function-list";
      const functions = fnSource.slice().filter(functionMatchesDifficulty).sort((a, b) => {
        const ad = a.difficulty || {};
        const bd = b.difficulty || {};
        const ar = Number(ad.rank || 9999);
        const br = Number(bd.rank || 9999);
        if (diffFilter !== "all" && diffFilter !== "hard" && ar !== br) return ar - br;
        const ap = Number(a.fuzzy_pct || 0);
        const bp = Number(b.fuzzy_pct || 0);
        if (ap !== bp) return ap - bp;
        return Number(b.size || 0) - Number(a.size || 0);
      });
      if (diffFilter !== "all") {
        setText(note, `${functions.length}/${fnSource.length} ${diffFilter}`);
      }
      if (!functions.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, diffFilter !== "all" ? "No functions match the difficulty filter for this file" : (tm.unitFns ? "No function rows recorded for this file" : "Loading functions..."));
        list.append(empty);
      }
      for (const fn of functions) {
        const row = document.createElement("div");
        row.className = "function-row";
        if (fn.name === tm.selectedFn) row.style.outline = "1px solid #547298";
        const name = document.createElement("div");
        name.className = "function-name";
        setText(name, fn.name || "unknown");
        const pctNode = document.createElement("div");
        pctNode.className = "function-pct";
        setText(pctNode, pctText(fn.fuzzy_pct));
        const size = document.createElement("div");
        size.className = "function-size";
        setText(size, `${Number(fn.size || 0).toLocaleString()}b`);
        row.append(name, pctNode, size, statusChip(fn.status), difficultyChip(fn.difficulty));
        row.style.cursor = "pointer";
        row.addEventListener("click", () => enterFn(fn));
        list.append(row);
      }
      panel.append(title, subtitle, stats, mix, listTitle, list);
      renderDecompAttemptLog(data, unit);
      // Real time series. If a fn is drilled, prefer /api/history/fn.
      if (tm.selectedFn) {
        loadFnHistory(tm.selectedFn);
      } else {
        loadUnitHistory(unit.source || unit.name || "");
      }
    }
    function loadUnitHistory(source) {
      if (!source) {
        drawTimeSeries($("file-history-chart"), [], [], "No history yet for this file");
        setText($("file-history-note"), "no source path");
        return;
      }
      fetch(`/api/history/unit?source=${encodeURIComponent(source)}`, { cache: "no-store" })
        .then(r => r.json())
        .then(rows => {
          if (store.tm.level !== "unit" || store.tm.selectedFn) return;
          const series = [
            { key: "fp", label: "fns %", color: "#f0b35a" },
            { key: "cp", label: "code %", color: "#5c91df" }
          ];
          drawTimeSeries($("file-history-chart"), rows, series, "No history recorded for this file yet");
          setText($("file-history-note"), rows.length ? `${rows.length} recorded changes` : "no changes recorded yet");
        })
        .catch(() => {
          drawTimeSeries($("file-history-chart"), [], [], "History unavailable");
          setText($("file-history-note"), "history unavailable");
        });
    }
    function loadFnHistory(name) {
      fetch(`/api/history/fn?name=${encodeURIComponent(name)}`, { cache: "no-store" })
        .then(r => r.json())
        .then(rows => {
          if (store.tm.selectedFn !== name) return;
          const series = [{ key: "fuzzy_pct", label: "fuzzy %", color: "#a98ee6" }];
          drawTimeSeries($("file-history-chart"), rows, series, "No fuzzy-match history for this fn yet");
          setText($("file-history-note"), rows.length ? `${rows.length} recorded changes for ${name}` : `no history yet for ${name}`);
        })
        .catch(() => {
          drawTimeSeries($("file-history-chart"), [], [], "History unavailable");
          setText($("file-history-note"), "history unavailable");
        });
    }
    function miniStat(label, value) {
      const box = document.createElement("div");
      box.className = "mini-stat";
      const k = document.createElement("div");
      k.className = "mini-label";
      setText(k, label);
      const v = document.createElement("div");
      v.className = "mini-value";
      setText(v, value);
      box.append(k, v);
      return box;
    }
    function renderDecompAttemptLog(data, unit) {
      const activeData = data || store._logData || { attempt_log: [] };
      const activeUnit = unit || null;
      store._logData = activeData;
      store._logUnit = activeUnit;
      const list = $("decomp-log");
      list.replaceChildren();
      const attemptTime = row => {
        const u = Number(row && row.unix);
        if (Number.isFinite(u) && u > 0) return u;
        const p = Date.parse((row && row.timestamp) || "");
        return Number.isFinite(p) ? p / 1000 : 0;
      };
      let attempts = (activeUnit ? relatedAttempts(activeData, activeUnit) : (activeData.attempt_log || []))
        .slice()
        .sort((a, b) => attemptTime(b) - attemptTime(a));
      // comprehensive filters (kind / free-text / row cap)
      const f = store.logFilter || { kind: "all", q: "", limit: 80 };
      const total = attempts.length;
      if (f.kind && f.kind !== "all") attempts = attempts.filter(a => (a.kind || "") === f.kind);
      if (f.q) {
        const q = f.q.toLowerCase();
        attempts = attempts.filter(a =>
          `${a.message || ""} ${a.function || ""} ${a.agent || ""} ${a.file || ""}`.toLowerCase().includes(q));
      }
      const cap = f.limit || 80;
      const shown = attempts.slice(0, cap);
      const scope = activeUnit ? ` for ${unitDisplayName(activeUnit)}` : "";
      const filtered = (f.kind !== "all" || f.q) ? ` (filtered from ${total})` : "";
      setText($("decomp-log-note"), `${shown.length}${attempts.length > cap ? "/" + attempts.length : ""} entries${scope}${filtered}`);
      if (!shown.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No log entries match this filter");
        list.append(empty);
        return;
      }
      for (const attempt of shown) {
        const row = document.createElement("div");
        row.className = `attempt-row k-${attempt.kind || "note"}`;
        const time = document.createElement("div");
        time.className = "attempt-time";
        setText(time, hstTime(Number.isFinite(Number(attempt.unix)) ? Number(attempt.unix) : attempt.timestamp));
        const agent = document.createElement("div");
        agent.className = "attempt-agent mono";
        setText(agent, attempt.agent || "");
        const kind = document.createElement("div");
        kind.className = "attempt-kind";
        setText(kind, attempt.kind);
        const message = document.createElement("div");
        message.className = "attempt-message";
        setText(message, attempt.message);
        row.append(time, agent, kind, message);
        list.append(row);
      }
    }
    function renderCharts(data) {
      if (data.charts) {
        drawDonut($("status-chart"), data.charts.status || []);
        drawBars($("provenance-chart"), data.charts.provenance || []);
      }
      store._history = data.history || store._history || [];
      drawHistoryRanged();
      renderSourceBars(data);
    }
    // #3: collapse the per-minute history ring into one point per COMMIT so the
    // "match progress over time" line steps at real milestones, not every minute.
    // Mirrors the server-side collapse_history_to_commits(): keep each commit's
    // last (newest) sample, then drop commits whose tracked %s didn't change.
    const HISTORY_PCT_KEYS = ["decomp_code_pct", "decomp_fuzzy_pct", "decomp_functions_pct", "c_converted_pct"];
    function collapseHistoryToCommits(rows) {
      rows = (rows || []).filter(r => r && typeof r === "object");
      if (!rows.length) return [];
      // 1) consecutive same-head runs collapse to the run's last sample.
      const perCommit = [];
      for (const r of rows) {
        const head = String(r.head || "");
        const prev = perCommit[perCommit.length - 1];
        if (prev && head && String(prev.head || "") === head) perCommit[perCommit.length - 1] = r;
        else perCommit.push(r);
      }
      // 2) drop commits whose tracked %s match the previously kept one.
      const sig = r => HISTORY_PCT_KEYS.map(k => Number(r[k] || 0).toFixed(4)).join("|");
      const out = [];
      let last = null;
      for (const r of perCommit) {
        const s = sig(r);
        if (last !== null && s === last) continue;
        out.push(r); last = s;
      }
      return out;
    }
    // history time-range filter (24h / 3d / 7d / 14d / all). The collapsed
    // commit-milestone series is always passed in full; `store.histView` (a
    // {min,max} unix window set by drag-pan/wheel-zoom, #2) decides what's shown.
    // When no manual view is active, the range buttons (days) set the window.
    function drawHistoryRanged() {
      // Step the line at commits/milestones (#3) instead of every minute-sample.
      const all = collapseHistoryToCommits(store._history || []);
      if (all.length < 2) { drawHistory($("history-chart"), all); updateHistHint(all, null); return; }
      const dataMin = all.reduce((m, r) => Math.min(m, Number(r.unix) || Infinity), Infinity);
      const dataMax = all.reduce((m, r) => Math.max(m, Number(r.unix) || 0), 0);
      let domain;
      if (store.histView && Number.isFinite(store.histView.min) && Number.isFinite(store.histView.max)) {
        // Manual pan/zoom window, clamped to the available data extent.
        let lo = Math.max(dataMin, store.histView.min);
        let hi = Math.min(dataMax, store.histView.max);
        if (hi - lo < 1800) { hi = Math.min(dataMax, lo + 1800); lo = Math.max(dataMin, hi - 1800); }
        store.histView = { min: lo, max: hi };
        domain = [lo, hi];
      } else {
        // Default: the days range-button window (or full when "All").
        const days = store.historyDays == null ? 14 : store.historyDays;
        if (days > 0) {
          const cutoff = dataMax - days * 86400;
          domain = [Math.max(dataMin, cutoff), dataMax];
        } else {
          domain = [dataMin, dataMax];
        }
      }
      drawHistory($("history-chart"), all, domain);
      updateHistHint(all, domain);
    }
    // Small live readout of the visible window + zoom affordance hint (#2).
    function updateHistHint(all, domain) {
      const el = $("timeline-range");
      if (!el) return;
      if (!all || all.length < 2 || !domain) { setText(el, "no snapshots yet"); return; }
      const f = u => new Date(Number(u) * 1000).toLocaleString("en-US", { timeZone: "Pacific/Honolulu", month: "numeric", day: "numeric", hour: "2-digit", minute: "2-digit", hour12: false });
      const manual = !!store.histView;
      setText(el, `${f(domain[0])} → ${f(domain[1])} HST · drag to pan, scroll to zoom${manual ? " · " : ""}`);
      // (re)build a tiny reset button after the readout when zoomed/panned.
      let reset = $("hist-reset");
      if (manual && !reset) {
        reset = document.createElement("button");
        reset.id = "hist-reset"; reset.type = "button"; reset.className = "range-btn hist-reset";
        setText(reset, "reset zoom");
        reset.addEventListener("click", () => { store.histView = null; drawHistoryRanged(); });
        el.append(reset);
      } else if (!manual && reset) {
        reset.remove();
      }
    }
    // #2: drag-to-pan + wheel-zoom on the history chart's time axis. Bound once.
    function bindHistoryPanZoom() {
      const canvas = $("history-chart");
      if (!canvas || canvas._panZoomBound) return;
      canvas._panZoomBound = true;
      const curWindow = () => {
        const c = canvas._chart;
        if (c && Number.isFinite(c.minX) && Number.isFinite(c.maxX)) return { min: c.minX, max: c.maxX, dmin: c.dataMinX, dmax: c.dataMaxX };
        return null;
      };
      // wheel: zoom around the cursor's time position.
      canvas.addEventListener("wheel", evt => {
        const c = canvas._chart, win = curWindow();
        if (!c || !win) return;
        evt.preventDefault();
        const rect = canvas.getBoundingClientRect();
        const mx = evt.clientX - rect.left;
        const plotL = c.pad.l, plotR = c.w - c.pad.r;
        const frac = Math.max(0, Math.min(1, (mx - plotL) / Math.max(1, plotR - plotL)));
        const span = win.max - win.min;
        const anchor = win.min + frac * span;
        const factor = evt.deltaY < 0 ? 0.8 : 1.25;   // up = zoom in
        let newSpan = Math.max(1800, Math.min(win.dmax - win.dmin, span * factor));
        let lo = anchor - frac * newSpan;
        let hi = lo + newSpan;
        // clamp into data extent without shrinking the span
        if (lo < win.dmin) { lo = win.dmin; hi = lo + newSpan; }
        if (hi > win.dmax) { hi = win.dmax; lo = hi - newSpan; }
        store.histView = { min: Math.max(win.dmin, lo), max: Math.min(win.dmax, hi) };
        drawHistoryRanged();
      }, { passive: false });
      // drag: pan the window left/right.
      let dragging = false, lastX = 0;
      canvas.addEventListener("mousedown", evt => {
        const win = curWindow(); if (!win) return;
        dragging = true; lastX = evt.clientX; canvas.classList.add("grabbing");
      });
      window.addEventListener("mousemove", evt => {
        if (!dragging) return;
        const c = canvas._chart, win = curWindow();
        if (!c || !win) { dragging = false; return; }
        const plotW = (c.w - c.pad.l - c.pad.r) || 1;
        const span = win.max - win.min;
        const dxPx = evt.clientX - lastX;
        lastX = evt.clientX;
        const dt = -(dxPx / plotW) * span;   // drag right -> move window earlier
        let lo = win.min + dt, hi = win.max + dt;
        if (lo < win.dmin) { lo = win.dmin; hi = lo + span; }
        if (hi > win.dmax) { hi = win.dmax; lo = hi - span; }
        store.histView = { min: lo, max: hi };
        drawHistoryRanged();
      });
      window.addEventListener("mouseup", () => { dragging = false; canvas.classList.remove("grabbing"); });
    }
    // ---- v9: agent activity ------------------------------------------------
    function renderAgents(payload) {
      const body = $("agents-body");
      body.replaceChildren();
      const rows = (payload && payload.agents) || [];
      setText($("agents-note"), payload && payload.available
        ? `${rows.length} agent(s) | ${payload.claimed || 0} claimed, ${payload.queued || 0} queued`
        : "coordination data unavailable");
      if (!rows.length) {
        const tr = document.createElement("tr");
        const cell = document.createElement("td");
        cell.colSpan = 5;
        cell.className = "empty-state";
        setText(cell, "No active claims in coordination/claims.json");
        tr.append(cell);
        body.append(tr);
        return;
      }
      for (const a of rows) {
        const tr = document.createElement("tr");
        const agent = document.createElement("td");
        agent.className = "mono";
        setText(agent, a.agent);
        const fn = document.createElement("td");
        fn.className = "mono";
        setText(fn, a.function || "-");
        const file = document.createElement("td");
        file.className = "mono";
        setText(file, a.file ? fileName(a.file) : "-");
        const claimed = document.createElement("td");
        claimed.className = "mono";
        setText(claimed, a.claimed_at ? hstTime(a.claimed_at) : "-");
        const state = document.createElement("td");
        const dot = document.createElement("span");
        dot.className = `agent-dot ${a.busy ? "busy" : "idle"}`;
        state.append(dot, document.createTextNode(a.busy ? "busy" : (a.task_status || "idle")));
        tr.append(agent, fn, file, claimed, state);
        body.append(tr);
      }
    }
    function pollAgents() {
      fetch("/api/agents", { cache: "no-store" })
        .then(r => r.json()).then(renderAgents).catch(() => {});
    }
    // ---- fleet locks (SQLite coordination/locks.db) ------------------------
    function lockAction(action, payload) {
      return fetch(`/api/locks/${action}`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(payload || {}),
        cache: "no-store",
      }).then(r => r.json()).then(() => pollLocks()).catch(() => {});
    }
    function renderLocks(payload) {
      const body = $("locks-body");
      if (!body) return;
      body.replaceChildren();
      const rows = (payload && payload.locks) || [];
      setText($("locks-note"), payload && payload.available
        ? `${payload.file_count || 0} file, ${payload.fn_count || 0} fn`
        : "no lock DB yet");
      if (!rows.length) {
        const tr = document.createElement("tr");
        const cell = document.createElement("td");
        cell.colSpan = 5;
        cell.className = "empty-state";
        setText(cell, "No active locks");
        tr.append(cell);
        body.append(tr);
        return;
      }
      for (const lk of rows) {
        const tr = document.createElement("tr");
        const scope = document.createElement("td");
        const tag = document.createElement("span");
        tag.className = `lock-scope ${lk.scope}`;
        setText(tag, lk.scope);
        scope.append(tag);
        const key = document.createElement("td");
        key.className = "mono";
        setText(key, lk.scope === "file" ? fileName(lk.key) : lk.key);
        key.title = lk.key + (lk.note ? ` — ${lk.note}` : "");
        const owner = document.createElement("td");
        owner.className = "mono";
        setText(owner, lk.owner || "-");
        const ttl = document.createElement("td");
        ttl.className = "mono";
        setText(ttl, lk.ttl_remaining == null ? "∞" : fmtCountdown(lk.ttl_remaining));
        if (lk.ttl_remaining != null && lk.ttl_remaining < 120) ttl.classList.add("hud-warn");
        const act = document.createElement("td");
        act.className = "lock-actions";
        const renew = document.createElement("button");
        renew.className = "btn ghost xs";
        setText(renew, "Renew");
        renew.onclick = () => lockAction("renew", { agent: lk.owner, key: lk.key, scope: lk.scope });
        const rel = document.createElement("button");
        rel.className = "btn ghost xs danger";
        setText(rel, "Release");
        rel.title = "Force-release this lock";
        rel.onclick = () => lockAction("release", { agent: lk.owner, key: lk.key, scope: lk.scope, force: true });
        act.append(renew, rel);
        tr.append(scope, key, owner, ttl, act);
        body.append(tr);
      }
    }
    function pollLocks() {
      fetch("/api/locks", { cache: "no-store" })
        .then(r => r.json()).then(renderLocks).catch(() => {});
    }

    // ===== orchestrator panels: sync / prs / ship / leases / reports / quantum =====
    function setT(id, v) { const e = $(id); if (e) setText(e, v); }
    function postJSON(url, body) {
      return fetch(url, { method: "POST", headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body || {}), cache: "no-store" }).then(r => r.json());
    }
    function renderSync(d) {
      if (!d) return;
      setT("sync-summary", d.available ? d.summary : "git unavailable");
      setT("sync-branch", d.branch || "-");
      setT("sync-head", d.head || "-");
      setT("sync-ab", `${d.ahead || 0} ahead / ${d.behind || 0} behind`);
      setT("sync-dirty", d.dirty == null ? "-" : String(d.dirty));
      const s = $("sync-summary");
      if (s) s.className = "rc-note " + (d.on_master ? "warn" : "good");
    }
    function pollSync() { fetch("/api/sync", { cache: "no-store" }).then(r => r.json()).then(renderSync).catch(() => {}); }

    function renderPrs(d) {
      if (!d) return;
      setT("prs-note", d.available
        ? `${d.open_count || 0} open · ${d.draft_count || 0} draft · ${d.merged_count || 0} merged`
        : "gh unavailable");
      const link = $("link-pokedex");
      const list = $("pr-list");
      if (!list) return;
      list.replaceChildren();
      const open = (d.open || []);
      if (!d.available) { const e = document.createElement("div"); e.className = "empty-state"; setText(e, d.error || "gh CLI not found"); list.append(e); return; }
      if (!open.length) { const e = document.createElement("div"); e.className = "empty-state"; setText(e, "No open PRs"); list.append(e); }
      for (const pr of open.slice(0, 8)) {
        const row = document.createElement("a");
        row.className = "pr-row"; row.href = pr.url || "#"; row.target = "_blank"; row.rel = "noopener";
        const num = document.createElement("span"); num.className = "pr-num mono"; setText(num, `#${pr.number}`);
        const title = document.createElement("span"); title.className = "pr-title"; setText(title, pr.title || pr.headRefName || "");
        const st = document.createElement("span");
        const dec = (pr.reviewDecision || (pr.isDraft ? "DRAFT" : "OPEN")).toLowerCase().replace(/_/g, " ");
        st.className = "pr-state " + (pr.isDraft ? "draft" : (pr.reviewDecision === "APPROVED" ? "approved" : "open"));
        setText(st, dec);
        row.append(num, title, st);
        list.append(row);
      }
    }
    function pollPrs() { fetch("/api/prs", { cache: "no-store" }).then(r => r.json()).then(renderPrs).catch(() => {}); }

    function renderShip(d) {
      if (!d) return;
      setT("ship-state", d.state || "");
      setT("ship-confirmed", d.confirmed || 0);
      setT("ship-regress", d.regressions || 0);
      const st = $("ship-state"); if (st) st.className = "rc-note " + (d.ready ? "good" : "warn");
      const hint = $("ship-hint");
      if (hint) setText(hint, d.on_master
        ? "Blocked on master. Create/use a feature branch before Prepare handoff."
        : (d.ready ? `Branch ${d.branch} - ${d.confirmed} wins ready to PR.` : `Branch ${d.branch || "-"} - not ready.`));
      const wins = $("ship-wins");
      if (wins) {
        wins.replaceChildren();
        for (const f of (d.files || []).slice(0, 8)) {
          const row = document.createElement("div");
          row.className = "ship-win";
          const head = document.createElement("b");
          setText(head, `${f.file}: ${f.count} win${Number(f.count) === 1 ? "" : "s"}`);
          const src = document.createElement("div");
          setText(src, f.source || "");
          const funcs = document.createElement("span");
          funcs.className = "mono";
          setText(funcs, (f.functions || []).join(", "));
          row.append(head, src, funcs);
          wins.append(row);
        }
        if (!(d.files || []).length) {
          const empty = document.createElement("div");
          empty.className = "empty-state";
          setText(empty, "No non-empty band-win bundles");
          wins.append(empty);
        }
      }
      const handoff = $("ship-handoff");
      if (handoff) {
        handoff.disabled = !d.ready;
        handoff.title = d.ready ? "Push current branch and open a PR" : "Not ready for handoff";
      }
      setT("run-active", (store._leases && store._leases.active_count) || 0);
      setT("run-queued", (store._leases && store._leases.queued_count) || 0);
    }
    function pollShip() { fetch("/api/ship", { cache: "no-store" }).then(r => r.json()).then(renderShip).catch(() => {}); }

    function renderLeases(d) {
      store._leases = d || {};
      // expose active leases to the treemap: by file (file-level tiles) AND by fn
      // (function-level tiles, after drilling into a file) for the working pulse.
      store.tmAgents = {}; store.tmAgentFns = {};
      for (const l of ((d && d.active) || [])) {
        const who = l.owner || l.tag || "?";
        const f = String(l.active_src || l.file || "").replace(/\\/g, "/");
        if (f) store.tmAgents[f] = who;
        if (l.active_fn) store.tmAgentFns[l.active_fn] = who;
      }
      if (store.tm && typeof renderTreemap === "function") { try { renderTreemap(); } catch (e) {} }
      const aN = $("lease-active-n"), qN = $("lease-queued-n");
      if (aN) setText(aN, d.active_count || 0);
      if (qN) setText(qN, d.queued_count || 0);
      setT("run-active", d.active_count || 0);
      setT("run-queued", d.queued_count || 0);
      setT("hud-active-leases", d.active_count || 0);
      setT("hud-queued", d.queued_count || 0);
      const ab = $("lease-active-body");
      if (ab) {
        ab.replaceChildren();
        if (!(d.active || []).length) { const tr = document.createElement("tr"); const td = document.createElement("td"); td.colSpan = 7; td.className = "empty-state"; setText(td, "No active leases right now"); tr.append(td); ab.append(tr); }
        for (const l of (d.active || [])) {
          const tr = document.createElement("tr");
          const scope = document.createElement("td"); const tag = document.createElement("span"); tag.className = `lock-scope ${l.scope}`; setText(tag, l.scope); scope.append(tag);
          // Separate columns (agent / file / function / attempts), derived read-only
          // from the lock owner + band scratch sidecar + attempt log.
          const agent = document.createElement("td"); agent.className = "mono"; setText(agent, l.owner || l.tag || "-");
          const fileTd = document.createElement("td"); fileTd.className = "mono";
          const srcFile = l.active_src ? fileName(l.active_src) : (l.scope === "file" ? fileName(l.key) : "");
          setText(fileTd, srcFile || "-"); fileTd.title = l.active_src || l.key || "";
          const fnTd = document.createElement("td"); fnTd.className = "mono";
          if (l.active_fn) { setText(fnTd, l.active_fn); }
          else { const hint = document.createElement("span"); hint.className = "lease-hint"; setText(hint, "(pending in log)"); fnTd.append(hint); }
          const att = document.createElement("td"); att.className = "mono"; setText(att, String(l.attempts || 0));
          if ((l.attempts || 0) >= 8) att.style.color = "#ff7a7a";  // high-waste fn
          const el = document.createElement("td"); el.className = "mono"; setText(el, l.elapsed == null ? "-" : fmtCountdown(l.elapsed));
          const ttl = document.createElement("td"); ttl.className = "mono"; setText(ttl, l.ttl_remaining == null ? "∞" : fmtCountdown(l.ttl_remaining));
          tr.append(scope, agent, fileTd, fnTd, att, el, ttl); ab.append(tr);
        }
      }
      const qb = $("lease-queued-body");
      if (qb) {
        qb.replaceChildren();
        if (!(d.queued || []).length) { const tr = document.createElement("tr"); const td = document.createElement("td"); td.colSpan = 3; td.className = "empty-state"; setText(td, "Queue empty"); tr.append(td); qb.append(tr); }
        for (const q of (d.queued || []).slice(0, 200)) {
          const tr = document.createElement("tr");
          const fn = document.createElement("td"); fn.className = "mono"; setText(fn, q.function || "-");
          const pr = document.createElement("td"); const tag = document.createElement("span"); tag.className = `prio prio-${q.priority || "normal"}`; setText(tag, q.priority || "normal"); pr.append(tag);
          const desc = document.createElement("td"); setText(desc, q.description || "");
          tr.append(fn, pr, desc); qb.append(tr);
        }
      }
    }
    function pollLeases() { fetch("/api/leases", { cache: "no-store" }).then(r => r.json()).then(renderLeases).catch(() => {}); }

    // #3/#4: render the unified active-work table (locks + scratch + commits) with
    // a LIVE per-file match%. Rows are clickable -> drill into the unit close-out.
    function fmtFreshAge(unix) {
      const now = Date.now() / 1000;
      const s = Math.max(0, Math.floor(now - Number(unix || 0)));
      if (!unix) return "-";
      if (s < 90) return s + "s ago";
      if (s < 5400) return Math.round(s / 60) + "m ago";
      if (s < 172800) return Math.round(s / 3600) + "h ago";
      return Math.round(s / 86400) + "d ago";
    }
    function renderActiveWork(d) {
      store._activeWork = d || {};
      const body = $("active-work-body");
      setT("active-work-note", d && d.available ? `${d.count} files · ${d.lease_count || 0} leased` : "no active work detected");
      if (!body) return;
      body.replaceChildren();
      const rows = (d && d.active) || [];
      if (!rows.length) {
        const tr = document.createElement("tr"); const td = document.createElement("td");
        td.colSpan = 5; td.className = "empty-state"; setText(td, "No active work (no locks, fresh scratch, or recent commits)");
        tr.append(td); body.append(tr); return;
      }
      for (const e of rows.slice(0, 40)) {
        const tr = document.createElement("tr"); tr.className = "active-work-row";
        const u = e.unit || null;
        // File (+ best-effort fn)
        const file = document.createElement("td"); file.className = "mono";
        const fileMain = document.createElement("span"); setText(fileMain, fileName(e.src) + (e.fn ? " → " + e.fn : ""));
        file.append(fileMain); file.title = e.src;
        // Live % (functions_pct with matched/total) + a mini bar
        const pctTd = document.createElement("td"); pctTd.className = "mono";
        if (u) {
          const p = Number(u.functions_pct || 0);
          setText(pctTd, `${pctText(u.functions_pct)} (${u.matched_functions || 0}/${u.total_functions || 0})`);
          pctTd.append(miniBar(p));
        } else { setText(pctTd, "-"); }
        // Who (owners/tags/authors)
        const who = document.createElement("td"); who.className = "mono"; setText(who, (e.owners || []).join(", ") || "-");
        // Signal chips
        const sig = document.createElement("td");
        for (const s of (e.signals || [])) {
          const chip = document.createElement("span"); chip.className = "sig-chip sig-" + s; setText(chip, s); sig.append(chip);
        }
        if (e.lease) tr.classList.add("is-lease");
        // Freshness
        const fresh = document.createElement("td"); fresh.className = "mono"; setText(fresh, fmtFreshAge(e.fresh));
        tr.append(file, pctTd, who, sig, fresh);
        if (u && (u.source || e.src)) {
          tr.classList.add("active-work-click");
          tr.title = "Open " + e.src + " close-out detail";
          tr.addEventListener("click", () => drillIntoUnit({ source: u.source || e.src, name: u.name || "" }));
        }
        body.append(tr);
      }
    }
    function pollActiveWork() {
      fetch("/api/active", { cache: "no-store" }).then(r => r.json()).then(payload => {
        renderActiveWork(payload);
        renderActiveFilesInDetail(payload);   // #3: feed the decomp-detail active list
      }).catch(() => {});
    }
    // #3: compact "active files + live %" block shown in the decomp detail panel
    // at the files level. Clicking a row drills into that unit's close-out detail.
    function renderActiveFilesInDetail(d) {
      const host = $("decomp-active-files");
      if (!host) return;   // only present at the files level
      host.replaceChildren();
      const rows = (d && d.active) || [];
      const title = document.createElement("div");
      title.className = "panel-title";
      const h = document.createElement("h2"); setText(h, "Active files");
      const note = document.createElement("span"); note.className = "panel-note";
      setText(note, rows.length ? `${rows.length} being worked · live %` : "no active files detected");
      title.append(h, note); host.append(title);
      if (!rows.length) return;
      const list = document.createElement("div");
      list.className = "active-files-list";
      for (const e of rows.slice(0, 12)) {
        const u = e.unit || null;
        const row = document.createElement("div");
        row.className = "active-file-row";
        const name = document.createElement("div"); name.className = "af-name mono";
        setText(name, fileName(e.src) + (e.fn ? " → " + e.fn : ""));
        const pct = document.createElement("div"); pct.className = "af-pct mono";
        if (u) { setText(pct, pctText(u.functions_pct)); pct.append(miniBar(Number(u.functions_pct || 0))); }
        else setText(pct, "-");
        const who = document.createElement("div"); who.className = "af-who";
        setText(who, (e.owners || []).slice(0, 3).join(", "));
        const fresh = document.createElement("div"); fresh.className = "af-fresh";
        setText(fresh, fmtFreshAge(e.fresh));
        row.append(name, pct, who, fresh);
        if (u && (u.source || e.src)) {
          row.classList.add("active-work-click");
          row.title = "Open " + e.src;
          row.addEventListener("click", () => drillIntoUnit({ source: u.source || e.src, name: u.name || "" }));
        }
        list.append(row);
      }
      host.append(list);
    }

    const REPORT_STATUSES = ["exact", "improved", "committed", "no progress", "needs rework", "tool error"];
    function renderReports(d) {
      store._reports = d || {};
      const filt = $("report-filters");
      const counts = (d && d.counts) || {};
      if (filt && !filt.dataset.built) {
        filt.dataset.built = "1";
        const all = document.createElement("button"); all.className = "chip-btn active"; all.dataset.rstatus = "all"; setText(all, "all"); filt.append(all);
        for (const s of REPORT_STATUSES) {
          const b = document.createElement("button"); b.className = "chip-btn"; b.dataset.rstatus = s; setText(b, s); filt.append(b);
        }
        filt.addEventListener("click", e => {
          const btn = e.target.closest("button[data-rstatus]"); if (!btn) return;
          store.reportFilter = btn.dataset.rstatus;
          filt.querySelectorAll("button").forEach(x => x.classList.toggle("active", x === btn));
          renderReports(store._reports);
        });
      }
      if (filt) filt.querySelectorAll("button[data-rstatus]").forEach(b => {
        const s = b.dataset.rstatus; const n = s === "all" ? (d && d.total) || 0 : (counts[s] || 0);
        b.dataset.n = n; b.style.opacity = (s !== "all" && !n) ? ".4" : "1";
      });
      setT("reports-note", d && d.available ? `${d.total} reports` : "no reports");
      const body = $("reports-body"); if (!body) return;
      body.replaceChildren();
      const want = store.reportFilter || "all";
      let reps = (d && d.reports) || [];
      if (want !== "all") reps = reps.filter(r => r.status === want);
      if (!reps.length) { const e = document.createElement("div"); e.className = "empty-state"; setText(e, "No worker reports"); body.append(e); return; }
      for (const r of reps.slice(0, 40)) {
        const card = document.createElement("div"); card.className = "report-card report-clickable";
        const top = document.createElement("div"); top.className = "report-top";
        const fn = document.createElement("span"); fn.className = "report-fn mono"; setText(fn, r.function || r.file || "—");
        // #4: show the report's timestamp inline (HST), right-aligned in the header.
        const ts = document.createElement("span"); ts.className = "report-ts";
        setText(ts, r.timestamp ? hstTime(r.timestamp) : "");
        const chip = document.createElement("span"); chip.className = "report-chip s-" + r.status.replace(/\s+/g, "-"); setText(chip, r.status);
        top.append(fn, ts, chip);
        const meta = document.createElement("div"); meta.className = "report-meta";
        const who = r.agent || "?"; const where = r.file ? ` · ${fileName(r.file)}` : "";
        const pct = (r.percent != null) ? ` · ${Number(r.percent).toFixed(2)}%` : "";
        setText(meta, `${who}${where}${pct}`);
        const msg = document.createElement("div"); msg.className = "report-msg"; setText(msg, r.message || "");
        card.append(top, meta, msg);
        // #4: click-to-summarize — expand a detail panel with the full message,
        // exact timestamp, agent, status and percent in place. Toggles per-card.
        const detail = document.createElement("div"); detail.className = "report-detail";
        const dl = (k, v) => {
          if (v == null || v === "") return;
          const row = document.createElement("div"); row.className = "report-dl";
          const kk = document.createElement("span"); kk.className = "report-dk"; setText(kk, k);
          const vv = document.createElement("span"); vv.className = "report-dv"; setText(vv, String(v));
          row.append(kk, vv); detail.append(row);
        };
        dl("when", r.timestamp ? `${hstTime(r.timestamp)}  (${r.timestamp})` : "");
        dl("agent", r.agent || "");
        dl("status", r.status || "");
        dl("function", r.function || "");
        dl("file", r.file ? fileName(r.file) : "");
        dl("percent", r.percent != null ? `${Number(r.percent).toFixed(2)}%` : "");
        const full = document.createElement("div"); full.className = "report-dl";
        const fk = document.createElement("span"); fk.className = "report-dk"; setText(fk, "message");
        const fv = document.createElement("span"); fv.className = "report-dv report-full"; setText(fv, r.message || "");
        full.append(fk, fv); detail.append(full);
        card.append(detail);
        card.addEventListener("click", () => card.classList.toggle("expanded"));
        body.append(card);
      }
    }
    function pollReports() { fetch("/api/reports", { cache: "no-store" }).then(r => r.json()).then(renderReports).catch(() => {}); }

    function renderQuantum(d) {
      const grid = $("quantum-grid"); if (!grid) return;
      grid.replaceChildren();
      if (!d || !d.available) {
        const e = document.createElement("div"); e.className = "empty-state";
        setText(e, "Permuter idle (no .omc/permuter_state.json)"); grid.append(e);
        setT("quantum-note", "idle"); return;
      }
      const s = d.state || {};
      // expose the permuter's active anneal targets to the treemap (file -> {fn,iter,best})
      // so the tile the swarm is currently cracking gets its own (amber) animation.
      store.tmPermuter = {}; store.tmPermuterFns = {};
      const pact = s.active || {};
      for (const k in pact) {
        const a = pact[k]; const f = String(a.file || "").replace(/\\/g, "/");
        if (f) store.tmPermuter[f] = { fn: a.fn, iter: a.iter, best: a.best };
        if (a.fn) store.tmPermuterFns[a.fn] = { iter: a.iter, best: a.best };
      }
      if (store.tm && typeof renderTreemap === "function") { try { renderTreemap(); } catch (e) {} }
      // grind2.py writes: active_fn/active_file, iteration, score, best_score, queue,
      // workers, plus a per-worker `active` map. Lower score = closer (0 = exact match).
      setT("quantum-note", s.active_fn || s.fn || s["function"] || "permuter swarm");
      const num = (v, dp) => v != null ? (dp != null ? Number(v).toFixed(dp) : v) : "-";
      const qn = Array.isArray(s.queue) ? s.queue.length : s.queue;
      const cells = [
        ["best score", num(s.best_score != null ? s.best_score : s.best)],
        ["score", num(s.score != null ? s.score : s.energy)],
        ["active", s.active_file ? String(s.active_file).split("/").pop() : "-"],
        ["iters", num(s.iteration != null ? s.iteration : (s.iterations != null ? s.iterations : s.iters))],
        ["queue", num(qn)],
        ["workers", num(s.workers)],
      ];
      for (const [k, v] of cells) {
        const c = document.createElement("div"); c.className = "q-cell";
        const kv = document.createElement("div"); kv.className = "q-k"; setText(kv, k);
        const vv = document.createElement("div"); vv.className = "q-v mono"; setText(vv, String(v));
        c.append(kv, vv); grid.append(c);
      }
    }
    function pollQuantum() { fetch("/api/quantum", { cache: "no-store" }).then(r => r.json()).then(renderQuantum).catch(() => {}); }

    const BUCKET_COLORS = { DONE: "#38b995", EQUIV: "#7da0c4", NEARWALL: "#e0a93b", STRUCT: "#c77dff", ASM: "#5aa9e6", LOW: "#8da0b8" };
    function renderBuckets(d) {
      const el = document.getElementById("bucket-bars");
      const note = document.getElementById("bucket-note");
      if (!el) return;
      if (!d || !d.available || !Array.isArray(d.buckets)) {
        el.innerHTML = '<div class="bucket-counts">no ledger yet — run <code>wall_ledger.py build</code></div>';
        if (note) note.textContent = "";
        return;
      }
      if (note) {
        const rom = d.match_total ? `${d.match_pct}% ROM-match (${d.match_fns.toLocaleString()}/${d.match_total.toLocaleString()}, incl. asm-wrappers)` : "";
        const fresh = (d.measure_fresh_age == null) ? "" :
          ` · live measures: ${(d.measure_fresh_fns||0).toLocaleString()} fns, ${d.measure_fresh_age < 120 ? "fresh" : Math.round(d.measure_fresh_age/60)+"m old"}`;
        note.textContent = `${rom} · ${d.overall_pct}% decompiled to C (${d.done_fns.toLocaleString()}/${d.total_fns.toLocaleString()}) · bar = % of bucket attacked${fresh}`;
      }
      el.innerHTML = d.buckets.map(b => {
        const c = BUCKET_COLORS[b.name] || "#5aa9e6";
        return `<div class="bucket-row">
          <div class="bucket-label" title="${b.desc}">${b.name}</div>
          <div class="bucket-track">
            <div class="bucket-fill" style="width:${b.pct}%;background:${c}"></div>
            <span class="bucket-pct">${b.pct}%</span>
          </div>
          <div class="bucket-counts">${b.attempted.toLocaleString()}/${b.total.toLocaleString()} <span class="muted">(${b.remaining.toLocaleString()} left)</span></div>
        </div>`;
      }).join("");
    }
    function pollBuckets() { fetch("/api/buckets", { cache: "no-store" }).then(r => r.json()).then(renderBuckets).catch(() => {}); }
    // ---- v10: live attempt-log poll (independent of the slow /api/state) ----
    function pollLog() {
      fetch("/api/log?limit=1000", { cache: "no-store" })
        .then(r => r.json())
        .then(payload => {
          const merged = (payload && payload.attempt_log) || [];
          // Render the log even before the (slow) /api/state arrives, so the
          // Decomp attempt log is never blank just because build_state lagged.
          store._logData = { attempt_log: merged, decomp: (store.data && store.data.decomp) || {} };
          if (!store.data) store.data = { attempt_log: merged, decomp: {} };
          store.data.attempt_log = merged;
          // Re-render the decomp attempt log against the currently-drilled unit
          // (so new git-commit + status.md entries appear without a manual reload).
          const tm = store.tm;
          let unit = null;
          if (tm.level === "unit") {
            const units = (store.data.decomp || {}).units || [];
            unit = units.find(u => (u.source || "") === tm.unitSource)
              || units.find(u => unitDisplayName(u) === tm.unitName) || null;
          }
          renderDecompAttemptLog(store.data, unit);
        })
        .catch(() => {});
    }
    // ---- v9: lockout reset countdowns --------------------------------------
    function fmtCountdown(secs) {
      secs = Math.max(0, Math.floor(secs));
      const h = Math.floor(secs / 3600);
      const m = Math.floor((secs % 3600) / 60);
      const s = secs % 60;
      if (h > 0) return `${h}h ${String(m).padStart(2, "0")}m`;
      if (m > 0) return `${m}m ${String(s).padStart(2, "0")}s`;
      return `${s}s`;
    }
    function renderLimits(payload) {
      store.limits = (payload && payload.agents) || [];
      setText($("limits-note"), payload && payload.available ? `${store.limits.length} tracked` : "agent_limits.json TODO");
      tickLimits();
    }
    function tickLimits() {
      const grid = $("limits-grid");
      grid.replaceChildren();
      if (!store.limits.length) {
        const empty = document.createElement("div");
        empty.className = "empty-state";
        setText(empty, "No agents in tools/decomp_work/agent_limits.json");
        grid.append(empty);
        return;
      }
      const now = Date.now() / 1000;
      for (const a of store.limits) {
        const card = document.createElement("div");
        card.className = "limit-card";
        const label = document.createElement("div");
        label.className = "limit-label";
        setText(label, a.label || a.name);
        const cd = document.createElement("div");
        const remaining = a.next_reset_unix ? a.next_reset_unix - now : 0;
        cd.className = "limit-countdown" + (remaining > 0 && remaining < 1800 ? " soon" : "");
        setText(cd, a.next_reset_unix ? fmtCountdown(remaining) : "n/a");
        const note = document.createElement("div");
        note.className = "limit-note";
        const assumed = a.assumed ? " (assumed)" : "";
        setText(note, a.next_reset_unix ? `resets ${hstTime(a.next_reset_unix)} HST${assumed}` : (a.note || "set last_reset"));
        card.append(label, cd, note);
        const models = Array.isArray(a.models) ? a.models : [];
        if (models.length || a.assumed) {
          const chips = document.createElement("div");
          chips.className = "limit-models";
          for (const model of models.slice(0, 5)) {
            const chip = document.createElement("span");
            chip.className = "model-chip" + (a.assumed ? " assumed" : "");
            setText(chip, model);
            chips.append(chip);
          }
          if (a.assumed) {
            const chip = document.createElement("span");
            chip.className = "model-chip assumed";
            setText(chip, "editable");
            chips.append(chip);
          }
          card.append(chips);
        }
        grid.append(card);
      }
    }
    function pollLimits() {
      fetch("/api/limits", { cache: "no-store" })
        .then(r => r.json()).then(renderLimits).catch(() => {});
    }
    // ---- per-agent token usage vs 5h/weekly caps ----------------------------
    // (token formatting reuses the existing `fmtTok` defined with the token chart)
    function usageColor(pct) {
      if (pct == null) return "#5aa9e6";          // no cap configured -> neutral
      if (pct >= 90) return "#e05a5a";             // red
      if (pct >= 70) return "#e0a93b";             // amber
      return "#38b995";                            // green
    }
    function renderAgentUsage(d) {
      const el = $("agent-usage-bars"); if (!el) return;
      const provs = (d && d.providers) || [];
      if (!provs.length) {
        el.innerHTML = '<div class="bucket-counts">no token data yet</div>';
        setText($("agent-usage-note"), ""); return;
      }
      setText($("agent-usage-note"), "5h / weekly rolling");
      const rows = [];
      for (const p of provs) {
        const live = p.live_status ? ` &middot; <span class="muted">live: ${p.live_status}</span>` : "";
        rows.push(`<div class="bucket-counts" style="margin-top:8px;font-weight:600">${p.label}${live}</div>`);
        for (const win of ["5h", "weekly"]) {
          const w = p.windows && p.windows[win]; if (!w) continue;
          const pct = w.pct;
          const fillW = pct != null ? Math.min(100, pct) : 8;  // neutral sliver when no cap
          const c = usageColor(pct);
          const capTxt = w.cap ? ` / ${fmtTok(w.cap)} (${pct}%)` : "";
          const reset = w.seconds_until > 0 ? `resets ${fmtCountdown(w.seconds_until)}` : "";
          rows.push(`<div class="bucket-row">
            <div class="bucket-label">${win}</div>
            <div class="bucket-track">
              <div class="bucket-fill" style="width:${fillW}%;background:${c}"></div>
              <span class="bucket-pct">${fmtTok(w.used)}${capTxt}</span>
            </div>
            <div class="bucket-counts muted">${reset}</div>
          </div>`);
        }
      }
      el.innerHTML = rows.join("");
    }
    function pollAgentUsage() {
      fetch("/api/agent_usage", { cache: "no-store" })
        .then(r => r.json()).then(renderAgentUsage).catch(() => {});
    }
    // ---- per-function token spend table (Phase 2) ---------------------------
    const FN_TOK_COLORS = { codex: "#a98ee6", claude: "#38b995", glm: "#5c91df", other: "#8da0b8" };
    function renderFnTokens(d) {
      const el = $("fn-tokens-body"); if (!el) return;
      const fns = (d && d.fns) || [];
      setText($("fn-tokens-note"), d && d.available ? `${d.count} fns tracked` : "no ledger yet");
      if (!fns.length) {
        el.innerHTML = '<div class="bucket-counts">no per-function token data yet — pane_io logs a row as each task completes</div>';
        return;
      }
      const cell = (bp, p) => {
        const v = bp[p] || 0;
        return `<td class="mono" style="text-align:right;color:${v ? FN_TOK_COLORS[p] : '#566'}">${v ? fmtTok(v) : '·'}</td>`;
      };
      let h = '<table style="width:100%;border-collapse:collapse;font-size:12px">'
        + '<thead><tr style="text-align:left;color:#8da0b8">'
        + '<th style="padding:3px 6px">fn</th><th>file</th>'
        + '<th style="text-align:right">codex</th><th style="text-align:right">claude</th>'
        + '<th style="text-align:right">glm</th><th style="text-align:right">total</th>'
        + '<th style="text-align:right">tasks</th></tr></thead><tbody>';
      for (const f of fns) {
        const bp = f.by_provider || {};
        const file = (f.file || "").split("/").pop();
        h += `<tr class="fn-tok-row" data-fn="${f.fn}" style="cursor:pointer;border-top:1px solid #1f2733">`
          + `<td class="mono" style="padding:3px 6px">${f.fn}</td>`
          + `<td class="muted" title="${f.file || ''}">${file}</td>`
          + cell(bp, "codex") + cell(bp, "claude") + cell(bp, "glm")
          + `<td class="mono" style="text-align:right;font-weight:600">${fmtTok(f.total)}</td>`
          + `<td class="mono" style="text-align:right">${f.tasks}</td></tr>`
          + `<tr class="fn-tok-detail" data-detail="${f.fn}" style="display:none">`
          + `<td colspan="7" style="padding:2px 6px 8px"><span class="muted">loading…</span></td></tr>`;
      }
      h += "</tbody></table>";
      el.innerHTML = h;
      el.querySelectorAll(".fn-tok-row").forEach(tr =>
        tr.addEventListener("click", () => toggleFnTokDetail(tr.getAttribute("data-fn"))));
    }
    function toggleFnTokDetail(fn) {
      const det = document.querySelector(`.fn-tok-detail[data-detail="${fn}"]`); if (!det) return;
      if (det.style.display !== "none") { det.style.display = "none"; return; }
      det.style.display = "";
      fetch(`/api/history/fn_tokens?name=${encodeURIComponent(fn)}`, { cache: "no-store" })
        .then(r => r.json()).then(d => {
          const rows = (d && d.rows) || [];
          const td = det.querySelector("td");
          if (!rows.length) { td.innerHTML = '<span class="muted">no history</span>'; return; }
          td.innerHTML = rows.map(r => {
            const t = r.unix ? new Date(r.unix * 1000).toLocaleString() : "?";
            const srcTag = r.src === "footer" ? ' <span style="color:#7a5">~footer</span>' : "";
            return `<span class="muted" style="margin-right:14px">${t} · ${r.provider} · in ${fmtTok(r.in)} / out ${fmtTok(r.out)}${srcTag}</span>`;
          }).join("");
        }).catch(() => {});
    }
    function pollFnTokens() {
      fetch("/api/fn_tokens", { cache: "no-store" })
        .then(r => r.json()).then(renderFnTokens).catch(() => {});
    }
    // ---- v10: token-expense STACKED bar chart, per-source colors ------------
    const TOKEN_SOURCES = [
      { key: "claude", label: "Claude", color: "#38b995" },
      { key: "opencode", label: "OpenCode", color: "#5c91df" },
      { key: "codex", label: "Codex", color: "#a98ee6" }
    ];
    const fmtTok = n => {
      n = Number(n || 0);
      return n >= 1e9 ? (n / 1e9).toFixed(2) + "B"
        : n >= 1e6 ? (n / 1e6).toFixed(1) + "M"
        : n >= 1e3 ? Math.round(n / 1e3) + "k" : String(Math.round(n));
    };
    function drawTokens(canvas, payload) {
      store._tokensPayload = payload || {};
      const { ctx, w, h } = fitCanvas(canvas);
      ctx.clearRect(0, 0, w, h);
      const buckets = (payload && payload.buckets) || [];
      if (!buckets.length) {
        return drawEmpty(ctx, w, h, payload && payload.reason ? payload.reason : "No token usage in window");
      }
      const srcVal = (b, key) => key === "codex"
        ? Number(b.codex || 0)
        : Number((b.by_source && b.by_source[key]) || b[key] || 0);
      const pad = { l: 54, r: 16, t: 22, b: 30 };
      const xs = buckets.map(b => Number(b.unix || 0));
      const maxTotal = Math.max(...buckets.map(b => TOKEN_SOURCES.reduce((s, src) => s + srcVal(b, src.key), 0)), 1);
      const maxEvents = Math.max(...buckets.map(b => Number(b.codex_events || 0)), 1);
      const minX = Math.min(...xs), maxX = Math.max(...xs);
      const x = v => pad.l + (w - pad.l - pad.r) * (v - minX) / Math.max(1, maxX - minX);
      const plotH = h - pad.t - pad.b;
      const y = v => h - pad.b - plotH * v / maxTotal;
      ctx.lineWidth = 1;
      for (let i = 0; i <= 4; i++) {
        const v = maxTotal * i / 4;
        const gy = y(v);
        ctx.strokeStyle = i === 0 ? "#3a4a5e" : "#2d3a4b";
        ctx.beginPath(); ctx.moveTo(pad.l, gy); ctx.lineTo(w - pad.r, gy); ctx.stroke();
        ctx.fillStyle = "#8da0b8"; ctx.font = "11px Segoe UI, Arial"; ctx.textAlign = "right";
        ctx.fillText(fmtTok(v), pad.l - 6, gy + 4);
      }
      // stacked bars per source
      const n = buckets.length;
      const slot = (w - pad.l - pad.r) / Math.max(1, n);
      const bw = Math.max(1, Math.min(22, slot * 0.78));
      buckets.forEach(b => {
        const cx = x(Number(b.unix));
        let base = h - pad.b;
        for (const src of TOKEN_SOURCES) {
          const segH = plotH * srcVal(b, src.key) / maxTotal;
          if (segH > 0) {
            ctx.fillStyle = src.color;
            ctx.fillRect(cx - bw / 2, base - segH, bw, segH);
            base -= segH;
          }
        }
        // codex prompt activity: a small tick marker (scaled to its own max) above axis
        const ev = Number(b.codex_events || 0);
        if (ev > 0) {
          ctx.fillStyle = "#cab8ff";
          const ty = h - pad.b - 2;
          const tickH = 4 + 8 * ev / maxEvents;
          ctx.fillRect(cx - 1.5, ty - tickH, 3, tickH);
        }
      });
      // legend
      ctx.textAlign = "left"; ctx.font = "12px Segoe UI, Arial";
      let lx = pad.l;
      for (const src of TOKEN_SOURCES) {
        ctx.fillStyle = src.color; ctx.fillRect(lx, pad.t - 14, 9, 9);
        ctx.fillStyle = "#cbd5e3"; ctx.fillText(src.label, lx + 13, pad.t - 5);
        lx += ctx.measureText(src.label).width + 30;
      }
      ctx.fillStyle = "#cab8ff"; ctx.fillRect(lx, pad.t - 14, 9, 9);
      ctx.fillStyle = "#cbd5e3"; ctx.fillText("Codex events", lx + 13, pad.t - 5);
      // x labels
      ctx.fillStyle = "#7c8aa0"; ctx.font = "11px Segoe UI, Arial"; ctx.textAlign = "center";
      const dateLbl = ux => new Date(Number(ux) * 1000).toLocaleString("en-US", { timeZone: "Pacific/Honolulu", month: "numeric", day: "numeric", hour: "2-digit", hour12: false });
      const step = Math.max(1, Math.ceil(n / 8));
      for (let i = 0; i < n; i += step) ctx.fillText(dateLbl(buckets[i].unix), x(Number(buckets[i].unix)), h - 8);
    }
    function renderTokenStats(payload) {
      const strip = $("tokens-stats");
      if (!strip) return;
      strip.replaceChildren();
      const totals = (payload && payload.totals) || {};
      const items = [
        ["Claude", fmtTok(totals.claude), "#38b995"],
        ["OpenCode", fmtTok(totals.opencode), "#5c91df"],
        ["Codex", fmtTok(totals.codex), "#a98ee6"],
        ["All tokens", fmtTok(totals.all_tokens), "#eef4fb"]
      ];
      if (totals.codex_events) items.push(["Codex events", String(totals.codex_events || 0), "#cab8ff"]);
      if (totals.agent_tokens_cumulative) items.push(["Agent (cumulative)", fmtTok(totals.agent_tokens_cumulative), "#8da0b8"]);
      for (const [label, value, color] of items) {
        const box = document.createElement("div");
        box.className = "token-stat";
        const v = document.createElement("div");
        v.className = "token-stat-value";
        v.style.color = color;
        setText(v, value);
        const k = document.createElement("div");
        k.className = "token-stat-label";
        setText(k, label);
        box.append(v, k);
        strip.append(box);
      }
      const modelStrip = document.createElement("div");
      modelStrip.className = "model-strip";
      modelStrip.style.flexBasis = "100%";
      for (const row of ((payload && payload.by_model) || []).slice(0, 10)) {
        const chip = document.createElement("span");
        chip.className = "model-chip";
        const model = row.model || "?";
        if (String(model).toLowerCase().includes("claude")) chip.style.borderColor = "rgba(56,185,149,.7)";
        if (String(model).toLowerCase().includes("glm")) chip.style.borderColor = "rgba(92,145,223,.75)";
        if (String(model).toLowerCase().includes("gpt")) chip.style.borderColor = "rgba(169,142,230,.75)";
        setText(chip, `${model}: ${fmtTok(row.tokens)}`);
        modelStrip.append(chip);
      }
      if (modelStrip.children.length) strip.append(modelStrip);
    }
    function pollTokens() {
      // Default window 168h (7d); widen progressively if recent buckets empty so
      // the chart is never silently blank when data exists further back.
      const tryHours = [168, 720, 2160, 4320, 8760];
      let i = 0;
      const attempt = () => {
        fetch(`/api/tokens?hours=${tryHours[i]}`, { cache: "no-store" })
          .then(r => r.json())
          .then(payload => {
            if (payload && payload.available && (payload.buckets || []).length) {
              drawTokens($("tokens-chart"), payload);
              renderTokenStats(payload);
              const t = payload.totals || {};
              setText($("tokens-note"), `${fmtTok(t.all_tokens)} tokens / ${payload.buckets.length} hr-buckets (${tryHours[i]}h)`);
            } else if (i < tryHours.length - 1) {
              i++; attempt();
            } else {
              drawTokens($("tokens-chart"), payload);
              renderTokenStats(payload);
              setText($("tokens-note"), (payload && payload.reason) || "no token data found");
            }
          })
          .catch(() => { setText($("tokens-note"), "token source unavailable"); });
      };
      attempt();
    }
    function drawKgGraph(canvas, kg) {
      const { ctx, w, h } = fitCanvas(canvas);
      ctx.clearRect(0, 0, w, h);
      if (!kg || !kg.available) return drawEmpty(ctx, w, h, kg && kg.error ? kg.error : "Knowledge graph unavailable");
      const c = kg.counts || {};
      const rel = kg.relationship_graph || {};
      const relNodes = (rel.nodes || []).slice(0, 36);
      const relEdges = (rel.edges || []).slice(0, 72);
      if (relNodes.length) {
        const pos = {};
        const classes = relNodes.filter(n => n.kind === "class").slice(0, 10);
        const fns = relNodes.filter(n => n.kind !== "class").slice(0, 26);
        classes.forEach((n, i) => {
          const step = h / Math.max(2, classes.length + 1);
          pos[n.id] = { x: w * .20, y: step * (i + 1), node: n };
        });
        fns.forEach((n, i) => {
          const angle = (Math.PI * 2 * i) / Math.max(1, fns.length);
          const rx = Math.max(80, w * .25);
          const ry = Math.max(55, h * .30);
          pos[n.id] = {
            x: w * .64 + Math.cos(angle) * rx,
            y: h * .52 + Math.sin(angle) * ry,
            node: n
          };
        });
        ctx.lineWidth = 1.2;
        for (const e of relEdges) {
          const a = pos[e.from], b = pos[e.to];
          if (!a || !b) continue;
          const isCall = e.kind === "calls";
          ctx.strokeStyle = isCall ? "rgba(92,145,223,.58)" : "rgba(56,185,149,.32)";
          if (e.confidence === "medium") ctx.strokeStyle = isCall ? "rgba(240,179,90,.55)" : "rgba(56,185,149,.24)";
          ctx.setLineDash(isCall ? [] : [4, 4]);
          ctx.beginPath();
          ctx.moveTo(a.x, a.y);
          const mx = (a.x + b.x) / 2;
          const my = (a.y + b.y) / 2 - (isCall ? 12 : 0);
          ctx.quadraticCurveTo(mx, my, b.x, b.y);
          ctx.stroke();
        }
        ctx.setLineDash([]);
        for (const item of Object.values(pos)) {
          const n = item.node;
          const isClass = n.kind === "class";
          const isNamed = n.kind === "named";
          const color = isClass ? "#38b995" : (isNamed ? "#a98ee6" : "#5c91df");
          if (isClass) {
            ctx.fillStyle = "#101824";
            ctx.strokeStyle = color;
            ctx.lineWidth = 1.8;
            ctx.beginPath();
            if (ctx.roundRect) ctx.roundRect(item.x - 58, item.y - 15, 116, 30, 6);
            else ctx.rect(item.x - 58, item.y - 15, 116, 30);
            ctx.fill();
            ctx.stroke();
          } else {
            ctx.fillStyle = "#101824";
            ctx.strokeStyle = color;
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.arc(item.x, item.y, isNamed ? 12 : 10, 0, Math.PI * 2);
            ctx.fill();
            ctx.stroke();
          }
          ctx.fillStyle = "#dfe8f4";
          ctx.textAlign = "center";
          ctx.font = isClass ? "10px Segoe UI, Arial" : "9.5px Consolas, 'Courier New', monospace";
          ctx.fillText(n.label || n.id, item.x, item.y + (isClass ? 4 : 24), isClass ? 104 : 92);
        }
        ctx.fillStyle = "#8da0b8";
        ctx.textAlign = "left";
        ctx.font = "11px Segoe UI, Arial";
        ctx.fillText(`${relNodes.length} nodes / ${relEdges.length} edges`, 10, h - 12);
        return;
      }
      const cx = w / 2, cy = h / 2;
      const nodes = [
        { id: "functions", label: "functions", value: c.functions || 0, x: cx, y: cy, r: 34, color: "#5c91df" },
        { id: "levers", label: "levers", value: c.levers || 0, x: w * .24, y: h * .30, r: 25, color: "#38b995" },
        { id: "walls", label: "walls", value: c.walls || 0, x: w * .76, y: h * .30, r: 24, color: "#e07171" },
        { id: "near", label: "near", value: c.near || 0, x: w * .25, y: h * .74, r: 27, color: "#f0b35a" },
        { id: "externals", label: "external tech", value: c.externals || 0, x: w * .76, y: h * .74, r: 24, color: "#a98ee6" },
        { id: "cracks", label: "cracks", value: c.cracked_edges || 0, x: cx, y: h * .16, r: 22, color: "#8da0b8" }
      ];
      const byId = Object.fromEntries(nodes.map(n => [n.id, n]));
      const edges = [
        ["levers", "functions"], ["cracks", "functions"], ["walls", "functions"],
        ["near", "functions"], ["externals", "levers"], ["externals", "cracks"],
        ["near", "levers"], ["walls", "near"]
      ];
      ctx.lineWidth = 1.4;
      for (const [a, b] of edges) {
        const A = byId[a], B = byId[b];
        const grad = ctx.createLinearGradient(A.x, A.y, B.x, B.y);
        grad.addColorStop(0, A.color);
        grad.addColorStop(1, B.color);
        ctx.strokeStyle = grad;
        ctx.globalAlpha = .45;
        ctx.beginPath();
        ctx.moveTo(A.x, A.y);
        const mx = (A.x + B.x) / 2;
        const my = (A.y + B.y) / 2 - 18;
        ctx.quadraticCurveTo(mx, my, B.x, B.y);
        ctx.stroke();
      }
      ctx.globalAlpha = 1;
      for (const n of nodes) {
        const glow = ctx.createRadialGradient(n.x, n.y, 2, n.x, n.y, n.r * 1.9);
        glow.addColorStop(0, n.color + "aa");
        glow.addColorStop(1, "rgba(0,0,0,0)");
        ctx.fillStyle = glow;
        ctx.beginPath();
        ctx.arc(n.x, n.y, n.r * 1.9, 0, Math.PI * 2);
        ctx.fill();
        ctx.fillStyle = "#101824";
        ctx.strokeStyle = n.color;
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.arc(n.x, n.y, n.r, 0, Math.PI * 2);
        ctx.fill();
        ctx.stroke();
        ctx.fillStyle = "#eef4fb";
        ctx.textAlign = "center";
        ctx.font = "800 15px Consolas, 'Courier New', monospace";
        ctx.fillText(Number(n.value || 0).toLocaleString(), n.x, n.y - 2);
        ctx.fillStyle = "#a8b4c4";
        ctx.font = "10px Segoe UI, Arial";
        ctx.fillText(n.label, n.x, n.y + 14);
      }
    }
    function renderKg(kg) {
      store._kgPayload = kg || {};
      drawKgGraph($("kg-graph"), store._kgPayload);
      setText($("kg-note"), kg && kg.available ? `kg.db updated ${kg.generated_at}` : "kg.db unavailable");
      const stats = $("kg-stats");
      const levers = $("kg-levers");
      const targets = $("kg-targets");
      const tags = $("kg-tags");
      const edges = $("kg-edges");
      if (!stats || !levers || !targets) return;
      stats.replaceChildren(); levers.replaceChildren(); targets.replaceChildren();
      if (tags) tags.replaceChildren();
      if (edges) edges.replaceChildren();
      const c = (kg && kg.counts) || {};
      for (const [label, value] of [
        ["functions", c.functions || 0], ["near", c.near || 0], ["cracks", c.cracked_edges || 0],
        ["calls", c.calls || 0], ["tags", c.calltags || 0], ["evidence", c.name_evidence || 0],
        ["levers", c.levers || 0], ["walls", c.walls || 0], ["external", c.externals || 0]
      ]) {
        const box = document.createElement("div");
        box.className = "kg-stat";
        const v = document.createElement("b");
        setText(v, Number(value || 0).toLocaleString());
        const k = document.createElement("span");
        setText(k, label);
        box.append(v, k);
        stats.append(box);
      }
      for (const row of ((kg && kg.top_levers) || []).slice(0, 8)) {
        const el = document.createElement("div");
        el.className = "kg-row";
        const a = document.createElement("span");
        a.className = "mono";
        setText(a, row.slug || row.title || "");
        const b = document.createElement("span");
        setText(b, `${row.cracks || 0}`);
        el.append(a, b);
        levers.append(el);
      }
      if (tags) {
        const tagRows = ((kg && kg.calltags) || []).slice(0, 6);
        for (const row of tagRows) {
          const el = document.createElement("div");
          el.className = "kg-row";
          const a = document.createElement("span");
          a.className = "mono";
          setText(a, `${row.tag || ""} ${row.confidence || ""}`);
          const b = document.createElement("span");
          setText(b, `${row.functions || 0} @ ${row.avg_score || 0}`);
          el.append(a, b);
          tags.append(el);
        }
        for (const row of ((kg && kg.name_evidence) || []).slice(0, Math.max(0, 8 - tagRows.length))) {
          const el = document.createElement("div");
          el.className = "kg-row";
          const a = document.createElement("span");
          a.className = "mono";
          setText(a, `${row.symbol || ""} -> ${row.candidate || ""}`);
          const b = document.createElement("span");
          setText(b, `${row.score || 0}`);
          el.append(a, b);
          tags.append(el);
        }
      }
      for (const row of ((kg && kg.targets) || []).slice(0, 10)) {
        const el = document.createElement("div");
        el.className = "kg-row";
        const a = document.createElement("span");
        a.className = "mono";
        setText(a, `${row.addr} ${fileName(row.tu || "")}`);
        const b = document.createElement("span");
        setText(b, pctText(row.pct));
        el.append(a, b);
        targets.append(el);
      }
      if (edges) {
        for (const row of ((kg && kg.call_edges) || []).slice(0, 10)) {
          const el = document.createElement("div");
          el.className = "kg-row";
          const a = document.createElement("span");
          a.className = "mono";
          setText(a, `${row.caller || ""} -> ${row.callee || ""}`);
          const b = document.createElement("span");
          setText(b, row.confidence || "");
          el.append(a, b);
          edges.append(el);
        }
      }
      setText($("kg-target-note"), `${((kg && kg.targets) || []).length} targets`);
    }
    function pollKg() {
      fetch("/api/kg", { cache: "no-store" })
        .then(r => r.json()).then(renderKg).catch(() => renderKg({ available: false, error: "kg endpoint unavailable" }));
    }
    function miniBar(pct) {
      const wrap = document.createElement("span");
      wrap.className = "mini-bar";
      const fill = document.createElement("span");
      fill.style.width = `${Math.max(0, Math.min(100, Number(pct || 0)))}%`;
      wrap.append(fill);
      return wrap;
    }
    function numCell(value, extra) {
      const cell = document.createElement("td");
      cell.className = "num";
      setText(cell, value);
      if (extra) cell.append(extra);
      return cell;
    }
    function renderFilesTable(data) {
      const body = $("files-body");
      const foot = $("files-foot");
      body.replaceChildren();
      foot.replaceChildren();
      const decomp = data.decomp || {};
      let units = (decomp.units || []).slice();
      const q = $("files-query").value.trim().toLowerCase();
      const incompleteOnly = $("files-incomplete").checked;
      units = units.filter(u => {
        if (q && !`${u.source} ${u.name}`.toLowerCase().includes(q)) return false;
        if (incompleteOnly && u.complete) return false;
        return true;
      });
      const key = store.filesSort;
      const dir = store.filesDir;
      const numKeys = new Set(["total_code", "matched_code", "code_pct", "total_functions", "matched_functions", "functions_pct", "fuzzy_pct"]);
      units.sort((a, b) => {
        let av, bv;
        if (key === "source") { av = String(a.source || ""); bv = String(b.source || ""); }
        else if (key === "complete") { av = a.complete ? 1 : 0; bv = b.complete ? 1 : 0; }
        else { av = Number(a[key] || 0); bv = Number(b[key] || 0); }
        if (av < bv) return -1 * dir;
        if (av > bv) return 1 * dir;
        return 0;
      });
      setText($("files-note"), decomp.available ? `${units.length}/${(decomp.units || []).length} units` : "report.json not available");
      if (!units.length) {
        const tr = document.createElement("tr");
        const cell = document.createElement("td");
        cell.colSpan = 9;
        cell.className = "empty-state";
        setText(cell, decomp.available ? "No units match the current filter" : "No report.json decomp metrics found");
        tr.append(cell);
        body.append(tr);
        return;
      }
      let sumCode = 0, sumMatchedCode = 0, sumFns = 0, sumMatchedFns = 0;
      for (const u of units) {
        sumCode += Number(u.total_code || 0);
        sumMatchedCode += Number(u.matched_code || 0);
        sumFns += Number(u.total_functions || 0);
        sumMatchedFns += Number(u.matched_functions || 0);
        const tr = document.createElement("tr");
        tr.style.cursor = "pointer";
        const src = document.createElement("td");
        src.className = "wrap mono";
        setText(src, u.source || u.name || "unknown");
        tr.append(
          src,
          numCell(Number(u.total_code || 0).toLocaleString()),
          numCell(Number(u.matched_code || 0).toLocaleString()),
          numCell(pctText(u.code_pct), miniBar(u.code_pct)),
          numCell(Number(u.total_functions || 0).toLocaleString()),
          numCell(Number(u.matched_functions || 0).toLocaleString()),
          numCell(pctText(u.functions_pct), miniBar(u.functions_pct)),
          numCell(pctText(u.fuzzy_pct)),
          td(u.complete ? "yes" : "-")
        );
        tr.addEventListener("click", () => {
          switchView("decomp");
          gotoFiles();
          enterUnit(u);
          document.querySelector(".decomp-workspace").scrollIntoView({ behavior: "smooth", block: "start" });
        });
        body.append(tr);
      }
      const codePct = sumCode ? (sumMatchedCode * 100 / sumCode) : 0;
      const fnPct = sumFns ? (sumMatchedFns * 100 / sumFns) : 0;
      foot.append(
        td(`TOTAL (${units.length})`),
        numCell(sumCode.toLocaleString()),
        numCell(sumMatchedCode.toLocaleString()),
        numCell(pctText(codePct)),
        numCell(sumFns.toLocaleString()),
        numCell(sumMatchedFns.toLocaleString()),
        numCell(pctText(fnPct)),
        numCell(pctText(decomp.fuzzy_pct)),
        td("")
      );
    }
    function switchView(view) {
      store.activeView = view;
      if (view !== "decomp") closePin();
      document.querySelectorAll(".tab-btn").forEach(button => {
        button.classList.toggle("active", button.dataset.view === view);
      });
      document.querySelectorAll(".view").forEach(section => {
        section.classList.toggle("active", section.id === `view-${view}`);
      });
      const labels = { symbols: "GC6E01/SYMBOLMAP", files: "GC6E01/FILES", decomp: "GC6E01/DECOMP" };
      setText($("hud-project"), labels[view] || "GC6E01/DECOMP");
      if (location.hash !== `#${view}`) {
        history.replaceState(null, "", `#${view}`);
      }
      if (store.data) {
        renderCharts(store.data);
        if (view === "decomp") {
          // Canvas had zero size while hidden; re-layout now that it is visible.
          renderTreemap();
          renderDecompDetail(store.data);
          // The token canvas also had zero size while hidden; redraw on show.
          pollTokens();
          drawKgGraph($("kg-graph"), store._kgPayload || {});
          tickLimits();
        } else if (view === "files") {
          renderFilesTable(store.data);
        }
      }
    }
    function scheduleRefresh() {
      if (store.refreshTimer) {
        clearInterval(store.refreshTimer);
        store.refreshTimer = null;
      }
      if ($("auto-refresh").checked) {
        store.refreshTimer = setInterval(refresh, Number($("refresh-rate").value || 5000));
      }
    }
    async function refresh() {
      let data;
      try {
        const response = await fetch("/api/state", { cache: "no-store" });
        data = await response.json();
      } catch (err) {
        // /api/state is a slow single-threaded rebuild; a transient reset under
        // concurrent polling must not throw an unhandled rejection. Skip this
        // tick — the live attempt-log / token / agent polls keep the UI moving.
        return;
      }
      store.data = data;
      store.rows = data.targets || [];
      renderHud(data);
      renderMetrics(data);
      renderAttackMatrix(data);
      renderTop(data);
      renderLegend(data);
      renderSourceFilters(store.rows);
      renderCharts(data);
      renderActivity(data);
      renderCommits(data);
      renderCrumbs();
      renderSymbolTreemap(data);
      renderRows();
      // switchView re-renders the active view's treemap/detail/files table with
      // correct canvas dimensions.
      switchView(store.activeView);
    }
    $("query").addEventListener("input", renderRows);
    $("status-filter").addEventListener("change", () => {
      store.attention = false;
      $("attention")?.classList.remove("primary");
      renderRows();
    });
    $("source-filter").addEventListener("change", renderRows);
    $("clear").addEventListener("click", () => {
      $("query").value = "";
      $("status-filter").value = "";
      $("source-filter").value = "";
      store.attention = false;
      $("attention")?.classList.remove("primary");
      renderRows();
    });
    // The "Needs Wiring" pill was removed from the top bar; guard the handler so
    // it is a no-op when the button is absent (status filter still works via the
    // Status Map / filter controls).
    $("attention")?.addEventListener("click", () => {
      store.attention = !store.attention;
      $("attention")?.classList.toggle("primary", store.attention);
      if (store.attention) $("status-filter").value = "";
      renderRows();
    });
    $("refresh").addEventListener("click", refresh);
    $("auto-refresh").addEventListener("change", scheduleRefresh);
    $("refresh-rate").addEventListener("change", scheduleRefresh);
    $("tu-query").addEventListener("input", () => {
      if (store.data) renderSymbolTreemap(store.data);
    });
    $("tu-needs").addEventListener("click", () => {
      store.tuNeedsOnly = !store.tuNeedsOnly;
      $("tu-needs").classList.toggle("primary", store.tuNeedsOnly);
      if (store.data) renderSymbolTreemap(store.data);
    });
    $("tu-clear").addEventListener("click", () => {
      $("tu-query").value = "";
      store.tuNeedsOnly = false;
      $("tu-needs").classList.remove("primary");
      if (store.data) renderSymbolTreemap(store.data);
    });
    $("decomp-query").addEventListener("input", () => {
      if (store.data) renderTreemap();
    });
    $("decomp-area-fns").addEventListener("change", () => {
      store.tm.areaByFns = $("decomp-area-fns").checked;
      if (store.data) renderTreemap();
    });
    $("decomp-difficulty").addEventListener("change", () => {
      store.tm.difficultyFilter = $("decomp-difficulty").value || "all";
      if (store.data) {
        renderDecompDetail(store.data);
        renderTreemap();
      }
    });
    $("decomp-near").addEventListener("click", () => {
      store.decompNearOnly = !store.decompNearOnly;
      $("decomp-near").classList.toggle("primary", store.decompNearOnly);
      if (store.data) renderTreemap();
    });
    $("decomp-clear").addEventListener("click", () => {
      $("decomp-query").value = "";
      $("decomp-difficulty").value = "all";
      store.tm.difficultyFilter = "all";
      store.decompNearOnly = false;
      $("decomp-near").classList.remove("primary");
      gotoFiles();
    });
    // Desktop hover tooltip stays exactly as before; on touch we suppress the
    // floating hover tip (it has no cursor to follow) and use the pinned panel.
    if (!store.isTouch) {
      $("decomp-treemap").addEventListener("mousemove", onTreemapMove);
      $("decomp-treemap").addEventListener("mouseleave", () => { $("decomp-tip").hidden = true; });
    }
    $("decomp-treemap").addEventListener("click", onTreemapClick);
    $("treemap-pin-open").addEventListener("click", () => {
      const it = store.pinnedItem;
      closePin();
      if (it) drillItem(it);
    });
    $("treemap-pin-close").addEventListener("click", closePin);
    // Re-pin position not needed (fixed panel); close pin when leaving decomp view
    // is handled by switchView. Tapping outside dismisses on touch.
    document.addEventListener("click", evt => {
      if (!store.isTouch || !store.pinnedItem) return;
      const pin = $("treemap-pin");
      const canvas = $("decomp-treemap");
      if (pin.contains(evt.target) || canvas.contains(evt.target)) return;
      closePin();
    });
    $("reader-back").addEventListener("click", () => {
      closeReader();
      document.querySelector(".decomp-workspace").scrollIntoView({ behavior: "smooth", block: "start" });
    });
    $("files-query").addEventListener("input", () => { if (store.data) renderFilesTable(store.data); });
    $("files-incomplete").addEventListener("change", () => { if (store.data) renderFilesTable(store.data); });
    $("files-clear").addEventListener("click", () => {
      $("files-query").value = "";
      $("files-incomplete").checked = false;
      if (store.data) renderFilesTable(store.data);
    });
    document.querySelectorAll("th[data-fsort]").forEach(th => {
      th.addEventListener("click", () => {
        const key = th.dataset.fsort;
        if (store.filesSort === key) store.filesDir *= -1;
        else { store.filesSort = key; store.filesDir = (key === "source" || key === "complete") ? 1 : -1; }
        if (store.data) renderFilesTable(store.data);
      });
    });
    document.querySelectorAll(".tab-btn").forEach(button => {
      button.addEventListener("click", () => switchView(button.dataset.view || "decomp"));
    });
    document.querySelectorAll("th[data-sort]").forEach(th => {
      th.addEventListener("click", () => {
        const key = th.dataset.sort;
        if (store.sortKey === key) store.sortDir *= -1;
        else {
          store.sortKey = key;
          store.sortDir = 1;
        }
        renderRows();
      });
    });
    window.addEventListener("resize", () => {
      if (store.data) {
        renderCharts(store.data);
        if (store.activeView === "decomp") renderTreemap();
      }
      if (store.activeView === "decomp") drawKgGraph($("kg-graph"), store._kgPayload || {});
    });
    if (location.hash === "#symbols") {
      store.activeView = "symbols";
    } else if (location.hash === "#files") {
      store.activeView = "files";
    }
    // v9: agent activity + lockout panels poll independently of the main refresh
    // (lighter endpoints, want a faster cadence). Token chart redraws on the
    // main refresh cadence and on view switch.
    pollAgents();
    pollLocks();
    pollLimits();
    pollAgentUsage();
    pollFnTokens();
    pollTokens();
    pollKg();
    pollCrackJobs();
    pollLog();   // populate the attempt log immediately (independent of /api/state)
    pollSync(); pollPrs(); pollShip(); pollLeases(); pollReports(); pollQuantum(); pollActiveWork(); pollBuckets();
    const _gcBtn = $("locks-gc");
    if (_gcBtn) _gcBtn.addEventListener("click", () => lockAction("gc", {}));

    // ---- orchestrator control wiring ----
    store.logFilter = { kind: "all", q: "", limit: 80 };
    store.historyDays = 14;
    store.reportFilter = "all";
    // Match-Progress time range buttons. Clicking a preset clears any manual
    // pan/zoom window so the preset takes effect (#2).
    $("history-range")?.addEventListener("click", e => {
      const b = e.target.closest("button[data-days]"); if (!b) return;
      store.historyDays = Number(b.dataset.days);
      store.histView = null;
      $("history-range").querySelectorAll("button").forEach(x => x.classList.toggle("active", x === b));
      drawHistoryRanged();
    });
    bindHistoryPanZoom();   // #2: drag-to-pan + wheel-zoom on the history chart
    // Leases active/queued tabs
    $("lease-tabs")?.addEventListener("click", e => {
      const b = e.target.closest("button[data-lease]"); if (!b) return;
      const which = b.dataset.lease;
      $("lease-tabs").querySelectorAll("button").forEach(x => x.classList.toggle("active", x === b));
      $("lease-active-pane").hidden = which !== "active";
      $("lease-queued-pane").hidden = which !== "queued";
    });
    // Comprehensive log filters
    const applyLog = () => {
      const data = store.data || store._logData;
      if (data) renderDecompAttemptLog(data, store._logUnit || null);
    };
    $("log-query")?.addEventListener("input", () => { store.logFilter.q = $("log-query").value.trim(); applyLog(); });
    $("log-limit")?.addEventListener("change", () => { store.logFilter.limit = Number($("log-limit").value) || 80; applyLog(); });
    $("log-kinds")?.addEventListener("click", e => {
      const b = e.target.closest("button[data-kind]"); if (!b) return;
      store.logFilter.kind = b.dataset.kind;
      $("log-kinds").querySelectorAll("button").forEach(x => x.classList.toggle("active", x === b));
      applyLog();
    });
    // Sync / PRs / Ship actions
    $("sync-fetch")?.addEventListener("click", () => {
      const b = $("sync-fetch"); const old = b.textContent; b.textContent = "Fetching…"; b.disabled = true;
      postJSON("/api/git/fetch", {}).then(r => { b.textContent = r && r.ok ? "Fetched ✓" : "Fetch failed"; })
        .catch(() => { b.textContent = "Fetch failed"; })
        .finally(() => setTimeout(() => { b.textContent = old; b.disabled = false; pollSync(); pollPrs(); }, 1500));
    });
    $("prs-refresh")?.addEventListener("click", () => pollPrs());
    $("ship-handoff")?.addEventListener("click", () => {
      if (!confirm("Push the current branch and open a PR for the confirmed wins?")) return;
      const b = $("ship-handoff"); const old = b.textContent; b.textContent = "Preparing…"; b.disabled = true;
      postJSON("/api/ship/prepare", {}).then(r => {
        if (r && r.ok) { b.textContent = "PR opened ✓"; if (r.url) window.open(r.url, "_blank"); }
        else { b.textContent = (r && r.error) ? "Failed" : "Failed"; alert((r && r.error) || "Handoff failed"); }
      }).catch(() => { b.textContent = "Failed"; })
        .finally(() => setTimeout(() => { b.textContent = old; b.disabled = false; pollShip(); pollPrs(); pollSync(); }, 2000));
    });
    // Pokédex dashboard link (separate server on its own port if running)
    const pdx = $("link-pokedex");
    if (pdx) pdx.href = `http://${location.hostname}:8793/`;

    store.agentsTimer = setInterval(pollAgents, 10000);
    store.locksTimer = setInterval(pollLocks, 10000);
    store.leasesTimer = setInterval(pollLeases, 8000);
    store.activeWorkTimer = setInterval(pollActiveWork, 12000);   // #3/#4 live active-work poll
    store.reportsTimer = setInterval(pollReports, 12000);
    store.bucketsTimer = setInterval(pollBuckets, 15000);
    store.syncTimer = setInterval(() => { pollSync(); pollShip(); }, 30000);
    store.prsTimer = setInterval(pollPrs, 60000);
    store.quantumTimer = setInterval(pollQuantum, 10000);
    store.limitsTimer = setInterval(pollLimits, 60000);
    store.agentUsageTimer = setInterval(pollAgentUsage, 60000);
    store.fnTokensTimer = setInterval(pollFnTokens, 30000);
    store.kgTimer = setInterval(pollKg, 30000);
    store.crackTimer = setInterval(pollCrackJobs, 15000);
    store.logTimer = setInterval(pollLog, 15000);   // live attempt-log refresh
    setInterval(tickLimits, 1000);   // smooth 1s countdown without refetching
    window.addEventListener("resize", () => {
      if (store.activeView === "decomp") drawTokens($("tokens-chart"), store._tokensPayload || {});
      if (store.activeView === "decomp") drawKgGraph($("kg-graph"), store._kgPayload || {});
    });
    refresh();
    scheduleRefresh();
  </script>
</body>
</html>
"""


def _fmt_bytes(n: int) -> str:
    if n >= 1 << 30:
        return f"{n / (1 << 30):.2f} GiB"
    if n >= 1 << 20:
        return f"{n / (1 << 20):.1f} MiB"
    if n >= 1 << 10:
        return f"{n / (1 << 10):.1f} KiB"
    return f"{n} B"


class Handler(BaseHTTPRequestHandler):
    def send_json(self, payload: object) -> None:
        body = json.dumps(payload, indent=2).encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    # ---- v10: private artifacts over Tailscale (user's own ROM-derived files)
    def send_file_stream(self, path: Path, content_type: str, filename: str) -> None:
        if not path.exists() or not path.is_file():
            self.send_json({"available": False, "error": f"not found: {path}"})
            return
        try:
            size = path.stat().st_size
            self.send_response(200)
            self.send_header("Content-Type", content_type)
            self.send_header("Content-Length", str(size))
            self.send_header("Content-Disposition", f'attachment; filename="{filename}"')
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            with open(path, "rb") as fh:
                while True:
                    chunk = fh.read(1 << 16)
                    if not chunk:
                        break
                    self.wfile.write(chunk)
        except (OSError, BrokenPipeError):
            return

    def send_target_objects_zip(self) -> None:
        if not TARGET_OBJ_DIR.exists():
            self.send_json({"available": False, "error": f"not found: {TARGET_OBJ_DIR}"})
            return
        buf = io.BytesIO()
        with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as zf:
            for obj in sorted(TARGET_OBJ_DIR.rglob("*")):
                if obj.is_file():
                    arc = "obj/" + str(obj.relative_to(TARGET_OBJ_DIR)).replace("\\", "/")
                    zf.write(obj, arc)
        body = buf.getvalue()
        self.send_response(200)
        self.send_header("Content-Type", "application/zip")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Content-Disposition", 'attachment; filename="target-objects.zip"')
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        try:
            self.wfile.write(body)
        except BrokenPipeError:
            return

    def send_artifacts_index(self) -> None:
        host = self.headers.get("Host", "127.0.0.1:8792")
        dol_size = MAIN_DOL.stat().st_size if MAIN_DOL.exists() else 0
        obj_count = sum(1 for p in TARGET_OBJ_DIR.rglob("*") if p.is_file()) if TARGET_OBJ_DIR.exists() else 0
        obj_size = sum(p.stat().st_size for p in TARGET_OBJ_DIR.rglob("*") if p.is_file()) if TARGET_OBJ_DIR.exists() else 0
        page = f"""<!doctype html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>GC6E01 private artifacts</title>
<style>body{{font-family:Consolas,monospace;background:#0d1118;color:#eef4fb;margin:0;padding:24px;line-height:1.5}}
h1{{font-size:18px}}a{{color:#76a9ff}}.card{{border:1px solid #2d3a4b;border-radius:8px;padding:14px;margin:12px 0;background:#151c28}}
code{{background:#0c131d;border:1px solid #253143;border-radius:5px;padding:2px 6px;display:inline-block;margin-top:6px}}
.muted{{color:#a8b4c4;font-size:13px}}</style></head><body>
<h1>GC6E01 private artifacts (Tailscale)</h1>
<p class="muted">ROM-derived, copyright-restricted inputs for the remote objdiff / asm-review workflow.
Serve only over the private tailnet. Not linked from the public dashboard UI.</p>
<div class="card"><b>main.dol</b> &mdash; {_fmt_bytes(dol_size)}<br>
<span class="muted">orig/GC6E01/sys/main.dol (the target ROM image)</span><br>
<code>curl -fL http://{host}/artifacts/main.dol -o orig/GC6E01/sys/main.dol</code></div>
<div class="card"><b>target-objects.zip</b> &mdash; {_fmt_bytes(obj_size)} ({obj_count} files)<br>
<span class="muted">build/GC6E01/obj/ &mdash; the per-TU target .o files objdiff diffs against</span><br>
<code>curl -fL http://{host}/artifacts/target-objects.zip -o /tmp/to.zip &amp;&amp; unzip -o /tmp/to.zip -d build/GC6E01/</code></div>
<div class="card"><b>fetch both</b><br>
<code>bash tools/decomp_work/fetch_artifacts.sh {host}</code></div>
<p class="muted">Server-side asm review (no download needed): <code>GET /api/asm?source=&lt;stem&gt;&amp;fn=&lt;fn_XXXX&gt;</code></p>
</body></html>"""
        body = page.encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _read_json_body(self) -> dict:
        try:
            n = int(self.headers.get("Content-Length", 0) or 0)
        except (TypeError, ValueError):
            n = 0
        if n <= 0:
            return {}
        raw = self.rfile.read(n)
        try:
            data = json.loads(raw.decode("utf-8") or "{}")
            return data if isinstance(data, dict) else {}
        except (ValueError, UnicodeDecodeError):
            return {}

    def do_POST(self) -> None:
        """Agent controls. Localhost-only mutating actions over the fleet-lock DB.
        All routes return {ok, ...}. Unknown routes -> 404 JSON."""
        parsed = urlparse(self.path)
        path = parsed.path
        # Git/ship actions (safe, read-ish): fetch origin, prepare a PR handoff.
        if path == "/api/git/fetch":
            try:
                _git("fetch", "origin", timeout=40)
                _CMD_CACHE.pop("sync", None); _CMD_CACHE.pop("prs", None)
                self.send_json({"ok": True})
            except Exception as exc:
                self.send_json({"ok": False, "error": str(exc)})
            return
        if path == "/api/ship/prepare":
            self.send_json(prepare_handoff())
            return
        if path == "/api/crack/enqueue":
            self.send_json(enqueue_crack_job(self._read_json_body()))
            return
        if path == "/api/crack/open-terminal":
            body = self._read_json_body()
            pane = str(body.get("pane") or "codex")
            self.send_json(open_tmux_window(pane))
            return
        if not path.startswith("/api/locks/"):
            self.send_response(404)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(b'{"ok": false, "error": "no such endpoint"}')
            return
        body = self._read_json_body()
        try:
            lk = locks_module()
        except Exception as exc:  # locks.py missing/broken — surface, don't crash
            self.send_json({"ok": False, "error": f"locks unavailable: {exc}"})
            return
        agent = str(body.get("agent") or "dashboard")
        scope = str(body.get("scope") or "fn")
        key = str(body.get("key") or "")
        try:
            if path == "/api/locks/release":
                r = lk.release(agent, key, scope=scope, force=bool(body.get("force")))
            elif path == "/api/locks/acquire":
                r = lk.acquire(agent, key, scope=scope,
                               ttl=int(body.get("ttl", lk.DEFAULT_TTL)),
                               file=body.get("file"), note=body.get("note"))
            elif path == "/api/locks/renew":
                r = lk.renew(agent, key=key or None, scope=(scope if key else None),
                             ttl=int(body.get("ttl", lk.DEFAULT_TTL)))
            elif path == "/api/locks/gc":
                r = {"ok": True, "purged": lk.gc()}
            else:
                self.send_response(404)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(b'{"ok": false, "error": "no such lock action"}')
                return
        except Exception as exc:
            self.send_json({"ok": False, "error": str(exc)})
            return
        self.send_json(r)

    def do_GET(self) -> None:
        parsed = urlparse(self.path)
        path = parsed.path
        query = parse_qs(parsed.query)
        if path == "/api/state":
            state = get_state()
            state["history"] = update_history(state)
            decomp = state.get("decomp", {})
            if isinstance(decomp, dict):
                update_unit_history(decomp)
                update_fn_history(decomp)
            update_fn_token_history()  # fold in any new per-task token ledger rows
            self.send_json(state)
            return
        if path == "/api/unit":
            source = (query.get("source") or query.get("name") or [""])[0]
            self.send_json(load_unit_functions(source))
            return
        if path == "/api/history":
            self.send_json(load_history())
            return
        if path == "/api/history/commits":
            # Milestone-spaced view: one point per commit that moved a %, not the
            # per-minute ring. Backs the "match progress over time" chart (#3).
            self.send_json(collapse_history_to_commits())
            return
        if path == "/api/history/unit":
            source = (query.get("source") or query.get("name") or [""])[0]
            self.send_json(load_unit_history(source))
            return
        if path == "/api/history/fn":
            name = (query.get("name") or [""])[0]
            self.send_json(load_fn_history(name))
            return
        if path == "/api/history/fn_tokens":
            name = (query.get("name") or [""])[0]
            self.send_json(load_fn_token_history(name))
            return
        if path == "/api/fn_tokens":
            self.send_json(load_fn_token_summary())
            return
        if path == "/api/asm":
            source = (query.get("source") or [""])[0]
            fn = (query.get("fn") or [""])[0]
            if not source or not fn:
                self.send_json({"available": False, "error": "source and fn are required"})
                return
            self.send_json(compute_asm_diff(source, fn))
            return
        if path == "/api/fninfo":
            fn = (query.get("fn") or [""])[0]
            self.send_json(load_fn_info(fn))
            return
        if path == "/api/agents":
            self.send_json(load_agents())
            return
        if path == "/api/locks":
            self.send_json(load_locks())
            return
        if path == "/api/leases":
            self.send_json(load_leases())
            return
        if path == "/api/active":
            # #3/#4: unified active-work view (locks + scratch sidecars + commits)
            # with live per-file match% from report.json.
            self.send_json(load_active_work())
            return
        if path == "/api/reports":
            self.send_json(load_reports())
            return
        if path == "/api/sync":
            self.send_json(load_sync())
            return
        if path == "/api/prs":
            self.send_json(load_prs())
            return
        if path == "/api/ship":
            self.send_json(load_ship())
            return
        if path == "/api/quantum":
            self.send_json(load_quantum())
            return
        if path == "/api/buckets":
            self.send_json(load_buckets())
            return
        if path == "/api/tokens":
            try:
                hours = int((query.get("hours") or ["168"])[0])
            except (TypeError, ValueError):
                hours = 168
            self.send_json(load_tokens(max(1, min(hours, 24 * 30))))
            return
        if path == "/api/agent_usage":
            self.send_json(load_agent_usage())
            return
        if path == "/api/log":
            raw = (query.get("limit") or ["1000"])[0]
            if str(raw).lower() == "all":
                limit = None
            else:
                try:
                    limit = max(1, min(int(raw), 100000))
                except (TypeError, ValueError):
                    limit = 1000
            merged = merged_attempt_log(limit=limit)
            self.send_json({
                "available": bool(merged),
                "limit": raw,
                "count": len(merged),
                "attempt_log": merged,
                "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
            })
            return
        if path == "/api/kg":
            self.send_json(load_kg())
            return
        if path == "/api/hardest":
            data = load_hard_targets()
            entries = data.get("entries") if isinstance(data, dict) else []
            self.send_json({
                "available": bool(data.get("available")) if isinstance(data, dict) else False,
                "source": data.get("source") if isinstance(data, dict) else "",
                "counts": data.get("counts") if isinstance(data, dict) else {},
                "entries": entries[:300] if isinstance(entries, list) else [],
                "generated_at": time.strftime("%Y-%m-%d %H:%M:%S"),
            })
            return
        if path == "/api/crack/jobs":
            raw = (query.get("limit") or ["80"])[0]
            try:
                limit = max(1, min(int(raw), 500))
            except (TypeError, ValueError):
                limit = 80
            self.send_json(load_crack_jobs(limit=limit))
            return
        if path == "/api/crack/terminal":
            pane = (query.get("pane") or ["codex"])[0]
            try:
                lines = int((query.get("lines") or ["100"])[0])
            except (TypeError, ValueError):
                lines = 100
            self.send_json(capture_tmux_pane(pane, lines))
            return
        if path == "/artifacts":
            self.send_artifacts_index()
            return
        if path == "/artifacts/main.dol":
            self.send_file_stream(MAIN_DOL, "application/octet-stream", "main.dol")
            return
        if path == "/artifacts/target-objects.zip":
            self.send_target_objects_zip()
            return
        if path == "/api/limits":
            self.send_json(load_limits())
            return
        if path == "/api/health":
            self.send_json({"ok": True, "version": DASHBOARD_VERSION})
            return
        if path == "/favicon.ico":
            self.send_response(204)
            self.end_headers()
            return
        body = HTML.encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt: str, *args: object) -> None:
        return


# ---- auto-refresh: periodically regenerate report.json so progress-over-time is
# captured automatically (the dashboard runs the "progress check" itself). --------
REPORT_REFRESH_SECONDS = int(os.environ.get("DASH_REPORT_REFRESH", "600"))  # 0 = off
GEN_REPORT = ROOT / "tools" / "gen_decomp_report.py"
STATUS_LOG = ROOT / "tools" / "decomp_work" / "coordination" / "status.md"
_auto_state = {"last_matched": None}


def _report_matched():
    try:
        d = json.loads(DECOMP_REPORT.read_text(encoding="utf-8", errors="replace"))
        return int(d.get("measures", d).get("matched_functions"))
    except Exception:
        return None


def _report_units():
    """{unit_name: (matched, total)} from report.json -> per-file attempt-log attribution."""
    out = {}
    try:
        d = json.loads(DECOMP_REPORT.read_text(encoding="utf-8", errors="replace"))
        for u in d.get("units", []):
            m = u.get("measures", {})
            out[u.get("name", "")] = (int(m.get("matched_functions", 0)),
                                      int(m.get("total_functions", 0)))
    except Exception:
        pass
    return out


def _refresh_report_once() -> bool:
    """Regenerate report.json atomically via gen_decomp_report.py."""
    if not GEN_REPORT.exists():
        return False
    tmp = DECOMP_REPORT.with_name(DECOMP_REPORT.name + ".auto.tmp")
    try:
        proc = subprocess.run(
            [sys.executable, str(GEN_REPORT), "-o", str(tmp)],
            cwd=str(ROOT), capture_output=True, text=True, timeout=1200,
        )
        if proc.returncode == 0 and tmp.exists() and tmp.stat().st_size > 100:
            os.replace(str(tmp), str(DECOMP_REPORT))
            return True
    except Exception:
        pass
    try:
        if tmp.exists():
            tmp.unlink()
    except Exception:
        pass
    return False


def _auto_report_loop(interval: int) -> None:
    _auto_state["last_matched"] = _report_matched()
    _auto_state["units"] = _report_units()
    # Seed the persisted token history once at startup so the chart has data even
    # before the first interval elapses.
    try:
        refresh_token_history()
    except Exception:
        pass
    while True:
        time.sleep(interval)
        # v10: persist aggregated token buckets every loop (change-gated write).
        try:
            refresh_token_history()
        except Exception:
            pass
        if not _refresh_report_once():
            continue
        # sample the fresh numbers into the time-series history
        try:
            state = get_state(force=True)
            update_history(state)
            decomp = state.get("decomp", {})
            if isinstance(decomp, dict):
                update_unit_history(decomp)
                update_fn_history(decomp)
        except Exception:
            pass
        # Append live attempt-log lines so the activity feed updates without any
        # agent writing to status.md: a per-FILE line for every unit whose matched
        # count moved (this is how codex's work — e.g. menu_middle.c — shows up,
        # since codex commits to git, not status.md), plus an aggregate line.
        new = _report_matched()
        old = _auto_state["last_matched"]
        new_units = _report_units()
        old_units = _auto_state.get("units") or {}
        ts = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
        try:
            lines = []
            for name, (mn, tn) in new_units.items():
                prev = old_units.get(name)
                if prev is not None and mn != prev[0]:
                    sign = "+" if mn > prev[0] else ""
                    lines.append(f"- **{ts}** `report` - {name} {prev[0]}->{mn}/{tn} "
                                 f"({sign}{mn - prev[0]} fns)\n")
            if new is not None and old is not None and new != old:
                tot = int(json.loads(DECOMP_REPORT.read_text(encoding="utf-8", errors="replace"))
                          .get("measures", {}).get("total_functions", 0)) or 1
                lines.append(f"- **{ts}** `auto-report` - report.json {old}->{new} / "
                             f"{tot} ({100.0 * new / tot:.2f}% fns)\n")
            if lines:
                with open(STATUS_LOG, "a", encoding="utf-8") as fh:
                    fh.writelines(lines)
        except Exception:
            pass
        if new is not None:
            _auto_state["last_matched"] = new
        _auto_state["units"] = new_units


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8788)
    parser.add_argument("--once", action="store_true", help="print JSON state and exit")
    args = parser.parse_args()

    if args.once:
        state = get_state(force=True)
        state["history"] = update_history(state)
        decomp = state.get("decomp", {})
        if isinstance(decomp, dict):
            update_unit_history(decomp)
            update_fn_history(decomp)
        print(json.dumps(state, indent=2))
        return 0

    if REPORT_REFRESH_SECONDS > 0 and GEN_REPORT.exists():
        threading.Thread(
            target=_auto_report_loop, args=(REPORT_REFRESH_SECONDS,), daemon=True
        ).start()
        print(f"  auto-refresh: regenerating report.json every {REPORT_REFRESH_SECONDS}s "
              f"(set DASH_REPORT_REFRESH=0 to disable)")

    server = ThreadingHTTPServer((args.host, args.port), Handler)
    print(f"Renaming dashboard: http://{args.host}:{args.port}/")
    server.serve_forever()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
