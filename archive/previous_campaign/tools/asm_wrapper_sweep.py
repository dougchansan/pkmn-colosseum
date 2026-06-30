#!/usr/bin/env python3
"""
asm_wrapper_sweep.py - Automated asm-wrapper sweep for stub functions.

For each void fn_8XXXXXXX(void) { stub (NOT preceded by #else):
1. Extract asm from the auto text .s file
2. Write .inc in same dir: <basename>_fn_8XXXXXXX.inc
3. Ensure file-scope extern decls exist for all bl targets
4. Wrap: #if 1 / asm void / #include / } / #else / original_stub / #endif
5. compile_check the file
6. match_test each function, report 100%

Usage:
    python tools/asm_wrapper_sweep.py src/crt/extras.c
    python tools/asm_wrapper_sweep.py src/game/gs_colsys.c
    python tools/asm_wrapper_sweep.py src/game/battle/battle_scene.c
"""

import re
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
ASM_FILE = PROJECT_ROOT / "build" / "GC6E01" / "asm" / "auto_01_800055E0_text.s"

# ---------------------------------------------------------------------------
# ASM file loading and indexing
# ---------------------------------------------------------------------------

def load_asm_index():
    """
    Load the ASM file and build an index: fn_name -> (start_lineno, end_lineno).
    start_lineno points to the '.fn ..., global' line (0-based).
    end_lineno points to the '.endfn ...' line.
    Returns (lines_list, index_dict).
    """
    # Build named SDA symbol table before processing
    _build_named_sda_table()

    print(f"Loading ASM file ({ASM_FILE.stat().st_size // (1024*1024)} MB)...")
    with open(ASM_FILE, "r", encoding="utf-8", errors="replace") as f:
        lines = f.readlines()
    print(f"  {len(lines)} lines loaded")

    index = {}
    cur_name = None
    cur_start = None
    for i, line in enumerate(lines):
        m = re.match(r'^\.fn\s+(\w+),', line)
        if m:
            cur_name = m.group(1)
            cur_start = i
        elif line.startswith('.endfn') and cur_name is not None:
            index[cur_name] = (cur_start, i)
            cur_name = None
            cur_start = None

    print(f"  {len(index)} functions indexed")
    return lines, index


# SDA address ranges (from instruction encoding analysis)
# Addresses 0x80478820 - 0x8047B680 use r13 (sdata/sbss)
# Addresses 0x8047B6A0 - 0x8047E734 use r2 (sdata2/sbss2)
SDA_R2_MIN = 0x8047B6A0

# SDA2 base register value (r2).
# Determined from binary: lfd f2, lbl_8047CCC8@sda21(r0) encodes as 0xC8429628
# => offset = 0x9628 signed = -27096 = 0x8047CCC8 - _SDA2_BASE_
# => _SDA2_BASE_ = 0x8047CCC8 + 27096 = 0x804836A0
# NOTE: CW asm{} does NOT support sym(r2) syntax for SDA2 symbols.
# Use numeric offset directly: sym_addr - _SDA2_BASE_
_SDA2_BASE = 0x804836A0

# Named (non-lbl_) SDA symbols with known addresses.
# Built from: auto_07_sdata.s, auto_08_sbss.s, auto_09_sdata2.s, auto_10_sbss2.s
_NAMED_SDA_SYMS = {}

def _build_named_sda_table():
    """Scan SDA section files to build symbol -> register lookup."""
    sda_files = [
        PROJECT_ROOT / 'build/GC6E01/asm/auto_07_80478820_sdata.s',
        PROJECT_ROOT / 'build/GC6E01/asm/auto_08_8047A7C8_sbss.s',
        PROJECT_ROOT / 'build/GC6E01/asm/auto_08_8047B6A0_sbss.s',
        PROJECT_ROOT / 'build/GC6E01/asm/auto_09_8047B6A0_sdata2.s',
        PROJECT_ROOT / 'build/GC6E01/asm/auto_10_8047E700_sbss2.s',
    ]
    obj_re = re.compile(
        r'# [^|]+\| (0x[0-9A-Fa-f]+) \| size: [^\n]+\n\.obj ([a-zA-Z_][a-zA-Z0-9_]*), global'
    )
    for fpath in sda_files:
        if not fpath.exists():
            continue
        text = fpath.read_text(encoding='utf-8', errors='replace')
        for m in obj_re.finditer(text):
            addr = int(m.group(1), 16)
            sym = m.group(2)
            if not sym.startswith('lbl_'):
                _NAMED_SDA_SYMS[sym] = 'r2' if addr >= SDA_R2_MIN else 'r13'


