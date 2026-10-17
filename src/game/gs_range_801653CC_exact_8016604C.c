/**
 * @file gs_range_801653CC_exact_8016604C.c
 * @brief Byte-exact gs-engine range, 0x8016604C - 0x80166D48.
 */
#include "game/gs_range_801653CC_shared.h"

void* fn_8016604C(u32 size)
{
    u32 handle = GSmemAlloc(size, 0x20);

    if ((u16)handle != 0) {
        return GSmemLock(handle);
    }
    return 0;
}

u32 fn_80166084(u32 id)
{
    return (lbl_80478FAC[id].flags & SND_FLAG_SE) >> 7;
}

u32 fn_80166098(u32 id)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 active = (entry->flags & SND_FLAG_ACTIVE) >> 5;

    if (active != 1U) {
        return 0;
    }
    if (entry->work != 0) {
        return entry->work->priority;
    }
    return 0;
}

void soundUnduck(u32 volume, u32 includeBgm, u32 includeSe)
{
    u32 offset;
    int i;

    for (i = 0, offset = 0; i < lbl_8047B0E8; offset += sizeof(GSsndEntry), i++) {
        u32 flags = *(u8*)((u8*)lbl_80478FAC + offset);

        if (((flags >> 5) & 1U) == 1U) {
            if (((flags >> 7) & 1U) == 1U) {
                if (includeSe == 0) {
                    continue;
                }
            } else if (includeBgm == 0) {
                continue;
            }
            fn_80166168(i, volume);
        }
    }
}

u32 fn_80166168(u32 id, u32 volume)
{
  u32 offset = id * (sizeof(GSsndEntry));
  u32 depth;
  int new_var;
  GSsndWork *work = *((GSsndWork **) ((((u8 *) lbl_80478FAC) + offset) + 8));
  if (work == 0)
  {
    return 0;
  }
  depth = work->stackDepth;
  if (depth == 0)
  {
    return 0;
  }
  new_var = depth - 1;
  work->stackDepth = new_var;
  return soundSetVolume(id, volume, work->volumeStack[work->stackDepth]);
}

void soundDuck(u32 limit, u32 volume, u32 includeBgm, u32 includeSe)
{
    u32 offset;
    s32 i;

    for (i = 0, offset = 0; i < lbl_8047B0E8; offset += sizeof(GSsndEntry), i++) {
        u32 flags = *(u8*)((u8*)lbl_80478FAC + offset);

        if (((flags >> 5) & 1U) == 1U) {
            if (((flags >> 7) & 1U) == 1U) {
                if (includeSe == 0) {
                    continue;
                }
            } else if (includeBgm == 0) {
                continue;
            }
            _soundDuckSlot(i, volume, limit);
        }
    }
}

u32 _soundDuckSlot(u32 id, u32 volume, u32 limit)
{
    u32 offset = id * sizeof(GSsndEntry);
    GSsndWork* work = *(GSsndWork**)((u8*)lbl_80478FAC + offset + 8);
    s32 current;
    u32 depth;

    if (work == 0) {
        return 0;
    }

    depth = work->stackDepth;
    if (depth >= 3) {
        return 0;
    }

    current = work->priority;
    if (current < (s32)limit) {
        limit = current;
    }

    work->volumeStack[depth] = current;
    depth = work->stackDepth;
    if (depth < 2) {
        work->stackDepth = depth + 1;
    }

    return soundSetVolume(id, volume, limit);
}

u32 fn_801662E8(u32 arg0, u32 arg1)
{
    return soundFindOwner(arg0, arg1);
}

u32 fn_80166308(u32 id, const GSvec* position)
{
    GSvec velocity;

    set__5GSvecFfff(&velocity, lbl_8047D560, lbl_8047D560, lbl_8047D560);
    return soundEmitterCreate(id, position, &velocity, lbl_8047D564, lbl_8047D560, 0x7F, 0);
}

