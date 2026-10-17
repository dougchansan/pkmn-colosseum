#include "dolphin/types.h"

typedef struct TRKBuffer {
    s32 mutex;
    s32 inUse;
    u32 position;
    u32 length;
    u8 data[0x880];
} TRKBuffer;

typedef struct TRKEvent {
    s32 type;
    s32 id;
    s32 bufferIndex;
} TRKEvent;

typedef struct TRKReply {
    u32 length;
    u8 command;
    u8 pad_05[3];
    u8 error;
    u8 pad_09[0x37];
} TRKReply;

extern s32 lbl_803FE7D0[3];

extern void* memset(void* destination, s32 value, u32 size);
extern s32 TRKWriteUARTN(void* message, u32 length);
extern void MWTRACE(s32 level, const char* format, ...);
extern s32 TRKTargetStop(void);
extern s32 TRKTargetStopped(void);
extern s32 TRKTargetContinue(void);
extern void TRKSetBufferPosition(TRKBuffer* buffer, u32 position);
extern void TRKResetBuffer(TRKBuffer* buffer, s32 keepData);
extern s32 TRKAppendBuffer(TRKBuffer* buffer, const void* data, u32 length);
extern s32 TRKAppendBuffer_ui8(TRKBuffer* buffer, const void* data, u32 length);
extern s32 TRKReadBuffer(TRKBuffer* buffer, void* data, u32 length);
extern s32 TRKTargetGetPC(void);
extern s32 TRKTargetSingleStep(u8 count, s32 stepOver);
extern s32 TRKTargetStepOutOfRange(u32 start, u32 end, s32 stepOver);
extern s32 TRKTargetAccessDefault(u16 first, u16 last, TRKBuffer* buffer,
                                  u32* length, s32 read);
extern s32 TRKTargetAccessFP(u16 first, u16 last, TRKBuffer* buffer,
                             u32* length, s32 read);
extern s32 TRKTargetAccessExtended1(u16 first, u16 last, TRKBuffer* buffer,
                                    u32* length, s32 read);
extern s32 TRKTargetAccessExtended2(u16 first, u16 last, TRKBuffer* buffer,
                                    u32* length, s32 read);
extern s32 TRKTargetAccessARAM(void* data, u32 address, u32* length, s32 read);
extern s32 TRKTargetAccessMemory(void* data, u32 address, u32* length,
                                 s32 debuggerMemory, s32 read);
extern s32 MessageSend(TRKBuffer* buffer);
extern void fn_800053E0(void);
extern void __TRK_copy_vectors(void);
extern void TRKConstructEvent(TRKEvent* event, s32 type);
extern s32 TRKPostEvent(TRKEvent* event);
extern s32 usr_puts_serial(char* text);
extern void TRKCommSetState(u8 enabled);

static inline s32 TRKStandardACK(TRKBuffer* buffer, u8 error)
{
    TRKReply reply;

    memset(&reply, 0, sizeof(reply));
    reply.command = 0x80;
    reply.length = sizeof(reply);
    reply.error = error;
    TRKWriteUARTN(&reply, sizeof(reply));
    return 0;
}

static inline s32 TRKSendACK(TRKBuffer* buffer)
{
    s32 error;

    MWTRACE(1, "SendACK : Calling MessageSend\n");
    error = MessageSend(buffer);
    MWTRACE(1, "MessageSend err : %ld\n", error);
    return error;
}

s32 TRKDoSetOption(TRKBuffer* buffer)
{
    u8 enabled = buffer->data[12];

    if (buffer->data[8] == 1) {
        usr_puts_serial("\nMetroTRK Option : SerialIO - ");
        if (enabled != 0) {
            usr_puts_serial("Enable\n");
        } else {
            usr_puts_serial("Disable\n");
        }
        TRKCommSetState(enabled);
    }
    TRKStandardACK(buffer, 0);
    return 0;
}

s32 TRKDoStop(TRKBuffer* buffer)
{
    u8 error;

    switch (TRKTargetStop()) {
    case 0:
        error = 0;
        break;
    case 0x704:
        error = 0x21;
        break;
    case 0x705:
        error = 0x22;
        break;
    case 0x706:
        error = 0x20;
        break;
    default:
        error = 1;
        break;
    }
    return TRKStandardACK(buffer, error);
}

s32 TRKDoStep(TRKBuffer* buffer)
{
    s32 result;
    u8 options;
    u8 count;
    u32 rangeStart;
    u32 rangeEnd;
    u32 pc;

    TRKSetBufferPosition(buffer, 0);
    options = buffer->data[8];
    rangeStart = *(u32*) &buffer->data[16];
    rangeEnd = *(u32*) &buffer->data[20];

    switch (options) {
    case 0:
    case 0x10:
        count = buffer->data[12];
        if (count < 1) {
            return TRKStandardACK(buffer, 0x11);
        }
        break;
    case 1:
    case 0x11:
        pc = TRKTargetGetPC();
        if (pc < rangeStart || pc > rangeEnd) {
            return TRKStandardACK(buffer, 0x11);
        }
        break;
    default:
        return TRKStandardACK(buffer, 0x12);
    }

    if (TRKTargetStopped() == 0) {
        return TRKStandardACK(buffer, 0x16);
    }

    result = TRKStandardACK(buffer, 0);
    switch (options) {
    case 0:
    case 0x10:
        result = TRKTargetSingleStep(count, options == 0x10);
        break;
    case 1:
    case 0x11:
        result =
            TRKTargetStepOutOfRange(rangeStart, rangeEnd, options == 0x11);
        break;
    }
    return result;
}

