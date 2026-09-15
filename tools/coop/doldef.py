#!/usr/bin/env python3
"""GameCube DOL container parsing shared by the co-op patch tooling.

The DOL header is a fixed 0x100-byte table describing 7 text sections and 11
data sections. Everything here is read-only structure handling; policy such as
checksum gating lives in build_patch.py.
"""

from __future__ import annotations

import struct
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional

TEXT_SLOTS = 7
DATA_SLOTS = 11
HEADER_SIZE = 0x100

_OFF_TEXT_FILE = 0x00
_OFF_DATA_FILE = 0x1C
_OFF_TEXT_ADDR = 0x48
_OFF_DATA_ADDR = 0x64
_OFF_TEXT_SIZE = 0x90
_OFF_DATA_SIZE = 0xAC
_OFF_BSS_ADDR = 0xD8
_OFF_BSS_SIZE = 0xDC
_OFF_ENTRY = 0xE0


@dataclass
class Section:
    kind: str  # "text" or "data"
    index: int
    file_off: int
    address: int
    size: int

    @property
    def used(self) -> bool:
        return self.size != 0 and self.address != 0

    @property
    def end(self) -> int:
        return self.address + self.size

    def contains(self, addr: int, length: int = 1) -> bool:
        return self.address <= addr and addr + length <= self.end


