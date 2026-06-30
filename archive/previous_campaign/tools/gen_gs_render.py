#!/usr/bin/env python3
"""
Generate decompiled gs_render.c by reading PPC assembly and converting
each stub function to C89 code.

This handles all 270 pragma stubs in gs_render.c.
"""

import re
import os
import sys

PROJ = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASM_FILE = os.path.join(PROJ, 'build', 'GC6E01', 'asm', 'auto_01_800055E0_text.s')
SRC_FILE = os.path.join(PROJ, 'src', 'game', 'gs_render.c')


def load_asm():
    with open(ASM_FILE, 'r') as f:
        text = f.read()
    funcs = {}
    for m in re.finditer(r'\.fn (fn_[0-9A-Fa-f]+), global\n(.*?)\.endfn \1', text, re.DOTALL):
        funcs[m.group(1)] = m.group(2).strip()
    return funcs


def get_insns(body):
    """Get clean instruction list from asm body."""
    result = []
    for line in body.split('\n'):
        line = line.strip()
        if line.startswith('.L_'):
            result.append(('label', line.rstrip(':').strip()))
            continue
        m = re.match(r'/\*\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f ]+\*/\s+(.*)', line)
        if m:
            inst = m.group(1).strip()
            parts = inst.split(None, 1)
            mnem = parts[0]
            ops = parts[1] if len(parts) > 1 else ''
            result.append(('inst', mnem, ops))
    return result


def ops_list(ops_str):
    """Split operands by comma."""
    if not ops_str:
        return []
    return [o.strip() for o in ops_str.split(',')]


def parse_mem(op):
    """Parse memory operand: offset(base) -> (offset_or_label, base)"""
    m = re.match(r'(.*)\((\w+)\)', op)
    if not m:
        return None, None
    offset_str = m.group(1).strip()
    base = m.group(2)
    # SDA label
    if '@sda21' in offset_str:
        label = offset_str.replace('@sda21', '')
        return label, base
    # HA/LO label
    if '@ha' in offset_str or '@l' in offset_str:
        label = re.sub(r'@\w+', '', offset_str)
        return label, base
    # Numeric
    try:
        if offset_str.startswith('-'):
            return -int(offset_str[1:], 0), base
        return int(offset_str, 0), base
    except ValueError:
        return offset_str, base


# ============================================================
# GX FIFO address constant
# ============================================================
FIFO_ADDR = 0xCC008000


def decompile_function(fn_name, asm_funcs):
    """Decompile a single function. Returns (signature, body, needs_pragma)."""
    if fn_name not in asm_funcs:
        return None

    insns = get_insns(asm_funcs[fn_name])
    # Just instruction tuples (no labels)
    real = [(entry[1], entry[2]) for entry in insns if entry[0] == 'inst'] if insns else []
    if not real:
        return None

    n = len(real)

    # ---- Pattern: single blr (empty function) ----
    if n == 1 and real[0][0] == 'blr':
        return ('void', '', False)

    # For all simple patterns: fall through to the general decompiler
    # which properly declares register variables

    # Now handle the full instruction-by-instruction decompilation
    return decompile_full(fn_name, insns, real)


def decompile_full(fn_name, insns, real):
    """Full decompilation -- always use the general decompiler."""
    return decompile_general(fn_name, insns, real)


def decompile_gx_fifo_leaf(fn_name, insns, real):
    """Decompile GX FIFO leaf (write state to FIFO)."""
    # These load lbl_8047AA80 ptr, optionally compute an index,
    # load fields, then write them to the FIFO at 0xCC008000.

    lines = []
    # Track what's happening
    state_ptr = 'state'
    has_index_param = False
    index_mult = None
    index_shift = None
    loads = []  # (type, offset_expr)
    writes_to_fifo = []

    # Analyze instructions
    regs = {}
    for mnem, ops_str in real:
        if mnem == 'blr':
            break
        op = ops_list(ops_str)

        if mnem == 'lwz' and '@sda21' in ops_str:
            label = re.search(r'(lbl_\w+)', ops_str).group(1)
            regs[op[0]] = label

        elif mnem == 'lis' and '0xcc01' in ops_str:
            regs[op[0]] = 'FIFO'

        elif mnem == 'mulli':
            mult = int(op[2], 0)
            index_mult = mult
            has_index_param = True
            regs[op[0]] = f'(idx * {mult})'

        elif mnem == 'slwi':
            shift = int(op[2])
            index_shift = shift
            has_index_param = True
            regs[op[0]] = f'(idx << {shift})'

        elif mnem == 'add':
            # Combining state_ptr + index
            regs[op[0]] = f'base'

        elif mnem in ('lbz', 'lhz', 'lwz', 'lfs'):
            off, base = parse_mem(op[1])
            if isinstance(off, int):
                lt = {'lbz': 'u8', 'lhz': 'u16', 'lwz': 'u32', 'lfs': 'f32'}[mnem]
                # Track what register holds what
                base_val = regs.get(base, base)
                if base_val == 'base' or 'idx' in str(base_val):
                    regs[op[0]] = (lt, off, True)  # indexed
                else:
                    regs[op[0]] = (lt, off, False)  # direct

        elif mnem in ('stb', 'sth', 'stw', 'stfs'):
            off, base = parse_mem(op[1])
            if isinstance(off, int) and off == -0x8000:
                # Write to FIFO
                src_info = regs.get(op[0])
                if isinstance(src_info, tuple):
                    writes_to_fifo.append(src_info)
                else:
                    # Could be a computed value
                    writes_to_fifo.append(('u8', 0, False))

    # Generate C
    fifo_type = 'volatile u8*'
    lines.append(f'    volatile u8* fifo = (volatile u8*)0xCC008000;')
    lines.append(f'    u8* state = (u8*){regs.get("r4", regs.get("r5", regs.get("r6", "lbl_8047AA80")))};')

    if has_index_param:
        if index_mult:
            lines.append(f'    u8* base = state + (idx * {index_mult});')
        elif index_shift:
            lines.append(f'    u8* base = state + (idx << {index_shift});')

    for lt, off, indexed in writes_to_fifo:
        src_base = 'base' if indexed else 'state'
        if lt == 'u8':
            lines.append(f'    *(volatile u8*)fifo = *({lt}*)({src_base} + 0x{off:X});')
        elif lt == 'u16':
            lines.append(f'    *(volatile u16*)fifo = *({lt}*)({src_base} + 0x{off:X});')
        elif lt == 'u32':
            lines.append(f'    *(volatile u32*)fifo = *({lt}*)({src_base} + 0x{off:X});')
        elif lt == 'f32':
            lines.append(f'    *(volatile f32*)fifo = *(f32*)({src_base} + 0x{off:X});')

    return ('void', '\n'.join(lines) + '\n', False)


