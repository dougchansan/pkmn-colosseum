/**
 * @file hsd_cobj.c
 * @brief HSD CObj - Camera object implementation.
 *
 * Colosseum address: 0x80193C24 (HSD_CObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/cobj.c
 */

#include "hsd/hsd_cobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_wobj.h"

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void C_MTXLookAt();
extern void C_MTXPerspective();
extern void OSFillFPUContext();
extern int fn_801963E0();   /* wrk7: was `void` asm-wrapper decl; typed-C returns int */
extern void fn_8019674C();
extern char lbl_80465080[];

static HSD_ClassInfo* default_class;
static HSD_CObj* current;

static void CObjInfoInit(void);

HSD_CObjInfo hsdCObj = { CObjInfoInit };

/* ========================================================================= */
/*  Accessors                                                                */
/* ========================================================================= */

HSD_CObj* HSD_CObjGetCurrent_Early(void)
{
    return current;
}

u32 HSD_CObjGetFlags(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->flags;
}

void HSD_CObjSetFlags(HSD_CObj* cobj, u32 flags)
{
    HSD_ASSERT(0, cobj);
    cobj->flags |= flags;
}

void HSD_CObjClearFlags(HSD_CObj* cobj, u32 flags)
{
    HSD_ASSERT(0, cobj);
    cobj->flags &= ~flags;
}

int HSD_CObjGetProjectionType_Early(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->projection_type;
}

void HSD_CObjSetProjectionType_Early(HSD_CObj* cobj, u32 type)
{
    HSD_ASSERT(0, cobj);
    cobj->projection_type = (u8) type;
}

f32 HSD_CObjGetFov(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->projection_param.perspective.fov;
}

void HSD_CObjSetFov_Early(HSD_CObj* cobj, f32 fov)
{
    HSD_ASSERT(0, cobj);
    cobj->projection_param.perspective.fov = fov;
}

f32 HSD_CObjGetAspect(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->projection_param.perspective.aspect;
}

void HSD_CObjSetAspect_Early(HSD_CObj* cobj, f32 aspect)
{
    HSD_ASSERT(0, cobj);
    cobj->projection_param.perspective.aspect = aspect;
}

f32 HSD_CObjGetNear_Early(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->near;
}

void HSD_CObjSetNear_Early(HSD_CObj* cobj, f32 near)
{
    HSD_ASSERT(0, cobj);
    cobj->near = near;
}

f32 HSD_CObjGetFar_Early(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->far;
}

void HSD_CObjSetFar_Early(HSD_CObj* cobj, f32 far)
{
    HSD_ASSERT(0, cobj);
    cobj->far = far;
}

/* ========================================================================= */
/*  WObj accessors                                                           */
/* ========================================================================= */

HSD_WObj* HSD_CObjGetEyePositionWObj(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->eyepos;
}

void HSD_CObjSetEyePositionWObj(HSD_CObj* cobj, HSD_WObj* eyepos)
{
    HSD_ASSERT(0, cobj);
    if (cobj->eyepos == eyepos) {
        return;
    }
    ref_INC(eyepos);
    HSD_WObjUnref(cobj->eyepos);
    cobj->eyepos = eyepos;
}

HSD_WObj* HSD_CObjGetInterestWObj(HSD_CObj* cobj)
{
    HSD_ASSERT(0, cobj);
    return cobj->interest;
}

void HSD_CObjSetInterestWObj(HSD_CObj* cobj, HSD_WObj* interest)
{
    HSD_ASSERT(0, cobj);
    if (cobj->interest == interest) {
        return;
    }
    ref_INC(interest);
    HSD_WObjUnref(cobj->interest);
    cobj->interest = interest;
}

/* ========================================================================= */
/*  Perspective / frustum / ortho setup                                      */
/* ========================================================================= */

void HSD_CObjSetPerspective_Early(HSD_CObj* cobj, f32 fov, f32 aspect)
{
    HSD_ASSERT(0, cobj);
    cobj->projection_type = PROJ_PERSPECTIVE;
    cobj->projection_param.perspective.fov = fov;
    cobj->projection_param.perspective.aspect = aspect;
}

void HSD_CObjSetFrustum_Early(HSD_CObj* cobj, f32 top, f32 bottom,
                              f32 left, f32 right)
{
    HSD_ASSERT(0, cobj);
    cobj->projection_type = PROJ_FRUSTUM;
    cobj->projection_param.frustum.top = top;
    cobj->projection_param.frustum.bottom = bottom;
    cobj->projection_param.frustum.left = left;
    cobj->projection_param.frustum.right = right;
}

void HSD_CObjSetOrtho_Early(HSD_CObj* cobj, f32 top, f32 bottom,
                            f32 left, f32 right)
{
    HSD_ASSERT(0, cobj);
    cobj->projection_type = PROJ_ORTHO;
    cobj->projection_param.ortho.top = top;
    cobj->projection_param.ortho.bottom = bottom;
    cobj->projection_param.ortho.left = left;
    cobj->projection_param.ortho.right = right;
}

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_CObjRemoveAnim_Early(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        HSD_AObjRemove(cobj->aobj);
        cobj->aobj = NULL;
        HSD_WObjRemoveAnim(cobj->eyepos);
        HSD_WObjRemoveAnim(cobj->interest);
    }
}

void HSD_CObjAddAnim_Early(HSD_CObj* cobj, HSD_CameraAnim* canim)
{
    if (cobj == NULL || canim == NULL) {
        return;
    }
    if (cobj->aobj != NULL) {
        HSD_AObjRemove(cobj->aobj);
    }
    cobj->aobj = HSD_AObjLoadDesc(canim->aobjdesc);
    HSD_WObjAddAnim(cobj->eyepos, canim->eye_anim);
    HSD_WObjAddAnim(cobj->interest, canim->interest_anim);
}

void HSD_CObjReqAnim_Early(HSD_CObj* cobj, f32 startframe)
{
    if (cobj != NULL) {
        HSD_AObjReqAnim(cobj->aobj, startframe);
        HSD_WObjReqAnim(cobj->eyepos, startframe);
        HSD_WObjReqAnim(cobj->interest, startframe);
    }
}

void HSD_CObjAnim_Early(HSD_CObj* cobj)
{
    if (cobj != NULL) {
        HSD_WObjInterpretAnim(cobj->eyepos);
        HSD_WObjInterpretAnim(cobj->interest);
    }
}

/* ========================================================================= */
/*  Alloc                                                                    */
/* ========================================================================= */

HSD_CObj* HSD_CObjAlloc(void)
{
    HSD_CObj* cobj;
    cobj = (HSD_CObj*) hsdNew(
        default_class ? default_class : &hsdCObj.parent.parent);
    HSD_ASSERT(0, cobj);
    return cobj;
}

void HSD_CObjSetDefaultClass(HSD_ClassInfo* info)
{
    if (info) {
        HSD_ASSERT(0, hsdIsDescendantOf(info, &hsdCObj));
    }
    default_class = info;
}

/* ========================================================================= */
/*  Class lifecycle                                                          */
/* ========================================================================= */

static void CObjRelease(HSD_Class* o)
{
    HSD_CObj* cobj = (HSD_CObj*) o;
    HSD_WObjUnref(cobj->eyepos);
    HSD_WObjUnref(cobj->interest);
    HSD_AObjRemove(cobj->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdCObj)->release(o);
}

static void CObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    current = NULL;
    HSD_OBJECT_PARENT_INFO(&hsdCObj)->amnesia(info);
}

static void CObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdCObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_cobj",
                     sizeof(HSD_CObjInfo), sizeof(HSD_CObj));
    HSD_CLASS_INFO(&hsdCObj)->release = CObjRelease;
    HSD_CLASS_INFO(&hsdCObj)->amnesia = CObjAmnesia;
    HSD_COBJ_INFO(&hsdCObj)->load =
        (int (*)(HSD_CObj*, HSD_CObjDesc*)) fn_801963E0;
    HSD_COBJ_INFO(&hsdCObj)->update =
        (void (*)(HSD_CObj*, u32, void*)) fn_8019674C;
}

/* 0x80193CD0 | 0x60 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80193CD0(void) {
#include "src/hsd/hsd_cobj_fn_80193CD0.inc"
}
#else
void fn_80193CD0(u8* ptr) {
    extern u32 lbl_8047B230;
    extern u32 lbl_8047B234;
    extern HSD_CObjInfo lbl_8036C678;
    u32 class_info;
    if (ptr == (u8*)lbl_8047B230) { lbl_8047B230 = 0; }
    if (ptr == (u8*)&lbl_8036C678) { lbl_8047B234 = 0; }
    class_info = *(u32*)((u8*)&lbl_8036C678 + 0x14);
    ((void(*)(u8*))*(u32*)(class_info + 0x38))(ptr);
}
#endif
#pragma pop

/* 0x80193D30 | 0x198 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern void __assert(const char*, u32, const char*);
extern HSD_CObjInfo lbl_8036C678;
extern char lbl_8047D958;
extern char lbl_8047D960;
static inline BOOL ref_DEC_v(void* o)
{
    BOOL ret;
    if ((ret = (HSD_OBJ(o)->ref_count == HSD_OBJ_NOREF))) {
        return ret;
    }
    ret = (*(volatile u16*)&HSD_OBJ(o)->ref_count == 0);
    HSD_OBJ(o)->ref_count--;
    return ret;
}
#if 0
asm void fn_80193D30(void) {
#include "src/hsd/hsd_cobj_fn_80193D30.inc"
}
#else
/* decompiled wrk5: functional (TU not byte-measurable) — CObj destructor:
   removes its anim, releases the eye/interest WObjs (ref-counted), frees the
   projection matrix, then chains to the parent class destroy method. */
void fn_80193D30(HSD_CObj* cobj) {
    extern void fn_801C25E4(void*);
    extern void HSD_MtxFree(void*);
    HSD_WObj* eyepos;
    HSD_WObj* interest;
    fn_801C25E4(cobj->aobj);
    if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
    eyepos = cobj->eyepos;
    if (eyepos != NULL) {
        if (ref_DEC_v(eyepos) && eyepos != NULL) {
            HSD_OBJECT_METHOD(eyepos)->release((HSD_Class*)eyepos);
            HSD_OBJECT_METHOD(eyepos)->destroy((HSD_Class*)eyepos);
        }
    }
    if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
    interest = cobj->interest;
    if (interest != NULL) {
        if (ref_DEC_v(interest) && interest != NULL) {
            HSD_OBJECT_METHOD(interest)->release((HSD_Class*)interest);
            HSD_OBJECT_METHOD(interest)->destroy((HSD_Class*)interest);
        }
    }
    if (cobj->proj_mtx != NULL) {
        HSD_MtxFree(cobj->proj_mtx);
    }
    {
        u32 class_info = *(u32*)((u8*)&lbl_8036C678 + 0x14);
        ((void (*)(HSD_CObj*)) *(u32*)(class_info + 0x30))(cobj);
    }
}
#endif
#pragma pop

/* 0x80193EC8 | 0x7C */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
#if 0
asm void fn_80193EC8(void) {
#include "src/hsd/hsd_cobj_fn_80193EC8.inc"
}
#else
int fn_80193EC8(HSD_CObj* cobj) {
    extern HSD_CObjInfo lbl_8036C678;
    HSD_WObj* interest;
    int result;
    result = HSD_CLASS_INFO(&lbl_8036C678)->head.parent->init((HSD_Class*)cobj);
    if (result < 0) return result;
    if (cobj != NULL) {
        cobj->flags |= 0xC0000000;
    }
    cobj->eyepos = HSD_WObjAlloc();
    interest = HSD_WObjAlloc();
    result = 0;
    cobj->interest = interest;
    return result;
}
#endif
#pragma pop

