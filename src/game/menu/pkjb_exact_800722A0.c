/**
 * @file pkjb_exact_800722A0.c
 * @brief pkjb_uploader.c carve, 0x800722A0 - 0x80072A00.
 *
 * Second copies of the 0x44 query and the zero-word wait, the alarm handler
 * they sleep on, and the fixed-size (0x78-byte) 0x55 upload. Unit flags and
 * shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* 0x800722A0 | 0x2A8: byte-identical to fn_80071700. */
s32 fn_800722A0(s32 chan)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = pkjbQuery44(chan);
    gbaCommandSetKeyState(key, 1);
    return result;
}

/* 0x80072548 | 0x13C: byte-identical to fn_800719A8. */
s32 fn_80072548(s32 chan)
{
    return pkjbWaitZeroResponse(chan);
}

/* 0x80072684 | 0x24: alarm handler, wakes the thread suspended above. */
void fn_80072684(OSAlarm* alarm, OSContext* context)
{
    OSResumeThread(lbl_8047A600);
}

/*
 * Send words to the GBA, waiting for each to be taken. fn_800726A8 drops
 * the result: in the target every failure branch of this loop lands on
 * the caller's "return 0" block.
 */
static inline s32 pkjbSendWordsIdle(s32 chan, const u32* data, s32 size)
{
    u32 word;
    u8 status;
    u32 timeout;
    u32 start;
    s32 offset;

    for (offset = 0; offset < size; data++, offset += 4) {
        word = *data;
        if (GBAWrite(chan, (u8*)&word, &status) != 0) {
            return 16;
        }
        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return 17;
            }
            if (GBAGetStatus(chan, &status) != 0) {
                return 18;
            }
            if ((status & GBA_JSTAT_RECV) == 0) {
                break;
            }
            if (lbl_803B6E18[chan].func != NULL) {
                lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
            }
            if (lbl_803B6E08[chan] != 0) {
                return 1000;
            }
        }
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    return 0;
}

/*
 * One attempt of command 0x55: wait for a GBA on the port, reset, send the
 * command and upload 0x78 bytes. The fn_80073C38 result is returned through
 * the inline's return value (tested in r3, then copied to the home register).
 */
static inline s32 pkjbSend55(s32 chan, const u32* data)
{
    u32 response;
    s32 result;
    u32 timeout;
    u32 start;

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 1;
        }
        if (SIProbe(chan) == PKJB_SI_GBA) {
            break;
        }
        if (lbl_803B6E18[chan].func != NULL) {
            lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
        }
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x55, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x55) {
        return 15;
    }
    pkjbSendWordsIdle(chan, data, 0x78);
    return 0;
}

/* 0x800726A8 | 0x358 */
s32 fn_800726A8(s32 chan, const u32* data)
{
    s32 key;
    s32 result;
    u32 timeout;
    u32 start;
    s32 expired;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    do {
        expired = OSGetTick() - start > timeout;
        result = pkjbSend55(chan, data);
    } while (result == 1 && !expired);
    gbaCommandSetKeyState(key, result != 0 ? 1 : 3);
    return result;
}
