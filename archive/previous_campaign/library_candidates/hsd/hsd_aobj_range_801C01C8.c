/**
 * @file hsd_aobj_range_801C01C8.c
 * @brief HSD library code split out of battle_grid.c (audit 2026-07-01).
 *
 * Address range: 0x801C01C8 - 0x801C2AE8. Contains the real, variadic
 * HSD_ForeachAnim (0x801C028C, classic PPC varargs prologue; callers in
 * gs_render*.c) plus small helpers, and the per-object-kind "ForeachAnim"
 * family (JObj/DObj/LObj/CObjForeachAnim) and the surrounding HSD_AObj*
 * functions (0x801C0F20-0x801C2AE8), all of which are HSD sysdolphin
 * library code, not battle-grid code -- this is the direct continuation
 * of the same XD translation unit (aobj.cpp) as the functions above.
 * HSD_VISetUserPostRetraceCallback is matched below; HSD_ForeachAnim remains a near match.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_fog.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_id.h"
#include "hsd/hsd_jobj.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_robj.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_wobj.h"
#include "game/battle/battle_grid_types.h"

/* HSD_ObjAllocData pool backing HSD_AObjAlloc/HSD_AObjFree (sizeof(HSD_AObj)
 * == 0x1C, 4-byte aligned). Declared as a byte array in the absence of an
 * HSD_ObjAllocData type in include/hsd/hsd_object.h, matching the existing
 * house style in hsd_dobj.c / hsd_mobj_range_801A8478.c. */
extern u8 lbl_80466DB8[];
extern void* volatile lbl_80466BC0[];
extern void* HSD_ObjAlloc(void* list);
extern void HSD_ObjFree(void* list, void* data);
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);

/* aobj.c's HSD_ASSERT __FILE__/expr strings (already matched in
 * src/hsd/hsd_sdata2_8047DE90.c). */
extern const u8 lbl_8047DF40[7]; /* "aobj.c" */
extern const u8 lbl_8047DF48[]; /* "obj"    */
extern const u8 lbl_8047DF4C[]; /* "new"    */

/* aobj.c's two end-callback counters (see HSD_AObjInvokeCallBacks): the
 * number of AObjs that finished this frame, and the number still running. */
extern s32 lbl_8047B390;
extern s32 lbl_8047B38C;
extern HSD_SList* lbl_8047B388;

/* HSD_JObjUnref's tail helper. The target calls the raw symbol at
 * 0x801A05EC, NOT src/hsd/hsd_jobj.c's HSD_JObjUnref, so it must be spelled
 * as HSD_JObjUnref here to keep the relocation identical. */
extern void HSD_JObjUnref(void* obj);
extern f64 fmod(f64 x, f64 y);
extern void* memset(void* dst, int val, u32 size);

/**
 * HSD_VISetUserGXDrawDoneCallback - Address: 0x801C01C8 | Size: 0x54
 */
void* HSD_VISetUserGXDrawDoneCallback(void* callback)
{
    void* previous = lbl_80466BC0[0x7A];
    BOOL enabled = OSDisableInterrupts();

    lbl_80466BC0[0x7A] = callback;
    OSRestoreInterrupts(enabled);
    return previous;
}

/**
 * HSD_VISetUserPostRetraceCallback - Address: 0x801C021C | Size: 0x54
 */
void* HSD_VISetUserPostRetraceCallback(void* callback)
{
    void* previous = lbl_80466BC0[0x77];
    BOOL enabled = OSDisableInterrupts();

    lbl_80466BC0[0x77] = callback;
    OSRestoreInterrupts(enabled);
    return previous;
}

/**
 * _HSD_AObjForgetMemory - Address: 0x801C0270 | Size: 0xC
 */
void _HSD_AObjForgetMemory(void* low, void* high) {
    lbl_8047B388 = NULL;
}

/**
 * HSD_AObjSetRate - Address: 0x801C027C | Size: 0x10
 */
void HSD_AObjSetRate(HSD_AObj* aobj, f32 rate) {
    if (aobj != NULL) {
        aobj->framerate = rate;
    }
}

