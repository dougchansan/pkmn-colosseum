/**
 * @file hw_dspctrl.c
 * @brief MusyX DSP controller, 0x8015A484 - 0x8015D7D0.
 *
 * Consolidates the seven previously matched source pieces into the original
 * library unit, including its mixer/SRC tables, studio state and command-list
 * scratch storage. The existing function bodies retain their matching forms.
 *
 * Source family: AxioDL/musyx, commit
 * 89850639d731ec080c12cb0bc8d7ab2ea3880c3a,
 * src/musyx/runtime/hw_dspctrl.c (CC0-1.0 at that revision; see its LICENSE).
 * The earlier imported-source MIT notice in ../LICENSE.AxioDL-musyx is
 * retained for the existing implementation being consolidated here.
 * Colosseum uses the 2.0.0/pre-2.0.1 variant: salMalloc allocations, the
 * 32-entry mixer-cycle table, no _PBLPF, and no compressor DSP command.
 *
 * Shared state retains its existing symbol-map names for the other MusyX
 * units. Private helpers and tables recover their upstream names and scope.
 * salFrame, salAuxFrame, salNumVoices and salMaxStudioNum belong to hardware.c
 * and remain external. The original GC/1.3.2 -O4,p -inline auto settings,
 * with -use_lmw_stmw off, -sdata 8 and -sdata2 8, reproduce all eighteen
 * retained functions and their required helper calls in native CI.
 * AddDpop remains explicitly inline within HandleDepopVoice.
 */
#include "musyx/runtime/hw_dspctrl.h"
#include "dolphin/os/OSClock.h"

typedef u32 SND_STUDIO_TYPE;
typedef u32 (*SynthMessageCallback)(u32 message, u32 userValue);

typedef struct DSPADPCMblock {
    s16 Y0;
    s16 Y1;
    u8 PS;
    u8 reserved;
} DSPADPCMblock;

typedef struct DSPADPCMplusInfo {
    u16 numCoef;
    u8 initialPS;
    u8 loopPS;
    s16 loopY0;
    s16 loopY1;
    s16 coefTab[8][2];
    DSPADPCMblock blk[1];
} DSPADPCMplusInfo;

typedef struct SNDADPCMinfo {
    u16 numCoef;
    u8 initialPS;
    u8 loopPS;
    s16 loopY0;
    s16 loopY1;
    s16 coefTab[8][2];
} SNDADPCMinfo;

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

/* Address aliases preserve the public names used by adjacent runtime units. */
#define dspStudio             lbl_80447E60
#define dspCmdLastLoad        lbl_8047AFF4
#define dspCmdLastBase        lbl_8047AFF8
#define dspCmdLastSize        lbl_8047AFFC
#define dspCmdCurBase         lbl_8047B000
#define dspCmdMaxPtr          lbl_8047B004
#define dspCmdPtr             lbl_8047B008
#define dspCmdFirstSize       lbl_8047B00C
#define dspCmdList            lbl_8047B010
#define dspHRTFOn             lbl_8047B014
#define dspHrtfHistoryBuffer  lbl_8047B018
#define dspSurround           lbl_8047B01C
#define dspITDBuffer          lbl_8047B020
#define dspVoice              lbl_8047B024
#define salMessageCallback    lbl_8047B028
#define salMaxStudioNum       lbl_8047B05C
#define salNumVoices          lbl_8047B05D
#define salAuxFrame           lbl_8047B05E
#define salFrame              lbl_8047B05F

/* .bss 0x80447E60-0x80448540. MWCC emits these uninitialized definitions
 * in reverse order. voices is the private salBuildCommandList workspace;
 * file scope gives its symbol a stable name without changing its lifetime. */
static DSPvoice* voices[64];
DSPstudioinfo dspStudio[8];

/* .sbss 0x8047AFF0-0x8047B030. Explicit zero initializers retain declaration
 * order; the final four bytes are section alignment, not another state word. */
