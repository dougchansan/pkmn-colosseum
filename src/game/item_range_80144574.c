/**
 * @file item_range_80144574.c
 * @brief item-use game code, 0x80144574 - 0x8014635C (1 fn).
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). fn_80144574
 * (size 0x1DE8) is item-use-on-Pokemon logic (calls hpRecover__FP20...);
 * it ends exactly at seqGetPrivateId (0x8014635C), the first MusyX seq.c
 * function. Asm-only until matched; the range name stays honest until the
 * function is decompiled.
 */
#include "dolphin/types.h"

typedef struct ItemUsePokemonLog {
    s32 type;
    u16 value;
    u16 extra;
} ItemUsePokemonLog;

#define ADD_LOG(_log, _count, _type, _value, _extra)         \
    do {                                                     \
        if ((_count) < 0x20) {                               \
            (_log)[(_count)].type = (_type);                 \
            (_log)[(_count)].value = (u16)(_value);          \
            (_log)[(_count)].extra = (u16)(_extra);          \
            (_count)++;                                      \
        }                                                    \
    } while (0)

extern u8* itemDataBiosGetPtr(u16 index);
extern u8 itemDataBiosGetItemEffectParam(u8* item);
extern void itemParamConvertOrigFormat(u8* dst, u8* src);
extern u8* itemParamGetPtr(u8 idx);
extern s32 itemParamGetMeromeroFlag(u8* p);
extern s32 itemParamGetCriticalFlag(u8* p);
extern u8 itemParamGetAttackUp(u8* p);
extern u8 itemParamGetDefenceUp(u8* p);
extern u8 itemParamGetQuickUp(u8* p);
extern u8 itemParamGetHitUp(u8* p);
extern u8 itemParamGetSpAttackUp(u8* p);
extern s32 itemParamGetGuardFlag(u8* p);
extern s32 itemParamGetLevelUpFlag(u8* p);
extern s32 itemParamGetSleepFlag(u8* p);
extern s32 itemParamGetPoisonFlag(u8* p);
extern s32 itemParamGetBurnFlag(u8* p);
extern s32 itemParamGetFreezeFlag(u8* p);
extern s32 itemParamGetParalyzeFlag(u8* p);
extern s32 itemParamGetConfuseFlag(u8* p);
extern s32 itemParamGetPPMaxUpFlag(u8* p);
extern s32 itemParamGetPPMaxFullFlag(u8* p);
extern u8 itemParamGetHPEffortUp(u8* p);
extern u8 itemParamGetAttackEffortUp(u8* p);
extern u8 itemParamGetDefenceEffortUp(u8* p);
extern u8 itemParamGetQuickEffortUp(u8* p);
extern u8 itemParamGetSpDefenceEffortUp(u8* p);
extern u8 itemParamGetSpAttackEffortUp(u8* p);
extern u8 itemParamGetHPUp(u8* p);
extern s32 itemParamGetReviveFlag(u8* p);
extern u8 itemParamGetPPUp(u8* p);
extern s32 itemParamGetPPSelectFlag(u8* p);
extern s32 itemParamGetEvolutionFlag(u8* p);
extern s8 itemParamGetFriend1Up(u8* p);
extern s8 itemParamGetFriend2Up(u8* p);
extern s8 itemParamGetFriend3Up(u8* p);
extern void* fightFloorGetFightPokemonPtrToFightTrainerPtr(s32, void*);
extern void* fightTrainerCheckFightPokemonFightOut(void*, void*);
extern void fightSeqCheckSetMotoPokemonToHensinBuff(void*, s32);
extern void fn_802331A4(void*, s32);
extern void fn_8020248C(void*, s32, u32);
extern u8 fn_802025B8(void*, s32);
extern u8 fightOutPokemonIsJoutaiDataId(void* fightPokemon, s32 statusId);
extern void fightOutPokemonInitJoutaiDataId(void* fightPokemon, s32 statusId);
extern u8 fightOutPokemonCheckWriteJoutaiDataId(void* fightPokemon, s32 statusId);
extern void fightOutPokemonWriteJoutaiDataId(void* fightPokemon, s32 statusId, u32 value);
extern u8 fn_80121ADC(void*, s32);
extern void fn_80121B4C(void*, s32);
extern void* fightTargetGetPtrAsNowFightType(s32 type, void* fightPokemon);
extern u8 fightSideCheckWriteJoutaiDataId(void* side, s32 statusId);
extern void fightSideWriteJoutaiDataId(void* side, s32 statusId, u32 value);
extern void* fightPokemonGetPokemonPtr(void* fightPokemon);
extern void* fightPokemonBiosGetMotoPokemonPtr(void* fightPokemon);
extern void hpRecover__FP20ITEMUSE2POKEMON_LOG1PsP7PokemonUcbUsP12FightPokemon(
    ItemUsePokemonLog* log, s16* logCount, void* pokemon, u8 recovery,
    u8 revive, u16 amount, void* fightPokemon);
