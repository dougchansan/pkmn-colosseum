#include "dolphin/types.h"

extern u8* lbl_8047B610;

extern void* fightTargetGetPtrAsNowFightType(u32 slotType, u32 index);
extern u32 pokemonGetStatus(void* object, u32 id, u32 selector, u32 index);
extern u8 figthOutPokemonGetLevel(void* context);
extern void wazaSetStatus(
    void* ptr, u16 dataId, u16 status, u32 index, u32 value);

void wsCmd9F(void)
{
    void* context = fightTargetGetPtrAsNowFightType(0x11, 0);
    void* move = (void*)pokemonGetStatus(context, 0, 0xd9, 0);

    wazaSetStatus(move, 0, 0x2d, 0, figthOutPokemonGetLevel(context));
    lbl_8047B610++;
}
