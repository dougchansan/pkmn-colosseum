/**
 * @file menu_candidate_80075DC8.c
 * @brief Menu range carve, 0x80075DC8 - 0x80075EE0.
 *
 * Function-boundary carve of the menu range bucket (menu_range_8007109C.c,
 * chunk menu_candidate_80075390): plays a camera animation, waits out the
 * delay at lbl_8047C0C0, opens menu 0xE2 and links to floor 0x321 or 0x384
 * depending on its answer. Same flags as the chunk
 * (GC/1.3 -O4,p, -opt nopeephole, no pragmas). The data is extern.
 */
#include "dolphin/types.h"

extern void* std093_getCurrentGroupID(void);
extern void cameraPlayAnime(s32, u32, s32, s32);
extern s32 fn_800D37CC(void);
extern u32 GSgetFrameStep(void);
extern void _threadSwitch(void);
extern void std115_mapObjectPlayAnimation(u32 object, s32 arg1, s32 arg2, s32 arg3);
extern s32 menuOpenCustom(s32, ...);
extern s32 fadeCheck(s32);
extern void fadeSet(s32, f32);
extern void floorLink(s32, s32);
extern void floorSetFadeScript(s32, u32);
extern f32 lbl_8047C0C0;
extern f32 lbl_8047C0C4;

/* 0x80075DC8 | 0x118: open menu 0xE2, then link to the chosen floor. */
void fn_80075DC8(void)
{
    u32 elapsed;
    u32 waitTicks;
    s32 destination;

    cameraPlayAnime((s32)std093_getCurrentGroupID(), 0x0B561800, 0, 0);
    waitTicks = 1;
    if (fn_800D37CC() == 0x32) {
        waitTicks = (u32)lbl_8047C0C0;
        if (waitTicks < 1) {
            waitTicks = 1;
        }
    }
    elapsed = 0;
    while (elapsed < waitTicks) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    std115_mapObjectPlayAnimation(0x0B541000, 2, 0, 1);
    switch (menuOpenCustom(0xE2, 0, 0, 0x10, 1, 0)) {
    case 0:
        destination = 0x321;
        break;
    case -1:
    case 1:
    default:
        destination = 0x384;
        break;
    }
    fadeCheck(1);
    fadeSet(3, lbl_8047C0C4);
    floorLink(destination, 0);
    floorSetFadeScript(0, 0x05960008);
}
