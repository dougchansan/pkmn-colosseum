/**
 * @file hsd_pobj_disp.c
 * @brief HSD PObj - polygon object lifecycle, memory management,
 *        and animation dispatch.
 *
 * Address range: 0x801AA35C - 0x801ADF54
 *
 * Decompiled from Melee src/sysdolphin/baselib/pobj.c
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_mobj.h"

/* =========================================================================
 * External function declarations
 * ========================================================================= */

extern void* fn_801A6928(s32 size);          /* HSD_MemAlloc */
extern void  fn_801A6960(void* ptr);          /* HSD_MemFree  */
/* fn_801A6990: virtual dispatch, returns s32 result via r3 */
extern s32   fn_801A6990(void* obj);
extern void  __assert(const char* file, u32 line, const char* msg);
extern void  HSD_Panic(const char* file, u32 line, const char* msg);
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void  fn_800B7D3C(void);
extern s32   fn_801BF138(u32 val);
extern void* fn_80193748(const char* name);
extern void* fn_80193828(void* classInfo);
extern void* fn_80193B10(s32 size);
extern void  fn_801C27F4(void* aobj, void* pobj, void* method);
extern void  fn_801C25E4(void* aobj);
extern void* fn_801C2670(void* ptr);
extern void  fn_801C29C4(f32 val, void* aobj);
extern void  fn_801A053C(void* ptr);
extern void* fn_801A3E64(void* ptr);
extern void* fn_801A3F48(void);
extern void* HSD_IDGetDataFromTable(void* desc, s32 a, s32 b);
extern void* fn_8019F01C(void* obj);
extern void  fn_8019D9DC(void* jobj);
extern void  fn_80193AF0(void* cls, s32 size);
extern void  fn_801BE490(s32 a, s32 b);
extern void  fn_800BD554(s32 a);
extern void  fn_800BD4B4(void* mtx, s32 a);
extern void  fn_800BD504(void* mtx, s32 a);
extern void  fn_800BD58C(void* mtx, s32 a, s32 b);
extern void  fn_800A2FAC(void* pobj, void* mtx);
extern void  fn_800A2D64(void* pobj, void* mtx);
extern void  fn_800A2D98(void* dst, void* src, void* dst2);
extern void  HSD_MtxScaledAdd(void* src, f32 weight, void* add, void* dst);
extern void  HSD_Index2TexMtx(s32 idx);
extern void* HSD_Index2PosNrmMtx(s32 idx);
extern void  fn_800B84E0(u32 type, void* data, u32 stride, u8 frac);
extern void  fn_800B7874(u32 type, u32 comptype);
extern void  fn_800B7D74(u32 type, u32 comptype, void* data, u32 stride,
                          u8 frac);
extern void  fn_800BD0FC(void* display, u32 nbytes);
extern void  fn_801B2878(s32 cullmode);
extern s32   fn_8019C6EC(s32 val);
extern f32   fn_800CE298(void);
extern f32   fn_800CE148(f32 x);
extern f32   fn_800CDBE0(f32 x);
extern void* fn_8019F01C(void* obj);

/* =========================================================================
 * Static global state (maps to sbss symbols accessed via r13)
 * ========================================================================= */

static void* lbl_8047B2E0;   /* free-list pool chain head */
static void* lbl_8047B2E8;   /* default class info pointer */
static void* lbl_8047B2EC;   /* active normal desc pointer */
static void* lbl_8047B2F0;   /* active color desc pointer */
static void* lbl_8047B2F4;   /* normal count */
static void* lbl_8047B2F8;   /* color count */
static u32   lbl_8047B2FC;   /* display list current marker */
static void* lbl_8047B300;   /* display list end marker */
static void* lbl_8047B308;   /* active texture desc */
static u32   lbl_8047B30C;   /* texture count */

/* sdata - extern because other TUs reference them */
extern void* lbl_80478C90;   /* RNG default state instance */
extern void* lbl_80478C94;   /* RNG current state pointer  */

