/**
 * @file pkjb_exact_80071E34.c
 * @brief Exact island 0x80071E34 - 0x80071EA4: a standalone copy of
 *        gbaCmdRecv33Locked from pkjb_candidate_80071AE4.c (body unchanged there).
 */
#include "game/menu/pkjb_uploader_shared.h"

s32 gbaCmdRecv33(s32 chan, u8* data);

/* 0x80071E34 | 0x70: gbaCmdRecv33 with the key state held. */
s32 gbaCmdRecv33Locked(s32 chan, u8* data)
{
    s32 key;
    s32 result;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    result = gbaCmdRecv33(chan, data);
    gbaCommandSetKeyState(key, 1);
    return result;
}
