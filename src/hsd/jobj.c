/**
 * @file jobj.c
 * @brief HAL jobj.c: HSD joint objects (JObj), 0x8019CE50 - 0x801A3E64.
 *
 * The whole translation unit, written for the HSD library flags
 * (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
 * -str reuse,readonly), no local pragmas, linked as one Matching unit
 * (2026-09-29) with every function and section at 100%. Its extent:
 *   .text   0x8019CE50 - 0x801A3E64 (initialize.c ends at 0x8019CE50,
 *           list.c starts at 0x801A3E64 with HSD_SListRemove)
 *   .rodata 0x80274AA0 - 0x80274D58 (the IK vectors, then the string pool;
 *           lobj.c's strings follow)
 *   .data   0x8036C8E0 - 0x8036CA1C (hsdJObj, then JObjUpdateFunc's switch
 *           table; lobj.c's hsdLObj follows)
 *   .sbss   0x8047B298 - 0x8047B2B0 (default_class, ufc_callbacks and the
 *           three callbacks, current_jobj)
 *   .sdata2 0x8047DB20 - 0x8047DBA0 (__FILE__ "jobj.c", the short assert
 *           strings and the float pool; list.c's "list.c" follows)
 *
 * It links under title-path rule exceptions (docs/RULE_EXCEPTIONS.md; the
 * TU is on the recomp's title path, e.g. HSD_JObjDispAll and JObjAnimAll in
 * the boot-sequence closure and HSD_JObjSetupMatrixSub / resolveIKJoint1/2 as
 * gs-material-init native functions), each tagged RULE-EXCEPTION(title-path):
 *  - Retail expands HSD_JObjSetMtxDirty into HSD_JObjClearFlags /
 *    HSD_JObjSetFlags and the jobj.h setters but calls the recursive
 *    HSD_JObjSetMtxDirtySub out of line. With a file-scope declaration of
 *    HSD_JObjSetMtxDirtySub (Melee's jobj.h) MWCC refuses to inline any
 *    function whose body calls the self-expanding SetMtxDirtySub, so
 *    ClearFlags/SetFlags(All), JObjUpdateFunc, HSD_JObjAddAnim(All) do not
 *    match. Only a block-scope declaration inside HSD_JObjSetMtxDirty
 *    (sysdolphin/baselib/jobj.h) reproduces retail; it is an exception. The
 *    unit must be deferred-inlined: ReleaseChild at 0x8019D05C expands
 *    HSD_JObjUnref (0x801A05EC) and HSD_JObjReparent, which in
 *    non-deferred mode would have to be defined, and so emitted, before it.
 *  - Retail's pools hold literals of functions it strips and Melee only
 *    keeps as orphaned strings: "jobj[%d,%d]", the flag names and the SRT
 *    labels, "jobj_root" / "jobj_root == NULL", and a "jp" assert pooled
 *    ahead of JObjUpdateFunc's 1.0. Their bodies are unknown.
 *    Melee (jobj.c) and Kirby Air Ride (wowjinxy/KAR, src/sysdolphin/jobj.c)
 *    keep them only as "#pragma force_active" static char arrays, not as
 *    functions. Arrays like that would be objects of their own, outside the
 *    string pool, so they cannot produce retail's pool offsets (for example
 *    JObjLoad's "addi r5,r31,460", which is 140 bytes further into the pool
 *    than without these strings). Three invented stand-in functions
 *    (HSD_JObjStrippedDump, HSD_JObjStrippedRootCheck,
 *    HSD_JObjStrippedPathCheck) reference the strings at retail's points of
 *    the generation order; nothing calls them and the linker strips them.
 *    They are exceptions.
 *  - Unit-wide inline modes do not help (2026-09-27 sweep of this file with
 *    the library flags): "-inline all,deferred", "smart,deferred",
 *    "on,deferred" and "auto,deferred,level=N" for N = 0 and 6-8 give the
 *    same 35 exact functions as the default. Level 1-5 lose between 1 and 17
 *    of them. "noauto,deferred" keeps only 21. The non-deferred modes (all,
 *    or auto with level=N) bring JObjUpdateFunc and HSD_JObjAddAnim closer
 *    but break JObjLoad, the ResolveRefs pair and HSD_JObjAddNext. None of
 *    them expands HSD_JObjClearFlags(All) / HSD_JObjSetFlags(All) as retail
 *    does.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/jobj.c). The functions are written in HAL's order;
 * deferred inlining emits them in reverse, which is retail's address order,
 * and the literal pools come out in first-use order of that reversed code
 * generation. With "-inline auto" every small function, recursive ones
 * included, is expanded a few levels deep; where the expansion stops the
 * compiler calls an out-of-line copy of the header inline, which it emits
 * after the function that needed it (HSD_JObjMtxIsDirty after
 * HSD_JObjSetMtxDirtySub, the object.h reference counters after
 * HSD_JObjResolveRefsAll, ...).
 *
 * Functions retail does not keep, but whose strings it does, are kept for
 * the pool order and stripped by the linker, as in Melee:
 * HSD_JObjSetDefaultClass ("hsdIsDescendantOf(info, &hsdJObj)").
 *
 * Colosseum's HAL version differs from Melee's in these places (all read
 * from retail):
 *  - the class info is 0x54 bytes: JObjInfoInit also installs
 *    JObjUpdateFunc as the class's update method (HSD_JObjInfo.update), and
 *    HSD_JObjAnim / the IK solvers pass that method on;
 *  - JObjLoad resets the links, loads the joint, then loads and appends the
 *    children in order (HSD_JObjAddChild), failing with -1;
 *  - HSD_JObjAddNext unlinks the new sibling and splices it after jobj;
 *  - HSD_JObjDispAll checks the view matrix and the current camera, and
 *    builds the instance matrix in a static helper (JObjSetupInstanceMtx,
 *    JObjSetupInstanceMtx) that the compiler expands at both instance levels it
 *    inlines and emits out of line for the deeper ones (policy: repeated
 *    expansion). As a plain static function it is emitted at its own place,
 *    after the header-inline copies DispAll needs, as in retail;
 *  - fn_8019F1C4 (vertex count of a tree) is new, JObjUpdateFunc does not
 *    clamp the scale, and HSD_JObjAddChild reports through
 *    HSD_ASSERTREPORT;
 *  - assert line numbers are those of Colosseum's longer file.
 *
 * Source-level details the retail code pins down:
 *  - several loops walk the tree through the parameter itself
 *    (HSD_JObjSetMtxDirtySub, HSD_JObjDispAll: the inlined copies move it
 *    into a register of their own), and some tests use the pointer's truth
 *    value; both change how deep MWCC's size-limited auto-inlining expands
 *    the recursive functions (retail: SetMtxDirtySub seven bodies,
 *    ClearFlagsAll / SetFlagsAll / ReqAnimAllByFlags six);
 *  - JObjResetRST / HSD_JObjResetRST sit between HSD_JObjClearFlagsAll and
 *    HSD_JObjAlloc: the out-of-line HSD_JObjSetMtxDirty copy (0x8019F778)
 *    is emitted after its first user, and retail places it before
 *    ClearFlagsAll, after HSD_JObjAlloc; the two are dead and stripped;
 *  - jobj_get_effector_checked takes the parent joint and searches its
 *    children (retail loads jobj->child after resolveIKJoint2's (1,1,1)
 *    copy, through the saved jobj register);
 *  - JObjUpdateFunc walks both callback lists with one function-scope list
 *    variable (it shares its register with jp; type and the pool base keep
 *    their own), calls each callback through the cast list data, and
 *    extracts the particle offset as (iv & 0x3FFFFFC0) >> 6.
 *
 * The symbols keep their address names: lbl_8036C8E0 is hsdJObj,
 * lbl_8047B298 default_class, lbl_8047B29C ufc_callbacks, lbl_8047B2A0
 * dptcl_callback, lbl_8047B2A4 jsound_callback, lbl_8047B2A8
 * ptcltgt_callback, lbl_8047B2AC current_jobj.
 */
/* Symbol names. Where MWCC's auto-inlining stops, it calls out-of-line
 * copies of header inlines that it emits
 * after the function needing them, named after the inline. These defines
 * give those copies the address names the rest of the project and the
 * report use; they change no code. object.h's ref_INC_nocheck keeps its
 * name (symbols.txt, 0x801A0C1C): object.h's NULL-checking ref_INC also
 * exists in this TU, so no define can rename one without the other. */