/* 0x80193F44 | 0xCC */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern HSD_ClassInfo* fn_80193748(const char*);
extern void* fn_80193828(void*);
#if 0
asm void fn_80193F44(void) {
#include "src/hsd/hsd_cobj_fn_80193F44.inc"
}
#else
void* fn_80193F44(u8* ptr) {
    extern u32 lbl_8047B230;
    extern HSD_CObjInfo lbl_8036C678;
    extern char lbl_8047D958;
    extern char lbl_8047D960;
    void* cobj_result;
    void* alloc_arg;
    HSD_ClassInfo* info;
    if (ptr == NULL) goto return_null;
    if (*(u32*)ptr != 0) {
        info = fn_80193748(*(const char**)ptr);
        if (info != 0) goto do_alloc_from_info;
    }
default_alloc:
    if (lbl_8047B230 == 0) {
        goto use_default;
    }
    alloc_arg = (void*)lbl_8047B230;
    goto have_arg;
use_default:
    alloc_arg = (void*)&lbl_8036C678;
have_arg:
    cobj_result = fn_80193828(alloc_arg);
    if (cobj_result == NULL) {
        __assert(&lbl_8047D958, 0x7a4, &lbl_8047D960);
    }
    goto setup;
do_alloc_from_info:
    cobj_result = fn_80193828(info);
    if (cobj_result == NULL) {
        __assert(&lbl_8047D958, 0x7f9, &lbl_8047D960);
    }
setup:
    {
        void** vtable = *(void***)cobj_result;
        ((void(*)(void*, u8*))vtable[0x3c / 4])(cobj_result, ptr);
    }
    return cobj_result;
return_null:
    return NULL;
}
#endif
#pragma pop

/* 0x80194010 | 0x248 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern void HSD_WObjInit(HSD_WObj*, HSD_WObjDesc*);
extern void fn_801947C8(HSD_CObj*, f32);   /* wrk8: typed-C takes (HSD_CObj*, f32) */
extern void fn_80194DA4();   /* wrk7: was `(void)`; typed-C takes (HSD_CObj*, f32*) */
extern f32 lbl_8036C6D4[]; /* default up vector (Vec3) */
#if 0
asm void fn_80194010(void) {
#include "src/hsd/hsd_cobj_fn_80194010.inc"
}
#else
/* decompiled wrk6: functional (TU not byte-measurable) — CObj class `load`:
   initialises a CObj from its descriptor. Copies flags, viewport (s16->f32),
   scissor and near/far; binds the eye/interest WObjs; sets the orientation
   from either an explicit up vector or a roll angle; then installs the
   projection parameters for perspective/frustum/ortho. Asserts on an unknown
   projection type. Returns 0 (success). The leading null-guards mirror the
   original's inlined HSD_ASSERT checks. */
int fn_80194010(HSD_CObj* cobj, HSD_CObjDesc* desc)
{
    /* flags: net effect is cobj->flags = desc->flags (the original also does a
       redundant top-2-bit-preserving rlwimi merge, a no-op for u16 flags). */
    if (cobj != NULL) {
        cobj->flags = desc->common.flags;
    }
    if (cobj != NULL) {
        cobj->viewport.xmin = (f32) desc->common.viewport.xmin;
        cobj->viewport.xmax = (f32) desc->common.viewport.xmax;
        cobj->viewport.ymin = (f32) desc->common.viewport.ymin;
        cobj->viewport.ymax = (f32) desc->common.viewport.ymax;
    }
    if (cobj != NULL) {
        cobj->scissor = desc->common.scissor;
    }
    HSD_WObjInit(cobj->eyepos, desc->common.eyepos);
    HSD_WObjInit(cobj->interest, desc->common.interest);
    if (cobj != NULL) {
        cobj->near = desc->common.nnear;
    }
    if (cobj != NULL) {
        cobj->far = desc->common.ffar;
    }
    if (desc->common.flags & 1) {
        void* up = desc->common.up_vector;
        if (up == NULL) {
            up = lbl_8036C6D4;
        }
        ((void (*)(HSD_CObj*, void*)) fn_80194DA4)(cobj, up);
    } else {
        ((void (*)(HSD_CObj*, f32)) fn_801947C8)(cobj, desc->common.roll);
    }
    switch (desc->common.projection_type) {
    case PROJ_PERSPECTIVE:
        if (cobj != NULL) {
            cobj->projection_type = PROJ_PERSPECTIVE;
            cobj->projection_param.perspective.fov = desc->perspective.fov;
            cobj->projection_param.perspective.aspect = desc->perspective.aspect;
        }
        break;
    case PROJ_FRUSTUM:
        if (cobj != NULL) {
            cobj->projection_type = PROJ_FRUSTUM;
            cobj->projection_param.frustum.top = desc->frustum.top;
            cobj->projection_param.frustum.bottom = desc->frustum.bottom;
            cobj->projection_param.frustum.left = desc->frustum.left;
            cobj->projection_param.frustum.right = desc->frustum.right;
        }
        break;
    case PROJ_ORTHO:
        if (cobj != NULL) {
            cobj->projection_type = PROJ_ORTHO;
            cobj->projection_param.ortho.top = desc->ortho.top;
            cobj->projection_param.ortho.bottom = desc->ortho.bottom;
            cobj->projection_param.ortho.left = desc->ortho.left;
            cobj->projection_param.ortho.right = desc->ortho.right;
        }
        break;
    default:
        HSD_ASSERT(2002, 0);
        break;
    }
    return 0;
}
#endif
#pragma pop

/* 0x80194258 | 0x60 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
#pragma peephole off
#if 0
asm void fn_80194258(void) {
#include "src/hsd/hsd_cobj_fn_80194258.inc"
}
#else
void* fn_80194258(void) {
    extern u32 lbl_8047B230;
    extern HSD_CObjInfo lbl_8036C678;
    extern char lbl_8047D958;
    extern char lbl_8047D960;
    void* result;
    result = fn_80193828(lbl_8047B230 != 0 ? (void*)lbl_8047B230 : (void*)&lbl_8036C678);
    if (result == NULL) {
        __assert(&lbl_8047D958, 0x7a4, &lbl_8047D960);
    }
    return result;
}
#endif
#pragma pop

/* 0x801942B8 | 0x8 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetCurrent(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetCurrent.inc"
}
#else
HSD_CObj* HSD_CObjGetCurrent(void) { extern u32 lbl_8047B234; return (HSD_CObj*)lbl_8047B234; }
#endif
#pragma pop

/* 0x801942C0 | 0x5C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetOrtho(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetOrtho.inc"
}
#else
void HSD_CObjGetOrtho(HSD_CObj* cobj, f32* a, f32* b, f32* c, f32* d) {
    if (cobj == NULL || cobj->projection_type != PROJ_ORTHO) {
        return;
    }
    if (a != NULL) *a = cobj->projection_param.ortho.top;
    if (b != NULL) *b = cobj->projection_param.ortho.bottom;
    if (c != NULL) *c = cobj->projection_param.ortho.left;
    if (d != NULL) *d = cobj->projection_param.ortho.right;
}
#endif
#pragma pop

/* 0x8019431C | 0x3C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetPerspective(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetPerspective.inc"
}
#else
void HSD_CObjGetPerspective(HSD_CObj* cobj, f32* a, f32* b) {
    if (cobj == NULL || cobj->projection_type != PROJ_PERSPECTIVE) {
        return;
    }
    if (a != NULL) *a = cobj->projection_param.perspective.fov;
    if (b != NULL) *b = cobj->projection_param.perspective.aspect;
}
#endif
#pragma pop

/* 0x80194358 | 0x24 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetOrtho(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetOrtho.inc"
}
#else
void HSD_CObjSetOrtho(HSD_CObj* cobj, f32 f1, f32 f2, f32 f3, f32 f4) {
    u8* ptr = (u8*) cobj;
    if (ptr == NULL) return;
    ptr[0x50] = 3;
    *(f32*)(ptr + 0x40) = f1;
    *(f32*)(ptr + 0x44) = f2;
    *(f32*)(ptr + 0x48) = f3;
    *(f32*)(ptr + 0x4C) = f4;
}
#endif
#pragma pop

/* 0x8019437C | 0x24 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetFrustum(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetFrustum.inc"
}
#else
void HSD_CObjSetFrustum(HSD_CObj* cobj, f32 f1, f32 f2, f32 f3, f32 f4) {
    u8* ptr = (u8*) cobj;
    if (ptr == NULL) return;
    ptr[0x50] = 2;
    *(f32*)(ptr + 0x40) = f1;
    *(f32*)(ptr + 0x44) = f2;
    *(f32*)(ptr + 0x48) = f3;
    *(f32*)(ptr + 0x4C) = f4;
}
#endif
#pragma pop

/* 0x801943A0 | 0x1C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetPerspective(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetPerspective.inc"
}
#else
void HSD_CObjSetPerspective(HSD_CObj* cobj, f32 f1, f32 f2) {
    u8* ptr = (u8*) cobj;
    if (ptr == NULL) return;
    ptr[0x50] = 1;
    *(f32*)(ptr + 0x40) = f1;
    *(f32*)(ptr + 0x44) = f2;
}
#endif
#pragma pop

/* 0x801943BC | 0x10 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetProjectionType(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetProjectionType.inc"
}
#else
void HSD_CObjSetProjectionType(HSD_CObj* cobj, u32 val) { u8* ptr = (u8*) cobj; if (ptr == NULL) return; ptr[0x50] = val; }
#endif
#pragma pop

/* 0x801943CC | 0x18 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetProjectionType(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetProjectionType.inc"
}
#else
int HSD_CObjGetProjectionType(HSD_CObj* cobj) {
    u8* ptr = (u8*) cobj;
    if (ptr == NULL) return 1;
    return ptr[0x50];
}
#endif
#pragma pop

/* 0x801943E4 | 0x1C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetViewportfx4(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetViewportfx4.inc"
}
#else
void HSD_CObjSetViewportfx4(HSD_CObj* cobj, f32 f1, f32 f2, f32 f3, f32 f4) {
    u8* ptr = (u8*) cobj;
    if (ptr == NULL) return;
    *(f32*)(ptr + 0x0C) = f1;
    *(f32*)(ptr + 0x10) = f2;
    *(f32*)(ptr + 0x14) = f3;
    *(f32*)(ptr + 0x18) = f4;
}
#endif
#pragma pop

/* 0x80194400 | 0xA4 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
#if 0
asm void fn_80194400(void) {
#include "src/hsd/hsd_cobj_fn_80194400.inc"
}
#else
/* decompiled wrk4 2026-06-16: functional (TU not byte-measurable).
 * Set the camera viewport from an s16 rect (signed lha + xoris/0x8000 magic
 * => (f32)(s16) on each of the four halfwords). */
void fn_80194400(HSD_CObj* cobj, HSD_RectS16* rect) {
    if (cobj == NULL) {
        return;
    }
    cobj->viewport.xmin = (f32) rect->xmin;
    cobj->viewport.xmax = (f32) rect->xmax;
    cobj->viewport.ymin = (f32) rect->ymin;
    cobj->viewport.ymax = (f32) rect->ymax;
}
#endif
#pragma pop

