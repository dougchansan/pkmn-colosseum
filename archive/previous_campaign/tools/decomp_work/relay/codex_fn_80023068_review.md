#pragma optimization_level 4
s32 fn_80023068(u32 arg0, u32* arg1) {
    u8 name_buf[0x84];
    u8 text_buf[0x108];
    s32 blocked;
    s32 mode;
    s32 slot;
    s32 sc;
    s32 sd;
    s32 effect;

    blocked = 0;
    for (;;) {
        slot = fn_800141BC((void*)arg0, 1);
        if (slot < 0) {
            break;
        }
        fn_80014118(slot, &sc, &sd);
        if ((u8)fn_80121ADC(sc, 0x3E) != 0) {
            blocked = 1;
            break;
        }
        mode = fn_80019B48((s8)slot);
        fn_80019B1C();
        if (mode >= 0) {
            break;
        }
    }

    if (slot >= 0) {
        if (blocked != 0) {
            fn_80132A38(0x32, fn_8011F4F0(sc));
            fn_80106D3C(2, 0x424D, 1, 0);
            fn_801069FC(1);
            effect = 0;
        } else {
            effect = ((s32 (*)(void*, s32, s32, u16, s32))fn_80144574)(text_buf, sc, sd, (u16)arg0, (u8)mode);
            if ((s16)effect > 0) {
                u16 entries[5];
                s32 match_index;

                memcpy(entries, lbl_80266DB0, sizeof(entries));
                if (arg0 == entries[0]) {
                    match_index = 0;
                } else if (arg0 == entries[1]) {
                    match_index = 1;
                } else if (arg0 == entries[2]) {
                    match_index = 2;
                } else if (arg0 == entries[3]) {
                    match_index = 3;
                } else if (arg0 == entries[4]) {
                    match_index = 4;
                } else {
                    match_index = 5;
                }
                if (match_index < 5) {
                    fn_80166A50(0x466, 0, 0xFF, 0);
                } else {
                    fn_80166A50(0x465, 0, 0xFF, 0);
                }
                fn_8001D378();
            }
            fn_800216E8(name_buf, 0x40, text_buf, effect, sc);
            fn_80132A38(0x4D, name_buf);
            fn_80106D3C(2, 0xE0, 1, 0);
            fn_801069FC(1);
        }
    }

    fn_80014198(slot);
    if (slot >= 0 && (s16)effect > 0) {
        *arg1 = 1;
        return 0;
    }
    return 1;
}

## Match-Sensitive Notes

- GLM's `name_buf[0x40]` / `data_buf[0xA0]` sizing is not safe for this frame. The asm uses `name_buf` at `r1+0x1C` and `text_buf` at `r1+0xA0`, so the actual local extents are `0x84` and `0x108` with the saved-register block starting at `r1+0x1A8`.
- Keep `mode = fn_80019B48((s8)slot);`. The asm has `extsb r3, r29`; passing plain `slot` is the main signedness/ABI miss in the GLM draft.
- Keep the raw `fn_80144574` result in a 32-bit temp and only test it via `(s16)effect`; the epilogue does `extsh` before the compare. If the file-level extern remains `s16`, keep the local casted call above or fix the extern for this site.
- Do not initialize `effect` before the loop just to silence warnings. In asm it is only written on the `slot >= 0` paths, and the final `slot >= 0 && (s16)effect > 0` short-circuit avoids reading it on the failure path.
- Direct `lbl_80266DB0` comparisons are semantically fine but not codegen-faithful. The target copies 10 bytes to stack (`0x10..0x18`) before the compare chain; if `memcpy` does not inline to `lwz/lwz/lhz` + `stw/stw/sth`, this block will need hand-tuning. `#pragma scheduling off` is a reasonable fallback if the compare/copy sequence drifts.
