/**
 * @file seq.c
 * @brief MusyX 2.0.0 sequencer, 0x8014635C - 0x8014A23C.
 *
 * Reunites the existing matched sequencer implementations and their storage.
 * Reference: AxioDL/musyx, commit
 * 89850639d731ec080c12cb0bc8d7ab2ea3880c3a, src/musyx/runtime/seq.c.
 * The Dolphin 2.0.0 path has a four-byte PRG_STATE, two note/time banks,
 * and no later program field or group/program argument to synthStartSound.
 * Existing Count-Sheep and upstream integration contributions are retained;
 * see src/musyx/LICENSE.AxioDL-musyx for the imported-source notice.
 *
 * Native ownership:
 *   .text   0x8014635C - 0x8014A23C  sixteen retained functions
 *   .data   0x80368A30 - 0x80368A80  HandleEvent's compiler jump table
 *   .bss    0x804271D0 - 0x80434A10  notes, instances, MIDI priorities
 *   .sbss   0x8047AEF8 - 0x8047AF18  IDs and sequencer/list state
 *   .sdata2 0x8047D350 - 0x8047D370  compiler-generated tick-time literals
 *
 * The existing inline helper decisions and floating-point conversions are
 * retained pending whole-object verification with GC/1.3.2, -O4,p,
 * -inline auto, -use_lmw_stmw off, -sdata 8 and -sdata2 8.
 */

#include "dolphin/types.h"

typedef struct NOTE {
    struct NOTE* next;                    // 0x0
    struct NOTE* prev;                    // 0x4
    u32 id;                               // 0x8
    s32 endTime;                          // 0xC
    u8 section;                           // 0x10
    u8 timeIndex;                         // 0x11
    u8 reserved[2];                       // 0x12
} NOTE; // size 0x14

typedef struct {
    u32 seqId1;                           // 0x0
    u16 time1;                            // 0x4
    u16 pad_6;                            // 0x6
    u32 seqId2;                           // 0x8
    u16 time2;                            // 0xC
    u16 pad_E;                            // 0xE
    void* arr2;                           // 0x10
    u16 gid2;                             // 0x14
    u16 sid2;                             // 0x16
    u8 vol2;                              // 0x18
    u8 studio2;                           // 0x19
    u16 pad_1A;                           // 0x1A
    u32 trackMute2[2];                    // 0x1C
    u16 speed2;                           // 0x24
    u8 flags;                             // 0x26
    u8 pad_27;                            // 0x27
} SND_CROSSFADE; // size 0x28

typedef struct {
    volatile u32 time;                    // 0x0
    u32 bpm;                              // 0x4
} MTRACK_DATA;           // size 0x8

typedef struct {
    MTRACK_DATA* base;                    // 0x0
    MTRACK_DATA* addr;                    // 0x4
} MTRACK;               // size 0x8

typedef struct {
    u32 low;                              // 0x0
    s32 high;                             // 0x4
} TICKS;      // size 0x8

typedef struct {
    u32 time;                             // 0x0
    u8 prgChange;                         // 0x4
    u8 velocity;                          // 0x5
    u8 res[2];                            // 0x6
    u16 pattern;                          // 0x8
    s8 transpose;                         // 0xA
    s8 velocityAdd;                       // 0xB
} TENTRY; // size 0xC

typedef struct {
    TENTRY* base;                         // 0x0
    TENTRY* addr;                         // 0x4
} TRACK;          // size 0x8

typedef struct {
    u16 time;                             // 0x0
    u8 key;                               // 0x2
    u8 velocity;                          // 0x3
    u16 length;                           // 0x4
} NOTE_DATA;          // size 0x6

typedef struct {
    u8* nextAddr;                         // 0x0
    u16 value;                            // 0x4
    s16 nextDelta;                        // 0x6
    u32 nextTime;                         // 0x8
} SEQ_STREAM;            // size 0xC

