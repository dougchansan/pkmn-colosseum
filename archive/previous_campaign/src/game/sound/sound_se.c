/**
 * @file sound_se.c
 * @brief Sound effect playback for Pokemon Colosseum.
 *
 * Manages sound effect (SE) playback including:
 *   - 3D positioned sound effects using the GS engine's Vec3 system
 *   - Volume queue system for area transition sound blending
 *   - Per-work-entry volume fade control
 *   - 4-point 3D listener placement for surround sound
 *
 * The SE system supports two modes of 3D audio:
 *   - Simple 3D: position + direction with inner cone angle
 *   - Full 4-point: position + forward + up + right for surround
 *
 * SE entries are tracked in SndWork with the isActive flag (bit 2)
 * and use the SndResData volume queue for smooth transitions.
 *
 * Address range: 0x80166098 - 0x80166670 (approx.)
 * Source file:   sound.c (same TU as the core sound functions)
 */

#include "game/sound/sound.h"

/* =========================================================================
 * External engine functions
 * =========================================================================
 */

/* GS Vec3 operations */
extern void fn_800E01F4(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E01D0(void* dst, void* src);
extern void fn_800E0168(void* dst, void* src, void* offset);

/* Sound 3D / listener layer */
extern void* fn_80167864(void);   /* _sndAllocListener */
extern void* fn_801678E4(void);   /* _sndAllocListener4Point */
extern void  fn_801677BC(void*);  /* _sndReleaseListener */
extern void  fn_801677F4(void*);  /* _sndReleaseListener2 */

/* JAudio 3D sound setup */
extern s32  fn_8015ECA8(void* dst, void* pos, void* dir, u32 innerAngle,
                        u16 pad, u8 volume, u32 pan, u32 flags);
extern BOOL fn_8015E890(void* jAudioParams);
extern s32  fn_8015ED00(void* dst, void* pos, void* srcPos,
                        void* srcDir, void* srcUp, u8 volume, u32 flags);
extern s32  fn_8015EF04(void* dst, void* pos, void* fwd, void* up,
                        void* right, u8 isOmni, u8 volume, u32 pan, u32 flags);

/* Internal sound volume control */
extern u32  fn_8016737C(void* entry, u32 fadeTime, u32 flags);

/* Forward declarations for static functions used before their definitions */
static void _sndTriggerVolumeEntry(u32 sndId, u32 volume);
static void _sndQueueVolumeEntry(u32 sndId, u32 volume, u32 fadeTime);

/* =========================================================================
 * SDA constants from sdata2 section (float pool)
 * =========================================================================
 */
/* lbl_8047D540 = 1.0f */
/* lbl_8047D560 = 0.0f (or small value for default positioning) */
/* lbl_8047D564 = (3D default forward distance) */
/* lbl_8047D568 = (3D default up offset) */
/* lbl_8047D56C = (3D default right offset) */
/* lbl_8047D570 = (3D default volume scale) */
/* lbl_8047D574 = (3D default max distance) */

/* =========================================================================
 * fn_801660D8 -- sndTriggerVolumeChange
 *
 * Iterates all active sound work entries and triggers a queued volume
 * change. Used when transitioning between areas to blend BGM/SE volumes.
 *
 * The function checks each work entry's flags:
 *   - If isBGM and playBgm is set, triggers the volume change
 *   - If not isBGM and playSe is set, triggers the volume change
 *
 * Each trigger pops the next value from the SndResData volume queue
 * and applies it via fn_80166670 (_sndCheckSndWorkAll).
 *
 * r3 = volume, r4 = playSe, r5 = playBgm
 * =========================================================================
 */
void sndTriggerVolumeChange(u32 volume, BOOL playSe, BOOL playBgm) {
    u32 i;
    u32 offset;

    offset = 0;
    for (i = 0; i < g_sndWorkCount; i++) {
        u8* basePtr = (u8*)g_sndWorkTable;
        u8 flags = basePtr[offset];

        /* Check isActive (bit 2) */
        if ((flags >> 3) & 1) {
            /* Check isBGM (bit 1) */
            if ((flags >> 1) & 1) {
                /* This is a BGM entry -- only trigger if playBgm */
                if (playBgm) {
                    _sndTriggerVolumeEntry(i, volume);
                }
            } else {
                /* This is an SE entry -- only trigger if playSe */
                if (playSe) {
                    _sndTriggerVolumeEntry(i, volume);
                }
            }
        }

        offset += 0x0C;
    }
}

/* =========================================================================
 * fn_80166168 -- _sndTriggerVolumeEntry
 *
 * Pops one entry from the SndResData volume queue and applies the
 * resulting volume change to the sound via _sndCheckSndWorkAll.
 *
 * The volume queue (volQueue[3]) is a LIFO stack with volQueueCount
 * tracking the current depth. Each pop decrements the count and uses
 * the value at volQueue[count-1] as the new volume for that sound.
 *
 * r3 = sndId (used as loop index), r4 = volume (passed to fn_80166670)
 * =========================================================================
 */
static void _sndTriggerVolumeEntry(u32 sndId, u32 volume) {
    u8* entry;
    SndResData* resData;
    u8 queueCount;
    u8 newVolume;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    resData = *(SndResData**)(entry + 0x08);

    if (resData == NULL) {
        return;
    }

    queueCount = resData->volQueueCount;
    if (queueCount == 0) {
        return;
    }

    /* Pop the next volume from the queue */
    queueCount--;
    resData->volQueueCount = queueCount;
    newVolume = resData->volQueue[queueCount];

    /* Apply the volume change */
    _sndCheckSndWorkAll(sndId, newVolume, 0);
}

/* =========================================================================
 * fn_801661D0 -- sndQueueVolumeChange
 *
 * Iterates all active sound work entries and queues a volume change.
 * Unlike sndTriggerVolumeChange, this pushes values into the queue
 * rather than immediately applying them.
 *
 * r3 = volume, r4 = fadeTime, r5 = playSe, r6 = playBgm
 * =========================================================================
 */
void sndQueueVolumeChange(u32 volume, u32 fadeTime, u32 playSe,
                          u32 playBgm) {
    u32 i;
    u32 offset;

    offset = 0;
    for (i = 0; i < g_sndWorkCount; i++) {
        u8* basePtr = (u8*)g_sndWorkTable;
        u8 flags = basePtr[offset];

        /* Check isActive (bit 2) */
        if ((flags >> 3) & 1) {
            /* Check isBGM (bit 1) */
            if ((flags >> 1) & 1) {
                if (playBgm) {
                    _sndQueueVolumeEntry(i, fadeTime, volume);
                }
            } else {
                if (playSe) {
                    _sndQueueVolumeEntry(i, fadeTime, volume);
                }
            }
        }

        offset += 0x0C;
    }
}

/* =========================================================================
 * fn_80166268 -- _sndQueueVolumeEntry
 *
 * Pushes a volume value onto a SndResData's volume queue.
 * The queue has a maximum depth of 3 (SND_VOLQUEUE_MAX).
 *
 * If the requested volume exceeds the resource's max volume, it is
 * clamped to maxVolume.
 *
 * r3 = sndId, r4 = volume, r5 = fadeTime
 * =========================================================================
 */
static void _sndQueueVolumeEntry(u32 sndId, u32 volume, u32 fadeTime) {
    u8* entry;
    SndResData* resData;
    u8 queueCount;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    resData = *(SndResData**)(entry + 0x08);

    if (resData == NULL) {
        return;
    }

    queueCount = resData->volQueueCount;

    /* Queue is full (max 3 entries) */
    if (queueCount >= SND_VOLQUEUE_MAX) {
        return;
    }

    /* Clamp volume to maxVolume */
    if (volume > resData->maxVolume) {
        volume = resData->maxVolume;
    }

    /* Push the volume onto the queue */
    resData->volQueue[queueCount] = volume;

    /* Increment queue count (but cap at 2 for the next push) */
    if (queueCount < 2) {
        resData->volQueueCount = queueCount + 1;
    }

    /* Apply the volume change now */
    _sndCheckSndWorkAll(sndId, fadeTime, 0);
}

/* =========================================================================
 * fn_80166308 -- sndPlaySe3D
 *
 * Plays a sound effect positioned in 3D space.
 *
 * Sets up default listener parameters (forward direction with a small
 * offset), then calls the full 3D play function.
 *
 * r3 = sndId, r4 = pos (Vec3*)
 * =========================================================================
 */
s32 sndPlaySe3D(u32 sndId, void* pos) {
    SndVec defaultDir;
    s32 result;

    /* Set default forward direction (lbl_8047D560 = 0.0f-ish value) */
    fn_800E01F4(&defaultDir, 0.0f, 0.0f, 0.0f);

    /* Play with default volume (0x7F) and no pan */
    result = sndPlaySe3DFull(sndId, pos, &defaultDir, SND_VOLUME_MAX, 0);

    return result;
}

/* =========================================================================
 * fn_80166370 -- sndPlaySe3DFull
 *
 * Full 3D sound effect playback with position, direction, and volume.
 *
 * Allocates a SndListener from the listener pool, configures the
 * JAudio 3D parameters, and starts playback. If the JAudio start
 * fails, the listener is released.
 *
 * r3 = sndId, r4 = pos, r5 = dir, r6 = volume, r7 = pan
 * =========================================================================
 */
s32 sndPlaySe3DFull(u32 sndId, void* pos, void* dir, u8 volume, u8 pan) {
    u8* workEntry;
    u16 workParam;
    SndListener* listener;

    workEntry = (u8*)g_sndWorkTable + (sndId * 0x0C);

    /* Allocate a listener slot */
    listener = (SndListener*)fn_80167864();
    if (listener == NULL) {
        return 0;
    }

    /* Read the work parameter (halfword at offset 0x04) */
    workParam = *(u16*)(workEntry + 0x04);

    /* Configure JAudio 3D parameters */
    if (fn_8015ECA8(
            listener->jAudioParams,  /* offset 0x28 in listener */
            pos,
            dir,
            0x1F,                    /* inner cone angle */
            workParam,
            volume,
            pan,
            0) == 0) {
        /* JAudio rejected the parameters -- release listener */
        fn_801677BC(listener);
        return 0;
    }

    /* Copy position and direction into the listener */
    fn_800E01D0(&listener->pos, pos);
    fn_800E01D0(&listener->dir, dir);

    /* Store metadata */
    listener->volume = volume;
    listener->field_02 = pan;
    *(f32*)((u8*)listener + 0x08) = *(f32*)pos;  /* falloff from pos.x */
    *(f32*)((u8*)listener + 0x0C) = *(f32*)dir;  /* maxDist from dir.x */
    listener->sndWorkIndex = sndId;

    /* Start playback */
    if (fn_8015E890(listener->jAudioParams) == 0) {
        return (s32)listener;
    }

    /* Playback failed -- release */
    fn_801677BC(listener);
    return 0;
}

/* =========================================================================
 * fn_80166458 -- sndUpdateSe3D
 *
 * Updates a playing 3D sound effect's listener position.
 * Recalculates the JAudio 3D mix based on the new position.
 *
 * r3 = listener, r4 = pos
 * =========================================================================
 */
BOOL sndUpdateSe3D(SndListener* listener, void* pos) {
    SndVec interpPos;

    if (listener == NULL) {
        return FALSE;
    }

    /* Interpolate position */
    fn_800E0168(&interpPos, &listener->pos, pos);

    /* Update JAudio 3D mix */
    if (fn_8015ED00(
            (u8*)listener + 0x40,    /* JAudio update buffer */
            pos,
            &interpPos,
            (u8*)listener + 0x28,    /* jAudioParams */
            (u8*)listener + 0x34,    /* jAudioParams + 0x0C */
            listener->volume,
            0) == 0) {
        return FALSE;
    }

    /* Update stored position */
    fn_800E01D0(&listener->pos, pos);
    fn_800E01D0(&listener->dir, &interpPos);

    return TRUE;
}

/* =========================================================================
 * fn_801664F0 -- sndPlaySe (omnidirectional at origin)
 *
 * Plays an omnidirectional sound effect at the listener's position.
 * Uses 4-point listener setup with default vectors.
 *
 * r3 = listenerParams (output buffer on stack)
 * =========================================================================
 */
s32 sndPlaySe(void* listenerParams) {
    SndVec origin;
    SndVec forward;
    SndVec up;

    /* Origin at (0, 0, 0) with default values from sdata2 */
    fn_800E01F4(&origin, 0.0f, 0.0f, 0.0f);
    fn_800E01F4(&forward, 0.0f, 0.0f, 0.0f);  /* lbl_8047D568 for z */
    fn_800E01F4(&up, 0.0f, 0.0f, 0.0f);        /* lbl_8047D56C for y */

    return sndPlaySe3D4Point(
        (u32)listenerParams,
        &origin,
        &forward,
        &up,
        NULL,  /* right vector auto-calculated */
        SND_VOLUME_MAX);
}

/* =========================================================================
 * fn_80166578 -- sndPlaySe3D4Point
 *
 * Full 4-point 3D sound effect playback for surround sound.
 *
 * Allocates a 4-point listener, configures the JAudio surround
 * parameters, and starts playback.
 *
 * r3 = sndId, r4 = pos, r5 = fwd, r6 = up, r7 = right
 * r8 (from stack) = volume
 * =========================================================================
 */
s32 sndPlaySe3D4Point(u32 sndId, void* pos, void* fwd, void* up,
                       void* right, u8 volume) {
    SndListener* listener;

    listener = (SndListener*)fn_801678E4(); /* alloc 4-point listener */
    if (listener == NULL) {
        return 0;
    }

    /* Configure 4-point JAudio parameters */
    if (fn_8015EF04(
            (u8*)listener + 0x40,    /* JAudio 4-point buffer */
            pos,
            fwd,
            up,
            right,
            1,                       /* isOmni = TRUE */
            volume,
            0,
            0) == 0) {
        /* JAudio rejected -- release listener */
        fn_801677F4(listener);
        return 0;
    }

    /* Copy vectors into listener */
    fn_800E01D0(&listener->pos, pos);
    fn_800E01D0(&listener->dir, fwd);
    fn_800E01D0((u8*)listener + 0x28, up);
    fn_800E01D0((u8*)listener + 0x34, right);

    /* Store volume and metadata */
    *(f32*)((u8*)listener + 0x04) = *(f32*)pos;
    *(f32*)((u8*)listener + 0x08) = *(f32*)fwd;
    *(f32*)((u8*)listener + 0x0C) = *(f32*)up;
    listener->volume = volume;

    return 0;
}

/* =========================================================================
 * fn_801652DC -- _sndUpdateAllVolumes
 *
 * Per-frame volume update for all active sound channels.
 * Iterates through the three channel types (0=BGM, 1=SE, 2=stream)
 * and applies volume adjustments based on scene parameters.
 *
 * For channel 0 (BGM): if the scene has a volume multiplier != 0,
 *   scales the volume accordingly before applying.
 *
 * r3 = bgmHandle, r4 = seHandle, r5 = streamHandle, r6 = params
 * =========================================================================
 */
void _sndUpdateAllVolumes(u32 bgmHandle, u32 seHandle, u32 streamHandle,
                          void* params) {
    u32 channel;
    f32 volumeMultiplier;

    for (channel = 0; channel < 3; channel++) {
        switch (channel) {
            case 0: {
                /* BGM channel: check volume multiplier */
                f32* paramsFloat = (f32*)params;
                volumeMultiplier = paramsFloat[0x6A]; /* offset 0x1A8 */

                if (volumeMultiplier != 0.0f) {
                    f32 scaledVol = 1.0f * volumeMultiplier;
                    f32 invVol = 1.0f - scaledVol;
                    fn_80164C40(bgmHandle, seHandle, scaledVol, invVol);
                }

                fn_80164DD0(bgmHandle, params, 0);
                break;
            }
            case 1:
                fn_80164DD0(seHandle, params, 1);
                break;
            case 2:
                fn_80164DD0(streamHandle, params, 2);
                break;
        }
    }
}
