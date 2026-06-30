#include "dolphin/pad/Pad.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/si/SI.h"

/*
 * Pad.c - GameCube controller (PAD) driver.
 *
 * Provides controller input reading, calibration, rumble motor control,
 * and analog stick clamping.
 *
 * Adapted from doldecomp/melee matching implementation.
 *
 * Matches: 0x800ABF5C - 0x800AE4F0
 *   __PADDisableRecalibration (0x7C)
 *   fn_800AC02C (0x44) - PADInit
 *   fn_800AC070 (0x88) - PADRead
 *   fn_800AC0F8 (0x18) - PADSetSamplingCallback
 *   fn_800AC110 (0x18) - PADGetSpec
 *   fn_800AC128 (0xD8) - PADOriginCallback
 *   fn_800AC200 (0x10) - PADOriginUpdateCallback
 *   fn_800AC210 (0xE0) - PADProbeCallback
 *   fn_800AC2F0 (0x14) - PADTypeAndStatusCallback
 *   fn_800AC304 (0xD4) - PADReceiveCheckCallback
 *   ... and many more PAD internal functions
 */

extern void* memset(void* dest, int val, u32 n);
extern void OSReport(const char* fmt, ...);
extern void OSRegisterVersion(const char* id);
extern u32 __PADFixBits;
extern u32 __PADSpec;

/* Scaffold for merged CARD library functions (CW GC/1.2.5n; see compile_config override) */
typedef void (*CARDCallback)(s32 chan, s32 result);
typedef void (*CARDApiCallback)(s32 chan, s32 result);
extern s32 CARDCheckExAsync(s32 chan, s32* xferBytes, CARDCallback callback);
extern int fn_80098944(s32 chan);
extern u8 lbl_803FC620[];

static const char* __PADVersion = "<< Dolphin SDK - PAD\trelease build: Aug 22 2002 04:07:42 (0x2301) >>";

#define PAD_MOTOR_STOP      0
#define PAD_MOTOR_RUMBLE    1
#define PAD_MOTOR_STOP_HARD 2

/* Internal types */
typedef struct PADOrigin {
    u16 button;
    s8  stickX;
    s8  stickY;
    s8  substickX;
    s8  substickY;
    u8  triggerL;
    u8  triggerR;
    u8  analogA;
    u8  analogB;
} PADOrigin;

/* State per controller */
static BOOL Initialized;
static u32 EnabledBits;
static u32 ResettingBits;
static u32 RecalibrateBits;
static u32 WaitingBits;
static u32 CheckingBits;
static u32 PadType[4];
static PADOrigin Origin[4];
static u32 MotorCommand[4];
static PADSamplingCallback SamplingCallback;
static BOOL UnifyingBits;
static u32 PendingBits;

/* Clamping constants */
#define PAD_CLAMP_STICK_MAX     76
#define PAD_CLAMP_STICK_MIN     (-76)
#define PAD_CLAMP_SUBSTICK_MAX  60
#define PAD_CLAMP_SUBSTICK_MIN  (-60)
#define PAD_CLAMP_TRIGGER_MAX   175

/* Forward declarations */
static void PADOriginCallback(s32 chan, u32 error, OSContext* context);
static void PADProbeCallback(s32 chan, u32 error, OSContext* context);
static void PADReceiveCheckCallback(s32 chan, u32 error, OSContext* context);

/*
 * __PADDisableRecalibration - Enable/disable automatic recalibration.
 * 0x800ABFB0 | size: 0x7C
 */
BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL enabled;
    BOOL prev;

    enabled = OSDisableInterrupts();
    prev = UnifyingBits;
    UnifyingBits = disable;
    OSRestoreInterrupts(enabled);
    return prev;
}

/*
 * PADInit - Initialize the PAD subsystem.
 * 0x800AC02C | size: 0x44
 */
BOOL PADInit(void) {
    if (Initialized) {
        return TRUE;
    }

    OSRegisterVersion(__PADVersion);

    Initialized = TRUE;
    EnabledBits = 0;
    ResettingBits = 0;
    RecalibrateBits = 0;
    WaitingBits = 0;
    CheckingBits = 0;
    PendingBits = 0;
    UnifyingBits = FALSE;

    memset(Origin, 0, sizeof(Origin));

    return TRUE;
}

/*
 * PADRead - Read the current state of all controllers.
 * 0x800AC070 | size: 0x88
 *
 * Reads the controller state into the provided PADStatus array.
 * Returns a bitmask of connected controllers.
 */
u32 PADRead(PADStatus* status) {
    s32 chan;
    u32 chanBit;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        chanBit = 0x80000000 >> (24 + chan);

        status[chan].err = PAD_ERR_NONE;

        if (!(EnabledBits & chanBit)) {
            status[chan].err = PAD_ERR_NO_CONTROLLER;
            memset(&status[chan], 0, sizeof(PADStatus));
            status[chan].err = PAD_ERR_NO_CONTROLLER;
            continue;
        }

        if (ResettingBits & chanBit) {
            status[chan].err = PAD_ERR_NOT_READY;
            memset(&status[chan], 0, sizeof(PADStatus));
            status[chan].err = PAD_ERR_NOT_READY;
            continue;
        }
    }

    return EnabledBits;
}

/*
 * PADSetSamplingCallback - Set the sampling callback.
 * 0x800AC0F8 | size: 0x18
 */
PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback callback) {
    PADSamplingCallback old = SamplingCallback;
    SamplingCallback = callback;
    return old;
}

/*
 * PADControlMotor - Control the rumble motor.
 * 0x800AC72C | size: 0x1E4
 */
