/**
 * @file jobj.h
 * @brief HAL sysdolphin jobj.h: the HSD_JObj layout and the jobj.h inlines
 *        other baselib files expand.
 *
 * The file keeps HAL's name because its inlines assert through __FILE__:
 * retail callers pool "jobj.h" and "jobj" in their own .sdata2 (robj.c at
 * 0x8047DD68/0x8047DD70). Colosseum's jobj.h is newer than Melee's; its
 * assert lines are read from the retail code (HSD_JObjMtxIsDirty 605,
 * HSD_JObjGetMtxPtr 1148, the rotation / scale / translation setters).
 *
 * The bodies below are the forms jobj.c's code pins down (jobj.c expands
 * them deep inside recursive functions, where MWCC's auto-inline budget is
 * sensitive to the size of each body):
 *  - HSD_JObjMtxIsDirty returns the flag test directly; with it
 *    HSD_JObjSetMtxDirtySub expands itself six levels (0x8019D620) and the
 *    register assignment of resolveIKJoint1 and JObjUpdateFunc is retail's;
 *  - HSD_JObjRef tests for NULL and increments through object.h's
 *    unchecked ref_INC_nocheck (jobj.c's out-of-line copy at 0x801A0C1C).
 *
 * Layout and inline bodies follow the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/jobj.h); the 0x88-byte size is Colosseum's.
 */
#ifndef SYSDOLPHIN_BASELIB_JOBJ_H
#define SYSDOLPHIN_BASELIB_JOBJ_H

#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_forward.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/object.h"

/* The SDK quaternion type (Melee keeps it in dolphin/mtx.h); hsd/hsd_tobj.h
 * carries the same definition. */
#ifndef HSD_TOBJ_H
typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;
#endif

#define JOBJ_SKELETON        (1 << 0)
#define JOBJ_SKELETON_ROOT   (1 << 1)
#define JOBJ_HIDDEN          (1 << 4)
#define JOBJ_PTCL            (1 << 5)
#define JOBJ_MTX_DIRTY       (1 << 6)
#define JOBJ_LIGHTING        (1 << 7)
#define JOBJ_BILLBOARD_FIELD 0xE00
#define JOBJ_BILLBOARD       0x200
#define JOBJ_VBILLBOARD      0x400
#define JOBJ_HBILLBOARD      0x600
#define JOBJ_RBILLBOARD      0x800
#define JOBJ_PBILLBOARD      0x2000
#define JOBJ_SPLINE          (1 << 14)
#define JOBJ_SPECULAR        (1 << 16)
#define JOBJ_OPA             (1 << 18)
#define JOBJ_XLU             (1 << 19)
#define JOBJ_TEXEDGE         (1 << 20)
#define JOBJ_USER_DEF_MTX    (1 << 23)

#define JOBJ_TRSP_SHIFT 18

#define JOBJ_PTCL_ACTIVE       0x7FFFFFFF
#define JOBJ_PTCL_OFFSET_MASK  0xFFFFFF
#define JOBJ_PTCL_OFFSET_SHIFT 6
#define JOBJ_PTCL_BANK_MASK    0x3F

#define union_type_ptcl(o) ((o)->flags & JOBJ_PTCL ? TRUE : FALSE)
#define union_type_dobj(o) ((o)->flags & (JOBJ_PTCL | JOBJ_SPLINE) ? FALSE : TRUE)

struct HSD_JObj {
    /* 0x00 */ HSD_Obj object;
    /* 0x08 */ HSD_JObj* next;
    /* 0x0C */ HSD_JObj* parent;
    /* 0x10 */ HSD_JObj* child;
    /* 0x14 */ u32 flags;
    /* 0x18 */ union {
        HSD_SList* ptcl;
        HSD_DObj* dobj;
        HSD_Spline* spline;
    } u;
    /* 0x1C */ Quaternion rotate;
    /* 0x2C */ Vec3 scale;
    /* 0x38 */ Vec3 translate;
    /* 0x44 */ Mtx mtx;
    /* 0x74 */ Vec3* scl;
    /* 0x78 */ MtxPtr envelopemtx;
    /* 0x7C */ HSD_AObj* aobj;
    /* 0x80 */ HSD_RObj* robj;
    /* 0x84 */ u32 id;
};

