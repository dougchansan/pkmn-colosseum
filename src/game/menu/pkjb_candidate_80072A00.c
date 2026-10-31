/**
 * @file pkjb_candidate_80072A00.c
 * @brief pkjb_uploader.c carve, 0x80072A00 - 0x80072C74.
 *
 * gbaCmdPing60: command 0x60, then ping with 0xAA until the GBA answers
 * (3 s), with the key state held.
 *
 * The earlier pkjbWait60 inline form was 99.7%: the key channel and the
 * result swapped callee-saved registers, because the inline's GBAWrite
 * result became a frontend split temp numbered above key (lane D9/D16
 * replays). Retail keeps the result in the caller: the first reset call
 * has its own temporary, and the 3 s ping loop writes the timeout code
 * through a pointer to the caller's result. With result a caller local
 * declared after key, key is coloured first (r29, result r28), and the
 * loop's locals keep their inline numbering (lane D28).
 * Unit flags and shared helpers: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/*
 * Ping with 0xAA until the GBA answers; after 3 s store 16 in *result.
 * RULE-EXCEPTION(title-path): single-use inline helper with register-only
 * evidence -- see docs/RULE_EXCEPTIONS.md
 */
static inline void pkjbWaitPing(s32 chan, s32* result)
{
    u32 timeout;
    u32 start;
    s32 ping;

    timeout = OS_TIMER_CLOCK * 3;
    start = OSGetTick();
    do {
        if (OSGetTick() - start > timeout) {
            *result = 16;
            return;
        }
        ping = pkjbPingAA(chan);
        _threadSwitch();
    } while (ping != 0);
}

/* 0x80072A00 | 0x274 */
s32 gbaCmdPing60(s32 chan)
{
    s32 key;
    s32 result;
    s32 first;
    u32 command;
    u8 length;

    key = chan + 1;
    gbaCommandSetKeyState(key, 2);
    first = gbaCmdResetLink(chan);
    if (first != 0) {
        result = first;
    } else {
        command = 0x60;
        result = GBAWrite(chan, (u8*)&command, &length);
        if (result != 0) {
            result = 11;
        } else {
            pkjbWaitPing(chan, &result);
        }
    }
    gbaCommandSetKeyState(key, 1);
    return result;
}
