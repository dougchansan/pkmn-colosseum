#!/usr/bin/env python3
"""
convert_colosseum_script.py - Convert pragma-guarded register-level C to idiomatic C89.

Strips pragma boilerplate (push/pop/optimizewithasm) from all functions.
Keeps #pragma optimization_level 0 for CW compiler stability.
Cleans up: synthetic stack frames (when safe), epilogue register restores,
and assembly prologue/epilogue comments.
Preserves all register variables and body code that may be needed.

Target compiler: CW GC/1.2.5n with -O4,p flags.
"""

import re
import sys
import os


def process_file(filepath):
    """Full conversion of colosseum_script.c pragma blocks.

    Uses a stack-based approach to properly handle nested pragma blocks.
    Inner blocks (optimization_level/optimizewithasm) get their functions cleaned.
    Outer blocks (force_active) get their push/pop stripped.
    """
    with open(filepath, 'r') as f:
        content = f.read()

    lines = content.split('\n')
    total_lines = len(lines)

    # =========================================================================
    # Step 1: Find all pragma push/pop blocks using stack-based nesting
    # =========================================================================
    stack = []
    all_blocks = []  # (start, end, depth) - depth 0 = outermost

    for i, line in enumerate(lines):
        if line.strip() == '#pragma push':
            stack.append(i)
        elif line.strip() == '#pragma pop':
            if stack:
                start = stack.pop()
                depth = len(stack)
                all_blocks.append((start, i, depth))

    # Classify blocks
    function_blocks = []  # Blocks containing function definitions (have optimization_level)
    wrapper_blocks = []   # Outer blocks (force_active, etc.)

    for start, end, depth in sorted(all_blocks):
        block_lines = lines[start:end + 1]
        has_opt_level = any('#pragma optimization_level' in l for l in block_lines[:5])
        has_force_active = any('#pragma force_active' in l for l in block_lines[:5])

        if has_opt_level:
            function_blocks.append((start, end))
        elif has_force_active:
            wrapper_blocks.append((start, end))
        else:
            wrapper_blocks.append((start, end))

    print(f"Found {len(function_blocks)} function pragma blocks, "
          f"{len(wrapper_blocks)} wrapper pragma blocks")

    # =========================================================================
    # Step 2: Process function blocks (convert register-level code)
    # =========================================================================
    # Sort by start line for ordered processing
    function_blocks.sort()

    # Build replacement map: line_idx -> replacement line (or None to delete)
    replacements = {}

    for start, end in function_blocks:
        converted = convert_block(lines, start, end)
        # Mark original lines for replacement
        for i in range(start, end + 1):
            replacements[i] = None  # Delete original
        # Store converted lines at the start position
        replacements[start] = converted

    # =========================================================================
    # Step 3: Process wrapper blocks (just strip push/pop/force_active)
    # =========================================================================
    for start, end in wrapper_blocks:
        # Only delete the push and pop lines themselves, plus force_active
        for i in range(start, min(start + 3, end)):
            s = lines[i].strip()
            if s == '#pragma push' or s.startswith('#pragma force_active'):
                if i not in replacements:
                    replacements[i] = None
        if end not in replacements:
            s = lines[end].strip()
            if s == '#pragma pop':
                replacements[end] = None

    # =========================================================================
    # Step 4: Build output
    # =========================================================================
    new_lines = []
    i = 0
    while i < total_lines:
        if i in replacements:
            val = replacements[i]
            if val is not None:
                # This is a converted block (list of lines)
                new_lines.extend(val)
            # else: line deleted
        else:
            new_lines.append(lines[i])
        i += 1

    # Fix u16 params that cause CW ICE
    result = '\n'.join(new_lines)
    result = result.replace('u16 sequenceId', 'u32 sequenceId')
    result = result.replace('u16 seqId', 'u32 seqId')

    # Final cleanup: remove any stray push/pop that might remain
    result_lines = result.split('\n')
    final_lines = []
    for line in result_lines:
        s = line.strip()
        if s == '#pragma pop' or s == '#pragma push':
            continue
        if s.startswith('#pragma force_active'):
            continue
        if s.startswith('#pragma optimizewithasm'):
            continue
        final_lines.append(line)

    with open(filepath, 'w') as f:
        f.write('\n'.join(final_lines))

    total_converted = len(function_blocks) + len(wrapper_blocks)
    print(f"Converted {total_converted} pragma blocks total.")
    return total_converted


