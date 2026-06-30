#include "dolphin/dvd/dvd.h"

/*
 * DVDError.c - DVD error code translation and storage.
 *
 * Translates DVD hardware error codes to numeric indices and stores
 * them in SRAM for persistence across resets.
 *
 * Matches: 0x800A7FE0 - 0x800A8178
 */

/* Error code lookup table - located at 0x80311C00 in rodata */
extern const u32 ErrorCodeTable[];

/* SRAM access functions */
extern void* __OSLockSramEx(void);
extern BOOL __OSUnlockSramEx(u32 offset);

/*
 * ErrorCode2Num - Convert a DVD error code to a numeric index
 * 0x800A7FE0 | size: 0x11C
 *
 * Looks up the error code in a table of known error values.
 * The table is organized in groups of ~9 entries per "round"
 * with 2 rounds (outer loop count = 2).
 *
 * Returns:
 *   0-0x10: index of matching error code
 *   0x11:   error code is in range [0x100000, 0x100008]
 *   0x1d:   unknown error code
 */
static u32 ErrorCode2Num(u32 error) {
    const u32* table = ErrorCodeTable;
    u32 idx = 0;
    s32 round;

    for (round = 0; round < 2; round++) {
        /* Check entry 0 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 1 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 2 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 3 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 4 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 5 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 6 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 7 */
        if (error == table[0]) return (u8)idx;
        idx++; table++;

        /* Check entry 8 (last in this round, uses bdnz) */
        if (error == table[0]) return (u8)idx;
        idx++; table++;
    }

    /* Check for error codes in range 0x00100000 - 0x00100008 */
    if (error >= 0x00100000 && error <= 0x00100008) {
        return 0x11;
    }

    /* Unknown error */
    return 0x1D;
}

/*
 * __DVDStoreErrorCode - Translate and store a DVD error code in SRAM
 * 0x800A80FC | size: 0x7C
 *
 * Special error codes:
 *   0x01234567 -> stores 0xFF
 *   0x01234568 -> stores 0xFE
 *
 * Otherwise, extracts the severity from bits 24-31 (capped at 6),
 * translates the lower 24 bits via ErrorCode2Num,
 * and stores (severity * 30 + errorNum) in SRAM at offset 0x24.
 */
void __DVDStoreErrorCode(u32 error) {
    u32 code;
    u32 severity;
    u32 errNum;
    void* sram;

    if (error == 0x01234567) {
        code = 0xFF;
    } else if (error == 0x01234568) {
        code = 0xFE;
    } else {
        severity = error >> 24;
        errNum = ErrorCode2Num(error & 0x00FFFFFF);

        if (severity >= 6) {
            severity = 6;
        }

        code = severity * 30 + (u8)errNum;
    }

    sram = __OSLockSramEx();
    ((u8*)sram)[0x24] = (u8)code;
    __OSUnlockSramEx(1);
}
