/**
 * @file lobj.c
 * @brief HAL sysdolphin lobj.c: light objects, 0x801A4000-0x801A6928.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/lobj.c) and checked against Colosseum's retail
 * code, which is the newer sysdolphin:
 * - the class info carries the AObj update callback, which HSD_LObjAnim
 *   calls through the class;
 * - lights are enabled by the DIFFUSE/ALPHA/SPECULAR reference bits, the
 *   ambient light is picked from the remaining list, and specular lights
 *   are assigned from the active lights;
 * - light vectors are normalised with a degenerate-vector check that falls
 *   back to a fixed axis, and spot lights clamp their reference distance;
 * - HSD_LObjAddCurrentAll, HSD_LObjRemoveAnimAll and the GXLightObj
 *   shininess macro are new, SetDefaultClass is gone.
 * The library is built with deferred inlining, so functions are listed in
 * HAL's order and MWCC emits them in reverse (the retail address order).
 * Functions nothing in the game references are compiled and dead-stripped
 * by the linker as in retail.
 *
 * Globals other objects already link against keep their dtk names; the
 * comments give the HAL names. The GX light calls are fn_ symbols in
 * Colosseum's map; their SDK names are given where they are declared.
 */
#include "dolphin/gx/GX.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_wobj.h"
#include "crt/float.h"
#include "crt/math_ppc.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/lobj.h"
#include "sysdolphin/baselib/object.h"

void OSReport(const char* msg, ...);
void PSMTXMultVecSR(const Mtx m, const Vec3* src, Vec3* dst);
void PSVECAdd(const Vec3* a, const Vec3* b, Vec3* ab);
void PSVECSubtract(const Vec3* a, const Vec3* b, Vec3* ab);
void PSVECNormalize(const Vec3* src, Vec3* unit);

/* GXInitLightAttn */
void GXInitLightAttn(GXLightObj* lt_obj, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1,
                 f32 k2);
void GXInitLightSpot(GXLightObj* lt_obj, f32 cutoff, s32 spot_func);
void GXInitLightDistAttn(GXLightObj* lt_obj, f32 ref_dist, f32 ref_br,
                         s32 dist_func);
/* GXInitLightPos */
void GXInitLightPos(GXLightObj* lt_obj, f32 x, f32 y, f32 z);
/* GXInitLightDir */
void GXInitLightDir(GXLightObj* lt_obj, f32 nx, f32 ny, f32 nz);
void GXLoadLightObjImm(GXLightObj* lt_obj, s32 light);

/* SDK GXLighting.h macro */
#define GXInitLightShininess(lobj, shininess)                              \
    (GXInitLightAttn(lobj, 0.0F, 0.0F, 1.0F, (shininess) / 2.0F, 0.0F,         \
                 1.0F - (shininess) / 2.0F))

/* hsdSearchClassInfo */
HSD_ClassInfo* fn_80193748(const char* class_name);
/* hsdNew */
void* fn_80193828(HSD_ClassInfo* info);
/* HSD_SListRemove */
HSD_SList* HSD_SListRemove(HSD_SList* list);
/* HSD_SListAllocAndPrepend */
HSD_SList* HSD_SListPrepend(HSD_SList* list, void* data);

#define GX_LIGHT_NULL 0
#define GX_LIGHT0 (1 << 0)
#define GX_LIGHT1 (1 << 1)
#define GX_LIGHT2 (1 << 2)
#define GX_LIGHT3 (1 << 3)
#define GX_LIGHT4 (1 << 4)
#define GX_LIGHT5 (1 << 5)
#define GX_LIGHT6 (1 << 6)
#define GX_LIGHT7 (1 << 7)
#define GX_MAX_LIGHT (1 << 8)

static void LObjInfoInit(void);

/* hsdLObj */
HSD_LObjInfo lbl_8036CA20 = { LObjInfoInit };
#define hsdLObj lbl_8036CA20

static HSD_LObjInfo* default_class = NULL;

static HSD_SList* current_lights = NULL;
/* active_lights */
HSD_LObj* lbl_804655E0[MAX_GXLIGHT];
#define active_lights lbl_804655E0
static s32 nb_active_lights = 0;

static s32 lightmask_diffuse = 0;
static s32 lightmask_specular = 0;
static s32 lightmask_attnfunc = 0;
static s32 lightmask_alpha = 0;

/*
 * The degenerate-vector check the newer library normalises with (Melee's
 * util.h form, as in cobj.c).
 */
static inline int vec_normalize_check(Vec3* src, Vec3* dst)
{
    if (!src || !dst) {
        return -1;
    }
    if (fabs(src->x) <= FLT_MIN && fabs(src->y) <= FLT_MIN &&
        fabs(src->z) <= FLT_MIN)
    {
        return -1;
    }
    PSVECNormalize(src, dst);
    return 0;
}

