#!/usr/bin/env python3
"""
bulk_import.py - Bulk import ALL Ghidra functions into colosseum_script.c.

Strategy:
1. Read the Ghidra translated output and extract all functions in our range
2. Apply C89 fixups to each function
3. Replace ALL stubs at once with Ghidra versions
4. Change file-scope declarations to use () params
5. Compile and fix errors iteratively
"""

import re
import shutil
import subprocess
import sys
import os
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = TOOLS_DIR.parent
sys.path.insert(0, str(TOOLS_DIR))

from c89_fixup import (
    parse_functions_raw,
    RE_WARNING_LINE, RE_PSQ_LINE, RE_DECL_IN_GQR, RE_DECL_UNAFF,
    RE_UNAFF_R, RE_IN_GQR, RE_DAT, RE_UDAT, RE_CLZ,
    RE_BOOL_CAST, RE_BOOL_TYPE, RE_TRUE, RE_FALSE,
    RE_CONCAT, RE_SUB, RE_SEXT, RE_ZEXT, RE_CHAR_VAR,
    RE_CODE_CAST, RE_CODE_PTR, RE_LABEL_BEFORE_BRACE, RE_WHILE_1,
    _replace_concat, _replace_sub, _replace_sext, _replace_zext,
    apply_type_replacements, apply_param_renaming
)

SRC_FILE = PROJECT_ROOT / "src" / "game" / "colosseum_script.c"
GHIDRA_TRANSLATED = PROJECT_ROOT / "build" / "ghidra_output" / "raw_decompilation_translated.c"

ADDR_MIN = 0x80212000
ADDR_MAX = 0x80240000
SDA_BASE_R13 = 0x80480820
SDA_BASE_R2 = 0x804836A0


def run_compile(source_file):
    cmd = [sys.executable, str(TOOLS_DIR / "compile_check.py"), str(source_file)]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120,
                                cwd=str(PROJECT_ROOT))
        return result.returncode == 0, result.stdout + result.stderr
    except Exception as e:
        return False, str(e)


