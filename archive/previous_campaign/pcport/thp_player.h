/**
 * @file thp_player.h
 * @brief Host-side THP movie decoder (PC port).
 *
 * The GameCube THP boot/opening movies (movie/*.thp) are a simple container of
 * per-frame baseline JPEGs (plus optional ADPCM audio, ignored here). The game's
 * own THP player is GC-target pseudo-register asm/C that does not build for the
 * host, so this is a from-scratch host reimplementation (Path A): parse the 0x50
 * big-endian header, walk the frame chain, and decode each frame's JPEG to RGBA8
 * via stb_image. Frames are streamed from disk (never the whole file at once --
 * the opening/autodemo/staffroll movies are 64-109 MB).
 *
 * Decoding is decoupled from presentation: this module returns RGBA pixels; the
 * caller uploads + draws them through the existing 2D quad path.
 */
#ifndef PCPORT_THP_PLAYER_H
#define PCPORT_THP_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PCPortTHP PCPortTHP;

/** Open a THP movie for streamed playback. Returns NULL on failure. */
PCPortTHP* PCPortTHP_Open(const char* path);

/** Close a THP movie and free its resources (safe on NULL). */
void PCPortTHP_Close(PCPortTHP* thp);

int   PCPortTHP_Width(const PCPortTHP* thp);
int   PCPortTHP_Height(const PCPortTHP* thp);
int   PCPortTHP_FrameCount(const PCPortTHP* thp);
float PCPortTHP_Fps(const PCPortTHP* thp);

/**
 * Decode the next video frame to RGBA8 (Width*Height*4, opaque alpha).
 *
 * @param thp     the movie.
 * @param outRGBA receives a pointer to the decoded pixels, owned by the THP and
 *                valid until the next call or PCPortTHP_Close.
 * @return 1 on success, 0 at end-of-stream or on decode error.
 */
int PCPortTHP_NextFrameRGBA(PCPortTHP* thp, const unsigned char** outRGBA);

/* --- Audio (THP DSP-ADPCM boot-movie sound) ------------------------------ */

/** @return 1 if the movie carries an audio component, else 0. */
int      PCPortTHP_HasAudio(const PCPortTHP* thp);
/** @return audio sample rate in Hz (e.g. 32000), or 0 if no audio. */
unsigned PCPortTHP_AudioSampleRate(const PCPortTHP* thp);
/** @return audio channel count (1 or 2), or 0 if no audio. */
int      PCPortTHP_AudioChannels(const PCPortTHP* thp);
/** @return total decoded sample frames across the movie, or 0 if no audio. */
unsigned PCPortTHP_AudioTotalSamples(const PCPortTHP* thp);

/**
 * Decode the current frame's audio block to interleaved s16 PCM.
 *
 * Must be called for the SAME frame as PCPortTHP_NextFrameRGBA (it reads the
 * audio block that follows the video block in the current frame header). The
 * returned buffer is owned by the THP and valid until the next call/close.
 *
 * @param thp           the movie.
 * @param outPCM        receives a pointer to interleaved samples (L R L R ...).
 * @param outNumFrames  receives the per-channel sample-frame count.
 * @return 1 on success, 0 if no audio, end-of-stream, or a parse error.
 */
int PCPortTHP_NextFrameAudioPCM(PCPortTHP* thp, const short** outPCM,
                                unsigned* outNumFrames);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_THP_PLAYER_H */
