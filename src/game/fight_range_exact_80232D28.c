/**
 * @file fight_range_exact_80232D28.c
 * @brief Exact island 0x80232D28 - 0x80232FE4 (_fightCheckLockOnSub), moved out of
 *        fight_range_80211A00.c.
 */
#include "dolphin/types.h"

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimize_for_size on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimize_for_size on
u32 _fightCheckLockOnSub(s32 pokemonArg, u32 hostSide, u32* data)
{
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern void fightOutPokemonSetOumuWazaDataId();
    extern u32 fightOutPokemonGetJoutaiParam();
    extern void fn_80201FDC();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonIsAlly();
    extern void fightOutPokemonEnemyClearOumu();
    extern u8 fightOutPokemonCheckFightOut();
    u32 pokemon;
    u16 statusTarget;
    u32 other;
    u32 targetId;
    u32 relative;
    u8 matched;

    pokemon = pokemonArg;
    statusTarget = (u16)data[1];
    other = data[0];
    if (fightOutPokemonCheckFightOut() == 0) {
        return 1;
    }
    if (pokemon == other) {
        return 1;
    }

    if (statusTarget == 0x7f) {
        if (fightOutPokemonIsAlly(other, pokemon) == 0) {
            if (fightOutPokemonIsPokemonJoutaiDataId(pokemon, 0x1d) == 1 &&
                (targetId = fightOutPokemonGetJoutaiParam(pokemon, 0x1d), (targetId & 0xffff) != 0) &&
                (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                     targetId, hostSide)) != 0 &&
                relative == other) {
                matched = 1;
            } else {
                matched = 0;
            }
            if (matched == 1) {
                fn_80201FDC(pokemon, 0x1d, 0);
            }
        }
    } else {
        if (fightOutPokemonIsPokemonJoutaiDataId(pokemon, 0x16) == 1 &&
            (targetId = fightOutPokemonGetJoutaiParam(pokemon, 0x16), (targetId & 0xffff) != 0) &&
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
        if (fightOutPokemonIsPokemonJoutaiDataId(pokemon, 0x1d) == 1 &&
            (targetId = fightOutPokemonGetJoutaiParam(pokemon, 0x1d), (targetId & 0xffff) != 0) &&
            (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                 targetId, hostSide)) != 0 &&
            relative == other) {
            matched = 1;
        } else {
            matched = 0;
        }
        if (matched == 1) {
            fightOutPokemonWriteJoutaiDataId(pokemon, 0x1d);
        }
    }
    if (fightOutPokemonIsPokemonJoutaiDataId(pokemon, 10) == 1 &&
        (targetId = fightOutPokemonGetJoutaiParam(pokemon, 10), (targetId & 0xffff) != 0) &&
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
    if (fightOutPokemonIsPokemonJoutaiDataId(pokemon, 0xe) == 1 &&
        (targetId = fightOutPokemonGetJoutaiParam(pokemon, 0xe), (targetId & 0xffff) != 0) &&
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
        fightOutPokemonEnemyClearOumu(pokemon, other);
    }
    return 1;
}
#pragma optimize_for_size reset
