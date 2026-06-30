# New Session — Continue Decompilation Project

You are picking up a Pokémon Colosseum (GPXE01) asm→C byte-match decompilation project. The previous session landed a lot of progress; this handoff has everything you need to resume cleanly.

## Current state (2026-04-18)

- **Master HEAD** is clean and green. Run `git log --oneline -5` to see the last commits.
- **`asm-active` functions across `src/game/*.c`**: **373**
- Progress over the last few sessions: **~75 functions converted** from asm wrappers to matching C (from 448 → 373).
- Top-matching files are in great shape (gs_title, gs_scene complete; gs_event_exec 21/25 converted). The remaining bulk is in:
  - `gs_field_world.c` (88 asm-active) — biggest file, lots of hard stubs
  - `scene_init.c` (56)
  - `gs_worldmap.c` (46)
  - `gs_render.c` (43)
  - `gs_thread.c` (34)
  - `gs_npc_interact.c` (22)
  - `gs_pokemon_summary.c` (23 — all TODO stubs, needs fresh C)
  - `gs_title.c` (19)
  - `gs_npc_event.c` (9)

## What's set up

### Queue system (`tools/decomp_work/queue/`)
- **`QUEUE.md`** — manifest of 65 briefs ordered by asm size
- **`WORKFLOW.md`** — the pipeline spec (GLM plans → Codex implements → Claude merges)
- **`CLAUDE_OVERNIGHT.md`** — cycle instructions for future Claude sessions
- **`fn_XXXXXXXX.md`** — per-function brief files; each has GLM Plan / Codex Input / Claude Review sections
- **`generate_queue.py`** — regenerates briefs; rerun after completing a batch

### Batch-flip tool (`tools/batch_flip_stubs.py`)
- Automatically flips `#if 1 / asm / #else / <real C stub> / #endif` blocks to `#if 0`
- Auto-removes conflicting `extern void fn_X(void);` declarations
- Tries pragma variants (`peephole off`, `scheduling off`, combo) if initial flip <40%
- Usage: `python3 tools/batch_flip_stubs.py <file_stem>`
- Already exhausted on: gs_thread (18+18=36 wins), gs_render (5), gs_material (2), gs_title (3), gs_field_world (6), gs_scene (5). Most files now show "Found 0 candidate stubs".

### Other tools
- `python3 tools/compile_check.py src/game/<file>.c` — compile one file
- `python3 tools/match_scan_file.py <stem> fn_XXX [...]` — per-function match%
- `./tools/objdiff-cli.exe diff -1 build/GC6E01/obj/auto_01_800055E0_text.o -2 build/GC6E01/base/game/<file>.o -o - --format json -c ppc.calculatePoolRelocations=false fn_XXX` — full diff

### Stop-hook beep
`.claude/settings.local.json` has a Stop hook that plays two quick beeps (`[console]::beep`) when you finish responding. Keep it.

### Worktrees
Currently there are 2 worktrees left for external agents:
- `C:/Users/douglaswhittingham/pkmn-colosseum-wt/npc-event` (branch `wt-npc-event`, Codex used it)
- `C:/Users/douglaswhittingham/pkmn-colosseum-wt/npc-interact` (branch `wt-npc-interact`, GLM used it)

Both worktrees have their own queue copy and compiled tools. Either:
1. Keep them and re-dispatch Codex/GLM to continue, OR
2. Remove them if you plan to work solo: `git worktree remove <path> --force && git branch -D wt-<name>`

## Session playbook (all learnings)

### Core patterns that WORK
1. **`#pragma push / #pragma peephole off / #pragma pop`** around each converted function. Defeats CW 1.3's branchless `subfic/cntlzw/srwi/addi` lowering of adjacent-value ternaries and `extsb.`/`cmpwi` collapse.
2. **Switch cases in NUMERIC ORDER** in source (case 0 first, then case 3, etc.) — CW emits the switch dispatch as a comparison tree but case BODIES follow source order.
3. **Initialize variables BEFORE the dispatch**, not inside case 0 (matches prologue order).
4. **Single-return style** preferred over multi-early-return with blr.
5. **Type externs properly**: `extern s32 fn_X(s32)` not `extern void fn_X(void)`. GREP first; don't duplicate. If a `void fn_X(void)` decl already exists in the file, update it to match your new usage.
6. **Bitfield structs** (`u8 bit:1` at byte offset) match CW's `extlwi/rlwimi` bit-toggle idiom better than `^= MASK`. See `gs_party_access.c::fn_8000CAA4` for the pattern.
7. **For-loop pointer-walk**: `for (idx=0; idx<N; idx++) { if (*p == key) break; p += 0xC; }` forces CW to emit `addi p, p, 0xC; lwz r0, 0(p)` instead of folded indexed loads. Key for Quirk 7 table searches. See `gs_event_exec.c::fn_80014234`.
8. **Comma-operator `!=` chain** for Quirk 7 when pointer-walk isn't ideal:
   `if ((((key != tbl[0].k) && (idx=1, key != tbl[1].k)) && (idx=2, key != tbl[2].k))) idx = 3;`
