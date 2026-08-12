from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from .common import IntelError, read_json, run, write_json
from .discovery import discover_mwcc_root, find_external_scripts
from .manifest import transition


def _one(paths: list[Path], pattern: str) -> Path | None:
    matches = sorted(path for path in paths if path.match(pattern))
    return matches[0] if matches else None


def _pcode_stage(paths: list[Path], phase: str) -> Path | None:
    matches = sorted(
        path
        for path in paths
        if path.name.startswith("pcode-")
        and not path.name.startswith("pcode-creations-")
        and path.name.endswith(f"-{phase}.json")
    )
    return matches[0] if matches else None


def _invoke(
    script: Path,
    args: list[str | Path],
    *,
    cwd: Path,
    output: Path | None = None,
    timeout: int = 120,
) -> dict[str, Any]:
    help_result = run([sys.executable, script, "--help"], cwd=script.parent, timeout=30)
    if help_result.returncode:
        return {
            "tool": script.name,
            "status": "help_failed",
            "help_returncode": help_result.returncode,
            "stderr": help_result.stderr[-1200:],
        }
    argv = [sys.executable, str(script), *[str(arg) for arg in args]]
    result = run(argv, cwd=cwd, timeout=timeout)
    record = {
        "tool": script.name,
        "status": "ok" if result.returncode == 0 else "failed",
        "argv": argv,
        "help": help_result.stdout,
        "returncode": result.returncode,
        "stdout": result.stdout[-12000:],
        "stderr": result.stderr[-12000:],
    }
    if output is not None:
        record["output"] = str(output)
        if result.returncode == 0 and not output.exists() and result.stdout.strip():
            output.write_text(result.stdout, encoding="utf-8")
    return record


