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

extern void std104_soundSetBackgroundMusic(s32, s32, s32);
extern void* memcpy(void*, const void*, u32);
extern MenuCBEntryPort* savedataBattleGetEntryPtr(s32);
extern void* fn_8006A814(MenuCBEntryPort*);
extern s32 savedataBattleGetTeamPtr(s32);
extern void gbaCommandSendWazaText(void*, s32);
extern void fn_8008AB20(void*, u16, s32);
extern void toolentryTaisenInitPokemonOrder(s32);
extern void* savedataBattleFindEntryByPlayer(s32);
extern s32 menuOpen(s32, s32);
extern void menuSetEnablePort(s32);
extern void menuCloseCustom(s32, s32, s32);
extern s32 toolentryTaisenGetHomePlace(s32);
extern void msgctrlSetValue(s32, s32);
extern void winMsgOpen(s32, s32, s32, s32);
extern u8 GSinputIsConnected(s32);
extern s32 GSinputGetSubType(s32);
extern void _threadSwitch(void);
extern void winMsgClose(s32);
extern void menuCBBattleStartTrainerFaceFree(void);
extern void toolentryTaisenEntryPokemon(s32);

typedef struct MenuCBEntrySlot {
    u8 used;
    s32 value;
    s32 extra;
} MenuCBEntrySlot;

typedef struct MenuCBEntryPlayer {
    MenuCBEntrySlot slot[6];
} MenuCBEntryPlayer;

typedef struct MenuCBEntryWork {
    /* 0x0000 */ s32 state;
    /* 0x0004 */ u8 ready[4];
    /* 0x0008 */ s8 order[4];
    /* 0x000C */ s32 selection;
    /* 0x0010 */ u8 _10[0x1C];
    /* 0x002C */ s32 cursor;
    /* 0x0030 */ MenuCBEntryPlayer player[4];
    /* 0x0150 */ u8 copy[0xCC2C];
    /* 0xCD7C */ u8 _CD7C[4];
    /* 0xCD80 */ s32 timer;
    /* 0xCD84 */ u8 slotsDirty;
    /* 0xCD85 */ u8 _CD85[0xC7];
    /* 0xCE4C */ f32 scale;
    /* 0xCE50 */ s32 frame;
    /* 0xCE54 */ u8 _CE54[4];
    /* 0xCE58 */ u8 cancelled;
    /* 0xCE5C */ s32 message;
} MenuCBEntryWork;

#define MENUCB_WORK ((MenuCBEntryWork*)lbl_803A9F08)

static inline s32 menuCBEntryMode(s32 battleType)
{
    s32 mode;

    if (battleType == 1) {
        mode = 2;
    } else {
        mode = 1;
    }
    return mode;
}

static inline s32 menuCBMin(u16 a, u16 b)
{
    if (a < b) {
        return a;
    }
    return b;
}

static inline void menuCBSetEntryState(s32 next)
{
    *(s32*)&lbl_803A9F08[0x00] = next;
}

