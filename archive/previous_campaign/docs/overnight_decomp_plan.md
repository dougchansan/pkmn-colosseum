# Overnight Decomp Plan

Date: 2026-04-10
Mode: Ralph + isolated worktree lanes

## Current verified state

- Verified in the dirty main workspace:
  - `fn_80069048` in `src/game/ui/ui_core.c`
  - `fn_8018F470` in `src/game/people/people.c`
- Scheduler baseline after resync:
  - `2` completed
  - `1006` remaining wrappers
  - `9` simple-tier wrappers still active
- Active lock blockers in the main workspace:
  - `fn_80162070`
  - `fn_80163490`

## Overnight lane architecture

The main workspace is not used for speculative decomp edits. All autonomous
overnight work runs in isolated git worktrees created from `HEAD`:

- `C:\Users\douglaswhittingham\pkmn-colosseum-wt-kimi`
  - branch: `decomp/kimi-overnight`
  - scope: `OSException.c`, `hsd_initialize.c`, `__init_cpp_exceptions.c`, `hsd_mobj.c`
  - model strategy: Kimi first using Moonshot key from local opencode config
- `C:\Users\douglaswhittingham\pkmn-colosseum-wt-ollama`
  - branch: `decomp/ollama-overnight`
  - scope: `gs_texture.c`, `effect_util.c`, `hsd_cobj.c`
  - model strategy: 3090 Ollama first, Codex only for near-match rescue
- `C:\Users\douglaswhittingham\pkmn-colosseum-wt-codex`
  - branch: `decomp/codex-rescue-overnight`
  - scope: near-miss analysis, merge gates, docs/scratch only unless a 100% rescue is proven

## Merge gates

Only merge a worker result back to the main workspace when all conditions hold:

1. The worker reports the exact function(s) and source file.
2. `python tools/compile_check.py <file>` passes in the worker worktree.
3. `python tools/match_test.py fn_XXXXXXXX --verbose` reports `100.0%`.
4. The change does not widen into unrelated source files.
5. The change is replayed or cherry-picked into main and re-verified there before commit.

## Commit / push policy

- Commit only verified 100% matches.
- Prefer one file per commit when a worker owns a file.
- Use Lore commit format with verification trailers.
- Push only commits that are cleanly verified after integration to the main branch context.

## Claude refresh plan

Claude is expected to refresh around local 07:00. After refresh, use Claude only for:

- complex/hard wrappers
- stuck near-misses where free models plateau above 50%
- register-allocation / pragma / compiler-version diagnosis

Free-model-first remains the default until then.
