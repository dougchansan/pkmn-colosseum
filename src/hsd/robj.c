/**
 * @file robj.c
 * @brief HAL sysdolphin robj.c: reference objects (joint constraints,
 *        limits and expressions), 0x801ADD0C-0x801B019C.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/robj.c) and checked against Colosseum's retail
 * code, which is the newer sysdolphin (>= 1.3.0.0). The library is built
 * with deferred inlining, so functions are listed in HAL's order and MWCC
 * emits them in reverse (the retail address order). Functions nothing in
 * the game references (HSD_RObjSetFlags, the Rvalue helpers, ...) are
 * compiled and dead-stripped by the linker as in retail; their strings still
 * shape the TU's .rodata pool.
 *
 * Globals other objects already link against keep their dtk names; the
 * comments give the HAL names. Colosseum's HSD_JObjUnref/SetupMatrixSub/
 * MakeMatrix, HSD_MemAlloc and HSD_MtxGetRotation are HSD_JObjUnref-style
 * symbols declared in the headers.
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_objalloc.h"
#include "hsd/hsd_robj.h"
#include "hsd/hsd_aobj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"
#include "sysdolphin/baselib/mtx.h"
#include "sysdolphin/baselib/object.h"

#define TYPE_ROBJ 1

void OSReport(const char* msg, ...);
void* memset(void* dst, int c, u32 n);
void PSMTXCopy(const Mtx src, Mtx dst);
void PSVECScale(const Vec3* src, Vec3* dst, f32 scale);
void PSVECSubtract(const Vec3* a, const Vec3* b, Vec3* ab);
f32 PSVECMag(const Vec3* v);
f32 PSVECDotProduct(const Vec3* a, const Vec3* b);
void PSVECCrossProduct(const Vec3* a, const Vec3* b, Vec3* axb);

void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);
void HSD_ObjFree(HSD_ObjAllocData* data, void* obj);
void* HSD_IDGetDataFromTable(void* table, u32 id, s32* success);
/* HSD_MemAlloc */
void* HSD_MemAlloc(u32 size);
/* HSD_IsMemoryReleased: nonzero when the pointer lies in freed memory. */
int fn_801A6990(void* ptr);
u32 HSD_GetNbBits(u32 value);
f32 HSD_ByteCodeEval(u8* bytecode, f32* args, u32 nb_args);

BOOL hsdObjIsDescendantOf(HSD_Obj* o, HSD_ClassInfo* p);
/* hsdJObj, the JObj class info */
extern HSD_ClassInfo lbl_8036C8E0;

static inline void* HSD_IDGetData(u32 id, s32* success)
{
    return HSD_IDGetDataFromTable(NULL, id, success);
}

/* robj_alloc_data */
HSD_ObjAllocData lbl_804656B4;
/* rvalue_alloc_data */
HSD_ObjAllocData lbl_80465688;

/* arg_buf_size, arg_buf: expEvaluate's argument buffer */
static u32 lbl_8047B30C;
static f32* lbl_8047B308;

/* HSD_RObjInitAllocData */
void HSD_RObjInitAllocData(void)
{
    HSD_ObjAllocInit(&lbl_804656B4, sizeof(HSD_RObj), 4);
    HSD_ObjAllocInit(&lbl_80465688, sizeof(HSD_Rvalue), 4);
}

HSD_ObjAllocData* HSD_RObjGetAllocData(void)
{
    return &lbl_804656B4;
}

HSD_ObjAllocData* HSD_RvalueObjGetAllocData(void)
{
    return &lbl_80465688;
}

void HSD_RObjSetFlags(HSD_RObj* robj, u32 flags)
{
    if (robj != NULL) {
        robj->flags |= flags;
    }
}

