#include "dolphin/types.h"
#include "pcport_window.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    u8 r, g, b, a;
} GXColor;

typedef void (*GXDrawDoneCallback)(void);

extern void VIConfigure_PC(void* mode);
extern void VIFlush_PC(void);
extern void VISetBlack_PC(BOOL black);
extern void GXInit(void* base, u32 size);
extern GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb);
extern void GXSetDispCopyGamma(u32 gamma);
extern void GXSetViewport(f32 xOrig, f32 yOrig, f32 wd, f32 ht,
                          f32 nearZ, f32 farZ);
extern void GXSetScissor(u32 xOrig, u32 yOrig, u32 wd, u32 ht);
extern void GXSetBlendMode(u32 type, u32 src_factor, u32 dst_factor, u32 op);
extern void GXSetAlphaCompare(u32 comp0, u8 ref0, u32 op, u32 comp1, u8 ref1);
extern void GXSetZMode(u8 compare_enable, u32 func, u8 update_enable);
extern void GXSetFog(u32 type, f32 startz, f32 endz,
                     f32 nearz, f32 farz, GXColor color);
extern void GXSetTevOp(u32 stage, u32 mode);
extern void GXCallDisplayList(void* list, u32 nbytes);

typedef struct {
    void* ptr;
    u32 size;
} HostAllocRecord;

typedef void (*PreRetraceCallback)(s32 flag, f32 p1, f32 p2,
                                   f32 p3, f32 p4, f32 p5, f32 p6);

static HostAllocRecord g_hostAllocs[32];
static void* g_retraceCallbacks[8];
static void* g_preRetraceCallback;
static s32 g_internalMode = -1;

const char lbl_80270360[] = "GSgfx: unable to allocate gsgfx state!\n";
const char lbl_80270388[] = "GSgfx: Init OK, state located at %p (size=%d)\n";

u8 lbl_80312D30[0x20] = { 0, 0, 0, 0, 0x80, 0x02, 0xE0, 0x01 };
u8 lbl_803130F0[0x20] = { 0, 0, 0, 0, 0x80, 0x02, 0x40, 0x02 };
u8 lbl_80312F4C[0x20] = { 0, 0, 0, 0, 0x80, 0x02, 0xE0, 0x01 };
u8 lbl_80466BC0[0x200];
u8 lbl_804001F0[0x100];

static HostAllocRecord* HostAllocFromHandle(u16 handle) {
    if (handle == 0 || handle > (u16)(sizeof(g_hostAllocs) / sizeof(g_hostAllocs[0]))) {
        return NULL;
    }

    return &g_hostAllocs[handle - 1];
}

void fn_800DD970(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

u16 GSmemAllocRaw(u32 size) {
    size_t i;

    for (i = 0; i < sizeof(g_hostAllocs) / sizeof(g_hostAllocs[0]); ++i) {
        if (g_hostAllocs[i].ptr == NULL) {
            g_hostAllocs[i].ptr = calloc(1, size);
            g_hostAllocs[i].size = size;
            if (g_hostAllocs[i].ptr == NULL) {
                return 0;
            }
            return (u16)(i + 1);
        }
    }

    return 0;
}

void* GSmemGetPtr(u16 handle) {
    HostAllocRecord* record = HostAllocFromHandle(handle);
    if (record == NULL) {
        return NULL;
    }

    return record->ptr;
}

void fn_800E0790(void) {
}

void fn_800EEC38(u32 slot, void* callback) {
    if (slot < (sizeof(g_retraceCallbacks) / sizeof(g_retraceCallbacks[0]))) {
        g_retraceCallbacks[slot] = callback;
    }
}

void fn_8019C3C4(u32 selector, ...) {
    va_list args;

    va_start(args, selector);
    if (selector == 4) {
        void* mode = va_arg(args, void*);
        VIConfigure_PC(mode);
    } else {
        (void)va_arg(args, u32);
    }
    va_end(args);
}

void fn_8019CB70(void) {
    VIFlush_PC();
}

void fn_8019C690(u32 black, u32 unused) {
    (void)unused;
    VISetBlack_PC((BOOL)(black != 0));
}

void fn_801C021C(void) {
    if (getenv("PCPORT_GL_TRACE") != NULL) {
        fprintf(stderr, "[gs_gfx_host_support] GXInit:start\n");
    }
    GXInit(NULL, 0);
    if (getenv("PCPORT_GL_TRACE") != NULL) {
        fprintf(stderr, "[gs_gfx_host_support] GXInit:done\n");
    }
}

void fn_801C01C8(void* callback) {
    GXSetDrawDoneCallback((GXDrawDoneCallback)callback);
}

void fn_80196C3C(void* callback) {
    g_preRetraceCallback = callback;
}

void PCPort_RunPreRetraceCallback(void) {
    PreRetraceCallback callback = (PreRetraceCallback)g_preRetraceCallback;

    if (callback == NULL) {
        return;
    }

    callback(0,
             0.0f,
             0.0f,
             (f32)PCPort_GetVideoWidth(),
             (f32)PCPort_GetVideoHeight(),
             0.0f,
             1.0f);
}

void fn_801BF4C4(u32 gamma) {
    *(u32*)(lbl_80466BC0 + 0x3C) = gamma;
    *(u8*)(lbl_80466BC0 + 0x54) = 1;
    GXSetDispCopyGamma(gamma);
}

void fn_800D5504(u32 memSize) {
    (void)memSize;
}

void fn_800D83E4(u32 fifoSize) {
    (void)fifoSize;
}

void fn_800D7B80(u32 mtxDepth) {
    (void)mtxDepth;
}

void fn_800DB890(u32 lightCount) {
    (void)lightCount;
}

void fn_800D7894(void) {
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
}

void fn_800D9C24(u32 x0, u32 y0, u32 x1, u32 y1) {
    GXSetViewport((f32)x0, (f32)y0,
                  (f32)((x1 - x0) + 1), (f32)((y1 - y0) + 1),
                  0.0f, 1.0f);
}

void fn_800D87AC(s32 mode) {
    g_internalMode = mode;
    (void)g_internalMode;
}

void fn_800DA2BC(u32 type, u32 src, u32 dst) {
    GXSetBlendMode(type, src, dst, 3);
}

void fn_800DA1E8(u32 zEnable, u32 zFunc, u32 zUpdate) {
    GXSetZMode((u8)zEnable, zFunc, (u8)zUpdate);
}

void fn_800DA100(u32 comp0, u32 ref0, u32 op,
                 u32 comp1, u32 ref1, u32 unused) {
    (void)unused;
    GXSetAlphaCompare(comp0, (u8)ref0, op, comp1, (u8)ref1);
}

void fn_800DA028(u32 mode) {
    GXSetTevOp(0, mode);
}

void fn_800D9F40(u32 fogMode) {
    GXColor fogColor = { 0, 0, 0, 0 };
    GXSetFog(fogMode, 0.0f, 0.0f, 0.0f, 1.0f, fogColor);
}

void fn_800BD0FC(void* list, u32 nbytes) {
    GXCallDisplayList(list, nbytes);
}

void fn_800D3E4C(void) {
}

void fn_800D3F5C(void) {
}
