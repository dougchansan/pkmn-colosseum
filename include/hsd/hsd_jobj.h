/**
 * @file hsd_jobj.h
 * @brief HSD JObj - Joint objects (skeletal hierarchy).
 *
 * JObj is the core scene graph node. It represents a joint in a
 * skeletal hierarchy with rotation, scale, translation, and an
 * attached display object chain. JObj supports billboarding,
 * envelope skinning, spline attachment, and IK constraints.
 *
 * Colosseum address range: 0x8019CE50 (HSD_JObjInit)
 * Adapted from the Melee decompilation (doldecomp/melee).
 */
#ifndef HSD_JOBJ_H
#define HSD_JOBJ_H

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_object.h"

/* ========================================================================= */
/*  Joint animation attribute indices                                        */
/* ========================================================================= */

#define HSD_A_J_ROTX   1
#define HSD_A_J_ROTY   2
#define HSD_A_J_ROTZ   3
#define HSD_A_J_PATH   4
#define HSD_A_J_TRAX   5
#define HSD_A_J_TRAY   6
#define HSD_A_J_TRAZ   7
#define HSD_A_J_SCAX   8
#define HSD_A_J_SCAY   9
#define HSD_A_J_SCAZ   10
#define HSD_A_J_NODE   11
#define HSD_A_J_BRANCH 12

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

/* ========================================================================= */
/*  JObj flags                                                               */
/* ========================================================================= */

#define JOBJ_BILLBOARD_FIELD    0xE00
#define JOBJ_BILLBOARD          0x200
#define JOBJ_VBILLBOARD         0x400
#define JOBJ_HBILLBOARD         0x600
#define JOBJ_RBILLBOARD         0x800
#define JOBJ_PBILLBOARD         0x2000

#define JOBJ_SKELETON           (1 << 0)
#define JOBJ_SKELETON_ROOT      (1 << 1)
#define JOBJ_ENVELOPE_MODEL     (1 << 2)
#define JOBJ_CLASSICAL_SCALE    (1 << 3)
#define JOBJ_HIDDEN             (1 << 4)
#define JOBJ_PTCL               (1 << 5)
#define JOBJ_MTX_DIRTY          (1 << 6)
#define JOBJ_LIGHTING           (1 << 7)
#define JOBJ_TEXGEN             (1 << 8)
#define JOBJ_INSTANCE           (1 << 12)
#define JOBJ_SPLINE             (1 << 14)
#define JOBJ_FLIP_IK            (1 << 15)
#define JOBJ_SPECULAR           (1 << 16)
#define JOBJ_USE_QUATERNION     (1 << 17)
#define JOBJ_NULL_OBJ           (0 << 21)
#define JOBJ_JOINT1             (1 << 21)
#define JOBJ_JOINT2             (2 << 21)
#define JOBJ_JOINT              (3 << 21)
#define JOBJ_EFFECTOR           (3 << 21)
#define JOBJ_USER_DEF_MTX       (1 << 23)
#define JOBJ_MTX_INDEP_PARENT   (1 << 24)
#define JOBJ_MTX_INDEP_SRT      (1 << 25)
#define JOBJ_ROOT_OPA           (1 << 28)
#define JOBJ_ROOT_XLU           (1 << 29)
#define JOBJ_ROOT_TEXEDGE       (1 << 30)

#define JOBJ_ROOT_MASK (JOBJ_ROOT_OPA | JOBJ_ROOT_TEXEDGE | JOBJ_ROOT_XLU)

#define union_type_ptcl(o) ((o)->flags & JOBJ_PTCL ? TRUE : FALSE)
#define union_type_spline(o) ((o)->flags & JOBJ_SPLINE ? TRUE : FALSE)
#define union_type_dobj(o) \
    ((o)->flags & (JOBJ_PTCL | JOBJ_SPLINE) ? FALSE : TRUE)

/* ========================================================================= */
/*  Casting macros                                                           */
/* ========================================================================= */

