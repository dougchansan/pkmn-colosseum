# fn_800DBBFC (gs_render.c, 61 asm lines)

## Status
- [x] GLM plan
- [ ] Codex implementation
- [ ] Claude review+merge

## Target asm
See `src/game/gs_render_fn_800DBBFC.inc`

## File target
`src/game/gs_render.c`

## Baseline
- Current: asm wrapper at 100% (target identical bytes, but wrapper, not real C)
- Target: ≥60% real C match

## GLM Plan (planner)
_GLM-5.1: read the .inc, identify args/return types, describe pseudocode, list externs needed. Note any quirks (switch, table-search, bit-ops, FP math, list walks, etc.)_

**Signature**: `void fn_800DBBFC(s32 r3_x, s32 r4_y, s32 r5_unk, s32 r6_unk2, s32 r7_unk3, s32 r8_unk4, s32 r9_unk5, s32 r10_unk6, s32 vararg_unk7, s32 vararg_unk8)` — Takes many args (up to 10). Conditionally dispatches to either a render-path function (fn_800D4F98) with 10 args, or fills a struct directly.

**Pseudocode**:
```c
// This is a render-element submission function. It checks a global mode flag.
// If mode == 1 (immediate render), it dispatches to fn_800D4F98 with all args.
// If mode != 1 (deferred/batch), it fills a struct in the lbl_80400B28 array.

void fn_800DBBFC(s32 x, s32 y, s32 r5, s32 r6, s32 r7, s32 r8, s32 r9, s32 r10, ...) {
    // varargs: r11 from sp+0x3C, r12 from sp+0x38 (on stack)
    // Additional args passed on stack
    
    s32 saved_r5 = r5;  // r28
    s32 saved_r6 = r6;  // r29
    s32 saved_r7 = r7;  // r30
    s32 saved_r8 = r8;  // r31
    s32 r11 = sp[0x3C]; // from stack
    s32 r12 = sp[0x38]; // from stack
    
    s32* global_ptr = lbl_8047AA80;  // SDA global
    s32 mode = global_ptr[0];
    
    if (mode == 1) {
        // Immediate mode: dispatch to render function
        // Note: args are rearranged — some 16-bit truncated
        fn_800D4F98(0x54,  // type constant
                     0xA,   // sub-type
                     x, y,
                     (u16)saved_r5, (u16)saved_r6,
                     (u16)saved_r7, (u16)saved_r8,
                     ... // remaining args stored on stack
        );
    } else {
        // Deferred mode: fill an entry in the batch array
        // Array base at lbl_80400B28, each entry is 0x14 bytes
        // Entry struct: { u8 flags; u8 field_1; u8 field_2; u8 field_3;
        //                 u8 field_4; u8 field_5; pad; s16 x; s16 y;
        //                 s16 hw5; s16 hw6; ... }
        
        u8* base = lbl_80400B28;
        
        // Mark entry as active
        base[x + 0x1fc] = 1;
        
        // Calculate entry pointer
        u8* entry = base + (x * 0x14);
        entry[0x20C] = 2;  // stbu r0, 0x20c(r3) — pre-increment store
        
        entry[1] = (u8)y;        // stb r4
        entry[0xC] = (s16)saved_r5;  // sth r28
        entry[0xE] = (s16)saved_r6;  // sth r29
        entry[0x10] = (s16)saved_r7; // sth r30
        entry[0x12] = (s16)saved_r8; // sth r31
        entry[3] = (u8)r9;
        entry[2] = (u8)r10;
        entry[4] = (u8)r12;      // from stack
        entry[5] = (u8)r11;      // from stack
    }
}
```

**Externs needed**: `fn_800D4F98`, `lbl_8047AA80` (SDA global), `lbl_80400B28` (batch array base)

**Quirks**:
- **Many arguments** (10+). PPC calling convention: r3-r10 for first 8 GPR args, rest on stack. `sp+0x38` and `sp+0x3C` are additional stack args.
- `lwz r12, 0x38(r1)` and `lwz r11, 0x3c(r1)` — reading args passed on the stack.
- In immediate mode (mode==1): `clrlwi` truncates r28-r31 to 16-bit (u16) before passing to fn_800D4F98.
- In deferred mode: stores bytes and halfwords into a struct array with stride 0x14.
- `stbu r0, 0x20c(r3)` is a store-byte-update (pre-increment). The `0x20C` offset likely sets a "count" or "valid" flag.
- `crxor 6, 6, 6` before the call clears CR field 6 (sets CR6[EQ]=0), which is a MWCC convention for variadic argument count signaling.
- **Implementation challenge**: The many-arg calling convention and stack arg reads require careful handling. May need to define the function with explicit stack-frame manipulation or use va_list.

## Codex Input (implementer, iteration 1)
_Codex: implement the C based on GLM's plan. Wrap with `#pragma push / peephole off / pragma pop`. Run `python3 tools/compile_check.py src/game/gs_render.c` then `python3 tools/match_scan_file.py gs_render fn_800DBBFC`. If <60%, try the pragma variants (scheduling off), reorder locals, or use pointer-walk patterns. Commit when ≥60% OR after 5 attempts. Regression-check 3 already-matched functions in gs_render.c._

(fill in)

## Claude Review (merger)
_Claude: verify compile + match, check no regression, commit to master with message `gs_render: fn_800DBBFC 0%->X.X% via <approach>`, move this file to completed/._

(fill in)
