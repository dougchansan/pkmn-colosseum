#ifndef GAME_MENU_PKJB_UPLOADER_SHARED_H
#define GAME_MENU_PKJB_UPLOADER_SHARED_H

/*
 * Shared part of the pkjb_uploader.c translation unit (0x800716C8 -
 * 0x80075390): the GBA link command layer in front of the PKJB multiboot
 * uploader. The unit's rodata is the "pkjb_uploader.c" string block at
 * 0x80268780 (PKJB_Prepare, pkjbPrepare); the command functions share its
 * statics (the idle callbacks at lbl_803B6E18 and cancel flags at
 * lbl_803B6E08, also used by pkjbValidateKey, and the boot key at lbl_8047A60C,
 * written by pkjbPrepare and checked by fn_80073C38), so they sit in the
 * same unit. It has no .data or .sdata2.
 *
 * Unit flags: GC/2.0, -O4,p, "-opt nopeephole" (see pkjb_exact_80073990.c
 * for the evidence). The unit is carved at function boundaries so its exact
 * functions can link; its data stays extern here.
 *
 * The static inline helpers below are expanded repeatedly in the target:
 * - pkjbRecvWait: the "wait for SEND, then GBARead" poll loop (status &
 *   0xA == 8, idle callback, cancel flag), in every command function;
 * - pkjbSendCommand: GBAWrite of a command word followed by pkjbRecvWait
 *   (result + 11), before every command reply check;
 * - pkjbSwap32: the byte swap of GBA words (fn_80071AE4, fn_80071EA4,
 *   fn_800730F8, fn_80073A44);
 * - pkjbQuery44: fn_80071700 and fn_800722A0 are byte-identical copies;
 * - pkjbWaitZeroResponse: fn_800719A8 and fn_80072548 are byte-identical;
 * - pkjbResetRetry / pkjbPingAA: the 5 ms reset retry followed by the 0xAA
 *   ping, expanded in fn_80072A00 and fn_800745B4 (and the reset retry
 *   again in fn_800733D0).
 */

#include "dolphin/types.h"
#include "dolphin/gba.h"
#include "dolphin/os/OSAlarm.h"
#include "dolphin/os/OSClock.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/OSTime.h"

/* SI device type reported for a GBA on the JOY Bus. */
#define PKJB_SI_GBA 0x40000

typedef struct PkjbIdleCallback {
    void (*func)(s32 chan, void* arg);
    void* arg;
} PkjbIdleCallback;

/* Alarm that resumes the waiting thread after 1 ms (fn_80072684). */
extern OSAlarm lbl_803B6DE0;
/*
 * Per-channel cancel flags. fn_800716E8 sets them from the GBA
 * communication code while the command functions poll them in their wait
 * loops, so they are volatile: every loop re-derives the flag address
 * instead of reusing the previous loop's, as MWCC does for volatile
 * objects (e.g. fn_80073C38's second wait loop reuses the idle-callback
 * slot pointer from the first loop but recomputes the flag address).
 */
extern volatile s32 lbl_803B6E08[4];
/* Per-channel idle callbacks, run while waiting on the link. */
extern PkjbIdleCallback lbl_803B6E18[4];
/* Thread suspended by pkjbWaitZeroResponse. */
extern OSThread* lbl_8047A600;
/* Boot key expected back from the GBA after a reset (set by pkjbPrepare). */
extern u32 lbl_8047A60C;

extern void _threadSwitch(void);
extern void gbaCommandSetKeyState(s32 mode, s32 flag);
extern u32 SIProbe(s32 chan);
extern OSThread* OSGetCurrentThread(void);

void fn_80072684(OSAlarm* alarm, OSContext* context);
s32 fn_80073C38(s32 chan);

static inline u32 pkjbSwap32(u32 value)
{
    return (value >> 24) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | (value << 24);
}

/* Wait until the GBA has sent a word (SEND set, RECV clear), then read it. */
static inline s32 pkjbRecvWait(s32 chan, u32* data, u8* length, u32 msec)
{
    u8 status;
    u32 timeout;
    u32 start;

    timeout = OSMillisecondsToTicks(msec);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 1;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            return 2;
        }
        if ((status & (GBA_JSTAT_SEND | GBA_JSTAT_RECV)) == GBA_JSTAT_SEND) {
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    if (GBARead(chan, (u8*)data, length) != 0) {
        return 3;
    }
    return 0;
}

/* Send one command word and read the GBA's reply word. */
static inline s32 pkjbSendCommand(s32 chan, u32 command, u32* response)
{
    u8 length;
    s32 result;

    if (GBAWrite(chan, (u8*)&command, &length) != 0) {
        return 11;
    }
    result = pkjbRecvWait(chan, response, &length, 100);
    if (result != 0) {
        return result + 11;
    }
    return 0;
}

/* Reset the link, retrying for up to 5 ms while the GBA is not answering. */
static inline s32 pkjbResetRetry(s32 chan)
{
    u32 timeout;
    u32 start;
    s32 expired;
    s32 result;

    timeout = OSMillisecondsToTicks(5);
    start = OSGetTick();
    do {
        expired = OSGetTick() - start > timeout;
        result = fn_80073C38(chan);
    } while (result == 1 && !expired);
    return result;
}

/* Reset the link and check that the GBA answers the 0xAA ping. */
static inline s32 pkjbPingAA(s32 chan)
{
    u32 response;
    s32 result;

    result = pkjbResetRetry(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0xAA, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0xAA) {
        return 15;
    }
    return 0;
}

/* Command 0x44: the GBA answers, then reports a zero word within 30 s. */
static inline s32 pkjbQuery44(s32 chan)
{
    u32 response;
    u8 length;
    s32 result;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x44, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x44) {
        return 15;
    }
    result = pkjbRecvWait(chan, &response, &length, 30000);
    if (result != 0) {
        return result + 15;
    }
    return response != 0 ? 19 : 0;
}

/* Sleep 1 ms on the alarm, then read one word that must be zero. */
static inline s32 pkjbWaitZeroResponse(s32 chan)
{
    u8 status;
    u32 response;
    BOOL enabled;
    s32 result;

    lbl_8047A600 = OSGetCurrentThread();
    OSCreateAlarm(&lbl_803B6DE0);
    enabled = OSDisableInterrupts();
    OSSetAlarm(&lbl_803B6DE0, OSMillisecondsToTicks(1), fn_80072684);
    OSSuspendThread(lbl_8047A600);
    OSRestoreInterrupts(enabled);

    if (SIProbe(chan) != PKJB_SI_GBA) {
        result = 1;
    } else if (GBAGetStatus(chan, &status) != 0) {
        result = 2;
    } else if ((status & GBA_JSTAT_SEND) == 0) {
        result = -1;
    } else if (GBARead(chan, (u8*)&response, &status) != 0) {
        result = 3;
    } else if (response != 0) {
        result = 4;
    } else {
        result = 0;
    }
    if (result == 0 || result >= 3) {
        gbaCommandSetKeyState(chan + 1, 1);
    }
    return result;
}

#endif /* GAME_MENU_PKJB_UPLOADER_SHARED_H */