HSD_RObj* HSD_RObjGetByType(HSD_RObj* robj, u32 type, u32 subtype)
{
    BOOL has_type;
    HSD_RObj* curr;

    if (robj == NULL) {
        return NULL;
    }

    for (curr = robj; curr != NULL; curr = curr->next) {
        if (curr->flags & 0x80000000) {
            has_type = TRUE;
        } else {
            has_type = FALSE;
        }

        if (has_type) {
            if ((curr->flags & ROBJ_TYPE_MASK) == type &&
                (!subtype || subtype == (curr->flags & 0xFFFFFFF)))
            {
                return curr;
            }
        }
    }

    return NULL;
}

static void RObjUpdateFunc(void* obj, s32 type, HSD_ObjData* val)
{
    HSD_RObj* robj;

    if (obj == NULL) {
        return;
    }

    if (type != TYPE_ROBJ) {
        return;
    }

    robj = (HSD_RObj*) obj;
    if (val->fv >= 0.5) {
        robj->flags = robj->flags | 0x80000000;
        return;
    }
    robj->flags = robj->flags & 0x7FFFFFFF;
}

void HSD_RObjAnim(HSD_RObj* robj)
{
    if (robj == NULL) {
        return;
    }

    /* HAL's update callbacks take the attribute type as a signed enum_t
     * (RObjUpdateFunc compares it with cmpwi); hsd_forward.h spells the
     * callback type with u32. */
    HSD_AObjInterpretAnim(robj->aobj, robj, (HSD_ObjUpdateFunc) RObjUpdateFunc);
}

/* HSD_RObjAnimAll */
void fn_801B0040(HSD_RObj* robj)
{
    HSD_RObj* curr;

    if (robj == NULL) {
        return;
    }

    for (curr = robj; curr != NULL; curr = curr->next) {
        HSD_RObjAnim(curr);
    }
}

void HSD_RObjRemoveAnimByFlags(HSD_RObj* robj, u32 flags)
{
    if (robj == NULL) {
        return;
    }

    if (robj->aobj != NULL && (flags & 0x80) != 0) {
        HSD_AObjRemove(robj->aobj);
        robj->aobj = NULL;
    }
}

void HSD_RObjRemoveAnimAllByFlags(HSD_RObj* robj, u32 flags)
{
    if (robj == NULL) {
        return;
    }

    for (; robj != NULL; robj = robj->next) {
        HSD_RObjRemoveAnimByFlags(robj, flags);
    }
}

/* HSD_RObjRemoveAnimAll */
void fn_801AFFE0(HSD_RObj* robj)
{
    HSD_RObjRemoveAnimAllByFlags(robj, 0x7FF);
}

void HSD_RObjReqAnimByFlags(HSD_RObj* robj, f32 startframe, u32 flags)
{
    if (robj == NULL) {
        return;
    }

    if (robj->aobj != NULL && (flags & 0x80) != 0) {
        HSD_AObjReqAnim(robj->aobj, startframe);
    }
}

/* HSD_RObjReqAnimAllByFlags */
void HSD_RObjReqAnimAllByFlags(HSD_RObj* robj, f32 startframe, u32 flags)
{
    if (robj == NULL) {
        return;
    }

    for (; robj != NULL; robj = robj->next) {
        HSD_RObjReqAnimByFlags(robj, startframe, flags);
    }
}

/* HSD_RObjReqAnimAll */
void fn_801AFEFC(HSD_RObj* robj, f32 startframe)
{
    HSD_RObjReqAnimAllByFlags(robj, startframe, 0x7FF);
}

void HSD_RObjAddAnim(HSD_RObj* robj, HSD_RObjAnimJoint* anim)
{
    if (robj == NULL || anim == NULL) {
        return;
    }

    if (robj->aobj != NULL) {
        HSD_AObjRemove(robj->aobj);
    }
    robj->aobj = HSD_AObjLoadDesc(anim->aobjdesc);
}

/* HSD_RObjAddAnimAll */
void HSD_RObjAddAnimAll(HSD_RObj* robj, HSD_RObjAnimJoint* anim)
{
    HSD_RObj* i;
    HSD_RObjAnimJoint* j;

    if (robj == NULL || anim == NULL) {
        return;
    }

    for (i = robj, j = anim; i != NULL && j != NULL; i = i->next, j = j->next)
    {
        HSD_RObjAddAnim(i, j);
    }
}

