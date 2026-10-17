/**
 * @file shadow.c
 * @brief HAL sysdolphin shadow.c: projected shadow textures and viewing
 *        rectangles, 0x801B019C-0x801B1890.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/shadow.c) and checked against Colosseum's retail
 * code, which is the newer sysdolphin: the shadow image lives in GSmem
 * (allocated by handle, freed through the handle lookup), the background
 * rectangle can be drawn in three passes to leave a clear border, the
 * shadow camera can be aimed from a light (HSD_ShadowSetLight), and the
 * viewing-rectangle maths uses the double-precision fabs/atan2. The library
 * is built with deferred inlining, so functions are listed in HAL's order
 * and MWCC emits them in reverse (the retail address order). Functions
 * nothing in the game references are compiled and dead-stripped by the
 * linker as in retail.
 *
 * Globals other objects already link against keep their dtk names; the
 * comments give the HAL names. The GX and GSmem calls are fn_ symbols in
 * Colosseum's map; their SDK names are given where they are declared.
 */
#include "crt/math_ppc.h"
#include "dolphin/gx/GX.h"
#include "dolphin/gx/GXVert.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_objalloc.h"
#include "hsd/hsd_tobj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/lobj.h"
#include "sysdolphin/baselib/object.h"

void* memset(void* dst, int c, u32 n);
double atan2(double y, double x);
void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
void PSVECAdd(const Vec3* a, const Vec3* b, Vec3* ab);
void PSVECSubtract(const Vec3* a, const Vec3* b, Vec3* ab);
void PSVECScale(const Vec3* src, Vec3* dst, f32 scale);
void PSVECNormalize(const Vec3* src, Vec3* unit);
f32 PSVECMag(const Vec3* v);
f32 PSVECDotProduct(const Vec3* a, const Vec3* b);
void PSVECCrossProduct(const Vec3* a, const Vec3* b, Vec3* axb);
void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS,
                           f32 scaleT, f32 transS, f32 transT);
void C_MTXLightFrustum(Mtx m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 scaleS,
                       f32 scaleT, f32 transS, f32 transT);
void C_MTXLightOrtho(Mtx m, f32 t, f32 b, f32 l, f32 r, f32 scaleS,
                     f32 scaleT, f32 transS, f32 transT);

/* GXSetVtxDesc */
void GXSetVtxDesc(int attr, int type);
/* GXSetVtxAttrFmt */
void GXSetVtxAttrFmt(int vtxfmt, int attr, int cnt, int type, u8 frac);
/* GXPixModeSync */
void GXPixModeSync(void);
/* GXBegin */
void GXBegin(int type, int vtxfmt, u16 nverts);
/* GXSetTexCopySrc */
void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht);
/* GXSetTexCopyDst */
void GXSetTexCopyDst(u16 wd, u16 ht, int fmt, GXBool mipmap);
/* GXCopyTex */
void GXCopyTex(void* dest, GXBool clear);
void GXInvalidateTexAll(void);
u32 GXGetTexBufferSize(u16 width, u16 height, u32 format, GXBool mipmap,
                       u8 max_lod);
void GXLoadPosMtxImm(Mtx mtx, u32 id);
/* GXSetCurrentMtx */
void GXSetCurrentMtx(u32 id);
/* GXSetScissor */
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);

/* GSmemFindHandle */
u16 GSmemGetHandleByPtr(void* ptr);
/* GSmemLock */
void fn_800E24B0(u16 handle);
/* GSmemFree */
void GSmemFree(u16 handle);
/* GSmemGetPtr */
void* GSmemLock(u16 handle);
/* GSmemAllocTail */
u16 fn_800E2B00(u32 size, u32 alignment);

