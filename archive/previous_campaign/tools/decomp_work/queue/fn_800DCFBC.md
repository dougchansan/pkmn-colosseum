# fn_800DCFBC (gs_render.c, 70 asm lines)

## Status
- [x] GLM plan
- [ ] Codex implementation
- [ ] Claude review+merge

## Target asm
See `src/game/gs_render_fn_800DCFBC.inc`

## File target
`src/game/gs_render.c`

## Baseline
- Current: asm wrapper at 100% (target identical bytes, but wrapper, not real C)
- Target: ≥60% real C match

## GLM Plan (planner)
_GLM-5.1: read the .inc, identify args/return types, describe pseudocode, list externs needed. Note any quirks (switch, table-search, bit-ops, FP math, list walks, etc.)_

**Signature**: `s32 fn_800DCFBC(void)` — no args. Pool slot allocator that walks a fixed array to find a free entry, then initializes it as a render object with sub-structures.

**Pseudocode**:
```c
s32 fn_800DCFBC(void) {
    // SDA globals:
    //   lbl_8047AAF0(r13) = count/max slots
    //   lbl_8047AAEC(r13) = pool array base (stride 0x74)
    
    s32 count = lbl_8047AAF0;
    u8* ptr = lbl_8047AAEC;
    
    // Walk pool to find free slot (byte at offset 0 == 0)
    if (count > 0) {
        do {
            if (ptr[0] == 0) goto found;  // free slot
            ptr += 0x74;  // next slot
        } while (--count > 0);
    }
    return 0;  // no free slot
    
found:
    // Initialize the slot (stride 0x74 structure)
    ptr[0x10] = 0;            // clear field
    f32 f1_zero = lbl_8047CA78; // 0.0f
    f32 f0_one  = lbl_8047CA8C; // 1.0f (or some constant)
    
    *(f32*)(ptr + 0x14) = f1_zero;
    *(f32*)(ptr + 0x18) = f1_zero;
    *(f32*)(ptr + 0x1C) = f1_zero;
    *(s32*)(ptr + 0x20) = 0;
    *(s32*)(ptr + 0x24) = 0;
    *(f32*)(ptr + 0x28) = f1_zero;
    *(f32*)(ptr + 0x2C) = f1_zero;
    *(f32*)(ptr + 0x30) = f1_zero;
    *(s32*)(ptr + 0x34) = 0;
    *(f32*)(ptr + 0x54) = f0_one;   // offset 0x54 = 1.0f
    *(s32*)(ptr + 0x38) = 0;
    *(s32*)(ptr + 0x3C) = 0;
    *(s16*)(ptr + 0x40) = 4;       // sth r7 (r7=4)
    *(s16*)(ptr + 0x42) = 0;       // sth r8 (r8=0)
    ptr[0x44] = 0x80;              // stb r6 (r6=0x80)
    ptr[0x45] = 0x80;
    ptr[0x46] = 0x80;
    ptr[0x47] = 0;                 // stb r8
    
    // Self-referential pointers for sub-structures
    *(s32*)(ptr + 0x48) = (s32)(ptr + 0x10);  // points to offset 0x10
    *(s32*)(ptr + 0x4C) = (s32)(ptr + 0x24);  // points to offset 0x24
    *(s32*)(ptr + 0x50) = (s32)(ptr + 0x54);  // points to offset 0x54
    
    // Create/initialize render sub-object
    s32 sub_obj = fn_801A4344();  // called with r3=ptr, r4=ptr+0x24, r5=ptr+0x10, r6=0x80, r7=4, r8=0
    ptr[0x0C] = sub_obj;  // wait, actually it's stw r3, 0xc(r31)
    
    // Mark as active
    ptr[0] = 1;     // stb r4 (r4=1)
    ptr[1] = 0;     // stb r0 (r0=0)
    
    return (s32)ptr;  // actually, function doesn't return ptr explicitly...
    // Actually the epilogue just restores and returns. The return value
    // is likely the ptr in r3 (set by fn_801A4344 return via stw r3, 0xc)
    // Wait — looking at the end, it falls through to epilogue with r31 intact.
    // r31 is the found slot pointer. But the function stores the return of 
    // fn_801A4344 at offset 0xC, and doesn't explicitly set r3.
    // Actually there's no `mr r3, r31` at the end — so r3 still holds
    // whatever fn_801A4344 returned. The slot pointer is r31.
    // This function likely returns void (or the fn_801A4344 result).
}
```

**Externs needed**: `fn_801A4344`, `lbl_8047AAF0` (SDA global), `lbl_8047AAEC` (SDA global), `lbl_8047CA78` (R2 constant, 0.0f), `lbl_8047CA8C` (R2 constant, likely 1.0f)

**Quirks**:
- **Pool allocator pattern**: Walk array with stride 0x74 (116 bytes per entry), looking for ptr[0] == 0.
- **bdnz loop**: Uses `mtctr` + `bdnz` pattern for the free-slot search.
- **Self-referential pointers**: Stores `(ptr + offset)` into the struct itself — forms linked list or tree structure.
- **Mixed field initialization**: Floats, shorts, bytes, words — many individual stores. Use explicit `(f32*)`, `(s16*)`, `(s32*)` casts.
- `stb r6, 0x44/0x45/0x46/0x47` — r6=0x80 for first 3 bytes, r8=0 for 4th. These are color bytes (0x80 = 128, mid-gray in RGBA).
- **No explicit return value**: End of function doesn't set r3, so this may return whatever was left in r3 from fn_801A4344, or it may be void.
- Use `#pragma push / peephole off / pragma pop`.

## Codex Input (implementer, iteration 1)
_Codex: implement the C based on GLM's plan. Wrap with `#pragma push / peephole off / pragma pop`. Run `python3 tools/compile_check.py src/game/gs_render.c` then `python3 tools/match_scan_file.py gs_render fn_800DCFBC`. If <60%, try the pragma variants (scheduling off), reorder locals, or use pointer-walk patterns. Commit when ≥60% OR after 5 attempts. Regression-check 3 already-matched functions in gs_render.c._

(fill in)

## Claude Review (merger)
_Claude: verify compile + match, check no regression, commit to master with message `gs_render: fn_800DCFBC 0%->X.X% via <approach>`, move this file to completed/._

(fill in)
