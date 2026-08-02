/**
 * @file synth_suffix_8014B044.c
 * @brief MusyX runtime synth suffix, 0x8014B044 - 0x8014D598.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/synth.c` (byte-exact matched in MP4 / Prime /
 * Strikers at GC/1.3.2). This unit owns LowPrecisionHandler through
 * synthExit, reference synth.c's last function.
 */

#include "dolphin/types.h"
#include "game/people/people.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void  DCFlushRange(void* ptr, u32 size);
extern u32   OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(u32 level);

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void ARQPostRequest();
extern void InitStreamBuffers();
extern void aramQueueCallback();
extern void aramUploadData();
extern u16 inpGetMidiCtrl(u8 ctrl, u8 channel, u8 set);
extern void salCalcVolume(u32 volumeArg, f32* out, u32 pan, u32 surroundPan,
                          f32 a, f32 b, f32 c, u32 narrowPan,
                          u32 studioMode);
extern void salCallback();
extern u8 jumptable_80369CB0[];
extern u8 jumptable_80369CD4[];
extern u8 jumptable_80369CF8[];
extern u8 lbl_80273448[];
extern u8 lbl_8036944C[];
extern u8 lbl_8036BF00[];
extern u8 lbl_80434C50[];
extern f32 lbl_8047D4D8;
extern f32 lbl_8047D4DC;
extern f32 lbl_8047D4E0;
extern f64 lbl_8047D4E8;
extern f32 lbl_8047D4F0;
extern f32 lbl_8047D4F4;
extern f32 lbl_8047D4F8;
extern f32 lbl_8047D4FC;
extern f32 lbl_8047D500;
extern f32 lbl_8047D504;
extern f64 lbl_8047D508;
extern f32 lbl_8047D510;
extern f32 lbl_8047D514;
extern f64 lbl_8047D518;
extern f32 lbl_8047D520;
extern f64 lbl_8047D528;
extern f32 lbl_8047D530;
extern f32 lbl_8047D534;
extern u32 lbl_8047B070;
extern u32 lbl_8047B078;
extern u32 lbl_8047B07C;

/* GSmem allocator */
extern u16   _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);

/* External functions referenced from asm wrappers */
extern u32 sndAuxCallbackUpdateSettingsReverbHI(u8* ptr);

/* Model system */
extern void  GSmodelGetPart(void* model, u32 param);
extern void  GSpartFree(void* model, u32 param);
extern void  fn_800E24B0(void* model, u32 param);
extern void  fn_800E209C(void* model, u32 param);
extern void  set__5GSvecFfff(void* dst, void* src);
extern void  GSvecCopy(void* dst, void* src);
extern void  GSvecAdd(void* model, void* param);
extern void  fn_800E0BA0(void* param);
extern void  fn_800E0BE4(void* param);
extern void  fn_800E013C(void* param);
extern u32   __cvt_fp2unsigned(f64 val);

/* Floor/field system */
extern void* GSresGetResource(u16 group, u16 model, u16 param);

/* GX rendering */
extern void  GSmodelSetVisibility(void* param);

/* People data layer (people_data.c) */
extern void* itemDataBiosGetPtr(u16 index);   /* peopleFieldGetByIndex */
extern void* fn_80142CF4(u32 a, u32 b, u32 c, u32 d);  /* peopleFieldAlloc */
extern void  fn_801429E8(void* entry);  /* peopleFieldGetEntry */
extern void  fn_80142984(u32 id);       /* peopleFieldGetByID */

/* Script system */
extern u32 lbl_80434A10[9][16]; /* synthTicksPerSecond */

/* ===================================================================
 * MusyX runtime: synth.c (voice/macro work struct core, synthvoice).
 * Reference: AxioDL/musyx `musyx/runtime/synth.c`, cross-verified against
 * byte-exact matched copies in Mario Party 4, Metroid Prime and Mario
 * Strikers (GC/1.3.2). SYNTH_VOICE below matches the pre-2.0.1 field set
 * (no sampleId/lpfLower.../inpFilterSwitch/inpFilterParameter, allocId is
 * u16) -- confirmed by struct size (0x404 with 13 CTRL_DEST members
 * ending exactly at mesgNum 0x3EC) and by the `block` field being present
 * (MUSY_VERSION >= 1,5,4, < 2,0,1). lbl_8047AF48 (fka "PeopleFieldWork"
 * array) is `SYNTH_VOICE* synthVoice` -- a POINTER (size 4), not the
 * array itself; synthInit allocates the real array via salMalloc.
 * =================================================================== */

#pragma pack(4)
typedef struct MSTEP { u32 para[2]; } MSTEP;

typedef struct SYNTH_QUEUE {
    struct SYNTH_QUEUE* next; // 0x0
    struct SYNTH_QUEUE* prev; // 0x4
    u8 voice;                 // 0x8
    u8 jobTabIndex;           // 0x9
} SYNTH_QUEUE; // size 0xC

typedef struct SYNTH_JOBTAB {
    SYNTH_QUEUE* lowPrecision;
    SYNTH_QUEUE* event;
    SYNTH_QUEUE* zeroOffset;
} SYNTH_JOBTAB; // size 0xC

typedef struct SYNTH_LFO {
    u32 time;
    u32 period;
    s16 value;
    s16 lastValue;
} SYNTH_LFO; // size 0xC

typedef struct ADSR_VARS {
    u8 mode;
    u8 state;
    u16 pad_2;
    u32 cnt;
    s32 currentVolume;
    s32 currentIndex;
    s32 currentDelta;
    u8 data[20];
} ADSR_VARS; // size 0x28

typedef struct CTRL_SOURCE { u8 midiCtrl; u8 combine; u16 pad; s32 scale; } CTRL_SOURCE; // 0x8
typedef struct CTRL_DEST { CTRL_SOURCE source[4]; u16 oldValue; u8 numSource; u8 pad; } CTRL_DEST; // 0x24

