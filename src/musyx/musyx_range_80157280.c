/**
 * @file musyx_range_80157280.c
 * @brief MusyX runtime residual range, 0x80157280 - 0x801652DC.
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). Contains the
 * remaining MusyX runtime TUs whose internal boundaries are not yet derived
 * from transcription slices: synthvoice (vidInit @0x80157280 is reference
 * synthvoice.c's first function, immediately after macInit ends
 * synthmacros.c), synth_adsr, synth_vsamples, hw_dspctrl/sal*, snd_midictrl
 * (inp*), snd_service, hardware/hw*, hw_aramdma (aram*), hw_dolphin,
 * hw_memory and StdReverb (ReverbHI*). To be split by later slices; do not
 * guess boundaries here.
 */

#include "dolphin/types.h"
#include "game/people/people.h"
#include "musyx/synthdata.h"

/* ===== snd_synthapi.c: sndPitchUpOne / sndGetPitch, 0x80158BB4 / 0x80158BF0 =====
 * sndGetPitch: sampleInfo word = (rootKey << 24) | sampleRate(24bit);
 * 0xFFFFFFFF means "default" 0x40005622 (root key 0x40, 22050Hz decimal
 * 0x5622=22050 with rootKey 0x40). Pitch tables: lbl_80368EC8 =
 * up-factors[semitones], lbl_803690C8 = down-factors[semitones] (f32[]).
 * Result is a 4.12 fixed-point resampling ratio against the synth mix
 * frequency (first u32 of the synthInfo block at lbl_80434C50).
 * PLACEMENT: these two live at the TOP of the TU because sndGetPitch needs
 * a scalar `extern u32 lbl_80434C50` view (retail folds the mixFrq load:
 * `lwz r0, lbl_80434C50@l(r3)` -- only object-typed symbols fold) which
 * MWCC rejects after the file-scope `extern u8 lbl_80434C50[]` below.
 * lbl_8047D3F0/lbl_8047D400 are the retail-named .sdata2 float constants
 * (2^(1/12) and 4096.f) -- referenced by name so relocs match. */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#pragma peephole off
#pragma push
#pragma peephole on
u32 sndPitchUpOne(u16 pitch) {
    extern f32 lbl_8047D3F0;
    return (s32)(lbl_8047D3F0 * (f32)(u32)pitch);
}
#pragma pop

#pragma push
#pragma peephole on
u32 sndGetPitch(u8 key, u32 sInfo) {
    extern f32 lbl_80368EC8[];
    extern f32 lbl_803690C8[];
    extern f32 lbl_8047D400;
    /* scalar-object view of the synthInfo block: must be an OBJECT (not a
     * cast pointer) so the mixFrq load folds to `lwz r0, @l(r3)`, and must
     * be >8 bytes so MWCC does not assume .sdata/sda21 for the extern. */
    extern struct SynthInfoView_ { u32 mixFrq; u32 numSamples; u32 pad[6]; } lbl_80434C50;
    u32 okey;
    f32 frq;
    f32 fourk;

    if (sInfo == 0xFFFFFFFF) {
        sInfo = 0x40005622;
    }
    okey = sInfo >> 24;
    fourk = lbl_8047D400;
    if (key != okey) {
        frq = (f32)(sInfo & 0xFFFFFF) *
              (okey < key ? lbl_80368EC8[key - okey]
                          : lbl_803690C8[okey - key]);
    } else {
        frq = sInfo & 0xFFFFFF;
    }
    return (fourk * frq) / lbl_80434C50.mixFrq;
}
#pragma pop
#pragma pop


/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void  DCFlushRange(void* ptr, u32 size);
extern u32   OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(u32 level);

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void ARQPostRequest();
void InitStreamBuffers(void);
extern void aramQueueCallback();
extern void aramUploadData();
extern u16 inpGetMidiCtrl(u8 ctrl, u8 channel, u8 set); /* true signature, verified via disassembly */
extern void salCallback();
extern u8 jumptable_80369CB0[];
extern u8 jumptable_80369CD4[];
extern u8 jumptable_80369CF8[];
extern volatile const u16 lbl_80273448[];
extern u8 lbl_8036944C[];
extern u8 lbl_8036BF00[];
extern u8 lbl_80434C50[];
extern const f32 lbl_8047D4D0[2];
extern volatile const f32 lbl_8047D4D8;
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
typedef struct ReverbDelayLine {
    u32 writeOffset;
    u32 readOffset;
    u32 endOffset;
    f32* samples;
    f32 state;
} ReverbDelayLine;

typedef struct ReverbWork {
    ReverbDelayLine allpass[9];
    ReverbDelayLine comb[9];
    f32 feedback;
    f32 combGain[9];
    f32 previous[3];
    f32 wet;
    f32 dampingMix;
    s32 preDelayLength;
    u32 _1A8;
    f32* preDelayBuffer[3];
    f32* preDelayCursor[3];
} ReverbWork;

static inline void ReverbAdvanceLine(ReverbDelayLine* line)
{
    line->writeOffset += 4;
    if (line->writeOffset == line->endOffset) {
        line->writeOffset = 0;
    }
    line->readOffset += 4;
    if (line->readOffset == line->endOffset) {
        line->readOffset = 0;
    }
}

static inline f32 ReverbCombSample(ReverbDelayLine* line, f32 input,
                                   f32 gain)
{
    f32 value = gain * line->state + input;
    *(f32*)((u8*)line->samples + line->writeOffset) = value;
    line->state = *(f32*)((u8*)line->samples + line->readOffset);
    ReverbAdvanceLine(line);
    return line->state;
}

static inline f32 ReverbAllpassSample(ReverbDelayLine* line, f32 input,
                                      f32 feedback)
{
    f32 value = feedback * line->state + input;
    f32 previous;

    *(f32*)((u8*)line->samples + line->writeOffset) = value;
    previous = *(f32*)((u8*)line->samples + line->readOffset);
    line->state = previous;
    ReverbAdvanceLine(line);
    return previous - feedback * value;
}

/* Process one 160-sample block through the standard high-quality reverb. */
void HandleReverb(s32* samples, ReverbWork* work, u32 channel)
{
    ReverbDelayLine* comb;
    ReverbDelayLine* allpass;
    f32* preDelay;
    f32* preDelayCursor;
    f32 input;
    f32 delayed;
    f32 mixed;
    f32 filtered;
    f32 wet;
    f32 dry;
    u32 stageBase;
    u32 i;

    stageBase = channel * 3;
    comb = &work->comb[stageBase];
    allpass = &work->allpass[stageBase];
    wet = work->wet * lbl_8047D530;
    dry = lbl_8047D530 - wet;
    preDelay = work->preDelayBuffer[channel];
    preDelayCursor = work->preDelayCursor[channel];
    filtered = work->previous[channel];

    for (i = 0; i < 160; i++) {
        input = (f32)samples[i];
        delayed = input;
        if (work->preDelayLength != 0) {
            f32* end = preDelay + work->preDelayLength - 1;
            delayed = *preDelayCursor;
            *preDelayCursor = input;
            preDelayCursor++;
            if (preDelayCursor == end) {
                preDelayCursor = preDelay;
            }
        }

        mixed = ReverbCombSample(&comb[0], delayed,
                                 work->combGain[stageBase]);
        mixed += ReverbCombSample(&comb[1], delayed,
                                  work->combGain[stageBase + 1]);
        mixed += ReverbCombSample(&comb[2], delayed,
                                  work->combGain[stageBase + 2]);

        mixed = ReverbAllpassSample(&allpass[0], mixed, work->feedback);
        mixed = ReverbAllpassSample(&allpass[1], mixed, work->feedback);
        mixed = ReverbAllpassSample(&allpass[2], mixed, work->feedback);
        filtered = mixed * lbl_8047D534 +
                   work->dampingMix * filtered;
        work->previous[channel] = filtered;
        samples[i] = (s32)(wet * filtered + dry * input);
    }

    work->preDelayCursor[channel] = preDelayCursor;
}

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

/* shared with synthdata.c (sndBSearch comparator signature) */
typedef s32 (*PeopleCmpFn)(u8* a, u8* b);

extern f64 fmod(f64 x, f64 y);
extern f32 lbl_8036984C[];
extern f32 lbl_80369A68[];
extern f32 lbl_8047D430;
extern f32 lbl_8047D434;
extern f32 lbl_8047D438;
extern f32 lbl_8047D448;
extern f64 lbl_8047D450;
extern f32 lbl_8047D458;
extern f32 lbl_8047D45C;
extern f32 lbl_8047D460[];

/* MusyX SAL volume and pan matrix builder. */
extern f64 fmod(f64 value, f64 modulus);

#define SAL_FRAC(value) ((f32)fmod((value), lbl_8047D450))
#define SAL_INTERP(table, index, fraction)                                   \
    (((lbl_8047D434 - (fraction)) * (table)[index]) +                        \
     ((fraction) * (table)[(index) + 1]))

void salCalcVolume(u32 volumeArg, f32* out, f32 inputA, u32 pan,
                   u32 surroundPan, f32 inputB, f32 inputC, u32 narrowPan,
                   u32 studioMode)
{
    f32* volumeTable;
    f32* panTable;
    f32 panValue;
    f32 surroundValue;
    f32 reversePan;
    f32 reverseSurround;
    f32 panFraction;
    f32 surroundFraction;
    f32 reversePanFraction;
    f32 reverseSurroundFraction;
    f32 originalPanFraction;
    f32 originalReverseFraction;
    u32 panIndex;
    u32 surroundIndex;
    u32 reversePanIndex;
    u32 reverseSurroundIndex;
    u32 originalPanIndex;
    u32 originalReverseIndex;
    f32 level;
    f32 levelFraction;
    u32 levelIndex;
    f32 gain;
    f32 front;
    f32 rear;
    f32 left;

    volumeTable = lbl_8036984C;
    panTable = lbl_80369A68;
    if ((u8)volumeArg == 0) {
        volumeTable = panTable;
    }

    if (pan == 0x800000) {
        pan = 0;
        surroundPan = 0x7F0000;
    }

    if (pan > 0x10000) {
        panValue = (f32)(pan - 0x10000) * lbl_8047D448;
    } else {
        panValue = 0.0f;
    }
    if (surroundPan > 0x10000) {
        surroundValue = (f32)(surroundPan - 0x10000) * lbl_8047D448;
    } else {
        surroundValue = 0.0f;
    }

    if (studioMode != 0) {
        originalPanFraction = SAL_FRAC(panValue);
        originalPanIndex = __cvt_fp2unsigned(panValue);
        reversePan = lbl_8047D458 - panValue;
        originalReverseFraction = SAL_FRAC(reversePan);
        originalReverseIndex = __cvt_fp2unsigned(reversePan);
    }

    if (narrowPan != 0) {
        panValue = lbl_8047D434 +
                   lbl_8047D45C * (panValue - lbl_8047D434);
    }

    panFraction = SAL_FRAC(panValue);
    panIndex = __cvt_fp2unsigned(panValue);
    surroundFraction = SAL_FRAC(surroundValue);
    surroundIndex = __cvt_fp2unsigned(surroundValue);

    reversePan = lbl_8047D458 - panValue;
    reverseSurround = lbl_8047D458 - surroundValue;
    reversePanFraction = SAL_FRAC(reversePan);
    reversePanIndex = __cvt_fp2unsigned(reversePan);
    reverseSurroundFraction = SAL_FRAC(reverseSurround);
    reverseSurroundIndex = __cvt_fp2unsigned(reverseSurround);

    panTable += 129;

    if (studioMode == 0) {
        level = lbl_8047D430 * inputA;
        levelIndex = __cvt_fp2unsigned(level);
        levelFraction = level - (f32)levelIndex;
        gain = SAL_INTERP(volumeTable, levelIndex, levelFraction);

        front = SAL_INTERP(panTable, surroundIndex, surroundFraction);
        out[2] = lbl_8047D438 * gain * front;
        rear = SAL_INTERP(panTable, reverseSurroundIndex,
                          reverseSurroundFraction);
        left = SAL_INTERP(panTable, panIndex, panFraction);
        out[1] = gain * rear * left;
        left = SAL_INTERP(panTable, reversePanIndex, reversePanFraction);
        out[0] = gain * left;

        level = lbl_8047D430 * inputB;
        levelIndex = __cvt_fp2unsigned(level);
        levelFraction = level - (f32)levelIndex;
        gain = SAL_INTERP(volumeTable, levelIndex, levelFraction);

        front = SAL_INTERP(panTable, surroundIndex, surroundFraction);
        out[5] = lbl_8047D438 * gain * front;
        rear = SAL_INTERP(panTable, reverseSurroundIndex,
                          reverseSurroundFraction);
        left = SAL_INTERP(panTable, panIndex, panFraction);
        out[4] = gain * rear * left;
        left = SAL_INTERP(panTable, reversePanIndex, reversePanFraction);
        out[3] = gain * left;

        level = lbl_8047D430 * inputC;
        levelIndex = __cvt_fp2unsigned(level);
        levelFraction = level - (f32)levelIndex;
        gain = SAL_INTERP(volumeTable, levelIndex, levelFraction);

        front = SAL_INTERP(panTable, surroundIndex, surroundFraction);
        out[8] = lbl_8047D438 * gain * front;
        rear = SAL_INTERP(panTable, reverseSurroundIndex,
                          reverseSurroundFraction);
        left = SAL_INTERP(panTable, panIndex, panFraction);
        out[7] = gain * rear * left;
        left = SAL_INTERP(panTable, reversePanIndex, reversePanFraction);
        out[6] = gain * left;
    } else {
        f32 original;

        level = lbl_8047D430 * inputA;
        levelIndex = __cvt_fp2unsigned(level);
        levelFraction = level - (f32)levelIndex;
        gain = SAL_INTERP(volumeTable, levelIndex, levelFraction);

        front = SAL_INTERP(panTable, surroundIndex, surroundFraction);
        rear = SAL_INTERP(panTable, reverseSurroundIndex,
                          reverseSurroundFraction);
        left = SAL_INTERP(panTable, panIndex, panFraction);
        out[1] = gain * front * rear * left;
        left = SAL_INTERP(panTable, reversePanIndex, reversePanFraction);
        out[0] = gain * rear * left;

        original =
            SAL_INTERP(panTable + 4, originalPanIndex, originalPanFraction);
        out[7] = gain * front * original;
        original = SAL_INTERP(panTable + 4, originalReverseIndex,
                              originalReverseFraction);
        out[6] = gain * front * original;

        level = lbl_8047D430 * inputB;
        levelIndex = __cvt_fp2unsigned(level);
        levelFraction = level - (f32)levelIndex;
        gain = SAL_INTERP(volumeTable, levelIndex, levelFraction);

        front = SAL_INTERP(panTable, surroundIndex, surroundFraction);
        out[5] = lbl_8047D438 * gain * front;
        rear = SAL_INTERP(panTable, reverseSurroundIndex,
                          reverseSurroundFraction);
        left = SAL_INTERP(panTable, panIndex, panFraction);
        out[4] = gain * rear * left;
        left = SAL_INTERP(panTable, reversePanIndex, reversePanFraction);
        out[3] = gain * left;
        out[2] = lbl_8047D460[0];
        out[8] = lbl_8047D460[0];
    }
}

#undef SAL_INTERP
#undef SAL_FRAC
typedef struct MusyxVec3 {
    f32 x;
    f32 y;
    f32 z;
} MusyxVec3;

typedef struct MusyxEmitterListener {
    struct MusyxEmitterListener* next; /* 0x00 */
    struct MusyxEmitterListener* prev; /* 0x04 */
    u32 field_08;                      /* 0x08 */
    u32 field_0C;                      /* 0x0C */
    MusyxVec3 pos;                     /* 0x10 */
    f32 distanceScore;                 /* 0x1C */
    MusyxVec3 velocity;                /* 0x20 */
    f32 field_2C;                      /* 0x2C */
    f32 field_30;                      /* 0x30 */
    f32 field_34;                      /* 0x34 */
    u8 pad_38[0x18];                   /* 0x38 */
    f32 matrix[12];                    /* 0x50 */
    u8 pad_80[0x8];                    /* 0x80 */
    f32 dopplerScale;                  /* 0x88 */
    f32 volumeScale;                   /* 0x8C */
} MusyxEmitterListener;

typedef struct MusyxEmitter {
    struct MusyxEmitter* next;         /* 0x00 */
    struct MusyxEmitter* prev;         /* 0x04 */
    MusyxEmitterListener* listener;    /* 0x08 */
    void* ctrlList;                    /* 0x0C */
    u32 flags;                         /* 0x10 */
    MusyxVec3 pos;                     /* 0x14 */
    MusyxVec3 velocity;                /* 0x20 */
    f32 maxDistance;                   /* 0x2C */
    f32 minVolume;                     /* 0x30 */
    f32 maxVolume;                     /* 0x34 */
    f32 curve;                         /* 0x38 */
    u32 voice;                         /* 0x3C */
    void* user;                        /* 0x40 */
    u16 fxId;                          /* 0x44 */
    u8 studio;                         /* 0x46 */
    u8 maxVoices;                      /* 0x47 */
    u16 field_48;                      /* 0x48 */
    f32 field_4C;                      /* 0x4C */
} MusyxEmitter;

typedef struct MusyxVoiceLink {
    struct MusyxVoiceLink* next;       /* 0x00 */
    struct MusyxVoiceLink* prev;       /* 0x04 */
    MusyxEmitterListener* listener;    /* 0x08 */
    u32 flags;                         /* 0x10 */
    u8 pad_14[0x28];                   /* 0x14 */
    u32 voice;                         /* 0x3C */
} MusyxVoiceLink;

typedef struct MusyxStudioEmitter {
    struct MusyxStudioEmitter* next;   /* 0x00 */
    struct MusyxStudioEmitter* prev;   /* 0x04 */
    u32 field_08;                      /* 0x08 */
    MusyxVec3 pos;                     /* 0x0C */
    f32 distanceScore;                 /* 0x18 */
    u8 studio;                         /* 0x1C */
    u8 pad_1D[3];                      /* 0x1D */
    void (*activateCb)(u8 studio, void* user); /* 0x20 */
    void (*releaseCb)(u8 studio);      /* 0x24 */
    void* user;                        /* 0x28 */
    s32 fade;                          /* 0x2C */
} MusyxStudioEmitter;

extern MusyxStudioEmitter* lbl_8047B040;
extern MusyxEmitterListener* lbl_8047B044;
extern MusyxVoiceLink* lbl_8047B048;
extern u8 lbl_8047B034;
extern u8 lbl_8047B035;
extern u32 lbl_8047B038;
extern void synthSendKeyOff(u32 voice);
extern void fn_8014DCA8(u8 studio);
extern void fn_8014DC00(u8 studio, u32 isMaster, u32 type);
extern void salApplyMatrix(const f32* matrix, const f32* src, f32* dst);
extern f32 salNormalizeVector(f32* vec);
extern f32 sqrtf(f32 x);
extern f32 lbl_8047D468;
extern f64 lbl_8047D470;
extern f32 lbl_8047D478;
extern f32 lbl_8047D47C;
extern f64 lbl_8047D480;
extern f32 lbl_8047D48C;
extern f32 lbl_8047D498;

static f32 fn_8015DEC0_average_distance(MusyxStudioEmitter* emitter, u32 listenerCount)
{
    MusyxEmitterListener* listener;
    f32 sum;

    sum = lbl_8047D468;
    for (listener = lbl_8047B044; listener != NULL; listener = listener->next) {
        f32 dx;
        f32 dy;
        f32 dz;

        dx = emitter->pos.x - listener->pos.x;
        dy = emitter->pos.y - listener->pos.y;
        dz = emitter->pos.z - listener->pos.z;
        sum += dx * dx + dy * dy + dz * dz;
    }

    return sum / (f32)listenerCount;
}

static void fn_8015DEC0_apply_fade(MusyxStudioEmitter* emitter)
{
    if ((f64)(lbl_8047D47C * (f32)emitter->fade) >= lbl_8047D480) {
        fn_8014DC00(emitter->studio, 1, 0);
    } else {
        fn_8014DC00(emitter->studio, 0, 0);
    }
}

