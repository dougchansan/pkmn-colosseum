/**
 * @file hsd_dobj.c
 * @brief HSD DObj - Display object implementation.
 *
 * Colosseum address: 0x80198F7C (HSD_DObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/dobj.c
 */

#include "hsd/hsd_dobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_pobj.h"

static void fn_80198F7C(void);

extern HSD_DObjInfo hsdDObj;
extern u8 lbl_8036C638[];
extern char lbl_80274708[];
extern char lbl_80274720[];
extern HSD_ClassInfo lbl_8036C7A0;
extern HSD_ClassInfo* lbl_8047B260;
static HSD_ClassInfo* default_class = NULL;

/* ========================================================================= */
/*  Flag accessors                                                           */
/* ========================================================================= */

u32 HSD_DObjGetFlags(HSD_DObj* dobj)
{
    HSD_ASSERT(0, dobj);
    return dobj->flags;
}

void HSD_DObjSetFlags(HSD_DObj* dobj, u32 flags)
{
    HSD_ASSERT(0, dobj);
    dobj->flags |= flags;
}

void HSD_DObjClearFlags(HSD_DObj* dobj, u32 flags)
{
    HSD_ASSERT(0, dobj);
    dobj->flags &= ~flags;
}

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_DObjAddAnim(HSD_DObj* dobj, HSD_MatAnim* mat_anim,
                     HSD_ShapeAnimDObj* sh_anim)
{
    if (dobj == NULL) {
        return;
    }
    HSD_MObjAddAnim(dobj->mobj, mat_anim);
    if (sh_anim != NULL) {
        HSD_PObjAddAnim(dobj->pobj, sh_anim->shapeanim);
    }
}

void HSD_DObjAddAnimAll_Early(HSD_DObj* dobj, HSD_MatAnim* matanim,
                        HSD_ShapeAnimDObj* shapeanimdobj)
{
    HSD_DObj* d;
    HSD_MatAnim* ma;
    HSD_ShapeAnimDObj* sa;

    d = dobj;
    ma = matanim;
    sa = shapeanimdobj;

    while (d != NULL) {
        HSD_DObjAddAnim(d, ma, sa);
        d = d->next;
        if (ma != NULL) ma = ma->next;
        if (sa != NULL) sa = sa->next;
    }
}

void HSD_DObjReqAnimAll(HSD_DObj* dobj, f32 startframe)
{
    HSD_DObj* d;
    for (d = dobj; d != NULL; d = d->next) {
        HSD_MObjReqAnim(d->mobj, startframe);
        /* PObj shape anim req would go here */
    }
}

void HSD_DObjAnim(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        HSD_MObjAnim(dobj->mobj);
        HSD_PObjAnim(dobj->pobj);
    }
}

void HSD_DObjAnimAll_Early(HSD_DObj* dobj)
{
    HSD_DObj* d;
    for (d = dobj; d != NULL; d = d->next) {
        HSD_DObjAnim(d);
    }
}

/* ========================================================================= */
/*  Load                                                                     */
/* ========================================================================= */

static int DObjLoad_Early(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    if (dobj->mobj != NULL) {
        HSD_MObjRemove(dobj->mobj);
    }
    dobj->mobj = HSD_MObjLoadDesc(desc->mobjdesc);
    if (dobj->pobj != NULL) {
        HSD_PObjRemoveAll(dobj->pobj);
    }
    dobj->pobj = HSD_PObjLoadDesc(desc->pobjdesc);
    return 0;
}

/* NOTE: HSD_DObjLoadDesc's real body lives in the address-scaffolded
 * section below (0x801992D8); that's the disassembled target. */

/* ========================================================================= */
/*  Resolve refs                                                             */
/* ========================================================================= */

void HSD_DObjResolveRefs(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    if (dobj != NULL && desc != NULL) {
        HSD_PObjResolveRefsAll(dobj->pobj, desc->pobjdesc);
    }
}

void HSD_DObjResolveRefsAll_Early(HSD_DObj* dobj, HSD_DObjDesc* desc)
{
    HSD_DObj* d;
    HSD_DObjDesc* dd;

    d = dobj;
    dd = desc;
    while (d != NULL && dd != NULL) {
        HSD_DObjResolveRefs(d, dd);
        d = d->next;
        dd = dd->next;
    }
}

/* ========================================================================= */
/*  Remove                                                                   */
/* ========================================================================= */

void HSD_DObjRemove(HSD_DObj* dobj)
{
    if (dobj != NULL) {
        HSD_CLASS_METHOD(dobj)->release((HSD_Class*) dobj);
        HSD_CLASS_METHOD(dobj)->destroy((HSD_Class*) dobj);
    }
}

/* NOTE: HSD_DObjRemoveAll's real body lives in the address-scaffolded
 * section below (0x80199264); that's the disassembled target. */

/* ========================================================================= */
/*  Alloc                                                                    */
/* ========================================================================= */

HSD_DObj* HSD_DObjAlloc(void)
{
    HSD_DObj* dobj;
    dobj = (HSD_DObj*) hsdNew(
        default_class ? default_class : (HSD_ClassInfo*) &hsdDObj);
    HSD_ASSERT(0, dobj);
    return dobj;
}

