from __future__ import annotations

from pathlib import Path
from typing import Any

from .candidates import safe_symbol
from .common import SUPPORTED_COMPILERS, IntelError, require_within, sha256_file, stable_hash, write_json
from .discovery import discover_mwcc_root, external_fingerprint
from .manifest import transition
from .ninja import CompileUnit


def _mount(source: Path, destination: str, readonly: bool = True) -> list[str]:
    value = f"type=bind,src={source.resolve()},dst={destination}"
    if readonly:
        value += ",readonly"
    return ["--mount", value]


def sandbox_argv(
    *,
    runtime: str,
    image: str,
    platform: str,
    repo: Path,
    external: Path,
    compiler: Path,
    wibo: Path,
    wrapper: Path | None,
    output: Path,
    unit: CompileUnit,
) -> tuple[list[str], list[str]]:
    source_in_container = "/workspace/" + unit.source
    compiler_args = [*unit.cflags, "-MMD", "-c", source_in_container, "-o", "/capture"]
    argv = [
        runtime,
        "run",
        "--rm",
        "--pull",
        "never",
        "--platform",
        platform,
        "--network",
        "none",
        "--read-only",
        "--cap-drop",
        "ALL",
        "--security-opt",
        "no-new-privileges",
        "--pids-limit",
        "128",
        "--memory",
        "2g",
        "--cpus",
        "2",
        "--tmpfs",
        "/tmp:rw,noexec,nosuid,nodev,size=128m",
        "--env",
        "WIBO_TMP_DIR=/tmp",
        "--workdir",
        "/workspace",
        *_mount(repo / "src", "/workspace/src"),
        *_mount(repo / "include", "/workspace/include"),
        *_mount(repo / "build" / "GC6E01" / "include", "/workspace/build/GC6E01/include"),
        *_mount(external, "/mwcc"),
        *_mount(compiler, "/input/mwcceppc.exe"),
        *_mount(wibo, "/input/wibo"),
        *(_mount(wrapper, "/input/sjiswrap.exe") if wrapper else []),
        *_mount(output, "/capture", readonly=False),
        image,
        "/bin/sh",
        "/capture/capture-run.sh",
        *compiler_args,
    ]
    return argv, compiler_args


def prepare_capture(
    repo: Path,
    unit: CompileUnit,
    symbol: str,
    *,
    mwcc_root: str | None,
    compiler: str | None,
    wibo: str | None,
    image: str,
    platform: str,
    runtime: str,
    objdiff: dict[str, Any] | None = None,
) -> Path:
    symbol = safe_symbol(symbol)
    external = discover_mwcc_root(repo, mwcc_root)
    if external is None:
        raise IntelError("mwcc-decomp not found; pass --mwcc-root or set MWCC_DECOMP_ROOT")
    compiler_path = Path(compiler).expanduser().resolve() if compiler else repo / unit.compiler
    wibo_path = Path(wibo).expanduser().resolve() if wibo else repo / "build" / "tools" / "wibo"
    wrapper_path = (repo / unit.wrapper).resolve() if unit.wrapper else None
    if not compiler_path.is_file():
        raise IntelError(f"compiler is missing: {compiler_path}")
    if not wibo_path.is_file():
        raise IntelError(f"Wibo is missing: {wibo_path}")
    if wrapper_path is not None and not wrapper_path.is_file():
        raise IntelError(f"compiler wrapper is missing: {wrapper_path}")
    compiler_hash = sha256_file(compiler_path)
    target = SUPPORTED_COMPILERS.get(compiler_hash)
    if target is None:
        raise IntelError(f"unsupported compiler SHA-256: {compiler_hash}")
    if target["version"] != unit.mw_version:
        raise IntelError(
            f"compiler hash maps to {target['version']}, but the owning unit uses {unit.mw_version}"
        )
    source_path = repo / unit.source
    fingerprint_inputs = {
        "schema": 1,
        "symbol": symbol,
        "source": unit.source,
        "source_sha256": sha256_file(source_path),
        "compiler_sha256": compiler_hash,
        "wibo_sha256": sha256_file(wibo_path),
        "wrapper_sha256": sha256_file(wrapper_path) if wrapper_path else None,
        "mwcc_decomp_fingerprint": external_fingerprint(external),
        "capture_target": target["target"],
        "cflags": unit.cflags,
    }
    fingerprint = stable_hash(fingerprint_inputs)
    root = require_within(repo / "build" / "mwcc_intel" / symbol / fingerprint, repo / "build" / "mwcc_intel")
    for child in ("capture", "analysis", "logs"):
        (root / child).mkdir(parents=True, exist_ok=True)
    gdb = "\n".join(
        [
            "set pagination off",
            "set confirm off",
            "set architecture i386",
            "target remote :1234",
            "source /mwcc/tools/gdb/allocator_snapshot.py",
            f"mwcc-auto-capture /capture/capture {symbol} {target['target']}",
            "continue",
            "quit",
            "",
        ]
    )
    (root / "capture.gdb").write_text(gdb, encoding="utf-8", newline="\n")
    compiler_launch = (
        'qemu-i386 -g 1234 /input/wibo /input/sjiswrap.exe /input/mwcceppc.exe "$@" &'
        if wrapper_path
        else 'qemu-i386 -g 1234 /input/wibo /input/mwcceppc.exe "$@" &'
    )
    runner = f"""#!/bin/sh
set -eu
{compiler_launch}
compiler_pid=$!
gdb-multiarch -batch -x /capture/capture.gdb
wait "$compiler_pid"
"""
    (root / "capture-run.sh").write_text(runner, encoding="utf-8", newline="\n")
    argv, mapped_compile = sandbox_argv(
        runtime=runtime,
        image=image,
        platform=platform,
        repo=repo,
        external=external,
        compiler=compiler_path,
        wibo=wibo_path,
        wrapper=wrapper_path,
        output=root,
        unit=unit,
    )
    original_compile = [
        unit.wrapper,
        unit.compiler,
        *unit.cflags,
        "-MMD",
        "-c",
        unit.source,
        "-o",
        unit.basedir,
    ]
    original_compile = [value for value in original_compile if value]
    write_json(root / "docker-argv.json", argv)
    write_json(
        root / "compile-argv.json",
        {"original": original_compile, "sandbox_mapped": mapped_compile, "working_directory": unit.working_directory},
    )
    write_json(root / "source-metadata.json", fingerprint_inputs)
    transition(
        root / "manifest.json",
        "planned",
        schema="mwcc-intel-capture-v1",
        fingerprint=fingerprint,
        symbol=symbol,
        unit=unit.to_dict(),
        inputs=fingerprint_inputs,
        compiler={"path": str(compiler_path), "sha256": compiler_hash, **target},
        wibo={"path": str(wibo_path), "sha256": fingerprint_inputs["wibo_sha256"]},
        wrapper=(
            {"path": str(wrapper_path), "sha256": fingerprint_inputs["wrapper_sha256"]}
            if wrapper_path
            else None
        ),
        mwcc_decomp={"path": str(external), "fingerprint": fingerprint_inputs["mwcc_decomp_fingerprint"]},
        sandbox={"argv": argv, "image": image, "runtime": runtime, "platform": platform},
        objdiff=objdiff or {},
    )
    return root


