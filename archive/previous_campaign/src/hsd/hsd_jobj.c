/**
 * @file hsd_jobj.c
 * @brief HSD JObj - Joint object implementation (skeletal hierarchy).
 *
 * Colosseum address: 0x8019CE50 (HSD_JObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/jobj.c
 *
 * This is one of the largest and most critical HSD modules.
 * JObj manages the skeletal hierarchy that drives all 3D models.
 */

#include "hsd/hsd_jobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_robj.h"

static void JObjInfoInit(void);

HSD_JObjInfo hsdJObj = { JObjInfoInit };

static HSD_ClassInfo* default_class = NULL;
static HSD_JObj* current_jobj = NULL;

/* ========================================================================= */
/*  Current JObj tracking                                                    */
/* ========================================================================= */

void HSD_JObjSetCurrent(HSD_JObj* jobj)
{
    current_jobj = jobj;
}

HSD_JObj* HSD_JObjGetCurrent(void)
{
    return current_jobj;
}

/* ========================================================================= */
/*  Flag accessors                                                           */
/* ========================================================================= */

u32 HSD_JObjGetFlags(HSD_JObj* jobj)
{
    HSD_ASSERT(0, jobj);
    return jobj->flags;
}

void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        jobj->flags |= flags;
    }
}

void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        HSD_JObjSetFlags(jobj, flags);
        HSD_JObjSetFlagsAll(jobj->child, flags);
        HSD_JObjSetFlagsAll(jobj->next, flags);
    }
}

void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        jobj->flags &= ~flags;
    }
}

void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj != NULL) {
        HSD_JObjClearFlags(jobj, flags);
        HSD_JObjClearFlagsAll(jobj->child, flags);
        HSD_JObjClearFlagsAll(jobj->next, flags);
    }
}

/* ========================================================================= */
/*  DObj accessor                                                            */
/* ========================================================================= */

HSD_DObj* HSD_JObjGetDObj(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return NULL;
    }
    if (!union_type_dobj(jobj)) {
        return NULL;
    }
    return jobj->u.dobj;
}

/* ========================================================================= */
/*  Matrix dirty management                                                  */
/* ========================================================================= */

void HSD_JObjSetMtxDirtySub(HSD_JObj* jobj)
{
    HSD_JObj* child;

    if (jobj == NULL) {
        return;
    }
    jobj->flags |= JOBJ_MTX_DIRTY;
    child = jobj->child;
    while (child != NULL) {
        if (!(child->flags & JOBJ_MTX_INDEP_PARENT)) {
            HSD_JObjSetMtxDirtySub(child);
        }
        child = child->next;
    }
}

/* ========================================================================= */
/*  Hierarchy manipulation                                                   */
/* ========================================================================= */

void HSD_JObjAddChild(HSD_JObj* jobj, HSD_JObj* child)
{
    HSD_JObj* c;

    HSD_ASSERT(0, jobj);
    HSD_ASSERT(0, child);
    HSD_ASSERT(0, child->parent == NULL);

    child->parent = jobj;
    if (jobj->child == NULL) {
        jobj->child = child;
    } else {
        c = jobj->child;
        while (c->next != NULL) {
            c = c->next;
        }
        c->next = child;
    }
    HSD_JObjSetMtxDirty(child);
}

void HSD_JObjAddNext(HSD_JObj* jobj, HSD_JObj* next)
{
    HSD_ASSERT(0, jobj);
    HSD_ASSERT(0, next);

    next->parent = jobj->parent;
    next->next = jobj->next;
    jobj->next = next;
    HSD_JObjSetMtxDirty(next);
}

void HSD_JObjAddDObj(HSD_JObj* jobj, HSD_DObj* dobj)
{
    HSD_DObj* d;

    HSD_ASSERT(0, jobj);
    HSD_ASSERT(0, union_type_dobj(jobj));

    if (jobj->u.dobj == NULL) {
        jobj->u.dobj = dobj;
    } else {
        d = jobj->u.dobj;
        while (d->next != NULL) {
            d = d->next;
        }
        d->next = dobj;
    }
}

/* ========================================================================= */
/*  Reference counting                                                       */
/* ========================================================================= */

void HSD_JObjUnref(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    if (ref_DEC(jobj) != 0) {
        if (jobj != NULL) {
            ((HSD_ClassInfo*)jobj->object.parent.class_info)->release((HSD_Class*) jobj);
            ((HSD_ClassInfo*)jobj->object.parent.class_info)->destroy((HSD_Class*) jobj);
        }
    }
}

/* ========================================================================= */
/*  Remove                                                                   */
/* ========================================================================= */

HSD_JObj* HSD_JObjRemove(HSD_JObj* jobj)
{
    HSD_JObj* next;

    if (jobj == NULL) {
        return NULL;
    }

    next = jobj->next;

    /* Unlink from parent's child list */
    if (jobj->parent != NULL) {
        HSD_JObj* p = jobj->parent;
        if (p->child == jobj) {
            p->child = jobj->next;
        } else {
            HSD_JObj* c = p->child;
            while (c != NULL && c->next != jobj) {
                c = c->next;
            }
            if (c != NULL) {
                c->next = jobj->next;
            }
        }
    }

    jobj->parent = NULL;
    jobj->next = NULL;
    HSD_JObjUnref(jobj);

    return next;
}

void HSD_JObjRemoveAll(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    HSD_JObjRemoveAll(jobj->child);
    HSD_JObjRemoveAll(jobj->next);
    jobj->child = NULL;
    jobj->next = NULL;
    jobj->parent = NULL;
    HSD_JObjUnref(jobj);
}

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_JObjRemoveAnim(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    HSD_AObjRemove(jobj->aobj);
    jobj->aobj = NULL;
    if (union_type_dobj(jobj)) {
        /* Remove DObj animations would go here */
    }
    HSD_RObjRemoveAnimAll(jobj->robj);
}

void HSD_JObjRemoveAnimAll(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    HSD_JObjRemoveAnim(jobj);
    HSD_JObjRemoveAnimAll(jobj->child);
    HSD_JObjRemoveAnimAll(jobj->next);
}

void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame)
{
    if (jobj == NULL) {
        return;
    }
    HSD_AObjReqAnim(jobj->aobj, frame);
    HSD_RObjReqAnimAll(jobj->robj, frame);
    if (union_type_dobj(jobj)) {
        HSD_DObjReqAnimAll(jobj->u.dobj, frame);
    }
    HSD_JObjReqAnimAll(jobj->child, frame);
    HSD_JObjReqAnimAll(jobj->next, frame);
}

void HSD_JObjAddAnimAll(HSD_JObj* jobj, HSD_AnimJoint* animjoint,
                        HSD_MatAnimJoint* matanimjoint,
                        HSD_ShapeAnimJoint* shapeanimjoint)
{
    if (jobj == NULL) {
        return;
    }

    if (animjoint != NULL) {
        if (jobj->aobj != NULL) {
            HSD_AObjRemove(jobj->aobj);
        }
        jobj->aobj = HSD_AObjLoadDesc(animjoint->aobjdesc);
        HSD_RObjAddAnimAll(jobj->robj, animjoint->robj_anim);
    }

    if (union_type_dobj(jobj)) {
        HSD_DObjAddAnimAll(jobj->u.dobj,
            matanimjoint ? matanimjoint->matanim : NULL,
            shapeanimjoint ? shapeanimjoint->shapeanimdobj : NULL);
    }

    HSD_JObjAddAnimAll(jobj->child,
        animjoint ? animjoint->child : NULL,
        matanimjoint ? matanimjoint->child : NULL,
        shapeanimjoint ? shapeanimjoint->child : NULL);

    HSD_JObjAddAnimAll(jobj->next,
        animjoint ? animjoint->next : NULL,
        matanimjoint ? matanimjoint->next : NULL,
        shapeanimjoint ? shapeanimjoint->next : NULL);
}