#define HSD_JObjMtxIsDirty fn_8019D980
#define HSD_JObjSetMtxDirty HSD_JObjSetMtxDirty
#define iref_INC iref_INC_801A0C9C
#define hsdDelete hsdDelete_801A0CE8
#define iref_CNT fn_801A0D3C
#define ref_DEC ref_DEC_801A0D48
#define HSD_CObjGetViewingMtxPtrDirect fn_801A1980
#define HSD_JObjSetupMatrix fn_801A1988

#include "crt/math_ppc.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_id.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_robj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/mtx.h"

#define M_PI 3.14159265358979323846

/* class.c */
extern HSD_ClassInfo* fn_80193748(const char* class_name);
extern BOOL hsdIsDescendantOf(void* info, void* p);
extern void* fn_80193828(HSD_ClassInfo* info);
#define hsdSearchClassInfo(n) fn_80193748(n)
#define hsdIsDescendantOf(i, p) hsdIsDescendantOf(i, p)
#define hsdNew(i) fn_80193828(i)

/* robj.c */
extern void fn_801AE50C(HSD_RObj* robj);
extern void fn_801AEBE4(HSD_RObj* robj, HSD_RObjDesc* desc);
extern void HSD_RObjUpdateAll(HSD_RObj* robj, void* obj, HSD_ObjUpdateFunc func);
extern BOOL HSD_RObjGetGlobalPosition(HSD_RObj* robj, u32 type, Vec3* pos);
extern void HSD_RObjReqAnimAllByFlags(HSD_RObj* robj, f32 frame, u32 flags);
extern void fn_801B0040(HSD_RObj* robj);
#define HSD_RObjRemoveAll fn_801AE50C
#define HSD_RObjResolveRefsAll fn_801AEBE4
#define HSD_RObjUpdateAll HSD_RObjUpdateAll
#define HSD_RObjGetGlobalPosition HSD_RObjGetGlobalPosition
#define HSD_RObjAddAnimAll HSD_RObjAddAnimAll
#define HSD_RObjReqAnimAllByFlags HSD_RObjReqAnimAllByFlags
#define HSD_RObjAnimAll fn_801B0040

/* displayfunc.c */
extern void HSD_JObjDisp(HSD_JObj* jobj, MtxPtr vmtx, u32 flags,
                        u32 rendermode);
extern void HSD_JObjDispDObj(HSD_JObj* jobj, MtxPtr vmtx, MtxPtr pmtx,
                        HSD_TrspMask trsp_mask, u32 rendermode);
extern void HSD_JObjMakePositionMtx(HSD_JObj* jobj, MtxPtr vmtx,
                                    MtxPtr pmtx);
#define HSD_JObjDisp HSD_JObjDisp
#define HSD_JObjDispSub HSD_JObjDispDObj

/* mtx.c */
extern void* HSD_VecAlloc(void);
extern void HSD_VecFree(void* vec);
extern MtxPtr HSD_MtxAlloc(void);
extern void HSD_MtxFree(MtxPtr mtx);
extern void HSD_MtxGetTranslate(Mtx m, Vec3* vec);
extern void HSD_MtxSRT(Mtx m, Vec3* scale, Vec3* rotate, Vec3* translate,
                       Vec3* parent_scale);
extern void HSD_MtxSRTQuat(Mtx m, Vec3* scale, Quaternion* rotate,
                           Vec3* translate, Vec3* parent_scale);
extern void fn_801A9DF0(Mtx inv, Mtx src, Mtx dest);
#define HSD_MtxInverseConcat fn_801A9DF0
#define HSD_MtxGetRotation HSD_MtxGetRotation

/* spline.c */
extern void splArcLengthPoint(Vec3* p, HSD_Spline* spline, f32 u);

extern void* memcpy(void* dst, const void* src, u32 size);

extern f32 PSVECDotProduct(const Vec* a, const Vec* b);
extern void PSVECScale(const Vec* src, Vec* dst, f32 scale);
extern void PSVECAdd(const Vec* a, const Vec* b, Vec* ab);
extern void PSVECSubtract(const Vec* a, const Vec* b, Vec* ab);
extern void PSVECNormalize(const Vec* src, Vec* unit);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* axb);
extern void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad);
extern void PSMTXMultVec(const Mtx m, const Vec* src, Vec* dst);

typedef void (*HSD_JObjWalkTreeCallback)(HSD_JObj* jobj, void* args,
                                         u32 type);
typedef void (*DPCtlCallback)(int, int lo, int hi, HSD_JObj* jobj);
typedef void (*ufc_callback)(HSD_JObj* jobj, u32 type, f32 val);

void JObjInfoInit(void); /* JObjInfoInit */
HSD_JObjInfo lbl_8036C8E0 = { JObjInfoInit };
#define hsdJObj lbl_8036C8E0

HSD_JObj* lbl_8047B2AC;
void (*lbl_8047B2A8)(HSD_JObj* jobj, s32 val);
void (*lbl_8047B2A4)(s32 val);
DPCtlCallback lbl_8047B2A0;
HSD_SList* lbl_8047B29C;
HSD_ClassInfo* lbl_8047B298;
#define default_class lbl_8047B298
#define ufc_callbacks lbl_8047B29C
#define dptcl_callback lbl_8047B2A0
#define jsound_callback lbl_8047B2A4
#define ptcltgt_callback lbl_8047B2A8
#define current_jobj lbl_8047B2AC

/* id.h */
static inline void* HSD_IDGetData(u32 id, s32* success)
{
    return HSD_IDGetDataFromTable(NULL, id, success);
}

/* Functions whose symbols keep their address names. */
void HSD_JObjUnref(HSD_JObj* jobj);
void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags);
void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags);
void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags);
void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags);
HSD_JObj* HSD_JObjAlloc(void);
#define HSD_JObjUnref HSD_JObjUnref
#define HSD_JObjSetFlags HSD_JObjSetFlags
#define HSD_JObjSetFlagsAll HSD_JObjSetFlagsAll
#define HSD_JObjClearFlags HSD_JObjClearFlags
#define HSD_JObjClearFlagsAll HSD_JObjClearFlagsAll
#define HSD_JObjAlloc HSD_JObjAlloc

void HSD_JObjAddChild(HSD_JObj* jobj, HSD_JObj* child);
void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint);
void HSD_JObjWalkTree0(HSD_JObj* jobj, HSD_JObjWalkTreeCallback cb,
                       void* cb_args);
HSD_JObj* HSD_JObjGetPrev(HSD_JObj* jobj);
void HSD_JObjAnimCallback(void* obj, u32 type, HSD_ObjData* val); /* JObjUpdateFunc */
s32 JObjLoad(HSD_JObj* jobj, HSD_Joint* joint, HSD_JObj* parent);
void JObjReleaseChild(HSD_JObj* jobj);

/* HSD_JObjCheckDepend */
void fn_801A3D04(HSD_JObj* jobj)
{
    if (jobj == NULL || HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    if (jobj->flags & JOBJ_USER_DEF_MTX) {
        if (!(jobj->flags & JOBJ_MTX_INDEP_PARENT) && jobj->parent != NULL &&
            HSD_JObjMtxIsDirty(jobj->parent))
        {
            jobj->flags |= JOBJ_MTX_DIRTY;
        }
    } else if ((jobj->parent != NULL &&
                (jobj->parent->flags & JOBJ_MTX_DIRTY)) ||
               (jobj->flags & JOBJ_EFFECTOR) == JOBJ_JOINT1 ||
               (jobj->flags & JOBJ_EFFECTOR) == JOBJ_JOINT2 ||
               (jobj->flags & JOBJ_EFFECTOR) == JOBJ_EFFECTOR ||
               jobj->robj != NULL)
    {
        jobj->flags |= JOBJ_MTX_DIRTY;
    }
}

void HSD_JObjWalkTree0(HSD_JObj* jobj, HSD_JObjWalkTreeCallback cb,
                       void* cb_args)
{
    u32 type;

    if (jobj == NULL) {
        return;
    }
    HSD_ASSERT(173, jobj->parent);
    type = jobj->parent->child == jobj ? 1 : 2;
    if (cb != NULL) {
        cb(jobj, cb_args, type);
    }
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        HSD_JObj* child = jobj->child;
        while (child != NULL) {
            HSD_JObjWalkTree0(child, cb, cb_args);
            child = child->next;
        }
    }
}

/* HSD_JObjWalkTree */
void HSD_JObjWalkTree(HSD_JObj* jobj, HSD_JObjWalkTreeCallback cb, void* cb_args)
{
    if (jobj == NULL) {
        return;
    }
    if (cb != NULL) {
        cb(jobj, cb_args, 0);
    }
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        HSD_JObj* child = jobj->child;
        while (child != NULL) {
            HSD_JObjWalkTree0(child, cb, cb_args);
            child = child->next;
        }
    }
}

