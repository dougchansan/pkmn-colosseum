/**
 * @file hsd_lobj.c
 * @brief HSD LObj - Light object implementation.
 *
 * Colosseum address: 0x801A4000 (HSD_LObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/lobj.c
 */

#include "hsd/hsd_lobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_wobj.h"

static void LObjInfoInit(void);
extern s32 fn_801A4440(HSD_LObj* lobj, HSD_LightDesc* ldesc);
extern void LObjUpdateFunc(HSD_LObj* lobj, u32 type, void* value);

HSD_LObjInfo hsdLObj = { LObjInfoInit };

static HSD_LObjInfo* default_class = NULL;
static s32 nb_active = 0;

typedef struct LObjVec {
    f32 x;
    f32 y;
    f32 z;
} LObjVec;

#define LOBJ_ACTIVE_SLOT(idx) (*(HSD_LObj**)(lbl_804655E0 + ((idx) * 4)))
#define LOBJ_LIGHTOBJ(lobj) ((void*)((u8*)(lobj) + 0x50))
#define LOBJ_SPEC_LIGHTOBJ(lobj) ((void*)((u8*)(lobj) + 0x94))
#define LOBJ_ABS(x) ((x) < 0.0f ? -(x) : (x))

/* ========================================================================= */
/*  Flag accessors                                                           */
/* ========================================================================= */

u32 HSD_LObjGetFlags(HSD_LObj* lobj)
{
    HSD_ASSERT(0, lobj);
    return lobj->flags;
}

void HSD_LObjSetFlags_Early(HSD_LObj* lobj, u32 flags)
{
    HSD_ASSERT(0, lobj);
    lobj->flags |= (u16) flags;
}

void HSD_LObjClearFlags_Early(HSD_LObj* lobj, u32 flags)
{
    HSD_ASSERT(0, lobj);
    lobj->flags &= (u16) ~flags;
}

/* ========================================================================= */
/*  Active lights                                                            */
/* ========================================================================= */

void HSD_LObjSetActive(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        nb_active++;
    }
}

s32 HSD_LObjGetNbActive_Early(void)
{
    return nb_active;
}

void HSD_LObjClearActive(void)
{
    nb_active = 0;
}

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_LObjAddAnim(HSD_LObj* lobj, HSD_LightAnim* lanim)
{
    if (lobj == NULL || lanim == NULL) {
        return;
    }
    if (lobj->aobj != NULL) {
        HSD_AObjRemove(lobj->aobj);
    }
    lobj->aobj = HSD_AObjLoadDesc(lanim->aobjdesc);
    HSD_WObjAddAnim(lobj->position, lanim->position_anim);
    HSD_WObjAddAnim(lobj->interest, lanim->interest_anim);
}

void HSD_LObjAddAnimAll_Early(HSD_LObj* lobj, HSD_LightAnim* lanim)
{
    HSD_LObj* l;
    HSD_LightAnim* a;

    l = lobj;
    a = lanim;
    while (l != NULL) {
        HSD_LObjAddAnim(l, a);
        l = l->next;
        if (a != NULL) a = a->next;
    }
}

void HSD_LObjAnim(HSD_LObj* lobj)
{
    if (lobj != NULL) {
        HSD_WObjInterpretAnim(lobj->position);
        HSD_WObjInterpretAnim(lobj->interest);
    }
}

void HSD_LObjAnimAll_Early(HSD_LObj* lobj)
{
    HSD_LObj* l;
    for (l = lobj; l != NULL; l = l->next) {
        HSD_LObjAnim(l);
    }
}

void HSD_LObjReqAnim(HSD_LObj* lobj, f32 startframe)
{
    if (lobj != NULL) {
        HSD_AObjReqAnim(lobj->aobj, startframe);
        HSD_WObjReqAnim(lobj->position, startframe);
        HSD_WObjReqAnim(lobj->interest, startframe);
    }
}

void HSD_LObjReqAnimAll_Early(HSD_LObj* lobj, f32 startframe)
{
    HSD_LObj* l;
    for (l = lobj; l != NULL; l = l->next) {
        HSD_LObjReqAnim(l, startframe);
    }
}

/* ========================================================================= */
/*  Color / position                                                         */
/* ========================================================================= */

void HSD_LObjSetColor_Early(HSD_LObj* lobj, u32 color)
{
    HSD_ASSERT(0, lobj);
    lobj->color = color;
}

void HSD_LObjSetPosition_Early(HSD_LObj* lobj, f32 x, f32 y, f32 z)
{
    HSD_ASSERT(0, lobj);
    HSD_WObjSetPosition_Early(lobj->position, x, y, z);
}

void HSD_LObjSetInterest_Early(HSD_LObj* lobj, f32 x, f32 y, f32 z)
{
    HSD_ASSERT(0, lobj);
    HSD_WObjSetPosition_Early(lobj->interest, x, y, z);
}

/* ========================================================================= */
/*  Remove / Alloc                                                           */
/* ========================================================================= */

void HSD_LObjRemoveAll(HSD_LObj* lobj)
{
    HSD_LObj* next;
    while (lobj != NULL) {
        next = lobj->next;
        HSD_OBJECT_METHOD(lobj)->release((HSD_Class*) lobj);
        HSD_OBJECT_METHOD(lobj)->destroy((HSD_Class*) lobj);
        lobj = next;
    }
}

HSD_LObj* HSD_LObjAlloc(void)
{
    HSD_LObj* lobj;
    lobj = (HSD_LObj*) hsdNew(
        default_class ? (HSD_ClassInfo*) default_class
                      : &hsdLObj.parent.parent);
    HSD_ASSERT(0, lobj);
    return lobj;
}

/* ========================================================================= */
/*  Class lifecycle                                                          */
/* ========================================================================= */

static void LObjRelease(HSD_Class* o)
{
    HSD_LObj* lobj = (HSD_LObj*) o;
    HSD_WObjUnref(lobj->position);
    HSD_WObjUnref(lobj->interest);
    HSD_AObjRemove(lobj->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdLObj)->release(o);
}

static void LObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    nb_active = 0;
    HSD_OBJECT_PARENT_INFO(&hsdLObj)->amnesia(info);
}

static void LObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdLObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_lobj",
                     sizeof(HSD_LObjInfo), sizeof(HSD_LObj));
    HSD_CLASS_INFO(&hsdLObj)->release = LObjRelease;
    HSD_CLASS_INFO(&hsdLObj)->amnesia = LObjAmnesia;
    HSD_LOBJ_INFO(&hsdLObj)->load =
        (int (*)(HSD_LObj*, HSD_LightDesc*)) fn_801A4440;
    HSD_LOBJ_INFO(&hsdLObj)->update =
        (void (*)(HSD_LObj*, u32, void*)) LObjUpdateFunc;
}

