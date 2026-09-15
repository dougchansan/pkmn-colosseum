#!/usr/bin/env python3
"""Minimal big-endian ELF32 reader.

Only what the co-op patch builder needs: loadable section bytes and a global
symbol table. Avoids a dependency on binutils, which is not installed here —
the mwcc toolchain shipped with the project provides the assembler and linker
but no objcopy/readelf.
"""

from __future__ import annotations

import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List

SHT_NOBITS = 8
SHT_SYMTAB = 2
SHF_ALLOC = 0x2


@dataclass
class ElfSection:
    name: str
    sh_type: int
    flags: int
    addr: int
    offset: int
    size: int


class Elf32BE:
    def __init__(self, data: bytes) -> None:
        if data[:4] != b"\x7fELF":
            raise ValueError("not an ELF file")
        if data[4] != 1 or data[5] != 2:
            raise ValueError("expected 32-bit big-endian ELF")
        self.data = data

        (
            self.e_shoff,
        ) = struct.unpack_from(">I", data, 0x20)
        self.e_shentsize, self.e_shnum, self.e_shstrndx = struct.unpack_from(
            ">HHH", data, 0x2E
        )

        raw: List[tuple] = []
        for i in range(self.e_shnum):
            off = self.e_shoff + i * self.e_shentsize
            raw.append(struct.unpack_from(">IIIIIIIIII", data, off))

        shstr_off = raw[self.e_shstrndx][4]

        self.sections: List[ElfSection] = []
        for entry in raw:
            name_off, sh_type, flags, addr, offset, size = entry[:6]
            self.sections.append(
                ElfSection(
                    self._cstr(shstr_off + name_off),
                    sh_type,
                    flags,
                    addr,
                    offset,
                    size,
                )
            )
        self._raw = raw

    @classmethod
    def load(cls, path: Path) -> "Elf32BE":
        return cls(path.read_bytes())

    def _cstr(self, off: int) -> str:
        end = self.data.index(b"\x00", off)
        return self.data[off:end].decode("ascii", "replace")

    def section_bytes(self, section: ElfSection) -> bytes:
        if section.sh_type == SHT_NOBITS:
            return b"\x00" * section.size
        return self.data[section.offset : section.offset + section.size]

    def alloc_sections(self) -> List[ElfSection]:
        return [
            s
            for s in self.sections
            if (s.flags & SHF_ALLOC) and s.size > 0 and s.addr != 0
        ]

    def flat_image(self) -> tuple:
        """Flatten allocated sections into (base_address, bytes).

        Gaps between sections are zero-filled so the result can be dropped into
        a DOL section verbatim.
        """
        alloc = self.alloc_sections()
        if not alloc:
            raise ValueError("ELF has no allocated sections")
        base = min(s.addr for s in alloc)
        end = max(s.addr + s.size for s in alloc)
        buf = bytearray(end - base)
        for s in alloc:
            buf[s.addr - base : s.addr - base + s.size] = self.section_bytes(s)
        return base, bytes(buf)

    def symbols(self) -> Dict[str, int]:
        out: Dict[str, int] = {}
        for i, s in enumerate(self.sections):
            if s.sh_type != SHT_SYMTAB:
                continue
            strtab_idx = self._raw[i][6]  # sh_link
            strtab_off = self.sections[strtab_idx].offset
            count = s.size // 16
            for n in range(count):
                off = s.offset + n * 16
                st_name, st_value = struct.unpack_from(">II", self.data, off)
                if st_name == 0:
                    continue
                out[self._cstr(strtab_off + st_name)] = st_value
        return out


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        sys.exit("usage: elfread.py <file.elf>")
    elf = Elf32BE.load(Path(sys.argv[1]))
    base, image = elf.flat_image()
    print(f"base 0x{base:08X} size 0x{len(image):X}")
    print(image[:32].hex())
    for name, value in sorted(elf.symbols().items(), key=lambda kv: kv[1]):
        print(f"  0x{value:08X} {name}")
