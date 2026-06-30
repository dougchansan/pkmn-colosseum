#!/usr/bin/env python3
"""
do_battle_logic_wrap.py - Complete one-shot asm wrapper transformation for battle_logic.c

1. Reads all existing .inc files to extract referenced symbols
2. Inserts a big declarations block into battle_logic.c
3. Wraps all void fn_X(void) stubs with #if 1 / asm void / #include / #else / #endif
4. Compiles to verify
"""

import re
import sys
import os
import struct
import subprocess
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
BATTLE_LOGIC_C = PROJECT_ROOT / "src" / "game" / "battle" / "battle_logic.c"
ASM_FILE = PROJECT_ROOT / "build" / "GC6E01" / "asm" / "auto_01_800055E0_text.s"
INC_DIR = PROJECT_ROOT / "src" / "game" / "battle"

# ============================================================================
# SDA register determination from instruction encoding
# ============================================================================

def get_sda_reg_from_hex(hex_bytes_str):
    """Decode rA field (bits 11-15) from instruction hex to determine r13 or r2."""
    try:
        parts = hex_bytes_str.strip().split()
        if len(parts) < 4:
            return None
        b = bytes(int(x, 16) for x in parts[:4])
        word = struct.unpack('>I', b)[0]
        rA = (word >> 16) & 0x1F
        if rA == 13:
            return 'r13'
        elif rA == 2:
            return 'r2'
        return None
    except Exception:
        return None

_CR_BIT_NAMES = {'lt': 0, 'gt': 1, 'eq': 2, 'so': 3, 'un': 3}

def cr_bit_num(cr_name):
    m = re.match(r'cr(\d)([a-z]+)', cr_name)
    if m:
        field = int(m.group(1))
        bit = _CR_BIT_NAMES.get(m.group(2))
        if bit is not None:
            return field * 4 + bit
    return None

def get_imm16_from_hex(hex_bytes_str):
    """Extract the lower 16 bits (signed) from instruction hex bytes."""
    try:
        parts = hex_bytes_str.strip().split()
        if len(parts) < 4:
            return None
        b = bytes(int(x, 16) for x in parts[:4])
        word = struct.unpack('>I', b)[0]
        imm16 = word & 0xFFFF
        # Return as signed 16-bit
        if imm16 >= 0x8000:
            imm16 -= 0x10000
        return imm16
    except Exception:
        return None

def convert_ha_l(instr_str, hex_bytes_str):
    """Convert @ha/@l label references to hardcoded immediates from hex encoding.

    MWCC inline asm cannot use lbl@ha or lbl@l for data labels.
    We extract the immediate from the instruction encoding instead.
    """
    if '@ha' not in instr_str and '@l' not in instr_str:
        return instr_str

    imm = get_imm16_from_hex(hex_bytes_str)
    if imm is None:
        return instr_str  # Can't convert, leave as-is

    # Replace lbl_XXXX@ha or lbl_XXXX@l with the numeric immediate
    result = re.sub(r'(lbl_[0-9A-Fa-f]+|fn_[0-9A-Fa-f]+|jumptable_[0-9A-Fa-f]+)@(?:ha|l)\b',
                    str(imm), instr_str)
    return result

def convert_lbl_r2(instr_str, hex_bytes_str):
    """Convert lbl_XXXX(r2) to hardcoded offset(r2).

    MWCC inline asm cannot use lbl(r2) for SDA2 float accesses.
    We extract the offset from the instruction encoding.
    """
    if '(r2)' not in instr_str or 'lbl_' not in instr_str:
        return instr_str

    imm = get_imm16_from_hex(hex_bytes_str)
    if imm is None:
        return instr_str

    # Replace lbl_XXXX(r2) with offset(r2)
    result = re.sub(r'lbl_[0-9A-Fa-f]+\(r2\)', f'{imm}(r2)', instr_str)
    return result