typedef struct SYNTH_VOICE {
    SYNTH_QUEUE lowPrecisionJob;             // 0x0
    SYNTH_QUEUE zeroOffsetJob;               // 0xC
    SYNTH_QUEUE eventJob;                    // 0x18
    u64 lastLowCallTime;                     // 0x24
    u64 lastZeroCallTime;                    // 0x2C
    MSTEP* addr;                             // 0x34
    MSTEP* curAddr;                          // 0x38
    struct SYNTH_VOICE* nextMacActive;       // 0x3C
    struct SYNTH_VOICE* prevMacActive;       // 0x40
    struct SYNTH_VOICE* nextTimeQueueMacro;  // 0x44
    struct SYNTH_VOICE* prevTimeQueueMacro;  // 0x48
    u32 macState;                            // 0x4C
    MSTEP* trapEventAddr[3];                 // 0x50
    MSTEP* trapEventCurAddr[3];              // 0x5C
    u8 trapEventAny;                         // 0x68
    u8 pad_69[3];
    u8 callStack[0x20];                      // 0x6C  (opaque; unused by this range)
    u8 callStackEntryNum;                    // 0x8C
    u8 callStackIndex;                       // 0x8D
    u8 pad_8E[2];
    u64 macStartTime;                        // 0x90
    u64 wait;                                // 0x98
    u64 waitTime;                            // 0xA0
    u8 timeUsedByInput;                      // 0xA8
    u8 pad_A9;
    u16 loop;                                // 0xAA
    s32 local_vars[16];                      // 0xAC
    u32 child;                               // 0xEC
    u32 parent;                              // 0xF0
    u32 id;                                  // 0xF4
    void* vidList;                           // 0xF8
    void* vidMasterList;                     // 0xFC
    u16 allocId;                             // 0x100
    u16 macroId;                             // 0x102
    u8 keyGroup;                             // 0x104
    u8 pad_105[3];
    u32 lastVID;                             // 0x108
    u8 prio;                                 // 0x10C
    u8 pad_10D;
    u16 ageSpeed;                            // 0x10E
    u32 age;                                 // 0x110
    u64 cFlags;                              // 0x114
    u8 block;                                // 0x11C
    u8 fxFlag;                               // 0x11D
    u8 vGroup;                               // 0x11E
    u8 studio;                               // 0x11F
    u8 track;                                // 0x120
    u8 midi;                                 // 0x121
    u8 midiSet;                              // 0x122
    u8 section;                              // 0x123
    u32 sInfo;                               // 0x124
    u32 playFrq;                             // 0x128
    u16 curNote;                             // 0x12C
    s8 curDetune;                            // 0x12E
    u8 orgNote;                              // 0x12F
    u8 lastNote;                             // 0x130
    u8 portType;                             // 0x131
    u16 portLastCtrlState;                   // 0x132
    u32 portDuration;                        // 0x134
    u32 portCurPitch;                        // 0x138
    u32 portTime;                            // 0x13C
    u8 vibKeyRange;                          // 0x140
    u8 vibCentRange;                         // 0x141
    u8 pad_142[2];
    u32 vibPeriod;                           // 0x144
    u32 vibCurTime;                          // 0x148
    s32 vibCurOffset;                        // 0x14C
    s16 vibModAddScale;                      // 0x150
    u16 pad_152;
    u32 volume;                              // 0x154
    u32 orgVolume;                           // 0x158
    f32 lastVolFaderScale;                   // 0x15C
    u32 lastPan;                             // 0x160
    u32 lastSPan;                            // 0x164
    f32 treCurScale;                         // 0x168
    u16 treScale;                            // 0x16C
    u16 treModAddScale;                      // 0x16E
    u32 panning[2];                          // 0x170
    s32 panDelta[2];                         // 0x178
    u32 panTarget[2];                        // 0x180
    u32 panTime[2];                          // 0x188
    u8 revVolScale;                          // 0x190
    u8 revVolOffset;                         // 0x191
    u8 volTable;                             // 0x192
    u8 itdMode;                              // 0x193
    s32 envDelta;                            // 0x194
    u32 envTarget;                           // 0x198
    u32 envCurrent;                          // 0x19C
    u32 sweepOff[2];                         // 0x1A0
    s32 sweepAdd[2];                         // 0x1A8
    s32 sweepCnt[2];                         // 0x1B0
    u8 sweepNum[2];                          // 0x1B8
    u8 pad_1BA[2];
    SYNTH_LFO lfo[2];                        // 0x1BC
    u8 lfoUsedByInput[2];                    // 0x1D4
    u8 pbLowerKeyRange;                      // 0x1D6
    u8 pbUpperKeyRange;                      // 0x1D7
    u16 pbLast;                              // 0x1D8
    u8 pad_1DA[2];
    ADSR_VARS pitchADSR;                     // 0x1DC
    s16 pitchADSRRange;                      // 0x204
    u16 curPitch;                            // 0x206
    u8 setup_vol;                            // 0x208
    u8 setup_pan;                            // 0x209
    u8 setup_midi;                           // 0x20A
    u8 setup_midiSet;                        // 0x20B
    u8 setup_section;                        // 0x20C
    u8 setup_track;                          // 0x20D
    u8 setup_vGroup;                         // 0x20E
    u8 setup_studio;                         // 0x20F
    u8 setup_itdMode;                        // 0x210
    u8 pad_211[3];
    u32 midiDirtyFlags;                      // 0x214
    CTRL_DEST inpVolume;                     // 0x218
    CTRL_DEST inpPanning;                    // 0x23C
    CTRL_DEST inpSurroundPanning;            // 0x260
    CTRL_DEST inpPitchBend;                  // 0x284
    CTRL_DEST inpDoppler;                    // 0x2A8
    CTRL_DEST inpModulation;                 // 0x2CC
    CTRL_DEST inpPedal;                      // 0x2F0
    CTRL_DEST inpPortamento;                 // 0x314
    CTRL_DEST inpPreAuxA;                    // 0x338
    CTRL_DEST inpReverb;                     // 0x35C
    CTRL_DEST inpPreAuxB;                    // 0x380
    CTRL_DEST inpPostAuxB;                   // 0x3A4
    CTRL_DEST inpTremolo;                    // 0x3C8
    u8 mesgNum;                              // 0x3EC
    u8 mesgRead;                             // 0x3ED
    u8 mesgWrite;                            // 0x3EE
    u8 pad_3EF;
    s32 mesgQueue[4];                        // 0x3F0
    u16 curOutputVolume;                     // 0x400
    u8 pad_402[2];
} SYNTH_VOICE; // size 0x404

typedef struct LAYER {
    u16 id;
    u8 keyLow;
    u8 keyHigh;
    s8 transpose;
    u8 volume;
    s16 prioOffset;
    u8 panning;
    u8 reserved[3];
} LAYER; // size 0xC

typedef struct KEYMAP {
    u16 id;
    s8 transpose;
    u8 panning;
    s16 prioOffset;
    u8 reserved[2];
} KEYMAP; // size 0x8

typedef struct FX_TAB {
    u16 id;
    u16 macro;
    u8 maxVoices;
    u8 priority;
    u8 volume;
    u8 panning;
    u8 key;
    u8 vGroup;
} FX_TAB; // size 0xA

typedef struct SynthInfo {
    u32 mixFrq;      // 0x0
    u32 numSamples;  // 0x4
    u8 pad[0x210 - 8];
    u8 voiceNum;     // 0x210
    u8 maxMusic;     // 0x211
    u8 maxSFX;       // 0x212
    u8 studioNum;    // 0x213
} SynthInfo; // size 0x214

typedef struct SYNTHMasterFader {
    f32 volume;         // 0x0
    f32 target;         // 0x4
    f32 start;          // 0x8
    f32 time;           // 0xC
    f32 deltaTime;      // 0x10
    f32 pauseVol;       // 0x14
    f32 pauseTarget;    // 0x18
    f32 pauseStart;     // 0x1C
    f32 pauseTime;      // 0x20
    f32 pauseDeltaTime; // 0x24
    u32 seqId;          // 0x28
    u8 seqMode;         // 0x2C
    u8 type;            // 0x2D
    u8 pad_2E[2];
} SYNTHMasterFader; // size 0x30
#pragma pack()

