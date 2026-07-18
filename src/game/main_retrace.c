/**
 * @file main_retrace.c
 * @brief Candidate per-retrace game logic.
 *
 * Address range: 0x80005E00 - 0x80005FA8
 */

#include "dolphin/types.h"

extern void fn_80101B90(u32 mask);
extern u32 fn_800F7AF0(u32 padIdx);
extern u32 fn_800F7BC4(u32 padIdx);
extern void fn_8000C0DC(void);
extern u8 fn_801E11E8(void);
extern s32 fn_801E11E0(void);
extern void fn_801E11B0(void);
extern void fn_801E119C(void);
extern void fn_801E118C(void);
extern void menuDaemon(void);
extern void GSmsgDaemon(void);
extern u32 fn_80128E24(void);
extern void* fn_80128E04(void);
extern void* gamedatasaveBiosGetPtr(void);
extern f32 gamedatasaveBiosGetPlaytime(void* mixer);
extern u32 fn_801906A0(u32 evtId);
extern u32 fn_800D3088(void);
extern s32 fn_800D37CC(void);
extern void gamedatasaveBiosSetPlaytime(void* mixer, f64 volume);

extern u8 lbl_80478DC8;
extern f32 lbl_8047B6A4;

/* =========================================================================
 *  fn_80005E00 / fn_80005E00
 *  Address: 0x80005E00, Size: 0x1A8
 *
 *  Per-retrace game logic handler. Checks controller buttons for
 *  debug menu activation, handles save/card state transitions,
 *  and processes particle effects and sound.
 *
 *  Also handles reading volume from the sound mixer and applying
 *  time-based adjustments.
 * =========================================================================
 */
#pragma push
#pragma peephole off
void fn_80005E00(void) {
    void* mixer;
    f32 vol;
    f32 v;
    u32 pressed;
    u32 held;

    fn_80101B90(0xFF);

    pressed = fn_800F7AF0(1);
    held = fn_800F7BC4(1);
    if (held & pressed & 0x10) {
        fn_8000C0DC();
    }

    if (fn_801E11E8()) {
        pressed = fn_800F7AF0(1);
        held = fn_800F7BC4(1);
        if (held & pressed & 0x40) {
            if (fn_801E11E0() == 2) {
                fn_801E11B0();
            } else {
                fn_801E119C();
            }
        }
        pressed = fn_800F7AF0(1);
        held = fn_800F7BC4(1);
        if (held & pressed & 0x20) {
            fn_801E118C();
        }
    }

    menuDaemon();
    GSmsgDaemon();

    if (fn_80128E24() != 0 && fn_80128E04() != 0) {
        mixer = gamedatasaveBiosGetPtr();
    } else {
        mixer = NULL;
    }

    if (mixer != NULL && lbl_80478DC8 == 1 && fn_801906A0(0x8AE) == 0) {
        vol = gamedatasaveBiosGetPlaytime(mixer);
        v = vol + (f32)fn_800D3088() / (f32)fn_800D37CC();
        if (v >= lbl_8047B6A4) {
            v = lbl_8047B6A4;
        }
        gamedatasaveBiosSetPlaytime(mixer, v);
    }
}
#pragma pop