def _sda_reg(lbl_name):
    """Return 'r13' or 'r2' for the given symbol (lbl_ or named)."""
    # lbl_XXXX: use address
    m = re.match(r'lbl_([0-9A-Fa-f]+)', lbl_name)
    if m:
        addr = int(m.group(1), 16)
        return 'r2' if addr >= SDA_R2_MIN else 'r13'
    # Named symbol: look up in table
    if lbl_name in _NAMED_SDA_SYMS:
        return _NAMED_SDA_SYMS[lbl_name]
    return 'r13'


def _sda2_numeric_offset(sym):
    """Compute the numeric offset of an SDA2 symbol from _SDA2_BASE_ (r2).

    For lbl_XXXXXXXX: extract address from name.
    For named symbols: look up in _NAMED_SDA_SYMS table (address not stored there,
    so fall back to None if unknown).
    Returns integer offset (may be negative), or None if unknown.
    """
    m = re.match(r'lbl_([0-9A-Fa-f]+)', sym)
    if m:
        addr = int(m.group(1), 16)
        offset = addr - _SDA2_BASE
        # Sign-extend to 16 bits
        if offset > 0x7FFF:
            offset -= 0x10000
        elif offset < -0x8000:
            offset += 0x10000
        return offset
    # Named symbol: unknown address, cannot compute
    return None


def _fix_sda21(instr):
    """
    Convert @sda21 addressing to explicit register form.

    Both r13 (SDA) and r2 (SDA2) symbols use the symbolic form sym(r13/r2).
    MWCC CW asm{} supports sym(r13) and sym(r2) when the symbol is declared
    with a file-scope extern. This produces R_PPC_SDAREL16 or R_PPC_EMB_SDA21
    relocations matching the baseline object file.

    Requirements:
    - r13 symbols need: extern u32 sym;  (or matching type)
    - r2 symbols need:  extern f32/f64 sym;  (must match float type used)

    sym@sda21(r0) -> sym(r13) or sym(r2)   [symbolic form always]
    li rX, sym@sda21 -> la rX, sym(r13/r2) [symbolic form always]
    """
    # Handle load/store: sym@sda21(r0/r13/r2) -> sym(r13) or sym(r2)
    # For SDA2 (r2) CW asm{} does NOT accept sym(r2) (see note above); emit the
    # numeric offset (sym_addr - _SDA2_BASE_) instead. r13 keeps the symbolic form.
    def _ls_replacer(m):
        sym = m.group(1)
        reg = _sda_reg(sym)
        if reg == 'r2':
            off = _sda2_numeric_offset(sym)
            if off is not None:
                return f'{off}({reg})'
        return f'{sym}({reg})'
    instr = re.sub(r'([a-zA-Z_][a-zA-Z0-9_]*)@sda21\(r\d+\)', _ls_replacer, instr)

    # Handle: li rX, sym@sda21 -> la rX, sym(r13) or la rX, sym(r2)
    m = re.match(r'^(li\s+(r\d+),\s+)([a-zA-Z_][a-zA-Z0-9_]*)@sda21\s*$', instr)
    if m:
        rx = m.group(2)
        sym = m.group(3)
        reg = _sda_reg(sym)
        if reg == 'r2':
            off = _sda2_numeric_offset(sym)
            if off is not None:
                instr = f'la {rx}, {off}(r2)'
            else:
                instr = f'la {rx}, {sym}({reg})'
        else:
            instr = f'la {rx}, {sym}({reg})'

    return instr