def decompile_simple_leaf(fn_name, insns, real):
    """Decompile simple leaf function with no frame or branches."""
    lines = []
    regs = {}

    for mnem, ops_str in real:
        if mnem == 'blr':
            break
        op = ops_list(ops_str)

        if mnem in ('stfs',):
            off, base = parse_mem(op[1])
            if isinstance(off, int):
                # f1-f6 are float params
                freg = op[0]
                lines.append(f'    *(f32*)((u8*){base} + 0x{off:X}) = {freg};')

        elif mnem in ('stw', 'stb', 'sth'):
            off, base = parse_mem(op[1])
            if isinstance(off, int):
                st = {'stw': 'u32', 'stb': 'u8', 'sth': 'u16'}[mnem]
                lines.append(f'    *({st}*)((u8*){base} + 0x{off:X}) = {op[0]};')

        elif mnem == 'li':
            regs[op[0]] = op[1]

        elif mnem in ('lbz', 'lhz', 'lwz', 'lfs'):
            off, base = parse_mem(op[1])
            if isinstance(off, int):
                lt = {'lbz': 'u8', 'lhz': 'u16', 'lwz': 'u32', 'lfs': 'f32'}[mnem]
                regs[op[0]] = f'*({lt}*)((u8*){base} + 0x{off:X})'

        elif mnem == 'mulli':
            regs[op[0]] = f'({op[1]} * {op[2]})'

        elif mnem == 'add':
            regs[op[0]] = f'({op[1]} + {op[2]})'

        elif mnem == 'mr':
            regs[op[0]] = regs.get(op[1], op[1])

        elif mnem == 'fneg':
            off_prev, base_prev = None, None
            # look at what was loaded
            regs[op[0]] = f'-{op[1]}'

    if lines:
        return ('void', '\n'.join(lines) + '\n', False)
    return None


def decompile_leaf_condret(fn_name, insns, real):
    """Decompile leaf function with conditional returns (beqlr etc)."""
    lines = []
    for mnem, ops_str in real:
        if mnem == 'blr':
            lines.append('    return;')
            break
        op = ops_list(ops_str)

        if mnem in ('lbz', 'lhz', 'lwz'):
            off, base = parse_mem(op[1])
            lt = {'lbz': 'u8', 'lhz': 'u16', 'lwz': 'u32'}[mnem]
            if isinstance(off, int):
                lines.append(f'    /* {mnem} */ {op[0]} = *({lt}*)((u8*){base} + 0x{off:X});')
        elif mnem == 'cmplwi':
            lines.append(f'    /* cmplwi {ops_str} */;')
        elif mnem == 'cmpwi':
            lines.append(f'    /* cmpwi {ops_str} */;')
        elif mnem == 'beqlr':
            lines.append('    if (/* eq */) return;')
        elif mnem == 'bnelr':
            lines.append('    if (/* ne */) return;')
        elif mnem in ('stb', 'sth', 'stw', 'stfs'):
            off, base = parse_mem(op[1])
            st = {'stb': 'u8', 'sth': 'u16', 'stw': 'u32', 'stfs': 'f32'}[mnem]
            if isinstance(off, int):
                lines.append(f'    *({st}*)((u8*){base} + 0x{off:X}) = {op[0]};')
        elif mnem == 'li':
            lines.append(f'    {op[0]} = {op[1]};')
        elif mnem == 'lfs':
            off, base = parse_mem(op[1])
            if isinstance(off, str):
                lines.append(f'    {op[0]} = *(f32*){off};')
            elif isinstance(off, int):
                lines.append(f'    {op[0]} = *(f32*)((u8*){base} + 0x{off:X});')
        else:
            lines.append(f'    /* {mnem} {ops_str} */;')

    return ('void', '\n'.join(lines) + '\n', True)


def decompile_wrapper(fn_name, insns, real):
    """Decompile wrapper that calls a single function."""
    # Find the bl target
    target = None
    for mnem, ops_str in real:
        if mnem == 'bl' and not ops_str.startswith('_'):
            target = ops_str.strip()
            break

    if not target:
        return decompile_general(fn_name, insns, real)

    # Analyze parameter shuffling before the call
    # Common patterns:
    # 1. Just call target() -- no param changes
    # 2. Swap r3/r4 then call
    # 3. Load a field then call
    # 4. Set extra params then call

    # For simplicity, generate the wrapper call
    # Check if there's parameter manipulation
    pre_bl = []
    for mnem, ops_str in real:
        if mnem == 'bl' and not ops_str.startswith('_'):
            break
        if mnem in ('stwu', 'mflr', 'stw'):
            continue
        if mnem == 'stw':
            op = ops_list(ops_str)
            off, base = parse_mem(op[1])
            if base == 'r1':
                continue
        pre_bl.append((mnem, ops_str))

    body_lines = []
    # Add the function call with a comment about what it wraps
    if not pre_bl:
        body_lines.append(f'    {target}();')
    else:
        # Generate parameter setup code
        for mnem, ops_str in pre_bl:
            op = ops_list(ops_str)
            if mnem == 'mr':
                body_lines.append(f'    /* {mnem} {ops_str} */;')
            elif mnem == 'li':
                body_lines.append(f'    /* {mnem} {ops_str} */;')
            elif mnem == 'lwz':
                off, base = parse_mem(op[1])
                if isinstance(off, int):
                    lt = 'u32'
                    body_lines.append(f'    /* {mnem} {ops_str} */;')
                elif isinstance(off, str):
                    body_lines.append(f'    /* {mnem} {ops_str} */;')
            elif mnem == 'lfs':
                body_lines.append(f'    /* {mnem} {ops_str} */;')
            elif mnem == 'fdivs':
                body_lines.append(f'    /* {mnem} {ops_str} */;')
            elif mnem == 'addi':
                body_lines.append(f'    /* {mnem} {ops_str} */;')
            else:
                body_lines.append(f'    /* {mnem} {ops_str} */;')
        body_lines.append(f'    {target}();')

    return ('void', '\n'.join(body_lines) + '\n', True)


