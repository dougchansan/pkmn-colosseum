/**
 * @file fight.c
 * @brief fightKouka + fightDataRewrite + fight core section -- split from colosseum_event.c (the fight
 *        engine bucket, 0x80202810-0x80211A00), address range
 *        0x8020D968-0x8020E4E8, 66 fns.
 *
 * fightKouka effect/target/condition data lookups, fightDataRewrite
 * (post-fight hero/pokedoru/mail/fade rewrite), and the FIGHT_WORK
 * fightSet / fightGet accessor farm. Corresponds to XD's fight.cpp
 * tail (0x8020C018-0x8020D858, capped by __sinit_fight_cpp).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

typedef struct FightTypeData {
    u8 trainerNum;
    u8 entryPokemonNum;
    u8 fightoutPokemonNum;
    u8 pad_03;
    u32 name;
} FightTypeData;

typedef struct FightKindData {
    u8 backSaveDataFlag;
    u8 doBadgeCheckFlag;
    u8 doZukanMitaFlag;
    u8 doZukanTukamaetaFlag;
    u8 useItemFlag;
    u8 callFlag;
    u8 nigeruFlag;
    u8 drawFlag;
    u8 getExpFlag;
    u8 getOkaneFlag;
    u8 okanePoolFlag;
    u8 getNekoniKobanFlag;
    u8 getFriendFlag;
    u8 getInfectPokerusFlag;
    u8 doCriticalAttackFlag;
    u8 doHizukiAiFlag;
    u8 doHizukiMiyaburiFlag;
    u8 doItemSoubiTokukoutokubouupFlag;
    u8 keikentihueruFlag;
    u8 bossFlag;
    u8 dorobouFlag;
    u8 monohiroiFlag;
    u8 darkpokemonHypermodeFlag;
    u8 pokemonStatusMenuSubbarFlag;
    u8 hostEnemyMsgFlag;
    u8 pad_19[3];
    u32 name;
} FightKindData;

typedef struct FightKoukaData {
    u16 fightJoukenDataId;
    u16 fightTargetDataId;
    u16 koukaDataId;
} FightKoukaData;

typedef struct FightEncountWipeData {
    u8 snapshotUse;
    u8 pad_01;
    u16 wipeEffectSndId;
    f32 wipeEffectTime;
    u32 wipeFunction;
} FightEncountWipeData;

typedef struct FightEncountTrainerSlot {
    u16 fightTrainerDataId;
    u16 pad_02;
    u32 gsInputDevice;
} FightEncountTrainerSlot;

typedef struct FightEncountData {
    u8 fightKind;
    u8 trainer;
    u8 zenmetuFlag;
    u8 pad_03;
    u16 fightFloorDataId;
    u16 pad_06;
    u32 fightName;
    u32 bgmSndId;
    u32 wipeId;
    u32 syoukaiWzxDataId;
    FightEncountTrainerSlot trainerSlots[4];
} FightEncountData;

typedef struct FightCopyBlock {
    u32 data[12];
} FightCopyBlock;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */
extern FightKoukaData lbl_80375CB8[];
extern FightEncountWipeData fight_encount_wipe_data[];
extern u32* lbl_80478F00;
extern FightTypeData* lbl_80478F04;
extern u32* lbl_80478F40;
extern FightKindData* lbl_80478F44;

/* 0x8020D968 | size: 0x38 | small */
void fn_8020D968(FightCopyBlock* dst, FightCopyBlock* src) {
    if (dst == 0) return;
    if (src == 0) return;
    *dst = *src;
}

/* Address: 0x8020D9A0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightKoukaDataBiosGetKoukaDataId(FightKoukaData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->koukaDataId;
}

/* Address: 0x8020D9B8 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightKoukaDataBiosGetFightTargetDataId(FightKoukaData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->fightTargetDataId;
}

/* Address: 0x8020D9D0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightKoukaDataBiosGetFightJoukenDataId(FightKoukaData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->fightJoukenDataId;
}

/* fightKoukaDataBiosGetPtr | Size: 0x2C | Look up entry in 6-byte table (u16 index) */
FightKoukaData* fightKoukaDataBiosGetPtr(u16 index) {
    extern u32 lbl_80478D50;
    if (index >= lbl_80478D50) {
        return NULL;
    }
    return &lbl_80375CB8[index];
}

