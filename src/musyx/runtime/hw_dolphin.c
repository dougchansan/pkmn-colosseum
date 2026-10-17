/**
 * @file hw_dolphin.c
 * @brief MusyX Dolphin AI/DSP glue, 0x80163EE0 - 0x801643D8.
 *
 * Follows the reference MusyX runtime's hw_dolphin.c (AxioDL/musyx) in its
 * pre-2.0.1 form (salExitDsp halts and resets the DSP, hwIRQEnterCritical
 * returns the interrupt state). The reference's static state words keep
 * their .sbss order (oldState 0x8047B080 ... userCallback 0x8047B0A4) and
 * stay extern here under their symbol-map names, as do dsp_task and
 * dram_image (.bss).
 *
 * callUserCallback is the reference's static inline helper; retail expands
 * the same sequence in salCallback (0x80163F3C) and dspResumeCallback
 * (0x80163FC0).
 */
#include "dolphin/types.h"
#include "dolphin/ai/AI.h"
#include "dolphin/os/OSInterrupt.h"

typedef void (*SND_SOME_CALLBACK)(void);
typedef void (*DSPCallback)(void* task);

typedef struct DSPTaskInfo {
    volatile u32 state;
    volatile u32 priority;
    volatile u32 flags;
    u16* iram_mmem_addr;
    u32 iram_length;
    u32 iram_addr;
    u16* dram_mmem_addr;
    u32 dram_length;
    u32 dram_addr;
    u16 dsp_init_vector;
    u16 dsp_resume_vector;
    DSPCallback init_cb;
    DSPCallback res_cb;
    DSPCallback done_cb;
    DSPCallback req_cb;
    struct DSPTaskInfo* next;
    struct DSPTaskInfo* prev;
    s64 t_context;
    s64 t_task;
} DSPTaskInfo;

typedef struct SND_PLAYBACKINFO {
    u32 frq;
    u8 stereo;
    u8 bits;
    s8 deviceName[256];
    s8 versionText[256];
} SND_PLAYBACKINFO;

typedef struct SynthInfo {
    u32 mixFrq;
    u32 numSamples;
    SND_PLAYBACKINFO pbInfo;
    u8 voiceNum;
    u8 maxMusic;
    u8 maxSFX;
    u8 studioNum;
} SynthInfo;

#define DMA_BUFFER_LEN 0x280
#define OSCachedToPhysical(caddr) ((u32)(caddr) - 0x80000000)
#define OS_BUS_CLOCK (*(u32*)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSTicksToMicroseconds(ticks) (((ticks) * 8) / (OS_TIMER_CLOCK / 125000))

extern DSPTaskInfo lbl_804504A0;             /* dsp_task */
extern u16 lbl_80450500[];                   /* dram_image */
extern u8 lbl_8036A520[];                    /* dspSlave */
extern u16 lbl_80478C08;                     /* dspSlaveLength */
extern SynthInfo lbl_80434C50;               /* synthInfo */
extern u16 lbl_8047B00C;                     /* dspCmdFirstSize */
extern u16* lbl_8047B010;                    /* dspCmdList */

extern volatile u32 lbl_8047B080;            /* oldState */
extern volatile u16 lbl_8047B084;            /* hwIrqLevel */
extern volatile u32 lbl_8047B088;            /* salDspInitIsDone */
extern volatile u32 lbl_8047B08C;            /* salLastTick */
extern volatile u32 lbl_8047B090;            /* salLogicActive */
extern volatile u32 lbl_8047B094;            /* salLogicIsWaiting */
extern volatile u32 lbl_8047B098;            /* salDspIsDone */
extern void* lbl_8047B09C;                   /* salAIBufferBase */
extern u8 lbl_8047B0A0;                      /* salAIBufferIndex */
extern SND_SOME_CALLBACK lbl_8047B0A4;       /* userCallback */

extern void* memset(void* dst, int value, u32 size);
extern void DCFlushRange(void* addr, u32 size);
extern u32 OSGetTick(void);
extern void PPCSync(void);
extern void DSPInit(void);
extern DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
extern void DSPSendMailToDSP(u32 mail);
extern u32 fn_800AE794(void);                /* DSPCheckMailToDSP */
extern void DSPHalt(void);               /* DSPHalt */
extern u32 DSPGetDMAStatus(void);                /* DSPGetDMAStatus */
extern void DSPReset(void);               /* DSPReset */
extern void* salMalloc(u32 size);          /* salMalloc */
extern void salFree(void* ptr);          /* salFree */
extern void salBuildCommandList(u16* dest, u32 nsDelay); /* salBuildCommandList */

u32 salGetStartDelay(void);
void hwEnableIrq(void);
void hwDisableIrq(void);

static inline void callUserCallback(void)
{
    if (lbl_8047B090) {
        return;
    }
    lbl_8047B090 = 1;
    OSEnableInterrupts();
    lbl_8047B0A4();
    OSDisableInterrupts();
    lbl_8047B090 = 0;
}

