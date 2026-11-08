#include "dolphin/types.h"

extern u32 lbl_8047AD68;
extern u32 lbl_8047AD6C;
extern u8 lbl_8047AD70;
extern u8 lbl_8047AD71;

u32 cameraReturnPending(void)
{
    u8 result = 0;

    if (lbl_8047AD68 != 0 && lbl_8047AD6C != 0) {
        result = 1;
    }
    return result;
}

u8 cameraGetReturnDone(void)
{
    return lbl_8047AD71;
}

void std136_fieldClearFlag_8047ad71(void)
{
    lbl_8047AD71 = 0;
}

void cameraSetReturnDone(void)
{
    lbl_8047AD71 = 1;
    lbl_8047AD70 = 0;
}
