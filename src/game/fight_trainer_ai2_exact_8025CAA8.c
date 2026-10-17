/**
 * @file fight_trainer_ai2_exact_8025CAA8.c
 * @brief Strict exact suffix of fightTrainerAi2.cpp (0x8025CAA8-0x8025CD64).
 */

#include "game/colosseum.h"

typedef u32 (*FightFloorCallback)(void*, u32, void*);

typedef struct SimerikeContext {
    void* trainer;
} SimerikeContext;

typedef struct SawaguContext {
    void* trainer;
    u32 target;
} SawaguContext;

extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
extern u8 fightIsWazaBlockedByMamoru(u32, u32);
extern u32 pokemonGetStatus(u32, u32, u32, u32);
extern u32 GSrand(void);
extern u8 fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(u32, u16*);
extern u32 fightFloorLoopValidFightOutPokemon(u32, FightFloorCallback, void*, u32);
extern u32 fightOutPokemonCheckFightOut(void*);
extern u32 fightTrainerAiIsPokemonJoutai(void*, void*, u32);
extern u32 fn_80237F74();

u32 _fightTrainerAiSimerikeCheckSub(void*, u32, void*);
u32 _fightTrainerAiSawaguCheckSub(void*, u32, void*);

u32 fightTrainerAiCheckGuard(void* trainer, u32 pokemon, u32 actionId)
{
    u32 value;

    value = fightTrainerGetStatus(trainer, 0, 0x43, 0) & 0xFFFF;
    value = fightTrainerGetStatus(0, value, 2, 0) & 0xFFFF;
    if ((u8)fightTrainerGetStatus(0, value, 0x24, 0) == 1) {
        if (fightIsWazaBlockedByMamoru(pokemon, actionId) == 1) {
            return 1;
        }
    }
    return 0;
}

u16 fightTrainerAiCheckOumu(void* trainer, u32 pokemon, u32 unused)
{
    u16 choices[4];
    u32 actionId;
    s32 random;
    s32 index;
    u8 count;

    actionId = pokemonGetStatus(pokemon, 0, 0xF7, 0) & 0xFFFF;
    if (actionId != 0 && actionId != 0x165 && actionId != 0xFFFF) {
        return actionId;
    }
    count = fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry(pokemon, choices);
    if (count != 0) {
        random = GSrand() & 0xFFFF;
        index = random % (s32)(u8)count;
        actionId = choices[(u8)index];
        if (actionId != 0 && actionId != 0x165) {
            return actionId;
        }
    }
    return 0;
}

u32 fightTrainerAiCheckSimerike(void* trainer, u32 unused1, u32 unused2)
{
    SimerikeContext context;
    u32 result;

    context.trainer = trainer;
    result = fightFloorLoopValidFightOutPokemon(
        0, _fightTrainerAiSimerikeCheckSub, &context, 0) & 0xFF;
    return (1 - result) != 0;
}

u32 _fightTrainerAiSimerikeCheckSub(void* pokemon, u32 slot, void* data)
{
    SimerikeContext* context = data;
    void* trainer = context->trainer;

    if ((fightOutPokemonCheckFightOut(pokemon) & 0xFF) == 0) {
        return 1;
    }
    return (fn_80237F74(trainer, pokemon, 6) & 0xFF) != 1;
}

u32 fightTrainerAiCheckSawagu(void* trainer, u32 target, u32 unused)
{
    SawaguContext context;
    u32 result;

    context.trainer = trainer;
    context.target = target;
    result = fightFloorLoopValidFightOutPokemon(
        0, _fightTrainerAiSawaguCheckSub, &context, 0) & 0xFF;
    return result != 1;
}

u32 _fightTrainerAiSawaguCheckSub(void* pokemon, u32 slot, void* data)
{
    SawaguContext* context = data;
    void* trainer = context->trainer;
    u32 target = context->target;

    if ((fightOutPokemonCheckFightOut(pokemon) & 0xFF) == 0) {
        return 1;
    }
    if ((fightTrainerAiIsPokemonJoutai(trainer, pokemon, 0xB) & 0xFF) == 1 &&
        (fn_80237F74(trainer, target, 0x2B) & 0xFF) == 0) {
        return 0;
    }
    return 1;
}
