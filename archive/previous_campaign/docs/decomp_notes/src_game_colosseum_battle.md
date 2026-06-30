# Decomp notes: src/game/colosseum_battle.c

## Status snapshot
199/646 @ 100% (30.8%, as of 2026-05-13)

## ⚠️ File-level diagnosis (2026-05-13, w5 attempt)

Agent w5 stalled trying to find 95-99.99% near-misses and reported:

> **The 199 matched are all simple/short fns. The unmatched 447 are complex
> asm-style fns sitting in the 35-60% band — there are essentially no
> 95-99% near-misses in this file.**

This means **the easy-win sweep strategy will not work here.** The file
needs:

1. **Full decomp passes** on the asm-style fns to lift them from 35-60%
   to 90%+ before any near-miss work
2. Or **TU split** so the .o becomes smaller targets — but we don't have
   evidence the original was multi-TU

## Recently landed (prior session, x8 wave 2)

5 commits cherry-picked, 175 → 199:

- fn_8025DD14 (if-inversion)
- fn_802612D0 (base-pointer array pattern)
- fn_8025E534 (97.9% array + casts, partial)
- fn_80261D8C (80.5% variadic, partial)
- fn_8025F2FC (85.2% revert goto, partial)
- + 8 type-fix wins

## Untouched
Most remaining unmatched fns. Use `python tools/objdiff-cli.exe diff -1 <target> -2 <obj>` to see per-fn match; focus on fns where match% is >70% before attempting near-miss work.

## Session log

- **2026-06-10 (Wave 2, 70-85 band)** — band re-scan: the 70-100 objdiff band
  is 17 fns. Cracks + improvement:
  - `fn_8025FE84` 83.13 -> **100.0%** — target returns 0 when `*r3==0` via a
    separate `li r3,0` arm (Ghidra had `if(*r3!=0){...} return field4;` returning
    field4 even on the zero path). Fix: result-var two-arm `if(*r3!=0){res=field4;}
    else{res=0;} return res;` reproduces the `beq->li r3,0; b epilogue` shape.
    Commit 81ded161.
  - `fn_8025DA3C` 76.19 -> **100.0%** — target materializes the default `li r3,2`
    BEFORE the cmpwi cascade. Fix: hoist `res=2;` before a NO-default switch
    (cases assign `res` + break), then `return res;`. Commit fd894d58. **LEVER:
    default-value hoist before a switch reproduces the early-default + signed
    3-way dispatch.**
  - `fn_8025E9BC` 88.57 -> **93.13%** (improvement, not 100) — multiple type +
    SDA + frame fixes: fn_80123FBC/fn_80121ADC return `u8` (clrlwi vs extsb byte
    compares); fn_8011F228/fn_8011EE58/fn_8011EDF8 return `u16` + masked u16 sum
    (clrlwi vs extsh); `(u16*)&lbl_80478DB0` address-of -> `li @sda21` (was `lwz`
    of value); REVERSED stack-array decl order so `local_28` lands at sp+0x8.
    Residual 16 = W2 r0-hop reg routing on the fn_80132A38 args + a fn_801065B8
    branch-merge shape; not C-controllable. Commit d0b4ed05.
  - **W4 confirmed DOMINANT**: fn_8025CC90/fn_8025CBE8/fn_80255D3C/fn_80256AE0 +
    the 76.92 quintuplet (fn_8025A220/fn_80254F54/fn_80254EE8/fn_80254180/
    fn_80253B44, all 10/13 IDENTICAL siblings) are ALL W4 bool-materialize
    (`subic/subfe` carry idiom vs CW `neg/or/srwi` or `cntlzw/srwi`). FAST-SKIP.
  - W3/W1 walls in band: fn_80261FB4/fn_80261CBC (W3 stmw + reg-band),
    fn_80266250/fn_8025E0B0 (W1 reg-band permutation), fn_8025E1BC (W6 sda-name +
    W4 + reg order). Rename candidates fn_802606D8 (->cameraSetFov) / fn_802615F4
    (->memcpy/GSmodelPopState) exist but are <72% multi-wall (stmw r22 band,
    psq_st, W4) — rename is a tiny fraction, not worth pursuing to 100.
- **2026-06-06 (Codex battle-flow grind)** — `fn_8025F2FC` converted from
  83.3333% to **100.0000%** active C by expressing the original shared
  completion branch for request slots where `entry[5] != 0 || entry[6] != 4`.
  Verified with `python tools/compile_check.py src/game/colosseum_battle.c --diff --symbols fn_8025F2FC fn_8025F514 --timeout 180`:
  `fn_8025F2FC 21/21 instructions, 0 mismatches, no active asm wrapper`.
- **2026-06-06 (Codex battle-flow grind)** — `fn_8025F514` converted from
  60.0000% to **100.0000%** active C by adding a local `#pragma scheduling off`
  around the battle request-stop flag setter. Verified with
  `python tools/compile_check.py src/game/colosseum_battle.c --diff --symbols fn_8025F514 fn_8025F584 fn_8025F648 --timeout 180`:
  `fn_8025F514 4/4 instructions, 0 mismatches, no active asm wrapper`.
- **2026-05-13 (w5)** — stalled, no salvageable commits. Identified the
  "no actual near-misses" problem above. Recommend full-decomp wave on the
  35-60% cluster, not easy-win sweeps.
- **2026-05-13 (x8 wave 2)** — 175 → 199 via 5-commit cherry-pick chain.
