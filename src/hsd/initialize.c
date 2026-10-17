/**
 * @file initialize.c
 * @brief HAL sysdolphin initialize.c: library start-up (arena, XFBs, GX
 *        FIFO, heap and memory callbacks, VI, lights, object pools), init
 *        parameters and render-pass state, 0x8019C3C4-0x8019CE50.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/initialize.c) and checked against Colosseum's
 * retail code, which is the newer (>= 1.3.0.0) sysdolphin:
 * - XFBs and the GX FIFO come from OSAllocFromArenaLo, and DVDInit is
 *   called from HSD_DVDInit.
 * - The heap is no longer an OS heap set up from HSD_INIT_HEAP_MAX_NUM /
 *   HSD_INIT_AUDIO_HEAP_SIZE (both now report "obsolete since 1.3.0.0").
 *   HSD_OSInit installs memory.c's allocator callbacks: either the game's
 *   own (HSD_INIT_MEMORY_CALLBACKS) or the _HSD_Mem*DefaultCB set here,
 *   which run an OS heap over the arena, an HSD_INIT_ARENA range or the
 *   first HSD_INIT_HEAP_SIZE bytes of the arena.
 * - HSD_StartRender also applies pending cache invalidations.
 *
 * The library is built with deferred inlining, so functions are listed in
 * HAL's order and MWCC emits them in reverse (the retail address order).
 * Functions nothing in the game references are compiled and dead-stripped
 * by the linker as in retail. Entry points other objects link against keep
 * their dtk names; the comments give the HAL names.
 */
#include "crt/stdarg.h"
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/objalloc.h"
#include "sysdolphin/baselib/video.h"

/* GX enum values used here (this tree's GX.h does not define them) */
#define GX_PF_RGB565_Z16 2
#define GX_ZC_LINEAR 0
#define GX_ZC_MID 2
#define GX_GM_1_0 0
#define GX_MAX_Z24 0x00FFFFFF

#define HSD_DEFAULT_FIFO_SIZE (256 * 1024)
#define HSD_DEFAULT_XFB_MAX_NUM 2

#define OSRoundUp32B(x) (((u32) (x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32) (x)) & ~(32 - 1))

/* HSD_SetInitParameter's parameter ids, numbered as retail's jump table
 * dispatches them. 0-4 keep Melee's HSD_InitParam order; 5-7 are the
 * newer library's additions (7 is named by the function's own error
 * string; 5 and 6 are named for what they set). */
typedef enum _HSD_InitParam {
    HSD_INIT_FIFO_SIZE,
    HSD_INIT_XFB_MAX_NUM,
    HSD_INIT_HEAP_MAX_NUM,    /* obsolete since 1.3.0.0 */
    HSD_INIT_AUDIO_HEAP_SIZE, /* obsolete since 1.3.0.0 */
    HSD_INIT_RENDER_MODE_OBJ,
    HSD_INIT_HEAP_SIZE,
    HSD_INIT_ARENA,
    HSD_INIT_MEMORY_CALLBACKS
} HSD_InitParam;

typedef struct _HSD_MemReport {
    u32 total;
    u32 system;
    u32 xfb;
    u32 gxfifo;
    u32 heap;
} HSD_MemReport;

/* memory.c's callback table */
typedef struct __mem_cb {
    void* (*alloc)(u32 size, u32 align, u32 flags);
    void (*free)(void* ptr);
    void (*clear)(void);
    u32 (*get_remain)(void);
    BOOL (*check_own)(void* ptr);
} __mem_cb;

typedef s32 OSHeapHandle;

void DVDInit(void);
void* OSGetArenaLo(void);
void* OSGetArenaHi(void);
void OSSetArenaLo(void* addr);
void* OSAllocFromArenaLo(u32 size, u32 align);
u32 OSGetConsoleSimulatedMemSize(void);                                   /* OSGetPhysicalMemSize */
void* OSAllocFromHeap(OSHeapHandle heap, u32 size);          /* OSAllocFromHeap */
void OSFreeToHeap(OSHeapHandle heap, void* ptr);          /* OSFreeToHeap */
OSHeapHandle OSSetCurrentHeap(OSHeapHandle heap);             /* OSSetCurrentHeap */
OSHeapHandle OSCreateHeap(void* start, void* end);        /* OSCreateHeap */
void OSDestroyHeap(OSHeapHandle heap);                     /* OSDestroyHeap */
s32 OSCheckHeap(OSHeapHandle heap);                      /* OSCheckHeap */
void OSReport(const char* fmt, ...);
void VIWaitForRetrace(void);

