/**
 * @file fight_side.c
 * @brief Fight-side stat calculation and party-side state.
 *
 * Split out of the former game/pokemon.c CodeCandidate bucket
 * (0x801F000C-0x801F7F80), which was mislabeled "pokemon" but is
 * entirely the XD-era fight-engine cluster. Address range covered by
 * this translation unit: 0x801F6B54-0x801F7798 (22 functions), per
 * config/GC6E01/splits.txt.
 */

#include "game/pokemon_fight_types.h"

#if defined(FIGHT_SIDE_801F6B54_801F6F38)

/* 0x801F6B54 | size: 0xF8 | medium */
void fightSideRegistFightSideEnemyPokemonFightAll(u32 param_1, u32 param_2, u32 param_3, u32 param_4, u32 param_5) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u8 fightTrainerCheckValid(u32);
    extern u32 fightTrainerGetStatus(u32, u32, u16, u32);
    extern u8 fightOutPokemonCheckFightOut(u32);
    extern u32 pokemonGetStatus(u32, u32, u16, u32);
    extern void fightTrainerSetStatus(u32, u32, u16, u32, s16);
    u32 outerObj;
    u32 innerObj;
    u32 outerIndex;
    s16 status;

    outerIndex = 0;
    for (; (u16)outerIndex < (u16)param_4; outerIndex = outerIndex + 1) {
        if (param_1 == 0) {
            outerObj = 0;
        } else {
            outerObj = fightSideBiosGetFightTrainerPtr(param_1, outerIndex);
        }
        status = fightTrainerCheckValid(outerObj);
        if (status != 0) {
            u32 innerIndex;

            innerIndex = 0;
            for (; (u16)innerIndex < (u16)param_5; innerIndex = innerIndex + 1) {
                innerObj = fightTrainerGetStatus(outerObj, 0, 0x46, innerIndex);
                status = fightOutPokemonCheckFightOut(innerObj);
                if (status != 0) {
                    innerObj = pokemonGetStatus(innerObj, 0, 0xD5, 0);
                    status = pokemonGetStatus(innerObj, 0, 0xCE, 0);
                    if ((s16)status >= 0) {
                        fightTrainerSetStatus(param_2, 0, 0x57, 0, status);
                    }
                }
            }
        }
    }
}

/* 0x801F6C4C | size: 0x54 | small */
u32 fightSideGetJoutaiUserFightTargetId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011A860(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0;
    }
    return fn_8011A860(param_1, param_2);
}

/* 0x801F6CA0 | size: 0x54 | small */
void fightSideSetNowKaisuuJoutaiDataId(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011AB50(u32, u32, u32);

    if ((u16)fn_80119ED0(param_2) == 6) {
        fn_8011AB50(param_1, param_2, param_3);
    }
}

/* 0x801F6CF4 | size: 0x54 | small */
u32 fightSideGetNowKaisuuJoutaiDataId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011ACB4(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0xFFFFFFFF;
    }
    return fn_8011ACB4(param_1, param_2);
}

/* 0x801F6D48 | size: 0x54 | small */
u32 fightSideGetKaisuuJoutaiDataId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011AE40(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0xFFFFFFFF;
    }
    return fn_8011AE40(param_1, param_2);
}

/* 0x801F6D9C | size: 0x54 | small */
u32 fightSideGetCountAsJoutaiDataId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B130(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0xFFFFFFFF;
    }
    return fn_8011B130(param_1, param_2);
}

/* 0x801F6DF0 | size: 0x54 | small */
void fightSideWriteJoutaiDataId(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B2C0(u32, u32, u32);

    if ((u16)fn_80119ED0(param_2) == 6) {
        fn_8011B2C0(param_1, param_2, param_3);
    }
}

/* 0x801F6E44 | size: 0x54 | small */
u32 fightSideCheckWriteJoutaiDataId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B444(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0;
    }
    return fn_8011B444(param_1, param_2);
}

/* 0x801F6E98 | size: 0x54 | small */
u32 fightSideIsJoutaiDataId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern u32 fn_8011B67C(u32, u32);

    if ((u16)fn_80119ED0(param_2) != 6) {
        return 0;
    }
    return fn_8011B67C(param_1, param_2);
}