/* HSD_JObjUnref */
void HSD_JObjUnref(HSD_JObj* jobj);
/* HSD_JObjDispAll */
void HSD_JObjDispAll(HSD_JObj* jobj, MtxPtr vmtx, u32 trsp_mask, u32 rendermode);
/* HSD_SListRemove */
HSD_SList* HSD_SListRemove(HSD_SList* list);
/* HSD_SListAllocAndPrepend */
HSD_SList* HSD_SListPrepend(HSD_SList* list, void* data);
/* HSD_CObjEndCurrent */
void HSD_CObjEndCurrent(void);
f32 HSD_CObjGetTop(HSD_CObj* cobj);
f32 HSD_CObjGetBottom(HSD_CObj* cobj);
f32 HSD_CObjGetLeft(HSD_CObj* cobj);
f32 HSD_CObjGetRight(HSD_CObj* cobj);
void HSD_MObjDeleteShadowTexture(HSD_TObj* tobj);
void HSD_ClearVtxDesc(void);
/* HSD_StateSetCullMode */
void HSD_StateSetCullMode(int mode);
/* HSD_SetupPEMode */
void HSD_SetupPEMode(u32 flags, HSD_PEDesc* pe);
/* HSD_SetupTevStageAll */
void HSD_SetupTevStageAll(void* tevdesc);
/* HSD_StateInitTev */
void HSD_StateInitTev(void);
/* HSD_StateSetNumTexGens */
void HSD_StateSetNumTexGens(void);
/* HSD_SetupChannelAll */
void HSD_SetupChannelAll(void* chan);

/* HSD_identityMtx */
extern Mtx lbl_8036CBC0;

/* HSD_PerfCurrentStat (perf.c) */
typedef struct HSD_PerfStat {
    f32 cpu_time;
    f32 draw_time;
    f32 total_time;
    u32 nb_mtx_load;
    u32 env_blend[32];
} HSD_PerfStat;
extern HSD_PerfStat lbl_8036CC40;

static inline void HSD_PerfCountMtxLoad(void)
{
    lbl_8036CC40.nb_mtx_load += 1;
}

typedef struct HSD_Chan {
    struct HSD_Chan* next;
    u32 chan;
    u32 flags;
    GXColor amb_color;
    GXColor mat_color;
    u8 enable;
    u32 amb_src;
    u32 mat_src;
    u32 light_mask;
    u32 diff_fn;
    u32 attn_fn;
    HSD_AObj* aobj;
} HSD_Chan;

typedef struct HSD_TevConf {
    u32 clr_op;
    u32 clr_a;
    u32 clr_b;
    u32 clr_c;
    u32 clr_d;
    u32 clr_scale;
    u32 clr_bias;
    u8 clr_clamp;
    u32 clr_out_reg;
    u32 alpha_op;
    u32 alpha_a;
    u32 alpha_b;
    u32 alpha_c;
    u32 alpha_d;
    u32 alpha_scale;
    u32 alpha_bias;
    u8 alpha_clamp;
    u32 alpha_out_reg;
    u32 mode;
    u32 ras_swap;
    u32 tex_swap;
    u32 kcsel;
    u32 kasel;
    u32 swap_r;
    u32 swap_g;
    u32 swap_b;
    u32 swap_a;
} HSD_TevConf;

typedef struct HSD_TevDesc {
    struct HSD_TevDesc* next;
    u32 flags;
    u32 stage;
    u32 coord;
    u32 map;
    u32 color;
    HSD_TevConf tevconf;
} HSD_TevDesc;

struct HSD_Shadow {
    /* 0x00 */ HSD_SList* objects;
    /* 0x04 */ HSD_CObj* camera;
    /* 0x08 */ HSD_TObj* texture;
    /* 0x0C */ f32 scaleS;
    /* 0x10 */ f32 scaleT;
    /* 0x14 */ f32 transS;
    /* 0x18 */ f32 transT;
    /* 0x1C */ BOOL active;
    /* 0x20 */ u8 intensity;
    /* 0x24 */ void* user_data;
};

typedef struct HSD_ViewingRect {
    Vec3 origin;
    Vec3 up_v;
    Vec3 right_v;
    Vec3 eye_v;
    Vec3 eye_vn;
    f32 distance;
    f32 top;
    f32 bottom;
    f32 left;
    f32 right;
    int perspective;
} HSD_ViewingRect;

#define F32_MAX 3.4028235e38F

#define PROJ_PERSPECTIVE 1
#define PROJ_FRUSTUM 2
#define PROJ_ORTHO 3

#define RENDER_SHADOW (1 << 26)
#define HSD_TRSP_OPA 1
#define HSD_TRSP_TEXEDGE 4

/* shadow_alloc_data */
HSD_ObjAllocData lbl_804656E0;

/* Draws the background rectangle in three passes when set. */
static u8 lbl_8047B310;

static void makeMatrix(HSD_Shadow* shadow);
void fn_801B06DC(HSD_Shadow* shadow);

HSD_ObjAllocData* HSD_ShadowGetAllocData(void)
{
    return &lbl_804656E0;
}

