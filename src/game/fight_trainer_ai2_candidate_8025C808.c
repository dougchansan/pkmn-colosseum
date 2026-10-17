/**
 * @file fight_trainer_ai2_candidate_8025C808.c
 * @brief Semantic candidate for fightTrainerAiCheckAbiCnt.
 */

#include "game/colosseum.h"

extern void* fightTargetGetPtrAsNowFightType(u32, u32);
extern u8 fightSideIsJoutaiDataId(void*, u32);
extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
extern s8 fightSeqCondChgActParaIdToValue(u32);
extern u32 fightSeqCondChgActTypeToPokemonStatusId(u32);
extern u8 fightIsWazaBlockedByMamoru(u32, u32);
extern u8 fn_80237F74(void*, u32, u32);
extern u32 pokemonGetStatus(u32, u32, u32, u32);

u32 fightTrainerAiCheckAbiCnt(
    void* trainer,
    u32 firstPokemon,
    u32 secondPokemon,
    u32 actionId,
    u32 conditionParam,
    u32 conditionType,
    u32 flags)
{
    u32 pokemon;
    void* side;
    u32 statusId;
    u8 ignoreAbility;
    u8 forceCheck;
    u8 mode;
    s8 abilityCount;
    s8 threshold;

    ignoreAbility = 0;
    forceCheck = 0;
    switch (flags & 0x40) {
    default:
        pokemon = firstPokemon;
        break;
    case 0:
        pokemon = secondPokemon;
        break;
    }
    side = fightTargetGetPtrAsNowFightType(2, pokemon);
    mode = flags;
    /* RULE-EXCEPTION(user-approved): codegen-only (u8) cast keeps the 0xBF mask before the bit-7 test as retail does — see docs/RULE_EXCEPTIONS.md */
    if ((u8)(mode & 0xBF) & 0x80) {
        ignoreAbility = 1;
    }
    if (mode & 0x20) {
        forceCheck = 1;
    }

    statusId = fightSeqCondChgActTypeToPokemonStatusId(conditionType);
    abilityCount = (s8)pokemonGetStatus(pokemon, 0, statusId, 0);
    threshold = fightSeqCondChgActParaIdToValue(conditionParam);

    if (threshold < 0) {
        u8 guarded;
        u32 value;

        if (fightSideIsJoutaiDataId(side, 0x4C) == 1 &&
            ignoreAbility == 0 && (u16)actionId != 0xAE) {
            return 0;
        }

        if ((u16)actionId != 0xAE && forceCheck != 1) {
            value = fightTrainerGetStatus(trainer, 0, 0x43, 0) & 0xFFFF;
            value = fightTrainerGetStatus(0, value, 2, 0) & 0xFFFF;
            if ((u8)fightTrainerGetStatus(0, value, 0x24, 0) == 1 &&
                fightIsWazaBlockedByMamoru(secondPokemon, actionId) == 1) {
                guarded = 1;
            } else {
                guarded = 0;
            }
            if (guarded == 1) {
                return 0;
            }
        }

        if ((fn_80237F74(trainer, pokemon, 0x1D) == 1 ||
             fn_80237F74(trainer, pokemon, 0x49) == 1) &&
            ignoreAbility == 0 && (u16)actionId != 0xAE) {
            return 0;
        }
        if (fn_80237F74(trainer, pokemon, 0x33) == 1 &&
            ignoreAbility == 0 && (u16)statusId == 0xEB) {
            return 0;
        }
        if (fn_80237F74(trainer, pokemon, 0x34) == 1 &&
            ignoreAbility == 0 && (u16)statusId == 0xE6) {
            return 0;
        }
        if (fn_80237F74(trainer, pokemon, 0x13) == 1 &&
            (mode & 0x1F) == 0) {
            return 0;
        }
        if (abilityCount > 0) {
            goto ok;
        }
        return 0;
    }

    if (abilityCount < 12) {
        goto ok;
    }
    return 0;
ok:
    return 1;
}
