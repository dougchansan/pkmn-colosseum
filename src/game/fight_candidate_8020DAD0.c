/**
 * @file fight_candidate_8020DAD0.c
 * @brief fn_8020DAD0 carve (fight encount start), 0x8020DAD0 - 0x8020DD44,
 *        with its literal pool (.sdata2 0x8047E528 - 0x8047E530), linked.
 *
 * Standalone copy of the fight.c body. Its two fade literals (1.0f, 0.5f)
 * are the only users of 0x8047E528/0x8047E52C, so the carve owns them;
 * fadeSet takes (f32, u32) as in pokemon_evolution.h, so 0.5f stays single.
 */
#include "dolphin/types.h"

typedef struct FightKindData FightKindData;

u32 fightStartEncount(u32 p1) {
    extern void _threadSwitch();
    extern u32 std092_getCurrentRoomID();
    extern void std098_mapWarpToMenuRoom();
    extern void floorStartSoundScript();
    extern void floorSetFadeScript();
    extern void floorSetPrevFloorID();
    extern void floorReturnToSavedRoom();
    extern void std162_fieldClearPending();
    extern void heroDecPokedoru();
    extern u32 heroGetStatus();
    extern void msgctrlSetValue();
    extern void scriptSoundStop();
    extern void std104_soundSetBackgroundMusic();
    extern void soundStartRaw();
    extern void fn_8018DA88();
    extern u8 std081_flagCheckTrue();
    extern void std079_flagSetTrue();
    extern void std078_flagSetFalse();
    extern void fadeCheck();
    extern void fadeSetEX(f32, f32, u32, u32, u32);
    extern void fadeSet(f32, u32);
    extern void fn_801D0AFC();
    extern void mailMainReceiveTerminate();
    extern void fightSetEncountId();
    extern void battleSetResult();
    extern u32 std103_battleGetResult();
    extern void fightResetSoundFlags();
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
    extern u32 fightEncountDataBiosGetWipeId();
    extern u16 fightEncountDataBiosGetFightTrainerDataId();
    extern u32 fightEncountDataBiosGetFightFloorDataId();
    extern u8 fightEncountDataBiosGetFightKind();
    extern u32 fightEncountDataBiosGetPtr();
    extern u8 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(FightKindData* ptr);
    extern FightKindData* fightKindDataBiosGetPtr(u16 index);
    extern u16 charNameBiosSearchIndex();
    extern u16 charNameBiosGetHearFlag();

    u32 uVar1;
    u32 wipeData;
    u32 msgValue;
    u32 uVar2;
    u16 uVar7;
    u8 uVar9;
    FightKindData* iVar3;
    u32 trainerData;
    u8 cVar10;
    u16 sVar8;
    u32 uVar4;
    u32 uVar6;

    if ((p1 & 0xffff) == 0) {
        uVar1 = 0;
    } else {
        uVar1 = fightEncountDataBiosGetPtr();
        battleSetResult(0);
        std079_flagSetTrue(0x9b0);
        fightSetEncountId(p1);
        uVar2 = std092_getCurrentRoomID();
        fightFloorSetStatus(0, 0, 0x4a, 0, uVar2);
        uVar4 = fightEncountDataBiosGetFightFloorDataId(uVar1);
        uVar7 = fightFloorGetStatus(0, uVar4, 2, 0);
        mailMainReceiveTerminate();
        uVar9 = fightEncountDataBiosGetFightKind(uVar1);
        iVar3 = fightKindDataBiosGetPtr(uVar9);
        if ((iVar3 != 0) && (cVar10 = fightKindDataBiosGetPokemonStatusMenuSubbarFlag(iVar3), cVar10 != 0)) {
            fightEncountDataBiosGetFightTrainerDataId(uVar1, 1);
            trainerData = fightTrainerDataBiosGetPtr();
            if (trainerData != 0) {
                msgValue = fn_801FCC7C();
                sVar8 = charNameBiosSearchIndex();
                if ((sVar8 != 0) && (sVar8 = charNameBiosGetHearFlag(), sVar8 != 0)) {
                    std078_flagSetFalse(sVar8);
                }
                msgctrlSetValue(0x59, msgValue);
            }
        }
        std104_soundSetBackgroundMusic(1, 1000, 0xff);
        scriptSoundStop(1000);
        wipeData = fightEncountWipeDataBiosGetPtr(fightEncountDataBiosGetWipeId(uVar1));
        fadeSetEX(1.0f, fightEncountDataBiosGetWipeEffectTime(wipeData), 9,
                  fightEncountDataBiosGetWipeFunction(wipeData),
                  fightEncountDataBiosGetWipeSnapshotUse(wipeData));
        sVar8 = fightEncountDataBiosGetWipeEffectSndID(wipeData);
        if (sVar8 != 0) {
            soundStartRaw(sVar8, 0, 0);
        }
        fightResetSoundFlags();
        std098_mapWarpToMenuRoom(uVar7);
        floorSetFadeScript(0, 0);
        _threadSwitch();
        floorSetPrevFloorID(uVar7);
        cVar10 = fightEncountDataBiosGetZenmetuFlag(uVar1);
        if (cVar10 != 0) {
            uVar1 = std103_battleGetResult();
            cVar10 = fightFloorIsGcHeroWin(0, uVar1);
            if (cVar10 == 0) {
                fightSetEncountId(0);
                std079_flagSetTrue(0xe05);
                uVar6 = heroGetStatus(0, 0xc, 0);
                heroDecPokedoru(0, (s32)uVar6 / 2);
                fn_801D0AFC(1);
                fn_8018DA88();
                floorReturnToSavedRoom();
                floorSetFadeScript(0, 0x5960008);
                _threadSwitch();
                uVar1 = std103_battleGetResult();
                return uVar1;
            }
        }
        std078_flagSetFalse(0x9b0);
        floorStartSoundScript();
        std162_fieldClearPending();
        cVar10 = std081_flagCheckTrue(0xe05);
        if (cVar10 == 0) {
            fadeSet(0.5f, 2);
            fadeCheck(1);
        }
        fightSetEncountId(0);
        uVar1 = std103_battleGetResult();
    }
    return uVar1;
}
