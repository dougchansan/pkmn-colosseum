#include "dolphin/types.h"
#include "dolphin/pad/Pad.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSReset.h"
#include "dolphin/os/OSTime.h"

/* Internal PAD library state. Names kept as lbl_/fn_ where the exact
 * original SDK identifier is not yet confirmed by symbols.txt. */
extern char *lbl_80478A08;         /* __PADVersion */
extern s32 lbl_80478A0C;           /* ResettingChan */
extern u32 lbl_80478A10;           /* XPatchBits */
extern u32 lbl_80478A14;           /* AnalogMode */
extern u32 lbl_80478A18;           /* Spec */
extern u32 lbl_80478A1C;           /* MakeStatus function pointer */
extern u32 lbl_80478A20;           /* CmdReadOrigin */
extern u32 lbl_80478A24;           /* CmdCalibrate */

extern BOOL lbl_8047A8A0;          /* Initialized */
extern u32 lbl_8047A8A4;           /* EnabledBits */
extern u32 lbl_8047A8A8;           /* ResettingBits */
extern u32 lbl_8047A8AC;           /* RecalibrateBits */
extern u32 lbl_8047A8B0;           /* WaitingBits */
extern u32 lbl_8047A8B4;           /* CheckingBits */
extern u32 lbl_8047A8B8;           /* PendingBits */
extern PADSamplingCallback lbl_8047A8BC; /* SamplingCallback */
extern u32 __PADSpec;
extern u32 lbl_8047AA58;           /* __PADFixBits */

extern OSResetFunctionInfo lbl_80312500; /* ResetFunctionInfo */

/* Type[4], Origin[4] and CmdProbeDevice[4] are contiguous TU-local .bss. */
static u32 lbl_803FC5D0[4];        /* Type[4] */
static PADStatus lbl_803FC5E0[4];  /* Origin[4] */
static u32 CmdProbeDevice[4];

extern u16 __OSWirelessPadFixMode : 0x800030E0;
extern u8 GameChoice : 0x800030E3;

/* SI library helpers not yet recovered by name (unassigned SI unit). */
extern void SISetCommand(s32 chan, u32 command);       /* SISetCommand */
extern u32 SIEnablePolling(u32 poll);
extern u32 SIDisablePolling(u32 poll);
extern BOOL SIGetResponse(s32 chan, void *data);
extern void SIGetTypeAsync(s32 chan, SITypeAndStatusCallback cb);
extern BOOL SIIsChanBusy(s32 chan);
extern u32 SIGetStatus(s32 chan);
extern void SIRefreshSamplingRate(void);                        /* SIRefreshSamplingRate */
extern void OSRegisterVersion(char *version);

static void UpdateOrigin(s32 chan);
static void PADOriginCallback(s32 chan, u32 error, OSContext *context);
static void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context);
static void PADProbeCallback(s32 chan, u32 error, OSContext *context);
static void PADTypeAndStatusCallback(s32 chan, u32 type);
void PADSetSpec(s32 spec);

static void PADEnable(s32 chan) {
    u32 cmd;
    u32 chanBit;
    u32 data[2];

    chanBit = 0x80000000u >> chan;
    lbl_8047A8A4 |= chanBit;
    SIGetResponse(chan, data);
    cmd = (0x40 << 16) | lbl_80478A14;
    SISetCommand(chan, cmd);
    SIEnablePolling(lbl_8047A8A4);
}

static void PADDisable(s32 chan) {
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();

    chanBit = 0x80000000u >> chan;
    SIDisablePolling(chanBit);
    lbl_8047A8A4 &= ~chanBit;
    lbl_8047A8B0 &= ~chanBit;
    lbl_8047A8B4 &= ~chanBit;
    lbl_8047A8B8 &= ~chanBit;
    OSSetWirelessID(chan, 0);

    OSRestoreInterrupts(enabled);
}

static void DoReset(void) {
    u32 chanBit;

    lbl_80478A0C = __cntlzw(lbl_8047A8A8);
    if (lbl_80478A0C != 32) {
        chanBit = 0x80000000u >> lbl_80478A0C;
        lbl_8047A8A8 &= ~chanBit;

        memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
        SIGetTypeAsync(lbl_80478A0C, PADTypeAndStatusCallback);
    }
}

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
        UpdateOrigin(lbl_80478A0C);
        PADEnable(lbl_80478A0C);
    }
    DoReset();
}

