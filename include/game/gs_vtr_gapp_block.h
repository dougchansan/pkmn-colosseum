#ifndef GAME_GS_VTR_GAPP_BLOCK_H
#define GAME_GS_VTR_GAPP_BLOCK_H
#include "dolphin/types.h"

/*
 * Blocks or unblocks every registered GSgapp handle. Retail expands the
 * same loop three times: twice in fn_801E0FB4 (XD _vtrUpdateFunc; block and
 * unblock) and once in vtrDeactivate (XD GSvtrDisable; unblock), the unblock
 * copies instruction for instruction. XD names the helper
 * _vtrGappSetBlock__Fb (0x80 bytes; dead-stripped from the NXXJ01 demo map
 * because every use is inlined), and XD's _vtrUpdateFunc has the same two
 * loop expansions. References: TeamOrre/xd-decomp config/GXXE01/
 * symbols.txt @ 4989794e, Colo-XD-PBR-symbol-maps NXXJ01.map @ 6b51d3af,
 * trevor403/xd-asm code/func_GSvtr_app.s @ b1087f18.
 */
static inline void _vtrGappSetBlock(BOOL block)
{
    extern u32 lbl_80467CF8[];
    extern u32 lbl_8047B42C;
    extern void GSgappBlock(u32 taskId);
    extern void GSgappUnblock(u32 taskId);
    u32 i;
    u32* hdl;

    for (i = 0, hdl = lbl_80467CF8; i < lbl_8047B42C; i++, hdl++) {
        if (*hdl != 0) {
            if (block) {
                GSgappBlock(*hdl);
            } else {
                GSgappUnblock(*hdl);
            }
        }
    }
}

#endif
