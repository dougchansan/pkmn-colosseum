from __future__ import annotations

import os
import shutil
from pathlib import Path
from typing import Any

from .common import IntelError, SUPPORTED_COMPILERS, display_path, run, sha256_file, stable_hash


EXTERNAL_SCRIPTS = (
    "allocator_snapshot.py",
    "allocator_provenance.py",
    "rank_register_origins.py",
    "compare_pcode_stages.py",
    "explain_register.py",
    "align_register_webs.py",
    "inverse_coloring.py",
    "source_rank_solver.py",
    "stack_frame_trace.py",
    "home_list_trace.py",
    "explain_code_motion.py",
    "compare_coloring_snapshots.py",
)


def discover_mwcc_root(repo: Path, explicit: str | None = None) -> Path | None:
    candidates = []
    if explicit:
        candidates.append(Path(explicit).expanduser())
    if os.environ.get("MWCC_DECOMP_ROOT"):
        candidates.append(Path(os.environ["MWCC_DECOMP_ROOT"]).expanduser())
    candidates.append(repo.parent / "mwcc-decomp")
    for candidate in candidates:
        resolved = candidate.resolve()
        if (resolved / "tools" / "gdb" / "allocator_snapshot.py").is_file():
            return resolved
    return None


def external_fingerprint(root: Path | None) -> str | None:
    if root is None:
        return None
    if (root / ".git").exists():
        revision = run(["git", "-C", root, "rev-parse", "HEAD"], cwd=root)
        if revision.returncode == 0 and revision.stdout.strip():
            return revision.stdout.strip()
    records = []
    for directory in (root / "tools", root / "config"):
        if not directory.is_dir():
            continue
        for path in sorted(candidate for candidate in directory.rglob("*") if candidate.is_file()):
            records.append((path.relative_to(root).as_posix(), sha256_file(path)))
    return stable_hash(records)


def find_external_scripts(root: Path | None) -> dict[str, str | None]:
    result: dict[str, str | None] = {}
    for name in EXTERNAL_SCRIPTS:
        matches = list(root.rglob(name)) if root else []
        result[name] = str(matches[0].resolve()) if matches else None
    return result


def compiler_record(path: Path | None, repo: Path) -> dict[str, Any]:
    if path is None or not path.is_file():
        return {"path": display_path(path, repo) if path else None, "present": False, "supported": False}
    digest = sha256_file(path)
    supported = SUPPORTED_COMPILERS.get(digest)
    return {
        "path": display_path(path, repo),
        "present": True,
        "sha256": digest,
        "supported": supported is not None,
        "version": supported["version"] if supported else None,
        "capture_target": supported["target"] if supported else None,
    }


def _tracked(repo: Path, path: Path | None) -> bool:
    if path is None or not path.exists():
        return False
    try:
        relative = path.resolve().relative_to(repo.resolve())
    except ValueError:
        return False
    result = run(["git", "ls-files", "--error-unmatch", relative], cwd=repo)
    return result.returncode == 0


def doctor(
    repo: Path,
    *,
    mwcc_root: str | None = None,
    compiler: str | None = None,
    wibo: str | None = None,
    image: str = "mwcc-intel-gdb:local",
    build_dir: str = "build",
    version: str = "GC6E01",
) -> dict[str, Any]:
    build = (repo / build_dir).resolve()
    external = discover_mwcc_root(repo, mwcc_root)
    compiler_paths = []
    if compiler:
        compiler_paths.append(Path(compiler).expanduser().resolve())
    else:
        compiler_paths.extend(
            [
                build / "compilers" / "GC" / "1.2.5" / "mwcceppc.exe",
                build / "compilers" / "GC" / "1.2.5n" / "mwcceppc.exe",
            ]
        )
    wibo_path = Path(wibo).expanduser().resolve() if wibo else build / "tools" / "wibo"
    runtime = shutil.which("docker") or shutil.which("podman")
    image_result = None
    if runtime:
        try:
            image_result = run([runtime, "image", "inspect", image], cwd=repo, timeout=5)
        except IntelError:
            image_result = None
    compilers = [compiler_record(path, repo) for path in compiler_paths]
    wibo_record = {
        "path": display_path(wibo_path, repo),
        "present": wibo_path.is_file(),
        "sha256": sha256_file(wibo_path) if wibo_path.is_file() else None,
        "tracked": _tracked(repo, wibo_path),
    }
    result = {
        "repository": str(repo.resolve()),
        "version": version,
        "build": {
            "directory": display_path(build, repo),
            "build_ninja": (repo / "build.ninja").is_file(),
            "report": (build / version / "report.json").is_file(),
            "objdiff_config": (repo / "objdiff.json").is_file(),
            "objdiff_cli": any((build / "tools" / name).is_file() for name in ("objdiff-cli", "objdiff-cli.exe")),
        },
        "mwcc_decomp": {
            "root": str(external) if external else None,
            "present": external is not None,
            "fingerprint": external_fingerprint(external),
            "scripts": find_external_scripts(external),
        },
        "container": {
            "runtime": runtime,
            "image": image,
            "image_available_without_pull": bool(image_result and image_result.returncode == 0),
        },
        "compilers": compilers,
        "wibo": wibo_record,
    }
    result["live_capture_ready"] = bool(
        external
        and runtime
        and result["container"]["image_available_without_pull"]
        and wibo_record["present"]
        and not wibo_record["tracked"]
        and any(item["supported"] for item in compilers)
    )
    return result
