/**
 * @file fight_trainer_range_801FAA58.c
 * @brief Candidate fight-trainer range, 0x801FAA58 - 0x801FB8F8.
 */

#include "dolphin/types.h"

extern void* fightTrainerDataBiosGetPtr(u16 slot);
extern void* fightTrainerPokemonPartDataBiosGetPtr(u16 slot);
extern void* fightTrainerPokemonDataBiosGetPtr(u16 slot);
extern void* fightTrainerAiDataBiosGetPtr(u16 slot);
extern void* fightTrainerAiValueAddsubDataBiosGetPtr(u16 slot);
extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
extern u8 fn_801FCCAC(u8* ptr);
extern u16 fn_801FCC94(u8* ptr);
extern u32 fn_801FCC7C(u8* ptr);
extern u16 fightTrainerDataBiosGetKindDataId(u8* ptr);
extern u16 fn_801FCC3C(u8* ptr);
extern u32 fn_801FCC08(u8* ptr, u8 idx);
extern u32 fn_801FCBF0(u8* ptr);
extern u32 fn_801FCBBC(u8* ptr, u8 idx);
extern u32 fn_801FCBA4(u8* ptr);
extern u32 fightTrainerPokemonPartDataBiosGetName(u8* ptr);
extern u32 fightTrainerPokemonPartDataBiosGetWazaTypeRevise(u8* ptr, u8 idx);
extern u32 fightTrainerPokemonDataBiosGetNickname(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetStatusRnd(u8* ptr, u8 idx);
extern u32 fightTrainerPokemonDataBiosGetStatusEffort(u8* ptr, u8 idx);
extern u32 fightTrainerPokemonDataBiosGetLevel(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetItemBallId(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetDarkPokemonFlag(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetTokuseiFlag(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetPokemonDataId(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetItemDataId(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetWazaDataId(u8* ptr, u8 idx);
extern u32 fightTrainerPokemonDataBiosGetPpCnt(u8* ptr, u8 idx);
extern u32 fightTrainerPokemonDataBiosGetFriend(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetSexDataId(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetSeikakuDataId(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetKeyPlayerFlag(u8* ptr);
extern u32 fightTrainerPokemonDataBiosGetPartDataId(u8* ptr);
extern u32 fightTrainerAiDataBiosGetPokemonSelectRandomFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetPokemonSelectWeakPointFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetPokemonDataOrderOutFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetPokemonDataOrderAceBossFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetKeyPlayerFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetPokemonJoutaiFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetComboValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetIrekaeValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetItemValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetParamExpectFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetParamStoreFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetZokuseiCheckFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetTokuseiCheckFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetNokoriHpValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaDamageFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetAbicntMaxValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetAbicntMinValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetDefensePokemonRndSelectFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaRndSelectFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaInitValueFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetPartFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaHitFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaAvgValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaRiskFlag(u8* ptr);
extern u32 fightTrainerAiDataBiosGetWazaNokoriPpValue(u8* ptr);
extern u32 fightTrainerAiDataBiosGetLastValueRevise(u8* ptr);
extern u32 fightTrainerAiDataBiosGetZokuseiReviseZokuseiDataId(u8* ptr, u32 idx);
extern u32 fightTrainerAiDataBiosGetZokuseiReviseValue(u8* ptr, u32 idx);
extern u32 fightTrainerAiDataBiosGetWazaTypeReviseTypeDataId(u8* ptr, u32 idx);
extern u32 fightTrainerAiDataBiosGetWazaTypeReviseValue(u8* ptr, u32 idx);
extern u32 fightTrainerAiValueAddsubDataBiosGetValue(u8* ptr);
extern u32 fightTrainerAiValueAddsubDataBiosGetName(u8* ptr);
extern u32 fightTrainerAiValueAddsubDataBiosGetKoudouName(u8* ptr);
extern u32 fightTrainerAiValueAddsubDataBiosGetPrefixName(u8* ptr);
extern u8* fightTrainer_GetHeroPtr(u8* ptr);
extern u16 fightTrainer_GetFightTrainerDataId(u8* ptr);
extern u8* fightTrainer_GetFightPokemonPtr(u8* ptr, u16 idx);
extern u8* fightTrainer_GetFightoutPokemonPtr(u8* ptr, u16 idx);
extern u8* fightTrainer_GetFightoutPokemonBuffPtr(u8* ptr);
extern u8 fightTrainer_GetOkaneBai(u8* ptr);
extern u32 fightTrainer_GetKoban(u8* ptr);
extern u8 fightTrainer_GetNigeruCount(u8* ptr);
extern u8 fightTrainer_GetControllerId(u8* ptr);
extern u8* fightTrainer_GetFightActionBuffPtr(u8* ptr);
extern u32 fightTrainer_GetSequencePtr(u8* ptr);
extern u8* fightTrainer_GetFightTrainerEnemyPokemonAryPtr(u8* ptr, u8 idx);
extern u16 fn_801FBED0(u8* ptr, u8 idx);
extern u16 fightTrainerEnemyPokemonBiosGetTokuseiFlag(u8* ptr);
extern u16 fightTrainerEnemyPokemonBiosGetStoreTokuseiData(u8* ptr);
extern u8 fightTrainerEnemyPokemonBiosGetNowhp1banhikuiFlag(u8* ptr);
extern u8 fightTrainerEnemyPokemonBiosGetLv1banhikuiFlag(u8* ptr);
extern u8 fightTrainerEnemyPokemonBiosGetDefense1banhikuiFlag(u8* ptr);
extern u8 fightTrainerEnemyPokemonBiosGetBadwazaHaveFlag(u8* ptr);
extern u8 fightTrainerEnemyPokemonBiosGetParam1bantakaiFlag(u8* ptr);

extern void fn_801FCB94(u8* ptr, u8 val);
extern void fn_801FCB84(u8* ptr, u16 val);
extern void fn_801FCB74(u8* ptr, u32 val);
extern void fightTrainerDataBiosSetKindDataId(u8* ptr, u16 val);
extern void fn_801FCB64(u8* ptr, u16 val);
extern void fn_801FCB40(u8* ptr, u8 idx, u16 val);
extern void fn_801FCB30(u8* ptr, u32 val);
extern void fn_801FCB0C(u8* ptr, u8 idx, u32 val);
extern void fn_801FCAFC(u8* ptr, u32 val);
extern void fightTrainerPokemonPartDataBiosSetName(u8* ptr, u32 val);
extern void fightTrainerPokemonPartDataBiosSetWazaTypeRevise(u8* ptr, u8 idx, u8 val);
extern void fightTrainerPokemonDataBiosSetNickname(u8* ptr, u32 val);
extern void fightTrainerPokemonDataBiosSetStatusRnd(u8* ptr, u8 idx, u8 val);
extern void fightTrainerPokemonDataBiosSetStatusEffort(u8* ptr, u8 idx, u16 val);
extern void fightTrainerPokemonDataBiosSetLevel(u8* ptr, u8 val);
extern void fightTrainerPokemonDataBiosSetItemBallId(u8* ptr, u16 val);
extern void fightTrainerPokemonDataBiosSetDarkPokemonFlag(u8* ptr, u8 val);
extern void fightTrainerPokemonDataBiosSetTokuseiFlag(u8* ptr, u8 val);
extern void fightTrainerPokemonDataBiosSetPokemonDataId(u8* ptr, u16 val);
extern void fightTrainerPokemonDataBiosSetItemDataId(u8* ptr, u32 val);
extern void fightTrainerPokemonDataBiosSetWazaDataId(u8* ptr, u8 idx, u32 val);
extern void fightTrainerPokemonDataBiosSetPpCnt(u8* ptr, u8 idx, u8 val);
extern void fightTrainerPokemonDataBiosSetFriend(u8* ptr, u16 val);
extern void fightTrainerPokemonDataBiosSetSexDataId(u8* ptr, u8 val);
extern void fightTrainerPokemonDataBiosSetSeikakuDataId(u8* ptr, u8 val);
extern void fightTrainerPokemonDataBiosSetKeyPlayerFlag(u8* ptr, u8 val);
extern void fightTrainerPokemonDataBiosSetPartDataId(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetPokemonSelectRandomFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetPokemonSelectWeakPointFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetPokemonDataOrderOutFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetPokemonDataOrderAceBossFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetKeyPlayerFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetPokemonJoutaiFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetComboValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetIrekaeValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetItemValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetParamExpectFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetParamStoreFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetZokuseiCheckFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetTokuseiCheckFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetNokoriHpValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaDamageFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetAbicntMaxValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetAbicntMinValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetDefensePokemonRndSelectFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaRndSelectFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaInitValueFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetPartFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaHitFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaAvgValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaRiskFlag(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetWazaNokoriPpValue(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetLastValueRevise(u8* ptr, u8 val);
extern void fightTrainerAiDataBiosSetZokuseiReviseZokuseiDataId(u8* ptr, u16 idx, u8 val);
extern void fightTrainerAiDataBiosSetZokuseiReviseValue(u8* ptr, u16 idx, u8 val);
extern void fightTrainerAiDataBiosSetWazaTypeReviseTypeDataId(u8* ptr, u16 idx, u8 val);
extern void fightTrainerAiDataBiosSetWazaTypeReviseValue(u8* ptr, u16 idx, u8 val);
extern void fightTrainerAiValueAddsubDataBiosSetValue(u8* ptr, u32 val);
extern void fightTrainerAiValueAddsubDataBiosSetName(u8* ptr, u32 val);
extern void fightTrainerAiValueAddsubDataBiosSetKoudouName(u8* ptr, u32 val);
extern void fightTrainerAiValueAddsubDataBiosSetPrefixName(u8* ptr, u32 val);
extern void fightTrainer_SetFightTrainerDataId(u8* ptr, u16 val);
extern void fightTrainer_SetOkaneBai(u8* ptr, u8 val);
extern void fightTrainer_SetKoban(u8* ptr, u32 val);
extern void fightTrainer_SetNigeruCount(u8* ptr, u8 val);
extern void fightTrainer_SetControllerId(u8* ptr, u8 val);
extern void fightTrainer_SetSequencePtr(u8* ptr, u32 val);
extern void fn_801FBDF4(u8* ptr, u8 idx, u16 val);
extern void fightTrainerEnemyPokemonBiosSetTokuseiFlag(u8* ptr, u16 val);
extern void fightTrainerEnemyPokemonBiosSetStoreTokuseiData(u8* ptr, u16 val);
extern void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(u8* ptr, u8 val);
extern void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(u8* ptr, u8 val);
extern void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(u8* ptr, u8 val);
extern void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(u8* ptr, u8 val);
extern void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(u8* ptr, u8 val);
extern u8* fightTrainerEnemyPokemonSearchAry(void* ctx, u16 count, s16 matchVal);
extern u32 fightTrainerEnemyPokemonRegistAry(void* ctx, u16 count, u32 matchVal);
extern u32 fightTrainerEnemyPokemonEraseAry(void* ctx, u16 count, s16 matchVal);
extern void fightTrainerEnemyPokemonInitFightOutStatus(void* ctx);

void fightTrainerSetStatus(void* ctx, u32 slot, u32 field, u32 idx, u32 val) {
    u8* ptr;
    u16 key;

    key = (u16)field;
    if (key == 0 || key >= 0x5B) {
        return;
    }

    if (key < 0x0A) {
        ptr = fightTrainerDataBiosGetPtr((u16)slot);
    } else if (key < 0x0D) {
        ptr = fightTrainerPokemonPartDataBiosGetPtr((u16)slot);
    } else if (key < 0x1E) {
        ptr = fightTrainerPokemonDataBiosGetPtr((u16)slot);
    } else if (key < 0x3D) {
        ptr = fightTrainerAiDataBiosGetPtr((u16)slot);
    } else if (key < 0x42) {
        ptr = fightTrainerAiValueAddsubDataBiosGetPtr((u16)slot);
    } else {
        ptr = ctx;
    }

    if (ptr == NULL) {
        return;
    }

    switch (key) {
    case 0x01: fn_801FCB94(ptr, (u8)val); break;
    case 0x02: fn_801FCB84(ptr, (u16)val); break;
    case 0x03: fn_801FCB74(ptr, val); break;
    case 0x04: fightTrainerDataBiosSetKindDataId(ptr, (u16)val); break;
    case 0x05: fn_801FCB64(ptr, (u16)val); break;
    case 0x06: fn_801FCB40(ptr, (u8)idx, (u16)val); break;
    case 0x07: fn_801FCB30(ptr, val); break;
    case 0x08: fn_801FCB0C(ptr, (u8)idx, val); break;
    case 0x09: fn_801FCAFC(ptr, val); break;
    case 0x0B: fightTrainerPokemonPartDataBiosSetName(ptr, val); break;
    case 0x0C: fightTrainerPokemonPartDataBiosSetWazaTypeRevise(ptr, (u8)idx, (u8)val); break;
    case 0x0E: fightTrainerPokemonDataBiosSetNickname(ptr, val); break;
    case 0x0F: fightTrainerPokemonDataBiosSetStatusRnd(ptr, (u8)idx, (u8)val); break;
    case 0x10: fightTrainerPokemonDataBiosSetStatusEffort(ptr, (u8)idx, (u16)val); break;
    case 0x11: fightTrainerPokemonDataBiosSetLevel(ptr, (u8)val); break;
    case 0x12: fightTrainerPokemonDataBiosSetItemBallId(ptr, (u16)val); break;
    case 0x13: fightTrainerPokemonDataBiosSetDarkPokemonFlag(ptr, (u8)val); break;
    case 0x14: fightTrainerPokemonDataBiosSetTokuseiFlag(ptr, (u8)val); break;
    case 0x15: fightTrainerPokemonDataBiosSetPokemonDataId(ptr, (u16)val); break;
    case 0x16: fightTrainerPokemonDataBiosSetItemDataId(ptr, val); break;
    case 0x17: fightTrainerPokemonDataBiosSetWazaDataId(ptr, (u8)idx, val); break;
    case 0x18: fightTrainerPokemonDataBiosSetPpCnt(ptr, (u8)idx, (u8)val); break;
    case 0x19: fightTrainerPokemonDataBiosSetFriend(ptr, (u16)val); break;
    case 0x1A: fightTrainerPokemonDataBiosSetSexDataId(ptr, (u8)val); break;
    case 0x1B: fightTrainerPokemonDataBiosSetSeikakuDataId(ptr, (u8)val); break;
    case 0x1C: fightTrainerPokemonDataBiosSetKeyPlayerFlag(ptr, (u8)val); break;
    case 0x1D: fightTrainerPokemonDataBiosSetPartDataId(ptr, (u8)val); break;
    case 0x1F: fightTrainerAiDataBiosSetPokemonSelectRandomFlag(ptr, (u8)val); break;
    case 0x20: fightTrainerAiDataBiosSetPokemonSelectWeakPointFlag(ptr, (u8)val); break;
    case 0x21: fightTrainerAiDataBiosSetPokemonDataOrderOutFlag(ptr, (u8)val); break;
    case 0x22: fightTrainerAiDataBiosSetPokemonDataOrderAceBossFlag(ptr, (u8)val); break;
    case 0x23: fightTrainerAiDataBiosSetKeyPlayerFlag(ptr, (u8)val); break;
    case 0x24: fightTrainerAiDataBiosSetPokemonJoutaiFlag(ptr, (u8)val); break;
    case 0x25: fightTrainerAiDataBiosSetComboValue(ptr, (u8)val); break;
    case 0x26: fightTrainerAiDataBiosSetIrekaeValue(ptr, (u8)val); break;
    case 0x27: fightTrainerAiDataBiosSetItemValue(ptr, (u8)val); break;
    case 0x28: fightTrainerAiDataBiosSetParamExpectFlag(ptr, (u8)val); break;
    case 0x29: fightTrainerAiDataBiosSetParamStoreFlag(ptr, (u8)val); break;
    case 0x2A: fightTrainerAiDataBiosSetZokuseiCheckFlag(ptr, (u8)val); break;
    case 0x2B: fightTrainerAiDataBiosSetTokuseiCheckFlag(ptr, (u8)val); break;
    case 0x2C: fightTrainerAiDataBiosSetNokoriHpValue(ptr, (u8)val); break;
    case 0x2D: fightTrainerAiDataBiosSetWazaDamageFlag(ptr, (u8)val); break;
    case 0x2E: fightTrainerAiDataBiosSetAbicntMaxValue(ptr, (u8)val); break;
    case 0x2F: fightTrainerAiDataBiosSetAbicntMinValue(ptr, (u8)val); break;
    case 0x30: fightTrainerAiDataBiosSetDefensePokemonRndSelectFlag(ptr, (u8)val); break;
    case 0x31: fightTrainerAiDataBiosSetWazaRndSelectFlag(ptr, (u8)val); break;
    case 0x32: fightTrainerAiDataBiosSetWazaInitValueFlag(ptr, (u8)val); break;
    case 0x33: fightTrainerAiDataBiosSetPartFlag(ptr, (u8)val); break;
    case 0x34: fightTrainerAiDataBiosSetWazaHitFlag(ptr, (u8)val); break;
    case 0x35: fightTrainerAiDataBiosSetWazaAvgValue(ptr, (u8)val); break;
    case 0x36: fightTrainerAiDataBiosSetWazaRiskFlag(ptr, (u8)val); break;
    case 0x37: fightTrainerAiDataBiosSetWazaNokoriPpValue(ptr, (u8)val); break;
    case 0x38: fightTrainerAiDataBiosSetLastValueRevise(ptr, (u8)val); break;
    case 0x39: fightTrainerAiDataBiosSetZokuseiReviseZokuseiDataId(ptr, (u16)idx, (u8)val); break;
    case 0x3A: fightTrainerAiDataBiosSetZokuseiReviseValue(ptr, (u16)idx, (u8)val); break;
    case 0x3B: fightTrainerAiDataBiosSetWazaTypeReviseTypeDataId(ptr, (u16)idx, (u8)val); break;
    case 0x3C: fightTrainerAiDataBiosSetWazaTypeReviseValue(ptr, (u16)idx, (u8)val); break;
    case 0x3E: fightTrainerAiValueAddsubDataBiosSetValue(ptr, val); break;
    case 0x3F: fightTrainerAiValueAddsubDataBiosSetName(ptr, val); break;
    case 0x40: fightTrainerAiValueAddsubDataBiosSetKoudouName(ptr, val); break;
    case 0x41: fightTrainerAiValueAddsubDataBiosSetPrefixName(ptr, val); break;
    case 0x43: fightTrainer_SetFightTrainerDataId(ptr, (u16)val); break;
    case 0x48: fightTrainer_SetOkaneBai(ptr, (u8)val); break;
    case 0x49: fightTrainer_SetKoban(ptr, val); break;
    case 0x4A: fightTrainer_SetNigeruCount(ptr, (u8)val); break;
    case 0x4B: fightTrainer_SetControllerId(ptr, (u8)val); break;
    case 0x4C: fightTrainer_SetSequencePtr(ptr, val); break;
    case 0x4F: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fn_801FBDF4(enemy, (u8)idx, (u16)val);
        }
        break;
    }
    case 0x50: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetTokuseiFlag(enemy, (u16)val);
        }
        break;
    }
    case 0x51: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetStoreTokuseiData(enemy, (u16)val);
        }
        break;
    }
    case 0x52: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(enemy, (u8)val);
        }
        break;
    }
    case 0x53: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(enemy, (u8)val);
        }
        break;
    }
    case 0x54: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(enemy, (u8)val);
        }
        break;
    }
    case 0x55: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(enemy, (u8)val);
        }
        break;
    }
    case 0x56: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(enemy, (u8)val);
        }
        break;
    }
    case 0x57: {
        void* enemyList = fightTrainerGetStatus(ctx, 0, 0x4E, 0);

        if (enemyList != NULL) {
            fightTrainerEnemyPokemonRegistAry(enemyList, 0xC, (s16)val);
        }
        break;
    }
    case 0x58: {
        void* enemyList = fightTrainerGetStatus(ctx, 0, 0x4E, 0);

        if (enemyList != NULL) {
            fightTrainerEnemyPokemonEraseAry(enemyList, 0xC, (s16)val);
        }
        break;
    }
    case 0x59: {
        u8* enemy;

        if ((s16)slot < 0) {
            break;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy != NULL) {
            fightTrainerEnemyPokemonInitFightOutStatus(enemy);
        }
        break;
    }
    default:
        break;
    }
}

