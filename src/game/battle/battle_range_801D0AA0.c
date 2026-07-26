/**
 * @file battle_range_801D0AA0.c
 * @brief battle-domain (direct calls into battle_*.c), 0x801D0AA0 - 0x801D1470.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

u8 fn_801D0AA0(u32 index)
{
    extern void* savedataGetStatus(u32, u32);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    extern u32 pokemonCheckValid(void*);
    extern u8 pokemonBiosGetCatchBallId(void*);
    void* pokemon;

    pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)index);
    if ((u8)pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    return pokemonBiosGetCatchBallId(pokemon);
}

u16 fn_801D0AFC(s32 mode)
{
    extern u16 lbl_8047B3D8;
    extern void* savedataGetStatus(u32, u32);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    extern u8 pokemonCheckValid(void*);
    extern u32 pokemonGetStatus(void*, u32, u32, u32);
    extern u8 pokemonBiosGetCatchBallId(void*);
    extern void pokemonAllKaihuku(void*);
    u32 i;
    u32 status;
    u8 valid;
    void* pokemon;

    lbl_8047B3D8 = i = 0;
    while ((u16)i < 6) {
        valid = 1;
        pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i);
        if (!pokemonCheckValid(pokemon)) {
            valid = 0;
        } else {
            status = pokemonGetStatus(pokemon, 0, 0x6E, 0);
            pokemonGetStatus(pokemon, (u16)status, 1, 0);
            pokemonBiosGetCatchBallId(pokemon);
            pokemonAllKaihuku(pokemon);
        }
        if (valid) {
            lbl_8047B3D8++;
        }
        i++;
    }
    return lbl_8047B3D8;
}

void fn_801D0BD0(s32 arg0, s32 arg1, s32 mode)
{
    extern void fn_801D0C30(void);
    extern void fn_801D0DB0(s32, s32);

    switch (mode) {
    case 0:
        fn_801D0DB0(arg0, arg1);
        break;
    case 1:
        fn_801D0C30();
        break;
    case 2:
        fn_801D0C30();
        break;
    case 3:
        fn_801D0C30();
        break;
    }
}

void fn_801D0C30(void)
{
    extern f32 lbl_8047E184;
    extern void _threadSwitch(void);
    extern void fadeSet(f32 duration, s32 mode);
    extern s8 fadeCheck(s32 wait);
    extern u32 fn_8016557C(void);
    extern void fn_80165548(u32 handle);
    extern void fn_801653CC(s32 id, s32 fade, s32 volume);
    s32 running;
    s32 state;
    s32 timer;
    u32 handle;

    running = 1;
    state = 0;
    timer = 0;
    do {
        switch (state) {
        case 0:
            timer = 0;
            fn_801D0AFC(1);
            state = 10;
            break;
        case 10:
            fadeSet(lbl_8047E184, 3);
            state = 12;
            handle = fn_8016557C();
            break;
        case 12:
            if (fadeCheck(0) == 0) {
                state = 6;
            } else {
                _threadSwitch();
            }
            break;
        case 6:
            timer++;
            if (timer >= 10) {
                state = 8;
                fn_801653CC(0x19, 2000, 0xFF);
                timer = 0;
            } else {
                _threadSwitch();
            }
            break;
        case 8:
            timer++;
            if (timer >= 10) {
                state = 9;
            } else {
                _threadSwitch();
            }
            break;
        case 9:
            fn_80165548(handle);
            fadeSet(lbl_8047E184, 2);
            state = 11;
            break;
        case 11:
            if (fadeCheck(0) == 0) {
                state = 1000;
            } else {
                _threadSwitch();
            }
            break;
        case 1000:
            running = 0;
            break;
        }
    } while (running != 0);
}
