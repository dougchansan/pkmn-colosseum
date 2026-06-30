# fn_80024DBC — Final Review

Target: `src/game/gs_title.c` lines 880-941 (replaces the active `#else` body). Size 0x170, 10.9% matched today.

## Final C (ready to paste into #else)

Leave the file-level externs at lines 869-879 unchanged. Replace the body:

```c
#pragma scheduling off
#pragma optimization_level 4
void fn_80024DBC(u8* arg0, u8* arg1) {
    f32 pos[2];
    s32 r28;
    s32 r31;
    u32 r27;
    s32 r29;
    u8* node;
    f32 t;

    switch ((s32)lbl_8047A370) {
    case 1:
        t = fn_800E0CA0(lbl_80478898);
        fn_800E090C(pos, lbl_803A2058, lbl_803A204C, t);
        r28 = (s32)pos[0];
        r31 = (s32)pos[1];
        break;
    default:
        r27 = lbl_8047A368;
        if (r27 < *(u32*)lbl_80478DD8) {
            node = (u8*)fn_8005DA18(*(u32*)(arg0 + 4));
            node = (u8*)fn_8005D934(*(s16*)(node + 4));
            r29 = 0;
            while (1) {
                if (((*(u8*)node >> 7) & 1) != 0) {
                    if (r27 == (u32)r29) {
                        break;
                    }
                    r29++;
                }
                if (((*(u8*)node >> 6) & 1) != 0) {
                    node = NULL;
                    break;
                }
                node = (u8*)fn_8005D934(*(s16*)(node + 0x18));
            }
            if (node != NULL) {
                r28 = *(s16*)(node + 2);
                r31 = *(s16*)(node + 4);
            }
        }
        break;
    }
    *(s16*)(arg1 + 0x50) = (s16)(s32)(lbl_8047B8E0 + ((f32)(s16)r28 + lbl_8047A374));
    *(s16*)(arg1 + 0x52) = (s16)(s32)(lbl_8047B8E0 + ((f32)(s16)r31 + lbl_8047A374));
}
```

## Rationale

1. **Bit-mask numbers were off by one**. The asm uses `extrwi r0, r0, 1, 24` (bit 24 left-numbered = bit 7 right-numbered = mask 0x80) and `extrwi r0, r0, 1, 25` (mask 0x40). The current C uses `>> 6` and `>> 5` which are masks 0x40 and 0x20. Fixed to `>> 7` (bit 7, 0x80) and `>> 6` (bit 6, 0x40). This is the biggest semantic change and alone accounts for most of the 89% diff.
2. **Missing `(s16)` cast in the final f32 math**. The asm does `extsh r0, r28; xoris r4, r0, 0x8000; ...` — it sign-extracts the low 16 of r28 *before* the int-to-float magic. Without the `(s16)` cast, CW would cast `s32 r28` directly to f32 using the signed-magic idiom with no `extsh`, missing two instructions per coord. Same for r31. Added `(f32)(s16)r28` / `(f32)(s16)r31`.
3. **Dropped the `flags = node[0];` local**. The asm emits two separate `lbz 0x0(r3)` loads (before each extrwi). Caching into a `u8 flags` local leaves the value in one register and CW emits a single lbz. Accessing `*(u8*)node` inline in each test keeps both loads present.
4. **Dropped unused locals `fx`, `fy`, `state`, `flags`**. `state` was only used in the switch — inlined `switch ((s32)lbl_8047A370)`. `fx`/`fy` were pass-throughs — inlined `(s32)pos[0]`, `(s32)pos[1]`. They added spill pressure that CW -O4 sometimes materializes against scheduling-off.
5. **`r27` retyped from `s32` to `u32`**. `lbl_8047A368` is declared `u32`. The asm uses unsigned compare `cmplw r27, r0`. Matching types avoids a needless `(u32)` cast in the compare. Comparison against the loop counter `r29` became `r27 == (u32)r29` to preserve the `cmpw` signed compare CW emits for `cmpw r27, r29` (CW uses `cmpw` when one side is signed; r27 being u32 and r29 being s32 forces a cast somewhere — asm uses `cmpw`, so the s32 interpretation wins; cast r29 to u32 side to avoid a warning without changing codegen).
   
   If this causes a `cmplw` vs `cmpw` diff in the final listing, switch to `(s32)r27 == r29` instead.
6. **`#pragma scheduling off` kept** (matches sibling `fn_80024CDC` in same file). Required for the double-lbz pattern and for the deterministic load order on `lbl_80478DD8` / `lbl_8047A368`.
7. **Case 1 body uses `t` local**. Required because fn_800E0CA0 returns in f1 and fn_800E090C consumes in f1 — but between the two calls CW has to move the return through a named slot if there's any instruction in between. The `t` local keeps it simple; the intermediate casts are what matter for code shape.
8. **`NULL` for pointer clear**. The asm `li r3, 0x0` matches either `0` or `NULL` — cosmetic.

The 4-sibling delta (the prompt flags `fn_80024DBC=(+,+)`, `fn_80024F2C=(+,-)`, `fn_8002509C=(-,+)`, `fn_8002520C=(-,-)`): this function is the (+,+) flavor — both `*(s16*)(arg1 + 0x50)` and `*(s16*)(arg1 + 0x52)` use `+` inside the parens. Confirmed by asm: both stores follow the same `fadds f0, f4, f1` addition chain with `lbl_8047B8E0` on the positive side. The three sibling functions will differ only by negating one or both coord expressions.

## Expected match risk (high/medium/low)

**Low-medium.**

- All six asm-specific quirks are addressed: bit masks, `(s16)` cast, double lbz, `*(u32*)lbl_80478DD8` indirect, unsigned compare, scheduling off.
- Matched sibling `fn_80024CDC` in the same file validates the pragma recipe and the `(f32)(s16)int` idiom.

Residual risks, ranked:
1. **CSE-ing the two `*(u8*)node` reads** (medium). CW -O4 may still fold them to a single lbz despite `scheduling off`. If the diff lands in the loop body, change each test to assign to a `volatile u8` temp or restructure as `u8 b0 = node[0]; if (b0 & 0x80) ...; { u8 b1 = node[0]; if (b1 & 0x40) ...; }` — the second local tricks CSE more reliably than two direct reads.
2. **`cmpw` vs `cmplw` on `r27 == r29`** (low). Noted above; two fallback types to try.
3. **Load ordering for `lbl_80478DD8` / `lbl_8047A368`** (low). Asm loads `lbl_80478DD8` first. With scheduling off, CW follows source order — `r27 = lbl_8047A368;` then the compare. If the listing shows the other order, hoist a temp: `u32 limit = *(u32*)lbl_80478DD8; r27 = lbl_8047A368; if (r27 < limit) ...`.
4. **Return type `void`** (low). The asm epilogue does not set r3. If a caller expects a return, change to `s32` + `return 0;` — but no `li r3` at epilogue suggests void is correct.

If this mismatches, the first diff will localize to the loop body or the final sth coord math.
