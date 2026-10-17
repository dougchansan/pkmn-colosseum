/**
 * @file sdk_range_800CEB64.c
 * @brief Shared source for SDK code at 0x800CEB64 - 0x800CF708 (13 fns).
 *
 * The former range combined three original libraries: OdemuExi2's
 * DebuggerDriver, Dolphin EXIBios, and Dolphin EXIUart. Small wrappers select
 * their address islands so each original compiler mode is represented.
 */
#include "dolphin/types.h"
#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"

typedef void (*DBGInterruptHandler)(__OSInterrupt interrupt, OSContext* context);

extern DBGInterruptHandler lbl_8047AA2C;
extern void (*lbl_8047AA28)(s32);
extern u8* lbl_8047AA38;
extern u8 lbl_8047AA3C;

void DBGHandler(s32 interrupt, OSContext* context);
void MWCallback(void);

volatile u32 EXI_REGS[16] : 0xCC006800;

#if defined(SDK_RANGE_800CEB64_800CED58)

void DBInitInterrupts(void) {
    __OSMaskInterrupts(0x18000);
    __OSMaskInterrupts(0x40);
    lbl_8047AA2C = (__OSInterruptHandler)MWCallback;
    __OSSetInterruptHandler(0x19, (__OSInterruptHandler)DBGHandler);
    __OSUnmaskInterrupts(0x40);
}

void DBInitComm(u8** comm, void (*callback)(s32)) {
    BOOL interrupts = OSDisableInterrupts();

    lbl_8047AA38 = &lbl_8047AA3C;
    *comm = lbl_8047AA38;
    lbl_8047AA28 = callback;
    __OSMaskInterrupts(0x18000);
    EXI_REGS[10] = 0;
    OSRestoreInterrupts(interrupts);
}

extern u8 lbl_8047AA3C;

volatile u32 lbl_CC003000 : 0xCC003000;

void DBGHandler(s32 interrupt, OSContext* context) {
    lbl_CC003000 = 0x1000;
    if (lbl_8047AA2C != NULL) {
        lbl_8047AA2C((__OSInterrupt)interrupt, context);
    }
}

void MWCallback(void) {
    lbl_8047AA3C = 1;
    if (lbl_8047AA28 != NULL) {
        lbl_8047AA28(0);
    }
}

#endif

#define IS_TRUE(value) ((value) != FALSE)
#define IS_FALSE(value) !IS_TRUE(value)

inline static u32 DBGEXISelect(u32 value) {
    u32 regs = EXI_REGS[10];

    regs &= 0x405;
    regs |= 0x80 | (value << 4);
    EXI_REGS[10] = regs;
    return TRUE;
}

inline static BOOL DBGEXISync(void) {
    while (EXI_REGS[13] & 1) {
    }
    return TRUE;
}

inline static BOOL DBGEXIDeselect(void) {
    EXI_REGS[10] &= 0x405;
    return TRUE;
}

#if defined(SDK_RANGE_800CEB64_800CED58)

s32 DBGReadStatus(u32* data) {
    extern s32 DBGEXIImm(void* buffer, s32 length, s32 write);
    BOOL total = FALSE;
    u32 value;

    DBGEXISelect(4);

    value = 1 << 30;
    total |= IS_FALSE(DBGEXIImm(&value, 2, 1));
    total |= IS_FALSE(DBGEXISync());

    total |= IS_FALSE(DBGEXIImm(data, 4, 0));
    total |= IS_FALSE(DBGEXISync());

    total |= IS_FALSE(DBGEXIDeselect());

    return IS_FALSE(total);
}

#endif

#if defined(SDK_RANGE_800CED58_800CEF10)

s32 DBGWrite(u32 type, u32* data, s32 length) {
    extern s32 DBGEXIImm(void* buffer, s32 length, s32 write);
    BOOL total;
    u32* current = data;
    u32 command;
    u32 value;
    volatile u32 scratch[3];

    DBGEXISelect(4);
    command = ((type << 8) & 0x01FFFC00) | 0xA0000000;

    total = IS_FALSE(DBGEXIImm(&command, 4, 1));
    DBGEXISync();

    while (length != 0) {
        value = *current++;
        total |= IS_FALSE(DBGEXIImm(&value, 4, 1));
        DBGEXISync();

        length -= 4;
        if (length < 0) {
            length = 0;
        }
    }

    DBGEXIDeselect();
    return IS_FALSE(total);
}

