/**
 * @file fight_range_exact_80232FE4.c
 * @brief Exact pure-C fight relationship cleanup island.
 */
#include "dolphin/types.h"

#pragma optimize_for_size on
u32 fn_80232FE4(u32 r3, u32 r4, u32 r5)
{
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern void fightOutPokemonSetOumuWazaDataId();
    extern u32 fn_80201D84();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonIsAlly();
    extern void fn_80203198();
    extern u8 fightOutPokemonCheckFightOut();
    u8 matched;
    u32 targetId;
    u32 relative;
    struct {
        u32 h;
        void* p;
        void* o;
    } values;
#define hostSide values.h
#define other ((u32)values.o)
#define pokemon ((u32)values.p)

    values.h = r4;
    values.p = (void*)r3;
    values.o = (void*)r5;

    if (fightOutPokemonCheckFightOut() == 0) {
        return 1;
    }
    if (pokemon == other) {
        return 1;
    }
    if (fn_802026E4(pokemon, 0x16) == 1 &&
        (targetId = fn_80201D84(pokemon, 0x16), (targetId & 0xffff) != 0) &&
        (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
             targetId, hostSide)) != 0 &&
        relative == other) {
        matched = 1;
    } else {
        matched = 0;
    }
    if (matched == 1) {
        fightOutPokemonWriteJoutaiDataId(pokemon, 0x16);
    }
    if (fn_802026E4(pokemon, 10) == 1 &&
        (targetId = fn_80201D84(pokemon, 10), (targetId & 0xffff) != 0) &&
        (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
             targetId, hostSide)) != 0 &&
        relative == other) {
        matched = 1;
    } else {
        matched = 0;
    }
    if (matched == 1) {
        fightOutPokemonWriteJoutaiDataId(pokemon, 10);
    }
    if (fn_802026E4(pokemon, 0xe) == 1 &&
        (targetId = fn_80201D84(pokemon, 0xe), (targetId & 0xffff) != 0) &&
        (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
             targetId, hostSide)) != 0 &&
        relative == other) {
        matched = 1;
    } else {
        matched = 0;
    }
    if (matched == 1) {
        fightOutPokemonWriteJoutaiDataId(pokemon, 0xe);
    }
    if (fightOutPokemonIsAlly(pokemon, other) == 0) {
        fightOutPokemonSetOumuWazaDataId(pokemon, other, 0);
        fn_80203198(pokemon, other);
    }
    return 1;
}
#undef pokemon
#undef other
#undef hostSide
#pragma optimize_for_size reset

#pragma optimize_for_size on
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_802331A4(void* param1, void* param2)
{
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();

    if ((u8)fn_801FECD4(param1, param2) == 1) {
        fn_801FE7EC(param1, param2, 0, 0);
    }
}
#undef fn_801FECD4
#undef fn_801FE7EC
#pragma optimize_for_size reset
