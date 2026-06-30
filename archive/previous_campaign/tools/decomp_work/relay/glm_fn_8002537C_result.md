# fn_8002537C / fn_80025490 — Corrected Semantic C

Module: `src/game/gs_title.c`
Size: 0x114 bytes each
Match: ~23% (current C uses pseudo-registers, not proper typed C)

---

## Signature + Control Flow

```c
void fn_8002537C(u8* arg0, u8* arg1);
```

- r3 = arg0 (structure with u32 at +4), r4 = arg1 (structure, writes u16 at +0x50 and +0x52)
- r29 starts 0; if lbl_8047A370==1: call fn_800E0CA0(lbl_80478898), then fn_800E090C(&sp+8, lbl_803A2058, lbl_803A204C), r29 = (s32)*(f32*)(sp+8), goto epilog
- Else (lbl_8047A370 != 1): if active_index (lbl_8047A368) < *(u32*)lbl_80478DD8: walk linked list from fn_8005DA18(arg0[4])→fn_8005D934(node[4]), counting bit7 entries; when count==active_index, save node as r3; if r3!=NULL, r29 = *(s16*)(r3+2)
- Epilog: *(s16*)(arg1+0x50) = (s16)(r29+0xf); *(s16*)(arg1+0x52) = 0

---

## Key Differences Between Siblings

| Aspect | fn_8002537C | fn_80025490 |
|--------|-------------|-------------|
| `lfs f0` stack offset | sp+0x8 | sp+0xC |
| Node field for r29 | `lha r29, 0x2(r3)` | `lha r29, 0x4(r3)` |
| Store at +0x50 | `(s16)(r29+0xf)` | `0` |
| Store at +0x52 | `0` | `(s16)(r29+0xf)` |

---

## Final C for fn_8002537C

```c
#pragma scheduling on
#pragma optimization_level 4
void fn_8002537C(u8* arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
    extern f32 fn_800E0CA0(f32);
    u8* node;
    s32 count;
    s32 val;

    val = 0;
    if ((s32)lbl_8047A370 == 1) {
        fn_800E0CA0(lbl_80478898);
        fn_800E090C((void*)((u8*)&val - 0x18), (void*)lbl_803A2058, (void*)lbl_803A204C);
        val = (s32)*(f32*)((u8*)&val - 0x18);
    } else {
        if ((u32)lbl_8047A368 < *(u32*)lbl_80478DD8) {
            node = fn_8005DA18(*(u32*)(arg0 + 4));
            node = fn_8005D934(*(s16*)(node + 4));
            count = 0;
            for (;;) {
                if (*(s8*)(node + 0) < 0) {
                    if ((s32)lbl_8047A368 == count)
                        break;
                    count++;
                }
                if (*(s8*)(node + 0) >= 0) {
                    node = fn_8005D934(*(s16*)(node + 0x18));
                } else {
                    node = NULL;
                    break;
                }
            }
            if (node != NULL) {
                val = *(s16*)(node + 2);
            }
        }
    }
    *(s16*)(arg1 + 0x50) = (s16)(val + 0xf);
    *(s16*)(arg1 + 0x52) = 0;
}
```

**Critical notes for byte-matching:**
- `extrwi r0, r0, 1, 24` = test bit 7 of node[0], which is `(s8)node[0] < 0` (sign bit)
- `extrwi r0, r0, 1, 25` = test bit 6 of node[0], which is `(*(s8*)(node+0) & 0x40) != 0` — but using `>= 0` check: bit 7 NOT set AND bit 6 IS set means `0x40` in the byte. The asm checks bit6==1 after bit7==0 to determine end-of-chain. CW compiles `(s8*)node[0] >= 0` as sign-bit test which covers the negative/positive split the asm uses
- `fctiwz f0, f0` + `stfd` + `lwz r29, 0x1c(r1)` = float→s32 truncation via FPR; CW generates this for `(s32)*(f32*)ptr`
- The `fn_800E090C` stack destination must be at `sp+8` = `(u8*)&val - 0x18` relative to the local `val` (approximately; exact stack layout depends on CW's allocation)
- `li r3, 0x0` at epilog for fn_8002537C: `(s16)(0+0xf) = 0xf` stored at +0x50, then `0` at +0x52

---

## Diff/Delta for fn_80025490

Three changes from fn_8002537C:

1. **`lfs` reads from sp+0xC** (not sp+0x8): Change the fn_800E090C destination offset or stack layout so the float result lands at sp+0xC
2. **Node field at offset 0x4** (not 0x2): Change `*(s16*)(node + 2)` → `*(s16*)(node + 4)`
3. **Swapped stores**: Change epilog to:
```c
    *(s16*)(arg1 + 0x50) = 0;
    *(s16*)(arg1 + 0x52) = (s16)(val + 0xf);
```

Full body for fn_80025490:

```c
#pragma scheduling on
#pragma optimization_level 4
void fn_80025490(u8* arg0, u8* arg1) {
    extern u8 lbl_803A204C[];
    extern u8 lbl_803A2058[];
    extern f32 lbl_80478898;
    extern u32 lbl_80478DD8;
    extern u32 lbl_8047A368;
    extern u32 lbl_8047A370;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern void fn_800E090C(void*, void*, void*);
    extern f32 fn_800E0CA0(f32);
    u8* node;
    s32 count;
    s32 val;

    val = 0;
    if ((s32)lbl_8047A370 == 1) {
        fn_800E0CA0(lbl_80478898);
        fn_800E090C((void*)((u8*)&val - 0x14), (void*)lbl_803A2058, (void*)lbl_803A204C);
        val = (s32)*(f32*)((u8*)&val - 0x14);
    } else {
        if ((u32)lbl_8047A368 < *(u32*)lbl_80478DD8) {
            node = fn_8005DA18(*(u32*)(arg0 + 4));
            node = fn_8005D934(*(s16*)(node + 4));
            count = 0;
            for (;;) {
                if (*(s8*)(node + 0) < 0) {
                    if ((s32)lbl_8047A368 == count)
                        break;
                    count++;
                }
                if (*(s8*)(node + 0) >= 0) {
                    node = fn_8005D934(*(s16*)(node + 0x18));
                } else {
                    node = NULL;
                    break;
                }
            }
            if (node != NULL) {
                val = *(s16*)(node + 4);
            }
        }
    }
    *(s16*)(arg1 + 0x50) = 0;
    *(s16*)(arg1 + 0x52) = (s16)(val + 0xf);
}
```

**Note on fn_800E090C stack offsets**: The destination offsets (`&val - 0x18` and `&val - 0x14`) are approximations. CW's exact stack layout depends on variable ordering. The key constraint is fn_8002537C writes to sp+8 and fn_80025490 writes to sp+0xC. If the compiler places `val` (r29) at different stack offsets, the `fn_800E090C` destination and the `f0` load must target the same address. A local `f32` variable may be needed instead of pointer arithmetic.