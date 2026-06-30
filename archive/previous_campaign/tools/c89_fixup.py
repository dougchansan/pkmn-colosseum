#!/usr/bin/env python3
"""
c89_fixup.py - Post-processor to fix Ghidra decompilation output for C89/CW compatibility.

Fixes all common Ghidra-isms that prevent compilation with MetroWerks CodeWarrior:
  1. true/false -> 1/0
  2. DAT_XXXXXXXX -> extern u32 declarations (inside function body)
  3. _DAT_XXXXXXXX -> same (overlap symbols)
  4. CONCAT44(a,b) -> (((u64)(a) << 32) | (u32)(b))
  5. SUB41(x,0) -> (u8)(x), SUB42(x,0) -> (u16)(x), etc.
  6. SEXT14(x) -> (s32)(s8)(x), SEXT24(x) -> (s32)(s16)(x)
  7. ZEXT14(x) -> (u32)(u8)(x), ZEXT24(x) -> (u32)(u16)(x)
  8. Remove /* WARNING: ... */ lines
  9. bool -> u32, (bool) -> (u32)
  10. countLeadingZeros(x) -> __cntlzw(x)
  11. unaff_rN -> SDA base handling
  12. __psq_ intrinsics -> remove (paired-single not available in C89)
  13. in_GQR0 -> remove declaration and usage
  14. Ghidra char types -> s8

Usage:
    python tools/c89_fixup.py --file input.c --output output.c
"""

import re
import sys
import argparse
from typing import List, Tuple, Optional


# ============================================================================
# Regex patterns for Ghidra-isms
# ============================================================================

RE_WARNING_LINE = re.compile(r'^\s*/\*\s*WARNING:.*\*/\s*$', re.MULTILINE)
RE_TRUE = re.compile(r'\btrue\b')
RE_FALSE = re.compile(r'\bfalse\b')
RE_BOOL_TYPE = re.compile(r'\bbool\b')
RE_BOOL_CAST = re.compile(r'\(bool\)')
RE_DAT = re.compile(r'\b(DAT_[0-9a-fA-F]{8})\b')
RE_UDAT = re.compile(r'\b(_DAT_[0-9a-fA-F]{8})\b')
RE_CONCAT = re.compile(r'\bCONCAT(\d)(\d)\s*\(([^,]+),\s*([^)]+)\)')
RE_SUB = re.compile(r'\bSUB(\d)(\d)\s*\(([^,]+),\s*(\d+)\)')
RE_SEXT = re.compile(r'\bSEXT(\d)(\d)\s*\(([^)]+)\)')
RE_ZEXT = re.compile(r'\bZEXT(\d)(\d)\s*\(([^)]+)\)')
RE_CLZ = re.compile(r'\bcountLeadingZeros\s*\(')
RE_UNAFF_R = re.compile(r'\bunaff_r(\d+)\b')
RE_IN_GQR = re.compile(r'\bin_GQR\d+\b')
RE_PSQ_LINE = re.compile(r'^\s*__psq_\w+\(.*?\);\s*$', re.MULTILINE)
RE_CHAR_VAR = re.compile(r'\bchar\s+(cVar\d+)\b')

RE_FUNC_HEADER = re.compile(
    r'/\*\s*----\s*(fn_[0-9a-fA-F]{8})\s+addr=0x[0-9a-fA-F]+\s+size=0x([0-9a-fA-F]+)\s*----\s*\*/'
)
RE_FUNC_HEADER_RAW = re.compile(
    r'/\*\s*=====\s*(fn_[0-9a-fA-F]{8})\s+addr=0x[0-9a-fA-F]+\s+size=0x([0-9a-fA-F]+)\s*=====\s*\*/'
)

# Variable declaration line patterns for removal
RE_DECL_IN_GQR = re.compile(r'^\s*u32\s+in_GQR\d+\s*;\s*$', re.MULTILINE)
RE_DECL_UNAFF = re.compile(r'^\s*(?:int|u32|u8|u16|u64|s32|s16|s8)\s+unaff_r\d+\s*;\s*$', re.MULTILINE)

# Label before closing brace (C89 requires statement after label)
RE_LABEL_BEFORE_BRACE = re.compile(r'^(LAB_[0-9a-fA-F]+:\s*)$(\s*})', re.MULTILINE)

# Ghidra 'code *' type (function pointer)
RE_CODE_PTR = re.compile(r'\bcode\s*\*')
RE_CODE_CAST = re.compile(r'\(code\s*\*\)')

# 'while( 1 )' cleanup
RE_WHILE_1 = re.compile(r'while\s*\(\s*1\s*\)')

