/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_800439BC.c
 * @brief PDA stick/button sampling with key repeat, 0x800439BC - 0x80043CD8.
 *
 * fn_800439BC is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool. The key-repeat counters at work + 0xA are signed
 * bytes (as is the repeat rate lbl_804788C4); the decrement is read into an
 * s8 before it is subtracted, which gives retail's extsb on the store.
 */
#include "dolphin/types.h"

extern s8 lbl_804788C4;

/* Absolute value; retail reloads the 0.0f literal at every compare. */
static inline f32 pda_800439BC_Fabs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

/* Sample the stick and buttons into the PDA's own key-repeat state. */
void pdaReadPadInput(void* scene)
{
    extern s8 GSinputGetLeftStickYData(s32 chan, s32 index);
    extern s8 GSinputGetLeftStickXData(s32 chan, s32 index);
    extern u32 GSinputGetButtons(s32 chan);
    extern s8 GSgetFrameStep(void);
    extern f64 atan2(f64 y, f64 x);
    s8 x;
    s8 y;
    f32 ang;
    s32 i;
    u16 buttons;
    u16 held;
    u16 trig;
    u32 raw;
    s8* work = (s8*)scene;

    buttons = 0;
    *(u16*)(work + 2) = *(u16*)work;
    x = GSinputGetLeftStickYData(1, 0);
    y = GSinputGetLeftStickXData(1, 0);
    if ((y < 0 ? -y : y) > 0x20 || (x < 0 ? -x : x) > 0x20) {
        ang = atan2((f64)y, (f64)x);
        if (pda_800439BC_Fabs(ang) < 0.9599311f) {
            buttons |= 2;
        } else if (pda_800439BC_Fabs(ang) > 2.1816616f) {
            buttons |= 1;
        }
        if (0.61086524f < pda_800439BC_Fabs(ang) && pda_800439BC_Fabs(ang) < 2.5307274f) {
            if (ang < 0.0f) {
                buttons |= 4;
            } else {
                buttons |= 8;
            }
        }
    }
    raw = GSinputGetButtons(1);
    if ((raw & 0x8) != 0) {
        buttons |= 1;
    }
    if ((raw & 0x4) != 0) {
        buttons |= 2;
    }
    if ((raw & 0x1) != 0) {
        buttons |= 4;
    }
    if ((raw & 0x2) != 0) {
        buttons |= 8;
    }
    if ((raw & 0x100) != 0) {
        buttons |= 0x10;
    }
    if ((raw & 0x200) != 0) {
        buttons |= 0x20;
    }
    if ((raw & 0x400) != 0) {
        buttons |= 0x40;
    }
    if ((raw & 0x800) != 0) {
        buttons |= 0x80;
    }
    if ((raw & 0x10) != 0) {
        buttons |= 0x100;
    }
    if ((raw & 0x40) != 0) {
        buttons |= 0x200;
    }
    if ((raw & 0x20) != 0) {
        buttons |= 0x400;
    }
    if ((raw & 0x1000) != 0) {
        buttons |= 0x800;
    }
    held = (*(u16*)(work + 2) ^ 0xffff) & buttons;
    trig = 0;
    for (i = 0; i < 16; i++) {
        u16 mask = 1 << i;
        if ((held & mask) != 0) {
            work[i + 0xa] = 15;
            trig |= mask;
        } else if ((buttons & mask) != 0) {
            s8 delta = GSgetFrameStep();
            work[i + 0xa] = work[i + 0xa] - delta;
            if (work[i + 0xa] <= 0) {
                work[i + 0xa] = lbl_804788C4;
                trig |= mask;
            }
        }
    }
    *(u16*)work = buttons;
    *(u16*)(work + 4) = held;
    *(u16*)(work + 6) = trig;
}
