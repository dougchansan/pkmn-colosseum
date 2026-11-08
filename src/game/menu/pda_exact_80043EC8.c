/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80043EC8.c
 * @brief PDA bobbing-cursor sprite callback, 0x80043EC8 - 0x80043FA8.
 *
 * The body is exact (copied from pda_range_80037158.c). Its compiler-owned
 * signed int-to-float bias is redirected by the temporary constant_import
 * build step to lbl_8047BCB0 in sdata2_8047BCA0.c.
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

extern u8 lbl_803A6818[];

void pdaMemoDrawAnim2(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BCA0;
    extern f32 lbl_8047BCA4;
    extern f32 lbl_8047BCA8;
    extern f32 lbl_8047BD0C;
    extern f64 sin(f64 angle);
    f32 f0;
    f32 f1;
    f32 f3;

    alphaSprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    if (*(s32*)&lbl_803A6818 != 0) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = lbl_8047BCA8 * *(f32*)((u8*)&lbl_803A6818 + 0x40);
    f0 = lbl_8047BCA8 * f0;
    f1 = lbl_8047BCA4 * f0 + lbl_8047BCA4;
    f1 = (f32)sin(f1);
    f3 = f1;
    sprite->field_52 = (s16)(lbl_8047BD0C * f3 +
                              (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x57bc));
}
