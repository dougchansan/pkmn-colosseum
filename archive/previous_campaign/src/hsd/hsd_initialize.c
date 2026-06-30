/**
 * @file hsd_initialize.c
 * @brief HSD initialization and render pass management.
 *
 * Colosseum address: 0x8019C690 (HSD_InitAssert1)
 * The initialize module sets up the HSD subsystem: GX FIFO,
 * framebuffers, heaps, and pixel format validation.
 *
 * Adapted from doldecomp/melee src/sysdolphin/baselib/initialize.c
 */

#include "hsd/hsd_initialize.h"
#include "hsd/hsd_debug.h"

static HSD_RenderPass current_render_pass = HSD_RP_SCREEN;
static s32 current_heap = -1;

/* ========================================================================= */
/*  Heap management                                                          */
/* ========================================================================= */

s32 HSD_GetHeap(void)
{
    return current_heap;
}

void HSD_SetHeap(s32 handle)
{
    current_heap = handle;
}

/* ========================================================================= */
/*  Render pass                                                              */
/* ========================================================================= */

HSD_RenderPass HSD_GetCurrentRenderPass(void)
{
    return current_render_pass;
}

void HSD_StartRender(HSD_RenderPass pass)
{
    current_render_pass = pass;
}

/* ===================================================================
 * Generated: 0 pattern-matched + 10 stubs
 * Range: 0x8019C690 - 0x8019CE50
 * =================================================================== */

