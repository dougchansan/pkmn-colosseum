#include "dolphin/types.h"

extern u8 lbl_8047A678;

s32 gbaSetSnapshotSent(void)
{
    lbl_8047A678 = 1;
    return 0;
}
