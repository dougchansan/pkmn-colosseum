/**
 * @file synthmacros.c
 * @brief MusyX runtime macro interpreter (musyx/runtime/synthmacros.c),
 * 0x801525E4 - 0x80157280.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/synthmacros.c`. Boundary evidence: mcmdWait
 * confirmed at 0x801525E4 (simindex seq=0.989 vs MP4/Strikers matched
 * copies); macInit (0x80157218 + 0x68) is reference synthmacros.c's last
 * function and ends exactly at vidInit (0x80157280), reference
 * synthvoice.c's first. The mcmd motion-setter family below
 * (mcmdPanningSelect .. mcmdDopplerSelect) inlines the same-TU static
 * MotionSetterCommon; all other functions asm-only until matched.
 */
#include "dolphin/types.h"

#if !defined(SYNTHMACROS_ISOLATED)
#define SYNTHMACROS_ALL
#endif

#define FLT_EPSILON 1.19209290e-7F

/* ===================================================================
 * Local copy of the SYNTH_VOICE layout, byte-exact-proven in synth.c
 * (see src/musyx/runtime/synth.c). Kept as an independent per-TU
 * typedef, matching this codebase's established convention (each MusyX
 * TU privately re-declares the structs it needs; only snd_synthapi.c
 * uses the opaque forward-declared form).
 * =================================================================== */
#pragma pack(4)
typedef struct MSTEP { u32 para[2]; } MSTEP;

typedef struct SYNTH_QUEUE {
    struct SYNTH_QUEUE* next; // 0x0
    struct SYNTH_QUEUE* prev; // 0x4
    u8 voice;                 // 0x8
    u8 jobTabIndex;           // 0x9
} SYNTH_QUEUE; // size 0xC

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
    union {
        struct {
            u32 aTime;  // 0x0
            u32 dTime;  // 0x4
            u16 sLevel; // 0x8
            u16 pad_A;
            u32 rTime;  // 0xC
            u16 cutOff; // 0x10
            u8 aMode;   // 0x12
            u8 pad_13;
        } dls;
        struct {
            u32 aTime;
            u32 dTime;
            u16 sLevel;
            u32 rTime;
        } linear;
        u8 raw[20];
    } data;
} ADSR_VARS; // size 0x28

/* ADSR_INFO: upstream musyx/adsr.h layout (size 0x14), used only as the
 * decoded-curve scratch struct inside the mcmdSetADSR* family below. */
typedef struct ADSR_INFO {
    union {
        struct {
            s32 atime;  // 0x0
            s32 dtime;  // 0x4
            u16 slevel; // 0x8
            u16 rtime;  // 0xA
            s32 ascale; // 0xC
            s32 dscale; // 0x10
        } dls;
        struct {
            u16 atime;  // 0x0
            u16 dtime;  // 0x2
            u16 slevel; // 0x4
            u16 rtime;  // 0x6
        } linear;
    } data;
} ADSR_INFO; // size 0x14

typedef struct CTRL_SOURCE { u8 midiCtrl; u8 combine; u16 pad; s32 scale; } CTRL_SOURCE; // 0x8
typedef struct CTRL_DEST { CTRL_SOURCE source[4]; u16 oldValue; u8 numSource; u8 pad; } CTRL_DEST; // 0x24

/* 4 entries * 8 bytes = 0x20, matching callStackIndex's "& 3" masking and
 * mcmdGosub/mcmdReturn's addr+curAddr pair per entry (synth.c left this
 * opaque since it never touches callStack fields directly). */
typedef struct CALL_STACK_ENTRY { MSTEP* addr; MSTEP* curAddr; } CALL_STACK_ENTRY;

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
    s32 macState;                            // 0x4C (signed: MAC_STATE enum -- confirmed by
                                              // cmpwi/cmplwi choice in target disassembly)
    MSTEP* trapEventAddr[3];                 // 0x50
    MSTEP* trapEventCurAddr[3];              // 0x5C
    u8 trapEventAny;                         // 0x68
    u8 pad_69[3];
    CALL_STACK_ENTRY callStack[4];            // 0x6C
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

typedef struct SynthInfo {
    u32 mixFrq;      // 0x0
    u32 numSamples;  // 0x4
    u8 pad[0x210 - 8];
    u8 voiceNum;     // 0x210
    u8 maxMusic;     // 0x211
    u8 maxSFX;       // 0x212
    u8 studioNum;    // 0x213
} SynthInfo; // size 0x214
#pragma pack()

#if defined(SYNTHMACROS_SELECT_macHandleActive) && !defined(SYNTHMACROS_ALL)
u8 lbl_8047AFC8;
SYNTH_VOICE* lbl_8047AFC4;
SYNTH_VOICE* lbl_8047AFC0;
u64 lbl_8047AFB8;
MSTEP lbl_8047AFB0;
#else
extern MSTEP lbl_8047AFB0;
extern u64 lbl_8047AFB8;
extern SYNTH_VOICE* lbl_8047AFC0;
extern SYNTH_VOICE* lbl_8047AFC4;
extern u8 lbl_8047AFC8;
extern const f32 lbl_8047D3EC;
#endif

extern SYNTH_VOICE* lbl_8047AF48;   /* synthVoice */
extern u8 lbl_80434C50[];           /* synthInfo */
extern u32 lbl_804356B4[16];        /* synthGlobalVariable */
/* macRealTime is NOT lbl_8047AF58 (that is a distinct counter incremented
 * separately in synth.c). */
extern void* lbl_8047AF4C;          /* synthMessageCallback */
#define synthInfo (*(SynthInfo*)lbl_80434C50)
#define synthVoice lbl_8047AF48
#define synthGlobalVariable lbl_804356B4
#define macRealTime lbl_8047AFB8

extern u32 dataGetMacro(u16 macId);
extern void* dataGetCurve(u32 curveId);
extern void voiceFree(SYNTH_VOICE* sv);
extern void voiceSetPriority(SYNTH_VOICE* sv, u8 prio);
extern void voiceKill(u32 voice);
extern u32 voiceIsLastStarted(SYNTH_VOICE* sv);
extern void inpSetMidiLastNote(u32 midi, u32 midiSet, u32 note);
extern u16 inpGetExCtrl(SYNTH_VOICE* sv, u8 index); /* returns u16: confirmed by target's
                                                      * zero-extend (clrlwi ...,16) of the result
                                                      * inside varGet32's ctrl!=0 branch. */
extern void inpSetExCtrl(SYNTH_VOICE* sv, u8 index, s16 v); /* takes s16 -- confirmed by target's
                                                              * explicit extsh before the call in
                                                              * mcmdVarCalculation. */
extern u16 inpGetMidiCtrl(u32 ctrl, u32 midi, u32 midiSet); /* returns u16 -- confirmed by
                                                              * target's zero-extend of the
                                                              * result in mcmdPortamento. */
extern u32 vidGetInternalId(u32 vid);
extern void synthForceLowPrecisionUpdate(SYNTH_VOICE* sv);
extern void synthStartSynthJobHandling(SYNTH_VOICE* sv);
extern void synthFXCloneMidiSetup(SYNTH_VOICE* dst, SYNTH_VOICE* src);
extern void fn_80157360(SYNTH_VOICE* sv);       /* vidRemoveVoiceReferences */
extern u32 fn_8016246C(u32 voice);              /* hwIsActive */
extern void fn_80162494(u32 voice, u32 val);    /* hwSetPriority */
extern u16 fn_80162070(void);                   /* sndRand -- returns u16, confirmed by target's
                                                  * zero-extend (clrlwi ...,16) of the result
                                                  * before the modulo in mcmdWait. */
extern void fn_801621BC(u32* ms);                /* sndConvertMs */
extern u32 fn_80162214(u32 time);                /* sndConvert2Ms: time / 256 */
extern void sndConvertTicks(u32* ticks, SYNTH_VOICE* sv);
extern void fn_801603C0(u32 ctrl, u32 midi, u32 midiSet, u32 value); /* inpSetMidiCtrl */
extern void synthInitPortamento(SYNTH_VOICE* svoice);
extern u32 macStart(u16 macid, u8 priority, u8 maxVoices, u16 allocId, u8 key, u8 vol, u8 panning,
                        u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid, u8 new_vid,
                        u8 vGroup, u8 studio, u32 itd); /* macStart, defined later in this file */
extern u16 fn_8014D740(u8 midiSet, u8 midi);              /* seqGetMIDIPriority */
extern u32 fn_80157A64(u8 priority, u8 maxVoices, u16 allocId, u8 isFX); /* voiceAllocate */
extern void inpResetMidiCtrl(u8 voice, u32 arg2, u32 arg3);
extern void fn_80160ED4(u8 voice, u32 val); /* inpResetChannelDefaults */
extern u32 fn_801576C4(SYNTH_VOICE* svoice, u8 new_vid); /* vidMakeNew */
extern void hwBreak(u32 voice); /* src/musyx/musyx_range_80157280.c, real symbol */

extern void inpAddCtrl(CTRL_DEST* dst, u8 lowByte, s32 value, u8 repeat, u32 hasUpperByte);
extern void fn_8016039C(u8 midi, u8 midiSet, u32 dirtyFlag); /* inpSetGlobalMIDIDirtyFlag */
extern CTRL_DEST lbl_80435B74[8][4]; /* inpAuxA, per-studio stride 0x90 */
extern CTRL_DEST lbl_804356F4[8][4]; /* inpAuxB, per-studio stride 0x90 */
#if defined(SYNTHMACROS_SELECT_mcmdAuxAFXSelect) && !defined(SYNTHMACROS_ALL)
u64 lbl_80368CA0[4] = {
    0x0000000100000000ULL,
    0x0000000200000000ULL,
    0x0000000400000000ULL,
    0x0000000800000000ULL,
};
u32 lbl_80368CC0[4] = {0x80000001, 0x80000002, 0x80000004, 0x80000008};
#else
extern u64 lbl_80368CA0[4];
extern u32 lbl_80368CC0[4];
#endif
#if defined(SYNTHMACROS_SELECT_mcmdAuxBFXSelect) && !defined(SYNTHMACROS_ALL)
u64 lbl_80368CD0[4] = {
    0x0000001000000000ULL,
    0x0000002000000000ULL,
    0x0000004000000000ULL,
    0x0000008000000000ULL,
};
u32 lbl_80368CF0[4] = {0x80000010, 0x80000020, 0x80000040, 0x80000080};
#else
extern u64 lbl_80368CD0[4];
extern u32 lbl_80368CF0[4];
#endif

/* SAMPLE_INFO: byte-exact-proven layout, copied from src/musyx/runtime/stream.c
 * (do not re-derive). */
typedef struct SAMPLE_INFO {
    u32 info;        // 0x0
    void* addr;      // 0x4
    void* extraData; // 0x8
    u32 offset;      // 0xc
    u32 length;      // 0x10
    u32 loop;        // 0x14
    u32 loopLength;  // 0x18
    u8 compType;     // 0x1c
} SAMPLE_INFO; // size 0x20

