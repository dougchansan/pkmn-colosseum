# Decomp notes: src/game/gs_title.c

## Status snapshot
~42/65 @ 100% (rough, re-measure with `tools/objdiff-cli.exe`)

## Blocked near-misses

All remaining near-misses (>= 90%) are blocked by these deep compiler-level
issues. **Do not re-attempt these unless you bring a new technique.**

- **fn_80024DBC / fn_80024F2C / fn_80024F9C / fn_8002520C / fn_80024308** —
  `anonymous-sda21`
  - Symptom: CW emits `lis r3, @255@sda21@ha; addi r3, r3, @255@sda21@l` for
    the float-bias constant. Target uses a named label
    (e.g. `lbl_8047B8D0@sda21@ha`). objdiff sees this as a symbol mismatch
    that no source change can fix.
  - Tried: every pragma combo, lifting the constant into a global, casting
    rearrangement — all attempts produce the same anonymous name.
  - Next leads: linker-level remapping, custom mwcc patch, or moving the
    .sdata2 entry into a different `.c`/`.h` to coincidentally change
    objdiff's anonymous numbering. Untested.
  - Last attempt: 2026-05-13 (w9 burned ~20 attempts here)

- **fn_80025490 / fn_8002537C / fn_80024A2C** — `peephole-bne-b`
  - Symptom: target was compiled with the backend peephole optimizer active
    (`beq @far` → `bne @near; b @far`). Our build uses global
    `-opt nopeephole`, so CW never produces `bne; b` pairs.
  - Tried: condition inversion at C level, function-local
    `#pragma push/pop peephole on`, all unsuccessful (file-wide flag
    overrides).
  - Next leads: split the file into two TUs so just these fns build with
    peephole on. Untested.
  - Last attempt: 2026-05-13 (w9)

- **fn_800218BC / fn_80024CDC / fn_80024308** — `instr-scheduling-order`
  - Symptom: CW's scheduler hoists `lfs/lfd` loads earlier than target.
    Target has `lfs f31; li r31, 0x0`, we get them swapped.
  - Tried: 5+ scheduling/peephole pragma combos.
  - Next leads: per-function scheduling pragma push/pop. Untested in
    isolation (was combined with other changes that may have masked the
    effect).
  - Last attempt: 2026-05-13 (w9)

- **fn_80025F84 / fn_80025A80 / fn_80024A2C** — `reg-alloc-permutation`
  - Symptom: target uses r28/r29/r30 where we use r29/r30/r31 (same count,
    shifted by 1).
  - Tried: declaration order reordering (does not affect CW allocation at
    -O4 for this fn).
  - Next leads: `-O2` + carefully-chosen variable order. Untested for these
    specific fns; this technique worked elsewhere (see
    `feedback_or_operand_and_optlevel.md`).
  - Last attempt: 2026-05-13 (w9)

## Matched
Run `tools/objdiff-cli.exe` for the up-to-date list.

## Session log

- **2026-05-13 (w9)** — explored all 12 near-misses, no progress. Identified
  4 distinct blocker classes (above). Source reverted to baseline.

- **2026-06-10** — re-measured + diffed every near-miss. New classifications:
  - **W6 name-only** (byte-neutral, not C-fixable): fn_8002092C / fn_800209BC /
    fn_80020A4C (99.86) all diff ONLY on `bl sin` vs `bl fn_800CE148` (the
    unnamed sin lib fn). Rename-infra only.
  - **anonymous-sda21** (extends the documented list): fn_80024CDC (99.29),
    fn_80024308 (99.28), fn_8002509C / fn_80024DBC (98.75), fn_80023E60 (93.88),
    fn_800255A4 (97.53) — all carry `@255@sda21`/`@257@sda21` magic-double pool
    refs vs target's named `lbl_8047B8xx`. Toolchain-locked.
  - **reg-permutation (W2)**: fn_80022834 (96.83, r24-r30 band shifted — confirmed
    the "structural look" yields no shape fix), fn_80025F84 (99.06, r29/r30/r31
    shifted), fn_80024160 (96.51), fn_800246FC (93.54, also stmw r25-vs-r27 W3).
  - **peephole-bne-b** (file is global `-opt nopeephole`): fn_8002509C /
    fn_80024DBC / fn_80024BA4 (91.41) / fn_80023E60 — target `bne @near; b @far`
    pairs we can't emit.
  - **instr-scheduling**: fn_800218BC (98.33) — SINGLE diff: the loop-counter
    `li r31,0x0` and the loop-invariant `lfs f31, lbl_8047B8A0@sda21` are swapped;
    CW's scheduler always hoists the `lfs` above the `li`. Tried: `i=0` before the
    `thresh` load (inert), `#pragma scheduling on` (inert, 98.33), `scheduling off`
    (regressed to 92.9 — reschedules the whole ctr loop). Hard scheduling wall.
  - fn_80025730 (52.42) — multi-wall (stmw r26 frame mismatch + control-flow
    divergence + reg perm); not a quick win.
  All experimental edits reverted to baseline; no commits.