static u32 HSD_RObjGetConstraintType(HSD_RObj* robj)
{
    if (robj == NULL) {
        return 0;
    }

    return robj->flags & 0x0FFFFFFF;
}

/* HSD_RObjGetGlobalPosition */
int HSD_RObjGetGlobalPosition(HSD_RObj* robj, int type, Vec3* p)
{
    Vec3 v = { 0, 0, 0 };
    HSD_RObj* rp;
    int n = 0;

    if (robj == NULL) {
        return n;
    }
    for (rp = robj; rp != NULL; rp = rp->next) {
        if (((rp->flags & ROBJ_TYPE_MASK) == REFTYPE_JOBJ ? 1 : 0) != 0) {
            if (((rp->flags & 0x80000000) ? 1 : 0) != 0 &&
                (unsigned) type == HSD_RObjGetConstraintType(rp))
            {
                HSD_ASSERT(498, rp->u.jobj);
                HSD_JObjSetupMatrix(rp->u.jobj);
                n += 1;
                v.x += rp->u.jobj->mtx[0][3];
                v.y += rp->u.jobj->mtx[1][3];
                v.z += rp->u.jobj->mtx[2][3];
            }
        }
    }
    if (n != 0) {
        f32 f = (f32) 1.0 / (f32) n;
        p->x = f * v.x;
        p->y = f * v.y;
        p->z = f * v.z;
    }
    return n;
}

static void set_dirup_matrix(Vec3* dir_ptr, Vec3* uv_ptr, Vec3* scale_ptr,
                             void* obj, HSD_ObjUpdateFunc update_func)
{
    Vec3 z_vec;
    Vec3 v;
    f32 kz;
    f32 kdir;

    PSVECCrossProduct(dir_ptr, uv_ptr, &z_vec);
    kdir =
        sqrtf(1.0F / (1.00000001335e-10f + PSVECDotProduct(dir_ptr, dir_ptr)));
    PSVECScale(dir_ptr, dir_ptr, kdir);
    kz = sqrtf(1.0F / (1.00000001335e-10f + PSVECDotProduct(&z_vec, &z_vec)));
    PSVECScale(&z_vec, &z_vec, kz);
    PSVECCrossProduct(&z_vec, dir_ptr, uv_ptr);
    v.x = dir_ptr->x * scale_ptr->x;
    v.y = dir_ptr->y * scale_ptr->x;
    v.z = dir_ptr->z * scale_ptr->x;
    update_func(obj, 50, (HSD_ObjData*) &v);
    v.x = uv_ptr->x * scale_ptr->y;
    v.y = uv_ptr->y * scale_ptr->y;
    v.z = uv_ptr->z * scale_ptr->y;
    update_func(obj, 51, (HSD_ObjData*) &v);
    v.x = z_vec.x * scale_ptr->z;
    v.y = z_vec.y * scale_ptr->z;
    v.z = z_vec.z * scale_ptr->z;
    update_func(obj, 52, (HSD_ObjData*) &v);
    update_func(obj, 55, NULL);
}

static void resolveCnsDirUp(HSD_RObj* robj, void* obj,
                            HSD_ObjUpdateFunc update_func)
{
    HSD_JObj* jobj = obj;
    Vec3 this_scale = { 1.0f, 1.0f, 1.0f };
    Vec3 up = { 0.0f, 1.0f, 0.0f };
    Vec3 this_pos;
    Vec3 dir;
    f32 k;

    if (HSD_RObjGetGlobalPosition(robj, 2, &this_pos) != 0) {
        dir.x = jobj->mtx[0][3];
        dir.y = jobj->mtx[1][3];
        dir.z = jobj->mtx[2][3];
        PSVECSubtract(&this_pos, &dir, &this_pos);
        if (HSD_RObjGetGlobalPosition(robj, 3, &up) != 0) {
            PSVECSubtract(&up, &dir, &up);
        } else {
            k = 1.0f - PSVECDotProduct(&this_pos, &up);
            if (__fabs(k) < 1.00000001335e-10f) {
                up.x = 0.0f;
                up.y = 0.0f;
                up.z = 1.0;
            }
        }

        if (jobj->scl != NULL) {
            this_scale = *jobj->scl;
        }
        set_dirup_matrix(&this_pos, &up, &this_scale, obj, update_func);
    }
}

