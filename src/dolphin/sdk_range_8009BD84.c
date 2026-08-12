/**
 * @file sdk_range_8009BD84.c
 * @brief dolphin-sdk code, 0x8009BD84 - 0x8009DF88 (21 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/db/DB.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/si/SI.h"
#include "dolphin/vi/VI.h"

#define OS_FPUCONTEXT (*(OSContext* volatile*)0x800000D8)

void OSInitContext(OSContext* context, u32 pc, u32 newsp) {
    extern u8 _SDA_BASE_[];
    extern u8 _SDA2_BASE_[];

    context->srr0 = pc;
    context->gpr[1] = newsp;
    context->srr1 = 0x9032;
    context->cr = 0;
    context->xer = 0;
    context->gpr[2] = (u32)_SDA2_BASE_;
    context->gpr[13] = (u32)_SDA_BASE_;
    context->gpr[3] = 0;
    context->gpr[4] = 0;
    context->gpr[5] = 0;
    context->gpr[6] = 0;
    context->gpr[7] = 0;
    context->gpr[8] = 0;
    context->gpr[9] = 0;
    context->gpr[10] = 0;
    context->gpr[11] = 0;
    context->gpr[12] = 0;
    context->gpr[14] = 0;
    context->gpr[15] = 0;
    context->gpr[16] = 0;
    context->gpr[17] = 0;
    context->gpr[18] = 0;
    context->gpr[19] = 0;
    context->gpr[20] = 0;
    context->gpr[21] = 0;
    context->gpr[22] = 0;
    context->gpr[23] = 0;
    context->gpr[24] = 0;
    context->gpr[25] = 0;
    context->gpr[26] = 0;
    context->gpr[27] = 0;
    context->gpr[28] = 0;
    context->gpr[29] = 0;
    context->gpr[30] = 0;
    context->gpr[31] = 0;
    context->gqr[0] = 0;
    context->gqr[1] = 0;
    context->gqr[2] = 0;
    context->gqr[3] = 0;
    context->gqr[4] = 0;
    context->gqr[5] = 0;
    context->gqr[6] = 0;
    context->gqr[7] = 0;
    OSClearContext(context);
}

#define OS_CURRENTCONTEXT (*(OSContext**)0x800000D4)

/*
 * Retail's OSClearContext lives at 0x8009BD60, in the split just ahead of
 * this one but in the same TU, so it inlines into OSDumpContext below.
 */
static void ClearContext(OSContext* context) {
    context->mode = 0;
    context->state = 0;
    if (OS_FPUCONTEXT == context) {
        OS_FPUCONTEXT = NULL;
    }
}

void OSDumpContext(OSContext* context) {
    extern void OSReport(const char* format, ...);
    extern void OSSetCurrentContext(OSContext* context);
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSRestoreInterrupts(BOOL level);
    extern u8 lbl_803107E0[];
    u32 i;
    u32* p;

    OSReport((const char*)&lbl_803107E0[0x0], context);

    for (i = 0; i < 16; ++i) {
        OSReport((const char*)&lbl_803107E0[0x44], i, context->gpr[i],
                 context->gpr[i], i + 16, context->gpr[i + 16],
                 context->gpr[i + 16]);
    }

    OSReport((const char*)&lbl_803107E0[0x74], context->lr, context->cr);
    OSReport((const char*)&lbl_803107E0[0xA4], context->srr0, context->srr1);

    OSReport((const char*)&lbl_803107E0[0xD4]);
    for (i = 0; i < 4; ++i) {
        OSReport((const char*)&lbl_803107E0[0xE8], i, context->gqr[i], i + 4,
                 context->gqr[i + 4]);
    }

    if (context->state & OS_CONTEXT_STATE_FPSAVED) {
        OSContext* currentContext;
        OSContext fpuContext;
        BOOL enabled;

        enabled = OSDisableInterrupts();
        currentContext = OS_CURRENTCONTEXT;
        ClearContext(&fpuContext);
        OSSetCurrentContext(&fpuContext);

        OSReport((const char*)&lbl_803107E0[0x10C]);
        for (i = 0; i < 32; i += 2) {
            OSReport((const char*)&lbl_803107E0[0x120], i, (u32)context->fpr[i],
                     i + 1, (u32)context->fpr[i + 1]);
        }
        OSReport((const char*)&lbl_803107E0[0x13C]);
        for (i = 0; i < 32; i += 2) {
            OSReport((const char*)&lbl_803107E0[0x150], i, (u32)context->psf[i],
                     i + 1, (u32)context->psf[i + 1]);
        }

        ClearContext(&fpuContext);
        OSSetCurrentContext(currentContext);
        OSRestoreInterrupts(enabled);
    }

    OSReport((const char*)&lbl_803107E0[0x170]);
    for (i = 0, p = (u32*)context->gpr[1];
         p && (u32)p != 0xFFFFFFFF && i++ < 16; p = (u32*)*p) {
        OSReport((const char*)&lbl_803107E0[0x198], p, p[0], p[1]);
    }
}

