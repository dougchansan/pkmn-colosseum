/** Exact save/card-state accessors, 0x801E1170 - 0x801E11F0. */
#include "dolphin/types.h"

void vtrSetMode4(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;
    extern u32 lbl_8047B430;

    lbl_8047B424 = 4;
    lbl_8047B428 = 3;
    lbl_8047B430 = 0;
}

void vtrSetMode3(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B424 = 3;
    lbl_8047B428 = 3;
}

void vtrSetMode2(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B424 = 2;
    lbl_8047B428 = 3;
}

void vtrSetMode1(void)
{
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B424 = 1;
    if (lbl_8047B428 == 2) {
        return;
    }
    lbl_8047B428 = 1;
}

u8 vtrGetDisp(void)
{
    extern u8 lbl_8047B434;

    return lbl_8047B434;
}

void vtrSetDisp(u8 index, u8 active)
{
    extern u8 lbl_8047B434;
    extern u8 lbl_8047B435;

    lbl_8047B434 = index;
    lbl_8047B435 = active;
}

s32 vtrGetMode(void)
{
    extern s32 lbl_8047B424;

    return lbl_8047B424;
}

u8 vtrIsActive(void)
{
    extern u8 lbl_8047B420;

    return lbl_8047B420;
}