extern SYNTH_VOICE* lbl_8047AF48;             /* synthVoice */
extern SYNTHMasterFader lbl_80434E64[32];     /* synthMasterFader */
extern SYNTH_JOBTAB lbl_804354A4[32];         /* synthJobTable */
extern u8 lbl_80435464[64];                   /* synthTrackVolume */
extern void* lbl_80435624[8];                 /* synthAuxAUser */
extern void (*lbl_80435644[8])(u32, void*, void*); /* synthAuxACallback */
extern void* lbl_80435664[8];                 /* synthAuxBUser */
extern void (*lbl_80435684[8])(u32, void*, void*); /* synthAuxBCallback */
extern u32 lbl_804356B4[16];                  /* synthGlobalVariable */
extern u8 lbl_804356A4[16];                    /* synthITDDefault */
extern u8 lbl_8047AF18;                       /* sndActive */
extern u8 lbl_8047AF19;                       /* synthJobTableIndex */
extern u8 lbl_8047AF1C[8];                    /* synthAuxBMIDISet */
extern u8 lbl_8047AF24[8];                    /* synthAuxBMIDI */
extern u8 lbl_8047AF2C[8];                    /* synthAuxAMIDISet */
extern u8 lbl_8047AF34[8];                    /* synthAuxAMIDI */
extern u32 lbl_8047AF3C;                      /* synthMasterFaderPauseActiveFlags */
extern u32 lbl_8047AF40;                      /* synthMasterFaderActiveFlags */
extern u32 lbl_8047AF44;                      /* synthFlags */
extern void* lbl_8047AF4C;                    /* synthMessageCallback */
extern u64 lbl_8047AF58;                      /* synthRealTime */
extern u32 lbl_8047AF5C;                      /* synthRealTime, low word */
extern const f32 lbl_8047D380;
extern const f32 lbl_8047D3A8;

/* Forward declarations for callees implemented elsewhere in this TU
 * (later address ranges, not yet decompiled -- slice 3 territory) or
 * later in this same file (defined after this block). Signatures follow
 * AxioDL/musyx headers (voice.h/macros.h/adsr.h/hardware.h/snd.h/seq.h). */
extern s32 sndSin(u32 angle);
extern u32 sndGetPitch(u8 key, u32 sInfo);
extern s32 sndPitchUpOne(u16 note);
extern u16 inpGetPitchBend(u8* obj);
extern u16 inpGetModulation(u8* obj);
extern u16 inpGetDoppler(u8* obj);
extern u16 inpGetPanning(u8* obj);
extern u16 inpGetSurroundPanning(u8* obj);
extern u16 inpGetPreAuxA(u8* obj);
extern u16 inpGetReverb(u8* obj);
extern u16 inpGetPreAuxB(u8* obj);
extern u16 inpGetPostAuxB(u8* obj);
extern u16 inpGetTremolo(u8* obj);
extern u16 inpGetPedal(u8* obj);
extern u16 inpGetVolume(u8* obj);
extern void inpSetMidiLastNote(u8 midi, u8 midiSet, u8 note);
extern void inpFXCopyCtrl(u8 ctrl, void* dvoice, void* svoice);
extern void inpSetMidiCtrl14(u8 ctrl, u8 channel, u8 set, u16 value);
extern u32 dataGetLayer(u16 arg, u16* out);
extern u32 dataGetKeymap(u16 arg);
extern u32 dataGetFX(u16 key);
extern u32 vidGetInternalId(u32 id);
extern void voiceSetLastStarted(void* svoice);
extern void voiceInitLastStarted(void);
extern void voiceKill(u32 vi);
extern void macInit(void);
extern void macSetPedalState(void* svoice, u32 state);
extern void macSetExternalKeyoff(void* svoice);
extern void macSampleEndNotify(void* svoice);
extern void vidInit(void);
extern void synthInitAllocationAids(void);
extern u32 adsrHandleLowPrecision(void* adsr, u16* adsr_start, u16* adsr_delta);
extern u32 adsrRelease(void* adsr);
extern void vsSampleEndNotify(u32 pubID);
extern u32 fn_80159550(u32 voiceID); /* vsSampleStartNotify */
extern void hwSetPitch(u32 index, u16 value);
extern void hwSetVolume(u32 v, u8 table, f32 vol, u32 pan, u32 span, f32 auxa, f32 auxb);
extern void hwStart(u32 index, u8 unused2);
extern void hwKeyOff(u32 index);
extern u32 hwGetVirtualSampleID(u32 index);

extern u32 fn_8016246C(u32 voice);            /* hwIsActive */
extern u8 fn_80162464(void);                  /* hwGetTimeOffset */
extern void fn_80162494(u32 index, u32 val); /* hwSetPriority (wraps hw voice-slot priority store) */
extern void fn_801631A8(void);                /* hwFrameDone */
extern void fn_8016248C(u32 val);             /* hwSetMesgCallback */
extern u32 fn_801643D8(u32 size);             /* salMalloc */
extern void fn_80164400(u32 a);               /* salFree */
extern void fn_80157360(SYNTH_VOICE* sv);     /* vidRemoveVoiceReferences */
extern u32 fn_801576B0(SYNTH_VOICE* sv);      /* vidMakeRoot */
extern u32 fn_801576C4(SYNTH_VOICE* sv, u32 isMaster); /* vidMakeNew */
extern u32 macStart(u16 macId, u8 prio, u8 maxVoices, u16 allocId, u8 key, u8 vol, u8 panning,
                        u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid, u8 vidFlag,
                        u8 vGroup, u8 studio, u32 itd); /* macStart */
extern void fn_801603C0(u8 ctrl, u8 midi, u8 midiSet, u8 value); /* inpSetMidiCtrl */
extern u32 fn_80161934(u8 idx, u8 param, u32 midi, u32 midiSet); /* inpGetAuxA */
extern u32 fn_801619E8(u8 idx, u8 param, u32 midi, u32 midiSet); /* inpGetAuxB */
extern void fn_80161A9C(u32 a);               /* inpInit */
extern void macHandle(u32 deltaTime);       /* macHandle */
extern u32 synthStartSound(u16 id, u8 prio, u8 max, u8 key, u8 vol, u8 panning, u8 midi,
                          u8 midiSet, u8 section, u16 step, u16 trackid, u8 vGroup,
                          s16 prioOffset, u8 studio, u32 itd);
#if defined(SYNTH_SUFFIX_SPLIT) && defined(SYNTH_SUFFIX_INIT)
extern u32 synthHWMessageHandler(u32 mesg, u32 voiceID);
#endif

/*
 * These definitions remain visible in this partition because GC/1.3.2 inlines
 * them into suffix functions. static inline prevents duplicate out-of-line
 * symbols; the canonical definitions live in the preceding partitions.
 */