/* 0x801F6EEC | size: 0x4C | small */
void fightSideInitJoutaiDataId(u32 param_1, u32 param_2) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B788(u32, u32);

    if ((u16)fn_80119ED0(param_2) == 6) {
        fn_8011B788(param_1, param_2);
    }
}

#endif

#if defined(FIGHT_SIDE_801F6F38_801F7258)

/* 0x801F6F38 | size: 0x9C | medium */
u16 fightSideGetHikaeFightPokemonNum(u32 param_1, u32 param_2, u32 param_3, u32 param_4) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern s16 fightTrainerGetHikaeFightPokemonNum(u32, u32, u32);
    extern u8 fightTrainerCheckValid(u32);
    u16 uVar4;
    u32 uVar2;
    u16 uVar1;
    u32 uVar5;
    u32 uVar3;

    uVar4 = param_2;
    uVar2 = 0;
    uVar1 = 0;
    for (; (uVar2 & 0xFFFF) < (uVar4 & 0xFFFF); uVar2 = uVar2 + 1) {
        if (param_1 == 0) {
            uVar5 = 0;
        } else {
            uVar5 = fightSideBiosGetFightTrainerPtr(param_1, uVar2);
        }
        uVar3 = fightTrainerCheckValid(uVar5);
        if (uVar3 != 0) {
            uVar1 = uVar1 + fightTrainerGetHikaeFightPokemonNum(uVar5, param_3, param_4);
        }
    }
    return uVar1;
}

/* 0x801F6FD4 | size: 0xBC | medium */
u16 fightSideGetFightPokemonNum(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetValidFightPokemonPtr(u32, u32);
    extern u8 fightTrainerCheckValid(u32);
    extern u8 fightPokemonCheckFightOut(u32);
    u32 uCount;
    u32 uVar4;
    u16 uBound2;
    u32 uVar1;
    u32 iVar2;
    u32 uVar3;
    u16 uBound3;
    u32 uVar5;

    uCount = 0;
    uVar4 = uCount;
    uBound2 = param_2;
    for (; (uVar4 & 0xFFFF) < (uBound2 & 0xFFFF); uVar4 = uVar4 + 1) {
        if (param_1 == 0) {
            uVar1 = 0;
        } else {
            uVar1 = fightSideBiosGetFightTrainerPtr(param_1, uVar4);
        }
        uVar3 = fightTrainerCheckValid(uVar1);
        if (uVar3 != 0) {
            uBound3 = param_3;
            uVar5 = 0;
            for (; (uVar5 & 0xFFFF) < (uBound3 & 0xFFFF); uVar5 = uVar5 + 1) {
                iVar2 = fightTrainerGetValidFightPokemonPtr(uVar1, uVar5);
                if ((iVar2 != 0) && (uVar3 = fightPokemonCheckFightOut(iVar2), uVar3 != 0)) {
                    uCount = uCount + 1;
                }
            }
        }
    }
    return uCount;
}

/* 0x801F7090 | size: 0xE4 | medium */
s32 fightSideGetFightPokemonMaxHp(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetValidFightPokemonPtr(u32, u32);
    extern u8 fightTrainerCheckValid(u32);
    extern u32 fightPokemonGetPokemonPtr(u32);
    extern u8 fightPokemonCheckFightOut(u32);
    extern u32 pokemonGetStatus(u32, u32, u16, u32);
    u16 uBound2;
    s32 iVar8;
    u32 uVar7;
    u32 uVar1;
    u32 iVar2;
    u32 iVar3;
    u32 uVar4;
    u16 uBound3;
    u32 uVar6;

    uBound2 = param_2;
    iVar8 = 0;
    uVar7 = 0;
    for (; (uVar7 & 0xFFFF) < (uBound2 & 0xFFFF); uVar7 = uVar7 + 1) {
        if (param_1 == 0) {
            uVar1 = 0;
        } else {
            uVar1 = fightSideBiosGetFightTrainerPtr(param_1, uVar7);
        }
        uVar4 = fightTrainerCheckValid(uVar1);
        if (uVar4 != 0) {
            uBound3 = param_3;
            uVar6 = 0;
            for (; (uVar6 & 0xFFFF) < (uBound3 & 0xFFFF); uVar6 = uVar6 + 1) {
                iVar2 = fightTrainerGetValidFightPokemonPtr(uVar1, uVar6);
                if (iVar2 != 0) {
                    iVar3 = fightPokemonGetPokemonPtr(iVar2);
                    if ((iVar3 != 0) && (uVar4 = fightPokemonCheckFightOut(iVar2), uVar4 != 0)) {
                        uVar4 = pokemonGetStatus(iVar3, 0, 0x87, 0);
                        iVar8 = iVar8 + (uVar4 & 0xFFFF);
                    }
                }
            }
        }
    }
    return iVar8;
}