def full_fixup(code, func_name, all_known_fns, all_known_lbls):
    """Apply all C89 fixups to a raw Ghidra function."""
    code = apply_type_replacements(code)
    code = apply_param_renaming(code)
    code = RE_WARNING_LINE.sub('', code)
    code = RE_PSQ_LINE.sub('', code)
    code = RE_DECL_IN_GQR.sub('', code)
    code = RE_DECL_UNAFF.sub('', code)

    unaff_regs = set(RE_UNAFF_R.findall(code))
    code = RE_IN_GQR.sub('0', code)

    sda_labels = {}
    for reg_num in unaff_regs:
        old = f'unaff_r{reg_num}'
        if reg_num in ('2', '13'):
            base = SDA_BASE_R2 if reg_num == '2' else SDA_BASE_R13
            for m in re.finditer(rf'{re.escape(old)}\s*\+\s*(-?\s*0x[0-9a-fA-F]+|-?\d+)', code):
                offset_str = m.group(1).replace(' ', '')
                if offset_str.startswith('-0x') or offset_str.startswith('-0X'):
                    offset = -int(offset_str[1:], 16)
                elif offset_str.startswith('0x') or offset_str.startswith('0X'):
                    offset = int(offset_str, 16)
                else:
                    offset = int(offset_str)
                addr = (base + offset) & 0xFFFFFFFF
                sda_labels[m.group(0)] = f'lbl_{addr:08X}'
            code = re.sub(rf'\b{re.escape(old)}\b(?!\s*\+)', f'0x{base:08X}', code)
        else:
            code = re.sub(rf'\b{re.escape(old)}\b', f'saved_r{reg_num}', code)

    for pat, repl in sorted(sda_labels.items(), key=lambda x: -len(x[0])):
        code = code.replace(pat, repl)

    code = RE_TRUE.sub('1', code)
    code = RE_FALSE.sub('0', code)
    code = RE_BOOL_CAST.sub('(u32)', code)
    code = RE_BOOL_TYPE.sub('u32', code)
    code = RE_CONCAT.sub(_replace_concat, code)
    code = RE_SUB.sub(_replace_sub, code)
    code = RE_SEXT.sub(_replace_sext, code)
    code = RE_ZEXT.sub(_replace_zext, code)
    code = RE_CLZ.sub('__cntlzw(', code)
    code = RE_CHAR_VAR.sub(r's8 \1', code)
    code = RE_CODE_CAST.sub('(void *)', code)
    code = RE_CODE_PTR.sub('void *', code)
    code = RE_LABEL_BEFORE_BRACE.sub(r'\1 (void)0;\2', code)
    code = RE_WHILE_1.sub('while (1)', code)

    # Fix char literal comparisons
    code = re.sub(r"'\\0'", '0', code)
    code = re.sub(r"'\\x([0-9a-fA-F]{2})'", lambda m: str(int(m.group(1), 16)), code)
    code = re.sub(r'\buint\b', 'u32', code)

    # Collect symbols for declarations
    dat_syms = set(RE_DAT.findall(code))
    udat_syms = set(RE_UDAT.findall(code))
    lbl_refs = set(re.findall(r'\b(lbl_[0-9a-fA-F]{8})\b', code))
    called_fns = set(re.findall(r'\b(fn_[0-9a-fA-F]{8})\s*\(', code))
    called_fns.discard(func_name)

    # Only add extern for functions NOT already known at file scope
    needs_fn_extern = sorted(called_fns - all_known_fns)
    needs_lbl_extern = sorted(lbl_refs - all_known_lbls)

    decls = []
    for sym in sorted(dat_syms):
        decls.append(f'    extern u32 {sym};')
    for sym in sorted(udat_syms):
        decls.append(f'    extern u32 {sym};')
    for fn in needs_fn_extern:
        ret_type = _infer_return_type(code, fn)
        decls.append(f'    extern {ret_type} {fn}();')
    for lbl in needs_lbl_extern:
        decls.append(f'    extern u8 {lbl}[];')
    for reg_num in sorted(unaff_regs, key=int):
        if reg_num not in ('2', '13'):
            decls.append(f'    u32 saved_r{reg_num} = 0;')

    if decls:
        brace_idx = code.find('{')
        if brace_idx >= 0:
            nl_after = code.find('\n', brace_idx)
            if nl_after >= 0:
                insert_pt = nl_after + 1
            else:
                insert_pt = brace_idx + 1
            code = code[:insert_pt] + '\n'.join(decls) + '\n' + code[insert_pt:]

    # Fix function pointer calls
    code = re.sub(r'\(\*(\w*[Vv]ar\d+)\)\s*\(', r'((void (*)())\1)(', code)
    code = re.sub(r'\(\*(r\d+)\)\s*\(', r'((void (*)())\1)(', code)

    code = re.sub(r'\n{3,}', '\n\n', code)
    return code


def _infer_return_type(code, fn_name):
    m = re.search(rf'(int|u32|u16|u8|s32|s16|s8|short|float|double|void\s*\*)\s+\w+\s*=\s*{re.escape(fn_name)}\s*\(', code)
    if m:
        return m.group(1).strip()
    m = re.search(rf'([a-z]+Var\d+)\s*=\s*{re.escape(fn_name)}\s*\(', code)
    if m:
        var = m.group(1)
        if var.startswith('i'): return 'int'
        elif var.startswith('u'): return 'u32'
        elif var.startswith('s'): return 'short'
        elif var.startswith('c'): return 's8'
        elif var.startswith('p'): return 'void *'
    if not re.search(rf'\w+\s*=\s*{re.escape(fn_name)}\s*\(', code):
        return 'void'
    return 'int'


