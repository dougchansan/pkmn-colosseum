/**
 * @file hsd_lobj.c
 * @brief HSD LObj - Light object implementation.
 *
 * Colosseum address: 0x801A4000 (LObjInfoInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/lobj.c
 */

#include "hsd/hsd_lobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"
#include "hsd/hsd_wobj.h"

static void LObjInfoInit(void);
extern s32 LObjLoad(HSD_LObj* lobj, HSD_LightDesc* ldesc);
extern void LObjUpdateFunc(HSD_LObj* lobj, u32 type, f32* val);

HSD_LObjInfo hsdLObj = { LObjInfoInit };

static HSD_LObjInfo* default_class = NULL;
static s32 nb_active = 0;

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

void HSD_LObjSetColor_Early(HSD_LObj* lobj, GXColor color)
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

/* NOTE: LObjRelease_801A40F8/LObjAmnesia_801A4098's real bodies live in the address-scaffolded
 * section below (0x801A40F8 / 0x801A4098); forward-declared here so
 * LObjInfoInit() can wire them into the vtable. */
static void LObjRelease_801A40F8(HSD_Class* o);
static void LObjAmnesia_801A4098(HSD_ClassInfo* info);

static void LObjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdLObj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_lobj",
                     sizeof(HSD_LObjInfo), sizeof(HSD_LObj));
    HSD_CLASS_INFO(&hsdLObj)->release = LObjRelease_801A40F8;
    HSD_CLASS_INFO(&hsdLObj)->amnesia = LObjAmnesia_801A4098;
    HSD_LOBJ_INFO(&hsdLObj)->load =
        (int (*)(HSD_LObj*, HSD_LightDesc*)) LObjLoad;
    HSD_LOBJ_INFO(&hsdLObj)->update =
        (void (*)(HSD_LObj*, u32, void*)) LObjUpdateFunc;
}

/* 0x801A4098 | 0x60 */
extern HSD_ClassInfo* lbl_8047B2B0;
extern u8 lbl_8036CA20[];
extern HSD_SList* lbl_8047B2B4;
#if 0
asm void LObjAmnesia_801A4098(void) {
#include "src/hsd/hsd_lobj_fn_801A4098.inc"
}
#else
#pragma push
#pragma peephole off
extern u8 lbl_8036CA20[];
extern void fn_801C25E4(HSD_AObj*);
static void LObjAmnesia_801A4098(HSD_ClassInfo* param) {
    if (param == lbl_8047B2B0) {
        lbl_8047B2B0 = NULL;
    }
    if ((void*)param == (void*)lbl_8036CA20) {
        lbl_8047B2B4 = NULL;
    }
    {
        void* ptr = (void*)lbl_8036CA20;
        void* vtable = *(void**)((u8*)ptr + 0x14);
        void (*func)(void*) = *(void(**)(void*))((u8*)vtable + 0x38);
        func((void*)param);
    }
}
#pragma pop
#endif