void HSD_JObjAnim(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    /* Interpret AObj animation -> update JObj transform */
    HSD_RObjAnimAll(jobj->robj);
    if (union_type_dobj(jobj)) {
        HSD_DObjAnim(jobj->u.dobj);
    }
}

void HSD_JObjAnimAll(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    HSD_JObjAnim(jobj);
    HSD_JObjAnimAll(jobj->child);
    HSD_JObjAnimAll(jobj->next);
}

/* ========================================================================= */
/*  Alloc / Load                                                             */
/* ========================================================================= */

HSD_JObj* HSD_JObjAlloc(void)
{
    HSD_JObj* jobj;
    jobj = (HSD_JObj*) hsdNew(
        default_class ? default_class : &hsdJObj.parent.parent);
    HSD_ASSERT(0, jobj);
    return jobj;
}

void HSD_JObjSetDefaultClass(HSD_ClassInfo* info)
{
    if (info) {
        HSD_ASSERT(0, hsdIsDescendantOf(info, &hsdJObj));
    }
    default_class = info;
}

/* ========================================================================= */
/*  Class lifecycle                                                          */
/* ========================================================================= */

static void JObjRelease(HSD_Class* o)
{
    HSD_JObj* jobj = (HSD_JObj*) o;

    /* Release children */
    HSD_JObjRemoveAll(jobj->child);
    jobj->child = NULL;

    /* Release DObj chain */
    if (union_type_dobj(jobj)) {
        HSD_DObjRemoveAll(jobj->u.dobj);
        jobj->u.dobj = NULL;
    }

    /* Release animation */
    HSD_AObjRemove(jobj->aobj);
    jobj->aobj = NULL;

    /* Release constraints */
    HSD_RObjRemoveAll(jobj->robj);
    jobj->robj = NULL;

    HSD_OBJECT_PARENT_INFO(&hsdJObj)->release(o);
}

static void JObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    current_jobj = NULL;
    HSD_OBJECT_PARENT_INFO(&hsdJObj)->amnesia(info);
}

static void JObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdJObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_jobj",
                     sizeof(HSD_JObjInfo), sizeof(HSD_JObj));
    HSD_CLASS_INFO(&hsdJObj)->release = JObjRelease;
    HSD_CLASS_INFO(&hsdJObj)->amnesia = JObjAmnesia;
}

/* ===================================================================
 * Generated: 0 pattern-matched + 28 stubs
 * Range: 0x8019CE50 - 0x801A1988
 * =================================================================== */