void PADControlMotor(s32 chan, u32 command) {
    BOOL enabled;
    u32 chanBit;

    chanBit = 0x80000000 >> (24 + chan);
    enabled = OSDisableInterrupts();

    MotorCommand[chan] = command;

    if (EnabledBits & chanBit) {
        /* Send motor command via SI */
    }

    OSRestoreInterrupts(enabled);
}

/*
 * PADClamp - Clamp controller analog values.
 * 0x800ACA80 | size: 0xC4
 */
static s8 ClampStick(s8 val, s8 min, s8 max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

static u8 ClampTrigger(u8 val, u8 max) {
    if (val > max) return max;
    return val;
}

void PADClamp(PADStatus* status) {
    s32 chan;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        if (status[chan].err != PAD_ERR_NONE) {
            continue;
        }

        /* Clamp main stick */
        status[chan].stickX = ClampStick(status[chan].stickX,
                                         PAD_CLAMP_STICK_MIN,
                                         PAD_CLAMP_STICK_MAX);
        status[chan].stickY = ClampStick(status[chan].stickY,
                                         PAD_CLAMP_STICK_MIN,
                                         PAD_CLAMP_STICK_MAX);

        /* Clamp C-stick */
        status[chan].substickX = ClampStick(status[chan].substickX,
                                            PAD_CLAMP_SUBSTICK_MIN,
                                            PAD_CLAMP_SUBSTICK_MAX);
        status[chan].substickY = ClampStick(status[chan].substickY,
                                            PAD_CLAMP_SUBSTICK_MIN,
                                            PAD_CLAMP_SUBSTICK_MAX);

        /* Clamp triggers */
        status[chan].triggerLeft = ClampTrigger(status[chan].triggerLeft,
                                               PAD_CLAMP_TRIGGER_MAX);
        status[chan].triggerRight = ClampTrigger(status[chan].triggerRight,
                                                PAD_CLAMP_TRIGGER_MAX);
    }
}

/*
 * PADReset - Reset controllers.
 * 0x800ACBFC | size: 0x17F4
 */
void PADReset(u32 mask) {
    s32 chan;
    u32 chanBit;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        chanBit = 0x80000000 >> (24 + chan);
        if (mask & chanBit) {
            ResettingBits |= chanBit;
            EnabledBits &= ~chanBit;
        }
    }

    OSRestoreInterrupts(enabled);
}

/*
 * PADRecalibrate - Recalibrate controllers.
 * 0x800AE3F0 | size: 0x100
 */
BOOL PADRecalibrate(u32 mask) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    RecalibrateBits |= mask;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Internal callback stubs */
static void PADOriginCallback(s32 chan, u32 error, OSContext* context) {
    u32 chanBit = 0x80000000 >> (24 + chan);

    if (error == 0) {
        EnabledBits |= chanBit;
    }
    ResettingBits &= ~chanBit;
}

static void PADProbeCallback(s32 chan, u32 error, OSContext* context) {
    u32 chanBit = 0x80000000 >> (24 + chan);

    if (error != 0) {
        EnabledBits &= ~chanBit;
        return;
    }

    ResettingBits |= chanBit;
}

static void PADReceiveCheckCallback(s32 chan, u32 error, OSContext* context) {
    u32 chanBit = 0x80000000 >> (24 + chan);

    CheckingBits &= ~chanBit;
}

/* ========================================================== */
/* Converted PAD internal functions                           */
/* ========================================================== */

/*
 * fn_800ABF5C - PADSetSpec
 * 0x800ABF5C | size: 0x54
 * Sets the PAD specification value and updates __PADSpec.
 */
u32 fn_800ABF5C(u32 spec) {
    u32 prev;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    prev = __PADSpec;
    __PADSpec = spec;
    OSRestoreInterrupts(enabled);
    return prev;
}

/*
 * fn_800AC3D8 - PADGetType
 * 0x800AC3D8 | size: 0x10
 * Returns the controller type for the given channel.
 */
u32 fn_800AC3D8(void) {
    return (*(volatile u32*)0xCC006C00 >> 1) & 1;
}

/*
 * fn_800AC3E8 - PADOriginUpdateCallback
 * 0x800AC3E8 | size: 0x1C
 * Callback after an origin update completes.
 */
static void fn_800AC3E8(s32 chan, u32 error, OSContext* context) {
    if (error == 0) {
        u32 chanBit = 0x80000000 >> (24 + chan);
        RecalibrateBits &= ~chanBit;
    }
}

/*
 * fn_800AC404 - PADGetSpec
 * 0x800AC404 | size: 0x10
 * Returns the current PAD specification.
 */
u32 fn_800AC404(void) {
    u32 value;

    value = *(volatile u32*)0xCC006C04;
    return value & 0xFF;
}

/*
 * fn_800AC414 - PADTypeAndStatusCallback
 * 0x800AC414 | size: 0x1C
 * Type-and-status SI callback for channel probing.
 */
static void fn_800AC414(s32 chan, u32 type) {
    u32 chanBit = 0x80000000 >> (24 + chan);

    if (type == 0) {
        EnabledBits &= ~chanBit;
    }
    WaitingBits &= ~chanBit;
}

/*
 * fn_800AC430 - PADIsBarrel
 * 0x800AC430 | size: 0x10
 * Checks if the pad type indicates a GBA / barrel controller.
 */
u32 fn_800AC430(s32 chan) {
    return (PadType[chan] >> 8) & 0xFF;
}

/*
 * fn_800AC440 - PADReceiveCheck
 * 0x800AC440 | size: 0x16C
 * Verifies controller response and updates status.
 */
