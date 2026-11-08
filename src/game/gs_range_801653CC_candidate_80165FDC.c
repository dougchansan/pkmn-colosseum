/**
 * @file gs_range_801653CC_candidate_80165FDC.c
 * @brief soundWaitGroup, 0x80165FDC - 0x8016604C: poll an fsys file group
 *        until it has loaded.
 *
 * Last function of the sound TU that starts at 0x801653CC: its log string
 * (lbl_802736CC) closes that TU's 4-byte-aligned .rodata pool, and the
 * 6 bytes of padding after it (0x802736EA -> 0x802736F0) are the 8-byte
 * section alignment of the next TU. Built with the TU's flags; no pragmas.
 *
 * The group is a signed id (the log prints it with %d) while fsys takes an
 * unsigned handle (GSresGroupGetState(u32)); retail's register assignment for the
 * hoisted string address (lis r3 after the id copy) is the one MWCC emits
 * for that signed-to-unsigned argument conversion.
 */
#include "game/gs_range_801653CC_shared.h"

void soundWaitGroup(s32 group)
{
    s32 status;

    while (1) {
        status = GSresGroupGetState(group);
        if (status < 0) {
            GSlogWrite(lbl_802736CC, group);
        }
        if (status == 0) {
            break;
        }
        _threadSwitch();
    }
}
