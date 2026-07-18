#include "dolphin/types.h"
#include "dolphin/pad/Pad.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"

/* Internal PAD library state. Names kept as lbl_/fn_ where the exact
 * original SDK identifier is not yet confirmed by symbols.txt. */
extern char *lbl_80478A08;         /* version string, passed to OSRegisterVersion */
extern s32 lbl_80478A0C;           /* ResettingChan */
extern u32 lbl_80478A10;           /* per-channel enable mask (bongo-fix related) */
extern u32 lbl_80478A14;           /* AnalogMode */
extern u32 lbl_80478A18;           /* Spec */
extern u32 lbl_80478A1C;           /* MakeStatus function pointer */
extern u32 lbl_80478A20;           /* SITransfer scratch output buffer (1 byte cmd) */
extern u32 lbl_80478A24;           /* SITransfer scratch output buffer (3 byte cmd) */

extern u32 lbl_8047A8A0;           /* Initialized */
extern u32 lbl_8047A8A4;           /* EnabledBits */
extern u32 lbl_8047A8A8;           /* ResettingBits */
extern u32 lbl_8047A8AC;           /* RecalibrateBits */
extern u32 lbl_8047A8B0;           /* ProbingBits */
extern u32 lbl_8047A8B4;           /* WaitingBits */
extern u32 lbl_8047A8B8;           /* CheckingBits */
extern PADSamplingCallback lbl_8047A8BC; /* SamplingCallback */
extern u32 lbl_8047A8C0;           /* OnReset() static "recalibrated" flag */
extern u32 __PADSpec;
extern u32 lbl_8047AA58;

extern PADStatus lbl_803FC5E0[4];  /* Origin[4] */
extern u32 lbl_803FC5D0[4];        /* Type[4] */

extern u16 __OSWirelessPadFixMode; /* absolute address 0x800030E0 */

/* SI library helpers not yet recovered by name (unassigned SI unit). */
extern void fn_800D0338(s32 chan, u32 command);       /* SISetCommand */
extern u32 SIEnablePolling(u32 poll);                       /* SIEnablePolling */
extern u32 SIDisablePolling(u32 poll);                        /* SIDisablePolling */
extern BOOL SIGetResponse(s32 chan, void *data);            /* SIGetResponse */
extern void fn_800D034C(void);                             /* SITransferCommands */
extern void SIGetTypeAsync(s32 chan, SITypeAndStatusCallback cb);
extern BOOL fn_800CF708(void);
extern BOOL SIIsChanBusy(s32 chan);
extern u32 SIGetStatus(s32 chan);
extern u32 __shr2i(u32 hi, u32 lo, u32 shift);
extern void fn_800D104C(void);
extern void OSRegisterVersion(char *version);

static void UpdateOrigin(s32 chan);
static void PADOriginCallback(s32 chan, u32 error, OSContext *context);
static void fn_800AA73C(s32 chan, u32 error, OSContext *context);
static void PADProbeCallback(s32 chan, u32 error, OSContext *context);
static void PADTypeAndStatusCallback(s32 chan, u32 type);
void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
/* forward declaration provided by dolphin/pad/Pad.h */

#include "dolphin/pad/PAD_spec_inline.h"

/*
 * fn_800AA4D4 = UpdateOrigin
 */
