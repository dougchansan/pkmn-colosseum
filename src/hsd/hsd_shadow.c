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
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_tobj.h"
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
    HSD_SList* objects; /* 0x00 */
    HSD_CObj* camera;   /* 0x04 */
    HSD_TObj* texture;  /* 0x08 */
    f32 scale_s;        /* 0x0C */
    f32 scale_t;        /* 0x10 */
    f32 trans_s;        /* 0x14 */
    f32 trans_t;        /* 0x18 */
    s32 active;         /* 0x1C */
    u8 intensity;       /* 0x20 */
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

typedef struct HSDShadowChan HSDShadowChan;

struct HSDShadowChan {
    HSDShadowChan* next;
    s32 chan;
    u32 flags;
    GXColor amb_color;
    GXColor mat_color;
    u8 enable;
    u8 pad_15[3];
    s32 amb_src;
    s32 mat_src;
    s32 light_mask;
    s32 diff_fn;
    s32 attn_fn;
    void* aobj;
};

typedef struct HSDShadowTevDesc {
    struct HSDShadowTevDesc* next;
    u32 flag;
    u32 stage;
    u32 coord;
    u32 map;
    u32 color;
    u32 color_op;
    u32 color_a;
    u32 color_b;
    u32 color_c;
    u32 color_d;
    u32 color_scale;
    u32 color_bias;
    u8 color_clamp;
    u8 pad_35[3];
    u32 color_tevreg;
    u32 alpha_op;
    u32 alpha_a;
    u32 alpha_b;
    u32 alpha_c;
    u32 alpha_d;
    u32 alpha_scale;
    u32 alpha_bias;
    u8 alpha_clamp;
    u8 pad_59[3];
    u32 alpha_tevreg;
    u32 pad_60;
    s32 kcolor0;
    s32 kcolor1;
    u32 swap0;
    u32 swap1;
    u32 kr;
    u32 kg;
    u32 kb;
    u32 ka;
} HSDShadowTevDesc;

void fn_801B1524(HSDShadow* shadow, u16 width, u16 height);

/* ========================================================================= */
/*  Shadow setup functions                                                   */
/* ========================================================================= */

/* Address: 0x801B019C | Size: 0x204 | Proposed: HSD_ShadowFunc1 */
/* Shadow map initialization and projection matrix setup */
void fn_801B019C(HSDViewingRect* rect, HSDShadowVec* point,
                 f32 x_max, f32 x_min, f32 y_min, f32 y_max) {
    extern char lbl_802752C0[];
    extern char lbl_802752CC[];
    extern char lbl_8047DDB8;
    extern f32 lbl_8047DDC0;
    extern void PSVECSubtract(void* a, void* b, void* out);
    extern f32 PSVECDotProduct(void* a, void* b);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    HSDShadowVec delta;
    HSDShadowVec projected;
    HSDShadowVec normal_distance;
    f32 distance;
    f32 scale;
    f32 x;
    f32 y;

    if (rect == NULL) {
        __assert(lbl_802752C0, 0x3A2, &lbl_8047DDB8);
    }
    if (point == NULL) {
        __assert(lbl_802752C0, 0x3A3, lbl_802752CC);
    }

    PSVECSubtract(point, &rect->origin, &delta);
    distance = PSVECDotProduct(&delta, &rect->normal);
    if (rect->object != NULL) {
        if (distance <= lbl_8047DDC0) {
            return;
        }
        scale = rect->distance / distance;
        PSVECScale(&delta, &delta, scale);
        PSVECSubtract(&delta, &rect->direction, &projected);
        y = PSVECDotProduct(&rect->axis_y, &projected);
        x = PSVECDotProduct(&rect->axis_x, &projected);
        x_max *= scale;
        x_min *= scale;
        y_min *= scale;
        y_max *= scale;
    } else {
        PSVECScale(&rect->normal, &normal_distance, distance);
        PSVECSubtract(&delta, &normal_distance, &projected);
        y = PSVECDotProduct(&rect->axis_y, &projected);
        x = PSVECDotProduct(&rect->axis_x, &projected);
    }

    if (y + y_max > rect->min_y) {
        rect->min_y = y + y_max;
    }
    if (y + y_min < rect->max_y) {
        rect->max_y = y + y_min;
    }
    if (x + x_max > rect->min_x) {
        rect->min_x = x + x_max;
    }
    if (x + x_min < rect->max_x) {
        rect->max_x = x + x_min;
    }
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
/* Enable or disable shadow texture application. */
void fn_801B0880(HSDShadow* shadow, s32 active) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    extern void HSD_MObjDeleteShadowTexture(HSD_TObj* texture);
    HSD_ImageDesc* image;

    if (shadow == NULL) {
        __assert(lbl_802752C0, 0x278, &lbl_8047DDCC);
    }
    if ((shadow->active && active) || (!shadow->active && !active)) {
        return;
    }

    shadow->active = active;
    if (active) {
        image = shadow->texture->imagedesc;
        if (image->image_ptr == NULL) {
            fn_801B1524(shadow, image->width, image->height);
        }
        HSD_MObjAddShadowTexture(shadow->texture);
    } else {
        HSD_MObjDeleteShadowTexture(shadow->texture);
    }
}

