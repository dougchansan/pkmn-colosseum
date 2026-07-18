/**
 * @file pokemon_range_exact_801248C4.c
 * @brief Exact pure-C Pokemon initialization island, 0x801248C4 - 0x801254B4.
 */
#include "dolphin/types.h"

extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
void pokemonResetBasisStatus(void* ptr);

#pragma optimization_level 4
u16 pokemonGetTokuseiDataId(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    u32 val;
    if (ptr == NULL) { return 0; }
    val = pokemonGetStatus(ptr, 0, 0x6e, 0) & 0xFFFF;
    if ((s32)pokemonGetStatus(NULL, val, 0x17, 1) == 0) {
        return (u16)pokemonGetStatus(NULL, val, 0x17, 0);
    }
    return (u16)pokemonGetStatus(NULL, val, 0x17, (u8)pokemonGetStatus(ptr, 0, 0xb7, 0));
}

void pokemonSetTokuseiFlag(u8* ptr, u32 arg2) {
    extern void pokemonSetStatus(u8* ptr, u32 a, u32 b, u32 c, u8 d);
    u16 val1;
    if (ptr == NULL) { return; }
    val1 = (u16)pokemonGetStatus(ptr, 0, 0x6e, 0);
    if ((s32)pokemonGetStatus(0, val1, 0x17, 1) == 0) { arg2 = 0; }
    pokemonSetStatus(ptr, 0, 0xb7, 0, (u8)arg2);
}

void pokemonInitAry(u8* ptr, u16 count) {
    extern void pokemonInit(u8* ptr);
    u16 i;
    if (ptr == NULL) { return; }
    for (i = 0; i < count; i++) {
        pokemonInit(ptr + (u16)i * 0x138);
    }
}

void pokemonInit(u8* ptr) {
    extern void pokemonSetStatus(u8* ptr, u32 a, u32 b, u32 c, u32 d);
    extern u32 pokemonGetStatus(u8* ptr, u32 a, u32 b, u32 c);
    extern void fn_8011B950(u32 base, u16 count);
    extern void gamedataAttestInit(u32 val);
    extern void pokemonBiosSetEventGetFlag(u8* ptr, u8 val);
    u32 i;
    u16 local;
    local = 0;
    if (ptr == NULL) { return; }
    pokemonSetStatus(ptr, 0, 0xc9, 0, 0);
    pokemonSetStatus(ptr, 0, 0xc3, 0, 0);
    if (ptr != NULL) { pokemonSetStatus(ptr, 0, 0xc5, 0, (u32)-0x64); }
    pokemonSetStatus(ptr, 0, 0xc6, 0, 0);
    pokemonSetStatus(ptr, 0, 0xc7, 0, 0);
    fn_8011B950(pokemonGetStatus(ptr, 0, 0xc8, 0), 1);
    pokemonSetStatus(ptr, 0, 0x6e, 0, 0);
    pokemonSetStatus(ptr, 0, 0x6f, 0, 0);
    gamedataAttestInit(pokemonGetStatus(ptr, 0, 0x70, 0));
    if (ptr != NULL) {
        pokemonSetStatus(ptr, 0, 0x71, 0, 0);
        pokemonSetStatus(ptr, 0, 0x72, 0, 0);
        pokemonSetStatus(ptr, 0, 0x73, 0, 0);
        pokemonSetStatus(ptr, 0, 0x74, 0, 2);
        pokemonSetStatus(ptr, 0, 0x75, 0, 0);
        pokemonSetStatus(ptr, 0, 0x76, 0, (u32)&local);
    }
    pokemonSetStatus(ptr, 0, 0x77, 0, (u32)&local);
    pokemonSetStatus(ptr, 0, 0x79, 0, 0);
    pokemonSetStatus(ptr, 0, 0x7a, 0, 0);
    fn_8011B950(pokemonGetStatus(ptr, 0, 0x7c, 0), 1);
    pokemonSetStatus(ptr, 0, 0x7d, 0, 0);
    if (ptr != NULL) {
        for (i = 0; (u16)i < 4; i++) {
            if (ptr != NULL) {
                pokemonSetStatus(ptr, 0, 0x7f, (u32)i, 0);
                pokemonSetStatus(ptr, 0, 0x80, (u32)i, 0);
                pokemonSetStatus(ptr, 0, 0x81, (u32)i, 0);
            }
        }
    }
    if (ptr != NULL) {
        pokemonGetStatus(ptr, 0, 0x82, 0);
        pokemonSetStatus(ptr, 0, 0x82, 0, 0);
    }
    pokemonSetStatus(ptr, 0, 0x83, 0, 0);
    pokemonSetStatus(ptr, 0, 0x87, 0, 0);
    pokemonSetStatus(ptr, 0, 0x88, 0, 0);
    pokemonSetStatus(ptr, 0, 0x89, 0, 0);
    pokemonSetStatus(ptr, 0, 0x8a, 0, 0);
    pokemonSetStatus(ptr, 0, 0x8b, 0, 0);
    pokemonSetStatus(ptr, 0, 0x8c, 0, 0);
    pokemonSetStatus(ptr, 0, 0x8d, 0, 0);
    pokemonSetStatus(ptr, 0, 0x8e, 0, 0);
    pokemonSetStatus(ptr, 0, 0x8f, 0, 0);
    pokemonSetStatus(ptr, 0, 0x90, 0, 0);
    pokemonSetStatus(ptr, 0, 0x91, 0, 0);
    pokemonSetStatus(ptr, 0, 0x92, 0, 0);
    pokemonSetStatus(ptr, 0, 0x93, 0, 0);
    pokemonSetStatus(ptr, 0, 0x94, 0, 0);
    pokemonSetStatus(ptr, 0, 0x95, 0, 0);
    pokemonSetStatus(ptr, 0, 0x96, 0, 0);
    pokemonSetStatus(ptr, 0, 0x97, 0, 0);
    pokemonSetStatus(ptr, 0, 0x98, 0, 0);
    pokemonSetStatus(ptr, 0, 0x99, 0, 0);
    pokemonSetStatus(ptr, 0, 0x9c, 0, 0);
    pokemonSetStatus(ptr, 0, 0x9d, 0, 0);
    pokemonSetStatus(ptr, 0, 0x9e, 0, 0);
    pokemonSetStatus(ptr, 0, 0x9f, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa0, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa1, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa3, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa4, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa5, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa6, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa7, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa8, 0, 0);
    pokemonSetStatus(ptr, 0, 0xa9, 0, 0);
    pokemonSetStatus(ptr, 0, 0xaa, 0, 0);
    pokemonSetStatus(ptr, 0, 0xab, 0, 0);
    pokemonSetStatus(ptr, 0, 0xac, 0, 0);
    pokemonSetStatus(ptr, 0, 0xad, 0, 0);
    pokemonSetStatus(ptr, 0, 0xae, 0, 0);
    pokemonSetStatus(ptr, 0, 0xaf, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb0, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb1, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb2, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb3, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb4, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb5, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb6, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb7, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb8, 0, 0);
    pokemonSetStatus(ptr, 0, 0xb9, 0, 0);
    pokemonSetStatus(ptr, 0, 0xbb, 0, 0);
    pokemonSetStatus(ptr, 0, 0xbc, 0, (u32)0xff);
    pokemonSetStatus(ptr, 0, 0xbd, 0, 0);
    pokemonSetStatus(ptr, 0, 0xbe, 0, 0);
    pokemonBiosSetEventGetFlag(ptr, 0);
}

