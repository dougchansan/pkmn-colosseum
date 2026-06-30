# fn_80025A80 — Corrected Semantic C

Module: `src/game/gs_title.c`
Size: 0x19C bytes
Match: 3.9% (current C uses pseudo-registers)
Target: CodeWarrior 1.3 -O4,p

---

## Required Pragmas

```c
#pragma optimization_level 4
#pragma scheduling off
```

## Control Flow

1. **Graphics setup**: call fn_800D9B58(f1=lbl_8047B8AC, f2=lbl_8047B8AC, f3=lbl_8047B8F8, f4=lbl_8047B8FC); then series of viewport/scissor calls (fn_800DA4C4, fn_800DA2BC, fn_800DA1E8, fn_800DA100, fn_800DA028, fn_800D9ED8)
2. **First timer update**: store {lbl_8047B8F0, lbl_8047B8F4, lbl_8047B8AC} to sp+8/0xC/0x10; call fn_80025C1C(r31, 1, 0, sp+8, f1=lbl_8047A3A0, f2=lbl_8047B8B0, f3=lbl_8047B904)
3. **Clamp timer A**: lbl_8047A3A0 -= lbl_8047B908; if < lbl_8047B8AC → lbl_8047A3A0 = lbl_8047B8AC
4. **Second timer update**: compute vec from lbl_803A204C + offsets; store {lbl_8047B8F8-vec.x, lbl_8047B8FC-vec.y, lbl_8047B8AC} to sp+8/0xC/0x10; call fn_80025C1C(lbl_8047A388, 1, 1, sp+8, f1=lbl_8047A3A4, f2=lbl_8047B8B0, f3=lbl_8047B8E4)
5. **Clamp timer B**: lbl_8047A3A4 -= lbl_8047B8C8; if < lbl_8047B8AC → lbl_8047A3A4 = lbl_8047B8AC
6. **Return**: if both lbl_8047A3A4 <= lbl_8047B8AC AND lbl_8047A3A0 <= lbl_8047B8AC → return 0 (both done); else return 1 (still animating)

---

## Final C Body

```c
#pragma optimization_level 4
#pragma scheduling off
s32 fn_80025A80(u8* arg0) {
    extern u8 lbl_803A204C[];
    extern u32 lbl_8047A388;
    extern f32 lbl_8047A3A0;
    extern f32 lbl_8047A3A4;
    extern f32 lbl_8047B8AC;
    extern f32 lbl_8047B8B0;
    extern f32 lbl_8047B8C8;
    extern f32 lbl_8047B8E0;
    extern f32 lbl_8047B8E4;
    extern f32 lbl_8047B8F0;
    extern f32 lbl_8047B8F4;
    extern f32 lbl_8047B8F8;
    extern f32 lbl_8047B8FC;
    extern f32 lbl_8047B904;
    extern f32 lbl_8047B908;
    extern void fn_800D9B58(f32, f32, f32, f32);
    extern void fn_800D9ED8(s32);
    extern void fn_800DA028(s32);
    extern void fn_800DA100(s32, s32, s32, s32, s32, s32);
    extern void fn_800DA1E8(s32, s32, s32);
    extern void fn_800DA2BC(s32, s32, s32);
    extern void fn_800DA4C4(s32, s32, s32);
    extern void fn_80025C1C(void*, s32, s32, void*, f32, f32, f32);
    f32 sp[6];

    fn_800D9B58(lbl_8047B8AC, lbl_8047B8AC, lbl_8047B8F8, lbl_8047B8FC);
    fn_800DA4C4(1, 6, 7);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA100(0, 7, 0, 1, 7, 0);
    fn_800DA028(0);
    fn_800D9ED8(1);

    sp[0] = lbl_8047B8F0;
    sp[1] = lbl_8047B8F4;
    sp[2] = lbl_8047B8AC;
    fn_80025C1C(arg0, 1, 0, &sp[0], lbl_8047A3A0, lbl_8047B8B0, lbl_8047B904);

    lbl_8047A3A0 -= lbl_8047B908;
    if (lbl_8047A3A0 < lbl_8047B8AC) {
        lbl_8047A3A0 = lbl_8047B8AC;
    }

    sp[0] = lbl_8047B8F8 - (lbl_8047B8E0 + *(f32*)&lbl_803A204C[0]);
    sp[1] = lbl_8047B8FC - (lbl_8047B8E0 + *(f32*)&lbl_803A204C[4]);
    sp[2] = lbl_8047B8AC;
    fn_80025C1C(*(void**)&lbl_8047A388, 1, 1, &sp[0], lbl_8047A3A4, lbl_8047B8B0, lbl_8047B8E4);

    lbl_8047A3A4 -= lbl_8047B8C8;
    if (lbl_8047A3A4 < lbl_8047B8AC) {
        lbl_8047A3A4 = lbl_8047B8AC;
    }

    if (lbl_8047A3A4 <= lbl_8047B8AC && lbl_8047A3A0 <= lbl_8047B8AC) {
        return 0;
    }
    return 1;
}
```

---

## ASM→C Notes

- **`cror eq, lt, eq`**: this PPC CR logical OR combines `lt` and `eq` into `eq`, implementing `f0 <= f1`. After `fcmpo cr0, f0, f1`, the `lt` bit is set if f0<f1 and `eq` if f0==f1. `cror eq, lt, eq` makes `eq = lt|eq`, so `beq` branches when `f0 <= f1`. The `bne` after it means "branch if NOT (f0 <= f1)" = "if f0 > f1"
- **Return logic**: two `fcmpo+cror+bne` checks — if EITHER timer > lbl_8047B8AC (i.e., not yet clamped), skip to return 1. Only when BOTH are <= lbl_8047B8AC does it return 0
- **Two fn_80025C1C calls**: first uses `arg0` (r31=r3), second uses `lbl_8047A388` (SDA global, dereferenced as pointer via `lwz r3, lbl_8047A388@sda21(r0)`). The first arg `*(void**)&lbl_8047A388` matches `lwz r3, lbl_8047A388@sda21` — SDA loads a u32 that's then used as a pointer
- **lbl_803A204C**: Accessed via `lis+addi` (non-SDA), read as two floats at offset 0 and 4
- **Stack layout**: sp[0]=8(r1), sp[1]=0xC(r1), sp[2]=0x10(r1) — each f32 takes 4 bytes. The `addi r6, r1, 0x8` passes pointer to sp[0] as r6 (4th GPR arg)
- **fn_80025C1C signatures**: declared as `void fn_80025C1C(void)` in the module but actually takes (void*, s32, s32, void*, f32, f32, f32). The void declaration is a placeholder — CW -O4,p passes FPRs f1-f3 directly matching the asm