/* The per-axis update types resolveCnsOrientation sends (Melee
 * HSD_RObj_80406E74). */
static int lbl_8036CD88[3] = { 0x32, 0x33, 0x34 };

static void resolveCnsOrientation(HSD_RObj* robj, void* obj,
                                  HSD_ObjUpdateFunc update_func)
{
    Mtx mtx;
    f32 sval;
    Vec3 v;
    HSD_JObj* jobj;
    int i;

    HSD_ASSERT(630, obj);

    robj = HSD_RObjGetByType(robj, REFTYPE_JOBJ, 4);
    if (robj == NULL) {
        return;
    }

    PSMTXCopy(HSD_JObjGetMtxPtr(robj->u.jobj), mtx);
    jobj = obj;

    for (i = 0; i < 3; i++) {
        HSD_MtxColVec(mtx, i, &v);
        sval = PSVECMag(&v);
        if (sval > 1e-10F) {
            sval = 1.0F / sval;
        }
        sval *= HSD_MtxColMag(jobj->mtx, i);
        v.x *= sval;
        v.y *= sval;
        v.z *= sval;
        update_func(obj, lbl_8036CD88[i], (HSD_ObjData*) &v);
    }
    update_func(obj, 0x37, NULL);
}

static void resolveLimits(HSD_RObj* robj, void* obj,
                          HSD_ObjUpdateFunc update_func)
{
    HSD_JObj* jobj = (HSD_JObj*) obj;
    HSD_RObj* rp;
    BOOL update_mtx = FALSE;

    HSD_ASSERT(670, jobj);

    rp = robj;
    while (rp != NULL) {
        if ((rp->flags & ROBJ_TYPE_MASK) == REFTYPE_LIMIT) {
            break;
        }
        rp = rp->next;
    }

    if (rp != NULL) {
        for (rp = robj; rp != NULL; rp = rp->next) {
            if ((rp->flags & ROBJ_TYPE_MASK) == REFTYPE_LIMIT) {
                switch (rp->flags & 0xFFFFFFF) {
                default:
                    continue;
                case 1:
                    if (jobj->rotate.x < rp->u.limit) {
                        jobj->rotate.x = rp->u.limit;
                    }
                    break;
                case 2:
                    if (jobj->rotate.x > rp->u.limit) {
                        jobj->rotate.x = rp->u.limit;
                    }
                    break;
                case 3:
                    if (jobj->rotate.y < rp->u.limit) {
                        jobj->rotate.y = rp->u.limit;
                    }
                    break;
                case 4:
                    if (jobj->rotate.y > rp->u.limit) {
                        jobj->rotate.y = rp->u.limit;
                    }
                    break;
                case 5:
                    if (jobj->rotate.z < rp->u.limit) {
                        jobj->rotate.z = rp->u.limit;
                    }
                    break;
                case 6:
                    if (jobj->rotate.z > rp->u.limit) {
                        jobj->rotate.z = rp->u.limit;
                    }
                    break;
                case 7:
                    if (jobj->translate.x < rp->u.limit) {
                        jobj->translate.x = rp->u.limit;
                    }
                    break;
                case 8:
                    if (jobj->translate.x > rp->u.limit) {
                        jobj->translate.x = rp->u.limit;
                    }
                    break;
                case 9:
                    if (jobj->translate.y < rp->u.limit) {
                        jobj->translate.y = rp->u.limit;
                    }
                    break;
                case 10:
                    if (jobj->translate.y > rp->u.limit) {
                        jobj->translate.y = rp->u.limit;
                    }
                    break;
                case 11:
                    if (jobj->translate.y < rp->u.limit) {
                        jobj->translate.y = rp->u.limit;
                    }
                    break;
                case 12:
                    if (jobj->translate.y > rp->u.limit) {
                        jobj->translate.y = rp->u.limit;
                    }
                    break;
                }
                update_mtx = TRUE;
            }
        }
        if (update_mtx) {
            HSD_JObjMakeMatrix(jobj);
        }
    }
}

