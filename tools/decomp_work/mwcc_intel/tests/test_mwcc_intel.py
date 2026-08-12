from __future__ import annotations

import json
import os
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from tools.decomp_work.mwcc_intel import candidates
from tools.decomp_work.mwcc_intel.analysis import analyze_capture
from tools.decomp_work.mwcc_intel.common import IntelError, SUPPORTED_COMPILERS, require_within
from tools.decomp_work.mwcc_intel.compare import compare_captures
from tools.decomp_work.mwcc_intel.discovery import compiler_record, discover_mwcc_root
from tools.decomp_work.mwcc_intel.manifest import transition
from tools.decomp_work.mwcc_intel.ninja import CompileUnit, parse_compile_units
from tools.decomp_work.mwcc_intel.sandbox import prepare_capture, sandbox_argv


STOCK_HASH = next(key for key, value in SUPPORTED_COMPILERS.items() if value["target"] == "stock")
NINJI_HASH = next(key for key, value in SUPPORTED_COMPILERS.items() if value["target"] == "ninji")


def unit(**updates) -> CompileUnit:
    values = {
        "output": "build/GC6E01/src/game/test.o",
        "rule": "mwcc",
        "source": "src/game/test.c",
        "mw_version": "GC/1.2.5n",
        "cflags_raw": '-O4,p -DVALUE=1 -DVALUE=2 -pragma "cats off"',
        "cflags": ["-O4,p", "-DVALUE=1", "-DVALUE=2", "-pragma", "cats off"],
        "basedir": "build/GC6E01/src/game",
        "basefile": "build/GC6E01/src/game/test",
        "working_directory": "C:/repo",
        "compiler": "build/compilers/GC/1.2.5n/mwcceppc.exe",
        "wrapper": None,
        "target_object": "build/GC6E01/obj/game/test.o",
        "candidate_object": "build/GC6E01/src/game/test.o",
    }
    values.update(updates)
    return CompileUnit(**values)


