#!/usr/bin/env python3
"""Compile typed battle programs to a normal PowerPC data object.

Inputs are authored JSON programs, their source catalog, and the public symbol
map. No original executable, extracted section, target object, or byte dump is an
input. GNU as emits ordinary data symbols and R_PPC_ADDR32 relocations; there is
no binary patching or native instruction generation.

Every address operand is a relocation in the linked object, so a modified link
layout moves branch targets, dispatch entries and memory operands with their
symbols. The interpreter packs most of those operands at unaligned offsets.
dtk's split target objects can only carry word-aligned relocations, so an
optional comparison object (read by objdiff, never linked) writes unaligned
operands as their public-map addresses instead. Both objects have identical
bytes in the original layout.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import json
from pathlib import Path
import re
import subprocess
import tempfile
from typing import Any

if __package__:
    from .battle_script_format import (BATTLERS, COMMANDS, COMPARISONS, FIELD_WIDTH, STATS,
                                       STATUS_FAMILIES, STATUS_GET_FIELDS, STATUS_SET_FIELDS)
else:
    from battle_script_format import (BATTLERS, COMMANDS, COMPARISONS, FIELD_WIDTH, STATS,
                                      STATUS_FAMILIES, STATUS_GET_FIELDS, STATUS_SET_FIELDS)


FORMAT = "colosseum-battle-source-v2"
CATALOG_FORMAT = "colosseum-battle-catalog-v2"
IDENTIFIER = re.compile(r"[A-Za-z_][A-Za-z_0-9]*\Z")
SOURCE_FILENAME = re.compile(r"[A-Za-z_0-9]+\.battle\.json\Z")
UNIT_NAME = re.compile(r"main/[A-Za-z_0-9]+(?:/[A-Za-z_0-9]+)*(?:\.c)?\Z")
MAP_ROW = re.compile(
    r"^(\S+) = ([^:]+):0x([0-9A-Fa-f]+);.*\btype:(\w+).*\bsize:0x([0-9A-Fa-f]+)"
)
VIEWS = ("link", "comparison")
# legacy name: (program, program address, dereferenced wire address)
LEGACY_ADDRESSES = {
    "wsCmd46_pointer_ten": ("lbl_80378AF1", 0x80378AF1, 10),
    "wsCmd46_pointer_eleven": ("lbl_80378B00", 0x80378B00, 11),
    "wsCmd46_pointer_twelve": ("lbl_80378B0F", 0x80378B0F, 12),
    "wsCmd46_pointer_thirteen": ("lbl_80378B1E", 0x80378B1E, 13),
}


class SourceError(ValueError):
    """Invalid program source; no output object should be installed."""


def require(condition: bool, reason: str) -> None:
    if not condition:
        raise SourceError(reason)


def fields(value: Any, required: set[str], optional: set[str], what: str) -> dict:
    require(isinstance(value, dict), f"{what}: expected an object")
    require(required <= value.keys(), f"{what}: missing required fields {sorted(required)}")
    require(value.keys() <= required | optional, f"{what}: unexpected fields")
    return value


def integer(value: Any, what: str, low: int, high: int) -> int:
    require(type(value) is int and low <= value <= high,
            f"{what}: expected an integer in {low}..{high}")
    return value


def identifier(value: Any, what: str) -> str:
    require(isinstance(value, str) and IDENTIFIER.fullmatch(value) is not None,
            f"{what}: invalid identifier")
    return value


def enum(value: Any, values: dict[str, int], what: str, numeric_domain: set[int]) -> int:
    if isinstance(value, str):
        require(value in values, f"{what}: unknown enum value {value!r}")
        return values[value]
    require(type(value) is int and value in numeric_domain, f"{what}: invalid enum value")
    return value


@dataclass(frozen=True)
class Symbol:
    address: int
    size: int
    section: str
    kind: str = "object"


def parse_symbols(text: str) -> dict[str, Symbol]:
    symbols = {}
    for line in text.splitlines():
        row = MAP_ROW.match(line)
        if row is None:
            continue
        name, section, address, kind, size = row.groups()
        # Compiler-local constants (@50, etc.) are deliberately not addressable
        # in this source language and can repeat in the public object map.
        if IDENTIFIER.fullmatch(name) is None or "scope:local" in line:
            continue
        require(name not in symbols, f"duplicate symbol map definition: {name}")
        symbols[name] = Symbol(int(address, 16), int(size, 16), section, kind)
    require(bool(symbols), "symbol map contains no sized definitions")
    return symbols


def apply_native_bounds(symbols: dict[str, Symbol], registry: list) -> None:
    """Use a registered native C array's full extent instead of its map fragment.

    The public map keeps historical fragments such as lbl_80478D78 (one byte)
    whose native C object is larger; data-alias registries record those bounds.
    """
    require(isinstance(registry, list), "native bounds registry must be a list")
    for group in registry:
        require(isinstance(group, dict) and {"base", "size"} <= group.keys(), "invalid native bounds")
        symbol = symbols.get(group["base"])
        require(symbol is not None and type(group["size"]) is int and group["size"] >= symbol.size,
                f"invalid native bounds for {group['base']}")
        symbols[group["base"]] = Symbol(symbol.address, group["size"], symbol.section, symbol.kind)


@dataclass
class Program:
    """One owned data object: a command program, a dispatch table or storage."""
    symbol: str
    kind: str
    body: list
    labels: dict[str, int]
    positions: list[int]
    size: int
    padding: int = 0


@dataclass
class Owner:
    filename: str
    unit: str
    alignment: int
    start: int
    programs: list[Program]
    size: int


class Compiler:
    """Validate a source catalog before serializing any selected owner.

    A branch or dispatch entry names an entry label, never a numeric offset.
    Labels are computed from command widths, so every reference lands on an
    instruction boundary of an owned program.
    """

    def __init__(self, documents: dict[str, dict], catalog: dict,
                 symbols: dict[str, Symbol]):
        fields(catalog, {"format", "sources"}, {"description"}, "catalog")
        require(catalog["format"] == CATALOG_FORMAT, "unsupported battle catalog format")
        require(isinstance(catalog["sources"], list) and catalog["sources"], "empty source catalog")
        require(all(isinstance(name, str) and SOURCE_FILENAME.fullmatch(name)
                    for name in catalog["sources"]), "invalid catalog source filename")
        require(len(set(catalog["sources"])) == len(catalog["sources"]), "duplicate catalog source")
        require(set(documents) == set(catalog["sources"]), "catalog/source set differs")
        self.symbols = symbols
        self.owners: dict[str, Owner] = {}
        self.programs: dict[str, Program] = {}
        self.entries: dict[str, dict[str, int]] = {}
        occupied = []
        for filename in catalog["sources"]:
            doc = documents[filename]
            fields(doc, {"format", "catalog", "unit", "alignment", "objects"},
                   {"description"}, filename)
            require(doc["format"] == FORMAT, f"{filename}: unsupported source format")
            require(doc["catalog"] == "catalog.json", "source must use its sibling catalog.json")
            require(isinstance(doc["unit"], str) and UNIT_NAME.fullmatch(doc["unit"]),
                    "invalid source unit name")
            require(doc["unit"] not in {o.unit for o in self.owners.values()}, "duplicate source unit")
            alignment = integer(doc["alignment"], "section alignment", 1, 32)
            require(alignment & (alignment - 1) == 0, "alignment must be a power of two")
            require(isinstance(doc["objects"], list) and doc["objects"], "empty program owner")
            programs = []
            start = None
            cursor = None
            for source in doc["objects"]:
                require(isinstance(source, dict) and "symbol" in source, "object: expected a symbol")
                name = identifier(source["symbol"], "object symbol")
                require(name not in self.programs, f"duplicate object definition: {name}")
                symbol = self.symbol(name)
                require(symbol.section == ".data" and symbol.kind == "object" and symbol.size > 0,
                        f"{name}: object must be a nonempty .data object")
                if start is None:
                    start = cursor = symbol.address
                    require(start % alignment == 0, f"{name}: section start violates alignment")
                require(symbol.address == cursor, f"{name}: source ownership has a gap or overlap")
                program = self.object(name, source)
                require(program.size == symbol.size,
                        f"{name}: object extent {program.size} differs from mapped object size {symbol.size}")
                self.programs[name] = program
                if program.kind == "program":
                    self.entries[name] = program.labels
                programs.append(program)
                occupied.append((symbol.address, symbol.address + symbol.size, name))
                cursor += symbol.size
            self.owners[filename] = Owner(filename, doc["unit"], alignment, start, programs,
                                          cursor - start)
        occupied.sort()
        require(all(left[1] <= right[0] for left, right in zip(occupied, occupied[1:])),
                "source program owners overlap")
        # Check operands in every catalog member, including cross-owner branches,
        # before an object for any one owner can be emitted.
        for owner in self.owners.values():
            self.assembly(owner.filename)

    def object(self, name: str, source: dict) -> Program:
        if "body" in source:
            fields(source, {"symbol", "body"}, {"description"}, name)
            require(isinstance(source["body"], list) and source["body"], f"{name}: empty body")
            labels = {"entry": 0}
            positions = []
            position = 0
            for node in source["body"]:
                require(isinstance(node, dict) and "command" in node, f"{name}: expected a command")
                require(isinstance(node["command"], str) and node["command"] in COMMANDS,
                        f"{name}: unknown command")
                command = COMMANDS[node["command"]]
                fields(node, {"command", *(field for field, _ in command.fields)},
                       {"label", "note"}, f"{name}/{command.name}")
                if "note" in node:
                    require(isinstance(node["note"], str), "command note must be text")
                if "label" in node:
                    label = identifier(node["label"], "entry label")
                    require(label not in labels, f"{name}: duplicate/reserved entry label {label}")
                    labels[label] = position
                positions.append(position)
                position += command.size
            return Program(name, "program", source["body"], labels, positions, position)
        if "table" in source:
            # A dispatch table of script entry addresses, e.g. the move-effect
            # table indexed by wsRunScript callers. Padding covers the alignment
            # bytes the mapped object includes after its last entry.
            fields(source, {"symbol", "table"}, {"description", "padding"}, name)
            require(isinstance(source["table"], list) and source["table"], f"{name}: empty table")
            padding = integer(source.get("padding", 0), "table padding", 0, 3)
            return Program(name, "table", source["table"], {}, [4 * i for i in range(len(source["table"]))],
                           4 * len(source["table"]) + padding, padding)
        fields(source, {"symbol", "zero_fill"}, {"description"}, name)
        size = integer(source["zero_fill"], "zero-filled storage size", 1, 0x1000000)
        return Program(name, "zero_fill", [], {}, [], size)

    def symbol(self, name: str) -> Symbol:
        identifier(name, "symbol reference")
        require(name in self.symbols, f"unresolved symbol: {name}")
        return self.symbols[name]

    def scalar(self, kind: str, value: Any, what: str) -> int:
        if kind == "comparison":
            return enum(value, COMPARISONS, what, set(COMPARISONS.values()))
        if kind == "stat":
            return enum(value, STATS, what, set(STATS.values()))
        if kind == "family":
            return enum(value, STATUS_FAMILIES, what, set(STATUS_FAMILIES.values()))
        if kind == "battler":
            return enum(value, BATTLERS, what, set(range(30)))
        if kind == "zero32":
            return integer(value, what, 0, 0)
        return integer(value, what, 0, (1 << (FIELD_WIDTH[kind] * 8)) - 1)

    def address(self, value: Any, kind: str, nullable: bool, program: Program,
                position: int, field: str, required_width: int) -> tuple[str | None, int, dict | None]:
        """Return a relocation expression (or None for a constant) and its wire value."""
        what = f"{program.symbol}+{position}/{field}"
        if value is None:
            require(nullable, f"{what}: required address cannot be null")
            return None, 0, None
        require(isinstance(value, dict), f"{what}: address must be symbolic")
        if "legacy_address" in value:
            fields(value, {"legacy_address"}, set(), what)
            # These four sibling commands actually ask wsCmd46 to dereference
            # addresses ten to thirteen. Preserve their wire behavior; do not
            # reinterpret them as immediate animation ids or generalize them
            # into a raw-byte escape.
            site = LEGACY_ADDRESSES.get(value["legacy_address"])
            require(site is not None and site[:2] == (program.symbol, self.symbol(program.symbol).address)
                    and (position, kind, field) == (0, "mem8", "animation"),
                    f"{what}: unsupported legacy address")
            return None, site[2], None
        if kind in {"branch", "ignored_branch"}:
            fields(value, {"script", "entry"}, set(), what)
            target = identifier(value["script"], "branch script")
            label = identifier(value["entry"], "branch label")
            require(target in self.entries and label in self.entries[target],
                    f"{what}: branch does not name an instruction entry")
            offset = self.entries[target][label]
        else:
            fields(value, {"symbol"}, {"addend"}, what)
            target = identifier(value["symbol"], "memory symbol")
            offset = integer(value.get("addend", 0), "memory addend", 0, 0xFFFFFFFF)
        symbol = self.symbol(target)
        require(symbol.kind == "object" and symbol.section in
                {".data", ".rodata", ".sdata", ".sdata2", ".bss", ".sbss", ".sbss2"},
                f"{what}: address must refer to a data object")
        require(0 <= offset < symbol.size and offset + required_width <= symbol.size,
                f"{what}: operand access exceeds the named object")
        address = symbol.address + offset
        require(address <= 0xFFFFFFFF, f"{what}: wire address overflows")
        edge = None
        if kind == "branch":
            edge = {"source": program.symbol, "offset": position, "field": field,
                    "target": target, "entry": value["entry"]}
        return target + (f"+{offset}" if offset else ""), address, edge

    def assembly(self, filename: str, view: str = "link") -> tuple[str, dict]:
        require(filename in self.owners, "source is not in the battle catalog")
        require(view in VIEWS, "unknown object view")
        owner = self.owners[filename]
        lines = ["# Generated from typed battle source; do not edit.",
                 '.section .data,"aw",@progbits', f".balign {owner.alignment}"]
        relocations = []
        resolved = []
        edges = []
        command_count = 0
        section_offset = 0

        def emit_address(expression: str | None, value: int, offset: int) -> None:
            # Unaligned relocations exist only in the linked object; the
            # comparison object mirrors what the split target can express.
            if expression is None:
                lines.append(f".long {value}")
            elif view == "comparison" and (owner.start + offset) % 4:
                lines.append(f".long {value}")
                resolved.append({"offset": offset, "expression": expression, "value": value})
            else:
                lines.append(f".long {expression}")
                relocations.append({"offset": offset, "expression": expression, "type": 1})

        for program in owner.programs:
            lines.extend((f".global {program.symbol}", f".type {program.symbol},@object",
                          program.symbol + ":"))
            if program.kind == "zero_fill":
                lines.append(f".space {program.size}")
            elif program.kind == "table":
                for index, entry in enumerate(program.body):
                    expression, value, _ = self.address(entry, "branch", False, program,
                                                        4 * index, f"table[{index}]", 1)
                    emit_address(expression, value, section_offset + 4 * index)
                if program.padding:
                    lines.append(f".space {program.padding}")
            for position, node in zip(program.positions, program.body) if program.kind == "program" else ():
                command = COMMANDS[node["command"]]
                command_count += 1
                lines.extend((f"# {command.name}", f".byte {command.opcode}"))
                field_offset = 1
                if command.opcode >= 0xF8:
                    family = self.scalar("family", node["family"], "status family")
                    field_id = self.scalar("status_field", node["field"], "status field")
                    domains = STATUS_GET_FIELDS if command.opcode <= 0xFA else STATUS_SET_FIELDS
                    require(field_id in domains[family], "unknown field for status family/direction")
                for name, wire_kind in command.fields:
                    kind = wire_kind.rstrip("?")
                    width = FIELD_WIDTH[kind]
                    if kind.startswith("mem") or kind in {"branch", "ignored_branch"}:
                        access = int(kind[3:]) // 8 if kind.startswith("mem") else 1
                        if kind.startswith("mem") and command.name in {"copy_memory", "jump_if_memory_equal", "jump_if_memory_different"}:
                            access = integer(node["count"], "memory transfer count", 0, 255)
                        expression, value, edge = self.address(
                            node[name], kind, wire_kind.endswith("?"), program, position, name, access)
                        emit_address(expression, value, section_offset + position + field_offset)
                        if edge is not None:
                            edges.append(edge)
                    else:
                        scalar = self.scalar(kind, node[name], f"{program.symbol}/{command.name}/{name}")
                        directive = {1: ".byte", 2: ".short", 4: ".long"}[width]
                        lines.append(f"{directive} {scalar}")
                    field_offset += width
            lines.append(f".size {program.symbol}, .-{program.symbol}")
            section_offset += program.size
        return "\n".join(lines) + "\n", {
            "unit": owner.unit, "size": owner.size, "alignment": owner.alignment,
            "symbols": [{"name": p.symbol, "size": p.size} for p in owner.programs],
            "commands": command_count, "relocations": relocations, "resolved": resolved,
            "edges": edges,
        }


def no_duplicate_keys(pairs: list[tuple[str, Any]]) -> dict:
    result = {}
    for key, value in pairs:
        require(key not in result, f"duplicate JSON field: {key}")
        result[key] = value
    return result


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(), object_pairs_hook=no_duplicate_keys)


def load_source(source: Path, symbols_path: Path,
                native_bounds: list[Path] = ()) -> tuple[Compiler, list[Path]]:
    source = source.resolve()
    catalog_path = source.parent / "catalog.json"
    catalog = read_json(catalog_path)
    require(isinstance(catalog, dict) and isinstance(catalog.get("sources"), list), "invalid catalog")
    require(all(isinstance(name, str) and SOURCE_FILENAME.fullmatch(name)
                for name in catalog["sources"]), "invalid source filename")
    documents = {}
    inputs = [catalog_path, symbols_path.resolve()]
    for name in catalog["sources"]:
        path = (source.parent / name).resolve()
        require(path.parent == source.parent, "catalog source leaves source directory")
        documents[name] = read_json(path)
        inputs.append(path)
    require(source in inputs, "requested source is not catalogued")
    symbols = parse_symbols(symbols_path.read_text())
    for path in native_bounds:
        apply_native_bounds(symbols, read_json(path))
        inputs.append(path.resolve())
    return Compiler(documents, catalog, symbols), inputs


def assemble(assembler: Path, assembly: str, output: Path, listing: Path, directory: str) -> None:
    temporary_asm = Path(directory) / f"{output.stem}.s"
    temporary_obj = Path(directory) / f"{output.stem}.o"
    temporary_asm.write_text(assembly)
    subprocess.run([str(assembler.resolve()), "-mgekko", "-o", str(temporary_obj),
                    str(temporary_asm)], check=True)
    temporary_obj.replace(output)
    listing.write_text(assembly)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--symbols", type=Path, required=True)
    parser.add_argument("--assembler", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--assembly", type=Path)
    parser.add_argument("--native-bounds", type=Path, action="append", default=[],
                        help="data-alias registry recording native C array extents")
    parser.add_argument("--comparison-output", type=Path,
                        help="also write the objdiff comparison object")
    args = parser.parse_args()
    compiler, inputs = load_source(args.source, args.symbols, args.native_bounds)
    outputs = [(args.output.resolve(), (args.assembly or args.output.with_suffix(".s")).resolve(), "link")]
    if args.comparison_output is not None:
        comparison = args.comparison_output.resolve()
        outputs.append((comparison, comparison.with_suffix(".s"), "comparison"))
    paths = [path for output in outputs for path in output[:2]]
    require(all(output.suffix == ".o" and listing.suffix == ".s" for output, listing, _ in outputs),
            "expected .o/.s output paths")
    require(not set(paths) & set(inputs) and len(set(paths)) == len(paths),
            "output would overwrite a source input")
    views = [(output, listing, compiler.assembly(args.source.name, view)[0])
             for output, listing, view in outputs]
    # Install only after the full source catalog validates and GNU as succeeds.
    for output, listing, assembly in views:
        output.parent.mkdir(parents=True, exist_ok=True)
        listing.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="battle-source-", dir=output.parent) as directory:
            assemble(args.assembler, assembly, output, listing, directory)


if __name__ == "__main__":
    main()