/* Address: 0x8020DA14 | Size: 0xbc */
#pragma dont_inline on
u32 fightKoukaDoFightKoukaJoukenAndKouka(void* target, u16 koukaDataIndex) {
    extern void koukaExec(u16 koukaDataId, void* fightTarget, void* target, u32 flags);
    extern void* fightTargetGetPtr(u32 targetDataId, void* target, u16 fightType);
    extern u32 fightFloorGetStatus(u32, u16, u32, u16);
    extern u8 fn_8020A8E0(u32 joukenDataId, void* target);

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
    if (fn_8020A8E0(joukenDataId, target) == 1) {
        koukaExec((u16)koukaDataId, fightTarget, target, 0);
        result = 1;
    }
    return result;
}
#pragma dont_inline reset

/* Address: 0x8020DAD0 | Size: 0x274 | Ghidra import */
#pragma push
#pragma peephole on
u32 fn_8020DAD0(u32 p1) {
    extern void _threadSwitch();
    extern u32 fn_800FF56C();
    extern void fn_800FF730();
    extern void fn_80112700();
    extern void floorSetFadeScript();
    extern void floorSetPrevFloorID();
    extern void fn_80113FE8();
    extern void fn_801140C8();
    extern void heroDecPokedoru();
    extern u32 heroGetStatus();
    extern void msgctrlSetValue();
    extern void scriptSoundStop();
    extern void fn_80165A20();
    extern void fn_80166AB8();
    extern void fn_8018DA88();
    extern u8 fn_801902E0();
    extern void fn_801903B0();
    extern void fn_80190528();
    extern void fadeCheck();
    extern void fadeSetEX(f32, f32, u32, u32, u32);
    extern void fadeSet();
    extern void fn_801D0AFC();
    extern void mailMainReceiveTerminate();
    extern void fn_801EF61C();
    extern void fn_801EF62C();
    extern u32 fn_801EF634();
    extern void fn_801EF7B4();
    extern u8 fightFloorIsGcHeroWin();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 fn_801FCC7C();
    extern u32 fightTrainerDataBiosGetPtr();
    extern u16 fightEncountDataBiosGetWipeEffectSndID();
    extern f32 fightEncountDataBiosGetWipeEffectTime();
    extern u32 fightEncountDataBiosGetWipeSnapshotUse();
    extern u32 fightEncountDataBiosGetWipeFunction();
    extern u32 fightEncountWipeDataBiosGetPtr();
    extern u8 fightEncountDataBiosGetZenmetuFlag();
    extern void fightEncountDataBiosGetWipeId();
    extern u16 fightEncountDataBiosGetFightTrainerDataId();
    extern u32 fightEncountDataBiosGetFightFloorDataId();
    extern u8 fightEncountDataBiosGetFightKind();
    extern u32 fightEncountDataBiosGetPtr();
    extern u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(FightKindData* ptr);
    extern FightKindData* fightKindDataBiosGetPtr(u16 index);
    extern u16 charNameBiosSearchIndex();
    extern u16 charNameBiosGetHearFlag();
    extern f32 lbl_8047E528;
    extern f32 lbl_8047E52C;

    u32 uVar1;
    u32 uVar2;
    u16 uVar7;
    u8 uVar9;
    FightKindData* iVar3;
    u32 trainerData;
    u8 cVar10;
    u16 sVar8;
    u32 uVar4;
    u32 uVar5;
    u32 uVar6;

    if ((p1 & 0xffff) == 0) {
        uVar1 = 0;
    } else {
        uVar1 = fightEncountDataBiosGetPtr();
        fn_801EF62C(0);
        fn_801903B0(0x9b0);
        fn_801EF61C(p1);
        uVar2 = fn_800FF56C();
        fightFloorSetStatus(0, 0, 0x4a, 0, uVar2);
        uVar2 = fightEncountDataBiosGetFightFloorDataId(uVar1);
        uVar7 = fightFloorGetStatus(0, uVar2, 2, 0);
        mailMainReceiveTerminate();
        uVar9 = fightEncountDataBiosGetFightKind(uVar1);
        iVar3 = fightKindDataBiosGetPtr(uVar9);
        if ((iVar3 != 0) && (cVar10 = fightKindDataBiosGetPokemonStatusMenuSubbarFlag(iVar3), cVar10 != 0)) {
            fightEncountDataBiosGetFightTrainerDataId(uVar1, 1);
            trainerData = fightTrainerDataBiosGetPtr();
            if (trainerData != 0) {
                uVar2 = fn_801FCC7C();
                sVar8 = charNameBiosSearchIndex();
                if ((sVar8 != 0) && (sVar8 = charNameBiosGetHearFlag(), sVar8 != 0)) {
                    fn_80190528();
                }
                msgctrlSetValue(0x59, uVar2);
            }
        }
        fn_80165A20(1, 1000, 0xff);
        scriptSoundStop(1000);
        fightEncountDataBiosGetWipeId(uVar1);
        uVar2 = fightEncountWipeDataBiosGetPtr();
        uVar4 = fightEncountDataBiosGetWipeSnapshotUse();
        uVar5 = fightEncountDataBiosGetWipeFunction(uVar2);
        fadeSetEX(lbl_8047E528, fightEncountDataBiosGetWipeEffectTime(uVar2), 9, uVar5, uVar4);
        sVar8 = fightEncountDataBiosGetWipeEffectSndID(uVar2);
        if (sVar8 != 0) {
            fn_80166AB8(sVar8, 0, 0);
        }
        fn_801EF7B4();
        fn_800FF730(uVar7);
        floorSetFadeScript(0, 0);
        _threadSwitch();
        floorSetPrevFloorID(uVar7);
        cVar10 = fightEncountDataBiosGetZenmetuFlag(uVar1);
        if (cVar10 != 0) {
            uVar1 = fn_801EF634();
            cVar10 = fightFloorIsGcHeroWin(0, uVar1);
            if (cVar10 == 0) {
                fn_801EF61C(0);
                fn_801903B0(0xe05);
                uVar6 = heroGetStatus(0, 0xc, 0);
                heroDecPokedoru(0, ((s32)uVar6 >> 1) + (((s32)uVar6 < 0) & (uVar6 & 1)));
                fn_801D0AFC(1);
                fn_8018DA88();
                fn_80113FE8();
                floorSetFadeScript(0, 0x5960008);
                _threadSwitch();
                uVar1 = fn_801EF634();
                return uVar1;
            }
        }
        fn_80190528(0x9b0);
        fn_80112700();
        fn_801140C8();
        cVar10 = fn_801902E0(0xe05);
        if (cVar10 == 0) {
            fadeSet(lbl_8047E52C, 2);
            fadeCheck(1);
        }
        fn_801EF61C(0);
        uVar1 = fn_801EF634();
    }
    return uVar1;
}
#pragma pop

