/**
 * gba_misc 0x80089978 - 0x80089B8C: copy one GBA party entry into a
 * fight-trainer Pokemon record. Same code as gbaBuildTrainerPokemon in gba_misc.c,
 * kept as a standalone unit (like the gba_misc_exact_* siblings) so it
 * can be linked.
 */
#include "dolphin/types.h"

/* 0x80089978 | size: 0x214 */
u8 gbaBuildTrainerPokemon(u8* pokemon, u8* entry) {
    extern u8 darkPokemonCheckFlag12(u8 id);
    extern void darkPokemonSaveSet4A6(u8 id, u8 value);
    extern void fightTrainerPokemonDataBiosSetPokemonDataId(u8* pokemon, u16 id);
    extern void fightTrainerPokemonDataBiosSetNickname(u8* pokemon, void* name);
    extern void fightTrainerPokemonDataBiosSetDarkPokemonFlag(u8* pokemon, u8 flag);
    extern void fightTrainerPokemonDataBiosSetLevel(u8* pokemon, u8 level);
    extern void fightTrainerPokemonDataBiosSetWazaDataId(u8* pokemon, u8 slot, u16 id);
    extern void fightTrainerPokemonDataBiosSetItemDataId(u8* pokemon, u16 id);
    extern void fightTrainerPokemonDataBiosSetTokuseiFlag(u8* pokemon, u8 flag);
    extern void fightTrainerPokemonDataBiosSetStatusRnd(u8* pokemon, s32 stat, u8 value);
    extern void fightTrainerPokemonDataBiosSetStatusEffort(u8* pokemon, s32 stat, s16 value);
    extern void fightTrainerPokemonDataBiosSetFriend(u8* pokemon, s16 value);
    extern void fightTrainerPokemonDataBiosSetSexDataId(u8* pokemon, s8 id);
    extern void fightTrainerPokemonDataBiosSetSeikakuDataId(u8* pokemon, u8 id);
    extern void fightTrainerPokemonDataBiosSetKeyPlayerFlag(u8* pokemon, u8 flag);
    extern void fightTrainerPokemonDataBiosSetPartDataId(u8* pokemon, u8 id);
    s32 i;
    u8 skip;

    if (entry[2] != 0 && darkPokemonCheckFlag12(entry[2]) == 1) {
        skip = 1;
    } else {
        skip = 0;
    }
    if (skip == 1) {
        fightTrainerPokemonDataBiosSetPokemonDataId(pokemon, 0);
        return 0;
    }

    fightTrainerPokemonDataBiosSetPokemonDataId(pokemon, *(u16*)(entry + 0x00));
    fightTrainerPokemonDataBiosSetNickname(pokemon, NULL);
    fightTrainerPokemonDataBiosSetDarkPokemonFlag(pokemon, entry[2]);
    if (entry[2] != 0) {
        darkPokemonSaveSet4A6(entry[2], entry[0x28]);
    }
    fightTrainerPokemonDataBiosSetLevel(pokemon, entry[3]);
    for (i = 0; i < 4; i++) {
        fightTrainerPokemonDataBiosSetWazaDataId(pokemon, i, *(u16*)(entry + 4 + i * 2));
    }
    fightTrainerPokemonDataBiosSetItemDataId(pokemon, *(u16*)(entry + 0x0C));
    fightTrainerPokemonDataBiosSetTokuseiFlag(pokemon, entry[0x0E]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 0, entry[0x0F]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 1, entry[0x10]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 2, entry[0x11]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 3, entry[0x12]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 4, entry[0x13]);
    fightTrainerPokemonDataBiosSetStatusRnd(pokemon, 5, entry[0x14]);
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 0, *(s16*)(entry + 0x16));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 1, *(s16*)(entry + 0x18));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 2, *(s16*)(entry + 0x1A));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 3, *(s16*)(entry + 0x1C));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 4, *(s16*)(entry + 0x1E));
    fightTrainerPokemonDataBiosSetStatusEffort(pokemon, 5, *(s16*)(entry + 0x20));
    fightTrainerPokemonDataBiosSetFriend(pokemon, *(s16*)(entry + 0x22));
    fightTrainerPokemonDataBiosSetSexDataId(pokemon, (s8)entry[0x24]);
    fightTrainerPokemonDataBiosSetSeikakuDataId(pokemon, entry[0x25]);
    fightTrainerPokemonDataBiosSetKeyPlayerFlag(pokemon, entry[0x26]);
    fightTrainerPokemonDataBiosSetPartDataId(pokemon, entry[0x27]);
    return 1;
}
