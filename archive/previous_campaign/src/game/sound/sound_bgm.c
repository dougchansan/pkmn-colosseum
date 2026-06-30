/**
 * @file sound_bgm.c
 * @brief Background music control for Pokemon Colosseum.
 *
 * Manages BGM playback state including:
 *   - Status queries (playing, fading, stopped)
 *   - Volume fade control for BGM transitions
 *   - BGM transpose (key change) for scene-specific music variants
 *   - Master volume with per-category filtering
 *   - Auto-volume adjustment during disc drive pauses
 *
 * The BGM system distinguishes between two categories:
 *   - Non-streaming BGM (SND_CATEGORY_SE / 0x407): loaded entirely into RAM
 *   - Streaming BGM (SND_CATEGORY_BGM / 0x406): read from disc in chunks
 *
 * Current BGM tracking:
 *   g_currentBgmId       (lbl_8047B0A8) -- non-streaming BGM handle
 *   g_currentStreamBgmId (lbl_8047B0AC) -- streaming BGM handle
 *
 * Address range: 0x80166670 - 0x80166E88
 * Source file:   sound.c (these functions are part of the same TU)
 */

#include "game/sound/sound.h"

/* =========================================================================
 * External engine / SDK functions
 * =========================================================================
 */

/* GS debug print */
extern void fn_800DD970(const char* fmt, ...);

/* GS engine frame wait */
extern void _threadSwitch(void);

/* JAudio: get handle status */
extern s32 fn_8014D598(u32 handle);
extern BOOL fn_8014D5C8(u32 handle);
extern void fn_8014D648(u32 handle);
extern void fn_8014D6D8(u32 channel, u16 value, u32 handle, u32 flags);
extern s32 fn_8014D880(u32 handle);
extern void fn_8014D8C8(u8 volume, u16 pan, u8 code);
extern void fn_8014D928(u8 chorus, u16 reverb, u8 delay, u8 wet);
extern void fn_8014D9BC(void);
extern s32  fn_800A0E34(void);
extern void fn_800A0EB4(u32 enable);

/* Sound internal functions */
extern u32  fn_8016737C(void* entry, u32 fadeTime, u32 flags);
extern void fn_80167070(u32 sndId, u32 mode);
extern s32  fn_80167408(u32 sndId, u32 volume);
extern void fn_80167490(void* entry, u32 fadeTime, u32 volume);
extern void fn_80167508(void* entry, u32 fadeTime, u32 volume);
extern void fn_8016758C(u32 sndId, u32 r4);
extern void fn_8016761C(void* entry, u32 fadeTime, u32 volume);
extern void fn_80166C34(u32 groupId);
extern void fn_80166AB8(u32 sndId, u32 fadeTime, u32 volume);
extern void fn_801669E4(u32 sndId, u32 fadeTime, u32 volume);
extern void sndAuxCallbackUpdateSettingsReverbHI(void* buffer);
extern void fn_80167A9C(u32 groupId);
extern u32  fn_80167768(u32 channel, u32 category);
extern BOOL fn_8015FFD4(void);
extern BOOL fn_8015FFA0(void);

/* =========================================================================
 * fn_80166670 -- _sndCheckSndWorkAll
 *
 * Validates a sound work entry and triggers cleanup/transition.
 *
 * Checks the isBGM and isActive flags. If valid, calls the internal
 * work checker (fn_8016737C).
 *
 * r3 = sndId, r4 = fadeTime, r5 = flags
 * =========================================================================
 */
u32 _sndCheckSndWorkAll(u32 sndId, u32 fadeTime, u32 flags) {
    u8* entry;
    u8 entryFlags;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    entryFlags = entry[0];

    /* Check isBGM flag (bit 1) -- if set, proceed */
    if ((entryFlags >> 1) & 1) {
        /* Already a BGM */
    } else {
        /* Check isActive flag (bit 2) */
        if (((entryFlags >> 3) & 1) == 0) {
            return 0; /* not active, nothing to do */
        }
    }

    /* Call the internal sound work checker */
    return fn_8016737C(entry, fadeTime, flags);
}

/* =========================================================================
 * fn_801666BC -- GSsndGetStatus
 *
 * Queries the playback status of a sound work entry.
 * If a sound is stuck (handle expired but work still active), forces
 * termination and prints the appropriate warning.
 *
 * Returns:
 *   0 = stopped / invalid
 *   1 = initializing
 *   2 = playing
 *   3 = fading
 *
 * r3 = sndId
 * =========================================================================
 */
