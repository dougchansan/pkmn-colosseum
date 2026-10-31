/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80041E48.c
 * @brief PDA summary-page per-frame update, 0x80041E48 - 0x80042658.
 *
 * pdaMemoUpdateScroll is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool.
 */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0x244];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_803A67FC[];
extern u8 lbl_804788C4;
extern u32 soundStartRaw(u32, u32, u32);

#define G(off) (*(f32*)((u8*)&lbl_803A6818 + (off)))

static inline f32 pdaAbsF(f32 x)
{
    return x > 0.0f ? x : -x;
}

/* Ease a value toward its target, snapping once a step would overshoot. */
static inline void pdaEase(f32* cur, f32* dstp)
{
    f32 c;
    f32 dst;
    f32 delta;
    f32 rate;
    f32 moved;
    f32 next;
    s8 mode;

    c = *cur;
    dst = *dstp;
    if (c == dst) {
        return;
    }
    delta = dst - c;
    rate = 10.0f * G(0x3c);
    if ((delta > 0.0f ? delta : -delta) >= 243.0f) {
        *cur = dst;
        return;
    }
    mode = *(s8*)((u8*)&lbl_803A6818 + 0x48);
    if (mode == 1) {
        rate = rate * (5.0f / (f32)(s8)lbl_804788C4);
    }
    if (mode == 2) {
        rate = rate * (5.0f / (f32)(s8)lbl_804788C4);
    }
    moved = delta * rate;
    next = c + moved;
    /* RULE-EXCEPTION(user-approved): scalar reuse - the dead rate holds the remaining distance (pdaMemoUpdateScroll) - see docs/RULE_EXCEPTIONS.md */
    rate = dst - next;
    *cur = next;
    if (pdaAbsF(rate) <= pdaAbsF(moved)) {
        *cur = dst;
    }
}

/* pdaEase for the filter list's scroll offset: retail allocates its start
 * value after the other locals. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pdaEase2 (pdaMemoUpdateScroll) - see docs/RULE_EXCEPTIONS.md */
static inline void pdaEase2(f32* cur, f32* dstp)
{
    f32 dst;
    f32 delta;
    f32 rate;
    f32 moved;
    f32 next;
    s8 mode;
    f32 c;

    c = *cur;
    dst = *dstp;
    if (c == dst) {
        return;
    }
    delta = dst - c;
    rate = 10.0f * G(0x3c);
    if ((delta > 0.0f ? delta : -delta) >= 243.0f) {
        *cur = dst;
        return;
    }
    mode = *(s8*)((u8*)&lbl_803A6818 + 0x48);
    if (mode == 1) {
        rate = rate * (5.0f / (f32)(s8)lbl_804788C4);
    }
    if (mode == 2) {
        rate = rate * (5.0f / (f32)(s8)lbl_804788C4);
    }
    moved = delta * rate;
    next = c + moved;
    /* RULE-EXCEPTION(user-approved): scalar reuse - the dead rate holds the remaining distance (pdaMemoUpdateScroll) - see docs/RULE_EXCEPTIONS.md */
    rate = dst - next;
    *cur = next;
    if (pdaAbsF(rate) <= pdaAbsF(moved)) {
        *cur = dst;
    }
}

/* RULE-EXCEPTION(user-approved): reconstructed inline helpers pdaAbsF/pdaApproach (fn_80040018, pdaMemoUpdateScroll, fn_80044630) - see docs/RULE_EXCEPTIONS.md */
static inline void pdaApproach(f32* current, f32* target, f32 delta)
{
    f32 step;
    f32 distance;
    f32 scaled;

    step = *target - *current;
    scaled = 10.0f * step;
    step = scaled * delta;
    if (step > 20.0f) {
        step = 20.0f;
    }
    if (step <= -20.0f) {
        step = -20.0f;
    }
    distance = *target - (*current += step);
    if (pdaAbsF(distance) <= pdaAbsF(step) || pdaAbsF(distance) < 1.0f) {
        *current = *target;
    }
}