/* 0x801F7174 | size: 0xE4 | medium */
s32 fightSideGetFightPokemonNokoriHp(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetValidFightPokemonPtr(u32, u32);
    extern u8 fightTrainerCheckValid(u32);
    extern u32 fightPokemonGetPokemonPtr(u32);
    extern u8 fightPokemonCheckFightOut(u32);
    extern u32 pokemonGetStatus(u32, u32, u16, u32);
    int total;
    u32 innerLimit;
    u32 baseObj;
    u32 outerObj;
    u32 innerObj;
    u32 statObj;
    u32 status;
    u16 outerCount;
    u32 outerIndex;

    total = 0;
    baseObj = param_1;
    innerLimit = param_3;
    outerCount = param_2;
    outerIndex = 0;
    for (; (outerIndex & 0xFFFF) < (outerCount & 0xFFFF); outerIndex = outerIndex + 1) {
        if (baseObj == 0) {
            outerObj = 0;
        } else {
            outerObj = fightSideBiosGetFightTrainerPtr(baseObj, outerIndex);
        }
        status = fightTrainerCheckValid(outerObj);
        if (status != 0) {
            u16 innerCount;
            u32 innerIndex;

            innerCount = innerLimit;
            innerIndex = 0;
            for (; (innerIndex & 0xFFFF) < (innerCount & 0xFFFF); innerIndex = innerIndex + 1) {
                innerObj = fightTrainerGetValidFightPokemonPtr(outerObj, innerIndex);
                if (innerObj != 0) {
                    statObj = fightPokemonGetPokemonPtr(innerObj);
                    if ((statObj != 0) && (status = fightPokemonCheckFightOut(innerObj), status != 0)) {
                        total = total + (pokemonGetStatus(statObj, 0, 0x83, 0) & 0xFFFF);
                    }
                }
            }
        }
    }
    return total;
}

#endif

#if defined(FIGHT_SIDE_801F7258_801F72B0)

/* 0x801F7258 | size: 0x58 | small */
u32 fightSideGetValidFightTrainerPtr(u32 param_1) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32);
    extern u8 fightTrainerCheckValid(u32);
    u32 uVar1;
    u8 cVar2;

    if (param_1 == 0) {
        uVar1 = 0;
    } else {
        uVar1 = fightSideBiosGetFightTrainerPtr(param_1);
    }
    cVar2 = fightTrainerCheckValid(uVar1);
    if (cVar2 == 0) {
        return 0;
    }
    return uVar1;
}

#endif

#if defined(FIGHT_SIDE_801F72B0_801F7388)

/* 0x801F72B0 | size: 0xD8 | medium */
void fightSideGetFightTrainerGridParam(u32 param_1, u16 param_2, u16 param_3, u8* param_4, s8* param_5) {
    extern u32 fightSideDataBiosGetPtr(u32);
    extern u16 fightSideDataBiosGetYrot(u32);
    u32 uVar1;
    u16 uVar2;

    if (param_4 == NULL) return;
    if (param_5 == NULL) return;
    uVar1 = fightSideDataBiosGetPtr(param_1);
    if (uVar1 == 0) {
        uVar2 = 0;
    } else {
        uVar2 = fightSideDataBiosGetYrot(uVar1);
    }
    *param_4 = (s8)uVar2;
    *param_5 = 0;
    if ((u16)param_2 == 1) return;
    if ((u16)param_2 != 2) return;
    if (*param_4 == 1) {
        if ((u16)param_3 == 0) {
            *param_5 = 1;
        } else if ((u16)param_3 == 1) {
            *param_5 = -1;
        }
    } else {
        if ((u16)param_3 == 0) {
            *param_5 = -1;
        } else if ((u16)param_3 == 1) {
            *param_5 = 1;
        }
    }
}

