/**
 * @file hsd_mobj.c
 * @brief HSD MObj - Material object implementation.
 *
 * Colosseum address: 0x801A6A34 (HSD_MObjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/mobj.c
 */

#include "hsd/hsd_mobj.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_memory.h"
#include "hsd/hsd_tobj.h"

extern void* memset(void* dst, int val, u32 size);

static void MObjInfoInit(void);

#ifdef PCPORT
HSD_TExp* PCPort_MObjMakeTExp(HSD_MObj* mobj, HSD_TObj* tobj_top,
                              HSD_TExp** list);
void PCPort_TObjMakeTExp(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                         HSD_TExp** c, HSD_TExp** a, HSD_TExp** list);
HSD_TExp* fn_801A7128(HSD_MObj* mobj, HSD_TObj* tobj_top, HSD_TExp** list);
#endif

HSD_MObjInfo hsdMObj = { MObjInfoInit };

static HSD_ClassInfo* default_class = NULL;

/* ========================================================================= */
/*  Flag accessors                                                           */
/* ========================================================================= */

void HSD_MObjSetFlags_Early(HSD_MObj* mobj, u32 flags)
{
    if (mobj != NULL) {
        mobj->rendermode |= flags;
    }
}

void HSD_MObjClearFlags_Early(HSD_MObj* mobj, u32 flags)
{
    if (mobj != NULL) {
        mobj->rendermode &= ~flags;
    }
}

/* ========================================================================= */
/*  Animation                                                                */
/* ========================================================================= */

void HSD_MObjAddAnim(HSD_MObj* mobj, HSD_MatAnim* matanim)
{
    if (mobj == NULL || matanim == NULL) {
        return;
    }
    if (mobj->aobj != NULL) {
        HSD_AObjRemove(mobj->aobj);
    }
    mobj->aobj = HSD_AObjLoadDesc(matanim->aobjdesc);
    HSD_TObjAddAnimAll(mobj->tobj, matanim->texanim);
}

void HSD_MObjReqAnim(HSD_MObj* mobj, f32 startframe)
{
    if (mobj != NULL) {
        HSD_AObjReqAnim(mobj->aobj, startframe);
        HSD_TObjReqAnimAll(mobj->tobj, startframe);
    }
}

void HSD_MObjAnim(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        HSD_TObjAnimAll(mobj->tobj);
    }
}

/* ========================================================================= */
/*  TObj accessor                                                            */
/* ========================================================================= */

HSD_TObj* HSD_MObjGetTObj(HSD_MObj* mobj)
{
    if (mobj == NULL) {
        return NULL;
    }
    return mobj->tobj;
}

/* ========================================================================= */
/*  Alpha                                                                    */
/* ========================================================================= */

void HSD_MObjSetAlpha(HSD_MObj* mobj, f32 alpha)
{
    HSD_ASSERT(0, mobj);
    if (mobj->mat != NULL) {
        mobj->mat->alpha = alpha;
    }
}

/* ========================================================================= */
/*  Load                                                                     */
/* ========================================================================= */

static int MObjLoad(HSD_MObj* mobj, HSD_MObjDesc* desc)
{
    mobj->rendermode = desc->rendermode;
    if (mobj->tobj != NULL) {
        HSD_TObjRemoveAll(mobj->tobj);
    }
    mobj->tobj = HSD_TObjLoadDesc(desc->texdesc);
    mobj->mat = desc->mat;
    mobj->pe = desc->pedesc;
    return 0;
}

HSD_MObj* HSD_MObjLoadDesc(HSD_MObjDesc* mobjdesc)
{
    HSD_MObj* mobj;
    HSD_ClassInfo* info;

    if (mobjdesc == NULL) {
        return NULL;
    }

    if (mobjdesc->class_name == NULL ||
        !(info = hsdSearchClassInfo(mobjdesc->class_name)))
    {
        mobj = HSD_MObjAlloc();
    } else {
        mobj = hsdNew(info);
        HSD_ASSERT(0, mobj);
    }

    HSD_MOBJ_METHOD(mobj)->load(mobj, mobjdesc);
    return mobj;
}

/* ========================================================================= */
/*  Remove / Alloc                                                           */
/* ========================================================================= */

void HSD_MObjRemove(HSD_MObj* mobj)
{
    if (mobj != NULL) {
        HSD_CLASS_METHOD(mobj)->release((HSD_Class*) mobj);
        HSD_CLASS_METHOD(mobj)->destroy((HSD_Class*) mobj);
    }
}

