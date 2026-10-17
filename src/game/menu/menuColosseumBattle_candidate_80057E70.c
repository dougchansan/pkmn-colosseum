/**
 * @file menuColosseumBattle_candidate_80057E70.c
 * @brief menuColosseumBattle.cpp candidate range, 0x80057E70 - 0x80058150.
 */
#include "dolphin/types.h"

typedef struct ColosseumConnectJob {
    s32 running;
    s32 ready;
    s32 request;
    s32 canceled;
} ColosseumConnectJob;

extern ColosseumConnectJob lbl_803A9A08;
extern u32* lbl_8047A590;
extern void _threadSwitch(void);
extern u8 fsysIsEntryLoaded(u32 fileHandle, u32 nameHash);
extern u8 fsysRequestEntry(s32, u32);
extern void GSresReleaseById(s32);
extern s32 fn_8017B2CC(s32);
extern void fn_800F915C(s32);
extern u8 pokemonCheckValid(void*);
extern u16 pokemonBiosGetPokemonDataId(void*);
extern u8 pokemonGetAnnonKatati();
extern u32 pokemonGetStatus(void*, u16, s32, u16);
extern void* GSresFindById(u32);
extern const u32 lbl_802676F0[56];

#ifndef MENU_COLOSSEUM_BATTLE_80057F94_ONLY
void fn_80057E70(void)
{
    u32* entry;
    u8 firstRequest = 1;

    lbl_803A9A08.running = 1;
    lbl_803A9A08.request = 0;
    lbl_803A9A08.ready = 0;
    lbl_803A9A08.canceled = 0;

    if (lbl_8047A590 != NULL) {
        for (entry = lbl_8047A590; *entry != 0 && !lbl_803A9A08.canceled; entry++) {
            /* The loop test leaves the entry hash in r4 for the loaded query. */
            if ((firstRequest || fsysIsEntryLoaded(0x48A, *entry) == 0) &&
                fsysRequestEntry(0x48A, *entry) != 0) {
                firstRequest = 0;
                while (fn_8017B2CC(0x48A) == 1) {
                    _threadSwitch();
                }
                while (lbl_803A9A08.request != 0) {
                    lbl_803A9A08.ready = 1;
                    _threadSwitch();
                }
                lbl_803A9A08.ready = 0;
            }
        }
    }

    lbl_803A9A08.running = 0;
    if (lbl_803A9A08.canceled) {
        GSresReleaseById(0x48A);
        fn_800F915C(0x48A);
    }
}

#endif

#ifndef MENU_COLOSSEUM_BATTLE_80057E70_ONLY
typedef struct ColosseumUnownMenuIds {
    u32 normal;
    u32 shiny;
} ColosseumUnownMenuIds;

typedef struct ColosseumUnownMenuTable {
    ColosseumUnownMenuIds forms[28];
} ColosseumUnownMenuTable;

static inline u32 fn_80057F94_speciesMenuId(void* pokemon, u16 id)
{
    u32 menuId;
    u8 shiny;

    if (id == 0xC9) {
        ColosseumUnownMenuTable table = *(const ColosseumUnownMenuTable*)lbl_802676F0;
        u8 form = pokemonGetAnnonKatati(pokemonGetStatus(pokemon, 0, 0x6F, 0));
        u16 dataId;

        if (form >= 28) {
            return -1;
        }
        dataId = pokemonBiosGetPokemonDataId(pokemon);
        if (dataId == 0) {
            return -1;
        }
        if ((u8)pokemonGetStatus(pokemon, dataId, 0xC1, 0)) {
            return table.forms[form].shiny;
        }
        return table.forms[form].normal;
    }

    shiny = pokemonGetStatus(pokemon, id, 0xC1, 0);
    menuId = pokemonGetStatus(0, id, 0x5A, shiny ? 1 : 0);
    if (menuId == 0) {
        return -1;
    }
    return menuId;
}

void* fn_80057F94(void* pokemon)
{
    u32 menuId;
    u16 id;

    if (!pokemonCheckValid(pokemon)) {
        return NULL;
    }

    id = pokemonBiosGetPokemonDataId(pokemon);
    if (id == 0) {
        menuId = -1;
    } else {
        menuId = fn_80057F94_speciesMenuId(pokemon, id);
    }

    if (menuId == (u32)-1) {
        return NULL;
    }
    if (fsysIsEntryLoaded(0x48A, menuId)) {
        return GSresFindById(menuId);
    }
    return NULL;
}
#endif
