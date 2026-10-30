#!/usr/bin/env python3
"""Compare linked DOL data symbol addresses with the existing retail map.

Complete, source-backed, data-only main units are selected by default. An
explicit --include-unit may also select a complete mixed main owner. Incomplete
candidates still linked from originals are explicitly skipped. Output contains
symbol names, addresses, sizes and comparison status, never section payloads.
Missing/dead-stripped symbols are diagnostic observations, not matching credit.
An optional array-alias registration directory preserves original map fragment
extents while checking the real complete C arrays and zero-size linked labels.
"""

import argparse
import json
from pathlib import Path
import re
import struct


DATA_SECTIONS = {".ctors", ".dtors", ".rodata", ".data", ".bss",
                 ".sdata", ".sbss", ".sdata2", ".sbss2"}
SYMBOL = re.compile(r"^(\S+) = (\.[^:]+):0x([0-9A-Fa-f]+);\s*//\s*(.*)$")


def linked_symbols(blob):
    def bounded(offset, size):
        if offset < 0 or size < 0 or offset + size > len(blob):
            raise ValueError("Linked ELF range exceeds file")
        return blob[offset:offset + size]

    header = struct.unpack(">16sHHIIIIIHHHHHH", bounded(0, 52))
    if header[0][:7] != b"\x7fELF\x01\x02\x01" or header[1:4] != (2, 20, 1) \
            or header[8] != 52 or header[11] != 40 or not header[12]:
        raise ValueError("Expected a linked big-endian ELF32 PowerPC executable")
    sections = list(struct.iter_unpack(">10I", bounded(header[6], header[12] * 40)))
    tables = [section for section in sections if section[1] == 2]
    if len(tables) != 1:
        raise ValueError("Expected one linked symbol table")
    table = tables[0]
    if table[9] != 16 or table[5] % 16 or table[6] >= len(sections) \
            or sections[table[6]][1] != 3:
        raise ValueError("Invalid linked symbol table")
    names_section = sections[table[6]]
    names = bounded(names_section[4], names_section[5])
    output = {}
    for name_offset, address, size, info, _, section_index in struct.iter_unpack(
            ">IIIBBH", bounded(table[4], table[5])):
        if not section_index or info & 15 not in (0, 1):
            continue
        if section_index >= len(sections) and section_index != 0xFFF1:
            raise ValueError("Invalid linked data symbol section")
        if name_offset >= len(names):
            raise ValueError("Invalid linked symbol name")
        end = names.find(b"\0", name_offset)
        if end < 0:
            raise ValueError("Unterminated linked symbol name")
        name = names[name_offset:end].decode("utf-8")
        if name:
            definition = {"address": address, "size": size}
            definitions = output.setdefault(name, [])
            if definition not in definitions:
                definitions.append(definition)
    return output


def map_symbols(text):
    output = []
    for line in text.splitlines():
        match = SYMBOL.match(line)
        if not match:
            continue
        name, section, address, metadata = match.groups()
        fields = dict(re.findall(r"(\w+):([^\s]+)", metadata))
        if section not in DATA_SECTIONS or fields.get("type") != "object":
            continue
        address, size = int(address, 16), int(fields.get("size", "0"), 0)
        if not size:
            continue
        alias = name
        suffix = f"{address:08X}"
        if fields.get("scope") == "local" and not name.endswith(suffix):
            alias += "_" + suffix
        output.append({"name": name, "alias": alias, "section": section,
                       "address": address, "size": size})
    return output


