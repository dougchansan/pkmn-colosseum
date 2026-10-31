#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x50];
    s16 field_50;
    s16 field_52;
} PdaSprite;

extern s8 lbl_8047A47C;
extern s8 lbl_8047A47D;
extern f32 lbl_8047A484;
extern f32 lbl_8047A488;
extern f32 lbl_8047A48C;
extern f32 lbl_8047A494;
extern f32 lbl_8047BA64;
extern f32 lbl_8047BA68;
extern f32 lbl_8047BA6C;
extern f32 lbl_8047BA70;
extern void* windowGetKeyInfo(void);
extern f64 cos(f64 angle);
extern f64 sin(f64 angle);

static inline f32 pdaOrbitAbs(f32 v)
{
    if (v > 0.0f) {
        return v;
    }
    return -v;
}

static inline f32 pdaOrbitBaseAngle(f32 baseAngle)
{
    return lbl_8047A484 - baseAngle;
}

static inline void pdaUpdateOrbitSprite(PdaSprite* sprite, f32 baseAngle,
                                        s32 updateTarget)
{
    extern f32 lbl_802E52A8[4];
    f32 angle;
    f32 roundedStep;
    f32 step;
    f32 distance;
    f32 magnitude;
    s8 direction;

    if (updateTarget) {
        lbl_8047A488 = lbl_802E52A8[lbl_8047A47C];
    }
    angle = pdaOrbitBaseAngle(baseAngle);
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
        roundedStep = (f32)(f64)step;
        lbl_8047A48C = step;
        lbl_8047A484 += roundedStep;
        if (lbl_8047A484 >= 6.28318548f) {
            lbl_8047A484 -= 6.28318548f;
        }
        if (lbl_8047A484 < 0.0f) {
            lbl_8047A484 += 6.28318548f;
        }

        distance = pdaOrbitAbs(lbl_8047A484 - lbl_8047A488);
        magnitude = pdaOrbitAbs(roundedStep);
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

#ifndef PDA_ORBIT_ONLY_FOURTH
void menu20DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[3], 0);
}

void menu22DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[2], 0);
}

void menu21DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[1], 0);
}
#endif

#ifdef PDA_ORBIT_INCLUDE_FOURTH
void menu23DrawAnim(void* window, PdaSprite* sprite)
{
    extern f32 lbl_802E52B8[4];
    (void)window;
    pdaUpdateOrbitSprite(sprite, lbl_802E52B8[0], 1);
}
#endif