void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx) {
    u8* ptr;
    u8* enemy;
    u16 key;

    key = (u16)field;
    if (key == 0 || key >= 0x5B) {
        return NULL;
    }

    if (key < 0x0A) {
        ptr = fightTrainerDataBiosGetPtr((u16)slot);
    } else if (key < 0x0D) {
        ptr = fightTrainerPokemonPartDataBiosGetPtr((u16)slot);
    } else if (key < 0x1E) {
        ptr = fightTrainerPokemonDataBiosGetPtr((u16)slot);
    } else if (key < 0x3D) {
        ptr = fightTrainerAiDataBiosGetPtr((u16)slot);
    } else if (key < 0x42) {
        ptr = fightTrainerAiValueAddsubDataBiosGetPtr((u16)slot);
    } else {
        ptr = ctx;
    }

    if (ptr == NULL) {
        return NULL;
    }

    switch (key) {
    case 0x01: return (void*)(u32)fn_801FCCAC(ptr);
    case 0x02: return (void*)(u32)fn_801FCC94(ptr);
    case 0x03: return (void*)fn_801FCC7C(ptr);
    case 0x04: return (void*)(u32)fightTrainerDataBiosGetKindDataId(ptr);
    case 0x05: return (void*)(u32)fn_801FCC3C(ptr);
    case 0x06: return (void*)(u32)fn_801FCC08(ptr, (u8)idx);
    case 0x07: return (void*)fn_801FCBF0(ptr);
    case 0x08: return (void*)fn_801FCBBC(ptr, (u8)idx);
    case 0x09: return (void*)fn_801FCBA4(ptr);
    case 0x0B: return (void*)fightTrainerPokemonPartDataBiosGetName(ptr);
    case 0x0C: return (void*)(u32)(u8)fightTrainerPokemonPartDataBiosGetWazaTypeRevise(ptr, (u8)idx);
    case 0x0E: return (void*)fightTrainerPokemonDataBiosGetNickname(ptr);
    case 0x0F: return (void*)(u32)(s32)(s8)fightTrainerPokemonDataBiosGetStatusRnd(ptr, (u8)idx);
    case 0x10: return (void*)(u32)(s32)(s16)fightTrainerPokemonDataBiosGetStatusEffort(ptr, (u8)idx);
    case 0x11: return (void*)(u32)(u8)fightTrainerPokemonDataBiosGetLevel(ptr);
    case 0x12: return (void*)(u32)(u16)fightTrainerPokemonDataBiosGetItemBallId(ptr);
    case 0x13: return (void*)(u32)(u8)fightTrainerPokemonDataBiosGetDarkPokemonFlag(ptr);
    case 0x14: return (void*)(u32)(s32)(s8)fightTrainerPokemonDataBiosGetTokuseiFlag(ptr);
    case 0x15: return (void*)(u32)(u16)fightTrainerPokemonDataBiosGetPokemonDataId(ptr);
    case 0x16: return (void*)fightTrainerPokemonDataBiosGetItemDataId(ptr);
    case 0x17: return (void*)fightTrainerPokemonDataBiosGetWazaDataId(ptr, (u8)idx);
    case 0x18: return (void*)(u32)(u8)fightTrainerPokemonDataBiosGetPpCnt(ptr, (u8)idx);
    case 0x19: return (void*)(u32)(s32)(s16)fightTrainerPokemonDataBiosGetFriend(ptr);
    case 0x1A: return (void*)(u32)(s32)(s8)fightTrainerPokemonDataBiosGetSexDataId(ptr);
    case 0x1B: return (void*)(u32)(s32)(s8)fightTrainerPokemonDataBiosGetSeikakuDataId(ptr);
    case 0x1C: return (void*)(u32)(u8)fightTrainerPokemonDataBiosGetKeyPlayerFlag(ptr);
    case 0x1D: return (void*)(u32)(u8)fightTrainerPokemonDataBiosGetPartDataId(ptr);
    case 0x1F: return (void*)(u32)(u8)fightTrainerAiDataBiosGetPokemonSelectRandomFlag(ptr);
    case 0x20: return (void*)(u32)(u8)fightTrainerAiDataBiosGetPokemonSelectWeakPointFlag(ptr);
    case 0x21: return (void*)(u32)(u8)fightTrainerAiDataBiosGetPokemonDataOrderOutFlag(ptr);
    case 0x22: return (void*)(u32)(u8)fightTrainerAiDataBiosGetPokemonDataOrderAceBossFlag(ptr);
    case 0x23: return (void*)(u32)(u8)fightTrainerAiDataBiosGetKeyPlayerFlag(ptr);
    case 0x24: return (void*)(u32)(u8)fightTrainerAiDataBiosGetPokemonJoutaiFlag(ptr);
    case 0x25: return (void*)(u32)(u8)fightTrainerAiDataBiosGetComboValue(ptr);
    case 0x26: return (void*)(u32)(u8)fightTrainerAiDataBiosGetIrekaeValue(ptr);
    case 0x27: return (void*)(u32)(u8)fightTrainerAiDataBiosGetItemValue(ptr);
    case 0x28: return (void*)(u32)(u8)fightTrainerAiDataBiosGetParamExpectFlag(ptr);
    case 0x29: return (void*)(u32)(u8)fightTrainerAiDataBiosGetParamStoreFlag(ptr);
    case 0x2A: return (void*)(u32)(u8)fightTrainerAiDataBiosGetZokuseiCheckFlag(ptr);
    case 0x2B: return (void*)(u32)(u8)fightTrainerAiDataBiosGetTokuseiCheckFlag(ptr);
    case 0x2C: return (void*)(u32)(u8)fightTrainerAiDataBiosGetNokoriHpValue(ptr);
    case 0x2D: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaDamageFlag(ptr);
    case 0x2E: return (void*)(u32)(u8)fightTrainerAiDataBiosGetAbicntMaxValue(ptr);
    case 0x2F: return (void*)(u32)(u8)fightTrainerAiDataBiosGetAbicntMinValue(ptr);
    case 0x30: return (void*)(u32)(u8)fightTrainerAiDataBiosGetDefensePokemonRndSelectFlag(ptr);
    case 0x31: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaRndSelectFlag(ptr);
    case 0x32: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaInitValueFlag(ptr);
    case 0x33: return (void*)(u32)(u8)fightTrainerAiDataBiosGetPartFlag(ptr);
    case 0x34: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaHitFlag(ptr);
    case 0x35: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaAvgValue(ptr);
    case 0x36: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaRiskFlag(ptr);
    case 0x37: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaNokoriPpValue(ptr);
    case 0x38: return (void*)(u32)(u8)fightTrainerAiDataBiosGetLastValueRevise(ptr);
    case 0x39: return (void*)(u32)(u8)fightTrainerAiDataBiosGetZokuseiReviseZokuseiDataId(ptr, idx);
    case 0x3A: return (void*)(u32)(u8)fightTrainerAiDataBiosGetZokuseiReviseValue(ptr, idx);
    case 0x3B: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaTypeReviseTypeDataId(ptr, idx);
    case 0x3C: return (void*)(u32)(u8)fightTrainerAiDataBiosGetWazaTypeReviseValue(ptr, idx);
    case 0x3E: return (void*)fightTrainerAiValueAddsubDataBiosGetValue(ptr);
    case 0x3F: return (void*)fightTrainerAiValueAddsubDataBiosGetName(ptr);
    case 0x40: return (void*)fightTrainerAiValueAddsubDataBiosGetKoudouName(ptr);
    case 0x41: return (void*)fightTrainerAiValueAddsubDataBiosGetPrefixName(ptr);
    case 0x43: return (void*)(u32)fightTrainer_GetFightTrainerDataId(ptr);
    case 0x44: return fightTrainer_GetHeroPtr(ptr);
    case 0x45: return fightTrainer_GetFightPokemonPtr(ptr, (u16)idx);
    case 0x46: return fightTrainer_GetFightoutPokemonPtr(ptr, (u16)idx);
    case 0x47: return fightTrainer_GetFightoutPokemonBuffPtr(ptr);
    case 0x48: return (void*)(u32)(u8)fightTrainer_GetOkaneBai(ptr);
    case 0x49: return (void*)fightTrainer_GetKoban(ptr);
    case 0x4A: return (void*)(u32)(u8)fightTrainer_GetNigeruCount(ptr);
    case 0x4B: return (void*)(u32)(u8)fightTrainer_GetControllerId(ptr);
    case 0x4C: return fightTrainer_GetFightActionBuffPtr(ptr);
    case 0x4D: return (void*)fightTrainer_GetSequencePtr(ptr);
    case 0x4E: return fightTrainer_GetFightTrainerEnemyPokemonAryPtr(ptr, (u8)idx);
    case 0x4F:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fn_801FBED0(enemy, (u8)idx);
    case 0x50:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetTokuseiFlag(enemy);
    case 0x51:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetStoreTokuseiData(enemy);
    case 0x52:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetNowhp1banhikuiFlag(enemy);
    case 0x53:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetLv1banhikuiFlag(enemy);
    case 0x54:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetDefense1banhikuiFlag(enemy);
    case 0x55:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetBadwazaHaveFlag(enemy);
    case 0x56:
        if ((s16)slot < 0) {
            return NULL;
        }
        enemy = fightTrainerEnemyPokemonSearchAry(
            fightTrainerGetStatus(ctx, 0, 0x4E, 0), 0xC, (s16)slot);
        if (enemy == NULL) {
            return NULL;
        }
        return (void*)(u32)fightTrainerEnemyPokemonBiosGetParam1bantakaiFlag(enemy);
    default:
        return NULL;
    }
}
