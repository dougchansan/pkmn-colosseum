#include "dolphin/types.h"
#include "game/camera_types.h"

extern void* lbl_80478C40;

void cameraSetTargetExt(u32 a, u32 b, u32 c)
{
    ((CameraPadState*)lbl_80478C40)->targetGroup = a;
    ((CameraPadState*)lbl_80478C40)->targetId = b;
    ((CameraPadState*)lbl_80478C40)->targetSubId = c;
}

void cameraSetTarget(u32 a, u32 b)
{
    ((CameraPadState*)lbl_80478C40)->targetGroup = a;
    ((CameraPadState*)lbl_80478C40)->targetId = b;
    ((CameraPadState*)lbl_80478C40)->targetSubId = -1;
}
