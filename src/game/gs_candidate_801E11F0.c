/* GSvtr: disable, 0x801E11F0 - 0x801E1258 (XD GSvtrDisable). */
#include "dolphin/types.h"
#include "game/gs_vtr_gapp_block.h"

extern u8 lbl_8047B420;  /* XD _vtrEnabled */
extern u32 lbl_8047B424; /* XD _vtrState */

void vtrDeactivate(void)
{
    lbl_8047B424 = 0;
    lbl_8047B420 = 0;
    _vtrGappSetBlock(FALSE);
}
