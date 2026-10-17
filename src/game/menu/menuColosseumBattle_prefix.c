/**
 * @file menuColosseumBattle_prefix.c
 * @brief menuColosseumBattle.cpp prefix candidate, 0x80058150 - 0x80058754.
 */
#include "dolphin/types.h"
#include "game/menu/menu_name_entry.h"

typedef struct ColosseumBattleConnectState {
    s32 active;
    s32 connected;
    s32 busyRequest;
    s32 reserved;
} ColosseumBattleConnectState;

typedef struct ColosseumUnownMenuIds {
    u32 normal;
    u32 shiny;
} ColosseumUnownMenuIds;

extern void _threadSwitch(void);
extern u32 _toolentryAlloc__FUl(u32);
extern void* GSmemLock(u32);
extern void fn_800E24B0(u32);
extern void GSmemFree(u32);
extern void* getPokemon__5PCBOXFScSc(void*, s8, s8);
extern u32 GSthreadCreate(u32, u32, u32, u32, u32, void*);
extern void* heroGetStatus(void*, s32, u16);
extern void menuClose();
extern s32 menuCloseSync(s32, s32);
extern s32 menuOpen(s32, s32);
extern s8 pcboxGetNbPokemonBox(void);
extern s8 fn_801347D8(void);
extern u8 pokemonCheckValid(void*);
extern u8 pokemonIsDarkPokemon(void*);
extern u32 pokemonBiosGetPokemonDataId(void*);
extern u32 pokemonGetStatus(void*, u32, u32, u32);
extern u32 pokemonGetAnnonKatati(u32);
extern void scriptStoreTemochiPokemon(s32);
extern u8 scriptCheckTemochiPokemon(s32);
extern u32 floorGetCurrentContext(void);
extern void fn_80057E70(void);
extern void fn_80055B98(s32);
extern void fn_800F915C(s32);
extern void fn_8017B3E4(s32);
extern s32 fn_8017B2CC(s32);
extern void GSresReleaseById(s32);
extern void std078_flagSetFalse(s32);

extern void* lbl_8047A590;
extern const ColosseumUnownMenuIds lbl_802676F0[28];
extern u8 lbl_803A9A08[];

#define gColosseumBattleConnect (*(ColosseumBattleConnectState*)lbl_803A9A08)

static inline s32 colosseumBattleGetConnectState(void) {
    if (gColosseumBattleConnect.active != 0) {
        if (gColosseumBattleConnect.connected != 0) {
            return 1;
        }
        return 0;
    }
    return 2;
}

typedef struct ColosseumUnownMenuTable {
    ColosseumUnownMenuIds ids[28];
} ColosseumUnownMenuTable;

static inline s32 colosseumBattleGetMenuPokemonId(void* pokemon) {
    u32 id;
    u32 form;

    id = pokemonBiosGetPokemonDataId(pokemon);
    if ((u16)id == 0) {
        return -1;
    }
    if ((u16)id == 0xC9) {
        ColosseumUnownMenuTable table = *(ColosseumUnownMenuTable*)lbl_802676F0;

        form = pokemonGetAnnonKatati(pokemonGetStatus(pokemon, 0, 0x6F, 0));
        if ((u8)form >= 28) {
            return -1;
        }
        id = pokemonBiosGetPokemonDataId(pokemon);
        if ((u16)id == 0) {
            return -1;
        }
        if ((u8)pokemonGetStatus(pokemon, id, 0xC1, 0) != 0) {
            return table.ids[(u8)form].shiny;
        }
        return table.ids[(u8)form].normal;
    }
    id = pokemonGetStatus(NULL, id, 0x5A,
                          (u16)(((u8)pokemonGetStatus(pokemon, id, 0xC1, 0) != 0) ? 1 : 0));
    if (id == 0) {
        return -1;
    }
    return id;
}

static inline s32 colosseumBattleCountPokemon(void) {
    s32 slot;
    s32 box;
    s32 count;
    void* pokemon;

    count = 0;
    for (slot = 0; slot < 6; slot++) {
        pokemon = heroGetStatus(NULL, 3, (u16)slot);
        if (pokemon != NULL && pokemonCheckValid(pokemon) != 0) {
            count++;
        }
    }

    for (box = 0; box < pcboxGetNbPokemonBox(); box++) {
        for (slot = 0; slot < fn_801347D8(); slot++) {
            pokemon = getPokemon__5PCBOXFScSc(0, (s8)box, (s8)slot);
            if (pokemon != NULL && pokemonCheckValid(pokemon) != 0) {
                count++;
            }
        }
    }
    return count;
}

/* Address: 0x80058150 | Size: 0x604 */
void pcBoxStorageMain(void) {
    u32 handle;
    u32* list;
    u32* out;
    void* pokemon;
    s32 listCount;
    s32 box;
    s32 slot;
    u32 id;
    s32 connectedState;
    s32 shouldSkipWait;

    scriptStoreTemochiPokemon(0);
    fn_8017B3E4(0x70B);
    while (fn_8017B2CC(0x70B) == 1) {
        _threadSwitch();
    }

    listCount = colosseumBattleCountPokemon() + 1;
    if (listCount > 0) {
        handle = _toolentryAlloc__FUl((u32)listCount * sizeof(u32));
        list = GSmemLock(handle);
        lbl_8047A590 = list;
        out = list;

        if (list != NULL) {
            for (slot = 0; slot < 6; slot++) {
                pokemon = heroGetStatus(NULL, 3, (u16)slot);
                if (pokemon != NULL && pokemonCheckValid(pokemon) != 0) {
                    id = colosseumBattleGetMenuPokemonId(pokemon);
                    if (id != -1) {
                        *out++ = id;
                    }
                }
            }

            for (box = 0; box < pcboxGetNbPokemonBox(); box++) {
                for (slot = 0; slot < fn_801347D8(); slot++) {
                    pokemon = getPokemon__5PCBOXFScSc(0, (s8)box, (s8)slot);
                    if (pokemon != NULL && pokemonCheckValid(pokemon) != 0) {
                        id = colosseumBattleGetMenuPokemonId(pokemon);
                        if (id != -1) {
                            *out++ = id;
                        }
                    }
                }
            }
            *out = 0;
        }
    } else {
        lbl_8047A590 = NULL;
    }

    GSthreadCreate(1, floorGetCurrentContext(), 0x4000, 1, 1, fn_80057E70);
    menuClose(0x19);
    menuClose(0x1A);
    menuClose(0x1B);
    menuCloseSync(0x19, 1);
    menuCloseSync(0x1A, 1);
    menuCloseSync(0x1B, 1);
    fn_80055B98(0);
    menuOpen(0x1A, 0);
    menuOpen(0x1B, 0);
    menuOpen(0x19, 0);

    connectedState = colosseumBattleGetConnectState();
    if (connectedState == 2) {
        GSresReleaseById(0x48A);
        fn_800F915C(0x48A);
        shouldSkipWait = 1;
    } else {
        if (connectedState == 1) {
            gColosseumBattleConnect.busyRequest = 0;
        }
        gColosseumBattleConnect.reserved = 1;
        shouldSkipWait = 0;
    }

    if (shouldSkipWait == 0) {
        while (colosseumBattleGetConnectState() != 2) {
            _threadSwitch();
        }
    }

    if (listCount > 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }

    GSresReleaseById(0x70B);
    fn_800F915C(0x70B);
    if (scriptCheckTemochiPokemon(0) != 0) {
        std078_flagSetFalse(0xDDE);
    }
}