extern s32 dataGetSample(u16 key, SAMPLE_INFO* out); /* dataGetSample */
extern void hwInitSamplePlayback(u32 voice, u16 pitch, void* smp, u32 resetState, u32 unk1C,
                                  u32 unk18, u32 initFlags, u32 setupFlag);
extern void synthKeyStateUpdate(SYNTH_VOICE* svoice); /* synth.c: synthAddJob(svoice, 2, 0) */
extern void hwSetADSR(u32 voice, ADSR_INFO* adsr, u32 mode);
extern u32 adsrConvertTimeCents(s32 tc);
extern u32 adsrSetup(ADSR_VARS* adsr);
extern f32 lbl_8036984C[129]; /* dspDLSVolTab, .data 0x204 */
extern u8 lbl_8036944C[1024]; /* dspScale2IndexTab, .data 0x400 */
#define dspDLSVolTab lbl_8036984C
#define dspScale2IndexTab lbl_8036944C
extern const f32 lbl_8047D3CC;
extern const f32 lbl_8047D3D0;
extern const f64 lbl_8047D3D8;
extern const f64 lbl_8047D3E0;

/* Public target functions are declared independently of the selection guards
 * below so every isolated function remains a well-typed translation unit. */
void DoSetPitch(SYNTH_VOICE* svoice);
u32 mcmdWait(SYNTH_VOICE* svoice, MSTEP* cstep);
u32 mcmdGosub(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdLoop(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPlayMacro(SYNTH_VOICE* svoice, MSTEP* cstep);
u32 mcmdAddKey(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdStartSample(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdVibrato(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSetADSR(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSetADSRFromCtrl(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSetPitchADSR(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSetPanning(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSetSurroundPanning(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdScaleVolume(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdEnvelope(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdFadeIn(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdRandomKey(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdVolumeSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPanningSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPitchWheelSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdModWheelSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPedalSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPortamentoSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdReverbSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPreAuxASelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPreAuxBSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPostAuxBSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSurroundPanningSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdDopplerSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdTremoloSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdAuxAFXSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdAuxBFXSelect(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdPortamento(SYNTH_VOICE* svoice, MSTEP* cstep);
s16 varGet(SYNTH_VOICE* svoice, u32 ctrl, u8 index);
void mcmdVarCalculation(SYNTH_VOICE* svoice, MSTEP* cstep, u8 op);
void mcmdIfVarCompare(SYNTH_VOICE* svoice, MSTEP* cstep, u8 cmp);
void mcmdSendMessage(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdGetVID(SYNTH_VOICE* svoice, MSTEP* cstep);
void mcmdSetKeyGroup(SYNTH_VOICE* svoice, MSTEP* cstep);
void macHandleActive(SYNTH_VOICE* svoice);
void macHandle(u32 deltaTime);
void macSampleEndNotify(SYNTH_VOICE* svoice);
void macSetExternalKeyoff(SYNTH_VOICE* svoice);
void macSetPedalState(SYNTH_VOICE* svoice, u32 state);
void TimeQueueAdd(SYNTH_VOICE* svoice);
#if defined(SYNTHMACROS_SELECT_macStart) && !defined(SYNTHMACROS_ALL)
static inline void macMakeActive(SYNTH_VOICE* svoice);
static inline void macMakeInactive(SYNTH_VOICE* svoice, s32 newState);
#else
void macMakeActive(SYNTH_VOICE* svoice);
void macMakeInactive(SYNTH_VOICE* svoice, s32 newState);
#endif
void macInit(void);

#define MIN(a, b) ((a) > (b) ? (b) : (a))
static inline void SelectSourceCommon(SYNTH_VOICE* svoice, CTRL_DEST* dest, MSTEP* cstep,
                                      u64 tstflag, u32 dirtyFlag) {
    u8 comb;
    s32 scale;

    if (!(svoice->cFlags & tstflag)) {
        comb = 0;
        svoice->cFlags |= tstflag;
    } else {
        comb = cstep->para[1] & 0xFF;
    }
    scale = ((s16)(cstep->para[0] >> 16) << 16) / 100;
    if (scale < 0) {
        scale -= ((s8)(cstep->para[1] >> 0x10) << 8) / 100;
    } else {
        scale += ((s8)(cstep->para[1] >> 0x10) << 8) / 100;
    }
    inpAddCtrl(dest, (u8)(cstep->para[0] >> 8), scale, comb,
               (u8)(cstep->para[1] >> 8) != 0);
    if ((dirtyFlag & 0x80000000u) != 0) {
        fn_8016039C(svoice->midi, svoice->midiSet, dirtyFlag);
    } else {
        svoice->midiDirtyFlags |= dirtyFlag;
    }
}
#define PF_DEFINE_MOTION_SETTER(name, initMask, dataOffset, doneMask) \
void name(SYNTH_VOICE* ctx, MSTEP* cmd) { \
    SelectSourceCommon(ctx, (CTRL_DEST*)((u8*)ctx + (dataOffset)), cmd, (initMask), (doneMask)); \
}
/* mcmdVolumeSelect = mcmdVolumeSelect (inpVolume@0x218, tstflag 0x80000, dirty 1);
 * identified from the SelectSource offset table below, not simindex (which
 * cannot distinguish these 260B siblings from each other). */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdVolumeSelect)
PF_DEFINE_MOTION_SETTER(mcmdVolumeSelect, 0x00080000ULL, 0x218, 0x0001u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPanningSelect)
PF_DEFINE_MOTION_SETTER(mcmdPanningSelect, 0x00100000ULL, 0x23C, 0x0002u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPitchWheelSelect)
PF_DEFINE_MOTION_SETTER(mcmdPitchWheelSelect, 0x00200000ULL, 0x284, 0x0008u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdModWheelSelect)
PF_DEFINE_MOTION_SETTER(mcmdModWheelSelect, 0x00400000ULL, 0x2CC, 0x0020u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPedalSelect)
PF_DEFINE_MOTION_SETTER(mcmdPedalSelect, 0x02000000ULL, 0x2F0, 0x0040u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPortamentoSelect)
PF_DEFINE_MOTION_SETTER(mcmdPortamentoSelect, 0x01000000ULL, 0x314, 0x0080u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdReverbSelect)
PF_DEFINE_MOTION_SETTER(mcmdReverbSelect, 0x00800000ULL, 0x35C, 0x0200u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPreAuxASelect)
PF_DEFINE_MOTION_SETTER(mcmdPreAuxASelect, 0x20000000ULL, 0x338, 0x0100u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPreAuxBSelect)
PF_DEFINE_MOTION_SETTER(mcmdPreAuxBSelect, 0x40000000ULL, 0x380, 0x0400u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPostAuxBSelect)
PF_DEFINE_MOTION_SETTER(mcmdPostAuxBSelect, 0x80000000ULL, 0x3A4, 0x0800u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSurroundPanningSelect)
PF_DEFINE_MOTION_SETTER(mcmdSurroundPanningSelect, 0x04000000ULL, 0x260, 0x0004u)
#endif
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdDopplerSelect)
PF_DEFINE_MOTION_SETTER(mcmdDopplerSelect, 0x08000000ULL, 0x2A8, 0x0010u)
#endif
/* mcmdTremoloSelect = mcmdTremoloSelect (inpTremolo@0x3C8, tstflag 0x10000000,
 * dirty 0x1000) -- last of the 13-member SelectSource family. */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdTremoloSelect)
PF_DEFINE_MOTION_SETTER(mcmdTremoloSelect, 0x10000000ULL, 0x3C8, 0x1000u)
#endif
#undef PF_DEFINE_MOTION_SETTER

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdAuxAFXSelect)
void mcmdAuxAFXSelect(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 i = (u8)(cstep->para[1] >> 0x18);
    SelectSourceCommon(svoice, &lbl_80435B74[svoice->studio][i], cstep, lbl_80368CA0[i],
                       lbl_80368CC0[i]);
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdAuxBFXSelect)
void mcmdAuxBFXSelect(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 i = (u8)(cstep->para[1] >> 0x18);
    SelectSourceCommon(svoice, &lbl_804356F4[svoice->studio][i], cstep, lbl_80368CD0[i],
                       lbl_80368CF0[i]);
}
#endif

/* ===================================================================
 * DoSetPitch: self-contained, no external callees.
 * =================================================================== */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_DoSetPitch)
void DoSetPitch(SYNTH_VOICE* svoice) {
    u32 f;
    u32 of;
    u32 i;
    u32 no;
    s32 key;
    u8 oKey;
    static u16 kf[14] = {
        4096, 4339, 4597, 4871, 5160, 5467, 5792, 6137, 6502, 6888, 7298, 7732, 8192,
    };
    u32 frq;
    u32 ofrq;

    frq = svoice->playFrq & 0xFFFFFF;
    ofrq = svoice->sInfo & 0xFFFFFF;

    if (ofrq == frq) {
        svoice->curNote = (u8)(svoice->sInfo >> 24);
        svoice->curDetune = 0;
    } else if (ofrq < frq) {
        f = (frq << 12) / ofrq;
        of = f >> 12;

        for (no = 0; no < 11; no++) {
            if (of < (1u << (no + 1))) {
                break;
            }
        }

        f /= (1u << no);

        for (i = 11;; i--) {
            if (f > kf[i]) {
                break;
            }
        }

        svoice->curNote = (svoice->sInfo >> 24) + (no * 12) + i;
        svoice->curDetune = ((f - kf[i]) * 100) / (kf[i + 1] - kf[i]);
    } else {
        f = (ofrq << 12) / frq;
        of = f >> 12;

        for (no = 0; no < 11; no++) {
            if (of < (1u << (no + 1))) {
                break;
            }
        }

        f /= (1u << no);

        for (i = 11;; i--) {
            if (f > kf[i]) {
                break;
            }
        }

        key = i + (no * 12);
        oKey = (u8)(svoice->sInfo >> 24);
        if (key > oKey) {
            svoice->curNote = svoice->curDetune = 0;
        } else {
            svoice->curNote = oKey - key;
            svoice->curDetune = ((kf[i] - f) * 100) / (kf[i + 1] - kf[i]);
        }
    }
}
#endif

/* ===================================================================
 * varGet: reference declares varGet32 (non-static helper) called from
 * both varGet and mcmdIfVarCompare/mcmdVarCalculation/etc.; only varGet
 * survives as a standalone symbol in this build (0x68 bytes), so
 * varGet32 must be a same-TU static that auto-inlines at every call
 * site (same trick validated by MotionSetterCommon/SelectSourceCommon).
 * =================================================================== */
static inline s32 varGet32(SYNTH_VOICE* svoice, u32 ctrl, u8 index) {
    if (ctrl != 0) {
        return inpGetExCtrl(svoice, index);
    }
    index &= 0x1f;
    return index < 16 ? svoice->local_vars[index] : synthGlobalVariable[index - 16];
}

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_varGet)
s16 varGet(SYNTH_VOICE* svoice, u32 ctrl, u8 index) {
    return (s16)varGet32(svoice, ctrl, index);
}
#endif

static inline void varSet32(SYNTH_VOICE* svoice, u32 ctrl, u8 index, s32 v) {
    if (ctrl != 0) {
        inpSetExCtrl(svoice, index, v);
        return;
    }
    index &= 0x1f;
    if (index < 16) {
        svoice->local_vars[index] = v;
        return;
    }
    synthGlobalVariable[index - 16] = v;
}

static inline void varSet(SYNTH_VOICE* svoice, u32 ctrl, u8 index, s16 v) {
    varSet32(svoice, ctrl, index, v);
}

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdGetVID)
void mcmdGetVID(SYNTH_VOICE* svoice, MSTEP* cstep) {
    if ((u8)(cstep->para[0] >> 0x10) == 0) {
        varSet32(svoice, 0, (u8)(cstep->para[0] >> 8), *(s32*)((u8*)svoice->vidList + 8));
    } else {
        varSet32(svoice, 0, (u8)(cstep->para[0] >> 8), svoice->lastVID);
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdVarCalculation)
void mcmdVarCalculation(SYNTH_VOICE* svoice, MSTEP* cstep, u8 op) {
    s16 s1;
    s16 s2;
    s32 t;

    s1 = (s16)varGet32(svoice, (u8)(cstep->para[0] >> 24), (u8)cstep->para[1]);
    if (op == 4) {
        s2 = (s16)(cstep->para[1] >> 8);
    } else {
        s2 = (s16)varGet32(svoice, (u8)(cstep->para[1] >> 8), (u8)(cstep->para[1] >> 16));
    }
    switch (op) {
    case 4:
    case 0:
        t = (s1 + s2);
        break;
    case 1:
        t = (s1 - s2);
        break;
    case 2:
        t = (s1 * s2);
        break;
    case 3:
        t = s2 != 0 ? (s1 / s2) : 0;
        break;
    }

    varSet(svoice, (u8)(cstep->para[0] >> 8), (u8)(cstep->para[0] >> 0x10),
           (t < -0x8000 ? -0x8000 : t > 0x7FFF ? 0x7FFF : t));
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdIfVarCompare)
void mcmdIfVarCompare(SYNTH_VOICE* svoice, MSTEP* cstep, u8 cmp) {
    s32 a;
    s32 b;
    u8 result;

    a = varGet32(svoice, (u8)(cstep->para[0] >> 8), (u8)(cstep->para[0] >> 0x10));
    b = varGet32(svoice, (u8)(cstep->para[0] >> 0x18), (u8)cstep->para[1]);

    switch (cmp) {
    case 0:
        result = !(b - a);
        break;
    case 1:
        result = (a < b);
        break;
    }

    if ((u8)(cstep->para[1] >> 8) != 0) {
        result = !result;
    }
    if ((u8)result != 0) {
        svoice->curAddr = svoice->addr + (u16)(cstep->para[1] >> 0x10);
    }
}
#endif

/* ===================================================================
 * Active-macro / time-queue list management. ExecuteTrap, HasHWEventTrap,
 * CheckHWEventTrap and UnYieldMacro are file-static helpers in the
 * reference with no standalone symbols in this build -- expected to
 * auto-inline at their call sites.
 * =================================================================== */
static inline u32 ExecuteTrap(SYNTH_VOICE* svoice, u8 trapType) {
    if (svoice->trapEventAny != 0 && svoice->trapEventAddr[trapType] != 0) {
        svoice->curAddr = svoice->trapEventCurAddr[trapType];
        svoice->addr = svoice->trapEventAddr[trapType];
        svoice->trapEventAddr[trapType] = 0;
        macMakeActive(svoice);
        return 1;
    }
    return 0;
}

static inline u32 HasHWEventTrap(SYNTH_VOICE* svoice) {
    if (svoice->trapEventAny != 0) {
        return svoice->trapEventAddr[1] != 0;
    }
    return 0;
}

static inline void CheckHWEventTrap(SYNTH_VOICE* svoice) {
    if ((svoice->cFlags & 0x20) == 0 && !fn_8016246C(svoice->id & 0xFF)) {
        ExecuteTrap(svoice, 1);
    }
}

/* macPostMessage inlined (no standalone symbol in this build's call sites). */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSendMessage)
void mcmdSendMessage(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u8 i;
    s32 mesg;
    u16 macro;
    u32 vid;
    SYNTH_VOICE* sv;

    mesg = varGet32(svoice, 0, (u8)(cstep->para[1] >> 8));

    if (!(u8)(cstep->para[0] >> 8)) {
        macro = (u16)(cstep->para[0] >> 16);
        if (macro != 0xFFFF) {
            for (i = 0; i < synthInfo.voiceNum; ++i) {
                if (synthVoice[i].addr != 0 && macro == synthVoice[i].macroId) {
                    vid = *(u32*)((u8*)synthVoice[i].vidList + 8);
                    if ((vid = vidGetInternalId(vid)) != (u32)-1 &&
                        (sv = &synthVoice[vid & 0xFF])->mesgNum < 4) {
                        ++sv->mesgNum;
                        sv->mesgQueue[sv->mesgWrite] = mesg;
                        sv->mesgWrite = (sv->mesgWrite + 1) & 3;
                        ExecuteTrap(sv, 2);
                    }
                }
            }
        } else if (lbl_8047AF4C != 0) {
            ((void (*)(u32, s32))lbl_8047AF4C)(*(u32*)((u8*)svoice->vidList + 8), mesg);
        }
    } else {
        vid = varGet32(svoice, 0, (u8)cstep->para[1]);
        if ((vid = vidGetInternalId(vid)) != (u32)-1 && (sv = &synthVoice[vid & 0xFF])->mesgNum < 4) {
            ++sv->mesgNum;
            sv->mesgQueue[sv->mesgWrite] = mesg;
            sv->mesgWrite = (sv->mesgWrite + 1) & 3;
            ExecuteTrap(sv, 2);
        }
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSetKeyGroup)
void mcmdSetKeyGroup(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 i;
    u8 kg;
    u32 kill;

    svoice->keyGroup = 0;
    kg = (u8)(cstep->para[0] >> 8);
    kill = (u8)(cstep->para[0] >> 0x10) != 0;

    if (kg) {
        for (i = 0; i < synthInfo.voiceNum; ++i) {
            if (synthVoice[i].addr != 0 && (synthVoice[i].cFlags & 0x2) == 0 &&
                kg == synthVoice[i].keyGroup) {
                if (!kill) {
                    macSetExternalKeyoff(&synthVoice[i]);
                } else {
                    voiceKill(i);
                }
            }
        }
        svoice->keyGroup = kg;
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macSampleEndNotify)
void macSampleEndNotify(SYNTH_VOICE* sv) {
    if (sv->macState != 1) {
        return;
    }
    if (!ExecuteTrap(sv, 1) && (sv->cFlags & 0x40000)) {
        macMakeActive(sv);
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macSetExternalKeyoff)
void macSetExternalKeyoff(SYNTH_VOICE* sv) {
    sv->cFlags |= 8;
    if (!sv->addr) {
        return;
    }
    if (!(sv->cFlags & 0x10000000000ULL)) {
        if (!ExecuteTrap(sv, 0) && (sv->cFlags & 0x4)) {
            macMakeActive(sv);
        }
    } else {
        sv->cFlags |= 0x40000000000ULL;
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macSetPedalState)
void macSetPedalState(SYNTH_VOICE* svoice, u32 state) {
    if (state != 0) {
        svoice->cFlags |= 0x10000000000ULL;
    } else {
        if (svoice->addr && (svoice->cFlags & 0x40000000000ULL)) {
            if (!ExecuteTrap(svoice, 0) && (svoice->cFlags & 0x4)) {
                macMakeActive(svoice);
            }
        }
        svoice->cFlags &= ~(0x10000000000ULL | 0x40000000000ULL);
    }
}
#endif

#define macActiveMacroRoot lbl_8047AFC4
#define macTimeQueueRoot lbl_8047AFC0

/* ===================================================================
 * Macro-command interpreter leaves. Ported in the order confirmed by
 * bl-target forensics against the target object (see report).
 * =================================================================== */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdWait)
u32 mcmdWait(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 w;
    u32 ms;

    if ((ms = (u16)(cstep->para[1] >> 0x10))) {
        if (((u8)(cstep->para[0] >> 8) & 1)) {
            if (svoice->cFlags & 8) {
                if (!(svoice->cFlags & 0x10000000000ULL)) {
                    return 0;
                }
                svoice->cFlags |= 0x40000000000ULL;
            }
            svoice->cFlags |= 4;
        } else {
            svoice->cFlags &= ~4ULL;
        }

        if (((u8)(cstep->para[0] >> 0x18) & 1)) {
            if (!(svoice->cFlags & 0x20) && !fn_8016246C(svoice->id & 0xFF)) {
                return 0;
            }
            svoice->cFlags |= 0x40000;
        } else {
            svoice->cFlags &= ~0x40000ULL;
        }

        if (((u8)(cstep->para[0] >> 0x10)) & 1) {
            ms = fn_80162070() % ms;
        }

        if (ms != 0xFFFF) {
            if ((w = ((u8)(cstep->para[1] >> 0x8) & 1) != 0)) {
                fn_801621BC(&ms);
            } else {
                sndConvertTicks(&ms, svoice);
            }

            if (w != 0) {
                if ((u8)cstep->para[1] & 1) {
                    svoice->wait = svoice->macStartTime + ms;
                } else {
                    svoice->wait = macRealTime + ms;
                }
            } else {
                if ((u8)cstep->para[1] & 1) {
                    svoice->wait = ms;
                } else {
                    svoice->wait = svoice->waitTime + ms;
                }
            }

            if (!(svoice->wait > macRealTime)) {
                svoice->waitTime = svoice->wait;
                svoice->wait = 0;
            }
        } else {
            svoice->wait = (u64)-1;
        }

        if (svoice->wait != 0) {
            if (svoice->wait != (u64)-1) {
                TimeQueueAdd(svoice);
            }
            macMakeInactive(svoice, 1);
            return 1;
        }
    }

    return 0;
}
#endif

static inline u32 mcmdEndOfMacro(SYNTH_VOICE* svoice) {
    fn_80157360(svoice);
    voiceFree(svoice);
    return 1;
}

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdGosub)
u32 mcmdGosub(SYNTH_VOICE* svoice, MSTEP* cstep) {
    MSTEP* addr;
    if ((addr = (MSTEP*)dataGetMacro((u16)(cstep->para[0] >> 0x10))) != 0) {
        svoice->callStackIndex = (svoice->callStackIndex + 1) & 3;
        svoice->callStack[svoice->callStackIndex].addr = svoice->addr;
        svoice->callStack[svoice->callStackIndex].curAddr = svoice->curAddr;
        if (++svoice->callStackEntryNum > 4) {
            svoice->callStackEntryNum = 4;
        }
        svoice->addr = addr;
        svoice->curAddr = addr + (u16)cstep->para[1];
        return 0;
    }
    return mcmdEndOfMacro(svoice);
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdLoop)
void mcmdLoop(SYNTH_VOICE* svoice, MSTEP* cstep) {
    if (svoice->loop == 0) {
        if ((u8)(cstep->para[0] >> 16) & 1) {
            svoice->loop = fn_80162070() % (u16)(cstep->para[1] >> 16);
        } else {
            svoice->loop = (u16)(cstep->para[1] >> 16);
        }

        if (svoice->loop == 0xFFFF) {
            goto skip;
        }
        ++svoice->loop;
    } else if (svoice->loop == 0xFFFF) {
        goto skip;
    }

    if (--svoice->loop == 0) {
        return;
    }
skip:
    if (((u8)(cstep->para[0] >> 8) & 1) != 0 && (svoice->cFlags & 0x10000000008ULL) == 0x8ULL) {
        svoice->loop = 0;
    } else if (((u8)(cstep->para[0] >> 0x18) & 1) && (svoice->cFlags & 0x20) == 0 &&
               !fn_8016246C(svoice->id & 0xFF)) {
        svoice->loop = 0;
    } else {
        svoice->curAddr = svoice->addr + ((u16)cstep->para[1]);
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdAddKey)
u32 mcmdAddKey(SYNTH_VOICE* svoice, MSTEP* cstep) {
    if ((u8)(cstep->para[0] >> 0x18) == 0) {
        svoice->curNote += (s8)(u8)(cstep->para[0] >> 8);
    } else {
        svoice->curNote = (u16)svoice->orgNote + (s16)(s8)(u8)(cstep->para[0] >> 8);
    }

    svoice->curNote = (s16)svoice->curNote < 0 ? 0 : svoice->curNote > 0x7f ? 0x7f : svoice->curNote;
    svoice->curDetune = (s8)(cstep->para[0] >> 0x10);

    if (voiceIsLastStarted(svoice) != 0) {
        inpSetMidiLastNote(svoice->midi, svoice->midiSet, (u8)svoice->curNote);
    }
    cstep->para[0] = 4;
    return mcmdWait(svoice, cstep);
}
#endif

/* mcmdStartSample */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdStartSample)
void mcmdStartSample(SYNTH_VOICE* svoice, MSTEP* cstep) {
    static SAMPLE_INFO newsmp;
    u16 smp;

    smp = (u16)(cstep->para[0] >> 8);
    if (dataGetSample(smp, &newsmp) != 0) {
        return;
    }

    switch ((u8)(cstep->para[0] >> 0x18)) {
    case 0:
        newsmp.offset = cstep->para[1];
        break;
    case 1:
        newsmp.offset = ((u8)(0x7f - (svoice->volume >> 0x10)) * (u32)cstep->para[1]) / 0x7f;
        break;
    case 2:
        newsmp.offset = ((u8)((svoice->volume >> 0x10)) * (u32)cstep->para[1]) / 0x7f;
        break;
    default:
        newsmp.offset = 0;
        break;
    }

    if (newsmp.offset >= newsmp.length) {
        newsmp.offset = newsmp.length - 1;
    }

    hwInitSamplePlayback(svoice->id & 0xFF, smp, &newsmp, (svoice->cFlags & 0x100) == 0,
                          ((u32)svoice->prio << 24) | ((u32)svoice->age >> 15), svoice->id,
                          (svoice->cFlags & 0x80000000000ULL) == 0, svoice->itdMode);

    svoice->sInfo = newsmp.info;

    if (svoice->playFrq != (u32)-1) {
        DoSetPitch(svoice);
    }
    svoice->cFlags |= 0x20;
    synthKeyStateUpdate(svoice);
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdVibrato)
void mcmdVibrato(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 time;
    s8 kr;
    s8 cr;

    if ((u8)(cstep->para[0] >> 0x18) & 3) {
        svoice->cFlags |= 0x4000;
    } else {
        svoice->cFlags &= ~0x4000ULL;
    }

    time = (u16)(cstep->para[1] >> 0x10);
    if ((u8)(cstep->para[1] >> 8) & 1) {
        fn_801621BC(&time);
    } else {
        sndConvertTicks(&time, svoice);
    }

    if (time) {
        svoice->cFlags |= 0x2000;
        svoice->vibPeriod = time;

        kr = (s8)(cstep->para[0] >> 8);
        cr = (s8)(cstep->para[0] >> 16);

        if (kr < 0) {
            if (cr < 0) {
                svoice->vibCentRange = -cr;
            } else {
                svoice->vibCentRange = cr;
            }

            svoice->vibKeyRange = -kr;
            svoice->vibCurTime = svoice->vibPeriod / 2;
        } else {
            if (cr < 0) {
                if (kr == 0) {
                    svoice->vibCentRange = -cr;
                    svoice->vibCurTime = svoice->vibPeriod / 2;
                } else {
                    --kr;
                    svoice->vibCentRange = 100 - cr;
                    svoice->vibCurTime = 0;
                }
            } else {
                svoice->vibCentRange = cr;
                svoice->vibCurTime = 0;
            }
            svoice->vibKeyRange = kr;
        }
    } else {
        svoice->cFlags &= ~0x2000ULL;
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSetADSR)
void mcmdSetADSR(SYNTH_VOICE* svoice, MSTEP* cstep) {
    ADSR_INFO adsr;
    ADSR_INFO* adsr_ptr;
    s32 ascale;
    s32 dscale;
    f32 sScale;

    if ((adsr_ptr = (ADSR_INFO*)dataGetCurve((u16)(cstep->para[0] >> 8))) != 0) {
        if (!(u8)(cstep->para[0] >> 24)) {
            adsr.data.linear.atime =
                adsr_ptr->data.linear.atime >> 8 | adsr_ptr->data.linear.atime << 8;
            adsr.data.linear.dtime =
                adsr_ptr->data.linear.dtime >> 8 | adsr_ptr->data.linear.dtime << 8;
            adsr.data.linear.slevel =
                adsr_ptr->data.linear.slevel >> 8 | adsr_ptr->data.linear.slevel << 8;
            adsr.data.linear.rtime =
                adsr_ptr->data.linear.rtime >> 8 | adsr_ptr->data.linear.rtime << 8;
            hwSetADSR(svoice->id & 0xFF, &adsr, 0);
        } else {
            sScale = dspDLSVolTab[(u16)(adsr_ptr->data.dls.slevel >> 8 |
                                         adsr_ptr->data.dls.slevel << 8) >>
                                   5];
            adsr.data.dls.atime =
                ((u8*)&adsr_ptr->data.dls.atime)[0] | ((u8*)&adsr_ptr->data.dls.atime)[1] << 8 |
                ((u8*)&adsr_ptr->data.dls.atime)[2] << 16 |
                ((u8*)&adsr_ptr->data.dls.atime)[3] << 24;
            adsr.data.dls.dtime =
                ((u8*)&adsr_ptr->data.dls.dtime)[0] | ((u8*)&adsr_ptr->data.dls.dtime)[1] << 8 |
                ((u8*)&adsr_ptr->data.dls.dtime)[2] << 16 |
                ((u8*)&adsr_ptr->data.dls.dtime)[3] << 24;
            adsr.data.dls.slevel = (u16)(s32)(4096.f * sScale);
            adsr.data.dls.rtime = adsr_ptr->data.dls.rtime >> 8 | adsr_ptr->data.dls.rtime << 8;
            ascale =
                ((u8*)&adsr_ptr->data.dls.ascale)[0] | ((u8*)&adsr_ptr->data.dls.ascale)[1] << 8 |
                ((u8*)&adsr_ptr->data.dls.ascale)[2] << 16 |
                ((u8*)&adsr_ptr->data.dls.ascale)[3] << 24;
            dscale =
                ((u8*)&adsr_ptr->data.dls.dscale)[0] | ((u8*)&adsr_ptr->data.dls.dscale)[1] << 8 |
                ((u8*)&adsr_ptr->data.dls.dscale)[2] << 16 |
                ((u8*)&adsr_ptr->data.dls.dscale)[3] << 24;

            if (ascale != (s32)0x80000000) {
                adsr.data.dls.atime += (s32)(FLT_EPSILON * svoice->orgVolume * ascale);
            }
            if (dscale != (s32)0x80000000) {
                adsr.data.dls.dtime += (s32)(0.0078125f * svoice->orgNote * dscale);
            }

            hwSetADSR(svoice->id & 0xFF, &adsr, 1);
        }

        svoice->cFlags |= 0x100;
    }
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSetADSRFromCtrl)
static s32 midi2TimeTab[129] = {
    0,      10,     20,     30,     40,     50,     60,     70,     80,     90,     100,    110,
    110,    120,    130,    140,    150,    160,    170,    190,    200,    220,    230,    250,
    270,    290,    310,    330,    350,    380,    410,    440,    470,    500,    540,    580,
    620,    660,    710,    760,    820,    880,    940,    1000,   1000,   1100,   1200,   1300,
    1400,   1500,   1600,   1700,   1800,   2000,   2100,   2300,   2400,   2600,   2800,   3000,
    3200,   3500,   3700,   4000,   4300,   4600,   4900,   5300,   5700,   6100,   6500,   7000,
    7500,   8100,   8600,   9300,   9900,   10000,  11000,  12000,  13000,  14000,  15000,  16000,
    17000,  18000,  19000,  21000,  22000,  24000,  26000,  28000,  30000,  32000,  34000,  37000,
    39000,  42000,  45000,  49000,  50000,  55000,  60000,  65000,  70000,  75000,  80000,  85000,
    90000,  95000,  100000, 105000, 110000, 115000, 120000, 125000, 130000, 135000, 140000, 145000,
    150000, 155000, 160000, 165000, 170000, 175000, 180000, 0,
};

void mcmdSetADSRFromCtrl(SYNTH_VOICE* svoice, MSTEP* cstep) {
    f32 sScale;
    ADSR_INFO adsr;

    sScale = dspDLSVolTab[inpGetMidiCtrl(cstep->para[0] >> 24, svoice->midi, svoice->midiSet) >> 7];
    adsr.data.dls.atime =
        midi2TimeTab[inpGetMidiCtrl(cstep->para[0] >> 8, svoice->midi, svoice->midiSet) >> 7];
    adsr.data.dls.dtime =
        midi2TimeTab[inpGetMidiCtrl(cstep->para[0] >> 16, svoice->midi, svoice->midiSet) >> 7];
    adsr.data.dls.slevel = 193 - dspScale2IndexTab[(u32)(1023.f * sScale)];
    adsr.data.dls.rtime =
        midi2TimeTab[inpGetMidiCtrl((u8)cstep->para[1], svoice->midi, svoice->midiSet) >> 7];
    adsr.data.dls.ascale = (s32)0x80000000;
    adsr.data.dls.dscale = (s32)0x80000000;
    hwSetADSR((u8)svoice->id, &adsr, 2);
    svoice->cFlags |= 0x100;
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSetPitchADSR)
void mcmdSetPitchADSR(SYNTH_VOICE* svoice, MSTEP* cstep) {
    ADSR_INFO adsr;
    ADSR_INFO* adsr_ptr;
    u32 sl;
    s32 ascale;
    s32 dscale;

    if ((adsr_ptr = (ADSR_INFO*)dataGetCurve((u16)(cstep->para[0] >> 8))) == 0) {
        return;
    }

    svoice->pitchADSRRange = ((s8)cstep->para[1] << 8);

    if (svoice->pitchADSRRange >= 0) {
        svoice->pitchADSRRange += ((s16)(s8)(cstep->para[1] >> 8) << 8) / 100;
    } else {
        svoice->pitchADSRRange -= ((s16)(s8)(cstep->para[1] >> 8) << 8) / 100;
    }

    adsr.data.dls.atime =
        ((u8*)&adsr_ptr->data.dls.atime)[0] | ((u8*)&adsr_ptr->data.dls.atime)[1] << 8 |
        ((u8*)&adsr_ptr->data.dls.atime)[2] << 16 | ((u8*)&adsr_ptr->data.dls.atime)[3] << 24;
    adsr.data.dls.dtime =
        ((u8*)&adsr_ptr->data.dls.dtime)[0] | ((u8*)&adsr_ptr->data.dls.dtime)[1] << 8 |
        ((u8*)&adsr_ptr->data.dls.dtime)[2] << 16 | ((u8*)&adsr_ptr->data.dls.dtime)[3] << 24;

    adsr.data.dls.slevel = (adsr_ptr->data.dls.slevel >> 8) | (adsr_ptr->data.dls.slevel << 8);
    adsr.data.dls.rtime = (adsr_ptr->data.dls.rtime >> 8) | (adsr_ptr->data.dls.rtime << 8);
    ascale =
        ((u8*)&adsr_ptr->data.dls.ascale)[0] | ((u8*)&adsr_ptr->data.dls.ascale)[1] << 8 |
        ((u8*)&adsr_ptr->data.dls.ascale)[2] << 16 | ((u8*)&adsr_ptr->data.dls.ascale)[3] << 24;
    dscale =
        ((u8*)&adsr_ptr->data.dls.dscale)[0] | ((u8*)&adsr_ptr->data.dls.dscale)[1] << 8 |
        ((u8*)&adsr_ptr->data.dls.dscale)[2] << 16 | ((u8*)&adsr_ptr->data.dls.dscale)[3] << 24;

    if (ascale != (s32)0x80000000) {
        adsr.data.dls.atime += (s32)((FLT_EPSILON * svoice->orgVolume) * (f32)ascale);
    }
    if (dscale != (s32)0x80000000) {
        adsr.data.dls.dtime += (s32)((0.0078125f * svoice->orgNote) * (f32)dscale);
    }

    svoice->pitchADSR.mode = 1;
    svoice->pitchADSR.data.dls.aMode = 0;
    svoice->pitchADSR.data.dls.aTime = adsrConvertTimeCents(adsr.data.dls.atime);
    svoice->pitchADSR.data.dls.dTime = adsrConvertTimeCents(adsr.data.dls.dtime);
    sl = (adsr.data.dls.slevel >> 2);
    if (sl > 0x3ff) {
        sl = 0x3ff;
    }

    svoice->pitchADSR.data.dls.sLevel = (u16)(193 - dspScale2IndexTab[sl]);
    svoice->pitchADSR.data.dls.rTime = adsr.data.dls.rtime;
    adsrSetup(&svoice->pitchADSR);
    svoice->cFlags |= 0x20000000000ULL;
}
#endif

static inline u32 mcmdSetKey(SYNTH_VOICE* svoice, MSTEP* cstep) {
    svoice->curNote = (u8)(cstep->para[0] >> 8) & 0x7f;
    svoice->curDetune = (s8)(cstep->para[0] >> 0x10);
    if (voiceIsLastStarted(svoice) != 0) {
        inpSetMidiLastNote(svoice->midi, svoice->midiSet, (u8)svoice->curNote);
    }
    cstep->para[0] = 4;
    return mcmdWait(svoice, cstep);
}

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdRandomKey)
void mcmdRandomKey(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u8 k1;
    u8 k2;
    u8 t;
    s32 i1;
    s32 i2;
    u8 detune;

    if (!(u8)(cstep->para[1] >> 8)) {
        k1 = (u8)(cstep->para[0] >> 8);
        k2 = (u8)(cstep->para[0] >> 24);
        if (k1 > k2) {
            t = k1;
            k1 = k2;
            k2 = t;
        }
    } else {
        i1 = svoice->curNote - (u8)(cstep->para[0] >> 8);
        i2 = svoice->curNote + (u8)(cstep->para[0] >> 24);

        k1 = i1 < 0 ? 0 : i1 > 127 ? 127 : i1;
        k2 = i2 < 0 ? 0 : i2 > 127 ? 127 : i2;
    }

    if ((u8)cstep->para[1]) {
        detune = (fn_80162070() % 201) - 100;
    } else {
        detune = (u8)(cstep->para[0] >> 16);
    }

    cstep->para[0] = ((u8)detune << 16) | 0x19 | ((k1 + fn_80162070() % ((k2 - k1) + 1)) * 0x100);
    cstep->para[1] = 0;
    mcmdSetKey(svoice, cstep);
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPortamento)
void mcmdPortamento(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 time;
    svoice->portType = (u8)(cstep->para[0] >> 16);
    time = (u16)(cstep->para[1] >> 16);
    if (((u8)(cstep->para[1] >> 8) & 1)) {
        fn_801621BC(&time);
    } else {
        sndConvertTicks(&time, svoice);
    }

    svoice->portDuration = time;

    switch ((u8)(cstep->para[0] >> 8)) {
    case 0:
        if (svoice->midi != 0xFF) {
            fn_801603C0(0x41, svoice->midi, svoice->midiSet, 0);
        }
        svoice->cFlags &= ~0x400ULL;
        return;
    case 1:
        if (svoice->midi != 0xFF) {
            fn_801603C0(0x41, svoice->midi, svoice->midiSet, 0x7f);
        }
    init_port:
        if (!(svoice->cFlags & 0x400)) {
            synthInitPortamento(svoice);
        }
        svoice->cFlags |= 0x400;
        break;
    case 2:
        if (svoice->midi != 0xFF && inpGetMidiCtrl(0x41, svoice->midi, svoice->midiSet) > 8064) {
            goto init_port;
        }
        break;
    }
}
#endif

static inline u32 TranslateVolume(u32 volume, u16 curve) {
    u8* ptr;
    u32 vlow;
    u32 vhigh;
    s32 d;

    if (curve != 0xFFFF) {
        if ((ptr = (u8*)dataGetCurve(curve))) {
            vhigh = (volume >> 16) & 0xFFFF;
            vlow = volume & 0xFFFF;

            if (vhigh < 0x7f) {
                d = vlow * (ptr[vhigh + 1] - ptr[vhigh]);
                volume = d + ((u16)ptr[vhigh] << 16);
            } else {
                volume = ptr[vhigh] << 16;
            }
        }
    }

    return volume;
}

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdScaleVolume)
void mcmdScaleVolume(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u16 curve;
    u16 scale;
    scale = (u16)(u8)(cstep->para[0] >> 8);

    if ((u8)(cstep->para[1] >> 8) == 0) {
        svoice->volume = (svoice->volume * scale) / 0x7f;
    } else {
        svoice->volume = (svoice->orgVolume * scale) / 0x7f;
    }
    svoice->volume += (u8)(cstep->para[0] >> 16) << 16;
    if (svoice->volume > 0x7f0000) {
        svoice->volume = 0x7f0000;
    }

    curve = (u8)(cstep->para[0] >> 0x18);
    curve |= ((u16)((u8)cstep->para[1]) << 8);

    svoice->volume = TranslateVolume(svoice->volume, curve);
    svoice->cFlags |= 0x100000000000ULL;
}
#endif

static inline void DoEnvelopeCalculation(SYNTH_VOICE* svoice, MSTEP* cstep, s32 start_vol) {
    u32 tvol;
    u32 time;
    s32 mstime;
    u16 curve;

    time = (u16)(cstep->para[1] >> 16);

    if ((u8)(cstep->para[1] >> 8) & 1) {
        fn_801621BC(&time);
    } else {
        sndConvertTicks(&time, svoice);
    }

    mstime = fn_80162214(time);
    if (mstime == 0) {
        mstime = 1;
    }

    tvol = (svoice->volume * (u8)(cstep->para[0] >> 8) >> 7);
    tvol += (u8)(cstep->para[0] >> 16) << 16;

    if (tvol > 0x7f0000) {
        tvol = 0x7f0000;
    }

    curve = (u16)(u8)(cstep->para[0] >> 0x18);
    curve |= (((u16)(u8)cstep->para[1]) << 8);
    tvol = TranslateVolume(tvol, curve);
    svoice->envTarget = tvol;
    svoice->envCurrent = start_vol;
    svoice->envDelta = (s32)(tvol - start_vol) / mstime;
    svoice->volume = start_vol;
    svoice->cFlags |= 0x8000;
}

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdEnvelope)
void mcmdEnvelope(SYNTH_VOICE* svoice, MSTEP* cstep) { DoEnvelopeCalculation(svoice, cstep, svoice->volume); }
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdFadeIn)
void mcmdFadeIn(SYNTH_VOICE* svoice, MSTEP* cstep) { DoEnvelopeCalculation(svoice, cstep, 0); }
#endif

static inline void DoPanningSetup(SYNTH_VOICE* svoice, MSTEP* cstep, u8 pi) {
    s32 width;
    u32 mstime;
    svoice->panTime[pi] = width = (u16)(cstep->para[0] >> 16);
    fn_801621BC(&svoice->panTime[pi]);
    mstime = (s8)(cstep->para[1]);
    svoice->panning[pi] = ((u8)(cstep->para[0] >> 8)) << 16;
    svoice->panTarget[pi] = svoice->panning[pi] + mstime * 0x10000;
    if (svoice->panTime[pi] != 0) {
        svoice->panDelta[pi] = (s32)(mstime << 16) / width;
    } else {
        svoice->panDelta[pi] = (s32)(mstime << 16);
    }

    svoice->cFlags |= 0x200000000000ULL;
}

/* mcmdSetPanning */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSetPanning)
void mcmdSetPanning(SYNTH_VOICE* svoice, MSTEP* cstep) { DoPanningSetup(svoice, cstep, 0); }
#endif

/* mcmdSetSurroundPanning */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdSetSurroundPanning)
void mcmdSetSurroundPanning(SYNTH_VOICE* svoice, MSTEP* cstep) { DoPanningSetup(svoice, cstep, 1); }
#endif

/* mcmdPlayMacro */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_mcmdPlayMacro)
void mcmdPlayMacro(SYNTH_VOICE* svoice, MSTEP* cstep) {
    s32 key;
    u32 new_child;

    key = ((u32)svoice->orgNote + (s8)(u8)(cstep->para[0] >> 8));
    key = (key < 0) ? 0 : key > 0x7f ? 0x7f : key;

    if (svoice->fxFlag != 0) {
        key |= 0x80;
    }

    svoice->block = 1;
    new_child = macStart((u16)(cstep->para[0] >> 0x10), (u8)(cstep->para[1] >> 0x10),
                             (u8)(cstep->para[1] >> 0x18), svoice->allocId, (u8)key,
                             (u8)(svoice->volume >> 0x10), (u8)(svoice->panning[0] >> 0x10),
                             svoice->midi, svoice->midiSet, svoice->section, (u16)cstep->para[1],
                             (u16)svoice->track, 0, svoice->vGroup, svoice->studio,
                             svoice->itdMode == 0);
    svoice->block = 0;
    if (new_child != 0xFFFFFFFF) {
        svoice->lastVID = *(u32*)((u8*)synthVoice[(u8)new_child].vidList + 8);
        synthVoice[(u8)new_child].parent = svoice->id;
        if (svoice->child != (u32)-1) {
            synthVoice[(u8)new_child].child = svoice->child;
            synthVoice[(u8)svoice->child].parent = new_child;
        }
        svoice->child = new_child;
        if (svoice->fxFlag != 0) {
            synthFXCloneMidiSetup(&synthVoice[(u8)new_child], svoice);
        }
    } else {
        svoice->lastVID = (u32)-1;
    }
}
#endif

extern void* memset(void* dst, int val, u32 size);
extern void fn_80161A9C(void* svoice); /* inpInit */
extern u8 inpGetMidiLastNote(u8 midi, u8 midiSet);
extern void voiceSetLastStarted(SYNTH_VOICE* svoice);
extern u8* fn_80160EA0(u8 midi, u8 midiSet); /* inpGetChannelDefaults; only pbRange (offset 0) used here */
extern u32 inpGetModulation(SYNTH_VOICE* sv);
extern void fn_801629A4(u32 index, u8 value);
extern void fn_801629D0(u32 index, u8 value);
extern u32 hwFrq2Pitch(u32 value);
#define DebugMacroSteps lbl_8047AFC8


/* Reference-shaped static helpers for macHandleActive's inlined cases.
 * Source position (before macHandleActive) makes MWCC auto-inline them. */
static inline u32 mcmdGoto(SYNTH_VOICE* svoice, MSTEP* cstep) {
    MSTEP* macAddr;

    if ((macAddr = (MSTEP*)dataGetMacro((u16)(cstep->para[0] >> 16))) != NULL) {
        svoice->addr = macAddr;
        svoice->curAddr = macAddr + (u16)cstep->para[1];
        return 0;
    }
    return mcmdEndOfMacro(svoice);
}

static inline void mcmdTrapEvent(SYNTH_VOICE* svoice, MSTEP* cstep) {
    MSTEP* addr;
    u8 t;

    if ((addr = (MSTEP*)dataGetMacro(cstep->para[0] >> 16)) != NULL) {
        t = (u8)(cstep->para[0] >> 8);
        svoice->trapEventAddr[t] = addr;
        svoice->trapEventCurAddr[t] = addr + (u16)cstep->para[1];
        svoice->trapEventAny = 1;
        if (t == 0 && (svoice->cFlags & 0x10000000008ULL) == 0x10000000008ULL) {
            svoice->cFlags |= 0x40000000000ULL;
        }
    }
}

static inline void mcmdUntrapEvent(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u8 i;

    svoice->trapEventAddr[(u8)(cstep->para[0] >> 8)] = NULL;
    for (i = 0; i < 3; ++i) {
        if (svoice->trapEventAddr[i] != NULL) {
            return;
        }
    }
    svoice->trapEventAny = 0;
}

static inline void mcmdSetPianoPanning(SYNTH_VOICE* svoice, MSTEP* cstep) {
    s32 delta;
    s32 scale;

    delta = (svoice->curNote << 16) - ((u8)(cstep->para[0] >> 16) << 16);
    scale = (s8)(u8)(cstep->para[0] >> 8);
    delta = (delta * scale) >> 7;
    delta += (u8)(cstep->para[0] >> 24) << 16;
    delta = delta < 0 ? 0 : delta > 0x7f0000 ? 0x7f0000 : delta;
    svoice->panTarget[0] = delta;
    svoice->panning[0] = delta;
}

static inline u32 mcmdLastKey(SYNTH_VOICE* svoice, MSTEP* cstep) {
    svoice->curNote = svoice->lastNote + (s8)(u8)(cstep->para[0] >> 8);
    svoice->curNote = (s16)svoice->curNote < 0 ? 0 : svoice->curNote > 0x7f ? 0x7f : svoice->curNote;
    svoice->curDetune = (s8)(cstep->para[0] >> 16);

    if (svoice->midi != 0xFF) {
        inpSetMidiLastNote(svoice->midi, svoice->midiSet, (u8)svoice->curNote);
    }

    cstep->para[0] = 4;
    return mcmdWait(svoice, cstep);
}

static inline u32 mcmdPitchSweep(SYNTH_VOICE* svoice, MSTEP* cstep, u8 i) {
    s32 pitch;
    s16 freq;

    svoice->sweepOff[i] = 0;
    svoice->sweepNum[i] = (u8)(cstep->para[0] >> 8);
    svoice->sweepCnt[i] = svoice->sweepNum[i] << 16;
    freq = (s16)(cstep->para[0] >> 16);
    if (freq >= 0) {
        pitch = hwFrq2Pitch(freq);
    } else {
        pitch = -(s32)hwFrq2Pitch(-freq);
    }
    svoice->sweepAdd[i] = pitch << 16;
    cstep->para[0] = 0;
    return mcmdWait(svoice, cstep);
}

static inline void mcmdSetPitch(SYNTH_VOICE* svoice, MSTEP* cstep) {
    svoice->playFrq = cstep->para[0] >> 8;
    svoice->playFrq |= (u8)cstep->para[1];

    if (svoice->sInfo != 0xFFFFFFFF) {
        DoSetPitch(svoice);
    }
}

static inline void mcmdScaleVolumeDLS(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u16 scale;

    scale = (u16)(cstep->para[0] >> 8);
    if ((cstep->para[0] >> 24) == 0) {
        svoice->volume = ((svoice->volume >> 5) * scale) >> 7;
    } else {
        svoice->volume = ((svoice->orgVolume >> 5) * scale) >> 7;
    }

    if (svoice->volume > 0x7f0000) {
        svoice->volume = 0x7f0000;
    }

    svoice->cFlags |= 0x100000000000ULL;
}

static inline void mcmdReturn(SYNTH_VOICE* svoice) {
    if (svoice->callStackEntryNum != 0) {
        svoice->addr = svoice->callStack[svoice->callStackIndex].addr;
        svoice->curAddr = svoice->callStack[svoice->callStackIndex].curAddr;
        svoice->callStackIndex = (svoice->callStackIndex - 1) & 3;
        --svoice->callStackEntryNum;
    }
}

static inline void mcmdAddAgeCounter(SYNTH_VOICE* svoice, MSTEP* cstep) {
    s32 age;

    age = (svoice->age >> 15) + (s16)(cstep->para[0] >> 16);
    if (age < 0) {
        svoice->age = 0;
    } else if (age > 0xFFFF) {
        svoice->age = 0x7FFF8000;
    } else {
        svoice->age = age << 15;
    }

    fn_80162494(svoice->id & 0xFF, ((u32)svoice->prio << 24) | (svoice->age >> 15));
}

static inline void mcmdSetAgeCounter(SYNTH_VOICE* svoice, MSTEP* cstep) {
    svoice->age = (u32)((u16)(cstep->para[0] >> 16)) << 15;
    fn_80162494(svoice->id & 0xFF, ((u32)svoice->prio << 24) | (svoice->age >> 15));
}

static inline void mcmdAddPriority(SYNTH_VOICE* svoice, MSTEP* cstep) {
    s16 prio;

    prio = svoice->prio + (s16)(cstep->para[0] >> 16);
    voiceSetPriority(svoice, prio < 0 ? 0 : prio > 0xFF ? 0xFF : prio);
}

static inline void mcmdSetAgeCounterByVolume(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 age;

    age = (((u8)(svoice->volume >> 16) * (u16)cstep->para[1]) >> 7) + (u16)(cstep->para[0] >> 16);
    svoice->age = age > 0xEA60 ? 0x75300000 : age << 15;
    fn_80162494(svoice->id & 0xFF, ((u32)svoice->prio << 24) | (svoice->age >> 15));
}

static inline void mcmdSetupLFO(SYNTH_VOICE* svoice, MSTEP* cstep) {
    u32 time;
    u32 ms;
    u8 index;

    time = cstep->para[0] >> 16;
    index = (u8)(cstep->para[0] >> 8);
    fn_801621BC(&time);
    if (svoice->lfo[index].period != 0) {
        ms = (u16)cstep->para[1];
        fn_801621BC(&ms);
        svoice->lfo[index].time = ms;
    }
    svoice->lfo[index].period = time;
}

#define cstep lbl_8047AFB0
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macHandleActive)
void macHandleActive(SYNTH_VOICE* svoice) { /* macHandleActive */
    u8 i;
    u32 vi;
    u32 voiceid;
    u8 lastNote;
    u32 ex;
    u8* channelDefaults;

    if (svoice->cFlags & 3) {
        if (svoice->cFlags & 1) {
            svoice->cFlags &= ~1ULL;
            hwBreak(svoice->id & 0xFF);
        }

        svoice->panning[0] = svoice->panTarget[0] = (u32)(svoice->setup_pan) << 16;
        svoice->panning[1] = svoice->panTarget[1] = 0;
        svoice->volume = (u32)(svoice->setup_vol) << 16;
        svoice->volTable = 0;
        svoice->orgVolume = svoice->volume;
        svoice->midi = svoice->setup_midi;
        svoice->midiSet = svoice->setup_midiSet;
        svoice->section = svoice->setup_section;
        svoice->track = svoice->setup_track;
        svoice->itdMode = svoice->setup_itdMode;
        svoice->keyGroup = 0;
        svoice->vibModAddScale = 0;
        svoice->treScale = 0;
        fn_80161A9C(svoice);
        if ((lastNote = inpGetMidiLastNote(svoice->midi, svoice->midiSet)) != 0xFF) {
            svoice->lastNote = lastNote;
        } else {
            svoice->lastNote = svoice->orgNote;
        }

        inpSetMidiLastNote(svoice->midi, svoice->midiSet, svoice->orgNote);
        voiceSetLastStarted(svoice);
        svoice->vGroup = svoice->setup_vGroup;
        svoice->studio = svoice->setup_studio;
        svoice->portTime = 0;
        svoice->portDuration = 25600;
        svoice->portType = 0;
        if (svoice->midi != 0xFF) {
            svoice->portLastCtrlState = inpGetMidiCtrl(0x41, svoice->midi, svoice->midiSet);
        } else {
            svoice->portLastCtrlState = 0;
        }
        channelDefaults = fn_80160EA0(svoice->midi, svoice->midiSet);
        svoice->pbLowerKeyRange = *channelDefaults;
        svoice->pbUpperKeyRange = *channelDefaults;
        svoice->revVolScale = 128;
        svoice->revVolOffset = 0;
        svoice->loop = 0;
        svoice->sweepNum[0] = 0;
        svoice->sweepNum[1] = 0;
        svoice->sweepOff[0] = 0;
        svoice->sweepOff[1] = 0;
        svoice->lfo[0].period = 0;
        svoice->lfo[0].value = 0;
        svoice->lfo[0].lastValue = 0x7fff;
        svoice->lfo[1].period = 0;
        svoice->lfo[1].value = 0;
        svoice->lfo[1].lastValue = 0x7fff;

        for (i = 0; i < 3; ++i) {
            svoice->trapEventAddr[i] = NULL;
        }

        svoice->trapEventAny = 0;
        svoice->sInfo = (u32)-1;
        svoice->playFrq = (u32)-1;
        svoice->pbLast = 0x2000;
        svoice->curOutputVolume = 0;
        svoice->cFlags &= 8;
        svoice->cFlags |= 0x300000000000ULL;
        memset(svoice->local_vars, 0, sizeof(svoice->local_vars));
        svoice->waitTime = macRealTime;
        svoice->macStartTime = macRealTime;
        synthStartSynthJobHandling(svoice);
    }

    DebugMacroSteps = 0;

    do {
        if (++DebugMacroSteps > 32) {
            break;
        }

        cstep.para[0] = svoice->curAddr->para[0];
        cstep.para[1] = svoice->curAddr->para[1];
        ++svoice->curAddr;
        ex = 0;

        switch (cstep.para[0] & 0x7F) {
        case 0x00:
            ex = mcmdEndOfMacro(svoice);
            break;
        case 0x01:
            ex = mcmdEndOfMacro(svoice);
            break;
        case 0x02: {
            MSTEP* m;
            if (svoice->curNote < (u8)(cstep.para[0] >> 8)) {
                break;
            }
            m = (MSTEP*)dataGetMacro((u16)(cstep.para[0] >> 16));
            if (m != NULL) {
                svoice->addr = m;
                svoice->curAddr = m + (u16)cstep.para[1];
            }
            break;
        }
        case 0x03: {
            MSTEP* m;
            if (((svoice->volume >> 16) & 0xFF) < (u8)(cstep.para[0] >> 8)) {
                break;
            }
            m = (MSTEP*)dataGetMacro((u16)(cstep.para[0] >> 16));
            if (m != NULL) {
                svoice->addr = m;
                svoice->curAddr = m + (u16)cstep.para[1];
            }
            break;
        }
        case 0x04:
            ex = mcmdWait(svoice, &cstep);
            break;
        case 0x05:
            mcmdLoop(svoice, &cstep);
            break;
        case 0x06:
            ex = mcmdGoto(svoice, &cstep);
            break;
        case 0x07:
            ((u8*)&cstep)[6] = 1;
            ex = mcmdWait(svoice, &cstep);
            break;
        case 0x08:
            mcmdPlayMacro(svoice, &cstep);
            break;
        case 0x09: {
            /* mcmdSendKeyOff, hand-inlined (contains a loop; -inline auto refuses) */
            voiceid = (svoice->orgNote + (u8)(cstep.para[0] >> 8)) << 8;
            voiceid |= ((u16)(cstep.para[0] >> 16)) << 16;
            for (vi = 0; vi < synthInfo.voiceNum; ++vi) {
                if (synthVoice[vi].id == (voiceid | vi)) {
                    /* SendSingleKeyOff(voiceid | vi), hand-inlined */
                    u32 vid = voiceid | vi;
                    if (vid != 0xFFFFFFFF) {
                        u32 v = vid & 0xFF;
                        if (vid == synthVoice[v].id) {
                            macSetExternalKeyoff(&synthVoice[v]);
                        }
                    }
                }
            }
            break;
        }
        case 0x0a: {
            MSTEP* m;
            if (svoice->midi == 0xFF) {
                break;
            }
            if ((u8)(inpGetModulation(svoice) >> 7) < ((cstep.para[0] >> 8) & 0xFF)) {
                break;
            }
            m = (MSTEP*)dataGetMacro((u16)(cstep.para[0] >> 16));
            if (m != NULL) {
                svoice->addr = m;
                svoice->curAddr = m + (u16)cstep.para[1];
            }
            break;
        }
        case 0x0b:
            mcmdSetPianoPanning(svoice, &cstep);
            break;
        case 0x0c:
            mcmdSetADSR(svoice, &cstep);
            break;
        case 0x0d:
            mcmdScaleVolume(svoice, &cstep);
            break;
        case 0x0e:
            mcmdSetPanning(svoice, &cstep);
            break;
        case 0x0f:
            mcmdEnvelope(svoice, &cstep);
            break;
        case 0x10:
            mcmdStartSample(svoice, &cstep);
            break;
        case 0x11:
            hwBreak(svoice->id & 0xFF);
            break;
        case 0x12:
            svoice->cFlags |= 0x80ULL;
            synthKeyStateUpdate(svoice);
            break;
        case 0x13: {
            MSTEP* m;
            if ((u8)fn_80162070() < ((cstep.para[0] >> 8) & 0xFF)) {
                break;
            }
            m = (MSTEP*)dataGetMacro((u16)(cstep.para[0] >> 16));
            if (m != NULL) {
                svoice->addr = m;
                svoice->curAddr = m + (u16)cstep.para[1];
            }
            break;
        }
        case 0x14:
            mcmdFadeIn(svoice, &cstep);
            break;
        case 0x15:
            mcmdSetSurroundPanning(svoice, &cstep);
            break;
        case 0x16:
            mcmdSetADSRFromCtrl(svoice, &cstep);
            break;
        case 0x17:
            mcmdRandomKey(svoice, &cstep);
            break;
        case 0x18:
            ex = mcmdAddKey(svoice, &cstep);
            break;
        case 0x19:
            ex = mcmdSetKey(svoice, &cstep);
            break;
        case 0x1a:
            ex = mcmdLastKey(svoice, &cstep);
            break;
        case 0x1b:
            mcmdPortamento(svoice, &cstep);
            break;
        case 0x1c:
            mcmdVibrato(svoice, &cstep);
            break;
        case 0x1d:
            ex = mcmdPitchSweep(svoice, &cstep, 0);
            break;
        case 0x1e:
            ex = mcmdPitchSweep(svoice, &cstep, 1);
            break;
        case 0x1f:
            mcmdSetPitch(svoice, &cstep);
            break;
        case 0x20:
            mcmdSetPitchADSR(svoice, &cstep);
            break;
        case 0x21:
            mcmdScaleVolumeDLS(svoice, &cstep);
            break;
        case 0x22:
            svoice->vibModAddScale = ((s8)(cstep.para[0] >> 8) << 8);
            if (svoice->vibModAddScale >= 0) {
                svoice->vibModAddScale += ((s16)(s8)(cstep.para[0] >> 16) << 8) / 100;
            } else {
                svoice->vibModAddScale -= ((s16)(s8)(cstep.para[0] >> 16) << 8) / 100;
            }
            break;
        case 0x23:
            svoice->treScale = (u16)(cstep.para[0] >> 8);
            svoice->treModAddScale = (u16)cstep.para[1];
#if defined(SYNTHMACROS_SELECT_macHandleActive) && !defined(SYNTHMACROS_ALL)
            svoice->treCurScale = 1.0f;
#else
            svoice->treCurScale = lbl_8047D3EC;
#endif
            break;
        case 0x24:
            mcmdReturn(svoice);
            break;
        case 0x25:
            ex = mcmdGosub(svoice, &cstep);
            break;
        case 0x28:
            mcmdTrapEvent(svoice, &cstep);
            break;
        case 0x29:
            mcmdUntrapEvent(svoice, &cstep);
            break;
        case 0x2a:
            mcmdSendMessage(svoice, &cstep);
            break;
        case 0x2b: {
            s32 mesg = 0;
            u8 idx;
            if (svoice->mesgNum != 0) {
                mesg = svoice->mesgQueue[svoice->mesgRead];
                svoice->mesgRead = (svoice->mesgRead + 1) & 3;
                svoice->mesgNum -= 1;
            }
            idx = (u8)(cstep.para[0] >> 8);
            varSet32(svoice, 0, idx, mesg);
            break;
        }
        case 0x2c:
            mcmdGetVID(svoice, &cstep);
            break;
        case 0x30:
            mcmdAddAgeCounter(svoice, &cstep);
            break;
        case 0x31:
            mcmdSetAgeCounter(svoice, &cstep);
            break;
        case 0x32:
            synthGlobalVariable[(u8)(cstep.para[0] >> 8)] = (u8)(cstep.para[0] >> 16);
            break;
        case 0x33:
            svoice->pbLowerKeyRange = (u8)(cstep.para[0] >> 16);
            svoice->pbUpperKeyRange = (u8)(cstep.para[0] >> 8);
            break;
        case 0x34:
            svoice->revVolScale = (u8)(cstep.para[0] >> 8);
            svoice->revVolOffset = (u8)(cstep.para[0] >> 16);
            break;
        case 0x35:
            svoice->cFlags |= 0x10000ULL;
            break;
        case 0x36:
            voiceSetPriority(svoice, (u8)(cstep.para[0] >> 8));
            break;
        case 0x37:
            mcmdAddPriority(svoice, &cstep);
            break;
        case 0x38:
            if (cstep.para[1] != 0) {
                svoice->ageSpeed = (svoice->age >> 8) / cstep.para[1];
            } else {
                svoice->ageSpeed = 0;
            }
            break;
        case 0x39:
            mcmdSetAgeCounterByVolume(svoice, &cstep);
            break;
        case 0x40:
            mcmdVolumeSelect(svoice, &cstep);
            break;
        case 0x41:
            mcmdPanningSelect(svoice, &cstep);
            break;
        case 0x42:
            mcmdPitchWheelSelect(svoice, &cstep);
            break;
        case 0x43:
            mcmdModWheelSelect(svoice, &cstep);
            break;
        case 0x44:
            mcmdPedalSelect(svoice, &cstep);
            break;
        case 0x45:
            mcmdPortamentoSelect(svoice, &cstep);
            break;
        case 0x46:
            mcmdReverbSelect(svoice, &cstep);
            break;
        case 0x47:
            mcmdSurroundPanningSelect(svoice, &cstep);
            break;
        case 0x48:
            mcmdDopplerSelect(svoice, &cstep);
            break;
        case 0x49:
            mcmdTremoloSelect(svoice, &cstep);
            break;
        case 0x4a:
            mcmdPreAuxASelect(svoice, &cstep);
            break;
        case 0x4b:
            mcmdPreAuxBSelect(svoice, &cstep);
            break;
        case 0x4c:
            mcmdPostAuxBSelect(svoice, &cstep);
            break;
        case 0x4d:
            mcmdAuxAFXSelect(svoice, &cstep);
            break;
        case 0x4e:
            mcmdAuxBFXSelect(svoice, &cstep);
            break;
        case 0x50:
            mcmdSetupLFO(svoice, &cstep);
            break;
        case 0x58: {
            u8 mode = (u8)(cstep.para[0] >> 8);
            svoice->volTable = (mode != 0);
            svoice->itdMode = ((u8)(cstep.para[0] >> 16) == 0);
            break;
        }
        case 0x59:
            mcmdSetKeyGroup(svoice, &cstep);
            break;
        case 0x5a:
            fn_801629A4(svoice->id & 0xFF, (u8)(cstep.para[0] >> 8));
            fn_801629D0(svoice->id & 0xFF, (u8)(cstep.para[0] >> 16));
            svoice->cFlags |= 0x80000000000ULL;
            break;
        case 0x60:
            mcmdVarCalculation(svoice, &cstep, 0);
            break;
        case 0x61:
            mcmdVarCalculation(svoice, &cstep, 1);
            break;
        case 0x62:
            mcmdVarCalculation(svoice, &cstep, 2);
            break;
        case 0x63:
            mcmdVarCalculation(svoice, &cstep, 3);
            break;
        case 0x64:
            mcmdVarCalculation(svoice, &cstep, 4);
            break;
        case 0x65:
            varSet32(svoice, (u8)(cstep.para[0] >> 8), (u8)(cstep.para[0] >> 16), (s16)cstep.para[1]);
            break;
        case 0x70:
            mcmdIfVarCompare(svoice, &cstep, 0);
            break;
        case 0x71:
            mcmdIfVarCompare(svoice, &cstep, 1);
            break;
        }
    } while (!ex);
}
#endif

#undef cstep
#undef DebugMacroSteps


/* macHandle */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macHandle)
void macHandle(u32 deltaTime) {
    SYNTH_VOICE* sv;
    SYNTH_VOICE* nextSv;
    u64 w;

    for (sv = macTimeQueueRoot; sv != 0 && sv->wait <= macRealTime;) {
        nextSv = sv->nextTimeQueueMacro;
        w = sv->wait;
        macMakeActive(sv);
        sv->waitTime = w;
        sv = nextSv;
    }

    for (sv = macActiveMacroRoot; sv != 0; sv = sv->nextMacActive) {
        if (HasHWEventTrap(sv) != 0) {
            CheckHWEventTrap(sv);
        }
        macHandleActive(sv);
    }
    macRealTime += deltaTime;
}
#endif

#pragma dont_inline on
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_TimeQueueAdd)
void TimeQueueAdd(SYNTH_VOICE* svoice) {
    SYNTH_VOICE* sv;
    SYNTH_VOICE* lastSv;

    lastSv = 0;
    for (sv = macTimeQueueRoot; sv != 0 && sv->wait < svoice->wait;) {
        lastSv = sv;
        sv = sv->nextTimeQueueMacro;
    }

    if (sv == 0) {
        if (lastSv == 0) {
            macTimeQueueRoot = svoice;
            svoice->nextTimeQueueMacro = 0;
            svoice->prevTimeQueueMacro = 0;
        } else {
            lastSv->nextTimeQueueMacro = svoice;
            svoice->prevTimeQueueMacro = lastSv;
            svoice->nextTimeQueueMacro = 0;
        }
    } else {
        svoice->nextTimeQueueMacro = sv;
        if ((svoice->prevTimeQueueMacro = sv->prevTimeQueueMacro)) {
            sv->prevTimeQueueMacro->nextTimeQueueMacro = svoice;
        } else {
            macTimeQueueRoot = svoice;
        }
        sv->prevTimeQueueMacro = svoice;
    }
}
#endif
#pragma dont_inline reset

static inline void UnYieldMacro(SYNTH_VOICE* svoice, u32 disableUpdate) {
    if (svoice->wait != 0) {
        if (svoice->wait != (u64)-1) {
            if (svoice->prevTimeQueueMacro == 0) {
                macTimeQueueRoot = svoice->nextTimeQueueMacro;
            } else {
                svoice->prevTimeQueueMacro->nextTimeQueueMacro = svoice->nextTimeQueueMacro;
            }

            if (svoice->nextTimeQueueMacro) {
                svoice->nextTimeQueueMacro->prevTimeQueueMacro = svoice->prevTimeQueueMacro;
            }
        }

        if (!disableUpdate) {
            synthForceLowPrecisionUpdate(svoice);
        }

        svoice->wait = 0;
        svoice->waitTime = macRealTime;
        svoice->cFlags &= ~0x40004ULL;
    }
}

/* MAC_STATE enum, confirmed from disassembly (not the reference's symbolic
 * names): RUNNABLE=0, YIELDED=1, STOPPED=2. */
#if defined(SYNTHMACROS_SELECT_macStart) && !defined(SYNTHMACROS_ALL)
#define SYNTHMACROS_MACSTART_INLINE static inline
#else
#define SYNTHMACROS_MACSTART_INLINE
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macMakeActive) || \
    defined(SYNTHMACROS_SELECT_macStart)
SYNTHMACROS_MACSTART_INLINE
void macMakeActive(SYNTH_VOICE* sv) {
    if (sv->macState == 0) {
        return;
    }
    UnYieldMacro(sv, 0);
    if ((sv->nextMacActive = macActiveMacroRoot)) {
        macActiveMacroRoot->prevMacActive = sv;
    }
    sv->prevMacActive = 0;
    macActiveMacroRoot = sv;
    sv->macState = 0;
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macMakeInactive) || \
    defined(SYNTHMACROS_SELECT_macStart)
SYNTHMACROS_MACSTART_INLINE
void macMakeInactive(SYNTH_VOICE* svoice, s32 newState) {
    if (svoice->macState == newState) {
        return;
    }

    if (svoice->macState == 0) {
        if (svoice->prevMacActive == 0) {
            macActiveMacroRoot = svoice->nextMacActive;
        } else {
            svoice->prevMacActive->nextMacActive = svoice->nextMacActive;
        }

        if (svoice->nextMacActive != 0) {
            svoice->nextMacActive->prevMacActive = svoice->prevMacActive;
        }
    }

    if (newState == 2) {
        UnYieldMacro(svoice, 1);
    }
    svoice->macState = newState;
}
#endif

#undef SYNTHMACROS_MACSTART_INLINE

/* macStart. Positioned after macMakeActive/macMakeInactive (matching
 * reference source order) so both auto-inline here -- confirmed by target
 * disassembly, which shows the full UnYieldMacro/active-list bodies
 * inlined twice (once via the initial macMakeInactive(svoice, 2), once via
 * the closing macMakeActive(svoice)) with no bl to either symbol. */
#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macStart)
u32 macStart(u16 macid, u8 priority, u8 maxVoices, u16 allocId, u8 key, u8 vol, u8 panning,
                u8 midi, u8 midiSet, u8 section, u16 step, u16 trackid, u8 new_vid, u8 vGroup,
                u8 studio, u32 itd) {
    u32 voice;
    u32 vid;
    MSTEP* addr;
    SYNTH_VOICE* svoice;
    u16 seqPrio;

    if ((addr = (MSTEP*)dataGetMacro(macid)) != 0) {
        if (!(key & 0x80) && (seqPrio = fn_8014D740(midiSet, midi)) != 0xFFFF) {
            priority = (u8)seqPrio;
        }

        if ((voice = fn_80157A64(priority, maxVoices, allocId, (key & 0x80) ? 1 : 0)) != (u32)-1) {
            svoice = &synthVoice[voice];
            fn_80157360(svoice);
            macMakeInactive(svoice, 2);
            svoice->cFlags = (svoice->cFlags & 0x10) | 2;

            if (fn_8016246C(voice)) {
                svoice->cFlags |= 1;
            }

            svoice->wait = 0;

            if ((key & 0x80) != 0) {
                key &= 0x7f;
                svoice->fxFlag = 1;
                inpResetMidiCtrl((u8)voice, 0xff, 1);
                fn_80160ED4((u8)voice, 0xff);
                svoice->setup_midi = (u8)voice;
                svoice->setup_midiSet = 0xff;
                svoice->setup_section = 0;
            } else {
                svoice->fxFlag = 0;
                svoice->setup_midi = midi;
                svoice->setup_midiSet = midiSet;
                svoice->setup_section = section;
            }

            svoice->macroId = macid;
            svoice->allocId = allocId;
            svoice->age = 0x75300000;
            svoice->ageSpeed = 0x400;
            svoice->addr = addr;
            svoice->curAddr = addr + step;
            svoice->orgNote = key;
            svoice->curNote = key;
            svoice->curDetune = 0;
            svoice->setup_vol = vol;
            svoice->setup_pan = panning;
            svoice->setup_track = (u8)trackid;
            svoice->callStackEntryNum = 0;
            svoice->callStackIndex = 0;
            svoice->child = (u32)-1;
            svoice->parent = (u32)-1;
            svoice->lastVID = (u32)-1;
            svoice->setup_vGroup = vGroup;
            svoice->setup_studio = studio;
            svoice->setup_itdMode = itd != 0 ? 0 : 1;
            svoice->mesgNum = svoice->mesgRead = svoice->mesgWrite = 0;
            svoice->id = voice | ((macid << 16) | (key << 8));
            voiceSetPriority(svoice, priority);

            if ((vid = fn_801576C4(svoice, new_vid)) != (u32)-1) {
                macMakeActive(svoice);
                return vid;
            }

            if (fn_8016246C(voice)) {
                hwBreak(voice);
            }

            voiceFree(svoice);
        }
    }

    return (u32)-1;
}
#endif

#if defined(SYNTHMACROS_ALL) || defined(SYNTHMACROS_SELECT_macInit)
void macInit(void) {
    u32 i;

    macActiveMacroRoot = 0;
    macTimeQueueRoot = 0;
    macRealTime = 0;
    for (i = 0; i < synthInfo.voiceNum; ++i) {
        synthVoice[i].addr = 0;
        synthVoice[i].macState = 2;
        synthVoice[i].loop = 0;
    }
}
#endif
