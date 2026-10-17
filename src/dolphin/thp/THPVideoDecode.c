/**
 * @file THPVideoDecode.c
 * @brief Dolphin SDK THP player sample: video decode thread,
 *        0x801E4EF0 - 0x801E5548.
 *
 * Built with -inline noauto,deferred, so functions and .bss objects are emitted in
 * reverse source order; the inline helpers below are expanded into their
 * callers and do not survive in the binary.
 */
#include "dolphin/thp/THPPlayer.h"

#define STACK_SIZE 4096
#define BUFFER_COUNT 3

extern s32 THPVideoDecode(void* file, void* tileY, void* tileU, void* tileV, void* work);
extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);

static OSThread lbl_8046D1E8; /* VideoDecodeThread */
static u8 lbl_8046C1E8[STACK_SIZE]; /* VideoDecodeThreadStack */
static OSMessageQueue lbl_8046C1C8; /* FreeTextureSetQueue */
static OSMessageQueue lbl_8046C1A8; /* DecodedTextureSetQueue */
static OSMessage lbl_8046C19C[BUFFER_COUNT]; /* FreeTextureSetMessage */
static OSMessage lbl_8046C190[BUFFER_COUNT]; /* DecodedTextureSetMessage */

static BOOL lbl_8047B48C; /* First */
static BOOL lbl_8047B488; /* VideoDecodeThreadCreated */

static void* VideoDecoderForOnMemory(void* arg);
static void* VideoDecoder(void* arg);
static inline void VideoDecode(THPReadBuffer* readBuffer);
static inline void* PopFreeTextureSet(void);
static inline void PushDecodedTextureSet(void* tex);

BOOL CreateVideoDecodeThread(s32 priority, void* arg)
{
    if (arg) {
        if (OSCreateThread(&lbl_8046D1E8, VideoDecoderForOnMemory, arg,
                           lbl_8046C1E8 + STACK_SIZE, STACK_SIZE, priority,
                           1) == FALSE) {
            return FALSE;
        }
    } else {
        if (OSCreateThread(&lbl_8046D1E8, VideoDecoder, NULL,
                           lbl_8046C1E8 + STACK_SIZE, STACK_SIZE, priority,
                           1) == FALSE) {
            return FALSE;
        }
    }

    OSInitMessageQueue(&lbl_8046C1C8, lbl_8046C19C, BUFFER_COUNT);
    OSInitMessageQueue(&lbl_8046C1A8, lbl_8046C190, BUFFER_COUNT);
    lbl_8047B488 = TRUE;
    lbl_8047B48C = TRUE;
    return TRUE;
}

void VideoDecodeThreadStart(void)
{
    if (lbl_8047B488) {
        OSResumeThread(&lbl_8046D1E8);
    }
}

void VideoDecodeThreadCancel(void)
{
    if (lbl_8047B488) {
        OSCancelThread(&lbl_8046D1E8);
        lbl_8047B488 = FALSE;
    }
}

/* VideoDecoder */
static void* VideoDecoder(void* arg)
{
    THPReadBuffer* readBuffer;
    u32 curFrame;
    BOOL level;

    while (TRUE) {
        if (lbl_8046AC60.audioExist) {
            while (lbl_8046AC60.videoDecodeCount < 0) {
                readBuffer = (THPReadBuffer*)fn_801E1B84();
                curFrame = (readBuffer->frameNumber + lbl_8046AC60.initReadFrame) %
                           lbl_8046AC60.header.numFrames;
                if (curFrame == lbl_8046AC60.header.numFrames - 1 &&
                    (lbl_8046AC60.playFlag & 1) == 0) {
                    VideoDecode(readBuffer);
                }

                PushFreeReadBuffer(readBuffer);
                level = OSDisableInterrupts();
                lbl_8046AC60.videoDecodeCount++;
                OSRestoreInterrupts(level);
            }
        }

        if (lbl_8046AC60.audioExist) {
            readBuffer = (THPReadBuffer*)fn_801E1B84();
        } else {
            readBuffer = (THPReadBuffer*)fn_801E1BE8();
        }

        VideoDecode(readBuffer);
        PushFreeReadBuffer(readBuffer);
    }
}