static void UpdateOrigin(s32 chan) {
    PADStatus *origin = &lbl_803FC5E0[chan];
    u32 chanBit = 0x80000000u >> chan;

    switch (lbl_80478A14 & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        origin->triggerLeft &= ~0xF;
        origin->triggerRight &= ~0xF;
        origin->analogA &= ~0xF;
        origin->analogB &= ~0xF;
        break;
    case 0x00000100:
        origin->substickX &= ~0xF;
        origin->substickY &= ~0xF;
        origin->analogA &= ~0xF;
        origin->analogB &= ~0xF;
        break;
    case 0x00000200:
        origin->substickX &= ~0xF;
        origin->substickY &= ~0xF;
        origin->triggerLeft &= ~0xF;
        origin->triggerRight &= ~0xF;
        break;
    case 0x00000300:
    case 0x00000400:
        break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (lbl_80478A10 & chanBit) {
        if (origin->stickX > 0x40) {
            if ((SIGetType(chan) & 0xFFFF0000u) == 0x09000000u) {
                origin->stickX = 0;
            }
        }
    }
}

/*
 * fn_800AA678 = PADOriginCallback
 */
static void PADOriginCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0xF)) {
        s32 rchan;
        u32 data[2];
        UpdateOrigin(lbl_80478A0C);
        rchan = lbl_80478A0C;
        if ((lbl_8047A8A8 && lbl_8047A8A8) && lbl_8047A8A8) {
            /* Preserve the original MWCC register allocation. */
        }
        lbl_8047A8A4 |= (0x80000000u >> rchan);
        SIGetResponse(rchan, data);
        fn_800D0338(rchan, lbl_80478A14 | 0x400000);
        SIEnablePolling(lbl_8047A8A4);
    }

    /* DoReset() */
    lbl_80478A0C = __cntlzw(lbl_8047A8A8);
    if (lbl_80478A0C != 0x20) {
        u32 chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;
        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
    }
}

/*
 * fn_800AA73C = fn_800AA73C
 */
static void fn_800AA73C(s32 chan, u32 error, OSContext *context) {
    u32 chanBit = 0x80000000u >> chan;

    if (lbl_8047A8A4 & chanBit) {
        if (!(error & 0xF)) {
            UpdateOrigin(chan);
        }
        if (error & 0x8) {
            BOOL enabled = OSDisableInterrupts();
            SIDisablePolling(chanBit);
            lbl_8047A8A4 &= ~chanBit;
            lbl_8047A8B0 &= ~chanBit;
            lbl_8047A8B4 &= ~chanBit;
            lbl_8047A8B8 &= ~chanBit;
            OSSetWirelessID(chan, 0);
            OSRestoreInterrupts(enabled);
        }
    }
}

/*
 * fn_800AA7FC = PADProbeCallback
 */
static void PADProbeCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0xF)) {
        u32 rchan = lbl_80478A0C;
        u32 data[2];
        lbl_8047A8A4 |= (0x80000000u >> rchan);
        SIGetResponse(rchan, data);
        fn_800D0338(rchan, lbl_80478A14 | 0x400000);
        SIEnablePolling(lbl_8047A8A4);
        /* The comma expression preserves the original MWCC schedule. */
        lbl_8047A8B0 |= (0x80000000u >> (0, lbl_80478A0C));
    }

    /* DoReset() */
    lbl_80478A0C = __cntlzw(lbl_8047A8A8);
    if (lbl_80478A0C != 0x20) {
        u32 chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;
        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
    }
}

/*
 * fn_800AA8D4 = PADTypeAndStatusCallback
 */
