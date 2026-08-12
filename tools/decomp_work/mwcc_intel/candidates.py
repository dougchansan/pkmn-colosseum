from __future__ import annotations

import json
import re
from pathlib import Path
from typing import Any

from .common import SUPPORTED_COMPILERS, IntelError, run, sha256_file
from .ninja import CompileUnit


EMPTY_MISMATCH = {
    "register": 0,
    "opcode": 0,
    "branch": 0,
    "relocation": 0,
    "stack_offset": 0,
    "order": 0,
    "unknown": 0,
}


def safe_symbol(value: str) -> str:
    if not re.fullmatch(r"[A-Za-z_.$][A-Za-z0-9_.$@]*", value):
        raise IntelError(f"unsafe or unsupported symbol selector: {value!r}")
    return value


def _has_definition(source: Path, symbol: str) -> bool:
    if not source.is_file():
        return False
    text = source.read_text(encoding="utf-8", errors="replace")
    return bool(re.search(rf"(?m)^\s*[\w\s*]+\b{re.escape(symbol)}\s*\([^;]*\)\s*\{{", text))


def _objdiff_binary(repo: Path) -> Path | None:
    for name in ("objdiff-cli.exe", "objdiff-cli"):
        path = repo / "build" / "tools" / name
        if path.is_file():
            return path
    return None


def classify_objdiff(repo: Path, record: dict[str, Any]) -> dict[str, Any]:
    summary = dict(EMPTY_MISMATCH)
    binary = _objdiff_binary(repo)
    target = repo / record["target_object"]
    candidate = repo / record["candidate_object"]
    if binary is None or not target.is_file() or not candidate.is_file():
        return {
            "mismatch_summary": summary,
            "target_instruction_count": 0,
            "candidate_instruction_count": 0,
            "classification": "unavailable",
        }
    result = run(
        [
            binary,
            "diff",
            "-1",
            target,
            "-2",
            candidate,
            "-o",
            "-",
            "--format",
            "json",
            record["symbol"],
        ],
        cwd=repo,
        timeout=30,
    )
    if result.returncode:
        return {
            "mismatch_summary": summary,
            "target_instruction_count": 0,
            "candidate_instruction_count": 0,
            "classification": "objdiff_failed",
        }
    try:
        payload = json.loads(result.stdout)
        left = next(item for item in payload["left"]["symbols"] if item.get("name") == record["symbol"])
        right = next(item for item in payload["right"]["symbols"] if item.get("name") == record["symbol"])
    except (KeyError, StopIteration, TypeError, json.JSONDecodeError):
        return {
            "mismatch_summary": summary,
            "target_instruction_count": 0,
            "candidate_instruction_count": 0,
            "classification": "schema_unknown",
        }
    instructions = left.get("instructions", [])
    for entry in instructions:
        kind = entry.get("diff_kind")
        if not kind:
            continue
        instruction = entry.get("instruction") or {}
        formatted = instruction.get("formatted", "")
        mnemonic = ""
        parts = instruction.get("parts") or []
        if parts and isinstance(parts[0], dict):
            mnemonic = parts[0].get("opcode", {}).get("mnemonic", "")
        if instruction.get("relocation") and kind == "DIFF_ARG_MISMATCH":
            summary["relocation"] += 1
        elif mnemonic.startswith("b") and kind in ("DIFF_ARG_MISMATCH", "DIFF_OP_MISMATCH"):
            summary["branch"] += 1
        elif "(r1)" in formatted and kind == "DIFF_ARG_MISMATCH":
            summary["stack_offset"] += 1
        elif kind == "DIFF_ARG_MISMATCH" and re.search(r"\b(?:r|f)\d+\b", formatted):
            summary["register"] += 1
        elif kind == "DIFF_OP_MISMATCH":
            summary["opcode"] += 1
        elif kind in ("DIFF_INSERT", "DIFF_DELETE"):
            summary["order"] += 1
        else:
            summary["unknown"] += 1
    return {
        "mismatch_summary": summary,
        "target_instruction_count": sum(1 for item in instructions if item.get("instruction")),
        "candidate_instruction_count": sum(1 for item in right.get("instructions", []) if item.get("instruction")),
        "target_size": int(left.get("size", record.get("target_size", 0))),
        "candidate_size": int(right.get("size", 0)),
        "match_percent": float(left.get("match_percent", record["match_percent"])),
        "classification": "conservative",
    }