#pragma scheduling off
void OSSwitchFPUContext(u8 exception, OSContext* context) {
    OSContext** fpuContext = (OSContext**)0x800000D8;
    OSContext* previous = *fpuContext;

    context->srr1 |= 0x2000;
    *fpuContext = context;
    if ((s32)previous != (s32)context) {
        if (previous != 0) {
            __OSSaveFPUContext(exception, 0, previous);
        }
        __OSLoadFPUContext(exception, context);
    }
    context->state &= ~OS_CONTEXT_STATE_EXC;
}
#pragma scheduling reset

#pragma peephole off
void __OSContextInit(void) {
    __OSSetExceptionHandler(OS_EXCEPTION_FLOATING_POINT,
                            (__OSExceptionHandler)OSSwitchFPUContext);
    OS_FPUCONTEXT = NULL;
    DBPrintf("FPU-unavailable handler installed\n");
}
#pragma peephole reset

void OSReport(const char* format, ...) {
    typedef struct {
        u8 gpr;
        u8 fpr;
        u16 reserved;
        u32* overflow_arg_area;
        u32* reg_save_area;
    } VaList[1];
    extern s32 vprintf(const char* format, VaList args);
    VaList args;

    __builtin_va_info(&args);
    vprintf(format, args);
}

/* FPSCR bits left standing when an FP exception is cleared. */
#define FPSCR_KEEP 0x6005F8FF
/* FPSCR VE|OE|UE|ZE|XE, and MSR FE0|FE1. */
#define FPSCR_ENABLE 0xF8
#define MSR_FE 0x900

OSErrorHandler OSSetErrorHandler(u16 error, OSErrorHandler handler) {
    extern OSErrorHandler __OSErrorTable[17];
    extern u32 lbl_80478990; /* OSDefaultFPSCR */
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSRestoreInterrupts(BOOL level);
    extern u32 PPCMfmsr(void);
    extern void PPCMtmsr(u32 msr);
    extern u32 PPCMffpscr(void);
    extern void PPCMtfpscr(u32 fpscr);
    OSErrorHandler oldHandler;
    OSThread* thread;
    BOOL enabled;
    u32 msr;
    u32 fpscr;
    u32 i;

    enabled = OSDisableInterrupts();
    oldHandler = __OSErrorTable[error];
    __OSErrorTable[error] = handler;

    if (error == 16) {
        msr = PPCMfmsr();
        PPCMtmsr(msr | 0x2000);
        fpscr = PPCMffpscr();

        if (handler != 0) {
            for (thread = *(OSThread**)0x800000DC; thread != NULL;
                 thread = thread->linkActive.next) {
                thread->context.srr1 |= MSR_FE;
                if (!(thread->context.state & OS_CONTEXT_STATE_FPSAVED)) {
                    thread->context.state |= OS_CONTEXT_STATE_FPSAVED;
                    for (i = 0; i < 32; i++) {
                        *(u64*)&thread->context.fpr[i] = -1;
                        *(u64*)&thread->context.psf[i] = -1;
                    }
                    thread->context.fpscr = 4;
                }
                thread->context.fpscr |= (lbl_80478990 & FPSCR_ENABLE);
                thread->context.fpscr &= FPSCR_KEEP;
            }
            msr |= MSR_FE;
            fpscr |= (lbl_80478990 & FPSCR_ENABLE);
        } else {
            for (thread = *(OSThread**)0x800000DC; thread != NULL;
                 thread = thread->linkActive.next) {
                thread->context.srr1 &= ~MSR_FE;
                thread->context.fpscr &= ~FPSCR_ENABLE;
                thread->context.fpscr &= FPSCR_KEEP;
            }
            fpscr &= ~FPSCR_ENABLE;
            msr &= ~MSR_FE;
        }

        fpscr &= FPSCR_KEEP;
        PPCMtfpscr(fpscr);
        PPCMtmsr(msr);
    }

    OSRestoreInterrupts(enabled);
    return oldHandler;
}