s32 TRKDoContinue(TRKBuffer* buffer)
{
    MWTRACE(1, "DoContinue\n");
    if (TRKTargetStopped() == 0) {
        TRKStandardACK(buffer, 0x16);
        return 0;
    }

    TRKStandardACK(buffer, 0);
    return TRKTargetContinue();
}

/* Unreferenced in the game, so the linker strips it; its message stays in
 * the string pool. */
s32 TRKDoFlushCache(TRKBuffer* buffer)
{
    MWTRACE(1, "DoFlushCache unimplemented!!!\n");
    TRKStandardACK(buffer, 0x11);
    return 0;
}

s32 SendACK(TRKBuffer* buffer)
{
    s32 error;
    s32 replyError;
    u8 options;
    u16 firstRegister;
    u16 lastRegister;
    u32 registersLength;

    options = buffer->data[8];
    firstRegister = *(u16*) &buffer->data[12];
    lastRegister = *(u16*) &buffer->data[16];
    TRKSetBufferPosition(buffer, 0);

    if (firstRegister > lastRegister) {
        return TRKStandardACK(buffer, 0x14);
    }
    TRKSetBufferPosition(buffer, 0x40);
    switch (options) {
    case 0:
        error = TRKTargetAccessDefault(firstRegister, lastRegister, buffer,
                                       &registersLength, FALSE);
        break;
    case 1:
        error = TRKTargetAccessFP(firstRegister, lastRegister, buffer,
                                  &registersLength, FALSE);
        break;
    case 2:
        error = TRKTargetAccessExtended1(firstRegister, lastRegister, buffer,
                                         &registersLength, FALSE);
        break;
    case 3:
        error = TRKTargetAccessExtended2(firstRegister, lastRegister, buffer,
                                         &registersLength, FALSE);
        break;
    default:
        error = 0x703;
        break;
    }
    TRKResetBuffer(buffer, 0);

    if (error == 0) {
        TRKReply reply;

        memset(&reply, 0, sizeof(reply));
        reply.length = 0x40;
        reply.command = 0x80;
        reply.error = error;
        error = TRKAppendBuffer(buffer, &reply, sizeof(reply));
    }
    if (error != 0) {
        switch (error) {
        case 0x703:
            replyError = 0x12;
            break;
        case 0x701:
            replyError = 0x14;
            break;
        case 0x302:
            replyError = 2;
            break;
        case 0x702:
            replyError = 0x15;
            break;
        case 0x704:
            replyError = 0x21;
            break;
        case 0x705:
            replyError = 0x22;
            break;
        case 0x706:
            replyError = 0x20;
            break;
        default:
            replyError = 3;
            break;
        }
        return TRKStandardACK(buffer, replyError);
    }
    return TRKSendACK(buffer);
}

s32 TRKDoReadRegisters(TRKBuffer* buffer)
{
    s32 error;
    s32 replyError;
    u16 firstRegister;
    u16 lastRegister;
    u32 registersLength;
    TRKReply reply;

    firstRegister = *(u16*) &buffer->data[12];
    lastRegister = *(u16*) &buffer->data[16];
    if (firstRegister > lastRegister) {
        return TRKStandardACK(buffer, 0x14);
    }

    reply.command = 0x80;
    reply.length = 0x468;
    TRKResetBuffer(buffer, 0);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->position);
    TRKAppendBuffer_ui8(buffer, &reply, sizeof(reply));
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->position);
    error = TRKTargetAccessDefault(0, 36, buffer, &registersLength, TRUE);
    MWTRACE(4, "DoReadRegisters : Error reading  default regs 0x%08x\n", error);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->position);
    if (error == 0) {
        error = TRKTargetAccessFP(0, 33, buffer, &registersLength, TRUE);
    }
    MWTRACE(4, "DoReadRegisters : Error FP regs 0x%08x\n", error);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->position);
    if (error == 0) {
        error = TRKTargetAccessExtended1(0, 0x60, buffer,
                                         &registersLength, TRUE);
    }
    MWTRACE(4, "DoReadRegisters : Error extended1 regs 0x%08x\n", error);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->position);
    if (error == 0) {
        error =
            TRKTargetAccessExtended2(0, 31, buffer, &registersLength, TRUE);
    }
    MWTRACE(4, "DoReadRegisters : Error extended2 regs 0x%08x\n", error);
    MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->position);

    if (error != 0) {
        switch (error) {
        case 0x703:
            replyError = 0x12;
            break;
        case 0x701:
            replyError = 0x14;
            break;
        case 0x702:
            replyError = 0x15;
            break;
        case 0x704:
            replyError = 0x21;
            break;
        case 0x705:
            replyError = 0x22;
            break;
        case 0x706:
            replyError = 0x20;
            break;
        default:
            replyError = 3;
            break;
        }
        return TRKStandardACK(buffer, replyError);
    }
    return TRKSendACK(buffer);
}