/* Address: 0x801B0A98 | Size: 0x140 | Proposed: HSD_ShadowFunc6 */
/* Shadow receiver configuration */
void fn_801B0A98(HSDShadow* shadow, HSD_LObj* light, f32 distance) {
    extern char lbl_802752C0[];
    extern char lbl_802752F4[];
    extern char lbl_8047DDCC;
    extern char lbl_8047DDD4;
    extern char lbl_8047DDE0;
    extern f32 lbl_8047DDC0;
    extern f32 PSVECMag(void* vec);
    extern void PSVECScale(void* src, void* dst, f32 scale);
    extern void PSVECAdd(void* a, void* b, void* out);
    Vec interest;
    Vec position;
    Vec eye;

    if (shadow == NULL) {
        __assert(lbl_802752C0, 0x24D, &lbl_8047DDCC);
    }
    if (light == NULL) {
        __assert(lbl_802752C0, 0x24E, &lbl_8047DDE0);
    }

    switch (light->flags & 3) {
    case 1:
        if (distance <= lbl_8047DDC0) {
            __assert(lbl_802752C0, 0x252, lbl_802752F4);
        }
        HSD_CObjGetInterest(shadow->camera, &interest);
        HSD_LObjGetPosition(light, &position);
        PSVECScale(&position, &position, distance / PSVECMag(&position));
        PSVECAdd(&interest, &position, &eye);
        HSD_CObjSetEyePosition(shadow->camera, &eye);
        break;
    case 2:
    case 3:
        HSD_LObjGetPosition(light, &eye);
        HSD_CObjSetEyePosition(shadow->camera, &eye);
        break;
    default:
        __assert(lbl_802752C0, 0x262, &lbl_8047DDD4);
        break;
    }
}

/* Address: 0x801B0BD8 | Size: 0x2E0 | Proposed: HSD_ShadowFunc7 */
/* Copy the shadow map and update its texture projection matrix. */
void fn_801B0BD8(HSDShadow* shadow) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    extern char lbl_8047DDD4;
    extern void fn_800B9FE4(void* destination, u8 clear);
    extern void fn_800B8E74(void);
    extern void GXInvalidateTexAll(void);
    extern void C_MTXLightPerspective(Mtx m, f32 fov, f32 aspect,
                                      f32 scale_s, f32 scale_t, f32 trans_s,
                                      f32 trans_t);
    extern void C_MTXLightFrustum(Mtx m, f32 top, f32 bottom, f32 left,
                                  f32 right, f32 near, f32 scale_s,
                                  f32 scale_t, f32 trans_s, f32 trans_t);
    extern void C_MTXLightOrtho(Mtx m, f32 top, f32 bottom, f32 left,
                                f32 right, f32 scale_s, f32 scale_t,
                                f32 trans_s, f32 trans_t);
    extern void PSMTXConcat(Mtx a, Mtx b, Mtx out);
    HSD_ImageDesc* image;
    Mtx projection;

    if (shadow == NULL) {
        __assert(lbl_802752C0, 0x229, &lbl_8047DDCC);
    }

    image = shadow->texture->imagedesc;
    if (image->image_ptr == NULL) {
        fn_801B1524(shadow, image->width, image->height);
    }

    fn_800B9FE4(image->image_ptr, 1);
    fn_800B8E74();
    GXInvalidateTexAll();

    switch (HSD_CObjGetProjectionType(shadow->camera)) {
    case 1:
        C_MTXLightPerspective(
            projection, shadow->camera->projection_param.perspective.fov,
            shadow->camera->projection_param.perspective.aspect,
            shadow->scale_s, shadow->scale_t, shadow->trans_s,
            shadow->trans_t);
        break;
    case 2:
        C_MTXLightFrustum(
            projection, shadow->camera->projection_param.frustum.top,
            shadow->camera->projection_param.frustum.bottom,
            shadow->camera->projection_param.frustum.left,
            shadow->camera->projection_param.frustum.right,
            shadow->camera->near, shadow->scale_s, shadow->scale_t,
            shadow->trans_s, shadow->trans_t);
        break;
    case 3:
        C_MTXLightOrtho(projection,
                        shadow->camera->projection_param.ortho.top,
                        shadow->camera->projection_param.ortho.bottom,
                        shadow->camera->projection_param.ortho.left,
                        shadow->camera->projection_param.ortho.right,
                        shadow->scale_s, shadow->scale_t, shadow->trans_s,
                        shadow->trans_t);
        break;
    default:
        __assert(lbl_802752C0, 0x305, &lbl_8047DDD4);
        break;
    }

    PSMTXConcat(projection, shadow->camera->view_mtx, shadow->texture->mtx);
}