class Dol:
    def __init__(self, data: bytes) -> None:
        if len(data) < HEADER_SIZE:
            raise ValueError("file is too small to be a DOL")
        self.data = bytearray(data)
        self.sections: List[Section] = []

        for i in range(TEXT_SLOTS):
            self.sections.append(
                Section(
                    "text",
                    i,
                    self._u32(_OFF_TEXT_FILE + i * 4),
                    self._u32(_OFF_TEXT_ADDR + i * 4),
                    self._u32(_OFF_TEXT_SIZE + i * 4),
                )
            )
        for i in range(DATA_SLOTS):
            self.sections.append(
                Section(
                    "data",
                    i,
                    self._u32(_OFF_DATA_FILE + i * 4),
                    self._u32(_OFF_DATA_ADDR + i * 4),
                    self._u32(_OFF_DATA_SIZE + i * 4),
                )
            )

        self.bss_addr = self._u32(_OFF_BSS_ADDR)
        self.bss_size = self._u32(_OFF_BSS_SIZE)
        self.entry = self._u32(_OFF_ENTRY)

    # -- header access -----------------------------------------------------

    def _u32(self, off: int) -> int:
        return struct.unpack_from(">I", self.data, off)[0]

    def _put_u32(self, off: int, value: int) -> None:
        struct.pack_into(">I", self.data, off, value)

    @classmethod
    def load(cls, path: Path) -> "Dol":
        return cls(path.read_bytes())

    def save(self, path: Path) -> None:
        path.write_bytes(bytes(self.data))

    # -- address mapping ---------------------------------------------------

    def find_section(self, addr: int, length: int = 1) -> Optional[Section]:
        for section in self.sections:
            if section.used and section.contains(addr, length):
                return section
        return None

    def to_file_offset(self, addr: int, length: int = 1) -> int:
        section = self.find_section(addr, length)
        if section is None:
            raise KeyError(f"address 0x{addr:08X} is not mapped by any DOL section")
        return section.file_off + (addr - section.address)

    def read(self, addr: int, length: int) -> bytes:
        off = self.to_file_offset(addr, length)
        return bytes(self.data[off : off + length])

    def write(self, addr: int, payload: bytes) -> None:
        off = self.to_file_offset(addr, len(payload))
        self.data[off : off + len(payload)] = payload

    # -- section allocation ------------------------------------------------

    def free_text_slot(self) -> Optional[int]:
        for section in self.sections:
            if section.kind == "text" and not section.used:
                return section.index
        return None

    def free_data_slot(self) -> Optional[int]:
        for section in self.sections:
            if section.kind == "data" and not section.used:
                return section.index
        return None

    def highest_mapped_end(self) -> int:
        """End of the highest loaded section, ignoring bss."""
        return max(s.end for s in self.sections if s.used)

    def add_text_section(
        self, address: int, payload: bytes, kind: str = "text"
    ) -> Section:
        """Append a new loaded section carrying `payload` at `address`.

        The payload is placed at the end of the file; DOL sections are not
        required to be ordered by address, only to be individually contiguous.

        `kind` selects a text or data slot. GameCube has no execute
        protection, so code runs from either; which one a given loader
        actually honours is an empirical question.
        """
        if kind == "data":
            return self._add_data_section(address, payload)
        slot = self.free_text_slot()
        if slot is None:
            raise RuntimeError("no free text section slot in this DOL")
        if address % 32:
            raise ValueError("section address must be 32-byte aligned")

        # DOL section data is conventionally 32-byte aligned within the file.
        pad = (-len(self.data)) % 32
        self.data.extend(b"\x00" * pad)
        file_off = len(self.data)
        self.data.extend(payload)
        tail = (-len(self.data)) % 32
        self.data.extend(b"\x00" * tail)

        size = len(payload) + tail
        self._put_u32(_OFF_TEXT_FILE + slot * 4, file_off)
        self._put_u32(_OFF_TEXT_ADDR + slot * 4, address)
        self._put_u32(_OFF_TEXT_SIZE + slot * 4, size)

        section = Section("text", slot, file_off, address, size)
        self.sections.append(section)
        return section

    def _add_data_section(self, address: int, payload: bytes) -> Section:
        slot = self.free_data_slot()
        if slot is None:
            raise RuntimeError("no free data section slot in this DOL")
        if address % 32:
            raise ValueError("section address must be 32-byte aligned")

        pad = (-len(self.data)) % 32
        self.data.extend(b"\x00" * pad)
        file_off = len(self.data)
        self.data.extend(payload)
        tail = (-len(self.data)) % 32
        self.data.extend(b"\x00" * tail)

        size = len(payload) + tail
        self._put_u32(_OFF_DATA_FILE + slot * 4, file_off)
        self._put_u32(_OFF_DATA_ADDR + slot * 4, address)
        self._put_u32(_OFF_DATA_SIZE + slot * 4, size)

        section = Section("data", slot, file_off, address, size)
        self.sections.append(section)
        return section

    def overlaps_existing(self, address: int, size: int) -> Optional[Section]:
        for section in self.sections:
            if not section.used:
                continue
            if address < section.end and section.address < address + size:
                return section
        if address < self.bss_addr + self.bss_size and self.bss_addr < address + size:
            return Section("bss", 0, 0, self.bss_addr, self.bss_size)
        return None

    # -- reporting ---------------------------------------------------------

    def describe(self) -> str:
        lines = [
            f"entry           0x{self.entry:08X}",
            f"bss             0x{self.bss_addr:08X} + 0x{self.bss_size:X} "
            f"-> 0x{self.bss_addr + self.bss_size:08X}",
            "",
            f"{'slot':<8} {'file off':>10} {'address':>12} {'size':>10} {'end':>12}",
        ]
        for section in self.sections:
            if not section.used:
                continue
            name = f"{section.kind}{section.index}"
            lines.append(
                f"{name:<8} 0x{section.file_off:08X} "
                f"0x{section.address:08X}   0x{section.size:08X} "
                f"0x{section.end:08X}"
            )
        free_text = [
            s.index for s in self.sections if s.kind == "text" and not s.used
        ]
        free_data = [
            s.index for s in self.sections if s.kind == "data" and not s.used
        ]
        lines.append("")
        lines.append(f"free text slots: {free_text}")
        lines.append(f"free data slots: {free_data}")
        lines.append(f"highest mapped end: 0x{self.highest_mapped_end():08X}")
        return "\n".join(lines)


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        sys.exit("usage: doldef.py <main.dol>")
    print(Dol.load(Path(sys.argv[1])).describe())