void pokemonInitDarkPokemon(u8* ptr) {
    extern void pokemonSetStatus(u8* a, u32 b, u32 c, u32 d, u32 e);
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern void fn_8011B950(u8* a, u32 b);
    pokemonSetStatus(ptr, 0, 0xc3, 0, 0);
    if (ptr != NULL) {
        pokemonSetStatus(ptr, 0, 0xc5, 0, (u32)-100);
    }
    pokemonSetStatus(ptr, 0, 0xc6, 0, 0);
    pokemonSetStatus(ptr, 0, 0xc7, 0, 0);
    fn_8011B950((u8*)pokemonGetStatus(ptr, 0, 0xc8, 0), 1);
}

void pokemonInitJoutai(u8* ptr) {
    extern u32 pokemonGetStatus(u8* a, u32 b, u32 c, u32 d);
    extern void fn_8011B950(u8* a, u32 b);
    fn_8011B950((u8*)pokemonGetStatus(ptr, 0, 0x7c, 0), 1);
}

void pokemonWazaInit(u8* ptr, u32 arg2) {
    extern void pokemonSetStatus(u8* a, u32 b, u32 c, u32 d, u32 e);
    if (ptr == NULL) { return; }
    pokemonSetStatus(ptr, 0, 0x7f, arg2, 0);
    pokemonSetStatus(ptr, 0, 0x80, arg2, 0);
    pokemonSetStatus(ptr, 0, 0x81, arg2, 0);
}

#pragma push
#pragma opt_propagation off
u32 pokemonCheckRare(void* ctx) {
    extern u32 pokemonGetStatus(void* a, u32 b, u32 c, u32 d);
    u32 a;
    u32 b;
    u32 eight;
    u32 lb;
    if (ctx == NULL) {
        return 0;
    }
    a = pokemonGetStatus(ctx, 0, 0x75, 0);
    b = pokemonGetStatus(ctx, 0, 0x6F, 0);
    eight = 8;
    lb = b & 0xFFFF;
    return (u32)(eight << __cntlzw((a >> 16) ^ (a & 0xFFFF) ^ (b >> 16) ^ lb ^ eight)) >> 31;
}
#pragma pop

void pokemonGrowBasisStatus(void* ptr, u32 arg2) {
    extern void pokemonSetStatus();
    if (ptr == NULL) { return; }
    pokemonSetStatus(ptr, 0, 0x79, 0, arg2);
    pokemonResetBasisStatus(ptr);
}

void pokemonResetBasisStatus(void* ptr) {
    extern u32 pokemonGetStatus();
    extern void pokemonSetLevelBasisStatus(u8* ptr, u32 level);
    u8 val;
    if (ptr == NULL) { return; }
    val = pokemonGetStatus(ptr, 0, 0xC0, 0);
    pokemonSetLevelBasisStatus(ptr, val);
}