def _score(record: dict[str, Any]) -> tuple[float, list[str]]:
    percent = record["match_percent"]
    size = record["target_size"]
    score = 100.0 - abs(98.0 - percent) * 8.0
    reasons = [f"{percent:.3f}% current match", f"{size} byte target"]
    if 64 <= size <= 1024:
        score += 12
        reasons.append("manageable function size")
    target_count = record.get("target_instruction_count", 0)
    candidate_count = record.get("candidate_instruction_count", 0)
    if target_count and abs(target_count - candidate_count) <= 2:
        score += 15
        reasons.append("instruction counts are equal or nearly equal")
    summary = record["mismatch_summary"]
    tractable = summary["register"] + summary["stack_offset"] + summary["order"]
    disruptive = summary["opcode"] + summary["branch"] + summary["relocation"] + summary["unknown"]
    if tractable > disruptive and tractable:
        score += 20
        reasons.append("mismatches are dominated by allocator/order/frame symptoms")
    if summary["relocation"] > tractable:
        score -= 20
        reasons.append("relocations dominate the observed mismatches")
    return round(score, 4), reasons


def select_candidates(
    repo: Path,
    report: dict,
    units: dict[str, CompileUnit],
    *,
    minimum_match: float = 90.0,
    maximum_match: float = 99.999,
    minimum_size: int = 32,
    maximum_size: int = 2048,
    limit: int = 20,
    enrich: bool = True,
) -> list[dict[str, Any]]:
    candidates = []
    by_source: dict[str, CompileUnit] = {unit.source: unit for unit in units.values()}
    for owner in report.get("units", []):
        metadata = owner.get("metadata", {})
        source_value = metadata.get("source_path")
        if not source_value or metadata.get("auto_generated"):
            continue
        source_value = source_value.replace("\\", "/")
        unit_name = owner.get("name", "").replace("\\", "/")
        expected_output = (
            "build/GC6E01/src/" + unit_name.split("/", 1)[1] + ".o"
            if unit_name.startswith("main/")
            else ""
        )
        unit = units.get(expected_output) or by_source.get(source_value)
        if unit is None or unit.mw_version not in ("GC/1.2.5", "GC/1.2.5n"):
            continue
        compiler = repo / unit.compiler
        digest = sha256_file(compiler) if compiler.is_file() else None
        supported = SUPPORTED_COMPILERS.get(digest or "")
        if compiler.is_file() and supported is None:
            continue
        source = repo / source_value
        target = repo / unit.target_object
        candidate = repo / unit.candidate_object
        if not target.is_file() or not candidate.is_file():
            continue
        for function in owner.get("functions", []):
            symbol = function.get("name")
            percent = function.get("fuzzy_match_percent")
            size = int(function.get("size", 0))
            if not symbol or percent is None or not minimum_match <= float(percent) <= maximum_match:
                continue
            if not minimum_size <= size <= maximum_size or not _has_definition(source, symbol):
                continue
            address = function.get("metadata", {}).get("virtual_address")
            record = {
                "symbol": safe_symbol(symbol),
                "address": f"0x{int(address):08X}" if address is not None else None,
                "source": source_value,
                "unit": owner.get("name"),
                "target_object": unit.target_object,
                "candidate_object": unit.candidate_object,
                "compiler_version": unit.mw_version,
                "compiler_target": supported["target"] if supported else None,
                "compiler_sha256": digest,
                "match_percent": float(percent),
                "target_size": size,
                "candidate_size": 0,
                "target_instruction_count": 0,
                "candidate_instruction_count": 0,
                "mismatch_summary": dict(EMPTY_MISMATCH),
                "selection_score": 0.0,
                "selection_reasons": [],
            }
            record["selection_score"], record["selection_reasons"] = _score(record)
            candidates.append(record)
    candidates.sort(key=lambda item: (-item["selection_score"], -item["match_percent"], item["symbol"]))
    if enrich:
        for record in candidates[: max(limit * 2, limit)]:
            record.update(classify_objdiff(repo, record))
            record["selection_score"], record["selection_reasons"] = _score(record)
        candidates.sort(key=lambda item: (-item["selection_score"], -item["match_percent"], item["symbol"]))
    return candidates[:limit]


def render_table(candidates: list[dict[str, Any]]) -> str:
    header = "score   match    bytes  target  symbol                            source"
    rows = [header, "-" * len(header)]
    for item in candidates:
        rows.append(
            f"{item['selection_score']:6.1f}  {item['match_percent']:7.3f}  "
            f"{item['target_size']:5d}  {(item['compiler_target'] or '-'):6s}  "
            f"{item['symbol'][:32]:32s}  {item['source']}"
        )
    return "\n".join(rows)
