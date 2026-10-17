/**
 * @file menuPokemonClose_exact.c
 * @brief Close the party menu and restore its field state.
 */
#include "dolphin/types.h"

extern u8 lbl_803A1D40[];
extern void menuCloseCustom();
extern void floorBattleEndTerminateScripts();
extern void floorSetFadeScript();
extern void _threadSwitch();

void menuPokemonClose(void)
{
    menuCloseCustom(0x63, 0, 1);
    if (lbl_803A1D40[2] != 0) {
        floorBattleEndTerminateScripts();
        if (lbl_803A1D40[3] != 1) {
            floorSetFadeScript(0, 0);
        }
        _threadSwitch();
    }
}