def convert_block(lines, start, end):
    """Convert a single pragma block to cleaned idiomatic C.

    Strategy:
    1. Parse function signature, externs, declarations, body
    2. First pass over body to identify what variables are needed
    3. Emit #pragma optimization_level 0 + cleaned function

    Returns list of output lines (strings without newlines).
    """
    block = lines[start:end + 1]

    # === Parse the block ===
    func_sig = None
    func_sig_idx = None
    externs = []
    all_decl_lines = []  # ALL register/stack declarations
    raw_body_lines = []  # Everything after declarations
    has_stack = False
    stack_size = None
    has_r1 = False

    in_func_body = False
    past_decls = False

    for i, line in enumerate(block):
        s = line.strip()

        # Skip all pragma directives
        if s in ('#pragma push', '#pragma pop'):
            continue
        if s.startswith('#pragma optimizewithasm'):
            continue
        if s.startswith('#pragma optimization_level'):
            continue

        # Find function signature
        if not in_func_body and func_sig is None:
            m = re.match(
                r'^((?:static\s+)?(?:void|u32|s32|u16|u8|int|f32|f64)\s*\**\s*'
                r'fn_[0-9A-Fa-f]+\s*\([^)]*\))\s*\{',
                s
            )
            if m:
                func_sig = m.group(1)
                func_sig_idx = i
                in_func_body = True
                continue

        if not in_func_body:
            continue

        # Closing brace at end
        if s == '}' and i >= len(block) - 2:
            continue

        # Extern declarations
        if s.startswith('extern '):
            externs.append(s)
            continue

        # Stack frame declaration
        if re.match(r'^u8\s+sp\[\s*0x[0-9A-Fa-f]+\s*\];$', s):
            has_stack = True
            m2 = re.search(r'0x([0-9A-Fa-f]+)', s)
            if m2:
                stack_size = int(m2.group(1), 16)
            all_decl_lines.append(('stack', s))
            continue

        # r1 = (u32)sp
        if re.match(r'^u32\s+r1\s*=\s*\(u32\)sp;$', s):
            has_r1 = True
            all_decl_lines.append(('r1_assign', s))
            continue

        # Register variable declarations
        m = re.match(r'^(u32|s32|u16|u8|f32|f64)\s+(r\d+|f\d+)\s*=\s*(.+);$', s)
        if m and not past_decls:
            all_decl_lines.append(('reg', s, m.group(2)))
            continue

        # ctr_fn and ctr
        if re.match(r'^void\s+\(\*ctr_fn\)\(void\)\s*=\s*0;$', s) and not past_decls:
            all_decl_lines.append(('ctr_fn', s))
            continue
        if re.match(r'^u32\s+ctr\s*=\s*0;$', s) and not past_decls:
            all_decl_lines.append(('ctr', s))
            continue

        # Once we hit non-declaration code, we're past declarations
        past_decls = True
        raw_body_lines.append((i, line))

    if func_sig is None:
        # Can't parse -- strip pragmas only
        out = []
        for line in block:
            s = line.strip()
            if s in ('#pragma push', '#pragma pop'):
                continue
            if s.startswith('#pragma optimization_level') or s.startswith('#pragma optimizewithasm'):
                continue
            out.append(line)
        return out

    # === Determine which body lines to keep ===
    # Remove epilogue register restores and asm prologue/epilogue comments
    clean_body = []
    for idx, line in raw_body_lines:
        s = line.strip()

        # Skip assembly prologue/epilogue comments
        if re.match(r'^/\*\s*(stmw|lmw|stwu|lwz\s+r1|psq_st|psq_l)\b.*\*/\s*;?\s*$', s):
            continue

        # Skip epilogue register restores from stack
        if re.match(r'^r\d+\s*=\s*\*\(u32\*\)\(sp\s*\+\s*0x[0-9A-Fa-f]+\);$', s):
            continue

        # Skip epilogue float restores near end of function
        if re.match(r'^f\d+\s*=\s*\*\(f64\*\)\(sp\s*\+\s*0x[0-9A-Fa-f]+\);$', s):
            remaining = end - idx
            if remaining <= 10:
                continue

        clean_body.append(line)

    # === Determine which declarations are needed ===
    body_text = '\n'.join(l.strip() for l in clean_body)

    # Check if r1 or sp is referenced in body
    r1_used = bool(re.search(r'\br1\b', body_text))
    sp_used = bool(re.search(r'\bsp\b', body_text))

    # For each register, check if used in body
    needed_decls = []
    for decl_info in all_decl_lines:
        dtype = decl_info[0]

        if dtype == 'stack':
            # Keep sp declaration if sp is referenced in body
            if sp_used or r1_used:
                needed_decls.append(decl_info[1])
        elif dtype == 'r1_assign':
            # Keep r1 = (u32)sp if r1 is referenced in body
            if r1_used:
                needed_decls.append(decl_info[1])
        elif dtype == 'reg':
            varname = decl_info[2]
            if re.search(r'\b' + re.escape(varname) + r'\b', body_text):
                needed_decls.append(decl_info[1])
        elif dtype == 'ctr_fn':
            if 'ctr_fn' in body_text:
                needed_decls.append(decl_info[1])
        elif dtype == 'ctr':
            if re.search(r'\bctr\b', body_text):
                needed_decls.append(decl_info[1])

    # === Build output ===
    out = []
    out.append('#pragma optimization_level 0')
    out.append(func_sig + ' {')

    # Externs
    for ext in externs:
        out.append('    ' + ext)

    # Declarations
    for decl in needed_decls:
        out.append('    ' + decl)

    if externs or needed_decls:
        out.append('')

    # Body
    for line in clean_body:
        out.append(line)

    # Ensure closing brace
    if not out[-1].strip().endswith('}'):
        out.append('}')

    # Restore optimization level after function
    out.append('#pragma optimization_level 4')

    return out


def main():
    if len(sys.argv) < 2:
        print("Usage: python tools/convert_colosseum_script.py <source_file>")
        sys.exit(1)

    filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print(f"ERROR: File not found: {filepath}")
        sys.exit(1)

    count = process_file(filepath)
    print(f"\nDone. Processed {count} pragma blocks.")


if __name__ == '__main__':
    main()
