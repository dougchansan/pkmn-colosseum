# Few-Shot Examples: Successful 100% Matches

These are real examples from this project where the C code achieved 100%
byte-match against the original binary. Use these as reference patterns.

---

## Pattern 1: Simple Getter (global pointer → struct field)

**ASM (fn_800D3094):**
```
nofralloc
lwz r3, lbl_8047AA80(r13)
lwz r3, 0x4c(r3)
blr
```

**Matching C:**
```c
extern u32 lbl_8047AA80;
u32 fn_800D3094(void) {
    return *(u32*)((u8*)lbl_8047AA80 + 0x4C);
}
```

**Key lesson:** Global holds a pointer value as u32. Cast to `(u8*)` then add offset.

---

## Pattern 2: Simple Setter (arg → global pointer field)

**ASM (fn_800D30A0):**
```
nofralloc
lwz r4, lbl_8047AA80(r13)
stw r3, 0x48(r4)
blr
```

**Matching C:**
```c
extern u32 lbl_8047AA80;
void fn_800D30A0(u32 val) {
    *(u32*)((u8*)lbl_8047AA80 + 0x48) = val;
}
```

---

## Pattern 3: sdata2 Float Return (MUST use extern label)

**ASM (fn_801AE000):**
```
nofralloc
lfs f1, -22848(r2)
blr
```

**WRONG (50% match):**
```c
f32 fn_801AE000(void) { return 0.0f; }
```

**CORRECT (100% match):**
```c
extern f32 lbl_8047DD60;
f32 fn_801AE000(void) {
    return lbl_8047DD60;
}
```

**Key lesson:** Float literals create TU-local sdata2 entries with wrong relocations. ALWAYS use extern labels.

---

## Pattern 4: Dual Global Store (same value)

**ASM (fn_80163F88):**
```
nofralloc
li r0, 0x1
stw r0, lbl_8047B098(r13)
stw r0, lbl_8047B088(r13)
blr
```

**Matching C:**
```c
extern u32 lbl_8047B098;
extern u32 lbl_8047B088;
void fn_80163F88(void) {
    lbl_8047B098 = 1;
    lbl_8047B088 = 1;
}
```

**Key lesson:** No pragma needed. Default O4 reuses r0 for both stores. Do NOT add `#pragma optimization_level 0` even if the asm wrapper had it.

---

## Pattern 5: Output Parameters (two globals → two pointers)

**ASM (fn_800140FC):**
```
nofralloc
lwz r0, lbl_8047A2F8(r13)
stw r0, 0x0(r3)
lwz r0, lbl_8047A2F4(r13)
stw r0, 0x0(r4)
blr
```

**Matching C:**
```c
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2F4;
void fn_800140FC(u32* out1, u32* out2) {
    *out1 = lbl_8047A2F8;
    *out2 = lbl_8047A2F4;
}
```

---

## Pattern 6: Pass-Through Thunk (call another function)

**ASM (fn_801C6908):**
```
nofralloc
stwu r1, -0x10(r1)
mflr r0
stw r0, 0x14(r1)
bl fn_800E0C04
lwz r0, 0x14(r1)
mtlr r0
addi r1, r1, 0x10
blr
```

**Matching C:**
```c
extern void fn_800E0C04(void);
void fn_801C6908(void) {
    fn_800E0C04();
}
```

**Key lesson:** Thunks are trivial. The prologue/epilogue is just the call overhead. No pragmas needed.

---

## Pattern 7: Conditional Float Getter with Null Check

**ASM (fn_8018F5B4):**
```
nofralloc
cmplwi r3, 0x0
beq @null
lfs f1, 0x14(r3)
blr
@null:
lfs f1, -24056(r2)
blr
```

**Matching C:**
```c
extern f32 lbl_8047D8A8;
f32 fn_8018F5B4(u8* ptr) {
    if (ptr != NULL) {
        return *(f32*)(ptr + 0x14);
    }
    return lbl_8047D8A8;
}
```

**Key lesson:** Null check uses `cmplwi` (unsigned compare) which maps to pointer != NULL. The sdata2 float fallback MUST use an extern label.

---

## Pattern 8: Block-Scoped Double Global Load

**ASM (fn_80162858):**
```
nofralloc
mulli r6, r3, 0xf4
lwz r0, lbl_8047B024(r13)
add r3, r0, r6
stw r4, 0x94(r3)
lwz r0, lbl_8047B024(r13)    ← loads global TWICE
add r3, r0, r6
stw r5, 0x98(r3)
blr
```

**Matching C:**
```c
void fn_80162858(u32 index, u32 val1, u32 val2) {
    extern u32 lbl_8047B024;
    u32 offset = index * 0xF4;
    {
        u8* elem1 = (u8*)lbl_8047B024 + offset;
        *(u32*)(elem1 + 0x94) = val1;
    }
    {
        u8* elem2 = (u8*)lbl_8047B024 + offset;
        *(u32*)(elem2 + 0x98) = val2;
    }
}
```

**Key lesson:** When asm loads a global TWICE, CW is NOT CSE-ing across the store. Use separate `{ }` blocks to force separate loads. Without blocks, CW at O4 would CSE into one load → mismatch.

---

## Pattern 9: Aligned Size Subtraction

**ASM (fn_80163BCC):**
```
nofralloc
addi r3, r4, 0x1f
lwz r0, lbl_8047B078(r13)
clrrwi r3, r3, 5
subf r0, r3, r0
stw r0, lbl_8047B078(r13)
blr
```

**Matching C:**
```c
extern u32 lbl_8047B078;
void fn_80163BCC(u8* unused, u32 size) {
    lbl_8047B078 -= (size + 0x1F) & ~0x1F;
}
```

**Key lesson:** `clrrwi rD, rS, 5` = `& ~0x1F` (align to 32). `addi + clrrwi` = round up. `subf` = subtraction (dst = src2 - src1). Note first param is unused (r3 overwritten immediately).

---

## Anti-Patterns (Things That DON'T Match)

### DON'T use float literals for sdata2 returns
```c
// WRONG: return 0.0f;
// RIGHT: extern f32 lbl_XXXX; return lbl_XXXX;
```

### DON'T keep #pragma optimization_level 0 for leaf functions
```c
// The asm wrapper's pragma was for the assembler, not the C replacement.
// Leaf functions (nofralloc) usually need default O4.
```

### DON'T declare sdata symbols as arrays when they should be scalars
```c
// WRONG: extern u8 lbl_XXXX[];  → generates lis/addi (2 instructions)
// RIGHT: extern u32 lbl_XXXX;   → generates SDA-relative (1 instruction)
// Exception: large objects NOT in SDA range need array declaration
```

### DON'T use local variables at O0 (they spill to stack)
```c
// At #pragma optimization_level 0, locals go on stack.
// Use direct assignment instead of intermediary variables.
```