/* 0x8020DD44 | size: 0x3C | small */
void fightEncountGetEnvSndDataId(void) {
    extern void fightFloorGetStatus();
    extern u32 fightEncountDataBiosGetFightFloorDataId();
    extern void fightEncountDataBiosGetPtr();
    u32 val;
    fightEncountDataBiosGetPtr();
    val = fightEncountDataBiosGetFightFloorDataId();
    fightFloorGetStatus(0, val, 0x7, 0);
}

/* Address: 0x8020DD80 | Size: 0xd0 */
u32 fightEncountGetBgmSndDataId(u16 encountDataIndex) {
    extern u32 fightFloorGetStatus(u32, u16, u32, u16);
    extern u32 fightTrainerGetStatus(u32, u16, u32, u16);
    extern u32 fightTrainerKindDataBiosGetBgmSndId(void);
    extern u8* fightTrainerKindDataBiosGetPtr(u16);
    extern u32 fightEncountDataBiosGetBgmSndId(FightEncountData* ptr);
    extern u16 fightEncountDataBiosGetFightTrainerDataId(FightEncountData* base, u8 slot);
    extern u16 fightEncountDataBiosGetFightFloorDataId(FightEncountData* ptr);
    extern FightEncountData* fightEncountDataBiosGetPtr(u16 index);

    FightEncountData* encountData;
    u32 bgmSndId;
    u16 fightFloorDataId;
    u16 fightTrainerDataId;
    u16 trainerStatus;
    u16 i;

    encountData = fightEncountDataBiosGetPtr(encountDataIndex);
    bgmSndId = fightEncountDataBiosGetBgmSndId(encountData);
    if (bgmSndId != 0) {
        return bgmSndId;
    }
    fightFloorDataId = fightEncountDataBiosGetFightFloorDataId(fightEncountDataBiosGetPtr(encountDataIndex));
    bgmSndId = fightFloorGetStatus(0, fightFloorDataId, 6, 0);
    if (bgmSndId != 0) {
        return bgmSndId;
    }
    for (i = 0; i < 4; i++) {
        fightTrainerDataId = fightEncountDataBiosGetFightTrainerDataId(encountData, (u8)i);
        if (fightTrainerDataId != 0) {
            trainerStatus = fightTrainerGetStatus(0, fightTrainerDataId, 4, 0);
            if (trainerStatus != 0) {
                fightTrainerKindDataBiosGetPtr(trainerStatus);
                bgmSndId = fightTrainerKindDataBiosGetBgmSndId();
                if (bgmSndId != 0) {
                    return bgmSndId;
                }
            }
        }
    }
    return 1;
}

