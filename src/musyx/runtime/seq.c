/**
 * @file seq.c
 * @brief MusyX runtime sequencer prefix, 0x8014635C - 0x80146E88.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Reference:
 * AxioDL/musyx `musyx/runtime/seq.c`, cross-verified byte-exact against the
 * matched copies in Mario Party 4, Metroid Prime and Mario Strikers
 * (GC/1.3.2). Boundary evidence: seqGetPrivateId (0x8014635C) is the first
 * seq.c function after the item-use function fn_80144574; seqInit
 * (0x8014A094 + 0x1A8) is reference seq.c's last function and ends exactly
 * at synthSetBpm (0x8014A23C), reference synth.c's first.
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
extern u32 inpGetMidiCtrl(u32 ctrl, u32 bank, u32 channel);
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
extern void fn_801621BC(u32* ptr);  /* peopleFieldUtilDispatch - same-TU asm wrapper */

/* ===================================================================
 * All functions in this module are listed above in the MODULE MAP.
 * The asm files in build/GC6E01/asm/ remain the authoritative
 * implementation until each function is individually decompiled.
 * =================================================================== */

/* ===================================================================
 * MusyX runtime: seq.c (sequencer instance / playback control).
 * Reference: AxioDL/musyx `musyx/runtime/seq.c`, cross-verified against
 * byte-exact matched copies in Mario Party 4, Metroid Prime and Mario
 * Strikers (GC/1.3.2). SEQ_INSTANCE / SND_CROSSFADE layouts below are
 * copied field-for-field from Metroid Prime's musyx/seq.h and confirmed
 * byte-identical against this TU's own disassembly (defVGroup @0xEB0,
 * syncCrossInfo @0xEB4, trackVolGroup @0x324, noteUsed/noteKeyOff
 * @0xE64/0xE6C all match exactly). One proven version delta: PRG_STATE
 * here is the pre-2.0.1 4-byte variant (no `program` field) even though
 * this build otherwise sits close to XD 2.0.3 -- proven by the
 * prgState[16] gap arithmetic (0xEB0-0xE70 = 0x40 = 16*4). Struct
 * internals not touched by the functions implemented so far (track[],
 * pattern[], prgState[], event[], section[]) are kept as opaque padding
 * until a later pass needs them.
 * =================================================================== */

typedef struct NOTE {
    struct NOTE* next; // 0x0
    struct NOTE* prev; // 0x4
    u32 id;            // 0x8
    s32 endTime;        // 0xC
    u8 section;          // 0x10
    u8 timeIndex;         // 0x11
    u8 reserved[2];        // 0x12
} NOTE; // size 0x14

typedef struct {
    u32 seqId1;         // 0x0
    u16 time1;           // 0x4
    u16 pad_6;            // 0x6
    u32 seqId2;            // 0x8
    u16 time2;              // 0xC
    u16 pad_E;               // 0xE
    void* arr2;               // 0x10
    u16 gid2;                  // 0x14
    u16 sid2;                    // 0x16
    u8 vol2;                      // 0x18
    u8 studio2;                     // 0x19
    u16 pad_1A;                      // 0x1A
    u32 trackMute2[2];                // 0x1C
    u16 speed2;                        // 0x24
    u8 flags;                            // 0x26
    u8 pad_27;                            // 0x27
} SND_CROSSFADE; // size 0x28

typedef struct {
    volatile u32 time; // 0x0
    u32 bpm;            // 0x4
} MTRACK_DATA;           // size 0x8

typedef struct {
    MTRACK_DATA* base; // 0x0
    MTRACK_DATA* addr; // 0x4
} MTRACK;               // size 0x8

typedef struct {
    u32 low;  // 0x0
    s32 high; // 0x4
} TICKS;      // size 0x8

typedef struct {
    u32 time;       // 0x0
    u8 prgChange;    // 0x4
    u8 velocity;      // 0x5
    u8 res[2];         // 0x6
    u16 pattern;        // 0x8
    s8 transpose;        // 0xA
    s8 velocityAdd;       // 0xB
} TENTRY; // size 0xC

typedef struct {
    TENTRY* base; // 0x0
    TENTRY* addr; // 0x4
} TRACK;          // size 0x8

typedef struct {
    u16 time;     // 0x0
    u8 key;        // 0x2
    u8 velocity;    // 0x3
    u16 length;      // 0x4
} NOTE_DATA;          // size 0x6

typedef struct {
    u8* nextAddr;    // 0x0
    u16 value;        // 0x4
    s16 nextDelta;     // 0x6
    u32 nextTime;       // 0x8
} SEQ_STREAM;            // size 0xC