/* HSD_ShadowInitAllocData */
void HSD_ShadowInitAllocData(void)
{
    HSD_ObjAllocInit(HSD_ShadowGetAllocData(), sizeof(HSD_Shadow), 4);
}

static HSD_TObj* makeShadowTObj(void)
{
    HSD_TObj* shadowTObj;
    shadowTObj = HSD_TObjAlloc();
    shadowTObj->src = 0;
    shadowTObj->wrap_s = 0;
    shadowTObj->wrap_t = 0;
    shadowTObj->flags = 0x540103;
    shadowTObj->imagedesc = HSD_ImageDescAlloc();
    return shadowTObj;
}

/* HSD_ShadowAlloc */
HSD_Shadow* HSD_ShadowAlloc(void)
{
    HSD_Shadow* shadow;

    shadow = HSD_ObjAlloc(HSD_ShadowGetAllocData());
    memset(shadow, 0, sizeof(HSD_Shadow));
    shadow->camera = HSD_CObjAlloc();
    shadow->texture = makeShadowTObj();

    shadow->scaleS = 0.5F;
    shadow->scaleT = -0.5F;
    shadow->transS = 0.5F;
    shadow->transT = 0.5F;
    shadow->intensity = 0;

    shadow->texture->imagedesc->format = 0;
    shadow->texture->imagedesc->width = 256;
    shadow->texture->imagedesc->height = 256;
    shadow->texture->imagedesc->image_ptr = NULL;

    HSD_CObjSetViewportfx4(shadow->camera, 0.0F, 256.0F, 0.0F, 256.0F);
    HSD_CObjSetScissorx4(shadow->camera, 0, 256, 0, 256);

    return shadow;
}

void HSD_ShadowInit(HSD_Shadow* shadow)
{
    HSD_ImageDesc* imagedesc;

    HSD_ASSERT(268, shadow);
    imagedesc = shadow->texture->imagedesc;
    GXSetTexCopySrc(0, 0, imagedesc->width, imagedesc->height);
    GXSetTexCopyDst(imagedesc->width, imagedesc->height, 0x20, 0);
}

/* HSD_ShadowSetSize */
void HSD_ShadowSetSize(HSD_Shadow* shadow, u16 width, u16 height)
{
    u32 size;
    HSD_ImageDesc* idesc;

    HSD_ASSERT(300, shadow);
    HSD_ASSERT(301, width > 0);
    HSD_ASSERT(302, height > 0);

    idesc = shadow->texture->imagedesc;
    if (!idesc->image_ptr || idesc->width != width || idesc->height != height)
    {
        if (idesc->image_ptr) {
            fn_801B06DC(shadow);
        }

        size = GXGetTexBufferSize(width, height, 0, 0, 0);
        HSD_ASSERT(319, size > 0);
        idesc->image_ptr = GSmemLock(fn_800E2B00(size, 0x20));
        idesc->width = width;
        idesc->height = height;

        HSD_CObjSetViewportfx4(shadow->camera, 0, width, 0, height);
        HSD_CObjSetScissorx4(shadow->camera, 0, width, 0, height);
    }
}

static void drawBackgroundRect(HSD_Shadow* shadow)
{
    f32 top, bottom, left, right, near;
    HSD_CObj* cobj = shadow->camera;

    GXLoadPosMtxImm(lbl_8036CBC0, 0);
    HSD_PerfCountMtxLoad();
    GXSetCurrentMtx(0);
    HSD_ClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    HSD_StateSetCullMode(2);

    top = HSD_CObjGetTop(cobj);
    bottom = HSD_CObjGetBottom(cobj);
    left = HSD_CObjGetLeft(cobj);
    right = HSD_CObjGetRight(cobj);
    near = HSD_CObjGetNear(cobj);

    top *= 1.2F;
    bottom *= 1.2F;
    left *= 1.2F;
    right *= 1.2F;
    near *= -1.1F;

    GXBegin(0x80, 0, 4);
    GXPosition3f32(left, top, near);
    GXPosition3f32(right, top, near);
    GXPosition3f32(right, bottom, near);
    GXPosition3f32(left, bottom, near);
    GXEnd();
}

