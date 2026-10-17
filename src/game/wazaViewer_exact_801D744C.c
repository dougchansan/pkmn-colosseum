#include "game/battle/battle_waza_types.h"

void wazaViewerSetErrorFlag(u32 bits)
{
    *(u32*)(lbl_804673F8 + 0x0C) |= bits;
}
