/**
 * @file menuFight_exact_8000DAA8.c
 * @brief dbgGetFlag8047A2A0 / menuFightOpenAndWake, 0x8000DAA8 - 0x8000DAE8.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c): no jump
 * table, no pooled constant; the data is the .sbss fight-menu state
 * (lbl_8047A2A0-lbl_8047A2B0), kept extern. GC/1.3 -O4,p with the TU's
 * unit-wide -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"

extern u8 lbl_8047A2A0;
extern u32 lbl_8047A2A4;
extern u32 lbl_8047A2A8;
extern u32 lbl_8047A2B0;

extern u32 menuOpen(u32, u32);
extern void GSthreadUnblockGroup(u32);

u8 dbgGetFlag8047A2A0(void) {
    return lbl_8047A2A0;
}

/* Open the fight menu and wake the thread group waiting on it. */
u32 menuFightOpenAndWake(void) {
    lbl_8047A2B0 = menuOpen(lbl_8047A2A4, 1);
    GSthreadUnblockGroup(lbl_8047A2A8);
    return lbl_8047A2B0;
}
