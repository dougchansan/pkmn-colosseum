#include "dolphin/types.h"

extern void GSmsgPrintColor(u32 a, u32 b, s32 c, u32 d);
extern u32 lbl_8047A350;
extern u32 lbl_80478880;

/** Check the title-screen autodemo timer. */
void fn_800205C8(u8* obj) {
    s32 mask = -0x100;
    u32 timer = obj[0x8B];

    GSmsgPrintColor(0, 0, timer | mask, (&lbl_80478880)[lbl_8047A350]);
}
