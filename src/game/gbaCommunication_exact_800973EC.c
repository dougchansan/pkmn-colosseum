/**
 * @file gbaCommunication_exact_800973EC.c
 * @brief gbaCommunication carve, 0x800973EC - 0x80097FFC (13 functions:
 *        the Pokemon-status submenu sync, menuPokemonStatus and the status
 *        menu callbacks, fn_800973EC .. fn_80097FF8, in address order).
 *
 * Split from gbaCommunication_candidate_80096C48.c so these exact functions
 * link while fn_80096D54 and fn_80096FA0 (which own the unit's jump tables)
 * are still being matched. Same declarations and -opt nopeephole build as the
 * candidate carve; the bodies are gbaCommunication.c's, keep them in step.
 */
#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

typedef struct RibbonGroupDescriptor {
    u16 selector;
    s8 firstRibbon;
    u8 maximum;
} RibbonGroupDescriptor;

typedef struct PokemonRibbonGrid {
    u32 count;
    s8 ribbon[9][4];
} PokemonRibbonGrid;

typedef struct PokemonStatusMenuWork {
    u8 flags;
    u8 state;
    s8 selection;
    s8 previousSelection;
    s32 result;
    u32 entityId;
    void* pokemon;
    void* callback;
    s32 callbackArg;
    u16 hasExtraMove;
    u16 padding;
    PokemonRibbonGrid ribbons;
} PokemonStatusMenuWork;

typedef struct MenuStatusPage {
    s8 group;
    s8 page;
} MenuStatusPage;

extern u8 menuIsCheck(s32 menuId);
extern void menuOpen(s32 menuId, s32 arg1);
extern void menuClose(s32 menuId);

/* Open the status submenu for the current page and close the others. */
static inline void menuStatusSyncPage(u8* menu)
{
    MenuStatusPage page;
    u32 pokemon;
    u32 index;
    s32 menuId;

    page = *(MenuStatusPage*)(menu + 0x94);
    menuId = 0;
    pokemon = *(u32*)(lbl_803FB380 + 0x0C);
    if (pokemon == 0) {
        return;
    }

    switch (page.page) {
    case 0:
        if ((s32)pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
            menuId = 0x55;
        } else {
            menuId = 0x54;
        }
        break;
    case 1:
        if (*(u16*)(lbl_803FB380 + 0x18) != 0) {
            menuId = 0x56;
        } else {
            menuId = 0x57;
        }
        break;
    case 2:
        menuId = 0x58;
        break;
    }

    for (index = 0; index < 5; index++) {
        if (menuId == lbl_802EEFC4[index]) {
            if (menuIsCheck(menuId) == 0) {
                menuOpen(menuId, 0);
            }
        } else if (menuIsCheck(lbl_802EEFC4[index]) != 0) {
            menuClose(lbl_802EEFC4[index]);
        }
    }
}

/* Select and synchronize the Pokemon-status submenu for the current page. */
s32 fn_800973EC(u8* menu)
{
    switch ((s8)menu[1]) {
    case 0:
        *(s8*)(menu + 0x97) = -1;
        lbl_803FB380[1] = 0;
        lbl_803FB380[2] = menu[0x95];
        *(s8*)(lbl_803FB380 + 3) = -1;
        *(s8*)(lbl_803FB380 + 0x1A) = -1;
        if (lbl_803FB380[0] & 4) {
            menu[0x95] = 0;
            lbl_803FB380[1] = 1;
        } else {
            menu[0x95] = 1;
            lbl_803FB380[1] = 7;
        }
        menuStatusSyncPage(menu);
        break;
    case 1:
        break;
    case 2:
        menuStatusSyncPage(menu);
        break;
    }

    return 0;
}