def find_function_range(source_text, func_name):
    """Find the start and end of a function definition including comments/pragmas."""
    func_def_pattern = re.compile(
        rf'^(?:void|u32|int|s32|u8|u16|s16|s8|float|double|short|long)\s+{re.escape(func_name)}\s*\([^)]*\)\s*\{{',
        re.MULTILINE
    )
    match = func_def_pattern.search(source_text)
    if match is None:
        return None

    fn_def_start = match.start()
    text_before = source_text[:fn_def_start]
    lines_before = text_before.split('\n')

    start_line_idx = len(lines_before) - 1
    while start_line_idx > 0 and lines_before[start_line_idx].strip() == '':
        start_line_idx -= 1

    while start_line_idx > 0:
        prev_line = lines_before[start_line_idx].strip()
        if prev_line.startswith('/* =====') or prev_line.startswith('/* #####') or \
           prev_line.startswith('/* -----') or prev_line.startswith('/* ====') or \
           prev_line.startswith('/* ####') or prev_line.startswith('/* ----'):
            start_line_idx += 1
            break
        if prev_line.endswith('*/') and len(prev_line) > 60 and '=====' in prev_line:
            start_line_idx += 1
            break
        if prev_line == '#pragma optimization_level 0':
            start_line_idx -= 1
            continue
        if prev_line.startswith('#pragma optimization_level'):
            start_line_idx += 1
            break
        if prev_line.startswith('/* TODO:') and func_name in prev_line:
            start_line_idx -= 1
            continue
        if prev_line.startswith('/* Address:') or prev_line.startswith(f'/* 0x{func_name[3:]}'):
            start_line_idx -= 1
            continue
        addr_hex = func_name[3:]
        if prev_line.startswith('/*') and prev_line.endswith('*/') and addr_hex in prev_line:
            start_line_idx -= 1
            continue
        if prev_line.startswith('/*') and prev_line.endswith('*/') and len(prev_line) < 120:
            if '====' not in prev_line and '####' not in prev_line and '----' not in prev_line:
                start_line_idx -= 1
                continue
        if prev_line == '':
            start_line_idx -= 1
            continue
        start_line_idx += 1
        break

    if start_line_idx < 0:
        start_line_idx = 0
    while start_line_idx < len(lines_before) and lines_before[start_line_idx].strip() == '':
        start_line_idx += 1

    start_pos = sum(len(lines_before[i]) + 1 for i in range(start_line_idx))

    brace_depth = 0
    found_open = False
    end_pos = fn_def_start
    for i in range(fn_def_start, len(source_text)):
        ch = source_text[i]
        if ch == '{':
            brace_depth += 1
            found_open = True
        elif ch == '}':
            brace_depth -= 1
            if found_open and brace_depth == 0:
                end_pos = i + 1
                break

    rest = source_text[end_pos:]
    extra = 0
    for line in rest.split('\n')[:3]:
        stripped = line.strip()
        if stripped == '' or stripped.startswith('#pragma optimization_level'):
            extra += len(line) + 1
        else:
            break
    end_pos += extra

    return (start_pos, end_pos)


