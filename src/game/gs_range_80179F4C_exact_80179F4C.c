/**
 * .text 0x80179F4C - 0x80179FA4: fn_80179F4C, the first function after
 * camera.c (which ends at 0x80179F4C with _cameraRestoreStateData).
 *
 * Stores its argument in the scene state word lbl_80478C4C and opens custom
 * menu 0xFE unless it is already open. Optimisation-level-0 code like the
 * fsys functions after it: the parameter is homed on the stack (stw r3,0x8)
 * and reloaded for the store. Exact (22/22 instructions and relocations)
 * with the unit-wide `-opt level=0` and no local pragmas; carved from
 * gs_range_80179F4C.c so it can link while fsysInitTOC stays a candidate.
 * The (u8) cast is menuIsCheck's byte result (clrlwi. r0,r3,24), written the
 * way menuFight.c tests it.
 */

#include "game/gs_scene_types.h"

void fn_80179F4C(u32 param)
{
    lbl_80478C4C = param;
    if ((u8)menuIsCheck(0xFE) == 0) {
        menuOpenCustom(0xFE, 0, 0, 0, 0, 0);
    }
}
