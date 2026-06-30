# Overnight Session Handoff — for future Claude

## What is running
- GLM-5.1 on tmux pane `%7` — PLANNER, editing queue brief "GLM Plan" sections, committing to main repo
- Codex GPT-5.4 on tmux pane `%8` — IMPLEMENTER, writing C in `wt-npc-event` worktree, editing "Codex Input" section of each brief
- 65 function briefs in `tools/decomp_work/queue/` ordered by asm size (smallest first)
- Worktrees:
  - `wt-npc-event` — Codex's branch, has its own tools/ mwcc + queue copy
  - `wt-npc-interact` — GLM's branch (mostly idle; GLM edits main repo queue directly)
- Stop hook: two beeps on response end (see `.claude/settings.local.json`)

## Your job in each wake-up cycle
1. **Check tmux panes**: `tmux capture-pane -p -t %7 | tail -8` (GLM) and `tmux capture-pane -p -t %8 | tail -8` (Codex).
2. **Check for new commits**:
   - `git log --oneline wt-npc-event ^master` (Codex's branch)
   - `git log --oneline origin/master ^master` (main repo from commits)
3. **Merge Codex branch into master** when it has new commits:
   `git merge --no-ff wt-npc-event -m "gs_npc_event: merge <summary>"`
4. **Regression check** before each merge — run match_scan_file on 3-5 previously-matched functions from same file. If any drop, back out.
5. **Sync queue briefs back to worktrees** after GLM updates any brief:
   `cp -r tools/decomp_work/queue/* /c/Users/douglaswhittingham/pkmn-colosseum-wt/npc-event/tools/decomp_work/queue/`
6. **Keep agents fed**: if either is idle, send them another nudge prompt telling them to pick up the next brief.
7. **Update `QUEUE.md` status** — mark `[X]` on claude-merged functions, keep visible.
8. **Periodically schedule a wake-up** to check again in 15-20 minutes.

## Helpful one-liners

### Count total asm-active remaining
```bash
python3 -c "import os,re;t=0
for f in sorted(os.listdir('src/game')):
    if not f.endswith('.c'): continue
    c=open(f'src/game/{f}','rb').read().decode('latin-1')
    t+=len(re.findall(r'#if 1\s*\nasm void fn_',c))
print(t)"
```

### Run batch-flip across all game files (nightly maintenance)
```bash
for f in gs_thread gs_field_world gs_material gs_title; do
  python3 tools/batch_flip_stubs.py $f 2>&1 | tail -5
done
```

### Status dashboard
```bash
git log --oneline -1 --format="master: %s"
tmux capture-pane -p -t %7 | tail -3
tmux capture-pane -p -t %8 | tail -3
```

## Rules (CRITICAL)
- **NEVER regress a matching function.** Always regression-check before commit.
- **NEVER commit uncompiled code.** Always run `compile_check.py` first.
- **NEVER mass-delete lines.** Preserve existing working code; only change what's needed.
- If a stub decomp produces 0% or fails to compile, REVERT (don't keep broken code).
- Prefer 60-90% matches over 0% asm wrappers — progress is progress.
- If Codex/GLM are idle for 20+ min, send them a nudge with the next brief number.

## When the user wakes up
Report: (a) N functions merged this session, (b) master asm-active count before/after, (c) any compile errors to fix, (d) any hard-stuck functions needing user input.
