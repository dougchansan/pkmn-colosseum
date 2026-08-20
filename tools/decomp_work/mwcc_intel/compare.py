from __future__ import annotations

from pathlib import Path
from typing import Any

from .analysis import _invoke
from .common import IntelError, read_json, sha256_file, write_json
from .discovery import discover_mwcc_root, find_external_scripts


PHASES = ("initial", "optimized", "scheduled", "forward_peephole", "allocator")
STAGE_CATEGORIES = {
    "initial": "frontend_or_lowering",
    "optimized": "optimizer",
    "scheduled": "scheduler",
    "forward_peephole": "forward_peephole",
    "allocator": "register_birth",
}


def _artifact(root: Path, pattern: str) -> Path | None:
    matches = sorted((root / "capture").glob(pattern))
    return matches[0] if matches else None


def _pcode_artifact(root: Path, phase: str) -> Path | None:
    matches = sorted(
        path
        for path in (root / "capture").glob(f"pcode-*-{phase}.json")
        if not path.name.startswith("pcode-creations-")
    )
    return matches[0] if matches else None


def _compatibility(left: dict, right: dict) -> None:
    if left.get("compiler", {}).get("sha256") != right.get("compiler", {}).get("sha256"):
        raise IntelError("captures use different compiler hashes")
    left_unit, right_unit = left.get("unit", {}), right.get("unit", {})
    if left_unit.get("source") != right_unit.get("source"):
        raise IntelError("captures do not share translation-unit ownership")
    if left_unit.get("cflags") != right_unit.get("cflags"):
        raise IntelError("captures use incompatible compiler flags")


def compare_captures(
    baseline: Path,
    candidate: Path,
    repo: Path,
    *,
    mwcc_root: str | None = None,
    output: Path | None = None,
) -> dict[str, Any]:
    left = read_json(baseline / "manifest.json")
    right = read_json(candidate / "manifest.json")
    _compatibility(left, right)
    destination = output or candidate / "analysis" / "comparison.json"
    output_dir = destination.parent
    output_dir.mkdir(parents=True, exist_ok=True)
    external = discover_mwcc_root(repo, mwcc_root or left.get("mwcc_decomp", {}).get("path"))
    scripts = find_external_scripts(external) if external else {}
    commands: list[dict[str, Any]] = []
    stage_changes = []
    for phase in PHASES:
        one = _pcode_artifact(baseline, phase) if phase != "allocator" else _artifact(baseline, "allocator-*.json")
        two = _pcode_artifact(candidate, phase) if phase != "allocator" else _artifact(candidate, "allocator-*.json")
        record = {
            "stage": phase,
            "category": STAGE_CATEGORIES[phase],
            "baseline_present": one is not None,
            "candidate_present": two is not None,
            "changed": bool(one and two and sha256_file(one) != sha256_file(two)),
        }
        if one and two and phase != "allocator" and scripts.get("compare_pcode_stages.py"):
            result_path = output_dir / f"variant-pcode-{phase}.json"
            invocation = _invoke(
                Path(scripts["compare_pcode_stages.py"]),
                [one, two, "--output", result_path],
                cwd=external,
                output=result_path,
            )
            commands.append(invocation)
            record["analysis"] = str(result_path) if result_path.is_file() else None
        stage_changes.append(record)
    first_record = next((item for item in stage_changes if item["changed"]), None)
    first = first_record["category"] if first_record else "unknown"
    web_alignments: dict[str, Any] = {}
    left_prov = baseline / "analysis" / "provenance.json"
    right_prov = candidate / "analysis" / "provenance.json"
    if external and left_prov.is_file() and right_prov.is_file():
        script_value = scripts.get("align_register_webs.py")
        if script_value:
            script = Path(script_value)
            for register_class in ("gpr", "fpr", "vr"):
                target = output_dir / f"web-alignment-{register_class}.json"
                invocation = _invoke(
                    script,
                    [left_prov, right_prov, "--register-class", register_class, "--output", target],
                    cwd=external,
                    output=target,
                )
                commands.append(invocation)
                web_alignments[register_class] = {
                    "status": invocation["status"],
                    "output": str(target) if target.is_file() else None,
                }
        rank_script = scripts.get("rank_register_origins.py")
        if rank_script:
            rank_output = output_dir / "register-origin-delta.json"
            commands.append(
                _invoke(
                    Path(rank_script),
                    [left_prov, "--compare", right_prov, "--limit", "20", "--output", rank_output],
                    cwd=external,
                    output=rank_output,
                )
            )

    coloring_outputs = []
    coloring_script = scripts.get("compare_coloring_snapshots.py")
    if external and coloring_script:
        for left_coloring in sorted((baseline / "capture").glob("coloring-*.json")):
            right_coloring = candidate / "capture" / left_coloring.name
            if not right_coloring.is_file():
                continue
            target = output_dir / f"variant-{left_coloring.stem}.json"
            invocation = _invoke(
                Path(coloring_script),
                [left_coloring, right_coloring],
                cwd=external,
                output=target,
            )
            commands.append(invocation)
            if target.is_file():
                coloring_outputs.append(str(target))
    frame_left = baseline / "analysis" / "stack-frame.json"
    frame_right = candidate / "analysis" / "stack-frame.json"
    raw_frame_left = _artifact(baseline, "stack-frame-*.json")
    raw_frame_right = _artifact(candidate, "stack-frame-*.json")
    frame_output = None
    stack_script = scripts.get("stack_frame_trace.py")
    if external and stack_script and raw_frame_left and raw_frame_right:
        frame_target = output_dir / "variant-stack-frame.json"
        args: list[str | Path] = [raw_frame_left]
        if left_prov.is_file():
            args.extend(["--provenance", left_prov])
        args.extend(["--compare", raw_frame_right])
        if right_prov.is_file():
            args.extend(["--compare-provenance", right_prov])
        args.extend(["--output", frame_target])
        invocation = _invoke(Path(stack_script), args, cwd=external, output=frame_target)
        commands.append(invocation)
        frame_output = str(frame_target) if frame_target.is_file() else None
    result = {
        "schema": "mwcc-intel-comparison-v1",
        "symbol": left.get("symbol"),
        "compiler_sha256": left.get("compiler", {}).get("sha256"),
        "compatible": True,
        "stage_changes": stage_changes,
        "first_divergent_stage": first,
        "register_webs": web_alignments,
        "frame": {
            "changed": bool(frame_left.is_file() and frame_right.is_file() and sha256_file(frame_left) != sha256_file(frame_right)),
            "baseline_present": frame_left.is_file(),
            "candidate_present": frame_right.is_file(),
            "analysis": frame_output,
        },
        "coloring_comparisons": coloring_outputs,
        "objdiff": {
            "before": left.get("objdiff", {}).get("match_percent"),
            "after": right.get("objdiff", {}).get("match_percent"),
        },
        "interpretation": "earliest changed captured artifact; semantic causality remains bounded by external-tool guarantees",
    }
    write_json(destination, result)
    write_json(output_dir / "comparison-commands.json", commands)
    return result
