# Handoff: parallel-agent decomp of remaining gs_title.c functions

This is the boot prompt for the next session. Use it to spawn parallel
subagents (Opus / Codex / Kimi-via-attack_function) to push the remaining
14 gs_title.c functions to ≥90% byte-match (commit threshold) and ideally
to 100%.

## Quick state

- **Repo:** `C:/Users/douglaswhittingham/pkmn-colosseum` (master)
- **Active worktree branch:** `decomp-opus-fns` at `C:/Users/douglaswhittingham/pkmn-colosseum-opus`
- **Latest committed match%s:**
  - `41795ca` fn_80020C9C 100%
  - `a30f607` fn_800218BC 94.5%
  - `973e357` fn_80020F54 100%
- **WIP candidates** in `.omc/decomp_attack/<fn>_wip.md` (5 functions; see leaderboard below)

## How to spawn the next session cleanly

```bash
# 1. Sync 3090 with current master
cd /c/Users/douglaswhittingham/pkmn-colosseum
scp src/game/gs_title.c douglaswhittingham@10.0.0.3:/storage/finetune/pkmn-colosseum/src/game/gs_title.c

# 2. Read the proven workflow + cheatsheets
cat tools/decomp_work/WORKTREE_WORKFLOW.md         # parallel-agent gotchas
cat tools/decomp_work/CW_QUIRKS.md                 # CW 1.3 idioms
cat tools/decomp_work/few_shot_examples.md         # 9 verified 100% patterns
cat tools/decomp_work/REFERENCE_PROJECTS.md        # external decomp project hints
cat tools/decomp_work/BENCHMARK_SUMMARY.md         # cross-model leaderboard

# 3. Pick a function, follow the proven workflow (see below)
```

## Remaining 14 functions, prioritized

Based on observed difficulty patterns:

### Tier 1 — likely fast wins (similar shape to committed wins)

| Function | asm lines | notes |
|---|---|---|
| `fn_80023DA8` (only 60 lines) | 60 | currently #if 0 with partial — check if just needs flip |
| `fn_80023E60` | 212 | partial in #else (line 3793) — was at #pragma scheduling off + works for shape |
| `fn_80020618` | 206 | needs first attempt — no partial |
| `fn_8002217C` | 206 | needs first attempt |
| `fn_80022478` | 184 | partial in #else (line 3052) |

### Tier 2 — known structural complexity (regalloc may block 100%, target 90%+)

| Function | asm lines | notes |
|---|---|---|
| `fn_80024160` | 123 | WIP @ 87.7% — regalloc-blocked. See `.omc/decomp_attack/fn_80024160_wip.md` |
| `fn_80023760` | 140 | WIP @ 75.1% — regalloc + stack-offset |
| `fn_80023B9C` | 143 | WIP @ 73.5% — regalloc + stack-offset, if-direction already fixed |
| `fn_80023068` | 143 | retry-loop dispatch, similar family to 80023760/B9C |
| `fn_80022834` | 214 | medium structural |
| `fn_80022B3C` | 217 | medium structural |
| `fn_80025730` | 179 | gs_title's central state machine — Kimi got 76% transient earlier |
| `fn_80023968` | 155 | first attempt failed signature conflict — needs (void) sig + casts |

### Tier 3 — hard (large or table-copy idiom)

| Function | asm lines | notes |
|---|---|---|
| `fn_800216E8` | 133 | WIP @ 46.6% — `mtctr/lwzu/stwu` table-copy idiom mismatch |
| `fn_800210F0` | 341 | save-data load — biggest |
| `fn_80021B14` | 366 | keyboard render — biggest |
| `fn_800232F0` | 298 | sound scheduler |

## Proven workflow (replicate per function)

This worked for fn_80020F54 → 100% and fn_80020C9C → 100%:

```
1. Read src/game/gs_title_<fn>.inc
2. Read the existing #else partial in src/game/gs_title.c (if any)
3. Submit existing partial via remote_reward to get baseline %
4. Identify structural fix (switch vs if/else, case order, fall-through).
   Switch with case bodies in TARGET'S source order is the key fix.
5. Block-scope extern declarations INSIDE the function body with the right
   signatures. Never modify file-scope externs (causes cascade compile errors).
6. Use cast-fnptr `((void(*)(args))fn_X)(args)` only when the function being
   called has an asm-active wrapper with a different signature.
7. Run python tools/decomp_work/benchmark/local_iterate.py to compile + diff.
8. Apply per-quirk fixes (see CW_QUIRKS.md):
   - `((s8*)arg)[N] &= ~K` for extsb-without-clrlwi
   - `s32 v = *arg` to defeat dot-form fusion
   - `(u8)fn_X()` cast to add clrlwi+cmplwi pattern
   - `for (ctr = count; ctr != 0; ctr--)` for mtctr/bdnz
   - Block-scope `{ }` for SDA reload-twice patterns
9. Iterate until ≥90% (commit floor) or 100%.
10. Update gs_title.c #if 1 → #if 0, paste candidate into #else block.
11. Verify 3-step:
    rm build/GC6E01/base/game/gs_title.o &&
    python tools/compile_check.py src/game/gs_title.c &&
    scp src/game/gs_title.c douglaswhittingham@10.0.0.3:/storage/finetune/pkmn-colosseum/src/game/gs_title.c &&
    ssh douglaswhittingham@10.0.0.3 "cd /storage/finetune/pkmn-colosseum && rm -f build/GC6E01/base/game/gs_title.o && python3 tools/compile_check.py src/game/gs_title.c && python3 tools/match_scan_file.py gs_title fn_X"
12. Commit with message in the established style:
    "gs_title: fn_X NN% byte-match" + paragraph of techniques applied.
```

