# Ghidra headless script: seed known fn_XXXXXXXX symbols from src/game/gs_title.c
#
# Invocation (via analyzeHeadless):
#   -postScript seed_symbols.py <path/to/gs_title.c>
#
# Reads all `fn_XXXXXXXX` identifiers from the C file, extracts the 8-hex-digit
# address, and creates a named function at that address in the current program.
# If a function already exists, only the name is updated.
#@category Pokemon

import re
import sys

from ghidra.program.model.symbol import SourceType
from ghidra.program.model.address import AddressOutOfBoundsException


def seed(program, c_path):
    listing = program.getListing()
    fn_mgr = program.getFunctionManager()
    space = program.getAddressFactory().getDefaultAddressSpace()

    pattern = re.compile(r"\bfn_([0-9A-Fa-f]{8})\b")
    seen = set()

    with open(c_path, "r") as f:
        src = f.read()

    for m in pattern.finditer(src):
        hex_addr = m.group(1).lower()
        if hex_addr in seen:
            continue
        seen.add(hex_addr)

        name = "fn_" + hex_addr.upper()
        try:
            addr = space.getAddress(int(hex_addr, 16))
        except AddressOutOfBoundsException:
            continue

        block = program.getMemory().getBlock(addr)
        if block is None:
            continue

        fn = fn_mgr.getFunctionAt(addr)
        if fn is None:
            try:
                fn = fn_mgr.createFunction(name, addr, None, SourceType.IMPORTED)
            except Exception as e:
                print("create fail %s @ %s: %s" % (name, addr, e))
                continue
        else:
            try:
                fn.setName(name, SourceType.IMPORTED)
            except Exception as e:
                print("rename fail %s @ %s: %s" % (name, addr, e))

    print("Seeded %d unique fn_ addresses" % len(seen))


args = getScriptArgs()
if not args:
    print("usage: seed_symbols.py <path/to/gs_title.c>")
else:
    seed(currentProgram, args[0])
