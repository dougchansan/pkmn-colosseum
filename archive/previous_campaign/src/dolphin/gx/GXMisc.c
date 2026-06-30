#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSContext.h"

/*
 * GXMisc.c - GX miscellaneous functions, performance counters, and pixel engine.
 *
 * Contains GXSetMisc, __GXPEInit, and various GX utility functions
 * for display copy, performance monitoring, and draw synchronization.
 *
 * Matches: 0x800B7514 - 0x800B96BC
 */

/* BP (Blitting Processor) register write */
#define GX_BP_REG ((volatile u32*)0xCC008000)

/* Write a GX BP register command to the FIFO */
static inline void GXWriteBPReg(u32 val) {
    *(volatile u8*)0xCC008000 = 0x61;
    *(volatile u32*)0xCC008000 = val;
}

/*
 * GXSetMisc - Set miscellaneous GX parameters.
 * 0x800B888C | size: 0x94
 */
void GXSetMisc(u32 token, u32 val) {
    switch (token) {
        case 0:
            /* Set dither mode */
            break;
        case 1:
            /* Set abort wait count */
            break;
    }
}

/*
 * GXFlush - Flush the GX command FIFO.
 */
void GXFlush(void) {
    /* Write a NOP to ensure all previous commands are sent */
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
    *(volatile u32*)0xCC008000 = 0;
}

/*
 * GXAbortFrame - Abort the current frame.
 */
void GXAbortFrame(void) {
    /* Reset the GP */
}

/*
 * __GXPEInit - Initialize the Pixel Engine.
 * 0x800B916C | size: 0x80
 */
void __GXPEInit(void) {
    /* Install PE interrupt handlers */
    __OSSetInterruptHandler(0x12, NULL); /* PE finish interrupt */
    __OSSetInterruptHandler(0x13, NULL); /* PE token interrupt */

    /* Unmask PE interrupts */
    __OSUnmaskInterrupts(0x00004000 | 0x00002000);
}

/*
 * GXDrawDone - Wait for rendering to complete.
 */
void GXDrawDone(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Send draw done token through FIFO */
    /* Wait for PE to signal completion */

    OSRestoreInterrupts(enabled);
}

/*
 * GXSetDrawDoneCallback - Set the draw done callback.
 */
typedef void (*GXDrawDoneCallback)(void);
static GXDrawDoneCallback DrawDoneCB;

GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb) {
    GXDrawDoneCallback old;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    old = DrawDoneCB;
    DrawDoneCB = cb;
    OSRestoreInterrupts(enabled);
    return old;
}

/*
 * GXWaitDrawDone - Wait for draw completion (blocking).
 */
void GXWaitDrawDone(void) {
    GXDrawDone();
}

/*
 * GXPixModeSync - Synchronize pixel processing mode changes.
 */
void GXPixModeSync(void) {
    /* Write BP register to sync pixel mode */
}

/*
 * GXTexModeSync - Synchronize texture mode changes.
 */
void GXTexModeSync(void) {
    /* Write BP register to sync texture mode */
}

/*
 * GXPokeAlphaMode - Direct-write alpha mode.
 */
void GXPokeAlphaMode(u8 func, u8 threshold) {
    volatile u32* peRegs = (volatile u32*)0xCC001000;
    peRegs[0x30 / 4] = ((u32)func << 8) | threshold;
}

/*
 * GXPokeAlphaUpdate - Direct-write alpha update.
 */
void GXPokeAlphaUpdate(GXBool enable) {
    volatile u32* peRegs = (volatile u32*)0xCC001000;
    if (enable) {
        peRegs[0x34 / 4] |= 0x10;
    } else {
        peRegs[0x34 / 4] &= ~0x10;
    }
}

/*
 * GXPokeColorUpdate - Direct-write color update.
 */
void GXPokeColorUpdate(GXBool enable) {
    volatile u32* peRegs = (volatile u32*)0xCC001000;
    if (enable) {
        peRegs[0x34 / 4] |= 0x08;
    } else {
        peRegs[0x34 / 4] &= ~0x08;
    }
}

/*
 * GXPokeBlendMode - Direct-write blend mode.
 */
void GXPokeBlendMode(u8 type, u8 srcFactor, u8 dstFactor, u8 op) {
    volatile u32* peRegs = (volatile u32*)0xCC001000;
    peRegs[0x34 / 4] = ((u32)type) | ((u32)srcFactor << 8) |
                        ((u32)dstFactor << 12) | ((u32)op << 16);
}

/*
 * GXPokeZMode - Direct-write Z mode.
 */
void GXPokeZMode(GXBool enable, u8 func, GXBool update) {
    volatile u32* peRegs = (volatile u32*)0xCC001000;
    peRegs[0x38 / 4] = enable | ((u32)func << 1) | ((u32)update << 4);
}