void __OSUnhandledException(u8 exception, OSContext* context, u32 dsisr,
                            u32 dar) {
    extern OSErrorHandler __OSErrorTable[17];
    extern u8 lbl_803109B8[];
    extern char lbl_80478994[];
    extern s16 __OSLastInterrupt;
    extern u32 __OSLastInterruptSrr0;
    extern s64 __OSLastInterruptTime;
    extern void OSReport(const char* format, ...);
    extern s64 OSGetTime(void);
    extern void OSDisableScheduler(void);
    extern void OSEnableScheduler(void);
    extern void __OSReschedule(void);
    extern void OSLoadContext(OSContext* context);
    extern void OSSaveFPUContext(OSContext* context);
    extern u32 PPCMfmsr(void);
    extern void PPCMtmsr(u32 msr);
    extern u32 PPCMffpscr(void);
    extern void PPCMtfpscr(u32 fpscr);
    extern void PPCHalt(void);
    s64 now = OSGetTime();
    u32 msr;

    if (!(context->srr1 & 0x2)) {
        OSReport((const char*)&lbl_803109B8[0x5C], exception);
    } else {
        if (exception == 6 && (context->srr1 & 0x00100000) &&
            __OSErrorTable[16]) {
            exception = 16;
            msr = PPCMfmsr();
            PPCMtmsr(msr | 0x2000);
            if (OS_FPUCONTEXT != NULL) {
                OSSaveFPUContext(OS_FPUCONTEXT);
            }
            PPCMtfpscr(PPCMffpscr() & FPSCR_KEEP);
            PPCMtmsr(msr);
            if (OS_FPUCONTEXT == context) {
                OSDisableScheduler();
                __OSErrorTable[16](16, context, dsisr, dar);
                context->srr1 &= ~0x2000;
                OS_FPUCONTEXT = NULL;
                context->fpscr &= FPSCR_KEEP;
                OSEnableScheduler();
                __OSReschedule();
            } else {
                context->srr1 &= ~0x2000;
                OS_FPUCONTEXT = NULL;
            }
            OSLoadContext(context);
        }

        if (__OSErrorTable[exception]) {
            OSDisableScheduler();
            __OSErrorTable[exception](exception, context, dsisr, dar);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }

        if (exception == 8) {
            OSLoadContext(context);
        }

        OSReport((const char*)&lbl_803109B8[0x7C], exception);
    }

    OSReport(lbl_80478994);
    OSDumpContext(context);
    OSReport((const char*)&lbl_803109B8[0x94], dsisr, dar);
    OSReport((const char*)&lbl_803109B8[0xC8], now);

    switch (exception) {
    case 2:
        OSReport((const char*)&lbl_803109B8[0xD8], context->srr0, dar);
        break;
    case 3:
        OSReport((const char*)&lbl_803109B8[0x138], context->srr0);
        break;
    case 5:
        OSReport((const char*)&lbl_803109B8[0x184], context->srr0, dar);
        break;
    case 6:
        OSReport((const char*)&lbl_803109B8[0x1E8], context->srr0, dar);
        break;
    case 15:
        OSReport(lbl_80478994);
        OSReport((const char*)&lbl_803109B8[0x248],
                 *(volatile u16*)0xCC005030, *(volatile u16*)0xCC005032);
        OSReport((const char*)&lbl_803109B8[0x268],
                 *(volatile u16*)0xCC005020, *(volatile u16*)0xCC005022);
        OSReport((const char*)&lbl_803109B8[0x288],
                 *(volatile u32*)0xCC006014);
        break;
    }

    OSReport((const char*)&lbl_803109B8[0x2A4], __OSLastInterrupt,
             __OSLastInterruptSrr0, __OSLastInterruptTime);
    PPCHalt();
}