#endif

#if defined(FIGHT_SIDE_801F7388_801F75F8)

u8 fightSideGetDoFightTrainerCount(u32 param_1) {
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u8 fightTrainerCheckDoFight(u32);
    u32 uVar2;
    u8 cVar3;
    u32 bVar4;
    u8 cVar1;

    cVar1 = 0;
    bVar4 = 0;
    while ((u8)bVar4 < 2) {
        if (param_1 == 0) {
            uVar2 = 0;
        } else {
            uVar2 = fightSideBiosGetFightTrainerPtr(param_1, (u8)(bVar4 & 0xFFFFFFFFFFFFFFFFu));
        }
        cVar3 = fightTrainerCheckDoFight(uVar2);
        if ((u8)cVar3 != 0) {
            cVar1 = (u8)((u8)cVar1 + 1);
        }
        bVar4 = bVar4 + 1;
    }
    return cVar1;
}

/* 0x801F7404 | size: 0x7C | small */
u32 fightSideCheckValid(u32 param_1) {
    extern u16 fn_801EF634(void);
    extern u16 fightSideBiosGetFightSideDataId(u32);
    u16 sVar2;
    u32 uVar1;

    if (param_1 == 0) {
        return 0;
    }
    if ((u16)fn_801EF634() == 1) {
        return 0;
    }
    if (param_1 == 0) {
        uVar1 = 0;
    } else {
        sVar2 = fightSideBiosGetFightSideDataId(param_1);
        uVar1 = (u16)sVar2;
    }
    if ((s32)uVar1 == 0) {
        return 0;
    }
    return 1;
}

/* 0x801F7480 | size: 0xB0 | medium */
void fightSideCreate(u32 param_1, u16 param_2) {
    extern void fn_8011B950(u32, u32);
    extern void fightSideBiosSetMakibisiCheckFlag(u32, u32);
    extern void fightSideBiosSetFightSideDataId(u32, u16);
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u32 fightSideBiosGetJoutaiPtr(u32, u32);
    extern void fightTrainerInit(u32, u32);
    u32 uVar1;

    if (param_1 != 0) {
        if (param_1 != 0) {
            if (param_1 != 0) {
                fightSideBiosSetFightSideDataId(param_1, 0);
            }
            if (param_1 == 0) {
                uVar1 = 0;
            } else {
                uVar1 = fightSideBiosGetJoutaiPtr(param_1, 0);
            }
            fn_8011B950(uVar1, 6);
            if (param_1 == 0) {
                uVar1 = 0;
            } else {
                uVar1 = fightSideBiosGetFightTrainerPtr(param_1, 0);
            }
            fightTrainerInit(uVar1, 2);
            if (param_1 != 0) {
                fightSideBiosSetMakibisiCheckFlag(param_1, 0);
            }
        }
        if (param_1 != 0) {
            fightSideBiosSetFightSideDataId(param_1, (u16)(param_2 & 0xFFFFFFFFu));
        }
    }
}

/* 0x801F7530 | size: 0xC8 | medium */
void fightSideInitAry(u32 param_1, u16 param_2) {
    extern void fn_8011B950(u32, u32);
    extern void fightSideBiosSetMakibisiCheckFlag(u32, u32);
    extern void fightSideBiosSetFightSideDataId(u32, u16);
    extern u32 fightSideBiosGetFightTrainerPtr(u32, u32);
    extern u32 fightSideBiosGetJoutaiPtr(u32, u32);
    extern void fightTrainerInit(u32, u32);
    u32 uVar1;
    int uVar2;
    u32 uVar3;

    if (param_1 != 0) {
        uVar2 = 0;
        while ((u16)uVar2 < (u16)param_2) {
            uVar3 = (u32)(uVar2 & 0xFFFF);
            uVar3 = param_1 + (uVar3 * 0x5230);
            if (uVar3 != 0) {
                if (uVar3 != 0) {
                    fightSideBiosSetFightSideDataId(uVar3, 0);
                }
                if (uVar3 == 0) {
                    uVar1 = 0;
                } else {
                    uVar1 = fightSideBiosGetJoutaiPtr((0, uVar3), 0);
                }
                fn_8011B950(uVar1, 6);
                if ((uVar3 == 0) != 0U) {
                    uVar1 = 0;
                } else {
                    uVar1 = fightSideBiosGetFightTrainerPtr(uVar3, 0);
                }
                fightTrainerInit(uVar1, 2);
                if (uVar3 != (0x5230 * 0)) {
                    fightSideBiosSetMakibisiCheckFlag(uVar3, 0);
                }
            }
            uVar2 = uVar2 + 1;
        }
    }
}

