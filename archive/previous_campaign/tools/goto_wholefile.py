#!/usr/bin/env python3
"""
Whole-file goto eliminator. Processes the entire file as one unit
instead of per-function, to handle files with brace mismatches.

Only applies safe transformations:
- Unconditional goto L; followed by } ... L: where between is only structural
- goto L at end of if-block where L follows the enclosing block
- if (cond) goto L; <code> L: where code has no live labels and terminates
- Backward goto to single-use label -> do-while
"""
import re
import sys


def count_gotos(lines):
    return sum(len(re.findall(r'\bgoto\s+\w+\s*;', l)) for l in lines)


def is_label(line):
    s = line.strip()
    m = re.match(r'^(\w+)\s*:\s*;?\s*$', s)
    if m and not s.startswith('default') and not s.startswith('case'):
        return m.group(1)
    return None


def label_uses(lines, label):
    pat = 'goto ' + label
    return sum(1 for l in lines if pat in l)


def find_label(lines, label, start=0, end=None):
    if end is None:
        end = len(lines)
    t = label + ':'
    for i in range(start, end):
        s = lines[i].strip()
        if s == t or s == t + ' ;' or s.startswith(t + ' '):
            return i
    return -1


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
    ops = {'!=': '==', '==': '!=', '>=': '<', '<=': '>', '>': '<=', '<': '>='}
    m = re.search(r'(.*?)\s*(!=|==|>=|<=|>(?!=)|<(?!=))\s*(.*)', cond)
    if m:
        lhs, op, rhs = m.groups()
        if op in ops:
            return f'{lhs} {ops[op]} {rhs}'
    return f'!({cond})'


def get_indent(line):
    return line[:len(line) - len(line.lstrip())] if line.strip() else ''


def indent_block(block, extra='    '):
    return [extra + l if l.strip() else l for l in block]


def is_structural(s):
    if not s:
        return True
    if s == '}':
        return True
    if is_label('  ' + s):
        return True
    if re.match(r'^\}\s*(else\s*(if\s*\(.+?\))?\s*\{)?\s*$', s):
        return True
    return False


def has_live_label(lines, start, end):
    for i in range(start, end):
        lbl = is_label(lines[i])
        if lbl:
            for j, l in enumerate(lines):
                if (j < start or j >= end) and f'goto {lbl}' in l:
                    return True
    return False


def block_terminates(lines_block):
    ne = [l.strip() for l in lines_block if l.strip()]
    if not ne:
        return False
    last = ne[-1]
    return (last.startswith('return') or last == 'break;' or last == 'continue;'
            or re.match(r'^goto\s+\w+\s*;$', last) is not None)


def process_file(filename):
    with open(filename) as f:
        lines = f.read().split('\n')

    orig_count = count_gotos(lines)
    total_removed = 0
    changed = True
    passes = 0

    while changed and passes < 5000:
        changed = False
        passes += 1

        # Pass A: uncond goto where between is structural
        for i in range(len(lines)):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                li = find_label(lines, lbl, i + 1)
                if li > i:
                    between = lines[i + 1:li]
                    if all(is_structural(b.strip()) for b in between):
                        lines[i] = ''
                        total_removed += 1
                        if label_uses(lines, lbl) == 0:
                            lines[li] = ''
                        changed = True
                        break

        if changed:
            continue

        # Pass B: uncond goto + } -> else wrapping
        for i in range(len(lines)):
            g = extract_goto(lines[i])
            if g and g[0] is None:
                lbl = g[1]
                j = i + 1
                while j < len(lines) and not lines[j].strip():
                    j += 1
                if j < len(lines) and lines[j].strip() == '}':
                    li = find_label(lines, lbl, j + 1)
                    if li > j and not has_live_label(lines, j + 1, li):
                        ind = get_indent(lines[j])
                        lines[i] = ''
                        total_removed += 1
                        lines[j] = ind + '} else {'
                        lines[li:li] = [ind + '}']
                        for ci in range(j + 1, li):
                            if lines[ci].strip():
                                lines[ci] = '    ' + lines[ci]
                        if label_uses(lines, lbl) == 0:
                            new_li = find_label(lines, lbl)
                            if new_li >= 0:
                                lines[new_li] = ''
                        changed = True
                        break

        if changed:
            continue

        # Pass C: if (cond) goto L; <code> L: where code has no live labels
        for i in range(len(lines)):
            g = extract_goto(lines[i])
            if g and g[0] is not None:
                cond, lbl = g
                li = find_label(lines, lbl, i + 1)
                if li > i and not has_live_label(lines, i + 1, li):
                    code = lines[i + 1:li]
                    safe = True
                    for cl in code:
                        cg = extract_goto(cl)
                        if cg:
                            _, clbl = cg
                            cli = find_label(lines, clbl)
                            if cli >= 0 and cli < i:
                                safe = False
                                break
                    if safe:
                        ind = get_indent(lines[i])
                        neg = negate(cond)
                        new = [f'{ind}if ({neg}) {{']
                        new += indent_block(code)
                        new += [f'{ind}}}']
                        total_removed += 1
                        uses = label_uses(lines, lbl)
                        if uses <= 1:
                            lines[i:li + 1] = new
                        else:
                            lines[i:li] = new
                        changed = True
                        break

        if changed:
            continue

        # Pass D: backward goto single-use -> do-while
        for i in range(len(lines)):
            g = extract_goto(lines[i])
            if g:
                cond, lbl = g
                li = find_label(lines, lbl, 0, i)
                if li >= 0 and label_uses(lines, lbl) == 1:
                    ind = get_indent(lines[li])
                    body = lines[li + 1:i]
                    if cond:
                        new = [f'{ind}do {{'] + indent_block(body) + [f'{ind}}} while ({cond});']
                    else:
                        new = [f'{ind}while (1) {{'] + indent_block(body) + [f'{ind}}}']
                    lines[li:i + 1] = new
                    total_removed += 1
                    changed = True
                    break

        if changed:
            continue

        # Pass E: OR chains
        for i in range(len(lines)):
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
                    removed_count = len(conds) - 1
                    lines[i:j] = [f'{ind}if ({merged}) goto {lbl};']
                    total_removed += removed_count
                    changed = True
                    break

    # Cleanup unused labels
    changed2 = True
    while changed2:
        changed2 = False
        new = []
        for line in lines:
            lbl = is_label(line)
            if lbl and label_uses(lines, lbl) == 0:
                changed2 = True
                continue
            new.append(line)
        lines = new

    # Cleanup blank lines
    final = []
    for line in lines:
        if not line.strip() and final and not final[-1].strip():
            continue
        final.append(line)

    with open(filename, 'w') as f:
        f.write('\n'.join(final))

    final_count = count_gotos(final)
    print(f'  {filename}: {orig_count} -> {final_count} ({orig_count - final_count} removed)')
    return orig_count - final_count


if __name__ == '__main__':
    total = 0
    for fn in sys.argv[1:]:
        total += process_file(fn)
    print(f'Total: {total} gotos removed')
