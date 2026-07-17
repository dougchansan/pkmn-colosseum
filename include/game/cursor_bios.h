#ifndef GAME_CURSOR_BIOS_H
#define GAME_CURSOR_BIOS_H

#include "dolphin/types.h"

u32 cursorBiosGetPos(u16 index);
u32 cursorBiosSetPos(u16 index, u16* position);

#endif /* GAME_CURSOR_BIOS_H */