#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_INIT)
static inline void synthSetBpm(u32 bpm, u8 seqId, u8 secIndex) {
    if (seqId == 0xff) {
        seqId = 8;
    }
    lbl_80434A10[seqId][secIndex] = ((bpm << 3) * 1536) / 240;
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT)
static inline u32 synthGetTicksPerSecond(SYNTH_VOICE* svoice) {
    return lbl_80434A10[svoice->midiSet == 0xff ? 8 : svoice->midiSet][svoice->section];
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_LOW)
static inline void synthInitPortamento(SYNTH_VOICE* svoice) {
    if (svoice->cFlags & 0x20000) {
        return;
    }
    if (svoice->portType == 1) {
        if (!(svoice->cFlags & 0x1000)) {
            svoice->portTime = 0;
        } else {
            svoice->portTime = svoice->portDuration;
        }
    } else {
        svoice->portTime = svoice->portDuration;
    }
    svoice->portCurPitch = svoice->lastNote << 16;
}
#endif

#if defined(SYNTH_SUFFIX_SPLIT) && \
    (defined(SYNTH_SUFFIX_LOW) || defined(SYNTH_SUFFIX_ZERO))
extern void synthAddJob(SYNTH_VOICE* svoice, u32 jobType, u32 deltaTime);
#endif

/* -------------------------------------------------------------------
 * synthGetTicksPerSecond / synthSetBpm's table
 * ---------------------------------------------------------------- */
#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_JOBS)
#if defined(SYNTH_SUFFIX_SPLIT)
void synthAddJob(SYNTH_VOICE* svoice, u32 jobType, u32 deltaTime) {
#else
static void synthAddJob(SYNTH_VOICE* svoice, u32 jobType, u32 deltaTime) {
#endif
    u8 jobTabIndex;
    SYNTH_QUEUE* newJq;
    SYNTH_QUEUE** root;
    SYNTH_JOBTAB* jobTab;

    jobTabIndex = ((deltaTime / 256) + lbl_8047AF19) & 0x1f;
    jobTab = &lbl_804354A4[jobTabIndex];

    switch (jobType) {
    case 0:
        newJq = &svoice->lowPrecisionJob;
        if (newJq->jobTabIndex != 0xff) {
            if (newJq->jobTabIndex == jobTabIndex) {
                return;
            }
            if (newJq->next != NULL) {
                newJq->next->prev = newJq->prev;
            }
            if (newJq->prev != NULL) {
                newJq->prev->next = newJq->next;
            } else {
                lbl_804354A4[newJq->jobTabIndex].lowPrecision = newJq->next;
            }
        }
        root = &jobTab->lowPrecision;
        break;
    case 1:
        newJq = &svoice->zeroOffsetJob;
        if (newJq->jobTabIndex != 0xff) {
            if (newJq->jobTabIndex == jobTabIndex) {
                return;
            }
            if (newJq->next != NULL) {
                newJq->next->prev = newJq->prev;
            }
            if (newJq->prev != NULL) {
                newJq->prev->next = newJq->next;
            } else {
                lbl_804354A4[newJq->jobTabIndex].zeroOffset = newJq->next;
            }
        }
        root = &jobTab->zeroOffset;
        break;
    case 2:
        newJq = &svoice->eventJob;
        if (newJq->jobTabIndex != 0xff) {
            return;
        }
        root = &jobTab->event;
    default:
        break;
        return;
    }

    newJq->jobTabIndex = jobTabIndex;
    if ((newJq->next = *root) != NULL) {
        (*root)->prev = newJq;
    }
    newJq->prev = NULL;
    *root = newJq;
}

void synthStartSynthJobHandling(SYNTH_VOICE* svoice) {
    svoice->lastLowCallTime = lbl_8047AF58;
    svoice->lastZeroCallTime = lbl_8047AF58;
    synthAddJob(svoice, 0, 0);
    synthAddJob(svoice, 1, 0);
}

void synthForceLowPrecisionUpdate(SYNTH_VOICE* svoice) {
    synthAddJob(svoice, 0, 0);
    synthAddJob(svoice, 1, 0);
}

void synthKeyStateUpdate(SYNTH_VOICE* svoice) { synthAddJob(svoice, 2, 0); }
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_FX)
u8 synthFXGetMaxVoices(u16 fid) {
    FX_TAB* fx;
    if ((fx = (FX_TAB*)dataGetFX(fid)) != NULL) {
        return fx->maxVoices;
    }
    return 0;
}

u32 synthFXStart(u16 fid, u8 vol, u8 pan, u8 studio, u32 itd) {
    FX_TAB* fx;
    u32 v;
    v = 0xFFFFFFFF;
    if ((fx = (FX_TAB*)dataGetFX(fid)) != NULL) {
        if (vol == 0xFF) {
            vol = fx->volume;
        }
        if (pan == 0xFF) {
            pan = fx->panning;
        }
        v = synthStartSound(fx->macro, fx->priority, fx->maxVoices, fx->key | 0x80, vol, pan, 0xFF,
                        0xFF, 0, 0, 0xFF, fx->vGroup, 0, studio, itd);
    }
    return v;
}

u32 synthFXSetCtrl(u32 vid, u8 ctrl, u8 value) {
    u32 i;
    u32 ret;
    ret = 0;
    vid = vidGetInternalId(vid);
    while (vid != 0xFFFFFFFF) {
        i = vid & 0xff;
        if (vid == lbl_8047AF48[i].id) {
            if ((lbl_8047AF48[i].cFlags & 0x2) != 0) {
                fn_801603C0(ctrl, i, lbl_8047AF48[i].setup_midiSet, value);
            } else {
                fn_801603C0(ctrl, i, lbl_8047AF48[i].midiSet, value);
            }
            vid = lbl_8047AF48[i].child;
            ret = 1;
        } else {
            return ret;
        }
    }
    return ret;
}

u32 synthFXSetCtrl14(u32 vid, u8 ctrl, u16 value) {
    u32 i;
    u32 ret;
    ret = 0;
    vid = vidGetInternalId(vid);
    while (vid != 0xFFFFFFFF) {
        i = vid & 0xff;
        if (vid == lbl_8047AF48[i].id) {
            if ((lbl_8047AF48[i].cFlags & 0x2) != 0) {
                inpSetMidiCtrl14(ctrl, i, lbl_8047AF48[i].setup_midiSet, value);
            } else {
                inpSetMidiCtrl14(ctrl, i, lbl_8047AF48[i].midiSet, value);
            }
            vid = lbl_8047AF48[i].child;
            ret = 1;
        } else {
            return ret;
        }
    }
    return ret;
}

void synthFXCloneMidiSetup(SYNTH_VOICE* dest, SYNTH_VOICE* src) {
    inpFXCopyCtrl(7, dest, src);
    inpFXCopyCtrl(0xa, dest, src);
    inpFXCopyCtrl(0x5b, dest, src);
    inpFXCopyCtrl(0x80, dest, src);
    inpFXCopyCtrl(0x84, dest, src);
}

