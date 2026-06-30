/**
 * @file audio_shim.h
 * @brief JAudio2 replacement -- SDL2_mixer audio backend.
 *
 * Replaces the ~20 JAudio2 DSP functions called by the sound system
 * (sound.c, sound_bgm.c, sound_se.c) with SDL2_mixer equivalents.
 *
 * The game's sound system (66 functions, fully decompiled) uses JAudio2
 * through a well-defined interface. This shim intercepts at the JAudio2
 * boundary, translating channel alloc/free/play/stop/volume operations
 * to SDL_mixer calls.
 *
 * References:
 *   - docs/pc_port_design.md Section 9 (Audio Replacement)
 *   - include/game/sound/sound.h (sound system API and structures)
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 */
#ifndef PCPORT_AUDIO_SHIM_H
#define PCPORT_AUDIO_SHIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Use the project's standard types */
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long    u32;
typedef signed short   s16;
typedef signed long      s32;
typedef float          f32;
typedef int            BOOL;

/* =========================================================================
 * Constants
 * ========================================================================= */

/** Maximum simultaneous audio channels (matches JAudio maxVoices=64) */
#define AUDIO_MAX_CHANNELS  64

/** Audio output parameters */
#define AUDIO_SAMPLE_RATE   48000
#define AUDIO_FORMAT        0x8010  /* AUDIO_S16SYS */
#define AUDIO_CHANNELS      2      /* Stereo */
#define AUDIO_CHUNK_SIZE    2048

/** Sound category IDs (matching SND_CATEGORY_* from sound.h) */
#define AUDIO_CATEGORY_BGM    0x0406
#define AUDIO_CATEGORY_SE     0x0407
#define AUDIO_CATEGORY_STREAM 0x0408

/* =========================================================================
 * Audio channel state
 * ========================================================================= */

typedef struct AudioChannel {
    /** Channel index (0-based) */
    s32 index;

    /** SDL_mixer channel number (-1 = unallocated) */
    s32 sdlChannel;

    /** Category (BGM, SE, STREAM) */
    u16 category;

    /** Current volume (0-127) */
    u8 volume;

    /** Pan value (0=left, 128=center, 255=right) */
    u8 pan;

    /** Playing state: 0=stopped, 1=playing, 2=paused */
    u8 state;

    /** Loop flag */
    u8 looping;

    /** Pointer to decoded audio data (Mix_Chunk*) */
    void* mixChunk;

    /** Callback function for channel completion */
    void (*callback)(s32 channel);
} AudioChannel;

/* =========================================================================
 * Public API -- JAudio2 function replacements
 *
 * These functions match the JAudio2 call sites identified in
 * pc_port_design.md Section 9.2. The game's sound.c calls these
 * through function pointers or direct calls at the listed addresses.
 * ========================================================================= */

/**
 * JAudio_Init -- Initialize the audio system.
 * Replaces: JAudio system initialization.
 * Called from: sndInit (fn_80166E88).
 *
 * SDL2 equivalent:
 *   SDL_Init(SDL_INIT_AUDIO);
 *   Mix_OpenAudio(48000, AUDIO_S16SYS, 2, 2048);
 *   Mix_AllocateChannels(64);
 */
BOOL JAudio_Init(void);

/**
 * JAudio_Shutdown -- Shut down the audio system.
 * Replaces: JAudio cleanup.
 * Called from: sndShutdown (fn_80165F84).
 *
 * SDL2 equivalent:
 *   Mix_CloseAudio();
 *   SDL_QuitSubSystem(SDL_INIT_AUDIO);
 */
void JAudio_Shutdown(void);

/**
 * JAudio_AllocChannel -- Allocate a playback channel.
 * Replaces: fn_800F9318 (JAudio channel alloc).
 *
 * SDL2 equivalent: Find a free SDL_mixer channel.
 *
 * @param category  Sound category (BGM/SE/STREAM).
 * @return          Channel handle (>=0), or -1 on failure.
 */
s32 JAudio_AllocChannel(u16 category);

