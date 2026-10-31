/**
 * @file menuCB_range_80063D14.c
 * @brief Residual menuCB candidate range, 0x80063D14 - 0x80064378.
 */
#include "dolphin/types.h"

extern s32 toolentryTaisenGetBattleType(void);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern s32 menuTaisenGetEntryCount(void);
extern u16 toolentryTaisenGetPokemonNum(s32);
extern s32 battleResultGet10(void);

typedef struct MenuCBEntryPort {
    u32 unused;
    s32 enabled;
} MenuCBEntryPort;

extern void std104_soundSetBackgroundMusic(s32, s32, s32);
extern void* memcpy(void*, const void*, u32);
extern MenuCBEntryPort* savedataBattleGetEntryPtr(s32);
extern u32 savedataBattleGetTrainerData(MenuCBEntryPort*);
extern s32 savedataBattleGetTeamPtr(s32);
extern void gbaCommandSendWazaText(s32, s32);
extern void gbaCommSendCmd77(s32, u32, u32);
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

extern MenuCBEntryWork lbl_803A9F08;
#define MENUCB_WORK (&lbl_803A9F08)

static inline u16 menuCBMin(u16 a, u16 b)
{
    if (a < b) {
        return a;
    }
    return b;
}

static inline void menuCBSetEntryState(s32 next)
{
    lbl_803A9F08.state = next;
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
    lbl_803A9F08.slotsDirty = 0;
    for (i = 0; i < 4; i++) {
        player = &state->player[i];
        for (j = 0; j < 6; j++) {
            slot = &player->slot[j];
            slot->used = 0;
            slot->value = 0;
        }
    }
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (GBA port loop, nested under menuCBEntryInit) chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
static inline void menuCBEntryPorts(s32 battleType, s32 playerCount)
{
    s32 player;
    u32 command;
    u16 maxCount;
    MenuCBEntryPort* port;
    u16 pokemonCount;

    for (player = 0; player < playerCount; player++) {
        s32 wazaText;

        port = savedataBattleGetEntryPtr(player);
        command = savedataBattleGetTrainerData(port);
        wazaText = savedataBattleGetTeamPtr(player);
        MENUCB_WORK->ready[player] = 0;
        if (port->enabled != 0) {
            s32 mode;

            gbaCommandSendWazaText(command, wazaText);
            if (battleType == 1) {
                mode = 2;
            } else {
                mode = 1;
            }
            maxCount = menuTaisenGetEntryCount();
            pokemonCount = toolentryTaisenGetPokemonNum(player) & 0xFFFF;
            gbaCommSendCmd77(command, menuCBMin(pokemonCount, maxCount), mode);
        }
    }
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (setup stores) chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
static inline void menuCBEntryInit(void)
{
    s32 battleType;
    s32 playerCount;

    battleType = toolentryTaisenGetBattleType();
    playerCount = toolentryTaisenGetEntryPlayerNum();
    MENUCB_WORK->cancelled = 1;
    MENUCB_WORK->message = -1;
    MENUCB_WORK->scale = 0.0f;
    MENUCB_WORK->frame = 0;
    MENUCB_WORK->cursor = 0;
    MENUCB_WORK->timer = 0;
    MENUCB_WORK->state = 0;
    MENUCB_WORK->selection = 0;
    menuCBEntryPorts(battleType, playerCount);
}


/* RULE-EXCEPTION(user-approved): single-use inline helper (decision wait loop) chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
static inline void menuCBWaitDecided(void)
{
    s32 waiting;

    waiting = 1;
    do {
        if (menuCBDecided()) {
            waiting = 0;
        } else {
            _threadSwitch();
        }
    } while (waiting != 0);
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (entry-order loop, nested under menuCBEntryOrderAll) chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
static inline void menuCBEntryOrder(void** entries)
{
    void** e;
    s32 player;

    for (player = 0, e = &entries[player]; player < 4; e++, player++) {
        *e = savedataBattleFindEntryByPlayer(player);
        if (*e != NULL) {
            MENUCB_WORK->order[player] = *(s32*)((u8*)*e + 0x28);
        } else {
            MENUCB_WORK->order[player] = -1;
        }
    }
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (depth wrapper for the entry-order loop) chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
static inline void menuCBEntryOrderAll(void** entries)
{
    menuCBEntryOrder(entries);
}

s32 menuTaisenSetupBattle(void* work)
{
    void* entries[4];
    s32 battleType;
    s32 player;
    s32 count;
    s32 menuResult;
    s32 keepRunning;

    keepRunning = 1;
    std104_soundSetBackgroundMusic(0x1E, 0, 0xFF);
    memcpy(lbl_803A9F08.copy, work, 0xCC2C);

    menuCBEntryInit();

    for (player = 0; player < 4; player++) {
        toolentryTaisenInitPokemonOrder(player);
    }

    menuCBEntryOrderAll(entries);

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
        switch (lbl_803A9F08.state) {
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
                lbl_803A9F08.ready[0] = 1;
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
            menuCBWaitDecided();
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