u32 soundEmitterCreate(u32 id, const GSvec* position, const GSvec* velocity, f32 minDistance, f32 maxDistance, u32 arg5,
                u32 arg6)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    GSsnd3dEmitter* emitter = _soundPool0Alloc();

    if (emitter != 0) {
        sndAddEmitter(emitter->emitter, position, velocity, 0x1F, entry->waveId, arg5, arg6, 0, minDistance, maxDistance);
        GSvecCopy(&emitter->position, position);
        GSvecCopy(&emitter->velocity, velocity);
        emitter->unk1 = arg5;
        emitter->unk2 = arg6;
        emitter->minDistance = minDistance;
        emitter->maxDistance = maxDistance;
        emitter->soundId = id;
        if ((u8)fn_8015E890(emitter->emitter) == 1) {
            return (u32)emitter;
        }
        fn_801677BC(emitter);
    }
    return 0;
}

u32 soundUpdateListener(GSsndEmitter* emitter, const GSvec* position)
{
    GSvec velocity;

    if (emitter != 0) {
        GSvecSub(&velocity, &emitter->position, position);
        if ((u8)s3dUpdateListener(&emitter->unk40, position, &velocity, &emitter->unk28, &emitter->unk34, emitter->unk1, 0)
            == 1) {
            GSvecCopy(&emitter->position, position);
            GSvecCopy(&emitter->velocity, &velocity);
            return 1;
        }
    }
    return 0;
}

u32 soundSetListener(const GSvec* position)
{
    GSvec arg1;
    GSvec arg2;
    GSvec arg3;

    set__5GSvecFfff(&arg1, lbl_8047D560, lbl_8047D560, lbl_8047D560);
    set__5GSvecFfff(&arg2, lbl_8047D560, lbl_8047D560, lbl_8047D568);
    set__5GSvecFfff(&arg3, lbl_8047D560, lbl_8047D56C, lbl_8047D560);
    return _soundSetListener3D(position, &arg1, &arg2, &arg3, lbl_8047D570, lbl_8047D570, lbl_8047D574, 0x7F);
}

u32 _soundSetListener3D(const GSvec* position, const GSvec* arg1, const GSvec* arg2, const GSvec* arg3, f32 arg4, f32 arg5,
                f32 arg6, u32 arg7)
{
    GSsndEmitter* emitter = fn_801678E4();

    if (emitter != 0) {
        if ((u8)s3dSetListener(&emitter->unk40, position, arg1, arg2, arg3, 1, arg7, 0, arg4, arg5, arg6) == 1) {
            GSvecCopy(&emitter->position, position);
            GSvecCopy(&emitter->velocity, arg1);
            GSvecCopy(&emitter->unk28, arg2);
            GSvecCopy(&emitter->unk34, arg3);
            emitter->unk4 = arg4;
            emitter->unk8 = arg5;
            emitter->unkC = arg6;
            emitter->unk1 = arg7;
            return (u32)emitter;
        }

        fn_801677F4(emitter);
    }
    return 0;
}

u32 soundSetVolume(u32 id, u32 volume, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;

    if (((flags >> 7) & 1U) == 0U && ((flags >> 5) & 1U) != 1U) {
        return 0;
    }
    return _soundSlotSetVolume(entry, volume, arg2);
}

s32 GSsndGetStatus(u32 id)
{
  u32 offset = id * (sizeof(GSsndEntry));
  u8 *entry = ((u8 *) lbl_80478FAC) + offset;
  u32 flags = *entry;
  GSsndWork *work;
  u32 handle;
  if (((flags >> 5) & 1U) != 1U)
  {
    return 0;
  }
  work = *((GSsndWork **) (entry + 8));
  if (work == 0)
  {
    return 0;
  }
  if (((flags >> 7) & 1U) == 1U)
  {
    handle = work->handle;
    if ((handle != ((u32) (-1))) && (sndFXCheck(handle) != ((u32) (-1))))
    {
      return 2;
    }
    GSlogWrite(lbl_802736F0, id);
    _soundSlotRelease(id, 1);
    return 0;
  }
  if ((((*entry) >> 4) & 1U) == 1U)
  {
    return 3;
  }
  handle = work->handle;
  if ((handle != ((u32) (-1))) && (((u8) fn_8014D598(handle)) == 1))
  {
    return 2;
  }
  GSlogWrite(lbl_8027371C, id);
  _soundSlotRelease(id, 1);
  return 0;
}