typedef struct OSFontHeader {
    u16 fontType;
    u16 firstChar;
    u16 lastChar;
    u16 invalChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
    u16 cellWidth;
    u16 cellHeight;
    u32 sheetSize;
    u16 sheetFormat;
    u16 sheetColumn;
    u16 sheetRow;
    u16 sheetWidth;
    u16 sheetHeight;
    u16 widthTable;
    u32 sheetImage;
    u32 sheetFullSize;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;
} OSFontHeader;

/**
 * OSFatal's parameter block, at the head of the 0x2E8-byte lbl_803FB538
 * object: FatalParam (0xC, padded to 0x10) followed by FatalContext (0x2D8).
 */
typedef struct OSFatalParam {
    /* 0x00 */ GXColor fg;
    /* 0x04 */ GXColor bg;
    /* 0x08 */ const char* msg;
} OSFatalParam;

/* Inlined into fn_8009CE8C (Halt); emits no standalone code. */
static void ScreenClear(void* xfb, u16 xfbW, u16 xfbH, GXColor yuv) {
    int i;
    int j;
    u8* ptr;

    ptr = xfb;
    for (i = 0; i < xfbH; i++) {
        for (j = 0; j < xfbW; j += 2) {
            *ptr++ = yuv.r;
            *ptr++ = yuv.g;
            *ptr++ = yuv.r;
            *ptr++ = yuv.b;
        }
    }
}

void ScreenReport(void* xfb, u16 xfbW, u16 xfbH, GXColor yuv, s32 x, s32 y,
                  s32 leading, const char* string) {
    extern char* fn_8009DC38(const char* string, void* image, s32 pos,
                             s32 stride, s32* width);
    u8* ptr;
    s32 width;
    u32 i;
    u32 j;
    u32 image[72];
    u32 k;
    u32 l;
    u8 Y;
    u32 pixel;
    s32 col;

loop_1:
    if (xfbH - 24 >= y) {
        ptr = (u8*)xfb + ((x + (y * xfbW)) * 2);
        col = x;

        while ((s8)*string != 0) {
            if ((s8)*string == '\n') {
                string++;
                y += leading;
                goto loop_1;
            }

            if (xfbW - 48 < col) {
                y += leading;
                goto loop_1;
            }

            for (i = 0; i < 24; i++) {
                j = (i & 7) + ((i >> 3) * 24);
                image[j + 0] = 0;
                image[j + 8] = 0;
                image[j + 16] = 0;
            }

            string = fn_8009DC38(string, image, 0, 6, &width);

            for (i = 0; i < 24; i++) {
                j = (i & 7) + ((i >> 3) * 24);

                for (k = 0; k < 24; k++) {
                    l = j + (k & 0xFFFFFFF8);

                    Y = (image[l] >> ((7 - (k & 7)) * 4)) & 0xF;
                    if (Y != 0) {
                        Y = (((yuv.r * (Y * 0xEF)) / 255) / 15) + 0x10;
                        pixel = k + (i * xfbW);
                        ptr[pixel * 2] = Y;

                        if ((col + k) & 1) {
                            ptr[(pixel * 2) - 1] = yuv.g;
                            ptr[(pixel * 2) + 1] = yuv.b;
                        } else {
                            ptr[(pixel * 2) - 1] = yuv.b;
                            ptr[(pixel * 2) + 1] = yuv.g;
                        }
                    }
                }
            }

            ptr += width * 2;
            col += width;
        }
    }
}

