# gs_material.c decomp session log

## Session: May 17 2026

### Functions matched to 100%

#### fn_800E8FE8 (99.44% → 100%)
Branch condition inversion: `if (val & 3)` → `if (!(val & 3))`

#### fn_800E3B08 (99.23% → 100%)
sdata2 label names: replaced `gsMatPoolCount` → `lbl_8047AB78`, `gsMatPool` → `lbl_8047AB74` (raw pointer used as u32)

#### fn_800E8F74 (98.33% → 100%)
sdata2 label: replaced named `gsMatDistThresholdSq` → `lbl_8047AB88`

#### fn_800E4D3C (95.86% → 100%)
- sdata2 label: replaced named pool vars with `lbl_8047AB74`, `lbl_8047AB78`, `lbl_8047AB70`
- Extra `mr r3, r0`: cast `(u32)fn_800E27B0((u32)handle)` forces explicit register move

#### fn_800EC990 (89.47% → 100%)
CSE-defeating reload pattern: explicit `flags = *(u32*)p` after each store to force memory reloads that match target's load-all-then-store pattern

#### fn_800E3760 (99.93% → 100%)
Shadow check scope: moved `if (*(u32*)entry & 0x200000) fn_80190E60(...)` INSIDE the `if (r31 != NULL)` block so the null path skips it (matching target's beq to epilogue)

#### fn_800EC9DC (99.87% → 100%)
Control flow restructure: moved secondary flags checks (`if !(flags & 0x2000)`, `if !(flags & 0x8)`) INSIDE the outer `if (*(u32*)entry & 0x4)` block; target's first beq branches to epilogue when outer bit clear

#### fn_800E7290 (94.74% → 99.69%)
- Added `#pragma push; #pragma scheduling on` to enable CW instruction scheduling
- Used temp var `t` to force load-before-store ordering of the 0x38/0x3c pair
- Remaining 2 diffs: stw r3 vs stw r0 store-order (scheduler picks opposite order); stuck

#### fn_800E8EFC (91.85% → 94.07%)
- Changed loop variable `s32 i` → `u32 i` to generate `cmplwi` (unsigned) instead of `cmpwi` (signed)
- Remaining 3 diffs: prologue address computation `addi r30, r3, @l` vs `addi r0; mr r30, r0`; stuck

#### fn_800EA820 (98.18% → 98.63%)
- Swapped local declarations in inner block: `s32 r3 = 0; u32 f2 = ...` (r3 first, f2 second) to fix r3/r4 register swap
- Enlarged buf: `u8 buf[48]` instead of `u8 buf[12]` to match target's -0x50 frame
- Remaining 3 diffs: `beq X; bne Y` vs `bne Z` code ordering for null check; stuck

### Functions NOT changed (baseline maintained)
- fn_800E5550/638C/65CC/68D8 (98.92%): reg-alloc permutation in first loop (r29/r30 swap for count/counter); scheduling issue
- fn_800E5790 (91.02%): same reg-alloc + initialization order issue  
- fn_800E3604 (99.37%): reg-alloc permutation (r29/r30/r31 for slotMatch/offset/mobj-inner)
- fn_800E8FA0 (94.17%): peephole-bgtlr (`if (h > 0x1e0) return` → bgtlr vs ble+blr)
- fn_800ED4D4: anonymous-sda21 BLOCKED

### Anti-fraud verification
- `git diff --name-only`: only `src/game/gs_material.c`
- No `*_fn_*.inc` files modified
- No `#if 0` → `#if 1` flips

## Session: May 17 2026 (continued)

### Functions matched to 100%

#### fn_800E4AC0 (98.51% → 100%)
Two fixes:
1. Split combined `if (*(s32*)((u8*)entry + 0x114) != 0 || r5 == 0) return;` into two separate `if` statements.
2. Inverted condition: `if (r7 != 1)` → `if (r7 == 1)` to match target's `bne @fn_800E01D0; [fall-through fn_800E01F4]` pattern (r7==1 → fn_800E01F4 via fall-through; r7!=1 → fn_800E01D0 via taken branch).

#### fn_800EA820 (98.62% → 100%)
Null check control flow restructure:
- Changed `if (entry == NULL) goto _null_err; goto _skip_null_err;` to `if (entry == NULL) goto _end; if (entry != NULL) goto _skip_null_err;`.
- Moved `_end:` label to right BEFORE the tex-null check (not after all processing), so when entry==NULL execution bypasses only the `fn_8019D9DC` active-check block and still runs the tex check + fn_80197B6C calls.
- This generates the target's `beq @L_800EA8A4; bne @L_800EA878` pair with L_800EA8A4 at the tex-check entry.

### Functions improved (not yet 100%)

#### fn_800EAFE4 (95.81% → 96.02%)
Partial register allocation improvement:
- Removed separate `void* ret` and `void* xform` — merged into `texObj` (ret=fn_8019F01C() initially, then texObj in loop). This matches target's r24 reuse.
- Removed separate `u8* mtxPtr` — replaced with `texObj` reuse and `void* tex` block-local.
- Swapped inner block variable declarations `s32 check; u32 flags` (check before flags) to match target's `li r3, 0` ordering.
- Remaining 49 diffs: param register permutation (r25/r26/r27 for obj/dst/output) + other reg allocation; stuck.

#### fn_800E465C (88.96% → 89.16%)
- Changed Section 3 condition from `*(u32*)` to `*(s32*)` cast at `entry + 0x114` (all three sections now use signed compare).
- Remaining issues: frame size mismatch (0x30 vs target 0x20), extra callee-saved r29, many structural diffs.

### Functions NOT changed (baseline maintained)
- fn_800E7290 (99.69%): 2-diff stw store order — scheduling on + 0x4-before-0x0 source order reaches 99.69%; swapping to 0x0-before-0x4 makes worse; stuck.
- fn_800E3604 (99.37%): 10-diff reg-alloc permutation (r29/r30/r31 for slotMatch/offset/mobj); removing animFlag/envFlag/shadowFlag made worse; stuck.
- fn_800E8EFC (94.07%): prologue `addi r0; mr r30` vs `addi r30` — scheduling off made much worse; stuck.
- fn_800E8FA0 (94.17%): peephole-bgtlr BLOCKED.
- fn_800E5550/638C/65CC/68D8 (98.92%): reg-alloc permutation r29/r30 for count/i; declaration order changes don't help; stuck.

### Anti-fraud verification
- `git diff --name-only`: only `src/game/gs_material.c` and `docs/decomp_notes/src_game_gs_material.md`
- No `*_fn_*.inc` files modified
- No `#if 0` → `#if 1` flips

## Session: Jun 10 2026 (Fable)

### fn_800EB268 -- ConfigureAlpha (85.4% -> 100%)
Four structure-first fixes (commit 9dc9f513):
1. **Direct call** `fn_800ECA78(entry, lbl_8047CC28)` instead of the
   `((void(*)(void*,f32))fn_800ECA78)(...)` fnptr cast. The cast produced the
   indirect `lis/addi r12/mtctr/bctrl`; target wants a direct `bl`. (Sibling
   fn_800EB340 already called it directly — copy that.)
2. **Decl order**: declared the 0x144 saved value (r31) BEFORE the 0x8 obj ptr
   (r30) so value->r31, ptr->r30.
3. **Nested-assert idiom**: moved the 0x25d HSD-assert INSIDE the
   `if (r30 != NULL)` block as a redundant `if (r30 == NULL) assert();` (mirrors
   the fn_800E4598 crack 16ef3799). This produces target's shared beq/bne null
   test instead of a re-loaded `cmplwi; beq`.
4. **active-first**: declared `s32 active;` before re-reading `flags` in an inner
   block, flipping the r3<->r4 coloring (flags->r4, active->r3).

### Lever confirmed
- The "inlined fn_8019D620/fn_8019D9DC precondition idiom" (redundant nested
  null-assert inside the not-null guard) is a repeatable W1-wall cracker for the
  ConfigureAlpha/blend family. Same shape as fn_800E4598.

### Quadruplet still walled
- fn_800E5550/638C/65CC/68D8 (98.92%): re-confirmed W1 reg-alloc permutation —
  the FIRST loop swaps count(r30)/counter(r29) vs target (counter->r29,
  count->r30). Data-flow-locked: pointer-walk, decl reorder, dec-var split, and
  count-first all kept 98.92% or worsened (95.5-95.6%). The SECOND loop already
  matches with identical C. Skip.
