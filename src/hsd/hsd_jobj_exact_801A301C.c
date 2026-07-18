/**
 * @file hsd_jobj_exact_801A301C.c
 * @brief Exact pure-C HSD JObj tail, 0x801A301C - 0x801A4000.
 */

#include "dolphin/mtx.h"

#define iref_DEC hsd_inline_iref_DEC
#define ref_INC hsd_inline_ref_INC
#include "hsd/hsd_jobj.h"
#undef iref_DEC
#undef ref_INC
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_id.h"
#include "hsd/hsd_mtx.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_robj.h"

extern u8 lbl_80465588[];
extern u8 lbl_804655B4[];
extern char lbl_8047DB20;
extern char lbl_8047DB34;
extern char lbl_8047DB3C;

extern void fn_8019D9DC(HSD_JObj*);
extern void fn_8019D620(HSD_JObj*);

static inline void JObjSetFlagsInline(HSD_JObj* jobj, u32 flags)
{
    s32 result;

    if (jobj == NULL) {
        return;
    }
    if (((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) && jobj != NULL) {
        if (jobj == NULL) {
            __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
        }
        result = 0;
        if (!(jobj->flags & JOBJ_USER_DEF_MTX)) {
            if (jobj->flags & JOBJ_MTX_DIRTY) {
                result = 1;
            }
        }
        if (result == 0) {
            fn_8019D620(jobj);
        }
    }
    jobj->flags |= flags;
}

static inline void JObjClearFlagsInline(HSD_JObj* jobj, u32 flags)
{
    s32 result;

    if (jobj == NULL) {
        return;
    }
    if (((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) && jobj != NULL) {
        if (jobj == NULL) {
            __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
        }
        result = 0;
        if (!(jobj->flags & JOBJ_USER_DEF_MTX)) {
            if (jobj->flags & JOBJ_MTX_DIRTY) {
                result = 1;
            }
        }
        if (result == 0) {
            fn_8019D620(jobj);
        }
    }
    jobj->flags &= ~flags;
}

/* This helper is expanded at both call sites in fn_801A3600. */
static inline void JObjSetupIfDirty(HSD_JObj* jobj)
{
    s32 dirty;

    if (jobj != NULL) {
        if (jobj == NULL) {
            __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
        }
        dirty = 0;
        if (!(jobj->flags & JOBJ_USER_DEF_MTX) &&
            (jobj->flags & JOBJ_MTX_DIRTY))
        {
            dirty = 1;
        }
        if (dirty != 0) {
            fn_8019D9DC(jobj);
        }
    }
}

#pragma push
#pragma optimization_level 1
void fn_801A301C(HSD_JObj* jobj, HSD_AnimJoint* animjoint,
                 HSD_MatAnimJoint* matanimjoint,
                 HSD_ShapeAnimJoint* shapeanimjoint)
{
    if (jobj == NULL) {
        return;
    }

    if (animjoint != NULL) {
        HSD_FObj* fobj;
        HSD_FObj* volatile* link;
        HSD_AObj* aobj;

        if (jobj->aobj != NULL) {
            HSD_AObjRemove(jobj->aobj);
        }
        jobj->aobj = HSD_AObjLoadDesc(animjoint->aobjdesc);
        aobj = jobj->aobj;
        if (aobj != NULL && aobj->fobj != NULL) {
            link = &aobj->fobj;
            while (*link != NULL) {
                if ((*link)->obj_type == HSD_A_J_BRANCH) {
                    HSD_FObj* next = (*link)->next;
                    fobj = *link;

                    *link = next;
                    fobj->next = aobj->fobj;
                    aobj->fobj = fobj;
                    break;
                }
                link = &(*link)->next;
            }
        }
        fn_801AFE68(jobj->robj, animjoint->robj_anim);

        if (animjoint->flags & 1) {
            JObjSetFlagsInline(jobj, JOBJ_CLASSICAL_SCALE);
        } else {
            JObjClearFlagsInline(jobj, JOBJ_CLASSICAL_SCALE);
        }
    }

    if (union_type_dobj(jobj)) {
        HSD_DObjAddAnimAll(
            jobj->u.dobj,
            matanimjoint != NULL ? matanimjoint->matanim : NULL,
            shapeanimjoint != NULL ? shapeanimjoint->shapeanimdobj : NULL);
    }
}
#pragma pop

void fn_801A323C(HSD_AObj* aobj)
{
    HSD_FObj* volatile* link;

    if (aobj == NULL || aobj->fobj == NULL) {
        return;
    }
    link = &aobj->fobj;
    while (*link != NULL) {
        if ((*link)->obj_type == HSD_A_J_BRANCH) {
            HSD_FObj* next = (*link)->next;
            HSD_FObj* fobj = *link;

            *link = next;
            fobj->next = aobj->fobj;
            aobj->fobj = fobj;
            return;
        }
        link = &(*link)->next;
    }
}

#pragma push
#pragma optimization_level 1
#pragma use_lmw_stmw on
#pragma inline_depth(5)
#pragma inline_max_size(10000)
void fn_801A32A0(HSD_JObj* jobj, u32 flags, f32 frame);
void fn_801A3574(HSD_JObj* jobj, u32 flags, f32 frame);

static inline void JObjReqAnimByFlagsInline(HSD_JObj* jobj, u32 flags,
                                            f32 frame)
{
    extern void fn_801AFF64(HSD_RObj* robj, f32 frame, u32 flags);

    if (jobj != NULL) {
        if (flags & 1) {
            HSD_AObjReqAnim(jobj->aobj, frame);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAllByFlags(jobj->u.dobj, frame, (void*) flags);
        }
        fn_801AFF64(jobj->robj, frame, flags);
    }
}

static inline void JObjReqAnimAllByFlagsLevel5(HSD_JObj* jobj, u32 flags,
                                               f32 frame)
{
    if (jobj != NULL) {
        HSD_JObj* child;

        fn_801A3574(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                fn_801A32A0(child, flags, frame);
                child = child->next;
            }
        }
    }
}

static inline void JObjReqAnimAllByFlagsLevel4(HSD_JObj* jobj, u32 flags,
                                               f32 frame)
{
    if (jobj != NULL) {
        HSD_JObj* child;

        JObjReqAnimByFlagsInline(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                JObjReqAnimAllByFlagsLevel5(child, flags, frame);
                child = child->next;
            }
        }
    }
}

