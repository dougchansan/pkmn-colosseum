/**
 * @file synth_exact_8014A280.c
 * @brief Exact MusyX synth startup island, 0x8014A280 - 0x8014B044.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/synth.c` (byte-exact matched in MP4 / Prime /
 * Strikers at GC/1.3.2). This unit owns synthGetTicksPerSecond through
 * synthStartSound.
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
extern void* memset(void* dst, int val, u32 size);
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

/* -------------------------------------------------------------------
 * synthGetTicksPerSecond / synthSetBpm's table
 * ---------------------------------------------------------------- */
#pragma force_active on

u32 synthGetTicksPerSecond(SYNTH_VOICE* svoice) {
    return lbl_80434A10[svoice->midiSet == 0xff ? 8 : svoice->midiSet][svoice->section];
}

void synthInitPortamento(SYNTH_VOICE* svoice) {
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

u32 do_voice_portamento(u8 key, u8 midi, u8 midiSet, u32 isMaster, u32* rejected) {
    u32 i;
    u32 vid;
    u32 id;
    SYNTH_VOICE* sv;
    SYNTH_VOICE* last_sv;
    u32 legatoVoiceIsStarting;

    legatoVoiceIsStarting = 0;
    vid = 0xFFFFFFFF;

    for (i = 0, sv = lbl_8047AF48; i < ((SynthInfo*)lbl_80434C50)->voiceNum; ++i, sv++) {
        if (sv->block == 0 && sv->id != 0xFFFFFFFF && sv->midi == midi && sv->midiSet == midiSet) {
            if ((sv->cFlags & 2) != 0) {
                legatoVoiceIsStarting = 1;
            }
            if ((sv->cFlags & 0x10) != 0 && (sv->cFlags & 0x10000000008) != 0x8 && fn_8016246C(i)) {
                if (vid == 0xFFFFFFFF && (sv->cFlags & 0x20002) == 0x20002) {
                    *rejected = 1;
                    return 0xFFFFFFFF;
                }
                last_sv = sv;
                sv->portCurPitch = (sv->curNote * 65536) + (sv->curDetune * 65536) / 100;
                sv->lastNote = sv->curNote;
                sv->curNote = key + ((sv->curNote & 0xff) - sv->orgNote);
                sv->orgNote = key;
                sv->curDetune = 0;
                sv->portTime = 0;
                sv->cFlags |= 0x20000;
                fn_80157360(&lbl_8047AF48[i]);
                if (vid == 0xFFFFFFFF) {
                    sv->child = 0xFFFFFFFF;
                    sv->parent = 0xFFFFFFFF;
                    vid = fn_801576C4(&lbl_8047AF48[i], isMaster);
                    id = sv->id;
                } else {
                    lbl_8047AF48[id & 0xff].child = sv->id;
                    sv->parent = id;
                    id = sv->id;
                    fn_801576C4(&lbl_8047AF48[i], 0);
                }
            }
        }
    }

    if (vid != 0xFFFFFFFF) {
        voiceSetLastStarted(last_sv);
        inpSetMidiLastNote(last_sv->midi, last_sv->midiSet, (u8)last_sv->curNote);
        *rejected = 0;
    } else {
        *rejected = legatoVoiceIsStarting;
    }
    return vid;
}

static inline u32 check_portamento(u8 key, u8 midi, u8 midiSet, u32 newVID, u32* vid) {
    u32 rejected;
    if (inpGetMidiCtrl(65, midi, midiSet) > 8064) {
        *vid = do_voice_portamento(key & 0x7f, midi, midiSet, newVID, &rejected);
        return !rejected;
    }
    *vid = 0xFFFFFFFF;
    return 1;
}

u32 StartKeymap(u16 keymapID, s16 prio, u8 maxVoices, u16 allocId, u8 key, u8 vol,
                        u8 panning, u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid,
                        u32 vidFlag, u8 vGroup, u8 studio, u32 itd);

u32 StartLayer(u16 layerID, s16 prio, u8 maxVoices, u16 allocId, u8 key, u8 vol, u8 panning,
                       u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid, u32 vidFlag,
                       u8 vGroup, u8 studio, u32 itd) {
    u16 n;
    u32 vid;
    u32 new_id;
    u32 id;
    LAYER* l;
    s32 p;
    s32 k;
    u8 v;
    u8 mKey;

    vid = 0xFFFFFFFF;
    if ((l = (LAYER*)dataGetLayer(layerID, &n)) == NULL) {
        goto end;
    }

    mKey = key & 0x7f;
    for (; n != 0; --n, l++) {
        if (l->id == 0xffff || l->keyLow > mKey || l->keyHigh < mKey) {
            continue;
        }

        k = mKey + l->transpose;
        k = k > 127 ? 127 : (k < 0 ? 0 : k);

        if ((l->id & 0xC000) == 0) {
            if (check_portamento(k, midi, midiSet, 0, &new_id)) {
                if (new_id != 0xFFFFFFFF) {
                    goto apply_new_id;
                } else {
                    goto start_new_id;
                }
            }
            continue;
        }

    start_new_id:
        if ((l->panning & 0x80) == 0) {
            p = l->panning - 0x40;
            p += panning;
            p = p < 0 ? 0 : (p > 0x7f ? 0x7f : p);
        } else {
            p = 0x80;
        }

        v = (vol * l->volume) / 0x7f;
        prio += l->prioOffset;
        prio = prio > 0xff ? 0xff : (prio < 0 ? 0 : prio);

        switch (l->id & 0xC000) {
        case 0:
            new_id = macStart(l->id, prio, maxVoices, allocId, k | (key & 0x80), v, p, midi,
                                  midiSet, section, step, trackid, 0, vGroup, studio, itd);
            break;
        case 0x4000:
            new_id = StartKeymap(l->id, prio, maxVoices, allocId, k | (key & 0x80), v, p, midi,
                                  midiSet, section, step, trackid, 0, vGroup, studio, itd);
            break;
        case 0x8000:
            new_id = StartLayer(l->id, prio, maxVoices, allocId, k | (key & 0x80), v, p, midi,
                                 midiSet, section, step, trackid, 0, vGroup, studio, itd);
            break;
        }

        if (new_id != 0xFFFFFFFF) {
        apply_new_id:
            if (vid == 0xFFFFFFFF) {
                if (vidFlag != 0) {
                    vid = fn_801576B0(&lbl_8047AF48[new_id & 0xff]);
                } else {
                    vid = new_id;
                }
            } else {
                lbl_8047AF48[id & 0xff].child = new_id;
                lbl_8047AF48[new_id & 0xff].parent = id;
            }
            id = new_id;
            while (lbl_8047AF48[id & 0xff].child != 0xFFFFFFFF) {
                lbl_8047AF48[id & 0xff].block = 1;
                id = lbl_8047AF48[id & 0xff].child;
            }
            lbl_8047AF48[id & 0xff].block = 1;
        }
    }

end:
    return vid;
}

u32 StartKeymap(u16 keymapID, s16 prio, u8 maxVoices, u16 allocId, u8 key, u8 vol,
                        u8 panning, u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid,
                        u32 vidFlag, u8 vGroup, u8 studio, u32 itd) {
    u8 o;
    KEYMAP* keymap;
    s32 p;
    s32 k;
    u32 vid;

    if ((keymap = (KEYMAP*)dataGetKeymap(keymapID)) != NULL) {
        o = key & 0x7f;
        if (keymap[o].id != 0xffff && (keymap[o].id & 0xc000) != 0x4000) {
            if ((keymap[o].panning & 0x80) == 0) {
                p = (keymap[key].panning - 0x40);
                p += panning;
                if (p < 0) {
                    panning = 0;
                } else if (p > 0x7f) {
                    panning = 0x7f;
                } else {
                    panning = p;
                }
            } else {
                panning = 0x80;
            }

            k = (key & 0x7f) + keymap[o].transpose;
            k = k > 127 ? 127 : (k < 0 ? 0 : k);

            prio += keymap[o].prioOffset;
            prio = prio > 0xff ? 0xff : (prio < 0 ? 0 : prio);

            if ((keymap[o].id & 0xc000) == 0) {
                if (!check_portamento(k & 0xff, midi, midiSet, vidFlag, &vid)) {
                    return 0xffffffff;
                }
                if (vid != 0xffffffff) {
                    return vid;
                }
                return macStart(keymap[o].id, prio, maxVoices, allocId, k | (key & 0x80), vol,
                                    panning, midi, midiSet, section, step, trackid, vidFlag,
                                    vGroup, studio, itd);
            }

            return StartLayer(keymap[o].id, prio, maxVoices, allocId, k | (key & 0x80), vol,
                               panning, midi, midiSet, section, step, trackid, vidFlag & 0xff,
                               vGroup, studio, itd);
        }
    }

    return 0xFFFFFFFF;
}

static inline void unblockAllAllocatedVoices(u32 vid) {
    u32 id;
    id = vidGetInternalId(vid);
    while (id != 0xFFFFFFFF) {
        lbl_8047AF48[id & 0xff].block = 0;
        id = lbl_8047AF48[id & 0xff].child;
    }
}

u32 synthStartSound(u16 id, u8 prio, u8 max, u8 key, u8 vol, u8 panning, u8 midi, u8 midiSet,
                u8 section, u16 step, u16 trackid, u8 vGroup, s16 prioOffset, u8 studio, u32 itd) {
    prio += prioOffset;
    prio = prio < 0 ? 0 : (prio > 0xff ? 0xff : prio);

    switch (id & 0xC000) {
    case 0: {
        u32 vid;
        if (!check_portamento(key, midi, midiSet, 1, &vid)) {
            return 0xFFFFFFFF;
        }
        if (vid != 0xFFFFFFFF) {
            return vid;
        }
        return macStart(id, prio, max, id, key, vol, panning, midi, midiSet, section, step,
                            trackid, 1, vGroup, studio, itd);
    }
    case 0x4000: {
        u32 vid = StartKeymap(id, prio, max, id, key, vol, panning, midi, midiSet, section, step,
                               trackid, 1, vGroup, studio, itd);
        if (vid != 0xFFFFFFFF) {
            unblockAllAllocatedVoices(vid);
        }
        return vid;
    }
    case 0x8000: {
        u32 vid = StartLayer(id, prio, max, id, key, vol, panning, midi, midiSet, section, step,
                              trackid, 1, vGroup, studio, itd);
        if (vid != 0xFFFFFFFF) {
            unblockAllAllocatedVoices(vid);
        }
        return vid;
    }
    default:
        return 0xFFFFFFFF;
    }
}
