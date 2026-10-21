/**
 * @file pkjb_candidate_80071AE4.c
 * @brief pkjb_uploader.c carve, 0x80071AE4 - 0x800722A0 (candidate).
 *
 * gbaCmdRecv33Locked is exact; gbaCmdUpload22 and gbaCmdRecv33 are not yet:
 * - gbaCmdUpload22: retail keeps the upload size (count * 4 + 16, + 0x70 with
 *   the extra block) inside the retry loop and computes the header swap
 *   after the hoisted idle-callback addresses; this source hoists both.
 * - gbaCmdRecv33: the two header/body receive loops are right; only the
 *   placement of the first loop's pointer copy and the swap scheduling
 *   differ.
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

s32 gbaCmdRecv33(s32 chan, u32* data);

static inline s32 pkjbSendBlockIdle(s32 chan, const u32* data, s32 size)
{
    u32 word;
    u8 status;
    u32 timeout;
    u32 start;
    s32 offset;

    for (offset = 0; offset < size; offset += 4) {
        word = *(const u32*)((u32)data + offset);
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

static inline s32 pkjbSend22(s32 chan, const u32* data, u16 count, u16 extra)
{
    u32 response;
    s32 result;
    s32 size;

    result = gbaCmdResetLink(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x22, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x22) {
        return 15;
    }
    size = count;
    size = size * 4 + 16;
    if (extra != 0) {
        size += 0x70;
    }
    result = pkjbSendBlockIdle(chan, data, size);
    if (result != 0) {
        return result;
    }
    return 0;
}

/* 0x80071AE4 | 0x350: command 0x22, upload a block sized by its own header. */
s32 gbaCmdUpload22(s32 chan, const u32* data)
{
    s32 key;
    s32 result;
    u32 timeout;
    u32 start;
    s32 expired;
    u32 packed;
    u16 extra;
    u16 count;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    packed = pkjbSwap32(data[3]);
    extra = packed >> 16;
    count = packed;
    do {
        expired = OSGetTick() - start > timeout;
        result = pkjbSend22(chan, data, count, extra);
    } while (result == 1 && !expired);
    gbaCommandSetKeyState(key, result != 0 ? 1 : 3);
    return result;
}

/* 0x80071E34 | 0x70: gbaCmdRecv33 with the key state held. */
s32 gbaCmdRecv33Locked(s32 chan, u32* data)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = gbaCmdRecv33(chan, data);
    gbaCommandSetKeyState(key, 1);
    return result;
}

static inline s32 pkjbRecvBlock(s32 chan, u32* data, s32 size)
{
    u32 word;
    u8 length;
    s32 offset;
    s32 result;

    for (offset = 0; offset < size; offset += 4) {
        result = pkjbRecvWait(chan, &word, &length, 100);
        if (result != 0) {
            return result;
        }
        *(u32*)((u8*)data + offset) = word;
        if (lbl_803B6E08[chan] != 0) {
            return 1000;
        }
    }
    return 0;
}

/* 0x80071EA4 | 0x3FC: command 0x33, receive a 16-byte header and its body. */
s32 gbaCmdRecv33(s32 chan, u32* data)
{
    u32 response;
    s32 result;
    s32 size;

    result = gbaCmdResetLink(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x33, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x33) {
        return 15;
    }
    result = pkjbRecvBlock(chan, data, 16);
    if (result != 0) {
        return result + 15;
    }
    size = (pkjbSwap32(data[3]) & 0xFFFF) * 4;
    result = pkjbRecvBlock(chan, data + 4, size);
    if (result != 0) {
        return result + 18;
    }
    return 0;
}