typedef struct {
    u32 lTime;                // 0x0
    u32 baseTime;              // 0x4
    NOTE_DATA* addr;            // 0x8
    TENTRY* patternInfo;         // 0xC
    SEQ_STREAM pitchBend;          // 0x10
    SEQ_STREAM modulation;           // 0x1C
    u8 midi;                          // 0x28
    u8 pad_29[3];                      // 0x29
} CPAT;                                 // size 0x2C

typedef struct SEQ_EVENT {
    struct SEQ_EVENT* next; // 0x0
    struct SEQ_EVENT* prev;  // 0x4
    u32 time;                 // 0x8
    union {
        TENTRY* trackAddr;
        struct {
            NOTE_DATA* addr; // 0x0
            CPAT* base;       // 0x4
        } pattern;
    } info;               // 0xC
    u8 type;               // 0x14
    u8 trackId;              // 0x15
    u8 pad_16[2];             // 0x16
} SEQ_EVENT;                   // size 0x18

typedef struct {
    u16 macId;     // 0x0
    u8 priority;    // 0x2
    u8 maxVoices;    // 0x3
} PRG_STATE;          // size 0x4 (pre-2.0.1 variant, no `program` field --
                       // proven by prgState[16] gap == 0x40 == 16*4)

typedef struct {
    u16 macro;    // 0x0
    u8 prio;       // 0x2
    u8 maxVoices;   // 0x3
    u8 index;        // 0x4
    u8 reserved;      // 0x5
} PAGE;                // size 0x6

typedef struct {
    u32 tTab;          // 0x0
    u32 pTab;           // 0x4
    u32 tmTab;           // 0x8
    u32 mTrack;           // 0xC
    u32 info;              // 0x10
    u32 loopPoint[16];      // 0x14
    u32 tsTab;               // 0x54
} ARR;                        // size 0x58

#define ARR_GET(arr, offset) ((void*)((offset) + (u32)(arr)))
#define ARR_GET_TYPE(arr, offset, ty) ((ty)ARR_GET(arr, offset))

typedef struct {
    MTRACK mTrack;              // 0x0
    u32 bpm;                     // 0x8
    TICKS tickDelta[2];           // 0xC
    SEQ_EVENT* globalEventRoot;     // 0x1C
    TICKS time[2];                    // 0x20
    u8 timeIndex;                       // 0x30
    u8 pad_31;                            // 0x31
    u16 speed;                              // 0x32
    u16 loopCnt;                              // 0x34
    u8 loopDisable;                             // 0x36
    u8 pad_37;                                    // 0x37
} SEQ_SECTION; // size 0x38

typedef struct SEQ_INSTANCE {
    struct SEQ_INSTANCE* next; // 0x0
    struct SEQ_INSTANCE* prev; // 0x4
    u8 state;                   // 0x8
    u8 index;                    // 0x9
    u16 groupID;                  // 0xA
    u32 publicId;                   // 0xC
    PAGE* normtab;                    // 0x10
    u8 normTrans[128];                  // 0x14
    PAGE* drumtab;                        // 0x94
    u8 drumTrans[128];                     // 0x98
    ARR* arrbase;                            // 0x118
    u32 trackMute[2];                          // 0x11C
    TRACK track[64];                             // 0x124
    u8 trackVolGroup[64];                          // 0x324
    CPAT pattern[64];                                // 0x364
    NOTE* noteUsed[2];                                 // 0xE64
    NOTE* noteKeyOff;                                    // 0xE6C
    PRG_STATE prgState[16];                                // 0xE70
    u8 defVGroup;                                            // 0xEB0
    u8 pad_EB1[3];                                             // 0xEB1
    SND_CROSSFADE syncCrossInfo;                                 // 0xEB4
    u32* syncSeqIdPtr;                                             // 0xEDC
    u8 syncActive;                                                   // 0xEE0
    u8 defStudio;                                                     // 0xEE1
    u8 keyOffCheck;                                                    // 0xEE2
    u8 pad_EE3;                                                         // 0xEE3
    SEQ_EVENT event[64];                                                 // 0xEE4
    u8* trackSectionTab;                                                   // 0x14E4
    SEQ_SECTION section[16];                                                // 0x14E8
} SEQ_INSTANCE; // size 0x1868

