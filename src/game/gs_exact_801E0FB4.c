/* GSvtr: update, 0x801E0FB4 - 0x801E1170 (XD _vtrUpdateFunc__FUlUl,
 * 0x801E03BC, 0x1B8 bytes, scope local).
 *
 * This is the first function of the vtr unit, not the last of the etctool
 * unit before it: XD keeps _vtrUpdateFunc with the GSvtr functions
 * (0x801E1170 onward here), and XD's etctool unit ends with
 * etctoolSetPokemonNakigoe, as Colosseum's does at 0x801E0F78. The function
 * reads no .sdata2 pool constant, so it links as a data-free carve like its
 * GSvtr neighbours. */
#include "dolphin/types.h"
#include "game/gs_vtr_gapp_block.h"

void fn_801E0FB4(s32 flags, u32 setupCamera, u32 resetQueue)
{
    extern u32 lbl_80467CF8[];
    extern u8 lbl_8047B420;
    extern u32 lbl_8047B424;
    extern s32 lbl_8047B428;
    extern u32 lbl_8047B42C;
    extern u32 lbl_8047B430;
    extern u8 lbl_8047B43C;
    extern void GSgappBlock(u32 taskId);
    extern void GSgappUnblock(u32 taskId);
    extern void GSthreadExecuteGroup(u32 group);
    extern void GSgfxRenderLayer(s32 mode, s32 enabled);
    extern void GSgfxDrawScene(s32 flags, u32 setupCamera, u32 resetQueue);
    extern void GSgfxRenderEnd(void);
    s32 enabled;

    if (lbl_8047B420 == 0) {
        enabled = 1;
    } else {
        switch (lbl_8047B428) {
        case 0:
            enabled = 1;
            break;
        case 1:
            lbl_8047B428 = 2;
            _vtrGappSetBlock(TRUE);
            enabled = 1;
            break;
        case 2:
            enabled = 0;
            break;
        case 3:
            lbl_8047B428 = 0;
            _vtrGappSetBlock(FALSE);
            enabled = 0;
            break;
        }

        switch (lbl_8047B424) {
        case 2:
            /* No action in state 2. Retail's compare tree pivots on 3 with
             * no test below it, which MWCC builds from three labels with the
             * lowest folded into the default path; XD's _vtrUpdateFunc has
             * the same tree. */
            break;
        case 3:
            if (lbl_8047B428 != 2) {
                lbl_8047B428 = 1;
            }
            break;
        case 4:
            {
                /* RULE-EXCEPTION(title-path): temporary for the next block
                 * state; writing the if/else (or ?:) straight to the global
                 * makes MWCC emit a branchless select where retail (and XD)
                 * branch - see docs/RULE_EXCEPTIONS.md */
                s32 next = 2;

                if (lbl_8047B428 == 0) {
                    next = 1;
                }
                lbl_8047B428 = next;
            }
            lbl_8047B430++;
            if (lbl_8047B430 >= 5) {
                lbl_8047B428 = 3;
                lbl_8047B430 = 0;
            }
            break;
        }

        if (lbl_8047B428 == 2) {
            GSthreadExecuteGroup(0xE38F910B);
        }
    }
    if (lbl_8047B43C != 0) {
        enabled = 0;
    }
    GSgfxRenderLayer(0, enabled);
    GSgfxDrawScene(flags, setupCamera, resetQueue);
    GSgfxRenderEnd();
}