void ConfigureVideo(u16 fbWidth, u16 xfbHeight) {
    extern void VIConfigure(GXRenderModeObj* mode);
    extern void VIConfigurePan(u16 xOrigin, u16 yOrigin, u16 width, u16 height);
    GXRenderModeObj mode;

    mode.fbWidth = fbWidth;
    mode.efbHeight = 480;
    mode.xfbHeight = xfbHeight;
    mode.viXOrigin = 40;
    mode.viWidth = 640;
    mode.viHeight = xfbHeight;

    switch (VIGetTvFormat()) {
    case VI_NTSC:
    case VI_MPAL:
        if (*(volatile u16*)0xCC00206C & 1) {
            mode.viTVmode = 2;
            mode.viYOrigin = 0;
            mode.xfbMode = 0;
        } else {
            mode.viTVmode = 0;
            mode.viYOrigin = 0;
            mode.xfbMode = 1;
        }
        break;
    case VI_EUR60:
        mode.viTVmode = 20;
        mode.viYOrigin = 0;
        mode.xfbMode = 1;
        break;
    case VI_PAL:
        mode.viTVmode = 4;
        mode.viYOrigin = 47;
        mode.xfbMode = 1;
        break;
    }

    VIConfigure(&mode);
    VIConfigurePan(0, 0, 640, 480);
}

/* Inlined into fn_8009CE8C (Halt); emits no standalone code. */
static GXColor RGB2YUV(GXColor rgb) {
    f32 Y;
    f32 Cb;
    f32 Cr;
    GXColor yuv;

    Y = 0.5f + (16.0f + ((0.098f * (f32)rgb.b) +
                         ((0.257f * (f32)rgb.r) + (0.504f * (f32)rgb.g))));
    Cb = 0.5f + (128.0f + ((0.439f * (f32)rgb.b) +
                           ((-0.148f * (f32)rgb.r) - (0.291f * (f32)rgb.g))));
    Cr = 0.5f + (128.0f + (((0.439f * (f32)rgb.r) - (0.368f * (f32)rgb.g)) -
                           (0.071f * (f32)rgb.b)));

    yuv.r = (Y > 235.0f) ? 235.0f : (Y < 16.0f) ? 16.0f : Y;
    yuv.g = (Cb > 240.0f) ? 240.0f : (Cb < 16.0f) ? 16.0f : Cb;
    yuv.b = (Cr > 240.0f) ? 240.0f : (Cr < 16.0f) ? 16.0f : Cr;
    yuv.a = 0;

    return yuv;
}

/**
 * OSFatal. Halts the machine, then hands off to fn_8009CE8C (Halt) on a fresh
 * fiber stack at the top of the arena to draw the message to the screen.
 */
void fn_8009CD38(GXColor fg, GXColor bg, const char* msg) {
    extern void __OSStopAudioSystem(void);
    extern void AISetStreamVolLeft(u8 vol);
    extern void AISetStreamVolRight(u8 vol);
    extern BOOL fn_8009FEBC(BOOL final); /* __OSCallResetFunctions */
    extern u32 fn_800AA280(void);        /* VIGetRetraceCount */
    extern void fn_800B8AE8(void);       /* GXAbortFrame */
    extern void VIInit(void);
    extern OSTime OSGetTime(void);
    extern void VISetBlack(BOOL black);
    extern void VIFlush(void);
    extern u8 lbl_803FB538[];            /* FatalParam + FatalContext */
#define FatalContext (*(OSContext*)(lbl_803FB538 + 0x10))
    extern void fn_8009CE8C(void);       /* Halt */
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSEnableInterrupts(void);

    OSFatalParam* fp = (OSFatalParam*)lbl_803FB538;
    OSBootInfo* bootInfo = (OSBootInfo*)0x80000000;
    u32 count;
    u32 timeout;
    OSTime t;

    OSDisableInterrupts();
    OSDisableScheduler();
    OSClearContext(&FatalContext);
    OSSetCurrentContext(&FatalContext);
    __OSStopAudioSystem();
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    VIInit();
    VISetBlack(TRUE);
    VIFlush();
    OSEnableInterrupts();

    count = fn_800AA280();
    do {
    } while ((s32)(fn_800AA280() - count) < 1);

    t = OSGetTime();
    timeout = OSMillisecondsToTicks(1000);
    while (!fn_8009FEBC(FALSE) && OSGetTime() - t < timeout) {
    }

    OSDisableInterrupts();
    fn_8009FEBC(TRUE);
    __OSSetExceptionHandler(8, OSDefaultExceptionHandler);
    fn_800B8AE8();
    OSSetArenaLo((void*)0x81200000);
    OSSetArenaHi((void*)bootInfo->FSTLocation);

    fp->fg = fg;
    fp->bg = bg;
    fp->msg = msg;
    OSSwitchFiber((u32)fn_8009CE8C, (u32)OSGetArenaHi());
}

