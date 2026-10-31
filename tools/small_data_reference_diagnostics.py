#!/usr/bin/env python3
"""Describe five known references to unresolved small-data operands.

Only the existing relocations to lbl_80478D86, lbl_80478D88 and lbl_80478D8C
are inspected. Output contains source-validated handler names, status kinds,
field IDs and operand widths. No script bytes, opcode values, instruction
listing, operand contents or other pointer values are emitted. Unknown
layouts fail closed; this diagnostic neither generates source nor changes
objects, matching credit or original-object fallback.
"""

import argparse
import json
from pathlib import Path
import re
import struct

if __package__:
    from .data_match_diagnostics import elf_metadata
else:
    from data_match_diagnostics import elf_metadata


SCRIPT_OWNERS = {
    "main/auto_05_80377088_data": (2692, "lbl_80377531", 0x4A9, 308),
    "main/auto_05_80377D90_data": (1840, "lbl_80378115", 0x385, 448),
}
SITES = (
    ("main/auto_05_80377088_data", 0xEB, "lbl_80478D86", 2),
    ("main/auto_05_80377088_data", 0xFF, "lbl_80478D88", 4),
    ("main/auto_05_80377088_data", 0x113, "lbl_80478D8C", 12),
    ("main/auto_05_80377D90_data", 0xB3, "lbl_80478D86", 2),
    ("main/auto_05_80377D90_data", 0xC7, "lbl_80478D88", 4),
)
HANDLERS = {
    "WS_STATUS_SET8": (1, "read"),
    "WS_STATUS_SET16": (2, "read"),
    "WS_STATUS_SET32": (4, "read"),
    "WS_STATUS_GET8": (1, "write"),
    "WS_STATUS_GET16": (2, "write"),
    "WS_STATUS_GET32": (4, "write"),
}
# These are the existing statusSetStatus/statusGetStatus family IDs. Only
# families with an explicit field switch in matching C are accepted here.
FAMILIES = {
    3: ("hero", "heroSetStatus", "src/game/hero_bios_exact_8012A450.c"),
    4: ("item", "fightItemCreate_80142B24", "src/game/people/people_data_exact_80142B24.c"),
    5: ("pokemon", "pokemonSetStatus", "src/game/pokemon_range_8011F5FC.c"),
    6: ("waza", "wazaSetStatus", "src/game/waza.c"),
    7: ("fight_floor", "fightFloorSetStatus", "src/game/fight_floor.c"),
    8: ("fight_side", "fightSideSetStatus", "src/game/fight_side.c"),
    9: ("fight_trainer", "fightTrainerSetStatus", "src/game/fight_trainer_range_801FAA58.c"),
}
SOURCE_PATHS = (
    "src/game/data/rodata_80279B78.c",
    "src/game/fight_range_80211A00.c",
    "src/game/status.c",
    *(row[2] for row in FAMILIES.values()),
)


def _require(condition, reason):
    if not condition:
        raise ValueError(reason)


def _without_comments(text):
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  " ", text, flags=re.S)


def _balanced_body(text, opening):
    depth = 1
    for index in range(opening + 1, len(text)):
        depth += (text[index] == "{") - (text[index] == "}")
        if not depth:
            return text[opening + 1:index]
    raise ValueError("unterminated source schema")


def _function(text, name):
    text = _without_comments(text)
    matches = list(re.finditer(r"(?m)^[A-Za-z_][^;{}]*\b" + re.escape(name)
                              + r"\s*\([^;{}]*\)\s*\{", text))
    _require(len(matches) == 1, "ambiguous source function schema")
    return _balanced_body(text, matches[0].end() - 1)


def _field_cases(body):
    switch = re.search(r"\bswitch\s*\([^{};]*\)\s*\{", body)
    _require(switch is not None, "missing source field switch")
    switch_body = _balanced_body(body, switch.end() - 1)
    depth = 0
    fields = set()
    for token in re.finditer(r"[{}]|\bcase\s+(0x[0-9A-Fa-f]+|[0-9]+)\s*:", switch_body):
        if token[0] == "{":
            depth += 1
        elif token[0] == "}":
            depth -= 1
        elif depth == 0:
            fields.add(int(token[1], 0))
    _require(fields and all(0 <= value <= 0xFFFF for value in fields),
             "invalid source field domain")
    return fields


def source_schema(root):
    """Validate handler layout and field domains against existing C source."""
    dispatch_text = _without_comments((root / SOURCE_PATHS[0]).read_text())
    table = re.search(r"const\s+void\*\s+lbl_8027A00C\[255\]\s*=\s*\{([^{}]+)\}",
                      dispatch_text)
    _require(table is not None, "missing source dispatch table")
    entries = [entry.strip() for entry in table[1].split(",") if entry.strip()]
    _require(len(entries) == 255, "unexpected source dispatch extent")
    dispatch = {}
    for index, entry in enumerate(entries):
        match = re.fullmatch(r"\(void\*\)\s*(\w+)", entry)
        if match and match[1] in HANDLERS:
            dispatch[index] = match[1]
    _require(set(dispatch.values()) == set(HANDLERS) and len(dispatch) == len(HANDLERS),
             "source dispatch handler identity differs")

    handlers = (root / SOURCE_PATHS[1]).read_text()
    for name, (width, access) in HANDLERS.items():
        body = re.sub(r"\s+", "", _function(handlers, name))
        ctype = "u" + str(width * 8)
        _require(f"({ctype}**)(iVar1+0x10)" in body
                 and "(u8*)(iVar1+1)" in body
                 and ("(u16*)(iVar1+10)" in body or "(u16*)(iVar1+0xa)" in body)
                 and "lbl_8047B610=lbl_8047B610+0x14;" in body,
                 "handler layout no longer matches source schema")
        callee = "statusSetStatus" if access == "read" else "statusGetStatus"
        _require(callee + "(" in body, "handler direction differs")

    status = _function((root / SOURCE_PATHS[2]).read_text(), "statusSetStatus")
    families = {}
    for kind, (family, callee, path) in FAMILIES.items():
        arm = re.search(r"\bcase\s+" + str(kind) + r"\s*:(.*?)(?:\bbreak\s*;)", status, re.S)
        _require(arm is not None and re.search(r"\b" + callee + r"\s*\(", arm[1]),
                 "status family mapping differs")
        fields = _field_cases(_function((root / path).read_text(), callee))
        families[kind] = (family, fields)
    return {"dispatch": dispatch, "families": families}