/* data section labels */
extern u8 lbl_8036C638[];
extern u8 lbl_8036CC00[];
extern u8 lbl_8036CC40[];
extern u8 lbl_8036CBF0[];
extern u8 lbl_8036CCD0[];

/* bss labels */
extern u8 lbl_80465678[];
extern u8 lbl_80465688[];

/* sdata2 string constants */
extern const char lbl_8047DC98[];
extern const char lbl_8047DCA0;
extern const char lbl_8047DCA8[];
extern const char lbl_8047DCB0[];
extern const char lbl_8047DCB8[];
extern const char lbl_8047DCC4[];
extern const char lbl_8047DCCC[];
extern const char lbl_8047DCD8[];
extern const char lbl_8047DD10[];
extern const char lbl_8047DD50[];
extern const char lbl_8047DD58[];

/* sdata2 float constants */
extern const f32 lbl_8047DCC0;
extern const f32 lbl_8047DCD4;
extern const f32 lbl_8047DD18;
extern const f32 lbl_8047DD1C;
extern const f32 lbl_8047DD20;
extern const f32 lbl_8047DD38;
extern const f32 lbl_8047DD40;
extern const f32 lbl_8047DD78;
extern const double lbl_8047DD00;
extern const double lbl_8047DD08;
extern const double lbl_8047DD28;
extern const double lbl_8047DD30;
extern const double lbl_8047DD48;

/* rodata labels */
extern u8 lbl_80274E90[];
extern u8 lbl_80274EC8[];
extern u8 lbl_80274EE0[];
extern u8 lbl_80274EF8[];
extern u8 lbl_80274F04[];
extern u8 lbl_8027503C[];
extern u8 lbl_8027506C[];
extern u8 lbl_80275084[];
extern u8 lbl_802750B4[];

/* forward declarations */
void fn_801AA788(void* pobj, void* vmtx, void* pmtx, u32 rendermode);
void fn_801AA6D0(void* pobj);
void fn_801AA8BC(void* pobj, void* vmtx, void* pmtx, u32 rendermode);
void fn_801AABB4(void* pobj, void* vmtx, void* pmtx, u32 rendermode);
void fn_801AD354(void* pobj_ptr, void* desc);
void PObjUpdateFunc(void* pobj, s32 idx, f32* weight_ptr);
void fn_801AB67C(void* pobj);

/* =========================================================================
 * 0x801AA35C | size: 0x13C  -- Initialize free-list memory pool
 * ========================================================================= */
void fn_801AA35C(void* list, u32 size, u32 alignment)
{
    void* p = list;

    if (p == NULL) {
        __assert((const char*)lbl_80274E90, 0x1ae, lbl_8047DC98);
    }

    if (lbl_8047B2E0 != NULL) {
        void** slot = (void**)&lbl_8047B2E0;
        while (*slot != NULL) {
            if (*slot == p) {
                *slot = *(void**)((u8*)*slot + 0x28);
                break;
            }
            slot = (void**)((u8*)*slot + 0x28);
        }
    } else {
        lbl_8047B2E0 = NULL;
    }

    memset(p, 0, 0x2c);
    *(s32*)((u8*)p + 0x14) = -1;
    *(u32*)((u8*)p + 0x18) = 0;
    *(s32*)((u8*)p + 0x1c) = -1;

    if (size > 0x20) {
        __assert((const char*)lbl_80274E90, 0x1b9,
                    (const char*)lbl_80274E90 + 0xc);
    }
    fn_801BF138(size);
    if (fn_801BF138(size) != 1) {
        __assert((const char*)lbl_80274E90, 0x1ba,
                    (const char*)lbl_80274E90 + 0x18);
    }

    *(u32*)((u8*)p + 0x24) = size;
    *(u32*)((u8*)p + 0x20) = (size + alignment - 1) & ~(alignment - 1);
    *(void**)((u8*)p + 0x28) = lbl_8047B2E0;
    lbl_8047B2E0 = p;
}