/* HSD_JObjMakeMatrix */
void HSD_JObjMakeMatrix(HSD_JObj* jobj)
{
    HSD_JObjSetupMatrix(jobj->parent);
    if (jobj->flags & JOBJ_CLASSICAL_SCALE) {
        if (jobj->parent != NULL && jobj->parent->scl != NULL) {
            if (jobj->scl == NULL) {
                jobj->scl = HSD_VecAlloc();
            }
            *jobj->scl = *jobj->parent->scl;
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
            jobj->scl->x = jobj->scale.x * jobj->parent->scl->x;
            jobj->scl->y = jobj->scale.y * jobj->parent->scl->y;
            jobj->scl->z = jobj->scale.z * jobj->parent->scl->z;
        } else {
            *jobj->scl = jobj->scale;
        }
    }
    if (jobj->flags & JOBJ_USE_QUATERNION) {
        HSD_MtxSRTQuat(jobj->mtx, &jobj->scale, &jobj->rotate,
                       &jobj->translate,
                       (jobj->parent != NULL && jobj->parent->scl != NULL)
                           ? jobj->parent->scl
                           : NULL);
    } else {
        HSD_MtxSRT(jobj->mtx, &jobj->scale, (Vec3*) &jobj->rotate,
                   &jobj->translate,
                   (jobj->parent != NULL && jobj->parent->scl != NULL)
                           ? jobj->parent->scl
                           : NULL);
    }
    if (jobj->parent != NULL) {
        PSMTXConcat(jobj->parent->mtx, jobj->mtx, jobj->mtx);
    }
    if (jobj->aobj != NULL && jobj->aobj->hsd_obj != NULL) {
        Vec3 vec;
        HSD_JObj* aobj_jobj = (HSD_JObj*) jobj->aobj->hsd_obj;
        HSD_JObjSetupMatrix(aobj_jobj);
        PSMTXMultVec(aobj_jobj->mtx, &jobj->translate, &vec);
        jobj->mtx[0][3] = vec.x;
        jobj->mtx[1][3] = vec.y;
        jobj->mtx[2][3] = vec.z;
    }
}

/* HSD_JObjReqAnimByFlags */
void HSD_JObjReqAnimByFlags(HSD_JObj* jobj, u32 flags, f32 frame)
{
    if (jobj) {
        if (flags & 1) {
            HSD_AObjReqAnim(jobj->aobj, frame);
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjReqAnimAllByFlags(jobj->u.dobj, frame, (void*) flags);
        }
        HSD_RObjReqAnimAllByFlags(jobj->robj, frame, flags);
    }
}

/* HSD_JObjReqAnimAllByFlags */
void HSD_JObjReqAnimAllByFlags(HSD_JObj* jobj, u32 flags, f32 frame)
{
    if (jobj) {
        HSD_JObjReqAnimByFlags(jobj, flags, frame);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            HSD_JObj* child = jobj->child;
            while (child) {
                HSD_JObjReqAnimAllByFlags(child, flags, frame);
                child = child->next;
            }
        }
    }
}

void HSD_JObjReqAnimAll(HSD_JObj* jobj, f32 frame)
{
    HSD_JObjReqAnimAllByFlags(jobj, 0x7FF, frame);
}

void HSD_JObjReqAnim(HSD_JObj* jobj, f32 frame)
{
    HSD_JObjReqAnimByFlags(jobj, 0x7FF, frame);
}

/* JObjSortAnim */
void JObjSortAnim(HSD_AObj* aobj)
{
    HSD_FObj** fobj_ptr;

    if (aobj == NULL || aobj->fobj == NULL) {
        return;
    }
    for (fobj_ptr = &aobj->fobj; *fobj_ptr != NULL;
         fobj_ptr = &(*fobj_ptr)->next)
    {
        if ((*fobj_ptr)->obj_type == HSD_A_J_BRANCH) {
            HSD_FObj* next = (*fobj_ptr)->next;
            HSD_FObj* fobj = *fobj_ptr;
            *fobj_ptr = next;
            fobj->next = aobj->fobj;
            aobj->fobj = fobj;
            return;
        }
    }
}

/* HSD_JObjAddAnim */
void HSD_JObjAddAnim(HSD_JObj* jobj, HSD_AnimJoint* an_joint,
                 HSD_MatAnimJoint* mat_joint, HSD_ShapeAnimJoint* sh_joint)
{
    if (jobj != NULL) {
        if (an_joint != NULL) {
            if (jobj->aobj != NULL) {
                HSD_AObjRemove(jobj->aobj);
            }
            jobj->aobj = HSD_AObjLoadDesc(an_joint->aobjdesc);
            JObjSortAnim(jobj->aobj);
            HSD_RObjAddAnimAll(jobj->robj, an_joint->robj_anim);
            if (an_joint->flags & 1) {
                HSD_JObjSetFlags(jobj, JOBJ_CLASSICAL_SCALE);
            } else {
                HSD_JObjClearFlags(jobj, JOBJ_CLASSICAL_SCALE);
            }
        }
        if (union_type_dobj(jobj)) {
            HSD_DObjAddAnimAll(
                jobj->u.dobj, mat_joint != NULL ? mat_joint->matanim : NULL,
                sh_joint != NULL ? sh_joint->shapeanimdobj : NULL);
        }
    }
}

/* HSD_JObjAddAnimAll */
void fn_801A2B5C(HSD_JObj* jobj, HSD_AnimJoint* ajoint,
                 HSD_MatAnimJoint* mjoint, HSD_ShapeAnimJoint* sjoint)
{
    HSD_JObj* jp;
    HSD_AnimJoint* aj;
    HSD_MatAnimJoint* mj;
    HSD_ShapeAnimJoint* sj;

    if (jobj != NULL) {
        HSD_JObjAddAnim(jobj, ajoint, mjoint, sjoint);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            jp = jobj->child;
            aj = ajoint != NULL ? ajoint->child : NULL;
            mj = mjoint != NULL ? mjoint->child : NULL;
            sj = sjoint != NULL ? sjoint->child : NULL;
            while (jp != NULL) {
                fn_801A2B5C(jp, aj, mj, sj);
                jp = jp->next;
                aj = aj != NULL ? aj->next : NULL;
                mj = mj != NULL ? mj->next : NULL;
                sj = sj != NULL ? sj->next : NULL;
            }
        }
    }
}

