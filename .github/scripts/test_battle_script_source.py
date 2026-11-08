#!/usr/bin/env python3
"""Synthetic wire-format tests and source-only interpreter/schema checks."""

import copy
import json
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools import battle_script_format as fmt
from tools.compile_battle_scripts import (
    CATALOG_FORMAT, FORMAT, Compiler, SourceError, Symbol, load_source,
    no_duplicate_keys, parse_symbols,
)
from tools.data_match_diagnostics import elf_metadata
from tools.small_data_reference_diagnostics import (
    _balanced_body, _field_cases, _without_comments, source_schema,
)

NATIVE_BOUNDS = [ROOT / "config/GC6E01/data_aliases/sdata_80478D78.json"]


def address(symbol="state", **extra):
    return {"symbol": symbol, **extra}


def branch(symbol="script_entry", entry="entry"):
    return {"script": symbol, "entry": entry}


def fixture(body, size, name="script_entry", base=0x80000000, alignment=4):
    documents = {"synthetic.battle.json": {
        "format": FORMAT, "catalog": "catalog.json", "unit": "main/synthetic",
        "alignment": alignment, "objects": [{"symbol": name, "body": body}],
    }}
    catalog = {"format": CATALOG_FORMAT, "sources": list(documents)}
    symbols = {name: Symbol(base, size, ".data"),
               "state": Symbol(0x80400000, 8, ".bss"),
               "other_state": Symbol(0x80400020, 8, ".sdata")}
    return documents, catalog, symbols


def objects(data):
    return data[0]["synthetic.battle.json"]["objects"]


def binutil(name):
    for suffix in ("", ".exe"):
        path = ROOT / "build/binutils" / f"powerpc-eabi-{name}{suffix}"
        if path.is_file():
            return path
    raise unittest.SkipTest("PowerPC binutils are not downloaded; source validation remains active")


def c_function(path, name):
    text = _without_comments(path.read_text())
    definitions = list(re.finditer(r"(?m)^(?!asm\b)[A-Za-z_][^;{}]*\b" + re.escape(name)
                                  + r"\s*\([^;{}]*\)\s*\{", text))
    if len(definitions) != 1:
        raise AssertionError(f"ambiguous C handler: {name}")
    return _balanced_body(text, definitions[0].end() - 1)