#define HSD_JOBJ_INFO(i) ((HSD_JObjInfo*) (i))
#define HSD_JOBJ_METHOD(o) HSD_JOBJ_INFO((o)->object.parent.class_info)

/* ========================================================================= */
/*  JObj structure                                                           */
/* ========================================================================= */

struct HSD_JObj {
    /*  +0 */ HSD_Obj object;
    /*  +8 */ HSD_JObj* next;
    /*  +C */ HSD_JObj* parent;
    /* +10 */ HSD_JObj* child;
    /* +14 */ u32 flags;
    /* +18 */ union {
        HSD_SList* ptcl;
        HSD_DObj* dobj;
        HSD_Spline* spline;
    } u;
    /* +1C */ f32 rotate_x;     /* Quaternion x / Euler x */
    /* +20 */ f32 rotate_y;     /* Quaternion y / Euler y */
    /* +24 */ f32 rotate_z;     /* Quaternion z / Euler z */
    /* +28 */ f32 rotate_w;     /* Quaternion w (if JOBJ_USE_QUATERNION) */
    /* +2C */ f32 scale_x;
    /* +30 */ f32 scale_y;
    /* +34 */ f32 scale_z;
    /* +38 */ f32 translate_x;
    /* +3C */ f32 translate_y;
    /* +40 */ f32 translate_z;
    /* +44 */ f32 mtx[3][4];    /* Local-to-world matrix */
    /* +74 */ f32* scl;         /* Optional non-uniform scale */
    /* +78 */ f32* envelopemtx; /* Envelope matrix pointer */
    /* +7C */ HSD_AObj* aobj;
    /* +80 */ HSD_RObj* robj;
    /* +84 */ u32 id;
};

/* ========================================================================= */
/*  Joint descriptor (data format)                                           */
/* ========================================================================= */

struct HSD_Joint {
    /* +0 */ char* class_name;
    /* +4 */ u32 flags;
    /* +8 */ HSD_Joint* child;
    /* +C */ HSD_Joint* next;
    /* +10 */ union {
        HSD_DObjDesc* dobjdesc;
        HSD_Spline* spline;
        HSD_SList* ptcl;
    } u;
    /* +14 */ f32 rotation_x;
    /* +18 */ f32 rotation_y;
    /* +1C */ f32 rotation_z;
    /* +20 */ f32 scale_x;
    /* +24 */ f32 scale_y;
    /* +28 */ f32 scale_z;
    /* +2C */ f32 position_x;
    /* +30 */ f32 position_y;
    /* +34 */ f32 position_z;
    /* +38 */ f32* mtx;
    /* +3C */ HSD_RObjDesc* robjdesc;
};

/* ========================================================================= */
/*  JObj class info                                                          */
/* ========================================================================= */

typedef struct _HSD_JObjInfo {
    HSD_ObjInfo parent;
    s32 (*load)(HSD_JObj* jobj, HSD_Joint* joint, HSD_JObj* parent);
    void (*make_mtx)(HSD_JObj* jobj);
    void (*make_pmtx)(HSD_JObj* jobj, f32 mtx[3][4], f32 rmtx[3][4]);
    void (*disp)(HSD_JObj* jobj, f32 vmtx[3][4], f32 pmtx[3][4],
                 HSD_TrspMask trsp_mask, u32 rendermode);
    void (*release_child)(HSD_JObj* jobj);
} HSD_JObjInfo;

extern HSD_JObjInfo hsdJObj;

/* ========================================================================= */
/*  Function declarations                                                    */
/* ========================================================================= */

void HSD_JObjSetDefaultClass(HSD_ClassInfo* info);
u32 HSD_JObjGetFlags(HSD_JObj* jobj);
void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags);
void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags);
void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags);
void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags);
void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame);
void fn_8019D9DC(HSD_JObj* jobj);
void fn_8019D620(HSD_JObj* jobj);
void HSD_JObjRef(HSD_JObj* jobj);
void HSD_JObjUnref(HSD_JObj* jobj);
HSD_JObj* HSD_JObjRemove(HSD_JObj* jobj);
void HSD_JObjRemoveAll(HSD_JObj* jobj);
HSD_DObj* HSD_JObjGetDObj(HSD_JObj* jobj);
HSD_JObj* HSD_JObjLoadJoint(HSD_Joint* joint);
void HSD_JObjAddAnimAll(HSD_JObj* jobj, HSD_AnimJoint* animjoint,
                        HSD_MatAnimJoint* matanimjoint,
                        HSD_ShapeAnimJoint* shapeanimjoint);
