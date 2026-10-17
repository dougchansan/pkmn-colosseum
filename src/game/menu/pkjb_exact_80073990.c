/**
 * @file pkjb_exact_80073990.c
 * @brief pkjb_uploader.c carve, 0x80073990 - 0x80073E8C.
 *
 * The 0x11 keep-alive, the 0xAA key read, the link reset/handshake every
 * command starts with, and a constant accessor.
 *
 * Unit flags (GC/2.0, -O4,p, "-opt nopeephole"), set for every unit of
 * pkjb_uploader.c (0x800716C8 - 0x80075390) in configure.py. Evidence:
 * - with the peephole pass off, every function of the command layer
 *   (0x800716C8 - 0x80073E8C) that is exact is exact with no local pragma:
 *   fn_800716C8, fn_800716E8, fn_80071700, fn_800719A8, fn_80071E34,
 *   fn_800722A0, fn_80072548, fn_80072684, fn_800726A8, fn_80072C74,
 *   fn_80072D58, _AGB_EntryGetStatus, fn_800730F8, fn_800733D0,
 *   fn_80073690, fn_80073990, fn_80073A44, fn_80073C38, fn_80073E84; with
 *   it on, fn_80073990/fn_80073A44/fn_80072C74/_AGB_EntryGetStatus and the
 *   others that used to need "#pragma peephole off" all lose;
 * - the uploader part moves toward the target too (old sources):
 *   pkjbPrepare 79.0 -> 82.4, fn_80074324 exact without its pragma,
 *   pkjbProbe 96.0 -> 96.7, fn_800745B4 79.1 -> 80.8, pkjbValidateKey 70.2
 *   (GC/1.2.5n) -> 83.8;
 * - version: GC/1.3 is out (pkjbPrepare ORs its boot key constant with
 *   oris/ori like the target only from GC/1.3.2 on; 1.3 emits lis/addi/or);
 *   GC/1.3.2 through GC/2.7 give identical output for the whole unit, and
 *   GC/2.0 matches the game's other GBA unit (gba_misc).
 * No function needs "-inline deferred" (it would inline fn_80073C38).
 */
#include "game/menu/pkjb_uploader_shared.h"

/* 0x80073990 | 0xB4: wait 1 ms, then send the 0x11 keep-alive. */
s32 fn_80073990(s32 chan)
{
    u32 command;
    u8 status;
    u32 delay;
    u32 start;

    delay = OSMillisecondsToTicks(1);
    start = OSGetTick();
    while (OSGetTick() - start < delay) {
    }
    if (SIProbe(chan) != PKJB_SI_GBA) {
        return 1;
    }
    command = 0x11;
    if (GBAWrite(chan, (u8*)&command, &status) != 0) {
        return 2;
    }
    return 0;
}

/* 0x80073A44 | 0x1F4: read the GBA's key state with the 0xAA command. */
s32 fn_80073A44(s32 chan, u16* buttons)
{
    u32 timeout;
    u32 start;
    u32 response;
    s32 result;
    s32 expired;

    timeout = OSMillisecondsToTicks(5);
    start = OSGetTick();
    do {
        expired = OSGetTick() - start > timeout;
        result = fn_80073C38(chan);
    } while (result == 1 && !expired);
    if (result != 0) {
        return result;
    }
    result = pkjbSendCommand(chan, 0xAA, &response);
    if (result != 0) {
        return result;
    }
    if ((response >> 24) != 0xAA) {
        return 15;
    }
    *buttons = pkjbSwap32(response) >> 16;
    return 0;
}

/*
 * 0x80073C38 | 0x24C: reset the link, check the GBA answers with the boot
 * key, echo it back and wait until the GBA has taken it.
 */
s32 fn_80073C38(s32 chan)
{
    u32 response;
    u8 status;
    s32 result;
    u32 timeout;
    u32 start;

    if (SIProbe(chan) != PKJB_SI_GBA) {
        return 1;
    }
    if (GBAReset(chan, &status) != 0) {
        return 2;
    }
    result = pkjbRecvWait(chan, &response, &status, 5);
    if (result != 0) {
        return result + 2;
    }
    if (response != lbl_8047A60C) {
        return 6;
    }
    if (GBAGetStatus(chan, &status) != 0) {
        return 7;
    }
    if (GBAWrite(chan, (u8*)&response, &status) != 0) {
        return 8;
    }
    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 9;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            return 10;
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

/* 0x80073E84 | 0x8 */
s32 fn_80073E84(void)
{
    return 1;
}
