/**
 * @file seq_candidate_801485FC.c
 * @brief MusyX runtime sequencer suffix candidate, 0x801485FC - 0x8014A23C.
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
extern u32 synthSendKeyOff(u32 id);
extern u32 synthIsFadeOutActive(u8 volGroup);
extern u32 sndFXCheck(u32 id); /* sndFXCheck */
extern u8 lbl_8047AEFC;         /* curFadeOutState */
extern u32 lbl_8047AEF8;        /* seq_next_id */
extern NOTE lbl_804271D0[256];  /* seqNote */
extern u16 lbl_80434910[8][16]; /* seqMIDIPriority */

extern void voiceKillSound(u32 id);
extern void synthVolume(u8 volume, u16 time, u8 volGroup, u8 mode, u32 pubId);
extern void fn_801603C0(u8 ctrl, u8 midi, u8 midiSet, u8 value);
extern void inpSetMidiCtrl14(u8 ctrl, u8 midi, u8 midiSet, u16 value);
extern void inpResetMidiCtrl(u8 midi, u8 midiSet, u32 coldReset);
extern void seqCrossFade(SND_CROSSFADE* ci, u32* newSeqId, u8 irqCall);
extern u32 synthStartSound(u16 id, u8 priority, u8 maxVoices, u8 key,
                          u8 volume, u8 panning, u8 midi, u8 midiSet,
                          u8 section, u16 step, u16 trackId, u8 volumeGroup,
                          s16 priorityOffset, u8 studio, u32 itd);
extern u8 lbl_804356A4[16]; /* synthITDDefault: { music, sfx } */
extern SEQ_EVENT* GenerateNextTrackEvent(u8 trackId);
extern SEQ_EVENT* fn_801485FC(SEQ_EVENT* event, u8 secIndex, u32* loopFlag);

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

#if !defined(SEQ_SUFFIX_BANK_ACTIVE)
typedef struct {
    u32 headerLen;
    u32 pitchBend;
    u32 modulation;
    u32 noteData;
} SEQ_PATTERN;

static inline NOTE* AllocateNote(u32 endTime, u8 section)
{
    NOTE* n;
    NOTE* nl;
    NOTE* last;

    n = lbl_8047AF04;
    if (n != NULL) {
        if ((lbl_8047AF04 = n->next) != NULL) {
            lbl_8047AF04->prev = NULL;
        }
        n->endTime = endTime;
        n->section = section;
        n->timeIndex = lbl_8047AF08->section[section].timeIndex;
        last = NULL;
        for (nl = lbl_8047AF08->noteUsed[n->timeIndex]; nl != NULL;
             last = nl, nl = nl->next)
        {
            if (nl->endTime > n->endTime) {
                n->next = nl;
                n->prev = last;
                if (last != NULL) {
                    last->next = n;
                } else {
                    lbl_8047AF08->noteUsed[n->timeIndex] = n;
                }
                nl->prev = n;
                return n;
            }
        }
        n->prev = last;
        if (last != NULL) {
            last->next = n;
        } else {
            lbl_8047AF08->noteUsed[n->timeIndex] = n;
        }
        n->next = NULL;
    }
    return n;
}

static inline void FreeNote(NOTE* n)
{
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }
    if (n->prev != NULL) {
        n->prev->next = n->next;
    } else {
        lbl_8047AF08->noteUsed[n->timeIndex] = n->next;
    }
    if ((n->next = lbl_8047AF04) != NULL) {
        lbl_8047AF04->prev = n;
    }
    n->prev = NULL;
    lbl_8047AF04 = n;
}

static inline void KeyOffNotes(void)
{
    NOTE* note;
    NOTE* next;
    u32 i;

    for (i = 0; i < 2; i++) {
        note = lbl_8047AF08->noteUsed[i];
        while (note != NULL) {
            next = note->next;
            synthSendKeyOff(note->id);
            if ((lbl_8047AF08->noteUsed[i] = note->next) != NULL) {
                lbl_8047AF08->noteUsed[i]->prev = NULL;
            }
            if ((note->next = lbl_8047AF08->noteKeyOff) != NULL) {
                lbl_8047AF08->noteKeyOff->prev = note;
            }
            lbl_8047AF08->noteKeyOff = note;
            note = next;
        }
    }
}

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