def execute_capture(capture_dir: Path, *, timeout: int = 300) -> dict[str, Any]:
    from .common import read_json, run

    manifest_path = capture_dir / "manifest.json"
    manifest = read_json(manifest_path)
    compiler_path = Path(manifest["compiler"]["path"])
    if sha256_file(compiler_path) != manifest["compiler"]["sha256"]:
        transition(manifest_path, "failed", error="compiler hash changed before execution")
        raise IntelError("compiler hash changed before execution")
    wibo_path = Path(manifest["wibo"]["path"])
    if sha256_file(wibo_path) != manifest["wibo"]["sha256"]:
        transition(manifest_path, "failed", error="Wibo hash changed before execution")
        raise IntelError("Wibo hash changed before execution")
    wrapper = manifest.get("wrapper")
    if wrapper and sha256_file(Path(wrapper["path"])) != wrapper["sha256"]:
        transition(manifest_path, "failed", error="compiler wrapper hash changed before execution")
        raise IntelError("compiler wrapper hash changed before execution")
    external = Path(manifest["mwcc_decomp"]["path"])
    if external_fingerprint(external) != manifest["mwcc_decomp"]["fingerprint"]:
        transition(manifest_path, "failed", error="mwcc-decomp fingerprint changed before execution")
        raise IntelError("mwcc-decomp fingerprint changed before execution")
    runtime = manifest["sandbox"]["runtime"]
    image = manifest["sandbox"]["image"]
    image_result = run(
        [runtime, "image", "inspect", image, "--format", "{{.Id}}"],
        cwd=capture_dir,
        timeout=10,
    )
    if image_result.returncode or not image_result.stdout.strip():
        transition(manifest_path, "failed", error="debugger image is unavailable with --pull never")
        raise IntelError("debugger image is unavailable locally; live capture will not pull it")
    argv = manifest["sandbox"]["argv"]
    transition(manifest_path, "executed", image_id=image_result.stdout.strip())
    result = run(argv, cwd=capture_dir, timeout=timeout)
    (capture_dir / "logs" / "container.stdout.log").write_text(result.stdout[-200000:], encoding="utf-8")
    (capture_dir / "logs" / "container.stderr.log").write_text(result.stderr[-200000:], encoding="utf-8")
    artifacts = sorted((capture_dir / "capture").glob("*.json"))
    output = capture_dir / Path(manifest["unit"]["source"]).with_suffix(".o").name
    if result.returncode or not artifacts or not output.is_file():
        return transition(
            manifest_path,
            "failed",
            error=f"container exited {result.returncode}; artifacts={len(artifacts)}; output={output.is_file()}",
        )
    hashes = {path.relative_to(capture_dir).as_posix(): sha256_file(path) for path in [output, *artifacts]}
    return transition(manifest_path, "capture_complete", artifact_hashes=hashes)