def diagnose(objects, schema):
    """Inspect the fixed five references; return no partial result on rejection."""
    _require(set(objects) == set(SCRIPT_OWNERS), "unexpected diagnostic object scope")
    rows = []
    for unit, relative, operand, extent in SITES:
        public, private = objects[unit]
        _require(set(public) == {".data"} and set(private) == {".data"},
                 "unexpected script object sections")
        section, data = public[".data"], private[".data"]
        section_size, script, script_offset, script_size = SCRIPT_OWNERS[unit]
        _require(not section["nobits"] and section["size"] == section_size
                 and data is not None and len(data) == section_size,
                 "script section extent differs")
        definitions = [s for s in section["symbols"] if s["name"] == script]
        _require(len(definitions) == 1, "script symbol identity differs")
        symbol = definitions[0]
        _require((symbol["offset"], symbol["size"], symbol["type"], symbol["binding"])
                 == (script_offset, script_size, 1, 1), "script symbol extent differs")
        pointer = script_offset + relative
        start, end = pointer - 0x10, pointer + 4
        _require(script_offset <= start < end <= script_offset + script_size,
                 "reference falls outside named script")

        relocation = [r for r in section["relocations"] if r["offset"] == pointer]
        _require(len(relocation) == 1, "missing or duplicate operand relocation")
        reloc = relocation[0]
        _require(reloc["type"] == 1 and reloc["symbol"] == operand
                 and reloc["addend"] == 0 and reloc.get("target_section") is None
                 and reloc.get("target_symbol_offset") == 0
                 and data[pointer:pointer + 4] == bytes(4),
                 "operand relocation identity differs")
        # This narrow schema requires the other optional context pointers to be
        # NULL or normalized ADDR32 relocations at the established pointer slots.
        slots = {start + 2, start + 6, start + 12, pointer}
        nearby = [r for r in section["relocations"]
                  if start - 3 <= r["offset"] < end]
        seen = set()
        for r in nearby:
            site = r["offset"]
            _require(site in slots and site not in seen and r["type"] == 1
                     and type(r.get("addend")) is int
                     and data[site:site + 4] == bytes(4),
                     "unexpected relocation in reference schema")
            seen.add(site)
        for site in slots - seen:
            _require(data[site:site + 4] == bytes(4), "opaque pointer in reference schema")

        handler = schema["dispatch"].get(data[start])
        _require(handler in HANDLERS, "unsupported reference handler")
        width, access = HANDLERS[handler]
        _require(width <= extent, "operand width exceeds mapped object")
        kind = data[start + 1]
        _require(kind in schema["families"], "unsupported status family")
        family, fields = schema["families"][kind]
        field = struct.unpack_from(">H", data, start + 10)[0]
        _require(field in fields, "field is outside source status schema")
        rows.append({"script_symbol": script, "reference_offset": relative,
                     "operand_symbol": operand, "handler_symbol": handler,
                     "status_kind": kind, "status_family": family,
                     "status_field_id": field, "operand_width_bits": width * 8,
                     "operand_access": access})
    return {"schema_version": 1, "payload_bytes_included": False,
            "scope": "five_known_small_data_references", "status": "verified",
            "references": rows}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, default=Path("objdiff.json"))
    parser.add_argument("--output", type=Path,
                        default=Path("build/GC6E01/small_data_reference_diagnostics.json"))
    args = parser.parse_args()
    root = args.config.resolve().parent
    protected = {args.config.resolve(), *((root / p).resolve() for p in SOURCE_PATHS)}
    try:
        config = json.loads(args.config.read_text())
        paths = {}
        for name in SCRIPT_OWNERS:
            units = [u for u in config["units"] if u["name"] == name]
            _require(len(units) == 1 and units[0].get("target_path"), "missing target owner")
            path = (root / (config.get("target_dir") or "") / units[0]["target_path"]).resolve()
            _require(path.is_relative_to(root), "target path is outside repository")
            paths[name] = path
            protected.add(path)
        protected.update(paths.values())
        _require(args.output.resolve() not in protected, "output aliases diagnostic input")
        schema = source_schema(root)
        output = diagnose({name: elf_metadata(path) for name, path in paths.items()}, schema)
    except (OSError, ValueError, KeyError, TypeError, struct.error):
        output = {"schema_version": 1, "payload_bytes_included": False,
                  "scope": "five_known_small_data_references", "status": "rejected",
                  "reason": "input_or_reference_schema_validation_failed"}
    if args.output.resolve() in protected:
        parser.error("output must not alias a diagnostic input")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n")
    print(json.dumps({"status": output["status"], "references": len(output.get("references", []))}))
    return 0 if output["status"] == "verified" else 1


if __name__ == "__main__":
    raise SystemExit(main())
