/**
 * @file cobj.c
 * @brief HAL sysdolphin cobj.c: camera objects, 0x80193C24-0x80196CE0.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/cobj.c) and checked against Colosseum's retail
 * code, the newer sysdolphin. The library is built with deferred inlining,
 * so functions are listed in HAL's order and MWCC emits them in reverse
 * (the retail address order). Functions nothing in the game references are
 * compiled and dead-stripped by the linker as in retail.
 *
 * Colosseum's cobj routes every viewport through a replaceable callback
 * (lbl_80478C58, set by fn_80196C3C, default fn_80196C54), and its eye
 * vector / up vector helpers report success as TRUE instead of Melee's
 * 0 / -1.
 *
 * Globals other objects already link against keep their dtk names; the
 * comments give the HAL names.
 */
#include "crt/math_ppc.h"
#include "dolphin/gx/GX.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_wobj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/object.h"

typedef f32 Mtx44[4][4];

typedef enum _HSD_RenderPass {
    HSD_RP_SCREEN,
    HSD_RP_TOPHALF,
    HSD_RP_BOTTOMHALF,
    HSD_RP_OFFSCREEN,
} HSD_RenderPass;

typedef enum _GXProjectionType {
    GX_PERSPECTIVE,
    GX_ORTHOGRAPHIC,
} GXProjectionType;

typedef void (*HSD_CObjViewportFunc)(BOOL jitter, f32 left, f32 top,
                                     f32 width, f32 height, f32 nearz,
                                     f32 farz);

void OSReport(const char* msg, ...);
void PSMTXCopy(const Mtx src, Mtx dst);
u32 PSMTXInverse(const Mtx src, Mtx inv);
void PSMTXMultVecSR(const Mtx m, const Vec3* src, Vec3* dst);
void PSMTXRotAxisRad(Mtx m, const Vec3* axis, f32 rad);
void PSVECNormalize(const Vec3* src, Vec3* unit);
f32 PSVECMag(const Vec3* v);
f32 PSVECDotProduct(const Vec3* a, const Vec3* b);
void PSVECSubtract(const Vec3* a, const Vec3* b, Vec3* ab);
void C_MTXLookAt(Mtx m, const Vec3* camPos, const Vec3* camUp,
                 const Vec3* target);
void C_MTXFrustum(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void C_MTXPerspective(Mtx44 m, f32 fovY, f32 aspect, f32 n, f32 f);
void C_MTXOrtho(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);

/* GXSetViewportJitter */
void GXSetViewportJitter(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz,
                 u32 field);
/* GXSetViewport */
void GXSetViewport(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz);
/* GXSetScissor */
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
/* GXSetProjection */
void GXSetProjection(Mtx44 mtx, GXProjectionType type);
/* VIGetNextField */
u32 VIGetNextField(void);

HSD_RenderPass HSD_GetCurrentRenderPass(void);
/* _HSD_ZListClear */
void _HSD_ZListClear(void);
/* _HSD_ZListSort */
void _HSD_ZListSort(void);
/* _HSD_ZListDisp */
void _HSD_ZListDisp(void);

/* hsdSearchClassInfo */
HSD_ClassInfo* fn_80193748(const char* class_name);
/* hsdNew */
void* fn_80193828(HSD_ClassInfo* info);

f32* HSD_MtxAlloc(void);
void HSD_MtxFree(f32* mtx);

/* MSL __float_min: FLT_MIN, addressed absolutely as an incomplete array. */
extern const f32 lbl_80478AC8[];
#define FLT_MIN (*lbl_80478AC8)

/* HSD_VIData (video.c); its current render mode sits at offset 0. */
extern GXRenderModeObj lbl_80466BC0;

static inline GXRenderModeObj* HSD_VIGetRenderMode(void)
{
    return &lbl_80466BC0;
}

/*
 * util.h. MWCC emits the out-of-line copy at 0x80194C2C for the call
 * SetRoll makes past its inline depth; the !src/!dst form is the one MWCC
 * folds when the argument is a local's address, as retail does.
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

/*
 * util.h (Melee's atan2f_check): upvec2roll loads -v.x and v.y once for
 * both the zero test and atan2, and the quarter turns are float constants.
 */
static inline f32 atan2f_check(f32 y, f32 x)
{
    if (x == 0.0F) {
        return y >= 0.0F ? 1.5707963267948966F : -1.5707963267948966F;
    }
    return atan2f(y, x);
}

/* current (.sbss is laid out in reverse declaration order) */
static HSD_CObj* lbl_8047B234;
/* default_class */
static HSD_ClassInfo* lbl_8047B230;

#define DegToRad(a) ((a) * 0.01745329252F)

static int CObjInit(HSD_Class* o);
static int CObjLoad(HSD_CObj* cobj, HSD_CObjDesc* cobjdesc);
static void CObjInfoInit(void);
static void CObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val);
void CObjRelease(HSD_Class* o);
static void CObjAmnesia(HSD_ClassInfo* info);
void HSD_CObjSetRoll(HSD_CObj* cobj, f32 roll);
BOOL HSD_CObjGetUpVector(HSD_CObj* cobj, Vec3* up);
void HSD_CObjSetUpVector(HSD_CObj* cobj, Vec3* up);
f32* HSD_CObjGetViewingMtxPtr(HSD_CObj* cobj);

