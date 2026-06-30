#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"

/*
 * GXLight.c - GX lighting functions.
 *
 * Contains light object initialization and loading functions.
 *
 * Part of the 0x800B5E8C - 0x800BE348 GX block.
 */

#define WGPIPE_U8  (*(volatile u8*)0xCC008000)
#define WGPIPE_U32 (*(volatile u32*)0xCC008000)
#define WGPIPE_F32 (*(volatile f32*)0xCC008000)

typedef struct GXLightObj {
    u32 _pad[3];
    u32 color;
    f32 a0, a1, a2;
    f32 k0, k1, k2;
    f32 px, py, pz;
    f32 dx, dy, dz;
} GXLightObj;

/*
 * GXInitLightAttn - Initialize light attenuation.
 */
void GXInitLightAttn(GXLightObj* light, f32 a0, f32 a1, f32 a2,
                     f32 k0, f32 k1, f32 k2) {
    light->a0 = a0;
    light->a1 = a1;
    light->a2 = a2;
    light->k0 = k0;
    light->k1 = k1;
    light->k2 = k2;
}

/*
 * GXInitLightSpot - Initialize spotlight parameters.
 */
void GXInitLightSpot(GXLightObj* light, f32 cutoff, u8 spotFn) {
    /* Calculate spotlight attenuation coefficients */
    if (cutoff <= 0.0f || cutoff > 90.0f) {
        light->a0 = 1.0f;
        light->a1 = 0.0f;
        light->a2 = 0.0f;
        return;
    }

    /* Compute based on spot function type */
    switch (spotFn) {
        case 0: /* Flat */
            light->a0 = -1000.0f * cutoff;
            light->a1 = 1000.0f;
            light->a2 = 0.0f;
            break;
        case 1: /* Cos */
            light->a0 = -cutoff;
            light->a1 = 1.0f;
            light->a2 = 0.0f;
            break;
        case 2: /* Cos^2 */
            light->a0 = 0.0f;
            light->a1 = -cutoff;
            light->a2 = 1.0f;
            break;
        default:
            light->a0 = 1.0f;
            light->a1 = 0.0f;
            light->a2 = 0.0f;
            break;
    }
}

/*
 * GXInitLightDistAttn - Initialize distance attenuation.
 */
void GXInitLightDistAttn(GXLightObj* light, f32 refDist, f32 refBright,
                         u8 distFn) {
    if (refDist == 0.0f || refBright <= 0.0f || refBright >= 1.0f) {
        light->k0 = 1.0f;
        light->k1 = 0.0f;
        light->k2 = 0.0f;
        return;
    }

    switch (distFn) {
        case 0: /* Gentle */
            light->k0 = 1.0f;
            light->k1 = (1.0f - refBright) / (refBright * refDist);
            light->k2 = 0.0f;
            break;
        case 1: /* Medium */
            light->k0 = 1.0f;
            light->k1 = 0.5f * (1.0f - refBright) / (refBright * refDist);
            light->k2 = 0.5f * (1.0f - refBright) / (refBright * refDist * refDist);
            break;
        case 2: /* Steep */
            light->k0 = 1.0f;
            light->k1 = 0.0f;
            light->k2 = (1.0f - refBright) / (refBright * refDist * refDist);
            break;
        default:
            light->k0 = 1.0f;
            light->k1 = 0.0f;
            light->k2 = 0.0f;
            break;
    }
}

/*
 * GXInitLightPos - Set light position.
 */
void GXInitLightPos(GXLightObj* light, f32 x, f32 y, f32 z) {
    light->px = x;
    light->py = y;
    light->pz = z;
}

/*
 * GXInitLightDir - Set light direction.
 */
void GXInitLightDir(GXLightObj* light, f32 x, f32 y, f32 z) {
    light->dx = -x;
    light->dy = -y;
    light->dz = -z;
}

/*
 * GXInitLightColor - Set light color.
 */
void GXInitLightColor(GXLightObj* light, u32 color) {
    light->color = color;
}

/*
 * GXLoadLightObjImm - Load a light object into hardware.
 */
void GXLoadLightObjImm(GXLightObj* light, u32 lightID) {
    u32 addr;
    u32 i;
    u32* data;

    /* XF light address = 0x600 + lightID * 0x10 */
    addr = 0x600 + lightID * 0x10;
    data = (u32*)light;

    WGPIPE_U8 = 0x10;
    WGPIPE_U32 = 0x000F;  /* 16-1 values */
    WGPIPE_U32 = addr;

    for (i = 0; i < 16; i++) {
        WGPIPE_U32 = data[i];
    }
}

/*
 * GXSetNumIndStages - Set number of indirect texture stages.
 */
void GXSetNumIndStages(u8 nStages) {
    /* Write BP register for indirect stages */
}

/*
 * GXSetTevDirect - Set a TEV stage to direct mode (no indirect).
 */
void GXSetTevDirect(u8 stage) {
    /* Write BP register to disable indirect for this stage */
}
