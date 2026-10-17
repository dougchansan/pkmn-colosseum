#include "game/fight_action.h"

extern u8 fightFloorSetStatus(u32 side, u16 index, u32 status,
                              u16 subIndex, u32 value);
extern void wsRunScript(void* context, void* buffData, u8 mode);

u32 fightActionFlowWazaKiaipantiPre(void* context)
{
    FightAction* action = context;

    fightFloorSetStatus(0, 0, 0x36, 0,
                        (u32)fightActionBiosGetActorFightTargetPtr(action));
    wsRunScript(context, fightActionBiosGetBuffDataPtr(action), 0);
    return 1;
}