/**
 * JAudio_FreeChannel -- Release a playback channel.
 * Replaces: fn_800F9210 (JAudio channel free).
 *
 * SDL2 equivalent: Mix_HaltChannel(channel).
 *
 * @param channel  Channel handle from JAudio_AllocChannel.
 */
void JAudio_FreeChannel(s32 channel);

/**
 * JAudio_ReleaseChannel -- Release channel resources.
 * Replaces: fn_800F9378 (JAudio channel release).
 *
 * @param channel  Channel handle.
 */
void JAudio_ReleaseChannel(s32 channel);

/**
 * JAudio_StartPlayback -- Begin playing audio on a channel.
 * Replaces: fn_800F9418 (JAudio start playback).
 *
 * SDL2 equivalent: Mix_PlayChannel(channel, chunk, loops).
 *
 * @param channel  Channel handle.
 * @param data     Pointer to decoded audio data.
 * @param dataSize Size of audio data in bytes.
 * @param loop     Non-zero for looping playback.
 * @return         0 on success, -1 on failure.
 */
s32 JAudio_StartPlayback(s32 channel, void* data, u32 dataSize, BOOL loop);

/**
 * JAudio_Stop -- Stop playback on a channel.
 * Replaces: fn_8014D598 (JAudio stop).
 *
 * SDL2 equivalent: Mix_HaltChannel(channel).
 *
 * @param channel  Channel handle.
 */
void JAudio_Stop(s32 channel);

/**
 * JAudio_IsPlaying -- Check if a channel is currently playing.
 * Replaces: fn_8014D5C8 (JAudio is playing).
 *
 * SDL2 equivalent: Mix_Playing(channel).
 *
 * @param channel  Channel handle.
 * @return         Non-zero if playing.
 */
BOOL JAudio_IsPlaying(s32 channel);

/**
 * JAudio_Pause -- Pause playback on a channel.
 * Replaces: fn_8014D648 (JAudio pause).
 *
 * SDL2 equivalent: Mix_Pause(channel).
 *
 * @param channel  Channel handle.
 */
void JAudio_Pause(s32 channel);

/**
 * JAudio_Resume -- Resume paused playback.
 * Replaces: fn_8014D880 (JAudio resume).
 *
 * SDL2 equivalent: Mix_Resume(channel).
 *
 * @param channel  Channel handle.
 */
void JAudio_Resume(s32 channel);

/**
 * JAudio_SetVolume -- Set the volume of a channel.
 * Replaces: fn_8014D6D8 (JAudio set volume).
 *
 * SDL2 equivalent: Mix_Volume(channel, vol * 128 / 127).
 *
 * @param channel  Channel handle.
 * @param volume   Volume level (0-127, matching JAudio range).
 */
void JAudio_SetVolume(s32 channel, u8 volume);

/**
 * JAudio_SetCallback -- Register a channel completion callback.
 * Replaces: fn_8014D8C0 (JAudio set callback).
 *
 * SDL2 equivalent: Mix_ChannelFinished() for global callback,
 * or per-channel tracking.
 *
 * @param channel   Channel handle.
 * @param callback  Function to call when playback completes.
 */
void JAudio_SetCallback(s32 channel, void (*callback)(s32));

/**
 * JAudio_SetParams -- Set channel playback parameters.
 * Replaces: fn_8014D8C8 (JAudio set params).
 *
 * @param channel  Channel handle.
 * @param volume   Volume (0-127).
 * @param pan      Pan value (0-255, 128=center).
 * @param pitch    Pitch shift (not directly supported in SDL_mixer;
 *                 approximate or ignore).
 */
void JAudio_SetParams(s32 channel, u8 volume, u8 pan, u16 pitch);

/**
 * JAudio_SetDSPMix -- Set DSP effect mix levels.
 * Replaces: fn_8014D928 (JAudio set DSP mix).
 *
 * SDL2 equivalent: Mix_RegisterEffect() for custom effects,
 * or approximate with volume/pan adjustments.
 *
 * @param chorus  Chorus effect level.
 * @param reverb  Reverb send level.
 * @param delay   Delay effect level.
 * @param wet     Wet/dry mix ratio.
 */
void JAudio_SetDSPMix(u8 chorus, u16 reverb, u8 delay, u8 wet);