/* =========================================================================
 * NOTE: 0x801C01C8-0x801C0F20 (incl. the real, variadic HSD_ForeachAnim at
 * 0x801C028C) is HSD library code (audit 2026-07-01).
 *
 * JObjForeachAnim/DObjForeachAnim/LObjForeachAnim/CObjForeachAnim are also HSD library
 * code: the per-object-kind "ForeachAnim" family (doldecomp/melee
 * src/sysdolphin/baselib/aobj.c) that HSD_ForeachAnim dispatches into by
 * HSD_Type. MWCC fully inlined the shared "callbackForeachFunc" 12-way
 * argument-marshaling switch (see HSD_FOREACH_INVOKE below) plus the
 * leaf RObj/WObj/MObj/TObj/PObj walkers into each of these four, which is
 * why each is a single large function with several duplicated jump
 * tables rather than a tree of small calls. Confirmed against XD's
 * matched JObjForeachAnim/DObjForeachAnim/LObjForeachAnim/
 * CObjForeachAnim (near-identical sizes) and against every mask-bit and
 * type-constant used below (each equals MASK_OF(x) / HSD_Type from
 * include/hsd/hsd_object.h).
 * ========================================================================= */

/* HSD_ForeachAnim's internal argument-shape tag. Colosseum's ordering
 * differs from melee's AObj_Arg_Type (AOT moved next to AO instead of
 * after AOU); order below is reverse-engineered from the jump tables. */
typedef enum HSD_ForeachArgType {
    HSD_FOREACH_A,
    HSD_FOREACH_AF,
    HSD_FOREACH_AV,
    HSD_FOREACH_AU,
    HSD_FOREACH_AO,
    HSD_FOREACH_AOT,
    HSD_FOREACH_AOF,
    HSD_FOREACH_AOV,
    HSD_FOREACH_AOU,
    HSD_FOREACH_AOTF,
    HSD_FOREACH_AOTV,
    HSD_FOREACH_AOTU
} HSD_ForeachArgType;

typedef union HSD_ForeachArg {
    f32 f;
    u32 d;
    void* v;
} HSD_ForeachArg;

/* Colosseum's HSD_PObj carries a direct HSD_AObj* immediately after the
 * payload union that include/hsd/hsd_pobj.h (melee-derived) models only
 * up to; that trailing field is what DObjForeachAnim's PObj sub-loop
 * walks (pobj+0x18, matching pobj->next chaining at pobj+0x4). Modeled
 * locally (mirroring HSD_PObj's field layout by hand) rather than
 * editing the shared header. */
typedef struct BattleGridPObjAnim {
    void* classInfo;                /* 0x00 */
    struct BattleGridPObjAnim* next; /* 0x04 */
    void* verts;                     /* 0x08 */
    u16 flags;                       /* 0x0C */
    u16 nDisplay;                    /* 0x0E */
    void* display;                   /* 0x10 */
    void* payload;                   /* 0x14 (jobj / shape_set / envelope_list) */
    HSD_AObj* aobj;                  /* 0x18 */
} BattleGridPObjAnim;

/* Shared 12-way argument-shape dispatch, inlined at every call site
 * (matches the repeated jumptable_XXXX blocks in each ForeachAnim). */
#define HSD_FOREACH_INVOKE(aobj_, obj_, type_, func_, argType_, arg_) \
    do { \
        switch (argType_) { \
        case HSD_FOREACH_A: \
            ((void (*)(HSD_AObj*))(func_))(aobj_); \
            break; \
        case HSD_FOREACH_AF: \
            ((void (*)(HSD_AObj*, f32))(func_))(aobj_, (arg_)->f); \
            break; \
        case HSD_FOREACH_AV: \
            ((void (*)(HSD_AObj*, void*))(func_))(aobj_, (arg_)->v); \
            break; \
        case HSD_FOREACH_AU: \
            ((void (*)(HSD_AObj*, u32))(func_))(aobj_, (arg_)->d); \
            break; \
        case HSD_FOREACH_AO: \
            ((void (*)(HSD_AObj*, void*))(func_))(aobj_, (obj_)); \
            break; \
        case HSD_FOREACH_AOT: \
            ((void (*)(HSD_AObj*, void*, s32))(func_))(aobj_, (obj_), (type_)); \
            break; \
        case HSD_FOREACH_AOF: \
            ((void (*)(HSD_AObj*, void*, f32))(func_))(aobj_, (obj_), (arg_)->f); \
            break; \
        case HSD_FOREACH_AOV: \
            ((void (*)(HSD_AObj*, void*, void*))(func_))(aobj_, (obj_), (arg_)->v); \
            break; \
        case HSD_FOREACH_AOU: \
            ((void (*)(HSD_AObj*, void*, u32))(func_))(aobj_, (obj_), (arg_)->d); \
            break; \
        case HSD_FOREACH_AOTF: \
            ((void (*)(HSD_AObj*, void*, s32, f32))(func_))(aobj_, (obj_), (type_), (arg_)->f); \
            break; \
        case HSD_FOREACH_AOTV: \
            ((void (*)(HSD_AObj*, void*, s32, void*))(func_))(aobj_, (obj_), (type_), (arg_)->v); \
            break; \
        case HSD_FOREACH_AOTU: \
            ((void (*)(HSD_AObj*, void*, s32, u32))(func_))(aobj_, (obj_), (type_), (arg_)->d); \
            break; \
        } \
    } while (0)