s32 TRKDoWriteMemory(TRKBuffer* buffer)
{
    u8 data[0x820] __attribute__((aligned(32)));
    u32 accessLength;
    s32 options;
    s32 error;
    s32 replyError;
    u32 length;
    u32 start;

    start = *(u32*) &buffer->data[16];
    length = *(u16*) &buffer->data[12];
    options = buffer->data[8];
    MWTRACE(1, "WriteMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", buffer->data[4], start, length,
            options);

    if ((options & 2) != 0) {
        return TRKStandardACK(buffer, 0x12);
    }
    accessLength = length;
    TRKSetBufferPosition(buffer, 0x40);
    if ((options & 0x40) != 0) {
        TRKReadBuffer(buffer, data + (start & 0x1F), accessLength);
        error =
            TRKTargetAccessARAM(data, start, &accessLength, FALSE);
    } else {
        TRKReadBuffer(buffer, data, accessLength);
        error = TRKTargetAccessMemory(data, start, &accessLength,
                                      (options & 8) != 0 ? 0 : 1, FALSE);
    }
    TRKResetBuffer(buffer, 0);
    if (error == 0) {
        TRKReply reply;

        memset(&reply, 0, sizeof(reply));
        reply.length = 0x40;
        reply.command = 0x80;
        reply.error = error;
        error = TRKAppendBuffer(buffer, &reply, sizeof(reply));
    }
    if (error != 0) {
        switch (error) {
        case 0x702: replyError = 0x15; break;
        case 0x700: replyError = 0x13; break;
        case 0x704: replyError = 0x21; break;
        case 0x705: replyError = 0x22; break;
        case 0x706: replyError = 0x20; break;
        default: replyError = 3; break;
        }
        return TRKStandardACK(buffer, replyError);
    }
    return TRKSendACK(buffer);
}

s32 TRKDoReadMemory(TRKBuffer* buffer)
{
    u8 data[0x820] __attribute__((aligned(32)));
    u32 accessLength;
    int error;
    s32 replyError;
    s32 options;
    u32 length;
    u32 start;

    start = *(u32*) &buffer->data[16];
    length = *(u16*) &buffer->data[12];
    options = buffer->data[8];
    MWTRACE(1, "ReadMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", buffer->data[4], start, length,
            options);
    if ((options & 2) != 0) {
        return TRKStandardACK(buffer, 0x12);
    }
    accessLength = length;
    if ((options & 0x40) != 0) {
        error = TRKTargetAccessARAM(data, start, &accessLength, TRUE);
    } else {
        error = TRKTargetAccessMemory(data, start, &accessLength,
                                      (options & 8) != 0 ? 0 : 1, TRUE);
    }
    TRKResetBuffer(buffer, 0);
    if (error == 0) {
        TRKReply reply;

        memset(&reply, 0, sizeof(reply));
        reply.error = error;
        reply.length = accessLength + 0x40;
        reply.command = 0x80;
        TRKAppendBuffer(buffer, &reply, sizeof(reply));
        if ((options & 0x40) != 0) {
            error = TRKAppendBuffer(buffer, data + (start & 0x1F),
                                    accessLength);
        } else {
            error = TRKAppendBuffer(buffer, data, accessLength);
        }
    }
    if (error != 0) {
        switch (error) {
        case 0x702: replyError = 0x15; break;
        case 0x700: replyError = 0x13; break;
        case 0x704: replyError = 0x21; break;
        case 0x705: replyError = 0x22; break;
        case 0x706: replyError = 0x20; break;
        default: replyError = 3; break;
        }
        return TRKStandardACK(buffer, replyError);
    }
    return TRKSendACK(buffer);
}

s32 TRKDoSupportMask(TRKBuffer* buffer)
{
    return 0;
}

s32 TRKDoVersions(TRKBuffer* buffer)
{
    return 0;
}

s32 TRKDoOverride(TRKBuffer* buffer)
{
    TRKStandardACK(buffer, 0);
    __TRK_copy_vectors();
    return 0;
}

s32 TRKDoReset(TRKBuffer* buffer)
{
    TRKStandardACK(buffer, 0);
    fn_800053E0();
    return 0;
}

s32 TRKDoDisconnect(TRKBuffer* buffer)
{
    TRKEvent event;

    lbl_803FE7D0[0] = 0;
    TRKStandardACK(buffer, 0);
    TRKConstructEvent(&event, 1);
    TRKPostEvent(&event);
    return 0;
}

s32 TRKDoConnect(TRKBuffer* buffer)
{
    lbl_803FE7D0[0] = 1;
    return TRKStandardACK(buffer, 0);
}
