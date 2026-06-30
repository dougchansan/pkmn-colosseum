#!/usr/bin/env python3
"""
Specialized goto converter for colosseum_battle.c, colosseum_script.c, colosseum_event.c.

Handles patterns the generic convert_gotos.py doesn't:
1. Guard chains (if(cond) goto NEXT; r0=0; goto MERGE; NEXT:)
2. Do-while with inner labels (permissive)
3. Forward gotos with inner gotos (wrapping in if-block)
4. While loops with update code between cond label and back-jump
5. Break/continue detection post-loop-creation
6. Merge-point collapse (multiple goto MERGE -> if/else chains)
"""
import re
import sys


def invert_cond(cond):
    """Invert a C condition. Returns None if unable."""
    # Handle compound conditions with || and &&
    if ' || ' in cond:
        # De Morgan: !(A || B) = !A && !B
        parts = cond.split(' || ')
        inverted = []
        for p in parts:
            inv = invert_cond(p.strip())
            if inv is None:
                return None
            inverted.append(inv)
        return ' && '.join(inverted)
    if ' && ' in cond:
        # De Morgan: !(A && B) = !A || !B
        parts = cond.split(' && ')
        inverted = []
        for p in parts:
            inv = invert_cond(p.strip())
            if inv is None:
                return None
            inverted.append(inv)
        return ' || '.join(inverted)
    if ' == ' in cond and cond.count('==') == 1:
        return cond.replace(' == ', ' != ')
    elif ' != ' in cond and cond.count('!=') == 1:
        return cond.replace(' != ', ' == ')
    elif ' < ' in cond and cond.count('<') == 1 and '<=' not in cond:
        return cond.replace(' < ', ' >= ')
    elif ' > ' in cond and cond.count('>') == 1 and '>=' not in cond:
        return cond.replace(' > ', ' <= ')
    elif ' <= ' in cond and cond.count('<=') == 1:
        return cond.replace(' <= ', ' > ')
    elif ' >= ' in cond and cond.count('>=') == 1:
        return cond.replace(' >= ', ' < ')
    return None


def build_indices(lines):
    label_refcount = {}
    label_sources = {}
    for i, line in enumerate(lines):
        for m in re.finditer(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', line):
            lbl = m.group(1)
            label_refcount[lbl] = label_refcount.get(lbl, 0) + 1
            if lbl not in label_sources:
                label_sources[lbl] = []
            label_sources[lbl].append(i)
    label_pos = {}
    for i, line in enumerate(lines):
        m = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', line)
        if m:
            label_pos[m.group(1)] = i
    return label_refcount, label_pos, label_sources


def convert_guard_chain(lines, label_refcount, label_pos):
    """Convert: if(cond) goto NEXT; r0=val; goto MERGE; NEXT:
    Into: if(!(cond)) { r0=val; goto MERGE; }

    This is the most common remaining pattern - short forward skip
    where the skipped code contains a goto to a merge point.
    """
    changes = 0
    for i in range(len(lines)-1, -1, -1):
        line = lines[i]
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if not m:
            continue
        indent, cond, label = m.group(1), m.group(2), m.group(3)
        if label_refcount.get(label, 0) != 1:
            continue
        if label not in label_pos:
            continue
        label_idx = label_pos[label]
        if label_idx <= i:
            continue
        # Only handle small gaps (1-4 non-empty statements between)
        gap = label_idx - i - 1
        if gap < 1 or gap > 5:
            continue
        # Don't allow inner labels
        has_inner_label = False
        for k in range(i+1, label_idx):
            if re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k]):
                has_inner_label = True
                break
        if has_inner_label:
            continue
        inv = invert_cond(cond)
        if inv is None:
            continue
        lines[i] = indent + 'if (' + inv + ') {'
        for k in range(i+1, label_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label_idx] = indent + '}'
        changes += 1
        label_refcount[label] = 0
    return changes


def convert_forward_allow_inner_gotos(lines, label_refcount, label_pos, label_sources):
    """Convert forward single-ref conditional gotos even when body contains
    gotos to external targets, as long as no inner labels are referenced
    from outside."""
    changes = 0
    for i in range(len(lines)-1, -1, -1):
        line = lines[i]
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if not m:
            continue
        indent, cond, label = m.group(1), m.group(2), m.group(3)
        if label_refcount.get(label, 0) != 1:
            continue
        if label not in label_pos:
            continue
        label_idx = label_pos[label]
        if label_idx <= i:
            continue
        # Skip huge gaps (these are often whole function restructurings)
        if label_idx - i > 200:
            continue
        # Check: no inner labels referenced from outside
        has_external_ref = False
        for k in range(i+1, label_idx):
            lm = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if lm:
                inner_lbl = lm.group(1)
                for src in label_sources.get(inner_lbl, []):
                    if src < i or src > label_idx:
                        has_external_ref = True
                        break
            if has_external_ref:
                break
        if has_external_ref:
            continue
        inv = invert_cond(cond)
        if inv is None:
            continue
        lines[i] = indent + 'if (' + inv + ') {'
        for k in range(i+1, label_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label_idx] = indent + '}'
        changes += 1
        label_refcount[label] = 0
    return changes


def convert_do_while_permissive(lines, label_refcount, label_pos, label_sources):
    """Convert do-while even with inner labels/gotos, as long as inner labels
    are only referenced from within the loop and the label/back-jump are at
    the same brace depth."""
    changes = 0
    for i in range(len(lines)-1, -1, -1):
        line = lines[i]
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if not m:
            continue
        indent, cond, label = m.group(1), m.group(2), m.group(3)
        if label not in label_pos:
            continue
        label_idx = label_pos[label]
        if label_idx >= i:
            continue
        if label_refcount.get(label, 0) != 1:
            continue
        # Check brace depth: label and back-jump must be at same depth
        depth = 0
        for k in range(label_idx, i + 1):
            depth += lines[k].count('{') - lines[k].count('}')
        if depth != 0:
            continue
        # Check inner labels
        has_external_ref = False
        for k in range(label_idx+1, i):
            lm = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if lm:
                inner_lbl = lm.group(1)
                for src in label_sources.get(inner_lbl, []):
                    if src < label_idx or src > i:
                        has_external_ref = True
                        break
            if has_external_ref:
                break
        if has_external_ref:
            continue
        lines[label_idx] = indent + 'do {'
        for k in range(label_idx+1, i):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[i] = indent + '} while (' + cond + ');'
        changes += 1
        label_refcount[label] = 0
    return changes


