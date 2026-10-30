#!/usr/bin/env python3
"""Ensure address diagnostics never credit fallback or export payloads."""

import json
import copy
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools.data_link_diagnostics import diagnose, linked_symbols, load_array_aliases, map_symbols


def executable_fixture():
    names = b"\0shifted\0local_80001004\0"
    payload = b"PRIVATE_GAME_BYTES"
    symbols = bytes(16) + struct.pack(">IIIBBH", 1, 0x80001004, 4, 0x11, 0, 1) \
        + struct.pack(">IIIBBH", 9, 0x80001004, 4, 0x11, 0, 1)
    blob = bytearray(52)
    records = [(0,) * 10]
    for kind, value, address, link, info, width in (
            (1, payload, 0x80001000, 0, 0, 0),
            (3, names, 0, 0, 0, 0), (2, symbols, 0, 2, 1, 16)):
        offset = len(blob)
        blob += value
        records.append((0, kind, 3 if kind == 1 else 0, address, offset,
                        len(value), link, info, 4, width))
    at = len(blob)
    for record in records:
        blob += struct.pack(">10I", *record)
    struct.pack_into(">16sHHIIIIIHHHHHH", blob, 0,
                     b"\x7fELF\x01\x02\x01" + bytes(9), 2, 20, 1, 0, 0, at,
                     0, 52, 0, 0, 40, len(records), 0)
    return blob