/* JObjUpdateFunc */
void HSD_JObjAnimCallback(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_JObj* jobj = obj;
    Vec3 p;
    HSD_JObj* jp;
    HSD_RObj* robj;
    Mtx mtx;
    HSD_SList* list;

    if (jobj != NULL) {
        switch (type) {
        case HSD_A_J_PATH:
            if (val->fv < 0.0) {
                val->fv = 0.0F;
            }
            if (1.0 < val->fv) {
                val->fv = 1.0F;
            }
            HSD_ASSERT(590, jobj->aobj);
            jp = (HSD_JObj*) jobj->aobj->hsd_obj;
            HSD_ASSERT(592, jp);
            HSD_ASSERT(593, jp->u.spline);
            splArcLengthPoint(&p, jp->u.spline, val->fv);
            HSD_JObjSetTranslateX(jobj, p.x);
            HSD_JObjSetTranslateY(jobj, p.y);
            HSD_JObjSetTranslateZ(jobj, p.z);
            break;
        case HSD_A_J_ROTX:
            if (jobj->flags & JOBJ_JOINT1) {
                robj = HSD_RObjGetByType(jobj->robj, REFTYPE_IKHINT, 0);
                if (robj != NULL) {
                    robj->u.ik_hint.rotate_x = val->fv;
                }
            }
            HSD_JObjSetRotationX(jobj, val->fv);
            break;
        case HSD_A_J_ROTY:
            HSD_JObjSetRotationY(jobj, val->fv);
            break;
        case HSD_A_J_ROTZ:
            HSD_JObjSetRotationZ(jobj, val->fv);
            break;
        case HSD_A_J_TRAX:
            HSD_JObjSetTranslateX(jobj, val->fv);
            break;
        case HSD_A_J_TRAY:
            HSD_JObjSetTranslateY(jobj, val->fv);
            break;
        case HSD_A_J_TRAZ:
            HSD_JObjSetTranslateZ(jobj, val->fv);
            break;
        case HSD_A_J_SCAX:
            HSD_JObjSetScaleX(jobj, val->fv);
            break;
        case HSD_A_J_SCAY:
            HSD_JObjSetScaleY(jobj, val->fv);
            break;
        case HSD_A_J_SCAZ:
            HSD_JObjSetScaleZ(jobj, val->fv);
            break;
        case HSD_A_J_BRANCH:
            if (val->fv > 0.5) {
                HSD_JObjClearFlagsAll(jobj, JOBJ_HIDDEN);
            } else {
                HSD_JObjSetFlagsAll(jobj, JOBJ_HIDDEN);
            }
            break;
        case HSD_A_J_NODE:
            if (val->fv > 0.5) {
                HSD_JObjClearFlags(jobj, JOBJ_HIDDEN);
            } else {
                HSD_JObjSetFlags(jobj, JOBJ_HIDDEN);
            }
            break;
        case HSD_A_J_SETBYTE0:
        case HSD_A_J_SETBYTE1:
        case HSD_A_J_SETBYTE2:
        case HSD_A_J_SETBYTE3:
        case HSD_A_J_SETBYTE4:
        case HSD_A_J_SETBYTE5:
        case HSD_A_J_SETBYTE6:
        case HSD_A_J_SETBYTE7:
        case HSD_A_J_SETBYTE8:
        case HSD_A_J_SETBYTE9: {
            list = ufc_callbacks;
            while (list != NULL) {
                ((ufc_callback) list->data)(jobj, type, val->iv);
                list = list->next;
            }
            break;
        }
        case HSD_A_J_SETFLOAT0:
        case HSD_A_J_SETFLOAT1:
        case HSD_A_J_SETFLOAT2:
        case HSD_A_J_SETFLOAT3:
        case HSD_A_J_SETFLOAT4:
        case HSD_A_J_SETFLOAT5:
        case HSD_A_J_SETFLOAT6:
        case HSD_A_J_SETFLOAT7:
        case HSD_A_J_SETFLOAT8:
        case HSD_A_J_SETFLOAT9: {
            list = ufc_callbacks;
            while (list != NULL) {
                ((ufc_callback) list->data)(jobj, type, val->fv);
                list = list->next;
            }
            break;
        }
        case 0x28: {
            s32 lo = val->iv & 0x3F;
            s32 hi = (val->iv & 0x3FFFFFC0) >> 6;
            if (dptcl_callback != NULL) {
                dptcl_callback(0, lo, hi, jobj);
            }
        } break;
        case 0x29:
            if (jsound_callback != NULL) {
                jsound_callback(val->iv);
            }
            break;
        case 0x2A:
            if (ptcltgt_callback != NULL) {
                ptcltgt_callback(jobj, val->iv);
            }
            break;
        case 0x32:
            jobj->mtx[0][0] = val->p.x;
            jobj->mtx[1][0] = val->p.y;
            jobj->mtx[2][0] = val->p.z;
            break;
        case 0x33:
            jobj->mtx[0][1] = val->p.x;
            jobj->mtx[1][1] = val->p.y;
            jobj->mtx[2][1] = val->p.z;
            break;
        case 0x34:
            jobj->mtx[0][2] = val->p.x;
            jobj->mtx[1][2] = val->p.y;
            jobj->mtx[2][2] = val->p.z;
            break;
        case 0x35:
            jobj->mtx[0][3] = val->p.x;
            jobj->mtx[1][3] = val->p.y;
            jobj->mtx[2][3] = val->p.z;
            break;
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
            if (jobj->parent != NULL) {
                HSD_MtxInverseConcat(jobj->parent->mtx, jobj->mtx, mtx);
            } else {
                PSMTXCopy(jobj->mtx, mtx);
            }
            if (type == 0x36U || type == 0x38U) {
                HSD_MtxGetTranslate(mtx, &jobj->translate);
            }
            if (type == 0x36 || type == 0x37) {
                HSD_MtxGetRotation(mtx, (Vec3*) &jobj->rotate);
            }
            if (type == 0x36U || type == 0x39U) {
                HSD_MtxGetScale(mtx, &jobj->scale);
            }
            break;
        }
    }
}

/* RULE-EXCEPTION(title-path): invented stand-in for a function retail
 * strips - see docs/RULE_EXCEPTIONS.md. Retail's .sdata2 pools the "jp"
 * assert string ahead of JObjUpdateFunc's 1.0, so a function generated
 * before JObjUpdateFunc used it first; its body is unknown (Melee and KAR
 * keep none). This one only references the string at that point of the
 * generation order; nothing calls it and the linker strips it. */
void HSD_JObjStrippedPathCheck(HSD_JObj* jp)
{
    HSD_ASSERT(592, jp);
}

/* HSD_JObjAnim */
void HSD_JObjAnim(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        fn_801A3D04(jobj);
        HSD_AObjInterpretAnim(jobj->aobj, jobj, HSD_JOBJ_METHOD(jobj)->update);
        HSD_RObjAnimAll(jobj->robj);
        if (union_type_dobj(jobj)) {
            HSD_DObjAnimAll(jobj->u.dobj);
        }
    }
}

/* JObjAnimAll */
void fn_801A1B7C(HSD_JObj* jobj)
{
    HSD_JObj* child;
    if (jobj != NULL) {
        HSD_JObjAnim(jobj);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            child = jobj->child;
            while (child != NULL) {
                fn_801A1B7C(child);
                child = child->next;
            }
        }
    }
}

void HSD_JObjAnimAll(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        HSD_AObjInitEndCallBack();
        fn_801A1B7C(jobj);
        HSD_AObjInvokeCallBacks();
    }
}

/* The viewing matrix of an instance joint: the instance's matrix relative
 * to the instanced tree's root, in front of vmtx or else the current
 * camera's viewing matrix. HSD_JObjDispAll expands it at both instance
 * levels it inlines. */
/* JObjSetupInstanceMtx */
static void JObjSetupInstanceMtx(MtxPtr vmtx, HSD_JObj* jobj, Mtx mtx)
{
    HSD_CObj* cobj;

    HSD_JObjSetupMatrix(jobj);
    HSD_JObjSetupMatrix(jobj->child);
    PSMTXInverse(jobj->child->mtx, mtx);
    PSMTXConcat(jobj->mtx, mtx, mtx);
    if (vmtx) {
        PSMTXConcat(vmtx, mtx, mtx);
    } else {
        cobj = HSD_CObjGetCurrent();
        if (cobj != NULL) {
            PSMTXConcat((MtxPtr) HSD_CObjGetViewingMtxPtrDirect(cobj), mtx,
                        mtx);
        }
    }
}

/* HSD_JObjDispAll */
void HSD_JObjDispAll(HSD_JObj* jobj, MtxPtr vmtx, u32 flags, u32 rendermode)
{
    if (jobj != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            if (!(jobj->flags & JOBJ_HIDDEN)) {
                Mtx mtx;

                JObjSetupInstanceMtx(vmtx, jobj, mtx);
                HSD_JObjDispAll(jobj->child, mtx, flags, rendermode);
            }
        } else {
            if (jobj->flags & (flags << JOBJ_TRSP_SHIFT)) {
                HSD_JObjDisp(jobj, vmtx, flags, rendermode);
            }
            if (jobj->flags & (flags << 28)) {
                jobj = jobj->child;
                while (jobj != NULL) {
                    HSD_JObjDispAll(jobj, vmtx, flags, rendermode);
                    jobj = jobj->next;
                }
            }
        }
    }
}

void HSD_JObjSetDefaultClass(HSD_ClassInfo* info)
{
    if (info != NULL) {
        HSD_ASSERT(933, hsdIsDescendantOf(info, &hsdJObj));
    }
    default_class = info;
}

/* RULE-EXCEPTION(title-path): invented stand-in for a function retail
 * strips - see docs/RULE_EXCEPTIONS.md. Retail's string pool keeps
 * "jobj_root" and "jobj_root == NULL" (Melee: force_active arrays before
 * HSD_JObjResolveRefs) between "jobj->child" and HSD_JObjSetDefaultClass's
 * assert; this function puts them there. Nothing calls it and the linker
 * strips it. */
void HSD_JObjStrippedRootCheck(HSD_JObj* jobj_root)
{
    HSD_ASSERT(0, jobj_root);
    HSD_ASSERT(0, jobj_root == NULL);
}

static inline HSD_JObj* JObjLoadJointSub(HSD_Joint* joint, HSD_JObj* parent)
{
    HSD_JObj* jobj;
    HSD_ClassInfo* info;

    if (joint == NULL) {
        return NULL;
    }
    if (joint->class_name == NULL ||
        !(info = hsdSearchClassInfo(joint->class_name)))
    {
        jobj = HSD_JObjAlloc();
    } else {
        jobj = hsdNew(info);
        HSD_ASSERT(981, jobj);
    }
    HSD_JOBJ_METHOD(jobj)->load(jobj, joint, parent);
    return jobj;
}

