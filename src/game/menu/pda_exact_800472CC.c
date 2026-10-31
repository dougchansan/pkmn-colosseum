/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_800472CC.c
 * @brief PDA orbit-point fade callback, 0x800472CC - 0x800473E0.
 *
 * The body is exact (copied from pda_range_80037158.c). Its float literals
 * 10.0f and 0.0f are redirected by the temporary constant_import build step
 * to the shared lbl_8047BCC4 (sdata2_8047BCA0.c) and lbl_8047BC94
 * (sdata2_8047BBA0.c).
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

typedef struct PdaOrbitPoint {
    f32 angle;
    f32 pad04[6];
    f32 alpha;
    f32 pad20;
} PdaOrbitPoint;

void menu9FDrawItem(PdaSprite* alphaSprite)
{
    extern PdaOrbitPoint lbl_802E52C8[8];
    extern u32 lbl_8047A4EC;
    extern f32 lbl_8047BCA0;
    extern void pdaDrawSpline(PdaOrbitPoint* point);
    s32 i;

    alphaSprite->alphaByte =
        lbl_8047BCA0 * lbl_803A6818.alphaScale;
    for (i = 0; i < 8; i++) {
        pdaDrawSpline(&lbl_802E52C8[i]);
    }
    for (i = (s32)lbl_8047A4EC - 1; i >= 0; i--) {
        lbl_802E52C8[i].alpha -= 10.0f;
        if (lbl_802E52C8[i].alpha < 0.0f) {
            lbl_802E52C8[i].alpha = 0.0f;
        }
    }
    for (i = 7; i > lbl_8047A4EC; i--) {
        lbl_802E52C8[i].alpha -= 10.0f;
        if (lbl_802E52C8[i].alpha < 0.0f) {
            lbl_802E52C8[i].alpha = 0.0f;
        }
    }
}