/* 0x801A4098 | 0x60 */
extern HSD_ClassInfo* lbl_8047B2B0;
extern u8 lbl_8036CA20[];
extern HSD_SList* lbl_8047B2B4;
#if 0
asm void fn_801A4098(void) {
#include "src/hsd/hsd_lobj_fn_801A4098.inc"
}
#else
#pragma push
#pragma peephole off
extern u8 lbl_8036CA20[];
extern void fn_801C25E4(HSD_AObj*);
void fn_801A4098(void* param) {
    if (param == lbl_8047B2B0) {
        lbl_8047B2B0 = NULL;
    }
    if (param == (void*)lbl_8036CA20) {
        lbl_8047B2B4 = NULL;
    }
    {
        void* ptr = (void*)lbl_8036CA20;
        void* vtable = *(void**)((u8*)ptr + 0x14);
        void (*func)(void*) = *(void(**)(void*))((u8*)vtable + 0x38);
        func(param);
    }
}
#pragma pop
#endif

/* 0x801A40F8 | 0x174 */
#if 0
asm void fn_801A40F8(void) {
#include "src/hsd/hsd_lobj_fn_801A40F8.inc"
}
#else
void fn_801A40F8(HSD_LObj* lobj)
{
    fn_801C25E4(lobj->aobj);
    HSD_WObjUnref(lobj != NULL ? lobj->position : NULL);
    HSD_WObjUnref(lobj != NULL ? lobj->interest : NULL);
    HSD_OBJECT_PARENT_INFO(&hsdLObj)->release((HSD_Class*) lobj);
}
#endif

/* 0x801A426C | 0xD8 */
extern HSD_AObj* fn_801C2670(HSD_AObjDesc*);
#if 0
asm void HSD_LObjAddAnimAll(void) {
#include "src/hsd/hsd_lobj_HSD_LObjAddAnimAll.inc"
}
#else
void HSD_LObjAddAnimAll(HSD_LObj* lobj, HSD_LightAnim* lanim)
{
    HSD_LObj* l;
    HSD_LightAnim* a;

    if (lobj == NULL) {
        return;
    }
    l = lobj;
    a = lanim;
    while (l != NULL) {
        if (l != NULL && a != NULL) {
            if (*(volatile u32*)((u8*)l + 0x48) != 0) {
                fn_801C25E4(l->aobj);
            }
            l->aobj = fn_801C2670(a->aobjdesc);
            HSD_WObjAddAnim(l != NULL ? l->position : NULL,
                            a->position_anim);
            HSD_WObjAddAnim(l != NULL ? l->interest : NULL,
                            a->interest_anim);
        }
        l = l != NULL ? l->next : NULL;
        a = a != NULL ? a->next : NULL;
    }
}
#endif

/* 0x801A4344 | 0xFC */
extern HSD_ClassInfo* fn_80193748(const char* class_name);
extern void* fn_80193828(HSD_ClassInfo* info);
extern void __assert(const char* file, u32 line, const char* expr);
extern char lbl_8047DBB8;
extern char lbl_8047DBC0;
extern char lbl_8047DBC4;
#if 0
asm void fn_801A4344(void) {
#include "src/hsd/hsd_lobj_fn_801A4344.inc"
}
#else
HSD_LObj* fn_801A4344(HSD_LightDesc* ldesc)
{
    HSD_LObj* first;
    HSD_LObj** nextp;

    nextp = &first;
    while (ldesc != NULL) {
        HSD_ClassInfo* info;
        HSD_LObj* lobj;

        if (ldesc->class_name == NULL ||
            (info = fn_80193748(ldesc->class_name)) == NULL) {
            if (lbl_8047B2B0 != NULL) {
                info = lbl_8047B2B0;
            } else {
                info = (HSD_ClassInfo*) lbl_8036CA20;
            }
            lobj = (HSD_LObj*) fn_80193828(info);
            if (lobj == NULL) {
                __assert(&lbl_8047DBB8, 0x5D5, &lbl_8047DBC0);
            }
        } else {
            lobj = (HSD_LObj*) fn_80193828(info);
            if (lobj == NULL) {
                __assert(&lbl_8047DBB8, 0x67B, &lbl_8047DBC4);
            }
        }

        *nextp = lobj;
        HSD_LOBJ_METHOD(lobj)->load(lobj, ldesc);
        nextp = &lobj->next;
        ldesc = ldesc->next;
    }
    *nextp = NULL;
    return first;
}
#endif

/* 0x801A4440 | 0x470 */
extern HSD_WObj* HSD_WObjLoadDesc(HSD_WObjDesc* desc);
extern void HSD_Panic(const char* file, s32 line, const char* msg);
extern void OSReport(const char* fmt, ...);
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GXInitLightSpot(void* light, f32 cutoff, u32 spot_func);
extern void GXInitLightDistAttn(void* light, f32 ref_dist, f32 ref_br,
                                u32 dist_func);
extern u8 lbl_80274D94[];
extern char lbl_8047DBC8;
#if 0
asm void fn_801A4440(void) {
#include "src/hsd/hsd_lobj_fn_801A4440.inc"
}
#else
s32 fn_801A4440(HSD_LObj* lobj, HSD_LightDesc* ldesc)
{
    HSD_WObj* wobj;

    lobj->color = ldesc->color;
    if (lobj != NULL) {
        lobj->flags |= ldesc->flags;
    }

    switch (ldesc->flags & LOBJ_TYPE_MASK) {
    case LOBJ_AMBIENT:
        break;
    case LOBJ_INFINITE:
        wobj = HSD_WObjLoadDesc(ldesc->position);
        if (lobj != NULL) {
            HSD_WObjUnref(lobj->position);
            lobj->position = wobj;
        }
        break;
    case LOBJ_POINT:
        wobj = HSD_WObjLoadDesc(ldesc->position);
        if (lobj != NULL) {
            HSD_WObjUnref(lobj->position);
            lobj->position = wobj;
        }
        if (ldesc->attnflags & LOBJ_LIGHT_ATTN) {
            if (lobj != NULL) {
                lobj->flags |= LOBJ_RAW_PARAM;
                lobj->u.attn.k0 = ldesc->u.attn->k0;
                lobj->u.attn.k1 = ldesc->u.attn->k1;
                lobj->u.attn.k2 = ldesc->u.attn->k2;
            }
        } else if (lobj != NULL) {
            lobj->u.point.ref_dist = ldesc->u.point->ref_dist;
            lobj->u.point.ref_br = ldesc->u.point->ref_br;
            lobj->u.point.dist_func = ldesc->u.point->dist_func;
        }
        break;
    case LOBJ_SPOT:
        wobj = HSD_WObjLoadDesc(ldesc->position);
        if (lobj != NULL) {
            HSD_WObjUnref(lobj->position);
            lobj->position = wobj;
        }
        wobj = HSD_WObjLoadDesc(ldesc->interest);
        if (lobj != NULL) {
            HSD_WObjUnref(lobj->interest);
            lobj->interest = wobj;
        }
        if (ldesc->attnflags & LOBJ_LIGHT_ATTN) {
            if (lobj != NULL) {
                lobj->flags |= LOBJ_RAW_PARAM;
                lobj->u.attn.a0 = ldesc->u.attn->a0;
                lobj->u.attn.a1 = ldesc->u.attn->a1;
                lobj->u.attn.a2 = ldesc->u.attn->a2;
                lobj->u.attn.k0 = ldesc->u.attn->k0;
                lobj->u.attn.k1 = ldesc->u.attn->k1;
                lobj->u.attn.k2 = ldesc->u.attn->k2;
            }
        } else if (lobj != NULL) {
            lobj->u.spot.ref_dist = ldesc->u.spot->ref_dist;
            lobj->u.spot.ref_br = ldesc->u.spot->ref_br;
            lobj->u.spot.dist_func = ldesc->u.spot->dist_func;
            lobj->u.spot.cutoff = ldesc->u.spot->cutoff;
            lobj->u.spot.spot_func = ldesc->u.spot->spot_func;
        }
        break;
    default:
        OSReport((const char*) lbl_80274D94, ldesc->flags);
        HSD_Panic(&lbl_8047DBB8, 0x659, &lbl_8047DBC8);
        break;
    }
    return 0;
}
#endif