u32 HSD_LObjGetFlags(HSD_LObj* lobj)
{
    return lobj ? lobj->flags : 0;
}

void HSD_LObjSetFlags(HSD_LObj* lobj, u32 flags)
{
    if (lobj == NULL) {
        return;
    }
    lobj->flags |= flags;
}

void HSD_LObjClearFlags(HSD_LObj* lobj, u32 flags)
{
    if (lobj == NULL) {
        return;
    }
    lobj->flags &= ~flags;
}

u32 HSD_LObjGetLightMaskDiffuse(void)
{
    return lightmask_diffuse;
}

s32 HSD_LObjGetLightMaskAttnFunc(void)
{
    return lightmask_attnfunc;
}

u32 HSD_LObjGetLightMaskAlpha(void)
{
    return lightmask_alpha;
}

u32 HSD_LObjGetLightMaskSpecular(void)
{
    return lightmask_specular;
}

u32 HSD_LObjGetType(HSD_LObj* lobj)
{
    return lobj->flags & LOBJ_TYPE_MASK;
}

s32 HSD_Index2LightID(u32 index);
u32 HSD_LightID2Index(s32 id);

/*
 * Retail has no standalone copy of this (its Index2LightID jump table would
 * sit in .data), so the newer library keeps it as an inline.
 */
static inline void HSD_LObjSetActive(HSD_LObj* lobj)
{
    int idx;

    if (HSD_LObjGetType(lobj) == LOBJ_AMBIENT) {
        idx = MAX_GXLIGHT - 1;
        if (active_lights[idx]) {
            return;
        }
    } else {
        idx = nb_active_lights++;
    }
    active_lights[idx] = lobj;
    lobj->id = HSD_Index2LightID(idx);
}

s32 HSD_LObjGetNbActive(void)
{
    return nb_active_lights;
}

HSD_LObj* HSD_LObjGetActiveByID(s32 id)
{
    s32 idx = HSD_LightID2Index(id);
    if (0 <= idx && idx < MAX_GXLIGHT) {
        return active_lights[idx];
    } else {
        return NULL;
    }
}

HSD_LObj* HSD_LObjGetActiveByIndex(s32 idx)
{
    if (0 <= idx && idx < MAX_GXLIGHT - 1) {
        return active_lights[idx];
    } else {
        return NULL;
    }
}

void HSD_LObjClearActive(void)
{
    int i;

    for (i = 0; i < MAX_GXLIGHT; i++) {
        active_lights[i] = NULL;
    }
    nb_active_lights = 0;
}

HSD_WObj* HSD_LObjGetPositionWObj(HSD_LObj* lobj);
HSD_WObj* HSD_LObjGetInterestWObj(HSD_LObj* lobj);

void HSD_LObjRemoveAnimByFlags(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        HSD_AObjRemove(lobj->aobj);
        lobj->aobj = NULL;
        HSD_WObjRemoveAnim(HSD_LObjGetPositionWObj(lobj));
        HSD_WObjRemoveAnim(HSD_LObjGetInterestWObj(lobj));
    }
}

void HSD_LObjRemoveAnim(HSD_LObj* lobj)
{
    HSD_LObj* lp;

    if (lobj == NULL) {
        return;
    }
    for (lp = lobj; lp != NULL; lp = lp->next) {
        HSD_LObjRemoveAnimByFlags(lp);
    }
}

void HSD_LObjRemoveAnimAll(HSD_LObj* lobj)
{
    HSD_LObj* lp;

    if (lobj == NULL) {
        return;
    }
    for (lp = lobj; lp != NULL; lp = lp->next) {
        HSD_LObjRemoveAnim(lp);
    }
}

void LObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_LObj* lobj = obj;
    f32 fv;

    if (lobj == NULL) {
        return;
    }

    switch (type) {
    case HSD_A_L_VIS:
        if (val->fv >= 0.5) {
            lobj->flags &= ~LOBJ_HIDDEN;
        } else {
            lobj->flags |= LOBJ_HIDDEN;
        }
        break;
    case HSD_A_L_A0:
    case HSD_A_L_CUTOFF:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a0 = val->fv;
        } else {
            lobj->u.spot.cutoff = val->fv;
        }
        break;
    case HSD_A_L_A1:
    case HSD_A_L_REFDIST:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a1 = val->fv;
        } else {
            lobj->u.spot.ref_dist = val->fv;
        }
        break;
    case HSD_A_L_A2:
    case HSD_A_L_REFBRIGHT:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a2 = val->fv;
        } else {
            lobj->u.spot.ref_br = val->fv;
        }
        break;
    case HSD_A_L_K0:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k0 = val->fv;
        }
        break;
    case HSD_A_L_K1:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k1 = val->fv;
        }
        break;
    case HSD_A_L_K2:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k2 = val->fv;
        }
        break;
    case HSD_A_L_LITC_R:
        fv = val->fv;
        if (fv <= 0.0F) {
            fv = 0.0F;
        } else if (fv >= 1.0F) {
            fv = 1.0F;
        }
        lobj->color.r = 255.0F * fv;
        break;
    case HSD_A_L_LITC_G:
        fv = val->fv;
        if (fv <= 0.0F) {
            fv = 0.0F;
        } else if (fv >= 1.0F) {
            fv = 1.0F;
        }
        lobj->color.g = 255.0F * fv;
        break;
    case HSD_A_L_LITC_B:
        fv = val->fv;
        if (fv <= 0.0F) {
            fv = 0.0F;
        } else if (fv >= 1.0F) {
            fv = 1.0F;
        }
        lobj->color.b = 255.0F * fv;
        break;
    case HSD_A_L_LITC_A:
        fv = val->fv;
        if (fv <= 0.0F) {
            fv = 0.0F;
        } else if (fv >= 1.0F) {
            fv = 1.0F;
        }
        lobj->color.a = 255.0F * fv;
        break;
    }
}

