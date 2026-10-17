/**
 * @file THPAudioDecode.c
 * @brief Dolphin SDK THP player sample: audio decode thread,
 *        0x801E4AC4 - 0x801E4EF0.
 *
 * Built with -inline noauto,deferred, so functions and .bss objects are emitted in
 * reverse source order; the inline helpers below are expanded into their
 * callers and do not survive in the binary.
 */
#include "dolphin/thp/THPPlayer.h"

#define STACK_SIZE 4096
#define BUFFER_COUNT 3

extern u32 THPAudioDecode(s16* audioBuffer, u8* audioFrame, s32 flag);

static OSThread lbl_8046BE78; /* AudioDecodeThread */
static u8 lbl_8046AE78[STACK_SIZE]; /* AudioDecodeThreadStack */
static OSMessageQueue lbl_8046AE58; /* FreeAudioBufferQueue */
static OSMessageQueue lbl_8046AE38; /* DecodedAudioBufferQueue */
static OSMessage lbl_8046AE2C[BUFFER_COUNT]; /* FreeAudioBufferMessage */
static OSMessage lbl_8046AE20[BUFFER_COUNT]; /* DecodedAudioBufferMessage */

static BOOL lbl_8047B480; /* AudioDecodeThreadCreated */

static void* AudioDecoderForOnMemory(void* arg);
static void* AudioDecoder(void* arg);
static inline void AudioDecode(THPReadBuffer* readBuffer);
static inline void* PopFreeAudioBuffer(void);
static inline void PushDecodedAudioBuffer(void* buffer);

BOOL CreateAudioDecodeThread(s32 priority, void* arg)
{
    if (arg) {
        if (OSCreateThread(&lbl_8046BE78, AudioDecoderForOnMemory, arg,
                           lbl_8046AE78 + STACK_SIZE, STACK_SIZE, priority,
                           1) == FALSE) {
            return FALSE;
        }
    } else {
        if (OSCreateThread(&lbl_8046BE78, AudioDecoder, NULL,
                           lbl_8046AE78 + STACK_SIZE, STACK_SIZE, priority,
                           1) == FALSE) {
            return FALSE;
        }
    }

    OSInitMessageQueue(&lbl_8046AE58, lbl_8046AE2C, BUFFER_COUNT);
    OSInitMessageQueue(&lbl_8046AE38, lbl_8046AE20, BUFFER_COUNT);
    lbl_8047B480 = TRUE;
    return TRUE;
}

void AudioDecodeThreadStart(void)
{
    if (lbl_8047B480) {
        OSResumeThread(&lbl_8046BE78);
    }
}

void AudioDecodeThreadCancel(void)
{
    if (lbl_8047B480) {
        OSCancelThread(&lbl_8046BE78);
        lbl_8047B480 = FALSE;
    }
}

/* AudioDecoder */
static void* AudioDecoder(void* arg)
{
    THPReadBuffer* readBuffer;
    u32 curFrame;
    s32 frame = 0;

    while (TRUE) {
        readBuffer = (THPReadBuffer*)fn_801E1BE8();
        AudioDecode(readBuffer);

        if (frame < 2 && (lbl_8046AC60.playFlag & 1) == 0) {
            curFrame = readBuffer->frameNumber + lbl_8046AC60.initReadFrame;
            if (curFrame == lbl_8046AC60.header.numFrames - 1) {
                PrepareReady(TRUE);
            }
        }

        if (frame == 2) {
            PrepareReady(TRUE);
        }

        fn_801E1B54(readBuffer);
        frame++;
    }
}

/* AudioDecoderForOnMemory */
static void* AudioDecoderForOnMemory(void* arg)
{
    THPReadBuffer readBuffer;
    s32 readSize;
    s32 frame;
    u32 remaining;

    frame = 0;
    readSize = lbl_8046AC60.initReadSize;
    readBuffer.ptr = (u8*)arg;

    while (TRUE) {
        readBuffer.frameNumber = frame;
        AudioDecode(&readBuffer);

        remaining = (frame + lbl_8046AC60.initReadFrame) % lbl_8046AC60.header.numFrames;
        if (remaining == lbl_8046AC60.header.numFrames - 1) {
            if (lbl_8046AC60.playFlag & 1) {
                readSize = *(s32*)readBuffer.ptr;
                readBuffer.ptr = lbl_8046AC60.movieData;
            } else {
                if (frame < 2) {
                    PrepareReady(TRUE);
                }
                OSSuspendThread(&lbl_8046BE78);
            }
        } else {
            s32 size = *(s32*)readBuffer.ptr;
            readBuffer.ptr += readSize;
            readSize = size;
        }

        if (frame == 2) {
            PrepareReady(TRUE);
        }
        frame++;
    }
}

static inline void AudioDecode(THPReadBuffer* readBuffer)
{
    THPAudioBuffer* audioBuffer;
    s32 i;
    u32* offsets;
    u8* audioData;

    offsets = (u32*)(readBuffer->ptr + 8);
    audioData = &readBuffer->ptr[lbl_8046AC60.compInfo.numComponents * 4] + 8;
    audioBuffer = (THPAudioBuffer*)PopFreeAudioBuffer();

    for (i = 0; i < lbl_8046AC60.compInfo.numComponents; i++) {
        switch (lbl_8046AC60.compInfo.frameComp[i]) {
        case 1:
            audioBuffer->validSample = THPAudioDecode(
                audioBuffer->buffer, audioData + *offsets * lbl_8046AC60.curAudioTrack, 0);
            audioBuffer->curPtr = audioBuffer->buffer;
            PushDecodedAudioBuffer(audioBuffer);
            return;
        }

        audioData += *offsets;
        offsets++;
    }
}

static inline void* PopFreeAudioBuffer(void)
{
    OSMessage msg;
    OSReceiveMessage(&lbl_8046AE58, &msg, OS_MESSAGE_BLOCK);
    return msg;
}

void PushFreeAudioBuffer(void* buffer)
{
    OSSendMessage(&lbl_8046AE58, buffer, OS_MESSAGE_NOBLOCK);
}

void* PopDecodedAudioBuffer(s32 flags)
{
    OSMessage msg;
    if (OSReceiveMessage(&lbl_8046AE38, &msg, flags) == TRUE) {
        return msg;
    }
    return NULL;
}

static inline void PushDecodedAudioBuffer(void* buffer)
{
    OSSendMessage(&lbl_8046AE38, buffer, OS_MESSAGE_BLOCK);
}