static u32 dspARAMZeroBuffer = 0;
u16* dspCmdLastLoad = NULL;
u16* dspCmdLastBase = NULL;
u16 dspCmdLastSize = 0;
u16* dspCmdCurBase = NULL;
u16* dspCmdMaxPtr = NULL;
u16* dspCmdPtr = NULL;
u16 dspCmdFirstSize = 0;
u16* dspCmdList = NULL;
u32 dspHRTFOn = FALSE;
u8* dspHrtfHistoryBuffer = NULL;
u16* dspSurround = NULL;
void* dspITDBuffer = NULL;
DSPvoice* dspVoice = NULL;
u32 salMessageCallback = 0;

/* hardware.c owns these frame indices and limits. */
extern u8 salFrame;

extern void* salMalloc(u32 size);
extern u32 aramGetZeroBuffer(void);
extern void DCFlushRange(void* addr, u32 size);
extern void DCInvalidateRange(void* addr, u32 size);
extern void DCStoreRangeNoSync(void* addr, u32 size);
extern u32 adsrSetup(AdsrVars* adsr);
extern u32 adsrStartRelease(AdsrVars* adsr, u32 rtime);
extern u32 adsrRelease(AdsrVars* adsr);
extern u32 adsrHandle(AdsrVars* adsr, u16* adsr_start, u16* adsr_delta);

void salActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type);
void salInitHRTFBuffer(void);
u32 salSynthSendMessage(DSPvoice* voice, u32 message);
void salDeactivateVoice(DSPvoice* voice);