void HSD_LObjAnim(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        HSD_AObjInterpretAnim(lobj->aobj, lobj,
                              (void*) HSD_LOBJ_METHOD(lobj)->update);
        HSD_WObjInterpretAnim(HSD_LObjGetPositionWObj(lobj));
        HSD_WObjInterpretAnim(HSD_LObjGetInterestWObj(lobj));
    }
}

void HSD_LObjAnimAll(HSD_LObj* lobj)
{
    HSD_LObj* lp;

    if (lobj == NULL) {
        return;
    }

    for (lp = lobj; lp; lp = lp->next) {
        HSD_LObjAnim(lp);
    }
}

void HSD_LObjReqAnim(HSD_LObj* lobj, f32 startframe)
{
    if (lobj == NULL) {
        return;
    }

    HSD_AObjReqAnim(lobj->aobj, startframe);
    HSD_WObjReqAnim(HSD_LObjGetPositionWObj(lobj), startframe);
    HSD_WObjReqAnim(HSD_LObjGetInterestWObj(lobj), startframe);
}

void HSD_LObjReqAnimAll(HSD_LObj* lobj, f32 startframe)
{
    HSD_LObj* lp;

    if (lobj == NULL) {
        return;
    }

    for (lp = lobj; lp; lp = lp->next) {
        HSD_LObjReqAnim(lp, startframe);
    }
}

void HSD_LObjGetLightVector(HSD_LObj* lobj, Vec3* dir)
{
    Vec3 position = { 0.0F, 0.0F, 0.0F };
    Vec3 interest = { 0.0F, 0.0F, 0.0F };

    if (lobj == NULL) {
        return;
    }

    HSD_LObjGetPosition(lobj, &position);
    HSD_LObjGetInterest(lobj, &interest);
    PSVECSubtract(&interest, &position, dir);
    if (vec_normalize_check(dir, dir)) {
        dir->x = 0.0F;
        dir->y = 0.0F;
        dir->z = 1.0F;
    }
}

/* HSD_LObjSetup */
void HSD_LObjSetupSpecular(HSD_LObj* lobj, GXColor color, f32 shininess)
{
    if (lobj->flags & LOBJ_HIDDEN || HSD_LObjGetType(lobj) == LOBJ_AMBIENT) {
        return;
    }

    if (lobj->flags & (LOBJ_DIFFUSE | LOBJ_ALPHA)) {
        if (lobj->hw_color.r != color.r || lobj->hw_color.g != color.g ||
            lobj->hw_color.b != color.b || lobj->hw_color.a != color.a)
        {
            GXInitLightColor(&lobj->lightobj, color);
            lobj->hw_color = color;
            lobj->flags |= LOBJ_DIFF_DIRTY;
        }

        if (lobj->flags & LOBJ_DIFF_DIRTY) {
            GXLoadLightObjImm(&lobj->lightobj, lobj->id);
            lobj->flags &= ~LOBJ_DIFF_DIRTY;
        }
    }

    if (lobj->spec_id != GX_LIGHT_NULL) {
        if (lobj->shininess != shininess) {
            lobj->shininess = shininess;
            GXInitLightShininess(&lobj->spec_lightobj, shininess);
            lobj->flags |= LOBJ_SPEC_DIRTY;
        }

        if (lobj->flags & LOBJ_SPEC_DIRTY) {
            GXLoadLightObjImm(&lobj->spec_lightobj, lobj->spec_id);
            lobj->flags &= ~LOBJ_SPEC_DIRTY;
        }
    }
}

