#!/usr/bin/env python3
"""
fix_sda_globals.py - Fix Ghidra-imported SDA global access patterns.

Ghidra declares SDA globals as `extern u8 lbl_XXXXXXXX[]` which causes
the compiler to emit a double-dereference (load address, then load value)
instead of a single SDA21 load (direct offset from r2/r13).

This script:
1. Finds all `extern u8 lbl_XXXXXXXX[]` declarations inside function bodies
2. Analyzes how each label is used in the function
3. Determines the correct type (u32, f32, u16, u8, etc.)
4. Rewrites declarations and access patterns

Only labels in SDA/SDA2 ranges (0x8047xxxx) are fixed. Labels in rodata
ranges (0x8027xxxx, 0x8037xxxx, 0x8039xxxx) that are used as addresses
are left as arrays.
"""

import re
import sys
from pathlib import Path


def is_sda_address(label: str) -> bool:
    """Check if a label address falls in SDA/SDA2 range."""
    m = re.match(r'lbl_([0-9A-Fa-f]{8})', label)
    if not m:
        return False
    addr = int(m.group(1), 16)
    return 0x80470000 <= addr < 0x80480000


def find_function_boundaries(lines: list) -> list:
    """Find function body boundaries."""
    functions = []
    brace_depth = 0
    func_start = None
    for i, line in enumerate(lines):
        # Skip string literals for brace counting
        in_string = False
        in_char = False
        for ch in line:
            if ch == '"' and not in_char:
                in_string = not in_string
            elif ch == "'" and not in_string:
                in_char = not in_char
            elif not in_string and not in_char:
                if ch == '{':
                    if brace_depth == 0:
                        func_start = i
                    brace_depth += 1
                elif ch == '}':
                    brace_depth -= 1
                    if brace_depth == 0 and func_start is not None:
                        functions.append((func_start, i + 1))
                        func_start = None
    return functions