HSD_MObj* HSD_MObjAlloc(void)
{
    HSD_MObj* mobj;
    mobj = (HSD_MObj*) hsdNew(
        default_class ? default_class : (HSD_ClassInfo*) &hsdMObj);
    HSD_ASSERT(0, mobj);
    return mobj;
}

HSD_Material* HSD_MaterialAlloc(void)
{
    HSD_Material* mat = (HSD_Material*) HSD_MemAlloc(sizeof(HSD_Material));
    if (mat != NULL) {
        memset(mat, 0, sizeof(HSD_Material));
    }
    return mat;
}

/* ========================================================================= */
/*  Class lifecycle                                                          */
/* ========================================================================= */

static void MObjRelease(HSD_Class* o)
{
    HSD_MObj* mobj = (HSD_MObj*) o;
    HSD_TObjRemoveAll(mobj->tobj);
    HSD_AObjRemove(mobj->aobj);
    HSD_PARENT_INFO(&hsdMObj)->release(o);
}

static void MObjAmnesia(HSD_ClassInfo* info)
{
    if (info == HSD_CLASS_INFO(default_class)) {
        default_class = NULL;
    }
    HSD_PARENT_INFO(&hsdMObj)->amnesia(info);
}

static void MObjInfoInit(void)
{
    hsdInitClassInfo((HSD_ClassInfo*) &hsdMObj, &hsdClass,
                     "sysdolphin_base_library", "hsd_mobj",
                     sizeof(HSD_MObjInfo), sizeof(HSD_MObj));
    ((HSD_ClassInfo*) &hsdMObj)->release = MObjRelease;
    ((HSD_ClassInfo*) &hsdMObj)->amnesia = MObjAmnesia;
    hsdMObj.load = MObjLoad;
#ifdef PCPORT
    hsdMObj.make_texp = fn_801A7128;
#endif
}

/* 0x801A6B24 | 0x68 */
extern u32 lbl_8047B2D0;
extern u8 lbl_8036CB30[];
extern u32 lbl_8047B2D8;
extern u32 lbl_8047B2DC;
#if 0
asm void fn_801A6B24(void) {
#include "src/hsd/hsd_mobj_fn_801A6B24.inc"
}
#else
#pragma push
#pragma optimization_level 1
void fn_801A6B24(void* info) {
    if (info == (void*)lbl_8047B2D0) {
        lbl_8047B2D0 = 0;
    }
    if (info == (void*)lbl_8036CB30) {
        lbl_8047B2D8 = 0;
        lbl_8047B2DC = 0;
    }
    ((HSD_ClassInfo*)lbl_8036CB30)->head.parent->amnesia(info);
}
#pragma pop
#endif

/* 0x801A6B8C | 0xA8 */
extern void fn_801C25E4(HSD_AObj* aobj);
extern void fn_80193AF0(void* obj, s32 size);
extern void fn_801BBE60(HSD_TObj* tobj);
extern void fn_801B42C0(void* node);
extern void fn_801B7178(HSD_TObj* tobj, u32 map_id, u32 coord_id);
#if 0
asm void fn_801A6B8C(void) {
#include "src/hsd/hsd_mobj_fn_801A6B8C.inc"
}
#else
#pragma push
#pragma optimization_level 1
void fn_801A6B8C(HSD_MObj* mobj) {
    fn_801C25E4(mobj->aobj);
    fn_80193AF0(mobj->mat, 0x14);
    fn_801BBE60(mobj->tobj);
    if (mobj->tevdesc != NULL) {
        fn_801B42C0(mobj->tevdesc);
    }
    if (mobj->texp != NULL) {
        fn_801B7178((HSD_TObj*)mobj->texp, 7, 1);
    }
    if (mobj->pe != NULL) {
        fn_80193AF0(mobj->pe, 0xc);
    }
    HSD_PARENT_INFO((HSD_MObjInfo*)lbl_8036CB30)->release((HSD_Class*)mobj);
}
#pragma pop
#endif

/* 0x801A6C34 | 0x70 */
#if 0
asm void fn_801A6C34(void) {
#include "src/hsd/hsd_mobj_fn_801A6C34.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_801A6C34(void* obj) {
    u32 node;
    volatile u32* pp;

    if (obj != NULL) {
        pp = &lbl_8047B2DC;
        while (*pp != 0) {
            if ((void*)*pp == obj) {
                *pp = *(u32*)((u8*)obj + 8);
                *(u32*)((u8*)obj + 8) = 0;
                return;
            }
            node = *pp;
            pp = (volatile u32*)((u8*)node + 8);
        }
        return;
    }
    while (lbl_8047B2DC != 0) {
        u32 b, a;
        a = *(volatile u32*)&lbl_8047B2DC;
        b = *(volatile u32*)&lbl_8047B2DC;
        a = *(u32*)((u8*)a + 8);
        *(u32*)((u8*)b + 8) = 0;
        lbl_8047B2DC = a;
    }
}
#pragma pop
#endif