_CR_BIT_NAMES = {
    'cr0lt': 0,  'cr0gt': 1,  'cr0eq': 2,  'cr0so': 3,
    'cr1lt': 4,  'cr1gt': 5,  'cr1eq': 6,  'cr1so': 7,
    'cr2lt': 8,  'cr2gt': 9,  'cr2eq': 10, 'cr2so': 11,
    'cr3lt': 12, 'cr3gt': 13, 'cr3eq': 14, 'cr3so': 15,
    'cr4lt': 16, 'cr4gt': 17, 'cr4eq': 18, 'cr4so': 19,
    'cr5lt': 20, 'cr5gt': 21, 'cr5eq': 22, 'cr5so': 23,
    'cr6lt': 24, 'cr6gt': 25, 'cr6eq': 26, 'cr6so': 27,
    'cr7lt': 28, 'cr7gt': 29, 'cr7eq': 30, 'cr7so': 31,
}


def _fix_cr_pseudo(instr):
    """
    Convert CR pseudo-instructions that CW asm doesn't support:
      crclr X -> crxor N, N, N
      crset X -> creqv N, N, N
    X can be a CR bit name (cr1eq) or numeric.
    """
    m = re.match(r'^(crclr|crset)\s+(\S+)', instr.strip())
    if not m:
        return instr
    op = m.group(1)
    operand = m.group(2).rstrip(',')
    if operand in _CR_BIT_NAMES:
        n = _CR_BIT_NAMES[operand]
    else:
        try:
            n = int(operand, 0)
        except ValueError:
            return instr
    if op == 'crclr':
        return f'crxor {n}, {n}, {n}'
    else:  # crset
        return f'creqv {n}, {n}, {n}'


def _sym_addr(sym):
    """Extract address from lbl_XXXXXXXX symbol names. Returns int or None."""
    m = re.match(r'^lbl_([0-9A-Fa-f]+)$', sym)
    if m:
        return int(m.group(1), 16)
    return None


def _fix_ha_l(instr):
    """
    Preserve sym@ha and sym@l as symbolic references for CW asm{} blocks.

    MWCC CW asm{} supports @ha/@l notation for symbols that have a file-scope
    extern declaration. Keeping them symbolic produces R_PPC_ADDR16_HA and
    R_PPC_ADDR16_LO relocations that match the baseline object file.

    Converting to numeric literals produces no relocations and causes objdiff
    mismatches even when the binary encoding is identical.

    Both lbl_XXXXXXXX and named symbols are left as-is.
    """
    # lbl_XXXXXXXX@ha and lbl_XXXXXXXX@l are kept symbolic
    # (extern u8 lbl_XXXXXXXX[] must be declared at file scope)
    return instr