u32 synthSendKeyOff(u32 voiceid) {
    u32 i;
    u32 ret;
    ret = 0;
    if (lbl_8047AF18 != 0) {
        voiceid = vidGetInternalId(voiceid);
        while (voiceid != 0xFFFFFFFF) {
            i = voiceid & 0xff;
            if (voiceid == lbl_8047AF48[i].id) {
                macSetExternalKeyoff(&lbl_8047AF48[i]);
                ret = 1;
            }
            voiceid = lbl_8047AF48[i].child;
        }
    }
    return ret;
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_VOLUME)
static inline void SetupFader(SYNTHMasterFader* smf, u8 volume, u32 time, u8 seqMode, u32 seqId) {
    smf->seqMode = seqMode;
    smf->seqId = seqId;
    if (time != 0) {
        smf->start = smf->volume;
        smf->target = (f32)volume * (1.f / 127.f);
        smf->time = 1.f;
        smf->deltaTime = 1280.f / (f32)time;
    } else {
        smf->volume = smf->target = (f32)volume * (1.f / 127.f);
        if (smf->seqId != 0xFFFFFFFF) {
            switch (smf->seqMode) {
            case 1: seqStop(smf->seqId); break;
            case 2: seqPause(smf->seqId); break;
            case 3: seqMute(smf->seqId, 0, 0); break;
            }
        }
    }
}

void synthVolume(u8 volume, u16 time, u8 vGroup, u8 seqMode, u32 seqId) {
    u32 ltime;
    u32 i;
    u8 type;
    SYNTHMasterFader* smf;

    if ((ltime = time) != 0) {
        fn_801621BC(&ltime);
    }

    switch (vGroup) {
    case 0xFF: /* SND_ALL_VOLGROUPS */
        for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
            if (smf->type == 0 || smf->type == 1) {
                SetupFader(smf, volume, ltime, seqMode, 0xFFFFFFFF);
                lbl_8047AF40 |= 1 << i;
            }
        }
        return;

    case 0xFC: /* SND_USERALL_VOLGROUPS */
        for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
            if (smf->type == 2 || smf->type == 3) {
                SetupFader(smf, volume, ltime, seqMode, 0xFFFFFFFF);
                lbl_8047AF40 |= 1 << i;
            }
        }
        return;

    case 0xFA: /* SND_USERMUSIC_VOLGROUPS */
        type = 2;
        goto setup_type;

    case 0xFB: /* SND_USERFX_VOLGROUPS */
        type = 3;
        goto setup_type;

    case 0xFD: /* SND_MUSIC_VOLGROUPS */
        type = 0;
        goto setup_type;

    case 0xFE: /* SND_FX_VOLGROUPS */
        type = 1;
        goto setup_type;

    setup_type:
        for (smf = lbl_80434E64, i = 0; i < 32; ++i, ++smf) {
            if (smf->type == type) {
                SetupFader(smf, volume, ltime, seqMode, 0xFFFFFFFF);
                lbl_8047AF40 |= 1 << i;
            }
        }
        return;

    default:
        SetupFader(&lbl_80434E64[vGroup], volume, ltime, seqMode, seqId);
        lbl_8047AF40 |= 1 << vGroup;
        return;
    }
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_POST)
u32 synthIsFadeOutActive(u8 vGroup) {
    if (lbl_80434E64[vGroup].type != 4 && (lbl_8047AF40 & (1 << vGroup)) != 0 &&
        lbl_80434E64[vGroup].start > lbl_80434E64[vGroup].target) {
        return 1;
    }
    return 0;
}

