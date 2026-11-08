/**
 * @file pkjb_candidate_80073700.c
 * @brief pkjb_uploader.c carve, 0x80073700 - 0x80073990.
 *
 * The receive buffer is a byte buffer (the caller, gbaCommunication.c,
 * passes a u8[0x278]) filled one word at a time at a byte offset. Retail
 * strength-reduces that store into a pointer walk, whose start is the
 * copy of the buffer pointer made in the loop preheader.
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* 0x80073700 | 0x290: command 0x99, receive 0x278 bytes. */
s32 gbaCmdRecv99(s32 chan, u8* data)
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
    for (offset = 0; offset < 0x278; offset += 4) {
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
        *(u32*)(data + offset) = response;
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    return 0;
}
