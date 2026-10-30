#!/usr/bin/env python3
"""Adversarial checks with synthetic ELF/DOL inputs; no retail bytes required."""
import copy
import json
from pathlib import Path
import struct
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))
from tools import verify_interrupt_vector_source as verifier

EVIDENCE_TEXT = """
# Synthetic evidence fixture
Why it cannot be C: The template uses privileged exception state and fixed hardware slots.
Other decompilations: https://github.com/example/fixture/blob/0123456789abcdef0123456789abcdef01234567/template.s
Origin: This synthetic fixture represents a vendor hardware exception template.
External branch targets: fn_800053E0 is the reset destination and TRKInterruptHandler is the SRR0 destination.
"""
SYMBOL_TEXT = """
fn_800053E0 = .init:0x800053E0; // type:function size:0x2C
TRKInterruptHandler = .text:0x800C0EAC; // type:function size:0x194
"""


def dol_fixture(payload):
    data = bytearray(0x100)
    struct.pack_into(">I", data, 0, 0x100)
    struct.pack_into(">I", data, 0x48, verifier.BASE)
    struct.pack_into(">I", data, 0x90, len(payload))
    return bytes(data) + payload


def object_fixture():
    """Build a synthetic ELF with public labels, typed imports and 49 RELAs.

    Unexamined instruction positions contain sync, so the fixture is not a
    stored copy of the game template. Original equality uses a synthetic DOL.
    """
    native = bytearray(verifier.SIZE)
    banner = verifier.BANNER.encode("ascii") + b"\0"
    native[:len(banner)] = banner
    for start, end in verifier.CODE_REGIONS:
        for offset in range(start, end, 4):
            struct.pack_into(">I", native, offset, 0x7C0004AC)  # sync
    struct.pack_into(">I", native, 0x100, 0x48000000)
    struct.pack_into(">I", native, 0xF00, 0x48000054)
    for slot in verifier.TLB_SLOTS:
        struct.pack_into(">I", native, slot + 0x14, 0x4182001C)
    for offset, vector in verifier.REDIRECTS:
        struct.pack_into(">5I", native, offset,
                         0x3C600000, 0x60630000, 0x7C7A03A6, 0x38600000 | vector, 0x4C000064)
    relocated = bytearray(native)
    struct.pack_into(">I", relocated, 0x100, 0x48000000 | (0x800053E0 - (verifier.BASE + 0x100)))
    for offset, _ in verifier.REDIRECTS:
        struct.pack_into(">HH", relocated, offset + 2, 0x800C, 0x6063)
        struct.pack_into(">H", relocated, offset + 6, 0x0EAC)

    symbol_names = ["", "fixture.s (asm)", "", *verifier.LABELS,
                    "fn_800053E0", "TRKInterruptHandler"]
    strings = bytearray(b"\0")
    name_offsets = {"": 0}
    for name in symbol_names:
        if name not in name_offsets:
            name_offsets[name] = len(strings)
            strings.extend(name.encode("ascii") + b"\0")
    symbols = [(0, 0, 0, 0, 0, 0),
               (name_offsets[symbol_names[1]], 0, 0, 4, 0, 0xFFF1),
               (0, 0, 0, 3, 0, 1)]
    symbols += [(name_offsets[name], offset, 0, 0x10, 0, 1)
                for name, offset in verifier.LABELS.items()]
    symbols += [(name_offsets[name], 0, 0, 0x10, 0, 0) for name in verifier.TARGETS]
    symbol_indices = {name: index for index, name in enumerate(symbol_names) if name}
    relocations = [(0x100, 10, "fn_800053E0")]
    for offset, _ in verifier.REDIRECTS:
        relocations += [(offset + 2, 5, "TRKInterruptHandler"),
                        (offset + 6, 4, "TRKInterruptHandler")]
    rela = b"".join(struct.pack(">IIi", offset, (symbol_indices[name] << 8) | kind, 0)
                    for offset, kind, name in relocations)
    names = ["", ".init", ".rela.init", ".symtab", ".strtab", ".shstrtab"]
    shstrings = bytearray(b"\0")
    shname = {"": 0}
    for name in names[1:]:
        shname[name] = len(shstrings)
        shstrings.extend(name.encode("ascii") + b"\0")
    data = bytearray(52)
    sections = [(0,) * 10]
    section_offsets = {}
    records = [
        (".init", bytes(native), 1, 6, 0, 0, 4, 0),
        (".rela.init", rela, 4, 0, 3, 1, 4, 12),
        (".symtab", b"".join(struct.pack(">IIIBBH", *s) for s in symbols), 2, 0, 4, 3, 4, 16),
        (".strtab", bytes(strings), 3, 0, 0, 0, 1, 0),
        (".shstrtab", bytes(shstrings), 3, 0, 0, 0, 1, 0),
    ]
    for name, payload, kind, flags, link, info, alignment, stride in records:
        data.extend(bytes(-len(data) % alignment))
        offset = len(data)
        section_offsets[name] = offset
        data.extend(payload)
        sections.append((shname[name], kind, flags, 0, offset, len(payload), link, info, alignment, stride))
    data.extend(bytes(-len(data) % 4))
    headers = len(data)
    data.extend(b"".join(struct.pack(">10I", *s) for s in sections))
    ident = b"\x7fELF\x01\x02\x01" + bytes(9)
    struct.pack_into(">16sHHIIIIIHHHHHH", data, 0,
                     ident, 1, 20, 1, 0, 0, headers, 0x80000000, 52, 0, 0, 40, len(sections), 5)
    return bytes(data), dol_fixture(bytes(relocated)), {
        "sections": section_offsets, "headers": headers, "symbols": symbol_indices,
        "relocated": bytes(relocated)}


