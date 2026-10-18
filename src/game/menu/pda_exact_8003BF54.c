#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[4];
    s8 flags;
    u8 pad05[0x4d];
    s16 field_52;
    u8 pad54[0x37];
    u8 alphaByte;
} PdaSprite;

typedef struct PdaEvent PdaEvent;

typedef struct PdaModelWindow {
    u8 pad00[0x8];
    s32 field_08;
    s32 field_0C;
    s32 field_10;
    u8 pad14[4];
    f32 field_18;
    u8 pad1C[0xc];
    f32 field_28;
    u8 pad2C[0x18];
    f32 alphaScale;
} PdaModelWindow;

extern PdaModelWindow lbl_803A6748;
extern f32 lbl_8047BAC0;

void fn_8003BF54(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BAD8;
    u8* layoutData;
    s16* baseY;
    f32 offset;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    if (lbl_803A6748.field_10 > 10) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    baseY = (s16*)((layoutData = lbl_802EF0A8) + 0x5084);
    offset = lbl_803A6748.field_18 /
             -(lbl_8047BAD8 * (f32)(lbl_803A6748.field_10 - 10));
    sprite->field_52 =
        (s16)(*baseY + offset * *(s16*)(layoutData + 0x5088));
}

void fn_8003C03C(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BAF0;
    extern f32 lbl_8047BAF4;
    volatile int pad;
    extern f64 sin(f64 angle);
    f32 f0;
    f32 f1;
    f32 f3;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    if (lbl_803A6748.field_0C >= 10) {
        if (lbl_803A6748.field_10 != lbl_803A6748.field_0C) {
            sprite->flags = sprite->flags | 2;
        } else {
            sprite->flags = sprite->flags & ~2;
            return;
        }
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = lbl_8047BAF4 * lbl_803A6748.field_28;
    f0 = lbl_8047BAF4 * f0;
    if (lbl_8047BAF0 && lbl_8047BAF0) {
        /* Preserve MWCC register allocation. */
    }
    f1 = lbl_8047BAF0 * f0;
    f1 = (f32)sin(f1);
    f3 = f1;
    sprite->field_52 = (s16)(lbl_8047BAF4 * f3 +
                              (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x504C));
}

void fn_8003C13C(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u8 lbl_802EF0A8[];
    extern f32 lbl_8047BAF0;
    extern f32 lbl_8047BAF4;
    extern f64 sin(f64 angle);
    f32 f0;
    f32 f1;
    f32 f3;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    if (lbl_803A6748.field_08 > 0) {
        sprite->flags = sprite->flags | 2;
    } else {
        sprite->flags = sprite->flags & ~2;
        return;
    }
    f0 = lbl_8047BAF4 * lbl_803A6748.field_28;
    f0 = lbl_8047BAF4 * f0;
    f1 = lbl_8047BAF0 * f0 + lbl_8047BAF0;
    f1 = (f32)sin(f1);
    f3 = f1;
    sprite->field_52 = (s16)(lbl_8047BAF4 * f3 +
                              (f32)(s32)*(s16*)(lbl_802EF0A8 + 0x5014));
}

void fn_8003C21C(PdaSprite* sprite, PdaEvent* event)
{
    (void)event;
    sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
}
