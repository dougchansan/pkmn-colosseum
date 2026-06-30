/**
 * @file musyx_wave.h
 * @brief MusyX .sdir + .samp single-wave lookup and playback (PC port).
 *
 * The Colosseum audio system uses three files per group:
 *   .sdir  -- wave metadata: sfxId, sampleRate, sampleCount, poolOffset, and
 *             (single-entry sdirs) 14 DSP-ADPCM coefficients at [+0x30].
 *   .pool  -- runtime descriptor table: GC ARAM addresses + loop metadata
 *             (loop start/end, yn1/yn2). Does NOT hold the raw ADPCM samples.
 *   .samp  -- raw GC DSP-ADPCM, no header, offset 0 = first sample. The .samp
 *             lives in the disc's sound/ directory as a plain file (not FSYS).
 *
 * The .sdir/.pool live inside an FSYS archive (e.g. common.fsys). This module
 * plays the FIRST valid sdir entry. Full MusyX sequencer / multi-track BGM and
 * multi-entry ARAM-delta indexing are out of scope (single-entry groups like
 * snd_se_motion play correctly). Verified layout (snd_se_motion, 2026-06-04):
 *   sdir stride 0x20; [+0x00]u16 sfxId, [+0x0E]u16 sampleRate (0xAC44=44100),
 *   [+0x10]u32 sampleCount, [+0x1C]u32 poolOffset, [+0x30] s16[14] coeffs.
 *   LZSS quirk: the decompressor pre-fills its window with 0x20, so a 0x20 byte
 *   in a decompressed sdir/pool maps to binary 0x00 (corrected on read).
 *
 * Derived from the parallel MusyX-parse worktree agent (branch 47be2727),
 * repackaged as a standalone module so it does not collide with bgm_host.c.
 */
#ifndef PCPORT_MUSYX_WAVE_H
#define PCPORT_MUSYX_WAVE_H

#ifndef __MWERKS__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Wave info resolved from a .sdir (+ optional .pool for loop metadata). */
typedef struct {
    uint32_t sampleRate;
    uint32_t numSamples;
    uint32_t adpcmOffset;     /* byte offset into the .samp (0 for first entry) */
    uint32_t adpcmSize;       /* ceil(numSamples/14)*8 */
    int16_t  coeffs[16];
    int16_t  yn1;
    int16_t  yn2;
    uint32_t loopStart;
    uint32_t loopEnd;
    uint8_t  hasLoop;
    uint16_t sfxId;
} MusyXWave;

/* Parse the first valid sdir entry. poolData may be NULL (loop metadata only). */
int MusyX_ParseSdir(const uint8_t* sdirData, uint32_t sdirSize,
                    const uint8_t* poolData, uint32_t poolSize,
                    MusyXWave* outInfo);

/* Load sdir+pool members from fsysPath, decode the first wave from sampPath
 * (raw DSP-ADPCM), and stream it once via WaveOutSink. 1 on success. */
int MusyX_PlayWave(const char* fsysPath, const char* sdirMember,
                   const char* poolMember, const char* sampPath);

/* Close the WaveOut device if MusyX_PlayWave opened it. */
void MusyX_StopWave(void);

#ifdef __cplusplus
}
#endif

#endif /* !__MWERKS__ */
#endif /* PCPORT_MUSYX_WAVE_H */