/* 0x801A6CA4 | 0x64 */
extern void __assert();
extern u32 lbl_8047DC18;
extern u32 lbl_8047DC20;
#if 0
asm void fn_801A6CA4(void) {
#include "src/hsd/hsd_mobj_fn_801A6CA4.inc"
}
#else
void fn_801A6CA4(void* obj) {
    void* node;

    if (obj == NULL) {
        __assert(&lbl_8047DC18, 0x495, &lbl_8047DC20);
    }
    node = (void*)lbl_8047B2DC;
    while (node != NULL) {
        if (node == obj) {
            return;
        }
        node = *(void**)((u8*)node + 8);
    }
    *(u32*)((u8*)obj + 8) = *(volatile u32*)&lbl_8047B2DC;
    lbl_8047B2DC = (u32)obj;
}
#endif

/* 0x801A6D08 | 0x54 */
#if 0
asm void fn_801A6D08(void) {
#include "src/hsd/hsd_mobj_fn_801A6D08.inc"
}
#else
#pragma push
#pragma optimization_level 1
void fn_801A6D08(HSD_MObj* mobj) {
    HSD_ClassInfo* info;
    if (mobj != NULL) {
        info = HSD_CLASS_METHOD(mobj);
        info->release((HSD_Class*) mobj);
        info = HSD_CLASS_METHOD(mobj);
        info->destroy((HSD_Class*) mobj);
    }
}
#pragma pop
#endif

/* 0x801A6D5C | 0x44 */
extern void fn_801BBE3C(void* a, void* b);
#if 0
asm void HSD_MObjAddTObjNext(void) {
#include "src/hsd/hsd_mobj_fn_801A6D5C.inc"
}
#else
void HSD_MObjAddTObjNext(void* a, void* b, void* c) {
    if (a == NULL || b == NULL || c == NULL) {
        return;
    }
    fn_801BBE3C(b, c);
}
#endif

/* 0x801A6DA0 | 0x24 */
#if 0
asm void fn_801A6DA0(void) {
#include "src/hsd/hsd_mobj_fn_801A6DA0.inc"
}
#else
void fn_801A6DA0(u32* a, u32* b) {
    if (a == NULL || b == NULL) {
        return;
    }
    *(u32**)((u8*)b + 8) = *(u32**)((u8*)a + 8);
    *(u32**)((u8*)a + 8) = (u32*)b;
}
#endif

/* 0x801A6DC4 | 0x18 */
#if 0
asm void fn_801A6DC4(void) {
#include "src/hsd/hsd_mobj_fn_801A6DC4.inc"
}
#else
u32* fn_801A6DC4(u32* node) {
    if (node == NULL) {
        return NULL;
    }
    return *(u32**)((u8*)node + 8);
}
#endif

/* 0x801A6DDC | 0x24 */
#pragma push
#pragma optimization_level 1
#pragma optimizewithasm off
#if 0
asm void fn_801A6DDC(void) {
#include "src/hsd/hsd_mobj_fn_801A6DDC.inc"
}
#else
void fn_801A6DDC(HSD_MObj* mobj, f32 val) {
    if (mobj == NULL) {
        return;
    }
    if (mobj->mat != NULL) {
        mobj->mat->alpha = val;
    }
}
#endif
#pragma pop

/* 0x801A6E00 | 0x24 */
#if 0
asm void fn_801A6E00(void) {
#include "src/hsd/hsd_mobj_fn_801A6E00.inc"
}
#else
void fn_801A6E00(void) {
    HSD_TObjSetup(NULL);
}
#endif

