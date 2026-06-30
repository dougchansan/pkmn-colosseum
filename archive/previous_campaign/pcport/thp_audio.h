/**
 * @file thp_audio.h
 * @brief GameCube DSP-ADPCM decode for THP boot-movie audio (PC port).
 *
 * THP audio is self-contained GC DSP-ADPCM (NOT JAudio). Each per-frame audio
 * block carries the coefficient table and yn1/yn2 seed per channel ("Layout A":
 * grouped headers then grouped data). This module is the pure decode kernel plus
 * a WAV verifier; THP framing lives in thp_player.c and playback in waveout_sink.c.
 *
 * DSP-ADPCM block format: every 8 bytes = 1 header byte + 7 data bytes. The
 * header byte = (predIdx << 4) | scaleShift. The 7 data bytes = 14 nibbles
 * (high nibble first), each a signed 4-bit residual. Decode:
 *   sample = clamp16((nibble << scaleShift)
 *                    + ((coeff[predIdx*2]*yn1 + coeff[predIdx*2+1]*yn2 + 1024) >> 11))
 *   yn2 = yn1; yn1 = sample;
 */
#ifndef PCPORT_THP_AUDIO_H
#define PCPORT_THP_AUDIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decode one channel of GC DSP-ADPCM into 16-bit PCM.
 *
 * @param adpcm      packed ADPCM bytes (8-byte blocks: 1 header + 7 data).
 * @param coeffs     16 s16 coefficients (8 predictor pairs).
 * @param yn1        decoder history sample n-1 (seed from the frame header).
 * @param yn2        decoder history sample n-2 (seed from the frame header).
 * @param numSamples number of PCM samples to produce.
 * @param outPCM     receives numSamples mono s16 PCM samples.
 *
 * The final yn1/yn2 are written back through @p yn1 / @p yn2 (for optional
 * cross-frame continuity; THP supplies fresh seeds each frame, so callers
 * normally re-seed and ignore these).
 */
void thp_adpcm_decode(const uint8_t* adpcm, const int16_t coeffs[16],
                      int16_t* yn1, int16_t* yn2,
                      uint32_t numSamples, int16_t* outPCM);

/**
 * Write interleaved s16 PCM to a standard RIFF/WAVE file (PCM16 LE).
 *
 * @param path       output .wav path.
 * @param pcm        interleaved samples (L R L R ... for stereo).
 * @param numFrames  number of sample frames (per-channel sample count).
 * @param numChannels channel count (1 or 2).
 * @param sampleRate samples per second (e.g. 32000).
 * @return 1 on success, 0 on failure.
 */
int thp_audio_write_wav(const char* path, const int16_t* pcm,
                        uint32_t numFrames, int numChannels,
                        uint32_t sampleRate);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_THP_AUDIO_H */