/** EXI channel register file; [3] is the channel-0 control register. */
volatile u32 __EXIRegs[15] : 0xCC006800;

/**
 * Halt: OSFatal's fiber entry point. Copies the message into the fresh arena,
 * quiesces EXI, loads the SDK font, clears the XFB to the background colour
 * and prints the message, then stops the CPU.
 *
 * ScreenClear and RGB2YUV above are both inlined here.
 */
void fn_8009CE8C(void) {
    extern void fn_8009870C(s32 chan, void* callback); /* EXISetExiCallback */
    extern BOOL EXILock(s32 chan, u32 dev, void* callback);
    extern BOOL EXIDeselect(s32 chan);
    extern BOOL EXIUnlock(s32 chan);
    extern u16 fn_8009D820(void);                        /* OSGetFontEncode */
    extern u32 fn_8009D904(OSFontHeader* fontData, void* tmp); /* OSLoadFont */
    extern u32 fn_800AA280(void);                        /* VIGetRetraceCount */
    extern void* OSAllocFromArenaLo(u32 size, u32 align);
    extern void* memmove(void* dst, const void* src, u32 size);
    extern u32 strlen(const char* s);
    extern void VISetNextFrameBuffer(void* fb);
    extern void VISetBlack(BOOL black);
    extern void VIFlush(void);
    extern void DCFlushRange(void* addr, u32 nBytes);
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSEnableInterrupts(void);
    extern void OSReport(const char* format, ...);
    extern u8 lbl_803FB538[]; /* FatalParam + FatalContext */
    extern char lbl_80478998[]; /* "%s\n" */

    OSFatalParam* fp;
    u32 count;
    OSFontHeader* fontData;
    void* xfb;
    u32 len;

    OSEnableInterrupts();
    fp = (OSFatalParam*)lbl_803FB538;
    len = strlen(fp->msg) + 1;
    fp->msg = memmove(OSAllocFromArenaLo(len, 32), fp->msg, len);

    fn_8009870C(0, NULL);
    fn_8009870C(2, NULL);

    while (!EXILock(0, 1, NULL)) {
        while ((__EXIRegs[3] & 1) == 1) {
        }
        EXIDeselect(0);
        EXIUnlock(0);
    }
    EXIUnlock(0);

    while ((__EXIRegs[3] & 1) == 1) {
    }

    if (fn_8009D820() == 1) {
        fontData = OSAllocFromArenaLo(0x90EE4, 32);
    } else {
        fontData = OSAllocFromArenaLo(0x10120, 32);
    }
    fn_8009D904(fontData, OSGetArenaLo());

    xfb = OSAllocFromArenaLo(0x96000, 32);
    ScreenClear(xfb, 640, 480, RGB2YUV(fp->bg));
    VISetNextFrameBuffer(xfb);
    ConfigureVideo(640, 480);
    VIFlush();

    count = fn_800AA280();
    do {
    } while ((s32)(fn_800AA280() - count) < 2);

    ScreenReport(xfb, 640, 480, RGB2YUV(fp->fg), 48, 100, fontData->leading,
                 fp->msg);
    DCFlushRange(xfb, 0x96000);
    VISetBlack(FALSE);
    VIFlush();

    count = fn_800AA280();
    do {
    } while ((s32)(fn_800AA280() - count) < 1);

    OSDisableInterrupts();
    OSReport(lbl_80478998, fp->msg);
    PPCHalt();
}


/* 0x8009D820 | 0x58 */
static BOOL IsSjisTrailByte(u8 c) {
    return (0x40 <= c && c <= 0xFC) && (c != 0x7F);
}