static void expEvaluate(HSD_Exp* exp, u32 type, void* obj,
                        HSD_ObjUpdateFunc update_func);

/* HSD_RObjUpdateAll */
void HSD_RObjUpdateAll(HSD_RObj* robj, void* obj,
                       HSD_ObjUpdateFunc update_func)
{
    HSD_RObj* rp;
    Vec3 vec;

    if (robj != NULL) {
        if (HSD_RObjGetGlobalPosition(robj, 1, &vec) != 0) {
            update_func(obj, 0x35, (HSD_ObjData*) &vec);
            update_func(obj, 0x38, NULL);
        }
        resolveCnsDirUp(robj, obj, update_func);
        resolveCnsOrientation(robj, obj, update_func);
        resolveLimits(robj, obj, update_func);

        for (rp = robj; rp != NULL; rp = rp->next) {
            if ((rp->flags & ROBJ_TYPE_MASK) == REFTYPE_EXP &&
                (rp->flags & 0x80000000) != 0)
            {
                expEvaluate(&rp->u.exp, rp->flags & 0xFFFFFFF, obj,
                            update_func);
            }
        }
    }
}

void HSD_RvalueResolveRefsAll(HSD_Rvalue* rvalue, HSD_RvalueList* list);

void HSD_RObjResolveRefs(HSD_RObj* robj, HSD_RObjDesc* desc)
{
    if (robj != NULL && desc != NULL) {
        switch (robj->flags & ROBJ_TYPE_MASK) {
        case REFTYPE_JOBJ:
            HSD_JObjUnrefThis(robj->u.jobj);
            robj->u.jobj = HSD_IDGetData((u32) desc->u.joint, NULL);
            HSD_ASSERT(816, robj->u.jobj);
            HSD_JObjRefThis(robj->u.jobj);
            break;
        case REFTYPE_EXP:
            HSD_RvalueResolveRefsAll(robj->u.exp.rvalue, desc->u.exp->rvalue);
            break;
        }
    }
}

/* HSD_RObjResolveRefsAll */
void fn_801AEBE4(HSD_RObj* robj, HSD_RObjDesc* desc)
{
    for (; robj != NULL && desc != NULL; robj = robj->next, desc = desc->next)
    {
        HSD_RObjResolveRefs(robj, desc);
    }
}

static void bcexpLoadDesc(HSD_Exp* exp, HSD_ByteCodeExpDesc* desc);
static void expLoadDesc(HSD_Exp* exp, HSD_ExpDesc* desc);
/* HSD_RObjAlloc */
HSD_RObj* HSD_RObjAlloc(void);

HSD_RObj* HSD_RObjLoadDesc(HSD_RObjDesc* robjdesc)
{
    HSD_RObj* robj;

    if (robjdesc != NULL) {
        robj = HSD_RObjAlloc();
        robj->next = HSD_RObjLoadDesc(robjdesc->next);
        robj->flags = robjdesc->flags;
        switch (robj->flags & ROBJ_TYPE_MASK) {
        case REFTYPE_JOBJ:
            break;
        case REFTYPE_LIMIT: {
            switch (robj->flags & 0xFFFFFFF) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                robj->u.limit = 0.017453292f * robjdesc->u.limit;
                break;
            default:
                robj->u.limit = robjdesc->u.limit;
                break;
            }
        } break;
        case REFTYPE_EXP:
            expLoadDesc(&robj->u.exp, robjdesc->u.exp);
            break;
        case REFTYPE_BYTECODE:
            bcexpLoadDesc(&robj->u.exp, robjdesc->u.bcexp);
            robj->flags &= ~ROBJ_TYPE_MASK;
            break;
        case REFTYPE_IKHINT:
            robj->u.ik_hint.bone_length = robjdesc->u.ik_hint->bone_length;
            robj->u.ik_hint.rotate_x = robjdesc->u.ik_hint->rotate_x;
            break;
        default:
            HSD_Panic(__FILE__, 893, "unexpected type of robj.\n");
            break;
        }
        return robj;
    }
    return NULL;
}