/* 0x801A6E24 | 0x154 */
extern void fn_801B28B8(HSD_Material* mat, f32 shininess);
extern void fn_801B28C8(u32* ambient, u32* diffuse, u32* specular, f32 alpha);
extern void fn_801B294C(u32 flags, HSD_PEDesc* pe);
extern void fn_801B3884(void);
extern void fn_801BDA58(void* tobj);
#if 1
asm void HSD_MObjSetup(HSD_MObj* mobj, u32 rendermode) {
#include "src/hsd/hsd_mobj_fn_801A6E24.inc"
}
#else
void HSD_MObjSetup(HSD_MObj* mobj, u32 rendermode_arg) {
    u32 rendermode;
    u32* pp;
    u32 tobj_slot;
    u32 amb_copy, diff_copy, spec_copy;

    fn_801B3884();
    rendermode = mobj->rendermode;
    spec_copy = mobj->mat->specular;
    diff_copy = mobj->mat->diffuse;
    amb_copy = mobj->mat->ambient;
    fn_801B28C8(&amb_copy, &diff_copy, &spec_copy, mobj->mat->alpha);
    if (rendermode & 0x8) {
        fn_801B28B8(mobj->mat, mobj->mat->shininess);
    }
    tobj_slot = (u32)mobj->tobj;
    pp = NULL;
    if ((rendermode & 0x04000000) && lbl_8047B2DC != 0) {
        pp = &tobj_slot;
        while (*pp != 0) {
            pp = (u32*)(*pp + 8);
        }
        *pp = lbl_8047B2DC;
    }
    if ((rendermode & 0x1000) && lbl_8047B2D8 != 0) {
        u32* d8 = (u32*)lbl_8047B2D8;
        if (*(u32*)((u8*)d8 + 0x58) != 0) {
            *(u32*)((u8*)d8 + 0x8) = tobj_slot;
            tobj_slot = lbl_8047B2D8;
        }
    }
    HSD_TObjSetup((HSD_TObj*)tobj_slot);
    fn_801BDA58((void*)tobj_slot);
    ((void (*)(HSD_MObj*, u32, u32))(HSD_MOBJ_METHOD(mobj)->make_texp))(mobj, tobj_slot, rendermode);
    fn_801B294C(rendermode, mobj->pe);
    if (pp != NULL) {
        *pp = 0;
    }
}
#endif

/* 0x801A6F78 | 0x78 */
extern void fn_801B45A4(void* a, void* b);
extern void fn_801BD8D0(void* a, void* b);
extern u8 lbl_80274E5C[];
#if 0
asm void MObjSetupTev(void) {
#include "src/hsd/hsd_mobj_fn_801A6F78.inc"
}
#else
void MObjSetupTev(void* obj, void* a, void* b) {
    if (*(u32*)((u8*)obj + 0x18) == 0) {
        __assert(&lbl_8047DC18, 0x31e, lbl_80274E5C);
    }
    fn_801B45A4(*(void**)((u8*)obj + 0x18), *(void**)((u8*)obj + 0x1c));
    fn_801BD8D0(a, b);
}
#endif

/* 0x801A7128 | 0x9FC */
extern void fn_801B5E40(void);
extern void fn_801B5F08(void);
extern void fn_801B64EC(void);
extern void fn_801B6CD8(void);
extern void fn_801B6E74(void);
extern void fn_801B6F5C(void);
extern void fn_801B707C(void);
extern void fn_801B7C60(void);
extern u32 lbl_80478C88;
extern u32 lbl_8047DC38;
#if 1
asm void fn_801A7128(void) {
#include "src/hsd/hsd_mobj_fn_801A7128.inc"
}
#else
void fn_801A7128(void) {}
#endif