extern SEQ_INSTANCE lbl_804285D0[8]; /* seqInstance */
extern NOTE* lbl_8047AF04;           /* noteFree */
extern SEQ_INSTANCE* lbl_8047AF08;   /* cseq (current sequence being processed) */
extern SEQ_INSTANCE* lbl_8047AF0C;   /* seqFreeRoot */
extern SEQ_INSTANCE* lbl_8047AF10;   /* seqPausedRoot */
extern SEQ_INSTANCE* lbl_8047AF14;   /* seqActiveRoot */
extern u32 lbl_8047AF00;             /* curSeqId */

extern void synthSetBpm(u32 bpm, u8 seqId, u8 secIndex);
extern f64 fmod(f64 a, f64 b); /* fmod (wraps __ieee754_fmod) */
extern f64 floor(f64 x);
extern SEQ_EVENT* fn_801485FC(SEQ_EVENT* event, u8 secIndex, u32* loopFlag); /* HandleEvent */
extern u32 synthSendKeyOff(u32 id);
extern u32 synthIsFadeOutActive(u8 volGroup);
extern u32 sndFXCheck(u32 id); /* sndFXCheck */
extern u8 lbl_8047AEFC;         /* curFadeOutState */
extern u32 lbl_8047AEF8;        /* seq_next_id */
extern NOTE lbl_804271D0[256];  /* seqNote */
extern u16 lbl_80434910[8][16]; /* seqMIDIPriority */

extern void voiceKillSound(u32 id);
extern void synthVolume(u8 volume, u16 time, u8 volGroup, u8 mode, u32 pubId);

#define SND_SEQ_ERROR_ID 0xFFFFFFFFU
#define SND_SEQ_CROSSFADE_ID 0x80000000U
#define SND_CROSSFADE_SYNC 4
#define SND_CROSSFADE_PAUSENEW 8
#define SND_CROSSFADE_TRACKMUTE 16
#define SND_CROSSFADE_SPEED 32
#define SND_CROSSFADE_MUTE 64
#define SND_CROSSFADE_MUTENEW 128
#define SND_SEQVOL_CONTINUE 0
#define SND_SEQVOL_STOP 1
#define SND_SEQVOL_PAUSE 2
#define SND_SEQVOL_MUTE 3
#define SND_SEQVOL_MODEMASK 0xF

typedef struct {
    u8 program;
    u8 volume;
    u8 panning;
    u8 reverb;
    u8 chorus;
} MIDI_CHANNEL_SETUP;

typedef struct {
    u16 songId;
    u16 reserved;
    MIDI_CHANNEL_SETUP channel[16];
} MIDISETUP;

typedef struct {
    u8 track;
    u8 volGroup;
} SND_SEQVOLDEF;

typedef struct {
    u32 flags;
    u32 trackMute[2];
    u16 speed;
    struct {
        u16 time;
        u8 target;
    } volume;
    u8 numSeqVolDef;
    SND_SEQVOLDEF* seqVolDef;
    u8 numFaded;
    u8* faded;
} SND_PLAYPARA;

#define SND_PLAYPARA_TRACKMUTE 1
#define SND_PLAYPARA_SPEED 2
#define SND_PLAYPARA_VOLUME 4
#define SND_PLAYPARA_SEQVOLDEF 8
#define SND_PLAYPARA_PAUSE 0x10

extern u8 lbl_80435464[64]; /* synthTrackVolume */
extern void synthSetMusicVolumeType(u8 volumeGroup, u8 type);
extern void fn_801603C0(u8 ctrl, u8 midi, u8 midiSet, u8 value);
extern void inpResetMidiCtrl(u8 midi, u8 midiSet, u32 coldReset);
extern void fn_80160ED4(u8 midi, u8 midiSet);
extern void InitTrackEvents(void);

static void StartPause(SEQ_INSTANCE* si);

static inline void DoPrgChange(SEQ_INSTANCE* seq, u8 program, u8 midi)
{
    lbl_80434910[lbl_8047AF00][midi] = 0xFFFF;
    if (midi != 9) {
        program = seq->normTrans[program];
        if (program == 0xFF) {
            return;
        }
        seq->prgState[midi].macId = seq->normtab[program].macro;
        seq->prgState[midi].priority = seq->normtab[program].prio;
        seq->prgState[midi].maxVoices = seq->normtab[program].maxVoices;
        return;
    }
    program = seq->drumTrans[program];
    if (program == 0xFF) {
        return;
    }
    seq->prgState[midi].macId = seq->drumtab[program].macro;
    seq->prgState[midi].priority = seq->drumtab[program].prio;
    seq->prgState[midi].maxVoices = seq->drumtab[program].maxVoices;
}

static inline void BuildTransTab(u8* table, PAGE* page)
{
    u8 i;

    for (i = 0; i < 128; i++) {
        table[i] = 0xFF;
    }
    for (i = 0; page->index != 0xFF; i++, page++) {
        table[page->index] = i;
    }
}

