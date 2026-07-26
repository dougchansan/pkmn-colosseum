/**
 * @file menuCB_range_800697F4.c
 * @brief Residual menuCB candidate range, 0x800697F4 - 0x80069A08.
 */
#define MENUCB_RANGE_RESIDUAL_800697F4_ONLY
#include "menuCB_range_80062948.c"

typedef struct PokemonEntryLoadTexture {
    u8 active;
    u8 _1;
    u16 face;
    u8 _4[8];
} PokemonEntryLoadTexture;

typedef struct PokemonEntryLoadWork {
    u8 _0[0x2C];
    s32 index;
    PokemonEntryLoadTexture entries[4][6];
    u8 _150[0xCC34];
    u8 complete;
} PokemonEntryLoadWork;

void* _menuCBPokemonEntryLoadCallBack__FPv(void* callbackFlag)
{
    extern s32 toolentryTaisenGetEntryPlayerNum(void);
    extern s32 toolentryTaisenGetBattleType(void);
    extern s32 menuCBBattleStartGetStatus(void);
    extern void* toolentryTaisenGetPokemonPtr(s32, s32);
    extern u16 toolentryTaisenGetEntryPokemonNum(s32);
    extern void* toolentryTaisenGetEntryPokemonPtr(s32, s32);
    extern u8 pokemonCheckValid(void*);
    extern u16 fn_8010BBB8(void*);
    PokemonEntryLoadWork* work;
    PokemonEntryLoadTexture* texture;
    void* pokemon;
    s32 loading;
    s32 player;
    s32 slot;

    loading = 1;
    toolentryTaisenGetEntryPlayerNum();
    toolentryTaisenGetBattleType();
    if (callbackFlag != 0) {
        *(u8*)callbackFlag = 1;
    }

    work = (PokemonEntryLoadWork*)lbl_803A9F08;
    while (loading != 0) {
        if (work->index == 24) {
            work->complete = 1;
            loading = 0;
            pokemon = 0;
        } else {
            player = work->index / 6;
            slot = work->index % 6;
            if (menuCBBattleStartGetStatus() == 0) {
                pokemon = toolentryTaisenGetPokemonPtr(player, slot);
            } else if (toolentryTaisenGetEntryPokemonNum(player) <= slot) {
                pokemon = 0;
            } else {
                pokemon = toolentryTaisenGetEntryPokemonPtr(player, slot);
            }

            texture = &work->entries[player][slot];
            if (pokemonCheckValid(pokemon) != 0) {
                texture->active = 1;
                texture->face = fn_8010BBB8(pokemon);
                loading = 0;
            } else {
                texture->active = 0;
            }
            work->index++;
        }
    }
    return pokemon;
}
