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
    extern u16 pokemonGetFaceSpriteId(void*);
    PokemonEntryLoadWork* work;
    u8* complete;
    PokemonEntryLoadTexture* texture;
    PokemonEntryLoadTexture* row;
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

    work = (PokemonEntryLoadWork*)&lbl_803A9F08;
    complete = &work->complete;
    do {
        if (work->index == 24) {
            *complete = 1;
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

            row = ((PokemonEntryLoadWork*)&lbl_803A9F08)->entries[player];
            texture = &row[slot];
            if (pokemonCheckValid(pokemon) != 0) {
                u16 face = pokemonGetFaceSpriteId(pokemon);

                texture->active = 1;
                loading = 0;
                texture->face = face;
                work->index++;
            } else {
                texture->active = 0;
                work->index++;
            }
        }
    } while (loading != 0);
    return pokemon;
}

/* Address: 0x80069944 | Size: 0xC4 */
void menuCBPokemonEntryTexWorkInit(void)
{
    PokemonEntryTexWork* e0;
    PokemonEntryTexWork* e5;
    PokemonEntryTexWork* e4;
    PokemonEntryTexWork* e3;
    PokemonEntryTexWork* e2;
    PokemonEntryTexWork* e1;
    s32 i;
    PokemonEntryTexWorkAll* work;

    work = &lbl_803A9F08;
    work->index = 0;
    work->complete = 0;
    for (i = 0; i < 4; i++) {
        e0 = work->entries[i];
        e1 = e0 + 1;
        e2 = e0 + 2;
        e3 = e0 + 3;
        e4 = e0 + 4;
        e5 = e0 + 5;
        e0->active = 0;
        e0->value = 0;
        e1->active = 0;
        e1->value = 0;
        e2->active = 0;
        e2->value = 0;
        e3->active = 0;
        e3->value = 0;
        e4->active = 0;
        e4->value = 0;
        e5->active = 0;
        e5->value = 0;
    }
}
