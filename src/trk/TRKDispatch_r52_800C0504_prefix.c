#include "dolphin/types.h"

extern char lbl_8026FA34[];
extern char lbl_8026FA3C[];
extern char lbl_8026FA40[];
extern void MWTRACE(s32 level, const char* format, ...);
extern void* memset(void* destination, s32 value, u32 size);
extern u32 strlen(const char* string);

typedef struct TRKBuffer {
    s32 mutex;
    s32 inUse;
    u32 position;
    u32 length;
    u8 data[0x880];
} TRKBuffer;

typedef struct TRKReply {
    u32 length;
    u8 command;
    u8 pad_05[3];
    union {
        u32 word;
        u8 byte;
    } error;
    u8 pad_0C[4];
    u8 data_10[0x30];
} TRKReply;

extern s32 TRKGetFreeBuffer(s32* id, TRKBuffer** buffer);
extern TRKBuffer* TRKGetBuffer(s32 id);
extern s32 TRKReleaseBuffer(s32 id);
extern s32 TRKAppendBuffer_ui8(TRKBuffer* buffer, const void* data, u32 length);
extern void TRKSetBufferPosition(TRKBuffer* buffer, u32 position);
extern s32 TRKTestForPacket(void);
extern void TRKProcessInput(s32 bufferId);
extern s32 TRKRequestSend(TRKBuffer* message, s32* bufferId, u32 retries,
                          u32 attempts, s32 wait);

void OutputData(void* data, s32 length)
{
    s32 i;
    u8* current = data;

    for (i = 0; i < length; i++) {
        MWTRACE(8, lbl_8026FA34, current[i]);
        if (i % 16 == 15) {
            MWTRACE(8, lbl_8026FA3C);
        }
    }
    MWTRACE(8, lbl_8026FA3C);
}

s32 HandlePositionFileSupportRequest(u32 replyError, u32* position,
                                     u8 mode, u32* ioResult)
{
    s32 error;
    s32 replyBufferId;
    s32 bufferId;
    TRKBuffer* buffer;
    TRKBuffer* replyBuffer;
    TRKReply reply;

    memset(&reply, 0, sizeof(reply));
    reply.command = 0xD4;
    reply.length = 0x40;
    reply.error.word = replyError;
    *(u32*) &reply.pad_0C[0] = *position;
    reply.data_10[0] = mode;
    error = TRKGetFreeBuffer(&bufferId, &buffer);
    if (error == 0) {
        error = TRKAppendBuffer_ui8(buffer, &reply, sizeof(reply));
    }
    if (error == 0) {
        *ioResult = 0;
        *position = -1;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            if (replyBuffer != NULL) {
                *ioResult = *(u32*) &replyBuffer->data[16];
                *position = *(u32*) &replyBuffer->data[24];
            }
        }
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}

s32 HandleCloseFileSupportRequest(s32 fileHandle, u32* ioResult)
{
    s32 error;
    s32 replyBufferId;
    s32 bufferId;
    TRKBuffer* buffer;
    TRKBuffer* replyBuffer;
    TRKReply reply;

    memset(&reply, 0, sizeof(reply));
    reply.command = 0xD3;
    reply.length = 0x40;
    reply.error.word = fileHandle;
    error = TRKGetFreeBuffer(&bufferId, &buffer);
    if (error == 0) {
        error = TRKAppendBuffer_ui8(buffer, &reply, sizeof(reply));
    }
    if (error == 0) {
        *ioResult = 0;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
        }
        if (error == 0) {
            *ioResult = *(u32*) &replyBuffer->data[16];
        }
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}

s32 HandleOpenFileSupportRequest(const char* path, u8 replyError,
                                 u32* fileHandle, u32* ioResult)
{
    s32 error;
    s32 replyBufferId;
    s32 bufferId;
    TRKBuffer* buffer;
    TRKBuffer* replyBuffer;
    TRKReply reply;

    memset(&reply, 0, sizeof(reply));
    *fileHandle = 0;
    reply.command = 0xD2;
    reply.length = strlen(path) + 0x41;
    reply.error.byte = replyError;
    *(u16*) &reply.pad_0C[0] = strlen(path) + 1;
    TRKGetFreeBuffer(&bufferId, &buffer);
    error = TRKAppendBuffer_ui8(buffer, &reply, sizeof(reply));
    if (error == 0) {
        error = TRKAppendBuffer_ui8(buffer, path, strlen(path) + 1);
    }
    if (error == 0) {
        *ioResult = 0;
        error = TRKRequestSend(buffer, &replyBufferId, 7, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
        }
        *ioResult = *(u32*) &replyBuffer->data[16];
        *fileHandle = *(u32*) &replyBuffer->data[8];
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}
