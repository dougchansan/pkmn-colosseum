# Decomp notes: src/game/battle/battle_waza.c

## Session log

- **2026-06-06 (Codex battle-flow grind)** — corrected the mislabeled waza
  accessor block at `0x801D1470`-`0x801D16F0`. Verified with
  `python tools/compile_check.py src/game/battle/battle_waza.c --diff --symbols fn_801D1470 fn_801D1650 fn_801D167C fn_801D16C4 fn_801D16F0 fn_801D1A44 fn_801D1A88 fn_801D1ACC fn_801D1B10 fn_801D1B4C --timeout 180`.
  Results after correction:
  `fn_801D1470 3/3 100.0000%`,
  `fn_801D1650 11/11 100.0000%`,
  `fn_801D16C4 11/11 100.0000%`,
  `fn_801D16F0 17/17 100.0000%`,
  `fn_801D1A44 17/17 100.0000%`,
  `fn_801D1A88 17/17 100.0000%`,
  `fn_801D1ACC 17/17 100.0000%`,
  `fn_801D1B10 15/15 100.0000%`,
  `fn_801D1B4C 11/11 100.0000%`.
  `fn_801D167C` is real active C and behaviorally corrected but remains at
  `17/19 89.4737%` due only to `li r4, 0xa` scheduling before/after LR save.
- **2026-06-06 (Codex battle-flow grind, continued)** — continued through
  waza active-list and teardown helpers. Verified exact matches with
  `python tools/compile_check.py src/game/battle/battle_waza.c --diff --symbols fn_801D1CC4 fn_801D1F7C fn_801D23C0 fn_801D2B08 fn_801D2F94 fn_801D301C fn_801D3034 --timeout 180`.
  New exact matches:
  `fn_801D1F7C 11/11 100.0000%`,
  `fn_801D2F94 34/34 100.0000%`,
  `fn_801D301C 6/6 100.0000%`,
  `fn_801D3034 34/34 100.0000%`.
  Semantic active-C bodies added but still non-matching:
  `fn_801D1CC4 34/38 89.4737%` (remaining `li r5`/epilogue scheduling),
  `fn_801D23C0 13/18 72.2222%` (remaining scheduling/signed compare shape),
  `fn_801D2B08 12/19 63.1579%` (remaining `lbl_80467390` base-store shape).
- **2026-06-06 (Codex battle-flow grind, effect-helper pass)** — corrected
  nullable effect field accessors and resolve/update helpers in the
  `0x801D9E1C`-`0x801DAC78` range. Verified with
  `python tools/compile_check.py src/game/battle/battle_waza.c --diff --symbols fn_801D9E1C fn_801DA354 fn_801DA42C fn_801DA5AC fn_801DA8C4 fn_801DA914 fn_801DA94C fn_801DA9B4 fn_801DAC24 fn_801DAC3C fn_801DAC54 fn_801DAC78 --timeout 180`.
  New exact matches:
  `fn_801D9E1C 6/6 100.0000%`,
  `fn_801DA354 6/6 100.0000%`,
  `fn_801DA42C 7/7 100.0000%`,
  `fn_801DA5AC 6/6 100.0000%`,
  `fn_801DA8C4 20/20 100.0000%`,
  `fn_801DA914 14/14 100.0000%`,
  `fn_801DA9B4 13/13 100.0000%`,
  `fn_801DAC24 6/6 100.0000%`,
  `fn_801DAC3C 6/6 100.0000%`,
  `fn_801DAC54 9/9 100.0000%`,
  `fn_801DAC78 6/6 100.0000%`.
  `fn_801DA94C` is real active C and remains at `25/26 96.1538%`
  due only to `clrlwi r3,r4,24` versus `mr r3,r4` on the boolean return.
- **2026-06-06 (Codex battle-flow grind, setup/flag-helper pass)** —
  continued in `battle_waza.c` through the waza entry-pool setup helpers,
  field/lighting flag togglers, and small data accessors. Verified with
  `python tools/compile_check.py src/game/battle/battle_waza.c --diff --symbols fn_801DB060 fn_801DB088 fn_801DB100 fn_801DB154 fn_801DB1CC fn_801DB848 fn_801DB850 fn_801DB858 fn_801DCDA8 fn_801DCDCC fn_801DCEA8 fn_801DCF84 fn_801DCFD8 fn_801DD028 fn_801DD078 fn_801DD0C8 --timeout 180`.
  New exact matches:
  `fn_801DB060 10/10 100.0000%`,
  `fn_801DB100 21/21 100.0000%`,
  `fn_801DB154 30/30 100.0000%`,
  `fn_801DB848 2/2 100.0000%`,
  `fn_801DB850 2/2 100.0000%`,
  `fn_801DB858 3/3 100.0000%`,
  `fn_801DCDA8 9/9 100.0000%`,
  `fn_801DCDCC 16/16 100.0000%`,
  `fn_801DCEA8 22/22 100.0000%`,
  `fn_801DCF84 21/21 100.0000%`,
  `fn_801DCFD8 20/20 100.0000%`,
  `fn_801DD028 20/20 100.0000%`,
  `fn_801DD078 20/20 100.0000%`.
  Semantic active-C non-wins left in this pass:
  `fn_801DB088 19/34 55.8824%` (pool cleanup loop is real C; remaining
  differences include loop register allocation and the legacy no-argument
  `fn_801D2D28` call shape),
  `fn_801DB1CC 42/47 89.3617%` (resource-release logic is real C; remaining
  differences are the swapped `r30`/`r31` resource-id temporaries),
  `fn_801DD0C8 8/14 57.1429%` (linked-list resolver is real C; remaining
  differences are scratch register allocation for the two halfword keys).
