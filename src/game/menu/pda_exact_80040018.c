/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_80040018.c
 * @brief PDA summary-page fades and filter-menu buttons, 0x80040018 - 0x80040308.
 *
 * fn_80040018 is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool.
 */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0x244];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern s32 lbl_804788C0;
extern u32 soundStartRaw(u32, u32, u32);
extern void* menuDataBiosGetPtr(s32 id);
extern void* windowGetKeyInfo(void* window);

#define G(off) (*(f32*)((u8*)&lbl_803A6818 + (off)))
#define GI(off) (*(s32*)((u8*)&lbl_803A6818 + (off)))

static inline f32 pdaAbsF(f32 x)
{
    return x > 0.0f ? x : -x;
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

/* Summary-page fades, list scroll and the filter menu's sort buttons. */
void pdaUpdateFadeAndSelect(u8* menu)
{
    f32 target;
    f32 current;
    f32 step;
    f32 next;

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

    if (G(0x1DC) != G(0x1E0)) {
        pdaApproach(&G(0x1DC), &G(0x1E0), G(0x3C));
    }

    if (G(0x50) == G(0x4C) && menu != 0) {
        u8* keyInfo;
        u16 buttons;

        keyInfo = (u8*)windowGetKeyInfo(
            menuDataBiosGetPtr(*(s32*)(menu + 4)));
        /* RULE-EXCEPTION(user-approved): codegen-only mask - & 0xffff on the u16 button read gives retail's clrlwi copies before the button tests (fn_80040018) - see docs/RULE_EXCEPTIONS.md */
        buttons = *(u16*)(keyInfo + 4) & 0xffff;
        if (lbl_804788C0 != 0) {
            if (buttons & 0x10) {
                soundStartRaw(0x24, 0, 0);
                menu[0x98] = 1;
                GI(0x18) = 0;
            }
            if (buttons & 0x40) {
                soundStartRaw(0x24, 0, 0);
                menu[0x98] = 1;
                GI(0x18) = 1;
            }
        }
        if (buttons & 0x20) {
            soundStartRaw(0x25, 0, 0);
            GI(0x18) = -1;
            menu[0x98] = 1;
            menu[0x99] = 1;
        }
    }
}
#undef G
#undef GI