/* 0x8009769C | size: 0x350 */
u32 menuPokemonStatusOpen(u8 flags, u32 arg1, s32 pokemon, u16 arg3, u32 arg4, u32 arg5) {
    extern void menuModelInit(void* model, s32 width, s32 height);
    extern void menuPokemonModelRequest(void* model, u32 pokemon);
    extern void fadeCheck(s32 wait);
    extern void fadeSet(s32 mode, f32 value);
    extern s32 menuOpenCustom(s32 menu, ...);
    extern u32 pokemonGetStatus(u32 pokemon, u32 index, u32 status, s32 slot);
    extern u8 pokemonWazaCheckValid(u32 pokemon, s32 slot);
    extern s32 wazaGetStatus(u32 data, u16 index, u32 status, u32 arg3);
    extern void winMsgOpen(s32 slot, s32 message, s32 arg2, s32 arg3);
    extern void winMsgClose(s32 slot);
    extern s32 menuIsCheck(s32 menu);
    extern void menuCloseCustom(s32 menu, s32 mode, s32 wait);
    extern void floorBattleEndTerminateScripts(void);
    extern void menuModelTerm(void* model);

    u32 cursor;
    s32 result;
    u16 move;
    u32 selectedPokemon;
    u8 currentFlags;
    u8* state;

    menuModelInit(lbl_803FB338, 0xC8, 0xB4);
    menuPokemonModelRequest(lbl_803FB338, pokemon);

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(2, lbl_8047C234);
        } else {
            fadeSet(2, lbl_8047C238);
        }
    }

    cursor = 0;
    state = lbl_803FB380;
    for (;;) {
        result = menuOpenCustom(0x53, 0, &cursor, 0, 1, 0);
        if (result == -1) {
            *(s32*)(state + 4) = result;
            break;
        }

        pokemon = *(s8*)(state + 2);
        *(u32*)(state + 4) = *(s8*)(state + 2);
        if (!(state[0] & 0x40)) {
            break;
        }

        selectedPokemon = *(u32*)(state + 0xC);
        if ((u16)pokemon == 4) {
            move = *(u16*)(state + 0x18);
        } else {
            move = (u16)pokemonGetStatus(selectedPokemon, 0, 0x7F, pokemon);
            if ((u8)pokemonWazaCheckValid(selectedPokemon, pokemon) == 0) {
                move = 0;
            }
        }

        if (wazaGetStatus(0, move, 0x19, 0) == 0) {
            break;
        }
        winMsgOpen(2, 0x2BE9, 1, 0);
        winMsgClose(1);
    }

    if (lbl_803FB380[0] & 8) {
        fadeCheck(1);
        if (lbl_803FB380[0] & 0x80) {
            fadeSet(3, lbl_8047C234);
        } else {
            fadeSet(3, lbl_8047C238);
        }
        fadeCheck(1);
    }

    currentFlags = lbl_803FB380[0];
    if ((currentFlags & 1) && !(currentFlags & 8)) {
        fadeCheck(1);
        fadeSet(3, lbl_8047C238);
        fadeCheck(1);
    }

    if ((u8)menuIsCheck(0x54) != 0) {
        menuCloseCustom(0x54, 0, 0);
    }
    if ((u8)menuIsCheck(0x55) != 0) {
        menuCloseCustom(0x55, 0, 0);
    }
    if ((u8)menuIsCheck(0x57) != 0) {
        menuCloseCustom(0x57, 0, 0);
    }
    if ((u8)menuIsCheck(0x56) != 0) {
        menuCloseCustom(0x56, 0, 0);
    }
    if ((u8)menuIsCheck(0x58) != 0) {
        menuCloseCustom(0x58, 0, 0);
    }
    menuCloseCustom(0x53, 0, 1);

    if (lbl_803FB380[0] & 1) {
        floorBattleEndTerminateScripts();
        if (lbl_803FB380[0] & 8) {
            floorSetFadeScript(0, 0);
        }
    }

    menuModelTerm(lbl_803FB338);
    _threadSwitch();
    return *(u32*)(state + 4);
}

