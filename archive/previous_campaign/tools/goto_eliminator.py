#!/usr/bin/env python3
"""
Goto eliminator for decompiled C code (PowerPC decomp).
Iteratively applies safe structural transformations to convert goto-based
control flow into if/else, while, do-while, break, continue.

Key passes:
  1. OR-chain: consecutive if-goto to same label -> compound condition
  2. Forward conditional skip (single-use label)
  3. Forward conditional skip (multi-use label, block ends with return/break/continue/goto-away)
  4. If-goto-else with unconditional goto before label
  5. Return guard patterns
  6. Backward goto -> do-while / while loops
  7. goto-into-do-while -> while loop
  8. Break conversion (goto past loop end)
  9. Continue conversion (goto to loop advance)
 10. Unconditional forward goto cleanup
 11. Multi-use convergence: goto L where L is at end of enclosing if-block
 12. Label cleanup
"""

import re
import sys

def count_gotos(lines):
    return sum(len(re.findall(r'\bgoto\s+\w+\s*;', l)) for l in lines)

def find_label(lines, label, start=0, end=None):
    if end is None:
        end = len(lines)
    t = label + ':'
    for i in range(start, end):
        s = lines[i].strip()
        if s == t or s == t + ' ;' or s.startswith(t + ' '):
            return i
    return -1

def is_label_line(line):
    s = line.strip()
    m = re.match(r'^(L_[0-9A-Fa-f]+|found|not_found|done|skip|end)\s*:\s*;?\s*$', s)
    return m.group(1) if m else None

def label_use_count(lines, label):
    pat = f'goto {label}'
    return sum(1 for l in lines if pat in l)

def get_indent(line):
    return line[:len(line) - len(line.lstrip())] if line.strip() else ''

def indent_block(block, extra='    '):
    return [extra + l if l.strip() else l for l in block]

def extract_goto(line):
    s = line.strip()
    m = re.match(r'if\s*\((.+?)\)\s+goto\s+(\w+)\s*;$', s)
    if m:
        return (m.group(1), m.group(2))
    m = re.match(r'goto\s+(\w+)\s*;$', s)
    if m:
        return (None, m.group(1))
    return None

def negate(cond):
    cond = cond.strip()
    if cond.startswith('!(') and cond.endswith(')'):
        return cond[2:-1]
    ops = {'!=':'==','==':'!=','>=':'<','<=':'>','>':'<=','<':'>='}
    m = re.search(r'(.*?)\s*(!=|==|>=|<=|>(?!=)|<(?!=))\s*(.*)', cond)
    if m:
        l, op, r = m.groups()
        if op in ops:
            return f'{l} {ops[op]} {r}'
    return f'!({cond})'

def has_external_label_target(lines, bstart, bend):
    """Check if labels in lines[bstart:bend] are targeted from outside."""
    for i in range(bstart, bend):
        lbl = is_label_line(lines[i])
        if lbl:
            for j, l in enumerate(lines):
                if (j < bstart or j >= bend) and f'goto {lbl}' in l:
                    return True
    return False

def block_terminates(block):
    """Check if block always terminates (ends with return/break/continue or goto)."""
    non_empty = [l.strip() for l in block if l.strip()]
    if not non_empty:
        return False
    last = non_empty[-1]
    if last.startswith('return') or last == 'break;' or last == 'continue;':
        return True
    if re.match(r'^goto\s+\w+\s*;$', last):
        return True
    return False