/* 0x801A48B0 | 0x44 */
extern void fn_80191688();
/* HSD_LObjGetInterest */
s32 HSD_LObjGetInterest(HSD_LObj* lobj, void* out) {
    if (lobj != NULL) {
        if (*(volatile u32*)((u8*)lobj + 0x1C) != 0) {
            fn_80191688(*(void**)((u8*)lobj + 0x1C), out);
            return 1;
        }
    }
    return 0;
}

/* 0x801A48F4 | 0x88 */
extern void HSD_WObjSetPosition(HSD_WObj* wobj, void* position);
extern char lbl_8047DBCC;
extern u8 lbl_80274DB8[];
extern char lbl_8047DBB8;
#if 0
asm void HSD_LObjSetInterest(void) {
#include "src/hsd/hsd_lobj_HSD_LObjSetInterest.inc"
}
#else
#pragma push
#pragma scheduling on
void HSD_LObjSetInterest(HSD_LObj* lobj, void* desc) {
    if (lobj == NULL) {
        __assert(&lbl_8047DBB8, 0x58c, &lbl_8047DBCC);
    }
    if (lobj->interest == NULL) {
        lobj->interest = HSD_WObjAlloc();
        if (lobj->interest == NULL) {
            __assert(&lbl_8047DBB8, 0x58f, (const char*) lbl_80274DB8);
        }
    }
    HSD_WObjSetPosition(lobj->interest, desc);
}
#pragma pop
#endif

/* 0x801A497C | 0x44 */
#if 0
asm void HSD_LObjGetPosition(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetPosition.inc"
}
#else
s32 HSD_LObjGetPosition(HSD_LObj* lobj, void* out) {
    if (lobj != NULL) {
        if (*(volatile u32*)((u8*)lobj + 0x18) != 0) {
            fn_80191688(*(void**)((u8*)lobj + 0x18), out);
            return 1;
        }
    }
    return 0;
}
#endif

/* 0x801A49C0 | 0x88 */
extern u8 lbl_80274DC8[];
#if 0
asm void HSD_LObjSetPosition(void) {
#include "src/hsd/hsd_lobj_HSD_LObjSetPosition.inc"
}
#else
void HSD_LObjSetPosition(HSD_LObj* lobj, void* desc) {
    if (lobj == NULL) {
        __assert(&lbl_8047DBB8, 0x568, &lbl_8047DBCC);
    }
    if (lobj->position == NULL) {
        lobj->position = HSD_WObjAlloc();
        if (lobj->position == NULL) {
            __assert(&lbl_8047DBB8, 0x56b, (const char*) lbl_80274DC8);
        }
    }
    HSD_WObjSetPosition(lobj->position, desc);
}
#endif

/* 0x801A4A48 | 0xC */
#if 0
asm void HSD_LObjSetColor(void) {
#include "src/hsd/hsd_lobj_HSD_LObjSetColor.inc"
}
#else
void HSD_LObjSetColor(HSD_LObj* lobj, u32* color) {
    lobj->color = *color;
}
#endif

/* 0x801A4A54 | 0x70 */
extern void jumptable_8036CA64();
/* GXLightIndex to GXLightID bitmask */
#if 0
asm void fn_801A4A54(void) {
#include "src/hsd/hsd_lobj_fn_801A4A54.inc"
}
#else
u32 fn_801A4A54(u32 idx) {
    switch (idx) {
    case 0: return 0x1;
    case 1: return 0x2;
    case 2: return 0x4;
    case 3: return 0x8;
    case 4: return 0x10;
    case 5: return 0x20;
    case 6: return 0x40;
    case 7: return 0x80;
    case 8: return 0x100;
    }
    return 0;
}
#endif

/* 0x801A4AC4 | 0x3C */
#if 0
asm void HSD_LObjGetCurrentByType(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetCurrentByType.inc"
}
#else
#pragma push
#pragma peephole off
/* Find active light by type */
HSD_LObj* HSD_LObjGetCurrentByType(u32 type) {
    void* data;
    HSD_SList* p;
    type &= 3;
    for (p = lbl_8047B2B4; p != NULL; p = p->next) {
        data = ((volatile HSD_SList*) p)->data;
        if (type != (((HSD_LObj*) data)->flags & 3)) {
            continue;
        }
        return (HSD_LObj*) ((volatile HSD_SList*) p)->data;
    }
    return NULL;
}
#pragma pop
#endif