s32 GSsndGetStatus(u32 sndId) {
    u8* entry;
    u8 flags;
    SndResData* resData;
    u32 handle;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    flags = entry[0];

    /* Check isActive flag (bit 2) */
    if (((flags >> 3) & 1) == 0) {
        return 0; /* not active */
    }

    /* Get resource data */
    resData = *(SndResData**)(entry + 0x08);
    if (resData == NULL) {
        return 0;
    }

    /* Check isBGM flag (bit 1) */
    if ((flags >> 1) & 1) {
        /* BGM path: check JAudio handle */
        handle = resData->handle;

        if ((handle + 0x10000) != 0xFFFF) {
            /* Handle is valid -- check if JAudio considers it active */
            s32 jStatus = fn_8014D880(handle);
            if ((jStatus + 0x10000) != 0xFFFF) {
                return 2; /* still playing */
            }
        }

        /* BGM handle expired -- force terminate */
        fn_800DD970("GSsndGetStatus:Forced termination SE=%d\n", sndId);
        fn_80167070(sndId, 1);
        return 0;
    }

    /* Check isFading flag (bit 3) */
    if ((flags >> 4) & 1) {
        return 3; /* fading */
    }

    /* SE path: check JAudio handle */
    handle = resData->handle;

    if ((handle + 0x10000) != 0xFFFF) {
        s32 seStatus = fn_8014D598(handle);
        if (seStatus) {
            return 2; /* still playing */
        }
    }

    /* SE handle expired -- force terminate */
    fn_800DD970("GSsndGetStatus:Forced termination BGM=%d\n", sndId);
    fn_80167070(sndId, 1);
    return 0;
}

/* =========================================================================
 * fn_801667D8 -- _sndBgmTranspose
 *
 * Performs a BGM key change / re-trigger on a BGM-type sound.
 *
 * r3 = sndId, r4 = transpose
 * =========================================================================
 */
BOOL _sndBgmTranspose(u32 sndId, u16 transpose) {
    u8* entry;
    u8 flags;
    SndResData* resData;
    u32 handle;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    flags = entry[0];

    /* Must not be a BGM-flagged entry (bit 1) */
    if ((flags >> 1) & 1) {
        return FALSE;
    }

    /* Must be active (bit 2) */
    if (((flags >> 3) & 1) == 0) {
        return FALSE;
    }

    /* Get resource data */
    resData = *(SndResData**)(entry + 0x08);
    if (resData == NULL) {
        return FALSE;
    }

    /* Check handle validity */
    handle = resData->handle;
    if ((handle + 0x10000) == 0xFFFF) {
        return FALSE;
    }

    /* Must not be fading (bit 3) */
    if (((flags >> 4) & 1) == 0) {
        return FALSE;
    }

    /* Change group if needed */
    _sndChangeGroup(entry[2]);

    /* Special case: sndId == 1 means just acknowledge the change */
    if (sndId == 1) {
        return TRUE;
    }

    /* Perform the actual transpose via JAudio */
    fn_8014D648(handle);
    fn_8014D6D8(resData->maxVolume, transpose, resData->handle, 0);

    /* Clear fading flag (bit 3) */
    {
        u8 newFlags = entry[0];
        newFlags &= ~(1 << 4); /* clear bit 3 in the packed byte */
        entry[0] = newFlags;
    }

    return TRUE;
}

/* =========================================================================
 * fn_801668DC -- _sndSeTranspose
 *
 * Performs a key change on an SE-type sound. Similar to _sndBgmTranspose
 * but for non-BGM entries.
 *
 * r3 = sndId, r4 = transpose, r5 = volume
 * =========================================================================
 */
BOOL _sndSeTranspose(u32 sndId, u32 transpose, u32 volume) {
    u8* entry;
    u8 flags;
    SndResData* resData;
    u32 handle;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    flags = entry[0];

    /* Must be a BGM-flagged entry (bit 1) */
    if ((flags >> 1) & 1) {
        return FALSE;
    }

    /* Must be active (bit 2) */
    if (((flags >> 3) & 1) == 0) {
        return FALSE;
    }

    /* Get resource data */
    resData = *(SndResData**)(entry + 0x08);
    if (resData == NULL) {
        return FALSE;
    }

    handle = resData->handle;
    if ((handle + 0x10000) == 0xFFFF) {
        return FALSE;
    }

    /* Must not be fading (bit 3) */
    if (((flags >> 4) & 1) == 0) {
        return FALSE;
    }

    /* Special case: sndId == 1 */
    if (sndId == 1) {
        return TRUE;
    }

    /* Perform the transpose */
    fn_8014D6D8(0, transpose, handle, 0);
    fn_8014D5C8(handle);

    /* Set fading flag (bit 3) */
    {
        u8 newFlags = entry[0];
        newFlags |= (1 << 4);
        entry[0] = newFlags;
    }

    return TRUE;
}

