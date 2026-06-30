#!/usr/bin/env python3
"""
Split a GameCube .dol executable into per-section raw binary files plus a
manifest describing each section's base address and size. Output is suitable
for feeding to Ghidra's BinaryLoader via analyzeHeadless.

Usage:
    python3 tools/ghidra/split_dol.py <path/to/main.dol> [out_dir]

DOL header layout (all big-endian):
    0x00-0x1b : u32[7]  text section file offsets
    0x1c-0x47 : u32[11] data section file offsets
    0x48-0x63 : u32[7]  text section load addresses
    0x64-0x8f : u32[11] data section load addresses
    0x90-0xab : u32[7]  text section sizes
    0xac-0xd7 : u32[11] data section sizes
    0xd8-0xdb : u32     bss load address
    0xdc-0xdf : u32     bss size
    0xe0-0xe3 : u32     entry point
"""
import json
import os
import struct
import sys


def split(dol_path: str, out_dir: str) -> dict:
    with open(dol_path, "rb") as f:
        dol = f.read()

    text_offs = struct.unpack(">7I", dol[0x00:0x1C])
    data_offs = struct.unpack(">11I", dol[0x1C:0x48])
    text_addrs = struct.unpack(">7I", dol[0x48:0x64])
    data_addrs = struct.unpack(">11I", dol[0x64:0x90])
    text_sizes = struct.unpack(">7I", dol[0x90:0xAC])
    data_sizes = struct.unpack(">11I", dol[0xAC:0xD8])
    bss_addr, bss_size, entry = struct.unpack(">3I", dol[0xD8:0xE4])

    os.makedirs(out_dir, exist_ok=True)
    manifest: list[dict] = []

    for i in range(7):
        if text_sizes[i] == 0:
            continue
        name = f"text{i}"
        out_path = os.path.join(out_dir, f"{name}.bin")
        with open(out_path, "wb") as f:
            f.write(dol[text_offs[i] : text_offs[i] + text_sizes[i]])
        manifest.append(
            {
                "name": name,
                "kind": "text",
                "base_addr": text_addrs[i],
                "size": text_sizes[i],
                "file": os.path.basename(out_path),
            }
        )

    for i in range(11):
        if data_sizes[i] == 0:
            continue
        name = f"data{i}"
        out_path = os.path.join(out_dir, f"{name}.bin")
        with open(out_path, "wb") as f:
            f.write(dol[data_offs[i] : data_offs[i] + data_sizes[i]])
        manifest.append(
            {
                "name": name,
                "kind": "data",
                "base_addr": data_addrs[i],
                "size": data_sizes[i],
                "file": os.path.basename(out_path),
            }
        )

    summary = {
        "dol_path": dol_path,
        "entry": entry,
        "bss_addr": bss_addr,
        "bss_size": bss_size,
        "sections": manifest,
    }
    with open(os.path.join(out_dir, "manifest.json"), "w") as f:
        json.dump(summary, f, indent=2)

    return summary


def main() -> int:
    if len(sys.argv) < 2:
        print(__doc__)
        return 1
    dol_path = sys.argv[1]
    out_dir = sys.argv[2] if len(sys.argv) > 2 else "tools/ghidra/dol_split"
    summary = split(dol_path, out_dir)
    print(f"Entry: 0x{summary['entry']:08x}")
    print(f"BSS:   0x{summary['bss_addr']:08x} size=0x{summary['bss_size']:x}")
    print(f"Wrote {len(summary['sections'])} sections to {out_dir}:")
    for s in summary["sections"]:
        print(f"  {s['name']:7s} 0x{s['base_addr']:08x} size=0x{s['size']:06x} -> {s['file']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
