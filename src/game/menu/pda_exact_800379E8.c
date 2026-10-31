/**
 * @file pda_exact_800379E8.c
 * @brief PDA orbit-sprite callbacks, 0x800379E8 - 0x80037F40.
 *
 * Function-boundary carve of pda_range_80037158.c: menu20DrawAnim,
 * menu22DrawAnim and menu21DrawAnim all inline pdaUpdateOrbitSprite without a
 * target update. The literals 0.0f and 2pi resolve to the shared
 * lbl_8047BA58/lbl_8047BA60 through configure.py's constant_import.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x50];
    s16 field_50;
    s16 field_52;
} PdaSprite;

typedef struct PdaKeyInfo PdaKeyInfo;

extern f32 lbl_8047A484;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern f32 lbl_8047A494;
extern s8 lbl_8047A47C;
extern s8 lbl_8047A47D;
extern f32 lbl_8047BA64;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA70;

/* |v|. Retail's shape -- `> 0` first -- is what emits the two-way
   branch rather than an inverted single one. */
static inline f32 pdaOrbitAbs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

static inline void pdaUpdateOrbitSprite(PdaSprite* sprite, f32 baseAngle,
                                        s32 updateTarget)
{
    extern f32 lbl_802E52A8[4];
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern f64 cos(f64 angle);
    extern f64 sin(f64 angle);
    f32 angle;
    f32 step;
    f32 distance;
    f32 magnitude;
    s8 direction;

    if (updateTarget) {
        lbl_8047A488 = lbl_802E52A8[lbl_8047A47C];
    }
    angle = lbl_8047A484 - baseAngle;
    if (angle < 0.0f) {
        angle += 6.28318548f;
    }
    if (angle >= 6.28318548f) {
        angle -= 6.28318548f;
    }

    windowGetKeyInfo();
    step = lbl_8047BA64 * lbl_8047A494;
    if (lbl_8047A484 != lbl_8047A488) {
        if (lbl_8047A488 - lbl_8047A484 < 0.0f) {
            step = -step;
        }
        lbl_8047A48C = step;
        /* Round-trip through double: retail rounds a separate copy (frsp)
           for the angle update while the store keeps the original. */
        step = (f64)step;
        lbl_8047A484 += step;
        if (lbl_8047A484 >= 6.28318548f) {
            lbl_8047A484 -= 6.28318548f;
        }
        if (lbl_8047A484 < 0.0f) {
            lbl_8047A484 += 6.28318548f;
        }

        distance = pdaOrbitAbs(lbl_8047A484 - lbl_8047A488);
        magnitude = pdaOrbitAbs(step);
        if (distance < magnitude) {
            lbl_8047A484 = lbl_8047A488;
            lbl_8047A48C = 0.0f;
        }
    } else if (lbl_8047A47D != lbl_8047A47C) {
        direction = lbl_8047A47D - lbl_8047A47C;
        if (direction > 0) {
            lbl_8047A48C = -step;
        } else if (direction < 0) {
            lbl_8047A48C = step;
        }
        lbl_8047A47D = lbl_8047A47C;
    }

    sprite->field_50 =
        (s16)(lbl_8047BA6C * (f32)sin(angle) + lbl_8047BA68);
    sprite->field_52 =
        (s16)(lbl_8047BA6C * (f32)cos(angle) + lbl_8047BA70);
}

#pragma peephole off
void menu20DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[3], 0);
}
#pragma peephole reset

#pragma peephole off
void menu22DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[2], 0);
}
#pragma peephole reset

#pragma peephole off
void menu21DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[1], 0);
}
#pragma peephole reset