void fn_8015DEC0(void)
{
    MusyxEmitterListener* listener;
    MusyxStudioEmitter* emitter;
    MusyxStudioEmitter* victim;
    MusyxVoiceLink* voice;
    u32 listenerCount;

    listenerCount = 0;
    for (listener = lbl_8047B044; listener != NULL; listener = listener->next) {
        listenerCount++;
    }

    if (listenerCount != 0) {
        for (emitter = lbl_8047B040; emitter != NULL; emitter = emitter->next) {
            if (emitter->studio != 0xFF) {
                emitter->distanceScore = fn_8015DEC0_average_distance(emitter, listenerCount);
            }
        }
    }

    listenerCount = 0;
    for (listener = lbl_8047B044; listener != NULL; listener = listener->next) {
        listenerCount++;
    }

    if (listenerCount == 0) {
        return;
    }

    for (emitter = lbl_8047B040; emitter != NULL; emitter = emitter->next) {
        if (emitter->studio == 0xFF) {
            u8 listenerOwnsEmitter;
            f32 distanceScore;
            u32 studioMask;
            u32 activeMask;
            u32 studioIndex;

            distanceScore = fn_8015DEC0_average_distance(emitter, listenerCount);
            listenerOwnsEmitter = 0;
            for (listener = lbl_8047B044; listener != NULL; listener = listener->next) {
                if (listener->field_08 == (u32)emitter) {
                    listenerOwnsEmitter = 1;
                    break;
                }
            }

            activeMask = ~(~0u << lbl_8047B034);
            studioMask = lbl_8047B038;
            if ((activeMask & studioMask) != activeMask) {
                for (studioIndex = 0; studioIndex < lbl_8047B034; studioIndex++) {
                    if ((studioMask & (1u << studioIndex)) == 0) {
                        break;
                    }
                }

                lbl_8047B038 |= 1u << studioIndex;
                emitter->studio = (u8)(studioIndex + lbl_8047B035);
            } else {
                f32 maxDistance;

                victim = NULL;
                maxDistance = lbl_8047D478;
                {
                    MusyxStudioEmitter* scan;

                    for (scan = lbl_8047B040; scan != NULL; scan = scan->next) {
                        if (scan->studio != 0xFF && maxDistance < scan->distanceScore) {
                            maxDistance = scan->distanceScore;
                            victim = scan;
                        }
                    }
                }

                if (listenerOwnsEmitter == 0 && maxDistance <= distanceScore) {
                    goto next_emitter;
                }

                for (voice = lbl_8047B048; voice != NULL; voice = voice->next) {
                    if (voice->listener == (MusyxEmitterListener*)victim) {
                        synthSendKeyOff(voice->voice);
                        voice->flags |= 0x80000;
                        voice->voice = (u32)-1;
                    }
                }

                if (victim->releaseCb != NULL) {
                    victim->releaseCb(victim->studio);
                }
                fn_8014DCA8(victim->studio);
                emitter->studio = victim->studio;
                victim->studio = 0xFF;
                victim->field_08 = 0;
            }

            emitter->distanceScore = distanceScore;
            emitter->fade = listenerOwnsEmitter != 0 ? 0x7F0000 : 0;
            fn_8015DEC0_apply_fade(emitter);
            if (emitter->activateCb != NULL) {
                emitter->activateCb(emitter->studio, emitter->user);
            }
        } else {
            if ((emitter->field_08 & 0x80000000) != 0) {
                emitter->fade += 0x40000;
                if ((u32)emitter->fade >= 0x7F0000) {
                    emitter->fade = 0x7F0000;
                    emitter->field_08 &= ~0x80000000u;
                }
                fn_8015DEC0_apply_fade(emitter);
            }

            if ((emitter->field_08 & 0x40000000) != 0) {
                emitter->fade -= 0x40000;
                if (emitter->fade >= 0) {
                    emitter->fade = 0;
                    emitter->field_08 &= ~0x40000000u;
                }
                fn_8015DEC0_apply_fade(emitter);
            }
        }

next_emitter:
        ;
    }
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_8015E374(MusyxEmitter* emitter, f32* outVolume, f32* outDoppler,
                 f32* outX, f32* outY, f32* outCone) {
    MusyxEmitterListener* listener;
    u32 count;
    f32 sumX;
    f32 sumY;
    f32 sumCone;

    *outVolume = lbl_8047D468;
    *outDoppler = lbl_8047D48C;
    sumCone = lbl_8047D468;
    sumY = lbl_8047D468;
    sumX = lbl_8047D468;
    count = 0;

    for (listener = lbl_8047B044; listener != NULL; listener = listener->next, count++) {
        MusyxVec3 rel;
        MusyxVec3 local;
        f32 distSq;
        f32 dist;

        rel.x = emitter->pos.x - (listener->pos.x + listener->field_2C * listener->distanceScore);
        rel.y = emitter->pos.y - (listener->pos.y + listener->field_30 * listener->distanceScore);
        rel.z = emitter->pos.z - (listener->pos.z + listener->field_34 * listener->distanceScore);
        distSq = rel.x * rel.x + rel.y * rel.y + rel.z * rel.z;
        dist = distSq;
        if (distSq > lbl_8047D468) {
            dist = sqrtf(distSq);
        }

        if (dist <= emitter->maxDistance) {
            f32 ratio;
            f32 volume;
            f32 curve;
            f32 spreadDist;

            ratio = dist / emitter->maxDistance;
            curve = emitter->curve;
            if (curve >= lbl_8047D468) {
                f32 one;
                f32 invCurve;
                f32 volumeRange;

                one = lbl_8047D48C;
                invCurve = one - curve;
                volumeRange = emitter->minVolume - emitter->maxVolume;
                volume = ratio * (curve * ratio);
                volume = (invCurve * ratio) + volume;
                volume = one - volume;
                volume = (volumeRange * volume) + emitter->maxVolume;
                *outVolume += listener->volumeScale * volume;
            } else {
                f32 one;
                f32 invRatio;
                f32 volumeRange;

                one = lbl_8047D48C;
                invRatio = one - ratio;
                volumeRange = emitter->minVolume - emitter->maxVolume;
                volume = (invRatio * invRatio);
                volume = one - volume;
                volume = curve * volume;
                volume = (one + curve) * ratio - volume;
                volume = one - volume;
                volume = (volumeRange * volume) + emitter->maxVolume;
                *outVolume += listener->volumeScale * volume;
            }

            if ((emitter->flags & 0x80000) != 0) {
                continue;
            }
            if ((emitter->flags & 8) == 0 && (listener->field_0C & 1) == 0) {
                goto skip_spread;
            }

            {
                f32 dx;
                f32 dy;
                f32 dz;
                f32 travelSq;

                dx = listener->velocity.x - emitter->velocity.x;
                dy = listener->velocity.y - emitter->velocity.y;
                dz = listener->velocity.z - emitter->velocity.z;
                travelSq = dx * dx + dy * dy + dz * dz;
                spreadDist = travelSq;
                if (travelSq > lbl_8047D468) {
                    spreadDist = sqrtf(travelSq);
                }
            }

            if (spreadDist > lbl_8047D468) {
                f32 aheadX;
                f32 aheadY;
                f32 aheadZ;
                f32 lAheadX;
                f32 lAheadY;
                f32 lAheadZ;
                f32 aheadSq;
                f32 aheadDist;

                aheadX = emitter->pos.x + emitter->velocity.x * lbl_8047D498;
                aheadY = emitter->pos.y + emitter->velocity.y * lbl_8047D498;
                aheadZ = emitter->pos.z + emitter->velocity.z * lbl_8047D498;
                lAheadX = listener->pos.x + listener->velocity.x * lbl_8047D498;
                lAheadY = listener->pos.y + listener->velocity.y * lbl_8047D498;
                lAheadZ = listener->pos.z + listener->velocity.z * lbl_8047D498;
                aheadX -= lAheadX;
                aheadY -= lAheadY;
                aheadZ -= lAheadZ;
                aheadSq = aheadX * aheadX + aheadY * aheadY + aheadZ * aheadZ;
                aheadDist = aheadSq;
                if (aheadSq > lbl_8047D468) {
                    aheadDist = sqrtf(aheadSq);
                }

                if (aheadDist < dist) {
                    *outDoppler = listener->dopplerScale / (listener->dopplerScale - spreadDist);
                } else {
                    *outDoppler = listener->dopplerScale / (listener->dopplerScale + spreadDist);
                }
            }

skip_spread:
            if (dist != lbl_8047D468) {
                salApplyMatrix(listener->matrix, (const f32*)&emitter->pos,
                               (f32*)&local);
                if (local.z <= lbl_8047D468) {
                    if (-listener->matrix[10] < local.z) {
                        sumCone += -local.z / listener->matrix[10];
                    } else {
                        sumCone += lbl_8047D48C;
                    }
                } else {
                    if (listener->matrix[11] > local.z) {
                        sumCone += -local.z / listener->matrix[11];
                    } else {
                        sumCone += lbl_8047D478;
                    }
                }

                if (local.x != lbl_8047D468 || local.y != lbl_8047D468 ||
                    local.z != lbl_8047D468) {
                    salNormalizeVector((f32*)&local);
                }
                sumX += local.x;
                sumY -= local.y;
            }
        }
    }

    if (count != 0) {
        *outX = sumX / (f32)count;
        *outY = sumY / (f32)count;
        *outCone = sumCone / (f32)count;
    }
}
#pragma pop

/* s_data.c: project group insertion. */
typedef struct SDataGroup {
    u32 nextOffset;
    u16 id;
    u16 type;
    u32 macroOffset;
    u32 sampleOffset;
    u32 curveOffset;
    u32 keymapOffset;
    u32 layerOffset;
    u32 normalPageOffset;
} SDataGroup;

typedef struct SDataStackEntry {
    SDataGroup* group;
    void* sampleDirectory;
    void* project;
} SDataStackEntry;

extern u8 lbl_8047AF18;
extern s16 lbl_8047AFE8;
extern SDataStackEntry lbl_80447860[128];
extern void* fn_80162FAC(void* address); /* hwTransAddr */
extern u32 fn_80151770(void* sampleDirectory, void* samples); /* dataInsertSDir */
extern void fn_80151A68(u16 group, void* effects, u16 count); /* dataInsertFX */
extern void fn_80159C54(u16 id, MusyxPoolData* data, u8 type,
                        u32 remove); /* InsertData */
extern void fn_80163188(void); /* hwSyncSampleMem */

static inline void sdataScanIDList(u16* reference, MusyxPoolData* data,
                                   u8 type) {
    u16 id;

    while (*reference != 0xFFFF) {
        if ((*reference & 0x8000) != 0) {
            id = *reference & 0x3FFF;
            while (id <= reference[1]) {
                fn_80159C54(id, data, type, 0);
                id++;
            }
            reference += 2;
        } else {
            fn_80159C54(*reference++, data, type, 0);
        }
    }
}

u32 fn_80159EF0(void* project, u16 group, void* samples,
                void* sampleDirectory,
                MusyxPoolData* pool) { /* sndPushGroup */
    SDataGroup* entry;

    if (lbl_8047AF18 != 0 && lbl_8047AFE8 < 128) {
        entry = project;
        while (entry->nextOffset != 0xFFFFFFFF) {
            if (entry->id == group) {
                lbl_80447860[lbl_8047AFE8].group = entry;
                lbl_80447860[lbl_8047AFE8].sampleDirectory =
                    sampleDirectory;
                lbl_80447860[lbl_8047AFE8].project = project;

                samples = fn_80162FAC(samples);
                if (fn_80151770(sampleDirectory, samples) != 0) {
                    sdataScanIDList((u16*)((u8*)project +
                                          entry->sampleOffset),
                                    (MusyxPoolData*)sampleDirectory, 1);
                }
                sdataScanIDList((u16*)((u8*)project + entry->macroOffset),
                                pool, 0);
                sdataScanIDList((u16*)((u8*)project + entry->curveOffset),
                                pool, 4);
                sdataScanIDList((u16*)((u8*)project + entry->keymapOffset),
                                pool, 2);
                sdataScanIDList((u16*)((u8*)project + entry->layerOffset),
                                pool, 3);

                if (entry->type == 1) {
                    u8* effectData =
                        (u8*)project + entry->normalPageOffset;
                    fn_80151A68(group, effectData + 4,
                                *(u16*)effectData);
                }
                fn_80163188();
                lbl_8047AFE8++;
                return 1;
            }
            entry = (SDataGroup*)((u8*)project + entry->nextOffset);
        }
    }
    return 0;
}

/* snd_service: periodically advances active sound emitters and publishes
 * positional updates to the synthesizer. */
typedef struct SndServiceSource {
    u8 _00[0x1C];
    u8 studio;
} SndServiceSource;

typedef struct SndServiceCtrl {
    u8 ctrl;
    u8 _01;
    u16 value;
} SndServiceCtrl;

typedef struct SndServiceCtrlList {
    u8 count;
    u8 _01[3];
    SndServiceCtrl* controls;
} SndServiceCtrlList;

typedef struct SndServiceVoice {
    struct SndServiceVoice* next;
    struct SndServiceVoice* prev;
    SndServiceSource* source;
    SndServiceCtrlList* ctrlList;
    u32 flags;
    f32 position[3];
    f32 velocity[3];
    f32 maxDistance;
    f32 innerLevel;
    f32 outerLevel;
    f32 distanceCurve;
    u32 handle;
    u32 group;
    u16 effectId;
    u8 fallbackStudio;
    u8 maxVoices;
    u16 volumeLevelCount;
    u8 _4A[2];
    f32 fade;
} SndServiceVoice;

typedef struct SndVec3 {
    f32 x;
    f32 y;
    f32 z;
} SndVec3;

typedef struct SndFMatrix {
    f32 m[3][3];
    f32 t[3];
} SndFMatrix;

typedef struct SndRoom {
    struct SndRoom* next;
    struct SndRoom* prev;
    u32 flags;
    SndVec3 position;
    f32 averageDistance;
    u8 studio;
    u8 _1D[3];
    void (*activateReverb)(u8 studio, void* user);
    void (*deactivateReverb)(u8 studio);
    void* user;
    u32 currentMasterVolume;
} SndRoom;

typedef struct SndListener {
    struct SndListener* next;
    struct SndListener* prev;
    SndRoom* room;
    u32 flags;
    SndVec3 position;
    f32 volumePositionOffset;
    SndVec3 direction;
    SndVec3 heading;
    SndVec3 right;
    SndVec3 up;
    SndFMatrix matrix;
    f32 surroundDistanceFront;
    f32 surroundDistanceBack;
    f32 soundSpeed;
    f32 volume;
} SndListener;

typedef struct SndServiceStudio {
    struct SndServiceStudio* next;
    u32 _04;
    u32 flags;
    SndVec3 position;
    f32 averageDistance;
    u8 studio;
    u8 _1D[3];
    void (*assignedCallback)(u8 studio, u32 userData);
    void (*releasedCallback)(u8 studio);
    u32 userData;
    s32 mix;
} SndServiceStudio;

typedef struct SndServiceGroupNode {
    struct SndServiceGroupNode* next;
    f32 value;
    SndServiceVoice* voice;
} SndServiceGroupNode;

typedef struct SndServiceStartNode {
    struct SndServiceStartNode* next;
    f32 value;
    f32 pan;
    f32 volume;
    f32 surround;
    f32 pitch;
    SndServiceVoice* voice;
} SndServiceStartNode;

typedef struct SndServiceGroup {
    u32 key;
    SndServiceStartNode* starting;
    SndServiceGroupNode* running;
    u16 numRunning;
    u16 _0E;
} SndServiceGroup;

typedef struct SndServiceEmitterPair {
    struct SndServiceEmitterPair* next;
    u8 _04[0x10];
    f32 scale;
    u8 _18[4];
    u8 level;
    u8 lastStudio;
    u8 _1E[2];
    SndServiceSource* left;
    SndServiceSource* right;
    u32 flags;
    u8 _2C[8];
    u8 update[4];
} SndServiceEmitterPair;

extern u8 lbl_8047B04C;
extern u8 lbl_8047B032;
extern u8 lbl_8047B031;
extern u8 lbl_8047B030;
extern u8 lbl_8047B033;
extern SndServiceEmitterPair* lbl_8047B03C;
extern SndServiceGroup lbl_80448590[];
extern SndServiceGroupNode lbl_80448990[];
extern SndServiceStartNode lbl_80448C90[];
extern f32 lbl_8047D488;
extern f32 lbl_8047D49C;
extern f32 lbl_8047D4A0;
extern f32 lbl_8047D4A8;
extern f32 lbl_8047D4AC;
extern f32 lbl_8047D4B0[];
extern u32 synthFXStart(u16 effect, u8 volume, u8 pan, u8 studio, u32 itd);
extern u32 synthFXSetCtrl(u32 handle, u8 ctrl, u8 value);
extern u32 synthFXSetCtrl14(u32 handle, u8 ctrl, u16 value);
extern u32 sndFXCheck(u32 handle);
extern void fn_8014DD98(u8 studio, void* update);
extern void fn_8014DDB8(u8 studio, void* update);
extern void hwDisableIrq(void);
extern void hwEnableIrq(void);
extern void salCrossProduct(f32* result, const f32* first,
                            const f32* second);
extern void salInvertMatrix(SndFMatrix* result, const SndFMatrix* matrix);

static inline void s3dMakeListenerMatrix(SndListener* listener) {
    SndFMatrix matrix;

    salCrossProduct((f32*)&listener->right, (f32*)&listener->heading,
                    (f32*)&listener->up);
    matrix.m[0][0] = listener->right.x;
    matrix.m[1][0] = listener->right.y;
    matrix.m[2][0] = listener->right.z;
    matrix.m[0][1] = listener->up.x;
    matrix.m[1][1] = listener->up.y;
    matrix.m[2][1] = listener->up.z;
    matrix.m[0][2] = -listener->heading.x;
    matrix.m[1][2] = -listener->heading.y;
    matrix.m[2][2] = -listener->heading.z;
    matrix.t[0] = listener->position.x;
    matrix.t[1] = listener->position.y;
    matrix.t[2] = listener->position.z;
    salInvertMatrix(&listener->matrix, &matrix);
}

static inline void s3dRemoveListenerFromRoom(SndRoom* room) {
    SndListener* listener;
    u32 count;

    count = 0;
    for (listener = (SndListener*)lbl_8047B044; listener != 0;
         listener = listener->next) {
        if (listener->room == room) {
            count++;
        }
    }
    if (count == 1) {
        room->flags &= 0x7FFFFFFF;
        room->flags |= 0x40000000;
    }
}

static inline void s3dAddListenerToRoom(SndRoom* room) {
    if ((room->flags & 0x80000000) == 0 &&
        room->currentMasterVolume == 0) {
        room->flags |= 0x80000000;
    }
}

u32 fn_8015ED00(SndListener* listener, SndVec3* position,
                SndVec3* direction, SndVec3* heading, SndVec3* up,
                u8 volume, SndRoom* room) { /* sndUpdateListener */
    if (lbl_8047AF18 != 0) {
        hwDisableIrq();
        listener->position = *position;
        listener->direction = *direction;
        listener->heading = *heading;
        listener->up = *up;
        s3dMakeListenerMatrix(listener);
        listener->volume = volume / lbl_8047D488;

        if (room != listener->room) {
            if (listener->room != 0) {
                s3dRemoveListenerFromRoom(listener->room);
            }
            listener->room = room;
            if (room != 0) {
                s3dAddListenerToRoom(room);
            }
        }
        hwEnableIrq();
        return 1;
    }
    return 0;
}

u32 fn_8015EF04(SndListener* listener, SndVec3* position,
                SndVec3* direction, SndVec3* heading, SndVec3* up,
                f32 surroundFront, f32 surroundBack, f32 soundSpeed,
                u32 flags, u8 volume, SndRoom* room) { /* sndAddListener */
    if (lbl_8047AF18 != 0) {
        hwDisableIrq();
        listener->next = (SndListener*)lbl_8047B044;
        if (listener->next != 0) {
            listener->next->prev = listener;
        }
        listener->prev = 0;
        lbl_8047B044 = (MusyxEmitterListener*)listener;

        listener->position = *position;
        listener->direction = *direction;
        listener->heading = *heading;
        listener->up = *up;
        listener->surroundDistanceFront = surroundFront;
        listener->surroundDistanceBack = surroundBack;
        listener->soundSpeed = soundSpeed;
        listener->volumePositionOffset = lbl_8047D468;
        s3dMakeListenerMatrix(listener);
        listener->flags = flags;
        listener->volume = volume / lbl_8047D488;
        listener->room = room;
        if (room != 0) {
            s3dAddListenerToRoom(room);
        }
        hwEnableIrq();
        return 1;
    }
    return 0;
}

u32 fn_8015F124(SndServiceVoice* voice, f32 value, f32 pan, f32 volume,
                f32 surround, f32 pitch) { /* AddStartingEmitter */
    s32 i;
    SndServiceStartNode* node;

    for (i = 0; i < lbl_8047B032; i++) {
        if (voice->group == lbl_80448590[i].key) {
            break;
        }
    }

    if (i == lbl_8047B032) {
        if (lbl_8047B032 == 64) {
            return 0;
        }
        lbl_80448590[i].starting = 0;
        lbl_80448590[i].running = 0;
        lbl_80448590[i].numRunning = 0;
        lbl_80448590[i].key = voice->group;
        lbl_8047B032++;
    }

    if (lbl_8047B031 == 64) {
        return 0;
    }

    node = lbl_80448590[i].starting;
    if (node != 0) {
        for (; node->next != 0; node = node->next) {
            if (node->value < value) {
                break;
            }
        }
        lbl_80448C90[lbl_8047B031].next = node->next;
        node->next = &lbl_80448C90[lbl_8047B031];
    } else {
        lbl_80448C90[lbl_8047B031].next = lbl_80448590[i].starting;
        lbl_80448590[i].starting = &lbl_80448C90[lbl_8047B031];
    }

    lbl_80448C90[lbl_8047B031].voice = voice;
    lbl_80448C90[lbl_8047B031].pitch = pitch;
    lbl_80448C90[lbl_8047B031].pan = pan;
    lbl_80448C90[lbl_8047B031].volume = volume;
    lbl_80448C90[lbl_8047B031].surround = surround;
    lbl_80448C90[lbl_8047B031].value = value;
    lbl_8047B031++;
    return 1;
}

static inline void s3dSetFXParameters(SndServiceVoice* voice, f32 value,
                                      f32 pan, f32 volume, f32 surround,
                                      f32 pitch) {
    u32 converted;
    u32 handle;
    u8 controlValue;
    u16 control14;

    handle = voice->handle;
    if (voice->flags & 0x100000) {
        converted = (u32)(s32)(lbl_8047D488 * (voice->fade * value));
    } else {
        converted = (u32)(s32)(lbl_8047D488 * value);
    }
    controlValue = 0x7F;
    if ((u8)converted <= 0x7F) {
        controlValue = converted;
    }
    synthFXSetCtrl(handle, 7, controlValue);

    converted = (u32)(s32)(lbl_8047D49C * (lbl_8047D48C + pan));
    controlValue = 0x7F;
    if ((u8)converted <= 0x7F) {
        controlValue = converted;
    }
    synthFXSetCtrl(handle, 0xA, controlValue);

    converted = (u32)(s32)(lbl_8047D49C * (lbl_8047D48C - surround));
    controlValue = 0x7F;
    if ((u8)converted <= 0x7F) {
        controlValue = converted;
    }
    synthFXSetCtrl(handle, 0x83, controlValue);

    converted = __cvt_fp2unsigned(lbl_8047D4A0 * pitch);
    control14 = 0x3FFF;
    if (converted <= 0x3FFF) {
        control14 = converted;
    }
    synthFXSetCtrl14(handle, 0x84, control14);

    if (voice->ctrlList != 0) {
        u32 i;
        SndServiceCtrl* ctrl = voice->ctrlList->controls;

        for (i = 0; i < voice->ctrlList->count; i++, ctrl++) {
            if (ctrl->ctrl < 0x40 || ctrl->ctrl == 0x80 ||
                ctrl->ctrl == 0x84) {
                synthFXSetCtrl14(handle, ctrl->ctrl, ctrl->value);
            } else {
                synthFXSetCtrl(handle, ctrl->ctrl, (u8)ctrl->value);
            }
        }
    }
}

void fn_8015F270(void) { /* StartContinousEmitters */
    s32 i;
    SndServiceStartNode* start;
    SndServiceVoice* voice;
    f32 difference;

    for (i = 0; i < lbl_8047B032; i++) {
        for (start = lbl_80448590[i].starting; start != 0;
             start = start->next) {
            if (lbl_80448590[i].running != 0 &&
                !((lbl_8047B033 != 0 &&
                   (lbl_80448590[i].key & 0x80000000) != 0) &&
                  lbl_80448590[i].numRunning <
                      lbl_80448590[i].starting->voice->maxVoices)) {
                difference =
                    start->value - lbl_80448590[i].running->value;
                if (difference <= lbl_8047D4A8) {
                    continue;
                } else if (difference <= lbl_8047D4AC) {
                    if (++start->voice->volumeLevelCount < 20) {
                        continue;
                    }
                } else {
                    start->voice->volumeLevelCount = 0;
                }
            }

            voice = start->voice;
            if (voice->source != 0 && voice->source->studio == 0xFF) {
                goto set_flags;
            }
            voice->handle =
                synthFXStart(voice->effectId, 127, 64,
                             voice->source != 0 ? voice->source->studio
                                                : voice->fallbackStudio,
                             (voice->flags & 0x10) != 0);
            if (voice->handle == 0xFFFFFFFF) {
set_flags:
                if (!(voice->flags & 2)) {
                    voice->flags |= 0x40000;
                    voice->flags &= ~0x20000;
                }
            } else {
                if (!(voice->flags & 0x20)) {
                    voice->flags |= 0x100000;
                    voice->fade = lbl_8047D468;
                } else {
                    voice->fade = lbl_8047D48C;
                }
                s3dSetFXParameters(voice, start->value, start->pan,
                                   start->volume, start->surround,
                                   start->pitch);
                voice->flags &= ~0x20000;
                lbl_80448590[i].numRunning++;
                if (lbl_80448590[i].running != 0) {
                    lbl_80448590[i].running =
                        lbl_80448590[i].running->next;
                }
            }
        }
    }
}

#pragma push
#pragma optimization_level 4
void fn_8015F620(void)
{
    SndServiceVoice* voice;
    SndServiceVoice* next;
    SndServiceEmitterPair* pair;
    f32 value;
    f32 pitch;
    f32 pan;
    f32 surround;
    f32 volume;

    if (lbl_8047B04C != 0) {
        lbl_8047B04C--;
        return;
    }

    lbl_8047B04C = 3;
    lbl_8047B032 = 0;
    lbl_8047B031 = 0;
    lbl_8047B030 = 0;

    voice = (SndServiceVoice*)lbl_8047B048;
    while (voice != 0) {
        u32 flags = voice->flags;
        next = voice->next;

        if (flags & 0x40000) {
            if (voice->next != 0) {
                voice->next->prev = voice->prev;
            }
            if (voice->prev != 0) {
                voice->prev->next = voice->next;
            } else {
                lbl_8047B048 = (MusyxVoiceLink*)voice->next;
            }
            voice->flags &= 0xFFFF;
            if (voice->handle != 0xFFFFFFFF) {
                synthSendKeyOff(voice->handle);
            }
            voice = next;
            continue;
        }

        if (flags & 0x20001) {
            fn_8015E374((MusyxEmitter*)voice, &value, &pitch, &pan,
                        &surround, &volume);
        }

        flags = voice->flags;
        if (flags & 0x80000) {
            if ((voice->source == 0 || voice->source->studio == 0xFF) &&
                value != lbl_8047D468) {
                voice->flags &= ~0x80000;
                voice->flags |= 0x20000;
            }
            voice = next;
            continue;
        }

        if (flags & 0x20000) {
            if (value == lbl_8047D468 && (flags & 4)) {
                voice->flags |= 0x80000;
                voice->flags &= ~0x20000;
            } else if (value == lbl_8047D468 && (flags & 0x40)) {
                if (voice->next != 0) {
                    voice->next->prev = voice->prev;
                }
                if (voice->prev != 0) {
                    voice->prev->next = voice->next;
                } else {
                    lbl_8047B048 = (MusyxVoiceLink*)voice->next;
                }
                voice->flags &= 0xFFFF;
                if (voice->handle != 0xFFFFFFFF) {
                    synthSendKeyOff(voice->handle);
                }
                voice = next;
                continue;
            } else if (flags & 1) {
                if (fn_8015F124(voice, value, pan, surround, volume, pitch) != 0) {
                    voice = next;
                    continue;
                }
            } else {
                u8 studio;
                if (voice->source != 0 && voice->source->studio != 0xFF) {
                    if (!(flags & 2)) {
                        voice->flags |= 0x40000;
                        voice->flags &= ~0x20000;
                    }
                } else {
                    studio = voice->source != 0 ? voice->source->studio : voice->fallbackStudio;
                    voice->handle = synthFXStart(voice->effectId, 0x7F, 0x40,
                                                 studio, (flags >> 4) & 1);
                    if (voice->handle == 0xFFFFFFFF) {
                        if (!(flags & 2)) {
                            voice->flags |= 0x40000;
                            voice->flags &= ~0x20000;
                        }
                    }
                }
            }
        } else {
            voice->handle = sndFXCheck(voice->handle);
            if (voice->handle == 0xFFFFFFFF) {
                if (flags & 2) {
                    voice->flags |= 0x20000;
                } else {
                    voice->flags |= 0x40000;
                }
            }
        }

        if (voice->handle != 0xFFFFFFFF) {
            u32 handle = voice->handle;

            if (voice->flags & 1) {
                u32 groupIndex;
                SndServiceGroup* group;
                SndServiceGroupNode* before;
                SndServiceGroupNode* node;

                for (groupIndex = 0; groupIndex < lbl_8047B032; groupIndex++) {
                    if (lbl_80448590[groupIndex].key == voice->group) {
                        break;
                    }
                }
                group = &lbl_80448590[groupIndex];
                if (groupIndex == lbl_8047B032) {
                    group->starting = 0;
                    group->running = 0;
                    group->numRunning = 0;
                    group->key = voice->group;
                    lbl_8047B032++;
                }
                group->numRunning++;
                before = 0;
                node = group->running;
                while (node != 0 && node->value <= value) {
                    before = node;
                    node = node->next;
                }
                if (before == 0) {
                    group->running = &lbl_80448990[lbl_8047B030];
                } else {
                    before->next = &lbl_80448990[lbl_8047B030];
                }
                before = &lbl_80448990[lbl_8047B030++];
                before->next = node;
                before->voice = voice;
                before->value = value;
            }

            if (value == lbl_8047D468 && (voice->flags & 4)) {
                synthSendKeyOff(handle);
                voice->handle = 0xFFFFFFFF;
                if (voice->flags & 2) {
                    voice->flags |= 0x80000;
                } else {
                    voice->flags |= 0x40000;
                }
            } else {
                u32 converted;
                u8 controlValue;
                u16 control14;

                if (voice->flags & 0x100000) {
                    converted = (u32)(s32)(lbl_8047D488 * (voice->fade * value));
                } else {
                    converted = (u32)(s32)(lbl_8047D488 * value);
                }
                controlValue = 0x7F;
                if ((u8)converted <= 0x7F) {
                    controlValue = converted;
                }
                synthFXSetCtrl(handle, 7, controlValue);

                converted = (u32)(s32)(lbl_8047D49C * (lbl_8047D48C + pan));
                controlValue = 0x7F;
                if ((u8)converted <= 0x7F) {
                    controlValue = converted;
                }
                synthFXSetCtrl(handle, 0xA, controlValue);

                converted = (u32)(s32)(lbl_8047D49C * (lbl_8047D48C - surround));
                controlValue = 0x7F;
                if ((u8)converted <= 0x7F) {
                    controlValue = converted;
                }
                synthFXSetCtrl(handle, 0x83, controlValue);

                converted = __cvt_fp2unsigned(lbl_8047D4A0 * pitch);
                control14 = 0x3FFF;
                if (converted <= 0x3FFF) {
                    control14 = converted;
                }
                synthFXSetCtrl14(handle, 0x84, control14);

                if (voice->ctrlList != 0) {
                    u32 i;
                    SndServiceCtrl* ctrl = voice->ctrlList->controls;
                    for (i = 0; i < voice->ctrlList->count; i++, ctrl++) {
                        if (ctrl->ctrl < 0x40 || ctrl->ctrl == 0x80 || ctrl->ctrl == 0x84) {
                            synthFXSetCtrl14(handle, ctrl->ctrl, ctrl->value);
                        } else {
                            synthFXSetCtrl(handle, ctrl->ctrl, (u8)ctrl->value);
                        }
                    }
                }
            }
        }

        if (voice->flags & 0x100000) {
            voice->fade += lbl_8047D4B0[0];
            if (voice->fade >= lbl_8047D48C) {
                voice->flags &= ~0x100000;
            }
        }
        voice = next;
    }

    fn_8015F270();
    fn_8015DEC0();

    pair = lbl_8047B03C;
    while (pair != 0) {
        u8 leftStudio = pair->left->studio;
        u8 rightStudio = pair->right->studio;

        if (!(pair->flags & 0x80000000)) {
            if (leftStudio != 0xFF && rightStudio != 0xFF) {
                pair->update[1] = (u8)(pair->level * pair->scale);
                pair->update[2] = 0;
                pair->update[0] = (u8)(lbl_8047D488 * pair->scale);
                if (pair->flags & 1) {
                    pair->update[3] = rightStudio;
                    fn_8014DD98(leftStudio, pair->update);
                } else {
                    pair->update[3] = leftStudio;
                    fn_8014DD98(rightStudio, pair->update);
                }
                pair->flags |= 0x80000000;
            }
        } else if (leftStudio == 0xFF || rightStudio == 0xFF) {
            if ((leftStudio != 0xFF && leftStudio == pair->lastStudio) ||
                (rightStudio != 0xFF && rightStudio == pair->lastStudio)) {
                fn_8014DDB8(pair->lastStudio, pair->update);
            }
            pair->flags &= 0x7FFFFFFF;
        } else {
            pair->update[1] = (u8)(pair->level * pair->scale);
            pair->update[2] = 0;
            pair->update[0] = (u8)(lbl_8047D488 * pair->scale);
        }
        pair = pair->next;
    }
}
#pragma pop

extern u32 _GetInputValue(u8* obj, u8* motionBase, u8 p1, u8 p2); /* true return type IS
                                                                      * u32 -- the callee
                                                                      * explicitly casts its
                                                                      * own return expression
                                                                      * to (u16) internally
                                                                      * (verified: changing
                                                                      * this to a declared u16
                                                                      * return regressed the
                                                                      * 11 already-matched
                                                                      * inpGet* callers, which
                                                                      * defensively re-mask a
                                                                      * u16-typed return value
                                                                      * that retail does not). */
/* inpGetVolume, 0x801615D4: same cached-getter shape as the 11 already-matched
 * inpGet* below (bit 0x1, motion struct at +0x218, cached value at +0x238). */
u32 inpGetVolume(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x1)) {
        return *(u16*)(obj + 0x238);
    }
    *(u32*)(obj + 0x214) = flags & ~0x1u;
    return _GetInputValue(obj, obj + 0x218, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#if 0
asm void inpGetPanning(void) {
#include "src/game/people/people_field_inpGetPanning.inc"
}
#else
/* If flags bit 30 (0x2) is CLEAR: return halfword at 0x25c.
 * If SET: clear bit 30, call _GetInputValue with motion data, return its result. */
u32 inpGetPanning(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x2)) {
        return *(u16*)(obj + 0x25c);
    }
    *(u32*)(obj + 0x214) = flags & ~0x2u;
    return _GetInputValue(obj, obj + 0x23c, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetSurroundPanning(void) {
#include "src/game/people/people_field_inpGetSurroundPanning.inc"
}
#else
/* bit 29 (0x4), offset 0x280, motion at 0x260 */
u32 inpGetSurroundPanning(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x4)) {
        return *(u16*)(obj + 0x280);
    }
    *(u32*)(obj + 0x214) = flags & ~0x4u;
    return _GetInputValue(obj, obj + 0x260, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetPitchBend(void) {
#include "src/game/people/people_field_inpGetPitchBend.inc"
}
#else
/* bit 28 (0x8), offset 0x2a4, motion at 0x284 */
u32 inpGetPitchBend(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x8)) {
        return *(u16*)(obj + 0x2a4);
    }
    *(u32*)(obj + 0x214) = flags & ~0x8u;
    return _GetInputValue(obj, obj + 0x284, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetDoppler(void) {
#include "src/game/people/people_field_inpGetDoppler.inc"
}
#else
/* bit 27 (0x10), offset 0x2c8, motion at 0x2a8 */
u32 inpGetDoppler(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x10)) {
        return *(u16*)(obj + 0x2c8);
    }
    *(u32*)(obj + 0x214) = flags & ~0x10u;
    return _GetInputValue(obj, obj + 0x2a8, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetModulation(void) {
#include "src/game/people/people_field_inpGetModulation.inc"
}
#else
/* bit 26 (0x20), offset 0x2ec, motion at 0x2cc */
u32 inpGetModulation(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x20)) {
        return *(u16*)(obj + 0x2ec);
    }
    *(u32*)(obj + 0x214) = flags & ~0x20u;
    return _GetInputValue(obj, obj + 0x2cc, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetPedal(void) {
#include "src/game/people/people_field_inpGetPedal.inc"
}
#else
/* bit 25 (0x40), offset 0x310, motion at 0x2f0 */
u32 inpGetPedal(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x40)) {
        return *(u16*)(obj + 0x310);
    }
    *(u32*)(obj + 0x214) = flags & ~0x40u;
    return _GetInputValue(obj, obj + 0x2f0, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetPreAuxA(void) {
#include "src/game/people/people_field_inpGetPreAuxA.inc"
}
#else
/* bit 23 (0x100), offset 0x358, motion at 0x338 */
u32 inpGetPreAuxA(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x100)) {
        return *(u16*)(obj + 0x358);
    }
    *(u32*)(obj + 0x214) = flags & ~0x100u;
    return _GetInputValue(obj, obj + 0x338, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetReverb(void) {
#include "src/game/people/people_field_inpGetReverb.inc"
}
#else
/* bit 22 (0x200), offset 0x37c, motion at 0x35c */
u32 inpGetReverb(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x200)) {
        return *(u16*)(obj + 0x37c);
    }
    *(u32*)(obj + 0x214) = flags & ~0x200u;
    return _GetInputValue(obj, obj + 0x35c, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetPreAuxB(void) {
#include "src/game/people/people_field_inpGetPreAuxB.inc"
}
#else
/* bit 21 (0x400), offset 0x3a0, motion at 0x380 */
u32 inpGetPreAuxB(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x400)) {
        return *(u16*)(obj + 0x3a0);
    }
    *(u32*)(obj + 0x214) = flags & ~0x400u;
    return _GetInputValue(obj, obj + 0x380, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetPostAuxB(void) {
#include "src/game/people/people_field_inpGetPostAuxB.inc"
}
#else
/* bit 20 (0x800), offset 0x3c4, motion at 0x3a4 */
u32 inpGetPostAuxB(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x800)) {
        return *(u16*)(obj + 0x3c4);
    }
    *(u32*)(obj + 0x214) = flags & ~0x800u;
    return _GetInputValue(obj, obj + 0x3a4, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#if 0
asm void inpGetTremolo(void) {
#include "src/game/people/people_field_inpGetTremolo.inc"
}
#else
/* bit 19 (0x1000), offset 0x3e8, motion at 0x3c8 */
u32 inpGetTremolo(u8* obj) {
    u32 flags;
    flags = *(u32*)(obj + 0x214);
    if (!(flags & 0x1000)) {
        return *(u16*)(obj + 0x3e8);
    }
    *(u32*)(obj + 0x214) = flags & ~0x1000u;
    return _GetInputValue(obj, obj + 0x3c8, *(u8*)(obj + 0x121), *(u8*)(obj + 0x122));
}
#endif
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
/* fn_80161934 = inpGetAuxA (0x80161934) -- same shape as fn_801619E8/inpGetAuxB
 * below but using lbl_80369C90 (dirtyMask table) and lbl_80435B74 (inpAuxA,
 * already-known identity) instead of lbl_80369CA0/lbl_804356F4. */
extern u32 lbl_80449390[];
extern u32 lbl_80369C90[];
extern u8  lbl_80435B74[];
u32 fn_80161934(u8 idx, u8 index, u8 midi, u8 midiSet) {
    u32 mask = lbl_80369C90[index] & ((u32(*)[16])lbl_80449390)[midiSet][midi];
    u32 nonzero = ((-mask | mask) >> 31);
    if (nonzero) {
        ((u32(*)[16])lbl_80449390)[midiSet][midi] &= ~lbl_80369C90[index];
    }
    if (nonzero) {
        return _GetInputValue(NULL, lbl_80435B74 + (u32)idx * 0x90 + (u32)index * 0x24, midi, midiSet);
    } else {
        u32 offset = (u32)idx * 0x90;
        u8* input = lbl_80435B74;
        input += offset;
        input += (u32)index * 0x24;
        return *(u16*)(input + 0x20);
    }
}
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801619E8(void) {
#include "src/game/people/people_field_fn_801619E8.inc"
}
#else
extern u32 lbl_80449390[];
extern u32 lbl_80369CA0[];
extern u8  lbl_804356F4[];
u32 fn_801619E8(u8 idx, u8 index, u8 midi, u8 midiSet) {
    u32 mask = lbl_80369CA0[index] & ((u32(*)[16])lbl_80449390)[midiSet][midi];
    u32 nonzero = ((-mask | mask) >> 31);
    if (nonzero) {
        ((u32(*)[16])lbl_80449390)[midiSet][midi] &= ~lbl_80369CA0[index];
    }
    if (nonzero) {
        return _GetInputValue(NULL, lbl_804356F4 + (u32)idx * 0x90 + (u32)index * 0x24, midi, midiSet);
    } else {
        u32 offset = (u32)idx * 0x90;
        u8* input = lbl_804356F4;
        input += offset;
        input += (u32)index * 0x24;
        return *(u16*)(input + 0x20);
    }
}
#endif
#pragma pop

/* snd_midictrl.c: inpInit. The pre-2.0.1 SYNTH_VOICE layout ends its
 * input-controller block at inpTremolo; later MusyX versions append filter
 * controls here. */
typedef struct CtrlSource {
    u8 midiCtrl;
    u8 combine;
    s32 scale;
} CtrlSource;

typedef struct CtrlDest {
    CtrlSource source[4];
    u16 oldValue;
    u8 numSource;
} CtrlDest;

typedef struct SndInputVoice {
    u8 pad_000[0xA8];
    u8 timeUsedByInput;             /* 0x0A8 */
    u8 pad_0A9[0x1D4 - 0x0A9];
    u8 lfoUsedByInput[2];           /* 0x1D4 */
    u8 pad_1D6[0x214 - 0x1D6];
    u32 midiDirtyFlags;             /* 0x214 */
    CtrlDest inpVolume;             /* 0x218 */
    CtrlDest inpPanning;            /* 0x23C */
    CtrlDest inpSurroundPanning;    /* 0x260 */
    CtrlDest inpPitchBend;          /* 0x284 */
    CtrlDest inpDoppler;            /* 0x2A8 */
    CtrlDest inpModulation;         /* 0x2CC */
    CtrlDest inpPedal;              /* 0x2F0 */
    CtrlDest inpPortamento;         /* 0x314 */
    CtrlDest inpPreAuxA;            /* 0x338 */
    CtrlDest inpReverb;             /* 0x35C */
    CtrlDest inpPreAuxB;             /* 0x380 */
    CtrlDest inpPostAuxB;            /* 0x3A4 */
    CtrlDest inpTremolo;             /* 0x3C8 */
} SndInputVoice;

static inline void inpResetGlobalMIDIDirtyFlags(void) {
    u32 i;
    u32 j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            ((u32 (*)[16])lbl_80449390)[i][j] = 0xFF;
        }
    }
}