/* 0x801A4B00 | 0x220 */
extern HSD_SList* fn_801A3E64(HSD_SList* node);
extern u8 lbl_804655E0[];
extern s32 lbl_8047B2B8;
#if 0
asm void HSD_LObjDeleteCurrentAll(HSD_LObj* lobj) {
#include "src/hsd/hsd_lobj_HSD_LObjDeleteCurrentAll.inc"
}
#else
void HSD_LObjDeleteCurrentAll(HSD_LObj* lobj)
{
    HSD_LObj* cur;
    HSD_LObj* data;
    HSD_SList** pp;
    s32 i;

    if (lobj != NULL) {
        for (cur = lobj; cur != NULL; cur = cur->next) {
            pp = &lbl_8047B2B4;
            while (*pp != NULL) {
                if ((*pp)->data == cur) {
                    for (i = 0; i < MAX_GXLIGHT; i++) {
                        if (LOBJ_ACTIVE_SLOT(i) == cur) {
                            LOBJ_ACTIVE_SLOT(i) = NULL;
                        }
                    }
                    *pp = fn_801A3E64(*pp);
                    if (cur != NULL && ref_DEC(cur)) {
                        hsdDelete(cur);
                    }
                    break;
                }
                pp = &(*pp)->next;
            }
        }
    } else {
        for (i = 0; i < MAX_GXLIGHT; i++) {
            LOBJ_ACTIVE_SLOT(i) = NULL;
        }
        lbl_8047B2B8 = 0;
        while (lbl_8047B2B4 != NULL) {
            data = (HSD_LObj*) lbl_8047B2B4->data;
            if (data != NULL && ref_DEC(data)) {
                hsdDelete(data);
            }
            lbl_8047B2B4 = fn_801A3E64(lbl_8047B2B4);
        }
    }
}
#endif

/* 0x801A4D20 | 0x234 */
extern HSD_SList* fn_801A3EB4(HSD_SList* next, void* data);
extern u8 lbl_80274DD8[];
extern u8 lbl_80274DE4[];
extern char lbl_8047DBD4;
extern char lbl_8047DBD8;
#if 0
asm void fn_801A4D20(void) {
#include "src/hsd/hsd_lobj_fn_801A4D20.inc"
}
#else
void fn_801A4D20(HSD_LObj* lobj)
{
    HSD_LObj* cur;
    HSD_LObj* listed;
    HSD_SList* node;
    HSD_SList** pp;
    s32 i;

    for (cur = lobj; cur != NULL; cur = cur->next) {
        for (node = lbl_8047B2B4; node != NULL; node = node->next) {
            if (node->data == cur) {
                pp = &lbl_8047B2B4;
                while (*pp != NULL) {
                    if ((*pp)->data == cur) {
                        for (i = 0; i < MAX_GXLIGHT; i++) {
                            if (LOBJ_ACTIVE_SLOT(i) == cur) {
                                LOBJ_ACTIVE_SLOT(i) = NULL;
                            }
                        }
                        *pp = fn_801A3E64(*pp);
                        if (cur != NULL && ref_DEC(cur)) {
                            hsdDelete(cur);
                        }
                        break;
                    }
                    pp = &(*pp)->next;
                }
                break;
            }
        }

        HSD_OBJ(cur)->ref_count++;
        if (HSD_OBJ(cur)->ref_count == HSD_OBJ_NOREF) {
            __assert((const char*) lbl_80274DD8, 0x5D,
                     (const char*) lbl_80274DE4);
        }

        pp = &lbl_8047B2B4;
        while (*pp != NULL) {
            if (cur == NULL) {
                __assert(&lbl_8047DBD8, 0x163, &lbl_8047DBD4);
            }
            listed = (HSD_LObj*) (*pp)->data;
            if (listed == NULL) {
                __assert(&lbl_8047DBD8, 0x163, &lbl_8047DBD4);
            }
            if ((u8) listed->priority > (u8) cur->priority) {
                break;
            }
            pp = &(*pp)->next;
        }
        *pp = fn_801A3EB4(*pp, cur);
    }
}
#endif

/* 0x801A4F54 | 0xE78 */
extern void fn_800A37CC(void* mtx, void* src, void* dst);
extern void fn_800BA414(void* light, f32 x, f32 y, f32 z);
extern void fn_800BA198(void* light, f32 a0, f32 a1, f32 a2,
                        f32 k0, f32 k1, f32 k2);
extern void fn_800BA440(void* light, u32* color);
extern void fn_800A3A9C(void* a, void* b, void* dst);
extern void fn_800A3ADC(void* src, void* dst);
extern void fn_800A3820(void* mtx, void* src, void* dst);
extern void fn_800BA424(void* light, f32 x, f32 y, f32 z);
extern void fn_800BA44C(void* light, u32 light_id);
extern void jumptable_8036CAD0();
extern void jumptable_8036CAAC();
extern void jumptable_8036CA88();
extern u32 lbl_8047B2BC;
extern u32 lbl_8047B2C0;
extern u32 lbl_8047B2C4;
extern u32 lbl_8047B2C8;
extern f32 lbl_8047DBE0;
extern f32 lbl_8047DBE4;
extern f32 lbl_8047DBE8;
extern u8 lbl_80274D58[];
extern u8 lbl_80274D64[];
extern f32 lbl_80478AC8;
extern f64 lbl_8047DBF0;
extern f32 lbl_8047DBEC;
extern f32 lbl_8047DBF8;
extern f32 lbl_8047DBFC;
#if 0
asm void HSD_LObjSetup(void* setup) {
#include "src/hsd/hsd_lobj_HSD_LObjSetup.inc"
}
#else
#define LOBJ_COPY_VEC(dst, src)                         \
    do {                                                \
        ((u32*) &(dst))[0] = *(u32*) &((src)[0]);       \
        ((u32*) &(dst))[1] = *(u32*) &((src)[4]);       \
        ((u32*) &(dst))[2] = *(u32*) &((src)[8]);       \
    } while (0)

#define LOBJ_DEFAULT_DIR(dst) \
    do {                      \
        (dst).x = lbl_8047DBE8; \
        (dst).y = lbl_8047DBE8; \
        (dst).z = lbl_8047DBE4; \
    } while (0)

#define LOBJ_NORMALIZE_OR_DEFAULT(vec)                                      \
    do {                                                                    \
        if (LOBJ_ABS((vec).x) <= lbl_80478AC8 &&                            \
            LOBJ_ABS((vec).y) <= lbl_80478AC8 &&                            \
            LOBJ_ABS((vec).z) <= lbl_80478AC8) {                            \
            LOBJ_DEFAULT_DIR(vec);                                          \
        } else {                                                            \
            fn_800A3ADC(&(vec), &(vec));                                    \
        }                                                                   \
    } while (0)

#define LOBJ_LOAD_LIGHT_VECTOR(lobj, dst)                                   \
    do {                                                                    \
        LObjVec lobj_pos;                                                   \
        LObjVec lobj_interest;                                              \
                                                                            \
        LOBJ_COPY_VEC(lobj_pos, lbl_80274D58);                              \
        LOBJ_COPY_VEC(lobj_interest, lbl_80274D64);                         \
        if ((lobj)->position != NULL) {                                     \
            fn_80191688((lobj)->position, &lobj_pos);                       \
        }                                                                   \
        if ((lobj)->interest != NULL) {                                     \
            fn_80191688((lobj)->interest, &lobj_interest);                  \
        }                                                                   \
        fn_800A3A9C(&lobj_interest, &lobj_pos, &(dst));                     \
        LOBJ_NORMALIZE_OR_DEFAULT(dst);                                     \
    } while (0)

