/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_candidate_8003B814_r46_80043FA8.c
 * @brief PDA tab-caption sprite callback, 0x80043FA8 - 0x80044378.
 *
 * The body is copied from pda_range_80037158.c with its float reads written
 * as literals (255.0f, 256.0f, 128.0f, pi, 2.0f, 0.0f): retail keeps 255.0f
 * live across the switch and reloads the others per case, which only literal
 * constants give. The temporary constant_import build step redirects the
 * private literals to lbl_8047BCA0, lbl_8047BD10, lbl_8047BD14,
 * lbl_8047BCA4, lbl_8047BCA8 (sdata2_8047BCA0.c) and lbl_8047BC94
 * (sdata2_8047BBA0.c). Built with -opt nopeephole as a unit flag.
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
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */
extern PdaSceneWork lbl_803A6818;
extern void GSmsgPrintColor(s32 arg0, s32 arg1, s32 arg2, void* data);
extern u32 GSmsgGetRect(s32 id);
/* Tab captions: pulse the active page's title and centre each caption. */
void pdaModelViewDrawText(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    f32 v;
    s32 tint;

    alphaSprite->alphaByte = 255.0f * lbl_803A6818.alphaScale;
    switch (sprite->eventId) {
    case 0x6d7:
        alphaSprite->alphaByte = 255.0f * lbl_803A6818.alphaScale;
        v = 128.0f *
                (f32)sin(3.1415927f * (2.0f * lbl_803A6818.angle)) +
            256.0f;
        if (v > 255.0f) {
            v = 255.0f;
        }
        if (v < 0.0f) {
            v = 0.0f;
        }
        sprite->alpha = (u8)v;
        if (lbl_803A6818.field_28 == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x6d8:
        alphaSprite->alphaByte = 255.0f * lbl_803A6818.alphaScale;
        v = 128.0f *
                (f32)sin(3.1415927f * (2.0f * lbl_803A6818.angle)) +
            256.0f;
        if (v > 255.0f) {
            v = 255.0f;
        }
        if (v < 0.0f) {
            v = 0.0f;
        }
        sprite->alpha = (u8)v;
        if (lbl_803A6818.field_28 == 1) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x6d9:
        alphaSprite->alphaByte = 255.0f * lbl_803A6818.alphaScale;
        v = 128.0f *
                (f32)sin(3.1415927f * (2.0f * lbl_803A6818.angle)) +
            256.0f;
        if (v > 255.0f) {
            v = 255.0f;
        }
        if (v < 0.0f) {
            v = 0.0f;
        }
        sprite->alpha = (u8)v;
        if (lbl_803A6818.field_28 == 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0x10c7:
        if (lbl_803A6818.field_28 == 0) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x3717) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3717);
        break;
    case 0x10c8:
        if (lbl_803A6818.field_28 == 1) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x3718) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3718);
        break;
    case 0x10c9:
        if (lbl_803A6818.field_28 == 2) {
            tint = 0x140000;
        } else {
            tint = 0x4a0000;
        }
        GSmsgPrintColor((s16)((sprite->x - (s16)(GSmsgGetRect(0x3719) >> 16)) / 2), 0,
                    tint | alphaSprite->alphaByte, (void*)0x3719);
        break;
    case 0xfbf:
        break;
    default:
        break;
    }
}
