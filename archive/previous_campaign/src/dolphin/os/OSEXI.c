#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/exi/EXI.h"

/*
 * OSEXI.c - OS-level EXI utility functions.
 *
 * Contains OS functions that use EXI for hardware access, including
 * font ROM access, RTC reading, and serial number retrieval. These
 * sit between OSInterrupt.c and OSMemory.c in the link order.
 *
 * Matches: 0x8009E7A4 - 0x8009F1B8
 *   fn_8009E7A8 (0x04) - __OSGetRTC stub / return
 *   fn_8009E7AC (0x04) - (nop)
 *   OSLink (0x2BC) - __OSReadROM (read from IPL font ROM via EXI)
 *   fn_8009EA6C (0x2E0) - __OSGetRTC / __OSSetRTC (RTC access via EXI)
 *   fn_8009ED4C (0x24)  - (helper)
 *   fn_8009ED70 (0x3C)  - (helper)
 *   OSUnlink (0x238) - __OSReadFontROM
 *   fn_8009EFE4 (0x1D4) - __OSGetSerialNumber
 */

extern void* memcpy(void* dest, const void* src, u32 n);
extern void DCInvalidateRange(void* addr, u32 size);

/*
 * fn_8009E7A8 - Stub / nop function.
 * 0x8009E7A8 | size: 0x04
 */
static void __OSRTCStub0(void) {
}

/*
 * fn_8009E7AC - Stub / nop function.
 * 0x8009E7AC | size: 0x04
 */
static void __OSRTCStub1(void) {
}

/*
 * __OSReadROM - Read data from the IPL ROM via EXI channel 0 device 1.
 * 0x8009E7B0 | size: 0x2BC
 *
 * The IPL ROM chip is accessed via EXI channel 0, device 1. This function
 * reads 'size' bytes from the ROM starting at the given offset.
 */
BOOL __OSReadROM(void* dest, s32 size, s32 offset) {
    BOOL result = FALSE;
    u32 cmd;

    DCInvalidateRange(dest, (u32)size);

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    /* Send read command */
    cmd = (u32)offset << 6;
    if (EXIImm(0, &cmd, 4, 1, NULL) && EXISync(0)) {
        if (EXIDma(0, dest, size, 0, NULL) && EXISync(0)) {
            result = TRUE;
        }
    }

    EXIDeselect(0);
    EXIUnlock(0);
    return result;
}

/*
 * __OSGetRTC - Read the Real-Time Clock value via EXI.
 * 0x8009EA6C | size: 0x2E0
 *
 * Reads the 32-bit RTC counter from the IPL ROM chip via EXI.
 */
BOOL __OSGetRTC(u32* rtc) {
    BOOL result = FALSE;
    u32 cmd;
    u32 val;

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    /* Read RTC command: address 0x20000000 */
    cmd = 0x20000000;
    if (EXIImm(0, &cmd, 4, 1, NULL) && EXISync(0)) {
        if (EXIImm(0, &val, 4, 0, NULL) && EXISync(0)) {
            *rtc = val;
            result = TRUE;
        }
    }

    EXIDeselect(0);
    EXIUnlock(0);
    return result;
}

/*
 * __OSSetRTC - Set the Real-Time Clock value via EXI.
 * Part of fn_8009EA6C block.
 */
BOOL __OSSetRTC(u32 rtc) {
    BOOL result = FALSE;
    u32 cmd;

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    /* Write RTC command: address 0xA0000000 */
    cmd = 0xA0000000;
    if (EXIImm(0, &cmd, 4, 1, NULL) && EXISync(0)) {
        if (EXIImm(0, &rtc, 4, 1, NULL) && EXISync(0)) {
            result = TRUE;
        }
    }

    EXIDeselect(0);
    EXIUnlock(0);
    return result;
}

/*
 * fn_8009ED4C - Helper: EXI lock check.
 * 0x8009ED4C | size: 0x24
 */
static BOOL __OSEXILockHelper(void) {
    return EXILock(0, 1, NULL);
}

/*
 * fn_8009ED70 - Helper: EXI unlock after ROM read.
 * 0x8009ED70 | size: 0x3C
 */
static void __OSEXIUnlockHelper(void) {
    EXIDeselect(0);
    EXIUnlock(0);
}

/*
 * __OSReadFontROM - Read font data from the IPL ROM.
 * 0x8009EDAC | size: 0x238
 *
 * Reads font ROM data which is stored in the IPL ROM starting at
 * a specific offset. Used by the OS font rendering system.
 */
BOOL __OSReadFontROM(void* dest, s32 offset, s32 size) {
    return __OSReadROM(dest, size, offset);
}

/*
 * __OSGetSerialNumber - Read the console serial number.
 * 0x8009EFE4 | size: 0x1D4
 *
 * Reads the serial number from SRAM/IPL ROM. The serial number is
 * derived from data stored in the extended SRAM area.
 */
BOOL __OSGetSerialNumber(u32* serial) {
    u32 buf[2];
    BOOL result;

    result = __OSReadROM(buf, 8, 0x14);
    if (result) {
        *serial = buf[0] ^ buf[1];
    }
    return result;
}