def convert_while_loop_with_update(lines, label_refcount, label_pos):
    """Convert while-loop where update code sits between cond label and back-jump:
        goto L_cond;
    L_body:
        ...body...
    L_cond:
        ...update code...
        if (loopCond) goto L_body;
    Into:
        while (1) {
            ...update code...
            if (!(loopCond)) break;
            ...body...
        }
    """
    changes = 0
    i = 0
    while i < len(lines):
        m_goto = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m_goto:
            i += 1
            continue
        indent = m_goto.group(1)
        cond_label = m_goto.group(2)
        if cond_label not in label_pos:
            i += 1
            continue
        cond_idx = label_pos[cond_label]
        if cond_idx <= i:
            i += 1
            continue

        # Find body label after the goto
        body_label = body_idx = None
        for k in range(i+1, min(i+3, cond_idx)):
            m_body = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if m_body:
                body_label = m_body.group(1)
                body_idx = k
                break
        if body_label is None:
            i += 1
            continue

        # Find the back-jump
        back_goto_idx = back_goto_cond = None
        for k in range(cond_idx + 1, min(cond_idx + 15, len(lines))):
            stripped = lines[k].strip()
            if not stripped:
                continue
            m_back = re.match(r'^if \((.+)\) goto (L_[0-9A-Fa-f]+);$', stripped)
            if m_back and m_back.group(2) == body_label:
                back_goto_idx = k
                back_goto_cond = m_back.group(1)
                break
            if re.match(r'^L_[0-9A-Fa-f]+\s*:', stripped):
                break
            if re.match(r'^goto\s+', stripped):
                break

        if back_goto_idx is None:
            i += 1
            continue

        if label_refcount.get(body_label, 0) != 1:
            i += 1
            continue
        if label_refcount.get(cond_label, 0) != 1:
            i += 1
            continue

        # Check no gotos in cond eval code
        has_bad = False
        for k in range(cond_idx + 1, back_goto_idx):
            if re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k]):
                has_bad = True
                break
        if has_bad:
            i += 1
            continue

        inv_cond = invert_cond(back_goto_cond)
        if inv_cond is None:
            i += 1
            continue

        # Collect cond eval lines
        cond_eval_content = []
        for k in range(cond_idx + 1, back_goto_idx):
            if lines[k].strip():
                cond_eval_content.append(indent + '    ' + lines[k].strip())

        # Apply
        lines[i] = indent + 'while (1) {'
        insert_code = cond_eval_content + [indent + '    if (' + inv_cond + ') break;']
        lines[body_idx] = '\n'.join(insert_code)

        for k in range(body_idx + 1, cond_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]

        lines[cond_idx] = ''
        for k in range(cond_idx + 1, back_goto_idx):
            lines[k] = ''
        lines[back_goto_idx] = indent + '}'

        label_refcount[cond_label] = 0
        label_refcount[body_label] = 0
        changes += 2
        i += 1

    return changes


def convert_goto_after_loop_to_break(lines, label_refcount, label_pos):
    """Convert gotos inside loops to break when target is right after the loop end."""
    changes = 0
    # Find loop structures
    loop_ranges = []
    for i, line in enumerate(lines):
        if re.match(r'^\s*do\s*\{', line):
            depth = 1
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    if re.match(r'^\s*\}\s*while\s*\(', lines[j]):
                        loop_ranges.append((i, j))
                    break
        elif re.match(r'^\s*while\s*\(.+\)\s*\{', line):
            depth = 1
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    loop_ranges.append((i, j))
                    break

    for loop_start, loop_end in loop_ranges:
        # Check label right after loop
        for k in range(loop_end + 1, min(loop_end + 3, len(lines))):
            stripped = lines[k].strip()
            if not stripped:
                continue
            m_lbl = re.match(r'^(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', stripped)
            if m_lbl:
                lbl = m_lbl.group(1)
                # Convert gotos to this label within the loop to break
                for j in range(loop_start, loop_end + 1):
                    mg = re.match(r'^(\s+)goto ' + re.escape(lbl) + r';$', lines[j])
                    if mg:
                        lines[j] = mg.group(1) + 'break;'
                        label_refcount[lbl] = label_refcount.get(lbl, 0) - 1
                        changes += 1
                        continue
                    mg = re.match(r'^(\s+)if \((.+)\) goto ' + re.escape(lbl) + r';$', lines[j])
                    if mg:
                        lines[j] = mg.group(1) + 'if (' + mg.group(2) + ') break;'
                        label_refcount[lbl] = label_refcount.get(lbl, 0) - 1
                        changes += 1
            break

    return changes


def convert_uncond_backward_to_loop(lines, label_refcount, label_pos, label_sources):
    """Convert unconditional backward goto with single ref into do-while(1):
        L_BODY: ;
        ...code...
        goto L_BODY;
    Into:
        do {
            ...code...
        } while (1);
    """
    changes = 0
    for i in range(len(lines)-1, -1, -1):
        line = lines[i]
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', line)
        if not m:
            continue
        indent, label = m.group(1), m.group(2)
        if label not in label_pos:
            continue
        label_idx = label_pos[label]
        if label_idx >= i:
            continue
        if label_refcount.get(label, 0) != 1:
            continue
        # Check brace depth
        depth = 0
        for k in range(label_idx, i + 1):
            depth += lines[k].count('{') - lines[k].count('}')
        if depth != 0:
            continue
        # Check inner labels not referenced from outside
        has_external_ref = False
        for k in range(label_idx+1, i):
            lm = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if lm:
                inner_lbl = lm.group(1)
                for src in label_sources.get(inner_lbl, []):
                    if src < label_idx or src > i:
                        has_external_ref = True
                        break
            if has_external_ref:
                break
        if has_external_ref:
            continue
        lines[label_idx] = indent + 'do {'
        for k in range(label_idx+1, i):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[i] = indent + '} while (1);'
        changes += 1
        label_refcount[label] = 0
    return changes


def convert_goto_to_continue(lines, label_refcount, label_pos):
    """Convert gotos to loop headers into continue statements."""
    changes = 0
    loop_ranges = []
    for i, line in enumerate(lines):
        if re.match(r'^\s*do\s*\{', line):
            depth = 1
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    if re.match(r'^\s*\}\s*while\s*\(', lines[j]):
                        loop_ranges.append((i, j, 'do-while'))
                    break
        elif re.match(r'^\s*while\s*\(.+\)\s*\{', line):
            depth = 1
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    loop_ranges.append((i, j, 'while'))
                    break

    for loop_start, loop_end, loop_type in loop_ranges:
        for i in range(loop_start + 1, loop_end):
            line = lines[i]
            m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', line)
            cond = None
            if not m:
                m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
                if m:
                    cond = m.group(2)
                    lbl = m.group(3)
                    indent = m.group(1)
                else:
                    continue
            else:
                lbl = m.group(2)
                indent = m.group(1)

            if lbl not in label_pos:
                continue
            target = label_pos[lbl]

            is_continue = False
            if target == loop_start:
                is_continue = True
            elif loop_type == 'do-while' and target == loop_end:
                is_continue = True

            if is_continue:
                if cond:
                    lines[i] = indent + 'if (' + cond + ') continue;'
                else:
                    lines[i] = indent + 'continue;'
                label_refcount[lbl] = label_refcount.get(lbl, 0) - 1
                changes += 1

    return changes


