/**
 * @file thp_player.c
 * @brief Host-side THP movie decoder -- see thp_player.h.
 *
 * THP layout (all big-endian) confirmed against movie/gs_logo.thp:
 *   header @0x00: "THP\0", version, maxBufSize@0x08, fps@0x10 (f32),
 *                 numFrames@0x14, firstFrameSize@0x18, componentDataOffset@0x20,
 *                 movieDataOffset@0x28.
 *   component data @componentDataOffset: numComponents(u32), 16 type bytes
 *                 (0=video,1=audio,0xFF=none), then per-component structs; the
 *                 video struct begins width(u32),height(u32).
 *   each frame @off: nextFrameSize(u32), prevFrameSize(u32), then one size(u32)
 *                 per component, then the component data blocks in order. The
 *                 video block is a complete baseline JPEG (FFD8..FFD9). The next
 *                 frame is at off + currentFrameSize (32-byte aligned); the
 *                 first frame's size is firstFrameSize.
 */
#ifdef __MWERKS__
/* GCN build: host THP player not applicable. */
#else

#include "thp_player.h"
#include "thp_audio.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_JPEG
#define STBI_NO_SIMD
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char  thp_u8;
typedef unsigned short thp_u16;
typedef short          thp_s16;
typedef unsigned int   thp_u32;

struct PCPortTHP {
    FILE*   fp;
    thp_u32 numFrames;
    thp_u32 numComponents;
    thp_u32 curOffset;     /* file offset of the current (next-to-decode) frame */
    thp_u32 curSize;       /* size of the current frame (to advance past it) */
    thp_u32 frameIndex;    /* number of frames decoded so far */
    thp_u32 maxBufSize;    /* upper bound on a frame's encoded size */
    int     width;
    int     height;
    float   fps;
    thp_u8* jpegBuf;       /* scratch for the raw (THP-variant) encoded block */
    thp_u8* stuffBuf;      /* scratch for the re-stuffed standard JPEG */
    thp_u32 stuffCap;
    unsigned char* rgba;   /* stb-allocated decoded frame (freed next call/close) */

    /* --- Audio (THP DSP-ADPCM) --- */
    int     hasAudio;
    int     audioCompIndex;     /* component index of the audio stream */
    int     numAudioChannels;
    thp_u32 audioSampleRate;
    thp_u32 audioTotalSamples;
    thp_u32 maxAudioSamples;    /* header @0x0C: max decoded samples per frame */
    thp_u8* audioBlock;         /* scratch for one frame's raw audio block */
    thp_u32 audioBlockCap;
    short*  pcmBuf;             /* interleaved decoded PCM (this frame) */
    thp_u32 pcmCap;             /* capacity in samples (frames*channels) */
    thp_s16* monoBuf;           /* per-channel mono decode scratch */

    /* per-frame bookkeeping captured by NextFrameRGBA so the audio call can
     * locate the audio block within the SAME frame regardless of call order. */
    thp_u32 frameBaseOffset;    /* file offset of the frame header just read */
    thp_u32 frameHdrSize;       /* 8 + 4*numComponents */
    thp_u32 compSize[16];       /* per-component data sizes for the current frame */
    int     frameValid;         /* 1 once NextFrameRGBA has read a frame header */
};

static thp_u32 thp_be32(const thp_u8* p) {
    return ((thp_u32)p[0] << 24) | ((thp_u32)p[1] << 16) |
           ((thp_u32)p[2] << 8)  |  (thp_u32)p[3];
}

static float thp_bef32(const thp_u8* p) {
    union { thp_u32 u; float f; } v;
    v.u = thp_be32(p);
    return v.f;
}