/* Address: 0x8020DE50 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightEncountDataBiosGetWipeEffectSndID(FightEncountWipeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->wipeEffectSndId;
}

/* fightEncountDataBiosGetWipeEffectTime | Size: 0x18 | Get float from ptr+4, or default if NULL */
f32 fightEncountDataBiosGetWipeEffectTime(FightEncountWipeData* ptr) {
    extern f32 lbl_8047E530;
    if (ptr == NULL) {
        return lbl_8047E530;
    }
    return ptr->wipeEffectTime;
}

/* Address: 0x8020DE80 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightEncountDataBiosGetWipeSnapshotUse(FightEncountWipeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->snapshotUse;
}

/* Address: 0x8020DE98 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightEncountDataBiosGetWipeFunction(FightEncountWipeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->wipeFunction;
}

/* fightEncountWipeDataBiosGetPtr | Size: 0x28 | Look up entry in 12-byte table */
FightEncountWipeData* fightEncountWipeDataBiosGetPtr(u32 index) {
    extern u32 lbl_80478D20;
    if (index >= lbl_80478D20) {
        return NULL;
    }
    return &fight_encount_wipe_data[index];
}

/* Address: 0x8020DED8 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightEncountDataBiosGetFightName(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->fightName;
}

/* Address: 0x8020DEF0 | Size: 0x10 | Pattern: nullcheck_setter */
void fightEncountDataBiosSetSyoukaiWzxDataId(FightEncountData* ptr, u32 val) {
    if (ptr == NULL) { return; }
    ptr->syoukaiWzxDataId = val;
}

/* Address: 0x8020DF00 | Size: 0x10 | Pattern: nullcheck_setter */
void fightEncountDataBiosSetBgmSndId(FightEncountData* ptr, u32 val) {
    if (ptr == NULL) { return; }
    ptr->bgmSndId = val;
}