void HSD_DObjSetDefaultClass(HSD_ClassInfo* info)
{
    if (info) {
        HSD_ASSERT(0, hsdIsDescendantOf(info, &hsdDObj));
    }
    default_class = info;
}

/* ========================================================================= */
/*  Class lifecycle                                                          */
/* ========================================================================= */

/* Forward decls of vtable entries wired by DObjInfoInit. */
static void DObjRelease(HSD_Class* o);
static void fn_80199014(HSD_ClassInfo* info);
static void fn_801990B8(HSD_DObj* dobj, f32 vmtx[3][4], f32 pmtx[3][4], u32 rendermode);
static int DObjLoad(HSD_DObj* dobj, HSD_DObjDesc* desc);

#pragma push
#pragma optimization_level 0
static void fn_80198F7C(void)
{
    hsdInitClassInfo((HSD_ClassInfo*) &hsdDObj, (HSD_ClassInfo*) lbl_8036C638,
                     lbl_80274708, lbl_80274720,
                     sizeof(HSD_DObjInfo), sizeof(HSD_DObj));
    hsdDObj.parent.release = DObjRelease;
    hsdDObj.parent.amnesia = (void (*)(HSD_ClassInfo*)) fn_80199014;
    hsdDObj.disp = fn_801990B8;
    hsdDObj.load = DObjLoad;
}
#pragma pop

/* ===================================================================
 * WP-0061: asm wrappers
 * Range: 0x80199014 - 0x80199794
 * =================================================================== */

/* 0x80199014 | 0x48 */
#pragma push
#pragma optimization_level 1
static void fn_80199014(HSD_ClassInfo* info)
{
    if (info == lbl_8047B260) {
        lbl_8047B260 = NULL;
    }
    (&lbl_8036C7A0)->head.parent->amnesia(info);
}
#pragma pop

/* 0x8019905C | 0x5C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_MObjRemove(HSD_MObj* mobj);
extern void HSD_PObjRemoveAll(HSD_PObj* pobj);
extern void fn_801C25E4(HSD_AObj* aobj);
#if 0
asm void DObjRelease(void) {
#include "src/hsd/hsd_dobj_fn_8019905C.inc"
}
#else
#pragma optimization_level 4
static void DObjRelease(HSD_Class* o)
{
    HSD_DObj* dobj = HSD_DOBJ(o);
    HSD_MObjRemove(dobj->mobj);
    HSD_PObjRemoveAll(dobj->pobj);
    fn_801C25E4(dobj->aobj);
    HSD_PARENT_INFO(&hsdDObj)->release(o);
}
#endif
#pragma pop

/* 0x801990B8 | 0xC0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_MObjSetCurrent(HSD_MObj* mobj);
#if 0
asm void fn_801990B8(void) {
#include "src/hsd/hsd_dobj_fn_801990B8.inc"
}
#else
#pragma optimization_level 1
#pragma use_lmw_stmw on
static void fn_801990B8(HSD_DObj* dobj, f32 vmtx[3][4], f32 pmtx[3][4], u32 rendermode)
{
    HSD_PObj* pobj;
    HSD_MObjSetCurrent(dobj->mobj);
    if (!(rendermode & 0x04000000)) {
        HSD_MOBJ_METHOD(dobj->mobj)->setup(dobj->mobj, rendermode);
    }
    for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
        HSD_POBJ_METHOD(pobj)->disp(pobj, vmtx, pmtx, rendermode);
    }
    if (!(rendermode & 0x04000000)) {
        HSD_MOBJ_METHOD(dobj->mobj)->unset(dobj->mobj, rendermode);
    }
    HSD_MObjSetCurrent(NULL);
}
#endif
#pragma pop

/* 0x80199178 | 0x80 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_801ACDAC(HSD_PObj* pobj, s32* out1, s32* out2);
#if 0
asm void HSD_DObjCountVertices(void) {
#include "src/hsd/hsd_dobj_HSD_DObjCountVertices.inc"
}
#else
#pragma optimization_level 4
void HSD_DObjCountVertices(HSD_DObj* dobj, s32* total_a, s32* total_b) {
    s32 sum_a;
    s32 sum_b;
    s32 a;
    s32 b;
    sum_a = 0;
    sum_b = 0;
    while (dobj != NULL) {
        fn_801ACDAC(dobj->pobj, &a, &b);
        dobj = dobj->next;
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
#endif
#pragma pop

/* 0x801991F8 | 0x6C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_PObjResolveRefsAll(HSD_PObj* pobj, HSD_PObjDesc* desc);
#if 0
asm void HSD_DObjResolveRefsAll(void) {
#include "src/hsd/hsd_dobj_HSD_DObjResolveRefsAll.inc"
}
#else
#pragma optimization_level 4
void HSD_DObjResolveRefsAll(HSD_DObj* dobj, HSD_DObjDesc* desc) {
    HSD_DObjDesc* dd;
    HSD_DObj* d;
    dd = desc;
    d = dobj;
    while (d != NULL && dd != NULL) {
        if (d != NULL && dd != NULL) {
            HSD_PObjResolveRefsAll(d->pobj, dd->pobjdesc);
        }
        d = d->next;
        dd = dd->next;
    }
}
#endif
#pragma pop

/* 0x80199264 | 0x74 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_DObjRemoveAll(void) {
#include "src/hsd/hsd_dobj_fn_80199264.inc"
}
#else
#pragma optimization_level 4

/* inc-label externs, hoisted before asm blocks (add_inc_externs.py) */
extern u8 lbl_80274758[];
extern u8 lbl_802747AC[];
extern u8 lbl_80465378[];