#define LOBJ_SETUP_SPECULAR(lobj, mtx, light_id)                            \
    do {                                                                    \
        u32 lobj_spec_id;                                                   \
        u32 lobj_color;                                                     \
        f32 lobj_half;                                                      \
                                                                            \
        lobj_spec_id = (light_id);                                          \
        (lobj)->spec_id = lobj_spec_id;                                     \
        if (lobj_spec_id != 0) {                                            \
            lobj_color = (lobj)->color;                                     \
            fn_800BA440(LOBJ_SPEC_LIGHTOBJ(lobj), &lobj_color);             \
            (lobj)->shininess = lbl_8047DBF8;                               \
            lobj_half = (lobj)->shininess * lbl_8047DBFC;                   \
            fn_800BA198(LOBJ_SPEC_LIGHTOBJ(lobj), lbl_8047DBE8,             \
                        lbl_8047DBE8, lbl_8047DBE4, lobj_half,              \
                        lbl_8047DBE8, lbl_8047DBE4 - lobj_half);            \
            switch ((lobj)->flags & LOBJ_TYPE_MASK) {                      \
            case LOBJ_POINT:                                                \
            case LOBJ_SPOT:                                                 \
                if ((lobj)->position != NULL) {                             \
                    fn_80191688((lobj)->position, &(lobj)->lvec_x);         \
                }                                                           \
                fn_800A37CC((mtx), &(lobj)->lvec_x, &(lobj)->lvec_x);       \
                break;                                                      \
            case LOBJ_INFINITE: {                                           \
                LObjVec lobj_dir;                                           \
                                                                            \
                LOBJ_LOAD_LIGHT_VECTOR(lobj, lobj_dir);                     \
                (lobj)->lvec_x = lobj_dir.x;                                \
                (lobj)->lvec_y = lobj_dir.y;                                \
                (lobj)->lvec_z = lobj_dir.z;                                \
                fn_800A3820((mtx), &(lobj)->lvec_x, &(lobj)->lvec_x);       \
                fn_800A3ADC(&(lobj)->lvec_x, &(lobj)->lvec_x);              \
                break;                                                      \
            }                                                               \
            default:                                                        \
                __assert(&lbl_8047DBB8, 0x2C0, &lbl_8047DBD4);              \
                break;                                                      \
            }                                                               \
            (lobj)->flags |= LOBJ_SPEC_DIRTY;                               \
            lbl_8047B2C0 |= lobj_spec_id;                                   \
        }                                                                   \
    } while (0)

#define LOBJ_RUNTIME_SETUP(lobj, color_ptr, shininess_val)                  \
    do {                                                                    \
        u32* lobj_color_ptr;                                                \
        f32 lobj_shininess;                                                 \
        f32 lobj_half;                                                      \
        u32 lobj_flags;                                                     \
                                                                            \
        lobj_color_ptr = (color_ptr);                                       \
        lobj_shininess = (shininess_val);                                   \
        if ((lobj) != NULL) {                                               \
            lobj_flags = (lobj)->flags;                                     \
            if ((lobj_flags & LOBJ_HIDDEN) == 0 &&                          \
                (lobj_flags & LOBJ_TYPE_MASK) != LOBJ_AMBIENT) {           \
                if (lobj_flags & (LOBJ_DIFFUSE | LOBJ_ALPHA)) {            \
                    if (((u8*) &(lobj)->hw_color)[0] !=                     \
                            ((u8*) lobj_color_ptr)[0] ||                    \
                        ((u8*) &(lobj)->hw_color)[1] !=                     \
                            ((u8*) lobj_color_ptr)[1] ||                    \
                        ((u8*) &(lobj)->hw_color)[2] !=                     \
                            ((u8*) lobj_color_ptr)[2] ||                    \
                        ((u8*) &(lobj)->hw_color)[3] !=                     \
                            ((u8*) lobj_color_ptr)[3]) {                   \
                        fn_800BA440(LOBJ_LIGHTOBJ(lobj), lobj_color_ptr);   \
                        (lobj)->hw_color = *lobj_color_ptr;                 \
                        (lobj)->flags |= LOBJ_DIFF_DIRTY;                   \
                    }                                                       \
                    if ((lobj)->flags & LOBJ_DIFF_DIRTY) {                  \
                        fn_800BA44C(LOBJ_LIGHTOBJ(lobj), (lobj)->id);       \
                        (lobj)->flags &= (u16) ~LOBJ_DIFF_DIRTY;            \
                    }                                                       \
                }                                                           \
                if ((lobj)->spec_id != 0) {                                 \
                    if ((lobj)->shininess != lobj_shininess) {              \
                        (lobj)->shininess = lobj_shininess;                 \
                        lobj_half = lobj_shininess * lbl_8047DBFC;          \
                        fn_800BA198(LOBJ_SPEC_LIGHTOBJ(lobj),               \
                                    lbl_8047DBE8, lbl_8047DBE8,             \
                                    lbl_8047DBE4, lobj_half,                \
                                    lbl_8047DBE8,                           \
                                    lbl_8047DBE4 - lobj_half);              \
                        (lobj)->flags |= LOBJ_SPEC_DIRTY;                   \
                    }                                                       \
                    if ((lobj)->flags & LOBJ_SPEC_DIRTY) {                  \
                        fn_800BA44C(LOBJ_SPEC_LIGHTOBJ(lobj),               \
                                    (lobj)->spec_id);                       \
                        (lobj)->flags &= (u16) ~LOBJ_SPEC_DIRTY;            \
                    }                                                       \
                }                                                           \
            }                                                               \
        }                                                                   \
    } while (0)

