/**
 * @file waveout_sink.h
 * @brief Win32 waveOut streaming audio sink (PC port THP playback backend).
 *
 * Zero-dependency double-buffered PCM streaming via the legacy waveOut API
 * (-lwinmm). Configured for THP audio: 32000Hz, 16-bit, stereo. The THP frame
 * loop submits a decoded PCM block per video frame; the sink recycles WAVEHDR
 * buffers as the device finishes them (CALLBACK_EVENT).
 */
#ifndef PCPORT_WAVEOUT_SINK_H
#define PCPORT_WAVEOUT_SINK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Open the waveOut device for streaming.
 *
 * @param sampleRate    samples per second (e.g. 32000).
 * @param channels      channel count (1 or 2).
 * @param bitsPerSample bits per sample (16).
 * @return 1 on success, 0 on failure (no device / busy / bad format).
 */
int WaveOutSink_Open(uint32_t sampleRate, int channels, int bitsPerSample);

/**
 * Submit one interleaved PCM block for playback. Blocks (with a timeout) until a
 * buffer is free, then queues the data. Safe to call when the sink is not open
 * (returns 0).
 *
 * @param pcmData   interleaved samples (L R L R ... for stereo).
 * @param numFrames number of sample frames (per-channel sample count).
 * @return 1 on success, 0 if not open or on a queueing error.
 */
int WaveOutSink_Submit(const int16_t* pcmData, uint32_t numFrames);

/** @return 1 if any submitted buffer is still playing, else 0. */
int WaveOutSink_IsPlaying(void);

/** Stop playback and release the device (safe if not open). */
void WaveOutSink_Close(void);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_WAVEOUT_SINK_H */