void HSD_DObjRemoveAll(HSD_DObj* dobj) {
    HSD_DObj* next;
    HSD_DObj* d;
    d = dobj;
    while (d != NULL) {
        next = d->next;
        if (d != NULL) {
            HSD_CLASS_METHOD(d)->release((HSD_Class*) d);
            HSD_CLASS_METHOD(d)->destroy((HSD_Class*) d);
        }
        d = next;
    }
}
#endif
#pragma pop

/* 0x801992D8 | 0xCC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern HSD_ClassInfo* fn_80193748(const char* class_name);
extern void* fn_80193828(HSD_ClassInfo* info);
extern char lbl_8047DA18;
extern char lbl_8047DA20;
#if 0
asm void HSD_DObjLoadDesc(void) {
#include "src/hsd/hsd_dobj_fn_801992D8.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma optimization_level 4
HSD_DObj* HSD_DObjLoadDesc(HSD_DObjDesc* desc)
{
    HSD_DObj* dobj;
    HSD_ClassInfo* info;

    if (desc == NULL) {
        return NULL;
    }

    if (*(volatile u32*) &desc->class_name == 0
        || (info = fn_80193748(desc->class_name)) == NULL)
    {
        if (*(volatile u32*) &default_class != 0) {
            info = default_class;
        } else {
            info = (HSD_ClassInfo*) &hsdDObj;
        }
        dobj = (HSD_DObj*) fn_80193828(info);
        if (dobj == NULL) {
            __assert(&lbl_8047DA18, 0x214, &lbl_8047DA20);
        }
    } else {
        dobj = (HSD_DObj*) fn_80193828(info);
        if (dobj == NULL) {
            __assert(&lbl_8047DA18, 0x181, &lbl_8047DA20);
        }
    }

    {
        void (**vtbl)(void) = (void (**)(void)) HSD_CLASS_METHOD(dobj);
        ((int (*)(HSD_DObj*, HSD_DObjDesc*)) vtbl[0x40 / 4])(dobj, desc);
    }
    return dobj;
}
#pragma pop
#endif
#pragma pop

/* 0x801993A4 | 0x1C4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void OSReport(const char* fmt, ...);
extern HSD_PObj* HSD_PObjLoadDesc(HSD_PObjDesc* pobjdesc);
extern void HSD_Panic(const char* file, s32 line, const char* msg);
extern char lbl_8027472C[];
extern char lbl_8047DA28;
extern char lbl_8047DA18;
extern char lbl_8047DA20;
#if 0
asm void DObjLoad(void) {
#include "src/hsd/hsd_dobj_DObjLoad.inc"
}
#else
#pragma optimization_level 4
#pragma push
#pragma optimization_level 1
static int DObjLoad(HSD_DObj* dobj_arg, HSD_DObjDesc* desc_arg)
{
    HSD_DObjDesc* desc;
    HSD_DObj* dobj;
    HSD_DObjDesc* subdesc;
    HSD_DObj* sub;
    HSD_ClassInfo* info;

    dobj = dobj_arg;
    desc = desc_arg;
    subdesc = desc->next;
    if (subdesc == NULL) {
        sub = NULL;
    } else {
        if (subdesc->class_name == NULL
            || (info = fn_80193748(subdesc->class_name)) == NULL)
        {
            info = default_class ? default_class : (HSD_ClassInfo*) &hsdDObj;
            sub = (HSD_DObj*) fn_80193828(info);
            if (sub == NULL) {
                __assert(&lbl_8047DA18, 0x214, &lbl_8047DA20);
            }
        } else {
            sub = (HSD_DObj*) fn_80193828(info);
            if (sub == NULL) {
                __assert(&lbl_8047DA18, 0x181, &lbl_8047DA20);
            }
        }
        dobj = dobj_arg;
        {
            void (**vtbl)(void) = (void (**)(void)) HSD_CLASS_METHOD(sub);
            ((int (*)(HSD_DObj*, HSD_DObjDesc*)) vtbl[0x40 / 4])(sub, subdesc);
        }
    }
    dobj->next = sub;
    dobj->mobj = HSD_MObjLoadDesc(desc->mobjdesc);
    dobj->pobj = HSD_PObjLoadDesc(desc->pobjdesc);

    if (dobj->mobj != NULL) {
        u32 type = dobj->mobj->rendermode;
        type = type & 0x60000000;
        switch (type) {
        case 0x00000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x02;
            }
            break;
        case 0x40000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x08;
            }
            break;
        case 0x60000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x04;
            }
            break;
        default:
            OSReport(lbl_8027472C, dobj->mobj->rendermode);
            HSD_Panic(&lbl_8047DA18, 0x13F, &lbl_8047DA28);
            break;
        }
    }
    return 0;
}
#pragma pop
#endif
#pragma pop

/* 0x80199568 | 0x6C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_MObjAnim(HSD_MObj* mobj);
extern void HSD_PObjAnimAll(HSD_PObj* pobj);
extern void fn_801C27F4(HSD_AObj* aobj, HSD_DObj* dobj, void* method);
#if 0
asm void HSD_DObjAnimAll(void) {
#include "src/hsd/hsd_dobj_HSD_DObjAnimAll.inc"
}
#else
#pragma optimization_level 4
void HSD_DObjAnimAll(HSD_DObj* dobj) {
    HSD_DObj* d;
    if (dobj == NULL) {
        return;
    }
    for (d = dobj; d != NULL; d = d->next) {
        if (d != NULL) {
            fn_801C27F4(d->aobj, d,
                        (void*) HSD_DOBJ_METHOD(d)->update);
            HSD_PObjAnimAll(d->pobj);
            HSD_MObjAnim(d->mobj);
        }
    }
}
#endif
#pragma pop

/* 0x801995D4 | 0x80 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_MObjReqAnimByFlags(f32 val, HSD_MObj* mobj, void* arg);
extern void HSD_PObjReqAnimAllByFlags(f32 val, HSD_PObj* pobj, void* arg);
#if 0
asm void HSD_DObjReqAnimAllByFlags(void) {
#include "src/hsd/hsd_dobj_HSD_DObjReqAnimAllByFlags.inc"
}
#else
#pragma optimization_level 4
void HSD_DObjReqAnimAllByFlags(HSD_DObj* dobj, f32 val, void* arg) {
    HSD_DObj* d;
    if (dobj == NULL) {
        return;
    }
    for (d = dobj; d != NULL; d = d->next) {
        if (d != NULL) {
            HSD_PObjReqAnimAllByFlags(val, d->pobj, arg);
            HSD_MObjReqAnimByFlags(val, d->mobj, arg);
        }
    }
}
#endif
#pragma pop

/* 0x80199654 | 0xB0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_801AD738(HSD_PObj* pobj, void* arg);
#if 0
asm void HSD_DObjAddAnimAll(void) {
#include "src/hsd/hsd_dobj_HSD_DObjAddAnimAll.inc"
}
#else
#pragma optimization_level 4
void HSD_DObjAddAnimAll(HSD_DObj* dobj, void* matanim, void* shapeanim) {
    HSD_DObj* d;
    void* ma;
    void* sa;
    if (dobj == NULL) {
        return;
    }
    d = dobj;
    ma = matanim;
    sa = shapeanim;
    while (d != NULL) {
        if (d != NULL) {
            HSD_PObj* pobj;
            void* shapeargl;
            pobj = d->pobj;
            if (sa != NULL) {
                shapeargl = *(void**)((u8*)sa + 4);
            } else {
                shapeargl = NULL;
            }
            fn_801AD738(pobj, shapeargl);
            HSD_MObjAddAnim(d->mobj, ma);
        }
        d = d->next;
        if (ma != NULL) {
            ma = *(void**)ma;
        } else {
            ma = NULL;
        }
        if (sa != NULL) {
            sa = *(void**)sa;
        } else {
            sa = NULL;
        }
    }
}
#endif
#pragma pop

/* 0x80199704 | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u32 lbl_8047B264;
#if 0
asm void HSD_DObjSetCurrent(HSD_DObj* dobj) {
#include "src/hsd/hsd_dobj_HSD_DObjSetCurrent.inc"
}
#else
void HSD_DObjSetCurrent(HSD_DObj* dobj) {
    lbl_8047B264 = (u32)dobj;
}
#endif
#pragma pop

/* 0x8019970C | 0x2C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_ObjFree(void* list, void* data);
extern u8 lbl_80465378[];
#if 0
asm void HSD_FObjFree(void) {
#include "src/hsd/hsd_dobj_HSD_FObjFree.inc"
}
#else
#pragma optimization_level 4
/* Debug hook, compiled out in the release build.  It normally vanishes into
 * its callers; the linker only ever emits the out-of-line copy (a bare blr,
 * fn_80199A84) because the inliner runs out of depth budget inside the
 * self-inlined HSD_FObjLoadDesc / HSD_FObjRemoveAll expansions. */