void GXInvalidateVtxCache(void);                                  /* GXInvalidateVtxCache */
void GXInvalidateTexAll(void);
void GXInitLightAttn(GXLightObj* lt, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1,
                 f32 k2);                                /* GXInitLightAttn */
void GXInitLightPos(GXLightObj* lt, f32 x, f32 y, f32 z);   /* GXInitLightPos */
void GXInitLightDir(GXLightObj* lt, f32 nx, f32 ny, f32 nz); /* GXInitLightDir */
void GXInitLightColor(GXLightObj* lt, GXColor color);         /* GXInitLightColor */
void GXLoadLightObjImm(GXLightObj* lt, u32 light);
void GXSetPixelFmt(u32 pix_fmt, u32 z_fmt);                /* GXSetPixelFmt */
void GXSetFieldMode(u8 field_mode, u8 half_aspect_ratio);   /* GXSetFieldMode */

void _HSD_MemSetCallbacks(__mem_cb* cb, u32 size);
u32 HSD_Index2LightID(u32 index);
void HSD_StateInvalidate(u32 flags);                             /* HSD_StateInvalidate */
void HSD_IDSetup(void);

void HSD_ListInitAllocData(void);                                  /* HSD_ListInitAllocData */
void HSD_AObjInitAllocData(void);
void HSD_FObjInitAllocData(void);
void HSD_IDInitAllocData(void);
void HSD_VecInitAllocData(void);
void HSD_MtxInitAllocData(void);
void HSD_RObjInitAllocData(void);                                  /* HSD_RObjInitAllocData */
void HSD_RenderInitAllocData(void);
void HSD_ShadowInitAllocData(void);                                  /* HSD_ShadowInitAllocData */
void HSD_ZListInitAllocData(void);

HSD_ObjAllocData* HSD_AObjGetAllocData(void);
HSD_ObjAllocData* HSD_FObjGetAllocData(void);
HSD_ObjAllocData* HSD_IDGetAllocData(void);
HSD_ObjAllocData* HSD_SListGetAllocData(void);
HSD_ObjAllocData* HSD_DListGetAllocData(void);
HSD_ObjAllocData* HSD_VecGetAllocData(void);
HSD_ObjAllocData* HSD_MtxGetAllocData(void);
HSD_ObjAllocData* HSD_RObjGetAllocData(void);
HSD_ObjAllocData* HSD_RvalueObjGetAllocData(void);
HSD_ObjAllocData* HSD_ShadowGetAllocData(void);
HSD_ObjAllocData* HSD_RenderGetAllocData(void);
HSD_ObjAllocData* HSD_ChanGetAllocData(void);
HSD_ObjAllocData* HSD_TevRegGetAllocData(void);

void _HSD_AObjForgetMemory(void* lo, void* hi);
void _HSD_DispForgetMemory(void* lo, void* hi);
void _HSD_IDForgetMemory(void* lo, void* hi);
void _HSD_ObjAllocForgetMemory(void* lo, void* hi);
void _HSD_RandForgetMemory(void* lo, void* hi);
void _HSD_RObjForgetMemory(void* lo, void* hi);
void hsdForgetClassLibrary(const char* library_name);

extern GXRenderModeObj lbl_80312D30;                     /* GXNtsc480IntDf */

/* MWCC lays .bss/.sbss out in reverse declaration order; this order gives
 * retail's layout (memReport, FrameBuffer; hsd_heap_arena_lo first and
 * current_render_pass last). */
static void* FrameBuffer[HSD_VI_XFB_MAX];
static HSD_MemReport memReport;
static HSD_RenderPass current_render_pass;
static s32 invalidate_flags;
static s32 shown;
static s32 iparam_memory_callbacks;
static s32 iparam_heap_size;
static s32 init_done;
static int current_pix_fmt;
static GXFifoObj* DefaultFifoObj;
static void* iparam_arena_hi;
static void* iparam_arena_lo;
static void* hsd_heap_arena_hi;
static void* hsd_heap_arena_lo;