static inline u32 GetPublicId(u32 seqId)
{
    u32 publicId;
    SEQ_INSTANCE* seq;

    do {
        publicId = lbl_8047AEF8++;
        lbl_8047AEF8 &= ~SND_SEQ_CROSSFADE_ID;
        for (seq = lbl_8047AF14; seq != NULL; seq = seq->next) {
            if (seq->publicId == publicId) {
                publicId = SND_SEQ_ERROR_ID;
                break;
            }
        }
        for (seq = lbl_8047AF10; seq != NULL; seq = seq->next) {
            if (seq->publicId == publicId) {
                publicId = SND_SEQ_ERROR_ID;
                break;
            }
        }
    } while (publicId == SND_SEQ_ERROR_ID);

    lbl_804285D0[seqId].publicId = publicId;
    return publicId;
}

u32 seqGetPrivateId(u32 seqId) {
    SEQ_INSTANCE* si;
    for (si = lbl_8047AF14; si != NULL; si = si->next) {
        if (si->publicId == (seqId & ~SND_SEQ_CROSSFADE_ID)) {
            return si->index | (seqId & SND_SEQ_CROSSFADE_ID);
        }
    }
    for (si = lbl_8047AF10; si != NULL; si = si->next) {
        if (si->publicId == (seqId & ~SND_SEQ_CROSSFADE_ID)) {
            return si->index | (seqId & SND_SEQ_CROSSFADE_ID);
        }
    }
    return SND_SEQ_ERROR_ID;
}

