# Decomp notes: src/game/gs_worldmap.c

## Status snapshot
Most fns @ 100% (re-measure with `tools/objdiff-cli.exe`). Remaining near-misses
are all deep CW-1.3 allocator/codegen walls — see below.

## Blocked near-misses (verified 2026-06-10)

Do not re-attempt without a NEW technique.

- **fn_800265C0 / fn_80026600 / fn_80026640 / fn_80026680 / fn_800266C0 /
  fn_80026700** (the cursor/state sextuplet) — `addr-index-regswap` @ **96.875**
  - Shape: `entry = lbl_80266DD8 + (*(s32*)(ctx+0x1c) << 4); test entry[1]`.
  - Target: `lwz r3,0x60(r3); lis r5; addi r5,r5; lwz r0,0x1c(r3); slwi r0,r0,4;
    add r3,r5,r0` — ctx pointer stays in r3, base addr in r5, index in r0.
  - Ours (opt4): field→r5, addr→r3, index→r3 — full r3/r5 swap. Deterministic
    CW-1.3 reg tie-break at the opt4 canonical form.
  - Swept 2026-06-10: opt4 base-local / idx-split / fresh-vars / compound-assign /
    array-subscript / cast-reorder / operand-swap — ALL produce identical 96.875.
    opt2 → 96.25 (worse). **opt1 → 98.125** (first two instrs match, but the addr
    `addi` deopts into r0 and index lands in r3); the named-`base` form at opt1
    gets the bottom `add r3,r5,r0` PERFECT but adds an `mr r3,r5` (93.4) or
    hoists `lis r5` above the field-load (80.9). No opt level both keeps the clean
    address materialization AND the target's r5/r0 split. → W1/W2 reg tie-break.
  - opt1 98.125 is real but under the +3pt commit bar AND would flip the whole
    sextuplet off the committed opt4 baseline for a cosmetic gain. Not taken.

- **fn_800263B0 / fn_8002641C** — `li-vs-mr` @ **95.56 / 94.78**
  - Only 2 instrs differ. Target: `li r0,0xff; mr r5,r0; mr r6,r0` (one constant
    + two copies into the 3-way-0xff branch). Ours: `li r5,0xff; li r6,0xff`
    (re-materialized).
  - Swept: `r5=r0; r6=r0` copy idiom, s32 vs u8 types, opt2 — CW constant-folds
    the copy back to `li rN,0xff` at every opt level. The `mr` requires r0 to be a
    runtime (non-constant) value, which 0xff is not. Unwinnable without making the
    value opaque (adds instrs). → toolchain-locked.

- **fn_8002BE08** — `regalloc+liveness` @ **97.06**
  - Two issues: (1) r5/r6 tie-break swap in the `(s8)arg0[0x95]+(s8)arg0[0x94]`
    block; (2) the message-id tail (`li r3,0x2b2d/0x2b46/0x2b37; stw r3,
    lbl_8047A3E4@sda21`) — target uses r3 directly; ours keeps `r3val` in r30
    because it's live across the `fn_801440A0` call in the sibling branch. Even
    fixing the tail wouldn't clear the reg-swap. → W2.

- **fn_80028830** @ **96.36** — `@400@sda21` anonymous-sda21 (magic-double literal
  pool) + xoris-block reg permutation (r29/r28 vs r31/r30). → SDA wall + W2.

- **fn_800281F4** @ **95.95** — `li rN, lbl@sda21` vs `addi rN, r13, 0x0` — the
  target materializes sda21 addresses via `li @sda21`; CW emits `addi r13` (sda
  base). Addressing-mode wall.

- **fn_80029CC0 (90.35)**, **fn_8002AEF8 (89.57)**, **fn_8002A5B0 (86.35)** — not
  re-swept this session; larger fns, expect layered walls.

## Filed elsewhere (WALLS.md)
fn_80029FAC / fn_8002A0B8 (97.01, W2 scheduling), fn_8002AE9C (90.39, W2 CSE),
fn_8002A1C4 / fn_8002A2CC (96.97 family).

## Session log
- **2026-06-10** — swept the cursor sextuplet (11 variants), li-vs-mr pair, and
  fn_8002BE08. All confirmed deep walls. Source reverted to baseline. Key finding:
  the `base + index*stride + const_offset` access pattern (forcing explicit
  `add`+offset-load) is a recurring CW-1.3 reg tie-break across this TU AND
  effect_util fn_80135F90/FBC/FF8/024.