extern s32 pokemonGetStatus(void* obj, u32 id, u32 selector, u32 subindex);
extern void pokemonSetStatus(
    void* obj, u32 id, u32 selector, u32 subindex, u32 value);
extern void pokemonResetBasisStatus(void* pokemon);
extern u8 pokemonIsDarkPokemon(void* pokemon);
extern u8 pokemonIsJoutaiDataId(void* pokemon, s32 statusId);
extern void pokemonInitJoutaiDataId(void* pokemon, s32 statusId);
extern u8 pokemonWazaCheckValid(void* pokemon, u8 wazaNum);
extern u8 pokemonWazaGetMaxPP(void* pokemon, u8 wazaNum);
extern u8 pokemonBiosGetPokemonWazaPp(void* pokemon, u8 wazaNum);
extern void pokemonBiosSetPokemonWazaPp(void* pokemon, u8 wazaNum, u8 value);
extern u8 pokemonBiosGetPokemonWazaPpCount(void* pokemon, u8 wazaNum);
extern void pokemonBiosSetPokemonWazaPpCount(void* pokemon, u8 wazaNum, u8 value);
extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u8 wazaNum);
extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
extern u8 pokemonBiosGetLevel(void* pokemon);
extern u16 pokemonBiosGetMaxHp(void* pokemon);
extern u16 pokemonBiosGetMaxHpEffort(void* pokemon);
extern u16 pokemonBiosGetPhyAtkEffort(void* pokemon);
extern u16 pokemonBiosGetPhyDefEffort(void* pokemon);
extern u16 pokemonBiosGetSpeAtkEffort(void* pokemon);
extern u16 pokemonBiosGetSpeDefEffort(void* pokemon);
extern u16 pokemonBiosGetNimblenessEffort(void* pokemon);
extern void pokemonBiosSetExp(void* pokemon, u32 exp);
extern u8 pokemonDataBiosGetGrowDataId(u8* data);
extern u8* pokemonDataBiosGetPtr(u16 species);
extern u32 pokemonGrowDataBiosGetExp(void* growData, u8 level);
extern void* pokemonGrowDataBiosGetPtr(u8 growDataId);
extern s32 pokemonEvolutionCheck(
    void* pokemon, s32 mode, u16 trigger, u16* outSpecies, u8* outMode);
extern u8 wazaCheckDarkWaza(u16 wazaDataId);
extern u8 wazaDB_GetPp(u16 wazaDataId);
extern s16 friendXUp__FP7PokemonP12FightPokemonScUsUs(
    void* pokemon, void* fightPokemon, s8 delta, u16 minFriend, u16 maxFriend);

#define origRamSeedParam_1965_804E8440 ((u8*)0x804E8440)

