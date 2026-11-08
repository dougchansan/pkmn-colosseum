/**
 * @file wobj.c
 * @brief HAL sysdolphin world objects, 0x801914F4-0x80191ECC.
 *
 * Reunites the Count-Sheep campaign's matching WObj code with its class
 * info, default-class pointer and compiler-owned literal pools. The former
 * hsd_wobj.c also held two unrelated archive functions; those now belong
 * to archive.c. The mixed pool's preceding game constants stay with their
 * existing owner.
 *
 * Source order and class/animation interfaces were checked against
 * doldecomp/melee, src/sysdolphin/baselib/wobj.c at
 * 598987cf347c125c7416517af18bf5c1dc3f15a1. Colosseum's newer WObj class
 * supplies the animation callback through its update method. Its path
 * callback and assertion lines remain the Colosseum versions.
 *
 * Retains the matching GC/1.3 -O1 codegen and function order; readonly
 * strings restore the owner's pools.
 * The shared jobj.h supplies its own assertions; no separately owned
 * literal stand-ins or local copies of the JObj header helpers remain.
 */
#include "hsd/hsd_wobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_robj.h"
#include "hsd/hsd_spline.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"

extern HSD_ClassInfo* hsdSearchClassInfo_80193748(const char* class_name); /* hsdSearchClassInfo */
extern void* hsdNew_80193828(HSD_ClassInfo* info);             /* hsdNew */
extern void HSD_RObjRemoveAll_801AE50C(void* robj);                      /* HSD_RObjRemoveAll */
extern void HSD_RObjResolveRefsAll_801AEBE4(void* robj, void* desc);            /* HSD_RObjResolveRefsAll */
extern void HSD_RObjAnimAll_801B0040(void* robj);                      /* HSD_RObjAnimAll */
extern void HSD_RObjReqAnimAll_801AFEFC(void* robj, f32 frame);            /* HSD_RObjReqAnimAll */
extern void HSD_RObjRemoveAnimAll_801AFFE0(void* robj);                      /* HSD_RObjRemoveAnimAll */

void WObjInfoInit(void);
int WObjLoad(HSD_WObj* wobj, HSD_WObjDesc* desc);
void WObjAmnesia(HSD_ClassInfo* info);
void WObjRelease(HSD_WObj* wobj);
void WObjUpdateFunc(void* obj, u32 type, void* value);

HSD_WObjInfo lbl_8036C5F0 = { WObjInfoInit }; /* hsdWObj */
HSD_ClassInfo* lbl_8047B218;                /* default_class */

#define WOBJ_USE_ANIM_POS 0x1u
#define WOBJ_POS_DIRTY 0x2u

/* Bake the animated position into pos and drop the "animated" flag, so that
   callers always read a plain world-space vector. jobj_ is the caller's
   scratch HSD_JObj*, shared across every use in a function. */
#define WOBJ_RESOLVE_ANIM_POSITION(wobj_, jobj_)                            \
    do {                                                                    \
        if (((wobj_)->flags & WOBJ_USE_ANIM_POS) != 0) {                    \
            if ((wobj_)->aobj != NULL) {                                    \
                if ((wobj_)->aobj->hsd_obj != NULL) {                       \
                    (jobj_) = (HSD_JObj*) (wobj_)->aobj->hsd_obj;           \
                    if ((jobj_) != NULL) {                                  \
                        if (HSD_JObjMtxIsDirty(jobj_)) {                    \
                            HSD_JObjSetupMatrixSub(jobj_);                  \
                        }                                                   \
                    }                                                       \
                    PSMTXMultVec((jobj_)->mtx, &(wobj_)->pos, &(wobj_)->pos);\
                }                                                           \
            }                                                               \
            (wobj_)->flags &= 0xFFFFFFFE;                                   \
        }                                                                   \
    } while (0)

/* Mirrors HSD_WObjSetPositionX/Y/Z: evaluate the new component, then (for a
   non-NULL wobj) bake any animated position and store the component. */
