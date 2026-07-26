#include "dolphin/types.h"

extern void MWTRACE(s32 level, const char* fmt, ...);
extern void* memset(void* dst, int val, u32 len);

extern s32  TRKTargetReadInstruction(u8* buf, u32 pc);
extern void TRKAppendBuffer_ui8(s32 a, void* buf, u32 len);
extern s32 TRKAppendBuffer_ui32(s32 buffer, u32* data, u32 count);
extern s32 TRKReadBuffer_ui32(s32 buffer, u32* data, u32 count);

/* TRK state and CPU state structures */
extern u8 gTRKState[];
extern u8 gTRKCPUState[];

/* Breakpoint info @ 0x80313834 */
extern u8 lbl_80313834[];

/* TRK exception status structure @ 0x80313824 */
extern u8 gTRKExceptionStatus_80313824[];
extern u8 gTRKRestoreFlags[];

typedef struct TRKTargetExceptionStatus {
    u32 words[3];
    u8 pad_0C;
    u8 exceptionDetected;
    u8 pad_0E[2];
} TRKTargetExceptionStatus;

/* "TargetDoStep()\n" */
extern u8 lbl_8026FB70[];

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C1348_800C1548)

s32 TRKTargetSupportRequest(void)
{
    extern void fn_800BE464(void* event, s32 type);
    extern s32 TRKPostEvent(void* event);
    extern s32 HandleOpenFileSupportRequest(u32 handle, u8 mode, u32 length,
                                            s32* ioResult);
    extern s32 HandleCloseFileSupportRequest(u32 handle, s32* ioResult);
    extern s32 HandlePositionFileSupportRequest(u32 handle, u32* position,
                                                u8 mode, s32* ioResult);
    extern s32 TRKSuppAccessFile(u32 handle, u8* data, u32* length,
                                s32* ioResult, s32 needReply, s32 read);
    extern void TRK_flush_cache(void* address, u32 length);

    s32 ioResult;
    u32* length;
    s32 command;
    s32 result;
    u32 position;
    u8 event[12];

    command = *(s32*) (gTRKCPUState + 0xC);
    if (command != 0xD1 && command != 0xD0 && command != 0xD2 &&
        command != 0xD3 && command != 0xD4)
    {
        fn_800BE464(event, 4);
        TRKPostEvent(event);
        return 0;
    } else if (command == 0xD2) {
        result = HandleOpenFileSupportRequest(
            *(u32*) (gTRKCPUState + 0x10),
            *(u32*) (gTRKCPUState + 0x14) & 0xFF,
            *(u32*) (gTRKCPUState + 0x18), &ioResult);
        if (ioResult == 0 && result != 0) {
            ioResult = 1;
        }
        *(s32*) (gTRKCPUState + 0xC) = ioResult;
    } else if (command == 0xD3) {
        result = HandleCloseFileSupportRequest(
            *(u32*) (gTRKCPUState + 0x10), &ioResult);
        if (ioResult == 0 && result != 0) {
            ioResult = 1;
        }
        *(s32*) (gTRKCPUState + 0xC) = ioResult;
    } else if (command == 0xD4) {
        position = *(u32*) *(u32*) (gTRKCPUState + 0x14);
        result = HandlePositionFileSupportRequest(
            *(u32*) (gTRKCPUState + 0x10), &position,
            *(u32*) (gTRKCPUState + 0x18) & 0xFF, &ioResult);
        if (ioResult == 0 && result != 0) {
            ioResult = 1;
        }
        *(s32*) (gTRKCPUState + 0xC) = ioResult;
        *(u32*) *(u32*) (gTRKCPUState + 0x14) = position;
    } else {
        length = (u32*) *(u32*) (gTRKCPUState + 0x14);
        result = TRKSuppAccessFile(
            *(u32*) (gTRKCPUState + 0x10),
            (u8*) *(u32*) (gTRKCPUState + 0x18), length, &ioResult, 1,
            command == 0xD1);
        if (ioResult == 0 && result != 0) {
            ioResult = 1;
        }
        *(s32*) (gTRKCPUState + 0xC) = ioResult;
        if (command == 0xD1) {
            TRK_flush_cache((void*) *(u32*) (gTRKCPUState + 0x18), *length);
        }
    }
    *(u32*) (gTRKCPUState + 0x80) += 4;
    return result;
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C1548_800C17CC)