def report_fixture():
    vector = {"name": verifier.UNIT,
              "metadata": {"module_name": "main", "module_id": 0, "complete": True,
                           "source_path": verifier.SOURCE},
              "measures": {"total_units": 1, "complete_units": 1},
              "sections": [{"name": ".init", "size": verifier.SIZE, "fuzzy_match_percent": 100.0,
                            "metadata": {"virtual_address": verifier.BASE}}]}
    return {"measures": {"total_code": 2504940, "matched_code": 2504940, "complete_code": 2504940,
                         "total_functions": 8677, "matched_functions": 8677,
                         "total_data": 3565996, "matched_data": 3565996, "complete_data": 3565996,
                         "total_units": 2272, "complete_units": 2272},
            "units": [vector] + [{"name": f"synthetic/{i}",
                                   "metadata": {"complete": True, "source_path": "src/synthetic.c"}}
                                  for i in range(2271)]}


class InterruptVectorSourceTests(unittest.TestCase):
    def setUp(self):
        self.entry = verifier.load_registration(ROOT / verifier.REGISTRY)
        self.source = (ROOT / verifier.SOURCE).read_text(encoding="utf-8")
        self.addresses = verifier.symbol_addresses(SYMBOL_TEXT)

    def test_registered_source_has_only_its_declared_assembly_language(self):
        result = verifier.check_source(self.source, self.entry, EVIDENCE_TEXT)
        self.assertFalse(result["source_uses_binary_inputs"])
        self.assertEqual(result["registered_macros"], 4)

    def test_raw_storage_includes_and_extra_sections_fail(self):
        for directive in ('.include "data.inc"', '.incbin "original.bin"', '.long 0x4c000064',
                          '.word 0x6000', '.byte 0x48', '.4byte 0x48000000',
                          '.fill 4,4,0x60000000', '.ascii "payload"',
                          '.asciz "different banner"', '.section .data,"aw",@progbits'):
            with self.subTest(directive=directive), self.assertRaises(ValueError):
                verifier.check_source(self.source + "\n" + directive, self.entry, EVIDENCE_TEXT)

    def test_macro_body_cannot_hide_includes_or_other_external_branch(self):
        for changed in (self.source.replace("    rfi", '    .include "body.inc"\n    rfi', 1),
                        self.source.replace("b fn_800053E0", "b TRKInterruptHandler"),
                        self.source.replace(".Lrestore_cr_\\@", "WrongLabel", 1),
                        self.source.replace("\\offset, 0", "\\offset, 0xff"),
                        self.source.replace("    rfi", "    bl HiddenFunction\n    rfi", 1)):
            with self.subTest(source=changed[-30:]), self.assertRaises(ValueError):
                verifier.check_source(changed, self.entry, EVIDENCE_TEXT)

    def test_line_comments_cannot_disguise_live_binary_includes_as_block_comments(self):
        for addition in ('# /*\n.incbin "synthetic.bin"\n# */',
                         '# /*\n.long 0x4c000064\n# */',
                         '/* an actual block comment */\n.incbin "synthetic.bin"',
                         '/* unterminated', '*/ stray'):
            with self.subTest(addition=addition), self.assertRaises(ValueError):
                verifier.check_source(self.source + "\n" + addition, self.entry, EVIDENCE_TEXT)
        verifier.check_source(self.source + '\n# /* ignored line-comment text\n', self.entry, EVIDENCE_TEXT)
        verifier.check_source(self.source + '\n/* .incbin "inactive.bin" */\n', self.entry, EVIDENCE_TEXT)

    def test_evidence_is_required_and_commit_pinned(self):
        for evidence in ("", EVIDENCE_TEXT.replace("Origin:", "Unrelated:"),
                         EVIDENCE_TEXT.replace("0123456789abcdef0123456789abcdef01234567", "master"),
                         EVIDENCE_TEXT.replace("TRKInterruptHandler", "AnotherHandler")):
            with self.subTest(evidence=evidence[:20]), self.assertRaises(ValueError):
                verifier.check_source(self.source, self.entry, evidence)

    def test_registry_cannot_expand_scope_or_claim_function_credit(self):
        document = json.loads((ROOT / verifier.REGISTRY).read_text())
        for key, value in (("size", 7992), ("function_credit", 1), ("code_credit", 7988),
                           ("mnemonics", sorted(verifier.MNEMONICS | {"bl"}))):
            with self.subTest(key=key), tempfile.TemporaryDirectory() as directory:
                altered = copy.deepcopy(document)
                altered["templates"][0][key] = value
                path = Path(directory) / "templates.json"
                path.write_text(json.dumps(altered))
                with self.assertRaises(ValueError):
                    verifier.load_registration(path)

    def test_synthetic_native_relocations_control_transfers_and_dol_agree(self):
        blob, dol, fixture = object_fixture()
        payload, result = verifier.validate_object(blob, self.addresses)
        self.assertEqual(payload, fixture["relocated"])
        self.assertEqual(result["relocations"], {"R_PPC_REL24": 1, "R_PPC_ADDR16_HI": 24,
                                                "R_PPC_ADDR16_LO": 24})
        self.assertEqual(verifier.check_control_transfers(payload, self.addresses),
                         {"instruction_bytes": 1460, "instructions": 365, "signature_bytes": 46,
                          "zero_fill_bytes": 6482, "direct_branches": 5, "rfi_redirects": 24})
        self.assertTrue(verifier.compare_original(payload, dol)["canonical_interval_equal"])

    def test_wrong_relocation_target_type_addend_or_site_fails(self):
        blob, _, fixture = object_fixture()
        offset = fixture["sections"][".rela.init"]
        bad_target = (fixture["symbols"]["TRKInterruptHandler"] << 8) | 10
        for record, values in ((0, (0x100, bad_target, 0)),
                               (1, (0x23A, (fixture["symbols"]["TRKInterruptHandler"] << 8) | 6, 0)),
                               (0, (0x100, (fixture["symbols"]["fn_800053E0"] << 8) | 10, 4)),
                               (0, (0x104, (fixture["symbols"]["fn_800053E0"] << 8) | 10, 0))):
            with self.subTest(record=record, values=values):
                altered = bytearray(blob)
                struct.pack_into(">IIi", altered, offset + 12 * record, *values)
                with self.assertRaises(ValueError):
                    verifier.validate_object(bytes(altered), self.addresses)

    def test_missing_duplicate_or_implicit_relocations_fail(self):
        blob, _, fixture = object_fixture()
        headers, offset = fixture["headers"], fixture["sections"][".rela.init"]
        mutations = []
        absent = bytearray(blob); struct.pack_into(">I", absent, headers + 2 * 40 + 20, 0)
        mutations.append(absent)
        duplicate = bytearray(blob); duplicate[offset + 12:offset + 24] = duplicate[offset:offset + 12]
        mutations.append(duplicate)
        implicit = bytearray(blob); struct.pack_into(">I", implicit, headers + 2 * 40 + 4, 9)
        mutations.append(implicit)
        for altered in mutations:
            with self.subTest(), self.assertRaises(ValueError):
                verifier.validate_object(bytes(altered), self.addresses)

    def test_symbol_bounds_alignment_and_preencoded_operands_fail(self):
        blob, _, fixture = object_fixture()
        cases = []
        label = bytearray(blob)
        sym = fixture["sections"][".symtab"] + fixture["symbols"]["gTRKInterruptVectorTableEnd"] * 16
        struct.pack_into(">I", label, sym + 4, verifier.SIZE + 4)
        cases.append(label)
        alignment = bytearray(blob); struct.pack_into(">I", alignment, fixture["headers"] + 40 + 32, 16)
        cases.append(alignment)
        encoded = bytearray(blob); struct.pack_into(">H", encoded, fixture["sections"][".init"] + 0x23A, 0x800C)
        cases.append(encoded)
        linked = bytearray(blob); struct.pack_into(">I", linked, fixture["sections"][".init"] + 0x100, 0x48000001)
        cases.append(linked)
        for altered in cases:
            with self.subTest(), self.assertRaises(ValueError):
                verifier.validate_object(bytes(altered), self.addresses)

    def test_even_local_function_symbols_are_rejected(self):
        blob, _, fixture = object_fixture()
        altered = bytearray(blob)
        file_symbol = fixture["sections"][".symtab"] + 16
        name = struct.unpack_from(">I", altered, file_symbol)[0]
        struct.pack_into(">IIIBBH", altered, file_symbol, name, 0, 4, 2, 0, 1)
        with self.assertRaisesRegex(ValueError, "function symbols"):
            verifier.validate_object(bytes(altered), self.addresses)

    def test_control_flow_checks_do_not_trust_equal_reference_bytes(self):
        _, _, fixture = object_fixture()
        for offset, word in ((0x104, 0x60000000),  # Reserved slot became code.
                             (0xF00, 0x48000050),  # Wrong shared-vector destination.
                             (0x1014, 0x4082001C),  # Wrong TLB branch condition.
                             (0x244, 0x38600300),  # Wrong machine-check exception ID.
                             (0x300, 0x4E800420),  # Unexpected CTR transfer.
                             (0x330, 0x7C0004AC)):  # Missing rfi.
            altered = bytearray(fixture["relocated"])
            struct.pack_into(">I", altered, offset, word)
            # Even a reference containing the same changed bytes cannot bless it.
            self.assertTrue(verifier.compare_original(bytes(altered), dol_fixture(bytes(altered))))
            with self.subTest(offset=offset), self.assertRaises(ValueError):
                verifier.check_control_transfers(bytes(altered), self.addresses)

    def test_complete_byte_comparison_catches_changes_outside_relocated_operands(self):
        _, dol, fixture = object_fixture()
        altered = bytearray(fixture["relocated"])
        altered[0x210] ^= 1
        with self.assertRaisesRegex(ValueError, "differs"):
            verifier.compare_original(bytes(altered), dol)

    def test_dol_bounds_and_overlapping_sections_fail(self):
        _, dol, _ = object_fixture()
        overlap = bytearray(dol)
        struct.pack_into(">I", overlap, 4, 0x100)
        struct.pack_into(">I", overlap, 0x4C, verifier.BASE)
        struct.pack_into(">I", overlap, 0x94, verifier.SIZE)
        for bad in (dol[:0x80], dol[:-1], bytes(overlap)):
            with self.subTest(), self.assertRaises(ValueError):
                verifier.original_interval(bad)

    def test_public_function_symbol_identity_cannot_drift(self):
        for text in (SYMBOL_TEXT.replace("800C0EAC", "800C0EB0"),
                     SYMBOL_TEXT.replace("type:function", "type:label"),
                     SYMBOL_TEXT + SYMBOL_TEXT, ""):
            with self.subTest(text=text[:30]), self.assertRaises(ValueError):
                verifier.symbol_addresses(text)

    def test_complete_report_retains_zero_function_code_and_data_credit(self):
        result = verifier.check_report(report_fixture())
        self.assertEqual(result["completed_units"], 2272)
        self.assertEqual(result["additional_function_credit"], 0)
        self.assertEqual(result["additional_code_credit"], 0)
        self.assertEqual(result["additional_data_credit"], 0)

    def test_coherent_repartition_preserves_vector_completion(self):
        report = report_fixture()
        report["units"].pop()
        report["measures"]["total_units"] -= 1
        report["measures"]["complete_units"] -= 1
        result = verifier.check_report(report)
        self.assertEqual(result["completed_units"], 2271)
        self.assertEqual(result["additional_code_credit"], 0)

    def test_repartition_cannot_hide_lost_inventory_or_changed_denominators(self):
        for mutation in ("inventory", "code", "data", "functions", "empty"):
            report = report_fixture()
            if mutation == "inventory":
                report["units"].pop()
            elif mutation == "empty":
                report["units"] = []
                report["measures"]["total_units"] = 0
                report["measures"]["complete_units"] = 0
            else:
                report["measures"]["total_" + mutation] -= 1
            with self.subTest(mutation=mutation), self.assertRaises(ValueError):
                verifier.check_report(report)

    def test_incomplete_or_misattributed_report_fails(self):
        for mutation in ("global_incomplete", "unit_incomplete", "source", "extent", "credit", "missing",
                         "other_incomplete", "duplicate", "unit_count", "section_mismatch"):
            report = report_fixture()
            unit = report["units"][0]
            if mutation == "global_incomplete": report["measures"]["complete_units"] = 2271
            elif mutation == "unit_incomplete": unit["metadata"]["complete"] = False
            elif mutation == "source": unit["metadata"]["source_path"] = "original/vector.o"
            elif mutation == "extent": unit["sections"][0]["size"] += 4
            elif mutation == "credit": unit["measures"]["total_code"] = verifier.SIZE
            elif mutation == "missing": report["units"].pop(0)
            elif mutation == "other_incomplete": report["units"][1]["metadata"]["complete"] = False
            elif mutation == "duplicate": report["units"][1]["name"] = verifier.UNIT
            elif mutation == "unit_count": unit["measures"]["complete_units"] = 0
            else: unit["sections"][0]["fuzzy_match_percent"] = 99.0
            with self.subTest(mutation=mutation), self.assertRaises(ValueError):
                verifier.check_report(report)


if __name__ == "__main__":
    unittest.main()
