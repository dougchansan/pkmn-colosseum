#include "dolphin/types.h"

extern u32 lbl_8047A2EC;
extern void menuPokemonClose(void);

void itemUseClosePartyMenu(u32 value)
{
    lbl_8047A2EC = value;
    menuPokemonClose();
}
