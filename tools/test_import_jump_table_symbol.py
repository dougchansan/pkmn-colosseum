#!/usr/bin/env python3
"""Fail-closed tests for the relocated switch-table import."""

import struct
import tempfile
import unittest
from pathlib import Path

import import_jump_table_symbol as importer
from import_constant_symbol import parse_symbols, read_elf


def elf(sections):
    names = ["", *(name for name, *_ in sections), ".shstrtab"]
    shstr = b"\0"
    offsets = {}
    for name in names:
        offsets[name] = len(shstr)
        shstr += name.encode() + b"\0"
    sections = [*sections, (".shstrtab", shstr, 3, 0, 0, 0, 1, 0)]
    indices = {name: i + 1 for i, (name, *_) in enumerate(sections)}
    blob = bytearray(52)
    records = [bytes(40)]
    for name, payload, kind, flags, link, info, align, entsize in sections:
        blob.extend(bytes(-len(blob) % align))
        records.append(struct.pack(">10I", offsets[name], kind, flags, 0,
                                   len(blob), len(payload), link, info, align, entsize))
        blob.extend(payload)
    blob.extend(bytes(-len(blob) % 4))
    shoff = len(blob)
    blob.extend(b"".join(records))
    blob[:52] = struct.pack(">16sHHIIIIIHHHHHH",
                            b"\x7fELF\x01\x02\x01" + bytes(9), 1, 20, 1,
                            0, 0, shoff, 0, 52, 0, 0, 40, len(records),
                            indices[".shstrtab"])
    return bytes(blob)


def symbol(strings, name, value, size, info, shndx):
    return struct.pack(">IIIBBH", strings.index(name.encode() + b"\0"),
                       value, size, info, 0, shndx)


def compiler_object(data=bytes(8), second_addend=8, extra_data_symbol=False,
                    second_text_kind=4):
    strings = b"\0@86\0fn_8006F720\0jumptable_802EE06C\0"
    local = [bytes(16), struct.pack(">IIIBBH", 0, 0, 0, 3, 0, 1),
             struct.pack(">IIIBBH", 0, 0, 0, 3, 0, 2),
             symbol(strings, "@86", 0, 8, 1, 2)]
    if extra_data_symbol:
        local.append(symbol(strings, "@86", 4, 4, 1, 2))
    function_index = len(local)
    symbols = b"".join(local + [symbol(strings, "fn_8006F720", 0, 16, 0x12, 1)])
    code_rela = struct.pack(">IIi", 2, 3 << 8 | 6, 0) + \
        struct.pack(">IIi", 6, 3 << 8 | second_text_kind, 0)
    data_rela = struct.pack(">IIi", 0, function_index << 8 | 1, 4) + \
        struct.pack(">IIi", 4, function_index << 8 | 1, second_addend)
    comment = b"CodeWarrior" + bytes(0x2C - 11) + bytes(8 * (function_index + 1))
    return elf([
        (".text", bytes(16), 1, 6, 0, 0, 4, 0),
        (".data", data, 1, 3, 0, 0, 8, 0),
        (".rela.text", code_rela, 4, 0, 5, 1, 4, 12),
        (".rela.data", data_rela, 4, 0, 5, 2, 4, 12),
        (".symtab", symbols, 2, 0, 6, function_index, 4, 16),
        (".strtab", strings, 3, 0, 0, 0, 1, 0),
        (".comment", comment, 1, 0, 0, 0, 1, 0),
    ])


def owner_object(data=bytes(8), second_addend=8, table_offset=0,
                 outside_relocation=False):
    strings = b"\0jumptable_802EE06C\0fn_8006F720\0"
    symbols = b"".join([
        bytes(16), struct.pack(">IIIBBH", 0, 0, 0, 3, 0, 1),
        symbol(strings, "jumptable_802EE06C", table_offset, 8, 0x11, 1),
        symbol(strings, "fn_8006F720", 0, 0, 0x10, 0),
    ])
    data_rela = (struct.pack(">IIi", 0, 3 << 8 | 1, 4)
                 if outside_relocation else b"") + \
        struct.pack(">IIi", table_offset, 3 << 8 | 1, 4) + \
        struct.pack(">IIi", table_offset + 4, 3 << 8 | 1, second_addend)
    return elf([
        (".data", bytes(table_offset) + data + bytes(4 if table_offset else 0),
         1, 3, 0, 0, 4, 0),
        (".rela.data", data_rela, 4, 0, 3, 1, 4, 12),
        (".symtab", symbols, 2, 0, 4, 2, 4, 16),
        (".strtab", strings, 3, 0, 0, 0, 1, 0),
    ])


class JumpTableImportTest(unittest.TestCase):
    def run_import(self, compiled, owner, owner_offset=0):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "compiled.o"
            owner_path = Path(directory) / "retail.o"
            path.write_bytes(compiled)
            owner_path.write_bytes(owner)
            changed = importer.import_table(path, owner_path, "jumptable_802EE06C",
                                            "fn_8006F720", owner_offset)
            return changed, path.read_bytes()

    def test_exact_table_import(self):
        changed, result = self.run_import(compiler_object(), owner_object())
        self.assertTrue(changed)
        _, sections = read_elf(result)
        self.assertNotIn(".data", [s["name"] for s in sections])
        self.assertNotIn(".rela.data", [s["name"] for s in sections])
        self.assertEqual(next(s["data"] for s in sections if s["name"] == ".text"), bytes(16))
        _, symbols = parse_symbols(sections)
        self.assertEqual([s["name"] for s in symbols if s["shndx"] == 0 and
                          s["name"] == "jumptable_802EE06C"], ["jumptable_802EE06C"])

    def test_interior_retail_table_import(self):
        changed, result = self.run_import(
            compiler_object(), owner_object(table_offset=4, outside_relocation=True),
            owner_offset=4)
        self.assertTrue(changed)
        _, sections = read_elf(result)
        self.assertNotIn(".data", [s["name"] for s in sections])

    def test_interior_table_offset_must_match(self):
        with self.assertRaisesRegex(ValueError, "complete table"):
            self.run_import(compiler_object(), owner_object(table_offset=4), owner_offset=0)

    def test_retail_bytes_must_match(self):
        with self.assertRaisesRegex(ValueError, "differs"):
            self.run_import(compiler_object(), owner_object(data=b"\x01" + bytes(7)))

    def test_retail_relocations_must_match(self):
        with self.assertRaisesRegex(ValueError, "differs"):
            self.run_import(compiler_object(), owner_object(second_addend=12))

    def test_compiled_table_must_be_sole_local_data(self):
        with self.assertRaisesRegex(ValueError, "symbols besides"):
            self.run_import(compiler_object(extra_data_symbol=True), owner_object())

    def test_text_must_use_ha_and_lo(self):
        with self.assertRaisesRegex(ValueError, "one HA and one LO"):
            self.run_import(compiler_object(second_text_kind=6), owner_object())


if __name__ == "__main__":
    unittest.main()
