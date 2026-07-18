#include "dolphin/types.h"

typedef s32 TRKResult;

typedef struct TRKEvent {
    s32 type;
    s32 unused;
    s32 bufferIndex;
} TRKEvent;

typedef struct TRKBuffer {
    s32 mutex;
    s32 inUse;
    s32 position;
    s32 length;
    u8 data[0x880];
} TRKBuffer;

#define kTRKSuccess 0
#define kEventMessage 2

extern void MWTRACE(s32 level, const char* format, ...);
extern u8 lbl_803FE7B8[];
extern void fn_800BE464(TRKEvent* event, s32 type);
extern TRKResult TRKPostEvent(TRKEvent* event);
extern TRKBuffer* TRKGetBuffer(s32 bufferID);
extern TRKResult TRKGetFreeBuffer(s32* bufferID, TRKBuffer** buffer);
extern TRKResult TRKSetBufferPosition(TRKBuffer* buffer, u32 position);
extern TRKResult fn_800C3600(void);
extern TRKResult fn_800C35C4(void* data, u32 length);
extern TRKResult TRKAppendBuffer_ui8(TRKBuffer* buffer, u8* data, s32 length);
extern void TRKReleaseBuffer(s32 bufferID);
extern s32 TRKTestForPacket(void);

s32 TRKTestForPacket(void)
{
    u8 payload[0x880];
    u8 packet[0x40];
    s32 bufferID;
    TRKBuffer* buffer;
    s32 result;

    if (fn_800C3600() <= 0) {
        return -1;
    }

    result = TRKGetFreeBuffer(&bufferID, &buffer);

    MWTRACE(4, "TestForPacket : FreeBuffer is  %ld\n", result);

    TRKSetBufferPosition(buffer, 0);
    if (fn_800C35C4(packet, 0x40) == kTRKSuccess) {
        s32 payloadSize;

        TRKAppendBuffer_ui8(buffer, packet, 0x40);
        payloadSize = ((u32*)packet)[0] - 0x40;
        result = bufferID;
        if (payloadSize > 0) {
            MWTRACE(1, "Reading payload %ld bytes\n", payloadSize);
            if (fn_800C35C4(payload, ((u32*)packet)[0] - 0x40) == kTRKSuccess) {
                TRKAppendBuffer_ui8(buffer, payload, ((u32*)packet)[0]);
            } else {
                MWTRACE(8, "TestForPacket : Invalid size of packet hdr.size\n");
                TRKReleaseBuffer(result);
                result = -1;
            }
        }
    } else {
        MWTRACE(8, "TestForPacket : Invalid size of packet\n");
        TRKReleaseBuffer(result);
        result = -1;
    }

    MWTRACE(1, "TestForPacket returning %ld\n", result);
    return result;
}

void TRKGetInput(void)
{
    s32 bufferID = TRKTestForPacket();

    if (bufferID != -1) {
        TRKEvent event;

        TRKGetBuffer(bufferID);
        fn_800BE464(&event, kEventMessage);
        event.bufferIndex = bufferID;
        ((s32*)lbl_803FE7B8)[0] = -1;
        TRKPostEvent(&event);
    }
}

void TRKProcessInput(s32 bufferID)
{
    TRKEvent event;

    fn_800BE464(&event, kEventMessage);
    event.bufferIndex = bufferID;
    ((s32*)lbl_803FE7B8)[0] = -1;
    TRKPostEvent(&event);
}

TRKResult TRKInitializeSerialHandler(void)
{
    u8* state = lbl_803FE7B8;

    ((s32*)state)[0] = -1;
    ((s32*)state)[2] = 0;
    ((s32*)state)[3] = 0;

    MWTRACE(1, "TRK_Packet_Header \t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReadMemory     %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_WriteMemory    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_Connect \t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReplyAck\t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReadRegisters\t%ld bytes\n", 0x40);

    return kTRKSuccess;
}