class DiscoveryTests(unittest.TestCase):
    def test_root_discovery_precedence(self):
        with tempfile.TemporaryDirectory() as temporary:
            base = Path(temporary)
            explicit = base / "explicit"
            env = base / "env"
            sibling = base / "mwcc-decomp"
            for root in (explicit, env, sibling):
                path = root / "tools" / "gdb"
                path.mkdir(parents=True)
                (path / "allocator_snapshot.py").write_text("", encoding="utf-8")
            with mock.patch.dict(os.environ, {"MWCC_DECOMP_ROOT": str(env)}):
                self.assertEqual(discover_mwcc_root(base / "repo", str(explicit)), explicit.resolve())

    def test_stock_and_ninji_mapping_is_hash_authoritative(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "mwcceppc.exe"
            path.write_bytes(b"synthetic")
            for digest, target in ((STOCK_HASH, "stock"), (NINJI_HASH, "ninji")):
                with mock.patch("tools.decomp_work.mwcc_intel.discovery.sha256_file", return_value=digest):
                    record = compiler_record(path, Path(temporary))
                self.assertTrue(record["supported"])
                self.assertEqual(record["capture_target"], target)

    def test_unsupported_hash_is_not_accepted(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "mwcceppc.exe"
            path.write_bytes(b"unsupported")
            record = compiler_record(path, Path(temporary))
            self.assertFalse(record["supported"])


class NinjaTests(unittest.TestCase):
    def test_exact_command_preserves_order_and_repeated_flags(self):
        text = """rule mwcc
  command = build/compilers/$mw_version/mwcceppc.exe $cflags -MMD -c $in -o $basedir
build build/GC6E01/src/game/test.o: mwcc src/game/test.c
  mw_version = GC/1.2.5n
  cflags = -O4,p -DVALUE=1 -DVALUE=2 -pragma \"cats off\" -O4,p
  basedir = build/GC6E01/src/game
  basefile = build/GC6E01/src/game/test
"""
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            path = root / "build.ninja"
            path.write_text(text, encoding="utf-8")
            parsed = parse_compile_units(path, root)
        record = parsed["build/GC6E01/src/game/test.o"]
        self.assertEqual(record.mw_version, "GC/1.2.5n")
        self.assertEqual(record.cflags.count("-O4,p"), 2)
        self.assertEqual(record.cflags[1:3], ["-DVALUE=1", "-DVALUE=2"])
        self.assertEqual(record.cflags[-2:], ["cats off", "-O4,p"])


class CandidateTests(unittest.TestCase):
    def _repo(self, root: Path) -> tuple[dict, dict[str, CompileUnit]]:
        for relative in (
            "src/game/test.c",
            "build/GC6E01/obj/game/test.o",
            "build/GC6E01/src/game/test.o",
            "build/compilers/GC/1.2.5n/mwcceppc.exe",
        ):
            path = root / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b"void candidate_fn(void) {}" if path.suffix == ".c" else b"fixture")
        report = {
            "units": [
                {
                    "name": "main/game/test",
                    "metadata": {"source_path": "src/game/test.c", "auto_generated": False},
                    "functions": [
                        {
                            "name": "candidate_fn",
                            "size": 128,
                            "fuzzy_match_percent": 97.5,
                            "metadata": {"virtual_address": 0x80001000},
                        },
                        {"name": "done", "size": 64, "fuzzy_match_percent": 100.0},
                    ],
                }
            ]
        }
        return report, {unit().output: unit(working_directory=str(root))}

    def test_filter_and_rank_supported_candidate(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            report, units = self._repo(root)
            with mock.patch("tools.decomp_work.mwcc_intel.candidates.sha256_file", return_value=NINJI_HASH):
                result = candidates.select_candidates(root, report, units, limit=10, enrich=False)
        self.assertEqual([item["symbol"] for item in result], ["candidate_fn"])
        self.assertEqual(result[0]["compiler_target"], "ninji")
        self.assertGreater(result[0]["selection_score"], 0)

    def test_conservative_mismatch_classification(self):
        payload = {
            "left": {
                "symbols": [
                    {
                        "name": "candidate_fn",
                        "size": 8,
                        "match_percent": 90,
                        "instructions": [
                            {
                                "diff_kind": "DIFF_ARG_MISMATCH",
                                "instruction": {
                                    "formatted": "mr r30, r31",
                                    "parts": [{"opcode": {"mnemonic": "mr"}}],
                                },
                            },
                            {"diff_kind": "DIFF_REPLACE", "instruction": {"formatted": "nop", "parts": []}},
                        ],
                    }
                ]
            },
            "right": {"symbols": [{"name": "candidate_fn", "size": 8, "instructions": [{}, {}]}]},
        }
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            binary = root / "build/tools/objdiff-cli.exe"
            binary.parent.mkdir(parents=True)
            binary.write_bytes(b"fixture")
            for relative in ("target.o", "candidate.o"):
                (root / relative).write_bytes(b"fixture")
            completed = mock.Mock(returncode=0, stdout=json.dumps(payload), stderr="")
            with mock.patch("tools.decomp_work.mwcc_intel.candidates.run", return_value=completed):
                result = candidates.classify_objdiff(
                    root,
                    {
                        "symbol": "candidate_fn",
                        "target_object": "target.o",
                        "candidate_object": "candidate.o",
                        "target_size": 8,
                        "match_percent": 90,
                    },
                )
        self.assertEqual(result["mismatch_summary"]["register"], 1)
        self.assertEqual(result["mismatch_summary"]["unknown"], 1)

    def test_symbol_validation_rejects_shell_text(self):
        with self.assertRaises(IntelError):
            candidates.safe_symbol("fn; whoami")


class SandboxTests(unittest.TestCase):
    def test_path_escape_is_rejected(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            with self.assertRaises(IntelError):
                require_within(root.parent / "elsewhere", root)

    def test_docker_argv_has_all_required_restrictions(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for relative in ("src", "include", "build/GC6E01/include", "external", "output"):
                (root / relative).mkdir(parents=True)
            for relative in ("compiler.exe", "wibo"):
                (root / relative).write_bytes(b"fixture")
            argv, compile_args = sandbox_argv(
                runtime="docker",
                image="debugger:test",
                platform="linux/amd64",
                repo=root,
                external=root / "external",
                compiler=root / "compiler.exe",
                wibo=root / "wibo",
                wrapper=None,
                output=root / "output",
                unit=unit(),
            )
        joined = " ".join(argv)
        for value in ("--pull never", "--network none", "--read-only", "--cap-drop ALL", "no-new-privileges", "--pids-limit", "--memory", "--cpus", "noexec,nosuid,nodev"):
            self.assertIn(value, joined)
        self.assertIn("/workspace/src/game/test.c", compile_args)

    def test_dry_run_is_stable_and_executes_nothing(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for relative in ("src/game", "include", "build/GC6E01/include", "build/compilers/GC/1.2.5n", "build/tools", "external/tools/gdb"):
                (root / relative).mkdir(parents=True, exist_ok=True)
            (root / "src/game/test.c").write_text("void candidate_fn(void) {}", encoding="utf-8")
            (root / "build/compilers/GC/1.2.5n/mwcceppc.exe").write_bytes(b"compiler")
            (root / "build/tools/wibo").write_bytes(b"wibo")
            (root / "external/tools/gdb/allocator_snapshot.py").write_text("", encoding="utf-8")
            with mock.patch("tools.decomp_work.mwcc_intel.sandbox.sha256_file", side_effect=lambda path: NINJI_HASH if path.name == "mwcceppc.exe" else ("1" * 64 if path.name == "wibo" else "2" * 64)), mock.patch("tools.decomp_work.mwcc_intel.sandbox.execute_capture") as execute:
                first = prepare_capture(root, unit(working_directory=str(root)), "candidate_fn", mwcc_root=str(root / "external"), compiler=None, wibo=None, image="debugger:test", platform="linux/amd64", runtime="docker")
                second = prepare_capture(root, unit(working_directory=str(root)), "candidate_fn", mwcc_root=str(root / "external"), compiler=None, wibo=None, image="debugger:test", platform="linux/amd64", runtime="docker")
            execute.assert_not_called()
            self.assertEqual(first, second)
            self.assertEqual(json.loads((first / "manifest.json").read_text())["state"], "planned")
            first.resolve().relative_to((root / "build").resolve())

    def test_manifest_state_transitions(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "manifest.json"
            transition(path, "planned")
            transition(path, "executed")
            transition(path, "capture_complete")
            transition(path, "analysis_complete")
            with self.assertRaises(IntelError):
                transition(path, "failed")


class AnalysisTests(unittest.TestCase):
    def test_missing_capture_artifacts_are_reported_without_blind_invocation(self):
        with tempfile.TemporaryDirectory() as temporary:
            repo = Path(temporary)
            external = repo / "external"
            (external / "tools/gdb").mkdir(parents=True)
            (external / "tools/gdb/allocator_snapshot.py").write_text("", encoding="utf-8")
            capture = repo / "build/mwcc_intel/fn/hash"
            (capture / "capture").mkdir(parents=True)
            transition(capture / "manifest.json", "planned", symbol="fn")
            transition(capture / "manifest.json", "executed")
            transition(capture / "manifest.json", "capture_complete")
            result = analyze_capture(capture, repo, mwcc_root=str(external))
            commands = json.loads((capture / "analysis/commands.json").read_text())
        self.assertEqual(result["diagnosis"]["first_divergent_stage"], "unknown")
        self.assertEqual(commands[0]["status"], "missing_inputs")

    def test_analysis_selects_only_applicable_tools_and_normalizes(self):
        with tempfile.TemporaryDirectory() as temporary:
            repo = Path(temporary)
            external = repo / "external"
            (external / "tools/gdb").mkdir(parents=True)
            (external / "tools/gdb/allocator_snapshot.py").write_text("", encoding="utf-8")
            for name in ("allocator_provenance.py", "rank_register_origins.py"):
                (external / "tools" / name).write_text("", encoding="utf-8")
            capture = repo / "build/mwcc_intel/fn/hash"
            artifact_dir = capture / "capture"
            artifact_dir.mkdir(parents=True)
            for name in ("allocator-0001.json", "pcode-creations-0001-allocator.json", "coloring-0001-gpr-01-before.json", "coloring-0001-gpr-01-after.json"):
                (artifact_dir / name).write_text("{}", encoding="utf-8")
            transition(capture / "manifest.json", "planned", symbol="fn", compiler={"sha256": NINJI_HASH})
            transition(capture / "manifest.json", "executed")
            transition(capture / "manifest.json", "capture_complete")

            def fake_invoke(script, args, *, cwd, output=None, timeout=120):
                if output:
                    output.parent.mkdir(parents=True, exist_ok=True)
                    output.write_text("{}", encoding="utf-8")
                return {"tool": script.name, "status": "ok", "output": str(output) if output else None}

            with mock.patch("tools.decomp_work.mwcc_intel.analysis._invoke", side_effect=fake_invoke):
                result = analyze_capture(capture, repo, mwcc_root=str(external))
        self.assertIn("provenance", result["generated"])
        self.assertIn(result["diagnosis"]["first_divergent_stage"], ("register_birth", "unknown"))


class CompareTests(unittest.TestCase):
    def _capture(self, root: Path, name: str, digest: str = NINJI_HASH, flags=None) -> Path:
        path = root / name
        (path / "capture").mkdir(parents=True)
        manifest = {
            "compiler": {"sha256": digest},
            "unit": {"source": "src/game/test.c", "cflags": flags or ["-O4,p"]},
            "symbol": "candidate_fn",
        }
        (path / "manifest.json").write_text(json.dumps(manifest), encoding="utf-8")
        return path

    def test_compatibility_rejects_different_compiler_hashes(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            left = self._capture(root, "left")
            right = self._capture(root, "right", STOCK_HASH)
            with self.assertRaises(IntelError):
                compare_captures(left, right, root)

    def test_comparison_identifies_earliest_changed_stage(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            left = self._capture(root, "left")
            right = self._capture(root, "right")
            (left / "capture/pcode-0001-initial.json").write_text("{}", encoding="utf-8")
            (right / "capture/pcode-0001-initial.json").write_text('{"changed":true}', encoding="utf-8")
            result = compare_captures(left, right, root)
        self.assertEqual(result["first_divergent_stage"], "frontend_or_lowering")


class DocumentationTests(unittest.TestCase):
    def test_documentation_has_no_personal_absolute_paths(self):
        readme = Path(__file__).parents[1] / "README.md"
        text = readme.read_text(encoding="utf-8")
        self.assertNotIn("C:\\Users\\", text)
        self.assertNotIn("/Users/", text)


if __name__ == "__main__":
    unittest.main()
