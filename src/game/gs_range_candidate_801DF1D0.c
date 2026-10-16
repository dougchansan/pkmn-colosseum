/**
 * @file gs_range_candidate_801DF1D0.c
 * @brief gs_range_801DE698.c carve, 0x801DF1D0 - 0x801DF474: the waza
 *        weather effect update, clear and start (fn_801DF1D0, fn_801DF33C,
 *        fn_801DF3D4).
 *
 * Owns .sdata2 0x8047E3D8 - 0x8047E3F0 (1.0f, 1/60, 50.0f, 2.0f and the
 * int-to-float bias), which only fn_801DF1D0 reads; MWCC emits them in
 * that order. fn_801DF474 is scored from gs_range_candidate_801DF474.c.
 */
#include "dolphin/types.h"

extern u8 fn_801DAC54(void* effect);
extern u32 GSgetFrameStep(void);
extern f32 fn_800E0BE4(void);
extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
extern void GSmodelSetTexAnimIndex(void* model, u32 index);
extern void GSmodelSetTexAnimRate(void* model, f32 rate);
extern void GSmodelSetTexAnimType(void* model, u32 type);
extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
extern void GSmodelStartTexAnimation(void* model);

/* RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own
 * pool literals -- see docs/RULE_EXCEPTIONS.md. 0.5f and 0.0f are shared
 * with the unlinked functions from 0x801DE698, so they stay in
 * sdata2_8047E390.c. */
extern const f32 lbl_8047E3C8; /* 0.5f */
extern const f32 lbl_8047E3CC; /* 0.0f */

/* Waza weather effect: start the texture animation after a random delay. */
void fn_801DF1D0(void* obj) {
    u8* effect = obj;
    void* model;
    u16 timer;
    f32 threshold;

    if (fn_801DAC54(effect) != 0) {
        return;
    }
    if ((effect[0x18] & 8) == 8) {
        return;
    }
    if (effect[0x19] != 0) {
        *(u16*)(effect + 0x20) = 0;
        return;
    }
    if (*(s16*)(effect + 0x1C) < 0) {
        return;
    }
    *(u16*)(effect + 0x20) += (u16)GSgetFrameStep();
    timer = *(u16*)(effect + 0x20);
    if (timer < 10) {
        return;
    }
    if (timer >= 60) {
        if (timer >= 180) {
            threshold = 1.0f;
        } else {
            threshold = 0.016666668f;
        }
    } else {
        f32 t = (f32)(timer - 10) / 50.0f;
        threshold = t * (2.0f - t);
        threshold *= 0.016666668f;
    }
    if (threshold >= fn_800E0BE4()) {
        model = *(void**)(effect + 0x24);
        if (model != NULL) {
            effect[0x19] = 3;
            GSmodelLinkTexAnimToAnim(model, 0);
            GSmodelSetTexAnimIndex(model, *(s16*)(effect + 0x1C));
            GSmodelSetTexAnimRate(model, lbl_8047E3C8);
            GSmodelSetTexAnimType(model, 0);
            GSmodelSetTexAnimFrame(model, lbl_8047E3CC);
            GSmodelStartTexAnimation(model);
            *(u16*)(effect + 0x20) = 0;
        }
    }
}

void fn_801DF33C(u8* obj) {
    void* model = *(void**)(obj + 0x24);

    if (*(s16*)(obj + 0x1E) >= 0 && obj[0x19] == 6) {
        obj[0x19] = 2;
        GSmodelLinkTexAnimToAnim(model, 0);
        GSmodelSetTexAnimIndex(model, *(s16*)(obj + 0x1E));
        GSmodelSetTexAnimRate(model, lbl_8047E3C8);
        GSmodelSetTexAnimType(model, 0);
        GSmodelSetTexAnimFrame(model, lbl_8047E3CC);
        GSmodelStartTexAnimation(model);
    }
}

void fn_801DF3D4(u8* obj) {
    void* model = *(void**)(obj + 0x24);

    if (*(s16*)(obj + 0x1A) >= 0 && obj[0x19] != 6 && obj[0x19] != 1) {
        obj[0x19] = 1;
        GSmodelLinkTexAnimToAnim(model, 0);
        GSmodelSetTexAnimIndex(model, *(s16*)(obj + 0x1A));
        GSmodelSetTexAnimRate(model, lbl_8047E3C8);
        GSmodelSetTexAnimType(model, 0);
        GSmodelSetTexAnimFrame(model, lbl_8047E3CC);
        GSmodelStartTexAnimation(model);
    }
}
