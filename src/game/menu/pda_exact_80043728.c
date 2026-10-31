/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80043728.c
 * @brief PDA memo-list cursor handling, 0x80043728 - 0x800439BC.
 *
 * pdaMemoListKey is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool.
 */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x234];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern u16 lbl_8047A4E8;
extern u32 soundStartRaw(u32, u32, u32);

#define SI(off) (*(s32*)((u8*)&lbl_803A6818 + (off)))
#define SF(off) (*(f32*)((u8*)&lbl_803A6818 + (off)))

/* Move the memo-list cursor by one entry or one page of ten. */
u32 pdaMemoListKey(u32 unused, s32 mode, u16 buttons)
{
    u8* scene;
    u16 total;
    s32 selected;
    s32 next;

    SI(0x1C) = 2;
    scene = (u8*)&lbl_803A6818;
    if (buttons & 2) {
        if (lbl_8047A4E8 != 0 && *(s32*)scene < lbl_8047A4E8 - 1) {
            *(s32*)scene += 1;
            soundStartRaw(0x23, 0, 0);
            SF(0x38) -= 27.0f;
            SI(8) += 1;
            SI(0xC) += 1;
        }
    } else if (buttons & 1) {
        if (lbl_8047A4E8 != 0 && *(s32*)scene > 0) {
            *(s32*)scene -= 1;
            soundStartRaw(0x23, 0, 0);
            SF(0x38) += 27.0f;
            SI(8) -= 1;
            SI(0xC) -= 1;
        }
    } else if (buttons & 8) {
        if (mode == 0 && lbl_8047A4E8 != 0) {
            selected = *(s32*)scene;
            next = selected + 10;
            *(s32*)scene = next;
            if (next > lbl_8047A4E8 - 1) {
                next = lbl_8047A4E8 - 1;
                *(f32*)(scene + 0x38) = -(f32)(next * 27);
                *(s32*)scene = next;
                if (selected != next) {
                    *(s32*)(scene + 8) += next - selected;
                    *(s32*)(scene + 0xC) += next - selected;
                }
            } else {
                *(f32*)(scene + 0x38) -= 270.0f;
                *(s32*)(scene + 8) += 10;
                *(s32*)(scene + 0xC) += 10;
                soundStartRaw(0x23, 0, 0);
            }
        }
    } else if (buttons & 4) {
        if (mode == 0 && (total = lbl_8047A4E8) != 0) {
            selected = *(s32*)scene;
            next = selected - 10;
            *(s32*)scene = next;
            if (next < 0) {
                next = 0;
                *(s32*)scene = next;
                *(f32*)(scene + 0x38) = 0.0f;
                if (selected != *(s32*)scene) {
                    *(s32*)(scene + 8) = -5;
                    if (total >= 5) {
                        *(s32*)(scene + 0xC) = 5;
                    } else {
                        *(s32*)(scene + 0xC) = total;
                    }
                    lbl_803A6818.field_10 = total;
                }
            } else {
                *(f32*)(scene + 0x38) += 270.0f;
                *(s32*)(scene + 8) -= 10;
                *(s32*)(scene + 0xC) -= 10;
                soundStartRaw(0x23, 0, 0);
            }
        }
    }
    return 0;
}
