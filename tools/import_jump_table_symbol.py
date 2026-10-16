#!/usr/bin/env python3
"""Replace a duplicate MWCC switch table with an identical retail-owned table.

The compiled table must occupy its whole .data section. The retail owner may
hold other tables, but the selected span must have identical bytes and
relocations. Only references to the sole compiled local table are changed;
the compiled .text and all other allocated sections are preserved byte-for-byte.
"""

import argparse
import struct
from pathlib import Path

from import_constant_symbol import (
    COMMENT_HEADER,
    SHN_LORESERVE,
    SHT_RELA,
    SHT_SYMTAB,
    parse_symbols,
    read_elf,
    write_elf,
)


def table_layout(sections, function):
    names = {section["name"]: i for i, section in enumerate(sections)}
    if ".data" not in names or ".rela.data" not in names:
        raise ValueError("expected one relocated .data switch table")
    data_index = names[".data"]
    rela = sections[names[".rela.data"]]
    symtab_index, symbols = parse_symbols(sections)
    data = sections[data_index]
    if data["fields"][1:3] != [1, 3] or len(data["data"]) == 0 or \
            len(data["data"]) % 4 or rela["fields"][1] != SHT_RELA or \
            rela["fields"][6] != symtab_index or rela["fields"][7] != data_index or \
            rela["fields"][9] != 12:
        raise ValueError("unexpected switch-table section layout")
    records = []
    for at in range(0, len(rela["data"]), 12):
        offset, info, addend = struct.unpack_from(">IIi", rela["data"], at)
        index, kind = info >> 8, info & 0xFF
        if index >= len(symbols) or offset != at // 12 * 4 or kind != 1 or \
                symbols[index]["name"] != function:
            raise ValueError("switch-table relocation differs from function-offset array")
        records.append((offset, kind, addend, function))
    if len(records) * 4 != len(data["data"]):
        raise ValueError("switch-table entries do not cover .data")
    return names, symtab_index, symbols, records


def owner_table_layout(sections, table, function, offset, size):
    names = {section["name"]: i for i, section in enumerate(sections)}
    if ".data" not in names or ".rela.data" not in names:
        raise ValueError("retail owner has no relocated .data")
    data_index = names[".data"]
    data = sections[data_index]["data"]
    rela = sections[names[".rela.data"]]
    symtab_index, symbols = parse_symbols(sections)
    if offset < 0 or offset % 4 or size == 0 or size % 4 or offset + size > len(data) or \
            rela["fields"][1] != SHT_RELA or rela["fields"][6] != symtab_index or \
            rela["fields"][7] != data_index or rela["fields"][9] != 12:
        raise ValueError("invalid retail-owned switch-table span")
    matches = [s for s in symbols if s["name"] == table and
               s["shndx"] == data_index and s["value"] == offset and
               s["size"] == size and s["info"] == 0x11]
    if len(matches) != 1 or sum(s["name"] == table for s in symbols) != 1:
        raise ValueError("retail owner does not export the complete table")
    if any(s is not matches[0] and s["shndx"] == data_index and
           offset <= s["value"] < offset + size and s["size"] > 0
           for s in symbols):
        raise ValueError("retail table overlaps another symbol")
    records = []
    for at in range(0, len(rela["data"]), 12):
        entry_offset, info, addend = struct.unpack_from(">IIi", rela["data"], at)
        if not offset <= entry_offset < offset + size:
            continue
        index, kind = info >> 8, info & 0xFF
        if index >= len(symbols) or entry_offset != offset + len(records) * 4 or \
                kind != 1 or symbols[index]["name"] != function:
            raise ValueError("retail switch-table relocation differs")
        records.append((entry_offset - offset, kind, addend, function))
    if len(records) * 4 != size:
        raise ValueError("retail switch-table relocations do not cover span")
    return data[offset:offset + size], records