def remove_dead_gotos(lines, label_refcount, label_pos):
    """Remove goto L; L: (goto to immediately following label)."""
    changes = 0
    for i in range(len(lines) - 1):
        m_goto = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m_goto:
            continue
        label = m_goto.group(2)
        for k in range(i + 1, min(i + 5, len(lines))):
            if lines[k].strip():
                m_lbl = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
                if m_lbl and m_lbl.group(1) == label:
                    lines[i] = ''
                    label_refcount[label] = label_refcount.get(label, 0) - 1
                    changes += 1
                break
    return changes


def remove_unreferenced_labels(lines, label_refcount, label_pos):
    """Remove labels with zero references."""
    changes = 0
    for label, pos in label_pos.items():
        if label_refcount.get(label, 0) <= 0:
            m = re.match(r'^(\s*)(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[pos])
            if m:
                lines[pos] = ''
                changes += 1
    return changes


def convert_goto_to_return(lines, label_refcount, label_pos):
    """Convert gotos to labels followed by return into direct returns."""
    changes = 0
    return_labels = set()
    for lbl, pos in label_pos.items():
        for k in range(pos + 1, min(pos + 3, len(lines))):
            stripped = lines[k].strip()
            if stripped:
                if stripped == 'return;':
                    return_labels.add(lbl)
                break
    for i in range(len(lines)):
        line = lines[i]
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', line)
        if m and m.group(2) in return_labels:
            lines[i] = m.group(1) + 'return;'
            label_refcount[m.group(2)] = label_refcount.get(m.group(2), 0) - 1
            changes += 1
            continue
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if m and m.group(3) in return_labels:
            lines[i] = m.group(1) + 'if (' + m.group(2) + ') return;'
            label_refcount[m.group(3)] = label_refcount.get(m.group(3), 0) - 1
            changes += 1
    return changes


def convert_multi_condition_or(lines, label_refcount, label_pos):
    """Collapse consecutive gotos to the same target into OR conditions."""
    changes = 0
    i = 0
    while i < len(lines):
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            i += 1
            continue
        indent, first_cond, label = m.group(1), m.group(2), m.group(3)
        conditions = [first_cond]
        j = i + 1
        while j < len(lines):
            mj = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[j])
            if mj and mj.group(3) == label:
                conditions.append(mj.group(2))
                j += 1
            else:
                break
        if len(conditions) > 1:
            combined = ' || '.join(conditions)
            lines[i] = indent + 'if (' + combined + ') goto ' + label + ';'
            for k in range(i + 1, i + len(conditions)):
                lines[k] = ''
                label_refcount[label] = label_refcount.get(label, 0) - 1
                changes += 1
            i = i + len(conditions)
        else:
            i += 1
    return changes