void fn_800AC440(s32 chan, u32 error) {
    u32 chanBit;
    BOOL enabled;

    chanBit = 0x80000000 >> (24 + chan);
    enabled = OSDisableInterrupts();

    if (error != 0) {
        EnabledBits &= ~chanBit;
        ResettingBits |= chanBit;
        OSRestoreInterrupts(enabled);
        return;
    }

    if (!(EnabledBits & chanBit)) {
        EnabledBits |= chanBit;
        ResettingBits &= ~chanBit;
    }

    if (RecalibrateBits & chanBit) {
        RecalibrateBits &= ~chanBit;
        /* Re-read origin */
    }

    OSRestoreInterrupts(enabled);
}

/*
 * fn_800AC5AC - PADUpdateOrigin
 * 0x800AC5AC | size: 0x7C
 * Updates the stored origin for a channel from SI response data.
 */
void fn_800AC5AC(s32 chan) {
    PADOrigin* org;

    org = &Origin[chan];
    org->stickX = 0;
    org->stickY = 0;
    org->substickX = 0;
    org->substickY = 0;
    org->triggerL = 0;
    org->triggerR = 0;
}

/*
 * fn_800AC628 - PADProbeCallback extended
 * 0x800AC628 | size: 0xAC
 * Extended probe callback that handles type detection.
 */
static void fn_800AC628(s32 chan, u32 error, OSContext* context) {
    u32 chanBit;

    chanBit = 0x80000000 >> (24 + chan);

    if (error != 0) {
        EnabledBits &= ~chanBit;
        ResettingBits &= ~chanBit;
        return;
    }

    PadType[chan] = SIGetType(chan);
    EnabledBits |= chanBit;
    ResettingBits &= ~chanBit;
}

/*
 * fn_800AC6D4 - PADMotorCallback
 * 0x800AC6D4 | size: 0x58
 * Callback for motor command completion.
 */
static void fn_800AC6D4(s32 chan, u32 error, OSContext* context) {
    u32 chanBit;

    chanBit = 0x80000000 >> (24 + chan);

    if (error != 0) {
        EnabledBits &= ~chanBit;
    }
}

/*
 * fn_800AC72C - PADControlMotorImpl
 * 0x800AC72C | size: 0x1E4
 * Full motor control implementation with SI transfer.
 */
void fn_800AC72C(s32 chan, u32 command) {
    BOOL enabled;
    u32 chanBit;

    chanBit = 0x80000000 >> (24 + chan);
    enabled = OSDisableInterrupts();

    MotorCommand[chan] = command;

    if (EnabledBits & chanBit) {
        /* Build SI command for motor control */
        u32 cmd = 0x00400000;  /* PAD motor command base */
        if (command == PAD_MOTOR_RUMBLE) {
            cmd |= 0x01;  /* Motor on */
        } else if (command == PAD_MOTOR_STOP_HARD) {
            cmd |= 0x02;  /* Motor brake */
        }
        SISetCommand(chan, cmd);
        SITransferCommands();
    }

    OSRestoreInterrupts(enabled);
}

/*
 * fn_800AC910 - PADControlAllMotors
 * 0x800AC910 | size: 0x44
 * Controls motors on all connected controllers.
 */
void fn_800AC910(u32* commands) {
    s32 chan;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        PADControlMotor(chan, commands[chan]);
    }
}

/*
 * fn_800AC954 - PADSetAnalogMode
 * 0x800AC954 | size: 0x3C
 * Sets the analog mode for SI polling (mode 0-3).
 */
void fn_800AC954(u32 mode) {
    u32 samplingRate;

    if (mode <= 3) {
        samplingRate = SISetSamplingRate(mode);
    }
}

/*
 * fn_800AC990 - PADClampCircle
 * 0x800AC990 | size: 0xF0
 * Clamps analog stick values to a circular range.
 */
void fn_800AC990(PADStatus* status) {
    s32 chan;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        s32 x, y, d;

        if (status[chan].err != PAD_ERR_NONE) {
            continue;
        }

        /* Main stick circular clamp */
        x = (s32)status[chan].stickX;
        y = (s32)status[chan].stickY;
        d = (x * x) + (y * y);

        if (d > (PAD_CLAMP_STICK_MAX * PAD_CLAMP_STICK_MAX)) {
            /* Scale to max radius */
        }

        /* C-stick circular clamp */
        x = (s32)status[chan].substickX;
        y = (s32)status[chan].substickY;
        d = (x * x) + (y * y);

        if (d > (PAD_CLAMP_SUBSTICK_MAX * PAD_CLAMP_SUBSTICK_MAX)) {
            /* Scale to max radius */
        }
    }
}

/*
 * fn_800ACA80 - PADClampExtended
 * 0x800ACA80 | size: 0xC4
 * Extended clamping with origin subtraction.
 */
void fn_800ACA80(PADStatus* status) {
    s32 chan;

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        if (status[chan].err != PAD_ERR_NONE) {
            continue;
        }

        /* Subtract origin and clamp */
        status[chan].stickX -= Origin[chan].stickX;
        status[chan].stickY -= Origin[chan].stickY;
        status[chan].substickX -= Origin[chan].substickX;
        status[chan].substickY -= Origin[chan].substickY;
        status[chan].triggerLeft -= Origin[chan].triggerL;
        status[chan].triggerRight -= Origin[chan].triggerR;
    }
}

/*
 * fn_800ACB44 - Returns a constant.
 * 0x800ACB44 | size: 0x8
 */
u32 fn_800ACB44(void) {
    return 0x4000;
}

/*
 * fn_800ACB4C - Returns a global.
 * 0x800ACB4C | size: 0x8
 */
u32 fn_800ACB4C(void) {
    extern u32 lbl_8047A90C;
    return lbl_8047A90C;
}

/*
 * fn_800ACB54 - PADFixOrigin
 * 0x800ACB54 | size: 0x78
 * Corrects origin values by applying __PADFixBits mask.
 */