void HSD_LObjSetup(void* setup)
{
    void* view_mtx;
    HSD_SList* node;
    HSD_LObj* lobj;
    LObjVec pos;
    LObjVec dir;
    u32 flags;
    u32 ref_flags;
    u32 type;
    u32 color;
    f32 ref_dist;
    s32 slot;
    s32 light_slot;
    s32 active_limit;
    s32 i;

    lbl_8047B2BC = 0;
    lbl_8047B2C0 = 0;
    lbl_8047B2C4 = 0;
    lbl_8047B2C8 = 0;
    view_mtx = (void*) ((u8*) setup + 0x54);

    for (i = 0; i < MAX_GXLIGHT; i++) {
        LOBJ_ACTIVE_SLOT(i) = NULL;
    }

    lbl_8047B2B8 = 0;
    node = lbl_8047B2B4;
    light_slot = 0;
    while (light_slot < 8 && node != NULL) {
        lobj = (HSD_LObj*) node->data;
        if (lobj != NULL && (lobj->flags & LOBJ_HIDDEN) == 0) {
            flags = lobj->flags;
            ref_flags = flags & (LOBJ_DIFFUSE | LOBJ_SPECULAR | LOBJ_ALPHA);
            type = flags & LOBJ_TYPE_MASK;
            if (ref_flags != 0) {
                if (type == LOBJ_AMBIENT) {
                    slot = 8;
                    if (LOBJ_ACTIVE_SLOT(slot) != NULL) {
                        node = node->next;
                        continue;
                    }
                } else {
                    slot = lbl_8047B2B8;
                    lbl_8047B2B8 = slot + 1;
                }

                LOBJ_ACTIVE_SLOT(slot) = lobj;
                lobj->id = fn_801A4A54(slot);
                light_slot = lbl_8047B2B8;
                lobj->spec_id = 0;

                switch (type) {
                case LOBJ_AMBIENT:
                    node = node->next;
                    continue;
                case LOBJ_INFINITE:
                    if (lobj->position != NULL) {
                        fn_80191688(lobj->position, &pos);
                    }
                    pos.x *= lbl_8047DBE0;
                    pos.y *= lbl_8047DBE0;
                    pos.z *= lbl_8047DBE0;
                    fn_800A37CC(view_mtx, &pos, &pos);
                    if (lobj->flags & LOBJ_DIFFUSE) {
                        fn_800BA414(LOBJ_LIGHTOBJ(lobj), pos.x, pos.y, pos.z);
                        fn_800BA198(LOBJ_LIGHTOBJ(lobj), lbl_8047DBE4,
                                    lbl_8047DBE8, lbl_8047DBE8,
                                    lbl_8047DBE4, lbl_8047DBE8,
                                    lbl_8047DBE8);
                    }
                    if (lobj->flags & LOBJ_SPECULAR) {
                        fn_800BA414(LOBJ_SPEC_LIGHTOBJ(lobj), pos.x, pos.y,
                                    pos.z);
                    }
                    break;
                case LOBJ_POINT:
                    color = lobj->color;
                    fn_800BA440(LOBJ_LIGHTOBJ(lobj), &color);
                    lobj->hw_color = color;
                    if (lobj->position != NULL) {
                        fn_80191688(lobj->position, &pos);
                    }
                    fn_800A37CC(view_mtx, &pos, &pos);
                    fn_800BA414(LOBJ_LIGHTOBJ(lobj), pos.x, pos.y, pos.z);
                    fn_800BA414(LOBJ_SPEC_LIGHTOBJ(lobj), pos.x, pos.y,
                                pos.z);
                    if (lobj->flags & LOBJ_RAW_PARAM) {
                        fn_800BA198(LOBJ_LIGHTOBJ(lobj), lbl_8047DBE4,
                                    lbl_8047DBE8, lbl_8047DBE8,
                                    lobj->u.attn.k0, lobj->u.attn.k1,
                                    lobj->u.attn.k2);
                    } else {
                        GXInitLightDistAttn(LOBJ_LIGHTOBJ(lobj),
                                            lobj->u.point.ref_dist,
                                            lobj->u.point.ref_br,
                                            lobj->u.point.dist_func);
                        GXInitLightSpot(LOBJ_LIGHTOBJ(lobj), lbl_8047DBE8, 0);
                        GXInitLightDistAttn(LOBJ_SPEC_LIGHTOBJ(lobj),
                                            lobj->u.point.ref_dist,
                                            lobj->u.point.ref_br,
                                            lobj->u.point.dist_func);
                    }
                    break;
                case LOBJ_SPOT:
                    LOBJ_COPY_VEC(pos, lbl_80274D58);
                    if (lobj->position != NULL) {
                        fn_80191688(lobj->position, &pos);
                    }
                    fn_800A37CC(view_mtx, &pos, &pos);
                    LOBJ_LOAD_LIGHT_VECTOR(lobj, dir);
                    fn_800A3820(view_mtx, &dir, &dir);
                    fn_800A3ADC(&dir, &dir);
                    fn_800BA414(LOBJ_LIGHTOBJ(lobj), pos.x, pos.y, pos.z);
                    fn_800BA414(LOBJ_SPEC_LIGHTOBJ(lobj), pos.x, pos.y,
                                pos.z);
                    fn_800BA424(LOBJ_LIGHTOBJ(lobj), dir.x, dir.y, dir.z);
                    if (lobj->flags & LOBJ_RAW_PARAM) {
                        fn_800BA198(LOBJ_LIGHTOBJ(lobj), lobj->u.attn.a0,
                                    lobj->u.attn.a1, lobj->u.attn.a2,
                                    lobj->u.attn.k0, lobj->u.attn.k1,
                                    lobj->u.attn.k2);
                    } else {
                        ref_dist = lobj->u.spot.ref_dist;
                        if (ref_dist < lbl_8047DBF0) {
                            ref_dist = lbl_8047DBEC;
                        }
                        GXInitLightDistAttn(LOBJ_LIGHTOBJ(lobj), ref_dist,
                                            lobj->u.spot.ref_br,
                                            lobj->u.spot.dist_func);
                        GXInitLightDistAttn(LOBJ_SPEC_LIGHTOBJ(lobj),
                                            ref_dist, lobj->u.spot.ref_br,
                                            lobj->u.spot.dist_func);
                        GXInitLightSpot(LOBJ_LIGHTOBJ(lobj),
                                        lobj->u.spot.cutoff,
                                        lobj->u.spot.spot_func);
                    }
                    break;
                default:
                    break;
                }

                if (ref_flags & (LOBJ_DIFFUSE | LOBJ_ALPHA)) {
                    color = lobj->color;
                    fn_800BA440(LOBJ_LIGHTOBJ(lobj), &color);
                    lobj->hw_color = color;
                    lobj->flags |= LOBJ_DIFF_DIRTY;

                    switch (type) {
                    case LOBJ_INFINITE:
                        break;
                    case LOBJ_POINT:
                    case LOBJ_SPOT:
                        lbl_8047B2C4 |= lobj->id;
                        break;
                    default:
                        __assert(&lbl_8047DBB8, 0x298, &lbl_8047DBD4);
                        break;
                    }

                    if (lobj->flags & LOBJ_DIFFUSE) {
                        lbl_8047B2BC |= lobj->id;
                    }
                    if (lobj->flags & LOBJ_ALPHA) {
                        lbl_8047B2C8 |= lobj->id;
                    }
                } else if (ref_flags & LOBJ_SPECULAR) {
                    LOBJ_SETUP_SPECULAR(lobj, view_mtx, lobj->id);
                }
            }
        }
        node = node->next;
    }

    if (LOBJ_ACTIVE_SLOT(8) == NULL) {
        while (node != NULL) {
            lobj = (HSD_LObj*) node->data;
            if (lobj != NULL && (lobj->flags & LOBJ_HIDDEN) == 0 &&
                (lobj->flags & LOBJ_TYPE_MASK) == LOBJ_AMBIENT &&
                (lobj->flags & (LOBJ_DIFFUSE | LOBJ_ALPHA))) {
                LOBJ_ACTIVE_SLOT(8) = lobj;
                lobj->id = fn_801A4A54(8);
                break;
            }
            node = node->next;
        }
    }

    active_limit = lbl_8047B2B8;
    for (i = 0; light_slot < 8 && i < active_limit; i++) {
        lobj = LOBJ_ACTIVE_SLOT(i);
        if (lobj != NULL &&
            (lobj->flags & LOBJ_SPECULAR) &&
            (lobj->flags & (LOBJ_DIFFUSE | LOBJ_ALPHA))) {
            LOBJ_SETUP_SPECULAR(lobj, view_mtx, fn_801A4A54(light_slot));
            light_slot++;
        }
    }

    for (i = 0; i < active_limit; i++) {
        lobj = LOBJ_ACTIVE_SLOT(i);
        if (lobj != NULL) {
            color = lobj->color;
            LOBJ_RUNTIME_SETUP(lobj, &color, lobj->shininess);
        }
    }
}
#endif