/* HSD_LObjSetupSpecularInit */
void HSD_LObjSetupSpecularInit(MtxPtr pmtx)
{
    int i;
    s32 num;
    Vec3 cdir;
    Vec3 jpos;

    jpos.x = pmtx[0][3];
    jpos.y = pmtx[1][3];
    jpos.z = pmtx[2][3];
    if (vec_normalize_check(&jpos, &cdir)) {
        cdir.x = 0.0F;
        cdir.y = 0.0F;
        cdir.z = -1.0F;
    }

    num = HSD_LObjGetNbActive();
    for (i = 0; i < num; i++) {
        Vec3 half, ldir;
        HSD_LObj* lobj = HSD_LObjGetActiveByIndex(i);

        if (lobj->spec_id == GX_LIGHT_NULL) {
            continue;
        }

        switch (HSD_LObjGetType(lobj)) {
        case LOBJ_POINT:
        case LOBJ_SPOT:
            /* A degenerate light direction takes the fixed half vector
             * straight to the light object: retail skips the second
             * normalisation on that path. */
            PSVECSubtract(&jpos, &lobj->lvec, &ldir);
            if (vec_normalize_check(&ldir, &ldir) == 0) {
                PSVECAdd(&ldir, &cdir, &half);
            } else {
                half.x = 0.0F;
                half.y = 0.0F;
                half.z = 1.0F;
                goto set_dir;
            }
            break;

        case LOBJ_INFINITE:
            PSVECAdd(&lobj->lvec, &cdir, &half);
            break;

        default:
            HSD_ASSERT(634, 0);
        }
        if (vec_normalize_check(&half, &half)) {
            half.x = 0.0F;
            half.y = 0.0F;
            half.z = 1.0F;
        }
    set_dir:
        GXInitLightDir(&lobj->spec_lightobj, half.x, half.y, half.z);
        lobj->flags |= LOBJ_SPEC_DIRTY;
    }
}

static void setup_diffuse_lightobj(HSD_LObj* lobj)
{
    GXInitLightColor(&lobj->lightobj, lobj->color);
    lobj->hw_color = lobj->color;
    lobj->flags |= LOBJ_DIFF_DIRTY;

    switch (HSD_LObjGetType(lobj)) {
    case LOBJ_SPOT:
    case LOBJ_POINT:
        lightmask_attnfunc |= lobj->id;
        break;
    case LOBJ_INFINITE:
        break;
    default:
        HSD_ASSERT(664, 0);
    }

    if (lobj->flags & LOBJ_DIFFUSE) {
        lightmask_diffuse |= lobj->id;
    }
    if (lobj->flags & LOBJ_ALPHA) {
        lightmask_alpha |= lobj->id;
    }
}

static void setup_spec_lightobj(HSD_LObj* lobj, MtxPtr mtx, s32 spec_id)
{
    lobj->spec_id = spec_id;
    if (spec_id != GX_LIGHT_NULL) {
        GXInitLightColor(&lobj->spec_lightobj, lobj->color);
        lobj->shininess = 50.0F;
        GXInitLightShininess(&lobj->spec_lightobj, lobj->shininess);
        switch (HSD_LObjGetType(lobj)) {
        case LOBJ_POINT:
        case LOBJ_SPOT:
            HSD_LObjGetPosition(lobj, &lobj->lvec);
            PSMTXMultVec(mtx, &lobj->lvec, &lobj->lvec);
            break;
        case LOBJ_INFINITE:
            HSD_LObjGetLightVector(lobj, &lobj->lvec);
            PSMTXMultVecSR(mtx, &lobj->lvec, &lobj->lvec);
            PSVECNormalize(&lobj->lvec, &lobj->lvec);
            break;
        default:
            HSD_ASSERT(704, 0);
        }
        lobj->flags |= LOBJ_SPEC_DIRTY;
        lightmask_specular |= spec_id;
    }
}