void synthSetMusicVolumeType(u8 vGroup, u8 type) {
    if (lbl_8047AF18) {
        lbl_80434E64[vGroup].type = type;
    }
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || \
    defined(SYNTH_SUFFIX_LOW) || defined(SYNTH_SUFFIX_ZERO) || \
    defined(SYNTH_SUFFIX_HANDLE)
static inline void UpdateTimeMIDICtrl(SYNTH_VOICE* sv) {
    if (!sv->timeUsedByInput) {
        return;
    }
    sv->timeUsedByInput = 0;
    sv->midiDirtyFlags = 0x1fff;
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_LOW)
static inline u32 apply_portamento(SYNTH_VOICE* svoice, u32 ccents, u32 deltaTime) {
    u32 old_portCurPitch;

    if ((svoice->cFlags & 0x400) != 0 && (s32)((svoice->portDuration - svoice->portTime) >> 8) > 0) {
        old_portCurPitch = svoice->portCurPitch;
        svoice->portCurPitch += (s32)deltaTime * ((s32)(ccents - svoice->portCurPitch) >> 8) /
                                 (s32)((svoice->portDuration - svoice->portTime) >> 8);

        if ((old_portCurPitch < ccents && svoice->portCurPitch < ccents) ||
            (old_portCurPitch > ccents && (svoice->portCurPitch > ccents))) {
            ccents = svoice->portCurPitch;
            svoice->portTime += deltaTime;
        } else {
            svoice->portTime = svoice->portDuration;
        }
    }
    return ccents;
}

static inline u32 convert_cents(SYNTH_VOICE* svoice, u32 ccents) {
    u32 curDetune;
    u32 cpitch;

    cpitch = sndGetPitch(ccents / 65536, svoice->sInfo) * 65536;
    if ((curDetune = ccents & 0xffff) != 0) {
        cpitch += curDetune * ((sndPitchUpOne(cpitch / 65536) & 0xffff) - (cpitch / 65536));
    }
    return cpitch;
}

void LowPrecisionHandler(u32 i) {
    u32 j;
    s32 pbend;
    u32 ccents;
    u32 cpitch;
    u16 Modulation;
    u16 portamento;
    u32 lowDeltaTime;
    SYNTH_VOICE* sv;
    u32 cntDelta;
    u32 addFactor;
    u16 adsr_start;
    u16 adsr_delta;
    s32 vrange;
    s32 voff;

    sv = &lbl_8047AF48[i];
    if (!fn_8016246C(i) && sv->addr == NULL) {
        goto end;
    }

    lowDeltaTime = (u32)(lbl_8047AF58 - sv->lastLowCallTime);
    sv->lastLowCallTime = lbl_8047AF58;

    for (j = 0; j < 2; ++j) {
        if (sv->lfo[j].period == 0) {
            continue;
        }
        sv->lfo[j].time += lowDeltaTime;
        sv->lfo[j].value =
            sndSin((u16)((sv->lfo[j].time % sv->lfo[j].period * 16) / (sv->lfo[j].period / 256)));
        if (sv->lfo[j].value != sv->lfo[j].lastValue) {
            sv->lfo[j].lastValue = sv->lfo[j].value;
            if (sv->lfoUsedByInput[j]) {
                sv->lfoUsedByInput[j] = 0;
                sv->midiDirtyFlags |= 0x1fff;
            }
        }
    }

    if ((sv->cFlags & 0x2000) != 0) {
        sv->vibCurTime += lowDeltaTime;
        sv->vibCurOffset = (s16)sndSin((u16)((sv->vibCurTime % sv->vibPeriod * 16) / (sv->vibPeriod / 256)));
    }

    if (sv->sweepNum[0] | sv->sweepNum[1]) {
        cntDelta = (lowDeltaTime << 8) >> 4;
        addFactor = (lowDeltaTime << 4) >> 4;
        for (j = 0; j < 2; ++j) {
            if (sv->sweepNum[j] == 0) {
                continue;
            }
            sv->sweepCnt[j] -= cntDelta;
            if (sv->sweepCnt[j] <= 0) {
                sv->sweepCnt[j] = sv->sweepNum[j] << 16;
                sv->sweepOff[j] = 0;
            } else {
                sv->sweepOff[j] += (sv->sweepAdd[j] >> 12) * addFactor;
            }
        }
    }

    for (j = 0; j < 2; ++j) {
        if (sv->panning[j] == sv->panTarget[j]) {
            continue;
        }
        sv->panTime[j] -= lowDeltaTime;
        if ((s32)sv->panTime[j] <= 0) {
            sv->panning[j] = sv->panTarget[j];
            sv->panTime[j] = 0;
        } else {
            sv->panning[j] = sv->panTarget[j] - (sv->panTime[j] / 256) * sv->panDelta[j];
            sv->panning[j] = (s32)sv->panning[j] < 0 ? 0
                              : sv->panning[j] > 0x7f0000u ? 0x7f0000
                                                           : sv->panning[j];
        }
        sv->cFlags |= 0x200000000000ULL;
    }

    if ((sv->cFlags & 0x20000000000ULL) != 0 &&
        adsrHandleLowPrecision(&sv->pitchADSR, &adsr_start, &adsr_delta)) {
        sv->cFlags &= ~0x20000000000ULL;
    }

    ccents = sv->curNote * 65536 + (sv->curDetune * 65536) / 100;
    if ((sv->cFlags & 0x10030) != 0) {
        if (sv->midi != 0xff) {
            pbend = inpGetPitchBend((u8*)sv);
            sv->pbLast = pbend;
            goto pbend_adjust;
        }
    } else {
        pbend = sv->pbLast;
    pbend_adjust:
        if (pbend != 0x2000) {
            pbend -= 0x2000;
            if (pbend < 0) {
                ccents += sv->pbLowerKeyRange * pbend * 8;
            } else {
                ccents += sv->pbUpperKeyRange * pbend * 8;
            }
        }
    }

    if ((sv->cFlags & 0x2000) != 0) {
        Modulation = inpGetModulation((u8*)sv);
        vrange = sv->vibKeyRange * 256 + (sv->vibCentRange * 256) / 100;
        if (sv->vibModAddScale != 0) {
            vrange += (sv->vibModAddScale * ((Modulation & 0x1ffff) >> 7)) >> 7;
        }
        if ((sv->cFlags & 0x4000) != 0) {
            voff = (sv->vibCurOffset * ((Modulation & 0x1ffff) >> 7)) >> 7;
        } else {
            voff = sv->vibCurOffset;
        }
        ccents += (vrange * voff) >> 4;
    }

    if (sv->midi != 0xff) {
        portamento = inpGetMidiCtrl(65, sv->midi, sv->midiSet);
        if (portamento != sv->portLastCtrlState || (sv->cFlags & 0x21000) == 0x20000) {
            if (portamento <= 0x1f80) {
                sv->cFlags &= ~0x400ULL;
            } else {
                if ((sv->cFlags & 0x400) == 0) {
                    synthInitPortamento(sv);
                }
                sv->cFlags |= 0x400;
            }
            sv->cFlags |= 0x1000;
            sv->portLastCtrlState = portamento;
        }
    }

    ccents = apply_portamento(sv, ccents, lowDeltaTime);
    if ((sv->cFlags & 0x20000000000ULL) != 0) {
        ccents += sv->pitchADSRRange * (sv->pitchADSR.currentVolume >> 16) >> 7;
    }

    cpitch = convert_cents(sv, ccents);
    cpitch += sv->sweepOff[0] + sv->sweepOff[1];
    cpitch = ((cpitch >> 16) * inpGetDoppler((u8*)sv)) >> 13;
    sv->curPitch = cpitch;

    hwSetPitch(i, cpitch);
    synthAddJob(sv, 0, 0xf00);

end:
    UpdateTimeMIDICtrl(sv);
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_ZERO)
#pragma fp_contract off
void ZeroOffsetHandler(u32 i) {
    SYNTH_VOICE* sv;
    u32 lowDeltaTime;
    u16 Modulation;
    f32 vol;
    f32 auxa;
    f32 auxb;
    f32 f;
    f32 voiceVol;
    u32 volUpdate;
    f32 lfo;
    f32 scale;
    f32 mscale;
    s32 pan;
    f32 preVol;
    f32 postVol;

    sv = &lbl_8047AF48[i];
    if (!fn_8016246C(i) && sv->addr == NULL) {
        goto end;
    }

    lowDeltaTime = (u32)(lbl_8047AF58 - sv->lastZeroCallTime);
    sv->lastZeroCallTime = lbl_8047AF58;

    if ((sv->cFlags & 0x8000) != 0) {
        sv->envCurrent += sv->envDelta * (lowDeltaTime >> 8);
        if (sv->envDelta < 0) {
            if ((s32)sv->envTarget >= (s32)sv->envCurrent) {
                sv->envCurrent = sv->envTarget;
                sv->cFlags &= ~0x8000ULL;
            }
        } else if ((s32)sv->envTarget <= (s32)sv->envCurrent) {
            sv->envCurrent = sv->envTarget;
            sv->cFlags &= ~0x8000ULL;
        }
        sv->volume = sv->envCurrent;
        volUpdate = 1;
    } else {
        volUpdate = (sv->cFlags & 0x100000000000ULL) != 0;
    }

    sv->cFlags &= ~0x100000000000ULL;

    f = lbl_80434E64[sv->vGroup].pauseVol * lbl_80434E64[sv->vGroup].volume *
        lbl_80434E64[sv->fxFlag ? 22 : 21].volume;

    if (sv->track != 0xff) {
        vol = f * (f32)lbl_80435464[sv->track] * (1.f / 127.f);
    } else {
        vol = f;
    }

    if (vol != sv->lastVolFaderScale) {
        sv->lastVolFaderScale = vol;
        volUpdate = 1;
    }

    voiceVol = (f32)sv->volume * (1.f / (8192.f * 1016.f));

    if ((sv->treScale | sv->treModAddScale) != 0) {
        Modulation = inpGetModulation((u8*)sv);
        lfo = (f32)(8192 - ((8192 - ((s16)inpGetTremolo((u8*)sv) - 8192)) >> 1)) * (1.f / 8192.f);
        mscale = 1.f - (f32)Modulation * (4096 - sv->treModAddScale) * 1.490207e-08f;
        scale = (f32)sv->treScale * mscale * (1.f / 4096.f);
        if (sv->treCurScale < scale) {
            if ((sv->treCurScale += 0.2f) > scale) {
                sv->treCurScale = scale;
            }
        } else if (sv->treCurScale > scale) {
            if ((sv->treCurScale -= 0.2f) < scale) {
                sv->treCurScale = scale;
            }
        }
        voiceVol *= 1.f - lfo * (1.f - sv->treCurScale);
        volUpdate = 1;
    }

    if ((lbl_8047AF44 & 1) == 0) {
        if ((sv->cFlags & 0x200000000000ULL) != 0 || (sv->midiDirtyFlags & 0x6) != 0) {
            sv->cFlags &= ~0x200000000000ULL;
            pan = sv->panning[0] + (inpGetPanning((u8*)sv) - 8192) * 0x200;
            sv->lastPan = pan < 0 ? 0 : (pan > 0x7f0000 ? 0x7f0000 : pan);

            if ((lbl_8047AF44 & 2) != 0) {
                if ((sv->lastSPan = sv->panning[1] + inpGetSurroundPanning((u8*)sv) * 512) > 0x7f0000) {
                    sv->lastSPan = 0x7f0000;
                }
            } else {
                sv->lastSPan = 0;
            }
            volUpdate = 1;
        } else if ((lbl_8047AF44 & 2) == 0) {
            sv->lastSPan = 0;
        }
    } else {
        sv->lastPan = 0x400000;
        sv->lastSPan = 0;
        volUpdate |= (sv->cFlags & 0x200000000000ULL) != 0;
        sv->cFlags &= ~0x200000000000ULL;
    }

    if (volUpdate || (sv->midiDirtyFlags & 0xf01) != 0) {
        preVol = voiceVol;
        postVol = voiceVol * vol * (f32)inpGetVolume((u8*)sv) * (1.f / 16383.f);
        auxa = ((f32)sv->revVolOffset * (1.f / 127.f)) +
               ((preVol * (f32)inpGetPreAuxA((u8*)sv) * (1.f / 16383.f)) +
                ((f32)sv->revVolScale *
                 (postVol * (f32)inpGetReverb((u8*)sv) * (1.f / 16383.f)) * (1.f / 127.f)));
        auxb = (preVol * (f32)inpGetPreAuxB((u8*)sv) * (1.f / 16383.f)) +
               (postVol * (f32)inpGetPostAuxB((u8*)sv) * (1.f / 16383.f));
        sv->curOutputVolume = (u16)(postVol * 32767.f);
        hwSetVolume(i, sv->volTable, postVol, sv->lastPan, sv->lastSPan, auxa, auxb);
    }

    if (sv->age != 0) {
        if ((s32)(sv->age -= sv->ageSpeed * lowDeltaTime) < 0) {
            sv->age = 0;
        }
        fn_80162494(i, sv->prio << 24 | sv->age >> 15);
    }

    synthAddJob(sv, 1, (5 - fn_80162464()) * 256);

end:
    UpdateTimeMIDICtrl(sv);
}
#pragma fp_contract on
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_HANDLE)
extern void LowPrecisionHandler(u32 i);
extern void ZeroOffsetHandler(u32 i);

