/**
 * @file audio_shim.c
 * @brief JAudio2 replacement -- SDL2_mixer audio backend stubs.
 *
 * References:
 *   - docs/pc_port_design.md Section 9 (Audio Replacement)
 *   - audio_shim.h for full API documentation
 *
 * Phase 3 PC port scaffolding -- skeleton only.
 * This file targets PC compilers; under MWCC (GCN) it compiles as empty.
 */

#ifdef __MWERKS__
/* GCN build: pcport shim not applicable */
#else

#include "audio_shim.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* TODO: Include SDL2 headers when build system is ready
 * #include <SDL2/SDL.h>
 * #include <SDL2/SDL_mixer.h>
 */

/* =========================================================================
 * Internal state
 * ========================================================================= */

/** Per-channel state tracking */
static AudioChannel g_channels[AUDIO_MAX_CHANNELS];

/** Number of allocated channels */
static s32 g_numAllocated = 0;

/** Global per-frame update callback (from the game's sound system) */
static void (*g_updateCallback)(void) = 0;

/** Audio system initialized flag */
static BOOL g_audioInitialized = 0;

/* =========================================================================
 * Initialization / Shutdown
 * ========================================================================= */

BOOL JAudio_Init(void) {
    memset(g_channels, 0, sizeof(g_channels));
    for (s32 i = 0; i < AUDIO_MAX_CHANNELS; i++) {
        g_channels[i].index = i;
        g_channels[i].sdlChannel = -1;
        g_channels[i].state = 0;
    }
    g_numAllocated = 0;
    g_audioInitialized = 1;

    printf("[audio_shim] JAudio_Init stub -- SDL2 audio init goes here\n");
    return 1;
}

void JAudio_Shutdown(void) {
    memset(g_channels, 0, sizeof(g_channels));
    g_numAllocated = 0;
    g_audioInitialized = 0;

    printf("[audio_shim] JAudio_Shutdown stub\n");
}

/* =========================================================================
 * Channel management
 * ========================================================================= */

s32 JAudio_AllocChannel(u16 category) {
    (void)category;
    for (s32 i = 0; i < AUDIO_MAX_CHANNELS; i++) {
        if (g_channels[i].state == 0 && g_channels[i].sdlChannel == -1) {
            g_channels[i].category = category;
            g_channels[i].sdlChannel = i;
            g_numAllocated++;
            return i;
        }
    }
    return -1;
}

void JAudio_FreeChannel(s32 channel) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;

    g_channels[channel].state = 0;
    g_channels[channel].sdlChannel = -1;
    g_channels[channel].volume = 0;
    g_channels[channel].callback = 0;
    g_channels[channel].mixChunk = 0;
    if (g_numAllocated > 0) g_numAllocated--;
}

void JAudio_ReleaseChannel(s32 channel) {
    JAudio_FreeChannel(channel);
}

/* =========================================================================
 * Playback control
 * ========================================================================= */

s32 JAudio_StartPlayback(s32 channel, void* data, u32 dataSize, BOOL loop) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return -1;

    (void)data; (void)dataSize;

    g_channels[channel].state = 1; /* playing */
    g_channels[channel].looping = loop ? 1 : 0;
    return 0;
}

void JAudio_Stop(s32 channel) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;
    g_channels[channel].state = 0;
}

BOOL JAudio_IsPlaying(s32 channel) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return 0;
    return g_channels[channel].state == 1;
}

void JAudio_Pause(s32 channel) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;
    if (g_channels[channel].state == 1)
        g_channels[channel].state = 2; /* paused */
}

void JAudio_Resume(s32 channel) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;
    if (g_channels[channel].state == 2)
        g_channels[channel].state = 1; /* playing */
}

void JAudio_SetVolume(s32 channel, u8 volume) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;
    g_channels[channel].volume = volume;
}

void JAudio_SetCallback(s32 channel, void (*callback)(s32)) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;
    g_channels[channel].callback = callback;
}

void JAudio_SetParams(s32 channel, u8 volume, u8 pan, u16 pitch) {
    if (channel < 0 || channel >= AUDIO_MAX_CHANNELS) return;
    g_channels[channel].volume = volume;
    g_channels[channel].pan = pan;
    (void)pitch;
}

void JAudio_SetDSPMix(u8 chorus, u16 reverb, u8 delay, u8 wet) {
    (void)chorus; (void)reverb; (void)delay; (void)wet;
}

void JAudio_Flush(void) {
}

void JAudio_RegisterUpdate(void (*callback)(void)) {
    g_updateCallback = callback;
}

/* =========================================================================
 * ADPCM decode
 * ========================================================================= */

s32 JAudio_DecodeADPCM(const void* adpcmData, u32 adpcmSize,
                       const s16 coeffs[16],
                       s16* outPCM, u32 outSize) {
    (void)adpcmData; (void)adpcmSize; (void)coeffs;
    (void)outPCM; (void)outSize;
    return -1; /* Not implemented */
}

void* JAudio_CreateChunk(const s16* pcmData, u32 numSamples,
                         u32 sampleRate, u8 channels) {
    (void)pcmData; (void)numSamples; (void)sampleRate; (void)channels;
    return (void*)0;
}

void JAudio_FreeChunk(void* chunk) {
    (void)chunk;
}

/* =========================================================================
 * Streaming BGM
 * ========================================================================= */

s32 JAudio_StartStream(const char* filePath, BOOL loop) {
    (void)filePath; (void)loop;
    printf("[audio_shim] JAudio_StartStream stub: %s (loop=%d)\n",
           filePath ? filePath : "(null)", loop);
    return -1;
}

void JAudio_StopStream(void) {
}

void JAudio_SetStreamVolume(u8 volume) {
    (void)volume;
}

BOOL JAudio_IsStreamPlaying(void) {
    return 0;
}

#endif /* __MWERKS__ */
