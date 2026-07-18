#ifndef GAME_GS_RANGE_801653CC_SHARED_H
#define GAME_GS_RANGE_801653CC_SHARED_H

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

#endif
