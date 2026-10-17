#include "dolphin/types.h"

s32 fn_80167E54(void)
{
    return -1;
}

u32 GSdvdGetLength(u8* object)
{
    return *(u32*)(object + 0x38);
}