/* 0x801A6098 | 0x174 */
extern f32 lbl_8047DBFC;
extern f32 lbl_8047DBE4;
extern f32 lbl_8047DBE8;
#if 0
asm void fn_801A6098(void) {
#include "src/hsd/hsd_lobj_fn_801A6098.inc"
}
#else
void fn_801A6098(HSD_LObj* lobj, u32* color, f32 shininess)
{
    LOBJ_RUNTIME_SETUP(lobj, color, shininess);
}
#endif

/* 0x801A620C | 0x164 */
extern f32 lbl_8047DBE8;
extern f32 lbl_8047DBE4;
#if 0
asm void fn_801A620C(void) {
#include "src/hsd/hsd_lobj_fn_801A620C.inc"
}
#else
void fn_801A620C(HSD_LObj* lobj, f32* out)
{
    u32 pos[3];
    u32 interest[3];
    s32 invalid;

    pos[0] = *(u32*)&lbl_80274D58[0];
    pos[1] = *(u32*)&lbl_80274D58[4];
    pos[2] = *(u32*)&lbl_80274D58[8];
    interest[0] = *(u32*)&lbl_80274D64[0];
    interest[1] = *(u32*)&lbl_80274D64[4];
    interest[2] = *(u32*)&lbl_80274D64[8];

    if (lobj != NULL) {
        if (lobj != NULL && *(volatile u32*)((u8*)lobj + 0x18) != 0) {
            fn_80191688(lobj->position, pos);
        }
        if (lobj != NULL && *(volatile u32*)((u8*)lobj + 0x1C) != 0) {
            fn_80191688(lobj->interest, interest);
        }
        fn_800A3A9C(interest, pos, out);

        if (out == NULL) {
            goto invalid_vec;
        }
        if (out != NULL) {
            goto check_vec;
        }
    invalid_vec:
        invalid = -1;
        goto checked_vec;
    check_vec:
        if (__fabs(out[0]) <= *(volatile f32*)&lbl_80478AC8 &&
            __fabs(out[1]) <= *(volatile f32*)&lbl_80478AC8 &&
            __fabs(out[2]) <= *(volatile f32*)&lbl_80478AC8) {
            invalid = -1;
        } else {
            fn_800A3ADC(out, out);
            invalid = 0;
        }

    checked_vec:
        if (invalid != 0) {
            f32 x = *(f32*)&lbl_8047DBE8;
            f32 y = *(f32*)&lbl_8047DBE8;
            f32 z = *(f32*)&lbl_8047DBE4;
            out[0] = x;
            out[1] = y;
            out[2] = z;
        }
    }
}
#endif

/* 0x801A6370 | 0x98 */
extern void fn_801C29C4();
#if 0
asm void HSD_LObjReqAnimAll(void) {
#include "src/hsd/hsd_lobj_HSD_LObjReqAnimAll.inc"
}
#else
/* HSD_LObjReqAnimAll - request animation start for all lobjs in list */
void HSD_LObjReqAnimAll(HSD_LObj* lobj, f32 startframe) {
    HSD_LObj* l;
    if (lobj == NULL) return;
    for (l = lobj; l != NULL; l = l->next) {
        if (l == NULL) continue;
        fn_801C29C4(l->aobj, startframe);
        HSD_WObjReqAnim(l != NULL ? l->position : NULL, startframe);
        HSD_WObjReqAnim(l != NULL ? l->interest : NULL, startframe);
    }
}
#endif

/* 0x801A6408 | 0x8C */
extern void fn_801C27F4();
#if 0
asm void HSD_LObjAnimAll(void) {
#include "src/hsd/hsd_lobj_HSD_LObjAnimAll.inc"
}
#else
/* HSD_LObjAnimAll - animate all lobjs in list */
void HSD_LObjAnimAll(HSD_LObj* lobj) {
    HSD_LObj* l;
    if (lobj == NULL) return;
    for (l = lobj; l != NULL; l = l->next) {
        if (l == NULL) continue;
        fn_801C27F4(l->aobj, l, HSD_LOBJ_METHOD(l)->update);
        HSD_WObjInterpretAnim(l != NULL ? l->position : NULL);
        HSD_WObjInterpretAnim(l != NULL ? l->interest : NULL);
    }
}
#endif

