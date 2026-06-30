# automatch report: effect_util.c

baseline 141/207 @ 100%, swept 30 near-misses in 1892s

## Solved (deterministic, zero-token)

- **fn_80131714** 99.19% -> 100% via `#pragma peephole off`
- **fn_80132570** 99.19% -> 100% via `#pragma peephole off`
- **fn_8013264C** 99.00% -> 100% via `#pragma peephole off`
- **fn_80131630** 91.67% -> 100% via `#pragma optimization_level 2`
- **fn_80131660** 91.67% -> 100% via `#pragma optimization_level 2`

## Improved but not 100%

- fn_80131F04 96.32% -> 96.45% via `#pragma optimization_level 1`
- fn_80131FF4 96.32% -> 96.45% via `#pragma optimization_level 1`
- fn_8013208C 96.32% -> 96.45% via `#pragma optimization_level 1`
- fn_80132124 96.32% -> 96.45% via `#pragma optimization_level 1`
- fn_801321BC 96.32% -> 96.45% via `#pragma optimization_level 1`
- fn_80132A38 94.36% -> 99.92% via `#pragma peephole off`
- fn_80135938 93.79% -> 94.92% via `#pragma optimization_level 1`
- fn_8013583C 88.41% -> 94.29% via `#pragma optimization_level 1`

## Blocked — escalate to LLM agent

- fn_80135D10 stuck at 99.87% (no pragma variant helped)
- fn_80135168 stuck at 99.85% (no pragma variant helped)
- fn_801334A8 stuck at 99.62% (no pragma variant helped)
- fn_801334DC stuck at 99.62% (no pragma variant helped)
- fn_801349DC stuck at 99.15% (no pragma variant helped)
- fn_80135F90 stuck at 98.18% (no pragma variant helped)
- fn_80135FF8 stuck at 98.18% (no pragma variant helped)
- fn_80136024 stuck at 98.18% (no pragma variant helped)
- fn_80135FBC stuck at 98.00% (no pragma variant helped)
- fn_80135E44 stuck at 97.54% (no pragma variant helped)
- fn_8013327C stuck at 97.39% (no pragma variant helped)
- fn_8013356C stuck at 93.46% (no pragma variant helped)
- fn_80133810 stuck at 92.84% (no pragma variant helped)
- fn_8013433C stuck at 92.40% (no pragma variant helped)
- fn_80134EF0 stuck at 91.45% (no pragma variant helped)
- fn_80134F88 stuck at 91.28% (no pragma variant helped)
- fn_8013467C stuck at 88.20% (no pragma variant helped)

## Wall classification (2026-06-10 manual sweep)

- **fn_80131BF8 / fn_80131C20 / fn_80131C48 / fn_80131C70 / fn_80131C98 /
  fn_80131CC0** (the 99.5 sextuplet) — **W6 name-only**. The single diff is
  `bl _msgctrlSideName__FP15FightOutPokemonUc` (target) vs `bl fn_80131CE8`
  (ours). Byte-identical relative branch; only the call-target symbol is unnamed.
  Fix = rename fn_80131CE8 to its real mangled symbol (cross-file naming infra,
  not a C-shape change). Byte-neutral.

- **fn_80131714** (99.76) — **W6 name-only**: `bl GSmsgAdjustAlign` vs
  `bl fn_800FA064`. Same address, unnamed callee. Byte-neutral.

- **fn_80135F90 / fn_80135FBC / fn_80135FF8 / fn_80136024** (98.18) —
  `addr-index-regswap`, the SAME wall as the gs_worldmap cursor sextuplet
  (see src_game_gs_worldmap.md). Access `lbl_80363B18 + index*0xA + const` forces
  an explicit `add`+offset-load; target emits `mulli r0; lis/addi r3; add r3,r3,r0`
  (index in r0, base in r3), CW emits `mulli r4; addi r0; add r3,r0,r4`. Note the
  sibling **fn_80136050** (offset +0x0) is 100% precisely because it collapses to a
  single `lbzx r3,r3,r0` with no explicit add — no tie-break. Swept: base+offset
  reassociation, pointer-accumulate (`p+=`), off-first split, named-base — all
  fold back to the 98.18 canonical at opt4; lower opt deopts. → W1/W2.
