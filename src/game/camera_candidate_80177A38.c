#include "dolphin/types.h"

extern u32 lbl_80478C40;

u32 GSscene_GetMode(void)
{
    return *(u8*) lbl_80478C40;
}
