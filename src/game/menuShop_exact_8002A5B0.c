/** Exact menuShop display callback, 0x8002A5B0 - 0x8002A618.
 * Built with the menuShop unit flags in configure.py (-schedule on,
 * -opt nopeephole).
 */
#include "dolphin/types.h"

extern u8 lbl_80266E58[];

s32 shopSubMenuDrawItem2(void* r3, u8* r4) {
    s16 val;
    u8 idx;
    u8* entry;
    idx = ((u8*)r3)[0x95];
    if ((s8)idx < 0 || (s8)idx >= 2) { return 0; }
    val = *(s16*)(r4 + 0x6);
    entry = lbl_80266E58;
    entry += (s8)idx * 0xc;
    if (*(s32*)(entry + 4) == val || *(s32*)(entry + 8) == val) {
        r4[0x67] = 0xff;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
