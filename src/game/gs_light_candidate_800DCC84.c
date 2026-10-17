/**
 * @file gs_light_candidate_800DCC84.c
 * @brief GSlight: colour/type setters, pool create/load/free/init and the
 *        per-frame light setup.
 *
 * Address range: 0x800DCC84 - 0x800DD270 (retail GSlight.cpp tail).
 */

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_object.h"

/* Same layout as HSD_WObjDesc. */
typedef struct GSlightWObjDesc {
    /* 0x00 */ char* className;
    /* 0x04 */ f32 pos[3];
    /* 0x10 */ void* robjDesc;
} GSlightWObjDesc; /* size 0x14 */

/* Same layout as HSD_LightDesc. */
typedef struct GSlightLObjDesc {
    /* 0x00 */ char* className;
    /* 0x04 */ struct GSlightLObjDesc* next;
    /* 0x08 */ u16 flags;
    /* 0x0A */ u16 attnFlags;
    /* 0x0C */ GXColor color;
    /* 0x10 */ GSlightWObjDesc* position;
    /* 0x14 */ GSlightWObjDesc* interest;
    /* 0x18 */ void* param;
} GSlightLObjDesc; /* size 0x1C */

typedef struct GSlightDesc {
    /* 0x00 */ HSD_LightDesc* lobjDesc;
    /* 0x04 */ HSD_LightAnim** animations;
} GSlightDesc;

/* Only the link field of HSD_LObj is touched here. */
typedef struct GSlightLObj {
    /* 0x00 */ u8 pad_00[0xC];
    /* 0x0C */ struct GSlightLObj* next;
} GSlightLObj;

typedef struct GSlight {
    /* 0x00 */ u8 allocated;
    /* 0x01 */ u8 active;
    /* 0x02 */ u8 hasAnimation;
    /* 0x03 */ u8 isAnimating;
    /* 0x04 */ s32 type;
    /* 0x08 */ GSlightDesc* desc;
    /* 0x0C */ GSlightLObj* lobj;
    /* 0x10 */ GSlightWObjDesc positionDesc; /* default light descriptors */
    /* 0x24 */ GSlightWObjDesc interestDesc;
    /* 0x38 */ GSlightLObjDesc lobjDesc;
    /* 0x54 */ f32 shininess;
    /* 0x58 */ u32 animCount;
    /* 0x5C */ s32 animMode;
    /* 0x60 */ u32 animIndex;
    /* 0x64 */ f32 animRate;
    /* 0x68 */ f32 animFrame;
    /* 0x6C */ f32 animEndFrame;
    /* 0x70 */ s8 animEnded;
    /* 0x71 */ u8 pad_71[3];
} GSlight; /* size 0x74 */

extern void HSD_LObjSetColor(GSlightLObj* lobj, GXColor color);
extern void HSD_LObjClearFlags(GSlightLObj* lobj, u32 flags);
extern void HSD_LObjSetFlags(GSlightLObj* lobj, u32 flags);
extern GSlightLObj* HSD_LObjLoadDesc(HSD_LightDesc* desc);
extern void HSD_LObjRemoveAnimAll(GSlightLObj* lobj);
extern void HSD_LObjAddAnimAll(GSlightLObj* lobj, HSD_LightAnim* anim);
extern void HSD_LObjReqAnimAll(GSlightLObj* lobj, f32 frame);
extern void HSD_LObjDeleteCurrentAll(GSlightLObj* lobj);
extern void HSD_LObjAddCurrentAll(GSlightLObj* lobj);
extern void HSD_LObjSetup(HSD_CObj* cobj);
extern void HSD_ForeachAnim(void* obj, u32 type, u32 mask, void (*cb)(HSD_AObj*), ...);
extern void __assert(const char* file, u32 line, const char* condition);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u32 handle);

extern u16 lbl_8047AAE8;       /* light pool GSmem handle */
extern GSlight* lbl_8047AAEC;  /* light pool */
extern u32 lbl_8047AAF0;       /* light pool size */
extern f32 lbl_8047AAF4;       /* animation end frame scratch */
extern const f32 lbl_8047CA70; /* 1.0f */
extern const f32 lbl_8047CA78; /* 0.0f */
extern const f32 lbl_8047CA8C;
extern const char lbl_8047CA90[] __attribute__((section(".sdata2")));
extern const char lbl_8047CA98[] __attribute__((section(".sdata2")));

void lightGetFrameCount__FP9_HSD_AObj(HSD_AObj* aobj);

typedef struct GSlightColor {
    f32 r;
    f32 g;
    f32 b;
} GSlightColor;

void GSlightSetColor(GSlight* light, GSlightColor* rgb)
{
    GXColor color;

    /* Retail leaves the alpha byte unset. */
    color.r = rgb->r;
    color.g = rgb->g;
    color.b = rgb->b;
    HSD_LObjSetColor(light->lobj, color);
}

void GSlightSetType(GSlight* light, s32 type)
{
    HSD_LObjClearFlags(light->lobj, 3);
    switch (type) {
    case 0:
        HSD_LObjSetFlags(light->lobj, 0);
        break;
    case 1:
        HSD_LObjSetFlags(light->lobj, 1);
        break;
    case 2:
        HSD_LObjSetFlags(light->lobj, 2);
        break;
    case 3:
        HSD_LObjSetFlags(light->lobj, 3);
        break;
    }
    light->type = type;
}