def load_array_aliases(directory, expected):
    """Validate explicit source registrations against all original map extents.

    These are the same scoped registrations validated against native objects by
    source_data_aliases.py. This diagnostic independently checks their map and
    owner bounds; it never infers aliases from a linked size mismatch.
    """
    def require(condition, message):
        if not condition:
            raise ValueError(message)

    def unique_fields(pairs):
        result = {}
        for key, value in pairs:
            require(key not in result, "Duplicate array-alias JSON field")
            result[key] = value
        return result

    require(directory.is_dir(), "Array-alias directory is missing")
    paths = sorted(directory.glob("*.json"))
    require(paths, "Array-alias directory has no registrations")
    by_name = {}
    for symbol in expected:
        by_name.setdefault(symbol["name"], []).append(symbol)

    def original(name):
        require(isinstance(name, str) and re.fullmatch(r"[A-Za-z_]\w*", name),
                "Invalid array-alias symbol name")
        definitions = by_name.get(name, [])
        require(len(definitions) == 1, "Unmapped or ambiguous array-alias symbol")
        result = definitions[0]
        require(result["section"] == ".sdata" and result["alias"] == name,
                "Array aliases require original global small-data names")
        return result

    output, host_ranges = {}, []
    for path in paths:
        require(path.is_file() and path.resolve().parent == directory.resolve()
                and re.fullmatch(r"sdata_[0-9A-Fa-f]{8}\.json", path.name),
                "Unexpected array-alias registration path")
        groups = json.loads(path.read_text(), object_pairs_hook=unique_fields)
        require(isinstance(groups, list) and groups, "Expected array-alias groups")
        for group in groups:
            require(isinstance(group, dict) and set(group) == {"base", "size", "aliases"},
                    "Malformed array-alias group")
            base, size, aliases = group["base"], group["size"], group["aliases"]
            host = original(base)
            require(type(size) is int and host["size"] < size <= 0xFFFFFFFF
                    and isinstance(aliases, list) and aliases,
                    "Invalid complete source-array extent")
            start, end = host["address"], host["address"] + size
            require(end <= 0x100000000 and not any(a < end and start < b
                                                  for a, b in host_ranges),
                    "Overlapping or out-of-range source arrays")
            host_ranges.append((start, end))
            members = {base: host}
            for alias in aliases:
                require(isinstance(alias, dict) and set(alias) == {"name", "offset", "size"},
                        "Malformed interior alias")
                name, offset, extent = alias["name"], alias["offset"], alias["size"]
                mapped = original(name)
                require(name not in members and type(offset) is int
                        and type(extent) is int and 0 < offset < size
                        and 0 < extent <= size - offset
                        and (mapped["address"], mapped["size"]) == (start + offset, extent),
                        "Interior alias exceeds its host or differs from the original map")
                members[name] = mapped
            spans = sorted((s["address"], s["address"] + s["size"])
                           for s in members.values())
            require(all(a[1] <= b[0] for a, b in zip(spans, spans[1:])),
                    "Original array fragments overlap")
            for mapped in expected:
                if mapped["section"] != ".sdata":
                    continue
                a, b = mapped["address"], mapped["address"] + mapped["size"]
                if a < end and start < b:
                    require(mapped["name"] in members and start <= a < b <= end,
                            "Unregistered mapped object intersects the source array")
            for name, mapped in members.items():
                require(name not in output, "Duplicate array-alias registration")
                output[name] = {
                    "registration": path.name,
                    "owner": "main/game/data/" + path.stem,
                    "source_path": "src/game/data/" + path.stem + ".c",
                    "kind": "complete_array" if name == base else "interior_label",
                    "host": base, "host_address": start, "host_size": size,
                    "expected_address": mapped["address"],
                    "expected_source_size": size if name == base else 0,
                    "original_fragment_size": mapped["size"],
                }
    return output