/* 0x8019C6EC | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern s32 lbl_8047B290;
#if 0
asm void fn_8019C6EC(void) {
#include "src/hsd/hsd_initialize_fn_8019C6EC.inc"
}
#else
void fn_8019C6EC(u32 flags) {
    lbl_8047B290 |= flags;
}
#endif
#pragma pop

/* 0x8019C6FC | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern volatile s32 lbl_8047B294;
#if 0
asm void fn_8019C6FC(void) {
#include "src/hsd/hsd_initialize_fn_8019C6FC.inc"
}
#else
void fn_8019C6FC(void) {
    if (lbl_8047B294 == HSD_RP_OFFSCREEN) {
        return;
    }
}
#endif
#pragma pop

/* 0x8019C708 | 0xA8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_800BCEF4(u32 a, u32 b);
extern void fn_800BD07C(u32 a, u32 b);
extern void fn_800B856C(void);
extern void fn_800BB29C(void);
extern u8 lbl_80466BC0[];
extern volatile s32 lbl_8047B294;
extern u32 lbl_80478C78;
extern u32 lbl_8047B27C;
#if 1
asm void fn_8019C708(void) {
#include "src/hsd/hsd_initialize_fn_8019C708.inc"
}
#else
#pragma optimization_level 4
void fn_8019C708(u32 arg) {
    u8* p;
    u16 a;
    u16 b;

    p = lbl_80466BC0;
    lbl_8047B294 = arg;
    if (p[0x19] != 0) {
        fn_800BCEF4(2, lbl_80478C78);
    } else {
        fn_800BCEF4(lbl_8047B27C, 0);
    }
    a = *(u16*)(p + 0x8);
    b = *(u16*)(p + 0x10);
    fn_800BD07C(p[0x18], (u32)(a - b) >> 31);
    if (lbl_8047B290 != 0) {
        if (lbl_8047B290 & 1) {
            fn_800B856C();
        }
        if (lbl_8047B290 & 2) {
            fn_800BB29C();
        }
        lbl_8047B290 = 0;
    }
}
#endif
#pragma pop

/* 0x8019C7B0 | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern volatile s32 lbl_8047B294;
#if 0
asm void fn_8019C7B0(void) {
#include "src/hsd/hsd_initialize_fn_8019C7B0.inc"
}
#else
u32 fn_8019C7B0(void) {
    return lbl_8047B294;
}
#endif
#pragma pop

/* 0x8019C7B8 | 0x28 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u32 lbl_8047B268;
extern u32 lbl_8047B26C;
#if 0
asm void _HSD_MemCheckOwnDefaultCB(void) {
#include "src/hsd/hsd_initialize_fn_8019C7B8.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 _HSD_MemCheckOwnDefaultCB(u32 addr) {
    s32 result = 0;
    if (lbl_8047B268 <= addr && addr < lbl_8047B26C) {
        result = 1;
    }
    return result;
}
#endif
#pragma pop

/* 0x8019C7E0 | 0x74 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_8009AC3C(u32 xfb);
extern u32 fn_8009ABD0(u32 a, u32 b);
extern void fn_8009AB50(u32 xfb);
extern u32 lbl_80478C70;
extern u32 lbl_8047B270;
extern u32 lbl_8047B274;
extern u32 lbl_8047B268;
extern u32 lbl_8047B26C;
extern void fn_8009AC3C(u32 xfb);
extern u32 fn_8009ABD0(u32 a, u32 b);
extern void fn_8009AB50(u32 xfb);
extern u32 lbl_80478C70;
extern u32 lbl_8047B270;
extern u32 lbl_8047B274;
extern u32 lbl_8047B268;
extern u32 lbl_8047B26C;

void fn_8019C7E0(void) {
    fn_8009AC3C(lbl_80478C70);
    if (lbl_8047B270 != 0 && lbl_8047B274 != 0) {
        lbl_8047B268 = lbl_8047B270;
        lbl_8047B26C = lbl_8047B274;
        lbl_8047B270 = 0;
        lbl_8047B274 = 0;
    }
    lbl_80478C70 = fn_8009ABD0(lbl_8047B268, lbl_8047B26C);
    fn_8009AB50(lbl_80478C70);
}

/* 0x8019C854 | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void __assert();
extern void fn_8009AC50(void* xfb);
extern s32 lbl_8047B288;
extern u8 lbl_802749E4[];
extern u8 lbl_80274A28[];
extern u32 lbl_80478C70;
#if 0
asm void _HSD_MemGetRemainDefaultCB(void) {
#include "src/hsd/hsd_initialize__HSD_MemGetRemainDefaultCB.inc"
}
#else
#pragma optimization_level 4
void _HSD_MemGetRemainDefaultCB(void) {
    if (lbl_8047B288 != 0) {
        __assert(lbl_802749E4, 0x1b6, lbl_80274A28);
    }
    fn_8009AC50((void*)lbl_80478C70);
}
#endif
#pragma pop

/* 0x8019C89C | 0x58 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_8009AAD4(void* xfb, void* param);
extern s32 lbl_8047B288;
extern u32 lbl_80478C70;
#if 0
asm void _HSD_MemFreeDefaultCB(void) {
#include "src/hsd/hsd_initialize__HSD_MemFreeDefaultCB.inc"
}
#else
#pragma optimization_level 4
void _HSD_MemFreeDefaultCB(void* param) {
    if (lbl_8047B288 != 0) {
        __assert(lbl_802749E4, 0x1b6, lbl_80274A28);
    }
    fn_8009AAD4((void*)lbl_80478C70, param);
}
#endif
#pragma pop

/* 0x8019C8F4 | 0x84 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void* fn_8009A9D8(void* xfb, void* buffer);
extern s32 lbl_8047B288;
extern u32 lbl_80478C70;
extern u32 lbl_8047DB0C;
#if 0
asm void _HSD_MemAllocDefaultCB(void) {
#include "src/hsd/hsd_initialize__HSD_MemAllocDefaultCB.inc"
}
#else
#pragma optimization_level 4
void* _HSD_MemAllocDefaultCB(void* buffer) {
    void* result;

    if (buffer == NULL) {
        return NULL;
    }
    if (lbl_8047B288 != 0) {
        __assert(lbl_802749E4, 0x1b6, lbl_80274A28);
    }
    result = fn_8009A9D8((void*)lbl_80478C70, buffer);
    if (result == NULL) {
        __assert(lbl_802749E4, 0x17a, &lbl_8047DB0C);
    }
    return result;
}
#endif
#pragma pop

/* 0x8019C978 | 0x1F8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_8009F3D4(void);
extern void _HSD_MemSetCallbacks(void);
extern void HSD_ObjSetHeap(void);
extern void OSGetArenaLo();
extern void OSGetArenaHi();
extern void OSSetArenaLo();
extern u8 lbl_80465568[];
extern s32 lbl_8047B288;
extern u32 lbl_8047B270;
extern u32 lbl_8047B274;
extern u32 lbl_8047B268;
extern u32 lbl_8047B26C;
extern u32 lbl_80478C70;
extern u32 lbl_8047B284;
/* Preserved asm include predates the recovered HSD memory callback names. */
#define fn_8019C7B8 _HSD_MemCheckOwnDefaultCB
#define fn_8019C854 _HSD_MemGetRemainDefaultCB
#define fn_8019C89C _HSD_MemFreeDefaultCB
#define fn_8019C8F4 _HSD_MemAllocDefaultCB
#define fn_801A69C0 _HSD_MemSetCallbacks
#define fn_801AA538 HSD_ObjSetHeap
#if 1
asm void fn_8019C978(void) {
#include "src/hsd/hsd_initialize_fn_8019C978.inc"
}
#else
void fn_8019C978(void) {
    /* TODO: match -- 504 bytes at 0x8019C978 */
}
#endif
#undef fn_8019C7B8
#undef fn_8019C854
#undef fn_8019C89C
#undef fn_8019C8F4
#undef fn_801A69C0
#undef fn_801AA538
#pragma pop
