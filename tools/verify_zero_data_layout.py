#!/usr/bin/env python3
"""Verify zero-data symbol layouts; identical zero bytes alone are insufficient.

Expectations come from the symbol map and named splits, not from candidate
declarations. String registrations retain the strict storage-only checks.
Version 2 also accepts explicitly registered mixed translation units: every
zero section must be listed, and each private symbol needs an exact native-name
mapping. The other declared sections may coexist with the checked storage;
this does not verify their contents. This checker never modifies objects or
grants matching credit.
"""

import argparse
import json
from pathlib import Path
import re
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tools.import_constant_symbol import parse_symbols, read_elf

ZERO_SECTIONS = {".bss", ".sbss", ".sbss2"}
SYMBOL = re.compile(r"^(\S+) = (\.[^:]+):0x([0-9A-Fa-f]+);\s*//\s*(.*)$")
SPLIT = re.compile(r"^\s+(\.\S+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)(.*)$")
SOURCE_NAME = re.compile(r"[A-Za-z_]\w*", re.ASCII)


def unique_keys(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError("duplicate manifest key")
        result[key] = value
    return result


def registrations(manifest_path):
    manifest = json.loads(manifest_path.read_text(), object_pairs_hook=unique_keys)
    if (not isinstance(manifest, dict) or set(manifest) != {"version", "units"}
            or type(manifest["version"]) is not int
            or manifest["version"] not in (1, 2)
            or not isinstance(manifest["units"], list) or not manifest["units"]):
        raise ValueError("invalid zero-data manifest")
    result = {}
    for entry in manifest["units"]:
        if isinstance(entry, str):
            unit, sections, local_symbols = entry, None, None
        elif (manifest["version"] == 2 and isinstance(entry, dict)
              and set(entry) == {"unit", "sections", "local_symbols"}):
            unit = entry["unit"]
            sections, local_symbols = entry["sections"], entry["local_symbols"]
            if (not isinstance(sections, list) or not sections
                    or any(not isinstance(s, str) or s not in ZERO_SECTIONS for s in sections)
                    or len(set(sections)) != len(sections)):
                raise ValueError("mixed owner must declare distinct zero-init sections")
            if (not isinstance(local_symbols, dict)
                    or any(not isinstance(k, str) or not SOURCE_NAME.fullmatch(k)
                           or not isinstance(v, str) or not SOURCE_NAME.fullmatch(v)
                           for k, v in local_symbols.items())
                    or len(set(local_symbols.values())) != len(local_symbols)):
                raise ValueError("invalid or ambiguous private-symbol mapping")
        else:
            raise ValueError("invalid zero-data registration")
        if (not isinstance(unit, str)
                or not re.fullmatch(r"(?:[A-Za-z0-9_-]+/)*[A-Za-z0-9_-]+\.c", unit)):
            raise ValueError("unsafe zero-data unit path")
        if unit in result:
            raise ValueError("invalid or duplicate zero-data units")
        result[unit] = {"sections": sections, "local_symbols": local_symbols}
    return result


def expectations(manifest_path, symbols_path, splits_path):
    units = registrations(manifest_path)
    ranges = {unit: [] for unit in units}
    all_ranges = []
    owner = None
    for line in splits_path.read_text().splitlines():
        if line and not line[0].isspace() and line.endswith(":"):
            owner = line[:-1]
        match = SPLIT.match(line)
        if match:
            section, begin, end, extra = match.groups()
            span = (section, int(begin, 16), int(end, 16))
            all_ranges.append((owner, *span))
            if owner in ranges:
                if extra.strip() or span[1] >= span[2]:
                    raise ValueError("unsupported zero-data split attributes")
                ranges[owner].append(span)
    symbols = []
    for line in symbols_path.read_text().splitlines():
        match = SYMBOL.match(line)
        if match:
            name, section, address, metadata = match.groups()
            fields = dict(re.findall(r"(\w+):([^\s]+)", metadata))
            symbols.append((name, section, int(address, 16), fields))
    result = []
    for unit, spans in ranges.items():
        registration = units[unit]
        mixed = registration["sections"] is not None
        zero_spans = [span for span in spans if span[0] in ZERO_SECTIONS]
        if mixed:
            if (len({s[0] for s in spans}) != len(spans)
                    or set(registration["sections"]) != {s[0] for s in zero_spans}
                    or len(zero_spans) == len(spans)):
                raise ValueError("mixed registration must cover every declared zero section")
        elif len(spans) != 1 or len(zero_spans) != 1:
            raise ValueError("zero-data owner must have one zero-init section")
        sections = []
        private_names = set()
        native_names = set()
        for section, start, end in zero_spans:
            for other, other_section, a, b in all_ranges:
                if other != unit and other_section == section and a < end and start < b:
                    raise ValueError("zero-data split overlaps another owner")
            selected = []
            for name, symbol_section, address, fields in symbols:
                if symbol_section != section:
                    continue
                size = int(fields.get("size", "0"), 0)
                if address < start and address + size > start:
                    raise ValueError("split bisects an existing symbol")
                if not start <= address < end:
                    continue
                if (fields.get("type") != "object" or size <= 0 or address + size > end
                        or mixed and fields.get("scope", "global") not in ("global", "local")):
                    raise ValueError("unsupported or bisected zero-data symbol")
                local = fields.get("scope") == "local"
                row = {"name": name, "offset": address - start, "size": size}
                if mixed:
                    row["info"] = 0x01 if local else 0x11
                    if local:
                        private_names.add(name)
                        if name not in registration["local_symbols"]:
                            raise ValueError("private zero-data symbol needs an exact native-name mapping")
                        row["name"] = registration["local_symbols"][name]
                elif local:
                    suffix = f"{address:08X}"
                    if not name.endswith(suffix):
                        row["name"] += "_" + suffix
                if row["name"] in native_names:
                    raise ValueError("duplicate zero-data symbol name")
                native_names.add(row["name"])
                selected.append(row)
            if not selected:
                raise ValueError("zero-data owner has no existing symbols")
            selected.sort(key=lambda row: row["offset"])
            cursor = 0
            gaps = []
            for row in selected:
                if row["offset"] < cursor:
                    raise ValueError("zero-data symbols overlap")
                if row["offset"] > cursor:
                    gaps.append({"offset": cursor, "size": row["offset"] - cursor})
                cursor = row["offset"] + row["size"]
            if cursor < end - start:
                gaps.append({"offset": cursor, "size": end - start - cursor})
            sections.append({"unit": unit, "section": section, "start": start,
                             "size": end - start, "symbols": selected, "gaps": gaps,
                             "existing_symbols": len(selected)})
        if mixed:
            if private_names != set(registration["local_symbols"]):
                raise ValueError("private-symbol mappings must name only owned local zero-data symbols")
            result.append({"unit": unit, "sections": sections,
                           "allocated_sections": [span[0] for span in spans],
                           "size": sum(row["size"] for row in sections),
                           "existing_symbols": sum(row["existing_symbols"] for row in sections)})
        else:
            result.extend(sections)
    return result


def check_section(sections, symbols, expected):
    errors = []
    indices = [i for i, section in enumerate(sections)
               if section["name"] == expected["section"]]
    if len(indices) != 1:
        return ["expected exactly one declared zero-init section"]
    index = indices[0]
    fields = sections[index]["fields"]
    if fields[1] != 8 or not fields[2] & 2 or fields[2] & 4:
        errors.append("expected allocated non-executable SHT_NOBITS storage")
    if fields[5] != expected["size"]:
        errors.append(f"section size {fields[5]} != {expected['size']}")
    alignment = fields[8]
    if not alignment or alignment & (alignment - 1) or expected["start"] % alignment:
        errors.append("compiled section alignment cannot occupy the retail start")
    for i, section in enumerate(sections):
        f = section["fields"]
        if f[1] in (4, 9) and f[7] == index and f[5]:
            errors.append("zero-init section unexpectedly has relocations")
    names = {row["name"] for row in expected["symbols"]}
    for row in expected["symbols"]:
        found = [symbol for symbol in symbols if symbol["name"] == row["name"]
                 and symbol["shndx"] != 0]
        if len(found) != 1:
            errors.append(f"{row['name']}: expected one definition")
            continue
        symbol = found[0]
        actual = (symbol["shndx"], symbol["value"], symbol["size"], symbol["info"])
        wanted = (index, row["offset"], row["size"], row.get("info", 0x11))
        if actual != wanted:
            errors.append(f"{row['name']}: section/offset/size/binding {actual} != {wanted}")
        if symbol["other"]:
            errors.append(f"{row['name']}: unexpected symbol visibility")
    # A compiler may leave map-derived gaps implicit. Any explicit padding
    # definition must name its own address and fit wholly inside one such gap.
    # It may not cover a real object, overlap another padding symbol, or create
    # extra storage outside the independently established section extent.
    padding_ranges = []
    for symbol in symbols:
        if (symbol["shndx"] != index or not symbol["size"]
                or symbol["info"] & 15 == 3 or symbol["name"] in names):
            continue
        match = re.fullmatch(r"padding_([0-9A-F]{8})", symbol["name"])
        start, end = symbol["value"], symbol["value"] + symbol["size"]
        valid = (match is not None and symbol["info"] == 0x11
                 and int(match[1], 16) == expected["start"] + start
                 and any(gap["offset"] <= start and end <= gap["offset"] + gap["size"]
                         for gap in expected.get("gaps", [])))
        if not valid:
            errors.append(f"unexpected storage symbol: {symbol['name']}")
        elif any(a < end and start < b for a, b in padding_ranges):
            errors.append(f"overlapping padding symbol: {symbol['name']}")
        else:
            padding_ranges.append((start, end))
    return errors


def check_object(blob, expected):
    _, sections = read_elf(blob)
    _, symbols = parse_symbols(sections)
    zero_sections = expected.get("sections", [expected])
    zero_names = {row["section"] for row in zero_sections}
    allowed = set(expected.get("allocated_sections", zero_names))
    mixed = "sections" in expected
    errors = []
    seen = set()
    for section in sections:
        fields = section["fields"]
        if not fields[2] & 2 or not fields[5]:
            continue
        name = section["name"]
        if name in seen:
            errors.append("duplicate allocated section")
        seen.add(name)
        if name not in allowed:
            errors.append("unexpected additional allocated section")
        if mixed and fields[1] == 8 and name not in zero_names:
            errors.append("unregistered zero-init storage section")
    for row in zero_sections:
        section_errors = check_section(sections, symbols, row)
        errors.extend(f"{row['section']}: {error}" if mixed else error
                      for error in section_errors)
    return errors


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=ROOT / "config/GC6E01/zero_data_units.json")
    parser.add_argument("--symbols", type=Path, default=ROOT / "config/GC6E01/symbols.txt")
    parser.add_argument("--splits", type=Path, default=ROOT / "config/GC6E01/splits.txt")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build/GC6E01")
    parser.add_argument("--metadata-only", action="store_true",
                        help="Validate declared ownership only; no compiler/layout proof")
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    expected = expectations(args.manifest, args.symbols, args.splits)
    result = {"metadata_valid": True, "objects_verified": 0,
              "compiled_layout_valid": None, "units": [],
              "total_bytes": sum(row["size"] for row in expected),
              "expected_zero_sections": sum(len(row.get("sections", [row])) for row in expected),
              "expected_symbols": sum(row["existing_symbols"] for row in expected)}
    if not args.metadata_only:
        for row in expected:
            path = args.build_dir / "src" / Path(row["unit"]).with_suffix(".o")
            try:
                errors = check_object(path.read_bytes(), row)
            except (OSError, ValueError, IndexError, struct.error) as exc:
                errors = [type(exc).__name__]
            result["units"].append({"unit": row["unit"], "errors": errors})
            result["objects_verified"] += not errors
        result["compiled_layout_valid"] = result["objects_verified"] == len(expected)
    summary = {key: value for key, value in result.items() if key != "units"}
    summary["expected_units"] = len(expected)
    print(json.dumps(summary, indent=2))
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2) + "\n")
    else:
        for row in result["units"]:
            if row["errors"]:
                print(row["unit"], row["errors"][:8])
    return 1 if result["compiled_layout_valid"] is False else 0


if __name__ == "__main__":
    raise SystemExit(main())
