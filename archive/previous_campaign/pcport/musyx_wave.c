/**
 * @file musyx_wave.c
 * @brief MusyX .sdir+.samp single-wave lookup + playback. See musyx_wave.h.
 *
 * Repackaged from the parallel MusyX-parse worktree agent (branch 47be2727) as a
 * standalone module (renamed symbols) so it does not collide with bgm_host.c.
 * Verified data (snd_se_motion): sr=44100, 7917 samples, 4528 ADPCM bytes,
 * coeffs at sdir[+0x30], samp = raw DSP-ADPCM at offset 0.
 */
#ifdef __MWERKS__
/* GCN build: host MusyX decode/stream not applicable. */
#else

#include "musyx_wave.h"
#include "thp_audio.h"          /* thp_adpcm_decode() */
#include "real_content_host.h"  /* PCPort_LoadFsysMember / PCPort_FreeBuffer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* The MusyX LZSS decompressor pre-fills its 4096-byte window with 0x20 (ASCII
 * space), so back-references into the pre-fill emit 0x20. A 0x20 byte in a
 * decompressed sdir/pool blob therefore maps to the true binary 0x00. */
static uint8_t mw_tz(uint8_t b) { return (b == 0x20u) ? 0u : b; }

static uint16_t mw_be16(const uint8_t* p) {
    return (uint16_t)(((uint16_t)mw_tz(p[0]) << 8) | (uint16_t)mw_tz(p[1]));
}
static uint32_t mw_be32(const uint8_t* p) {
    return ((uint32_t)mw_tz(p[0]) << 24) | ((uint32_t)mw_tz(p[1]) << 16) |
           ((uint32_t)mw_tz(p[2]) <<  8) |  (uint32_t)mw_tz(p[3]);
}
static int16_t mw_be_s16(const uint8_t* p) { return (int16_t)mw_be16(p); }

#define MW_SDIR_STRIDE        0x20u
#define MW_SDIR_OFF_SFXID     0x00u
#define MW_SDIR_OFF_RATE      0x0Eu
#define MW_SDIR_OFF_COUNT     0x10u
#define MW_SDIR_OFF_POOLOFF   0x1Cu
#define MW_SDIR_OFF_COEFFS    0x30u
#define MW_SDIR_COEFF_COUNT   14u

#define MW_POOL_STRIDE        0x28u
#define MW_POOL_OFF_LOOPINFO  0x0Cu
#define MW_POOL_OFF_LOOPEND   0x10u
#define MW_POOL_OFF_LOOPSTART 0x14u
#define MW_POOL_OFF_YN1       0x1Cu
#define MW_POOL_OFF_YN2       0x1Eu
#define MW_POOL_LOOPFLAG      0x01u

int MusyX_ParseSdir(const uint8_t* sdirData, uint32_t sdirSize,
                    const uint8_t* poolData, uint32_t poolSize,
                    MusyXWave* outInfo) {
    const uint8_t* entry;
    uint32_t pool_off, samp_cnt, adpcm_blocks, loop_info, i;
    uint16_t sr, loop_end_raw;

    if (sdirData == NULL || outInfo == NULL) return 0;
    if (sdirSize < MW_SDIR_STRIDE) {
        fprintf(stderr, "musyx: sdir too small (%u bytes)\n", sdirSize);
        return 0;
    }
    entry = sdirData;  /* first entry at offset 0 */

    sr       = mw_be16(entry + MW_SDIR_OFF_RATE);
    samp_cnt = mw_be32(entry + MW_SDIR_OFF_COUNT);
    pool_off = mw_be32(entry + MW_SDIR_OFF_POOLOFF);
    if (sr == 0u || samp_cnt == 0u) {
        fprintf(stderr, "musyx: sdir entry has zero rate/count\n");
        return 0;
    }
    adpcm_blocks = (samp_cnt + 13u) / 14u;

    memset(outInfo, 0, sizeof(*outInfo));
    outInfo->sampleRate  = (uint32_t)sr;
    outInfo->numSamples  = samp_cnt;
    outInfo->adpcmOffset = 0u;
    outInfo->adpcmSize   = adpcm_blocks * 8u;
    outInfo->sfxId       = mw_be16(entry + MW_SDIR_OFF_SFXID);

    if (sdirSize >= MW_SDIR_OFF_COEFFS + MW_SDIR_COEFF_COUNT * 2u) {
        for (i = 0u; i < MW_SDIR_COEFF_COUNT; ++i)
            outInfo->coeffs[i] = mw_be_s16(entry + MW_SDIR_OFF_COEFFS + i * 2u);
    }

    if (poolData != NULL && poolSize > pool_off + MW_POOL_STRIDE) {
        const uint8_t* desc = poolData + pool_off;
        loop_info = mw_be32(desc + MW_POOL_OFF_LOOPINFO);
        outInfo->hasLoop   = (loop_info & MW_POOL_LOOPFLAG) ? 1u : 0u;
        outInfo->loopStart = mw_be32(desc + MW_POOL_OFF_LOOPSTART);
        loop_end_raw       = mw_be16(desc + MW_POOL_OFF_LOOPEND);
        outInfo->loopEnd   = (loop_end_raw == 0xFFFFu) ? 0u : (uint32_t)loop_end_raw;
        outInfo->yn1       = mw_be_s16(desc + MW_POOL_OFF_YN1);
        outInfo->yn2       = mw_be_s16(desc + MW_POOL_OFF_YN2);
    }
    return 1;
}

