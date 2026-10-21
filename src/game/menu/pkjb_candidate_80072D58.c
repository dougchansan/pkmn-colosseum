/**
 * @file pkjb_candidate_80072D58.c
 * @brief pkjb_uploader.c carve, 0x80072D58 - 0x80073034.
 *
 * gbaCmdUpload66 is exact, but only with the send loop in its own inline
 * (pkjbSendBlock): retail allocates the loop's word/status slots (12, 9)
 * below pkjbSendCommand's (16, 10), which MWCC does only for locals of a
 * separate inline body. The loop expands once and shows no other inline
 * fingerprint, so under the strict policy it is a rule exception: linked
 * under the byte-match-first decision with RULE-EXCEPTION(user-approved).
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* RULE-EXCEPTION(user-approved): single-use inline helper - see docs/RULE_EXCEPTIONS.md */
static inline s32 pkjbSendBlock(s32 chan, const u32* data, s32 size)
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
        }
        if ((offset % 64) == 0) {
            _threadSwitch();
        }
    }
    return 0;
}

static inline s32 pkjbSend66(s32 chan, const u32* data, s32 size)
{
    u32 response;
    s32 result;

    result = gbaCmdResetLink(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x66, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x66) {
        return 15;
    }
    return pkjbSendBlock(chan, data, size);
}

/* 0x80072D58 | 0x2DC: command 0x66, upload size bytes. */
s32 gbaCmdUpload66(s32 chan, const u32* data, s32 size)
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
        result = pkjbSend66(chan, data, size);
    } while (result == 1 && !expired);
    gbaCommandSetKeyState(key, result != 0 ? 1 : 3);
    return result;
}
