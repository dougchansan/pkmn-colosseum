#!/usr/bin/env python3
"""Verify source-owned data progress entries against split target objects."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
DEFAULT_PROGRESS = ROOT / "config" / "GC6E01" / "data_progress.json"
DEFAULT_BUILD = ROOT / "build" / "GC6E01"
DEFAULT_OBJCOPY = ROOT / "build" / "binutils" / "powerpc-eabi-objcopy"
DEFAULT_OBJDUMP = ROOT / "build" / "binutils" / "powerpc-eabi-objdump"
DEFAULT_SYMBOLS = ROOT / "config" / "GC6E01" / "symbols.build.txt"

AUTO_BASE_RE = re.compile(r"auto_\d+_([0-9A-Fa-f]{8})_")
SECTION_LABEL_RE = re.compile(r"^\S+\s*=\s*(\.\w+):0x([0-9A-Fa-f]+);")
RELOC_RE = re.compile(r"^([0-9A-Fa-f]{8})\s+(\S+)\s+(.+)$")


def run(cmd: list[str]) -> str:
    proc = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if proc.returncode != 0:
        raise RuntimeError((proc.stdout or "") + f"\ncommand failed: {' '.join(cmd)}")
    return proc.stdout


def parse_int(value, *, field: str) -> int:
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise ValueError(f"{field} must be an int or int-like string")


def resolve_path(value: str | None, *, build: Path, default_dir: str) -> Path:
    if not value:
        raise ValueError("missing object path")
    path = Path(value)
    if path.suffix in {".c", ".cpp"}:
        path = path.with_suffix(".o")
    if not path.is_absolute():
        if len(path.parts) == 1:
            path = build / default_dir / path.name
        else:
            path = ROOT / path
    return path


def target_path(item: dict, *, build: Path) -> Path:
    value = item.get("target_object") or item.get("object_path") or item.get("object")
    if not value:
        raise ValueError("missing target object")
    path = Path(str(value))
    if path.suffix in {".c", ".cpp"}:
        path = path.with_suffix(".o")
    if not path.is_absolute():
        if len(path.parts) == 1:
            path = build / "obj" / path.name
        else:
            path = ROOT / path
    return path


def source_path(item: dict, *, build: Path) -> Path:
    if item.get("source_object"):
        return resolve_path(str(item["source_object"]), build=build, default_dir="src")
    value = item.get("object")
    if not value:
        raise ValueError("missing source object; set source_object")
    path = Path(str(value))
    if path.suffix in {".c", ".cpp"}:
        path = path.with_suffix(".o")
    if not path.is_absolute():
        if len(path.parts) == 1:
            path = build / "src" / path.name
        else:
            path = ROOT / path
    return path


def section_bases(symbols: Path) -> dict[str, int]:
    bases: dict[str, int] = {}
    try:
        lines = symbols.read_text(encoding="utf-8").splitlines()
    except OSError:
        return bases
    for line in lines:
        match = SECTION_LABEL_RE.match(line.strip())
        if not match:
            continue
        section, addr_s = match.groups()
        addr = int(addr_s, 16)
        bases[section] = min(addr, bases.get(section, addr))
    return bases


def object_base(path: Path, section: str, bases: dict[str, int]) -> int | None:
    match = AUTO_BASE_RE.search(path.name)
    if match:
        return int(match.group(1), 16)
    return bases.get(section)


def dump_section(objcopy: Path, obj: Path, section: str, out: Path) -> bytes:
    run([str(objcopy), "--dump-section", f"{section}={out}", str(obj)])
    return out.read_bytes()


def relocs(objdump: Path, obj: Path, section: str, start: int, size: int) -> list[tuple[int, str, str]]:
    out = run([str(objdump), "-r", "-j", section, str(obj)])
    records: list[tuple[int, str, str]] = []
    end = start + size
    for line in out.splitlines():
        match = RELOC_RE.match(line.strip())
        if not match:
            continue
        off_s, typ, value = match.groups()
        off = int(off_s, 16)
        if start <= off < end:
            records.append((off - start, typ, " ".join(value.split())))
    return records


def verify_item(item: dict, *, build: Path, objcopy: Path, objdump: Path, bases: dict[str, int]) -> None:
    section = str(item.get("section") or "")
    if not section.startswith("."):
        raise ValueError("section must be a named ELF section such as .sdata2")
    size = parse_int(item.get("size"), field="size")
    if size <= 0:
        raise ValueError("size must be positive")

    tgt = target_path(item, build=build)
    src = source_path(item, build=build)
    if not tgt.exists():
        raise FileNotFoundError(f"target object not found: {tgt}")
    if not src.exists():
        raise FileNotFoundError(f"source object not found: {src}")

    if item.get("target_offset") is not None:
        target_offset = parse_int(item.get("target_offset"), field="target_offset")
    else:
        start = parse_int(item.get("start"), field="start")
        base = object_base(tgt, section, bases)
        target_offset = 0 if base is None else start - base
    source_offset = parse_int(item.get("source_offset", 0), field="source_offset")
    if target_offset < 0 or source_offset < 0:
        raise ValueError("computed section offset is negative")

    source_section = str(item.get("source_section") or section)
    with tempfile.TemporaryDirectory(prefix="data-progress-") as tmp:
        tmpdir = Path(tmp)
        target_bytes = dump_section(objcopy, tgt, section, tmpdir / "target.bin")
        source_bytes = dump_section(objcopy, src, source_section, tmpdir / "source.bin")

    target_slice = target_bytes[target_offset : target_offset + size]
    source_slice = source_bytes[source_offset : source_offset + size]
    if len(target_slice) != size:
        raise ValueError(f"target slice is {len(target_slice)} bytes, expected {size}")
    if len(source_slice) != size:
        raise ValueError(f"source slice is {len(source_slice)} bytes, expected {size}")
    if target_slice != source_slice:
        raise ValueError("section bytes differ")

    target_relocs = relocs(objdump, tgt, section, target_offset, size)
    source_relocs = relocs(objdump, src, source_section, source_offset, size)
    if target_relocs != source_relocs:
        raise ValueError(f"relocations differ: target={target_relocs} source={source_relocs}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("progress", nargs="?", default=str(DEFAULT_PROGRESS))
    parser.add_argument("--build", default=str(DEFAULT_BUILD))
    parser.add_argument("--objcopy", default=str(DEFAULT_OBJCOPY))
    parser.add_argument("--objdump", default=str(DEFAULT_OBJDUMP))
    parser.add_argument("--symbols", default=str(DEFAULT_SYMBOLS))
    args = parser.parse_args()

    progress = Path(args.progress)
    build = Path(args.build)
    objcopy = Path(args.objcopy)
    objdump = Path(args.objdump)
    bases = section_bases(Path(args.symbols))

    data = json.loads(progress.read_text(encoding="utf-8"))
    items = data.get("matched", []) or []
    errors: list[str] = []
    total = 0
    for idx, item in enumerate(items):
        label = item.get("name") or item.get("start") or f"entry {idx}"
        try:
            verify_item(item, build=build, objcopy=objcopy, objdump=objdump, bases=bases)
            total += parse_int(item.get("size"), field="size")
        except Exception as exc:
            errors.append(f"{label}: {exc}")

    if errors:
        print("data-progress: FAIL", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return 1

    print(f"data-progress: OK ({len(items)} entries, {total} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