#if defined(_WIN32)
#include "waveout_sink.h"

static int s_mw_sink_open = 0;

int MusyX_PlayWave(const char* fsysPath, const char* sdirMember,
                   const char* poolMember, const char* sampPath) {
    u8 *sdir_blob = NULL, *pool_blob = NULL;
    u32 sdir_size = 0u, pool_size = 0u;
    uint8_t* samp_blob = NULL;
    long samp_len = 0;
    int16_t* pcm_buf = NULL;
    MusyXWave info;
    FILE* fp = NULL;
    int ok = 0;

    if (fsysPath == NULL || sdirMember == NULL || sampPath == NULL) return 0;

    if (!PCPort_LoadFsysMember(fsysPath, sdirMember, &sdir_blob, &sdir_size)) {
        fprintf(stderr, "musyx: failed to load sdir '%s' from '%s'\n",
                sdirMember, fsysPath);
        goto cleanup;
    }
    if (poolMember != NULL) {
        if (!PCPort_LoadFsysMember(fsysPath, poolMember, &pool_blob, &pool_size)) {
            fprintf(stderr, "musyx: pool '%s' not found -- no loop data\n", poolMember);
        }
    }
    if (!MusyX_ParseSdir(sdir_blob, sdir_size, pool_blob, pool_size, &info)) {
        fprintf(stderr, "musyx: failed to parse sdir first wave\n");
        goto cleanup;
    }
    fprintf(stderr, "musyx: sfxId=0x%04X sr=%u samp=%u adpcm_sz=%u loop=%d\n",
            info.sfxId, info.sampleRate, info.numSamples, info.adpcmSize,
            (int)info.hasLoop);

    fp = fopen(sampPath, "rb");
    if (fp == NULL) { fprintf(stderr, "musyx: cannot open samp '%s'\n", sampPath); goto cleanup; }
    fseek(fp, 0, SEEK_END);
    samp_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (samp_len <= 0 || (uint32_t)samp_len < info.adpcmOffset + info.adpcmSize) {
        fprintf(stderr, "musyx: samp too small (%ld, need %u+%u)\n",
                samp_len, info.adpcmOffset, info.adpcmSize);
        goto cleanup;
    }
    samp_blob = (uint8_t*)malloc((size_t)samp_len);
    if (samp_blob == NULL) goto cleanup;
    if (fread(samp_blob, 1u, (size_t)samp_len, fp) != (size_t)samp_len) {
        fprintf(stderr, "musyx: read error on samp\n"); goto cleanup;
    }
    fclose(fp); fp = NULL;

    pcm_buf = (int16_t*)malloc((size_t)info.numSamples * sizeof(int16_t));
    if (pcm_buf == NULL) goto cleanup;
    {
        int16_t yn1 = info.yn1, yn2 = info.yn2;
        thp_adpcm_decode(samp_blob + info.adpcmOffset, info.coeffs, &yn1, &yn2,
                         info.numSamples, pcm_buf);
    }

    if (!WaveOutSink_Open(info.sampleRate, 1 /* mono */, 16)) {
        fprintf(stderr, "musyx: WaveOutSink_Open failed\n"); goto cleanup;
    }
    s_mw_sink_open = 1;
    if (!WaveOutSink_Submit(pcm_buf, info.numSamples)) {
        fprintf(stderr, "musyx: WaveOutSink_Submit failed\n"); goto cleanup;
    }
    ok = 1;

cleanup:
    if (fp != NULL) fclose(fp);
    if (samp_blob != NULL) free(samp_blob);
    if (pcm_buf != NULL) free(pcm_buf);
    if (pool_blob != NULL) PCPort_FreeBuffer(pool_blob);
    if (sdir_blob != NULL) PCPort_FreeBuffer(sdir_blob);
    return ok;
}

void MusyX_StopWave(void) {
    if (s_mw_sink_open) { WaveOutSink_Close(); s_mw_sink_open = 0; }
}

#else /* !_WIN32 */

int MusyX_PlayWave(const char* fsysPath, const char* sdirMember,
                   const char* poolMember, const char* sampPath) {
    (void)fsysPath; (void)sdirMember; (void)poolMember; (void)sampPath;
    return 0;
}
void MusyX_StopWave(void) {}

#endif /* _WIN32 */
#endif /* __MWERKS__ */
