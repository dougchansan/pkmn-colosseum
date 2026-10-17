/**
 * @file sdk_range_800B71FC.c
 * @brief dolphin-sdk code, 0x800B71FC - 0x800B770C (10 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/PPCArch.h"

extern GXData* gx;
extern volatile u16* __cpReg;
extern volatile u32* __piReg;
extern GXFifoObj* lbl_8047A9A0;
extern GXFifoObj* lbl_8047A9A4;
extern u8 lbl_8047A9AC;

void __GXFifoLink(u8 arg0);
void __GXWriteFifoIntEnable(u8 arg0, u8 arg1);
void __GXWriteFifoIntReset(u8 arg0, u8 arg1);
void __GXFifoReadEnable(void);
void __GXFifoReadDisable(void);

void GXSetCPUFifo(GXFifoObj* fifo) {
    BOOL enabled;
    u32 reg;

    enabled = OSDisableInterrupts();
    lbl_8047A9A0 = fifo;
    if (lbl_8047A9A0 == lbl_8047A9A4) {
        __piReg[3] = (u32)fifo->base & 0x3FFFFFFF;
        __piReg[4] = (u32)fifo->top & 0x3FFFFFFF;
        reg = (u32)fifo->wrPtr & 0x3FFFFFE0;
        reg &= 0xFBFFFFFF;
        __piReg[5] = reg;
        lbl_8047A9AC = TRUE;
        __GXWriteFifoIntReset(1, 1);
        __GXWriteFifoIntEnable(1, 0);
        __GXFifoLink(1);
    } else {
        if (lbl_8047A9AC) {
            __GXFifoLink(0);
            lbl_8047A9AC = FALSE;
        }
        __GXWriteFifoIntEnable(0, 0);
        __piReg[3] = (u32)fifo->base & 0x3FFFFFFF;
        __piReg[4] = (u32)fifo->top & 0x3FFFFFFF;
        reg = (u32)fifo->wrPtr & 0x3FFFFFE0;
        reg &= 0xFBFFFFFF;
        __piReg[5] = reg;
    }
    PPCSync();
    OSRestoreInterrupts(enabled);
}

void GXSetGPFifo(GXFifoObj* fifo) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    __GXFifoReadDisable();
    __GXWriteFifoIntEnable(0, 0);
    lbl_8047A9A4 = fifo;

    __cpReg[16] = (u32)fifo->base & 0xFFFF;
    __cpReg[18] = (u32)fifo->top & 0xFFFF;
    __cpReg[24] = fifo->count & 0xFFFF;
    __cpReg[26] = (u32)fifo->wrPtr & 0xFFFF;
    __cpReg[28] = (u32)fifo->rdPtr & 0xFFFF;
    __cpReg[20] = fifo->hiWatermark & 0xFFFF;
    __cpReg[22] = fifo->loWatermark & 0xFFFF;
    __cpReg[17] = ((u32)fifo->base & 0x3FFFFFFF) >> 16;
    __cpReg[19] = ((u32)fifo->top & 0x3FFFFFFF) >> 16;
    __cpReg[25] = fifo->count >> 16;
    __cpReg[27] = ((u32)fifo->wrPtr & 0x3FFFFFFF) >> 16;
    __cpReg[29] = ((u32)fifo->rdPtr & 0x3FFFFFFF) >> 16;
    __cpReg[21] = fifo->hiWatermark >> 16;
    __cpReg[23] = fifo->loWatermark >> 16;

    PPCSync();

    if (lbl_8047A9A0 == lbl_8047A9A4) {
        lbl_8047A9AC = TRUE;
        __GXWriteFifoIntEnable(1, 0);
        __GXFifoLink(1);
    } else {
        lbl_8047A9AC = FALSE;
        __GXWriteFifoIntEnable(0, 0);
        __GXFifoLink(0);
    }
    __GXWriteFifoIntReset(1, 1);
    __GXFifoReadEnable();
    OSRestoreInterrupts(enabled);
}

void* GXSetBreakPtCallback(void* callback) {
    extern void* lbl_8047A9B4;
    BOOL enabled;
    void* old;

    old = lbl_8047A9B4;
    enabled = OSDisableInterrupts();
    lbl_8047A9B4 = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

void __GXFifoInit(void) {
    extern void* lbl_8047A9A8;
    extern u32 lbl_8047A9B0;
    extern OSThread* OSGetCurrentThread(void);
    extern void GXCPInterruptHandler(__OSInterrupt interrupt, OSContext* context);

    __OSSetInterruptHandler(0x11, GXCPInterruptHandler);
    __OSUnmaskInterrupts(0x4000);
    lbl_8047A9A8 = OSGetCurrentThread();
    lbl_8047A9B0 = 0;
    lbl_8047A9A0 = NULL;
    lbl_8047A9A4 = NULL;
}

void __GXFifoReadEnable(void) {
    u32* gxStatus = (u32*)gx;
    u32 value;

    value = gxStatus[2];
    gxStatus[2] = (value & ~1U) | 1U;
    __cpReg[1] = (u16)gxStatus[2];
}

void __GXFifoReadDisable(void) {
    u32* gxStatus = (u32*)gx;

    gxStatus[2] &= ~1U;
    __cpReg[1] = (u16)gxStatus[2];
}

void __GXFifoLink(u8 arg0) {
    u32* gxStatus;
    u32 bit;

    if ((arg0 & 0xFF) != 0) {
        bit = 1U;
    } else {
        bit = 0U;
    }
    gxStatus = (u32*)gx;
    gxStatus[2] = (gxStatus[2] & ~0x10U) | (bit << 4);
    __cpReg[1] = (u16)gxStatus[2];
}

void __GXWriteFifoIntEnable(u8 arg0, u8 arg1) {
    u32* gxStatus = (u32*)gx;
    gxStatus[2] = (gxStatus[2] & ~4U) | ((u32)(arg0 << 2));
    gxStatus[2] = (gxStatus[2] & ~8U) | ((u32)(arg1 << 3));
    __cpReg[1] = (u16)gxStatus[2];
}

void __GXWriteFifoIntReset(u8 arg0, u8 arg1) {
    u32* gxStatus = (u32*)gx;
    gxStatus[4] = (gxStatus[4] & ~1U) | (u32)arg0;
    gxStatus[4] = (gxStatus[4] & ~2U) | ((u32)arg1 << 1);
    __cpReg[2] = (u16)gxStatus[4];
}

typedef struct GXFifoObjPrivate {
    GXFifoObj fifo;
    u8 padding[0x80 - sizeof(GXFifoObj)];
} GXFifoObjPrivate;

static inline void GXResetFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fifo->rdPtr = readPtr;
    fifo->wrPtr = writePtr;
    fifo->count = (u8*)writePtr - (u8*)readPtr;
    if (fifo->count < 0) {
        fifo->count += fifo->size;
    }
    OSRestoreInterrupts(enabled);
}

void __GXCleanGPFifo(void) {
    extern GXFifoObjPrivate* GXGetCPUFifo(void);
    extern GXFifoObjPrivate* GXGetGPFifo(void);
    GXFifoObjPrivate* gpFifo;
    GXFifoObjPrivate* cpuFifo;
    GXFifoObjPrivate cleanFifo;
    u32 _pad[3];
    void* base;

    gpFifo = GXGetGPFifo();
    if (gpFifo != NULL) {
        cpuFifo = GXGetCPUFifo();
        base = gpFifo->fifo.base;
        (void)_pad;
        cleanFifo = *gpFifo;
        GXResetFifoPtrs(&cleanFifo.fifo, base, base);
        GXSetGPFifo(&cleanFifo.fifo);
        if (cpuFifo == gpFifo) {
            GXSetCPUFifo(&cleanFifo.fifo);
        }
        GXResetFifoPtrs(&gpFifo->fifo, base, base);
        GXSetGPFifo(&gpFifo->fifo);
        if (cpuFifo == gpFifo) {
            GXSetCPUFifo(&cpuFifo->fifo);
        }
    }
}
