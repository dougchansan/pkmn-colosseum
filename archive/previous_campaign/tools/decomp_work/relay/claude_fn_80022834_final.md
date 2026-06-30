# fn_80022834 — Final Review

Target: `src/game/gs_title.c` lines 3450-3559. Size 0x308. Currently asm-active (`#if 1`). Flip to `#if 0` and replace the current `#else` body.

## Forward declarations to update

Replace the file-level extern block at lines 3451-3463 with:

```c
extern void fn_800232F0(void);
extern void fn_80023274(void);
```

Everything else needed is declared as local `extern` inside the function body (see below). Delete the stale `extern void fn_80XXXXXX(void)` one-liners at lines 3452-3463 — they all had wrong signatures and the local externs shadow them anyway.

## Final C (ready to paste into #else, flip #if to 0)

```c
/* fn_80022834 - 0x80022834 | size: 0x308 */
extern void fn_800232F0(void);
extern void fn_80023274(void);
#if 0
asm void fn_80022834(void) {
#include "src/game/gs_title_fn_80022834.inc"
}
#else
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80022834(u32 arg0, u32* arg1) {
    extern void* fn_801440A0(u32);
    extern u32 fn_80143F24(void*);
    extern u32 fn_80143EF0(u32);
    extern u32 fn_80143E88(void*);
    extern s32 fn_8001E074(s32, s32, s32, s32);
    extern u32 fn_8011F228(s32, u32);
    extern void fn_8011F5C8(s32);
    extern s32 fn_8011E778(void);
    extern u8 fn_8011E2AC(s32, s32);
    extern s32 fn_802600E4(s32, u32, void*, s32, void*, s32);
    extern void fn_80123D58(s32, u32, u32);
    extern u32 fn_80123090(s32);
    extern void fn_80122370(s32, u32, s32);

    void* handle;
    u8 status;
    u32 value16;
    s32 type_byte;
    s32 msg;
    s32 state;
    s32 action;
    s32 slot;
    s32 sc;
    s32 sd;
    s32 effect;
    s32 i;
    u8 buf;

    handle = fn_801440A0((u16)arg0);
    status = (u8)fn_80143F24(handle);
    value16 = (u16)fn_80143EF0(status);
    fn_80166A50(0x4CB, 0, 0xFF, 0);
    type_byte = fn_80143E88(handle);
    if ((u8)type_byte != 0xFF) {
        msg = 0x4260;
    } else {
        msg = 0x4265;
    }
    fn_80132A38(0x39, (void*)value16);
    fn_80106D3C(2, msg, 1, 0);
    fn_801069FC(1);
    fn_80106D3C(2, 0x426B, 1, 0);
    state = (s8)fn_8001E074(0, -1, -1, 0);
    fn_801069FC(1);
    switch (state) {
    case 0:  action = 0; break;
    case 1:  action = 1; break;
    default: action = 2; break;
    }
    if (action == 1 || action == 2) {
        return 1;
    }

    slot = fn_800141BC((void*)arg0, 1);
    if (slot >= 0) {
        fn_80014118(slot, &sc, &sd);
        if (fn_8011FC74(sc) != 0) {
            fn_80106D3C(2, 0x424C, 1, 0);
            fn_801069FC(1);
            effect = 0;
        } else {
            value16 = fn_80143EF0(status);
            for (i = 0; i < 4; i++) {
                if ((u16)value16 == (u16)fn_8011F228(sc, (u16)i)) {
                    break;
                }
            }
            if (i < 4) {
                fn_80132A38(0x32, fn_8011F4F0(sc));
                fn_80132A38(0x39, (void*)(u16)value16);
                fn_80106D3C(2, 0x4244, 1, 0);
                fn_801069FC(1);
                effect = 0;
            } else {
                fn_8011F5C8(sc);
                if (fn_8011E2AC(fn_8011E778(), status) == 0) {
                    fn_80132A38(0x32, fn_8011F4F0(sc));
                    fn_80132A38(0x39, (void*)(u16)value16);
                    fn_80106D3C(2, 0x423F, 1, 0);
                    fn_801069FC(1);
                    effect = 0;
                } else {
                    effect = fn_802600E4(sc, value16, &buf, 1, fn_80023274, 0);
                    if (effect != 0) {
                        fn_80123D58(sc, buf, (u16)value16);
                        fn_80122370(sc, fn_80123090(sc), 4);
                    }
                }
            }
        }
    } else {
        effect = 0;
    }

    fn_80014198(slot);
    if (slot >= 0 && effect != 0) {
        if ((u8)type_byte == 0xFF) {
            *arg1 = 1;
        } else {
            *arg1 = 0;
        }
        return 0;
    }
    return 1;
}
#endif
```

## What changed vs the current #else attempt (lines 3469-3558)