void fn_801A301C(HSD_JObj* jobj, HSD_AnimJoint* animjoint,
                 HSD_MatAnimJoint* matanimjoint,
                 HSD_ShapeAnimJoint* shapeanimjoint);
void HSD_JObjAnimAll(HSD_JObj* jobj);
HSD_JObj* HSD_JObjAlloc(void);
void HSD_JObjSetCurrent(HSD_JObj* jobj);
HSD_JObj* HSD_JObjGetCurrent(void);
void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint);
void HSD_JObjDispAll(HSD_JObj* jobj, f32 vmtx[3][4], u32 flags,
                     u32 rendermode);
void HSD_JObjRemoveAnim(HSD_JObj* jobj);
void HSD_JObjRemoveAnimAll(HSD_JObj* jobj);
void HSD_JObjAddChild(HSD_JObj* jobj, HSD_JObj* child);
void HSD_JObjAddNext(HSD_JObj* jobj, HSD_JObj* next);
void HSD_JObjAddDObj(HSD_JObj* jobj, HSD_DObj* dobj);
void HSD_JObjAnim(HSD_JObj* jobj);

/* ========================================================================= */
/*  Inline functions                                                         */
/* ========================================================================= */

static inline HSD_JObj* HSD_JObjGetChild(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return NULL;
    }
    return jobj->child;
}

static inline HSD_JObj* HSD_JObjGetNext(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return NULL;
    }
    return jobj->next;
}

static inline HSD_JObj* HSD_JObjGetParent(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        return NULL;
    }
    return jobj->parent;
}

static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;
    HSD_ASSERT(564, jobj);
    result = FALSE;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX) &&
        (jobj->flags & JOBJ_MTX_DIRTY))
    {
        result = TRUE;
    }
    return result;
}

static inline void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (!jobj || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

#define HSD_JObjSetMtxDirty(jobj) \
    { \
        if ((jobj) != NULL && !HSD_JObjMtxIsDirty(jobj)) { \
            fn_8019D620(jobj); \
        } \
    }

static inline void HSD_JObjSetRotationX(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(0x2A4, jobj);
    HSD_ASSERT(0x2A5, !(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->rotate_x = x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetRotationY(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(0x2B8, jobj);
    HSD_ASSERT(0x2B9, !(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->rotate_y = y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetRotationZ(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(0x2CC, jobj);
    HSD_ASSERT(0x2CD, !(jobj->flags & JOBJ_USE_QUATERNION));
    jobj->rotate_z = z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetScaleX(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(0x325, jobj);
    jobj->scale_x = x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetScaleY(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(0x333, jobj);
    jobj->scale_y = y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetScaleZ(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(0x341, jobj);
    jobj->scale_z = z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetTranslateX(HSD_JObj* jobj, f32 x)
{
    HSD_ASSERT(0x3B8, jobj);
    jobj->translate_x = x;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetTranslateY(HSD_JObj* jobj, f32 y)
{
    HSD_ASSERT(0x3C6, jobj);
    jobj->translate_y = y;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline void HSD_JObjSetTranslateZ(HSD_JObj* jobj, f32 z)
{
    HSD_ASSERT(0x3D4, jobj);
    jobj->translate_z = z;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

static inline f32* HSD_JObjGetMtxPtr(HSD_JObj* jobj)
{
    HSD_ASSERT(1144, jobj);
    HSD_JObjSetupMatrix(jobj);
    return (f32*)jobj->mtx;
}

#endif /* HSD_JOBJ_H */
