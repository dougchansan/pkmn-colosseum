/**
 * @file pkjb_exact_80073034.c
 * @brief pkjb_uploader.c carve, 0x80073034 - 0x800733D0.
 *
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* 0x80073034 | 0xC4: like fn_80072C74, without touching the key state. */
s32 _AGB_EntryGetStatus__FlPUl(s32 chan, u32* response)
{
    u32 data;
    u32 command;
    u8 status;

    if (SIProbe(chan) != PKJB_SI_GBA) {
        return 1;
    }
    if (GBAGetStatus(chan, &status) != 0) {
        return 2;
    }
    if ((status & GBA_JSTAT_SEND) == 0) {
        command = 0x11;
        GBAWrite(chan, (u8*)&command, &status);
        return -1;
    }
    if (GBARead(chan, (u8*)&data, &status) != 0) {
        return 3;
    }
    *response = data;
    return 0;
}

/*
 * One attempt of command 0x77: send the command, then the byte-swapped
 * value, and wait until the GBA has taken it. Inline in the target: the
 * fn_80073C38 result is tested in r3 and only then copied to the caller's
 * result register.
 */
static inline s32 pkjbSend77(s32 chan, u32 value)
{
    u32 response;
    u32 command;
    u8 status;
    s32 result;
    u32 timeout;
    u32 start;

    result = fn_80073C38(chan);
    if (result != 0) {
        return result;
    }
    command = 0x77;
    result = pkjbSendCommand(chan, command, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x77) {
        return 15;
    }
    command = pkjbSwap32(value);
    GBAWrite(chan, (u8*)&command, &status);
    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 16;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            return 17;
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
    return 0;
}

/* 0x800730F8 | 0x2D8: command 0x77, retried for 100 ms while the GBA is absent. */
s32 fn_800730F8(s32 chan, u32 value)
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
        result = pkjbSend77(chan, value);
    } while (result == 1 && !expired);
    gbaCommandSetKeyState(key, result != 0 ? 1 : 3);
    return result;
}