static OSHeapHandle current_heap = -1;
static GXRenderModeObj* rmode = &lbl_80312D30;
static int current_z_fmt = GX_ZC_MID;
static u32 iparam_fifo_size = HSD_DEFAULT_FIFO_SIZE;
static int iparam_xfb_max_num = HSD_DEFAULT_XFB_MAX_NUM;
static GXColor HSD_Init_804D5E1C = { 0 };

static void HSD_DVDInit(void);
static void HSD_GXInit(void);
static void HSD_OSInit(void);
static void HSD_ObjInit(void);
void* _HSD_MemAllocDefaultCB(u32 size, u32 align, u32 flags);
void _HSD_MemFreeDefaultCB(void* ptr);
u32 _HSD_MemGetRemainDefaultCB(void);
void _HSD_MemClearDefaultCB(void);
BOOL _HSD_MemCheckOwnDefaultCB(void* ptr);
void** HSD_AllocateXFB(s32 nbuffer, GXRenderModeObj* rm);
GXFifoObj* HSD_AllocateFifo(u32 size);
void HSD_GXSetFifoObj(GXFifoObj* fifo);

/* HAL: HSD_InitComponent */
void HSD_InitComponent(void)
{
    HSD_DVDInit();
    HSD_AllocateXFB(iparam_xfb_max_num, rmode);
    HSD_GXSetFifoObj(GXInit(HSD_AllocateFifo(iparam_fifo_size),
                            iparam_fifo_size));
    HSD_OSInit();
    {
        HSD_VIStatus vi_status;
        GXColor black = { 0, 0, 0, 0 };

        vi_status.rmode = *rmode;
        vi_status.black = TRUE;
        vi_status.vf = TRUE;
        vi_status.gamma = GX_GM_1_0;
        vi_status.clear_clr = black;
        vi_status.clear_z = GX_MAX_Z24;
        vi_status.update_clr = TRUE;
        vi_status.update_alpha = TRUE;
        vi_status.update_z = TRUE;

        HSD_VIInit(&vi_status, FrameBuffer[0], FrameBuffer[1],
                    FrameBuffer[2]);
    }

    HSD_GXInit();
    HSD_IDSetup();
    VIWaitForRetrace();
    HSD_ObjInit();
    init_done = TRUE;
}

void HSD_GXSetFifoObj(GXFifoObj* fifo)
{
    memReport.gxfifo = iparam_fifo_size;
    DefaultFifoObj = fifo;
}

static void HSD_DVDInit(void)
{
    DVDInit();
}

void** HSD_AllocateXFB(s32 nbuffer, GXRenderModeObj* rm)
{
    u32 fb_size;
    s32 i;

    if (rm == NULL) {
        return NULL;
    }
    fb_size =
        VIPadFrameBufferWidth(rm->fbWidth) * rm->xfbHeight * VI_DISPLAY_PIX_SZ;
    memReport.xfb = fb_size * nbuffer;
    for (i = 0; i < nbuffer; i++) {
        if ((FrameBuffer[i] = OSAllocFromArenaLo(fb_size, 32)) == NULL) {
            HSD_Panic(__FILE__, 241, "No memory space remains for XFB.\n");
        }
    }
    for (i = nbuffer; i < HSD_VI_XFB_MAX; i++) {
        FrameBuffer[i] = NULL;
    }
    return FrameBuffer;
}

GXFifoObj* HSD_AllocateFifo(u32 size)
{
    GXFifoObj* fifo;

    if ((fifo = OSAllocFromArenaLo(size, 32)) == NULL) {
        HSD_Panic(__FILE__, 260, "no space remains for gx fifo.\n");
    }
    return fifo;
}