/* 0x8019CF54 | 0x68 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u32 lbl_8047B298;
extern u8 lbl_8036C8E0[];
extern u32 lbl_8047B29C;
extern u32 lbl_8047B2AC;
#if 0
asm void fn_8019CF54(void) {
#include "src/hsd/hsd_jobj_fn_8019CF54.inc"
}
#else
#pragma optimization_level 1
void fn_8019CF54(void* info) {
    if (info == (void*)lbl_8047B298) {
        lbl_8047B298 = 0;
    }
    if (info == (void*)lbl_8036C8E0) {
        lbl_8047B29C = 0;
        lbl_8047B2AC = 0;
    }
    ((HSD_ClassInfo*)lbl_8036C8E0)->head.parent->amnesia(info);
}
#endif
#pragma pop

/* 0x8019CFBC | 0xA0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void* HSD_IDGetDataFromTable(void* table, u32 key, u32* found);
extern void fn_8019C1B0(void* table, u32 key);
extern void HSD_VecFree(void* data);
extern void HSD_MtxFree(void* data);
#if 0
asm void fn_8019CFBC(void) {
#include "src/hsd/hsd_jobj_fn_8019CFBC.inc"
}
#else
#pragma optimization_level 4
void fn_8019CFBC(HSD_JObj* jobj) {
    HSD_JOBJ_METHOD(jobj)->release_child(jobj);

    if (HSD_IDGetDataFromTable(NULL, jobj->id, NULL) == jobj) {
        fn_8019C1B0(NULL, jobj->id);
    }
    if (((volatile HSD_JObj*) jobj)->scl != NULL) {
        HSD_VecFree(jobj->scl);
    }
    if (((volatile HSD_JObj*) jobj)->envelopemtx != NULL) {
        HSD_MtxFree(jobj->envelopemtx);
    }
    ((HSD_ClassInfo*) lbl_8036C8E0)->head.parent->release((HSD_Class*) jobj);
}
#endif
#pragma pop

/* 0x8019D05C | 0x544 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void __assert();
extern void HSD_Panic(void* file, u32 line, void* msg);
extern u8 lbl_80274AA0[];
extern char lbl_8047DB20;
extern char lbl_8047DB28;
typedef struct JObjVec {
    f32 x;
    f32 y;
    f32 z;
} JObjVec;

extern f32 sqrtf(f32 x);
extern f64 fn_800CE298(f64 x);
extern f32 fn_800A3B7C(JObjVec* a, JObjVec* b);
extern void fn_800A3AC0(JObjVec* src, JObjVec* dst, f32 scale);
extern void fn_800A3A78(JObjVec* a, JObjVec* b, JObjVec* dst);
extern void fn_800A3A9C(JObjVec* a, JObjVec* b, JObjVec* dst);
extern void fn_800A3B9C(JObjVec* a, JObjVec* b, JObjVec* dst);
extern void fn_800A3244(f32 mtx[3][4], JObjVec* axis, f32 angle);
extern void fn_800A37CC(f32 mtx[3][4], JObjVec* src, JObjVec* dst);
extern HSD_RObj* fn_801B00E0(HSD_RObj* robj, u32 type, u32 subtype);
extern void fn_801AED88(HSD_RObj* robj, HSD_JObj* jobj,
                        HSD_ObjUpdateFunc update_func);
extern s32 fn_801AFCAC(HSD_RObj* robj, u32 type, JObjVec* out);
extern void HSD_MtxGetTranslate(f32 mtx[3][4], JObjVec* out);

static void JObjVec_Set(JObjVec* vec, f32 x, f32 y, f32 z)
{
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

static void JObjVec_SetOne(JObjVec* vec)
{
    JObjVec_Set(vec, 1.0f, 1.0f, 1.0f);
}

static void JObjVec_SetZero(JObjVec* vec)
{
    JObjVec_Set(vec, 0.0f, 0.0f, 0.0f);
}

static void JObjVec_LoadTranslate(HSD_JObj* jobj, JObjVec* vec)
{
    vec->x = jobj->translate_x;
    vec->y = jobj->translate_y;
    vec->z = jobj->translate_z;
}

static void JObjVec_LoadScl(HSD_JObj* jobj, JObjVec* vec)
{
    if (jobj != NULL && jobj->scl != NULL) {
        vec->x = jobj->scl[0];
        vec->y = jobj->scl[1];
        vec->z = jobj->scl[2];
    }
}

static void JObjMtx_LoadColumn(HSD_JObj* jobj, s32 column, JObjVec* vec)
{
    vec->x = jobj->mtx[0][column];
    vec->y = jobj->mtx[1][column];
    vec->z = jobj->mtx[2][column];
}

static void JObjMtx_StoreScaledColumn(HSD_JObj* jobj, s32 column,
                                      JObjVec* vec, f32 scale)
{
    jobj->mtx[0][column] = vec->x * scale;
    jobj->mtx[1][column] = vec->y * scale;
    jobj->mtx[2][column] = vec->z * scale;
}

static void JObjMtx_StoreTranslation(HSD_JObj* jobj, JObjVec* vec)
{
    jobj->mtx[0][3] = vec->x;
    jobj->mtx[1][3] = vec->y;
    jobj->mtx[2][3] = vec->z;
}

static f32 JObj_InvSqrt(f32 value)
{
    if (value <= 0.0f) {
        return 0.0f;
    }
    return sqrtf(1.0f / value);
}

static void JObjVec_Normalize(JObjVec* src, JObjVec* dst)
{
    f32 dot;
    f32 scale;

    dot = fn_800A3B7C(src, src);
    scale = JObj_InvSqrt(1.0e-10f + dot);
    fn_800A3AC0(src, dst, scale);
}

static HSD_JObj* JObj_FindEffectType(HSD_JObj* jobj, u32 type)
{
    while (jobj != NULL) {
        if ((jobj->flags & JOBJ_JOINT) == type) {
            return jobj;
        }
        jobj = jobj->next;
    }
    return NULL;
}

static HSD_JObj* JObj_GetEffectorChecked(HSD_JObj* jobj)
{
    HSD_JObj* effector;

    effector = JObj_FindEffectType(jobj, JOBJ_EFFECTOR);
    if (effector == NULL) {
        __assert(&lbl_8047DB20, 0x82D, &lbl_8047DB28);
        return NULL;
    }
    if (fn_801B00E0(effector->robj, REFTYPE_JOBJ, 1) == NULL) {
        return NULL;
    }
    return effector;
}

static HSD_JObj* JObj_GetPrev(HSD_JObj* jobj)
{
    HSD_JObj* cur;

    if (jobj == NULL || jobj->parent == NULL) {
        return NULL;
    }
    if (jobj->parent->child == jobj) {
        return NULL;
    }
    cur = jobj->parent->child;
    while (cur != NULL) {
        if (cur->next == jobj) {
            return cur;
        }
        cur = cur->next;
    }
    HSD_Panic(&lbl_8047DB20, 0x5F8, &lbl_8047DB28);
    return NULL;
}

static void JObj_RecalcParentRootBits(HSD_JObj* jobj)
{
    HSD_JObj* child;
    u32 flags;

    while (jobj != NULL) {
        child = jobj->child;
        flags = ~JOBJ_ROOT_MASK;
        while (child != NULL) {
            flags |= (child->flags | (child->flags << 10)) & JOBJ_ROOT_MASK;
            child = child->next;
        }
        if ((jobj->flags & ~flags) == 0) {
            break;
        }
        jobj->flags &= flags;
        jobj = jobj->next;
    }
}

static void JObj_DetachFromParent(HSD_JObj* jobj)
{
    HSD_JObj* prev;
    HSD_JObj* next;
    HSD_JObj* parent;

    if (jobj == NULL || jobj->parent == NULL) {
        return;
    }

    parent = jobj->parent;
    next = jobj->next;
    if (parent->child == jobj) {
        parent->child = next;
    } else {
        prev = JObj_GetPrev(jobj);
        if (prev != NULL) {
            prev->next = next;
        } else {
            __assert(&lbl_8047DB20, 0x57B, &lbl_8047DB28);
        }
    }
    JObj_RecalcParentRootBits(parent);
    jobj->parent = NULL;
    jobj->next = NULL;
}

static BOOL JObj_IsDObjVisible(HSD_JObj* jobj)
{
    if (jobj->flags & JOBJ_HIDDEN) {
        return FALSE;
    }
    return union_type_dobj(jobj);
}

void fn_8019F1C4(HSD_JObj* jobj, s32* total_a, s32* total_b);

#if 0
asm void fn_8019D05C(void) {
#include "src/hsd/hsd_jobj_fn_8019D05C.inc"
}
#else
void fn_8019D05C(HSD_JObj* jobj) {
    /* decompiled cdx7: functional */
    HSD_JObj* child;

    child = jobj->child;
    if (child != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            HSD_JObjUnref(child);
        } else {
            child->parent = NULL;
            HSD_JObjRemoveAll(child);
        }
        jobj->child = NULL;
    }

    if (jobj->parent != NULL) {
        JObj_DetachFromParent(jobj);
    }

    if (union_type_dobj(jobj) && jobj->u.dobj != NULL) {
        HSD_DObjRemoveAll(jobj->u.dobj);
        jobj->u.dobj = NULL;
    }

    if (jobj->robj != NULL) {
        HSD_RObjRemoveAll(jobj->robj);
        jobj->robj = NULL;
    }

    if (jobj->aobj != NULL) {
        HSD_AObjRemove(jobj->aobj);
        jobj->aobj = NULL;
    }
}
#endif
#pragma pop

/* 0x8019D5A0 | 0x70 */
#pragma push
extern u32 lbl_8047DB30;
#if 1
asm void fn_8019D5A0(void) {
#include "src/hsd/hsd_jobj_fn_8019D5A0.inc"
}
#else
s32 fn_8019D5A0(HSD_JObj* jobj)
{
    if ((s32) ((HSD_ClassInfo*) lbl_8036C8E0)->head.parent->init((HSD_Class*) jobj) >= 0) {
        jobj->flags = JOBJ_MTX_DIRTY;
        jobj->scale_x = *(f32*) &lbl_8047DB30;
        jobj->scale_y = *(f32*) &lbl_8047DB30;
        jobj->scale_z = *(f32*) &lbl_8047DB30;
        return 0;
    }
}
#endif
#pragma pop