static void setup_infinite_lightobj(HSD_LObj* lobj, MtxPtr vmtx)
{
    Vec3 lpos;

    HSD_LObjGetPosition(lobj, &lpos);
    lpos.x *= 1048576.0F;
    lpos.y *= 1048576.0F;
    lpos.z *= 1048576.0F;
    PSMTXMultVec(vmtx, &lpos, &lpos);
    if (lobj->flags & LOBJ_DIFFUSE) {
        GXInitLightPos(&lobj->lightobj, lpos.x, lpos.y, lpos.z);
        GXInitLightAttn(&lobj->lightobj, 1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
    }
    if (lobj->flags & LOBJ_SPECULAR) {
        GXInitLightPos(&lobj->spec_lightobj, lpos.x, lpos.y, lpos.z);
    }
}

static void setup_point_lightobj(HSD_LObj* lobj, MtxPtr mtx)
{
    Vec3 lpos;

    GXInitLightColor(&lobj->lightobj, lobj->color);
    lobj->hw_color = lobj->color;
    HSD_LObjGetPosition(lobj, &lpos);
    PSMTXMultVec(mtx, &lpos, &lpos);
    GXInitLightPos(&lobj->lightobj, lpos.x, lpos.y, lpos.z);
    GXInitLightPos(&lobj->spec_lightobj, lpos.x, lpos.y, lpos.z);
    if (lobj->flags & LOBJ_RAW_PARAM) {
        GXInitLightAttn(&lobj->lightobj, 1.0F, 0.0F, 0.0F, lobj->u.attn.k0,
                    lobj->u.attn.k1, lobj->u.attn.k2);
    } else {
        f32 ref_br = lobj->u.spot.ref_br;
        f32 ref_dist = lobj->u.spot.ref_dist;
        s32 dist_func = lobj->u.spot.dist_func;
        GXInitLightDistAttn(&lobj->lightobj, ref_dist, ref_br, dist_func);
        GXInitLightSpot(&lobj->lightobj, 0.0F, 0);
        GXInitLightDistAttn(&lobj->spec_lightobj, ref_dist, ref_br,
                            dist_func);
    }
}

static void setup_spot_lightobj(HSD_LObj* lobj, MtxPtr mtx)
{
    Vec3 lpos;
    Vec3 ldir;

    HSD_LObjGetPosition(lobj, &lpos);
    PSMTXMultVec(mtx, &lpos, &lpos);
    HSD_LObjGetLightVector(lobj, &ldir);
    PSMTXMultVecSR(mtx, &ldir, &ldir);
    PSVECNormalize(&ldir, &ldir);
    GXInitLightPos(&lobj->lightobj, lpos.x, lpos.y, lpos.z);
    GXInitLightPos(&lobj->spec_lightobj, lpos.x, lpos.y, lpos.z);
    GXInitLightDir(&lobj->lightobj, ldir.x, ldir.y, ldir.z);
    if (lobj->flags & LOBJ_RAW_PARAM) {
        GXInitLightAttn(&lobj->lightobj, lobj->u.attn.a0, lobj->u.attn.a1,
                    lobj->u.attn.a2, lobj->u.attn.k0, lobj->u.attn.k1,
                    lobj->u.attn.k2);
    } else {
        f32 ref_br = lobj->u.point.ref_br;
        f32 ref_dist = lobj->u.point.ref_dist;
        f32 cutoff = lobj->u.point.cutoff;
        s32 spot_func = lobj->u.spot.spot_func;
        s32 dist_func = lobj->u.spot.dist_func;

        if (ref_dist < 0.001) {
            ref_dist = 0.001;
        }
        GXInitLightDistAttn(&lobj->lightobj, ref_dist, ref_br, dist_func);
        GXInitLightDistAttn(&lobj->spec_lightobj, ref_dist, ref_br,
                            dist_func);
        GXInitLightSpot(&lobj->lightobj, cutoff, spot_func);
    }
}

/* HSD_LObjSetupInit */
void HSD_LObjSetup(HSD_CObj* cobj)
{
    MtxPtr vmtx;
    int i, num, idx;
    HSD_SList* list;

    idx = 0;

    lightmask_diffuse = GX_LIGHT_NULL;
    lightmask_specular = GX_LIGHT_NULL;
    lightmask_attnfunc = GX_LIGHT_NULL;
    lightmask_alpha = GX_LIGHT_NULL;
    vmtx = (MtxPtr) HSD_CObjGetViewingMtxPtrDirect(cobj);

    HSD_LObjClearActive();

    for (list = current_lights; idx < MAX_GXLIGHT - 1 && list;
         list = list->next)
    {
        HSD_LObj* lobj = list->data;
        u32 type;
        u32 ref_type;

        if (lobj == NULL || (lobj->flags & LOBJ_HIDDEN)) {
            continue;
        }

        type = HSD_LObjGetType(lobj);
        ref_type = lobj->flags & (LOBJ_DIFFUSE | LOBJ_SPECULAR | LOBJ_ALPHA);
        if (ref_type == 0) {
            continue;
        }

        HSD_LObjSetActive(lobj);
        idx = HSD_LObjGetNbActive();

        lobj->spec_id = GX_LIGHT_NULL;

        switch (type) {
        case LOBJ_INFINITE:
            setup_infinite_lightobj(lobj, vmtx);
            break;
        case LOBJ_POINT:
            setup_point_lightobj(lobj, vmtx);
            break;
        case LOBJ_SPOT:
            setup_spot_lightobj(lobj, vmtx);
            break;
        case LOBJ_AMBIENT:
            continue;
        }

        if (ref_type & (LOBJ_DIFFUSE | LOBJ_ALPHA)) {
            setup_diffuse_lightobj(lobj);
        } else if (ref_type & LOBJ_SPECULAR) {
            setup_spec_lightobj(lobj, vmtx, lobj->id);
        }
    }

    if (!HSD_LObjGetActiveByID(GX_MAX_LIGHT)) {
        for (; list; list = list->next) {
            HSD_LObj* lobj = list->data;

            if (lobj != NULL && !(lobj->flags & LOBJ_HIDDEN) &&
                HSD_LObjGetType(lobj) == LOBJ_AMBIENT &&
                (lobj->flags & (LOBJ_DIFFUSE | LOBJ_ALPHA)))
            {
                HSD_LObjSetActive(lobj);
                break;
            }
        }
    }

    num = HSD_LObjGetNbActive();
    for (i = 0; idx < MAX_GXLIGHT - 1 && i < num; i++) {
        HSD_LObj* lobj = HSD_LObjGetActiveByIndex(i);
        u32 flags;

        if (lobj == NULL) {
            continue;
        }
        flags = lobj->flags;
        if (!(flags & LOBJ_SPECULAR) || !(flags & (LOBJ_DIFFUSE | LOBJ_ALPHA))) {
            continue;
        }
        setup_spec_lightobj(lobj, vmtx, HSD_Index2LightID(idx++));
    }

    for (i = 0; i < num; i++) {
        HSD_LObj* lobj = HSD_LObjGetActiveByIndex(i);

        if (lobj == NULL) {
            continue;
        }
        HSD_LObjSetupSpecular(lobj, lobj->color, lobj->shininess);
    }
}

void HSD_LObjDeleteCurrent(HSD_LObj* lobj);

void HSD_LObjAddCurrent(HSD_LObj* lobj)
{
    HSD_SList* node;
    HSD_SList** p;

    if (lobj != NULL) {
        node = current_lights;
        while (node != NULL) {
            if (node->data == lobj) {
                HSD_LObjDeleteCurrent(lobj);
                break;
            }
            node = node->next;
        }
        ref_INC(lobj);
        for (p = &current_lights; *p != NULL; p = &(*p)->next) {
            if (HSD_LObjGetPriority((*p)->data) > HSD_LObjGetPriority(lobj)) {
                break;
            }
        }
        *p = HSD_SListPrepend(*p, lobj);
    }
}

void HSD_LObjAddCurrentAll(HSD_LObj* lobj)
{
    HSD_LObj* lp;

    for (lp = lobj; lp != NULL; lp = lp->next) {
        HSD_LObjAddCurrent(lp);
    }
}

void HSD_LObjUnrefThis(HSD_LObj* lobj)
{
    if (lobj != NULL && ref_DEC(lobj)) {
        if (lobj != NULL) {
            HSD_OBJECT_METHOD(lobj)->release((HSD_Class*) lobj);
            HSD_OBJECT_METHOD(lobj)->destroy((HSD_Class*) lobj);
        }
    }
}

void HSD_LObjDeleteCurrent(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        HSD_SList** p;
        for (p = &current_lights; *p != NULL; p = &(*p)->next) {
            if ((*p)->data == lobj) {
                int i;
                for (i = 0; i < MAX_GXLIGHT; i++) {
                    if (lobj == active_lights[i]) {
                        active_lights[i] = NULL;
                    }
                }
                *p = HSD_SListRemove(*p);
                HSD_LObjUnrefThis(lobj);
                return;
            }
        }
    }
}