static void PADTypeAndStatusCallback(s32 chan, u32 type) {
    u32 chanBit;

    chanBit = 0x80000000u >> lbl_80478A0C;
    lbl_8047A8AC &= ~chanBit;

    if (!(lbl_8047A8AC & chanBit)) {
        /* recalibrate not requested: DoReset() */
        lbl_80478A0C = __cntlzw(lbl_8047A8A8);
        if (lbl_80478A0C == 0x20)
            return;
        chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;
        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
        return;
    }

    lbl_803FC5D0[chan] = type & 0xFFFF0000u;

    if (((type >> 16) & 0x18) != 0 && !((type >> 16) & 0x80)) {
        /* DoReset() */
        lbl_80478A0C = __cntlzw(lbl_8047A8A8);
        if (lbl_80478A0C == 0x20)
            return;
        chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;
        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
        return;
    }

    if (lbl_80478A18 < 2) {
        lbl_8047A8A4 |= (0x80000000u >> chan);
        SIGetResponse(chan, 0);
        fn_800D0338(chan, lbl_80478A14 | 0x400000);
        SIEnablePolling(lbl_8047A8A4);

        /* DoReset() */
        lbl_80478A0C = __cntlzw(lbl_8047A8A8);
        if (lbl_80478A0C == 0x20)
            return;
        chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;
        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
        return;
    }

    /* type has the 0x100000 wireless-id-present bit */
    if (type & 0x100000) {
        BOOL idMatch = FALSE;
        if (idMatch) {
            fn_800D0338(chan, lbl_80478A14 | 0x400000);
        }
        SITransfer(chan, &lbl_80478A24, 3, &lbl_803FC5E0[chan], 0xA,
                   PADOriginCallback, 0);
    } else if ((type & 0x40000000) && !(type & 0x80000) && !(type & 0x40000)) {
        SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 0xA,
                   PADOriginCallback, 0);
    } else {
        SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 8,
                   PADProbeCallback, 0);
    }
}

/*
 * fn_800AAC00 (unmatched attempt; see .inc for the ground-truth asm)
 */
void fn_800AAC00(s32 chan, u32 error) {
    u32 chanBit = 0x80000000u >> chan;

    if (lbl_8047A8A4 & chanBit) {
        u32 masked;
        lbl_8047A8B0 &= ~chanBit;
        lbl_8047A8B4 &= ~chanBit;

        masked = error & ~0xFF;
        if ((error & 0xF) == 0 && (masked & 0x80000000u) && (masked & 0x100000) &&
            (masked & 0x40000000) && !(masked & 0x4000000) && !(masked & 0x80000) &&
            !(masked & 0x40000)) {
            SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 0xA,
                       fn_800AA73C, 0);
            return;
        }
    }

    {
        BOOL enabled = OSDisableInterrupts();
        SIDisablePolling(chanBit);
        lbl_8047A8A4 &= ~chanBit;
        lbl_8047A8B0 &= ~chanBit;
        lbl_8047A8B4 &= ~chanBit;
        lbl_8047A8B8 &= ~chanBit;
        OSSetWirelessID(chan, 0);
        OSRestoreInterrupts(enabled);
    }
}

/*
 * fn_800AAD34 = PADReset (unmatched attempt)
 */