def analyze_capture(capture_dir: Path, repo: Path, *, mwcc_root: str | None = None) -> dict[str, Any]:
    manifest_path = capture_dir / "manifest.json"
    manifest = read_json(manifest_path)
    if manifest.get("state") not in ("capture_complete", "analysis_complete"):
        raise IntelError(f"analysis requires capture_complete state, got {manifest.get('state')!r}")
    external = discover_mwcc_root(repo, mwcc_root or manifest.get("mwcc_decomp", {}).get("path"))
    if external is None:
        raise IntelError("mwcc-decomp not found for analysis")
    scripts = find_external_scripts(external)
    capture = capture_dir / "capture"
    output_dir = capture_dir / "analysis"
    output_dir.mkdir(parents=True, exist_ok=True)
    artifacts = sorted(capture.glob("*.json"))
    commands: list[dict[str, Any]] = []
    generated: dict[str, str] = {}

    allocator = _one(artifacts, "allocator-*.json")
    creations = _one(artifacts, "pcode-creations-*-allocator.json")
    colorings = sorted(capture.glob("coloring-*.json"))
    provenance = output_dir / "provenance.json"
    provenance_script = scripts.get("allocator_provenance.py")
    if allocator and creations and colorings and provenance_script:
        args: list[str | Path] = [allocator]
        for coloring in colorings:
            args.extend(["--coloring", coloring])
        args.extend(["--creations", creations, "--output", provenance])
        record = _invoke(Path(provenance_script), args, cwd=external, output=provenance)
        commands.append(record)
        if record["status"] == "ok" and provenance.is_file():
            generated["provenance"] = str(provenance)
    else:
        commands.append({"tool": "allocator_provenance.py", "status": "missing_inputs"})

    rank_script = scripts.get("rank_register_origins.py")
    rank_output = output_dir / "register-origins.json"
    if provenance.is_file() and rank_script:
        record = _invoke(
            Path(rank_script),
            [provenance, "--limit", "20", "--output", rank_output],
            cwd=external,
            output=rank_output,
        )
        commands.append(record)
        if record["status"] == "ok" and rank_output.is_file():
            generated["register_origins"] = str(rank_output)

    pcode_script = scripts.get("compare_pcode_stages.py")
    phases = ("initial", "optimized", "scheduled", "forward_peephole")
    pcode_paths = {phase: _pcode_stage(artifacts, phase) for phase in phases}
    for before, after in zip(phases, phases[1:]):
        left, right = pcode_paths[before], pcode_paths[after]
        if not (left and right and pcode_script):
            continue
        result_path = output_dir / f"pcode-{before}-to-{after}.json"
        args = [left, right]
        after_creations = _one(artifacts, f"pcode-creations-*-{after}.json")
        if after_creations:
            args.extend(["--creations", after_creations])
        args.extend(["--output", result_path])
        record = _invoke(Path(pcode_script), args, cwd=external, output=result_path)
        commands.append(record)
        if record["status"] == "ok" and result_path.is_file():
            generated[f"pcode_{before}_to_{after}"] = str(result_path)

    stack_script = scripts.get("stack_frame_trace.py")
    stack = _one(artifacts, "stack-frame-*.json")
    if stack and stack_script:
        result_path = output_dir / "stack-frame.json"
        args = [stack]
        if provenance.is_file():
            args.extend(["--provenance", provenance])
        args.extend(["--output", result_path])
        record = _invoke(Path(stack_script), args, cwd=external, output=result_path)
        commands.append(record)
        if record["status"] == "ok" and result_path.is_file():
            generated["stack_frame"] = str(result_path)

    simple_tools = (
        ("home_list_trace.py", "home-list-*.json", "home-list.json", []),
        ("explain_code_motion.py", "code-motion-*.json", "code-motion.json", []),
    )
    for tool, pattern, filename, extra in simple_tools:
        artifact = _one(artifacts, pattern)
        script = scripts.get(tool)
        if not artifact or not script:
            continue
        result_path = output_dir / filename
        record = _invoke(Path(script), [artifact, *extra], cwd=external, output=result_path)
        commands.append(record)
        if record["status"] == "ok" and result_path.is_file():
            generated[tool.removesuffix(".py")] = str(result_path)

    coloring_script = scripts.get("compare_coloring_snapshots.py")
    for before in sorted(capture.glob("coloring-*-before.json")):
        after = before.with_name(before.name.replace("-before.json", "-after.json"))
        if not after.is_file() or not coloring_script:
            continue
        result_path = output_dir / (before.stem + "-delta.json")
        record = _invoke(Path(coloring_script), [before, after], cwd=external, output=result_path)
        commands.append(record)
        if record["status"] == "ok" and result_path.is_file():
            generated[result_path.stem] = str(result_path)

    classifications = []
    for key, category in (
        ("pcode_initial_to_optimized", "optimizer"),
        ("pcode_optimized_to_scheduled", "scheduler"),
        ("pcode_scheduled_to_forward_peephole", "forward_peephole"),
        ("stack_frame", "stack_frame"),
        ("provenance", "register_birth"),
    ):
        path_value = generated.get(key)
        if path_value:
            classifications.append(
                {
                    "category": category,
                    "evidence_file": str(Path(path_value).relative_to(capture_dir)),
                    "confidence": "medium",
                    "basis": "inferred",
                    "finding": "the applicable external analysis completed; interpret its bounded result",
                    "recommended_experiment": "compare a single semantics-preserving source-shape variant",
                    "falsified_when": "the controlled capture leaves this stage unchanged",
                }
            )
    normalized = {
        "schema": "mwcc-intel-analysis-v1",
        "symbol": manifest.get("symbol"),
        "compiler_sha256": manifest.get("compiler", {}).get("sha256"),
        "generated": {key: str(Path(value).relative_to(capture_dir)) for key, value in generated.items()},
        "diagnosis": {
            "first_divergent_stage": "unknown",
            "confidence": "low",
            "exact_findings": [
                {"tool": item["tool"], "output": item.get("output")}
                for item in commands
                if item.get("status") == "ok"
            ],
            "inferences": classifications,
            "hypotheses": [],
        },
        "interpretation_limits": [
            "allocator replay is exact only for the modeled captured path",
            "inverse-order witnesses do not prove source realizability",
            "pressure vectors are lower bounds, not concrete source edits",
            "sampled misses are not impossibility proofs",
            "ambiguous semantic-web mappings must remain ambiguous",
        ],
    }
    write_json(output_dir / "analysis.json", normalized)
    write_json(output_dir / "commands.json", commands)
    lines = [
        f"# MWCC intelligence report: {manifest.get('symbol')}",
        "",
        f"Compiler: `{manifest.get('compiler', {}).get('sha256')}`",
        f"First bounded classification: `{normalized['diagnosis']['first_divergent_stage']}`",
        "",
        "## Completed analyses",
        "",
    ]
    lines.extend(f"- `{item['tool']}`: {item['status']}" for item in commands)
    lines.extend(["", "## Interpretation", "", "No result is a source-necessity proof. Generated evidence remains local under `build/`.", ""])
    (output_dir / "report.md").write_text("\n".join(lines), encoding="utf-8")
    transition(manifest_path, "analysis_complete", analysis="analysis/analysis.json")
    return normalized