/* 0x801944A4 | 0x1C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetScissorx4(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetScissorx4.inc"
}
#else
void HSD_CObjSetScissorx4(HSD_CObj* cobj, u16 a, u16 b, u16 c, u16 d) {
    u8* ptr = (u8*) cobj;
    if (ptr == NULL) return;
    *(u16*)(ptr + 0x1C) = a;
    *(u16*)(ptr + 0x1E) = b;
    *(u16*)(ptr + 0x20) = c;
    *(u16*)(ptr + 0x22) = d;
}
#endif
#pragma pop

/* 0x801944C0 | 0x10 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetFar(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetFar.inc"
}
#else
void HSD_CObjSetFar(HSD_CObj* cobj, f32 val) { u8* ptr = (u8*) cobj; if (ptr == NULL) return; *(f32*)(ptr + 0x3C) = val; }
#endif
#pragma pop

/* 0x801944D0 | 0x18 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetFar(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetFar.inc"
}
#else
f32 HSD_CObjGetFar(HSD_CObj* cobj) {
    u8* ptr = (u8*) cobj;
    extern f32 lbl_8047D978;
    if (ptr == NULL) return lbl_8047D978;
    return *(f32*)(ptr + 0x3C);
}
#endif
#pragma pop

/* 0x801944E8 | 0x10 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetNear(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetNear.inc"
}
#else
void HSD_CObjSetNear(HSD_CObj* cobj, f32 val) { u8* ptr = (u8*) cobj; if (ptr == NULL) return; *(f32*)(ptr + 0x38) = val; }
#endif
#pragma pop

/* 0x801944F8 | 0x18 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetNear(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetNear.inc"
}
#else
f32 HSD_CObjGetNear(HSD_CObj* cobj) {
    u8* ptr = (u8*) cobj;
    extern f32 lbl_8047D978;
    if (ptr == NULL) return lbl_8047D978;
    return *(f32*)(ptr + 0x38);
}
#endif
#pragma pop

/* 0x80194510 | 0xA0 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern double fn_800CE220(f32);
extern f32 lbl_8047D978;
extern f32 lbl_8047D97C;
extern f32 lbl_8047D980;
#if 0
asm void fn_80194510(void) {
#include "src/hsd/hsd_cobj_fn_80194510.inc"
}
#else
/* NEAR (W1 reg-permutation in tan-result FP chain): case1 keeps tan in f1, decomp evacuates to f2. */
f32 fn_80194510(u8* ptr) {
    if (ptr == NULL) {
        return lbl_8047D978;
    }
    switch (ptr[0x50]) {
    case 1:
        return *(f32*)(ptr + 0x44) * (*(f32*)(ptr + 0x38) * (f32) fn_800CE220(lbl_8047D97C * (lbl_8047D980 * *(f32*)(ptr + 0x40))));
    case 2:
        return *(f32*)(ptr + 0x4C);
    case 3:
        return *(f32*)(ptr + 0x4C);
    default:
        return lbl_8047D978;
    }
}
#endif
#pragma pop

/* 0x801945B0 | 0xA4 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
#if 0
asm void fn_801945B0(void) {
#include "src/hsd/hsd_cobj_fn_801945B0.inc"
}
#else
/* MATCH 100% (opt1 + case1 split into inner/x44 temps: -x38*tan accumulates in f0, x44 loaded
 * after into f2, final fmuls written x44*inner). */
f32 fn_801945B0(u8* ptr) {
    if (ptr == NULL) {
        return lbl_8047D978;
    }
    switch (ptr[0x50]) {
    case 1:
    {
        f32 inner = -*(f32*)(ptr + 0x38) * (f32) fn_800CE220(lbl_8047D97C * (lbl_8047D980 * *(f32*)(ptr + 0x40)));
        f32 x44 = *(f32*)(ptr + 0x44);
        return x44 * inner;
    }
    case 2:
        return *(f32*)(ptr + 0x48);
    case 3:
        return *(f32*)(ptr + 0x48);
    default:
        return lbl_8047D978;
    }
}
#endif
#pragma pop

/* 0x80194654 | 0x9C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80194654(void) {
#include "src/hsd/hsd_cobj_fn_80194654.inc"
}
#else
f32 fn_80194654(u8* ptr) {
    if (ptr == NULL) {
        return lbl_8047D978;
    }
    switch (ptr[0x50]) {
    case 1:
        return -*(f32*)(ptr + 0x38) * (f32) fn_800CE220(lbl_8047D97C * (lbl_8047D980 * *(f32*)(ptr + 0x40)));
    case 2:
        return *(f32*)(ptr + 0x44);
    case 3:
        return *(f32*)(ptr + 0x44);
    default:
        return lbl_8047D978;
    }
}
#endif
#pragma pop

/* 0x801946F0 | 0x98 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801946F0(void) {
#include "src/hsd/hsd_cobj_fn_801946F0.inc"
}
#else
/* decompiled wrk4 2026-06-16: functional (TU not byte-measurable).
 * Get the top extent of the view frustum. Twin of fn_80194654 (which returns
 * the bottom, i.e. -near*tan); this returns +near*tan(D97C * D980 * fov) for
 * perspective, the +0x40 field for frustum/ortho, else the default. */
f32 fn_801946F0(HSD_CObj* cobj) {
    extern f32 lbl_8047D978; /* default (no/invalid camera) */
    extern f32 lbl_8047D97C; /* deg->rad scale            */
    extern f32 lbl_8047D980; /* 0.5 (half-fov)            */
    extern double fn_800CE220(f32); /* tan */
    if (cobj == NULL) {
        return lbl_8047D978;
    }
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE:
        return cobj->near *
               (f32) fn_800CE220(lbl_8047D97C *
                                 (lbl_8047D980 *
                                  cobj->projection_param.perspective.fov));
    case PROJ_FRUSTUM:
        return cobj->projection_param.frustum.top;
    case PROJ_ORTHO:
        return cobj->projection_param.ortho.top;
    default:
        return lbl_8047D978;
    }
}
#endif
#pragma pop

/* 0x80194788 | 0x20 */
#if 0
asm void HSD_CObjSetAspect(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetAspect.inc"
}
#else
void HSD_CObjSetAspect(HSD_CObj* cobj, f32 val)
{
    if (cobj == NULL || cobj->projection_type != 1) {
        return;
    }
    cobj->projection_param.perspective.aspect = val;
}
#endif

/* 0x801947A8 | 0x20 */
void HSD_CObjSetFov(HSD_CObj* cobj, f32 val)
{
    if (cobj == NULL || cobj->projection_type != PROJ_PERSPECTIVE) {
        return;
    }
    cobj->projection_param.perspective.fov = val;
}

/* 0x801947C8 | 0x464 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
extern void OSReport(const char* fmt, ...);
extern void fn_800A3244(void);
extern void fn_800A3820(void);
extern void fn_800A3A9C(void*, void*, void*);
extern void fn_800A3ADC(void);
extern void fn_800CE59C(void);
extern void fn_800CE718(void);
extern int fn_80194C2C(f32*, void*);   /* wrk8: real sig (was `void (void)`) */
extern void HSD_CObjSetMtxDirty(HSD_CObj*);
extern int fn_8019513C(HSD_CObj*, f32, f32*);   /* wrk8: typed-C returns int */
extern f32 fn_80195590(HSD_CObj*, f32*);
extern void HSD_CObjGetEyePosition(HSD_CObj*, void*);
extern void HSD_CObjGetInterest(HSD_CObj*, void*);
#if 0
asm void fn_801947C8(void) {
#include "src/hsd/hsd_cobj_fn_801947C8.inc"
}
#else
/* decompiled wrk8 2026-06-16: functional (TU not byte-measurable) — HSD_CObjSetRoll.
   In plain roll mode (flags&1 clear) it just records the roll and marks the
   matrices dirty if it changed. In up-vector mode (flags&1 set) it rebuilds the
   up-vector that corresponds to `roll` — the same horizon-perp + rotate-about-dir
   construction as fn_8019513C — then commits it through the (inlined)
   HSD_CObjSetUpVector store logic. The trailing flags&1==0 sub-path is structurally
   dead given the up-vector entry condition but is reproduced faithfully from the asm.
   INFERRED: the near-vertical threshold and the sqrt-degenerate fallback are
   anonymous SDA2 doubles reconstructed by value; lbl_80274654 (up-path assert msg)
   is an anonymous SDA2 string — all flagged for review. */
void fn_801947C8(HSD_CObj* cobj, f32 roll)
{
    extern f32 lbl_80478AC0;   /* sqrt-degenerate fallback (anonymous SDA2, ~0.0f) — INFERRED */
    extern f32 lbl_80478AC8;   /* degenerate-vector epsilon */
    extern char lbl_8047D958;
    extern char lbl_8047D960;
    extern char lbl_8027464C;  /* OSReport fmt: up-vector degenerate warning */
    extern char lbl_80274654;  /* up-path __assert message (anonymous SDA2) — INFERRED */
    extern f32 sqrtf(f32);
    f32 eye[3];        /* sp+0x14 */
    f32 interest[3];   /* sp+0x20 */
    f32 dir[3];        /* sp+0x38 */
    f32 perp[3];       /* sp+0x44 */
    f32 rotated[3];    /* sp+0x50 */
    f32 newup[3];      /* sp+0x5c */
    f32 normup[3];     /* sp+0x2c */
    f32 mtx[3][4];     /* sp+0x68 */
    f32 eps = lbl_80478AC8;

    if (cobj == NULL) {
        return;
    }
    if (!(cobj->flags & 1)) {
        /* plain roll-storage mode */
        if (cobj->u.roll != roll) {
            cobj->flags |= 0xC0000000;
        }
        cobj->u.roll = roll;
        return;
    }

    /* up-vector mode: rebuild the up-vector that matches `roll`. */
    {
        int ok;
        if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL) {
            ok = 0;
        } else {
            HSD_CObjGetEyePosition(cobj, eye);
            HSD_CObjGetInterest(cobj, interest);
            fn_800A3A9C(interest, eye, dir);                 /* dir = interest - eye */
            ok = (((int (*)(f32*, f32*)) fn_80194C2C)(dir, dir) == 0); /* normalize + guard */
        }
        if (ok) {
            f32 absy = ((f32 (*)(f32)) fn_800CE59C)(dir[1]); /* fabsf */
            if (1.0f - absy >= 1e-4f /* near-1 eps — INFERRED */) {
                f32 q = dir[0] * dir[0] + dir[2] * dir[2];
                f32 len = (q > 0.0f) ? sqrtf(q) : lbl_80478AC0;
                f32 s = -dir[1] / len;
                perp[0] = dir[0] * s;
                perp[1] = len;
                perp[2] = dir[2] * s;
            } else {
                f32 q = dir[1] * dir[1] + dir[2] * dir[2];
                f32 len = (q > 0.0f) ? sqrtf(q) : lbl_80478AC0;
                f32 s = -dir[0] / len;
                perp[0] = len;
                perp[1] = dir[1] * s;
                perp[2] = dir[2] * s;
            }
            ((void (*)(f32*, f32*, f32)) fn_800A3244)(&mtx[0][0], dir, -roll);
            ((void (*)(f32*, f32*, f32*)) fn_800A3820)(&mtx[0][0], perp, rotated);
            ((void (*)(f32*, f32*)) fn_800A3ADC)(rotated, newup); /* newup = normalized up */
        }
    }

    /* ---- inlined HSD_CObjSetUpVector(cobj, newup) ---- */
    if (cobj == NULL) {
        return;
    }
    if (cobj->flags & 1) {
        int ok;
        if (((f32 (*)(f32)) fn_800CE59C)(newup[0]) <= eps &&
            ((f32 (*)(f32)) fn_800CE59C)(newup[1]) <= eps &&
            ((f32 (*)(f32)) fn_800CE59C)(newup[2]) <= eps) {
            ok = -1; /* degenerate up-vector */
        } else {
            ((void (*)(f32*, f32*)) fn_800A3ADC)(newup, normup); /* normalize -> normup */
            ok = 0;
        }
        if (ok != 0) {
            OSReport(&lbl_8027464C);
            __assert(&lbl_8047D958, 0x3e4, &lbl_80274654);
        }
        if (cobj->u.up.x != normup[0] || cobj->u.up.y != normup[1] ||
            cobj->u.up.z != normup[2]) {
            cobj->flags |= 0xC0000000;
            cobj->u.up.x = normup[0];
            cobj->u.up.y = normup[1];
            cobj->u.up.z = normup[2];
        }
    } else {
        /* structurally-dead given flags&1 entry; reproduced faithfully */
        f32 tmp[3];
        roll = fn_80195590(cobj, newup);
        if (cobj == NULL) {
            return;
        }
        if (cobj->flags & 1) {
            ((void (*)(HSD_CObj*, f32, f32*)) fn_8019513C)(cobj, roll, tmp);
            ((void (*)(HSD_CObj*, f32*)) fn_80194DA4)(cobj, tmp);
        } else {
            if (cobj->u.roll != roll) {
                HSD_CObjSetMtxDirty(cobj);
            }
            cobj->u.roll = roll;
        }
    }
}
#endif
#pragma pop