/* Address: 0x801B0EB8 | Size: 0x66C | Proposed: HSD_ShadowMain */
/* Main shadow system entry point - orchestrates shadow map gen and apply */
extern Mtx lbl_8036CBC0;
extern u32 lbl_8036CC40[];
extern f32 lbl_8047DDE8;
extern f32 lbl_8047DDEC;
extern void GXLoadPosMtxImm(Mtx mtx, u32 index);
extern void fn_800BD554(u32 index);
extern void HSD_ClearVtxDesc(void);
extern void fn_800B7874(u32 attr, u32 type);
extern void fn_800B7D74(u32 vtxfmt, u32 attr, u32 comptype, u32 compsize,
                        u32 frac);
extern void fn_800B928C(u32 primitive, u32 vtxfmt, u32 count);
extern void fn_800BD7A0(u32 left, u32 top, u32 width, u32 height);
extern f32 HSD_CObjGetTop(HSD_CObj* cobj);
extern f32 HSD_CObjGetBottom(HSD_CObj* cobj);
extern f32 HSD_CObjGetLeft(HSD_CObj* cobj);
extern f32 HSD_CObjGetRight(HSD_CObj* cobj);
extern void fn_801B2878(u32 mode);
extern void fn_801B3884(void);
extern void fn_801B3408(HSDShadowTevDesc* desc);
extern void fn_801B3890(void);
extern void fn_801B29E4(u32 flags, HSD_PEDesc* pe);
extern void fn_801B3998(HSDShadowChan* chan);
extern void fn_801A13CC(void* jobj, Mtx vmtx, HSD_TrspMask flags,
                        u32 rendermode);
extern void fn_80195A48(void);

typedef union HSDShadowWGPipe {
    f32 f32;
} HSDShadowWGPipe;

volatile HSDShadowWGPipe HSDShadowGXWGFifo : 0xCC008000;

static inline void ShadowPosition2f32(f32 x, f32 y)
{
    HSDShadowGXWGFifo.f32 = x;
    HSDShadowGXWGFifo.f32 = y;
}

static inline void ShadowDrawBackground(HSDShadow* shadow)
{
    HSD_CObj* cobj;
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
    f32 near;

    cobj = shadow->camera;
    GXLoadPosMtxImm(lbl_8036CBC0, 0);
    lbl_8036CC40[3]++;
    fn_800BD554(0);
    HSD_ClearVtxDesc();
    fn_800B7874(9, 1);
    fn_800B7D74(0, 9, 1, 4, 0);
    fn_801B2878(2);

    top = HSD_CObjGetTop(cobj);
    bottom = HSD_CObjGetBottom(cobj);
    left = HSD_CObjGetLeft(cobj);
    right = HSD_CObjGetRight(cobj);
    near = HSD_CObjGetNear(cobj);

    top *= lbl_8047DDE8;
    bottom *= lbl_8047DDE8;
    left *= lbl_8047DDE8;
    right *= lbl_8047DDE8;
    near *= lbl_8047DDEC;

    fn_800B928C(0x80, 0, 4);
    ShadowPosition2f32(left, top);
    ShadowPosition2f32(near, right);
    ShadowPosition2f32(top, near);
    ShadowPosition2f32(right, bottom);
    ShadowPosition2f32(near, left);
    ShadowPosition2f32(bottom, near);
}

