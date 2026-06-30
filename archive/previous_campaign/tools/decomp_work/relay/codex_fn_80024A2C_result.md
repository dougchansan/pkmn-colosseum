```c
#pragma optimization_level 4
#pragma scheduling off
void fn_80024A2C(u8* arg0, u8* arg1) {
    u8* node;
    u8* found;
    s32 index;
    s32 offset;

    index = lbl_8047A368;
    node = fn_8005DA18(*(u32*)(arg0 + 4));
    node = fn_8005D934(*(s16*)(node + 4));
    offset = 0;
    while (1) {
        if ((((u32)node[0] >> 6) & 1) != 0) {
            if (index == offset) {
                found = node;
                break;
            }
            offset++;
        }
        if ((((u32)node[0] >> 5) & 1) == 0) {
            node = fn_8005D934(*(s16*)(node + 0x18));
        } else {
            found = 0;
            break;
        }
    }

    index = 0;
    offset = 0;
    while ((u32)index < *(u32*)lbl_80478DD8) {
        if (found == fn_8005D934(*(u32*)(lbl_80478DDC + offset + 8))) {
            break;
        }
        offset += 0x10;
        index++;
    }
    if ((u32)index >= *(u32*)lbl_80478DD8) {
        index = 0;
    }

    if ((s32)lbl_8047A370 == 1) {
        arg1[0x67] = (u8)(s32)(lbl_8047B8DC * (lbl_80478898 / lbl_8047B8A8));
    } else {
        arg1[0x67] = 0xFF;
    }

    if ((u32)index < *(u32*)lbl_80478DD8) {
        offset = index << 4;
        if (*(u32*)(lbl_80478DDC + offset + 4) == 0x66 && (u8)fn_801902E0((void*)0x45D) != 0) {
            *(u32*)(arg1 + 0x58) = 0x0C5F1200;
        } else {
            *(u32*)(arg1 + 0x58) = *(u32*)(lbl_80478DDC + offset + 0xC);
        }
    }
}
```

What was wrong:
- The current body has the wrong shape entirely: pseudo-register locals, manual `goto` labels, and fake `r3/r4/r28...` state force CodeWarrior into a very different CFG and register allocation.
- The real asm is a clean two-argument function: find the active node from the linked list, scan `lbl_80478DDC` for the matching entry, write `arg1[0x67]` from a small state-dependent float expression, then optionally write `arg1+0x58`.
- The state branch does not affect the lookup; it sits between the table scan and the final table-entry write.

Required pragmas:
- `#pragma optimization_level 4`
- `#pragma scheduling off`