class DataLinkDiagnosticTests(unittest.TestCase):
    def mixed_fixture(self):
        unit = {"name": "main/library/owner",
                "measures": {"total_code": "4", "complete_code": "4",
                             "total_data": "8", "complete_data": "8"},
                "metadata": {"source_path": "src/library/owner.c", "complete": True},
                "sections": [{"name": ".text", "size": "4",
                              "metadata": {"virtual_address": str(0x80000000)}},
                             {"name": ".sbss", "size": "8",
                              "metadata": {"virtual_address": str(0x80470000)}}]}
        expected = map_symbols("state = .sbss:0x80470000; // type:object size:0x4")
        actual = {"state": [{"address": 0x80470000, "size": 4}]}
        return {"units": [unit]}, expected, actual

    def test_explicit_mixed_owner_exposes_linked_state_and_address_errors(self):
        report, expected, actual = self.mixed_fixture()
        self.assertEqual(diagnose(report, expected, actual)["units"], [])
        selected = ["main/library/owner"]
        result = diagnose(report, expected, actual, include_units=selected)
        self.assertEqual(result["explicit_units"], selected)
        self.assertEqual(result["units"][0]["symbols"][0]["status"], "match")
        actual["state"][0]["address"] += 4
        result = diagnose(report, expected, actual, include_units=selected)
        self.assertEqual(result["units"][0]["symbols"][0]["status"], "address_mismatch")

    def test_explicit_selection_cannot_silently_skip_missing_or_unproven_sources(self):
        for mutation in ("missing", "duplicate", "incomplete", "no_source", "code", "data", "module"):
            report, expected, actual = self.mixed_fixture()
            unit = report["units"][0]
            selected = [unit["name"]]
            if mutation == "missing":
                report["units"] = []
            elif mutation == "duplicate":
                report["units"].append(copy.deepcopy(unit))
            elif mutation == "incomplete":
                unit["metadata"]["complete"] = False
            elif mutation == "no_source":
                unit["metadata"].pop("source_path")
            elif mutation in ("code", "data"):
                unit["measures"]["complete_" + mutation] = "0"
            else:
                unit["name"] = "rel/library/owner"
                selected = [unit["name"]]
            with self.subTest(mutation=mutation), self.assertRaises(ValueError):
                diagnose(report, expected, actual, include_units=selected)

    def alias_fixture(self):
        expected = map_symbols("\n".join([
            "flags = .sdata:0x80001000; // type:object size:0x1",
            "flag_three = .sdata:0x80001003; // type:object size:0x1",
            "flag_five = .sdata:0x80001005; // type:object size:0x1",
            "unrelated = .sdata:0x80001008; // type:object size:0x4",
        ]))
        report = {"units": [{
            "name": "main/game/data/sdata_80001000",
            "measures": {"total_data": "12", "complete_data": "12", "total_code": "0"},
            "metadata": {"source_path": "src/game/data/sdata_80001000.c", "complete": True},
            "sections": [{"name": ".sdata", "size": "12",
                          "metadata": {"virtual_address": str(0x80001000)}}],
        }]}
        actual = {name: [{"address": address, "size": size}] for name, address, size in (
            ("flags", 0x80001000, 8), ("flag_three", 0x80001003, 0),
            ("flag_five", 0x80001005, 0), ("unrelated", 0x80001008, 4))}
        groups = [{"base": "flags", "size": 8, "aliases": [
            {"name": "flag_three", "offset": 3, "size": 1},
            {"name": "flag_five", "offset": 5, "size": 1}]}]
        return expected, report, actual, groups

    def registrations(self, directory, expected, groups):
        path = Path(directory) / "sdata_80001000.json"
        path.write_text(json.dumps(groups))
        return load_array_aliases(Path(directory), expected)

    def test_address_shift_alias_missing_and_incomplete_selection(self):
        def unit(name, start, complete):
            return {"name": "main/" + name,
                    "measures": {"total_data": "12", "total_code": "0",
                                 "complete_data": "12" if complete else "0"},
                    "metadata": {"source_path": "src/" + name + ".c", "complete": complete},
                    "sections": [{"name": ".data", "size": "12",
                                  "metadata": {"virtual_address": str(start)}}]}
        expected = map_symbols("\n".join([
            "shifted = .data:0x80001000; // type:object size:0x4",
            "local = .data:0x80001004; // type:object size:0x4 scope:local",
            "stripped = .data:0x80001008; // type:object size:0x4",
            "candidate = .data:0x80002000; // type:object size:0x4",
        ]))
        result = diagnose({"units": [unit("linked", 0x80001000, True),
                                     unit("candidate", 0x80002000, False)]},
                          expected, linked_symbols(executable_fixture()))
        self.assertEqual(result["skipped_incomplete_units"], ["main/candidate"])
        self.assertEqual([r["status"] for r in result["units"][0]["symbols"]],
                         ["address_mismatch", "match", "missing"])
        self.assertEqual(result["units"][0]["symbols"][1]["linked_name"], "local_80001004")

    def test_payload_is_not_returned_and_truncated_elf_is_rejected(self):
        blob = executable_fixture()
        result = linked_symbols(blob)
        self.assertNotIn("PRIVATE_GAME_BYTES", json.dumps(result))
        self.assertEqual(set(result), {"shifted", "local_80001004"})
        with self.assertRaises(ValueError):
            linked_symbols(blob[:-1])
        struct.pack_into(">H", blob, 16, 1)
        with self.assertRaises(ValueError):
            linked_symbols(blob)

    def test_registered_native_bounds_and_zero_labels_preserve_original_fragments(self):
        expected, report, actual, groups = self.alias_fixture()
        ordinary = diagnose(report, expected, actual)
        self.assertNotIn("array_aliases", ordinary)
        self.assertEqual(ordinary["summary"]["symbol_counts"]["size_mismatch"], 3)
        with tempfile.TemporaryDirectory() as directory:
            aliases = self.registrations(directory, expected, groups)
            result = diagnose(report, expected, actual, aliases)
        rows = result["units"][0]["symbols"]
        self.assertEqual([row["status"] for row in rows], ["match"] * 4)
        self.assertEqual([row["expected_size"] for row in rows], [1, 1, 1, 4])
        self.assertEqual([row["source_array_layout"]["expected_source_size"] for row in rows[:3]],
                         [8, 0, 0])
        self.assertEqual(rows[0]["source_array_layout"]["kind"], "complete_array")
        self.assertEqual(rows[1]["source_array_layout"]["kind"], "interior_label")
        self.assertNotIn("source_array_layout", rows[3])
        self.assertEqual(result["array_aliases"]["complete_arrays"], 1)
        self.assertEqual(result["array_aliases"]["interior_labels"], 2)

    def test_alias_registration_does_not_hide_wrong_bounds_addresses_or_unrelated_sizes(self):
        expected, report, actual, groups = self.alias_fixture()
        with tempfile.TemporaryDirectory() as directory:
            aliases = self.registrations(directory, expected, groups)
            actual["flags"][0]["size"] = 1
            actual["flag_three"][0]["address"] += 1
            actual["unrelated"][0]["size"] = 2
            del actual["flag_five"]
            result = diagnose(report, expected, actual, aliases)
        self.assertEqual([row["status"] for row in result["units"][0]["symbols"]],
                         ["size_mismatch", "address_mismatch", "missing", "size_mismatch"])

    def test_alias_groups_reject_malformed_unmapped_and_intersecting_storage(self):
        expected, _, _, groups = self.alias_fixture()
        variants = []
        for change in ({"size": True}, {"size": 16}, {"base": "missing"}, {"extra": 0}):
            changed = copy.deepcopy(groups)
            changed[0].update(change)
            variants.append(changed)
        for change in ({"name": "missing"}, {"offset": 4}, {"size": 6}, {"offset": True}):
            changed = copy.deepcopy(groups)
            changed[0]["aliases"][0].update(change)
            variants.append(changed)
        variants.append(groups + groups)
        with tempfile.TemporaryDirectory() as directory:
            for value in variants:
                with self.subTest(value=value), self.assertRaises(ValueError):
                    self.registrations(directory, expected, value)
            path = Path(directory) / "sdata_80001000.json"
            path.write_text(json.dumps(groups).replace('"base": "flags"',
                                                     '"base": "flags", "base": "flags"'))
            with self.assertRaisesRegex(ValueError, "Duplicate"):
                load_array_aliases(Path(directory), expected)

    def test_alias_host_must_fit_the_named_native_source_owner(self):
        expected, report, actual, groups = self.alias_fixture()
        with tempfile.TemporaryDirectory() as directory:
            aliases = self.registrations(directory, expected, groups)
            for field in ("short_section", "wrong_source", "code_owner", "missing_owner"):
                changed = copy.deepcopy(report)
                unit = changed["units"][0]
                if field == "short_section":
                    unit["sections"][0]["size"] = "7"
                elif field == "wrong_source":
                    unit["metadata"]["source_path"] = "src/unrelated.c"
                elif field == "code_owner":
                    unit["measures"]["total_code"] = "4"
                else:
                    changed["units"] = []
                with self.subTest(field=field), self.assertRaises(ValueError):
                    diagnose(changed, expected, actual, aliases)

    def test_cli_without_alias_option_keeps_existing_classification(self):
        report = {"units": [{
            "name": "main/linked", "measures": {"total_data": "4", "complete_data": "4"},
            "metadata": {"source_path": "src/linked.c", "complete": True},
            "sections": [{"name": ".data", "size": "4",
                          "metadata": {"virtual_address": str(0x80001000)}}],
        }]}
        symbols = "shifted = .data:0x80001000; // type:object size:0x4\n"
        wanted = diagnose(report, map_symbols(symbols), linked_symbols(executable_fixture()))
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "main.elf").write_bytes(executable_fixture())
            (root / "report.json").write_text(json.dumps(report))
            (root / "symbols.txt").write_text(symbols)
            process = subprocess.run([
                sys.executable, str(ROOT / "tools/data_link_diagnostics.py"),
                "--elf", str(root / "main.elf"), "--report", str(root / "report.json"),
                "--symbols", str(root / "symbols.txt"), "--output", str(root / "result.json"),
            ], capture_output=True, text=True)
            self.assertEqual(process.returncode, 0, process.stderr)
            self.assertEqual(json.loads((root / "result.json").read_text()), wanted)


if __name__ == "__main__":
    unittest.main()
