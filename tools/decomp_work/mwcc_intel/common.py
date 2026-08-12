from __future__ import annotations

import hashlib
import json
import os
import subprocess
from pathlib import Path
from typing import Any, Iterable


SUPPORTED_COMPILERS = {
    "0443b5c02b1aa7b575b61e0e24c4d5ad6bed8fd54cc42de5a2204a5216001914": {
        "version": "GC/1.2.5",
        "target": "stock",
    },
    "ccf4b465cec73b5aae9c5c5543dcf8cda8a62aba246f89e2e0b200d742f2e55c": {
        "version": "GC/1.2.5n",
        "target": "ninji",
    },
}


class IntelError(RuntimeError):
    """Expected failure with a concise, safe diagnostic."""


def find_repo_root(start: Path | None = None) -> Path:
    current = (start or Path.cwd()).resolve()
    for candidate in (current, *current.parents):
        if (candidate / "configure.py").is_file() and (candidate / "config").is_dir():
            return candidate
    raise IntelError("could not locate the pkmn-colosseum repository root")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def stable_hash(value: Any) -> str:
    encoded = json.dumps(value, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def read_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise IntelError(f"could not read JSON {display_path(path)}: {exc}") from exc


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def display_path(path: Path, root: Path | None = None) -> str:
    resolved = path.resolve()
    if root is not None:
        try:
            return resolved.relative_to(root.resolve()).as_posix()
        except ValueError:
            pass
    return str(resolved)


def require_within(path: Path, parent: Path) -> Path:
    resolved = path.resolve()
    try:
        resolved.relative_to(parent.resolve())
    except ValueError as exc:
        raise IntelError(f"path escapes required root: {resolved}") from exc
    return resolved


def run(
    argv: Iterable[str | os.PathLike[str]],
    *,
    cwd: Path,
    timeout: int = 30,
    check: bool = False,
) -> subprocess.CompletedProcess[str]:
    command = [os.fspath(value) for value in argv]
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            text=True,
            capture_output=True,
            timeout=timeout,
            check=False,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        raise IntelError(f"command failed to start or timed out: {Path(command[0]).name}: {exc}") from exc
    if check and result.returncode:
        detail = (result.stderr or result.stdout).strip()[-1600:]
        raise IntelError(f"command failed ({result.returncode}): {Path(command[0]).name}: {detail}")
    return result
