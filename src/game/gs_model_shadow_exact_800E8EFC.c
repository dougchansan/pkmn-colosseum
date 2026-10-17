#include "dolphin/types.h"

typedef struct GSshadowSlot {
    u8 pad_00[0x50];
    u8 active;
    u8 pad_51[3];
    void* object;
} GSshadowSlot;

extern GSshadowSlot lbl_80401490[6];
extern u32 lbl_8047AB84;
extern u32 lbl_8047AB80;
extern f32 lbl_8047AB88;

extern void fn_801B06DC(void* object);
extern void HSD_ShadowSetActive(void* object, u32 flags);

void GSmodelFreeAllShadowTextures(void)
{
    u32 i;

    for (i = 0; i < 6; i++) {
        fn_801B06DC(lbl_80401490[i].object);
        HSD_ShadowSetActive(lbl_80401490[i].object, 0);
        lbl_80401490[i].active = 0;
    }
}

void GSmodelSetShadowBoundExpansion(u32 extent, u32 state)
{
    lbl_8047AB84 = extent;
    lbl_8047AB80 = state;
}

void GSmaterialSetDistanceThreshold(f32 distance)
{
    lbl_8047AB88 = distance * distance;
}