/* 0x801A40F8 | 0x174 */
#if 0
asm void LObjRelease_801A40F8(void) {
#include "src/hsd/hsd_lobj_fn_801A40F8.inc"
}
#else
static void LObjRelease_801A40F8(HSD_Class* o)
{
    HSD_LObj* lobj = (HSD_LObj*) o;
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
            if (*(HSD_AObj* volatile*) &l->aobj != NULL) {
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
asm void HSD_LObjLoadDesc(void) {
#include "src/hsd/hsd_lobj_fn_801A4344.inc"
}
#else
HSD_LObj* HSD_LObjLoadDesc(HSD_LightDesc* ldesc)
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
extern u8 lbl_80274D94[]; /* "unexpected lightdesc flags (%x)\n" */
extern char lbl_8047DBC8;
#if 0
asm void LObjLoad(void) {
#include "src/hsd/hsd_lobj_fn_801A4440.inc"
}
#else
s32 LObjLoad(HSD_LObj* lobj, HSD_LightDesc* ldesc)
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
/* HSD_LObjGetInterest */
s32 HSD_LObjGetInterest(HSD_LObj* lobj, Vec* out) {
    if (lobj != NULL) {
        if (*(HSD_WObj* volatile*) &lobj->interest != NULL) {
            HSD_WObjGetPosition(lobj->interest, out);
            return 1;
        }
    }
    return 0;
}

/* 0x801A48F4 | 0x88 */
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
void HSD_LObjSetInterest(HSD_LObj* lobj, Vec* interest) {
    if (lobj == NULL) {
        __assert(&lbl_8047DBB8, 0x58c, &lbl_8047DBCC);
    }
    if (lobj->interest == NULL) {
        lobj->interest = HSD_WObjAlloc();
        if (lobj->interest == NULL) {
            __assert(&lbl_8047DBB8, 0x58f, (const char*) lbl_80274DB8);
        }
    }
    HSD_WObjSetPosition(lobj->interest, interest);
}
#pragma pop
#endif

/* 0x801A497C | 0x44 */
#if 0
asm void HSD_LObjGetPosition(void) {
#include "src/hsd/hsd_lobj_HSD_LObjGetPosition.inc"
}
#else
s32 HSD_LObjGetPosition(HSD_LObj* lobj, Vec* out) {
    if (lobj != NULL) {
        if (*(HSD_WObj* volatile*) &lobj->position != NULL) {
            HSD_WObjGetPosition(lobj->position, out);
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
void HSD_LObjSetPosition(HSD_LObj* lobj, Vec* position) {
    if (lobj == NULL) {
        __assert(&lbl_8047DBB8, 0x568, &lbl_8047DBCC);
    }
    if (lobj->position == NULL) {
        lobj->position = HSD_WObjAlloc();
        if (lobj->position == NULL) {
            __assert(&lbl_8047DBB8, 0x56b, (const char*) lbl_80274DC8);
        }
    }
    HSD_WObjSetPosition(lobj->position, position);
}
#endif

/* 0x801A4A48 | 0xC */
#if 0
asm void HSD_LObjSetColor(void) {
#include "src/hsd/hsd_lobj_HSD_LObjSetColor.inc"
}
#else
void HSD_LObjSetColor(HSD_LObj* lobj, GXColor* color) {
    lobj->color = *color;
}
#endif

/* 0x801A4A54 | 0x70 */
extern void jumptable_8036CA64();
/* GXLightIndex to GXLightID bitmask */
#if 0
asm void HSD_Index2LightID(void) {
#include "src/hsd/hsd_lobj_fn_801A4A54.inc"
}
#else
#pragma push
#pragma optimization_level 1
u32 HSD_Index2LightID(u32 idx) {
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
#pragma pop
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
extern HSD_LObj* lbl_804655E0[];
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
                        if (lbl_804655E0[i] == cur) {
                            lbl_804655E0[i] = NULL;
                        }
                    }
                    *pp = fn_801A3E64(*pp);
                    if (cur != NULL && ref_DEC_801A0D48(cur)) {
                        hsdDelete_801A0CE8(cur);
                    }
                    break;
                }
                pp = &(*pp)->next;
            }
        }
    } else {
        for (i = 0; i < MAX_GXLIGHT; i++) {
            lbl_804655E0[i] = NULL;
        }
        lbl_8047B2B8 = 0;
        while (lbl_8047B2B4 != NULL) {
            data = (HSD_LObj*) lbl_8047B2B4->data;
            if (data != NULL && ref_DEC_801A0D48(data)) {
                hsdDelete_801A0CE8(data);
            }
            lbl_8047B2B4 = fn_801A3E64(lbl_8047B2B4);
        }
    }
}
#endif

/* 0x801A4D20 | 0x234 */
extern HSD_SList* HSD_SListPrepend(HSD_SList* next, void* data);
extern u8 lbl_80274DD8[];
extern u8 lbl_80274DE4[];
extern char lbl_8047DBD4;
extern char lbl_8047DBD8;
#if 0
asm void HSD_LObjAddCurrentAll(void) {
#include "src/hsd/hsd_lobj_fn_801A4D20.inc"
}
#else
void HSD_LObjAddCurrentAll(HSD_LObj* lobj)
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
                            if (lbl_804655E0[i] == cur) {
                                lbl_804655E0[i] = NULL;
                            }
                        }
                        *pp = fn_801A3E64(*pp);
                        if (cur != NULL && ref_DEC_801A0D48(cur)) {
                            hsdDelete_801A0CE8(cur);
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
        *pp = HSD_SListPrepend(*pp, cur);
    }
}
#endif

/* ========================================================================= */
/*  Light setup                                                              */
/* ========================================================================= */

#pragma push
#pragma optimization_level 1
#pragma inline_max_size(20000)
#pragma inline_max_auto_size(20000)