void DObjForeachAnim(HSD_DObj* dobj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg);
void JObjForeachAnim(HSD_JObj* obj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg);
void LObjForeachAnim(HSD_LObj* lobj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg);
void CObjForeachAnim(HSD_CObj* cobj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg);

void HSD_ForeachAnim(void* obj, HSD_Type type, HSD_TypeMask mask, void* func,
                     HSD_ForeachArgType argType, ...)
{
    typedef struct HSDVaList {
        u8 gpr;
        u8 fpr;
        u16 reserved;
        u32* overflow;
        u32* saveArea;
    } HSDVaList;
    typedef HSDVaList HSDVaListArray[1];
    extern void* __va_arg(void*, u32);
    extern const u8 lbl_80275780[];
    extern const u8 lbl_802757A0[];
    HSDVaListArray ap;
    HSD_ForeachArg arg;

    if (obj == NULL) {
        return;
    }

    __builtin_va_info(&ap);
    switch (argType) {
    case HSD_FOREACH_A:
    case HSD_FOREACH_AO:
    case HSD_FOREACH_AOT:
        break;
    case HSD_FOREACH_AF:
    case HSD_FOREACH_AOF:
    case HSD_FOREACH_AOTF:
        arg.f = *(f64*)__va_arg(ap, 3);
        break;
    case HSD_FOREACH_AV:
    case HSD_FOREACH_AOV:
    case HSD_FOREACH_AOTV:
        arg.v = *(void**)__va_arg(ap, 1);
        break;
    case HSD_FOREACH_AU:
    case HSD_FOREACH_AOU:
    case HSD_FOREACH_AOTU:
        arg.d = *(u32*)__va_arg(ap, 1);
        break;
    default:
        HSD_Panic((const char*)lbl_8047DF40, 0x33A,
                  (const char*)lbl_80275780);
        break;
    }

    switch (type) {
    case JOBJ_TYPE:
        JObjForeachAnim(obj, mask, func, argType, &arg);
        break;
    case DOBJ_TYPE:
        DObjForeachAnim(obj, mask, func, argType, &arg);
        break;
    case MOBJ_TYPE: {
        HSD_MObj* mobj = obj;
        HSD_TObj* tobj;
        if (mobj != NULL) {
            if ((mask & MOBJ_MASK) && mobj->aobj != NULL) {
                HSD_FOREACH_INVOKE(mobj->aobj, mobj, MOBJ_TYPE, func,
                                   argType, &arg);
            }
            for (tobj = mobj->tobj; tobj != NULL; tobj = tobj->next) {
                if ((mask & TOBJ_MASK) && tobj->aobj != NULL) {
                    HSD_FOREACH_INVOKE(tobj->aobj, tobj, TOBJ_TYPE, func,
                                       argType, &arg);
                }
            }
        }
        break;
    }
    case POBJ_TYPE: {
        BattleGridPObjAnim* pobj;
        for (pobj = obj; pobj != NULL; pobj = pobj->next) {
            if ((mask & POBJ_MASK) && pobj->aobj != NULL) {
                HSD_FOREACH_INVOKE(pobj->aobj, pobj, POBJ_TYPE, func,
                                   argType, &arg);
            }
        }
        break;
    }
    case TOBJ_TYPE: {
        HSD_TObj* tobj;
        for (tobj = obj; tobj != NULL; tobj = tobj->next) {
            if ((mask & TOBJ_MASK) && tobj->aobj != NULL) {
                HSD_FOREACH_INVOKE(tobj->aobj, tobj, TOBJ_TYPE, func,
                                   argType, &arg);
            }
        }
        break;
    }
    case LOBJ_TYPE:
        LObjForeachAnim(obj, mask, func, argType, &arg);
        break;
    case COBJ_TYPE:
        CObjForeachAnim(obj, mask, func, argType, &arg);
        break;
    case ROBJ_TYPE: {
        HSD_RObj* robj;
        for (robj = obj; robj != NULL; robj = robj->next) {
            if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
                HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func,
                                   argType, &arg);
            }
        }
        break;
    }
    case WOBJ_TYPE: {
        HSD_WObj* wobj = obj;
        HSD_RObj* robj;
        if (wobj != NULL) {
            if ((mask & WOBJ_MASK) && wobj->aobj != NULL) {
                HSD_FOREACH_INVOKE(wobj->aobj, wobj, WOBJ_TYPE, func,
                                   argType, &arg);
            }
            for (robj = wobj->robj; robj != NULL; robj = robj->next) {
                if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
                    HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func,
                                       argType, &arg);
                }
            }
        }
        break;
    }
    case FOG_TYPE: {
        HSD_Fog* fog = obj;
        if ((mask & FOG_MASK) && fog != NULL && fog->aobj != NULL) {
            HSD_FOREACH_INVOKE(fog->aobj, fog, FOG_TYPE, func, argType, &arg);
        }
        break;
    }
    default:
        HSD_Panic((const char*)lbl_8047DF40, 0x35E,
                  (const char*)lbl_802757A0);
        break;
    }
}