PCPortTHP* PCPortTHP_Open(const char* path) {
    PCPortTHP* thp;
    FILE* fp;
    thp_u8 hdr[0x50];
    thp_u8 comp[0x30];
    thp_u32 componentDataOffset;

    if (path == NULL) {
        return NULL;
    }
    fp = fopen(path, "rb");
    if (fp == NULL) {
        return NULL;
    }
    if (fread(hdr, 1, sizeof(hdr), fp) != sizeof(hdr) ||
        memcmp(hdr, "THP", 3) != 0 || hdr[3] != 0) {
        fclose(fp);
        return NULL;
    }

    thp = (PCPortTHP*)calloc(1, sizeof(*thp));
    if (thp == NULL) {
        fclose(fp);
        return NULL;
    }
    thp->fp = fp;
    thp->maxBufSize = thp_be32(hdr + 0x08);
    thp->maxAudioSamples = thp_be32(hdr + 0x0C);
    thp->fps        = thp_bef32(hdr + 0x10);
    thp->numFrames  = thp_be32(hdr + 0x14);
    thp->curSize    = thp_be32(hdr + 0x18);  /* firstFrameSize */
    componentDataOffset = thp_be32(hdr + 0x20);
    thp->curOffset  = thp_be32(hdr + 0x28);  /* movieDataOffset */
    thp->frameIndex = 0;

    if (fseek(fp, (long)componentDataOffset, SEEK_SET) != 0 ||
        fread(comp, 1, sizeof(comp), fp) != sizeof(comp)) {
        PCPortTHP_Close(thp);
        return NULL;
    }
    thp->numComponents = thp_be32(comp + 0x00);
    /* Video is component 0 in these files; its struct (after the 16 type bytes)
     * begins width, height. */
    thp->width  = (int)thp_be32(comp + 0x14);
    thp->height = (int)thp_be32(comp + 0x18);

    /* Walk the 16 type bytes (comp+0x04) and the per-component structs that
     * follow the 16-byte type block (comp+0x14): video=12 bytes, audio=16 bytes.
     * type 0=video, 1=audio, 0xFF=none. */
    {
        thp_u32 structOff = 0x14;  /* start of per-component structs */
        thp_u32 ci;
        thp->hasAudio = 0;
        thp->audioCompIndex = -1;
        for (ci = 0; ci < thp->numComponents && ci < 16; ++ci) {
            thp_u8 type = comp[0x04 + ci];
            if (type == 0) {            /* video: width+height+pad = 12 bytes */
                structOff += 12u;
            } else if (type == 1) {     /* audio: nCh+rate+total+unk = 16 bytes */
                if (structOff + 12u <= sizeof(comp)) {
                    thp->numAudioChannels  = (int)thp_be32(comp + structOff + 0x00);
                    thp->audioSampleRate   = thp_be32(comp + structOff + 0x04);
                    thp->audioTotalSamples = thp_be32(comp + structOff + 0x08);
                    if (thp->numAudioChannels >= 1 && thp->numAudioChannels <= 2 &&
                        thp->audioSampleRate > 0 && thp->maxAudioSamples > 0) {
                        thp->hasAudio = 1;
                        thp->audioCompIndex = (int)ci;
                    }
                }
                structOff += 16u;
            } else {
                break;  /* 0xFF / unknown: no more components */
            }
        }
    }

    if (thp->numComponents == 0 || thp->numComponents > 16 ||
        thp->width <= 0 || thp->height <= 0 ||
        thp->maxBufSize == 0 || thp->numFrames == 0) {
        PCPortTHP_Close(thp);
        return NULL;
    }
    thp->jpegBuf = (thp_u8*)malloc(thp->maxBufSize);
    /* Re-stuffing can at most double the entropy section (every byte 0xFF), plus
     * the header and EOI; 2x + slack is a safe upper bound. */
    thp->stuffCap = thp->maxBufSize * 2u + 16u;
    thp->stuffBuf = (thp_u8*)malloc(thp->stuffCap);
    if (thp->jpegBuf == NULL || thp->stuffBuf == NULL) {
        PCPortTHP_Close(thp);
        return NULL;
    }

    if (thp->hasAudio) {
        /* Raw audio block per frame: 8 (channelSize+numSamples) + numCh*36
         * (headers) + numCh*channelSize (data). channelSize bounded by
         * ceil(maxAudioSamples/14)*8. Generous cap covers worst-case frames. */
        thp_u32 chSize = ((thp->maxAudioSamples + 13u) / 14u) * 8u;
        thp->audioBlockCap = 8u + (thp_u32)thp->numAudioChannels * (36u + chSize) + 64u;
        thp->audioBlock = (thp_u8*)malloc(thp->audioBlockCap);
        thp->pcmCap = thp->maxAudioSamples * (thp_u32)thp->numAudioChannels;
        thp->pcmBuf = (short*)malloc(thp->pcmCap * sizeof(short));
        thp->monoBuf = (thp_s16*)malloc(thp->maxAudioSamples * sizeof(thp_s16));
        if (thp->audioBlock == NULL || thp->pcmBuf == NULL ||
            thp->monoBuf == NULL) {
            /* Audio is optional; degrade to video-only rather than fail. */
            thp->hasAudio = 0;
        }
    }
    return thp;
}