/* =========================================================================
 * fn_801669BC -- (wrapper) set fade target defaults for BGM
 *
 * Calls _sndSetFadeTarget(sndId, 0, 0).
 * =========================================================================
 */
void sndSetFadeTargetDefault(u32 sndId) {
    fn_801669E4(sndId, 0, 0);
}

/* =========================================================================
 * fn_801669E4 -- _sndSetFadeTarget
 *
 * Sets the fade target volume for a sound work entry.
 *
 * r3 = sndId, r4 = fadeTime, r5 = volume
 * =========================================================================
 */
static void _sndSetFadeTarget(u32 sndId, u32 fadeTime, u32 volume) {
    u8* entry;
    u8 flags;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    flags = entry[0];

    /* Check isBGM flag (bit 1) */
    if ((flags >> 1) & 1) {
        fn_80167490(entry, fadeTime, volume);
    } else {
        fn_80167508(entry, fadeTime, volume);
    }
}

/* =========================================================================
 * fn_80166A28 -- (wrapper) set fade target defaults for SE
 *
 * Calls fn_80166AB8(sndId, 0, 0).
 * =========================================================================
 */
void sndSetFadeTargetDefault2(u32 sndId) {
    fn_80166AB8(sndId, 0, 0);
}

/* =========================================================================
 * fn_80166A50 -- _sndStartPlayback
 *
 * Starts playback of a sound by first applying the initial volume
 * (via fn_80167408), then calling the internal start function.
 *
 * r3 = sndId, r4 = fadeTime, r5 = volume, r6 = flags
 * =========================================================================
 */
static void _sndStartPlayback(u32 sndId, u32 fadeTime, u32 volume,
                               u32 flags) {
    /* Apply initial volume */
    if (fn_80167408(sndId, volume) == 0) {
        return; /* volume set failed */
    }

    /* Start the actual playback */
    fn_80166AB8(sndId, fadeTime, flags);
}

/* =========================================================================
 * fn_80166AB8 -- _sndStartPlaybackInternal
 *
 * Internal playback start. Routes to the BGM or SE playback path
 * based on the isBGM flag in the SndWork entry.
 *
 * r3 = sndId, r4 = fadeTime, r5 = volume
 * =========================================================================
 */
static void _sndStartPlaybackInternal(u32 sndId, u32 fadeTime, u32 volume) {
    u8* entry;
    u8 flags;

    entry = (u8*)g_sndWorkTable + (sndId * 0x0C);
    flags = entry[0];

    /* Check isBGM flag (bit 1) */
    if ((flags >> 1) & 1) {
        fn_8016758C(sndId, sndId);  /* BGM play path */
    } else {
        /* Check isActive (bit 2) */
        if (((flags >> 3) & 1) == 0) {
            return; /* not active */
        }
        fn_8016761C(entry, fadeTime, volume); /* SE play path */
    }
}

/* =========================================================================
 * fn_80166B18 -- _sndReleaseWork
 *
 * Releases a sound work entry by calling the internal cleanup with mode=1.
 *
 * r3 = sndId
 * =========================================================================
 */
void _sndReleaseWork(u32 sndId) {
    fn_80167070(sndId, 1);
}

/* =========================================================================
 * fn_801662E8 -- _sndFindCurrentHandle
 *
 * Finds the currently active sound handle for a given channel/category.
 * Thin wrapper around the internal find function.
 *
 * r3 = channel, r4 = category
 * =========================================================================
 */
u32 _sndFindCurrentHandle(u32 channel, u32 category) {
    return fn_80167768(channel, category);
}

/* =========================================================================
 * fn_80166C34 -- _sndChangeGroup
 *
 * Switches the active sound group. If the group differs from the current
 * one, loads the new group's data from the FSYS archive.
 *
 * r3 = groupId (0 = no change)
 * =========================================================================
 */
void _sndChangeGroup(u32 groupId) {
    if (groupId != 0 && g_sndCurrentGroup != groupId) {
        fn_80167A9C(groupId);
        sndAuxCallbackUpdateSettingsReverbHI((void*)0x80452500);
    }
}

