#!/usr/bin/env python3
"""
Enhanced goto converter for Pokemon Colosseum decompilation.
Handles multi-ref patterns that convert_gotos.py cannot.
"""
import re
import sys
import os

def invert_cond(cond):
    """Invert a condition expression."""
    cond = cond.strip()

    # Handle compound conditions with && (De Morgan's: !(A && B) = !A || !B)
    if ' && ' in cond:
        parts = cond.split(' && ')
        inverted_parts = []
        for p in parts:
            inv = invert_cond(p.strip())
            if inv is None:
                return None
            inverted_parts.append(inv)
        return ' || '.join(inverted_parts)

    # Handle compound conditions with || (De Morgan's: !(A || B) = !A && !B)
    if ' || ' in cond:
        parts = cond.split(' || ')
        inverted_parts = []
        for p in parts:
            inv = invert_cond(p.strip())
            if inv is None:
                return None
            inverted_parts.append(inv)
        return ' && '.join(inverted_parts)

    replacements = [
        (' == ', ' != '),
        (' != ', ' == '),
        (' < ', ' >= '),
        (' >= ', ' < '),
        (' > ', ' <= '),
        (' <= ', ' > '),
    ]
    for old, new in replacements:
        if old in cond and cond.count(old.strip()) == 1:
            return cond.replace(old, new, 1)
    return None

