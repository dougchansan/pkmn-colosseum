from __future__ import annotations

import argparse
import json
import shutil
import sys
from pathlib import Path
from typing import Iterable

from .analysis import analyze_capture
from .candidates import render_table, select_candidates
from .common import IntelError, find_repo_root, read_json, write_json
from .compare import compare_captures
from .discovery import doctor
from .ninja import find_unit_for_symbol, parse_compile_units
from .sandbox import execute_capture, prepare_capture


def _add_environment(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--mwcc-root", help="external mwcc-decomp checkout")
    parser.add_argument("--compiler", help="exact mwcceppc.exe override")
    parser.add_argument("--wibo", help="Wibo override")
    parser.add_argument("--image", default="mwcc-debugger:local", help="pre-existing debugger image")
    parser.add_argument("--platform", default="linux/amd64")
    parser.add_argument("--build-dir", default="build")
    parser.add_argument("--version", default="GC6E01")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Optional external MWCC compiler-intelligence pipeline")
    subparsers = parser.add_subparsers(dest="command", required=True)

    doctor_parser = subparsers.add_parser("doctor", help="inspect prerequisites without executing the compiler")
    _add_environment(doctor_parser)
    doctor_parser.add_argument("--json", dest="json_output")

    select = subparsers.add_parser("select", help="rank supported near-match functions")
    _add_environment(select)
    select.add_argument("--minimum-match", type=float, default=90.0)
    select.add_argument("--maximum-match", type=float, default=99.999)
    select.add_argument("--minimum-size", type=int, default=32)
    select.add_argument("--maximum-size", type=int, default=2048)
    select.add_argument("--limit", type=int, default=20)
    select.add_argument("--output", default="build/mwcc_intel/candidates.json")
    select.add_argument("--no-objdiff", action="store_true", help="skip function-level mismatch enrichment")

    capture = subparsers.add_parser("capture", help="generate or execute a hardened capture")
    _add_environment(capture)
    capture.add_argument("--symbol", required=True)
    modes = capture.add_mutually_exclusive_group()
    modes.add_argument("--dry-run", action="store_true")
    modes.add_argument("--execute", action="store_true")
    capture.add_argument("--runtime", help="container runtime executable")
    capture.add_argument("--timeout", type=int, default=300)

    analyze = subparsers.add_parser("analyze", help="run applicable external analyses")
    analyze.add_argument("capture_dir")
    analyze.add_argument("--mwcc-root")

    compare = subparsers.add_parser("compare", help="compare compatible baseline/candidate captures")
    compare.add_argument("--baseline", required=True)
    compare.add_argument("--candidate", required=True)
    compare.add_argument("--mwcc-root")
    compare.add_argument("--output")

    pilot = subparsers.add_parser("pilot", help="orchestrate the deepest safe available workflow")
    _add_environment(pilot)
    pilot.add_argument("--symbol")
    pilot.add_argument("--auto", action="store_true")
    modes = pilot.add_mutually_exclusive_group()
    modes.add_argument("--dry-run", action="store_true")
    modes.add_argument("--execute", action="store_true")
    pilot.add_argument("--attempt-source-edit", action="store_true")
    pilot.add_argument("--runtime")
    pilot.add_argument("--timeout", type=int, default=300)
    return parser


def _context(repo: Path, build_dir: str, version: str):
    report = read_json(repo / build_dir / version / "report.json")
    units = parse_compile_units(repo / "build.ninja", repo)
    return report, units


def _runtime(value: str | None) -> str:
    result = value or shutil.which("docker") or shutil.which("podman")
    if not result:
        raise IntelError("no Docker-compatible container runtime found")
    return result


def command_doctor(args: argparse.Namespace, repo: Path) -> int:
    result = doctor(
        repo,
        mwcc_root=args.mwcc_root,
        compiler=args.compiler,
        wibo=args.wibo,
        image=args.image,
        build_dir=args.build_dir,
        version=args.version,
    )
    if args.json_output:
        write_json(repo / args.json_output, result)
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


def _select(args: argparse.Namespace, repo: Path) -> list[dict]:
    report, units = _context(repo, args.build_dir, args.version)
    return select_candidates(
        repo,
        report,
        units,
        minimum_match=args.minimum_match,
        maximum_match=args.maximum_match,
        minimum_size=args.minimum_size,
        maximum_size=args.maximum_size,
        limit=args.limit,
        enrich=not args.no_objdiff,
    )


def command_select(args: argparse.Namespace, repo: Path) -> int:
    candidates = _select(args, repo)
    write_json(repo / args.output, candidates)
    print(render_table(candidates))
    print(f"\nwrote {args.output}")
    return 0 if candidates else 1


def _prepare_for_symbol(args: argparse.Namespace, repo: Path, symbol: str) -> Path:
    report, units = _context(repo, args.build_dir, args.version)
    _owner, function, unit = find_unit_for_symbol(report, units, symbol)
    return prepare_capture(
        repo,
        unit,
        symbol,
        mwcc_root=args.mwcc_root,
        compiler=args.compiler,
        wibo=args.wibo,
        image=args.image,
        platform=args.platform,
        runtime=_runtime(args.runtime),
        objdiff={
            "match_percent": function.get("fuzzy_match_percent"),
            "target_size": int(function.get("size", 0)),
            "target_object": unit.target_object,
            "candidate_object": unit.candidate_object,
        },
    )


def command_capture(args: argparse.Namespace, repo: Path) -> int:
    capture_dir = _prepare_for_symbol(args, repo, args.symbol)
    if args.execute:
        manifest = execute_capture(capture_dir, timeout=args.timeout)
        print(json.dumps(manifest, indent=2, sort_keys=True))
        return 0 if manifest.get("state") == "capture_complete" else 2
    print(capture_dir)
    print("dry run complete; compiler was not executed")
    return 0


def command_analyze(args: argparse.Namespace, repo: Path) -> int:
    capture_dir = Path(args.capture_dir).expanduser().resolve()
    result = analyze_capture(capture_dir, repo, mwcc_root=args.mwcc_root)
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


def command_compare(args: argparse.Namespace, repo: Path) -> int:
    output = Path(args.output).resolve() if args.output else None
    result = compare_captures(
        Path(args.baseline).expanduser().resolve(),
        Path(args.candidate).expanduser().resolve(),
        repo,
        mwcc_root=args.mwcc_root,
        output=output,
    )
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


def command_pilot(args: argparse.Namespace, repo: Path) -> int:
    result: dict = {
        "schema": "mwcc-intel-pilot-v1",
        "doctor": doctor(
            repo,
            mwcc_root=args.mwcc_root,
            compiler=args.compiler,
            wibo=args.wibo,
            image=args.image,
            build_dir=args.build_dir,
            version=args.version,
        ),
    }
    symbol = args.symbol
    if not symbol:
        selection_args = argparse.Namespace(
            build_dir=args.build_dir,
            version=args.version,
            minimum_match=90.0,
            maximum_match=99.999,
            minimum_size=32,
            maximum_size=2048,
            limit=10,
            no_objdiff=False,
        )
        candidates = _select(selection_args, repo)
        result["candidates"] = candidates
        if not candidates:
            raise IntelError("pilot found no supported candidates")
        symbol = candidates[0]["symbol"]
    result["symbol"] = symbol
    capture_dir = _prepare_for_symbol(args, repo, symbol)
    result["capture_dir"] = str(capture_dir)
    result["capture_mode"] = "execute" if args.execute else "dry-run"
    if args.execute:
        manifest = execute_capture(capture_dir, timeout=args.timeout)
        result["capture_state"] = manifest.get("state")
        if manifest.get("state") == "capture_complete":
            result["analysis"] = analyze_capture(capture_dir, repo, mwcc_root=args.mwcc_root)
    else:
        result["capture_state"] = "planned"
    if args.attempt_source_edit:
        diagnosis = result.get("analysis", {}).get("diagnosis", {})
        strong = diagnosis.get("confidence") == "high" and diagnosis.get("first_divergent_stage") not in ("unknown", "unsupported")
        result["source_experiment"] = {
            "attempted": False,
            "reason": "no high-confidence constrained edit family" if not strong else "manual review required before source mutation",
        }
    destination = repo / "build" / "mwcc_intel" / "pilot-report.json"
    write_json(destination, result)
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


COMMANDS = {
    "doctor": command_doctor,
    "select": command_select,
    "capture": command_capture,
    "analyze": command_analyze,
    "compare": command_compare,
    "pilot": command_pilot,
}


def main(argv: Iterable[str] | None = None) -> int:
    args = build_parser().parse_args(list(argv) if argv is not None else None)
    try:
        repo = find_repo_root()
        return int(COMMANDS[args.command](args, repo))
    except IntelError as exc:
        print(f"mwcc-intel error: {exc}", file=sys.stderr)
        return 2