/**
 * JObjForeachAnim - HSD_JObj ForeachAnim (JObjForeachAnim).
 * Address: 0x801C0F20 | Size: 0x354
 */
void JObjForeachAnim(HSD_JObj* obj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg) {
    HSD_RObj* robj;

    (obj != NULL) ? (void)0
                  : __assert((const char*)lbl_8047DF40, 0x2CB,
                             (const char*)lbl_8047DF48);

    if ((mask & JOBJ_MASK) && obj->aobj != NULL) {
        HSD_FOREACH_INVOKE(obj->aobj, obj, JOBJ_TYPE, func, argType, arg);
    }

    if (union_type_dobj(obj)) {
        DObjForeachAnim(obj->u.dobj, mask, func, argType, arg);
    }

    for (robj = obj->robj; robj != NULL; robj = robj->next) {
        if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
            HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func, argType, arg);
        }
    }

    if (!(obj->flags & JOBJ_INSTANCE)) {
        for (obj = obj->child; obj != NULL; obj = obj->next) {
            JObjForeachAnim(obj, mask, func, argType, arg);
        }
    }
}

/**
 * DObjForeachAnim - HSD_DObj ForeachAnim (DObjForeachAnim).
 * Address: 0x801C1274 | Size: 0x59C
 */
void DObjForeachAnim(HSD_DObj* dobj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg) {
    for (; dobj != NULL; dobj = dobj->next) {
        if ((mask & DOBJ_MASK) && dobj->aobj != NULL) {
            HSD_FOREACH_INVOKE(dobj->aobj, dobj, DOBJ_TYPE, func, argType, arg);
        }

        {
            HSD_MObj* mobj = dobj->mobj;
            if (mobj != NULL) {
                HSD_TObj* tobj;

                if ((mask & MOBJ_MASK) && mobj->aobj != NULL) {
                    HSD_FOREACH_INVOKE(mobj->aobj, mobj, MOBJ_TYPE, func, argType, arg);
                }

                for (tobj = mobj->tobj; tobj != NULL; tobj = tobj->next) {
                    if ((mask & TOBJ_MASK) && tobj->aobj != NULL) {
                        HSD_FOREACH_INVOKE(tobj->aobj, tobj, TOBJ_TYPE, func, argType, arg);
                    }
                }
            }
        }

        {
            BattleGridPObjAnim* pobj;

            for (pobj = (BattleGridPObjAnim*)dobj->pobj; pobj != NULL;
                 pobj = pobj->next) {
                if ((mask & POBJ_MASK) && pobj->aobj != NULL) {
                    HSD_FOREACH_INVOKE(pobj->aobj, pobj, POBJ_TYPE, func, argType, arg);
                }
            }
        }
    }
}