/* 0x80194C2C | 0x98 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
extern f32 lbl_80478AC8;
extern void fn_800A3ADC(void);
#if 0
asm void fn_80194C2C(void) {
#include "src/hsd/hsd_cobj_fn_80194C2C.inc"
}
#else
/* decompiled wrk4 2026-06-16: functional (TU not byte-measurable).
 * Degenerate-vector guard: if every component of the 3-vector is within the
 * epsilon lbl_80478AC8 the direction is unusable -> return -1. Otherwise run
 * fn_800A3ADC on (vec,out) (the asm leaves r3=vec/r4=out live across the call,
 * inferred (f32*,void*) signature) and report success (0). */
int fn_80194C2C(f32* vec, void* out) {
    f32 epsilon = lbl_80478AC8;
    if (vec == NULL || out == NULL) {
        return -1;
    }
    if ((vec[0] < 0.0f ? -vec[0] : vec[0]) <= epsilon &&
        (vec[1] < 0.0f ? -vec[1] : vec[1]) <= epsilon &&
        (vec[2] < 0.0f ? -vec[2] : vec[2]) <= epsilon) {
        return -1;
    }
    ((void (*)(f32*, void*)) fn_800A3ADC)(vec, out);
    return 0;
}
#endif
#pragma pop

/* WP-0061 external references */
extern void fn_80191688(HSD_WObj*, void*);
extern void HSD_WObjSetPosition(HSD_WObj*, void*);
extern void fn_801C25E4(void);
extern void fn_801C2670(void);
extern void fn_801C27F4(void*, void*, void*);
extern void fn_801C29C4(void*, f32);
/* WP-0061 forward declarations (defined later in same TU) */
extern void __assert(const char*, u32, const char*);

/* 0x80194CC4 | 0x30 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern void fn_80195F0C(HSD_CObj*);
#if 0
asm void HSD_CObjGetViewingMtxPtr(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetViewingMtxPtr.inc"
}
#else
f32* HSD_CObjGetViewingMtxPtr(HSD_CObj* cobj)
{
    fn_80195F0C(cobj);
    return cobj->view_mtx[0];
}
#endif
#pragma pop

/* 0x80194CF4 | 0x6C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern f32* HSD_MtxAlloc(void);
extern void PSMTXInverse(f32*, f32*);
#if 0
asm void HSD_CObjGetInvViewingMtxPtrDirect(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetInvViewingMtxPtrDirect.inc"
}
#else
f32* HSD_CObjGetInvViewingMtxPtrDirect(HSD_CObj* cobj) {
    if (cobj->flags & 0x80000000) {
        if (cobj->proj_mtx == NULL) {
            cobj->proj_mtx = HSD_MtxAlloc();
        }
        PSMTXInverse(cobj->view_mtx[0], cobj->proj_mtx);
        if (cobj != NULL) {
            cobj->flags &= ~0x80000000;
        }
    }
    return cobj->proj_mtx;
}
#endif
#pragma pop

/* 0x80194D60 | 0x34 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern void fn_800A2D64(void*, void*);
#if 0
asm void HSD_CObjGetViewingMtx(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetViewingMtx.inc"
}
#else
void HSD_CObjGetViewingMtx(HSD_CObj* ptr, f32 arg2[3][4]) {
    fn_800A2D64(HSD_CObjGetViewingMtxPtr((HSD_CObj*) ptr), arg2);
}
#endif
#pragma pop

/* 0x80194D94 | 0x10 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetMtxDirty(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetMtxDirty.inc"
}
#else
void HSD_CObjSetMtxDirty(HSD_CObj* ptr) { *(u32*)((u8*) ptr + 0x8) |= 0xC0000000; }
#endif
#pragma pop

/* 0x80194DA4 | 0x32C */
#pragma push
#pragma optimization_level 3
#pragma optimizewithasm off
extern void OSReport(const char* fmt, ...);
extern void fn_800A3244(void);
extern void fn_800A3458(void);
extern void fn_800A3820(void);
extern void fn_800A3A9C(void*, void*, void*);
extern void fn_800A3ADC(void);
extern void fn_800A3B7C(void);
extern void fn_800CE2D8(void);
extern void fn_80191688(HSD_WObj*, void*);
extern void __assert(const char*, u32, const char*);
#if 0
asm void fn_80194DA4(void) {
#include "src/hsd/hsd_cobj_fn_80194DA4.inc"
}
#else
/* decompiled wrk7: functional (TU not byte-measurable) — HSD_CObjSetUpVector,
   the storage twin of the getter fn_80195590. Sets the camera up-vector / roll
   from `up` (r4, a 3-float Vec; inferred f32*).
   - Explicit-up mode (flags & 1 set): guard `up` against the degenerate epsilon
     lbl_80478AC8; if degenerate, warn (OSReport + __assert); else normalize it and,
     when it differs from the stored cobj->u.up, mark the matrices dirty
     (flags |= 0xC0000000) and store it.
   - Roll mode (flags & 1 clear): rebuild the eye->interest direction and derive a
     roll angle exactly as fn_80195590 does, then commit it via fn_801947C8.
   The +/-PI/2 results below are anonymous SDA2 literals reconstructed by value;
   flagged for orchestrator review. */
void fn_80194DA4(HSD_CObj* cobj, f32* up)
{
    extern f32 lbl_80478AC8;    /* degenerate-vector epsilon */
    extern f32 lbl_8036C6BC[];  /* look-at reference eye vector (shared with fn_80195590) */
    extern f32 lbl_8036C6C8[];  /* look-at reference up vector  (shared with fn_80195590) */
    extern char lbl_8027464C;   /* OSReport fmt: up-vector degenerate warning */
    extern char lbl_80274654;   /* up-path __assert message (SDA2 r2-23864) — INFERRED name */
    f32 epsilon = lbl_80478AC8;

    if (cobj == NULL) {
        return;
    }
    if (up == NULL) {
        return;
    }

    if (cobj->flags & 1) {
        /* ---- explicit up-vector path ---- */
        f32 newup[3];
        int ok;

        if (up == NULL) {
            ok = -1;
        } else if ((up[0] < 0.0f ? -up[0] : up[0]) <= epsilon &&
                   (up[1] < 0.0f ? -up[1] : up[1]) <= epsilon &&
                   (up[2] < 0.0f ? -up[2] : up[2]) <= epsilon) {
            ok = -1; /* degenerate up-vector */
        } else {
            ((void (*)(f32*, f32*)) fn_800A3ADC)(up, newup); /* normalize -> newup */
            ok = 0;
        }
        if (ok != 0) {
            OSReport(&lbl_8027464C);
            __assert(&lbl_8047D958, 0x3e4, &lbl_80274654);
        }
        if (cobj->u.up.x != newup[0] || cobj->u.up.y != newup[1] ||
            cobj->u.up.z != newup[2]) {
            cobj->flags |= 0xC0000000;
            cobj->u.up.x = newup[0];
            cobj->u.up.y = newup[1];
            cobj->u.up.z = newup[2];
        }
    } else {
        /* ---- roll path (twin of fn_80195590) ---- */
        f32 eye[3];
        f32 interest[3];
        f32 dir[3];
        f32 mtx[3][4];
        f32 out[3];
        f32 roll;
        int ok;

        if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL) {
            ok = 0;
        } else {
            if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
            if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
            fn_80191688(cobj->eyepos, eye);
            if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
            if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
            fn_80191688(cobj->interest, interest);
            fn_800A3A9C(interest, eye, dir);
            if ((dir[0] < 0.0f ? -dir[0] : dir[0]) <= epsilon &&
                (dir[1] < 0.0f ? -dir[1] : dir[1]) <= epsilon &&
                (dir[2] < 0.0f ? -dir[2] : dir[2]) <= epsilon) {
                ok = 0; /* direction ~= 0: unusable */
            } else {
                ((void (*)(f32*, f32*)) fn_800A3ADC)(dir, dir); /* normalize in place */
                ok = 1;
            }
        }
        if (!ok) {
            roll = 0.0f;
        } else {
            f32 dot = ((f32 (*)(f32*, f32*)) fn_800A3B7C)(up, dir);
            f32 absdot = dot < 0.0f ? -dot : dot;
            if (1.0f - absdot < epsilon) {
                roll = 0.0f; /* up nearly parallel to dir: unusable */
            } else {
                C_MTXLookAt(mtx, lbl_8036C6BC, lbl_8036C6C8, dir);
                ((void (*)(f32*, f32*, f32*)) fn_800A3820)(&mtx[0][0], up, out);
                if (out[1] == 0.0f) {
                    roll = (-out[0] >= 0.0f) ? 1.5707964f : -1.5707964f; /* +/-PI/2 */
                } else {
                    roll = ((f32 (*)(f32, f32)) fn_800CE2D8)(-out[0], out[1]);
                }
            }
        }
        ((void (*)(HSD_CObj*, f32)) fn_801947C8)(cobj, roll);
    }
}
#endif
#pragma pop

/* 0x801950D0 | 0x6C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern int fn_8019513C(HSD_CObj*, f32, f32*);   /* wrk8: typed-C returns int */
#if 0
asm void HSD_CObjGetUpVector(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetUpVector.inc"
}
#else
/* decompiled wrk6: functional (TU not byte-measurable).
   Returns the camera's up vector. If the CObj stores an explicit up vector
   (flag bit 0 set), copy its x/y/z out and return 1; otherwise derive the up
   vector from the stored roll angle via fn_8019513C and return its result.
   Returns 0 when cobj or the output pointer is NULL.
   `up` is a 3-float Vec (no Vec type in these headers — inferred f32[3]). */
int HSD_CObjGetUpVector(HSD_CObj* cobj, f32* up)
{
    if (cobj == NULL || up == NULL) {
        return 0;
    }
    if (cobj->flags & 1) {
        up[0] = cobj->u.up.x;
        up[1] = cobj->u.up.y;
        up[2] = cobj->u.up.z;
        return 1;
    }
    return ((int (*)(HSD_CObj*, f32, f32*)) fn_8019513C)(cobj, cobj->u.roll, up);
}
#endif
#pragma pop

/* 0x8019513C | 0x454 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
extern void fn_800A3820(void);
extern void fn_800A3A9C(void*, void*, void*);
extern void fn_800A3ADC(void);
#if 0
asm void fn_8019513C(void) {
#include "src/hsd/hsd_cobj_fn_8019513C.inc"
}
#else
/* decompiled wrk8 2026-06-16: functional (TU not byte-measurable) — derives the
   camera up-vector that corresponds to a roll angle. Builds the eye->interest
   direction; returns 0 if it is degenerate. Otherwise builds a horizon up-vector
   perpendicular to dir (Gram-Schmidt of a world axis against dir, choosing the
   axis that avoids gimbal lock), rotates it about dir by -roll, normalises it into
   `out`, and returns 1. The exact-twin of the roll path inlined inside
   fn_801947C8. INFERRED: the near-vertical threshold and the sqrt-degenerate
   fallback are anonymous SDA2 doubles reconstructed by value (flagged). */
