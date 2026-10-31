/**
 * @file fight_candidate_8020DA14.c
 * @brief fightKoukaDoFightKoukaJoukenAndKouka carve, 0x8020DA14 - 0x8020DAD0,
 *        linked.
 *
 * Standalone copy of the fight.c body. Retail schedules it against the 603
 * model (-proc 603 in configure.py; see the note in fight.c). The kouka data
 * accessors are external here, so no dont_inline pragma is needed.
 */
#include "dolphin/types.h"

typedef struct FightKoukaData FightKoukaData;

extern u32 fightKoukaDataBiosGetKoukaDataId(FightKoukaData* ptr);
extern u32 fightKoukaDataBiosGetFightTargetDataId(FightKoukaData* ptr);
extern u32 fightKoukaDataBiosGetFightJoukenDataId(FightKoukaData* ptr);
extern FightKoukaData* fightKoukaDataBiosGetPtr(u16 index);

u32 fightKoukaDoFightKoukaJoukenAndKouka(void* target, u16 koukaDataIndex) {
    extern void koukaExec(u16 koukaDataId, void* fightTarget, void* target, u32 flags);
    extern void* fightTargetGetPtr(u32 targetDataId, void* target, u16 fightType);
    extern u32 fightFloorGetStatus(u32, u16, u32, u16);
    extern u8 fightJoukenCheck_8020A8E0(u32 joukenDataId, void* target);

    u32 koukaDataId;
    u32 targetDataId;
    void* fightTarget;
    u32 result;
    u32 joukenDataId;
    u16 fightType;

    joukenDataId = fightKoukaDataBiosGetFightJoukenDataId(fightKoukaDataBiosGetPtr(koukaDataIndex));
    targetDataId = fightKoukaDataBiosGetFightTargetDataId(fightKoukaDataBiosGetPtr(koukaDataIndex));
    koukaDataId = fightKoukaDataBiosGetKoukaDataId(fightKoukaDataBiosGetPtr(koukaDataIndex));
    result = 0;
    fightType = fightFloorGetStatus(0, 0, 0x14, 0);
    fightTarget = fightTargetGetPtr(targetDataId, target, fightType);
    if (fightJoukenCheck_8020A8E0(joukenDataId, target) == 1) {
        koukaExec((u16)koukaDataId, fightTarget, target, 0);
        result = 1;
    }
    return result;
}