/* 0x8019DD00 | 0x760 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u8 lbl_80274AC4[];
extern u32 lbl_8047DB30;
extern char lbl_8047DB68;
extern u32 lbl_8047DB44;
extern u32 lbl_8047DB48;
extern u32 lbl_8047DB50;
extern u32 lbl_8047DB58;
extern u32 lbl_8047DB60;
extern u8 lbl_80478AC0[];
extern char lbl_8047DB6C;
extern u32 lbl_8047DB74;
extern u32 lbl_8047DB78;
#if 0
asm void fn_8019DD00(void) {
#include "src/hsd/hsd_jobj_fn_8019DD00.inc"
}
#else
void fn_8019DD00(HSD_JObj* jobj) {
    /* decompiled cdx7: functional */
    HSD_JObj* effector;
    HSD_JObj* parent;
    HSD_RObj* hint;
    HSD_RObj* min_limit;
    HSD_RObj* max_limit;
    JObjVec scale;
    JObjVec parent_pos;
    JObjVec parent_x;
    JObjVec parent_z;
    JObjVec joint_pos;
    JObjVec target_dir;
    JObjVec bend_axis;
    JObjVec side_axis;
    f32 rot_mtx[3][4];
    f32 x_scale;
    f32 dot;
    f32 angle;
    s32 clamped;
    s32 flip;

    effector = JObj_GetEffectorChecked(jobj->child);
    if (effector == NULL || jobj->parent == NULL) {
        return;
    }

    JObjVec_SetOne(&scale);
    JObjVec_LoadScl(jobj, &scale);
    parent = jobj->parent;
    HSD_MtxGetTranslate(parent->mtx, &parent_pos);
    JObjMtx_LoadColumn(parent, 0, &parent_x);
    JObjVec_Normalize(&parent_x, &parent_x);

    x_scale = 1.0f;
    if (parent->scl != NULL) {
        x_scale = parent->scl[0];
    }

    hint = fn_801B00E0(parent->robj, REFTYPE_IKHINT, 0);
    if (hint == NULL) {
        __assert(&lbl_8047DB20, 0x905, &lbl_8047DB6C);
        return;
    }

    fn_800A3AC0(&parent_x, &parent_x, hint->u.ik_hint.bone_length * x_scale);
    fn_800A3A78(&parent_pos, &parent_x, &joint_pos);
    JObjVec_LoadTranslate(effector, &target_dir);
    fn_800A3A9C(&target_dir, &joint_pos, &target_dir);
    JObjVec_Normalize(&target_dir, &target_dir);

    min_limit = fn_801B00E0(jobj->robj, REFTYPE_LIMIT, 5);
    max_limit = fn_801B00E0(jobj->robj, REFTYPE_LIMIT, 6);
    if (min_limit != NULL || max_limit != NULL) {
        clamped = 0;
        hint = fn_801B00E0(jobj->robj, REFTYPE_IKHINT, 0);
        if (hint == NULL) {
            __assert(&lbl_8047DB20, 0x927, &lbl_8047DB6C);
            return;
        }
        flip = (hint->flags & 4) != 0;
        JObjMtx_LoadColumn(parent, 0, &parent_x);
        JObjVec_Normalize(&parent_x, &parent_x);
        dot = fn_800A3B7C(&parent_x, &target_dir);
        if (dot >= 1.0f) {
            angle = 0.0f;
        } else if (dot <= -1.0f) {
            angle = 3.1415927f;
        } else {
            angle = (f32) fn_800CE298(dot);
        }
        if (!flip) {
            angle = -angle;
        }
        if (min_limit != NULL && angle < min_limit->u.limit) {
            angle = min_limit->u.limit;
            clamped = 1;
        } else if (max_limit != NULL && max_limit->u.limit < angle) {
            angle = max_limit->u.limit;
            clamped = 1;
        }
        if (clamped != 0) {
            JObjMtx_LoadColumn(parent, 2, &parent_z);
            fn_800A3244(rot_mtx, &parent_z, angle);
            fn_800A37CC(rot_mtx, &parent_x, &target_dir);
        }
    }

    JObjMtx_LoadColumn(parent, 2, &parent_z);
    fn_800A3B9C(&parent_z, &target_dir, &bend_axis);
    JObjVec_Normalize(&bend_axis, &bend_axis);
    fn_800A3B9C(&target_dir, &bend_axis, &side_axis);

    JObjMtx_StoreScaledColumn(jobj, 0, &target_dir, scale.x);
    JObjMtx_StoreScaledColumn(jobj, 1, &bend_axis, scale.y);
    JObjMtx_StoreScaledColumn(jobj, 2, &side_axis, scale.z);
    JObjMtx_StoreTranslation(jobj, &joint_pos);
}
#endif
#pragma pop

/* 0x8019E460 | 0xBBC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u32 lbl_8047DB48;
extern char lbl_8047DB34;
extern char lbl_8047DB3C;
extern u32 lbl_8047DB7C;
extern u32 lbl_8047DB44;
extern u32 lbl_8047DB30;
extern u32 lbl_8047DB50;
extern u32 lbl_8047DB58;
extern u32 lbl_8047DB60;
extern u32 lbl_8047DB84;
extern u32 lbl_8047DB80;
#if 0
asm void fn_8019E460(void) {
#include "src/hsd/hsd_jobj_fn_8019E460.inc"
}
#else
void fn_8019E460(HSD_JObj* jobj) {
    /* decompiled cdx7: functional */
    HSD_JObj* joint2;
    HSD_JObj* effector;
    HSD_JObj* parent;
    HSD_RObj* hint;
    JObjVec scale;
    JObjVec origin;
    JObjVec target;
    JObjVec target_dir;
    JObjVec bend_axis;
    JObjVec normal_axis;
    JObjVec pole;
    JObjVec pole_hint;
    JObjVec tmp;
    JObjVec column;
    f32 rot_mtx[3][4];
    f32 rotate_x;
    f32 first_len;
    f32 second_len;
    f32 dist_sq;
    f32 first_sq;
    f32 second_sq;
    f32 diff_sq;
    f32 height_sq;
    f32 axial_sq;
    f32 axial_len;
    f32 height_len;
    f32 norm_scale;
    s32 flip;
    HSD_ObjUpdateFunc update_func;

    joint2 = JObj_FindEffectType(jobj->child, JOBJ_JOINT2);
    JObjVec_SetOne(&scale);
    JObjVec_SetZero(&origin);
    JObjVec_LoadScl(jobj, &scale);

    hint = fn_801B00E0(jobj->robj, REFTYPE_IKHINT, 0);
    if (hint == NULL) {
        __assert(&lbl_8047DB20, 0x85C, &lbl_8047DB3C);
        return;
    }
    rotate_x = hint->u.ik_hint.rotate_x;
    first_len = hint->u.ik_hint.bone_length * scale.x;
    second_len = 0.0f;
    flip = 0;

    if (joint2 != NULL) {
        hint = fn_801B00E0(joint2->robj, REFTYPE_IKHINT, 0);
        if (hint == NULL) {
            __assert(&lbl_8047DB20, 0x867, &lbl_8047DB3C);
            return;
        }
        second_len = hint->u.ik_hint.bone_length * joint2->scale_x * scale.x;
        flip = (hint->flags & 4) != 0;
        effector = JObj_GetEffectorChecked(joint2->child);
    } else {
        effector = JObj_GetEffectorChecked(jobj->child);
    }

    if (effector == NULL) {
        return;
    }

    if (fn_801B00E0(jobj->robj, REFTYPE_JOBJ, 3) == NULL &&
        jobj->robj != NULL)
    {
        update_func =
            *(HSD_ObjUpdateFunc*) ((u8*) HSD_JOBJ_METHOD(jobj) + 0x50);
        fn_801AED88(jobj->robj, jobj, update_func);
        if (HSD_JObjMtxIsDirty(jobj)) {
            HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
            jobj->flags &= ~JOBJ_MTX_DIRTY;
        }
    }

    parent = jobj->parent;
    if (parent != NULL) {
        HSD_MtxGetTranslate(parent->mtx, &origin);
    }

    fn_801AFCAC(effector->robj, 1, &target);
    effector->translate_x = target.x;
    effector->translate_y = target.y;
    effector->translate_z = target.z;
    fn_800A3A9C(&target, &origin, &target_dir);
    dist_sq = fn_800A3B7C(&target_dir, &target_dir);
    first_sq = first_len * first_len;
    second_sq = second_len * second_len;
    height_sq = 0.0f;
    axial_len = 0.0f;

    if (dist_sq > 1.0e-8f) {
        target = target_dir;
        if (fn_801AFCAC(jobj->robj, 3, &pole_hint) != 0) {
            fn_800A3A9C(&pole_hint, &origin, &pole_hint);
            if (rotate_x != 0.0f) {
                fn_800A3244(rot_mtx, &target, rotate_x);
                fn_800A37CC(rot_mtx, &pole_hint, &pole_hint);
            }
            fn_800A3B9C(&target, &pole_hint, &normal_axis);
            fn_800A3B9C(&normal_axis, &target, &pole_hint);
        } else {
            JObjMtx_LoadColumn(jobj, 2, &normal_axis);
            fn_800A3B9C(&normal_axis, &target, &pole_hint);
            fn_800A3B9C(&target, &pole_hint, &normal_axis);
        }

        JObjVec_Normalize(&normal_axis, &normal_axis);
        JObjVec_Normalize(&pole_hint, &bend_axis);
        diff_sq = first_sq - second_sq;
        height_sq =
            0.25f *
            (((2.0f * (first_sq + second_sq)) - dist_sq) -
             ((diff_sq * diff_sq) / dist_sq));
        if (height_sq < 0.0f) {
            height_sq = 0.0f;
        }
        axial_sq = (first_sq - height_sq) / dist_sq;
        axial_len = axial_sq * JObj_InvSqrt(1.0e-10f + axial_sq);
        height_len = height_sq * JObj_InvSqrt(1.0e-10f + height_sq);
    } else {
        height_len = first_len;
    }

    if (flip != 0) {
        height_len = -height_len;
    }
    if ((second_sq - height_sq) < dist_sq) {
        fn_800A3AC0(&target_dir, &tmp, axial_len);
    } else {
        fn_800A3AC0(&target_dir, &tmp, -axial_len);
    }
    fn_800A3AC0(&bend_axis, &pole, height_len);
    fn_800A3A78(&tmp, &pole, &tmp);
    norm_scale = JObj_InvSqrt(1.0e-10f + fn_800A3B7C(&tmp, &tmp));
    fn_800A3AC0(&tmp, &tmp, norm_scale);

    JObjMtx_StoreScaledColumn(jobj, 0, &tmp, scale.x);
    fn_800A3B9C(&normal_axis, &tmp, &column);
    JObjMtx_StoreScaledColumn(jobj, 1, &column, scale.y);
    JObjMtx_StoreScaledColumn(jobj, 2, &normal_axis, scale.z);
    JObjMtx_StoreTranslation(jobj, &origin);
}
#endif
#pragma pop

