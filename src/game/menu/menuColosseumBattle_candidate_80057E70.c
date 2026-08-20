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
extern u8 fn_8017B07C();
extern u8 fn_8017B13C(s32, u32);
extern void fn_8017B1CC(s32);
extern s32 fn_8017B2CC(s32);
extern void fn_800F915C(s32);
extern u8 pokemonCheckValid(void*);
extern u16 pokemonBiosGetPokemonDataId(void*);
extern u8 pokemonGetAnnonKatati();
extern u32 pokemonGetStatus(void*, s32, s32, s32);
extern void* fn_800F92D4(u32);
extern const u32 lbl_802676F0[56];

void fn_80057E70(void)
{
    u32* entry;
    u8 firstRequest = 1;

    lbl_803A9A08.running = 1;
    lbl_803A9A08.request = 0;
    lbl_803A9A08.ready = 0;
    lbl_803A9A08.canceled = 0;

    for (entry = lbl_8047A590; entry != NULL && *entry != 0 &&
         !lbl_803A9A08.canceled; entry++) {
        if (!firstRequest && fn_8017B07C(0x48A) != 0) {
            break;
        }
        if (fn_8017B13C(0x48A, *entry) != 0) {
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

    lbl_803A9A08.running = 0;
    if (lbl_803A9A08.canceled) {
        fn_8017B1CC(0x48A);
        fn_800F915C(0x48A);
    }
}

void* fn_80057F94(void* pokemon)
{
    u32 id;
    u8 form;

    if (!pokemonCheckValid(pokemon)) {
        return NULL;
    }

    id = pokemonBiosGetPokemonDataId(pokemon);
    if (id == 0) {
        return (void*)-1;
    }

    if (id == 0xC9) {
        form = pokemonGetAnnonKatati(pokemonGetStatus(pokemon, 0, 0x6F, 0));
        if (form >= 28) {
            return (void*)-1;
        }
        if (pokemonBiosGetPokemonDataId(pokemon) == 0) {
            return (void*)-1;
        }
        if (pokemonGetStatus(pokemon, 0, 0xC1, 0) != 0) {
            id = lbl_802676F0[form * 2 + 1];
        } else {
            id = lbl_802676F0[form * 2];
        }
    } else {
        if (pokemonGetStatus(0, id, 0x5A,
                             pokemonGetStatus(pokemon, id, 0xC1, 0) != 0) == 0) {
            return (void*)-1;
        }
    }

    if (id == (u32)-1 || fn_8017B07C(0x48A, id) == 0) {
        return NULL;
    }
    return fn_800F92D4(id);
}
