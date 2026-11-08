/**
 * @file pkjb_candidate_800733D0.c
 * @brief pkjb_uploader.c carve, 0x800733D0 - 0x80073690.
 *
 * gbaCmdUpload88 is exact, but only with the send loop in its own inline
 * (pkjbSendWords): retail allocates the loop's word/status slots (12, 9)
 * below pkjbSendCommand's (16, 10), which MWCC does only for locals of a
 * separate inline body. The loop expands once and shows no other inline
 * fingerprint, so under the strict policy it is a rule exception: linked
 * under the byte-match-first decision with RULE-EXCEPTION(user-approved).
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 * The RULE-EXCEPTION forms above are listed in docs/RULE_EXCEPTIONS.md.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* RULE-EXCEPTION(user-approved): single-use inline helper - see docs/RULE_EXCEPTIONS.md */
static inline s32 pkjbSendWords(s32 chan, const u32* data, s32 size)
{
    u32 word;
    u8 status;
    s32 offset;
    u32 timeout;
    u32 start;

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
        }
        if ((offset % 64) == 0) {
            _threadSwitch();
        }
    }
    return 0;
}

static inline s32 pkjbSend88(s32 chan, const u32* data)
{
    u32 response;
    s32 result;

    result = pkjbResetRetry(chan);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0x88, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0x88) {
        return 15;
    }
    return pkjbSendWords(chan, data, 0x780);
}

/* 0x800733D0 | 0x2C0: command 0x88, upload 0x780 bytes. */
s32 gbaCmdUpload88(s32 chan, const u32* data)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = pkjbSend88(chan, data);
    gbaCommandSetKeyState(key, 1);
    return result;
}