/* hsdCObj */
HSD_CObjInfo lbl_8036C678 = { CObjInfoInit };

/* The screen size the viewports are laid out for, and the viewport hook. */
s32 lbl_80478C50 = 640;
s32 lbl_80478C54 = 480;

void fn_80196C54(BOOL jitter, f32 left, f32 top, f32 width, f32 height,
                 f32 nearz, f32 farz)
{
    if (jitter) {
        GXSetViewportJitter(left, top, width, height, nearz, farz, VIGetNextField());
    } else {
        GXSetViewport(left, top, width, height, nearz, farz);
    }
}

HSD_CObjViewportFunc lbl_80478C58 = fn_80196C54;

void fn_80196C3C(HSD_CObjViewportFunc func)
{
    if (func == NULL) {
        func = fn_80196C54;
    }
    lbl_80478C58 = func;
}

void HSD_CObjRemoveAnimByFlags(HSD_CObj* cobj, u32 flags)
{
    HSD_WObj* wobj;

    if (cobj == NULL) {
        return;
    }

    HSD_AObjRemove(cobj->aobj);
    cobj->aobj = NULL;
    wobj = HSD_CObjGetEyePositionWObj(cobj);
    HSD_WObjRemoveAnim(wobj);
    wobj = HSD_CObjGetInterestWObj(cobj);
    HSD_WObjRemoveAnim(wobj);
}

void HSD_CObjRemoveAnim(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return;
    }

    HSD_CObjRemoveAnimByFlags(cobj, 0x7FF);
}

void HSD_CObjAddAnim(HSD_CObj* cobj, HSD_CameraAnim* canim)
{
    if (cobj == NULL) {
        return;
    }

    if (canim == NULL) {
        return;
    }

    if (cobj->aobj != NULL) {
        HSD_AObjRemove(cobj->aobj);
    }
    cobj->aobj = HSD_AObjLoadDesc(canim->aobjdesc);
    HSD_WObjAddAnim(HSD_CObjGetEyePositionWObj(cobj), canim->eye_anim);
    HSD_WObjAddAnim(HSD_CObjGetInterestWObj(cobj), canim->interest_anim);
}

static void CObjUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_CObj* cobj = obj;
    Vec3 vec;

    if (cobj == NULL) {
        return;
    }

    switch (type) {
    case 1:
        HSD_CObjGetEyePosition(cobj, &vec);
        vec.x = val->fv;
        HSD_CObjSetEyePosition(cobj, &vec);
        break;
    case 2:
        HSD_CObjGetEyePosition(cobj, &vec);
        vec.y = val->fv;
        HSD_CObjSetEyePosition(cobj, &vec);
        break;
    case 3:
        HSD_CObjGetEyePosition(cobj, &vec);
        vec.z = val->fv;
        HSD_CObjSetEyePosition(cobj, &vec);
        break;
    case 5:
        HSD_CObjGetInterest(cobj, &vec);
        vec.x = val->fv;
        HSD_CObjSetInterest(cobj, &vec);
        break;
    case 6:
        HSD_CObjGetInterest(cobj, &vec);
        vec.x = val->fv;
        HSD_CObjSetInterest(cobj, &vec);
        break;
    case 7:
        HSD_CObjGetInterest(cobj, &vec);
        vec.x = val->fv;
        HSD_CObjSetInterest(cobj, &vec);
        break;
    case 9:
        HSD_CObjSetRoll(cobj, val->fv);
        break;
    case 10:
        HSD_CObjSetFov(cobj, val->fv);
        break;
    case 11:
        HSD_CObjSetNear(cobj, val->fv);
        break;
    case 12:
        HSD_CObjSetFar(cobj, val->fv);
        break;
    }
}

void HSD_CObjAnim(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return;
    }

    HSD_AObjInterpretAnim(cobj->aobj, cobj,
                          (HSD_ObjUpdateFunc) HSD_COBJ_METHOD(cobj)->update);
    HSD_WObjInterpretAnim(cobj->eyepos);
    HSD_WObjInterpretAnim(cobj->interest);
}

void HSD_CObjReqAnim(HSD_CObj* cobj, f32 startframe)
{
    if (cobj == NULL) {
        return;
    }

    if (cobj == NULL) {
        return;
    }

    HSD_AObjReqAnim(cobj->aobj, startframe);
    HSD_WObjReqAnim(cobj->eyepos, startframe);
    HSD_WObjReqAnim(cobj->interest, startframe);
}