/* TRKTargetGetPC - 0x800C1548 | size 0x10 | scope none */
s32 TRKTargetGetPC(void) {
    return *(s32*)&gTRKCPUState[0x80];
}

/* TRKTargetStepOutOfRange - 0x800C1558 | size 0xB8 | scope none */
s32 TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, s32 c) {
    u8* bpInfo;
    u8* cpuState;
    s32 bpType;
    u32 msr;

    if (c != 0) {
        return 0x703;
    }
    bpInfo = lbl_80313834;
    *(u32*)&bpInfo[0xC] = rangeStart;
    *(u32*)&bpInfo[0x10] = rangeEnd;
    *(s32*)&bpInfo[0x4] = 1;
    *(s32*)&bpInfo[0x0] = 1;

    MWTRACE(1, (const char*)lbl_8026FB70);

    cpuState = gTRKCPUState;
    bpType = *(s32*)&bpInfo[0x4];
    msr = *(u32*)&cpuState[0x1F8];
    msr |= 0x400;
    *(u32*)&cpuState[0x1F8] = msr;

    if (bpType == 0 || bpType == 0x10) {
        *(u32*)&lbl_80313834[0x8] = *(u32*)&lbl_80313834[0x8] - 1;
    }
    *(s32*)&gTRKState[0x98] = 0;
    return 0;
}

/* TRKTargetSingleStep - 0x800C1610 | size 0xAC | scope none */
s32 TRKTargetSingleStep(u32 count, s32 c) {
    u8* bpInfo;
    u8* cpuState;
    s32 bpType;
    u32 msr;

    if (c != 0) {
        return 0x703;
    }
    bpInfo = lbl_80313834;
    *(u32*)&bpInfo[0x8] = count;
    *(s32*)&bpInfo[0x4] = 0;
    *(s32*)&bpInfo[0x0] = 1;

    MWTRACE(1, (const char*)lbl_8026FB70);

    cpuState = gTRKCPUState;
    bpType = *(s32*)&bpInfo[0x4];
    msr = *(u32*)&cpuState[0x1F8];
    msr |= 0x400;
    *(u32*)&cpuState[0x1F8] = msr;

    if (bpType == 0 || bpType == 0x10) {
        *(u32*)&bpInfo[0x8] = *(u32*)&bpInfo[0x8] - 1;
    }
    *(s32*)&gTRKState[0x98] = 0;
    return 0;
}

/* TRKTargetAddExceptionInfo - 0x800C16BC | size 0x84 | scope none */
void TRKTargetAddExceptionInfo(s32 arg) {
    u8 buf[0x40];
    s32 result;
    u32 dataword;

    memset(buf, 0, 0x40);
    dataword = *(u32*)gTRKExceptionStatus_80313824;
    *(u32*)&buf[0x0] = 0x40;
    buf[0x4] = 0x91;
    *(u32*)&buf[0x8] = dataword;
    TRKTargetReadInstruction((u8*)&result, dataword);
    *(u32*)&buf[0xC] = result;
    *(u32*)&buf[0x10] = *(u16*)&gTRKExceptionStatus_80313824[0x8];
    TRKAppendBuffer_ui8(arg, buf, 0x40);
}

/* TRKTargetAddStopInfo - 0x800C1740 | size 0x8C | scope none */
void TRKTargetAddStopInfo(s32 arg) {
    u8 buf[0x40];
    s32 result;
    u32 dataword;

    memset(buf, 0, 0x40);
    dataword = *(u32*)&gTRKCPUState[0x80];
    *(u32*)&buf[0x0] = 0x40;
    buf[0x4] = 0x90;
    *(u32*)&buf[0x8] = dataword;
    TRKTargetReadInstruction((u8*)&result, dataword);
    *(u32*)&buf[0xC] = result;
    *(u32*)&buf[0x10] = *(u32*)&gTRKCPUState[0x2F8] & 0xFFFF;
    TRKAppendBuffer_ui8(arg, buf, 0x40);
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C17CC_800C195C)