## Three subagent invocation templates

### Template A — for the Opus subagent (deep iteration)

```
You are decompiling fn_XXXXXXXX in src/game/gs_title.c to byte-match Codewarrior 1.3 output.

Required reads (in order):
1. tools/decomp_work/HANDOFF_PARALLEL_AGENTS.md (this file)
2. tools/decomp_work/WORKTREE_WORKFLOW.md
3. tools/decomp_work/CW_QUIRKS.md
4. tools/decomp_work/few_shot_examples.md
5. src/game/gs_title.c — find your fn's existing #else block and surrounding externs
6. src/game/gs_title_<fn>.inc — the target asm

Use a git worktree at ../pkmn-colosseum-<your-name> on a fresh branch.
Copy the toolchain artifacts (mwcc_compiler/, objdiff-cli.exe, build/GC6E01/{obj,base/game}/*.o, openrouterkey.txt) per WORKTREE_WORKFLOW.md.

Iterate via tools/decomp_work/benchmark/local_iterate.py until ≥90%.
Commit at ≥90% with the standard "gs_title: fn_X NN% byte-match" message.
If stuck below 90% after 8+ iterations, save WIP to .omc/decomp_attack/<fn>_wip.md and pick a different function from Tier 1.

Hard rules:
- Re-Read the .c file immediately before each Edit (local_iterate mutates the file).
- Never modify file-scope `extern void fn_X(void)` declarations — use block-scoped externs or cast-fnptr instead.
- C89 only: declare ALL locals at top of each block.
- Always block-scope new externs INSIDE the function body, not at file scope.
```

### Template B — for the Codex subagent (parallel mechanical attack)

```
Run the multi-model attack pipeline on fn_XXXXXXXX:

cd <your-worktree>
python tools/decomp_work/benchmark/attack_function.py \
    --suite tools/decomp_work/benchmark/test_suite_gs_title_expert.json \
    --fn fn_XXXXXXXX \
    --use-codex \
    --repair-rounds 3 \
    --max-tokens 4096

After it runs, inspect .omc/decomp_attack/<fn>.json for the best candidate.
If best is ≥90%, paste it into src/game/gs_title.c #else block, verify, commit.
If 50–90%, hand-finish it with the workflow in HANDOFF_PARALLEL_AGENTS.md.
If <50%, save WIP and pick a different function.
```

### Template C — for the Kimi subagent (simple/medium structural)

```
Decompile fn_XXXXXXXX in src/game/gs_title.c using the proven 11-step workflow in tools/decomp_work/HANDOFF_PARALLEL_AGENTS.md.

Pre-load context: tools/decomp_work/CW_QUIRKS.md + tools/decomp_work/few_shot_examples.md.

Key idioms for byte-match success:
- Switch with cases in TARGET asm's body-layout order
- Block-scoped externs with the right signature inside the function
- Cast-fnptr `((void(*)(args))fn_X)(args)` for functions whose file-scope extern is `void(void)` 
- `((s8*)p)[i] &= ~K` to force `extsb` instead of `clrlwi` for byte-mask stores
- `for (ctr = count; ctr != 0; ctr--)` for mtctr/bdnz CTR-loop pattern

Iterate via remote_reward until ≥90%, commit, move on.
```

## Worktree allocation suggestion

Spawn 3 parallel worktrees:

```bash
git worktree add ../pkmn-colosseum-opus2  -b decomp-opus-tier1
git worktree add ../pkmn-colosseum-codex2 -b decomp-codex-tier2  
git worktree add ../pkmn-colosseum-kimi2  -b decomp-kimi-tier3
```

Assign:
- **opus-tier1**: fn_80020618, fn_8002217C, fn_80022478, fn_80023DA8 (probably fast wins)
- **codex-tier2**: fn_80024160 (continue from 87.7% WIP), fn_80023760 (75.1% WIP), fn_80023B9C (73.5% WIP) — regalloc mining
- **kimi-tier3**: fn_80023068, fn_80022834, fn_80022B3C, fn_80025730 — medium complexity

Each worktree commits to its branch; merge back to master when each function is at ≥90%.

## Merge protocol

```bash
cd /c/Users/douglaswhittingham/pkmn-colosseum
git fetch
git merge --ff-only decomp-opus-tier1   # if linear
# OR for non-linear:
git cherry-pick <sha-from-each-branch>
```

After all 14 functions are at ≥90% committed: gs_title.c is fully matched. Then move to next file (e.g., gs_field_world.c which has the cw-focus benchmark suite).

## Cost budget reminder

- DeepSeek: $9.50 remaining of $10 (used ~$0.50 cumulatively)
- Moonshot Kimi: free tier
- OpenRouter: ~$0.20 used
- Codex: ChatGPT plan, resets every ~5h
- 3090 reward: free