GXProjectionType makeProjectionMtx(HSD_CObj* cobj, Mtx44 mtx)
{
    GXProjectionType projection_type;

    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        projection_type = GX_PERSPECTIVE;
        C_MTXPerspective(mtx, cobj->projection_param.perspective.fov,
                         cobj->projection_param.perspective.aspect, cobj->near,
                         cobj->far);
        break;
    case PROJ_FRUSTUM:
        projection_type = GX_PERSPECTIVE;
        C_MTXFrustum(mtx, cobj->projection_param.frustum.top,
                     cobj->projection_param.frustum.bottom,
                     cobj->projection_param.frustum.left,
                     cobj->projection_param.frustum.right, cobj->near,
                     cobj->far);
        break;
    case PROJ_ORTHO:
        projection_type = GX_ORTHOGRAPHIC;
        C_MTXOrtho(mtx, cobj->projection_param.ortho.top,
                   cobj->projection_param.ortho.bottom,
                   cobj->projection_param.ortho.left,
                   cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;
    }
    return projection_type;
}

static BOOL setupOffscreenCamera(HSD_CObj* cobj)
{
    Mtx44 mtx;

    lbl_80478C58(FALSE, cobj->viewport.xmin, cobj->viewport.ymin,
                 cobj->viewport.xmax - cobj->viewport.xmin,
                 cobj->viewport.ymax - cobj->viewport.ymin, 0.0F, 1.0F);
    GXSetScissor(cobj->scissor.left, cobj->scissor.top,
                cobj->scissor.right - cobj->scissor.left,
                cobj->scissor.bottom - cobj->scissor.top);
    GXSetProjection(mtx, makeProjectionMtx(cobj, mtx));
    return TRUE;
}

static BOOL setupNormalCamera(HSD_CObj* cobj)
{
    GXProjectionType projection_type;
    Mtx44 p;

    f64 x_scale;
    f64 y_scale;

    f32 top;
    f32 bottom;
    f32 left;
    f32 right;

    f32 width;
    f32 height;

    GXRenderModeObj* rmode = HSD_VIGetRenderMode();

    x_scale = (f64) rmode->fbWidth / (f64) lbl_80478C50;
    y_scale = (f64) rmode->efbHeight / (f64) lbl_80478C54;

    left = cobj->viewport.xmin * x_scale;
    right = cobj->viewport.xmax * x_scale;
    top = cobj->viewport.ymin * y_scale;
    bottom = cobj->viewport.ymax * y_scale;

    width = right - left;
    height = bottom - top;

    if (rmode->field_rendering) {
        lbl_80478C58(TRUE, left, top, width, height, 0.0F, 1.0F);
    } else {
        lbl_80478C58(FALSE, left, top, width, height, 0.0F, 1.0F);
    }

    left = cobj->scissor.left * x_scale;
    right = cobj->scissor.right * x_scale;
    top = cobj->scissor.top * y_scale;
    bottom = cobj->scissor.bottom * y_scale;
    width = right - left;
    height = bottom - top;
    GXSetScissor(left, top, width, height);

    projection_type = makeProjectionMtx(cobj, p);
    GXSetProjection(p, projection_type);

    return TRUE;
}

static BOOL setupTopHalfCamera(HSD_CObj* cobj)
{
    GXProjectionType projection_type;
    Mtx44 p;

    f32 t;
    f32 b;
    f32 w;

    f32 top;
    f32 bottom;
    f32 left;
    f32 right;

    f32 width;
    f32 height;
    f32 h_scale;

    GXRenderModeObj* rmode = HSD_VIGetRenderMode();

    if (cobj->viewport.ymin >= rmode->efbHeight) {
        return FALSE;
    }
    left = cobj->viewport.xmin;
    right = cobj->viewport.xmax;
    top = cobj->viewport.ymin;
    bottom = cobj->viewport.ymax < rmode->efbHeight ? cobj->viewport.ymax
                                                     : rmode->efbHeight;
    height = bottom - top;
    width = right - left;
    GXSetScissor(left, top, width, height);

    left = cobj->viewport.xmin;
    right = cobj->viewport.xmax;
    top = cobj->viewport.ymin;
    bottom = cobj->viewport.ymax < rmode->efbHeight ? cobj->viewport.ymax
                                                     : rmode->efbHeight;
    height = bottom - top;
    h_scale = height / (cobj->viewport.ymax - cobj->viewport.ymin);
    width = right - left;

    lbl_80478C58(FALSE, left, top, width, height, 0.0F, 1.0F);

    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        projection_type = GX_PERSPECTIVE;
        t = cobj->near *
            tanf(DegToRad(0.5 * cobj->projection_param.perspective.fov));
        w = t * cobj->projection_param.perspective.aspect;
        b = t * -(2.0F * h_scale - 1.0F);
        C_MTXFrustum(p, t, b, -w, w, cobj->near, cobj->far);
        break;
    case PROJ_FRUSTUM:
        projection_type = GX_PERSPECTIVE;
        t = cobj->projection_param.frustum.top;
        C_MTXFrustum(p, t, -(h_scale * (t - cobj->projection_param.frustum.bottom) - t),
                     cobj->projection_param.frustum.left,
                     cobj->projection_param.frustum.right, cobj->near,
                     cobj->far);
        break;
    case PROJ_ORTHO:
        projection_type = GX_ORTHOGRAPHIC;
        t = cobj->projection_param.ortho.top;
        C_MTXOrtho(p, t, -(h_scale * (t - cobj->projection_param.ortho.bottom) - t),
                   cobj->projection_param.ortho.left,
                   cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;
    }

    GXSetProjection(p, projection_type);

    return TRUE;
}