/* 0x801A6494 | 0x24C */
extern void jumptable_8036CAF4();
extern f64 lbl_8047DC08;
extern f32 lbl_8047DBE8;
extern f32 lbl_8047DBE4;
extern f32 lbl_8047DC10;
#if 0
asm void LObjUpdateFunc(void) {
#include "src/hsd/hsd_lobj_LObjUpdateFunc.inc"
}
#else
void LObjUpdateFunc(HSD_LObj* lobj, u32 type, void* value)
{
    f32 val;
    u8 color;
    s32 channel;

    if (lobj == NULL) {
        return;
    }
    if (type < HSD_A_L_LITC_R || type > HSD_A_L_LITC_A) {
        return;
    }

    val = *(f32*) value;
    switch (type) {
    case HSD_A_L_VIS:
        if (val < lbl_8047DC08) {
            lobj->flags |= LOBJ_HIDDEN;
        } else {
            lobj->flags &= (u16) ~LOBJ_HIDDEN;
        }
        break;
    case HSD_A_L_A0:
    case HSD_A_L_CUTOFF:
        lobj->u.attn.a0 = val;
        break;
    case HSD_A_L_A1:
    case HSD_A_L_REFDIST:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a1 = val;
        } else {
            lobj->u.point.ref_dist = val;
        }
        break;
    case HSD_A_L_A2:
    case HSD_A_L_REFBRIGHT:
        lobj->u.attn.a2 = val;
        break;
    case HSD_A_L_K0:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k0 = val;
        }
        break;
    case HSD_A_L_K1:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k1 = val;
        }
        break;
    case HSD_A_L_K2:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k2 = val;
        }
        break;
    case HSD_A_L_LITC_R:
    case HSD_A_L_LITC_G:
    case HSD_A_L_LITC_B:
    case HSD_A_L_LITC_A:
        if (val <= lbl_8047DBE8) {
            val = lbl_8047DBE8;
        } else if (val >= lbl_8047DBE4) {
            val = lbl_8047DBE4;
        }
        if (type == HSD_A_L_LITC_A) {
            channel = 3;
        } else {
            channel = type - HSD_A_L_LITC_R;
        }
        color = (u8) (lbl_8047DC10 * val);
        ((u8*) &lobj->color)[channel] = color;
        break;
    default:
        break;
    }
}
#endif

/* 0x801A66E0 | 0xAC */
extern void HSD_WObjRemoveAnim(HSD_WObj* wobj);
#if 0
asm void HSD_LObjRemoveAnimAll(HSD_LObj* lobj) {
#include "src/hsd/hsd_lobj_HSD_LObjRemoveAnimAll.inc"
}
#else
void HSD_LObjRemoveAnimAll(HSD_LObj* lobj)
{
    HSD_LObj* outer;
    HSD_LObj* inner;

    if (lobj == NULL) {
        return;
    }

    for (outer = lobj; outer != NULL; outer = outer->next) {
        for (inner = outer; inner != NULL; inner = inner->next) {
            fn_801C25E4(inner->aobj);
            inner->aobj = NULL;
            HSD_WObjRemoveAnim(inner->position);
            HSD_WObjRemoveAnim(inner->interest);
        }
    }
}
#endif

/* 0x801A678C | 0x30 */
#if 0
asm void HSD_LObjGetActiveByIndex(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetActiveByIndex.inc"
}
#else
/* Get active light by index from lbl_804655E0 table */
extern u8 lbl_804655E0[];
#pragma optimization_level 1
HSD_LObj* HSD_LObjGetActiveByIndex(s32 idx) {
    if (idx >= 0) {
        if (idx < 8) {
            return *(HSD_LObj**)(lbl_804655E0 + idx * 4);
        }
    }
    return NULL;
}
#endif

/* 0x801A67BC | 0x114 */
#if 0
asm void HSD_LObjGetActiveByID(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetActiveByID.inc"
}
#else
HSD_LObj* HSD_LObjGetActiveByID(s32 light_id)
{
    s32 index;

    switch (light_id) {
        case 1:
            index = 0;
            break;
        case 2:
            index = 1;
            break;
        case 4:
            index = 2;
            break;
        case 8:
            index = 3;
            break;
        case 0x10:
            index = 4;
            break;
        case 0x20:
            index = 5;
            break;
        case 0x40:
            index = 6;
            break;
        case 0x80:
            index = 7;
            break;
        case 0x100:
            index = 8;
            break;
        default:
            __assert(&lbl_8047DBB8, 0x4A1, &lbl_8047DBD4);
            break;
    }

    {
        s32 checked;

        checked = index;
        if (checked >= 0 && checked < 9) {
            return *(HSD_LObj**)(lbl_804655E0 + checked * 4);
        }
    }
    return NULL;
}
#endif

/* 0x801A68D0 | 0x8 */
extern s32 lbl_8047B2B8;
#if 0
asm void HSD_LObjGetNbActive(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetNbActive.inc"
}
#else
s32 HSD_LObjGetNbActive(void) {
    return lbl_8047B2B8;
}
#endif

/* 0x801A68D8 | 0x8 */
extern u32 lbl_8047B2C0;
#if 0
asm void HSD_LObjGetLightMaskSpecular(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetLightMaskSpecular.inc"
}
#else
u32 HSD_LObjGetLightMaskSpecular(void) {
    return lbl_8047B2C0;
}
#endif

/* 0x801A68E0 | 0x8 */
extern u32 lbl_8047B2C8;
#if 0
asm void HSD_LObjGetLightMaskAlpha(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetLightMaskAlpha.inc"
}
#else
u32 HSD_LObjGetLightMaskAlpha(void) {
    return lbl_8047B2C8;
}
#endif

/* 0x801A68E8 | 0x8 */
extern u32 lbl_8047B2C4;
#if 0
asm void HSD_LObjGetLightMaskAttnFunc(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetLightMaskAttnFunc.inc"
}
#else
u32 HSD_LObjGetLightMaskAttnFunc(void) {
    return lbl_8047B2C4;
}
#endif

/* 0x801A68F0 | 0x8 */
extern u32 lbl_8047B2BC;
#if 0
asm void HSD_LObjGetLightMaskDiffuse(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetLightMaskDiffuse.inc"
}
#else
u32 HSD_LObjGetLightMaskDiffuse(void) {
    return lbl_8047B2BC;
}
#endif

/* 0x801A68F8 | 0x18 */
#if 0
asm void HSD_LObjClearFlags(void) {
#include "src/hsd/hsd_lobj_HSD_LObjClearFlags.inc"
}
#else
void HSD_LObjClearFlags(HSD_LObj* lobj, u32 flags) {
    if (lobj == NULL) return;
    lobj->flags &= ~flags;
}
#endif

/* 0x801A6910 | 0x18 */
#if 0
asm void HSD_LObjSetFlags(void) {
#include "src/hsd/hsd_lobj_HSD_LObjSetFlags.inc"
}
#else
void HSD_LObjSetFlags(HSD_LObj* lobj, u32 flags) {
    if (lobj == NULL) return;
    lobj->flags |= flags;
}
#endif

/* 0x801A6990 | 0x30 */
extern u8 lbl_80465608[];
#if 0
asm void fn_801A6990(void) {
#include "src/hsd/hsd_lobj_fn_801A6990.inc"
}
#else
/* Call virtual dispatch via lbl_80465608 */
typedef void (*fn_ptr)(void*);
void fn_801A6990(HSD_LObj* lobj) {
    fn_ptr func;
    func = ((fn_ptr*) lbl_80465608)[4];
    func(lobj);
}
#endif