typedef struct {
    u32 lTime;                            // 0x0
    u32 baseTime;                         // 0x4
    NOTE_DATA* addr;                      // 0x8
    TENTRY* patternInfo;                  // 0xC
    SEQ_STREAM pitchBend;                 // 0x10
    SEQ_STREAM modulation;                // 0x1C
    u8 midi;                              // 0x28
    u8 pad_29[3];                         // 0x29
} CPAT;                                 // size 0x2C

typedef struct SEQ_EVENT {
    struct SEQ_EVENT* next;               // 0x0
    struct SEQ_EVENT* prev;               // 0x4
    u32 time;                             // 0x8
    union {
        TENTRY* trackAddr;
        struct {
            NOTE_DATA* addr;              // 0x0
            CPAT* base;                   // 0x4
        } pattern;
    } info;                               // 0xC
    u8 type;                              // 0x14
    u8 trackId;                           // 0x15
    u8 pad_16[2];                         // 0x16
} SEQ_EVENT;                   // size 0x18

typedef struct {
    u16 macId;                            // 0x0
    u8 priority;                          // 0x2
    u8 maxVoices;                         // 0x3
} PRG_STATE;          // size 0x4 (MusyX 2.0.0; no later program field)

typedef struct {
    u16 macro;                            // 0x0
    u8 prio;                              // 0x2
    u8 maxVoices;                         // 0x3
    u8 index;                             // 0x4
    u8 reserved;                          // 0x5
} PAGE;                // size 0x6

typedef struct {
    u32 tTab;                             // 0x0
    u32 pTab;                             // 0x4
    u32 tmTab;                            // 0x8
    u32 mTrack;                           // 0xC
    u32 info;                             // 0x10
    u32 loopPoint[16];                    // 0x14
    u32 tsTab;                            // 0x54
} ARR;                        // size 0x58

#define ARR_GET(arr, offset) ((void*)((offset) + (u32)(arr)))
#define ARR_GET_TYPE(arr, offset, ty) ((ty)ARR_GET(arr, offset))

typedef struct {
    MTRACK mTrack;                        // 0x0
    u32 bpm;                              // 0x8
    TICKS tickDelta[2];                   // 0xC
    SEQ_EVENT* globalEventRoot;           // 0x1C
    TICKS time[2];                        // 0x20
    u8 timeIndex;                         // 0x30
    u8 pad_31;                            // 0x31
    u16 speed;                            // 0x32
    u16 loopCnt;                          // 0x34
    u8 loopDisable;                       // 0x36
    u8 pad_37;                            // 0x37
} SEQ_SECTION; // size 0x38

typedef struct SEQ_INSTANCE {
    struct SEQ_INSTANCE* next;            // 0x0
    struct SEQ_INSTANCE* prev;            // 0x4
    u8 state;                             // 0x8
    u8 index;                             // 0x9
    u16 groupID;                          // 0xA
    u32 publicId;                         // 0xC
    PAGE* normtab;                        // 0x10
    u8 normTrans[128];                    // 0x14
    PAGE* drumtab;                        // 0x94
    u8 drumTrans[128];                    // 0x98
    ARR* arrbase;                         // 0x118
    u32 trackMute[2];                     // 0x11C
    TRACK track[64];                      // 0x124
    u8 trackVolGroup[64];                 // 0x324
    CPAT pattern[64];                     // 0x364
    NOTE* noteUsed[2];                    // 0xE64
    NOTE* noteKeyOff;                     // 0xE6C
    PRG_STATE prgState[16];               // 0xE70
    u8 defVGroup;                         // 0xEB0
    u8 pad_EB1[3];                        // 0xEB1
    SND_CROSSFADE syncCrossInfo;          // 0xEB4
    u32* syncSeqIdPtr;                    // 0xEDC
    u8 syncActive;                        // 0xEE0
    u8 defStudio;                         // 0xEE1
    u8 keyOffCheck;                       // 0xEE2
    u8 pad_EE3;                           // 0xEE3
    SEQ_EVENT event[64];                  // 0xEE4
    u8* trackSectionTab;                  // 0x14E4
    SEQ_SECTION section[16];              // 0x14E8
} SEQ_INSTANCE; // size 0x1868

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

