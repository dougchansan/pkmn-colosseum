/**
 * @file field_range_80089048.c
 * @brief field code, 0x80089048 - 0x800896B8 (3 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

#define BSWAP32(value)                                                        \
    (((value) << 24) | (((value) & 0x0000FF00) << 8)                         \
     | (((value) & 0x00FF0000) >> 8) | ((value) >> 24))

/* fn_80089048 - 0x80089048 | size: 0x338 */
s32 fn_80089048(u8* destination, const u8* source, void* pokemon)
{
    extern u8 pokemonCheckValid(void*);
    extern u16 pokemonBiosGetPokemonDataId(void*);
    extern void pokeconvPokemonToGBA(void*, void*);
    extern u8 exribbonGetNo(s32);
    extern void* memset(void*, s32, u32);
    extern const char lbl_8026F568[];
    extern const char lbl_8026F574[];
    extern void __assert(const char*, u32, const char*);
    u32 word0;
    u32 word1;
    u32 word2;
    u32 packed;
    u32 value;
    s32 valid;
    s32 i;
    s32 count;
    u32 items;
    const u8* input;
    u8* output;
    u8* ribbons;

    if (pokemon != NULL && pokemonCheckValid(pokemon) == 0) {
        return 0;
    }

    word0 = *(const u32*)(source + 0);
    word1 = *(const u32*)(source + 4);
    word2 = *(const u32*)(source + 8);
    *(u32*)(destination + 0) = BSWAP32(word0);
    *(u32*)(destination + 4) = BSWAP32(word1);
    *(u32*)(destination + 8) = BSWAP32(word2);

    packed = *(const u16*)(source + 0xE);
    if (pokemon != NULL) {
        packed |= (u32)pokemonBiosGetPokemonDataId(pokemon) << 16;
    }
    items = *(const u16*)(source + 0xE);
    *(u32*)(destination + 0xC) = BSWAP32(packed);
    output = destination + 0x10;

    valid = 0;
    if (items == 0x32 || items == 0x1E) {
        valid = 1;
    }
    if (valid == 0) {
        __assert(lbl_8026F568, 0xB7, lbl_8026F574);
    }

    input = source;
    count = *(const u16*)(source + 0xE);
    for (i = count; i > 0; i--) {
        value = *(const u16*)(input + 0x10);
        value |= *(const u16*)(input + 0x12) << 16;
        input += 4;
        value = BSWAP32(value);
        *(u32*)output = value;
        output += 4;
    }
    if (pokemon != NULL) {
        pokeconvPokemonToGBA(pokemon, output);
        memset(output + 0x64, 0, 0xC);
        ribbons = output + 0x64;
        for (i = 0; i < 11; i++) {
            *ribbons++ = exribbonGetNo(i);
        }
    }
    return 1;
}

/* fn_80089380 - 0x80089380 | size: 0x224 */
s32 fn_80089380(u8* destination, const u8* source)
{
    extern volatile s32 lbl_8047A660;
    extern s32 lbl_8047A664;
    extern s32 lbl_8047A668;
    extern s32 lbl_8047A66C;
    extern const char lbl_8026F568[];
    extern const char lbl_8026F574[];
    extern void __assert(const char*, u32, const char*);
    u32 word0;
    u32 word1;
    u32 word2;
    u32 word3;
    u32 value;
    s32 valid;
    s32 i;
    const u8* input;

    word0 = *(const u32*)(source + 0);
    input = source + 0x10;
    word1 = *(const u32*)(source + 4);
    valid = 0;
    word2 = *(const u32*)(source + 8);
    word3 = *(const u32*)(source + 0xC);
    *(u32*)(destination + 0) = BSWAP32(word0);
    *(u32*)(destination + 4) = BSWAP32(word1);
    *(u32*)(destination + 8) = BSWAP32(word2);
    value = BSWAP32(word3);
    *(u16*)(destination + 0xC) = value >> 16;
    *(u16*)(destination + 0xE) = value;

    if (*(u16*)(destination + 0xE) == 0x32 || *(u16*)(destination + 0xE) == 0x1E) {
        valid = 1;
    }
    if (valid == 0) {
        __assert(lbl_8026F568, 0x6F, lbl_8026F574);
    }

    for (i = 0; i < *(u16*)(destination + 0xE); i++) {
        value = *(const u32*)input;
        input += 4;
        value = BSWAP32(value);
        *(u16*)(destination + 0x10 + i * 4) = value;
        *(u16*)(destination + 0x12 + i * 4) = value >> 16;
    }

    if (lbl_8047A664 != 0) {
        *(u32*)(destination + 0) = 0;
        *(u32*)(destination + 4) = 0;
        lbl_8047A664 = 0;
    }
    if (lbl_8047A660 != 0) {
        *(u32*)(destination + 0) += lbl_8047A660;
        *(u32*)(destination + 4) += lbl_8047A660;
        lbl_8047A660 = 0;
    }
    if (lbl_8047A66C != 0) {
        *(u32*)(destination + 8) = 0;
        lbl_8047A66C = 0;
    }
    if (lbl_8047A668 != 0) {
        *(u32*)(destination + 8) |= 0x10;
        lbl_8047A668 = 0;
    }
    return 1;
}

/* gbaReceiveHero - 0x800895A4 | size: 0x114 */
void gbaReceiveHero(u8* hero, u8* source) {
    extern void pokeconvPokemonFromGBA(void*, u8*);
    extern void heroBiosSetHomePlace(u8*, u8);
    extern void heroBiosSetSexDataId(u8*, u8);
    extern void heroBiosSetRnd(u8*, u32);
    extern void heroBiosSetNamePtr(u8*, void*);
    extern void* heroBiosGetPokemonPtr(u8*, u16);
    extern u32 gamedataGetStatus(s32, s32);
    extern u32 GScharFromGBA(void*, u8*, u32, u32);
    extern void pokemonBiosSetFightTrainerPokemonDataId(void*, u16);
    extern void exribbonSetNo(s32, u8);
    u16 name[8];
    u32 value;
    s32 i;
    void* pokemon;

    heroBiosSetHomePlace(hero, (source[0] & 4) ? 2 : 1);
    GScharFromGBA(name, source + 4, 7, gamedataGetStatus(0, 5));
    heroBiosSetNamePtr(hero, name);
    heroBiosSetSexDataId(hero, source[0xC]);
    value = *(u32*)(source + 0x10);
    heroBiosSetRnd(hero, (value << 24) | ((value & 0xFF00) << 8)
                         | ((value & 0xFF0000) >> 8) | (value >> 24));
    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(hero, i);
        pokeconvPokemonFromGBA(pokemon, source + 0x14 + i * 0x64);
        pokemonBiosSetFightTrainerPokemonDataId(pokemon, i);
    }
    for (i = 0; i < 11; i++) {
        exribbonSetNo(i, source[0x26C + i]);
    }
}

#undef BSWAP32