static inline void JObjReqAnimAllByFlagsLevel3(HSD_JObj* jobj, u32 flags,
                                               f32 frame)
{
    if (jobj != NULL) {
        HSD_JObj* child;

        JObjReqAnimByFlagsInline(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                JObjReqAnimAllByFlagsLevel4(child, flags, frame);
                child = child->next;
            }
        }
    }
}

static inline void JObjReqAnimAllByFlagsLevel2(HSD_JObj* jobj, u32 flags,
                                               f32 frame)
{
    if (jobj != NULL) {
        HSD_JObj* child;

        JObjReqAnimByFlagsInline(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                JObjReqAnimAllByFlagsLevel3(child, flags, frame);
                child = child->next;
            }
        }
    }
}

static inline void JObjReqAnimAllByFlagsLevel1(HSD_JObj* jobj, u32 flags,
                                               f32 frame)
{
    if (jobj != NULL) {
        HSD_JObj* child;

        JObjReqAnimByFlagsInline(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                JObjReqAnimAllByFlagsLevel2(child, flags, frame);
                child = child->next;
            }
        }
    }
}

void fn_801A32A0(HSD_JObj* jobj, u32 flags, f32 frame)
{
    if (jobj != NULL) {
        JObjReqAnimByFlagsInline(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            HSD_JObj* child = jobj->child;

            while (child != NULL) {
                JObjReqAnimAllByFlagsLevel1(child, flags, frame);
                child = child->next;
            }
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
void fn_801A3574(HSD_JObj* jobj, u32 flags, f32 frame)
{
    extern void fn_801AFF64(HSD_RObj* robj, f32 frame, u32 flags);

    if (jobj != NULL) {
        if (flags & 1) {
            HSD_AObjReqAnim(jobj->aobj, frame);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAllByFlags(jobj->u.dobj, frame, (void*) flags);
        }
        fn_801AFF64(jobj->robj, frame, flags);
    }
}
#pragma pop

typedef struct JObjQuat {
    f32 x, y, z, w;
} JObjQuat;

extern void HSD_MtxSRT(f32 mtx[3][4], Vec* scale, Vec* rotate,
                       Vec* translate, Vec* parent_scale);
extern void HSD_MtxSRTQuat(f32 mtx[3][4], Vec* scale, JObjQuat* rotate,
                           Vec* translate, Vec* parent_scale);

static inline BOOL JObjParentHasScale(HSD_JObj* jobj)
{
    BOOL result = FALSE;

    if (((volatile HSD_JObj*) jobj)->parent != NULL &&
        ((volatile HSD_JObj*) jobj)->parent->scl != NULL)
    {
        result = TRUE;
    }
    return result;
}

#pragma push
#pragma optimization_level 1
void fn_801A3600(HSD_JObj* jobj)
{
    JObjSetupIfDirty(jobj->parent);
    if (jobj->flags & JOBJ_CLASSICAL_SCALE) {
        if (jobj->parent != NULL && jobj->parent->scl != NULL) {
            if (jobj->scl == NULL) {
                jobj->scl = HSD_VecAlloc();
            }
            *(Vec*) jobj->scl = *(Vec*) jobj->parent->scl;
        } else {
            if (jobj->scl != NULL) {
                HSD_VecFree(jobj->scl);
                jobj->scl = NULL;
            }
        }
    } else {
        if (jobj->scl == NULL) {
            jobj->scl = HSD_VecAlloc();
        }
        if (jobj->parent != NULL && jobj->parent->scl != NULL) {
            jobj->scl[0] = jobj->scale_x * jobj->parent->scl[0];
            jobj->scl[1] = jobj->scale_y * jobj->parent->scl[1];
            jobj->scl[2] = jobj->scale_z * jobj->parent->scl[2];
        } else {
            *(Vec*) jobj->scl = *(Vec*) &jobj->scale_x;
        }
    }

    if (jobj->flags & JOBJ_USE_QUATERNION) {
        f32 (*mtx)[4] = jobj->mtx;
        Vec* scale = (Vec*) &jobj->scale_x;
        JObjQuat* rotate = (JObjQuat*) &jobj->rotate_x;
        Vec* translate = (Vec*) &jobj->translate_x;
        Vec* parent_scale;

        if (JObjParentHasScale(jobj)) {
            parent_scale = (Vec*) jobj->parent->scl;
        } else {
            parent_scale = NULL;
        }
        HSD_MtxSRTQuat(mtx, scale, rotate, translate, parent_scale);
    } else {
        f32 (*mtx)[4] = jobj->mtx;
        Vec* scale = (Vec*) &jobj->scale_x;
        Vec* rotate = (Vec*) &jobj->rotate_x;
        Vec* translate = (Vec*) &jobj->translate_x;
        Vec* parent_scale;

        if (JObjParentHasScale(jobj)) {
            parent_scale = (Vec*) jobj->parent->scl;
        } else {
            parent_scale = NULL;
        }
        HSD_MtxSRT(mtx, scale, rotate, translate, parent_scale);
    }
    if (jobj->parent != NULL) {
        PSMTXConcat(jobj->parent->mtx, jobj->mtx, jobj->mtx);
    }
    if (jobj->aobj != NULL && jobj->aobj->hsd_obj != NULL) {
        Vec position;
        HSD_JObj* aobj_jobj = (HSD_JObj*) jobj->aobj->hsd_obj;

        JObjSetupIfDirty(aobj_jobj);
        PSMTXMultVec(aobj_jobj->mtx, (Vec*) &jobj->translate_x, &position);
        jobj->mtx[0][3] = position.x;
        jobj->mtx[1][3] = position.y;
        jobj->mtx[2][3] = position.z;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
#pragma use_lmw_stmw on
#pragma inline_depth(5)
typedef void (*HSD_JObjWalkTreeCallback)(HSD_JObj* jobj, void* user_data,
                                         s32 type);

void HSD_JObjWalkTree0(HSD_JObj* jobj, HSD_JObjWalkTreeCallback callback,
                       void* user_data);

#pragma push
#pragma optimization_level 1
void fn_801A3918(HSD_JObj* jobj, HSD_JObjWalkTreeCallback callback,
                 void* user_data)
{
    HSD_JObj* child;
    if (jobj == NULL) {
        return;
    }
    if (callback != NULL) {
        callback(jobj, user_data, 0);
    }
    if (jobj->flags & JOBJ_INSTANCE) {
        return;
    }
    for (child = jobj->child; child != NULL; child = child->next) {
        HSD_JObjWalkTree0(child, callback, user_data);
    }
}
#pragma pop

extern char lbl_80274D44[];

void HSD_JObjWalkTree0(HSD_JObj* jobj, HSD_JObjWalkTreeCallback callback,
                       void* user_data)
{
    HSD_JObj* child;
    s32 type;

    if (jobj == NULL) {
        return;
    }
    if (jobj->parent == NULL) {
        __assert(&lbl_8047DB20, 0xAD, lbl_80274D44);
    }
    type = (jobj->parent->child == jobj) ? 1 : 2;
    if (callback != NULL) {
        callback(jobj, user_data, type);
    }
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        child = jobj->child;
        while (child != NULL) {
            HSD_JObjWalkTree0(child, callback, user_data);
            child = child->next;
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
void fn_801A3D04(HSD_JObj* jobj)
{
    HSD_JObj* parent;
    BOOL result;

    if (jobj == NULL) {
        return;
    }
    if (jobj == NULL) {
        __assert(&lbl_8047DB34, 0x25D, &lbl_8047DB3C);
    }
    result = FALSE;
    if (!(((volatile HSD_JObj*) jobj)->flags & JOBJ_USER_DEF_MTX) &&
        (((volatile HSD_JObj*) jobj)->flags & JOBJ_MTX_DIRTY))
    {
        result = TRUE;
    }
    switch (result) {
    case 0:
        if (((volatile HSD_JObj*) jobj)->flags & JOBJ_USER_DEF_MTX) {
            if (!(((volatile HSD_JObj*) jobj)->flags &
                  JOBJ_MTX_INDEP_PARENT) &&
                ((volatile HSD_JObj*) jobj)->parent != NULL)
            {
                parent = ((volatile HSD_JObj*) jobj)->parent;
                if (parent == NULL) {
                    __assert(&lbl_8047DB34, 0x25D, &lbl_8047DB3C);
                }
                result = FALSE;
                if (!(((volatile HSD_JObj*) parent)->flags &
                      JOBJ_USER_DEF_MTX) &&
                    (((volatile HSD_JObj*) parent)->flags & JOBJ_MTX_DIRTY))
                {
                    result = TRUE;
                }
                if (result) {
                    jobj->flags = jobj->flags | JOBJ_MTX_DIRTY;
                }
            }
        } else if ((((volatile HSD_JObj*) jobj)->parent != NULL &&
                    (((volatile HSD_JObj*) jobj)->parent->flags &
                     JOBJ_MTX_DIRTY)) ||
                   (((volatile HSD_JObj*) jobj)->flags & JOBJ_EFFECTOR) ==
                       JOBJ_JOINT1 ||
                   (((volatile HSD_JObj*) jobj)->flags & JOBJ_EFFECTOR) ==
                       JOBJ_JOINT2 ||
                   (((volatile HSD_JObj*) jobj)->flags & JOBJ_EFFECTOR) ==
                       JOBJ_EFFECTOR ||
                   ((volatile HSD_JObj*) jobj)->robj != NULL)
        {
            jobj->flags = jobj->flags | JOBJ_MTX_DIRTY;
        }
        break;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 1
extern void HSD_ObjFree(void* list, void* data);

HSD_SList* fn_801A3E64(HSD_SList* node)
{
    HSD_SList* next;

    if (node != NULL) {
        next = node->next;
        HSD_ObjFree(lbl_804655B4, node);
        return next;
    }
    return NULL;
}
#pragma pop

extern void* HSD_ObjAlloc(void* list);
extern void* memset(void* dst, int c, u32 n);
extern char lbl_8047DBA0;
extern char lbl_8047DBA8;
extern char lbl_8047DBB0;
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);

HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data)
{
    HSD_SList* prev = (HSD_SList*) HSD_ObjAlloc(&lbl_804655B4);

    if (prev == NULL) {
        __assert(&lbl_8047DBA0, 0x4C, &lbl_8047DBB0);
    }
    memset(prev, 0, sizeof(HSD_SList));
    prev->data = data;
    if (prev == NULL) {
        __assert(&lbl_8047DBA0, 0xCA, &lbl_8047DBA8);
    }
    prev->next = next;
    return prev;
}

HSD_SList* fn_801A3F48(void)
{
    HSD_SList* list = (HSD_SList*) HSD_ObjAlloc(&lbl_804655B4);
    if (list == NULL) {
        __assert(&lbl_8047DBA0, 0x4C, &lbl_8047DBB0);
    }
    memset(list, 0, sizeof(HSD_SList));
    return list;
}

void* HSD_DListGetAllocData(void)
{
    return lbl_80465588;
}

void* HSD_SListGetAllocData(void)
{
    return lbl_804655B4;
}

void fn_801A3FBC(void)
{
    HSD_ObjAllocInit(lbl_804655B4, sizeof(HSD_SList), 4);
    HSD_ObjAllocInit(lbl_80465588, 0xC, 4);
}
