/**
 * @file THPRead.c
 * @brief Dolphin SDK THP player sample: DVD read thread, 0x801E1B54 - 0x801E1E1C.
 *
 * Built with -inline noauto,deferred: functions and .bss objects are emitted
 * in reverse source order. PopFreeReadBuffer and PushReadedBuffer are only
 * called by the reader thread (Reader), where they are expanded; they
 * have no copy of their own in the binary.
 */
#include "dolphin/thp/THPPlayer.h"

#define STACK_SIZE 4096
#define BUFFER_COUNT 10

static OSMessageQueue lbl_8046A410; /* FreeReadBufferQueue */
static OSMessageQueue lbl_8046A3F0; /* ReadedBufferQueue */
static OSMessageQueue lbl_8046A3D0; /* ReadedBufferQueue2 */
static OSMessage lbl_8046A3A8[BUFFER_COUNT]; /* FreeReadBufferMessage */
static OSMessage lbl_8046A380[BUFFER_COUNT]; /* ReadedBufferMessage */
static OSMessage lbl_8046A358[BUFFER_COUNT]; /* ReadedBufferMessage2 */
static OSThread lbl_8046A040; /* ReadThread */
static u8 lbl_80469040[STACK_SIZE]; /* ReadThreadStack */
static BOOL lbl_8047B460; /* ReadThreadCreated */

static void* Reader(void* arg);
static inline void* PopFreeReadBuffer(void);
static inline void PushReadedBuffer(void* buffer);

BOOL CreateReadThread(s32 priority)
{
    if (OSCreateThread(&lbl_8046A040, Reader, NULL, lbl_80469040 + STACK_SIZE,
                       STACK_SIZE, priority, 1) == FALSE) {
        return FALSE;
    }

    OSInitMessageQueue(&lbl_8046A410, lbl_8046A3A8, BUFFER_COUNT);
    OSInitMessageQueue(&lbl_8046A3F0, lbl_8046A380, BUFFER_COUNT);
    OSInitMessageQueue(&lbl_8046A3D0, lbl_8046A358, BUFFER_COUNT);
    lbl_8047B460 = TRUE;
    return TRUE;
}

void ReadThreadStart(void)
{
    if (lbl_8047B460) {
        OSResumeThread(&lbl_8046A040);
    }
}

void ReadThreadCancel(void)
{
    if (lbl_8047B460) {
        OSCancelThread(&lbl_8046A040);
        lbl_8047B460 = FALSE;
    }
}

static void* Reader(void* arg)
{
    THPReadBuffer* readBuffer;
    s32 offset;
    s32 size;
    s32 readFrame;
    s32 result;
    u32 frameNumber;

    readFrame = 0;
    offset = lbl_8046AC60.initOffset;
    size = lbl_8046AC60.initReadSize;

    while (TRUE) {
        readBuffer = (THPReadBuffer*)PopFreeReadBuffer();

        result = DVDRead(&lbl_8046AC60.fileInfo, readBuffer->ptr, size, offset, 2);
        if (result != size) {
            if (result == -1) {
                lbl_8046AC60.dvdError = -1;
            }
            if (readFrame == 0) {
                PrepareReady(FALSE);
            }
            OSSuspendThread(&lbl_8046A040);
        }

        readBuffer->frameNumber = readFrame;
        PushReadedBuffer(readBuffer);

        offset += size;
        size = *(s32*)readBuffer->ptr;

        frameNumber = (readFrame + lbl_8046AC60.initReadFrame) % lbl_8046AC60.header.numFrames;
        if (frameNumber == lbl_8046AC60.header.numFrames - 1) {
            if (lbl_8046AC60.playFlag & 1) {
                offset = lbl_8046AC60.header.movieDataOffsets;
            } else {
                OSSuspendThread(&lbl_8046A040);
            }
        }

        readFrame++;
    }
}

void* fn_801E1BE8(void)
{
    OSMessage msg;
    OSReceiveMessage(&lbl_8046A3F0, &msg, OS_MESSAGE_BLOCK);
    return msg;
}

static inline void PushReadedBuffer(void* buffer)
{
    OSSendMessage(&lbl_8046A3F0, buffer, OS_MESSAGE_BLOCK);
}

static inline void* PopFreeReadBuffer(void)
{
    OSMessage msg;
    OSReceiveMessage(&lbl_8046A410, &msg, OS_MESSAGE_BLOCK);
    return msg;
}

void PushFreeReadBuffer(void* buffer)
{
    OSSendMessage(&lbl_8046A410, buffer, OS_MESSAGE_BLOCK);
}

void* fn_801E1B84(void)
{
    OSMessage msg;
    OSReceiveMessage(&lbl_8046A3D0, &msg, OS_MESSAGE_BLOCK);
    return msg;
}

BOOL fn_801E1B54(void* buffer)
{
    return OSSendMessage(&lbl_8046A3D0, buffer, OS_MESSAGE_BLOCK);
}