/*
 * PADOriginUpdateCallback = PADOriginUpdateCallback
 */
static void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context) {
    if (!(lbl_8047A8A4 & (0x80000000u >> chan))) {
        return;
    }

    if (!(error & 0xF)) {
        UpdateOrigin(chan);
    }

    if (error & 0x8) {
        PADDisable(chan);
    }
}

/*
 * fn_800AA7FC = PADProbeCallback
 */
static void PADProbeCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0xF)) {
        PADEnable(lbl_80478A0C);
        lbl_8047A8B0 |= 0x80000000u >> lbl_80478A0C;
    }
    DoReset();
}

/*
 * fn_800AA8D4 = PADTypeAndStatusCallback
 */
static void PADTypeAndStatusCallback(s32 chan, u32 type) {
    u32 chanBit;
    u32 recalibrate;
    BOOL rc = TRUE;
    u32 error;

    chanBit = 0x80000000u >> lbl_80478A0C;
    error = type & 0xFF;
    recalibrate = lbl_8047A8AC & chanBit;
    lbl_8047A8AC &= ~chanBit;

    if (error & 0xF) {
        DoReset();
        return;
    }

    type &= ~0xFF;
    lbl_803FC5D0[lbl_80478A0C] = type;

    if ((type & 0x18000000) != 0x08000000 || !(type & 0x01000000)) {
        DoReset();
        return;
    }

    if (lbl_80478A18 < 2) {
        PADEnable(lbl_80478A0C);
        DoReset();
        return;
    }

    if (!(type & 0x80000000) || (type & 0x04000000)) {
        if (recalibrate) {
            rc = SITransfer(lbl_80478A0C, &lbl_80478A24, 3, &lbl_803FC5E0[lbl_80478A0C], 10,
                            PADOriginCallback, 0);
        } else {
            rc = SITransfer(lbl_80478A0C, &lbl_80478A20, 1, &lbl_803FC5E0[lbl_80478A0C], 10,
                            PADOriginCallback, 0);
        }
    } else if ((type & 0x00100000) && !(type & 0x00080000) && !(type & 0x00040000)) {
        if (type & 0x40000000) {
            rc = SITransfer(lbl_80478A0C, &lbl_80478A20, 1, &lbl_803FC5E0[lbl_80478A0C], 10,
                            PADOriginCallback, 0);
        } else {
            rc = SITransfer(lbl_80478A0C, &CmdProbeDevice[lbl_80478A0C], 3,
                            &lbl_803FC5E0[lbl_80478A0C], 8, PADProbeCallback, 0);
        }
    }

    if (!rc) {
        lbl_8047A8B8 |= chanBit;
        DoReset();
        return;
    }
}

/*
 * PADReceiveCheckCallback = PADReceiveCheckCallback
 */
static void PADReceiveCheckCallback(s32 chan, u32 type) {
    u32 error;
    u32 chanBit;

    chanBit = 0x80000000u >> chan;
    if (lbl_8047A8A4 & chanBit) {
        error = type & 0xFF;
        type &= ~0xFF;

        lbl_8047A8B0 &= ~chanBit;
        lbl_8047A8B4 &= ~chanBit;

        if (!(error & 0xF) && (type & 0x80000000) && (type & 0x00100000) &&
            (type & 0x40000000) && !(type & 0x04000000) && !(type & 0x00080000) &&
            !(type & 0x00040000)) {
            SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 10, PADOriginUpdateCallback, 0);
        } else {
            PADDisable(chan);
        }
    }
}

/*
 * fn_800AAD34 = PADReset
 */