#define SND_CROSSFADE_STOP 0
#define SND_CROSSFADE_PAUSE 1
#define SND_CROSSFADE_CONTINUE 2

typedef struct {
    u32 headerLen;
    u32 pitchBend;
    u32 modulation;
    u32 noteData;
} SEQ_PATTERN;

/* Zero-initialized storage belongs to this sequencer. The early helper
 * definitions retain the upstream first-reference order of these arrays. */
static NOTE seqNote[256];
SEQ_INSTANCE lbl_804285D0[8];        /* seqInstance */
u16 lbl_80434910[8][16];             /* seqMIDIPriority */

static u32 seq_next_id = 0;
static u8 curFadeOutState = 0;
static u32 curSeqId = 0;
static NOTE* noteFree = NULL;
static SEQ_INSTANCE* cseq = NULL;
SEQ_INSTANCE* lbl_8047AF0C = NULL;   /* seqFreeRoot */
SEQ_INSTANCE* lbl_8047AF10 = NULL;   /* seqPausedRoot */
SEQ_INSTANCE* lbl_8047AF14 = NULL;   /* seqActiveRoot */

extern void synthSetBpm(u32 bpm, u8 seqId, u8 secIndex);
extern f64 fmod(f64 a, f64 b);
extern f64 floor(f64 x);
extern u32 synthSendKeyOff(u32 id);
extern u32 synthIsFadeOutActive(u8 volGroup);
extern u32 sndFXCheck(u32 id);
extern void voiceKillSound(u32 id);
extern void synthVolume(u8 volume, u16 time, u8 volGroup, u8 mode, u32 pubId);
extern u8 lbl_80435464[64];          /* synthTrackVolume */
extern void synthSetMusicVolumeType(u8 volumeGroup, u8 type);
extern void inpSetMidiCtrl(u8 ctrl, u8 midi, u8 midiSet, u8 value);
extern void inpSetMidiCtrl14(u8 ctrl, u8 midi, u8 midiSet, u16 value);
extern void inpResetMidiCtrl(u8 midi, u8 midiSet, u32 coldReset);
extern void inpResetChannelDefaults(u8 midi, u8 midiSet);
extern u32 synthStartSound(u16 id, u8 priority, u8 maxVoices, u8 key,
                          u8 volume, u8 panning, u8 midi, u8 midiSet,
                          u8 section, u16 step, u16 trackId, u8 volumeGroup,
                          s16 priorityOffset, u8 studio, u32 itd);
extern u8 lbl_804356A4[16];          /* synthITDDefault: { music, sfx } */
extern void sndSeqVolume(u8 volume, u16 time, u32 seqId, u8 mode);
extern void sndSeqMute(u32 seqId, u32 mask1, u32 mask2);
extern void sndSeqSpeed(u32 seqId, u16 speed);
extern void sndSeqContinue(u32 seqId);
extern u32 seqPlaySong(u16 sgid, u16 sid, void* arrfile, SND_PLAYPARA* para,
                       u32 irqCall, u8 studio);
extern u32 sndSeqPlayEx(u16 sgid, u16 sid, void* arrfile, SND_PLAYPARA* para,
                       u8 studio);

static void StartPause(SEQ_INSTANCE* si);
static void InitTrackEvents(void);

static void ClearNotes(void) {
    NOTE* ln;
    s32 i;

    ln = NULL;
    noteFree = &seqNote[0];
    for (i = 0; i < 256; i++) {
        seqNote[i].prev = ln;
        if (ln != NULL) {
            ln->next = &seqNote[i];
        }
        ln = &seqNote[i];
    }

    ln->next = NULL;
}