def apply_line_fixes(line: str, label: str, new_type: str) -> str:
    """Apply all necessary pattern fixes to a single line for a given label.

    This function applies regex replacements in priority order to handle
    all access patterns for the label.
    """
    le = re.escape(label)
    out = line

    # --- Double dereferences (pointer-valued globals) ---

    # **(u32 **)(lbl_XXX)  ->  *(u32*)lbl_XXX
    out = re.sub(
        rf'\*\s*\*\s*\(\s*u32\s*\*\s*\*\s*\)\s*\(\s*{le}\s*\)',
        f'*(u32*){label}', out)
    out = re.sub(
        rf'\*\s*\*\s*\(\s*u32\s*\*\s*\*\s*\)\s*{le}(?!\w)',
        f'*(u32*){label}', out)

    # **(u8 **)(u32)(lbl_XXX) or **(u8 **)(lbl_XXX)  ->  *(u8*)lbl_XXX
    out = re.sub(
        rf'\*\s*\*\s*\(\s*u8\s*\*\s*\*\s*\)\s*(?:\(\s*u32\s*\)\s*)?\(\s*{le}\s*\)',
        f'*(u8*){label}', out)

    # Store: *(u8 **)(u32)(lbl_XXX) = value -> lbl_XXX = (u32)value
    out = re.sub(
        rf'\*\s*\(\s*u8\s*\*\s*\*\s*\)\s*\(\s*u32\s*\)\s*\(\s*{le}\s*\)\s*=\s*',
        f'{label} = (u32)', out)

    # Read: *(u8 **)(u32)(lbl_XXX) -> (u8*)lbl_XXX
    out = re.sub(
        rf'\*\s*\(\s*u8\s*\*\s*\*\s*\)\s*\(\s*u32\s*\)\s*\(\s*{le}\s*\)',
        f'(u8*){label}', out)

    # --- Float dereferences ---

    # *(f32*)(u32)lbl_XXX -> lbl_XXX
    out = re.sub(
        rf'\*\s*\(\s*f32\s*\*\s*\)\s*\(\s*u32\s*\)\s*{le}(?!\w)',
        label, out)

    # *(f32 *)(lbl_XXX) -> lbl_XXX   and   *(f32 *)lbl_XXX -> lbl_XXX
    out = re.sub(
        rf'\*\s*\(\s*f32\s*\*\s*\)\s*\(\s*{le}\s*\)',
        label, out)
    out = re.sub(
        rf'\*\s*\(\s*f32\s*\*\s*\)\s*{le}(?!\w)',
        label, out)

    # *(float *)(lbl_XXX) -> lbl_XXX
    out = re.sub(
        rf'\*\s*\(\s*float\s*\*\s*\)\s*\(\s*{le}\s*\)',
        label, out)
    out = re.sub(
        rf'\*\s*\(\s*float\s*\*\s*\)\s*{le}(?!\w)',
        label, out)

    # --- Double (f64) dereferences ---

    # *(f64 *)(lbl_XXX) -> lbl_XXX   and   *(f64 *)lbl_XXX -> lbl_XXX
    out = re.sub(
        rf'\*\s*\(\s*f64\s*\*\s*\)\s*\(\s*{le}\s*\)',
        label, out)
    out = re.sub(
        rf'\*\s*\(\s*f64\s*\*\s*\)\s*{le}(?!\w)',
        label, out)

    # *(double *)(lbl_XXX) -> lbl_XXX
    out = re.sub(
        rf'\*\s*\(\s*double\s*\*\s*\)\s*\(\s*{le}\s*\)',
        label, out)
    out = re.sub(
        rf'\*\s*\(\s*double\s*\*\s*\)\s*{le}(?!\w)',
        label, out)

    # --- Type-matched dereferences ---
    # Only remove deref patterns that match the label's actual type.
    # Cross-type derefs (e.g., *(u8*)u32_global) are legitimate pointer ops.

    if new_type == 'u32':
        # *(u32 *)(lbl_XXX) -> lbl_XXX
        out = re.sub(
            rf'\*\s*\(\s*u32\s*\*\s*\)\s*\(\s*{le}\s*\)',
            label, out)
        out = re.sub(
            rf'\*\s*\(\s*u32\s*\*\s*\)\s*{le}(?!\w)',
            label, out)

        # *(int *) store: *(int *)(lbl_XXX) = val -> lbl_XXX = val
        out = re.sub(
            rf'\*\s*\(\s*int\s*\*\s*\)\s*\(\s*{le}\s*\)\s*=',
            f'{label} =', out)
        out = re.sub(
            rf'\*\s*\(\s*int\s*\*\s*\)\s*{le}(?!\w)\s*=',
            f'{label} =', out)
        # *(int *) read: *(int *)(lbl_XXX) -> (int)lbl_XXX
        out = re.sub(
            rf'\*\s*\(\s*int\s*\*\s*\)\s*\(\s*{le}\s*\)',
            f'(int){label}', out)
        out = re.sub(
            rf'\*\s*\(\s*int\s*\*\s*\)\s*{le}(?!\w)',
            f'(int){label}', out)

    elif new_type == 'u16':
        # *(u16 *)(lbl_XXX) -> lbl_XXX
        out = re.sub(
            rf'\*\s*\(\s*u16\s*\*\s*\)\s*\(\s*{le}\s*\)',
            label, out)
        out = re.sub(
            rf'\*\s*\(\s*u16\s*\*\s*\)\s*{le}(?!\w)',
            label, out)
        # *(short *) store: -> lbl_XXX = val
        out = re.sub(
            rf'\*\s*\(\s*short\s*\*\s*\)\s*\(\s*{le}\s*\)\s*=',
            f'{label} =', out)
        # *(short *) read: -> (short)lbl_XXX
        out = re.sub(
            rf'\*\s*\(\s*short\s*\*\s*\)\s*\(\s*{le}\s*\)',
            f'(short){label}', out)
        out = re.sub(
            rf'\*\s*\(\s*short\s*\*\s*\)\s*{le}(?!\w)',
            f'(short){label}', out)

    elif new_type == 'u8':
        # Store: *(u8 *)(lbl_XXX) = val -> lbl_XXX = val
        out = re.sub(
            rf'\*\s*\(\s*u8\s*\*\s*\)\s*\(\s*{le}\s*\)\s*=',
            f'{label} =', out)
        out = re.sub(
            rf'\*\s*\(\s*u8\s*\*\s*\)\s*{le}(?!\w)\s*=',
            f'{label} =', out)
        # Read: *(u8 *)(lbl_XXX) -> lbl_XXX
        out = re.sub(
            rf'\*\s*\(\s*u8\s*\*\s*\)\s*\(\s*{le}\s*\)',
            label, out)
        out = re.sub(
            rf'\*\s*\(\s*u8\s*\*\s*\)\s*{le}(?!\w)',
            label, out)

        # *(char *) store: -> lbl_XXX = val
        out = re.sub(
            rf'\*\s*\(\s*char\s*\*\s*\)\s*\(\s*{le}\s*\)\s*=',
            f'{label} =', out)
        # *(char *) read: -> (char)lbl_XXX
        out = re.sub(
            rf'\*\s*\(\s*char\s*\*\s*\)\s*\(\s*{le}\s*\)',
            f'(char){label}', out)
        out = re.sub(
            rf'\*\s*\(\s*char\s*\*\s*\)\s*{le}(?!\w)',
            f'(char){label}', out)

        # Ghidra widened store artifact: *(u32 *)(lbl_XXX) = val
        # The original binary uses stb, not stw. Fix by removing deref.
        out = re.sub(
            rf'\*\s*\(\s*u32\s*\*\s*\)\s*\(\s*{le}\s*\)\s*=',
            f'{label} =', out)
        out = re.sub(
            rf'\*\s*\(\s*u32\s*\*\s*\)\s*{le}(?!\w)\s*=',
            f'{label} =', out)

    return out


