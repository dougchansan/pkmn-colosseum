/**
 * @file SI_range_800CF764.c
 * @brief Dolphin SI (one TU proven by static GetTypeCallback linkage), 0x800CF764 - 0x800D0DF8.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * Ported against the Dolphin SDK SIBios.c reference; SIBusy / SIIsChanBusy /
 * SIClearTCInterrupt / SITransferNext / CallTypeAndStatusCallback have no
 * symbol of their own in the target (they are split out or inlined).
 */
#include "dolphin/types.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/vi/VI.h"

/* The bus clock is the SDK's address-bound __OSBusClock, which MWCC
 * reloads at each use. */
extern u32 __OSBusClock : 0x800000F8;
#undef OS_BUS_CLOCK
#define OS_BUS_CLOCK __OSBusClock

/* SI.h's approximation loses the SDK's divide-by-8 rounding step. */
#undef OSMicrosecondsToTicks
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)

typedef struct {
    u32 reg;
    u32 unk4;
    u32 unk8;
} SICommandQueueEntry;
volatile SICommandQueueEntry __SIChannelRegs[4] : 0xCC006400;

volatile u32 __SIRegs[15] : 0xCC006400;
#define SI_COMCSR_IDX (0x34 / 4)
#define SI_STATUS_IDX (0x38 / 4)
#define SI_POLL_IDX (0x30 / 4)

#define SI_COMCSR_TCINT_MASK 0x80000000u
#define SI_COMCSR_TCINTMSK_MASK 0x40000000u
#define SI_COMCSR_COMERR_MASK 0x20000000u
#define SI_COMCSR_RDSTINT_MASK 0x10000000u
#define SI_COMCSR_RDSTINTMSK_MASK 0x08000000u
#define SI_COMCSR_TSTART_MASK 0x00000001u

#define ROUND(n, a) (((u32)(n) + (a)-1) & ~((a)-1))

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
static __OSInterruptHandler lbl_80400150[4];
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

const char* __SIVersion =
    "<< Dolphin SDK - SI\trelease build: Sep  5 2002 05:33:08 (0x2301) >>";

static SIControl Si_80313F8C = {-1, 0, 0, NULL, NULL};
static u32 Type_80313FA0[4] = {8, 8, 8, 8};

u32 lbl_8047AA58; /* __PADFixBits */

int __SITransfer(s32 chan, void* output, u32 outputBytes, void* input,
                 u32 inputBytes, SICallback callback);
BOOL SIGetResponseRaw(s32 chan);
static void GetTypeCallback(s32 chan, u32 error, OSContext* context);

BOOL SIBusy(void) {
    if (Si_80313F8C.chan != -1) {
        return TRUE;
    }
    return FALSE;
}

BOOL SIIsChanBusy(s32 channel) {
    BOOL available = TRUE;

    if (Packet_803FFFB0[channel].chan == -1 && Si_80313F8C.chan != channel) {
        available = FALSE;
    }
    return available;
}

static inline BOOL SIIsChanBusyLocal(s32 chan) {
    return Packet_803FFFB0[chan].chan != -1 || Si_80313F8C.chan == chan;
}

static inline void SIClearTCInterrupt(void) {
    u32 reg;

    reg = __SIRegs[SI_COMCSR_IDX];
    reg |= SI_COMCSR_TCINT_MASK;
    reg &= ~SI_COMCSR_TSTART_MASK;
    __SIRegs[SI_COMCSR_IDX] = reg;
}

