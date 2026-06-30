#!/usr/bin/env python3
"""
import_battle.py - Import Ghidra functions into colosseum_battle.c stubs.

Imports ~84 trivial stubs (return 0) that have Ghidra decompilation available
in the address range 0x80240000-0x8025DC2C.

Adapted from import_all.py (colosseum_script.c) with same fixup logic.
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

SRC_FILE = PROJECT_ROOT / "src" / "game" / "colosseum_battle.c"
GHIDRA_TRANSLATED = PROJECT_ROOT / "build" / "ghidra_output" / "raw_decompilation_translated.c"

ADDR_MIN = 0x80240000
ADDR_MAX = 0x8025DC2C
SDA_BASE_R13 = 0x80480820
SDA_BASE_R2 = 0x804836A0


def run_compile():
    cmd = [sys.executable, str(TOOLS_DIR / "compile_check.py"), str(SRC_FILE), "-cv", "1.3"]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120,
                                cwd=str(PROJECT_ROOT))
        return result.returncode == 0, result.stdout + result.stderr
    except Exception as e:
        return False, str(e)


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


def full_fixup(code, func_name, source_text):
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
    code = re.sub(r"'\\0'", '0', code)
    code = re.sub(r"'\\x([0-9a-fA-F]{2})'", lambda m: str(int(m.group(1), 16)), code)
    code = re.sub(r'\buint\b', 'u32', code)

    # Fix Ghidra-specific identifiers
    code = re.sub(r'\bSUB_([0-9a-fA-F]{8})\b', r'fn_\1', code)
    code = re.sub(r'\bstack0x[0-9a-fA-F]+\b', '0', code)
    code = re.sub(r'&stack0x[0-9a-fA-F]+', '0', code)

    # Collect symbols
    dat_syms = set(RE_DAT.findall(code))
    udat_syms = set(RE_UDAT.findall(code))
    lbl_refs = set(re.findall(r'\b(lbl_[0-9a-fA-F]{8})\b', code))
    called_fns = set(re.findall(r'\b(fn_[0-9a-fA-F]{8})\s*\(', code))
    called_fns.discard(func_name)

    # Get all known symbols from source
    all_known_fns = set()
    for m2 in re.finditer(r'^extern\s+\w[\w\s\*]*\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
        all_known_fns.add(m2.group(1))
    for m2 in re.finditer(r'^(?:void|int|u32|u8|u16|s32|s16|s8|short|float|double|long)\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
        all_known_fns.add(m2.group(1))
    for m2 in re.finditer(r'^(?:void|int|u32|u8|u16|s32|s16|s8|short|float|double|long)\s+(fn_[0-9a-fA-F]{8})\s*\(\)\s*;', source_text, re.MULTILINE):
        all_known_fns.add(m2.group(1))

    all_known_lbls = set()
    for m in re.finditer(r'^extern\s+\w+\s+(lbl_[0-9a-fA-F]{8})', source_text, re.MULTILINE):
        all_known_lbls.add(m.group(1))

    file_scope_externs = set()
    if source_text:
        for m2 in re.finditer(r'^extern\s+\w[\w\s\*]*\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
            file_scope_externs.add(m2.group(1))

    addr_fns = set(re.findall(r'&(fn_[0-9a-fA-F]{8})\b', code))
    all_called_or_ref = called_fns | addr_fns
    needs_fn_extern = sorted(all_called_or_ref - file_scope_externs - {func_name})
    needs_lbl_extern = sorted(lbl_refs - all_known_lbls)

    # Build function return type map
    fn_return_types = {}
    if source_text:
        for m2 in re.finditer(r'^(extern\s+)?(void\s*\*|void|int|u32|u8|u16|s32|s16|s8|short|float|double|long)\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
            fn_return_types[m2.group(3)] = m2.group(2).strip()

    decls = []
    for sym in sorted(dat_syms):
        decls.append(f'    extern u32 {sym};')
    for sym in sorted(udat_syms):
        decls.append(f'    extern u32 {sym};')
    for fn in needs_fn_extern:
        if fn in fn_return_types:
            ret_type = fn_return_types[fn]
        else:
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

    code = re.sub(r'\(\*(\w*[Vv]ar\d+)\)\s*\(', r'((void (*)())\1)(', code)
    code = re.sub(r'\(\*(r\d+)\)\s*\(', r'((void (*)())\1)(', code)

    # Fix computed function pointer calls
    def fix_void_pp_call(code):
        result = code
        while True:
            m = re.search(r'\(\*\*\(void\s*\*\*\)', result)
            if not m:
                break
            start = m.start()
            cast_end = m.end()
            depth = 1
            pos = cast_end
            while pos < len(result) and depth > 0:
                if result[pos] == '(': depth += 1
                elif result[pos] == ')': depth -= 1
                pos += 1
            if depth != 0:
                break
            outer_close = pos - 1
            rest = result[outer_close+1:].lstrip()
            if rest.startswith('()'):
                expr = result[cast_end:outer_close]
                call_end = outer_close + 1 + (len(result[outer_close+1:]) - len(rest)) + 2
                replacement = f'((int (*)(void))**(void ***){expr})()'
                result = result[:start] + replacement + result[call_end:]
            else:
                break
        return result

    code = fix_void_pp_call(code)

    # Fix void*/integer conversion
    void_ptr_fns = set()
    if source_text:
        for m2 in re.finditer(r'^extern\s+void\s*\*\s*(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
            void_ptr_fns.add(m2.group(1))

    for vpfn in void_ptr_fns:
        code = re.sub(
            rf'(\b[iuscb]Var\d+\s*=\s*)({re.escape(vpfn)}\s*\()',
            rf'\1(int)\2',
            code
        )
        code = re.sub(
            rf'(\br\d+\s*=\s*)({re.escape(vpfn)}\s*\()',
            rf'\1(int)\2',
            code
        )

    # Fix null pointer comparisons
    code = re.sub(r'==\s*\(\w+\s*\*\s*\)0x0\b', '== (void *)0', code)
    code = re.sub(r'!=\s*\(\w+\s*\*\s*\)0x0\b', '!= (void *)0', code)
    code = re.sub(r'==\s*\(\w+\s*\*\s*\)0\b(?!x)', '== (void *)0', code)
    code = re.sub(r'!=\s*\(\w+\s*\*\s*\)0\b(?!x)', '!= (void *)0', code)

    code = re.sub(r',\s*0xffffffff\s*\)', ', (void*)0xffffffff)', code)

    # Fix void-returning functions used as values
    void_fns_in_source = set()
    if source_text:
        for m2 in re.finditer(r'^void\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
            void_fns_in_source.add(m2.group(1))
        for m2 in re.finditer(r'^\s+extern\s+void\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
            void_fns_in_source.add(m2.group(1))

    for vfn in sorted(void_fns_in_source):
        if re.search(rf'\w+\s*=\s*{re.escape(vfn)}\s*\(', code):
            code = re.sub(
                rf'(\s+extern\s+)void(\s+{re.escape(vfn)}\s*\(\))',
                r'\1int\2',
                code
            )

    code = re.sub(r'\n{3,}', '\n\n', code)

    # Convert function params to () with local variable declarations
    fn_sig_match = re.match(r'^(\w[\w\s\*]*\s+fn_[0-9a-fA-F]{8})\s*\(([^)]+)\)(\s*\n\n?\{)', code)
    if fn_sig_match:
        params = fn_sig_match.group(2).strip()
        if params != 'void' and params != '':
            ret_type_name = fn_sig_match.group(1)
            brace = fn_sig_match.group(3)
            param_parts = [p.strip() for p in params.split(',')]
            local_decls = []
            for p in param_parts:
                parts = p.rsplit(None, 1)
                if len(parts) == 2:
                    local_decls.append(f'    {parts[0]} {parts[1]};')

            new_sig = f'{ret_type_name}(){brace}'
            if local_decls:
                brace_pos = new_sig.rfind('{')
                decl_str = '\n'.join(local_decls) + '\n'
                new_sig = new_sig[:brace_pos+1] + '\n' + decl_str + new_sig[brace_pos+1:]

            code = new_sig + code[fn_sig_match.end():]

    return code


def find_function_range(source_text, func_name):
    """Find start/end of a function definition (including preceding comment)."""
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
            start_line_idx += 1; break
        if prev_line.endswith('*/') and len(prev_line) > 60 and '=====' in prev_line:
            start_line_idx += 1; break
        if prev_line == '#pragma optimization_level 0':
            start_line_idx -= 1; continue
        if prev_line.startswith('#pragma optimization_level'):
            start_line_idx += 1; break
        if prev_line.startswith('/* TODO:') and func_name in prev_line:
            start_line_idx -= 1; continue
        if prev_line.startswith('/* Address:') or prev_line.startswith(f'/* 0x{func_name[3:]}'):
            start_line_idx -= 1; continue
        addr_hex = func_name[3:]
        if prev_line.startswith('/*') and prev_line.endswith('*/') and addr_hex in prev_line:
            start_line_idx -= 1; continue
        if prev_line.startswith('/*') and prev_line.endswith('*/') and len(prev_line) < 120:
            if '====' not in prev_line and '####' not in prev_line and '----' not in prev_line:
                start_line_idx -= 1; continue
        if prev_line == '':
            start_line_idx -= 1; continue
        start_line_idx += 1; break

    if start_line_idx < 0: start_line_idx = 0
    while start_line_idx < len(lines_before) and lines_before[start_line_idx].strip() == '':
        start_line_idx += 1

    start_pos = sum(len(lines_before[i]) + 1 for i in range(start_line_idx))

    brace_depth = 0
    found_open = False
    end_pos = fn_def_start
    for i in range(fn_def_start, len(source_text)):
        if source_text[i] == '{':
            brace_depth += 1; found_open = True
        elif source_text[i] == '}':
            brace_depth -= 1
            if found_open and brace_depth == 0:
                end_pos = i + 1; break

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
    print("IMPORT: colosseum_battle.c stub replacement")
    print("=" * 70)

    # Extract Ghidra functions
    print(f"Reading Ghidra output...")
    with open(GHIDRA_TRANSLATED, 'r') as f:
        ghidra_text = f.read()
    all_ghidra = parse_functions_raw(ghidra_text)

    ghidra_dict = {}
    for name, sz, code in all_ghidra:
        addr = int(name[3:], 16)
        if ADDR_MIN <= addr < ADDR_MAX:
            ghidra_dict[name] = (sz, code)
    print(f"Ghidra functions in range: {len(ghidra_dict)}")

    # Find which functions are trivial stubs
    source = SRC_FILE.read_text()
    fn_pat = re.compile(
        r'^(?:void|u32|int|s32|u8|u16|s16|s8|float|double|short|long|void\s*\*)\s+(fn_([0-9a-fA-F]{8}))\s*\([^)]*\)\s*\{',
        re.MULTILINE
    )
    all_fns = list(fn_pat.finditer(source))

    target_names = []
    for m in all_fns:
        name = m.group(1)
        addr_val = int(m.group(2), 16)
        if not (ADDR_MIN <= addr_val < ADDR_MAX):
            continue
        if name not in ghidra_dict:
            continue

        brace_start = m.end() - 1
        depth = 1
        pos = brace_start + 1
        while pos < len(source) and depth > 0:
            if source[pos] == '{': depth += 1
            elif source[pos] == '}': depth -= 1
            pos += 1
        body = source[brace_start+1:pos-1].strip()
        body_lines = len([l for l in body.split('\n') if l.strip()])

        if body_lines <= 1:
            ghidra_size = int(ghidra_dict[name][0], 16)
            if ghidra_size > 8:  # Only import if Ghidra has real code
                target_names.append(name)

    print(f"Trivial stubs with Ghidra code: {len(target_names)}")

    # Backup
    backup = str(SRC_FILE) + '.battle_import.bak'
    shutil.copy2(str(SRC_FILE), backup)
    print(f"Backup saved to {backup}")

    # Import functions one by one
    fn_names = sorted(target_names, key=lambda n: int(n[3:], 16))
    imported = 0
    failed_list = []

    print(f"\nImporting {len(fn_names)} functions...")

    for i, fn_name in enumerate(fn_names):
        sz, raw = ghidra_dict[fn_name]

        # Re-read source each time
        source = SRC_FILE.read_text()

        # Find function in source
        rng = find_function_range(source, fn_name)
        if not rng:
            failed_list.append((fn_name, "not found in source"))
            continue

        start, end = rng
        old_code = source[start:end]

        # Apply fixups
        fixed = full_fixup(raw, fn_name, source)
        addr_str = fn_name[3:]
        new_block = f'/* Address: 0x{addr_str} | Size: 0x{sz} | Ghidra import */\n{fixed}\n'

        # Replace
        new_source = source[:start] + new_block + source[end:]
        SRC_FILE.write_text(new_source)

        # Compile
        ok, out = run_compile()
        if ok:
            imported += 1
            if (imported % 10) == 0 or i == len(fn_names) - 1:
                print(f"  [{i+1}/{len(fn_names)}] {fn_name}: OK ({imported} imported)")
        else:
            # Revert
            SRC_FILE.write_text(source)
            err = ''
            for line in out.split('\n'):
                if 'Error:' in line or 'illegal' in line or 'does not match' in line or 'redeclared' in line:
                    err = line.strip()[:120]
                    break
            failed_list.append((fn_name, err))

    print(f"\nPhase 1 complete: {imported} imported, {len(failed_list)} failed")

    # Phase 2: Retry failures
    if failed_list:
        max_passes = 3
        for pass_num in range(1, max_passes + 1):
            retry = list(failed_list)
            failed_list = []
            newly_imported = 0

            print(f"\nRetry pass {pass_num}: {len(retry)} functions...")

            for fn_name, _ in retry:
                sz, raw = ghidra_dict[fn_name]
                source = SRC_FILE.read_text()

                rng = find_function_range(source, fn_name)
                if not rng:
                    failed_list.append((fn_name, "not found"))
                    continue

                start, end = rng
                fixed = full_fixup(raw, fn_name, source)
                addr_str = fn_name[3:]
                new_block = f'/* Address: 0x{addr_str} | Size: 0x{sz} | Ghidra import */\n{fixed}\n'

                new_source = source[:start] + new_block + source[end:]
                SRC_FILE.write_text(new_source)

                ok, out = run_compile()
                if ok:
                    imported += 1
                    newly_imported += 1
                else:
                    SRC_FILE.write_text(source)
                    err = ''
                    for line in out.split('\n'):
                        if 'Error:' in line or 'illegal' in line or 'does not match' in line:
                            err = line.strip()[:120]
                            break
                    failed_list.append((fn_name, err))

            print(f"  Pass {pass_num}: {newly_imported} newly imported")
            if newly_imported == 0:
                break

    # Final summary
    print(f"\n{'='*70}")
    print(f"FINAL RESULTS")
    print(f"{'='*70}")
    print(f"  Total targets: {len(fn_names)}")
    print(f"  Imported: {imported}")
    print(f"  Failed: {len(failed_list)}")
    print(f"{'='*70}")

    if failed_list:
        print(f"\nFailed functions ({len(failed_list)}):")
        for fn, err in sorted(failed_list):
            print(f"  {fn}: {err}")

    # Verify final compilation
    ok, _ = run_compile()
    print(f"\nFinal compile: {'OK' if ok else 'FAILED'}")


if __name__ == '__main__':
    main()
