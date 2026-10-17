/**
 * @file fight_range_exact_80212840.c
 * @brief Strict fight-sequence status-initializer island at 0x80212840.
 */
#include "dolphin/types.h"

extern u8 fightOutPokemonIsPokemonJoutaiDataId(void*, u32);
extern void fightOutPokemonWriteJoutaiDataId(void*, u32);

void fightSeqSpecificationActionCounterInit(void* ctx)
{
    if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 0x2e) == 1) {
        fightOutPokemonWriteJoutaiDataId(ctx, 0x2e);
    }
    if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 0x15) == 1) {
        fightOutPokemonWriteJoutaiDataId(ctx, 0x15);
    }
    if (fightOutPokemonIsPokemonJoutaiDataId(ctx, 0x28) == 1) {
        fightOutPokemonWriteJoutaiDataId(ctx, 0x28);
    }
}
