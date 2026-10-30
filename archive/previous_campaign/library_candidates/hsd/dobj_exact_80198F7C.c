/**
 * @file dobj_exact_80198F7C.c
 * @brief sysdolphin dobj.c, class and load head: .text 0x80198F7C-0x801993A4.
 *
 * DObjInfoInit, DObjAmnesia, DObjRelease, HSD_DObjDisp,
 * HSD_DObjCountVertices, HSD_DObjResolveRefsAll, HSD_DObjRemoveAll and
 * HSD_DObjLoadDesc. Built with the sysdolphin library flags (GC/1.3.2
 * -O4,p -O1 -inline auto,deferred -use_lmw_stmw on). Deferred inlining emits
 * functions in reverse definition order, so they are listed here in
 * HAL/Melee source order (last address first).
 *
 * HSD_DObjAlloc and HSD_DObjResolveRefs have no retail symbol. HSD_DObjAlloc
 * is expanded twice (here and in DObjLoad, same assert line 532), and
 * HSD_DObjResolveRefsAll re-tests the helper's dobj/desc guard inside the
 * loop, so both are recovered as static inline.
 *
 * Text-only unit: hsdDObj (.data), the class/format strings (.rodata), the
 * assert strings (.sdata2) and default_class (.sbss 0x8047B260) are owned by
 * other units and stay extern. DObjInfoInit stays global because hsdDObj,
 * which points at it, lives in the carved data unit, and DObjLoad (the next
 * function, 0x801993A4) is its own linked unit, src/hsd/dobj_exact_801993A4.c.
 */

#include "hsd/hsd_dobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_pobj.h"

/* hsdSearchClassInfo */
extern HSD_ClassInfo* fn_80193748(const char* class_name);
/* hsdNew */
extern void* fn_80193828(HSD_ClassInfo* info);
/* HSD_PObjCountVertices */
extern void fn_801ACDAC(HSD_PObj* pobj, s32* total_a, s32* total_b);
extern void __assert(const char* file, u32 line, const char* expr);

extern HSD_DObjInfo lbl_8036C7A0;  /* hsdDObj */
extern HSD_ClassInfo lbl_8036C638; /* hsdClass */
extern HSD_ClassInfo* lbl_8047B260; /* default_class */
extern char lbl_80274708[];        /* "sysdolphin_base_library" */
extern char lbl_80274720[];        /* "hsd_dobj" */
extern char lbl_8047DA18[7];       /* "dobj.c" */
extern char lbl_8047DA20[5];       /* "dobj" */

void DObjInfoInit(void);
int DObjLoad(HSD_DObj* dobj, HSD_DObjDesc* desc);

static inline HSD_DObj* HSD_DObjAlloc(void);

HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc)
{
    HSD_DObj* dobj;
    HSD_ClassInfo* info;

    if (desc == NULL) {
        return NULL;
    }

    if (desc->class_name == NULL ||
        (info = fn_80193748(desc->class_name)) == NULL)
    {
        dobj = HSD_DObjAlloc();
    } else {
        dobj = HSD_DOBJ(fn_80193828(info));
        if (dobj == NULL) {
            __assert(lbl_8047DA18, 385, lbl_8047DA20);
        }
    }
    HSD_DOBJ_METHOD(dobj)->load(dobj, desc);

    return dobj;
}

void HSD_DObjRemoveAll(HSD_DObj* dobj)
{
    HSD_DObj* next;

    for (; dobj != NULL; dobj = next) {
        next = dobj->next;
        hsdDelete(dobj);
    }
}

static inline HSD_DObj* HSD_DObjAlloc(void)
{
    HSD_DObj* dobj = fn_80193828(lbl_8047B260 != NULL
                                     ? lbl_8047B260
                                     : HSD_CLASS_INFO(&lbl_8036C7A0));
    if (dobj == NULL) {
        __assert(lbl_8047DA18, 532, lbl_8047DA20);
    }
    return dobj;
}

static inline void HSD_DObjResolveRefs(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    if (dobj == NULL || desc == NULL) {
        return;
    }
    HSD_PObjResolveRefsAll(dobj->pobj, desc->pobjdesc);
}

void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    for (; dobj != NULL && desc != NULL; dobj = dobj->next, desc = desc->next)
    {
        HSD_DObjResolveRefs(dobj, desc);
    }
}

void HSD_DObjCountVertices(HSD_DObj* dobj, s32* total_a, s32* total_b)
{
    s32 sum_a = 0;
    s32 sum_b = 0;

    for (; dobj != NULL; dobj = dobj->next) {
        s32 a;
        s32 b;

        fn_801ACDAC(dobj->pobj, &a, &b);
        sum_a += a;
        sum_b += b;
    }
    if (total_a != NULL) {
        *total_a = sum_a;
    }
    if (total_b != NULL) {
        *total_b = sum_b;
    }
}

void HSD_DObjDisp(HSD_DObj* dobj, f32 vmtx[3][4], f32 pmtx[3][4],
                  u32 rendermode)
{
    HSD_PObj* p;

    HSD_MObjSetCurrent(dobj->mobj);
    if ((rendermode & RENDER_SHADOW) == 0) {
        HSD_MOBJ_METHOD(dobj->mobj)->setup(dobj->mobj, rendermode);
    }
    for (p = dobj->pobj; p != NULL; p = p->next) {
        HSD_POBJ_METHOD(p)->disp(p, vmtx, pmtx, rendermode);
    }
    if ((rendermode & RENDER_SHADOW) == 0) {
        HSD_MOBJ_METHOD(dobj->mobj)->unset(dobj->mobj, rendermode);
    }
    HSD_MObjSetCurrent(NULL);
}

static void DObjRelease(HSD_Class* o)
{
    HSD_DObj* dobj = HSD_DOBJ(o);

    HSD_MObjRemove(dobj->mobj);
    HSD_PObjRemoveAll(dobj->pobj);
    HSD_AObjRemove(dobj->aobj);

    HSD_PARENT_INFO(&lbl_8036C7A0)->release(o);
}

static void DObjAmnesia(HSD_ClassInfo* info)
{
    if (info == lbl_8047B260) {
        lbl_8047B260 = NULL;
    }
    HSD_PARENT_INFO(&lbl_8036C7A0)->amnesia(info);
}

void DObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&lbl_8036C7A0), &lbl_8036C638,
                     lbl_80274708, lbl_80274720, sizeof(HSD_DObjInfo),
                     sizeof(HSD_DObj));

    HSD_CLASS_INFO(&lbl_8036C7A0)->release = DObjRelease;
    HSD_CLASS_INFO(&lbl_8036C7A0)->amnesia = DObjAmnesia;
    HSD_DOBJ_INFO(&lbl_8036C7A0)->disp = HSD_DObjDisp;
    HSD_DOBJ_INFO(&lbl_8036C7A0)->load = DObjLoad;
}