void fn_801B0EB8(HSDShadow* shadow) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    static HSDShadowChan chan = {
        NULL, 4, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 255 }, 0, { 0, 0, 0 },
        0, 0, 0, 2, 2, NULL,
    };
    static HSDShadowTevDesc tev = {
        NULL, 1, 0, 0xFF, 0xFF, 4,
        0, 0xF, 0xF, 0xF, 0xA, 0, 0, 1, { 0, 0, 0 }, 0,
        0, 7, 7, 7, 5, 0, 0, 1, { 0, 0, 0 }, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    static HSD_PEDesc pedesc = {
        9, 0, 0, 0, 0, 4, 5, 0xF, 7, 7, 0, 7,
    };
    HSD_SList* list;
    HSD_CObj* cobj;
    HSD_ImageDesc* image;

    if (shadow == NULL) {
        __assert(lbl_802752C0, 0x18D, &lbl_8047DDCC);
    }
    if (shadow->camera == NULL) {
        __assert(lbl_802752C0, 0x18E, lbl_802752C0 + 0xAC);
    }
    if (shadow->texture == NULL) {
        __assert(lbl_802752C0, 0x18F, lbl_802752C0 + 0xBC);
    }
    if (shadow->texture->imagedesc == NULL) {
        __assert(lbl_802752C0, 0x190, lbl_802752C0 + 0xCC);
    }

    list = shadow->objects;
    cobj = shadow->camera;
    image = shadow->texture->imagedesc;

    if (list != NULL) {
        HSD_CObjSetCurrent(cobj);
        fn_801B3884();
        fn_801B3408(&tev);
        fn_801B3890();
        fn_801B29E4(0, &pedesc);

        chan.mat_color.r = 0xFF;
        chan.mat_color.g = 0xFF;
        chan.mat_color.b = 0xFF;
        fn_801B3998(&chan);
        fn_800BD7A0(0, 0, image->width, image->height);
        ShadowDrawBackground(shadow);

        if (lbl_8047B310 != 0) {
            chan.mat_color.r = 0;
            chan.mat_color.g = 0;
            chan.mat_color.b = 0;
            fn_801B3998(&chan);
            fn_800BD7A0(2, 2, image->width - 4, image->height - 4);
            ShadowDrawBackground(shadow);

            chan.mat_color.r = 0xFF;
            chan.mat_color.g = 0xFF;
            chan.mat_color.b = 0xFF;
            fn_801B3998(&chan);
            fn_800BD7A0(4, 4, image->width - 8, image->height - 8);
            ShadowDrawBackground(shadow);
        }

        chan.mat_color.r = shadow->intensity;
        chan.mat_color.g = shadow->intensity;
        chan.mat_color.b = shadow->intensity;
        fn_801B3998(&chan);
        fn_800BD7A0(2, 2, image->width - 4, image->height - 4);

        for (list = shadow->objects; list != NULL; list = list->next) {
            fn_801A13CC(list->data, NULL,
                        HSD_TRSP_OPA | HSD_TRSP_TEXEDGE, RENDER_SHADOW);
        }
        fn_80195A48();
    }
}

/* Address: 0x801B1524 | Size: 0x19C | Proposed: HSD_ShadowFunc9 */
/* Shadow cleanup / restore GX state */
void fn_801B1524(HSDShadow* shadow, u16 width, u16 height) {
    extern char lbl_802752C0[];
    extern char lbl_8047DDCC;
    extern f32 lbl_8047DDC0;
    extern u32 GXGetTexBufferSize(u16 width, u16 height, u32 format,
                                  u32 mipmap, u32 max_lod);
    extern void* fn_800E202C(void* handle);
    extern void fn_800E24B0(void);
    extern void fn_800E209C(void* saved);
    extern u16 fn_800E2B00(u32 size, u32 alignment);
    extern void* fn_800E27B0(u16 handle);
    HSD_ImageDesc* image;
    void* saved;
    u16 handle;
    u32 size;

    if (shadow == NULL) {
        __assert(lbl_802752C0, 0x12C, &lbl_8047DDCC);
    }
    if (width == 0) {
        __assert(lbl_802752C0, 0x12D, lbl_802752C0 + 0x88);
    }
    if (height == 0) {
        __assert(lbl_802752C0, 0x12E, lbl_802752C0 + 0x94);
    }

    image = shadow->texture->imagedesc;
    if (image->image_ptr != NULL && image->width == width &&
        image->height == height) {
        return;
    }

    if (image->image_ptr != NULL) {
        saved = fn_800E202C(image->image_ptr);
        fn_800E24B0();
        fn_800E209C(saved);
        image->image_ptr = NULL;
    }

    size = GXGetTexBufferSize(width, height, 0, 0, 0);
    if (size == 0) {
        __assert(lbl_802752C0, 0x13F, lbl_802752C0 + 0xA0);
    }
    handle = fn_800E2B00(size, 0x20);
    image->image_ptr = fn_800E27B0(handle);
    image->width = width;
    image->height = height;
    HSD_CObjSetViewportfx4(shadow->camera, lbl_8047DDC0, (f32)width,
                           lbl_8047DDC0, (f32)height);
    HSD_CObjSetScissorx4(shadow->camera, 0, width, 0, height);
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