static void fn_80199A84(void* list, void* name)
{
}

void HSD_FObjFree(HSD_FObj* data) {
    HSD_ObjFree(lbl_80465378, data);
    fn_80199A84(lbl_80465378, lbl_802747AC);
}
#endif
#pragma pop

/* 0x80199738 | 0x5C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void* HSD_ObjAlloc(void* list);
extern void* memset(void* dst, int val, u32 size);
extern char lbl_8047DA30;
extern char lbl_8047DA38;
#if 0
asm void HSD_FObjAlloc(void) {
#include "src/hsd/hsd_dobj_HSD_FObjAlloc.inc"
}
#else
#pragma optimization_level 4
HSD_FObj* HSD_FObjAlloc(void) {
    HSD_FObj* p;
    p = HSD_ObjAlloc(lbl_80465378);
    fn_80199A84(lbl_80465378, lbl_80274758);
    if (p == NULL) {
        __assert(&lbl_8047DA30, 755, &lbl_8047DA38);
    }
    memset(p, 0, 0x30);
    return p;
}
#endif
#pragma pop

/* 0x80199794 | 0x2F0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_FObjLoadDesc(void) {
#include "src/hsd/hsd_dobj_fn_80199794.inc"
}
#else
#pragma optimization_level 4
extern void* HSD_ObjAlloc(void* list);
extern void* memset(void* dst, int val, u32 size);
extern char lbl_8047DA30;
extern char lbl_8047DA38;

#pragma inline_max_size(600)
HSD_FObj* HSD_FObjLoadDesc(HSD_FObjDesc* desc)
{
    if (desc != NULL) {
        HSD_FObj* fobj = HSD_FObjAlloc();
        fobj->next = HSD_FObjLoadDesc(desc->next);
        fobj->startframe = desc->startframe;
        fobj->obj_type = desc->type;
        fobj->frac_value = desc->frac_value;
        fobj->frac_slope = desc->frac_slope;
        fobj->ad_head = desc->ad;
        fobj->length = desc->length;
        fobj->flags = 0;
        return fobj;
    }
    return NULL;
}
#endif
#pragma pop

/* 0x80199A88 | 0x70 */
#pragma push
#pragma optimization_level 4
extern void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                                  HSD_ObjUpdateFunc obj_update, f32 rate);
void HSD_FObjInterpretAnimAll(void* fobj, void* obj,
                              HSD_ObjUpdateFunc obj_update, f32 rate)
{
    HSD_FObj* f = (HSD_FObj*) fobj;
    while (f != NULL) {
        HSD_FObjInterpretAnim(f, obj, obj_update, rate);
        f = f->next;
    }
}
#pragma pop