/* Per-frame scene animation and D-pad handling for the PDA summary page. */
s32 pdaMemoUpdateScroll(void* work, s32 mode)
{
    extern u32 GSgetFrameStep(void);
    extern s32 GSgfxGetVsyncRate(void);
    u8* S = (u8*)&lbl_803A6818;
    u16 keys;
    f32 dt;
    f32 target;
    f32 current;
    f32 step;
    f32 next;

    (void)work;
    (void)mode;
    /* RULE-EXCEPTION(user-approved): codegen-only mask - & 0xffff on the u16 key read gives retail's clrlwi before each key test (pdaMemoUpdateScroll) - see docs/RULE_EXCEPTIONS.md */
    keys = *(u16*)(lbl_803A67FC + 6) & 0xffff;
    dt = (f32)GSgetFrameStep() / (f32)GSgfxGetVsyncRate();
    G(0x3c) = dt;
    G(0x40) = G(0x40) + dt;
    if (G(0x40) >= 1.0f) {
        G(0x40) = 0.0f;
    }
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x2c),
            (f32*)((u8*)&lbl_803A6818 + 0x34));
    pdaEase((f32*)((u8*)&lbl_803A6818 + 0x30),
            (f32*)((u8*)&lbl_803A6818 + 0x38));
    pdaEase2((f32*)(S + 0x1d0), (f32*)(S + 0x1d8));
    target = G(0x50);
    current = G(0x4C);
    if (target != current) {
        step = G(0x3C) / 0.25f;
        if (target > current) {
            next = current + step;
            G(0x4C) = next;
            if (next > 1.0f) {
                G(0x4C) = 1.0f;
            }
        } else {
            next = current - step;
            G(0x4C) = next;
            if (next < 0.0f) {
                G(0x4C) = 0.0f;
            }
        }
    }

    target = G(0x58);
    current = G(0x54);
    if (target != current) {
        step = G(0x3C) / 0.25f;
        if (target > current) {
            next = current + step;
            G(0x54) = next;
            if (next > 1.0f) {
                G(0x54) = 1.0f;
            }
        } else {
            next = current - step;
            G(0x54) = next;
            if (next < 0.0f) {
                G(0x54) = 0.0f;
            }
        }
    }

    target = G(0x60);
    current = G(0x5C);
    if (target != current) {
        step = G(0x3C) / 0.8f;
        if (target > current) {
            next = current + step;
            G(0x5C) = next;
            if (next > 1.0f) {
                G(0x5C) = 1.0f;
            }
        } else {
            next = current - step;
            G(0x5C) = next;
            if (next < 0.0f) {
                G(0x5C) = 0.0f;
            }
        }
    }

    if (*(f32*)(S + 0x1dc) != *(f32*)(S + 0x1e0)) {
        pdaApproach((f32*)(S + 0x1dc), (f32*)(S + 0x1e0), G(0x3C));
    }

    if (*(s32*)(S + 0x154) == 0) {
        switch (*(s32*)(S + 0x1e4)) {
        case 0:
            if (*(s8*)(S + 0x15d) == 2) {
                if ((keys & 0x4) != 0) {
                    if (*(s8*)(S + 0x15e) != 0) {
                        soundStartRaw(0x23, 0, 0);
                    }
                    *(s8*)(S + 0x15e) = 0;
                } else if ((keys & 0x8) != 0) {
                    if (*(s8*)(S + 0x15e) == 0) {
                        soundStartRaw(0x23, 0, 0);
                    }
                    *(s8*)(S + 0x15e) = 1;
                }
            } else {
                *(s8*)(S + 0x15e) = 0;
            }
            if ((keys & 0x2) != 0) {
                if (*(s8*)(S + 0x15d) < 4) {
                    (*(s8*)(S + 0x15d))++;
                    soundStartRaw(0x23, 0, 0);
                }
            } else if ((keys & 0x1) != 0) {
                if (*(s8*)(S + 0x15d) > 0) {
                    (*(s8*)(S + 0x15d))--;
                    soundStartRaw(0x23, 0, 0);
                }
            }
            break;
        }
    } else if ((keys & 0x2) != 0) {
        if (*(s8*)(S + 0x15f) < *(s8*)(S + 0x160) - 1) {
            (*(s8*)(S + 0x15f))++;
            soundStartRaw(0x23, 0, 0);
        }
        if (*(s8*)(S + 0x15f) >= *(s8*)(S + 0x1c9)) {
            *(f32*)(S + 0x1d8) = *(f32*)(S + 0x1d8) - 29.0f;
            (*(s8*)(S + 0x1c9))++;
            (*(s8*)(S + 0x1c8))++;
        }
    } else if ((keys & 0x1) != 0) {
        if (*(s8*)(S + 0x15f) > 0) {
            (*(s8*)(S + 0x15f))--;
            soundStartRaw(0x23, 0, 0);
        }
        if (*(s8*)(S + 0x15f) < *(s8*)(S + 0x1c8)) {
            *(f32*)(S + 0x1d8) += 29.0f;
            (*(s8*)(S + 0x1c8))--;
            (*(s8*)(S + 0x1c9))--;
        }
    }
    return 0;
}
#undef G