static inline void EventHandler(u32 i) {
    SYNTH_VOICE* sv;

    sv = &lbl_8047AF48[i];
    if (!fn_8016246C(i) && sv->addr == NULL) {
        goto end;
    }

    macSetPedalState(sv, inpGetPedal((u8*)sv) > 0x1f80);

    if ((sv->cFlags & 0x20) != 0) {
        sv->cFlags &= ~0x20ULL;
        sv->cFlags |= 0x10;
        hwStart(i, sv->studio);
    }

    if ((sv->cFlags & 0x10000000090ULL) == 0x90) {
        sv->cFlags &= ~0x90ULL;
        hwKeyOff(i);
        if ((sv->cFlags & 0x20000000000ULL) != 0 && adsrRelease(&sv->pitchADSR)) {
            sv->cFlags &= ~0x20000000000ULL;
        }
    }

end:
    UpdateTimeMIDICtrl(sv);
}

static inline void HandleJobQueue(SYNTH_QUEUE** queueRoot, void (*handler)(u32)) {
    SYNTH_QUEUE* jq;
    SYNTH_QUEUE* nextJq;

    jq = *queueRoot;
    while (jq != NULL) {
        nextJq = jq->next;
        jq->jobTabIndex = 0xff;
        if (!lbl_8047AF48[jq->voice].block) {
            handler(jq->voice);
        }
        jq = nextJq;
    }

    *queueRoot = NULL;
}

static inline void HandleVoices(void) {
    SYNTH_JOBTAB* jTab;

    jTab = &lbl_804354A4[lbl_8047AF19];
    HandleJobQueue(&jTab->lowPrecision, LowPrecisionHandler);
    HandleJobQueue(&jTab->event, EventHandler);
    HandleJobQueue(&jTab->zeroOffset, ZeroOffsetHandler);
    lbl_8047AF19 = (lbl_8047AF19 + 1) & 0x1f;
}

static inline void HandleFaderTermination(SYNTHMasterFader* smf) {
    switch (smf->seqMode) {
    case 1: seqStop(smf->seqId); break;
    case 2: seqPause(smf->seqId); break;
    case 3: seqMute(smf->seqId, 0, 0); break;
    }
}

#pragma fp_contract off
void synthHandle(u32 deltaTime) {
    u32 i;
    u32 s;
    SYNTHMasterFader* smf;
    SynthInfo* synthInfo;
    u32 pauseFlags;
    u32 testFlag;
    u8* synthBase;

    /* These globals were contiguous in the original TU; retain its shared address base. */
    synthBase = (u8*)lbl_80434A10;
    synthInfo = (SynthInfo*)(synthBase + 0x240);
    if (synthInfo->numSamples == 0) {
        return;
    }

    macHandle(deltaTime);
    HandleVoices();

    if (fn_80162464() == 0) {
        if ((lbl_8047AF40 | lbl_8047AF3C) != 0) {
            for (i = 0, smf = (SYNTHMasterFader*)(synthBase + 0x454), testFlag = 1;
                 i < 32; testFlag <<= 1, ++i, ++smf) {
                if ((lbl_8047AF40 & testFlag) != 0) {
                    smf->volume = smf->target - smf->time * (smf->target - smf->start);
                    if ((smf->time -= smf->deltaTime) <= lbl_8047D3A8) {
                        smf->volume = smf->target;
                        HandleFaderTermination(smf);
                        if ((lbl_8047AF40 &= ~testFlag) == 0 && lbl_8047AF3C == 0) {
                            break;
                        }
                    }
                }

                pauseFlags = lbl_8047AF3C;
                if ((pauseFlags & testFlag) != 0) {
                    smf->pauseVol = smf->pauseTarget - smf->pauseTime * (smf->pauseTarget - smf->pauseStart);
                    if ((smf->pauseTime -= smf->pauseDeltaTime) <= lbl_8047D3A8) {
                        pauseFlags &= ~testFlag;
                        smf->pauseVol = smf->pauseTarget;
                        if ((lbl_8047AF3C = pauseFlags) == 0 && lbl_8047AF40 == 0) {
                            break;
                        }
                    }
                }
            }
        }

        for (s = 0; s < 8; ++s) {
            if (lbl_8047AF34[s] != 0xff) {
                union {
                    void* buffer[3];
                    u16 para[4];
                } info;
                for (i = 0; i < 4; ++i) {
                    info.para[i] = fn_80161934(s, i, lbl_8047AF34[s], lbl_8047AF2C[s]);
                }
                ((void (**)(u32, void*, void*))(synthBase + 0xc34))[s](
                    1, &info, ((void**)(synthBase + 0xc14))[s]);
            }

            if (lbl_8047AF24[s] != 0xff) {
                union {
                    void* buffer[3];
                    u16 para[4];
                } info;
                for (i = 0; i < 4; ++i) {
                    info.para[i] = fn_801619E8(s, i, lbl_8047AF24[s], lbl_8047AF1C[s]);
                }
                ((void (**)(u32, void*, void*))(synthBase + 0xc74))[s](
                    1, &info, ((void**)(synthBase + 0xc54))[s]);
            }
        }
    }

    fn_801631A8();
    lbl_8047AF58 += deltaTime;
}
#pragma fp_contract on
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_POST)
u32 synthHWMessageHandler(u32 mesg, u32 voiceID) {
    u32 ret;
    ret = 0;
    switch (mesg) {
    case 0:
        if (lbl_8047AF48[voiceID & 0xff].block != 0) {
            break;
        }
        vsSampleEndNotify(hwGetVirtualSampleID(voiceID & 0xff));
        if (voiceID != lbl_8047AF48[voiceID & 0xff].id) {
            break;
        }
        macSampleEndNotify(&lbl_8047AF48[voiceID & 0xff]);
        break;
    case 1:
        voiceKill(voiceID & 0xff);
        break;
    case 2:
        ret = fn_80159550(voiceID & 0xFF);
        break;
    case 3:
        vsSampleEndNotify(hwGetVirtualSampleID(voiceID & 0xff));
        break;
    default:
        break;
    }
    return ret;
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_INIT)
static inline void synthInitJobQueue(SYNTH_JOBTAB* jobTables) {
    u8 i;

    for (i = 0; i < 32; ++i) {
        jobTables[i].lowPrecision = NULL;
        jobTables[i].event = NULL;
        jobTables[i].zeroOffset = NULL;
    }

    lbl_8047AF19 = 0;
}