static inline void LObjRemoveAll(void)
{
    int i;
    for (i = 0; i < MAX_GXLIGHT; i++) {
        active_lights[i] = NULL;
    }
    nb_active_lights = 0;
    while (current_lights != NULL) {
        HSD_LObjUnrefThis(current_lights->data);
        current_lights = HSD_SListRemove(current_lights);
    }
}

void HSD_LObjDeleteCurrentAll(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        while (lobj != NULL) {
            HSD_LObjDeleteCurrent(lobj);
            lobj = lobj->next;
        }
        return;
    }
    LObjRemoveAll();
}

HSD_LObj* HSD_LObjGetCurrentByType(u32 type)
{
    HSD_SList* cur = current_lights;
    type &= LOBJ_TYPE_MASK;
    while (cur != NULL) {
        if (type == HSD_LObjGetType(cur->data)) {
            return cur->data;
        }
        cur = cur->next;
    }
    return NULL;
}

u32 HSD_LightID2Index(s32 id)
{
    u32 index;
    switch (id) {
    case GX_LIGHT0:
        index = 0;
        break;
    case GX_LIGHT1:
        index = 1;
        break;
    case GX_LIGHT2:
        index = 2;
        break;
    case GX_LIGHT3:
        index = 3;
        break;
    case GX_LIGHT4:
        index = 4;
        break;
    case GX_LIGHT5:
        index = 5;
        break;
    case GX_LIGHT6:
        index = 6;
        break;
    case GX_LIGHT7:
        index = 7;
        break;
    case GX_MAX_LIGHT:
        index = 8;
        break;
    default:
        HSD_ASSERT(1185, 0);
        break;
    }
    return index;
}

s32 HSD_Index2LightID(u32 index)
{
    switch (index) {
    case 0:
        return GX_LIGHT0;
    case 1:
        return GX_LIGHT1;
    case 2:
        return GX_LIGHT2;
    case 3:
        return GX_LIGHT3;
    case 4:
        return GX_LIGHT4;
    case 5:
        return GX_LIGHT5;
    case 6:
        return GX_LIGHT6;
    case 7:
        return GX_LIGHT7;
    case 8:
        return GX_MAX_LIGHT;
    default:
        return GX_LIGHT_NULL;
    }
}