SIZE_TO_UNSIGNED = {1: 'u8', 2: 'u16', 4: 'u32', 8: 'u64'}
SIZE_TO_SIGNED = {1: 's8', 2: 's16', 4: 's32', 8: 's64'}


def _replace_concat(m):
    src_sz = int(m.group(1))
    dst_sz = int(m.group(2))
    a = m.group(3).strip()
    b = m.group(4).strip()
    total = src_sz + dst_sz
    shift = dst_sz * 8
    total_type = SIZE_TO_UNSIGNED.get(total, 'u64')
    dst_type = SIZE_TO_UNSIGNED.get(dst_sz, 'u32')
    return f'(({total_type})({a}) << {shift} | ({dst_type})({b}))'


def _replace_sub(m):
    src_sz = int(m.group(1))
    dst_sz = int(m.group(2))
    expr = m.group(3).strip()
    byte_offset = int(m.group(4))
    dst_type = SIZE_TO_UNSIGNED.get(dst_sz, 'u8')
    if byte_offset == 0:
        return f'({dst_type})({expr})'
    else:
        shift = byte_offset * 8
        return f'({dst_type})(({expr}) >> {shift})'


def _replace_sext(m):
    src_sz = int(m.group(1))
    dst_sz = int(m.group(2))
    expr = m.group(3).strip()
    src_type = SIZE_TO_SIGNED.get(src_sz, 's8')
    dst_type = SIZE_TO_SIGNED.get(dst_sz, 's32')
    return f'({dst_type})({src_type})({expr})'


def _replace_zext(m):
    src_sz = int(m.group(1))
    dst_sz = int(m.group(2))
    expr = m.group(3).strip()
    src_type = SIZE_TO_UNSIGNED.get(src_sz, 'u8')
    dst_type = SIZE_TO_UNSIGNED.get(dst_sz, 'u32')
    return f'({dst_type})({src_type})({expr})'


def fixup_function(code: str, func_name: str = None) -> str:
    """
    Apply all C89 fixups to a single Ghidra function body.
    """
    # 1. Remove WARNING comment lines
    code = RE_WARNING_LINE.sub('', code)

    # 2. Remove __psq_ intrinsic lines (before removing in_GQR refs)
    code = RE_PSQ_LINE.sub('', code)

    # 3. Remove variable declarations for in_GQR and unaff_rN
    #    (we'll add proper ones back later)
    code = RE_DECL_IN_GQR.sub('', code)
    code = RE_DECL_UNAFF.sub('', code)

    # 4. Collect unaff register references BEFORE replacing them
    unaff_regs = set(RE_UNAFF_R.findall(code))

    # 5. Replace in_GQR0 with 0 in remaining code
    code = RE_IN_GQR.sub('0', code)

    # 6. Replace unaff_rN -> proper variable names
    for reg_num in unaff_regs:
        old = f'unaff_r{reg_num}'
        if reg_num in ('2', '13'):
            new = f'sda_{reg_num}'
        else:
            new = f'saved_r{reg_num}'
        code = re.sub(rf'\b{re.escape(old)}\b', new, code)

    # 7. Replace true/false
    code = RE_TRUE.sub('1', code)
    code = RE_FALSE.sub('0', code)

    # 8. Replace bool
    code = RE_BOOL_CAST.sub('(u32)', code)
    code = RE_BOOL_TYPE.sub('u32', code)

    # 9. Replace Ghidra macros
    code = RE_CONCAT.sub(_replace_concat, code)
    code = RE_SUB.sub(_replace_sub, code)
    code = RE_SEXT.sub(_replace_sext, code)
    code = RE_ZEXT.sub(_replace_zext, code)

    # 10. Replace countLeadingZeros -> __cntlzw
    code = RE_CLZ.sub('__cntlzw(', code)

    # 11. Replace Ghidra char -> s8
    code = RE_CHAR_VAR.sub(r's8 \1', code)

    # 11b. Replace 'code *' function pointer type
    code = RE_CODE_CAST.sub('(void *)', code)
    code = RE_CODE_PTR.sub('void *', code)

    # 11c. Fix labels before closing braces (C89 requires statement after label)
    code = RE_LABEL_BEFORE_BRACE.sub(r'\1 (void)0;\2', code)

    # 11d. Clean up while( 1 ) -> while (1)
    code = RE_WHILE_1.sub('while (1)', code)

    # 12. Collect DAT symbols for extern declarations
    dat_syms = set(RE_DAT.findall(code))
    udat_syms = set(RE_UDAT.findall(code))

    # 13. Build extern/variable declarations to insert inside function body
    externs = []

    for sym in sorted(dat_syms):
        externs.append(f'    extern u32 {sym};')
    for sym in sorted(udat_syms):
        externs.append(f'    extern u32 {sym};')

    # SDA base register declarations
    for reg_num in sorted(unaff_regs, key=int):
        if reg_num == '2':
            externs.append(f'    register int sda_2 asm("r2");')
        elif reg_num == '13':
            externs.append(f'    register int sda_13 asm("r13");')
        else:
            externs.append(f'    u32 saved_r{reg_num} = 0;')

    # 14. Insert declarations after opening brace of function body
    if externs:
        brace_idx = code.find('{')
        if brace_idx >= 0:
            # Find the newline after the brace
            nl_after = code.find('\n', brace_idx)
            if nl_after >= 0:
                insert_point = nl_after + 1
            else:
                insert_point = brace_idx + 1
            extern_block = '\n'.join(externs) + '\n'
            code = code[:insert_point] + extern_block + code[insert_point:]

    # 15. Clean up multiple blank lines
    code = re.sub(r'\n{3,}', '\n\n', code)

    return code