static void HSD_GXInit(void)
{
    GXLightObj lightobj;
    int i;

    GXInitLightPos(&lightobj, 1.0F, 0.0F, 0.0F);
    GXInitLightDir(&lightobj, 1.0F, 0.0F, 0.0F);
    GXInitLightAttn(&lightobj, 1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
    GXInitLightColor(&lightobj, HSD_Init_804D5E1C);

    for (i = 0; i < 8; i++) {
        GXLoadLightObjImm(&lightobj, HSD_Index2LightID(i));
    }
    HSD_StateInvalidate(-1);
}

/*
 * A function nothing in the game calls sits here in HAL's source, so the
 * linker strips it. All retail proves is its message ("missing
 * argument.\n", pooled between the heap-callback assert and the FIFO
 * panic); its name, parameters and line number are unknown.
 */
void initialize_stripped_missing_argument(void* arg)
{
    if (arg == NULL) {
        HSD_Panic(__FILE__, 0, "missing argument.\n");
    }
}

/* HAL: HSD_OSInit. The arena bounds it reads first are no longer used (Melee
 * passed them to OSInitAlloc); retail still makes both calls. */
static void HSD_OSInit(void)
{
    u32 old_arena_lo = (u32) OSGetArenaLo();
    u32 old_arena_hi = (u32) OSGetArenaHi();

    memReport.total = OSGetConsoleSimulatedMemSize();
    memReport.system = memReport.total - (u32) OSGetArenaHi() +
                       (u32) OSGetArenaLo() - memReport.xfb -
                       memReport.gxfifo;

    if (!iparam_memory_callbacks) {
        __mem_cb cb;

        cb.alloc = _HSD_MemAllocDefaultCB;
        cb.free = _HSD_MemFreeDefaultCB;
        cb.get_remain = _HSD_MemGetRemainDefaultCB;
        cb.clear = _HSD_MemClearDefaultCB;
        cb.check_own = _HSD_MemCheckOwnDefaultCB;
        _HSD_MemSetCallbacks(&cb, sizeof(cb));

        if (iparam_arena_lo != NULL && iparam_arena_hi != NULL) {
            hsd_heap_arena_lo = iparam_arena_lo;
            hsd_heap_arena_hi = iparam_arena_hi;
            iparam_arena_lo = NULL;
            iparam_arena_hi = NULL;
            current_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
            OSSetCurrentHeap(current_heap);
            memReport.heap = (u32) hsd_heap_arena_hi - (u32) hsd_heap_arena_lo;
            HSD_ObjSetHeap(memReport.heap, NULL);
        } else {
            u32 arena_lo = OSRoundUp32B(OSGetArenaLo());
            u32 arena_hi = (u32) OSGetArenaHi();

            hsd_heap_arena_lo = (void*) arena_lo;
            if (iparam_heap_size > 0) {
                hsd_heap_arena_hi = (void*) (arena_lo + iparam_heap_size);
                if ((u32) hsd_heap_arena_hi > arena_hi) {
                    hsd_heap_arena_hi = (void*) arena_hi;
                }
                hsd_heap_arena_hi = (void*) OSRoundDown32B(hsd_heap_arena_hi);
            } else {
                hsd_heap_arena_hi = (void*) OSRoundDown32B(arena_hi);
            }
            current_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
            OSSetCurrentHeap(current_heap);
            memReport.heap = (u32) hsd_heap_arena_hi - (u32) hsd_heap_arena_lo;
            HSD_ObjSetHeap(memReport.heap, NULL);
            OSSetArenaLo(hsd_heap_arena_hi);
        }
    }
}

OSHeapHandle HSD_GetHeap(void);

void* _HSD_MemAllocDefaultCB(u32 size, u32 align, u32 flags)
{
    void* addr;

    if (size == 0) {
        return NULL;
    }
    addr = OSAllocFromHeap(HSD_GetHeap(), size);
    HSD_ASSERT(378, addr);
    return addr;
}

void _HSD_MemFreeDefaultCB(void* ptr)
{
    OSFreeToHeap(HSD_GetHeap(), ptr);
}

u32 _HSD_MemGetRemainDefaultCB(void)
{
    return OSCheckHeap(HSD_GetHeap());
}

void _HSD_MemClearDefaultCB(void)
{
    OSDestroyHeap(current_heap);
    if (iparam_arena_lo != NULL && iparam_arena_hi != NULL) {
        hsd_heap_arena_lo = iparam_arena_lo;
        hsd_heap_arena_hi = iparam_arena_hi;
        iparam_arena_lo = NULL;
        iparam_arena_hi = NULL;
    }
    current_heap = OSCreateHeap(hsd_heap_arena_lo, hsd_heap_arena_hi);
    OSSetCurrentHeap(current_heap);
}

BOOL _HSD_MemCheckOwnDefaultCB(void* ptr)
{
    BOOL result = FALSE;

    if (hsd_heap_arena_lo <= ptr && ptr < hsd_heap_arena_hi) {
        result = TRUE;
    }
    return result;
}

OSHeapHandle HSD_GetHeap(void)
{
    HSD_ASSERT(438, !iparam_memory_callbacks);
    return current_heap;
}

/*
 * HSD_CreateMainHeap: nothing in the game calls it, so the linker strips
 * it; what survives is its callback table (the first object in the TU's
 * .rodata) and the class-library name it forgets. Reduced to Melee's use
 * of both.
 */
OSHeapHandle HSD_CreateMainHeap(void* lo, void* hi)
{
    int i;
    void (*cb_table[])(void* lo, void* hi) = {
        _HSD_AObjForgetMemory,
        _HSD_DispForgetMemory,
        _HSD_IDForgetMemory,
        _HSD_ObjAllocForgetMemory,
        _HSD_RandForgetMemory,
        _HSD_RObjForgetMemory,
        NULL,
    };

    hsdForgetClassLibrary("sysdolphin_base_library");
    for (i = 0; cb_table[i] != NULL; i++) {
        cb_table[i](lo, hi);
    }
    return current_heap;
}

/* HAL: HSD_GetCurrentRenderPass */
HSD_RenderPass HSD_GetCurrentRenderPass(void)
{
    return current_render_pass;
}

/* HAL: HSD_StartRender */
void HSD_StartRender(HSD_RenderPass pass)
{
    GXRenderModeObj* rmode = HSD_VIGetRenderMode();

    current_render_pass = pass;
    if (rmode->aa) {
        GXSetPixelFmt(GX_PF_RGB565_Z16, current_z_fmt);
    } else {
        GXSetPixelFmt(current_pix_fmt, GX_ZC_LINEAR);
    }
    GXSetFieldMode(rmode->field_rendering, rmode->xfbHeight < rmode->viHeight);

    if (invalidate_flags) {
        if (invalidate_flags & 1) {
            GXInvalidateVtxCache();
        }
        if (invalidate_flags & 2) {
            GXInvalidateTexAll();
        }
        invalidate_flags = 0;
    }
}

/* Melee: HSD_Init_803755A8. Retail loads and compares the render pass and
 * returns; the body after the test is compiled out. */
void fn_8019C6FC(void)
{
    if (current_render_pass == HSD_RP_OFFSCREEN) {
        return;
    }
}

/* Requests cache invalidations for the next HSD_StartRender
 * (1: vertex cache, 2: all textures). */
void fn_8019C6EC(s32 flags)
{
    invalidate_flags |= flags;
}

static void HSD_ObjInit(void)
{
    HSD_ListInitAllocData();
    HSD_AObjInitAllocData();
    HSD_FObjInitAllocData();
    HSD_IDInitAllocData();
    HSD_VecInitAllocData();
    HSD_MtxInitAllocData();
    HSD_RObjInitAllocData();
    HSD_RenderInitAllocData();
    HSD_ShadowInitAllocData();
    HSD_ZListInitAllocData();
}

/* Sets the pixel and Z formats HSD_StartRender uses outside anti-aliasing
 * (which always renders RGB565_Z16). HAL name unknown; Melee's
 * initialize.c keeps only its assert string. */
void fn_8019C690(int pix_fmt, int z_fmt)
{
    HSD_ASSERT(758, pix_fmt != GX_PF_RGB565_Z16);
    current_pix_fmt = pix_fmt;
    current_z_fmt = z_fmt;
}

void HSD_ObjDumpStat(void)
{
    struct {
        HSD_ObjAllocData* (*func)(void);
        char* label;
    } types[] = { { HSD_AObjGetAllocData, "aobj" },
                  { HSD_FObjGetAllocData, "fobj" },
                  { HSD_IDGetAllocData, "id" },
                  { HSD_SListGetAllocData, "slist" },
                  { HSD_DListGetAllocData, "dlist" },
                  { HSD_VecGetAllocData, "vec" },
                  { HSD_MtxGetAllocData, "mtx" },
                  { HSD_RObjGetAllocData, "robj" },
                  { HSD_RvalueObjGetAllocData, "rval" },
                  { HSD_ShadowGetAllocData, "shadow" },
                  { HSD_RenderGetAllocData, "render" },
                  { HSD_ChanGetAllocData, "chan" },
                  { HSD_TevRegGetAllocData, "tevreg" },
                  { NULL, NULL } };
    int i;

    for (i = 0; types[i].label; i++) {
        OSReport("objalloc: %s\tusing %d\tfreed %d\tpeak %d\n", types[i].label,
                 HSD_ObjAllocGetUsing(types[i].func()),
                 HSD_ObjAllocGetFreed(types[i].func()),
                 HSD_ObjAllocGetPeak(types[i].func()));
    }
}

/* HAL: HSD_SetInitParameter */
BOOL HSD_SetInitParameter(HSD_InitParam param, ...)
{
    va_list ap;
    BOOL ok = FALSE;

    if (init_done) {
        if (!shown) {
            OSReport(
                "init parameter should be set before invoking HSD_Init().\n");
            shown = TRUE;
        }
        return ok;
    }

    va_start(ap, param);
    switch (param) {
    case HSD_INIT_FIFO_SIZE: {
        u32 fifo_size = va_arg(ap, u32);
        if (fifo_size != 0) {
            iparam_fifo_size = fifo_size;
            ok = TRUE;
        }
    } break;

    case HSD_INIT_XFB_MAX_NUM: {
        u32 xfb_max_num = va_arg(ap, u32);
        if (xfb_max_num != 0) {
            iparam_xfb_max_num = xfb_max_num;
            ok = TRUE;
        }
    } break;

    case HSD_INIT_HEAP_SIZE: {
        u32 heap_size = va_arg(ap, u32);
        if (heap_size < OSGetConsoleSimulatedMemSize()) {
            iparam_heap_size = heap_size;
            ok = TRUE;
        }
    } break;

    case HSD_INIT_ARENA: {
        void* lo = va_arg(ap, void*);
        void* hi = va_arg(ap, void*);
        iparam_arena_lo = lo;
        iparam_arena_hi = hi;
        ok = TRUE;
    } break;

    case HSD_INIT_MEMORY_CALLBACKS: {
        __mem_cb cb;

        cb.alloc = va_arg(ap, void*);
        cb.free = va_arg(ap, void*);
        cb.clear = va_arg(ap, void*);
        cb.get_remain = va_arg(ap, void*);
        cb.check_own = va_arg(ap, void*);
        if (va_arg(ap, void*) != NULL) {
            OSReport("ERROR in HSD_SetInitParameter():\n");
            OSReport(
                "  HSD_INIT_MEMORY_CALLBACKS was given invalid arguments.\n");
            break;
        }
        _HSD_MemSetCallbacks(&cb, sizeof(cb));
        iparam_memory_callbacks = TRUE;
        ok = TRUE;
    } break;

    case HSD_INIT_RENDER_MODE_OBJ: {
        GXRenderModeObj* rm = va_arg(ap, GXRenderModeObj*);
        if (rm != NULL) {
            rmode = rm;
            ok = TRUE;
        }
    } break;

    case HSD_INIT_HEAP_MAX_NUM:
        OSReport("ERROR in HSD_SetInitParameter():\n");
        OSReport("  HSD_INIT_HEAP_MAX_NUM is obsolete since 1.3.0.0. \n");
        if (va_arg(ap, s32) == 0) {
            ok = TRUE;
        }
        break;

    case HSD_INIT_AUDIO_HEAP_SIZE:
        OSReport("ERROR in HSD_SetInitParameter():\n");
        OSReport("  HSD_INIT_AUDIO_HEAP_SIZE is obsolete since 1.3.0.0. \n");
        break;
    }
    va_end(ap);

    return ok;
}