static inline u16 menuCBIsHomeBattle(void)
{
    if (toolentryTaisenGetBattleType() == 2) {
        if ((s32)(u16)toolentryTaisenGetHomePlace(0) != 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

static inline u8 menuCBDecided(void)
{
    if (GSinputIsConnected(1)) {
        if (GSinputGetSubType(1) == 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

static inline void menuCBClearEntrySlots(MenuCBEntryWork* state)
{
    MenuCBEntryPlayer* player;
    MenuCBEntrySlot* slot;
    s32 i;
    s32 j;

    state->cursor = 0;
    ((MenuCBEntryWork*)lbl_803A9F08)->slotsDirty = 0;
    for (i = 0; i < 4; i++) {
        player = &state->player[i];
        for (j = 0; j < 6; j++) {
            slot = &player->slot[j];
            slot->used = 0;
            slot->value = 0;
        }
    }
}

s32 menuTaisenSetupBattle(void* work)
{
    void* entries[4];
    s32 battleType;
    s32 waiting;
    u16 maxCount;
    s32 playerCount;
    u16 pokemonCount;
    s32 player;
    s32 count;
    s32 menuResult;
    s32 keepRunning;
    MenuCBEntryPort* port;
    void* command;

    keepRunning = 1;
    std104_soundSetBackgroundMusic(0x1E, 0, 0xFF);
    memcpy(((MenuCBEntryWork*)lbl_803A9F08)->copy, work, 0xCC2C);

    battleType = toolentryTaisenGetBattleType();
    playerCount = toolentryTaisenGetEntryPlayerNum();
    MENUCB_WORK->cancelled = 1;
    MENUCB_WORK->message = -1;
    MENUCB_WORK->scale = lbl_8047BFE8;
    MENUCB_WORK->frame = 0;
    MENUCB_WORK->cursor = 0;
    MENUCB_WORK->timer = 0;
    MENUCB_WORK->state = 0;
    MENUCB_WORK->selection = 0;

    for (player = 0; player < playerCount; player++) {
        s32 wazaText;

        port = savedataBattleGetEntryPtr(player);
        command = fn_8006A814(port);
        wazaText = savedataBattleGetTeamPtr(player);
        MENUCB_WORK->ready[player] = 0;
        if (port->enabled != 0) {
            s32 mode;

            gbaCommandSendWazaText(command, wazaText);
            mode = menuCBEntryMode(battleType);
            maxCount = fn_8006B1D4();
            pokemonCount = toolentryTaisenGetPokemonNum(player);
            fn_8008AB20(command, menuCBMin(pokemonCount, maxCount), mode);
        }
    }

    for (player = 0; player < 4; player++) {
        toolentryTaisenInitPokemonOrder(player);
    }

    for (player = 0; player < 4; player++) {
        entries[player] = savedataBattleFindEntryByPlayer(player);
        if (entries[player] != NULL) {
            MENUCB_WORK->order[player] = *(s32*)((u8*)entries[player] + 0x28);
        } else {
            MENUCB_WORK->order[player] = -1;
        }
    }

    switch (toolentryTaisenGetBattleType()) {
    case 0:
    case 1:
        MENUCB_WORK->timer = 0x136;
        break;
    case 2:
        MENUCB_WORK->timer = 0;
        break;
    default:
        MENUCB_WORK->timer = 0;
        break;
    }

    do {
        switch (*(s32*)&lbl_803A9F08[0x00]) {
        case 0:
            menuSetEnablePort(0);
            menuResult = menuOpen(0xC6, 1);
            menuSetEnablePort(1);
            if (menuResult == 0) {
                menuCBSetEntryState(2);
            } else if (menuResult == 1) {
                menuCBSetEntryState(1);
            } else {
                menuCBSetEntryState(3);
            }
            break;
        case 1:
            if (menuOpen(0xC5, 1) == 0) {
                lbl_803A9F08[4] = 1;
                menuCloseCustom(0xC5, 0, 1);
                menuCBSetEntryState(0);
            } else {
                toolentryTaisenInitPokemonOrder(0);
                menuCloseCustom(0xC5, 0, 1);
                menuCBSetEntryState(0);
            }
            break;
        case 2:
            if (menuOpen(0xC7, 1) >= 0) {
                menuCloseCustom(0xC7, 0, 1);
                menuCBSetEntryState(0);
            } else {
                menuCloseCustom(0xC7, 0, 1);
                menuCBSetEntryState(0);
            }
            break;
        case 3:
            keepRunning = 0;
            menuCloseCustom(0xC6, 0, 1);
            menuCloseCustom(0xDF, 0, 1);
            break;
        }
    } while (keepRunning != 0);

    if (MENUCB_WORK->cancelled == 0) {
        if (menuCBIsHomeBattle() == 0) {
            msgctrlSetValue(0x30, MENUCB_WORK->message);
            winMsgOpen(2, 0x44DC, 1, 1);
        } else {
            winMsgOpen(2, 0x44E7, 1, 1);
            waiting = 1;
            do {
                if (menuCBDecided()) {
                    waiting = 0;
                } else {
                    _threadSwitch();
                }
            } while (waiting != 0);
            winMsgClose(1);
        }
        menuCBClearEntrySlots(MENUCB_WORK);
        menuCBBattleStartTrainerFaceFree();
        return 0xB3;
    }

    battleType = toolentryTaisenGetBattleType();
    if (battleResultGet10() == 4) {
        switch (battleType) {
        case 0:
        case 1:
            count = 2;
            break;
        default:
            count = 4;
            break;
        }
    } else {
        switch (battleType) {
        case 0:
        case 1:
            count = 2;
            break;
        default:
            count = 1;
            break;
        }
    }

    for (player = 0; player < count; player++) {
        toolentryTaisenEntryPokemon(player);
    }

    menuCBClearEntrySlots(MENUCB_WORK);
    return 0xB8;
}
