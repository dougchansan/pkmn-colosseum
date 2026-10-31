/**
 * @file battle_range_candidate_801ED780.c
 * @brief Battle lens-flare render callback.
 *
 * Address range: 0x801ED780 - 0x801EE034.
 *
 * fn_801ED740 registers fn_801ED780 as a GSgapp render callback. Once the
 * flare resource has been loaded (fn_801ED680) and enabled (fn_801ED640),
 * it projects the sun position (lbl_80375230) to the screen, draws the flare
 * elements along the line through the screen centre and fades a full-screen
 * glare in as the sun nears the centre.
 */

#include "dolphin/types.h"
#include "dolphin/mtx.h"

typedef struct LensFlareElement {
    u8 type;
    u8 pad01;
    u16 textureId;
    f32 position;
    f32 scale;
} LensFlareElement;

typedef struct LensFlareResource {
    u32 magic;
    u16 textureCount;
    u16 elementCount;
    u32 farDistance;
    u32 midDistance;
    u32 nearDistance;
    f32 glareScale[2]; /* [0] when fn_800D2F34 reports 2, [1] otherwise */
    u8 data[];
} LensFlareResource;

extern u8 lbl_80314958[];
extern u8 lbl_80314C78[];
extern Vec lbl_80375230;
extern void* lbl_8046D630[];
extern u8 lbl_8047B5C0;
extern u8 lbl_8047B5C1;
extern LensFlareResource* lbl_8047B5C4;
extern LensFlareElement* lbl_8047B5C8;

extern void* GScameraGetActiveCamera(void);
extern void GScameraGetPosition(void* camera, Vec* position);
extern s32 GScolsys2Sun(Vec* sun, Vec* eye);
extern s32 gfxWorldToScreen(Vec* world, Vec* screen);
extern f32 vecMag(Vec* v);
extern void vecNormalize(Vec* src, Vec* dst, f32 length);
extern void vecScale(Vec* dst, Vec* src, f32 scale);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void gfxSetOrtho(f32 left, f32 top, f32 right, f32 bottom);
extern void gfxSetBlendMode(s32, s32, s32);
extern void gfxSetUpdates(s32, s32, s32);
extern void gfxSetZMode(s32, s32, s32);
extern void gfxSetCullMode(s32);
extern void gfxSetFlag1A(s32);
extern void gfxLoadTexture(s32 map, void* texture);
extern void gfxBegin(s32 primitive);
extern void gfxVtxPos(f32 x, f32 y, f32 z);
extern void gfxVtxColor(s32 index, u8 r, u8 g, u8 b);
extern void gfxVtxTexCoord(s32 index, f32 s, f32 t);
extern void gfxEnd(void);
extern void gfxSetPrimitive(s32);
extern void gfxSetVtxDesc(void* data);

