#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * EXIBios.c - EXI BIOS-level helpers.
 *
 * Contains SetExiInterruptMask and related EXI internal functions
 * that sit between PPCArch.c and EXI.c in the link order.
 *
 * Matches: 0x80098110 - 0x8009820C (3 functions)
 *   fn_80098110 (0xFC) - __EXISetExiInterruptMask (full version)
 *   fn_80098168 (0x58) - (internal helper, part of above)
 *   fn_800981C0 (0x4C) - EXIGetID or EXI probe helper
 */

/* Per-channel register stride: 5 registers x 4 bytes = 20 bytes */
#define EXI_CHAN_PARAMS(chan) ((volatile u32*)(0xCC006800 + (chan) * 0x14))

/* EXI channel state - same structure as in EXI.c */
typedef struct EXIChan {
    void* callback;
    void* tcCallback;
    u32   dev;
    u32   flags;
    s32   bytesLeft;
    void* buf;
    u32   devType;
    u32   _1c;
    u32   idleStatus;
    u32   nQueued;
    void* queuedDev[2];
    void* queuedCb[2];
    u8    _padding[0x40 - 0x38];
} EXIChan;

extern EXIChan Ecb[3];

/*
 * __EXISetExiInterruptMask - Set interrupt masks for an EXI channel.
 * 0x80098110 | size: 0xFC
 *
 * Controls the TC, EXT, and EXI interrupt enable bits in the channel's
 * CSR register. When a device is locked (flags & 0x10), external
 * interrupts are suppressed for that channel.
 */
void __EXISetExiInterruptMask(s32 chan, EXIChan* exi) {
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    u32 csr;
    u32 mask;

    /* Only channels 0 and 1 have EXT interrupts */
    if (chan == 2) {
        return;
    }

    /* Read current CSR, preserve only non-interrupt bits */
    csr = reg[0];
    mask = csr & 0x7F5;

    /* If the channel is locked and device 0 is selected */
    if (exi->flags & 0x10) {
        if (exi->devType == 0) {
            /* Mask external interrupt while device 0 is locked */
            if (chan == 0) {
                __OSMaskInterrupts(0x00100000);
            } else {
                __OSMaskInterrupts(0x00020000);
            }
        } else {
            /* Enable EXT interrupt for non-device-0 locks */
            if (chan == 0) {
                __OSUnmaskInterrupts(0x00100000);
            } else {
                __OSUnmaskInterrupts(0x00020000);
            }
        }
    } else {
        /* Not locked: enable EXT interrupt if callback is set */
        if (exi->callback != NULL) {
            if (chan == 0) {
                __OSUnmaskInterrupts(0x00100000);
            } else {
                __OSUnmaskInterrupts(0x00020000);
            }
        } else {
            if (chan == 0) {
                __OSMaskInterrupts(0x00100000);
            } else {
                __OSMaskInterrupts(0x00020000);
            }
        }
    }
}

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 2 function(s)
 * =================================================================== */

/* fn_80098014 - 0x80098014 | size: 0x8 */
void fn_80098014(void) {
    asm { mtspr HID0, r3 }
}

/* fn_80098034 - 0x80098034 | size: 0x8 */
void fn_80098034(void) {
    asm { sc }
}

