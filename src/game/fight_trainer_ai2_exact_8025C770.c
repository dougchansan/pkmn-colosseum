/**
 * @file fight_trainer_ai2_exact_8025C770.c
 * @brief Strict exact texture-attribute check from fightTrainerAi2.cpp.
 */

#include "game/colosseum.h"

extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
extern u8 fightAiCollectWazaTypes(u32, void*);

u32 fightTrainerAiCheckTextureZokusei(void* trainer, u32 pokemon, u32 unused)
{
    u8 attributes[0x10];
    u32 value;
    u32 result;

    value = fightTrainerGetStatus(trainer, 0, 0x43, 0) & 0xFFFF;
    value = fightTrainerGetStatus(0, value, 2, 0) & 0xFFFF;
    if ((u8)fightTrainerGetStatus(0, value, 0x2A, 0) == 1) {
        value = fightAiCollectWazaTypes(pokemon, attributes) & 0xFF;
        result = value >= 1;
        return result & 0xFF;
    }
    return 1;
}
