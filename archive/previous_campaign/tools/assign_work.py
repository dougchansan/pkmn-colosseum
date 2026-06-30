#!/usr/bin/env python3
"""
assign_work.py - Divide unmatched functions into work packets for parallel matching.

Reads the symbol table and current matching status to produce JSON work packets.
Each packet contains a batch of ~50-100 unmatched functions from a contiguous
address range, along with metadata about the subsystem and difficulty.

Usage:
    python tools/assign_work.py                         # Create packets in work_packets/
    python tools/assign_work.py --output-dir my_work    # Custom output directory
    python tools/assign_work.py --packet-size 50        # Smaller packets
    python tools/assign_work.py --list                  # Show packet summary only
    python tools/assign_work.py --subsystem game        # Only game-code packets
    python tools/assign_work.py --status                # Show overall status
"""

import argparse
import json
import os
import re
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
CONFIG_DIR = PROJECT_ROOT / "config" / "GC6E01"
SRC_DIR = PROJECT_ROOT / "src"
SYMBOLS_TXT = CONFIG_DIR / "symbols.txt"
DEFAULT_OUTPUT_DIR = PROJECT_ROOT / "work_packets"

# ---------------------------------------------------------------------------
# Symbol parsing (duplicated from match_test.py to keep this self-contained)
# ---------------------------------------------------------------------------

SYMBOL_RE = re.compile(
    r'^(\S+)\s*=\s*(\.\w+):0x([0-9A-Fa-f]+)\s*;'
    r'(?:\s*//\s*(.*))?$'
)


class Symbol:
    def __init__(self, name, section, address, sym_type=None, size=None,
                 scope=None):
        self.name = name
        self.section = section
        self.address = address
        self.type = sym_type
        self.size = size
        self.scope = scope

    @property
    def is_function(self):
        return self.type == "function"

    @property
    def end_address(self):
        return self.address + self.size if self.size else self.address


