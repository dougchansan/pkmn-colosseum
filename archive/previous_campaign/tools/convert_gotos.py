#!/usr/bin/env python3
"""Convert goto-based patterns to structured control flow.

Enhanced version with additional pattern support:
- Forward single-ref gotos -> if blocks
- If/else patterns
- Do/while (backward single-ref)
- While-loop (goto cond; body; cond: if exit; if body)
- Multi-target OR conditions (if x==A goto L; if x!=B goto M; L:)
- Dead goto-to-next-label removal
- Unconditional goto + label on next line -> removal
- Two-condition while loop with init goto
"""
import re
import glob
import sys

def invert_cond(cond):
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


def build_indices_compat(lines):
    rc, lp, _ = build_indices(lines)
    return rc, lp

def convert_forward_singles(lines, label_refcount, label_pos):
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
        has_other = any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(i+1, label_idx))
        if has_other:
            continue
        has_bad = False
        for k in range(i+1, label_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) in label_pos:
                t = label_pos[gm.group(1)]
                if t < i or t > label_idx:
                    has_bad = True
                    break
        if has_bad:
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

def convert_if_else(lines, label_refcount, label_pos):
    changes = 0
    for i in range(len(lines)-1, -1, -1):
        line = lines[i]
        m_cond = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if not m_cond:
            continue
        indent, cond, label1 = m_cond.group(1), m_cond.group(2), m_cond.group(3)
        if label1 not in label_pos or label_refcount.get(label1, 0) != 1:
            continue
        label1_idx = label_pos[label1]
        if label1_idx <= i:
            continue
        uncond_idx = uncond_label = None
        for k in range(i+1, label1_idx):
            m_u = re.match(r'^\s+goto (L_[0-9A-Fa-f]+);$', lines[k])
            if m_u:
                uncond_idx = k
                uncond_label = m_u.group(1)
                break
        if not uncond_idx or not uncond_label:
            continue
        if uncond_label not in label_pos or label_refcount.get(uncond_label, 0) != 1:
            continue
        label2_idx = label_pos[uncond_label]
        if label2_idx <= label1_idx:
            continue
        if any(lines[k].strip() and not lines[k].strip().startswith('//') for k in range(uncond_idx+1, label1_idx)):
            continue
        if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(i+1, uncond_idx)):
            continue
        if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(label1_idx+1, label2_idx)):
            continue
        has_bad = False
        for rng in [(i+1, uncond_idx), (label1_idx+1, label2_idx)]:
            for k in range(*rng):
                gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
                if gm and gm.group(1) in label_pos:
                    t = label_pos[gm.group(1)]
                    if t < i or t > label2_idx:
                        has_bad = True
                        break
            if has_bad:
                break
        if has_bad:
            continue
        inv = invert_cond(cond)
        if inv is None:
            continue
        lines[i] = indent + 'if (' + inv + ') {'
        for k in range(i+1, uncond_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[uncond_idx] = indent + '} else {'
        lines[label1_idx] = ''
        for k in range(label1_idx+1, label2_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label2_idx] = indent + '}'
        label_refcount[label1] = 0
        label_refcount[uncond_label] = 0
        changes += 2
    return changes

def convert_do_while(lines, label_refcount, label_pos):
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
        has_other = any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(label_idx+1, i))
        if has_other:
            continue
        has_bad = False
        for k in range(label_idx+1, i):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) in label_pos:
                t = label_pos[gm.group(1)]
                if t < label_idx or t > i:
                    has_bad = True
                    break
        if has_bad:
            continue
        lines[label_idx] = indent + 'do {'
        for k in range(label_idx+1, i):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[i] = indent + '} while (' + cond + ');'
        changes += 1
        label_refcount[label] = 0
    return changes

