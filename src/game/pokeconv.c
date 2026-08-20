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
    extern u8 fn_80123FBC(void*);
    extern u16 fn_8011F5C8(void*);
    extern void fn_8008AE18(void*, void*);
    extern u8 fn_80265F14(s32);
    extern void* memset(void*, s32, u32);
    extern const char lbl_8026F568[];
    extern const char lbl_8026F574[];
    extern void fn_80196E10(const char*, u32, const char*);
    u32 word0;
    u32 word1;
    u32 word2;
    u32 packed;
    u32 count4;
    u32 i;
    u16 count;
    const u8* input;
    u8* output;

    if (pokemon != NULL && fn_80123FBC(pokemon) == 0) {
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
        packed |= (u32)fn_8011F5C8(pokemon) << 16;
    }
    *(u32*)(destination + 0xC) = BSWAP32(packed);

    count = *(const u16*)(source + 0xE);
    i = 0;
    if (count == 0x32 || count == 0x1E) {
        i = 1;
    }
    if (i == 0) {
        fn_80196E10(lbl_8026F568, 0xB7, lbl_8026F574);
    }

    input = source + 0x10;
    output = destination + 0x10;
    if ((s32)count > 0) {
        count4 = count >> 2;
        while (count4 != 0) {
            *(u32*)output = BSWAP32(*(const u32*)input);
            input += 4;
            output += 4;
            *(u32*)output = BSWAP32(*(const u32*)input);
            input += 4;
            output += 4;
            *(u32*)output = BSWAP32(*(const u32*)input);
            input += 4;
            output += 4;
            *(u32*)output = BSWAP32(*(const u32*)input);
            input += 4;
            output += 4;
            count4--;
        }
        count &= 3;
        while (count != 0) {
            *(u32*)output = BSWAP32(*(const u32*)input);
            input += 4;
            output += 4;
            count--;
        }
    }

    if (pokemon != NULL) {
        fn_8008AE18(pokemon, output);
        memset(output + 0x64, 0, 0xC);
        for (i = 0; i < 11; i++) {
            output[0x64 + i] = fn_80265F14(i);
        }
    }
    return 1;
}

/* fn_80089380 - 0x80089380 | size: 0x224 */
s32 fn_80089380(u8* destination, const u8* source)
{
    extern u32 lbl_8047A660;
    extern u32 lbl_8047A664;
    extern u32 lbl_8047A668;
    extern u32 lbl_8047A66C;
    extern const char lbl_8026F568[];
    extern const char lbl_8026F574[];
    extern void __assert(const char*, u32, const char*);
    u32 value;
    u32 swapped;
    u32 word0;
    u32 word1;
    u32 word2;
    u32 valid;
    u32 i;
    const u8* input;
    u8* output;

    word0 = *(const u32*)(source + 0);
    input = source + 0x10;
    word1 = *(const u32*)(source + 4);
    valid = 0;
    word2 = *(const u32*)(source + 8);
    swapped = word0 << 24;
    swapped |= (word0 & 0x00FF0000) << 8;
    swapped |= (word0 & 0x0000FF00) >> 8;
    swapped |= word0 >> 24;
    *(u32*)(destination + 0) = swapped;

    swapped = word1 << 24;
    swapped |= (word1 & 0x00FF0000) << 8;
    swapped |= (word1 & 0x0000FF00) >> 8;
    swapped |= word1 >> 24;
    *(u32*)(destination + 4) = swapped;

    swapped = word2 << 24;
    swapped |= (word2 & 0x00FF0000) << 8;
    swapped |= (word2 & 0x0000FF00) >> 8;
    swapped |= word2 >> 24;
    *(u32*)(destination + 8) = swapped;

    value = *(const u32*)(source + 0xC);
    swapped = value << 24;
    swapped |= (value & 0x00FF0000) << 8;
    swapped |= (value & 0x0000FF00) >> 8;
    swapped |= value >> 24;
    *(u16*)(destination + 0xC) = swapped >> 16;
    *(u16*)(destination + 0xE) = swapped;

    if (*(u16*)(destination + 0xE) == 0x32 ||
        *(u16*)(destination + 0xE) == 0x1E)
    {
        valid = 1;
    }
    if (valid == 0) {
        __assert(lbl_8026F568, 0x6F, lbl_8026F574);
    }

    output = destination;
    for (i = 0; i < *(u16*)(destination + 0xE); i++) {
        value = *(const u32*)input;
        input += 4;
        swapped = value << 24;
        swapped |= (value & 0x00FF0000) << 8;
        swapped |= (value & 0x0000FF00) >> 8;
        swapped |= value >> 24;
        *(u16*)(output + 0x10) = swapped;
        *(u16*)(output + 0x12) = swapped >> 16;
        output += 4;
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

/* fn_800895A4 - 0x800895A4 | size: 0x114 */
void fn_800895A4(u8* hero, u8* source) {
    extern void fn_8008BBDC(void*, u8*);
    extern void heroBiosSetHomePlace(u8*, u8);
    extern void heroBiosSetSexDataId(u8*, u8);
    extern void heroBiosSetRnd(u8*, u32);
    extern void heroBiosSetNamePtr(u8*, void*);
    extern void* heroBiosGetPokemonPtr(u8*, u16);
    extern u32 fn_80135938(s32, s32);
    extern u32 fn_800F9C04(void*, u8*, u32, u32);
    extern void fn_8011D494(void*, u16);
    extern void exribbonSetNo(s32, u8);
    u8 name[0x28];
    u32 value;
    u32 i;
    void* pokemon;

    if ((source[0] & 4) != 0) {
        heroBiosSetHomePlace(hero, 2);
    } else {
        heroBiosSetHomePlace(hero, 1);
    }
    fn_800F9C04(name, source + 4, 7, fn_80135938(0, 5));
    heroBiosSetNamePtr(hero, name);
    heroBiosSetSexDataId(hero, source[0xC]);

    value = *(u32*)(source + 0x10);
    value = (value << 24) | ((value & 0xFF00) << 8)
          | ((value & 0xFF0000) >> 8) | (value >> 24);
    heroBiosSetRnd(hero, value);

    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(hero, (u16)i);
        fn_8008BBDC(pokemon, source + 0x14 + i * 0x64);
        fn_8011D494(pokemon, (u16)i);
    }
    for (i = 0; i < 11; i++) {
        exribbonSetNo(i, source[0x26C + i]);
    }
}

#undef BSWAP32