static inline u8* GetStreamValue(u8* stream, u16* deltaTime, s16* deltaData)
{
    u8 b1;
    u8 b2;
    s16 value;

    b1 = stream[0];
    b2 = stream[1];
    if (b1 == 0x80 && b2 == 0) {
        return NULL;
    }
    if ((b1 & 0x80) != 0) {
        *deltaTime = (((u16)b1 & 0x7F) << 8) | b2;
        stream += 2;
    } else {
        *deltaTime = b1;
        stream++;
    }
    b1 = stream[0];
    b2 = stream[1];
    if ((b1 & 0x80) != 0) {
        value = (((u16)b1 & 0x7F) << 8) | b2;
        value |= (value & 0x4000) << 1;
        *deltaData = value;
        stream += 2;
    } else {
        b1 |= (b1 & 0x40) << 1;
        *deltaData = (s8)b1;
        stream++;
    }
    return stream;
}

static inline void InitStream(SEQ_STREAM* stream, u32 offset)
{
    u16 delta;

    if (offset != 0) {
        stream->nextAddr =
            GetStreamValue(ARR_GET(lbl_8047AF08->arrbase, offset), &delta,
                           &stream->nextDelta);
        if (stream->nextAddr != NULL) {
            stream->nextTime = delta;
        } else {
            stream->nextTime = 0x7FFFFFFF;
        }
    } else {
        stream->nextTime = 0x7FFFFFFF;
    }
}

static inline u16 HandleStream(SEQ_STREAM* stream)
{
    u16 delta;

    stream->value += stream->nextDelta;
    if (stream->nextAddr != NULL) {
        stream->nextAddr =
            GetStreamValue(stream->nextAddr, &delta, &stream->nextDelta);
        if (stream->nextAddr != NULL) {
            stream->nextTime += delta;
        } else {
            stream->nextTime = 0x7FFFFFFF;
        }
    } else {
        stream->nextTime = 0x7FFFFFFF;
    }
    return stream->value;
}

SEQ_EVENT* fn_801485FC(SEQ_EVENT* event, u8 secIndex, u32* loopFlag)
{
    CPAT* pa;
    NOTE_DATA* pe;
    s32 velocity;
    s32 key;
    u8 midi;
    u16 macId;
    NOTE* note;
    TENTRY* trackEntry;
    CPAT* pattern;
    u32* patternTable;
    SEQ_PATTERN* patternData;

    switch (event->type) {
    case 4:
        trackEntry = event->info.trackAddr;
        pattern = &lbl_8047AF08->pattern[event->trackId];
        patternTable =
            ARR_GET(lbl_8047AF08->arrbase, lbl_8047AF08->arrbase->pTab);
        patternData =
            ARR_GET(lbl_8047AF08->arrbase, patternTable[trackEntry->pattern]);
        pattern->addr = (NOTE_DATA*)&patternData->noteData;
        pattern->lTime = 0;
        pattern->baseTime = trackEntry->time;
        pattern->patternInfo = trackEntry;
        InitStream(&pattern->pitchBend, patternData->pitchBend);
        pattern->pitchBend.value = 0x2000;
        InitStream(&pattern->modulation, patternData->modulation);
        pattern->modulation.value = 0;
        pattern->midi =
            ARR_GET_TYPE(lbl_8047AF08->arrbase, lbl_8047AF08->arrbase->tmTab,
                         u8*)[event->trackId];
        if (trackEntry->prgChange != 0xFF) {
            DoPrgChange(lbl_8047AF08, trackEntry->prgChange, pattern->midi);
        }
        if (trackEntry->velocity != 0xFF) {
            fn_801603C0(7, pattern->midi, lbl_8047AF00,
                        trackEntry->velocity);
        }
        break;

    case 0:
        pe = event->info.pattern.addr;
        pa = event->info.pattern.base;
        key = pe->key;
        velocity = pe->velocity;
        midi = pa->midi;
        if ((key & 0x80) != 0) {
            switch (velocity) {
            case 0:
                DoPrgChange(lbl_8047AF08, key & 0x7F, midi);
                break;
            case 1:
                fn_801603C0(0x82, midi, lbl_8047AF00, key & 0x7F);
                break;
            default:
                if ((velocity & 0x80) != 0x80) {
                    break;
                }
                switch (velocity & 0x7F) {
                case 0x68:
                    if (lbl_8047AF08->syncActive) {
                        seqCrossFade(&lbl_8047AF08->syncCrossInfo,
                                     lbl_8047AF08->syncSeqIdPtr, 1);
                        lbl_8047AF08->syncActive = 0;
                    }
                    break;
                case 0x69:
                    lbl_80434910[lbl_8047AF00][midi] = key & 0x7F;
                    break;
                case 0x6A:
                    lbl_80434910[lbl_8047AF00][midi] =
                        (key & 0x7F) + 0x80;
                    break;
                case 0x79:
                    inpResetMidiCtrl(midi, lbl_8047AF00, 0);
                    break;
                case 0x7B:
                    KeyOffNotes();
                    break;
                default:
                    fn_801603C0(velocity & 0x7F, midi, lbl_8047AF00,
                                key & 0x7F);
                    break;
                }
            }
            break;
        }
        if ((lbl_8047AF08->trackMute[event->trackId / 32] &
             (1 << (event->trackId & 0x1F))) != 0)
        {
            macId = lbl_8047AF08->prgState[midi].macId;
            if (macId != 0xFFFF) {
                key += pa->patternInfo->transpose;
                key = key < 0 ? 0 : (key > 0x7F ? 0x7F : key);
                velocity += pa->patternInfo->velocityAdd;
                velocity =
                    velocity < 0 ? 0 : (velocity > 0x7F ? 0x7F : velocity);
                note = AllocateNote(event->time + pe->length, secIndex);
                if (note != NULL) {
                    note->id = synthStartSound(
                        macId, lbl_8047AF08->prgState[midi].priority,
                        lbl_8047AF08->prgState[midi].maxVoices, key, velocity,
                        64, midi, lbl_8047AF00, secIndex, 0, event->trackId,
                        lbl_8047AF08->trackVolGroup[event->trackId],
                        lbl_8047AEFC ? -1 : 0, lbl_8047AF08->defStudio,
                        lbl_804356A4[lbl_8047AF08->defStudio * 2]);
                    if (note->id == SND_SEQ_ERROR_ID) {
                        FreeNote(note);
                    }
                }
            }
        }
        break;

    case 2:
        pa = event->info.pattern.base;
        inpSetMidiCtrl14(0x80, pa->midi, lbl_8047AF00,
                         HandleStream(&pa->pitchBend));
        break;
    case 1:
        pa = event->info.pattern.base;
        inpSetMidiCtrl14(1, pa->midi, lbl_8047AF00,
                         HandleStream(&pa->modulation));
        break;
    case 3:
        *loopFlag |= 1;
        return NULL;
    }
    return GenerateNextTrackEvent(event->trackId);
}
#endif


