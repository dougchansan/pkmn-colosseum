/**
 * @file menuCB_range_800676EC.c
 * @brief Residual menuCB candidate range, 0x800676EC - 0x80068738.
 */
#define MENUCB_RANGE_RESIDUAL_EMPTY_ONLY
#include "menuCB_range_80062948.c"

typedef struct MenuCBContext676EC {
    u8 pad_00[0x95];
    u8 state95;
    u8 pad_96[2];
    u8 state98;
    u8 state99;
} MenuCBContext676EC;

typedef struct MenuCBAfe4 {
    u8 pad[4];
    s32 type;
} MenuCBAfe4;

extern u8 lbl_803A9F08[];
extern u16 lbl_802EDB64[];
extern f32 lbl_8047BFE8;
extern f32 lbl_8047BFEC;
extern MenuCBAfe4* fn_8006AFE4(void);
extern s32 fn_8006B1D4(void);
extern u8 fn_8008ABA0(void);
extern s32 fn_800F7AF0(void);
extern s32 fn_800F7BC4(s32);
extern void fn_80166AB8(u32, u32, u32);
extern s32 fn_8025D9CC(void);
extern s32 toolentryTaisenDeleteEtnryPokemonOrder(s32);
extern u16 toolentryTaisenGetBattlePlayerID(s32);
extern s32 toolentryTaisenGetBattleType(void);
extern s32 toolentryTaisenGetControlerType(s32);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern s32 toolentryTaisenGetHomePlace(s32);
extern s32 toolentryTaisenGetPokemonNum(s32);
extern s32 toolentryTaisenSetEtnryPokemonOrder(s32, s32);
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);

static void menuCBSetAnimTrack(s32 player, s32 slot) {
    *(f32*)&lbl_803A9F08[0xCD8C + (player * 0x30) + (slot * 4)] = (f32)((5 - slot) * 0x18);
    *(f32*)&lbl_803A9F08[0xCDA4 + (player * 0x30) + (slot * 4)] = lbl_8047BFE8;
}