def parse_functions_processed(text: str) -> List[Tuple[str, str, str]]:
    """
    Parse processed Ghidra output (---- style headers) into individual functions.
    Returns list of (func_name, func_size_hex, func_code) tuples.
    """
    functions = []
    headers = list(RE_FUNC_HEADER.finditer(text))

    for idx, header in enumerate(headers):
        func_name = header.group(1)
        size_hex = header.group(2)
        start = header.end()
        if idx + 1 < len(headers):
            end = headers[idx + 1].start()
        else:
            end = len(text)
        func_code = text[start:end].strip()
        if func_code:
            functions.append((func_name, size_hex, func_code))

    return functions


def parse_functions_raw(text: str) -> List[Tuple[str, str, str]]:
    """Parse raw Ghidra decompilation (===== style headers)."""
    functions = []
    headers = list(RE_FUNC_HEADER_RAW.finditer(text))

    for idx, header in enumerate(headers):
        func_name = header.group(1)
        size_hex = header.group(2)
        start = header.end()
        if idx + 1 < len(headers):
            end = headers[idx + 1].start()
        else:
            end = len(text)
        func_code = text[start:end].strip()
        if func_code:
            functions.append((func_name, size_hex, func_code))

    return functions


def apply_type_replacements(code: str) -> str:
    """Apply Ghidra type -> project type replacements."""
    replacements = [
        (r'\bundefined8\b', 'u64'),
        (r'\bundefined4\b', 'u32'),
        (r'\bundefined2\b', 'u16'),
        (r'\bundefined1\b', 'u8'),
        (r'\bundefined\b', 'u8'),
        (r'\bulonglong\b', 'u64'),
        (r'\blonglong\b', 's64'),
        (r'\buint\b', 'u32'),
        (r'\bushort\b', 'u16'),
        (r'\bbyte\b', 'u8'),
        (r'\bulong\b', 'u32'),
    ]
    for pat, repl in replacements:
        code = re.sub(pat, repl, code)
    return code


def apply_param_renaming(code: str) -> str:
    """Rename Ghidra param_N to PowerPC register names."""
    int_map = {
        'param_1': 'r3', 'param_2': 'r4', 'param_3': 'r5',
        'param_4': 'r6', 'param_5': 'r7', 'param_6': 'r8',
        'param_7': 'r9', 'param_8': 'r10',
    }
    for old, new in int_map.items():
        code = re.sub(rf'\b{old}\b', new, code)
    return code


def fixup_raw_function(code: str, func_name: str = None) -> str:
    """Full fixup for raw Ghidra output."""
    code = apply_type_replacements(code)
    code = apply_param_renaming(code)
    code = fixup_function(code, func_name)
    return code


def main():
    parser = argparse.ArgumentParser(description='C89 fixup for Ghidra output')
    parser.add_argument('--file', '-f', help='Input file')
    parser.add_argument('--output', '-o', help='Output file')
    parser.add_argument('--raw', action='store_true',
                        help='Input is raw Ghidra output')
    args = parser.parse_args()

    if args.file:
        with open(args.file, 'r') as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    if args.raw:
        functions = parse_functions_raw(text)
        results = []
        for name, size, code in functions:
            fixed = fixup_raw_function(code, name)
            results.append(f'/* ---- {name}  size=0x{size} ---- */\n{fixed}')
        output = '\n\n'.join(results)
    else:
        functions = parse_functions_processed(text)
        if functions:
            results = []
            for name, size, code in functions:
                fixed = fixup_function(code, name)
                results.append(f'/* ---- {name}  size=0x{size} ---- */\n{fixed}')
            output = '\n\n'.join(results)
        else:
            output = fixup_function(text)

    if args.output:
        with open(args.output, 'w') as f:
            f.write(output)
    else:
        sys.stdout.write(output)


if __name__ == '__main__':
    main()