void synthInit(u32 mixFrq, u32 numVoices) {
    u32 i;
    u8* synthBase;

    synthBase = (u8*)lbl_80434A10;
    ((SynthInfo*)(synthBase + 0x240))->mixFrq = mixFrq;
    lbl_8047AF5C = 0;
    *(u32*)&lbl_8047AF58 = 0;
    synthSetBpm(120, 255, 0);
    lbl_8047AF44 = 0;
    lbl_8047AF4C = NULL;

    lbl_8047AF48 = (SYNTH_VOICE*)fn_801643D8(numVoices * sizeof(SYNTH_VOICE));
    memset(lbl_8047AF48, 0, numVoices * sizeof(SYNTH_VOICE));

    for (i = 0; i < numVoices; ++i) {
        lbl_8047AF48[i].id = 0xffffffff;
        lbl_8047AF48[i].cFlags = 0;
        lbl_8047AF48[i].age = 0;
        lbl_8047AF48[i].prio = 0;
        lbl_8047AF48[i].midi = 0xff;
        lbl_8047AF48[i].volume = 0;
        lbl_8047AF48[i].volTable = 0;
        lbl_8047AF48[i].revVolScale = 128;
        lbl_8047AF48[i].revVolOffset = 0;
        lbl_8047AF48[i].panning[0] = lbl_8047AF48[i].panTarget[0] = 0x400000;
        lbl_8047AF48[i].panning[1] = lbl_8047AF48[i].panTarget[1] = 0;
        lbl_8047AF48[i].sweepOff[0] = 0;
        lbl_8047AF48[i].sweepOff[1] = 0;
        lbl_8047AF48[i].sweepNum[0] = 0;
        lbl_8047AF48[i].sweepNum[1] = 0;
        lbl_8047AF48[i].block = 0;
        lbl_8047AF48[i].vGroup = 23;
        lbl_8047AF48[i].keyGroup = 0;
        lbl_8047AF48[i].itdMode = 1;
        lbl_8047AF48[i].lfo[0].period = 0;
        lbl_8047AF48[i].lfo[0].value = 0;
        lbl_8047AF48[i].lfo[0].lastValue = 0x7fff;
        lbl_8047AF48[i].lfo[1].period = 0;
        lbl_8047AF48[i].lfo[1].value = 0;
        lbl_8047AF48[i].lfo[1].lastValue = 0x7fff;
        lbl_8047AF48[i].portTime = 25600;
        lbl_8047AF48[i].portType = 0;
        lbl_8047AF48[i].studio = 0;
        lbl_8047AF48[i].lowPrecisionJob.voice = i;
        lbl_8047AF48[i].lowPrecisionJob.jobTabIndex = 0xff;
        lbl_8047AF48[i].zeroOffsetJob.voice = i;
        lbl_8047AF48[i].zeroOffsetJob.jobTabIndex = 0xff;
        lbl_8047AF48[i].eventJob.voice = i;
        lbl_8047AF48[i].eventJob.jobTabIndex = 0xff;
    }

    for (i = 0; i < 32; ++i) {
        ((SYNTHMasterFader*)(synthBase + 0x454))[i].volume = lbl_8047D3A8;
        ((SYNTHMasterFader*)(synthBase + 0x454))[i].pauseVol = lbl_8047D380;
        ((SYNTHMasterFader*)(synthBase + 0x454))[i].type = 4;
    }

    lbl_8047AF40 = 0;
    lbl_8047AF3C = 0;
    ((SYNTHMasterFader*)(synthBase + 0x454))[31].type = 1;

    for (i = 0; i < 8; ++i) {
        ((SYNTHMasterFader*)(synthBase + 0x454))[i + 23].type = 0;
    }

    ((SYNTHMasterFader*)(synthBase + 0x454))[21].volume = lbl_8047D380;
    ((SYNTHMasterFader*)(synthBase + 0x454))[22].volume = lbl_8047D380;
    fn_80161A9C(0);

    for (i = 0; i < 8; ++i) {
        ((void**)(synthBase + 0xc34))[i] = NULL;
        lbl_8047AF34[i] = 0xff;
        ((void**)(synthBase + 0xc74))[i] = NULL;
        lbl_8047AF24[i] = 0xff;
        (synthBase + 0xc94)[i * 2] = 0;
        (synthBase + 0xc94)[i * 2 + 1] = 0;
    }

    macInit();
    vidInit();
    synthInitAllocationAids();

    for (i = 0; i < 16; ++i) {
        ((u32*)(synthBase + 0xca4))[i] = 0;
    }

    voiceInitLastStarted();
    synthInitJobQueue((SYNTH_JOBTAB*)(synthBase + 0xa94));

    fn_8016248C((u32)synthHWMessageHandler);
}
#endif

#if !defined(SYNTH_SUFFIX_SPLIT) || defined(SYNTH_SUFFIX_EXIT)
void synthExit(void) {
    fn_80164400((u32)lbl_8047AF48);
}
#endif