void GSlightFree(GSlight* light)
{
    GSlightLObj* lobj = light->lobj;

    if (lobj != NULL && ref_DEC(lobj)) {
        hsdDelete(lobj);
    }
    light->active = 0;
    light->allocated = 0;
}

static inline GSlight* lightFindFree(void)
{
    GSlight* light = lbl_8047AAEC;
    u32 i;

    for (i = 0; i < lbl_8047AAF0; i++, light++) {
        if (light->allocated == 0) {
            return light;
        }
    }
    return NULL;
}

/*
 * Same body as GSlightSetAnimIndex, expanded here by the target with the
 * unsigned index/count compare against the constant 0 still present.
 */
static inline void lightSetAnimIndex(GSlight* light, u32 animIndex)
{
    if (light->hasAnimation != 0) {
        HSD_LObjRemoveAnimAll(light->lobj);
        if (animIndex <= light->animCount) {
            light->animIndex = animIndex;
            HSD_LObjAddAnimAll(light->lobj, light->desc->animations[light->animIndex]);
            HSD_LObjReqAnimAll(light->lobj, lbl_8047CA78);
            lbl_8047AAF4 = lbl_8047CA78;
            HSD_ForeachAnim(light->lobj, 7, 0xFFFF, lightGetFrameCount__FP9_HSD_AObj, 0);
            light->animEndFrame = lbl_8047AAF4;
        }
    }
}

GSlight* GSlightLoad(GSlightDesc* desc)
{
    GSlight* light = lightFindFree();

    if (light == NULL) {
        return NULL;
    }

    light->desc = desc;
    light->lobj = HSD_LObjLoadDesc(light->desc->lobjDesc);
    light->allocated = 1;
    light->active = 0;
    light->isAnimating = 0;
    if (light->desc->animations != NULL) {
        light->hasAnimation = 1;
        light->animRate = lbl_8047CA70;
        light->animMode = 1;
        light->animEnded = 0;
        light->animCount = 0;
        while (light->desc->animations[light->animCount] != NULL) {
            light->animCount++;
        }
        lightSetAnimIndex(light, 0);
    } else {
        light->hasAnimation = 0;
    }
    return light;
}

GSlight* GSlightCreate(void)
{
    GSlight* light = lightFindFree();

    if (light == NULL) {
        return NULL;
    }

    light->positionDesc.className = NULL;
    light->positionDesc.pos[0] = lbl_8047CA78;
    light->positionDesc.pos[1] = lbl_8047CA78;
    light->positionDesc.pos[2] = lbl_8047CA78;
    light->positionDesc.robjDesc = NULL;
    light->interestDesc.className = NULL;
    light->interestDesc.pos[0] = lbl_8047CA78;
    light->interestDesc.pos[1] = lbl_8047CA78;
    light->interestDesc.pos[2] = lbl_8047CA78;
    light->interestDesc.robjDesc = NULL;
    light->shininess = lbl_8047CA8C;
    light->lobjDesc.className = NULL;
    light->lobjDesc.next = NULL;
    light->lobjDesc.flags = 4;
    light->lobjDesc.attnFlags = 0;
    light->lobjDesc.color.r = 0x80;
    light->lobjDesc.color.g = 0x80;
    light->lobjDesc.color.b = 0x80;
    light->lobjDesc.color.a = 0;
    light->lobjDesc.position = &light->positionDesc;
    light->lobjDesc.interest = &light->interestDesc;
    light->lobjDesc.param = &light->shininess;
    light->lobj = HSD_LObjLoadDesc((HSD_LightDesc*)&light->lobjDesc);
    light->allocated = 1;
    light->active = 0;
    return light;
}

void GSlightInit(u32 count)
{
    u32 i;

    lbl_8047AAF0 = count;
    lbl_8047AAE8 = _toolentryAlloc__FUl(count * sizeof(GSlight));
    if (lbl_8047AAE8 != 0) {
        lbl_8047AAEC = GSmemLock(lbl_8047AAE8);
        for (i = 0; i < lbl_8047AAF0; i++) {
            lbl_8047AAEC[i].allocated = 0;
        }
    }
}

void lightGetFrameCount__FP9_HSD_AObj(HSD_AObj* aobj)
{
    if (aobj == NULL) {
        __assert(lbl_8047CA90, 0xAB, lbl_8047CA98);
    }
    lbl_8047AAF4 = lbl_8047CA70 + aobj->end_frame;
}

static inline s32 lightFindFirstActive(GSlight* light)
{
    u32 i;

    for (i = 0; i < lbl_8047AAF0; light++, i++) {
        if (light->allocated == 1 && light->active != 0) {
            return i;
        }
    }
    return -1;
}

void GSlightSetupLights(HSD_CObj* cobj)
{
    GSlight* tail;
    s32 first;
    u32 i;

    HSD_LObjDeleteCurrentAll(NULL);

    tail = lbl_8047AAEC;
    first = lightFindFirstActive(tail);
    if (first != -1) {
        tail = &tail[first];
        for (i = first + 1; i < lbl_8047AAF0; i++) {
            GSlight* light = &lbl_8047AAEC[i];
            if (light->allocated == 1 && light->active != 0) {
                tail->lobj->next = light->lobj;
                tail = light;
            }
        }
        tail->lobj->next = NULL;
        HSD_LObjAddCurrentAll(lbl_8047AAEC[first].lobj);
    }
    HSD_LObjSetup(cobj);
}