static BOOL setupBottomHalfCamera(HSD_CObj* cobj)
{
    GXProjectionType projection_type;
    Mtx44 p;

    f32 t, b, w;
    f32 top, bottom;
    f32 left, right;
    f32 width, height;
    f32 hscale;
    u32 screen_top;

    GXRenderModeObj* rmode = HSD_VIGetRenderMode();

    screen_top = rmode->efbHeight - 8;

    if (cobj->viewport.ymax < screen_top) {
        return FALSE;
    }

    left = cobj->scissor.left;
    right = cobj->scissor.right;
    top = (cobj->scissor.top > screen_top ? cobj->scissor.top : screen_top) -
          screen_top;
    bottom = cobj->scissor.bottom - screen_top;
    width = right - left;
    height = bottom - top;
    GXSetScissor(left, top, width, height);

    left = cobj->viewport.xmin;
    right = cobj->viewport.xmax;
    top = (cobj->viewport.ymin > screen_top ? cobj->viewport.ymin
                                             : screen_top) -
          screen_top;
    width = right - left;
    bottom = cobj->viewport.ymax - screen_top;
    height = bottom - top;
    hscale = height / (cobj->viewport.ymax - cobj->viewport.ymin);

    lbl_80478C58(FALSE, left, top, width, height, 0.0F, 1.0F);

    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        projection_type = GX_PERSPECTIVE;
        b = cobj->near *
            tanf(DegToRad(0.5 * cobj->projection_param.perspective.fov));
        w = b * cobj->projection_param.perspective.aspect;
        t = b * (2.0F * hscale + -1.0F);
        C_MTXFrustum(p, t, -b, -w, w, cobj->near, cobj->far);
        break;
    case PROJ_FRUSTUM:
        projection_type = GX_PERSPECTIVE;
        b = cobj->projection_param.frustum.bottom;
        C_MTXFrustum(p, hscale * (cobj->projection_param.frustum.top - b) + b,
                     b, cobj->projection_param.frustum.left,
                     cobj->projection_param.frustum.right, cobj->near,
                     cobj->far);
        break;
    case PROJ_ORTHO:
        projection_type = GX_ORTHOGRAPHIC;
        b = cobj->projection_param.ortho.bottom;
        C_MTXOrtho(p, hscale * (cobj->projection_param.ortho.top - b) + b, b,
                   cobj->projection_param.ortho.left,
                   cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;
    }

    GXSetProjection(p, projection_type);

    return TRUE;
}

/*
 * The eye and interest tests are inlines of their own: retail materialises
 * each result before the || and re-reads the WObj pointer for the flag test
 * (0x80195F40-0x80195FA4).
 */
static inline BOOL CObjEyePositionIsDirty(HSD_CObj* cobj)
{
    return cobj->eyepos != NULL && (cobj->eyepos->flags & 2);
}

static inline BOOL CObjInterestIsDirty(HSD_CObj* cobj)
{
    return cobj->interest != NULL && (cobj->interest->flags & 2);
}

static inline BOOL HSD_CObjMtxIsDirty(HSD_CObj* cobj)
{
    return (cobj->flags & (1 << 30)) || CObjEyePositionIsDirty(cobj) ||
           CObjInterestIsDirty(cobj);
}

void HSD_CObjSetupViewingMtx(HSD_CObj* cobj)
{
    Vec3 eyepos;
    Vec3 up_vec;
    Vec3 interest;

    if (!(cobj->flags & 2) && HSD_CObjMtxIsDirty(cobj)) {
        HSD_CObjGetEyePosition(cobj, &eyepos);
        if (!HSD_CObjGetUpVector(cobj, &up_vec)) {
            up_vec.x = 0.0F;
            up_vec.y = 1.0F;
            up_vec.z = 0.0F;
        }
        HSD_CObjGetInterest(cobj, &interest);
        C_MTXLookAt(cobj->view_mtx, &eyepos, &up_vec, &interest);
        HSD_WObjClearFlags(cobj->eyepos, 2);
        HSD_WObjClearFlags(cobj->interest, 2);
        HSD_CObjClearFlags(cobj, 0x40000000);
        HSD_CObjSetFlags(cobj, 0x80000000);
    }
}