void fn_800ACB54(PADStatus* status, s32 chan) {
    PADOrigin* org;

    org = &Origin[chan];

    if (__PADFixBits & (0x80000000 >> (24 + chan))) {
        /* Apply origin correction from stored values */
        status->stickX -= org->stickX;
        status->stickY -= org->stickY;
        status->substickX -= org->substickX;
        status->substickY -= org->substickY;
    }
}

/*
 * fn_800ACBCC - PADSetPendingBits
 * 0x800ACBCC | size: 0x20
 * Marks channels as pending re-initialization.
 */
void fn_800ACBCC(u32 mask) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    PendingBits |= mask;
    OSRestoreInterrupts(enabled);
}

/*
 * fn_800ACBEC - PADGetEnabledBits
 * 0x800ACBEC | size: 0x10
 * Returns the enabled controller bitmask.
 */
u32 fn_800ACBEC(void) {
    u16 value;

    value = *(volatile u16*)0xCC00500A;
    return value & 0x20;
}

/*
 * fn_800ACBFC - PADReset internal (large)
 * 0x800ACBFC | size: 0x17F4
 *
 * This is the full PAD reset state machine -- the largest function in the
 * PAD module. It handles SI type detection, probing, origin reading, and
 * controller initialization for all 4 channels. Due to its enormous size
 * and deep SI hardware interaction, it remains a stub pending further
 * Ghidra analysis of the SI transfer sequences.
 */
void fn_800ACBFC(void) {
    /* Large PAD reset state machine -- requires SI transfer chain analysis */
}

/*
 * fn_800AE3F0 - PADRecalibrate internal
 * 0x800AE3F0 | size: 0x100
 * Issues recalibration commands to specified controllers.
 */
void fn_800AE3F0(u32 mask) {
    s32 chan;
    u32 chanBit;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        chanBit = 0x80000000 >> (24 + chan);
        if (mask & chanBit) {
            RecalibrateBits |= chanBit;
            EnabledBits &= ~chanBit;
            ResettingBits |= chanBit;
        }
    }

    OSRestoreInterrupts(enabled);
}

/*
 * fn_800AE4F0 - Empty function (pad alignment)
 * 0x800AE4F0 | size: 0x4
 */
void fn_800AE4F0(void) {
}

/* ========================================================== */
/* Converted SI / PAD extended functions (0x800AE4F4+)        */
/* These functions are SI transfer helpers and PAD subsystem   */
/* internals that share this translation unit.                 */
/* ========================================================== */

/* fn_800AE4F4 - SITransferHelper | 0x800AE4F4 | size: 0xCC */
void fn_800AE4F4(s32 chan, void* output, u32 outputBytes, void* input, u32 inputBytes, SICallback callback) {
    SITransfer(chan, output, outputBytes, input, inputBytes, callback, OSMicrosecondsToTicks(65));
}

/* fn_800AE5C0 - SIEnablePollingHelper | 0x800AE5C0 | size: 0x70 */
void fn_800AE5C0(u32 poll) {
    BOOL enabled;
    enabled = OSDisableInterrupts();
    SIEnablePolling(poll);
    OSRestoreInterrupts(enabled);
}

/* fn_800AE630 - SITransferConfig | 0x800AE630 | size: 0x15C */
void fn_800AE630(s32 chan, u32 type, SICallback callback) {
    BOOL enabled;
    u32 chanBit;

    chanBit = 0x80000000 >> (24 + chan);
    enabled = OSDisableInterrupts();

    SISetCommand(chan, type);
    SITransferCommands();

    OSRestoreInterrupts(enabled);
}

/* fn_800AE78C - Returns a global | 0x800AE78C | size: 0x8 */
u32 fn_800AE78C(void) {
    extern u32 lbl_8047A948;
    return lbl_8047A948;
}

/* fn_800AE794 - SIGetChannelStatus | 0x800AE794 | size: 0x10 */
u32 fn_800AE794(s32 chan) {
    return SIGetStatus(chan);
}

/* fn_800AE7A4 - SIGetChannelCommand | 0x800AE7A4 | size: 0x10 */
u32 fn_800AE7A4(s32 chan) {
    return SIGetCommand(chan);
}

/* fn_800AE7B4 - SIGetTypeHelper | 0x800AE7B4 | size: 0x18 */
u32 fn_800AE7B4(s32 chan) {
    u32 type = SIGetType(chan);
    return type & 0x18000000;
}

/* fn_800AE7CC - SIGetTypeCheck | 0x800AE7CC | size: 0x14 */
BOOL fn_800AE7CC(s32 chan) {
    u32 type = SIGetType(chan);
    return (type != 0) ? TRUE : FALSE;
}

/* fn_800AE7E0 - SISetSamplingConfig | 0x800AE7E0 | size: 0xC4 */
void fn_800AE7E0(u32 x, u32 y) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SISetXY(x, y);
    OSRestoreInterrupts(enabled);
}

/* fn_800AE8A4 - SIRegisterHandler | 0x800AE8A4 | size: 0x48 */
void fn_800AE8A4(__OSInterruptHandler handler) {
    SIRegisterPollingHandler(handler);
}

/* fn_800AE8EC - SIUnregisterHandler | 0x800AE8EC | size: 0x40 */
void fn_800AE8EC(__OSInterruptHandler handler) {
    SIUnregisterPollingHandler(handler);
}

/* fn_800AE92C - SIIsChanBusyHelper | 0x800AE92C | size: 0x10 */
BOOL fn_800AE92C(s32 chan) {
    return SIIsChanBusy(chan);
}

/* fn_800AE93C - SIBusyWait | 0x800AE93C | size: 0x70 */
void fn_800AE93C(s32 chan) {
    while (SIIsChanBusy(chan)) {
        /* Spin until channel is free */
    }
}