def parse_symbols() -> list:
    """Parse symbols.txt into a list of Symbol objects."""
    if not SYMBOLS_TXT.exists():
        print(f"ERROR: Symbol file not found: {SYMBOLS_TXT}")
        sys.exit(1)

    symbols = []
    with open(SYMBOLS_TXT, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("//") or line.startswith("#"):
                continue
            m = SYMBOL_RE.match(line)
            if not m:
                continue

            name = m.group(1)
            section = m.group(2)
            address = int(m.group(3), 16)
            comment = m.group(4) or ""

            sym_type = None
            size = None
            scope = None
            for part in comment.split():
                if part.startswith("type:"):
                    sym_type = part.split(":", 1)[1]
                elif part.startswith("size:"):
                    try:
                        size = int(part.split(":", 1)[1], 0)
                    except ValueError:
                        pass
                elif part.startswith("scope:"):
                    scope = part.split(":", 1)[1]

            symbols.append(Symbol(name, section, address, sym_type, size, scope))

    return symbols


# ---------------------------------------------------------------------------
# Subsystem classification by address range
# ---------------------------------------------------------------------------

SUBSYSTEM_RANGES = [
    (0x80003100, 0x80005600, "init",     "CRT / boot / init"),
    (0x800055E0, 0x80006500, "game",     "Game main / init"),
    (0x80006500, 0x80009700, "game",     "Game logic / flags / scene"),
    (0x80009700, 0x8000D300, "game",     "Battle system"),
    (0x8000D300, 0x80080000, "game",     "World / scene / scripting"),
    (0x80080000, 0x80098000, "game",     "PAD / controller / input"),
    (0x80098000, 0x800A3000, "dolphin",  "Dolphin SDK: OS / PPC / EXI"),
    (0x800A3000, 0x800B0000, "dolphin",  "Dolphin SDK: DVD / Reset / SRAM"),
    (0x800B0000, 0x800C4000, "dolphin",  "Dolphin SDK: VI / AI / AR / GX"),
    (0x800C4000, 0x800D2000, "crt",      "C runtime / libc"),
    (0x800D2000, 0x80120000, "game",     "GS engine (gfx/mem/thread/model)"),
    (0x80120000, 0x80136000, "game",     "Sound / RNG / model loader"),
    (0x80136000, 0x80168000, "game",     "UI / Menu system"),
    (0x80168000, 0x80192000, "game",     "Floor / scene / world / Pokemon"),
    (0x80192000, 0x801C4000, "game",     "HSD (sysdolphin rendering)"),
    (0x801C4000, 0x801F0000, "game",     "Script engine / save / GBA"),
    (0x801F0000, 0x80250000, "game",     "Game systems (misc)"),
    (0x80250000, 0x80270000, "game",     "REL module loader / runtime"),
]


def classify_subsystem(address: int) -> tuple:
    """Return (subsystem_key, description) for a given address."""
    for start, end, key, desc in SUBSYSTEM_RANGES:
        if start <= address < end:
            return key, desc
    return "unknown", "Unclassified"


# ---------------------------------------------------------------------------
# Determine which functions are already matched / have source
# ---------------------------------------------------------------------------

def find_covered_functions() -> set:
    """Return a set of function addresses that already have source coverage.

    Scans all .c files under src/ for address references in comments.
    """
    covered = set()

    for src_file in sorted(SRC_DIR.rglob("*.c")):
        try:
            with open(src_file, "r", errors="replace") as f:
                content = f.read()
        except OSError:
            continue

        # Match "Address: 0xNNNNNNNN" or "0x800XXXXX" patterns in comments
        for m in re.finditer(r'0x(8[0-9A-Fa-f]{7})', content):
            covered.add(int(m.group(1), 16))

    return covered


def get_functions_needing_work(symbols: list,
                               covered_addrs: set) -> list:
    """Return function symbols that are not yet covered by source files."""
    unmatched = []
    for sym in symbols:
        if not sym.is_function:
            continue
        if sym.size is None or sym.size == 0:
            continue
        if sym.address in covered_addrs:
            continue
        unmatched.append(sym)
    return unmatched


# ---------------------------------------------------------------------------
# Packet generation
# ---------------------------------------------------------------------------

def create_packets(functions: list, packet_size: int = 75,
                   subsystem_filter: str = None) -> list:
    """Divide functions into work packets.

    Groups by contiguous address regions and subsystem, then splits into
    chunks of approximately `packet_size` functions.
    """
    # Sort by address
    functions = sorted(functions, key=lambda s: s.address)

    if subsystem_filter:
        functions = [f for f in functions
                     if classify_subsystem(f.address)[0] == subsystem_filter]

    if not functions:
        return []

    # Group into contiguous regions (gap > 0x1000 bytes = new group)
    groups = []
    current_group = [functions[0]]
    for fn in functions[1:]:
        prev_end = current_group[-1].end_address
        if fn.address - prev_end > 0x1000:
            groups.append(current_group)
            current_group = [fn]
        else:
            current_group.append(fn)
    groups.append(current_group)

    # Split groups into packets of `packet_size`
    packets = []
    packet_id = 1
    for group in groups:
        for i in range(0, len(group), packet_size):
            chunk = group[i:i + packet_size]
            subsys_key, subsys_desc = classify_subsystem(chunk[0].address)

            total_bytes = sum(fn.size for fn in chunk if fn.size)
            addr_start = chunk[0].address
            addr_end = chunk[-1].end_address

            packet = {
                "packet_id": f"WP-{packet_id:04d}",
                "subsystem": subsys_key,
                "subsystem_description": subsys_desc,
                "address_range": {
                    "start": f"0x{addr_start:08X}",
                    "end": f"0x{addr_end:08X}",
                },
                "function_count": len(chunk),
                "total_bytes": total_bytes,
                "estimated_instructions": total_bytes // 4,
                "difficulty": _estimate_difficulty(chunk),
                "functions": [],
            }

            for fn in chunk:
                packet["functions"].append({
                    "name": fn.name,
                    "address": f"0x{fn.address:08X}",
                    "size": fn.size,
                    "size_hex": f"0x{fn.size:X}" if fn.size else None,
                    "section": fn.section,
                    "scope": fn.scope,
                })

            packets.append(packet)
            packet_id += 1

    return packets


def _estimate_difficulty(functions: list) -> str:
    """Rough difficulty estimate based on function sizes and subsystem."""
    avg_size = sum(fn.size for fn in functions if fn.size) / max(len(functions), 1)
    subsys = classify_subsystem(functions[0].address)[0]

    if subsys == "crt":
        return "hard"  # CRT functions are notoriously hard to match
    if subsys == "dolphin":
        return "medium"  # SDK code has known patterns but SDA issues
    if avg_size > 0x400:
        return "hard"  # Large functions are complex
    if avg_size > 0x100:
        return "medium"
    return "easy"


def save_packets(packets: list, output_dir: Path):
    """Write each packet as a JSON file and create a manifest."""
    output_dir.mkdir(parents=True, exist_ok=True)

    manifest = {
        "project": "Pokemon Colosseum (GC6E01)",
        "total_packets": len(packets),
        "total_functions": sum(p["function_count"] for p in packets),
        "total_bytes": sum(p["total_bytes"] for p in packets),
        "packets": [],
    }

    for packet in packets:
        pid = packet["packet_id"]
        filename = f"{pid}.json"
        filepath = output_dir / filename

        with open(filepath, "w") as f:
            json.dump(packet, f, indent=2)

        manifest["packets"].append({
            "packet_id": pid,
            "file": filename,
            "subsystem": packet["subsystem"],
            "function_count": packet["function_count"],
            "address_range": packet["address_range"],
            "difficulty": packet["difficulty"],
            "status": "available",
            "assigned_to": None,
        })

    manifest_path = output_dir / "manifest.json"
    with open(manifest_path, "w") as f:
        json.dump(manifest, f, indent=2)

    return manifest


def print_status(symbols: list, covered_addrs: set):
    """Print overall matching status."""
    all_fns = [s for s in symbols if s.is_function and s.size]
    covered_fns = [s for s in all_fns if s.address in covered_addrs]
    uncovered_fns = [s for s in all_fns if s.address not in covered_addrs]

    total_bytes = sum(s.size for s in all_fns)
    covered_bytes = sum(s.size for s in covered_fns)
    uncovered_bytes = sum(s.size for s in uncovered_fns)

    print(f"Overall Matching Status")
    print(f"{'='*60}")
    print(f"  Total functions:    {len(all_fns):>6}")
    print(f"  With source:        {len(covered_fns):>6} "
          f"({100*len(covered_fns)/len(all_fns):.1f}%)")
    print(f"  Needing work:       {len(uncovered_fns):>6} "
          f"({100*len(uncovered_fns)/len(all_fns):.1f}%)")
    print()
    print(f"  Total code bytes:   {total_bytes:>10} ({total_bytes/1024:.1f} KB)")
    print(f"  Covered bytes:      {covered_bytes:>10} ({covered_bytes/1024:.1f} KB)")
    print(f"  Remaining bytes:    {uncovered_bytes:>10} "
          f"({uncovered_bytes/1024:.1f} KB)")
    print()

    # Per-subsystem breakdown
    print(f"  {'Subsystem':<35} {'Functions':>10} {'Bytes':>10}")
    print(f"  {'-'*35} {'-'*10} {'-'*10}")
    by_subsys = {}
    for fn in uncovered_fns:
        key, desc = classify_subsystem(fn.address)
        if key not in by_subsys:
            by_subsys[key] = {"desc": desc, "count": 0, "bytes": 0}
        by_subsys[key]["count"] += 1
        by_subsys[key]["bytes"] += fn.size

    for key in sorted(by_subsys.keys()):
        info = by_subsys[key]
        print(f"  {info['desc']:<35} {info['count']:>10} "
              f"{info['bytes']:>10}")


def main():
    parser = argparse.ArgumentParser(
        description="Divide unmatched functions into work packets for "
                    "parallel matching.",
    )
    parser.add_argument(
        "--output-dir", "-o", default=str(DEFAULT_OUTPUT_DIR),
        help=f"Output directory for work packets (default: work_packets/)",
    )
    parser.add_argument(
        "--packet-size", "-n", type=int, default=75,
        help="Target number of functions per packet (default: 75)",
    )
    parser.add_argument(
        "--subsystem", "-s",
        choices=["game", "dolphin", "crt", "init", "unknown"],
        help="Only generate packets for a specific subsystem",
    )
    parser.add_argument(
        "--list", action="store_true",
        help="Show packet summary without writing files",
    )
    parser.add_argument(
        "--status", action="store_true",
        help="Show overall matching status",
    )

    args = parser.parse_args()

    print("Parsing symbols ...")
    symbols = parse_symbols()
    print(f"  {len(symbols)} symbols total, "
          f"{sum(1 for s in symbols if s.is_function)} functions")

    print("Scanning source files for coverage ...")
    covered = find_covered_functions()
    print(f"  {len(covered)} function addresses covered by source files")

    if args.status:
        print()
        print_status(symbols, covered)
        return 0

    uncovered = get_functions_needing_work(symbols, covered)
    print(f"  {len(uncovered)} functions needing work")

    if not uncovered:
        print("\nAll functions are covered! Nothing to assign.")
        return 0

    print(f"\nGenerating work packets (target size: {args.packet_size}) ...")
    packets = create_packets(uncovered, packet_size=args.packet_size,
                             subsystem_filter=args.subsystem)

    if not packets:
        print("No packets generated (check subsystem filter).")
        return 0

    print(f"  {len(packets)} packets generated")

    if args.list:
        print(f"\n{'ID':<12} {'Subsystem':<12} {'Functions':>10} "
              f"{'Bytes':>10} {'Range':<30} {'Difficulty':<10}")
        print(f"{'-'*12} {'-'*12} {'-'*10} {'-'*10} {'-'*30} {'-'*10}")
        for p in packets:
            rng = (f"{p['address_range']['start']}-"
                   f"{p['address_range']['end']}")
            print(f"{p['packet_id']:<12} {p['subsystem']:<12} "
                  f"{p['function_count']:>10} {p['total_bytes']:>10} "
                  f"{rng:<30} {p['difficulty']:<10}")
        return 0

    output_dir = Path(args.output_dir)
    manifest = save_packets(packets, output_dir)

    print(f"\nWork packets written to: {output_dir}")
    print(f"  Manifest: {output_dir / 'manifest.json'}")
    print(f"  Packets:  {manifest['total_packets']}")
    print(f"  Functions: {manifest['total_functions']}")
    print(f"  Total code: {manifest['total_bytes'] / 1024:.1f} KB")
    print()
    print("Next steps:")
    print("  1. Assign packets to contributors (update manifest.json)")
    print("  2. Contributors pick up WP-XXXX.json and match the functions")
    print("  3. Submit results via tools/merge_results.py")

    return 0


if __name__ == "__main__":
    sys.exit(main() or 0)