int fn_8019513C(HSD_CObj* cobj, f32 roll, f32* out)
{
    extern f32 lbl_80478AC0;   /* sqrt-degenerate fallback (anonymous SDA2, ~0.0f) — INFERRED */
    extern f32 lbl_80478AC8;   /* degenerate-vector epsilon */
    extern char lbl_8047D958;
    extern char lbl_8047D960;
    extern f32 sqrtf(f32);
    f32 eye[3];        /* sp+0x10 */
    f32 interest[3];   /* sp+0x1c */
    f32 dir[3];        /* sp+0x40 */
    f32 perp[3];       /* sp+0x34 */
    f32 rotated[3];    /* sp+0x28 */
    f32 mtx[3][4];     /* sp+0x4c */
    f32 eps = lbl_80478AC8;
    int ok;

    if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL) {
        ok = 0;
    } else {
        if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        fn_80191688(cobj->eyepos, eye);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        fn_80191688(cobj->interest, interest);
        fn_800A3A9C(interest, eye, dir);   /* dir = interest - eye */
        if ((dir[0] < 0.0f ? -dir[0] : dir[0]) <= eps &&
            (dir[1] < 0.0f ? -dir[1] : dir[1]) <= eps &&
            (dir[2] < 0.0f ? -dir[2] : dir[2]) <= eps) {
            ok = 0; /* direction ~= 0: unusable (asm sets -1, folded to 0 below) */
        } else {
            ((void (*)(f32*, f32*)) fn_800A3ADC)(dir, dir); /* normalize in place */
            ok = 1;
        }
    }
    if (!ok) {
        return 0;
    }

    /* horizon up-vector perpendicular to dir, picking the world axis that
       avoids gimbal lock near a vertical direction. */
    if (1.0f - (dir[1] < 0.0f ? -dir[1] : dir[1]) >= 1e-4f /* near-1 eps — INFERRED */) {
        /* dir not near-vertical: project world up (0,1,0) onto plane perp to dir */
        f32 q = dir[0] * dir[0] + dir[2] * dir[2];
        f32 len = (q > 0.0f) ? sqrtf(q) : lbl_80478AC0;
        f32 s = -dir[1] / len;
        perp[0] = dir[0] * s;
        perp[1] = len;
        perp[2] = dir[2] * s;
    } else {
        /* dir near-vertical: project world x (1,0,0) onto plane perp to dir */
        f32 q = dir[1] * dir[1] + dir[2] * dir[2];
        f32 len = (q > 0.0f) ? sqrtf(q) : lbl_80478AC0;
        f32 s = -dir[0] / len;
        perp[0] = len;
        perp[1] = dir[1] * s;
        perp[2] = dir[2] * s;
    }

    ((void (*)(f32*, f32*, f32)) fn_800A3244)(&mtx[0][0], dir, -roll); /* rot about dir by -roll */
    ((void (*)(f32*, f32*, f32*)) fn_800A3820)(&mtx[0][0], perp, rotated);
    ((void (*)(f32*, f32*)) fn_800A3ADC)(rotated, out); /* normalize -> out */
    return 1;
}
#endif
#pragma pop

/* 0x80195590 | 0x204 */
#pragma push
#pragma optimization_level 3
#pragma optimizewithasm off
extern f32 fn_800A3B38(void*);
extern void __assert(const char*, u32, const char*);
extern void fn_80191688(HSD_WObj*, void*);
extern void fn_800A3A9C(void*, void*, void*);
extern void fn_800A3ADC(void);
extern void fn_800A3820(void);
extern void fn_800A3B7C(void);
extern void fn_800CE2D8(void);
extern f32 lbl_80478AC8;
extern char lbl_8047D958;
extern char lbl_8047D960;
extern f32 lbl_8036C6BC[];
extern f32 lbl_8036C6C8[];
#if 0
asm void fn_80195590(void) {
#include "src/hsd/hsd_cobj_fn_80195590.inc"
}
#else
/* decompiled wrk5: functional (TU not byte-measurable) — computes a roll/twist
   angle for the camera. Builds the eye->interest direction; bails (returns 0)
   if it is degenerate (near-zero length, or nearly parallel to `arg`). Otherwise
   builds a reference look-at frame and returns the twist angle of `arg` about it.
   INFERRED: `arg` (r4) typed as f32* (matrix/vector — exact type uncertain);
   the +/-PI/2 constants below are anonymous SDA2 float literals (r2-0x5CDC /
   r2-0x5CD8) reconstructed by value. Both flagged for orchestrator review. */
f32 fn_80195590(HSD_CObj* cobj, f32* arg) {
    f32 eye[3];        /* sp+0x08 */
    f32 interest[3];   /* sp+0x14 */
    f32 dir[3];        /* sp+0x20 */
    f32 mtx[3][4];     /* sp+0x38 */
    f32 out[3];        /* sp+0x2c */
    int ok;

    if (cobj == NULL || cobj->eyepos == NULL || cobj->interest == NULL) {
        ok = 0;
    } else {
        if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        fn_80191688(cobj->eyepos, eye);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        fn_80191688(cobj->interest, interest);
        fn_800A3A9C(interest, eye, dir);
        if ((dir[0] < 0.0f ? -dir[0] : dir[0]) <= lbl_80478AC8 &&
            (dir[1] < 0.0f ? -dir[1] : dir[1]) <= lbl_80478AC8 &&
            (dir[2] < 0.0f ? -dir[2] : dir[2]) <= lbl_80478AC8) {
            ok = 0; /* direction ~= 0: unusable */
        } else {
            ((void (*)(f32*, f32*)) fn_800A3ADC)(dir, dir); /* normalize in place */
            ok = 1;
        }
    }
    if (!ok) {
        return 0.0f;
    }
    {
        f32 dot = ((f32 (*)(f32*, f32*)) fn_800A3B7C)(arg, dir);
        f32 absdot = dot < 0.0f ? -dot : dot;
        if (1.0f - absdot < lbl_80478AC8) {
            return 0.0f; /* dir nearly parallel to arg: unusable */
        }
    }
    C_MTXLookAt(mtx, lbl_8036C6BC, lbl_8036C6C8, dir);
    ((void (*)(f32*, f32*, f32*)) fn_800A3820)(&mtx[0][0], arg, out);
    if (out[1] == 0.0f) {
        return (-out[0] >= 0.0f) ? 1.5707964f : -1.5707964f; /* +/-PI/2 */
    }
    return ((f32 (*)(f32, f32)) fn_800CE2D8)(-out[0], out[1]);
}
#endif
#pragma pop

/* 0x80195794 | 0x104 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern char lbl_8047D958;
extern char lbl_8047D960;
extern char lbl_80274660[];
extern char lbl_80274670[];
#if 0
asm void HSD_CObjGetEyeDistance(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetEyeDistance.inc"
}
#else
f32 HSD_CObjGetEyeDistance(HSD_CObj* cobj) {
    f32 eye[3];
    f32 interest[3];
    f32 diff[3];

    if (cobj == NULL) {
        return lbl_8047D978;
    }
    if (cobj->eyepos == NULL) {
        __assert(&lbl_8047D958, 0x353, lbl_80274660);
    }
    if (cobj->interest == NULL) {
        __assert(&lbl_8047D958, 0x354, lbl_80274670);
    }
    if (cobj == NULL) {
        __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
    }
    if (cobj == NULL) {
        __assert(&lbl_8047D958, 0x2E8, &lbl_8047D960);
    }
    fn_80191688(cobj->eyepos, eye);
    if (cobj == NULL) {
        __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
    }
    if (cobj == NULL) {
        __assert(&lbl_8047D958, 0x2D0, &lbl_8047D960);
    }
    fn_80191688(cobj->interest, interest);
    fn_800A3A9C(interest, eye, diff);
    return fn_800A3B38(diff);
}
#endif
#pragma pop

extern char lbl_8047D958;
extern char lbl_8047D960;

/* 0x80195898 | 0x6C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetEyePosition(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetEyePosition.inc"
}
#else
void HSD_CObjSetEyePosition(HSD_CObj* cobj, void* arg) {
    if (!cobj) __assert(&lbl_8047D958, 0x324, &lbl_8047D960);
    if (!cobj) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
    HSD_WObjSetPosition(cobj->eyepos, arg);
}
#endif
#pragma pop

/* 0x80195904 | 0x6C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetEyePosition(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetEyePosition.inc"
}
#else
void HSD_CObjGetEyePosition(HSD_CObj* cobj, void* arg) {
    if (!cobj) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
    if (!cobj) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
    fn_80191688(cobj->eyepos, arg);
}
#endif
#pragma pop

/* 0x80195970 | 0x6C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjSetInterest(void) {
#include "src/hsd/hsd_cobj_HSD_CObjSetInterest.inc"
}
#else
void HSD_CObjSetInterest(HSD_CObj* cobj, void* arg) {
    if (!cobj) __assert(&lbl_8047D958, 0x30c, &lbl_8047D960);
    if (!cobj) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
    HSD_WObjSetPosition(cobj->interest, arg);
}
#endif
#pragma pop

/* 0x801959DC | 0x6C */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjGetInterest(void) {
#include "src/hsd/hsd_cobj_HSD_CObjGetInterest.inc"
}
#else
void HSD_CObjGetInterest(HSD_CObj* cobj, void* arg) {
    if (!cobj) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
    if (!cobj) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
    fn_80191688(cobj->interest, arg);
}
#endif
#pragma pop

/* 0x80195A48 | 0x24 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern void fn_801975FC(void);
extern void fn_801974A8(void);
#if 0
asm void fn_80195A48(void) {
#include "src/hsd/hsd_cobj_fn_80195A48.inc"
}
#else
/* WALL: CW 1.2.5n emits a compact 8-byte frame; this C is real-diffs=0 with --cw 1.3. */
void fn_80195A48(void) { fn_801975FC(); fn_801974A8(); }
#endif
#pragma pop

/* 0x80195A6C | 0x4A0 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#pragma optimize_for_size on
#pragma peephole off
extern void fn_800A3874(f32*, f32, f32, f32, f32, f32, f32);
extern void fn_800A3910(void);
extern void fn_800A39E0(f32*, f32, f32, f32, f32, f32, f32);
extern void fn_800BD2E0(f32*, int);
extern void fn_800BD7A0(int, int, int, int);
extern int fn_800C46B0(f32);
extern int fn_801960C4(HSD_CObj*);   /* wrk7: was `void (void)`; typed-C returns int, takes HSD_CObj* */
extern int fn_801963E0(HSD_CObj*);   /* wrk7: was `void (void)`; typed-C returns int, takes HSD_CObj* */
extern void HSD_Panic(const char*, u32, const char*);
extern void fn_80197400(void);
extern int fn_8019C7B0(void);
#if 0
asm void fn_80195A6C(void) {
#include "src/hsd/hsd_cobj_fn_80195A6C.inc"
}
#else
/* decompiled wrk8 2026-06-16: functional (TU not byte-measurable) — the camera
   "set current / program GX" dispatcher. Stores `current` (lbl_8047B234), queries
   the active render/projection mode (fn_8019C7B0) and ticks fn_80197400, then
   programs the GX viewport + scissor + projection matrix one of four ways:
     mode 0 : viewport/scissor scaled through the active EFB dims (lbl_80466BC0)
              and per-axis denominators (lbl_80478C50/C54) — sub-screen render.
     mode 1 : delegate to fn_801963E0 (plain EFB-clamped viewport).
     mode 2 : delegate to fn_801960C4 (scissor-box + ratio variant).
     mode 3 : viewport/scissor used directly (no EFB scaling).
   On success it recomputes the viewing matrix (fn_80195F0C) and returns 1; an
   unknown mode panics and returns 0. The viewport callback's last two args are
   the GX near/far Z (0.0, 1.0). INFERRED: the projection/scaling constants and the
   panic message string are anonymous SDA2 literals reconstructed by value/name. */