/* Melee baselib/fobj.c positional port.  These helpers are intentionally
 * inline: FObjLoadData's target body contains all of them expanded. */
extern char lbl_8027477C[];
extern const f32 lbl_8047DA3C;

static inline u32 FObjGetStateInline(HSD_FObj* fobj)
{
    if (fobj == NULL) {
        return 0;
    }
    return fobj->flags & 0xF;
}

static inline u32 FObjSetStateInline(HSD_FObj* fobj, u32 state)
{
    if (fobj != NULL) {
        fobj->flags = (state & 0xF) | (fobj->flags & 0xF0);
    }
    return state;
}

static inline f32 FObjParseFloat(u8** pos, u8 frac)
{
    union {
        f32 f;
        u32 d;
    } value;
    f32 numerator;
    s32 denominator;

    if (frac == HSD_A_FRAC_FLOAT) {
        value.d = (s32)((*pos)++)[0];
        value.d |= ((*pos)++)[0] << 8;
        value.d |= ((*pos)++)[0] << 16;
        value.d |= ((*pos)++)[0] << 24;
        return value.f;
    }

    denominator = 1 << (frac & 0x1F);
    switch (frac & 0xE0) {
    case HSD_A_FRAC_S8:
        numerator = (s8)(*pos)[0];
        *pos += 1;
        break;
    case HSD_A_FRAC_U8:
        numerator = (*pos)[0];
        *pos += 1;
        break;
    case HSD_A_FRAC_S16:
        numerator = ((s8)(*pos)[1] << 8) | (*pos)[0];
        *pos += 2;
        break;
    case HSD_A_FRAC_U16:
        numerator = ((*pos)[1] << 8) | (*pos)[0];
        *pos += 2;
        break;
    default:
        return 0.0f;
    }
    return numerator / denominator;
}

static inline u8 FObjParseOpCode(u8** pos)
{
    return **pos & 0xF;
}

static inline u32 FObjParsePackInfo(u8** pos)
{
    u8 data;
    u32 count;
    s32 shift;

    data = *(*pos)++;
    count = ((data >> 4) & 7) + 1;
    shift = 3;
    if (!(data & 0x80)) {
        return count;
    }
    do {
        data = *(*pos)++;
        count += (data & 0x7F) << shift;
        shift += 7;
    } while (data & 0x80);
    return count;
}

static inline void FObjLaunchKeyDataInline(HSD_FObj* fobj)
{
    if (fobj->flags & 0x40) {
        fobj->op_intrp = fobj->op;
        fobj->flags &= ~0x40;
        fobj->flags |= 0x80;
        fobj->p0 = fobj->p1;
    }
}

static inline void FObjAssertDataState(HSD_FObj* fobj, u32 line)
{
    u32 state = FObjGetStateInline(fobj);
    if (!(state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA)) {
        __assert(&lbl_8047DA30, line, lbl_8027477C);
    }
}

