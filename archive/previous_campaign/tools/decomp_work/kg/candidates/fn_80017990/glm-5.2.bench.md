# GLM-5.2 Diff-Repair Candidacy Benchmark - fn_80017990

**Date:** 2026-06-15
**Loop:** `kg_diff_repair.py` (diff-aware minimal-edit repair)
**Target:** `fn_80017990` (`src/game/gs_pokemon_summary.c`), real active C, near-miss.

## Result

| Stage | pct (trusted objdiff) |
|---|---|
| KG baseline / current source | 99.67742 |
| deepseek-v4-pro (1 round) | 99.67742 (no improvement) |
| kimi-k2.7-code (1 round) | 99.67742 (no improvement) |
| mimo-v2.5-pro (1 round) | 99.67742 (no improvement) |
| **GLM-5.2 (1 diff-repair round)** | **100.00000 - accepted as byte-match only** |

Verified by the loop's own `splice_and_measure` (cs_splice live-copy + band.py real CW flags + objdiff). Win promoted to `repair_pool/fn_80017990/shared_best.c` (99.67742 -> 100.0) and recorded to KG via `bestof.py record ... --rep glm-5.2 --lever commutative-operand-order`.

Portability status: not final. This is a valid original-toolchain byte-match,
but raw table addressing is still portability debt. Community/PC-port acceptance
needs a separate gate: no unresolved pointer arithmetic, typed data definitions,
and readable field access that still preserves behavior. A follow-up pass moved
the raw spelling behind `SummaryPageEntry` accessors, but direct typed-array
indexing still regressed MWCC output, so more type/data recovery remains.

## The wall (single instruction)

```
TARGET (aim for)            | OURS
add r5, r6, r8   >>>        | add r5, r8, r6
```

One commutative add operand-order divergence on the `*(s32*)` compare load. Everything else byte-matched. The branch-target address deltas (`0x1241c` vs `0x29e0`) are base-address artifacts, not real diffs.

## The edit (one line, line 3 only)

Address-of-array-element form lowered index-first; pointer-arithmetic form lowers base-first.

```c
// before  (emits  add r5, r8, r6  - index first)
if ((s32)*(s16*)(dst + 6) == *(s32*)(&lbl_80266918[idx * 0x4C + 0x10])) {

// after   (emits  add r5, r6, r8  - base first; +0x10 stays load displacement)
if ((s32)*(s16*)(dst + 6) == *(s32*)((u8*)lbl_80266918 + idx * 0x4C + 0x10)) {
```

Signature preserved; lines 4-12 untouched (their `lbzx`/base-first adds already matched). Lever = `commutative-operand-order` (cf. memory `feedback_commutative_operand_order_unsolvable` - here the address-of vs pointer-plus form flipped it; not the unbreakable saved+call case).

## Verdict

GLM-5.2 is a **viable diff-repair candidate** for byte-matching: it read the current best C + exact objdiff, isolated the single divergent instruction, applied one minimal edit, and produced a trusted byte-exact 100% where three HTTP models stalled at baseline. This should not be treated as proof that the recovered source is port-ready; it is a useful compiler oracle for the next typed, portable recovery pass.

Candidate: `tools/decomp_work/kg/candidates/fn_80017990/glm-5.2.c`