/* =========================================================================
 * 0x801AA498 | size: 0x34  -- HSD_ObjFree
 * ========================================================================= */
#pragma push
#pragma optimization_level 1
void HSD_ObjFree(void* list, void* data)
{
    void* l = list;
    *(u32*)((u8*)l + 0x8) = *(u32*)((u8*)l + 0x8) - 1;
    fn_801A6960(data);
}
#pragma pop

/* =========================================================================
 * 0x801AA4CC | size: 0x6C  -- Allocate from pool
 * ========================================================================= */
#pragma push
#pragma optimization_level 1
void* HSD_ObjAlloc(void* list)
{
    void* l = list;

    if ((*(u8*)l & 0x80) >> 7) {
        if (*(u32*)((u8*)l + 0x8) >= *(u32*)((u8*)l + 0x14)) {
            return NULL;
        }
    }

    *(u32*)((u8*)l + 0x8) += 1;
    if (*(u32*)((u8*)l + 0x8) > *(u32*)((u8*)l + 0x10)) {
        *(u32*)((u8*)l + 0x10) = *(u32*)((u8*)l + 0x8);
    }

    return fn_801A6928(*(u32*)((u8*)l + 0x20));
}
#pragma pop

/* =========================================================================
 * 0x801AA538 | size: 0x30 -- HSD_ObjSetHeap
 * ========================================================================= */
#pragma push
#pragma optimization_level 0
void HSD_ObjSetHeap(void* a, void* b)
{
    *(volatile void**)((u8*)lbl_8036CBF0 + 0x4) = b;
    *(volatile void**)((u8*)lbl_8036CBF0 + 0x0) = b;
    *(volatile void**)((u8*)lbl_8036CBF0 + 0xc) = a;
    *(volatile void**)((u8*)lbl_8036CBF0 + 0x8) = a;
}
#pragma pop

/* =========================================================================
 * 0x801AA568 | size: 0x44  -- PObj class info init (small)
 * Calls hsdInitClassInfo to register lbl_8036CC00 class
 * ========================================================================= */
void ObjInfoInit_802596A4(void)
{
    hsdInitClassInfo((HSD_ClassInfo*) lbl_8036CC00,
                     (HSD_ClassInfo*) lbl_8036C638, (char*) lbl_80274EC8,
                     (char*) &lbl_8047DCA0, 0x3c, 0x8);
}

/* =========================================================================
 * 0x801AA5AC | size: 0x5C  -- Increment reference count in GX array
 * fn_801AA5AC(idx)
 * ========================================================================= */
void fn_801AA5AC(s32 idx)
{
    s32 i = idx;
    if (i >= 0x20) {
        __assert(lbl_8047DCA8, 0xa4, lbl_8047DCB0);
    }
    {
        u32* entry = (u32*)((u8*)lbl_8036CC40 + (u32)i * 4);
        entry[4] = entry[4] + 1;
    }
}

/* =========================================================================
 * 0x801AA608 | size: 0xC8  -- PObj class info init (main)
 * Calls hsdInitClassInfo to register lbl_8036CCD0 and installs vtable
 * ========================================================================= */
void HSD_PObjInit(void)
{
    hsdInitClassInfo((HSD_ClassInfo*) lbl_8036CCD0,
                     (HSD_ClassInfo*) lbl_8036C638, (char*) lbl_80274EE0,
                     (char*) lbl_80274EF8, 0x4c, 0x1c);

    *(void**)((u8*)lbl_8036CCD0 + 0x30) = (void*)fn_801AA788;
    *(void**)((u8*)lbl_8036CCD0 + 0x38) = (void*)fn_801AA6D0;
    *(void**)((u8*)lbl_8036CCD0 + 0x3c) = (void*)fn_801AA8BC;
    *(void**)((u8*)lbl_8036CCD0 + 0x40) = (void*)fn_801AABB4;
    *(void**)((u8*)lbl_8036CCD0 + 0x44) = (void*)fn_801AD354;
    *(void**)((u8*)lbl_8036CCD0 + 0x48) = (void*)PObjUpdateFunc;
}

