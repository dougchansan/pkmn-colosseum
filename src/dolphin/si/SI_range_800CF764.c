/**
 * @file SI_range_800CF764.c
 * @brief Dolphin SI (one TU proven by static GetTypeCallback linkage), 0x800CF764 - 0x800D0DF8.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * Only a subset of the unit's functions are matched here (see batch 059);
 * the remainder (CompleteTransfer, SIInterruptHandler_800CFA60, __SITransfer,
 * SITransfer, SIGetType, SIGetTypeAsync) stay asm-only and
 * are referenced only through function-local extern declarations.
 */
#include "dolphin/types.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSAlarm.h"

#pragma push
#pragma optimization_level 0
#pragma peephole off
#pragma scheduling off
typedef struct {
    u32 reg;
    u32 unk4;
    u32 unk8;
} SICommandQueueEntry;
#pragma scheduling reset
#pragma peephole reset
#pragma pop

#define __SIRegs ((volatile u32*)0xCC006400)
#define SI_COMCSR_IDX (0x34 / 4)
#define SI_STATUS_IDX (0x38 / 4)
#define SI_POLL_IDX (0x30 / 4)

#define SI_COMCSR_TCINT_MASK 0x80000000u
#define SI_COMCSR_RDSTINTMSK_MASK 0x08000000u
#define SI_COMCSR_TSTART_MASK 0x00000001u

typedef struct SIPacket {
    s32 chan;
    void* output;
    u32 outputBytes;
    void* input;
    u32 inputBytes;
    SICallback callback;
    s64 fire;
} SIPacket;

typedef SIPacket SIGlobalData[4];

static SIGlobalData Packet_803FFFB0;
static OSAlarm lbl_80400030[4];
static s64 TypeTime[4];
static s64 XferTime[4];
static SITypeAndStatusCallback TypeCallback[4][4];
/* Keep the retail slot while partial polling functions retain external RDSTHandler. */
static u8 _rdstHandlerStorage[sizeof(__OSInterruptHandler) * 4];
static BOOL InputBufferValid[4];
static u32 InputBuffer[4][2];
static volatile u32 InputBufferVcount[4];

typedef struct SIControl {
    s32 chan;
    u32 poll;
    u32 inputBytes;
    void* input;
    SICallback callback;
} SIControl;

extern SIControl Si_80313F8C;
extern __OSInterruptHandler RDSTHandler[4];
extern u32 Type_80313FA0[4];

extern const char* __SIVersion;
extern u32 lbl_8047AA58;

void fn_800D0338(s32 chan, u32 command) {
    ((volatile SICommandQueueEntry*)0xCC006400)[chan].reg = command;
}

void fn_800D034C(void) {
    volatile u32* commandRegister;
    u32 command;

    command = 0x80000000u;
    *(commandRegister = (volatile u32*)0xCC006438) = command;
}

BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    volatile u32* registerBlock;
    volatile u32* csr;
    u32 reg;
    BOOL wasEnabled;
    int i;

    enabled = OSDisableInterrupts();
    csr = (registerBlock = __SIRegs) + SI_COMCSR_IDX;
    reg = *csr;
    wasEnabled = (reg & SI_COMCSR_RDSTINTMSK_MASK) ? TRUE : FALSE;

    if (enable) {
        reg |= SI_COMCSR_RDSTINTMSK_MASK;
        for (i = 0; i < 4; i++) {
            InputBufferVcount[i] = 0;
        }
    } else {
        reg &= ~SI_COMCSR_RDSTINTMSK_MASK;
    }

    reg &= ~(SI_COMCSR_TCINT_MASK | SI_COMCSR_TSTART_MASK);
    *csr = reg;
    OSRestoreInterrupts(enabled);
    return wasEnabled;
}