s32 DBGRead(u32 type, u32* data, s32 length) {
    extern s32 DBGEXIImm(void* buffer, s32 length, s32 write);
    BOOL total;
    u32* current = data;
    u32 command;
    u32 value;
    volatile u32 scratch[3];

    DBGEXISelect(4);
    command = ((type << 8) & 0x01FFFC00) | 0x20000000;

    total = IS_FALSE(DBGEXIImm(&command, 4, 1));
    DBGEXISync();

    while (length != 0) {
        total |= IS_FALSE(DBGEXIImm(&value, 4, 0));
        DBGEXISync();
        *current++ = value;

        length -= 4;
        if (length < 0) {
            length = 0;
        }
    }

    DBGEXIDeselect();
    return IS_FALSE(total);
}

#endif

#if defined(SDK_RANGE_800CEF10_800CF254)

s32 DBGReadMailbox(u32* data) {
    extern s32 DBGEXIImm(void* buffer, s32 length, s32 write);
    BOOL total = FALSE;
    u32 value;

    DBGEXISelect(4);

    value = 3 << 29;
    total |= IS_FALSE(DBGEXIImm(&value, 2, 1));
    total |= IS_FALSE(DBGEXISync());

    total |= IS_FALSE(DBGEXIImm(data, 4, 0));
    total |= IS_FALSE(DBGEXISync());

    total |= IS_FALSE(DBGEXIDeselect());

    return IS_FALSE(total);
}

BOOL DBGEXIImm(void* buffer, s32 bytecounter, u32 write) {
    u8* tempPointer;
    u32 writeOutValue;
    int i;

    if (write) {
        writeOutValue = 0;
        for (i = 0; i < bytecounter; i++) {
            u8* temp = (u8*)buffer + i;
            writeOutValue |= *temp << ((3 - i) << 3);
        }
        EXI_REGS[14] = writeOutValue;
    }

    EXI_REGS[13] = 1 | (write << 2) | ((bytecounter - 1) << 4);
    DBGEXISync();

    if (!write) {
        writeOutValue = EXI_REGS[14];
        tempPointer = buffer;
        for (i = 0; i < bytecounter; i++) {
            *tempPointer++ = writeOutValue >> ((3 - i) << 3);
        }
    }

    return TRUE;
}

#endif

#if defined(SDK_RANGE_800CF254_800CF47C)

typedef struct EXIProbeControl {
    u8 pad_00[0xC];
    u32 state;
    u8 pad_10[0x10];
    s32 idTime;
    u8 pad_24[0x1C];
} EXIProbeControl;

volatile s32 EXIProbeStartTime[2] : 0x800030C0;
extern EXIProbeControl lbl_803FFEF0[];

#define OS_BUS_CLOCK (*(u32*)0x800000F8)