/* =========================================================================
 * 0x801AA6D0 | size: 0xB8  -- PObj remove
 * ========================================================================= */
#pragma push
#pragma optimization_level 1
void fn_801AA6D0(void* pobj)
{
    void* p = pobj;

    if (p == lbl_8047B2E8) {
        lbl_8047B2E8 = NULL;
    }

    if (p == (void*)lbl_8036CCD0) {
        s32 r;

        r = fn_801A6990(lbl_8047B2EC);
        if (r != 0) {
            lbl_8047B2EC = NULL;
            lbl_8047B2F4 = NULL;
        }

        r = fn_801A6990(lbl_8047B2F0);
        if (r != 0) {
            lbl_8047B2F0 = NULL;
            lbl_8047B2F8 = NULL;
        }

        lbl_8047B2FC = 0;
        lbl_8047B300 = NULL;
    }

    {
        void** ci = *(void***)((u8*)lbl_8036CCD0 + 0x14);
        ((void(*)(void*))ci[0x38/4])(p);
    }
}
#pragma pop

/* =========================================================================
 * 0x801AA788 | size: 0x134  -- PObj display dispatch
 * ========================================================================= */
void fn_801AA788(void* pobj, void* vmtx, void* pmtx, u32 rendermode)
{
    void* p = pobj;
    void* pp = p;

    if (*(void**)((u8*)p + 0x18) != NULL) {
        fn_801C25E4(*(void**)((u8*)pp + 0x18));
    }

    {
        u16 flags = *(u16*)((u8*)pp + 0xc);
        u32 skintype = (u32)(flags & 0x3000);

        switch (skintype) {
        case 0x0000:
        {
            void* dl = *(void**)((u8*)pp + 0x14);
            if (dl == NULL) goto dispatch;
            {
                u16 fl = *(u16*)((u8*)dl + 0x0);
                if (fl & 0x2) {
                    fn_801A6960(*(void**)((u8*)dl + 0x1c));
                    if (dl != NULL) {
                        fn_80193AF0(dl, 0x20);
                    }
                    goto dispatch;
                }
            }
            {
                void* cur = dl;
                while (cur != NULL) {
                    void* nxt = *(void**)((u8*)cur + 0x4);
                    fn_801A053C(*(void**)((u8*)cur + 0x4));
                    if (cur != NULL) {
                        fn_80193AF0(cur, 0xc);
                    }
                    cur = nxt;
                }
            }
            {
                void* r = fn_801A3E64(dl);
                if (r == NULL) goto dispatch;
            }
            goto dispatch;
        }

        case 0x1000:
        {
            void* shapeset = *(void**)((u8*)pp + 0x14);
            if (shapeset == NULL) goto dispatch;
            fn_801A053C(shapeset);
            goto call_dispatch;
        }

        case 0x2000:
        {
            void* envlist = *(void**)((u8*)pp + 0x14);
            if (envlist == NULL) goto dispatch;
            {
                void* cur = envlist;
                while (cur != NULL) {
                    void* nxt  = *(void**)((u8*)cur + 0x4);
                    void* data = *(void**)((u8*)cur + 0x0);
                    fn_801A053C(*(void**)((u8*)cur + 0x4));
                    if (cur != NULL) {
                        fn_80193AF0(cur, 0xc);
                    }
                    cur = data;
                }
            }
            {
                void* r = fn_801A3E64(envlist);
                if (r == NULL) goto dispatch;
            }
        }
        /* fallthrough */

        default:
            goto dispatch;
        }
    }

call_dispatch:
dispatch:
    {
        void** ci = *(void***)((u8*)lbl_8036CCD0 + 0x14);
        ((void(*)(void*))ci[0x30/4])(p);
    }
}

/* =========================================================================
 * 0x801AA8BC | size: 0x2F8  -- Rigid skin display
 * ========================================================================= */
