#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

u32 fightTrainerAiSelectFightActionIrekae(void* ctx, u32 param1, u32 param2,
                                          u32 param3)
{
    extern u8 lbl_80375D30[];
    int new_var2;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void*,
                                                 s32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern void fn_8023A118(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                            s32);
    long long new_var;
    extern s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void*, u32, u32,
                                                          u32);
    extern s32 fightTrainerAiGetFightPokemonIrekaeModosuValue();
    u32 choice;
    s32 score;
    u32 field;

    fightTrainerGetStatus(
        0, fightTrainerGetStatus((u32)ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
    field = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param1);
    score = fightTrainerAiGetFightPokemonIrekaeModosuValue(ctx, param1, param2);
    if (score <= 0) {
        return 0;
    }
    new_var = !ctx;
    if (new_var) {
    }
    new_var2 = 0;
    fn_8023A118(
        0xEC63, 0xEC04, 0xEC05, field, fightOutPokemonGetPokemonPtr(param1), 0,
        new_var2, new_var2, 0,
        (((((0x228 & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF,
        score);
    choice = fightTrainerAiSelectIrekaeDasuFightPokemon(ctx, param2, 1, param1);
    if ((s16)choice < 0) {
        return 0;
    }
    new_var = choice;
    fightOutPokemonCreateFightAction(param1, new_var2, 9, 0, lbl_80375D30,
                                     (s16)new_var);
    return 1;
}
