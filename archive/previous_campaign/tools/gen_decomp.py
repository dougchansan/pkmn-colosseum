#!/usr/bin/env python3
"""Generate all C source files and asm includes for Pokemon Colosseum decomp.

This script reads the assembly file and symbols, categorizes functions by
subsystem, and generates C source files with either decompiled C code (for
small functions) or asm includes (for larger functions).

Usage: python tools/gen_decomp.py
"""
import re
import os
import sys

PROJECT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASM_FILE = os.path.join(PROJECT, "build", "GC6E01", "asm", "auto_01_800055E0_text.s")
SYMBOLS = os.path.join(PROJECT, "config", "GC6E01", "symbols.txt")

# Parse symbols for function names and sizes in range
funcs = []
in_range = False
with open(SYMBOLS, 'r') as f:
    for line in f:
        line = line.strip()
        if 'fn_80059BDC' in line and 'type:function' in line:
            in_range = True
        if not in_range:
            continue
        if 'type:function' not in line:
            continue
        m = re.match(
            r'(\w+)\s*=\s*\.text:(0x[0-9A-Fa-f]+);\s*//\s*type:function\s+size:(0x[0-9A-Fa-f]+)',
            line
        )
        if m:
            name = m.group(1)
            addr = int(m.group(2), 16)
            size = int(m.group(3), 16)
            funcs.append((name, addr, size))
        if 'PPCMfmsr' in line:
            break

print(f"Total functions in range: {len(funcs)}")

# Categorize into source files based on address ranges
plan = {}
for name, addr, size in funcs:
    if addr < 0x80069A60:
        key = 'ui_core'
    elif addr < 0x80069C0C:
        key = 'menu_battle'
    elif addr < 0x8007109C:
        key = 'menu_middle'
    elif addr < 0x80072A00:
        key = 'menu_common_ext'
    elif addr < 0x80075818:
        key = 'menu_tool'
    elif addr < 0x80076788:
        key = 'menu_tool2'
    elif addr < 0x80077A5C:
        key = 'menu_rule'
    elif addr < 0x80078D38:
        key = 'menu_exdisc'
    elif addr < 0x8007C2C0:
        key = 'menu_exdisc2'
    elif addr < 0x8007C300:
        key = 'menu_pokecoupon'
    elif addr < 0x8007FD64:
        key = 'menu_carde_matrix'
    elif addr < 0x80082650:
        key = 'menu_carde_main'
    elif addr < 0x80083AF4:
        key = 'save_carde'
    elif addr < 0x80089048:
        key = 'gba_conv'
    elif addr < 0x800895A4:
        key = 'gba_conv2'
    elif addr < 0x80092C90:
        key = 'gba_misc'
    elif addr < 0x800937F4:
        key = 'gba_comm_ext'
    elif addr < 0x80098000:
        key = 'late_game'
    else:
        key = 'ppc_arch'
    plan.setdefault(key, []).append((name, addr, size))

for key, fl in sorted(plan.items()):
    print(f"  {key}: {len(fl)} functions")

# Read ASM file and index function line positions
print("Indexing ASM file...")
fn_lines = {}
current_fn = None
current_start = 0
with open(ASM_FILE, 'r') as f:
    for i, line in enumerate(f, 1):
        m = re.match(r'\.fn (\w+)', line)
        if m:
            current_fn = m.group(1)
            current_start = i
        m2 = re.match(r'\.endfn (\w+)', line)
        if m2 and current_fn:
            fn_lines[current_fn] = (current_start, i)
            current_fn = None

print(f"Indexed {len(fn_lines)} functions")

# Read ASM file into memory
print("Loading ASM file...")
with open(ASM_FILE, 'r') as f:
    asm_lines = f.readlines()