def extract_inc_lines(fn_name, asm_lines, asm_index):
    """
    Extract and transform the ASM for fn_name into a list of strings
    suitable for writing into a .inc file.
    The first line is always '    nofralloc'.
    Instructions are indented with 4 spaces.
    Labels (@L_XXXXXXXX:) have no indent.
    Returns None if not found.
    """
    if fn_name not in asm_index:
        return None

    start, end = asm_index[fn_name]
    # Lines between .fn line and .endfn line (exclusive)
    raw_lines = asm_lines[start + 1 : end]

    out = ['    nofralloc']
    # Pattern for instruction line prefix: /* HHHHHHHH HHHHHHHH  HH HH HH HH */\t
    instr_prefix = re.compile(
        r'^/\* [0-9A-Fa-f]{8} [0-9A-Fa-f]{8}\s+(?:[0-9A-Fa-f]{2} ){4}\*/'
    )
    # Pattern for label lines: .L_HHHHHHHH:
    label_re = re.compile(r'^\.L_([0-9A-Fa-f]+):')

    for raw in raw_lines:
        line = raw.rstrip('\r\n')

        lm = label_re.match(line)
        if lm:
            out.append('@L_' + lm.group(1) + ':')
            continue

        pm = instr_prefix.match(line)
        if pm:
            instr = line[pm.end():].lstrip('\t').lstrip(' ')
            # Replace .L_ branch targets with @L_
            instr = re.sub(r'\.L_([0-9A-Fa-f]+)\b', r'@L_\1', instr)
            # Convert @sda21(r0) to explicit register form
            instr = _fix_sda21(instr)
            # Convert lbl_XXXXXXXX@ha / lbl_XXXXXXXX@l to numeric immediates
            instr = _fix_ha_l(instr)
            # Convert qrN register names to numeric (CW asm doesn't know qr0..qr7)
            instr = re.sub(r'\bqr([0-7])\b', r'\1', instr)
            # Convert CR bit pseudo-mnemonics: crclr X -> crxor N,N,N; crset X -> creqv N,N,N
            instr = _fix_cr_pseudo(instr)
            if instr.strip():
                out.append('    ' + instr)
        elif line.strip():
            # Any other non-empty line (directives etc) — pass through
            out.append('    ' + line.strip())
        # Empty lines are omitted to keep inc files clean

    return out


# ---------------------------------------------------------------------------
# C source file parsing
# ---------------------------------------------------------------------------

def find_stub_regions(src_text):
    """
    Find all stub function regions: void fn_8XXXXXXX(void) { ... }
    that are NOT preceded by #else (i.e. not already wrapped).

    A 'region' spans from the line immediately before the function (to include
    any #pragma push block) to the closing #pragma pop (if any), or just the
    closing brace.

    Returns list of dicts:
        fn_name, sig_line_start, region_start, region_end
    where start/end are character offsets in src_text.
    """
    results = []
    lines = src_text.split('\n')
    n = len(lines)
    # Reconstruct character offsets for each line start
    offsets = []
    pos = 0
    for l in lines:
        offsets.append(pos)
        pos += len(l) + 1  # +1 for the '\n'

    i = 0
    while i < n:
        line = lines[i]
        m = re.match(r'^void (fn_8[0-9A-Fa-f]+)\(void\)\s*\{', line)
        if m:
            fn_name = m.group(1)
            # Check if preceded by #else (within last 5 lines)
            ctx_start = max(0, i - 5)
            if any(lines[j].strip() == '#else' for j in range(ctx_start, i)):
                i += 1
                continue

            # Find the closing brace of this function
            depth = line.count('{') - line.count('}')
            j = i
            while j < n and depth > 0:
                j += 1
                if j < n:
                    depth += lines[j].count('{') - lines[j].count('}')
            fn_end_line = j  # line with closing '}'

            # Check for surrounding #pragma push / #pragma pop
            region_start_line = i
            region_end_line = fn_end_line

            # Look backward for #pragma push (within 5 lines)
            for k in range(i - 1, max(0, i - 6), -1):
                if lines[k].strip().startswith('#pragma push'):
                    region_start_line = k
                    break
                elif lines[k].strip() and not lines[k].strip().startswith('#pragma'):
                    break

            # Look forward for #pragma pop (within 3 lines after closing brace)
            for k in range(fn_end_line + 1, min(n, fn_end_line + 4)):
                if lines[k].strip().startswith('#pragma pop'):
                    region_end_line = k
                    break
                elif lines[k].strip():
                    break

            # character offsets
            region_char_start = offsets[region_start_line]
            region_char_end = offsets[region_end_line] + len(lines[region_end_line])

            results.append({
                'fn_name': fn_name,
                'sig_line': i,
                'region_start_line': region_start_line,
                'region_end_line': region_end_line,
                'region_char_start': region_char_start,
                'region_char_end': region_char_end,
                # The original region text (for #else block)
                'original_text': src_text[region_char_start : region_char_end],
            })
            i = region_end_line + 1
            continue
        i += 1

    return results


