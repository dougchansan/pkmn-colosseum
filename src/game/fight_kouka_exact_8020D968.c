/**
 * @file fight_kouka_exact_8020D968.c
 * @brief Strict fight effect-data helpers, 0x8020D968-0x8020DA14.
 */

#include "game/colosseum.h"

typedef struct FightKoukaData {
    u16 fightJoukenDataId;
    u16 fightTargetDataId;
    u16 koukaDataId;
} FightKoukaData;

typedef struct FightCopyBlock {
    u32 data[12];
} FightCopyBlock;

extern FightKoukaData lbl_80375CB8[];

void fightActionBiosCopy(FightCopyBlock* dst, FightCopyBlock* src)
{
    if (dst == 0) {
        return;
    }
    if (src == 0) {
        return;
    }
    *dst = *src;
}

u16 fightKoukaDataBiosGetKoukaDataId(FightKoukaData* ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return ptr->koukaDataId;
}

u16 fightKoukaDataBiosGetFightTargetDataId(FightKoukaData* ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return ptr->fightTargetDataId;
}

u16 fightKoukaDataBiosGetFightJoukenDataId(FightKoukaData* ptr)
{
    if (ptr == NULL) {
        return 0;
    }
    return ptr->fightJoukenDataId;
}

FightKoukaData* fightKoukaDataBiosGetPtr(u16 index)
{
    extern u32 lbl_80478D50;

    if (index >= lbl_80478D50) {
        return NULL;
    }
    return &lbl_80375CB8[index];
}