void flareDraw(void)
{
    Vec screen;
    Vec dir;
    Vec offset;
    Vec eye;
    LensFlareElement* elem;
    void* camera;
    void* texture;
    s32 mode;
    s32 alpha;
    s32 count;
    f32 centerX = 320.0f;
    f32 centerY = 240.0f;
    f32 dist;
    f32 ratio;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;

    if (lbl_8047B5C0 == 0 || lbl_8047B5C1 == 0) {
        return;
    }

    camera = GScameraGetActiveCamera();
    if (camera == NULL) {
        return;
    }
    GScameraGetPosition(camera, &eye);
    if (GScolsys2Sun(&lbl_80375230, &eye) == 1) {
        return;
    }
    mode = gfxWorldToScreen(&lbl_80375230, &screen);
    if (mode == 0) {
        return;
    }

    dir.x = centerX - screen.x;
    dir.y = centerY - screen.y;
    dir.z = 0.0f;
    dist = vecMag(&dir);
    if ((dist > 0.0f ? dist : -dist) < 0.001f || dist > lbl_8047B5C4->farDistance) {
        return;
    }

    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    gfxSetBlendMode(1, 1, 1);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetCullMode(0);
    gfxSetFlag1A(1);

    if (mode == 2) {
        vecNormalize(&dir, &dir, dist);

        if (dist < lbl_8047B5C4->midDistance) {
            ratio = 1.0f;
        } else {
            ratio = ((f32)(lbl_8047B5C4->farDistance - lbl_8047B5C4->midDistance) -
                     (dist - lbl_8047B5C4->midDistance)) /
                    (f32)(lbl_8047B5C4->farDistance - lbl_8047B5C4->midDistance);
            if (ratio < 0.0f) {
                ratio = 0.0f;
            } else if (ratio > 1.0f) {
                ratio = 1.0f;
            }
        }
        alpha = 255.0f * ratio;

        gfxSetPrimitive(4);
        gfxSetVtxDesc(lbl_80314C78);

        elem = lbl_8047B5C8;
        count = lbl_8047B5C4->elementCount;
        while (count-- != 0) {
            texture = lbl_8046D630[elem->textureId];
            if (elem->type == 1) {
                x0 = screen.x - elem->scale * (GStextureGetXsize(texture) >> 1);
                y0 = screen.y - elem->scale * (GStextureGetYsize(texture) >> 1);
                x1 = screen.x + elem->scale * (GStextureGetXsize(texture) >> 1);
                y1 = screen.y + elem->scale * (GStextureGetYsize(texture) >> 1);
            } else {
                vecScale(&offset, &dir, (1.0f / 3.0f) * elem->position * dist);
                x0 = (offset.x + centerX) - elem->scale * (GStextureGetXsize(texture) >> 1);
                y0 = (offset.y + centerY) - elem->scale * (GStextureGetYsize(texture) >> 1);
                x1 = (offset.x + centerX) + elem->scale * (GStextureGetXsize(texture) >> 1);
                y1 = (offset.y + centerY) + elem->scale * (GStextureGetYsize(texture) >> 1);
            }

            gfxLoadTexture(0, texture);
            gfxBegin(4);
            gfxVtxPos(x0, y0, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxTexCoord(0, 0.0f, 0.0f);
            gfxVtxPos(x1, y0, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxTexCoord(0, 1.0f, 0.0f);
            gfxVtxPos(x0, y1, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxTexCoord(0, 0.0f, 1.0f);
            gfxVtxPos(x1, y1, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxTexCoord(0, 1.0f, 1.0f);
            gfxEnd();
            elem++;
        }

        if (dist < lbl_8047B5C4->midDistance) {
            if (dist < lbl_8047B5C4->nearDistance) {
                ratio = lbl_8047B5C4->glareScale[0];
            } else {
                ratio = lbl_8047B5C4->glareScale[0] *
                        (((f32)(lbl_8047B5C4->midDistance - lbl_8047B5C4->nearDistance) -
                          (dist - lbl_8047B5C4->nearDistance)) /
                         (f32)(lbl_8047B5C4->midDistance - lbl_8047B5C4->nearDistance));
                if (ratio < 0.0f) {
                    ratio = 0.0f;
                } else if (ratio > 1.0f) {
                    ratio = 1.0f;
                }
            }
            alpha = 255.0f * ratio;
            gfxClearFlags(2);
            gfxSetPrimitive(4);
            gfxSetVtxDesc(lbl_80314958);
            gfxBegin(4);
            gfxVtxPos(0.0f, 0.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxPos(640.0f, 0.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxPos(0.0f, 480.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxPos(640.0f, 480.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxEnd();
        }
    } else {
        gfxSetBlendMode(2, 1, 1);
        if (dist < lbl_8047B5C4->midDistance) {
            if (dist < lbl_8047B5C4->nearDistance) {
                ratio = lbl_8047B5C4->glareScale[1];
            } else {
                ratio = lbl_8047B5C4->glareScale[1] *
                        (((f32)(lbl_8047B5C4->midDistance - lbl_8047B5C4->nearDistance) -
                          (dist - lbl_8047B5C4->nearDistance)) /
                         (f32)(lbl_8047B5C4->midDistance - lbl_8047B5C4->nearDistance));
                if (ratio < 0.0f) {
                    ratio = 0.0f;
                } else if (ratio > 1.0f) {
                    ratio = 1.0f;
                }
            }
            alpha = 255.0f * ratio;
            gfxClearFlags(2);
            gfxSetPrimitive(4);
            gfxSetVtxDesc(lbl_80314958);
            gfxBegin(4);
            gfxVtxPos(0.0f, 0.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxPos(640.0f, 0.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxPos(0.0f, 480.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxVtxPos(640.0f, 480.0f, 0.0f);
            gfxVtxColor(0, alpha, alpha, alpha);
            gfxEnd();
        }
    }
}