BOOL __EXIProbe(s32 chan) {
    EXIProbeControl* exi = &lbl_803FFEF0[chan];
    BOOL enabled;
    int result;
    u32 csr;
    s32 time;

    if (chan == 2) {
        return TRUE;
    }

    result = TRUE;
    enabled = OSDisableInterrupts();
    csr = EXI_REGS[chan * 5];

    if (!(exi->state & 8)) {
        if (csr & 0x800) {
            EXI_REGS[chan * 5] = (EXI_REGS[chan * 5] & 0x7F5) | 0x800;
            EXIProbeStartTime[chan] = exi->idTime = 0;
        }

        if (csr & 0x1000) {
            time = (s32)(OSGetTime() / (OS_BUS_CLOCK / 4 / 1000) / 100) + 1;
            if (EXIProbeStartTime[chan] == 0) {
                EXIProbeStartTime[chan] = time;
            }
            if (time - EXIProbeStartTime[chan] < 3) {
                result = FALSE;
            }
        } else {
            EXIProbeStartTime[chan] = exi->idTime = 0;
            result = FALSE;
        }
    } else if (!(csr & 0x1000) || (csr & 0x800)) {
        EXIProbeStartTime[chan] = exi->idTime = 0;
        result = FALSE;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

s32 EXIProbeEx(s32 chan) {
    extern BOOL __EXIProbe(s32 chan);
    extern BOOL EXIGetID(s32 chan, u32 dev, u32* id);
    extern EXIProbeControl lbl_803FFEF0[];
    EXIProbeControl* exi = &lbl_803FFEF0[chan];
    u32 id;
    BOOL probe;

    probe = __EXIProbe(chan);
    if (probe && exi->idTime == 0) {
        probe = EXIGetID(chan, 0, &id) ? TRUE : FALSE;
    }
    if (probe) {
        return 1;
    }
    if (EXIProbeStartTime[chan] != 0) {
        return 0;
    }
    return -1;
}

#endif

#if defined(SDK_RANGE_800CF47C_800CF708)

s32 InitializeUART(u32 baud) {
    extern u32 OSGetConsoleType(void);
    extern u32 lbl_8047AA40;
    extern u32 lbl_8047AA44;
    extern u32 lbl_8047AA48;
    extern u32 lbl_8047AA4C;

    if (lbl_8047AA4C == 0xA5FF005A) {
        return 0;
    }
    if ((OSGetConsoleType() & 0x10000000) == 0) {
        lbl_8047AA48 = 0;
        return 2;
    }
    lbl_8047AA48 = 0xA5FF005A;
    lbl_8047AA40 = 0;
    lbl_8047AA44 = 1;
    return 0;
}

static int QueueLength(void) {
    extern u32 lbl_8047AA40;
    extern u32 lbl_8047AA44;
    u32 command;

    if (!EXISelect(lbl_8047AA40, lbl_8047AA44, 3)) {
        return -1;
    }

    command = 0x20010000;
    EXIImm(lbl_8047AA40, &command, 4, 1, NULL);
    EXISync(lbl_8047AA40);
    EXIImm(lbl_8047AA40, &command, 1, 0, NULL);
    EXISync(lbl_8047AA40);
    EXIDeselect(lbl_8047AA40);

    return 16 - (int)((command >> 24) & 0xFF);
}

u32 WriteUARTN(const void* buffer, u32 length) {
    extern u32 lbl_8047AA40;
    extern u32 lbl_8047AA44;
    extern u32 lbl_8047AA48;
    u32 command;
    BOOL interrupts;
    int queueLength;
    s32 transferLength;
    char* current;
    BOOL locked;
    u32 error;

    if (lbl_8047AA48 != 0xA5FF005A) {
        return 2;
    }

    interrupts = OSDisableInterrupts();
    locked = EXILock(lbl_8047AA40, lbl_8047AA44, NULL);
    if (!locked) {
        OSRestoreInterrupts(interrupts);
        return 0;
    }

    for (current = (char*)buffer; current - buffer < length; current++) {
        if (*current == '\n') {
            *current = '\r';
        }
    }

    error = 0;
    command = 0xA0010000;
    while (length != 0) {
        queueLength = QueueLength();

        if (queueLength < 0) {
            error = 3;
            break;
        }
        if (queueLength < 12 && (u32)queueLength < length) {
            continue;
        }
        if (!EXISelect(lbl_8047AA40, lbl_8047AA44, 3)) {
            error = 3;
            break;
        }

        EXIImm(lbl_8047AA40, &command, 4, 1, NULL);
        EXISync(lbl_8047AA40);

        while (queueLength != 0 && length != 0) {
            if (queueLength < 4 && (u32)queueLength < length) {
                break;
            }
            transferLength = length < 4 ? (s32)length : 4;
            EXIImm(lbl_8047AA40, (void*)buffer, transferLength, 1, NULL);
            buffer = (u8*)buffer + transferLength;
            length -= transferLength;
            queueLength -= transferLength;
            EXISync(lbl_8047AA40);
        }
        EXIDeselect(lbl_8047AA40);
    }

    EXIUnlock(lbl_8047AA40);
    OSRestoreInterrupts(interrupts);
    return error;
}

#endif