def determine_type(label: str, lines: list, start_idx: int, end_idx: int):
    """Determine the correct type for an SDA global based on usage patterns.

    Returns (type_str, keep_array) where type_str is the C type and
    keep_array indicates this should remain as an array.

    The dereference patterns must be *directly* applied to the label,
    not just appearing on the same line. For example:
      *(u16 *)((u32)*(u8 *)(lbl_80478D7B) * 2 + offset)
    The label is accessed as u8 here, not u16 -- the u16 deref is on
    the outer computed expression.
    """
    if not is_sda_address(label):
        return None, True

    le = re.escape(label)
    types_seen = set()

    # Patterns that match a dereference DIRECTLY applied to the label.
    # These use the label name immediately after the cast, possibly with
    # parens around the label: *(TYPE *)(label) or *(TYPE *)label
    direct_deref_patterns = [
        # *(f32 *)(u32)label  (intermediate cast pattern)
        (rf'\*\s*\(\s*f32\s*\*\s*\)\s*\(\s*u32\s*\)\s*{le}', 'f32'),
        # *(f32 *)(label) or *(f32 *)label
        (rf'\*\s*\(\s*f32\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'f32'),
        # *(float *)(label) or *(float *)label
        (rf'\*\s*\(\s*float\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'f32'),
        # *(f64 *)(label) or *(f64 *)label
        (rf'\*\s*\(\s*f64\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'f64'),
        # *(double *)(label) or *(double *)label
        (rf'\*\s*\(\s*double\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'f64'),
        # *(u32 *)(label) or *(u32 *)label
        (rf'\*\s*\(\s*u32\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u32'),
        # *(int *)(label) or *(int *)label
        (rf'\*\s*\(\s*int\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u32'),
        # *(u16 *)(label) or *(u16 *)label
        (rf'\*\s*\(\s*u16\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u16'),
        # *(short *)(label)
        (rf'\*\s*\(\s*short\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u16'),
        # *(u8 *)(label) or *(u8 *)label
        (rf'\*\s*\(\s*u8\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u8'),
        # *(char *)(label) or *(char *)label
        (rf'\*\s*\(\s*char\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u8'),
        # **(u32 **)(label) -> global stores a pointer (u32)
        (rf'\*\s*\*\s*\(\s*u32\s*\*\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u32'),
        # **(u8 **)(label) -> global stores a pointer (u32)
        (rf'\*\s*\*\s*\(\s*u8\s*\*\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u32'),
        # *(u8 **)(label) -> global stores a pointer (u32)
        (rf'\*\s*\(\s*u8\s*\*\s*\*\s*\)\s*\(?\s*{le}\s*\)?', 'u32'),
    ]

    for i in range(start_idx, end_idx):
        line = lines[i]
        if label not in line:
            continue
        # Skip declaration
        if re.match(r'\s*extern\s+', line):
            continue

        # Array indexing -> keep as array
        if re.search(rf'{le}\s*\[\d', line):
            return None, True
        # Address-of pattern: r4 = (u32)lbl_XXX;  (used to get the address)
        # But NOT *(f32*)(u32)lbl_XXX;  (intermediate cast in dereference)
        if re.search(rf'\(\s*u32\s*\)\s*{le}\s*;', line):
            # Only keep-array if it's a plain assignment, not a deref
            if not re.search(rf'\*\s*\([^)]*\*\s*\)\s*\(\s*u32\s*\)\s*{le}', line):
                return None, True
        # Offset access: lbl_XXX + expr -> keep as array
        if re.search(rf'{le}\s*\+\s*', line) and not re.search(rf'=\s*{le}\s*\+', line):
            # But allow "lbl_XXX = lbl_XXX + N" (increment pattern)
            # Only flag as array if it's in a dereference with offset
            if re.search(rf'\*\s*\([^)]+\)\s*\(\s*{le}\s*\+', line):
                return None, True

        # Check all direct dereference patterns
        for pattern, type_name in direct_deref_patterns:
            if re.search(pattern, line):
                types_seen.add(type_name)

    # Priority: f64 > f32 > u32 > u16 > u8
    if 'f64' in types_seen:
        return 'f64', False
    if 'f32' in types_seen:
        return 'f32', False
    if 'u32' in types_seen:
        return 'u32', False
    if 'u16' in types_seen:
        return 'u16', False
    if 'u8' in types_seen:
        return 'u8', False

    # No clear type from deref patterns; check for plain usage
    return 'u32', False


def prescan_array_labels(lines: list) -> set:
    """Pre-scan the entire file to identify labels that use offset access
    or address-of patterns ANYWHERE. These must stay as arrays globally,
    even if some functions only use them without offsets."""
    array_labels = set()
    for line in lines:
        # Skip extern declarations and block comment lines (but NOT code
        # starting with * dereference operator)
        stripped = line.strip()
        if stripped.startswith('extern') or stripped.startswith('//') or stripped.startswith('/*'):
            continue
        # Block comment continuation: " * text" but not "*(type*)" (dereference)
        if stripped.startswith('*') and not stripped.startswith('*('):
            continue

        # Find all lbl_ references on this line
        for m in re.finditer(r'(lbl_[0-9A-Fa-f]+)', line):
            label = m.group(1)
            le = re.escape(label)

            # Offset access: *(type*)(lbl_XXX + expr)
            if re.search(rf'\(\s*{le}\s*\+', line):
                array_labels.add(label)
            # Array indexing: lbl_XXX[N]
            if re.search(rf'{le}\s*\[', line):
                array_labels.add(label)

            # Address-of usage only matters for non-SDA labels.
            # For SDA labels (0x8047xxxx), (u32)lbl_XXX is just a
            # value widening cast, not "take address of array".
            # For rodata/data labels (0x8027, 0x8037, 0x8039), it IS
            # an address reference.
            if not is_sda_address(label):
                # (u32)lbl_XXX (taking the address as a value)
                if re.search(rf'\(\s*u32\s*\)\s*{le}(?!\s*[\[\(])', line):
                    if not re.search(rf'\(\s*u32\s*\)\s*\*', line):
                        array_labels.add(label)
                # (void*)(u32)lbl_XXX pattern
                if re.search(rf'\(\s*void\s*\*\s*\)\s*\(\s*u32\s*\)\s*{le}', line):
                    array_labels.add(label)

    return array_labels


def fix_file(filepath: str, dry_run: bool = False) -> int:
    """Fix SDA global access patterns in a file."""
    path = Path(filepath)
    content = path.read_text(encoding='utf-8')
    lines = content.split('\n')

    # Pre-scan to find labels that must stay as arrays globally
    global_array_labels = prescan_array_labels(lines)
    if global_array_labels:
        print(f"Labels that must stay as arrays (offset/address-of usage):")
        for lbl in sorted(global_array_labels):
            print(f"  {lbl}")

    functions = find_function_boundaries(lines)
    total_fixes = 0

    for func_start, func_end in functions:
        # Find all extern u8 lbl_XXX[] declarations in this function
        decl_pattern = re.compile(r'(\s*)extern\s+u8\s+(lbl_[0-9A-Fa-f]+)\[\]\s*;')
        labels_in_func = {}
        for i in range(func_start, func_end):
            m = decl_pattern.match(lines[i])
            if m:
                indent = m.group(1)
                label = m.group(2)
                if label not in labels_in_func:
                    labels_in_func[label] = []
                labels_in_func[label].append((i, indent))

        if not labels_in_func:
            continue

        for label, decl_locations in labels_in_func.items():
            # Skip labels that need to stay as arrays globally
            if label in global_array_labels:
                continue

            new_type, keep_array = determine_type(label, lines, func_start, func_end)

            if keep_array:
                continue

            # Fix declarations
            for decl_idx, indent in decl_locations:
                old_line = lines[decl_idx]
                new_line = f'{indent}extern {new_type} {label};'
                if old_line != new_line:
                    if dry_run:
                        print(f"  L{decl_idx + 1}: {old_line.strip()}")
                        print(f"     -> {new_line.strip()}")
                    else:
                        lines[decl_idx] = new_line
                    total_fixes += 1

            # Fix usages
            for i in range(func_start, func_end):
                if label not in lines[i]:
                    continue
                if re.match(r'\s*extern\s+', lines[i]):
                    continue

                old_line = lines[i]
                new_line = apply_line_fixes(old_line, label, new_type)

                if new_line != old_line:
                    if dry_run:
                        print(f"  L{i + 1}: {old_line.strip()}")
                        print(f"     -> {new_line.strip()}")
                    else:
                        lines[i] = new_line
                    total_fixes += 1

    # Second pass: fix dereference patterns on labels that are ALREADY
    # declared with the correct scalar type (either at file level or
    # in function bodies from a previous run). These have declarations
    # like `extern u32 lbl_XXX;` but still use `*(u32*)lbl_XXX`.
    typed_decl_pattern = re.compile(
        r'extern\s+(u32|u16|u8|s32|s16|s8|int|f32|f64|float|double)\s+'
        r'(lbl_[0-9A-Fa-f]+)\s*;')
    typed_labels = {}  # label -> type
    for line in lines:
        m = typed_decl_pattern.search(line)
        if m:
            declared_type = m.group(1)
            label = m.group(2)
            if is_sda_address(label) and label not in global_array_labels:
                typed_labels[label] = declared_type

    for label, declared_type in typed_labels.items():
        for i in range(len(lines)):
            if label not in lines[i]:
                continue
            if re.match(r'\s*extern\s+', lines[i]):
                continue

            old_line = lines[i]
            new_line = apply_line_fixes(old_line, label, declared_type)

            if new_line != old_line:
                if dry_run:
                    print(f"  L{i + 1}: {old_line.strip()}")
                    print(f"     -> {new_line.strip()}")
                else:
                    lines[i] = new_line
                total_fixes += 1

    if not dry_run and total_fixes > 0:
        path.write_text('\n'.join(lines), encoding='utf-8')

    print(f"\nApplied {total_fixes} fixes to {filepath}")
    return total_fixes


def main():
    import argparse
    parser = argparse.ArgumentParser(description='Fix SDA global access patterns')
    parser.add_argument('file', help='Source file to fix')
    parser.add_argument('--dry-run', '-n', action='store_true',
                        help='Show changes without applying them')
    args = parser.parse_args()

    fixes = fix_file(args.file, dry_run=args.dry_run)
    print(f"\nTotal fixes: {fixes}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