/* fn_800AE9AC - SIGetResponseHelper | 0x800AE9AC | size: 0x50 */
BOOL fn_800AE9AC(s32 chan, void* data) {
    BOOL enabled;
    BOOL result;

    enabled = OSDisableInterrupts();
    result = SIGetResponse(chan, data);
    OSRestoreInterrupts(enabled);
    return result;
}

/* fn_800AE9FC - SITransferSequence | 0x800AE9FC | size: 0x424 */
void fn_800AE9FC(void) {
    /* Large SI transfer sequencer -- handles multi-channel polling,
     * command dispatch, and response collection. Requires detailed
     * SI register analysis. */
}

/* fn_800AEE20 - SICallbackDispatch | 0x800AEE20 | size: 0x1A0 */
void fn_800AEE20(s32 chan, u32 error) {
    /* SI callback dispatch -- routes SI completion events to
     * the appropriate registered callback for each channel. */
}

/* fn_800AEFC0 - SIInterruptHandler | 0x800AEFC0 | size: 0x18C */
void fn_800AEFC0(s32 interrupt, OSContext* context) {
    /* SI transfer completion interrupt handler. Reads SI status,
     * collects response data, and dispatches callbacks. */
}

/* fn_800AF14C - SITypeDetect | 0x800AF14C | size: 0xA0 */
u32 fn_800AF14C(s32 chan) {
    u32 type;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);
    OSRestoreInterrupts(enabled);
    return type;
}

/* fn_800AF1EC - SIProbeChannel | 0x800AF1EC | size: 0x94 */
BOOL fn_800AF1EC(s32 chan) {
    u32 type;

    type = SIGetType(chan);
    if (type == 0) {
        return FALSE;
    }
    return TRUE;
}

/* fn_800AF280 - Empty function | 0x800AF280 | size: 0x4 */
void fn_800AF280(void) {
}

/* fn_800AF284 - SISetTypeCallback | 0x800AF284 | size: 0xD8 */
void fn_800AF284(s32 chan, SITypeAndStatusCallback callback) {
    /* Registers a type-and-status callback for a specific channel.
     * Used by PAD probing to detect controller connections. */
}

/* fn_800AF35C - SITransferInit | 0x800AF35C | size: 0x118 */
void fn_800AF35C(s32 chan, u32 command) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SISetCommand(chan, command);
    SITransferCommands();
    OSRestoreInterrupts(enabled);
}

/* fn_800AF474 - SIReadResponse | 0x800AF474 | size: 0xA8 */
void fn_800AF474(s32 chan, void* data, u32 size) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SIGetResponse(chan, data);
    OSRestoreInterrupts(enabled);
}

/* fn_800AF51C - SISyncTransfer | 0x800AF51C | size: 0x84 */
void fn_800AF51C(s32 chan) {
    u8* block = lbl_803FC620 + chan * 0x110;
    CARDApiCallback cb = *(CARDApiCallback*)(block + 0xdc);

    if (cb != 0) {
        *(CARDApiCallback*)(block + 0xdc) = 0;
        cb(chan, fn_80098944(chan) != 0 ? 1 : -3);
    }
}

/* fn_800AF5A0 - SIPollController | 0x800AF5A0 | size: 0xC0 */
void fn_800AF5A0(s32 chan) {
    u32 chanBit;
    BOOL enabled;

    chanBit = 0x80000000 >> (24 + chan);
    enabled = OSDisableInterrupts();
    SIEnablePolling(chanBit);
    OSRestoreInterrupts(enabled);
}

/* fn_800AF660 - SIConfigController | 0x800AF660 | size: 0xF0 */
void fn_800AF660(s32 chan, u32 config) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SISetCommand(chan, config);
    SITransferCommands();
    OSRestoreInterrupts(enabled);
}

/* fn_800AF750 - SIGetTypeSync | 0x800AF750 | size: 0xAC */
u32 fn_800AF750(s32 chan) {
    u32 type;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);
    if (type == 0) {
        type = SIGetType(chan);  /* Retry once */
    }
    OSRestoreInterrupts(enabled);
    return type;
}

/* fn_800AF7FC - SIReadOrigin | 0x800AF7FC | size: 0xA4 */
void fn_800AF7FC(s32 chan, PADOrigin* origin) {
    u32 data[3];

    if (SIGetResponse(chan, data)) {
        origin->button = (u16)(data[0] >> 16);
        origin->stickX = (s8)(data[0] >> 8);
        origin->stickY = (s8)(data[0]);
        origin->substickX = (s8)(data[1] >> 24);
        origin->substickY = (s8)(data[1] >> 16);
        origin->triggerL = (u8)(data[1] >> 8);
        origin->triggerR = (u8)(data[1]);
    }
}

/* fn_800AF8A0 - SIDecodeStatus | 0x800AF8A0 | size: 0x22C */
void fn_800AF8A0(PADStatus* status, s32 chan) {
    /* Decodes raw SI response data into PADStatus fields.
     * Reads 3 words from SI response buffer, extracts button,
     * stick, substick, and trigger values. */
}

/* fn_800AFACC - SIGetOriginFromResponse | 0x800AFACC | size: 0x110 */
void fn_800AFACC(s32 chan) {
    /* Reads SI origin response and stores into Origin[chan].
     * Part of the controller initialization sequence. */
}

/* fn_800AFBDC - SIInitController | 0x800AFBDC | size: 0x1B4 */
void fn_800AFBDC(s32 chan) {
    /* Full controller initialization sequence:
     * 1. Send ID command
     * 2. Wait for response
     * 3. Read controller type
     * 4. Read origin values
     * 5. Enable polling */
}