class BattleSourceTests(unittest.TestCase):
    def compile(self, data):
        return Compiler(*data)

    def assemble(self, compiler, view="link"):
        assembler = binutil("as")
        assembly, summary = compiler.assembly("synthetic.battle.json", view)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "synthetic.s"
            obj = path.with_suffix(".o")
            path.write_text(assembly)
            subprocess.run([str(assembler), "-mgekko", "-o", str(obj), str(path)], check=True,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            metadata, payload = elf_metadata(obj)
        return metadata, payload, summary

    def relocations(self, metadata):
        return [(r["offset"], r["type"], r["symbol"], r["addend"])
                for r in metadata[".data"]["relocations"]]

    def test_every_address_is_relocated_and_unaligned_ones_resolve_for_comparison(self):
        data = fixture([
            {"command": "set_byte", "destination": address(), "value": 7},
            {"command": "jump", "target": branch("script_tail")},
            {"command": "return_sequence"},
        ], 12)
        objects(data).append({"symbol": "script_tail", "body": [{"command": "return_sequence"}]})
        data[2]["script_tail"] = Symbol(0x8000000C, 1, ".data")
        compiler = self.compile(data)
        metadata, payload, summary = self.assemble(compiler)
        self.assertEqual(payload[".data"], b"\x2e\0\0\0\0\x07\x28\0\0\0\0\x3c\x3c")
        section = metadata[".data"]
        self.assertEqual((section["size"], section["alignment"]), (13, 4))
        self.assertEqual([(s["name"], s["offset"], s["size"], s["type"], s["binding"])
                          for s in section["symbols"]],
                         [("script_entry", 0, 12, 1, 1), ("script_tail", 12, 1, 1, 1)])
        self.assertEqual(self.relocations(metadata),
                         [(1, 1, "state", 0), (7, 1, "script_tail", 0)])
        self.assertEqual((summary["size"], summary["resolved"]), (13, []))
        self.assertTrue(all(s["size"] == 0 for n, s in metadata.items() if n != ".data"))
        # The comparison object writes the same wire addresses without the
        # unaligned relocations that dtk's split target cannot carry.
        metadata, payload, summary = self.assemble(compiler, "comparison")
        self.assertEqual(payload[".data"], b"\x2e\x80\x40\0\0\x07\x28\x80\0\0\x0c\x3c\x3c")
        self.assertEqual(metadata[".data"]["relocations"], [])
        self.assertEqual([(r["offset"], r["value"]) for r in summary["resolved"]],
                         [(1, 0x80400000), (7, 0x8000000C)])

    def test_aligned_address_keeps_its_relocation_in_comparison_view(self):
        data = fixture([{"command": "end_move"}, {"command": "end_move"}, {"command": "end_move"},
                        {"command": "jump", "target": branch()}], 8)
        compiler = self.compile(data)
        for view in ("link", "comparison"):
            with self.subTest(view=view):
                metadata, payload, _ = self.assemble(compiler, view)
                self.assertEqual(payload[".data"], b"\x3d\x3d\x3d\x28\0\0\0\0")
                self.assertEqual(self.relocations(metadata), [(4, 1, "script_entry", 0)])

    def test_relocated_address_addend_is_native_rela(self):
        data = fixture([{"command": "set_byte", "destination": address(addend=3), "value": 42}], 6)
        metadata, payload, _ = self.assemble(self.compile(data))
        self.assertEqual(payload[".data"], b"\x2e\0\0\0\0\x2a")
        relocation = metadata[".data"]["relocations"][0]
        self.assertEqual((relocation["offset"], relocation["symbol"], relocation["addend"]),
                         (1, "state", 3))

    def test_byte_and_halfword_fields_have_packed_widths(self):
        data = fixture([{"command": "jump_if_halfword", "comparison": "less",
                         "address": address(), "value": 0x1234, "target": branch()}], 12)
        metadata, payload, _ = self.assemble(self.compile(data), "comparison")
        # The aligned branch target remains a relocation in both views.
        self.assertEqual(payload[".data"], b"\x2a\x03\x80\x40\0\0\x12\x34\0\0\0\0")
        self.assertEqual(self.relocations(metadata), [(8, 1, "script_entry", 0)])

    def test_message_legacy_stride_preserves_u32_lookahead(self):
        data = fixture([{"command": "print_selection_message_legacy", "message_prefix": 0x1234}], 3)
        objects(data).append({"symbol": "following_commands", "body": [
            {"command": "end_move"}, {"command": "end_sequence"}]})
        data[2]["following_commands"] = Symbol(0x80000003, 2, ".data")
        _, payload, _ = self.assemble(self.compile(data))
        self.assertEqual(payload[".data"], b"\x11\x12\x34\x3d\x3e")
        self.assertEqual(struct.unpack_from(">I", payload[".data"], 1)[0], 0x12343D3E)

    def test_ignored_target_keeps_relocation_without_a_branch_edge(self):
        data = fixture([{"command": "copy_target_stat_stages",
                         "ignored_failure_target": branch()},
                        {"command": "end_sequence"}], 6)
        metadata, payload, summary = self.assemble(self.compile(data))
        self.assertEqual(payload[".data"], b"\xbd\0\0\0\0\x3e")
        self.assertEqual(len(metadata[".data"]["relocations"]), 1)
        self.assertEqual(summary["edges"], [])

    def test_ignored_condition_byte_is_separate_from_numeric_word(self):
        data = fixture([{"command": "animate_selected_condition", "battler": "attacker",
                         "ignored_condition_kind": 1, "condition": 7}], 7)
        metadata, payload, _ = self.assemble(self.compile(data))
        self.assertEqual(payload[".data"], b"\x66\x11\x01\0\0\0\x07")
        self.assertEqual(metadata[".data"]["relocations"], [])

    def test_ignored_zero_word_is_still_four_bytes(self):
        data = fixture([{"command": "apply_intimidate", "ignored_operand": 0}], 5)
        _, payload, _ = self.assemble(self.compile(data))
        self.assertEqual(payload[".data"], b"\xe1\0\0\0\0")
        objects(data)[0]["body"][0]["ignored_operand"] = 1
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_scoped_legacy_pointers_are_not_immediate_animations(self):
        for name, symbol, base, value in (
                ("wsCmd46_pointer_ten", "lbl_80378AF1", 0x80378AF1, 10),
                ("wsCmd46_pointer_thirteen", "lbl_80378B1E", 0x80378B1E, 13)):
            node = {"command": "play_animation_indirect", "battler": "attacker",
                    "animation": {"legacy_address": name}, "argument": None}
            with self.subTest(name=name):
                data = fixture([node], 10, symbol, base, 1)
                for view in ("link", "comparison"):
                    metadata, payload, _ = self.assemble(self.compile(data), view)
                    self.assertEqual(payload[".data"], b"\x46\x11\0\0\0" + bytes([value]) + b"\0\0\0\0")
                    self.assertEqual(metadata[".data"]["relocations"], [])
                with self.assertRaises(SourceError):
                    self.compile(fixture([node], 10))
        # Each name belongs to exactly one original site.
        node = {"command": "play_animation_indirect", "battler": "attacker",
                "animation": {"legacy_address": "wsCmd46_pointer_eleven"}, "argument": None}
        with self.assertRaises(SourceError):
            self.compile(fixture([node], 10, "lbl_80378AF1", 0x80378AF1, 1))

    def test_no_numeric_address_or_general_legacy_escape(self):
        for invalid in (10, 0x80400000, {"legacy_address": 10},
                        {"legacy_address": "wsCmd46_pointer_ten"}, {"raw_bytes": [1, 2, 3, 4]},
                        {"symbol": "state", "encoding": "absolute"}):
            with self.subTest(invalid=invalid), self.assertRaises(SourceError):
                self.compile(fixture([{"command": "set_byte", "destination": invalid, "value": 1}], 6))

    def test_unknown_command_or_payload_field_rejected(self):
        for node in ({"command": "RAW_BYTES", "data": [0]},
                     {"command": "return_sequence", "raw": 0},
                     {"command": "set_byte", "destination": address(), "value": 3, "opcode": 46}):
            with self.subTest(node=node), self.assertRaises(SourceError):
                self.compile(fixture([node], 1))

    def test_scalar_bounds_and_enum_domains_rejected(self):
        for invalid in (-1, 256, True, "0x12"):
            with self.subTest(invalid=invalid), self.assertRaises(SourceError):
                self.compile(fixture([{"command": "set_byte", "destination": address(), "value": invalid}], 6))
        for invalid in (-1, 6, "approximately"):
            with self.subTest(invalid=invalid), self.assertRaises(SourceError):
                self.compile(fixture([{"command": "jump_if_byte", "comparison": invalid,
                                       "address": address(), "value": 3, "target": branch()}], 11))

    def test_symbol_extent_and_kind_are_enforced(self):
        data = fixture([{"command": "set_word_bits", "destination": address(addend=6), "mask": 1}], 9)
        with self.assertRaises(SourceError):
            self.compile(data)
        objects(data)[0]["body"][0]["destination"] = address()
        data[2]["state"] = Symbol(0x80400000, 8, ".text", "function")
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_transfer_bounds_apply_only_to_memory_not_branch_target(self):
        data = fixture([{"command": "jump_if_memory_equal", "left": address(),
                         "right": address("other_state"), "count": 8,
                         "target": branch("one_byte_tail")}], 14)
        objects(data).append({"symbol": "one_byte_tail", "body": [{"command": "return_sequence"}]})
        data[2]["one_byte_tail"] = Symbol(0x8000000E, 1, ".data")
        self.compile(data)
        data[2]["state"] = Symbol(0x80400000, 7, ".bss")
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_null_only_for_optional_address_fields(self):
        self.compile(fixture([{"command": "play_animation", "battler": "defender",
                               "animation": 7, "argument": None}], 7))
        with self.assertRaises(SourceError):
            self.compile(fixture([{"command": "set_byte", "destination": None, "value": 0}], 6))

    def test_branch_must_name_an_existing_command_label(self):
        data = fixture([{"command": "jump", "target": branch(entry="inside_operand")}], 5)
        with self.assertRaises(SourceError):
            self.compile(data)
        objects(data)[0]["body"][0]["target"] = {"script": "script_entry", "entry": "entry", "addend": 1}
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_local_branch_label_comes_from_command_widths(self):
        data = fixture([{"command": "jump", "target": branch(entry="finished")},
                        {"label": "finished", "command": "return_sequence"}], 6)
        _, payload, _ = self.assemble(self.compile(data), "comparison")
        self.assertEqual(payload[".data"], b"\x28\x80\0\0\x05\x3c")

    def test_dispatch_table_entries_are_relocated_script_entries(self):
        data = fixture([{"command": "end_move"}, {"label": "second", "command": "end_sequence"}], 2)
        objects(data).append({"symbol": "dispatch", "table": [branch(), branch(entry="second")],
                              "padding": 1})
        data[2]["dispatch"] = Symbol(0x80000002, 9, ".data")
        compiler = self.compile(data)
        metadata, payload, _ = self.assemble(compiler)
        self.assertEqual(payload[".data"], b"\x3d\x3e" + bytes(9))
        self.assertEqual(self.relocations(metadata),
                         [(2, 1, "script_entry", 0), (6, 1, "script_entry", 1)])
        metadata, payload, summary = self.assemble(compiler, "comparison")
        self.assertEqual(payload[".data"], b"\x3d\x3e\x80\0\0\0\x80\0\0\x01\0")
        self.assertEqual(len(summary["resolved"]), 2)
        for entry in ({"script": "dispatch", "entry": "entry"}, branch(entry="inside_operand"),
                      address("state")):
            objects(data)[1]["table"][0] = entry
            with self.subTest(entry=entry), self.assertRaises(SourceError):
                self.compile(data)
        objects(data)[1]["table"][0] = branch()
        objects(data)[1]["padding"] = 4
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_zero_filled_storage_has_declared_extent(self):
        data = fixture([{"command": "end_move"}], 1)
        objects(data).append({"symbol": "storage", "zero_fill": 7})
        data[2]["storage"] = Symbol(0x80000001, 7, ".data")
        metadata, payload, summary = self.assemble(self.compile(data))
        self.assertEqual(payload[".data"], b"\x3d" + bytes(7))
        self.assertEqual(summary["symbols"], [{"name": "script_entry", "size": 1},
                                              {"name": "storage", "size": 7}])
        for invalid in ({"symbol": "storage", "zero_fill": 6},
                        {"symbol": "storage", "zero_fill": 7, "bytes": [1]}):
            objects(data)[1] = invalid
            with self.subTest(invalid=invalid), self.assertRaises(SourceError):
                self.compile(data)

    def test_duplicate_labels_and_extent_mismatch_rejected(self):
        for body, size in (([{"label": "entry", "command": "end_move"}], 1),
                           ([{"label": "same", "command": "end_move"},
                             {"label": "same", "command": "end_sequence"}], 2),
                           ([{"command": "end_sequence"}], 2)):
            with self.subTest(body=body), self.assertRaises(SourceError):
                self.compile(fixture(body, size))

    def test_owner_gaps_and_overlaps_rejected(self):
        for offset in (0, 2):
            data = fixture([{"command": "end_move"}], 1)
            objects(data).append({"symbol": "second", "body": [{"command": "end_sequence"}]})
            data[2]["second"] = Symbol(0x80000000 + offset, 1, ".data")
            with self.subTest(offset=offset), self.assertRaises(SourceError):
                self.compile(data)

    def test_status_getter_and_setter_domains_are_distinct(self):
        node = {"command": "get_status_word", "family": "fight_floor", "context": None,
                "object_id": None, "field": 93, "index": None, "value": address()}
        self.compile(fixture([node], 20))
        node["command"] = "set_status_word"
        with self.assertRaises(SourceError):
            self.compile(fixture([node], 20))
        node["field"] = 62
        self.compile(fixture([node], 20))
        node["field"] = 65535
        with self.assertRaises(SourceError):
            self.compile(fixture([node], 20))

    def test_v1_sources_and_external_entries_are_rejected(self):
        data = fixture([{"command": "end_move"}], 1)
        data[1]["external_entries"] = []
        with self.assertRaises(SourceError):
            self.compile(data)
        data = fixture([{"command": "end_move"}], 1)
        document = data[0]["synthetic.battle.json"]
        document["programs"] = document.pop("objects")
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_duplicate_json_keys_and_catalog_path_escape_rejected(self):
        with self.assertRaises(SourceError):
            json.loads('{"command":"end_move","command":"end_sequence"}',
                       object_pairs_hook=no_duplicate_keys)
        data = fixture([{"command": "end_move"}], 1)
        data[1]["sources"] = ["../synthetic.battle.json"]
        with self.assertRaises(SourceError):
            self.compile(data)

    def test_symbol_parser_excludes_repeated_compiler_locals(self):
        symbols = parse_symbols("state = .data:0x80000000; // type:object size:0x4 scope:global\n"
                                "@2 = .data:0x80000100; // type:object size:0x4 scope:local\n"
                                "@2 = .data:0x80000200; // type:object size:0x4 scope:local\n")
        self.assertEqual(set(symbols), {"state"})
        with self.assertRaises(SourceError):
            parse_symbols("state = .data:0x80000000; // type:object size:0x4\n"
                          "state = .data:0x80000004; // type:object size:0x4\n")


class ExistingInterpreterTests(unittest.TestCase):
    def catalog(self):
        return load_source(ROOT / "src/game/battle_scripts/scripts_80375E60.battle.json",
                           ROOT / "config/GC6E01/symbols.txt", NATIVE_BOUNDS)

    def test_dispatch_identity_for_every_supported_command(self):
        text = _without_comments((ROOT / "src/game/data/rodata_80279B78.c").read_text())
        table = re.search(r"lbl_8027A00C\[255\]\s*=\s*\{([^{}]+)\}", text)
        self.assertIsNotNone(table)
        entries = [re.sub(r"^\(void\*\)\s*", "", x.strip()) for x in table[1].split(",") if x.strip()]
        self.assertEqual(len(entries), 255)
        for command in fmt.COMMANDS.values():
            with self.subTest(command=command.name):
                self.assertEqual(entries[command.opcode], command.handler)
                self.assertEqual(command.size, 1 + sum(fmt.FIELD_WIDTH[k.rstrip("?")]
                                                       for _, k in command.fields))

    def test_status_schema_matches_existing_getter_and_setter_field_switches(self):
        schema = source_schema(ROOT)
        self.assertEqual({kind: set(row[1]) for kind, row in schema["families"].items()},
                         {kind: set(values) for kind, values in fmt.STATUS_SET_FIELDS.items()})
        for kind, (name, path) in fmt.STATUS_GET_SOURCES.items():
            with self.subTest(getter=name):
                body = c_function(ROOT / path, name)
                self.assertEqual(_field_cases(body), set(fmt.STATUS_GET_FIELDS[kind]))

    def test_compatibility_quirks_remain_in_matching_c(self):
        body = re.sub(r"\s+", "", c_function(ROOT / "src/game/fight_range_exact_80226134.c", "wsCmd11"))
        self.assertIn("*(u32*)(lbl_8047B610+1)", body)
        self.assertRegex(body, r"lbl_8047B610(?:\+=3|=lbl_8047B610\+3);")
        body = re.sub(r"\s+", "", c_function(ROOT / "src/game/fight_range_exact_80222110.c", "wsCmd46"))
        self.assertIn("ptr1=*(void**)(pc+2);", body)
        self.assertIn("uVar1=*(u8*)ptr1;", body)
        self.assertIn("lbl_8047B610+=10;", body)

    def test_complete_source_catalog_preserves_physical_extent_and_dependencies(self):
        compiler, inputs = self.catalog()
        summaries = [compiler.assembly(name)[1] for name in compiler.owners]
        views = [compiler.assembly(name, "comparison")[1] for name in compiler.owners]
        self.assertEqual(len(summaries), 34)
        self.assertEqual(sum(s["size"] for s in summaries), 106756)
        self.assertEqual(sum(len(s["symbols"]) for s in summaries), 262)
        self.assertEqual(sum(s["commands"] for s in summaries), 3789)
        self.assertEqual(sum(len(s["relocations"]) for s in summaries), 1515)
        self.assertEqual(sum(len(s["relocations"]) for s in views), 331)
        self.assertEqual(sum(len(s["resolved"]) for s in views), 1184)
        self.assertTrue(all(path.resolve() in inputs for path in NATIVE_BOUNDS))

    def test_shifted_link_layout_moves_every_address_and_no_other_byte(self):
        """Link the real catalog twice with GNU ld: original and shifted layouts."""
        assembler, linker, objcopy = binutil("as"), binutil("ld"), binutil("objcopy")
        compiler, _ = self.catalog()
        owners = sorted(compiler.owners.values(), key=lambda owner: owner.start)
        start = owners[0].start
        owned = {program.symbol for owner in owners for program in owner.programs}
        summaries = []
        with tempfile.TemporaryDirectory() as directory:
            directory = Path(directory)
            inputs = []
            for owner in owners:
                assembly, summary = compiler.assembly(owner.filename, "link")
                source = directory / f"{owner.filename}.s"
                source.write_text(assembly)
                subprocess.run([str(assembler), "-mgekko", "-o", str(source.with_suffix(".o")),
                                str(source)], check=True)
                inputs.append(str(source.with_suffix(".o")))
                summaries.append(summary)
            external = sorted({r["expression"].partition("+")[0] for s in summaries
                               for r in s["relocations"]} - owned)
            self.assertTrue(external)

            def link(data_shift, external_shift):
                script = directory / "layout.ld"
                script.write_text(f"SECTIONS {{ . = {start + data_shift:#x}; .data : {{ *(.data) }}"
                                  " /DISCARD/ : { *(*) } }\n")
                elf = directory / f"layout_{data_shift:x}.elf"
                subprocess.run([str(linker), "-T", str(script), "-e", "0", "-o", str(elf),
                                *[f"--defsym={name}={compiler.symbols[name].address + external_shift:#x}"
                                  for name in external], *inputs], check=True)
                image = elf.with_suffix(".bin")
                subprocess.run([str(objcopy), "-O", "binary", "-j", ".data", str(elf), str(image)],
                               check=True)
                return image.read_bytes()

            original, shifted = link(0, 0), link(0x18, 0x1000)
        self.assertEqual(len(original), sum(owner.size for owner in owners))
        operands = bytearray(len(original))
        checked = 0
        for owner, summary in zip(owners, summaries):
            for relocation in summary["relocations"]:
                symbol, _, addend = relocation["expression"].partition("+")
                target = compiler.symbols[symbol].address + (int(addend) if addend else 0)
                offset = owner.start - start + relocation["offset"]
                self.assertEqual(int.from_bytes(original[offset:offset + 4], "big"), target)
                self.assertEqual(int.from_bytes(shifted[offset:offset + 4], "big"),
                                 target + (0x18 if symbol in owned else 0x1000))
                operands[offset:offset + 4] = b"\1\1\1\1"
                checked += 1
        self.assertEqual(checked, 1515)
        self.assertEqual([i for i, mask in enumerate(operands) if not mask and original[i] != shifted[i]], [])


if __name__ == "__main__":
    unittest.main()