s32 TRKTargetInterrupt(void* event)
{
    extern s32 TRKDoNotifyStopped(s32 command);
    s32 result = 0;

    switch (*(s32*)event) {
    case 3:
    case 4:
        if (*(s32*)&lbl_80313834[0] != 0) {
            s32 done = 1;

            *(u32*)&gTRKCPUState[0x1F8] &= ~0x400;
            if ((*(u32*)&gTRKCPUState[0x2F8] & 0xFFFF) == 0xD00) {
                switch (*(s32*)&lbl_80313834[4]) {
                case 0:
                    if (*(u32*)&lbl_80313834[8] > 0) {
                        done = 0;
                    }
                    break;
                case 1:
                    if (*(u32*)&gTRKCPUState[0x80] >=
                            *(u32*)&lbl_80313834[0xC] &&
                        *(u32*)&gTRKCPUState[0x80] <=
                            *(u32*)&lbl_80313834[0x10])
                    {
                        done = 0;
                    }
                    break;
                }
            }
            if (done) {
                *(s32*)&lbl_80313834[0] = 0;
            } else {
                s32 stepType;

                *(s32*)&lbl_80313834[0] = 1;
                MWTRACE(1, (const char*)lbl_8026FB70);
                stepType = *(s32*)&lbl_80313834[4];
                *(u32*)&gTRKCPUState[0x1F8] |= 0x400;
                if (stepType == 0 || stepType == 0x10) {
                    *(u32*)&lbl_80313834[8] -= 1;
                }
                *(s32*)&gTRKState[0x98] = 0;
            }
        }
        if (*(s32*)&lbl_80313834[0] == 0) {
            *(s32*)&gTRKState[0x98] = 1;
            result = TRKDoNotifyStopped(0x90);
        }
        break;
    }
    return result;
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C195C_800C1A08)

/* TRKPostInterruptEvent - 0x800C195C | size 0xAC | scope global */
void TRKPostInterruptEvent(void) {
    u32 instruction;
    u8 event[0xC];
    s32 eventType;

    extern void fn_800BE464(void* event, s32 type);
    extern s32 TRKPostEvent(void* event);

    if (*(s32*)&gTRKState[0x9C] != 0) {
        *(s32*)&gTRKState[0x9C] = 0;
    } else {
        s32 exceptionID = *(s32*)&gTRKCPUState[0x2F8] & 0xFFFF;

        switch (exceptionID) {
        case 0x700:
        case 0xD00:
            TRKTargetReadInstruction((u8*)&instruction, *(u32*)&gTRKCPUState[0x80]);
            if (instruction == 0x0FE00000) {
                eventType = 5;
            } else {
                eventType = 3;
            }
            break;
        default:
            eventType = 4;
            break;
        }
        fn_800BE464(event, eventType);
        TRKPostEvent(event);
    }
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C24BC_800C25FC)

/* TRKTargetAccessDefault - 0x800C24BC | size 0xF4 | scope global */
s32 TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, s32 buffer,
                           u32* transferSize, s32 read) {
    typedef struct TRKExceptionState {
        u32 words[4];
    } TRKExceptionState;

    extern s32 TRKAppendBuffer_ui32(s32 buffer, u32* data, u32 count);
    extern s32 TRKReadBuffer_ui32(s32 buffer, u32* data, u32 count);

    s32 result;
    u32 registerCount;
    u32* registers;
    TRKExceptionState savedState;

    if (lastRegister > 0x24) {
        return 0x701;
    }

    savedState = *(TRKExceptionState*)gTRKExceptionStatus_80313824;
    gTRKExceptionStatus_80313824[0xD] = 0;
    registers = (u32*)gTRKCPUState + firstRegister;
    registerCount = lastRegister - firstRegister + 1;
    *transferSize = registerCount * sizeof(u32);

    if (read != 0) {
        result = TRKAppendBuffer_ui32(buffer, registers, registerCount);
    } else {
        result = TRKReadBuffer_ui32(buffer, registers, registerCount);
    }

    if (gTRKExceptionStatus_80313824[0xD] != 0) {
        *transferSize = 0;
        result = 0x702;
    }

    *(TRKExceptionState*)gTRKExceptionStatus_80313824 = savedState;
    return result;
}

