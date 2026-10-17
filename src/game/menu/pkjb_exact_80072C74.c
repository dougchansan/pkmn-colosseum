/**
 * @file pkjb_exact_80072C74.c
 * @brief pkjb_uploader.c carve, 0x80072C74 - 0x80072D58.
 *
 * Unit flags and shared declarations: see game/menu/pkjb_uploader_shared.h.
 */
#include "game/menu/pkjb_uploader_shared.h"

/*
 * 0x80072C74 | 0xE4: poll one channel for a pending word. -1 (after
 * kicking the GBA with 0x11) while it has nothing to send; the key state is
 * released on any definite result.
 */
s32 fn_80072C74(s32 chan, u32* response)
{
    u32 data;
    u32 command;
    u8 status;
    s32 result;

    if (SIProbe(chan) != PKJB_SI_GBA) {
        result = 1;
    } else if (GBAGetStatus(chan, &status) != 0) {
        result = 2;
    } else if ((status & GBA_JSTAT_SEND) == 0) {
        command = 0x11;
        GBAWrite(chan, (u8*)&command, &status);
        result = -1;
    } else if (GBARead(chan, (u8*)&data, &status) != 0) {
        result = 3;
    } else {
        result = 0;
        *response = data;
    }
    if (result >= 0) {
        gbaCommandSetKeyState(chan + 1, 1);
    }
    return result;
}
