# Decomp notes: src/game/gs_field_world.c

## Status snapshot
649/735 @ 100% (88.3%, as of 2026-05-13 commit 51e49ec)

## Recently landed (2026-05-13, agent w1)

- **fn_8011E1D4 / E21C / E264 / E2DC / E324** — 99.72% → 100%. Change `u16 idx` → `s32 idx`. Forces CW to emit `clrlslwi r4, r4, 16, N` using the original param register instead of zero-extending through a temp in r0.
- **fn_801231A4** — 99.24% → 100%. `threshold <= (u8)count` → `(u16)threshold <= (u16)(u8)count`. Forces unsigned `cmplw` instead of signed `cmpw`.
- **fn_80129718** — 98.75% → 100%. Cast `fn_80142368(...)` return to `(u32)` before `!= 0`. Forces `cmplwi` (unsigned) instead of `cmpwi` (signed).
- **fn_80129514** — 98.68% → 100%. `u16 arg2, u16 arg3` → `s32 arg2, s32 arg3`. Fixes register allocation for `clrlslwi`.

Technique pattern: **demoting `u16` params to `s32`** keeps the value in the original param register through `clrlslwi`, avoiding an extra `clrlwi`/`mr` pair.

## Untouched near-misses
86 near-misses remain (per recon 2026-05-13). Top candidates:

| Function | % |
|---|---|
| fn_80129280 | 99.85 |
| fn_80128E38 | 99.60 |
| fn_801294C4 | 99.50 |

Run `tools/objdiff-cli.exe` for the up-to-date list.

## Blocked near-misses
None identified yet — explore freely.

## Session log

- **2026-05-13 (w1)** — 8 near-misses pushed to 100% via `u16→s32` and unsigned-cast techniques.

- **2026-05-17 (w1)** — 2 new 100% matches, 2 partial improvements:
  - **fn_80115AC8** 93.44%→100%: `#pragma peephole off` — stops CW from using `mr.`/`clrlwi.` record variants, forces explicit `cmplwi 0x0` compares to match target.
  - **fn_8011D504** 94.81%→100%: Changed `if (idx >= lbl_80478F90)` to `if (idx >= *(u32*)lbl_80478F90)` — target double-dereferences the SDA pointer (confirmed by `lwz r5, ...; lwz r0, 0.0(r5)` pattern in target). The symbol is a pointer-to-header, not a direct scalar count.
  - **fn_8011AB50** 98.43%→98.60%: Swapped local decl order to `u8* base; u16 idx;` so base→r27, idx→r28 aligns with target. The `mr r0, r3; mr r27, r0` extra-intermediate pattern remains (CW scheduling artifact, unresolved).
  - **fn_8011AFCC** 98.43%→98.60%: Same decl swap as AB50.

  ### Blocked near-misses (new) from this session

  - **fn_8011CEF0 / fn_8011D24C / fn_8011E078** @ 98.89% / 98.89% / 99.23% — `add r3, r0, r3` vs `add r3, r3, r0` operand swap. CW puts computed offset (r0=clrlslwi result) as left operand of add instead of ptr (r3). Tried: `ptr + idx*2 + offset` (made worse with `sthx`), `s32 idx` with `(u16)` cast (same diff). Root cause: CW's internal expression tree builds `r0 + r3` when ptr[idx*2 + offset] is written, wants left=ptr for `ptr + offset` form. No source change found.
  - **fn_801293FC / fn_801294C4** @ 99.33% / 99.50% — `add r5, r31, r3` vs `add r5, r3, r31` (fn_result + saved_offset operand order). CW puts callee-saved offset register (r31) first because it was assigned before the function call. No source change found.
  - **fn_8011ED18** @ 99.00% — r0 vs r3 as scratch register for temporary load. CW uses r0 to load the conditional value (spares r3=ptr for later fn call), target reuses r3 destructively. No pragma or expression change found.
  - **fn_8011A280 / fn_8011A570 / fn_8011A9EC / fn_8011AB50 / fn_8011AFCC** @ 98.60% — `mr r0, r3; mr r27, r0` vs `mr r27, r3`. CW inserts r0 intermediate when saving fn_80119F10 result to callee-save. Tried: `scheduling off` (worsened prologue), `optimization_level 2` (wrong reg layout), decl reorder (partial improvement). CW scheduling artifact.
  - **fn_8011B67C** @ 98.43% — same `mr r0, r3; mr r31, r0` intermediate pattern.
  - **fn_8011CBC8** @ 93.64% — `slwi r0, r5, 1` vs `clrlslwi r0, r3, 24, 1` AND global loaded into r3 (base) vs r4 (target). With `peephole off` already applied. Tried: removing pragma (major regression). CW peephole/regalloc conflict.
  - **fn_80115D64** @ 98.50% — 8-way reg-alloc permutation (r27-r31 shifted). Tried: decl reorder (made worse). Deep CW allocator issue.
  - **fn_8012F150** @ 99.07% — float registers f0/f1 vs f1/f2 (shifted by 1). Tried: dummy f0 variable (optimized away). CW float alloc starts at f0 not f1.
  - **fn_80128E38** @ 99.60% — `addi r4, r2, 0x0` vs `li r4, lbl_8047D028@sda21` in .inc asm stub. `la r4, lbl_8047D028(r2)` in inc file doesn't generate proper sda21 reloc. Cannot fix without editing .inc file (anti-fraud rule).

  ### Technique notes
  - `#pragma peephole off` fixes `mr.`/`clrlwi.` record-bit vs explicit `cmplwi 0x0` pattern (fn_80115AC8 case). Already applied to some functions; check near-misses with `[DIFF_OP_MISMATCH] mr. vs mr` pattern.
  - Double-deref lbl_80478F90: this is a pointer-to-count in SDA, many similar fns likely have `*(u32*)lbl_80478F90` pattern. Check other fns that compare against it directly.
