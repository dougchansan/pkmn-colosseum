#!/usr/bin/env python3
"""Reject equal-zero objects whose symbols would occupy wrong addresses."""

import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools.verify_zero_data_layout import check_object, expectations


def object_blob(symbols=None, size=8, kind=8, alignment=4, extra_data=False):
    symbols = symbols or [("first", 0, 4), ("second", 4, 4)]
    strings = bytearray(b"\0")
    table = bytearray(16)
    for name, offset, symbol_size in symbols:
        at = len(strings)
        strings.extend(name.encode() + b"\0")
        table.extend(struct.pack(">IIIBBH", at, offset, symbol_size, 0x11, 0, 1))
    # name, type, flags, payload, section size, link, info, alignment, entry size
    sections = [("", 0, 0, b"", 0, 0, 0, 1, 0),
                (".sbss", kind, 3, b"" if kind == 8 else bytes(size), size, 0, 0, alignment, 0),
                (".strtab", 3, 0, bytes(strings), len(strings), 0, 0, 1, 0),
                (".symtab", 2, 0, bytes(table), len(table), 2, 1, 4, 16)]
    if extra_data:
        sections.append((".data", 1, 3, bytes(4), 4, 0, 0, 4, 0))
    names = bytearray(b"\0")
    offsets = []
    for name in [row[0] for row in sections] + [".shstrtab"]:
        offsets.append(len(names))
        names.extend(name.encode() + b"\0")
    sections.append((".shstrtab", 3, 0, bytes(names), len(names), 0, 0, 1, 0))
    blob = bytearray(52)
    headers = []
    for index, row in enumerate(sections):
        name, kind, flags, data, size, link, info, align, entsize = row
        blob.extend(bytes(-len(blob) % align))
        at = len(blob)
        blob.extend(data)
        headers.append((offsets[index], kind, flags, 0, at, size, link, info, align, entsize))
    blob.extend(bytes(-len(blob) % 4))
    shoff = len(blob)
    for header in headers:
        blob.extend(struct.pack(">10I", *header))
    ident = b"\x7fELF\x01\x02\x01" + bytes(9)
    blob[:52] = struct.pack(">16sHHIIIIIHHHHHH", ident, 1, 20, 1, 0, 0,
                            shoff, 0, 52, 0, 0, 40, len(sections), len(sections) - 1)
    return bytes(blob)


def mixed_object_blob(symbols=None, *, bss_size=8, sbss_size=8,
                      extra_section=None, omit=None, initialized=None,
                      uninitialized=None, relocation=None, alignment=4):
    """Synthetic code/data plus two independent zero sections; no retail input."""
    if symbols is None:
        symbols = [("private_note", ".bss", 0, 4, 0x01),
                   ("current", ".sbss", 0, 1, 0x01),
                   ("public_note", ".bss", 4, 4, 0x11),
                   ("root", ".sbss", 4, 4, 0x11),
                   ("run", ".text", 0, 4, 0x12),
                   ("table", ".data", 0, 4, 0x11)]
    spans = [(".bss", 8, 3, bss_size, alignment),
             (".sbss", 8, 3, sbss_size, alignment),
             (".text", 1, 6, 4, 4), (".data", 1, 3, 4, 4)]
    if extra_section:
        spans.append(extra_section)
    spans = [(n, 1 if n == initialized else 8 if n == uninitialized else k, f, z, a)
             for n, k, f, z, a in spans if n != omit]
    section_names = [""] + [s[0] for s in spans]
    if relocation:
        section_names += [".rela" + relocation]
    section_names += [".strtab", ".symtab", ".shstrtab"]
    indices = {n: i for i, n in enumerate(section_names)}
    symbols = sorted(symbols, key=lambda row: row[4] >> 4)
    strings = bytearray(b"\0")
    table = bytearray(16)
    for name, section, value, size, info in symbols:
        if section == omit:
            continue
        at = len(strings)
        strings.extend(name.encode() + b"\0")
        table.extend(struct.pack(">IIIBBH", at, value, size, info, 0, indices[section]))
    shstrings = b"\0".join(n.encode() for n in section_names) + b"\0"
    records = [(n, k, f, b"" if k == 8 else bytes(z), z, 0, 0, a, 0)
               for n, k, f, z, a in spans]
    if relocation:
        records.append((".rela" + relocation, 4, 0, struct.pack(">IIi", 0, 1, 0),
                        12, indices[".symtab"], indices[relocation], 4, 12))
    local_count = 1 + sum(row[4] >> 4 == 0 and row[1] != omit for row in symbols)
    records += [(".strtab", 3, 0, bytes(strings), len(strings), 0, 0, 1, 0),
                (".symtab", 2, 0, bytes(table), len(table), indices[".strtab"], local_count, 4, 16),
                (".shstrtab", 3, 0, shstrings, len(shstrings), 0, 0, 1, 0)]
    blob = bytearray(52)
    headers = [(0,) * 10]
    for name, kind, flags, data, size, link, info, align, entsize in records:
        blob.extend(bytes(-len(blob) % max(align, 1)))
        headers.append((shstrings.index(name.encode() + b"\0"), kind, flags, 0,
                        len(blob), size, link, info, align, entsize))
        blob.extend(data)
    blob.extend(bytes(-len(blob) % 4))
    shoff = len(blob)
    blob.extend(b"".join(struct.pack(">10I", *h) for h in headers))
    blob[:52] = struct.pack(">16sHHIIIIIHHHHHH", b"\x7fELF\x01\x02\x01" + bytes(9),
                            1, 20, 1, 0, 0, shoff, 0, 52, 0, 0, 40,
                            len(headers), indices[".shstrtab"])
    return bytes(blob)


