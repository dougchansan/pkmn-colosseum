#include "dolphin/ar/AR.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSContext.h"

/*
 * AR.c - Auxiliary RAM (ARAM) driver for GameCube.
 *
 * The GameCube has 16MB of ARAM accessible via DMA through the
 * DSP interface registers. This module provides allocation and
 * DMA transfer functions.
 *
 * Adapted from doldecomp/melee matching implementation.
 *
 * Matches: part of 0x800AF280 - 0x800B5E8C range
 */

/* DSP/ARAM DMA registers at 0xCC005000 */
#define AR_DMA_MMADDR_H  (*(volatile u16*)0xCC005020)
#define AR_DMA_MMADDR_L  (*(volatile u16*)0xCC005022)
#define AR_DMA_ARADDR_H  (*(volatile u16*)0xCC005024)
#define AR_DMA_ARADDR_L  (*(volatile u16*)0xCC005026)
#define AR_DMA_CNT_H     (*(volatile u16*)0xCC005028)
#define AR_DMA_CNT_L     (*(volatile u16*)0xCC00502A)

#define AR_SIZE           (*(volatile u16*)0xCC005012)

extern void OSRegisterVersion(const char* id);
extern void OSReport(const char* fmt, ...);
extern void DCFlushRange(void* addr, u32 size);
extern void DCInvalidateRange(void* addr, u32 size);

static const char* __ARVersion = "<< Dolphin SDK - AR\trelease build: Aug 22 2002 04:07:28 (0x2301) >>";

static BOOL ARInitFlag;
static u32 ARFreeBlocks;
static u32 ARAMBaseAddr;
static u32 ARAMStackPointer;
static ARCallback ARDMACallback;

static void __ARInterruptHandler(__OSInterrupt interrupt, OSContext* context);

/*
 * ARCheckInit - Check if ARAM is initialized.
 */
BOOL ARCheckInit(void) {
    return ARInitFlag;
}

/*
 * ARInit - Initialize the ARAM subsystem.
 */
u32 ARInit(u32* stack, u32 stackSize) {
    u32 arSize;

    if (ARInitFlag) {
        return ARAMBaseAddr;
    }

    OSRegisterVersion(__ARVersion);

    ARInitFlag = TRUE;
    ARDMACallback = NULL;

    /* Get ARAM size */
    arSize = 0x01000000;  /* 16 MB default */

    ARAMBaseAddr = 0x4000;  /* Reserve first 16K for DSP */
    ARAMStackPointer = ARAMBaseAddr;
    ARFreeBlocks = arSize - ARAMBaseAddr;

    /* Install ARAM DMA interrupt handler */
    __OSSetInterruptHandler(0x06, (__OSInterruptHandler)__ARInterruptHandler);
    __OSUnmaskInterrupts(0x02000000);

    return ARAMBaseAddr;
}

/*
 * ARGetSize - Get total ARAM size.
 */
u32 ARGetSize(void) {
    return 0x01000000;  /* 16 MB */
}

/*
 * ARGetBaseAddress - Get the base usable ARAM address.
 */
u32 ARGetBaseAddress(void) {
    return ARAMBaseAddr;
}

/*
 * ARAlloc - Allocate ARAM memory.
 */
u32 ARAlloc(u32 length) {
    u32 addr;

    /* Align to 32 bytes */
    length = (length + 31) & ~31;

    if (length > ARFreeBlocks) {
        return 0;
    }

    addr = ARAMStackPointer;
    ARAMStackPointer += length;
    ARFreeBlocks -= length;

    return addr;
}

/*
 * ARFree - Free ARAM memory (stack-based).
 */
u32 ARFree(u32* length) {
    if (length != NULL) {
        *length = ARAMStackPointer - ARAMBaseAddr;
    }

    ARAMStackPointer = ARAMBaseAddr;
    ARFreeBlocks = ARGetSize() - ARAMBaseAddr;
    return ARAMBaseAddr;
}

/*
 * ARStartDMA - Start an ARAM DMA transfer.
 *
 * type: 0 = ARAM to main memory, 1 = main memory to ARAM
 */
void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Flush or invalidate cache depending on direction */
    if (type == 1) {
        DCFlushRange((void*)mainmem_addr, length);
    } else {
        DCInvalidateRange((void*)mainmem_addr, length);
    }

    /* Set main memory address */
    AR_DMA_MMADDR_H = (u16)((mainmem_addr >> 16) & 0x03FF);
    AR_DMA_MMADDR_L = (u16)(mainmem_addr & 0xFFE0);

    /* Set ARAM address */
    AR_DMA_ARADDR_H = (u16)((aram_addr >> 16) & 0x03FF);
    AR_DMA_ARADDR_L = (u16)(aram_addr & 0xFFE0);

    /* Set transfer length and start DMA */
    AR_DMA_CNT_H = (u16)(((length >> 16) & 0x7FFF) | (type << 15));
    AR_DMA_CNT_L = (u16)(length & 0xFFE0);

    OSRestoreInterrupts(enabled);
}

/*
 * ARGetDMAStatus - Check if ARAM DMA is active.
 */
u32 ARGetDMAStatus(void) {
    return AR_DMA_CNT_H & 0x8000 ? 1 : 0;
}

/*
 * ARReset - Reset ARAM subsystem.
 */
void ARReset(void) {
    ARInitFlag = FALSE;
}

/*
 * __ARInterruptHandler - ARAM DMA completion interrupt handler.
 */
static void __ARInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext tempCtx;

    /* Clear interrupt */

    /* Call DMA callback */
    if (ARDMACallback != NULL) {
        OSClearContext(&tempCtx);
        OSSetCurrentContext(&tempCtx);
        ARDMACallback();
        OSClearContext(&tempCtx);
        OSSetCurrentContext(context);
    }
}