BOOL HSD_CObjSetCurrent(HSD_CObj* cobj)
{
    HSD_RenderPass render_pass;
    BOOL result;

    if (cobj == NULL) {
        return FALSE;
    }
    render_pass = HSD_GetCurrentRenderPass();
    _HSD_ZListClear();
    lbl_8047B234 = cobj;
    switch (render_pass) {
    case HSD_RP_OFFSCREEN:
        result = setupOffscreenCamera(cobj);
        break;
    case HSD_RP_SCREEN:
        result = setupNormalCamera(cobj);
        break;
    case HSD_RP_TOPHALF:
        result = setupTopHalfCamera(cobj);
        break;
    case HSD_RP_BOTTOMHALF:
        result = setupBottomHalfCamera(cobj);
        break;
    default:
        HSD_Panic(__FILE__, 683, "unkown type of render pass.\n");
        return FALSE;
    }
    if (!result) {
        return FALSE;
    } else {
        HSD_CObjSetupViewingMtx(cobj);
        return TRUE;
    }
}

/* HSD_CObjEndCurrent */
void HSD_CObjEndCurrent(void)
{
    _HSD_ZListSort();
    _HSD_ZListDisp();
}

HSD_WObj* HSD_CObjGetInterestWObj(HSD_CObj* cobj)
{
    HSD_ASSERT(720, cobj);
    return cobj->interest;
}

HSD_WObj* HSD_CObjGetEyePositionWObj(HSD_CObj* cobj)
{
    HSD_ASSERT(744, cobj);
    return cobj->eyepos;
}

void HSD_CObjGetInterest(HSD_CObj* cobj, Vec3* interest)
{
    HSD_ASSERT(768, cobj);
    HSD_WObjGetPosition(HSD_CObjGetInterestWObj(cobj), interest);
}

void HSD_CObjSetInterest(HSD_CObj* cobj, Vec3* interest)
{
    HSD_ASSERT(780, cobj);
    HSD_WObjSetPosition(HSD_CObjGetInterestWObj(cobj), interest);
}

void HSD_CObjGetEyePosition(HSD_CObj* cobj, Vec3* position)
{
    HSD_ASSERT(792, cobj);
    HSD_WObjGetPosition(HSD_CObjGetEyePositionWObj(cobj), position);
}

void HSD_CObjSetEyePosition(HSD_CObj* cobj, Vec3* position)
{
    HSD_ASSERT(804, cobj);
    HSD_WObjSetPosition(HSD_CObjGetEyePositionWObj(cobj), position);
}

BOOL HSD_CObjGetEyeVector(HSD_CObj* cobj, Vec3* eye)
{
    Vec3 eyepos;
    Vec3 interest;

    if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL) {
        return FALSE;
    }
    HSD_CObjGetEyePosition(cobj, &eyepos);
    HSD_CObjGetInterest(cobj, &interest);
    PSVECSubtract(&interest, &eyepos, eye);
    return vec_normalize_check(eye, eye) == 0;
}

f32 HSD_CObjGetEyeDistance(HSD_CObj* cobj)
{
    Vec3 position;
    Vec3 interest;
    Vec3 look_vector;

    if (cobj == NULL) {
        return 0.0F;
    }
    HSD_ASSERT(851, cobj->eyepos);
    HSD_ASSERT(852, cobj->interest);
    HSD_CObjGetEyePosition(cobj, &position);
    HSD_CObjGetInterest(cobj, &interest);
    PSVECSubtract(&interest, &position, &look_vector);
    return PSVECMag(&look_vector);
}

/* orig */
static Vec3 lbl_8036C6BC = { 0.0F, 0.0F, 0.0F };
/* uy */
static Vec3 lbl_8036C6C8 = { 0.0F, 1.0F, 0.0F };

static f32 upvec2roll(HSD_CObj* cobj, Vec3* up)
{
    Vec3 v;
    Vec3 eye;
    Mtx vmtx;
    f32 dot;

    if (!HSD_CObjGetEyeVector(cobj, &eye)) {
        return 0.0F;
    }
    dot = 1.0F - fabsf(PSVECDotProduct(up, &eye));
    if (dot < FLT_MIN) {
        return 0.0F;
    }
    C_MTXLookAt(vmtx, &lbl_8036C6BC, &lbl_8036C6C8, &eye);
    PSMTXMultVecSR(vmtx, up, &v);
    return atan2f_check(-v.x, v.y);
}

static BOOL roll2upvec(HSD_CObj* cobj, Vec3* up, f32 roll)
{
    Vec3 eye;
    Vec3 v0;
    Vec3 v1;
    Mtx m;

    if (!HSD_CObjGetEyeVector(cobj, &eye)) {
        return FALSE;
    }
    if (1.0 - fabsf(eye.y) < 0.0001) {
        v0.x = sqrtf(eye.y * eye.y + eye.z * eye.z);
        v0.y = eye.y * (-eye.x / v0.x);
        v0.z = eye.z * (-eye.x / v0.x);
    } else {
        v0.y = sqrtf(eye.x * eye.x + eye.z * eye.z);
        v0.x = eye.x * (-eye.y / v0.y);
        v0.z = eye.z * (-eye.y / v0.y);
    }
    PSMTXRotAxisRad(m, &eye, -roll);
    PSMTXMultVecSR(m, &v0, &v1);
    PSVECNormalize(&v1, up);
    return TRUE;
}

