## Corrected C for fn_80024F2C

```c
#pragma optimization_level 4
#pragma scheduling off
void fn_80024F2C(u8* arg0, u8* arg1) {
    f32 pos[2];
    f32 fx;
    f32 fy;
    u8* node;
    u8 flags;
    u32 state;
    s32 r27;
    s32 r28;
    s32 r29;
    s32 r31;
    f32 t;

    state = lbl_8047A370;
    switch ((s32)state) {
    case 1:
        t = fn_800E0CA0(lbl_80478898);
        fn_800E090C(pos, lbl_803A2058, lbl_803A204C, t);
        fx = pos[0];
        fy = pos[1];
        r28 = (s32)fx;
        r31 = (s32)fy;
        break;
    default:
        r27 = lbl_8047A368;
        if ((u32)r27 < *(u32*)lbl_80478DD8) {
            node = fn_8005DA18(*(u32*)(arg0 + 4));
            node = fn_8005D934(*(s16*)(node + 4));
            r29 = 0;
            while (1) {
                flags = node[0];
                if ((((u32)flags >> 6) & 1) != 0) {
                    if (r27 == r29) {
                        break;
                    }
                    r29++;
                }
                if ((((u32)flags >> 5) & 1) == 0) {
                    node = fn_8005D934(*(s16*)(node + 0x18));
                } else {
                    node = 0;
                    break;
                }
            }
            if (node != 0) {
                r28 = *(s16*)(node + 2);
                r31 = *(s16*)(node + 4);
            }
        }
        break;
    }
    *(s16*)(arg1 + 0x50) = (s16)(s32)(lbl_8047B8E0 + ((f32)r28 + lbl_8047A374));
    *(s16*)(arg1 + 0x52) = (s16)(s32)(lbl_8047B8E0 + ((f32)r31 - lbl_8047A374));
}
```

## Diff vs fn_8002509C

```c
    *(s16*)(arg1 + 0x50) = (s16)(s32)(lbl_8047B8E0 + ((f32)r28 - lbl_8047A374));
    *(s16*)(arg1 + 0x52) = (s16)(s32)(lbl_8047B8E0 + ((f32)r31 + lbl_8047A374));
```

## Diff vs fn_8002520C

```c
    *(s16*)(arg1 + 0x50) = (s16)(s32)(lbl_8047B8E0 + ((f32)r28 - lbl_8047A374));
    *(s16*)(arg1 + 0x52) = (s16)(s32)(lbl_8047B8E0 + ((f32)r31 - lbl_8047A374));
```

## What was wrong

- The current `#else` bodies have the wrong signature; asm clearly takes `(u8* arg0, u8* arg1)` and uses `r4` as the output object.
- They are pseudo-registerized and dropped the matched control-flow shape. These functions are the same skeleton as `fn_80024DBC`, not hand-translated register code.
- The sibling delta is only the final sign choice on the two coordinate offsets:
  `24DBC = (+,+)`, `24F2C = (+,-)`, `2509C = (-,+)`, `2520C = (-,-)`.

## Required pragmas

```c
#pragma optimization_level 4
#pragma scheduling off
```
