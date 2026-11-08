#include "dolphin/types.h"

s32 fightAiIsUncopyableWaza(u16 id)
{
    if (id == 0x164 || (u16)(id - 0xa5) <= 1 || id == 0xffff || id == 0 ||
        id == 0x165) {
        return 1;
    }
    return 0;
}