/* 0x801A7B24 | 0x1D8 */
extern HSD_MObj* fn_80193748(void* class_name);
extern HSD_MObj* fn_80193828(HSD_ClassInfo* info);
extern void fn_801B4300(void* a, void* b);
extern void fn_801BC33C(void* tobj);
extern u32 lbl_8047DC30;
#if 1
asm void fn_801A7B24(void) {
#include "src/hsd/hsd_mobj_fn_801A7B24.inc"
}
#else
HSD_MObj* fn_801A7B24(void* desc) {
    HSD_MObj* mobj;
    u32 tobj_slot;
    typedef void (*load3arg_t)(HSD_MObj*, u32, HSD_TExp**);

    if (desc == NULL) {
        return NULL;
    }
    if (*(u32*)desc != 0) {
        mobj = fn_80193748(*(void**)desc);
        if (mobj != NULL) {
            goto found;
        }
    }
    if (lbl_8047B2D0 != 0) {
        mobj = fn_80193828((HSD_ClassInfo*)lbl_8047B2D0);
    } else {
        mobj = fn_80193828((HSD_ClassInfo*)lbl_8036CB30);
    }
    if (mobj == NULL) {
        __assert(&lbl_8047DC18, 0x44a, &lbl_8047DC30);
    }
    goto call_setup;
found:
    mobj = fn_80193828((HSD_ClassInfo*)mobj);
    if (mobj == NULL) {
        __assert(&lbl_8047DC18, 0x175, &lbl_8047DC30);
    }
call_setup:
    HSD_MOBJ_METHOD(mobj)->setup(mobj, (u32)desc);
    desc = NULL;
    if (mobj == NULL) {
        return NULL;
    }
    if (mobj->tevdesc != NULL) {
        fn_801B42C0(mobj->tevdesc);
        mobj->tevdesc = NULL;
    }
    if (mobj->texp != NULL) {
        fn_801B42C0(mobj->texp);
        mobj->texp = NULL;
    }
    tobj_slot = (u32)mobj->tobj;
    if ((mobj->rendermode & 0x04000000) && lbl_8047B2DC != 0) {
        desc = (void*)&tobj_slot;
        while (*(u32*)desc != 0) {
            desc = (void*)(*(u32*)desc + 8);
        }
        *(u32*)desc = lbl_8047B2DC;
    }
    if ((mobj->rendermode & 0x1000) && lbl_8047B2D8 != 0) {
        u32* d8 = (u32*)lbl_8047B2D8;
        if (*(u32*)((u8*)d8 + 0x58) != 0) {
            *(u32*)((u8*)d8 + 0x8) = tobj_slot;
            tobj_slot = lbl_8047B2D8;
        }
    }
    fn_801BC33C((void*)tobj_slot);
    ((load3arg_t)(HSD_MOBJ_METHOD(mobj)->load))(mobj, tobj_slot, &mobj->texp);
    fn_801B4300(&mobj->tevdesc, &mobj->texp);
    if (desc != NULL) {
        *(u32*)desc = 0;
    }
    return mobj;
}
#endif

/* 0x801A7D58 | 0xE4 */
extern u32 fn_801BE4CC(u32 hsd_format);
extern void* fn_80193B10(s32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern u32 lbl_8047DC28;
extern f32 lbl_8047DC2C;
#if 0
asm void fn_801A7D58(void) {
#include "src/hsd/hsd_mobj_fn_801A7D58.inc"
}
#else
#pragma push
#pragma optimization_level 1
s32 fn_801A7D58(void* arg0, void* arg1) {
    HSD_Material* mat;
    u32 converted_format;
    u32 mat_addr;

    *(u32*)((u8*)arg0 + 4) = *(u32*)((u8*)arg1 + 4);
    converted_format = fn_801BE4CC(*(u32*)((u8*)arg1 + 8));
    *(u32*)((u8*)arg0 + 8) = converted_format;
    mat = (HSD_Material*)fn_80193B10(0x14);
    if (mat == NULL) {
        __assert(&lbl_8047DC18, 0x466, &lbl_8047DC28);
    }
    memset(mat, 0, 0x14);
    mat->alpha = lbl_8047DC2C;
    mat_addr = (u32)mat;
    *(u32*)((u8*)arg0 + 0xc) = mat_addr;
    memcpy(*(HSD_Material**)((u8*)arg0 + 0xc),
           *(HSD_Material**)((u8*)arg1 + 0xc), 0x14);
    *(u32*)((u8*)arg0 + 4) |= 0x1000u;
    if (*(u32*)((u8*)arg1 + 0x14) != 0) {
        *(void**)((u8*)arg0 + 0x10) = fn_80193B10(0xc);
        memcpy(*(void**)((u8*)arg0 + 0x10),
               *(void**)((u8*)arg1 + 0x14), 0xc);
    }
    *(u32*)((u8*)arg0 + 0x14) = 0;
    return 0;
}
#pragma pop
#endif

/* 0x801A7E3C | 0x48 */
extern void fn_801C27F4(HSD_AObj* aobj, HSD_MObj* mobj,
                        void (*setup_tev)(HSD_MObj*, HSD_TObj*, u32));
extern void fn_801BE800(HSD_TObj* tobj);
#if 0
asm void fn_801A7E3C(void) {
#include "src/hsd/hsd_mobj_fn_801A7E3C.inc"
}
#else
void fn_801A7E3C(HSD_MObj* mobj) {
    if (mobj == NULL) {
        return;
    }
    fn_801C27F4(mobj->aobj, mobj, HSD_MOBJ_METHOD(mobj)->setup_tev);
    fn_801BE800(mobj->tobj);
}
#endif

/* 0x801A7E84 | 0x4D0 */
extern void jumptable_8036CB84();
extern u32 lbl_8047DC40;
extern u32 lbl_8047DC44;
#if 1
asm void MObjUpdateFunc(void) {
#include "src/hsd/hsd_mobj_fn_801A7E84.inc"
}
#else
void MObjUpdateFunc(void) {}
#endif
