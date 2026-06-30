#!/usr/bin/env python3
"""
gen_accessors.py - Auto-generate matching C code for trivial accessor functions.

Reads the disassembly from build/GC6E01/asm/auto_01_800055E0_text.s and
config/GC6E01/symbols.txt to identify functions matching simple getter/setter
patterns, then generates matching C code and appends it to source files.

Patterns detected:
  A) Simple getter:       load rD, OFFSET(r3); blr
  B) Null-check getter:   cmplwi r3,0; bne/beq; li r3,0; blr; load rD, OFFSET(r3); blr
  C) Simple setter:       store rS, OFFSET(r3); blr
  D) Null-check setter:   cmplwi r3,0; beqlr; store rS, OFFSET(r3); blr
  E) SDA getter:          load r3, sym@sda21(r0/r13); blr
  F) SDA setter:          store r3, sym@sda21(r0/r13); blr
  G) Return constant:     li r3, IMM; blr
  H) Null-check u32 get:  cmplwi r3,0; bne; li r3,0; blr; lwz/lhz/lbz r3, OFF(r3); blr
  I) Null-check u32 set:  cmplwi r3,0; beqlr; stw/sth/stb r4, OFF(r3); blr

Usage:
    python tools/gen_accessors.py                  # Scan and report
    python tools/gen_accessors.py --generate       # Generate C code into source files
    python tools/gen_accessors.py --dry-run        # Show what would be generated
    python tools/gen_accessors.py --stats          # Print statistics only
"""

import argparse
import os
import re
import sys
from pathlib import Path
from collections import defaultdict

PROJECT_ROOT = Path(__file__).resolve().parent.parent
ASM_FILE = PROJECT_ROOT / "build" / "GC6E01" / "asm" / "auto_01_800055E0_text.s"
SYMBOLS_FILE = PROJECT_ROOT / "config" / "GC6E01" / "symbols.txt"
LINK_ORDER = PROJECT_ROOT / "config" / "GC6E01" / "link_order.txt"
SRC_DIR = PROJECT_ROOT / "src"

# ============================================================================
# Assembly parsing
# ============================================================================

class AsmFunction:
    """Represents a parsed assembly function."""
    def __init__(self, name, addr, size, scope, instructions):
        self.name = name
        self.addr = addr
        self.size = size
        self.scope = scope
        self.instructions = instructions  # list of (addr, mnemonic, operands_str)


def parse_asm_file(asm_path):
    """Parse the assembly file and return a dict of AsmFunction by name."""
    functions = {}
    current_fn = None
    current_instrs = []
    current_addr = 0
    current_size = 0
    current_scope = "global"

    # Pre-compile regex patterns
    fn_start_re = re.compile(r'^\.fn\s+(\w+)(?:,\s*(\w+))?')
    fn_end_re = re.compile(r'^\.endfn\s+(\w+)')
    comment_re = re.compile(r'^# \.text:.*\|\s*(0x[0-9A-Fa-f]+)\s*\|\s*size:\s*(0x[0-9A-Fa-f]+)')
    instr_re = re.compile(r'/\*\s*([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f ]+\*/\s+(.+)')
    label_re = re.compile(r'^\.\w+:')

    pending_addr = 0
    pending_size = 0

    with open(asm_path, 'r') as f:
        for line in f:
            line = line.rstrip()

            # Size comment before function
            m = comment_re.match(line)
            if m:
                pending_addr = int(m.group(1), 16)
                pending_size = int(m.group(2), 16)
                continue

            # Function start
            m = fn_start_re.match(line)
            if m:
                if current_fn:
                    functions[current_fn] = AsmFunction(
                        current_fn, current_addr, current_size,
                        current_scope, current_instrs
                    )
                current_fn = m.group(1)
                current_scope = m.group(2) or "global"
                current_addr = pending_addr
                current_size = pending_size
                current_instrs = []
                continue

            # Function end
            m = fn_end_re.match(line)
            if m:
                if current_fn:
                    functions[current_fn] = AsmFunction(
                        current_fn, current_addr, current_size,
                        current_scope, current_instrs
                    )
                current_fn = None
                current_instrs = []
                continue

            # Skip labels (but keep them noted)
            if label_re.match(line):
                continue

            # Instruction line
            if current_fn:
                m = instr_re.match(line.strip())
                if m:
                    addr = int(m.group(1), 16)
                    rest = m.group(2).strip()
                    # Split mnemonic and operands
                    parts = rest.split(None, 1)
                    mnemonic = parts[0]
                    operands = parts[1] if len(parts) > 1 else ""
                    current_instrs.append((addr, mnemonic, operands))

    return functions


# ============================================================================
# Link order parsing
# ============================================================================

class SourceRange:
    """A translation unit with address range."""
    def __init__(self, src_path, start_addr, end_addr):
        self.src_path = src_path
        self.start_addr = start_addr
        self.end_addr = end_addr


