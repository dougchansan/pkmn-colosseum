#include "game/fight_action.h"

typedef u32 (*FightOutPokemonVisitor)(void* pokemon, u16 index,
                                      void** selectedPokemon);

extern u32 tenkouDataBiosGetFightInitMsgId(u16 weatherId);
extern u32 fightFloorLoopValidFightOutPokemon(
    u32 side, FightOutPokemonVisitor visitor, void* context, u32 flags);
extern s32 fightFloorGetNowTenkouDataId(void* floor, u32 index);
extern u8 fightFloorSetStatus(u32 side, u16 index, u32 status,
                              u16 subIndex, u32 value);
extern u32 fightKoukaDoFightKoukaJoukenAndKouka(void* floor, u16 mode);
extern void wsRunScript(void* context, void* buffData, u8 mode);
extern void* pokemonGetStatus(void* pokemon, u32 group, u16 status, u32 index);

u32 _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr__FPvUsPv(
    void* pokemon, u16 index, void** selectedPokemon);

u32 fightActionFlowTenkouInit(void* context)
{
    u8 weatherId;
    void* selectedPokemon;
    u32 messageId;

    fightKoukaDoFightKoukaJoukenAndKouka(0, 1);
    weatherId = fightFloorGetNowTenkouDataId(0, 0);
    selectedPokemon = 0;
    fightFloorLoopValidFightOutPokemon(
        0, _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr__FPvUsPv,
        &selectedPokemon, 0);
    if (weatherId != 0) {
        fightFloorSetStatus(0, 0, 0x36, 0, (u32)selectedPokemon);
        messageId = tenkouDataBiosGetFightInitMsgId(weatherId);
        fightFloorSetStatus(0, 0, 0x50, 0, messageId);
        wsRunScript(context,
                    fightActionBiosGetBuffDataPtr((FightAction*)context), 0);
    }
    return 1;
}

u32 _fightActionFlowTenkouInitSubGetSeqFightOutPokemonPtr__FPvUsPv(
    void* pokemon, u16 index, void** selectedPokemon)
{
    if (pokemonGetStatus(pokemon, 0, 0xEE, 0) != 0) {
        if (selectedPokemon != 0) {
            *selectedPokemon = pokemon;
        }
        return 0;
    }
    return 1;
}

u32 fightActionFlowHeijou(void* action)
{
    return 1;
}