def decompile_general(fn_name, insns, real):
    """General decompilation -- compilable C with register vars and gotos."""

    # First pass: collect used registers, labels, float regs, stack locals
    used_iregs = set()
    used_fregs = set()
    used_labels = set()
    branch_targets = set()
    has_frame = real[0][0] == 'stwu' if real else False
    frame_size = 0
    stack_loads = set()  # offsets loaded from stack
    stack_stores = set()  # offsets stored to stack (that aren't LR/saved regs)
    has_ctr_call = False
    last_cmp = None

    if has_frame:
        op = ops_list(real[0][1])
        fmem = parse_mem(op[1])
        if fmem[0] is not None and isinstance(fmem[0], int):
            frame_size = -fmem[0]

    for entry in insns:
        if entry[0] == 'label':
            used_labels.add(entry[1])
            continue
        if entry[0] != 'inst':
            continue
        mnem, ops_str = entry[1], entry[2] if len(entry) > 2 else ''
        op = ops_list(ops_str) if ops_str else []

        # Collect register usage
        for o in op:
            m = re.match(r'^r(\d+)$', o)
            if m:
                rn = int(m.group(1))
                if rn != 2:
                    used_iregs.add(rn)
            m = re.match(r'^f(\d+)$', o)
            if m:
                used_fregs.add(int(m.group(1)))

        # Also find registers in memory operands
        for o in op:
            for rm in re.finditer(r'r(\d+)', o):
                rn = int(rm.group(1))
                if rn != 2:
                    used_iregs.add(rn)
            for fm in re.finditer(r'f(\d+)', o):
                used_fregs.add(int(fm.group(1)))

        # Collect branch targets
        if mnem.startswith('b') and mnem not in ('blr', 'bl', 'bctrl', 'bctr',
                                                   'beqlr', 'bnelr', 'bgelr',
                                                   'bltlr', 'bgtlr', 'blelr'):
            target = op[-1].strip() if op else ''
            if target.startswith('.L_'):
                branch_targets.add(target)

        if mnem == 'bctrl':
            has_ctr_call = True

        # Track stack loads for local variables
        if mnem in ('lwz', 'lbz', 'lhz', 'lfs', 'lfd') and len(op) >= 2:
            off, base = parse_mem(op[1])
            if base == 'r1' and isinstance(off, int) and off > 0 and off < frame_size:
                stack_loads.add(off)
        if mnem in ('stw', 'stb', 'sth', 'stfs', 'stfd') and len(op) >= 2:
            off, base = parse_mem(op[1])
            if base == 'r1' and isinstance(off, int) and off > 0 and off < frame_size:
                stack_stores.add(off)

    # Determine which stack offsets are local variables (both read and written)
    stack_locals = stack_loads & stack_stores
    # Also include stores that are later loaded (even if only stored first)
    stack_locals = stack_loads | stack_stores

    # Collect SDA/HA label references
    sda_labels = set()
    ha_labels = set()
    bl_targets = set()
    for entry in insns:
        if entry[0] != 'inst':
            continue
        ops_str = entry[2] if len(entry) > 2 else ''
        for m in re.finditer(r'(lbl_[0-9A-Fa-f]+)@sda21', ops_str):
            sda_labels.add(m.group(1))
        for m in re.finditer(r'(lbl_[0-9A-Fa-f]+)@ha', ops_str):
            ha_labels.add(m.group(1))
        for m in re.finditer(r'(lbl_[0-9A-Fa-f]+)@l', ops_str):
            ha_labels.add(m.group(1))
        if entry[1] == 'bl':
            target = ops_str.strip()
            if not target.startswith('_') and target.startswith('fn_'):
                bl_targets.add(target)

    # Build declarations
    decl_lines = []

    # Labels already declared at file scope with specific types
    FILE_SCOPE_LABELS = {
        'lbl_80270440', 'lbl_80270460', 'lbl_80270480',
        'lbl_802704A0', 'lbl_80270528', 'lbl_8027056C',
        'lbl_802705C0', 'lbl_802705D0', 'lbl_80270610',
        'lbl_80400248', 'lbl_8047AA80', 'lbl_80400B28',
        'lbl_80400248',
    }
    # Labels declared as u32 (not arrays)
    U32_LABELS = {'lbl_8047AB08'}

    # Declare extern labels (skip file-scope ones)
    for lbl in sorted(sda_labels | ha_labels):
        if lbl in FILE_SCOPE_LABELS:
            continue
        if lbl in U32_LABELS:
            decl_lines.append(f'    extern u32 {lbl};')
        else:
            decl_lines.append(f'    extern u8 {lbl}[];')

    # Declare extern functions called
    for fn_target in sorted(bl_targets):
        decl_lines.append(f'    extern void {fn_target}();')

    # Also find jumptable and fn_ references used as addresses
    jt_set = set()
    fn_addr_set = set()
    has_bdnz = False
    extra_labels = set()
    for entry in insns:
        if entry[0] != 'inst':
            continue
        mnem = entry[1]
        ops_str = entry[2] if len(entry) > 2 else ''
        for m2 in re.finditer(r'(jumptable_[0-9A-Fa-f]+)', ops_str):
            jt_set.add(m2.group(1))
        # Check for fn_ used as address (not bl target)
        if entry[1] not in ('bl',):
            for m2 in re.finditer(r'(fn_[0-9A-Fa-f]+)@', ops_str):
                fn_addr_set.add(m2.group(1))
        if mnem == 'bdnz':
            has_bdnz = True
        # For bl calls, add implicit argument registers
        if mnem == 'bl':
            target = ops_str.strip()
            if target == 'memcpy' or target == 'memset':
                used_iregs.update({3, 4, 5})
            elif target.startswith('fn_') or target.startswith('_'):
                pass  # don't add implicit regs for general calls
            else:
                used_iregs.update({3, 4, 5})
        # For li rX, label@sda21, declare the label
        if mnem == 'li' and '@sda21' in ops_str:
            m2 = re.search(r'(\w+)@sda21', ops_str)
            if m2:
                extra_labels.add(m2.group(1))
    for jt in sorted(jt_set):
        decl_lines.append(f'    extern u8 {jt}[];')
    for fa in sorted(fn_addr_set):
        if fa not in bl_targets:
            decl_lines.append(f'    extern void {fa}();')
    for el in sorted(extra_labels):
        if el not in (sda_labels | ha_labels | FILE_SCOPE_LABELS | U32_LABELS):
            decl_lines.append(f'    extern u8 {el}[];')

    # Stack frame local variables (must come before r1)
    if frame_size > 0:
        decl_lines.append(f'    u8 sp[0x{frame_size:X}];')

    # Integer register declarations (include r0 as a temp)
    for rn in sorted(used_iregs):
        if rn == 1 and frame_size > 0:
            decl_lines.append(f'    u32 r1 = (u32)sp;')
        elif rn not in (1, 2):
            decl_lines.append(f'    u32 r{rn} = 0;')

    # Float register declarations
    for fn in sorted(used_fregs):
        if 0 <= fn <= 31:
            decl_lines.append(f'    f32 f{fn} = 0.0f;')

    # CTR variable for indirect calls
    if has_ctr_call or any(m == 'mtctr' for m, _ in real):
        decl_lines.append('    void (*ctr_fn)(void) = 0;')

    # CTR counter for bdnz loops
    if has_bdnz:
        decl_lines.append('    u32 ctr = 0;')

    # Second pass: generate code
    body_lines = []
    in_epilogue = False
    last_cmp_info = ('cmpwi', 'r0', '0')  # (type, lhs, rhs)

    for entry in insns:
        if entry[0] == 'label':
            label_name = entry[1].replace('.L_', 'L_')
            body_lines.append(f'{label_name}: ;')
            in_epilogue = False
            continue

        if entry[0] != 'inst':
            continue

        mnem = entry[1]
        ops_str = entry[2] if len(entry) > 2 else ''
        op = ops_list(ops_str) if ops_str else []

        # Skip prologue/epilogue
        if mnem == 'stwu' and op and op[0] == 'r1':
            continue
        if mnem == 'mflr':
            continue
        if mnem == 'mtlr':
            in_epilogue = True
            continue
        if in_epilogue and mnem == 'addi' and op and op[0] == 'r1':
            continue
        if mnem == 'blr':
            body_lines.append('    return;')
            in_epilogue = False
            continue

        # Stack saves/restores of callee-saved registers
        if mnem == 'stw' and len(op) >= 2:
            off, base = parse_mem(op[1])
            if base == 'r1' and isinstance(off, int) and off > 0:
                # Callee-saved register save or LR save
                src = op[0]
                if src.startswith('r') and src != 'r0':
                    continue  # skip saving callee-saved regs
                if src == 'r0' and off >= frame_size - 4:
                    continue  # LR save
                # Otherwise it's a stack local store
                body_lines.append(f'    *(u32*)(sp + 0x{off:X}) = {src};')
                continue

        if mnem == 'lwz' and len(op) >= 2:
            off, base = parse_mem(op[1])
            if base == 'r1' and isinstance(off, int):
                if in_epilogue and off > 0:
                    continue  # skip restoring callee-saved regs
                if off > 0 and off < frame_size:
                    # Stack local load
                    body_lines.append(f'    {op[0]} = *(u32*)(sp + 0x{off:X});')
                    continue
                if off > 0:
                    continue  # LR or register restore

        # _save/_rest register calls
        if mnem == 'bl' and (ops_str.startswith('_save') or ops_str.startswith('_rest')):
            continue

        # Track comparisons for condition codes
        if mnem == 'cmpwi':
            parts = op
            if len(parts) >= 2:
                last_cmp_expr = f'(s32){parts[0]} == {parts[1]}'
                last_cmp_info = ('cmpwi', parts[0], parts[1])
        elif mnem == 'cmplwi':
            parts = op
            if len(parts) >= 2:
                last_cmp_expr = f'(u32){parts[0]} == {parts[1]}'
                last_cmp_info = ('cmplwi', parts[0], parts[1])
        elif mnem == 'cmpw':
            parts = op
            if len(parts) >= 2:
                last_cmp_expr = f'(s32){parts[0]} == {parts[1]}'
                last_cmp_info = ('cmpw', parts[0], parts[1])
        elif mnem == 'cmplw':
            parts = op
            if len(parts) >= 2:
                last_cmp_expr = f'(u32){parts[0]} == {parts[1]}'
                last_cmp_info = ('cmplw', parts[0], parts[1])
        elif mnem == 'fcmpo' or mnem == 'fcmpu':
            parts = op
            if len(parts) >= 3:
                last_cmp_expr = f'{parts[1]} == {parts[2]}'
                last_cmp_info = ('fcmp', parts[1], parts[2])
            elif len(parts) >= 2:
                last_cmp_expr = f'{parts[0]} == {parts[1]}'
                last_cmp_info = ('fcmp', parts[0], parts[1])

        # Handle branch instructions with proper conditions
        if mnem in ('beq', 'bne', 'blt', 'bgt', 'ble', 'bge'):
            target = op[-1].strip().replace('.L_', 'L_')
            cmp_op = mnem[1:]  # eq, ne, lt, gt, le, ge
            cmp_type, lhs, rhs = last_cmp_info
            sign = '(s32)' if cmp_type in ('cmpwi', 'cmpw') else '(u32)' if cmp_type in ('cmplwi', 'cmplw') else ''

            op_map = {'eq': '==', 'ne': '!=', 'lt': '<', 'gt': '>', 'le': '<=', 'ge': '>='}
            c_op = op_map[cmp_op]
            body_lines.append(f'    if ({sign}{lhs} {c_op} {sign}{rhs}) goto {target};')
            continue

        if mnem in ('beqlr', 'bnelr', 'bltlr', 'bgtlr', 'blelr', 'bgelr'):
            cmp_op = mnem[1:-2]  # strip b and lr
            cmp_type, lhs, rhs = last_cmp_info
            sign = '(s32)' if cmp_type in ('cmpwi', 'cmpw') else '(u32)' if cmp_type in ('cmplwi', 'cmplw') else ''
            op_map = {'eq': '==', 'ne': '!=', 'lt': '<', 'gt': '>', 'le': '<=', 'ge': '>='}
            c_op = op_map.get(cmp_op, '==')
            body_lines.append(f'    if ({sign}{lhs} {c_op} {sign}{rhs}) return;')
            continue

        if mnem == 'b':
            target = op[0].strip().replace('.L_', 'L_')
            if target.startswith('L_'):
                body_lines.append(f'    goto {target};')
            else:
                body_lines.append(f'    /* b {target} */;')
            continue

        # MTR for indirect calls
        if mnem == 'mtctr':
            body_lines.append(f'    ctr_fn = (void(*)(void)){op[0]};')
            continue
        if mnem == 'bctrl':
            body_lines.append(f'    ctr_fn();')
            continue

        # Skip comparison instructions (already tracked above)
        if mnem in ('cmpwi', 'cmplwi', 'cmpw', 'cmplw', 'fcmpo', 'fcmpu'):
            continue

        # Generate the C statement
        c = translate_inst_v2(mnem, op, ops_str, frame_size)
        if c:
            body_lines.append(f'    {c}')

    # Combine declarations and body
    all_lines = decl_lines + [''] + body_lines if decl_lines else body_lines
    return ('void', '\n'.join(all_lines) + '\n', True)


