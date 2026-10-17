#include "dolphin/types.h"
#include "game/fsys/fsys.h"

u32 fsysStatus3(FSYSSlot* slot)
{
    switch (slot->padding054) {
    case 4:
    case 5:
    case 11:
        return 1;
    default:
        return 0;
    }
}
