# decomp notes: HSD asm-wrapper SDA21 relocation loss (cobj/dobj/mobj/fog/lobj)

## 2026-06-10: the "99.x wrapper near-miss" class is mostly an UNFIXABLE SDA21 reloc loss, NOT a typed-extern win

### Summary

A large set of `hsd_*` functions are **asm-wrapper-active** (`#if 1` around
`asm void fn_X(void) { #include "...inc" }`) and measure 92-99.x% via objdiff.
The prompt-level hypothesis was that these are "reloc-name artifacts from untyped
externs" fixable by retyping the local `extern void fn(void)` decls (which worked
on `fn_801A0FBC` in hsd_jobj, commit 8297e45f).

**That lever does NOT generalize to these functions.** Investigated the actual
objdiff instruction diffs (read-only) and found the residual mismatch in every
case is a **lost SDA21 relocation**, e.g.:

| target (left, real ROM)          | our wrapper output (right) |
|----------------------------------|----------------------------|
| `li r3, lbl_8047DA74@sda21`       | `addi r3, r2, 0x0`         |
| `li r5, lbl_8047DBC8@sda21`       | `addi r5, r2, 0x0`         |
| `li r3, lbl_8047D958@sda21`       | `addi r3, r2, 0x5d48` (raw)|
| `lis r4, lbl_8036C678@ha` + `addi`| `lis r4, 0x8037` + `subi`  |

The `.inc` files contain **raw numeric operands** (`addi r3, r2, -23880`,
`la r5, lbl_8047DBC0(r2)` that resolves to offset 0) that the CW inline assembler
emits literally. An asm wrapper cannot synthesize an `@sda21` relocation from a
raw `addi rN, r2/r13, imm`. **Even when the SDA label IS declared as an extern in
scope** (verified: `lbl_8047DBC0[]`, `lbl_8047DBC4[]`, etc. are declared at the
top of hsd_lobj.c), the wrapper still emits `addi r5, r2, 0x0`. Typed externs
only help wrappers whose `.inc` uses a SYMBOLIC operand whose reloc depends on the
decl (e.g. `bl fn_X`) — the fn_801A0FBC case. SDA `la sym(r2)` / raw-imm cases are
immune.

### Conclusion: the wrapper is the BEST AVAILABLE for these functions

The ONLY path to 100% is full C reimplementation that references the SDA symbols
symbolically (which DOES generate `@sda21`). But these functions also contain
C-unmatchable constructs, so C regresses far below the wrapper. Do NOT flip these
to C, and do NOT re-chase them with typed externs.

### Measured baselines (objdiff calculatePoolRelocations=false, 2026-06-10)

Wrapper-active, SDA-loss class (leave as wrappers — these are best available):

| fn          | file     | wrapper % | residual               |
|-------------|----------|-----------|------------------------|
| fn_80194C2C | hsd_cobj | 84.2*     | lbl_80478AC8 reloc; C=60 (opt0 param-spill) |
| fn_80193D30 | hsd_cobj | 94.1      | lbl_8047D958/60, lbl_8036C678 + cntlzw bool idiom |
| fn_801A4344 | hsd_lobj | 96.8      | lbl_8047DBC0/C4 SDA; C reconstruction = 55 (branch layout + r29/30/31 alloc) |
| fn_801A4440 | hsd_lobj | 99.8      | single lbl_8047DBC8 SDA loss |
| fn_801A4D20 | hsd_lobj | 98.6      | SDA loss |
| fn_8019BD18 | hsd_fog  | 99.7      | lbl_8047DA74 SDA + psq_st/psq_l (HARD-STOP), fctiwz magic, memset |
| fn_8019BB78 | hsd_fog  | 92.3      | lbl_8047DA74/7C/80 SDA loss (assert strings) |
| fn_801A7128 | hsd_mobj | 99.9      | lbl_80478C88 SDA (r13-relative) loss |

(*) `match_scan_file.py` scores fn_80194C2C at 84.2 because it counts the lost-reloc
`lis 0x8048` immediate as a full mismatch; the objdiff per-instruction view rates
the same wrapper ~99.2 (relocation immediate scored partially). Both agree the C
path (60%) is strictly worse — `optimization_level 0` spills the two params
(vec/p) live across the trailing `fn_800A3ADC` call into r31/r30, whereas the
target keeps them in r3/r4 and uses r5 for the epsilon address. Not C-controllable
across opt0-4; volatile/const-f32/negated-or forms all produce the same spill.

### Filed wall class

W-SDA-WRAPPER: asm-wrapper near-miss whose only residual is a lost `@sda21` (or
`@ha/@l`) relocation that the `.inc` baked into a raw `addi rN, r2/r13, imm`.
Unfixable in the wrapper; C reimplementation regresses. FAST-SKIP.

### Tooling note

`/tmp/idiff.py <stem> <fn>` (this session) prints left-vs-right instruction lists.
Filter real mismatches with:
`grep "<<<" | grep -viE "\| (b|bne|beq|bge|ble|bgt|blt|bdnz|bdz) 0x"`
because branch-target lines show absolute-vs-relative address rendering that
objdiff scores as MATCHING (not real diffs). Only non-branch `<<<` lines are real.