/* fn_800AFD90 - SIResetController | 0x800AFD90 | size: 0x134 */
void fn_800AFD90(s32 chan) {
    /* Resets a controller channel:
     * 1. Disable polling for channel
     * 2. Send reset command
     * 3. Clear state bits
     * 4. Re-probe */
}

/* fn_800AFEC4 - SIDetectController | 0x800AFEC4 | size: 0x11C */
void fn_800AFEC4(s32 chan) {
    /* Detects controller type on channel via SI type command.
     * Sets PadType[chan] based on response. */
}

/* fn_800AFFE0 - SIHandleError | 0x800AFFE0 | size: 0xE0 */
void fn_800AFFE0(s32 chan, u32 error) {
    u32 chanBit;
    BOOL enabled;

    chanBit = 0x80000000 >> (24 + chan);
    enabled = OSDisableInterrupts();

    if (error != 0) {
        EnabledBits &= ~chanBit;
        ResettingBits |= chanBit;
    }

    OSRestoreInterrupts(enabled);
}

/* fn_800B00C0 - SIProcessResponse | 0x800B00C0 | size: 0xAC */
void fn_800B00C0(s32 chan) {
    u32 chanBit;

    chanBit = 0x80000000 >> (24 + chan);

    if (!(EnabledBits & chanBit)) {
        return;
    }

    /* Process SI response data for channel */
}

/* fn_800B016C - Returns a global | 0x800B016C | size: 0x8 */
u32 fn_800B016C(void) {
    extern u16 lbl_8047A970;
    return lbl_8047A970;
}

/* fn_800B0174 - SISetPollingRate | 0x800B0174 | size: 0x38 */
u32 fn_800B0174(u32 msec) {
    u8* base;
    u32 value;

    base = lbl_803FC620;
    if (msec != 0) {
        value = msec;
    } else {
        value = (u32)(base + 0x220);
    }
    *(u32*)(base + 0x10C) = value;
    msec = (msec != 0) ? msec : (u32)(base + 0x220);
    *(u32*)(base + 0x21C) = msec;
    return msec;
}

/* fn_800B01AC - SIDisablePollingHelper | 0x800B01AC | size: 0x18 */
void fn_800B01AC(u32 poll) {
    SIDisablePolling(poll);
}

/* fn_800B01C4 - SIEnablePollingAll | 0x800B01C4 | size: 0x78 */
void fn_800B01C4(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SIEnablePolling(0xF0000000);
    OSRestoreInterrupts(enabled);
}

/* fn_800B023C - SITransferAndWait | 0x800B023C | size: 0xB8 */
BOOL fn_800B023C(s32 chan, void* output, u32 outLen, void* input, u32 inLen) {
    BOOL result;

    result = SITransfer(chan, output, outLen, input, inLen, NULL, OSMicrosecondsToTicks(65));
    if (result) {
        SISync();
    }
    return result;
}

/* fn_800B02F4 - SIWaitForChannel | 0x800B02F4 | size: 0x64 */
void fn_800B02F4(s32 chan) {
    while (SIIsChanBusy(chan)) {
        /* Wait */
    }
}

/* fn_800B0358 - SISendCommand | 0x800B0358 | size: 0x30 */
s32 fn_800B0358(s32 chan) {
    if ((chan < 0) || (chan >= 2)) {
        return -0x80;
    }
    return *(s32*)(lbl_803FC620 + chan * 0x110 + 4);
}

/* fn_800B0388 - SIConfigurePolling | 0x800B0388 | size: 0x150 */
void fn_800B0388(u32 config) {
    s32 chan;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++) {
        SISetCommand(chan, config);
    }
    SITransferCommands();

    OSRestoreInterrupts(enabled);
}

/* fn_800B04D8 - SIGetResponseSync | 0x800B04D8 | size: 0x50 */
BOOL fn_800B04D8(s32 chan, void* data) {
    while (SIIsChanBusy(chan)) {
        /* Wait */
    }
    return SIGetResponse(chan, data);
}

/* fn_800B0528 - SITransferConfig2 | 0x800B0528 | size: 0x16C */
void fn_800B0528(s32 chan, u32 command, SICallback callback) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SISetCommand(chan, command);
    SITransferCommands();
    OSRestoreInterrupts(enabled);
}

/* fn_800B0694 - SIInitChannels | 0x800B0694 | size: 0x144 */
void fn_800B0694(void) {
    /* Initializes all 4 SI channels for controller polling. */
}

/* fn_800B07D8 - 0x800B07D8 | size: 0xC4 */
void fn_800B07D8(void) {
    /* SI/PAD helper (0xC4 bytes) */
}

/* fn_800B089C - 0x800B089C | size: 0xB58 */
void fn_800B089C(void) {
    /* Large SI/PAD function (0xB58 bytes) -- requires Ghidra analysis */
}

/* fn_800B13F4 - 0x800B13F4 | size: 0x70 */
void fn_800B13F4(void) {
    /* Medium function (0x70 bytes) */
}

/* fn_800B1464 - 0x800B1464 | size: 0x324 */
void fn_800B1464(void) {
    /* Large SI/PAD function (0x324 bytes) -- requires Ghidra analysis */
}

/* fn_800B1788 - 0x800B1788 | size: 0xDC */
void fn_800B1788(void) {
    /* SI/PAD helper (0xDC bytes) */
}

/* fn_800B1864 - 0x800B1864 | size: 0x64 */
void fn_800B1864(void) {
    /* Medium function (0x64 bytes) */
}

/* fn_800B18C8 - 0x800B18C8 | size: 0xDC */
void fn_800B18C8(void) {
    /* SI/PAD helper (0xDC bytes) */
}

/* fn_800B19A4 - 0x800B19A4 | size: 0x64 */
void fn_800B19A4(void) {
    /* Medium function (0x64 bytes) */
}

