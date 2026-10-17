/**
 * @file menu_candidate_80074324.c
 * @brief pkjb_uploader.c carve, 0x80074324 - 0x8007480C.
 *
 * The link-reset entry points of the uploader: fn_80074324 drops the key
 * state and resets the link, pkjbProbe resets the GBA and waits for the
 * "AXVE" (Pokemon Ruby) boot key, and fn_800745B4 retries the 0xAA ping
 * for up to 2 s once pkjbValidateKey has booted the GBA.
 *
 * Same unit flags as the rest of pkjb_uploader.c (GC/2.0, -O4,p,
 * "-opt nopeephole"; see pkjb_exact_80073990.c) and the same shared
 * helpers and statics (include/game/menu/pkjb_uploader_shared.h).
 */
#include "game/menu/pkjb_uploader_shared.h"

s32 pkjbProbe(s32 chan);
s32 pkjbValidateKey(s32 chan);

/* 0x80074324 | 0x3C: drop the key state, then reset the link. */
s32 fn_80074324(s32 chan)
{
    gbaCommandSetKeyState(chan + 1, 0);
    return pkjbProbe(chan);
}

/* 0x80074360 | 0x254: reset the GBA and exchange the "AXVE" boot key. */
s32 pkjbProbe(s32 chan)
{
    u32 response;
    u32 timeout;
    u32 start;
    u8 status;
    s32 result;

    if (SIProbe(chan) != PKJB_SI_GBA) {
        return 1;
    }
    if (GBAReset(chan, &status) != 0) {
        return 2;
    }

    timeout = OSMillisecondsToTicks(100);
    start = OSGetTick();
    for (;;) {
        if (OSGetTick() - start > timeout) {
            return 3;
        }
        if (GBAGetStatus(chan, &status) != 0) {
            return 4;
        }
        if (status == (GBA_JSTAT_PSF0 | GBA_JSTAT_SEND)) {
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
        return 5;
    }
    if (response != 0x41585645) {
        return 6;
    }
    if (GBAGetStatus(chan, &status) != 0 && status != GBA_JSTAT_PSF0) {
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
        if ((status & GBA_JSTAT_FLAGS_MASK) != GBA_JSTAT_PSF0) {
            return 11;
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
    result = 0;
    return result;
}

/* 0x800745B4 | 0x258: boot the GBA, then retry the 0xAA ping for 2 s. */
s32 fn_800745B4(s32 chan)
{
    s32 result;
    u32 timeout;
    u32 start;

    result = pkjbValidateKey(chan);
    if (result == 0) {
        timeout = OS_TIMER_CLOCK * 8;
        start = OSGetTick();
        do {
            if (OSGetTick() - start > timeout) {
                return 22;
            }
            if (lbl_803B6E18[chan].func != NULL) {
                lbl_803B6E18[chan].func(chan, lbl_803B6E18[chan].arg);
            }
            if (lbl_803B6E08[chan] != 0) {
                return 1000;
            }
        } while (pkjbPingAA(chan) != 0);

        gbaCommandSetKeyState(chan + 1, 1);
    }
    return result;
}
