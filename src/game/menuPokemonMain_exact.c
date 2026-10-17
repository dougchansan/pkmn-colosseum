/**
 * @file menuPokemonMain_exact.c
 * @brief Run and close the synchronous party menu.
 */
#include "dolphin/types.h"

extern u8 lbl_803A1D40[];
extern void menuPokemonSub();
extern void menuCloseCustom();
extern void floorBattleEndTerminateScripts();
extern void floorSetFadeScript();
extern void _threadSwitch();

s32 menuPokemonMain(void)
{
    u8 mode;
    u16 selection;
    u32 context;

    mode = lbl_803A1D40[0];
    selection = *(u16*)(lbl_803A1D40 + 0x12);
    context = *(u32*)(lbl_803A1D40 + 0xC);
    menuPokemonSub((u32)mode, (u32)selection, context);
    menuCloseCustom(0x63, 0, 1);
    if (lbl_803A1D40[2] != 0) {
        floorBattleEndTerminateScripts();
        if (lbl_803A1D40[3] != 1) {
            floorSetFadeScript(0, 0);
        }
        _threadSwitch();
    }
    return 0;
}
