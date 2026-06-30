#!/usr/bin/env python3
"""
merge_results.py - Merge completed work packets back into the main source tree.

Takes a directory of completed work packet results, validates the C code
compiles, runs match tests, and merges new/updated source files into src/.

Usage:
    python tools/merge_results.py results/              # Merge all results
    python tools/merge_results.py results/ --dry-run    # Check without modifying
    python tools/merge_results.py results/ --validate   # Only compile-check
    python tools/merge_results.py results/ --report     # Show match report only

Expected results directory structure:
    results/
      WP-0001/
        packet.json           # Original work packet (for reference)
        src/                  # New/updated source files mirroring src/ layout
          game/some_module.c
          dolphin/os/SomeOS.c
        notes.txt             # Optional contributor notes
      WP-0002/
        ...
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
SRC_DIR = PROJECT_ROOT / "src"
TOOLS_DIR = PROJECT_ROOT / "tools"
BUILD_DIR = PROJECT_ROOT / "build" / "GC6E01"

sys.path.insert(0, str(TOOLS_DIR))


# ---------------------------------------------------------------------------
# Discovery
# ---------------------------------------------------------------------------

def discover_results(results_dir: Path) -> list:
    """Find completed work packet result directories.

    Each result directory should contain at least a src/ subdirectory with
    new or updated source files.

    Returns a list of dicts with keys:
        - packet_dir: Path to the result directory
        - packet_id: Extracted packet ID (e.g., "WP-0001") or directory name
        - source_files: List of (relative_path, absolute_path) tuples
        - packet_json: Parsed packet.json if present, else None
        - notes: Contents of notes.txt if present, else None
    """
    results = []

    if not results_dir.is_dir():
        print(f"ERROR: Results directory not found: {results_dir}")
        sys.exit(1)

    for entry in sorted(results_dir.iterdir()):
        if not entry.is_dir():
            continue

        src_subdir = entry / "src"
        if not src_subdir.is_dir():
            # Maybe the result directory directly contains .c files
            c_files = list(entry.rglob("*.c"))
            if not c_files:
                continue
            # Treat the directory itself as containing source
            src_subdir = entry

        # Collect source files
        source_files = []
        for src_file in src_subdir.rglob("*.c"):
            try:
                rel = src_file.relative_to(src_subdir)
            except ValueError:
                rel = Path(src_file.name)
            source_files.append((str(rel).replace("\\", "/"), src_file))

        # Also collect headers
        for hdr_file in src_subdir.rglob("*.h"):
            try:
                rel = hdr_file.relative_to(src_subdir)
            except ValueError:
                rel = Path(hdr_file.name)
            source_files.append((str(rel).replace("\\", "/"), hdr_file))

        if not source_files:
            continue

        # Load packet.json if present
        packet_json = None
        pj_path = entry / "packet.json"
        if pj_path.exists():
            try:
                with open(pj_path) as f:
                    packet_json = json.load(f)
            except (json.JSONDecodeError, OSError):
                pass

        # Load notes
        notes = None
        notes_path = entry / "notes.txt"
        if notes_path.exists():
            try:
                notes = notes_path.read_text(errors="replace").strip()
            except OSError:
                pass

        # Extract packet ID
        packet_id = entry.name
        if packet_json and "packet_id" in packet_json:
            packet_id = packet_json["packet_id"]

        results.append({
            "packet_dir": entry,
            "packet_id": packet_id,
            "source_files": source_files,
            "packet_json": packet_json,
            "notes": notes,
        })

    return results


# ---------------------------------------------------------------------------
# Validation
# ---------------------------------------------------------------------------

def validate_compilation(source_files: list, packet_id: str,
                         verbose: bool = False) -> dict:
    """Try to compile each source file and report results.

    Returns a dict with:
        - ok: list of successfully compiled files
        - fail: list of (file, error) tuples
    """
    from compile_check import compile_source, get_file_compiler_version

    ok = []
    fail = []

    for rel_path, abs_path in source_files:
        if not abs_path.suffix == ".c":
            continue  # Skip headers for compilation

        # We need to compile from a location relative to SRC_DIR.
        # Copy the file temporarily to the right place under src/
        target = SRC_DIR / rel_path
        target_existed = target.exists()
        original_content = None

        if target_existed:
            original_content = target.read_bytes()

        try:
            # Ensure parent dirs exist
            target.parent.mkdir(parents=True, exist_ok=True)
            # Copy the result file into position
            shutil.copy2(abs_path, target)

            # Compile
            try:
                compile_source(target, verbose=verbose)
                ok.append(rel_path)
            except SystemExit:
                fail.append((rel_path, "Compilation failed"))

        finally:
            # Restore original file if it existed, or remove if it was new
            if target_existed and original_content is not None:
                target.write_bytes(original_content)
            elif not target_existed and target.exists():
                target.unlink()

    return {"ok": ok, "fail": fail}


def run_match_tests(source_files: list, packet_id: str,
                    verbose: bool = False) -> dict:
    """Run match_test.py --scan on each source file.

    Returns match results per file.
    """
    results = {}

    for rel_path, abs_path in source_files:
        if not abs_path.suffix == ".c":
            continue

        target = SRC_DIR / rel_path
        if not target.exists():
            # File not yet merged, skip
            results[rel_path] = {"error": "Not yet in src/"}
            continue

        # Run match_test.py --scan
        cmd = [
            sys.executable,
            str(TOOLS_DIR / "match_test.py"),
            "--scan", str(target),
        ]

        result = subprocess.run(cmd, capture_output=True, text=True,
                                cwd=str(PROJECT_ROOT))

        # Parse the summary line
        output = result.stdout
        match_count = 0
        total_count = 0

        for line in output.split("\n"):
            if "Total:" in line and "functions matching" in line:
                import re
                m = re.search(r'(\d+)/(\d+)\s+functions matching', line)
                if m:
                    match_count = int(m.group(1))
                    total_count = int(m.group(2))

        results[rel_path] = {
            "matched": match_count,
            "total": total_count,
            "output": output if verbose else None,
        }

    return results


# ---------------------------------------------------------------------------
# Merging
# ---------------------------------------------------------------------------

def merge_files(source_files: list, dry_run: bool = False) -> dict:
    """Copy source files into the main src/ tree.

    Returns a dict with:
        - new: files that did not previously exist
        - updated: files that were overwritten
        - unchanged: files identical to existing
    """
    new = []
    updated = []
    unchanged = []

    for rel_path, abs_path in source_files:
        target = SRC_DIR / rel_path
        target.parent.mkdir(parents=True, exist_ok=True)

        new_content = abs_path.read_bytes()

        if target.exists():
            old_content = target.read_bytes()
            if old_content == new_content:
                unchanged.append(rel_path)
                continue
            else:
                if not dry_run:
                    # Back up the old file
                    backup = target.with_suffix(target.suffix + ".bak")
                    shutil.copy2(target, backup)
                    target.write_bytes(new_content)
                updated.append(rel_path)
        else:
            if not dry_run:
                target.write_bytes(new_content)
            new.append(rel_path)

    return {"new": new, "updated": updated, "unchanged": unchanged}


# ---------------------------------------------------------------------------
# Report
# ---------------------------------------------------------------------------

def print_report(results: list, merge_results: dict = None,
                 match_results: dict = None):
    """Print a summary report of the merge operation."""
    total_files = 0
    total_new = 0
    total_updated = 0
    total_matched = 0
    total_tested = 0

    for r in results:
        pid = r["packet_id"]
        sf = r["source_files"]
        c_files = [f for f in sf if f[0].endswith(".c")]
        total_files += len(c_files)

        print(f"\n{'='*60}")
        print(f"Packet: {pid}")
        print(f"  Source files: {len(c_files)} .c files")

        if r.get("notes"):
            print(f"  Notes: {r['notes'][:200]}")

        if r.get("compile_result"):
            cr = r["compile_result"]
            print(f"  Compilation: {len(cr['ok'])} OK, {len(cr['fail'])} FAIL")
            for path, err in cr["fail"]:
                print(f"    FAIL: {path} -- {err}")

        if r.get("merge_result"):
            mr = r["merge_result"]
            print(f"  Merged: {len(mr['new'])} new, {len(mr['updated'])} updated, "
                  f"{len(mr['unchanged'])} unchanged")
            total_new += len(mr["new"])
            total_updated += len(mr["updated"])

        if r.get("match_result"):
            mtch = r["match_result"]
            for path, info in mtch.items():
                if "error" not in info:
                    print(f"  Match: {path} -- "
                          f"{info['matched']}/{info['total']} functions")
                    total_matched += info["matched"]
                    total_tested += info["total"]

    print(f"\n{'='*60}")
    print(f"SUMMARY")
    print(f"  Packets processed: {len(results)}")
    print(f"  Total source files: {total_files}")
    print(f"  New files: {total_new}")
    print(f"  Updated files: {total_updated}")
    if total_tested > 0:
        print(f"  Functions matched: {total_matched}/{total_tested} "
              f"({100*total_matched/total_tested:.1f}%)")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Merge completed work packets back into the source tree.",
    )
    parser.add_argument(
        "results_dir",
        help="Directory containing completed work packet results",
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Validate and report without modifying the source tree",
    )
    parser.add_argument(
        "--validate", action="store_true",
        help="Only run compilation checks (do not merge)",
    )
    parser.add_argument(
        "--report", action="store_true",
        help="Only run match tests and report (do not merge)",
    )
    parser.add_argument(
        "--skip-compile", action="store_true",
        help="Skip compilation validation before merging",
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Show detailed output",
    )

    args = parser.parse_args()
    results_dir = Path(args.results_dir)
    if not results_dir.is_absolute():
        results_dir = PROJECT_ROOT / results_dir

    print(f"Scanning results directory: {results_dir}")
    results = discover_results(results_dir)

    if not results:
        print("No completed work packets found.")
        print(f"\nExpected structure:")
        print(f"  {results_dir}/")
        print(f"    WP-0001/")
        print(f"      src/")
        print(f"        game/some_module.c")
        print(f"      packet.json  (optional)")
        print(f"      notes.txt    (optional)")
        sys.exit(1)

    total_c_files = sum(
        len([f for f in r["source_files"] if f[0].endswith(".c")])
        for r in results
    )
    print(f"  Found {len(results)} result packets with {total_c_files} .c files")

    # Step 1: Validate compilation
    if not args.skip_compile and not args.report:
        print(f"\nValidating compilation ...")
        all_compile_ok = True
        for r in results:
            print(f"\n  [{r['packet_id']}] Compiling ...")
            cr = validate_compilation(r["source_files"], r["packet_id"],
                                      verbose=args.verbose)
            r["compile_result"] = cr
            if cr["fail"]:
                all_compile_ok = False
                for path, err in cr["fail"]:
                    print(f"    FAIL: {path}")

        if not all_compile_ok:
            print("\nSome files failed to compile.")
            if not args.dry_run and not args.validate:
                print("Fix compilation errors before merging, or use "
                      "--skip-compile to force.")
                sys.exit(1)

        if args.validate:
            print_report(results)
            return 0

    # Step 2: Merge files (or dry-run)
    if not args.report:
        print(f"\n{'[DRY RUN] ' if args.dry_run else ''}Merging files ...")
        for r in results:
            mr = merge_files(r["source_files"], dry_run=args.dry_run)
            r["merge_result"] = mr
            new_count = len(mr["new"])
            upd_count = len(mr["updated"])
            unch_count = len(mr["unchanged"])
            print(f"  [{r['packet_id']}] "
                  f"{new_count} new, {upd_count} updated, "
                  f"{unch_count} unchanged")

    # Step 3: Run match tests on merged files
    if not args.validate and not args.dry_run:
        print(f"\nRunning match tests ...")
        for r in results:
            print(f"  [{r['packet_id']}] Testing ...")
            mtch = run_match_tests(r["source_files"], r["packet_id"],
                                   verbose=args.verbose)
            r["match_result"] = mtch

    # Step 4: Report
    print_report(results)

    if args.dry_run:
        print("\n(Dry run -- no files were modified)")

    return 0


if __name__ == "__main__":
    sys.exit(main() or 0)
