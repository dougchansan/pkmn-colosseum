#ifndef DOLPHIN_THP_THPPLAYER_H
#define DOLPHIN_THP_THPPLAYER_H

/*
 * Dolphin SDK THP player sample (THPPlayer.c, THPRead.c, THPAudioDecode.c,
 * THPVideoDecode.c, THPDraw.c) as built into Pokemon Colosseum.
 *
 * Layout evidence: the sample's structures as matched in doldecomp/sms
 * (libs/THPPlayer), minus the volume-ramp fields that this build drops
 * (curAudioTrack sits at 0xDC here, see the audio decoder's frame indexing).
 */

#include "dolphin/types.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/dvd/dvd.h"

typedef void* OSMessage;

typedef struct OSMessageQueue {
    /* 0x00 */ OSThreadQueue queueSend;
    /* 0x08 */ OSThreadQueue queueReceive;
    /* 0x10 */ OSMessage* msgArray;
    /* 0x14 */ s32 msgCount;
    /* 0x18 */ s32 firstIndex;
    /* 0x1C */ s32 usedCount;
} OSMessageQueue;

#ifndef OSRoundUp32B
#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#endif

#define OS_MESSAGE_NOBLOCK 0
#define OS_MESSAGE_BLOCK   1

/* OSInitMessageQueue / OSSendMessage / OSReceiveMessage (src/dolphin/os/OSMemory.c) */
void OSInitMessageQueue(OSMessageQueue* mq, OSMessage* msgArray, s32 msgCount);
BOOL OSSendMessage(OSMessageQueue* mq, OSMessage msg, s32 flags);
BOOL OSReceiveMessage(OSMessageQueue* mq, OSMessage* msg, s32 flags);

typedef struct THPTextureSet {
    /* 0x0 */ u8* ytexture;
    /* 0x4 */ u8* utexture;
    /* 0x8 */ u8* vtexture;
    /* 0xC */ s32 frameNumber;
} THPTextureSet;

typedef struct THPAudioBuffer {
    /* 0x0 */ s16* buffer;
    /* 0x4 */ s16* curPtr;
    /* 0x8 */ u32 validSample;
} THPAudioBuffer;

typedef struct THPReadBuffer {
    /* 0x0 */ u8* ptr;
    /* 0x4 */ s32 frameNumber;
    /* 0x8 */ BOOL isValid;
} THPReadBuffer;

typedef struct THPHeader {
    /* 0x00 */ char magic[4];
    /* 0x04 */ u32 version;
    /* 0x08 */ u32 bufSize;
    /* 0x0C */ u32 audioMaxSamples;
    /* 0x10 */ f32 frameRate;
    /* 0x14 */ u32 numFrames;
    /* 0x18 */ u32 firstFrameSize;
    /* 0x1C */ u32 movieDataSize;
    /* 0x20 */ u32 compInfoDataOffsets;
    /* 0x24 */ u32 offsetDataOffsets;
    /* 0x28 */ u32 movieDataOffsets;
    /* 0x2C */ u32 finalFrameDataOffsets;
} THPHeader;

typedef struct THPFrameCompInfo {
    /* 0x00 */ u32 numComponents;
    /* 0x04 */ u8 frameComp[16];
} THPFrameCompInfo;

typedef struct THPVideoInfo {
    /* 0x0 */ u32 xSize;
    /* 0x4 */ u32 ySize;
    /* 0x8 */ u32 videoType;
} THPVideoInfo;

typedef struct THPAudioInfo {
    /* 0x0 */ u32 sndChannels;
    /* 0x4 */ u32 sndFrequency;
    /* 0x8 */ u32 sndNumSamples;
    /* 0xC */ u32 sndNumTracks;
} THPAudioInfo;