s32 JObjLoad(HSD_JObj* jobj, HSD_Joint* joint, HSD_JObj* parent)
{
    jobj->child = NULL;
    jobj->next = NULL;
    jobj->parent = NULL;
    jobj->flags |= joint->flags;
    if (union_type_spline(jobj)) {
        jobj->u.spline = joint->u.spline;
    } else if (union_type_ptcl(jobj)) {
        HSD_SList* slist;
        jobj->u.ptcl = joint->u.ptcl;
        slist = joint->u.ptcl;
        while (slist != NULL) {
            *(u32*) &slist->data |= 0x80000000;
            slist = slist->next;
        }
    } else {
        jobj->u.dobj = HSD_DObjLoadDesc(joint->u.dobjdesc);
    }
    jobj->robj = HSD_RObjLoadDesc(joint->robjdesc);
    jobj->rotate.x = joint->rotation.x;
    jobj->rotate.y = joint->rotation.y;
    jobj->rotate.z = joint->rotation.z;
    jobj->scale = joint->scale;
    jobj->translate = joint->position;
    PSMTXIdentity(jobj->mtx);
    jobj->scl = NULL;
    if (joint->mtx != NULL) {
        jobj->envelopemtx = HSD_MtxAlloc();
        memcpy(jobj->envelopemtx, joint->mtx, sizeof(Mtx));
    }
    HSD_IDInsertToTable(NULL, (u32) joint, jobj);
    jobj->id = (u32) joint;
    if (!(joint->flags & JOBJ_INSTANCE)) {
        HSD_Joint* child_joint;
        for (child_joint = joint->child; child_joint != NULL;
             child_joint = child_joint->next)
        {
            HSD_JObj* child = JObjLoadJointSub(child_joint, jobj);
            if (child == NULL) {
                return -1;
            }
            HSD_JObjAddChild(jobj, child);
        }
    }
    return 0;
}

HSD_JObj* HSD_JObjLoadJoint(HSD_Joint* joint)
{
    HSD_JObj* jobj = JObjLoadJointSub(joint, NULL);
    HSD_JObjResolveRefsAll(jobj, joint);
    return jobj;
}

void HSD_JObjResolveRefs(HSD_JObj* jobj, HSD_Joint* joint)
{
    if (jobj == NULL || joint == NULL) {
        return;
    }

    HSD_RObjResolveRefsAll(jobj->robj, joint->robjdesc);
    if (jobj->flags & JOBJ_INSTANCE) {
        HSD_JObjUnref(jobj->child);
        jobj->child = HSD_IDGetData((u32) joint->child, NULL);
        HSD_ASSERT(1119, jobj->child);
        HSD_JObjRef(jobj->child);
    }
    if (union_type_dobj(jobj)) {
        HSD_DObjResolveRefsAll(jobj->u.dobj, joint->u.dobjdesc);
    }
}

void HSD_JObjResolveRefsAll(HSD_JObj* jobj, HSD_Joint* joint)
{
    while (jobj != NULL && joint != NULL) {
        HSD_JObjResolveRefs(jobj, joint);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            HSD_JObjResolveRefsAll(jobj->child, joint->child);
        }
        jobj = jobj->next;
        joint = joint->next;
    }
}

/* HSD_JObjUnref */
void HSD_JObjUnref(HSD_JObj* jobj)
{
    if (jobj != NULL && ref_DEC(jobj)) {
        if (iref_CNT(jobj) < 0) {
            hsdDelete(jobj);
        } else {
            iref_INC(jobj);
            HSD_JOBJ_METHOD(jobj)->release_child(jobj);
            if (iref_DEC(jobj)) {
                hsdDelete(jobj);
            }
        }
    }
}

void HSD_JObjUnrefThis(HSD_JObj* jobj)
{
    if (jobj != NULL && iref_DEC(jobj) && ref_CNT(jobj) < 0) {
        hsdDelete(jobj);
    }
}

/* HSD_JObjRemove */
HSD_JObj* fn_801A02B0(HSD_JObj* jobj)
{
    HSD_JObj* child;
    HSD_JObj* next;
    HSD_JObj* prev;

    if (jobj == NULL) {
        return NULL;
    }
    child = jobj->child;
    if (child != NULL) {
        HSD_ASSERT(1227, child->next == NULL);
    }

    next = child != NULL ? child : jobj->next;

    prev = HSD_JObjGetPrev(jobj);
    if (prev != NULL) {
        prev->next = next;
    } else if (jobj->parent != NULL) {
        jobj->parent->child = next;
    }
    if (next != NULL && next == child) {
        next->next = jobj->next;
        next->parent = jobj->parent;
    }
    jobj->parent = NULL;
    jobj->child = NULL;
    jobj->next = NULL;
    HSD_JObjUnref(jobj);
    return child;
}

void HSD_JObjRemoveAll(HSD_JObj* jobj)
{
    HSD_JObj* prev;
    HSD_JObj* next;

    if (jobj == NULL) {
        return;
    }
    if (jobj->parent != NULL) {
        prev = HSD_JObjGetPrev(jobj);
        if (prev != NULL) {
            prev->next = NULL;
        } else {
            jobj->parent->child = NULL;
        }
    }
    while (jobj != NULL) {
        next = jobj->next;
        jobj->parent = NULL;
        jobj->next = NULL;
        HSD_JObjUnref(jobj);
        jobj = next;
    }
}

void RecalcParentTrspBits(HSD_JObj* jobj)
{
    while (jobj != NULL) {
        HSD_JObj* child = jobj->child;
        u32 flags = ~JOBJ_ROOT_MASK;
        while (child != NULL) {
            flags |= (child->flags | child->flags << 10) & JOBJ_ROOT_MASK;
            child = child->next;
        }
        if (!(jobj->flags & ~flags)) {
            break;
        }
        jobj->flags &= flags;
        jobj = jobj->next;
    }
}

static void UpdateParentTrspBits(HSD_JObj* jobj, HSD_JObj* child)
{
    u32 flags = (child->flags | (child->flags << 10)) & JOBJ_ROOT_MASK;
    while (jobj != NULL) {
        if (!(flags & ~jobj->flags)) {
            break;
        }
        jobj->flags |= flags;
        jobj = jobj->parent;
    }
}

void HSD_JObjAddChild(HSD_JObj* jobj, HSD_JObj* child)
{
    HSD_JObj* last;

    if (jobj == NULL || child == NULL) {
        return;
    }
    HSD_ASSERTREPORT(1362, child->parent == NULL,
                     "child should be a orphan.\n");
    HSD_ASSERTREPORT(1363, child->next == NULL,
                     "child should not have siblings");
    if (jobj->child == NULL) {
        jobj->child = child;
    } else {
        HSD_ASSERT(1369, !(jobj->flags & JOBJ_INSTANCE));
        last = jobj->child;
        while (last->next != NULL) {
            HSD_ASSERT(1372, last != child);
            last = last->next;
        }
        last->next = child;
    }
    child->parent = jobj;
    UpdateParentTrspBits(jobj, child);
}

HSD_JObj* HSD_JObjReparent(HSD_JObj* jobj, HSD_JObj* parent)
{
    HSD_JObj* next;

    if (jobj == NULL) {
        return NULL;
    }
    next = jobj->next;
    if (jobj->parent != NULL) {
        if (jobj->parent->child == jobj) {
            jobj->parent->child = next;
        } else {
            HSD_JObj* prev = HSD_JObjGetPrev(jobj);
            HSD_ASSERT(1403, prev);
            prev->next = next;
        }
        RecalcParentTrspBits(jobj->parent);
        jobj->parent = NULL;
    }
    jobj->next = NULL;
    HSD_JObjAddChild(parent, jobj);
    return next;
}

/* RULE-EXCEPTION(title-path): invented stand-in for a function retail
 * strips - see docs/RULE_EXCEPTIONS.md. Retail's string pool keeps a
 * stripped JObj dump function's literals (Melee and KAR keep them only as
 * force_active arrays) between object.h's NOREF assert and
 * HSD_JObjAddChild's "child should be a orphan.\n". Arrays would be
 * objects of their own; this function puts them in the pool, in retail's
 * order, at that point of the generation order. Nothing calls it and the
 * linker strips it. */
void HSD_JObjStrippedDump(void)
{
    OSReport("jobj[%d,%d]");
    OSReport("SKELETON_ROOT ");
    OSReport("SKELETON ");
    OSReport("ENVELOPE_MODEL ");
    OSReport("EFFECTOR ");
    OSReport("  rot(L): ");
    OSReport("  sca(L): ");
    OSReport("  tra(L): ");
    OSReport("  rot(G): ");
    OSReport("  sca(G): ");
    OSReport("  tra(G): ");
}