BOOL HSD_CObjGetUpVector(HSD_CObj* cobj, Vec3* up)
{
    if (cobj == NULL || up == NULL) {
        return FALSE;
    }
    if (cobj->flags & 1) {
        *up = cobj->u.up;
        return TRUE;
    }
    return roll2upvec(cobj, up, cobj->u.roll);
}

void HSD_CObjSetUpVector(HSD_CObj* cobj, Vec3* up)
{
    Vec3 v;

    if (!cobj || !up) {
        return;
    }
    if (cobj->flags & 1) {
        if (vec_normalize_check(up, &v)) {
            OSReport("illegal up vector.");
            HSD_ASSERT(996, 0);
        }
        if (cobj->u.up.x != v.x || cobj->u.up.y != v.y ||
            cobj->u.up.z != v.z)
        {
            HSD_CObjSetMtxDirty(cobj);
            cobj->u.up = v;
        }
    } else {
        HSD_CObjSetRoll(cobj, upvec2roll(cobj, up));
    }
}

void HSD_CObjSetMtxDirty(HSD_CObj* cobj)
{
    cobj->flags |= (1 << 30) | (1 << 31);
}

void HSD_CObjGetViewingMtx(HSD_CObj* cobj, Mtx mtx)
{
    PSMTXCopy((MtxPtr) HSD_CObjGetViewingMtxPtr(cobj), mtx);
}

f32* HSD_CObjGetInvViewingMtxPtrDirect(HSD_CObj* cobj)
{
    if (cobj->flags & (1 << 31)) {
        if (cobj->proj_mtx == NULL) {
            cobj->proj_mtx = HSD_MtxAlloc();
        }
        PSMTXInverse(cobj->view_mtx, (MtxPtr) cobj->proj_mtx);
        HSD_CObjClearFlags(cobj, (1 << 31));
    }
    return cobj->proj_mtx;
}

f32* HSD_CObjGetViewingMtxPtr(HSD_CObj* cobj)
{
    HSD_CObjSetupViewingMtx(cobj);
    return (f32*) cobj->view_mtx;
}

void HSD_CObjSetRoll(HSD_CObj* cobj, f32 roll)
{
    Vec3 up;

    if (cobj == NULL) {
        return;
    }

    if (cobj->flags & 1) {
        roll2upvec(cobj, &up, roll);
        HSD_CObjSetUpVector(cobj, &up);
    } else {
        if (cobj->u.roll != roll) {
            HSD_CObjSetMtxDirty(cobj);
        }
        cobj->u.roll = roll;
    }
}

f32 HSD_CObjGetFov(HSD_CObj* cobj)
{
    if (cobj == NULL || cobj->projection_type != 1) {
        return 0.0F;
    }
    return cobj->projection_param.perspective.fov;
}

void HSD_CObjSetFov(HSD_CObj* cobj, f32 fov)
{
    if (cobj == NULL || cobj->projection_type != 1) {
        return;
    }
    cobj->projection_param.perspective.fov = fov;
}

f32 HSD_CObjGetAspect(HSD_CObj* cobj)
{
    if (cobj == NULL || cobj->projection_type != 1) {
        return 0.0F;
    }
    return cobj->projection_param.perspective.aspect;
}

void HSD_CObjSetAspect(HSD_CObj* cobj, f32 aspect)
{
    if (cobj == NULL || cobj->projection_type != 1) {
        return;
    }
    cobj->projection_param.perspective.aspect = aspect;
}

f32 HSD_CObjGetTop(HSD_CObj* cobj)
{
    f32 result;

    if (cobj == NULL) {
        return 0.0F;
    }
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        result = cobj->near *
                 tanf(0.5F * DegToRad(cobj->projection_param.perspective.fov));
        break;
    case PROJ_FRUSTUM:
        result = cobj->projection_param.frustum.top;
        break;
    case PROJ_ORTHO:
        result = cobj->projection_param.ortho.top;
        break;
    default:
        result = 0.0F;
        break;
    }
    return result;
}

f32 HSD_CObjGetBottom(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0F;
    }
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        return -cobj->near *
               tanf(0.5F * DegToRad(cobj->projection_param.perspective.fov));
    case PROJ_FRUSTUM:
        return cobj->projection_param.frustum.bottom;
    case PROJ_ORTHO:
        return cobj->projection_param.ortho.bottom;
    default:
        return 0.0F;
    }
}

f32 HSD_CObjGetLeft(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0F;
    }
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        return cobj->projection_param.perspective.aspect *
               (-cobj->near *
                tanf(0.5F * DegToRad(cobj->projection_param.perspective.fov)));
    case PROJ_FRUSTUM:
        return cobj->projection_param.frustum.left;
    case PROJ_ORTHO:
        return cobj->projection_param.ortho.left;
    default:
        return 0.0F;
    }
}

