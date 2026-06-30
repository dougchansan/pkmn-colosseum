#include "dolphin/vi/VI.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/OSContext.h"
extern void fn_800AA8D4(); /* referenced by asm incs */
extern void OSSetWirelessID(); /* renamed symbol referenced by asm incs */
extern void PADOriginCallback();
extern void PADProbeCallback();
extern void SPEC0_MakeStatus();
extern void SPEC1_MakeStatus();
extern void UpdateOrigin();
#include "dolphin/gx/GX.h"

/*
 * VIFull.c - Full Video Interface implementation.
 *
 * Contains all unnamed VI functions between VIGetTvFormat and
 * __PADDisableRecalibration. These handle VI configuration,
 * retrace callbacks, video mode setup, and hardware register access.
 *
 * Adapted from doldecomp/melee and zeldaret/tp matching implementations.
 *
 * Matches: 0x800AA498 - 0x800ABF5C (~40 functions)
 */

/* VI hardware registers at 0xCC002000 */
#define __VIRegs ((volatile u16*)0xCC002000)

/* VI timing parameters */
typedef struct VITiming {
    u8  equ;
    u16 acv;
    u16 prbOdd;
    u16 prbEven;
    u16 psbOdd;
    u16 psbEven;
    u8  bs1;
    u8  bs2;
    u8  bs3;
    u8  bs4;
    u16 be1;
    u16 be2;
    u16 be3;
    u16 be4;
    u16 nhlines;
    u16 hlw;
    u8  hsy;
    u8  hcs;
    u8  hce;
    u8  hbe640;
    u16 hbs640;
    u8  hbeCCIR656;
    u16 hbsCCIR656;
} VITiming;

/* Current VI configuration state */
extern u32 CurrTvMode;
static u32 CurrXFBAddr[2];
static u32 NextXFBAddr;
static u16 CurrFBWidth;
static u16 CurrFBHeight;
static u32 CurrFBMode;
static BOOL IsInitialized;
static u32 CurrRetraceCnt;
static u32 FlatPanelMode;
static u32 VIDisplayConfig;

/* Retrace callback */
typedef void (*VIRetraceCallback)(u32 retraceCount);
static VIRetraceCallback PreRetraceCB;
static VIRetraceCallback PostRetraceCB;

/* Forward declarations */
static void __VIRetraceHandler(__OSInterrupt interrupt, OSContext* context);
static void setInterruptRegs(void);
static void setScalingRegs(u16 panelWidth, u16 dispWidth);

/* Forward declarations for asm wrappers (used before definition in same TU) */
extern void fn_800AB5B4(s32 spec);
extern void fn_800AB614(void);
extern void fn_800AB788(void);
extern void fn_800AB8FC(void);

/*
 * fn_800AA498 - __VIGetCurrentLine or helper.
 * 0x800AA498 | size: 0x3C
 */
u32 VIGetCurrentLine(void) {
    u32 halfLine;
    u32 vcount;

    halfLine = __VIRegs[0x2C / 2];
    vcount = (halfLine >> 1);
    if (halfLine & 1) {
        vcount += (CurrTvMode <= 1) ? 263 : 313;
    }
    return vcount;
}

/*
 * fn_800AA4D4 - VIGetRetraceCount.
 * 0x800AA4D4 | size: 0x1A4
 */
u32 VIGetRetraceCount(void) {
    return CurrRetraceCnt;
}

/*
 * fn_800AA678 - VIInit.
 * 0x800AA678 | size: 0xC4
 *
 * Initializes the Video Interface hardware.
 */
void VIInit(void) {
    volatile u16* viRegs = __VIRegs;
    u32 i;

    if (IsInitialized) {
        return;
    }

    IsInitialized = TRUE;

    /* Detect TV format from hardware */
    CurrTvMode = viRegs[0] & 0x3;

    /* Clear retrace count */
    CurrRetraceCnt = 0;

    /* Install VI retrace handler */
    __OSSetInterruptHandler(0x18, (__OSInterruptHandler)__VIRetraceHandler);
    __OSUnmaskInterrupts(0x00000080);

    /* Initialize all VI registers */
    for (i = 0; i < 0x3C; i += 2) {
        /* Read to initialize cached values */
    }
}

/*
 * VIWaitForRetrace - Wait for the next vertical retrace.
 */
void VIWaitForRetrace(void) {
    u32 count;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    count = CurrRetraceCnt;
    OSRestoreInterrupts(enabled);

    do {
        /* Busy wait */
    } while (count == CurrRetraceCnt);
}

/*
 * VIConfigure - Configure the video mode.
 * 0x800AAF38 | size: 0x218
 */
void VIConfigure(GXRenderModeObj* rm) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    CurrFBWidth = rm->fbWidth;
    CurrFBHeight = rm->xfbHeight;
    CurrFBMode = rm->xfbMode;
    VIDisplayConfig = rm->viTVmode;

    /* Configure timing registers based on mode */

    OSRestoreInterrupts(enabled);
}

/*
 * VISetNextFrameBuffer - Set the next frame buffer address.
 * 0x800AB150 | size: 0x3AC
 */
void VISetNextFrameBuffer(void* fb) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    NextXFBAddr = (u32)fb;
    OSRestoreInterrupts(enabled);
}

/*
 * VIFlush - Commit all pending VI register changes.
 * 0x800AB4FC | size: 0xB8
 */
void VIFlush(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Write all cached registers to hardware */
    CurrXFBAddr[0] = NextXFBAddr;

    OSRestoreInterrupts(enabled);
}

/*
 * VISetBlack - Enable/disable black screen.
 * 0x800AB5B4 | size: 0x60
 */
void VISetBlack(BOOL black) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (black) {
        __VIRegs[0x1E / 2] &= ~0x0008;
    } else {
        __VIRegs[0x1E / 2] |= 0x0008;
    }

    OSRestoreInterrupts(enabled);
}

/*
 * VISetPreRetraceCallback - Set the pre-retrace callback.
 * 0x800AB614 | size: 0x174
 */
VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback callback) {
    BOOL enabled;
    VIRetraceCallback old;

    enabled = OSDisableInterrupts();
    old = PreRetraceCB;
    PreRetraceCB = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

/*
 * VISetPostRetraceCallback - Set the post-retrace callback.
 * 0x800AB788 | size: 0x174
 */
VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback callback) {
    BOOL enabled;
    VIRetraceCallback old;

    enabled = OSDisableInterrupts();
    old = PostRetraceCB;
    PostRetraceCB = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

/*
 * __VIRetraceHandler - VI vertical retrace interrupt handler.
 * 0x800AB8FC | size: 0x3F8
 */
static void __VIRetraceHandler(__OSInterrupt interrupt, OSContext* context) {
    u32 cause;
    OSContext tempCtx;

    cause = __VIRegs[0x2E / 2];

    if (cause & 0x8) {
        /* Clear interrupt */
        __VIRegs[0x2E / 2] = cause & ~0x8;

        CurrRetraceCnt++;

        /* Update frame buffer address */
        if (NextXFBAddr != 0) {
            CurrXFBAddr[0] = NextXFBAddr;
        }

        /* Call pre-retrace callback */
        if (PreRetraceCB != NULL) {
            OSClearContext(&tempCtx);
            OSSetCurrentContext(&tempCtx);
            PreRetraceCB(CurrRetraceCnt);
            OSClearContext(&tempCtx);
            OSSetCurrentContext(context);
        }
    }

    /* Call post-retrace callback */
    if (PostRetraceCB != NULL) {
        OSClearContext(&tempCtx);
        OSSetCurrentContext(&tempCtx);
        PostRetraceCB(CurrRetraceCnt);
        OSClearContext(&tempCtx);
        OSSetCurrentContext(context);
    }
}

/*
 * VIGetNextField - Get the next field (even/odd).
 * 0x800ABCF4 | size: 0x74
 */
u32 VIGetNextField(void) {
    u32 halfLine = __VIRegs[0x2C / 2];
    return halfLine & 1;
}

/*
 * VIGetDTVStatus - Get DTV status.
 * 0x800ABD68 | size: 0x194
 */
u32 VIGetDTVStatus(void) {
    return FlatPanelMode;
}

/*
 * VISetNextField - Set the next field to display.
 * 0x800ABEFC | size: 0x60
 */
void VISetNextField(u32 field) {
    /* Not commonly used - stub for link compatibility */
}

/*
 * setInterruptRegs - Configure VI interrupt registers.
 */
static void setInterruptRegs(void) {
    /* Internal helper for VI timing configuration */
}

/*
 * setScalingRegs - Configure VI scaling.
 */
static void setScalingRegs(u16 panelWidth, u16 dispWidth) {
    /* Internal helper for VI scaling configuration */
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800AA498 - 0x800AA498 | size: 0x3C */
#if 0
asm void fn_800AA498(void) {
#include "src/dolphin/vi/VIFull_fn_800AA498.inc"
}
#else
u32 fn_800AA498(void) {
    BOOL enabled;
    u32 val;

    enabled = OSDisableInterrupts();
    val = *(volatile u16*)0xCC00206E & 0x3;
    OSRestoreInterrupts(enabled);
    return val & 0x1;
}
#endif

/* fn_800AA4D4 - 0x800AA4D4 | size: 0x1A4 */
extern u32 SIGetType(s32 chan);
extern u8 lbl_803FC5E0[];
extern u32 lbl_80478A14;
extern u32 lbl_80478A10;
#if 1
asm void fn_800AA4D4(void) {
#include "src/dolphin/vi/VIFull_fn_800AA4D4.inc"
}
#else
void fn_800AA4D4(s32 chan) {
    u8* base;
    u32 mode;
    u32 chanBit;

    base = &lbl_803FC5E0[chan * 12];
    mode = lbl_80478A14 & 0x700;
    chanBit = 0x80000000u >> chan;
    switch (mode) {
    case 0x000:
    case 0x500:
    case 0x600:
    case 0x700:
        base[6] &= 0xF0;
        base[7] &= 0xF0;
        base[8] &= 0xF0;
        base[9] &= 0xF0;
        break;
    case 0x100:
        base[4] &= 0xF0;
        base[5] &= 0xF0;
        base[8] &= 0xF0;
        base[9] &= 0xF0;
        break;
    case 0x200:
        base[4] &= 0xF0;
        base[5] &= 0xF0;
        base[6] &= 0xF0;
        base[7] &= 0xF0;
        break;
    default:
        break;
    }
    base[2] = (u8)(base[2] - 0x80);
    base[3] = (u8)(base[3] - 0x80);
    base[4] = (u8)(base[4] - 0x80);
    base[5] = (u8)(base[5] - 0x80);
    if (lbl_80478A10 & chanBit) {
        if ((s8)base[2] > 64) {
            if ((SIGetType(chan) & ~0xFFFF) == 0x09000000u) {
                base[2] = 0;
            }
        }
    }
}
#endif

/* fn_800AA678 - 0x800AA678 | size: 0xC4 */
extern void fn_800D05A4(u32 chan, void* buf);
extern void fn_800D0338(u32 chan, u32 cmd);
extern void fn_800D03C8(u32 mask);
extern void fn_800D0CBC(u32 chan, void* callback);
extern void fn_800AA4D4();
extern void PADTypeAndStatusCallback(void);
extern void* memset(void* dst, int val, u32 n);
extern u32 lbl_80478A0C;
extern u32 lbl_8047A8A4;
extern u32 lbl_80478A14;
extern u32 lbl_8047A8A8;
extern u8 lbl_803FC5E0[];
#if 1
asm void fn_800AA678(void) {
#include "src/dolphin/vi/VIFull_fn_800AA678.inc"
}
#else
void fn_800AA678(u32 chan, u32 status) {
    u32 buf[4];
    u32 curChan;
    u32 pending;
    u32 nextChan;

    if ((status & 0xF) == 0) {
        fn_800AA4D4(lbl_80478A0C);
        curChan = lbl_80478A0C;
        lbl_8047A8A4 |= (0x80000000u >> curChan);
        fn_800D05A4(curChan, buf);
        fn_800D0338(curChan, lbl_80478A14 | 0x00400000);
        fn_800D03C8(lbl_8047A8A4);
    }
    pending = lbl_8047A8A8;
    nextChan = __cntlzw(pending);
    lbl_80478A0C = nextChan;
    if (nextChan != 32) {
        lbl_8047A8A8 = pending & ~(0x80000000u >> nextChan);
        memset(&lbl_803FC5E0[nextChan * 0xC], 0, 0xC);
        fn_800D0CBC(lbl_80478A0C, PADTypeAndStatusCallback);
    }
}
#endif

/* fn_800AA73C - 0x800AA73C | size: 0xC0 */
extern void fn_800D0464(u32 mask);
extern void fn_800A115C(u32 chan, u32 val);
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8B8;
#if 1
asm void fn_800AA73C(void) {
#include "src/dolphin/vi/VIFull_fn_800AA73C.inc"
}
#else
void fn_800AA73C(s32 chan, s32 status) {
    u32 chanBit;
    u32 mask;
    BOOL intr;

    chanBit = 0x80000000u >> chan;
    if (lbl_8047A8A4 & chanBit) {
        if (!(status & 0xF)) {
            fn_800AA4D4(chan);
        }
        if (status & 0x8) {
            intr = OSDisableInterrupts();
            fn_800D0464(chanBit);
            mask = ~chanBit;
            lbl_8047A8A4 = lbl_8047A8A4 & mask;
            lbl_8047A8B0 = lbl_8047A8B0 & mask;
            lbl_8047A8B4 = lbl_8047A8B4 & mask;
            lbl_8047A8B8 = lbl_8047A8B8 & mask;
            fn_800A115C(chan, 0);
            OSRestoreInterrupts(intr);
        }
    }
}
#endif

/* fn_800AA7FC - 0x800AA7FC | size: 0xD8 */
extern u32 lbl_80478A0C;
extern u32 lbl_8047A8A4;
extern u32 lbl_80478A14;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8A8;
#if 1
asm void fn_800AA7FC(void) {
#include "src/dolphin/vi/VIFull_fn_800AA7FC.inc"
}
#else
void fn_800AA7FC(void) {
    u8 sp[0x30];
    extern u8 lbl_803FC5E0[];
    extern u32 lbl_80478A0C;
    extern u32 lbl_80478A14;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8B0;
    extern void fn_800D0338();
    extern void fn_800D03C8();
    extern void fn_800D05A4();
    extern void fn_800D0CBC();
    extern void PADTypeAndStatusCallback();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    tmp = r4 & 0xF;
    if ((s32)tmp == 0) {
        r30 = lbl_80478A0C;
        r31 = 0x80000000;
        r3 = lbl_8047A8A4;
        r4 = (u32)sp + 0x1c;
        tmp = (u32)r31 >> r30;
        tmp = r3 | tmp;
        lbl_8047A8A4 = tmp;
        r3 = r30;
        fn_800D05A4();
        tmp = lbl_80478A14;
        r3 = r30 + 0x0;
        r4 = tmp | (0x40 << 16);
        fn_800D0338();
        r3 = lbl_8047A8A4;
        fn_800D03C8();
        tmp = lbl_80478A0C;
        r3 = lbl_8047A8B0;
        tmp = (u32)r31 >> tmp;
        tmp = r3 | tmp;
        lbl_8047A8B0 = tmp;
    }
    r5 = lbl_8047A8A8;
    tmp = __cntlzw(r5);
    lbl_80478A0C = tmp;
    r4 = lbl_80478A0C;
    if ((s32)r4 != 0x20) {
        tmp = 0x80000000;
        tmp = (u32)tmp >> r4;
        tmp = r5 & ~tmp;
        r4 = r4 * 0xc;
        lbl_8047A8A8 = tmp;
        r3 = (u32)lbl_803FC5E0;
        tmp = (u32)lbl_803FC5E0;
        r3 = tmp + r4;
        r4 = 0x0;
        r5 = 0xc;
        memset((void*)r3, (int)r4, (u32)r5);
        r4 = (u32)PADTypeAndStatusCallback;
        r3 = lbl_80478A0C;
        r4 = (u32)PADTypeAndStatusCallback;
        fn_800D0CBC();
    }
    return;
}
#endif

/* PADTypeAndStatusCallback - 0x800AA8D4 | size: 0x32C */
extern void SITransfer();
extern u8 lbl_803FC5D0[];
extern u32 lbl_80478A0C;
extern u32 lbl_8047A8AC;
extern u32 lbl_8047A8A8;
extern u32 lbl_80478A18;
extern u32 lbl_8047A8A4;
extern u32 lbl_80478A14;
extern char lbl_80478A24;
extern char lbl_80478A20;
extern u32 lbl_8047A8B8;
#if 1
asm void PADTypeAndStatusCallback(void) {
#include "src/dolphin/vi/VIFull_PADTypeAndStatusCallback.inc"
}
#else
void PADTypeAndStatusCallback(void) {
    u8 sp[0x30];
    extern u8 lbl_803FC5D0[];
    extern u32 lbl_80478A0C;
    extern u32 lbl_80478A14;
    extern u32 lbl_80478A18;
    extern char lbl_80478A20;
    extern char lbl_80478A24;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8AC;
    extern u32 lbl_8047A8B8;
    extern void fn_800D0338();
    extern void fn_800D03C8();
    extern void fn_800D05A4();
    extern void fn_800D0CBC();
    extern void fn_800AA678();
    extern void fn_800AA7FC();
    extern void PADTypeAndStatusCallback();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r6 = (u32)lbl_803FC5D0;
    tmp = r4 & 0xF;
    r31 = 0x80000000;
    r30 = (u32)lbl_803FC5D0;
    r29 = lbl_80478A0C;
    r5 = lbl_8047A8AC;
    r28 = (u32)r31 >> r29;
    r3 = r5 & ~r28;
    lbl_8047A8AC = r3;
    r5 = r5 & r28;
    r3 = 0x1;
    if ((s32)tmp != 0) {
        r4 = lbl_8047A8A8;
        tmp = __cntlzw(r4);
        lbl_80478A0C = tmp;
        r3 = lbl_80478A0C;
        if ((s32)r3 == 0x20) return;
        tmp = r3 * 0xc;
        r3 = (u32)r31 >> r3;
        r4 = r4 & ~r3;
        r3 = r30 + tmp;
        lbl_8047A8A8 = r4;
        r4 = 0x0;
        r5 = 0xc;
        r3 = r3 + 0x10;
        memset((void*)r3, (int)r4, (u32)r5);
        r4 = (u32)PADTypeAndStatusCallback;
        r3 = lbl_80478A0C;
        r4 = (u32)PADTypeAndStatusCallback;
        fn_800D0CBC();
        return;
    }
    /* clrrwi r6, r4, 8 */;
    r4 = r4 & 0x18000000;
    /* subis tmp, r4, 0x800 */;
    r4 = r29 << 2;
    *(u32*)(r30 + r4) = r6;
    if (tmp == 0) {
        tmp = r6 & 0x01000000;
        if (tmp == 0) {
        }
        r5 = lbl_8047A8A8;
        tmp = __cntlzw(r5);
        lbl_80478A0C = tmp;
        r4 = lbl_80478A0C;
        if ((s32)r4 == 0x20) return;
        r3 = 0x80000000;
        tmp = r4 * 0xc;
        r3 = (u32)r3 >> r4;
        r4 = r5 & ~r3;
        r3 = r30 + tmp;
        lbl_8047A8A8 = r4;
        r4 = 0x0;
        r5 = 0xc;
        r3 = r3 + 0x10;
        memset((void*)r3, (int)r4, (u32)r5);
        r4 = (u32)PADTypeAndStatusCallback;
        r3 = lbl_80478A0C;
        r4 = (u32)PADTypeAndStatusCallback;
        fn_800D0CBC();
        return;
        }
    tmp = lbl_80478A18;
    if (tmp < 2) {
        tmp = lbl_8047A8A4;
        r3 = r29 + 0x0;
        r4 = (u32)sp + 0x1c;
        tmp = tmp | r28;
        lbl_8047A8A4 = tmp;
        fn_800D05A4();
        tmp = lbl_80478A14;
        r3 = r29 + 0x0;
        r4 = tmp | (0x40 << 16);
        fn_800D0338();
        r3 = lbl_8047A8A4;
        fn_800D03C8();
        r4 = lbl_8047A8A8;
        tmp = __cntlzw(r4);
        lbl_80478A0C = tmp;
        r3 = lbl_80478A0C;
        if ((s32)r3 == 0x20) return;
        tmp = r3 * 0xc;
        r3 = (u32)r31 >> r3;
        r4 = r4 & ~r3;
        r3 = r30 + tmp;
        lbl_8047A8A8 = r4;
        r4 = 0x0;
        r5 = 0xc;
        r3 = r3 + 0x10;
        memset((void*)r3, (int)r4, (u32)r5);
        r4 = (u32)PADTypeAndStatusCallback;
        r3 = lbl_80478A0C;
        r4 = (u32)PADTypeAndStatusCallback;
        fn_800D0CBC();
        return;
    }
    /* clrrwi. tmp, r6, 31 */;
    if ((s32)r3 != 0x20) {
        tmp = r6 & 0x04000000;
        if ((s32)r3 != 0x20) {
        }
        if (r5 != 0) {
            tmp = r29 * 0xc;
            r3 = (u32)fn_800AA678;
            r6 = r30 + tmp;
            r8 = (u32)fn_800AA678;
            r3 = r29 + 0x0;
            r4 = (u32)&lbl_80478A24;
            r5 = 0x3;
            r7 = 0xa;
            r10 = 0x0;
            r9 = 0x0;
            r6 = r6 + 0x10;
            SITransfer();
            goto L_800AAB7C;
        }
        tmp = r29 * 0xc;
        r3 = (u32)fn_800AA678;
        r6 = r30 + tmp;
        r8 = (u32)fn_800AA678;
        r3 = r29 + 0x0;
        r4 = (u32)&lbl_80478A20;
        r5 = 0x1;
        r7 = 0xa;
        r10 = 0x0;
        r9 = 0x0;
        r6 = r6 + 0x10;
        SITransfer();

        } else {
        tmp = r6 & 0x00100000;
        if (r5 != 0) {
            tmp = r6 & 0x00080000;
            if (r5 == 0) {
                tmp = r6 & 0x00040000;
            }
            if (r5 != 0) goto L_800AAB7C;
            tmp = r6 & 0x40000000;
            if (r5 != 0) {
                tmp = r29 * 0xc;
                r3 = (u32)fn_800AA678;
                r6 = r30 + tmp;
                r8 = (u32)fn_800AA678;
                r3 = r29 + 0x0;
                r4 = (u32)&lbl_80478A20;
                r5 = 0x1;
                r7 = 0xa;
                r10 = 0x0;
                r9 = 0x0;
                r6 = r6 + 0x10;
                SITransfer();
                goto L_800AAB7C;
            }
            tmp = r29 * 0xc;
            r3 = (u32)fn_800AA7FC;
            r4 = r30 + r4;
            r6 = r30 + tmp;
            r8 = (u32)fn_800AA7FC;
            r3 = r29 + 0x0;
            r5 = 0x3;
            r7 = 0x8;
            r10 = 0x0;
            r9 = 0x0;
            r4 = r4 + 0x40;
            r6 = r6 + 0x10;
            SITransfer();
        }
        }
L_800AAB7C:
    if ((s32)r3 != 0) return;
    r5 = lbl_8047A8A8;
    r3 = lbl_8047A8B8;
    tmp = __cntlzw(r5);
    lbl_80478A0C = tmp;
    tmp = r3 | r28;
    r4 = lbl_80478A0C;
    lbl_8047A8B8 = tmp;
    if ((s32)r4 == 0x20) return;
    r3 = 0x80000000;
    tmp = r4 * 0xc;
    r3 = (u32)r3 >> r4;
    r4 = r5 & ~r3;
    r3 = r30 + tmp;
    lbl_8047A8A8 = r4;
    r4 = 0x0;
    r5 = 0xc;
    r3 = r3 + 0x10;
    memset((void*)r3, (int)r4, (u32)r5);
    r4 = (u32)PADTypeAndStatusCallback;
    r3 = lbl_80478A0C;
    r4 = (u32)PADTypeAndStatusCallback;
    fn_800D0CBC();

    return;
}
#endif

/* fn_800AAC00 - 0x800AAC00 | size: 0x134 */
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8B8;
#if 1
asm void fn_800AAC00(void) {
#include "src/dolphin/vi/VIFull_fn_800AAC00.inc"
}
#else
void fn_800AAC00(void) {
    extern u8 lbl_803FC5E0[];
    extern char lbl_80478A20;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8B0;
    extern u32 lbl_8047A8B4;
    extern u32 lbl_8047A8B8;
    extern void fn_800A115C();
    extern void fn_800D0464();
    extern void fn_800AA73C();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r3;
    r3 = 0x80000000;
    r29 = (u32)r3 >> r31;
    tmp = lbl_8047A8A4;
    /* and. tmp, tmp, r29 */;
    if ((s32)tmp == 0) return;
    r5 = lbl_8047A8B0;
    r6 = ~(r29 | r29);
    r3 = lbl_8047A8B4;
    tmp = r4 & 0xF;
    r5 = r5 & r6;
    r3 = r3 & r6;
    lbl_8047A8B0 = r5;
    lbl_8047A8B4 = r3;
    /* clrrwi r3, r4, 8 */;
    if ((s32)tmp == 0) {
        /* clrrwi. tmp, r3, 31 */;
        if ((s32)tmp != 0) {
            tmp = r3 & 0x00100000;
            if ((s32)tmp != 0) {
                tmp = r3 & 0x40000000;
                if ((s32)tmp != 0) {
                    tmp = r3 & 0x04000000;
                    if ((s32)tmp == 0) {
                        tmp = r3 & 0x00080000;
                        if ((s32)tmp == 0) {
                            tmp = r3 & 0x00040000;
                            if ((s32)tmp == 0) {
                                r4 = r31 * 0xc;
                                r3 = (u32)lbl_803FC5E0;
                                tmp = (u32)lbl_803FC5E0;
                                r3 = (u32)fn_800AA73C;
                                r6 = tmp + r4;
                                r8 = (u32)fn_800AA73C;
                                r3 = r31 + 0x0;
                                r4 = (u32)&lbl_80478A20;
                                r5 = 0x1;
                                r7 = 0xa;
                                r10 = 0x0;
                                r9 = 0x0;
                                SITransfer();
                                return;
    }
    }
    }
    }
    }
    }
    }
    OSDisableInterrupts();
    r30 = r3 + 0x0;
    r3 = r29 + 0x0;
    fn_800D0464();
    r6 = lbl_8047A8A4;
    r8 = ~(r29 | r29);
    r5 = lbl_8047A8B0;
    r3 = r31;
    r4 = lbl_8047A8B4;
    tmp = lbl_8047A8B8;
    r7 = r6 & r8;
    r6 = r5 & r8;
    lbl_8047A8A4 = r7;
    r5 = r4 & r8;
    tmp = tmp & r8;
    lbl_8047A8B0 = r6;
    r4 = 0x0;
    lbl_8047A8B4 = r5;
    lbl_8047A8B8 = tmp;
    fn_800A115C();
    r3 = r30;
    OSRestoreInterrupts(r3);

    return;
}
#endif

/* fn_800AAD34 - 0x800AAD34 | size: 0x100 */
extern u32 lbl_8047A8B8;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8A8;
extern u32 lbl_80478A18;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8AC;
extern u32 lbl_80478A0C;
#if 1
asm void fn_800AAD34(void) {
#include "src/dolphin/vi/VIFull_fn_800AAD34.inc"
}
#else
void fn_800AAD34(void) {
    extern u8 lbl_803FC5E0[];
    extern u32 lbl_80478A0C;
    extern u32 lbl_80478A18;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8AC;
    extern u32 lbl_8047A8B0;
    extern u32 lbl_8047A8B4;
    extern u32 lbl_8047A8B8;
    extern void fn_800D0464();
    extern void fn_800D0CBC();
    extern void PADTypeAndStatusCallback();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    OSDisableInterrupts();
    r4 = lbl_8047A8B8;
    r7 = 0x0;
    r5 = lbl_8047A8B0;
    r31 = r3 + 0x0;
    tmp = lbl_8047A8B4;
    r30 = r30 | r4;
    r4 = lbl_8047A8A8;
    r5 = r5 | tmp;
    tmp = lbl_80478A18;
    r30 = r30 & ~r5;
    r5 = lbl_8047A8A4;
    r6 = r4 | r30;
    lbl_8047A8B8 = r7;
    r4 = r5 & ~r30;
    lbl_8047A8A8 = r6;
    tmp = lbl_8047A8A8;
    lbl_8047A8A4 = r4;
    r3 = tmp & r5;
    if (tmp == 4) {
        tmp = lbl_8047A8AC;
        tmp = tmp | r30;
        lbl_8047A8AC = tmp;
    }
    fn_800D0464();
    tmp = lbl_80478A0C;
    if ((s32)tmp == 0x20) {
        r5 = lbl_8047A8A8;
        tmp = __cntlzw(r5);
        lbl_80478A0C = tmp;
        r4 = lbl_80478A0C;
        if ((s32)r4 != 0x20) {
            tmp = 0x80000000;
            tmp = (u32)tmp >> r4;
            tmp = r5 & ~tmp;
            r4 = r4 * 0xc;
            lbl_8047A8A8 = tmp;
            r3 = (u32)lbl_803FC5E0;
            tmp = (u32)lbl_803FC5E0;
            r3 = tmp + r4;
            r4 = 0x0;
            r5 = 0xc;
            memset((void*)r3, (int)r4, (u32)r5);
            r4 = (u32)PADTypeAndStatusCallback;
            r3 = lbl_80478A0C;
            r4 = (u32)PADTypeAndStatusCallback;
            fn_800D0CBC();
    }
    }
    r3 = r31;
    OSRestoreInterrupts(r3);
    r3 = 0x1;
    return;
}
#endif

/* fn_800AAE34 - 0x800AAE34 | size: 0x104 */
extern u32 lbl_8047A8B8;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8A8;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8AC;
extern u32 lbl_80478A0C;
#if 1
asm void fn_800AAE34(void) {
#include "src/dolphin/vi/VIFull_fn_800AAE34.inc"
}
#else
void fn_800AAE34(void) {
    extern u8 lbl_803FC5E0[];
    extern u32 lbl_80478A0C;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8AC;
    extern u32 lbl_8047A8B0;
    extern u32 lbl_8047A8B4;
    extern u32 lbl_8047A8B8;
    extern void fn_800D0464();
    extern void fn_800D0CBC();
    extern void PADTypeAndStatusCallback();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3;
    OSDisableInterrupts();
    r6 = lbl_8047A8B8;
    r7 = 0x0;
    r5 = lbl_8047A8B0;
    r4 = 0x80000000;
    tmp = lbl_8047A8B4;
    r30 = r30 | r6;
    r6 = lbl_8047A8A8;
    r5 = r5 | tmp;
    tmp = *(u8*)((u8*)r4 + 0x30E3);
    r30 = r30 & ~r5;
    r5 = lbl_8047A8A4;
    r4 = r6 | r30;
    lbl_8047A8B8 = r7;
    tmp = tmp & 0x00000040;
    lbl_8047A8A8 = r4;
    r4 = r5 & ~r30;
    r31 = r3 + 0x0;
    r6 = lbl_8047A8A8;
    lbl_8047A8A4 = r4;
    r3 = r6 & r5;
    if ((s32)tmp == 0) {
        tmp = lbl_8047A8AC;
        tmp = tmp | r30;
        lbl_8047A8AC = tmp;
    }
    fn_800D0464();
    tmp = lbl_80478A0C;
    if ((s32)tmp == 0x20) {
        r5 = lbl_8047A8A8;
        tmp = __cntlzw(r5);
        lbl_80478A0C = tmp;
        r4 = lbl_80478A0C;
        if ((s32)r4 != 0x20) {
            tmp = 0x80000000;
            tmp = (u32)tmp >> r4;
            tmp = r5 & ~tmp;
            r4 = r4 * 0xc;
            lbl_8047A8A8 = tmp;
            r3 = (u32)lbl_803FC5E0;
            tmp = (u32)lbl_803FC5E0;
            r3 = tmp + r4;
            r4 = 0x0;
            r5 = 0xc;
            memset((void*)r3, (int)r4, (u32)r5);
            r4 = (u32)PADTypeAndStatusCallback;
            r3 = lbl_80478A0C;
            r4 = (u32)PADTypeAndStatusCallback;
            fn_800D0CBC();
    }
    }
    r3 = r31;
    OSRestoreInterrupts(r3);
    r3 = 0x1;
    return;
}
#endif

/* fn_800AAF38 - 0x800AAF38 | size: 0x218 */
extern void fn_800C4C98(void);
extern void fn_800D104C(void);
extern void OSRegisterResetFunction();
extern u32 lbl_8047A8A0;
extern u32 lbl_80478A08;
extern u32 lbl_8047AA58;
extern u32 lbl_8047A8AC;
extern u8 lbl_80312500[];
extern u32 lbl_8047A8B8;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8A8;
extern u32 lbl_80478A18;
extern u32 lbl_8047A8A4;
extern u32 lbl_80478A0C;
extern u32 __PADSpec;
#if 1
asm void fn_800AAF38(void) {
#include "src/dolphin/vi/VIFull_fn_800AAF38.inc"
}
#else
void fn_800AAF38(void) {
    extern u8 lbl_80312500[];
    extern u8 lbl_803FC5D0[];
    extern u32 lbl_80478A08;
    extern u32 lbl_80478A0C;
    extern u32 lbl_80478A18;
    extern u32 lbl_8047A8A0;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8AC;
    extern u32 lbl_8047A8B0;
    extern u32 lbl_8047A8B4;
    extern u32 lbl_8047A8B8;
    extern u32 lbl_8047AA58;
    extern void fn_800AB5B4();
    extern void fn_800C4C98();
    extern void fn_800D0464();
    extern void fn_800D0CBC();
    extern void fn_800D104C();
    extern void PADTypeAndStatusCallback();
    extern u32 __PADSpec;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    r3 = (u32)lbl_803FC5D0;
    r31 = (u32)lbl_803FC5D0;
    tmp = lbl_8047A8A0;
    if ((s32)tmp != 0) {
        r3 = 0x1;
    } else {

        r3 = lbl_80478A08;
        OSRegisterVersion((const char*)r3);
        r3 = *(u32*)__PADSpec;
        if (r3 != 0) {
            fn_800AB5B4();
        }
        tmp = lbl_8047AA58;
        r3 = 0x1;
        lbl_8047A8A0 = r3;
        if (tmp != 0) {
            OSGetTime();
            r25 = r4 + 0x0;
            r26 = r3 + 0x0;
            r5 = 0x10;
            fn_800C4C98();
            r5 = 0x10000;
            r28 = 0x0;
            r6 = r4 & r27;
            r4 = r25 & r27;
            r5 = r3 & r28;
            tmp = r26 & r28;
            r29 = r4 + r6;
            r3 = r26 + 0x0;
            r4 = r25 + 0x0;
            r30 = tmp + r5; /* +carry */;
            r5 = 0x20;
            fn_800C4C98();
            r4 = r4 & r27;
            tmp = r3 & r28;
            r29 = r4 + r29;
            r3 = r26 + 0x0;
            r4 = r25 + 0x0;
            r30 = tmp + r30; /* +carry */;
            r5 = 0x30;
            fn_800C4C98();
            tmp = r4 & r27;
            r5 = tmp + r29;
            tmp = 0xF0000000;
            r4 = 0x3fff;
            lbl_8047A8AC = tmp;
            tmp = r5 & r4;
            r3 = 0x80000000;
            *(u16*)((u8*)r3 + 0x30E0) = tmp;
        }
        r3 = 0x80000000;
        tmp = *(u16*)((u8*)r3 + 0x30E0);
        tmp = tmp | (0x4d00 << 16);
        *(u32*)((u8*)r31 + 0x40) = tmp;
        tmp = *(u16*)((u8*)r3 + 0x30E0);
        tmp = tmp | (0x4d40 << 16);
        *(u32*)((u8*)r31 + 0x44) = tmp;
        tmp = *(u16*)((u8*)r3 + 0x30E0);
        tmp = tmp | (0x4d80 << 16);
        *(u32*)((u8*)r31 + 0x48) = tmp;
        tmp = *(u16*)((u8*)r3 + 0x30E0);
        tmp = tmp | (0x4dc0 << 16);
        *(u32*)((u8*)r31 + 0x4C) = tmp;
        fn_800D104C();
        r3 = (u32)lbl_80312500;
        r3 = (u32)lbl_80312500;
        OSRegisterResetFunction();
        r28 = 0xF0000000;
        OSDisableInterrupts();
        r4 = lbl_8047A8B8;
        r7 = 0x0;
        r5 = lbl_8047A8B0;
        r27 = r3 + 0x0;
        tmp = lbl_8047A8B4;
        r28 = r28 | r4;
        r4 = lbl_8047A8A8;
        r5 = r5 | tmp;
        tmp = lbl_80478A18;
        r28 = r28 & ~r5;
        r5 = lbl_8047A8A4;
        r6 = r4 | r28;
        lbl_8047A8B8 = r7;
        r4 = r5 & ~r28;
        lbl_8047A8A8 = r6;
        tmp = lbl_8047A8A8;
        lbl_8047A8A4 = r4;
        r3 = tmp & r5;
        if (tmp == 4) {
            tmp = lbl_8047A8AC;
            tmp = tmp | r28;
            lbl_8047A8AC = tmp;
        }
        fn_800D0464();
        tmp = lbl_80478A0C;
        if ((s32)tmp == 0x20) {
            r5 = lbl_8047A8A8;
            tmp = __cntlzw(r5);
            lbl_80478A0C = tmp;
            r4 = lbl_80478A0C;
            if ((s32)r4 != 0x20) {
                r3 = 0x80000000;
                tmp = r4 * 0xc;
                r3 = (u32)r3 >> r4;
                r4 = r5 & ~r3;
                r3 = r31 + tmp;
                lbl_8047A8A8 = r4;
                r4 = 0x0;
                r5 = 0xc;
                r3 = r3 + 0x10;
                memset((void*)r3, (int)r4, (u32)r5);
                r4 = (u32)PADTypeAndStatusCallback;
                r3 = lbl_80478A0C;
                r4 = (u32)PADTypeAndStatusCallback;
                fn_800D0CBC();
        }
        }
        r3 = r27;
        OSRestoreInterrupts(r3);
        r3 = 0x1;
    }
    return;
}
#endif

/* fn_800AB150 - 0x800AB150 | size: 0x3AC */
extern void fn_800CF728(void);
extern void fn_800D02BC(void);
extern u32 lbl_8047A8B8;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8A8;
extern u32 lbl_8047A8A4;
extern u32 lbl_80478A18;
extern u32 lbl_8047A8AC;
extern u32 lbl_80478A0C;
extern u32 lbl_80478A1C;
#if 1
asm void fn_800AB150(void) {
#include "src/dolphin/vi/VIFull_fn_800AB150.inc"
}
#else
void fn_800AB150(void) {
    u8 sp[0x20];
    extern u8 lbl_803FC5E0[];
    extern u32 lbl_80478A0C;
    extern u32 lbl_80478A18;
    extern u32 lbl_80478A1C;
    extern char lbl_80478A20;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8AC;
    extern u32 lbl_8047A8B0;
    extern u32 lbl_8047A8B4;
    extern u32 lbl_8047A8B8;
    extern void fn_800A115C();
    extern void fn_800CF728();
    extern void fn_800D02BC();
    extern void fn_800D0464();
    extern void fn_800D05A4();
    extern void fn_800D0CBC();
    extern void fn_800AA73C();
    extern void PADTypeAndStatusCallback();
    extern void fn_800AAC00();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r12 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r3;
    OSDisableInterrupts();
    r21 = 0x0;
    tmp = r21 * 0xc;
    r4 = (u32)lbl_803FC5E0;
    r26 = (u32)lbl_803FC5E0;
    r4 = (u32)fn_800AA73C;
    r5 = (u32)fn_800AAC00;
    r6 = (u32)PADTypeAndStatusCallback;
    r24 = r26 + tmp;
    r22 = r3 + 0x0;
    r30 = (u32)fn_800AA73C;
    r29 = (u32)fn_800AAC00;
    r28 = (u32)PADTypeAndStatusCallback;
    r20 = 0x0;
    r27 = 0x80000000;
    do {
    do {
        tmp = lbl_8047A8B8;
        r23 = (u32)r27 >> r21;
        /* and. tmp, tmp, r23 */;
        if ((s32)tmp != 0) {
            OSDisableInterrupts();
            r4 = lbl_8047A8B0;
            r6 = 0x0;
            tmp = lbl_8047A8B4;
            r25 = r3;
            r5 = lbl_8047A8B8;
            r4 = r4 | tmp;
            tmp = lbl_8047A8A8;
            r7 = r5 & ~r4;
            r5 = lbl_8047A8A4;
            r4 = tmp | r7;
            tmp = lbl_80478A18;
            lbl_8047A8A8 = r4;
            r4 = r5 & ~r7;
            tmp = lbl_8047A8A8;
            lbl_8047A8B8 = r6;
            r3 = tmp & r5;
            lbl_8047A8A4 = r4;
            if (tmp == 4) {
                tmp = lbl_8047A8AC;
                tmp = tmp | r7;
                lbl_8047A8AC = tmp;
            }
            fn_800D0464();
            tmp = lbl_80478A0C;
            if ((s32)tmp == 0x20) {
                r4 = lbl_8047A8A8;
                tmp = __cntlzw(r4);
                lbl_80478A0C = tmp;
                r3 = lbl_80478A0C;
                if ((s32)r3 != 0x20) {
                    tmp = r3 * 0xc;
                    r3 = (u32)r27 >> r3;
                    r4 = r4 & ~r3;
                    lbl_8047A8A8 = r4;
                    r3 = r26 + tmp;
                    r4 = 0x0;
                    r5 = 0xc;
                    memset((void*)r3, (int)r4, (u32)r5);
                    r3 = lbl_80478A0C;
                    r4 = r28;
                    fn_800D0CBC();
            }
            }
            r3 = r25;
            OSRestoreInterrupts(r3);
            tmp = -0x2;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
        }
        tmp = lbl_8047A8A8;
        /* and. tmp, tmp, r23 */;
        if ((s32)r3 == 0x20) {
            tmp = lbl_80478A0C;
            if ((s32)tmp == (s32)r21) {
            }
            tmp = -0x2;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
            }
        tmp = lbl_8047A8A4;
        /* and. tmp, tmp, r23 */;
        if ((s32)tmp == (s32)r21) {
            tmp = -0x1;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
        }
        r3 = r21;
        fn_800CF728();
        if ((s32)r3 != 0) {
            tmp = -0x3;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
        }
        r3 = r21;
        fn_800D02BC();
        tmp = r3 & 0x00000008;
        if ((s32)r3 != 0) {
            r3 = r21 + 0x0;
            r4 = (u32)sp + 0x14;
            fn_800D05A4();
            tmp = lbl_8047A8B0;
            /* and. tmp, tmp, r23 */;
            if ((s32)r3 != 0) {
                tmp = 0x0;
                *(u8*)((u8*)r31 + 0xA) = tmp;
                r3 = r31 + 0x0;
                r4 = 0x0;
                r5 = 0xa;
                memset((void*)r3, (int)r4, (u32)r5);
                r3 = lbl_8047A8B4;
                /* and. tmp, r3, r23 */;
                if ((s32)r3 != 0) break;
                tmp = r3 | r23;
                lbl_8047A8B4 = tmp;
                r3 = r21 + 0x0;
                r4 = r29 + 0x0;
                fn_800D0CBC();
                break;
            }
            OSDisableInterrupts();
            r25 = r3 + 0x0;
            r3 = r23 + 0x0;
            fn_800D0464();
            r6 = lbl_8047A8A4;
            r8 = ~(r23 | r23);
            r5 = lbl_8047A8B0;
            r3 = r21;
            r4 = lbl_8047A8B4;
            tmp = lbl_8047A8B8;
            r7 = r6 & r8;
            r6 = r5 & r8;
            lbl_8047A8A4 = r7;
            r5 = r4 & r8;
            tmp = tmp & r8;
            lbl_8047A8B0 = r6;
            r4 = 0x0;
            lbl_8047A8B4 = r5;
            lbl_8047A8B8 = tmp;
            fn_800A115C();
            r3 = r25;
            OSRestoreInterrupts(r3);
            tmp = -0x1;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
        }
        r3 = r21;
        SIGetType();
        tmp = r3 & 0x20000000;
        if ((s32)r3 == 0) {
            r20 = r20 | r23;
        }
        r3 = r21 + 0x0;
        r4 = (u32)sp + 0x14;
        fn_800D05A4();
        if ((s32)r3 == 0) {
            tmp = -0x3;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
        }
        /* clrrwi. tmp, tmp, 31 */;
        if ((s32)r3 != 0) {
            tmp = -0x3;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            break;
        }
        r12 = lbl_80478A1C;
        r3 = r21 + 0x0;
        r4 = r31 + 0x0;
        r5 = (u32)sp + 0x14;
        /* blrl  */;
        tmp = *(u16*)((u8*)r31 + 0x0);
        tmp = tmp & 0x00002000;
        if ((s32)r3 != 0) {
            tmp = -0x3;
            *(u8*)((u8*)r31 + 0xA) = tmp;
            r3 = r31 + 0x0;
            r4 = 0x0;
            r5 = 0xa;
            memset((void*)r3, (int)r4, (u32)r5);
            r3 = r21 + 0x0;
            r6 = r24 + 0x0;
            r8 = r30 + 0x0;
            r4 = (u32)&lbl_80478A20;
            r5 = 0x1;
            r7 = 0xa;
            r10 = 0x0;
            r9 = 0x0;
            SITransfer();
            break;
        }
        tmp = 0x0;
        *(u8*)((u8*)r31 + 0xA) = tmp;
        tmp = *(u16*)((u8*)r31 + 0x0);
        tmp = tmp & 0xFFFFFF7F;
        *(u16*)((u8*)r31 + 0x0) = tmp;
    } while (0);
        r21 = r21 + 0x1;
        r24 = r24 + 0xc;
        r31 = r31 + 0xc;
    } while ((s32)r21 < 4);
    r3 = r22;
    OSRestoreInterrupts(r3);
    r3 = r20;
    return;
}
#endif

/* fn_800AB4FC - 0x800AB4FC | size: 0xB8 */
extern void fn_800D034C(void);
extern u32 lbl_8047A8A4;
extern u32 lbl_80478A18;
extern u32 lbl_80478A14;
#if 1
asm void fn_800AB4FC(void) {
#include "src/dolphin/vi/VIFull_fn_800AB4FC.inc"
}
#else
void fn_800AB4FC(u32* types) {
    BOOL enabled;
    s32 i;
    s32 changed;
    u32 type;
    u32 cmd;

    enabled = OSDisableInterrupts();
    changed = 0;
    i = 0;
    do {
        if (lbl_8047A8A4 & (0x80000000u >> i)) {
            if (!(SIGetType(i) & 0x20000000)) {
                type = types[0];
                if (lbl_80478A18 < 2u && type == 2u) {
                    type = 0;
                }
                cmd = lbl_80478A14 | 0x00400000;
                cmd |= (type & 0x3);
                fn_800D0338(i, cmd);
                changed = 1;
            }
        }
        i++;
        types++;
    } while (i < 4);
    if (changed) {
        fn_800D034C();
    }
    OSRestoreInterrupts(enabled);
}
#endif

/* fn_800AB5B4 - 0x800AB5B4 | size: 0x60 */
extern u32 lbl_80478A1C;
extern u32 lbl_80478A18;
extern u32 __PADSpec;
#if 0
asm void fn_800AB5B4(void) {
#include "src/dolphin/vi/VIFull_fn_800AB5B4.inc"
}
#else
void fn_800AB5B4(s32 spec) {
    __PADSpec = 0;
    switch (spec) {
    case 0:
        lbl_80478A1C = (u32)SPEC0_MakeStatus;
        break;
    case 1:
        lbl_80478A1C = (u32)SPEC1_MakeStatus;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        lbl_80478A1C = (u32)fn_800AB8FC;
        break;
    }
    lbl_80478A18 = spec;
}
#endif

/* fn_800AB614 - 0x800AB614 | size: 0x174 */
#if 1
asm void fn_800AB614(void) {
#include "src/dolphin/vi/VIFull_fn_800AB614.inc"
}
#else
void fn_800AB614(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;

    r3 = 0x0;
    *(u16*)((u8*)r4 + 0x0) = r3;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000008;
    if ((s32)tmp != 0) {
        r3 = 0x100;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000020;
    if ((s32)tmp != 0) {
        r3 = 0x200;
    } else {

        r3 = 0x0;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000100;
    if ((s32)tmp != 0) {
        r3 = 0x400;
    } else {

        r3 = 0x0;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    /* extrwi. tmp, tmp, 1, 15 */;
    if ((s32)tmp != 0) {
        r3 = 0x800;
    } else {

        r3 = 0x0;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000010;
    if ((s32)tmp != 0) {
        r6 = 0x1000;
    } else {

        r6 = 0x0;
    }
    r3 = *(u16*)((u8*)r4 + 0x0);
    tmp = 0x0;
    r3 = r3 | r6;
    *(u16*)((u8*)r4 + 0x0) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (u32)r3 >> 16;
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x2) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (u32)r3 >> 24;
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x3) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x4) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (u32)r3 >> 8;
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x5) = r3;
    r3 = *(u32*)((u8*)r5 + 0x0);
    /* extrwi r3, r3, 8, 16 */;
    *(u8*)((u8*)r4 + 0x6) = r3;
    r3 = *(u32*)((u8*)r5 + 0x0);
    *(u8*)((u8*)r4 + 0x7) = r3;
    *(u8*)((u8*)r4 + 0x8) = tmp;
    *(u8*)((u8*)r4 + 0x9) = tmp;
    tmp = *(u8*)((u8*)r4 + 0x6);
    if (tmp >= 0xaa) {
        tmp = *(u16*)((u8*)r4 + 0x0);
        tmp = tmp | 0x40;
        *(u16*)((u8*)r4 + 0x0) = tmp;
    }
    tmp = *(u8*)((u8*)r4 + 0x7);
    if (tmp >= 0xaa) {
        tmp = *(u16*)((u8*)r4 + 0x0);
        tmp = tmp | 0x20;
        *(u16*)((u8*)r4 + 0x0) = tmp;
    }
    r3 = *(u8*)((u8*)r4 + 0x2);
    *(u8*)((u8*)r4 + 0x2) = tmp;
    r3 = *(u8*)((u8*)r4 + 0x3);
    *(u8*)((u8*)r4 + 0x3) = tmp;
    r3 = *(u8*)((u8*)r4 + 0x4);
    *(u8*)((u8*)r4 + 0x4) = tmp;
    r3 = *(u8*)((u8*)r4 + 0x5);
    *(u8*)((u8*)r4 + 0x5) = tmp;
    return;
}
#endif

/* fn_800AB788 - 0x800AB788 | size: 0x174 */
#if 1
asm void fn_800AB788(void) {
#include "src/dolphin/vi/VIFull_fn_800AB788.inc"
}
#else
void fn_800AB788(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;

    r3 = 0x0;
    *(u16*)((u8*)r4 + 0x0) = r3;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000080;
    if ((s32)tmp != 0) {
        r3 = 0x100;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000100;
    if ((s32)tmp != 0) {
        r3 = 0x200;
    } else {

        r3 = 0x0;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000020;
    if ((s32)tmp != 0) {
        r3 = 0x400;
    } else {

        r3 = 0x0;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000010;
    if ((s32)tmp != 0) {
        r3 = 0x800;
    } else {

        r3 = 0x0;
    }
    tmp = *(u16*)((u8*)r4 + 0x0);
    tmp = tmp | r3;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x00000200;
    if ((s32)tmp != 0) {
        r6 = 0x1000;
    } else {

        r6 = 0x0;
    }
    r3 = *(u16*)((u8*)r4 + 0x0);
    tmp = 0x0;
    r3 = r3 | r6;
    *(u16*)((u8*)r4 + 0x0) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (u32)r3 >> 16;
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x2) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (u32)r3 >> 24;
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x3) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x4) = r3;
    r3 = *(u32*)((u8*)r5 + 0x4);
    r3 = (u32)r3 >> 8;
    r3 = (s8)r3;
    *(u8*)((u8*)r4 + 0x5) = r3;
    r3 = *(u32*)((u8*)r5 + 0x0);
    /* extrwi r3, r3, 8, 16 */;
    *(u8*)((u8*)r4 + 0x6) = r3;
    r3 = *(u32*)((u8*)r5 + 0x0);
    *(u8*)((u8*)r4 + 0x7) = r3;
    *(u8*)((u8*)r4 + 0x8) = tmp;
    *(u8*)((u8*)r4 + 0x9) = tmp;
    tmp = *(u8*)((u8*)r4 + 0x6);
    if (tmp >= 0xaa) {
        tmp = *(u16*)((u8*)r4 + 0x0);
        tmp = tmp | 0x40;
        *(u16*)((u8*)r4 + 0x0) = tmp;
    }
    tmp = *(u8*)((u8*)r4 + 0x7);
    if (tmp >= 0xaa) {
        tmp = *(u16*)((u8*)r4 + 0x0);
        tmp = tmp | 0x20;
        *(u16*)((u8*)r4 + 0x0) = tmp;
    }
    r3 = *(u8*)((u8*)r4 + 0x2);
    *(u8*)((u8*)r4 + 0x2) = tmp;
    r3 = *(u8*)((u8*)r4 + 0x3);
    *(u8*)((u8*)r4 + 0x3) = tmp;
    r3 = *(u8*)((u8*)r4 + 0x4);
    *(u8*)((u8*)r4 + 0x4) = tmp;
    r3 = *(u8*)((u8*)r4 + 0x5);
    *(u8*)((u8*)r4 + 0x5) = tmp;
    return;
}
#endif

/* fn_800AB8FC - 0x800AB8FC | size: 0x3F8 */
extern u32 lbl_80478A14;
#if 1
asm void fn_800AB8FC(void) {
#include "src/dolphin/vi/VIFull_fn_800AB8FC.inc"
}
#else
void fn_800AB8FC(void) {
    extern u8 lbl_803FC5E0[];
    extern u32 lbl_80478A14;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;

    tmp = *(u32*)((u8*)r5 + 0x0);
    /* extrwi tmp, tmp, 14, 2 */;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = (u32)tmp >> 8;
    tmp = (s8)tmp;
    *(u8*)((u8*)r4 + 0x2) = tmp;
    tmp = *(u32*)((u8*)r5 + 0x0);
    tmp = (s8)tmp;
    *(u8*)((u8*)r4 + 0x3) = tmp;
    tmp = lbl_80478A14;
    tmp = tmp & 0x00000700;
    if ((s32)tmp != 0x400) {
        if ((s32)tmp < 0x400) {
            if ((s32)tmp != 0x200) {
                if ((s32)tmp < 0x200) {
                    if ((s32)tmp != 0x100) {
                        if ((s32)tmp >= 0x100) goto L_800ABB08;
                        if ((s32)tmp != 0) {
                            goto L_800ABB08;
                        }
                        if ((s32)tmp != 0x300) {
                            goto L_800ABB08;
                        }
                        if ((s32)tmp != 0x600) {
                            if ((s32)tmp < 0x600) {
                        }
                            if ((s32)tmp == 0x500) goto L_800AB98C;
                            goto L_800ABB08;
                        }
                        if ((s32)tmp == 0x700) goto L_800AB98C;
                        goto L_800ABB08;
                    }
                L_800AB98C:
                    tmp = *(u32*)((u8*)r5 + 0x4);
                    tmp = (u32)tmp >> 24;
                    tmp = (s8)tmp;
                    *(u8*)((u8*)r4 + 0x4) = tmp;
                    tmp = *(u32*)((u8*)r5 + 0x4);
                    tmp = (u32)tmp >> 16;
                    tmp = (s8)tmp;
                    *(u8*)((u8*)r4 + 0x5) = tmp;
                    tmp = *(u32*)((u8*)r5 + 0x4);
                    tmp = ((tmp << 24) | ((u32)tmp >> 8)) & 0x000000F0;
                    *(u8*)((u8*)r4 + 0x6) = tmp;
                    tmp = *(u32*)((u8*)r5 + 0x4);
                    tmp = ((tmp << 28) | ((u32)tmp >> 4)) & 0x000000F0;
                    *(u8*)((u8*)r4 + 0x7) = tmp;
                    tmp = *(u32*)((u8*)r5 + 0x4);
                    tmp = tmp & 0x000000F0;
                    *(u8*)((u8*)r4 + 0x8) = tmp;
                    tmp = *(u32*)((u8*)r5 + 0x4);
                    *(u8*)((u8*)r4 + 0x9) = tmp;
                    goto L_800ABB08;
                        }
                tmp = *(u32*)((u8*)r5 + 0x4);
                tmp = ((tmp << 8) | ((u32)tmp >> 24)) & 0x000000F0;
                tmp = (s8)tmp;
                *(u8*)((u8*)r4 + 0x4) = tmp;
                tmp = *(u32*)((u8*)r5 + 0x4);
                tmp = ((tmp << 12) | ((u32)tmp >> 20)) & 0x000000F0;
                tmp = (s8)tmp;
                *(u8*)((u8*)r4 + 0x5) = tmp;
                tmp = *(u32*)((u8*)r5 + 0x4);
                /* extrwi tmp, tmp, 8, 8 */;
                *(u8*)((u8*)r4 + 0x6) = tmp;
                tmp = *(u32*)((u8*)r5 + 0x4);
                /* extrwi tmp, tmp, 8, 16 */;
                *(u8*)((u8*)r4 + 0x7) = tmp;
                tmp = *(u32*)((u8*)r5 + 0x4);
                tmp = tmp & 0x000000F0;
                *(u8*)((u8*)r4 + 0x8) = tmp;
                tmp = *(u32*)((u8*)r5 + 0x4);
                *(u8*)((u8*)r4 + 0x9) = tmp;
                goto L_800ABB08;
                }
            tmp = *(u32*)((u8*)r5 + 0x4);
            tmp = ((tmp << 8) | ((u32)tmp >> 24)) & 0x000000F0;
            tmp = (s8)tmp;
            *(u8*)((u8*)r4 + 0x4) = tmp;
            tmp = *(u32*)((u8*)r5 + 0x4);
            tmp = ((tmp << 12) | ((u32)tmp >> 20)) & 0x000000F0;
            tmp = (s8)tmp;
            *(u8*)((u8*)r4 + 0x5) = tmp;
            tmp = *(u32*)((u8*)r5 + 0x4);
            tmp = ((tmp << 16) | ((u32)tmp >> 16)) & 0x000000F0;
            *(u8*)((u8*)r4 + 0x6) = tmp;
            tmp = *(u32*)((u8*)r5 + 0x4);
            tmp = ((tmp << 20) | ((u32)tmp >> 12)) & 0x000000F0;
            *(u8*)((u8*)r4 + 0x7) = tmp;
            tmp = *(u32*)((u8*)r5 + 0x4);
            /* extrwi tmp, tmp, 8, 16 */;
            *(u8*)((u8*)r4 + 0x8) = tmp;
            tmp = *(u32*)((u8*)r5 + 0x4);
            *(u8*)((u8*)r4 + 0x9) = tmp;
                        } else {

            r6 = *(u32*)((u8*)r5 + 0x4);
            tmp = 0x0;
            r6 = (u32)r6 >> 24;
            r6 = (s8)r6;
            *(u8*)((u8*)r4 + 0x4) = r6;
            r6 = *(u32*)((u8*)r5 + 0x4);
            r6 = (u32)r6 >> 16;
            r6 = (s8)r6;
            *(u8*)((u8*)r4 + 0x5) = r6;
            r6 = *(u32*)((u8*)r5 + 0x4);
            /* extrwi r6, r6, 8, 16 */;
            *(u8*)((u8*)r4 + 0x6) = r6;
            r5 = *(u32*)((u8*)r5 + 0x4);
            *(u8*)((u8*)r4 + 0x7) = r5;
            *(u8*)((u8*)r4 + 0x8) = tmp;
            *(u8*)((u8*)r4 + 0x9) = tmp;
            goto L_800ABB08;
        }
        r6 = *(u32*)((u8*)r5 + 0x4);
        tmp = 0x0;
        r6 = (u32)r6 >> 24;
        r6 = (s8)r6;
        *(u8*)((u8*)r4 + 0x4) = r6;
        r6 = *(u32*)((u8*)r5 + 0x4);
        r6 = (u32)r6 >> 16;
        r6 = (s8)r6;
        *(u8*)((u8*)r4 + 0x5) = r6;
        *(u8*)((u8*)r4 + 0x6) = tmp;
        *(u8*)((u8*)r4 + 0x7) = tmp;
        tmp = *(u32*)((u8*)r5 + 0x4);
        /* extrwi tmp, tmp, 8, 16 */;
        *(u8*)((u8*)r4 + 0x8) = tmp;
        tmp = *(u32*)((u8*)r5 + 0x4);
        *(u8*)((u8*)r4 + 0x9) = tmp;
                        }
L_800ABB08:
do {
    r6 = *(u8*)((u8*)r4 + 0x2);
    r5 = (u32)lbl_803FC5E0;
    r3 = r3 * 0xc;
    *(u8*)((u8*)r4 + 0x2) = tmp;
    tmp = (u32)lbl_803FC5E0;
    r3 = tmp + r3;
    r5 = *(u8*)((u8*)r4 + 0x3);
    *(u8*)((u8*)r4 + 0x3) = tmp;
    r5 = *(u8*)((u8*)r4 + 0x4);
    *(u8*)((u8*)r4 + 0x4) = tmp;
    r5 = *(u8*)((u8*)r4 + 0x5);
    *(u8*)((u8*)r4 + 0x5) = tmp;
    r7 = *(u8*)((u8*)r3 + 0x2);
    r6 = *(u8*)((u8*)r4 + 0x2);
    tmp = (s8)r7;
    if ((s32)tmp > 0x700) {
        r5 = (s8)r7;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)tmp >= (s32)r5) break;
        r6 = r5;
        break;
    }
    tmp = (s8)r7;
    if ((s32)tmp < (s32)r5) {
        r5 = (s8)r7;
        tmp = r5 + 0x7f;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)r5 >= (s32)tmp) break;
        r6 = r5;
    }
} while (0);
do {
    r6 = r6 - r7;
    *(u8*)((u8*)r4 + 0x2) = r6;
    r7 = *(u8*)((u8*)r3 + 0x3);
    r6 = *(u8*)((u8*)r4 + 0x3);
    tmp = (s8)r7;
    if ((s32)r5 > (s32)tmp) {
        r5 = (s8)r7;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)tmp >= (s32)r5) break;
        r6 = r5;
        break;
    }
    tmp = (s8)r7;
    if ((s32)tmp < (s32)r5) {
        r5 = (s8)r7;
        tmp = r5 + 0x7f;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)r5 >= (s32)tmp) break;
        r6 = r5;
    }
} while (0);
do {
    r6 = r6 - r7;
    *(u8*)((u8*)r4 + 0x3) = r6;
    r7 = *(u8*)((u8*)r3 + 0x4);
    r6 = *(u8*)((u8*)r4 + 0x4);
    tmp = (s8)r7;
    if ((s32)r5 > (s32)tmp) {
        r5 = (s8)r7;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)tmp >= (s32)r5) break;
        r6 = r5;
        break;
    }
    tmp = (s8)r7;
    if ((s32)tmp < (s32)r5) {
        r5 = (s8)r7;
        tmp = r5 + 0x7f;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)r5 >= (s32)tmp) break;
        r6 = r5;
    }
} while (0);
do {
    r6 = r6 - r7;
    *(u8*)((u8*)r4 + 0x4) = r6;
    r7 = *(u8*)((u8*)r3 + 0x5);
    r6 = *(u8*)((u8*)r4 + 0x5);
    tmp = (s8)r7;
    if ((s32)r5 > (s32)tmp) {
        r5 = (s8)r7;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)tmp >= (s32)r5) break;
        r6 = r5;
        break;
    }
    tmp = (s8)r7;
    if ((s32)tmp < (s32)r5) {
        r5 = (s8)r7;
        tmp = r5 + 0x7f;
        r5 = (s8)tmp;
        tmp = (s8)r6;
        if ((s32)r5 >= (s32)tmp) break;
        r6 = r5;
    }
} while (0);
    r6 = r6 - r7;
    *(u8*)((u8*)r4 + 0x5) = r6;
    tmp = *(u8*)((u8*)r3 + 0x6);
    r5 = *(u8*)((u8*)r4 + 0x6);
    if (r5 < tmp) {
        r5 = tmp;
    }
    r5 = r5 - tmp;
    *(u8*)((u8*)r4 + 0x6) = r5;
    tmp = *(u8*)((u8*)r3 + 0x7);
    r3 = *(u8*)((u8*)r4 + 0x7);
    if (r3 < tmp) {
        r3 = tmp;
    }
    r3 = r3 - tmp;
    *(u8*)((u8*)r4 + 0x7) = r3;
    return;
}
#endif

