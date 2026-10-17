/**
 * @file fight_trainer_ai_waza_hit_candidate_80256ED0.c
 * @brief Candidate fightTrainerAiWazaHit.cpp range, 0x80256ED0 - 0x802570D0.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

extern u8 fn_80237F74(void* ctx, u32 pokemon, u32 type);

/* Clear unless the target is under condition 0xC and none of the
 * conditions 0x11, 0x14, 0x7, 0xF, 0x48, 0x29 and 0x28. */
/* RULE-EXCEPTION(user-approved): single-use inline helper — see docs/RULE_EXCEPTIONS.md */
static inline u8 wazaHit120TargetClear(void* ctx, u32 target) {
    if (fn_80237F74(ctx, target, 0x11) != 1 && fn_80237F74(ctx, target, 0x14) != 1 &&
        fn_80237F74(ctx, target, 0x7) != 1 && fn_80237F74(ctx, target, 0xF) != 1 &&
        fn_80237F74(ctx, target, 0x48) != 1 && fn_80237F74(ctx, target, 0x29) != 1 &&
        fn_80237F74(ctx, target, 0x28) != 1 && fn_80237F74(ctx, target, 0xC) == 1) {
        return 0;
    }
    return 1;
}

s32 fightTrainerAiWazaHit120(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightOutPokemonGetSex(u32 pokemon);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fn_80237288(void* ctx, u32 pokemon);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 pokemon, s32 kind);
    u8 sex1 = fightOutPokemonGetSex(param1);
    u8 sex3 = fightOutPokemonGetSex(param3);
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_80237F74(ctx, param3, 0xC) == 1) {
        gate = 0;
    }
    if (!wazaHit120TargetClear(ctx, param3)) {
        return 0;
    }
    if (sex1 == sex3 || fn_80237288(ctx, param3) == 1 || fightTrainerAiIsPokemonJoutai(ctx, param3, 0xA) == 1 ||
        sex1 == 2 || sex3 == 2) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}