def translate_inst_v2(mnem, op, ops_str, frame_size):
    """Translate instruction to C, using sp[] for stack frame."""

    # Stores to stack frame
    if mnem in ('stw', 'stb', 'sth', 'stfs', 'stfd') and len(op) >= 2:
        off, base = parse_mem(op[1])
        if base == 'r1' and isinstance(off, int) and off > 0:
            st = {'stw': 'u32', 'stb': 'u8', 'sth': 'u16', 'stfs': 'f32', 'stfd': 'f64'}[mnem]
            return f'*({st}*)(sp + 0x{off:X}) = {op[0]};'

    # Loads from stack frame
    if mnem in ('lwz', 'lbz', 'lhz', 'lfs', 'lfd') and len(op) >= 2:
        off, base = parse_mem(op[1])
        if base == 'r1' and isinstance(off, int) and off > 0 and off < frame_size:
            lt = {'lwz': 'u32', 'lbz': 'u8', 'lhz': 'u16', 'lfs': 'f32', 'lfd': 'f64'}[mnem]
            return f'{op[0]} = *({lt}*)(sp + 0x{off:X});'

    # Use the standard translator
    return translate_inst(mnem, op, ops_str)


def translate_inst(mnem, op, ops_str):
    """Translate a PPC instruction to a C statement."""

    if mnem == 'li':
        val = op[1]
        if '@sda21' in val:
            label = val.replace('@sda21', '')
            return f'{op[0]} = (u32){label};'
        return f'{op[0]} = {val};'
    if mnem == 'lis':
        if '@ha' in op[1]:
            label = op[1].replace('@ha', '')
            return f'{op[0]} = (u32){label};'
        return f'{op[0]} = ({op[1]} << 16);'
    if mnem == 'addi':
        if '@l' in op[2]:
            label = op[2].replace('@l', '')
            return f'{op[0]} = (u32){label};'
        if op[1] == 'r0':
            return f'{op[0]} = {op[2]};'
        return f'{op[0]} = {op[1]} + {op[2]};'
    if mnem == 'addic':
        return f'{op[0]} = {op[1]} + {op[2]};'
    if mnem == 'addis':
        return f'{op[0]} = {op[1]} + ({op[2]} << 16);'
    if mnem == 'subfic':
        return f'{op[0]} = {op[2]} - {op[1]};'

    # Load instructions
    if mnem in ('lwz', 'lbz', 'lhz', 'lwzu', 'lbzu', 'lhzu'):
        off, base = parse_mem(op[1])
        lt = {'lwz': 'u32', 'lbz': 'u8', 'lhz': 'u16',
              'lwzu': 'u32', 'lbzu': 'u8', 'lhzu': 'u16'}.get(mnem, 'u32')
        if isinstance(off, str):
            return f'{op[0]} = *({lt}*){off};'
        if isinstance(off, int):
            if off < 0:
                return f'{op[0]} = *({lt}*)((u8*){base} + ({off}));'
            return f'{op[0]} = *({lt}*)((u8*){base} + 0x{off:X});'

    if mnem in ('lfs', 'lfsu'):
        off, base = parse_mem(op[1])
        if isinstance(off, str):
            return f'{op[0]} = *(f32*){off};'
        if isinstance(off, int):
            if off < 0:
                return f'{op[0]} = *(f32*)((u8*){base} + ({off}));'
            return f'{op[0]} = *(f32*)((u8*){base} + 0x{off:X});'

    if mnem == 'lfd':
        off, base = parse_mem(op[1])
        if isinstance(off, str):
            return f'{op[0]} = *(f64*){off};'
        if isinstance(off, int):
            return f'{op[0]} = *(f64*)((u8*){base} + 0x{off:X});'

    if mnem == 'lha':
        off, base = parse_mem(op[1])
        if isinstance(off, int):
            return f'{op[0]} = *(s16*)((u8*){base} + 0x{off:X});'

    # Store instructions
    if mnem in ('stw', 'stb', 'sth'):
        off, base = parse_mem(op[1])
        st = {'stw': 'u32', 'stb': 'u8', 'sth': 'u16'}[mnem]
        if isinstance(off, str):
            return f'*({st}*){off} = {op[0]};'
        if isinstance(off, int):
            if off < 0:
                return f'*({st}*)((u8*){base} + ({off})) = {op[0]};'
            return f'*({st}*)((u8*){base} + 0x{off:X}) = {op[0]};'

    if mnem == 'stbu':
        off, base = parse_mem(op[1])
        if isinstance(off, str):
            return f'{base} = (u32){off}; *(u8*){base} = {op[0]};'
        if isinstance(off, int):
            return f'{base} += {off}; *(u8*){base} = {op[0]};'

    if mnem in ('stfs',):
        off, base = parse_mem(op[1])
        if isinstance(off, str):
            return f'*(f32*){off} = {op[0]};'
        if isinstance(off, int):
            if off < 0:
                return f'*(f32*)((u8*){base} + ({off})) = {op[0]};'
            return f'*(f32*)((u8*){base} + 0x{off:X}) = {op[0]};'

    if mnem == 'stfd':
        off, base = parse_mem(op[1])
        if isinstance(off, int):
            return f'*(f64*)((u8*){base} + 0x{off:X}) = {op[0]};'

    # Arithmetic
    if mnem == 'mr' or (mnem == 'or' and len(op) >= 3 and op[1] == op[2]):
        return f'{op[0]} = {op[1]};'
    if mnem == 'add':
        return f'{op[0]} = {op[1]} + {op[2]};'
    if mnem == 'addc':
        return f'{op[0]} = {op[1]} + {op[2]};'
    if mnem == 'adde':
        return f'{op[0]} = {op[1]} + {op[2]}; /* +carry */;'
    if mnem == 'subf':
        return f'{op[0]} = {op[2]} - {op[1]};'
    if mnem == 'subfc':
        return f'{op[0]} = {op[2]} - {op[1]};'
    if mnem == 'subfe':
        return f'{op[0]} = {op[2]} - {op[1]}; /* -borrow */;'
    if mnem == 'neg':
        return f'{op[0]} = -{op[1]};'
    if mnem == 'mulli':
        return f'{op[0]} = {op[1]} * {op[2]};'
    if mnem == 'mullw':
        return f'{op[0]} = {op[1]} * {op[2]};'
    if mnem == 'mulhw':
        return f'{op[0]} = (s32)((s64){op[1]} * (s64){op[2]} >> 32);'
    if mnem == 'mulhwu':
        return f'{op[0]} = (u32)((u64){op[1]} * (u64){op[2]} >> 32);'
    if mnem == 'divw':
        return f'{op[0]} = (s32){op[1]} / (s32){op[2]};'
    if mnem == 'divwu':
        return f'{op[0]} = (u32){op[1]} / (u32){op[2]};'

    # Logical
    if mnem == 'or':
        return f'{op[0]} = {op[1]} | {op[2]};'
    if mnem == 'ori':
        return f'{op[0]} = {op[1]} | {op[2]};'
    if mnem == 'oris':
        return f'{op[0]} = {op[1]} | ({op[2]} << 16);'
    if mnem == 'and':
        return f'{op[0]} = {op[1]} & {op[2]};'
    if mnem in ('andi.', 'andi'):
        return f'{op[0]} = {op[1]} & {op[2]};'
    if mnem == 'andis.':
        return f'{op[0]} = {op[1]} & ({op[2]} << 16);'
    if mnem == 'xor':
        return f'{op[0]} = {op[1]} ^ {op[2]};'
    if mnem == 'xori':
        return f'{op[0]} = {op[1]} ^ {op[2]};'
    if mnem == 'nor':
        return f'{op[0]} = ~({op[1]} | {op[2]});'
    if mnem == 'not':
        return f'{op[0]} = ~{op[1]};'
    if mnem == 'nand':
        return f'{op[0]} = ~({op[1]} & {op[2]});'
    if mnem == 'andc':
        return f'{op[0]} = {op[1]} & ~{op[2]};'
    if mnem == 'orc':
        return f'{op[0]} = {op[1]} | ~{op[2]};'

    # Shifts
    if mnem == 'slwi':
        return f'{op[0]} = {op[1]} << {op[2]};'
    if mnem == 'slw':
        return f'{op[0]} = {op[1]} << {op[2]};'
    if mnem == 'srwi':
        return f'{op[0]} = (u32){op[1]} >> {op[2]};'
    if mnem == 'srw':
        return f'{op[0]} = (u32){op[1]} >> {op[2]};'
    if mnem == 'srawi':
        return f'{op[0]} = (s32){op[1]} >> {op[2]};'
    if mnem == 'sraw':
        return f'{op[0]} = (s32){op[1]} >> {op[2]};'

    # Rotate/mask
    if mnem in ('clrlwi', 'clrlwi.'):
        n = int(op[2])
        mask = (1 << (32 - n)) - 1
        return f'{op[0]} = {op[1]} & 0x{mask:X};'
    if mnem in ('rlwinm', 'rlwinm.'):
        return f'/* rlwinm {ops_str} */;'
    if mnem == 'rlwimi':
        return f'/* rlwimi {ops_str} */;'
    if mnem == 'rotlwi':
        return f'/* rotlwi {ops_str} */;'

    # Extend
    if mnem == 'extsb':
        return f'{op[0]} = (s8){op[1]};'
    if mnem == 'extsh':
        return f'{op[0]} = (s16){op[1]};'
    if mnem == 'extsb.':
        return f'{op[0]} = (s8){op[1]};'

    # Compare
    if mnem == 'cmpwi':
        return f'/* cmpwi {ops_str} */;'
    if mnem == 'cmplwi':
        return f'/* cmplwi {ops_str} */;'
    if mnem == 'cmpw':
        return f'/* cmpw {ops_str} */;'
    if mnem == 'cmplw':
        return f'/* cmplw {ops_str} */;'

    # Branches
    if mnem == 'b':
        target = op[0].strip()
        if target.startswith('.L_'):
            return f'goto {target.replace(".L_", "L_")};'
        return f'/* b {target} */;'
    if mnem in ('beq', 'bne', 'blt', 'bgt', 'ble', 'bge'):
        target = op[-1].strip()
        cond = {'beq': 'eq', 'bne': 'ne', 'blt': 'lt', 'bgt': 'gt',
                'ble': 'le', 'bge': 'ge'}[mnem]
        if target.startswith('.L_'):
            return f'if (/* {cond} */) goto {target.replace(".L_", "L_")};'
        return f'/* {mnem} {ops_str} */;'
    if mnem in ('beqlr', 'bnelr', 'bltlr', 'bgtlr', 'blelr', 'bgelr'):
        cond = mnem[1:-2]  # strip b and lr
        return f'if (/* {cond} */) return;'
    if mnem == 'bdnz':
        target = op[0].strip().replace('.L_', 'L_')
        if target.startswith('L_'):
            return f'if (--ctr != 0) goto {target};'

    # Function calls
    if mnem == 'bl':
        target = op[0].strip()
        if target.startswith('_save') or target.startswith('_rest'):
            return f'/* {target} */;'
        # Well-known CRT functions
        if target == 'memcpy':
            return f'memcpy((void*)r3, (const void*)r4, (u32)r5);'
        if target == 'memset':
            return f'memset((void*)r3, (int)r4, (u32)r5);'
        # Generic function call
        return f'{target}();'
    if mnem == 'bctrl':
        return f'/* indirect call via ctr */;'
    if mnem == 'bctr':
        return f'/* indirect jump via ctr */;'

    # CTR/LR
    if mnem == 'mtctr':
        return f'/* mtctr {op[0]} */;'
    if mnem == 'mfctr':
        return f'{op[0]} = /* ctr */;'

    # Float ops
    if mnem == 'fmr':
        return f'{op[0]} = {op[1]};'
    if mnem == 'fadds':
        return f'{op[0]} = {op[1]} + {op[2]};'
    if mnem == 'fadd':
        return f'{op[0]} = {op[1]} + {op[2]};'
    if mnem == 'fsubs':
        return f'{op[0]} = {op[1]} - {op[2]};'
    if mnem == 'fsub':
        return f'{op[0]} = {op[1]} - {op[2]};'
    if mnem == 'fmuls':
        return f'{op[0]} = {op[1]} * {op[2]};'
    if mnem == 'fmul':
        return f'{op[0]} = {op[1]} * {op[2]};'
    if mnem == 'fdivs':
        return f'{op[0]} = {op[1]} / {op[2]};'
    if mnem == 'fdiv':
        return f'{op[0]} = {op[1]} / {op[2]};'
    if mnem == 'fneg':
        return f'{op[0]} = -{op[1]};'
    if mnem == 'fabs':
        return f'/* fabs */ {op[0]} = ({op[1]} < 0) ? -{op[1]} : {op[1]};'
    if mnem == 'fmadds':
        return f'{op[0]} = {op[1]} * {op[2]} + {op[3]};'
    if mnem == 'fmadd':
        return f'{op[0]} = {op[1]} * {op[2]} + {op[3]};'
    if mnem == 'fmsubs':
        return f'{op[0]} = {op[1]} * {op[2]} - {op[3]};'
    if mnem == 'fmsub':
        return f'{op[0]} = {op[1]} * {op[2]} - {op[3]};'
    if mnem == 'fnmsubs':
        return f'{op[0]} = -({op[1]} * {op[2]} - {op[3]});'
    if mnem == 'fnmsub':
        return f'{op[0]} = -({op[1]} * {op[2]} - {op[3]});'
    if mnem == 'fnmadds':
        return f'{op[0]} = -({op[1]} * {op[2]} + {op[3]});'
    if mnem == 'fctiwz':
        return f'{op[0]} = (f64)(s32){op[1]};'
    if mnem == 'frsp':
        return f'{op[0]} = (f32){op[1]};'
    if mnem == 'fcmpo':
        return f'/* fcmpo {ops_str} */;'
    if mnem == 'fcmpu':
        return f'/* fcmpu {ops_str} */;'
    if mnem == 'fsel':
        return f'{op[0]} = ({op[1]} >= 0) ? {op[2]} : {op[3]};'

    # CR ops
    if mnem == 'crclr':
        return f'/* crclr {ops_str} */;'
    if mnem == 'crset':
        return f'/* crset {ops_str} */;'
    if mnem == 'cror':
        return f'/* cror {ops_str} */;'
    if mnem == 'crandc':
        return f'/* crandc {ops_str} */;'
    if mnem == 'crorc':
        return f'/* crorc {ops_str} */;'
    if mnem == 'crand':
        return f'/* crand {ops_str} */;'

    # Paired singles
    if mnem.startswith('ps_'):
        return f'/* {mnem} {ops_str} */;'

    # Misc
    if mnem == 'mfcr':
        return f'{op[0]} = /* cr */;'
    if mnem == 'mtcrf':
        return f'/* mtcrf {ops_str} */;'
    if mnem == 'isync':
        return f'/* isync */;'
    if mnem == 'sync':
        return f'/* sync */;'
    if mnem == 'eieio':
        return f'/* eieio */;'
    if mnem == 'dcbi':
        return f'/* dcbi {ops_str} */;'
    if mnem == 'icbi':
        return f'/* icbi {ops_str} */;'

    # stmw/lmw
    if mnem == 'stmw':
        return f'/* stmw {ops_str} */;'
    if mnem == 'lmw':
        return f'/* lmw {ops_str} */;'

    return f'/* {mnem} {ops_str} */;'


