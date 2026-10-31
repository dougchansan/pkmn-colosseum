#include "dolphin/types.h"

typedef struct GSFieldWorldResourceState {
    u8 pad_00[0x10];
    u32 field_10;
} GSFieldWorldResourceState;

extern GSFieldWorldResourceState lbl_804083D0;

u32 fieldCameraDebugGetRoom(void)
{
    return lbl_804083D0.field_10;
}