def import_table(path, owner_path, table, function, owner_offset=0):
    original = path.read_bytes()
    header, sections = read_elf(original)
    _, owner_sections = read_elf(owner_path.read_bytes())
    names = {section["name"]: i for i, section in enumerate(sections)}
    if ".data" not in names:
        _, symbols = parse_symbols(sections)
        if len([s for s in symbols if s["name"] == table and s["shndx"] == 0 and
                s["info"] == 0x10]) == 1:
            return False
        raise ValueError("table section missing without an imported symbol")

    names, symtab_index, symbols, records = table_layout(sections, function)
    data_index = names[".data"]
    owner_data, owner_records = owner_table_layout(
        owner_sections, table, function, owner_offset, len(sections[data_index]["data"]))
    if sections[data_index]["data"] != owner_data or records != owner_records:
        raise ValueError("compiled table differs from retail-owned table")
    functions = [s for s in symbols if s["name"] == function and
                 s["info"] == 0x12 and s["shndx"] == names.get(".text")]
    if len(functions) != 1 or any(addend < 0 or addend >= functions[0]["size"]
                                   for _, _, addend, _ in records):
        raise ValueError("table entries do not point inside the compiled function")

    removed_symbols = {i for i, s in enumerate(symbols) if s["shndx"] == data_index}
    table_symbols = [i for i in removed_symbols if symbols[i]["value"] == 0 and
                     symbols[i]["size"] == len(records) * 4 and
                     symbols[i]["info"] == 1]
    if len(table_symbols) != 1 or len(removed_symbols) != 2 or \
            any(symbols[i]["info"] != 3 for i in removed_symbols - set(table_symbols)):
        raise ValueError("compiled .data contains symbols besides one local table")

    removed_sections = sorted((data_index, names[".rela.data"]))
    first_global = sections[symtab_index]["fields"][7]
    if any(i >= first_global for i in removed_symbols):
        raise ValueError("compiled switch table is not local")
    kept = [i for i in range(len(symbols)) if i not in removed_symbols]
    remap = {old: new for new, old in enumerate(kept)}
    new_symbols = [dict(symbols[i]) for i in kept]
    if any(s["name"] == table for s in new_symbols):
        raise ValueError("table symbol is already present in compiled object")
    strtab_index = sections[symtab_index]["fields"][6]
    strtab = bytearray(sections[strtab_index]["data"])
    name_offset = len(strtab)
    strtab.extend(table.encode("ascii") + b"\0")
    table_index = len(new_symbols)
    new_symbols.append({"name": table, "value": 0, "size": 0,
                        "info": 0x10, "other": 0, "shndx": 0})

    retargeted = []
    for section in sections:
        fields = section["fields"]
        if fields[1] != SHT_RELA or section["name"] == ".rela.data":
            continue
        if fields[6] != symtab_index or fields[9] != 12:
            raise ValueError("unexpected relocation section layout")
        payload = bytearray(section["data"])
        for at in range(0, len(payload), 12):
            offset, info, addend = struct.unpack_from(">IIi", payload, at)
            old_index, kind = info >> 8, info & 0xFF
            if old_index >= len(symbols):
                raise ValueError("relocation symbol index out of range")
            if old_index in removed_symbols:
                if section["name"] != ".rela.text" or old_index not in table_symbols or \
                        kind not in (4, 6) or addend != 0:
                    raise ValueError("unsupported reference into removed table")
                new_index = table_index
                retargeted.append(kind)
            else:
                new_index = remap[old_index]
            struct.pack_into(">IIi", payload, at, offset, new_index << 8 | kind, addend)
        section["data"] = bytes(payload)
    if sorted(retargeted) != [4, 6]:
        raise ValueError("expected one HA and one LO relocation to switch table")

    if ".comment" in names:
        comment = sections[names[".comment"]]
        payload = comment["data"]
        if not payload.startswith(b"CodeWarrior") or \
                len(payload) != COMMENT_HEADER + 8 * len(symbols):
            raise ValueError("unexpected MWCC .comment symbol table")
        entries = [payload[COMMENT_HEADER + 8 * i:COMMENT_HEADER + 8 * i + 8]
                   for i in kept]
        comment["data"] = payload[:COMMENT_HEADER] + b"".join(entries) + bytes(8)

    def section_index(old):
        return old - sum(index < old for index in removed_sections)

    old_symbols = sections[symtab_index]["data"]
    packed = bytearray()
    for new_index, symbol in enumerate(new_symbols):
        name = (struct.unpack_from(">I", old_symbols, kept[new_index] * 16)[0]
                if new_index < len(kept) else name_offset)
        shndx = symbol["shndx"]
        if 0 < shndx < SHN_LORESERVE:
            shndx = section_index(shndx)
        packed.extend(struct.pack(">IIIBBH", name, symbol["value"], symbol["size"],
                                  symbol["info"], symbol["other"], shndx))
    sections[symtab_index]["data"] = bytes(packed)
    sections[symtab_index]["fields"][7] = first_global - len(removed_symbols)
    sections[strtab_index]["data"] = bytes(strtab)
    for section in sections:
        fields = section["fields"]
        if fields[1] in (SHT_SYMTAB, SHT_RELA) or fields[6]:
            fields[6] = section_index(fields[6]) if fields[6] else 0
        if fields[1] == SHT_RELA and fields[7]:
            fields[7] = section_index(fields[7])
    header[13] = section_index(header[13])
    rewritten = [s for i, s in enumerate(sections) if i not in removed_sections]
    output = write_elf(header, rewritten)
    _, checked_sections = read_elf(output)
    if {s["name"]: s["data"] for s in checked_sections if s["fields"][2] & 2} != \
            {s["name"]: s["data"] for s in read_elf(original)[1]
             if s["fields"][2] & 2 and s["name"] != ".data"}:
        raise ValueError("import changed allocated sections other than .data")
    _, checked_symbols = parse_symbols(checked_sections)
    if len([s for s in checked_symbols if s["name"] == table and s["shndx"] == 0 and
            s["info"] == 0x10]) != 1:
        raise ValueError("import did not create one undefined table symbol")
    path.write_bytes(output)
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--object", required=True, type=Path)
    parser.add_argument("--owner", required=True, type=Path)
    parser.add_argument("--symbol", required=True)
    parser.add_argument("--function", required=True)
    parser.add_argument("--owner-offset", type=lambda value: int(value, 0), default=0)
    args = parser.parse_args()
    import_table(args.object, args.owner, args.symbol, args.function, args.owner_offset)


if __name__ == "__main__":
    main()
