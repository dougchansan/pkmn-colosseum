/**
 * @file gs_range_801653CC_candidate_80165FDC.c
 * @brief Candidate gs-engine range, 0x80165FDC - 0x8016604C.
 */
#include "game/gs_range_801653CC_shared.h"

#pragma peephole off
void fn_80165FDC(u32 id)
{
    s32 status;

    while (1) {
        status = fn_8017B2CC(id);
        if (status < 0) {
            GSlogWrite(lbl_802736CC, id);
        }
        if (status == 0) {
            break;
        }
        _threadSwitch();
    }
}
#pragma peephole reset
