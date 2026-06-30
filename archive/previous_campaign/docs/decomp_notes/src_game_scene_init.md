# Decomp notes: src/game/scene_init.c

## Status snapshot
107/261 @ 100% (41.0%, as of 2026-05-14 commit a4c3074)

## Recently landed (2026-05-14, agent w10)

5 transition-block-init functions, all via the same pattern:

- **fn_800388C4** (90.6% → 100%) — `#pragma scheduling off` fixes prologue order; target saves LR (`stw r0, 0x14(r1)`) before `li` arg setup. Scheduling off prevents CW reorder.
- **fn_800373C8 / fn_80037468 / fn_80037508 / fn_800375A8** (all 90.2% → 100%) — `#pragma peephole off` eliminates spurious `clrlwi`/`extsb`; **direct `(s16)f32` cast** (not `(s16)(s32)f32`) avoids the extra `extsh r0, r0`. `sth` already truncates to 16 bits.

**Key technique discovered:** `(s16)f32_expr` (direct float-to-s16 cast)
prevents CW from emitting the extra `extsh r0, r0` that `(s16)(s32)f32_expr`
produces. Generalizes beyond this file — file under "float→short store"
pattern.

## Blocked near-misses

- **fn_80037180** @ 96.7% — `branch-layout`
  - Symptom: target has `cmpwi; beq @then; b @else` (2 branches) but CW emits `cmpwi; bne @else` (1 branch) regardless of if/else structure.
  - Tried: `#pragma peephole off` (keeps at 96.7%), peephole on (92.4%), goto-based restructure with peephole off (still 96.7%), temp var + goto.
  - Next leads: unknown — possibly requires a TU-split or forcing CW to place else block before then block via some other means.
  - Last attempt: 2026-05-17 by w2

## Untouched near-misses
Per recon 2026-05-13, 87 near-misses in this file. Top candidates:

| Function | % |
|---|---|
| fn_80040308 | 99.93 |
| fn_8004B7EC | 99.68 |
| fn_80038990 | 98.93 |
| fn_80037180 | 98.70 |
| fn_8004B598 | 98.66 |

## Session log

- **2026-05-14 (w10)** — 5 new 100%s via `#pragma peephole off` + direct `(s16)f32` cast. Pattern works for any "f32 → s16 store" function.

- **2026-05-17 (w2)** — 4 new 100%s via HINT-class analysis:
  - **fn_8004DFCC** (92.39% → 100%): Remove `(u8)` cast on `lbl_8047A524` in the three `fn_801D1650()` calls. The cast forced extra `clrlwi` instructions that target didn't have.
  - **fn_80053C00** (96.30% → 100%): `#pragma peephole off` before function. Target used `clrlwi r0; cmplwi r0, 0` (2 instrs) instead of `clrlwi. r3` (1 instr with record bit).
  - **fn_80057DE8** (95.23% → 100%): `#pragma peephole off`. Same `clrlwi.` vs `clrlwi; cmplwi` pattern on u8 return from `fn_80123FBC`.
  - **fn_80053E7C** (95.43% → 100%): `#pragma scheduling on`. Target used `mr r0, r3; li r3, 0x37; mr r4, r0` pattern (scheduler-generated r0 temp) vs our direct `mr r4, r3; li r3, 0x37`.
  - Blocked near-miss: **fn_80037180** remains at 96.7% — branch-layout mismatch: target has `beq @then; b @else` but CW with peephole off generates `bne @else`. Neither goto-restructure nor scheduling changes fix it. New technique needed.

- **2026-05-17 (w2) — batch 2** — 6 new 100%s:
  - **fn_8004D5EC** (→ 100%): Change `u8 val` → `s8 val`. Target uses `clrlwi r4, r0, 24` (zero-extend to u8 argument) which requires `s8` local to force CW into the signed→unsigned widening path.
  - **fn_8004D64C** (→ 100%): Same `u8 val` → `s8 val` fix (identical twin function).
  - **fn_8004D6F0** (→ 100%): `#pragma scheduling on` — target uses `mr r0, r3; li r3, 0x37; mr r4, r0` (scheduler r0-temp pattern) for the `fn_80132A38(0x37, fn_800FA280())` call.
  - **fn_8004D760** (→ 100%): Same `#pragma scheduling on` fix (identical twin function).
  - **fn_8004BE90** (→ 100%): Swap `u32 r31; u32 r30` → `u32 r30; u32 r31` declaration order. CW assigns lower reg numbers to earlier declarations; target loads r31 from first param.
  - **fn_8004BF20** (→ 100%): Same declaration-order swap (identical twin function).

- **2026-05-17 (w2) — batch 5** — 7 new 100%s:
  - **fn_8004D590, fn_8004D6AC** (→ 100%): `#pragma scheduling on` epilogue fix (same pattern as batch 3).
  - **fn_80053D64** (92.43% → 100%): `#pragma scheduling on` (epilogue + store position) combined with existing `#pragma peephole off`.
  - **fn_80053A60, fn_80053AC8, fn_80053B30, fn_80053B98** (87% → 100%): Triple fix: `#pragma scheduling on` (epilogue) + `#pragma peephole off` (bit-mask clrlwi/rlwinm record-bit) + `s8 val` (forces `clrlwi r4, r31, 24` for fn_80109220 call). All four are identical structure (fn_8011E820() & 0x08/04/02/01 bit tests).

- **2026-05-17 (w2) — batch 4** — 1 new 100%:
  - **fn_80053C84** (92.68% → 100%): `#pragma scheduling on` (epilogue order) + `#pragma peephole off` (u16 clrlwi/cmplwi) + `s8 val` (forces `clrlwi r4, r31, 24` zero-extend before call to `fn_80109220`).
  - Note: `#pragma scheduling on` needed before `peephole off` for the epilogue; peephole off needed for the u16 comparison; s8 type needed for the u8 zero-extend. All three combine for this function.

- **2026-05-17 (w2) — batch 3** — 6 new 100%s via `#pragma scheduling on` epilogue-order fix:
  - **fn_8004D928, fn_8004DB80, fn_8004DF34** (→ 100%): `#pragma scheduling on`. Target epilogue emits `lwz r0, 0x14(r1)` (restore LR) before `lwz r31, 0xc(r1)` (restore callee-save) — same order as prologue. Scheduling on enables CW to reorder the epilogue loads to match this pattern.
  - **fn_80054E7C, fn_80058798, fn_80058AB0** (→ 100%): Same `#pragma scheduling on` epilogue-order fix (identical twin functions calling `fn_80105624()`).
