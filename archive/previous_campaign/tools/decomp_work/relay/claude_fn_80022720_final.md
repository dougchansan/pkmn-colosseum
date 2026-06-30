# fn_80022720 — Final Review

Target: `src/game/gs_title.c` line 3491-3523 (replaces the active `#else` body). Size 0x114, 10.1% matched today.

## Final C (ready to paste)

Leave the file-level externs at lines 3492-3494 unchanged. Replace the `#else` body:

```c
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80022720(u32 arg0, u32* arg1) {
    struct CamEntry {
        u16 id;
        u16 pad;
        u32 msg;
    } entries[5];
    s32 idx;

    memcpy(entries, lbl_80266C54, sizeof(entries));
    if (arg0 == entries[0].id) {
        idx = 0;
    } else if (arg0 == entries[1].id) {
        idx = 1;
    } else if (arg0 == entries[2].id) {
        idx = 2;
    } else if (arg0 == entries[3].id) {
        idx = 3;
    } else if (arg0 == entries[4].id) {
        idx = 4;
    } else {
        idx = 5;
    }

    fn_80106D3C(2, entries[idx].msg, 1, 0);
    fn_801069FC(1);
    *arg1 = 0;
    return 0;
}
```

## What changed from the current attempt

1. **Return type fixed: `void` → `s32`, with `return 0;`**. The asm emits `li r3, 0x0` right before the epilogue (after `li r0, 0x0` for the store). A `void` function would not emit the `li r3, 0x0` return-value setup. This alone explains a lot of the 89.9% diff.
2. **Removed register-name variables (`r31`, `r30`, `r29`)**. arg1 is used directly; there is no `r30` — the table base lives only in the memcpy. `r29` becomes the semantic `idx`. These aliases forced extra mr/stw ops that the asm does not have.
3. **Replaced per-word u32 copy with `memcpy(entries, lbl_80266C54, 40)`**. The asm does ten `lwz` into r12/r11/r10/r9/r8/r7/r6/r5/r4/r0 and then ten `stw`, in that bulk-load-then-bulk-store shape. CW 1.3's inliner produces exactly that for a constant-size small `memcpy`. The previous `buf[0] = *(u32*)(r30+0x0); buf[1] = ...;` chain emits interleaved lwz/stw pairs, which mismatches.
4. **Introduced `struct CamEntry { u16 id; u16 pad; u32 msg; }` (8-byte stride)**. The asm compares halfwords at `buf+0, buf+8, buf+16, buf+24, buf+32` (stride 8) and later reads msg words at `buf+4, buf+12, buf+20, buf+28, buf+36, buf+44` via `addi r3, r1, 0xc; slwi r0, idx, 3; lwzx r4, r3, r0`. That indexing shape is exactly `entries[idx].msg` for an 8-byte struct whose `msg` is at offset 4.
5. **Added `#pragma scheduling on`** (matched-function default for this file). The current attempt only sets `optimization_level 4`.
6. **Removed casts like `(u32)*(u16*)((u8*)buf + 0x00)` and `(s32)*(u32*)((u8*)buf + r29*8 + 4)`**. These force CW into address-arithmetic-through-u8* expressions instead of clean struct access; CW -O4 generates different intermediate temps for the two forms.
7. **Signature: `(u32 arg0, u32* arg1)` instead of `(u32 r3, u32* r4)`**. Purely cosmetic, but aligns with the rest of the file (see `fn_80022EE4`) and matches the naming the prompt asks for.

Intentional OOB access: when no entry matches, `idx = 5` and the final call reads `entries[5].msg` which is past the end of the 40-byte buffer. The asm does the same thing — it reads `r1+0x34`, which is the saved-r29 slot. This is UB in C but CW compiles it as plain pointer arithmetic without bounds checks, so leaving the C as written is correct for matching. Do **not** guard the call with `if (idx < 5)`; the asm has no such branch.

## Expected match risk (high/medium/low)

**Low.**

- Every asm quirk maps to one specific C choice above (bulk memcpy, 8-byte struct stride, `lwzx` via struct member, `s32` return with `li r3, 0x0`).
- No floating-point, no slot-lock, no shared-memory reload pattern — nothing that scheduling on can break.
- Matched sibling `fn_80023068` in the same file uses the same if/else if idx-search chain shape.

Residual risks, ranked:
1. **`else if` chain codegen** (low). The asm pre-sets `r29` then tests; CW -O4 may emit either pre-set-then-test or test-then-set from the if/else if chain. If the diff lands in the compare chain specifically, restructure as a linearized sequence:
   ```c
   idx = 0; if (arg0 == entries[0].id) goto done;
   idx = 1; if (arg0 == entries[1].id) goto done;
   idx = 2; if (arg0 == entries[2].id) goto done;
   idx = 3; if (arg0 == entries[3].id) goto done;
   idx = 4; if (arg0 == entries[4].id) goto done;
   idx = 5;
   done:
   ```
   Ugly but faithful.
2. **Struct padding visibility** (low). CW 1.3 lays out `{u16; u16; u32}` as a flat 8-byte struct with no trailing pad. If a warning or layout mismatch appears, drop the struct and use `u32 entries[10]` + `((u16*)entries)[idx*4]` / `entries[idx*2 + 1]` — same codegen.
3. **`memcpy` not inlined** (very low). Already used elsewhere in the file (line 4056) with the same idiom on this exact region (`lbl_80266DB0`). No reason 40 bytes from `lbl_80266C54` would be treated differently.

If this mismatches, first diff location identifies which of the three applies.