/* TRKTargetReadInstruction - 0x800C25B0 | size 0x4C | scope global */
s32 TRKTargetReadInstruction(u8* buf, u32 pc) {
    extern s32 TRKTargetAccessMemory(u8* buf, u32 address, u32* length,
                                     s32 memorySpace, s32 read);

    u32 length = sizeof(u32);
    s32 result = TRKTargetAccessMemory(buf, pc, &length, 0, 1);

    if (result == 0 && length != sizeof(u32)) {
        result = 0x700;
    }
    return result;
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT)

s32 TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, s32 buffer,
                             u32* transferSize, s32 read)
{
    TRKTargetExceptionStatus savedState;
    s32 result;
    u32* registers;
    u32 registerCount;

    if (lastRegister > 0x60) {
        return 0x701;
    }

    savedState =
        *(TRKTargetExceptionStatus*) gTRKExceptionStatus_80313824;
    gTRKExceptionStatus_80313824[0xD] = 0;
    *transferSize = 0;
    if (firstRegister <= lastRegister) {
        registers = (u32*) (gTRKCPUState + 0x1A8) + firstRegister;
        registerCount = lastRegister - firstRegister + 1;
        *transferSize += registerCount * sizeof(u32);
        if (read) {
            result = TRKAppendBuffer_ui32(buffer, registers, registerCount);
        } else {
            if (registers <= (u32*) (gTRKCPUState + 0x1EC) &&
                registers + registerCount - 1 >=
                    (u32*) (gTRKCPUState + 0x1E8))
            {
                gTRKRestoreFlags[0] = 1;
            }
            if (registers <= (u32*) (gTRKCPUState + 0x278) &&
                registers + registerCount - 1 >=
                    (u32*) (gTRKCPUState + 0x278))
            {
                gTRKRestoreFlags[1] = 1;
            }
            result = TRKReadBuffer_ui32(buffer, registers, registerCount);
        }
    }
    if (gTRKExceptionStatus_80313824[0xD] != 0) {
        *transferSize = 0;
        result = 0x702;
    }
    *(TRKTargetExceptionStatus*) gTRKExceptionStatus_80313824 = savedState;
    return result;
}

s32 TRKTargetAccessMemory(void* data, u32 start, u32* length,
                          s32 accessOptions, s32 read)
{
    extern void* TRKTargetTranslate(u32 address);
    extern s32 TRKValidMemory32(void* address, u32 length, s32 write);
    extern u32 fn_800C0E60(void);
    extern void TRK_ppc_memcpy(void* destination, const void* source,
                               u32 length, u32 destinationMSR,
                               u32 sourceMSR);
    extern void TRK_flush_cache(void* address, u32 length);

    TRKTargetExceptionStatus savedState =
        *(TRKTargetExceptionStatus*) gTRKExceptionStatus_80313824;
    s32 result;
    u32 currentMSR;
    void* address;
    u32 targetMSR;

    gTRKExceptionStatus_80313824[0xD] = 0;
    address = TRKTargetTranslate(start);
    result = TRKValidMemory32(address, *length, read == FALSE);
    if (result != 0) {
        *length = 0;
    } else {
        currentMSR = fn_800C0E60();
        targetMSR =
            currentMSR | (*(u32*) (gTRKCPUState + 0x1F8) & 0x10);
        if (read) {
            TRK_ppc_memcpy(data, address, *length, currentMSR, targetMSR);
        } else {
            TRK_ppc_memcpy(address, data, *length, targetMSR, currentMSR);
            TRK_flush_cache(address, *length);
            if ((void*) start != address) {
                TRK_flush_cache((void*) start, *length);
            }
        }
    }
    if (gTRKExceptionStatus_80313824[0xD] != 0) {
        *length = 0;
        result = 0x702;
    }
    *(TRKTargetExceptionStatus*) gTRKExceptionStatus_80313824 = savedState;
    return result;
}

/* TRKAccessFile - 0x800C29F0 | size: 0x8 | scope global */
u32 TRKAccessFile(u32 cmd, u32 dir, u32* addrBuf, u32 len) {
    (void)cmd;
    (void)dir;
    (void)addrBuf;
    (void)len;
    return 0;
}