#define WOBJ_SET_POSITION_COMPONENT(wobj_, jobj_, field_, val_)             \
    do {                                                                    \
        f32 component_ = (val_);                                            \
        if ((wobj_) != NULL) {                                              \
            WOBJ_RESOLVE_ANIM_POSITION(wobj_, jobj_);                       \
            (wobj_)->pos.field_ = component_;                               \
            (wobj_)->flags |= WOBJ_POS_DIRTY;                               \
        }                                                                   \
    } while (0)

void WObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&lbl_8036C5F0),
                     HSD_CLASS_INFO(&hsdObj), "sysdolphin_base_library", "had_wobj",
                     sizeof(HSD_WObjInfo), sizeof(HSD_WObj));
    HSD_CLASS_INFO(&lbl_8036C5F0)->release = (void (*)(HSD_Class*)) WObjRelease;
    HSD_CLASS_INFO(&lbl_8036C5F0)->amnesia = WObjAmnesia;
    HSD_WOBJ_INFO(&lbl_8036C5F0)->load = WObjLoad;
    HSD_WOBJ_INFO(&lbl_8036C5F0)->update =
        (void (*)(HSD_WObj*, u32, void*)) WObjUpdateFunc;
}

void WObjAmnesia(HSD_ClassInfo* info)
{
    if (info == lbl_8047B218) {
        lbl_8047B218 = NULL;
    }
    HSD_OBJECT_PARENT_INFO(&lbl_8036C5F0)->amnesia(info);
}

void WObjRelease(HSD_WObj* wobj) {
    HSD_RObjRemoveAll_801AE50C(wobj->robj);
    HSD_AObjRemove(wobj->aobj);
    HSD_OBJECT_PARENT_INFO(&lbl_8036C5F0)->release((HSD_Class*) wobj);
}

HSD_WObj* HSD_WObjAlloc(void)
{
    HSD_WObj* wobj;

    if ((wobj = (HSD_WObj*) hsdNew_80193828(
        lbl_8047B218 != NULL
            ? lbl_8047B218
            : HSD_CLASS_INFO(&lbl_8036C5F0))) == NULL)
    {
        __assert(__FILE__, 0x257, "wobj");
    }
    return wobj;
}

void HSD_WObjGetPosition(HSD_WObj* wobj, Vec* position)
{
    HSD_JObj* jobj;

    if (wobj == NULL || position == NULL) {
        return;
    }
    if ((wobj->flags & WOBJ_USE_ANIM_POS) != 0) {
        if (wobj->aobj != NULL && wobj->aobj->hsd_obj != NULL) {
            HSD_JObjSetupMatrix(jobj = (HSD_JObj*) wobj->aobj->hsd_obj);
            PSMTXMultVec(jobj->mtx, &wobj->pos, &wobj->pos);
        }
        wobj->flags &= 0xFFFFFFFE;
    }
    *position = wobj->pos;
}

void HSD_WObjSetPosition(HSD_WObj* wobj, Vec* position)
{
    if (wobj == NULL || position == NULL) {
        return;
    }
    wobj->pos = *position;
    wobj->flags = wobj->flags | WOBJ_POS_DIRTY;
    wobj->flags = wobj->flags & 0xFFFFFFFE;
}

HSD_WObj* HSD_WObjLoadDesc(HSD_WObjDesc* desc)
{

    if (desc != NULL) {
        HSD_WObj* wobj;
        HSD_ClassInfo* info;

        if (desc->class_name == NULL ||
            !(info = hsdSearchClassInfo_80193748(desc->class_name)))
        {
            /* HSD_WObjAlloc() inlined */
            wobj = (HSD_WObj*) hsdNew_80193828(
                lbl_8047B218 ? lbl_8047B218 : HSD_CLASS_INFO(&lbl_8036C5F0));
            if (wobj == NULL) {
                __assert(__FILE__, 0x257, "wobj");
            }
        } else {
            wobj = (HSD_WObj*) hsdNew_80193828(info);
            if (wobj == NULL) {
                __assert(__FILE__, 0x104, "wobj");
            }
        }
        HSD_WOBJ_METHOD(wobj)->load(wobj, desc);
        return wobj;
    }
    return NULL;
}

