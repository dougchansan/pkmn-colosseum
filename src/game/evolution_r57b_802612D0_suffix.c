#include "dolphin/types.h"

extern u32 lbl_804787E0[];
extern u32 evolutionStart(u32, u32, u32, u16*, u32, u8*);
extern void fn_800FF660(void);
extern void fn_800FF730(u32);
extern void floorSetFadeScript(u32, u32);
extern void _threadSwitch(void);

void evolution(void)
{
    u32* base = lbl_804787E0;

    base[6] =
        evolutionStart(base[0], base[1], base[2], (u16*) base[4], base[3],
                       (u8*) base[5]);
    fn_800FF660();
    floorSetFadeScript(0, 0);
}

u32 evolutionOpen(u32 r3, u32 r4, u32 r5, u16* r6, u32 r7, u8* r8)
{
    u32* base;

    base = lbl_804787E0;
    base[0] = r3;
    base[1] = r4;
    base[2] = r5;
    base[3] = r7;
    base[4] = (u32) r6;
    base[5] = (u32) r8;
    fn_800FF730(0x386);
    floorSetFadeScript(0, 0);
    _threadSwitch();
    return base[6];
}