/* HSD_ShadowStartRender */
void HSD_ShadowStartRender(HSD_Shadow* shadow)
{
    HSD_CObj* cobj;
    HSD_ImageDesc* idesc;
    HSD_SList* list;

    HSD_ASSERT(397, shadow);
    HSD_ASSERT(398, shadow->camera);
    HSD_ASSERT(399, shadow->texture);
    HSD_ASSERT(400, shadow->texture->imagedesc);

    cobj = shadow->camera;
    idesc = shadow->texture->imagedesc;

    if (shadow->objects != NULL) {
        HSD_CObjSetCurrent(cobj);
        {
            static HSD_Chan chan = {
                NULL, 4, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 255 }, 0, 0, 0, 0,
                2,    2, NULL,
            };
            static HSD_TevDesc tev = {
                NULL, 1, 0, 0xFF, 0xFF, 4,
                {
                    0, 0xF, 0xF, 0xF, 0xA, 0, 0, 1, 0, 0, 7, 7, 7, 5, 0, 0,
                    1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3,
                },
            };
            static HSD_PEDesc pedesc = {
                9, 0, 0, 0, 0, 4, 5, 0xF, 7, 7, 0, 7,
            };

            HSD_StateInitTev();
            HSD_SetupTevStageAll(&tev);

            HSD_StateSetNumTexGens();

            HSD_SetupPEMode(0, &pedesc);

            chan.mat_color.r = 255;
            chan.mat_color.g = 255;
            chan.mat_color.b = 255;

            HSD_SetupChannelAll(&chan);
            GXSetScissor(0, 0, idesc->width, idesc->height);

            drawBackgroundRect(shadow);

            if (lbl_8047B310) {
                chan.mat_color.r = 0;
                chan.mat_color.g = 0;
                chan.mat_color.b = 0;

                HSD_SetupChannelAll(&chan);
                GXSetScissor(2, 2, idesc->width - 4, idesc->height - 4);

                drawBackgroundRect(shadow);

                chan.mat_color.r = 255;
                chan.mat_color.g = 255;
                chan.mat_color.b = 255;

                HSD_SetupChannelAll(&chan);
                GXSetScissor(4, 4, idesc->width - 8, idesc->height - 8);

                drawBackgroundRect(shadow);
            }

            chan.mat_color.r = shadow->intensity;
            chan.mat_color.g = shadow->intensity;
            chan.mat_color.b = shadow->intensity;

            HSD_SetupChannelAll(&chan);
            GXSetScissor(2, 2, idesc->width - 4, idesc->height - 4);
        }

        for (list = shadow->objects; list != NULL; list = list->next) {
            HSD_JObjDispAll(list->data, NULL, HSD_TRSP_OPA | HSD_TRSP_TEXEDGE,
                        RENDER_SHADOW);
        }

        HSD_CObjEndCurrent();
    }
}

/* HSD_ShadowEndRender */
void HSD_ShadowEndRender(HSD_Shadow* shadow)
{
    HSD_ImageDesc* idesc;

    HSD_ASSERT(553, shadow);

    idesc = shadow->texture->imagedesc;
    if (!idesc->image_ptr) {
        HSD_ShadowSetSize(shadow, idesc->width, idesc->height);
    }

    GXCopyTex(idesc->image_ptr, 1);
    GXPixModeSync();

    GXInvalidateTexAll();

    makeMatrix(shadow);
}

/* HSD_ShadowSetLight */
void HSD_ShadowSetLight(HSD_Shadow* shadow, HSD_LObj* lobj, f32 distance)
{
    Vec3 eye;
    Vec3 interest;
    Vec3 lpos;

    HSD_ASSERT(589, shadow);
    HSD_ASSERT(590, lobj);

    switch (lobj->flags & LOBJ_TYPE_MASK) {
    case LOBJ_INFINITE:
        HSD_ASSERT(594, distance > 0.0F);
        HSD_CObjGetInterest(shadow->camera, &interest);
        HSD_LObjGetPosition(lobj, &lpos);
        PSVECScale(&lpos, &lpos, distance / PSVECMag(&lpos));
        PSVECAdd(&interest, &lpos, &eye);
        HSD_CObjSetEyePosition(shadow->camera, &eye);
        break;
    case LOBJ_POINT:
    case LOBJ_SPOT:
        HSD_LObjGetPosition(lobj, &eye);
        HSD_CObjSetEyePosition(shadow->camera, &eye);
        break;
    default:
        HSD_ASSERT(610, 0);
    }
}