void HSD_JObjAddNext(HSD_JObj* jobj, HSD_JObj* next)
{
    if (jobj == NULL || next == NULL) {
        return;
    }
    HSD_JObjReparent(next, NULL);
    next->parent = jobj->parent;
    next->next = jobj->next;
    jobj->next = next;
    if (jobj->parent != NULL) {
        UpdateParentTrspBits(jobj->parent, next);
    }
}

HSD_JObj* HSD_JObjGetPrev(HSD_JObj* jobj)
{
    HSD_JObj* cur;

    if (jobj == NULL || jobj->parent == NULL) {
        return NULL;
    }
    if (jobj == jobj->parent->child) {
        return NULL;
    }
    cur = jobj->parent->child;
    while (cur != NULL) {
        if (cur->next == jobj) {
            return cur;
        }
        cur = cur->next;
    }
    HSD_Panic(__FILE__, 1528,
              "can not find specified jobj. maybe jobj tree is broken.\n");
    return NULL;
}

/* HSD_JObjGetDObj */
HSD_DObj* fn_8019FF48(HSD_JObj* jobj)
{
    if (jobj == NULL || !union_type_dobj(jobj)) {
        return NULL;
    }
    return jobj->u.dobj;
}

void HSD_JObjAddDObj(HSD_JObj* jobj, HSD_DObj* dobj)
{
    if (jobj == NULL || dobj == NULL || !union_type_dobj(jobj)) {
        return;
    }
    dobj->next = jobj->u.dobj;
    jobj->u.dobj = dobj;
}

static inline HSD_RObj* robj_set_next(HSD_RObj* robj, HSD_RObj* next)
{
    if (robj == NULL) {
        return next;
    }
    robj->next = next;
    return robj;
}

void HSD_JObjPrependRObj(HSD_JObj* jobj, HSD_RObj* robj)
{
    if (jobj == NULL || robj == NULL) {
        return;
    }
    jobj->robj = robj_set_next(robj, jobj->robj);
}

void HSD_JObjDeleteRObj(HSD_JObj* jobj, HSD_RObj* robj)
{
    if (jobj == NULL || robj == NULL) {
        return;
    }
    if (robj != NULL) {
        HSD_RObj** cur_ptr = &jobj->robj;
        HSD_RObj* cur;
        while (*cur_ptr != NULL) {
            cur = *cur_ptr;
            if (cur == robj) {
                *cur_ptr = cur->next;
                robj->next = NULL;
                return;
            }
            cur_ptr = &cur->next;
        }
    }
}

u32 HSD_JObjGetFlags(HSD_JObj* jobj)
{
    if (jobj != NULL) {
        return jobj->flags;
    }
    return 0;
}

/* HSD_JObjSetFlags */
void HSD_JObjSetFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj) {
        if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
            HSD_JObjSetMtxDirty(jobj);
        }
        jobj->flags |= flags;
    }
}

/* HSD_JObjSetFlagsAll */
void HSD_JObjSetFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj) {
        HSD_JObjSetFlags(jobj, flags);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            HSD_JObj* i;
            for (i = jobj->child; i; i = i->next) {
                HSD_JObjSetFlagsAll(i, flags);
            }
        }
    }
}

/* HSD_JObjClearFlags */
void HSD_JObjClearFlags(HSD_JObj* jobj, u32 flags)
{
    if (jobj) {
        if ((jobj->flags ^ flags) & JOBJ_CLASSICAL_SCALE) {
            HSD_JObjSetMtxDirty(jobj);
        }
        jobj->flags &= ~flags;
    }
}

/* HSD_JObjClearFlagsAll */
void HSD_JObjClearFlagsAll(HSD_JObj* jobj, u32 flags)
{
    if (jobj) {
        HSD_JObjClearFlags(jobj, flags);
        if (!(jobj->flags & JOBJ_INSTANCE)) {
            HSD_JObj* i;
            for (i = jobj->child; i; i = i->next) {
                HSD_JObjClearFlagsAll(i, flags);
            }
        }
    }
}

void JObjResetRST(HSD_JObj* jobj, HSD_Joint* joint)
{
    if (jobj == NULL || joint == NULL) {
        return;
    }
    jobj->rotate.x = joint->rotation.x;
    jobj->rotate.y = joint->rotation.y;
    jobj->rotate.z = joint->rotation.z;
    jobj->scale = joint->scale;
    jobj->translate = joint->position;
    if (!(jobj->flags & JOBJ_MTX_INDEP_SRT)) {
        HSD_JObjSetMtxDirty(jobj);
    }
}

void HSD_JObjResetRST(HSD_JObj* jobj, HSD_Joint* joint)
{
    if (jobj == NULL || joint == NULL) {
        return;
    }
    JObjResetRST(jobj, joint);
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        HSD_JObj* child_jobj = jobj->child;
        HSD_Joint* child_joint = joint->child;
        while (child_jobj != NULL) {
            HSD_JObjResetRST(child_jobj, child_joint);
            child_jobj = child_jobj->next;
            child_joint = child_joint != NULL ? child_joint->next : NULL;
        }
    }
}

/* HSD_JObjAlloc */
HSD_JObj* HSD_JObjAlloc(void)
{
    HSD_JObj* jobj =
        hsdNew(default_class != NULL ? default_class : &hsdJObj.parent.parent);
    HSD_ASSERT(2015, jobj);
    return jobj;
}

/* Counts the vertices of the tree's visible DObjs (both totals of
 * HSD_DObjCountVertices); an instance counts its instanced tree. */
void fn_8019F1C4(HSD_JObj* jobj, s32* total_a, s32* total_b)
{
    HSD_JObj* child;
    s32 sum_a = 0;
    s32 sum_b = 0;

    if (jobj != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            fn_8019F1C4(jobj->child, &sum_a, &sum_b);
        } else {
            if (!(jobj->flags & JOBJ_HIDDEN) && union_type_dobj(jobj)) {
                HSD_DObjCountVertices(jobj->u.dobj, &sum_a, &sum_b);
            }
            for (child = jobj->child; child != NULL; child = child->next) {
                s32 child_a;
                s32 child_b;
                fn_8019F1C4(child, &child_a, &child_b);
                sum_a += child_a;
                sum_b += child_b;
            }
        }
    }
    if (total_a) {
        *total_a = sum_a;
    }
    if (total_b) {
        *total_b = sum_b;
    }
}

/* HSD_JObjSetCurrent */
void HSD_JObjSetCurrent(HSD_JObj* jobj)
{
    HSD_JObjRef(jobj);
    HSD_JObjUnref(current_jobj);
    current_jobj = jobj;
}

HSD_JObj* HSD_JObjGetCurrent(void)
{
    return current_jobj;
}

static inline HSD_JObj* jobj_get_joint2(HSD_JObj* jobj)
{
    while (jobj != NULL) {
        if ((jobj->flags & JOBJ_EFFECTOR) == JOBJ_JOINT2) {
            return jobj;
        }
        jobj = jobj->next;
    }
    return NULL;
}

static inline HSD_JObj* jobj_get_effector(HSD_JObj* jobj)
{
    while (jobj != NULL) {
        if ((jobj->flags & JOBJ_EFFECTOR) == JOBJ_EFFECTOR) {
            return jobj;
        }
        jobj = jobj->next;
    }
    return NULL;
}

HSD_JObj* jobj_get_effector_checked(HSD_JObj* jobj)
{
    HSD_JObj* eff = jobj_get_effector(jobj->child);
    HSD_ASSERT(2093, eff);
    if (HSD_RObjGetByType(eff->robj, REFTYPE_JOBJ, 1) != NULL) {
        return eff;
    } else {
        return NULL;
    }
}

extern const Vec3 lbl_80274AAC;
extern const Vec3 lbl_80274AB8;

/* resolveIKJoint1 keeps single variables where Melee's decompiler split
 * them (the joint-2 length is reused for the signed height, (a - b)^2 and
 * the sqrt products are written in place), tests the literal 1e-8f,
 * re-reads jobj->scl and jobj->parent, and copies an unused (0, 0, 1)
 * vector (retail only stores it). Local declarations are ordered to give
 * retail's register assignment. */