void PCPortTHP_Close(PCPortTHP* thp) {
    if (thp == NULL) {
        return;
    }
    if (thp->rgba != NULL) {
        stbi_image_free(thp->rgba);
    }
    if (thp->jpegBuf != NULL) {
        free(thp->jpegBuf);
    }
    if (thp->stuffBuf != NULL) {
        free(thp->stuffBuf);
    }
    if (thp->audioBlock != NULL) {
        free(thp->audioBlock);
    }
    if (thp->pcmBuf != NULL) {
        free(thp->pcmBuf);
    }
    if (thp->monoBuf != NULL) {
        free(thp->monoBuf);
    }
    if (thp->fp != NULL) {
        fclose(thp->fp);
    }
    free(thp);
}

int   PCPortTHP_Width(const PCPortTHP* thp)      { return thp ? thp->width : 0; }
int   PCPortTHP_Height(const PCPortTHP* thp)     { return thp ? thp->height : 0; }
int   PCPortTHP_FrameCount(const PCPortTHP* thp) { return thp ? (int)thp->numFrames : 0; }
float PCPortTHP_Fps(const PCPortTHP* thp)        { return thp ? thp->fps : 0.0f; }

/* THP video frames are JPEG with the standard FF00 entropy byte-stuffing OMITTED
 * (a Nintendo space-saving variant), so stock JPEG decoders choke on any literal
 * 0xFF in the scan. Rebuild a standard JPEG by re-inserting a 0x00 after every
 * 0xFF in the scan region [SOS-data .. EOI), then a fresh EOI. Returns the length
 * written to dst, or 0 on failure. */
static thp_u32 thp_restuff(const thp_u8* src, thp_u32 srcLen,
                           thp_u8* dst, thp_u32 dstCap) {
    thp_u32 p;
    thp_u32 scanStart = 0;
    thp_u32 eoiPos = 0;
    thp_u32 di;
    thp_u32 i;

    if (srcLen < 4 || src[0] != 0xFF || src[1] != 0xD8) {
        return 0; /* not a JPEG (no SOI) */
    }
    /* Walk header segments to the start of the entropy-coded scan. */
    p = 2;
    while (p + 4 <= srcLen) {
        thp_u8 marker;
        thp_u32 segLen;
        if (src[p] != 0xFF) {
            return 0;
        }
        marker = src[p + 1];
        if (marker == 0xFF) { p++; continue; }            /* fill byte */
        if (marker == 0xD8 || marker == 0xD9) { p += 2; continue; }
        segLen = ((thp_u32)src[p + 2] << 8) | (thp_u32)src[p + 3];
        if (marker == 0xDA) {                              /* SOS */
            scanStart = p + 2 + segLen;
            break;
        }
        p += 2 + segLen;
    }
    if (scanStart == 0 || scanStart >= srcLen) {
        return 0;
    }
    /* Real EOI = last FF D9 in the block (any trailing bytes are padding). */
    i = srcLen;
    while (i >= scanStart + 2) {
        if (src[i - 2] == 0xFF && src[i - 1] == 0xD9) {
            eoiPos = i - 2;
            break;
        }
        --i;
    }
    if (eoiPos <= scanStart) {
        return 0;
    }
    if (scanStart > dstCap) {
        return 0;
    }
    memcpy(dst, src, scanStart);                            /* header verbatim */
    di = scanStart;
    for (i = scanStart; i < eoiPos; ++i) {                  /* restuff entropy */
        if (di + 2u > dstCap) {
            return 0;
        }
        dst[di++] = src[i];
        if (src[i] == 0xFF) {
            dst[di++] = 0x00;
        }
    }
    if (di + 2u > dstCap) {
        return 0;
    }
    dst[di++] = 0xFF;                                       /* fresh EOI */
    dst[di++] = 0xD9;
    return di;
}

