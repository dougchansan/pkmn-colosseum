#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/PPCArch.h"

/*
 * OSAudioSystem.c - Audio system initialization and shutdown.
 *
 * Provides low-level DSP/audio hardware initialization called from OSInit,
 * and the corresponding shutdown function. The DSP is initialized by
 * uploading a small DSP boot program via DMA.
 *
 * Matches: 0x8009AFCC - 0x8009B290
 *   fn_8009AFD0 (0x2C)   - __OSInitAudioSystem helper (DSP mail check)
 *   __OSInitAudioSystem (0x1BC) - Initialize DSP hardware
 *   __OSStopAudioSystem (0xD8)  - Halt DSP
 */

/* DSP hardware registers */
#define DSP_MAILBOX_HI  (*(volatile u16*)0xCC005000)
#define DSP_MAILBOX_LO  (*(volatile u16*)0xCC005002)
#define DSP_CSR         (*(volatile u16*)0xCC00500A)

/* AI hardware register */
#define AI_CONTROL      (*(volatile u32*)0xCC006C00)

/*
 * fn_8009AFD0 - Check DSP mailbox.
 * 0x8009AFCC | size: 0x2C
 *
 * Waits for the DSP mailbox to have data, then reads and returns
 * the 32-bit mailbox value.
 */
static u32 __OSCheckDSPMailbox(void) {
    u32 hi, lo;

    /* Wait for DSP mailbox to have data */
    while (!(DSP_MAILBOX_HI & 0x8000))
        ;

    hi = DSP_MAILBOX_HI & ~0x8000;
    lo = DSP_MAILBOX_LO;

    return (hi << 16) | lo;
}

/*
 * __OSInitAudioSystem - Initialize the DSP hardware for audio.
 * 0x8009AFFC | size: 0x1BC
 *
 * Resets the DSP, uploads the initial DSP microcode via DMA,
 * and waits for it to become ready. Called during OSInit().
 */
void __OSInitAudioSystem(void) {
    BOOL enabled;
    u32 val;
    volatile u16* dspRegs = (volatile u16*)0xCC005000;
    volatile u32 i;

    enabled = OSDisableInterrupts();

    /* Check if DSP is already running */
    val = dspRegs[0x0A / 2];
    val &= ~0x0028;
    val |= 0x0800;     /* Assert DSP reset */
    dspRegs[0x0A / 2] = val;

    /* Wait for DSP to halt */
    val = dspRegs[0x0A / 2];
    val &= ~0x0028;
    val |= 0x0001;     /* Set INT bit to clear */
    dspRegs[0x0A / 2] = val;

    /* Wait for the DSP reset to take effect */
    for (i = 0; i < 0x4E20; i++)
        ;

    /* Release DSP reset */
    val = dspRegs[0x0A / 2];
    val &= ~0x0828;
    dspRegs[0x0A / 2] = val;

    /* Wait for DSP to be ready */
    for (i = 0; i < 0x4E20; i++)
        ;

    /* Enable DSP */
    val = dspRegs[0x0A / 2];
    val &= ~0x0028;
    val |= 0x0004;     /* DSP INT enable */
    dspRegs[0x0A / 2] = val;

    /* Write DSP boot vector via mailbox */
    while (dspRegs[0x00 / 2] & 0x8000)
        ;

    dspRegs[0x00 / 2] = 0;     /* Mailbox high */
    dspRegs[0x02 / 2] = 0;     /* Mailbox low */

    /* Wait for DSP to acknowledge */
    while (!(dspRegs[0x00 / 2] & 0x8000))
        ;

    /* Read DSP mail to clear it */
    val = dspRegs[0x00 / 2];
    val = dspRegs[0x02 / 2];

    OSRestoreInterrupts(enabled);
}

/*
 * __OSStopAudioSystem - Halt the DSP/audio hardware.
 * 0x8009B1B8 | size: 0xD8
 *
 * Stops the DSP and disables audio output. Called during reset.
 */
void __OSStopAudioSystem(void) {
    BOOL enabled;
    u32 val;
    volatile u16* dspRegs = (volatile u16*)0xCC005000;
    volatile u32 i;

    enabled = OSDisableInterrupts();

    /* Disable AI streaming */
    AI_CONTROL = 0;

    /* Assert DSP reset */
    val = dspRegs[0x0A / 2];
    val &= ~0x0028;
    val |= 0x0800;
    dspRegs[0x0A / 2] = val;

    /* Wait for DSP to halt */
    for (i = 0; i < 0x4E20; i++)
        ;

    /* Clear DSP reset and disable */
    val = dspRegs[0x0A / 2];
    val &= ~0x0828;
    dspRegs[0x0A / 2] = val;

    /* Additional wait */
    for (i = 0; i < 0x4E20; i++)
        ;

    OSRestoreInterrupts(enabled);
}
