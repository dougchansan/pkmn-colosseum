#include "dolphin/types.h"

u32 gbaKeyGetWord(u32 value)
{
    return value & 0xFFFF;
}

u32 gbaKeyGetHighByte(u32 value)
{
    return (value >> 8) & 0xFF;
}

u32 gbaKeyGetByte(u32 value)
{
    return value & 0xFF;
}

u32 gbaKeyGetHighHalf(u32 value)
{
    return value >> 16;
}