/* TRKOpenFile - 0x800C29F8 | size: 0x8 | scope global */
u32 TRKOpenFile(u32 cmd, u32 dir, u32* addrBuf, u32 len) {
    (void)cmd;
    return TRKAccessFile(0xD2, dir, addrBuf, len);
}

/* TRKCloseFile - 0x800C2A00 | size: 0x8 | scope global */
u32 TRKCloseFile(u32 cmd, u32 param) {
    (void)cmd;
    return TRKAccessFile(cmd, param, NULL, 0);
}

/* TRKPositionFile - 0x800C2A08 | size: 0x8 | scope global */
u32 TRKPositionFile(u32 cmd, u32 dir, u32* addrBuf, u32 len) {
    (void)cmd;
    return TRKAccessFile(0xD4, dir, addrBuf, len);
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C1A08_800C24BC)

extern const u32 lbl_8026FAF8[10];
extern const u32 lbl_8026FB20[10];
extern const u32 lbl_8026FB48[10];
extern u8 lbl_803FED44[];
extern void TRK_flush_cache(void* address, u32 length);
extern s32 TRKAppendBuffer1_ui64(s32 buffer, u64 value);
extern s32 TRKReadBuffer1_ui64(s32 buffer, u64* value);
extern u32 fn_800C0E60(void);
extern void fn_800C0E68(u32 msr);
extern void fn_800C11F4(void* value);
extern void fn_800C1218(void* value);

static inline s32 TRKPPCAccessSpecialReg(void* value, u32* instructions)
{
    typedef void (*AccessFunction)(void*, void*);

    instructions[9] = 0x4E800020;
    TRK_flush_cache(instructions, sizeof(u32[10]) * 10);
    ((AccessFunction)instructions)(value, lbl_803FED44);
    return 0;
}

static inline s32 TRKPPCAccessSPR(void* value, u32 spr, s32 read)
{
    u32 instructions[10];

    memcpy(instructions, lbl_8026FAF8, sizeof(instructions));
    if (read) {
        instructions[0] =
            0x7C0002A6 | (4 << 21) | ((spr & 0xFE0) << 6) |
            ((spr & 0x1F) << 16);
        instructions[1] = 0x90000000 | (4 << 21) | (3 << 16);
    } else {
        instructions[0] = 0x80000000 | (4 << 21) | (3 << 16);
        instructions[1] =
            0x7C0003A6 | (4 << 21) | ((spr & 0xFE0) << 6) |
            ((spr & 0x1F) << 16);
    }
    return TRKPPCAccessSpecialReg(value, instructions);
}

static inline s32 TRKPPCAccessPairedSingleRegister(
    void* value, u32 reg, s32 read)
{
    u32 instructions[10];

    memcpy(instructions, lbl_8026FB20, sizeof(instructions));
    if (read) {
        instructions[0] = 0xF0000000 | (reg << 21) | (3 << 16);
    } else {
        instructions[0] = 0xE0000000 | (reg << 21) | (3 << 16);
    }
    return TRKPPCAccessSpecialReg(value, instructions);
}

static inline s32 TRKPPCAccessFPRegister(void* value, u32 reg, s32 read)
{
    s32 result = 0;
    u32 instructions[10];

    memcpy(instructions, lbl_8026FB48, sizeof(instructions));
    if (reg < 0x20) {
        if (read) {
            instructions[0] = 0xD8000000 | (reg << 21) | (3 << 16);
        } else {
            instructions[0] = 0xC8000000 | (reg << 21) | (3 << 16);
        }
        result = TRKPPCAccessSpecialReg(value, instructions);
    } else if (reg == 0x20) {
        if (read) {
            fn_800C1218(value);
        } else {
            fn_800C11F4(value);
        }
        *(u64*)value &= 0xFFFFFFFF;
    } else if (reg == 0x21) {
        if (!read) {
            *(u32*)value = *((u32*)value + 1);
        }
        result = TRKPPCAccessSPR(value, 1022, read);
        if (read) {
            *(u64*)value = *(u32*)value & 0xFFFFFFFFLL;
        }
    }
    return result;
}