void resolveIKJoint1(HSD_JObj* jobj)
{
    Vec3 spBC = { 1.0F, 1.0F, 1.0F };
    Vec3 spB0;
    Vec3 unused;
    Vec3 sp98;
    Vec3 sp8C;
    Vec3 sp80;
    Vec3 sp74;
    Vec3 sp68;
    Vec3 sp5C;
    Vec3 sp50;
    Mtx sp20;
    f32 temp_f30;
    f32 var_f29;
    f32 temp_f31;
    f32 var_f28;
    f32 var_f27;
    f32 temp_f26;
    f32 temp_f5;
    f32 temp_f5_2;
    f32 var_f1;
    f32 var_f4;
    f32 var_f4_2;
    f32 var_f4_4;
    HSD_IKHint* new_var;
    HSD_JObj* var_r28;
    s32 var_r30;
    HSD_JObj* var_r31;
    HSD_RObj* robj;
    var_r30 = 0;
    var_f29 = 0.0F;
    var_r31 = jobj_get_joint2(jobj->child);
    spB0 = lbl_80274AAC;
    unused = lbl_80274AB8;
    if (jobj->scl != NULL) {
        spBC = *jobj->scl;
    }
    robj = HSD_RObjGetByType(jobj->robj, REFTYPE_IKHINT, 0);
    HSD_ASSERT(2140, robj);
    new_var = &robj->u.ik_hint;
    temp_f26 = new_var->rotate_x;
    temp_f30 = new_var->bone_length * spBC.x;
    if (var_r31 != NULL) {
        robj = HSD_RObjGetByType(var_r31->robj, REFTYPE_IKHINT, 0);
        HSD_ASSERT(2151, robj);
        var_f29 = robj->u.ik_hint.bone_length * var_r31->scale.x * spBC.x;
        var_r30 = robj->flags & 4 ? 1 : 0;
        var_r28 = jobj_get_effector_checked(var_r31);
    } else {
        var_r28 = jobj_get_effector_checked(jobj);
    }
    if (var_r28 != NULL) {
        if ((HSD_RObjGetByType(jobj->robj, REFTYPE_JOBJ, 3) == NULL) &&
            (jobj != NULL))
        {
            if (jobj->robj != NULL) {
                HSD_RObjUpdateAll(jobj->robj, jobj,
                                  HSD_JOBJ_METHOD(jobj)->update);
                if (HSD_JObjMtxIsDirty(jobj)) {
                    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
                    jobj->flags &= ~JOBJ_MTX_DIRTY;
                }
            }
        }
        if (jobj->parent != NULL) {
            HSD_MtxGetTranslate(jobj->parent->mtx, &spB0);
        }
        HSD_RObjGetGlobalPosition(var_r28->robj, 1, &var_r28->translate);
        PSVECSubtract(&var_r28->translate, &spB0, &sp8C);
        temp_f31 = PSVECDotProduct(&sp8C, &sp8C);

        if (temp_f31 > 1e-8F) {
            sp68 = sp8C;
            if (HSD_RObjGetGlobalPosition(jobj->robj, 3, &sp5C)) {
                PSVECSubtract(&sp5C, &spB0, &sp5C);
                if (temp_f26 != 0.0F) {
                    PSMTXRotAxisRad(sp20, &sp68, temp_f26);
                    PSMTXMultVec(sp20, &sp5C, &sp5C);
                }
                PSVECCrossProduct(&sp68, &sp5C, &sp50);
                PSVECCrossProduct(&sp50, &sp68, &sp5C);
            } else {
                sp50.x = jobj->mtx[0][2];
                sp50.y = jobj->mtx[1][2];
                sp50.z = jobj->mtx[2][2];
                PSVECCrossProduct(&sp50, &sp68, &sp5C);
                PSVECCrossProduct(&sp68, &sp5C, &sp50);
            }
            var_f4 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp50, &sp50)));
            PSVECScale(&sp50, &sp80, var_f4);
            var_f4_2 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp5C, &sp5C)));
            PSVECScale(&sp5C, &sp74, var_f4_2);
            temp_f5 = temp_f30 * temp_f30;
            var_f28 = var_f29 * var_f29;
            var_f27 = 0.25F * (((2.0F * (temp_f5 + var_f28)) - temp_f31) -
                               (((temp_f5 - var_f28) * (temp_f5 - var_f28)) /
                                temp_f31));
            if (var_f27 < 0.0F) {
                var_f27 = 0.0F;
            }
            temp_f5_2 = (temp_f5 - var_f27) / temp_f31;
            var_f1 = temp_f5_2 * sqrtf(1.0F / (1e-10F + temp_f5_2));
            var_f29 = var_f27 * sqrtf(1.0F / (1e-10F + var_f27));
        } else {
            var_f1 = 0.0F;
            var_f29 = temp_f30;
        }
        if (var_r30 != 0) {
            var_f29 = -var_f29;
        }
        if ((var_f28 - var_f27) < temp_f31) {
            PSVECScale(&sp8C, &sp98, var_f1);
        } else {
            PSVECScale(&sp8C, &sp98, -var_f1);
        }
        PSVECScale(&sp74, &sp5C, var_f29);
        PSVECAdd(&sp98, &sp5C, &sp98);
        var_f4_4 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp98, &sp98)));
        PSVECScale(&sp98, &sp98, var_f4_4);
        jobj->mtx[0][0] = sp98.x * spBC.x;
        jobj->mtx[1][0] = sp98.y * spBC.x;
        jobj->mtx[2][0] = sp98.z * spBC.x;
        PSVECCrossProduct(&sp80, &sp98, &sp5C);
        jobj->mtx[0][1] = sp5C.x * spBC.y;
        jobj->mtx[1][1] = sp5C.y * spBC.y;
        jobj->mtx[2][1] = sp5C.z * spBC.y;
        jobj->mtx[0][2] = sp80.x * spBC.z;
        jobj->mtx[1][2] = sp80.y * spBC.z;
        jobj->mtx[2][2] = sp80.z * spBC.z;
        jobj->mtx[0][3] = spB0.x;
        jobj->mtx[1][3] = spB0.y;
        jobj->mtx[2][3] = spB0.z;
    }
}

/* Melee's HSD_JObj_803B94C4 (zero) and HSD_JObj_803B94D0 (one); the
 * (0, 0, 1) vector between them is Colosseum's. */
const Vec3 lbl_80274AAC = { 0.0F, 0.0F, 0.0F };
const Vec3 lbl_80274AB8 = { 0.0F, 0.0F, 1.0F };
const Vec3 lbl_80274AC4 = { 1.0F, 1.0F, 1.0F };

void resolveIKJoint2(HSD_JObj* jobj)
{
    Vec3 spA0;
    Vec3 sp94;
    Vec3 sp88;
    Vec3 sp7C;
    Vec3 sp70;
    Vec3 sp64;
    Mtx sp34;
    Vec3 sp28;
    Vec3 sp1C;
    f32 temp_f1_4;
    f32 var_f1_2;
    f32 var_f31;
    f32 var_f4;
    f32 var_f4_2;
    HSD_JObj* var_r29;
    HSD_RObj* temp_r28;
    HSD_RObj* robj;
    HSD_RObj* temp_r29;
    s32 var_r27;
    s32 var_r30;

    var_f31 = 1.0F;
    spA0 = lbl_80274AC4;
    var_r29 = jobj_get_effector_checked(jobj);
    if (var_r29 == NULL || jobj->parent == NULL) {
        return;
    }
    if (jobj->scl != NULL) {
        spA0 = *jobj->scl;
    }
    {
        MtxPtr mtx = jobj->parent->mtx;
        sp88.x = mtx[0][3];
        sp88.y = mtx[1][3];
        sp88.z = mtx[2][3];
    }
    {
        MtxPtr mtx = jobj->parent->mtx;
        sp7C.x = mtx[0][0];
        sp7C.y = mtx[1][0];
        sp7C.z = mtx[2][0];
    }
    var_f4 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp7C, &sp7C)));
    PSVECScale(&sp7C, &sp7C, var_f4);
    if (jobj->parent->scl != NULL) {
        var_f31 = jobj->parent->scl->x;
    }
    robj = HSD_RObjGetByType(jobj->parent->robj, REFTYPE_IKHINT, 0);
    HSD_ASSERT(2309, robj);
    PSVECScale(&sp7C, &sp7C, robj->u.ik_hint.bone_length * var_f31);
    PSVECAdd(&sp88, &sp7C, &sp94);
    PSVECSubtract(&var_r29->translate, &sp94, &sp7C);
    PSVECScale(&sp7C, &sp7C,
               sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp7C, &sp7C))));
    temp_r28 = HSD_RObjGetByType(jobj->robj, 0x20000000, 5);
    temp_r29 = HSD_RObjGetByType(jobj->robj, 0x20000000, 6);
    if ((temp_r28 != NULL) || (temp_r29 != NULL)) {
        HSD_RObj* robj;

        var_r27 = 0;
        robj = HSD_RObjGetByType(jobj->robj, REFTYPE_IKHINT, 0);
        HSD_ASSERT(2343, robj);
        var_r30 = robj->flags & 4 ? 1 : 0;
        {
            MtxPtr mtx = jobj->parent->mtx;
            sp28.x = mtx[0][0];
            sp28.y = mtx[1][0];
            sp28.z = mtx[2][0];
        }
        PSVECNormalize(&sp28, &sp28);
        temp_f1_4 = PSVECDotProduct(&sp28, &sp7C);
        var_f1_2 = temp_f1_4 >= 1.0F    ? 0.0F
                   : temp_f1_4 <= -1.0F ? (f32) M_PI
                                        : acosf(temp_f1_4);
        if (var_r30 == 0) {
            var_f1_2 = -var_f1_2;
        }
        if (temp_r28 != NULL && var_f1_2 < temp_r28->u.limit) {
            var_f1_2 = temp_r28->u.limit;
            var_r27 = 1;
        } else if (temp_r29 != NULL) {
            if (temp_r29->u.limit < var_f1_2) {
                var_f1_2 = temp_r29->u.limit;
                var_r27 = 1;
            }
        }
        if (var_r27 != 0) {
            {
                MtxPtr mtx = jobj->parent->mtx;
                sp1C.x = mtx[0][2];
                sp1C.y = mtx[1][2];
                sp1C.z = mtx[2][2];
            }
            PSMTXRotAxisRad(sp34, &sp1C, var_f1_2);
            PSMTXMultVec(sp34, &sp28, &sp7C);
        }
    }
    {
        MtxPtr mtx = jobj->parent->mtx;
        sp64.x = mtx[0][2];
        sp64.y = mtx[1][2];
        sp64.z = mtx[2][2];
    }
    PSVECCrossProduct(&sp64, &sp7C, &sp70);
    var_f4_2 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp70, &sp70)));
    PSVECScale(&sp70, &sp70, var_f4_2);
    PSVECCrossProduct(&sp7C, &sp70, &sp64);
    jobj->mtx[0][0] = sp7C.x * spA0.x;
    jobj->mtx[1][0] = sp7C.y * spA0.x;
    jobj->mtx[2][0] = sp7C.z * spA0.x;
    jobj->mtx[0][1] = sp70.x * spA0.y;
    jobj->mtx[1][1] = sp70.y * spA0.y;
    jobj->mtx[2][1] = sp70.z * spA0.y;
    jobj->mtx[0][2] = sp64.x * spA0.z;
    jobj->mtx[1][2] = sp64.y * spA0.z;
    jobj->mtx[2][2] = sp64.z * spA0.z;
    jobj->mtx[0][3] = sp94.x;
    jobj->mtx[1][3] = sp94.y;
    jobj->mtx[2][3] = sp94.z;
}