/* HSD_ShadowSetActive */
void HSD_ShadowSetActive(HSD_Shadow* shadow, BOOL active)
{
    HSD_ImageDesc* idesc;

    HSD_ASSERT(632, shadow);

    if ((shadow->active && active) || (!shadow->active && !active)) {
        return;
    }

    shadow->active = active;
    if (active) {
        idesc = shadow->texture->imagedesc;
        if (!idesc->image_ptr) {
            HSD_ShadowSetSize(shadow, idesc->width, idesc->height);
        }

        HSD_MObjAddShadowTexture(shadow->texture);
    } else {
        HSD_MObjDeleteShadowTexture(shadow->texture);
    }
}

/* HSD_ShadowAddObject */
void HSD_ShadowAddObject(HSD_Shadow* shadow, HSD_JObj* jobj)
{
    HSD_SList* list;

    if (!shadow || !jobj) {
        return;
    }

    for (list = shadow->objects; list; list = list->next) {
        if (list->data == jobj) {
            return;
        }
    }
    shadow->objects = HSD_SListPrepend(shadow->objects, jobj);
    HSD_JObjRef(jobj);
}

/* HSD_ShadowDeleteObject */
void HSD_ShadowDeleteObject(HSD_Shadow* shadow, HSD_JObj* jobj)
{
    if (!shadow) {
        return;
    }

    if (jobj) {
        HSD_SList** list = &(shadow->objects);
        for (; *list; list = &((*list)->next)) {
            if ((*list)->data == jobj) {
                HSD_JObjUnref(jobj);
                (*list) = HSD_SListRemove(*list);
                return;
            }
        }
    } else {
        while (shadow->objects) {
            HSD_JObjUnref(shadow->objects->data);
            shadow->objects = HSD_SListRemove(shadow->objects);
        }
    }
}

/* HSD_ShadowFreeImage */
void fn_801B06DC(HSD_Shadow* shadow)
{
    u16 handle;
    HSD_ImageDesc* idesc = shadow->texture->imagedesc;

    if (idesc->image_ptr) {
        handle = GSmemGetHandleByPtr(idesc->image_ptr);
        fn_800E24B0(handle);
        GSmemFree(handle);
        idesc->image_ptr = NULL;
    }
}

/* HSD_ShadowSetMultiPass */
void fn_801B06D4(u8 multipass)
{
    lbl_8047B310 = multipass;
}

static void makeMatrix(HSD_Shadow* shadow)
{
    Mtx Mprj;

    switch (HSD_CObjGetProjectionType(shadow->camera)) {
    case PROJ_PERSPECTIVE:
        C_MTXLightPerspective(
            Mprj, shadow->camera->projection_param.perspective.fov,
            shadow->camera->projection_param.perspective.aspect,
            shadow->scaleS, shadow->scaleT, shadow->transS, shadow->transT);
        break;

    case PROJ_FRUSTUM:
        C_MTXLightFrustum(Mprj, shadow->camera->projection_param.frustum.top,
                          shadow->camera->projection_param.frustum.bottom,
                          shadow->camera->projection_param.frustum.left,
                          shadow->camera->projection_param.frustum.right,
                          shadow->camera->near, shadow->scaleS,
                          shadow->scaleT, shadow->transS, shadow->transT);
        break;

    case PROJ_ORTHO:
        C_MTXLightOrtho(Mprj, shadow->camera->projection_param.ortho.top,
                        shadow->camera->projection_param.ortho.bottom,
                        shadow->camera->projection_param.ortho.left,
                        shadow->camera->projection_param.ortho.right,
                        shadow->scaleS, shadow->scaleT, shadow->transS,
                        shadow->transT);
        break;

    default:
        HSD_ASSERT(773, 0);
    }

    PSMTXConcat(Mprj, (MtxPtr) HSD_CObjGetViewingMtxPtrDirect(shadow->camera),
                shadow->texture->mtx);
}

