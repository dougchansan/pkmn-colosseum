# fn_80023068 — Final Review

Target: `src/game/gs_title.c` (replaces current `#else` branch at line 3909-3982).
Size: 0x20C. Sibling `fn_80022EE4` matched with `#pragma scheduling on` + local `s32`-return extern for `fn_80144574`.

## Final C (ready to paste)

```c
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80023068(u32 arg0, u32* arg1) {
    extern s32 fn_80144574(void*, s32, s32, u16, s32);
    u8 name_buf[0x84];
    u8 text_buf[0x108];
    u16 entries[5];
    s32 slot;
    s32 sc;
    s32 sd;
    s32 mode;
    s32 effect;
    s32 blocked;
    s32 match_index;

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
            effect = fn_80144574(text_buf, sc, sd, (u16)arg0, (u8)mode);
            if ((s16)effect > 0) {
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
```

## Rationale for key decisions

- **Pragma stack**: `#pragma scheduling on` + `#pragma optimization_level 4`. Exact match with sibling `fn_80022EE4` at line 3856-3857 which just matched 100%. No `#pragma peephole` override — file default is `on`, sibling doesn't set it.
- **Local `fn_80144574` extern as `s32`**: File-level prototype at line 3334 is `extern s16 fn_80144574(...)`. An `s16` return would trigger an immediate `extsh r3,r3` at the call site, but the target asm only does `extsh r0, r28` at the compare, with the raw r3 value stored into r28 untouched. Shadowing with a local `s32`-return extern defeats the fold — same trick that unlocked `fn_80022EE4` (line 3859).
- **No function-pointer casts**: The previous attempt wrapped every call (`((u32 (*)(...))fn_80121ADC)`, etc.) in casts. All these have correct file-level prototypes already (lines 3333, 3335, 3336, 3212, 3903, 3904). Casts force a function-pointer through a temp and can perturb scheduling. Sibling calls directly; mirror that.
- **`(u8)fn_80121ADC` / `(s8)slot` / `(u8)mode` / `(s16)effect`**: match `clrlwi r0,r3,24`, `extsb r3,r29`, `clrlwi r7,r27,24`, `extsh r0,r28` respectively. These are load-bearing — removing any of them perturbs codegen.
- **`memcpy(entries, lbl_80266DB0, 10)` instead of direct `*(u16*)(lbl_80266DB0 + N)` chain**: the target asm explicitly copies 10 bytes to stack (`stw r4,0x10(r1); stw r0,0x14(r1); sth r3,0x18(r1)`) then reloads via `lhz r0, 0x10(r1)` etc. CW 1.3 inlines a 10-byte `memcpy` to exactly this `lwz/lwz/lhz + stw/stw/sth + lhz reload` shape. The direct-dereference pattern at line 3469-3473 (in `fn_80022478`) compiles to a different shape — it matches there because that function's asm does not spill to stack; ours does. Use `memcpy` here. Note `memcpy` is already used in the same file at line 4056 with the same idiom.
- **Buffer sizing**: frame is `0x1C0`, saved regs start at `0x1A8`. `name_buf` used at `r1+0x1C` with next local (`text_buf`) at `r1+0xA0` → `0x84` bytes. `text_buf` at `r1+0xA0` up to `r1+0x1A8` → `0x108` bytes. GLM's `0x40`/`0xA0` underflows the frame.
- **Declaration order**: buffers first (`name_buf`, `text_buf`, `entries`), then scalars in asm register-usage order (`slot`=r29, `sc`=0xC, `sd`=0x8, `mode`=r27, `effect`=r28, `blocked`=r26, `match_index`=r5). CW 1.3 -O4,p lays out locals roughly in declaration order; putting the three arrays first encourages them into the fixed frame slots the asm expects (`0x1C`, `0xA0`, `0x10`).
- **`effect` uninitialized on failure path**: intentional. asm only writes r28 on `slot >= 0 && blocked == 0`, and the final guard `slot >= 0 && (s16)effect > 0` short-circuits before reading it on any failure path. CW 1.3 will not emit a spurious init. Codex flagged this; do not "fix" it.
- **`fn_800216E8` called directly**: its visible prototype is `void fn_800216E8(void)` (the `asm void` at line 3215). Matched caller at line 3481 calls it the same way with 5 args. No local extern needed — CW with `(void)` proto passes the registers we supply without promotion.

## Expected match risk

**Medium-low.**

Confidence drivers:
- Sibling function with near-identical structure just matched using the same pragma + local-extern recipe. Every non-trivial codegen quirk in that sibling is replicated here (scheduling on, optimization_level 4, local `s32`-return fn_80144574, `(s8)slot`, `(u8)mode`, `(s16)effect` epilogue check, no casts).
- All five match-sensitive spots Codex identified are addressed.

Residual risks, ranked:
1. **memcpy inlining shape** (medium). CW 1.3 inlines fixed-size small `memcpy` but alignment/scheduling interactions with the surrounding lhz chain could differ by one instruction. If mismatch appears in the `0x16C..0x1C8` range of the asm, try (a) `#pragma scheduling off` around just the compare block, or (b) switch to a hand-written 10-byte copy `entries[0]=((u16*)lbl_80266DB0)[0]; ...` which also tends to produce lwz+lwz+lhz on CW -O4,p.
2. **Scalar local layout** (low). If `blocked`/`match_index` land at the wrong offsets the spill/reload pattern will drift. Mitigation: both are short-lived and should stay in r26/r5 as in the asm, never spilling.
3. **`fn_80166A50` arg-order / peephole** (low). The `if(match_index<5)` branches into two nearly-identical calls; CW peephole may fold to a single call with a conditional `li r3`. Current asm shows the folded form (`li r3,0x466; b; li r3,0x465; ... bl fn_80166A50`). The C above should fold identically with scheduling on.

If this mismatches, the first diff will point at exactly one of the three. Address it, don't rewrite.