void fn_800679C0(MenuCBContext676EC* context, s32 startPlayer) {
    s32 player;
    s32 startOffset;
    s32 entryPlayers;

    entryPlayers = toolentryTaisenGetEntryPlayerNum();
    toolentryTaisenGetBattleType();
    toolentryTaisenGetHomePlace(0);
    toolentryTaisenGetHomePlace(1);
    toolentryTaisenGetHomePlace(2);
    toolentryTaisenGetHomePlace(3);

    player = (startPlayer != 0) ? 1 : 0;
    startOffset = player * 0x30;

    while (player < entryPlayers) {
        if ((fn_8025D9CC() == 4) &&
            (lbl_803A9F08[player + 4] != 0) &&
            ((toolentryTaisenGetControlerType(player),
              (fn_8006AFE4()->type == 1)) ||
             (fn_8006AFE4()->type == 2))) {
            s32 controller = toolentryTaisenGetControlerType(player);
            if (fn_8008ABA0() == 0) {
                lbl_803A9F08[0xCE58] = 0;
                if (*(s32*)&lbl_803A9F08[0xCE5C] < 0) {
                    *(s32*)&lbl_803A9F08[0xCE5C] = controller;
                }
            }
        }

        if (lbl_803A9F08[player + 4] == 0) {
            if (fn_8025D9CC() == 4) {
                toolentryTaisenGetControlerType(player);
                if ((fn_8006AFE4()->type == 1) || (fn_8006AFE4()->type == 2)) {
                    _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
                        toolentryTaisenGetControlerType(player), player);
                } else {
                    s32 controller = toolentryTaisenGetControlerType(player);
                    s32 buttons = fn_800F7BC4(controller) & fn_800F7AF0();

                    toolentryTaisenGetBattlePlayerID(player);
                    if ((buttons & 0x40) != 0) {
                        s32 count = toolentryTaisengetEtnryPokemonOrderNum(player);
                        if (count != toolentryTaisenDeleteEtnryPokemonOrder(player)) {
                            fn_80166AB8(0x25, 0, 0);
                        }
                    } else if ((buttons & 0xC0F) != 0) {
                        s32 selection = -1;
                        s32 slot;
                        u16 maxPokemon = toolentryTaisenGetPokemonNum(player);

                        if (buttons & 1) {
                            selection = 0;
                        }
                        if (buttons & 8) {
                            selection = 1;
                        }
                        if (buttons & 0x800) {
                            selection = 2;
                        }
                        if (buttons & 4) {
                            selection = 3;
                        }
                        if (buttons & 2) {
                            selection = 4;
                        }
                        if (buttons & 0x400) {
                            selection = 5;
                        }
                        if ((s32)maxPokemon <= selection) {
                            selection = -1;
                        }
                        if (selection >= 0) {
                            slot = toolentryTaisenSetEtnryPokemonOrder(player, selection);
                            if (slot >= 0) {
                                fn_80166AB8(0x3C3, 0, 0);
                                menuCBSetAnimTrack(player, slot);
                            }
                        }
                    }
                }
            } else if (player == 1) {
                s32 buttons;
                s32 selection;
                s32 slot;
                u16 maxPokemon;

                toolentryTaisenGetControlerType(player);
                toolentryTaisenGetBattlePlayerID(player);
                *(f32*)&lbl_803A9F08[0xCE4C] =
                    *(f32*)&lbl_803A9F08[0xCE4C] + *(f32*)&lbl_803A9F08[0xCD88];
                if (*(f32*)&lbl_803A9F08[0xCE4C] == lbl_8047BFEC) {
                    *(f32*)&lbl_803A9F08[0xCE4C] = lbl_8047BFE8;
                    buttons = lbl_802EDB64[*(s32*)&lbl_803A9F08[0xCE50]];
                    *(s32*)&lbl_803A9F08[0xCE50] = *(s32*)&lbl_803A9F08[0xCE50] + 1;
                } else {
                    buttons = 0;
                }
                if ((buttons & 0xC0F) != 0) {
                    selection = -1;
                    maxPokemon = toolentryTaisenGetPokemonNum(player);
                    if (buttons & 1) {
                        selection = 0;
                    }
                    if (buttons & 8) {
                        selection = 1;
                    }
                    if (buttons & 0x800) {
                        selection = 2;
                    }
                    if (buttons & 4) {
                        selection = 3;
                    }
                    if (buttons & 2) {
                        selection = 4;
                    }
                    if (buttons & 0x400) {
                        selection = 5;
                    }
                    if ((s32)maxPokemon <= selection) {
                        selection = -1;
                    }
                    slot = toolentryTaisenSetEtnryPokemonOrder(player, selection);
                    if (slot >= 0) {
                        fn_80166AB8(0x3C3, 0, 0);
                        menuCBSetAnimTrack(player, slot);
                    }
                }
            } else {
                u16 homePlace = toolentryTaisenGetHomePlace(player);
                if ((s32)homePlace != 0) {
                    if (((s32)homePlace >= 0) && ((s32)homePlace < 3)) {
                        _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
                            toolentryTaisenGetControlerType(player), player);
                    } else {
                        s32 controller = toolentryTaisenGetControlerType(player);
                        s32 buttons = fn_800F7BC4(controller) & fn_800F7AF0();

                        toolentryTaisenGetBattlePlayerID(player);
                        if ((buttons & 0x40) != 0) {
                            s32 count = toolentryTaisengetEtnryPokemonOrderNum(player);
                            if (count != toolentryTaisenDeleteEtnryPokemonOrder(player)) {
                                fn_80166AB8(0x25, 0, 0);
                            }
                        } else if ((buttons & 0xC0F) != 0) {
                            s32 selection = -1;
                            s32 slot;
                            u16 maxPokemon = toolentryTaisenGetPokemonNum(player);

                            if (buttons & 1) {
                                selection = 0;
                            }
                            if (buttons & 8) {
                                selection = 1;
                            }
                            if (buttons & 0x800) {
                                selection = 2;
                            }
                            if (buttons & 4) {
                                selection = 3;
                            }
                            if (buttons & 2) {
                                selection = 4;
                            }
                            if (buttons & 0x400) {
                                selection = 5;
                            }
                            if ((s32)maxPokemon <= selection) {
                                selection = -1;
                            }
                            if (selection >= 0) {
                                slot = toolentryTaisenSetEtnryPokemonOrder(player, selection);
                                if (slot >= 0) {
                                    fn_80166AB8(0x3C3, 0, 0);
                                    menuCBSetAnimTrack(player, slot);
                                }
                            }
                        }
                    }
                } else {
                    s32 controller = toolentryTaisenGetControlerType(player);
                    s32 buttons = fn_800F7BC4(controller) & fn_800F7AF0();

                    toolentryTaisenGetBattlePlayerID(player);
                    if ((buttons & 0x40) != 0) {
                        s32 count = toolentryTaisengetEtnryPokemonOrderNum(player);
                        if (count != toolentryTaisenDeleteEtnryPokemonOrder(player)) {
                            fn_80166AB8(0x25, 0, 0);
                        }
                    } else if ((buttons & 0xC0F) != 0) {
                        s32 selection = -1;
                        s32 slot;
                        u16 maxPokemon = toolentryTaisenGetPokemonNum(player);

                        if (buttons & 1) {
                            selection = 0;
                        }
                        if (buttons & 8) {
                            selection = 1;
                        }
                        if (buttons & 0x800) {
                            selection = 2;
                        }
                        if (buttons & 4) {
                            selection = 3;
                        }
                        if (buttons & 2) {
                            selection = 4;
                        }
                        if (buttons & 0x400) {
                            selection = 5;
                        }
                        if ((s32)maxPokemon <= selection) {
                            selection = -1;
                        }
                        if (selection >= 0) {
                            slot = toolentryTaisenSetEtnryPokemonOrder(player, selection);
                            if (slot >= 0) {
                                fn_80166AB8(0x3C3, 0, 0);
                                menuCBSetAnimTrack(player, slot);
                            }
                        }
                    }
                }
            }

            if ((toolentryTaisenGetControlerType(player) == 1) &&
                (toolentryTaisenGetHomePlace(player) == 0)) {
                s32 orderCount = toolentryTaisengetEtnryPokemonOrderNum(player);
                u16 pokemonCount = fn_8006B1D4();
                s32 limit = toolentryTaisenGetPokemonNum(player);
                s32 compareCount = ((s32)limit < (s32)pokemonCount) ? limit : pokemonCount;

                if (orderCount == compareCount) {
                    s32 prev = orderCount - 1;
                    if (prev < 0) {
                        prev = 0;
                    }
                    if (lbl_8047BFE8 == *(f32*)&lbl_803A9F08[0xCD8C + startOffset + (prev * 4)]) {
                        context->state95 = 1;
                        context->state98 = 1;
                    }
                }
            }
        }

        player += 1;
        startOffset += 0x30;
    }
}