/* HSD_JObjSetupMatrixSub */
void HSD_JObjSetupMatrixSub(HSD_JObj* jobj)
{
    Vec3 sp28;
    Vec3 sp1C;
    Vec3 sp10;
    HSD_RObj* robj;
    HSD_JObj* parent;
    f32 x_scale;

    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
    jobj->flags &= ~JOBJ_MTX_DIRTY;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX)) {
        switch (jobj->flags & JOBJ_EFFECTOR) {
        case JOBJ_JOINT1:
            resolveIKJoint1(jobj);
            break;
        case JOBJ_JOINT2:
            resolveIKJoint2(jobj);
            break;
        case JOBJ_EFFECTOR:
            parent = jobj->parent;
            x_scale = 1.0F;
            if (parent != NULL) {
                robj = HSD_RObjGetByType(parent->robj, REFTYPE_IKHINT, 0);
                if (robj != NULL) {
                    sp1C.x = parent->mtx[0][3];
                    sp1C.y = parent->mtx[1][3];
                    sp1C.z = parent->mtx[2][3];
                    sp10.x = parent->mtx[0][0];
                    sp10.y = parent->mtx[1][0];
                    sp10.z = parent->mtx[2][0];
                    PSVECScale(&sp10, &sp10,
                               sqrtf(1.0F /
                                     (1e-10F + PSVECDotProduct(&sp10, &sp10))));
                    if (parent->scl != NULL) {
                        x_scale = parent->scl->x;
                    }
                    PSVECScale(&sp10, &sp10,
                               robj->u.ik_hint.bone_length * x_scale);
                    PSVECAdd(&sp1C, &sp10, &sp28);
                    jobj->mtx[0][3] = sp28.x;
                    jobj->mtx[1][3] = sp28.y;
                    jobj->mtx[2][3] = sp28.z;
                }
            }
            break;
        default:
            if (jobj->robj != NULL && jobj != NULL && jobj->robj != NULL) {
                HSD_RObjUpdateAll(jobj->robj, jobj,
                                  HSD_JOBJ_METHOD(jobj)->update);
                if (HSD_JObjMtxIsDirty(jobj)) {
                    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
                    jobj->flags &= ~JOBJ_MTX_DIRTY;
                }
            }
            break;
        }
        jobj->flags &= ~JOBJ_MTX_DIRTY;
    }
}

/* HSD_JObjSetMtxDirtySub */
void fn_8019D620(HSD_JObj* jobj)
{
    jobj->flags |= JOBJ_MTX_DIRTY;
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        jobj = jobj->child;
        while (jobj) {
            if (!(jobj->flags & JOBJ_MTX_INDEP_PARENT) &&
                !HSD_JObjMtxIsDirty(jobj))
            {
                fn_8019D620(jobj);
            }
            jobj = jobj->next;
        }
    }
}

/* HSD_JObjSetDPtclCallback */
void HSD_JObjSetPtclCallback(DPCtlCallback cb)
{
    dptcl_callback = cb;
}

/* HSD_JObjSetPtclTgtCallback */
void HSD_JObjSetDPtclCallback(void (*cb)(HSD_JObj* jobj, s32 val))
{
    ptcltgt_callback = cb;
}

int JObjInit(HSD_Class* o)
{
    int status = HSD_OBJECT_PARENT_INFO(&hsdJObj)->init(o);
    if (status >= 0) {
        HSD_JObj* jobj = (HSD_JObj*) o;
        status = 0;
        jobj->flags = JOBJ_MTX_DIRTY;
        jobj->scale.x = 1.0F;
        jobj->scale.y = 1.0F;
        jobj->scale.z = 1.0F;
    }
    return status;
}

void JObjReleaseChild(HSD_JObj* jobj)
{
    if (jobj->child != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            HSD_JObjUnref(jobj->child);
        } else {
            jobj->child->parent = NULL;
            HSD_JObjRemoveAll(jobj->child);
        }
        jobj->child = NULL;
    }
    if (jobj->parent != NULL) {
        HSD_JObjReparent(jobj, NULL);
    }
    if (union_type_dobj(jobj)) {
        if (jobj->u.dobj != NULL) {
            HSD_DObjRemoveAll(jobj->u.dobj);
            jobj->u.dobj = NULL;
        }
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

void JObjRelease(HSD_Class* o)
{
    HSD_JObj* jobj = (HSD_JObj*) o;
    HSD_JOBJ_METHOD(jobj)->release_child(jobj);

    if (HSD_IDGetDataFromTable(NULL, jobj->id, NULL) == jobj) {
        u32 id = jobj->id;
        HSD_IDRemoveByIDFromTable(NULL, id);
    }
    if (jobj->scl != NULL) {
        HSD_VecFree(jobj->scl);
    }
    if (jobj->envelopemtx != NULL) {
        HSD_MtxFree(jobj->envelopemtx);
    }
    HSD_OBJECT_PARENT_INFO(&hsdJObj)->release(o);
}

void JObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    if (info == HSD_CLASS_INFO(&hsdJObj)) {
        ufc_callbacks = NULL;
        current_jobj = NULL;
    }
    HSD_OBJECT_PARENT_INFO(&hsdJObj)->amnesia(info);
}

/* JObjInfoInit */
void JObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdJObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_jobj",
                     sizeof(HSD_JObjInfo), sizeof(HSD_JObj));
    HSD_CLASS_INFO(&hsdJObj)->init = JObjInit;
    HSD_CLASS_INFO(&hsdJObj)->release = JObjRelease;
    HSD_CLASS_INFO(&hsdJObj)->amnesia = JObjAmnesia;
    HSD_JOBJ_INFO(&hsdJObj)->make_mtx = HSD_JObjMakeMatrix;
    HSD_JOBJ_INFO(&hsdJObj)->make_pmtx = HSD_JObjMakePositionMtx;
    HSD_JOBJ_INFO(&hsdJObj)->disp = HSD_JObjDispSub;
    HSD_JOBJ_INFO(&hsdJObj)->load = JObjLoad;
    HSD_JOBJ_INFO(&hsdJObj)->release_child = JObjReleaseChild;
    HSD_JOBJ_INFO(&hsdJObj)->update = HSD_JObjAnimCallback;
}