u32 soundResumeSong(u32 id, u32 volume, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;
    GSsndWork* work;
    u32 handle;

    if (((flags >> 7) & 1U) == 1U) {
        return 0;
    }
    if (((flags >> 5) & 1U) != 1U) {
        return 0;
    }

    work = entry->work;
    if (work == 0) {
        return 0;
    }

    handle = work->handle;
    if (handle == (u32)-1) {
        return 0;
    }
    if (((flags >> 4) & 1U) != 1U) {
        return 0;
    }

    soundApplyReverb(entry->reverb);
    if (id == 1) {
        return 1;
    }

    sndSeqContinue(work->handle);
    sndSeqVolume(work->priority, volume, work->handle, 0);

    ((GSsndFlagBits*)&entry->flags)->paused = 0;
    return 1;
}

u32 soundPauseSong(u32 id, u32 volume, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;
    GSsndWork* work;
    u32 handle;

    if (((flags >> 7) & 1U) == 1U) {
        return 0;
    }
    if (((flags >> 5) & 1U) != 1U) {
        return 0;
    }

    work = entry->work;
    if (work == 0) {
        return 0;
    }

    handle = work->handle;
    if (handle == (u32)-1) {
        return 0;
    }
    if (((flags >> 4) & 1U) == 1U) {
        return 0;
    }
    if (id == 1) {
        return 1;
    }

    sndSeqVolume(0, volume, handle, 0);
    sndSeqStop(work->handle);

    ((GSsndFlagBits*)&entry->flags)->paused = 1;
    return 1;
}

void soundStopSE(u32 id)
{
    soundStopRaw(id, 0, 0);
}

void soundStopRaw(u32 id, u32 arg1, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;

    if (((flags >> 7) & 1U) == 1U) {
        _sndStopSE(entry, arg1, arg2);
    } else {
        _sndStopBGM(entry, arg1, arg2);
    }
}

void soundStartSE(u32 id)
{
    soundStartRaw(id, 0, 0);
}

u32 soundStart(u32 id, u32 arg1, u32 volume, u32 arg3)
{
    extern u8 _sndSetVolumeWork(u32, u32);

    if ((u8)_sndSetVolumeWork(id, volume) == 0) {
        return 0;
    }
    return soundStartRaw(id, arg1, arg3);
}

u32 soundStartRaw(u32 id, u32 arg1, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;

    if (((flags >> 7) & 1U) == 1U) {
        return soundStartFX(entry, id, arg2);
    }
    if (((flags >> 5) & 1U) != 1U) {
        return 0;
    }
    return _soundSlotStartStream(entry, arg1, arg2);
}

void soundKill(u32 id)
{
    _soundSlotRelease(id, 1);
}

u32 soundAttachWork(u32 id, u32 arg1, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    GSsndWork* work;
    u32 flags = entry->flags;

    if (((flags >> 5) & 1U) == 1U) {
        return 0;
    }

    work = _soundPool2Alloc();
    if (work == 0) {
        _sndCheckSndWorkALL();
        work = _soundPool2Alloc();
        if (work == 0) {
            return 0;
        }
    }

    _sndInitParms(entry, work);
    work->unkC = arg1;
    work->unk10 = arg2;
    return 1;
}

u8 GSsndOpenWaveDVD(u32 wave, u32 fileId, u32 heap, u32 unk0, u32 unk1, u32 unk2, u32 unk3)
{
    return soundPushWave(wave, 1, fileId, heap, unk0, unk1, unk2, unk3);
}

void soundApplyReverb(u32 reverb)
{
    if (reverb != 0 && lbl_8047B0E4 != reverb) {
        _sndSetReverbParm(reverb);
        sndAuxCallbackUpdateSettingsReverbHI(lbl_80452500);
    }
}

u32 GSsndGetOutputMode(void)
{
    switch (OSGetSoundMode()) {
    case 0:
        return 0;
    case 1:
        return 1;
    default:
        return 0;
    }
}

void soundSetOutputMode(s32 mode)
{
    sndOutputMode();
    switch (mode) {
    case 0:
        OSSetSoundMode(0);
        break;
    case 1:
    case 2:
        OSSetSoundMode(1);
        break;
    }
}

void fn_80166D18(u32 group, u32 volume, u32 left, u32 right)
{
    sndMasterVolume(group, volume, left, right);
}