BOOL fn_800AAD34(u32 mask) {
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();

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

    if (lbl_80478A0C == 32) {
        DoReset();
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * PADRecalibrate = PADRecalibrate
 */
BOOL PADRecalibrate(u32 mask) {
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();

    mask |= lbl_8047A8B8;
    lbl_8047A8B8 = 0;
    mask &= ~(lbl_8047A8B0 | lbl_8047A8B4);
    lbl_8047A8A8 |= mask;
    disableBits = lbl_8047A8A8 & lbl_8047A8A4;
    lbl_8047A8A4 &= ~mask;

    if (!(GameChoice & 0x40)) {
        lbl_8047A8AC |= mask;
    }

    SIDisablePolling(disableBits);

    if (lbl_80478A0C == 32) {
        DoReset();
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

/*
 * PADInit = PADInit
 */
BOOL PADInit(void) {
    s32 chan;

    if (lbl_8047A8A0) {
        return TRUE;
    }

    OSRegisterVersion(lbl_80478A08);

    if (__PADSpec) {
        PADSetSpec(__PADSpec);
    }

    lbl_8047A8A0 = TRUE;

    if (lbl_8047AA58 != 0) {
        OSTime time = OSGetTime();
        __OSWirelessPadFixMode =
            (u16)((((time) & 0xffff) + ((time >> 16) & 0xffff) + ((time >> 32) & 0xffff) +
                   ((time >> 48) & 0xffff)) &
                  0x3fffu);
        lbl_8047A8AC = 0xF0000000;
    }

    for (chan = 0; chan < 4; ++chan) {
        CmdProbeDevice[chan] =
            (0x4D << 24) | (chan << 22) | ((__OSWirelessPadFixMode & 0x3fffu) << 8);
    }

    SIRefreshSamplingRate();
    OSRegisterResetFunction(&lbl_80312500);

    return fn_800AAD34(0xF0000000);
}

/*
 * PADRead = PADRead
 */
u32 PADRead(PADStatus *status) {
    BOOL enabled;
    s32 chan;
    u32 data[2];
    u32 chanBit;
    u32 sr;
    int chanShift;
    u32 motor;

    enabled = OSDisableInterrupts();

    motor = 0;
    for (chan = 0; chan < 4; chan++, status++) {
        chanBit = 0x80000000u >> chan;
        chanShift = 8 * (4 - 1 - chan);

        if (lbl_8047A8B8 & chanBit) {
            fn_800AAD34(0);
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, 10);
            continue;
        }

        if ((lbl_8047A8A8 & chanBit) || lbl_80478A0C == chan) {
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, 10);
            continue;
        }

        if (!(lbl_8047A8A4 & chanBit)) {
            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, 10);
            continue;
        }

        if (SIIsChanBusy(chan)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        sr = SIGetStatus(chan);
        if (sr & 0x8) {
            SIGetResponse(chan, data);

            if (lbl_8047A8B0 & chanBit) {
                status->err = (s8)PAD_ERR_NONE;
                memset(status, 0, 10);

                if (!(lbl_8047A8B4 & chanBit)) {
                    lbl_8047A8B4 |= chanBit;
                    SIGetTypeAsync(chan, PADReceiveCheckCallback);
                }
                continue;
            }

            PADDisable(chan);

            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, 10);
            continue;
        }

        if (!(SIGetType(chan) & 0x20000000)) {
            motor |= chanBit;
        }

        if (!SIGetResponse(chan, data)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        if (data[0] & 0x80000000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        ((void (*)(s32, PADStatus *, u32 *))lbl_80478A1C)(chan, status, data);

        if (status->button & 0x2000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            SITransfer(chan, &lbl_80478A20, 1, &lbl_803FC5E0[chan], 10, PADOriginUpdateCallback, 0);
            continue;
        }

        status->err = PAD_ERR_NONE;
        status->button &= ~0x0080;
    }

    OSRestoreInterrupts(enabled);
    return motor;
}

/* ---- 0x800AB4FC ---- */
/**
 * @file PAD_exact_800AB4FC.c
 * @brief Exact Dolphin PAD island, 0x800AB4FC - 0x800ABD68.
 */

extern u32 lbl_80478A14;
extern u32 lbl_80478A18;
extern u32 lbl_80478A1C;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 __PADSpec;

extern void SISetCommand(s32 chan, u32 command);
extern void SITransferCommands(void);

void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);

#include "dolphin/pad/PAD_spec_inline.h"

void fn_800AB4FC(const u32 *commandArray) {
    BOOL enabled = OSDisableInterrupts();
    s32 chan;
    BOOL commit = FALSE;

    for (chan = 0; chan < 4; chan++, commandArray++) {
        u32 chanBit = 0x80000000u >> chan;
        if (!(lbl_8047A8A4 & chanBit))
            continue;
        if (SIGetType(chan) & 0x20000000)
            continue;

        {
            u32 command = *commandArray;
            if (lbl_80478A18 < 2 && command == 2)
                command = 0;
            SISetCommand(chan, (lbl_80478A14 | 0x400000) | (command & 3));
            commit = TRUE;
        }
    }

    if (commit)
        SITransferCommands();

    OSRestoreInterrupts(enabled);
}

void PADSetSpec(s32 spec) {
    PADSetSpecInline(spec);
}

void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0008) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0001) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_START : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0080) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0200) ? PAD_BUTTON_START : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static inline s8 ClampS8(s8 var, s8 org) {
    if (0 < org) {
        s8 min = (s8)(-128 + org);
        if (var < min)
            var = min;
    } else if (org < 0) {
        s8 max = (s8)(127 + org);
        if (max < var)
            var = max;
    }
    return var -= org;
}