def get_bl_targets(inc_lines):
    """Return set of ALL symbol names in bl/blrl instructions."""
    targets = set()
    for line in inc_lines:
        m = re.search(r'\bbl\s+(\w+)\b', line)
        if m:
            targets.add(m.group(1))
    return targets


def get_lbl_sda_refs(inc_lines):
    """Return set of lbl_XXXX names used in SDA-relative addressing (r13/r2)."""
    refs = set()
    for line in inc_lines:
        for m in re.finditer(r'\b(lbl_[0-9A-Fa-f]+)\((r13|r2)\)', line):
            refs.add(m.group(1))
    return refs


def get_named_sda_refs(inc_lines):
    """Return set of named (non-lbl_) SDA symbols used in (r13/r2) form."""
    refs = set()
    for line in inc_lines:
        for m in re.finditer(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\((r13|r2)\)', line):
            sym = m.group(1)
            if not sym.startswith('lbl_') and not sym.startswith('fn_'):
                refs.add(sym)
    return refs


def get_lbl_ha_l_refs(inc_lines):
    """Return set of lbl_XXXX names used in @ha/@l form (need extern u8[] decl)."""
    refs = set()
    for line in inc_lines:
        for m in re.finditer(r'\b(lbl_[0-9A-Fa-f]+)@(?:ha|l)\b', line):
            refs.add(m.group(1))
    return refs


def get_jumptable_refs(inc_lines):
    """Return set of jumptable_XXXX names used in @ha/@l form (need extern u32[] decl)."""
    refs = set()
    for line in inc_lines:
        for m in re.finditer(r'\b(jumptable_[0-9A-Fa-f]+)@(?:ha|l)\b', line):
            refs.add(m.group(1))
    return refs


def get_fn_ha_l_refs(inc_lines):
    """Return set of fn_XXXX names used in @ha/@l form (need extern void fn() decl)."""
    refs = set()
    for line in inc_lines:
        for m in re.finditer(r'\b(fn_8[0-9A-Fa-f]+)@(?:ha|l)\b', line):
            refs.add(m.group(1))
    return refs


def find_existing_file_scope_externs(src_text):
    """
    Return set of symbol names that are EXPLICITLY extern-declared at the
    file scope (lines starting with 'extern ').
    Strips C line comments (// ...) and block-comment tails (/* ... */) before
    scanning, so identifiers that appear only in comments are not collected.
    """
    externs = set()
    for line in src_text.split('\n'):
        stripped = line.strip()
        if not stripped.startswith('extern '):
            continue
        # Strip C block/line comments before extracting identifiers
        no_comments = re.sub(r'/\*.*?\*/', '', stripped)  # /* ... */
        no_comments = re.sub(r'//.*$', '', no_comments)   # // ...
        for m in re.finditer(r'\b([A-Za-z_]\w*)\b', no_comments):
            externs.add(m.group(1))
    return externs


def collect_local_fn_prototypes(src_text):
    """
    Return dict: fn_name -> 'RETTYPE fn_NAME(PARAMS);' for all functions
    defined in the file that would conflict with 'extern void fn();'.
    This includes:
    - Functions with non-void return type
    - Functions with parameters (even if void return)
    Excludes: void fn_XXXX(void) stubs (which are being wrapped).
    """
    local_defs = {}
    fn_def_re = re.compile(
        r'^(?:static\s+)?'
        r'((?:BOOL|bool|void|s32|u32|s16|u16|s8|u8|f32|f64|int|char|short|long|void\s*\*'
        r'|unsigned\s+(?:int|long|short)|signed\s+(?:int|long|short))\s+)'
        r'(fn_8[0-9A-Fa-f]+)\s*\(([^)]*)\)\s*\{',
        re.MULTILINE
    )
    for m in fn_def_re.finditer(src_text):
        ret_type = m.group(1).strip()
        fn_name = m.group(2)
        params = m.group(3).strip()
        # Skip void fn(void) — those are the stubs being wrapped
        if ret_type == 'void' and params == 'void':
            continue
        # Skip void fn() — empty params also means stub
        if ret_type == 'void' and not params:
            continue
        local_defs[fn_name] = f'{ret_type} {fn_name}({params});'
    return local_defs


# ---------------------------------------------------------------------------
# File transformation
# ---------------------------------------------------------------------------

def build_wrapper(fn_name, original_text, inc_include_path):
    """
    Build the #if 1 / asm void / #include / } / #else / original / #endif block.
    """
    return (
        f'#if 1\n'
        f'asm void {fn_name}(void) {{\n'
        f'#include "{inc_include_path}"\n'
        f'}}\n'
        f'#else\n'
        f'{original_text}\n'
        f'#endif'
    )


def find_extern_insertion_point(src_text):
    """
    Find the best insertion point for new extern declarations:
    Just before the first function definition, or after the last existing
    extern/include block.
    """
    # Find the position of the first void fn_8 definition
    m = re.search(r'^void fn_8[0-9A-Fa-f]+\(', src_text, re.MULTILINE)
    if m:
        # Insert at the beginning of that line, after any preceding blank line
        pos = m.start()
        return pos
    return len(src_text)


# ---------------------------------------------------------------------------
# Run tools
# ---------------------------------------------------------------------------

def run_compile_check(src_path):
    result = subprocess.run(
        [sys.executable, 'tools/compile_check.py',
         str(src_path.relative_to(PROJECT_ROOT))],
        capture_output=True, text=True, cwd=str(PROJECT_ROOT)
    )
    return result.returncode == 0, result.stdout + result.stderr


def run_match_test(fn_name):
    result = subprocess.run(
        [sys.executable, 'tools/match_test.py', fn_name],
        capture_output=True, text=True, cwd=str(PROJECT_ROOT)
    )
    output = result.stdout + result.stderr
    # Extract match percentage
    m = re.search(r'\[([#\-]+)\]\s+([\d.]+)%', output)
    if m:
        pct = float(m.group(2))
        return pct, output
    if 'MATCHING' in output or '100%' in output:
        return 100.0, output
    return None, output


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def process_file(src_path):
    src_path = src_path.resolve()
    if not src_path.exists():
        print(f"ERROR: {src_path} not found")
        sys.exit(1)

    basename = src_path.stem
    src_dir = src_path.parent
    try:
        src_rel_dir = src_dir.relative_to(PROJECT_ROOT)
    except ValueError:
        src_rel_dir = src_dir

    # Load ASM
    asm_lines, asm_index = load_asm_index()

    print(f"\n{'='*65}")
    print(f"Processing: {src_path.relative_to(PROJECT_ROOT)}")
    print('='*65)

    # Read source
    with open(src_path, 'r', encoding='utf-8') as f:
        src_text = f.read()

    # Find all stubs
    stubs = find_stub_regions(src_text)
    print(f"Found {len(stubs)} void fn_8XXXXXXX(void) stubs\n")

    if not stubs:
        print("Nothing to wrap.")
        return []

    # For each stub: extract asm, write .inc, collect bl targets
    stub_data = []
    all_new_externs = set()
    for stub in stubs:
        fn_name = stub['fn_name']
        inc_lines = extract_inc_lines(fn_name, asm_lines, asm_index)
        if inc_lines is None:
            print(f"  WARNING: {fn_name} not found in ASM file, skipping")
            stub['skip'] = True
            continue
        stub['skip'] = False
        stub['inc_lines'] = inc_lines
        bl_targets = get_bl_targets(inc_lines)
        lbl_sda_refs = get_lbl_sda_refs(inc_lines)
        lbl_ha_l_refs = get_lbl_ha_l_refs(inc_lines)
        jumptable_refs = get_jumptable_refs(inc_lines)
        fn_ha_l_refs = get_fn_ha_l_refs(inc_lines)
        named_sda_refs = get_named_sda_refs(inc_lines)
        stub['bl_targets'] = bl_targets
        stub['lbl_sda_refs'] = lbl_sda_refs
        stub['lbl_ha_l_refs'] = lbl_ha_l_refs
        stub['jumptable_refs'] = jumptable_refs
        stub['fn_ha_l_refs'] = fn_ha_l_refs
        stub['named_sda_refs'] = named_sda_refs

        # Write .inc file
        inc_name = f'{basename}_{fn_name}.inc'
        inc_path = src_dir / inc_name
        inc_content = '\n'.join(inc_lines) + '\n'
        with open(inc_path, 'w', encoding='utf-8', newline='\n') as f:
            f.write(inc_content)
        print(f"  Wrote {inc_name}")
        stub['inc_path'] = inc_path
        stub['inc_include_path'] = str(src_rel_dir / inc_name).replace('\\', '/')
        stub_data.append(stub)

    # Determine which bl targets and lbl_ SDA symbols need new file-scope extern declarations
    existing_externs = find_existing_file_scope_externs(src_text)
    # Collect all internal fn names (void stubs being wrapped)
    internal_fns = set(s['fn_name'] for s in stubs)
    # Collect local non-void function prototypes (for proper forward decls)
    local_fn_protos = collect_local_fn_prototypes(src_text)

    all_new_fn_externs = set()         # fn/named symbols needing extern void decl
    all_new_fn_typed_externs = {}      # fn -> proto string for non-void local defs
    all_new_lbl_u32_externs = set()    # lbl_ symbols needing extern u32 decl (SDA)
    all_new_lbl_arr_externs = set()    # lbl_ symbols needing extern u8[] decl (@ha/@l)
    all_new_jumptable_externs = set()  # jumptable_ symbols needing extern u32[] decl
    all_new_named_sda_externs = set()  # named SDA symbols needing extern u32 decl

    def _add_fn_extern(fn_name):
        if fn_name in existing_externs:
            return
        if fn_name in internal_fns:
            # This is a void(void) stub being wrapped in this file.
            # Add a forward declaration so asm blocks that reference it before
            # its definition in the file don't get "undefined label" errors.
            all_new_fn_typed_externs[fn_name] = f'void {fn_name}(void);'
            return
        if fn_name in local_fn_protos:
            # This is a non-void or parameterized function defined later in this file
            # We need a proper typed forward declaration, not extern void
            all_new_fn_typed_externs[fn_name] = local_fn_protos[fn_name]
        elif re.match(r'^fn_8[0-9A-Fa-f]+$', fn_name):
            # Only add extern void for project-internal fn_8XXXXXXX stubs.
            # Other names (OS*, GX*, SI*, etc.) come from headers — don't redeclare them.
            all_new_fn_externs.add(fn_name)

    for stub in stub_data:
        for t in stub['bl_targets']:
            _add_fn_extern(t)
        for lbl in stub['lbl_sda_refs']:
            if lbl not in existing_externs:
                all_new_lbl_u32_externs.add(lbl)
        for lbl in stub['lbl_ha_l_refs']:
            if lbl not in existing_externs:
                all_new_lbl_arr_externs.add(lbl)
        for jt in stub['jumptable_refs']:
            if jt not in existing_externs:
                all_new_jumptable_externs.add(jt)
        for fn_hl in stub['fn_ha_l_refs']:
            _add_fn_extern(fn_hl)
        for sym in stub.get('named_sda_refs', set()):
            if sym not in existing_externs:
                all_new_named_sda_externs.add(sym)

    all_new_externs = (all_new_fn_externs | set(all_new_fn_typed_externs.keys()) |
                       all_new_lbl_u32_externs | all_new_lbl_arr_externs |
                       all_new_jumptable_externs | all_new_named_sda_externs)

    # Apply all wrappers in REVERSE order (to preserve char offsets)
    # and insert new extern decls
    stub_data_sorted = sorted(stub_data, key=lambda s: s['region_char_start'], reverse=True)
    modified = src_text
    for stub in stub_data_sorted:
        wrapper = build_wrapper(
            stub['fn_name'],
            stub['original_text'],
            stub['inc_include_path']
        )
        rs = stub['region_char_start']
        re_end = stub['region_char_end']
        modified = modified[:rs] + wrapper + modified[re_end:]

    # Add new file-scope extern declarations before the first #if 1 wrapper
    if all_new_externs:
        insertion_re = re.search(r'^#if 1', modified, re.MULTILINE)
        if insertion_re:
            insert_pos = insertion_re.start()
            decl_lines = []
            # u8[] externs for @ha/@l labels (array-style)
            for lbl in sorted(all_new_lbl_arr_externs - all_new_lbl_u32_externs):
                decl_lines.append(f'extern u8 {lbl}[];')
            # u32 externs for SDA labels
            for lbl in sorted(all_new_lbl_u32_externs):
                decl_lines.append(f'extern u32 {lbl};')
            # u32[] externs for jumptable labels
            for jt in sorted(all_new_jumptable_externs):
                decl_lines.append(f'extern u32 {jt}[];')
            # void extern for bl targets and fn@ha/fn@l references
            for fn in sorted(all_new_fn_externs):
                decl_lines.append(f'extern void {fn}();')
            # typed forward decls for non-void functions defined later in this file
            for fn, proto in sorted(all_new_fn_typed_externs.items()):
                decl_lines.append(proto)
            # u32 externs for named SDA symbols (scalar, for (r13)/(r2) addressing)
            for sym in sorted(all_new_named_sda_externs):
                decl_lines.append(f'extern u32 {sym};')
            decls = '\n'.join(decl_lines)
            modified = modified[:insert_pos] + decls + '\n' + modified[insert_pos:]
            print(f"\n  Added file-scope externs: "
                  f"{len(all_new_lbl_arr_externs - all_new_lbl_u32_externs)} lbl_[]  "
                  f"{len(all_new_lbl_u32_externs)} lbl_u32  "
                  f"{len(all_new_jumptable_externs)} jumptable  "
                  f"{len(all_new_fn_externs)} fn  "
                  f"{len(all_new_named_sda_externs)} named_sda")

    # Write the modified file
    with open(src_path, 'w', encoding='utf-8', newline='\n') as f:
        f.write(modified)
    print(f"\n  Wrote {src_path.name}")

    # Compile check
    print(f"\n  Running compile_check...")
    ok, output = run_compile_check(src_path)
    if not ok:
        print(f"  COMPILE FAILED:")
        print(output[-800:])
        print("\nAborting — please fix compile errors before running match_test.")
        return stub_data

    print(f"  Compile OK")

    # match_test for each function
    print(f"\n  Running match_test for each function...")
    results = []
    for stub in stub_data:
        fn_name = stub['fn_name']
        pct, mt_output = run_match_test(fn_name)
        if pct == 100.0:
            print(f"  ✓ {fn_name}: 100%")
            results.append((fn_name, '100%'))
        elif pct is not None:
            print(f"  ✗ {fn_name}: {pct:.1f}%")
            results.append((fn_name, f'{pct:.1f}%'))
        else:
            print(f"  ? {fn_name}: unknown")
            results.append((fn_name, 'unknown'))

    # Summary
    ok_count = sum(1 for _, r in results if r == '100%')
    print(f"\n{'='*65}")
    print(f"SUMMARY: {src_path.name}")
    print(f"  Wrapped: {len(stub_data)}, 100% match: {ok_count}/{len(results)}")
    non_100 = [(fn, r) for fn, r in results if r != '100%']
    if non_100:
        print(f"  Non-100% functions:")
        for fn, r in non_100:
            print(f"    {fn}: {r}")
    print('='*65)

    return results


def main():
    if len(sys.argv) < 2:
        print("Usage: python tools/asm_wrapper_sweep.py <src_file.c>")
        sys.exit(1)

    src_path = Path(sys.argv[1])
    if not src_path.is_absolute():
        src_path = PROJECT_ROOT / src_path

    process_file(src_path)


if __name__ == '__main__':
    main()
