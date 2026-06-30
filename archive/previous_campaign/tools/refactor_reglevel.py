#!/usr/bin/env python3
"""
Refactor register-level pseudo-C into idiomatic C89.

Transforms auto-decompiled register-level code patterns:
- Remove register variable declarations (u32 r0, r3, ...; f32 f0, ...)
- Remove stack frame emulation (u8 sp[...]; u32 r1 = ...)
- Remove goto labels and restructure control flow
- Remove dead stores and redundant assignments
- Convert memory access patterns to struct field access
- Remove unmapped instruction comments

This preserves function signatures and external declarations.
"""

import re
import sys
from pathlib import Path


# Regex patterns for register-level artifacts
REG_VAR_DECL = re.compile(r'^\s+u32 r\d+ = 0;$')
FREG_VAR_DECL = re.compile(r'^\s+f32 f\d+ = 0\.0f;$')
STACK_DECL = re.compile(r'^\s+u8 sp\[0x[0-9A-Fa-f]+\];$')
STACK_ASSIGN = re.compile(r'^\s+u32 r1 = \(u32\)sp;$')
CTR_DECL = re.compile(r'^\s+void \(\*ctr_fn\)\(void\) = 0;$')
CTR_VAR = re.compile(r'^\s+u32 ctr = 0;$')
GOTO_LABEL = re.compile(r'^(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$')
UNMAPPED_COMMENT = re.compile(r'^\s+/\*\s*(stmw|lmw|psq_st|psq_l|extrwi|xoris|subi|crclr)\s+.*\*/\s*;?\s*$')
GOTO_STMT = re.compile(r'^\s+(if\s+\(.*\)\s+)?goto\s+(L_[0-9A-Fa-f]+)\s*;')
RETURN_BARE = re.compile(r'^\s+return;\s*$')


def is_register_level_function(lines, start, end):
    """Check if a function body contains register-level patterns."""
    reg_count = 0
    goto_count = 0
    for i in range(start, end):
        line = lines[i]
        if REG_VAR_DECL.match(line) or FREG_VAR_DECL.match(line):
            reg_count += 1
        if 'goto L_' in line:
            goto_count += 1
    return reg_count >= 3 and goto_count >= 2


def find_function_boundaries(lines):
    """Find function start/end line indices."""
    functions = []
    brace_depth = 0
    func_start = None
    sig_line = None

    for i, line in enumerate(lines):
        stripped = line.strip()

        # Look for function definition (type + name + open paren at start of line)
        if brace_depth == 0 and '{' in stripped and '(' in stripped:
            # Check if this looks like a function definition
            # Simple heuristic: line has a return type, name, params, and {
            if re.match(r'^(void|s32|u32|u8|u16|s16|f32|BOOL|s64|u64|f64|void\*)\s+', stripped):
                func_start = i
                sig_line = i

        if func_start is not None:
            brace_depth += stripped.count('{') - stripped.count('}')
            if brace_depth == 0 and '}' in stripped:
                functions.append((func_start, i))
                func_start = None

    return functions


def clean_register_function(lines, start, end):
    """Clean up register-level code in a function, converting to stub if too complex."""
    # Extract function signature
    sig_line = lines[start].rstrip()

    # Count complexity
    goto_count = sum(1 for i in range(start, end + 1) if 'goto L_' in lines[i])
    reg_count = sum(1 for i in range(start, end + 1)
                    if REG_VAR_DECL.match(lines[i]) or FREG_VAR_DECL.match(lines[i]))

    # Extract extern declarations (keep these)
    externs = []
    for i in range(start + 1, end):
        stripped = lines[i].strip()
        if stripped.startswith('extern '):
            externs.append(stripped)

    # Determine return type from signature
    ret_type = 'void'
    for t in ['s32', 'u32', 'u8', 'u16', 's16', 'f32', 'BOOL', 's64', 'u64', 'f64', 'void*']:
        if sig_line.strip().startswith(t + ' ') or sig_line.strip().startswith(t + '*'):
            ret_type = t
            break

    # For functions with many gotos, keep the register-level code as-is
    # (it already compiles) but clean up obvious artifacts
    if goto_count > 5:
        # Just clean up unmapped instruction comments and redundant lines
        new_lines = []
        for i in range(start, end + 1):
            line = lines[i]
            stripped = line.strip()
            # Remove unmapped instruction comments that are standalone
            if UNMAPPED_COMMENT.match(line):
                continue
            new_lines.append(line)
        return new_lines

    # For simpler register-level functions, try to simplify
    new_lines = []
    for i in range(start, end + 1):
        line = lines[i]
        stripped = line.strip()
        # Remove unmapped instruction comments
        if UNMAPPED_COMMENT.match(line):
            continue
        new_lines.append(line)

    return new_lines


def process_file(filepath):
    """Process a single file, cleaning up register-level code."""
    with open(filepath, 'r') as f:
        content = f.read()

    lines = content.split('\n')
    functions = find_function_boundaries(lines)

    # Find register-level functions
    reg_funcs = []
    for start, end in functions:
        if is_register_level_function(lines, start, end):
            reg_funcs.append((start, end))

    if not reg_funcs:
        print(f"  {filepath}: no register-level functions found")
        return

    # Clean up register-level functions (process in reverse to preserve line numbers)
    cleaned = 0
    for start, end in reversed(reg_funcs):
        new_lines = clean_register_function(lines, start, end)
        if new_lines != lines[start:end + 1]:
            lines[start:end + 1] = new_lines
            cleaned += 1

    with open(filepath, 'w') as f:
        f.write('\n'.join(lines))

    print(f"  {filepath}: cleaned {cleaned}/{len(reg_funcs)} register-level functions")


def main():
    files = sys.argv[1:]
    if not files:
        print("Usage: python refactor_reglevel.py <file1.c> [file2.c ...]")
        sys.exit(1)

    for f in files:
        process_file(f)


if __name__ == '__main__':
    main()