int PCPortTHP_NextFrameRGBA(PCPortTHP* thp, const unsigned char** outRGBA) {
    thp_u8 fh[8 + 16 * 4];   /* next/prev size + one size per component */
    thp_u32 nextSize;
    thp_u32 videoSize;
    thp_u32 hdrSize;
    int w;
    int h;
    int n;

    if (thp == NULL || thp->frameIndex >= thp->numFrames) {
        return 0;
    }
    hdrSize = 8u + 4u * thp->numComponents;
    if (fseek(thp->fp, (long)thp->curOffset, SEEK_SET) != 0 ||
        fread(fh, 1, hdrSize, thp->fp) != hdrSize) {
        return 0;
    }
    nextSize  = thp_be32(fh + 0x00);
    videoSize = thp_be32(fh + 0x08);  /* first component (video) data size */

    /* Capture per-component sizes + frame base so NextFrameAudioPCM can locate
     * the audio block in this SAME frame (independent of advance order). */
    {
        thp_u32 ci;
        thp->frameBaseOffset = thp->curOffset;
        thp->frameHdrSize = hdrSize;
        for (ci = 0; ci < thp->numComponents && ci < 16; ++ci) {
            thp->compSize[ci] = thp_be32(fh + 0x08 + ci * 4u);
        }
        thp->frameValid = 1;
    }

    if (videoSize == 0 || videoSize > thp->maxBufSize) {
        fprintf(stderr, "[thp] frame %u: bad videoSize %u (max %u)\n",
                thp->frameIndex, videoSize, thp->maxBufSize);
        return 0;
    }
    /* The video JPEG block follows all the per-component size fields. */
    if (fseek(thp->fp, (long)(thp->curOffset + hdrSize), SEEK_SET) != 0 ||
        fread(thp->jpegBuf, 1, videoSize, thp->fp) != videoSize) {
        fprintf(stderr, "[thp] frame %u: read of %u video bytes failed\n",
                thp->frameIndex, videoSize);
        return 0;
    }

    {
        thp_u32 stuffLen = thp_restuff(thp->jpegBuf, videoSize,
                                       thp->stuffBuf, thp->stuffCap);
        if (stuffLen == 0) {
            fprintf(stderr, "[thp] frame %u: JPEG re-stuff failed (%u bytes)\n",
                    thp->frameIndex, videoSize);
            return 0;
        }
        if (thp->rgba != NULL) {
            stbi_image_free(thp->rgba);
            thp->rgba = NULL;
        }
        thp->rgba = stbi_load_from_memory(thp->stuffBuf, (int)stuffLen, &w, &h, &n, 4);
    }
    if (thp->rgba == NULL) {
        fprintf(stderr, "[thp] frame %u: stbi decode failed (stbi: %s)\n",
                thp->frameIndex, stbi_failure_reason());
        return 0;
    }
    if (w != thp->width || h != thp->height) {
        thp->width = w;
        thp->height = h;
    }

    /* Advance by the CURRENT frame size; the next frame's size becomes current. */
    thp->curOffset += thp->curSize;
    thp->curSize = (nextSize + 31u) & ~31u;
    thp->frameIndex++;

    if (outRGBA != NULL) {
        *outRGBA = thp->rgba;
    }
    return 1;
}

/* --- Audio --------------------------------------------------------------- */

int      PCPortTHP_HasAudio(const PCPortTHP* thp) {
    return (thp && thp->hasAudio) ? 1 : 0;
}
unsigned PCPortTHP_AudioSampleRate(const PCPortTHP* thp) {
    return (thp && thp->hasAudio) ? thp->audioSampleRate : 0u;
}
int      PCPortTHP_AudioChannels(const PCPortTHP* thp) {
    return (thp && thp->hasAudio) ? thp->numAudioChannels : 0;
}
unsigned PCPortTHP_AudioTotalSamples(const PCPortTHP* thp) {
    return (thp && thp->hasAudio) ? thp->audioTotalSamples : 0u;
}