/* HSD_ShadowSetViewingRect */
void HSD_ShadowSetViewingRect(HSD_Shadow* shadow, f32 top, f32 bottom, f32 left, f32 right)
{
    HSD_CObj* cobj;
    f32 distance;

    HSD_ASSERT(796, shadow);

    cobj = shadow->camera;
    distance = HSD_CObjGetEyeDistance(cobj);
    HSD_ASSERT(800, distance > 0.0F);

    switch (HSD_CObjGetProjectionType(cobj)) {
    case PROJ_PERSPECTIVE: {
        f32 width, height;

        if (fabs(top) > fabs(bottom)) {
            width = fabs(top);
        } else {
            width = fabs(bottom);
        }
        if (fabs(left) > fabs(right)) {
            height = fabs(left);
        } else {
            height = fabs(right);
        }
        HSD_CObjSetAspect(cobj, height / width);
        HSD_CObjSetFov(cobj, atan2(height, distance));
    } break;

    case PROJ_ORTHO:
        HSD_CObjSetOrtho(cobj, top, bottom, left, right);
        break;

    case PROJ_FRUSTUM: {
        f32 scale = HSD_CObjGetNear(cobj) / distance;
        HSD_ASSERT(829, scale > 0.0F);
        HSD_CObjSetFrustum(cobj, scale * top, scale * bottom, scale * left,
                           scale * right);
    } break;

    default:
        HSD_ASSERT(837, 0);
    }
}

/* HSD_ViewingRectInit */
void HSD_ViewingRectInit(HSD_ViewingRect* rect, Vec3* position, Vec3* interest,
                 Vec3* upvector, int perspective)
{
    Vec3 v;
    HSD_ASSERT(870, rect);

    rect->origin = *position;
    PSVECSubtract(interest, position, &rect->eye_v);
    PSVECNormalize(&rect->eye_v, &rect->eye_vn);
    PSVECNormalize(upvector, &v);
    PSVECCrossProduct(&rect->eye_vn, &v, &rect->right_v);
    PSVECCrossProduct(&rect->right_v, &rect->eye_vn, &rect->up_v);
    rect->distance = PSVECMag(&rect->eye_v);

    rect->top = rect->right = -F32_MAX;
    rect->bottom = rect->left = F32_MAX;
    rect->perspective = perspective;
}

int HSD_ViewingRectCheck(HSD_ViewingRect* rect)
{
    HSD_ASSERT(893, rect);
    return rect->top > rect->bottom && rect->right > rect->left;
}

void HSD_ViewingRectAddRect(HSD_ViewingRect* rect, Vec3* position, f32 top, f32 bottom,
                 f32 left, f32 right);

/*
 * HAL's shadow.c has two more functions here that nothing in the game calls,
 * so the linker strips them. Melee's shadow.c has no bodies for them either
 * (only the same two strings, as unused data). All retail proves is that
 * each asserts one float argument ("a > 0.0F", then "radius > 0.0F", pooled
 * between AddRect's and SetViewingRect's strings), so they are reduced to
 * exactly that; names, other parameters and line numbers are unknown and
 * never reach the binary.
 */
void shadow_stripped_a_assert(f32 a)
{
    HSD_ASSERT(0, a > 0.0F);
}

void shadow_stripped_radius_assert(f32 radius)
{
    HSD_ASSERT(0, radius > 0.0F);
}

/* HSD_ViewingRectAddRect */
void HSD_ViewingRectAddRect(HSD_ViewingRect* rect, Vec3* position, f32 top, f32 bottom,
                 f32 left, f32 right)
{
    f32 x, y, dot, scale;
    Vec3 o2p, e2p;

    HSD_ASSERT(930, rect);
    HSD_ASSERT(931, position);

    PSVECSubtract(position, &rect->origin, &o2p);
    dot = PSVECDotProduct(&o2p, &rect->eye_vn);
    if (rect->perspective) {
        if (dot <= 0.0F) {
            return;
        }
        scale = rect->distance / dot;
        PSVECScale(&o2p, &o2p, scale);
        PSVECSubtract(&o2p, &rect->eye_v, &e2p);
        x = PSVECDotProduct(&rect->right_v, &e2p);
        y = PSVECDotProduct(&rect->up_v, &e2p);

        top *= scale;
        bottom *= scale;
        left *= scale;
        right *= scale;
    } else {
        Vec3 tmp;
        PSVECScale(&rect->eye_vn, &tmp, dot);
        PSVECSubtract(&o2p, &tmp, &e2p);
        x = PSVECDotProduct(&rect->right_v, &e2p);
        y = PSVECDotProduct(&rect->up_v, &e2p);
    }

    if (x + right > rect->right) {
        rect->right = x + right;
    }
    if (x + left < rect->left) {
        rect->left = x + left;
    }
    if (y + top > rect->top) {
        rect->top = y + top;
    }
    if (y + bottom < rect->bottom) {
        rect->bottom = y + bottom;
    }
}
