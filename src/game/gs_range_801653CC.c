/**
 * @file gs_range_801653CC.c
 * @brief gs-engine code, 0x801653CC - 0x80167040 (57 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "game/data/rodata_80270008.h"

typedef struct GSsndWork {
    u8 flags;
    u8 priority;
    u8 unk2;
    u8 unk3;
    u8 stackDepth;
    u8 volumeStack[3];
    s32 handle;
    u32 unkC;
    u32 unk10;
} GSsndWork;

typedef struct GSsndFlagBits {
    u8 isSe : 1;
    u8 unk6 : 1;
    u8 active : 1;
    u8 paused : 1;
    u8 unk0_3 : 4;
} GSsndFlagBits;

typedef struct GSsndEntry {
    u8 flags;
    u8 unk1;
    u8 reverb;
    u8 waveIndex;
    u16 waveId;
    u16 unk6;
    GSsndWork* work;
} GSsndEntry;

typedef struct GSsndWaveSlot {
    void* data;
    void* archive;
} GSsndWaveSlot;

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

typedef struct GSsndEmitter {
    u8 unk0;
    u8 unk1;
    u8 pad2[2];
    f32 unk4;
    f32 unk8;
    f32 unkC;
    GSvec position;
    GSvec velocity;
    GSvec unk28;
    GSvec unk34;
    u8 unk40[0x38];
} GSsndEmitter;

typedef struct GSsnd3dEmitter {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 pad3;
    u32 soundId;
    f32 minDistance;
    f32 maxDistance;
    GSvec position;
    GSvec velocity;
    u8 emitter[0x50];
} GSsnd3dEmitter;

extern u32* lbl_80478FA8;
extern u32* lbl_80478E30;
extern GSsndEntry* lbl_80478FAC;
extern GSsndWaveSlot* lbl_80478E34;
extern u32* lbl_80478FB0;
extern void* lbl_80478FB4;
extern u32 lbl_8047B0A8;
extern u32 lbl_8047B0AC;
extern u32 lbl_8047B0B0;
extern void* lbl_8047B0B4;
extern u32 lbl_8047B0E4;
extern u32 lbl_8047B0E8;
extern u8 lbl_80452500[];
extern const f32 lbl_8047D540;
extern const f32 lbl_8047D560;
extern const f32 lbl_8047D564;
extern const f32 lbl_8047D568;
extern const f32 lbl_8047D56C;
extern const f32 lbl_8047D570;
extern const f32 lbl_8047D574;

extern const char lbl_80273548[];
extern const char lbl_80273568[];
extern const char lbl_8027358C[];
extern const char lbl_802736CC[];
extern const char lbl_802736F0[];
extern const char lbl_8027371C[];

extern void* GSresAllocResourceAlign(u32, u32, u32, u32, u32);
extern void* GSresGetResource(u32, u32);
extern void* GSresRegisterResource(void*, u32, u32, u32);
extern void GSlogWrite(const char*, ...);
extern void _threadSwitch(void);
extern void* memset(void*, int, u32);
extern void set__5GSvecFfff(GSvec*, f32, f32, f32);
extern void GSvecCopy(GSvec*, const GSvec*);
extern void fn_800E0168(GSvec*, const GSvec*, const GSvec*);
extern void fn_800F9210(u32, u32);
extern u32 fn_800E2C04(u32, u32);
extern void* fn_800E27B0(u32);
extern s32 OSGetSoundMode(void);
extern void fn_800A0EB4(u32);
extern void sndOutputMode(void);
extern void sndMasterVolume(u8, u16, u8, u8);
extern void sndVolume(u8, u16, u8);
extern void sndSeqVolume(u8, u16, u32, u8);
extern u32 sndFXCheck(u32);
extern void sndAddEmitter(void*, const GSvec*, const GSvec*, u32, u32, u32, u32, u32, f32, f32);
extern u8 sndAuxCallbackUpdateSettingsReverbHI(void*);
extern u8 fn_8015FFD4(void);
extern void sndQuit(void);
extern u8 fn_8015E890(void*);
extern u8 fn_8015ED00(void*, const GSvec*, const GSvec*, const GSvec*, const GSvec*, u32, u32);
extern u8 fn_8015EF04(void*, const GSvec*, const GSvec*, const GSvec*, const GSvec*, u32, u32, u32, f32, f32, f32);
extern u32 fn_8014D598(u32);
extern void fn_8014D5C8(u32);
extern void fn_8014D648(u32);
extern s32 fn_8017B2CC(u32);
extern void* fn_80167F28(const char*);
extern u32 fn_80167E5C(void*);
extern void fn_80167E64(void*);
extern s32 fn_80167ED0(void*, void*, u32, u32);
extern void fn_80166D48(u32, u32, u32, u32);
extern u32 fn_8016737C(GSsndEntry*, u32, u32);
extern u32 fn_8016758C(GSsndEntry*, u32, u32);
extern u32 fn_8016761C(GSsndEntry*, u32, u32);
extern void _sndStopSE(GSsndEntry*, u32, u32);
extern void _sndStopBGM(GSsndEntry*, u32, u32);
extern u32 fn_80167768(u32, u32);
extern void fn_801677BC(GSsnd3dEmitter*);
extern void fn_801677F4(GSsndEmitter*);
extern GSsnd3dEmitter* fn_80167864(void);
extern GSsndEmitter* fn_801678E4(void);
extern void fn_80165FDC(u32);
extern void _sndSetReverbParm(u32);
extern u8 fn_80167118(u32, u32, u32, u32, u32, u32, u32, u32);
extern u8 fn_80167070(u32, u32);
extern GSsndWork* fn_80167964(void);
extern void _sndCheckSndWorkALL(void);
extern void _sndInitParms(GSsndEntry*, GSsndWork*);
extern void fn_8017AF6C(u32, void*);
extern void fn_8017B370(u32);

#define SND_FLAG_SE 0x80
#define SND_FLAG_ACTIVE 0x20
#define SND_FLAG_PAUSED 0x10

u32 soundStop(u32 id, u32 fade);
u32 fn_801653CC(u32 id, void* buffer, u32 size);
u32 fn_80165744(void);
u32 fn_80165788(u32 id, f32 x, f32 y, f32 z);
u32 fn_801658FC(u32 id, u32 fade, u32 volume, u32 priority);
u32 fn_8016597C(u32 id, u32 fade, u32 volume, u32 priority);
void fn_80165EE0(void* wave, void* buffer, u32 size);
u32 fn_80165A44(u32 id, u32 volume, u32 priority, s32 kind);
void fn_80165C70(u32 volume, u32 isSe, s32 wait);
void fn_80165D0C(u32 frames);
void fn_80165DEC(const char* path, void* buffer, u32 capacity);
u32 fn_80166084(u32 id);
u32 fn_80166098(u32 id);
void* fn_8016604C(u32 size);
u32 fn_80166168(u32 id, u32 volume);
u32 fn_80166268(u32 id, u32 volume, u32 limit);
u32 fn_801662E8(u32 arg0, u32 arg1);
u32 fn_80166308(u32 id, const GSvec* position);
u32 fn_80166370(u32 id, const GSvec* position, const GSvec* velocity, f32 minDistance, f32 maxDistance, u32 arg5,
                u32 arg6);
u32 fn_80166458(GSsndEmitter* emitter, const GSvec* position);
u32 fn_801664F0(const GSvec* position);
u32 fn_80166578(const GSvec* position, const GSvec* arg1, const GSvec* arg2, const GSvec* arg3, f32 arg4, f32 arg5,
                f32 arg6, u32 arg7);
u32 fn_80166670(u32 id, u32 volume, u32 arg2);
u32 fn_801666BC(u32 id);
u32 fn_801667D8(u32 id, u32 volume, u32 arg2);
u32 fn_801668DC(u32 id, u32 volume, u32 arg2);
void fn_801669E4(u32 id, u32 arg1, u32 arg2);
u32 fn_80166A50(u32 id, u32 arg1, u32 volume, u32 arg3);
u32 fn_80166AB8(u32 id, u32 arg1, u32 arg2);
void fn_80166B18(u32 id);
u32 fn_80166B3C(u32 id, u32 arg1, u32 arg2);
u8 GSsndOpenWaveDVD(u32 wave, u32 fileId, u32 heap, u32 unk0, u32 unk1, u32 unk2, u32 unk3);
void fn_80166C34(u32 reverb);

typedef struct GSsndDvdWave {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 fileId;
} GSsndDvdWave;

u32 fn_801653CC(u32 id, void* buffer, u32 size)
{
    GSsndEntry* entry;
    void* resource;
    void* wave;
    u32 waveIndex;

    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    entry = &lbl_80478FAC[id];
    waveIndex = entry->waveIndex;
    if (waveIndex < *lbl_80478E30) {
        resource = GSresAllocResourceAlign(0x10000, 0x20, 0, 0x408, 0);
        if (resource != 0) {
            wave = lbl_80478E34[waveIndex].data;
            if (wave != 0) {
                fn_80165EE0(wave, resource, 0x10000);
                fn_80166B3C(id, 0, 0x408);
                fn_80166A50(id, (u32)buffer, size, 0);
                while (fn_801666BC(id) == 2) {
                    _threadSwitch();
                }
                fn_80166B18(id);
            }
            fn_800F9210(0, 0x408);
        }
    }

    return 1;
}

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

#pragma dont_inline on
void fn_80165548(u32 id)
{
    if (id != (u32)-1) {
        fn_801667D8(id, 0x3E8, 0);
    }
}

u32 fn_8016557C(void)
{
    u32 id = fn_801662E8(0, 0x406);

    if (id != (u32)-1) {
        fn_801668DC(id, 0x32, 0);
        fn_80165D0C(0x32);
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

u32 fn_80165668(u32 id, void* buffer, u32 size)
{
    u32 savedId = fn_8016557C();
    u32 result = fn_801653CC(id, buffer, size);

    fn_80165548(savedId);
    return result;
}

u32 fn_801656D8(u32 id)
{
    return fn_80166098(id);
}

u32 fn_801656F8(u32 id, u32 arg1, u32 arg2)
{
    return fn_80166670(id, arg1, arg2);
}

u32 fn_80165718(u32 volume, u32 fade)
{
    fn_80166D48(volume, fade, 1, 0);
    return 1;
}

u32 fn_80165744(void)
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
        fn_80165C70(fade, restartSe, 1);
        if (restartSe != 0) {
            GSlogWrite(lbl_8027358C, id);
            return fn_80165A44(1, 0, 0xFF, restartSe);
        }
        fn_80166B18(id);
    } else {
        fn_801669E4(id, fade, 0);
        fn_80166B18(id);
    }
}

u32 fn_801658FC(u32 id, u32 fade, u32 volume, u32 priority)
{
    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    fn_80165C70(fade, 0, 1);
    return fn_80165A44(id, volume, priority, 0);
}

u32 fn_8016597C(u32 id, u32 fade, u32 volume, u32 priority)
{
    if (id == 0 || id >= *lbl_80478FA8) {
        return 0;
    }

    fn_80165C70(fade, 1, 1);
    return fn_80165A44(id, volume, priority, 1);
}

u32 fn_801659FC(u32 id, u32 volume, u32 priority)
{
    return fn_80165A44(id, volume, priority, 0);
}

u32 fn_80165A20(u32 id, u32 volume, u32 priority)
{
    return fn_80165A44(id, volume, priority, 1);
}

u32 fn_80165A44(u32 id, u32 volume, u32 priority, s32 kind)
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
            s32 status = fn_801666BC(id);

            if (status != 0 && status >= 0 && status < 4) {
                fn_80166670(id, 0x32, 0);
                fn_80165D0C(0x32);
                fn_80166B18(id);
            }
            lbl_8047B0A8 = id;
        }
        return fn_80166A50(id, volume, priority, 0);
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
                s32 status = fn_801666BC(currentId);

                if (status != 0 && status >= 0 && status < 4) {
                    fn_80166670(currentId, 0x32, 0);
                    fn_80165D0C(0x32);
                    fn_80166B18(currentId);
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
            fn_80166B3C(id, 0, resourceId);
        }

        if (kind != 0) {
            lbl_8047B0AC = id;
        }
        if (kind == 0) {
            lbl_8047B0A8 = id;
        }
        return fn_80166A50(id, volume, priority, 0);
    }
}

void fn_80165C70(u32 volume, u32 isSe, s32 wait)
{
    u32 resourceId = isSe != 0 ? 0x406 : 0x407;
    u32 id = fn_801662E8(0, resourceId);

    if (id != (u32)-1) {
        s32 status = fn_801666BC(id);

        if (status != 0 && status >= 0 && status < 4) {
            fn_80166670(id, volume, 0);
            if (wait != 0) {
                fn_80165D0C(volume);
            }
        }
    }
}

void fn_80165D0C(u32 frames)
{
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern const f32 lbl_8047D544;
    f32 duration = (f32)frames / lbl_8047D544;
    f32 elapsed = lbl_8047D540;

    while (elapsed < duration) {
        _threadSwitch();
        elapsed += (f32)fn_800D3088() / (f32)fn_800D37CC();
    }
}

void fn_80165DEC(const char* path, void* buffer, u32 capacity)
{
    const char* messages = lbl_80273548;
    u32 alignedSize;
    void* file = fn_80167F28(path);
    u32 size;

    if (file == 0) {
        GSlogWrite(messages + 0xE0, path);
        return;
    }

    size = fn_80167E5C(file);
    if (size == 0) {
        GSlogWrite(messages + 0xFC, path);
    } else {
        alignedSize = (size + 0x1F) & ~0x1FU;
        if (alignedSize < capacity) {
            if (buffer == 0) {
                GSlogWrite(messages + 0x11C);
            } else {
                memset(buffer, 0xE0, alignedSize);
                if (fn_80167ED0(file, buffer, alignedSize, 0) <= 0) {
                    GSlogWrite(messages + 0x140, path);
                }
            }
        } else {
            GSlogWrite(messages + 0x168);
        }
    }

    fn_80167E64(file);
}

void fn_80165EE0(void* wave, void* buffer, u32 size)
{
    memset(buffer, 0xE0, size);
    lbl_8047B0B0 = size;
    lbl_8047B0B4 = buffer;
    fn_8017AF6C(0x99, wave);
}

void fn_80165F40(void)
{
    lbl_8047B0B4 = GSresGetResource(0, 0x406);
    lbl_8047B0B0 = 0x10000;
    fn_8017B370(0x99);
    fn_80165FDC(0x99);
}

void fn_80165F84(void)
{
    GSresRegisterResource(fn_8016604C(0x10000), 0, 0x406, 0);
    GSresRegisterResource(fn_8016604C(0x2000), 0, 0x407, 0);
    lbl_8047B0AC = 0;
    lbl_8047B0A8 = 0;
}

#pragma peephole off
void fn_80165FDC(u32 id)
{
    s32 status;

    while (1) {
        status = fn_8017B2CC(id);
        if (status < 0) {
            GSlogWrite(lbl_802736CC, id);
        }
        if (status == 0) {
            break;
        }
        _threadSwitch();
    }
}
#pragma peephole reset

void* fn_8016604C(u32 size)
{
    u32 handle = fn_800E2C04(size, 0x20);

    if ((u16)handle != 0) {
        return fn_800E27B0(handle);
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

void fn_801660D8(u32 volume, u32 includeBgm, u32 includeSe)
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
  return fn_80166670(id, volume, work->volumeStack[work->stackDepth]);
}

void fn_801661D0(u32 limit, u32 volume, u32 includeBgm, u32 includeSe)
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
            fn_80166268(i, volume, limit);
        }
    }
}

u32 fn_80166268(u32 id, u32 volume, u32 limit)
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

    return fn_80166670(id, volume, limit);
}

u32 fn_801662E8(u32 arg0, u32 arg1)
{
    return fn_80167768(arg0, arg1);
}

u32 fn_80166308(u32 id, const GSvec* position)
{
    GSvec velocity;

    set__5GSvecFfff(&velocity, lbl_8047D560, lbl_8047D560, lbl_8047D560);
    return fn_80166370(id, position, &velocity, lbl_8047D564, lbl_8047D560, 0x7F, 0);
}

u32 fn_80166370(u32 id, const GSvec* position, const GSvec* velocity, f32 minDistance, f32 maxDistance, u32 arg5,
                u32 arg6)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    GSsnd3dEmitter* emitter = fn_80167864();

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

u32 fn_80166458(GSsndEmitter* emitter, const GSvec* position)
{
    GSvec velocity;

    if (emitter != 0) {
        fn_800E0168(&velocity, &emitter->position, position);
        if ((u8)fn_8015ED00(&emitter->unk40, position, &velocity, &emitter->unk28, &emitter->unk34, emitter->unk1, 0)
            == 1) {
            GSvecCopy(&emitter->position, position);
            GSvecCopy(&emitter->velocity, &velocity);
            return 1;
        }
    }
    return 0;
}

u32 fn_801664F0(const GSvec* position)
{
    GSvec arg1;
    GSvec arg2;
    GSvec arg3;

    set__5GSvecFfff(&arg1, lbl_8047D560, lbl_8047D560, lbl_8047D560);
    set__5GSvecFfff(&arg2, lbl_8047D560, lbl_8047D560, lbl_8047D568);
    set__5GSvecFfff(&arg3, lbl_8047D560, lbl_8047D56C, lbl_8047D560);
    return fn_80166578(position, &arg1, &arg2, &arg3, lbl_8047D570, lbl_8047D570, lbl_8047D574, 0x7F);
}

u32 fn_80166578(const GSvec* position, const GSvec* arg1, const GSvec* arg2, const GSvec* arg3, f32 arg4, f32 arg5,
                f32 arg6, u32 arg7)
{
    GSsndEmitter* emitter = fn_801678E4();

    if (emitter != 0) {
        if ((u8)fn_8015EF04(&emitter->unk40, position, arg1, arg2, arg3, 1, arg7, 0, arg4, arg5, arg6) == 1) {
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

u32 fn_80166670(u32 id, u32 volume, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;

    if (((flags >> 7) & 1U) == 0U && ((flags >> 5) & 1U) != 1U) {
        return 0;
    }
    return fn_8016737C(entry, volume, arg2);
}

u32 fn_801666BC(u32 id)
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
    fn_80167070(id, 1);
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
  fn_80167070(id, 1);
  return 0;
}

u32 fn_801667D8(u32 id, u32 volume, u32 arg2)
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

    fn_80166C34(entry->reverb);
    if (id == 1) {
        return 1;
    }

    fn_8014D648(work->handle);
    sndSeqVolume(work->priority, volume, work->handle, 0);

    ((GSsndFlagBits*)&entry->flags)->paused = 0;
    return 1;
}

u32 fn_801668DC(u32 id, u32 volume, u32 arg2)
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
    fn_8014D5C8(work->handle);

    ((GSsndFlagBits*)&entry->flags)->paused = 1;
    return 1;
}

void fn_801669BC(u32 id)
{
    fn_801669E4(id, 0, 0);
}

void fn_801669E4(u32 id, u32 arg1, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;

    if (((flags >> 7) & 1U) == 1U) {
        _sndStopSE(entry, arg1, arg2);
    } else {
        _sndStopBGM(entry, arg1, arg2);
    }
}

void fn_80166A28(u32 id)
{
    fn_80166AB8(id, 0, 0);
}

u32 fn_80166A50(u32 id, u32 arg1, u32 volume, u32 arg3)
{
    extern u8 _sndSetVolumeWork(u32, u32);

    if ((u8)_sndSetVolumeWork(id, volume) == 0) {
        return 0;
    }
    return fn_80166AB8(id, arg1, arg3);
}

u32 fn_80166AB8(u32 id, u32 arg1, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    u32 flags = entry->flags;

    if (((flags >> 7) & 1U) == 1U) {
        return fn_8016758C(entry, id, arg2);
    }
    if (((flags >> 5) & 1U) != 1U) {
        return 0;
    }
    return fn_8016761C(entry, arg1, arg2);
}

void fn_80166B18(u32 id)
{
    fn_80167070(id, 1);
}

u32 fn_80166B3C(u32 id, u32 arg1, u32 arg2)
{
    GSsndEntry* entry = &lbl_80478FAC[id];
    GSsndWork* work;
    u32 flags = entry->flags;

    if (((flags >> 5) & 1U) == 1U) {
        return 0;
    }

    work = fn_80167964();
    if (work == 0) {
        _sndCheckSndWorkALL();
        work = fn_80167964();
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
    return fn_80167118(wave, 1, fileId, heap, unk0, unk1, unk2, unk3);
}

void fn_80166C34(u32 reverb)
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

void fn_80166CC0(s32 mode)
{
    sndOutputMode();
    switch (mode) {
    case 0:
        fn_800A0EB4(0);
        break;
    case 1:
    case 2:
        fn_800A0EB4(1);
        break;
    }
}

void fn_80166D18(u32 group, u32 volume, u32 left, u32 right)
{
    sndMasterVolume(group, volume, left, right);
}

void fn_80166D48(u32 volume, u32 fade, u32 includeBgm, u32 includeSe)
{
    extern u8 _sndSetVolumeWork(u32, u8);
    u32 offset;
    u32 group;
    u32 i;

    if (includeBgm == 1 && includeSe == 1) {
        group = 0xFF;
    } else if (includeBgm == 1 && includeSe != 1) {
        group = 0xFD;
    } else if (includeBgm != 1 && includeSe == 1) {
        group = 0xFE;
    } else {
        return;
    }

    for (i = 0, offset = 0; i < lbl_8047B0E8; offset += sizeof(GSsndEntry), i++) {
        u32 flags = *(u8*)((u8*)lbl_80478FAC + offset);

        if (((flags >> 5) & 1U) == 1U) {
            switch (group) {
            case 0xFD:
                if (((flags >> 7) & 1U) == 1U) {
                    continue;
                }
                break;
            case 0xFE:
                if (((flags >> 7) & 1U) == 0U) {
                    continue;
                }
                break;
            }
            _sndSetVolumeWork(i, volume);
        }
    }

    sndVolume(volume, fade, group);
}

void fn_80166E44(void)
{
    if (fn_8015FFD4() == 1) {
        fn_80166D48(0, 0, 1, 1);
        sndQuit();
    }
}