#pragma push
#pragma optimization_level 4
void fn_80161A9C(SndInputVoice* voice) {
    u32 i;
    u32 studio;

    if (voice != NULL) {
        voice->inpVolume.source[0].midiCtrl = 7;
        voice->inpVolume.source[0].combine = 0;
        voice->inpVolume.source[0].scale = 0x10000;
        voice->inpVolume.source[1].midiCtrl = 11;
        voice->inpVolume.source[1].combine = 2;
        voice->inpVolume.source[1].scale = 0x10000;
        voice->inpVolume.numSource = 2;
        voice->inpPanning.source[0].midiCtrl = 10;
        voice->inpPanning.source[0].combine = 0;
        voice->inpPanning.source[0].scale = 0x10000;
        voice->inpPanning.numSource = 1;
        voice->inpSurroundPanning.source[0].midiCtrl = 131;
        voice->inpSurroundPanning.source[0].combine = 0;
        voice->inpSurroundPanning.source[0].scale = 0x10000;
        voice->inpSurroundPanning.numSource = 1;
        voice->inpPitchBend.source[0].midiCtrl = 128;
        voice->inpPitchBend.source[0].combine = 0;
        voice->inpPitchBend.source[0].scale = 0x10000;
        voice->inpPitchBend.numSource = 1;
        voice->inpModulation.source[0].midiCtrl = 1;
        voice->inpModulation.source[0].combine = 0;
        voice->inpModulation.source[0].scale = 0x10000;
        voice->inpModulation.numSource = 1;
        voice->inpPedal.source[0].midiCtrl = 64;
        voice->inpPedal.source[0].combine = 0;
        voice->inpPedal.source[0].scale = 0x10000;
        voice->inpPedal.numSource = 1;
        voice->inpPortamento.source[0].midiCtrl = 65;
        voice->inpPortamento.source[0].combine = 0;
        voice->inpPortamento.source[0].scale = 0x10000;
        voice->inpPortamento.numSource = 1;
        voice->inpPreAuxA.numSource = 0;
        voice->inpReverb.source[0].midiCtrl = 91;
        voice->inpReverb.source[0].combine = 0;
        voice->inpReverb.source[0].scale = 0x10000;
        voice->inpReverb.numSource = 1;
        voice->inpPreAuxB.numSource = 0;
        voice->inpPostAuxB.source[0].midiCtrl = 93;
        voice->inpPostAuxB.source[0].combine = 0;
        voice->inpPostAuxB.source[0].scale = 0x10000;
        voice->inpPostAuxB.numSource = 1;
        voice->inpDoppler.source[0].midiCtrl = 132;
        voice->inpDoppler.source[0].combine = 0;
        voice->inpDoppler.source[0].scale = 0x10000;
        voice->inpDoppler.numSource = 1;
        voice->inpTremolo.numSource = 0;
        voice->midiDirtyFlags = 0x1FFF;
        voice->lfoUsedByInput[0] = 0;
        voice->lfoUsedByInput[1] = 0;
        voice->timeUsedByInput = 0;
    } else {
        CtrlDest (*auxA)[4] = (CtrlDest (*)[4])lbl_80435B74;
        CtrlDest (*auxB)[4] = (CtrlDest (*)[4])lbl_804356F4;

        for (studio = 0; studio < 8; studio++) {
            for (i = 0; i < 4; i++) {
                auxA[studio][i].numSource = 0;
                auxB[studio][i].numSource = 0;
            }
        }

        inpResetGlobalMIDIDirtyFlags();
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void inpTranslateExCtrl(void) {
#include "src/game/people/people_field_fn_80161D20.inc"
}
#else
/* WALL (w_sg2 2026-06-18, measured 99.64% @ opt_level 4): only residual is anonymous
   jumptable @174@ha/l vs named jumptable_80369CB0@ha/l (numeric-vs-named reloc artifact,
   not C-controllable). Real C is active for coverage. NOTE: needs opt_level 4 (opt 0 = 58%). */
/* dont_inline: inpAddCtrl calls this via a real `bl` in retail (confirmed by
 * disassembly @0x80160F10) -- without this, MWCC's -inline auto inlines it
 * there instead (turning the call into a duplicated jump-table dispatch).
 * NOTE: at some OTHER call sites (fn_80161D90/fn_80161E8C) retail fully
 * inlines this same function via a jump table -- another same-callee,
 * different-call-site inlining split; those two remain parked/unmatched. */
#pragma dont_inline on
u8 inpTranslateExCtrl(u8 r3) {
    u32 key = r3 & 0xFF;
    switch (key) {
    case 0x80: return 0x80;
    case 0x81: return 0x82;
    case 0x82: return 0xa0;
    case 0x83: return 0xa1;
    case 0x84: return 0x83;
    case 0x85: return 0x84;
    case 0x86: return 0xa2;
    case 0x87: return 0xa3;
    case 0x88: return 0xa4;
    default:   return r3;
    }
}
#pragma dont_inline reset
#endif
#pragma pop
extern void fn_80160BDC(void);
extern void fn_801603C0(u8 ctrl, u8 channel, u8 set, u8 value); /* inpSetMidiCtrl -- true
                                                                  * signature is all-u8
                                                                  * (verified: callers don't
                                                                  * re-mask already-clean u8
                                                                  * args when this is declared
                                                                  * u8, matching retail) */
inline void inpSetMidiCtrl14(u8 ctrl, u8 channel, u8 set, u16 value) {
    if (channel == 0xFF) {
        return;
    }

    if (ctrl < 64) {
        fn_801603C0(ctrl & 31, channel, set, value >> 7);
        fn_801603C0((ctrl & 31) + 32, channel, set, value & 0x7f);
    } else if (ctrl == 128 || ctrl == 129) {
        fn_801603C0(ctrl & 254, channel, set, value >> 7);
        fn_801603C0((ctrl & 254) + 1, channel, set, value & 0x7f);
    } else if (ctrl == 132 || ctrl == 133) {
        fn_801603C0(ctrl & 254, channel, set, value >> 7);
        fn_801603C0((ctrl & 254) + 1, channel, set, value & 0x7f);
    } else {
        fn_801603C0(ctrl, channel, set, value >> 7);
    }
}
extern u32  salInitDspCtrl(u8 a, u8 b, u8 c);
u32 salExitDspCtrl(void);
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80161D90(void) {
#include "src/game/people/people_field_fn_80161D90.inc"
}
#else
u16 inpGetExCtrl(u8* obj, u8 ctrl) {
    u16 value;
    u8 code = ctrl;

    switch (code) {
    case 0x80: code = 0x80; break;
    case 0x81: code = 0x82; break;
    case 0x82: code = 0xA0; break;
    case 0x83: code = 0xA1; break;
    case 0x84: code = 0x83; break;
    case 0x85: code = 0x84; break;
    case 0x86: code = 0xA2; break;
    case 0x87: code = 0xA3; break;
    case 0x88: code = 0xA4; break;
    }

    switch (code) {
    case 0xA0:
        value = (*(s16*)(obj + 0x1C4) << 1) + 0x2000;
        break;
    case 0xA1:
        value = (*(s16*)(obj + 0x1D0) << 1) + 0x2000;
        break;
    default:
        value = obj[0x121] != 0xFF ? inpGetMidiCtrl(ctrl, obj[0x121], obj[0x122]) : 0;
        break;
    }
    return value;
}
#endif
#pragma pop
void inpSetExCtrl(u8* obj, u8 ctrl, s16 value) {
    u8 code;
    u8 raw;
    u8 channel;

    value = value < 0 ? 0 : value > 0x3FFF ? 0x3FFF : value;
    code = ctrl;
    raw = ctrl;
    switch (raw) {
    case 0x80: code = 0x80; break;
    case 0x81: code = 0x82; break;
    case 0x82: code = 0xA0; break;
    case 0x83: code = 0xA1; break;
    case 0x84: code = 0x83; break;
    case 0x85: code = 0x84; break;
    case 0x86: code = 0xA2; break;
    case 0x87: code = 0xA3; break;
    case 0x88: code = 0xA4; break;
    }
    switch (code) {
    case 0xA0:
    case 0xA1:
        break;
    default:
        channel = obj[0x121];
        if (channel != 0xFF) {
            inpSetMidiCtrl14(raw, channel, obj[0x122], value);
        }
        break;
    }
}
extern u32 lbl_80478BF0;
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162070(void) {
#include "src/game/people/people_field_fn_80162070.inc"
}
#else
u32 fn_80162070(void) {
    u32 temp;

    temp = lbl_80478BF0 * 0xA8351D63;
    lbl_80478BF0 = temp;
    return (temp >> 6) & 0xFFFF;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void sndSin(void) {
#include "src/game/people/people_field_sndSin.inc"
}
#else
extern s16 lbl_80369D20[];
s32 sndSin(u32 angle) {
    u32 a = angle & 0xFFF;
    if (a < 0x400) {
        return lbl_80369D20[(a & 0xFFFF) * 2 / 2];
    }
    if (a < 0x800) {
        u32 idx = 0x3FF - (a & 0x3FF);
        return lbl_80369D20[idx];
    }
    if (a < 0xC00) {
        u32 idx = (a & 0x3FF) * 2 / 2;
        s16 v = lbl_80369D20[idx];
        return (s16)(-v);
    }
    {
        u32 idx = 0x3FF - (a & 0x3FF);
        s16 v = lbl_80369D20[idx];
        return (s16)(-v);
    }
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void sndBSearch(void) {
#include "src/game/people/people_field_fn_80162118.inc"
}
#else
void* sndBSearch(u8* key, u8* base, s32 count, u32 size, PeopleCmpFn cmp) {
    s32 lo, hi, mid;
    s32 r;
    u8* elem;

    if (count != 0) {
        lo = 1;
        hi = count;
        do {
            mid = (lo + hi) >> 1;
            elem = base + size * (mid - 1);
            r = cmp(key, elem);
            if (r == 0) {
                return elem;
            }
            if (r < 0) {
                hi = mid - 1;
            }
            if (((u32)r >> 31) == 0) {
                lo = mid + 1;
            }
        } while (lo <= hi);
    }
    return 0;
}
#endif
#pragma pop
extern u32  OSEnableInterrupts(void);
extern u32  OSGetTick(void);
extern void PPCSync(void);
extern void AIStartDMA(void);
extern void AIStopDMA(void);
extern u32  fn_800AE794(void);
extern void DSPSendMailToDSP(u32 a);
extern void DSPInit(void);
extern void fn_800AE8A4(void);
extern void fn_800AE8EC(void);
extern u32  fn_800AE92C(void);
extern void DSPAddTask(u8* ptr);
extern f64 pow(f64 base, f64 exp); /* MSL pow(double,double); Colosseum's
                                             * pre-2.0.1 adsrConvertTimeCents calls
                                             * this (not powf) -- confirmed by
                                             * disassembly: args loaded via lfd
                                             * (double) not lfs, and result is
                                             * frsp'd down to single afterward. */
extern void fn_8015B250(u32, u32);
extern u32 ReverbHICreate(u8* obj, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6);
extern u32 ReverbHIModify(u8* obj, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6);
extern void ReverbHICallback(u32 a, u32 b, u32 c, u8* d);
extern void DCStoreRange(void* addr, u32 nBytes);
extern void AIRegisterDMACallback(u32 a);
extern void AIInitDMA(u8* ptr, u32 size);
extern u32  ARGetBaseAddress(void);
extern u32  ARGetSize(void);
extern void fn_800AE630(void);
extern u32  ARQGetChunkSize(void);
extern u32  adsrConvertTimeCents(s32 tc); /* verified true signature via synth_adsr.c reference + callsite (see below) */
extern void salActivateStudio();
extern u8 lbl_8047B033;
extern u8 lbl_8047B034;
extern u8 lbl_8047B035;
extern void dataInit(u32 smpBase, u32 smpLength);
extern void dataExit(void);
extern void synthExit(void);
extern void fn_8015AAA0(u32 studio);
extern void salActivateVoice(u8* ptr, u8 unused2);
typedef union HwVolumeStudio {
    struct {
        u32 allocation;
        u8 pad_04[0x24];
        u32 auxAllocation;
        u8 pad_2C[0x24];
        u8 state;
        u8 isMaster;
        u8 numInputs;
        u8 pad_53;
        s32 type;
        u8 pad_58[0x64];
    } named;
    u8 bytes[0xBC];
} HwVolumeStudio;
extern HwVolumeStudio lbl_80447E60[];
extern void fn_8015D7D0(void);
extern void fn_801629A4(u32 index, u8 value);
extern void fn_801629D0(u32 index, u8 value);
extern void hwSetITDMode(u32 index, u8 flag);
extern void fn_801632B4(u8* dst, u8* src, u32 size, u32 priority, u32 callbackArg0, u32 callbackArg1);
extern void fn_80163490(void);
extern void fn_801634A8(u32 size);
extern void fn_80163794(void);
extern void aramSetUploadCallback(u8* ptr, u32 size);
extern u32  fn_80163810(u32 ptr, u32 size);
extern void fn_80163BCC(u8* a, u32 b);
extern u8   fn_80163CA8();
extern u32  aramGetStreamBufferAddress(u32 idx, u32 *out);
extern void aramFreeStreamBuffer();
extern u32  salInitAi(void(*fnptr)(void), u32 d, u32 a);
extern void fn_801640C4(void);
extern u32  salExitAi(void);
extern u32  fn_80164148(u32 d);
extern u32  fn_80164204(void);
extern void fn_80164324(void);
extern void hwEnableIrq(void);
extern void hwDisableIrq(void);
extern void hwInitIrq(void);
extern u32 salGetStartDelay(void);
extern u32  fn_801643D8(u32 size);
extern void fn_80164400(u32 a);
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801621BC(void) {
#include "src/game/people/people_field_fn_801621BC.inc"
}
#else
void fn_801621BC(u32* ptr) { *ptr <<= 8; }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void sndConvertTicks(void) {
#include "src/game/people/people_field_sndConvertTicks.inc"
}
#else
void sndConvertTicks(u32* ptr, u32 divisor) {
    extern u32 synthGetTicksPerSecond(u32 a);
    u32 result = synthGetTicksPerSecond(divisor);
    *ptr = ((*ptr << 16) / result * 0x3E8) >> 5;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162214(void) {
#include "src/game/people/people_field_fn_80162214.inc"
}
#else
/* identity: sndConvert2Ms (MusyX runtime, snd_service.c) */
u32 fn_80162214(u32 time) { return time / 256; }
#endif
#pragma pop
extern u8 lbl_8047AF18;
extern u8 lbl_8047B05D;
extern u8 lbl_8047B05E;
extern u8 lbl_8047B05F;
extern u32 lbl_8047B024;
extern void fn_8014E7CC(void);
extern u32 fn_80164398(void);
extern u32 fn_801643B8(void);
extern u8* salAiGetDest(void);
extern void salCtrlDsp(u32 arg);
extern void salHandleAuxProcessing(void);
extern void fn_8016245C(u8 offset);
extern void fn_801496A0(u32 ticks);
extern void synthHandle(u32 ticks);
extern void fn_8015F620(void);
extern void fn_8014DF20(void);
extern void vsSampleUpdates(void);

typedef struct HwIrqVoice {
    u8 pad_00[0x24];
    u32 changed[5];
} HwIrqVoice;

void snd_handle_irq(void) {
    u8 frame;
    u8 voice;
    u8 i;

    if (lbl_8047AF18 == 0) {
        return;
    }

    fn_8014E7CC();
    fn_80164398();
    salCtrlDsp((u32)salAiGetDest());
    fn_801643B8();
    fn_80164398();
    salHandleAuxProcessing();
    fn_801643B8();
    fn_80164398();

    lbl_8047B05F ^= 1;
    lbl_8047B05E = (lbl_8047B05E + 1) % 3;
    for (voice = 0; voice < lbl_8047B05D; voice++) {
        for (i = 0; i < 5; i++) {
            ((HwIrqVoice*)((u8*)lbl_8047B024 + voice * 0xF4))->changed[i] = 0;
        }
    }
    fn_801643B8();

    for (frame = 0; frame < 5; frame++) {
        fn_80164398();
        fn_8016245C(frame);
        fn_801496A0(0x100);
        synthHandle(0x100);
        fn_801643B8();
    }

    fn_80164398();
    fn_8016245C(0);
    fn_8015F620();
    fn_801643B8();
    fn_80164398();
    fn_8014DF20();
    fn_801643B8();
    fn_80164398();
    vsSampleUpdates();
    fn_801643B8();
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwInit(void) {
#include "src/game/people/people_field_fn_80162370.inc"
}
#else
extern u8  lbl_8047B05E;
extern u8  lbl_8047B05F;
extern void snd_handle_irq(void);
u32 hwInit(u32 a, u16 b, u32 c, u32 d) {
    extern u32 lbl_8047B028;
    hwInitIrq();
    lbl_8047B05F = 0;
    lbl_8047B05E = 0;
    lbl_8047B028 = 0;
    if (salInitAi(snd_handle_irq, d, a) != 0
     && salInitDspCtrl(b, c, (u32)(d & 1)) != 0
     && fn_80164148(d) != 0) {
        hwEnableIrq();
        fn_801640C4();
        return 0;
    }
    return -1;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void hwExit(void) {
#include "src/game/people/people_field_hwExit.inc"
}
#else
void hwExit(void) {
    hwDisableIrq();
    fn_80164204();
    salExitDspCtrl();
    salExitAi();
    hwEnableIrq();
    fn_80164324();
}
#endif
extern u8  lbl_8047B050;
extern u32 lbl_8047B028;
extern u32 lbl_8047B024;
typedef struct PeopleFieldMoveSlot {
    u8 pad_00[0x1C];       /* 0x00 */
    u32 field_1C;          /* 0x1C */
    u8 pad_20[0x4];        /* 0x20 */
    u32 flags_24[0x13];    /* 0x24 */
    u16 field_70;          /* 0x70 */
    u8 pad_72[0x1E];       /* 0x72 */
    u8 field_90;           /* 0x90 */
    u8 pad_91[0x3];        /* 0x91 */
    u32 field_94;          /* 0x94 */
    u32 field_98;          /* 0x98 */
    u8 field_9C;           /* 0x9C */
    u8 pad_9D[0x3];        /* 0x9D */
    u8 field_A0;           /* 0xA0 */
    u8 pad_A1[0x2B];       /* 0xA1 */
    u16 field_CC;          /* 0xCC */
    u16 field_CE;          /* 0xCE */
    u16 field_D0;          /* 0xD0 */
    u16 field_D2;          /* 0xD2 */
    u8 field_D4;           /* 0xD4 */
    u8 pad_D5[0x13];       /* 0xD5 */
    u32 field_E8;          /* 0xE8 */
    u8 active;             /* 0xEC */
    u8 field_ED;           /* 0xED */
    u8 field_EE;           /* 0xEE */
    u8 pad_EF;             /* 0xEF */
    u32 field_F0;          /* 0xF0 */
} PeopleFieldMoveSlot;

#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8016245C(void) {
#include "src/game/people/people_field_fn_8016245C.inc"
}
#else
void fn_8016245C(u8 val) { lbl_8047B050 = val; }
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80162464(void) {
#include "src/game/people/people_field_fn_80162464.inc"
}
#else
u8 fn_80162464(void) { return lbl_8047B050; }
#endif
#pragma pop
extern u32 lbl_8047B024;
/* hwIsActive (hardware.c) -- cross-TU boundary: synthvoice.c's voiceBlock
 * calls this via a real `bl`, not inlined, in retail. dont_inline keeps
 * this residual TU's auto-inliner from collapsing it into same-file
 * callers that originally lived in a different TU (validated trick). */
#pragma dont_inline on
u32 fn_8016246C(u32 index) {
    PeopleFieldMoveSlot* entries = (PeopleFieldMoveSlot*)lbl_8047B024;

    return entries[index].active != 0;
}
#pragma dont_inline reset
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8016248C(void) {
#include "src/game/people/people_field_fn_8016248C.inc"
}
#else
void fn_8016248C(u32 val) { lbl_8047B028 = val; }
#endif
#pragma pop
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_80162494(void) {
#include "src/game/people/people_field_fn_80162494.inc"
}
#else
/* hwSetPriority (hardware.c) -- cross-TU boundary: synthvoice.c's
 * voiceSetPriority calls this via a real `bl`, not inlined, in retail (see
 * fn_8016246C's dont_inline note). */
#pragma dont_inline on
void fn_80162494(u32 index, u32 val) {
    PeopleFieldMoveSlot* entries = (PeopleFieldMoveSlot*)lbl_8047B024;

    entries[index].field_1C = val;
}
#pragma dont_inline reset
#endif
#pragma pop
void hwInitSamplePlayback(u32 index, u16 value70, void* words74, u32 resetState, u32 value1C, u32 value18, u32 initFlags, u8 setupFlag) {
    typedef struct HardwareSampleInfo {
        u32 words[8];
    } HardwareSampleInfo;
    typedef struct {
        u8 pad_00[0x18];      /* 0x00 */
        u32 field_18;         /* 0x18 */
        u32 field_1C;         /* 0x1C */
        u8 pad_20[0x4];       /* 0x20 */
        u32 flags_24[0x13];   /* 0x24 */
        u16 field_70;         /* 0x70 */
        u8 pad_72[0x2];       /* 0x72 */
        HardwareSampleInfo sample; /* 0x74 */
        u32 field_94;         /* 0x94 */
        u32 field_98;         /* 0x98 */
        u8 field_9C;          /* 0x9C */
        u8 pad_9D[0x3];       /* 0x9D */
        u8 field_A0;          /* 0xA0 */
        u8 pad_A1[0x3];       /* 0xA1 */
        u8 field_A4;          /* 0xA4 */
        u8 pad_A5[0x13];      /* 0xA5 */
        u32 field_B8;         /* 0xB8 */
        u32 field_BC;         /* 0xBC */
        u16 field_C0;         /* 0xC0 */
        u8 pad_C2[0x2];       /* 0xC2 */
        u32 field_C4;         /* 0xC4 */
        u8 pad_C8[0x1C];      /* 0xC8 */
        u8 bytes_E4[4];       /* 0xE4 */
        u8 pad_E8[0x8];       /* 0xE8 */
        u32 field_F0;         /* 0xF0 */
    } PeopleFieldState;
    extern u8 lbl_8047B050;
    u8 i;
    u32 flags = 0;

#define HW_PLAYBACK_VOICES (*(PeopleFieldState**)&lbl_8047B024)
    for (i = 0; i <= lbl_8047B050; i++) {
        flags |= HW_PLAYBACK_VOICES[index].flags_24[i] & 0x20;
        HW_PLAYBACK_VOICES[index].flags_24[i] = 0;
    }

    HW_PLAYBACK_VOICES[index].flags_24[0] = flags;
    HW_PLAYBACK_VOICES[index].field_1C = value1C;
    HW_PLAYBACK_VOICES[index].field_18 = value18;
    HW_PLAYBACK_VOICES[index].field_F0 = 0;
    HW_PLAYBACK_VOICES[index].field_70 = value70;

    HW_PLAYBACK_VOICES[index].sample = *(HardwareSampleInfo*)words74;

    if (resetState != 0) {
        HW_PLAYBACK_VOICES[index].field_A4 = 0;
        HW_PLAYBACK_VOICES[index].field_B8 = 0;
        HW_PLAYBACK_VOICES[index].field_BC = 0;
        HW_PLAYBACK_VOICES[index].field_C0 = 0x7FFF;
        HW_PLAYBACK_VOICES[index].field_C4 = 0;
    }

    HW_PLAYBACK_VOICES[index].bytes_E4[0] = 0xFF;
    HW_PLAYBACK_VOICES[index].bytes_E4[1] = 0xFF;
    HW_PLAYBACK_VOICES[index].bytes_E4[2] = 0xFF;
    HW_PLAYBACK_VOICES[index].bytes_E4[3] = 0xFF;

    if (initFlags != 0) {
        fn_801629A4(index, 0);
        fn_801629D0(index, 1);
    }

    hwSetITDMode(index, setupFlag);
#undef HW_PLAYBACK_VOICES
}
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwBreak(void) {
#include "src/game/people/people_field_fn_8016265C.inc"
}
#else
/* hardware.c -- see fn_8016246C's dont_inline note (cross-TU boundary vs.
 * voiceBlock/synthvoice.c). */
#pragma dont_inline on
void hwBreak(u32 index) {
    extern u32 lbl_8047B024;
    extern u8 lbl_8047B050;
    u32 offset = index * 0xF4;
    PeopleFieldMoveSlot* entry;
    u8* p;

    entry = (PeopleFieldMoveSlot*)((u8*)lbl_8047B024 + offset);
    if (entry->active == 1 && lbl_8047B050 == 0) {
        entry->field_EE = 1;
    }
    p = (u8*)lbl_8047B024 + offset;
    p += (u32)lbl_8047B050 * 4;
    *(u32*)(p + 0x24) |= 0x20;
}
#pragma dont_inline reset
#endif
#pragma pop
void hwSetADSR(u32 index, void* ptr, u8 mode) {
    typedef struct {
        u8 pad_00[0x18];      /* 0x00 */
        u32 field_18;         /* 0x18 */
        u32 field_1C;         /* 0x1C */
        u8 pad_20[0x4];       /* 0x20 */
        u32 flags_24[0x13];   /* 0x24 */
        u16 field_70;         /* 0x70 */
        u8 pad_72[0x2];       /* 0x72 */
        u32 words_74[0x8];    /* 0x74 */
        u32 field_94;         /* 0x94 */
        u32 field_98;         /* 0x98 */
        u8 field_9C;          /* 0x9C */
        u8 pad_9D[0x3];       /* 0x9D */
        u8 field_A0;          /* 0xA0 */
        u8 pad_A1[0x3];       /* 0xA1 */
        u8 field_A4;          /* 0xA4 */
        u8 pad_A5[0x13];      /* 0xA5 */
        u32 field_B8;         /* 0xB8 */
        u32 field_BC;         /* 0xBC */
        u16 field_C0;         /* 0xC0 */
        u8 pad_C2[0x2];       /* 0xC2 */
        u32 field_C4;         /* 0xC4 */
        u8 pad_C8[0x2];       /* 0xC8 */
        u8 field_CA;          /* 0xCA */
        u8 pad_CB[0x19];      /* 0xCB */
        u8 bytes_E4[4];       /* 0xE4 */
        u8 pad_E8[0x8];       /* 0xE8 */
        u32 field_F0;         /* 0xF0 */
    } PeopleFieldState;
    typedef struct {
        u16 field_00;         /* 0x00 */
        u16 field_02;         /* 0x02 */
        u16 field_04;         /* 0x04 */
        u16 field_06;         /* 0x06 */
    } PeopleFieldMode0Args;
    typedef struct {
        u32 field_00;         /* 0x00 */
        u32 field_04;         /* 0x04 */
        u16 field_08;         /* 0x08 */
        u16 field_0A;         /* 0x0A */
    } PeopleFieldMode12Args;
    extern u8 lbl_8036944C[];

#define HW_ADSR_VOICES (*(PeopleFieldState**)&lbl_8047B024)
    switch (mode) {
    case 0: {
        PeopleFieldMode0Args* args = ptr;
        u32 v;
        HW_ADSR_VOICES[index].field_A4 = 0;
        HW_ADSR_VOICES[index].field_B8 = args->field_00;
        HW_ADSR_VOICES[index].field_BC = args->field_02;
        v = args->field_04 << 3;
        if (v > 0x7FFF) {
            v = 0x7FFF;
        }
        HW_ADSR_VOICES[index].field_C0 = (u16)v;
        HW_ADSR_VOICES[index].field_C4 = args->field_06;
        break;
    }
    case 1:
    case 2:
        {
        PeopleFieldMode12Args* args = ptr;
        HW_ADSR_VOICES[index].field_A4 = 1;
        HW_ADSR_VOICES[index].field_CA = 0;
        if (mode == 1) {
            HW_ADSR_VOICES[index].field_B8 = (u16)adsrConvertTimeCents(args->field_00);
            HW_ADSR_VOICES[index].field_BC = (u16)adsrConvertTimeCents(args->field_04);
            {
                s32 idx = args->field_08 >> 2;
                if ((u32)idx > 0x3FF) {
                    idx = 0x3FF;
                }
                HW_ADSR_VOICES[index].field_C0 = (u16)(0xC1 - lbl_8036944C[idx]);
            }
        } else {
            HW_ADSR_VOICES[index].field_B8 = (u16)args->field_00;
            HW_ADSR_VOICES[index].field_BC = (u16)args->field_04;
            HW_ADSR_VOICES[index].field_C0 = args->field_08;
        }
        HW_ADSR_VOICES[index].field_C4 = args->field_0A;
        }
    }

    HW_ADSR_VOICES[index].flags_24[0] |= 0x10;
#undef HW_ADSR_VOICES
}
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_80162858(void) {
#include "src/game/people/people_field_fn_80162858.inc"
}
#else
void fn_80162858(u32 index, u32 val1, u32 val2) {
    extern u32 lbl_8047B024;
    u32 offset = index * 0xF4;
    {
        PeopleFieldMoveSlot* entry1 = (PeopleFieldMoveSlot*)((u8*)lbl_8047B024 + offset);
        entry1->field_94 = val1;
    }
    {
        PeopleFieldMoveSlot* entry2 = (PeopleFieldMoveSlot*)((u8*)lbl_8047B024 + offset);
        entry2->field_98 = val2;
    }
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_80162878(void) {
#include "src/game/people/people_field_fn_80162878.inc"
}
#else
u8 fn_80162878(u32 index) {
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    return entries[index].field_9C;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_8016288C(void) {
#include "src/game/people/people_field_fn_8016288C.inc"
}
#else
u8 fn_8016288C(u32 index) {
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    return entries[index].field_90;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_801628A0(void) {
#include "src/game/people/people_field_fn_801628A0.inc"
}
#else
u16 fn_801628A0(u32 index) {
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    return entries[index].field_70;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_801628B4(void) {
#include "src/game/people/people_field_fn_801628B4.inc"
}
#else
void fn_801628B4(u32 index, u8 val) {
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    entries[index].field_A0 = val;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwStart(void) {
#include "src/game/people/people_field_fn_801628C8.inc"
}
#else
void hwStart(u32 index, u8 unused2) {
#define PF (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024)
    PF[index].field_D4 = lbl_8047B050;
    salActivateVoice((u8*)&PF[index], unused2);
#undef PF
}
#endif
#pragma pop
extern u32 lbl_8047B024;
extern u8 lbl_8047B050;
void hwKeyOff(u32 index) {
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    entries[index].flags_24[lbl_8047B050] |= 0x40;
}
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwSetPitch(void) {
#include "src/game/people/people_field_fn_8016292C.inc"
}
#else
void hwSetPitch(u32 index, u16 value) {
    typedef struct {
        u8 pad_00[0x24];      /* 0x00 */
        u32 words_24[0x30];   /* 0x24 */
        u8 activeWordIndex;   /* 0xE4 */
        u8 pad_E5[0x0F];      /* 0xE5 */
    } PeopleFieldState;
    extern u32 lbl_8047B024;
    extern u8 lbl_8047B050;
    PeopleFieldState* entries = (*(PeopleFieldState* volatile*)&lbl_8047B024);
    PeopleFieldState* entry = &entries[index];
    u32 scaledValue;

    if ((u16)value >= 0x4000) {
        value = 0x3FFF;
    }

    if (entry->activeWordIndex != 0xFF) {
        scaledValue = (u16)value << 4;
        if (entry->words_24[5 + entry->activeWordIndex] == scaledValue) {
            return;
        }
    }

    scaledValue = (u16)value << 4;
    entry->words_24[5 + lbl_8047B050] = scaledValue;
    entry->words_24[lbl_8047B050] |= 8;
    entry->activeWordIndex = lbl_8047B050;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801629A4(void) {
#include "src/game/people/people_field_fn_801629A4.inc"
}
#else
void fn_801629A4(u32 index, u8 value) {
    extern u16 lbl_80478BF8;
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    entries[index].field_CC = (&lbl_80478BF8)[(u8)value];
    entries[index].flags_24[0] |= 0x100;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801629D0(void) {
#include "src/game/people/people_field_fn_801629D0.inc"
}
#else
void fn_801629D0(u32 index, u8 value) {
    extern u16 lbl_80478C00;
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    entries[index].field_CE = (&lbl_80478C00)[(u8)value];
    entries[index].flags_24[0] |= 0x80;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwSetITDMode(void) {
#include "src/game/people/people_field_fn_801629FC.inc"
}
#else
/* peopleFieldMemSetup: flag==0 marks NPC slot active (set hi bit of +0xF0, +0xD0/+0xD2=0x10);
 * else clears the hi bit. Base array lbl_8047B024 (stride 0xF4, same as fn_801629A4/D0) is
 * re-read per access (volatile reinterpret) to match the target. byte-match verified 22/22. */
void hwSetITDMode(u32 index, u8 flag) {
    extern u32 lbl_8047B024;
#define PF (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024)
    if (flag == 0) {
        PF[index].field_F0 |= 0x80000000;
        PF[index].field_D0 = 0x10;
        PF[index].field_D2 = 0x10;
    } else {
        PF[index].field_F0 &= ~0x80000000u;
    }
#undef PF
}
#endif
#pragma pop
typedef union HwVolumeInfo {
    struct {
        f32 volL;
        f32 volR;
        f32 volS;
        f32 volAuxAL;
        f32 volAuxAR;
        f32 volAuxAS;
        f32 volAuxBL;
        f32 volAuxBR;
        f32 volAuxBS;
    } named;
    f32 values[9];
} HwVolumeInfo;

typedef struct HwVolumeVoice {
    u8 pad_00[0x24];
    u32 changed[5];
    u32 pitch[5];
    u16 volL;
    u16 volR;
    u16 volS;
    u16 volLa;
    u16 volRa;
    u16 volSa;
    u16 volLb;
    u16 volRb;
    u16 volSb;
    u8 pad_5E[0x72];
    u16 itdShiftL;
    u16 itdShiftR;
    u8 pad_D4[0x10];
    struct {
        u8 pitch;
        u8 vol;
        u8 volA;
        u8 volB;
    } lastUpdate;
    u32 virtualSampleID;
    u8 state;
    u8 postBreak;
    u8 startupBreak;
    u8 studio;
    u32 flags;
} HwVolumeVoice;

static void hwSetupITD(HwVolumeVoice* voice, u8 pan) {
    voice->itdShiftL = lbl_80273448[pan];
    voice->itdShiftR = 32 - lbl_80273448[pan];
    voice->changed[0] |= 0x200;
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwSetVolume(u32 voice, u32 table, f32 volume, u32 pan, u32 span,
                 f32 auxA, f32 auxB) {
    HwVolumeInfo volumeInfo;
    u16 left;
    u16 right;
    u16 surround;
    HwVolumeVoice* dspVoice = (HwVolumeVoice*)lbl_8047B024;
    HwVolumeVoice* dspVoicePtr = &dspVoice[voice];

    {
        f32 one = lbl_8047D4D8;
        if (volume >= one) {
            volume = one;
        }
    }
    {
        f32 one = lbl_8047D4D8;
        if (auxA >= one) {
            auxA = one;
        }
    }
    {
        f32 one = lbl_8047D4D8;
        if (auxB >= one) {
            auxB = one;
        }
    }

    {
        u32 hasITD = (dspVoicePtr->flags & 0x80000000) != 0;
        u32 dpl2 =
            lbl_80447E60[dspVoicePtr->studio].named.type == 1;

        salCalcVolume(table, volumeInfo.values, volume, pan, span, auxA, auxB,
                      hasITD, dpl2);
    }

    left = lbl_8047D4DC * volumeInfo.named.volL;
    right = lbl_8047D4DC * volumeInfo.named.volR;
    surround = lbl_8047D4DC * volumeInfo.named.volS;
    if (dspVoicePtr->lastUpdate.vol == 0xFF || dspVoicePtr->volL != left ||
        dspVoicePtr->volR != right || dspVoicePtr->volS != surround) {
        dspVoicePtr->volL = left;
        dspVoicePtr->volR = right;
        dspVoicePtr->volS = surround;
        dspVoicePtr->changed[0] |= 1;
        dspVoicePtr->lastUpdate.vol = 0;
    }

    left = lbl_8047D4DC * volumeInfo.named.volAuxAL;
    right = lbl_8047D4DC * volumeInfo.named.volAuxAR;
    surround = lbl_8047D4DC * volumeInfo.named.volAuxAS;
    if (dspVoicePtr->lastUpdate.volA == 0xFF || dspVoicePtr->volLa != left ||
        dspVoicePtr->volRa != right || dspVoicePtr->volSa != surround) {
        dspVoicePtr->volLa = left;
        dspVoicePtr->volRa = right;
        dspVoicePtr->volSa = surround;
        dspVoicePtr->changed[0] |= 2;
        dspVoicePtr->lastUpdate.volA = 0;
    }

    left = lbl_8047D4DC * volumeInfo.named.volAuxBL;
    right = lbl_8047D4DC * volumeInfo.named.volAuxBR;
    surround = lbl_8047D4DC * volumeInfo.named.volAuxBS;
    if (dspVoicePtr->lastUpdate.volB == 0xFF || dspVoicePtr->volLb != left ||
        dspVoicePtr->volRb != right || dspVoicePtr->volSb != surround) {
        dspVoicePtr->volLb = left;
        dspVoicePtr->volRb = right;
        dspVoicePtr->volSb = surround;
        dspVoicePtr->changed[0] |= 4;
        dspVoicePtr->lastUpdate.volB = 0;
    }

    if (dspVoicePtr->flags & 0x80000000) {
        hwSetupITD(dspVoicePtr, pan >> 16);
    }
}
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162D18(void) {
#include "src/game/people/people_field_fn_80162D18.inc"
}
#else
void fn_80162D18(u32 index) {
    extern u32 lbl_8047B024;
    extern void salDeactivateVoice(void* ptr);
    PeopleFieldMoveSlot* entries = (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);

    salDeactivateVoice((u8*)&entries[index]);
}
#endif
#pragma pop
typedef struct PeopleStudioState {
    u8 pad_00[0xAC];       /* 0x00 */
    u32 field_AC;          /* 0xAC */
    u32 field_B0;          /* 0xB0 */
    u32 field_B4;          /* 0xB4 */
    u32 field_B8;          /* 0xB8 */
} PeopleStudioState;

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwSetAUXProcessingCallbacks(void) {
#include "src/game/people/people_field_fn_80162D44.inc"
}
#else
void hwSetAUXProcessingCallbacks(u8 index, u32 a, u32 b, u32 c, u32 d) {
    PeopleStudioState* entries = (PeopleStudioState*)lbl_80447E60;
    entries[(u8)index].field_AC = a;
    entries[(u8)index].field_B4 = b;
    entries[(u8)index].field_B0 = c;
    entries[(u8)index].field_B8 = d;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162D6C(void) {
#include "src/game/people/people_field_fn_80162D6C.inc"
}
#else
void fn_80162D6C(void) { salActivateStudio(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162D8C(void) {
#include "src/game/people/people_field_fn_80162D8C.inc"
}
#else
void fn_80162D8C(u32 studio) { fn_8015AAA0(studio); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162DAC(void) {
#include "src/game/people/people_field_fn_80162DAC.inc"
}
#else
void fn_80162DAC(u8 index, u32 arg1) {
    extern u32 fn_8015D54C(void*, void*);
    PeopleStudioState* entries = (PeopleStudioState*)lbl_80447E60;
    fn_8015D54C((u8*)&entries[(u8)index], (void*)arg1);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162DE0(void) {
#include "src/game/people/people_field_fn_80162DE0.inc"
}
#else
void fn_80162DE0(u8 index, u32 arg1) {
    extern u32 fn_8015D5F4(void*, void*);
    PeopleStudioState* entries = (PeopleStudioState*)lbl_80447E60;
    fn_8015D5F4((u8*)&entries[(u8)index], (void*)arg1);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80162E14(void) {
#include "src/game/people/people_field_fn_80162E14.inc"
}
#else
/* STAGED SEED (Claude Opus 2026-06-16) -- logic verified vs target; NOT 100%.
 * At #pragma optimization_level 4 the prologue is gone (volatile-only leaf)
 * and it reaches ~58-60%. Residual is compute-block (case 0/1/4/5) reg-alloc:
 *   (1) target recomputes the entry base into r3 (`add r3,r6,r5`) then loads
 *       dim_78/dim_20 via r3, freeing r4 for `lo`; ours loads via the saved r4.
 *   (2) target emits `cmplwi r4,2; mulli r3,r0,0xe; bltlr` (return m if lo<2);
 *       ours inverts to `mulli r5,...; ... bgelr` (different reg + polarity).
 * Cracking needs permuter/band lever search on operand order + the base-recompute
 * binding. The TU default for this block was opt 0 (prologue/spill => 18.5%);
 * opt 4 is correct. */
#pragma optimization_level 4
u32 fn_80162E14(u32 idx) {
    /* Local view of the 0xf4-stride people-field entry; only the fields
     * touched here are named. */
    typedef struct PeopleFieldEntry_E14 {
        u8  _00[0x20];   /* 0x00 */
        u32 dim_20;      /* 0x20 */
        u8  _24[0x54];   /* 0x24 */
        u32 dim_78;      /* 0x78 */
        u8  _7C[0x14];   /* 0x7C */
        u8  kind_90;     /* 0x90 */
        u8  _91[0x5B];   /* 0x91 */
        u8  flag_ec;     /* 0xEC */
        u8  _ED[0x07];   /* 0xED ... 0xF4 */
    } PeopleFieldEntry_E14;
    extern u32 lbl_8047B024;
    PeopleFieldEntry_E14* entries;

    entries = (*(PeopleFieldEntry_E14* volatile*)&lbl_8047B024);

    if (entries[idx].flag_ec != 2) {
        return 0;
    }
    switch (entries[idx].kind_90) {
    case 0:
    case 1:
    case 4:
    case 5: {
        PeopleFieldEntry_E14* q = (PeopleFieldEntry_E14*)((u32)entries + idx * 0xf4);
        u32 small = q->dim_78;
        u32 big = q->dim_20;
        u32 m = ((big - (small << 1)) >> 4) * 0xe;
        u32 lo = big & 0xF;
        if (lo >= 2) {
            m = lo + m;
            m -= 2;
        }
        return m;
    }
    case 3:
        return entries[idx].dim_20 - entries[idx].dim_78;
    case 2:
        return entries[idx].dim_20 - (entries[idx].dim_78 >> 1);
    default:
        return idx;
    }
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwFlushStream(void) {
#include "src/game/people/people_field_fn_80162EB8.inc"
}
#else
void hwFlushStream(u8* dstBase, u32 srcOffset, u32 size, u32 streamIndex, u32 arg7, u32 arg8) {
    u32 unusedOut;
    u8* srcBase = (u8*)aramGetStreamBufferAddress(streamIndex, &unusedOut);
    u8* dst;

    size += srcOffset & 0x1F;
    srcOffset &= ~0x1F;
    dst = dstBase + srcOffset;
    size = (size + 0x1F) & ~0x1F;

    DCStoreRange(dst, size);
    aramUploadData(dst, srcBase + srcOffset, size, 1, arg7, arg8);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162F48(void) {
#include "src/game/people/people_field_fn_80162F48.inc"
}
#else
void fn_80162F48(void) { fn_80163CA8(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162F68(void) {
#include "src/game/people/people_field_fn_80162F68.inc"
}
#else
void fn_80162F68(void) { aramFreeStreamBuffer(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80162F88(void) {
#include "src/game/people/people_field_fn_80162F88.inc"
}
#else
void fn_80162F88(void* a) {
    aramGetStreamBufferAddress((u32)a, 0);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80162FAC(void) {
#include "src/game/people/people_field_fn_80162FAC.inc"
}
#else
void* fn_80162FAC(void* address) { return address; }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwFrq2Pitch(void) {
#include "src/game/people/people_field_fn_80162FB0.inc"
}
#else
typedef struct PeopleFieldMoveScale {
    u32 divisor; /* 0x00 */
} PeopleFieldMoveScale;

u32 hwFrq2Pitch(u32 value) {
    PeopleFieldMoveScale* scale = (PeopleFieldMoveScale*)lbl_80434C50;

    return __cvt_fp2unsigned((lbl_8047D4E0 * (f32)value) / (f32)scale->divisor);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_8016300C(void) {
#include "src/game/people/people_field_fn_8016300C.inc"
}
#else
void fn_8016300C(u32 a, u32 b) {
    extern void fn_801634A8(u32 x);
    fn_801634A8(b);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163030(void) {
#include "src/game/people/people_field_fn_80163030.inc"
}
#else
void fn_80163030(void) { fn_80163794(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163050(void) {
#include "src/game/people/people_field_fn_80163050.inc"
}
#else
typedef struct PeopleFieldMoveCommand {
    u32 field_00;          /* 0x00 */
    u32 packedSizeWord;    /* 0x04: high byte type, low 24-bit payload */
} PeopleFieldMoveCommand;

void fn_80163050(u32** src, u32* out) {
    extern u32 fn_80163810(u32 a, u32 b);
    PeopleFieldMoveCommand* command = (PeopleFieldMoveCommand*)*src;
    u32 val = command->packedSizeWord;
    u32 type = val >> 24;
    u32 payload = val & 0xFFFFFF;
    switch (type) {
        case 0: case 1: case 4: case 5:
            payload = (payload + 13) / 7 * 4 & ~7u;
            break;
        case 2:
            payload = payload << 1;
            break;
    }
    *out = fn_80163810(*out, payload);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801630E4(void) {
#include "src/game/people/people_field_fn_801630E4.inc"
}
#else
void fn_801630E4(u8* ptr, u32 size) { aramSetUploadCallback(ptr, size); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163104(void) {
#include "src/game/people/people_field_fn_80163104.inc"
}
#else
void fn_80163104(u8* src, u8* dest) {
    extern void fn_80163BCC(u8* a, u32 b);
    PeopleFieldMoveCommand* command = (PeopleFieldMoveCommand*)src;
    u32 val = command->packedSizeWord;
    u32 type = val >> 24;
    u32 payload = val & 0xFFFFFF;
    switch (type) {
        case 0: case 1: case 4: case 5:
            payload = (payload + 13) / 7 * 4 & ~7u;
            break;
        case 2:
            payload = payload << 1;
            break;
    }
    fn_80163BCC(dest, payload);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163188(void) {
#include "src/game/people/people_field_fn_80163188.inc"
}
#else
void fn_80163188(void) { fn_80163490(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801631A8(void) {
#include "src/game/people/people_field_fn_801631A8.inc"
}
#else
void fn_801631A8(void) {}
#endif
#pragma pop
extern u32 lbl_8047B054;
extern u32 lbl_8047B058;
extern u32 lbl_8047B014;
#pragma push
typedef struct { u32 a; u32 b; } fn_801631AC_Pair;

void fn_801631AC(fn_801631AC_Pair* src) {
    *(fn_801631AC_Pair*)&lbl_8047B054 = *src;
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801631C0(void) {
#include "src/game/people/people_field_fn_801631C0.inc"
}
#else
void fn_801631C0(void) { lbl_8047B014 = 0; }
#endif
#pragma pop
extern u32 lbl_8047B024;

u32 hwGetVirtualSampleID(u32 index) {
    PeopleFieldMoveSlot* entries = (PeopleFieldMoveSlot*)lbl_8047B024;
    PeopleFieldMoveSlot* entry = &entries[index];

    if (entry->active == 0) {
        return -1;
    } else {
        return entry->field_E8;
    }
}
#pragma push
#pragma optimization_level 2
#if 0
asm void fn_801631F4(void) {
#include "src/game/people/people_field_fn_801631F4.inc"
}
#else
u32 fn_801631F4(u32 index) {
    PeopleFieldMoveSlot* entries = (PeopleFieldMoveSlot*)lbl_8047B024;
    u8 v = entries[index].active;
    u32 diff = 1 - v;
    return (u32)(diff == 0);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163214(void) {
#include "src/game/people/people_field_fn_80163214.inc"
}
#else
extern u8 lbl_8044FB90[];
extern u8 lbl_8044FE14[];
typedef struct PFAramQueueEntry {
    u32 request;       /* 0x00 */
    u32 command;       /* 0x04 */
    u32 zero;          /* 0x08 */
    u32 priority;      /* 0x0C */
    u8* dst;           /* 0x10 */
    u8* src;           /* 0x14 */
    u32 size;          /* 0x18 */
    void (*callback)(void*); /* 0x1C */
    u32 callbackArg0;  /* 0x20 */
    u32 callbackArg1;  /* 0x24 */
} PFAramQueueEntry;

typedef struct PFAramQueue {
    PFAramQueueEntry entries[16]; /* 0x000 */
    u8 writeIndex;                /* 0x280 */
    u8 count;                     /* 0x281 */
} PFAramQueue;

void aramQueueCallback(void *arg) {
    u8 *tbl;
    u32 i;
    if (*(u32*)((u8*)arg + 0xc) == 1) {
        tbl = lbl_8044FE14;
    } else {
        tbl = lbl_8044FB90;
    }
    for (i = 0; i < 16; i++) {
        u8 *entry = tbl + i * 0x28;
        if (arg == (void*)entry) {
            void (*fn)(void*) = *(void(**)(void*))(entry + 0x20);
            if (fn != 0) {
                fn(*(void**)(entry + 0x24));
            }
        }
    }
    tbl[0x281]--;
}
#endif
#pragma pop
#if 0
asm void fn_801632B4(void) {
#include "src/game/people/people_field_fn_801632B4.inc"
}
#else
void aramUploadData(void* mram, u32 aram, u32 size, u32 highPriority,
                    void (*callback)(u32), u32 user) {
    PFAramQueue* queue;
    s32 old;

    queue = highPriority != 0 ? (PFAramQueue*)lbl_8044FE14
                              : (PFAramQueue*)lbl_8044FB90;

    for (;;) {
        old = OSDisableInterrupts();
        if (queue->count < 16) {
            queue->entries[queue->writeIndex].command = 42;
            queue->entries[queue->writeIndex].zero = 0;
            queue->entries[queue->writeIndex].priority =
                highPriority != 0 ? 1 : 0;
            queue->entries[queue->writeIndex].dst = mram;
            queue->entries[queue->writeIndex].src = (u8*)aram;
            queue->entries[queue->writeIndex].size = size;
            queue->entries[queue->writeIndex].callback =
                (void (*)(void*))aramQueueCallback;
            queue->entries[queue->writeIndex].callbackArg0 = (u32)callback;
            queue->entries[queue->writeIndex].callbackArg1 = user;
            ARQPostRequest(
                &queue->entries[queue->writeIndex],
                queue->entries[queue->writeIndex].command,
                queue->entries[queue->writeIndex].zero,
                queue->entries[queue->writeIndex].priority,
                queue->entries[queue->writeIndex].dst,
                queue->entries[queue->writeIndex].src,
                queue->entries[queue->writeIndex].size,
                queue->entries[queue->writeIndex].callback);
            ++queue->count;
            queue->writeIndex = (queue->writeIndex + 1) % 16;
            OSRestoreInterrupts(old);
            return;
        }
        OSRestoreInterrupts(old);
    }
}
#endif
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern u8 lbl_8044FB90[];
#if 0
asm void fn_80163490(void) {
#include "src/game/people/people_field_fn_80163490.inc"
}
#else
void fn_80163490(void) {
    u8* ptr;

    ptr = lbl_8044FB90;
    while (*(volatile u8*)(ptr + 0x281) != 0) {
    }
}
#endif
#pragma pop
#if 0
asm void fn_801634A8(void) {
#include "src/game/people/people_field_fn_801634A8.inc"
}
#else
void fn_801634A8(u32 size) {
    s16* temporary;
    u32 i;
    u32 base;
    u32 end;
    PFAramQueue* lowQueue;
    PFAramQueue* highQueue;

    base = ARGetBaseAddress();
    temporary = (s16*)fn_801643D8(0x500);
    if (temporary != NULL) {
        for (i = 0; i < 640; ++i) {
            temporary[i] = 0;
        }
        DCFlushRange(temporary, 0x500);
    }

    lowQueue = (PFAramQueue*)lbl_8044FB90;
    highQueue = (PFAramQueue*)lbl_8044FE14;
    lowQueue->writeIndex = 0;
    lowQueue->count = 0;
    highQueue->writeIndex = 0;
    highQueue->count = 0;

    if (temporary != NULL) {
        aramUploadData(temporary, base, 0x500, 0, 0, 0);
        fn_80163490();
        fn_80164400((u32)temporary);
    }

    lbl_8047B07C = base + size;
    end = ARGetSize();
    if (lbl_8047B07C > end) {
        lbl_8047B07C = end;
    }
    lbl_8047B078 = base + 0x500;
    lbl_8047B070 = 0;
    InitStreamBuffers();
}
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80163794(void) {
#include "src/game/people/people_field_fn_80163794.inc"
}
#else
void fn_80163794(void) {}
#endif
#pragma pop
extern u32 ARGetBaseAddress(void);
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80163798(void) {
#include "src/game/people/people_field_fn_80163798.inc"
}
#else
void fn_80163798(void) { ARGetBaseAddress(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
extern u32 lbl_8047B06C;
extern u32 lbl_8047B070;
#if 0
asm void aramSetUploadCallback(void) {
#include "src/game/people/people_field_fn_801637B8.inc"
}
#else
void aramSetUploadCallback(u8* ptr, u32 size) {
    u32 aligned;
    u32 avail;
    if (ptr) {
        aligned = (size + 0x1f) & ~0x1fu;
        avail = ARQGetChunkSize();
        lbl_8047B06C = aligned < avail ? avail : aligned;
    }
    lbl_8047B070 = (u32)ptr;
}
#endif
#pragma pop
#if 0
asm void fn_80163810(void) {
#include "src/game/people/people_field_fn_80163810.inc"
}
#else
u32 fn_80163810(u32 ptr, u32 size) {
    u32 address;
    void* buffer;
    u32 blockSize;

    size = (size + 31) & ~31;
    address = lbl_8047B078;
    if (lbl_8047B070 == 0) {
        DCFlushRange((void*)ptr, size);
        aramUploadData((void*)ptr, lbl_8047B078, size, 0, 0, 0);
        lbl_8047B078 += size;
        return address;
    }

    while (size != 0) {
        blockSize =
            size >= lbl_8047B06C ? lbl_8047B06C : size;
        buffer = ((void* (*)(u32, u32))lbl_8047B070)(ptr, blockSize);
        DCFlushRange(buffer, blockSize);
        aramUploadData(buffer, lbl_8047B078, blockSize, 0, 0, 0);
        size -= blockSize;
        lbl_8047B078 += blockSize;
        ptr += blockSize;
    }

    return address;
}
#endif
extern u32 lbl_8047B078;
void fn_80163BCC(u8* unused, u32 size) {
    lbl_8047B078 -= (size + 0x1F) & ~0x1F;
}
/* shared free-list node (stride 0x10) + heads; used by 80163BE4/CA8/DB0/DE8 */
typedef struct PFNode { struct PFNode* next; u32 f4; u32 f8; u32 fc; } PFNode;
extern PFNode lbl_80450098[];
extern PFNode *lbl_8047B060, *lbl_8047B064, *lbl_8047B068;
extern u32 lbl_8047B074, lbl_8047B07C;
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void InitStreamBuffers(void) {
#include "src/game/people/people_field_fn_80163BE4.inc"
}
#else
/* peopleFieldMoveSetState: init the 0x40-entry free list (node[i].next=&node[i+1],
 * tail NULL; head ptrs reset). byte-match verified via objdiff. */
void InitStreamBuffers(void) {
    u32 i;
    lbl_8047B068 = 0;
    lbl_8047B064 = 0;
    lbl_8047B060 = &lbl_80450098[0];
    for (i = 1; i < 0x40; i++) {
        lbl_80450098[i - 1].next = &lbl_80450098[i];
    }
    lbl_80450098[i - 1].next = 0;
    lbl_8047B074 = lbl_8047B07C;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163CA8(void) {
#include "src/game/people/people_field_fn_80163CA8.inc"
}
#else
/* peopleFieldMoveApplyForce: best-fit allocator over the free list; returns the slot
 * index (u8) or 0xFF. byte-match verified via objdiff. */
u8 fn_80163CA8(u32 size) {
    PFNode* node;
    PFNode* prev;
    PFNode* best;
    u32 bestExcess;
    u32 aligned;

    aligned = (size + 0x1f) & ~0x1fu;
    node = lbl_8047B064;
    best = 0;
    prev = 0;
    bestExcess = (u32)-1;

    while (node != 0) {
        if (node->fc == aligned) {
            best = node;
            goto have_best;
        }
        if (node->fc > aligned) {
            if (bestExcess > node->fc) {
                best = node;
                bestExcess = node->fc;
            }
        }
        prev = node;
        node = node->next;
    }
have_best:

    if (best == 0) {
        PFNode* fh = lbl_8047B060;
        if (fh != 0) {
            if ((u32)(lbl_8047B074 - aligned) >= lbl_8047B078) {
                lbl_8047B060 = fh->next;
                best = fh;
                fh->fc = aligned;
                fh->f8 = aligned;
                lbl_8047B074 = lbl_8047B074 - aligned;
                fh->f4 = lbl_8047B074;
                fh->next = lbl_8047B068;
                lbl_8047B068 = fh;
            }
        }
    } else {
        if (prev != 0) {
            prev->next = best->next;
        } else {
            lbl_8047B064 = best->next;
        }
        best->f8 = aligned;
        best->next = lbl_8047B068;
        lbl_8047B068 = best;
    }

    if (best == 0) {
        return 0xff;
    }
    return (u8)(best - lbl_80450098);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void aramGetStreamBufferAddress(void) {
#include "src/game/people/people_field_fn_80163DB0.inc"
}
#else
u32 aramGetStreamBufferAddress(u32 idx, u32 *out) {
    if (out != 0) {
        *out = lbl_80450098[(u8)idx].f8;
    }
    return lbl_80450098[(u8)idx].f4;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void aramFreeStreamBuffer(void) {
#include "src/game/people/people_field_fn_80163DE8.inc"
}
#else
/* peopleFieldMoveCalcForce: unlink a block, and if it was the active one, recompute the
 * min and recoalesce; else push to the other free list. byte-match verified via objdiff. */
void aramFreeStreamBuffer(u32 idx) {
    PFNode* blk;
    PFNode* cur;
    PFNode* prev;

    blk = &lbl_80450098[(u8)idx];
    cur = lbl_8047B068;
    prev = 0;

    while (cur != 0) {
        if (cur == blk) {
            if (prev != 0) {
                prev->next = blk->next;
            } else {
                lbl_8047B068 = blk->next;
            }
            break;
        }
        prev = cur;
        cur = cur->next;
    }

    if (blk->f4 == lbl_8047B074) {
        u32 mn;
        PFNode* scan;

        blk->next = lbl_8047B060;
        lbl_8047B060 = blk;

        mn = (u32)-1;
        scan = lbl_8047B068;
        while (scan != 0) {
            if (scan->f4 <= mn) {
                mn = scan->f4;
            }
            scan = scan->next;
        }

        scan = lbl_8047B064;
        while (scan != 0) {
            PFNode* nx = scan->next;
            if (scan->f4 < mn) {
                lbl_8047B064 = scan->next;
                scan->next = lbl_8047B060;
                lbl_8047B060 = scan;
            }
            scan = nx;
        }

        lbl_8047B074 = (mn != (u32)-1) ? mn : lbl_8047B07C;
        return;
    }

    blk->next = lbl_8047B064;
    lbl_8047B064 = blk;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80163EE0(void) {
#include "src/game/people/people_field_fn_80163EE0.inc"
}
#else
extern u8  lbl_8047B0A0;
extern u32 lbl_8047B09C;
extern u32 lbl_8047B08C;
extern u32 lbl_8047B098;
extern u32 lbl_8047B094;
extern u32 lbl_8047B090;
extern u32 lbl_8047B0A4;
void salCallback(void) {
    int counter = ((int)lbl_8047B0A0 + 1) % 4;
    u8* ptr = (u8*)(lbl_8047B09C + 0x80000000u) + (u8)counter * 0x280;
    lbl_8047B0A0 = counter;
    AIInitDMA(ptr, 0x280);
    *(volatile u32*)&lbl_8047B08C = OSGetTick();
    if (*(volatile u32*)&lbl_8047B098 != 0) {
        if (lbl_8047B090 == 0) {
            lbl_8047B090 = 1;
            OSEnableInterrupts();
            ((void(*)(void))lbl_8047B0A4)();
            OSDisableInterrupts();
            lbl_8047B090 = 0;
        }
    } else {
        lbl_8047B094 = 1;
    }
}
#endif
#pragma pop
extern u32 lbl_8047B098;
extern u32 lbl_8047B088;
void fn_80163F88(void) {
    lbl_8047B098 = 1;
    lbl_8047B088 = 1;
}
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void dspResumeCallback(void) {
#include "src/game/people/people_field_fn_80163F98.inc"
}
#else
void dspResumeCallback(void) {
    /* volatile accesses: retail keeps program order (stw B098 before
     * lwz B094; stw B094 before lwz B090) -- without volatile the -O4,p
     * scheduler hoists each load over the preceding store, which also
     * flips the 0/1 constant register pairing (r0/r3 -> r3/r4). */
    *(volatile u32*)&lbl_8047B098 = 1;
    if (*(volatile u32*)&lbl_8047B094 != 0) {
        *(volatile u32*)&lbl_8047B094 = 0;
        if (*(volatile u32*)&lbl_8047B090 == 0) {
            *(volatile u32*)&lbl_8047B090 = 1;
            OSEnableInterrupts();
            ((void(*)(void))lbl_8047B0A4)();
            OSDisableInterrupts();
            *(volatile u32*)&lbl_8047B090 = 0;
        }
    }
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm u32 salInitAi(u32(*fnptr)(void), u32 d, u32 a) {
#include "src/game/people/people_field_fn_80163FFC.inc"
}
#else
/* The guard-clause layout preserves the target branch polarity. Keep the
 * adjacent stores direct: swapping or chaining them changes the constant-
 * register pairing. */
u32 salInitAi(void(*fnptr)(void), u32 d, u32 a) {
    lbl_8047B09C = fn_801643D8(0xA00);
    if (lbl_8047B09C != 0) {
        memset((void*)lbl_8047B09C, 0, 0xA00);
        DCFlushRange((void*)lbl_8047B09C, 0xA00);
        lbl_8047B0A0 = 1;
        *(volatile u32*)&lbl_8047B094 = 0;
        *(volatile u32*)&lbl_8047B098 = 1;
        *(volatile u32*)&lbl_8047B090 = 0;
        lbl_8047B0A4 = (u32)fnptr;
        AIRegisterDMACallback((u32)salCallback);
        AIInitDMA((u8*)(lbl_8047B09C + 0x80000000u) + (u32)lbl_8047B0A0 * 0x280, 0x280);
        ((u32*)lbl_80434C50)[1] = 0x20;
        *(u32*)a = 0x7D00;
        return 1;
    }
    return 0;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801640C4(void) {
#include "src/game/people/people_field_fn_801640C4.inc"
}
#else
void fn_801640C4(void) { AIStartDMA(); }
#endif
#pragma pop
u32 salExitAi(void) {
    extern u32 lbl_8047B09C;
    extern void fn_80164400(u32 a);
    AIRegisterDMACallback(0);
    AIStopDMA();
    fn_80164400(lbl_8047B09C);
    return 1;
}
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
#if 0
asm void salAiGetDest(void) {
#include "src/game/people/people_field_fn_80164118.inc"
}
#else
u8* salAiGetDest(void) {
    return (u8*)lbl_8047B09C + (u8)((lbl_8047B0A0 + 2) % 4) * 0x280;
}
#endif
#pragma pop
extern u32 lbl_8047B010;
extern u32 lbl_8047B018;
extern u32 lbl_8047B020;
extern u16* lbl_8047B01C;

extern unsigned char lbl_8047B05C;
extern unsigned char lbl_8047B05D;

/* fn_80164400's body is defined AFTER this function so MWCC's single-pass
 * inliner cannot see it here and each call remains a real `bl` (retail:
 * 0x8015A870). Preferred over #pragma dont_inline per global standard
 * global_standard:avoid-pragmas-register-asm. */
u32 salExitDspCtrl(void) {
    extern u32 lbl_8047B010;
    u8 i;

    fn_80164400(lbl_8047B018);
    for (i = 0; i < lbl_8047B05D; i++) {
        fn_80164400(*(u32*)(lbl_8047B024 + (u32)i * 0xF4));
        fn_80164400(*(u32*)(lbl_8047B024 + (u32)i * 0xF4 + 4));
    }
    for (i = 0; i < lbl_8047B05C; i++) {
        fn_80164400(lbl_80447E60[i].named.allocation);
        fn_80164400(lbl_80447E60[i].named.auxAllocation);
    }
    fn_80164400(lbl_8047B020);
    fn_80164400(lbl_8047B024);
    fn_80164400((u32)lbl_8047B01C);
    fn_80164400((u32)lbl_8047B010);
    return 1;
}
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80164148(void) {
#include "src/game/people/people_field_fn_80164148.inc"
}
#else
extern u8  lbl_804504A0[];
extern u8  lbl_80450500[];
extern u8  lbl_8036A520[];
extern u16 lbl_80478C08;
u32 fn_80164148(u32 d) {
    u8* s = lbl_804504A0;
    *(u32*)(s + 0x0C) = (u32)lbl_8036A520;
    *(u32*)(s + 0x10) = lbl_80478C08;
    *(u32*)(s + 0x14) = 0;
    *(u32*)(s + 0x18) = (u32)lbl_80450500;
    *(u32*)(s + 0x1C) = 0x2000;
    *(u32*)(s + 0x20) = 0;
    *(u16*)(s + 0x24) = 0x10;
    *(u16*)(s + 0x26) = 0x30;
    *(u32*)(s + 0x28) = (u32)fn_80163F88;
    *(u32*)(s + 0x2C) = (u32)dspResumeCallback;
    *(u32*)(s + 0x30) = 0;
    *(u32*)(s + 0x34) = 0;
    *(u32*)(s + 0x04) = 0;
    DSPInit();
    DSPAddTask(lbl_804504A0);
    lbl_8047B088 = 0;
    hwEnableIrq();
    while (*(volatile u32*)&lbl_8047B088 == 0) {
    }
    hwDisableIrq();
    return 1;
}
#endif
#pragma pop
u32 fn_80164204(void) {
    fn_800AE8EC();
    while (fn_800AE92C() != 0) {}
    fn_800AE8A4();
    return 1;
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void salCtrlDsp(void) {
#include "src/game/people/people_field_fn_80164238.inc"
}
#else
extern u32 lbl_8047B010;
extern u16 lbl_8047B00C;
void salCtrlDsp(u32 arg) {
    extern u32 lbl_8047B098;
    extern u32 salGetStartDelay(void);
    fn_8015B250(arg, salGetStartDelay());
    arg = lbl_8047B010;
    lbl_8047B098 = 0;
    PPCSync();
    DSPSendMailToDSP((u32)lbl_8047B00C | 0xBABE0000);
    while (fn_800AE794() != 0) {}
    DSPSendMailToDSP(arg);
    while (fn_800AE794() != 0) {}
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void salGetStartDelay(void) {
#include "src/game/people/people_field_salGetStartDelay.inc"
}
#else
u32 salGetStartDelay(void) {
    extern u32 OSGetTick(void);
    extern u32 lbl_8047B08C;
    u32 tick;
    u32 divisor;
    u32 prev;
    tick = OSGetTick();
    divisor = *(volatile u32*)0x800000F8;
    prev = lbl_8047B08C;
    divisor = __mulhwu(0x431BDE83u, divisor >> 2);
    tick = tick - prev;
    tick = tick << 3;
    return tick / (divisor >> 15);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void hwInitIrq(void) {
#include "src/game/people/people_field_hwInitIrq.inc"
}
#else
void hwInitIrq(void) {
    extern u32 OSDisableInterrupts(void);
    extern u32 lbl_8047B080;
    extern u16 lbl_8047B084;
    lbl_8047B080 = OSDisableInterrupts();
    lbl_8047B084 = 1;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80164324(void) {
#include "src/game/people/people_field_fn_80164324.inc"
}
#else
void fn_80164324(void) {}
#endif
#pragma pop
void hwEnableIrq(void) {
    extern u16 lbl_8047B084;
    extern u32 lbl_8047B080;
    lbl_8047B084 = lbl_8047B084 - 1;
    if (lbl_8047B084 == 0) {
        OSRestoreInterrupts(lbl_8047B080);
    }
}
#pragma push
#pragma optimization_level 2
#pragma optimizewithasm off
void hwDisableIrq(void) {
    extern u16 lbl_8047B084;
    extern u32 lbl_8047B080;
    u16 v = lbl_8047B084++;
    if (v == 0) {
        lbl_8047B080 = OSDisableInterrupts();
    }
}
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80164398(void) {
#include "src/game/people/people_field_fn_80164398.inc"
}
#else
u32 fn_80164398(void) { return OSDisableInterrupts(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801643B8(void) {
#include "src/game/people/people_field_fn_801643B8.inc"
}
#else
u32 fn_801643B8(void) { return OSEnableInterrupts(); }
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801643D8(void) {
#include "src/game/people/people_field_fn_801643D8.inc"
}
#else
u32 fn_801643D8(u32 size) {
    extern u32 lbl_8047B054;
    return ((u32(*)(u32))lbl_8047B054)(size);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80164400(void) {
#include "src/game/people/people_field_fn_80164400.inc"
}
#else
void fn_80164400(u32 a) {
    extern u32 lbl_8047B054;
    u32* base = &lbl_8047B054;
    ((void(*)(u32))base[1])(a);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#if 0
asm void sndAuxCallbackReverbHI(void) {
#include "src/game/people/people_field_sndAuxCallbackReverbHI.inc"
}
#else
void sndAuxCallbackReverbHI(u8 type, u32* data, u8* obj) {
    switch (type) {
        case 0:
            if (obj[0x1C4] != 0) { break; }
            ReverbHICallback(data[0], data[1], data[2], obj);
            break;
        case 1:
            break;
    }
}
#endif
#pragma pop
u32 sndAuxCallbackUpdateSettingsReverbHI(u8* ptr) {
    extern u32 ReverbHIModify(u8* obj, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6);
    ptr[0x1C4] = 1;
    ReverbHIModify(
        ptr,
        *(f32*)(ptr + 0x1C8),
        *(f32*)(ptr + 0x1D0),
        *(f32*)(ptr + 0x1CC),
        *(f32*)(ptr + 0x1D4),
        *(f32*)(ptr + 0x1D8),
        *(f32*)(ptr + 0x1DC)
    );
    ptr[0x1C4] = 0;
    return 1;
}
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void sndAuxCallbackPrepareReverbHI(void) {
#include "src/game/people/people_field_sndAuxCallbackPrepareReverbHI.inc"
}
#else
void sndAuxCallbackPrepareReverbHI(u8* ptr) {
    extern u32 ReverbHICreate(u8* obj, f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6);
    ptr[0x1C4] = 0;
    ReverbHICreate(
        ptr,
        *(f32*)(ptr + 0x1C8),
        *(f32*)(ptr + 0x1D0),
        *(f32*)(ptr + 0x1CC),
        *(f32*)(ptr + 0x1D4),
        *(f32*)(ptr + 0x1D8),
        *(f32*)(ptr + 0x1DC)
    );
}
#endif
#pragma pop
/* ===== StdReverb/reverb.c, ReverbHI* =====
 * Layout and code ported from the MusyX reference `_SND_REVHI_WORK` /
 * `_SND_REVHI_DELAYLINE` (musyx.h) and reverb.c.  Retail's fixed delay-line
 * length table `static int lens[] = {1789,1999,2333,433,149,47,73,67}` is the
 * already-transcribed .data blob at lbl_8036BF00 (0x20 bytes) -- referenced by
 * name so the relocation matches instead of emitting a second copy.
 * salMalloc/salFree are the indirect thunks fn_801643D8 / fn_80164400. */
typedef struct _SND_REVHI_DELAYLINE {
    s32 inPoint;
    s32 outPoint;
    s32 length;
    f32* inputs;
    f32 lastOutput;
} _SND_REVHI_DELAYLINE;

typedef struct _SND_REVHI_WORK {
    _SND_REVHI_DELAYLINE AP[9];
    _SND_REVHI_DELAYLINE C[9];
    f32 allPassCoeff;
    f32 combCoef[9];
    f32 lpLastout[3];
    f32 level;
    f32 damping;
    s32 preDelayTime;
    f32 crosstalk;
    f32* preDelayLine[3];
    f32* preDelayPtr[3];
} _SND_REVHI_WORK;

static inline void DLsetdelay(_SND_REVHI_DELAYLINE* delayline, s32 len) {
    delayline->outPoint = delayline->inPoint - (len * sizeof(f32));
    while (delayline->outPoint < 0) {
        delayline->outPoint += delayline->length;
    }
}

static inline void DLcreate(_SND_REVHI_DELAYLINE* delayline, s32 length) {
    delayline->length = (s32)length * sizeof(f32);
    delayline->inputs = (f32*)fn_801643D8(length * sizeof(f32));
    memset(delayline->inputs, 0, length * sizeof(length));
    delayline->lastOutput = 0.f;
    DLsetdelay(delayline, length >> 1);
    delayline->inPoint = 0;
    delayline->outPoint = 0;
}

static inline void DLdelete(_SND_REVHI_DELAYLINE* delayline) {
    fn_80164400((u32)delayline->inputs);
}

u32 ReverbHICreate(u8* obj, f32 coloration, f32 time, f32 mix, f32 damping, f32 preDelay,
                   f32 crosstalk) {
    _SND_REVHI_WORK* rev = (_SND_REVHI_WORK*)obj;
    const s32* lens;
    u8 i;
    u8 k;

    if (coloration < 0.f || coloration > 1.f || time < 0.01f || time > 10.f || mix < 0.f ||
        mix > 1.f || crosstalk < 0.f || crosstalk > 1.f || damping < 0.f || damping > 1.f ||
        preDelay < 0.f || preDelay > 0.1f) {
        return 0;
    }

    memset(rev, 0, sizeof(_SND_REVHI_WORK));

    lens = (const s32*)lbl_8036BF00;
    for (k = 0; k < 3; ++k) {
        for (i = 0; i < 3; ++i) {
            DLcreate(&rev->C[i + k * 3], lens[i] + 2);
            DLsetdelay(&rev->C[i + k * 3], lens[i]);
            rev->combCoef[i + k * 3] = (f32)pow(10.0, (lens[i] * -3) / (32000.f * time));
        }

        for (i = 0; i < 2; ++i) {
            DLcreate(&rev->AP[i + k * 3], lens[i + 3] + 2);
            DLsetdelay(&rev->AP[i + k * 3], lens[i + 3]);
        }
        DLcreate(&rev->AP[k * 3 + 2], lens[k + 5] + 2);
        DLsetdelay(&rev->AP[k * 3 + 2], lens[k + 5]);
        rev->lpLastout[k] = 0.f;
    }

    rev->allPassCoeff = coloration;
    rev->level = mix;
    rev->crosstalk = crosstalk;
    rev->damping = damping;
    if (rev->damping < 0.05f) {
        rev->damping = 0.05f;
    }

    rev->damping = 1.f - (0.05f + 0.8f * rev->damping);
    if (0.f != preDelay) {
        rev->preDelayTime = 32000.f * preDelay;
        for (i = 0; i < 3; ++i) {
            rev->preDelayLine[i] = (f32*)fn_801643D8(rev->preDelayTime * sizeof(f32));
            memset(rev->preDelayLine[i], 0, rev->preDelayTime * sizeof(f32));
            rev->preDelayPtr[i] = rev->preDelayLine[i];
        }
    } else {
        rev->preDelayTime = 0;
        for (i = 0; i < 3; ++i) {
            rev->preDelayPtr[i] = NULL;
            rev->preDelayLine[i] = NULL;
        }
    }

    return 1;
}
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
/* ReverbHIModify's preserved asm include predates the symbol-map rename. */
#define fn_80164520 ReverbHICreate
#if 0
asm void ReverbHIModify(void) {
#include "src/game/people/people_field_fn_80164A2C.inc"
}
#else
u32 ReverbHIModify(u8* obj, f32 coloration, f32 time, f32 mix, f32 damping,
                   f32 preDelay, f32 crosstalk) {
    _SND_REVHI_WORK* rev = (_SND_REVHI_WORK*)obj;
    u8 i;

    if (coloration < 0.f || coloration > 1.f || time < 0.01f ||
        time > 10.f || mix < 0.f || mix > 1.f || crosstalk < 0.f ||
        crosstalk > 1.f || damping < 0.f || damping > 1.f ||
        preDelay < 0.f || preDelay > 100.f) {
        return 0;
    }

    rev->allPassCoeff = coloration;
    rev->level = mix;
    rev->crosstalk = crosstalk;
    rev->damping = damping;
    if (rev->damping < 0.05f) {
        rev->damping = 0.05f;
    }

    rev->damping = 1.f - (rev->damping * 0.8f + 0.05f);

    for (i = 0; i < 9; ++i) {
        DLdelete(&rev->AP[i]);
    }

    for (i = 0; i < 9; ++i) {
        DLdelete(&rev->C[i]);
    }

    if (rev->preDelayTime != 0) {
        for (i = 0; i < 3; ++i) {
            fn_80164400((u32)rev->preDelayLine[i]);
        }
    }

    return ReverbHICreate(obj, coloration, time, mix, damping, preDelay,
                          crosstalk);
}
#endif
#undef fn_80164520
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80164C40(void) {
#include "src/game/people/people_field_fn_80164C40.inc"
}
#else
void fn_80164C40(s32* a, s32* b, f32 scaleA, f32 scaleB) {
    u32 i;

    if (a == NULL || b == NULL) {
        return;
    }
    for (i = 0; i < 160; i++) {
        a[i] = (s32)((f32)a[i] * scaleA);
        b[i] = (s32)((f32)b[i] * scaleB);
    }
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80164DD0(void) {
#include "src/game/people/people_field_fn_80164DD0.inc"
}
#else
void fn_80164DD0(s32* samples, u8* obj, u32 channel) {
    u32 i;
    f32 wet;
    f32 feedback;
    f32 dry;
    f32 prev;
    f32 value;
    u32 stateOffset;

    if (samples == NULL || obj == NULL) {
        return;
    }
    if (channel > 2) {
        channel = 0;
    }
    wet = *(f32*)(obj + 0x168);
    feedback = *(f32*)(obj + 0x1A0);
    dry = lbl_8047D4F4 - wet;
    stateOffset = 0x18C + channel * 4;
    prev = (f32)*(s32*)(obj + stateOffset);
    for (i = 0; i < 160; i++) {
        value = (f32)samples[i] * dry + prev * wet * feedback;
        samples[i] = (s32)value;
        prev = value;
    }
    *(s32*)(obj + stateOffset) = (s32)prev;
}
#endif
#pragma pop

/* ===== synthvoice.c: voiceBlock, 0x80158328 =====
 * identity: simindex ext:metroidprime seq=1.0 vs synthvoice.c voiceBlock
 * (100% shape match). synthVoice is lbl_8047AF48 (SYNTH_VOICE*, stride
 * 0x404, id@0xF4) per synth.c/stream.c. Colosseum MusyX 2.0.0/2.0.1 (pre-
 * 2.0.1 4-byte PRG_STATE build per slice-3 pin) takes the
 * `MUSY_VERSION <= 2.0.1` branch of allocId (u16, value 0xFFFF).
 * fn_80157A64 = voiceAllocate (not yet decompiled -- still asm/orig; report
 * only, no rename per campaign rules). fn_80157360 = vidRemoveVoiceReferences
 * (0x350 bytes, not yet decompiled). macMakeInactive/voiceSetPriority are
 * cross-TU (synthmacros.c / not-yet-decompiled-here respectively). */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80158328(void) {
#include "src/game/people/people_field_fn_80158328.inc"
}
#else
/* pack(4): matches synth.c/synthmacros.c's SYNTH_VOICE (also pack(4)) --
 * without it MWCC 8-byte-aligns the u64 cFlags field, bumping struct size
 * to 0x408 and shifting block/fxFlag/etc. by 4 bytes (confirmed by
 * objdiff: mulli by 0x408 instead of 0x404, fxFlag read at 0x121 instead
 * of 0x11D). */
#pragma pack(4)
typedef struct {
    u8 pad_00[0x34];
    void* addr;                /* 0x34 */
    u8 pad_38[0xEC - 0x38];
    u32 child;                 /* 0xEC */
    u32 parent;                /* 0xF0 */
    u32 id;                    /* 0xF4 */
    void* vidList;              /* 0xF8 (VidListFull*; see vidGetInternalId below) */
    void* vidMasterList;        /* 0xFC (VidListFull*) */
    u16 allocId;               /* 0x100 */
    u8 pad_102[0x10C - 0x102];
    u8 prio;                    /* 0x10C */
    u8 pad_10D[3];
    u32 age;                    /* 0x110 */
    u64 cFlags;                 /* 0x114 */
    u8 block;                  /* 0x11C */
    u8 fxFlag;                 /* 0x11D */
    u8 pad_11E[0x121 - 0x11E]; /* vGroup, studio, track */
    u8 midi;                   /* 0x121 */
    u8 midiSet;                /* 0x122 */
    u8 pad_123[0x1D6 - 0x123];
    u8 pbLowerKeyRange;        /* 0x1D6 */
    u8 pbUpperKeyRange;        /* 0x1D7 */
    u8 pad_1D8[0x206 - 0x1D8];
    u16 curPitch;               /* 0x206 */
    u8 pad_208[0x214 - 0x208];
    u32 midiDirtyFlags;        /* 0x214 */
    u8 pad_218[0x404 - 0x218];
} SynthVoiceMini; /* offset-mirror of SYNTH_VOICE, stride 0x404 */
#pragma pack()

extern SynthVoiceMini* lbl_8047AF48;      /* synthVoice */
extern u32 fn_80157A64(u8 priority, u8 maxVoices, u16 allocId, u8 fxFlag); /* voiceAllocate */
extern void fn_80157360(SynthVoiceMini* sv);                    /* vidRemoveVoiceReferences */
extern void macMakeInactive(SynthVoiceMini* sv, s32 newState);
extern void voiceSetPriority(SynthVoiceMini* sv, u8 prio);

u32 fn_80158328(u8 prio) {
    u32 voice;

    if ((voice = fn_80157A64(prio, 0xFF, 0xFFFF, 1)) != 0xFFFFFFFF) {
        lbl_8047AF48[voice].block = 1;
        lbl_8047AF48[voice].fxFlag = 1;
        lbl_8047AF48[voice].allocId = 0xFFFF;

        fn_80157360(&lbl_8047AF48[voice]);
        lbl_8047AF48[voice].id = voice | 0xFFFFFF00;

        if (fn_8016246C(voice)) {
            hwBreak(voice);
        }

        macMakeInactive(&lbl_8047AF48[voice], 2); /* MAC_STATE_STOPPED */
        lbl_8047AF48[voice].addr = NULL;
        voiceSetPriority(&lbl_8047AF48[voice], prio);
    }

    return voice;
}
#endif
#pragma pop

/* ===== synthvoice.c: vidMakeRoot, 0x801576B0 =====
 * identity: reference synthvoice.c vidMakeRoot(SYNTH_VOICE* svoice)
 * { svoice->vidMasterList = svoice->vidList; return svoice->vidList->vid; }
 * VID_LIST: { next; prev; u32 vid; u32 root; } -- vid @0x8. SYNTH_VOICE
 * vidList@0xF8 / vidMasterList@0xFC per synth.c. */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801576B0(void) {
#include "src/game/people/people_field_fn_801576B0.inc"
}
#else
typedef struct {
    u8 pad_00[8];
    u32 vid; /* 0x8 */
} VidListMini;

typedef struct {
    u8 pad_00[0xF8];
    VidListMini* vidList;       /* 0xF8 */
    VidListMini* vidMasterList; /* 0xFC */
} SvoiceVidMini;

u32 fn_801576B0(SvoiceVidMini* svoice) {
    svoice->vidMasterList = svoice->vidList;
    return svoice->vidList->vid;
}
#endif
#pragma pop

/* ===== synthvoice.c: vidGetInternalId, 0x801577C8 (already named) =====
 * identity: reference synthvoice.c vidGetInternalId(u32 vid):
 * VID_LIST* vl; if (vid != 0xffffffff) { if ((vl = get_vidlist(vid)) !=
 * NULL) return vl->root; } return 0xffffffff;
 * get_vidlist(vid) walks a sorted singly-linked vidRoot list comparing
 * ->vid (static helper, single call site -> inlined here). VID_LIST.root
 * is the 4th field (0xC). */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void vidGetInternalId(void) {
#include "src/game/people/people_field_vidGetInternalId.inc"
}
#else
typedef struct VidListFull {
    struct VidListFull* next; /* 0x0 */
    struct VidListFull* prev; /* 0x4 */
    u32 vid;                  /* 0x8 */
    u32 root;                 /* 0xC */
} VidListFull;

extern VidListFull* lbl_8047AFD4; /* vidRoot (static file-scope in synthvoice.c;
                                    * confirmed by objdiff -- NOT lbl_8047AF14) */

static VidListFull* get_vidlist(u32 vid) {
    VidListFull* vl = lbl_8047AFD4;

    while (vl != NULL) {
        if (vl->vid == vid) {
            return vl;
        }
        if (vl->vid > vid) {
            break;
        }
        vl = vl->next;
    }

    return NULL;
}

u32 vidGetInternalId(u32 vid) {
    VidListFull* vl;

    if (vid != 0xFFFFFFFF) {
        if ((vl = get_vidlist(vid)) != NULL) {
            return vl->root;
        }
    }

    return 0xFFFFFFFF;
}
#endif
#pragma pop

/* ===== synthvoice.c: voice allocation/priority core continuation =====
 * identity: reference synthvoice.c, pre-2.0.1 branch (Colosseum pin
 * 2.0.0/2.0.1, no `block` guard for MUSY_VERSION>=1.5.4 second inner loop,
 * u16 allocId). Globals cross-verified against
 * build/GC6E01/asm/musyx/musyx_range_80157280.s disassembly:
 *   lbl_8047AFD0 = vidFree           lbl_8047AFD8 = vidCurrentId
 *   lbl_8047AFDC = voicePrioSortRootListRoot (u16)
 *   lbl_8047AFDE = voiceMusicRunning lbl_8047AFDF = voiceFxRunning
 *   lbl_8047AFE0 = voiceListInsert   lbl_8047AFE1 = voiceListRoot
 *   lbl_8047AF50 = synthIdleWaitActive (first byte of an 8-byte object)
 *   lbl_80445F50 = merged synthvoice statics blob (size 0xF00):
 *     +0x000 vidList[128]              (VID_LIST, 0x10 each)
 *     +0x800 voicePrioSortVoices[64]   (SYNTH_VOICELIST, 4 each)
 *     +0x900 voicePrioSortVoicesRoot[256] (u8)
 *     +0xA00 voicePrioSortRootList[256]   (SYNTH_ROOTLIST, 4 each)
 *     +0xE00 voiceList[64]             (SYNTH_VOICELIST, 4 each)
 * voiceRemovePriority/vidRemove/voiceInitFreeList/voiceInitPrioSort are
 * static single-or-multi-callsite helpers in the reference and are fully
 * auto-inlined at every call site here (confirmed by disassembly: no `bl`
 * to any of them). vidMakeNew/voiceSetPriority/voiceAllocate/voiceFree/
 * synthInitAllocationAids/voiceUnblock/voiceKill/vidRemoveVoiceReferences
 * remain real `bl` targets at their call sites (never inlined into each
 * other), matching their non-static linkage in the reference. */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_801576C4(void) {
#include "src/game/people/people_field_fn_801576C4.inc"
}
#else
typedef struct { u8 prev; u8 next; u16 user; } VoiceListEntry; /* SYNTH_VOICELIST, size 4 */
typedef struct { u16 next; u16 prev; } RootListEntry;          /* SYNTH_ROOTLIST, size 4 */

typedef struct {
    u8 pad_00[0x210];
    u8 voiceNum;  /* 0x210 */
    u8 maxMusic;  /* 0x211 */
    u8 maxSFX;    /* 0x212 */
    u8 studioNum; /* 0x213 */
} SynthInfoMini;

extern VidListFull* lbl_8047AFD0; /* vidFree */
extern u32 lbl_8047AFD8;          /* vidCurrentId */
extern u16 lbl_8047AFDC;          /* voicePrioSortRootListRoot */
extern u8 lbl_8047AFDE;           /* voiceMusicRunning */
extern u8 lbl_8047AFDF;           /* voiceFxRunning */
extern u8 lbl_8047AFE0;           /* voiceListInsert */
extern u8 lbl_8047AFE1;           /* voiceListRoot */
extern u8 lbl_8047AF50;           /* synthIdleWaitActive */
extern u8 lbl_80445F50[];         /* merged synthvoice statics blob, see above */

/* NOTE: voicePrioSortVoices/voicePrioSortVoicesRoot/voicePrioSortRootList/
 * voiceListArr are NOT macros -- each function that touches them must
 * declare its own local pointer variable(s), e.g.
 *   VoiceListEntry* voicePrioSortVoices = (VoiceListEntry*)(lbl_80445F50 + 0x800);
 * Retail computes each array's base pointer once per function and reuses
 * that register for every indexed access (confirmed by objdiff: routing
 * these through a macro that re-derives `blob + CONST` at each access
 * site produced displacement-addressed stores instead of retail's
 * register-indexed ones). */
#define voicePrioSortRootListRoot lbl_8047AFDC
#define voiceMusicRunning lbl_8047AFDE
#define voiceFxRunning lbl_8047AFDF
#define voiceListInsert lbl_8047AFE0
#define voiceListRoot lbl_8047AFE1
#define synthIdleWaitActive lbl_8047AF50
#define synthInfo (*(SynthInfoMini*)lbl_80434C50)

extern void voiceResetLastStarted(SynthVoiceMini* svoice);
extern void fn_8014E7D0(u32 voice); /* streamKill (stream.c) */

static void voiceRemovePriority(SynthVoiceMini* svoice);
static void vidRemove(VidListFull** list);

/* ===== synthvoice.c: vidInit, 0x80157280 =====
 * Reference synthvoice.c vidInit: initializes the 128-entry vidList as a
 * doubly-linked free list. */
#pragma push
#pragma optimization_level 4
void vidInit(void) {
    VidListFull* vl;
    u32 i;
    VidListFull* prev;

    prev = NULL;
    lbl_8047AFD8 = 0;
    lbl_8047AFD4 = NULL;
    lbl_8047AFD0 = vl = (VidListFull*)lbl_80445F50;
    for (i = 0; i < 128; i++) {
        vl->prev = prev;
        if (prev != NULL) {
            prev->next = vl;
        }
        prev = vl++;
    }
    prev->next = NULL;
}
#pragma pop

static u32 get_newvid(void) {
    u32 vid;

    do {
        vid = lbl_8047AFD8++;
    } while (vid == 0xFFFFFFFF);

    return vid;
}

u32 fn_801576C4(SynthVoiceMini* svoice, u32 isMaster) {
    u32 vid;
    VidListFull* nvl;
    VidListFull* lvl;
    VidListFull* vl;

    vid = get_newvid();

    lvl = NULL;
    nvl = lbl_8047AFD4;

    while (nvl != NULL) {
        if (nvl->vid > vid) {
            break;
        }

        if (nvl->vid == vid) {
            vid = get_newvid();
        }

        lvl = nvl;
        nvl = nvl->next;
    }

    if ((vl = lbl_8047AFD0) == NULL) {
        return 0xFFFFFFFF;
    }

    if ((lbl_8047AFD0 = vl->next) != NULL) {
        lbl_8047AFD0->prev = NULL;
    }

    if (lvl == NULL) {
        lbl_8047AFD4 = vl;
    } else {
        lvl->next = vl;
    }

    vl->prev = lvl;
    vl->next = nvl;

    if (nvl != NULL) {
        nvl->prev = vl;
    }

    vl->vid = vid;
    vl->root = svoice->id;
    svoice->vidMasterList = isMaster ? vl : NULL;
    svoice->vidList = vl;

    return isMaster ? vid : svoice->id;
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void voiceSetPriority(void) {
#include "src/game/people/people_field_voiceSetPriority.inc"
}
#else
static void voiceRemovePriority(SynthVoiceMini* svoice) {
    VoiceListEntry* voicePrioSortVoices = (VoiceListEntry*)(lbl_80445F50 + 0x800);
    u8* voicePrioSortVoicesRoot = lbl_80445F50 + 0x900;
    RootListEntry* voicePrioSortRootList = (RootListEntry*)(lbl_80445F50 + 0xA00);
    VoiceListEntry* vps;
    RootListEntry* rps;

    vps = &voicePrioSortVoices[(u8)svoice->id];
    if (vps->user != 1) {
        return;
    }

    if (vps->prev != 0xFF) {
        voicePrioSortVoices[vps->prev].next = vps->next;
    } else {
        voicePrioSortVoicesRoot[svoice->prio] = vps->next;
    }

    if (vps->next != 0xFF) {
        voicePrioSortVoices[vps->next].prev = vps->prev;
    } else if (vps->prev == 0xFF) {
        rps = &voicePrioSortRootList[svoice->prio];

        if (rps->prev != 0xFFFF) {
            voicePrioSortRootList[rps->prev].next = rps->next;
        } else {
            voicePrioSortRootListRoot = rps->next;
        }

        if (rps->next != 0xFFFF) {
            voicePrioSortRootList[rps->next].prev = rps->prev;
        }
    }

    vps->user = 0;
}

void voiceSetPriority(SynthVoiceMini* svoice, u8 prio) {
    VoiceListEntry* voicePrioSortVoices = (VoiceListEntry*)(lbl_80445F50 + 0x800);
    u8* voicePrioSortVoicesRoot = lbl_80445F50 + 0x900;
    RootListEntry* voicePrioSortRootList = (RootListEntry*)(lbl_80445F50 + 0xA00);
    u16 li;
    VoiceListEntry* vps;
    u16 i;
    u32 v;

    v = (u8)svoice->id;
    vps = &voicePrioSortVoices[v];
    if (vps->user == 1) {
        if (svoice->prio == prio) {
            return;
        }

        voiceRemovePriority(svoice);
    }

    vps->user = 1;
    vps->prev = 0xFF;
    if ((vps->next = voicePrioSortVoicesRoot[prio]) != 0xFF) {
        voicePrioSortVoices[voicePrioSortVoicesRoot[prio]].prev = v;
    } else if (voicePrioSortRootListRoot != 0xFFFF) {
        if (prio >= voicePrioSortRootListRoot) {
            for (i = voicePrioSortRootListRoot; i != 0xFFFF; i = voicePrioSortRootList[i].next) {
                if ((u16)i > prio) {
                    break;
                }
                li = i;
            }

            voicePrioSortRootList[li].next = (u16)prio;
            voicePrioSortRootList[prio].prev = li;
            voicePrioSortRootList[prio].next = i;
            if (i != 0xFFFF) {
                voicePrioSortRootList[i].prev = prio;
            }
        } else {
            voicePrioSortRootList[prio].next = voicePrioSortRootListRoot;
            voicePrioSortRootList[prio].prev = 0xFFFF;
            voicePrioSortRootList[voicePrioSortRootListRoot].prev = prio;
            voicePrioSortRootListRoot = prio;
        }
    } else {
        voicePrioSortRootList[prio].next = 0xFFFF;
        voicePrioSortRootList[prio].prev = 0xFFFF;
        voicePrioSortRootListRoot = prio;
    }

    voicePrioSortVoicesRoot[prio] = v;
    svoice->prio = prio;
    fn_80162494(svoice->id & 0xFF, ((u32)prio << 24) | (svoice->age >> 15));
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80157A64(void) {
#include "src/game/people/people_field_fn_80157A64.inc"
}
#else
u32 fn_80157A64(u8 priority, u8 maxVoices, u16 allocId, u8 fxFlag) {
    u8* voicePrioSortVoicesRoot = lbl_80445F50 + 0x900;
    VoiceListEntry* voicePrioSortVoices = (VoiceListEntry*)(lbl_80445F50 + 0x800);
    RootListEntry* voicePrioSortRootList = (RootListEntry*)(lbl_80445F50 + 0xA00);
    VoiceListEntry* voiceListArr = (VoiceListEntry*)(lbl_80445F50 + 0xE00);
    s32 i;
    s32 num;
    s32 voice;
    u16 p;
    u32 type_alloc;
    VoiceListEntry* sfv;

    if (synthIdleWaitActive) {
        goto _fail;
    }

    if (fxFlag) {
        type_alloc = (voiceFxRunning >= synthInfo.maxSFX) && (synthInfo.voiceNum > synthInfo.maxSFX);
        if (synthInfo.maxSFX <= maxVoices) {
            goto _skip_alloc;
        }
        goto _do_alloc;
    } else {
        type_alloc = (voiceMusicRunning >= synthInfo.maxMusic) && (synthInfo.voiceNum > synthInfo.maxMusic);
        if (synthInfo.maxMusic <= maxVoices) {
            goto _skip_alloc;
        }

    _do_alloc:
        num = 0;
        voice = -1;
        p = voicePrioSortRootListRoot;
        while (p != 0xFFFF && priority >= p && voice == -1) {
            for (i = voicePrioSortVoicesRoot[p]; i != 0xFF; i = voicePrioSortVoices[i].next) {
                if (allocId != lbl_8047AF48[i].allocId) {
                    continue;
                }
                ++num;
                if (lbl_8047AF48[i].block) {
                    continue;
                }
                if (!type_alloc || fxFlag == lbl_8047AF48[i].fxFlag) {
                    if (lbl_8047AF48[i].cFlags & 2) {
                        continue;
                    }
                    if (voice != -1) {
                        if (lbl_8047AF48[i].age < lbl_8047AF48[voice].age) {
                            voice = i;
                        }
                    } else {
                        voice = i;
                    }
                }
            }

            p = voicePrioSortRootList[p].next;
        }
    }

    if (num < maxVoices) {
        while (p != 0xFFFF && num < maxVoices) {
            for (i = voicePrioSortVoicesRoot[p]; i != 0xFF; i = voicePrioSortVoices[i].next) {
                if (allocId == lbl_8047AF48[i].allocId) {
                    num++;
                }
            }

            p = voicePrioSortRootList[p].next;
        }

        if (num < maxVoices) {
        _skip_alloc:
            if (voiceListRoot != 0xFF && type_alloc == 0) {
                voice = voiceListRoot;
                goto _update;
            }

            if (priority < voicePrioSortRootListRoot) {
                return 0xFFFFFFFF;
            }

            voice = -1;
            p = voicePrioSortRootListRoot;

            while (p != 0xFFFF && priority >= p && voice == -1) {
                for (i = voicePrioSortVoicesRoot[p]; i != 0xFF; i = voicePrioSortVoices[i].next) {
                    if (lbl_8047AF48[i].block != 0) {
                        continue;
                    }

                    if (!type_alloc || fxFlag == lbl_8047AF48[i].fxFlag) {
                        if (lbl_8047AF48[i].cFlags & 2) {
                            continue;
                        }
                        if (voice != -1) {
                            if (lbl_8047AF48[voice].age > lbl_8047AF48[i].age) {
                                voice = i;
                            }
                        } else {
                            voice = i;
                        }
                    }
                }
                p = voicePrioSortRootList[p].next;
            }

            if (voice == -1) {
                return 0xFFFFFFFF;
            }

            if (lbl_8047AF48[voice].prio > priority) {
                goto _fail;
            }
        }
    }

_update:
    if (voice == -1) {
        goto _fail;
    }

    if (voiceListArr[voice].user == 1) {
        sfv = voiceListArr + voice;
        i = sfv->prev;

        if (i != 0xFF) {
            voiceListArr[i].next = sfv->next;
        } else {
            voiceListRoot = sfv->next;
        }

        i = sfv->next;
        if (i != 0xFF) {
            voiceListArr[i].prev = sfv->prev;
        }

        if (voice == voiceListInsert) {
            voiceListInsert = sfv->prev;
        }

        sfv->user = 0;
    } else if (lbl_8047AF48[voice].fxFlag) {
        voiceFxRunning--;
    } else {
        voiceMusicRunning--;
    }

    if (fxFlag != FALSE) {
        ++voiceFxRunning;
    } else {
        ++voiceMusicRunning;
    }

    return voice;

_fail:
    return -1;
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void voiceFree(void) {
#include "src/game/people/people_field_voiceFree.inc"
}
#else
void voiceFree(SynthVoiceMini* svoice) {
    VoiceListEntry* voiceListArr = (VoiceListEntry*)(lbl_80445F50 + 0xE00);
    u32 i;
    VoiceListEntry* sfv;

    macMakeInactive(svoice, 2); /* MAC_STATE_STOPPED */
    voiceRemovePriority(svoice);
    svoice->addr = NULL;
    svoice->prio = 0;
    sfv = &voiceListArr[(i = (u8)svoice->id)];
    if (sfv->user == 0) {
        sfv->user = 1;
        if (voiceListRoot != 0xFF) {
            sfv->next = 0xFF;
            sfv->prev = voiceListInsert;
            voiceListArr[voiceListInsert].next = i;
        } else {
            sfv->next = 0xFF;
            sfv->prev = 0xFF;
            voiceListRoot = i;
        }

        voiceListInsert = i;
        if (svoice->fxFlag != 0) {
            --voiceFxRunning;
        } else {
            --voiceMusicRunning;
        }
    }

    svoice->id = 0xFFFFFFFF;
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void synthInitAllocationAids(void) {
#include "src/game/people/people_field_synthInitAllocationAids.inc"
}
#else
static void voiceInitFreeList(void) {
    VoiceListEntry* voiceListArr = (VoiceListEntry*)(lbl_80445F50 + 0xE00);
    u32 i;

    for (i = 0; i < synthInfo.voiceNum; ++i) {
        voiceListArr[i].prev = (u8)(i - 1);
        voiceListArr[i].next = (u8)(i + 1);
        voiceListArr[i].user = 1;
    }

    voiceListArr[0].prev = 0xFF;
    voiceListArr[synthInfo.voiceNum - 1].next = 0xFF;
    voiceListRoot = 0;
    voiceListInsert = synthInfo.voiceNum - 1;
}

static void voiceInitPrioSort(void) {
    VoiceListEntry* voicePrioSortVoices = (VoiceListEntry*)(lbl_80445F50 + 0x800);
    u8* voicePrioSortVoicesRoot = lbl_80445F50 + 0x900;
    u32 i;

    for (i = 0; i < synthInfo.voiceNum; ++i) {
        voicePrioSortVoices[i].user = 0;
    }

    for (i = 0; i < 256; ++i) {
        voicePrioSortVoicesRoot[i] = 0xFF;
    }

    voicePrioSortRootListRoot = 0xFFFF;
}

void synthInitAllocationAids(void) {
    voiceInitFreeList();
    voiceInitPrioSort();
    voiceFxRunning = 0;
    voiceMusicRunning = 0;
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void voiceUnblock(void) {
#include "src/game/people/people_field_voiceUnblock.inc"
}
#else
void voiceUnblock(u32 voice) {
    if (voice == 0xFFFFFFFF) {
        return;
    }

    if (fn_8016246C(voice)) {
        hwBreak(voice);
    }

    lbl_8047AF48[voice].id = voice;
    voiceFree(&lbl_8047AF48[voice]);
    lbl_8047AF48[voice].block = 0;
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void voiceKill(void) {
#include "src/game/people/people_field_voiceKill.inc"
}
#else
void voiceKill(u32 vi) {
    SynthVoiceMini* sv = &lbl_8047AF48[vi];

    if (sv->addr != NULL) {
        fn_80157360(sv);
        sv->cFlags &= ~(u64)3;
        sv->age = 0;
        voiceFree(sv);
    }

    if (sv->block != 0) {
        fn_8014E7D0(vi);
    }

    hwBreak(vi);
}
#endif
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void fn_80157360(void) {
#include "src/game/people/people_field_fn_80157360.inc"
}
#else
static void vidRemove(VidListFull** list) {
    if ((*list)->prev != NULL) {
        (*list)->prev->next = (*list)->next;
    } else {
        lbl_8047AFD4 = (*list)->next;
    }

    if ((*list)->next != NULL) {
        (*list)->next->prev = (*list)->prev;
    }

    (*list)->next = lbl_8047AFD0;

    if (lbl_8047AFD0 != NULL) {
        lbl_8047AFD0->prev = *list;
    }

    (*list)->prev = NULL;
    lbl_8047AFD0 = *list;
    *list = NULL;
}

void fn_80157360(SynthVoiceMini* svoice) {
    if (svoice->id == 0xFFFFFFFF) {
        return;
    }

    voiceResetLastStarted(svoice);
    if (svoice->parent != 0xFFFFFFFF) {
        lbl_8047AF48[(u8)svoice->parent].child = svoice->child;
        if (svoice->child != 0xFFFFFFFF) {
            lbl_8047AF48[(u8)svoice->child].parent = svoice->parent;
        }

        vidRemove((VidListFull**)&svoice->vidList);
    } else if (svoice->child != 0xFFFFFFFF) {
        ((VidListFull*)svoice->vidList)->root = svoice->child;
        lbl_8047AF48[(u8)svoice->child].parent = 0xFFFFFFFF;
        lbl_8047AF48[(u8)svoice->child].vidMasterList = svoice->vidMasterList;
        if (svoice->vidList != svoice->vidMasterList) {
            vidRemove((VidListFull**)&svoice->vidList);
        }

        svoice->vidMasterList = svoice->vidList = NULL;
    } else if (svoice->vidList != svoice->vidMasterList) {
        vidRemove((VidListFull**)&svoice->vidList);
        vidRemove((VidListFull**)&svoice->vidMasterList);
    } else {
        vidRemove((VidListFull**)&svoice->vidList);
        svoice->vidMasterList = NULL;
    }
}
#endif
#pragma pop

/* ===== synthvoice.c: voiceKillSound, 0x80158870 =====
 * identity: reference synthvoice.c voiceKillSound(u32 voiceid). sndActive =
 * lbl_8047AF18 (u8, confirmed by disassembly @0x80158884). The reference's
 * vidGetInternalId(voiceid) call is fully auto-inlined here (no `bl` to
 * vidGetInternalId in the disassembly, even though vidGetInternalId itself
 * remains a real standalone symbol elsewhere) -- residual-split-map note:
 * this is a case where the SAME callee is inlined at one call site and not
 * at another within retail. */
extern u8 lbl_8047AF18; /* sndActive */

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
s32 voiceKillSound(u32 voiceid) {
    s32 ret = -1;
    u32 next_voiceid;
    u32 i;

    if (lbl_8047AF18 != FALSE) {
        for (voiceid = vidGetInternalId(voiceid); voiceid != 0xFFFFFFFF; voiceid = next_voiceid) {
            i = voiceid & 0xFF;
            next_voiceid = lbl_8047AF48[i].child;
            if (voiceid == lbl_8047AF48[i].id) {
                voiceKill(i);
                ret = 0;
            }
        }
    }

    return ret;
}
#pragma pop

/* ===== synthvoice.c: voice*LastStarted cluster, 0x80158934-0x80158BB0 =====
 * identity: reference synthvoice.c voiceIsLastStarted/voiceSetLastStarted/
 * voiceResetLastStarted/voiceInitLastStarted (identical across all three
 * reference corpora -- marioparty4/smstrikers/prime). Backing statics
 * synth_last_fxstarted[64] / synth_last_started[8][16] identified via
 * symbol-map size-run correlation: XD's symbols.txt has
 * synth_last_fxstarted @.bss size 0x40 immediately followed by
 * synth_last_started @.bss size 0x80, both immediately after voiceList
 * (size 0x100) -- same contiguous layout as Colosseum's
 * lbl_80445F50 (size 0xF00, ends with voiceList) followed immediately by
 * lbl_80446E50 (size 0x40) then lbl_80446E90 (size 0x80) in
 * config/GC6E01/symbols.txt. Confirmed by disassembly
 * (build/GC6E01/asm/musyx/musyx_range_80157280.s @0x80158934): both
 * lbl_80446E50 and lbl_80446E90 are referenced exactly as
 * synth_last_fxstarted/synth_last_started would be. */
extern u8 lbl_80446E50[64];    /* synth_last_fxstarted */
extern u8 lbl_80446E90[8][16]; /* synth_last_started */

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 voiceIsLastStarted(SynthVoiceMini* svoice) {
    u32 i;

    if (svoice->id != 0xFFFFFFFF && svoice->midi != 0xFF) {
        i = svoice->id & 0xFF;
        if (svoice->midiSet == 0xFF) {
            if (lbl_80446E50[i] == i) {
                return TRUE;
            }
        } else if (lbl_80446E90[svoice->midiSet][svoice->midi] == i) {
            return TRUE;
        }
    }

    return FALSE;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void voiceSetLastStarted(SynthVoiceMini* svoice) {
    u32 i;

    if (svoice->id != 0xFFFFFFFF && svoice->midi != 0xFF) {
        i = svoice->id & 0xFF;
        if (svoice->midiSet == 0xFF) {
            lbl_80446E50[i] = i;
        } else {
            lbl_80446E90[svoice->midiSet][svoice->midi] = i;
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void voiceResetLastStarted(SynthVoiceMini* svoice) {
    u32 i;

    if (svoice->id != 0xFFFFFFFF && svoice->midi != 0xFF) {
        i = svoice->id & 0xFF;
        if (svoice->midiSet == 0xFF) {
            if (lbl_80446E50[i] == i) {
                lbl_80446E50[i] = 0xFF;
            }
        } else if (i == lbl_80446E90[svoice->midiSet][svoice->midi]) {
            lbl_80446E90[svoice->midiSet][svoice->midi] = 0xFF;
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void voiceInitLastStarted(void) {
    u32 i;
    u32 j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            lbl_80446E90[i][j] = 0xFF;
        }
    }

    for (j = 0; j < 64; j++) {
        lbl_80446E50[j] = 0xFF;
    }
}
#pragma pop

/* ===== synth_adsr.c: adsrConvertTimeCents, 0x80158CD4 =====
 * identity: reference synth_adsr.c `u32 adsrConvertTimeCents(s32 tc) {
 * return 1000.f * powf(2.f, 1.2715658e-08f * tc); }`. Colosseum's build
 * (pre-2.0.1 fork) calls the DOUBLE-precision `pow` (pow), not
 * `powf` -- confirmed by disassembly: the base constant 2.0 and the
 * exponent are both loaded/passed as doubles (lfd, not lfs), and the
 * result is explicitly `frsp`'d down to single precision before the
 * final `* 1000.f` and the u32 truncation (__cvt_fp2unsigned). This
 * FIXES the previously-stale `void adsrConvertTimeCents(void)` /
 * `u32(u32)` declarations (see identity notes on the extern near line
 * 520 / removed local redeclaration) -- true signature is
 * `u32 adsrConvertTimeCents(s32 tc)`. */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 adsrConvertTimeCents(s32 tc) {
    return (u32)(1000.f * (f32)pow(2.0, 1.2715658e-08f * tc));
}
#pragma pop

/* ===== synth_adsr.c: salChangeADSRState/adsrSetup/adsrStartRelease/
 * adsrRelease/adsrHandle/adsrHandleLowPrecision, 0x80158D2C-0x80159490 =====
 * identity: reference synth_adsr.c, byte-for-byte match to Colosseum's
 * disassembly for all struct offsets and control flow (no pre-2.0.1
 * deviations found here, unlike adsrConvertTimeCents's pow/powf swap).
 * ADSR_VARS layout confirmed field-by-field from disassembly (mode@0x00,
 * state@0x01, cnt@0x04, currentVolume@0x08, currentIndex@0x0C,
 * currentDelta@0x10, data.dls: aTime@0x14, dTime@0x18, sLevel@0x1C,
 * rTime@0x20, cutOff@0x24, aMode@0x26) -- this is exactly the reference
 * header's natural (unpacked) alignment, no #pragma pack needed.
 * dspAttenuationTab = lbl_803692C8 (u16[], confirmed by disassembly at
 * both salChangeADSRState and adsrHandle). dspScale2IndexTab = lbl_8036944C
 * (already known, reused here in adsrStartRelease).
 * RESIDUAL-SPLIT-MAP: adsrRelease's call to adsrStartRelease(adsr,
 * adsr->data.dls.rTime) is FULLY INLINED in retail (no `bl
 * adsrStartRelease` anywhere in adsrRelease's disassembly -- the whole
 * mode-0/mode-1 switch body is duplicated in place), even though
 * adsrStartRelease remains a real standalone `bl` target at its other
 * call sites (e.g. 0x8015C640, 0x8015BEF8). Mirrors the vidGetInternalId/
 * voiceKillSound case: same callee, inlined at one call site and not at
 * another. Source-order placement of adsrStartRelease's definition
 * *before* adsrRelease in this file is required to get MWCC's -inline
 * auto to make the same call. adsrGetIndex (static, 2 call sites within
 * salChangeADSRState/adsrHandle) is always inlined in retail (no
 * standalone symbol) -- matches reference's `static` linkage. */
extern u16 lbl_803692C8[]; /* dspAttenuationTab */

typedef struct {
    u8 mode;
    u8 state;
    u32 cnt;
    s32 currentVolume;
    s32 currentIndex;
    s32 currentDelta;
    union {
        struct {
            u32 aTime;
            u32 dTime;
            u16 sLevel;
            u32 rTime;
            u16 cutOff;
            u8 aMode;
        } dls;
        struct {
            u32 aTime;
            u32 dTime;
            u16 sLevel;
            u32 rTime;
        } linear;
    } data;
} AdsrVars;

static u32 adsrGetIndex(AdsrVars* adsr) {
    s32 i = 193 - ((adsr->currentIndex + 0x8000) >> 16);
    return i < 0 ? 0 : i;
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 salChangeADSRState(AdsrVars* adsr) {
    u32 VoiceDone;
    VoiceDone = FALSE;

    switch (adsr->mode) {
    case 0:
        switch (adsr->state) {
        case 0: {
            if ((adsr->cnt = adsr->data.dls.aTime)) {
                adsr->state = 1;
                adsr->currentVolume = 0;
                adsr->currentDelta = 0x7fff0000 / adsr->data.dls.aTime;
                goto done;
            }
        }
        case 1: {
            if ((adsr->cnt = adsr->data.dls.dTime)) {
                adsr->state = 2;
                adsr->currentVolume = 0x7fff0000;
                adsr->currentDelta =
                    -((0x7fff0000 - (adsr->data.dls.sLevel * 0x10000)) / adsr->data.dls.dTime);
                goto done;
            }
        }
        case 2: {
            if (adsr->data.dls.sLevel != 0) {
                adsr->state = 3;
                adsr->currentVolume = adsr->data.dls.sLevel << 0x10;
                adsr->currentDelta = 0;
                goto done;
            }
        }
        case 4: {
            break;
        }
        default:
            goto done;
        }
        adsr->currentVolume = 0;
        VoiceDone = TRUE;
        break;
    case 1:
        switch (adsr->state) {
        case 0: {
            if ((adsr->cnt = adsr->data.dls.aTime)) {
                adsr->state = 1;
                if (adsr->data.dls.aMode == 0) {
                    adsr->currentVolume = 0;
                    adsr->currentDelta = 0x7fff0000 / adsr->cnt;
                } else {
                    adsr->currentVolume = adsr->currentIndex = 0;
                    adsr->currentDelta = 0xc10000 / adsr->cnt;
                }
                goto done;
            }
        }
        case 1: {
            adsr->cnt = adsr->data.dls.dTime * (((0xc1u - adsr->data.dls.sLevel) * 0x10000) / 0xc1) >> 16;
            if (adsr->cnt) {
                adsr->state = 2;
                adsr->currentVolume = 0x7fff0000;
                adsr->currentIndex = 0xc10000;
                adsr->currentDelta = -(((0xc1 - (u32)adsr->data.dls.sLevel) * 0x10000) / adsr->cnt);
                goto done;
            }
        }
        case 2: {
            if (adsr->data.dls.sLevel) {
                adsr->state = 3;
                adsr->currentIndex = adsr->data.dls.sLevel << 16;
                adsr->currentVolume = lbl_803692C8[adsrGetIndex(adsr)] << 16;
                adsr->currentDelta = 0;
                goto done;
            }
            break;
        }
        case 4: {
            break;
        }
        default:
            goto done;
        }
        adsr->currentVolume = 0;
        VoiceDone = TRUE;
        break;
    }
done:
    return VoiceDone;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 adsrSetup(AdsrVars* adsr) {
    adsr->state = 0;
    return salChangeADSRState(adsr);
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 adsrStartRelease(AdsrVars* adsr, u32 rtime) {
    switch (adsr->mode) {
    case 0:
        adsr->state = 4;
        adsr->cnt = rtime;
        if (rtime == 0) {
            adsr->cnt = 1;
            adsr->currentDelta = 0;
            return 1;
        }
        adsr->currentDelta = -(adsr->currentVolume / rtime);
        break;
    case 1:
        if (adsr->data.dls.aMode == 0 && adsr->state == 1) {
            adsr->currentIndex = (193 - lbl_8036944C[adsr->currentVolume >> 21]) * 0x10000;
        }

        adsr->cnt = (u32)(3.238342E-4f * (f32)adsr->currentIndex * (f32)rtime) >> 12;
        adsr->state = 4;
        if (adsr->cnt == 0) {
            adsr->cnt = 1;
            adsr->currentDelta = adsr->currentIndex = adsr->currentVolume = 0;
            return 1;
        }

        adsr->currentDelta = -(adsr->currentIndex / adsr->cnt);
        break;
    }

    return 0;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 adsrRelease(AdsrVars* adsr) {
    switch (adsr->mode) {
    case 0:
    case 1:
        return adsrStartRelease(adsr, adsr->data.dls.rTime);
    }

    return FALSE;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 adsrHandle(AdsrVars* adsr, u16* adsr_start, u16* adsr_delta) {
    s32 old_volume;
    u32 VoiceDone;
    s32 vDelta;

    VoiceDone = FALSE;

    switch (adsr->mode) {
    case 0:
        if (adsr->state != 3) {
            old_volume = adsr->currentVolume;
            adsr->currentVolume += adsr->currentDelta;
            *adsr_start = old_volume >> 16;
            if (adsr->currentDelta >= 0) {
                *adsr_delta = adsr->currentDelta >> 21;
            } else {
                *adsr_delta = -(-adsr->currentDelta >> 21);
            }

            if (--adsr->cnt == 0) {
                VoiceDone = salChangeADSRState(adsr);
            }
        } else {
            *adsr_start = adsr->currentVolume >> 16;
            *adsr_delta = 0;
        }
        break;
    case 1:
        if (adsr->state != 3) {
            old_volume = adsr->currentVolume;
            if (adsr->data.dls.aMode == 0 && adsr->state == 1) {
                adsr->currentVolume += adsr->currentDelta;
            } else {
                adsr->currentIndex += adsr->currentDelta;
                adsr->currentVolume = lbl_803692C8[adsrGetIndex(adsr)] << 16;
            }
            *adsr_start = old_volume >> 16;
            vDelta = adsr->currentVolume - old_volume;
            if (vDelta >= 0) {
                *adsr_delta = vDelta >> 21;
            } else {
                *adsr_delta = -(-vDelta >> 21);
            }

            if (--adsr->cnt == 0) {
                VoiceDone = salChangeADSRState(adsr);
            }
        } else {
            *adsr_start = adsr->currentVolume >> 16;
            *adsr_delta = 0;
        }
        break;
    }

    return VoiceDone;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 adsrHandleLowPrecision(AdsrVars* adsr, u16* adsr_start, u16* adsr_delta) {
    u8 i;

    for (i = 0; i < 15; i++) {
        if (adsrHandle(adsr, adsr_start, adsr_delta)) {
            return 1;
        }
    }

    return 0;
}
#pragma pop

/* ===== synth_vsamples.c: vsInit, 0x80159494 =====
 * Initializes the virtual-sample table (BSS block at 0x80446F10, size
 * 0x950 per symbols.txt): clears count, marks 64 vsID slots as free
 * (0xFF), and zeroes trailing bookkeeping fields. */
extern u8 lbl_80446F10[0x950]; /* vsSampleInfo BSS block */

typedef struct MusyxVirtualSampleInfo {
    u16 smpID;
    u16 instID;
    union {
        struct {
            u32 off1;
            u32 len1;
            u32 off2;
            u32 len2;
        } update;
    } data;
} MusyxVirtualSampleInfo;

typedef struct MusyxVirtualSampleBuffer {
    u8 state;
    u8 hwId;
    u8 smpType;
    u8 voice;
    u32 last;
    u32 finalGoodSamples;
    u32 finalLast;
    MusyxVirtualSampleInfo info;
} MusyxVirtualSampleBuffer;

typedef struct MusyxVirtualSamples {
    u8 numBuffers;
    u8 pad_01[3];
    u32 bufferLength;
    MusyxVirtualSampleBuffer streamBuffer[64];
    u8 voices[64];
    u16 nextInstID;
    u8 pad_94A[2];
    u32 (*callback)(u8 reason, const MusyxVirtualSampleInfo* info);
} MusyxVirtualSamples;

extern void hwSetVirtualSampleLoopBuffer(u8 voice, void* address, u32 length);
extern u16 hwGetSampleID(u8 voice);
extern u8 hwGetSampleType(u8 voice);

#pragma push
#pragma optimization_level 4
void vsInit(void) {
    u32 i;

    lbl_80446F10[0] = 0;
    for (i = 0; i < 64; i++) {
        lbl_80446F10[0x908 + i] = 0xFF;
    }
    *(u16*)&lbl_80446F10[0x948] = 0;
    *(u32*)&lbl_80446F10[0x94C] = 0;
}
#pragma pop

static u16 vsNewInstanceID(MusyxVirtualSamples* vs) {
    u8 i;
    u16 instID;

    do {
        instID = vs->nextInstID++;
        for (i = 0; i < vs->numBuffers; ++i) {
            if (vs->streamBuffer[i].state != 0 &&
                vs->streamBuffer[i].info.instID == instID) {
                break;
            }
        }
    } while (i != vs->numBuffers);

    return instID;
}

static u8 vsAllocateBuffer(MusyxVirtualSamples* vs) {
    u8 i;

    for (i = 0; i < vs->numBuffers; ++i) {
        if (vs->streamBuffer[i].state == 0) {
            vs->streamBuffer[i].state = 1;
            vs->streamBuffer[i].last = 0;
            return i;
        }
    }

    return 0xFF;
}

static void vsFreeBuffer(MusyxVirtualSamples* vs, u8 bufferIndex) {
    vs->streamBuffer[bufferIndex].state = 0;
    vs->voices[vs->streamBuffer[bufferIndex].voice] = 0xFF;
}

u32 fn_80159550(u8 voice) {
    MusyxVirtualSamples* vs = (MusyxVirtualSamples*)lbl_80446F10;
    u8 sb;
    u8 i;
    u32 address;

    for (i = 0; i < vs->numBuffers; ++i) {
        if (vs->streamBuffer[i].state != 0 &&
            vs->streamBuffer[i].voice == voice) {
            vsFreeBuffer(vs, i);
        }
    }

    sb = vs->voices[voice] = vsAllocateBuffer(vs);
    if (sb != 0xFF) {
        address = aramGetStreamBufferAddress(vs->voices[voice], 0);
        hwSetVirtualSampleLoopBuffer(voice, (void*)address,
                                     vs->bufferLength);
        vs->streamBuffer[sb].info.smpID = hwGetSampleID(voice);
        vs->streamBuffer[sb].info.instID = vsNewInstanceID(vs);
        vs->streamBuffer[sb].smpType = hwGetSampleType(voice);
        vs->streamBuffer[sb].voice = voice;
        if (vs->callback != NULL) {
            vs->callback(0, &vs->streamBuffer[sb].info);
            return (vs->streamBuffer[sb].info.instID << 8) | voice;
        }
        hwSetVirtualSampleLoopBuffer(voice, NULL, 0);
    } else {
        hwSetVirtualSampleLoopBuffer(voice, NULL, 0);
    }

    return 0xFFFFFFFF;
}

void fn_80159840(MusyxVirtualSampleBuffer* sb, u32 cpos) {
    MusyxVirtualSamples* vs = (MusyxVirtualSamples*)lbl_80446F10;
    u32 len;

    if (sb->last == cpos) {
        return;
    }

    if ((s32)sb->last < cpos) {
        if (sb->smpType == 5) {
            sb->info.data.update.off1 = (sb->last / 14) * 8;
            sb->info.data.update.len1 = cpos - sb->last;
            sb->info.data.update.off2 = 0;
            sb->info.data.update.len2 = 0;
            if ((len = vs->callback(1, &sb->info)) != 0) {
                sb->last = (sb->last + len) % vs->bufferLength;
            }
        }
    } else if (cpos == 0) {
        if (sb->smpType == 5) {
            sb->info.data.update.off1 = (sb->last / 14) * 8;
            sb->info.data.update.len1 = vs->bufferLength - sb->last;
            sb->info.data.update.off2 = 0;
            sb->info.data.update.len2 = 0;
            if ((len = vs->callback(1, &sb->info)) != 0) {
                sb->last = (sb->last + len) % vs->bufferLength;
            }
        }
    } else if (sb->smpType == 5) {
        sb->info.data.update.off1 = (sb->last / 14) * 8;
        sb->info.data.update.len1 = vs->bufferLength - sb->last;
        sb->info.data.update.off2 = 0;
        sb->info.data.update.len2 = cpos;
        if ((len = vs->callback(1, &sb->info)) != 0) {
            sb->last = (sb->last + len) % vs->bufferLength;
        }
    }
}

extern u8 fn_80162878(u32 voice); /* hwGetVirtualSampleState */
extern u32 fn_80162E14(u32 voice); /* hwGetPos */
extern u32 hwGetVirtualSampleID(u32 voice);
extern u32 fn_801631F4(u32 voice); /* hwVoiceInStartup */
extern void macSampleEndNotify(SynthVoiceMini* voice);

void vsSampleUpdates(void) {
    extern void fn_80159840(MusyxVirtualSampleBuffer* buffer, u32 position);
    extern u8 fn_80162878(u32 voice);
    extern u32 fn_80162E14(u32 voice);
    extern u32 hwGetVirtualSampleID(u32 voice);
    extern u32 fn_801631F4(u32 voice);
    extern void hwBreak(u32 voice);
    extern void macSampleEndNotify(SynthVoiceMini* voice);
    extern void voiceKill(u32 voice);
    MusyxVirtualSamples* vs = (MusyxVirtualSamples*)lbl_80446F10;
    u32 i;
    u32 cpos;
    u32 realCPos;
    MusyxVirtualSampleBuffer* buffer;
    u32 nextSamples;

    if (vs->callback == NULL) {
        return;
    }

    for (i = 0; i < 64; i++) {
        if (vs->voices[i] != 0xFF && fn_80162878(i) != 0) {
            buffer = &vs->streamBuffer[vs->voices[i]];
            realCPos = fn_80162E14(i);
            if (buffer->smpType == 5) {
                cpos = (realCPos / 14) * 14;
            } else {
                cpos = realCPos;
            }

            switch (buffer->state) {
            case 1:
                fn_80159840(buffer, cpos);
                break;
            case 2:
            case 3:
                if (((buffer->info.instID << 8) | buffer->voice) ==
                    hwGetVirtualSampleID(buffer->voice)) {
                    fn_80159840(buffer, cpos);
                    if (realCPos >= buffer->finalLast) {
                        buffer->finalGoodSamples -=
                            realCPos - buffer->finalLast;
                    } else {
                        buffer->finalGoodSamples -=
                            vs->bufferLength -
                            (buffer->finalLast - realCPos);
                    }
                    buffer->finalLast = realCPos;
                    nextSamples =
                        (lbl_8047AF48[buffer->voice].curPitch * 160 +
                         0xFFF) /
                        4096;
                    if ((s32)nextSamples >
                        (s32)buffer->finalGoodSamples) {
                        if (!fn_801631F4(buffer->voice)) {
                            if (buffer->state == 2) {
                                hwBreak(buffer->voice);
                                macSampleEndNotify(
                                    &lbl_8047AF48[buffer->voice]);
                            } else {
                                voiceKill(buffer->voice);
                            }
                        }
                        buffer->state = 0;
                        vs->voices[buffer->voice] = 0xFF;
                    }
                } else {
                    buffer->state = 0;
                    vs->voices[buffer->voice] = 0xFF;
                }
                break;
            }
        }
    }
}

/* ===== hw_dspctrl.c: salBuildCommandList, 0x8015B250 =====
 * identity: simindex ext:metroidprime/marioparty4 hw_dspctrl.c
 * salBuildCommandList -- 100% shape match (sz=8632 both sides, seq=1.0,
 * mh=1.0). Colosseum pre-2.0.1 pin confirmed (no FILTERInfo/_PBLPF field in
 * DSPvoice/_PB -- postBreak@0xED/startupBreak@0xEE line up exactly without
 * it; needsDelta/single-dim dspMixerCycles[] variant used, not the
 * >=2.0.1 three-lookup dspMixerCyclesMain[] form; no dspCompressorOn tail
 * command). Globals cross-verified field-by-field against this file's own
 * disassembly (build/GC6E01/asm/musyx/musyx_range_80157280.s, fn_8015B250):
 *   lbl_8047AFF0 = dspARAMZeroBuffer   lbl_8047AFF4 = dspCmdLastLoad
 *   lbl_8047AFF8 = dspCmdLastBase      lbl_8047AFFC = dspCmdLastSize
 *   lbl_8047B000 = dspCmdCurBase       lbl_8047B004 = dspCmdMaxPtr
 *   lbl_8047B008 = dspCmdPtr           lbl_8047B00C = dspCmdFirstSize
 *   lbl_8047B010 = dspCmdList          lbl_8047B014 = dspHRTFOn (already
 *     known from fn_801631C0 = "disable HRTF" setter elsewhere in this file)
 *   lbl_8047B01C = dspSurround         lbl_8047B05C = salMaxStudioNum
 *   lbl_8047B05E = salAuxFrame (used with /3 magic-division + aux offset
 *     0x28/0x30 indexing)              lbl_8047B05F = salFrame (used both
 *     bare and ^1, e.g. the MIX_AUXB_LR command's two back-to-back reads)
 *   lbl_80447E60 = dspStudio[8] (DSPstudioinfo, stride 0xBC -- already
 *     known/used as u8[] and partially mirrored as PeopleStudioState
 *     elsewhere in this file; cast locally here, not redeclared)
 *   lbl_80448440 = the retail function-local `static DSPvoice* voices[64]`
 *     promoted to a fixed blob per this campaign's established convention
 *   lbl_802732E0 = dspMixerCycles[32] (u16, indexed directly by
 *     pb->mixerCtrl, confirmed via `lhz r0,0xc(pb); slwi; lhzx`)
 *   lbl_80369A50 = dspSRCCycles[3][6] (u16, row stride 0xC, confirmed via
 *     `mulli r4,ratioHi,0x6; lhz srcSelect@0x8(pb); slwi; add; lhzx`)
 *   __OSBusClock read directly at absolute 0x800000F8 (same idiom already
 *     used by salGetStartDelay elsewhere in this file), re-read fresh both
 *     times it's needed (not cached across the function).
 * Helper callees (salCheckVolErrorAndResetDelta, sal_setup_dspvol,
 * sal_update_hostplayinfo, DoDepopFade, SortVoices, HandleDepopVoice =
 * fn_8015AD1C) are real `bl` targets at fixed addresses immediately
 * preceding this function (0x8015AAC0-0x8015B24C per symbols.txt,
 * confirmed by the disassembly's literal `bl <name>` targets). SortVoices
 * below is ported from AxioDL/musyx v2.0.0 `runtime/hw_dspctrl.c` and
 * matches retail exactly. The remaining helpers are not yet decompiled;
 * their extern prototypes match the MP4/Prime reference signatures. */
typedef struct DSPADPCMblock {
    s16 Y0;
    s16 Y1;
    u8  PS;
    u8  reserved;
} DSPADPCMblock;

typedef struct DSPADPCMplusInfo {
    u16 numCoef;
    u8  initialPS;
    u8  loopPS;
    s16 loopY0;
    s16 loopY1;
    s16 coefTab[8][2];
    DSPADPCMblock blk[1];
} DSPADPCMplusInfo;

typedef struct SNDADPCMinfo {
    u16 numCoef;
    u8  initialPS;
    u8  loopPS;
    s16 loopY0;
    s16 loopY1;
    s16 coefTab[8][2];
} SNDADPCMinfo;

typedef struct SAMPLE_INFO {
    u32   info;
    void* addr;
    void* extraData;
    u32   offset;
    u32   length;
    u32   loop;
    u32   loopLength;
    u8    compType;
} SAMPLE_INFO;

typedef struct VSampleInfo {
    void* loopBufferAddr;
    u32   loopBufferLength;
    u8    inLoopBuffer;
} VSampleInfo;

typedef struct DSPvoice {
    struct _PB* pb;
    void* patchData;
    void* itdBuffer;
    struct DSPvoice* next;
    struct DSPvoice* prev;
    struct DSPvoice* nextAlien;
    u32 mesgCallBackUserValue;
    u32 prio;
    u32 currentAddr;
    u32 changed[5];
    u32 pitch[5];
    u16 volL;
    u16 volR;
    u16 volS;
    u16 volLa;
    u16 volRa;
    u16 volSa;
    u16 volLb;
    u16 volRb;
    u16 volSb;
    u16 lastVolL;
    u16 lastVolR;
    u16 lastVolS;
    u16 lastVolLa;
    u16 lastVolRa;
    u16 lastVolSa;
    u16 lastVolLb;
    u16 lastVolRb;
    u16 lastVolSb;
    u16 smp_id;
    SAMPLE_INFO smp_info;
    VSampleInfo vSampleInfo;
    u8 streamLoopPS;
    AdsrVars adsr;
    u16 srcTypeSelect;
    u16 srcCoefSelect;
    u16 itdShiftL;
    u16 itdShiftR;
    u8 singleOffset;
    struct {
        u32 posHi;
        u32 posLo;
        u32 pitch;
    } playInfo;
    struct {
        u8 pitch;
        u8 vol;
        u8 volA;
        u8 volB;
    } lastUpdate;
    u32 virtualSampleID;
    u8 state;
    u8 postBreak;
    u8 startupBreak;
    u8 studio;
    u32 flags;
} DSPvoice;

typedef struct DSPhostDPop {
    s32 l, r, s;
    s32 lA, rA, sA;
    s32 lB, rB, sB;
} DSPhostDPop;

typedef struct DSPinput {
    u8    studio;
    u16   vol;
    u16   volA;
    u16   volB;
    void* desc;
} DSPinput;

typedef struct DSPstudioinfo {
    void*        spb;
    DSPhostDPop  hostDPopSum;
    s32*         main[2];
    s32*         auxA[3];
    s32*         auxB[3];
    DSPvoice*    voiceRoot;
    DSPvoice*    alienVoiceRoot;
    u8           state;
    u8           isMaster;
    u8           numInputs;
    u8           pad_53;
    s32          type;
    DSPinput     in[7];
    void*        auxAHandler;
    void*        auxBHandler;
    void*        auxAUser;
    void*        auxBUser;
} DSPstudioinfo;

typedef struct _PBMIX {
    u16 vL, vDeltaL;
    u16 vR, vDeltaR;
    u16 vAuxAL, vDeltaAuxAL;
    u16 vAuxAR, vDeltaAuxAR;
    u16 vAuxBL, vDeltaAuxBL;
    u16 vAuxBR, vDeltaAuxBR;
    u16 vAuxBS, vDeltaAuxBS;
    u16 vS, vDeltaS;
    u16 vAuxAS, vDeltaAuxAS;
} _PBMIX;

typedef struct _PBITD {
    u16 flag;
    u16 bufferHi, bufferLo;
    u16 shiftL, shiftR;
    u16 targetShiftL, targetShiftR;
} _PBITD;

typedef struct _PBUPDATE {
    u16 updNum[5];
    u16 dataHi, dataLo;
} _PBUPDATE;

typedef struct _PBDPOP {
    u16 aL, aAuxAL, aAuxBL;
    u16 aR, aAuxAR, aAuxBR;
    u16 aS, aAuxAS, aAuxBS;
} _PBDPOP;

typedef struct _PBVE {
    u16 currentVolume;
    u16 currentDelta;
} _PBVE;

typedef struct _PBFIR {
    u16 numCoefs;
    u16 coefsHi, coefsLo;
} _PBFIR;

typedef struct _PBADDR {
    u16 loopFlag;
    u16 format;
    u16 loopAddressHi, loopAddressLo;
    u16 endAddressHi, endAddressLo;
    u16 currentAddressHi, currentAddressLo;
} _PBADDR;

typedef struct _PBADPCM {
    u16 a[8][2];
    u16 gain;
    u16 pred_scale;
    u16 yn1, yn2;
} _PBADPCM;

typedef struct _PBSRC {
    u16 ratioHi, ratioLo;
    u16 currentAddressFrac;
    u16 last_samples[4];
} _PBSRC;

typedef struct _PBADPCMLOOP {
    u16 loop_pred_scale;
    u16 loop_yn1, loop_yn2;
} _PBADPCMLOOP;

typedef struct _PB {
    u16 nextHi, nextLo;
    u16 currHi, currLo;
    u16 srcSelect;
    u16 coefSelect;
    u16 mixerCtrl;
    u16 state;
    u16 loopType;
    _PBMIX mix;
    _PBITD itd;
    _PBUPDATE update;
    _PBDPOP dpop;
    _PBVE ve;
    _PBFIR fir;
    _PBADDR addr;
    _PBADPCM adpcm;
    _PBSRC src;
    _PBADPCMLOOP adpcmLoop;
    u16 streamLoopCnt;
} _PB;

typedef struct _SPB {
    u16 dpopLHi, dpopLLo, dpopLDelta;
    u16 dpopRHi, dpopRLo, dpopRDelta;
    u16 dpopSHi, dpopSLo, dpopSDelta;
    u16 dpopALHi, dpopALLo, dpopALDelta;
    u16 dpopARHi, dpopARLo, dpopARDelta;
    u16 dpopASHi, dpopASLo, dpopASDelta;
    u16 dpopBLHi, dpopBLLo, dpopBLDelta;
    u16 dpopBRHi, dpopBRLo, dpopBRDelta;
    u16 dpopBSHi, dpopBSLo, dpopBSDelta;
} _SPB;

extern u32 lbl_8047AFF0;      /* dspARAMZeroBuffer */
extern u16* lbl_8047AFF4;     /* dspCmdLastLoad */
extern u16* lbl_8047AFF8;     /* dspCmdLastBase */
extern u16  lbl_8047AFFC;     /* dspCmdLastSize */
extern u16* lbl_8047B000;     /* dspCmdCurBase */
extern u16* lbl_8047B004;     /* dspCmdMaxPtr */
extern u16* lbl_8047B008;     /* dspCmdPtr */
extern u16  lbl_8047B00C;     /* dspCmdFirstSize */
extern u32  lbl_8047B010;     /* dspCmdList -- already declared u32 elsewhere in
                               * this file (salCtrlDsp); reinterpret via a
                               * pointer-cast macro below instead of redeclaring
                               * the symbol with a conflicting type. */
extern u32  lbl_8047B014;     /* dspHRTFOn */
extern u16* lbl_8047B01C;     /* dspSurround */
extern u8   lbl_8047B05C;     /* salMaxStudioNum */
extern u8   lbl_8047B05E;     /* salAuxFrame */
extern u8   lbl_8047B05F;     /* salFrame */
extern DSPvoice* lbl_80448440[64]; /* static local `voices[64]` promoted to a fixed blob */
extern u16 lbl_802732E0[32];       /* dspMixerCycles */
extern u16 lbl_80369A50[3][6];     /* dspSRCCycles */

#define dspARAMZeroBuffer lbl_8047AFF0
#define dspCmdLastLoad    lbl_8047AFF4
#define dspCmdLastBase    lbl_8047AFF8
#define dspCmdLastSize    lbl_8047AFFC
#define dspCmdCurBase     lbl_8047B000
#define dspCmdMaxPtr      lbl_8047B004
#define dspCmdPtr         lbl_8047B008
#define dspCmdFirstSize   lbl_8047B00C
#define dspCmdList        (*(u16**)&lbl_8047B010)
#define dspHRTFOn         lbl_8047B014
#define dspSurround       lbl_8047B01C
#define salMaxStudioNum   lbl_8047B05C
#define salAuxFrame       lbl_8047B05E
#define salFrame          lbl_8047B05F
#define voices            lbl_80448440
#define dspMixerCycles    lbl_802732E0
#define dspSRCCycles      lbl_80369A50
#define dspStudio         ((DSPstudioinfo*)lbl_80447E60)

extern u32 salSynthSendMessage(DSPvoice* dsp_vptr, u32 mesg);

static inline void AddDpop(s32* sum, s16 delta) {
    *sum += delta;
    *sum = (*sum > 0x7FFFFF)
               ? 0x7FFFFF
               : (*sum < -0x7FFFFF ? -0x7FFFFF : *sum);
}

void fn_8015AD1C(DSPstudioinfo* stp, DSPvoice* dsp_vptr) {
    _PB* pb;

    dsp_vptr->postBreak = 0;
    dsp_vptr->pb->state = 0;
    pb = dsp_vptr->pb;

    AddDpop(&stp->hostDPopSum.l, (s16)pb->dpop.aL);
    AddDpop(&stp->hostDPopSum.r, (s16)pb->dpop.aR);

    if ((pb->mixerCtrl & 0x04) != 0) {
        AddDpop(&stp->hostDPopSum.s, (s16)pb->dpop.aS);
    }

    if ((pb->mixerCtrl & 0x01) != 0) {
        AddDpop(&stp->hostDPopSum.lA, (s16)pb->dpop.aAuxAL);
        AddDpop(&stp->hostDPopSum.rA, (s16)pb->dpop.aAuxAR);

        if ((pb->mixerCtrl & 0x14) != 0) {
            AddDpop(&stp->hostDPopSum.sA, (s16)pb->dpop.aAuxAS);
        }
    }

    if ((pb->mixerCtrl & 0x12) != 0) {
        AddDpop(&stp->hostDPopSum.lB, (s16)pb->dpop.aAuxBL);
        AddDpop(&stp->hostDPopSum.rB, (s16)pb->dpop.aAuxBR);

        if ((pb->mixerCtrl & 0x04) != 0) {
            AddDpop(&stp->hostDPopSum.sB, (s16)pb->dpop.aAuxBS);
        }
    }
}

void SortVoices(DSPvoice** voices, s32 l, s32 r) {
    s32 i;
    s32 last;
    DSPvoice* tmp;

    if (l >= r) {
        return;
    }

    tmp = voices[l];
    voices[l] = voices[(l + r) / 2];
    voices[(l + r) / 2] = tmp;
    last = l;
    i = l + 1;

    for (; i <= r; ++i) {
        if (voices[i]->prio < voices[l]->prio) {
            last += 1;
            tmp = voices[last];
            voices[last] = voices[i];
            voices[i] = tmp;
        }
    }

    tmp = voices[l];
    voices[l] = voices[last];
    voices[last] = tmp;
    SortVoices(voices, l, last - 1);
    SortVoices(voices, last + 1, r);
}

extern void DoDepopFade(s32* dspStart, s16* dspDelta, s32* hostSum);
extern void sal_setup_dspvol(u16* dsp_delta, u16* last_vol, u16 vol);
extern void sal_update_hostplayinfo(DSPvoice* dsp_vptr);
extern void DCFlushRangeNoSync(void* addr, u32 nBytes);

void vsSampleEndNotify(u32 pubID) {
    u8* entry;
    typedef void (*VSampleCallback)(u32 reason, u32* userValue);
    u32 offset;
    u8* idMap;
    u8 id;

    if (pubID != 0xFFFFFFFF) {
        idMap = lbl_80446F10 + 0x908;
        entry = &idMap[(u8)pubID];
        id = *entry;
        if (id != 0xFF) {
            offset = id * 0x24;
            if (*(u16*)(lbl_80446F10 + offset + 0x1A) == (u16)(pubID >> 8)) {
                VSampleCallback callback = *(VSampleCallback*)(lbl_80446F10 + 0x94C);
                if (callback != 0) {
                    offset = id * 0x24;
                    callback(2, (u32*)(lbl_80446F10 + offset + 0x18));
                }
                lbl_80446F10[offset + 8] = 0;
                idMap[lbl_80446F10[offset + 0xB]] = 0xFF;
            }
        }
    }
}
void salInitHRTFBuffer(void) {
    extern u32 lbl_8047B018;

    memset((void*)lbl_8047B018, 0, 0x100);
    DCFlushRangeNoSync((void*)lbl_8047B018, 0x100);
}

u32 salInitDspCtrl(u8 numVoices, u8 numStudios, u8 defaultStudioDPL2) {
    extern u32 fn_80163798(void);
    extern void DCInvalidateRange(void* addr, u32 nBytes);
    extern void DCStoreRangeNoSync(void* addr, u32 nBytes);
    u32 i;
    u32 j;
    u32 itdPtr;
    DSPvoice* dv;
    DSPstudioinfo* sp;

    lbl_8047B05D = numVoices;
    salMaxStudioNum = numStudios;
    dspARAMZeroBuffer = fn_80163798();

    lbl_8047B010 = fn_801643D8(1024 * sizeof(u16));
    if (lbl_8047B010 != 0) {
        lbl_8047B01C = (u16*)fn_801643D8(160 * sizeof(s32));
        if (lbl_8047B01C != NULL) {
            memset(lbl_8047B01C, 0, 160 * sizeof(s32));
            DCFlushRange(lbl_8047B01C, 160 * sizeof(s32));

            lbl_8047B024 =
                fn_801643D8((u32)numVoices * sizeof(DSPvoice));
            if (lbl_8047B024 != 0) {
                lbl_8047B020 = fn_801643D8((u32)numVoices * 64);
                if (lbl_8047B020 != 0) {
                    DCInvalidateRange((void*)lbl_8047B020,
                                      (u32)numVoices * 64);
                    itdPtr = lbl_8047B020;
                    dv = (DSPvoice*)lbl_8047B024;

                    for (i = 0; i < numVoices; ++i) {
                        dv[i].state = 0;
                        dv[i].postBreak = 0;
                        dv[i].startupBreak = 0;
                        dv[i].lastUpdate.pitch = 0xFF;
                        dv[i].lastUpdate.vol = 0xFF;
                        dv[i].lastUpdate.volA = 0xFF;
                        dv[i].lastUpdate.volB = 0xFF;
                        dv[i].pb = (_PB*)fn_801643D8(sizeof(_PB));
                        memset(dv[i].pb, 0, sizeof(_PB));
                        dv[i].patchData = (void*)fn_801643D8(0x80);
                        dv[i].pb->currHi = (u32)dv[i].pb >> 16;
                        dv[i].pb->currLo = (u16)(u32)dv[i].pb;
                        dv[i].pb->update.dataHi =
                            (u32)dv[i].patchData >> 16;
                        dv[i].pb->update.dataLo =
                            (u16)(u32)dv[i].patchData;
                        dv[i].pb->itd.bufferHi = itdPtr >> 16;
                        dv[i].pb->itd.bufferLo = (u16)itdPtr;
                        dv[i].itdBuffer = (void*)itdPtr;
                        itdPtr += 0x40;
                        dv[i].virtualSampleID = 0xFFFFFFFF;
                        DCStoreRangeNoSync(dv[i].pb, sizeof(_PB));
                        for (j = 0; j < 5; ++j) {
                            dv[i].changed[j] = 0;
                        }
                    }

                    sp = dspStudio;
                    for (i = 0; i < salMaxStudioNum; ++i) {
                        sp[i].state = 0;
                        sp[i].spb = (void*)fn_801643D8(sizeof(_SPB));
                        if (sp[i].spb == NULL) {
                            return 0;
                        }

                        sp[i].main[0] = (s32*)fn_801643D8(0x3C00);
                        if (sp[i].main[0] == NULL) {
                            return 0;
                        }

                        memset(sp[i].main[0], 0, 0x3C00);
                        DCFlushRangeNoSync(sp[i].main[0], 0x3C00);
                        sp[i].main[1] = sp[i].main[0] + 0x1E0;
                        sp[i].auxA[0] = sp[i].main[1] + 0x1E0;
                        sp[i].auxA[1] = sp[i].auxA[0] + 0x1E0;
                        sp[i].auxA[2] = sp[i].auxA[1] + 0x1E0;
                        sp[i].auxB[0] = sp[i].auxA[2] + 0x1E0;
                        sp[i].auxB[1] = sp[i].auxB[0] + 0x1E0;
                        sp[i].auxB[2] = sp[i].auxB[1] + 0x1E0;
                        memset(sp[i].spb, 0, sizeof(_SPB));
                        sp[i].hostDPopSum.l = sp[i].hostDPopSum.r =
                            sp[i].hostDPopSum.s = 0;
                        sp[i].hostDPopSum.lA = sp[i].hostDPopSum.rA =
                            sp[i].hostDPopSum.sA = 0;
                        sp[i].hostDPopSum.lB = sp[i].hostDPopSum.rB =
                            sp[i].hostDPopSum.sB = 0;
                        DCFlushRangeNoSync(sp[i].spb, sizeof(_SPB));
                    }

                    salActivateStudio(0, 1,
                                      defaultStudioDPL2 != 0 ? 1 : 0);
                    lbl_8047B018 = fn_801643D8(0x100);
                    if (lbl_8047B018 == 0) {
                        return 0;
                    }

                    salInitHRTFBuffer();
                    return 1;
                }
            }
        }
    }

    return 0;
}

void salActivateStudio(studio, isMaster, type)
u8 studio;
u32 isMaster;
s32 type;
{
    DSPstudioinfo* sp = &dspStudio[studio];

    memset(sp->main[0], 0, 0x3C00);
    DCFlushRangeNoSync(sp->main[0], 0x3C00);
    memset(sp->spb, 0, sizeof(_SPB));
    sp->hostDPopSum.l = sp->hostDPopSum.r = sp->hostDPopSum.s = 0;
    sp->hostDPopSum.lA = sp->hostDPopSum.rA = sp->hostDPopSum.sA = 0;
    sp->hostDPopSum.lB = sp->hostDPopSum.rB = sp->hostDPopSum.sB = 0;
    DCFlushRangeNoSync(sp->spb, sizeof(_SPB));
    memset(sp->auxA[0], 0, 0x780);
    DCFlushRangeNoSync(sp->auxA[0], 0x780);
    memset(sp->auxB[0], 0, 0x780);
    DCFlushRangeNoSync(sp->auxB[0], 0x780);
    sp->voiceRoot = NULL;
    sp->alienVoiceRoot = NULL;
    sp->state = 1;
    sp->isMaster = isMaster;
    sp->numInputs = 0;
    sp->type = type;
    sp->auxAHandler = sp->auxBHandler = NULL;
}

typedef struct MusyxAuxInfo {
    struct {
        struct {
            s32* left;
            s32* right;
            s32* surround;
        } bufferUpdate;
    } data;
} MusyxAuxInfo;

typedef void (*MusyxAuxCallback)(u8 reason, MusyxAuxInfo* info, void* user);

void salHandleAuxProcessing(void) {
    u8 studio;
    s32* work;
    DSPstudioinfo* sp;
    MusyxAuxInfo info;

    for (sp = dspStudio, studio = 0; studio < salMaxStudioNum;
         ++studio, ++sp) {
        if (sp->state != 1) {
            continue;
        }

        if (sp->auxAHandler != NULL) {
            work = sp->auxA[(salAuxFrame + 2) % 3];
            info.data.bufferUpdate.left = work;
            info.data.bufferUpdate.right = work + 0xA0;
            info.data.bufferUpdate.surround = work + 0x140;
            ((MusyxAuxCallback)sp->auxAHandler)(0, &info, sp->auxAUser);
            DCFlushRangeNoSync(work, 0x780);
        }

        if (sp->type == 0 && sp->auxBHandler != NULL) {
            work = sp->auxB[(salAuxFrame + 2) % 3];
            info.data.bufferUpdate.left = work;
            info.data.bufferUpdate.right = work + 0xA0;
            info.data.bufferUpdate.surround = work + 0x140;
            ((MusyxAuxCallback)sp->auxBHandler)(0, &info, sp->auxBUser);
            DCFlushRangeNoSync(work, 0x780);
        }
    }
}

void fn_8015AAA0(u32 studio) {
    lbl_80447E60[(u8)studio].named.state = 0;
}

void sal_setup_dspvol(u16* dsp_delta, u16* last_vol, u16 vol) {
    *dsp_delta = ((s16)vol - (s16)*last_vol) / 160;
    *last_vol += (s16)*dsp_delta * 160;
}

void sal_update_hostplayinfo(DSPvoice* dsp_vptr) {
    u32 pitch;
    u32 oldLo;

    if (*(u32*)((u8*)dsp_vptr + 0x8C) != 0) {
        return;
    }
    if (dsp_vptr->pb->srcSelect != 2) {
        pitch = dsp_vptr->playInfo.pitch << 5;
    } else {
        pitch = 0x200000;
    }
    oldLo = dsp_vptr->playInfo.posLo;
    dsp_vptr->playInfo.posLo += pitch << 16;
    if (oldLo > dsp_vptr->playInfo.posLo) {
        dsp_vptr->playInfo.posHi += (pitch >> 16) + 1;
    } else {
        dsp_vptr->playInfo.posHi += pitch >> 16;
    }
}

void DoDepopFade(s32* dspStart, s16* dspDelta, s32* hostSum) {
    s16 delta;

    if (*hostSum <= -160) {
        if (*hostSum <= -3200) {
            delta = 20;
        } else {
            delta = (s16)(-*hostSum / 160);
        }
        *dspDelta = delta;
    } else if (*hostSum >= 160) {
        if (*hostSum >= 3200) {
            delta = -20;
        } else {
            delta = (s16)(-*hostSum / 160);
        }
        *dspDelta = delta;
    } else {
        *dspDelta = 0;
    }
    *dspStart = *hostSum;
    *hostSum += *dspDelta * 160;
}

u32 salSynthSendMessage(DSPvoice* dsp_vptr, u32 mesg) {
    typedef u32 (*SynthMessageCallback)(u32 mesg, u32 userValue);
    extern u32 lbl_8047B028;
    SynthMessageCallback callback = (SynthMessageCallback)lbl_8047B028;

    if (callback == 0) {
        return 0;
    }
    return callback(mesg, dsp_vptr->mesgCallBackUserValue);
}

void salActivateVoice(u8* voice, u8 studio) {
    DSPvoice* dsp_vptr = (DSPvoice*)voice;
    u8* studioData;
    extern void salDeactivateVoice(void* dsp_vptr);

    if (dsp_vptr->state != 0) {
        salDeactivateVoice(dsp_vptr);
        dsp_vptr->changed[0] |= 0x20;
    }

    dsp_vptr->postBreak = 0;
    studioData = lbl_80447E60[0].bytes;
    studioData += (u8)studio * 0xBC;
    if ((dsp_vptr->next = *(DSPvoice**)(studioData += 0x48)) != 0) {
        dsp_vptr->next->prev = dsp_vptr;
    }
    dsp_vptr->prev = 0;
    *(DSPvoice**)studioData = dsp_vptr;
    dsp_vptr->startupBreak = 0;
    dsp_vptr->state = 1;
    dsp_vptr->studio = studio;
}

void salDeactivateVoice(void* voice) {
    DSPvoice* dsp_vptr = voice;
    if (dsp_vptr->state == 0) {
        return;
    }
    if (dsp_vptr->prev != 0) {
        dsp_vptr->prev->next = dsp_vptr->next;
    } else {
        DSPstudioinfo* studioData = (DSPstudioinfo*)lbl_80447E60;
        studioData[dsp_vptr->studio].voiceRoot = dsp_vptr->next;
    }
    if (dsp_vptr->next != 0) {
        dsp_vptr->next->prev = dsp_vptr->prev;
    }
    dsp_vptr->state = 0;
}

u32 fn_8015D54C(void* studio, void* data) {
    DSPstudioinfo* stp = studio;
    u8* input = data;

    if (stp->numInputs < 7) {
        stp->in[stp->numInputs].studio = input[3];
        stp->in[stp->numInputs].vol = ((u16)input[0] << 8) | ((u16)input[0] << 1);
        stp->in[stp->numInputs].volA = ((u16)input[1] << 8) | ((u16)input[1] << 1);
        stp->in[stp->numInputs].volB = ((u16)input[2] << 8) | ((u16)input[2] << 1);
        stp->in[stp->numInputs].desc = input;
        stp->numInputs++;
        return 1;
    }
    return 0;
}

u32 fn_8015D5F4(DSPstudioinfo* stp, void* input) {
    long i;

    for (i = 0; i < stp->numInputs; i++) {
        if (stp->in[i].desc == input) {
            for (; i <= stp->numInputs - 2; i++) {
                stp->in[i] = stp->in[i + 1];
            }
            stp->numInputs--;
            return 1;
        }
    }
    return 0;
}

u32 salCheckVolErrorAndResetDelta(u16* dsp_vol, u16* dsp_delta, u16* last_vol, u16 targetVol,
                                  u16* resetFlags, u16 resetMask) {
  s16 d;
  s16 x;

  if (targetVol != *last_vol) {
    d = (s16)targetVol - (s16)*last_vol;
    if ((s16)d >= 32 && (s16)d < 160) {
      x = (s16)d >> 5;
      if ((s16)x < 5) {
        resetFlags[x] |= resetMask;
      }

      *dsp_delta = 1;
      *last_vol += (x << 5);
      return 1;
    }

    if (-32 >= (s16)d && -160 < (s16)d) {
      x = -(s16)d >> 5;
      if (x < 5) {
        resetFlags[x] |= resetMask;
      }
      *dsp_delta = 0xFFFF;
      *last_vol -= x << 5;
      return 1;
    }

    if (targetVol == 0 && (s16)d > -32) {
      *dsp_vol = *last_vol = 0;
    }
  }

  *dsp_delta = 0;
  return 0;
}
extern void fn_8015AD1C(DSPstudioinfo* stp, DSPvoice* dsp_vptr); /* HandleDepopVoice */
extern void DCStoreRangeNoSync(void* addr, u32 nBytes);
extern void DCFlushRangeNoSync(void* addr, u32 nBytes);

/* Ensure `n` more u16 slots fit before dspCmdMaxPtr; if not, chain onto a
 * fresh command buffer (MORE). Written as a macro (not a helper function)
 * because retail repeats this exact sequence inline at every call site
 * (confirmed: no shared `bl` target for it anywhere in the disassembly). */
#define DSP_CMD_ENSURE(n)                                                     \
    if ((dspCmdPtr + (n)) > (dspCmdMaxPtr - 4)) {                             \
        u16 size;                                                            \
        dspCmdPtr[0] = 13;                                                   \
        dspCmdPtr[1] = (u32)dspCmdMaxPtr >> 16;                              \
        dspCmdPtr[2] = (u32)dspCmdMaxPtr;                                    \
        size = (u16)(((u32)(dspCmdPtr + 4) - (u32)dspCmdCurBase) + 3) & ~3;  \
        if (dspCmdLastLoad) {                                                \
            dspCmdLastLoad[3] = size;                                        \
            DCStoreRangeNoSync(dspCmdLastBase, dspCmdLastSize);              \
        } else {                                                             \
            dspCmdFirstSize = size;                                          \
        }                                                                    \
        dspCmdLastLoad = dspCmdPtr;                                          \
        dspCmdLastSize = size;                                               \
        dspCmdLastBase = dspCmdCurBase;                                      \
        dspCmdCurBase = dspCmdPtr = dspCmdMaxPtr;                            \
        dspCmdMaxPtr = dspCmdPtr + 0xC0;                                     \
    }

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_8015B250(u32 dest, u32 nsDelay) {
    static const u16 pbOffsets[9] = {10, 12, 24, 14, 16, 26, 18, 20, 22};

    u8 s;
    u8 mix_start;
    u8 st;
    u8 st1;
    u8 getAuxFrame;
    u16 rampResetOffsetFlags[5];
    DSPvoice* dsp_vptr;
    DSPvoice* next_dsp_vptr;
    u32 tmp_addr;
    u32 addr;
    u32 base;
    u32 in;
    u32 voiceNum;
    u32 cyclesUsed;
    u16* pptr;
    u16* pend;
    u16 adsr_start;
    u16 adsr_delta;
    u16 old_adsr_delta;
    s32 current_delta;
    s32 v;
    _PB* pb;
    _PB* last_pb;
    u32 VoiceDone;
    u32 needsDelta;

    u32 newVoice;
    _SPB* spb;
    DSPstudioinfo* stp;
    u32 procVoiceFlag;
    u32 offset;
    u32 endAddr;
    u32 loopAddr;
    u32 zeroAddr;
    DSPvoice* sp78;
    DSPvoice* sp74;

    dspCmdCurBase = dspCmdPtr = dspCmdList;
    dspCmdMaxPtr = dspCmdPtr + 0xC0;
    dspCmdLastLoad = NULL;
    if (nsDelay < 200) {
        cyclesUsed = 10430;
    } else {
        cyclesUsed = ((nsDelay - 200) * ((*(volatile u32*)0x800000F8 / 400) / 5000)) + 10430;
    }
    if (dspHRTFOn != FALSE) {
        cyclesUsed += 45000;
    }
    rampResetOffsetFlags[0] = 0;
    for (st = 0; st < salMaxStudioNum; st++) {
        if (dspStudio[st].state == 1) {
            stp = &dspStudio[st];
            for (dsp_vptr = stp->voiceRoot; dsp_vptr; dsp_vptr = next_dsp_vptr) {
                next_dsp_vptr = dsp_vptr->next;
                if ((dsp_vptr->postBreak != 0) || ((dsp_vptr->changed[0] & 0x20) != 0)) {
                    fn_8015AD1C(stp, dsp_vptr);
                    if (dsp_vptr->virtualSampleID != (u32)-1) {
                        salSynthSendMessage(dsp_vptr, 3);
                    }
                    if ((dsp_vptr->state != 1) || (dsp_vptr->startupBreak != 0)) {
                        extern void salDeactivateVoice(void* dsp_vptr);
                        salDeactivateVoice(dsp_vptr);
                        dsp_vptr->startupBreak = 0;
                    }
                }
            }
            dsp_vptr = stp->alienVoiceRoot;
            while (dsp_vptr) {
                fn_8015AD1C(stp, dsp_vptr);
                dsp_vptr = dsp_vptr->nextAlien;
            }
            stp->alienVoiceRoot = NULL;

            DSP_CMD_ENSURE(3)

            dspCmdPtr[0] = 0; /* SETUP */
            dspCmdPtr[1] = (u32)stp->spb >> 16;
            dspCmdPtr[2] = (u32)stp->spb;
            dspCmdPtr += 3;
            cyclesUsed += 0x2C62;
            for (in = 0; in < stp->numInputs; in++) {
                DSP_CMD_ENSURE(6)

                dspCmdPtr[0] = 1; /* DL_AND_VOL_MIX */
                dspCmdPtr[1] = (u32)dspStudio[stp->in[in].studio].main[salFrame ^ 1] >> 16;
                dspCmdPtr[2] = (u32)dspStudio[stp->in[in].studio].main[salFrame ^ 1];
                dspCmdPtr[3] = stp->in[in].vol;
                dspCmdPtr[4] = stp->in[in].volA;
                dspCmdPtr[5] = stp->in[in].volB;
                dspCmdPtr += 6;
                cyclesUsed += 0x294D;
            }
            last_pb = NULL;
            for (v = 0, dsp_vptr = stp->voiceRoot, sp78 = dsp_vptr; dsp_vptr;
                 v++, dsp_vptr = dsp_vptr->next, sp74 = dsp_vptr) {
                voices[v] = dsp_vptr;
            }
            voiceNum = (u32)v;
            SortVoices(voices, 0, voiceNum - 1);
            procVoiceFlag = 0;
            for (v = voiceNum; v > 0; v--) {
                dsp_vptr = voices[v - 1];
                if (dsp_vptr->state != 0) {
                    u8 i;
                    pb = dsp_vptr->pb;
                    for (s = 1; s < 5; s++) {
                        rampResetOffsetFlags[s] = 0;
                    }
                    if (dsp_vptr->state == 1) {
                        dsp_vptr->virtualSampleID = (u32)-1;
                        dsp_vptr->pb->ve.currentDelta = 0x8000;
                        if (adsrSetup(&dsp_vptr->adsr) != 0) {
                            salSynthSendMessage(dsp_vptr, 0);
                            {
                                extern void salDeactivateVoice(void* dsp_vptr);
                                salDeactivateVoice(dsp_vptr);
                            }
                            continue;
                        }
                        dsp_vptr->virtualSampleID = (u32)-1;
                        switch (dsp_vptr->smp_info.compType) {
                        case 5:
                            dsp_vptr->vSampleInfo.loopBufferLength = 0;
                            dsp_vptr->virtualSampleID = salSynthSendMessage(dsp_vptr, 2);
                            if (dsp_vptr->vSampleInfo.loopBufferLength == 0) {
                                salSynthSendMessage(dsp_vptr, 1);
                                {
                                    extern void salDeactivateVoice(void* dsp_vptr);
                                    salDeactivateVoice(dsp_vptr);
                                }
                                continue;
                            }
                            break;
                        }
                        pb->src.currentAddressFrac = 0;
                        pb->src.last_samples[0] = 0;
                        pb->src.last_samples[1] = 0;
                        pb->src.last_samples[2] = 0;
                        pb->src.last_samples[3] = 0;
                        if ((dsp_vptr->flags & 0x80000000) != 0) {
                            memset(dsp_vptr->itdBuffer, 0, 0x40);
                            DCFlushRange(dsp_vptr->itdBuffer, 0x40);
                            pb->itd.targetShiftL = dsp_vptr->itdShiftL;
                            pb->itd.shiftL = dsp_vptr->itdShiftL;
                            pb->itd.targetShiftR = dsp_vptr->itdShiftR;
                            pb->itd.shiftR = dsp_vptr->itdShiftR;
                            pb->itd.flag = 1;
                        } else {
                            pb->itd.flag = 0;
                        }
                        switch (dsp_vptr->smp_info.compType) {
                        case 0:
                        case 4:
                        case 5: {
                            SNDADPCMinfo* adpcmInfo;
                            u8 i;
                            pb->addr.format = 0;
                            pb->adpcm.gain = 0;
                            adpcmInfo = dsp_vptr->smp_info.extraData;
                            pb->adpcm.yn2 = 0;
                            pb->adpcm.yn1 = 0;
                            pb->adpcm.pred_scale = adpcmInfo->initialPS;
                            for (i = 0; i < 8; i++) {
                                pb->adpcm.a[i][0] = adpcmInfo->coefTab[i][0];
                                pb->adpcm.a[i][1] = adpcmInfo->coefTab[i][1];
                            }
                            base = (u32)dsp_vptr->smp_info.addr * 2;
                            addr = base + 2;
                            dsp_vptr->playInfo.posHi = dsp_vptr->playInfo.posLo = 0;
                            if ((dsp_vptr->smp_info.compType == 4) || (dsp_vptr->smp_info.compType == 5)) {
                                pb->loopType = 1;
                            } else {
                                pb->adpcmLoop.loop_yn2 = adpcmInfo->loopY0;
                                pb->adpcmLoop.loop_yn1 = adpcmInfo->loopY1;
                                pb->adpcmLoop.loop_pred_scale = adpcmInfo->loopPS;
                                pb->loopType = 0;
                            }
                        } break;
                        case 1: {
                            DSPADPCMplusInfo* adpcmInfo;
                            u8 i;
                            pb->addr.format = 0;
                            pb->adpcm.gain = 0;
                            offset = (dsp_vptr->smp_info.offset + 0xD) / 14;
                            adpcmInfo = dsp_vptr->smp_info.extraData;
                            pb->adpcm.yn2 = adpcmInfo->blk[offset].Y0;
                            pb->adpcm.yn1 = adpcmInfo->blk[offset].Y1;
                            pb->adpcm.pred_scale = adpcmInfo->blk[offset].PS;
                            pb->adpcmLoop.loop_yn2 = adpcmInfo->loopY0;
                            pb->adpcmLoop.loop_yn1 = adpcmInfo->loopY1;
                            pb->adpcmLoop.loop_pred_scale = adpcmInfo->loopPS;
                            for (i = 0; i < 8; i++) {
                                pb->adpcm.a[i][0] = adpcmInfo->coefTab[i][0];
                                pb->adpcm.a[i][1] = adpcmInfo->coefTab[i][1];
                            }
                            base = (u32)dsp_vptr->smp_info.addr * 2;
                            addr = base + offset * 16 + 2;
                            dsp_vptr->playInfo.posHi = offset * 0xE;
                            dsp_vptr->playInfo.posLo = 0;
                        } break;
                        case 3: {
                            u8 i;
                            pb->addr.format = 0x19;
                            pb->adpcm.gain = 0x100;
                            for (i = 0; i < 8; i++) {
                                pb->adpcm.a[i][0] = 0;
                                pb->adpcm.a[i][1] = 0;
                            }
                            addr = (u32)dsp_vptr->smp_info.offset + (base = (u32)dsp_vptr->smp_info.addr);
                            dsp_vptr->playInfo.posHi = dsp_vptr->smp_info.offset;
                            dsp_vptr->playInfo.posLo = 0;
                        } break;
                        case 2: {
                            u8 i;
                            pb->addr.format = 0xA;
                            pb->adpcm.gain = 0x800;
                            for (i = 0; i < 8; i++) {
                                pb->adpcm.a[i][0] = 0;
                                pb->adpcm.a[i][1] = 0;
                            }
                            addr = dsp_vptr->smp_info.offset + (base = (u32)dsp_vptr->smp_info.addr >> 1);
                            dsp_vptr->playInfo.posHi = dsp_vptr->smp_info.offset;
                            dsp_vptr->playInfo.posLo = 0;
                        } break;
                        default:
                            break;
                        }
                        pb->addr.currentAddressHi = addr >> 0x10;
                        pb->addr.currentAddressLo = addr;
                        dsp_vptr->currentAddr = addr;
                        if (dsp_vptr->smp_info.loopLength != 0) {
                            pb->addr.loopFlag = 1;
                            switch (dsp_vptr->smp_info.compType) {
                            case 0:
                            case 1:
                            case 4: {
                                u32 bn, bo;
                                bn = dsp_vptr->smp_info.loop / 14;
                                bo = dsp_vptr->smp_info.loop - (bn * 0xE);
                                loopAddr = base + bn * 16 + 2 + bo;
                                endAddr = dsp_vptr->smp_info.loop + dsp_vptr->smp_info.loopLength - 1;
                                bn = endAddr / 14;
                                bo = endAddr - (bn * 0xE);
                                endAddr = base + bn * 16 + 2 + bo;
                            } break;
                            case 5: {
                                u32 bn, bo;
                                loopAddr = ((u32)dsp_vptr->vSampleInfo.loopBufferAddr * 2) + 2;
                                endAddr = dsp_vptr->smp_info.loop + dsp_vptr->smp_info.loopLength - 1;
                                bn = endAddr / 14;
                                bo = endAddr - (bn * 0xE);
                                endAddr = base + bn * 16 + 2 + bo;
                                dsp_vptr->vSampleInfo.inLoopBuffer = 0;
                            } break;
                            case 2:
                            case 3:
                            default:
                                loopAddr = base + dsp_vptr->smp_info.loop;
                                endAddr = base + dsp_vptr->smp_info.loop + dsp_vptr->smp_info.loopLength - 1;
                                break;
                            }
                            pb->addr.loopAddressHi = loopAddr >> 16;
                            pb->addr.loopAddressLo = loopAddr;
                            pb->addr.endAddressHi = endAddr >> 16;
                            pb->addr.endAddressLo = endAddr;
                            pb->streamLoopCnt = 0;
                        } else {
                            pb->addr.loopFlag = 0;
                            switch (dsp_vptr->smp_info.compType) {
                            case 0:
                            case 1:
                            case 4:
                            case 5: {
                                u32 bn, bo;
                                bn = dsp_vptr->smp_info.length / 14;
                                bo = dsp_vptr->smp_info.length - (bn * 0xE);
                                tmp_addr = base + bn * 16 + 2 + bo;
                                zeroAddr = (dspARAMZeroBuffer * 2) + 2;
                            } break;
                            case 3:
                                tmp_addr = base + dsp_vptr->smp_info.length;
                                zeroAddr = dspARAMZeroBuffer;
                                break;
                            case 2:
                            default:
                                tmp_addr = base + dsp_vptr->smp_info.length;
                                zeroAddr = dspARAMZeroBuffer >> 1;
                                break;
                            }
                            pb->addr.loopAddressHi = zeroAddr >> 16;
                            pb->addr.loopAddressLo = zeroAddr;
                            pb->addr.endAddressHi = tmp_addr >> 16;
                            pb->addr.endAddressLo = tmp_addr;
                        }
                        pb->srcSelect = dsp_vptr->srcTypeSelect;
                        pb->coefSelect = dsp_vptr->srcCoefSelect;

                        pb->state = (mix_start = dsp_vptr->singleOffset) ? 0 : 1;
                        pb->mix.vL = dsp_vptr->lastVolL = dsp_vptr->volL;
                        pb->mix.vR = dsp_vptr->lastVolR = dsp_vptr->volR;
                        pb->mix.vS = dsp_vptr->lastVolS = dsp_vptr->volS;
                        pb->mix.vAuxAL = dsp_vptr->lastVolLa = dsp_vptr->volLa;
                        pb->mix.vAuxAR = dsp_vptr->lastVolRa = dsp_vptr->volRa;
                        pb->mix.vAuxAS = dsp_vptr->lastVolSa = dsp_vptr->volSa;

                        pb->mixerCtrl = (pb->mix.vAuxAS | (pb->mix.vAuxAL | pb->mix.vAuxAR)) != 0 ? 1 : 0;

                        pb->mix.vAuxBL = dsp_vptr->lastVolLb = dsp_vptr->volLb;
                        pb->mix.vAuxBR = dsp_vptr->lastVolRb = dsp_vptr->volRb;
                        pb->mix.vAuxBS = dsp_vptr->lastVolSb = dsp_vptr->volSb;
                        pb->mix.vDeltaL = 0;
                        pb->mix.vDeltaR = 0;
                        pb->mix.vDeltaS = 0;
                        pb->mix.vDeltaAuxAL = 0;
                        pb->mix.vDeltaAuxAR = 0;
                        pb->mix.vDeltaAuxAS = 0;
                        pb->mix.vDeltaAuxBL = 0;
                        pb->mix.vDeltaAuxBR = 0;
                        pb->mix.vDeltaAuxBS = 0;
                        if (stp->type == 0 /* SND_STUDIO_TYPE_STD */) {
                            if ((pb->mix.vAuxBS | (pb->mix.vAuxBL | pb->mix.vAuxBR)) != 0) {
                                pb->mixerCtrl |= 2;
                            }
                            if ((pb->mix.vAuxBS | (pb->mix.vS | pb->mix.vAuxAS)) != 0) {
                                pb->mixerCtrl |= 4;
                            }
                        } else if ((pb->mix.vAuxAS | (pb->mix.vAuxBL | pb->mix.vAuxBR)) != 0) {
                            pb->mixerCtrl |= 0x10;
                        }
                        dsp_vptr->state = 2;
                        newVoice = 1;
                        goto block_186;
                    }
                    if ((dsp_vptr->smp_info.compType == 4) || (dsp_vptr->smp_info.compType == 5)) {
                        pb->adpcmLoop.loop_pred_scale = dsp_vptr->streamLoopPS;
                        if ((dsp_vptr->smp_info.compType == 5) && (dsp_vptr->vSampleInfo.inLoopBuffer == 0) &&
                            (pb->streamLoopCnt != 0)) {
                            u32 bn, bo;
                            bn = (dsp_vptr->vSampleInfo.loopBufferLength - 1) / 14;
                            bo = (dsp_vptr->vSampleInfo.loopBufferLength - 1) - (bn * 14);
                            tmp_addr = ((u32)dsp_vptr->vSampleInfo.loopBufferAddr * 2) + bn * 16 + 2 + bo;
                            dsp_vptr->smp_info.addr = dsp_vptr->vSampleInfo.loopBufferAddr;
                            pb->addr.endAddressHi = tmp_addr >> 0x10;
                            pb->addr.endAddressLo = tmp_addr;
                            dsp_vptr->vSampleInfo.inLoopBuffer = 1;
                        }
                    }
                    if ((dsp_vptr->smp_info.loopLength == 0) &&
                        (dsp_vptr->playInfo.posHi >= dsp_vptr->smp_info.length)) {
                        salSynthSendMessage(dsp_vptr, 0);
                        {
                            extern void salDeactivateVoice(void* dsp_vptr);
                            salDeactivateVoice(dsp_vptr);
                        }
                        continue;
                    }
                    if (((dsp_vptr->changed[0] & 0x10) != 0) && (adsrSetup(&dsp_vptr->adsr) != 0)) {
                        salSynthSendMessage(dsp_vptr, 0);
                        {
                            extern void salDeactivateVoice(void* dsp_vptr);
                            salDeactivateVoice(dsp_vptr);
                        }
                        continue;
                    }
                    if ((dsp_vptr->changed[0] & 1) != 0) {
                        sal_setup_dspvol(&pb->mix.vDeltaL, &dsp_vptr->lastVolL, dsp_vptr->volL);
                        sal_setup_dspvol(&pb->mix.vDeltaR, &dsp_vptr->lastVolR, dsp_vptr->volR);
                        sal_setup_dspvol(&pb->mix.vDeltaS, &dsp_vptr->lastVolS, dsp_vptr->volS);
                        needsDelta = 1;
                    } else {
                        needsDelta = salCheckVolErrorAndResetDelta(&pb->mix.vL, &pb->mix.vDeltaL,
                                                                    &dsp_vptr->lastVolL, dsp_vptr->volL,
                                                                    rampResetOffsetFlags, 1);
                        needsDelta |= salCheckVolErrorAndResetDelta(&pb->mix.vR, &pb->mix.vDeltaR,
                                                                     &dsp_vptr->lastVolR, dsp_vptr->volR,
                                                                     rampResetOffsetFlags, 2);
                        needsDelta |= salCheckVolErrorAndResetDelta(&pb->mix.vS, &pb->mix.vDeltaS,
                                                                     &dsp_vptr->lastVolS, dsp_vptr->volS,
                                                                     rampResetOffsetFlags, 4);
                    }
                    if ((dsp_vptr->changed[0] & 2) != 0) {
                        sal_setup_dspvol(&pb->mix.vDeltaAuxAL, &dsp_vptr->lastVolLa, dsp_vptr->volLa);
                        sal_setup_dspvol(&pb->mix.vDeltaAuxAR, &dsp_vptr->lastVolRa, dsp_vptr->volRa);
                        sal_setup_dspvol(&pb->mix.vDeltaAuxAS, &dsp_vptr->lastVolSa, dsp_vptr->volSa);

                        if ((pb->mix.vDeltaAuxAS | (pb->mix.vDeltaAuxAL | pb->mix.vDeltaAuxAR)) != 0) {
                            pb->mixerCtrl |= 1;
                            needsDelta = 1;
                        } else if ((pb->mix.vAuxAS | (pb->mix.vAuxAL | pb->mix.vAuxAR)) != 0) {
                            pb->mixerCtrl |= 1;
                        } else {
                            pb->mixerCtrl &= ~1;
                        }
                    } else if ((pb->mixerCtrl & 1) != 0) {
                        u32 localNeedsDelta;
                        localNeedsDelta = salCheckVolErrorAndResetDelta(
                            &pb->mix.vAuxAL, &pb->mix.vDeltaAuxAL, &dsp_vptr->lastVolLa, dsp_vptr->volLa,
                            rampResetOffsetFlags, 8);
                        localNeedsDelta |= salCheckVolErrorAndResetDelta(
                            &pb->mix.vAuxAR, &pb->mix.vDeltaAuxAR, &dsp_vptr->lastVolRa, dsp_vptr->volRa,
                            rampResetOffsetFlags, 0x10);
                        localNeedsDelta |= salCheckVolErrorAndResetDelta(
                            &pb->mix.vAuxAS, &pb->mix.vDeltaAuxAS, &dsp_vptr->lastVolSa, dsp_vptr->volSa,
                            rampResetOffsetFlags, 0x20);
                        if ((localNeedsDelta | (pb->mix.vAuxAS | (pb->mix.vAuxAL | pb->mix.vAuxAR))) == 0) {
                            pb->mixerCtrl &= ~1;
                        } else {
                            needsDelta = 1;
                        }
                    } else {
                        pb->mix.vDeltaAuxAL = 0;
                        pb->mix.vDeltaAuxAR = 0;
                        pb->mix.vDeltaAuxAS = 0;
                    }
                    if ((dsp_vptr->changed[0] & 4) != 0) {
                        if (stp->type == 0) {
                            sal_setup_dspvol(&pb->mix.vDeltaAuxBL, &dsp_vptr->lastVolLb, dsp_vptr->volLb);
                            sal_setup_dspvol(&pb->mix.vDeltaAuxBR, &dsp_vptr->lastVolRb, dsp_vptr->volRb);
                            sal_setup_dspvol(&pb->mix.vDeltaAuxBS, &dsp_vptr->lastVolSb, dsp_vptr->volSb);

                            if ((pb->mix.vDeltaAuxBS | (pb->mix.vDeltaAuxBL | pb->mix.vDeltaAuxBR)) != 0) {
                                pb->mixerCtrl |= 2;
                                needsDelta = 1;
                            } else if ((pb->mix.vAuxBS | (pb->mix.vAuxBL | pb->mix.vAuxBR)) != 0) {
                                pb->mixerCtrl |= 2;
                            } else {
                                pb->mixerCtrl &= ~2;
                            }
                        } else {
                            sal_setup_dspvol(&pb->mix.vDeltaAuxBL, &dsp_vptr->lastVolLb, dsp_vptr->volLb);
                            sal_setup_dspvol(&pb->mix.vDeltaAuxBR, &dsp_vptr->lastVolRb, dsp_vptr->volRb);
                            if ((pb->mix.vDeltaAuxBL | pb->mix.vDeltaAuxBR) != 0) {
                                pb->mixerCtrl |= 0x10;
                                needsDelta = 1;
                            } else if ((pb->mix.vDeltaAuxAS |
                                        (pb->mix.vAuxAS | (pb->mix.vAuxBL | pb->mix.vAuxBR))) != 0) {
                                pb->mixerCtrl |= 0x10;
                            } else {
                                pb->mixerCtrl &= ~0x10;
                            }
                        }
                    } else if (stp->type == 0) {
                        if ((pb->mixerCtrl & 2) != 0) {
                            u32 localNeedsDelta;
                            localNeedsDelta = salCheckVolErrorAndResetDelta(
                                &pb->mix.vAuxBL, &pb->mix.vDeltaAuxBL, &dsp_vptr->lastVolLb, dsp_vptr->volLb,
                                rampResetOffsetFlags, 0x40);
                            localNeedsDelta |= salCheckVolErrorAndResetDelta(
                                &pb->mix.vAuxBR, &pb->mix.vDeltaAuxBR, &dsp_vptr->lastVolRb, dsp_vptr->volRb,
                                rampResetOffsetFlags, 0x80);
                            localNeedsDelta |= salCheckVolErrorAndResetDelta(
                                &pb->mix.vAuxBS, &pb->mix.vDeltaAuxBS, &dsp_vptr->lastVolSb, dsp_vptr->volSb,
                                rampResetOffsetFlags, 0x100);
                            if ((localNeedsDelta | (pb->mix.vAuxBS | (pb->mix.vAuxBL | pb->mix.vAuxBR))) == 0) {
                                pb->mixerCtrl &= ~2;
                            } else {
                                needsDelta = 1;
                            }
                        } else {
                            pb->mix.vDeltaAuxBL = 0;
                            pb->mix.vDeltaAuxBR = 0;
                            pb->mix.vDeltaAuxBS = 0;
                        }
                    } else if ((pb->mixerCtrl & 0x10) != 0) {
                        u32 localNeedsDelta;
                        localNeedsDelta = salCheckVolErrorAndResetDelta(
                            &pb->mix.vAuxBL, &pb->mix.vDeltaAuxBL, &dsp_vptr->lastVolLb, dsp_vptr->volLb,
                            rampResetOffsetFlags, 0x40);
                        localNeedsDelta |= salCheckVolErrorAndResetDelta(
                            &pb->mix.vAuxBR, &pb->mix.vDeltaAuxBR, &dsp_vptr->lastVolRb, dsp_vptr->volRb,
                            rampResetOffsetFlags, 0x80);
                        if ((localNeedsDelta | (pb->mix.vAuxBL | pb->mix.vAuxBR)) == 0) {
                            if ((pb->mix.vAuxAS | pb->mix.vDeltaAuxAS) == 0) {
                                pb->mixerCtrl &= ~0x10;
                            }
                        } else {
                            needsDelta = 1;
                        }
                    } else {
                        pb->mix.vDeltaAuxBL = 0;
                        pb->mix.vDeltaAuxBR = 0;
                        if ((pb->mix.vAuxAS | pb->mix.vDeltaAuxAS) != 0) {
                            pb->mixerCtrl |= 0x10;
                        }
                    }
                    if (needsDelta != 0) {
                        pb->mixerCtrl |= 8;
                    } else {
                        pb->mixerCtrl &= ~8;
                    }
                    if (stp->type == 0 /* SND_STUDIO_TYPE_STD */) {
                        if ((pb->mix.vS != 0) || (pb->mix.vDeltaS != 0) || (pb->mix.vAuxAS != 0) ||
                            (pb->mix.vDeltaAuxAS != 0) || (pb->mix.vAuxBS != 0) || (pb->mix.vDeltaAuxBS != 0)) {
                            pb->mixerCtrl |= 4;
                        } else {
                            pb->mixerCtrl &= ~4;
                        }
                    }
                    if ((dsp_vptr->changed[0] & 0x200) != 0) {
                        pb->itd.targetShiftL = dsp_vptr->itdShiftL;
                        pb->itd.targetShiftR = dsp_vptr->itdShiftR;
                    }
                    if ((dsp_vptr->changed[0] & 0x100) != 0) {
                        pb->srcSelect = dsp_vptr->srcTypeSelect;
                    }
                    if ((dsp_vptr->changed[0] & 0x80) != 0) {
                        pb->coefSelect = dsp_vptr->srcCoefSelect;
                    }
                    mix_start = 0;
                    newVoice = 0;
                    dsp_vptr->currentAddr = (pb->addr.currentAddressHi << 0x10) | pb->addr.currentAddressLo;
                block_186:
                    if ((dsp_vptr->changed[mix_start] & 0x40) != 0) {
                        adsrRelease(&dsp_vptr->adsr);
                    }
                    if ((dsp_vptr->changed[mix_start] & 8) != 0) {
                        pb->src.ratioHi = dsp_vptr->pitch[mix_start] >> 0x10;
                        pb->src.ratioLo = dsp_vptr->pitch[mix_start];
                        dsp_vptr->playInfo.pitch = dsp_vptr->pitch[mix_start];
                    }
                    VoiceDone = adsrHandle(&dsp_vptr->adsr, &pb->ve.currentVolume, &pb->ve.currentDelta);
                    old_adsr_delta = pb->ve.currentDelta;
                    for (s = 0; s < 5; s++) {
                        pb->update.updNum[s] = 0;
                    }
                    pptr = dsp_vptr->patchData;
                    pend = (u16*)((u32)dsp_vptr->patchData + 0x80);
                    if (mix_start != 0) {
                        pptr[0] = 7;
                        pptr[1] = 1;
                        pptr += 2;
                        pb->update.updNum[mix_start]++;
                    }
                    sal_update_hostplayinfo(dsp_vptr);
                    for (s = mix_start + 1; s < 5; s++) {
                        if (VoiceDone != 0) {
                            pptr[0] = 7;
                            pptr[1] = 0;
                            pptr += 2;
                            pb->update.updNum[s]++;
                            salSynthSendMessage(dsp_vptr, 0);
                            {
                                extern void salDeactivateVoice(void* dsp_vptr);
                                salDeactivateVoice(dsp_vptr);
                            }
                            break;
                        } else {
                            if (rampResetOffsetFlags[s] != 0) {
                                for (i = 0; i < 9; i++) {
                                    if (((1 << i) & rampResetOffsetFlags[s]) != 0) {
                                        pptr[0] = pbOffsets[i];
                                        pptr[1] = 0;
                                        pptr += 2;
                                        pb->update.updNum[s]++;
                                    }
                                }
                            }
                            if ((dsp_vptr->changed[s] & 0x20) != 0) {
                                adsrStartRelease(&dsp_vptr->adsr, 10);
                                dsp_vptr->postBreak = 1;
                            } else if (dsp_vptr->postBreak == 0) {
                                if ((dsp_vptr->changed[s] & 0x40) != 0) {
                                    adsrRelease(&dsp_vptr->adsr);
                                }
                                if ((dsp_vptr->changed[s] & 8) != 0) {
                                    pptr[0] = 0x53;
                                    pptr[1] = dsp_vptr->pitch[s] >> 16;
                                    pptr[2] = 0x54;
                                    pptr[3] = dsp_vptr->pitch[s];
                                    pptr += 4;
                                    pb->update.updNum[s] += 2;
                                    dsp_vptr->playInfo.pitch = dsp_vptr->pitch[s];
                                }
                            }
                            current_delta = dsp_vptr->adsr.currentDelta;
                            VoiceDone = adsrHandle(&dsp_vptr->adsr, &adsr_start, &adsr_delta);
                            if (old_adsr_delta == adsr_delta) {
                                if (current_delta != 0) {
                                    pptr[0] = 0x32;
                                    pptr[1] = adsr_start;
                                    pptr += 2;
                                    pb->update.updNum[s]++;
                                }
                            } else {
                                pptr[0] = 0x32;
                                pptr[1] = adsr_start;
                                pptr[2] = 0x33;
                                pptr[3] = adsr_delta;
                                pptr += 4;
                                pb->update.updNum[s] += 2;
                                old_adsr_delta = adsr_delta;
                            }
                            sal_update_hostplayinfo(dsp_vptr);
                        }
                    }
                    if (VoiceDone != 0) {
                        salSynthSendMessage(dsp_vptr, 0);
                        {
                            extern void salDeactivateVoice(void* dsp_vptr);
                            salDeactivateVoice(dsp_vptr);
                        }
                    }
                    DCStoreRangeNoSync(dsp_vptr->patchData, (u32)pptr - (u32)dsp_vptr->patchData);
                    cyclesUsed += dspMixerCycles[pb->mixerCtrl] + 0x4FE;
                    switch (pb->src.ratioHi) {
                    case 0:
                    case 1:
                        cyclesUsed += dspSRCCycles[pb->src.ratioHi][pb->srcSelect];
                        break;
                    default:
                        cyclesUsed += dspSRCCycles[2][pb->srcSelect];
                        break;
                    }
                    for (s = 0; s < 5; s++) {
                        cyclesUsed += pb->update.updNum[s] * 4;
                    }
                    if (cyclesUsed > (*(volatile u32*)0x800000F8 / 400)) {
                        extern void salDeactivateVoice(void* dsp_vptr);
                        if ((newVoice == 0) && (VoiceDone == 0)) {
                            fn_8015AD1C(stp, dsp_vptr);
                        }
                        salDeactivateVoice(dsp_vptr);
                        salSynthSendMessage(dsp_vptr, 1);
                        for (v = v - 1; v > 0; v--) {
                            if (voices[v - 1]->state == 2) {
                                fn_8015AD1C(stp, voices[v - 1]);
                            }
                            salDeactivateVoice(voices[v - 1]);
                            salSynthSendMessage(voices[v - 1], 1);
                        }
                        for (st1 = st + 1; st1 < salMaxStudioNum; st1++) {
                            if (dspStudio[st1].state == 1) {
                                for (dsp_vptr = dspStudio[st1].voiceRoot; dsp_vptr; dsp_vptr = next_dsp_vptr) {
                                    next_dsp_vptr = dsp_vptr->next;
                                    if (dsp_vptr->state == 2) {
                                        fn_8015AD1C(&dspStudio[st1], dsp_vptr);
                                    }
                                    salDeactivateVoice(dsp_vptr);
                                    salSynthSendMessage(dsp_vptr, 1);
                                }
                            }
                        }
                        break;
                    } else {
                        if (!last_pb) {
                            DSP_CMD_ENSURE(3)

                            dspCmdPtr[0] = 2; /* PB_ADDR */
                            dspCmdPtr[1] = (u32)pb >> 0x10;
                            dspCmdPtr[2] = (u32)pb;
                            dspCmdPtr += 3;
                            procVoiceFlag = 1;
                        } else {
                            last_pb->nextHi = (u32)pb >> 16;
                            last_pb->nextLo = (u32)pb;
                            procVoiceFlag = 1;
                            DCFlushRangeNoSync(last_pb, sizeof(_PB));
                        }
                        last_pb = pb;
                    }
                }
            }
            if (procVoiceFlag != 0) {
                DSP_CMD_ENSURE(1)

                *dspCmdPtr++ = 3; /* PROCESS */
            }
            if (last_pb) {
                last_pb->nextHi = 0;
                last_pb->nextLo = 0;
                DCFlushRangeNoSync(last_pb, sizeof(_PB));
            }
            getAuxFrame = (salAuxFrame + 1) % 3;
            if (stp->auxAHandler) {
                DSP_CMD_ENSURE(5)

                dspCmdPtr[0] = 4; /* MIX_AUXA */
                dspCmdPtr[1] = (u32)stp->auxA[salAuxFrame] >> 16;
                dspCmdPtr[2] = (u32)stp->auxA[salAuxFrame];
                dspCmdPtr[3] = (u32)stp->auxA[getAuxFrame] >> 16;
                dspCmdPtr[4] = (u32)stp->auxA[getAuxFrame];
                dspCmdPtr += 5;
            }
            if (stp->type == 0) {
                if (stp->auxBHandler) {
                    DSP_CMD_ENSURE(5)

                    dspCmdPtr[0] = 5; /* MIX_AUXB */
                    dspCmdPtr[1] = (u32)stp->auxB[salAuxFrame] >> 16;
                    dspCmdPtr[2] = (u32)stp->auxB[salAuxFrame];
                    dspCmdPtr[3] = (u32)stp->auxB[getAuxFrame] >> 16;
                    dspCmdPtr[4] = (u32)stp->auxB[getAuxFrame];
                    dspCmdPtr += 5;
                }
            } else {
                DSP_CMD_ENSURE(5)

                dspCmdPtr[0] = 16; /* MIX_AUXB_LR */
                dspCmdPtr[1] = (u32)stp->auxB[salFrame] >> 16;
                dspCmdPtr[2] = (u32)stp->auxB[salFrame];
                dspCmdPtr[3] = (u32)stp->auxB[salFrame ^ 1] >> 16;
                dspCmdPtr[4] = (u32)stp->auxB[salFrame ^ 1];
                dspCmdPtr += 5;
            }
            DSP_CMD_ENSURE(3)

            dspCmdPtr[0] = 6; /* UPLOAD_LRS */
            dspCmdPtr[1] = (u32)stp->main[salFrame] >> 16;
            dspCmdPtr[2] = (u32)stp->main[salFrame];
            dspCmdPtr += 3;
            spb = stp->spb;
            DoDepopFade((s32*)&spb->dpopLHi, (s16*)&spb->dpopLDelta, &stp->hostDPopSum.l);
            DoDepopFade((s32*)&spb->dpopRHi, (s16*)&spb->dpopRDelta, &stp->hostDPopSum.r);
            DoDepopFade((s32*)&spb->dpopSHi, (s16*)&spb->dpopSDelta, &stp->hostDPopSum.s);
            DoDepopFade((s32*)&spb->dpopALHi, (s16*)&spb->dpopALDelta, &stp->hostDPopSum.lA);
            DoDepopFade((s32*)&spb->dpopARHi, (s16*)&spb->dpopARDelta, &stp->hostDPopSum.rA);
            DoDepopFade((s32*)&spb->dpopASHi, (s16*)&spb->dpopASDelta, &stp->hostDPopSum.sA);
            DoDepopFade((s32*)&spb->dpopBLHi, (s16*)&spb->dpopBLDelta, &stp->hostDPopSum.lB);
            DoDepopFade((s32*)&spb->dpopBRHi, (s16*)&spb->dpopBRDelta, &stp->hostDPopSum.rB);
            DoDepopFade((s32*)&spb->dpopBSHi, (s16*)&spb->dpopBSDelta, &stp->hostDPopSum.sB);
            DCFlushRangeNoSync(spb, sizeof(_SPB));
        }
    }
    DSP_CMD_ENSURE(3)

    dspCmdPtr[0] = 17; /* SET_OPPOSITE_LR */
    dspCmdPtr[1] = (u32)dspSurround >> 16;
    dspCmdPtr[2] = (u32)dspSurround;
    dspCmdPtr += 3;
    for (st = 0; st < salMaxStudioNum; st++) {
        if ((dspStudio[st].state == 1) && (dspStudio[st].isMaster != 0)) {
            DSP_CMD_ENSURE(3)

            dspCmdPtr[0] = 9; /* MIX_AUXB_NOWRITE */
            dspCmdPtr[1] = (u32)dspStudio[st].main[salFrame] >> 16;
            dspCmdPtr[2] = (u32)dspStudio[st].main[salFrame];
            dspCmdPtr += 3;
        }
    }
    DSP_CMD_ENSURE(5)

    {
        u16 size;
        dspCmdPtr[0] = 14; /* OUTPUT */
        dspCmdPtr[1] = (u32)dspSurround >> 16;
        dspCmdPtr[2] = (u32)dspSurround;
        dspCmdPtr[3] = (u32)dest >> 16;
        dspCmdPtr[4] = (u32)dest;
        dspCmdPtr += 5;
        *dspCmdPtr++ = 15; /* END */
        size = (u16)(((u32)dspCmdPtr - (u32)dspCmdCurBase) + 3) & ~3;
        if (dspCmdLastLoad) {
            dspCmdLastLoad[3] = size;
            DCStoreRangeNoSync(dspCmdLastBase, dspCmdLastSize);
        } else {
            dspCmdFirstSize = size;
        }
    }
    DCStoreRangeNoSync(dspCmdCurBase, (u32)dspCmdPtr - (u32)dspCmdCurBase);
}
#pragma pop

#undef DSP_CMD_ENSURE
#undef dspARAMZeroBuffer
#undef dspCmdLastLoad
#undef dspCmdLastBase
#undef dspCmdLastSize
#undef dspCmdCurBase
#undef dspCmdMaxPtr
#undef dspCmdPtr
#undef dspCmdFirstSize
#undef dspCmdList
#undef dspHRTFOn
#undef dspSurround
#undef salMaxStudioNum
#undef salAuxFrame
#undef salFrame
#undef voices
#undef dspMixerCycles
#undef dspSRCCycles
#undef dspStudio

void fn_80159C48(void) {
    extern s16 lbl_8047AFE8;
    lbl_8047AFE8 = 0;
}

static inline MusyxPoolEntry* MusyxPoolEntryAt(u32 address)
{
    return (MusyxPoolEntry*)address;
}

static inline MusyxPoolEntry* fn_80159C54_get_entry(u16 id,
                                                    MusyxPoolEntry* entry)
{
    while (entry->nextOffset != 0xFFFFFFFF) {
        if (entry->id == id) {
            return entry;
        }
        entry = MusyxPoolEntryAt((u32)entry + entry->nextOffset);
    }
    return NULL;
}

static inline MusyxPoolEntry* fn_80159C54_get_macro(u16 id,
                                                    MusyxPoolData* pool)
{
    return pool == NULL ? NULL :
        fn_80159C54_get_entry(id,
            MusyxPoolEntryAt((u32)pool + pool->macroOffset));
}

static inline MusyxPoolEntry* fn_80159C54_get_curve(u16 id,
                                                    MusyxPoolData* pool)
{
    return pool == NULL ? NULL :
        fn_80159C54_get_entry(id,
            MusyxPoolEntryAt((u32)pool + pool->curveOffset));
}

static inline MusyxPoolEntry* fn_80159C54_get_keymap(u16 id,
                                                     MusyxPoolData* pool)
{
    return pool == NULL ? NULL :
        fn_80159C54_get_entry(id,
            MusyxPoolEntryAt((u32)pool + pool->keymapOffset));
}

static inline MusyxPoolEntry* fn_80159C54_get_layer(u16 id,
                                                    MusyxPoolData* pool)
{
    return pool == NULL ? NULL :
        fn_80159C54_get_entry(id,
            MusyxPoolEntryAt((u32)pool + pool->layerOffset));
}

void fn_80159C54(u16 id, MusyxPoolData* pool, u8 dataType, u32 remove)
{
    MusyxPoolEntry* entry;

    switch (dataType) {
    case 0:
        if (!remove) {
            if ((entry = fn_80159C54_get_macro(id, pool)) != NULL) {
                dataInsertMacro(id, entry->data.macros);
            } else {
                dataInsertMacro(id, NULL);
            }
        } else {
            dataRemoveMacro(id);
        }
        break;
    case 2:
        id |= 0x4000;
        if (!remove) {
            if ((entry = fn_80159C54_get_keymap(id, pool)) != NULL) {
                dataInsertKeymap(id, entry->data.keymaps);
            } else {
                dataInsertKeymap(id, NULL);
            }
        } else {
            dataRemoveKeymap(id);
        }
        break;
    case 3:
        id |= 0x8000;
        if (!remove) {
            if ((entry = fn_80159C54_get_layer(id, pool)) != NULL) {
                dataInsertLayer(id, entry->data.layer.entries,
                                entry->data.layer.count);
            } else {
                dataInsertLayer(id, NULL, 0);
            }
        } else {
            dataRemoveLayer(id);
        }
        break;
    case 4:
        if (!remove) {
            if ((entry = fn_80159C54_get_curve(id, pool)) != NULL) {
                dataInsertCurve(id, entry->data.curve);
            } else {
                dataInsertCurve(id, NULL);
            }
        } else {
            dataRemoveCurve(id);
        }
        break;
    case 1:
        if (!remove) {
            dataAddSampleReference(id);
        } else {
            dataRemoveSampleReference(id);
        }
        break;
    }
}

/* fn_80159ED0: pass-through wrapper -- params stay in r3/r4, retail emits a
 * plain `bl fn_801630E4` (its only caller). dont_inline keeps the call real
 * (fn_801630E4 is defined earlier in this TU, so -inline auto would
 * otherwise fold it to `bl aramSetUploadCallback`). */
#pragma dont_inline on
void fn_80159ED0(u8* ptr, u32 size) {
    fn_801630E4(ptr, size);
}
#pragma dont_inline reset



u8 fn_8015E890(void* emitter) {
    extern u8 lbl_8047AF18;
    if (lbl_8047AF18 != 0) {
        return (((u32*)emitter)[4] >> 16) & 1;
    }
    return 0;
}

extern MusyxEmitter lbl_80448540;
extern u8 synthFXGetMaxVoices(u16 effect);
extern f32 lbl_8047D4A4;

u32 fn_8015E8B0(MusyxEmitter* emitterBuffer, const MusyxVec3* position,
                 const MusyxVec3* velocity, f32 maxDistance, f32 curve,
                 u32 flags, u16 effectId, u32 group, u8 maxVolume,
                 u8 minVolume, MusyxStudioEmitter* studioEmitter,
                 SndServiceCtrlList* controls, u8 studio)
{
    MusyxEmitter* emitter;
    f32 volume;
    f32 pitch;
    f32 pan;
    f32 unusedPan;
    f32 surround;

    hwDisableIrq();
    emitter = emitterBuffer == NULL ? &lbl_80448540 : emitterBuffer;
    emitter->flags = flags;
    emitter->pos = *position;
    emitter->velocity = *velocity;
    emitter->maxDistance = maxDistance;
    emitter->fxId = effectId;
    emitter->maxVolume = (f32)maxVolume * lbl_8047D4A4;
    emitter->minVolume = (f32)minVolume * lbl_8047D4A4;
    emitter->curve = curve;
    emitter->user = (void*)group;
    emitter->listener = (MusyxEmitterListener*)studioEmitter;
    emitter->studio = studio;

    if (emitterBuffer == NULL) {
        u32 converted;
        u8 controlValue;
        u16 control14;

        if (studioEmitter != NULL && studioEmitter->studio == 0xFF) {
            hwEnableIrq();
            return -1;
        }
        fn_8015E374(emitter, &volume, &pitch, &pan, &unusedPan, &surround);
        if (volume == lbl_8047D468) {
            hwEnableIrq();
            return -1;
        }
        emitter->voice =
            synthFXStart(emitter->fxId, 127, 64,
                         studioEmitter != NULL ? studioEmitter->studio
                                               : emitter->studio,
                         (emitter->flags & 0x10) != 0);
        if (emitter->voice == 0xFFFFFFFF) {
            hwEnableIrq();
            return -1;
        }

        if ((emitter->flags & 0x100000) != 0) {
            converted =
                (u32)(s32)(lbl_8047D488 * (emitter->field_4C * volume));
        } else {
            converted = (u32)(s32)(lbl_8047D488 * volume);
        }
        controlValue = 0x7F;
        if ((u8)converted <= 0x7F) {
            controlValue = converted;
        }
        synthFXSetCtrl(emitter->voice, 7, controlValue);

        converted = (u32)(s32)(lbl_8047D49C * (lbl_8047D48C + pan));
        controlValue = 0x7F;
        if ((u8)converted <= 0x7F) {
            controlValue = converted;
        }
        synthFXSetCtrl(emitter->voice, 0xA, controlValue);

        converted =
            (u32)(s32)(lbl_8047D49C * (lbl_8047D48C - surround));
        controlValue = 0x7F;
        if ((u8)converted <= 0x7F) {
            controlValue = converted;
        }
        synthFXSetCtrl(emitter->voice, 0x83, controlValue);

        converted = __cvt_fp2unsigned(lbl_8047D4A0 * pitch);
        control14 = 0x3FFF;
        if (converted <= 0x3FFF) {
            control14 = converted;
        }
        synthFXSetCtrl14(emitter->voice, 0x84, control14);

        if (controls != NULL) {
            u32 i;
            SndServiceCtrl* control;

            control = controls->controls;
            for (i = 0; i < controls->count; i++, control++) {
                if (control->ctrl < 0x40 || control->ctrl == 0x80 ||
                    control->ctrl == 0x84)
                {
                    synthFXSetCtrl14(emitter->voice, control->ctrl,
                                     control->value);
                } else {
                    synthFXSetCtrl(emitter->voice, control->ctrl,
                                   (u8)control->value);
                }
            }
        }
        hwEnableIrq();
        return emitter->voice;
    }

    emitter->next = (MusyxEmitter*)lbl_8047B048;
    if (lbl_8047B048 != NULL) {
        lbl_8047B048->prev = (MusyxVoiceLink*)emitter;
    }
    emitter->prev = NULL;
    lbl_8047B048 = (MusyxVoiceLink*)emitter;
    emitter->ctrlList = controls;
    emitter->voice = -1;
    emitter->field_48 = 0;
    emitter->flags |= 0x30000;
    emitter->maxVoices = synthFXGetMaxVoices(emitter->fxId);
    hwEnableIrq();
    return -1;
}

u32 sndAddEmitter(void* emitter, const f32* position, const f32* velocity,
                  u32 maxVoices, u32 soundId, u32 volume, u32 panning,
                  u32 studio, f32 minDistance, f32 maxDistance) {
    extern u32 fn_8015E8B0(void*, const f32*, const f32*, u32, u32, u32,
                           u32, u32, u32, u32, u32, f32, f32);
    extern u8 lbl_8047AF18;

    if (lbl_8047AF18 != 0) {
        return fn_8015E8B0(emitter, position, velocity, maxVoices, soundId,
                           (soundId & 0xffff) | 0x80000000, volume, panning,
                           studio, 0, 0, minDistance, maxDistance);
    }
    return -1;
}

void fn_8015FE4C(u32 arg) {
    extern u8 lbl_8047B033;
    extern u8 lbl_8047B034;
    extern u8 lbl_8047B035;
    extern u32 lbl_8047B038;
    extern SndServiceEmitterPair* lbl_8047B03C;
    extern MusyxStudioEmitter* lbl_8047B040;
    extern MusyxEmitterListener* lbl_8047B044;
    extern MusyxVoiceLink* lbl_8047B048;
    extern u8 lbl_8047B04C;
    lbl_8047B048 = 0;
    lbl_8047B044 = 0;
    lbl_8047B040 = 0;
    lbl_8047B03C = 0;
    lbl_8047B038 = 0;
    lbl_8047B035 = 1;
    lbl_8047B034 = 3;
    lbl_8047B04C = 0;
    lbl_8047B033 = (u8)((arg >> 1) & 1);
}

void fn_8015FE84(void) { }

extern void seqInit(void);
extern void synthInit(u32 mixFrq, u8 numVoices);

s32 fn_8015FE88(u8 voicesArg, u8 music, u8 sfx, u8 studios, u32 flags,
                u32 aramSize) {
    extern u32 hwInit(u32 mixFrq, u16 numVoices, u32 numStudios, u32 flags);
    extern void fn_80159C48(void);
    extern void vsInit(void);
    extern void fn_8015FE4C(u32 flags);
    s32 ret;
    u32 mixFrq;

    ret = 0;
    lbl_8047AF18 = 0;
    if (voicesArg <= 64) {
        synthInfo.voiceNum = voicesArg;
    } else {
        synthInfo.voiceNum = 64;
    }
    if (studios <= 8) {
        synthInfo.studioNum = studios;
    } else {
        synthInfo.studioNum = 8;
    }

    synthInfo.maxMusic = music;
    synthInfo.maxSFX = sfx;
    mixFrq = 32000;
    ret = hwInit((u32)&mixFrq, synthInfo.voiceNum, synthInfo.studioNum, flags);
    if (ret == 0) {
        fn_80159C48();
        dataInit(0, aramSize);
        seqInit();
        synthIdleWaitActive = 0;
        synthInit(32000, synthInfo.voiceNum);
        vsInit();
        fn_8015FE4C(flags);
        lbl_8047AF18 = 1;
    }

    return ret;
}

#pragma dont_inline on
void sndQuit(void) {
    hwExit();
    dataExit();
    fn_8015FE84();
    synthExit();
    lbl_8047AF18 = 0;
}
#pragma dont_inline reset

u8 fn_8015FFD4(void) {
    extern u8 lbl_8047AF18;
    return lbl_8047AF18;
}

#pragma fp_contract off
void salApplyMatrix(const f32* matrix, const f32* src, f32* dst) {
    dst[0] = matrix[0] * src[0] + matrix[1] * src[1] + matrix[2] * src[2] + matrix[9];
    dst[1] = matrix[3] * src[0] + matrix[4] * src[1] + matrix[5] * src[2] + matrix[10];
    dst[2] = matrix[6] * src[0] + matrix[7] * src[1] + matrix[8] * src[2] + matrix[11];
}

static inline f32 salSqrtPositive(f32 x) {
    extern f32 lbl_8047D4B8;
    extern f64 lbl_8047D4C0;
    extern f64 lbl_8047D4C8;
    if (x > lbl_8047D4B8) {
        f64 estimate = __frsqrte(x);
        volatile f32 rounded;
        estimate = lbl_8047D4C0 * estimate *
                   (lbl_8047D4C8 - x * (estimate * estimate));
        estimate = lbl_8047D4C0 * estimate *
                   (lbl_8047D4C8 - x * (estimate * estimate));
        estimate = lbl_8047D4C0 * estimate *
                   (lbl_8047D4C8 - x * (estimate * estimate));
        rounded = (f32)(x * estimate);
        x = rounded;
    }
    return x;
}

f32 salNormalizeVector(f32* v) {
    f32 squared = salSqrtPositive(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v[0] /= squared;
    v[1] /= squared;
    v[2] /= squared;
    return squared;
}

void salCrossProduct(f32* dst, const f32* a, const f32* b) {
    dst[0] = a[1] * b[2] - a[2] * b[1];
    dst[1] = a[2] * b[0] - a[0] * b[2];
    dst[2] = a[0] * b[1] - a[1] * b[0];
}

void salInvertMatrix(SndFMatrix* out, const SndFMatrix* in) {
    f32 a;
    f32 b;
    f32 c;
    f32 f;

    a = in->m[1][1] * in->m[2][2] - in->m[2][1] * in->m[1][2];
    b = -(in->m[1][0] * in->m[2][2] - in->m[2][0] * in->m[1][2]);
    c = in->m[1][0] * in->m[2][1] - in->m[2][0] * in->m[1][1];
    f = lbl_8047D4D0[0] / (in->m[0][0] * a + in->m[0][1] * b + in->m[0][2] * c);
    out->m[0][0] = f * a;
    out->m[1][0] = f * b;
    out->m[2][0] = f * c;
    out->m[0][1] = -f * (in->m[0][1] * in->m[2][2] - in->m[2][1] * in->m[0][2]);
    out->m[1][1] = f * (in->m[0][0] * in->m[2][2] - in->m[2][0] * in->m[0][2]);
    out->m[2][1] = -f * (in->m[0][0] * in->m[2][1] - in->m[2][0] * in->m[0][1]);
    out->m[0][2] = f * (in->m[0][1] * in->m[1][2] - in->m[1][1] * in->m[0][2]);
    out->m[1][2] = -f * (in->m[0][0] * in->m[1][2] - in->m[1][0] * in->m[0][2]);
    out->m[2][2] = f * (in->m[0][0] * in->m[1][1] - in->m[1][0] * in->m[0][1]);
    out->t[0] = (-in->t[0] * out->m[0][0] - in->t[1] * out->m[0][1]) - in->t[2] * out->m[0][2];
    out->t[1] = (-in->t[0] * out->m[1][0] - in->t[1] * out->m[1][1]) - in->t[2] * out->m[1][2];
    out->t[2] = (-in->t[0] * out->m[2][0] - in->t[1] * out->m[2][1]) - in->t[2] * out->m[2][2];
}
#pragma fp_contract reset

/* ===== snd_midictrl.c: inp* cluster, 0x8015FFD4-0x801610F4 =====
 * identity: reference snd_midictrl.c. Statics identified via symbol-map
 * size-run correlation against XD's contiguous run (inpGlobalMIDIDirtyFlags
 * 0x200, midi_ctrl 0x4300, inpChannelDefaults 0x480, fx_ctrl 0x2180,
 * inpFXChannelDefaults 0x240, midi_lastNote 0x80, fx_lastNote 0x40) mapped
 * onto Colosseum's config/GC6E01/symbols.txt run (lbl_80449390 0x200,
 * lbl_80449590 0x4300, lbl_8044D890 0x80, lbl_8044D910 0x2180,
 * lbl_8044FA90 0x40, lbl_8044FAD0 0x80, lbl_8044FB50 0x40) -- note
 * inpChannelDefaults/inpFXChannelDefaults are 6x SMALLER than XD's
 * (0x80/0x40 vs 0x480/0x240) because Colosseum's pre-2.0.1
 * CHANNEL_DEFAULTS is just `u8 pbRange` (1 byte) -- the lpfLower/
 * UpperFrqBoundary fields are a >=2.0.1 addition (confirmed by
 * disassembly: fn_80160ED4/inpResetChannelDefaults only ever touches
 * offset 0x0, never the lpf fields the reference has under
 * MUSY_VERSION>=2.0.1). inpColdMIDIDefaults/inpWarmMIDIDefaults (both
 * 134-byte tables, padded to 0x88 in .rodata) = lbl_80273338/
 * lbl_802733C0 (data already 100% matched in this unit -- no new data
 * needed, just extern declarations by address). */
extern u32 lbl_80449390[];            /* inpGlobalMIDIDirtyFlags (flat; matches existing
                                        * extern in fn_801619E8 -- must not redeclare with
                                        * a conflicting sized type) */
extern u8  lbl_80449590[8][16][134];  /* midi_ctrl */
extern u8  lbl_8044D890[8][16];       /* inpChannelDefaults (pre-2.0.1: just pbRange) */
extern u8  lbl_8044D910[64][134];     /* fx_ctrl */
extern u8  lbl_8044FA90[64];          /* inpFXChannelDefaults (pre-2.0.1: just pbRange) */
extern u8  lbl_8044FAD0[8][16];       /* midi_lastNote */
extern u8  lbl_8044FB50[64];          /* fx_lastNote */
extern const u8 lbl_80273338[134];    /* inpColdMIDIDefaults */
extern const u8 lbl_802733C0[134];    /* inpWarmMIDIDefaults */
extern void inpSetMidiLastNote(u8 midi, u8 midiSet, u8 key);

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_8016039C(u8 chan, u8 midiSet, s32 flag) { /* inpSetGlobalMIDIDirtyFlag */
    /* cast to a true 2D array locally (rather than manually flattening the
     * index) so MWCC emits retail's separate row-stride/column-stride
     * multiplies instead of a single combined-index multiply. */
    ((u32(*)[16])lbl_80449390)[midiSet][chan] |= flag;
}
#pragma pop

extern void synthKeyStateUpdate(SynthVoiceMini* svoice);

static inline void inpSetRPNHi(u8 set, u8 channel, u8 value) {
    u32 i;
    u16 rpn;
    u8 range;

    rpn = lbl_80449590[set][channel][100] |
          (lbl_80449590[set][channel][101] << 8);
    switch (rpn) {
    case 0:
        range = value > 24 ? 24 : value;
        lbl_8044D890[set][channel] = range;
        for (i = 0; i < synthInfo.voiceNum; i++) {
            if (set == lbl_8047AF48[i].midiSet &&
                channel == lbl_8047AF48[i].midi) {
                lbl_8047AF48[i].pbUpperKeyRange = range;
                lbl_8047AF48[i].pbLowerKeyRange = range;
            }
        }
        break;
    default:
        break;
    }
}

static inline void inpSetRPNLo(u8 set, u8 channel, u8 value) {
}

static inline void inpSetRPNDec(u8 set, u8 channel) {
    u32 i;
    u16 rpn;
    u8 range;

    rpn = lbl_80449590[set][channel][100] |
          (lbl_80449590[set][channel][101] << 8);
    switch (rpn) {
    case 0:
        range = lbl_8044D890[set][channel];
        if (range != 0) {
            range--;
        }
        lbl_8044D890[set][channel] = range;
        for (i = 0; i < synthInfo.voiceNum; i++) {
            if (set == lbl_8047AF48[i].midiSet &&
                channel == lbl_8047AF48[i].midi) {
                lbl_8047AF48[i].pbUpperKeyRange = range;
                lbl_8047AF48[i].pbLowerKeyRange = range;
            }
        }
        break;
    default:
        break;
    }
}

static inline void inpSetRPNInc(u8 set, u8 channel) {
    u32 i;
    u16 rpn;
    u8 range;

    rpn = lbl_80449590[set][channel][100] |
          (lbl_80449590[set][channel][101] << 8);
    switch (rpn) {
    case 0:
        range = lbl_8044D890[set][channel];
        if (range < 24) {
            range++;
        }
        lbl_8044D890[set][channel] = range;
        for (i = 0; i < synthInfo.voiceNum; i++) {
            if (set == lbl_8047AF48[i].midiSet &&
                channel == lbl_8047AF48[i].midi) {
                lbl_8047AF48[i].pbUpperKeyRange = range;
                lbl_8047AF48[i].pbLowerKeyRange = range;
            }
        }
        break;
    default:
        break;
    }
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_801603C0(u8 ctrl, u8 channel, u8 set, u8 value) { /* inpSetMidiCtrl */
    u32 i;

    if (channel == 0xFF) {
        return;
    }

    if (set != 0xFF) {
        switch (ctrl) {
        case 6:
            inpSetRPNHi(set, channel, value);
            break;
        case 38:
            inpSetRPNLo(set, channel, value);
            break;
        case 96:
            inpSetRPNDec(set, channel);
            break;
        case 97:
            inpSetRPNInc(set, channel);
            break;
        }

        lbl_80449590[set][channel][ctrl] = value & 0x7F;
        for (i = 0; i < synthInfo.voiceNum; i++) {
            if (set == lbl_8047AF48[i].midiSet &&
                channel == lbl_8047AF48[i].midi) {
                lbl_8047AF48[i].midiDirtyFlags = 0x1FFF;
                synthKeyStateUpdate(&lbl_8047AF48[i]);
            }
        }
        ((u32(*)[16])lbl_80449390)[set][channel] = 0xFF;
    } else {
        switch (ctrl) {
        case 6:
            inpSetRPNHi(set, channel, value);
            break;
        case 38:
            inpSetRPNLo(set, channel, value);
            break;
        case 96:
            inpSetRPNDec(set, channel);
            break;
        case 97:
            inpSetRPNInc(set, channel);
            break;
        }

        lbl_8044D910[channel][ctrl] = value & 0x7F;
        for (i = 0; i < synthInfo.voiceNum; i++) {
            if (set == lbl_8047AF48[i].midiSet &&
                channel == lbl_8047AF48[i].midi) {
                lbl_8047AF48[i].midiDirtyFlags = 0x1FFF;
                synthKeyStateUpdate(&lbl_8047AF48[i]);
            }
        }
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void inpResetMidiCtrl(u8 ch, u8 set, u32 coldReset) {
    const u8* values;
    u8* dest;
    u32 i;

    values = (coldReset ? lbl_80273338 : lbl_802733C0);
    dest = set != 0xFF ? lbl_80449590[set][ch] : lbl_8044D910[ch];

    if (coldReset) {
        memcpy(dest, values, 134);
    } else {
        for (i = 0; i < 134; i++) {
            if (values[i] != 0xFF) {
                dest[i] = values[i];
            }
        }
    }

    inpSetMidiLastNote(ch, set, 0xFF);
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u16 inpGetMidiCtrl(u8 ctrl, u8 channel, u8 set) {

    if (channel != 0xff) {
        if (set != 0xff) {

            if (ctrl < 0x40) {
                return lbl_80449590[set][channel][ctrl & 0x1f] << 7 |
                       lbl_80449590[set][channel][(ctrl & 0x1f) + 0x20];
            }
            if (ctrl < 0x46) {
                return lbl_80449590[set][channel][ctrl] < 0x40 ? 0 : 0x3fff;
            }
            if (ctrl >= 0x60 && ctrl < 0x66) {
                return 0;
            }

            if ((ctrl == 0x80) || (ctrl == 0x81)) {
                return lbl_80449590[set][channel][ctrl & 0xfe] << 7 |
                       lbl_80449590[set][channel][(ctrl & 0xfe) + 1];
            }
            if ((ctrl == 0x84) || (ctrl == 0x85)) {
                return lbl_80449590[set][channel][ctrl & 0xfe] << 7 |
                       lbl_80449590[set][channel][(ctrl & 0xfe) + 1];
            }

            return lbl_80449590[set][channel][ctrl] << 7;
        }
        if (ctrl < 0x40) {
            return lbl_8044D910[channel][ctrl & 0x1f] << 7 | lbl_8044D910[channel][(ctrl & 0x1f) + 0x20];
        }
        if (ctrl < 0x46) {
            return lbl_8044D910[channel][ctrl] < 0x40 ? 0 : 0x3fff;
        }
        if (ctrl >= 0x60 && ctrl < 0x66) {
            return 0;
        }
        if ((ctrl == 0x80) || (ctrl == 0x81)) {
            return lbl_8044D910[channel][ctrl & 0xfe] << 7 | lbl_8044D910[channel][(ctrl & 0xfe) + 1];
        }
        if ((ctrl == 0x84) || (ctrl == 0x85)) {
            return lbl_8044D910[channel][ctrl & 0xfe] << 7 | lbl_8044D910[channel][(ctrl & 0xfe) + 1];
        }
        return lbl_8044D910[channel][ctrl] << 7;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u8* fn_80160EA0(u8 midi, u8 midiSet) { /* inpGetChannelDefaults */
    if (midiSet == 0xFF) {
        return &lbl_8044FA90[midi];
    }

    return &lbl_8044D890[midiSet][midi];
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80160ED4(u8 midi, u8 midiSet) { /* inpResetChannelDefaults */
    u8* channelDefaults;
    channelDefaults = midiSet != 0xFF ? &lbl_8044D890[midiSet][midi] : &lbl_8044FA90[midi];
    *channelDefaults = 2;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void inpAddCtrl(CtrlDest* dest, u8 ctrl, s32 scale, u8 comb, u32 isVar) {
    u8 n;
    if (comb == 0) {
        dest->numSource = 0;
    }

    if (dest->numSource < 4) {
        n = dest->numSource++;
        if (isVar == 0) {
            ctrl = inpTranslateExCtrl(ctrl);
        } else {
            comb |= 0x10;
        }

        dest->source[n].midiCtrl = ctrl;
        dest->source[n].combine = comb;
        dest->source[n].scale = scale;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void inpFXCopyCtrl(u8 ctrl, u8* dvoice, u8* svoice) {
    u8 di;
    u8 si;
    di = *(u32*)(dvoice + 0xF4); /* id field is u32; narrows via lwz+clrlwi, matching retail */
    si = *(u32*)(svoice + 0xF4);

    if (ctrl < 64) {
        lbl_8044D910[di][ctrl & 31] = lbl_8044D910[si][ctrl & 31];
        lbl_8044D910[di][(ctrl & 31) + 32] = lbl_8044D910[si][(ctrl & 31) + 32];
    } else if (ctrl == 128 || ctrl == 129) {
        lbl_8044D910[di][ctrl & 254] = lbl_8044D910[si][ctrl & 254];
        lbl_8044D910[di][(ctrl & 254) + 1] = lbl_8044D910[si][(ctrl & 254) + 1];
    } else if (ctrl == 132 || ctrl == 133) {
        lbl_8044D910[di][ctrl & 254] = lbl_8044D910[si][ctrl & 254];
        lbl_8044D910[di][(ctrl & 254) + 1] = lbl_8044D910[si][(ctrl & 254) + 1];
    } else {
        lbl_8044D910[di][ctrl] = lbl_8044D910[si][ctrl];
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void inpSetMidiLastNote(u8 midi, u8 midiSet, u8 key) {
    if (midiSet != 0xFF) {
        lbl_8044FAD0[midiSet][midi] = key;
    } else {
        lbl_8044FB50[midi] = key;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u8 inpGetMidiLastNote(u8 midi, u8 midiSet) {
    if (midiSet != 0xFF) {
        return lbl_8044FAD0[midiSet][midi];
    }
    return lbl_8044FB50[midi];
}
#pragma pop

/* ===== snd_midictrl.c: _GetInputValue, 0x80161134 =====
 * identity: reference snd_midictrl.c static _GetInputValue. svoice field
 * offsets confirmed by disassembly: macStartTime@0x90 (u64, matches
 * reference's synth.h offset), timeUsedByInput@0xA8, orgNote@0x12F,
 * orgVolume@0x158 (u32), lfo[].value@0x1C4/0x1D0 (s16, stride 0xC,
 * matches fn_80161D90/inpGetExCtrl's already-known 0x1C4/0x1D0 offsets),
 * lfoUsedByInput[]@0x1D4/0x1D5. synthRealTime = lbl_8047AF58 (u64, hi/lo
 * halves at consecutive .sbss addresses 0x8047AF58/0x8047AF5C).
 * varGet = synthmacros.c's varGet (already matched there, real signature
 * s16 varGet(SYNTH_VOICE*, u32, u8) -- cross-TU real `bl`, not inlined).
 * CtrlDest (CTRL_DEST) reused from inpAddCtrl's definition above:
 * source[i] entries are 8 bytes (midiCtrl@0, combine@1, scale@4),
 * numSource@0x22 -- confirmed again here via the `addi r31,r31,8` pointer
 * walk and `lbz r0,0x22(r25)` loop bound. */
extern s16 varGet(u8* svoice, u32 ctrl, u8 index);
extern u32 lbl_8047AF58; /* synthRealTime, high word */
extern u32 lbl_8047AF5C; /* synthRealTime, low word */

#define GIV_CLAMP(value, min, max) ((value) > (max) ? (max) : (value) < (min) ? (min) : (value))
#define GIV_CLAMP_INV(value, min, max) ((value) < (min) ? (min) : (value) > (max) ? (max) : (value))
#define GIV_MIN(a, b) ((a) > (b) ? (b) : (a))

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 _GetInputValue(u8* svoice, u8* motionBase, u8 midi, u8 midiSet) {
    CtrlDest* inp = (CtrlDest*)motionBase;
    u32 i;
    u32 value;
    u8 ctrl;
    s32 tmp;
    s32 vtmp;
    u32 sign;

    for (value = 0, i = 0; i < ((CtrlDest*)motionBase)->numSource; i++) {
        if (inp->source[i].combine & 0x10) {
            tmp = svoice != NULL ? varGet(svoice, 0, inp->source[i].midiCtrl) : 0;
            goto block_18;
        }
        ctrl = inp->source[i].midiCtrl;
        if (ctrl == 128 || ctrl == 1 || ctrl == 10 || ctrl == 160 || ctrl == 161 || ctrl == 131) {
            switch (ctrl) {
            case 160:
            case 161:
                if (svoice != NULL) {
                    tmp = (*(s16*)(svoice - 0x5bc + ctrl * 12)) << 1;
                    svoice[ctrl + 0x134] = 1;
                } else {
                    tmp = 0;
                }
                break;
            default:
                tmp = inpGetMidiCtrl(ctrl, midi, midiSet) - 0x2000;
                break;
            }
        block_18:
            tmp = (tmp * (inp->source[i].scale >> 1)) >> 15;
            tmp = GIV_CLAMP_INV(tmp, -0x2000, 0x1FFF);
            switch (inp->source[i].combine & 15) {
            case 0:
                value = tmp + 0x2000;
                sign = 1;
                break;
            case 1:
                if (sign != 0) {
                    vtmp = (value + tmp);
                    vtmp -= 0x2000;
                    value = GIV_CLAMP_INV(vtmp, -0x2000, 0x1FFF) + 0x2000;
                } else {
                    vtmp = value + tmp;
                    value = GIV_CLAMP(vtmp, 0, 0x3FFF);
                }
                break;
            case 2:
                if (sign != 0) {
                    vtmp = (s32)((value - 0x2000) * tmp) >> 13;
                } else {
                    vtmp = (tmp * value) >> 13;
                    sign = 1;
                }
                value = GIV_CLAMP_INV(vtmp, -0x2000, 0x1FFF) + 0x2000;
                break;
            case 3:
                if (sign != 0) {
                    vtmp = (value - 0x2000) - tmp;
                    value = GIV_CLAMP_INV(vtmp, -0x2000, 0x1FFF) + 0x2000;
                } else {
                    vtmp = value - tmp;
                    value = GIV_CLAMP(vtmp, 0, 0x3FFF);
                }
                break;
            }
        } else {
            switch (ctrl) {
            case 162:
                tmp = svoice != NULL ? svoice[0x12F] << 7 : 0;
                break;
            case 163:
                tmp = svoice != NULL ? (*(u32*)(svoice + 0x158)) >> 9 : 0;
                break;
            case 164:
                if (svoice != NULL) {
                    tmp = (s32)((((((u64)lbl_8047AF58) << 32) | (u64)lbl_8047AF5C) -
                                *(u64*)(svoice + 0x90)) >> 8);
                    if (tmp > 0x3fff) {
                        tmp = 0x3fff;
                    }
                    svoice[0xA8] = 1;
                } else {
                    tmp = 0;
                }
                break;
            default:
                tmp = inpGetMidiCtrl(ctrl, midi, midiSet);
                break;
            }
            tmp = (tmp * (inp->source[i].scale >> 1)) >> 15;
            if (tmp > 0x3FFF) {
                tmp = 0x3FFF;
            }
            switch (inp->source[i].combine & 0xF) {
            case 0:
                value = tmp;
                sign = 0;
                break;
            case 1:
                if (sign != 0) {
                    vtmp = (value + tmp);
                    vtmp -= 0x2000;
                    value = GIV_CLAMP_INV(vtmp, -0x2000, 0x1FFF) + 0x2000;
                } else {
                    value += tmp;
                    value = GIV_MIN(value, 0x3FFF);
                }
                break;
            case 2:
                if (sign != 0) {
                    vtmp = (s32)(tmp * (value - 0x2000)) >> 14;
                    value = GIV_CLAMP_INV(vtmp, -0x2000, 0x1FFF) + 0x2000;
                } else {
                    value = ((value * tmp) >> 0xE);
                    value = GIV_MIN(value, 0x3FFF);
                }
                break;
            case 3:
                if (sign != 0) {
                    vtmp = (value - 0x2000) - tmp;
                    value = GIV_CLAMP_INV(vtmp, -0x2000, 0x1FFF) + 0x2000;
                } else {
                    vtmp = value - tmp;
                    value = GIV_CLAMP(vtmp, 0, 0x3FFF);
                }
                break;
            }
        }
    }
    ((CtrlDest*)motionBase)->oldValue = value;
    return (u16)value;
}
#pragma pop