def convert_multi_ref_forward_2(lines, label_refcount, label_pos, label_sources):
    """Convert labels with exactly 2 adjacent conditional forward refs into
    combined if-block."""
    changes = 0
    processed = set()
    for label, pos in label_pos.items():
        if label in processed:
            continue
        if label_refcount.get(label, 0) != 2:
            continue
        refs = label_sources.get(label, [])
        if len(refs) != 2:
            continue
        idx1, idx2 = sorted(refs)
        if idx1 >= pos or idx2 >= pos:
            continue
        if idx2 != idx1 + 1:
            continue
        m1 = re.match(r'^(\s+)if \((.+)\) goto ' + re.escape(label) + r';$', lines[idx1])
        m2 = re.match(r'^(\s+)if \((.+)\) goto ' + re.escape(label) + r';$', lines[idx2])
        if not m1 or not m2:
            continue
        indent = m1.group(1)
        cond1, cond2 = m1.group(2), m2.group(2)
        # No labels between idx2 and target
        has_other = False
        for k in range(idx2 + 1, pos):
            if re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]):
                has_other = True
                break
        if has_other:
            continue
        # No gotos between
        has_bad = False
        for k in range(idx2 + 1, pos):
            if re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k]):
                has_bad = True
                break
        if has_bad:
            continue
        inv1 = invert_cond(cond1)
        inv2 = invert_cond(cond2)
        if not inv1 or not inv2:
            continue
        lines[idx1] = indent + 'if (' + inv1 + ' && ' + inv2 + ') {'
        lines[idx2] = ''
        for k in range(idx2 + 1, pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[pos] = indent + '}'
        label_refcount[label] = 0
        processed.add(label)
        changes += 2
    return changes


def convert_chain_checks_to_body(lines, label_refcount, label_pos, label_sources):
    """Convert multi-ref forward gotos to same label into do-while(0)+break pattern.

    Pattern:
        if (check1) goto L_body;
        ...code1...
        if (check2) goto L_body;
        ...code2...
        if (!check3) goto L_skip;
    L_body:

    Becomes:
        do {
            if (check1) break;
            ...code1...
            if (check2) break;
            ...code2...
            if (!check3) goto L_skip;
        } while (0);
    L_body: (now unreferenced by gotos, may be removable)

    This preserves semantics because each check is evaluated with the correct
    variable values at that point in the execution flow.
    """
    changes = 0
    processed = set()
    for label, pos in label_pos.items():
        if label in processed:
            continue
        refs = label_sources.get(label, [])
        refcount = label_refcount.get(label, 0)
        if refcount < 2 or refcount > 15:
            continue
        # All refs must be conditional forward gotos
        all_cond = True
        all_forward = True
        ref_lines = sorted(refs)
        for r in ref_lines:
            if r >= pos:
                all_forward = False
                break
            if not re.match(r'^\s+if \((.+)\) goto ' + re.escape(label) + r';$', lines[r]):
                all_cond = False
                break
        if not all_cond or not all_forward:
            continue
        # The last ref should be reasonably close to the label
        if pos - ref_lines[-1] > 15:
            continue
        # Check: between last ref and label, should have no internal labels
        has_label_between = False
        for k in range(ref_lines[-1] + 1, pos):
            s = lines[k].strip()
            if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                has_label_between = True
                break
        if has_label_between:
            continue
        # Check that between consecutive refs, inner labels are only internal
        valid = True
        for ri in range(len(ref_lines) - 1):
            for k in range(ref_lines[ri] + 1, ref_lines[ri + 1]):
                s = lines[k].strip()
                if re.match(r'^L_[0-9A-Fa-f]+\s*:\s*;?\s*$', s):
                    lm = re.match(r'^(L_[0-9A-Fa-f]+)', s)
                    inner_lbl = lm.group(1)
                    for src in label_sources.get(inner_lbl, []):
                        if src < ref_lines[0] or src > pos:
                            valid = False
                            break
                if not valid:
                    break
            if not valid:
                break
        if not valid:
            continue
        # Check that brace depth is balanced from first ref to label
        depth = 0
        for k in range(ref_lines[0], pos):
            depth += lines[k].count('{') - lines[k].count('}')
        if depth != 0:
            continue

        # All gotos to this label are within the chain range
        # Wrap from first ref to just before label in do { } while(0)
        # Convert gotos to break
        indent = re.match(r'^(\s*)', lines[ref_lines[0]]).group(1)

        # Convert all gotos to break
        for r in ref_lines:
            lines[r] = re.sub(
                r'goto\s+' + re.escape(label) + r'\s*;',
                'break;',
                lines[r]
            )
            label_refcount[label] = label_refcount.get(label, 0) - 1
            changes += 1

        # Indent all lines from first ref to just before label
        for k in range(ref_lines[0], pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]

        # Insert do { at the first ref position (prepend)
        lines[ref_lines[0]] = indent + 'do {' + '\n' + lines[ref_lines[0]]

        # Insert } while(0); before label
        lines[pos] = indent + '} while (0);' + '\n' + lines[pos]

        processed.add(label)

    return changes


def convert_goto_merge_to_nested_if(lines, label_refcount, label_pos, label_sources):
    """Convert the guard chain pattern where a forward conditional goto's else branch
    contains 'r0 = 0; goto MERGE;' into a nested if:

    Pattern:
        if (!(check)) {
            r0 = 0x0;
            goto L_MERGE;
        }
        ...more_checks...

    Where L_MERGE is a multi-ref merge point. This can be converted to:
        if (check) {
            ...more_checks...
        } else {
            r0 = 0x0;
        }
        L_MERGE: ;

    But since L_MERGE is multi-ref, we can't easily remove it.
    Instead, we can wrap the whole chain in a do { ... } while(0); and convert
    the goto MERGE to break.
    """
    changes = 0
    # Find merge labels (multi-ref forward targets)
    for label, pos in label_pos.items():
        refcount = label_refcount.get(label, 0)
        if refcount < 3:
            continue
        refs = sorted(label_sources.get(label, []))
        if not refs:
            continue
        # All refs must be forward
        if any(r >= pos for r in refs):
            continue
        # All refs must be unconditional gotos
        all_uncond = all(re.match(r'^\s+goto ' + re.escape(label) + r';$', lines[r]) for r in refs)
        if not all_uncond:
            continue
        # All refs should be from "inside if blocks" - check if prev line is an assignment
        # Pattern: r0 = val; goto MERGE;
        all_assignment_before = True
        for r in refs:
            if r == 0:
                all_assignment_before = False
                break
            prev = lines[r - 1].strip()
            if not re.match(r'^r0\s*=\s*0x[0-9a-fA-F]+\s*;$', prev):
                all_assignment_before = False
                break
        if not all_assignment_before:
            continue
        # All refs are: r0 = val; goto MERGE;
        # The first ref tells us the start of the chain
        chain_start = refs[0]
        # Find what's before the chain - should be right after a function or block start

        # Wrap everything from chain_start-1 to pos in do { } while(0);
        # and convert gotos to break

        # Find the actual start of the chain (first ref's enclosing if-block)
        # This is complex - let's just convert the gotos to break and wrap in do-while(0)

        # Find the beginning of the chain region
        # It should be after some initial code
        first_ref_line = refs[0]
        # Look backwards for where the chain starts (should be before the first if-block)
        region_start = first_ref_line - 1  # r0 = val
        # Go further back to find the if statement that wraps it
        if region_start > 0:
            prev = lines[region_start - 1].strip()
            if re.match(r'^if\s*\(', prev) and '{' in prev:
                region_start = region_start - 1

        # Check if we're inside a do { } while already
        # For safety, skip if the region is too large or complex
        region_size = pos - region_start
        if region_size > 200:
            continue

        # Convert: wrap in do { } while (0); and replace gotos with break
        indent = re.match(r'^(\s*)', lines[region_start]).group(1)

        # Don't wrap if already in a loop
        in_loop = False
        for k in range(max(0, region_start - 5), region_start):
            if re.match(r'^\s*(do|while|for)\s*[\({]', lines[k]):
                in_loop = True
                break
        if in_loop:
            continue

        # Apply: wrap in do-while(0) and convert gotos to break
        lines[region_start] = indent + 'do {\n' + lines[region_start]
        for r in refs:
            lines[r] = re.sub(r'goto\s+' + re.escape(label) + r'\s*;', 'break;', lines[r])
            label_refcount[label] = label_refcount.get(label, 0) - 1
            changes += 1
        # Add closing after the last ref's enclosing if-block or before the label
        # Insert } while (0); right before the label
        lines[pos] = indent + '} while (0);\n' + lines[pos]
        # Indent the content
        for k in range(region_start + 1, pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]

    return changes


def convert_merge_gotos_to_break(lines, label_refcount, label_pos, label_sources):
    """Convert multi-ref forward unconditional gotos all targeting the same merge
    label into do { ... } while (0); + break pattern.

    Pattern (common in colosseum_event.c guard chains):
        if (check1) { r0 = 0x0; goto L_MERGE; }
        if (check2) { r0 = 0x0; goto L_MERGE; }
        r0 = 0x1;
    L_MERGE: ;

    Becomes:
        do {
            if (check1) { r0 = 0x0; break; }
            if (check2) { r0 = 0x0; break; }
            r0 = 0x1;
        } while (0);

    Only processes labels where:
    - All refs are forward unconditional gotos (no conditional gotos to the label)
    - The region is brace-balanced
    - The region contains no inner labels referenced from outside
    - All gotos are inside if-blocks (the pattern: if(...) { ...; goto MERGE; })
    """
    changes = 0
    processed = set()

    # Process from bottom up so line adjustments don't affect unprocessed labels
    sorted_labels = sorted(label_pos.items(), key=lambda x: -x[1])

    for label, pos in sorted_labels:
        if label in processed:
            continue
        refcount = label_refcount.get(label, 0)
        if refcount < 2:
            continue
        refs = sorted(label_sources.get(label, []))
        if not refs:
            continue
        # All refs must be forward (before the label)
        if any(r >= pos for r in refs):
            continue

        # All refs must be unconditional gotos (inside if-blocks typically)
        all_uncond = True
        for r in refs:
            if not re.match(r'^\s+goto ' + re.escape(label) + r';$', lines[r]):
                all_uncond = False
                break
        if not all_uncond:
            continue

        # Find the actual start of the region
        # Look for the if-block containing the first goto
        first_ref = refs[0]

        # Find the enclosing scope start: walk backwards from first_ref to find
        # the start of the if-block
        region_start = first_ref
        d = 0
        for k in range(first_ref, max(first_ref - 20, -1), -1):
            d += lines[k].count('}') - lines[k].count('{')
            if d < 0:
                region_start = k
                break

        # Region must not be too huge
        if pos - region_start > 300:
            continue

        # Check brace depth balance between region_start and pos
        depth = 0
        for k in range(region_start, pos):
            depth += lines[k].count('{') - lines[k].count('}')
        if depth != 0:
            continue

        # Check that we're not already inside a do-while(0)
        in_do_while = False
        for k in range(max(0, region_start - 3), region_start):
            if re.match(r'^\s*do\s*\{', lines[k]):
                in_do_while = True
                break
        if in_do_while:
            continue

        # Check no inner labels referenced from outside
        has_ext_ref = False
        for k in range(region_start, pos):
            lm = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if lm:
                inner_lbl = lm.group(1)
                for src in label_sources.get(inner_lbl, []):
                    if src < region_start or src >= pos:
                        has_ext_ref = True
                        break
            if has_ext_ref:
                break
        if has_ext_ref:
            continue

        # Get indent from the first line of the region
        indent_m = re.match(r'^(\s*)', lines[region_start])
        indent = indent_m.group(1) if indent_m else ''

        # Apply transformation:
        # 1. Convert all gotos to break
        for r in refs:
            lines[r] = re.sub(r'goto\s+' + re.escape(label) + r'\s*;', 'break;', lines[r])
            label_refcount[label] = label_refcount.get(label, 0) - 1
            changes += 1

        # 2. Indent all lines in the region
        for k in range(region_start, pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]

        # 3. Replace region_start line with do { + the indented original content
        # Instead of using \n, insert a new element in the list
        lines.insert(region_start, indent + 'do {')

        # 4. Insert } while (0); before the label (which shifted by 1 due to insert)
        # The label was at pos, now it's at pos+1
        lines.insert(pos + 1, indent + '} while (0);')

        processed.add(label)
        # Only process one label per call since line insertions shift indices
        break

    return changes


def convert_uncond_fwd_goto_to_else(lines, label_refcount, label_pos, label_sources):
    """Convert pattern:
        ...code_a...
        goto L_MERGE;
        ...code_b...
    L_MERGE: ;

    Where the goto is the only ref to MERGE, into:
        if (condition_that_reached_code_a) {
            ...code_a...
        } else {
            ...code_b...
        }

    But since we may not know the condition, we can at least handle the common case:
        if (cond) {
            ...
            goto L_MERGE;
        }
        ...else code...
    L_MERGE: ;

    Which becomes:
        if (cond) {
            ...
        } else {
            ...else code...
        }
    """
    changes = 0
    for i in range(len(lines) - 1, -1, -1):
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent = m.group(1)
        label = m.group(2)
        if label_refcount.get(label, 0) != 1:
            continue
        if label not in label_pos:
            continue
        label_idx = label_pos[label]
        if label_idx <= i:
            continue
        # Gap between goto and label must be reasonable
        gap = label_idx - i - 1
        if gap < 1 or gap > 100:
            continue
        # Check no other labels in between
        has_inner_label = False
        for k in range(i + 1, label_idx):
            if re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k]):
                inner_lbl = re.match(r'^\s*(L_[0-9A-Fa-f]+)', lines[k]).group(1)
                # Check if this inner label has refs from outside the range
                for src in label_sources.get(inner_lbl, []):
                    if src < i or src > label_idx:
                        has_inner_label = True
                        break
                if has_inner_label:
                    break
        if has_inner_label:
            continue
        # Check if the goto is at the end of an if-block
        # Look backwards for the enclosing if's opening brace
        # Pattern: if (...) { ... goto MERGE; }  code... L_MERGE:
        # We need the goto to be the last statement inside a { } block
        brace_before = False
        depth = 0
        for k in range(i - 1, max(i - 5, -1), -1):
            s = lines[k].strip()
            if not s:
                continue
            depth += s.count('}') - s.count('{')
            break
        # Check if there's a closing brace on the line after the goto
        # or if the goto is itself the last thing before a }
        next_nonblank = None
        for k in range(i + 1, label_idx):
            if lines[k].strip():
                next_nonblank = k
                break

        # Convert: wrap the code between goto and label in else block
        # Replace the goto with '} else {'
        # Replace the label with '}'
        # But we need the goto to be inside a { } block for this to work

        # Simpler approach: just check if it's a return-like pattern
        # Many of these are: code; goto MERGE; more_code; MERGE:
        # We can wrap in if(1) { code; } else { more_code; }
        # But that changes nothing semantically...

        # Better: look for preceding if-block
        # Check if current goto is inside an if {} block
        if_block_start = None
        d = 0
        for k in range(i, -1, -1):
            d += lines[k].count('}') - lines[k].count('{')
            if d < 0:
                # Found the opening brace
                if re.match(r'^\s*if\s*\(', lines[k]):
                    if_block_start = k
                break

        if if_block_start is None:
            continue

        # Check there's a } right after the goto
        closing_brace_idx = None
        for k in range(i + 1, min(i + 3, label_idx)):
            s = lines[k].strip()
            if not s:
                continue
            if s == '}':
                closing_brace_idx = k
            break

        if closing_brace_idx is None:
            continue

        # Now we have:
        # if (...) {
        #     ...code...
        #     goto MERGE;
        # }
        # ...else_code...
        # MERGE:

        # Convert the goto to nothing (remove it), change } to } else {, and add } at label
        lines[i] = ''  # remove goto
        lines[closing_brace_idx] = indent[:-4] + '} else {' if len(indent) >= 4 else '} else {'
        # Indent the else code
        for k in range(closing_brace_idx + 1, label_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label_idx] = indent[:-4] + '}' if len(indent) >= 4 else '}'

        label_refcount[label] = 0
        changes += 1

    return changes


