#include "dolphin/ai/AI.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/OSContext.h"

/*
 * AI.c - Audio Interface (AI) driver for GameCube.
 *
 * Controls the audio DMA engine that streams audio samples from
 * main memory to the audio DAC at 32kHz or 48kHz.
 *
 * Adapted from doldecomp/melee matching implementation.
 *
 * Matches: part of 0x800AE4F0 - 0x800AF280 range
 */

/* AI hardware registers at 0xCC006C00 */
#define AI_CONTROL       (*(volatile u32*)0xCC006C00)
#define AI_VOLUME        (*(volatile u32*)0xCC006C04)
#define AI_SAMPLE_COUNT  (*(volatile u32*)0xCC006C08)
#define AI_INT_TIMING    (*(volatile u32*)0xCC006C0C)

#define AI_CONTROL_DMA_ENABLE  0x8000
#define AI_CONTROL_SAMPLE_RATE 0x0040
#define AI_CONTROL_INT_ENABLE  0x0004
#define AI_CONTROL_INT_STATUS  0x0008

extern void OSRegisterVersion(const char* id);
extern void OSReport(const char* fmt, ...);

static const char* __AIVersion = "<< Dolphin SDK - AI\trelease build: Aug 22 2002 04:07:21 (0x2301) >>";

static AISCallback StreamCallback;
static AIDCallback DMACallback;
static BOOL AIInitFlag;
static u32 AISampleRate;

static void __AIInterruptHandler(__OSInterrupt interrupt, OSContext* context);

/*
 * AICheckInit - Check if AI is initialized.
 */
BOOL AICheckInit(void) {
    return AIInitFlag;
}

/*
 * AIInit - Initialize the Audio Interface.
 */
void AIInit(u8* stack) {
    if (AIInitFlag) {
        return;
    }

    OSRegisterVersion(__AIVersion);

    AIInitFlag = TRUE;
    StreamCallback = NULL;
    DMACallback = NULL;

    /* Set default sample rate to 48kHz */
    AISampleRate = 0;  /* 0 = 48kHz, 1 = 32kHz */
    AI_CONTROL = 0;
    AI_VOLUME = 0xFF;  /* Max volume both channels */

    /* Install AI interrupt handler */
    __OSSetInterruptHandler(0x05, (__OSInterruptHandler)__AIInterruptHandler);
    __OSUnmaskInterrupts(0x04000000);
}

/*
 * AIReset - Reset the Audio Interface.
 */
void AIReset(void) {
    AIInitFlag = FALSE;
}

/*
 * AIStartDMA - Start audio DMA.
 */
void AIStartDMA(void) {
    AI_CONTROL |= AI_CONTROL_DMA_ENABLE;
}

/*
 * AIStopDMA - Stop audio DMA.
 */
void AIStopDMA(void) {
    AI_CONTROL &= ~AI_CONTROL_DMA_ENABLE;
}

/*
 * AIInitDMA - Initialize DMA with address and length.
 */
void AIInitDMA(u32 addr, u32 length) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Set DMA start address (physical) */
    *(volatile u32*)0xCC005030 = addr & 0x1FFFFFFF;
    /* Set DMA length */
    *(volatile u32*)0xCC005036 = (u16)(length >> 5);

    OSRestoreInterrupts(enabled);
}

/*
 * AIGetDMABytesLeft - Get remaining DMA bytes.
 */
u32 AIGetDMABytesLeft(void) {
    return (*(volatile u16*)0xCC00503A) << 5;
}

/*
 * AIGetDMAStartAddr - Get DMA start address.
 */
u32 AIGetDMAStartAddr(void) {
    return (*(volatile u32*)0xCC005030) & 0x03FFFFFF;
}

/*
 * AIGetDMALength - Get DMA length.
 */
u16 AIGetDMALength(void) {
    return *(volatile u16*)0xCC005036;
}

/*
 * AIGetDSPSampleRate - Get the current sample rate.
 */
u32 AIGetDSPSampleRate(void) {
    return AISampleRate;
}

/*
 * AISetDSPSampleRate - Set the sample rate.
 */
void AISetDSPSampleRate(u32 rate) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    AISampleRate = rate;
    if (rate) {
        AI_CONTROL |= AI_CONTROL_SAMPLE_RATE;
    } else {
        AI_CONTROL &= ~AI_CONTROL_SAMPLE_RATE;
    }

    OSRestoreInterrupts(enabled);
}

/*
 * AIRegisterStreamCallback - Register the stream callback.
 */
AISCallback AIRegisterStreamCallback(AISCallback callback) {
    AISCallback old;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    old = StreamCallback;
    StreamCallback = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

/*
 * AIRegisterDMACallback - Register the DMA callback.
 */
AIDCallback AIRegisterDMACallback(AIDCallback callback) {
    AIDCallback old;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    old = DMACallback;
    DMACallback = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

/*
 * __AIInterruptHandler - AI interrupt handler.
 */
static void __AIInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u32 ctrl;
    OSContext tempCtx;

    ctrl = AI_CONTROL;

    if (ctrl & AI_CONTROL_INT_STATUS) {
        /* Clear interrupt */
        AI_CONTROL = ctrl | AI_CONTROL_INT_STATUS;

        /* Call DMA callback */
        if (DMACallback != NULL) {
            OSClearContext(&tempCtx);
            OSSetCurrentContext(&tempCtx);
            DMACallback();
            OSClearContext(&tempCtx);
            OSSetCurrentContext(context);
        }
    }

    /* Check for stream interrupt */
    if (StreamCallback != NULL) {
        OSClearContext(&tempCtx);
        OSSetCurrentContext(&tempCtx);
        StreamCallback(AI_SAMPLE_COUNT);
        OSClearContext(&tempCtx);
        OSSetCurrentContext(context);
    }
}