def build_indices(lines):
    """Build label position map and refcount map."""
    label_pos = {}
    label_refcount = {}
    label_sources = {}

    for i, line in enumerate(lines):
        m = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', line)
        if m:
            label_pos[m.group(1)] = i

    for i, line in enumerate(lines):
        for m in re.finditer(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', line):
            lbl = m.group(1)
            label_refcount[lbl] = label_refcount.get(lbl, 0) + 1
            if lbl not in label_sources:
                label_sources[lbl] = []
            label_sources[lbl].append(i)

    return label_pos, label_refcount, label_sources

def count_gotos(lines):
    return sum(len(re.findall(r'\bgoto\s+L_[0-9A-Fa-f]+\b', line)) for line in lines)

def remove_goto_to_next_label(lines, lp, rc, ls):
    changes = 0
    for i in range(len(lines) - 1):
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        label = m.group(2)
        for k in range(i + 1, min(i + 5, len(lines))):
            if lines[k].strip():
                ml = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
                if ml and ml.group(1) == label:
                    lines[i] = ''
                    rc[label] = rc.get(label, 0) - 1
                    changes += 1
                break
    return changes

def remove_unreferenced_labels(lines, lp, rc, ls):
    changes = 0
    for label, pos in list(lp.items()):
        if rc.get(label, 0) <= 0:
            m = re.match(r'^(\s*)(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[pos])
            if m:
                lines[pos] = ''
                changes += 1
    return changes

def convert_goto_to_return(lines, lp, rc, ls):
    changes = 0
    return_labels = set()
    for lbl, pos in lp.items():
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
            rc[m.group(2)] = rc.get(m.group(2), 0) - 1
            changes += 1
            continue
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', line)
        if m and m.group(3) in return_labels:
            lines[i] = m.group(1) + 'if (' + m.group(2) + ') return;'
            rc[m.group(3)] = rc.get(m.group(3), 0) - 1
            changes += 1
    return changes

def convert_return_value(lines, lp, rc, ls):
    changes = 0
    return_var_labels = {}
    for lbl, pos in lp.items():
        for k in range(pos + 1, min(pos + 3, len(lines))):
            stripped = lines[k].strip()
            if stripped:
                m = re.match(r'^return\s+(\w+)\s*;$', stripped)
                if m:
                    return_var_labels[lbl] = m.group(1)
                break
    for i in range(len(lines) - 1, -1, -1):
        m_assign = re.match(r'^(\s+)(\w+)\s*=\s*(.+?);\s*$', lines[i])
        if not m_assign:
            continue
        indent, var, value = m_assign.group(1), m_assign.group(2), m_assign.group(3)
        for j in range(i + 1, min(i + 3, len(lines))):
            stripped = lines[j].strip()
            if stripped:
                mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', stripped)
                if mg:
                    lbl = mg.group(1)
                    if lbl in return_var_labels and return_var_labels[lbl] == var:
                        lines[i] = indent + 'return ' + value + ';'
                        lines[j] = ''
                        rc[lbl] = rc.get(lbl, 0) - 1
                        changes += 1
                break
    return changes

def convert_multi_condition_or(lines, lp, rc, ls):
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
            mj = re.match(r'^(\s+)if \((.+)\) goto ' + re.escape(label) + r';$', lines[j])
            if mj:
                conditions.append(mj.group(2))
                j += 1
            elif not lines[j].strip():
                j += 1
            else:
                break
        if len(conditions) > 1:
            combined = ' || '.join(conditions)
            lines[i] = indent + 'if (' + combined + ') goto ' + label + ';'
            for k in range(i + 1, i + len(conditions)):
                lines[k] = ''
                rc[label] = rc.get(label, 0) - 1
            changes += len(conditions) - 1
            i = j
        else:
            i += 1
    return changes

def convert_multi_ref_while(lines, lp, rc, ls):
    """While loops with multi-ref body labels."""
    changes = 0
    for i in range(len(lines)):
        m = re.match(r'^(\s+)goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent = m.group(1)
        cond_label = m.group(2)

        if cond_label not in lp:
            continue
        cond_idx = lp[cond_label]
        if cond_idx <= i:
            continue
        if rc.get(cond_label, 0) != 1:
            continue

        # Find body label right after
        body_label = body_idx = None
        for k in range(i + 1, min(i + 3, cond_idx)):
            if lines[k].strip():
                ml = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
                if ml:
                    body_label = ml.group(1)
                    body_idx = k
                break
        if body_label is None:
            continue

        body_rc = rc.get(body_label, 0)
        if body_rc < 1 or body_rc > 2:
            continue

        # Find back-jump
        back_goto_idx = back_goto_cond = None
        for k in range(cond_idx + 1, min(cond_idx + 15, len(lines))):
            stripped = lines[k].strip()
            if not stripped:
                continue
            mb = re.match(r'^if \((.+)\) goto ' + re.escape(body_label) + r';$', stripped)
            if mb:
                back_goto_idx = k
                back_goto_cond = mb.group(1)
                break
            if re.match(r'^L_[0-9A-Fa-f]+\s*:', stripped) or re.match(r'^goto\s+', stripped):
                break
        if back_goto_idx is None:
            continue

        # Check body doesn't escape
        has_bad = False
        for k in range(body_idx + 1, cond_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm:
                target_lbl = gm.group(1)
                if target_lbl in lp:
                    t = lp[target_lbl]
                    if t < body_idx or t > back_goto_idx + 2:
                        has_bad = True
                        break
        if has_bad:
            continue

        # Check cond eval
        for k in range(cond_idx + 1, back_goto_idx):
            if re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k]):
                has_bad = True
                break
        if has_bad:
            continue

        inv_cond = invert_cond(back_goto_cond)
        if inv_cond is None:
            continue

        # Collect cond eval lines
        cond_eval_lines = []
        for k in range(cond_idx + 1, back_goto_idx):
            if lines[k].strip():
                cond_eval_lines.append(k)

        # Transform
        lines[i] = indent + 'while (1) {'
        lines[body_idx] = ''
        lines[cond_idx] = ''

        # Build cond eval + break block
        cond_block = []
        for k in cond_eval_lines:
            cond_block.append(indent + '    ' + lines[k].strip())
            lines[k] = ''
        cond_block.append(indent + '    if (' + inv_cond + ') break;')

        # Insert cond block after the while(1) line
        # Find the first body line
        insert_point = body_idx
        lines[insert_point] = '\n'.join(cond_block)

        # Indent body
        for k in range(body_idx + 1, cond_idx):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]

        lines[back_goto_idx] = indent + '}'

        rc[cond_label] = 0
        rc[body_label] = rc.get(body_label, 0) - 1
        changes += 2

    return changes


def convert_validation_chain(lines, lp, rc, ls):
    """Convert validation chain pattern (very common in PPC decomps):

    if (c1) goto L1; fail_code; goto END; L1: ;
    ...code1...
    if (c2) goto L2; fail_code; goto END; L2: ;
    ...code2...
    if (c3) goto L3; fail_code; goto END; L3: ;
    success_code;
    END: ;

    Into:
    if (c1) {
        ...code1...
        if (c2) {
            ...code2...
            if (c3) {
                success_code;
            } else { fail_code; }
        } else { fail_code; }
    } else { fail_code; }

    Or simpler form when fail_code is just "r0 = 0;":
    r0 = 0;
    if (c1) {
        ...code1...
        if (c2) {
            ...code2...
            if (c3) {
                success_code;
            }
        }
    }
    """
    changes = 0
    processed = set()

    # Find candidate end labels (targets of multiple unconditional gotos)
    for end_label, end_pos in sorted(lp.items(), key=lambda x: x[1]):
        if end_label in processed:
            continue

        sources = ls.get(end_label, [])
        if len(sources) < 2:
            continue

        # Find all unconditional gotos to this label
        uncond_gotos = []
        for src in sources:
            m = re.match(r'^(\s+)goto ' + re.escape(end_label) + r';$', lines[src])
            if m and src < end_pos:
                uncond_gotos.append(src)

        if len(uncond_gotos) < 2:
            continue

        uncond_gotos.sort()

        # For each uncond goto, find the preceding pattern:
        # line X: if (cond) goto L_next;
        # line X+1..Y-1: fail assignments (1-3 lines)
        # line Y: goto END;             <- this is our uncond_goto
        # line Y+1: L_next: ;

        chain = []
        valid = True
        for ug in uncond_gotos:
            # Find fail code between conditional goto and unconditional goto
            # Walk backward from ug to find the if-goto
            cond_line = None
            cond_text = None
            next_label = None
            fail_lines = []

            for k in range(ug - 1, max(ug - 6, -1), -1):
                s = lines[k].strip()
                if not s:
                    continue
                m_if = re.match(r'^if \((.+)\) goto (L_[0-9A-Fa-f]+);$', s)
                if m_if:
                    cond_line = k
                    cond_text = m_if.group(1)
                    next_label = m_if.group(2)
                    break
                else:
                    fail_lines.insert(0, k)

            if cond_line is None or next_label is None:
                valid = False
                break

            # next_label should be right after the uncond goto
            if next_label not in lp:
                valid = False
                break
            next_label_pos = lp[next_label]
            if next_label_pos < ug or next_label_pos > ug + 2:
                valid = False
                break
            if rc.get(next_label, 0) != 1:
                valid = False
                break

            chain.append({
                'cond_line': cond_line,
                'cond': cond_text,
                'fail_lines': fail_lines,
                'uncond_goto': ug,
                'next_label': next_label,
                'next_label_pos': next_label_pos,
            })

        if not valid or len(chain) < 2:
            continue

        chain.sort(key=lambda x: x['cond_line'])

        # Verify chain is contiguous
        is_contiguous = True
        for ci in range(len(chain) - 1):
            if chain[ci]['next_label_pos'] > chain[ci + 1]['cond_line']:
                is_contiguous = False
                break
        if not is_contiguous:
            continue

        # Check all fail paths have the same assignment (typically r0 = 0)
        fail_assignments = set()
        for part in chain:
            for fl in part['fail_lines']:
                fail_assignments.add(lines[fl].strip())

        # Get the indent
        indent = re.match(r'^(\s*)', lines[chain[0]['cond_line']]).group(1)

        # Convert: wrap each validation step in if (cond) { }
        # The fail code before each goto END becomes redundant since we
        # just fall out of all the ifs
        # Put the common fail assignment before the chain

        if len(fail_assignments) == 1:
            common_fail = list(fail_assignments)[0]
            # Put common fail assignment at the beginning, before first cond
            # Then wrap each step in if(cond) { ... }
            for ci, part in enumerate(chain):
                lines[part['cond_line']] = indent + 'if (' + part['cond'] + ') {'
                for fl in part['fail_lines']:
                    lines[fl] = ''  # Remove fail assignment (already set before)
                lines[part['uncond_goto']] = ''  # Remove goto END
                lines[part['next_label_pos']] = ''  # Remove next label
                rc[end_label] = rc.get(end_label, 0) - 1
                rc[part['next_label']] = 0
                changes += 1

            # Now add closing braces at end_pos
            # We need N closing braces for N chain entries
            close_braces = '\n'.join([indent + '}'] * len(chain))
            if rc.get(end_label, 0) <= 0:
                lines[end_pos] = close_braces
            else:
                lines[end_pos] = close_braces + '\n' + lines[end_pos]

            # Indent the body between each chain step
            # Each deeper level gets one more indent
            for ci in range(len(chain)):
                extra_indent = '    ' * (ci + 1)
                start = chain[ci]['next_label_pos'] + 1 if chain[ci]['next_label_pos'] > chain[ci]['cond_line'] else chain[ci]['cond_line'] + 1
                end = chain[ci + 1]['cond_line'] if ci + 1 < len(chain) else end_pos

                for k in range(start, end):
                    if lines[k].strip():
                        lines[k] = extra_indent + lines[k].lstrip()

            processed.add(end_label)

        else:
            # Different fail assignments - skip for now
            continue

    return changes


def convert_multi_ref_forward_skip(lines, lp, rc, ls):
    """2+ adjacent conditional gotos to same label -> combined if."""
    changes = 0
    processed = set()

    for label, pos in sorted(lp.items(), key=lambda x: x[1]):
        if label in processed:
            continue
        ref_count = rc.get(label, 0)
        if ref_count < 2:
            continue

        sources = ls.get(label, [])
        refs = []
        for src in sources:
            m = re.match(r'^(\s+)if \((.+)\) goto ' + re.escape(label) + r';$', lines[src])
            if m:
                refs.append((src, m.group(1), m.group(2)))

        if len(refs) < 2:
            continue

        refs.sort(key=lambda x: x[0])
        consecutive = [refs[0]]
        for ri in range(1, len(refs)):
            if refs[ri][0] == consecutive[-1][0] + 1:
                consecutive.append(refs[ri])
            else:
                break

        if len(consecutive) < 2:
            continue
        if any(r[0] >= pos for r in consecutive):
            continue
        if len(consecutive) != len(sources):
            continue

        first_idx = consecutive[0][0]
        last_idx = consecutive[-1][0]
        indent = consecutive[0][1]

        if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(last_idx + 1, pos)):
            continue

        has_bad = False
        for k in range(last_idx + 1, pos):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm and gm.group(1) in lp:
                t = lp[gm.group(1)]
                if t < first_idx or t > pos:
                    has_bad = True
                    break
        if has_bad:
            continue

        inv_conds = []
        valid = True
        for _, _, cond in consecutive:
            inv = invert_cond(cond)
            if inv is None:
                valid = False
                break
            inv_conds.append(inv)
        if not valid:
            continue

        combined = ' && '.join(inv_conds)
        lines[first_idx] = indent + 'if (' + combined + ') {'
        for k in range(first_idx + 1, last_idx + 1):
            lines[k] = ''
        for k in range(last_idx + 1, pos):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[pos] = indent + '}'
        rc[label] = 0
        processed.add(label)
        changes += len(consecutive)

    return changes