/* 0x8019F01C | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u32 lbl_8047B2AC;
#if 0
asm void fn_8019F01C(void) {
#include "src/hsd/hsd_jobj_fn_8019F01C.inc"
}
#else
u32 fn_8019F01C(void) {
    return lbl_8047B2AC;
}
#endif
#pragma pop

/* 0x8019F1C4 | 0x554 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_DObjCountVertices(HSD_DObj* dobj, s32* total_a, s32* total_b);
#if 0
asm void fn_8019F1C4(void) {
#include "src/hsd/hsd_jobj_fn_8019F1C4.inc"
}
#else
void fn_8019F1C4(HSD_JObj* jobj, s32* total_a, s32* total_b) {
    /* decompiled cdx7: functional */
    HSD_JObj* child;
    s32 sum_a;
    s32 sum_b;
    s32 child_a;
    s32 child_b;

    sum_a = 0;
    sum_b = 0;
    if (jobj != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            fn_8019F1C4(jobj->child, &sum_a, &sum_b);
        } else {
            if (JObj_IsDObjVisible(jobj)) {
                HSD_DObjCountVertices(jobj->u.dobj, &sum_a, &sum_b);
            }
            child = jobj->child;
            while (child != NULL) {
                child_a = 0;
                child_b = 0;
                fn_8019F1C4(child, &child_a, &child_b);
                sum_a += child_a;
                sum_b += child_b;
                child = child->next;
            }
        }
    }

    if (total_a != NULL) {
        *total_a = sum_a;
    }
    if (total_b != NULL) {
        *total_b = sum_b;
    }
}
#endif
#pragma pop

/* 0x8019F778 | 0x78 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_8019D620(HSD_JObj*);
#if 0
asm void fn_8019F778(void) {
#include "src/hsd/hsd_jobj_fn_8019F778.inc"
}
#else
#pragma optimization_level 1
void fn_8019F778(HSD_JObj* jobj) {
    extern void __assert();
    extern char lbl_8047DB34;
    extern char lbl_8047DB3C;
    s32 result;
    if (!jobj) return;
    if (!jobj) __assert(&lbl_8047DB34, 0x25d, &lbl_8047DB3C);
    result = 0;
    if (!(jobj->flags & 0x00800000)) {
        if (jobj->flags & 0x00000040) {
            result = 1;
        }
    }
    switch (result) {
    case 0:
        fn_8019D620(jobj);
        break;
    }
}
#endif
#pragma pop

/* 0x8019FF30 | 0x18 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8019FF30(void) {
#include "src/hsd/hsd_jobj_fn_8019FF30.inc"
}
#else
#pragma optimization_level 4
u32 fn_8019FF30(HSD_JObj* jobj) {
    if (jobj != NULL) {
        return jobj->flags;
    }
    return 0;
}
#endif
#pragma pop

/* 0x8019FF48 | 0x2C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8019FF48(void) {
#include "src/hsd/hsd_jobj_fn_8019FF48.inc"
}
#else
#pragma optimization_level 4
HSD_DObj* fn_8019FF48(HSD_JObj* jobj) {
    if (jobj == NULL) {
        goto end;
    }
    if (union_type_dobj(jobj)) {
        goto ok;
    }
end:
    return NULL;
ok:
    return jobj->u.dobj;
}
#endif
#pragma pop

/* 0x8019FF74 | 0x1E8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u8 lbl_80274B28[];
#if 0
asm void fn_8019FF74(void) {
#include "src/hsd/hsd_jobj_fn_8019FF74.inc"
}
#else
#pragma optimization_level 4
void fn_8019FF74(HSD_JObj* jobj, HSD_JObj* next) {
    /* decompiled cdx5: functional (non-byte-exact) */
    HSD_JObj* old_next;
    HSD_JObj* prev;
    HSD_JObj* scan;
    HSD_JObj* parent;
    u32 root_flags;
    u32 keep_flags;

    if (jobj == NULL || next == NULL) {
        return;
    }

    if (next != NULL) {
        if (next->parent != NULL) {
            old_next = next->next;
            if (next->parent->child == next) {
                next->parent->child = old_next;
            } else {
                prev = NULL;
                if (next != NULL && next->parent != NULL &&
                    next->parent->child != next)
                {
                    prev = next->parent->child;
                    while (prev != NULL) {
                        if (prev->next == next) {
                            break;
                        }
                        prev = prev->next;
                    }
                    if (prev == NULL) {
                        HSD_Panic(&lbl_8047DB20, 0x5F8, lbl_80274B28);
                    }
                }

                if (prev == NULL) {
                    __assert(&lbl_8047DB20, 0x57B, &lbl_8047DB28);
                }
                prev->next = old_next;
            }

            parent = next->parent;
            while (parent != NULL) {
                keep_flags = 0x8FFFFFFF;
                scan = parent->child;
                while (scan != NULL) {
                    root_flags = ((scan->flags << 10) | scan->flags) &
                                 JOBJ_ROOT_MASK;
                    keep_flags |= root_flags;
                    scan = scan->next;
                }
                if ((parent->flags & ~keep_flags) == 0) {
                    break;
                }
                parent->flags &= keep_flags;
                parent = parent->next;
            }
            next->parent = NULL;
        }
        next->next = NULL;
    }

    next->parent = jobj->parent;
    next->next = jobj->next;
    jobj->next = next;

    if (jobj->parent != NULL) {
        root_flags = ((next->flags << 10) | next->flags) & JOBJ_ROOT_MASK;
        parent = jobj->parent;
        while (parent != NULL) {
            if ((root_flags & ~parent->flags) == 0) {
                break;
            }
            parent->flags |= root_flags;
            parent = parent->parent;
        }
    }
}
#endif
#pragma pop