/**
 * LObjForeachAnim - HSD_LObj ForeachAnim (LObjForeachAnim).
 * Address: 0x801C1810 | Size: 0x6F0
 */
void LObjForeachAnim(HSD_LObj* lobj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg) {
    HSD_WObj* wobj;
    HSD_RObj* robj;

    for (; lobj != NULL; lobj = lobj->next) {
        if ((mask & LOBJ_MASK) && lobj->aobj != NULL) {
            HSD_FOREACH_INVOKE(lobj->aobj, lobj, LOBJ_TYPE, func, argType, arg);
        }

        wobj = lobj->position;
        if (wobj != NULL) {
            if ((mask & WOBJ_MASK) && wobj->aobj != NULL) {
                HSD_FOREACH_INVOKE(wobj->aobj, wobj, WOBJ_TYPE, func, argType, arg);
            }
            for (robj = wobj->robj; robj != NULL; robj = robj->next) {
                if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
                    HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func, argType, arg);
                }
            }
        }

        wobj = lobj->interest;
        if (wobj != NULL) {
            if ((mask & WOBJ_MASK) && wobj->aobj != NULL) {
                HSD_FOREACH_INVOKE(wobj->aobj, wobj, WOBJ_TYPE, func, argType, arg);
            }
            for (robj = wobj->robj; robj != NULL; robj = robj->next) {
                if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
                    HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func, argType, arg);
                }
            }
        }
    }
}

/**
 * CObjForeachAnim - HSD_CObj ForeachAnim (CObjForeachAnim).
 * Address: 0x801C1F00 | Size: 0x6E4
 */
void CObjForeachAnim(HSD_CObj* cobj, HSD_TypeMask mask, void* func,
                  HSD_ForeachArgType argType, HSD_ForeachArg* arg) {
    HSD_WObj* wobj;
    HSD_RObj* robj;

    if (cobj == NULL) {
        return;
    }

    if ((mask & COBJ_MASK) && cobj->aobj != NULL) {
        HSD_FOREACH_INVOKE(cobj->aobj, cobj, COBJ_TYPE, func, argType, arg);
    }

    wobj = cobj->eyepos;
    if (wobj != NULL) {
        if ((mask & WOBJ_MASK) && wobj->aobj != NULL) {
            HSD_FOREACH_INVOKE(wobj->aobj, wobj, WOBJ_TYPE, func, argType, arg);
        }
        for (robj = wobj->robj; robj != NULL; robj = robj->next) {
            if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
                HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func, argType, arg);
            }
        }
    }

    wobj = cobj->interest;
    if (wobj != NULL) {
        if ((mask & WOBJ_MASK) && wobj->aobj != NULL) {
            HSD_FOREACH_INVOKE(wobj->aobj, wobj, WOBJ_TYPE, func, argType, arg);
        }
        for (robj = wobj->robj; robj != NULL; robj = robj->next) {
            if ((mask & ROBJ_MASK) && robj->aobj != NULL) {
                HSD_FOREACH_INVOKE(robj->aobj, robj, ROBJ_TYPE, func, argType, arg);
            }
        }
    }
}

/**
 * HSD_AObjRemove - Tear down an AObj and return it to the allocator.
 *
 * HSD_AObjSetFObj / HSD_AObjFree are inlined by MWCC at every call site in
 * this TU, which is why the redundant NULL re-tests below are load-bearing.
 * Address: 0x801C25E4 | Size: 0x8C
 */
void HSD_AObjRemove(HSD_AObj* aobj) {
    if (!aobj) {
        return;
    }

    /* inlined HSD_AObjSetFObj(aobj, NULL) */
    if (aobj) {
        if (aobj->fobj) {
            HSD_FObjRemoveAll(aobj->fobj);
        }
        aobj->fobj = NULL;
    }

    if (aobj) {
        if (aobj->hsd_obj != NULL) {
            HSD_JObjUnref(aobj->hsd_obj);
        }
        aobj->hsd_obj = NULL;
    }

    /* inlined HSD_AObjFree(aobj) */
    if (aobj) {
        HSD_ObjFree(lbl_80466DB8, aobj);
    }
}

