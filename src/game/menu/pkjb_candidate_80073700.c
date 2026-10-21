/**
 * @file pkjb_candidate_80073700.c
 * @brief pkjb_uploader.c carve, 0x80073700 - 0x80073990 (candidate).
 *
 * gbaCmdRecv99 is 97.3%: the code is right, but retail walks the output
 * through a copy of the data pointer (made in the loop preheader, after the
 * offset is zeroed) and keeps the parameter in r30, so chan and the hoisted
 * values land one register off. Neither an explicit pointer nor an
 * offset-indexed store reproduces that copy here.
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* 0x80073700 | 0x290: command 0x99, receive 0x278 bytes. */
s32 gbaCmdRecv99(s32 chan, u32* data)
{
    u32 response;
    u8 status;
    s32 result;
    s32 offset;
    u32 timeout;
    u32 start;

    result = gbaCmdResetLink(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x99, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x99) {
        return 15;
    }
    for (offset = 0; offset < 0x278; offset += 4, data++) {
        start = OSGetTick();
        timeout = OSMillisecondsToTicks(100);
        start = OSGetTick();
        for (;;) {
            if (OSGetTick() - start > timeout) {
                return 16;
            }
            if (GBAGetStatus(chan, &status) != 0) {
                return 17;
            }
            if ((status & GBA_JSTAT_SEND) != 0) {
                break;
            }
            if (lbl_803B6E18[chan].func != NULL) {
                lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
            }
            if (lbl_803B6E08[chan] != 0) {
                return 1000;
            }
        }
        if (GBARead(chan, (u8*)&response, &status) != 0) {
            return 18;
        }
        *data = response;
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    return 0;
}