class ZeroDataLayoutTests(unittest.TestCase):
    def expected(self):
        return {"section": ".sbss", "start": 0x80470000, "size": 8,
                "symbols": [{"name": "first", "offset": 0, "size": 4},
                            {"name": "second", "offset": 4, "size": 4}]}

    def test_correct_symbol_layout_passes(self):
        self.assertEqual(check_object(object_blob(), self.expected()), [])

    def test_swapped_offsets_fail_despite_identical_zero_section(self):
        errors = check_object(object_blob([("first", 4, 4), ("second", 0, 4)]), self.expected())
        self.assertEqual(len(errors), 2)

    def test_wrong_symbol_extent_fails_despite_identical_zero_section(self):
        self.assertTrue(check_object(object_blob([("first", 0, 8), ("second", 4, 4)]), self.expected()))

    def test_missing_and_duplicate_definitions_fail(self):
        self.assertTrue(check_object(object_blob([("first", 0, 4)]), self.expected()))
        self.assertTrue(check_object(object_blob([("first", 0, 4), ("first", 0, 4),
                                                  ("second", 4, 4)]), self.expected()))

    def test_extra_storage_and_initialized_zero_storage_fail(self):
        self.assertTrue(check_object(object_blob(extra_data=True), self.expected()))
        self.assertTrue(check_object(object_blob(size=12), self.expected()))
        self.assertTrue(check_object(object_blob(kind=1), self.expected()))

    def test_incompatible_link_alignment_fails(self):
        expected = self.expected()
        expected["start"] += 4
        self.assertTrue(check_object(object_blob(alignment=8), expected))

    def test_implicit_gap_needs_no_manufactured_storage_symbol(self):
        expected = self.expected()
        expected["symbols"][0]["size"] = 1
        expected["gaps"] = [{"offset": 1, "size": 3}]
        self.assertEqual(check_object(object_blob([("first", 0, 1), ("second", 4, 4)]), expected), [])

    def test_explicit_padding_must_fit_independent_gap_and_name_address(self):
        expected = self.expected()
        expected["symbols"][0]["size"] = 1
        expected["gaps"] = [{"offset": 1, "size": 3}]
        base = [("first", 0, 1), ("second", 4, 4)]
        self.assertEqual(check_object(object_blob(base + [("padding_80470001", 1, 1),
                                                         ("padding_80470002", 2, 2)]), expected), [])
        for extra in [[("padding_80470001", 1, 4)],
                      [("padding_80470002", 1, 1)],
                      [("unmapped", 1, 3)],
                      [("padding_80470001", 1, 3), ("padding_80470002", 2, 1)]]:
            self.assertTrue(check_object(object_blob(base + extra), expected), extra)

    def test_metadata_derives_local_alias_and_gap_without_source_declarations(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            (base / "units.json").write_text(json.dumps({"version": 1, "units": ["game/data/test.c"]}))
            (base / "splits.txt").write_text("game/data/test.c:\n\t.sbss start:0x80470000 end:0x80470008\n")
            (base / "symbols.txt").write_text(
                "first = .sbss:0x80470000; // type:object size:0x1 scope:local\n"
                "second = .sbss:0x80470004; // type:object size:0x4\n")
            result = expectations(base / "units.json", base / "symbols.txt", base / "splits.txt")
            self.assertEqual(result[0]["symbols"], [
                {"name": "first_80470000", "offset": 0, "size": 1},
                {"name": "second", "offset": 4, "size": 4}])
            self.assertEqual(result[0]["gaps"], [{"offset": 1, "size": 3}])
            (base / "splits.txt").write_text("game/data/test.c:\n\t.sbss start:0x80470000 end:0x80470006\n")
            with self.assertRaises(ValueError):
                expectations(base / "units.json", base / "symbols.txt", base / "splits.txt")

    def test_v2_string_registration_preserves_legacy_global_alias_binding(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            (base / "units.json").write_text(json.dumps({"version": 2, "units": ["game/data/test.c"]}))
            (base / "splits.txt").write_text("game/data/test.c:\n\t.sbss start:0x80470000 end:0x80470008\n")
            (base / "symbols.txt").write_text(
                "first = .sbss:0x80470000; // type:object size:0x4 scope:local\n"
                "second = .sbss:0x80470004; // type:object size:0x4 scope:weak\n")
            expected = expectations(base / "units.json", base / "symbols.txt", base / "splits.txt")[0]
            self.assertEqual(check_object(object_blob([("first_80470000", 0, 4),
                                                       ("second", 4, 4)]), expected), [])
            self.assertTrue(check_object(object_blob(), expected))

    def test_storage_only_owner_still_requires_full_physical_extent(self):
        expected = self.expected()
        expected["symbols"] = expected["symbols"][:1]
        expected["gaps"] = [{"offset": 4, "size": 4}]
        self.assertEqual(check_object(object_blob([("first", 0, 4)], alignment=8), expected), [])
        self.assertTrue(check_object(object_blob([("first", 0, 4)], size=4, alignment=8), expected))


class MixedZeroDataLayoutTests(unittest.TestCase):
    SPLITS = ("library/test.c:\n"
              "\t.text start:0x80000000 end:0x80000004\n"
              "\t.data start:0x80300000 end:0x80300004\n"
              "\t.bss start:0x80400000 end:0x80400008\n"
              "\t.sbss start:0x80470000 end:0x80470008\n")
    SYMBOLS = ("private_note = .bss:0x80400000; // type:object size:0x4 scope:local\n"
               "public_note = .bss:0x80400004; // type:object size:0x4\n"
               "current = .sbss:0x80470000; // type:object size:0x1 scope:local\n"
               "root = .sbss:0x80470004; // type:object size:0x4\n")

    def entry(self):
        return {"unit": "library/test.c", "sections": [".bss", ".sbss"],
                "local_symbols": {"private_note": "private_note", "current": "current"}}

    def metadata(self, entry=None, splits=None, symbols=None, version=2):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            (base / "units.json").write_text(json.dumps({"version": version, "units": [
                self.entry() if entry is None else entry]}))
            (base / "splits.txt").write_text(self.SPLITS if splits is None else splits)
            (base / "symbols.txt").write_text(self.SYMBOLS if symbols is None else symbols)
            return expectations(base / "units.json", base / "symbols.txt", base / "splits.txt")[0]

    def symbols(self):
        return [("private_note", ".bss", 0, 4, 0x01),
                ("current", ".sbss", 0, 1, 0x01),
                ("public_note", ".bss", 4, 4, 0x11),
                ("root", ".sbss", 4, 4, 0x11),
                ("run", ".text", 0, 4, 0x12),
                ("table", ".data", 0, 4, 0x11)]

    def test_mixed_owner_checks_both_zero_sections_and_exact_local_names(self):
        expected = self.metadata()
        self.assertEqual(expected["size"], 16)
        self.assertEqual(expected["existing_symbols"], 4)
        self.assertEqual(len(expected["sections"]), 2)
        self.assertEqual(expected["sections"][1]["gaps"], [{"offset": 1, "size": 3}])
        self.assertEqual(check_object(mixed_object_blob(), expected), [])

    def test_private_alias_is_exact_and_must_keep_local_binding(self):
        entry = self.entry()
        entry["local_symbols"]["private_note"] = "note_storage"
        expected = self.metadata(entry)
        symbols = self.symbols()
        symbols[0] = ("note_storage", ".bss", 0, 4, 0x01)
        self.assertEqual(check_object(mixed_object_blob(symbols), expected), [])
        self.assertTrue(check_object(mixed_object_blob(), expected))
        symbols[0] = ("note_storage", ".bss", 0, 4, 0x11)
        self.assertTrue(check_object(mixed_object_blob(symbols), expected))

    def test_wrong_offsets_sizes_and_bindings_fail_in_mixed_unit(self):
        expected = self.metadata()
        for replacement in [("private_note", ".bss", 4, 4, 0x01),
                            ("private_note", ".bss", 0, 8, 0x01),
                            ("private_note", ".bss", 0, 4, 0x11),
                            ("private_note", ".bss", 0, 4, 0x21),
                            ("private_note", ".bss", 0, 4, 0x02),
                            ("private_note", ".sbss", 0, 4, 0x01)]:
            symbols = self.symbols()
            symbols[0] = replacement
            self.assertTrue(check_object(mixed_object_blob(symbols), expected), replacement)
        symbols = self.symbols()
        symbols[2] = ("public_note", ".bss", 4, 4, 0x01)
        self.assertTrue(check_object(mixed_object_blob(symbols), expected))

    def test_lost_duplicate_and_extra_private_symbols_fail(self):
        expected = self.metadata()
        symbols = self.symbols()
        self.assertTrue(check_object(mixed_object_blob(symbols[1:]), expected))
        self.assertTrue(check_object(mixed_object_blob(symbols + [symbols[0]]), expected))
        self.assertTrue(check_object(mixed_object_blob(symbols + [
            ("unmapped", ".sbss", 1, 3, 0x01)]), expected))

    def test_address_suffix_is_not_an_implicit_alias_for_native_private_symbol(self):
        symbols = self.symbols()
        symbols[0] = ("private_note_80400000", ".bss", 0, 4, 0x01)
        self.assertTrue(check_object(mixed_object_blob(symbols), self.metadata()))

    def test_missing_zero_section_and_wrong_extent_fail(self):
        expected = self.metadata()
        self.assertTrue(check_object(mixed_object_blob(omit=".bss"), expected))
        self.assertTrue(check_object(mixed_object_blob(bss_size=12), expected))
        self.assertTrue(check_object(mixed_object_blob(sbss_size=12), expected))
        self.assertTrue(check_object(mixed_object_blob(initialized=".sbss"), expected))

    def test_undeclared_allocated_sections_and_hidden_zero_storage_fail(self):
        expected = self.metadata()
        for extra in [(".sbss2", 8, 2, 4, 4), (".rodata", 1, 2, 4, 4)]:
            self.assertTrue(check_object(mixed_object_blob(extra_section=extra), expected))
        # A declared initialized-data section cannot hide extra NOBITS storage.
        self.assertTrue(check_object(mixed_object_blob(
            uninitialized=".data"), expected))

    def test_zero_section_relocations_reject_but_text_relocations_are_allowed(self):
        expected = self.metadata()
        self.assertEqual(check_object(mixed_object_blob(relocation=".text"), expected), [])
        for section in (".bss", ".sbss"):
            self.assertTrue(check_object(mixed_object_blob(relocation=section), expected))

    def test_alignment_and_padding_rules_remain_strict_for_mixed_unit(self):
        expected = self.metadata()
        expected["sections"][0]["start"] += 4
        self.assertTrue(check_object(mixed_object_blob(alignment=8), expected))
        expected = self.metadata()
        symbols = self.symbols()
        valid = ("padding_80470001", ".sbss", 1, 3, 0x11)
        self.assertEqual(check_object(mixed_object_blob(symbols + [valid]), expected), [])
        for invalid in [("padding_80470001", ".sbss", 1, 4, 0x11),
                        ("padding_80470002", ".sbss", 1, 3, 0x11),
                        ("padding_80470001", ".sbss", 1, 3, 0x01)]:
            self.assertTrue(check_object(mixed_object_blob(symbols + [invalid]), expected))

    def trailing_case(self):
        entry = self.entry()
        del entry["local_symbols"]["current"]
        mapped = self.SYMBOLS.replace(
            "current = .sbss:0x80470000; // type:object size:0x1 scope:local\n", "")
        mapped = mapped.replace("root = .sbss:0x80470004", "root = .sbss:0x80470000")
        native = [row for row in self.symbols() if row[0] != "current"]
        native = [(n, s, 0 if n == "root" else o, z, i) for n, s, o, z, i in native]
        return self.metadata(entry, symbols=mapped), native

    def test_mixed_terminal_alignment_is_observed_and_preserves_physical_size(self):
        expected, native = self.trailing_case()
        observations = []
        self.assertEqual(check_object(mixed_object_blob(native, sbss_size=4, alignment=8),
                                      expected, observations=observations), [])
        self.assertEqual(expected["size"], 16)
        self.assertEqual(observations[1], {
            "section": ".sbss", "start": 0x80470000, "physical_size": 8,
            "native_size": 4, "alignment": 8, "last_symbol_end": 4,
            "terminal_alignment_bytes": 4})
        self.assertEqual(observations[0]["terminal_alignment_bytes"], 0)

    def test_terminal_alignment_cannot_hide_lost_or_altered_state(self):
        expected, native = self.trailing_case()
        missing = [row for row in native if row[0] != "root"]
        self.assertTrue(check_object(mixed_object_blob(missing, sbss_size=4, alignment=8), expected))
        for replacement in [("root", ".sbss", 1, 4, 0x11),
                            ("root", ".sbss", 0, 3, 0x11),
                            ("root", ".sbss", 0, 8, 0x11),
                            ("root", ".sbss", 0, 4, 0x01),
                            ("root", ".sbss", 0, 4, 0x21)]:
            mutated = [replacement if row[0] == "root" else row for row in native]
            self.assertTrue(check_object(mixed_object_blob(mutated, sbss_size=4, alignment=8),
                                         expected), replacement)

    def test_terminal_alignment_requires_exact_native_symbol_end(self):
        expected, native = self.trailing_case()
        for size in (0, 3, 5, 7, 9):
            self.assertTrue(check_object(mixed_object_blob(native, sbss_size=size, alignment=8),
                                         expected), size)
        self.assertTrue(check_object(mixed_object_blob(native, sbss_size=4, alignment=8,
                                                       initialized=".sbss"), expected))

    def test_terminal_alignment_cannot_invent_or_enlarge_alignment(self):
        expected, native = self.trailing_case()
        for alignment in (0, 3, 4, 16):
            self.assertTrue(check_object(mixed_object_blob(native, sbss_size=4,
                                                           alignment=alignment), expected), alignment)
        expected["sections"][1]["start"] += 4
        self.assertTrue(check_object(mixed_object_blob(native, sbss_size=4, alignment=8), expected))

    def test_terminal_alignment_requires_independently_mapped_tail_gap(self):
        expected, native = self.trailing_case()
        for gaps in ([], [{"offset": 3, "size": 5}], [{"offset": 4, "size": 8}]):
            expected["sections"][1]["gaps"] = gaps
            self.assertTrue(check_object(mixed_object_blob(native, sbss_size=4, alignment=8),
                                         expected), gaps)
        expected["sections"][1]["gaps"] = [{"offset": 4, "size": 12}]
        expected["sections"][1]["size"] = 16
        self.assertTrue(check_object(mixed_object_blob(native, sbss_size=4, alignment=8), expected))

    def test_terminal_alignment_rejects_extra_or_partially_materialized_padding(self):
        expected, native = self.trailing_case()
        for extra in [("unknown", ".sbss", 0, 4, 0x01),
                      ("padding_80470000", ".sbss", 0, 4, 0x11),
                      ("padding_80470004", ".sbss", 4, 4, 0x11),
                      ("padding_80470004", ".sbss", 4, 1, 0x11)]:
            self.assertTrue(check_object(mixed_object_blob(native + [extra], sbss_size=4,
                                                           alignment=8), expected), extra)
        partial = native + [("padding_80470004", ".sbss", 4, 1, 0x11)]
        self.assertTrue(check_object(mixed_object_blob(partial, sbss_size=5, alignment=8), expected))

    def test_terminal_alignment_preserves_independent_interior_gaps(self):
        mapped = self.SYMBOLS.replace(
            "current = .sbss:0x80470000; // type:object size:0x1 scope:local",
            "current = .sbss:0x80470000; // type:object size:0xE scope:local")
        mapped = mapped.replace("root = .sbss:0x80470004; // type:object size:0x4",
                                "root = .sbss:0x80470010; // type:object size:0xE")
        mapped += "callback = .sbss:0x80470020; // type:object size:0x1C\n"
        expected = self.metadata(symbols=mapped,
                                 splits=self.SPLITS.replace("end:0x80470008", "end:0x80470040"))
        native = [row for row in self.symbols() if row[1] != ".sbss"]
        native += [("current", ".sbss", 0, 14, 0x01),
                   ("root", ".sbss", 16, 14, 0x11),
                   ("callback", ".sbss", 32, 28, 0x11)]
        self.assertEqual(expected["sections"][1]["gaps"], [
            {"offset": 14, "size": 2}, {"offset": 30, "size": 2}, {"offset": 60, "size": 4}])
        self.assertEqual(check_object(mixed_object_blob(native, sbss_size=60, alignment=8), expected), [])
        shifted = [(n, s, 14 if n == "root" else o, z, i) for n, s, o, z, i in native]
        self.assertTrue(check_object(mixed_object_blob(shifted, sbss_size=60, alignment=8), expected))
        for extra in [("padding_8047000E", ".sbss", 14, 3, 0x11),
                      ("padding_8047001E", ".sbss", 30, 4, 0x11)]:
            self.assertTrue(check_object(mixed_object_blob(native + [extra], sbss_size=60,
                                                           alignment=8), expected), extra)

    def test_partial_registration_cannot_skip_a_declared_zero_section(self):
        entry = self.entry()
        entry["sections"] = [".bss"]
        with self.assertRaises(ValueError):
            self.metadata(entry)
        entry["sections"] = [".bss", ".sbss", ".sbss2"]
        with self.assertRaises(ValueError):
            self.metadata(entry)

    def test_private_maps_cannot_ignore_symbols_or_override_globals(self):
        for mappings in [{"private_note": "private_note"},
                         {"private_note": "private_note", "current": "current", "root": "other"},
                         {"private_note": "private_note", "current": "current", "unknown": "other"},
                         {"private_note": "same", "current": "same"},
                         {"private_note": "public_note", "current": "current"},
                         {"private_note": "private*", "current": "current"}]:
            entry = self.entry()
            entry["local_symbols"] = mappings
            with self.assertRaises(ValueError, msg=str(mappings)):
                self.metadata(entry)

    def test_string_registration_cannot_enable_mixed_mode(self):
        with self.assertRaises(ValueError):
            self.metadata(entry="library/test.c")
        with self.assertRaises(ValueError):
            self.metadata(version=1)

    def test_bisected_overlapping_and_foreign_owned_storage_fail(self):
        for symbols, splits in [
                (self.SYMBOLS.replace('size:0x4 scope:local', 'size:0x9 scope:local'), self.SPLITS),
                (self.SYMBOLS.replace('public_note = .bss:0x80400004', 'public_note = .bss:0x80400002'), self.SPLITS),
                (self.SYMBOLS, self.SPLITS + 'other.c:\n\t.sbss start:0x80470004 end:0x80470008\n')]:
            with self.assertRaises(ValueError):
                self.metadata(symbols=symbols, splits=splits)

    def test_manifest_rejects_duplicate_keys_and_unrecognized_options(self):
        entry = self.entry()
        entry["allow_global_aliases"] = True
        with self.assertRaises(ValueError):
            self.metadata(entry)
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            (base / "units.json").write_text('{"version":2,"version":1,"units":["test.c"]}')
            (base / "splits.txt").write_text('test.c:\n\t.sbss start:0x80470000 end:0x80470008\n')
            (base / "symbols.txt").write_text(self.SYMBOLS)
            with self.assertRaises(ValueError):
                expectations(base / "units.json", base / "symbols.txt", base / "splits.txt")


if __name__ == "__main__":
    unittest.main()