static inline u32 GetPublicId(u32 seqId)
{
    u32 publicId;
    SEQ_INSTANCE* seq;

    do {
        publicId = seq_next_id++;
        seq_next_id &= ~SND_SEQ_CROSSFADE_ID;
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

static inline void DoPrgChange(SEQ_INSTANCE* seq, u8 program, u8 midi)
{
    lbl_80434910[curSeqId][midi] = 0xFFFF;
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

            if (noteFree != NULL) {
                n->next = noteFree;
                noteFree->prev = n;
            }

            noteFree = seq->noteUsed[i];
            seq->noteUsed[i] = NULL;
        }
    }

    n = seq->noteKeyOff;
    if (n != NULL) {
        for (; n->next != NULL; n = n->next) {
        }

        if (noteFree != NULL) {
            n->next = noteFree;
            noteFree->prev = n;
        }

        noteFree = seq->noteKeyOff;
        seq->noteKeyOff = NULL;
    }
}

u32 seqStartPlay(PAGE* normal, PAGE* drum, MIDISETUP* midiSetup, u32* song,
                SND_PLAYPARA* para, u8 studio, u16 groupId)
{
    ARR* arr;
    u32* trackTable;
    s32 i;
    SEQ_INSTANCE* seq;
    SEQ_INSTANCE* oldSeq;
    u32 seqId;
    u32 bpm;

    if ((seq = lbl_8047AF0C) == NULL) {
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
        inpResetChannelDefaults(i, seqId);
    }
    if (midiSetup != NULL) {
        for (i = 0; i < 16; i++) {
            DoPrgChange(seq, midiSetup->channel[i].program, i);
            inpSetMidiCtrl(7, i, seqId, midiSetup->channel[i].volume);
            inpSetMidiCtrl(10, i, seqId, midiSetup->channel[i].panning);
            inpSetMidiCtrl(91, i, seqId, midiSetup->channel[i].reverb);
            inpSetMidiCtrl(93, i, seqId, midiSetup->channel[i].chorus);
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
    oldSeq = cseq;
    cseq = seq;
    InitTrackEvents();
    cseq = oldSeq;
    seqId = GetPublicId(seqId);
    return seqId;
}

static void StartPause(SEQ_INSTANCE* si) {
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
                sndSeqContinue(ci->seqId2);
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
        pp.volume.time = ci->time2;
        pp.volume.target = ci->vol2;
        pp.numFaded = 0;
        if (irq_call != 0) {
            if ((*new_seqId = seqPlaySong(ci->gid2, ci->sid2, ci->arr2, &pp, TRUE, ci->studio2)) !=
                    SND_SEQ_ERROR_ID &&
                (ci->flags & SND_CROSSFADE_MUTENEW) != 0) {
                seqMute(*new_seqId, 0, 0);
            }
        } else {
            if ((*new_seqId = sndSeqPlayEx(ci->gid2, ci->sid2, ci->arr2, &pp, ci->studio2)) !=
                    SND_SEQ_ERROR_ID &&
                (ci->flags & SND_CROSSFADE_MUTENEW) != 0) {
                sndSeqMute(*new_seqId, 0, 0);
            }
        }
    }
}

static SEQ_EVENT* GenerateNextTrackEvent(u8 trackId) {
    TRACK* track;
    CPAT* pattern;
    SEQ_EVENT* ev;
    u32 patternTime;
    u32 pitchTime;
    u32 modTime;

    track = &cseq->track[trackId];
    pattern = &cseq->pattern[trackId];

    if (track->addr != NULL) {
        ev = &cseq->event[trackId];
        ev->trackId = trackId;
        ev->info.pattern.base = pattern;

        if (pattern->addr == NULL) {
        null_pattern_addr:
            if (track->addr->pattern == 0xffff) {
                track->addr = NULL;
                return NULL;
            }

            if (track->addr->pattern == 0xfffe) {
                if (cseq->trackSectionTab == NULL) {
                    if (cseq->section[0].loopDisable) {
                        track->addr = NULL;
                        return NULL;
                    }
                } else if (cseq->section[cseq->trackSectionTab[trackId]].loopDisable) {
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

static inline NOTE* AllocateNote(u32 endTime, u8 section)
{
    NOTE* n;
    NOTE* nl;
    NOTE* last;

    n = noteFree;
    if (n != NULL) {
        if ((noteFree = n->next) != NULL) {
            noteFree->prev = NULL;
        }
        n->endTime = endTime;
        n->section = section;
        n->timeIndex = cseq->section[section].timeIndex;
        last = NULL;
        for (nl = cseq->noteUsed[n->timeIndex]; nl != NULL;
             last = nl, nl = nl->next)
        {
            if (nl->endTime > n->endTime) {
                n->next = nl;
                n->prev = last;
                if (last != NULL) {
                    last->next = n;
                } else {
                    cseq->noteUsed[n->timeIndex] = n;
                }
                nl->prev = n;
                return n;
            }
        }
        n->prev = last;
        if (last != NULL) {
            last->next = n;
        } else {
            cseq->noteUsed[n->timeIndex] = n;
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
        cseq->noteUsed[n->timeIndex] = n->next;
    }
    if ((n->next = noteFree) != NULL) {
        noteFree->prev = n;
    }
    n->prev = NULL;
    noteFree = n;
}

static inline void KeyOffNotes(void)
{
    NOTE* note;
    NOTE* next;
    u32 i;

    for (i = 0; i < 2; i++) {
        note = cseq->noteUsed[i];
        while (note != NULL) {
            next = note->next;
            synthSendKeyOff(note->id);
            if ((cseq->noteUsed[i] = note->next) != NULL) {
                cseq->noteUsed[i]->prev = NULL;
            }
            if ((note->next = cseq->noteKeyOff) != NULL) {
                cseq->noteKeyOff->prev = note;
            }
            cseq->noteKeyOff = note;
            note = next;
        }
    }
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
        if ((stream->nextAddr =
                 GetStreamValue(ARR_GET(cseq->arrbase, offset), &delta,
                                &stream->nextDelta)) != NULL) {
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
        if ((stream->nextAddr = GetStreamValue(stream->nextAddr, &delta,
                                               &stream->nextDelta)) != NULL) {
            stream->nextTime += delta;
        } else {
            stream->nextTime = 0x7FFFFFFF;
        }
    } else {
        stream->nextTime = 0x7FFFFFFF;
    }
    return stream->value;
}

static SEQ_EVENT* HandleEvent(SEQ_EVENT* event, u8 secIndex, u32* loopFlag)
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
        pattern = &cseq->pattern[event->trackId];
        patternTable =
            ARR_GET(cseq->arrbase, cseq->arrbase->pTab);
        patternData =
            ARR_GET(cseq->arrbase, patternTable[trackEntry->pattern]);
        pattern->addr = (NOTE_DATA*)&patternData->noteData;
        pattern->lTime = 0;
        pattern->baseTime = trackEntry->time;
        pattern->patternInfo = trackEntry;
        InitStream(&pattern->pitchBend, patternData->pitchBend);
        pattern->pitchBend.value = 0x2000;
        InitStream(&pattern->modulation, patternData->modulation);
        pattern->modulation.value = 0;
        pattern->midi =
            ARR_GET_TYPE(cseq->arrbase, cseq->arrbase->tmTab,
                         u8*)[event->trackId];
        if (trackEntry->prgChange != 0xFF) {
            DoPrgChange(cseq, trackEntry->prgChange, pattern->midi);
        }
        if (trackEntry->velocity != 0xFF) {
            inpSetMidiCtrl(7, pattern->midi, curSeqId,
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
                DoPrgChange(cseq, key & 0x7F, midi);
                break;
            case 1:
                inpSetMidiCtrl(0x82, midi, curSeqId, key & 0x7F);
                break;
            default:
                if ((velocity & 0x80) != 0x80) {
                    break;
                }
                switch (velocity & 0x7F) {
                case 0x68:
                    if (cseq->syncActive) {
                        seqCrossFade(&cseq->syncCrossInfo,
                                     cseq->syncSeqIdPtr, 1);
                        cseq->syncActive = 0;
                    }
                    break;
                case 0x69:
                    lbl_80434910[curSeqId][midi] = key & 0x7F;
                    break;
                case 0x6A:
                    lbl_80434910[curSeqId][midi] =
                        (key & 0x7F) + 0x80;
                    break;
                case 0x79:
                    inpResetMidiCtrl(midi, curSeqId, 0);
                    break;
                case 0x7B:
                    KeyOffNotes();
                    break;
                default:
                    inpSetMidiCtrl(velocity & 0x7F, midi, curSeqId,
                                key & 0x7F);
                    break;
                }
            }
            break;
        }
        if ((cseq->trackMute[event->trackId / 32] &
             (1 << (event->trackId & 0x1F))) != 0)
        {
            macId = cseq->prgState[midi].macId;
            if (macId != 0xFFFF) {
                key += pa->patternInfo->transpose;
                key = key > 0x7F ? 0x7F : (key < 0 ? 0 : key);
                velocity += pa->patternInfo->velocityAdd;
                velocity = velocity > 0x7F
                               ? 0x7F
                               : (velocity < 0 ? 0 : velocity);
                note = AllocateNote(event->time + pe->length, secIndex);
                if (note != NULL) {
                    if ((note->id = synthStartSound(
                             macId, cseq->prgState[midi].priority,
                             cseq->prgState[midi].maxVoices, key,
                             velocity, 64, midi, curSeqId, secIndex, 0,
                             event->trackId,
                             cseq->trackVolGroup[event->trackId],
                             curFadeOutState ? -1 : 0, cseq->defStudio,
                             lbl_804356A4[cseq->defStudio * 2])) ==
                        SND_SEQ_ERROR_ID) {
                        FreeNote(note);
                    }
                }
            }
        }
        break;

    case 2:
        pa = event->info.pattern.base;
        inpSetMidiCtrl14(0x80, pa->midi, curSeqId,
                         HandleStream(&pa->pitchBend));
        break;
    case 1:
        pa = event->info.pattern.base;
        inpSetMidiCtrl14(1, pa->midi, curSeqId,
                         HandleStream(&pa->modulation));
        break;
    case 3:
        *loopFlag |= 1;
        return NULL;
    }
    return GenerateNextTrackEvent(event->trackId);
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

static void InitTrackEvents(void) {
    u32 i;
    SEQ_EVENT* ev;

    if (cseq->trackSectionTab == NULL) {
        for (i = 0; i < 0x40; i += 1) {
            if ((ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(cseq->section, ev);
            }
        }
    } else {
        for (i = 0; i < 0x40; i += 1) {
            if ((ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(cseq->section + cseq->trackSectionTab[i], ev);
            }
        }
    }
}

static void InitTrackEventsSection(u8 secIndex) {
    u32 i;
    SEQ_EVENT* ev;

    if (cseq->trackSectionTab == NULL) {
        for (i = 0; i < 64; i += 1) {
            if ((ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(cseq->section, ev);
            }
        }
    } else {
        for (i = 0; i < 64; i += 1) {
            if (secIndex == cseq->trackSectionTab[i] &&
                (ev = GenerateNextTrackEvent(i)) != NULL) {
                InsertGlobalEvent(cseq->section + secIndex, ev);
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

    section = &cseq->section[secIndex];
    if (section->mTrack.base != NULL) {
        while (section->mTrack.addr->time != -1) {
            if (section->mTrack.addr->time > section->time[section->timeIndex].high) {
                break;
            }

            if ((cseq->arrbase->info & 0x40000000) != 0) {
                synthSetBpm((section->bpm = section->mTrack.addr->bpm) >> 10, curSeqId,
                            secIndex);
            } else {
                synthSetBpm(section->mTrack.addr->bpm, curSeqId, secIndex);
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
    if (cseq->section[secIndex].mTrack.base == NULL) {
        return;
    }
    cseq->section[secIndex].mTrack.addr = cseq->section[secIndex].mTrack.base;
    HandleMasterTrack(secIndex);
    SetTickDelta(cseq->section + secIndex, deltaTime);
}

static u32 HandleTrackEvents(u8 secIndex, u32 deltaTime) {
    SEQ_EVENT* ev;
    u32 loopFlag;
    SEQ_SECTION* section;

    section = &cseq->section[secIndex];
    loopFlag = FALSE;

    while (GetNextEventTime(section) <= section->time[section->timeIndex].high) {
        if ((ev = GetGlobalEvent(section)) == NULL) {
            if (!loopFlag) {
                return FALSE;
            }

            loopFlag = FALSE;
            section->timeIndex ^= 1;
            section->time[section->timeIndex].high = cseq->arrbase->loopPoint[secIndex];
            section->time[section->timeIndex].low = section->time[section->timeIndex ^ 1].low;
            RewindMTrack(secIndex, deltaTime);
            section->loopCnt += 1;
            InitTrackEventsSection(secIndex);
            continue;
        }

        if ((ev = HandleEvent(ev, secIndex, &loopFlag)) != NULL) {
            InsertGlobalEvent(section, ev);
        }
    }

    return TRUE;
}

static u32 HandleNotes(void) {
    NOTE* note;
    u32 i;

    for (i = 0; i < 2; i++) {
        note = cseq->noteUsed[i];
        if (note != NULL) {
            while (note->endTime <= cseq->section[note->section].time[i].high) {
                synthSendKeyOff(note->id);

                if ((cseq->noteUsed[i] = note->next) != NULL) {
                    cseq->noteUsed[i]->prev = NULL;
                }

                if ((note->next = cseq->noteKeyOff) != NULL) {
                    cseq->noteKeyOff->prev = note;
                }
                cseq->noteKeyOff = note;
                note = cseq->noteUsed[i];

                if (note == NULL) {
                    break;
                }
            }
        }
    }

    return cseq->noteUsed[0] != NULL || cseq->noteUsed[1] != NULL;
}

static void seqFreeKeyOffNote(NOTE* n) {
    if (n->next != NULL) {
        n->next->prev = n->prev;
    }

    if (n->prev != NULL) {
        n->prev->next = n->next;
    } else {
        cseq->noteKeyOff = n->next;
    }

    if ((n->next = noteFree) != NULL) {
        noteFree->prev = n;
    }

    n->prev = NULL;
    noteFree = n;
}

static void HandleKeyOffNotes(void) {
    NOTE* n;
    NOTE* nn;

    if (!cseq->keyOffCheck) {
        n = cseq->noteKeyOff;
        while (n != NULL) {
            nn = n->next;
            if (n->id != SND_SEQ_ERROR_ID && sndFXCheck(n->id) == SND_SEQ_ERROR_ID) {
                seqFreeKeyOffNote(n);
            }
            n = nn;
        }
    }

    cseq->keyOffCheck = (cseq->keyOffCheck + 1) % 5;
}

void seqHandle(u32 deltaTime) {
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
        cseq = si;
        curSeqId = si->index;
        curFadeOutState = synthIsFadeOutActive(si->defVGroup);

        if (cseq->trackSectionTab == NULL) {
            HandleMasterTrack(0);
            SetTickDelta(cseq->section, deltaTime);
            eventsActive = HandleTrackEvents(0, deltaTime);
            notesActive = HandleNotes();
            HandleKeyOffNotes();

            for (i = 0; i < 2; i++) {
                x = cseq->section[0].time[i].low + cseq->section[0].tickDelta[i].low;
                cseq->section[0].time[i].low = x & 0xffff;
                x >>= 16;
                cseq->section[0].time[i].high += x + cseq->section[0].tickDelta[i].high;
            }
        } else {
            eventsActive = 0;
            for (i = 0; i < 16; i++) {
                HandleMasterTrack(i);
                SetTickDelta(&cseq->section[i], deltaTime);
                eventsActive |= HandleTrackEvents(i, deltaTime);
            }
            notesActive = HandleNotes();
            HandleKeyOffNotes();

            for (i = 0; i < 16; i++) {
                for (j = 0; j < 2; j++) {
                    x = cseq->section[i].time[j].low + cseq->section[i].tickDelta[j].low;
                    cseq->section[i].time[j].low = x & 0xffff;
                    x >>= 16;
                    cseq->section[i].time[j].high +=
                        x + cseq->section[i].tickDelta[j].high;
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

static void InitPublicIds(void) { seq_next_id = 0; }

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