9. **`(s32)(s8)ctx[N]`** for signed byte read (emits `lbz; extsb`).
10. **`(volatile u16*)` cast** to force CW to reload a global between two bit extractions (vs CSE'ing them).
11. **`#pragma optimization_level 4`** (not `0`) for existing stubs in `gs_scene.c` / `gs_render.c` — changes reg allocation to match target.

### Common failure modes — AVOID
- Duplicate extern decls → "identifier redeclared" compile error. Always grep first.
- Destructive edits that remove working code. Only change what's needed.
- Pseudo-code scraps (`u8 sp[0x20]; u32 r3 = 0; ctr_fn = ...`) — these never compile. Either write real C or leave the asm wrapper.
- Regressing already-matched functions. ALWAYS regression-check 3-5 matched functions in the same file before committing a change.

### Regression-safe merge protocol
Before every commit / merge:
1. `rm -f build/GC6E01/base/game/<stem>.o` (force fresh compile)
2. `python3 tools/compile_check.py src/game/<stem>.c` — must pass
3. `python3 tools/match_scan_file.py <stem> fn_A fn_B fn_C` — all previously-matching peers must stay ≥ their prior match%
4. If any drop, revert the offending change.

## Work remaining (ordered by ROI)

### Low-hanging fruit
- **`gs_pokemon_summary.c` — 23 asm-active with TODO stubs**. These need fresh C written. Any simple ones (byte accessors etc) could land quickly.
- **`gs_field_world.c` reverted candidates** — some stubs hit 30-45% match. Manual tuning (pragma combos, pointer-walks) could push a few over 60%.
- **gs_title.c remaining 19** — jump-table-heavy; harder.

### Medium effort
- `gs_render.c` (43 remaining), `scene_init.c` (56), `gs_worldmap.c` (46) — lots of small-to-medium asm functions. Some may have existing stubs my batch tool missed due to regex filters.

### Requires new C writing (no stubs exist)
- `gs_npc_event.c` (9 remaining biggest functions)
- `gs_npc_interact.c` (22 remaining)
- `gs_material.c` (9 remaining)

### Hard / skip for now
- `fn_80014574` (gs_event_exec, 0x4D4) — huge cutscene function
- `fn_80013A18` (gs_event_exec, 0x3E4) — huge camera function
- `fn_8001329C` (gs_event_exec, 0x3CC) — has jump-table switch, very hard
- `fn_80012FB0` (gs_event_exec, 0x2EC) — 64-bit saturating math
- Anything using jump tables (lwzx + bctr pattern)

## Typical session flow

```bash
# 1. Status
python3 -c "import os,re;t=0
for f in sorted(os.listdir('src/game')):
    if not f.endswith('.c'): continue
    c=open(f'src/game/{f}','rb').read().decode('latin-1')
    t+=len(re.findall(r'#if 1\s*\nasm void fn_',c))
print(f'asm-active: {t}')"

git log --oneline -5

# 2. Pick a small function from a target file
grep -B1 "^asm void fn_" src/game/<stem>.c | grep "#if 1" -A1

# 3. Read its .inc, write matching C, wrap with pragma push/peephole off/pop
# 4. Compile + measure + regression-check + commit
```

## Things I learned you may need

- Reported "regressions" often mean the `.o` file is stale. Always `rm -f build/GC6E01/base/game/<stem>.o` before re-matching.
- The pokedex dashboard (`tools/decomp_work/pokedex_dashboard.sh`) reads these stale caches too — if it shows a regression, try rebuilding the file first.
- `gs_texture.c` 7 asm-active functions are LEGITIMATE assembly primitives (GC thread context switch, full 32-GPR+32-FPR save/restore). They CANNOT be expressed in C — leave them as asm.
- `colosseum_event.c` has 1 remaining function (fn_802050F4 at 74.7%). Good enough.
- `pokemon.c` has 1 remaining function (fn_801F54A4 at 871 lines — skip, too big).

## Model routing advice

- **Claude Opus 4.7** (me) — complex function analysis, merging, regression-checking, architecture. Best for the medium-hard functions where pattern-matching from asm matters.
- **Codex GPT-5.4** (tmux pane %8) — fast, good at iteration on pragma combos and bitfield variants. Burns API credits quickly; have usage limit issues around 3-5am Pacific.
- **GLM-5.1** (tmux pane %7) — decent at planning / pseudo-code translation; API has intermittent server errors. Works best as a planner writing to queue briefs.
- **gemma3:4b on proxmox** — benchmarked at 7/13, mediocre. Not recommended for hard functions but can handle trivial accessors.

## Saved memories (read these first)

- `feedback_peephole_local_pragma.md` — the pragma peephole-off trick
- `feedback_gs_event_exec_pipeline.md` — all patterns discovered in the gs_event_exec push
- `feedback_pragma_combos.md` — combos beyond single pragmas
- `feedback_nopeephole_flag.md` — file-level `-opt nopeephole` as an alternative
- `feedback_fp_contract.md` — fmadds fusion via `#pragma fp_contract on`
- `feedback_scheduling_pragma.md` — `#pragma scheduling on` for instruction scheduling matches

## First prompt suggestions for a fresh session

- "Resume decomp work, pick the next small easy function and knock it out."
- "Run `python3 tools/batch_flip_stubs.py gs_pokemon_summary` and commit any wins."
- "Convert fn_XXXXXXXX from src/game/YYY.c."
- "Write 3 new simple accessor functions in gs_pokemon_summary.c — there are lots of small byte-getter patterns."

Good luck — the boulder keeps rolling.