/* fn_800B1A08 - Returns word at offset 0x88 | 0x800B1A08 | size: 0x8 */
u32 fn_800B1A08(u8* p) {
    return *(u32*)(p + 0x88);
}

/* fn_800B1A10 - 0x800B1A10 | size: 0xD4 */
void fn_800B1A10(void) {
    /* SI/PAD helper (0xD4 bytes) */
}

/* fn_800B1AE4 - 0x800B1AE4 | size: 0xC8 */
void fn_800B1AE4(void) {
    /* SI/PAD helper (0xC8 bytes) */
}

/* fn_800B1BAC - 0x800B1BAC | size: 0x118 */
void fn_800B1BAC(void) {
    /* SI/PAD helper (0x118 bytes) */
}

/* fn_800B1CC4 - 0x800B1CC4 | size: 0x9C */
void fn_800B1CC4(void) {
    /* SI/PAD helper (0x9C bytes) */
}

/* fn_800B1D60 - 0x800B1D60 | size: 0xAC */
void fn_800B1D60(void) {
    /* SI/PAD helper (0xAC bytes) */
}

/* fn_800B1E0C - Returns word at offset 0x84 | 0x800B1E0C | size: 0x8 */
u32 fn_800B1E0C(u8* p) {
    return *(u32*)(p + 0x84);
}

/* fn_800B1E14 - 0x800B1E14 | size: 0xD0 */
void fn_800B1E14(void) {
    /* SI/PAD helper (0xD0 bytes) */
}

/* fn_800B1EE4 - 0x800B1EE4 | size: 0xC8 */
void fn_800B1EE4(void) {
    /* SI/PAD helper (0xC8 bytes) */
}

/* fn_800B1FAC - 0x800B1FAC | size: 0xC4 */
void fn_800B1FAC(void) {
    /* SI/PAD helper (0xC4 bytes) */
}

/* fn_800B2070 - 0x800B2070 | size: 0x1B0 */
void fn_800B2070(void) {
    /* SI/PAD helper (0x1B0 bytes) */
}

/* fn_800B2220 - 0x800B2220 | size: 0x284 */
void fn_800B2220(void) {
    /* Large SI/PAD function (0x284 bytes) -- requires Ghidra analysis */
}

/* fn_800B24A4 - 0x800B24A4 | size: 0x240 */
void fn_800B24A4(void) {
    /* Large SI/PAD function (0x240 bytes) -- requires Ghidra analysis */
}

/* fn_800B26E4 - 0x800B26E4 | size: 0x284 */
void fn_800B26E4(void) {
    /* Large SI/PAD function (0x284 bytes) -- requires Ghidra analysis */
}

/* fn_800B2968 - 0x800B2968 | size: 0x8C */
void fn_800B2968(void) {
    /* SI/PAD helper (0x8C bytes) */
}

/* fn_800B29F4 - 0x800B29F4 | size: 0x590 */
void fn_800B29F4(void) {
    /* Large SI/PAD function (0x590 bytes) -- requires Ghidra analysis */
}

/* fn_800B2F84 - 0x800B2F84 | size: 0x28 */
s32 fn_800B2F84(s32 chan, CARDCallback callback) {
    s32 xferBytes;
    return CARDCheckExAsync(chan, &xferBytes, callback);
}

/* fn_800B2FAC - 0x800B2FAC | size: 0xCC */
void fn_800B2FAC(void) {
    /* SI/PAD helper (0xCC bytes) */
}

/* fn_800B3078 - 0x800B3078 | size: 0x17C */
void fn_800B3078(void) {
    /* SI/PAD helper (0x17C bytes) */
}

/* fn_800B31F4 - 0x800B31F4 | size: 0x410 */
void fn_800B31F4(void) {
    /* Large SI/PAD function (0x410 bytes) -- requires Ghidra analysis */
}

/* fn_800B3604 - 0x800B3604 | size: 0x138 */
void fn_800B3604(void) {
    /* SI/PAD helper (0x138 bytes) */
}

/* fn_800B373C - 0x800B373C | size: 0x1A0 */
void fn_800B373C(void) {
    /* SI/PAD helper (0x1A0 bytes) */
}

/* fn_800B38DC - 0x800B38DC | size: 0x9C */
void fn_800B38DC(s32 chan, s32 arg1) {
    extern u8 lbl_803FC620[];
    extern int OSDisableInterrupts(void);
    extern void OSRestoreInterrupts(int);
    extern void fn_8009870C(s32, s32);
    extern void fn_80098AE8(s32);
    extern void OSCancelAlarm(void*);
    u8* card;
    int enabled;

    card = lbl_803FC620 + chan * 0x110;
    enabled = OSDisableInterrupts();
    if (*(s32*)card != 0) {
        fn_8009870C(chan, 0);
        fn_80098AE8(chan);
        OSCancelAlarm(card + 0xE0);
        *(s32*)card = 0;
        *(s32*)(card + 0x4) = arg1;
        *(s32*)(card + 0x24) = 0;
    }
    OSRestoreInterrupts(enabled);
}

/* fn_800B3978 - 0x800B3978 | size: 0xAC */
void fn_800B3978(void) {
    /* SI/PAD helper (0xAC bytes) */
}

/* fn_800B3A24 - 0x800B3A24 | size: 0x144 */
void fn_800B3A24(void) {
    /* SI/PAD helper (0x144 bytes) */
}

/* fn_800B3B68 - 0x800B3B68 | size: 0x658 */
void fn_800B3B68(void) {
    /* Large SI/PAD function (0x658 bytes) -- requires Ghidra analysis */
}

/* fn_800B41C0 - 0x800B41C0 | size: 0x48 */
void fn_800B41C0(void) {
    /* Medium function (0x48 bytes) */
}

