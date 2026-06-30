# fn_80024BA4 — Corrected Semantic C

Module: `src/game/gs_title.c`
Size: 0x138 bytes
Match: 9.0% (current C uses pseudo-registers)
Target: CodeWarrior 1.3 -O4,p

---

## Required Pragmas

```c
#pragma optimization_level 4
#pragma scheduling off
```

## Control Flow

- r31 = arg1, r30 = lbl_8047A36C (active_index); resolve linked list from fn_8005DA18(*(arg0+4))→fn_8005D934(node[4])
- Walk linked list: count bit7-nodes; when count==active_index → save node ptr as `found`; if chain ends → found=NULL
- Search entries table at lbl_80478DDC (stride 0x10, count from lbl_80478DD8): for each entry, call fn_8005D934(entry[8]) and compare with found ptr; break on match (keep entry_index r30, offset r28)
- If no match (loop exhausted) → r30=0; then check r30 < count to decide if we read an entry
- If r30 < count: look up entry at lbl_80478DDC + r28; if entry[4] == 0x66 → call fn_801902E0(0x45D); if returns nonzero → value = 0xC5F1200; else → value = entry[0xC]; store value at arg1[0x58]
- If r30 >= count → skip store, return

---

## Final C Body

```c
#pragma optimization_level 4
#pragma scheduling off
void fn_80024BA4(u8* arg0, u8* arg1) {
    extern u32 lbl_80478DD8;
    extern u32 lbl_80478DDC;
    extern u32 lbl_8047A36C;
    extern u8* fn_8005D934(s32);
    extern u8* fn_8005DA18(u32);
    extern u32 fn_801902E0(s32);
    u8* node;
    u8* found;
    s32 count;
    s32 entry_index;
    s32 offset;
    u32 value;
    u8* entry;

    found = (u8*)0;
    node = fn_8005DA18(*(u32*)(arg0 + 4));
    node = fn_8005D934(*(s16*)(node + 4));
    count = 0;
    for (;;) {
        if (*(s8*)(node + 0) < 0) {
            if ((s32)lbl_8047A36C == count) {
                found = node;
                break;
            }
            count++;
        }
        if (*(s8*)(node + 0) >= 0) {
            node = fn_8005D934(*(s16*)(node + 0x18));
        } else {
            found = (u8*)0;
            break;
        }
    }

    entry_index = 0;
    offset = 0;
    while ((u32)entry_index < *(u32*)lbl_80478DD8) {
        if (found == fn_8005D934(*(u32*)(*(u8**)&lbl_80478DDC + offset + 8)))
            break;
        offset += 0x10;
        entry_index++;
    }
    if (entry_index >= *(u32*)lbl_80478DD8)
        entry_index = 0;

    if ((u32)entry_index < *(u32*)lbl_80478DD8) {
        entry = *(u8**)&lbl_80478DDC + offset;
        value = *(u32*)(entry + 0xC);
        if (*(u32*)(entry + 0x4) == 0x66) {
            if ((fn_801902E0(0x45D) & 0xFF) != 0) {
                value = 0xC5F1200;
            }
        }
        *(u32*)(arg1 + 0x58) = value;
    }
}
```

---

## ASM→C Notes

- **r29 dual use**: r29 starts as counter (0,1,2...) in first walk, then becomes the found NODE POINTER on match (`mr r29, r3` at line 23), or 0 on chain-end (`li r29, 0x0` at line 36). In C, use separate `count`/`found` variables — CW allocates r29 to `found`
- **r30 dual use**: r30 holds `lbl_8047A36C` (active index) during first walk, then reused as `entry_index` in second walk. CW reuses the register across scopes
- **`extrwi r0, r0, 1, 24`** = bit7 test = `*(s8*)(node+0) < 0` (sign bit)
- **`extrwi r0, r0, 1, 25`** = bit6 test: in asm this checks for end-of-chain. When bit7=0 and bit6=1, the node has `s8` value `0x40..0x7F` which is `>= 0`. The `if (*(s8*)(node+0) >= 0)` in the else-branch after the sign-bit `if` covers both "continue" (bit6=0) and "break" (bit6=1)
- **`lwz r3, lbl_80478DDC@sda21(r0)` + `lwzx`**: lbl_80478DDC is a pointer stored in SDA. `*(u8**)&lbl_80478DDC` dereferences it to get the table base
- **Entry stride**: 0x10 bytes; field at +0x4 is u32 (checked == 0x66), field at +0x8 is u32 (resolved via fn_8005D934 for matching), field at +0xC is u32 (default value stored to arg1+0x58)
- **0xC5F1200**: `lis r3, 0xC5F` + `addi r0, r3, 0x1200` = 0xC5F0000 + 0x1200 = 0xC5F1200