#include "dolphin/vi/VI.h"
#include "dolphin/os/OSInterrupt.h"

/*
 * VI.c - Video Interface functions.
 *
 * Provides VIGetTvFormat which returns the current TV standard
 * (NTSC, PAL, MPAL, etc.) based on the VI hardware configuration.
 *
 * Matches: 0x800AA430 | size: 0x68
 */

/* Current TV mode - cached from VI hardware configuration */
/* SDA-relative variable at 0x8047A88C */
extern u32 CurrTvMode;

/* Jump table for the switch in VIGetTvFormat - located at 0x8031249C */
/* The switch maps CurrTvMode values 0-6 to output format codes */

/*
 * VIGetTvFormat - Get the current TV video format
 * 0x800AA430 | size: 0x68
 *
 * Returns:
 *   0 = NTSC
 *   1 = PAL
 *   2 = MPAL
 *   3 = DEBUG
 *   4 = DEBUG_PAL
 *   5 = EUR60
 *
 * The function uses a switch table on CurrTvMode (0-6):
 *   CurrTvMode 0,1 -> return 0 (NTSC)
 *   CurrTvMode 2,3 -> return 1 (PAL)
 *   CurrTvMode 4   -> return 2 (MPAL)
 *   CurrTvMode 5   -> return CurrTvMode (5 = EUR60)
 *   CurrTvMode 6   -> return CurrTvMode (6)
 *   default        -> falls through to return CurrTvMode
 */
u32 VIGetTvFormat(void) {
    BOOL enabled;
    u32 format;

    enabled = OSDisableInterrupts();

    switch (CurrTvMode) {
    case 0:
    case 1:
        format = VI_NTSC;
        break;
    case 2:
    case 3:
        format = VI_PAL;
        break;
    case 4:
    case 5:
    case 6:
    default:
        format = CurrTvMode;
        break;
    }

    OSRestoreInterrupts(enabled);
    return format;
}
