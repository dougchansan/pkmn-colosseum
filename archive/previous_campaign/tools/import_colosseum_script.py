#!/usr/bin/env python3
"""
import_colosseum_script.py - Import ALL Ghidra functions into colosseum_script.c.

Extracts functions from raw_decompilation_translated.c in the range
0x80212000-0x80240000, applies C89 fixups, and replaces stubs.
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
    parse_functions_raw, fixup_function,
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

# SDA bases
SDA_BASE_R13 = 0x80480820
SDA_BASE_R2 = 0x804836A0


def run_compile(source_file):
    """Compile and return (success, output)."""
    cmd = [sys.executable, str(TOOLS_DIR / "compile_check.py"), str(source_file)]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120,
                                cwd=str(PROJECT_ROOT))
        return result.returncode == 0, result.stdout + result.stderr
    except Exception as e:
        return False, str(e)


def extract_ghidra_functions():
    """Extract all functions in our range from the Ghidra translated output."""
    print(f"Reading {GHIDRA_TRANSLATED}...")
    with open(GHIDRA_TRANSLATED, 'r') as f:
        text = f.read()

    # Parse all functions using raw format parser
    all_fns = parse_functions_raw(text)
    print(f"Total functions in Ghidra output: {len(all_fns)}")

    # Filter to our address range
    in_range = []
    for name, size_hex, code in all_fns:
        addr = int(name[3:], 16)
        if ADDR_MIN <= addr < ADDR_MAX:
            in_range.append((name, size_hex, code))

    print(f"Functions in range 0x{ADDR_MIN:08X}-0x{ADDR_MAX:08X}: {len(in_range)}")
    return in_range


def full_fixup(code, func_name, source_text):
    """Apply all C89 fixups to a raw Ghidra function."""
    # Step 1: Type replacements (undefined4 -> u32, etc.)
    code = apply_type_replacements(code)

    # Step 2: Param renaming (param_1 -> r3, etc.)
    code = apply_param_renaming(code)

    # Step 3: Remove WARNING lines
    code = RE_WARNING_LINE.sub('', code)

    # Step 4: Remove __psq_ lines
    code = RE_PSQ_LINE.sub('', code)

    # Step 5: Remove in_GQR and unaff declarations
    code = RE_DECL_IN_GQR.sub('', code)
    code = RE_DECL_UNAFF.sub('', code)

    # Step 6: Collect unaff registers
    unaff_regs = set(RE_UNAFF_R.findall(code))

    # Step 7: Replace in_GQR with 0
    code = RE_IN_GQR.sub('0', code)

    # Step 8: Handle unaff registers - SDA resolution
    sda_labels = {}
    for reg_num in unaff_regs:
        old = f'unaff_r{reg_num}'
        if reg_num in ('2', '13'):
            base = SDA_BASE_R2 if reg_num == '2' else SDA_BASE_R13

            # Find all unaff_rN + offset patterns
            for m in re.finditer(rf'{re.escape(old)}\s*\+\s*(-?\s*0x[0-9a-fA-F]+|-?\d+)', code):
                offset_str = m.group(1).replace(' ', '')
                if offset_str.startswith('-0x') or offset_str.startswith('-0X'):
                    offset = -int(offset_str[1:], 16)
                elif offset_str.startswith('0x') or offset_str.startswith('0X'):
                    offset = int(offset_str, 16)
                else:
                    offset = int(offset_str)
                addr = (base + offset) & 0xFFFFFFFF
                label = f'lbl_{addr:08X}'
                full_match = m.group(0)
                sda_labels[full_match] = label

            # Replace standalone unaff_rN (without + offset)
            code = re.sub(rf'\b{re.escape(old)}\b(?!\s*\+)', f'0x{base:08X}', code)
        else:
            new = f'saved_r{reg_num}'
            code = re.sub(rf'\b{re.escape(old)}\b', new, code)

    # Apply SDA label substitutions (longest first)
    for pat, repl in sorted(sda_labels.items(), key=lambda x: -len(x[0])):
        code = code.replace(pat, repl)

    # Step 9: true/false/bool
    code = RE_TRUE.sub('1', code)
    code = RE_FALSE.sub('0', code)
    code = RE_BOOL_CAST.sub('(u32)', code)
    code = RE_BOOL_TYPE.sub('u32', code)

    # Step 10: Ghidra macros
    code = RE_CONCAT.sub(_replace_concat, code)
    code = RE_SUB.sub(_replace_sub, code)
    code = RE_SEXT.sub(_replace_sext, code)
    code = RE_ZEXT.sub(_replace_zext, code)

    # Step 11: countLeadingZeros -> __cntlzw
    code = RE_CLZ.sub('__cntlzw(', code)

    # Step 12: char vars -> s8
    code = RE_CHAR_VAR.sub(r's8 \1', code)

    # Step 13: code * -> void *
    code = RE_CODE_CAST.sub('(void *)', code)
    code = RE_CODE_PTR.sub('void *', code)

    # Step 14: Fix labels before closing braces
    code = RE_LABEL_BEFORE_BRACE.sub(r'\1 (void)0;\2', code)

    # Step 15: while( 1 ) -> while (1)
    code = RE_WHILE_1.sub('while (1)', code)

    # Step 16: Fix char literal comparisons -> integer comparisons
    # '\0' -> 0, '\x01' -> 1, '\x02' -> 2, etc.
    code = re.sub(r"'\\0'", '0', code)
    code = re.sub(r"'\\x([0-9a-fA-F]{2})'", lambda m: str(int(m.group(1), 16)), code)
    code = re.sub(r"'\\\\0'", '0', code)  # escaped backslash variant

    # Step 17: Fix Ghidra pointer types
    # (TYPE *)0x0 -> NULL (or just 0 for C89)
    code = re.sub(r'\(\w+\s*\*\s*\)0x0\b', '(void *)0', code)
    code = re.sub(r'\(\w+\s*\*\s*\)0\b(?!x)', '(void *)0', code)

    # Step 18: Fix 'uint' type remaining
    code = re.sub(r'\buint\b', 'u32', code)

    # Step 19: Collect DAT and lbl symbols for extern declarations
    dat_syms = set(RE_DAT.findall(code))
    udat_syms = set(RE_UDAT.findall(code))
    lbl_refs = set(re.findall(r'\b(lbl_[0-9a-fA-F]{8})\b', code))

    # Step 20: Find called functions needing extern declarations
    called_fns = set(re.findall(r'\b(fn_[0-9a-fA-F]{8})\s*\(', code))
    called_fns.discard(func_name)

    # Check which are already declared/defined at file scope
    # This includes both extern declarations and function definitions
    existing_file_scope = set()
    if source_text:
        # Match extern declarations
        for m in re.finditer(r'^extern\s+\w[\w\s\*]*\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
            existing_file_scope.add(m.group(1))
        # Match function definitions (return_type fn_XXX(...) {)
        for m in re.finditer(r'^(?:void|int|u32|u8|u16|s32|s16|s8|short|float|double|long)\s+(fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*\{', source_text, re.MULTILINE):
            existing_file_scope.add(m.group(1))
        # Match forward declarations (return_type fn_XXX(...);)
        for m in re.finditer(r'^(?:void|int|u32|u8|u16|s32|s16|s8|short|float|double|long)\s+(fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*;', source_text, re.MULTILINE):
            existing_file_scope.add(m.group(1))

    needs_fn_extern = sorted(called_fns - existing_file_scope)

    # Check existing file-scope lbl declarations
    existing_lbls = set()
    if source_text:
        for m in re.finditer(r'^extern\s+\w+\s+(lbl_[0-9a-fA-F]{8})', source_text, re.MULTILINE):
            existing_lbls.add(m.group(1))

    # Build declarations to insert
    decls = []

    for sym in sorted(dat_syms):
        decls.append(f'    extern u32 {sym};')
    for sym in sorted(udat_syms):
        decls.append(f'    extern u32 {sym};')

    for fn in needs_fn_extern:
        # Infer return type from usage
        ret_type = _infer_return_type(code, fn)
        decls.append(f'    extern {ret_type} {fn}();')

    for lbl in sorted(lbl_refs):
        if lbl not in existing_lbls:
            decls.append(f'    extern u8 {lbl}[];')

    # Non-SDA unaff register declarations
    for reg_num in sorted(unaff_regs, key=int):
        if reg_num not in ('2', '13'):
            decls.append(f'    u32 saved_r{reg_num} = 0;')

    # Insert declarations after opening brace
    if decls:
        brace_idx = code.find('{')
        if brace_idx >= 0:
            nl_after = code.find('\n', brace_idx)
            if nl_after >= 0:
                insert_pt = nl_after + 1
            else:
                insert_pt = brace_idx + 1
            decl_block = '\n'.join(decls) + '\n'
            code = code[:insert_pt] + decl_block + code[insert_pt:]

    # Step 21: Fix function pointer calls
    code = re.sub(
        r'\(\*(\w*[Vv]ar\d+)\)\s*\(',
        r'((void (*)())\1)(',
        code
    )
    code = re.sub(
        r'\(\*(r\d+)\)\s*\(',
        r'((void (*)())\1)(',
        code
    )

    # Step 22: Clean up blank lines
    code = re.sub(r'\n{3,}', '\n\n', code)

    return code


def _infer_return_type(code, fn_name):
    """Infer return type of a called function from usage context."""
    # Check assignment: typeVar = fn_XXX(...)
    m = re.search(rf'(int|u32|u16|u8|s32|s16|s8|short|float|double|void\s*\*)\s+\w+\s*=\s*{re.escape(fn_name)}\s*\(', code)
    if m:
        return m.group(1).strip()

    # Check variable prefix: iVar = fn -> int, uVar -> u32, sVar -> short, cVar -> s8
    m = re.search(rf'([a-z]+Var\d+)\s*=\s*{re.escape(fn_name)}\s*\(', code)
    if m:
        var = m.group(1)
        if var.startswith('i'):
            return 'int'
        elif var.startswith('u'):
            return 'u32'
        elif var.startswith('s'):
            return 'short'
        elif var.startswith('c'):
            return 's8'
        elif var.startswith('b'):
            return 'u32'
        elif var.startswith('p'):
            return 'void *'

    # Check cast: (type)fn_XXX(...)
    m = re.search(rf'\((\w+)\)\s*{re.escape(fn_name)}\s*\(', code)
    if m:
        t = m.group(1)
        type_map = {'int': 'int', 'u32': 'u32', 'u16': 'u16', 'u8': 'u8',
                     's32': 's32', 's16': 's16', 's8': 's8', 'short': 'short'}
        if t in type_map:
            return type_map[t]

    # No return value used -> could be void, but default to int to be safe
    # Check if fn is only called as a statement (no assignment)
    if not re.search(rf'\w+\s*=\s*{re.escape(fn_name)}\s*\(', code):
        return 'void'

    return 'int'


def find_function_in_source(source_text, func_name):
    """Find the start and end position of a function definition in source.

    Returns (start, end) or None if not found.
    The range includes the immediate comment/pragma above and trailing pragma.
    """
    # Pattern to match function definition line
    func_def_pattern = re.compile(
        rf'^(?:void|u32|int|s32|u8|u16|s16|s8|float|double|short|long)\s+{re.escape(func_name)}\s*\([^)]*\)\s*\{{',
        re.MULTILINE
    )

    match = func_def_pattern.search(source_text)
    if match is None:
        return None

    fn_def_start = match.start()

    # Split into lines up to the function def
    text_before = source_text[:fn_def_start]
    lines_before = text_before.split('\n')

    # Walk backwards from the function def to find the start of its block
    # Include: immediate comment (/* ... */), #pragma, TODO comment, Address comment
    # Stop at: another function's closing brace, a blank line after non-comment content,
    #          or a section header (/* ==== or /* #### or /* ---)
    start_line_idx = len(lines_before) - 1

    # Skip blank lines immediately before the function def
    while start_line_idx > 0 and lines_before[start_line_idx].strip() == '':
        start_line_idx -= 1

    # Now check for comment block and pragmas that belong to THIS function
    while start_line_idx > 0:
        prev_line = lines_before[start_line_idx].strip()
        # Stop at section headers
        if prev_line.startswith('/* =====') or prev_line.startswith('/* #####') or \
           prev_line.startswith('/* -----') or prev_line.startswith('/* ====') or \
           prev_line.startswith('/* ####') or prev_line.startswith('/* ----'):
            start_line_idx += 1
            break
        if prev_line.endswith('*/') and len(prev_line) > 60 and '=====' in prev_line:
            start_line_idx += 1
            break
        # A #pragma optimization_level 0 right before the function def => belongs to us
        if prev_line == '#pragma optimization_level 0':
            start_line_idx -= 1
            continue
        # A #pragma optimization_level 4 => belongs to PREVIOUS function, stop
        if prev_line.startswith('#pragma optimization_level'):
            start_line_idx += 1
            break
        # TODO comment about this function
        if prev_line.startswith('/* TODO:') and func_name in prev_line:
            start_line_idx -= 1
            continue
        # Address/size comment about this function
        if prev_line.startswith('/* Address:') or prev_line.startswith(f'/* 0x{func_name[3:]}'):
            start_line_idx -= 1
            continue
        # Short one-line comment that mentions our address
        addr_hex = func_name[3:]
        if prev_line.startswith('/*') and prev_line.endswith('*/') and addr_hex in prev_line:
            start_line_idx -= 1
            continue
        # Short generic one-line comment (pattern description, etc.)
        if prev_line.startswith('/*') and prev_line.endswith('*/') and len(prev_line) < 120:
            # Only include if this looks like a function-specific comment, not a section header
            if '====' not in prev_line and '####' not in prev_line and '----' not in prev_line:
                start_line_idx -= 1
                continue
        # Blank line
        if prev_line == '':
            start_line_idx -= 1
            continue
        # Hit something else (closing brace, another function, etc.)
        start_line_idx += 1
        break

    if start_line_idx < 0:
        start_line_idx = 0

    # Skip leading blank lines
    while start_line_idx < len(lines_before) and lines_before[start_line_idx].strip() == '':
        start_line_idx += 1

    # Calculate start position
    start_pos = sum(len(lines_before[i]) + 1 for i in range(start_line_idx))

    # Find the end - match balanced braces
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

    # Include trailing pragma and one blank line
    rest = source_text[end_pos:]
    extra = 0
    for line in rest.split('\n')[:3]:  # Only check next 3 lines
        stripped = line.strip()
        if stripped == '' or stripped.startswith('#pragma optimization_level'):
            extra += len(line) + 1
        else:
            break

    end_pos += extra

    return (start_pos, end_pos)


def main():
    print("=" * 70)
    print("IMPORTING GHIDRA FUNCTIONS INTO colosseum_script.c")
    print("=" * 70)

    # Step 1: Extract Ghidra functions
    ghidra_fns = extract_ghidra_functions()

    # Step 2: Build lookup dict
    ghidra_dict = {}
    for name, size_hex, code in ghidra_fns:
        ghidra_dict[name] = (size_hex, code)

    # Step 3: Read current source
    source_text = SRC_FILE.read_text()

    # Step 4: Find all existing function definitions in source
    existing_defs = re.findall(
        r'^(?:void|u32|int|s32|u8|u16|s16|s8|float|double|short|long)\s+(fn_[0-9a-fA-F]{8})\s*\([^)]*\)\s*\{',
        source_text, re.MULTILINE
    )
    print(f"Existing function definitions in source: {len(existing_defs)}")

    # Step 5: Filter to functions in our range that have Ghidra decompilations
    candidates = []
    for fn_name in existing_defs:
        addr = int(fn_name[3:], 16)
        if ADDR_MIN <= addr < ADDR_MAX and fn_name in ghidra_dict:
            candidates.append(fn_name)

    print(f"Functions with Ghidra decompilations available: {len(candidates)}")

    # Also find functions in Ghidra that aren't in the source yet
    source_fn_set = set(existing_defs)
    new_fns = []
    for name in ghidra_dict:
        addr = int(name[3:], 16)
        if ADDR_MIN <= addr < ADDR_MAX and name not in source_fn_set:
            new_fns.append(name)
    if new_fns:
        print(f"New functions not yet in source: {len(new_fns)}")

    # Step 6: Backup
    backup_path = str(SRC_FILE) + '.pre_import.bak'
    if not Path(backup_path).exists():
        shutil.copy2(str(SRC_FILE), backup_path)
        print(f"Backup saved to {backup_path}")

    # Step 7: Process functions in batches
    BATCH_SIZE = 10
    total_imported = 0
    total_failed = 0
    failed_list = []
    skipped_list = []

    # Sort candidates by address
    candidates.sort(key=lambda n: int(n[3:], 16))

    # Process in batches
    batch_num = 0
    i = 0
    while i < len(candidates):
        batch = candidates[i:i + BATCH_SIZE]
        batch_num += 1
        print(f"\n{'='*60}")
        print(f"BATCH {batch_num}: functions {i+1}-{min(i+BATCH_SIZE, len(candidates))} of {len(candidates)}")
        print(f"{'='*60}")

        # Read current source for this batch
        current_source = SRC_FILE.read_text()
        original_source = current_source

        batch_imported = 0
        batch_changes = []  # Track (fn_name, old_range, new_code) for revert

        for fn_name in batch:
            size_hex, raw_code = ghidra_dict[fn_name]

            # Find the function in current source
            fn_range = find_function_in_source(current_source, fn_name)
            if fn_range is None:
                print(f"  {fn_name}: NOT FOUND in source (skipping)")
                skipped_list.append(fn_name)
                continue

            start, end = fn_range
            old_code = current_source[start:end]

            # Check if it's already a real decompilation (not a stub)
            # Stubs typically have #pragma optimization_level 0, or { /* stub */ }
            # or are very short with just register assignments
            is_stub = (
                '#pragma optimization_level 0' in old_code or
                '/* stub */' in old_code or
                '/* TODO:' in old_code or
                ('param1 = 0x0' in old_code and '_ctx = _ctx' in old_code)  # register pseudo-code
            )

            # Also check if it's a simple return-constant function that's already correct
            is_simple_correct = re.match(
                r'^\s*/\*.*\*/\s*\n\s*(?:void|u32|int)\s+fn_[0-9a-fA-F]{8}\s*\([^)]*\)\s*\{\s*(?:return\s+\d+\s*;)?\s*\}\s*$',
                old_code, re.DOTALL
            )

            if is_simple_correct and not is_stub:
                print(f"  {fn_name}: already correct (skip)")
                skipped_list.append(fn_name)
                continue

            # Apply fixups to Ghidra code
            fixed_code = full_fixup(raw_code, fn_name, current_source)

            # Build the replacement block
            addr_str = fn_name[3:]
            new_block = f'/* Address: 0x{addr_str} | Size: 0x{size_hex} | Ghidra import */\n{fixed_code}\n'

            # Replace in source
            current_source = current_source[:start] + new_block + current_source[end:]
            batch_imported += 1

        if batch_imported == 0:
            print(f"  No changes in this batch")
            i += BATCH_SIZE
            continue

        # Write and compile
        SRC_FILE.write_text(current_source)
        print(f"\n  Compiling batch ({batch_imported} replacements)...")
        success, output = run_compile(str(SRC_FILE))

        if success:
            print(f"  BATCH {batch_num} OK - {batch_imported} functions compiled successfully")
            total_imported += batch_imported
        else:
            # Extract errors
            errors = []
            for line in output.split('\n'):
                if 'Error:' in line or 'error:' in line.lower():
                    errors.append(line.strip())

            print(f"  BATCH {batch_num} FAILED - reverting and trying one by one")
            if errors:
                for e in errors[:5]:
                    print(f"    {e[:120]}")

            # Revert
            SRC_FILE.write_text(original_source)

            # Try one by one
            for fn_name in batch:
                size_hex, raw_code = ghidra_dict[fn_name]
                current_source = SRC_FILE.read_text()

                fn_range = find_function_in_source(current_source, fn_name)
                if fn_range is None:
                    continue

                start, end = fn_range
                old_code = current_source[start:end]

                is_stub = (
                    '#pragma optimization_level 0' in old_code or
                    '/* stub */' in old_code or
                    '/* TODO:' in old_code or
                    ('param1 = 0x0' in old_code and '_ctx = _ctx' in old_code)
                )
                is_simple_correct = re.match(
                    r'^\s*/\*.*\*/\s*\n\s*(?:void|u32|int)\s+fn_[0-9a-fA-F]{8}\s*\([^)]*\)\s*\{\s*(?:return\s+\d+\s*;)?\s*\}\s*$',
                    old_code, re.DOTALL
                )
                if is_simple_correct and not is_stub:
                    continue

                fixed_code = full_fixup(raw_code, fn_name, current_source)
                addr_str = fn_name[3:]
                new_block = f'/* Address: 0x{addr_str} | Size: 0x{size_hex} | Ghidra import */\n{fixed_code}\n'

                new_source = current_source[:start] + new_block + current_source[end:]
                SRC_FILE.write_text(new_source)

                success, output = run_compile(str(SRC_FILE))
                if success:
                    print(f"    {fn_name}: OK")
                    total_imported += 1
                else:
                    errors = []
                    for line in output.split('\n'):
                        if 'Error:' in line or 'error:' in line.lower():
                            errors.append(line.strip())
                    err_msg = errors[0][:100] if errors else 'unknown'
                    print(f"    {fn_name}: FAILED ({err_msg})")
                    # Revert this one
                    SRC_FILE.write_text(current_source)
                    total_failed += 1
                    failed_list.append((fn_name, err_msg))

        i += BATCH_SIZE

    # Final summary
    print(f"\n{'='*70}")
    print(f"IMPORT COMPLETE")
    print(f"{'='*70}")
    print(f"  Imported: {total_imported}")
    print(f"  Failed:   {total_failed}")
    print(f"  Skipped:  {len(skipped_list)}")
    print(f"{'='*70}")

    if failed_list:
        print(f"\nFailed functions:")
        for name, err in failed_list:
            print(f"  {name}: {err}")

    return total_imported, total_failed, failed_list


if __name__ == '__main__':
    main()
