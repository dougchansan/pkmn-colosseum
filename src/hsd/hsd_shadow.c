/**
 * @file hsd_shadow.c
 * @brief HSD Shadow system - shadow map rendering and application.
 *
 * Address range: 0x801B019C - 0x801B16C0
 * Contains the HSD shadow rendering system: shadow map generation,
 * projection matrix computation, shadow texture binding, and
 * the main shadow rendering dispatch.
 *
 * "Proposed:" names from symbols.txt: HSD_ShadowFunc1-10, HSD_ShadowMain
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"
#include "crt/math.h"

extern u8 lbl_8047B310;
void fn_801B06D4(u8 value)
{
    lbl_8047B310 = value;
}

typedef struct HSDShadowOwner {
    u8 unk_00[0x8];
    struct HSDShadowObject* object; /* 0x08 */
} HSDShadowOwner;

typedef struct HSDShadow {
    void* unk_00;
    HSD_CObj* camera; /* 0x04 */
} HSDShadow;

typedef struct HSDShadowObject {
    u8 unk_00[0x58];
    struct HSDShadowData* shadow; /* 0x58 */
} HSDShadowObject;

typedef struct HSDShadowData {
    u8 unk_00[0x4];
    u16 width;  /* 0x04 */
    u16 height; /* 0x06 */
} HSDShadowData;

typedef struct HSDShadowVec {
    f32 x;
    f32 y;
    f32 z;
} HSDShadowVec;

typedef struct HSDViewingRect {
    HSDShadowVec origin;     /* 0x00 */
    HSDShadowVec axis_x;     /* 0x0C */
    HSDShadowVec axis_y;     /* 0x18 */
    HSDShadowVec direction;  /* 0x24 */
    HSDShadowVec normal;     /* 0x30 */
    f32 distance;            /* 0x3C */
    f32 min_x;               /* 0x40 */
    f32 max_x;               /* 0x44 */
    f32 max_y;               /* 0x48 */
    f32 min_y;               /* 0x4C */
    void* object;            /* 0x50 */
} HSDViewingRect;

/* ========================================================================= */
/*  Shadow setup functions                                                   */
/* ========================================================================= */

/* Address: 0x801B019C | Size: 0x204 | Proposed: HSD_ShadowFunc1 */
/* Shadow map initialization and projection matrix setup */
void fn_801B019C(void) {
}

/* Address: 0x801B03A0 | Size: 0x68 | Proposed: HSD_ShadowFunc2 */
/* Shadow texture coordinate generation */
s32 HSD_ViewingRectCheck(void* arg0) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDB8;
    s32 result;

    if (arg0 == NULL) {
        __assert(lbl_802752C0, 0x37D, &lbl_8047DDB8);
    }
    result = 0;
    if (*(f32*)((u8*)arg0 + 0x40) > *(f32*)((u8*)arg0 + 0x44)) {
        if (*(f32*)((u8*)arg0 + 0x4C) > *(f32*)((u8*)arg0 + 0x48)) {
            result = 1;
        }
    }
    return result;
}

/* Address: 0x801B0408 | Size: 0xD8 | Proposed: HSD_ShadowFunc3 */
/* Shadow TEV stage configuration */
void fn_801B0408(HSDViewingRect* rect, HSDShadowVec* origin,
                 HSDShadowVec* target, HSDShadowVec* up, void* object) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDB8;
    extern f32 lbl_8047DDC4;
    extern f32 lbl_8047DDC8;
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern void PSVECNormalize(void* src, void* dst);
    extern void PSVECCrossProduct(void* a, void* b, void* out);
    extern f32 PSVECMag(void* vec);
    HSDShadowVec normalized_up;

    if (rect == NULL) {
        __assert(lbl_802752C0, 0x366, &lbl_8047DDB8);
    }

    rect->origin = *origin;
    PSVECSubtract(target, origin, &rect->direction);
    PSVECNormalize(&rect->direction, &rect->normal);
    PSVECNormalize(up, &normalized_up);
    PSVECCrossProduct(&rect->normal, &normalized_up, &rect->axis_y);
    PSVECCrossProduct(&rect->axis_y, &rect->normal, &rect->axis_x);
    rect->distance = PSVECMag(&rect->direction);
    rect->min_y = lbl_8047DDC4;
    rect->min_x = lbl_8047DDC4;
    rect->max_y = lbl_8047DDC8;
    rect->max_x = lbl_8047DDC8;
    rect->object = object;
}