void fn_801AA8BC(void* pobj, void* vmtx, void* pmtx, u32 rendermode)
{
    /* stub - complex asm */
}

/* =========================================================================
 * 0x801AABB4 | size: 0x2F4  -- Rigid skin (indexed)
 * ========================================================================= */
void fn_801AABB4(void* pobj, void* vmtx, void* pmtx, u32 rendermode)
{
    /* stub - complex asm */
}

/* =========================================================================
 * 0x801AAEA8 | size: 0x3C8  -- Envelope skin display
 * ========================================================================= */
void fn_801AAEA8(void* pobj, void* vmtx, void* pmtx, u32 rendermode)
{
    /* stub - complex asm */
}

/* =========================================================================
 * 0x801AB270 | size: 0x2C8  -- Envelope direct display
 * ========================================================================= */
void fn_801AB270(void* pobj, void* vmtx, void* pmtx, u32 rendermode)
{
    /* stub - complex asm */
}

/* =========================================================================
 * 0x801AB538 | size: 0xC0  -- Set entry in 2-slot array
 * fn_801AB538(idx, ptr1, ptr2)
 * ========================================================================= */
void fn_801AB538(s32 idx, void* ptr1, void* ptr2)
{
    s32 i = idx;
    if (i >= 2) return;
    if (i < 0) goto do_store;
    if (i < 2) return;
do_store:
    *(void**)((u8*)lbl_80465678 + (u32)i * 8 + 0x0) = ptr1;
    *(void**)((u8*)lbl_80465678 + (u32)i * 8 + 0x4) = ptr2;
}

/* =========================================================================
 * 0x801AB5F8 | size: 0x44  -- Set entry in 2-slot array (void*, s32)
 * fn_801AB5F8(idx, ptr, val)
 * ========================================================================= */
void fn_801AB5F8(s32 idx, void* ptr, s32 val)
{
    s32 i = idx;
    if (i >= 2) return;
    if (i < 0) goto do_store;
    if (i < 2) return;
do_store:
    *(void**)((u8*)lbl_80465678 + (u32)i * 8 + 0x0) = ptr;
    *(s32*)  ((u8*)lbl_80465678 + (u32)i * 8 + 0x4) = val;
}

/* =========================================================================
 * 0x801AB63C | size: 0x40  -- Init all 2 entries in array
 * fn_801AB63C(ptr, val)
 * ========================================================================= */
void fn_801AB63C(void* ptr, s32 val)
{
    s32 i = 0;
    do {
        *(void**)((u8*)lbl_80465678 + (u32)i * 8 + 0x0) = ptr;
        *(s32*)  ((u8*)lbl_80465678 + (u32)i * 8 + 0x4) = val;
        i++;
    } while (i < 2);
}

/* =========================================================================
 * 0x801AB67C | size: 0x758  -- PObj shape animation display
 * ========================================================================= */
void fn_801AB67C(void* pobj)
{
    /* stub - complex asm */
}

/* =========================================================================
 * 0x801ABDD4 | size: 0x424
 * ========================================================================= */
void fn_801ABDD4(void)
{
}

/* =========================================================================
 * 0x801AC1F8 | size: 0x2C4
 * ========================================================================= */
void fn_801AC1F8(void)
{
}

/* =========================================================================
 * 0x801AC4BC | size: 0x460
 * ========================================================================= */
void get_shape_normal_xyz(void)
{
}

/* =========================================================================
 * 0x801AC91C | size: 0x460
 * ========================================================================= */
void fn_801AC91C(void)
{
}

/* =========================================================================
 * 0x801ACD7C | size: 0x30  -- Draw sync + clear display list state
 * ========================================================================= */
#pragma push
#pragma optimization_level 1
void fn_801ACD7C(void)
{
    fn_800B7D3C();
    lbl_8047B2FC = 0;
    lbl_8047B300 = NULL;
}
#pragma pop

/* =========================================================================
 * 0x801ACDAC | size: 0x298
 * ========================================================================= */