void HSD_LObjRemoveAll(HSD_LObj* lobj)
{
    HSD_LObj* next;
    HSD_LObj* cur;

    cur = lobj;
    while (cur != NULL) {
        next = cur->next;
        HSD_LObjDeleteCurrent(cur);
        HSD_LObjUnrefThis(cur);
        cur = next;
    }
}

void HSD_LObjSetColor(HSD_LObj* lobj, GXColor color)
{
    lobj->color = color;
}

void HSD_LObjGetColor(HSD_LObj* lobj, GXColor* color)
{
    *color = lobj->color;
}

void HSD_LObjSetSpot(HSD_LObj* lobj, f32 cutoff, s32 spot_func)
{
    if (lobj != NULL) {
        lobj->u.spot.cutoff = cutoff;
        lobj->u.spot.spot_func = spot_func;
    }
}

void HSD_LObjSetDistAttn(HSD_LObj* lobj, f32 ref_dist, f32 ref_br,
                         s32 dist_func)
{
    if (lobj != NULL) {
        lobj->u.spot.ref_dist = ref_dist;
        lobj->u.spot.ref_br = ref_br;
        lobj->u.spot.dist_func = dist_func;
    }
}

void HSD_LObjSetAttnA(HSD_LObj* lobj, f32 a0, f32 a1, f32 a2)
{
    if (lobj == NULL) {
        return;
    }

    lobj->u.attn.a0 = a0;
    lobj->u.attn.a1 = a1;
    lobj->u.attn.a2 = a2;
}

void HSD_LObjSetAttnK(HSD_LObj* lobj, f32 k0, f32 k1, f32 k2)
{
    if (lobj == NULL) {
        return;
    }

    lobj->u.attn.k0 = k0;
    lobj->u.attn.k1 = k1;
    lobj->u.attn.k2 = k2;
}

void HSD_LObjSetAttn(HSD_LObj* lobj, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1,
                     f32 k2)
{
    HSD_LObjSetAttnA(lobj, a0, a1, a2);
    HSD_LObjSetAttnK(lobj, k0, k1, k2);
}

void HSD_LObjSetPosition(HSD_LObj* lobj, Vec3* position)
{
    HSD_ASSERT(1384, lobj);
    if (lobj->position == NULL) {
        lobj->position = HSD_WObjAlloc();
        HSD_ASSERT(1387, lobj->position);
    }
    HSD_WObjSetPosition(lobj->position, position);
}

s32 HSD_LObjGetPosition(HSD_LObj* lobj, Vec3* position)
{
    if (lobj != NULL && lobj->position != NULL) {
        HSD_WObjGetPosition(lobj->position, position);
        return TRUE;
    }
    return FALSE;
}

void HSD_LObjSetInterest(HSD_LObj* lobj, Vec3* interest)
{
    HSD_ASSERT(1420, lobj);
    if (lobj->interest == NULL) {
        lobj->interest = HSD_WObjAlloc();
        HSD_ASSERT(1423, lobj->interest);
    }
    HSD_WObjSetPosition(lobj->interest, interest);
}

s32 HSD_LObjGetInterest(HSD_LObj* lobj, Vec3* interest)
{
    if (lobj != NULL && lobj->interest != NULL) {
        HSD_WObjGetPosition(lobj->interest, interest);
        return TRUE;
    }
    return FALSE;
}

HSD_LObjInfo* HSD_LObjGetDefaultClass(void)
{
    return default_class ? default_class : &hsdLObj;
}

HSD_LObj* HSD_LObjAlloc(void)
{
    HSD_LObj* new = fn_80193828(&HSD_LObjGetDefaultClass()->parent.parent);
    HSD_ASSERT(1493, new);
    return new;
}

HSD_WObj* HSD_LObjGetPositionWObj(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        return lobj->position;
    }
    return NULL;
}

HSD_WObj* HSD_LObjGetInterestWObj(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        return lobj->interest;
    }
    return NULL;
}

void HSD_LObjSetPositionWObj(HSD_LObj* lobj, HSD_WObj* wobj)
{
    if (lobj == NULL) {
        return;
    }

    HSD_WObjUnref(lobj->position);
    lobj->position = wobj;
}

void HSD_LObjSetInterestWObj(HSD_LObj* lobj, HSD_WObj* wobj)
{
    if (lobj == NULL) {
        return;
    }

    HSD_WObjUnref(lobj->interest);
    lobj->interest = wobj;
}

