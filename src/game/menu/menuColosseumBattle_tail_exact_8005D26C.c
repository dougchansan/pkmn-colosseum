/**
 * @file menuColosseumBattle_tail_exact_8005D26C.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x8005D26C - 0x8005D2E8.
 */
#include "dolphin/types.h"

extern s32 menuOpen(s32, s32);
extern void menuClose();
extern s32 menuCloseSync(s32, s32);

extern const u32 lbl_8047BF30;
extern const u32 lbl_8047BF34;

/* Address: 0x8005D26C | Size: 0x7C */
#pragma push
#pragma peephole off
s32 fn_8005D26C(void) {
    s32 menuResult;
    s32 results[2];

    results[0] = lbl_8047BF30;
    results[1] = lbl_8047BF34;
    menuResult = menuOpen(0x9E, 1);
    menuClose(0x9E);
    menuCloseSync(0x9E, 1);
    if (menuResult < -1 || menuResult >= 2) {
        return 1;
    }
    return results[menuResult];
}
#pragma pop