/* =========================================================================
 * fn_80166C74 -- sndIsAudioAvailable
 *
 * Checks whether audio output is available by querying the disc drive
 * status (fn_800A0E34).
 *
 * Returns TRUE if audio is ready, FALSE otherwise.
 * =========================================================================
 */
BOOL sndIsAudioAvailable(void) {
    s32 driveStatus;

    driveStatus = fn_800A0E34();

    switch (driveStatus) {
        case 0:
            return FALSE;
        case 1:
            return TRUE;
        default:
            return FALSE;
    }
}

/* =========================================================================
 * fn_80166CC0 -- sndSetSurroundMode
 *
 * Configures the audio output mode (mono/stereo/surround).
 * Calls JAudio's DSP configuration to apply the mode.
 *
 * r3 = mode (0=mono, 1=stereo, 2=surround)
 * =========================================================================
 */
void sndSetSurroundMode(s32 mode) {
    fn_8014D9BC(); /* reset DSP state */

    if (mode == 0) {
        /* Mono: disable surround processing */
        fn_800A0EB4(FALSE);
    } else if (mode > 0 && mode < 3) {
        /* Stereo or Surround: enable surround processing */
        fn_800A0EB4(TRUE);
    }
}

/* =========================================================================
 * fn_80166D18 -- sndSetDspMix
 *
 * Sets the DSP effect mix parameters (chorus, reverb, delay, wet/dry).
 * Values are masked to their expected widths before passing to JAudio.
 *
 * r3 = chorus (u8), r4 = reverb (u16), r5 = delay (u8), r6 = wet (u8)
 * =========================================================================
 */
void sndSetDspMix(u8 chorus, u16 reverb, u8 delay, u8 wet) {
    fn_8014D928(chorus & 0xFF, reverb & 0xFFFF, delay & 0xFF, wet & 0xFF);
}

/* =========================================================================
 * fn_80166D48 -- sndSetMasterVolume
 *
 * Sets the master volume for all active sound work entries.
 *
 * Iterates through all SndWork entries. Based on the apply flags,
 * selects which categories to modify:
 *   applyBgm=1, applySe=0: code 0xFF -- all categories
 *   applyBgm=1, applySe=1: code 0xFD -- BGM only
 *   applyBgm=0, applySe=1: code 0xFE -- SE only
 *
 * After iterating work entries, calls the JAudio master volume function.
 *
 * r3 = volume, r4 = panValue, r5 = applyBgm, r6 = applySe
 * =========================================================================
 */
void sndSetMasterVolume(u32 volume, u32 panValue, BOOL applyBgm,
                        BOOL applySe) {
    u8 code;
    u32 i;
    u32 offset;
    u8 flags;

    /* Determine the master volume command code */
    if (applyBgm && applySe) {
        code = SND_MASTER_ALL;      /* 0xFF */
    } else if (applyBgm && applySe == 0) {
        code = SND_MASTER_BGM_ONLY; /* 0xFD */
    } else if (applyBgm == 0 && applySe) {
        code = SND_MASTER_SE_ONLY;  /* 0xFE */
    } else {
        return; /* nothing to apply */
    }

    /* Iterate all active work entries */
    offset = 0;
    for (i = 0; i < g_sndWorkCount; i++) {
        u8* basePtr = (u8*)g_sndWorkTable;
        flags = basePtr[offset];

        /* Check isActive (bit 2) */
        if ((flags >> 3) & 1) {
            /* Apply filter based on code */
            int skip = 0;
            if (code == SND_MASTER_BGM_ONLY) {
                /* BGM only: skip if isBGM flag is set */
                if ((flags >> 1) & 1) {
                    skip = 1;
                }
            } else if (code == SND_MASTER_SE_ONLY) {
                /* SE only: skip if isBGM flag is NOT set */
                if (((flags >> 1) & 1) == 0) {
                    skip = 1;
                }
            }
            /* Apply the volume change */
            if (skip == 0) {
                fn_80167408(i, volume);
            }
        }
        offset += 0x0C;
    }

    /* Apply to the JAudio master mixer */
    fn_8014D8C8(volume & 0xFF, panValue & 0xFFFF, code);
}

/* =========================================================================
 * fn_80166E44 -- sndAutoVolumeOnDiscPause
 *
 * Called when the disc drive enters a pause state (between loads).
 * Applies full master volume to both BGM and SE to prevent audio
 * glitches during disc seeks.
 * =========================================================================
 */
void sndAutoVolumeOnDiscPause(void) {
    if (fn_8015FFD4()) {
        sndSetMasterVolume(0, 0, TRUE, TRUE);
        fn_8015FFA0();
    }
}