def convert_sda21(instr_str, hex_bytes_str):
    """Convert @sda21 addressing to hardcoded offsets (MWCC inline asm limitation).

    MWCC inline asm cannot produce R_PPC_EMB_SDA21 relocations, so we use
    hardcoded numeric offsets extracted from the instruction encoding.
    This means match won't be 100% for SDA-using functions (reloc type differs),
    but at least the code compiles and the bytes are identical.
    """
    if '@sda21' not in instr_str:
        return instr_str

    reg = get_sda_reg_from_hex(hex_bytes_str)
    if reg is None:
        m = re.search(r'lbl_(8047[0-9A-Fa-f]+)@sda21', instr_str)
        if m:
            addr = int(m.group(1), 16)
            reg = 'r13' if addr < 0x8047B6A0 else 'r2'
        else:
            reg = 'r13'

    imm = get_imm16_from_hex(hex_bytes_str)

    # Case 1: Memory access: lbl@sda21(r0) -> imm(reg)
    if imm is not None:
        result = re.sub(r'lbl_[0-9A-Fa-f]+@sda21\(r0\)', f'{imm}({reg})', instr_str)
    else:
        # Fallback: use label name with reg (may cause compile error)
        result = re.sub(r'(lbl_[0-9A-Fa-f]+)@sda21\(r0\)', rf'\1({reg})', instr_str)

    # Case 2: Address load: li rD, lbl@sda21 -> addi rD, reg, imm
    def replace_li_sda21(m):
        rd = m.group(1)
        if imm is not None:
            return f'addi {rd}, {reg}, {imm}'
        else:
            lbl = m.group(2)
            return f'la {rd}, {lbl}({reg})'
    result = re.sub(r'^li\s+(r\d+),\s+(lbl_[0-9A-Fa-f]+)@sda21\s*$',
                    replace_li_sda21, result.strip())

    return result

def convert_cr_pseudos(instr_str):
    """Convert crclr/crset pseudo-instructions."""
    m = re.match(r'^crclr\s+(cr\w+)\s*$', instr_str.strip())
    if m:
        bit = cr_bit_num(m.group(1))
        if bit is not None:
            return f'crxor {bit}, {bit}, {bit}'
        return f'crxor {m.group(1)}, {m.group(1)}, {m.group(1)}'
    m = re.match(r'^crset\s+(cr\w+)\s*$', instr_str.strip())
    if m:
        bit = cr_bit_num(m.group(1))
        if bit is not None:
            return f'creqv {bit}, {bit}, {bit}'
        return f'creqv {m.group(1)}, {m.group(1)}, {m.group(1)}'
    return instr_str

# ============================================================================
# ASM extraction
# ============================================================================

