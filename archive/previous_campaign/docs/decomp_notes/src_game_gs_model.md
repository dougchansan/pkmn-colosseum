# Decomp notes: src/game/gs_model.c

## Status snapshot
49/135 @ 100% (36.3%, as of 2026-05-14 commit 170f18a)

## Recently landed (2026-05-14, agent w3)

- **fn_80109290** — 88.6% → 92.6%. Reorder `*(s32*)((u8*)r31 + 0x64) = -1` to AFTER the two `f32` stores (matches target's `stfs f0,0x68/0x6c; stw r0,0x64(r31)` sequence).

## Blocked near-misses

- **fn_80103484** @ 99.5% — `reg-alloc-permutation`
  - Symptom: CW uses scratch r0 instead of r3 in-place for `clrlwi`/`add` operand.
  - Tried: declaration order, type changes.
  - Next leads: `-O2` per-function pragma. Untested.
  - Last attempt: 2026-05-14 (w3)

- **fn_80101A70** @ 98.2% — same `reg-alloc-permutation` (CW scratch r0 vs target r3 in-place)
  - Last attempt: 2026-05-14 (w3)

- **fn_801096AC** @ 98.0% — `fp-reg-alloc-by-liveness`
  - Symptom: target uses f0/f3, we get f3/f0 swapped.
  - Tried: declaration order. CW allocates FP regs by liveness analysis, not name.
  - Next leads: temp var insertion to force liveness ordering. Untested.
  - Last attempt: 2026-05-14 (w3)

- **fn_80103F74** @ 99.1% — `instr-scheduling-order` (prologue-vs-epilogue tradeoff)
  - Symptom: `scheduling off` fixes prologue but breaks epilogue ordering. Accepted as best achievable.
  - Tried: scheduling on/off, push/pop.
  - Next leads: maybe an outer expr reorder lets the scheduler do both correctly.
  - Last attempt: 2026-05-14 (w3)

- **fn_80107F38** @ 94.8% / **fn_801080CC** @ 92.5% — `optlevel-mr-emission`
  - Symptom: `#pragma optimization_level 2` generates extra `mr` instructions and saves values in wrong saved regs.
  - Tried: declaration reordering at -O2.
  - Next leads: stick with -O4 default but rewrite expression structure.
  - Last attempt: 2026-05-14 (w3)

## Untouched near-misses
Many. Run `tools/objdiff-cli.exe` for current list.

## Session log

- **2026-05-14 (w3)** — 1 small improvement (88.6% → 92.6%). 6 fns documented as blocked.
- **2026-05-13 (x6 wave 2)** — 22 → 49 via 4-commit chain.