/* Maps a character code to its index in the IPL font sheet. */
int fn_8009D510(u16 code) {
    extern u16 lbl_80310CD8[]; /* HankakuToCode */
    extern u16 lbl_80310E58[]; /* Zenkaku2Code */
    extern u16 fn_8009D820(void);

    if (fn_8009D820() == 1) {
        if (code >= 0x20 && code <= 0xDF) {
            return lbl_80310CD8[code - 0x20];
        }

        if (code > 0x889E && code <= 0x9872) {
            int i = ((code >> 8) - 0x88) * 188;
            int j = (code & 0xFF);

            if (!IsSjisTrailByte(j)) {
                return 0;
            }

            j -= 0x40;
            if (j >= 0x40) {
                j--;
            }

            return (i + j + 0x2BE);
        }

        if (code >= 0x8140 && code < 0x879E) {
            int i = ((code >> 8) - 0x81) * 188;
            int j = (code & 0xFF);

            if (!IsSjisTrailByte(j)) {
                return 0;
            }

            j -= 0x40;
            if (j >= 0x40) {
                j--;
            }

            return lbl_80310E58[i + j];
        }
    } else if (code > 0x20 && code <= 0xFF) {
        return code - 0x20;
    }

    return 0;
}

/* Yay0 decompressor for the IPL font sheet. */
void Decode(u8* s, u8* d) {
    int i;
    int j;
    int k;
    int p;
    int q;
    int linkTable;
    int chunkSrc;
    int cnt;
    int os;
    unsigned int flag;
    unsigned int code;

    os = *(int*)(s + 0x4);
    linkTable = *(int*)(s + 0x8);
    chunkSrc = *(int*)(s + 0xC);

    q = 0;
    flag = 0;
    p = 16;

    do {
        if (flag == 0) {
            code = *(u32*)(s + p);
            p += sizeof(u32);
            flag = sizeof(u32) * 8;
        }

        if (code & 0x80000000) {
            d[q++] = s[chunkSrc++];
        } else {
            j = s[linkTable] << 8 | s[linkTable + 1];
            linkTable += sizeof(u16);

            k = q - (j & 0x0FFF);
            cnt = j >> 12;
            if (cnt == 0) {
                cnt = s[chunkSrc++] + 0x12;
            } else {
                cnt += 2;
            }

            for (i = 0; i < cnt; i++, q++, k++) {
                d[q] = d[k - 1];
            }
        }

        code <<= 1;
        flag--;
    } while (q < os);
}

u16 fn_8009D820(void) {
    extern u16 lbl_804789A0;

    if (lbl_804789A0 <= 1) {
        return lbl_804789A0;
    }

    switch (*(s32*)0x800000CC) {
    case 0:
        lbl_804789A0 = (*(volatile u16*)0xCC00206E & 2) ? 1 : 0;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        lbl_804789A0 = 0;
        break;
    }

    return lbl_804789A0;
}

void fn_8009D878(void* dest, s32 size, s32 offset) {
    extern BOOL __OSReadROM(void* dest, s32 size, s32 offset);

    while (size > 0) {
        s32 chunkSize = size <= 0x100 ? size : 0x100;
        size -= chunkSize;
        while (!__OSReadROM(dest, chunkSize, offset)) {
        }
        offset += chunkSize;
        dest = (u8*)dest + chunkSize;
    }
}

/* Font state: header, width table, and the cached sheetColumn*sheetRow. */
#define FontData lbl_8047A700
#define FontWidthTable lbl_8047A708
#define FontCharsPerSheet lbl_8047A70C

static inline u32 GetFontSize(u8* buf) {
    if (buf[0] == 'Y' && buf[1] == 'a' && buf[2] == 'y') {
        return *(u32*)(buf + 4);
    }

    return 0;
}

static inline u32 ReadFont(void* image) {
    if (fn_8009D820() == 1) {
        fn_8009D878(image, 0x4D000, 0x1AFF00);
    } else {
        fn_8009D878(image, 0x3000, 0x1FCF00);
    }

    return GetFontSize(image);
}

/*
 * Reads the font sheet out of IPL ROM, decompresses it, and caches the
 * header, width table and chars-per-sheet that fn_8009DC38 reads back.
 */