def convert_while_loop(lines, label_refcount, label_pos):
    """Convert pattern:
        goto L_cond;
    L_body:
        ...body...
    L_cond:
        if (exitCond) return/goto_out;
        if (loopCond) goto L_body;

    Into:
        while (!exitCond && loopCond) {
            ...body...
        }

    Also handles two-condition while:
        goto L_cond;
    L_body:
        ...body...
    L_cond:
        if (cond1) goto L_exit;
        if (cond2) goto L_body;
    L_exit:
    """
    changes = 0
    for i in range(len(lines)):
        # Look for unconditional goto (the init jump)
        m_goto = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m_goto:
            continue
        indent = m_goto.group(1)
        cond_label = m_goto.group(2)
        if cond_label not in label_pos:
            continue
        cond_idx = label_pos[cond_label]
        if cond_idx <= i:
            continue

        # Check next line is a label (body label)
        body_label = None
        body_idx = None
        for k in range(i+1, cond_idx):
            m_body = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if m_body:
                body_label = m_body.group(1)
                body_idx = k
                break
        if body_label is None or body_idx != i + 1:
            continue

        # At cond_idx, look for the condition pattern
        # Pattern 1: two lines - "if (exitCond) return;" + "if (loopCond) goto L_body;"
        # Pattern 2: two lines - "if (exitCond) goto L_exit;" + "if (loopCond) goto L_body;"
        # Pattern 3: one line - "if (loopCond) goto L_body;" (preceded by "if (exitCond) return;")

        # Look at lines after the cond label
        lines_after_cond = []
        k = cond_idx + 1
        while k < len(lines) and k < cond_idx + 5:
            stripped = lines[k].strip()
            if stripped:
                lines_after_cond.append((k, stripped))
            k += 1

        if len(lines_after_cond) < 1:
            continue

        # Find the backward goto to body
        back_goto_idx = None
        back_goto_cond = None
        exit_cond_lines = []

        for idx, (line_idx, stripped) in enumerate(lines_after_cond):
            m_back = re.match(r'^if \((.+)\) goto (L_[0-9A-Fa-f]+);$', stripped)
            if m_back and m_back.group(2) == body_label:
                back_goto_idx = line_idx
                back_goto_cond = m_back.group(1)
                break
            else:
                exit_cond_lines.append((line_idx, stripped))

        if back_goto_idx is None:
            continue

        # Check refcounts - cond_label should only be referenced by our goto
        if label_refcount.get(cond_label, 0) != 1:
            continue
        # body_label referenced by the back-goto (and possibly the init, but we check)
        if label_refcount.get(body_label, 0) != 1:
            continue

        # No other labels between body and cond
        has_other_labels = False
        for k in range(body_idx + 1, cond_idx):
            m_lbl = re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k])
            if m_lbl:
                has_other_labels = True
                break
        if has_other_labels:
            continue

        # No gotos in body that jump outside the loop
        has_bad = False
        for k in range(body_idx + 1, cond_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) in label_pos:
                t = label_pos[gm.group(1)]
                if t < body_idx or t > back_goto_idx:
                    has_bad = True
                    break
        if has_bad:
            continue

        # Build the while condition
        # exit_cond_lines contains conditions that exit the loop
        cond_parts = []
        exit_lines_to_remove = []
        valid = True
        for line_idx, stripped in exit_cond_lines:
            # "if (cond) return;" -> while (!cond)
            m_ret = re.match(r'^if \((.+)\) return;$', stripped)
            if m_ret:
                inv = invert_cond(m_ret.group(1))
                if inv is None:
                    valid = False
                    break
                cond_parts.append(inv)
                exit_lines_to_remove.append(line_idx)
                continue
            # "if (cond) goto L_exit;" where L_exit is right after back_goto
            m_exit = re.match(r'^if \((.+)\) goto (L_[0-9A-Fa-f]+);$', stripped)
            if m_exit:
                exit_label = m_exit.group(2)
                if exit_label in label_pos:
                    exit_pos = label_pos[exit_label]
                    # exit label should be right after back_goto_idx (or close)
                    if exit_pos == back_goto_idx + 1 or exit_pos == back_goto_idx + 2:
                        inv = invert_cond(m_exit.group(1))
                        if inv is None:
                            valid = False
                            break
                        cond_parts.append(inv)
                        exit_lines_to_remove.append(line_idx)
                        continue
            valid = False
            break

        if not valid:
            continue

        # Add the back-goto condition
        cond_parts.append(back_goto_cond)

        # Build while condition
        while_cond = ' && '.join(cond_parts)

        # Apply transformation
        lines[i] = indent + 'while (' + while_cond + ') {'
        lines[body_idx] = ''  # remove body label
        for k in range(body_idx + 1, cond_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[cond_idx] = ''  # remove cond label
        for line_idx in exit_lines_to_remove:
            lines[line_idx] = ''
        lines[back_goto_idx] = indent + '}'

        label_refcount[cond_label] = 0
        label_refcount[body_label] = 0
        changes += 2  # removed at least the init goto + the back goto

    return changes

def convert_while_loop_simple(lines, label_refcount, label_pos):
    """Convert simpler while-loop pattern where condition evaluation
    code appears between the cond label and the back-jump:

        goto L_cond;
    L_body:
        ...body...
    L_cond:
        ...cond eval code...
        if (loopCond) goto L_body;

    Into:
        while (1) {
            ...cond eval code...
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

        # Find body label right after the goto
        body_label = None
        body_idx = None
        for k in range(i+1, min(i+3, cond_idx)):
            m_body = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if m_body:
                body_label = m_body.group(1)
                body_idx = k
                break
        if body_label is None or body_idx != i + 1:
            i += 1
            continue

        # Both labels must have refcount 1
        if label_refcount.get(cond_label, 0) != 1:
            i += 1
            continue
        if label_refcount.get(body_label, 0) != 1:
            i += 1
            continue

        # No other labels in body
        has_other_labels = False
        for k in range(body_idx + 1, cond_idx):
            if re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]):
                has_other_labels = True
                break
        if has_other_labels:
            i += 1
            continue

        # Find the back-jump to body_label after cond_idx
        back_goto_idx = None
        back_goto_cond = None
        for k in range(cond_idx + 1, min(cond_idx + 10, len(lines))):
            stripped = lines[k].strip()
            if not stripped:
                continue
            m_back = re.match(r'^if \((.+)\) goto (L_[0-9A-Fa-f]+);$', stripped)
            if m_back and m_back.group(2) == body_label:
                back_goto_idx = k
                back_goto_cond = m_back.group(1)
                break
            # If we hit a label or unconditional goto, stop
            if re.match(r'^L_[0-9A-Fa-f]+\s*:', stripped):
                break
            if re.match(r'^goto\s+', stripped):
                break

        if back_goto_idx is None:
            i += 1
            continue

        # Check no gotos in body that jump outside
        has_bad = False
        for k in range(body_idx + 1, cond_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) in label_pos:
                t = label_pos[gm.group(1)]
                if t < body_idx or t > back_goto_idx:
                    has_bad = True
                    break
        if has_bad:
            i += 1
            continue

        # Check no gotos in cond eval code that jump outside
        for k in range(cond_idx + 1, back_goto_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm:
                has_bad = True
                break
        if has_bad:
            i += 1
            continue

        # Invert condition for break
        inv_cond = invert_cond(back_goto_cond)
        if inv_cond is None:
            i += 1
            continue

        # Collect cond eval lines (between cond label and back-jump)
        cond_eval_lines = []
        for k in range(cond_idx + 1, back_goto_idx):
            if lines[k].strip():
                cond_eval_lines.append(k)

        # Apply transformation:
        # goto -> while (1) {
        lines[i] = indent + 'while (1) {'
        # body label -> empty
        lines[body_idx] = ''
        # Cond eval code goes first in loop (indented)
        for k in cond_eval_lines:
            lines[k] = '    ' + lines[k]
        # Move cond eval + break to before body
        # Actually, rearrange: put cond eval + break at top of loop body
        cond_eval_content = []
        for k in cond_eval_lines:
            cond_eval_content.append(lines[k])
            lines[k] = ''
        # Back-jump becomes break
        lines[back_goto_idx] = ''
        # Cond label becomes empty
        lines[cond_idx] = ''

        # Now insert: cond eval + break at body_idx+1 position
        # But we need to be careful with line indices
        # Instead, let's restructure differently:
        # Put while(1) at goto line
        # Move cond eval + break into beginning of body
        insert_lines = cond_eval_content + [indent + '    if (' + inv_cond + ') break;']

        # Body lines need indenting
        for k in range(body_idx + 1, cond_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]

        # Insert cond eval after the (now empty) body_idx
        # Replace body_idx with the cond eval + break
        lines[body_idx] = '\n'.join(insert_lines)

        # Close the while after cond area
        lines[back_goto_idx] = indent + '}'

        label_refcount[cond_label] = 0
        label_refcount[body_label] = 0
        changes += 2
        i += 1

    return changes

def remove_dead_gotos(lines, label_refcount, label_pos):
    """Remove patterns like:
        goto L_X;
    L_X:
    Where goto jumps to the very next label (effectively a no-op).
    Also handles: goto L_X; L_X: goto L_Y; -> goto L_Y; (chain collapse)
    """
    changes = 0
    for i in range(len(lines) - 1):
        m_goto = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m_goto:
            continue
        label = m_goto.group(2)
        # Find the next non-empty line
        next_idx = None
        for k in range(i + 1, min(i + 5, len(lines))):
            if lines[k].strip():
                next_idx = k
                break
        if next_idx is None:
            continue
        # Check if next non-empty line is the target label
        m_lbl = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[next_idx])
        if m_lbl and m_lbl.group(1) == label:
            lines[i] = ''
            label_refcount[label] = label_refcount.get(label, 0) - 1
            changes += 1
    return changes

def remove_unreferenced_labels(lines, label_refcount, label_pos):
    """Remove labels that have zero references."""
    changes = 0
    for label, pos in label_pos.items():
        if label_refcount.get(label, 0) <= 0:
            # Make sure the line is just a label
            m = re.match(r'^(\s*)(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[pos])
            if m:
                lines[pos] = ''
                changes += 1
    return changes

def convert_multi_target_or(lines, label_refcount, label_pos):
    """Convert pattern:
        if (x == A) goto L_target;
        if (x != B) goto L_other;
    L_target: ;
        ...code...

    Into:
        if (x == A || x == B) {
            ...code...
        }

    This handles the common multi-target branch where two conditions
    share the same target label, forming an OR condition.
    """
    changes = 0
    for i in range(len(lines) - 1):
        # Match first: if (x == A) goto L_target;
        m1 = re.match(r'^(\s+)if \((.+) == (.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m1:
            continue
        indent = m1.group(1)
        var1 = m1.group(2)
        val1 = m1.group(3)
        target = m1.group(4)

        if target not in label_pos:
            continue
        target_idx = label_pos[target]

        # Match second: if (x != B) goto L_other; on the next line
        next_line = i + 1
        if next_line >= len(lines):
            continue
        m2 = re.match(r'^(\s+)if \((.+) != (.+)\) goto (L_[0-9A-Fa-f]+);$', lines[next_line])
        if not m2:
            continue
        var2 = m2.group(2)
        val2 = m2.group(3)
        other_label = m2.group(4)

        # Variables must match
        if var1 != var2:
            continue

        # Target label must be right after the second if (line i+2 or thereabouts)
        if target_idx != next_line + 1 and target_idx != next_line + 2:
            continue

        # This pattern: if (x == A) goto target; if (x != B) goto other; target:
        # Means: if x == A OR x == B, fall through to target code
        # The "goto other" is the else branch

        # Only handle if label_refcount for target is exactly 1 (from our goto)
        if label_refcount.get(target, 0) != 1:
            continue

        # Replace with combined condition
        # if (x == A) goto target; if (x != B) goto other;
        # becomes: r0 = 0; if (x == A || x == B) r0 = 1;
        # But that changes semantics... let's be careful.

        # Actually, the common pattern in this codebase is:
        #   if (x == 0x32) goto L; if (x != 0x1e) goto M; L: ; r0 = 1; M: ;
        # This means: if (x == 0x32 || x == 0x1e) { r0 = 1; }

        # For now, just combine the two conditions and the label
        lines[i] = ''  # remove first goto
        # Change second line to combined condition
        # if (x != B) goto other -> if (x != A && x != B) goto other
        lines[next_line] = indent + 'if (' + var1 + ' != ' + val1 + ' && ' + var2 + ' != ' + val2 + ') goto ' + other_label + ';'
        # Remove the target label since it's now unreferenced
        if target_idx < len(lines):
            m_lbl = re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[target_idx])
            if m_lbl:
                lines[target_idx] = ''
        label_refcount[target] = 0
        changes += 1

    return changes

def convert_flag_set_skip(lines, label_refcount, label_pos):
    """Convert pattern:
        var = init_val;
        if (cond) goto L_skip;
        var = other_val;
    L_skip: ;

    Into:
        if (!(cond)) {
            var = other_val;
        } else {
            var = init_val;  (already set above)
        }

    Or more simply, into:
        var = init_val;
        if (!(cond)) {
            var = other_val;
        }
    """
    changes = 0
    for i in range(len(lines)):
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
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
        # Check that between goto and label there are only 1-3 simple assignment lines
        body_lines = []
        skip = False
        for k in range(i+1, label_idx):
            s = lines[k].strip()
            if not s:
                continue
            if re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]):
                skip = True
                break
            if re.search(r'\bgoto\s+', s):
                skip = True
                break
            body_lines.append(k)
        if skip or len(body_lines) == 0 or len(body_lines) > 5:
            continue
        # All body lines should be simple assignments or statements
        inv = invert_cond(cond)
        if inv is None:
            continue
        lines[i] = indent + 'if (' + inv + ') {'
        for k in body_lines:
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label_idx] = indent + '}'
        label_refcount[label] = 0
        changes += 1
    return changes

def convert_multi_ref_forward(lines, label_refcount, label_pos):
    """Convert forward gotos where a label is referenced by exactly 2 adjacent
    conditional gotos, forming an OR condition.

    Pattern:
        if (condA) goto L_target;
        if (condB) goto L_target;
        ...code between...
    L_target:

    Convert to:
        if (!(condA) && !(condB)) {
            ...code between...
        }
    """
    changes = 0
    processed = set()

    for label, pos in label_pos.items():
        if label in processed:
            continue
        if label_refcount.get(label, 0) != 2:
            continue

        # Find both references
        refs = []
        for k in range(len(lines)):
            m = re.match(r'^(\s+)if \((.+)\) goto ' + re.escape(label) + r';$', lines[k])
            if m:
                refs.append((k, m.group(1), m.group(2)))

        if len(refs) != 2:
            continue

        idx1, indent1, cond1 = refs[0]
        idx2, indent2, cond2 = refs[1]

        # Both must be before the label
        if idx1 >= pos or idx2 >= pos:
            continue

        # They must be adjacent (idx2 == idx1 + 1)
        if idx2 != idx1 + 1:
            continue

        # No other labels between second goto and target
        has_other = False
        for k in range(idx2 + 1, pos):
            if re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]):
                has_other = True
                break
        if has_other:
            continue

        # No gotos between second ref and label
        has_bad = False
        for k in range(idx2 + 1, pos):
            if re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k]):
                has_bad = True
                break
        if has_bad:
            continue

        # Invert both conditions
        inv1 = invert_cond(cond1)
        inv2 = invert_cond(cond2)
        if inv1 is None or inv2 is None:
            continue

        # Apply: combine into if (inv1 && inv2) { body }
        lines[idx1] = indent1 + 'if (' + inv1 + ' && ' + inv2 + ') {'
        lines[idx2] = ''
        for k in range(idx2 + 1, pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[pos] = indent1 + '}'
        label_refcount[label] = 0
        processed.add(label)
        changes += 2

    return changes


def convert_multi_condition_or(lines, label_refcount, label_pos):
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


def convert_goto_to_return(lines, label_refcount, label_pos):
    changes = 0
    return_labels = set()
    for lbl, pos in label_pos.items():
        for k in range(pos + 1, min(pos + 3, len(lines))):
            stripped = lines[k].strip()
            if stripped:
                if stripped == 'return;':
                    return_labels.add(lbl)
                break
    if not return_labels:
        return 0
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


def convert_return_value(lines, label_refcount, label_pos):
    changes = 0
    return_var_labels = {}
    for lbl, pos in label_pos.items():
        for k in range(pos + 1, min(pos + 3, len(lines))):
            stripped = lines[k].strip()
            if stripped:
                m = re.match(r'^return\s+(\w+)\s*;$', stripped)
                if m:
                    return_var_labels[lbl] = m.group(1)
                break
    if not return_var_labels:
        return 0
    for i in range(len(lines) - 1, -1, -1):
        m_assign = re.match(r'^(\s+)(\w+)\s*=\s*(.+);\s*$', lines[i])
        if not m_assign:
            continue
        indent, var, value = m_assign.group(1), m_assign.group(2), m_assign.group(3)
        for j in range(i + 1, min(i + 3, len(lines))):
            stripped = lines[j].strip()
            if stripped:
                m_goto = re.match(r'^goto (L_[0-9A-Fa-f]+);$', stripped)
                if m_goto:
                    lbl = m_goto.group(1)
                    if lbl in return_var_labels and return_var_labels[lbl] == var:
                        lines[i] = indent + 'return ' + value + ';'
                        lines[j] = ''
                        label_refcount[lbl] = label_refcount.get(lbl, 0) - 1
                        changes += 1
                break
    return changes


def convert_forward_singles_nested(lines, label_refcount, label_pos, label_sources):
    changes = 0
    for i in range(len(lines) - 1, -1, -1):
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
        body_labels = []
        for k in range(i + 1, label_idx):
            lm = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if lm:
                body_labels.append(lm.group(1))
        if not body_labels:
            continue
        all_internal = True
        for bl in body_labels:
            for src in label_sources.get(bl, []):
                if src < i or src > label_idx:
                    all_internal = False
                    break
            if not all_internal:
                break
        if not all_internal:
            continue
        has_bad = False
        for k in range(i + 1, label_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) in label_pos:
                t = label_pos[gm.group(1)]
                if t < i or t > label_idx:
                    has_bad = True
                    break
        if has_bad:
            continue
        inv = invert_cond(cond)
        if inv is None:
            continue
        lines[i] = indent + 'if (' + inv + ') {'
        for k in range(i + 1, label_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label_idx] = indent + '}'
        changes += 1
        label_refcount[label] = 0
    return changes


def convert_cascaded_if_else(lines, label_refcount, label_pos):
    changes = 0
    processed = set()
    for i in range(len(lines)):
        if i in processed:
            continue
        m1 = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m1:
            continue
        indent, first_cond, first_label = m1.group(1), m1.group(2), m1.group(3)
        if first_label not in label_pos or label_refcount.get(first_label, 0) != 1:
            continue
        first_label_idx = label_pos[first_label]
        if first_label_idx <= i:
            continue
        end_goto_idx = end_label = None
        for k in range(first_label_idx - 1, i, -1):
            s = lines[k].strip()
            if not s or s.startswith('//'):
                continue
            mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', s)
            if mg:
                end_goto_idx = k
                end_label = mg.group(1)
            break
        if end_goto_idx is None or end_label is None or end_label not in label_pos:
            continue
        end_label_idx = label_pos[end_label]
        if end_label_idx <= first_label_idx:
            continue
        if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(i + 1, end_goto_idx)):
            continue
        chain = [{'cond': first_cond, 'body_start': i + 1, 'body_end': end_goto_idx, 'skip_goto_idx': end_goto_idx, 'label': first_label, 'label_idx': first_label_idx}]
        cur_label_idx = first_label_idx
        chain_end_label = end_label
        while True:
            next_if_idx = next_cond_val = next_label_val = None
            for k in range(cur_label_idx + 1, min(cur_label_idx + 3, len(lines))):
                s = lines[k].strip()
                if not s:
                    continue
                mn = re.match(r'^if \((.+)\) goto (L_[0-9A-Fa-f]+);$', s)
                if mn:
                    next_if_idx, next_cond_val, next_label_val = k, mn.group(1), mn.group(2)
                break
            if next_if_idx is None:
                break
            if next_label_val not in label_pos or label_refcount.get(next_label_val, 0) != 1:
                break
            next_label_idx = label_pos[next_label_val]
            if next_label_idx <= next_if_idx:
                break
            next_end_goto_idx = None
            for k in range(next_label_idx - 1, next_if_idx, -1):
                s = lines[k].strip()
                if not s or s.startswith('//'):
                    continue
                mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', s)
                if mg and mg.group(1) == chain_end_label:
                    next_end_goto_idx = k
                break
            if next_end_goto_idx is None:
                break
            if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(next_if_idx + 1, next_end_goto_idx)):
                break
            chain.append({'cond': next_cond_val, 'body_start': next_if_idx + 1, 'body_end': next_end_goto_idx, 'skip_goto_idx': next_end_goto_idx, 'label': next_label_val, 'label_idx': next_label_idx})
            cur_label_idx = next_label_idx
        if len(chain) < 2:
            continue
        if not all(invert_cond(c['cond']) is not None for c in chain):
            continue
        inv = invert_cond(chain[0]['cond'])
        lines[i] = indent + 'if (' + inv + ') {'
        for k in range(chain[0]['body_start'], chain[0]['body_end']):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[chain[0]['skip_goto_idx']] = ''
        label_refcount[chain[0]['label']] = 0
        label_refcount[chain_end_label] = label_refcount.get(chain_end_label, 0) - 1
        processed.add(i)
        for ci in range(1, len(chain)):
            c = chain[ci]
            inv = invert_cond(c['cond'])
            lines[c['label_idx']] = ''
            lines[chain[ci - 1]['label_idx']] = indent + '} else if (' + inv + ') {'
            for k in range(c['body_start'], c['body_end']):
                if lines[k].strip():
                    lines[k] = '    ' + lines[k]
            lines[c['skip_goto_idx']] = ''
            label_refcount[c['label']] = 0
            label_refcount[chain_end_label] = label_refcount.get(chain_end_label, 0) - 1
        last_label_idx = chain[-1]['label_idx']
        else_start, else_end = last_label_idx + 1, end_label_idx
        has_else_body = any(lines[k].strip() for k in range(else_start, else_end))
        has_else_label = any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(else_start, else_end))
        if has_else_body and not has_else_label:
            lines[last_label_idx] = indent + '} else {'
            for k in range(else_start, else_end):
                if lines[k].strip():
                    lines[k] = '    ' + lines[k]
            lines[else_end] = indent + '}'
            label_refcount[chain_end_label] = max(label_refcount.get(chain_end_label, 0) - 1, 0)
        else:
            lines[last_label_idx] = indent + '}'
        changes += len(chain) * 2
    return changes


def convert_break_from_loop(lines, label_refcount, label_pos):
    changes = 0
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
    for i, line in enumerate(lines):
        if re.match(r'^\s*while\s*\(.+\)\s*\{', line):
            depth = 1
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    loop_ranges.append((i, j))
                    break
    if not loop_ranges:
        return 0
    for i in range(len(lines)):
        line = lines[i]
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', line)
        is_conditional = False
        cond = None
        if m:
            indent, lbl = m.group(1), m.group(2)
        else:
            m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
            if m:
                indent, cond, lbl = m.group(1), m.group(2), m.group(3)
                is_conditional = True
            else:
                continue
        if lbl not in label_pos:
            continue
        target = label_pos[lbl]
        best_loop = None
        for (start, end) in loop_ranges:
            if start < i < end:
                if best_loop is None or start > best_loop[0]:
                    best_loop = (start, end)
        if best_loop is None:
            continue
        loop_start, loop_end = best_loop
        valid = (target == loop_end + 1) or (target == loop_end + 2 and not lines[loop_end + 1].strip())
        if valid:
            if is_conditional:
                lines[i] = indent + 'if (' + cond + ') break;'
            else:
                lines[i] = indent + 'break;'
            label_refcount[lbl] = label_refcount.get(lbl, 0) - 1
            changes += 1
    return changes


def convert_continue_in_loop(lines, label_refcount, label_pos):
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
    for i, line in enumerate(lines):
        if re.match(r'^\s*while\s*\(.+\)\s*\{', line):
            depth = 1
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    loop_ranges.append((i, j, 'while'))
                    break
    if not loop_ranges:
        return 0
    for i in range(len(lines)):
        line = lines[i]
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', line)
        is_conditional = False
        cond = None
        if m:
            indent, lbl = m.group(1), m.group(2)
        else:
            m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
            if m:
                indent, cond, lbl = m.group(1), m.group(2), m.group(3)
                is_conditional = True
            else:
                continue
        if lbl not in label_pos:
            continue
        target = label_pos[lbl]
        best_loop = None
        for (start, end, ltype) in loop_ranges:
            if start < i < end:
                if best_loop is None or start > best_loop[0]:
                    best_loop = (start, end, ltype)
        if best_loop is None:
            continue
        loop_start, loop_end, loop_type = best_loop
        is_continue = (target == loop_start) or (loop_type == 'do-while' and target == loop_end)
        if is_continue:
            if is_conditional:
                lines[i] = indent + 'if (' + cond + ') continue;'
            else:
                lines[i] = indent + 'continue;'
            label_refcount[lbl] = label_refcount.get(lbl, 0) - 1
            changes += 1
    return changes


def convert_switch(lines, label_refcount, label_pos):
    changes = 0
    i = 0
    while i < len(lines):
        m = re.match(r'^(\s+)if \((.+?) == (.+?)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            i += 1
            continue
        indent, switch_var = m.group(1), m.group(2)
        first_val, first_label = m.group(3), m.group(4)
        cases = [(first_val, first_label)]
        j = i + 1
        while j < len(lines):
            mj = re.match(r'^\s+if \(' + re.escape(switch_var) + r' == (.+?)\) goto (L_[0-9A-Fa-f]+);$', lines[j])
            if mj:
                cases.append((mj.group(1), mj.group(2)))
                j += 1
            else:
                break
        if len(cases) < 3:
            i += 1
            continue
        default_label = default_goto_idx = None
        if j < len(lines):
            md = re.match(r'^\s+goto (L_[0-9A-Fa-f]+);$', lines[j])
            if md:
                default_label = md.group(1)
                default_goto_idx = j
        all_valid = all(lbl in label_pos and label_pos[lbl] > i and label_refcount.get(lbl, 0) == 1 for _, lbl in cases)
        if not all_valid:
            i += 1
            continue
        case_positions = sorted([(val, lbl, label_pos[lbl]) for val, lbl in cases], key=lambda x: x[2])
        end_boundary = label_pos.get(default_label, case_positions[-1][2]) if default_label else case_positions[-1][2]
        end_label = end_label_idx = None
        search_start = end_boundary + 1
        for k in range(search_start, min(search_start + 100, len(lines))):
            ml = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if ml:
                end_label, end_label_idx = ml.group(1), k
                break
        valid = True
        for ci in range(len(case_positions)):
            _, _, cpos = case_positions[ci]
            nxt = case_positions[ci+1][2] if ci+1 < len(case_positions) else (label_pos.get(default_label, end_label_idx or cpos+20) if default_label else (end_label_idx or cpos+20))
            for k in range(cpos + 1, min(nxt, len(lines))):
                gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
                if gm and gm.group(1) in label_pos and label_pos[gm.group(1)] < i:
                    valid = False
                    break
            if not valid:
                break
        if not valid:
            i += 1
            continue
        lines[i] = indent + 'switch (' + switch_var + ') {'
        for k in range(i + 1, i + len(cases)):
            lines[k] = ''
        if default_goto_idx is not None:
            lines[default_goto_idx] = ''
        for ci in range(len(case_positions)):
            val, lbl, cpos = case_positions[ci]
            nxt = case_positions[ci+1][2] if ci+1 < len(case_positions) else (label_pos.get(default_label, end_label_idx or cpos+1) if default_label else (end_label_idx or cpos+1))
            lines[cpos] = indent + 'case ' + val + ':'
            for k in range(cpos + 1, nxt):
                if lines[k].strip():
                    lines[k] = '    ' + lines[k]
            last_nonempty = None
            for k in range(nxt - 1, cpos, -1):
                if lines[k].strip():
                    last_nonempty = k
                    break
            if last_nonempty is not None:
                s = lines[last_nonempty].strip()
                if not (s.startswith('goto ') or s.startswith('return') or s.startswith('break') or s.startswith('continue')):
                    lines[last_nonempty] += '\n' + indent + '    break;'
            label_refcount[lbl] = 0
        if default_label and default_label in label_pos:
            dpos = label_pos[default_label]
            lines[dpos] = indent + 'default:'
            if end_label_idx and end_label_idx > dpos:
                for k in range(dpos + 1, end_label_idx):
                    if lines[k].strip():
                        lines[k] = '    ' + lines[k]
                lines[end_label_idx] = indent + '}'
            else:
                lines[dpos] += '\n' + indent + '}'
            label_refcount[default_label] = label_refcount.get(default_label, 0) - 1
        elif end_label_idx:
            lines[end_label_idx] = indent + '}'
        else:
            lines[case_positions[-1][2]] += '\n' + indent + '}'
        changes += len(cases) + (1 if default_label else 0)
        i = j + 1 if default_goto_idx is not None else j
    return changes


def main():
    c_files = glob.glob('src/**/*.c', recursive=True)
    total_changes = 0

    verbose = '--verbose' in sys.argv or '-v' in sys.argv

    for iteration in range(25):
        iter_changes = 0
        for filepath in c_files:
            try:
                with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
                    content = f.read()
            except:
                continue
            if 'goto' not in content:
                continue
            lines = content.split('\n')
            changes = 0

            # Pass 0: Multi-condition OR (collapse consecutive gotos to same target)
            label_refcount, label_pos, label_sources = build_indices(lines)
            changes += convert_multi_condition_or(lines, label_refcount, label_pos)

            # Pass 1: Remove dead gotos (goto L; L:)
            label_refcount, label_pos = build_indices_compat(lines)
            changes += remove_dead_gotos(lines, label_refcount, label_pos)

            # Pass 2: Goto-to-return
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_goto_to_return(lines, label_refcount, label_pos)

            # Pass 3: Return-value
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_return_value(lines, label_refcount, label_pos)

            # Pass 4: Forward single-ref gotos -> if blocks
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_forward_singles(lines, label_refcount, label_pos)

            # Pass 5: Forward singles with nested internal labels
            label_refcount, label_pos, label_sources = build_indices(lines)
            changes += convert_forward_singles_nested(lines, label_refcount, label_pos, label_sources)

            # Pass 6: If/else patterns
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_if_else(lines, label_refcount, label_pos)

            # Pass 7: Cascaded if-else-if chains
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_cascaded_if_else(lines, label_refcount, label_pos)

            # Pass 8: Do/while (backward single-ref)
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_do_while(lines, label_refcount, label_pos)

            # Pass 9: While loops
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_while_loop(lines, label_refcount, label_pos)

            # Pass 9b: Simple while loops
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_while_loop_simple(lines, label_refcount, label_pos)

            # Pass 10: Break-from-loop
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_break_from_loop(lines, label_refcount, label_pos)

            # Pass 11: Continue-in-loop
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_continue_in_loop(lines, label_refcount, label_pos)

            # Pass 12: Switch statements
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_switch(lines, label_refcount, label_pos)

            # Pass 13: Multi-target OR conditions
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_multi_target_or(lines, label_refcount, label_pos)

            # Pass 14: Flag-set skip patterns
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_flag_set_skip(lines, label_refcount, label_pos)

            # Pass 15: Multi-ref forward gotos (2 adjacent conditions -> OR)
            label_refcount, label_pos = build_indices_compat(lines)
            changes += convert_multi_ref_forward(lines, label_refcount, label_pos)

            # Pass 16: Remove unreferenced labels
            label_refcount, label_pos = build_indices_compat(lines)
            changes += remove_unreferenced_labels(lines, label_refcount, label_pos)

            if changes > 0:
                with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
                    f.write('\n'.join(lines))
                iter_changes += changes
                total_changes += changes
                if verbose:
                    print(f'  {filepath}: {changes} changes')

        if iter_changes == 0:
            break
        print(f'Iteration {iteration+1}: {iter_changes} gotos/labels converted')

    print(f'Total: {total_changes} gotos/labels converted')

if __name__ == '__main__':
    main()