static inline u8 ClampU8(u8 var, u8 org) {
    if (var < org)
        var = org;
    return var -= org;
}

void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    PADStatus *origin;

    status->button = (u16)((data[0] >> 16) & 0x3FFF);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);

    switch (lbl_80478A14 & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0f) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0f) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    case 0x00000300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1] >> 0);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x00000400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    origin = &lbl_803FC5E0[chan];
    status->stickX = ClampS8(status->stickX, origin->stickX);
    status->stickY = ClampS8(status->stickY, origin->stickY);
    status->substickX = ClampS8(status->substickX, origin->substickX);
    status->substickY = ClampS8(status->substickY, origin->substickY);
    status->triggerLeft = ClampU8(status->triggerLeft, origin->triggerLeft);
    status->triggerRight = ClampU8(status->triggerRight, origin->triggerRight);
}

void PADSetAnalogMode(s32 mode) {
    BOOL enabled = OSDisableInterrupts();
    u32 mask = lbl_8047A8A4;

    lbl_80478A14 = mode << 8;
    lbl_8047A8A4 &= ~mask;
    lbl_8047A8B0 &= ~mask;
    lbl_8047A8B4 &= ~mask;

    SIDisablePolling(mask);

    OSRestoreInterrupts(enabled);
}

/* ---- 0x800ABD68 ---- */
/**
 * @file PAD_suffix_800ABD68.c
 * @brief Dolphin PAD suffix, 0x800ABD68 - 0x800AC02C.
 */

extern s32 lbl_80478A0C;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8A8;
extern u32 lbl_8047A8AC;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8B8;
extern PADSamplingCallback lbl_8047A8BC;
extern BOOL lbl_8047A8C0;

extern u32 SIDisablePolling(u32 poll);
extern void SIGetTypeAsync(s32 chan, SITypeAndStatusCallback cb);
extern BOOL SIBusy(void);

/*
 * PADSync, as in the SDK's PAD.c. Nothing in the game calls it, so the
 * linker strips it; OnReset (fn_800ABD68) inlines it, as it does
 * PADRecalibrate (PADRecalibrate).
 */
/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped function — see docs/RULE_EXCEPTIONS.md */
BOOL PADSync(void) {
    return lbl_8047A8A8 == 0 && lbl_80478A0C == 0x20 && !SIBusy();
}


BOOL fn_800ABD68(BOOL final) {
    BOOL sync;

    if (lbl_8047A8BC != NULL) {
        PADSetSamplingCallback(0);
    }

    if (final == 0) {
        sync = PADSync();
        if (lbl_8047A8C0 == 0 && sync) {
            lbl_8047A8C0 = PADRecalibrate(0xF0000000);
            return FALSE;
        }
        return sync;
    }

    lbl_8047A8C0 = 0;
    return TRUE;
}

static void SamplingHandler(__OSInterrupt interrupt, OSContext *context) {
    OSContext newContext;
    if (lbl_8047A8BC != NULL) {
        OSClearContext(&newContext);
        OSSetCurrentContext(&newContext);
        lbl_8047A8BC();
        OSClearContext(&newContext);
        OSSetCurrentContext(context);
    }
}

PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback callback) {
    PADSamplingCallback old = lbl_8047A8BC;
    lbl_8047A8BC = callback;
    if (callback != NULL) {
        SIRegisterPollingHandler((__OSInterruptHandler)SamplingHandler);
    } else {
        SIUnregisterPollingHandler((__OSInterruptHandler)SamplingHandler);
    }
    return old;
}

BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL old;
    BOOL enabled = OSDisableInterrupts();
    int flags;

    old = (*(volatile u8 *)0x800030E3 & 0x40) ? TRUE : FALSE;
    flags = *(volatile u8 *)0x800030E3;
    flags &= 0xbf;
    *(volatile u8 *)0x800030E3 = flags;
    if (disable) {
        *(volatile u8 *)0x800030E3 = *(volatile u8 *)0x800030E3 | 0x40;
    }

    OSRestoreInterrupts(enabled);
    return old;
}