def diagnose(report, expected, actual, array_aliases=None, include_units=()):
    result = {"schema_version": 1, "payload_bytes_included": False,
              "scope": "complete_source_data_only_main_units",
              "units": [], "skipped_incomplete_units": []}
    totals = {key: 0 for key in ("match", "address_mismatch", "size_mismatch",
                                "missing", "ambiguous")}
    aliases = array_aliases or {}
    requested = set(include_units)
    if requested:
        result["scope"] = "complete_source_data_only_main_units_and_explicit_main_units"
        result["explicit_units"] = sorted(requested)
        for name in requested:
            owners = [unit for unit in report["units"] if unit["name"] == name]
            if len(owners) != 1:
                raise ValueError("Explicit source owner is missing or ambiguous")
            owner = owners[0]
            measures, metadata = owner["measures"], owner.get("metadata", {})
            if (not name.startswith("main/") or not metadata.get("source_path")
                    or not metadata.get("complete") or not int(measures.get("total_data", 0))
                    or int(measures.get("complete_data", 0)) != int(measures["total_data"])
                    or int(measures.get("complete_code", 0)) != int(measures.get("total_code", 0))):
                raise ValueError("Explicit unit is not a complete source-backed main data owner")
    if aliases:
        # A registration must belong wholly to its named source owner, even if
        # that owner is incomplete and will be skipped by the address report.
        for entry in aliases.values():
            owners = [u for u in report["units"] if u["name"] == entry["owner"]]
            if len(owners) != 1:
                raise ValueError("Array-alias owner is missing or ambiguous")
            owner = owners[0]
            if owner.get("metadata", {}).get("source_path") != entry["source_path"] \
                    or int(owner["measures"].get("total_code", 0)):
                raise ValueError("Array-alias registration is not a native data-only source owner")
            sections = [s for s in owner.get("sections", []) if s["name"] == ".sdata"]
            if len(sections) != 1:
                raise ValueError("Array-alias owner has no unique small-data section")
            start = int(sections[0]["metadata"]["virtual_address"])
            end = start + int(sections[0]["size"])
            if not start <= entry["host_address"] < entry["host_address"] + entry["host_size"] <= end:
                raise ValueError("Complete source array exceeds its reported physical owner")
        result["array_aliases"] = {
            "registrations": sorted({entry["registration"] for entry in aliases.values()}),
            "complete_arrays": sum(entry["kind"] == "complete_array" for entry in aliases.values()),
            "interior_labels": sum(entry["kind"] == "interior_label" for entry in aliases.values()),
        }
    for unit in report["units"]:
        measures, metadata = unit["measures"], unit.get("metadata", {})
        if not unit["name"].startswith("main/") or not metadata.get("source_path") \
                or int(measures.get("total_code", 0)) != 0 and unit["name"] not in requested \
                or not int(measures.get("total_data", 0)):
            continue
        if not metadata.get("complete") or int(measures.get("complete_data", 0)) \
                != int(measures["total_data"]):
            result["skipped_incomplete_units"].append(unit["name"])
            continue
        rows = []
        for section in unit.get("sections", []):
            if section["name"] not in DATA_SECTIONS:
                continue
            start = int(section["metadata"]["virtual_address"])
            end = start + int(section["size"])
            for symbol in expected:
                if symbol["section"] != section["name"] or not start <= symbol["address"] < end:
                    continue
                if symbol["address"] + symbol["size"] > end:
                    raise ValueError("Reported owner bisects a mapped symbol")
                name = symbol["alias"]
                definitions = actual.get(name, [])
                # Source objects may retain a unique local basename while
                # extracted objects export DTK's address-qualified alias.
                if not definitions and name != symbol["name"]:
                    name = symbol["name"]
                    definitions = actual.get(name, [])
                row = {"name": symbol["name"], "expected_alias": symbol["alias"],
                       "section": symbol["section"],
                       "expected_address": symbol["address"],
                       "expected_size": symbol["size"],
                       "linked_name": name if definitions else None,
                       "actual_definitions": definitions}
                expected_size = symbol["size"]
                if symbol["name"] in aliases:
                    entry = aliases[symbol["name"]]
                    if entry["expected_address"] != symbol["address"] \
                            or entry["original_fragment_size"] != symbol["size"]:
                        raise ValueError("Array-alias expectation disagrees with the original map")
                    row["source_array_layout"] = entry
                    expected_size = entry["expected_source_size"]
                if not definitions:
                    status = "missing"
                elif len(definitions) != 1:
                    status = "ambiguous"
                elif definitions[0]["address"] != symbol["address"]:
                    status = "address_mismatch"
                elif definitions[0]["size"] != expected_size:
                    status = "size_mismatch"
                else:
                    status = "match"
                row["status"] = status
                totals[status] += 1
                rows.append(row)
        result["units"].append({"name": unit["name"],
                                "source_path": metadata["source_path"], "symbols": rows})
    result["summary"] = {"complete_source_units": len(result["units"]),
                         "skipped_incomplete_units": len(result["skipped_incomplete_units"]),
                         "symbol_counts": totals}
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--elf", type=Path, default=Path("build/GC6E01/main.elf"))
    parser.add_argument("--report", type=Path, default=Path("build/GC6E01/report.json"))
    parser.add_argument("--symbols", type=Path, default=Path("config/GC6E01/symbols.txt"))
    parser.add_argument("--include-unit", action="append", default=[], metavar="UNIT",
                        help="Also inspect this complete source-backed main owner, including mixed units")
    parser.add_argument("--array-aliases", type=Path,
                        help="Optional scoped source-array alias registration directory")
    parser.add_argument("--output", type=Path, default=Path("build/GC6E01/data_link_diagnostics.json"))
    args = parser.parse_args()
    try:
        expected = map_symbols(args.symbols.read_text())
        aliases = load_array_aliases(args.array_aliases, expected) if args.array_aliases else None
        result = diagnose(json.loads(args.report.read_text()), expected,
                          linked_symbols(args.elf.read_bytes()), aliases, args.include_unit)
    except (OSError, ValueError, KeyError, TypeError, struct.error) as exc:
        result = {"schema_version": 1, "payload_bytes_included": False,
                  "error": type(exc).__name__}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result.get("summary", result), indent=2))
    # Address differences and absent/dead-stripped symbols are observations.
    # Canonical hashes and the independent object checks remain the gates.
    return 1 if "error" in result else 0


if __name__ == "__main__":
    raise SystemExit(main())