u32 CompleteTransfer(void) {
    u32 sr;
    u32 i;
    u32 rLen;
    u8* input;
    u32 temp;

    sr = __SIRegs[SI_STATUS_IDX];
    SIClearTCInterrupt();

    if (Si_80313F8C.chan != -1) {
        XferTime[Si_80313F8C.chan] = __OSGetSystemTime();
        input = Si_80313F8C.input;
        rLen = Si_80313F8C.inputBytes / sizeof(u32);
        for (i = 0; i < rLen; i++) {
            *((u32*)input)++ = __SIRegs[i + 0x20];
        }

        rLen = Si_80313F8C.inputBytes & 3;
        if (rLen != 0) {
            temp = __SIRegs[i + 32];
            for (i = 0; i < rLen; i++) {
                *(input++) = temp >> ((3 - i) * 8);
            }
        }

        if (__SIRegs[SI_COMCSR_IDX] & SI_COMCSR_COMERR_MASK) {
            sr >>= (3 - Si_80313F8C.chan) * 8;
            sr &= 0xF;
            if ((sr & 8) != 0 && (Type_80313FA0[Si_80313F8C.chan] & 0x80) == 0) {
                Type_80313FA0[Si_80313F8C.chan] = 8;
            }

            if (sr == 0) {
                sr = 4;
            }
        } else {
            TypeTime[Si_80313F8C.chan] = __OSGetSystemTime();
            sr = 0;
        }

        Si_80313F8C.chan = -1;
    }

    return sr;
}

static inline void SITransferNext(s32 chan) {
    int i;
    SIPacket* packet;

    for (i = 0; i < 4; i++) {
        chan++;
        chan %= 4;
        packet = &Packet_803FFFB0[chan];

        if (packet->chan != -1) {
            if (packet->fire <= __OSGetSystemTime()) {
                if (__SITransfer(packet->chan, packet->output, packet->outputBytes,
                                 packet->input, packet->inputBytes,
                                 packet->callback) != 0) {
                    OSCancelAlarm(&lbl_80400030[chan]);
                    packet->chan = -1;
                }
                return;
            }
        }
    }
}