def convert_forward_singles_relaxed(lines, lp, rc, ls):
    """Forward single-ref with relaxed constraints."""
    changes = 0
    for i in range(len(lines) - 1, -1, -1):
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent, cond, label = m.group(1), m.group(2), m.group(3)

        if rc.get(label, 0) != 1:
            continue
        if label not in lp:
            continue
        label_idx = lp[label]
        if label_idx <= i:
            continue
        if label_idx - i > 2000:
            continue

        # Allow internal labels if all their sources are inside
        body_labels = set()
        for k in range(i + 1, label_idx):
            ml = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if ml:
                body_labels.add(ml.group(1))

        all_internal = True
        for bl in body_labels:
            for src in ls.get(bl, []):
                if src < i or src >= label_idx:
                    all_internal = False
                    break
            if not all_internal:
                break
        if not all_internal:
            continue

        has_bad = False
        for k in range(i + 1, label_idx):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm:
                target_lbl = gm.group(1)
                if target_lbl in body_labels or target_lbl == label:
                    continue
                if target_lbl in lp:
                    t = lp[target_lbl]
                    if t >= label_idx:
                        continue
                    if t < i:
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
        rc[label] = 0
        changes += 1

    return changes


def convert_do_while_relaxed(lines, lp, rc, ls):
    """Do-while with internal labels allowed."""
    changes = 0
    for i in range(len(lines) - 1, -1, -1):
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent, cond, label = m.group(1), m.group(2), m.group(3)

        if label not in lp:
            continue
        label_idx = lp[label]
        if label_idx >= i:
            continue
        if rc.get(label, 0) != 1:
            continue

        body_labels = set()
        for k in range(label_idx + 1, i):
            ml = re.match(r'^\s*(L_[0-9A-Fa-f]+)\s*:\s*;?\s*$', lines[k])
            if ml:
                body_labels.add(ml.group(1))

        all_internal = True
        for bl in body_labels:
            for src in ls.get(bl, []):
                if src <= label_idx or src >= i:
                    all_internal = False
                    break
            if not all_internal:
                break
        if not all_internal:
            continue

        has_bad = False
        for k in range(label_idx + 1, i):
            gm = re.search(r'\bgoto\s+(L_[0-9A-Fa-f]+)\s*;', lines[k])
            if gm:
                target = gm.group(1)
                if target in body_labels or target == label:
                    continue
                if target in lp:
                    t = lp[target]
                    if t < label_idx or t > i:
                        has_bad = True
                        break
        if has_bad:
            continue

        lines[label_idx] = indent + 'do {'
        for k in range(label_idx + 1, i):
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[i] = indent + '} while (' + cond + ');'
        rc[label] = 0
        changes += 1

    return changes


