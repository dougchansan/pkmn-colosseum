#!/usr/bin/env python3
"""Validate the authored MetroTRK template without granting function credit.

The source-only gate checks the narrowly registered assembly language and its
evidence. The native gate independently applies the 49 allowed PowerPC ELF
relocations, checks hardware control transfers and zero-filled vector slots,
and compares the complete 7,988-byte result with its private DOL interval.
Only hashes, addresses, counts and equality results are printed or written.
Original inputs never supply bytes to a generated source or linker object.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import sys

if __package__:
    from .source_object_metadata import Elf
else:
    from source_object_metadata import Elf

ROOT = Path(__file__).resolve().parents[1]
SOURCE = "src/crt/interrupt_vectors.s"
UNIT = "main/crt/interrupt_vectors"
EVIDENCE = "docs/asm_evidence/interrupt_vectors.md"
REGISTRY = "docs/asm_evidence/templates.json"
BASE = 0x800034AC
SIZE = 0x1F34
BANNER = "Metrowerks Target Resident Kernel for PowerPC"
LABELS = {"gTRKInterruptVectorTable": 0, "lbl_80003FF0": 0xB44,
          "lbl_80004000": 0xB54, "gTRKInterruptVectorTableEnd": SIZE}
TARGETS = {"__TRK_reset": (".init", 0x800053E0),
           "TRKInterruptHandler": (".text", 0x800C0EAC)}
MACROS = {"trk_redirect", "vector_slot", "trk_vector", "trk_tlb_redirect"}
MNEMONICS = {"andis.", "b", "beq", "dcbi", "icbi", "li", "lis", "mfcr",
             "mfdar", "mfmsr", "mfsprg", "mfsrr0", "mfsrr1", "mtcrf",
             "mtmsr", "mtsprg", "mtsrr0", "mtsrr1", "ori", "rfi", "sync", "xoris"}
STANDARD_SLOTS = (0x300, 0x400, 0x500, 0x600, 0x700, 0x800, 0x900,
                  0xC00, 0xD00, 0xE00, 0x1300, 0x1400, 0x1600, 0x1700,
                  0x1C00, 0x1D00, 0x1E00, 0x1F00)
TLB_SLOTS = (0x1000, 0x1100, 0x1200)
# (first instruction of lis/ori pair, hardware exception identifier).
REDIRECTS = ((0x238, 0x200), *((s + 0x20, s) for s in STANDARD_SLOTS),
             (0xF40, 0xF20), (0xF74, 0xF00),
             *((s + 0x5C, s) for s in TLB_SLOTS))
CODE_REGIONS = ((0x100, 0x104), (0x200, 0x24C),
                *((s, s + 0x34) for s in STANDARD_SLOTS),
                (0xF00, 0xF04), (0xF20, 0xF88),
                *((s, s + 0x70) for s in TLB_SLOTS))
REL24, ADDR16_HI, ADDR16_LO = 10, 5, 4


def require(condition, message):
    if not condition:
        raise ValueError(message)


def unique_fields(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, "Duplicate JSON field")
        result[key] = value
    return result


def load_registration(path):
    document = json.loads(path.read_text(encoding="utf-8"), object_pairs_hook=unique_fields)
    require(set(document) == {"schema", "templates"} and document["schema"] == 1
            and isinstance(document["templates"], list) and len(document["templates"]) == 1,
            "Expected the single audited hardware-template registration")
    entry = document["templates"][0]
    expected = {"id": "metrotrk-interrupt-vectors", "tier": "library-hardware-template",
                "path": SOURCE, "unit": UNIT, "evidence": EVIDENCE,
                "section": ".init", "address": "0x800034AC", "size": SIZE,
                "alignment": 4, "function_credit": 0, "code_credit": 0,
                "labels": LABELS}
    require(isinstance(entry, dict) and set(entry) == set(expected) |
            {"external_targets", "macros", "mnemonics"}, "Unexpected template registry fields")
    require(all(entry[key] == value for key, value in expected.items()),
            "Template registry changes the audited owner, bounds or credit")
    for key, values in (("external_targets", set(TARGETS)), ("macros", MACROS),
                        ("mnemonics", MNEMONICS)):
        require(isinstance(entry[key], list) and len(entry[key]) == len(values)
                and set(entry[key]) == values, "Template registry changes its audited instruction scope")
    return entry


def check_evidence(text):
    # Match formatted field labels, without requiring one particular heading.
    plain = text.replace("**", "")
    fields = ("Why it cannot be C:", "Other decompilations:", "Origin:",
              "External branch targets:")
    for field in fields:
        match = re.search(re.escape(field) + r"(.*?)(?=\n\s*(?:[-*]\s*)?(?:" +
                          "|".join(re.escape(f) for f in fields) + r")|\n## |\Z)", plain, re.S)
        require(match is not None and len(match[1].strip()) >= 20,
                "Template evidence is missing a substantive required field")
        if field == "Other decompilations:":
            require("github.com/" in match[1] and re.search(r"\b[0-9a-f]{40}\b", match[1]),
                    "Template evidence needs a commit-pinned public source-family reference")
        if field == "External branch targets:":
            require(all(target in match[1] for target in TARGETS),
                    "Template evidence must describe both external targets")


def clean_lines(text):
    require("\x00" not in text, "Assembly source contains a NUL")
    # GNU as ignores everything after # before recognizing a later /* on that
    # line. Removing block comments first would hide a live .incbin between
    # two # comment lines. Scan in lexical order and retain string contents.
    cleaned = []
    cursor, state = 0, "normal"
    while cursor < len(text):
        character = text[cursor]
        pair = text[cursor:cursor + 2]
        if state == "block":
            if pair == "*/":
                state = "normal"
                cursor += 2
                continue
            if character == "\n":
                cleaned.append(character)
            cursor += 1
            continue
        if state == "string":
            require(character not in "\r\n", "Unterminated assembly string")
            cleaned.append(character)
            if character == "\\":
                require(cursor + 1 < len(text) and text[cursor + 1] not in "\r\n",
                        "Unterminated assembly string escape")
                cleaned.append(text[cursor + 1])
                cursor += 2
                continue
            if character == '"':
                state = "normal"
            cursor += 1
            continue
        if character == "#":
            end = text.find("\n", cursor)
            cursor = len(text) if end < 0 else end
            continue
        if pair == "/*":
            cleaned.append(" ")
            state = "block"
            cursor += 2
            continue
        require(pair != "*/", "Stray assembly block-comment terminator")
        if character == '"':
            state = "string"
        cleaned.append(character)
        cursor += 1
    require(state == "normal", "Unterminated assembly comment or string")
    for number, line in enumerate("".join(cleaned).splitlines(), 1):
        line = line.strip()
        if line:
            require(";" not in line, "Multiple assembly statements on one line are unsupported")
            yield number, line


def check_source(text, entry, evidence_text):
    require(entry["evidence"] in "\n".join(text.splitlines()[:40]),
            "Assembly source must cite its registered evidence near the beginning")
    check_evidence(evidence_text)
    active = None
    macros, globals_, labels, mnemonics = set(), set(), set(), set()
    section_count = banner_count = 0
    for _, line in clean_lines(text):
        if line.endswith(":"):
            label = line[:-1]
            require(re.fullmatch(r"[A-Za-z_.$][A-Za-z_0-9.$\\@]*", label),
                    "Unsupported assembly label")
            require(label.startswith(".L") or label in LABELS, "Unregistered public assembly label")
            require(label not in labels, "Duplicate assembly label")
            labels.add(label)
            continue
        pieces = line.split(None, 1)
        operation, operands = pieces[0], pieces[1].strip() if len(pieces) == 2 else ""
        if operation == ".macro":
            fields = operands.split()
            require(active is None and len(fields) == 2 and fields[0] in MACROS
                    and fields[0] not in macros and fields[1] in ("vector", "offset"),
                    "Unsupported or nested vector macro")
            active = fields[0]
            macros.add(active)
        elif operation == ".endm":
            require(active is not None and not operands, "Unbalanced vector macro")
            active = None
        elif operation == ".section":
            require(active is None and re.fullmatch(r'\.init\s*,\s*"ax"\s*,\s*@progbits', operands),
                    "The source may define only its executable .init section")
            section_count += 1
        elif operation in (".global", ".globl"):
            names = [name.strip() for name in operands.split(",")]
            require(active is None and all(name in LABELS and name not in globals_ for name in names)
                    and len(names) == len(set(names)), "Unregistered or duplicate global vector label")
            globals_.update(names)
        elif operation == ".balign":
            require(re.fullmatch(r"4\s*,\s*0", operands), "Only four-byte zero alignment is allowed")
        elif operation == ".org":
            require(re.fullmatch(r"gTRKInterruptVectorTable\s*\+\s*(?:\\offset|0x[0-9A-Fa-f]+|[0-9]+)\s*,\s*0",
                                 operands), "Vector slots require explicit zero fill relative to the table")
        elif operation == ".asciz":
            require(active is None and operands == json.dumps(BANNER),
                    "The only emitted text is the fixed public MetroTRK banner")
            banner_count += 1
        elif operation.startswith("."):
            raise ValueError("Unsupported assembly directive; binary includes and raw words are forbidden")
        elif operation in MACROS:
            require(operation in macros and operands, "Unknown vector macro invocation")
        else:
            require(operation in MNEMONICS, "Instruction is outside the registered hardware-template scope")
            if operation == "b":
                require(operands in ("__TRK_reset", ".Lperformance_monitor"),
                        "Unregistered direct vector branch target")
            elif operation == "beq":
                require(operands == r".Lrestore_cr_\@", "Unregistered conditional vector branch target")
            mnemonics.add(operation)
    require(active is None and macros == MACROS and globals_ == set(LABELS)
            and set(LABELS) <= labels and section_count == banner_count == 1,
            "Assembly source omits its registered structure or owner labels")
    require(mnemonics == MNEMONICS, "Assembly source differs from its registered mnemonic set")
    return {"source_sha256": hashlib.sha256(text.encode("utf-8")).hexdigest(),
            "registered_macros": len(macros), "registered_mnemonics": len(mnemonics),
            "source_uses_binary_inputs": False}


def symbol_addresses(text):
    result = {}
    for name, section, address, fields in re.findall(
            r"(?m)^([A-Za-z_][A-Za-z_0-9]*) = (\.[^:]+):0x([0-9A-Fa-f]+);\s*//([^\n]*)", text):
        if name not in TARGETS:
            continue
        require(name not in result and re.search(r"\btype:function\b", fields),
                "Vector external target must be a unique real function")
        require((section, int(address, 16)) == TARGETS[name],
                "Vector external target differs from its audited symbol identity")
        result[name] = int(address, 16)
    require(set(result) == set(TARGETS), "A required vector function is absent from the public symbol map")
    return result


def expected_relocations():
    expected = {0x100: (REL24, "__TRK_reset")}
    for first, _ in REDIRECTS:
        expected[first + 2] = (ADDR16_HI, "TRKInterruptHandler")
        expected[first + 6] = (ADDR16_LO, "TRKInterruptHandler")
    return expected


def read_relocations(obj, init):
    result = {}
    for index, section in enumerate(obj.sections):
        require(obj.names[index] != ".note.split", "Native source must not carry original split metadata")
        if section[1] not in (4, 9):
            continue
        require(section[1] == 4 and section[6] == obj.symtab and section[7] == init
                and section[9] == 12 and section[5] % 12 == 0,
                "Vector source requires explicit .init RELA relocations")
        for at in range(0, section[5], 12):
            offset, info, addend = struct.unpack_from(">IIi", obj.payload(index), at)
            symbol_index, kind = info >> 8, info & 255
            require(0 < symbol_index < len(obj.symbols) and offset not in result,
                    "Invalid or duplicate vector relocation site")
            name, symbol = obj.symbols[symbol_index]
            require(name in TARGETS and symbol[1:] == (0, 0, 0x10, 0, 0)
                    and addend == 0, "Vector relocation changes an external target or addend")
            result[offset] = (kind, name)
    require(result == expected_relocations(), "Vector relocation types, destinations or sites differ from the audited 49")
    return result


def validate_object(blob, addresses):
    obj = Elf(blob)
    require(obj.header[7] == 0x80000000, "Vector object must pass the normal PowerPC ELF fixup")
    require(set(obj.allocated) == {".init"}, "Vector object has an unexpected allocated section")
    index = obj.allocated[".init"]
    section = obj.sections[index]
    require((section[1], section[2], section[3], section[5], section[8]) == (1, 6, 0, SIZE, 4),
            "Vector object changes its .init type, flags, extent or alignment")
    for name, offset in LABELS.items():
        _, symbol = obj.unique_symbol(name)
        require(symbol[1:] == (offset, 0, 0x10, 0, index),
                "Vector public label binding, visibility or offset is incorrect")
    defined = {name for name, symbol in obj.symbols if symbol[3] >> 4 and symbol[5] != 0}
    undefined = {name for name, symbol in obj.symbols if symbol[3] >> 4 and symbol[5] == 0}
    require(defined == set(LABELS) and undefined == set(TARGETS),
            "Vector object has unexpected public definitions or imports")
    require(not any(symbol[3] & 15 == 2 for _, symbol in obj.symbols),
            "Native hardware template must not define function symbols")
    require(not any(name == "pad_00_800034AC_init" for name, _ in obj.symbols),
            "Native template must not contain the original synthetic padding marker")
    relocations = read_relocations(obj, index)
    payload = bytearray(obj.payload(index))
    for offset, (kind, name) in relocations.items():
        target = addresses[name]
        if kind == REL24:
            word = struct.unpack_from(">I", payload, offset)[0]
            require(word == 0x48000000, "Reset relocation is not a plain relative branch")
            displacement = target - (BASE + offset)
            require(displacement % 4 == 0 and -(1 << 25) <= displacement < 1 << 25,
                    "Reset branch is outside its signed displacement range")
            struct.pack_into(">I", payload, offset, word | (displacement & 0x03FFFFFC))
        else:
            word = struct.unpack_from(">I", payload, offset - 2)[0]
            expected = 0x3C600000 if kind == ADDR16_HI else 0x60630000
            require(word == expected, "Handler relocation is not an unfilled lis/ori r3 operand")
            value = (target >> 16) & 65535 if kind == ADDR16_HI else target & 65535
            struct.pack_into(">H", payload, offset, value)
    return bytes(payload), {"native_object_sha256": hashlib.sha256(blob).hexdigest(),
                            "relocations": {"R_PPC_REL24": 1, "R_PPC_ADDR16_HI": 24,
                                            "R_PPC_ADDR16_LO": 24},
                            "public_labels": LABELS, "original_metadata_dependency": False}


def check_control_transfers(payload, addresses):
    require(len(payload) == SIZE, "Vector payload has an incorrect extent")
    used = bytearray(SIZE)
    signature = BANNER.encode("ascii") + b"\0"
    require(payload[:len(signature)] == signature, "Public MetroTRK banner differs")
    used[:len(signature)] = b"\1" * len(signature)
    for start, end in CODE_REGIONS:
        require(start % 4 == end % 4 == 0 and not any(used[start:end]), "Invalid vector code region")
        used[start:end] = b"\1" * (end - start)
    require(all(value == 0 for value, occupied in zip(payload, used) if not occupied),
            "Reserved vector slots or alignment contain nonzero payload")
    direct, returns = {}, set()
    for start, end in CODE_REGIONS:
        for offset in range(start, end, 4):
            word = struct.unpack_from(">I", payload, offset)[0]
            require(word != 0, "A declared vector instruction is zero-filled")
            opcode = word >> 26
            if opcode in (16, 18):
                require(not word & 3, "Vector branch unexpectedly uses absolute addressing or linkage")
                mask, sign = (0x03FFFFFC, 0x02000000) if opcode == 18 else (0xFFFC, 0x8000)
                displacement = word & mask
                if displacement & sign:
                    displacement -= (sign << 1)
                direct[offset] = (opcode, BASE + offset + displacement)
            if opcode == 19:
                xo = (word >> 1) & 1023
                require(xo not in (16, 528), "Unexpected link-register or count-register vector transfer")
                if xo == 50:
                    require(word == 0x4C000064, "Malformed rfi instruction")
                    returns.add(offset)
    expected = {0x100: (18, addresses["__TRK_reset"]), 0xF00: (18, BASE + 0xF54)}
    expected.update({slot + 0x14: (16, BASE + slot + 0x30) for slot in TLB_SLOTS})
    require(direct == expected, "Vector direct control-transfer destinations differ")
    require(returns == {first + 0x10 for first, _ in REDIRECTS}, "Vector rfi positions differ")
    for first, vector in REDIRECTS:
        high, low, write_srr0, identity, _ = struct.unpack_from(">5I", payload, first)
        require(high == 0x3C600000 | (addresses["TRKInterruptHandler"] >> 16)
                and low == 0x60630000 | (addresses["TRKInterruptHandler"] & 65535)
                and write_srr0 == 0x7C7A03A6 and identity == 0x38600000 | vector,
                "Vector rfi does not target its registered handler with the correct exception identifier")
    for slot in TLB_SLOTS:
        require(struct.unpack_from(">I", payload, slot + 0x14)[0] == 0x4182001C,
                "TLB conditional branch condition differs")
    return {"instruction_bytes": sum(end - start for start, end in CODE_REGIONS),
            "instructions": 365, "signature_bytes": len(signature),
            "zero_fill_bytes": used.count(0), "direct_branches": len(direct),
            "rfi_redirects": len(returns)}


def original_interval(dol):
    require(len(dol) >= 0x100, "Truncated DOL header")
    offsets = struct.unpack_from(">18I", dol, 0)
    addresses = struct.unpack_from(">18I", dol, 0x48)
    sizes = struct.unpack_from(">18I", dol, 0x90)
    found = []
    spans = []
    for offset, address, size in zip(offsets, addresses, sizes):
        if not size:
            continue
        require(offset >= 0x100 and offset + size <= len(dol)
                and address + size <= 0x100000000, "DOL initialized section is outside the file or address space")
        require(not any(a < address + size and address < b for a, b in spans),
                "Overlapping DOL initialized addresses")
        spans.append((address, address + size))
        if address <= BASE and BASE + SIZE <= address + size:
            start = offset + BASE - address
            found.append(dol[start:start + SIZE])
    require(len(found) == 1, "Vector interval must belong to exactly one initialized DOL section")
    return found[0]


def compare_original(payload, dol):
    require(payload == original_interval(dol), "Complete relocated vector template differs from the original DOL interval")
    return {"canonical_interval_equal": True,
            "relocated_interval_sha256": hashlib.sha256(payload).hexdigest()}


def check_report(report):
    wanted = {"total_code": 2504940, "matched_code": 2504940, "complete_code": 2504940,
              "total_functions": 8677, "matched_functions": 8677,
              "total_data": 3565996, "matched_data": 3565996, "complete_data": 3565996}
    require(all(int(report.get("measures", {}).get(key, -1)) == value for key, value in wanted.items()),
            "Whole-project report changes the completion scope or is incomplete")
    # Other source owners may be reunited into their original translation
    # units. Their reviewed topology is enforced by check_object_map_freeze;
    # this vector gate requires a coherent, fully source-complete inventory
    # without baking that unrelated historical partition count into the unit.
    total_units = int(report.get("measures", {}).get("total_units", -1))
    require(total_units > 0 and
            int(report["measures"].get("complete_units", -1)) == total_units,
            "Whole-project report contains incomplete scoring units")
    require(len(report.get("units", [])) == total_units, "Report unit inventory is incomplete")
    require(len({unit.get("name") for unit in report["units"]}) == total_units,
            "Report unit identities are duplicated")
    require(all(unit.get("metadata", {}).get("complete") is True and
                isinstance(unit.get("metadata", {}).get("source_path"), str) and
                unit["metadata"]["source_path"] for unit in report["units"]),
            "Report claims complete units without complete source ownership")
    units = [unit for unit in report["units"] if unit.get("name") == UNIT]
    require(len(units) == 1, "Expected one vector owner in the report")
    unit = units[0]
    metadata, measures = unit.get("metadata", {}), unit.get("measures", {})
    require(metadata.get("complete") is True and metadata.get("source_path") == SOURCE
            and metadata.get("module_name") == "main" and int(metadata.get("module_id", -1)) == 0,
            "Vector report owner lacks its completed authored source")
    require(not unit.get("functions") and all(int(measures.get(key, 0)) == 0 for key in
            ("total_code", "matched_code", "complete_code", "total_functions", "matched_functions",
             "total_data", "matched_data", "complete_data")),
            "Hardware template must not acquire function, code or data credit")
    require(int(measures.get("total_units", 0)) == int(measures.get("complete_units", 0)) == 1,
            "Vector owner does not count as exactly one complete unit")
    sections = unit.get("sections", [])
    require(len(sections) == 1 and sections[0].get("name") == ".init"
            and int(sections[0].get("size", -1)) == SIZE
            and int(sections[0].get("metadata", {}).get("virtual_address", -1)) == BASE
            and float(sections[0].get("fuzzy_match_percent", 0)) == 100.0
            and not sections[0].get("functions"), "Vector report physical extent differs")
    return {"completed_units": total_units, "vector_source_complete": True,
            "additional_function_credit": 0, "additional_code_credit": 0,
            "additional_data_credit": 0}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source-only", action="store_true")
    parser.add_argument("--source", type=Path, default=ROOT / SOURCE)
    parser.add_argument("--registry", type=Path, default=ROOT / REGISTRY)
    parser.add_argument("--symbols", type=Path, default=ROOT / "config/GC6E01/symbols.txt")
    parser.add_argument("--object", type=Path)
    parser.add_argument("--orig", type=Path)
    parser.add_argument("--report", type=Path)
    parser.add_argument("--output", type=Path)
    args = parser.parse_args(argv)
    if args.source_only:
        if any((args.object, args.orig, args.report)):
            parser.error("--source-only cannot use native/private/report inputs")
    elif not all((args.object, args.orig, args.report)):
        parser.error("Native verification requires --object, --orig and --report")
    try:
        entry = load_registration(args.registry)
        source = args.source.read_text(encoding="utf-8")
        result = {"schema": 1, "status": "passed", "template": entry["id"],
                  "mode": "source-only" if args.source_only else "native-original-report",
                  "unit": UNIT, "address": hex(BASE), "bytes": SIZE}
        result.update(check_source(source, entry, (ROOT / entry["evidence"]).read_text(encoding="utf-8")))
        if not args.source_only:
            addresses = symbol_addresses(args.symbols.read_text(encoding="utf-8"))
            payload, native = validate_object(args.object.read_bytes(), addresses)
            result.update(native)
            result["control_transfers"] = check_control_transfers(payload, addresses)
            result.update(compare_original(payload, args.orig.read_bytes()))
            result.update(check_report(json.loads(args.report.read_text(encoding="utf-8"))))
        if args.output:
            inputs = [args.source, args.registry, args.symbols, ROOT / entry["evidence"],
                      *[p for p in (args.object, args.orig, args.report) if p]]
            require(args.output.resolve() not in {p.resolve() for p in inputs},
                    "Metadata output must not overwrite a validation input")
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(result, sort_keys=True))
        return 0
    except (ValueError, OSError, KeyError, TypeError, struct.error) as error:
        print(f"Vector validation failed: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
