#!/usr/bin/env python3
"""
smart_import.py - Smartly import Ghidra functions into source files.

Handles:
- Proper extern declarations with correct return types
- C89 variable declaration ordering
- Conflict detection with existing declarations
- Individual compile testing per function
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
    fixup_function, parse_functions_processed,
    RE_WARNING_LINE, RE_PSQ_LINE, RE_DECL_IN_GQR, RE_DECL_UNAFF,
    RE_UNAFF_R, RE_IN_GQR, RE_DAT, RE_UDAT, RE_CLZ,
    RE_BOOL_CAST, RE_BOOL_TYPE, RE_TRUE, RE_FALSE,
    RE_CONCAT, RE_SUB, RE_SEXT, RE_ZEXT, RE_CHAR_VAR
)

SRC_DIR = PROJECT_ROOT / "src" / "game"
GHIDRA_DIR = PROJECT_ROOT / "build" / "ghidra_output" / "src" / "game"
BATTLE_SRC = SRC_DIR / "colosseum_battle.c"
BATTLE_GHIDRA = GHIDRA_DIR / "colosseum_battle.c"
BATTLE_CV = "1.3"


def run_compile(source_file: str, cv: str) -> tuple:
    """Compile and return (success, errors)."""
    cmd = [sys.executable, str(TOOLS_DIR / "compile_check.py"), source_file, "-cv", cv]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120, cwd=str(PROJECT_ROOT))
        return result.returncode == 0, result.stdout + result.stderr
    except Exception as e:
        return False, str(e)


def collect_declared_functions(source_text: str) -> set:
    """Collect all function names that are declared or defined in the source."""
    # Match function declarations: `extern type fn_XXX(...)` or `type fn_XXX(...)`
    # Also match forward declarations
    names = set()
    for m in re.finditer(r'\b(fn_[0-9a-fA-F]{8})\b', source_text):
        names.add(m.group(1))
    return names


def infer_return_type(func_code: str, called_name: str) -> str:
    """Infer the return type of a called function from how its result is used."""
    # Pattern: varType varName = fn_XXX(...)
    patterns = [
        (rf'(\w+)\s+\w+\s*=\s*{re.escape(called_name)}\s*\(', lambda m: m.group(1)),
        # Pattern: (type)fn_XXX(...)
        (rf'\((\w+)\)\s*{re.escape(called_name)}\s*\(', lambda m: m.group(1)),
    ]
    for pat, extractor in patterns:
        m = re.search(pat, func_code)
        if m:
            ret = extractor(m)
            # Map common var types to return types
            type_map = {
                'iVar1': 'int', 'iVar2': 'int', 'iVar3': 'int', 'iVar4': 'int',
                'iVar5': 'int', 'iVar6': 'int',
                'uVar1': 'u32', 'uVar2': 'u32', 'uVar3': 'u32', 'uVar4': 'u32',
                'uVar5': 'u32', 'uVar6': 'u32', 'uVar7': 'u32', 'uVar8': 'u32',
                'sVar1': 'short', 'sVar2': 'short', 'sVar3': 'short',
                'sVar4': 'short', 'sVar5': 'short',
                'cVar1': 's8', 'cVar2': 's8', 'cVar3': 's8',
                'cVar4': 's8', 'cVar5': 's8', 'cVar6': 's8',
                'cVar7': 's8', 'cVar8': 's8', 'cVar9': 's8',
                'bVar1': 'u32', 'bVar2': 'u32',
                'int': 'int', 'u32': 'u32', 'u16': 'u16', 'u8': 'u8',
                's32': 's32', 's16': 's16', 's8': 's8',
                'short': 'short', 'float': 'float', 'double': 'double',
            }
            if ret in type_map:
                return type_map[ret]
            return ret

    # Check assignment context
    # varName = fn_XXX(...) -- look for the var's type in declarations
    m = re.search(rf'(\w+)\s*=\s*{re.escape(called_name)}\s*\(', func_code)
    if m:
        var_name = m.group(1)
        # Find variable declaration
        decl = re.search(rf'^\s*(\w+)\s+{re.escape(var_name)}\s*[;=]', func_code, re.MULTILINE)
        if decl:
            return decl.group(1)

    return 'int'  # Default to int (most common)


def fix_c89_declarations(func_code: str) -> str:
    """
    Fix C89 declaration ordering: all declarations must be at the top of their block.
    This handles the case where Ghidra puts extern declarations before local vars.
    """
    lines = func_code.split('\n')
    result = []

    # Process the function body
    in_body = False
    brace_depth = 0
    decl_section = True

    for line in lines:
        stripped = line.strip()

        # Track brace depth
        open_braces = stripped.count('{')
        close_braces = stripped.count('}')

        if '{' in stripped and brace_depth == 0:
            in_body = True
            decl_section = True

        brace_depth += open_braces - close_braces
        result.append(line)

    return '\n'.join(result)


def collect_all_symbols(source_text: str) -> set:
    """Collect all symbol names (fn_, lbl_, DAT_, etc.) referenced in source."""
    symbols = set()
    for m in re.finditer(r'\b(fn_[0-9a-fA-F]{8}|lbl_[0-9a-fA-F]{8}|DAT_[0-9a-fA-F]{8}|_DAT_[0-9a-fA-F]{8})\b', source_text):
        symbols.add(m.group(1))
    return symbols


def fixup_function_smart(code: str, func_name: str, existing_fns: set,
                         existing_all_syms: set = None,
                         source_text: str = '') -> str:
    """
    Apply all C89 fixups and generate proper extern declarations.
    Only adds extern decls for functions not already declared in the source.
    Uses proper return types inferred from usage context.
    """
    if existing_all_syms is None:
        existing_all_syms = existing_fns
    # Step 1: Remove WARNING lines
    code = RE_WARNING_LINE.sub('', code)

    # Step 2: Remove __psq_ lines
    code = RE_PSQ_LINE.sub('', code)

    # Step 3: Remove in_GQR and unaff declarations
    code = RE_DECL_IN_GQR.sub('', code)
    code = RE_DECL_UNAFF.sub('', code)

    # Step 4: Collect unaff registers
    unaff_regs = set(RE_UNAFF_R.findall(code))

    # Step 5: Replace in_GQR with 0
    code = RE_IN_GQR.sub('0', code)

    # Step 6: Replace unaff registers
    # SDA bases: r13 = _SDA_BASE_ = 0x80480820, r2 = _SDA2_BASE_ = 0x804836A0
    SDA_BASE_R13 = 0x80480820
    SDA_BASE_R2 = 0x804836A0

    # Convert SDA-relative accesses to direct address labels
    sda_labels = {}
    for reg_num in unaff_regs:
        old = f'unaff_r{reg_num}'
        if reg_num in ('2', '13'):
            base = SDA_BASE_R2 if reg_num == '2' else SDA_BASE_R13

            # Find all patterns: unaff_rN + offset (including negative)
            # Pattern: unaff_rN + -0xXXXX or unaff_rN + 0xXXXX
            for m in re.finditer(rf'{re.escape(old)}\s*\+\s*(-?\s*0x[0-9a-fA-F]+|-?\d+)', code):
                offset_str = m.group(1).replace(' ', '')
                if offset_str.startswith('-0x'):
                    offset = -int(offset_str[1:], 16)
                elif offset_str.startswith('0x'):
                    offset = int(offset_str, 16)
                else:
                    offset = int(offset_str)
                addr = (base + offset) & 0xFFFFFFFF
                label = f'lbl_{addr:08X}'
                full_match = m.group(0)
                # Use lbl_XXX directly as u8* -- caller casts to proper type
                sda_labels[full_match] = f'lbl_{addr:08X}'

            # Replace standalone unaff_rN references (without + offset)
            code = re.sub(rf'\b{re.escape(old)}\b(?!\s*\+)', f'0x{base:08X}', code)
        else:
            new = f'saved_r{reg_num}'
            code = re.sub(rf'\b{re.escape(old)}\b', new, code)

    # Apply SDA label substitutions (longest patterns first to avoid partial matches)
    for pat, repl in sorted(sda_labels.items(), key=lambda x: -len(x[0])):
        code = code.replace(pat, repl)

    # Step 7: true/false/bool
    code = RE_TRUE.sub('1', code)
    code = RE_FALSE.sub('0', code)
    code = RE_BOOL_CAST.sub('(u32)', code)
    code = RE_BOOL_TYPE.sub('u32', code)

    # Step 8: Ghidra macros
    from c89_fixup import _replace_concat, _replace_sub, _replace_sext, _replace_zext
    code = RE_CONCAT.sub(_replace_concat, code)
    code = RE_SUB.sub(_replace_sub, code)
    code = RE_SEXT.sub(_replace_sext, code)
    code = RE_ZEXT.sub(_replace_zext, code)

    # Step 9: countLeadingZeros
    code = RE_CLZ.sub('__cntlzw(', code)

    # Step 10: char vars -> s8
    code = RE_CHAR_VAR.sub(r's8 \1', code)

    # Step 10b: Replace 'code *' function pointer type
    from c89_fixup import RE_CODE_CAST, RE_CODE_PTR, RE_LABEL_BEFORE_BRACE, RE_WHILE_1
    code = RE_CODE_CAST.sub('(void *)', code)
    code = RE_CODE_PTR.sub('void *', code)

    # Step 10c: Fix labels before closing braces
    code = RE_LABEL_BEFORE_BRACE.sub(r'\1 (void)0;\2', code)

    # Step 10d: Clean up while( 1 )
    code = RE_WHILE_1.sub('while (1)', code)

    # Step 11: Collect DAT symbols
    dat_syms = set(RE_DAT.findall(code))
    udat_syms = set(RE_UDAT.findall(code))

    # Step 12: Find called functions that need extern declarations
    called_fns = set(re.findall(r'\b(fn_[0-9a-fA-F]{8})\s*\(', code))
    called_fns.discard(func_name)

    # Only declare functions not already in the source
    needs_extern = sorted(called_fns - existing_fns)

    # Step 13: Build declarations
    decls = []

    # DAT externs
    for sym in sorted(dat_syms):
        if sym not in existing_fns:  # Reuse the set for any symbol
            decls.append(f'    extern u32 {sym};')
    for sym in sorted(udat_syms):
        decls.append(f'    extern u32 {sym};')

    # Function externs with inferred return types
    for fn in needs_extern:
        ret_type = infer_return_type(code, fn)
        decls.append(f'    extern {ret_type} {fn}();')

    # SDA label declarations (replace register asm approach)
    lbl_refs = set(re.findall(r'\blbl_([0-9a-fA-F]{8})\b', code))
    for lbl_addr in sorted(lbl_refs):
        lbl_name = f'lbl_{lbl_addr}'
        # Check if declared at file scope (non-indented extern line)
        # File-scope declarations would conflict if we re-declare with different type
        has_file_scope_decl = bool(re.search(
            rf'^extern\s+\w+\s+{re.escape(lbl_name)}\b',
            source_text, re.MULTILINE
        )) if source_text else False
        if not has_file_scope_decl:
            # Safe to add function-scoped extern declaration
            decls.append(f'    extern u8 {lbl_name}[];')

    # Non-SDA unaff register declarations
    for reg_num in sorted(unaff_regs, key=int):
        if reg_num not in ('2', '13'):
            decls.append(f'    u32 saved_r{reg_num} = 0;')

    # Step 14: Insert declarations after opening brace
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

    # Step 15: Post-processing fixes

    # 15a: Cast fn_8012640C returns to proper types
    # fn_8012640C returns void* but is often assigned to int/short/u32 vars
    code = re.sub(
        r'(\w+)\s*=\s*(fn_8012640C\s*\([^)]*\))',
        lambda m: f'{m.group(1)} = (int){m.group(2)}' if not m.group(0).startswith('(') else m.group(0),
        code
    )
    # More specifically: sVarN = fn_8012640C(...) -> sVarN = (short)(int)fn_8012640C(...)
    # And bVarN = fn_8012640C(...) -> bVarN = (u32)(int)fn_8012640C(...)

    # 15b: Cast function pointer calls - (*pcVar)(args) where pcVar is void*
    # Pattern: (*identifier)(args) where identifier is pXVar type
    code = re.sub(
        r'\(\*(\w*[Vv]ar\d+)\)\s*\(',
        r'((void (*)())\1)(',
        code
    )
    # Also handle (*r7)( pattern (parameter used as function pointer)
    code = re.sub(
        r'\(\*(r\d+)\)\s*\(',
        r'((void (*)())\1)(',
        code
    )

    # 15c: Cast array arguments to int* where needed
    # Pattern: fn_XXX(auStack_XX, ...) where auStack is u8[]
    # This is harder to fix generically, so let's just cast the SDA label usages
    # Pattern: fn_8025DD14(auStack_28) -> fn_8025DD14((int*)auStack_28)
    code = re.sub(
        r'(fn_\w+\s*\()(\s*auStack_\w+\s*[,)])',
        lambda m: m.group(1) + '(int*)' + m.group(2).lstrip(),
        code
    )
    # Also: fn_8025DE54(auStack_XX, (int)lbl_XXX, ...)
    code = re.sub(
        r'(fn_\w+\s*\()(\s*auStack_\w+\s*,)',
        lambda m: m.group(1) + '(int*)' + m.group(2).lstrip(),
        code
    )

    # 15d: Cast pointer assignments to u32
    # puVar[N] = piVarX -> puVar[N] = (u32)piVarX
    code = re.sub(
        r'(\w+\[\d+\])\s*=\s*(piVar\d+)',
        r'\1 = (u32)\2',
        code
    )

    # Step 16: Clean up blank lines
    code = re.sub(r'\n{3,}', '\n\n', code)

    return code


def main():
    print("=" * 70)
    print("SMART IMPORT: colosseum_battle.c")
    print("=" * 70)

    if not BATTLE_GHIDRA.exists():
        print(f"ERROR: {BATTLE_GHIDRA} not found")
        return

    # Parse Ghidra output
    ghidra_text = BATTLE_GHIDRA.read_text()
    all_functions = parse_functions_processed(ghidra_text)
    print(f"Ghidra functions available: {len(all_functions)}")

    # Filter: skip functions with PSQ/GQR (hardware intrinsics)
    # and functions with unaff_rN (non-SDA saved registers) for now
    simple_fns = []
    deferred_fns = []

    for name, size, code in all_functions:
        has_psq = '__psq_' in code
        has_gqr = 'in_GQR' in code
        has_unaff_other = bool(re.search(r'unaff_r(?!2\b|13\b)\d+', code))

        if has_psq or has_gqr or has_unaff_other:
            deferred_fns.append((name, size, code, 'PSQ' if has_psq else '',
                               'GQR' if has_gqr else '',
                               'UNAFF' if has_unaff_other else ''))
        else:
            simple_fns.append((name, size, code))

    print(f"Simple functions (importable now): {len(simple_fns)}")
    print(f"Complex functions (deferred): {len(deferred_fns)}")

    # Backup
    backup_path = str(BATTLE_SRC) + '.bak'
    if not Path(backup_path).exists():
        shutil.copy2(str(BATTLE_SRC), backup_path)

    # Collect existing declarations
    source_text = BATTLE_SRC.read_text()
    existing_fns = collect_declared_functions(source_text)
    print(f"Existing function references in source: {len(existing_fns)}")

    # Track stats
    imported = 0
    failed = 0
    failed_names = []

    # Import functions one at a time, testing compilation after each
    for i, (name, size, code) in enumerate(simple_fns):
        print(f"\n[{i+1}/{len(simple_fns)}] Importing {name} (0x{size})...")

        # Re-read source (previous import may have changed it)
        current_source = BATTLE_SRC.read_text()
        existing_fns = collect_declared_functions(current_source)

        # Apply smart fixups
        fixed = fixup_function_smart(code, name, existing_fns)

        # Build the import block
        addr = name[3:]
        import_block = f'\n/* Address: 0x{addr} | Size: 0x{size} | Ghidra import */\n{fixed}\n'

        # Append to source
        new_source = current_source.rstrip() + '\n' + import_block
        BATTLE_SRC.write_text(new_source)

        # Try compiling
        success, output = run_compile(str(BATTLE_SRC), BATTLE_CV)

        if success:
            print(f"  OK - compiled successfully")
            imported += 1
        else:
            # Extract first error
            errors = []
            for line in output.split('\n'):
                if 'Error:' in line or 'error:' in line.lower():
                    errors.append(line.strip())
            if errors:
                print(f"  FAILED: {errors[0][:100]}")
            else:
                print(f"  FAILED: unknown error")

            # Revert
            BATTLE_SRC.write_text(current_source)
            failed += 1
            failed_names.append((name, errors[0] if errors else 'unknown'))

    print(f"\n{'='*70}")
    print(f"RESULTS: {imported} imported, {failed} failed")
    print(f"{'='*70}")

    if failed_names:
        print("\nFailed functions:")
        for name, err in failed_names:
            print(f"  {name}: {err[:100]}")

    # Now try the SDA-only functions (no PSQ/GQR, no other unaff regs)
    sda_only = []
    for name, size, code, psq, gqr, unaff in deferred_fns:
        if not psq and not gqr and not unaff:
            sda_only.append((name, size, code))

    if sda_only:
        print(f"\n{'='*70}")
        print(f"PHASE 2: SDA-only functions ({len(sda_only)})")
        print(f"{'='*70}")

        for i, (name, size, code) in enumerate(sda_only):
            print(f"\n[{i+1}/{len(sda_only)}] Importing {name} (0x{size})...")

            current_source = BATTLE_SRC.read_text()
            existing_fns = collect_declared_functions(current_source)

            fixed = fixup_function_smart(code, name, existing_fns)

            addr = name[3:]
            import_block = f'\n/* Address: 0x{addr} | Size: 0x{size} | Ghidra import */\n{fixed}\n'

            new_source = current_source.rstrip() + '\n' + import_block
            BATTLE_SRC.write_text(new_source)

            success, output = run_compile(str(BATTLE_SRC), BATTLE_CV)

            if success:
                print(f"  OK - compiled successfully")
                imported += 1
            else:
                errors = []
                for line in output.split('\n'):
                    if 'Error:' in line or 'error:' in line.lower():
                        errors.append(line.strip())
                if errors:
                    print(f"  FAILED: {errors[0][:100]}")
                else:
                    print(f"  FAILED: unknown error")
                BATTLE_SRC.write_text(current_source)
                failed += 1
                failed_names.append((name, errors[0] if errors else 'unknown'))

    print(f"\n{'='*70}")
    print(f"FINAL RESULTS: {imported} imported, {failed} failed out of {len(all_functions)} total")
    print(f"{'='*70}")


if __name__ == '__main__':
    main()