/* 0x801A015C | 0x154 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void OSReport(void* fmt, ...);
#if 0
asm void fn_801A015C(void) {
#include "src/hsd/hsd_jobj_fn_801A015C.inc"
}
#else
#pragma optimization_level 4
void fn_801A015C(HSD_JObj* jobj, HSD_JObj* child) {
    /* decompiled cdx5: functional (non-byte-exact) */
    HSD_JObj* tail;
    HSD_JObj* parent;
    u8* base;
    u32 flags;
    u32 root_flags;

    base = lbl_80274AA0;
    if (jobj == NULL || child == NULL) {
        return;
    }

    if (child->parent != NULL) {
        OSReport(base + 0x178);
        __assert(&lbl_8047DB20, 0x552, base + 0x194);
    }
    if (child->next != NULL) {
        OSReport(base + 0x1AC);
        __assert(&lbl_8047DB20, 0x553, base + 0x1CC);
    }

    if (jobj->child == NULL) {
        jobj->child = child;
    } else {
        if (jobj->flags & JOBJ_INSTANCE) {
            __assert(&lbl_8047DB20, 0x559, base + 0x1E0);
        }
        tail = jobj->child;
        while (tail->next != NULL) {
            if (tail == child) {
                __assert(&lbl_8047DB20, 0x55C, base + 0x200);
            }
            tail = tail->next;
        }
        tail->next = child;
    }

    child->parent = jobj;
    flags = ((volatile HSD_JObj*) child)->flags;
    root_flags = ((volatile HSD_JObj*) child)->flags;
    flags <<= 10;
    flags = root_flags | flags;
    root_flags = flags & JOBJ_ROOT_MASK;
    parent = jobj;
    while (parent != NULL) {
        if ((root_flags & ~((volatile HSD_JObj*) parent)->flags) == 0) {
            break;
        }
        parent->flags = ((volatile HSD_JObj*) parent)->flags | root_flags;
        parent = parent->parent;
    }
}
#endif
#pragma pop

/* 0x801A02B0 | 0x28C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A02B0(void) {
#include "src/hsd/hsd_jobj_fn_801A02B0.inc"
}
#else
#pragma optimization_level 4
HSD_JObj* fn_801A02B0(HSD_JObj* jobj)
{
    /* decompiled cdx6: functional */
    HSD_JObj* child;
    HSD_JObj* replacement;
    HSD_JObj* prev;
    u8* base;

    base = lbl_80274AA0;
    if (jobj == NULL) {
        return NULL;
    }

    child = jobj->child;
    if (child != NULL && child->next != NULL) {
        __assert(&lbl_8047DB20, 0x4CB, base + 0x1CC);
    }

    replacement = child != NULL ? child : jobj->next;

    if (jobj == NULL || jobj->parent == NULL) {
        prev = NULL;
    } else if (jobj->parent->child == jobj) {
        prev = NULL;
    } else {
        prev = jobj->parent->child;
        while (prev != NULL) {
            if (prev->next == jobj) {
                break;
            }
            prev = prev->next;
        }
        if (prev == NULL) {
            HSD_Panic(&lbl_8047DB20, 0x5F8, base + 0x88);
        }
    }

    if (prev != NULL) {
        prev->next = replacement;
    } else if (jobj->parent != NULL) {
        jobj->parent->child = replacement;
    }

    if (replacement != NULL && replacement == child) {
        replacement->next = jobj->next;
        replacement->parent = jobj->parent;
    }

    jobj->parent = NULL;
    jobj->child = NULL;
    jobj->next = NULL;

    if (ref_DEC(jobj) != 0) {
        if (jobj->object.ref_count_individual < 1) {
            HSD_CLASS_METHOD(jobj)->release((HSD_Class*) jobj);
            HSD_CLASS_METHOD(jobj)->destroy((HSD_Class*) jobj);
        } else {
            iref_INC(jobj);
            HSD_JOBJ_METHOD(jobj)->release_child(jobj);
            if (iref_DEC(jobj) != 0) {
                HSD_CLASS_METHOD(jobj)->release((HSD_Class*) jobj);
                HSD_CLASS_METHOD(jobj)->destroy((HSD_Class*) jobj);
            }
        }
    }

    return child;
}
#endif
#pragma pop

/* 0x801A053C | 0xB0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A053C(void) {
#include "src/hsd/hsd_jobj_fn_801A053C.inc"
}
#else
#pragma optimization_level 1
static inline BOOL iref_DEC_indiv(void* o) {
    BOOL r;
    if ((r = (*(volatile u16*)&HSD_OBJ(o)->ref_count_individual == 0))) {
        return r;
    }
    HSD_OBJ(o)->ref_count_individual -= 1;
    return HSD_OBJ(o)->ref_count_individual == 0;
}
static inline s32 ref_CNT_obj(void* o) {
    if (*(volatile u16*)&HSD_OBJ(o)->ref_count == HSD_OBJ_NOREF) {
        return -1;
    }
    return HSD_OBJ(o)->ref_count;
}
void fn_801A053C(void* obj) {
    HSD_ClassInfo* info;

    if (obj == NULL) {
        return;
    }
    if (iref_DEC_indiv(obj)) {
        if (ref_CNT_obj(obj) < 0) {
            if (obj != NULL) {
                info = HSD_CLASS_METHOD(obj);
                info->release((HSD_Class*)obj);
                info = HSD_CLASS_METHOD(obj);
                info->destroy((HSD_Class*)obj);
            }
        }
    }
}
#endif
#pragma pop

/* 0x801A0744 | 0x458 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void __assert();
extern void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObjDesc* desc);
extern void* HSD_IDGetDataFromTable(void* table, u32 key, u32* found);
extern void fn_80196E10(void* file, u32 line, void* expr);
extern void fn_801991F8(HSD_DObj* dobj, HSD_DObjDesc* desc);
extern void* fn_8019C128(void* table, u32 key, u32* found);
extern void fn_801A05EC(void);
extern void HSD_JObjRef(HSD_JObj* jobj);
extern void* HSD_IDGetData(u32 key, u32* found);
extern void fn_801A0B9C(HSD_JObj* jobj);
extern void* fn_801A0BF0(u32 key, u32* found);
extern void fn_801A0C1C();
extern BOOL fn_801A0C68(HSD_Obj*);
extern void fn_801A0C9C();
extern void fn_801A0CE8(void*);
extern s32 fn_801A0D3C();
extern BOOL fn_801A0D48(void*);
extern void fn_801A0744(HSD_JObj* jobj, HSD_Joint* joint);
extern void fn_801A0D94(HSD_JObj* jobj, HSD_Joint* joint);
extern void fn_801AEBE4(HSD_RObj* robj, HSD_RObjDesc* desc);
#if 0
asm void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint) {
#include "src/hsd/hsd_jobj_fn_801A0744.inc"
}
#else
#pragma optimization_level 4
void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint)
{
    /* decompiled cdx6: functional */
    while (jobj != NULL && joint != NULL) {
        fn_801A0D94(jobj, joint);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            HSD_JObjResolveRefsAll(jobj->child, joint->child);
        }
        jobj = jobj->next;
        joint = joint->next;
    }
}
#endif
#pragma pop

