/**
 * @file seq_exact_80146E88.c
 * @brief Exact MusyX runtime sequencer island, 0x80146E88 - 0x801485FC.
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


/* This translation unit is a topology-only split of the original seq.c. */

/*
 * Compile-only copies preserve MWCC's original same-TU inlining decisions.
 * This variant uses no custom section.
 */
static inline u32 seqGetPrivateId(u32 seqId) {
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

static inline void KillNotes(SEQ_INSTANCE* seq) {
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

static inline void ResetNotes(SEQ_INSTANCE* seq) {
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


void StartPause(SEQ_INSTANCE* si) {
    if (si->prev != NULL) {
        si->prev->next = si->next;
    } else {
        lbl_8047AF14 = si->next;
    }

    if (si->next != NULL) {
        si->next->prev = si->prev;
    }

    if ((si->next = lbl_8047AF10) != NULL) {
        lbl_8047AF10->prev = si;
    }

    si->prev = NULL;
    lbl_8047AF10 = si;
    si->state = 2;
}

void seqPause(u32 seqId) {
    SEQ_INSTANCE* si;
    seqId = seqGetPrivateId(seqId);

    if (seqId == SND_SEQ_ERROR_ID) {
        return;
    }

    if ((seqId & SND_SEQ_CROSSFADE_ID) == 0) {
        si = &lbl_804285D0[seqId];
        if (si->state == 1) {
            StartPause(si);
            KillNotes(si);
            ResetNotes(si);
        }
    } else {
        si = &lbl_804285D0[seqId & ~SND_SEQ_CROSSFADE_ID];
        if (si->state != 0) {
            si->syncCrossInfo.flags |= SND_CROSSFADE_PAUSENEW;
        }
    }
}

void seqStop(u32 seqId) {
    SEQ_INSTANCE* si;

    if ((seqId = seqGetPrivateId(seqId)) == SND_SEQ_ERROR_ID) {
        return;
    }

    if ((seqId & SND_SEQ_CROSSFADE_ID) == 0) {
        si = &lbl_804285D0[seqId];
        switch (si->state) {
        case 1:
            if (si->prev != NULL) {
                si->prev->next = si->next;
            } else {
                lbl_8047AF14 = si->next;
            }

            KillNotes(&lbl_804285D0[seqId]);
            ResetNotes(&lbl_804285D0[seqId]);
            break;
        case 2:
            if (si->prev != NULL) {
                si->prev->next = si->next;
            } else {
                lbl_8047AF10 = si->next;
            }
            break;
        }

        if (si->next != NULL) {
            si->next->prev = si->prev;
        }
        si->state = 0;
        if (lbl_8047AF0C != NULL) {
            lbl_8047AF0C->prev = si;
        }
        si->next = lbl_8047AF0C;
        si->prev = NULL;
        lbl_8047AF0C = si;
    } else {
        si = &lbl_804285D0[seqId & ~SND_SEQ_CROSSFADE_ID];
        if (si->state != 0) {
            si->syncSeqIdPtr = NULL;
        }
    }
}

void seqSpeed(u32 seqId, u16 speed) {
    u32 i;

    seqId = seqGetPrivateId(seqId);

    if ((seqId & SND_SEQ_CROSSFADE_ID) == 0) {
        for (i = 0; i < 16; i++) {
            lbl_804285D0[seqId].section[i].speed = speed;
        }
    } else {
        seqId &= ~SND_SEQ_CROSSFADE_ID;
        lbl_804285D0[seqId].syncCrossInfo.flags |= SND_CROSSFADE_SPEED;
        lbl_804285D0[seqId].syncCrossInfo.speed2 = speed;
    }
}

void seqContinue(u32 seqId) {
    SEQ_INSTANCE* si;

    seqId = seqGetPrivateId(seqId);

    if ((seqId & SND_SEQ_CROSSFADE_ID) == 0) {
        si = &lbl_804285D0[seqId];

        if (si->state == 2) {
            if (si->prev != NULL) {
                si->prev->next = si->next;
            } else {
                lbl_8047AF10 = si->next;
            }

            if (si->next != NULL) {
                si->next->prev = si->prev;
            }

            if ((si->next = lbl_8047AF14) != NULL) {
                lbl_8047AF14->prev = si;
            }

            si->prev = NULL;
            lbl_8047AF14 = si;
            si->state = 1;
        }
    } else {
        lbl_804285D0[seqId & ~SND_SEQ_CROSSFADE_ID].syncCrossInfo.flags &= ~SND_CROSSFADE_PAUSENEW;
    }
}

void seqMute(u32 seqId, u32 mask1, u32 mask2) {
    seqId = seqGetPrivateId(seqId);
    if (seqId == SND_SEQ_ERROR_ID) {
        return;
    }

    if ((seqId & SND_SEQ_CROSSFADE_ID) == 0) {
        lbl_804285D0[seqId].trackMute[0] = mask1;
        lbl_804285D0[seqId].trackMute[1] = mask2;
    } else {
        seqId &= ~SND_SEQ_CROSSFADE_ID;
        lbl_804285D0[seqId].syncCrossInfo.flags |= SND_CROSSFADE_TRACKMUTE;
        lbl_804285D0[seqId].syncCrossInfo.trackMute2[0] = mask1;
        lbl_804285D0[seqId].syncCrossInfo.trackMute2[1] = mask2;
    }
}

void seqVolume(u8 volume, u16 time, u32 seqId, u8 mode) {
    u32 i;
    u32 pub_id;

    pub_id = seqId;
    seqId = seqGetPrivateId(seqId);
    if (seqId == SND_SEQ_ERROR_ID) {
        return;
    }

    if ((seqId & SND_SEQ_CROSSFADE_ID) == 0) {
        synthVolume(volume, time, lbl_804285D0[seqId].defVGroup, mode, pub_id);
        for (i = 0; i < 64; i++) {
            if (lbl_804285D0[seqId].trackVolGroup[i] != lbl_804285D0[seqId].defVGroup) {
                synthVolume(volume, time, lbl_804285D0[seqId].trackVolGroup[i], SND_SEQVOL_CONTINUE,
                            SND_SEQ_ERROR_ID);
            }
        }
    } else {
        seqId &= ~SND_SEQ_CROSSFADE_ID;
        switch (mode & SND_SEQVOL_MODEMASK) {
        case SND_SEQVOL_CONTINUE:
            lbl_804285D0[seqId].syncCrossInfo.vol2 = volume;
            break;
        case SND_SEQVOL_STOP:
            lbl_804285D0[seqId].syncSeqIdPtr = NULL;
            break;
        case SND_SEQVOL_PAUSE:
            lbl_804285D0[seqId].syncCrossInfo.flags |= SND_CROSSFADE_PAUSENEW;
            lbl_804285D0[seqId].syncCrossInfo.vol2 = volume;
            break;
        case SND_SEQVOL_MUTE:
            lbl_804285D0[seqId].syncCrossInfo.flags |= SND_CROSSFADE_MUTENEW;
            lbl_804285D0[seqId].syncCrossInfo.vol2 = volume;
            break;
        default:
            break;
        }
    }
}

#define SND_CROSSFADE_STOP 0
#define SND_CROSSFADE_PAUSE 1
#define SND_CROSSFADE_CONTINUE 2
#define SND_PLAYPARA_TRACKMUTE 0x1
#define SND_PLAYPARA_SPEED 0x2
#define SND_PLAYPARA_VOLUME 0x4
#define SND_PLAYPARA_SEQVOLDEF 0x8
#define SND_PLAYPARA_PAUSE 0x10

typedef struct {
    u32 flags;         // 0x0
    u32 trackMute[2];   // 0x4
    u16 speed;            // 0xC
    u16 volTime;           // 0xE
    u8 volTarget;            // 0x10
    u8 numSeqVolDef;          // 0x11
    u8 pad_12[2];
    void* seqVolDef;            // 0x14
    u8 numFaded;                  // 0x18
    u8 pad_19[3];
    u8* faded;                       // 0x1C
} SND_PLAYPARA; // size 0x20

extern void sndSeqVolume(u8 volume, u16 time, u32 seqId, u8 mode);
extern void sndSeqMute(u32 seqId, u32 mask1, u32 mask2);
extern void sndSeqSpeed(u32 seqId, u16 speed);
extern void fn_8014D648(u32 seqId); /* sndSeqContinue */
extern u32 fn_8015A21C(u16 sgid, u16 sid, void* arrfile, SND_PLAYPARA* para, u32 irqCall,
                        u8 studio); /* seqPlaySong */
extern u32 fn_8015A368(u16 sgid, u16 sid, void* arrfile, SND_PLAYPARA* para,
                        u8 studio); /* sndSeqPlayEx */

void seqCrossFade(SND_CROSSFADE* ci, u32* new_seqId, u8 irq_call) {
    SND_PLAYPARA pp;
    u32 seqId;
    u16 time;

    seqId = seqGetPrivateId(ci->seqId1);

    if ((ci->flags & SND_CROSSFADE_SYNC) != 0) {
        lbl_804285D0[seqId].syncCrossInfo = *ci;
        lbl_804285D0[seqId].syncActive = TRUE;
        lbl_804285D0[seqId].syncSeqIdPtr = new_seqId;
        lbl_804285D0[seqId].syncCrossInfo.flags &= ~SND_CROSSFADE_SYNC;
        *new_seqId = ci->seqId1 | SND_SEQ_CROSSFADE_ID;
        return;
    }

    if (irq_call) {
        time = ci->time1 < 5 ? 5 : ci->time1;
        if ((ci->flags & SND_CROSSFADE_PAUSE) != 0) {
            seqVolume(0, time, ci->seqId1, SND_SEQVOL_PAUSE);
        } else if ((ci->flags & SND_CROSSFADE_MUTE) != 0) {
            seqVolume(0, time, ci->seqId1, SND_SEQVOL_MUTE);
        } else {
            seqVolume(0, time, ci->seqId1, SND_SEQVOL_STOP);
        }
    } else {
        if ((ci->flags & SND_CROSSFADE_PAUSE) != 0) {
            sndSeqVolume(0, ci->time1, ci->seqId1, SND_SEQVOL_PAUSE);
        } else if ((ci->flags & SND_CROSSFADE_MUTE) != 0) {
            sndSeqVolume(0, ci->time1, ci->seqId1, SND_SEQVOL_MUTE);
        } else {
            sndSeqVolume(0, ci->time1, ci->seqId1, SND_SEQVOL_STOP);
        }
    }

    if (new_seqId == NULL) {
        return;
    }

    if ((ci->flags & SND_CROSSFADE_CONTINUE) != 0) {
        if (seqGetPrivateId(ci->seqId2) != SND_SEQ_ERROR_ID) {
            if (irq_call) {
                seqContinue(ci->seqId2);
                seqVolume(ci->vol2, ci->time2, ci->seqId2, SND_SEQVOL_CONTINUE);
                if ((ci->flags & SND_CROSSFADE_TRACKMUTE) != 0) {
                    seqMute(ci->seqId2, ci->trackMute2[0], ci->trackMute2[1]);
                }
                if ((ci->flags & SND_CROSSFADE_SPEED) != 0) {
                    seqSpeed(ci->seqId2, ci->speed2);
                }
            } else {
                fn_8014D648(ci->seqId2);
                sndSeqVolume(ci->vol2, ci->time2, ci->seqId2, SND_SEQVOL_CONTINUE);
                if ((ci->flags & SND_CROSSFADE_TRACKMUTE) != 0) {
                    sndSeqMute(ci->seqId2, ci->trackMute2[0], ci->trackMute2[1]);
                }
                if ((ci->flags & SND_CROSSFADE_SPEED) != 0) {
                    sndSeqSpeed(ci->seqId2, ci->speed2);
                }
            }
            *new_seqId = ci->seqId2;
        } else {
            *new_seqId = SND_SEQ_ERROR_ID;
        }
    } else {
        pp.flags = SND_PLAYPARA_VOLUME;
        if ((ci->flags & SND_CROSSFADE_PAUSENEW) != 0) {
            pp.flags |= SND_PLAYPARA_PAUSE;
        }
        if ((ci->flags & SND_CROSSFADE_SPEED) != 0) {
            pp.flags |= SND_PLAYPARA_SPEED;
            pp.speed = ci->speed2;
        }
        if ((ci->flags & SND_CROSSFADE_TRACKMUTE) != 0) {
            pp.flags |= SND_PLAYPARA_TRACKMUTE;
            pp.trackMute[0] = ci->trackMute2[0];
            pp.trackMute[1] = ci->trackMute2[1];
        }
        pp.volTime = ci->time2;
        pp.volTarget = ci->vol2;
        pp.numFaded = 0;
        if (irq_call != 0) {
            if ((*new_seqId = fn_8015A21C(ci->gid2, ci->sid2, ci->arr2, &pp, TRUE, ci->studio2)) !=
                    SND_SEQ_ERROR_ID &&
                (ci->flags & SND_CROSSFADE_MUTENEW) != 0) {
                seqMute(*new_seqId, 0, 0);
            }
        } else {
            if ((*new_seqId = fn_8015A368(ci->gid2, ci->sid2, ci->arr2, &pp, ci->studio2)) !=
                    SND_SEQ_ERROR_ID &&
                (ci->flags & SND_CROSSFADE_MUTENEW) != 0) {
                sndSeqMute(*new_seqId, 0, 0);
            }
        }
    }
}

SEQ_EVENT* GenerateNextTrackEvent(u8 trackId) {
    TRACK* track;
    CPAT* pattern;
    SEQ_EVENT* ev;
    u32 patternTime;
    u32 pitchTime;
    u32 modTime;

    track = &lbl_8047AF08->track[trackId];
    pattern = &lbl_8047AF08->pattern[trackId];

    if (track->addr != NULL) {
        ev = &lbl_8047AF08->event[trackId];
        ev->trackId = trackId;
        ev->info.pattern.base = pattern;

        if (pattern->addr == NULL) {
        null_pattern_addr:
            if (track->addr->pattern == 0xffff) {
                track->addr = NULL;
                return NULL;
            }

            if (track->addr->pattern == 0xfffe) {
                if (lbl_8047AF08->trackSectionTab == NULL) {
                    if (lbl_8047AF08->section[0].loopDisable) {
                        track->addr = NULL;
                        return NULL;
                    }
                } else if (lbl_8047AF08->section[lbl_8047AF08->trackSectionTab[trackId]].loopDisable) {
                    track->addr = NULL;
                    return NULL;
                }

                ev->type = 3;
                ev->time = track->addr->time;
                track->addr = &track->base[*((u16*)&track->addr->transpose)];
                return ev;
            }

            ev->type = 4;
            ev->time = track->addr->time;
            ev->info.trackAddr = track->addr;
            ++track->addr;
            return ev;
        }

        pitchTime = pattern->pitchBend.nextTime;
        modTime = pattern->modulation.nextTime;

    loop:
        patternTime = pattern->addr->time + pattern->lTime;
        if (patternTime >= pitchTime) {
            goto use_pitch_time;
        }
        if (patternTime >= modTime) {
            goto use_mod_time;
        }
        if (pattern->addr->key == 0xff && pattern->addr->velocity == 0xff) {
            pattern->addr = NULL;
            goto null_pattern_addr;
        }

        ev->info.trackAddr = (TENTRY*)pattern->addr;
        pattern->lTime = patternTime;

        if ((pattern->addr->key & 0x80) != 0) {
            pattern->addr = (NOTE_DATA*)((u8*)pattern->addr + 4);
            goto use_pattern_time;
        }
        if ((pattern->addr->key | pattern->addr->velocity) == 0) {
            pattern->addr = (NOTE_DATA*)((u8*)pattern->addr + 4);
            goto loop;
        }
        ++pattern->addr;

    use_pattern_time:
        ev->type = 0;
        ev->time = patternTime + pattern->baseTime;
        goto end;

    use_pitch_time:
        if (pitchTime < modTime) {
            ev->time = pitchTime + pattern->baseTime;
            ev->type = 2;
            goto end;
        }

    use_mod_time:
        ev->time = modTime + pattern->baseTime;
        ev->type = 1;

    end:
        return ev;
    }

    return NULL;
}
