import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from patch_forceactive_ldscript import parse_symbols, patch_forceactive_block

SCRIPT = Path(__file__).with_name("patch_forceactive_ldscript.py")


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

    def test_symbols_file_extends_command_line_symbols(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "in.lcf").write_text("FORCEACTIVE\n{\n}\n", encoding="utf-8")
            (root / "symbols.rsp").write_text("bar,foo", encoding="utf-8")
            subprocess.run([sys.executable, str(SCRIPT), str(root / "in.lcf"), str(root / "out.lcf"),
                            "--symbols", "foo", "--symbols-file", str(root / "symbols.rsp")],
                           check=True)
            self.assertEqual((root / "out.lcf").read_text(encoding="utf-8"),
                             "FORCEACTIVE\n{\n    foo\n    bar\n}\n")


if __name__ == "__main__":
    unittest.main()