def parse_link_order(link_order_path):
    """Parse link_order.txt and return a sorted list of SourceRange."""
    ranges = []
    line_re = re.compile(
        r'^(src/\S+\.c)\s+(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)'
    )
    with open(link_order_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            m = line_re.match(line)
            if m:
                src = m.group(1)
                start = int(m.group(2), 16)
                end = int(m.group(3), 16)
                ranges.append(SourceRange(src, start, end))
    ranges.sort(key=lambda r: r.start_addr)
    return ranges


def find_source_file(addr, source_ranges):
    """Find which source file an address belongs to."""
    for sr in source_ranges:
        if sr.start_addr <= addr < sr.end_addr:
            return sr.src_path
    return None


# ============================================================================
# Pattern matching
# ============================================================================

def parse_load_store_offset(operands):
    """Parse 'rD, OFFSET(rBase)' -> (reg, offset, base_reg) or None."""
    m = re.match(r'r(\d+),\s*(-?(?:0x[0-9a-fA-F]+|\d+))\(r(\d+)\)', operands)
    if m:
        reg = int(m.group(1))
        offset_str = m.group(2)
        if offset_str.startswith('-0x'):
            offset = -int(offset_str[1:], 16)
        elif offset_str.startswith('0x'):
            offset = int(offset_str, 16)
        elif offset_str.startswith('-'):
            offset = int(offset_str)
        else:
            offset = int(offset_str)
        base = int(m.group(3))
        return (reg, offset, base)
    return None


def parse_sda_operands(operands):
    """Parse 'r3, sym@sda21(rBase)' -> (reg, symbol, base_reg) or None."""
    m = re.match(r'r(\d+),\s*(\w+)@sda21\(r(\d+)\)', operands)
    if m:
        return (int(m.group(1)), m.group(2), int(m.group(3)))
    # Also try float registers
    m = re.match(r'f(\d+),\s*(\w+)@sda21\(r(\d+)\)', operands)
    if m:
        return (int(m.group(1)), m.group(2), int(m.group(3)))
    return None


def parse_float_load_store_offset(operands):
    """Parse 'fD, OFFSET(rBase)' -> (freg, offset, base_reg) or None."""
    m = re.match(r'f(\d+),\s*(-?(?:0x[0-9a-fA-F]+|\d+))\(r(\d+)\)', operands)
    if m:
        freg = int(m.group(1))
        offset_str = m.group(2)
        if offset_str.startswith('-0x'):
            offset = -int(offset_str[1:], 16)
        elif offset_str.startswith('0x'):
            offset = int(offset_str, 16)
        elif offset_str.startswith('-'):
            offset = int(offset_str)
        else:
            offset = int(offset_str)
        base = int(m.group(3))
        return (freg, offset, base)
    return None


def load_type(mnemonic):
    """Return the C type for a load mnemonic, or None."""
    mapping = {
        'lwz': 'u32', 'lhz': 'u16', 'lbz': 'u8',
        'lha': 's16', 'lfs': 'f32', 'lfd': 'f64',
    }
    return mapping.get(mnemonic)


def store_type(mnemonic):
    """Return the C type for a store mnemonic, or None."""
    mapping = {
        'stw': 'u32', 'sth': 'u16', 'stb': 'u8',
        'stfs': 'f32', 'stfd': 'f64',
    }
    return mapping.get(mnemonic)


def is_load(mnemonic):
    return mnemonic in ('lwz', 'lhz', 'lbz', 'lha', 'lfs', 'lfd')


def is_store(mnemonic):
    return mnemonic in ('stw', 'sth', 'stb', 'stfs', 'stfd')


def is_float_load(mnemonic):
    return mnemonic in ('lfs', 'lfd')


def is_float_store(mnemonic):
    return mnemonic in ('stfs', 'stfd')


class MatchedFunction:
    """A function that matched a known pattern."""
    def __init__(self, name, addr, size, pattern, c_code, return_type='u32'):
        self.name = name
        self.addr = addr
        self.size = size
        self.pattern = pattern  # e.g., "simple_getter", "nullcheck_getter", etc.
        self.c_code = c_code
        self.return_type = return_type


def try_match_patterns(fn):
    """Try to match a function against known patterns. Returns MatchedFunction or None."""
    instrs = fn.instructions
    n = len(instrs)

    if n < 2:
        return None

    # Last instruction must be blr
    if instrs[-1][1] != 'blr':
        return None

    # ========================================================================
    # Pattern G: Return constant (li r3, IMM; blr) -- 2 instructions, 8 bytes
    # ========================================================================
    if n == 2 and fn.size == 0x8:
        if instrs[0][1] == 'li':
            m = re.match(r'r3,\s*(-?(?:0x[0-9a-fA-F]+|\d+))', instrs[0][2])
            if m:
                val_str = m.group(1)
                if val_str.startswith('-0x'):
                    val = -int(val_str[1:], 16)
                elif val_str.startswith('0x'):
                    val = int(val_str, 16)
                elif val_str.startswith('-'):
                    val = int(val_str)
                else:
                    val = int(val_str)
                if val < 0:
                    code = f"s32 {fn.name}(void) {{ return {val}; }}"
                    return MatchedFunction(fn.name, fn.addr, fn.size,
                                          "return_constant", code, 's32')
                else:
                    code = f"u32 {fn.name}(void) {{ return {val}; }}"
                    return MatchedFunction(fn.name, fn.addr, fn.size,
                                          "return_constant", code, 'u32')

    # ========================================================================
    # Pattern A: Simple getter (load r3, OFFSET(r3); blr) -- 2 instructions
    # ========================================================================
    if n == 2 and fn.size == 0x8:
        mnem = instrs[0][1]
        if is_load(mnem) and not is_float_load(mnem):
            parsed = parse_load_store_offset(instrs[0][2])
            if parsed and parsed[0] == 3 and parsed[2] == 3:
                offset = parsed[1]
                ctype = load_type(mnem)
                code = (f"{ctype} {fn.name}(u8* obj) {{\n"
                        f"    return *({ctype}*)((u8*)obj + 0x{offset:X});\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "simple_getter", code, ctype)
        # Float getter: lfs f1, OFFSET(r3); blr
        if is_float_load(mnem):
            parsed = parse_float_load_store_offset(instrs[0][2])
            if parsed and parsed[0] == 1 and parsed[2] == 3:
                offset = parsed[1]
                ctype = load_type(mnem)
                code = (f"{ctype} {fn.name}(u8* obj) {{\n"
                        f"    return *({ctype}*)((u8*)obj + 0x{offset:X});\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "simple_getter", code, ctype)

    # ========================================================================
    # Pattern C: Simple setter (store r4, OFFSET(r3); blr) -- 2 instructions
    # ========================================================================
    if n == 2 and fn.size == 0x8:
        mnem = instrs[0][1]
        if is_store(mnem) and not is_float_store(mnem):
            parsed = parse_load_store_offset(instrs[0][2])
            if parsed and parsed[0] == 4 and parsed[2] == 3:
                offset = parsed[1]
                ctype = store_type(mnem)
                code = (f"void {fn.name}(u8* obj, {ctype} val) {{\n"
                        f"    *({ctype}*)((u8*)obj + 0x{offset:X}) = val;\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "simple_setter", code, 'void')
        # Float setter: stfs f1, OFFSET(r3); blr
        if is_float_store(mnem):
            parsed = parse_float_load_store_offset(instrs[0][2])
            if parsed and parsed[0] == 1 and parsed[2] == 3:
                offset = parsed[1]
                ctype = store_type(mnem)
                code = (f"void {fn.name}(u8* obj, {ctype} val) {{\n"
                        f"    *({ctype}*)((u8*)obj + 0x{offset:X}) = val;\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "simple_setter", code, 'void')

    # ========================================================================
    # Pattern E: SDA getter (load r3, sym@sda21(rX); blr) -- 2 instructions
    # ========================================================================
    if n == 2 and fn.size == 0x8:
        mnem = instrs[0][1]
        if is_load(mnem) and not is_float_load(mnem):
            parsed = parse_sda_operands(instrs[0][2])
            if parsed and parsed[0] == 3:
                sym = parsed[1]
                ctype = load_type(mnem)
                code = (f"{ctype} {fn.name}(void) {{\n"
                        f"    return {sym};\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "sda_getter", code, ctype)
        # Float SDA getter: lfs f1, sym@sda21(rX); blr
        if is_float_load(mnem):
            parsed = parse_sda_operands(instrs[0][2])
            if parsed and parsed[0] == 1:
                sym = parsed[1]
                ctype = load_type(mnem)
                code = (f"{ctype} {fn.name}(void) {{\n"
                        f"    return {sym};\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "sda_getter", code, ctype)

    # ========================================================================
    # Pattern F: SDA setter (store r3, sym@sda21(rX); blr) -- 2 instructions
    # ========================================================================
    if n == 2 and fn.size == 0x8:
        mnem = instrs[0][1]
        if is_store(mnem) and not is_float_store(mnem):
            parsed = parse_sda_operands(instrs[0][2])
            if parsed and parsed[0] == 3:
                sym = parsed[1]
                ctype = store_type(mnem)
                code = (f"void {fn.name}({ctype} val) {{\n"
                        f"    {sym} = val;\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "sda_setter", code, 'void')
        # Float SDA setter: stfs f1, sym@sda21(rX); blr
        if is_float_store(mnem):
            parsed = parse_sda_operands(instrs[0][2])
            if parsed and parsed[0] == 1:
                sym = parsed[1]
                ctype = store_type(mnem)
                code = (f"void {fn.name}({ctype} val) {{\n"
                        f"    {sym} = val;\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "sda_setter", code, 'void')

    # ========================================================================
    # Pattern H: Null-check getter (4-5 instructions, 0x14-0x18 bytes)
    # cmplwi r3, 0; bne .L; li r3, 0; blr; .L: load r3, OFF(r3); blr
    # ========================================================================
    if n >= 4 and fn.size in (0x14, 0x18, 0x1C):
        if (instrs[0][1] == 'cmplwi' and
            'r3' in instrs[0][2] and '0x0' in instrs[0][2].replace(' ', '')):
            # Check for bne + li r3,0 + blr pattern
            if instrs[1][1] == 'bne' and instrs[2][1] == 'li' and instrs[3][1] == 'blr':
                m_li = re.match(r'r3,\s*0x0', instrs[2][2].replace(' ', ''))
                if m_li:
                    # The actual getter is after the blr
                    if n >= 5:
                        mnem = instrs[4][1]
                        if is_load(mnem) and not is_float_load(mnem):
                            parsed = parse_load_store_offset(instrs[4][2])
                            if parsed and parsed[0] == 3 and parsed[2] == 3:
                                offset = parsed[1]
                                ctype = load_type(mnem)
                                code = (f"{ctype} {fn.name}(u8* ptr) {{\n"
                                        f"    if (ptr == NULL) {{ return 0; }}\n"
                                        f"    return *({ctype}*)(&ptr[0x{offset:X}]);\n"
                                        f"}}")
                                return MatchedFunction(fn.name, fn.addr, fn.size,
                                                      "nullcheck_getter", code, ctype)
                        # Check for load into r0 then move to r3 (extsb pattern for s8)
                        if mnem == 'lbz':
                            parsed = parse_load_store_offset(instrs[4][2])
                            if parsed and parsed[0] == 0 and parsed[2] == 3:
                                # Check next instruction for extsb r3, r0
                                if n >= 6 and instrs[5][1] == 'extsb':
                                    m_ext = re.match(r'r3,\s*r0', instrs[5][2])
                                    if m_ext:
                                        offset = parsed[1]
                                        code = (f"s32 {fn.name}(u8* ptr) {{\n"
                                                f"    if (ptr == NULL) {{ return 0; }}\n"
                                                f"    return (s8)ptr[0x{offset:X}];\n"
                                                f"}}")
                                        return MatchedFunction(fn.name, fn.addr, fn.size,
                                                              "nullcheck_getter_s8", code, 's32')
                        # Float getter with null check
                        if is_float_load(mnem):
                            parsed = parse_float_load_store_offset(instrs[4][2])
                            if parsed and parsed[0] == 1 and parsed[2] == 3:
                                offset = parsed[1]
                                ctype = load_type(mnem)
                                code = (f"{ctype} {fn.name}(u8* ptr) {{\n"
                                        f"    if (ptr == NULL) {{ return 0; }}\n"
                                        f"    return *({ctype}*)(&ptr[0x{offset:X}]);\n"
                                        f"}}")
                                return MatchedFunction(fn.name, fn.addr, fn.size,
                                                      "nullcheck_getter", code, ctype)
                    # Check for load r3 directly (lbz r3, OFF(r3))
                    if n == 5:
                        mnem = instrs[4][1]
                        if is_load(mnem) and not is_float_load(mnem):
                            parsed = parse_load_store_offset(instrs[4][2])
                            if parsed and parsed[0] == 3 and parsed[2] == 3:
                                offset = parsed[1]
                                ctype = load_type(mnem)
                                code = (f"u32 {fn.name}(u8* ptr) {{\n"
                                        f"    if (ptr == NULL) {{ return 0; }}\n"
                                        f"    return *({ctype}*)(&ptr[0x{offset:X}]);\n"
                                        f"}}")
                                return MatchedFunction(fn.name, fn.addr, fn.size,
                                                      "nullcheck_getter", code, 'u32')

    # ========================================================================
    # Pattern I: Null-check setter (4 instructions, 0x10 bytes)
    # cmplwi r3, 0; beqlr; store r4, OFF(r3); blr
    # ========================================================================
    if n == 4 and fn.size == 0x10:
        if (instrs[0][1] == 'cmplwi' and
            'r3' in instrs[0][2] and '0' in instrs[0][2]):
            if instrs[1][1] == 'beqlr':
                mnem = instrs[2][1]
                if is_store(mnem) and not is_float_store(mnem):
                    parsed = parse_load_store_offset(instrs[2][2])
                    if parsed and parsed[0] == 4 and parsed[2] == 3:
                        offset = parsed[1]
                        ctype = store_type(mnem)
                        code = (f"void {fn.name}(u8* ptr, {ctype} val) {{\n"
                                f"    if (ptr == NULL) {{ return; }}\n"
                                f"    *({ctype}*)(&ptr[0x{offset:X}]) = val;\n"
                                f"}}")
                        return MatchedFunction(fn.name, fn.addr, fn.size,
                                              "nullcheck_setter", code, 'void')
                # Float setter with null check
                if is_float_store(mnem):
                    parsed = parse_float_load_store_offset(instrs[2][2])
                    if parsed and parsed[0] == 1 and parsed[2] == 3:
                        offset = parsed[1]
                        ctype = store_type(mnem)
                        code = (f"void {fn.name}(u8* ptr, {ctype} val) {{\n"
                                f"    if (ptr == NULL) {{ return; }}\n"
                                f"    *({ctype}*)(&ptr[0x{offset:X}]) = val;\n"
                                f"}}")
                        return MatchedFunction(fn.name, fn.addr, fn.size,
                                              "nullcheck_setter", code, 'void')

    # ========================================================================
    # Pattern: Null-check u16/u32 getter returning u32
    # (5 instrs, 0x18 bytes): cmplwi r3,0; bne .L; li r3,0; blr; lhz/lwz r3,OFF(r3); blr
    # Already handled above, but ensure we also match with lwz r0 pattern
    # ========================================================================

    # ========================================================================
    # Pattern: Addis getter (3 instructions, 0xC bytes)
    # addis r3, r3, IMM; load r3, OFF(r3); blr
    # ========================================================================
    if n == 3 and fn.size == 0xC:
        if instrs[0][1] == 'addis':
            m = re.match(r'r3,\s*r3,\s*(0x[0-9a-fA-F]+|\d+)', instrs[0][2])
            if m:
                addis_val = int(m.group(1), 16) if m.group(1).startswith('0x') else int(m.group(1))
                mnem = instrs[1][1]
                if is_load(mnem) and not is_float_load(mnem):
                    parsed = parse_load_store_offset(instrs[1][2])
                    if parsed and parsed[0] == 3 and parsed[2] == 3:
                        # Compute actual offset: addis adds (val << 16), then load adds signed offset
                        hi = addis_val << 16
                        lo = parsed[1]
                        if lo < 0:
                            actual_offset = hi + lo
                        else:
                            actual_offset = hi + lo
                        ctype = load_type(mnem)
                        code = (f"{ctype} {fn.name}(u8* obj) {{\n"
                                f"    return *({ctype}*)((u8*)obj + 0x{actual_offset:X});\n"
                                f"}}")
                        return MatchedFunction(fn.name, fn.addr, fn.size,
                                              "addis_getter", code, ctype)

    # ========================================================================
    # Pattern: Addis setter (3 instructions, 0xC bytes)
    # addis r3, r3, IMM; store r4, OFF(r3); blr
    # ========================================================================
    if n == 3 and fn.size == 0xC:
        if instrs[0][1] == 'addis':
            m = re.match(r'r3,\s*r3,\s*(0x[0-9a-fA-F]+|\d+)', instrs[0][2])
            if m:
                addis_val = int(m.group(1), 16) if m.group(1).startswith('0x') else int(m.group(1))
                mnem = instrs[1][1]
                if is_store(mnem) and not is_float_store(mnem):
                    parsed = parse_load_store_offset(instrs[1][2])
                    if parsed and parsed[0] == 4 and parsed[2] == 3:
                        hi = addis_val << 16
                        lo = parsed[1]
                        actual_offset = hi + lo
                        ctype = store_type(mnem)
                        code = (f"void {fn.name}(u8* obj, {ctype} val) {{\n"
                                f"    *({ctype}*)((u8*)obj + 0x{actual_offset:X}) = val;\n"
                                f"}}")
                        return MatchedFunction(fn.name, fn.addr, fn.size,
                                              "addis_setter", code, 'void')

    # ========================================================================
    # Pattern: Null-check addis getter (5 instrs)
    # cmplwi r3,0; beqlr; addis r3, r3, IMM; load r3, OFF(r3); blr
    # ========================================================================
    if n == 5 and fn.size in (0x14, 0x18):
        if (instrs[0][1] == 'cmplwi' and
            'r3' in instrs[0][2] and '0' in instrs[0][2]):
            if instrs[1][1] == 'beqlr' and instrs[2][1] == 'addis':
                m = re.match(r'r3,\s*r3,\s*(0x[0-9a-fA-F]+|\d+)', instrs[2][2])
                if m:
                    addis_val = int(m.group(1), 16) if m.group(1).startswith('0x') else int(m.group(1))
                    mnem = instrs[3][1]
                    if is_load(mnem) and not is_float_load(mnem):
                        parsed = parse_load_store_offset(instrs[3][2])
                        if parsed and parsed[0] == 3 and parsed[2] == 3:
                            hi = addis_val << 16
                            lo = parsed[1]
                            actual_offset = hi + lo
                            ctype = load_type(mnem)
                            code = (f"u32 {fn.name}(u8* ptr) {{\n"
                                    f"    if (ptr == NULL) {{ return 0; }}\n"
                                    f"    return *({ctype}*)((u8*)ptr + 0x{actual_offset:X});\n"
                                    f"}}")
                            return MatchedFunction(fn.name, fn.addr, fn.size,
                                                  "nullcheck_addis_getter", code, 'u32')

    # ========================================================================
    # Pattern: Null-check addis getter, bne variant (7 instrs, 0x1C bytes)
    # cmplwi r3,0; bne .L; li r3,0; blr; .L: addis r3, r3, IMM; load r3, OFF(r3); blr
    # ========================================================================
    if n == 7 and fn.size == 0x1C:
        if (instrs[0][1] == 'cmplwi' and
            'r3' in instrs[0][2] and '0' in instrs[0][2]):
            if (instrs[1][1] == 'bne' and instrs[2][1] == 'li' and
                instrs[3][1] == 'blr' and instrs[4][1] == 'addis'):
                m_li = re.match(r'r3,\s*0x0', instrs[2][2].replace(' ', ''))
                if m_li:
                    m = re.match(r'r3,\s*r3,\s*(0x[0-9a-fA-F]+|\d+)', instrs[4][2])
                    if m:
                        addis_val = int(m.group(1), 16) if m.group(1).startswith('0x') else int(m.group(1))
                        mnem = instrs[5][1]
                        if is_load(mnem) and not is_float_load(mnem):
                            parsed = parse_load_store_offset(instrs[5][2])
                            if parsed and parsed[0] == 3 and parsed[2] == 3:
                                hi = addis_val << 16
                                lo = parsed[1]
                                actual_offset = hi + lo
                                ctype = load_type(mnem)
                                code = (f"u32 {fn.name}(u8* ptr) {{\n"
                                        f"    if (ptr == NULL) {{ return 0; }}\n"
                                        f"    return *({ctype}*)((u8*)ptr + 0x{actual_offset:X});\n"
                                        f"}}")
                                return MatchedFunction(fn.name, fn.addr, fn.size,
                                                      "nullcheck_addis_getter_bne", code, 'u32')

    # ========================================================================
    # Pattern: Null-check addis setter (5 instrs)
    # cmplwi r3,0; beqlr; addis r3, r3, IMM; store r4, OFF(r3); blr
    # ========================================================================
    if n == 5 and fn.size in (0x14, 0x18):
        if (instrs[0][1] == 'cmplwi' and
            'r3' in instrs[0][2] and '0' in instrs[0][2]):
            if instrs[1][1] == 'beqlr' and instrs[2][1] == 'addis':
                m = re.match(r'r3,\s*r3,\s*(0x[0-9a-fA-F]+|\d+)', instrs[2][2])
                if m:
                    addis_val = int(m.group(1), 16) if m.group(1).startswith('0x') else int(m.group(1))
                    mnem = instrs[3][1]
                    if is_store(mnem) and not is_float_store(mnem):
                        parsed = parse_load_store_offset(instrs[3][2])
                        if parsed and parsed[0] == 4 and parsed[2] == 3:
                            hi = addis_val << 16
                            lo = parsed[1]
                            actual_offset = hi + lo
                            ctype = store_type(mnem)
                            code = (f"void {fn.name}(u8* ptr, {ctype} val) {{\n"
                                    f"    if (ptr == NULL) {{ return; }}\n"
                                    f"    *({ctype}*)((u8*)ptr + 0x{actual_offset:X}) = val;\n"
                                    f"}}")
                            return MatchedFunction(fn.name, fn.addr, fn.size,
                                                  "nullcheck_addis_setter", code, 'void')

    # ========================================================================
    # Pattern: addi return pointer (addi r3, r3, OFF; blr) -- 2 instrs, 8 bytes
    # Returns a pointer to a sub-struct within the object
    # ========================================================================
    if n == 2 and fn.size == 0x8:
        if instrs[0][1] == 'addi':
            m = re.match(r'r3,\s*r3,\s*(0x[0-9a-fA-F]+|\d+)', instrs[0][2])
            if m:
                offset_str = m.group(1)
                offset = int(offset_str, 16) if offset_str.startswith('0x') else int(offset_str)
                code = (f"u8* {fn.name}(u8* obj) {{\n"
                        f"    return (u8*)obj + 0x{offset:X};\n"
                        f"}}")
                return MatchedFunction(fn.name, fn.addr, fn.size,
                                      "addi_ptr_return", code, 'u8*')

    # ========================================================================
    # Pattern: mr r3, r4; blr (move argument to return) -- 2 instrs, 8 bytes
    # Very rare but sometimes used as identity/pass-through
    # ========================================================================

    # ========================================================================
    # Pattern: blr only (1 instruction, 4 bytes) -- void stub
    # ========================================================================
    if n == 1 and fn.size == 0x4:
        if instrs[0][1] == 'blr':
            code = f"void {fn.name}(void) {{\n}}"
            return MatchedFunction(fn.name, fn.addr, fn.size,
                                  "void_stub", code, 'void')

    return None


# ============================================================================
# Functions already in source files
# ============================================================================

def find_existing_functions(src_dir):
    """Scan all .c files to find which fn_XXXXXXXX names are already defined."""
    existing = set()

    # Match function definitions: TYPE fn_XXXXXXXX(
    # The key distinction is that a definition has a C type keyword before the name
    type_keywords = r'(?:void|u8|u16|u32|s8|s16|s32|f32|f64|BOOL|int|unsigned|signed|char|short|long|float|double)'
    fn_def_re = re.compile(
        rf'^\s*(?:/\*.*?\*/\s*)?'           # optional comment
        rf'{type_keywords}'                    # return type
        rf'(?:\s*\*)*\s+'                     # optional pointer, whitespace
        rf'(fn_[0-9A-Fa-f]{{8}})\s*\(',       # function name
        re.MULTILINE
    )
    # Also match named functions (not fn_ prefixed) that have address comments
    named_def_re = re.compile(
        rf'^\s*{type_keywords}'
        rf'(?:\s*\*)*\s+'
        rf'(\w+)\s*\([^)]*\)\s*\{{',
        re.MULTILINE
    )
    # Also catch address comments like /* Address: 0xXXXXXXXX */
    addr_comment_re = re.compile(r'/\*\s*Address:\s*0x([0-9A-Fa-f]+)\s*\*/')

    for c_file in src_dir.rglob("*.c"):
        try:
            content = c_file.read_text(encoding='utf-8', errors='replace')
        except Exception:
            continue

        # Find fn_ function definitions
        for m in fn_def_re.finditer(content):
            existing.add(m.group(1))

        # Find named function definitions (they may replace fn_ names)
        for m in named_def_re.finditer(content):
            existing.add(m.group(1))

        # Find address comments that indicate a function was already decompiled
        for m in addr_comment_re.finditer(content):
            addr = int(m.group(1), 16)
            fn_name = f"fn_{addr:08X}"
            existing.add(fn_name)

    return existing


def find_existing_functions_in_file(filepath):
    """Find fn_ names already defined, declared, or mentioned as address comments in a specific file."""
    existing = set()
    try:
        content = Path(filepath).read_text(encoding='utf-8', errors='replace')
    except Exception:
        return existing

    # Look for function definitions
    fn_def_re = re.compile(r'(?:^|\n)\s*(?:void|u8|u16|u32|s8|s16|s32|f32|f64|u8\*|void\*|BOOL|int|unsigned|signed)\s+\*?\s*(fn_[0-9A-Fa-f]{8})\s*\(')
    for m in fn_def_re.finditer(content):
        existing.add(m.group(1))

    # Also look for extern declarations (these indicate the function is already known
    # with a specific signature and we shouldn't generate a conflicting definition)
    extern_re = re.compile(r'extern\s+\S+\s+(fn_[0-9A-Fa-f]{8})\s*\(')
    for m in extern_re.finditer(content):
        existing.add(m.group(1))

    # Also look for address comments (/* Address: 0xXXXXXXXX */) with matching fn
    addr_re = re.compile(r'/\*\s*Address:\s*0x([0-9A-Fa-f]+)\s*\*/')
    for m in addr_re.finditer(content):
        addr = int(m.group(1), 16)
        fn_name = f"fn_{addr:08X}"
        existing.add(fn_name)

    # Also look for named functions that correspond (like PokemonSlotLookupDefault etc)
    # These have been renamed from fn_ names
    fn_call_re = re.compile(r'(?:^|\n)\s*(?:void|u8|u16|u32|s8|s16|s32|f32|f64|u8\*|void\*|BOOL|int|unsigned|signed)\s+\*?\s*(\w+)\s*\([^)]*\)\s*\{')
    for m in fn_call_re.finditer(content):
        existing.add(m.group(1))

    return existing


# ============================================================================
# Code generation
# ============================================================================

def build_file_block(matched_fns, src_path):
    """Build a C code block for insertion into a source file."""
    if not matched_fns:
        return ""

    lines = []
    lines.append("")
    lines.append("/* ===================================================================")
    lines.append(" * AUTO-GENERATED accessor functions")
    lines.append(f" * Generated by tools/gen_accessors.py")
    lines.append(f" * {len(matched_fns)} functions matched")
    lines.append(" * =================================================================== */")
    lines.append("")

    # Collect SDA externs needed, tracking the best type for each symbol
    sda_sym_types = {}  # sym -> set of types used
    for mf in matched_fns:
        if mf.pattern in ('sda_getter', 'sda_setter'):
            m = re.search(r'return (\w+);|(\w+) = val;', mf.c_code)
            if m:
                sym = m.group(1) or m.group(2)
                # Determine the type from the function signature
                if mf.pattern == 'sda_getter':
                    # Return type is the first word
                    rtype = mf.c_code.split()[0]
                else:
                    # Setter: void fn(TYPE val) -> find TYPE
                    m2 = re.search(r'\((\w+)\s+val\)', mf.c_code)
                    rtype = m2.group(1) if m2 else 'u32'
                if sym not in sda_sym_types:
                    sda_sym_types[sym] = set()
                sda_sym_types[sym].add(rtype)

    if sda_sym_types:
        for sym in sorted(sda_sym_types.keys()):
            types = sda_sym_types[sym]
            # Pick the widest type if multiple
            if 'u32' in types or 'f32' in types:
                etype = 'u32'
            elif 'u16' in types:
                etype = 'u16'
            elif 'u8' in types:
                etype = 'u8'
            else:
                etype = 'u32'
            lines.append(f"extern {etype} {sym};")
        lines.append("")

    # Sort by address
    matched_fns.sort(key=lambda mf: mf.addr)

    for mf in matched_fns:
        lines.append(f"/* Address: 0x{mf.addr:08X} | Size: 0x{mf.size:X} | Pattern: {mf.pattern} */")
        lines.append(mf.c_code)
        lines.append("")

    return "\n".join(lines)


# ============================================================================
# Main
# ============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Auto-generate matching C code for trivial accessor functions."
    )
    parser.add_argument(
        "--generate", action="store_true",
        help="Generate C code and write to source files"
    )
    parser.add_argument(
        "--dry-run", action="store_true",
        help="Show what would be generated without writing"
    )
    parser.add_argument(
        "--stats", action="store_true",
        help="Print statistics only"
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Verbose output"
    )
    args = parser.parse_args()

    # Default to stats mode
    if not args.generate and not args.dry_run:
        args.stats = True

    print("=" * 70)
    print("Pokemon Colosseum Accessor Function Generator")
    print("=" * 70)
    print()

    # Step 1: Parse assembly
    print(f"Parsing assembly: {ASM_FILE.name}...")
    if not ASM_FILE.exists():
        print(f"ERROR: Assembly file not found: {ASM_FILE}")
        sys.exit(1)

    functions = parse_asm_file(ASM_FILE)
    print(f"  Parsed {len(functions)} functions")

    # Step 2: Parse link order
    print(f"Parsing link order: {LINK_ORDER.name}...")
    if not LINK_ORDER.exists():
        print(f"ERROR: Link order file not found: {LINK_ORDER}")
        sys.exit(1)

    source_ranges = parse_link_order(LINK_ORDER)
    print(f"  Found {len(source_ranges)} translation units")

    # Step 3: Find existing functions in source
    print("Scanning existing source files for already-decompiled functions...")
    existing_fns = find_existing_functions(SRC_DIR)
    print(f"  Found {len(existing_fns)} already-defined function names")

    # Step 4: Pattern match
    print("\nMatching patterns...")
    matched = []
    pattern_counts = defaultdict(int)

    for fn_name, fn in functions.items():
        # Skip already-defined functions
        if fn_name in existing_fns:
            continue

        # Skip named SDK/library functions (those without fn_ prefix)
        if not fn_name.startswith('fn_') and fn_name != 'main':
            continue

        result = try_match_patterns(fn)
        if result:
            matched.append(result)
            pattern_counts[result.pattern] += 1

    print(f"\n  Total matched: {len(matched)} functions")
    print(f"\n  Pattern breakdown:")
    for pattern, count in sorted(pattern_counts.items(), key=lambda x: -x[1]):
        print(f"    {pattern:30s}: {count:4d}")

    # Step 5: Group by source file
    file_groups = defaultdict(list)
    unassigned = []

    for mf in matched:
        src = find_source_file(mf.addr, source_ranges)
        if src:
            file_groups[src].append(mf)
        else:
            unassigned.append(mf)

    print(f"\n  Assigned to {len(file_groups)} source files")
    print(f"  Unassigned (no source file mapping): {len(unassigned)}")

    if args.stats:
        print(f"\n{'=' * 70}")
        print("Source file breakdown:")
        print(f"{'=' * 70}")
        for src, fns in sorted(file_groups.items(), key=lambda x: -len(x[1])):
            print(f"  {src:50s}: {len(fns):4d} functions")

        if unassigned:
            print(f"\n  Unassigned functions by address range:")
            if unassigned:
                unassigned.sort(key=lambda mf: mf.addr)
                # Group unassigned by 64KB blocks
                blocks = defaultdict(int)
                for mf in unassigned:
                    block = mf.addr & 0xFFFF0000
                    blocks[block] += 1
                for block, count in sorted(blocks.items()):
                    print(f"    0x{block:08X}: {count:4d}")

        print(f"\n{'=' * 70}")
        print("SUMMARY")
        print(f"{'=' * 70}")
        print(f"  Total functions in binary:        {len(functions)}")
        print(f"  Already decompiled:               {len(existing_fns)}")
        print(f"  Matched accessor patterns:        {len(matched)}")
        print(f"  Assigned to source files:         {len(matched) - len(unassigned)}")
        print(f"  Unassigned:                       {len(unassigned)}")
        return

    # Step 6: For each source file, check existing and generate new code
    if args.dry_run or args.generate:
        print(f"\n{'=' * 70}")
        if args.dry_run:
            print("DRY RUN - showing what would be generated")
        else:
            print("GENERATING code into source files")
        print(f"{'=' * 70}")

        total_written = 0
        total_skipped = 0
        files_modified = 0
        files_created = 0

        for src_rel, matched_fns in sorted(file_groups.items()):
            src_path = PROJECT_ROOT / src_rel
            already_in_file = set()

            if src_path.exists():
                already_in_file = find_existing_functions_in_file(src_path)

            # Filter out functions already in this file
            new_fns = [mf for mf in matched_fns if mf.name not in already_in_file]
            skipped = len(matched_fns) - len(new_fns)
            total_skipped += skipped

            if not new_fns:
                if args.verbose:
                    print(f"  SKIP {src_rel} -- all {len(matched_fns)} already present")
                continue

            code_block = build_file_block(new_fns, src_rel)

            if args.dry_run:
                print(f"\n--- {src_rel} ({len(new_fns)} new, {skipped} skipped) ---")
                # Show first few lines
                code_lines = code_block.split('\n')
                preview = '\n'.join(code_lines[:20])
                if len(code_lines) > 20:
                    preview += f"\n  ... ({len(code_lines) - 20} more lines)"
                print(preview)
                total_written += len(new_fns)
            else:
                # Write to file
                if src_path.exists():
                    # Append to existing file
                    with open(src_path, 'a', encoding='utf-8') as f:
                        f.write(code_block)
                    print(f"  APPEND {src_rel}: +{len(new_fns)} functions ({skipped} skipped)")
                    files_modified += 1
                else:
                    # Create new file with header
                    src_path.parent.mkdir(parents=True, exist_ok=True)
                    header = (
                        f"/**\n"
                        f" * @file {src_path.name}\n"
                        f" * @brief Auto-generated accessor functions.\n"
                        f" *\n"
                        f" * Address range: see individual functions\n"
                        f" */\n"
                        f"\n"
                        f"#include \"dolphin/types.h\"\n"
                    )
                    with open(src_path, 'w', encoding='utf-8') as f:
                        f.write(header)
                        f.write(code_block)
                    print(f"  CREATE {src_rel}: {len(new_fns)} functions")
                    files_created += 1
                total_written += len(new_fns)

        print(f"\n{'=' * 70}")
        print("RESULTS")
        print(f"{'=' * 70}")
        print(f"  Functions generated:  {total_written}")
        print(f"  Functions skipped:    {total_skipped} (already in source)")
        if args.generate:
            print(f"  Files modified:       {files_modified}")
            print(f"  Files created:        {files_created}")


if __name__ == "__main__":
    main()