BOOL SIRegisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < 4; i++) {
        if (RDSTHandler[i] == handler) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    for (i = 0; i < 4; i++) {
        if (RDSTHandler[i] == 0) {
            RDSTHandler[i] = handler;
            SIEnablePollingInterrupt(TRUE);
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL SIUnregisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    int i;
    int count;

    enabled = OSDisableInterrupts();

    for (i = 0; i < 4; i++) {
        if (RDSTHandler[i] == handler) {
            RDSTHandler[i] = 0;

            count = 0;
            if (RDSTHandler[0] == 0) {
                count = 1;
                if (RDSTHandler[1] == 0) {
                    count = 2;
                    if (RDSTHandler[2] == 0) {
                        count = 3;
                        if (RDSTHandler[3] == 0) {
                            count = 4;
                        }
                    }
                }
            }
            if (count == 4) {
                SIEnablePollingInterrupt(FALSE);
            }

            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SIInit(void) {
    extern void SIInterruptHandler_800CFA60(__OSInterrupt interrupt, OSContext* context);

    OSRegisterVersion(__SIVersion);

    Packet_803FFFB0[3].chan = -1;
    Packet_803FFFB0[2].chan = -1;
    Packet_803FFFB0[1].chan = -1;
    Packet_803FFFB0[0].chan = -1;
    Si_80313F8C.poll = 0;
    SISetSamplingRate(0);

    {
        volatile u32* csr = &__SIRegs[SI_COMCSR_IDX];
        while (*csr & SI_COMCSR_TSTART_MASK) {
        }
        *csr = SI_COMCSR_TCINT_MASK;
    }

    __OSSetInterruptHandler(0x14, SIInterruptHandler_800CFA60);
    __OSUnmaskInterrupts(0x800);

    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);
}

#pragma dont_inline on
u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sr;

    enabled = OSDisableInterrupts();
    sr = __SIRegs[SI_STATUS_IDX];
    sr >>= (3 - chan) * 8;

    if (sr & 8) {
        if ((Type_80313FA0[chan] & 0x80) == 0) {
            Type_80313FA0[chan] = 8;
        }
    }

    OSRestoreInterrupts(enabled);
    return sr;
}
#pragma dont_inline reset

u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = (x << 16) | (y << 8);
    enabled = OSDisableInterrupts();
    Si_80313F8C.poll &= 0xFC0000FF;
    Si_80313F8C.poll |= poll;
    poll = Si_80313F8C.poll;
    __SIRegs[SI_POLL_IDX] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

#pragma push
#pragma peephole off
u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 top;
    u32 en;

    if (poll == 0) {
        return Si_80313F8C.poll;
    }

    enabled = OSDisableInterrupts();
    top = poll >> 24;
    en = top >> 4;
    Si_80313F8C.poll &= ~en;
    Si_80313F8C.poll |= top & (0xF0 | en);
    Si_80313F8C.poll |= poll;

    __SIRegs[SI_STATUS_IDX] = 0x80000000u;
    __SIRegs[SI_POLL_IDX] = Si_80313F8C.poll;
    OSRestoreInterrupts(enabled);
    return Si_80313F8C.poll;
}
#pragma pop

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;
    u32 newPoll;

    if (poll == 0) {
        return Si_80313F8C.poll;
    }

    enabled = OSDisableInterrupts();
    poll = (poll >> 24) & 0xF0;
    newPoll = Si_80313F8C.poll & ~poll;
    __SIRegs[SI_POLL_IDX] = newPoll;
    Si_80313F8C.poll = newPoll;
    OSRestoreInterrupts(enabled);
    return newPoll;
}

BOOL SIGetResponseRaw(s32 chan) {
    BOOL enabled;
    u32 sr;

    enabled = OSDisableInterrupts();
    sr = __SIRegs[SI_STATUS_IDX];
    sr >>= (3 - chan) * 8;
    if (sr & 8) {
        if ((Type_80313FA0[chan] & 0x80) == 0) {
            Type_80313FA0[chan] = 8;
        }
    }
    OSRestoreInterrupts(enabled);

    if (sr & 0x20) {
        InputBuffer[chan][0] = ((volatile SICommandQueueEntry*)0xCC006400)[chan].unk4;
        InputBuffer[chan][1] = ((volatile SICommandQueueEntry*)0xCC006400)[chan].unk8;
        InputBufferValid[chan] = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL SIGetResponse(s32 chan, void* data) {
    BOOL enabled;
    BOOL valid;

    enabled = OSDisableInterrupts();

    if (SIGetStatus(chan) & 0x20) {
        InputBuffer[chan][0] = ((volatile SICommandQueueEntry*)0xCC006400)[chan].unk4;
        InputBuffer[chan][1] = ((volatile SICommandQueueEntry*)0xCC006400)[chan].unk8;
        InputBufferValid[chan] = TRUE;
    }

    valid = InputBufferValid[chan];
    InputBufferValid[chan] = FALSE;
    if (valid) {
        ((u32*)data)[0] = InputBuffer[chan][0];
        ((u32*)data)[1] = InputBuffer[chan][1];
    }

    OSRestoreInterrupts(enabled);
    return valid;
}

void AlarmHandler_800D0668(OSAlarm* alarm, OSContext* context) {
    extern int __SITransfer(s32 chan, void* output, u32 outputBytes, void* input,
                             u32 inputBytes, SICallback callback);
    s32 chan;
    SIPacket* packet;

    chan = alarm - lbl_80400030;
    packet = &Packet_803FFFB0[chan];
    if (packet->chan != -1) {
        if (__SITransfer(packet->chan, packet->output, packet->outputBytes,
                          packet->input, packet->inputBytes, packet->callback)) {
            packet->chan = -1;
        }
    }
}

static void CallTypeAndStatusCallback(s32 chan, u32 type) {
    SITypeAndStatusCallback callback;
    int i;

    for (i = 0; i < 4; i++) {
        callback = TypeCallback[chan][i];
        if (callback != 0) {
            TypeCallback[chan][i] = 0;
            (*callback)(chan, type);
        }
    }
}

static void GetTypeCallback(s32 chan, u32 error, OSContext* context) {
    u32 type;
    u32 chanBit;
    int fix;
    u32 id;

    Type_80313FA0[chan] &= ~0x80;
    Type_80313FA0[chan] |= error;
    TypeTime[chan] = __OSGetSystemTime();

    type = Type_80313FA0[chan];
    chanBit = 0x80000000 >> chan;
    fix = lbl_8047AA58 & chanBit;
    lbl_8047AA58 &= ~chanBit;

    if ((error & 0xF) != 0 || (type & 0x18000000) != 0x08000000 ||
        (type & 0x80000000) == 0 || (type & 0x04000000) != 0) {
        OSSetWirelessID(chan, 0);
        CallTypeAndStatusCallback(chan, Type_80313FA0[chan]);
    } else {
        static u32 cmdFixDevice[4];

        id = OSGetWirelessID(chan) << 8;

        if (fix != 0 && (id & 0x100000) != 0) {
            cmdFixDevice[chan] = 0x4E000000 | (id & 0xCFFF00) | 0x100000;
            Type_80313FA0[chan] = 0x80;
            SITransfer(chan, &cmdFixDevice[chan], 3, &Type_80313FA0[chan], 3,
                       GetTypeCallback, 0);
            return;
        }

        if ((type & 0x00100000) != 0) {
            if ((id & 0xCFFF00) != (type & 0xCFFF00)) {
                if ((id & 0x100000) == 0) {
                    id = type & 0xCFFF00;
                    id |= 0x100000;
                    OSSetWirelessID(chan, id >> 8);
                }
                cmdFixDevice[chan] = 0x4E000000 | id;
                Type_80313FA0[chan] = 0x80;
                SITransfer(chan, &cmdFixDevice[chan], 3, &Type_80313FA0[chan],
                           3, GetTypeCallback, 0);
                return;
            }
        } else {
            if ((type & 0x40000000) != 0) {
                id = type & 0xCFFF00;
                id |= 0x100000;
                OSSetWirelessID(chan, id >> 8);
                cmdFixDevice[chan] = 0x4E000000 | id;
                Type_80313FA0[chan] = 0x80;
                SITransfer(chan, &cmdFixDevice[chan], 3,
                           &Type_80313FA0[chan], 3, GetTypeCallback, 0);
                return;
            }
            OSSetWirelessID(chan, 0);
        }

        CallTypeAndStatusCallback(chan, Type_80313FA0[chan]);
    }
}
