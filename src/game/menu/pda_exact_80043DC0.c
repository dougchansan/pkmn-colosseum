/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80043DC0.c
 * @brief PDA swinging-arrow sprite callback, 0x80043DC0 - 0x80043EC8.
 *
 * The body is exact (copied from pda_range_80037158.c). Its float literals
 * (pi, 2.0f) and the signed int-to-float bias are redirected by the
 * temporary constant_import build step to lbl_8047BCA4, lbl_8047BCA8 and
 * lbl_8047BCB0 in sdata2_8047BCA0.c.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
} PdaSceneWork;

extern PdaSceneWork lbl_803A6818;

void pdaMemoDrawAnim(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern s32 lbl_804788C0;
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BD0C;
    extern f64 sin(f64 angle);

    alphaSprite->alphaByte =
        lbl_8047BCA0 * lbl_803A6818.alphaScale;
    if (lbl_803A6818.currentIndex != lbl_803A6818.field_10 - 1) {
        if (lbl_804788C0 != 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
            return;
        }
    } else {
        sprite->flags &= ~2;
        return;
    }
    sprite->field_52 =
                (s16)(lbl_8047BD0C *
                          (f32)sin(3.1415927f * (2.0f * (2.0f * lbl_803A6818.angle))) +
                      (f32)*(s16*)(lbl_802EF0A8 + 0x57F4));
}