u32 fn_801463C4(PAGE* normal, PAGE* drum, MIDISETUP* midiSetup, u32* song,
                SND_PLAYPARA* para, u8 studio, u16 groupId)
{
    ARR* arr;
    u32* trackTable;
    s32 i;
    SEQ_INSTANCE* seq;
    SEQ_INSTANCE* oldSeq;
    u32 seqId;
    u32 bpm;

    seq = lbl_8047AF0C;
    if (seq == NULL) {
        return SND_SEQ_ERROR_ID;
    }
    if ((lbl_8047AF0C = seq->next) != NULL) {
        lbl_8047AF0C->prev = NULL;
    }
    if ((seq->next = lbl_8047AF14) != NULL) {
        lbl_8047AF14->prev = seq;
    }
    seq->prev = NULL;
    lbl_8047AF14 = seq;
    seq->state = 1;
    for (i = 0; i < 16; i++) {
        seq->section[i].globalEventRoot = NULL;
    }

    seqId = seq->index;
    seq->syncActive = 0;
    seq->normtab = normal;
    seq->drumtab = drum;
    seq->arrbase = (ARR*)song;
    seq->groupID = groupId;
    BuildTransTab(seq->normTrans, seq->normtab);
    BuildTransTab(seq->drumTrans, seq->drumtab);
    seq->defVGroup = seqId + 23;
    for (i = 0; i < 64; i++) {
        seq->trackVolGroup[i] = seq->defVGroup;
    }
    seq->defStudio = studio;

    if (para == NULL) {
        seq->trackMute[0] = -1;
        seq->trackMute[1] = -1;
        for (i = 0; i < 16; i++) {
            seq->section[i].speed = 256;
        }
        synthVolume(127, 0, seq->defVGroup, 0, SND_SEQ_ERROR_ID);
    } else {
        if ((para->flags & SND_PLAYPARA_TRACKMUTE) != 0) {
            seq->trackMute[0] = para->trackMute[0];
            seq->trackMute[1] = para->trackMute[1];
        } else {
            seq->trackMute[0] = -1;
            seq->trackMute[1] = -1;
        }
        if ((para->flags & SND_PLAYPARA_SPEED) != 0) {
            for (i = 0; i < 16; i++) {
                seq->section[i].speed = para->speed;
            }
        } else {
            for (i = 0; i < 16; i++) {
                seq->section[i].speed = 256;
            }
        }
        if ((para->flags & SND_PLAYPARA_SEQVOLDEF) != 0) {
            for (i = 0; i < para->numSeqVolDef; i++) {
                seq->trackVolGroup[para->seqVolDef[i].track] =
                    para->seqVolDef[i].volGroup;
                synthSetMusicVolumeType(para->seqVolDef[i].volGroup, 0);
            }
        }
        if ((para->flags & SND_PLAYPARA_VOLUME) != 0) {
            synthVolume(para->volume.target, para->volume.time,
                        seq->defVGroup, 0, SND_SEQ_ERROR_ID);
            for (i = 0; i < para->numFaded; i++) {
                synthVolume(para->volume.target, para->volume.time,
                            para->faded[i], 0, SND_SEQ_ERROR_ID);
            }
        }
    }

    arr = (ARR*)song;
    if ((arr->info & 0x80000000) != 0) {
        seq->trackSectionTab = ARR_GET(arr, arr->tsTab);
    } else {
        seq->trackSectionTab = NULL;
    }
    bpm = arr->info & 0x0FFFFFFF;
    if ((arr->info & 0x40000000) == 0) {
        bpm <<= 10;
    }
    for (i = 0; i < 16; i++) {
        seq->section[i].bpm = bpm;
        synthSetBpm(bpm >> 10, seqId, i);
        if (arr->mTrack != 0) {
            seq->section[i].mTrack.base = ARR_GET(arr, arr->mTrack);
            seq->section[i].mTrack.addr = seq->section[i].mTrack.base;
        } else {
            seq->section[i].mTrack.base = NULL;
        }
        seq->section[i].loopDisable = 0;
        seq->section[i].loopCnt = 0;
    }

    trackTable = ARR_GET(arr, arr->tTab);
    for (i = 0; i < 64; i++) {
        lbl_80435464[i] = 127;
        seq->pattern[i].addr = NULL;
        if (trackTable[i] != 0) {
            seq->track[i].addr = seq->track[i].base =
                ARR_GET(arr, trackTable[i]);
        } else {
            seq->track[i].addr = seq->track[i].base = NULL;
        }
    }
    seq->noteUsed[0] = NULL;
    seq->noteUsed[1] = NULL;
    seq->noteKeyOff = NULL;
    for (i = 0; i < 16; i++) {
        inpResetMidiCtrl(i, seqId, 1);
    }
    for (i = 0; i < 16; i++) {
        seq->prgState[i].macId = 0xFFFF;
    }
    for (i = 0; i < 16; i++) {
        fn_80160ED4(i, seqId);
    }
    if (midiSetup != NULL) {
        for (i = 0; i < 16; i++) {
            DoPrgChange(seq, midiSetup->channel[i].program, i);
            fn_801603C0(7, i, seqId, midiSetup->channel[i].volume);
            fn_801603C0(10, i, seqId, midiSetup->channel[i].panning);
            fn_801603C0(91, i, seqId, midiSetup->channel[i].reverb);
            fn_801603C0(93, i, seqId, midiSetup->channel[i].chorus);
        }
    }
    for (i = 0; i < 16; i++) {
        lbl_80434910[seqId][i] = 0xFFFF;
    }
    for (i = 0; i < 16; i++) {
        seq->section[i].time[0].high = 0;
        seq->section[i].time[0].low = 0;
        seq->section[i].time[1].high = 0;
        seq->section[i].time[1].low = 0;
        seq->section[i].timeIndex = 0;
    }
    seq->keyOffCheck = 0;
    if (para != NULL && (para->flags & SND_PLAYPARA_PAUSE) != 0) {
        StartPause(seq);
    }
    oldSeq = lbl_8047AF08;
    lbl_8047AF08 = seq;
    InitTrackEvents();
    lbl_8047AF08 = oldSeq;
    seqId = GetPublicId(seqId);
    return seqId;
}

static void KillNotes(SEQ_INSTANCE* seq) {
    NOTE* n;
    u32 i;

    for (i = 0; i < 2; i++) {
        for (n = seq->noteUsed[i]; n != NULL; n = n->next) {
            voiceKillSound(n->id);
        }
    }

    for (n = seq->noteKeyOff; n != NULL; n = n->next) {
        voiceKillSound(n->id);
    }
}

static void ResetNotes(SEQ_INSTANCE* seq) {
    NOTE* n;
    u32 i;

    for (i = 0; i < 2; i++) {
        n = seq->noteUsed[i];
        if (n != NULL) {
            for (; n->next != NULL; n = n->next) {
            }

            if (lbl_8047AF04 != NULL) {
                n->next = lbl_8047AF04;
                lbl_8047AF04->prev = n;
            }

            lbl_8047AF04 = seq->noteUsed[i];
            seq->noteUsed[i] = NULL;
        }
    }

    n = seq->noteKeyOff;
    if (n != NULL) {
        for (; n->next != NULL; n = n->next) {
        }

        if (lbl_8047AF04 != NULL) {
            n->next = lbl_8047AF04;
            lbl_8047AF04->prev = n;
        }

        lbl_8047AF04 = seq->noteKeyOff;
        seq->noteKeyOff = NULL;
    }
}