int fn_80195A6C(HSD_CObj* cobj)
{
    extern u32 lbl_8047B234;     /* `current` camera (SDA) */
    extern u8 lbl_80466BC0[];    /* GX render-mode global; +4/+6 = u16 EFB w/h, +0x18 = u8 offset flag */
    extern u32 lbl_80478C58;     /* viewport/scissor callback (default fn_80196C54) */
    extern s32 lbl_80478C50;     /* EFB-relative x denominator */
    extern s32 lbl_80478C54;     /* EFB-relative y denominator */
    extern char lbl_8047D958;
    extern char lbl_80274680[];  /* HSD_Panic message: bad camera mode — INFERRED name */
    extern f32 lbl_8047D978;     /* 0.0f (GX near Z) — named SDA2 constant */
    extern f32 lbl_8047D9B0;     /* 1.0f (GX far Z)  — named SDA2 constant */
    int mode;
    int ok;
    int ortho_flag;
    f32 mtx[3][4];

    if (cobj == NULL) {
        return 0;
    }
    ortho_flag = 0;
    mode = fn_8019C7B0();
    fn_80197400();
    lbl_8047B234 = (u32) cobj; /* current = cobj */

    switch (mode) {
    case 3: {
        /* direct path: viewport/scissor used verbatim */
        ((void (*)(int, f32, f32, f32, f32, f32, f32)) lbl_80478C58)(
            0,
            *(volatile f32*)&cobj->viewport.xmin,
            *(volatile f32*)&cobj->viewport.ymin,
            cobj->viewport.xmax - cobj->viewport.xmin,
            cobj->viewport.ymax - cobj->viewport.ymin,
            lbl_8047D978, lbl_8047D9B0);
        fn_800BD7A0(
            cobj->scissor.left, cobj->scissor.top,
            cobj->scissor.right - cobj->scissor.left,
            cobj->scissor.bottom - cobj->scissor.top);
        switch (cobj->projection_type) {
        case PROJ_PERSPECTIVE:
            ortho_flag = 0;
            C_MTXPerspective(&mtx[0][0],
                             cobj->projection_param.perspective.fov,
                             cobj->projection_param.perspective.aspect,
                             cobj->near, cobj->far);
            break;
        case PROJ_FRUSTUM:
            ortho_flag = 0;
            fn_800A3874(
                &mtx[0][0],
                cobj->projection_param.frustum.top,
                cobj->projection_param.frustum.bottom,
                cobj->projection_param.frustum.left,
                cobj->projection_param.frustum.right,
                cobj->near, cobj->far);
            break;
        case PROJ_ORTHO:
            ortho_flag = 1;
            fn_800A39E0(
                &mtx[0][0],
                cobj->projection_param.ortho.top,
                cobj->projection_param.ortho.bottom,
                cobj->projection_param.ortho.left,
                cobj->projection_param.ortho.right,
                cobj->near, cobj->far);
            break;
        }
        fn_800BD2E0(&mtx[0][0], ortho_flag);
        ok = 1;
        break;
    }
    case 0: {
        /* sub-screen path: scale viewport/scissor through the EFB dimensions */
        int has_off = (lbl_80466BC0[0x18] != 0);
        f64 sy = (f64)(u32) *(u16*)(lbl_80466BC0 + 6) / (f64)(u32) lbl_80478C54;
        f64 sx = (f64)(u32) *(u16*)(lbl_80466BC0 + 4) / (f64)(u32) lbl_80478C50;
        f32 vy = (f32)(cobj->viewport.ymin * sy);
        f32 vx = (f32)(cobj->viewport.xmin * sx);
        f32 xmax_sx = (f32)(cobj->viewport.xmax * sx);
        f32 ymax_sy = (f32)(cobj->viewport.ymax * sy);
        /* viewport.* are f32; scissor.* are u16 (lhz) */
        if (has_off) {
            ((void (*)(int, f32, f32, f32, f32, f32, f32)) lbl_80478C58)(
                1, vx, vy, xmax_sx - vx, ymax_sy - vy, lbl_8047D978, lbl_8047D9B0);
        } else {
            ((void (*)(int, f32, f32, f32, f32, f32, f32)) lbl_80478C58)(
                0, vx, vy, xmax_sx - vx, ymax_sy - vy, lbl_8047D978, lbl_8047D9B0);
        }
        {
            f32 sl = (f32)((f64)(u32) cobj->scissor.left   * sx);
            f32 st = (f32)((f64)(u32) cobj->scissor.top    * sy);
            f32 sr = (f32)((f64)(u32) cobj->scissor.right  * sx);
            f32 sb = (f32)((f64)(u32) cobj->scissor.bottom * sy);
            fn_800BD7A0(
                fn_800C46B0(sl),
                fn_800C46B0(st),
                fn_800C46B0(sr - sl),
                fn_800C46B0(sb - st));
        }
        switch (cobj->projection_type) {
        case PROJ_PERSPECTIVE:
            ortho_flag = 0;
            C_MTXPerspective(&mtx[0][0],
                             cobj->projection_param.perspective.fov,
                             cobj->projection_param.perspective.aspect,
                             cobj->near, cobj->far);
            break;
        case PROJ_FRUSTUM:
            ortho_flag = 0;
            fn_800A3874(
                &mtx[0][0],
                cobj->projection_param.frustum.top,
                cobj->projection_param.frustum.bottom,
                cobj->projection_param.frustum.left,
                cobj->projection_param.frustum.right,
                cobj->near, cobj->far);
            break;
        case PROJ_ORTHO:
            ortho_flag = 1;
            fn_800A39E0(
                &mtx[0][0],
                cobj->projection_param.ortho.top,
                cobj->projection_param.ortho.bottom,
                cobj->projection_param.ortho.left,
                cobj->projection_param.ortho.right,
                cobj->near, cobj->far);
            break;
        }
        fn_800BD2E0(&mtx[0][0], ortho_flag);
        ok = 1;
        break;
    }
    case 1:
        ok = fn_801963E0(cobj);
        break;
    case 2:
        ok = fn_801960C4(cobj);
        break;
    default:
        HSD_Panic(&lbl_8047D958, 0x2ab, lbl_80274680);
        return 0;
    }

    if (ok == 0) {
        return 0;
    }
    fn_80195F0C(cobj);
    return 1;
}
#endif
#pragma pop

/* 0x80195F0C | 0x1B8 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern int HSD_CObjGetUpVector(HSD_CObj*, f32*);   /* wrk8: real sig (was `void (void)`) */
extern void __assert(const char*, u32, const char*);
extern void fn_80191688(HSD_WObj*, void*);
extern char lbl_8047D958;
extern char lbl_8047D960;
#if 0
asm void fn_80195F0C(u8* ptr) {
#include "src/hsd/hsd_cobj_fn_80195F0C.inc"
}
#else
/* decompiled wrk5: functional (TU not byte-measurable) — recomputes the camera
   viewing matrix via C_MTXLookAt when it (or its eye/interest WObjs) is dirty.
   flags: 0x2 = up-to-date, 0x40000000 = force-recompute, 0x80000000 = updated. */
void fn_80195F0C(HSD_CObj* cobj) {
    f32 eye[3];
    f32 up[3];
    f32 interest[3];
    int update;
    int interest_dirty;

    if (cobj->flags & 0x2) {
        return;
    }
    interest_dirty = 1;
    update = 1;
    if (!(cobj->flags & 0x40000000)) {
        int eye_dirty = 0;
        if (cobj->eyepos != NULL && (cobj->eyepos->flags & 0x2)) {
            eye_dirty = 1;
        }
        if (eye_dirty) {
            interest_dirty = 1;
        } else {
            interest_dirty = 0;
            if (cobj->interest != NULL && (cobj->interest->flags & 0x2)) {
                interest_dirty = 1;
            }
        }
    }
    if (!interest_dirty) {
        update = 0;
    }
    if (!update) {
        return;
    }
    if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
    if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
    fn_80191688(cobj->eyepos, eye);
    if (HSD_CObjGetUpVector(cobj, up) == 0) {
        extern f32 lbl_8047D978;   /* 0.0f named SDA2 constant */
        extern f32 lbl_8047D9B0;   /* 1.0f named SDA2 constant */
        up[0] = lbl_8047D978;
        up[1] = lbl_8047D9B0;
        up[2] = lbl_8047D978;
    }
    if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
    if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
    fn_80191688(cobj->interest, interest);
    C_MTXLookAt(cobj->view_mtx, eye, up, interest);
    {
        HSD_WObj* w;
        w = cobj->eyepos;
        w->flags = w->flags & ~0x2;
        w = cobj->interest;
        w->flags = w->flags & ~0x2;
    }
    if (cobj != NULL) {
        cobj->flags = cobj->flags & ~0x40000000;
    }
    if (cobj != NULL) {
        cobj->flags |= 0x80000000;
    }
}
#endif
#pragma pop

/* 0x801960C4 | 0x31C */
#pragma push
#pragma optimization_level 3
#pragma optimizewithasm off
#if 0
asm void fn_801960C4(void) {
#include "src/hsd/hsd_cobj_fn_801960C4.inc"
}
#else
/* decompiled wrk7: functional (TU not byte-measurable) — near-twin of fn_801963E0
   that ALSO programs the GX scissor box. Reads the GX render-mode scan height
   (lbl_80466BC0 + 6, u16) and a guard band `vfilter = height - 8`; returns 0 if the
   viewport bottom is above the band. Clamps cobj->scissor (u16 left/right/top/
   bottom at 0x1C..0x22) to the band and commits it via fn_800BD7A0; clamps the
   viewport, invokes the viewport callback (lbl_80478C58, default fn_80196C54),
   computes the visible-height ratio, then builds and loads the projection matrix
   (1 perspective / 2 frustum via fn_800A3874, 3 ortho via fn_800A39E0, then
   fn_800BD2E0). Returns 1.
   SDA2 constants resolved by address (see NOTES): lbl_8047D998/lbl_8047D9C8 are
   doubles (half-fov / deg->rad), lbl_8047D9D4/lbl_8047D9D0 the perspective
   recenter terms, lbl_8047D978/lbl_8047D9B0 the callback constants. */