s16 fn_80144574(
    ItemUsePokemonLog* log, void* pokemon, void* fightPokemon,
    u16 itemDataId, u8 moveSlot)
{
    u8 convertedParam[8];
    u8* itemParam;
    void* activePokemon;
    void* trainer;
    void* side;
    void* growthData;
    u16 totalEffort;
    u16 oldEffort;
    u16 newEffort;
    u16 levelBeforeHp;
    u16 levelAfterHp;
    u16 evolutionSpecies;
    u8 evolutionMode;
    u8 boost;
    u8 oldStage;
    u8 newStage;
    u8 delta;
    u8 oldPp;
    u8 newPp;
    u8 maxPp;
    u8 moveIdx;
    u8 battleSleepCleared;
    u8 poisonCleared;
    u8 badPoisonCleared;
    s16 logCount;
    s16 friendDelta;
    u16 wazaDataId;

    evolutionSpecies = 0;
    evolutionMode = 0;
    logCount = 0;
    activePokemon = pokemon;

    if (itemDataId != 0xAF) {
        itemParam = itemDataBiosGetPtr(itemDataId);
        if (itemParam == NULL) {
            itemParam = NULL;
        } else if (itemDataBiosGetItemEffectParam(itemParam) == 0) {
            itemParam = NULL;
        } else {
            itemParam = itemParamGetPtr(itemDataBiosGetItemEffectParam(itemParam));
        }
    } else {
        itemParamConvertOrigFormat(convertedParam, origRamSeedParam_1965_804E8440);
        itemParam = convertedParam;
    }

    if (itemParam == NULL) {
        return 0;
    }

    if (fightPokemon != NULL) {
        activePokemon = fightPokemonGetPokemonPtr(fightPokemon);
    }

    if (itemParamGetMeromeroFlag(itemParam) != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL && fightOutPokemonIsJoutaiDataId(trainer, 0xA) != 0) {
            fightOutPokemonInitJoutaiDataId(trainer, 0xA);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x7C);
                }
            }
            ADD_LOG(log, logCount, 0, 0, 0);
        }
    }

    if (itemParamGetCriticalFlag(itemParam) != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL && fn_802025B8(trainer, 0xF) == 2) {
            fn_8020248C(trainer, 0xF, 0);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x7C);
                }
            }
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x99);
                }
            }
            ADD_LOG(log, logCount, 1, 0, 0);
        }
    }

    boost = itemParamGetAttackUp(itemParam);
    if (boost != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL) {
            oldStage = (u8)pokemonGetStatus(trainer, 0, 0xE6, 0);
            if (oldStage < 0xC) {
                newStage = oldStage + boost;
                if (newStage > 0xC) {
                    newStage = 0xC;
                }
                pokemonSetStatus(trainer, 0, 0xE6, 0, newStage);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x99);
                    }
                }
                delta = newStage - oldStage;
                if (delta != 0) {
                    ADD_LOG(log, logCount, 2, delta, 0);
                }
            }
        }
    }

    boost = itemParamGetDefenceUp(itemParam);
    if (boost != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL) {
            oldStage = (u8)pokemonGetStatus(trainer, 0, 0xE7, 0);
            if (oldStage < 0xC) {
                newStage = oldStage + boost;
                if (newStage > 0xC) {
                    newStage = 0xC;
                }
                pokemonSetStatus(trainer, 0, 0xE7, 0, newStage);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x99);
                    }
                }
                delta = newStage - oldStage;
                if (delta != 0) {
                    ADD_LOG(log, logCount, 3, delta, 0);
                }
            }
        }
    }

    boost = itemParamGetQuickUp(itemParam);
    if (boost != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL) {
            oldStage = (u8)pokemonGetStatus(trainer, 0, 0xEA, 0);
            if (oldStage < 0xC) {
                newStage = oldStage + boost;
                if (newStage > 0xC) {
                    newStage = 0xC;
                }
                pokemonSetStatus(trainer, 0, 0xEA, 0, newStage);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x99);
                    }
                }
                delta = newStage - oldStage;
                if (delta != 0) {
                    ADD_LOG(log, logCount, 4, delta, 0);
                }
            }
        }
    }

    boost = itemParamGetHitUp(itemParam);
    if (boost != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL) {
            oldStage = (u8)pokemonGetStatus(trainer, 0, 0xEB, 0);
            if (oldStage < 0xC) {
                newStage = oldStage + boost;
                if (newStage > 0xC) {
                    newStage = 0xC;
                }
                pokemonSetStatus(trainer, 0, 0xEB, 0, newStage);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x99);
                    }
                }
                delta = newStage - oldStage;
                if (delta != 0) {
                    ADD_LOG(log, logCount, 5, delta, 0);
                }
            }
        }
    }

    boost = itemParamGetSpAttackUp(itemParam);
    if (boost != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL) {
            oldStage = (u8)pokemonGetStatus(trainer, 0, 0xE8, 0);
            if (oldStage < 0xC) {
                newStage = oldStage + boost;
                if (newStage > 0xC) {
                    newStage = 0xC;
                }
                pokemonSetStatus(trainer, 0, 0xE8, 0, newStage);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x99);
                    }
                }
                delta = newStage - oldStage;
                if (delta != 0) {
                    ADD_LOG(log, logCount, 6, delta, 0);
                }
            }
        }
    }

    if (itemParamGetGuardFlag(itemParam) != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL) {
            side = fightTargetGetPtrAsNowFightType(2, trainer);
            if (fightSideCheckWriteJoutaiDataId(side, 0x4C) == 2) {
                fightSideWriteJoutaiDataId(side, 0x4C, 0);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x99);
                    }
                }
                ADD_LOG(log, logCount, 7, 5, 0);
            }
        }
    }

    levelAfterHp = 0;
    if (itemParamGetLevelUpFlag(itemParam) != 0 && pokemonIsDarkPokemon(activePokemon) == 0) {
        boost = pokemonBiosGetLevel(activePokemon);
        if (boost < 100) {
            levelBeforeHp = pokemonBiosGetMaxHp(activePokemon);
            growthData = pokemonGrowDataBiosGetPtr(
                pokemonDataBiosGetGrowDataId(
                    pokemonDataBiosGetPtr(
                        pokemonBiosGetPokemonDataId(activePokemon))));
            pokemonBiosSetExp(activePokemon,
                pokemonGrowDataBiosGetExp(growthData, (u8)(boost + 1)));
            pokemonResetBasisStatus(activePokemon);
            levelAfterHp = pokemonBiosGetMaxHp(activePokemon) - levelBeforeHp;
            ADD_LOG(log, logCount, 8, 0, 0);
        }
    }

    if (itemParamGetSleepFlag(itemParam) != 0) {
        battleSleepCleared = 0;
        if (pokemonIsJoutaiDataId(activePokemon, 8) != 0) {
            pokemonInitJoutaiDataId(activePokemon, 8);
            if (fightPokemon != NULL) {
                trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(
                    0, fightPokemon);
                trainer = fightTrainerCheckFightPokemonFightOut(
                    trainer, fightPokemon);
                if (trainer != NULL) {
                    ((void (*)(void*, s32))fightOutPokemonWriteJoutaiDataId)(
                        trainer, 0x17);
                    battleSleepCleared = 1;
                }
            }
            ADD_LOG(log, logCount, 9, 0, 0);
            if (battleSleepCleared != 0) {
                ADD_LOG(log, logCount, 0xA, 0, 0);
            }
        }
    }

    if (itemParamGetPoisonFlag(itemParam) != 0) {
        poisonCleared = 0;
        badPoisonCleared = 0;
        if (fn_80121ADC(activePokemon, 3) != 0) {
            fn_80121B4C(activePokemon, 3);
            poisonCleared = 1;
        }
        if (fn_80121ADC(activePokemon, 4) != 0) {
            fn_80121B4C(activePokemon, 4);
            badPoisonCleared = 1;
        }
        if (fightPokemon != NULL) {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
            if (trainer != NULL) {
                fightSeqCheckSetMotoPokemonToHensinBuff(trainer, 0x7C);
            }
        }
        if (poisonCleared != 0) {
            ADD_LOG(log, logCount, 0xB, 0, 0);
        }
        if (badPoisonCleared != 0) {
            ADD_LOG(log, logCount, 0xC, 0, 0);
        }
    }

    if (itemParamGetBurnFlag(itemParam) != 0 &&
        pokemonIsJoutaiDataId(activePokemon, 6) != 0) {
        pokemonInitJoutaiDataId(activePokemon, 6);
        if (fightPokemon != NULL) {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
            if (trainer != NULL) {
                fightSeqCheckSetMotoPokemonToHensinBuff(trainer, 0x7C);
            }
        }
        ADD_LOG(log, logCount, 0xE, 0, 0);
    }

    if (itemParamGetFreezeFlag(itemParam) != 0 &&
        pokemonIsJoutaiDataId(activePokemon, 7) != 0) {
        pokemonInitJoutaiDataId(activePokemon, 7);
        if (fightPokemon != NULL) {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
            if (trainer != NULL) {
                fightSeqCheckSetMotoPokemonToHensinBuff(trainer, 0x7C);
            }
        }
        ADD_LOG(log, logCount, 0xF, 0, 0);
    }

    if (itemParamGetParalyzeFlag(itemParam) != 0 &&
        pokemonIsJoutaiDataId(activePokemon, 5) != 0) {
        pokemonInitJoutaiDataId(activePokemon, 5);
        if (fightPokemon != NULL) {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
            if (trainer != NULL) {
                fn_802331A4(trainer, 0x7C);
            }
        }
        ADD_LOG(log, logCount, 0x10, 0, 0);
    }

    if (itemParamGetConfuseFlag(itemParam) != 0) {
        if (fightPokemon == NULL) {
            trainer = NULL;
        } else {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
            trainer = fightTrainerCheckFightPokemonFightOut(trainer, fightPokemon);
        }
        if (trainer != NULL && fightOutPokemonIsJoutaiDataId(trainer, 9) != 0) {
            fightOutPokemonInitJoutaiDataId(trainer, 9);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fightSeqCheckSetMotoPokemonToHensinBuff(side, 0x7C);
                }
            }
            ADD_LOG(log, logCount, 0x11, 0, 0);
        }
    }

    if (itemParamGetPPMaxUpFlag(itemParam) != 0) {
        wazaDataId = pokemonBiosGetPokemonWazaDataId(activePokemon, moveSlot);
        if ((u16)(wazaDataId - 0x164) > 1 &&
            pokemonWazaCheckValid(activePokemon, moveSlot) != 0) {
            oldPp = pokemonBiosGetPokemonWazaPpCount(activePokemon, moveSlot);
            if (oldPp < 3) {
                maxPp = pokemonWazaGetMaxPP(activePokemon, moveSlot);
                pokemonBiosSetPokemonWazaPpCount(activePokemon, moveSlot, (u8)(oldPp + 1));
                delta = pokemonWazaGetMaxPP(activePokemon, moveSlot) - maxPp;
                pokemonBiosSetPokemonWazaPp(
                    activePokemon, moveSlot,
                    (u8)(pokemonBiosGetPokemonWazaPp(activePokemon, moveSlot) + delta));
                ADD_LOG(log, logCount, 0x12, delta, moveSlot);
            }
        }
    }

    boost = itemParamGetHPEffortUp(itemParam);
    if (boost != 0) {
        if (pokemonBiosGetPokemonDataId(activePokemon) == 0x12F) {
            delta = 1;
        } else {
            totalEffort = pokemonBiosGetMaxHpEffort(activePokemon) +
                          pokemonBiosGetPhyAtkEffort(activePokemon) +
                          pokemonBiosGetPhyDefEffort(activePokemon) +
                          pokemonBiosGetSpeAtkEffort(activePokemon) +
                          pokemonBiosGetSpeDefEffort(activePokemon) +
                          pokemonBiosGetNimblenessEffort(activePokemon);
            if (totalEffort >= 0x1FE) {
                return 0;
            }
            oldEffort = (u16)pokemonGetStatus(activePokemon, 0, 0x8D, 0);
            if (oldEffort < 0x64) {
                newEffort = oldEffort + boost;
                if (newEffort > 0x64) {
                    newEffort = 0x64;
                }
                if ((u16)(totalEffort - oldEffort + newEffort) > 0x1FE) {
                    newEffort -= (u16)(totalEffort - oldEffort + newEffort) - 0x1FE;
                }
                pokemonSetStatus(activePokemon, 0, 0x8D, 0, (u8)newEffort);
                pokemonResetBasisStatus(activePokemon);
                if (fightPokemon != NULL) {
                    side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                    side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                    if (side != NULL) {
                        fn_802331A4(side, 0x8D);
                    }
                }
                delta = (u8)(newEffort - oldEffort);
                if (delta != 0) {
                    ADD_LOG(log, logCount, 0x13, delta, 0);
                }
            }
        }
    }

    boost = itemParamGetAttackEffortUp(itemParam);
    if (boost != 0) {
        totalEffort = pokemonBiosGetMaxHpEffort(activePokemon) +
                      pokemonBiosGetPhyAtkEffort(activePokemon) +
                      pokemonBiosGetPhyDefEffort(activePokemon) +
                      pokemonBiosGetSpeAtkEffort(activePokemon) +
                      pokemonBiosGetSpeDefEffort(activePokemon) +
                      pokemonBiosGetNimblenessEffort(activePokemon);
        if (totalEffort >= 0x1FE) {
            return 0;
        }
        oldEffort = (u16)pokemonGetStatus(activePokemon, 0, 0x8E, 0);
        if (oldEffort < 0x64) {
            newEffort = oldEffort + boost;
            if (newEffort > 0x64) {
                newEffort = 0x64;
            }
            if ((u16)(totalEffort - oldEffort + newEffort) > 0x1FE) {
                newEffort -= (u16)(totalEffort - oldEffort + newEffort) - 0x1FE;
            }
            pokemonSetStatus(activePokemon, 0, 0x8E, 0, (u8)newEffort);
            pokemonResetBasisStatus(activePokemon);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x8E);
                }
            }
            delta = (u8)(newEffort - oldEffort);
            if (delta != 0) {
                ADD_LOG(log, logCount, 0x14, delta, 0);
            }
        }
    }

    boost = itemParamGetHPUp(itemParam);
    if (boost != 0) {
        hpRecover__FP20ITEMUSE2POKEMON_LOG1PsP7PokemonUcbUsP12FightPokemon(
            log, &logCount, activePokemon, boost,
            (u8)itemParamGetReviveFlag(itemParam), levelAfterHp, fightPokemon);
    }

    boost = itemParamGetPPUp(itemParam);
    if (boost != 0) {
        moveIdx = 0xFF;
        if (itemParamGetPPSelectFlag(itemParam) != 0) {
            moveIdx = moveSlot;
        }
        trainer = activePokemon;
        if (pokemonIsJoutaiDataId(activePokemon, 0x10) != 0 ||
            pokemonIsJoutaiDataId(activePokemon, 0x31) != 0) {
            trainer = fightPokemonBiosGetMotoPokemonPtr(fightPokemon);
        }
        if (moveIdx != 0xFF) {
            wazaDataId = pokemonBiosGetPokemonWazaDataId(trainer, moveIdx);
            if (wazaDataId != 0x176 && wazaCheckDarkWaza(wazaDataId) == 0) {
                maxPp = pokemonWazaGetMaxPP(trainer, moveIdx);
                oldPp = pokemonBiosGetPokemonWazaPp(trainer, moveIdx);
                if (oldPp < maxPp) {
                    newPp = oldPp + boost;
                    if (newPp > maxPp) {
                        newPp = maxPp;
                    }
                    pokemonBiosSetPokemonWazaPp(trainer, moveIdx, newPp);
                    delta = (u8)(newPp - oldPp);
                    if (delta != 0) {
                        ADD_LOG(log, logCount, 0x17, delta, moveIdx);
                    }
                }
            }
        } else {
            moveIdx = 0;
            while (moveIdx < 4) {
                wazaDataId = pokemonBiosGetPokemonWazaDataId(trainer, moveIdx);
                if (wazaDataId != 0x176 && wazaCheckDarkWaza(wazaDataId) == 0) {
                    maxPp = pokemonWazaGetMaxPP(trainer, moveIdx);
                    oldPp = pokemonBiosGetPokemonWazaPp(trainer, moveIdx);
                    if (oldPp < maxPp) {
                        newPp = oldPp + boost;
                        if (newPp > maxPp) {
                            newPp = maxPp;
                        }
                        pokemonBiosSetPokemonWazaPp(trainer, moveIdx, newPp);
                        delta = (u8)(newPp - oldPp);
                        if (delta != 0) {
                            ADD_LOG(log, logCount, 0x17, delta, moveIdx);
                        }
                    }
                }
                moveIdx++;
            }
        }
    }

    if (itemParamGetEvolutionFlag(itemParam) != 0 &&
        pokemonIsDarkPokemon(activePokemon) == 0) {
        if (pokemonEvolutionCheck(
                activePokemon, 1, itemDataId,
                &evolutionSpecies, &evolutionMode) != 0 ||
            evolutionSpecies != 0) {
            ADD_LOG(log, logCount, 0x18, evolutionMode, evolutionSpecies);
        }
    }

    boost = itemParamGetDefenceEffortUp(itemParam);
    if (boost != 0) {
        totalEffort = pokemonBiosGetMaxHpEffort(activePokemon) +
                      pokemonBiosGetPhyAtkEffort(activePokemon) +
                      pokemonBiosGetPhyDefEffort(activePokemon) +
                      pokemonBiosGetSpeAtkEffort(activePokemon) +
                      pokemonBiosGetSpeDefEffort(activePokemon) +
                      pokemonBiosGetNimblenessEffort(activePokemon);
        if (totalEffort >= 0x1FE) {
            return 0;
        }
        oldEffort = (u16)pokemonGetStatus(activePokemon, 0, 0x8F, 0);
        if (oldEffort < 0x64) {
            newEffort = oldEffort + boost;
            if (newEffort > 0x64) {
                newEffort = 0x64;
            }
            if ((u16)(totalEffort - oldEffort + newEffort) > 0x1FE) {
                newEffort -= (u16)(totalEffort - oldEffort + newEffort) - 0x1FE;
            }
            pokemonSetStatus(activePokemon, 0, 0x8F, 0, (u8)newEffort);
            pokemonResetBasisStatus(activePokemon);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x8F);
                }
            }
            delta = (u8)(newEffort - oldEffort);
            if (delta != 0) {
                ADD_LOG(log, logCount, 0x19, delta, 0);
            }
        }
    }

    boost = itemParamGetQuickEffortUp(itemParam);
    if (boost != 0) {
        totalEffort = pokemonBiosGetMaxHpEffort(activePokemon) +
                      pokemonBiosGetPhyAtkEffort(activePokemon) +
                      pokemonBiosGetPhyDefEffort(activePokemon) +
                      pokemonBiosGetSpeAtkEffort(activePokemon) +
                      pokemonBiosGetSpeDefEffort(activePokemon) +
                      pokemonBiosGetNimblenessEffort(activePokemon);
        if (totalEffort >= 0x1FE) {
            return 0;
        }
        oldEffort = (u16)pokemonGetStatus(activePokemon, 0, 0x92, 0);
        if (oldEffort < 0x64) {
            newEffort = oldEffort + boost;
            if (newEffort > 0x64) {
                newEffort = 0x64;
            }
            if ((u16)(totalEffort - oldEffort + newEffort) > 0x1FE) {
                newEffort -= (u16)(totalEffort - oldEffort + newEffort) - 0x1FE;
            }
            pokemonSetStatus(activePokemon, 0, 0x92, 0, (u8)newEffort);
            pokemonResetBasisStatus(activePokemon);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x92);
                }
            }
            delta = (u8)(newEffort - oldEffort);
            if (delta != 0) {
                ADD_LOG(log, logCount, 0x1A, delta, 0);
            }
        }
    }

    boost = itemParamGetSpDefenceEffortUp(itemParam);
    if (boost != 0) {
        totalEffort = pokemonBiosGetMaxHpEffort(activePokemon) +
                      pokemonBiosGetPhyAtkEffort(activePokemon) +
                      pokemonBiosGetPhyDefEffort(activePokemon) +
                      pokemonBiosGetSpeAtkEffort(activePokemon) +
                      pokemonBiosGetSpeDefEffort(activePokemon) +
                      pokemonBiosGetNimblenessEffort(activePokemon);
        if (totalEffort >= 0x1FE) {
            return 0;
        }
        oldEffort = (u16)pokemonGetStatus(activePokemon, 0, 0x91, 0);
        if (oldEffort < 0x64) {
            newEffort = oldEffort + boost;
            if (newEffort > 0x64) {
                newEffort = 0x64;
            }
            if ((u16)(totalEffort - oldEffort + newEffort) > 0x1FE) {
                newEffort -= (u16)(totalEffort - oldEffort + newEffort) - 0x1FE;
            }
            pokemonSetStatus(activePokemon, 0, 0x91, 0, (u8)newEffort);
            pokemonResetBasisStatus(activePokemon);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x91);
                }
            }
            delta = (u8)(newEffort - oldEffort);
            if (delta != 0) {
                ADD_LOG(log, logCount, 0x1B, delta, 0);
            }
        }
    }

    boost = itemParamGetSpAttackEffortUp(itemParam);
    if (boost != 0) {
        totalEffort = pokemonBiosGetMaxHpEffort(activePokemon) +
                      pokemonBiosGetPhyAtkEffort(activePokemon) +
                      pokemonBiosGetPhyDefEffort(activePokemon) +
                      pokemonBiosGetSpeAtkEffort(activePokemon) +
                      pokemonBiosGetSpeDefEffort(activePokemon) +
                      pokemonBiosGetNimblenessEffort(activePokemon);
        if (totalEffort >= 0x1FE) {
            return 0;
        }
        oldEffort = (u16)pokemonGetStatus(activePokemon, 0, 0x90, 0);
        if (oldEffort < 0x64) {
            newEffort = oldEffort + boost;
            if (newEffort > 0x64) {
                newEffort = 0x64;
            }
            if ((u16)(totalEffort - oldEffort + newEffort) > 0x1FE) {
                newEffort -= (u16)(totalEffort - oldEffort + newEffort) - 0x1FE;
            }
            pokemonSetStatus(activePokemon, 0, 0x90, 0, (u8)newEffort);
            pokemonResetBasisStatus(activePokemon);
            if (fightPokemon != NULL) {
                side = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon);
                side = fightTrainerCheckFightPokemonFightOut(side, fightPokemon);
                if (side != NULL) {
                    fn_802331A4(side, 0x90);
                }
            }
            delta = (u8)(newEffort - oldEffort);
            if (delta != 0) {
                ADD_LOG(log, logCount, 0x1C, delta, 0);
            }
        }
    }

    if (itemParamGetPPMaxFullFlag(itemParam) != 0) {
        wazaDataId = pokemonBiosGetPokemonWazaDataId(activePokemon, moveSlot);
        if ((u16)(wazaDataId - 0x164) > 1 &&
            pokemonBiosGetPokemonWazaPpCount(activePokemon, moveSlot) < 3) {
            maxPp = pokemonWazaGetMaxPP(activePokemon, moveSlot);
            pokemonBiosSetPokemonWazaPpCount(activePokemon, moveSlot, 3);
            delta = pokemonWazaGetMaxPP(activePokemon, moveSlot) - maxPp;
            if (delta != 0) {
                pokemonBiosSetPokemonWazaPp(
                    activePokemon, moveSlot,
                    (u8)(pokemonBiosGetPokemonWazaPp(activePokemon, moveSlot) + delta));
            }
            ADD_LOG(log, logCount, 0x1D, delta, moveSlot);
        }
    }

    friendDelta = 0;
    boost = (u8)itemParamGetFriend1Up(itemParam);
    if ((s8)boost != 0) {
        if (logCount <= 0) {
            friendDelta = 0;
        } else {
            friendDelta = friendXUp__FP7PokemonP12FightPokemonScUsUs(
                activePokemon, fightPokemon, (s8)boost, 0, 0x64);
            if (friendDelta != 0) {
                ADD_LOG(log, logCount, 0x1E, friendDelta, 0);
            }
        }
    }

    if (friendDelta == 0) {
        boost = (u8)itemParamGetFriend2Up(itemParam);
        if ((s8)boost != 0) {
            if (logCount <= 0) {
                friendDelta = 0;
            } else {
                friendDelta = friendXUp__FP7PokemonP12FightPokemonScUsUs(
                    activePokemon, fightPokemon, (s8)boost, 0x64, 0xC8);
                if (friendDelta != 0) {
                    ADD_LOG(log, logCount, 0x1E, friendDelta, 0);
                }
            }
        }
    }

    if (friendDelta == 0) {
        boost = (u8)itemParamGetFriend3Up(itemParam);
        if ((s8)boost != 0 && logCount > 0) {
            friendDelta = friendXUp__FP7PokemonP12FightPokemonScUsUs(
                activePokemon, fightPokemon, (s8)boost, 0xC8, 0xFFFF);
            if (friendDelta != 0) {
                ADD_LOG(log, logCount, 0x1E, friendDelta, 0);
            }
        }
    }

    return logCount;
}