def main():
    print("=" * 70)
    print("BULK IMPORT: colosseum_script.c")
    print("=" * 70)

    # Step 1: Extract Ghidra functions
    print(f"Reading {GHIDRA_TRANSLATED}...")
    with open(GHIDRA_TRANSLATED, 'r') as f:
        ghidra_text = f.read()
    all_ghidra = parse_functions_raw(ghidra_text)
    print(f"Total Ghidra functions: {len(all_ghidra)}")

    ghidra_dict = {}
    for name, sz, code in all_ghidra:
        addr = int(name[3:], 16)
        if ADDR_MIN <= addr < ADDR_MAX:
            ghidra_dict[name] = (sz, code)
    print(f"In range: {len(ghidra_dict)}")

    # Step 2: Backup
    backup_path = str(SRC_FILE) + '.bulk_import.bak'
    shutil.copy2(str(SRC_FILE), backup_path)
    print(f"Backup: {backup_path}")

    # Step 3: Read source and fix file-scope declarations
    source = SRC_FILE.read_text()

    # Fix file-scope extern declarations to use ()
    source = re.sub(
        r'^(extern\s+\w[\w\s\*]*\s+fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*;',
        r'\1();',
        source,
        flags=re.MULTILINE
    )

    # Fix forward declarations to use ()
    source = re.sub(
        r'^((?:void|u32|int|s32|u8|u16|s16|s8|short|float|double|long)\s+fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*;',
        r'\1();',
        source,
        flags=re.MULTILINE
    )

    SRC_FILE.write_text(source)

    # Step 4: Collect all known symbols at file scope
    all_known_fns = set()
    for m in re.finditer(r'\b(fn_[0-9a-fA-F]{8})\b', source):
        all_known_fns.add(m.group(1))

    all_known_lbls = set()
    for m in re.finditer(r'^extern\s+\w+\s+(lbl_[0-9a-fA-F]{8})', source, re.MULTILINE):
        all_known_lbls.add(m.group(1))

    # Step 5: Process all functions, sorted by address
    fn_names = sorted(ghidra_dict.keys(), key=lambda n: int(n[3:], 16))
    print(f"\nProcessing {len(fn_names)} functions...")

    # Phase 1: Replace all functions at once
    # We process from bottom to top to maintain position stability
    source = SRC_FILE.read_text()

    replacements = {}
    for fn_name in fn_names:
        sz, raw = ghidra_dict[fn_name]
        fixed = full_fixup(raw, fn_name, all_known_fns, all_known_lbls)
        addr_str = fn_name[3:]
        new_block = f'/* Address: 0x{addr_str} | Size: 0x{sz} | Ghidra import */\n{fixed}\n'
        replacements[fn_name] = new_block

    # Find all function ranges first
    ranges = {}
    for fn_name in fn_names:
        rng = find_function_range(source, fn_name)
        if rng:
            ranges[fn_name] = rng

    print(f"Found {len(ranges)} functions to replace")

    # Sort by position (descending) for safe replacement
    sorted_fns = sorted(ranges.keys(), key=lambda n: ranges[n][0], reverse=True)

    for fn_name in sorted_fns:
        start, end = ranges[fn_name]
        source = source[:start] + replacements[fn_name] + source[end:]

    SRC_FILE.write_text(source)

    # Step 6: Compile
    print("\nCompiling full replacement...")
    ok, out = run_compile(str(SRC_FILE))

    if ok:
        print(f"SUCCESS: All {len(ranges)} functions compiled!")
        return

    print("Full replacement has errors. Starting individual rollback...")

    # Step 7: Identify errors and rollback failing functions
    # Parse error messages to find function names
    error_lines = set()
    for line in out.split('\n'):
        m = re.search(r'#\s+(\d+):', line)
        if m:
            error_lines.add(int(m.group(1)))

    if not error_lines:
        print("Could not parse error line numbers")
        print(out[:2000])
        return

    print(f"Error on {len(error_lines)} lines")

    # Find which Ghidra-imported functions contain those error lines
    source = SRC_FILE.read_text()
    source_lines = source.split('\n')

    failing_fns = set()
    for err_line in error_lines:
        if err_line > len(source_lines):
            continue
        # Search backward from error line to find the function
        for i in range(err_line - 1, max(0, err_line - 2000), -1):
            line = source_lines[i]
            m = re.match(r'/\* Address: 0x([0-9a-fA-F]{8}) \| Size: 0x\w+ \| Ghidra import \*/', line)
            if m:
                failing_fns.add(f'fn_{m.group(1)}')
                break

    print(f"Functions with errors: {len(failing_fns)}")

    # Rollback failing functions one by one
    original = Path(backup_path).read_text()
    # Fix declarations in original too
    original = re.sub(
        r'^(extern\s+\w[\w\s\*]*\s+fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*;',
        r'\1();',
        original,
        flags=re.MULTILINE
    )
    original = re.sub(
        r'^((?:void|u32|int|s32|u8|u16|s16|s8|short|float|double|long)\s+fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*;',
        r'\1();',
        original,
        flags=re.MULTILINE
    )

    rollback_count = 0
    for fn_name in failing_fns:
        # Get the original function code
        orig_range = find_function_range(original, fn_name)
        if not orig_range:
            continue
        orig_start, orig_end = orig_range
        orig_code = original[orig_start:orig_end]

        # Find current range in modified source
        source = SRC_FILE.read_text()
        curr_range = find_function_range(source, fn_name)
        if not curr_range:
            continue
        curr_start, curr_end = curr_range

        # Replace Ghidra import with original stub
        source = source[:curr_start] + orig_code + source[curr_end:]
        SRC_FILE.write_text(source)
        rollback_count += 1

    print(f"Rolled back {rollback_count} functions")

    # Step 8: Try compiling again
    ok, out = run_compile(str(SRC_FILE))
    if ok:
        successful = len(ranges) - rollback_count
        print(f"\nSUCCESS after rollback: {successful} functions imported, {rollback_count} rolled back")
    else:
        print(f"\nStill has errors after rollback. More investigation needed.")
        # Print first few errors
        for line in out.split('\n'):
            if 'Error:' in line or 'illegal' in line or 'does not match' in line:
                print(f"  {line.strip()[:120]}")

    # Step 9: Multi-pass retry for rolled-back functions
    # Some may now work because dependencies were updated
    pass_num = 0
    max_passes = 5
    while failing_fns and pass_num < max_passes:
        pass_num += 1
        source = SRC_FILE.read_text()
        all_known_fns_updated = set()
        for m in re.finditer(r'\b(fn_[0-9a-fA-F]{8})\b', source):
            all_known_fns_updated.add(m.group(1))
        all_known_lbls_updated = set()
        for m in re.finditer(r'^extern\s+\w+\s+(lbl_[0-9a-fA-F]{8})', source, re.MULTILINE):
            all_known_lbls_updated.add(m.group(1))

        retry_fns = list(failing_fns)
        newly_imported = []

        print(f"\n--- Retry pass {pass_num}: {len(retry_fns)} functions ---")

        for fn_name in sorted(retry_fns, key=lambda n: int(n[3:], 16)):
            if fn_name not in ghidra_dict:
                continue
            sz, raw = ghidra_dict[fn_name]
            source = SRC_FILE.read_text()

            curr_range = find_function_range(source, fn_name)
            if not curr_range:
                continue

            fixed = full_fixup(raw, fn_name, all_known_fns_updated, all_known_lbls_updated)
            addr_str = fn_name[3:]
            new_block = f'/* Address: 0x{addr_str} | Size: 0x{sz} | Ghidra import */\n{fixed}\n'

            start, end = curr_range
            new_source = source[:start] + new_block + source[end:]
            SRC_FILE.write_text(new_source)

            ok, out = run_compile(str(SRC_FILE))
            if ok:
                print(f"  {fn_name}: OK")
                newly_imported.append(fn_name)
                failing_fns.discard(fn_name)
            else:
                SRC_FILE.write_text(source)
                # Show first error
                for line in out.split('\n'):
                    if 'Error:' in line or 'illegal' in line:
                        print(f"  {fn_name}: {line.strip()[:100]}")
                        break

        if not newly_imported:
            print(f"  No progress in pass {pass_num}, stopping")
            break
        print(f"  Pass {pass_num}: imported {len(newly_imported)} more functions")

    # Final summary
    source = SRC_FILE.read_text()
    ghidra_count = len(re.findall(r'Ghidra import', source))

    print(f"\n{'='*70}")
    print(f"FINAL: {ghidra_count} Ghidra functions imported")
    print(f"Remaining failures: {len(failing_fns)}")
    if failing_fns:
        print("Failed functions:")
        for fn in sorted(failing_fns):
            print(f"  {fn}")
    print(f"{'='*70}")


if __name__ == '__main__':
    main()