void SIInterruptHandler_800CFA60(__OSInterrupt interrupt, OSContext* context) {
    u32 reg;
    s32 chan;
    u32 sr;
    SICallback callback;
    int i;
    u32 vcount;
    u32 x;

    reg = __SIRegs[SI_COMCSR_IDX];
    if ((reg & (SI_COMCSR_TCINT_MASK | SI_COMCSR_TCINTMSK_MASK)) ==
        (SI_COMCSR_TCINT_MASK | SI_COMCSR_TCINTMSK_MASK)) {
        chan = Si_80313F8C.chan;
        sr = CompleteTransfer();
        callback = Si_80313F8C.callback;
        Si_80313F8C.callback = NULL;
        SITransferNext(chan);

        if (callback) {
            callback(chan, sr, context);
        }

        sr = __SIRegs[SI_STATUS_IDX];
        sr &= 0x0F000000 >> (chan << 3);
        __SIRegs[SI_STATUS_IDX] = sr;

        if (Type_80313FA0[chan] == 0x80 && !SIIsChanBusyLocal(chan)) {
            static u32 cmdTypeAndStatus;

            SITransfer(chan, &cmdTypeAndStatus, 1, &Type_80313FA0[chan], 3,
                       GetTypeCallback, OSMicrosecondsToTicks(65));
        }
    }

    if ((reg & (SI_COMCSR_RDSTINT_MASK | SI_COMCSR_RDSTINTMSK_MASK)) ==
        (SI_COMCSR_RDSTINT_MASK | SI_COMCSR_RDSTINTMSK_MASK)) {
        vcount = 1 + VIGetCurrentLine();
        x = (Si_80313F8C.poll & (0x3FF << 16)) >> 16;

        for (i = 0; i < 4; i++) {
            if (SIGetResponseRaw(i)) {
                InputBufferVcount[i] = vcount;
            }
        }

        for (i = 0; i < 4; i++) {
            if ((Si_80313F8C.poll & (0x80000000 >> (24 + i))) != 0) {
                if (InputBufferVcount[i] == 0 ||
                    ((x >> 1) + InputBufferVcount[i]) < vcount) {
                    return;
                }
            }
        }

        for (i = 0; i < 4; i++) {
            InputBufferVcount[i] = 0;
        }

        for (i = 0; i < 4; i++) {
            if (lbl_80400150[i] != 0) {
                (*lbl_80400150[i])(interrupt, context);
            }
        }
    }
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
        if (lbl_80400150[i] == handler) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }

    for (i = 0; i < 4; i++) {
        if (lbl_80400150[i] == 0) {
            lbl_80400150[i] = handler;
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

    enabled = OSDisableInterrupts();

    for (i = 0; i < 4; i++) {
        if (lbl_80400150[i] == handler) {
            lbl_80400150[i] = 0;

            for (i = 0; i < 4; i++) {
                if (lbl_80400150[i] != 0) {
                    break;
                }
            }

            if (i == 4) {
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
    OSRegisterVersion(__SIVersion);

    Packet_803FFFB0[3].chan = -1;
    Packet_803FFFB0[2].chan = -1;
    Packet_803FFFB0[1].chan = -1;
    Packet_803FFFB0[0].chan = -1;
    Si_80313F8C.poll = 0;
    SISetSamplingRate(0);

    while (__SIRegs[SI_COMCSR_IDX] & SI_COMCSR_TSTART_MASK) {
    }
    __SIRegs[SI_COMCSR_IDX] = SI_COMCSR_TCINT_MASK;

    __OSSetInterruptHandler(0x14, SIInterruptHandler_800CFA60);
    __OSUnmaskInterrupts(0x800);

    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);
}

int __SITransfer(s32 chan, void* output, u32 outputBytes, void* input,
                 u32 inputBytes, SICallback callback) {
    BOOL enabled;
    u32 rLen;
    u32 i;
    u32 sr;
    union {
        u32 val;
        struct {
            u32 tcint : 1;
            u32 tcintmsk : 1;
            u32 comerr : 1;
            u32 rdstint : 1;
            u32 rdstintmsk : 1;
            u32 pad2 : 4;
            u32 outlngth : 7;
            u32 pad1 : 1;
            u32 inlngth : 7;
            u32 pad0 : 5;
            u32 channel : 2;
            u32 tstart : 1;
        } f;
    } comcsr;

    enabled = OSDisableInterrupts();
    if (Si_80313F8C.chan != -1) {
        OSRestoreInterrupts(enabled);
        return 0;
    }

    sr = __SIRegs[SI_STATUS_IDX];
    sr &= (0x0F000000 >> (chan * 8));
    __SIRegs[SI_STATUS_IDX] = sr;

    Si_80313F8C.chan = chan;
    Si_80313F8C.callback = callback;
    Si_80313F8C.inputBytes = inputBytes;
    Si_80313F8C.input = input;

    rLen = ROUND(outputBytes, 4) / 4;
    for (i = 0; i < rLen; i++) {
        __SIRegs[i + 0x20] = ((u32*)output)[i];
    }

    comcsr.val = __SIRegs[SI_COMCSR_IDX];
    comcsr.f.tcint = 1;
    comcsr.f.tcintmsk = callback ? 1 : 0;
    comcsr.f.outlngth = outputBytes == 0x80 ? 0 : outputBytes;
    comcsr.f.inlngth = inputBytes == 0x80 ? 0 : inputBytes;
    comcsr.f.channel = chan;
    comcsr.f.tstart = 1;

    __SIRegs[SI_COMCSR_IDX] = comcsr.val;
    OSRestoreInterrupts(enabled);
    return 1;
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

void SISetCommand(s32 chan, u32 command) {
    __SIChannelRegs[chan].reg = command;
}

void SITransferCommands(void) {
    volatile u32* commandRegister;
    u32 command;

    command = 0x80000000u;
    *(commandRegister = (volatile u32*)0xCC006438) = command;
}

u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;
    enabled = OSDisableInterrupts();
    Si_80313F8C.poll &= 0xFC0000FF;
    Si_80313F8C.poll |= poll;
    poll = Si_80313F8C.poll;
    __SIRegs[SI_POLL_IDX] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si_80313F8C.poll;
    }

    enabled = OSDisableInterrupts();
    poll >>= 24;
    en = poll & 0xF0;
    poll &= (en >> 4) | 0x03FFFFF0;
    poll &= 0xFC0000FF;
    Si_80313F8C.poll &= ~(en >> 4);
    Si_80313F8C.poll |= poll;
    poll = Si_80313F8C.poll;

    __SIRegs[SI_STATUS_IDX] = 0x80000000u;
    __SIRegs[SI_POLL_IDX] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

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

static inline u32 SIGetStatusInline(s32 chan) {
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

BOOL SIGetResponseRaw(s32 chan) {
    u32 sr;

    sr = SIGetStatusInline(chan);
    if (sr & 0x20) {
        InputBuffer[chan][0] = __SIRegs[3 * chan + 1];
        InputBuffer[chan][1] = __SIRegs[3 * chan + 2];
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
        InputBuffer[chan][0] = __SIChannelRegs[chan].unk4;
        InputBuffer[chan][1] = __SIChannelRegs[chan].unk8;
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

BOOL SITransfer(s32 chan, void* output, u32 outputBytes, void* input,
                u32 inputBytes, SICallback callback, s64 delay) {
    BOOL enabled;
    SIPacket* packet;
    s64 now;
    s64 fire;

    packet = &Packet_803FFFB0[chan];
    enabled = OSDisableInterrupts();

    if (packet->chan != -1 || Si_80313F8C.chan == chan) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    now = __OSGetSystemTime();
    if (delay == 0) {
        fire = now;
    } else {
        fire = delay + XferTime[chan];
    }

    if (now < fire) {
        delay = fire - now;
        OSSetAlarm(&lbl_80400030[chan], delay, AlarmHandler_800D0668);
    } else if (__SITransfer(chan, output, outputBytes, input, inputBytes,
                            callback)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    packet->chan = chan;
    packet->output = output;
    packet->outputBytes = outputBytes;
    packet->input = input;
    packet->inputBytes = inputBytes;
    packet->callback = callback;
    packet->fire = fire;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

static inline void CallTypeAndStatusCallback(s32 chan, u32 type) {
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

u32 SIGetType(s32 chan) {
    static u32 cmdTypeAndStatus;
    BOOL enabled;
    u32 type;
    s64 diff;

    enabled = OSDisableInterrupts();
    type = Type_80313FA0[chan];
    diff = __OSGetSystemTime() - TypeTime[chan];
    if ((Si_80313F8C.poll & (0x80 >> chan)) != 0) {
        if (type != 8) {
            TypeTime[chan] = __OSGetSystemTime();
            OSRestoreInterrupts(enabled);
            return type;
        }

        type = Type_80313FA0[chan] = 0x80;
    } else {
        if (diff <= OSMillisecondsToTicks(50) && type != 8) {
            OSRestoreInterrupts(enabled);
            return type;
        }

        if (diff <= OSMillisecondsToTicks(75)) {
            Type_80313FA0[chan] = 0x80;
        } else {
            type = Type_80313FA0[chan] = 0x80;
        }
    }

    TypeTime[chan] = __OSGetSystemTime();
    SITransfer(chan, &cmdTypeAndStatus, 1, &Type_80313FA0[chan], 3,
               GetTypeCallback, OSMicrosecondsToTicks(65));
    OSRestoreInterrupts(enabled);
    return type;
}

u32 SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback) {
    BOOL enabled;
    u32 type;
    int i;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);

    if ((Type_80313FA0[chan] & 0x80) != 0) {
        for (i = 0; i < 4; i++) {
            if (TypeCallback[chan][i] == callback) {
                break;
            }

            if (TypeCallback[chan][i] == 0) {
                TypeCallback[chan][i] = callback;
                break;
            }
        }
    } else {
        (*callback)(chan, type);
    }

    OSRestoreInterrupts(enabled);
    return type;
}
