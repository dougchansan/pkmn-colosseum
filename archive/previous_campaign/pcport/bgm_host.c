/**
 * @file bgm_host.c
 * @brief Background-music playback for the Colosseum PC port -- see bgm_host.h.
 *
 * -------------------------------------------------------------------------
 * Architecture
 * -------------------------------------------------------------------------
 * Layer 1 – FSYS container
 *   The game archives are FSYS files with a custom header that lists
 *   named members.  Each member is a raw blob prefixed with an LZSS
 *   compression header ("LZSS" + decompSize + compSize + reserved).
 *   We parse just enough of the FSYS header to locate a member by name
 *   and read its compressed bytes, then decompress with a ring-buffer
 *   LZSS (0x1000-byte window, init position 0xFEE, flag-byte MSB=literal).
 *
 * Layer 2 – MusyX wave pool (best-effort)
 *   Music data lives in .pool members (e.g. snd_music_pool).  A .pool is
 *   a MusyX "AudioWavePool" blob.  Full MusyX decoding (sequencer, .sdir
 *   wave directory, .proj patterns) is NOT implemented (see bgm_host.h).
 *   Instead we scan for a wave header signature and play the first
 *   DSP-ADPCM sample we find.
 *
 *   MusyX DSP-ADPCM wave block header layout (all big-endian u32):
 *     +0x00  numSamples    total sample count for this wave
 *     +0x04  loopStartSample
 *     +0x08  loopEndSample  (0 = no loop)
 *     +0x0C  sampleRate    (typically 22050 or 32000)
 *     +0x10  coefficients  16 x s16 (ADPCM predictor coefficients)
 *     +0x30  loopPred      s16 loop-context predictor index
 *     +0x32  loopYn1       s16 loop-context yn1
 *     +0x34  loopYn2       s16 loop-context yn2
 *     +0x36  padding       u16
 *     +0x38  adpcmData     packed 4-bit ADPCM nibbles (8-byte blocks)
 *
 *   The pool starts with a u32 table-of-contents offset, then a 4-byte
 *   entry count, followed by per-entry {offset, size} pairs (8 bytes each).
 *   We use these to locate wave data within the pool.
 *
 * Layer 3 – DSP-ADPCM decode
 *   Reuses thp_adpcm_decode() from thp_audio.c (identical block format).
 *
 * Layer 4 – WaveOut streaming
 *   Reuses WaveOutSink from waveout_sink.c.  PCPortBGM_Update decodes one
 *   ADPCM block (14 samples) per call and submits it.  At 32 kHz that is
 *   ~0.4 ms per block; callers should drive Update at frame rate (~60 Hz)
 *   which fills well ahead of the waveOut cursor.  We use a larger batch
 *   (BGM_DECODE_BATCH_SAMPLES) to reduce submit overhead.
 */
#ifdef __MWERKS__
/* GCN build: host BGM module not applicable. */
#else

#include "bgm_host.h"
#include "thp_audio.h"     /* thp_adpcm_decode() */
#include "waveout_sink.h"  /* WaveOutSink_* */
#include "real_content_host.h"  /* PCPort_LoadFsysMember (proven FSYS loader) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* =========================================================================
 * Tunables
 * ========================================================================= */

/** Samples decoded per PCPortBGM_Update call (both channels if stereo). */
#define BGM_DECODE_BATCH_SAMPLES 1024u

/** Loop by default (1 = loop continuously, 0 = play once). */
#define BGM_LOOP 1

/** Maximum FSYS member name length (including NUL). */
#define BGM_MAX_NAME 128u

/** Maximum number of FSYS entries we scan. */
#define BGM_MAX_FSYS_ENTRIES 256u

/* =========================================================================
 * FSYS parsing helpers (all big-endian)
 * ========================================================================= */

static uint32_t bgm_be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |  (uint32_t)p[3];
}