extern void PSVECAdd(const Vec* a, const Vec* b, Vec* dst);
extern void PSVECSubtract(const Vec* a, const Vec* b, Vec* dst);
extern void PSVECNormalize(const Vec* src, Vec* dst);
extern void PSMTXMultVecSR(const Mtx m, const Vec* src, Vec* dst);

extern void GXInitLightSpot(GXLightObj* lo, f32 cutoff, u32 spot_func);
extern void GXInitLightDistAttn(GXLightObj* lo, f32 ref_dist, f32 ref_br,
                                u32 dist_func);
extern void GXLoadLightObjImm(GXLightObj* lo, u32 light_id);
/* GXInitLightPos / GXInitLightDir / GXInitLightColor / GXInitLightAttn */
extern void fn_800BA414(GXLightObj* lo, f32 x, f32 y, f32 z);
extern void fn_800BA424(GXLightObj* lo, f32 nx, f32 ny, f32 nz);
extern void fn_800BA198(GXLightObj* lo, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1,
                        f32 k2);

/* active_lights[MAX_GXLIGHT] */
extern HSD_LObj* lbl_804655E0[];
/* current_lights */
extern HSD_SList* lbl_8047B2B4;
/* nb_active_lights */
extern s32 lbl_8047B2B8;
/* lightmask_diffuse / _specular / _attnfunc / _alpha */
extern u32 lbl_8047B2BC;
extern u32 lbl_8047B2C0;
extern u32 lbl_8047B2C4;
extern u32 lbl_8047B2C8;

/* Zero vectors used as the default position / interest. */
extern const Vec lbl_80274D58;
extern const Vec lbl_80274D64;

/* Degenerate-vector epsilon (lives in .sdata, addressed absolutely). */
extern const f32 lbl_80478AC8[];

extern const f32 lbl_8047DBE0; /* 1048576.0f */
extern const f32 lbl_8047DBE4; /* 1.0f */
extern const f32 lbl_8047DBE8; /* 0.0f */
extern const f32 lbl_8047DBEC; /* 0.001f */
extern const f64 lbl_8047DBF0; /* 0.001 */
extern const f32 lbl_8047DBF8; /* 50.0f */
extern const f32 lbl_8047DBFC; /* 0.5f */
extern const f32 lbl_8047DC00; /* -1.0f */

/**
 * Normalises @p src into @p dst.  Returns -1 when the source vector is
 * degenerate (all components within the epsilon), 0 otherwise.
 */
static inline s32 LObjNormalizeVec(const Vec* src, Vec* dst)
{
    if (src == NULL || dst == NULL) {
        return -1;
    }
    if (__fabs(src->x) <= lbl_80478AC8[0] &&
        __fabs(src->y) <= lbl_80478AC8[0] &&
        __fabs(src->z) <= lbl_80478AC8[0])
    {
        return -1;
    }
    PSVECNormalize(src, dst);
    return 0;
}

/** Normalise @p src into @p dst, falling back to (x, y, z) if degenerate. */
static inline void LObjNormalizeVecOr(const Vec* src, Vec* dst, f32 x, f32 y, f32 z)
{
    if (LObjNormalizeVec(src, dst) != 0) {
        dst->x = x;
        dst->y = y;
        dst->z = z;
    }
}

/** Unit vector from the light position towards its interest point. */
static inline void LObjGetLightVector(HSD_LObj* lobj, Vec* dir)
{
    Vec interest = lbl_80274D64;
    Vec position = lbl_80274D58;

    if (lobj == NULL) {
        return;
    }

    HSD_LObjGetPosition(lobj, &position);
    HSD_LObjGetInterest(lobj, &interest);
    PSVECSubtract(&interest, &position, dir);
    LObjNormalizeVecOr(dir, dir, lbl_8047DBE8, lbl_8047DBE8, lbl_8047DBE4);
}

/** Assign @p lobj the next free hardware light slot. */
static inline void LObjSetActive(HSD_LObj* lobj)
{
    s32 idx;

    if ((lobj->flags & LOBJ_TYPE_MASK) == LOBJ_AMBIENT) {
        idx = MAX_GXLIGHT - 1;
        if (lbl_804655E0[idx] != NULL) {
            return;
        }
    } else {
        idx = lbl_8047B2B8++;
    }
    lbl_804655E0[idx] = lobj;
    lobj->id = HSD_Index2LightID(idx);
}