/* fn_800ABCF4 - 0x800ABCF4 | size: 0x74 */
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_80478A14;
#if 1
asm void fn_800ABCF4(void) {
#include "src/dolphin/vi/VIFull_fn_800ABCF4.inc"
}
#else
void fn_800ABCF4(u32 spec) {
    BOOL enabled;
    u32 oldA4;
    u32 mask;

    enabled = OSDisableInterrupts();
    oldA4 = lbl_8047A8A4;
    lbl_80478A14 = spec << 8;
    mask = ~oldA4;
    lbl_8047A8A4 &= mask;
    lbl_8047A8B0 &= mask;
    lbl_8047A8B4 &= mask;
    fn_800D0464(oldA4);
    OSRestoreInterrupts(enabled);
}
#endif

/* fn_800ABD68 - 0x800ABD68 | size: 0x194 */
extern void fn_800ABF5C(void);
extern void fn_800CF708(void);
extern u32 lbl_8047A8BC;
extern u32 lbl_8047A8A8;
extern u32 lbl_80478A0C;
extern u32 lbl_8047A8C0;
extern u32 lbl_8047A8B8;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8AC;
#if 1
asm void fn_800ABD68(void) {
#include "src/dolphin/vi/VIFull_fn_800ABD68.inc"
}
#else
void fn_800ABD68(void) {
    extern u8 lbl_803FC5E0[];
    extern u32 lbl_80478A0C;
    extern u32 lbl_8047A8A4;
    extern u32 lbl_8047A8A8;
    extern u32 lbl_8047A8AC;
    extern u32 lbl_8047A8B0;
    extern u32 lbl_8047A8B4;
    extern u32 lbl_8047A8B8;
    extern u32 lbl_8047A8BC;
    extern u32 lbl_8047A8C0;
    extern void fn_800ABF5C();
    extern void fn_800CF708();
    extern void fn_800D0464();
    extern void fn_800D0CBC();
    extern void PADTypeAndStatusCallback();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    r30 = r3 + 0x0;
    tmp = lbl_8047A8BC;
    if (tmp != 0) {
        r3 = 0x0;
        fn_800ABF5C();
    }
    if ((s32)r30 == 0) {
        tmp = lbl_8047A8A8;
        r30 = 0x0;
        r3 = r30 + 0x0;
        if (tmp == 0) {
            tmp = lbl_80478A0C;
            if ((s32)tmp == 0x20) {
                r3 = 0x1;
        }
        }
        if ((s32)r3 != 0) {
            fn_800CF708();
            if ((s32)r3 == 0) {
                r30 = 0x1;
        }
        }
        tmp = lbl_8047A8C0;
        if (((s32)tmp == 0) && ((s32)r30 != 0)) {

            r30 = 0xF0000000;
            OSDisableInterrupts();
            r6 = lbl_8047A8B8;
            r7 = 0x0;
            r5 = lbl_8047A8B0;
            r4 = 0x80000000;
            tmp = lbl_8047A8B4;
            r30 = r30 | r6;
            r6 = lbl_8047A8A8;
            r5 = r5 | tmp;
            tmp = *(u8*)((u8*)r4 + 0x30E3);
            r30 = r30 & ~r5;
            r5 = lbl_8047A8A4;
            r4 = r6 | r30;
            lbl_8047A8B8 = r7;
            tmp = tmp & 0x00000040;
            lbl_8047A8A8 = r4;
            r4 = r5 & ~r30;
            r31 = r3 + 0x0;
            r6 = lbl_8047A8A8;
            lbl_8047A8A4 = r4;
            r3 = r6 & r5;
            if ((s32)r30 == 0) {
                tmp = lbl_8047A8AC;
                tmp = tmp | r30;
                lbl_8047A8AC = tmp;
            }
            fn_800D0464();
            tmp = lbl_80478A0C;
            if ((s32)tmp == 0x20) {
                r5 = lbl_8047A8A8;
                tmp = __cntlzw(r5);
                lbl_80478A0C = tmp;
                r4 = lbl_80478A0C;
                if ((s32)r4 != 0x20) {
                    tmp = 0x80000000;
                    tmp = (u32)tmp >> r4;
                    tmp = r5 & ~tmp;
                    r4 = r4 * 0xc;
                    lbl_8047A8A8 = tmp;
                    r3 = (u32)lbl_803FC5E0;
                    tmp = (u32)lbl_803FC5E0;
                    r3 = tmp + r4;
                    r4 = 0x0;
                    r5 = 0xc;
                    memset((void*)r3, (int)r4, (u32)r5);
                    r4 = (u32)PADTypeAndStatusCallback;
                    r3 = lbl_80478A0C;
                    r4 = (u32)PADTypeAndStatusCallback;
                    fn_800D0CBC();
            }
            }
            r3 = r31;
            OSRestoreInterrupts(r3);
            tmp = 0x1;
            lbl_8047A8C0 = tmp;
            r3 = 0x0;
            return;
        }
        r3 = r30;
        return;
    }
    tmp = 0x0;
    lbl_8047A8C0 = tmp;
    r3 = 0x1;

    return;
}
#endif

/* fn_800ABEFC - 0x800ABEFC | size: 0x60
 * VICallPreRetraceCallback - Call the pre-retrace callback if set.
 * Sets up a temporary context so the callback runs safely.
 */
void fn_800ABEFC(u32 retraceCount, OSContext* currentCtx) {
    extern u32 lbl_8047A8BC;
    OSContext tmpCtx;
    void (*callback)(void);

    callback = (void (*)(void))lbl_8047A8BC;
    if (callback != NULL) {
        OSClearContext(&tmpCtx);
        OSSetCurrentContext(&tmpCtx);
        callback();
        OSClearContext(&tmpCtx);
        OSSetCurrentContext(currentCtx);
    }
}