/* 0x801A0B9C | 0x54 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u8 lbl_80274AF4[];
extern u8 lbl_80274B64[];
#if 0
asm void HSD_JObjRef(void) {
#include "src/hsd/hsd_jobj_fn_801A0B9C.inc"
}
#else
#pragma optimization_level 4
void HSD_JObjRef(HSD_JObj* jobj) {
    if (jobj != NULL) {
        jobj->object.ref_count++;
        if (!(jobj->object.ref_count != HSD_OBJ_NOREF)) {
            __assert(lbl_80274AF4, 0x5d, lbl_80274B64);
        }
    }
}
#endif
#pragma pop

/* 0x801A0BF0 | 0x2C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_IDGetData(void) {
#include "src/hsd/hsd_jobj_HSD_IDGetData.inc"
}
#else
#pragma optimization_level 4
void* HSD_IDGetData(u32 key, u32* found) {
    return HSD_IDGetDataFromTable(NULL, key, found);
}
#endif
#pragma pop

/* 0x801A0C1C | 0x4C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A0C1C(void) {
#include "src/hsd/hsd_jobj_fn_801A0C1C.inc"
}
#else
#pragma optimization_level 4
void fn_801A0C1C(HSD_Obj* obj) {
    obj->ref_count++;
    if (!(obj->ref_count != HSD_OBJ_NOREF)) {
        __assert(lbl_80274AF4, 0x5d, lbl_80274B64);
    }
}
#endif
#pragma pop

/* 0x801A0C68 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A0C68(void) {
#include "src/hsd/hsd_jobj_fn_801A0C68.inc"
}
#else
#pragma optimization_level 4
BOOL fn_801A0C68(HSD_Obj* obj) {
    BOOL ret;

    if ((ret = (*(volatile u16*)&obj->ref_count_individual == 0))) {
        return ret;
    }
    obj->ref_count_individual--;
    return obj->ref_count_individual == 0;
}
#endif
#pragma pop

/* 0x801A0C9C | 0x4C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u8 lbl_80274B00[];
#if 0
asm void fn_801A0C9C(void) {
#include "src/hsd/hsd_jobj_fn_801A0C9C.inc"
}
#else
#pragma optimization_level 4
void fn_801A0C9C(HSD_Obj* obj) {
    obj->ref_count_individual++;
    if (!(obj->ref_count_individual != 0)) {
        __assert(lbl_80274AF4, 0x9e, lbl_80274B00);
    }
}
#endif
#pragma pop

/* 0x801A0CE8 | 0x54 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A0CE8(void) {
#include "src/hsd/hsd_jobj_fn_801A0CE8.inc"
}
#else
#pragma optimization_level 1
void fn_801A0CE8(void* obj) {
    HSD_ClassInfo* info;

    if (obj != NULL) {
        info = HSD_CLASS_METHOD(obj);
        info->release((HSD_Class*)obj);
        info = HSD_CLASS_METHOD(obj);
        info->destroy((HSD_Class*)obj);
    }
}
#endif
#pragma pop

/* 0x801A0D3C | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm s32 fn_801A0D3C(void) {
#include "src/hsd/hsd_jobj_fn_801A0D3C.inc"
}
#else
#pragma optimization_level 4
s32 fn_801A0D3C(HSD_Obj* obj) {
    return obj->ref_count_individual - 1;
}
#endif
#pragma pop

/* 0x801A0D48 | 0x4C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A0D48(void) {
#include "src/hsd/hsd_jobj_fn_801A0D48.inc"
}
#else
#pragma optimization_level 1
BOOL fn_801A0D48(void* o) {
    BOOL ret;

    if ((ret = (*(volatile u16*)&HSD_OBJ(o)->ref_count == HSD_OBJ_NOREF))) {
        return ret;
    }
    ret = (*(volatile u16*)&HSD_OBJ(o)->ref_count == 0);
    HSD_OBJ(o)->ref_count--;
    return ret;
}
#endif
#pragma pop

/* 0x801A0D94 | 0x228 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A0D94(void) {
#include "src/hsd/hsd_jobj_fn_801A0D94.inc"
}
#else
#pragma optimization_level 4
void fn_801A0D94(HSD_JObj* jobj, HSD_Joint* joint) {
    /* decompiled cdx5: functional (non-byte-exact) */
    HSD_JObj* child;
    u8* base;

    base = lbl_80274AA0;
    if (jobj == NULL || joint == NULL) {
        return;
    }

    fn_801AEBE4(jobj->robj, joint->robjdesc);
    if (jobj->flags & JOBJ_INSTANCE) {
        child = jobj->child;
        if (child != NULL) {
            if (ref_DEC(child) != 0) {
                if (child->object.ref_count_individual < 1) {
                    if (child != NULL) {
                        HSD_CLASS_METHOD(child)->release((HSD_Class*) child);
                        HSD_CLASS_METHOD(child)->destroy((HSD_Class*) child);
                    }
                } else {
                    iref_INC(child);
                    HSD_JOBJ_METHOD(child)->release_child(child);
                    if (iref_DEC(child) != 0) {
                        if (child != NULL) {
                            HSD_CLASS_METHOD(child)->release((HSD_Class*) child);
                            HSD_CLASS_METHOD(child)->destroy((HSD_Class*) child);
                        }
                    }
                }
            }
        }

        jobj->child =
            HSD_IDGetDataFromTable(NULL, (u32) joint->child, NULL);
        if (jobj->child == NULL) {
            __assert(&lbl_8047DB20, 0x45F, base + 0x210);
        }
        if (jobj->child != NULL) {
            HSD_JObjRef(jobj->child);
        }
    }

    if (!union_type_ptcl(jobj) && !union_type_spline(jobj)) {
        HSD_DObjResolveRefsAll(jobj->u.dobj, joint->u.dobjdesc);
    }
}
#endif
#pragma pop

