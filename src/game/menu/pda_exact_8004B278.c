/**
 * @file pda_exact_8004B278.c
 * @brief PDA mail screen per-frame update, 0x8004B278 - 0x8004B598:
 *        pdaTopMenuCtrl measures the frame time, eases the widgets' horizontal
 *        and vertical offsets toward their targets and advances the
 *        background scroll.
 *        Copied from pda_range_80037158.c.
 */
#include "dolphin/types.h"

/* Mail-screen state (PdaMailSceneState in menu_pda_mail.c): +0x08 frame time,
 * +0x30/+0x34 widget x/y offsets, +0x38/+0x3C their targets, +0x40 pulse phase. */
extern u8 lbl_803A6A60[];

extern u8 std081_flagCheckTrue(s32 id);
extern void menuItemBiosSetSelectFlag(s32 id, s32 flag);
extern u32 GSgetFrameStep(void);
extern s32 GSgfxGetVsyncRate(void);

/* Ease the widgets' horizontal offset toward its target: 10 x the gap per
 * second, at most 20 per frame, snapping once the remaining gap is within the
 * step or under 1. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8004B278_EaseX in this standalone copy (pdaTopMenuCtrl) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8004B278_EaseX(f32* cur, f32* target)
{
    f32 tgt;
    f32 delta;
    f32 rest;
    f32 moved;

    if (*cur != *target) {
        delta = *target - *cur;
        delta = 10.0f * delta;
        moved = delta * *(f32*)((u8*)&lbl_803A6A60 + 8);
        if (moved > 20.0f) {
            moved = 20.0f;
        }
        if (moved <= -20.0f) {
            moved = -20.0f;
        }
        *cur += moved;
        tgt = *target;
        rest = tgt - *cur;
        moved = moved > 0.0f ? moved : -moved;
        if ((rest > 0.0f ? rest : -rest) <= moved || (rest > 0.0f ? rest : -rest) < 1.0f) {
            *cur = tgt;
        }
    }
}

/* The same easing for the vertical offset. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8004B278_EaseY in this standalone copy (pdaTopMenuCtrl) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8004B278_EaseY(f32* cur, f32* target)
{
    f32 delta;
    f32 rest;
    f32 moved;
    f32 speed;
    f32 tgt;

    if (*cur != *target) {
        delta = *target - *cur;
        delta = 10.0f * delta;
        moved = delta * *(f32*)((u8*)&lbl_803A6A60 + 8);
        if (moved > 20.0f) {
            moved = 20.0f;
        }
        if (moved <= -20.0f) {
            moved = -20.0f;
        }
        *cur += moved;
        tgt = *target;
        rest = tgt - *cur;
        if (moved > 0.0f) {
            speed = moved;
        } else {
            speed = -moved;
        }
        if ((rest > 0.0f ? rest : -rest) <= speed || (rest > 0.0f ? rest : -rest) < 1.0f) {
            *cur = tgt;
        }
    }
}

/* Per-frame timing, offset easing and background scroll for the mail screen. */
s32 pdaTopMenuCtrl(u8* work)
{
    u8* data;

    data = *(u8**)(work + 0x60);
    *(f32*)((u8*)&lbl_803A6A60 + 8) = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    if (std081_flagCheckTrue(0x3f0) != 0) {
        menuItemBiosSetSelectFlag(0x438, 1);
    } else {
        menuItemBiosSetSelectFlag(0x438, 0);
    }
    *(f32*)(lbl_803A6A60 + 0x40) =
        0.5f * *(f32*)((u8*)&lbl_803A6A60 + 8) + *(f32*)(lbl_803A6A60 + 0x40);
    if (*(f32*)(lbl_803A6A60 + 0x40) > 1.0f) {
        *(f32*)(lbl_803A6A60 + 0x40) = 0.0f;
    }

    pda_8004B278_EaseX((f32*)(lbl_803A6A60 + 0x30), (f32*)(lbl_803A6A60 + 0x38));
    pda_8004B278_EaseY((f32*)(lbl_803A6A60 + 0x34), (f32*)(lbl_803A6A60 + 0x3c));

    switch (*(s8*)(work + 1)) {
    case 0:
        if (*(s8*)(work + 2) == 0) {
            **(f32**)(data + 8) = 0.0f;
            work[2] = 1;
        }
        break;
    case 2:
        if ((**(f32**)(data + 8) += 0.003125f) >= 1.0f) {
            **(f32**)(data + 8) -= 1.0f;
        }
        break;
    case 3:
        if (*(s8*)(work + 2) == 0) {
            work[2] = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
