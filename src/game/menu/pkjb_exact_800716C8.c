/**
 * @file pkjb_exact_800716C8.c
 * @brief pkjb_uploader.c carve, 0x800716C8 - 0x80071AE4.
 *
 * Idle-callback and cancel-flag setters, the 0x44 query and the zero-word
 * wait. Unit flags and helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/* 0x800716C8 | 0x20 */
s32 gbaCommSetChannelBuffer(s32 chan, void* arg, void (*func)(s32 chan, void* arg))
{
    lbl_803B6E18[chan].func = func;
    lbl_803B6E18[chan].arg = arg;
    return 0;
}

/* 0x800716E8 | 0x18 */
s32 gbaCommSetChannelResult(s32 chan, s32 value)
{
    lbl_803B6E08[chan] = value;
    return 0;
}

/* 0x80071700 | 0x2A8 */
s32 gbaCmdHandshakeA(s32 chan)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = pkjbQuery44(chan);
    gbaCommandSetKeyState(key, 1);
    return result;
}

/* 0x800719A8 | 0x13C */
s32 gbaCmdWaitReady(s32 chan)
{
    return pkjbWaitZeroResponse(chan);
}