/* 0x800979EC | size: 0x4C */
void menuPokemonStatus(void) {
    *(u32*)(lbl_803FB380 + 4) = menuPokemonStatusOpen(
        lbl_803FB380[0],
        *(u32*)(lbl_803FB380 + 8),
        *(u32*)(lbl_803FB380 + 0xC),
        *(u16*)(lbl_803FB380 + 0x18),
        *(u32*)(lbl_803FB380 + 0x10),
        *(u32*)(lbl_803FB380 + 0x14));
}

/* 0x80097A38 | size: 0xCC */
s32 menuPokemonStatusOpenWazaForget(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x59;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    std098_mapWarpToMenuRoom(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097B04 | size: 0xB8 */
s32 fn_80097B04(u32 arg0, u16 arg1) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x58;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = arg1;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    menuPokemonStatusOpen(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                arg1, *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097BBC | size: 0x114 */
s32 fn_80097BBC(u8 chan) {
    extern void* savedataGetStatus();
    extern void* heroBiosGetPokemonPtr();
    extern int pokemonCheckValid();
    extern int fn_8010B560();
    void* entity;
    void* mgr;

    entity = NULL;
    if (chan < 6) {
        mgr = savedataGetStatus(0, 2);
        if (mgr != 0) {
            entity = heroBiosGetPokemonPtr(mgr, chan);
            if ((u8)pokemonCheckValid() == 0) {
                entity = NULL;
            }
        }
    }
    if (entity == 0) {
        return -1;
    }
    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x11;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = (u32)entity;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = 0;
    *(u32*)(lbl_803FB380 + 0x14) = 0;
    *(u32*)(lbl_803FB380 + 0x4) = -1;
    std098_mapWarpToMenuRoom(0x39d);
    if (lbl_803FB380[0] & 8) {
        floorSetFadeScript(0, 0);
    }
    _threadSwitch();
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097CD0 | size: 0xC4 */
s32 fn_80097CD0(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xc;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    menuPokemonStatusOpen(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097D94 | size: 0xC4 */
s32 fn_80097D94(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xe;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    menuPokemonStatusOpen(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097E58 | size: 0xB0 */
s32 fn_80097E58(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0xac;
    *(u32*)(lbl_803FB380 + 0x8) = arg0;
    *(u32*)(lbl_803FB380 + 0xC) = arg1;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg2;
    *(u32*)(lbl_803FB380 + 0x14) = arg3;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    menuPokemonStatusOpen(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097F08 | size: 0xC4 */
s32 fn_80097F08(u32 arg0, u32 arg1, u32 arg2) {
    extern int fn_8010B560();

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    memset(lbl_803FB380, 0, 0x44);
    *(u8*)(lbl_803FB380 + 0x0) = 0x8e;
    *(u32*)(lbl_803FB380 + 0x8) = 0;
    *(u32*)(lbl_803FB380 + 0xC) = arg0;
    *(u16*)(lbl_803FB380 + 0x18) = 0;
    *(u32*)(lbl_803FB380 + 0x10) = arg1;
    *(u32*)(lbl_803FB380 + 0x14) = arg2;
    *(s32*)(lbl_803FB380 + 0x4) = -1;
    menuPokemonStatusOpen(lbl_803FB380[0], *(u32*)(lbl_803FB380 + 0x8), *(u32*)(lbl_803FB380 + 0xC),
                *(u16*)(lbl_803FB380 + 0x18), *(u32*)(lbl_803FB380 + 0x10), *(u32*)(lbl_803FB380 + 0x14));
    return *(s32*)(lbl_803FB380 + 0x4);
}

/* 0x80097FCC | size: 0x4 */
void fn_80097FCC(void) {
}

/* 0x80097FD0 | size: 0x28 */
void fn_80097FD0(void) {
    extern int std093_getCurrentGroupID();
    GSresGetResource(std093_getCurrentGroupID(), 0x12670000);
}

/* 0x80097FF8 | size: 0x4 */
void fn_80097FF8(void) {
}
