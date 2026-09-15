#!/usr/bin/env python3
"""Build the co-op mod DOL from the original, with byte-verified hooks.

Design rules this tool enforces, from colosseum_coop_architecture.md section 5:

* The original DOL is never modified in place, and its checksum must be one of
  the supported originals or the build is refused.
* Every hook site declares the exact original instruction bytes it expects. A
  mismatch aborts the build rather than producing a silently wrong patch.
* Overlay code is not placed at an address that merely "looks unused". It is
  placed at the retail arena base and the arena base is raised past it, so the
  game's own allocator can never hand out the reserved region.
* A manifest records every input and output checksum plus toolchain identity.

Usage:
    python tools/coop/build_patch.py \
        --orig orig/GC6E01/sys/main.dol \
        --hooks mods/coop/config/GC6E01-hooks.json \
        --out build/coop/main.dol
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import shutil
import struct
import subprocess
import sys
import tempfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, List

sys.path.insert(0, str(Path(__file__).resolve().parent))

from doldef import Dol  # noqa: E402
from elfread import Elf32BE  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]


# --------------------------------------------------------------------------
# helpers
# --------------------------------------------------------------------------


def sha1_file(path: Path) -> str:
    return hashlib.sha1(path.read_bytes()).hexdigest()


def sha1_bytes(data: bytes) -> str:
    return hashlib.sha1(data).hexdigest()


def parse_bytes(text: str) -> bytes:
    """Parse '3C 60 80 49' or '3c608049' into bytes."""
    cleaned = text.replace(" ", "").replace("_", "")
    return bytes.fromhex(cleaned)


class BuildError(Exception):
    pass


# --------------------------------------------------------------------------
# PowerPC encoding
# --------------------------------------------------------------------------


def encode_branch(source: int, target: int, link: bool) -> bytes:
    """Encode a b/bl from `source` to `target`."""
    delta = target - source
    if delta % 4:
        raise BuildError(f"branch target 0x{target:08X} is not 4-byte aligned")
    if not (-(1 << 25) <= delta < (1 << 25)):
        raise BuildError(
            f"branch from 0x{source:08X} to 0x{target:08X} is out of "
            f"+/-32MB range (delta 0x{delta:X})"
        )
    insn = 0x48000000 | (delta & 0x03FFFFFC) | (1 if link else 0)
    return struct.pack(">I", insn)


def split_hi_lo(value: int) -> tuple:
    """Split a 32-bit address into lis/addi immediate pair (@ha / @l)."""
    lo = value & 0xFFFF
    if lo & 0x8000:
        ha = ((value >> 16) + 1) & 0xFFFF
    else:
        ha = (value >> 16) & 0xFFFF
    return ha, lo


def encode_lis(reg: int, imm: int) -> bytes:
    return struct.pack(">I", (15 << 26) | (reg << 21) | (0 << 16) | (imm & 0xFFFF))


def encode_addi(dst: int, src: int, imm: int) -> bytes:
    return struct.pack(
        ">I", (14 << 26) | (dst << 21) | (src << 16) | (imm & 0xFFFF)
    )


# --------------------------------------------------------------------------
# payload build
# --------------------------------------------------------------------------


def build_payload(
    sources: List[Path], base: int, compilers: Path, workdir: Path
) -> tuple:
    """Assemble/compile `sources` and link them at `base`.

    Returns (image_bytes, symbols, toolchain_identity).
    """
    asm = compilers / "mwasmeppc.exe"
    cc = compilers / "mwcceppc.exe"
    ld = compilers / "mwldeppc.exe"
    for tool in (asm, ld):
        if not tool.is_file():
            raise BuildError(f"missing toolchain component: {tool}")

    objs: List[Path] = []
    for src in sources:
        if not src.is_file():
            raise BuildError(f"payload source not found: {src}")
        obj = workdir / (src.stem + ".o")
        if src.suffix == ".s":
            cmd = [str(asm), "-o", str(obj), str(src)]
        elif src.suffix == ".c":
            if not cc.is_file():
                raise BuildError(f"missing toolchain component: {cc}")
            cmd = [
                str(cc),
                "-proc", "gekko",
                "-fp", "hard",
                "-O4,p",
                "-nodefaults",
                "-msgstyle", "gcc",
                "-I-",
                "-i", str(ROOT / "mods" / "coop" / "include"),
                "-c",
                "-o", str(obj),
                str(src),
            ]
        else:
            raise BuildError(f"unsupported payload source type: {src}")

        proc = subprocess.run(cmd, capture_output=True, text=True)
        if proc.returncode != 0 or not obj.is_file():
            raise BuildError(
                f"failed to build {src.name}:\n{proc.stdout}\n{proc.stderr}"
            )
        objs.append(obj)

    lcf = workdir / "coop.lcf"
    lcf.write_text(
        "MEMORY {\n"
        f"    text : origin = 0x{base:08X}\n"
        "}\n"
        "SECTIONS {\n"
        "    GROUP: {\n"
        "        .text : {}\n"
        "        .rodata : {}\n"
        "        .data : {}\n"
        "        .bss : {}\n"
        "    } > text\n"
        "}\n"
    )

    elf_path = workdir / "coop.elf"
    proc = subprocess.run(
        [
            str(ld),
            "-nodefaults",
            "-m", "coop_entry",
            "-lcf", str(lcf),
            "-o", str(elf_path),
            *[str(o) for o in objs],
        ],
        capture_output=True,
        text=True,
    )
    if proc.returncode != 0 or not elf_path.is_file():
        raise BuildError(f"link failed:\n{proc.stdout}\n{proc.stderr}")

    elf = Elf32BE.load(elf_path)
    image_base, image = elf.flat_image()
    if image_base != base:
        raise BuildError(
            f"linked image base 0x{image_base:08X} != requested 0x{base:08X}"
        )

    toolchain = {
        "assembler": asm.name,
        "linker": ld.name,
        "compilers_dir": str(compilers.relative_to(ROOT))
        if compilers.is_relative_to(ROOT)
        else str(compilers),
        "mwasmeppc_sha1": sha1_file(asm),
        "mwldeppc_sha1": sha1_file(ld),
    }
    return image, elf.symbols(), toolchain


# --------------------------------------------------------------------------
# hook application
# --------------------------------------------------------------------------


def apply_hooks(
    dol: Dol, hooks: List[dict], symbols: Dict[str, int]
) -> List[dict]:
    """Verify and apply each hook. Returns the applied-hook record."""
    applied = []
    for hook in hooks:
        name = hook.get("name", "<unnamed>")
        try:
            address = int(str(hook["address"]), 16)
        except (KeyError, ValueError) as exc:
            raise BuildError(f"hook {name}: bad or missing address ({exc})")

        expect = parse_bytes(hook["expect"])
        actual = dol.read(address, len(expect))
        if actual != expect:
            raise BuildError(
                f"hook {name} at 0x{address:08X}: original bytes do not match.\n"
                f"  expected {expect.hex(' ').upper()}\n"
                f"  found    {actual.hex(' ').upper()}\n"
                "This original DOL is not the one this hook was written "
                "against. Refusing to patch."
            )

        kind = hook["kind"]
        if kind in ("b", "bl"):
            target_name = hook["target"]
            if target_name not in symbols:
                raise BuildError(
                    f"hook {name}: payload symbol '{target_name}' not found. "
                    f"Known: {sorted(symbols)}"
                )
            target = symbols[target_name]
            payload = encode_branch(address, target, link=(kind == "bl"))
            if len(expect) != 4:
                raise BuildError(
                    f"hook {name}: a {kind} hook must replace exactly one "
                    f"instruction, but expect is {len(expect)} bytes"
                )
            detail = f"{kind} -> {target_name} (0x{target:08X})"
        elif kind == "bytes":
            payload = parse_bytes(hook["replace"])
            if len(payload) != len(expect):
                raise BuildError(
                    f"hook {name}: replace is {len(payload)} bytes but expect "
                    f"is {len(expect)} bytes; refusing to change code size"
                )
            detail = f"bytes {payload.hex(' ').upper()}"
        else:
            raise BuildError(f"hook {name}: unknown kind '{kind}'")

        dol.write(address, payload)
        applied.append(
            {
                "name": name,
                "address": f"0x{address:08X}",
                "original": expect.hex(" ").upper(),
                "patched": payload.hex(" ").upper(),
                "detail": detail,
                "note": hook.get("note", ""),
            }
        )
    return applied


def reserve_arena(dol: Dol, reservation: dict, payload_size: int) -> dict:
    """Raise __OSArenaLo past the reserved co-op region.

    The retail boot path in OSInit loads __ArenaLo from a lis/addi immediate
    pair. Rewriting that pair moves the heap base, which is what guarantees the
    reserved block is never handed out by the allocator.
    """
    base = int(str(reservation["base"]), 16)
    size = int(str(reservation["size"]), 16)
    site = int(str(reservation["arena_lo_site"]), 16)
    expect = parse_bytes(reservation["arena_lo_expect"])

    if payload_size > size:
        raise BuildError(
            f"payload is 0x{payload_size:X} bytes but the reservation is only "
            f"0x{size:X}; raise reservation.size"
        )

    actual = dol.read(site, len(expect))
    if actual != expect:
        raise BuildError(
            f"arena reservation at 0x{site:08X}: original bytes do not match.\n"
            f"  expected {expect.hex(' ').upper()}\n"
            f"  found    {actual.hex(' ').upper()}"
        )

    old_ha, old_lo = struct.unpack_from(">HH", expect, 2)[0], 0
    old_value = struct.unpack_from(">H", expect, 2)[0] << 16
    lo_signed = struct.unpack_from(">h", expect, 6)[0]
    old_value = (old_value + lo_signed) & 0xFFFFFFFF

    if old_value != base:
        raise BuildError(
            f"arena reservation: the DOL's __ArenaLo is 0x{old_value:08X} but "
            f"reservation.base is 0x{base:08X}. The co-op block must start at "
            "the existing arena base."
        )

    new_lo = base + size
    if new_lo % 32:
        raise BuildError("reserved region must end on a 32-byte boundary")

    ha, lo = split_hi_lo(new_lo)
    # Preserve the original register allocation: lis rD / addi rD, rD, lo.
    reg = (struct.unpack_from(">I", expect, 0)[0] >> 21) & 0x1F
    patched = encode_lis(reg, ha) + encode_addi(reg, reg, lo)
    dol.write(site, patched)

    overlap = dol.overlaps_existing(base, size)
    if overlap is not None:
        raise BuildError(
            f"reserved region 0x{base:08X}+0x{size:X} overlaps existing "
            f"{overlap.kind}{overlap.index} at 0x{overlap.address:08X}"
        )

    return {
        "base": f"0x{base:08X}",
        "size": f"0x{size:X}",
        "payload_size": f"0x{payload_size:X}",
        "arena_lo_site": f"0x{site:08X}",
        "arena_lo_before": f"0x{old_value:08X}",
        "arena_lo_after": f"0x{new_lo:08X}",
        "original": expect.hex(" ").upper(),
        "patched": patched.hex(" ").upper(),
    }


# --------------------------------------------------------------------------
# main
# --------------------------------------------------------------------------


def git_commit() -> str:
    try:
        return subprocess.run(
            ["git", "rev-parse", "HEAD"],
            cwd=ROOT,
            capture_output=True,
            text=True,
            check=True,
        ).stdout.strip()
    except Exception:
        return "unknown"


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--orig", type=Path, default=ROOT / "orig/GC6E01/sys/main.dol")
    ap.add_argument(
        "--hooks", type=Path, default=ROOT / "mods/coop/config/GC6E01-hooks.json"
    )
    ap.add_argument("--out", type=Path, default=ROOT / "build/coop/main.dol")
    ap.add_argument("--manifest", type=Path, default=None)
    ap.add_argument(
        "--compilers", type=Path, default=ROOT / "build/compilers/GC/1.3"
    )
    ap.add_argument(
        "--keep-temp", action="store_true", help="retain the intermediate build dir"
    )
    args = ap.parse_args()

    if not args.orig.is_file():
        raise BuildError(f"original DOL not found: {args.orig}")
    config = json.loads(args.hooks.read_text())

    orig_sha1 = sha1_file(args.orig)
    supported = [s.lower() for s in config["supported_original_sha1"]]
    if orig_sha1.lower() not in supported:
        raise BuildError(
            f"unsupported original DOL.\n"
            f"  {args.orig} has SHA-1 {orig_sha1}\n"
            f"  supported: {', '.join(supported)}\n"
            "Refusing to patch an unrecognised executable."
        )

    workdir = Path(tempfile.mkdtemp(prefix="coop-patch-"))
    try:
        sources = [ROOT / p for p in config["payload_sources"]]
        base = int(str(config["reservation"]["base"]), 16)
        image, symbols, toolchain = build_payload(
            sources, base, args.compilers, workdir
        )

        dol = Dol.load(args.orig)
        reservation = reserve_arena(dol, config["reservation"], len(image))
        applied = apply_hooks(dol, config.get("hooks", []), symbols)
        section = dol.add_text_section(base, image)

        args.out.parent.mkdir(parents=True, exist_ok=True)
        dol.save(args.out)

        manifest = {
            "generated_utc": datetime.now(timezone.utc).isoformat(),
            "repo_commit": git_commit(),
            "original_dol": {
                "path": str(args.orig.relative_to(ROOT))
                if args.orig.is_relative_to(ROOT)
                else str(args.orig),
                "sha1": orig_sha1,
            },
            "patched_dol": {
                "path": str(args.out.relative_to(ROOT))
                if args.out.is_relative_to(ROOT)
                else str(args.out),
                "sha1": sha1_file(args.out),
                "size": args.out.stat().st_size,
            },
            "payload": {
                "sha1": sha1_bytes(image),
                "size": f"0x{len(image):X}",
                "section_slot": f"text{section.index}",
                "sources": config["payload_sources"],
                "symbols": {k: f"0x{v:08X}" for k, v in sorted(symbols.items())},
            },
            "reservation": reservation,
            "hooks": applied,
            "toolchain": toolchain,
            "hooks_config_sha1": sha1_file(args.hooks),
        }
        manifest_path = args.manifest or args.out.with_suffix(".manifest.json")
        manifest_path.write_text(json.dumps(manifest, indent=2) + "\n")

        print(f"original  {orig_sha1}  {args.orig}")
        print(f"patched   {manifest['patched_dol']['sha1']}  {args.out}")
        print(
            f"payload   0x{len(image):X} bytes at 0x{base:08X} "
            f"(slot text{section.index})"
        )
        print(
            f"arena     {reservation['arena_lo_before']} -> "
            f"{reservation['arena_lo_after']}"
        )
        print(f"hooks     {len(applied)} applied")
        for hook in applied:
            print(f"  {hook['address']}  {hook['name']}: {hook['detail']}")
        print(f"manifest  {manifest_path}")
        return 0
    finally:
        if args.keep_temp:
            print(f"temp build dir retained: {workdir}")
        else:
            shutil.rmtree(workdir, ignore_errors=True)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except BuildError as exc:
        print(f"error: {exc}", file=sys.stderr)
        sys.exit(1)