/* Address: 0x801B04E0 | Size: 0x1F4 | Proposed: HSD_ShadowFunc4 */
/* Shadow map projection computation */
void fn_801B04E0(HSDShadow* shadow, f32 top, f32 bottom, f32 left, f32 right) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    extern char lbl_8047DDD4;
    extern f32 lbl_8047DDC0;
    HSD_CObj* camera;
    f32 distance;
    f32 width;
    f32 height;
    f32 scale;

    if (shadow == NULL) {
        __assert(lbl_802752C0, 0x31C, &lbl_8047DDCC);
    }

    camera = shadow->camera;
    distance = HSD_CObjGetEyeDistance(camera);
    if (distance <= lbl_8047DDC0) {
        __assert(lbl_802752C0, 0x320, lbl_802752C0 + 0x34);
    }

    switch (HSD_CObjGetProjectionType(camera)) {
    case 1:
        width = __fabs(top) > __fabs(bottom) ? __fabs(top) : __fabs(bottom);
        height = __fabs(left) > __fabs(right) ? __fabs(left) : __fabs(right);
        HSD_CObjSetAspect(camera, height / width);
        HSD_CObjSetFov(camera, (f32)atan2(height, distance));
        break;
    case 2:
        scale = HSD_CObjGetNear(camera) / distance;
        if (scale <= lbl_8047DDC0) {
            __assert(lbl_802752C0, 0x33D, lbl_802752C0 + 0x44);
        }
        HSD_CObjSetFrustum(camera, scale * top, scale * bottom, scale * left,
                           scale * right);
        break;
    case 3:
        HSD_CObjSetOrtho(camera, top, bottom, left, right);
        break;
    default:
        __assert(lbl_802752C0, 0x345, &lbl_8047DDD4);
        break;
    }
}

/* ========================================================================= */
/*  Shadow state management                                                  */
/* ========================================================================= */

/* Address: 0x801B06DC | Size: 0x60 */
/* Shadow state setter / initializer */
void fn_801B06DC(void* arg0) {
    extern void* fn_800E202C(void*);
    extern void fn_800E24B0(void);
    extern void fn_800E209C(void*);
    void* saved;
    void** slot;
    void* obj;

    obj = *(void**)((u8*)arg0 + 8);
    slot = *(void***)((u8*)obj + 0x58);
    if (*(void* volatile*)slot != NULL) {
        saved = fn_800E202C(*slot);
        fn_800E24B0();
        fn_800E209C(saved);
        *slot = NULL;
    }
}

/* Address: 0x801B073C | Size: 0x98 */
/* Shadow map texture bind */
void fn_801B073C(HSD_SList** list, void* object) {
    extern void fn_801A05EC(void* object);
    extern HSD_SList* fn_801A3E64(HSD_SList* node);
    HSD_SList** current;

    if (list == NULL) {
        return;
    }

    current = list;
    if (object != NULL) {
        while (*current != NULL) {
            if ((*current)->data == object) {
                fn_801A05EC(object);
                *current = fn_801A3E64(*current);
                break;
            }
            current = &(*current)->next;
        }
    } else {
        while (*current != NULL) {
            fn_801A05EC((*current)->data);
            *current = fn_801A3E64(*current);
        }
    }
}

/* Address: 0x801B07D4 | Size: 0xAC */
/* Shadow light direction setup */
void fn_801B07D4(HSD_SList** list, void* object) {
    extern HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data);
    HSD_SList* current;

    if (list == NULL || object == NULL) {
        return;
    }

    for (current = *list; current != NULL; current = current->next) {
        if (current->data == object) {
            return;
        }
    }

    *list = HSD_SListPrepend(*list, object);
    ref_INC(object);
}

/* ========================================================================= */
/*  Main shadow rendering                                                    */
/* ========================================================================= */

/* Address: 0x801B0880 | Size: 0x218 | Proposed: HSD_ShadowFunc5 */
/* Shadow pass setup - configures GX for shadow map rendering */
void fn_801B0880(void) {
}

/* Address: 0x801B0A98 | Size: 0x140 | Proposed: HSD_ShadowFunc6 */
/* Shadow receiver configuration */
void fn_801B0A98(void) {
}

/* Address: 0x801B0BD8 | Size: 0x2E0 | Proposed: HSD_ShadowFunc7 */
/* Shadow caster traversal and rendering */
void fn_801B0BD8(void) {
}

/* Address: 0x801B0EB8 | Size: 0x66C | Proposed: HSD_ShadowMain */
/* Main shadow system entry point - orchestrates shadow map gen and apply */
void fn_801B0EB8(void) {
}

/* Address: 0x801B1524 | Size: 0x19C | Proposed: HSD_ShadowFunc9 */
/* Shadow cleanup / restore GX state */
void fn_801B1524(void) {
}

/* Address: 0x801B16C0 | Size: 0x70 | Proposed: HSD_ShadowFunc10 */
/* Shadow finalize - restore render state after shadow pass */
void HSD_ShadowInit(void* arg0) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    extern void fn_800B962C(u32, u32, u32, u32);
    extern void fn_800B96F8(u32, u32, u32, u32);
    HSDShadowObject* obj;
    HSDShadowData* shadow;

    if (arg0 == NULL) {
        __assert(lbl_802752C0, 0x10C, &lbl_8047DDCC);
    }
    obj = ((HSDShadowOwner*)arg0)->object;
    shadow = obj->shadow;
    fn_800B962C(0, 0, shadow->width, shadow->height);
    fn_800B96F8(shadow->width, shadow->height, 0x20, 0);
}
