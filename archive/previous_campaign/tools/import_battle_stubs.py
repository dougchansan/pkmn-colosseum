#!/usr/bin/env python3
"""
import_battle_stubs.py - Import remaining single-line stubs in colosseum_battle.c.

Handles the special case where stubs are on a single line like:
  u32 fn_XXXX(void* ctx, u32 param) { return 0; /* stub */ }
"""

import re
import shutil
import subprocess
import sys
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


def full_fixup_for_stub(raw_code, func_name, source_text, stub_return_type):
    """Apply all C89 fixups with proper declaration ordering."""
    code = apply_type_replacements(raw_code)
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
    code = re.sub(r'\bSUB_([0-9a-fA-F]{8})\b', r'fn_\1', code)
    code = re.sub(r'\bstack0x[0-9a-fA-F]+\b', '0', code)
    code = re.sub(r'&stack0x[0-9a-fA-F]+', '0', code)

    # Extract function signature and body
    sig_match = re.match(r'^(\w[\w\s\*]*?)\s+(fn_[0-9a-fA-F]{8})\s*\(([^)]*)\)\s*\n\n?\{', code)
    if not sig_match:
        sig_match = re.match(r'^(\w[\w\s\*]*?)\s+(fn_[0-9a-fA-F]{8})\s*\(([^)]*)\)\s*\{', code)

    if not sig_match:
        return None  # Can't parse

    ghidra_ret_type = sig_match.group(1).strip()
    params_str = sig_match.group(3).strip()

    # Find body
    brace_idx = code.find('{')
    body_start = brace_idx + 1
    depth = 1
    pos = body_start
    while pos < len(code) and depth > 0:
        if code[pos] == '{': depth += 1
        elif code[pos] == '}': depth -= 1
        pos += 1
    body = code[body_start:pos-1]

    # Parse body into declarations and statements
    body_lines = body.split('\n')
    decl_lines = []  # Variable declarations from Ghidra body
    stmt_lines = []  # Actual statements

    for line in body_lines:
        stripped = line.strip()
        if not stripped:
            continue
        # Check if this is a variable declaration (C89 style)
        if re.match(r'^(int|u32|u16|u8|s32|s16|s8|short|float|double|long|u64|s64|void\s*\*)\s+\w+', stripped):
            # It's a local var declaration
            decl_lines.append(f'    {stripped}')
        else:
            stmt_lines.append(line)

    # Build parameter declarations
    param_decls = []
    if params_str and params_str != 'void':
        for p in params_str.split(','):
            p = p.strip()
            parts = p.rsplit(None, 1)
            if len(parts) == 2:
                param_decls.append(f'    {parts[0]} {parts[1]};')

    # Collect extern symbols needed
    full_body = '\n'.join(stmt_lines)
    dat_syms = set(RE_DAT.findall(full_body))
    udat_syms = set(RE_UDAT.findall(full_body))
    lbl_refs = set(re.findall(r'\b(lbl_[0-9a-fA-F]{8})\b', full_body))
    called_fns = set(re.findall(r'\b(fn_[0-9a-fA-F]{8})\s*\(', full_body))
    called_fns.discard(func_name)
    addr_fns = set(re.findall(r'&(fn_[0-9a-fA-F]{8})\b', full_body))
    all_called = called_fns | addr_fns

    # We need local extern declarations with () for ALL called functions,
    # even those declared at file scope with typed parameters.
    # This overrides the file-scope prototypes and allows calling with
    # any number/type of arguments (C89 unspecified params).

    all_known_lbls = set()
    for m2 in re.finditer(r'^extern\s+\w+\s+(lbl_[0-9a-fA-F]{8})', source_text, re.MULTILINE):
        all_known_lbls.add(m2.group(1))

    fn_return_types = {}
    for m2 in re.finditer(r'^(extern\s+)?(void\s*\*|void|int|u32|u8|u16|s32|s16|s8|short|float|double|long)\s+(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
        fn_return_types[m2.group(3)] = m2.group(2).strip()

    # ALL called/referenced functions need local extern with ()
    needs_fn_extern = sorted(all_called - {func_name})
    needs_lbl_extern = sorted(lbl_refs - all_known_lbls)

    extern_decls = []
    for sym in sorted(dat_syms):
        extern_decls.append(f'    extern u32 {sym};')
    for sym in sorted(udat_syms):
        extern_decls.append(f'    extern u32 {sym};')
    for fn in needs_fn_extern:
        if fn in fn_return_types:
            ret_type = fn_return_types[fn]
        else:
            ret_type = _infer_return_type(full_body, fn)
        extern_decls.append(f'    extern {ret_type} {fn}();')
    for lbl in needs_lbl_extern:
        extern_decls.append(f'    extern u8 {lbl}[];')
    for reg_num in sorted(unaff_regs, key=int):
        if reg_num not in ('2', '13'):
            extern_decls.append(f'    u32 saved_r{reg_num} = 0;')

    # Identify void* returning functions that need casts
    void_ptr_fns = set()
    for m2 in re.finditer(r'^extern\s+void\s*\*\s*(fn_[0-9a-fA-F]{8})\s*\(', source_text, re.MULTILINE):
        void_ptr_fns.add(m2.group(1))

    # Fix statements: cast void* returns to int where needed
    fixed_stmts = []
    for line in stmt_lines:
        fixed_line = line
        for vpfn in void_ptr_fns:
            # iVar = fn_XXX(...) -> iVar = (int)fn_XXX(...)
            fixed_line = re.sub(
                rf'(\b[iuscb]Var\d+\s*=\s*)({re.escape(vpfn)}\s*\()',
                rf'\1(int)\2',
                fixed_line
            )
            fixed_line = re.sub(
                rf'(\br\d+\s*=\s*)({re.escape(vpfn)}\s*\()',
                rf'\1(int)\2',
                fixed_line
            )
        # Fix function pointer calls
        fixed_line = re.sub(r'\(\*(\w*[Vv]ar\d+)\)\s*\(', r'((void (*)())\1)(', fixed_line)
        fixed_line = re.sub(r'\(\*(r\d+)\)\s*\(', r'((void (*)())\1)(', fixed_line)
        # Fix null pointer comparisons
        fixed_line = re.sub(r'==\s*\(\w+\s*\*\s*\)0x0\b', '== (void *)0', fixed_line)
        fixed_line = re.sub(r'!=\s*\(\w+\s*\*\s*\)0x0\b', '!= (void *)0', fixed_line)
        fixed_stmts.append(fixed_line)

    # Fix return statements for type mismatch
    # If stub says u32 but Ghidra says void, change 'return;' to 'return 0;'
    if stub_return_type in ('u32', 'int', 's32', 'u16', 'u8', 's16', 's8', 'short', 'float', 'double'):
        fixed_stmts = [re.sub(r'\breturn\s*;', 'return 0;', s) for s in fixed_stmts]

    # Build the function: all declarations first (C89), then statements
    all_decl_lines = param_decls + extern_decls + decl_lines
    decl_block = '\n'.join(all_decl_lines) + '\n' if all_decl_lines else ''
    stmt_block = '\n'.join(fixed_stmts)

    # Ensure stmt_block lines have proper indentation
    formatted_stmts = []
    for line in fixed_stmts:
        s = line.rstrip()
        if s:
            # Ensure at least 2-space indent for body lines
            if not s.startswith(' ') and not s.startswith('}'):
                s = '    ' + s
            formatted_stmts.append(s)
        # Keep blank lines between blocks for readability

    stmt_block = '\n'.join(formatted_stmts)
    if stmt_block and not stmt_block.endswith('\n'):
        stmt_block += '\n'

    new_code = f'{stub_return_type} {func_name}()\n{{\n{decl_block}{stmt_block}}}\n'

    # Clean up blank lines
    new_code = re.sub(r'\n{3,}', '\n\n', new_code)

    return new_code


def main():
    print("=" * 70)
    print("IMPORT: colosseum_battle.c remaining single-line stubs")
    print("=" * 70)

    # Load Ghidra functions
    with open(GHIDRA_TRANSLATED, 'r') as f:
        ghidra_text = f.read()
    all_ghidra = parse_functions_raw(ghidra_text)
    ghidra_dict = {}
    for name, sz, code in all_ghidra:
        addr = int(name[3:], 16)
        if ADDR_MIN <= addr < ADDR_MAX:
            ghidra_dict[name] = (sz, code)

    # Backup
    backup = str(SRC_FILE) + '.stubs_import2.bak'
    shutil.copy2(str(SRC_FILE), backup)

    source = SRC_FILE.read_text()

    # Find single-line stubs
    stub_pattern = re.compile(
        r'^(/\*[^\n]*\*/\n)'
        r'((?:void\s*\*|void|u32|int|s32|u8|u16|s16|s8|float|double|short|long)\s+'
        r'(fn_([0-9a-fA-F]{8}))\s*\([^)]*\)\s*\{[^}]*\})',
        re.MULTILINE
    )

    targets = []
    for m in stub_pattern.finditer(source):
        comment = m.group(1)
        full_line = m.group(2)
        fn_name = m.group(3)
        addr = int(m.group(4), 16)

        if not (ADDR_MIN <= addr < ADDR_MAX):
            continue
        if fn_name not in ghidra_dict:
            continue

        brace_start = full_line.find('{')
        brace_end = full_line.rfind('}')
        body = full_line[brace_start+1:brace_end].strip()
        if body not in ('return 0;', 'return 0; /* stub */', ''):
            continue

        ret_match = re.match(r'(void\s*\*|void|u32|int|s32|u8|u16|s16|s8|float|double|short|long)', full_line)
        stub_ret_type = ret_match.group(1) if ret_match else 'u32'

        ghidra_size = int(ghidra_dict[fn_name][0], 16)
        if ghidra_size <= 8:
            continue

        targets.append((fn_name, stub_ret_type))

    print(f"Found {len(targets)} single-line stubs with Ghidra code")

    imported = 0
    failed_list = []

    for fn_name, stub_ret_type in sorted(targets, key=lambda x: int(x[0][3:], 16)):
        sz, raw = ghidra_dict[fn_name]
        source = SRC_FILE.read_text()

        fixed = full_fixup_for_stub(raw, fn_name, source, stub_ret_type)
        if fixed is None:
            failed_list.append((fn_name, "could not parse Ghidra code"))
            continue

        addr_str = fn_name[3:]
        new_block = f'/* Address: 0x{addr_str} | Size: 0x{sz} | Ghidra import */\n{fixed}'

        # Find the stub
        stub_pat = re.compile(
            rf'^(/\*[^\n]*{re.escape(fn_name[3:])}[^\n]*\*/\n)'
            rf'((?:void\s*\*|void|u32|int|s32|u8|u16|s16|s8|float|double|short|long)\s+'
            rf'{re.escape(fn_name)}\s*\([^)]*\)\s*\{{[^}}]*\}})',
            re.MULTILINE
        )
        m = stub_pat.search(source)
        if not m:
            failed_list.append((fn_name, "stub not found"))
            continue

        new_source = source[:m.start()] + new_block + source[m.end():]
        SRC_FILE.write_text(new_source)

        ok, out = run_compile()
        if ok:
            imported += 1
            print(f"  {fn_name}: OK")
        else:
            SRC_FILE.write_text(source)
            err = ''
            for line in out.split('\n'):
                if 'Error' in line:
                    err = line.strip()[:120]
                    break
                if 'illegal' in line.lower() or 'does not match' in line.lower() or 'redeclared' in line.lower():
                    err = line.strip()[:120]
                    break
            if not err:
                # Get more context
                for line in out.split('\n'):
                    if line.strip() and not line.startswith('#'):
                        err = line.strip()[:120]
                        break
            failed_list.append((fn_name, err))
            print(f"  {fn_name}: FAILED - {err[:100]}")

    print(f"\n{'='*70}")
    print(f"RESULTS: {imported} imported, {len(failed_list)} failed")
    print(f"{'='*70}")

    if failed_list:
        print(f"\nFailed ({len(failed_list)}):")
        for fn, err in sorted(failed_list):
            print(f"  {fn}: {err}")

    ok, _ = run_compile()
    print(f"\nFinal compile: {'OK' if ok else 'FAILED'}")


if __name__ == '__main__':
    main()
