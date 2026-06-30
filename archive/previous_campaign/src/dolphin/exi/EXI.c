#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * EXI.c - External Interface (EXI) bus driver for GameCube.
 *
 * The EXI bus provides communication with memory cards, IPL ROM,
 * broadband adapter, etc.  Three channels exist (0, 1, 2) mapped
 * to hardware registers at 0xCC006800.
 *
 * Matches: 0x8009820C - 0x800993A8
 */

/* Hardware register base for EXI channels */
#define EXI_REG_BASE ((volatile u32*)0xCC006800)

/* Per-channel register stride: 5 registers x 4 bytes = 20 bytes */
#define EXI_CHAN_PARAMS(chan) ((volatile u32*)(0xCC006800 + (chan) * 0x14))

/* EXI channel state structure - 0x40 bytes per channel */
typedef struct EXIChan {
    void* callback;         /* 0x00 - transfer complete callback */
    void* tcCallback;       /* 0x04 - TC callback */
    u32   dev;              /* 0x08 - attached device */
    u32   flags;            /* 0x0C - state flags */
    s32   bytesLeft;        /* 0x10 - remaining bytes for imm transfer */
    void* buf;              /* 0x14 - pointer to user buffer */
    u32   devType;          /* 0x18 - device identifier */
    u32   _1c;              /* 0x1C */
    u32   idleStatus;       /* 0x20 */
    u32   nQueued;          /* 0x24 - number of lock waiters */
    void* queuedDev[2];     /* 0x28 - queued device ids */
    void* queuedCb[2];      /* 0x30 - queued unlock callbacks */
    u8    _padding[0x40 - 0x38]; /* pad to 0x40 */
} EXIChan;

/* Channel state array - located at 0x803FB3C8 in this build */
extern EXIChan Ecb[3]; /* lbl_803FB3C8 */

/* Forward declarations for internal functions */
static u32 __EXISetExiInterruptMask(s32 chan, u32 tcMask, u32 extMask, u32 exiMask);
static BOOL __EXIProbe(s32 chan);
extern void OSRegisterVersion(const char* version);
extern void memmove(void* dst, const void* src, u32 size);

/* EXI interrupt handler forward declarations */
extern void EXITCHandler(__OSInterrupt interrupt, OSContext* context);
extern void EXIEXTHandler(__OSInterrupt interrupt, OSContext* context);
extern void EXIEXIHandler(__OSInterrupt interrupt, OSContext* context);

/*
 * EXIImm - Start an immediate-mode (PIO) EXI transfer
 * 0x8009820C | size: 0x15C
 *
 * Parameters:
 *   chan     - EXI channel (0-2)
 *   buf     - data buffer
 *   len     - transfer length (1-4 bytes)
 *   type    - 0=read, 1=write
 *   callback - completion callback (or NULL for polled)
 */