s32 TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, s32 buffer,
                      u32* transferSize, s32 read)
{
    u64 value;
    s32 result;
    TRKTargetExceptionStatus savedState;
    u32 current;

    if (lastRegister > 0x21) {
        return 0x701;
    }

    savedState =
        *(TRKTargetExceptionStatus*)gTRKExceptionStatus_80313824;
    gTRKExceptionStatus_80313824[0xD] = 0;
    fn_800C0E68(fn_800C0E60() | 0x2000);
    *transferSize = 0;
    result = 0;

    for (current = firstRegister;
         current <= lastRegister && result == 0;
         current++, *transferSize += sizeof(u64))
    {
        if (read) {
            TRKPPCAccessFPRegister(&value, current, read);
            result = TRKAppendBuffer1_ui64(buffer, value);
        } else {
            TRKReadBuffer1_ui64(buffer, &value);
            result = TRKPPCAccessFPRegister(&value, current, read);
        }
    }

    if (gTRKExceptionStatus_80313824[0xD] != 0) {
        *transferSize = 0;
        result = 0x702;
    }
    *(TRKTargetExceptionStatus*)gTRKExceptionStatus_80313824 = savedState;
    return result;
}

s32 TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, s32 buffer,
                             u32* transferSize, s32 read)
{
    TRKTargetExceptionStatus savedState;
    u32 current;
    u32 sprValue[1];
    u64 value;
    s32 result;

    if (lastRegister > 0x1F) {
        return 0x701;
    }

    savedState =
        *(TRKTargetExceptionStatus*)gTRKExceptionStatus_80313824;
    gTRKExceptionStatus_80313824[0xD] = 0;

    TRKPPCAccessSPR(sprValue, 920, 1);
    sprValue[0] |= 0xA0000000;
    TRKPPCAccessSPR(sprValue, 920, 0);
    sprValue[0] = 0;
    TRKPPCAccessSPR(sprValue, 912, 0);

    *transferSize = 0;
    result = 0;
    for (current = firstRegister;
         current <= lastRegister && result == 0;
         current++)
    {
        if (read) {
            result =
                TRKPPCAccessPairedSingleRegister(&value, current, read);
            result = TRKAppendBuffer1_ui64(buffer, value);
        } else {
            result = TRKReadBuffer1_ui64(buffer, &value);
            result =
                TRKPPCAccessPairedSingleRegister(&value, current, read);
        }
        *transferSize += sizeof(u64);
    }

    if (gTRKExceptionStatus_80313824[0xD] != 0) {
        *transferSize = 0;
        result = 0x702;
    }
    *(TRKTargetExceptionStatus*)gTRKExceptionStatus_80313824 = savedState;
    return result;
}

#endif

#if !defined(TRK_TARGET_RANGE_SPLIT) || \
    defined(TRK_TARGET_RANGE_800C25FC_800C2A10)

typedef struct TRKMemoryRange {
    const u8* start;
    const u8* end;
    s32 readable;
    s32 writeable;
} TRKMemoryRange;

extern const TRKMemoryRange lbl_8026FAE8[1];

s32 TRKValidMemory32(const void* address, u32 length, s32 write)
{
    s32 result = 0x700;
    const u8* start = address;
    const u8* end = start + length - 1;
    s32 i;

    if (end < start) {
        return 0x700;
    }

    for (i = 0; i < 1; i++) {
        if (start <= lbl_8026FAE8[i].end &&
            end >= lbl_8026FAE8[i].start)
        {
            if ((write == 0 && !lbl_8026FAE8[i].readable) ||
                (write == 1 && !lbl_8026FAE8[i].writeable))
            {
                result = 0x700;
            } else {
                result = 0;
                if (start < lbl_8026FAE8[i].start) {
                    result = TRKValidMemory32(
                        start, lbl_8026FAE8[i].start - start, write);
                }
                if (result == 0 && end > lbl_8026FAE8[i].end) {
                    result = TRKValidMemory32(
                        lbl_8026FAE8[i].end,
                        end - lbl_8026FAE8[i].end, write);
                }
            }
            break;
        }
    }
    return result;
}

#endif