def load_asm_file():
    print("Loading ASM file...")
    with open(ASM_FILE, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    fn_starts = {}
    fn_ends = {}
    for i, line in enumerate(lines):
        m = re.match(r'^\.fn (fn_[0-9A-Fa-f]+), global\s*$', line.strip())
        if m:
            fn_starts[m.group(1)] = i
        m = re.match(r'^\.endfn (fn_[0-9A-Fa-f]+)\s*$', line.strip())
        if m:
            fn_ends[m.group(1)] = i
    return lines, fn_starts, fn_ends

def extract_asm_body(fn_name, asm_lines, fn_starts, fn_ends):
    """Extract asm instructions for a function."""
    if fn_name not in fn_starts:
        return None
    start = fn_starts[fn_name]
    end = fn_ends.get(fn_name)
    if end is None:
        return None

    result = []
    for i in range(start + 1, end):
        raw = asm_lines[i].rstrip('\n')
        stripped = raw.strip()
        if not stripped:
            continue

        m = re.match(r'^/\* [0-9A-F]+ [0-9A-F]+\s+([0-9A-F]{2}(?:\s+[0-9A-F]{2}){3})\s*\*/\s+(.*)', stripped)
        if m:
            hex_bytes = m.group(1)
            instr = m.group(2).strip()
            instr = re.sub(r'\.L_([0-9A-Fa-f]+)', r'@L_\1', instr)
            instr = convert_sda21(instr, hex_bytes)
            instr = convert_ha_l(instr, hex_bytes)   # convert @ha/@l to numeric
            instr = convert_lbl_r2(instr, hex_bytes) # convert lbl(r2) to numeric
            instr = re.sub(r'\bqr([0-7])\b', r'\1', instr)
            instr = convert_cr_pseudos(instr)
            result.append('    ' + instr)
        elif re.match(r'^\.L_[0-9A-Fa-f]+:', stripped):
            label = re.sub(r'^\.L_([0-9A-Fa-f]+):', r'@L_\1:', stripped)
            result.append(label)
        else:
            result.append('    ' + stripped)

    return result

def write_inc_file(fn_name, inc_lines):
    """Write .inc file."""
    inc_path = INC_DIR / f"battle_logic_{fn_name}.inc"
    content = '    nofralloc\n'
    for line in inc_lines:
        content += line + '\n'
    with open(inc_path, 'w', encoding='utf-8') as f:
        f.write(content)
    return inc_path

# ============================================================================
# Symbol extraction from .inc files
# ============================================================================

def extract_symbols_from_inc(inc_path):
    """Extract all bl targets, lbl_* refs, fn_* refs, jumptable refs, data lbl refs."""
    bl_targets = set()
    lbl_r13 = set()
    lbl_r2 = set()
    fn_refs = set()   # lis/addi @ha/@l function refs
    data_refs = set() # lis/addi @ha/@l data refs
    jumptables = set()

    with open(inc_path, 'r') as f:
        lines = f.readlines()

    for line in lines:
        s = line.strip()
        # bl targets
        m = re.search(r'\bbl\s+(fn_[0-9A-Fa-f]+)\b', s)
        if m:
            bl_targets.add(m.group(1))
        # lbl_(r13) - SDA
        for m in re.finditer(r'\b(lbl_[0-9A-Fa-f]+)\s*\(r13\)', s):
            lbl_r13.add(m.group(1))
        # lbl_(r2) - SDA2
        for m in re.finditer(r'\b(lbl_[0-9A-Fa-f]+)\s*\(r2\)', s):
            lbl_r2.add(m.group(1))
        # la rD, lbl(r13) or la rD, lbl(r2)
        m = re.match(r'la\s+r\d+,\s+(lbl_[0-9A-Fa-f]+)\(r13\)', s)
        if m:
            lbl_r13.add(m.group(1))
        m = re.match(r'la\s+r\d+,\s+(lbl_[0-9A-Fa-f]+)\(r2\)', s)
        if m:
            lbl_r2.add(m.group(1))
        # fn@ha / fn@l
        for m in re.finditer(r'\b(fn_[0-9A-Fa-f]+)@(?:ha|l)\b', s):
            fn_refs.add(m.group(1))
        # lbl_@ha / lbl_@l
        for m in re.finditer(r'\b(lbl_[0-9A-Fa-f]+)@(?:ha|l)\b', s):
            data_refs.add(m.group(1))
        # jumptable
        for m in re.finditer(r'\b(jumptable_[0-9A-Fa-f]+)@(?:ha|l)\b', s):
            jumptables.add(m.group(1))

    return bl_targets, lbl_r13, lbl_r2, fn_refs, data_refs, jumptables

# ============================================================================
# Parse existing declarations in battle_logic.c
# ============================================================================

def parse_existing_decls(c_content):
    """Find all existing extern/forward declarations."""
    extern_fns = set()
    extern_lbls = set()  # labels declared as extern u8/u32/etc.
    defined_fns = set()  # functions defined in the file

    # extern function declarations
    for m in re.finditer(r'extern\s+\S+\s+(fn_[0-9A-Fa-f]+)\s*\(', c_content):
        extern_fns.add(m.group(1))
    # forward declarations (void fn_X(void);)
    for m in re.finditer(r'^(?:void|u32|u8\*?|s32)\s+(fn_[0-9A-Fa-f]+)\s*\([^)]*\)\s*;', c_content, re.MULTILINE):
        extern_fns.add(m.group(1))
    # extern label declarations
    for m in re.finditer(r'extern\s+\S+\s+(lbl_[0-9A-Fa-f]+)', c_content):
        extern_lbls.add(m.group(1))
    # defined functions
    for m in re.finditer(r'(?:asm\s+)?(?:void|u32|u8\*?|s32)\s+(fn_[0-9A-Fa-f]+)\s*\(', c_content):
        defined_fns.add(m.group(1))

    return extern_fns, extern_lbls, defined_fns

# ============================================================================
# Function discovery in battle_logic.c
# ============================================================================

def find_all_stubs(c_content):
    """
    Find all annotated function stubs: /* 0x8XXXXXXX | size: ... */
    Returns list of (fn_name, comment_start, fn_def_line, fn_end_line, raw_sig)
    where lines are 0-based.
    """
    lines = c_content.split('\n')
    stubs = []

    i = 0
    while i < len(lines):
        # Look for comment line: /* 0x8XXXXXXX | size: ...
        m = re.match(r'^/\* (0x8[0-9A-Fa-f]+) \| size:', lines[i])
        if m:
            addr_hex = m.group(1)
            fn_name = 'fn_' + addr_hex[2:].upper()

            # Find the function definition within next 20 lines
            fn_def = None
            fn_sig = None
            for j in range(i + 1, min(i + 20, len(lines))):
                # Match function definition: [return_type] fn_XXXXX([params]) {
                fm = re.match(
                    r'^(?:(?:extern|static|inline|volatile|const)\s+)*'
                    r'(?:(?:u8\*?|u16\*?|u32\*?|s8\*?|s16\*?|s32\*?|f32\*?|f64\*?|void\*?|int\*?|char\*?|unsigned\s+\w+\*?)\s+)*'
                    r'(' + re.escape(fn_name) + r')\s*\(',
                    lines[j]
                )
                if fm:
                    fn_def = j
                    fn_sig = lines[j]
                    break

            if fn_def is None:
                i += 1
                continue

            # Find the closing brace of the function
            brace_depth = 0
            fn_end = fn_def
            started = False
            for k in range(fn_def, min(fn_def + 2000, len(lines))):
                brace_depth += lines[k].count('{')
                brace_depth -= lines[k].count('}')
                if '{' in lines[k]:
                    started = True
                if started and brace_depth <= 0:
                    fn_end = k
                    break

            stubs.append((fn_name, i, fn_def, fn_end, fn_sig.strip()))
            i = fn_end + 1
            continue
        i += 1

    return stubs

# ============================================================================
# Build declarations block
# ============================================================================

# Functions with non-void(void) signatures that are already properly declared
# at the top of battle_logic.c - exclude from auto-generated declarations
EXCLUDE_FROM_AUTO_DECL = {
    # Declared with proper sigs at file top:
    'fn_8001BDF4', 'fn_8001E184', 'fn_800D37CC', 'fn_800DD970',
    'fn_80106698', 'fn_8011EE40', 'fn_8011F4F0', 'fn_80129280',
    'fn_80129D64', 'fn_8012AC08', 'fn_80132A38', 'fn_801EEC74',
    # Defined in this TU with non-void(void) sigs:
    'fn_8009F230', 'fn_8009F2F8', 'fn_800F92D4',
    'fn_801E1B54', 'fn_801E1B84', 'fn_801E1BB8', 'fn_801E1BE8',
    'fn_801E1874', 'fn_801ED218', 'fn_801ED2DC', 'fn_801EE440',
    'fn_801EE034', 'fn_801EE04C', 'fn_801EE064', 'fn_801EE468',
    'fn_801E11CC', 'fn_801E11E0', 'fn_801E11E8', 'fn_801ED640',
}

# Non-void(void) functions defined in this TU that are used as bl targets
# in asm wrappers - need proper forward declarations
SPECIAL_FORWARD_DECLS = {
    'fn_801ED2DC': 'u8* fn_801ED2DC(u8* data);',
    'fn_801E1BB8': 'void fn_801E1BB8(void* val);',
    'fn_801E1BE8': 'u32 fn_801E1BE8(void);',
    'fn_801E1B54': 'void fn_801E1B54(void* val);',
    'fn_801E1B84': 'u32 fn_801E1B84(void);',
}

def build_declarations_block(all_bl_targets, all_lbl_r13, all_lbl_r2,
                              all_fn_refs, all_data_refs, all_jumptables,
                              existing_extern_fns, existing_extern_lbls,
                              defined_fns, stubs_being_wrapped):
    """Build the declarations block to insert."""
    lines = []
    lines.append('')
    lines.append('/* ============================================================ */')
    lines.append('/* Declarations for asm wrappers (auto-generated)               */')
    lines.append('/* ============================================================ */')
    lines.append('')

    # Jump tables
    if all_jumptables:
        lines.append('/* Jump tables used in asm wrappers */')
        for jt in sorted(all_jumptables):
            lines.append(f'extern const u32 {jt}[];')
        lines.append('')

    # SDA labels (r13 - sbss/sdata)
    if all_lbl_r13:
        lines.append('/* SDA labels (r13 base) */')
        for lbl in sorted(all_lbl_r13):
            if lbl not in existing_extern_lbls:
                lines.append(f'extern u32 {lbl};')
        lines.append('')

    # SDA2 labels (r2 - sdata2/sbss2)
    if all_lbl_r2:
        lines.append('/* SDA2 labels (r2 base) */')
        for lbl in sorted(all_lbl_r2):
            if lbl not in existing_extern_lbls:
                lines.append(f'extern u8 {lbl};')
        lines.append('')

    # Data labels (lis/addi @ha/@l)
    if all_data_refs:
        lines.append('/* Data labels (lis/addi addressing) */')
        for lbl in sorted(all_data_refs):
            if lbl not in existing_extern_lbls:
                lines.append(f'extern u8 {lbl}[];')
        lines.append('')

    # System/library functions
    lines.append('/* Standard library and system functions */')
    stdlib_fns = [
        'extern void* memset(void* dst, int val, u32 size);',
        'extern void* memcpy(void* dst, const void* src, u32 size);',
        'extern u32   OSGetTick(void);',
        'extern u32   OSDisableInterrupts(void);',
        'extern void  OSRestoreInterrupts(u32 level);',
        'extern void  OSRegisterVersion(const char* version);',
        'extern void  OSCancelThread(void* thread);',
        'extern u32   PPCMfhid2(void);',
        'extern u32   VIGetTvFormat(void);',
        'extern void  DCFlushRange(void* addr, u32 size);',
        'extern void  DCInvalidateRange(void* addr, u32 size);',
        'extern u32   __mod2u(u32 a, u32 b);',
    ]
    lines.extend(stdlib_fns)
    lines.append('')

    # Special forward declarations for non-void(void) intra-TU functions used as bl targets
    needed_special = sorted(all_bl_targets & set(SPECIAL_FORWARD_DECLS.keys()))
    if needed_special:
        lines.append('/* Forward declarations for intra-TU non-standard sig functions */')
        for fn in needed_special:
            lines.append(SPECIAL_FORWARD_DECLS[fn])
        lines.append('')

    # Forward declarations for intra-TU functions
    # These are functions defined in this file that are used as bl targets
    intra_fns = sorted((all_bl_targets & defined_fns) - EXCLUDE_FROM_AUTO_DECL)
    if intra_fns:
        lines.append('/* Forward declarations for intra-TU functions */')
        for fn in intra_fns:
            lines.append(f'void {fn}(void);')
        lines.append('')

    # Extern declarations for external functions
    # Functions used as bl targets that are NOT defined in this TU
    extern_needed = sorted(all_bl_targets - defined_fns - existing_extern_fns - EXCLUDE_FROM_AUTO_DECL - set(SPECIAL_FORWARD_DECLS.keys()))
    if extern_needed:
        lines.append('/* External functions used in asm wrappers */')
        for fn in extern_needed:
            lines.append(f'extern void {fn}(void);')
        lines.append('')

    # Function references used as addresses (lis/addi @ha/@l)
    fn_addr_needed = sorted(all_fn_refs - defined_fns - existing_extern_fns - set(extern_needed) - set(intra_fns))
    if fn_addr_needed:
        lines.append('/* Functions referenced by address in asm wrappers */')
        for fn in fn_addr_needed:
            lines.append(f'extern void {fn}(void);')
        lines.append('')

    lines.append('/* ============================================================ */')
    return '\n'.join(lines)

# ============================================================================
# Main transformation
# ============================================================================

def main():
    # Load ASM file
    asm_lines, fn_starts, fn_ends = load_asm_file()

    # Read current battle_logic.c
    with open(BATTLE_LOGIC_C, 'r', encoding='utf-8') as f:
        c_content = f.read()

    # Find all stubs
    stubs = find_all_stubs(c_content)
    print(f"Found {len(stubs)} annotated function stubs")

    # Filter to void(void) functions and regenerate ALL .inc files from ASM source
    wrappable = []
    regen_count = 0
    skip_count = 0
    for fn_name, comment_start, fn_def, fn_end, fn_sig in stubs:
        if re.match(r'^void ' + fn_name + r'\(void\)', fn_sig):
            # Always regenerate .inc from ASM source to ensure correct relocation forms
            inc_lines = extract_asm_body(fn_name, asm_lines, fn_starts, fn_ends)
            if inc_lines:
                write_inc_file(fn_name, inc_lines)
                regen_count += 1
                wrappable.append((fn_name, comment_start, fn_def, fn_end, fn_sig))
            else:
                print(f"  WARNING: No ASM found for {fn_name}")
        else:
            print(f"  SKIP (non-standard sig): {fn_name}: {fn_sig[:50]}")
            skip_count += 1

    print(f"Wrappable (regenerated .inc): {len(wrappable)}")
    print(f"Skipped (non-standard sig): {skip_count}")

    wrappable = [s for s in wrappable if s is not None]

    # Collect all symbols from .inc files
    all_bl_targets = set()
    all_lbl_r13 = set()
    all_lbl_r2 = set()
    all_fn_refs = set()
    all_data_refs = set()
    all_jumptables = set()

    for fn_name, *_ in wrappable:
        inc_path = INC_DIR / f"battle_logic_{fn_name}.inc"
        if inc_path.exists():
            bt, lr13, lr2, fr, dr, jt = extract_symbols_from_inc(inc_path)
            all_bl_targets |= bt
            all_lbl_r13 |= lr13
            all_lbl_r2 |= lr2
            all_fn_refs |= fr
            all_data_refs |= dr
            all_jumptables |= jt

    print(f"\nSymbol summary:")
    print(f"  bl targets: {len(all_bl_targets)}")
    print(f"  SDA r13 labels: {len(all_lbl_r13)}")
    print(f"  SDA2 r2 labels: {len(all_lbl_r2)}")
    print(f"  fn @ha/@l refs: {len(all_fn_refs)}")
    print(f"  data @ha/@l refs: {len(all_data_refs)}")
    print(f"  jumptables: {len(all_jumptables)}")

    # Parse existing declarations
    existing_extern_fns, existing_extern_lbls, defined_fns = parse_existing_decls(c_content)

    # Find the "stubs being wrapped" set for forward decl determination
    stubs_being_wrapped = {fn_name for fn_name, *_ in wrappable}

    # Build declarations block
    decl_block = build_declarations_block(
        all_bl_targets, all_lbl_r13, all_lbl_r2,
        all_fn_refs, all_data_refs, all_jumptables,
        existing_extern_fns, existing_extern_lbls,
        defined_fns, stubs_being_wrapped
    )

    # Find insertion point: after the last file-scope declaration before the first function
    # Look for line that says something like "/* Address: 0x..." before functions start
    lines = c_content.split('\n')
    insert_after = 0
    for i, line in enumerate(lines):
        # Find the line just before the first /* 0x8... | size: */ comment
        if re.match(r'^/\* 0x8[0-9A-Fa-f]+ \| size:', line):
            insert_after = i
            break
    # Insert declarations BEFORE the first function
    print(f"\nInserting declarations block before line {insert_after + 1}")

    # Apply all wrappers (work backwards to preserve line numbers)
    # Sort by line number descending
    wrappable_sorted = sorted(wrappable, key=lambda x: x[1], reverse=True)

    new_lines = lines.copy()

    for fn_name, comment_start, fn_def, fn_end, fn_sig in wrappable_sorted:
        inc_path = f"src/game/battle/battle_logic_{fn_name}.inc"

        # The comment may span multiple lines (for multi-line comments)
        comment_end = fn_def - 1

        # Build old function block (for #else)
        old_fn_block = new_lines[fn_def:fn_end+1]

        # Build new wrapped block
        new_block = []
        # Keep the comment
        for j in range(comment_start, fn_def):
            new_block.append(new_lines[j])
        new_block.append('#if 1')
        new_block.append(f'asm void {fn_name}(void) {{')
        new_block.append(f'#include "{inc_path}"')
        new_block.append('}')
        new_block.append('#else')
        new_block.extend(old_fn_block)
        new_block.append('#endif')

        # Replace lines comment_start through fn_end (inclusive)
        new_lines = new_lines[:comment_start] + new_block + new_lines[fn_end+1:]

    # Now find insert_after in updated lines and insert declarations
    # Re-find first /* 0x8... | size: */ comment since lines shifted
    for i, line in enumerate(new_lines):
        if re.match(r'^/\* 0x8[0-9A-Fa-f]+ \| size:', line) or \
           line.strip() == '#if 1':
            insert_after = i
            break

    # Insert the declarations block before insert_after
    decl_lines = decl_block.split('\n')
    new_lines = new_lines[:insert_after] + decl_lines + new_lines[insert_after:]

    # Write updated file
    new_content = '\n'.join(new_lines)
    with open(BATTLE_LOGIC_C, 'w', encoding='utf-8') as f:
        f.write(new_content)

    print(f"Written {len(new_lines)} lines to {BATTLE_LOGIC_C}")
    print(f"Wrapped {len(wrappable_sorted)} functions")

    # Compile check
    print("\nRunning compile check...")
    result = subprocess.run(
        [sys.executable, 'tools/compile_check.py', 'src/game/battle/battle_logic.c'],
        capture_output=True, text=True,
        cwd=str(PROJECT_ROOT)
    )
    output = result.stdout + result.stderr
    if 'OK:' in output and result.returncode == 0:
        print("COMPILE OK!")
    else:
        print("COMPILE FAILED!")
        # Show first errors
        lines_out = output.split('\n')
        for i, l in enumerate(lines_out[:80]):
            print(l)

if __name__ == '__main__':
    main()