static uint16_t bgm_be16(const uint8_t *p) {
    return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

/*
 * FSYS header layout (big-endian):
 *   +0x00  u32  magic       "FSYS"
 *   +0x04  u32  version     0x104
 *   +0x08  u32  fileCount   total member count
 *   +0x0C  u32  (unused in parse)
 *   +0x10  u32  (unused in parse)
 *   +0x14  u32  numSections  (2 or 3)
 *   +0x18  u32  section[0]  offset of name+offset table
 *   +0x1C  u32  section[1]  offset of LZSS file entry table
 *   +0x20  u32  section[2]  end of file (== file size)
 *
 * Offset table at section[0]:
 *   [0x00]  u32 -> offset of per-member name-string block (inside archive)
 *   [0x08] .. array of u32 member entry offsets (unused; we scan entries)
 *
 * Entry table at section[1] (skipped if LZSS-compressed itself):
 *   Each entry is 0x50 bytes:
 *     +0x04  u32  dataOff   file offset of this member's data blob
 *     +0x24  u32  nameOff   file offset of this member's name string
 *   All entries have flags 0x80000000 (LZSS-compressed member data).
 *
 * Member data blob layout:
 *   +0x00  u32  "LZSS"
 *   +0x04  u32  decompSize
 *   +0x08  u32  compSize
 *   +0x0C  u32  reserved
 *   +0x10  u8[] compressed payload (compSize bytes)
 */

/* =========================================================================
 * LZSS decompressor (GC ring-buffer variant)
 * ========================================================================= */

/**
 * Decompress a GC LZSS stream into a caller-supplied buffer.
 *
 * @param comp       Compressed payload bytes (NOT including the LZSS header).
 * @param compSize   Number of compressed bytes.
 * @param out        Output buffer, must be at least decompSize bytes.
 * @param decompSize Expected decompressed size (stops when reached).
 * @return Actual bytes written to @p out, or 0 on error.
 *
 * Ring-buffer parameters (matching the game's encoder):
 *   window = 0x1000 bytes, init position = 0xFEE.
 *   Flag byte: bit 0 = chunk 0, bit 7 = chunk 7.
 *   Bit 1: literal; bit 0: back-reference.
 *   Back-reference encoding: 2 bytes (b0, b1):
 *     matchPos = b0 | ((b1 & 0xF0) << 4)
 *     matchLen = (b1 & 0x0F) + 3
 */
static uint32_t bgm_lzss_decompress(const uint8_t *comp, uint32_t compSize,
                                     uint8_t *out, uint32_t decompSize) {
    uint8_t ring[0x1000];
    uint32_t ring_pos = 0xFEE;
    uint32_t src = 0;
    uint32_t dst = 0;
    int bit;

    memset(ring, 0, sizeof(ring));

    while (dst < decompSize && src < compSize) {
        uint8_t flags;
        if (src >= compSize) {
            break;
        }
        flags = comp[src++];

        for (bit = 0; bit < 8; ++bit) {
            uint32_t match_pos;
            uint32_t match_len;
            uint32_t k;
            uint8_t b;

            if (dst >= decompSize || src >= compSize) {
                break;
            }

            if ((flags >> bit) & 1u) {
                /* Literal byte */
                b = comp[src++];
                out[dst++] = b;
                ring[ring_pos] = b;
                ring_pos = (ring_pos + 1u) & 0xFFFu;
            } else {
                /* Back-reference */
                uint8_t b0, b1;
                if (src + 1u >= compSize) {
                    break;
                }
                b0 = comp[src++];
                b1 = comp[src++];
                match_pos = (uint32_t)b0 | (((uint32_t)b1 & 0xF0u) << 4u);
                match_len = ((uint32_t)b1 & 0x0Fu) + 3u;
                for (k = 0; k < match_len && dst < decompSize; ++k) {
                    b = ring[match_pos & 0xFFFu];
                    out[dst++] = b;
                    ring[ring_pos] = b;
                    ring_pos = (ring_pos + 1u) & 0xFFFu;
                    match_pos = (match_pos + 1u) & 0xFFFu;
                }
            }
        }
    }
    return dst;
}

/* =========================================================================
 * FSYS member extraction
 * ========================================================================= */

/**
 * Find a member by name in an open FSYS file, LZSS-decompress it, and
 * return the decompressed buffer (caller must free).
 *
 * @param fp          Open FILE* positioned at start of the FSYS archive.
 * @param memberName  Member name to find (NUL-terminated).
 * @param outSize     Receives the decompressed byte count on success.
 * @return Heap-allocated decompressed buffer, or NULL on failure.
 */
static uint8_t *bgm_fsys_extract(FILE *fp, const char *memberName,
                                  uint32_t *outSize) {
    uint8_t hdr[0x40];
    uint32_t fileCount;
    uint32_t numSections;
    uint32_t entryTableOff;
    uint32_t nameBase;          /* offset of the name string block */
    uint32_t i;
    uint8_t  entry[0x50];

    if (!fp || !memberName || !outSize) {
        return NULL;
    }
    *outSize = 0;

    /* Read and validate FSYS header */
    if (fseek(fp, 0, SEEK_SET) != 0 ||
        fread(hdr, 1, sizeof(hdr), fp) != sizeof(hdr)) {
        return NULL;
    }
    if (memcmp(hdr, "FSYS", 4) != 0) {
        return NULL;
    }

    fileCount   = bgm_be32(hdr + 0x08);
    numSections = bgm_be32(hdr + 0x14);
    if (numSections < 2u || fileCount == 0u || fileCount > BGM_MAX_FSYS_ENTRIES) {
        return NULL;
    }

    /*
     * section[0] -> offset-table block.  The name string block starts at
     * the u32 stored at offset [section[0] + 0x00].
     * section[1] -> file-entry table (each entry 0x50 bytes).
     */
    {
        uint8_t secBuf[8];
        if (fseek(fp, (long)bgm_be32(hdr + 0x18), SEEK_SET) != 0 ||
            fread(secBuf, 1, 4, fp) != 4) {
            return NULL;
        }
        nameBase = bgm_be32(secBuf);
    }
    entryTableOff = bgm_be32(hdr + 0x1C);

    /* Scan file entries */
    for (i = 0; i < fileCount && i < BGM_MAX_FSYS_ENTRIES; ++i) {
        uint32_t entryOff = entryTableOff + i * 0x50u;
        uint32_t nameOff;
        uint32_t dataOff;
        char nameBuf[BGM_MAX_NAME];
        uint32_t nameLen;
        uint8_t lzssHdr[16];
        uint32_t decompSize;
        uint32_t compSize;
        uint8_t *compBuf;
        uint8_t *decompBuf;
        uint32_t actual;

        if (fseek(fp, (long)entryOff, SEEK_SET) != 0 ||
            fread(entry, 1, sizeof(entry), fp) != sizeof(entry)) {
            break;
        }

        /* Skip padding / sentinel entries */
        if (entry[0] == 0x11 || (entry[0] == 0 && entry[4] == 0 &&
                                   entry[8] == 0 && entry[12] == 0)) {
            continue;
        }

        dataOff = bgm_be32(entry + 0x04);
        nameOff = bgm_be32(entry + 0x24);

        /* Validate name offset falls within the name string block */
        if (nameOff < nameBase || nameOff >= entryTableOff) {
            continue;
        }

        /* Read the member name */
        if (fseek(fp, (long)nameOff, SEEK_SET) != 0) {
            continue;
        }
        nameLen = (uint32_t)fread(nameBuf, 1, BGM_MAX_NAME - 1u, fp);
        {
            uint32_t j;
            for (j = 0; j < nameLen; ++j) {
                if (nameBuf[j] == '\0') { break; }
            }
            nameBuf[j] = '\0';
        }

        if (strcmp(nameBuf, memberName) != 0) {
            continue;
        }

        /* Found the member -- read its LZSS header */
        if (dataOff == 0 ||
            fseek(fp, (long)dataOff, SEEK_SET) != 0 ||
            fread(lzssHdr, 1, sizeof(lzssHdr), fp) != sizeof(lzssHdr) ||
            memcmp(lzssHdr, "LZSS", 4) != 0) {
            return NULL;
        }
        decompSize = bgm_be32(lzssHdr + 4);
        compSize   = bgm_be32(lzssHdr + 8);

        if (decompSize == 0 || compSize == 0) {
            return NULL;
        }

        compBuf = (uint8_t *)malloc((size_t)compSize);
        if (!compBuf) {
            return NULL;
        }
        if (fread(compBuf, 1, (size_t)compSize, fp) != (size_t)compSize) {
            free(compBuf);
            return NULL;
        }

        decompBuf = (uint8_t *)malloc((size_t)decompSize);
        if (!decompBuf) {
            free(compBuf);
            return NULL;
        }

        actual = bgm_lzss_decompress(compBuf, compSize, decompBuf, decompSize);
        free(compBuf);

        if (actual == 0) {
            free(decompBuf);
            return NULL;
        }

        *outSize = actual;
        return decompBuf;
    }

    return NULL;
}

/* =========================================================================
 * MusyX wave-pool best-effort first-wave locator
 * =========================================================================
 *
 * The MusyX AudioWavePool blob layout (all big-endian):
 *   +0x00  u32  tocOffset    byte offset (from blob start) to entry table
 *   +tocOffset+0x00  u32  numWaves
 *   +tocOffset+0x04  entry[0..numWaves-1]:
 *       entry {
 *         u32 waveOffset  (from blob start)
 *         u32 waveSize
 *       }
 *
 * Each wave starts at waveOffset and has a DSP-ADPCM header:
 *   +0x00  u32  numSamples
 *   +0x04  u32  loopStart
 *   +0x08  u32  loopEnd
 *   +0x0C  u32  sampleRate
 *   +0x10  s16[16] coefficients
 *   +0x30  s16  loopPredIdx
 *   +0x32  s16  loopYn1
 *   +0x34  s16  loopYn2
 *   +0x36  u16  padding
 *   +0x38  u8[] adpcmData
 *
 * For a plausible wave we require:
 *   - numSamples in [256, 16 * 1024 * 1024]  (at least some data, <16M)
 *   - sampleRate in [8000, 48000]
 *   - waveOffset + 0x38 + adpcmBytes <= blobSize
 */

#define MUSYX_WAVE_HDR_SIZE 0x38u

typedef struct {
    uint32_t numSamples;
    uint32_t loopStart;
    uint32_t loopEnd;
    uint32_t sampleRate;
    int16_t  coeffs[16];    /* 8 predictor pairs */
    int16_t  loopPredIdx;
    int16_t  loopYn1;
    int16_t  loopYn2;
    uint32_t adpcmOffset;   /* byte offset inside the blob */
    uint32_t adpcmBytes;    /* byte count of packed ADPCM data */
} BgmWaveInfo;

/**
 * Scan a decompressed pool blob for the first plausible DSP-ADPCM wave.
 *
 * @param blob      Decompressed pool data.
 * @param blobSize  Byte count of blob.
 * @param outWave   Receives wave description on success.
 * @return 1 if a usable wave was found, 0 otherwise.
 */
static int bgm_pool_find_first_wave(const uint8_t *blob, uint32_t blobSize,
                                     BgmWaveInfo *outWave) {
    uint32_t tocOff;
    uint32_t numWaves;
    uint32_t wi;
    int i;

    if (!blob || blobSize < 8u || !outWave) {
        return 0;
    }

    tocOff = bgm_be32(blob + 0);

    /* Sanity: tocOff must fit inside the blob */
    if (tocOff + 8u > blobSize) {
        return 0;
    }

    numWaves = bgm_be32(blob + tocOff);
    if (numWaves == 0u || numWaves > 65536u) {
        return 0;
    }

    for (wi = 0; wi < numWaves; ++wi) {
        uint32_t entryOff = tocOff + 4u + wi * 8u;
        uint32_t waveOff;
        uint32_t waveSize;
        uint32_t numSamples;
        uint32_t sampleRate;
        uint32_t adpcmBytes;

        if (entryOff + 8u > blobSize) {
            break;
        }

        waveOff  = bgm_be32(blob + entryOff + 0);
        waveSize = bgm_be32(blob + entryOff + 4);

        if (waveOff + MUSYX_WAVE_HDR_SIZE > blobSize) {
            continue;
        }
        if (waveSize < MUSYX_WAVE_HDR_SIZE) {
            continue;
        }

        numSamples = bgm_be32(blob + waveOff + 0x00);
        sampleRate = bgm_be32(blob + waveOff + 0x0C);

        /* Plausibility checks */
        if (numSamples < 256u || numSamples > 16u * 1024u * 1024u) {
            continue;
        }
        if (sampleRate < 8000u || sampleRate > 48000u) {
            continue;
        }

        /* adpcmBytes = ceil(numSamples / 14) * 8  (14 samples per 8-byte block) */
        adpcmBytes = ((numSamples + 13u) / 14u) * 8u;
        if (waveOff + MUSYX_WAVE_HDR_SIZE + adpcmBytes > blobSize) {
            /* If waveSize gives us enough data use that instead */
            adpcmBytes = waveSize - MUSYX_WAVE_HDR_SIZE;
            if (adpcmBytes == 0u) {
                continue;
            }
        }

        /* Read coefficients (16 x s16 big-endian) */
        for (i = 0; i < 16; ++i) {
            outWave->coeffs[i] = (int16_t)bgm_be16(
                blob + waveOff + 0x10 + (uint32_t)i * 2u);
        }

        outWave->numSamples  = numSamples;
        outWave->loopStart   = bgm_be32(blob + waveOff + 0x04);
        outWave->loopEnd     = bgm_be32(blob + waveOff + 0x08);
        outWave->sampleRate  = sampleRate;
        outWave->loopPredIdx = (int16_t)bgm_be16(blob + waveOff + 0x30);
        outWave->loopYn1     = (int16_t)bgm_be16(blob + waveOff + 0x32);
        outWave->loopYn2     = (int16_t)bgm_be16(blob + waveOff + 0x34);
        outWave->adpcmOffset = waveOff + MUSYX_WAVE_HDR_SIZE;
        outWave->adpcmBytes  = adpcmBytes;

        return 1;
    }

    return 0;
}

/* =========================================================================
 * BGM state
 * ========================================================================= */

typedef struct {
    int      active;            /* 1 = playing */
    int      sinkOpen;          /* 1 = WaveOutSink is open */

    /* Decompressed pool blob (heap, freed on Stop) */
    uint8_t *poolBlob;
    uint32_t poolSize;

    /* Wave info extracted from pool */
    BgmWaveInfo wave;

    /* Current decode position within the ADPCM data */
    uint32_t adpcmPos;          /* byte offset relative to wave.adpcmOffset */
    uint32_t samplePos;         /* sample offset (for loop tracking) */

    /* ADPCM decoder state carried across blocks */
    int16_t  yn1;
    int16_t  yn2;

    /* Small PCM decode batch buffer (mono, one channel -- BGM is mono for
     * simplicity; stereo duplication happens at submit time) */
    int16_t  pcmBatch[BGM_DECODE_BATCH_SAMPLES];
    /* Stereo interleave scratch: 2x the mono batch */
    int16_t  stereoBatch[BGM_DECODE_BATCH_SAMPLES * 2u];
} BgmState;

static BgmState g_bgm;

/* =========================================================================
 * Public API implementation
 * ========================================================================= */

int PCPortBGM_Init(void) {
    if (g_bgm.sinkOpen) {
        return 1;
    }
    /* Open at 32 kHz stereo s16.  The actual wave's sampleRate may differ;
     * if so we still submit at 32 kHz (pitch will be off).  A resampler
     * is future work. */
    if (!WaveOutSink_Open(32000u, 2, 16)) {
        fprintf(stderr, "[bgm] WaveOutSink_Open failed\n");
        return 0;
    }
    g_bgm.sinkOpen = 1;
    return 1;
}

int PCPortBGM_PlayFromFsys(const char *fsysPath, const char *member) {
    uint8_t *blob;
    uint32_t blobSize;
    BgmWaveInfo wv;

    if (!fsysPath || !member) {
        return 0;
    }
    if (!g_bgm.sinkOpen) {
        if (!PCPortBGM_Init()) {
            return 0;
        }
    }

    /* Stop any currently playing BGM */
    PCPortBGM_Stop();

    /* Use the proven host FSYS loader (real_content_host) rather than bgm_host's
     * own header parser, which did not match the archive layout and resolved
     * none of the real members. PCPort_LoadFsysMember handles the FSYS header,
     * name table and LZSS decompression and malloc()s the output (free()-able
     * here and in PCPortBGM_Stop). */
    {
        u8 *md = NULL;
        u32 ms = 0u;
        if (!PCPort_LoadFsysMember(fsysPath, member, &md, &ms) || md == NULL) {
            fprintf(stderr, "[bgm] member '%s' not found in %s\n",
                    member, fsysPath);
            return 0;
        }
        blob = (uint8_t *)md;
        blobSize = (uint32_t)ms;
    }

    /*
     * Attempt to locate a DSP-ADPCM wave inside the pool blob.
     *
     * NOTE: This is the best-effort heuristic described in bgm_host.h.
     * For .pool members we apply the MusyX pool table-of-contents parse.
     * For other member types (e.g. raw DSP-ADPCM blobs from area .fsys)
     * we fall through to the pool parser which will likely fail gracefully
     * (returning 0) and leave the blob in g_bgm.poolBlob for inspection.
     */
    if (!bgm_pool_find_first_wave(blob, blobSize, &wv)) {
        fprintf(stderr, "[bgm] no playable DSP-ADPCM wave found in '%s'\n"
                        "[bgm] (full MusyX sequencer not implemented -- "
                        "see bgm_host.h)\n", member);
        free(blob);
        return 0;
    }

    /* Reopen sink at the wave's actual sample rate if it differs */
    if (!g_bgm.sinkOpen ||
        /* Re-open with correct sample rate every time */
        1) {
        WaveOutSink_Close();
        g_bgm.sinkOpen = 0;
        if (!WaveOutSink_Open(wv.sampleRate, 2, 16)) {
            fprintf(stderr, "[bgm] WaveOutSink_Open(%u) failed\n",
                    (unsigned)wv.sampleRate);
            free(blob);
            return 0;
        }
        g_bgm.sinkOpen = 1;
    }

    g_bgm.poolBlob  = blob;
    g_bgm.poolSize  = blobSize;
    g_bgm.wave      = wv;
    g_bgm.adpcmPos  = 0;
    g_bgm.samplePos = 0;
    g_bgm.yn1       = 0;
    g_bgm.yn2       = 0;
    g_bgm.active    = 1;

    fprintf(stderr,
            "[bgm] playing '%s' from '%s': %u samples @ %u Hz "
            "(loop %u-%u)\n",
            member, fsysPath,
            (unsigned)wv.numSamples, (unsigned)wv.sampleRate,
            (unsigned)wv.loopStart,  (unsigned)wv.loopEnd);

    return 1;
}

void PCPortBGM_Update(void) {
    const uint8_t *adpcmBase;
    uint32_t       samplesLeft;
    uint32_t       batchSamples;
    uint32_t       si;

    if (!g_bgm.active || !g_bgm.poolBlob) {
        return;
    }

    adpcmBase = g_bgm.poolBlob + g_bgm.wave.adpcmOffset;

    /* How many samples remain before end of ADPCM data? */
    {
        uint32_t blocksUsed = g_bgm.adpcmPos / 8u;
        uint32_t samplesDecoded = blocksUsed * 14u;
        samplesLeft = (g_bgm.wave.numSamples > samplesDecoded)
                      ? (g_bgm.wave.numSamples - samplesDecoded)
                      : 0u;
    }

    if (samplesLeft == 0u) {
#if BGM_LOOP
        /* Loop: reset decode position and re-seed yn1/yn2 from loop context */
        g_bgm.adpcmPos  = 0;
        g_bgm.samplePos = 0;
        g_bgm.yn1 = g_bgm.wave.loopYn1;
        g_bgm.yn2 = g_bgm.wave.loopYn2;
        samplesLeft = g_bgm.wave.numSamples;
#else
        g_bgm.active = 0;
        return;
#endif
    }

    batchSamples = BGM_DECODE_BATCH_SAMPLES;
    if (batchSamples > samplesLeft) {
        batchSamples = samplesLeft;
    }
    if (batchSamples == 0u) {
        return;
    }

    /* Decode mono batch */
    thp_adpcm_decode(adpcmBase + g_bgm.adpcmPos,
                     g_bgm.wave.coeffs,
                     &g_bgm.yn1, &g_bgm.yn2,
                     batchSamples,
                     g_bgm.pcmBatch);

    /* Advance ADPCM byte position */
    {
        uint32_t blocksNeeded = (batchSamples + 13u) / 14u;
        g_bgm.adpcmPos += blocksNeeded * 8u;
        if (g_bgm.adpcmPos > g_bgm.wave.adpcmBytes) {
            g_bgm.adpcmPos = g_bgm.wave.adpcmBytes;
        }
    }
    g_bgm.samplePos += batchSamples;

    /* Duplicate mono to stereo interleave */
    for (si = 0; si < batchSamples; ++si) {
        g_bgm.stereoBatch[si * 2u + 0u] = g_bgm.pcmBatch[si];
        g_bgm.stereoBatch[si * 2u + 1u] = g_bgm.pcmBatch[si];
    }

    /* Submit to waveOut -- drops silently if device is full */
    WaveOutSink_Submit(g_bgm.stereoBatch, batchSamples);
}

void PCPortBGM_Stop(void) {
    g_bgm.active = 0;
    if (g_bgm.poolBlob) {
        free(g_bgm.poolBlob);
        g_bgm.poolBlob = NULL;
        g_bgm.poolSize = 0;
    }
    memset(&g_bgm.wave, 0, sizeof(g_bgm.wave));
    g_bgm.adpcmPos  = 0;
    g_bgm.samplePos = 0;
    g_bgm.yn1 = 0;
    g_bgm.yn2 = 0;
}

int PCPortBGM_IsPlaying(void) {
    return g_bgm.active ? 1 : 0;
}

#endif /* __MWERKS__ */
