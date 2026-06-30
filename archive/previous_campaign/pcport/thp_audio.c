/**
 * @file thp_audio.c
 * @brief GC DSP-ADPCM decoder + WAV verifier for THP boot-movie audio.
 *
 * See thp_audio.h for the block format. Verified against tpc.thp (Layout A):
 * zero predIdx>7 occurrences, plausible per-frame coefficients, and a sane RMS
 * envelope on the decoded ~2.5s stereo jingle.
 */
#ifdef __MWERKS__
/* GCN build: host THP audio decode not applicable. */
#else

#include "thp_audio.h"

#include <stdio.h>
#include <string.h>

static int16_t thp_clamp16(int32_t v) {
    if (v > 32767) {
        return (int16_t)32767;
    }
    if (v < -32768) {
        return (int16_t)-32768;
    }
    return (int16_t)v;
}

void thp_adpcm_decode(const uint8_t* adpcm, const int16_t coeffs[16],
                      int16_t* yn1, int16_t* yn2,
                      uint32_t numSamples, int16_t* outPCM) {
    int32_t h1 = (yn1 != NULL) ? (int32_t)*yn1 : 0;
    int32_t h2 = (yn2 != NULL) ? (int32_t)*yn2 : 0;
    uint32_t produced = 0;
    uint32_t blk = 0;

    if (adpcm == NULL || coeffs == NULL || outPCM == NULL) {
        return;
    }

    while (produced < numSamples) {
        const uint8_t* block = adpcm + (size_t)blk * 8u;
        uint8_t header = block[0];
        int predIdx = (header >> 4) & 0x0F;
        int scaleShift = header & 0x0F;
        int32_t c0;
        int32_t c1;
        int i;

        if (predIdx > 7) {
            predIdx = 7; /* defensive clamp; valid Layout-A data never exceeds 7 */
        }
        c0 = (int32_t)coeffs[predIdx * 2 + 0];
        c1 = (int32_t)coeffs[predIdx * 2 + 1];

        for (i = 0; i < 14 && produced < numSamples; ++i) {
            uint8_t byte = block[1 + (i >> 1)];
            int32_t nibble = (i & 1) ? (byte & 0x0F) : (byte >> 4);
            int32_t sample;

            /* sign-extend the 4-bit residual */
            if (nibble >= 8) {
                nibble -= 16;
            }
            sample = (nibble << scaleShift) + ((c0 * h1 + c1 * h2 + 1024) >> 11);
            outPCM[produced++] = thp_clamp16(sample);
            h2 = h1;
            h1 = outPCM[produced - 1];
        }
        ++blk;
    }

    if (yn1 != NULL) {
        *yn1 = (int16_t)h1;
    }
    if (yn2 != NULL) {
        *yn2 = (int16_t)h2;
    }
}

/* --- WAV writer ---------------------------------------------------------- */

static void thp_put_u32le(uint8_t* p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}

static void thp_put_u16le(uint8_t* p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
}

int thp_audio_write_wav(const char* path, const int16_t* pcm,
                        uint32_t numFrames, int numChannels,
                        uint32_t sampleRate) {
    FILE* fp;
    uint8_t hdr[44];
    uint32_t bytesPerSample = 2u;
    uint32_t blockAlign = (uint32_t)numChannels * bytesPerSample;
    uint32_t byteRate = sampleRate * blockAlign;
    uint32_t dataBytes = numFrames * blockAlign;

    if (path == NULL || pcm == NULL || numChannels <= 0) {
        return 0;
    }
    fp = fopen(path, "wb");
    if (fp == NULL) {
        return 0;
    }

    memcpy(hdr + 0, "RIFF", 4);
    thp_put_u32le(hdr + 4, 36u + dataBytes);
    memcpy(hdr + 8, "WAVE", 4);
    memcpy(hdr + 12, "fmt ", 4);
    thp_put_u32le(hdr + 16, 16u);                 /* fmt chunk size */
    thp_put_u16le(hdr + 20, 1u);                  /* PCM */
    thp_put_u16le(hdr + 22, (uint16_t)numChannels);
    thp_put_u32le(hdr + 24, sampleRate);
    thp_put_u32le(hdr + 28, byteRate);
    thp_put_u16le(hdr + 32, (uint16_t)blockAlign);
    thp_put_u16le(hdr + 34, (uint16_t)(bytesPerSample * 8u)); /* bits per sample */
    memcpy(hdr + 36, "data", 4);
    thp_put_u32le(hdr + 40, dataBytes);

    if (fwrite(hdr, 1, sizeof(hdr), fp) != sizeof(hdr)) {
        fclose(fp);
        return 0;
    }
    if (dataBytes != 0 &&
        fwrite(pcm, 1, dataBytes, fp) != dataBytes) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}

#endif /* __MWERKS__ */
