#include "dolphin/types.h"
#include "game/fsys/fsys.h"

u32 fsysStatus302(void)
{
    return 1;
}

u32 fsysStatus301(void)
{
    return 1;
}

u32 fsysStatus300(void)
{
    return 0;
}

u32 fsysStatus162(FSYSSlot* slot)
{
    slot->status = 0x66;
    return 0;
}

u32 fsysStatus161(void)
{
    return 1;
}

u32 fsysStatus160(void)
{
    return 1;
}

u32 fsysStatus152(FSYSSlot* slot)
{
    slot->status = 0x66;
    return 0;
}

u32 fsysStatus151(void)
{
    return 1;
}