/* 0x801A0FBC | 0xDC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern HSD_ClassInfo* fn_80193748(const char*);
extern HSD_JObj* fn_80193828(HSD_ClassInfo*);
extern u32 lbl_8047B298;
#if 1
HSD_JObj* fn_801A0FBC(HSD_Joint* joint)
{
#pragma optimization_level 1
    HSD_JObj* jobj;
    HSD_Joint* joint_arg;
    HSD_ClassInfo* info;

    if (joint == NULL) {
        jobj = NULL;
        goto done;
    }

    if (joint->class_name != NULL) {
        info = fn_80193748(joint->class_name);
        if (info != NULL) {
            goto found;
        }
    }

    if (lbl_8047B298 == 0) {
        goto default_info;
    }
    info = (HSD_ClassInfo*) lbl_8047B298;
    goto got_info;

default_info:
    info = (HSD_ClassInfo*) lbl_8036C8E0;

got_info:
    jobj = fn_80193828(info);
    if (jobj == NULL) {
        __assert(&lbl_8047DB20, 0x7DF, &lbl_8047DB3C);
    }
    goto setup;

found:
    jobj = fn_80193828(info);
    if (jobj == NULL) {
        __assert(&lbl_8047DB20, 0x3D5, &lbl_8047DB3C);
    }

setup:
    {
        void** vtable = *(void***) jobj;
        ((void (*)(HSD_JObj*, HSD_Joint*, HSD_JObj*)) vtable[0x3C / 4])(jobj, joint, NULL);
    }

done:
    HSD_JObjResolveRefsAll(jobj, (joint_arg = joint));
    return jobj;
}
#else
#pragma optimization_level 1
HSD_JObj* fn_801A0FBC(HSD_Joint* joint)
{
    HSD_JObj* jobj;
    HSD_ClassInfo* info;

    if (joint == NULL) {
        jobj = NULL;
        goto done;
    }

    if (joint->class_name != NULL) {
        info = fn_80193748(joint->class_name);
        if (info != NULL) {
            goto found;
        }
    }

    if (lbl_8047B298 != 0) {
        info = (HSD_ClassInfo*) lbl_8047B298;
    } else {
        info = (HSD_ClassInfo*) lbl_8036C8E0;
    }
    jobj = fn_80193828(info);
    if (jobj == NULL) {
        __assert(&lbl_8047DB20, 0x7DF, &lbl_8047DB3C);
    }
    goto setup;

found:
    jobj = fn_80193828(info);
    if (jobj == NULL) {
        __assert(&lbl_8047DB20, 0x3D5, &lbl_8047DB3C);
    }

setup:
    {
        void** vtable = *(void***) jobj;
        ((void (*)(HSD_JObj*, HSD_Joint*, HSD_JObj*)) vtable[0x3C / 4])(jobj, joint, NULL);
    }

done:
    HSD_JObjResolveRefsAll(jobj, joint);
    return jobj;
}
#endif
#pragma pop

/* 0x801A1098 | 0x334 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern HSD_DObj* fn_801992D8(HSD_DObjDesc* desc);
extern HSD_RObj* fn_801AE5E8(HSD_RObjDesc* desc);
extern void fn_800A2D38(f32 mtx[3][4]);
extern f32* HSD_MtxAlloc(void);
extern void HSD_IDInsertToTable(void* table, u32 key, u32 value);
extern void* memcpy(void* dst, const void* src, u32 n);
extern u32 lbl_8047B298;
#if 0
asm void fn_801A1098(void) {
#include "src/hsd/hsd_jobj_fn_801A1098.inc"
}
#else
#pragma optimization_level 4
s32 fn_801A1098(HSD_JObj* jobj, HSD_Joint* joint, HSD_JObj* parent)
{
    /* decompiled cdx6: functional */
    HSD_JObj* child;
    HSD_Joint* child_joint;
    HSD_ClassInfo* info;
    HSD_SList* ptcl;
    HSD_JObj* tail;
    HSD_JObj* ancestor;
    u32 root_flags;
    u8* base;

    (void) parent;
    base = lbl_80274AA0;

    jobj->child = NULL;
    jobj->next = NULL;
    jobj->parent = NULL;
    jobj->flags |= joint->flags;

    if (jobj->flags & JOBJ_SPLINE) {
        jobj->u.spline = joint->u.spline;
    } else if (jobj->flags & JOBJ_PTCL) {
        jobj->u.ptcl = joint->u.ptcl;
        ptcl = jobj->u.ptcl;
        while (ptcl != NULL) {
            *(u32*) &ptcl->data |= 0x80000000u;
            ptcl = ptcl->next;
        }
    } else {
        jobj->u.dobj = (HSD_DObj*) fn_801992D8(joint->u.dobjdesc);
    }

    jobj->robj = (HSD_RObj*) fn_801AE5E8(joint->robjdesc);
    jobj->rotate_x = joint->rotation_x;
    jobj->rotate_y = joint->rotation_y;
    jobj->rotate_z = joint->rotation_z;
    jobj->scale_x = joint->scale_x;
    jobj->scale_y = joint->scale_y;
    jobj->scale_z = joint->scale_z;
    jobj->translate_x = joint->position_x;
    jobj->translate_y = joint->position_y;
    jobj->translate_z = joint->position_z;
    fn_800A2D38(jobj->mtx);
    jobj->scl = NULL;

    if (joint->mtx != NULL) {
        jobj->envelopemtx = HSD_MtxAlloc();
        memcpy(jobj->envelopemtx, joint->mtx, 0x30);
    }

    HSD_IDInsertToTable(NULL, (u32) joint, (u32) jobj);
    jobj->id = (u32) joint;

    if (joint->flags & JOBJ_INSTANCE) {
        return 0;
    }

    child_joint = joint->child;
    while (child_joint != NULL) {
        if (child_joint->class_name != NULL) {
            info = fn_80193748(child_joint->class_name);
            if (info != NULL) {
                child = fn_80193828(info);
                if (child == NULL) {
                    __assert(&lbl_8047DB20, 0x3D5, &lbl_8047DB3C);
                }
                goto load_child;
            }
        }

        if (lbl_8047B298 != 0) {
            info = (HSD_ClassInfo*) lbl_8047B298;
        } else {
            info = (HSD_ClassInfo*) lbl_8036C8E0;
        }
        child = fn_80193828(info);
        if (child == NULL) {
            __assert(&lbl_8047DB20, 0x7DF, &lbl_8047DB3C);
        }

load_child:
        HSD_JOBJ_METHOD(child)->load(child, child_joint, jobj);
        if (child == NULL) {
            return -1;
        }

        if (child->parent != NULL) {
            OSReport(base + 0x178);
            __assert(&lbl_8047DB20, 0x552, base + 0x194);
        }
        if (child->next != NULL) {
            OSReport(base + 0x1AC);
            __assert(&lbl_8047DB20, 0x553, base + 0x1CC);
        }

        if (jobj->child == NULL) {
            jobj->child = child;
        } else {
            if (jobj->flags & JOBJ_INSTANCE) {
                __assert(&lbl_8047DB20, 0x559, base + 0x1E0);
            }
            tail = jobj->child;
            while (tail->next != NULL) {
                if (tail == child) {
                    __assert(&lbl_8047DB20, 0x55C, base + 0x200);
                }
                tail = tail->next;
            }
            tail->next = child;
        }

        child->parent = jobj;
        root_flags = ((child->flags << 10) | child->flags) & JOBJ_ROOT_MASK;
        ancestor = jobj;
        while (ancestor != NULL) {
            if ((root_flags & ~ancestor->flags) == 0) {
                break;
            }
            ancestor->flags |= root_flags;
            ancestor = ancestor->parent;
        }

        child_joint = child_joint->next;
    }

    return 0;
}
#endif
#pragma pop

/* 0x801A1980 | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801A1980(void) {
#include "src/hsd/hsd_jobj_fn_801A1980.inc"
}
#else
f32* fn_801A1980(HSD_JObj* jobj) {
    return jobj->mtx[1];
}
#endif
#pragma pop
