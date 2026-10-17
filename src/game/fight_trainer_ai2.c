/**
 * @file fight_trainer_ai2.c
 * @brief Legacy exact-source candidate for the Horobinouta callback.
 *
 * This function remains CodeCandidate: its natural typed form is not exact,
 * so it is intentionally excluded from linked progress.
 */

#include "game/colosseum.h"

typedef struct HorobinoutaContext {
    u32 trainer;
    u32 count;
} HorobinoutaContext;

extern u8 fightOutPokemonCheckFightOut(
    void* pokemon, u32 slot, HorobinoutaContext* context);
extern u8 fightTrainerIsAllyFightTargetPtr(u32 trainer, void* pokemon, u32 slot);
extern u8 fightTrainerAiIsPokemonJoutai(u32 trainer, void* pokemon, u32 flag);
extern u8 fn_80237F74(u32 trainer, void* pokemon, u32 flag);

s32 _fightTrainerAiCheckHorobinoutaSub(
    void* pokemon, u32 slot, HorobinoutaContext* context)
{
    u32 r31;
    u32 r30;
    u32 r28;
    u32 r29;

    r29 = (u32)context;
    r28 = slot;
    r31 = (u32)pokemon;
    r30 = *(u32*)r29;
    if (!fightOutPokemonCheckFightOut(
            (void*)r31, r28, (HorobinoutaContext*)r29)) {
        return 1;
    }
    if (fightTrainerIsAllyFightTargetPtr(r30, (void*)r31, r28) == 1) {
        return 1;
    }
    if (fightTrainerAiIsPokemonJoutai(r30, (void*)r31, 0x1E) != 1 &&
        fn_80237F74(r30, (void*)r31, 0x2B) != 1) {
        *(u32*)(r29 + 4) = *(u32*)(r29 + 4) + 1;
    }
    return 1;
}