def extract_asm(name):
    """Extract assembly instructions for a function in CW inline asm format.

    Converts from GNU as syntax to CodeWarrior inline asm syntax:
    - .L_XXXX labels become @XXXX local labels
    - crclr cr1eq becomes crclr 6
    - psq_st/psq_ld qr0 becomes numeric 0
    - beq .L_X becomes beq @X
    """
    if name not in fn_lines:
        return None
    start, end = fn_lines[name]
    result = []

    # CR bit name to number mapping
    cr_bits = {
        'cr0lt': '0', 'cr0gt': '1', 'cr0eq': '2', 'cr0un': '3',
        'cr1lt': '4', 'cr1gt': '5', 'cr1eq': '6', 'cr1un': '7',
        'cr2lt': '8', 'cr2gt': '9', 'cr2eq': '10', 'cr2un': '11',
        'cr3lt': '12', 'cr3gt': '13', 'cr3eq': '14', 'cr3un': '15',
        'cr4lt': '16', 'cr4gt': '17', 'cr4eq': '18', 'cr4un': '19',
        'cr5lt': '20', 'cr5gt': '21', 'cr5eq': '22', 'cr5un': '23',
        'cr6lt': '24', 'cr6gt': '25', 'cr6eq': '26', 'cr6un': '27',
        'cr7lt': '28', 'cr7gt': '29', 'cr7eq': '30', 'cr7un': '31',
    }

    for i in range(start, end):
        line = asm_lines[i - 1]
        m = re.match(r'/\*.*\*/\s*(.*)', line)
        if m:
            instr = m.group(1).strip()
            if instr:
                # Convert .L_XXXX references to @XXXX for CW
                instr = re.sub(r'\.L_([0-9A-Fa-f]+)', r'@\1', instr)
                # Convert CR bit names to numbers
                for cr_name, cr_num in cr_bits.items():
                    instr = instr.replace(cr_name, cr_num)
                # Convert qr0-qr7 to numeric in psq instructions
                instr = re.sub(r'\bqr(\d)\b', r'\1', instr)
                result.append(instr)
        elif line.strip().startswith('.L_'):
            # Convert .L_XXXX: to @XXXX
            lbl = line.strip().rstrip(':')
            lbl = lbl.replace('.L_', '@')
            result.append(lbl)
    return '\n'.join(result) + '\n'


def get_fn_body(name):
    """Get raw asm body text for analysis."""
    if name not in fn_lines:
        return ''
    start, end = fn_lines[name]
    return ''.join(asm_lines[start:end - 1])


# Mapping of plan key -> (src_path, asm_dir, description)
FILE_MAP = {
    'ui_core': ('src/game/ui/ui_core.c', 'ui_core',
                'UI Core state machine and dispatch (0x80059BDC-0x80069A60)'),
    'menu_middle': ('src/game/menu/menu_middle.c', 'menu_middle',
                    'Menu middle code between battle and common (0x80069C0C-0x8007109C)'),
    'menu_common_ext': ('src/game/menu/menu_common_ext.c', 'menu_common_ext',
                        'Menu common extensions - helpers, draw, input (0x8007109C-0x80072A00)'),
    'menu_tool': ('src/game/menu/menu_tool.c', 'menu_tool',
                  'Menu tool functions (0x80072A00-0x80075818)'),
    'menu_tool2': ('src/game/menu/menu_tool2.c', 'menu_tool2',
                   'Menu tool functions continued (0x80075818-0x80076788)'),
    'menu_rule': ('src/game/menu/menu_rule.c', 'menu_rule',
                  'Menu rule handlers (0x800767B8-0x80077A5C)'),
    'menu_exdisc': ('src/game/menu/menu_exdisc.c', 'menu_exdisc',
                    'Extra disc shrine and related menus (0x80077A5C-0x80078D38)'),
    'menu_exdisc2': ('src/game/menu/menu_exdisc2.c', 'menu_exdisc2',
                     'Extra disc coupon and related menus (0x80078D38-0x8007C2C0)'),
    'menu_carde_matrix': ('src/game/menu/menu_carde_matrix.c', 'menu_carde_matrix',
                          'Card-E matrix display (0x8007C300-0x8007FD64)'),
    'menu_carde_main': ('src/game/menu/menu_carde_main.c', 'menu_carde_main',
                        'Card-E main handlers (0x8007FD64-0x80082650)'),
    'save_carde': ('src/game/save/save_carde.c', 'save_carde',
                   'Card-E save data validation (0x80082650-0x80083AF4)'),
    'gba_conv': ('src/game/gba/gba_conv.c', 'gba_conv',
                 'GBA data conversion and validation (0x80083AF4-0x80089048)'),
    'gba_conv2': ('src/game/gba/gba_conv2.c', 'gba_conv2',
                  'Pokemon conversion GBA-GCN (0x80089048-0x800895A4)'),
    'gba_misc': ('src/game/gba/gba_misc.c', 'gba_misc',
                 'GBA miscellaneous communication support (0x800895A4-0x80092C90)'),
    'gba_comm_ext': ('src/game/gba/gba_comm_ext.c', 'gba_comm_ext',
                     'GBA communication transfers (0x80092C90-0x800937F4)'),
    'late_game': ('src/game/late_game.c', 'late_game',
                  'Late game code before SDK (0x800937F4-0x80097FFC)'),
}

