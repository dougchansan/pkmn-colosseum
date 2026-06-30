# Decomp notes: src/game/battle/battle_main.c

## Status snapshot
~10/21 @ 100% (as of 2026-05-13 commit 211f7c8)

## Recently landed (2026-05-13, agent w7)

- **fn_801EF6FC** — 100% (decomp)
- **fn_801EF758** — 100% (decomp)
- **fn_801EF7C4** — 100% — clean nested-loop implementation. Key trick: `fn_801F54A4` returning `s32` (not `void`) causes CW to emit `clrlwi` at the u16 assignment, which reduces callee-save count from `stmw r22` (10 regs) to `stmw r24` (8 regs). Declaration order `void* r31; u16 r30, r29, r28, r27, r26; void* r25` matches CW's allocation. `_h` as an unnamed `void*` temp stays out of the callee-save set.

## Blocked near-misses — `stmw-emission` cluster

A whole family of fns is stuck because **CW 1.3 won't emit `stmw/lmw` for
this TU at the call-site counts that target was built with.** Pattern is
the same across all of these — CW emits individual `stw/lwz` pairs even
with `#pragma use_lmw_stmw on`.

- **fn_801EF214 / fn_801EF274** @ 80.08% — 2-reg stmw
- **fn_801EF02C / fn_801EF080 / fn_801EF0D4** @ 78.10% — 2-reg stmw
- **fn_801EFFC4** @ 74.44% — 2-reg stmw r30
- **fn_801EF128** @ 60.79% — 5-reg stmw + complex struct-init pattern
- **fn_801EF644** @ 26.91% — 4-reg stmw, saves interleaved with volatile arg capture
- **fn_801EF95C** @ 26.88% — 2-reg stmw

  - Tried: `#pragma use_lmw_stmw on` file-wide, function-local pragma push/pop, opt level tweaks.
  - Next leads: **TU split** — break this file into smaller TUs so each one
    independently triggers CW's stmw heuristic. See `docs/tu_split.md`.
    Untested.
  - Last attempt: 2026-05-13 (w7)

## Untouched near-misses
None — agent w7 worked through all remaining near-misses in this file.

## Session log

- **2026-05-13 (w7)** — 3 new 100%s. Identified `stmw-emission` as the dominant blocker for the remaining ~11 functions. TU split is the only known unblock.
