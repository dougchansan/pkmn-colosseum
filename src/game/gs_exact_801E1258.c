/** Exact save/card load and registration helpers, 0x801E1258 - 0x801E1300. */
#include "dolphin/types.h"
#include "game/gs_scene_types.h"

void vtrActivate(void)
{
    extern u8 lbl_8047B420;
    extern s32 lbl_8047B424;
    extern s32 lbl_8047B428;

    lbl_8047B420 = 1;
    lbl_8047B424 = 2;
    lbl_8047B428 = 3;
}

void GSvtrLoadTexture(void)
{
    extern void* lbl_8047B438;

    lbl_8047B438 = GSresFindById(0x0B521200);
}

s32 GSvtrRegisterGSgapp(u32 taskId)
{
    extern u32 lbl_80467CF8[4];
    extern u32 lbl_8047B42C;
    u32* entry;
    u32 i;

    if (lbl_8047B42C + 1 >= 4) {
        return 0;
    }
    entry = lbl_80467CF8;
    for (i = 0; i < 4; i++, entry++) {
        if (*entry == 0) {
            *entry = taskId;
            lbl_8047B42C++;
            return 1;
        }
    }
    return 0;
}