int PCPortTHP_NextFrameAudioPCM(PCPortTHP* thp, const short** outPCM,
                                unsigned* outNumFrames) {
    thp_u32 audioOffset;
    thp_u32 audioSize;
    thp_u32 channelSize;
    thp_u32 numSamples;
    thp_u32 headersBase;
    thp_u32 dataBase;
    int ch;
    int nCh;
    thp_u32 ci;

    if (outNumFrames != NULL) {
        *outNumFrames = 0;
    }
    if (thp == NULL || !thp->hasAudio || !thp->frameValid ||
        thp->audioBlock == NULL || thp->pcmBuf == NULL) {
        return 0;
    }
    nCh = thp->numAudioChannels;

    /* The audio block is the audioCompIndex-th component within the frame; its
     * file offset = frameBase + frameHdr + sum(sizes of earlier components). */
    audioOffset = thp->frameBaseOffset + thp->frameHdrSize;
    for (ci = 0; ci < (thp_u32)thp->audioCompIndex && ci < 16; ++ci) {
        audioOffset += thp->compSize[ci];
    }
    audioSize = (thp->audioCompIndex < 16) ?
                thp->compSize[thp->audioCompIndex] : 0u;
    if (audioSize < 8u || audioSize > thp->audioBlockCap) {
        return 0;
    }

    if (fseek(thp->fp, (long)audioOffset, SEEK_SET) != 0 ||
        fread(thp->audioBlock, 1, audioSize, thp->fp) != audioSize) {
        return 0;
    }

    /* Layout A: [channelSize][numSamples][ch0 hdr 36][ch1 hdr 36][ch0 data][ch1 data] */
    channelSize = thp_be32(thp->audioBlock + 0x00);
    numSamples  = thp_be32(thp->audioBlock + 0x04);
    if (numSamples == 0 || numSamples > thp->maxAudioSamples) {
        return 0;
    }
    headersBase = 8u;
    dataBase    = 8u + (thp_u32)nCh * 36u;
    /* Validate the block is large enough for headers + data. */
    if (dataBase + (thp_u32)nCh * channelSize > audioSize) {
        return 0;
    }

    for (ch = 0; ch < nCh; ++ch) {
        const thp_u8* chHdr  = thp->audioBlock + headersBase + (thp_u32)ch * 36u;
        const thp_u8* chData = thp->audioBlock + dataBase + (thp_u32)ch * channelSize;
        thp_s16 coeffs[16];
        thp_s16 yn1;
        thp_s16 yn2;
        thp_u32 s;
        int i;

        for (i = 0; i < 16; ++i) {
            coeffs[i] = (thp_s16)(thp_u16)
                (((thp_u16)chHdr[i * 2] << 8) | (thp_u16)chHdr[i * 2 + 1]);
        }
        yn1 = (thp_s16)(thp_u16)(((thp_u16)chHdr[32] << 8) | (thp_u16)chHdr[33]);
        yn2 = (thp_s16)(thp_u16)(((thp_u16)chHdr[34] << 8) | (thp_u16)chHdr[35]);

        /* Decode this channel to mono, then scatter into the interleaved out.
         * THP supplies fresh yn1/yn2 per frame, so re-seed each frame (do NOT
         * carry decoder state across frames). */
        thp_adpcm_decode(chData, coeffs, &yn1, &yn2, numSamples, thp->monoBuf);
        for (s = 0; s < numSamples; ++s) {
            thp->pcmBuf[s * (thp_u32)nCh + (thp_u32)ch] = thp->monoBuf[s];
        }
    }

    if (outPCM != NULL) {
        *outPCM = thp->pcmBuf;
    }
    if (outNumFrames != NULL) {
        *outNumFrames = numSamples;
    }
    return 1;
}

#endif /* __MWERKS__ */