void fn_801ACDAC(void)
{
}

/* =========================================================================
 * 0x801AD044 | size: 0x1D0
 * ========================================================================= */
void fn_801AD044(void* pobj, void* desc)
{
}

/* =========================================================================
 * 0x801AD214 | size: 0x74  -- Walk pobj list, call vtable[0x30] + [0x34]
 * ========================================================================= */
#pragma push
#pragma peephole off
void HSD_PObjRemoveAll(HSD_PObj* pobj)
{
    void* next;
    void* cur = pobj;

    while (cur != NULL) {
        next = *(void**)((u8*)cur + 0x4);
        if (cur != NULL) {
            void** vtbl = *(void***)cur;
            ((void(*)(void*))vtbl[0x30 / 4])(cur);
            vtbl = *(void***)cur;
            ((void(*)(void*))vtbl[0x34 / 4])(cur);
        }
        cur = next;
    }
}
#pragma pop

/* =========================================================================
 * 0x801AD288 | size: 0xCC  -- PObj load from descriptor
 * ========================================================================= */
void* fn_801AD288(void* desc)
{
    void* d = desc;
    void* p;

    if (d == NULL) {
        return NULL;
    }

    if (*(u32*)d != 0) {
        void* info = fn_80193748((const char*)*(u32*)d);
        if (info != NULL) {
            p = fn_80193828(info);
            if (p == NULL) {
                __assert(lbl_8047DCB8, 0x2a9, lbl_8047DD10);
            }
            goto load;
        }
    }

    if (lbl_8047B2E8 != NULL) {
        p = lbl_8047B2E8;
    } else {
        p = fn_80193828(lbl_8036CCD0);
        if (p == NULL) {
            __assert(lbl_8047DCB8, 0x247, lbl_8047DD10);
        }
    }

load:
    {
        void** vtbl = *(void***)p;
        ((void(*)(void*, void*))vtbl[0x44 / 4])(p, d);
    }
    return p;
}

/* =========================================================================
 * 0x801AD354 | size: 0x2C8
 * ========================================================================= */
void fn_801AD354(void* pobj_ptr, void* desc)
{
}

/* =========================================================================
 * 0x801AD61C | size: 0x5C  -- Walk pobj list, call reqAnim
 * HSD_PObjAnimAll(pobj)
 * ========================================================================= */
void HSD_PObjAnimAll(HSD_PObj* pobj)
{
    HSD_PObj* cur;

    if (pobj == NULL) {
        return;
    }

    cur = pobj;
    while (cur != NULL) {
        if (cur != NULL) {
            void** vtbl = *(void***)cur;
            fn_801C27F4(*(void**)((u8*)cur + 0x18), cur, vtbl[0x48 / 4]);
        }
        cur = cur->next;
    }
}

/* =========================================================================
 * 0x801AD678 | size: 0x4C  -- Set shape blend weight
 * PObjUpdateFunc(pobj, idx, f32* weight_ptr)
 * ========================================================================= */
void PObjUpdateFunc(void* pobj, s32 idx, f32* weight_ptr)
{
    void* p = pobj;

    if (p == NULL) return;

    {
        u16 flags = *(u16*)((u8*)p + 0xc);
        if ((flags & 0x3000) != 0x1000) return;
    }

    {
        void* shapeset = *(void**)((u8*)p + 0x14);
        u16 ssflags = *(u16*)shapeset;

        if (ssflags & 0x2) {
            f32* arr = *(f32**)((u8*)shapeset + 0x1c);
            arr[idx - 2] = *weight_ptr;
        } else {
            *(f32*)((u8*)shapeset + 0x1c) = *weight_ptr;
        }
    }
}

/* =========================================================================
 * 0x801AD6C4 | size: 0x74  -- Request PObj animation by flags
 * ========================================================================= */
