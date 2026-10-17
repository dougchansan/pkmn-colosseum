/** Exact menuFight suffix, 0x80010C98 - 0x80011288. */
#include "dolphin/types.h"

extern u8 fightFloorCheckFightActionFightOutPokemonIrekaeSelect(s32, void*, void*);
extern void* fightOutPokemonGetNicknamePtr(void*);
extern void winMsgOpen(s32, s32, s32, s32);
extern void winMsgClose(s32);
extern u8 fn_801F18DC(s32);
extern u8 fightFloorIsUseFightTimerCommand(s32);
extern u8 fightTimerCommandIsOver(void);
extern u16 std103_battleGetResult(void);
extern void _threadSwitch(void);
extern u32 GSinputButtonsChanged(s32);
extern u32 fn_800F7BC4(s32);
extern void msgctrlSetValue();
extern void fightFloorSetStatus();
extern u32 fightFloorGetGcHeroFightTrainerPtr(s32);
extern u32 fightTrainerGetValidFightPokemonPtr(u32, u16);
extern u32 fightOutPokemonGetTokuseiDataId(void*);
extern u8 fightTrainerCheckCanIrekaeFightPokemon(u32, u32);
extern s32 fightPokemonGetNicknamePtr(u32);
extern void GSlogWrite(const char*, ...);
extern u8 lbl_80266788[];
extern u8 lbl_802E4B78[];
extern u32 fn_80089F78(u32, u32, u32, u32);
extern u32 menuIsCheck();
extern void menuCloseCustom();

u32 menuPokemonCheckPokemonChange(void* npc, u32 warpId, u32 variant)
{
#define WAIT_FOR_DIALOG(waitLabel, checkLabel, haveLabel, doneLabel) \
    goto checkLabel; \
waitLabel: \
    advance = fn_801F18DC(0); \
    if (advance != 0) { \
        if ((fightFloorIsUseFightTimerCommand(0) == 1) && (fightTimerCommandIsOver() == 1)) { \
            advance = 1; \
            goto haveLabel; \
        } else if (std103_battleGetResult() == 1) { \
            advance = 1; \
            goto haveLabel; \
        } \
    } \
    advance = 0; \
haveLabel: \
    if (advance != 0) { \
        goto doneLabel; \
    } \
    _threadSwitch(); \
checkLabel: \
    inputFlags = GSinputButtonsChanged(1); \
    maskedFlags = fn_800F7BC4(1); \
    maskedFlags &= inputFlags; \
    if ((maskedFlags & 0x300) == 0) { \
        goto waitLabel; \
    } \
doneLabel:

    void* linkedNpc;
    u32 inputFlags;
    u32 maskedFlags;
    u8 relation;
    u8 advance;
    u8 kind;

    relation = fightFloorCheckFightActionFightOutPokemonIrekaeSelect(0, npc, &linkedNpc);
    if (relation == 1) {
        msgctrlSetValue(0xD, (s32)fightOutPokemonGetNicknamePtr(npc));
        winMsgOpen(1, 0x76FB, 1, 0);
        WAIT_FOR_DIALOG(waitRelationOne, checkRelationOne, haveRelationOne, doneRelationOne);
        winMsgClose(1);
        return 0;
    }
    if (relation == 2) {
        fightFloorSetStatus(0, 0, 0x57, 0, (u16)fightOutPokemonGetTokuseiDataId(linkedNpc));
        msgctrlSetValue(0xD, (s32)fightOutPokemonGetNicknamePtr(linkedNpc));
        msgctrlSetValue(0xE, (s32)fightOutPokemonGetNicknamePtr(npc));
        winMsgOpen(1, 0x761F, 1, 0);
        WAIT_FOR_DIALOG(waitRelationTwo, checkRelationTwo, haveRelationTwo, doneRelationTwo);
        winMsgClose(1);
        return 0;
    }
    if (warpId == 0) {
        warpId = fightFloorGetGcHeroFightTrainerPtr(0);
    }
    if (warpId == 0) {
        return 0;
    }
    variant = fightTrainerGetValidFightPokemonPtr(warpId, (u16)variant);
    if (variant == 0) {
        return 0;
    }
    kind = fightTrainerCheckCanIrekaeFightPokemon(warpId, variant);
    if (kind == 1) {
        msgctrlSetValue(0xD, fightPokemonGetNicknamePtr(variant));
        winMsgOpen(1, 0x76FE, 1, 0);
        WAIT_FOR_DIALOG(waitKindOne, checkKindOne, haveKindOne, doneKindOne);
        winMsgClose(1);
        return 0;
    }
    if (kind == 2) {
        msgctrlSetValue(0xD, fightPokemonGetNicknamePtr(variant));
        winMsgOpen(1, 0x76FC, 1, 0);
        WAIT_FOR_DIALOG(waitKindTwo, checkKindTwo, haveKindTwo, doneKindTwo);
        winMsgClose(1);
        return 0;
    }
    if (kind == 3) {
        msgctrlSetValue(0xD, fightPokemonGetNicknamePtr(variant));
        winMsgOpen(1, 0x76FD, 1, 0);
        WAIT_FOR_DIALOG(waitKindThree, checkKindThree, haveKindThree, doneKindThree);
        winMsgClose(1);
        return 0;
    }
    if (kind == 0) {
        return 1;
    }
    GSlogWrite((const char*)lbl_80266788, (const char*)lbl_802E4B78);
#undef WAIT_FOR_DIALOG
    return 0;
}

void menuFightOpenGBAMain(u32 a, u32 b, u32 c)
{
    fn_80089F78(a, b, c, 0);
}

void menuFightOpenGBAIrekae(u32 a, u32 b, u32 c)
{
    fn_80089F78(a, b, c, 1);
}

u32 menuFightCloseTarget(void* obj)
{
    if ((u8)menuIsCheck(0xff) != 0) {
        menuCloseCustom(0xff, 0, obj);
    }
    if ((u8)menuIsCheck(0x104) != 0) {
        menuCloseCustom(0x104, 0, obj);
    }
    menuIsCheck(0x100);
    return 0;
}