void salCallback(void)
{
    lbl_8047B0A0 = (lbl_8047B0A0 + 1) % 4;
    AIInitDMA(OSCachedToPhysical(lbl_8047B09C) + (lbl_8047B0A0 * DMA_BUFFER_LEN), DMA_BUFFER_LEN);
    lbl_8047B08C = OSGetTick();

    if (lbl_8047B098) {
        callUserCallback();
    } else {
        lbl_8047B094 = 1;
    }
}

void dspInitCallback(void) /* dspInitCallback */
{
    lbl_8047B098 = TRUE;
    lbl_8047B088 = TRUE;
}

void dspResumeCallback(void)
{
    lbl_8047B098 = TRUE;
    if (lbl_8047B094) {
        lbl_8047B094 = FALSE;
        callUserCallback();
    }
}

u32 salInitAi(SND_SOME_CALLBACK callback, u32 unk, u32* outFreq)
{
    if ((lbl_8047B09C = salMalloc(DMA_BUFFER_LEN * 4)) != NULL) {
        memset(lbl_8047B09C, 0, DMA_BUFFER_LEN * 4);
        DCFlushRange(lbl_8047B09C, DMA_BUFFER_LEN * 4);
        lbl_8047B0A0 = TRUE;
        lbl_8047B094 = FALSE;
        lbl_8047B098 = TRUE;
        lbl_8047B090 = FALSE;
        lbl_8047B0A4 = callback;
        AIRegisterDMACallback(salCallback);
        AIInitDMA(OSCachedToPhysical(lbl_8047B09C) + (lbl_8047B0A0 * DMA_BUFFER_LEN),
                  DMA_BUFFER_LEN);
        lbl_80434C50.numSamples = 0x20;
        *outFreq = 32000;
        return TRUE;
    }

    return FALSE;
}

void salStartAi(void) /* salStartAi */
{
    AIStartDMA();
}

u32 salExitAi(void)
{
    AIRegisterDMACallback(NULL);
    AIStopDMA();
    salFree(lbl_8047B09C);
    return TRUE;
}

void* salAiGetDest(void)
{
    u8 index;

    index = (lbl_8047B0A0 + 2) % 4;
    return (void*)((u8*)lbl_8047B09C + index * DMA_BUFFER_LEN);
}

u32 salInitDsp(void) /* salInitDsp */
{
    lbl_804504A0.iram_mmem_addr = (u16*)lbl_8036A520;
    lbl_804504A0.iram_length = lbl_80478C08;
    lbl_804504A0.iram_addr = 0;
    lbl_804504A0.dram_mmem_addr = lbl_80450500;
    lbl_804504A0.dram_length = 0x2000;
    lbl_804504A0.dram_addr = 0;
    lbl_804504A0.dsp_init_vector = 0x10;
    lbl_804504A0.dsp_resume_vector = 0x30;
    lbl_804504A0.init_cb = (DSPCallback)dspInitCallback;
    lbl_804504A0.res_cb = (DSPCallback)dspResumeCallback;
    lbl_804504A0.done_cb = NULL;
    lbl_804504A0.req_cb = NULL;
    lbl_804504A0.priority = 0;
    DSPInit();
    DSPAddTask(&lbl_804504A0);
    lbl_8047B088 = FALSE;
    hwEnableIrq();
    while (!lbl_8047B088) {
    }
    hwDisableIrq();
    return TRUE;
}

u32 salExitDsp(void) /* salExitDsp */
{
    DSPHalt();
    while (DSPGetDMAStatus()) {
    }
    DSPReset();
    return TRUE;
}

static void salStartDsp(u16* cmdList)
{
    lbl_8047B098 = FALSE;
    PPCSync();
    DSPSendMailToDSP(lbl_8047B00C | 0xBABE0000);

    while (fn_800AE794()) {
    }
    DSPSendMailToDSP((u32)cmdList);
    while (fn_800AE794()) {
    }
}

void salCtrlDsp(u16* dest)
{
    salBuildCommandList(dest, salGetStartDelay());
    salStartDsp(lbl_8047B010);
}

u32 salGetStartDelay(void)
{
    return OSTicksToMicroseconds(OSGetTick() - lbl_8047B08C);
}

void hwInitIrq(void)
{
    lbl_8047B080 = OSDisableInterrupts();
    lbl_8047B084 = 1;
}

void hwExitIrq(void) /* hwExitIrq */
{
}

void hwEnableIrq(void)
{
    if (--lbl_8047B084 == 0) {
        OSRestoreInterrupts(lbl_8047B080);
    }
}

void hwDisableIrq(void)
{
    if ((lbl_8047B084++) == 0) {
        lbl_8047B080 = OSDisableInterrupts();
    }
}

u32 hwIRQEnterCritical(void) /* hwIRQEnterCritical */
{
    return OSDisableInterrupts();
}

u32 hwIRQLeaveCritical(void) /* hwIRQLeaveCritical */
{
    return OSEnableInterrupts();
}