int fn_801960C4(HSD_CObj* cobj)
{
    extern u8 lbl_80466BC0[];   /* GX render-mode global; +6 = u16 scan/EFB height */
    extern u32 lbl_80478C58;    /* viewport/scissor callback (default fn_80196C54) */
    extern f32 lbl_8047D978;    /* callback const A */
    extern f32 lbl_8047D9B0;    /* callback const B */
    extern double lbl_8047D998; /* perspective half-fov scale */
    extern double lbl_8047D9C8; /* perspective deg->rad scale */
    extern f32 lbl_8047D9D4;    /* perspective ratio scale (D3) */
    extern f32 lbl_8047D9D0;    /* perspective recenter base (D0) */

    int vfilter;
    f32 fvfilter;
    f32 ratio;
    f32 mtx[3][4];
    int ortho_flag;

    vfilter = (int) *(u16*)(lbl_80466BC0 + 6) - 8;
    fvfilter = (f32)(u32) vfilter;
    if (cobj->viewport.ymax < fvfilter) {
        return 0;
    }

    /* ---- GX scissor box, clamped to the guard band ---- */
    {
        f32 sc_left = (f32)(u16) cobj->scissor.left;
        f32 sc_right = (f32)(u16) cobj->scissor.right;
        u16 clamped_top = (cobj->scissor.top <= (u16) vfilter)
                              ? (u16) vfilter : cobj->scissor.top;
        f32 top_off = (f32)(u32) ((int) clamped_top - vfilter);
        f32 bottom_off = (f32)(u32) ((int) cobj->scissor.bottom - vfilter);
        f32 sc_width = sc_right - sc_left;
        f32 sc_height = bottom_off - top_off;
        ((void (*)(int, int, int, int)) fn_800BD7A0)(
            ((int (*)(f32)) fn_800C46B0)(sc_left),
            ((int (*)(f32)) fn_800C46B0)(top_off),
            ((int (*)(f32)) fn_800C46B0)(sc_width),
            ((int (*)(f32)) fn_800C46B0)(sc_height));
    }

    /* ---- viewport rect + callback + visible-height ratio ---- */
    {
        f32 vp_xmin = cobj->viewport.xmin;
        f32 vp_width = cobj->viewport.xmax - cobj->viewport.xmin;
        f32 vp_ymax = cobj->viewport.ymax;
        f32 vp_ymin_clamped =
            (cobj->viewport.ymin <= fvfilter) ? fvfilter : cobj->viewport.ymin;
        f32 top_off = vp_ymin_clamped - fvfilter;
        f32 visible_h = vp_ymax - vp_ymin_clamped;
        ((void (*)(int, f32, f32, f32, f32, f32, f32)) lbl_80478C58)(
            0, vp_xmin, top_off, vp_width, visible_h, lbl_8047D978, lbl_8047D9B0);
        ratio = visible_h / (vp_ymax - cobj->viewport.ymin);
    }

    /* ---- build & load the projection matrix ---- */
    ortho_flag = 0;
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE: {
        f32 t = (f32) fn_800CE220(
                    (f32) (lbl_8047D9C8 *
                           (lbl_8047D998 * cobj->projection_param.perspective.fov)));
        f32 nt = cobj->near * t;
        f32 off = lbl_8047D9D4 * ratio + lbl_8047D9D0;
        f32 aspect = cobj->projection_param.perspective.aspect;
        ((void (*)(f32*, f32, f32, f32, f32, f32, f32)) fn_800A3874)(
            &mtx[0][0], nt * off, -nt, -(nt * aspect), nt * aspect,
            cobj->near, cobj->far);
        break;
    }
    case PROJ_FRUSTUM: {
        f32 top = cobj->projection_param.frustum.top;
        f32 bottom = cobj->projection_param.frustum.bottom;
        ((void (*)(f32*, f32, f32, f32, f32, f32, f32)) fn_800A3874)(
            &mtx[0][0], ratio * (top - bottom) + bottom, bottom,
            cobj->projection_param.frustum.left,
            cobj->projection_param.frustum.right, cobj->near, cobj->far);
        break;
    }
    case PROJ_ORTHO: {
        f32 top = cobj->projection_param.ortho.top;
        f32 bottom = cobj->projection_param.ortho.bottom;
        ortho_flag = 1;
        ((void (*)(f32*, f32, f32, f32, f32, f32, f32)) fn_800A39E0)(
            &mtx[0][0], ratio * (top - bottom) + bottom, bottom,
            cobj->projection_param.ortho.left,
            cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;
    }
    default:
        break;
    }

    ((void (*)(f32*, int)) fn_800BD2E0)(&mtx[0][0], ortho_flag);
    return 1;
}
#endif
#pragma pop

/* 0x801963E0 | 0x2B8 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_801963E0(void) {
#include "src/hsd/hsd_cobj_fn_801963E0.inc"
}
#else
/* decompiled wrk7: functional (TU not byte-measurable) — programs the camera's
   GX viewport and projection matrix for the frame (the CObj setup path).
   Returns 0 if the viewport is clipped past the EFB top (viewport.ymin >= EFB
   height); otherwise sets the GX viewport (fn_800BD7A0), invokes the viewport
   callback (lbl_80478C58, default fn_80196C54), builds the projection matrix on
   the stack per projection_type (1 perspective / 2 frustum via fn_800A3874,
   3 ortho via fn_800A39E0), loads it with fn_800BD2E0, and returns 1. `ratio`
   shrinks the frustum vertically to the visible fraction of a partially
   off-screen viewport. lbl_80466BC0 = active GX render-mode global (+6 = u16 EFB
   height). SDA2 constants resolved by address (see NOTES). */
int fn_801963E0(HSD_CObj* cobj)
{
    extern u8 lbl_80466BC0[];   /* GX render-mode global; +6 = u16 EFB height */
    extern u32 lbl_80478C58;    /* viewport/scissor callback (default fn_80196C54) */
    extern f32 lbl_8047D978;    /* callback const A */
    extern f32 lbl_8047D9B0;    /* const B (perspective recenter base) */
    extern double lbl_8047D998; /* perspective half-fov scale */
    extern double lbl_8047D9C8; /* perspective deg->rad scale */
    extern f32 lbl_8047D9D4;    /* perspective ratio scale (D3) */

    f32 efb_h;
    f32 clamped_ymax;
    f32 vheight;
    f32 vwidth;
    f32 ratio;
    f32 mtx[3][4];
    int ortho_flag;

    efb_h = (f32)(u16) *(u16*)(lbl_80466BC0 + 6);
    if (cobj->viewport.ymin >= efb_h) {
        return 0;
    }

    /* program the GX viewport box (float coords -> fixed via fn_800C46B0) */
    efb_h = (f32)(u16) *(u16*)(lbl_80466BC0 + 6);
    clamped_ymax = (cobj->viewport.ymax < efb_h) ? cobj->viewport.ymax : efb_h;
    vheight = clamped_ymax - cobj->viewport.ymin;
    vwidth = cobj->viewport.xmax - cobj->viewport.xmin;
    ((void (*)(int, int, int, int)) fn_800BD7A0)(
        ((int (*)(f32)) fn_800C46B0)(cobj->viewport.xmin),
        ((int (*)(f32)) fn_800C46B0)(cobj->viewport.ymin),
        ((int (*)(f32)) fn_800C46B0)(vwidth),
        ((int (*)(f32)) fn_800C46B0)(vheight));

    /* recompute clamp + visible-height ratio for the projection */
    efb_h = (f32)(u16) *(u16*)(lbl_80466BC0 + 6);
    clamped_ymax = (cobj->viewport.ymax < efb_h) ? cobj->viewport.ymax : efb_h;
    vheight = clamped_ymax - cobj->viewport.ymin;
    vwidth = cobj->viewport.xmax - cobj->viewport.xmin;
    ratio = vheight / (cobj->viewport.ymax - cobj->viewport.ymin);

    /* viewport/scissor offset callback */
    ((void (*)(int, f32, f32, f32, f32, f32, f32)) lbl_80478C58)(
        0, cobj->viewport.xmin, cobj->viewport.ymin, vwidth, vheight,
        lbl_8047D978, lbl_8047D9B0);

    ortho_flag = 0;
    switch (cobj->projection_type) {
    case PROJ_PERSPECTIVE: {
        f32 t = (f32) fn_800CE220(
                    (f32) (lbl_8047D9C8 *
                           (lbl_8047D998 * cobj->projection_param.perspective.fov)));
        f32 nt = cobj->near * t;
        f32 off = lbl_8047D9B0 - lbl_8047D9D4 * ratio;
        f32 aspect = cobj->projection_param.perspective.aspect;
        ((void (*)(f32*, f32, f32, f32, f32, f32, f32)) fn_800A3874)(
            &mtx[0][0], nt, nt * off, -(nt * aspect), nt * aspect,
            cobj->near, cobj->far);
        break;
    }
    case PROJ_FRUSTUM: {
        f32 top = cobj->projection_param.frustum.top;
        f32 bottom = cobj->projection_param.frustum.bottom;
        ((void (*)(f32*, f32, f32, f32, f32, f32, f32)) fn_800A3874)(
            &mtx[0][0], top, top - ratio * (top - bottom),
            cobj->projection_param.frustum.left,
            cobj->projection_param.frustum.right, cobj->near, cobj->far);
        break;
    }
    case PROJ_ORTHO: {
        f32 top = cobj->projection_param.ortho.top;
        f32 bottom = cobj->projection_param.ortho.bottom;
        ortho_flag = 1;
        ((void (*)(f32*, f32, f32, f32, f32, f32, f32)) fn_800A39E0)(
            &mtx[0][0], top, top - ratio * (top - bottom),
            cobj->projection_param.ortho.left,
            cobj->projection_param.ortho.right, cobj->near, cobj->far);
        break;
    }
    default:
        break;
    }

    ((void (*)(f32*, int)) fn_800BD2E0)(&mtx[0][0], ortho_flag);
    return 1;
}
#endif
#pragma pop

/* 0x80196698 | 0x64 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_801C29C4(void*, f32);
#if 1
void HSD_CObjReqAnim(HSD_CObj* cobj, f32 frame)
{
    if ((cobj = cobj) && cobj) {
        fn_801C29C4(cobj->aobj, frame);
        HSD_WObjReqAnim(cobj->eyepos, frame);
        HSD_WObjReqAnim(cobj->interest, frame);
    }
}
#else
void HSD_CObjReqAnim(void) { /* TODO */ }
#endif
#pragma pop

/* 0x801966FC | 0x50 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void HSD_CObjAnim(void) {
#include "src/hsd/hsd_cobj_HSD_CObjAnim.inc"
}
#else
void HSD_CObjAnim(HSD_CObj* cobj) {
    if (!cobj) return;
    fn_801C27F4(cobj->aobj, cobj, HSD_COBJ_METHOD(cobj)->update);
    HSD_WObjInterpretAnim(cobj->eyepos);
    HSD_WObjInterpretAnim(cobj->interest);
}
#endif
#pragma pop

/* 0x8019674C | 0x3C4 */
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void fn_8019674C(void) {
#include "src/hsd/hsd_cobj_fn_8019674C.inc"
}
#else
/* decompiled wrk7: functional (TU not byte-measurable) — the CObj `update` method
   installed by CObjInfoInit. Dispatches on an animation channel `type` (jumptable
   jumptable_8036C6E0, valid 0..0xC) and applies *(f32*)value to the matching
   camera property. Index->case mapping taken from the jumptable .data dump; bodies
   match the .inc faithfully. NOTE: interest channels 5/6/7 all overwrite component
   0 in the original asm (eye channels 1/2/3 write X/Y/Z) — reproduced verbatim,
   FLAGGED for review (likely an original-game copy/paste artefact). */