/* fightEncountDataBiosSetGSInputDevice | Size: 0x40 | Write u32 to slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
void fightEncountDataBiosSetGSInputDevice(FightEncountData* base, u8 slot, u32 value) {
    FightEncountTrainerSlot* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = &base->trainerSlots[slot];
    }
    if (entry == NULL) {
        return;
    }
    entry->gsInputDevice = value;
}
#pragma pop

/* fightEncountDataBiosSetFightTrainerDataId | Size: 0x40 | Write u16 to slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
void fightEncountDataBiosSetFightTrainerDataId(FightEncountData* base, u8 slot, u16 value) {
    FightEncountTrainerSlot* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = &base->trainerSlots[slot];
    }
    if (entry == NULL) {
        return;
    }
    entry->fightTrainerDataId = value;
}
#pragma pop

/* Address: 0x8020DF90 | Size: 0x10 | Pattern: nullcheck_setter */
void fightEncountDataBiosSetFightFloorDataId(FightEncountData* ptr, u16 val) {
    if (ptr == NULL) { return; }
    ptr->fightFloorDataId = val;
}

/* Address: 0x8020DFA0 | Size: 0x10 | Pattern: nullcheck_setter */
void fightEncountDataBiosSetTrainer(FightEncountData* ptr, u8 val) {
    if (ptr == NULL) { return; }
    ptr->trainer = val;
}

/* Address: 0x8020DFB0 | Size: 0x10 | Pattern: nullcheck_setter */
void fightEncountDataBiosSetFightKind(FightEncountData* ptr, u8 val) {
    if (ptr == NULL) { return; }
    ptr->fightKind = val;
}