struct HSD_Joint {
    /* 0x00 */ char* class_name;
    /* 0x04 */ u32 flags;
    /* 0x08 */ HSD_Joint* child;
    /* 0x0C */ HSD_Joint* next;
    /* 0x10 */ union {
        HSD_DObjDesc* dobjdesc;
        HSD_Spline* spline;
        HSD_SList* ptcl;
    } u;
    /* 0x14 */ Vec3 rotation;
    /* 0x20 */ Vec3 scale;
    /* 0x2C */ Vec3 position;
    /* 0x38 */ MtxPtr mtx;
    /* 0x3C */ HSD_RObjDesc* robjdesc;
};

/* Colosseum's JObj class info is 0x54 bytes (JObjInfoInit passes 0x54):
 * after Melee's methods it carries the animation update function the class
 * hands to HSD_AObjInterpretAnim / HSD_RObjUpdateAll. */
typedef struct _HSD_JObjInfo {
    HSD_ObjInfo parent;
    s32 (*load)(HSD_JObj* jobj, HSD_Joint* joint, HSD_JObj* parent);
    void (*make_mtx)(HSD_JObj* jobj);
    void (*make_pmtx)(HSD_JObj* jobj, MtxPtr vmtx, MtxPtr pmtx);
    void (*disp)(HSD_JObj* jobj, MtxPtr vmtx, MtxPtr pmtx,
                 HSD_TrspMask trsp_mask, u32 rendermode);
    void (*release_child)(HSD_JObj* jobj);
    HSD_ObjUpdateFunc update;
} HSD_JObjInfo;

#define HSD_JOBJ_INFO(i) ((HSD_JObjInfo*) (i))
#define HSD_JOBJ_METHOD(o) HSD_JOBJ_INFO(HSD_CLASS_METHOD(o))

#define JOBJ_ENVELOPE_MODEL   (1 << 2)
#define JOBJ_CLASSICAL_SCALE  (1 << 3)
#define JOBJ_INSTANCE         (1 << 12)
#define JOBJ_USE_QUATERNION   (1 << 17)
#define JOBJ_JOINT1           (1 << 21)
#define JOBJ_JOINT2           (2 << 21)
#define JOBJ_EFFECTOR         (3 << 21)
#define JOBJ_MTX_INDEP_PARENT (1 << 24)
#define JOBJ_MTX_INDEP_SRT    (1 << 25)
#define JOBJ_ROOT_OPA         (1 << 28)
#define JOBJ_ROOT_XLU         (1 << 29)
#define JOBJ_ROOT_TEXEDGE     (1 << 30)
#define JOBJ_ROOT_MASK        (JOBJ_ROOT_OPA | JOBJ_ROOT_XLU | JOBJ_ROOT_TEXEDGE)

#define union_type_spline(o) ((o)->flags & JOBJ_SPLINE ? TRUE : FALSE)

/* JObj animation tracks (HSD_FObj::obj_type / JObjUpdateFunc's type). */
#define HSD_A_J_ROTX      1
#define HSD_A_J_ROTY      2
#define HSD_A_J_ROTZ      3
#define HSD_A_J_PATH      4
#define HSD_A_J_TRAX      5
#define HSD_A_J_TRAY      6
#define HSD_A_J_TRAZ      7
#define HSD_A_J_SCAX      8
#define HSD_A_J_SCAY      9
#define HSD_A_J_SCAZ      10
#define HSD_A_J_NODE      11
#define HSD_A_J_BRANCH    12
#define HSD_A_J_SETBYTE0  20
#define HSD_A_J_SETBYTE1  21
#define HSD_A_J_SETBYTE2  22
#define HSD_A_J_SETBYTE3  23
#define HSD_A_J_SETBYTE4  24
#define HSD_A_J_SETBYTE5  25
#define HSD_A_J_SETBYTE6  26
#define HSD_A_J_SETBYTE7  27
#define HSD_A_J_SETBYTE8  28
#define HSD_A_J_SETBYTE9  29
#define HSD_A_J_SETFLOAT0 30
#define HSD_A_J_SETFLOAT1 31
#define HSD_A_J_SETFLOAT2 32
#define HSD_A_J_SETFLOAT3 33
#define HSD_A_J_SETFLOAT4 34
#define HSD_A_J_SETFLOAT5 35
#define HSD_A_J_SETFLOAT6 36
#define HSD_A_J_SETFLOAT7 37
#define HSD_A_J_SETFLOAT8 38
#define HSD_A_J_SETFLOAT9 39