static inline void LObjClearActive(void)
{
    s32 i;

    for (i = 0; i < MAX_GXLIGHT; i++) {
        lbl_804655E0[i] = NULL;
    }
    lbl_8047B2B8 = 0;
}

static inline HSD_LObj* LObjGetActiveByIndex(s32 idx)
{
    if (idx >= 0 && idx < MAX_GXLIGHT - 1) {
        return lbl_804655E0[idx];
    }
    return NULL;
}

static inline HSD_LObj* LObjGetActiveBySlot(s32 idx)
{
    if (idx >= 0 && idx < MAX_GXLIGHT) {
        return lbl_804655E0[idx];
    }
    return NULL;
}

static inline void LObjSetupDiffuse(HSD_LObj* lobj)
{
    fn_800BA440(&lobj->lightobj, lobj->color);
    lobj->hw_color = lobj->color;
    lobj->flags |= LOBJ_DIFF_DIRTY;

    switch (lobj->flags & LOBJ_TYPE_MASK) {
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
}

static inline void LObjSetupSpecular(HSD_LObj* lobj, MtxPtr mtx, s32 spec_id)
{
    lobj->spec_id = spec_id;
    if (spec_id != 0) {
        fn_800BA440(&lobj->spec_lightobj, lobj->color);
        lobj->shininess = lbl_8047DBF8;
        fn_800BA198(&lobj->spec_lightobj, lbl_8047DBE8, lbl_8047DBE8,
                    lbl_8047DBE4, lobj->shininess * lbl_8047DBFC, lbl_8047DBE8,
                    lbl_8047DBE4 - lobj->shininess * lbl_8047DBFC);

        switch (lobj->flags & LOBJ_TYPE_MASK) {
        case LOBJ_POINT:
        case LOBJ_SPOT:
            HSD_LObjGetPosition(lobj, &lobj->lvec);
            PSMTXMultVec(mtx, &lobj->lvec, &lobj->lvec);
            break;
        case LOBJ_INFINITE:
            LObjGetLightVector(lobj, &lobj->lvec);
            PSMTXMultVecSR(mtx, &lobj->lvec, &lobj->lvec);
            PSVECNormalize(&lobj->lvec, &lobj->lvec);
            break;
        default:
            __assert(&lbl_8047DBB8, 0x2C0, &lbl_8047DBD4);
            break;
        }

        lobj->flags |= LOBJ_SPEC_DIRTY;
        lbl_8047B2C0 |= spec_id;
    }
}

static inline void LObjSetupInfinite(HSD_LObj* lobj, MtxPtr vmtx)
{
    Vec lpos;

    HSD_LObjGetPosition(lobj, &lpos);
    lpos.x *= lbl_8047DBE0;
    lpos.y *= lbl_8047DBE0;
    lpos.z *= lbl_8047DBE0;
    PSMTXMultVec(vmtx, &lpos, &lpos);

    if (lobj->flags & LOBJ_DIFFUSE) {
        fn_800BA414(&lobj->lightobj, lpos.x, lpos.y, lpos.z);
        fn_800BA198(&lobj->lightobj, lbl_8047DBE4, lbl_8047DBE8, lbl_8047DBE8,
                    lbl_8047DBE4, lbl_8047DBE8, lbl_8047DBE8);
    }
    if (lobj->flags & LOBJ_SPECULAR) {
        fn_800BA414(&lobj->spec_lightobj, lpos.x, lpos.y, lpos.z);
    }
}

static inline void LObjSetupPoint(HSD_LObj* lobj, MtxPtr mtx)
{
    Vec lpos;

    fn_800BA440(&lobj->lightobj, lobj->color);
    lobj->hw_color = lobj->color;
    HSD_LObjGetPosition(lobj, &lpos);
    PSMTXMultVec(mtx, &lpos, &lpos);
    fn_800BA414(&lobj->lightobj, lpos.x, lpos.y, lpos.z);
    fn_800BA414(&lobj->spec_lightobj, lpos.x, lpos.y, lpos.z);

    if (lobj->flags & LOBJ_RAW_PARAM) {
        fn_800BA198(&lobj->lightobj, lbl_8047DBE4, lbl_8047DBE8, lbl_8047DBE8,
                    lobj->u.attn.k0, lobj->u.attn.k1, lobj->u.attn.k2);
    } else {
        f32 ref_br = lobj->u.spot.ref_br;
        f32 ref_dist = lobj->u.spot.ref_dist;
        u32 dist_func = lobj->u.spot.dist_func;

        GXInitLightDistAttn(&lobj->lightobj, ref_dist, ref_br, dist_func);
        GXInitLightSpot(&lobj->lightobj, lbl_8047DBE8, 0);
        GXInitLightDistAttn(&lobj->spec_lightobj, ref_dist, ref_br, dist_func);
    }
}

static inline void LObjSetupSpot(HSD_LObj* lobj, MtxPtr mtx)
{
    Vec lpos;
    Vec ldir;

    HSD_LObjGetPosition(lobj, &lpos);
    PSMTXMultVec(mtx, &lpos, &lpos);
    LObjGetLightVector(lobj, &ldir);
    PSMTXMultVecSR(mtx, &ldir, &ldir);
    PSVECNormalize(&ldir, &ldir);
    fn_800BA414(&lobj->lightobj, lpos.x, lpos.y, lpos.z);
    fn_800BA414(&lobj->spec_lightobj, lpos.x, lpos.y, lpos.z);
    fn_800BA424(&lobj->lightobj, ldir.x, ldir.y, ldir.z);

    if (lobj->flags & LOBJ_RAW_PARAM) {
        fn_800BA198(&lobj->lightobj, lobj->u.attn.a0, lobj->u.attn.a1,
                    lobj->u.attn.a2, lobj->u.attn.k0, lobj->u.attn.k1,
                    lobj->u.attn.k2);
    } else {
        f32 ref_dist = lobj->u.point.ref_dist;
        f32 ref_br = lobj->u.point.ref_br;
        f32 cutoff = lobj->u.point.cutoff;
        u32 spot_func = lobj->u.spot.spot_func;
        u32 dist_func = lobj->u.spot.dist_func;

        if (ref_dist < lbl_8047DBF0) {
            ref_dist = lbl_8047DBEC;
        }
        GXInitLightDistAttn(&lobj->lightobj, ref_dist, ref_br, dist_func);
        GXInitLightDistAttn(&lobj->spec_lightobj, ref_dist, ref_br, dist_func);
        GXInitLightSpot(&lobj->lightobj, cutoff, spot_func);
    }
}

extern void fn_801A6098(HSD_LObj* lobj, GXColor color, f32 shininess);

/* 0x801A4F54 | 0xE78 */
/* Really HSD_LObjSetupInit: binds every current light to a hardware slot. */
void HSD_LObjSetup(HSD_CObj* cobj)
{
    MtxPtr vmtx;
    HSD_SList* list;
    s32 i;
    s32 num;
    s32 idx;

    lbl_8047B2BC = 0;
    lbl_8047B2C0 = 0;
    lbl_8047B2C4 = 0;
    lbl_8047B2C8 = 0;
    vmtx = cobj->view_mtx;

    idx = 0;
    LObjClearActive();

    for (list = lbl_8047B2B4; idx < MAX_GXLIGHT - 1 && list != NULL;
         list = list->next)
    {
        HSD_LObj* lobj = (HSD_LObj*) list->data;
        u32 ref_type;
        u32 type;

        if (lobj == NULL) {
            continue;
        }
        if (lobj->flags & LOBJ_HIDDEN) {
            continue;
        }

        ref_type = lobj->flags & (LOBJ_DIFFUSE | LOBJ_SPECULAR | LOBJ_ALPHA);
        type = lobj->flags & LOBJ_TYPE_MASK;
        if (ref_type == 0) {
            continue;
        }

        LObjSetActive(lobj);
        idx = lbl_8047B2B8;
        lobj->spec_id = 0;

        switch (type) {
        case LOBJ_INFINITE:
            LObjSetupInfinite(lobj, vmtx);
            break;
        case LOBJ_POINT:
            LObjSetupPoint(lobj, vmtx);
            break;
        case LOBJ_SPOT:
            LObjSetupSpot(lobj, vmtx);
            break;
        case LOBJ_AMBIENT:
            continue;
        }

        if (ref_type & (LOBJ_DIFFUSE | LOBJ_ALPHA)) {
            LObjSetupDiffuse(lobj);
        } else if (ref_type & LOBJ_SPECULAR) {
            LObjSetupSpecular(lobj, vmtx, lobj->id);
        }
    }

    if (LObjGetActiveBySlot(MAX_GXLIGHT - 1) == NULL) {
        for (; list != NULL; list = list->next) {
            HSD_LObj* lobj = (HSD_LObj*) list->data;

            if (lobj == NULL) {
                continue;
            }
            if (lobj->flags & LOBJ_HIDDEN) {
                continue;
            }
            if ((lobj->flags & LOBJ_TYPE_MASK) != LOBJ_AMBIENT) {
                continue;
            }
            if (!(lobj->flags & (LOBJ_DIFFUSE | LOBJ_ALPHA))) {
                continue;
            }
            LObjSetActive(lobj);
            break;
        }
    }

    num = lbl_8047B2B8;
    for (i = 0; idx < MAX_GXLIGHT - 1 && i < num; i++) {
        HSD_LObj* lobj = LObjGetActiveByIndex(i);
        u32 flags;

        if (lobj == NULL) {
            continue;
        }
        flags = lobj->flags;
        if (!(flags & LOBJ_SPECULAR)) {
            continue;
        }
        if (!(flags & (LOBJ_DIFFUSE | LOBJ_ALPHA))) {
            continue;
        }
        LObjSetupSpecular(lobj, vmtx, HSD_Index2LightID(idx++));
    }

    for (i = 0; i < num; i++) {
        HSD_LObj* lobj = LObjGetActiveByIndex(i);

        if (lobj == NULL) {
            continue;
        }
        fn_801A6098(lobj, lobj->color, lobj->shininess);
    }
}

/* 0x801A5DCC | 0x2CC */
/* Really HSD_LObjSetupSpecularInit: rebuilds every specular half-vector. */
void fn_801A5DCC(MtxPtr pmtx)
{
    s32 i;
    s32 num;
    Vec cdir;
    Vec jpos;

    jpos.x = pmtx[0][3];
    jpos.y = pmtx[1][3];
    jpos.z = pmtx[2][3];
    LObjNormalizeVecOr(&jpos, &cdir, lbl_8047DBE8, lbl_8047DBE8, lbl_8047DC00);

    num = lbl_8047B2B8;
    for (i = 0; i < num; i++) {
        Vec half;
        Vec ldir;
        HSD_LObj* lobj = LObjGetActiveByIndex(i);

        if (lobj->spec_id == 0) {
            continue;
        }

        switch (lobj->flags & LOBJ_TYPE_MASK) {
        case LOBJ_POINT:
        case LOBJ_SPOT:
            PSVECSubtract(&jpos, &lobj->lvec, &ldir);
            if (LObjNormalizeVec(&ldir, &ldir) != 0) {
                half.x = lbl_8047DBE8;
                half.y = lbl_8047DBE8;
                half.z = lbl_8047DBE4;
                goto init_dir;
            }
            PSVECAdd(&ldir, &cdir, &half);
            break;
        case LOBJ_INFINITE:
            PSVECAdd(&lobj->lvec, &cdir, &half);
            break;
        default:
            __assert(&lbl_8047DBB8, 0x27A, &lbl_8047DBD4);
            break;
        }

        LObjNormalizeVecOr(&half, &half, lbl_8047DBE8, lbl_8047DBE8,
                           lbl_8047DBE4);
    init_dir:
        fn_800BA424(&lobj->spec_lightobj, half.x, half.y, half.z);
        lobj->flags |= LOBJ_SPEC_DIRTY;
    }
}

/* 0x801A6098 | 0x174 */
/* The public per-light HSD_LObjSetup(lobj, color, shininess). */
void fn_801A6098(HSD_LObj* lobj, GXColor color, f32 shininess)
{
    if ((lobj->flags & LOBJ_HIDDEN) ||
        (lobj->flags & LOBJ_TYPE_MASK) == LOBJ_AMBIENT)
    {
        return;
    }

    if (lobj->flags & (LOBJ_DIFFUSE | LOBJ_ALPHA)) {
        if (lobj->hw_color.r != color.r || lobj->hw_color.g != color.g ||
            lobj->hw_color.b != color.b || lobj->hw_color.a != color.a)
        {
            fn_800BA440(&lobj->lightobj, color);
            lobj->hw_color = color;
            lobj->flags |= LOBJ_DIFF_DIRTY;
        }
        if (lobj->flags & LOBJ_DIFF_DIRTY) {
            GXLoadLightObjImm(&lobj->lightobj, lobj->id);
            lobj->flags &= ~LOBJ_DIFF_DIRTY;
        }
    }

    if (lobj->spec_id != 0) {
        if (lobj->shininess != shininess) {
            lobj->shininess = shininess;
            fn_800BA198(&lobj->spec_lightobj, lbl_8047DBE8, lbl_8047DBE8,
                        lbl_8047DBE4, shininess * lbl_8047DBFC, lbl_8047DBE8,
                        lbl_8047DBE4 - shininess * lbl_8047DBFC);
            lobj->flags |= LOBJ_SPEC_DIRTY;
        }
        if (lobj->flags & LOBJ_SPEC_DIRTY) {
            GXLoadLightObjImm(&lobj->spec_lightobj, lobj->spec_id);
            lobj->flags &= ~LOBJ_SPEC_DIRTY;
        }
    }
}

/* 0x801A620C | 0x164 */
void HSD_LObjGetLightVector(HSD_LObj* lobj, Vec* dir)
{
    LObjGetLightVector(lobj, dir);
}


#pragma pop

/* 0x801A6370 | 0x98 */
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
        HSD_AObjReqAnim(l->aobj, startframe);
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
extern const f64 lbl_8047DC08;
extern const f32 lbl_8047DC10;
#if 0
asm void LObjUpdateFunc(void) {
#include "src/hsd/hsd_lobj_LObjUpdateFunc.inc"
}
#else
static u8 LObjClampColor(f32 val)
{
    if (val <= lbl_8047DBE8) {
        val = lbl_8047DBE8;
    } else if (val >= lbl_8047DBE4) {
        val = lbl_8047DBE4;
    }
    return (u8) (lbl_8047DC10 * val);
}

void LObjUpdateFunc(HSD_LObj* lobj, u32 type, f32* val)
{
    if (lobj == NULL) {
        return;
    }

    switch (type) {
    case HSD_A_L_VIS:
        if (*val >= lbl_8047DC08) {
            lobj->flags &= ~LOBJ_HIDDEN;
        } else {
            lobj->flags |= LOBJ_HIDDEN;
        }
        break;
    case HSD_A_L_A0:
    case HSD_A_L_CUTOFF:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a0 = *val;
        } else {
            lobj->u.spot.cutoff = *val;
        }
        break;
    case HSD_A_L_A1:
    case HSD_A_L_REFDIST:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a1 = *val;
        } else {
            lobj->u.spot.ref_dist = *val;
        }
        break;
    case HSD_A_L_A2:
    case HSD_A_L_REFBRIGHT:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.a2 = *val;
        } else {
            lobj->u.spot.ref_br = *val;
        }
        break;
    case HSD_A_L_K0:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k0 = *val;
        }
        break;
    case HSD_A_L_K1:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k1 = *val;
        }
        break;
    case HSD_A_L_K2:
        if (lobj->flags & LOBJ_RAW_PARAM) {
            lobj->u.attn.k2 = *val;
        }
        break;
    case HSD_A_L_LITC_R:
        lobj->color.r = LObjClampColor(*val);
        break;
    case HSD_A_L_LITC_G:
        lobj->color.g = LObjClampColor(*val);
        break;
    case HSD_A_L_LITC_B:
        lobj->color.b = LObjClampColor(*val);
        break;
    case HSD_A_L_LITC_A:
        lobj->color.a = LObjClampColor(*val);
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
/* Get active light by index from the active_lights table */
#pragma optimization_level 1
HSD_LObj* HSD_LObjGetActiveByIndex(s32 idx) {
    if (idx >= 0) {
        if (idx < 8) {
            return lbl_804655E0[idx];
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
            return lbl_804655E0[checked];
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

/* 0x801A6928 | 0x38 */
extern u8 lbl_80465608[];
void fn_801A6928(HSD_LObj* lobj) {
    void (*func)(HSD_LObj*, u32, u32);
    func = ((void (**)(HSD_LObj*, u32, u32)) lbl_80465608)[0];
    func(lobj, 0x20, 0);
}

/* 0x801A6960 | 0x30 */
void fn_801A6960(HSD_LObj* lobj) {
    void (*func)(HSD_LObj*);
    func = ((void (**)(HSD_LObj*)) lbl_80465608)[1];
    func(lobj);
}

/* 0x801A6990 | 0x30 */
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
