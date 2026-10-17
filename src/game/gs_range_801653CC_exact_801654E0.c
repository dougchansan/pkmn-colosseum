/**
 * @file gs_range_801653CC_exact_801654E0.c
 * @brief Byte-exact gs-engine range, 0x801654E0 - 0x80165D0C.
 */
#include "game/gs_range_801653CC_shared.h"

u32 fn_801654E0(u32 id, void* buffer, u32 size)
{
    void* wave;
    u32 waveIndex = lbl_80478FAC[id].waveIndex;

    if (waveIndex < *lbl_80478E30) {
        wave = lbl_80478E34[waveIndex].data;
    } else {
        wave = 0;
    }

    if (wave == 0) {
        return 0;
    }
    fn_80165EE0(wave, buffer, size);
    return 1;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (dont_inline on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma dont_inline on
void soundResumeMusic(u32 id)
{
    if (id != (u32)-1) {
        soundResumeSong(id, 0x3E8, 0);
    }
}

u32 soundPauseMusic(void)
{
    u32 id = fn_801662E8(0, 0x406);

    if (id != (u32)-1) {
        soundPauseSong(id, 0x32, 0);
        soundWaitMs(0x32);
    }
    return id;
}
#pragma dont_inline reset

void fn_801655D4(u32 index)
{
    GSsndDvdWave* wave;

    if (*lbl_80478FB0 <= index) {
        GSlogWrite(lbl_80273548, index);
    }

    wave = &((GSsndDvdWave*)lbl_80478FB4)[index];
    if (!GSsndOpenWaveDVD(index, wave->fileId, 0, wave->unk4, wave->unk8, wave->unkC, wave->unk10)) {
        GSlogWrite(lbl_80273568, wave->fileId);
    }
}

u32 std130_soundUnk_801653cc(u32 id, void* buffer, u32 size)
{
    u32 savedId = soundPauseMusic();
    u32 result = soundPlayFanfare(id, buffer, size);

    soundResumeMusic(savedId);
    return result;
}

u32 fn_801656D8(u32 id)
{
    return fn_80166098(id);
}

u32 std137_soundStopAll(u32 id, u32 arg1, u32 arg2)
{
    return soundSetVolume(id, arg1, arg2);
}

u32 std109_soundPlayEffectVariant(u32 volume, u32 fade)
{
    soundMasterVolume(volume, fade, 1, 0);
    return 1;
}

u32 std108_soundIsAnyPlaying(void)
{
    GSvec position;
    u32 result;

    set__5GSvecFfff(&position, lbl_8047D540, lbl_8047D540, lbl_8047D540);
    result = fn_801664F0(&position);
    return (result | -result) >> 31;
}

u32 fn_80165788(u32 id, f32 x, f32 y, f32 z)
{
    GSvec position;
    u32 result;

    set__5GSvecFfff(&position, x, y, z);
    result = fn_80166308(id, &position);
    result = (result | -result) >> 31;
    return result;
}

void scriptSoundStop(u32 fade)
{
    soundStop(lbl_8047B0A8, fade);
}

u32 soundStop(u32 id, u32 fade)
{
    s32 restartSe;

    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    restartSe = 0;
    if (lbl_8047B0AC == id) {
        lbl_8047B0AC = 0;
        restartSe = 1;
    }

    if (lbl_8047B0A8 == id) {
        lbl_8047B0A8 = 0;
        restartSe = 0;
    }

    if (!(u8)fn_80166084(id)) {
        soundFadeSlot(fade, restartSe, 1);
        if (restartSe != 0) {
            GSlogWrite(lbl_8027358C, id);
            return soundPlay(1, 0, 0xFF, restartSe);
        }
        soundKill(id);
    } else {
        soundStopRaw(id, fade, 0);
        soundKill(id);
    }
}

u32 fn_801658FC(u32 id, u32 fade, u32 volume, u32 priority)
{
    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    soundFadeSlot(fade, 0, 1);
    return soundPlay(id, volume, priority, 0);
}

u32 std138_soundPlayEffectEx(u32 id, u32 fade, u32 volume, u32 priority)
{
    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    soundFadeSlot(fade, 1, 1);
    return soundPlay(id, volume, priority, 1);
}

u32 std105_soundPlayEffect(u32 id, u32 volume, u32 priority)
{
    return soundPlay(id, volume, priority, 0);
}

u32 std104_soundSetBackgroundMusic(u32 id, u32 volume, u32 priority)
{
    return soundPlay(id, volume, priority, 1);
}

u32 soundPlay(u32 id, u32 volume, u32 priority, s32 kind)
{
    u32 flags;
    u32 waveIndex;
    u32 currentId;
    void* wave;

    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    flags = lbl_80478FAC[id].flags;
    if (((flags >> 7) & 1U) == 1U) {
        if (kind == 0) {
            s32 status = GSsndGetStatus(id);

            if (status != 0 && status >= 0 && status < 4) {
                soundSetVolume(id, 0x32, 0);
                soundWaitMs(0x32);
                soundKill(id);
            }
            lbl_8047B0A8 = id;
        }
        return soundStart(id, volume, priority, 0);
    }

    {
        u32 resourceId;
        u32 resourceSize;

        if (kind != 0) {
            resourceId = 0x406;
            resourceSize = 0x10000;
        } else {
            resourceId = 0x407;
            resourceSize = 0x2000;
        }

        waveIndex = lbl_80478FAC[id].waveIndex;
        if (waveIndex >= *lbl_80478E30) {
            GSlogWrite(lbl_802735C4, id);
            return 0;
        }

        currentId = fn_801662E8(0, resourceId);
        if (currentId != id) {
            if (currentId != (u32)-1) {
                s32 status = GSsndGetStatus(currentId);

                if (status != 0 && status >= 0 && status < 4) {
                    soundSetVolume(currentId, 0x32, 0);
                    soundWaitMs(0x32);
                    soundKill(currentId);
                }
            }

            wave = lbl_80478E34[waveIndex].data;
            if (wave != 0) {
                fn_80165EE0(wave, GSresGetResource(0, resourceId), resourceSize);
            } else {
                wave = lbl_80478E34[waveIndex].archive;
                if (wave != 0) {
                    fn_80165DEC(wave, GSresGetResource(0, resourceId), resourceSize);
                } else {
                    GSlogWrite(lbl_802735F8, id);
                    return 0;
                }
            }
            soundAttachWork(id, 0, resourceId);
        }

        if (kind != 0) {
            lbl_8047B0AC = id;
        }
        if (kind == 0) {
            lbl_8047B0A8 = id;
        }
        return soundStart(id, volume, priority, 0);
    }
}

void soundFadeSlot(u32 volume, u32 isSe, s32 wait)
{
    u32 resourceId = isSe != 0 ? 0x406 : 0x407;
    u32 id = fn_801662E8(0, resourceId);

    if (id != (u32)-1) {
        s32 status = GSsndGetStatus(id);

        if (status != 0 && status >= 0 && status < 4) {
            soundSetVolume(id, volume, 0);
            if (wait != 0) {
                soundWaitMs(volume);
            }
        }
    }
}