def convert_break_from_loop(lines, lp, rc, ls):
    changes = 0
    loop_ranges = []
    for i, line in enumerate(lines):
        if re.match(r'^\s*(do\s*\{|while\s*\(.+\)\s*\{)', line):
            depth = line.count('{') - line.count('}')
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    loop_ranges.append((i, j))
                    break

    for i in range(len(lines)):
        m = re.match(r'^(\s+)(?:if \((.+)\) )?goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent = m.group(1)
        cond = m.group(2)
        lbl = m.group(3)
        if lbl not in lp:
            continue
        target = lp[lbl]

        for (start, end) in loop_ranges:
            if start < i < end:
                if target == end + 1 or (target == end + 2 and not lines[end + 1].strip()):
                    if cond:
                        lines[i] = indent + 'if (' + cond + ') break;'
                    else:
                        lines[i] = indent + 'break;'
                    rc[lbl] = rc.get(lbl, 0) - 1
                    changes += 1
                    break
    return changes


def convert_continue_in_loop(lines, lp, rc, ls):
    changes = 0
    loop_ranges = []
    for i, line in enumerate(lines):
        m_do = re.match(r'^\s*do\s*\{', line)
        m_while = re.match(r'^\s*while\s*\(.+\)\s*\{', line)
        if m_do or m_while:
            depth = line.count('{') - line.count('}')
            for j in range(i + 1, len(lines)):
                depth += lines[j].count('{') - lines[j].count('}')
                if depth <= 0:
                    ltype = 'do-while' if m_do else 'while'
                    loop_ranges.append((i, j, ltype))
                    break

    for i in range(len(lines)):
        m = re.match(r'^(\s+)(?:if \((.+)\) )?goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent, cond, lbl = m.group(1), m.group(2), m.group(3)
        if lbl not in lp:
            continue
        target = lp[lbl]

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
            if cond:
                lines[i] = indent + 'if (' + cond + ') continue;'
            else:
                lines[i] = indent + 'continue;'
            rc[lbl] = rc.get(lbl, 0) - 1
            changes += 1

    return changes


def convert_cascaded_if_else(lines, lp, rc, ls):
    """Convert cascaded if-else-if chains with shared end label."""
    changes = 0
    processed = set()

    for i in range(len(lines)):
        if i in processed:
            continue
        m1 = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m1:
            continue
        indent, first_cond, first_label = m1.group(1), m1.group(2), m1.group(3)
        if first_label not in lp or rc.get(first_label, 0) != 1:
            continue
        first_label_idx = lp[first_label]
        if first_label_idx <= i:
            continue

        # Find unconditional goto before first_label
        end_goto_idx = end_label = None
        for k in range(first_label_idx - 1, i, -1):
            s = lines[k].strip()
            if not s:
                continue
            mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', s)
            if mg:
                end_goto_idx = k
                end_label = mg.group(1)
            break

        if end_goto_idx is None or end_label is None or end_label not in lp:
            continue
        end_label_idx = lp[end_label]
        if end_label_idx <= first_label_idx:
            continue

        # No labels in then body
        if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(i + 1, end_goto_idx)):
            continue

        chain = [{'cond': first_cond, 'body_start': i + 1, 'body_end': end_goto_idx,
                  'skip_goto_idx': end_goto_idx, 'label': first_label, 'label_idx': first_label_idx}]

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
            if next_label_val not in lp or rc.get(next_label_val, 0) != 1:
                break
            next_label_idx = lp[next_label_val]
            if next_label_idx <= next_if_idx:
                break
            next_end_goto_idx = None
            for k in range(next_label_idx - 1, next_if_idx, -1):
                s = lines[k].strip()
                if not s:
                    continue
                mg = re.match(r'^goto (L_[0-9A-Fa-f]+);$', s)
                if mg and mg.group(1) == chain_end_label:
                    next_end_goto_idx = k
                break
            if next_end_goto_idx is None:
                break
            if any(re.match(r'^\s*L_[0-9A-Fa-f]+\s*:\s*;?\s*$', lines[k]) for k in range(next_if_idx + 1, next_end_goto_idx)):
                break
            chain.append({'cond': next_cond_val, 'body_start': next_if_idx + 1, 'body_end': next_end_goto_idx,
                         'skip_goto_idx': next_end_goto_idx, 'label': next_label_val, 'label_idx': next_label_idx})
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
        rc[chain[0]['label']] = 0
        rc[chain_end_label] = rc.get(chain_end_label, 0) - 1
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
            rc[c['label']] = 0
            rc[chain_end_label] = rc.get(chain_end_label, 0) - 1

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
        else:
            lines[last_label_idx] = indent + '}'

        changes += len(chain) * 2

    return changes


def convert_flag_set_skip(lines, lp, rc, ls):
    """if (cond) goto L; code; L: -> if (!cond) { code; }  (for small bodies)"""
    changes = 0
    for i in range(len(lines)):
        m = re.match(r'^(\s+)if \((.+)\) goto (L_[0-9A-Fa-f]+);$', lines[i])
        if not m:
            continue
        indent, cond, label = m.group(1), m.group(2), m.group(3)
        if rc.get(label, 0) != 1:
            continue
        if label not in lp:
            continue
        label_idx = lp[label]
        if label_idx <= i:
            continue

        body_lines = []
        skip = False
        for k in range(i + 1, label_idx):
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

        inv = invert_cond(cond)
        if inv is None:
            continue

        lines[i] = indent + 'if (' + inv + ') {'
        for k in body_lines:
            if lines[k].strip():
                lines[k] = '    ' + lines[k]
        lines[label_idx] = indent + '}'
        rc[label] = 0
        changes += 1

    return changes


def process_file(filepath):
    """Process a file through all conversion passes."""
    with open(filepath, 'r', encoding='utf-8', errors='replace') as f:
        content = f.read()

    initial_gotos = len(re.findall(r'\bgoto\s+L_[0-9A-Fa-f]+\b', content))
    if initial_gotos == 0:
        return 0

    print(f'Processing {filepath}: {initial_gotos} gotos')

    lines = content.split('\n')
    total_changes = 0
    prev_goto_count = initial_gotos

    for iteration in range(40):
        lp, rc, ls = build_indices(lines)
        iter_changes = 0

        iter_changes += convert_multi_condition_or(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += remove_goto_to_next_label(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_goto_to_return(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_return_value(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_validation_chain(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_cascaded_if_else(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_forward_singles_relaxed(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_flag_set_skip(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_multi_ref_forward_skip(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_multi_ref_while(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_do_while_relaxed(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_break_from_loop(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += convert_continue_in_loop(lines, lp, rc, ls)
        lp, rc, ls = build_indices(lines)
        iter_changes += remove_unreferenced_labels(lines, lp, rc, ls)

        # Check actual goto count to detect real progress vs label-only changes
        current_gotos = count_gotos(lines)
        real_goto_reduction = prev_goto_count - current_gotos

        if real_goto_reduction <= 0:
            break

        prev_goto_count = current_gotos
        total_changes += iter_changes
        print(f'  Iteration {iteration + 1}: {real_goto_reduction} gotos removed ({current_gotos} remaining)')

    content = '\n'.join(lines)
    content = re.sub(r'\n{4,}', '\n\n\n', content)

    final_gotos = len(re.findall(r'\bgoto\s+L_[0-9A-Fa-f]+\b', content))
    print(f'  Result: {initial_gotos} -> {final_gotos} gotos ({initial_gotos - final_gotos} removed)')

    with open(filepath, 'w', encoding='utf-8', errors='replace') as f:
        f.write(content)

    return initial_gotos - final_gotos


def main():
    files = sys.argv[1:]
    if not files:
        print("Usage: convert_gotos_v2.py <file.c> [file2.c ...]")
        sys.exit(1)

    total = 0
    for f in files:
        if os.path.exists(f):
            total += process_file(f)
        else:
            print(f'File not found: {f}')

    print(f'\nTotal gotos removed: {total}')


if __name__ == '__main__':
    main()