f32 HSD_CObjGetRight(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0F;
    }
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        return cobj->projection_param.perspective.aspect *
               (cobj->near *
                tanf(0.5F * DegToRad(cobj->projection_param.perspective.fov)));
    case PROJ_FRUSTUM:
        return cobj->projection_param.frustum.right;
    case PROJ_ORTHO:
        return cobj->projection_param.ortho.right;
    default:
        return 0.0F;
    }
}

f32 HSD_CObjGetNear(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0F;
    }
    return cobj->near;
}

void HSD_CObjSetNear(HSD_CObj* cobj, f32 near)
{
    if (cobj != NULL) {
        cobj->near = near;
    }
}

f32 HSD_CObjGetFar(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return 0.0F;
    }
    return cobj->far;
}

void HSD_CObjSetFar(HSD_CObj* cobj, f32 far)
{
    if (cobj != NULL) {
        cobj->far = far;
    }
}

void HSD_CObjSetScissor(HSD_CObj* cobj, Scissor* scissor)
{
    if (cobj == NULL) {
        return;
    }
    cobj->scissor = *scissor;
}

void HSD_CObjSetScissorx4(HSD_CObj* cobj, u16 left, u16 right, u16 top,
                          u16 bottom)
{
    if (cobj == NULL) {
        return;
    }
    cobj->scissor.left = left;
    cobj->scissor.right = right;
    cobj->scissor.top = top;
    cobj->scissor.bottom = bottom;
}

void HSD_CObjSetViewport(HSD_CObj* cobj, HSD_RectS16* viewport)
{
    if (cobj == NULL) {
        return;
    }
    cobj->viewport.xmin = viewport->xmin;
    cobj->viewport.xmax = viewport->xmax;
    cobj->viewport.ymin = viewport->ymin;
    cobj->viewport.ymax = viewport->ymax;
}

void HSD_CObjSetViewportfx4(HSD_CObj* cobj, f32 left, f32 right, f32 top,
                            f32 bottom)
{
    if (cobj == NULL) {
        return;
    }
    cobj->viewport.xmin = left;
    cobj->viewport.xmax = right;
    cobj->viewport.ymin = top;
    cobj->viewport.ymax = bottom;
}

int HSD_CObjGetProjectionType(HSD_CObj* cobj)
{
    if (cobj == NULL) {
        return PROJ_PERSPECTIVE;
    }
    return cobj->projection_type;
}

void HSD_CObjSetProjectionType(HSD_CObj* cobj, u32 proj_type)
{
    if (cobj == NULL) {
        return;
    }
    cobj->projection_type = proj_type;
}

void HSD_CObjSetPerspective(HSD_CObj* cobj, f32 fov, f32 aspect)
{
    if (cobj == NULL) {
        return;
    }
    cobj->projection_type = PROJ_PERSPECTIVE;
    cobj->projection_param.perspective.fov = fov;
    cobj->projection_param.perspective.aspect = aspect;
}

void HSD_CObjSetFrustum(HSD_CObj* cobj, f32 top, f32 bottom, f32 left,
                        f32 right)
{
    if (cobj == NULL) {
        return;
    }
    cobj->projection_type = PROJ_FRUSTUM;
    cobj->projection_param.frustum.top = top;
    cobj->projection_param.frustum.bottom = bottom;
    cobj->projection_param.frustum.left = left;
    cobj->projection_param.frustum.right = right;
}

void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 top, f32 bottom, f32 left,
                      f32 right)
{
    if (cobj == NULL) {
        return;
    }
    cobj->projection_type = PROJ_ORTHO;
    cobj->projection_param.ortho.top = top;
    cobj->projection_param.ortho.bottom = bottom;
    cobj->projection_param.ortho.left = left;
    cobj->projection_param.ortho.right = right;
}

void HSD_CObjGetPerspective(HSD_CObj* cobj, f32* fov, f32* aspect)
{
    if (cobj == NULL || cobj->projection_type != PROJ_PERSPECTIVE) {
        return;
    }
    if (fov != NULL) {
        *fov = cobj->projection_param.perspective.fov;
    }
    if (aspect != NULL) {
        *aspect = cobj->projection_param.perspective.aspect;
    }
}

void HSD_CObjGetOrtho(HSD_CObj* cobj, f32* top, f32* bottom, f32* left,
                      f32* right)
{
    if (cobj == NULL || cobj->projection_type != PROJ_ORTHO) {
        return;
    }
    if (top != NULL) {
        *top = cobj->projection_param.ortho.top;
    }
    if (bottom != NULL) {
        *bottom = cobj->projection_param.ortho.bottom;
    }
    if (left != NULL) {
        *left = cobj->projection_param.ortho.left;
    }
    if (right != NULL) {
        *right = cobj->projection_param.ortho.right;
    }
}

void HSD_CObjSetFlags(HSD_CObj* cobj, u32 flags)
{
    if (cobj == NULL) {
        return;
    }
    cobj->flags |= flags;
}

void HSD_CObjClearFlags(HSD_CObj* cobj, u32 flags)
{
    if (cobj == NULL) {
        return;
    }
    cobj->flags &= ~flags;
}