def main():
    print("Loading assembly...", file=sys.stderr)
    asm_funcs = load_asm()
    print(f"Loaded {len(asm_funcs)} functions from assembly", file=sys.stderr)

    # Read the current C file
    with open(SRC_FILE, 'r') as f:
        c_text = f.read()

    # Parse the C file to find all stub blocks
    # Pattern: #pragma push ... void fn_XXX(...) { /* TODO */ } ... #pragma pop
    # or: void fn_XXX(...) { /* TODO: match ... */ }
    # We need to replace the function body between { and }

    lines = c_text.split('\n')
    output_lines = []
    i = 0
    stubs_replaced = 0
    stubs_failed = 0

    while i < len(lines):
        line = lines[i]

        # Check for pragma push block
        if line.strip() == '#pragma push':
            # Look ahead for the function signature and TODO body
            # Pattern:
            #   #pragma push
            #   #pragma optimization_level 0
            #   #pragma optimizewithasm off
            #   void fn_XXX(...) { /* TODO... */ }
            #   #pragma pop
            if (i + 4 < len(lines) and
                '#pragma optimization_level' in lines[i+1] and
                '#pragma optimizewithasm' in lines[i+2]):

                # Collect the function signature (may span multiple lines)
                # and the function body up to #pragma pop
                func_line = lines[i+3]
                sig_end = i + 3

                # If the function signature spans multiple lines
                # (i.e., doesn't contain '{' yet), keep reading
                while sig_end < len(lines) and '{' not in lines[sig_end]:
                    sig_end += 1

                # Now lines[i+3..sig_end] contain the full signature
                func_line = ' '.join(lines[j].strip() for j in range(i+3, sig_end+1))

                # Check for single-line or multi-line function body
                is_todo = False
                skip_count = 5  # default: push, opt, asm, func, pop

                if 'TODO' in func_line:
                    is_todo = True
                    # Find the closing }
                    j = sig_end
                    while j < len(lines) and '}' not in lines[j].split('{', 1)[-1]:
                        j += 1
                    # Account for cases where { and } are on same line
                    if '{' in func_line and '}' in func_line:
                        skip_count = sig_end + 1 - i + 1  # func line + pop
                    else:
                        skip_count = sig_end + 1 - i + 1  # default
                elif not func_line.rstrip().endswith('}'):
                    # Multi-line function body -- look for TODO in next lines
                    j = sig_end + 1
                    while j < len(lines) and lines[j].strip() != '}':
                        if 'TODO' in lines[j]:
                            is_todo = True
                        j += 1
                    # j now points to the closing '}'
                    # next line should be #pragma pop
                    if j + 1 < len(lines) and '#pragma pop' in lines[j+1]:
                        skip_count = j + 2 - i  # skip everything up to and including pop

                # Extract function name from the function line
                fn_match = re.search(r'(fn_[0-9A-Fa-f]+|GS\w+)', func_line)
                if fn_match and is_todo:
                    fn_name = fn_match.group(1)

                    # Map named functions to their fn_ addresses
                    name_map = {
                        'GSgfx_VBlankCallback': 'fn_800D3E4C',
                        'GSgfx_PreRetraceCallback': 'fn_800D3EC4',
                        'GSgfx_FrameEndCallback': 'fn_800D3F5C',
                        'GSgfx_BeginFrame': 'fn_800D3FA4',
                        'GSlog_PrintFormatted': 'fn_800D461C',
                        'GSlog_QueueCommand': 'fn_800D4F98',
                        'GSgfx_ConfigurePipeline': 'fn_800D892C',
                        'GSmaterial_Create': 'fn_800DE680',
                        'GSgfx_DrawDispatch': 'fn_800E1544',
                    }

                    asm_name = name_map.get(fn_name, fn_name)
                    result = decompile_function(asm_name, asm_funcs)

                    if result:
                        ret_type, body, needs_pragma = result

                        if needs_pragma:
                            # Keep pragma wrapping
                            output_lines.append('#pragma push')
                            output_lines.append('#pragma optimization_level 0')
                            output_lines.append('#pragma optimizewithasm off')

                        # Extract the function signature
                        sig_match = re.match(r'(.*?)\{', func_line)
                        if sig_match:
                            sig = sig_match.group(1).strip()
                            output_lines.append(f'{sig} {{')
                            for bline in body.rstrip('\n').split('\n'):
                                output_lines.append(bline)
                            output_lines.append('}')
                        else:
                            output_lines.append(func_line)

                        if needs_pragma:
                            output_lines.append('#pragma pop')

                        # Skip past the original block
                        i += skip_count
                        stubs_replaced += 1
                        continue
                    else:
                        stubs_failed += 1
                        # Keep original
                        pass

            # If we didn't replace, output as-is
            output_lines.append(line)
            i += 1
            continue

        output_lines.append(line)
        i += 1

    # Write the output
    result_text = '\n'.join(output_lines)
    with open(SRC_FILE, 'w') as f:
        f.write(result_text)

    print(f"Replaced {stubs_replaced} stubs, {stubs_failed} failed", file=sys.stderr)


if __name__ == '__main__':
    main()