static int LObjLoad(HSD_LObj* lobj, HSD_LightDesc* ldesc)
{
    HSD_LObjSetColor(lobj, ldesc->color);
    HSD_LObjSetFlags(lobj, ldesc->flags);
    switch (ldesc->flags & LOBJ_TYPE_MASK) {
    case LOBJ_AMBIENT:
        break;
    case LOBJ_INFINITE:
        HSD_LObjSetPositionWObj(lobj, HSD_WObjLoadDesc(ldesc->position));
        break;
    case LOBJ_POINT:
        HSD_LObjSetPositionWObj(lobj, HSD_WObjLoadDesc(ldesc->position));
        if (ldesc->attnflags & LOBJ_LIGHT_ATTN) {
            HSD_LObjSetFlags(lobj, LOBJ_RAW_PARAM);
            HSD_LObjSetAttnK(lobj, ldesc->u.attn->k0, ldesc->u.attn->k1,
                             ldesc->u.attn->k2);
        } else {
            HSD_LObjSetDistAttn(lobj, ldesc->u.point->ref_dist,
                                ldesc->u.point->ref_br,
                                ldesc->u.point->dist_func);
        }
        break;
    case LOBJ_SPOT:
        HSD_LObjSetPositionWObj(lobj, HSD_WObjLoadDesc(ldesc->position));
        HSD_LObjSetInterestWObj(lobj, HSD_WObjLoadDesc(ldesc->interest));
        if (ldesc->attnflags & LOBJ_LIGHT_ATTN) {
            HSD_LObjSetFlags(lobj, LOBJ_RAW_PARAM);
            HSD_LObjSetAttn(lobj, ldesc->u.attn->a0, ldesc->u.attn->a1,
                            ldesc->u.attn->a2, ldesc->u.attn->k0,
                            ldesc->u.attn->k1, ldesc->u.attn->k2);
        } else {
            HSD_LObjSetDistAttn(lobj, ldesc->u.spot->ref_dist,
                                ldesc->u.spot->ref_br,
                                ldesc->u.spot->dist_func);
            HSD_LObjSetSpot(lobj, ldesc->u.spot->cutoff,
                            ldesc->u.spot->spot_func);
        }
        break;
    default:
        OSReport("unexpected lightdesc flags (%x)\n", ldesc->flags);
        HSD_Panic(__FILE__, 1625, "");
        break;
    }
    return 0;
}

HSD_LObj* HSD_LObjLoadDesc(HSD_LightDesc* ldesc)
{
    HSD_LObj *top, **p = &top;

    for (; ldesc; ldesc = ldesc->next) {
        HSD_ClassInfo* info;

        if (!ldesc->class_name ||
            !(info = fn_80193748(ldesc->class_name)))
        {
            *p = HSD_LObjAlloc();
        } else {
            *p = fn_80193828(info);
            HSD_ASSERT(1659, *p);
        }
        HSD_LOBJ_METHOD(*p)->load(*p, ldesc);
        p = &(*p)->next;
    }
    *p = NULL;

    return top;
}

void HSD_LObjAddAnim(HSD_LObj* lobj, HSD_LightAnim* lanim)
{
    if (lobj == NULL) {
        return;
    }

    if (lanim != NULL) {
        if (lobj->aobj) {
            HSD_AObjRemove(lobj->aobj);
        }
        lobj->aobj = HSD_AObjLoadDesc(lanim->aobjdesc);
        HSD_WObjAddAnim(HSD_LObjGetPositionWObj(lobj), lanim->position_anim);
        HSD_WObjAddAnim(HSD_LObjGetInterestWObj(lobj), lanim->interest_anim);
    }
}

void HSD_LObjAddAnimAll(HSD_LObj* lobj, HSD_LightAnim* lanim)
{
    HSD_LObj* lp;
    HSD_LightAnim* la;

    if (lobj == NULL) {
        return;
    }

    for (lp = lobj, la = lanim; lp; lp = next_p(lp), la = next_p(la)) {
        HSD_LObjAddAnim(lp, la);
    }
}

static void LObjRelease(HSD_Class* o)
{
    HSD_LObj* lobj = HSD_LOBJ(o);

    HSD_AObjRemove(lobj->aobj);
    HSD_WObjUnref(HSD_LObjGetPositionWObj(lobj));
    HSD_WObjUnref(HSD_LObjGetInterestWObj(lobj));

    HSD_OBJECT_PARENT_INFO(&hsdLObj)->release(o);
}

static void LObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdLObj)) {
        current_lights = NULL;
    }
    HSD_OBJECT_PARENT_INFO(&hsdLObj)->amnesia(info);
}

static void LObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdLObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_lobj",
                     sizeof(HSD_LObjInfo), sizeof(HSD_LObj));
    HSD_CLASS_INFO(&hsdLObj)->release = LObjRelease;
    HSD_CLASS_INFO(&hsdLObj)->amnesia = LObjAmnesia;
    HSD_LOBJ_INFO(&hsdLObj)->load = LObjLoad;
    HSD_LOBJ_INFO(&hsdLObj)->update = (void*) LObjUpdateFunc;
}