typedef struct THPPlayer {
    /* 0x000 */ DVDFileInfo fileInfo;
    /* 0x03C */ THPHeader header;
    /* 0x06C */ THPFrameCompInfo compInfo;
    /* 0x080 */ THPVideoInfo videoInfo;
    /* 0x08C */ THPAudioInfo audioInfo;
    /* 0x09C */ void* thpWork;
    /* 0x0A0 */ BOOL open;
    /* 0x0A4 */ u8 state;
    /* 0x0A5 */ u8 internalState;
    /* 0x0A6 */ u8 playFlag;
    /* 0x0A7 */ u8 audioExist;
    /* 0x0A8 */ s32 dvdError;
    /* 0x0AC */ s32 videoError;
    /* 0x0B0 */ BOOL onMemory;
    /* 0x0B4 */ u8* movieData;
    /* 0x0B8 */ s32 initOffset;
    /* 0x0BC */ s32 initReadSize;
    /* 0x0C0 */ s32 initReadFrame;
    /* 0x0C4 */ u32 curField;
    /* 0x0C8 */ s64 retraceCount;
    /* 0x0D0 */ s32 prevCount;
    /* 0x0D4 */ s32 curCount;
    /* 0x0D8 */ s32 videoDecodeCount;
    /* 0x0DC */ s32 curAudioTrack;
    /* 0x0E0 */ s32 curVideoNumber;
    /* 0x0E4 */ s32 curAudioNumber;
    /* 0x0E8 */ THPTextureSet* dispTextureSet;
    /* 0x0EC */ THPAudioBuffer* playAudioBuffer;
    /* 0x0F0 */ THPReadBuffer readBuffer[10];
    /* 0x168 */ THPTextureSet textureSet[3];
    /* 0x198 */ THPAudioBuffer audioBuffer[3];
} THPPlayer; /* size 0x1C0 */

/* ActivePlayer (THPPlayer.c). */
extern THPPlayer lbl_8046AC60;

/* THPPlayer.c */
void PrepareReady(BOOL msg); /* PrepareReady */

/* THPRead.c */
BOOL CreateReadThread(s32 priority);    /* CreateReadThread */
void ReadThreadStart(void);            /* ReadThreadStart */
void ReadThreadCancel(void);            /* ReadThreadCancel */
void* fn_801E1BE8(void);           /* PopReadedBuffer */
void PushFreeReadBuffer(void* buffer);    /* PushFreeReadBuffer */
void* fn_801E1B84(void);           /* PopReadedBuffer2 */
BOOL fn_801E1B54(void* buffer);    /* PushReadedBuffer2 */

/* THPAudioDecode.c */
BOOL CreateAudioDecodeThread(s32 priority, void* arg); /* CreateAudioDecodeThread */
void AudioDecodeThreadStart(void);                    /* AudioDecodeThreadStart */
void AudioDecodeThreadCancel(void);                    /* AudioDecodeThreadCancel */
void* PopDecodedAudioBuffer(s32 flags);              /* PopDecodedAudioBuffer */
void PushFreeAudioBuffer(void* buffer);            /* PushFreeAudioBuffer */

/* THPVideoDecode.c */
BOOL CreateVideoDecodeThread(s32 priority, void* arg); /* CreateVideoDecodeThread */
void VideoDecodeThreadStart(void);                    /* VideoDecodeThreadStart */
void VideoDecodeThreadCancel(void);                    /* VideoDecodeThreadCancel */
void* PopDecodedTextureSet(s32 flags);              /* PopDecodedTextureSet */
void PushFreeTextureSet(void* tex);               /* PushFreeTextureSet */

/* THPDraw.c */
void THPGXRestore(void); /* THPGXRestore */
void THPGXYuv2RgbSetup(void* rmode); /* THPGXYuv2RgbSetup */
void THPGXYuv2RgbDraw(u8* y_data, u8* u_data, u8* v_data, s16 x, s16 y,
                 s16 textureWidth, s16 textureHeight, s16 polygonWidth,
                 s16 polygonHeight); /* THPGXYuv2RgbDraw */

#endif