#pragma push
#pragma optimization_level 1
void HSD_PObjReqAnimAllByFlags(f32 val, void* pobj, u32 flags)
{
    void* cur;

    if (pobj == NULL) {
        return;
    }

    cur = pobj;
    while (cur != NULL) {
        if (cur != NULL) {
            if (flags & 0x8) {
                fn_801C29C4(val, *(void**)((u8*)cur + 0x18));
            }
        }
        cur = *(void**)((u8*)cur + 0x4);
    }
}
#pragma pop

/* =========================================================================
 * 0x801AD738 | size: 0x94
 * ========================================================================= */
void fn_801AD738(void* pobj, void* animlist)
{
    void* p = pobj;
    void* al = animlist;

    if (p == NULL || al == NULL) return;

    while (p != NULL) {
        if (*(void**)((u8*)p + 0x18) != NULL) {
            fn_801C25E4(*(void**)((u8*)p + 0x18));
        }
        *(void**)((u8*)p + 0x18) = fn_801C2670(*(void**)((u8*)al + 0x4));
        if (al == NULL) {
            al = NULL;
        } else {
            al = *(void**)al;
        }
        p = *(void**)((u8*)p + 0x4);
    }
}

/* =========================================================================
 * 0x801AD7CC | size: 0x2E0  -- Quaternion slerp
 * ========================================================================= */
s32 fn_801AD7CC(f32* q1, f32* q2, f32 t, f32* out)
{
    return 1;
}

/* =========================================================================
 * 0x801ADAAC | size: 0x15C  -- Quaternion multiply / normalize
 * ========================================================================= */
void fn_801ADAAC(f32* q1, f32* q2, f32* out)
{
}

/* =========================================================================
 * 0x801ADC08 | size: 0x34  -- Forget RNG memory state
 * ========================================================================= */
void _HSD_RandForgetMemory(void)
{
    s32 r = fn_801A6990(lbl_80478C94);
    if (r != 0) {
        lbl_80478C94 = &lbl_80478C90;
    }
}

/* =========================================================================
 * 0x801ADC3C | size: 0x40  -- LCG next, scaled return
 * fn_801ADC3C(s32 scale) -> s32
 * ========================================================================= */
s32 fn_801ADC3C(s32 scale)
{
    u32* state = (u32*)lbl_80478C94;
    *state = *state * 0x343fd + 0x269EC3;
    return (s32)((s32)(scale * (s32)(*state >> 16)) >> 16);
}

/* =========================================================================
 * 0x801ADC7C | size: 0x5C  -- LCG next, float return
 * fn_801ADC7C() -> f32
 * ========================================================================= */
f32 fn_801ADC7C(void)
{
    u32* state = (u32*)lbl_80478C94;
    *state = *state * 0x343fd + 0x269EC3;
    return (f32)(*state >> 16) / lbl_8047DD40;
}

/* =========================================================================
 * 0x801ADCD8 | size: 0x34  -- LCG next, u16 return
 * fn_801ADCD8() -> u32
 * ========================================================================= */
u32 fn_801ADCD8(void)
{
    u32* state = (u32*)lbl_80478C94;
    *state = *state * 0x343fd + 0x269EC3;
    return *state >> 16;
}

/* =========================================================================
 * 0x801ADD0C | size: 0x3C  -- Deactivate texture anim state
 * ========================================================================= */
#pragma push
#pragma optimization_level 1
void fn_801ADD0C(void)
{
    s32 r = fn_801A6990(lbl_8047B308);
    if (r != 0) {
        lbl_8047B308 = NULL;
        lbl_8047B30C = 0;
    }
}
#pragma pop

/* =========================================================================
 * 0x801ADD48 | size: 0x108
 * ========================================================================= */
void fn_801ADD48(void* out, void* desc)
{
}

/* =========================================================================
 * 0x801ADE50 | size: 0x104
 * ========================================================================= */
void fn_801ADE50(void* out, void* desc)
{
}

/* =========================================================================
 * 0x801ADF54 | size: 0xAC
 * ========================================================================= */
void* fn_801ADF54(void* desc)
{
    return NULL;
}
