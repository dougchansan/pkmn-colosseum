#!/usr/bin/env python3
"""
Phase 3b: Switch-case reconstruction from binary search dispatch gotos.

Primary transformations:
1. Binary search dispatch trees -> switch statements (highest impact)
2. if { ... goto L; } code; L: -> if { ... } else { code; }
3. Fallthrough goto removal, unreachable code removal, condition combining
"""
import re
import sys
import os
import subprocess
import shutil


def count_gotos(text):
    if isinstance(text, list):
        return sum(len(re.findall(r'\bgoto\s+L_[0-9A-Fa-f]+\s*;', l)) for l in text)
    return len(re.findall(r'\bgoto\s+L_[0-9A-Fa-f]+\s*;', text))


def compile_check(filepath, cv=None):
    try:
        cmd = [sys.executable, 'tools/compile_check.py', filepath]
        if cv:
            cmd += ['--compiler-version', cv]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        return result.returncode == 0
    except Exception:
        return False


def build_label_info(lines):
    label_pos = {}
    label_refs = {}
    for i, line in enumerate(lines):
        m = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:', line)
        if m:
            label_pos[m.group(1)] = i
        for gm in re.finditer(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', line):
            lbl = gm.group(1)
            label_refs.setdefault(lbl, []).append(i)
    return label_pos, label_refs


def count_label_refs(lines, label):
    count = 0
    for line in lines:
        count += len(re.findall(r'\bgoto\s+' + re.escape(label) + r'\s*;', line))
    return count


def cleanup_unreferenced_labels(lines):
    changes = 0
    label_pos, _ = build_label_info(lines)
    for label, pos in list(label_pos.items()):
        if count_label_refs(lines, label) == 0:
            s = lines[pos].strip()
            if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                lines[pos] = ''
                changes += 1
    return changes


# =====================================================================
# PASS: Binary search dispatch tree -> switch statement
# =====================================================================

def _parse_dispatch_tree(lines, start_idx):
    """Parse a binary search dispatch tree starting at start_idx.

    Returns (dispatch_var, case_map, default_labels, tree_end, case_body_region)
    where case_map is {value: label} and tree_end is the line after the tree.
    """
    # Find the dispatch variable by looking for comparison patterns
    # Pattern: if ((s32)VAR op (s32)VALUE) ...
    comp_re = re.compile(
        r'if\s+\(\(s32\)([\w*()>.\-+\[\]]+)\s*'
        r'(==|!=|<|>|<=|>=)\s*'
        r'\(s32\)(0x[0-9A-Fa-f]+|-?[0-9]+)\)'
    )

    # Scan to find the common comparison variable
    var_counts = {}
    for i in range(start_idx, min(start_idx + 20, len(lines))):
        for m in comp_re.finditer(lines[i]):
            var = m.group(1)
            var_counts[var] = var_counts.get(var, 0) + 1

    if not var_counts:
        return None

    dispatch_var = max(var_counts, key=var_counts.get)
    if var_counts[dispatch_var] < 3:
        return None

    # Now parse the full dispatch tree
    # Track: value -> label for == comparisons
    # Track: range info from < and >= comparisons
    case_map = {}  # value (int) -> label
    default_labels = []  # labels reached by unconditional/range gotos
    tree_lines = set()
    goto_targets = set()

    # Determine the nesting depth at the start
    base_indent = len(lines[start_idx]) - len(lines[start_idx].lstrip())

    i = start_idx
    max_line = min(start_idx + 500, len(lines))
    brace_depth = 0

    while i < max_line:
        s = lines[i].strip()

        if not s:
            i += 1
            continue

        # Track braces
        opens = s.count('{')
        closes = s.count('}')
        brace_depth += opens - closes

        # Stop if we've come back to base level and hit a label
        if brace_depth <= 0 and re.match(r'^L_[0-9A-Fa-f]+\s*:', s):
            break

        # Stop if we've come back to base level and hit non-dispatch code
        current_indent = len(lines[i]) - len(lines[i].lstrip())
        if brace_depth <= 0 and current_indent <= base_indent:
            if not re.search(r'\(s32\)' + re.escape(dispatch_var), s):
                if not re.match(r'^\s*goto\s+L_', s):
                    if s != '{' and s != '}' and s != 'return;':
                        break

        # Extract equality comparisons: if (var == val) goto L;
        eq_m = re.search(
            r'if\s+\(\(s32\)' + re.escape(dispatch_var) +
            r'\s*==\s*\(s32\)(0x[0-9A-Fa-f]+|-?[0-9]+)\)\s*goto\s+(L_[0-9A-Fa-f]+)',
            s
        )
        if eq_m:
            try:
                val = int(eq_m.group(1), 0)
                label = eq_m.group(2)
                case_map[val] = label
                goto_targets.add(label)
                tree_lines.add(i)
            except ValueError:
                pass
            i += 1
            continue

        # Extract range boundary gotos that indicate case values
        # if (var >= val) goto L; means val is a case value in some contexts
        range_m = re.search(
            r'if\s+\(\(s32\)' + re.escape(dispatch_var) +
            r'\s*>=\s*\(s32\)(0x[0-9A-Fa-f]+|-?[0-9]+)\)\s*goto\s+(L_[0-9A-Fa-f]+)',
            s
        )
        if range_m:
            try:
                val = int(range_m.group(1), 0)
                label = range_m.group(2)
                # In binary search trees, >= X goto L means X is a case
                # (specifically the case for values >= X in this subtree)
                case_map[val] = label
                goto_targets.add(label)
                tree_lines.add(i)
            except ValueError:
                pass
            i += 1
            continue

        # Bare goto (default/fallthrough in tree)
        bare_goto = re.match(r'^\s*goto\s+(L_[0-9A-Fa-f]+)\s*;$', s)
        if bare_goto:
            label = bare_goto.group(1)
            goto_targets.add(label)
            default_labels.append(label)
            tree_lines.add(i)
            i += 1
            continue

        # return in dispatch tree (some case values cause early return)
        if s == 'return;':
            tree_lines.add(i)
            i += 1
            continue

        # Other comparison patterns (!=, <) are just tree structure
        if re.search(r'\(s32\)' + re.escape(dispatch_var), s):
            tree_lines.add(i)
            i += 1
            continue

        # Braces
        if s in ('{', '}'):
            tree_lines.add(i)
            i += 1
            continue

        # Unknown line - stop
        break

    tree_end = i
    if len(case_map) < 3:
        return None

    return dispatch_var, case_map, default_labels, tree_end, tree_lines, goto_targets


def _find_case_bodies(lines, goto_targets, tree_start_line):
    """Find the case body regions for each label target.

    The decompiler interleaves case bodies with the binary search tree's
    closing braces. Case bodies can appear:
    1. After named labels (L_XXXX:) that are goto targets
    2. Between closing braces in the tree's unwind region (unnamed cases
       reached by fall-through from != branches)

    Returns (case_bodies_dict, region_end_line)
    where case_bodies_dict is: label -> (body_start, body_end, body_lines)
    and region_end_line is the line after the last case body.
    """
    label_pos, _ = build_label_info(lines)
    case_bodies = {}

    # Sort target labels by position
    sorted_targets = sorted(
        [(label, label_pos[label]) for label in goto_targets if label in label_pos],
        key=lambda x: x[1]
    )

    # Find the last case body label position
    max_body_end = tree_start_line

    for idx, (label, pos) in enumerate(sorted_targets):
        # Find the end of this case body
        body_start = pos + 1
        body_end = body_start

        for k in range(body_start, len(lines)):
            sk = lines[k].strip()
            if not sk:
                body_end = k + 1
                continue
            # Closing brace is part of the tree's unwind - skip
            if sk == '}':
                body_end = k + 1
                continue
            # Next label = end of this case body
            if re.match(r'^L_[0-9A-Fa-f]+\s*:', sk):
                body_end = k
                break
            # Return = end of case body (common pattern)
            if sk == 'return;':
                body_end = k + 1
                break
            # Break = end of case body
            if sk == 'break;':
                body_end = k + 1
                break
            body_end = k + 1

        # Filter body lines: remove closing braces (tree unwind artifacts)
        raw_body = []
        for k in range(body_start, body_end):
            sk = lines[k].strip()
            if sk == '}':
                continue
            raw_body.append(lines[k])

        case_bodies[label] = (body_start, body_end, raw_body)
        if body_end > max_body_end:
            max_body_end = body_end

    return case_bodies, max_body_end


def pass_dispatch_tree_to_switch(lines):
    """Convert binary search dispatch trees to switch statements.

    The decompiler generates deeply nested if/else with gotos for switch
    statements. The structure is:
    1. Binary search tree (comparisons, gotos to case labels)
    2. Interleaved closing braces from the tree nesting + case body code
    3. Labeled case bodies (L_XXXX: code; return;)

    We replace the ENTIRE region (tree + interleaved braces + case bodies)
    with a clean switch statement.
    """
    changes = 0
    label_pos, label_refs = build_label_info(lines)

    comp_re = re.compile(
        r'if\s+\(\(s32\)([\w*()>.\-+\[\]]+)\s*'
        r'(==|!=|<|>|<=|>=)\s*'
        r'\(s32\)(0x[0-9A-Fa-f]+|-?[0-9]+)\)'
    )

    i = 0
    while i < len(lines):
        s = lines[i].strip()
        if not comp_re.search(s):
            i += 1
            continue

        result = _parse_dispatch_tree(lines, i)
        if result is None:
            i += 1
            continue

        dispatch_var, case_map, default_labels, tree_end, tree_lines, goto_targets = result

        if len(case_map) < 5:
            i = tree_end
            continue

        # Find case bodies (handles interleaved braces)
        case_bodies, region_end = _find_case_bodies(lines, goto_targets, i)

        # Build the set of lines to blank:
        first_tree_line = min(tree_lines)
        lines_to_blank = set(tree_lines)

        # Add case body lines (label + body)
        for label, (bs, be, body) in case_bodies.items():
            if label in label_pos:
                lines_to_blank.add(label_pos[label])
            for k in range(bs, be):
                lines_to_blank.add(k)

        # Add empty/semicolon lines in the region
        if lines_to_blank:
            region_start = min(lines_to_blank)
            region_end_line = max(lines_to_blank)
            for k in range(region_start, region_end_line + 1):
                if k in lines_to_blank:
                    continue
                sk = lines[k].strip()
                if not sk or sk == ';':
                    lines_to_blank.add(k)

        # Safety check: verify there's no "orphan" code in the blank region
        # that isn't part of any labeled case body or the tree itself.
        # Orphan code = non-blank, non-brace code that isn't in lines_to_blank.
        has_orphan = False
        if lines_to_blank:
            region_start = min(lines_to_blank)
            region_end_line = max(lines_to_blank)
            for k in range(region_start, region_end_line + 1):
                if k in lines_to_blank:
                    continue
                sk = lines[k].strip()
                if not sk or sk == ';' or sk == '}':
                    lines_to_blank.add(k)  # add structural line
                    continue
                # Real code that's not in our blank set = orphan
                has_orphan = True
                break

        if has_orphan:
            # This tree has interleaved unnamed case bodies - too complex
            i = tree_end
            continue

        # Brace balance check
        blank_opens = sum(lines[k].count('{') for k in lines_to_blank if k < len(lines))
        blank_closes = sum(lines[k].count('}') for k in lines_to_blank if k < len(lines))

        if blank_opens != blank_closes:
            # Unbalanced - skip this tree
            i = tree_end
            continue

        # Build the switch statement
        base_indent = ''
        for tl in sorted(tree_lines):
            indent_m = re.match(r'^(\s*)', lines[tl])
            if indent_m:
                ind = indent_m.group(1)
                if not base_indent or len(ind) < len(base_indent):
                    base_indent = ind
        if not base_indent:
            base_indent = '    '

        case_indent = base_indent + '    '
        body_indent = base_indent + '        '

        switch_lines = [base_indent + f'switch ((s32){dispatch_var}) {{']

        for val, label in sorted(case_map.items()):
            if val < 0:
                val_str = f'-0x{abs(val):x}'
            else:
                val_str = f'0x{val:x}'

            if label in case_bodies:
                bs, be, body = case_bodies[label]
                switch_lines.append(case_indent + f'case {val_str}:')
                for bl in body:
                    bl_stripped = bl.strip()
                    if bl_stripped:
                        switch_lines.append(body_indent + bl_stripped)
                last_real = ''
                for bl in reversed(body):
                    if bl.strip():
                        last_real = bl.strip()
                        break
                if last_real != 'return;' and last_real != 'break;' and \
                   not re.match(r'^goto\s+', last_real):
                    switch_lines.append(body_indent + 'break;')
            else:
                switch_lines.append(case_indent + f'case {val_str}:')
                switch_lines.append(body_indent + 'break;')

        if default_labels:
            switch_lines.append(case_indent + 'default:')
            for dl in default_labels:
                if dl in case_bodies:
                    bs, be, body = case_bodies[dl]
                    for bl in body:
                        bl_stripped = bl.strip()
                        if bl_stripped:
                            switch_lines.append(body_indent + bl_stripped)
                    break
            else:
                switch_lines.append(body_indent + 'break;')

        switch_lines.append(base_indent + '}')

        # Blank all identified lines
        for k in sorted(lines_to_blank):
            if k < len(lines):
                lines[k] = ''

        # Insert the switch at the first tree line
        lines[first_tree_line] = '\n'.join(switch_lines)

        changes += len(case_map) + len(default_labels)
        i = max(lines_to_blank) + 1 if lines_to_blank else tree_end
        label_pos, label_refs = build_label_info(lines)

    return changes


# =====================================================================
# PASS: if-goto-else conversion (single-ref labels)
# =====================================================================
def pass_if_goto_else(lines):
    changes = 0
    label_pos, label_refs = build_label_info(lines)

    for i in range(len(lines) - 1, -1, -1):
        gm = re.match(r'^(\s*)goto\s+(L_[0-9A-Fa-f]+)\s*;$', lines[i])
        if not gm:
            continue

        indent = gm.group(1)
        target = gm.group(2)
        if target not in label_pos:
            continue
        target_pos = label_pos[target]
        if target_pos <= i:
            continue

        refs = count_label_refs(lines, target)
        if refs != 1:
            continue

        close_brace_line = None
        for k in range(i + 1, min(i + 5, len(lines))):
            sk = lines[k].strip()
            if not sk:
                continue
            if sk == '}':
                close_brace_line = k
            break

        if close_brace_line is None:
            continue

        # Also check the brace line itself - make sure it's just '}'
        if lines[close_brace_line].strip() != '}':
            continue

        # Check that the closing brace isn't closing an else block.
        # Search backward from the } to find the matching {.
        # If that { is part of 'else {' or '} else {', skip.
        brace_count = 0
        in_else = False
        for k in range(close_brace_line, max(close_brace_line - 200, -1), -1):
            # Process braces right-to-left on this line
            for ch in reversed(lines[k]):
                if ch == '}':
                    brace_count += 1
                elif ch == '{':
                    brace_count -= 1
                    if brace_count == 0:
                        # Found the matching {
                        if 'else' in lines[k]:
                            in_else = True
                        break
            if brace_count == 0:
                break
        if in_else:
            continue

        # Also check: the next non-empty line after } shouldn't start with 'else'
        next_after_brace = close_brace_line + 1
        while next_after_brace < len(lines) and not lines[next_after_brace].strip():
            next_after_brace += 1
        if next_after_brace < len(lines):
            nab = lines[next_after_brace].strip()
            if nab.startswith('else') or nab.startswith('} else'):
                continue

        else_start = close_brace_line + 1
        else_end = target_pos

        actual_else_start = else_start
        while actual_else_start < else_end and not lines[actual_else_start].strip():
            actual_else_start += 1

        if actual_else_start >= else_end:
            lines[i] = ''
            lines[target_pos] = ''
            changes += 1
            label_pos, label_refs = build_label_info(lines)
            continue

        has_label = False
        has_case = False
        for k in range(else_start, else_end):
            sk = lines[k].strip()
            if not sk or sk == ';':
                continue
            if re.match(r'^L_[0-9A-Fa-f]+\s*:', sk):
                has_label = True
                break
            if sk.startswith('case ') or sk.startswith('default:'):
                has_case = True
                break

        if has_label or has_case:
            continue

        brace_depth = 0
        for k in range(else_start, else_end):
            brace_depth += lines[k].count('{') - lines[k].count('}')
        if brace_depth != 0:
            continue

        lines[i] = ''
        lines[close_brace_line] = lines[close_brace_line].replace('}', '} else {', 1)
        brace_indent = re.match(r'^(\s*)', lines[close_brace_line]).group(1)
        lines[target_pos] = brace_indent + '}'

        changes += 1
        label_pos, label_refs = build_label_info(lines)

    return changes


# =====================================================================
# PASS: multi-ref goto removal (fallthrough only)
# =====================================================================
def pass_if_goto_else_multiref(lines):
    """For multi-ref labels, only remove gotos that fall through to the label
    (nothing but braces/empty between goto and label). Does NOT restructure
    as if/else to avoid creating invalid brace structures."""
    changes = 0
    label_pos, label_refs = build_label_info(lines)

    for i in range(len(lines) - 1, -1, -1):
        gm = re.match(r'^(\s*)goto\s+(L_[0-9A-Fa-f]+)\s*;$', lines[i])
        if not gm:
            continue

        target = gm.group(2)
        if target not in label_pos:
            continue
        target_pos = label_pos[target]
        if target_pos <= i:
            continue

        refs = count_label_refs(lines, target)
        if refs < 2:
            continue

        # Only remove if there's nothing but structural code between
        only_structural = True
        for k in range(i + 1, target_pos):
            sk = lines[k].strip()
            if not sk or sk == ';' or sk == '}':
                continue
            if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', sk):
                continue
            only_structural = False
            break

        if not only_structural:
            continue

        brace_depth = 0
        for k in range(i + 1, target_pos):
            brace_depth += lines[k].count('{') - lines[k].count('}')
        if brace_depth > 0:
            continue

        lines[i] = ''
        changes += 1

        if count_label_refs(lines, target) == 0:
            s = lines[target_pos].strip()
            if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                lines[target_pos] = ''

        label_pos, label_refs = build_label_info(lines)

    return changes


# =====================================================================
# PASS: Goto fallthrough removal
# =====================================================================
def pass_goto_fallthrough(lines):
    changes = 0
    label_pos, _ = build_label_info(lines)

    for i in range(len(lines) - 1, -1, -1):
        m = re.match(r'^(\s*)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        target = m.group(2)
        if target not in label_pos:
            continue
        target_pos = label_pos[target]
        if target_pos <= i:
            continue

        only_structural = True
        for k in range(i + 1, target_pos):
            s = lines[k].strip()
            if not s or s == ';' or s == '}':
                continue
            if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                continue
            only_structural = False
            break

        if not only_structural:
            continue

        brace_depth = 0
        for k in range(i + 1, target_pos):
            brace_depth += lines[k].count('{') - lines[k].count('}')
        if brace_depth > 0:
            continue

        lines[i] = ''
        changes += 1
        if count_label_refs(lines, target) == 0:
            s = lines[target_pos].strip()
            if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                lines[target_pos] = ''

    return changes


# =====================================================================
# PASS: Combine adjacent if-goto same target
# =====================================================================
def pass_combine_cond_goto(lines):
    changes = 0
    i = 0
    while i < len(lines):
        m = re.match(r'^(\s+)if \((.+?)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            i += 1
            continue
        indent, cond, target = m.group(1), m.group(2), m.group(3)
        conds = [cond]
        j = i + 1
        while j < len(lines):
            s = lines[j].strip()
            if not s:
                j += 1
                continue
            mj = re.match(r'^' + re.escape(indent) + r'if \((.+?)\) goto ' + re.escape(target) + r';$', lines[j])
            if mj:
                conds.append(mj.group(1))
                j += 1
            else:
                break

        if len(conds) > 1:
            lines[i] = indent + 'if (' + ' || '.join(conds) + ') goto ' + target + ';'
            for k in range(i + 1, j):
                if re.match(r'^\s+if \(.+?\) goto ' + re.escape(target) + r';$', lines[k]):
                    lines[k] = ''
                    changes += 1
            i = j
        else:
            i += 1
    return changes


# =====================================================================
# PASS: Remove unreachable code after terminators
# =====================================================================
def pass_remove_unreachable(lines):
    """Remove unreachable code after terminators (goto/return/break).

    After a terminator, everything until the next label, closing brace,
    case label, or function boundary is unreachable and can be removed.
    This includes both gotos and regular statements.
    """
    changes = 0
    i = 0
    while i < len(lines):
        s = lines[i].strip()
        is_term = bool(
            re.match(r'^goto\s+L_[0-9A-Fa-f]+\s*;$', s) or
            re.match(r'^return\s*;?$', s) or
            re.match(r'^return\s+', s) or
            s == 'break;' or
            s == 'continue;'
        )
        if not is_term:
            i += 1
            continue

        j = i + 1
        while j < len(lines):
            sj = lines[j].strip()
            if not sj or sj == ';':
                j += 1
                continue
            # Stop at labels, braces, case labels
            if re.match(r'^L_[0-9A-Fa-f]+\s*:', sj):
                break
            if sj in ('}', '{') or sj.startswith('case ') or sj.startswith('default:'):
                break
            # Stop at function definitions
            if re.match(r'^(void|s32|u32|f32)\s+fn_', sj):
                break
            # Only remove unreachable gotos and simple assignments
            # Don't remove other code that might be declarations
            if re.match(r'^\s*goto\s+L_[0-9A-Fa-f]+\s*;$', sj):
                lines[j] = ''
                changes += 1
                j += 1
                continue
            # Remove unreachable simple statements (r = val; fn();)
            # but stop if it looks like something structural
            if re.match(r'^\s*(r\d+|tmp|f\d+|r12|ctr_fn)\s*=\s*.+;$', sj) or \
               re.match(r'^\s*fn_[0-9A-Fa-f]+\(.*\)\s*;$', sj) or \
               re.match(r'^\s*\(\(void\(\*\)\(void\)\)', sj) or \
               re.match(r'^\s*memcpy\(', sj) or \
               sj == 'return;':
                lines[j] = ''
                changes += 1
                j += 1
                continue
            break
        i = j

    return changes


# =====================================================================
# PASS: Value-set-goto-merge
# =====================================================================
def pass_value_set_goto_merge(lines):
    changes = 0
    label_pos, label_refs = build_label_info(lines)

    for label, refs in list(label_refs.items()):
        if label not in label_pos:
            continue
        if len(refs) < 3:
            continue

        pos = label_pos[label]

        for ref_line in refs:
            s = lines[ref_line].strip()
            if not re.match(r'^goto ' + re.escape(label) + r';$', s):
                continue
            if pos <= ref_line:
                continue

            can_remove = True
            for k in range(ref_line + 1, pos):
                sk = lines[k].strip()
                if not sk or sk == ';' or sk == '}':
                    continue
                if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', sk):
                    continue
                can_remove = False
                break

            if can_remove:
                brace_depth = 0
                for k in range(ref_line + 1, pos):
                    brace_depth += lines[k].count('{') - lines[k].count('}')
                if brace_depth <= 0:
                    lines[ref_line] = ''
                    changes += 1

    cleanup_unreferenced_labels(lines)
    return changes


# =====================================================================
# PASS: Conditional goto to next label
# =====================================================================
def pass_cond_goto_next(lines):
    changes = 0
    label_pos, _ = build_label_info(lines)

    i = 0
    while i < len(lines):
        m = re.match(r'^(\s*)if\s+\(.+?\)\s*goto\s+(L_[0-9A-Fa-f]+)\s*;$', lines[i])
        if m:
            target = m.group(2)
            if target in label_pos:
                target_pos = label_pos[target]
                if target_pos > i:
                    all_struct = True
                    for k in range(i + 1, target_pos):
                        sk = lines[k].strip()
                        if not sk or sk == ';' or sk == '}':
                            continue
                        all_struct = False
                        break

                    if all_struct:
                        brace_depth = 0
                        for k in range(i + 1, target_pos):
                            brace_depth += lines[k].count('{') - lines[k].count('}')
                        if brace_depth <= 0:
                            lines[i] = ''
                            changes += 1
                            if count_label_refs(lines, target) == 0:
                                s = lines[target_pos].strip()
                                if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                                    lines[target_pos] = ''
        i += 1
    return changes


# =====================================================================
# Main processing
# =====================================================================

def process_file(filepath):
    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()

    initial = count_gotos(content)
    if initial == 0:
        return 0

    lines = content.split('\n')

    for iteration in range(50):
        c = 0
        c += pass_dispatch_tree_to_switch(lines)
        c += pass_combine_cond_goto(lines)
        c += pass_goto_fallthrough(lines)
        c += pass_if_goto_else(lines)
        c += pass_if_goto_else_multiref(lines)
        c += pass_value_set_goto_merge(lines)
        c += pass_cond_goto_next(lines)
        c += pass_remove_unreachable(lines)
        c += cleanup_unreferenced_labels(lines)
        if c == 0:
            break

    content = '\n'.join(lines)
    content = re.sub(r'\n{4,}', '\n\n\n', content)
    final = count_gotos(content)
    removed = initial - final

    if removed > 0:
        with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
            f.write(content)
        print(f'  {filepath}: {initial} -> {final} gotos ({removed} removed)')
    else:
        print(f'  {filepath}: no changes ({initial} gotos)')

    return removed


def main():
    import argparse
    parser = argparse.ArgumentParser(description='Phase 3b switch-case goto converter')
    parser.add_argument('files', nargs='*', help='Source files to process')
    parser.add_argument('--all', action='store_true', help='Process all files with gotos')
    parser.add_argument('--no-compile', action='store_true', help='Skip compile check')
    parser.add_argument('-cv', '--compiler-version', help='Compiler version override')
    args = parser.parse_args()

    if args.all:
        import glob
        files = []
        for pattern in ['src/**/*.c']:
            files.extend(glob.glob(pattern, recursive=True))
        files = [f for f in files if count_gotos(open(f, 'r', errors='replace').read()) > 0]
        files.sort(key=lambda f: count_gotos(open(f, 'r', errors='replace').read()), reverse=True)
    elif args.files:
        files = args.files
    else:
        print("Usage: convert_switch_phase3b.py <file.c> [...]")
        print("       convert_switch_phase3b.py --all")
        sys.exit(1)

    total_removed = 0
    total_files = 0

    for filepath in files:
        if not os.path.exists(filepath):
            print(f'  File not found: {filepath}')
            continue

        backup = filepath + '.p3b.bak'
        shutil.copy2(filepath, backup)

        removed = process_file(filepath)

        if removed > 0:
            if args.no_compile:
                print(f'  -> skipping compile check')
                os.remove(backup)
                total_removed += removed
                total_files += 1
            elif compile_check(filepath, args.compiler_version):
                print(f'  -> compile OK')
                os.remove(backup)
                total_removed += removed
                total_files += 1
            else:
                print(f'  -> compile FAILED, reverting')
                shutil.copy2(backup, filepath)
                os.remove(backup)
        else:
            os.remove(backup)

    print(f'\nTotal: {total_removed} gotos removed across {total_files} files')


if __name__ == '__main__':
    main()
