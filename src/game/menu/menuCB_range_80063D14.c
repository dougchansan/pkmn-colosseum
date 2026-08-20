/**
 * @file menuCB_range_80063D14.c
 * @brief Residual menuCB candidate range, 0x80063D14 - 0x80064378.
 */
#define MENUCB_RANGE_RESIDUAL_EMPTY_ONLY
#include "menuCB_range_80062948.c"

typedef struct MenuCBEntryPort {
    u32 unused;
    s32 enabled;
} MenuCBEntryPort;

s32 fn_80063D14(void* work)
{
    extern void fn_80165A20(s32, s32, s32);
    extern void* memcpy(void*, const void*, u32);
    extern MenuCBEntryPort* fn_8006B09C(s32);
    extern void* fn_8006A814(MenuCBEntryPort*);
    extern s32 fn_8006B0F8(s32);
    extern void gbaCommandSendWazaText(void*, s32);
    extern void fn_8008AB20(void*, u16, s32);
    extern void toolentryTaisenInitPokemonOrder(s32);
    extern void* fn_8006ACCC(s32);
    extern s32 menuOpen(s32, s32);
    extern void menuSetEnablePort(s32);
    extern void menuCloseCustom(s32, s32, s32);
    extern s32 toolentryTaisenGetHomePlace(s32);
    extern void msgctrlSetValue(s32, s32);
    extern void winMsgOpen(s32, s32, s32, s32);
    extern s32 fn_800F7EF8(s32);
    extern s32 fn_800F7C28(s32);
    extern void _threadSwitch(void);
    extern void winMsgClose(s32);
    extern void menuCBBattleStartTrainerFaceFree(void);
    extern s32 fn_8025D9CC(void);
    extern void toolentryTaisenEntryPokemon(s32);
    extern f32 lbl_8047BFE8;
    u8* state;
    s32 battleType;
    s32 battleMode;
    s32 playerCount;
    s32 player;
    s32 count;
    s32 menuResult;
    s32 keepRunning;
    s32 home;
    s32 waiting;

    fn_80165A20(0x1E, 0, 0xFF);
    memcpy(lbl_803A9F08 + 0x150, work, 0xCC2C);

    state = lbl_803A9F08;
    battleType = toolentryTaisenGetBattleType();
    playerCount = toolentryTaisenGetEntryPlayerNum();
    *(s32*)&state[0x00] = 0;
    *(s32*)&state[0x0C] = 0;
    *(s32*)&state[0x2C] = 0;
    *(s32*)&state[0xCD80] = 0;
    *(f32*)&state[0xCE4C] = lbl_8047BFE8;
    *(s32*)&state[0xCE50] = 0;
    state[0xCE58] = 1;
    *(s32*)&state[0xCE5C] = -1;

    for (player = 0; player < playerCount; player++) {
        MenuCBEntryPort* port = fn_8006B09C(player);
        void* command = fn_8006A814(port);
        state[player + 4] = 0;
        if (port->enabled != 0) {
            u16 pokemonCount;
            gbaCommandSendWazaText(command, fn_8006B0F8(player));
            pokemonCount = (u16)fn_8006B1D4();
            if ((u16)toolentryTaisenGetPokemonNum(player) < pokemonCount) {
                pokemonCount = (u16)toolentryTaisenGetPokemonNum(player);
            }
            fn_8008AB20(command, pokemonCount, battleType == 1 ? 2 : 1);
        }
    }

    for (player = 0; player < 4; player++) {
        void* entry;

        toolentryTaisenInitPokemonOrder(player);
        entry = fn_8006ACCC(player);
        state[player + 8] =
            entry == NULL ? 0xFF : *(s8*)((u8*)entry + 0x28);
    }

    if (battleType == 0) {
        *(s32*)&state[0xCD80] = 0x136;
    } else {
        *(s32*)&state[0xCD80] = 0;
    }

    keepRunning = 1;
    while (keepRunning != 0) {
        switch (*(s32*)&state[0x00]) {
        case 0:
            menuSetEnablePort(0);
            menuResult = menuOpen(0xC6, 1);
            menuSetEnablePort(1);
            if (menuResult == 0) {
                *(s32*)&state[0x00] = 2;
            } else if (menuResult == 1) {
                *(s32*)&state[0x00] = 1;
            } else {
                *(s32*)&state[0x00] = 3;
            }
            break;
        case 1:
            menuResult = menuOpen(0xC5, 1);
            if (menuResult == 0) {
                state[4] = 1;
                menuCloseCustom(0xC5, 0, 1);
                *(s32*)&state[0x00] = 0;
            } else {
                toolentryTaisenInitPokemonOrder(0);
                menuCloseCustom(0xC5, 0, 1);
                *(s32*)&state[0x00] = 0;
            }
            break;
        case 2:
            menuResult = menuOpen(0xC7, 1);
            menuCloseCustom(0xC7, 0, 1);
            *(s32*)&state[0x00] = 0;
            break;
        case 3:
            keepRunning = 0;
            menuCloseCustom(0xC6, 0, 1);
            menuCloseCustom(0xDF, 0, 1);
            break;
        }
    }

    if (state[0xCE58] == 0) {
        if (toolentryTaisenGetBattleType() == 2 &&
            toolentryTaisenGetHomePlace(0) != 0) {
            winMsgOpen(2, 0x44E7, 1, 1);
            waiting = 1;
            while (waiting != 0) {
                if (fn_800F7EF8(1) != 0 && fn_800F7C28(1) == 0) {
                    waiting = 0;
                } else {
                    _threadSwitch();
                }
            }
            winMsgClose(1);
        } else {
            msgctrlSetValue(0x30, *(s32*)&state[0xCE5C]);
            winMsgOpen(2, 0x44DC, 1, 1);
        }

        *(s32*)&state[0x2C] = 0;
        state[0xCD84] = 0;
        for (player = 0; player < 4; player++) {
            u8* slotState;

            slotState = state + 0x30 + player * 0x48;
            for (count = 0; count < 6; count++) {
                slotState[count * 0x0C] = 0;
                *(s32*)&slotState[count * 0x0C + 4] = 0;
            }
        }
        menuCBBattleStartTrainerFaceFree();
        return 0xB3;
    }

    battleType = toolentryTaisenGetBattleType();
    battleMode = fn_8025D9CC();
    if (battleMode == 4) {
        if (battleType >= 0 && battleType < 2) {
            count = 2;
        } else {
            count = 4;
        }
    } else if (battleType >= 0 && battleType < 2) {
        count = 2;
    } else {
        count = 1;
    }

    for (player = 0; player < count; player++) {
        toolentryTaisenEntryPokemon(player);
    }

    *(s32*)&state[0x2C] = 0;
    state[0xCD84] = 0;
    for (player = 0; player < 4; player++) {
        u8* slotState;

        slotState = state + 0x30 + player * 0x48;
        for (count = 0; count < 6; count++) {
            slotState[count * 0x0C] = 0;
            *(s32*)&slotState[count * 0x0C + 4] = 0;
        }
    }
    return 0xB8;
}
