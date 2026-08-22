import unittest

from patch_forceactive_ldscript import parse_symbols, patch_forceactive_block


class PatchForceactiveLdscriptTests(unittest.TestCase):
    def test_parse_symbols_deduplicates_and_drops_empty_entries(self) -> None:
        self.assertEqual(
            parse_symbols("foo,, bar,foo ,baz"),
            ["foo", "bar", "baz"],
        )

    def test_patch_forceactive_block_replaces_empty_block(self) -> None:
        original = "FORCEACTIVE\n{\n    \n}\n"
        expected = "FORCEACTIVE\n{\n    foo\n    bar\n}\n"
        self.assertEqual(
            patch_forceactive_block(original, ["foo", "bar"]),
            expected,
        )

    def test_patch_forceactive_block_preserves_crlf(self) -> None:
        original = "MEMORY\r\n{\r\n}\r\n\r\nFORCEACTIVE\r\n{\r\n    \r\n}\r\n"
        patched = patch_forceactive_block(original, ["foo"])
        self.assertIn("FORCEACTIVE\r\n{\r\n    foo\r\n}\r\n", patched)

    def test_patch_forceactive_block_preserves_existing_entries(self) -> None:
        original = "FORCEACTIVE\n{\n    keep_me\n}\n"
        expected = "FORCEACTIVE\n{\n    keep_me\n    add_me\n}\n"
        self.assertEqual(
            patch_forceactive_block(original, ["add_me"]),
            expected,
        )


if __name__ == "__main__":
    unittest.main()