/**
 * JAudio_Flush -- Flush pending audio operations.
 * Replaces: fn_8014D9BC (JAudio flush).
 *
 * SDL2 equivalent: No-op or Mix_ExpireChannel.
 */
void JAudio_Flush(void);

/**
 * JAudio_RegisterUpdate -- Register the per-frame audio update callback.
 * Replaces: fn_8014DAA8 (JAudio register update).
 *
 * SDL2 equivalent: Register an SDL audio thread callback or call
 * from the main loop's per-frame update.
 *
 * @param callback  Per-frame update function.
 */
void JAudio_RegisterUpdate(void (*callback)(void));

/* =========================================================================
 * GCN ADPCM decode
 * ========================================================================= */

/**
 * JAudio_DecodeADPCM -- Decode GCN DSP-ADPCM data to PCM16.
 *
 * @param adpcmData   Source ADPCM data.
 * @param adpcmSize   Size of ADPCM data in bytes.
 * @param coeffs      ADPCM coefficient table (16 coefficients).
 * @param outPCM      Output buffer for decoded PCM16 samples.
 * @param outSize     Size of output buffer in bytes.
 * @return            Number of decoded samples, or -1 on error.
 *
 * GCN audio resources use Nintendo's DSP-ADPCM encoding (4-bit per sample).
 * This function decodes to 16-bit signed PCM for use with SDL_mixer.
 *
 * The coefficient table is stored in the .aw / .bms audio resource headers.
 * Reference: vgmstream DSP-ADPCM decoder for algorithm details.
 */
s32 JAudio_DecodeADPCM(const void* adpcmData, u32 adpcmSize,
                       const s16 coeffs[16],
                       s16* outPCM, u32 outSize);

/**
 * JAudio_CreateChunk -- Create an SDL_mixer chunk from decoded PCM data.
 *
 * @param pcmData    PCM16 audio samples.
 * @param numSamples Number of samples.
 * @param sampleRate Sample rate in Hz (typically 32000 for GCN audio).
 * @param channels   Number of audio channels (1=mono, 2=stereo).
 * @return           Opaque pointer to the created Mix_Chunk, or NULL.
 *
 * Allocates an SDL_mixer chunk and converts the sample rate if necessary
 * (GCN typically uses 32000 Hz; SDL output is 48000 Hz).
 */
void* JAudio_CreateChunk(const s16* pcmData, u32 numSamples,
                         u32 sampleRate, u8 channels);

/**
 * JAudio_FreeChunk -- Free an SDL_mixer chunk.
 *
 * @param chunk  Chunk pointer from JAudio_CreateChunk.
 */
void JAudio_FreeChunk(void* chunk);

/* =========================================================================
 * Streaming BGM
 * ========================================================================= */

/**
 * JAudio_StartStream -- Begin streaming BGM playback.
 *
 * @param filePath   Path to the audio file (WAV/OGG/etc. extracted from FSYS).
 * @param loop       Non-zero for looping.
 * @return           0 on success, -1 on failure.
 *
 * SDL2 equivalent: Mix_LoadMUS + Mix_PlayMusic.
 * The game's 64KB streaming buffer (SND_STREAM_BUFFER_SIZE) maps to
 * SDL_mixer's built-in streaming.
 */
s32 JAudio_StartStream(const char* filePath, BOOL loop);

/**
 * JAudio_StopStream -- Stop streaming BGM playback.
 *
 * SDL2 equivalent: Mix_HaltMusic.
 */
void JAudio_StopStream(void);

/**
 * JAudio_SetStreamVolume -- Set streaming BGM volume.
 *
 * @param volume  Volume (0-127).
 *
 * SDL2 equivalent: Mix_VolumeMusic.
 */
void JAudio_SetStreamVolume(u8 volume);

/**
 * JAudio_IsStreamPlaying -- Check if streaming BGM is playing.
 *
 * @return  Non-zero if playing.
 *
 * SDL2 equivalent: Mix_PlayingMusic.
 */
BOOL JAudio_IsStreamPlaying(void);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_AUDIO_SHIM_H */