u32 fn_8009D904(OSFontHeader* fontData, void* tmp) {
    extern OSFontHeader* FontData;
    extern u8* FontWidthTable;
    extern s32 FontCharsPerSheet;
    extern u32 lbl_8047A704;
    extern const u32 lbl_8047C280;
    extern const u32 lbl_8047C284;
    extern void fn_8009D878(void* dest, s32 size, s32 offset);
    extern void Decode(u8* s, u8* d);
    u32 size;

    lbl_8047A704 = 0;
    size = ReadFont(tmp);
    if (size != 0) {
        Decode(tmp, (u8*)fontData);
        FontData = fontData;
        FontWidthTable = (u8*)FontData + FontData->widthTable;
        FontCharsPerSheet = FontData->sheetColumn * FontData->sheetRow;

        if (fn_8009D820() == 1) {
            u8* image;
            u8* ptr;
            int fontCode;
            int sheet;
            int numChars;
            int row;
            int column;
            int x;
            int y;
            u16 glyph[4];

            *(u32*)&glyph[0] = lbl_8047C280;
            *(u32*)&glyph[2] = lbl_8047C284;

            fontCode = fn_8009D510(0x54);
            sheet = fontCode / FontCharsPerSheet;
            numChars = fontCode - (sheet * FontCharsPerSheet);
            row = numChars / FontData->sheetColumn;
            column = numChars - (row * FontData->sheetColumn);
            row *= FontData->cellHeight;
            column *= FontData->cellWidth;
            image = (u8*)FontData + FontData->sheetImage;
            image += (sheet * FontData->sheetSize) / 2;

            for (y = 4; y < 8; y++) {
                x = 0;
                ptr = image + (((FontData->sheetWidth / 8) * 32) / 2) *
                                  ((row + y) / 8);
                ptr += ((column + x) / 8) * 16;
                ptr += ((row + y) % 8) * 2;
                ptr += ((column + x) % 8) / 4;

                *(u16*)ptr = glyph[y - 4];
            }
        }
    }

    return size;
}

static BOOL IsSjisLeadByte(u8 c) {
    return ((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xE0) && (c <= 0xFC));
}

char* fn_8009DC38(const char* string, void* image, s32 pos, s32 stride,
                  s32* width) {
    extern OSFontHeader* FontData;
    extern u8* FontWidthTable;
    extern s32 FontCharsPerSheet;
    extern int fn_8009D510(u16 code);
    u16 encode;
    u16 code;
    u8* dst;
    s32 fontCode;
    s32 sheet;
    u8* src;
    s32 numChars;
    s32 row;
    s32 column;
    s32 x;
    s32 y;
    s32 offsetSrc;
    u8* colorIndex;
    u8* imageSrc;

    code = (u8)*string;
    if (code == 0) {
        return (char*)string;
    }
    string++;

    encode = fn_8009D820();
    if (encode == 1) {
        if (IsSjisLeadByte((u8)code) && (s8)*string != 0) {
            code = (code << 8) | (u8)*string;
            string++;
        }
    }

    colorIndex = &FontData->c0;
    fontCode = fn_8009D510(code);

    sheet = fontCode / FontCharsPerSheet;
    numChars = fontCode - (sheet * FontCharsPerSheet);
    row = numChars / FontData->sheetColumn;
    column = numChars - (row * FontData->sheetColumn);
    row *= FontData->cellHeight;
    column *= FontData->cellWidth;

    imageSrc = (u8*)FontData + FontData->sheetImage;
    imageSrc += ((sheet * FontData->sheetSize) >> 1);

    for (y = 0; y < FontData->cellHeight; y++) {
        for (x = 0; x < FontData->cellWidth; x++) {
            src = imageSrc + (((FontData->sheetWidth / 8) * 32) / 2) *
                                 ((row + y) / 8);
            src += ((column + x) / 8) * 16;
            src += ((row + y) % 8) * 2;
            src += ((column + x) % 8) / 4;

            offsetSrc = (column + x) % 4;

            dst = (u8*)image + ((y / 8) * (((stride * 4) / 8) * 32));
            dst += (((pos + x) / 8) * 32);
            dst += ((y % 8) * 4);
            dst += ((pos + x) % 8) / 2;

            *dst |= colorIndex[*src >> (6 - (offsetSrc * 2)) & 3] &
                    (((pos + x) % 2 != 0) ? 0x0F : 0xF0);
        }
    }

    if (width != 0) {
        *width = FontWidthTable[fontCode];
    }

    return (char*)string;
}