void HSD_WObjInit(HSD_WObj* wobj, HSD_WObjDesc* desc)
{
    if (wobj == NULL || desc == NULL) {
        return;
    }

    HSD_WObjSetPosition(wobj, &desc->pos);
    if (wobj->robj != NULL) {
        HSD_RObjRemoveAll_801AE50C(wobj->robj);
    }
    wobj->robj = (HSD_RObj*) HSD_RObjLoadDesc(desc->robjdesc);
    HSD_RObjResolveRefsAll_801AEBE4(wobj->robj, desc->robjdesc);
}

int WObjLoad(HSD_WObj* wobj, HSD_WObjDesc* desc)
{
    HSD_WObjSetPosition(wobj, &desc->pos);
    if (wobj->robj != NULL) {
        HSD_RObjRemoveAll_801AE50C(wobj->robj);
    }
    wobj->robj = (HSD_RObj*) HSD_RObjLoadDesc(desc->robjdesc);
    HSD_RObjResolveRefsAll_801AEBE4(wobj->robj, desc->robjdesc);
    return 0;
}

void HSD_WObjInterpretAnim(HSD_WObj* wobj) {
    if (wobj != NULL) {
        HSD_AObjInterpretAnim(wobj->aobj, wobj,
                              (HSD_ObjUpdateFunc) HSD_WOBJ_METHOD(wobj)->update);
        HSD_RObjAnimAll_801B0040(wobj->robj);
    }
}

void WObjUpdateFunc(void* obj, u32 type, void* value)
{
    HSD_WObj* wobj;
    Vec position;
    HSD_JObj* jobj;
    f32* fval;

    wobj = (HSD_WObj*) obj;
    fval = (f32*) value;

    if (wobj == NULL) {
        return;
    }

    switch (type) {
    case HSD_A_J_PATH:
        if (*fval < 0.0) {
            *fval = 0.0f;
        }
        if (1.0 < *fval) {
            *fval = 1.0f;
        }
        if (wobj->aobj == NULL) {
            __assert(__FILE__, 0x98, "wobj->aobj");
        }
        jobj = (HSD_JObj*) wobj->aobj->hsd_obj;
        if (jobj == NULL) {
            __assert(__FILE__, 0x9A, "jp");
        }
        if (jobj->u.spline == NULL) {
            __assert(__FILE__, 0x9B, "jp->u.spline");
        }
        splArcLengthPoint(&position, jobj->u.spline, *fval);
        HSD_WObjSetPosition(wobj, &position);
        wobj->flags |= WOBJ_USE_ANIM_POS;
        break;
    case HSD_A_J_TRAX:
        WOBJ_SET_POSITION_COMPONENT(wobj, jobj, x, *fval);
        break;
    case HSD_A_J_TRAY:
        WOBJ_SET_POSITION_COMPONENT(wobj, jobj, y, *fval);
        break;
    case HSD_A_J_TRAZ:
        WOBJ_SET_POSITION_COMPONENT(wobj, jobj, z, *fval);
        break;
    }
}

void HSD_WObjAddAnim(HSD_WObj* wobj, HSD_WObjAnim* desc) {
    if (wobj == NULL) {
        return;
    }
    if (desc == NULL) {
        return;
    }
    if (wobj->aobj != NULL) {
        HSD_AObjRemove(wobj->aobj);
    }
    wobj->aobj = HSD_AObjLoadDesc(desc->aobjdesc);
    HSD_RObjAddAnimAll(wobj->robj, desc->robjanim);
}

void HSD_WObjReqAnim(HSD_WObj* wobj, f32 frame) {
    if (wobj != NULL) {
        HSD_AObjReqAnim(wobj->aobj, frame);
        HSD_RObjReqAnimAll_801AFEFC(wobj->robj, frame);
    }
}

void HSD_WObjRemoveAnim(HSD_WObj* wobj) {
    if (wobj != NULL) {
        HSD_AObjRemove(wobj->aobj);
        wobj->aobj = NULL;
        HSD_RObjRemoveAnimAll_801AFFE0(wobj->robj);
    }
}