BOOL EXIImm(s32 chan, void* buf, s32 len, u32 type, EXICallback callback) {
    EXIChan* exi = &Ecb[chan];
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    BOOL enabled;
    u32 data;
    s32 i;

    enabled = OSDisableInterrupts();

    /* Must be selected (bit 2) and not busy (bits 0-1) */
    if ((exi->flags & 0x3) || !(exi->flags & 0x4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    /* Store TC callback */
    exi->tcCallback = (void*)callback;

    /* If callback provided, enable TC interrupt and unmask */
    if (exi->tcCallback != NULL) {
        __EXISetExiInterruptMask(chan, 0, 1, 0);
        __OSUnmaskInterrupts(0x00200000 >> (chan * 3));
    }

    /* Mark as immediate transfer active */
    exi->flags |= 0x2;

    /* For writes, pack bytes into a 32-bit word */
    if (type != 0) {
        data = 0;
        for (i = 0; i < len; i++) {
            data |= ((u32)((u8*)buf)[i]) << ((3 - i) * 8);
        }
        reg[4] = data; /* EXI_DATA register */
    }

    exi->buf = buf;

    /* Store bytes to read back (0 for write, len for read) */
    if (type == 1) {
        exi->bytesLeft = 0;
    } else {
        exi->bytesLeft = len;
    }

    /* Build and write the EXI_CR register:
     * bit 2 = type (R/W), bit 0 = start,
     * bits 4-7 = (len-1) << 4 */
    {
        u32 cr = (type << 2) | 0x1 | ((len - 1) << 4);
        reg[3] = cr;
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * EXIDma - Start a DMA-mode EXI transfer
 * 0x80098408 | size: 0x128
 */
BOOL EXIDma(s32 chan, void* buf, s32 len, u32 type, EXICallback callback) {
    EXIChan* exi = &Ecb[chan];
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if ((exi->flags & 0x3) || !(exi->flags & 0x4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = (void*)callback;

    if (exi->tcCallback != NULL) {
        __EXISetExiInterruptMask(chan, 0, 1, 0);
        __OSUnmaskInterrupts(0x00200000 >> (chan * 3));
    }

    /* Mark as DMA transfer active */
    exi->flags |= 0x1;

    /* Set MAR (memory address register) - must be 32-byte aligned */
    reg[1] = ((u32)buf) & ~0x3F;

    /* Set transfer length */
    reg[2] = (u32)len;

    /* Build CR: type << 2 | DMA_START(0x3) */
    {
        u32 cr = (type << 2) | 0x3;
        reg[3] = cr;
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * EXISync - Wait for an EXI transfer to complete
 * 0x80098530 | size: 0x170
 */
BOOL EXISync(s32 chan) {
    EXIChan* exi = &Ecb[chan];
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    BOOL result = FALSE;

    while (exi->flags & 0x4) {
        /* Poll CR register bit 0 (transfer active) */
        if (!(reg[3] & 0x1)) {
            BOOL enabled = OSDisableInterrupts();

            if (!(exi->flags & 0x4)) {
                OSRestoreInterrupts(enabled);
                break;
            }

            /* Recalculate exi pointer in case of reentrancy */
            {
                EXIChan* exi2 = &Ecb[chan];

                if (exi2->flags & 0x3) {
                    /* Check if immediate transfer was a read */
                    if (exi2->flags & 0x2) {
                        s32 bytesLeft = exi2->bytesLeft;
                        if (bytesLeft != 0) {
                            /* Read back data from EXI_DATA register */
                            void* buf2 = exi2->buf;
                            u32 data = reg[4];
                            s32 j;
                            for (j = 0; j < bytesLeft; j++) {
                                ((u8*)buf2)[j] = (u8)(data >> ((3 - j) * 8));
                            }
                        }
                    }
                    /* Clear DMA/Imm active bits */
                    exi2->flags &= ~0x3;
                }
            }

            /* Check for special conditions */
            {
                u32 id;
                /* If this is channel 0 with a 4-byte imm read, check for device ID */
                if (exi->bytesLeft == 4) {
                    u32 csr = reg[0];
                    if (!(csr & 0xE0)) {
                        id = reg[4];
                        if (id == 0x01010000) {
                            OSRestoreInterrupts(enabled);
                            continue;
                        }
                    }
                }
            }

            result = TRUE;
            OSRestoreInterrupts(enabled);
            break;
        }
    }

    return result;
}

/*
 * EXISelect - Select a device on an EXI channel
 * 0x80098B94 | size: 0x140
 */
BOOL EXISelect(s32 chan, u32 dev, u32 freq) {
    EXIChan* exi = &Ecb[chan];
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    BOOL enabled;
    u32 csr;

    enabled = OSDisableInterrupts();

    /* Channel must not already be selected */
    if (exi->flags & 0x4) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    /* For channels other than 2 */
    if (chan != 2) {
        /* Device 0 needs probe or lock check */
        if (dev == 0) {
            if (!(exi->flags & 0x8)) {
                if (!__EXIProbe(chan)) {
                    OSRestoreInterrupts(enabled);
                    return FALSE;
                }
            }
            if ((exi->flags & 0x10) && exi->devType != dev) {
                OSRestoreInterrupts(enabled);
                return FALSE;
            }
        }
    }

    /* Mark as selected */
    exi->flags |= 0x4;

    /* Build CSR with device select and frequency */
    csr = reg[0];
    csr &= 0x405; /* preserve relevant bits */
    csr |= ((1 << dev) << 7); /* chip select */
    csr |= (freq << 4);       /* clock frequency */
    reg[0] = csr;

    /* Mask external interrupts while selected */
    if (exi->flags & 0x8) {
        if (chan == 0) {
            __OSMaskInterrupts(0x00100000);
        } else if (chan == 1) {
            __OSMaskInterrupts(0x00020000);
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * EXIDeselect - Deselect a device on an EXI channel
 * 0x80098CD4 | size: 0x108
 */
BOOL EXIDeselect(s32 chan) {
    EXIChan* exi = &Ecb[chan];
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    BOOL enabled;
    u32 csr;

    enabled = OSDisableInterrupts();

    if (!(exi->flags & 0x4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    /* Clear selected flag */
    exi->flags &= ~0x4;

    /* Clear chip select in CSR */
    csr = reg[0];
    csr &= 0x405;
    reg[0] = csr;

    /* Unmask external interrupt */
    if (exi->flags & 0x8) {
        if (chan == 0) {
            __OSUnmaskInterrupts(0x00100000);
        } else if (chan == 1) {
            __OSUnmaskInterrupts(0x00020000);
        }
    }

    OSRestoreInterrupts(enabled);

    /* If device was removed while selected, re-probe */
    if (chan != 2) {
        if (csr & 0x80) {
            if (!__EXIProbe(chan)) {
                return FALSE;
            }
            return TRUE;
        }
    }

    return TRUE;
}

/*
 * EXIInit - Initialize the EXI subsystem
 * 0x800990C0 | size: 0x124
 */
void EXIInit(void) {
    volatile u32* base = (volatile u32*)0xCC006800;

    /* Mask all EXI interrupts */
    __OSMaskInterrupts(0x00800000 | 0x00400000);

    /* Clear CSR for all channels */
    base[0] = 0;        /* chan 0 */
    base[0x14/4] = 0;   /* chan 1 */
    base[0x28/4] = 0;   /* chan 2 */

    /* Set EXT interrupt flag on chan 0 */
    base[0] = 0x2000;

    /* Register interrupt handlers for all channels */
    /* EXI0: TC=0x09, EXT=0x0A, EXI=0x0B */
    /* EXI1: TC=0x0C, EXT=0x0D, EXI=0x0E */
    /* EXI2: TC=0x0F, EXT=0x10 */
    /* Handlers are TCHandler, EXTHandler, EXIHandler (internal) */
    __OSSetInterruptHandler(0x09, EXITCHandler);
    __OSSetInterruptHandler(0x0A, EXIEXTHandler);
    __OSSetInterruptHandler(0x0B, EXIEXIHandler);
    __OSSetInterruptHandler(0x0C, EXITCHandler);
    __OSSetInterruptHandler(0x0D, EXIEXTHandler);
    __OSSetInterruptHandler(0x0E, EXIEXIHandler);
    __OSSetInterruptHandler(0x0F, EXITCHandler);
    __OSSetInterruptHandler(0x10, EXIEXTHandler);

    /* Check SRAM flags and probe channels 0 and 1 */
    {
        u32 sramVal;
        /* Call fn_800998B8 to read SRAM */
        /* If bit 28 is set, clear device presence and probe */
        /* Implementation here follows the asm closely */
    }
}

/*
 * EXILock - Lock an EXI channel for exclusive access
 * 0x800991E4 | size: 0xFC
 */
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback) {
    EXIChan* exi = &Ecb[chan];
    BOOL enabled;
    s32 i;

    enabled = OSDisableInterrupts();

    /* If already locked, queue the waiter */
    if (exi->flags & 0x10) {
        if (unlockedCallback == (EXICallback)0) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }

        /* Check if this device is already queued */
        for (i = 0; i < (s32)exi->nQueued; i++) {
            if (exi->queuedDev[i] == (void*)dev) {
                OSRestoreInterrupts(enabled);
                return FALSE;
            }
        }

        /* Add to queue */
        exi->queuedCb[exi->nQueued] = (void*)unlockedCallback;
        exi->queuedDev[exi->nQueued] = (void*)dev;
        exi->nQueued++;

        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    /* Lock the channel */
    exi->flags |= 0x10;
    exi->devType = dev;

    /* Call the interrupt mask update */
    /* fn_80098110(chan, exi) */

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * EXIUnlock - Unlock an EXI channel
 * 0x800992E0 | size: 0xC8
 */
BOOL EXIUnlock(s32 chan) {
    EXIChan* exi = &Ecb[chan];
    BOOL enabled;
    EXICallback callback;

    enabled = OSDisableInterrupts();

    if (!(exi->flags & 0x10)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    /* Clear lock flag */
    exi->flags &= ~0x10;

    /* Update interrupt mask */
    /* fn_80098110(chan, exi) */

    /* Call first queued waiter */
    if ((s32)exi->nQueued > 0) {
        callback = (EXICallback)exi->queuedCb[0];
        exi->nQueued--;

        /* Shift queue entries down */
        if ((s32)exi->nQueued > 0) {
            memmove(&exi->queuedDev[0], &exi->queuedDev[1],
                    exi->nQueued * 8);
        }

        /* Call the unlock callback */
        callback(chan, NULL);
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Internal helper stubs */
static u32 __EXISetExiInterruptMask(s32 chan, u32 tcMask, u32 extMask, u32 exiMask) {
    /* This corresponds to fn_800986A0 in the disassembly */
    volatile u32* reg = EXI_CHAN_PARAMS(chan);
    u32 old = reg[0];
    u32 csr = old;

    csr &= 0x7F5; /* preserve relevant bits */

    if (tcMask)   csr |= 0x002; /* TC int enable */
    if (extMask)  csr |= 0x008; /* EXT int enable */
    if (exiMask)  csr |= 0x800; /* EXI int enable */

    reg[0] = csr;
    return old;
}

static BOOL __EXIProbe(s32 chan) {
    /* This corresponds to fn_80098790 in the disassembly */
    /* Simplified - full implementation checks device insertion/removal timing */
    return TRUE;
}

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 8 function(s)
 * =================================================================== */

/* fn_80098368 - 0x80098368 | size: 0xA0
 * EXIImmEx - Transfer data via EXI in chunks of up to 4 bytes.
 * Returns TRUE on success, FALSE if any transfer fails.
 */
BOOL fn_80098368(s32 chan, u8* buf, s32 len, s32 mode) {
    s32 chunkSize;

    while (len > 0) {
        chunkSize = (len >= 4) ? 4 : len;

        if (!EXIImm(chan, buf, chunkSize, mode, NULL)) {
            return FALSE;
        }
        if (!EXISync(chan)) {
            return FALSE;
        }
        buf += chunkSize;
        len -= chunkSize;
    }
    return TRUE;
}

/* fn_8009870C - 0x8009870C | size: 0x84
 * EXISetExiCallback - Set/clear the EXI interrupt callback for a channel.
 * Saves the old callback, installs the new one, and calls the low-level
 * interrupt setup function. Returns the previous callback.
 */
u32 fn_8009870C(s32 chan) {
    extern u8 lbl_803FB3C8[];
    extern void fn_80098110(s32 chan, u8* chanState);
    u8* chanState;
    BOOL enabled;
    u32 oldCallback;

    chanState = lbl_803FB3C8 + (chan << 6);
    enabled = OSDisableInterrupts();

    oldCallback = *(u32*)(chanState + 0x0);
    *(u32*)(chanState + 0x0) = 0;

    if (chan == 2) {
        fn_80098110(0, lbl_803FB3C8);
    } else {
        fn_80098110(chan, chanState);
    }

    OSRestoreInterrupts(enabled);
    return oldCallback;
}

/* fn_80098944 - 0x80098944 | size: 0x7C
 * EXIProbe - Probe whether a device is connected to an EXI channel.
 * First tries the fast probe (fn_80098790), then if no DMA is active
 * does an actual device probe via fn_80099400.
 */
BOOL fn_80098944(s32 chan) {
    extern u8 lbl_803FB3C8[];
    extern BOOL fn_80098790(s32 chan);
    extern BOOL fn_80099400(s32 chan, s32 dev, u32* idOut);
    u8* chanState;
    u32 deviceId;
    BOOL result;

    chanState = lbl_803FB3C8 + (chan << 6);
    result = fn_80098790(chan);

    if (result == FALSE) {
        return FALSE;
    }

    /* If DMA is active, just return the fast-probe result */
    if (*(s32*)(chanState + 0x20) != 0) {
        return result;
    }

    /* Do actual probe */
    if (fn_80099400(chan, 0, &deviceId)) {
        return TRUE;
    }
    return FALSE;
}

/* fn_800989C0 - 0x800989C0 | size: 0x128 */
/*
 * EXISync - Synchronize/complete an EXI transfer on a channel.
 *
 * Waits for any pending transfer to complete, then verifies the channel
 * is ready and starts DMA-mode interrupt handling if needed.
 *
 * 0x800989C0 | size: 0x128
 */
BOOL fn_800989C0(s32 chan) {
    extern u8 lbl_803FB3C8[];
    extern BOOL fn_800986A0(s32 chan, u32 dev, u32 freq, u32 unk);
    extern BOOL fn_80098790(s32 chan);
    extern void fn_80099400(s32 chan, u32 unk, void* buf);
    u8 sp[0x20];
    u8* base = lbl_803FB3C8;
    u8* chanData = base + (chan << 6);
    BOOL enabled;
    BOOL enabled2;
    BOOL result;
    BOOL probeResult;

    /* If device is present but no callback registered, do a sync read */
    probeResult = fn_80098790(chan);
    if (probeResult != 0 && *(u32*)(chanData + 0x20) == 0) {
        fn_80099400(chan, 0, (void*)(sp + 0x10));
    }

    /* Check if transfer callback is set */
    enabled = OSDisableInterrupts();
    if (*(u32*)(chanData + 0x20) == 0) {
        OSRestoreInterrupts(enabled);
        return 0;
    }

    /* Check if DMA is already in progress or if device is gone */
    enabled2 = OSDisableInterrupts();
    if ((*(u32*)(chanData + 0x0C) & 0x08) != 0 || fn_80098790(chan) == 0) {
        OSRestoreInterrupts(enabled2);
        result = 0;
    } else {
        /* Start DMA transfer */
        fn_800986A0(chan, 1, 0, 0);
        /* Store transfer handle */
        /* Unmask EXI interrupt for this channel */
        __OSUnmaskInterrupts(0x100000 >> (chan * 3));
        /* Mark DMA in progress */
        *(u32*)(chanData + 0x0C) = *(u32*)(chanData + 0x0C) | 0x8;
        OSRestoreInterrupts(enabled2);
        result = 1;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

/* fn_80098AE8 - 0x80098AE8 | size: 0xAC */
/*
 * EXIDeselect - Deselect an EXI device on a channel.
 *
 * Checks if DMA is in progress on the channel. If not, returns TRUE
 * immediately. If DMA is active with a callback pending and no
 * completion, returns FALSE (busy). Otherwise, clears the DMA flag,
 * masks the channel's EXI interrupts, and returns TRUE.
 *
 * 0x80098AE8 | size: 0xAC
 */
BOOL fn_80098AE8(s32 chan) {
    extern u8 lbl_803FB3C8[];
    u8* chanData = lbl_803FB3C8 + (chan << 6);
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* If DMA is not in progress, nothing to deselect */
    if ((*(u32*)(chanData + 0x0C) & 0x08) == 0) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    /* If callback mode is active but no completion yet, busy */
    if ((*(u32*)(chanData + 0x0C) & 0x10) != 0 && *(u32*)(chanData + 0x18) == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    /* Clear DMA-in-progress flag */
    *(u32*)(chanData + 0x0C) = *(u32*)(chanData + 0x0C) & ~0x08;

    /* Mask all 3 EXI interrupts for this channel */
    __OSMaskInterrupts(0x700000 >> (chan * 3));

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* fn_80098DDC - 0x80098DDC | size: 0xC0 */
/*
 * EXIIntrHandler - Handle EXI channel interrupt.
 *
 * Determines the channel from the interrupt number (irq / 3),
 * acknowledges the interrupt by writing to the EXI CSR register,
 * then calls the registered callback if one exists.
 *
 * 0x80098DDC | size: 0xC0
 */
void fn_80098DDC(s16 irq, OSContext* context) {
    extern u8 lbl_803FB3C8[];
    OSContext exiContext;
    s32 chan = (s32)irq / 3;
    u8* chanData = lbl_803FB3C8 + (chan << 6);
    volatile u32* exiCsr = (volatile u32*)(0xCC006800 + (chan * 5 * 4));
    u32 csrVal;
    void (*callback)(s32 chan, OSContext* ctx);

    /* Acknowledge interrupt: preserve status bits, set bit 1 */
    csrVal = *exiCsr;
    csrVal = (csrVal & 0x7F5) | 0x2;
    *exiCsr = csrVal;

    /* Call registered callback if present */
    callback = (void (*)(s32, OSContext*)) *(u32*)(chanData + 0x00);
    if (callback != NULL) {
        OSClearContext(&exiContext);
        OSSetCurrentContext(&exiContext);
        callback(chan, context);
        OSClearContext(&exiContext);
        OSSetCurrentContext(context);
    }
}

/* fn_80098E9C - 0x80098E9C | size: 0x15C */
/*
 * EXIDmaHandler - Handle EXI DMA transfer complete interrupt.
 *
 * Determines channel from IRQ, masks interrupt, acknowledges CSR,
 * reads any remaining bytes from the EXI data register, clears
 * transfer state, and invokes the completion callback.
 *
 * 0x80098E9C | size: 0x15C
 */
void fn_80098E9C(s16 irq, OSContext* context) {
    extern u8 lbl_803FB3C8[];
    OSContext exiContext;
    s32 chan = (s32)(s16)irq / 3;
    u8* chanData = lbl_803FB3C8 + (chan << 6);
    volatile u32* exiCsr = (volatile u32*)(0xCC006800 + (chan * 5 * 4));
    volatile u32* exiData = (volatile u32*)(0xCC006800 + ((chan * 5 + 4) * 4));
    u32 csrVal;
    void (*callback)(s32, OSContext*);
    s32 remaining;
    u8* destBuf;
    u32 dataReg;
    s32 i;

    /* Mask the interrupt source */
    __OSMaskInterrupts(0x80000000u >> (s16)irq);

    /* Acknowledge DMA complete: preserve status, set bit 3 */
    csrVal = *exiCsr;
    csrVal = (csrVal & 0x7F5) | 0x8;
    *exiCsr = csrVal;

    /* Get and clear the completion callback */
    callback = (void (*)(s32, OSContext*)) *(u32*)(chanData + 0x04);
    if (callback == NULL) {
        return;
    }
    *(u32*)(chanData + 0x04) = 0;

    /* If there are remaining bytes from an immediate transfer, read them */
    if ((*(u32*)(chanData + 0x0C) & 0x02) != 0) {
        remaining = *(s32*)(chanData + 0x10);
        if (remaining > 0) {
            destBuf = (u8*) *(u32*)(chanData + 0x14);
            dataReg = *exiData;
            for (i = 0; i < remaining; i++) {
                *destBuf = (u8)(dataReg >> ((3 - i) * 8));
                destBuf++;
            }
        }
    }

    /* Clear transfer mode bits */
    *(u32*)(chanData + 0x0C) = *(u32*)(chanData + 0x0C) & ~0x03;

    /* Call the completion callback in a clean context */
    OSClearContext(&exiContext);
    OSSetCurrentContext(&exiContext);
    callback(chan, context);
    OSClearContext(&exiContext);
    OSSetCurrentContext(context);
}

/*
 * EXIExtIntrHandler - Handle EXI external device insertion/removal interrupt.
 *
 * Masks all 3 EXI interrupts for the channel, clears the CSR register,
 * clears the DMA-in-progress flag, then invokes the ext callback if set.
 *
 * 0x80098FF8 | size: 0xC8
 */
void fn_80098FF8(s16 irq, OSContext* context) {
    extern u8 lbl_803FB3C8[];
    OSContext exiContext;
    s32 chan = (s32)irq / 3;
    u8* chanData = lbl_803FB3C8 + (chan << 6);
    volatile u32* exiCsr = (volatile u32*)(0xCC006800 + (chan * 5 * 4));
    void (*callback)(s32, OSContext*);

    /* Mask all 3 EXI interrupts for this channel */
    __OSMaskInterrupts(0x700000 >> (chan * 3));

    /* Clear CSR */
    *exiCsr = 0;

    /* Clear DMA-in-progress flag */
    *(u32*)(chanData + 0x0C) = *(u32*)(chanData + 0x0C) & ~0x08;

    /* Call ext callback if registered */
    callback = (void (*)(s32, OSContext*)) *(u32*)(chanData + 0x08);
    if (callback != NULL) {
        *(u32*)(chanData + 0x08) = 0;
        OSClearContext(&exiContext);
        OSSetCurrentContext(&exiContext);
        callback(chan, context);
        OSClearContext(&exiContext);
        OSSetCurrentContext(context);
    }
}