void HSD_RvalueRemoveAll(HSD_Rvalue* rvalue);

void HSD_RObjRemove(HSD_RObj* robj)
{
    if (robj != NULL) {
        switch (robj->flags & ROBJ_TYPE_MASK) {
        case REFTYPE_JOBJ:
            HSD_JObjUnrefThis(robj->u.jobj);
            break;
        case REFTYPE_EXP:
            HSD_RvalueRemoveAll(robj->u.exp.rvalue);
            break;
        }
        HSD_AObjRemove(robj->aobj);
        HSD_RObjFree(robj);
    }
}

/* HSD_RObjRemoveAll */
void fn_801AE50C(HSD_RObj* robj)
{
    HSD_RObj* next;

    for (; robj != NULL; robj = next) {
        next = robj->next;
        HSD_RObjRemove(robj);
    }
}

/*
 * An empty allocator hook HSD_RObjAlloc passes its pool and tag to. MWCC
 * expands it everywhere HSD_RObjAlloc is inlined except the one site past
 * its inline depth (the second-level copy in HSD_RObjLoadDesc), which calls
 * the out-of-line copy at 0x801AEBE0 with (&robj_alloc_data, "robj: alloc").
 * fobj.c shows the same pattern (fn_80199A84 with "fobj: alloc"), so HAL
 * keeps it in a shared header; its name is not recoverable from the binary.
 */
static inline void fn_801AEBE0(HSD_ObjAllocData* data, char* name)
{
}

/* HSD_RObjAlloc */
HSD_RObj* HSD_RObjAlloc(void)
{
    HSD_RObj* new = HSD_ObjAlloc(&lbl_804656B4);
    fn_801AEBE0(&lbl_804656B4, "robj: alloc");
    HSD_ASSERT(965, new);
    memset(new, 0, sizeof(*new));
    return new;
}

void HSD_RObjFree(HSD_RObj* robj)
{
    HSD_ObjFree(HSD_RObjGetAllocData(), robj);
}

/*
 * Supplies the argument buffer expEvaluate() collects rvalues into
 * (expEvaluate allocates lbl_8047B30C floats itself when none is set).
 * Nothing in the game calls it, so the linker strips it; its assert string
 * "(ptr && nitems) || !ptr" still sits in robj.c's .rodata pool between
 * expEvaluate's and HSD_RObjAlloc's strings (0x80275264).
 */
void HSD_RObjSetArgBuffer(f32* ptr, u32 nitems)
{
    HSD_ASSERT(1004, (ptr && nitems) || !ptr);
    lbl_8047B308 = ptr;
    lbl_8047B30C = nitems;
}

