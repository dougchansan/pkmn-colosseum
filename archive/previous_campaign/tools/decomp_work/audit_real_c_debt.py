#!/usr/bin/env python3
"""Audit real-C debt in src/.

This is intentionally conservative: it does not decide whether a function is
"good" C, but it gives repeatable signals for campaign planning:

* active asm wrappers / stubs / real C via progress2.py's classifier
* tracked .inc includes in source files
* raw pointer-offset lines
* register-named variable/reference lines
* address-style fn_ definitions/references

Outputs:
  build/real_c_debt_audit.json
  build/real_c_debt_audit.md
"""
from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools" / "decomp_work"))

import progress2  # noqa: E402

SRC = ROOT / "src"
FN_RE = re.compile(r"\bfn_[0-9A-Fa-f]{8}\b")
INC_RE = re.compile(r"#\s*include\s+[<\"].*\.inc[>\"]")
INLINE_ASM_RE = re.compile(r"\b(?:__asm|asm)\b")
REG_NAME_RE = re.compile(r"\br(?:[0-9]|[12][0-9]|3[01])\b")
REG_LOCAL_RE = re.compile(
    r"\b(?:u8|s8|u16|s16|u32|s32|u64|s64|sint|uint|int|long|float|double|"
    r"f32|f64|BOOL|void\s*\*)\s+r(?:[0-9]|[12][0-9]|3[01])\b"
)
PTR_OFFSET_PATTERNS = [
    re.compile(
        r"\*\s*\(\s*(?:volatile\s+|const\s+)*[A-Za-z_]\w*(?:\s*\*|\s+)\s*\)"
        r"\s*\([^;\n]*\+\s*(?:0x[0-9A-Fa-f]+|\d+)"
    ),
    re.compile(
        r"\(\s*(?:u8|s8|char|void)\s*\*\s*\)\s*[A-Za-z_]\w*"
        r"\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)"
    ),
    re.compile(
        r"\b[A-Za-z_]\w*\s*=\s*\([^;\n]*(?:u8|s8|char|void)\s*\*\s*\)"
        r"[^;\n]*\+\s*(?:0x[0-9A-Fa-f]+|\d+)"
    ),
]


def tracked_c_files() -> list[Path]:
    try:
        r = subprocess.run(
            ["git", "ls-files", "src"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=True,
        )
        files = [
            ROOT / line
            for line in r.stdout.splitlines()
            if line.endswith(".c") and "/pcport/" not in line
        ]
        if files:
            return files
    except Exception:
        pass
    return sorted(p for p in SRC.rglob("*.c") if "/pcport/" not in p.as_posix())


def line_count(lines: list[str], rx: re.Pattern[str]) -> int:
    return sum(1 for line in lines if rx.search(line))


def pointer_offset_lines(lines: list[str]) -> int:
    return sum(1 for line in lines if any(rx.search(line) for rx in PTR_OFFSET_PATTERNS))


def audit_file(path: Path) -> dict:
    text = path.read_text(encoding="latin-1", errors="replace")
    lines = text.splitlines()
    counts, names = progress2.classify_file(path)
    all_defs = names["ASM"] + names["STUB"] + names["REAL_C"]
    rel = path.relative_to(ROOT).as_posix()
    return {
        "path": rel,
        "source_functions": sum(counts.values()),
        "real_c_functions": counts["REAL_C"],
        "asm_wrapper_functions": counts["ASM"],
        "stub_functions": counts["STUB"],
        "fn_named_definitions": sum(1 for name in all_defs if FN_RE.fullmatch(name)),
        "fn_named_references": len(set(FN_RE.findall(text))),
        "inc_include_lines": line_count(lines, INC_RE),
        "inline_asm_token_lines": line_count(lines, INLINE_ASM_RE),
        "raw_pointer_offset_lines": pointer_offset_lines(lines),
        "register_local_lines": line_count(lines, REG_LOCAL_RE),
        "register_named_lines": line_count(lines, REG_NAME_RE),
    }


def totals(rows: list[dict]) -> dict:
    keys = [k for k in rows[0] if k != "path"] if rows else []
    out = {k: sum(int(row[k]) for row in rows) for k in keys}
    out["files"] = len(rows)
    out["files_with_inc"] = sum(1 for row in rows if row["inc_include_lines"])
    out["files_with_asm_wrappers"] = sum(1 for row in rows if row["asm_wrapper_functions"])
    out["files_with_pointer_offsets"] = sum(1 for row in rows if row["raw_pointer_offset_lines"])
    out["files_with_register_names"] = sum(1 for row in rows if row["register_named_lines"])
    return out


def top(rows: list[dict], key: str, n: int) -> list[dict]:
    return [
        {"path": row["path"], key: row[key]}
        for row in sorted(rows, key=lambda row: (-int(row[key]), row["path"]))[:n]
        if row[key]
    ]


def write_markdown(out: dict, path: Path) -> None:
    t = out["totals"]
    lines = [
        "# Real-C Debt Audit",
        "",
        f"Generated: {out['generated_at']}",
        "",
        "| Metric | Count |",
        "|---|---:|",
    ]
    for key in [
        "files",
        "source_functions",
        "real_c_functions",
        "asm_wrapper_functions",
        "stub_functions",
        "fn_named_definitions",
        "fn_named_references",
        "inc_include_lines",
        "inline_asm_token_lines",
        "raw_pointer_offset_lines",
        "register_local_lines",
        "register_named_lines",
        "files_with_inc",
        "files_with_asm_wrappers",
        "files_with_pointer_offsets",
        "files_with_register_names",
    ]:
        lines.append(f"| `{key}` | {t.get(key, 0)} |")

    labels = {
        "asm_wrapper_functions": "Top asm-wrapper files",
        "inc_include_lines": "Top .inc include files",
        "raw_pointer_offset_lines": "Top raw pointer-offset files",
        "register_named_lines": "Top register-name files",
        "fn_named_definitions": "Top fn_ definition files",
        "stub_functions": "Top stub files",
    }
    for key, title in labels.items():
        lines.extend(["", f"## {title}", "", "| File | Count |", "|---|---:|"])
        for row in out["top"][key]:
            lines.append(f"| `{row['path']}` | {row[key]} |")

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--top", type=int, default=20)
    ap.add_argument("--json-out", default="build/real_c_debt_audit.json")
    ap.add_argument("--md-out", default="build/real_c_debt_audit.md")
    args = ap.parse_args()

    rows = [audit_file(path) for path in tracked_c_files()]
    out = {
        "generated_at": time.strftime("%Y-%m-%dT%H:%M:%S%z"),
        "totals": totals(rows),
        "top": {
            key: top(rows, key, args.top)
            for key in [
                "asm_wrapper_functions",
                "inc_include_lines",
                "raw_pointer_offset_lines",
                "register_named_lines",
                "fn_named_definitions",
                "stub_functions",
            ]
        },
        "files": rows,
    }

    json_path = ROOT / args.json_out
    md_path = ROOT / args.md_out
    json_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.write_text(json.dumps(out, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    write_markdown(out, md_path)

    t = out["totals"]
    print(f"files: {t['files']}")
    print(
        "functions: "
        f"real_c={t['real_c_functions']} asm={t['asm_wrapper_functions']} "
        f"stub={t['stub_functions']} total={t['source_functions']}"
    )
    print(
        "debt lines: "
        f"inc={t['inc_include_lines']} ptr_offsets={t['raw_pointer_offset_lines']} "
        f"reg_named={t['register_named_lines']} fn_defs={t['fn_named_definitions']}"
    )
    print(f"wrote {json_path.relative_to(ROOT)} and {md_path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