void fn_8019674C(HSD_CObj* cobj, u32 type, void* value)
{
    f32 v[3];

    if (cobj == NULL || type > 0xC) {
        return;
    }
    switch (type) {
    case 1: /* eye position, component X */
        if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        fn_80191688(cobj->eyepos, v);
        v[0] = *(f32*)value;
        if (cobj == NULL) __assert(&lbl_8047D958, 0x324, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        HSD_WObjSetPosition(cobj->eyepos, v);
        break;
    case 2: /* eye position, component Y */
        if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        fn_80191688(cobj->eyepos, v);
        v[1] = *(f32*)value;
        if (cobj == NULL) __assert(&lbl_8047D958, 0x324, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        HSD_WObjSetPosition(cobj->eyepos, v);
        break;
    case 3: /* eye position, component Z */
        if (cobj == NULL) __assert(&lbl_8047D958, 0x318, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        fn_80191688(cobj->eyepos, v);
        v[2] = *(f32*)value;
        if (cobj == NULL) __assert(&lbl_8047D958, 0x324, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
        HSD_WObjSetPosition(cobj->eyepos, v);
        break;
    case 5: /* interest position (asm writes component 0) */
        if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        fn_80191688(cobj->interest, v);
        v[0] = *(f32*)value;
        if (cobj == NULL) __assert(&lbl_8047D958, 0x30c, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        HSD_WObjSetPosition(cobj->interest, v);
        break;
    case 6: /* interest position (asm writes component 0) */
        if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        fn_80191688(cobj->interest, v);
        v[0] = *(f32*)value;
        if (cobj == NULL) __assert(&lbl_8047D958, 0x30c, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        HSD_WObjSetPosition(cobj->interest, v);
        break;
    case 7: /* interest position (asm writes component 0) */
        if (cobj == NULL) __assert(&lbl_8047D958, 0x300, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        fn_80191688(cobj->interest, v);
        v[0] = *(f32*)value;
        if (cobj == NULL) __assert(&lbl_8047D958, 0x30c, &lbl_8047D960);
        if (cobj == NULL) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
        HSD_WObjSetPosition(cobj->interest, v);
        break;
    case 9: /* roll */
        ((void (*)(HSD_CObj*, f32)) fn_801947C8)(cobj, *(f32*)value);
        break;
    case 10: /* field of view (perspective only) */
        if (cobj != NULL && cobj->projection_type == PROJ_PERSPECTIVE) {
            cobj->projection_param.perspective.fov = *(f32*)value;
        }
        break;
    case 11: /* near clip plane */
        if (cobj != NULL) {
            cobj->near = *(f32*)value;
        }
        break;
    case 12: /* far clip plane */
        if (cobj != NULL) {
            cobj->far = *(f32*)value;
        }
        break;
    default: /* 0, 4, 8: no-op */
        break;
    }
}
#endif
#pragma pop

/* 0x80196B10 | 0xA8 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern void fn_801C2670(void);
#if 0
asm void HSD_CObjAddAnim(void) {
#include "src/hsd/hsd_cobj_HSD_CObjAddAnim.inc"
}
#else
void HSD_CObjAddAnim(HSD_CObj* cobj_arg, HSD_CameraAnim* aobj_info_arg) {
    extern void fn_801C25E4(u32);
    extern u32 fn_801C2670(u32);
    extern void __assert(const char*, u32, const char*);
    extern char lbl_8047D958;
    extern char lbl_8047D960;
    u8* cobj = (u8*)cobj_arg;
    u8* aobj_info = (u8*)aobj_info_arg;
    if (cobj == NULL) return;
    if (aobj_info == NULL) return;
    if (*(volatile u32*)(cobj + 0x84) != 0) {
        fn_801C25E4(*(u32*)(cobj + 0x84));
    }
    *(u32*)(cobj + 0x84) = fn_801C2670(*(u32*)aobj_info);
    if (!cobj) __assert(&lbl_8047D958, 0x2e8, &lbl_8047D960);
    HSD_WObjAddAnim((HSD_WObj*)*(u32*)(cobj + 0x24),
                    (HSD_WObjAnim*)*(u32*)(aobj_info + 0x4));
    if (!cobj) __assert(&lbl_8047D958, 0x2d0, &lbl_8047D960);
    HSD_WObjAddAnim((HSD_WObj*)*(u32*)(cobj + 0x28),
                    (HSD_WObjAnim*)*(u32*)(aobj_info + 0x8));
}
#endif
#pragma pop

/* 0x80196BB8 | 0x84 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
#if 0
asm void HSD_CObjRemoveAnim(void) {
#include "src/hsd/hsd_cobj_HSD_CObjRemoveAnim.inc"
}
#else
void HSD_CObjRemoveAnim(HSD_CObj* cobj) {
    extern void fn_801C25E4(u32);
    extern void __assert(const char*, u32, const char*);
    extern char lbl_8047D958;
    extern char lbl_8047D960;
    u8* ptr = (u8*)cobj;
    if (!ptr) { return; }
    if (!ptr) { return; }
    fn_801C25E4(*(u32*)(ptr + 0x84));
    *(u32*)(ptr + 0x84) = 0;
    if (!ptr) { __assert(&lbl_8047D958, 0x2E8, &lbl_8047D960); }
    HSD_WObjRemoveAnim((HSD_WObj*)*(u32*)(ptr + 0x24));
    if (!ptr) { __assert(&lbl_8047D958, 0x2D0, &lbl_8047D960); }
    HSD_WObjRemoveAnim((HSD_WObj*)*(u32*)(ptr + 0x28));
}
#endif
#pragma pop

/* 0x80196C3C | 0x18 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80196C3C(void) {
#include "src/hsd/hsd_cobj_fn_80196C3C.inc"
}
#else
void fn_80196C3C(u8* ptr) {
    extern u32 lbl_80478C58;
    extern void fn_80196C54(void);
    if (ptr == NULL) ptr = (u8*)fn_80196C54;
    lbl_80478C58 = (u32)ptr;
}
#endif
#pragma pop

/* 0x80196C54 | 0x8C */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern void _savefpr_26(void);
extern void _restfpr_26(void);
extern void fn_800AA2F0(void);
extern void fn_800BD640(void);
extern void fn_800BD744(void);
#if 0
asm void fn_80196C54(void) {
#include "src/hsd/hsd_cobj_fn_80196C54.inc"
}
#else
void fn_80196C54(int flag, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) {
    extern void fn_800AA2F0(void);
    extern void fn_800BD640(f32, f32, f32, f32, f32, f32);
    extern void fn_800BD744(f32, f32, f32, f32, f32, f32);
    if (flag != 0) {
        fn_800AA2F0();
        fn_800BD640(a, b, c, d, e, f);
    } else {
        fn_800BD744(a, b, c, d, e, f);
    }
}
#endif
#pragma pop

/* 0x80196CE0 | 0x98 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_8009C1B4(void);
#if 1
asm void fn_80196CE0(void) {
#include "src/hsd/hsd_cobj_fn_80196CE0.inc"
}
#else
void fn_80196CE0(void) { /* TODO */ }
#endif
#pragma pop

/* 0x80196D78 | 0x98 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern void fn_800060F0(const char*, u32, const char*, ...);
extern void fn_80196CE0(void);
extern void OSReport(const char* fmt, ...);
extern char lbl_802746A0[];
extern char lbl_80465080[];
#if 0
asm void HSD_Panic(void) {
#include "src/hsd/hsd_cobj_HSD_Panic.inc"
}
#else
void HSD_Panic(const char* file, u32 line, const char* expr) {
    extern u32 lbl_8047B238;
    if (lbl_8047B238 != 0) {
        fn_80196CE0();
        OSReport(lbl_802746A0, expr, file, line);
        ((void(*)(char*, ...))lbl_8047B238)(lbl_80465080);
    }
    fn_800060F0(file, line, expr);
}
#endif
#pragma pop

/* 0x80196E10 | 0xA4 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern char lbl_802746B8[];
extern char lbl_8047D9D8;
#if 0
asm void __assert(const char*, u32, const char*) {
#include "src/hsd/hsd_cobj___assert.inc"
}
#else
void __assert(const char* file, u32 line, const char* expr) {
    extern u32 lbl_8047B238;
    OSReport(lbl_802746B8, expr);
    if (lbl_8047B238 != 0) {
        fn_80196CE0();
        OSReport(lbl_802746A0, &lbl_8047D9D8, file, line);
        ((void(*)(char*, ...))lbl_8047B238)(lbl_80465080);
    }
    fn_800060F0(file, line, &lbl_8047D9D8);
}
#endif
#pragma pop

/* 0x80196EB4 | 0x44 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80196EB4(void) {
#include "src/hsd/hsd_cobj_fn_80196EB4.inc"
}
#else
void fn_80196EB4(void) {
    extern u32 lbl_8047B24C;
    extern u32 lbl_8047B250;
    extern u32 lbl_8047B254;
    extern u32 lbl_8047B258;
    extern u32 lbl_8047B25C;
    extern u32 lbl_80478C64;
    extern u32 lbl_80478C68;
    extern u32 lbl_80478C6C;
    lbl_8047B24C = 0;
    lbl_80478C64 = (u32)&lbl_8047B24C;
    lbl_8047B250 = 0;
    lbl_80478C68 = (u32)&lbl_8047B250;
    lbl_8047B254 = 0;
    lbl_8047B258 = 0;
    lbl_80478C6C = (u32)&lbl_8047B258;
    lbl_8047B25C = 0;
}
#endif
#pragma pop

/* 0x80196EF8 | 0x424 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void _savefpr_27(void);
extern void _restfpr_27(void);
extern void fn_800B7874(void);
extern void fn_800B7D3C(void);
extern void fn_800B7D74(void);
extern void fn_800B857C(void);
extern void fn_800B884C(void);
extern void fn_800B928C(void);
extern void fn_800B94F0(void);
extern void fn_800BA6B0(void);
extern void fn_800BA6F4(void);
extern void fn_800BA9E4(void);
extern void fn_800BAFFC(void);
extern void fn_800BC114(void);
extern void fn_800BC618(void);
extern void fn_800BC66C(void);
extern void fn_800BC6F0(void);
extern void fn_800BC8C8(void);
extern void fn_800BCDDC(void);
extern void fn_800BCE30(void);
extern void fn_800BCE5C(void);
extern void fn_800BCE88(void);
extern void fn_800BCEBC(void);
extern void fn_800BD4B4(void);
extern void fn_800BD554(void);
extern void fn_801B25C4(void);
extern void HSD_StateInvalidate(void);
extern u8 lbl_8036C720[];
extern u8 lbl_8036CBC0[];
extern u8 lbl_80478C60;
#if 1
asm void fn_80196EF8(void) {
#include "src/hsd/hsd_cobj_fn_80196EF8.inc"
}
#else
void fn_80196EF8(void) { /* TODO */ }
#endif
#pragma pop

/* 0x8019731C | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8019731C(void) {
#include "src/hsd/hsd_cobj_fn_8019731C.inc"
}
#else
void fn_8019731C(u8 a, u8 b, u8 c, u8 d) {
    extern u8 lbl_80478C60;
    /* WALL: CW CSE collapses the repeated SDA base; target keeps r9/r8/r7 bases. */
    (&lbl_80478C60)[0] = a;
    (&lbl_80478C60)[1] = b;
    (&lbl_80478C60)[2] = c;
    (&lbl_80478C60)[3] = d;
}
#endif
#pragma pop

/* 0x8019733C | 0x8 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_8019733C(void) {
#include "src/hsd/hsd_cobj_fn_8019733C.inc"
}
#else
void fn_8019733C(u32 val) { extern u32 lbl_8047B240; lbl_8047B240 = val; }
#endif
#pragma pop

/* 0x801975FC | 0x54 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
extern u32 fn_80197650(u32, u32, u32);
#if 0
asm void fn_801975FC(void) {
#include "src/hsd/hsd_cobj_fn_801975FC.inc"
}
#else
void fn_801975FC(void) {
    extern s32 lbl_8047B248;
    extern u32 lbl_8047B250;
    extern u32 lbl_8047B254;
    extern u32 lbl_8047B258;
    extern u32 lbl_8047B25C;
    if (lbl_8047B248 == 0) return;
    lbl_8047B250 = fn_80197650(lbl_8047B250, lbl_8047B254, 0x3c);
    lbl_8047B258 = fn_80197650(lbl_8047B258, lbl_8047B25C, 0x40);
}
#endif
#pragma pop