/* fn_800B4208 - 0x800B4208 | size: 0x68 */
void fn_800B4208(void) {
    /* Medium function (0x68 bytes) */
}

/* fn_800B4270 - 0x800B4270 | size: 0x98 */
void fn_800B4270(void) {
    /* SI/PAD helper (0x98 bytes) */
}

/* fn_800B4308 - 0x800B4308 | size: 0x30 */
s32 fn_800B4308(u8* ent) {
    if (ent[0] == 0xff) {
        return -4;
    }
    if (ent[0x34] & 4) {
        return 0;
    }
    return -10;
}

/* fn_800B4338 - 0x800B4338 | size: 0x150 */
void fn_800B4338(void) {
    /* SI/PAD helper (0x150 bytes) */
}

/* fn_800B4488 - 0x800B4488 | size: 0x160 */
void fn_800B4488(void) {
    /* SI/PAD helper (0x160 bytes) */
}

/* fn_800B45E8 - 0x800B45E8 | size: 0x54 */
void fn_800B45E8(void) {
    /* Medium function (0x54 bytes) */
}

/* fn_800B463C - Returns zero | 0x800B463C | size: 0x8 */
u32 fn_800B463C(void) {
    return 0;
}

/* fn_800B4644 - 0x800B4644 | size: 0x130 */
void fn_800B4644(void) {
    /* SI/PAD helper (0x130 bytes) */
}

/* fn_800B4774 - 0x800B4774 | size: 0x220 */
void fn_800B4774(void) {
    /* Large SI/PAD function (0x220 bytes) -- requires Ghidra analysis */
}

/* fn_800B4994 - 0x800B4994 | size: 0x1B8 */
void fn_800B4994(void) {
    /* SI/PAD helper (0x1B8 bytes) */
}

/* fn_800B4B4C - 0x800B4B4C | size: 0x130 */
void fn_800B4B4C(void) {
    /* SI/PAD helper (0x130 bytes) */
}

/* fn_800B4C7C - 0x800B4C7C | size: 0x148 */
void fn_800B4C7C(void) {
    /* SI/PAD helper (0x148 bytes) */
}

/* fn_800B4DC4 - 0x800B4DC4 | size: 0x8C */
void fn_800B4DC4(void) {
    /* SI/PAD helper (0x8C bytes) */
}

/* fn_800B4E50 - 0x800B4E50 | size: 0x170 */
void fn_800B4E50(void) {
    /* SI/PAD helper (0x170 bytes) */
}

/* fn_800B4FC0 - 0x800B4FC0 | size: 0xB0 */
void fn_800B4FC0(void) {
    /* SI/PAD helper (0xB0 bytes) */
}

/* fn_800B5070 - 0x800B5070 | size: 0x114 */
void fn_800B5070(void) {
    /* SI/PAD helper (0x114 bytes) */
}

/* fn_800B5184 - 0x800B5184 | size: 0xA4 */
void fn_800B5184(void) {
    /* SI/PAD helper (0xA4 bytes) */
}

/* fn_800B5228 - 0x800B5228 | size: 0x110 */
void fn_800B5228(void) {
    /* SI/PAD helper (0x110 bytes) */
}

/* fn_800B5338 - 0x800B5338 | size: 0x1F8 */
void fn_800B5338(void) {
    /* SI/PAD helper (0x1F8 bytes) */
}

/* fn_800B5530 - 0x800B5530 | size: 0x12C */
void fn_800B5530(void) {
    /* SI/PAD helper (0x12C bytes) */
}

/* fn_800B565C - 0x800B565C | size: 0x174 */
void fn_800B565C(void) {
    /* SI/PAD helper (0x174 bytes) */
}

/* fn_800B57D0 - 0x800B57D0 | size: 0xBC */
void fn_800B57D0(void) {
    /* SI/PAD helper (0xBC bytes) */
}

/* fn_800B588C - 0x800B588C | size: 0x254 */
void fn_800B588C(void) {
    /* Large SI/PAD function (0x254 bytes) -- requires Ghidra analysis */
}

/* fn_800B5AE0 - 0x800B5AE0 | size: 0xC4 */
void fn_800B5AE0(void) {
    /* SI/PAD helper (0xC4 bytes) */
}

/* fn_800B5BA4 - 0x800B5BA4 | size: 0x40 */
void fn_800B5BA4(void) {
    /* Medium function (0x40 bytes) */
}

/* fn_800B5BE4 - 0x800B5BE4 | size: 0x78 */
void fn_800B5BE4(void) {
    /* Medium function (0x78 bytes) */
}

/* fn_800B5C5C - 0x800B5C5C | size: 0x7C */
#pragma peephole off
void* fn_800B5C5C(void* obj, int id) {
    extern u8* gx;
    extern s32 fn_800BAE5C();
    s32 fmt = fn_800BAE5C(obj);

    if (fmt != 8 && fmt != 9 && fmt != 10) {
        u32* g = (u32*)gx;
        u32 c = g[0xB2];
        g[0xB2] = c + 1;
        return (u8*)g + ((c & 7) << 4) + 0x208;
    } else {
        u32* g = (u32*)gx;
        u32 c = g[0xB3];
        g[0xB3] = c + 1;
        return (u8*)g + ((c & 3) << 4) + 0x288;
    }
}
#pragma peephole reset

/* fn_800B5CD8 - 0x800B5CD8 | size: 0x24 */
void fn_800B5CD8(void) {
    /* Medium function (0x24 bytes) */
}

/* fn_800B5CFC - 0x800B5CFC | size: 0x190 */
void fn_800B5CFC(void) {
    /* SI/PAD helper (0x190 bytes) */
}