static inline u32 FObjAnimCONInline(HSD_FObj* fobj)
{
    u32 state = FObjGetStateInline(fobj);
    HSD_ASSERTMSG(0x17F, state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA,
                  lbl_8027477C);
    fobj->p0 = fobj->p1;
    fobj->p1 = FObjParseFloat(&fobj->ad, fobj->frac_value);
    if (fobj->op_intrp != HSD_A_OP_SLP) {
        fobj->d0 = fobj->d1;
        fobj->d1 = 0.0f;
    }
    return FObjSetStateInline(fobj, state == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimLinearInline(HSD_FObj* fobj)
{
    u32 state = FObjGetStateInline(fobj);
    HSD_ASSERTMSG(0x193, state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA,
                  lbl_8027477C);
    fobj->p0 = fobj->p1;
    fobj->p1 = FObjParseFloat(&fobj->ad, fobj->frac_value);
    if (fobj->op_intrp != HSD_A_OP_SLP) {
        fobj->d0 = fobj->d1;
        fobj->d1 = 0.0f;
    }
    return FObjSetStateInline(fobj, state == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimSPL0Inline(HSD_FObj* fobj)
{
    u32 state = FObjGetStateInline(fobj);
    HSD_ASSERTMSG(0x1A7, state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA,
                  lbl_8027477C);
    fobj->p0 = fobj->p1;
    fobj->d0 = fobj->d1;
    fobj->p1 = FObjParseFloat(&fobj->ad, fobj->frac_value);
    fobj->d1 = 0.0f;
    return FObjSetStateInline(fobj, state == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimSPLInline(HSD_FObj* fobj)
{
    u32 state = FObjGetStateInline(fobj);
    HSD_ASSERTMSG(0x1B9, state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA,
                  lbl_8027477C);
    fobj->p0 = fobj->p1;
    fobj->p1 = FObjParseFloat(&fobj->ad, fobj->frac_value);
    fobj->d0 = fobj->d1;
    fobj->d1 = FObjParseFloat(&fobj->ad, fobj->frac_slope);
    return FObjSetStateInline(fobj, state == FOBJ_LOAD_DATA0 ? 3 : 4);
}

static inline u32 FObjAnimSLPInline(HSD_FObj* fobj)
{
    u32 state = FObjGetStateInline(fobj);
    HSD_ASSERTMSG(0x1CC, state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA,
                  lbl_8027477C);
    fobj->d0 = fobj->d1;
    fobj->d1 = FObjParseFloat(&fobj->ad, fobj->frac_slope);
    return FObjGetStateInline(fobj);
}

static inline u32 FObjAnimKeyInline(HSD_FObj* fobj)
{
    u32 state = FObjGetStateInline(fobj);
    HSD_ASSERTMSG(0x1E9, state == FOBJ_LOAD_DATA0 || state == FOBJ_LOAD_DATA,
                  lbl_8027477C);
    FObjLaunchKeyDataInline(fobj);
    fobj->p1 = FObjParseFloat(&fobj->ad, fobj->frac_value);
    fobj->flags |= 0x40;
    return FObjSetStateInline(fobj, state == FOBJ_LOAD_DATA0 ? 3 : 4);
}

u32 FObjLoadData(HSD_FObj* fobj)
{
    if ((u32)(fobj->ad - fobj->ad_head) >= fobj->length) {
        return 6;
    }

    fobj->op_intrp = fobj->op;
    if (fobj->nb_pack == 0) {
        fobj->op = FObjParseOpCode(&fobj->ad);
        fobj->nb_pack = FObjParsePackInfo(&fobj->ad);
    }
    fobj->nb_pack -= 1;

    switch (fobj->op) {
    case HSD_A_OP_CON:
        return FObjAnimCONInline(fobj);
    case HSD_A_OP_LIN:
        return FObjAnimLinearInline(fobj);
    case HSD_A_OP_SPL0:
        return FObjAnimSPL0Inline(fobj);
    case HSD_A_OP_SPL:
        return FObjAnimSPLInline(fobj);
    case HSD_A_OP_SLP:
        return FObjAnimSLPInline(fobj);
    case HSD_A_OP_KEY:
        return FObjAnimKeyInline(fobj);
    default:
        return 0;
    }
}

extern f32 fn_801B2560(f32 inv_duration, f32 time, f32 p0, f32 p1,
                       f32 d0, f32 d1);
extern const f64 lbl_8047DA40;
extern const f64 lbl_8047DA48;

static inline void FObjUpdateAnimInline(HSD_FObj* fobj, void* obj,
                                        HSD_ObjUpdateFunc update)
{
    HSD_ObjData data;
    f32 value;

    if (update == NULL) {
        return;
    }

    switch (fobj->op_intrp) {
    case HSD_A_OP_KEY:
        if (*(volatile u8*)&fobj->flags & 0x80) {
            data.fv = fobj->p0;
            fobj->flags = *(volatile u8*)&fobj->flags & 0xFFFFFF7F;
        } else {
            return;
        }
        break;
    case HSD_A_OP_CON:
        if (fobj->time >= fobj->fterm) {
            value = fobj->p1;
        } else {
            value = fobj->p0;
        }
        data.fv = value;
        break;
    case HSD_A_OP_LIN:
        if (*(volatile u8*)&fobj->flags & 0x20) {
            fobj->flags = *(volatile u8*)&fobj->flags & 0xFFFFFFDF;
            if (*(volatile u16*)&fobj->fterm != 0) {
                fobj->d0 = (fobj->p1 - fobj->p0) /
                           *(volatile u16*)&fobj->fterm;
            } else {
                fobj->d0 = lbl_8047DA3C;
                fobj->p0 = fobj->p1;
            }
        }
        data.fv = fobj->d0 * fobj->time + fobj->p0;
        break;
    case HSD_A_OP_SPL0:
    case HSD_A_OP_SPL:
    case HSD_A_OP_SLP:
        if (*(volatile u16*)&fobj->fterm != 0) {
            data.fv = fn_801B2560(lbl_8047DA48 /
                                      *(volatile u16*)&fobj->fterm,
                                  fobj->time,
                                  fobj->p0, fobj->p1, fobj->d0, fobj->d1);
        } else {
            data.fv = fobj->p1;
        }
        break;
    default:
        break;
    }
    update(obj, fobj->obj_type, &data);
}

/* Positional port of Melee baselib/fobj.c.  FObjUpdateAnim remains inline in
 * this routine, while the substantially larger FObjLoadData stays a call. */
void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                           HSD_ObjUpdateFunc update, f32 rate)
{
    f32 previousTerm = lbl_8047DA3C;
    u32 state = fobj != NULL ? FObjGetStateInline(fobj) : 0;

    if (state != 0 && !(fobj->time += rate, fobj->time < lbl_8047DA40)) {
        for (;;) {
            switch (state) {
            case 6: {
                fobj->time += previousTerm;
                if (fobj->flags & 0x40) {
                    fobj->op_intrp = fobj->op;
                    fobj->flags = fobj->flags & 0xFFFFFFBF;
                    fobj->flags |= 0x80;
                    fobj->p0 = fobj->p1;
                }
                FObjUpdateAnimInline(fobj, obj, update);
                return;
            }

            case FOBJ_LOAD_DATA0:
            case FOBJ_LOAD_DATA:
                state = FObjLoadData(fobj);
                break;

            case FOBJ_LOAD_WAIT: {
                if (fobj->flags & 0x80) {
                    FObjUpdateAnimInline(fobj, obj, update);
                }
                state = FObjGetStateInline(fobj);
                HSD_ASSERTMSG(0x16C, state == FOBJ_LOAD_WAIT, lbl_8027477C);
                if ((u32)(fobj->ad - fobj->ad_head) >= fobj->length) {
                    state = 6;
                } else {
                    u8 byte;
                    s32 wait = 0;
                    s32 shift = 0;
                    do {
                        byte = *fobj->ad++;
                        wait |= (byte & 0x7F) << shift;
                        shift += 7;
                    } while (byte & 0x80);
                    fobj->fterm = wait;
                    fobj->flags |= 0x20;
                    state = FObjSetStateInline(fobj, FOBJ_LOAD_DATA);
                }
                break;
            }

            case 4:
                if (fobj->fterm <= fobj->time) {
                    u8 unused[8] = { 0 };
                    state = FOBJ_LOAD_WAIT;
                    previousTerm = fobj->fterm;
                    fobj->time -= fobj->fterm;
                    FObjSetStateInline(fobj, state);
                    break;
                }
                FObjUpdateAnimInline(fobj, obj, update);
                state = 5;
                FObjSetStateInline(fobj, state);
                return;

            case 5:
                state = 4;
                FObjSetStateInline(fobj, state);
                break;

            case 0:
                return;
            }
        }
    }
}

/* 0x8019B490 | 0x98 */
#pragma push
#pragma optimization_level 4
extern void HSD_FObjInterpretAnim(HSD_FObj* fobj, void* obj,
                                  HSD_ObjUpdateFunc obj_update, f32 rate);
void HSD_FObjStopAnimAll(HSD_FObj* fobj, void* obj,
                         HSD_ObjUpdateFunc obj_update, f32 rate) {
    while (fobj != NULL) {
        if (fobj != NULL) {
            if (fobj->op_intrp == 6) {
                HSD_FObjInterpretAnim(fobj, obj, obj_update, rate);
            }
            if (fobj != NULL) {
                fobj->flags &= 0xF0;
            }
        }
        fobj = fobj->next;
    }
}
#pragma pop

/* ===================================================================
 * Generated: 0 pattern-matched + 5 stubs
 * Range: 0x8019B528 - 0x8019B7C0
 * =================================================================== */

/* 0x8019B528 | 0xC0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern const f64 lbl_8047DA58;
extern const f32 lbl_8047DA3C;
#if 0
asm void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 startframe) {
#include "src/hsd/hsd_dobj_HSD_FObjReqAnimAll.inc"
}
#else
#pragma optimization_level 1
static inline u32 HSD_FObjSetState(HSD_FObj* fobj, u32 state)
{
    if (fobj != NULL) {
        fobj->flags = (state & 0xF) | (fobj->flags & 0xF0);
    }
    return state;
}

static inline void HSD_FObjReqAnim(HSD_FObj* fobj, f32 startframe)
{
    if (fobj == NULL) {
        return;
    }
    fobj->ad = fobj->ad_head;
    fobj->time = (f32) fobj->startframe + startframe;
    fobj->op = 0;
    fobj->op_intrp = 0;
    fobj->flags &= ~0x40;
    fobj->nb_pack = 0;
    fobj->fterm = 0;
    fobj->p0 = 0.0f;
    fobj->p1 = 0.0f;
    fobj->d0 = 0.0f;
    fobj->d1 = 0.0f;
    HSD_FObjSetState(fobj, 1);
}

void HSD_FObjReqAnimAll(HSD_FObj* fobj, f32 startframe)
{
    HSD_FObj* fp;

    if (fobj == NULL) {
        return;
    }
    for (fp = fobj; fp != NULL; fp = fp->next) {
        HSD_FObjReqAnim(fp, startframe);
    }
}
#endif
#pragma pop

/* 0x8019B5E8 | 0x168 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_FObjFree(HSD_FObj* data);
extern void HSD_FObjRemove(HSD_FObj* data);
extern u8 lbl_80465378[];
extern u8 lbl_802747AC[];
#if 0
asm void HSD_FObjRemoveAll(void) {
#include "src/hsd/hsd_dobj_HSD_FObjRemoveAll.inc"
}
#else
#pragma optimization_level 4
/* The original source is the plain recursion
 *     if (fobj == NULL) return;
 *     HSD_FObjRemoveAll(fobj->next);
 *     HSD_FObjRemove(fobj);
 * which MWCC self-inlines 8 levels deep.  We cannot reproduce that here
 * because HSD_FObjRemove is defined *after* this function (matching the
 * original link order), so the inliner can no longer see its body.  The
 * unrolled form below is the compiler's own expansion, written out. */
#pragma dont_inline on
void HSD_FObjRemoveAll(HSD_FObj* fobj)
{
    HSD_FObj* f0;
    HSD_FObj* f1;
    HSD_FObj* f2;
    HSD_FObj* f3;
    HSD_FObj* f4;
    HSD_FObj* f5;
    HSD_FObj* f6;
    HSD_FObj* f7;
    HSD_FObj* f8;

    f0 = fobj;
    if (f0 == NULL) {
        return;
    }
    f1 = f0->next;
    if (f1 == NULL) {
        goto free0;
    }
    f2 = f1->next;
    if (f2 == NULL) {
        goto free1;
    }
    f3 = f2->next;
    if (f3 == NULL) {
        goto free2;
    }
    f4 = f3->next;
    if (f4 == NULL) {
        goto free3;
    }
    f5 = f4->next;
    if (f5 == NULL) {
        goto free4;
    }
    f6 = f5->next;
    if (f6 == NULL) {
        goto free5;
    }
    f7 = f6->next;
    if (f7 == NULL) {
        goto free6;
    }
    f8 = f7->next;
    if (f8 == NULL) {
        goto free7;
    }
    HSD_FObjRemoveAll(f8->next);
    HSD_FObjRemove(f8);
free7:
    if (f7 != NULL) {
        HSD_FObjFree(f7);
    }
free6:
    if (f6 != NULL) {
        HSD_ObjFree(lbl_80465378, f6);
        fn_80199A84(lbl_80465378, lbl_802747AC);
    }
free5:
    if (f5 != NULL) {
        HSD_ObjFree(lbl_80465378, f5);
    }
free4:
    if (f4 != NULL) {
        HSD_ObjFree(lbl_80465378, f4);
    }
free3:
    if (f3 != NULL) {
        HSD_ObjFree(lbl_80465378, f3);
    }
free2:
    if (f2 != NULL) {
        HSD_ObjFree(lbl_80465378, f2);
    }
free1:
    if (f1 != NULL) {
        HSD_ObjFree(lbl_80465378, f1);
    }
free0:
    if (f0 != NULL) {
        HSD_ObjFree(lbl_80465378, f0);
    }
}
#pragma dont_inline off
#endif
#pragma pop

/* 0x8019B750 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_FObjRemove(void) {
#include "src/hsd/hsd_dobj_HSD_FObjRemove.inc"
}
#else
#pragma optimization_level 4
void HSD_FObjRemove(HSD_FObj* data) {
    if (data != NULL) {
        HSD_ObjFree(lbl_80465378, data);
    }
}
#endif
#pragma pop

/* 0x8019B784 | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);
#if 0
asm void HSD_FObjInitAllocData(void) {
#include "src/hsd/hsd_dobj_HSD_FObjInitAllocData.inc"
}
#else
void HSD_FObjInitAllocData(void) {
    HSD_ObjAllocInit(lbl_80465378, 0x30, 4);
}
#endif
#pragma pop

/* 0x8019B7B4 | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_FObjGetAllocData(void) {
#include "src/hsd/hsd_dobj_HSD_FObjGetAllocData.inc"
}
#else
void* HSD_FObjGetAllocData(void) {
    return lbl_80465378;
}
#endif
#pragma pop
