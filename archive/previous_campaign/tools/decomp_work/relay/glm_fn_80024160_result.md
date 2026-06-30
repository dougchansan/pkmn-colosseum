# fn_80024160 (GStitle_AutodemoRecord) — Semantic C Analysis

Module: `src/game/gs_title.c`
Size: 0x1A8 bytes
Assembly source: `tools/decomp_work/relay/fn_80024160_asm.s`

---

## (1) Inferred Signature

```c
void fn_80024160(u8* arg0, void* arg1, u16* arg2, u8* arg3);
```

- r3 = arg0 (structure base, writes to arg0[0x95])
- r4 = arg1 (unused, void-cast in body)
- r5 = arg2 (pointer to u16 bitfield, read once)
- r6 = arg3 (pointer to linked-list table, traversed per entry)

Call site in gs_title.c (line 4280): `fn_80024160(arg0, temp_r30, temp_r29)` — 3 explicit args; r6 carries through from prior computation.

---

## (2) Control Flow

- **Build mask from *arg2**: read u16 from arg2, remap bits: bit0→0x01, bit1→0x04, bit2→0x08, bit3→0x02; if mask==0 return immediately
- **Find active record**: load active_index from `lbl_8047A368`; resolve `fn_8005DA18(*(u32*)(arg0+4))` then `fn_8005D934(*(s16*)(current+4))` to get first node; walk linked list checking node[0] bit7 (0x80) to count active entries until chain_index==active_index → save as `active`; if end-of-chain (bit6 set) → active=NULL
- **Search entries table**: for each entry in table at `lbl_80478DF4` (count from `*(u32*)lbl_80478DF0`, stride 0xC): restart linked-list walk from `fn_8005D934(*(s16*)(arg3+4))`; for each node with bit7 set: check if `(entry_byte & mask) == mask`; if match, verify `fn_8005D934(entry[4]) == active` AND `fn_8005D934(entry[8+stride]) == candidate`; if both match → write `arg0[0x95] = (u8)active_count` and return
- **No match found**: function returns with arg0[0x95] unchanged (implicitly 0 or whatever caller initialized)

---

## (3) Complete C Body

```c
void fn_80024160(u8* arg0, void* arg1, u16* arg2, u8* arg3) {
    u16 bits;
    u8 mask;
    s32 active_index;
    u8* current;
    u8* active;
    u8* candidate;
    u8* entry;
    s32 active_count;
    s32 entry_index;
    s32 stride;

    (void)arg1;

    bits = *arg2;
    mask = 0;
    if (bits & 0x0001)
        mask |= 0x01;
    if (bits & 0x0002)
        mask |= 0x04;
    if (bits & 0x0004)
        mask |= 0x08;
    if (bits & 0x0008)
        mask |= 0x02;

    if (mask == 0)
        return;

    active_index = lbl_8047A368;
    current = fn_8005DA18(*(u32*)(arg0 + 4));
    current = fn_8005D934(*(s16*)(current + 4));

    active_count = 0;
    for (;;) {
        if (current[0] & 0x80) {
            if (active_index == active_count) {
                active = current;
                break;
            }
            active_count++;
        }
        if (current[0] & 0x40) {
            active = NULL;
            break;
        }
        current = fn_8005D934(*(s16*)(current + 0x18));
    }

    stride = 0;
    entry_index = 0;
    while ((u32)entry_index < *(u32*)lbl_80478DF0) {
        candidate = fn_8005D934(*(s16*)(arg3 + 4));
        active_count = 0;
        while (candidate != NULL) {
            if (candidate[0] & 0x80) {
                entry = (u8*)(lbl_80478DF4 + stride);
                if ((entry[0] & mask) == mask) {
                    if (fn_8005D934(*(u32*)(entry + 4)) == active) {
                        if (fn_8005D934(*(u32*)(entry + 8)) == candidate) {
                            arg0[0x95] = (u8)active_count;
                            return;
                        }
                    }
                }
                active_count++;
            }
            if (candidate[0] & 0x40)
                break;
            candidate = fn_8005D934(*(s16*)(candidate + 0x18));
        }
        stride += 0xC;
        entry_index++;
    }
}
```

---

## Key ASM→C Notes

- **Bit remap** (lines 8-33): u16 bit0→mask 0x01, bit1→0x04, bit2→0x08, bit3→0x02 — this is a specific input→internal mapping, not a shift
- **r28 reuse**: r28 first holds `active_index` (from lbl_8047A368), then is reused as `active_count` counter inside both walks (reset to 0 at line 71); the write `stb r28, 0x95(r25)` stores the inner walk's `active_count`, NOT the original `active_index`
- **Double-deref SDA**: `lwz r3, lbl_8047A368@sda21(r0)` loads a u32 from small-data area; `lwz r3, lbl_80478DF0@sda21(r0)` + `lwz r0, 0x0(r3)` is a pointer dereference — `*(u32*)lbl_80478DF0` gives the table count
- **lbl_80478DF4**: used as base pointer for the entries table; entries are 0xC bytes apart (stride of 12 bytes); each entry has byte[0]=mask byte, u32[4]=linked ref, u32[8]=linked ref
- **Linked-list nodes**: node[0] has bit7 (0x80) = "active" flag, bit6 (0x40) = "last" flag; node[0x18] (s16) is the index for fn_8005D934 to get next node
- **Early exit**: if mask==0 or a match is found in the entries table, function returns immediately; no match leaves arg0[0x95] unmodified