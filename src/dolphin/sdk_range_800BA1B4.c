/**
 * @file sdk_range_800BA1B4.c
 * @brief dolphin-sdk code, 0x800BA1B4 - 0x800BA414 (2 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef struct GXLightObj_800BA344 {
    u8 pad_00[0x10];
    f32 a0;
    f32 a1;
    f32 a2;
    f32 k0;
    f32 k1;
    f32 k2;
} GXLightObj_800BA344;

typedef enum GXSpotFn_800BA1B4 {
    GX_SP_OFF_800BA1B4,
    GX_SP_FLAT_800BA1B4,
    GX_SP_COS_800BA1B4,
    GX_SP_COS2_800BA1B4,
    GX_SP_SHARP_800BA1B4,
    GX_SP_RING1_800BA1B4,
    GX_SP_RING2_800BA1B4,
} GXSpotFn_800BA1B4;

typedef enum GXDistAttnFn_800BA344 {
    GX_DA_OFF_800BA344,
    GX_DA_GENTLE_800BA344,
    GX_DA_MEDIUM_800BA344,
    GX_DA_STEEP_800BA344,
} GXDistAttnFn_800BA344;

extern f32 cosf(f32 value);

void GXInitLightSpot(GXLightObj_800BA344* light, f32 cutoff,
                     GXSpotFn_800BA1B4 spot_fn)
{
    f32 a0;
    f32 a1;
    f32 a2;
    f32 angle;
    f32 distance;
    f32 cosine;

    if (cutoff <= 0.0F || cutoff > 90.0F) {
        spot_fn = GX_SP_OFF_800BA1B4;
    }
    angle = cutoff * 3.14159265358979323846F / 180.0F;
    cosine = cosf(angle);

    switch (spot_fn) {
    case GX_SP_FLAT_800BA1B4:
        a0 = -1000.0F * cosine;
        a1 = 1000.0F;
        a2 = 0.0F;
        break;
    case GX_SP_COS_800BA1B4:
        a1 = 1.0F / (1.0F - cosine);
        a0 = -cosine * a1;
        a2 = 0.0F;
        break;
    case GX_SP_COS2_800BA1B4:
        a2 = 1.0F / (1.0F - cosine);
        a0 = 0.0F;
        a1 = -cosine * a2;
        break;
    case GX_SP_SHARP_800BA1B4:
        distance = 1.0F / ((1.0F - cosine) * (1.0F - cosine));
        a0 = cosine * (cosine - 2.0F) * distance;
        a1 = 2.0F * distance;
        a2 = -distance;
        break;
    case GX_SP_RING1_800BA1B4:
        distance = 1.0F / ((1.0F - cosine) * (1.0F - cosine));
        a2 = -4.0F * distance;
        a0 = a2 * cosine;
        a1 = 4.0F * (1.0F + cosine) * distance;
        break;
    case GX_SP_RING2_800BA1B4:
        distance = 1.0F / ((1.0F - cosine) * (1.0F - cosine));
        a0 = 1.0F - 2.0F * cosine * cosine * distance;
        a1 = 4.0F * cosine * distance;
        a2 = -2.0F * distance;
        break;
    case GX_SP_OFF_800BA1B4:
    default:
        a0 = 1.0F;
        a1 = 0.0F;
        a2 = 0.0F;
        break;
    }
    light->a0 = a0;
    light->a1 = a1;
    light->a2 = a2;
}

void GXInitLightDistAttn(GXLightObj_800BA344* light, f32 ref_dist,
                         f32 ref_br, GXDistAttnFn_800BA344 dist_fn)
{
    f32 k0;
    f32 k1;
    f32 k2;

    if (ref_dist < 0.0f) {
        dist_fn = GX_DA_OFF_800BA344;
    }
    if (ref_br <= 0.0f || ref_br >= 1.0f) {
        dist_fn = GX_DA_OFF_800BA344;
    }

    switch (dist_fn) {
    case GX_DA_GENTLE_800BA344:
        k0 = 1.0f;
        k1 = (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.0f;
        break;
    case GX_DA_MEDIUM_800BA344:
        k0 = 1.0f;
        k1 = 0.5f * (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.5f * (1.0f - ref_br) /
             (ref_br * ref_dist * ref_dist);
        break;
    case GX_DA_STEEP_800BA344:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = (1.0f - ref_br) / (ref_br * ref_dist * ref_dist);
        break;
    case GX_DA_OFF_800BA344:
    default:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = 0.0f;
        break;
    }

    light->k0 = k0;
    light->k1 = k1;
    light->k2 = k2;
}
