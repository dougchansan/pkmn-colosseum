#!/usr/bin/env python3
"""
Per-function wrapper for convert_gotos_v2.py.
Applies v2 converter to individual functions, compile-checks each,
and reverts on failure.
"""
import re
import sys
import os
import subprocess
import shutil
import tempfile

sys.path.insert(0, os.path.dirname(__file__))


def count_gotos(lines):
    return sum(len(re.findall(r'\bgoto\s+L_[0-9A-Fa-f]+\s*;', l)) for l in lines)


def find_functions(lines):
    """Find function boundaries."""
    funcs = []
    depth = 0
    func_start = None
    for i, line in enumerate(lines):
        if depth == 0 and re.match(
            r'^(void|u32|s32|u16|s16|u8|s8|f32|f64|int|void\s*\*|static\s+void|static\s+u32|static\s+s32|static\s+int|\w+\s*\*)\s+\w+\s*\(',
            line
        ):
            func_start = i
        opens = line.count('{')
        closes = line.count('}')
        if depth == 0 and opens > 0 and func_start is None:
            func_start = i
        depth += opens - closes
        if depth == 0 and func_start is not None:
            funcs.append((func_start, i))
            func_start = None
    return funcs


def compile_check(filepath):
    try:
        result = subprocess.run(
            [sys.executable, 'tools/compile_check.py', filepath],
            capture_output=True, text=True, timeout=120
        )
        return result.returncode == 0
    except Exception:
        return False


def apply_v2_to_function(func_lines):
    """Apply v2 converter to a single function's lines."""
    import convert_gotos_v2 as v2

    lines = list(func_lines)
    initial = count_gotos(lines)
    if initial == 0:
        return func_lines, 0

    prev = initial + 1
    for iteration in range(10):
        if count_gotos(lines) >= prev:
            break
        prev = count_gotos(lines)

        lp, rc, ls = v2.build_indices(lines)
        c = 0
        c += v2.convert_multi_condition_or(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_goto_to_return(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_return_value(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_validation_chain(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_cascaded_if_else(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_forward_singles_relaxed(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_flag_set_skip(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_multi_ref_forward_skip(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_multi_ref_while(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_do_while_relaxed(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_break_from_loop(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.convert_continue_in_loop(lines, lp, rc, ls)
        lp, rc, ls = v2.build_indices(lines)
        c += v2.remove_unreferenced_labels(lines, lp, rc, ls)
        if c == 0:
            break

    final = count_gotos(lines)
    content = '\n'.join(lines)
    content = re.sub(r'\n{4,}', '\n\n\n', content)
    new_lines = content.split('\n')
    return new_lines, initial - final


def process_file(filepath):
    total_eliminated = 0
    funcs_touched = 0
    failed_funcs = set()

    for attempt in range(100):
        with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
            good_content = f.read()

        lines = good_content.split('\n')
        functions = find_functions(lines)

        made_progress = False

        for start, end in functions:
            func_lines = lines[start:end+1]
            gc = count_gotos(func_lines)
            if gc == 0:
                continue

            name_m = re.match(r'^(?:void|u32|s32|u16|s16|u8|s8|f32|f64|int|void\s*\*|static\s+\w+)\s+(\w+)', func_lines[0])
            name = name_m.group(1) if name_m else '???'

            if name in failed_funcs:
                continue

            new_func_lines, elim = apply_v2_to_function(list(func_lines))

            if elim == 0:
                failed_funcs.add(name)
                continue

            # Check brace balance
            depth = 0
            for line in new_func_lines:
                depth += line.count('{') - line.count('}')
            if depth != 0:
                print(f'  {name}: SKIP - brace imbalance (depth={depth})')
                failed_funcs.add(name)
                continue

            # Apply and compile
            test_lines = list(lines)
            test_lines[start:end+1] = new_func_lines
            test_content = '\n'.join(test_lines)
            test_content = re.sub(r'\n{4,}', '\n\n\n', test_content)

            with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
                f.write(test_content)

            if compile_check(filepath):
                remain = gc - elim
                total_eliminated += elim
                funcs_touched += 1
                print(f'  {name}: -{elim}/{gc} ({remain} left) [OK]')
                made_progress = True
                break
            else:
                print(f'  {name}: REVERT - compile failed')
                with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
                    f.write(good_content)
                failed_funcs.add(name)

        if not made_progress:
            break

    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        final_content = f.read()
    final_gotos = count_gotos(final_content.split('\n'))
    print(f'\n  TOTAL: {total_eliminated} eliminated, {final_gotos} remaining ({funcs_touched} functions)')
    return total_eliminated


def main():
    if len(sys.argv) < 2:
        print("Usage: convert_gotos_v2_perfunc.py <file.c>")
        sys.exit(1)

    filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print(f'File not found: {filepath}')
        sys.exit(1)

    backup = filepath + '.v2pf.bak'
    shutil.copy2(filepath, backup)

    total = process_file(filepath)

    if total > 0:
        print(f'\nSuccess! {total} gotos eliminated.')
        if compile_check(filepath):
            print('Final compile: OK')
        else:
            print('Final compile: FAILED - reverting all')
            shutil.copy2(backup, filepath)
    else:
        print('\nNo gotos eliminated.')

    if os.path.exists(backup):
        os.remove(backup)


if __name__ == '__main__':
    main()