def convert_cond_goto_multi_ref(lines, label_refcount, label_pos, label_sources):
    """Convert conditional forward gotos where the target has 2 refs from adjacent lines.

    Pattern:
        if (cond1) goto L;
        if (cond2) goto L;
        ...code...
    L: ;

    Where both refs are the only refs. Convert to:
        if (!(cond1) && !(cond2)) {
            ...code...
        }
    """
    changes = 0
    processed = set()

    for label, pos in sorted(label_pos.items(), key=lambda x: x[1]):
        if label in processed:
            continue
        refcount = label_refcount.get(label, 0)
        if refcount < 2 or refcount > 6:
            continue
        refs = sorted(label_sources.get(label, []))
        if not refs or any(r >= pos for r in refs):
            continue

        # Check if all refs are consecutive conditional gotos
        all_consecutive = True
        all_conditional = True
        conditions = []

        for idx, r in enumerate(refs):
            m = re.match(r'^\s+if \((.+)\) goto ' + re.escape(label) + r';$', lines[r])
            if not m:
                all_conditional = False
                break
            conditions.append(m.group(1))
            if idx > 0 and r != refs[idx-1] + 1:
                # Allow one blank line between
                gap_ok = True
                for k in range(refs[idx-1] + 1, r):
                    if lines[k].strip():
                        gap_ok = False
                        break
                if not gap_ok:
                    all_consecutive = False
                    break

        if not all_conditional or not all_consecutive:
            continue

        last_ref = refs[-1]
        # Check no labels between last ref and target
        has_inner = False
        for k in range(last_ref + 1, pos):
            if re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k]):
                has_inner = True
                break
        if has_inner:
            continue

        # Check no external gotos in the body
        has_ext_goto = False
        for k in range(last_ref + 1, pos):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm:
                tgt = gm.group(1)
                if tgt in label_pos:
                    t = label_pos[tgt]
                    if t < refs[0] or t > pos:
                        has_ext_goto = True
                        break
        if has_ext_goto:
            continue

        # Build negated combined condition
        neg_conds = []
        valid = True
        for c in conditions:
            inv = invert_cond(c)
            if inv is None:
                valid = False
                break
            neg_conds.append(inv)
        if not valid:
            continue

        # Check there's actually body code between last ref and label
        has_body = False
        for k in range(last_ref + 1, pos):
            if lines[k].strip():
                has_body = True
                break
        if not has_body:
            continue

        indent_m = re.match(r'^(\s*)', lines[refs[0]])
        indent = indent_m.group(1) if indent_m else '    '

        # Replace: combine all conditions, wrap body
        combined = ' && '.join(neg_conds)
        lines[refs[0]] = indent + 'if (' + combined + ') {'
        # Remove other ref lines
        for r in refs[1:]:
            lines[r] = ''
        # Indent body
        for k in range(last_ref + 1, pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[pos] = indent + '}'

        label_refcount[label] = 0
        changes += len(refs)
        processed.add(label)

    return changes


def convert_dispatch_chain_to_if_else(lines, label_refcount, label_pos, label_sources):
    """Convert binary search dispatch chains into nested if/else.

    This handles the common MWCC switch pattern:
        if ((s32)tmp == 0x1f7) goto L_case1;
        if ((s32)tmp >= 0x1f7) goto L_branch2;
        if ((s32)tmp >= 0x1f6) goto L_case2;
        return;
    L_branch2:
        if ((s32)tmp == 0x1291) goto L_case3;
        return;
    L_case1:
        ...case1 code...
        return;
    L_case2:
        ...case2 code...
        return;
    L_case3:
        ...case3 code...
        return;

    These are already partially structured by the earlier passes.
    Focus on the simpler remaining patterns.
    """
    # This is very complex to handle generically. Skip for now and
    # handle via the merge-point approach.
    return 0


def convert_goto_return_label(lines, label_refcount, label_pos, label_sources):
    """Convert gotos to labels that are followed only by return or
    a simple assignment + return.

    Only handles the simple case: goto L; ... L: return;
    For assignments before return, only handles unconditional gotos to avoid
    multi-line insertions that break the lines array.
    """
    changes = 0
    # Find labels followed immediately by return
    pure_return_labels = set()
    assign_return_labels = {}  # label -> (assignment_line, return_line)

    for lbl, pos in label_pos.items():
        for k in range(pos + 1, min(pos + 4, len(lines))):
            s = lines[k].strip()
            if not s:
                continue
            if s == 'return;':
                pure_return_labels.add(lbl)
            break

    # Convert gotos to pure return labels
    for i in range(len(lines)):
        line = lines[i]
        # Unconditional goto -> return
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', line)
        if m and m.group(2) in pure_return_labels:
            lines[i] = m.group(1) + 'return;'
            label_refcount[m.group(2)] = label_refcount.get(m.group(2), 0) - 1
            changes += 1
            continue
        # Conditional goto -> if (cond) return;
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if m and m.group(3) in pure_return_labels:
            lines[i] = m.group(1) + 'if (' + m.group(2) + ') return;'
            label_refcount[m.group(3)] = label_refcount.get(m.group(3), 0) - 1
            changes += 1

    return changes


def convert_if_goto_else_block(lines, label_refcount, label_pos, label_sources):
    """Convert pattern where if-block contains only a goto to merge:

        if (cond) {
            goto L_MERGE;
        }
        ...else_code...
    (more gotos to L_MERGE from other branches also exist)

    Into:
        if (!(cond)) {
            ...else_code...
            goto L_MERGE;  (add goto at end if needed to maintain flow)
        }

    This eliminates one goto per such block.
    """
    changes = 0
    for i in range(len(lines)):
        # Look for: if (cond) {
        m_if = re.match(r'^(\s+)if \((.+)\) \{$', lines[i])
        if not m_if:
            continue
        indent = m_if.group(1)
        cond = m_if.group(2)

        # Next non-blank should be a goto
        goto_idx = None
        goto_label = None
        for k in range(i + 1, min(i + 4, len(lines))):
            s = lines[k].strip()
            if not s:
                continue
            mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', s)
            if mg:
                goto_idx = k
                goto_label = mg.group(1)
            break

        if goto_idx is None or goto_label is None:
            continue

        # Next should be closing brace
        close_idx = None
        for k in range(goto_idx + 1, min(goto_idx + 3, len(lines))):
            s = lines[k].strip()
            if not s:
                continue
            if s == '}':
                close_idx = k
            break

        if close_idx is None:
            continue

        # Check if merge label is forward
        if goto_label not in label_pos:
            continue
        merge_pos = label_pos[goto_label]
        if merge_pos <= close_idx:
            continue

        # Check what's between close_idx and merge_pos
        # There should be some code (the "else" body)
        body_start = close_idx + 1
        has_body = False
        for k in range(body_start, merge_pos):
            if lines[k].strip():
                has_body = True
                break
        if not has_body:
            continue

        # Check no inner labels between close_idx and merge_pos that have external refs
        has_ext_label = False
        for k in range(body_start, merge_pos):
            lm = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if lm:
                inner_lbl = lm.group(1)
                for src in label_sources.get(inner_lbl, []):
                    if src < i or src > merge_pos:
                        has_ext_label = True
                        break
                if has_ext_label:
                    break
        if has_ext_label:
            continue

        # Check the body doesn't have gotos going outside
        # (except gotos to the same merge label, which are fine)
        has_ext_goto = False
        for k in range(body_start, merge_pos):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) != goto_label:
                tgt = gm.group(1)
                if tgt in label_pos:
                    t = label_pos[tgt]
                    if t < i or t > merge_pos:
                        has_ext_goto = True
                        break
        if has_ext_goto:
            continue

        # Check if the body ends with a goto to merge or return
        body_ends_with_goto_or_return = False
        for k in range(merge_pos - 1, body_start - 1, -1):
            s = lines[k].strip()
            if not s:
                continue
            if s == 'return;' or re.match(r'^return\s+', s):
                body_ends_with_goto_or_return = True
            elif re.match(r'^goto ' + re.escape(goto_label) + r';$', s):
                body_ends_with_goto_or_return = True
            break

        # Invert condition
        inv = invert_cond(cond)
        if inv is None:
            continue

        # Only handle cases where body ends with return/goto (avoids needing to add goto)
        if not body_ends_with_goto_or_return:
            continue

        # Apply transformation
        lines[i] = indent + 'if (' + inv + ') {'
        lines[goto_idx] = ''  # remove goto
        lines[close_idx] = ''  # remove }

        # Indent the else body
        for k in range(body_start, merge_pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[merge_pos] = indent + '}'

        label_refcount[goto_label] = label_refcount.get(goto_label, 0) - 1
        changes += 1

    return changes


def convert_guard_set_goto_merge(lines, label_refcount, label_pos, label_sources):
    """Convert pattern:
        if (cond) {
            rN = VALUE;
            goto L_MERGE;
        }
        rN = OTHER_VALUE;
    L_MERGE: ;

    Into:
        if (cond) {
            rN = VALUE;
        } else {
            rN = OTHER_VALUE;
        }

    This is the most common remaining pattern in colosseum_event.c.
    Handles single-ref labels only.
    """
    changes = 0
    for i in range(len(lines) - 1, -1, -1):
        # Look for: goto L_LABEL; (unconditional)
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent = m.group(1)
        label = m.group(2)
        if label_refcount.get(label, 0) != 1:
            continue
        if label not in label_pos:
            continue
        label_idx = label_pos[label]
        if label_idx <= i:
            continue

        # The goto must be followed by } then assignment then L_MERGE:
        # Pattern:
        #   line i:   goto MERGE;
        #   line i+1: }
        #   line i+2: rN = OTHER; (1-3 assignment lines)
        #   line label_idx: L_MERGE: ;

        # Check closing brace right after goto
        close_idx = None
        for k in range(i + 1, min(i + 3, len(lines))):
            s = lines[k].strip()
            if not s:
                continue
            if s == '}':
                close_idx = k
            break

        if close_idx is None:
            continue

        # Check that between close_idx+1 and label_idx there are 1-3 assignment lines
        body_lines = []
        valid = True
        for k in range(close_idx + 1, label_idx):
            s = lines[k].strip()
            if not s:
                continue
            # Must be a simple assignment (rN = val; or similar)
            if re.match(r'^(r\d+|tmp|r0)\s*=\s*.+;$', s):
                body_lines.append(k)
            elif re.match(r'^L_[0-9A-Fa-f]+\s*:', s):
                valid = False
                break
            elif re.search(r'\bgoto\b', s):
                valid = False
                break
            else:
                valid = False
                break

        if not valid or len(body_lines) < 1 or len(body_lines) > 3:
            continue

        # Check there's an if-block containing the goto
        # The goto should be preceded by an assignment within a { } block
        # Look backwards from the goto to find the if ( ... ) {
        if_line = None
        d = 0
        for k in range(i, max(i - 10, -1), -1):
            d += lines[k].count('}') - lines[k].count('{')
            if d < 0:
                if re.match(r'^\s*if\s*\(', lines[k]):
                    if_line = k
                break

        if if_line is None:
            continue

        # Convert: the goto becomes the end of the if-block,
        # the } after becomes } else {, and the assignment lines become the else body,
        # and the label becomes }
        lines[i] = ''  # remove goto
        # Find the indent of the if statement for proper } else { indentation
        if_indent_m = re.match(r'^(\s*)', lines[if_line])
        if_indent = if_indent_m.group(1) if if_indent_m else ''

        lines[close_idx] = if_indent + '} else {'
        # Indent the else body lines
        for k in body_lines:
            lines[k] = if_indent + '    ' + lines[k].strip()
        lines[label_idx] = if_indent + '}'

        label_refcount[label] = 0
        changes += 1

    return changes


def convert_cond_goto_with_assign_merge(lines, label_refcount, label_pos, label_sources):
    """Convert pattern where conditional goto is inside if-block with assignment:
        if (cond) {
            rN = 0x0;
            goto L_MERGE;
        }

    Where L_MERGE is multi-ref but all refs follow this same pattern.
    Convert each ref individually when possible using the guard_set pattern.
    """
    # This is handled by convert_guard_set_goto_merge for single-ref.
    # For multi-ref, we need a different approach.
    # For now, just return 0.
    return 0


def convert_assign_goto_to_assign(lines, label_refcount, label_pos, label_sources):
    """Convert pattern where an unconditional goto targets a label followed by
    code that continues execution. When the goto is the only ref and the label
    has no other references entering it, convert by just removing the goto and label.

    Also handles: rN = val; goto L; ... L: ; where L is followed by r0 = r0 & 0xFF;
    These are common in colosseum_event.c guard chains.

    Specifically targets:
        if (cond) {
            r0 = 0x0;
            goto L_MERGE;
        }
        r0 = 0x1;
    L_MERGE: ;
        r0 = r0 & 0xFF;

    Where multiple gotos target MERGE, this pattern can be converted by
    recognizing that the if(cond) { r0=0; goto MERGE; } r0=1; MERGE:
    is equivalent to r0 = cond ? 0 : 1; (with r0 being a byte flag).

    For each if-block ending in { r0 = 0x0; goto MERGE; } preceded by r0 = 0x1;
    at the MERGE label, we convert to:
        if (!(cond)) {
            r0 = 0x1;
        }
    """
    changes = 0
    # This needs to work differently - find patterns from the goto side
    for i in range(len(lines) - 1, -1, -1):
        # Look for: r0 = 0x0; followed by goto L on next line, inside { }
        line = lines[i].strip()
        if line != 'r0 = 0x0;':
            continue
        # Next non-blank should be a goto
        goto_idx = None
        goto_label = None
        for k in range(i + 1, min(i + 3, len(lines))):
            s = lines[k].strip()
            if not s:
                continue
            mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', s)
            if mg:
                goto_idx = k
                goto_label = mg.group(1)
            break

        if goto_idx is None:
            continue

        if goto_label not in label_pos:
            continue
        label_idx = label_pos[goto_label]
        if label_idx <= goto_idx:
            continue

        # Check there's a } after the goto
        close_idx = None
        for k in range(goto_idx + 1, min(goto_idx + 3, len(lines))):
            s = lines[k].strip()
            if not s:
                continue
            if s == '}':
                close_idx = k
            break

        if close_idx is None:
            continue

        # Check that between close_idx and label_idx there's exactly "r0 = 0x1;"
        has_r0_1 = False
        r0_1_idx = None
        valid = True
        for k in range(close_idx + 1, label_idx):
            s = lines[k].strip()
            if not s:
                continue
            if s == 'r0 = 0x1;' and not has_r0_1:
                has_r0_1 = True
                r0_1_idx = k
            elif re.match(r'^L_[0-9A-Fa-f]+\s*:', s):
                valid = False
                break
            elif re.search(r'\bgoto\b', s):
                valid = False
                break
            else:
                valid = False
                break

        if not valid or not has_r0_1:
            continue

        # Find the enclosing if statement
        if_line = None
        d = 0
        for k in range(i - 1, max(i - 10, -1), -1):
            s = lines[k].strip()
            d += s.count('}') - s.count('{')
            if d < 0:
                if re.match(r'^if\s*\(', s):
                    if_line = k
                break

        if if_line is None:
            continue

        # Get the if condition
        if_m = re.match(r'^(\s*)if \((.+)\) \{$', lines[if_line])
        if not if_m:
            continue

        if_indent = if_m.group(1)
        cond = if_m.group(2)

        # Invert condition
        inv = invert_cond(cond)
        if inv is None:
            continue

        # Apply transformation:
        # Replace: if (cond) { r0=0; goto MERGE; } r0=1;
        # With: if (!(cond)) { r0=0x1; } else { r0=0x0; }
        # But wait - this is simpler: just replace with
        # r0 = (cond) ? 0x0 : 0x1; but MWCC doesn't have ternary elegantly
        # Better: if (!(cond)) { r0 = 0x1; }
        # since r0 starts at 0 (set by the original r0 = 0x0 in the if branch)
        # Actually no, we need to keep both paths.
        #
        # Simplest safe transform:
        # if (cond) { r0 = 0; } else { r0 = 1; }
        lines[if_line] = if_indent + 'if (' + cond + ') {'
        lines[i] = if_indent + '    r0 = 0x0;'
        lines[goto_idx] = ''  # remove goto
        lines[close_idx] = if_indent + '} else {'
        lines[r0_1_idx] = if_indent + '    r0 = 0x1;'

        # If this was the only ref to the label, remove it
        label_refcount[goto_label] = label_refcount.get(goto_label, 0) - 1
        if label_refcount.get(goto_label, 0) <= 0:
            lines[label_idx] = if_indent + '}'
        else:
            # Can't remove the label, but we still removed the goto
            # Need to add closing brace before the label
            # Insert } before the label
            # This is tricky with line arrays... skip for now
            # Actually we need to put } right before the label
            # The safest thing: put } on the r0_1_idx line after the assignment
            # Actually label_idx should have the label, and we need } before it
            # Let's just add } to the line before label
            # Find last non-empty line before label
            for k in range(label_idx - 1, r0_1_idx, -1):
                if lines[k].strip():
                    lines[k] = lines[k] + ' /* end else */'
                    break
            # We can't easily add } here without breaking line numbering
            # So only do this transform when the label has 1 ref
            # Undo everything
            lines[if_line] = if_m.group(0)
            lines[i] = if_indent + '    r0 = 0x0;'
            lines[goto_idx] = if_indent + '    goto ' + goto_label + ';'
            lines[close_idx] = if_indent + '}'
            lines[r0_1_idx] = if_indent + 'r0 = 0x1;'
            label_refcount[goto_label] = label_refcount.get(goto_label, 0) + 1
            for k in range(label_idx - 1, r0_1_idx, -1):
                if '/* end else */' in lines[k]:
                    lines[k] = lines[k].replace(' /* end else */', '')
                    break
            continue

        changes += 1

    return changes


def process_file(filepath, verbose=False):
    """Process a single file through all conversion passes."""
    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()
    if 'goto' not in content:
        return 0

    lines = content.split('\n')
    initial_gotos = sum(1 for line in lines if re.search(r'\bgoto\b', line))

    total_changes = 0
    for iteration in range(50):
        changes = 0

        # Rebuild indices each sub-pass
        rc, lp, ls = build_indices(lines)

        # 1: Multi-condition OR
        changes += convert_multi_condition_or(lines, rc, lp)

        # 2: Dead gotos
        rc, lp, ls = build_indices(lines)
        changes += remove_dead_gotos(lines, rc, lp)

        # 3: Goto to return
        rc, lp, ls = build_indices(lines)
        changes += convert_goto_to_return(lines, rc, lp)

        # 3b: Goto to return-label (inline short return code)
        rc, lp, ls = build_indices(lines)
        changes += convert_goto_return_label(lines, rc, lp, ls)

        # 4: Guard chain (small forward skips with inner gotos)
        rc, lp, ls = build_indices(lines)
        changes += convert_guard_chain(lines, rc, lp)

        # 5: Forward singles allowing inner gotos
        rc, lp, ls = build_indices(lines)
        changes += convert_forward_allow_inner_gotos(lines, rc, lp, ls)

        # 5b: Multi-ref conditional forward gotos (adjacent conditions -> AND)
        rc, lp, ls = build_indices(lines)
        changes += convert_cond_goto_multi_ref(lines, rc, lp, ls)

        # 6: Multi-ref forward (2 adjacent -> AND)
        rc, lp, ls = build_indices(lines)
        changes += convert_multi_ref_forward_2(lines, rc, lp, ls)

        # 6b: if { goto MERGE } else_code -> if(!(cond)) { else_code }
        # (disabled - causes brace nesting corruption)
        # rc, lp, ls = build_indices(lines)
        # changes += convert_if_goto_else_block(lines, rc, lp, ls)

        # 6c: Unconditional forward goto (single ref) -> else block
        # (disabled - causes brace nesting corruption)
        # rc, lp, ls = build_indices(lines)
        # changes += convert_uncond_fwd_goto_to_else(lines, rc, lp, ls)

        # 7: Do-while permissive
        rc, lp, ls = build_indices(lines)
        changes += convert_do_while_permissive(lines, rc, lp, ls)

        # 7b: Unconditional backward -> do-while(1)
        rc, lp, ls = build_indices(lines)
        changes += convert_uncond_backward_to_loop(lines, rc, lp, ls)

        # 8: While with update
        rc, lp, ls = build_indices(lines)
        changes += convert_while_loop_with_update(lines, rc, lp)

        # 9: Break from loop
        rc, lp, ls = build_indices(lines)
        changes += convert_goto_after_loop_to_break(lines, rc, lp)

        # 9c: Merge gotos to break (do-while(0) wrapping)
        rc, lp, ls = build_indices(lines)
        changes += convert_merge_gotos_to_break(lines, rc, lp, ls)

        # 10: Continue in loop
        rc, lp, ls = build_indices(lines)
        changes += convert_goto_to_continue(lines, rc, lp)

        # 11: Guard set goto merge (if{r0=0;goto M;} r0=1; M: -> if/else)
        rc, lp, ls = build_indices(lines)
        changes += convert_guard_set_goto_merge(lines, rc, lp, ls)

        # 11b: Assign+goto merge -> if/else (r0=0; goto M; ... r0=1; M:)
        rc, lp, ls = build_indices(lines)
        changes += convert_assign_goto_to_assign(lines, rc, lp, ls)

        # 12: Remove unreferenced labels
        rc, lp, ls = build_indices(lines)
        changes += remove_unreferenced_labels(lines, rc, lp)

        total_changes += changes
        if changes == 0:
            break

    final_gotos = sum(1 for line in lines if re.search(r'\bgoto\b', line))

    with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
        f.write('\n'.join(lines))

    if verbose:
        print(f'  {filepath}: {initial_gotos} -> {final_gotos} gotos ({initial_gotos - final_gotos} removed)')

    return initial_gotos - final_gotos


def main():
    verbose = '--verbose' in sys.argv or '-v' in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not files:
        files = [
            'src/game/colosseum_event.c',
            'src/game/colosseum_battle.c',
            'src/game/colosseum_script.c',
        ]

    total = 0
    for f in files:
        total += process_file(f, verbose)

    print(f'Total gotos removed: {total}')


if __name__ == '__main__':
    main()