static void expEvaluate(HSD_Exp* exp, u32 type, void* obj,
                        HSD_ObjUpdateFunc update_func)
{
    HSD_Rvalue* rvalue;
    f32* cur_arg;
    int cur_bit;
    HSD_JObj* jobj;
    Vec3 scale;
    /* The rotation extracted from the joint matrix goes into a quaternion-
     * sized local (x, y, z used), as jobj.c passes &jobj->rotate; the
     * result travels to the update callback in a Vec3, as set_dirup_matrix
     * does: the retail frame is val 0x8, rotate 0x14, scale 0x24. */
    Quaternion rotate;
    Vec3 val;

    if (exp->nb_args == -1) {
        u32 nb_args = 0;
        HSD_Rvalue* rvalue;
        for (rvalue = exp->rvalue; rvalue != NULL; rvalue = rvalue->next) {
            nb_args += HSD_GetNbBits(rvalue->flags);
        }
        exp->nb_args = nb_args;
    }
    if (lbl_8047B308 == NULL) {
        if (lbl_8047B30C == 0) {
            lbl_8047B30C = 100;
        }
        lbl_8047B308 = HSD_MemAlloc(lbl_8047B30C * sizeof(f32));
    }
    if (lbl_8047B30C < exp->nb_args) {
        OSReport(
            "Number of argment of expression exceeds the argument buffer\n"
            "size. (requested num of arg %d, allocated %d)\n",
            exp->nb_args, lbl_8047B30C);
        HSD_Panic(__FILE__, 1051, "");
    }
    cur_arg = lbl_8047B308;
    for (rvalue = exp->rvalue; rvalue != NULL; rvalue = rvalue->next) {
        jobj = rvalue->jobj;
        HSD_ASSERT(1060, jobj);
        HSD_JObjSetupMatrix(rvalue->jobj);
        for (cur_bit = 1; cur_bit && cur_bit <= rvalue->flags; cur_bit <<= 1) {
            switch (rvalue->flags & cur_bit) {
            case 0x1:
                *cur_arg++ = 57.29578F * jobj->rotate.x;
                break;
            case 0x2:
                *cur_arg++ = 57.29578F * jobj->rotate.y;
                break;
            case 0x4:
                *cur_arg++ = 57.29578F * jobj->rotate.z;
                break;
            case 0x8:
                break;
            case 0x10:
                *cur_arg++ = jobj->translate.x;
                break;
            case 0x20:
                *cur_arg++ = jobj->translate.y;
                break;
            case 0x40:
                *cur_arg++ = jobj->translate.z;
                break;
            case 0x80:
                *cur_arg++ = jobj->scale.x;
                break;
            case 0x100:
                *cur_arg++ = jobj->scale.y;
                break;
            case 0x200:
                *cur_arg++ = jobj->scale.z;
                break;
            case 0x400:
            case 0x800:
                break;
            case 0x10000:
                HSD_MtxGetRotation(jobj->mtx, (Vec3*) &rotate);
                *cur_arg++ = 57.29578F * rotate.x;
                break;
            case 0x20000:
                HSD_MtxGetRotation(jobj->mtx, (Vec3*) &rotate);
                *cur_arg++ = 57.29578F * rotate.y;
                break;
            case 0x40000:
                HSD_MtxGetRotation(jobj->mtx, (Vec3*) &rotate);
                *cur_arg++ = 57.29578F * rotate.z;
                break;
            case 0x100000:
                *cur_arg++ = jobj->mtx[0][3];
                break;
            case 0x200000:
                *cur_arg++ = jobj->mtx[1][3];
                break;
            case 0x400000:
                *cur_arg++ = jobj->mtx[2][3];
                break;
            case 0x800000:
                HSD_MtxGetScale(jobj->mtx, &scale);
                *cur_arg++ = scale.x;
                break;
            case 0x1000000:
                HSD_MtxGetScale(jobj->mtx, &scale);
                *cur_arg++ = scale.y;
                break;
            case 0x2000000:
                HSD_MtxGetScale(jobj->mtx, &scale);
                *cur_arg++ = scale.z;
                break;
            }
        }
    }
    if (exp->is_bytecode) {
        val.x = HSD_ByteCodeEval(exp->expr.bytecode, lbl_8047B308, exp->nb_args);
    } else {
        val.x = exp->expr.func(lbl_8047B308);
    }
    if (type - 1 <= 1 || type == 3) {
        val.x = val.x * 0.017453292F;
    }
    update_func(obj, type, (HSD_ObjData*) &val);
}

static f32 dummy_func(void* unused)
{
    return 0.0f;
}