/* VideoDecoderForOnMemory */
static void* VideoDecoderForOnMemory(void* arg)
{
    THPReadBuffer readBuffer;
    s32 readSize;
    s32 frame;
    u32 remaining;
    BOOL level;

    readSize = lbl_8046AC60.initReadSize;
    frame = 0;
    readBuffer.ptr = (u8*)arg;

    while (TRUE) {
        if (lbl_8046AC60.audioExist) {
            while (lbl_8046AC60.videoDecodeCount < 0) {
                level = OSDisableInterrupts();
                lbl_8046AC60.videoDecodeCount++;
                OSRestoreInterrupts(level);

                remaining = (frame + lbl_8046AC60.initReadFrame) % lbl_8046AC60.header.numFrames;
                if (remaining == lbl_8046AC60.header.numFrames - 1) {
                    if ((lbl_8046AC60.playFlag & 1) == 0) {
                        break;
                    }
                    readSize = *(s32*)readBuffer.ptr;
                    readBuffer.ptr = lbl_8046AC60.movieData;
                } else {
                    s32 size = *(s32*)readBuffer.ptr;
                    readBuffer.ptr += readSize;
                    readSize = size;
                }
                frame++;
            }
        }

        readBuffer.frameNumber = frame;
        VideoDecode(&readBuffer);

        remaining = (frame + lbl_8046AC60.initReadFrame) % lbl_8046AC60.header.numFrames;
        if (remaining == lbl_8046AC60.header.numFrames - 1) {
            if (lbl_8046AC60.playFlag & 1) {
                readSize = *(s32*)readBuffer.ptr;
                readBuffer.ptr = lbl_8046AC60.movieData;
            } else {
                OSSuspendThread(&lbl_8046D1E8);
            }
        } else {
            s32 size = *(s32*)readBuffer.ptr;
            readBuffer.ptr += readSize;
            readSize = size;
        }

        frame++;
    }
}

static inline void VideoDecode(THPReadBuffer* readBuffer)
{
    THPTextureSet* textureSet;
    u32 i;
    u32* tileOffsets;
    u8* tile;
    BOOL level;

    tileOffsets = (u32*)(readBuffer->ptr + 8);
    tile = &readBuffer->ptr[lbl_8046AC60.compInfo.numComponents * 4] + 8;
    textureSet = (THPTextureSet*)PopFreeTextureSet();

    for (i = 0; i < lbl_8046AC60.compInfo.numComponents; i++) {
        switch (lbl_8046AC60.compInfo.frameComp[i]) {
        case 0:
            if ((lbl_8046AC60.videoError =
                     THPVideoDecode(tile, textureSet->ytexture, textureSet->utexture,
                                    textureSet->vtexture, lbl_8046AC60.thpWork))) {
                if (lbl_8047B48C) {
                    PrepareReady(FALSE);
                    lbl_8047B48C = FALSE;
                }
                OSSuspendThread(&lbl_8046D1E8);
            }
            textureSet->frameNumber = readBuffer->frameNumber;
            PushDecodedTextureSet(textureSet);
            level = OSDisableInterrupts();
            lbl_8046AC60.videoDecodeCount++;
            OSRestoreInterrupts(level);
        }

        tile += *tileOffsets;
        tileOffsets++;
    }

    if (lbl_8047B48C) {
        PrepareReady(TRUE);
        lbl_8047B48C = FALSE;
    }
}

static inline void* PopFreeTextureSet(void)
{
    OSMessage tex;
    OSReceiveMessage(&lbl_8046C1C8, &tex, OS_MESSAGE_BLOCK);
    return tex;
}

void PushFreeTextureSet(void* tex)
{
    OSSendMessage(&lbl_8046C1C8, tex, OS_MESSAGE_NOBLOCK);
}

void* PopDecodedTextureSet(s32 flags)
{
    OSMessage tex;
    if (OSReceiveMessage(&lbl_8046C1A8, &tex, flags) == TRUE) {
        return tex;
    }
    return NULL;
}

static inline void PushDecodedTextureSet(void* tex)
{
    OSSendMessage(&lbl_8046C1A8, tex, OS_MESSAGE_BLOCK);
}