/**
 * HSD_AObjLoadDesc - Instantiate an AObj from its descriptor.
 *
 * HSD_AObjAlloc (assert + memset + defaults), HSD_AObjSetFlags,
 * HSD_AObjSetRewindFrame, HSD_AObjSetEndFrame and HSD_AObjSetFObj are all
 * inlined here by MWCC.
 * Address: 0x801C2670 | Size: 0x184
 */
HSD_AObj* HSD_AObjLoadDesc(HSD_AObjDesc* aobjdesc) {
    HSD_FObjDesc* fobjdesc;
    HSD_AObj* aobj;

    u8 _[4];

    HSD_FObj* fobj;
    u32 id;
    HSD_Obj* phi_r30;

    if (aobjdesc != NULL) {
        HSD_AObj* new;

        /* inlined HSD_AObjAlloc() */
        new = (HSD_AObj*) HSD_ObjAlloc(lbl_80466DB8);
        (new != NULL) ? (void) 0
                      : __assert((const char*) lbl_8047DF40, 0x1E9,
                                 (const char*) lbl_8047DF4C);
        memset(new, 0, sizeof(HSD_AObj));
        new->flags = AOBJ_NO_ANIM;
        new->framerate = 1.0F;
        aobj = new;

        HSD_AObjSetFlags(aobj, aobjdesc->flags);

        /* inlined HSD_AObjSetRewindFrame(aobj, 0.0F) */
        if (aobj) {
            aobj->rewind_frame = 0.0F;
        }

        /* inlined HSD_AObjSetEndFrame(aobj, aobjdesc->end_frame) */
        if (aobj) {
            aobj->end_frame = aobjdesc->end_frame;
        }

        fobjdesc = aobjdesc->fobjdesc;
        fobj = HSD_FObjLoadDesc(fobjdesc);

        /* inlined HSD_AObjSetFObj(aobj, fobj) */
        if (aobj) {
            if (aobj->fobj) {
                HSD_FObjRemoveAll(aobj->fobj);
            }
            aobj->fobj = fobj;
        }

        id = aobjdesc->obj_id;
        if (id != 0U) {
            HSD_Obj* hsd_obj = HSD_IDGetDataFromTable(0, id, 0);
            phi_r30 = hsd_obj;
            if (hsd_obj != NULL) {
                ref_INC(hsd_obj);
            } else {
                phi_r30 =
                    (HSD_Obj*) HSD_JObjLoadJoint((void*) aobjdesc->obj_id);
            }
            if (aobj != NULL) {
                if (aobj->hsd_obj != NULL) {
                    HSD_JObjUnref(aobj->hsd_obj);
                }
                aobj->hsd_obj = phi_r30;
            }
        }
        return aobj;
    }
    return NULL;
}

/**
 * HSD_AObjInterpretAnim - Advance an AObj one frame and push the resulting
 * FObj values into @p obj through @p update_func, handling looping/rewind
 * and tallying the two end-callback counters.
 * Address: 0x801C27F4 | Size: 0x1D0
 */
void HSD_AObjInterpretAnim(HSD_AObj* aobj, void* obj,
                           HSD_ObjUpdateFunc update_func) {
    f32 rate = 0;

    if (!aobj || aobj->flags & AOBJ_NO_ANIM) {
        return;
    }

    if (aobj->flags & AOBJ_FIRST_PLAY) {
        aobj->flags &= 0xF7FFFFFF;
        rate = 0.0F;
    } else {
        rate = aobj->framerate;
        aobj->curr_frame += aobj->framerate;
    }

    if ((aobj->flags & AOBJ_LOOP) && aobj->end_frame <= aobj->curr_frame) {
        if (aobj->rewind_frame < aobj->end_frame) {
            f32 x, y;

            HSD_FObjStopAnimAll(aobj->fobj, obj, update_func, rate);
            y = aobj->end_frame - aobj->rewind_frame;
            x = aobj->curr_frame - aobj->rewind_frame;
            aobj->curr_frame = fmod(x, y) + aobj->rewind_frame;
            HSD_FObjReqAnimAll(aobj->fobj, aobj->curr_frame);
        } else {
            aobj->curr_frame = aobj->end_frame;
        }
        rate = 0.0F;
        aobj->flags |= AOBJ_REWINDED;
    } else {
        aobj->flags &= 0xFBFFFFFF;
    }

    if (aobj->flags & AOBJ_NO_UPDATE) {
        HSD_FObjInterpretAnimAll(aobj->fobj, obj, NULL, rate);
    } else {
        HSD_FObjInterpretAnimAll(aobj->fobj, obj, update_func, rate);
    }

    if (!(aobj->flags & AOBJ_LOOP) && (aobj->end_frame <= aobj->curr_frame) &&
        aobj)
    {
        HSD_FObjStopAnimAll(aobj->fobj, obj, update_func, aobj->framerate);
        aobj->flags |= AOBJ_NO_ANIM;
    }

    if (aobj->flags & AOBJ_NO_ANIM) {
        lbl_8047B390 += 1;
    } else {
        lbl_8047B38C += 1;
    }
}