# ---------------------------------------------------------------------------
# Pass: OR-chain  if(A) goto L; if(B) goto L; -> if(A||B) goto L;
# ---------------------------------------------------------------------------
def pass_or_chain(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is not None:
                cond1, lbl = g
                j = i + 1
                conds = [cond1]
                while j < len(lines):
                    gj = extract_goto(lines[j])
                    if gj and gj[0] is not None and gj[1] == lbl:
                        conds.append(gj[0])
                        j += 1
                    else:
                        break
                if len(conds) > 1:
                    merged = ' || '.join(f'({c})' for c in conds)
                    ind = get_indent(lines[i])
                    lines[i:j] = [f'{ind}if ({merged}) goto {lbl};']
                    changed = True
                    continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: Forward conditional skip (single-use)
# ---------------------------------------------------------------------------
def pass_fwd_cond_skip(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is not None:
                cond, lbl = g
                li = find_label(lines, lbl, i+1)
                if li > i and label_use_count(lines, lbl) == 1:
                    if not has_external_label_target(lines, i+1, li):
                        ind = get_indent(lines[i])
                        new = [f'{ind}if ({negate(cond)}) {{']
                        new += indent_block(lines[i+1:li])
                        new += [f'{ind}}}']
                        lines[i:li+1] = new
                        changed = True
                        continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: Forward conditional skip (multi-use label, block terminates)
# When the block between if-goto and label ends with return/break/continue,
# we can wrap it without needing the label to be single-use.
# ---------------------------------------------------------------------------
def pass_fwd_cond_skip_term(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is not None:
                cond, lbl = g
                li = find_label(lines, lbl, i+1)
                if li > i:
                    block = lines[i+1:li]
                    if block_terminates(block) and not has_external_label_target(lines, i+1, li):
                        ind = get_indent(lines[i])
                        new = [f'{ind}if ({negate(cond)}) {{']
                        new += indent_block(block)
                        new += [f'{ind}}}']
                        # Keep label if still used by others
                        uses = label_use_count(lines, lbl)
                        if uses <= 1:
                            lines[i:li+1] = new
                        else:
                            lines[i:li] = new
                        changed = True
                        continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: if-goto-else
# ---------------------------------------------------------------------------
def pass_if_goto_else(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is not None:
                cond, lbl_a = g
                la = find_label(lines, lbl_a, i+1)
                if la > i + 1:
                    pre = la - 1
                    while pre > i and not lines[pre].strip():
                        pre -= 1
                    g2 = extract_goto(lines[pre])
                    if g2 and g2[0] is None and pre > i:
                        lbl_b = g2[1]
                        lb = find_label(lines, lbl_b, la+1)
                        if lb > la and label_use_count(lines, lbl_a) == 1:
                            if (not has_external_label_target(lines, i+1, pre)
                                and not has_external_label_target(lines, la+1, lb)):
                                ind = get_indent(lines[i])
                                else_block = lines[i+1:pre]
                                then_block = lines[la+1:lb]
                                new = [f'{ind}if ({cond}) {{']
                                new += indent_block(then_block)
                                new += [f'{ind}}} else {{']
                                new += indent_block(else_block)
                                new += [f'{ind}}}']
                                end = lb + 1 if label_use_count(lines, lbl_b) <= 1 else lb
                                lines[i:end] = new
                                changed = True
                                continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: Return guard (if (cond) goto L; <return-block> L:)
# Works with multi-use labels since block terminates with return.
# ---------------------------------------------------------------------------
def pass_return_guard(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is not None:
                cond, lbl = g
                li = find_label(lines, lbl, i+1)
                if li > i:
                    block = lines[i+1:li]
                    if block_terminates(block) and not has_external_label_target(lines, i+1, li):
                        ind = get_indent(lines[i])
                        new = [f'{ind}if ({negate(cond)}) {{']
                        new += indent_block(block)
                        new += [f'{ind}}}']
                        uses = label_use_count(lines, lbl)
                        if uses <= 1:
                            lines[i:li+1] = new
                        else:
                            lines[i:li] = new
                        changed = True
                        continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: Backward goto -> do-while
# ---------------------------------------------------------------------------
def pass_backward_loop(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g:
                cond, lbl = g
                li = find_label(lines, lbl, 0, i)
                if li >= 0 and label_use_count(lines, lbl) == 1:
                    ind = get_indent(lines[li])
                    body = lines[li+1:i]
                    if cond:
                        new = [f'{ind}do {{'] + indent_block(body) + [f'{ind}}} while ({cond});']
                    else:
                        new = [f'{ind}while (1) {{'] + indent_block(body) + [f'{ind}}}']
                    lines[li:i+1] = new
                    changed = True
                    continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: goto-into-do-while -> while
# ---------------------------------------------------------------------------
def pass_goto_into_dowhile(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                ni = i + 1
                while ni < len(lines) and not lines[ni].strip():
                    ni += 1
                if ni < len(lines) and lines[ni].strip().startswith('do {'):
                    depth = 0
                    end = -1
                    for j in range(ni, len(lines)):
                        depth += lines[j].count('{') - lines[j].count('}')
                        if depth == 0:
                            end = j
                            break
                    if end > 0:
                        li = find_label(lines, lbl, ni, end+1)
                        if li >= 0 and label_use_count(lines, lbl) == 1:
                            m = re.match(r'\s*\}\s*while\s*\((.+)\)\s*;', lines[end])
                            if m:
                                wc = m.group(1)
                                ind = get_indent(lines[i])
                                body_before_label = lines[ni+1:li]
                                body_after_label = lines[li+1:end]
                                new = [f'{ind}while ({wc}) {{']
                                new += indent_block(body_after_label)
                                new += indent_block(body_before_label)
                                new += [f'{ind}}}']
                                lines[i:end+1] = new
                                changed = True
                                continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: Break conversion
# ---------------------------------------------------------------------------
def pass_break(lines):
    changed = True
    while changed:
        changed = False
        for i, line in enumerate(lines):
            s = line.strip()
            if s.startswith('do {') or (s.startswith('while (') and s.endswith('{')):
                depth = 0
                end = -1
                for j in range(i, len(lines)):
                    depth += lines[j].count('{') - lines[j].count('}')
                    if depth == 0:
                        end = j
                        break
                if end < 0:
                    continue
                after = end + 1
                while after < len(lines) and not lines[after].strip():
                    after += 1
                if after < len(lines):
                    albl = is_label_line(lines[after])
                    if albl:
                        for k in range(i+1, end):
                            gk = extract_goto(lines[k])
                            if gk and gk[1] == albl:
                                ck = gk[0]
                                ind_k = get_indent(lines[k])
                                lines[k] = f'{ind_k}if ({ck}) break;' if ck else f'{ind_k}break;'
                                changed = True
                        if changed and label_use_count(lines, albl) == 0:
                            lines[after] = ''
    return lines

# ---------------------------------------------------------------------------
# Pass: Continue conversion
# ---------------------------------------------------------------------------
def pass_continue(lines):
    changed = True
    while changed:
        changed = False
        for i, line in enumerate(lines):
            s = line.strip()
            if s.startswith('do {'):
                depth = 0
                end = -1
                for j in range(i, len(lines)):
                    depth += lines[j].count('{') - lines[j].count('}')
                    if depth == 0:
                        end = j
                        break
                if end < 0:
                    continue
                for k in range(i+1, end):
                    lbl = is_label_line(lines[k])
                    if lbl:
                        all_inside = True
                        any_goto = False
                        for m_idx in range(len(lines)):
                            if f'goto {lbl}' in lines[m_idx]:
                                any_goto = True
                                if m_idx <= i or m_idx >= end:
                                    all_inside = False
                                    break
                        if any_goto and all_inside:
                            non_empty_after = [idx for idx in range(k+1, end) if lines[idx].strip()]
                            if len(non_empty_after) <= 2:
                                for m_idx in range(i+1, end):
                                    gm = extract_goto(lines[m_idx])
                                    if gm and gm[1] == lbl:
                                        cm = gm[0]
                                        ind_m = get_indent(lines[m_idx])
                                        lines[m_idx] = f'{ind_m}if ({cm}) continue;' if cm else f'{ind_m}continue;'
                                        changed = True
                                if changed and label_use_count(lines, lbl) == 0:
                                    lines[k] = ''
    return lines

# ---------------------------------------------------------------------------
# Pass: Unconditional forward goto to single-use label with empty block
# ---------------------------------------------------------------------------
def pass_uncond_empty(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                li = find_label(lines, lbl, i+1)
                if li > i and label_use_count(lines, lbl) == 1:
                    block = lines[i+1:li]
                    non_empty = [l for l in block if l.strip() and not is_label_line(l)]
                    if not non_empty:
                        lines[i:li+1] = block
                        changed = True
                        continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: Multi-use convergence -- goto L at end of if-block where L is right
# after the closing brace. Remove the goto since control falls through.
#
# Pattern (inside if/else block):
#   ...code...
#   goto L;
# }
# L: ;
#
# The goto is redundant - control would fall to L anyway after the }.
# ---------------------------------------------------------------------------
def pass_convergence_fallthrough(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                li = find_label(lines, lbl, i+1)
                if li > i:
                    # Check if everything between goto and label is just closing braces/empty/labels
                    between = lines[i+1:li]
                    all_braces = True
                    for b in between:
                        bs = b.strip()
                        if bs and bs != '}' and bs != '} else {' and not is_label_line(b):
                            all_braces = False
                            break
                    if all_braces:
                        # Remove this goto
                        ind = get_indent(lines[i])
                        lines[i] = ''  # Remove goto
                        if label_use_count(lines, lbl) == 0:
                            lines[li] = ''
                        changed = True
                        continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Pass: goto at end of if-block to label after else-block
# Pattern:
#   if (...) {
#       ...
#       goto L;
#   } else {
#       ...
#   }
#   L: ;
# The goto is redundant.
# ---------------------------------------------------------------------------
def pass_goto_end_if_block(lines):
    changed = True
    while changed:
        changed = False
        for i in range(len(lines)):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                # Check if next non-empty line is closing brace
                j = i + 1
                while j < len(lines) and not lines[j].strip():
                    j += 1
                if j < len(lines) and lines[j].strip().startswith('}'):
                    li = find_label(lines, lbl, j)
                    if li >= 0:
                        # Check everything between } and label is structural
                        between = lines[j+1:li]
                        structural = True
                        for b in between:
                            bs = b.strip()
                            if bs and bs != '}' and bs != '} else {' and not is_label_line(b):
                                structural = False
                                break
                        if structural:
                            lines[i] = ''
                            if label_use_count(lines, lbl) == 0:
                                lines[li] = ''
                            changed = True
                            break
    return lines

# ---------------------------------------------------------------------------
# Pass: Unconditional goto to label, where between them is dead code
# (code after unconditional goto/return/break that can never execute)
# ---------------------------------------------------------------------------
def pass_dead_code_goto(lines):
    changed = True
    while changed:
        changed = False
        i = 0
        while i < len(lines):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                li = find_label(lines, lbl, i+1)
                if li > i + 1:
                    # Everything between goto and label is dead code (unreachable)
                    # BUT only if no labels in between are targeted from elsewhere
                    between = lines[i+1:li]
                    has_live_label = False
                    for bi, bl in enumerate(between):
                        blbl = is_label_line(bl)
                        if blbl and label_use_count(lines, blbl) > 0:
                            has_live_label = True
                            break
                    if not has_live_label and label_use_count(lines, lbl) == 1:
                        # Remove dead code and goto+label
                        lines[i:li+1] = []
                        changed = True
                        continue
            i += 1
    return lines

# ---------------------------------------------------------------------------
# Cleanup passes
# ---------------------------------------------------------------------------
def cleanup_labels(lines):
    changed = True
    while changed:
        changed = False
        new = []
        for line in lines:
            lbl = is_label_line(line)
            if lbl and label_use_count(lines, lbl) == 0:
                changed = True
                continue
            new.append(line)
        lines = new
    return lines

def cleanup_empty_lines(lines):
    """Remove runs of more than 1 blank line, and blank lines inside braces."""
    new = []
    for i, line in enumerate(lines):
        if not line.strip():
            # Skip if previous was also blank
            if new and not new[-1].strip():
                continue
        new.append(line)
    return new

def cleanup_empty_strings(lines):
    """Remove '' entries that are truly empty (not blank lines)."""
    return [l for l in lines if l is not None]

# ---------------------------------------------------------------------------
# Main orchestration
# ---------------------------------------------------------------------------
def process_function(func_lines):
    orig = count_gotos(func_lines)
    if orig == 0:
        return func_lines, 0

    lines = list(func_lines)
    prev = orig + 1
    rounds = 0

    while count_gotos(lines) > 0 and count_gotos(lines) < prev and rounds < 40:
        prev = count_gotos(lines)
        rounds += 1

        lines = pass_or_chain(lines)
        lines = pass_fwd_cond_skip(lines)
        lines = pass_fwd_cond_skip_term(lines)
        lines = pass_if_goto_else(lines)
        lines = pass_return_guard(lines)
        lines = pass_backward_loop(lines)
        lines = pass_goto_into_dowhile(lines)
        lines = pass_break(lines)
        lines = pass_continue(lines)
        lines = pass_convergence_fallthrough(lines)
        lines = pass_goto_end_if_block(lines)
        lines = pass_uncond_empty(lines)
        lines = pass_dead_code_goto(lines)
        lines = cleanup_labels(lines)

    lines = cleanup_empty_lines(lines)
    return lines, orig - count_gotos(lines)


def find_functions(lines):
    funcs = []
    i = 0
    while i < len(lines):
        if re.match(r'^(void|u32|s32|u16|s16|u8|s8|f32|f64|int|void\s*\*|static\s+void|static\s+u32|static\s+s32|static\s+int|\w+\s*\*)\s+\w+\s*\(', lines[i]):
            j = i
            while j < len(lines) and '{' not in lines[j]:
                j += 1
            if j >= len(lines):
                i += 1
                continue
            depth = 0
            for k in range(j, len(lines)):
                depth += lines[k].count('{') - lines[k].count('}')
                if depth == 0:
                    funcs.append((i, k))
                    i = k + 1
                    break
            else:
                i += 1
        else:
            i += 1
    return funcs


def process_file(filename):
    with open(filename, 'r') as f:
        all_lines = f.read().split('\n')

    functions = find_functions(all_lines)
    total_elim = 0
    total_remain = 0
    funcs_done = 0

    for start, end in reversed(functions):
        func_lines = all_lines[start:end+1]
        gc = count_gotos(func_lines)
        if gc == 0:
            continue

        new_lines, elim = process_function(func_lines)
        if elim > 0:
            all_lines[start:end+1] = new_lines
            total_elim += elim
            remain = gc - elim
            total_remain += remain
            funcs_done += 1
            name = func_lines[0].strip()[:70]
            print(f'  {name}: -{elim}/{gc} ({remain} left)')
        else:
            total_remain += gc

    with open(filename, 'w') as f:
        f.write('\n'.join(all_lines))

    print(f'\n  TOTAL: {total_elim} eliminated, {total_remain} remaining ({funcs_done} functions touched)')
    return total_elim, total_remain


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python goto_eliminator.py <file.c> [...]")
        sys.exit(1)

    ge, gr = 0, 0
    for fn in sys.argv[1:]:
        print(f'\n=== {fn} ===')
        e, r = process_file(fn)
        ge += e
        gr += r
    print(f'\n=== GRAND TOTAL: {ge} eliminated, {gr} remaining ===')
