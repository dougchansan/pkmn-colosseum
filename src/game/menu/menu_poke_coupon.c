/**
 * @file menu_poke_coupon.c
 * @brief Poke Coupon menu, 0x8007C260 - 0x8007C300.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

extern s32 lbl_80478940;
extern void __assert(const char* file, u32 line, const char* msg);
extern void menuShopOpen(u32 flag);
extern void floorRequestChange(u32 id);
extern const char lbl_80268B38[];
extern const char lbl_80268B4C[];

/* Retail retains this otherwise-unreferenced menu entry-point group. */
#pragma force_active on

void cardeMatrixResetSelection(void) {
    lbl_80478940 = -1;
}

void menuPokeCouponMain(void) {
    switch (lbl_80478940) {
    case 0:
        menuShopOpen(0xB);
        break;
    case 1:
        menuShopOpen(0xC);
        break;
    }

    floorRequestChange(0x395);
}

void menuPokeCouponInit(void) {
    if (lbl_80478940 == -1) {
        __assert((const char*)lbl_80268B38, 0x3B, (const char*)lbl_80268B4C);
    }
}
