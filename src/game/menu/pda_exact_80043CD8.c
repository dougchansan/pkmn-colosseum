#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[4];
    s8 flags;
    u8 pad05[0x4d];
    s16 field_52;
    u8 pad54[0x37];
    u8 alphaByte;
} PdaSprite;

typedef struct PdaSceneWork {
    u8 pad00[0x10];
    s32 field_10;
} PdaSceneWork;

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_802EF0A8[];
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BCF4;

#pragma push
#pragma peephole off
/* RULE-EXCEPTION(user-approved): allocation-shaping temporaries preserve the retail conversion spill order. */
void fn_80043CD8(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    f32 numerator;
    f32 negated;
    u8* layoutEntry;
    u8* layoutValue;
    u8* layoutData;
    f32 f0;
    s32 count_m1;
    f32 f3;
    s16 stored;

    alphaSprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    if (lbl_803A6818.field_10 > 10) {
        sprite->flags = sprite->flags | 2;
        do {
        } while (0);
    } else {
        sprite->flags = sprite->flags & ~2;
        if (1 != 0) {
            return;
        }
    }
    layoutData = lbl_802EF0A8;
    f0 = *(f32*)((u8*)&lbl_803A6818 + 0x30);
    negated = -f0;
    layoutValue = (layoutEntry = lbl_802EF0A8 + 0x5788);
    count_m1 = lbl_803A6818.field_10 - 1;
    numerator = negated;
    f3 = numerator / (f32)count_m1;
    f0 = lbl_8047BCF4;
    sprite->field_52 =
        (s16)((f3 / f0) * (stored = *(s16*)layoutValue) +
              *(s16*)(layoutData + 0x5784));
}
#pragma pop
