# fn_80023068 (GStitle_UpdateEffects) — Semantic C Analysis

Module: `src/game/gs_title.c`  
Size: 0x20C bytes  
Assembly source: `tools/decomp_work/relay/fn_80023068_asm.s`

---

## (1) Inferred Signature

```c
s32 fn_80023068(u32 arg0, u32* arg1);
```

Matches sibling `fn_80022EE4` pattern: `s32 fn(u32 arg0, u32* arg1)`.

---

## (2) Control Flow

- **Retry loop** (L_80023084 → L_800230E0 fallback): acquire slot via `fn_800141BC((void*)arg0, 1)`; if slot < 0 break out; read card data via `fn_80014118(slot, &sc, &sd)`; check flag 0x3e with `fn_80121ADC(sc, 0x3e)` — if nonzero, set `card_present = 1` and break; otherwise call `fn_80019B48(slot)` to get mode, call `fn_80019B1C()`, and loop back while mode < 0 (device busy/retrying)
- **Card-present branch** (card_present != 0 and slot >= 0): resolve pointer via `fn_8011F4F0(sc)`, notify with `fn_80132A38(0x32, ptr)`, send message `fn_80106D3C(2, 0x424D, 1, 0)` and sound `fn_801069FC(1)`; set result = 0 which leads to failure return
- **Normal read branch** (card_present == 0 and slot >= 0): call `fn_80144574(data_buf, sc, sd, (u16)arg0, mode & 0xFF)` to spawn/update effect; store return as `result`
- **Result > 0 lookup** (fn_80144574 succeeded): compare `arg0` against the 5 u16 values in `lbl_80266DB0` (offsets 0x0, 0x2, 0x4, 0x6, 0x8) to find a matching index; if index found (0-4) use message code 0x466, otherwise 0x465; call `fn_80166A50(code, 0, 0xFF, 0)` then `fn_8001D378()`
- **Name formatting** (always after read branch): call `fn_800216E8(name_buf, 0x40, data_buf, result, sc)`, then `fn_80132A38(0x4D, name_buf)`, `fn_80106D3C(2, 0xE0, 1, 0)`, `fn_801069FC(1)`
- **Slot release** (always): call `fn_80014198(slot)`
- **Success return**: if slot >= 0 AND `(s16)result > 0`: set `*arg1 = 1`, return 0
- **Failure return**: otherwise return 1

---

## (3) Complete C Body

```c
s32 fn_80023068(u32 arg0, u32* arg1) {
    s32 slot;
    s32 sc;
    s32 sd;
    s32 card_present;
    s32 mode;
    s32 result;
    void* ptr;
    char name_buf[0x40];
    char data_buf[0xA0];

    card_present = 0;
    do {
        slot = fn_800141BC((void*)arg0, 1);
        if (slot < 0)
            break;
        fn_80014118(slot, &sc, &sd);
        if (fn_80121ADC(sc, 0x3e) != 0) {
            card_present = 1;
            break;
        }
        mode = fn_80019B48(slot);
        fn_80019B1C();
    } while (mode < 0);

    result = 0;
    if (slot >= 0 && card_present != 0) {
        ptr = fn_8011F4F0(sc);
        fn_80132A38(0x32, ptr);
        fn_80106D3C(2, 0x424D, 1, 0);
        fn_801069FC(1);
    } else if (slot >= 0) {
        result = fn_80144574(data_buf, sc, sd, (u16)arg0, mode & 0xFF);
        if (result > 0) {
            s32 idx = -1;
            if (arg0 == *(u16*)(lbl_80266DB0 + 0x0))
                idx = 0;
            else if (arg0 == *(u16*)(lbl_80266DB0 + 0x2))
                idx = 1;
            else if (arg0 == *(u16*)(lbl_80266DB0 + 0x4))
                idx = 2;
            else if (arg0 == *(u16*)(lbl_80266DB0 + 0x6))
                idx = 3;
            else if (arg0 == *(u16*)(lbl_80266DB0 + 0x8))
                idx = 4;
            fn_80166A50((idx >= 0) ? 0x466 : 0x465, 0, 0xFF, 0);
            fn_8001D378();
        }
        fn_800216E8(name_buf, 0x40, data_buf, result, sc);
        fn_80132A38(0x4D, name_buf);
        fn_80106D3C(2, 0xE0, 1, 0);
        fn_801069FC(1);
    }

    fn_80014198(slot);
    if (slot >= 0 && (s16)result > 0) {
        *arg1 = 1;
        return 0;
    }
    return 1;
}
```

---

## Register Mapping (reference)

| Register | Role |
|----------|------|
| r30 | arg0 (u32) |
| r31 | arg1 (u32*) |
| r29 | slot |
| r28 | result (fn_80144574 return) |
| r27 | mode |
| r26 | card_present flag |
| r1+0x0C | sc (slot card output) |
| r1+0x08 | sd (slot data output) |
| r1+0xA0 | data_buf (fn_80144574 output) |
| r1+0x1C | name_buf (fn_800216E8 output) |
| r1+0x10..0x18 | stack copy of lbl_80266DB0 (5 u16 values) |

## Key ASM→C Notes

- `extsb r3, r29` before `fn_80019B48`: sign-extends low byte of slot; semantically redundant for small slot values (0-3 or -1), so plain `slot` is correct for CodeWarrior -O4,p
- `clrlwi r6, r30, 16` = `(u16)arg0` (low 16 bits, zero-extended)
- `clrlwi r7, r27, 24` = `mode & 0xFF` (low 8 bits, zero-extended)
- `(s16)result` at epilogue matches `extsh r0, r28` — fn_80144574 returns s32 but callers treat it as sign-extended s16
- `fn_80121ADC` returns u8; `clrlwi r0, r3, 24` masks to low byte; C `!= 0` is equivalent
- Card-present branch intentionally returns failure (1): card is physically detected but cannot be read
- Loop condition `while (mode < 0)` from `cmpwi r27, 0x0; blt @L_80023084` = branch to loop top when mode negative (device busy)