/* HSD_JObjSetupMatrixSub */
void HSD_JObjSetupMatrixSub(HSD_JObj* jobj);
void HSD_JObjUnrefThis(HSD_JObj* jobj);
/* HSD_JObjMakeMatrix */
void HSD_JObjMakeMatrix(HSD_JObj* jobj);

static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    HSD_ASSERT(605, jobj);
    return !(jobj->flags & JOBJ_USER_DEF_MTX) && (jobj->flags & JOBJ_MTX_DIRTY);
}

/* HSD_JObjSetMtxDirtySub (fn_8019D620) is declared inside HSD_JObjSetMtxDirty
 * only. With a file-scope declaration MWCC will not inline, into jobj.c's
 * callers, any function whose body calls the self-expanding recursive
 * SetMtxDirtySub, and jobj.c's ClearFlags/SetFlags(All), JObjUpdateFunc and
 * HSD_JObjAddAnim(All) no longer match (they come out as calls). */
static inline void HSD_JObjSetMtxDirty(HSD_JObj* jobj)
{
    /* RULE-EXCEPTION(title-path): block-scope extern to steer inlining - see docs/RULE_EXCEPTIONS.md */
    void fn_8019D620(HSD_JObj* jobj);

    if (jobj == NULL || HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D620(jobj);
}

static inline void HSD_JObjSetRotationX(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(676, jobj);
    HSD_ASSERT(677, !(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->rotate.x = x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetRotationY(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(696, jobj);
    HSD_ASSERT(697, !(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->rotate.y = y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetRotationZ(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(716, jobj);
    HSD_ASSERT(717, !(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->rotate.z = z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetScaleX(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(805, jobj);
    jobj->scale.x = x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetScaleY(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(819, jobj);
    jobj->scale.y = y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetScaleZ(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(833, jobj);
    jobj->scale.z = z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetTranslateX(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(952, jobj);
    jobj->translate.x = x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetTranslateY(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(966, jobj);
    jobj->translate.y = y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetTranslateZ(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(980, jobj);
    jobj->translate.z = z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

/*
 * Melee's HSD_JObjAddTranslationX/Y/Z. The names follow symbols.txt, where
 * psinterpret.c's out-of-line copies (0x80172790 / 0x801726E0 /
 * 0x80172630, assert lines 1109 / 1120 / 1131) are HSD_JObjAddTx/Ty/Tz.
 */
static inline void HSD_JObjAddTx(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(1109, jobj);
    jobj->translate.x += x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjAddTy(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(1120, jobj);
    jobj->translate.y += y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjAddTz(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(1131, jobj);
    jobj->translate.z += z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (!jobj || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    HSD_JObjSetupMatrixSub(jobj);
}

static inline MtxPtr HSD_JObjGetMtxPtr(HSD_JObj* jobj)
{
    HSD_ASSERT(1148, jobj);
    HSD_JObjSetupMatrix(jobj);
    return jobj->mtx;
}

static inline void HSD_JObjRef(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        ref_INC_nocheck(jobj);
    }
}

static inline void HSD_JObjRefThis(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return;
    }
    iref_INC(jobj);
}

#endif /* SYSDOLPHIN_BASELIB_JOBJ_H */