HSD_CObj* HSD_CObjGetCurrent(void)
{
    return lbl_8047B234;
}

HSD_CObj* HSD_CObjAlloc(void)
{
    HSD_CObj* cobj = HSD_COBJ(
        fn_80193828(lbl_8047B230 ? lbl_8047B230 : HSD_CLASS_INFO(&lbl_8036C678)));
    HSD_ASSERT(1956, cobj);
    return cobj;
}

static inline void CObjResetFlags(HSD_CObj* cobj, u32 flags)
{
    if (cobj == NULL) {
        return;
    }
    cobj->flags = (cobj->flags & 0xC0000000) | (flags & ~0xC0000000);
}

static int CObjLoad(HSD_CObj* cobj, HSD_CObjDesc* desc)
{
    static Vec3 up = { 0.0F, 1.0F, 0.0F };

    cobj->flags = desc->common.flags;
    CObjResetFlags(cobj, desc->common.flags);
    HSD_CObjSetViewport(cobj, &desc->common.viewport);
    HSD_CObjSetScissor(cobj, &desc->common.scissor);
    HSD_WObjInit(cobj->eyepos, desc->common.eyepos);
    HSD_WObjInit(cobj->interest, desc->common.interest);
    HSD_CObjSetNear(cobj, desc->common.nnear);
    HSD_CObjSetFar(cobj, desc->common.ffar);
    if (desc->common.flags & 1) {
        if (desc->common.up_vector != NULL) {
            HSD_CObjSetUpVector(cobj, desc->common.up_vector);
        } else {
            HSD_CObjSetUpVector(cobj, &up);
        }
    } else {
        HSD_CObjSetRoll(cobj, desc->common.roll);
    }
    switch (desc->common.projection_type) {
    case PROJ_PERSPECTIVE:
        HSD_CObjSetPerspective(cobj, desc->perspective.fov,
                               desc->perspective.aspect);
        break;
    case PROJ_ORTHO:
        HSD_CObjSetOrtho(cobj, desc->ortho.top, desc->ortho.bottom,
                         desc->ortho.left, desc->ortho.right);
        break;
    case PROJ_FRUSTUM:
        HSD_CObjSetFrustum(cobj, desc->frustum.top, desc->frustum.bottom,
                           desc->frustum.left, desc->frustum.right);
        break;
    default:
        HSD_ASSERT(2002, 0);
        break;
    }
    return 0;
}

HSD_CObj* HSD_CObjLoadDesc(HSD_CObjDesc* desc)
{
    HSD_ClassInfo* info;
    HSD_CObj* cobj;

    if (desc != NULL) {
        if (desc->class_name == NULL ||
            (info = fn_80193748(desc->class_name)) == NULL)
        {
            cobj = HSD_CObjAlloc();
        } else {
            cobj = fn_80193828(info);
            HSD_ASSERT(2041, cobj);
        }
        HSD_COBJ_METHOD(cobj)->load(cobj, desc);
        return cobj;
    }
    return NULL;
}

static int CObjInit(HSD_Class* o)
{
    HSD_CObj* cobj;
    int status = HSD_OBJECT_PARENT_INFO(&lbl_8036C678)->init(o);
    if (status < 0) {
        return status;
    }
    cobj = HSD_COBJ(o);
    if (cobj != NULL) {
        HSD_CObjSetMtxDirty(cobj);
    }
    cobj->eyepos = HSD_WObjAlloc();
    cobj->interest = HSD_WObjAlloc();
    return 0;
}

void CObjRelease(HSD_Class* o)
{
    HSD_CObj* cobj = HSD_COBJ(o);

    HSD_AObjRemove(cobj->aobj);
    HSD_WObjUnref(HSD_CObjGetEyePositionWObj(cobj));
    HSD_WObjUnref(HSD_CObjGetInterestWObj(cobj));
    if (cobj->proj_mtx != NULL) {
        HSD_MtxFree(cobj->proj_mtx);
    }
    HSD_OBJECT_PARENT_INFO(&lbl_8036C678)->release(o);
}

static void CObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(lbl_8047B230)) {
        lbl_8047B230 = NULL;
    }
    if (info == HSD_CLASS_INFO(&lbl_8036C678)) {
        lbl_8047B234 = NULL;
    }
    HSD_OBJECT_PARENT_INFO(&lbl_8036C678)->amnesia(info);
}

/* CObjInfoInit */
static void CObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&lbl_8036C678), &hsdObj,
                     "sysdolphin_base_library", "hsd_cobj",
                     sizeof(HSD_CObjInfo), sizeof(HSD_CObj));

    HSD_CLASS_INFO(&lbl_8036C678)->init = CObjInit;
    HSD_CLASS_INFO(&lbl_8036C678)->release = CObjRelease;
    HSD_CLASS_INFO(&lbl_8036C678)->amnesia = CObjAmnesia;
    lbl_8036C678.load = CObjLoad;
    lbl_8036C678.update = (void (*)(HSD_CObj*, u32, void*)) CObjUpdateFunc;
}
