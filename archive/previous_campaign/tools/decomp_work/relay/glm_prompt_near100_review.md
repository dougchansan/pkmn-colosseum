# Task: Close the near-100 gaps in gs_title.c

## Context
We (Claude) have decomped Pokemon Colosseum `src/game/gs_title.c` to an average
of **92.23% match** across 15 previously-unmatched functions via a Ghidra pipeline.
Full session log + learnings in `tools/decomp_work/CLAUDE.md`.

The file compiles with CW 1.3 `-opt nopeephole` + per-function pragmas
(see `config/GC6E01/compile_config.json` overrides).

## Three functions stuck near 100% -- your task

Pick ONE and try to push to 100%. These are the specific diffs we cannot
crack. Do not regress below the current baseline.

### A. fn_80024CDC (99.3%, 8 diffs)
**Source:** `src/game/gs_title.c` around line 730 (void fn_80024CDC(s32, u8*))
**Target asm:** `src/game/gs_title_fn_80024CDC.inc`
**Ghidra:** `tools/decomp_work/ghidra_out/fn_80024CDC_ghidra.c`

**Remaining diffs (all DIFF_ARG_MISMATCH):**
```
 23  lfd f2, lbl_8047B8D0@sda21   | lfd f2, @271@sda21
 25  lfd f1, lbl_8047B8B8@sda21   | lfd f1, @273@sda21
 36  lwz r31, 0x1c(r1)            | lwz r0, 0x1c(r1)
 37  cmpwi r31, 0x40              | cmpwi r0, 0x40
 40  li r31, 0x40                 | li r0, 0x40
 43  cmpwi r31, 0xff              | cmpwi r0, 0xff
 46  li r31, 0xff                 | li r0, 0xff
 48  clrlwi r0, r31, 24           | clrlwi r0, r0, 24
```

**Our hypothesis:** Target uses r31 for iVar4 (non-volatile) by reusing r31
across the bVar1 lifetime. We use r0 (volatile) because CW allocator picks
it for short-lived values. The @271/@273 are anonymous f64 constants CW
generated for the int-to-float conversion magic; target uses the named
externs lbl_8047B8D0 / lbl_8047B8B8.

**Things we tried that did NOT help:**
- Merging bVar1 and iVar4 into single variable `val`
- Type swaps (int vs s32, unsigned char vs u8)
- `register int iVar4`, `do { ... } while (0)` wrapper
- Explicit union with (u32 hi,lo) + bias subtraction (regressed to 92.9%)
- `#pragma force_active on`
- All CW versions 1.3 through 2.7 produce identical output

### B. fn_8002537C (98.4%, 2 diffs)
**Source:** `src/game/gs_title.c` around line 1050
**Remaining diffs:**
```
 42  bne 0x1fe4c  | beq 0x1080
 43  b 0x1fe70    | ---
```

**Our hypothesis:** Target emits `cmpw; bne skip; b found; skip: addi` for the
`if (uVar3 == uVar4) goto LAB_80025450;` pattern. Our compile emits `cmpw;
beq LAB; addi` directly. Both are semantically identical but byte-different.

**Things we tried that did NOT help:**
- Explicit goto pair: `if (uVar3 != uVar4) goto skip; goto LAB; skip: ...`
- if/else inversion: `if (!= ) {counter++;} else {goto LAB;}`
- `#pragma peephole on` (regresses to 83.8%)
- Wrapping in labeled statements

### C. fn_80025490 (98.4%, 2 diffs)
Exact twin of fn_8002537C. Same bne+b vs beq pattern.

## How to verify

```bash
cd /c/Users/douglaswhittingham/pkmn-colosseum
python3 tools/compile_check.py src/game/gs_title.c    # must print OK
python3 tools/match_scan.py fn_80024CDC               # print match %
# Full diff:
./tools/objdiff-cli.exe diff \
  -1 build/GC6E01/obj/auto_01_800055E0_text.o \
  -2 build/GC6E01/base/game/gs_title.o \
  -o - --format json \
  -c ppc.calculatePoolRelocations=false \
  fn_80024CDC
```

## Constraints
1. **No regressions** on any currently-matched function (run full
   `python3 tools/match_scan.py fn_80024CDC fn_80024DBC fn_80024A2C
   fn_80024BA4 fn_80024F2C fn_8002509C fn_8002520C fn_8002537C fn_80025490
   fn_800255A4 fn_80025A80 fn_80025C1C fn_80025F84 fn_80022720 fn_800246FC`
   before and after).
2. **Compile must stay green** — no removing pragmas or flags.
3. Write your proposed rewrite to `tools/decomp_work/relay/glm_fnXXX_v2_result.md`.
4. Do not touch any other function — pick ONE target (A, B, or C).

Report: final match %, asm diff of what changed, and 1-sentence on why you
think CW allocated differently.