/* Address: 0x8020DFC0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightEncountDataBiosGetSyoukaiWzxDataId(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->syoukaiWzxDataId;
}

/* Address: 0x8020DFD8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightEncountDataBiosGetZenmetuFlag(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->zenmetuFlag;
}

/* Address: 0x8020DFF0 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightEncountDataBiosGetWipeId(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->wipeId;
}

/* Address: 0x8020E008 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightEncountDataBiosGetBgmSndId(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->bgmSndId;
}

/* fightEncountDataBiosGetGSInputDevice | Size: 0x48 | Read u32 from slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
u32 fightEncountDataBiosGetGSInputDevice(FightEncountData* base, u8 slot) {
    FightEncountTrainerSlot* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = &base->trainerSlots[slot];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry->gsInputDevice;
}
#pragma pop

/* fightEncountDataBiosGetFightTrainerDataId | Size: 0x48 | Read u16 from slot in 8-byte array at offset 0x18 */
#pragma push
#pragma peephole on
u16 fightEncountDataBiosGetFightTrainerDataId(FightEncountData* base, u8 slot) {
    FightEncountTrainerSlot* entry;
    if (base == NULL) {
        entry = NULL;
    } else if (slot >= 4) {
        entry = NULL;
    } else {
        entry = &base->trainerSlots[slot];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry->fightTrainerDataId;
}
#pragma pop

/* Address: 0x8020E0B0 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightEncountDataBiosGetFightFloorDataId(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->fightFloorDataId;
}

/* Address: 0x8020E0C8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightEncountDataBiosGetTrainer(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->trainer;
}

/* Address: 0x8020E0E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightEncountDataBiosGetFightKind(FightEncountData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->fightKind;
}

/* fightEncountDataBiosGetPtr | Size: 0x2C | Look up entry in 0x38-byte table (indirect) */
FightEncountData* fightEncountDataBiosGetPtr(u16 index) {
    extern u32* lbl_80478F50;
    extern FightEncountData* lbl_80478F54;
    if (index >= *lbl_80478F50) {
        return NULL;
    }
    return &lbl_80478F54[index];
}

/* 0x8020E124 | size: 0x80 | small */
u16 fightTypeGetFightSideFightOutPokemonMax(u16 index) {
    FightTypeData* type;
    u32 count;
    u8 trainerNum;
    u8 fightoutPokemonNum;

    count = *lbl_80478F00;
    if (index > count) {
        type = NULL;
    } else {
        type = &lbl_80478F04[index];
    }
    if (type == NULL) {
        trainerNum = 0;
    } else {
        trainerNum = type->trainerNum;
    }
    if (index > count) {
        type = NULL;
    } else {
        type = &lbl_80478F04[index];
    }
    if (type == NULL) {
        fightoutPokemonNum = 0;
    } else {
        fightoutPokemonNum = type->fightoutPokemonNum;
    }
    return (u8)trainerNum * (u8)fightoutPokemonNum;
}

/* Address: 0x8020E1A4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightTypeDataBiosGetFightoutPokemonNum(FightTypeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->fightoutPokemonNum;
}

/* Address: 0x8020E1BC | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightTypeDataBiosGetEntryPokemonNum(FightTypeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->entryPokemonNum;
}

/* Address: 0x8020E1D4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightTypeDataBiosGetTrainerNum(FightTypeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->trainerNum;
}

/* Address: 0x8020E1EC | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightTypeDataBiosGetName(FightTypeData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->name;
}

/* fightTypeDataBiosGetPtr | Size: 0x2C | Look up entry in 8-byte table (indirect) */
FightTypeData* fightTypeDataBiosGetPtr(u16 index) {
    if (index > *lbl_80478F00) {
        return NULL;
    }
    return &lbl_80478F04[index];
}

/* Address: 0x8020E230 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetHostEnemyMsgFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->hostEnemyMsgFlag;
}

/* Address: 0x8020E248 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightKindDataBiosGetName(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->name;
}

/* Address: 0x8020E260 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->pokemonStatusMenuSubbarFlag;
}

/* Address: 0x8020E278 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDarkpokemonHypermodeFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->darkpokemonHypermodeFlag;
}

/* Address: 0x8020E290 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetMonohiroiFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->monohiroiFlag;
}

/* Address: 0x8020E2A8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDorobouFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->dorobouFlag;
}

/* Address: 0x8020E2C0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetBossFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->bossFlag;
}

/* Address: 0x8020E2D8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetKeikentihueruFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->keikentihueruFlag;
}

/* Address: 0x8020E2F0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doItemSoubiTokukoutokubouupFlag;
}

/* Address: 0x8020E308 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoHizukiMiyaburiFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doHizukiMiyaburiFlag;
}

/* Address: 0x8020E320 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoHizukiAiFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doHizukiAiFlag;
}

/* Address: 0x8020E338 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoCriticalAttackFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doCriticalAttackFlag;
}

/* Address: 0x8020E350 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetGetInfectPokerusFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->getInfectPokerusFlag;
}

/* Address: 0x8020E368 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetGetFriendFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->getFriendFlag;
}

/* Address: 0x8020E380 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetGetNekoniKobanFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->getNekoniKobanFlag;
}

/* Address: 0x8020E398 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetGetOkaneFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->getOkaneFlag;
}

/* Address: 0x8020E3B0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetOkanePoolFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->okanePoolFlag;
}

/* Address: 0x8020E3C8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetGetExpFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->getExpFlag;
}

/* Address: 0x8020E3E0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDrawFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->drawFlag;
}

/* Address: 0x8020E3F8 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetNigeruFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->nigeruFlag;
}

/* Address: 0x8020E410 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetCallFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->callFlag;
}

/* Address: 0x8020E428 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetUseItemFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->useItemFlag;
}

/* Address: 0x8020E440 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoZukanTukamaetaFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doZukanTukamaetaFlag;
}

/* Address: 0x8020E458 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoZukanMitaFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doZukanMitaFlag;
}

/* Address: 0x8020E470 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetDoBadgeCheckFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->doBadgeCheckFlag;
}

/* fightKindDataBiosGetPtr | Size: 0x2C | Look up entry in 32-byte table (indirect) */
FightKindData* fightKindDataBiosGetPtr(u16 index) {
    if (index > *lbl_80478F40) {
        return NULL;
    }
    return &lbl_80478F44[index];
}

/* Address: 0x8020E4B4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightKindDataBiosGetBackSaveDataFlag(FightKindData* ptr) {
    if (ptr == NULL) { return 0; }
    return ptr->backSaveDataFlag;
}

/* 0x8020E4CC | size: 0x1C | tiny */
/* fightAbicntFitMinMax | Size: 0x1C | Clamp value to [0, 12] */
s32 fightAbicntFitMinMax(s32 value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 12) {
        value = 12;
    }
    return value;
}