HSD_Rvalue* HSD_RvalueAlloc(void)
{
    HSD_Rvalue* rvalue = HSD_ObjAlloc(HSD_RvalueObjGetAllocData());
    HSD_ASSERT(1157, rvalue);
    memset(rvalue, 0, sizeof(HSD_Rvalue));
    return rvalue;
}

void HSD_RvalueRemove(HSD_Rvalue* rvalue)
{
    if (rvalue != NULL) {
        HSD_JObjUnrefThis(rvalue->jobj);
        HSD_ObjFree(HSD_RvalueObjGetAllocData(), rvalue);
    }
}

void HSD_RvalueRemoveAll(HSD_Rvalue* rvalue)
{
    HSD_Rvalue* next;

    for (; rvalue != NULL; rvalue = next) {
        next = rvalue->next;
        HSD_RvalueRemove(rvalue);
    }
}

static HSD_Rvalue* loadRvalue(HSD_RvalueList* list)
{
    HSD_Rvalue* rvalue = NULL;
    HSD_Rvalue** rp = &rvalue;

    if (list == NULL) {
        return NULL;
    }
    for (; list->joint != NULL; list++) {
        *rp = HSD_RvalueAlloc();
        (*rp)->flags = list->flags;
        rp = &(*rp)->next;
    }
    return rvalue;
}

static void expLoadDesc(HSD_Exp* exp, HSD_ExpDesc* desc)
{
    memset(exp, 0, sizeof(HSD_Exp));
    if (desc != NULL) {
        if (desc->func != NULL) {
            exp->expr.func = desc->func;
        } else {
            exp->expr.func = dummy_func;
        }
        exp->rvalue = loadRvalue(desc->rvalue);
        exp->nb_args = -1;
    }
}

static void bcexpLoadDesc(HSD_Exp* exp, HSD_ByteCodeExpDesc* desc)
{
    memset(exp, 0, sizeof(HSD_Exp));
    if (desc != NULL) {
        if (desc->bytecode != NULL) {
            exp->expr.bytecode = desc->bytecode;
        } else {
            exp->expr.bytecode = NULL;
        }
        exp->rvalue = loadRvalue(desc->rvalue);
        exp->nb_args = -1;
        exp->is_bytecode = 1;
    }
}

void HSD_RvalueResolveRefs(HSD_Rvalue* rvalue, HSD_RvalueList* list)
{
    if (rvalue != NULL && list != NULL) {
        HSD_JObjUnrefThis(rvalue->jobj);
        rvalue->jobj = HSD_IDGetData((u32) list->joint, NULL);
        HSD_ASSERT(1266, rvalue->jobj);
        HSD_JObjRefThis(rvalue->jobj);
    }
}

void HSD_RvalueResolveRefsAll(HSD_Rvalue* rvalue, HSD_RvalueList* list)
{
    if (list == NULL) {
        return;
    }
    for (; rvalue != NULL && list->joint != NULL;
         rvalue = rvalue->next, list++)
    {
        HSD_RvalueResolveRefs(rvalue, list);
    }
}

/*
 * Not referenced by the game, so the linker strips it; its strings (the
 * object.h iref assert, then the message) open robj.c's .rodata string
 * pool at 0x8027518C. Retail pools no "0" in .sdata2 for it, so the
 * failure path reports through HSD_Panic rather than an assert.
 */
void HSD_RObjSetConstraintObj(HSD_RObj* robj, void* obj)
{
    if (robj != NULL) {
        if (robj->u.jobj != NULL) {
            HSD_JObjUnrefThis(robj->u.jobj);
            robj->u.jobj = NULL;
        }

        if (hsdObjIsDescendantOf(obj, &lbl_8036C8E0)) {
            robj->u.jobj = obj;
            HSD_JObjRefThis(obj);
        } else {
            HSD_Panic(__FILE__, 1376, "constraint only support jobj target.\n");
        }
    }
}

void _HSD_RObjForgetMemory(void)
{
    if (fn_801A6990(lbl_8047B308)) {
        lbl_8047B308 = NULL;
        lbl_8047B30C = 0;
    }
}