u32 salInitDspCtrl(u8 numVoices, u8 numStudios, u32 defaultStudioDPL2)
{
    u32 i;
    u32 j;
    u32 itdPtr;

    salNumVoices = numVoices;
    salMaxStudioNum = numStudios;

    dspARAMZeroBuffer = aramGetZeroBuffer();
    if ((dspCmdList = salMalloc(1024 * sizeof(u16)))) {
        if ((dspSurround = salMalloc(160 * sizeof(s32)))) {
            memset(dspSurround, 0, 160 * sizeof(s32));
            DCFlushRange(dspSurround, 160 * sizeof(s32));
            if ((dspVoice = salMalloc(salNumVoices * sizeof(DSPvoice)))) {
                if ((dspITDBuffer = salMalloc(salNumVoices * 64))) {
                    DCInvalidateRange(dspITDBuffer, salNumVoices * 64);
                    itdPtr = (u32)dspITDBuffer;
                    for (i = 0; i < salNumVoices; ++i) {
                        dspVoice[i].state = 0;
                        dspVoice[i].postBreak = 0;
                        dspVoice[i].startupBreak = 0;
                        dspVoice[i].lastUpdate.pitch = 0xFF;
                        dspVoice[i].lastUpdate.vol = 0xFF;
                        dspVoice[i].lastUpdate.volA = 0xFF;
                        dspVoice[i].lastUpdate.volB = 0xFF;
                        dspVoice[i].pb = salMalloc(sizeof(_PB));
                        memset(dspVoice[i].pb, 0, sizeof(_PB));
                        dspVoice[i].patchData = salMalloc(0x80);
                        dspVoice[i].pb->currHi = ((u32)dspVoice[i].pb >> 16);
                        dspVoice[i].pb->currLo = (u16)(u32)dspVoice[i].pb;
                        dspVoice[i].pb->update.dataHi = ((u32)dspVoice[i].patchData >> 16);
                        dspVoice[i].pb->update.dataLo = ((u16)(u32)dspVoice[i].patchData);
                        dspVoice[i].pb->itd.bufferHi = ((u32)itdPtr >> 16);
                        dspVoice[i].pb->itd.bufferLo = ((u16)itdPtr);
                        dspVoice[i].itdBuffer = (void*)itdPtr;
                        itdPtr += 0x40;
                        dspVoice[i].virtualSampleID = 0xFFFFFFFF;
                        DCStoreRangeNoSync(dspVoice[i].pb, sizeof(_PB));
                        for (j = 0; j < 5; ++j) {
                            dspVoice[i].changed[j] = 0;
                        }
                    }

                    for (i = 0; i < salMaxStudioNum; ++i) {
                        dspStudio[i].state = 0;
                        if (!(dspStudio[i].spb = salMalloc(0x36 /* sizeof(_SPB) */))) {
                            return FALSE;
                        }

                        if (!(dspStudio[i].main[0] = salMalloc(0x3C00))) {
                            return FALSE;
                        }

                        memset(dspStudio[i].main[0], 0, 0x3C00);
                        DCFlushRangeNoSync(dspStudio[i].main[0], 0x3C00);
                        dspStudio[i].main[1] = dspStudio[i].main[0] + 0x1E0;
                        dspStudio[i].auxA[0] = dspStudio[i].main[1] + 0x1E0;
                        dspStudio[i].auxA[1] = dspStudio[i].auxA[0] + 0x1E0;
                        dspStudio[i].auxA[2] = dspStudio[i].auxA[1] + 0x1E0;
                        dspStudio[i].auxB[0] = dspStudio[i].auxA[2] + 0x1E0;
                        dspStudio[i].auxB[1] = dspStudio[i].auxB[0] + 0x1E0;
                        dspStudio[i].auxB[2] = dspStudio[i].auxB[1] + 0x1E0;
                        memset(dspStudio[i].spb, 0, 0x36 /* sizeof(_SPB) */);
                        dspStudio[i].hostDPopSum.l = dspStudio[i].hostDPopSum.r =
                            dspStudio[i].hostDPopSum.s = 0;
                        dspStudio[i].hostDPopSum.lA = dspStudio[i].hostDPopSum.rA =
                            dspStudio[i].hostDPopSum.sA = 0;
                        dspStudio[i].hostDPopSum.lB = dspStudio[i].hostDPopSum.rB =
                            dspStudio[i].hostDPopSum.sB = 0;
                        DCFlushRangeNoSync(dspStudio[i].spb, 0x36);
                    }
                    salActivateStudio(0, 1, defaultStudioDPL2 != FALSE ? 1 : 0);
                    if (!(dspHrtfHistoryBuffer = salMalloc(0x100))) {
                        return FALSE;
                    }

                    salInitHRTFBuffer();
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void salInitHRTFBuffer(void)
{
    memset(dspHrtfHistoryBuffer, 0, 0x100);
    DCFlushRangeNoSync(dspHrtfHistoryBuffer, 0x100);
}

u32 salExitDspCtrl(void)
{
    u8 i;

    salFree(dspHrtfHistoryBuffer);
    for (i = 0; i < salNumVoices; i++) {
        salFree(dspVoice[i].pb);
        salFree(dspVoice[i].patchData);
    }
    for (i = 0; i < salMaxStudioNum; i++) {
        salFree(dspStudio[i].spb);
        salFree(dspStudio[i].main[0]);
    }
    salFree(dspITDBuffer);
    salFree(dspVoice);
    salFree(dspSurround);
    salFree(dspCmdList);
    return 1;
}

void salActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type)
{
    memset(dspStudio[studio].main[0], 0, 0x3C00);
    DCFlushRangeNoSync(dspStudio[studio].main[0], 0x3C00);
    memset(dspStudio[studio].spb, 0, 0x36 /* sizeof(_SPB) */);
    dspStudio[studio].hostDPopSum.l = dspStudio[studio].hostDPopSum.r =
        dspStudio[studio].hostDPopSum.s = 0;
    dspStudio[studio].hostDPopSum.lA = dspStudio[studio].hostDPopSum.rA =
        dspStudio[studio].hostDPopSum.sA = 0;
    dspStudio[studio].hostDPopSum.lB = dspStudio[studio].hostDPopSum.rB =
        dspStudio[studio].hostDPopSum.sB = 0;
    DCFlushRangeNoSync(dspStudio[studio].spb, 0x36 /* sizeof(_SPB) */);
    memset(dspStudio[studio].auxA[0], 0, 0x780);
    DCFlushRangeNoSync(dspStudio[studio].auxA[0], 0x780);
    memset(dspStudio[studio].auxB[0], 0, 0x780);
    DCFlushRangeNoSync(dspStudio[studio].auxB[0], 0x780);
    dspStudio[studio].voiceRoot = NULL;
    dspStudio[studio].alienVoiceRoot = NULL;
    dspStudio[studio].state = 1;
    dspStudio[studio].isMaster = isMaster;
    dspStudio[studio].numInputs = 0;
    dspStudio[studio].type = type;
    dspStudio[studio].auxAHandler = dspStudio[studio].auxBHandler = NULL;
}

/* .data 0x80369A50: nine SRC costs; the six following bytes are alignment. */
static u16 dspSRCCycles[3][3] = {
    {2990, 2990, 1115},
    {3300, 3300, 1115},
    {3700, 3700, 1115},
};

/* .rodata 0x802732E0: the pre-2.0.1 mixer bitfield has 32 cost entries. */
static const u16 dspMixerCycles[32] = {
    1470, 2940, 2940, 4410, 2230, 4460, 4460, 6690,
    2470, 4940, 4940, 7410, 3735, 7470, 7470, 11205,
    2940, 3386, 2940, 3386, 2940, 3386, 2940, 3386,
    4940, 5687, 4940, 5687, 4940, 5687, 4940, 5687,
};

/* .rodata 0x80273320: salBuildCommandList's DSP parameter-block word
 * offsets. The six following bytes are section alignment. */
static const u16 pbOffsets[9] = {10, 12, 24, 14, 16, 26, 18, 20, 22};

void salDeactivateStudio(u32 studio)
{
    dspStudio[(u8) studio].state = 0;
}

static u32 salCheckVolErrorAndResetDelta(u16* dspVol, u16* dspDelta,
                                  u16* lastVol, u16 targetVol,
                                  u16* resetFlags, u16 resetMask)
{
    s16 delta;
    s16 steps;

    if (targetVol != *lastVol) {
        delta = (s16) targetVol - (s16) *lastVol;
        if ((s16) delta >= 32 && (s16) delta < 160) {
            steps = (s16) delta >> 5;
            if ((s16) steps < 5) {
                resetFlags[steps] |= resetMask;
            }
            *dspDelta = 1;
            *lastVol += steps << 5;
            return 1;
        }
        if (-32 >= (s16) delta && -160 < (s16) delta) {
            steps = -(s16) delta >> 5;
            if (steps < 5) {
                resetFlags[steps] |= resetMask;
            }
            *dspDelta = 0xFFFF;
            *lastVol -= steps << 5;
            return 1;
        }
        if (targetVol == 0 && (s16) delta > -32) {
            *dspVol = *lastVol = 0;
        }
    }
    *dspDelta = 0;
    return 0;
}

static void sal_setup_dspvol(u16* dspDelta, u16* lastVol, u16 volume)
{
    *dspDelta = ((s16) volume - (s16) *lastVol) / 160;
    *lastVol += (s16) *dspDelta * 160;
}

static void sal_update_hostplayinfo(DSPvoice* voice)
{
    u32 pitch;
    u32 oldLo;

    if (voice->smp_info.loopLength != 0) {
        return;
    }
    if (voice->pb->srcSelect != 2) {
        pitch = voice->playInfo.pitch << 5;
    } else {
        pitch = 0x200000;
    }
    oldLo = voice->playInfo.posLo;
    voice->playInfo.posLo += pitch << 16;
    if (oldLo > voice->playInfo.posLo) {
        voice->playInfo.posHi += (pitch >> 16) + 1;
    } else {
        voice->playInfo.posHi += pitch >> 16;
    }
}

static void DoDepopFade(s32* dspStart, s16* dspDelta, s32* hostSum)
{
    s16 delta;

    if (*hostSum <= -160) {
        if (*hostSum <= -3200) {
            delta = 20;
        } else {
            delta = (s16) (-*hostSum / 160);
        }
        *dspDelta = delta;
    } else if (*hostSum >= 160) {
        if (*hostSum >= 3200) {
            delta = -20;
        } else {
            delta = (s16) (-*hostSum / 160);
        }
        *dspDelta = delta;
    } else {
        *dspDelta = 0;
    }
    *dspStart = *hostSum;
    *hostSum += *dspDelta * 160;
}

static inline void AddDpop(s32* sum, s16 delta) {
    *sum += delta;
    *sum = (*sum > 0x7FFFFF)
               ? 0x7FFFFF
               : (*sum < -0x7FFFFF ? -0x7FFFFF : *sum);
}

static void HandleDepopVoice(DSPstudioinfo* stp, DSPvoice* dsp_vptr) {
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

static void SortVoices(DSPvoice** voices, s32 l, s32 r) {
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

void salBuildCommandList(s16* dest, u32 nsDelay) {
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
        cyclesUsed = ((nsDelay - 200) * ((__OSBusClock / 400) / 5000)) + 10430;
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
                    HandleDepopVoice(stp, dsp_vptr);
                    if (dsp_vptr->virtualSampleID != (u32)-1) {
                        salSynthSendMessage(dsp_vptr, 3);
                    }
                    if ((dsp_vptr->state != 1) || (dsp_vptr->startupBreak != 0)) {
                        salDeactivateVoice(dsp_vptr);
                        dsp_vptr->startupBreak = 0;
                    }
                }
            }
            dsp_vptr = stp->alienVoiceRoot;
            while (dsp_vptr) {
                HandleDepopVoice(stp, dsp_vptr);
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
            voiceNum = (int)v;
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
                            salDeactivateVoice(dsp_vptr);
                            continue;
                        }
                        dsp_vptr->virtualSampleID = (u32)-1;
                        switch (dsp_vptr->smp_info.compType) {
                        case 5:
                            dsp_vptr->vSampleInfo.loopBufferLength = 0;
                            dsp_vptr->virtualSampleID = salSynthSendMessage(dsp_vptr, 2);
                            if (dsp_vptr->vSampleInfo.loopBufferLength == 0) {
                                salSynthSendMessage(dsp_vptr, 1);
                                salDeactivateVoice(dsp_vptr);
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
                                tmp_addr = base + dsp_vptr->smp_info.length;
                                zeroAddr = dspARAMZeroBuffer >> 1;
                                break;
                            default:
                                /* Retail sets neither address for an unknown
                                 * compType: its dispatch sends compType < 0
                                 * and >= 6 straight to the stores below
                                 * (0x8015BC24, 0x8015BC2C -> 0x8015BC98), so
                                 * tmp_addr and zeroAddr are read
                                 * uninitialised on that path. That is also
                                 * why both live in callee-saved registers
                                 * (r26, r15) for the whole function. */
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
                        salDeactivateVoice(dsp_vptr);
                        continue;
                    }
                    if (((dsp_vptr->changed[0] & 0x10) != 0) && (adsrSetup(&dsp_vptr->adsr) != 0)) {
                        salSynthSendMessage(dsp_vptr, 0);
                        salDeactivateVoice(dsp_vptr);
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
                            salDeactivateVoice(dsp_vptr);
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
                        salDeactivateVoice(dsp_vptr);
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
                    if (cyclesUsed > (__OSBusClock / 400)) {
                        if ((newVoice == 0) && (VoiceDone == 0)) {
                            HandleDepopVoice(stp, dsp_vptr);
                        }
                        salDeactivateVoice(dsp_vptr);
                        salSynthSendMessage(dsp_vptr, 1);
                        for (v = v - 1; v > 0; v--) {
                            if (voices[v - 1]->state == 2) {
                                HandleDepopVoice(stp, voices[v - 1]);
                            }
                            salDeactivateVoice(voices[v - 1]);
                            salSynthSendMessage(voices[v - 1], 1);
                        }
                        for (st1 = st + 1; st1 < salMaxStudioNum; st1++) {
                            if (dspStudio[st1].state == 1) {
                                for (dsp_vptr = dspStudio[st1].voiceRoot; dsp_vptr; dsp_vptr = next_dsp_vptr) {
                                    next_dsp_vptr = dsp_vptr->next;
                                    if (dsp_vptr->state == 2) {
                                        HandleDepopVoice(&dspStudio[st1], dsp_vptr);
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

u32 salSynthSendMessage(DSPvoice* voice, u32 message)
{
    SynthMessageCallback callback = (SynthMessageCallback) salMessageCallback;

    if (callback == NULL) {
        return 0;
    }
    return callback(message, voice->mesgCallBackUserValue);
}

void salActivateVoice(DSPvoice* voice, u8 studio)
{
    if (voice->state != 0) {
        salDeactivateVoice(voice);
        voice->changed[0] |= 0x20;
    }
    voice->postBreak = 0;
    if ((voice->next = dspStudio[studio].voiceRoot) != NULL) {
        voice->next->prev = voice;
    }
    voice->prev = NULL;
    dspStudio[studio].voiceRoot = voice;
    voice->startupBreak = 0;
    voice->state = 1;
    voice->studio = studio;
}

void salDeactivateVoice(DSPvoice* voice)
{
    if (voice->state == 0) {
        return;
    }
    if (voice->prev != NULL) {
        voice->prev->next = voice->next;
    } else {
        dspStudio[voice->studio].voiceRoot = voice->next;
    }
    if (voice->next != NULL) {
        voice->next->prev = voice->prev;
    }
    voice->state = 0;
}

u32 salAddStudioInput(DSPstudioinfo* studio, SND_STUDIO_INPUT* input)
{
    if (studio->numInputs < 7) {
        studio->in[studio->numInputs].studio = input->srcStudio;
        studio->in[studio->numInputs].vol =
            ((u16) input->vol << 8) | ((u16) input->vol << 1);
        studio->in[studio->numInputs].volA =
            ((u16) input->volA << 8) | ((u16) input->volA << 1);
        studio->in[studio->numInputs].volB =
            ((u16) input->volB << 8) | ((u16) input->volB << 1);
        studio->in[studio->numInputs].desc = input;
        studio->numInputs++;
        return 1;
    }
    return 0;
}

u32 salRemoveStudioInput(DSPstudioinfo* studio, SND_STUDIO_INPUT* input)
{
    long i;

    for (i = 0; i < studio->numInputs; i++) {
        if (studio->in[i].desc == input) {
            for (; i <= studio->numInputs - 2; i++) {
                studio->in[i] = studio->in[i + 1];
            }
            studio->numInputs--;
            return 1;
        }
    }
    return 0;
}

/* Last function of hw_dspctrl.c; hw_volconv.c starts at 0x8015D7D0. */
typedef struct SND_AUX_INFO {
    union {
        struct {
            s32* left;
            s32* right;
            s32* surround;
        } bufferUpdate;
    } data;
} SND_AUX_INFO;

typedef void (*SND_AUX_CALLBACK)(u8 reason, SND_AUX_INFO* info, void* user);

void salHandleAuxProcessing(void)
{
    u8 st;
    s32* work;
    DSPstudioinfo* sp;
    SND_AUX_INFO ai;

    sp = &dspStudio[0];
    for (st = 0; st < salMaxStudioNum; ++st, ++sp) {
        if (sp->state != 1) {
            continue;
        }
        if (sp->auxAHandler != NULL) {
            work = sp->auxA[(salAuxFrame + 2) % 3];
            ai.data.bufferUpdate.left = work;
            ai.data.bufferUpdate.right = work + 0xA0;
            ai.data.bufferUpdate.surround = work + 0x140;
            ((SND_AUX_CALLBACK)sp->auxAHandler)(0, &ai, sp->auxAUser);
            DCFlushRangeNoSync(work, 0x780);
        }
        if (sp->type == 0 && sp->auxBHandler != NULL) {
            work = sp->auxB[(salAuxFrame + 2) % 3];
            ai.data.bufferUpdate.left = work;
            ai.data.bufferUpdate.right = work + 0xA0;
            ai.data.bufferUpdate.surround = work + 0x140;
            ((SND_AUX_CALLBACK)sp->auxBHandler)(0, &ai, sp->auxBUser);
            DCFlushRangeNoSync(work, 0x780);
        }
    }
}
