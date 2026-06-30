#!/usr/bin/env python3
"""Generate per-function brief files for the queue.

Picks up to N functions from specified files (smallest first)
and creates tools/decomp_work/queue/fn_XXXXXXXX.md for each.
"""
import os
import re
import sys
from pathlib import Path

os.chdir(Path(__file__).resolve().parent.parent.parent.parent)

# (stem, max_count_from_file)
TARGETS = [
    ('gs_npc_event', 15),
    ('gs_npc_interact', 25),
    ('gs_material', 9),
    ('gs_event_exec', 4),
    ('gs_title', 10),
    ('gs_render', 10),
]

queue_dir = Path('tools/decomp_work/queue')

def get_inc_size(stem, fn):
    p = Path(f'src/game/{stem}_{fn}.inc')
    if not p.exists():
        return 0
    return len(p.read_text(encoding='latin-1').splitlines())

def asm_active_fns(stem):
    p = Path(f'src/game/{stem}.c')
    c = p.read_bytes().decode('latin-1')
    # Only #if 1 (active) blocks
    out = []
    for m in re.finditer(r'#if 1\s*\nasm void (fn_\w+)\(void\)', c):
        fn = m.group(1)
        out.append((get_inc_size(stem, fn), fn))
    out.sort()  # smallest first
    return out

def make_brief(stem, fn, inc_size):
    inc_path = f'src/game/{stem}_{fn}.inc'
    return f"""# {fn} ({stem}.c, {inc_size} asm lines)

## Status
- [ ] GLM plan
- [ ] Codex implementation
- [ ] Claude review+merge

## Target asm
See `{inc_path}`

## File target
`src/game/{stem}.c`

## Baseline
- Current: asm wrapper at 100% (target identical bytes, but wrapper, not real C)
- Target: ≥60% real C match

## GLM Plan (planner)
_GLM-5.1: read the .inc, identify args/return types, describe pseudocode, list externs needed. Note any quirks (switch, table-search, bit-ops, FP math, list walks, etc.)_

(fill in)

## Codex Input (implementer, iteration 1)
_Codex: implement the C based on GLM's plan. Wrap with `#pragma push / peephole off / pragma pop`. Run `python3 tools/compile_check.py src/game/{stem}.c` then `python3 tools/match_scan_file.py {stem} {fn}`. If <60%, try the pragma variants (scheduling off), reorder locals, or use pointer-walk patterns. Commit when ≥60% OR after 5 attempts. Regression-check 3 already-matched functions in {stem}.c._

(fill in)

## Claude Review (merger)
_Claude: verify compile + match, check no regression, commit to master with message `{stem}: {fn} 0%->X.X% via <approach>`, move this file to completed/._

(fill in)
"""

def main():
    queue_dir.mkdir(parents=True, exist_ok=True)
    manifest_lines = [
        '# Overnight Queue Manifest',
        '',
        'Functions are processed in order. See WORKFLOW.md for the pipeline.',
        '',
        'Legend: `[ ]`=pending `[G]`=GLM-planned `[C]`=Codex-implemented `[X]`=Claude-merged',
        '',
        '| # | Function | File | Size | Status |',
        '|---|---|---|---|---|',
    ]
    count = 0
    for stem, max_n in TARGETS:
        for inc_size, fn in asm_active_fns(stem)[:max_n]:
            count += 1
            brief = queue_dir / f'{fn}.md'
            if not brief.exists():
                brief.write_text(make_brief(stem, fn, inc_size), encoding='utf-8')
            manifest_lines.append(f'| {count} | {fn} | {stem}.c | {inc_size} lines | `[ ]` |')

    manifest_path = queue_dir / 'QUEUE.md'
    manifest_path.write_text('\n'.join(manifest_lines) + '\n', encoding='utf-8')
    print(f'Generated {count} briefs in {queue_dir}')
    print(f'Manifest: {manifest_path}')

if __name__ == '__main__':
    main()