#endif

#if defined(FIGHT_SIDE_801F75F8_801F76B8)

/* 0x801F75F8 | size: 0xC4 | medium */
void fightSideSetStatus(u8* ptr1, u32 param2, u32 slotType, u32 param4, u32 param5) {
    extern u8* fightSideDataBiosGetPtr(u32);
    extern void fightSideDataBiosSetYrot(u8*, u32);
    extern void fightSideDataBiosSetFightTrainerStatusMenuDataId(u8*, u32, u32);
    extern void fightSideDataBiosSetFightoutPokemonStatusMenuDataId(u8*, u32, u32);
    extern void fightSideBiosSetFightSideDataId(u8*, u32);
    extern void fightSideBiosSetMakibisiCheckFlag(u8*, u32);

    if ((u16)slotType == 0) {
        return;
    }
    if ((u16)slotType < 0xA) {
        ;
    } else {
        return;
    }
    if ((u16)slotType < 4) {
        ptr1 = fightSideDataBiosGetPtr(param2);
    }
    if (ptr1 == NULL) {
        return;
    }
    switch ((u16)slotType) {
    case 1:
        fightSideDataBiosSetYrot(ptr1, (u16)param5);
        break;
    case 2:
        fightSideDataBiosSetFightTrainerStatusMenuDataId(ptr1, (0, (u8)param4), param5);
        break;
    case 3:
        fightSideDataBiosSetFightoutPokemonStatusMenuDataId(ptr1, (0, (u8)param4), param5);
        break;
    case 5:
        fightSideBiosSetFightSideDataId(ptr1, (u16)param5);
        break;
    case 8:
        fightSideBiosSetMakibisiCheckFlag(ptr1, (u8)param5);
        break;
    }
}

#endif

#if defined(FIGHT_SIDE_801F76B8_801F7798)

/* 0x801F76B8 | size: 0xE0 | fightSideGetStatus: per-side status dispatcher */
u32 fightSideGetStatus(u8* ptr1, u32 param2, u32 slotType, u32 param5) {
    extern u32 fightSideDataBiosGetFightoutPokemonStatusMenuDataId(u8*, u32);
    extern u32 fightSideDataBiosGetFightTrainerStatusMenuDataId(u8*, u32);
    extern u32 fightSideDataBiosGetYrot(u8*);
    extern u8* fightSideDataBiosGetPtr(u32);
    extern u32 fightSideBiosGetMakibisiCheckFlag(u8*);
    extern u32 fightSideBiosGetFightTrainerPtr(u8*, u32);
    extern u32 fightSideBiosGetJoutaiPtr(u8*, u32);
    extern u32 fightSideBiosGetFightSideDataId(u8*);

    if ((u16)slotType == 0 || (u16)slotType >= 0xA) {
        return 0;
    }
    if ((u16)slotType < 4) {
        ptr1 = fightSideDataBiosGetPtr(param2);
    }
    if (ptr1 == NULL) {
        return 0;
    }
    switch ((u16)slotType) {
    case 1:
        return (u16)fightSideDataBiosGetYrot(ptr1);
    case 2:
        return fightSideDataBiosGetFightTrainerStatusMenuDataId(ptr1, (u8)param5);
    case 3:
        return fightSideDataBiosGetFightoutPokemonStatusMenuDataId(ptr1, (u8)param5);
    case 5:
        return (u16)fightSideBiosGetFightSideDataId(ptr1);
    case 6:
        return fightSideBiosGetJoutaiPtr(ptr1, param5);
    case 7:
        return fightSideBiosGetFightTrainerPtr(ptr1, param5);
    case 8:
        return (u8)fightSideBiosGetMakibisiCheckFlag(ptr1);
    default:
        return 0;
    }
}

#endif
