#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x52];
    s16 field_52;
    u8 pad54[0x37];
    u8 alphaByte;
} PdaSprite;

typedef struct PdaModelWindow {
    s32 field_00;
    u8 pad04[4];
    s32 field_08;
    u8 pad0C[0x38];
    f32 alphaScale;
} PdaModelWindow;

extern PdaModelWindow lbl_803A6748;
extern f32 lbl_8047BAC0;
extern u8 lbl_802EF0A8[];

#pragma peephole off
void fn_8003C728(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    s32 row;
    s16 baseY;

    alphaSprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
    baseY = *(s16*)(lbl_802EF0A8 + 0x50a0);
    row = *(s32*)&lbl_803A6748 - lbl_803A6748.field_08;
    if (row >= 10) {
        row = 9;
    }
    *(f32*)((u8*)&lbl_803A6748 + 0x38) = (f32)(baseY + row * 0x18);
    sprite->field_52 = *(f32*)((u8*)&lbl_803A6748 + 0x30);
}
#pragma peephole reset
