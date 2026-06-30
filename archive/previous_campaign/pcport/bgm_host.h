/**
 * @file bgm_host.h
 * @brief Background-music playback module for the Colosseum PC port.
 *
 * Locates music data inside FSYS container archives, decompresses LZSS
 * members, and streams decoded DSP-ADPCM PCM audio through the existing
 * waveout_sink backend.
 *
 * -------------------------------------------------------------------------
 * What is implemented
 * -------------------------------------------------------------------------
 *  - FSYS container parsing (header, name table, per-member LZSS headers).
 *  - LZSS decompression (ring-buffer variant used by GC FSYS containers).
 *  - DSP-ADPCM decode reused from thp_audio.c (same 8-byte block, 14
 *    samples/block format).
 *  - Streaming playback via WaveOutSink (waveout_sink.h) at the sample
 *    rate carried inside each wave header.
 *  - PCPortBGM_Init / PCPortBGM_PlayFromFsys / PCPortBGM_Update /
 *    PCPortBGM_Stop public API.
 *
 * -------------------------------------------------------------------------
 * What is NOT implemented (scaffolded / stubbed)
 * -------------------------------------------------------------------------
 *  The music data in common.fsys (snd_music_proj, snd_music_sdir,
 *  snd_music_pool, snd_music_atmos_*) and in per-area .fsys files uses
 *  the proprietary Nintendo AX / MusyX sequenced-music format:
 *    - .proj  sequence/pattern data   (MusyX project file)
 *    - .sdir  wave-bank directory     (maps wave IDs to pool offsets)
 *    - .pool  raw DSP-ADPCM samples   (wave pool blob)
 *
 *  A full MusyX runtime would need:
 *    1. .proj parser to schedule note events.
 *    2. .sdir to resolve wave IDs -> pool byte offsets + coefficient tables.
 *    3. Per-voice DSP-ADPCM decoder + mixer (we have the ADPCM decoder).
 *    4. AX effect pipeline (reverb, chorus).
 *
 *  Items 1, 2, and 4 are not implemented here.  The playback path in
 *  PCPortBGM_PlayFromFsys will extract and decompress the pool member,
 *  then play the FIRST wave sample it finds using a best-effort heuristic
 *  parse of the pool header (see bgm_host.c for details and what the
 *  heuristic checks).  For most music tracks this will produce an
 *  audible but not necessarily musically-correct result (one instrument
 *  voice rather than a full orchestrated sequence).
 *
 *  TODO (future work):
 *    - Implement MusyX .sdir parser to enumerate all wave entries.
 *    - Implement MusyX .proj sequencer to drive note scheduling.
 *    - Mix multiple voices for full-fidelity reproduction.
 *    - Alternatively, extract all audio to OGG/WAV offline and play those.
 *
 * @note Compile guard: entire file is a no-op when __MWERKS__ is defined,
 *       so the GCN build never sees it.
 */
#ifndef PCPORT_BGM_HOST_H
#define PCPORT_BGM_HOST_H

#ifndef __MWERKS__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * Initialise the BGM subsystem.
 *
 * Opens the waveOut device (32000 Hz stereo s16) via WaveOutSink_Open.
 * Safe to call multiple times; subsequent calls are no-ops if already open.
 *
 * @return 1 on success, 0 if the audio device could not be opened.
 */
int PCPortBGM_Init(void);

/**
 * Locate a named member inside an FSYS archive and start streaming it.
 *
 * The function:
 *   1. Opens @p fsysPath, parses the FSYS header and name table.
 *   2. Finds the member named @p member.
 *   3. LZSS-decompresses it into a heap buffer.
 *   4. Attempts a best-effort parse of the DSP-ADPCM wave pool to find
 *      the first valid wave sample (see bgm_host.h limitation notes).
 *   5. Decodes the DSP-ADPCM samples and begins streaming via WaveOutSink.
 *
 * @param fsysPath   Host path to the .fsys file
 *                   (e.g. "orig/GC6E01/disc/files/common.fsys").
 * @param member     Null-terminated name of the member to play
 *                   (e.g. "snd_music_pool").
 * @return  1 if streaming started, 0 on any error (file not found, member
 *          not found, decode failure, audio device not open, etc.).
 */
int PCPortBGM_PlayFromFsys(const char *fsysPath, const char *member);

/**
 * Per-frame update pump.
 *
 * Call once per game frame (or once per ~16 ms) to keep the waveOut
 * buffer filled.  When the current decoded block is exhausted the
 * function decodes the next ADPCM block and submits it.  If playback
 * is looping (BGM_LOOP default = 1) it wraps back to the start of the
 * wave sample data.
 *
 * Safe to call when BGM is stopped -- becomes a no-op.
 */
void PCPortBGM_Update(void);

/**
 * Stop the currently playing BGM and release all buffers.
 *
 * After this call PCPortBGM_Update is a no-op until the next
 * PCPortBGM_PlayFromFsys succeeds.  Does NOT close the waveOut device;
 * call WaveOutSink_Close() explicitly at program exit if needed.
 */
void PCPortBGM_Stop(void);

/**
 * Query whether BGM is currently playing.
 *
 * @return 1 if playing, 0 if stopped or not initialised.
 */
int PCPortBGM_IsPlaying(void);

#ifdef __cplusplus
}
#endif

#endif /* !__MWERKS__ */
#endif /* PCPORT_BGM_HOST_H */