1. **`fn_8011E2AC` arg order was wrong.** Current: `fn_8011E2AC(status, sc)`. Asm: `bl fn_8011E778` (result stays in r3) then `mr r4, r24` (status → r4) then `bl fn_8011E2AC`. So args are `(fn_8011E778(), status)` — fix changes both the arg list *and* drops the discarded `fn_8011E778()` call the current C had as a statement expression of its own.
2. **`fn_80123D58` arg order was wrong.** Current: `(sc, (u16)value16, type_byte)`. Asm: `r3=sc, r4=*(u8*)(r1+0x8), r5=clrlwi r27, 16`. So args are `(sc, buf, (u16)value16)` — the buf byte (local stack output of `fn_802600E4`), not `type_byte`.
3. **Duplicate `fn_80123090(sc)` call removed.** Current had two invocations at lines 3538-3539; asm has only one, whose return feeds `fn_80122370`.
4. **State switch restructured.** Current: `if (state == 0 || state == 1) return 1;`. Asm: a 3-way switch mapping `{0→0, 1→1, default→2}` then `if (action == 1 || action == 2) return 1;`. Only `state == 0` falls through. Current C wrongly made `state == 0` return 1 and `state == 2/negative` continue — exact opposite for the two main paths. Fix: switch + action-check form.
5. **`type_byte` retyped `u8 → s32`.** Asm stores raw r3 into r31 with no `clrlwi` at assignment, then masks `clrlwi r0, r31, 24` at each use. `u8` declaration would push the mask to assignment time, changing codegen at both check sites. `s32` with `(u8)type_byte` at every compare site matches.
6. **All `((T (*)(...))fn_X)(args)` function-pointer casts replaced with local `extern` prototypes.** The pointer-cast form routes the call through a temp register and can perturb scheduling under `#pragma scheduling on`. The sibling matched function `fn_80022EE4` (line 3763) uses the same local-extern technique.
7. **`#pragma scheduling on` added.** Current only has `optimization_level 4`. Matched sibling uses both. Scheduling on is required for the tight call-sequence patterns here; nothing in this function reloads shared memory between same-address loads, so scheduling can safely reorder.
8. **Removed unused stale declarations** `fn_801440A0..fn_80122370` at file-level (lines 3452-3463) — they were all `void X(void)` bogus prototypes. Keep only `fn_800232F0` and `fn_80023274` which are still used (callback passed to `fn_802600E4`).
9. **`value16` used with consistent u32 typing.** First assignment pre-masks `(u16)` — matches asm `clrlwi r25, r3, 16` at assignment. Second assignment (inside slot branch) leaves it raw — matches asm `mr r27, r3` with `clrlwi r4, r27, 16` at use sites. Same variable; CW -O4 emits different code at each site based on whether a `(u16)` cast is present.
10. **`handle` typed `void*`, `status` typed `u8`, `buf` typed `u8`.** Matches asm register widths.

## Expected match risk

**Medium.**

Confidence drivers:
- Every obvious-wrong thing in the current C is addressed (two arg-order bugs, duplicate call, inverted state logic).
- Local-extern + pragma recipe validated by the matched sibling `fn_80022EE4` just two functions below in the same file.
- Structural shape is a straightforward slot-lock + nested dispatch — no float magic, no out-of-bounds quirks, no scheduling-sensitive memory aliasing.

Residual risks, ranked:
1. **Switch codegen for `state`** (medium). CW -O4 may generate either the asm's compare-chain shape or a table. If the diff lands in the 4-instruction range after `fn_8001E074`, replace the switch with explicit `if`/`else if`/`else` in the order `case 1 → case 0 → default` to bias CW toward the asm shape.
2. **`(u16)value16` hoisting** (low). Inside the 4-iteration loop, CW should hoist the `(u16)value16` mask out; asm confirms one-time precompute (`clrlwi r28, r27, 16`) before the loop. If it doesn't hoist, cache explicitly: `u32 vmask = (u16)value16;` before the loop.
3. **`(u16)i` cast on loop counter** (low). Asm does `clrlwi r4, r26, 16` inside each iteration. With `u32 i` and `(u16)i` at the call site, CW should emit this every iteration — no hoist because the value changes. If CW elides it (treats `i` as always ≤ 3 and therefore already u16-clean), remove the `(u16)` cast and let CW pass raw `i`.
4. **`(s8)` cast on `fn_8001E074`'s return** (low). Asm has `extsb r28, r3`. `(s8)` cast produces exactly that.
5. **Result-code constant folding** (low). CW might fold the `0x4260/0x4265` branch to a conditional `li` without the extra `b`. Asm shows the `beq; li; b; li` form, which matches the literal if/else as written.

If this mismatches, the first diff locates to one of the above; restructure that block specifically and re-diff.
