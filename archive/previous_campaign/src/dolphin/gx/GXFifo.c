#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSCache.h"

/*
 * GXFifo.c - GX Command FIFO management.
 *
 * Manages the command processor FIFO buffer that transfers rendering
 * commands from the CPU to the GP (Graphics Processor).
 *
 * Matches: 0x800B6FE0 - 0x800B74C8
 *   fn_800B6FE0  (0x134) - GXGetFifoStatus or __GXFifoLink
 *   GXInitFifoBase (0x6C) - Initialize FIFO buffer
 *   fn_800B7180  (0x70)  - GXGetFifoObj helpers
 *   fn_800B71F0  (0x0C)  - GXGetCPUFifo
 *   GXSetCPUFifo (0x110) - Set CPU command FIFO
 *   GXSetGPFifo  (0x178) - Set GP command FIFO
 *   fn_800B7484  (0x44)  - __GXSaveCPUFifoAux
 *   __GXFifoInit (0x4C)  - Install FIFO interrupt handler
 */

extern void* memset(void* dest, int val, u32 n);

/* CP (Command Processor) registers */
#define CP_STATUS    (*(volatile u16*)0xCC000000)
#define CP_ENABLE    (*(volatile u16*)0xCC000002)
#define CP_CLR       (*(volatile u16*)0xCC000004)

/* PI FIFO registers */
#define PI_FIFO_BASE_REG  (*(volatile u32*)0xCC00300C)
#define PI_FIFO_END_REG   (*(volatile u32*)0xCC003010)
#define PI_FIFO_WP_REG    (*(volatile u32*)0xCC003014)

extern GXData* __GXContextPtr;

/*
 * __GXFifoLink - Link/unlink CPU and GP FIFOs.
 * 0x800B6FE0 | size: 0x134
 */
static void __GXFifoLink(BOOL link) {
    if (link) {
        /* Link CPU FIFO to GP FIFO */
        CP_ENABLE |= 0x10;
    } else {
        CP_ENABLE &= ~0x10;
    }
}

/*
 * GXGetFifoStatus - Get the current FIFO status.
 */
void GXGetFifoStatus(GXFifoObj* fifo, GXBool* overhi, GXBool* underlow,
                     u32* fifoCount, GXBool* cpuWrite, GXBool* gpRead) {
    if (overhi) *overhi = 0;
    if (underlow) *underlow = 0;
    if (fifoCount) *fifoCount = fifo->count;
    if (cpuWrite) *cpuWrite = 1;
    if (gpRead) *gpRead = 1;
}

/*
 * GXInitFifoBase - Initialize a FIFO object with a buffer.
 * 0x800B7114 | size: 0x6C
 */
void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size) {
    fifo->base = (u8*)base;
    fifo->top = (u8*)base + size - 4;
    fifo->size = size;
    fifo->hiWatermark = size - 0x4000;
    fifo->loWatermark = size / 2;
    fifo->rdPtr = base;
    fifo->wrPtr = base;
    fifo->count = 0;
    fifo->wrap = 0;
}

/*
 * GXGetCPUFifo - Get the current CPU FIFO.
 * 0x800B71F0 | size: 0x0C
 */
static GXFifoObj* __CPUFifo;
static GXFifoObj* __GPFifo;

GXFifoObj* GXGetCPUFifo(void) {
    return __CPUFifo;
}

/*
 * GXGetGPFifo - Get the current GP FIFO.
 */
GXFifoObj* GXGetGPFifo(void) {
    return __GPFifo;
}

/*
 * GXSetCPUFifo - Set the CPU command FIFO.
 * 0x800B71FC | size: 0x110
 */
void GXSetCPUFifo(GXFifoObj* fifo) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    __CPUFifo = fifo;

    /* Program PI FIFO registers */
    PI_FIFO_BASE_REG = (u32)fifo->base & 0x03FFFFFF;
    PI_FIFO_END_REG = (u32)fifo->top & 0x03FFFFFF;
    PI_FIFO_WP_REG = (u32)fifo->wrPtr & 0x03FFFFFF;

    /* If CPU and GP share the same FIFO, link them */
    if (__CPUFifo == __GPFifo) {
        __GXFifoLink(TRUE);
    } else {
        __GXFifoLink(FALSE);
    }

    OSRestoreInterrupts(enabled);
}

/*
 * GXSetGPFifo - Set the GP (Graphics Processor) FIFO.
 * 0x800B730C | size: 0x178
 */
void GXSetGPFifo(GXFifoObj* fifo) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    __GPFifo = fifo;

    /* Program CP FIFO registers */
    /* CP reads from this FIFO to execute graphics commands */
    {
        volatile u16* cp = (volatile u16*)0xCC000000;

        /* Base address */
        cp[0x20 / 2] = (u16)((u32)fifo->base >> 16);
        cp[0x22 / 2] = (u16)((u32)fifo->base);

        /* Top address */
        cp[0x24 / 2] = (u16)((u32)fifo->top >> 16);
        cp[0x26 / 2] = (u16)((u32)fifo->top);

        /* High watermark */
        cp[0x28 / 2] = (u16)(fifo->hiWatermark >> 16);
        cp[0x2A / 2] = (u16)(fifo->hiWatermark);

        /* Low watermark */
        cp[0x2C / 2] = (u16)(fifo->loWatermark >> 16);
        cp[0x2E / 2] = (u16)(fifo->loWatermark);

        /* Read/write pointers */
        cp[0x30 / 2] = (u16)((u32)fifo->rdPtr >> 16);
        cp[0x32 / 2] = (u16)((u32)fifo->rdPtr);
        cp[0x34 / 2] = (u16)((u32)fifo->wrPtr >> 16);
        cp[0x36 / 2] = (u16)((u32)fifo->wrPtr);
    }

    /* Link if shared */
    if (__CPUFifo == __GPFifo) {
        __GXFifoLink(TRUE);
    }

    OSRestoreInterrupts(enabled);
}

/*
 * __GXFifoInit - Initialize FIFO interrupt handling.
 * 0x800B74C8 | size: 0x4C
 */
void __GXFifoInit(void) {
    /* Install CP interrupt handler */
    __OSSetInterruptHandler(0x11, NULL);  /* CP interrupt */
}