/* This translation unit is a topology-only split of the original seq.c. */
#if !defined(SEQ_SUFFIX_BANK_ACTIVE) || \
    defined(SEQ_EXACT_80149090_8014A23C)
/*
 * Compile-only copies preserve MWCC's original same-TU inlining decisions.
 * Declaring them inline prevents unused out-of-line copies from being emitted.
 */
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

static void InsertGlobalEvent(SEQ_SECTION* section, SEQ_EVENT* event) {
    SEQ_EVENT* el;
    SEQ_EVENT* last_el;

    last_el = NULL;
    el = section->globalEventRoot;
    for (; el != NULL; last_el = el, el = el->next) {
        if (el->time > event->time) {
            event->next = el;
            event->prev = last_el;
            if (last_el != NULL) {
                last_el->next = event;
            } else {
                section->globalEventRoot = event;
            }
            el->prev = event;
            return;
        }
    }

    event->prev = last_el;
    if (last_el != NULL) {
        last_el->next = event;
    } else {
        section->globalEventRoot = event;
    }
    event->next = NULL;
}

/* Exported for the physical split; static in the original seq.c TU. */
void InitTrackEvents(void) {
    u32 i;
    SEQ_EVENT* ev;

    if (lbl_8047AF08->trackSectionTab == NULL) {
        for (i = 0; i < 0x40; i += 1) {
            if ((ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(lbl_8047AF08->section, ev);
            }
        }
    } else {
        for (i = 0; i < 0x40; i += 1) {
            if ((ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(lbl_8047AF08->section + lbl_8047AF08->trackSectionTab[i], ev);
            }
        }
    }
}

static void InitTrackEventsSection(u8 secIndex) {
    u32 i;
    SEQ_EVENT* ev;

    if (lbl_8047AF08->trackSectionTab == NULL) {
        for (i = 0; i < 64; i += 1) {
            if ((ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(lbl_8047AF08->section, ev);
            }
        }
    } else {
        for (i = 0; i < 64; i += 1) {
            if (secIndex == lbl_8047AF08->trackSectionTab[i] &&
                (ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(lbl_8047AF08->section + secIndex, ev);
            }
        }
    }
}

static u32 GetNextEventTime(SEQ_SECTION* section) {
    if (section->globalEventRoot == NULL) {
        return 0;
    }
    return section->globalEventRoot->time;
}

static SEQ_EVENT* GetGlobalEvent(SEQ_SECTION* section) {
    SEQ_EVENT* ev;
    ev = section->globalEventRoot;
    if (ev != NULL && (section->globalEventRoot = ev->next) != NULL) {
        section->globalEventRoot->prev = NULL;
    }
    return ev;
}

static void HandleMasterTrack(u8 secIndex) {
    SEQ_SECTION* section;

    section = &lbl_8047AF08->section[secIndex];
    if (section->mTrack.base != NULL) {
        while (section->mTrack.addr->time != -1) {
            if (section->mTrack.addr->time > section->time[section->timeIndex].high) {
                break;
            }

            if ((lbl_8047AF08->arrbase->info & 0x40000000) != 0) {
                synthSetBpm((section->bpm = section->mTrack.addr->bpm) >> 10, lbl_8047AF00,
                            secIndex);
            } else {
                synthSetBpm(section->mTrack.addr->bpm, lbl_8047AF00, secIndex);
                section->bpm = section->mTrack.addr->bpm << 10;
            }

            ++section->mTrack.addr;
        }
    }
}

static void SetTickDelta(SEQ_SECTION* section, u32 deltaTime) {
    f32 tickDelta = (f32)section->bpm * (f32)deltaTime * (1.f / 40960000.f);
    tickDelta *= (f32)section->speed * (1.f / 256.f);

    section->tickDelta[section->timeIndex].low =
        (u32)(f32)fmod(tickDelta * 65536.f, 65536.f);
    section->tickDelta[section->timeIndex].high = (s32)(f32)floor(tickDelta);
}

static void RewindMTrack(u8 secIndex, u32 deltaTime) {
    if (lbl_8047AF08->section[secIndex].mTrack.base == NULL) {
        return;
    }
    lbl_8047AF08->section[secIndex].mTrack.addr = lbl_8047AF08->section[secIndex].mTrack.base;
    HandleMasterTrack(secIndex);
    SetTickDelta(lbl_8047AF08->section + secIndex, deltaTime);
}

static u32 HandleTrackEvents(u8 secIndex, u32 deltaTime) {
    SEQ_EVENT* ev;
    u32 loopFlag;
    SEQ_SECTION* section;

    section = &lbl_8047AF08->section[secIndex];
    loopFlag = FALSE;

    while (GetNextEventTime(section) <= section->time[section->timeIndex].high) {
        if ((ev = GetGlobalEvent(section)) == NULL) {
            if (!loopFlag) {
                return FALSE;
            }

            loopFlag = FALSE;
            section->timeIndex ^= 1;
            section->time[section->timeIndex].high = lbl_8047AF08->arrbase->loopPoint[secIndex];
            section->time[section->timeIndex].low = section->time[section->timeIndex ^ 1].low;
            RewindMTrack(secIndex, deltaTime);
            section->loopCnt += 1;
            InitTrackEventsSection(secIndex);
            continue;
        }

        if ((ev = fn_801485FC(ev, secIndex, &loopFlag)) != NULL) {
            InsertGlobalEvent(section, ev);
        }
    }

    return TRUE;
}

static u32 HandleNotes(void) {
    NOTE* note;
    u32 i;

    for (i = 0; i < 2; i++) {
        note = lbl_8047AF08->noteUsed[i];
        if (note != NULL) {
            while (note->endTime <= lbl_8047AF08->section[note->section].time[i].high) {
                synthSendKeyOff(note->id);

                if ((lbl_8047AF08->noteUsed[i] = note->next) != NULL) {
                    lbl_8047AF08->noteUsed[i]->prev = NULL;
                }

                if ((note->next = lbl_8047AF08->noteKeyOff) != NULL) {
                    lbl_8047AF08->noteKeyOff->prev = note;
                }
                lbl_8047AF08->noteKeyOff = note;
                note = lbl_8047AF08->noteUsed[i];

                if (note == NULL) {
                    break;
                }
            }
        }
    }

    return lbl_8047AF08->noteUsed[0] != NULL || lbl_8047AF08->noteUsed[1] != NULL;
}

static void seqFreeKeyOffNote(NOTE* n) {
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }

    if (n->prev != NULL) {
        n->prev->next = n->next;
    } else {
        lbl_8047AF08->noteKeyOff = n->next;
    }

    if ((n->next = lbl_8047AF04) != NULL) {
        lbl_8047AF04->prev = n;
    }

    n->prev = NULL;
    lbl_8047AF04 = n;
}

static void HandleKeyOffNotes(void) {
    NOTE* n;
    NOTE* nn;

    if (!lbl_8047AF08->keyOffCheck) {
        n = lbl_8047AF08->noteKeyOff;
        while (n != NULL) {
            nn = n->next;
            if (n->id != SND_SEQ_ERROR_ID && sndFXCheck(n->id) == SND_SEQ_ERROR_ID) {
                seqFreeKeyOffNote(n);
            }
            n = nn;
        }
    }

    lbl_8047AF08->keyOffCheck = (lbl_8047AF08->keyOffCheck + 1) % 5;
}

void fn_801496A0(u32 deltaTime) {
    u32 i;
    u32 j;
    u32 x;
    u32 eventsActive;
    u32 notesActive;
    SEQ_INSTANCE* si;
    SEQ_INSTANCE* nextSi;

    if (deltaTime == 0) {
        return;
    }

    si = lbl_8047AF14;
    while (si != NULL) {
        nextSi = si->next;
        lbl_8047AF08 = si;
        lbl_8047AF00 = si->index;
        lbl_8047AEFC = synthIsFadeOutActive(si->defVGroup);

        if (lbl_8047AF08->trackSectionTab == NULL) {
            HandleMasterTrack(0);
            SetTickDelta(lbl_8047AF08->section, deltaTime);
            eventsActive = HandleTrackEvents(0, deltaTime);
            notesActive = HandleNotes();
            HandleKeyOffNotes();

            for (i = 0; i < 2; i++) {
                x = lbl_8047AF08->section[0].time[i].low + lbl_8047AF08->section[0].tickDelta[i].low;
                lbl_8047AF08->section[0].time[i].low = x & 0xffff;
                x >>= 16;
                lbl_8047AF08->section[0].time[i].high += x + lbl_8047AF08->section[0].tickDelta[i].high;
            }
        } else {
            eventsActive = 0;
            for (i = 0; i < 16; i++) {
                HandleMasterTrack(i);
                SetTickDelta(&lbl_8047AF08->section[i], deltaTime);
                eventsActive |= HandleTrackEvents(i, deltaTime);
            }
            notesActive = HandleNotes();
            HandleKeyOffNotes();

            for (i = 0; i < 16; i++) {
                for (j = 0; j < 2; j++) {
                    x = lbl_8047AF08->section[i].time[j].low + lbl_8047AF08->section[i].tickDelta[j].low;
                    lbl_8047AF08->section[i].time[j].low = x & 0xffff;
                    x >>= 16;
                    lbl_8047AF08->section[i].time[j].high +=
                        x + lbl_8047AF08->section[i].tickDelta[j].high;
                }
            }
        }

        if (eventsActive == 0 && notesActive == 0) {
            if (si->prev != NULL) {
                si->prev->next = nextSi;
            } else {
                lbl_8047AF14 = nextSi;
            }
            if (nextSi != NULL) {
                nextSi->prev = si->prev;
            }
            ResetNotes(si);
            si->state = 0;
            si->prev = NULL;
            if ((si->next = lbl_8047AF0C) != NULL) {
                lbl_8047AF0C->prev = si;
            }
            lbl_8047AF0C = si;
        }
        si = nextSi;
    }
}

static void ClearNotes(void) {
    NOTE* ln;
    s32 i;

    ln = NULL;
    lbl_8047AF04 = &lbl_804271D0[0];
    for (i = 0; i < 256; i++) {
        lbl_804271D0[i].prev = ln;
        if (ln != NULL) {
            ln->next = &lbl_804271D0[i];
        }
        ln = &lbl_804271D0[i];
    }

    ln->next = NULL;
}

static void InitPublicIds(void) { lbl_8047AEF8 = 0; }

void seqInit(void) {
    u32 i;
    u32 j;

    lbl_8047AF14 = NULL;
    lbl_8047AF10 = NULL;

    for (i = 0; i < 8; i++) {
        if (i == 0) {
            lbl_8047AF0C = &lbl_804285D0[i];
            lbl_804285D0[i].prev = NULL;
        } else {
            lbl_804285D0[i - 1].next = &lbl_804285D0[i];
            lbl_804285D0[i].prev = &lbl_804285D0[i - 1];
        }
        lbl_804285D0[i].index = i;
        lbl_804285D0[i].state = 0;

        for (j = 0; j < 0x10; j++) {
            lbl_80434910[i][j] = 0xffff;
        }
    }
    lbl_804285D0[i - 1].next = NULL;

    ClearNotes();
    InitPublicIds();
}
#endif