/**
 * HSD_AObjReqAnim - Rewind an AObj to @p frame and re-request its FObjs.
 * Address: 0x801C29C4 | Size: 0x40
 */
void HSD_AObjReqAnim(HSD_AObj* aobj, f32 frame) {
    u32 flags;

    if (aobj == NULL) {
        return;
    }

    aobj->curr_frame = frame;

    flags = aobj->flags & ~AOBJ_NO_ANIM;
    aobj->flags = flags | AOBJ_FIRST_PLAY;

    HSD_FObjReqAnimAll(aobj->fobj, frame);
}

/**
 * HSD_AObjInvokeCallBacks - Run the registered end-of-animation callbacks,
 * but only once every AObj interpreted this frame has finished.
 * Address: 0x801C2A04 | Size: 0x5C
 */
void HSD_AObjInvokeCallBacks(void) {
    HSD_SList* list;

    if (lbl_8047B390 != 0 && lbl_8047B38C == 0) {
        list = lbl_8047B388;
        while (list != NULL) {
            void (*func)(void) = (void (*)(void))list->data;
            (*func)();
            list = list->next;
        }
    }
}

/**
 * HSD_AObjInitEndCallBack - Reset the two per-frame AObj counters that
 * HSD_AObjInvokeCallBacks gates on.
 * Address: 0x801C2A60 | Size: 0x14
 */
/* Compiled at optimization_level 0: at -O4 MWCC CSEs the two zero constants
 * into a single register, but the target materialises `0` twice (li r3,0 /
 * li r0,0). */
#pragma push
#pragma optimization_level 0
void HSD_AObjInitEndCallBack(void) {
    lbl_8047B390 = 0;
    lbl_8047B38C = 0;
}
#pragma pop

/**
 * HSD_AObjClearFlags - Clear the user-settable playback flags.
 * Address: 0x801C2A74 | Size: 0x1C
 */
static inline int AObjUserFlagMask(int bit) {
    return (1 << bit) | (1 << 28);
}

void HSD_AObjClearFlags(HSD_AObj* aobj, u32 flags) {
    if (aobj) {
        flags &= AObjUserFlagMask(29);
        flags = flags & 0xFFFFFFFFFFFFFFFFu;
        aobj->flags &= ~flags;
    }
}

/**
 * HSD_AObjSetFlags - Set the user-settable playback flags.
 * Address: 0x801C2A90 | Size: 0x1C
 */
void HSD_AObjSetFlags(HSD_AObj* aobj, u32 flags) {
    if (aobj) {
        flags &= (AOBJ_LOOP | AOBJ_NO_UPDATE);
        if (!flags) {
            flags++;
            flags--;
        }
        aobj->flags |= flags;
    }
}

/**
 * HSD_AObjGetAllocData - Get the AObj allocator pool.
 * Address: 0x801C2AAC | Size: 0xC
 */
void* HSD_AObjGetAllocData(void) {
    return lbl_80466DB8;
}

/**
 * HSD_AObjInitAllocData - Initialise the AObj allocator pool.
 * Address: 0x801C2AB8 | Size: 0x30
 */
void HSD_AObjInitAllocData(s32 slot, s32 animState) {
    HSD_ObjAllocInit(lbl_80466DB8, sizeof(HSD_AObj), 4);
}