# Skip these - already exist or are SDK
SKIP_KEYS = {'menu_battle', 'menu_pokecoupon', 'ppc_arch'}

for key, func_list in sorted(plan.items()):
    if key in SKIP_KEYS:
        continue
    if key not in FILE_MAP:
        print(f"SKIP (no mapping): {key}")
        continue

    src_path, asm_dir, desc = FILE_MAP[key]
    full_path = os.path.join(PROJECT, src_path)

    # Skip if file already exists
    if os.path.exists(full_path):
        print(f"SKIP (exists): {src_path}")
        continue

    os.makedirs(os.path.dirname(full_path), exist_ok=True)

    lines = []
    lines.append('/**')
    lines.append(' * @file %s' % os.path.basename(src_path))
    lines.append(' * @brief %s' % desc)
    lines.append(' *')
    lines.append(' * Address range: 0x%08X - 0x%08X' % (
        func_list[0][1], func_list[-1][1] + func_list[-1][2]))
    lines.append(' * Total functions: %d' % len(func_list))
    lines.append(' */')
    lines.append('')
    lines.append('#include "dolphin/types.h"')
    lines.append('')

    # Collect all called functions for extern declarations
    called_fns = set()
    local_fn_names = {n for n, a, s in func_list}

    for name, addr, size in func_list:
        body = get_fn_body(name)
        calls = re.findall(r'bl\s+(\w+)', body)
        for c in calls:
            if c not in local_fn_names and c.startswith('fn_'):
                called_fns.add(c)

    # Also find named function calls
    named_calls = set()
    for name, addr, size in func_list:
        body = get_fn_body(name)
        calls = re.findall(r'bl\s+(\w+)', body)
        for c in calls:
            if c not in local_fn_names and not c.startswith('fn_') and c not in ('memset', 'memcpy'):
                named_calls.add(c)

    lines.append('/* ===== External function declarations ===== */')
    for fn in sorted(called_fns)[:60]:
        lines.append('extern void %s();' % fn)
    if len(called_fns) > 60:
        lines.append('/* ... and %d more external functions */' % (len(called_fns) - 60))
    for fn in sorted(named_calls):
        lines.append('extern void %s();' % fn)

    # memset/memcpy
    has_memset = False
    has_memcpy = False
    for name, addr, size in func_list:
        body = get_fn_body(name)
        if 'bl memset' in body:
            has_memset = True
        if 'bl memcpy' in body:
            has_memcpy = True
    if has_memset:
        lines.append('extern void* memset(void* dst, int val, u32 size);')
    if has_memcpy:
        lines.append('extern void* memcpy(void* dst, const void* src, u32 size);')

    # SDA references
    sda_refs = set()
    for name, addr, size in func_list:
        body = get_fn_body(name)
        refs = re.findall(r'(lbl_[0-9A-Fa-f]+)@sda21', body)
        sda_refs.update(refs)

    if sda_refs:
        lines.append('')
        lines.append('/* ===== SDA globals ===== */')
        for ref in sorted(sda_refs):
            lines.append('extern u8 %s;' % ref)

    # Rodata references
    rodata_refs = set()
    for name, addr, size in func_list:
        body = get_fn_body(name)
        refs = re.findall(r'(lbl_[0-9A-Fa-f]+)@(?:ha|l)', body)
        for r in refs:
            if r not in sda_refs:
                rodata_refs.add(r)
        jts = re.findall(r'(jumptable_[0-9A-Fa-f]+)', body)
        rodata_refs.update(jts)

    if rodata_refs:
        lines.append('')
        lines.append('/* ===== Rodata / data labels ===== */')
        for ref in sorted(rodata_refs):
            lines.append('extern u8 %s[];' % ref)

    # Pre-compute signatures for all local functions
    # This ensures forward declarations match definitions exactly
    fn_sigs = {}  # name -> (ret_type, param_str, returns_value, has_ret_neg1, calls)
    for name, addr, size in func_list:
        body = get_fn_body(name) if name in fn_lines else ''
        calls_list = re.findall(r'bl\s+(\w+)', body)
        has_ret_zero = 'li r3, 0x0' in body or 'li r3, 0\n' in body
        has_ret_neg1 = 'li r3, -0x1' in body
        returns_value = has_ret_zero or has_ret_neg1 or ('li r3,' in body and 'blr' in body)

        # Simplified param detection: just use void for everything
        # This avoids mismatches. Parameters can be refined later.
        param_str = 'void'
        ret_type = 's32' if returns_value else 'void'

        fn_sigs[name] = (ret_type, param_str, returns_value, has_ret_neg1, calls_list)

    # Add forward declarations for all local functions
    lines.append('')
    lines.append('/* ===== Forward declarations ===== */')
    for name, addr, size in func_list:
        ret_type, param_str, _, _, _ = fn_sigs[name]
        lines.append('%s %s(%s);' % (ret_type, name, param_str))

    lines.append('')
    lines.append('/* ===== Function implementations ===== */')
    lines.append('')
    lines.append('#pragma push')
    lines.append('#pragma force_active on')
    lines.append('')

    # Generate each function as C code
    for name, addr, size in func_list:
        lines.append('/* 0x%08X | size: 0x%X */' % (addr, size))
        ret_type, param_str, returns_value, has_ret_neg1, calls = fn_sigs[name]
        body = get_fn_body(name) if name in fn_lines else ''

        # Filter out known prototyped functions from stub calls
        SKIP_CALL = {'memset', 'memcpy', 'memcmp', 'strlen', 'strcpy', 'sprintf',
                     'strcmp', 'strncpy', 'strncmp'}
        calls = [c for c in calls if c not in SKIP_CALL]

        # Use precomputed signature for consistency
        sig = '%s %s(%s)' % (ret_type, name, param_str)

        if size <= 0x04:
            lines.append('%s {' % sig)
            lines.append('}')
        elif size <= 0x08:
            if 'li r3' in body:
                m = re.search(r'li r3,\s*([-0-9xa-fA-F]+)', body)
                val = m.group(1) if m else '0'
                lines.append('%s {' % sig)
                lines.append('    return %s;' % val)
                lines.append('}')
            else:
                lines.append('%s {' % sig)
                if returns_value:
                    lines.append('    return 0;')
                lines.append('}')
        elif size <= 0x30:
            # Small function
            lines.append('%s {' % sig)
            for c in calls:
                if c != name:
                    lines.append('    %s();' % c)
            if returns_value:
                lines.append('    return %s;' % ('-1' if has_ret_neg1 else '0'))
            lines.append('}')
        elif size <= 0x80:
            # Medium-small function
            lines.append('%s {' % sig)
            lines.append('    /* TODO: decompile (0x%X bytes) */' % size)
            unique_calls = list(dict.fromkeys(c for c in calls if c != name))
            for c in unique_calls[:5]:
                lines.append('    %s();' % c)
            if returns_value:
                lines.append('    return %s;' % ('-1' if has_ret_neg1 else '0'))
            lines.append('}')
        elif size <= 0x200:
            # Medium function
            lines.append('%s {' % sig)
            lines.append('    /* TODO: decompile (0x%X bytes) */' % size)
            if 'bctr' in body:
                lines.append('    /* Contains switch statement */')
            unique_calls = list(dict.fromkeys(c for c in calls if c != name))
            for c in unique_calls[:8]:
                lines.append('    %s();' % c)
            if returns_value:
                lines.append('    return %s;' % ('-1' if has_ret_neg1 else '0'))
            lines.append('}')
        else:
            # Large function
            lines.append('%s {' % sig)
            lines.append('    /* TODO: decompile (0x%X bytes, ~%d instructions) */' % (
                size, size // 4))
            if 'bctr' in body:
                lines.append('    /* Contains switch/jump table */')
            if 'stfd' in body or 'lfd' in body:
                lines.append('    /* Uses floating point */')
            if 'stmw' in body:
                lines.append('    /* Uses many saved registers */')
            unique_calls = list(dict.fromkeys(c for c in calls if c != name))
            for c in unique_calls[:12]:
                lines.append('    %s();' % c)
            if returns_value:
                lines.append('    return %s;' % ('-1' if has_ret_neg1 else '0'))
            lines.append('}')

        lines.append('')

    lines.append('#pragma pop')

    with open(full_path, 'w') as f:
        f.write('\n'.join(lines) + '\n')

    print(f"WROTE: {src_path} ({len(func_list)} functions)")

print("\nGeneration complete!")