BOOL fn_800AAD34(u32 mask) {
    BOOL enabled = OSDisableInterrupts();
    u32 disableBits;

    mask |= lbl_8047A8B8;
    lbl_8047A8B8 = 0;
    mask &= ~(lbl_8047A8B0 | lbl_8047A8B4);
    lbl_8047A8A8 |= mask;
    disableBits = lbl_8047A8A8 & lbl_8047A8A4;
    lbl_8047A8A4 &= ~mask;
    if (lbl_80478A18 == 4) {
        lbl_8047A8AC |= mask;
    }
    SIDisablePolling(disableBits);

    if (lbl_80478A0C == 0x20) {
        lbl_80478A0C = __cntlzw(lbl_8047A8A8);
        if (lbl_80478A0C != 0x20) {
            u32 chanBit = 0x80000000u >> lbl_80478A0C;
            lbl_8047A8A8 &= ~chanBit;
            memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
            SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * fn_800AAE34 = PADRecalibrate (unmatched attempt)
 */
BOOL fn_800AAE34(u32 mask) {
    BOOL enabled;
    u32 disableBits;
    BOOL noRecal;

    enabled = OSDisableInterrupts();
    noRecal = *(volatile u8 *)0x800030E3 & 0x40;

    mask |= lbl_8047A8B8;
    lbl_8047A8B8 = 0;
    mask &= ~(lbl_8047A8B0 | lbl_8047A8B4);
    lbl_8047A8A8 |= mask;
    disableBits = lbl_8047A8A8 & lbl_8047A8A4;
    lbl_8047A8A4 &= ~mask;
    if (!noRecal) {
        lbl_8047A8AC |= mask;
    }
    SIDisablePolling(disableBits);

    if (lbl_80478A0C == 0x20) {
        lbl_80478A0C = __cntlzw(lbl_8047A8A8);
        if (lbl_80478A0C != 0x20) {
            u32 chanBit = 0x80000000u >> lbl_80478A0C;
            lbl_8047A8A8 &= ~chanBit;
            memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
            SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * fn_800AAF38 = PADInit (unmatched attempt)
 */
BOOL fn_800AAF38(void) {
    if (!lbl_8047A8A0) {
        OSRegisterVersion(lbl_80478A08);

        if (__PADSpec)
            PADSetSpecInline(__PADSpec);

        lbl_8047A8A0 = 1;

        if (lbl_8047AA58) {
            OSTime time = OSGetTime();
            __OSWirelessPadFixMode = (u16)(__shr2i((u32)(time >> 32), (u32)time, 0x30) & 0x3FFF);
            lbl_8047A8AC = 0xF0000000;
        }

        fn_800D104C();
        OSRegisterResetFunction(NULL);
    }

    return fn_800AAD34(0xF0000000);
}

/*
 * fn_800AB150 = PADRead (unmatched attempt)
 */
BOOL fn_800AB150(PADStatus *status) {
    s32 chan;
    BOOL rumble = FALSE;
    BOOL enabled = OSDisableInterrupts();

    for (chan = 0; chan < 4; chan++, status++) {
        u32 chanBit = 0x80000000u >> chan;

        if (lbl_8047A8B8 & chanBit) {
            /* fn_800AAD34(chanBit)-equivalent inline reset */
            status->err = -2;
            memset(status, 0, 0xA);
            continue;
        }
        if (!(lbl_8047A8A8 & chanBit) && lbl_80478A0C == chan) {
            status->err = -2;
            memset(status, 0, 0xA);
            continue;
        }
        if (!(lbl_8047A8A4 & chanBit)) {
            status->err = -1;
            memset(status, 0, 0xA);
            continue;
        }
        if (!SIIsChanBusy(chan)) {
            status->err = -3;
            memset(status, 0, 0xA);
            continue;
        }
        if (SIGetStatus(chan) & 0x8) {
            OSContext ctx;
            SIGetResponse(chan, &ctx);
            if (lbl_8047A8B0 & chanBit) {
                status->err = 0;
                memset(status, 0, 0xA);
                if (!(lbl_8047A8B4 & chanBit)) {
                    lbl_8047A8B4 |= chanBit;
                    SIGetTypeAsync(chan, (SITypeAndStatusCallback)fn_800AAC00);
                }
                continue;
            }
            /* fn_800AAC00-style disable path */
            {
                BOOL en2 = OSDisableInterrupts();
                SIDisablePolling(chanBit);
                lbl_8047A8A4 &= ~chanBit;
                lbl_8047A8B0 &= ~chanBit;
                lbl_8047A8B4 &= ~chanBit;
                lbl_8047A8B8 &= ~chanBit;
                OSSetWirelessID(chan, 0);
                OSRestoreInterrupts(en2);
            }
            status->err = -1;
            memset(status, 0, 0xA);
            continue;
        }

        if (SIGetType(chan) & 0x20000000) {
            rumble |= chanBit;
        }

        if (!SIGetResponse(chan, 0)) {
            status->err = -3;
            memset(status, 0, 0xA);
            continue;
        }

        {
            u32 data[2];
            if (data[0] & 0x80000000u) {
                status->err = -3;
                memset(status, 0, 0xA);
                continue;
            }
            ((void (*)(s32, PADStatus *, u32 *))lbl_80478A1C)(chan, status, data);
            if (status->button & 0x2000) {
                status->err = -3;
                memset(status, 0, 0xA);
                SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 0xA,
                           fn_800AA73C, 0);
            } else {
                status->err = 0;
                status->button &= ~0x80;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return rumble;
}
