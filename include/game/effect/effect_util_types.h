#ifndef GAME_EFFECT_EFFECT_UTIL_TYPES_H
#define GAME_EFFECT_EFFECT_UTIL_TYPES_H

/**
 * @file effect_util_types.h
 * @brief Shared types and cross-TU declarations for the functions split
 *        out of the former game/effect/effect_util.c CodeCandidate bucket
 *        (0x8013151C - 0x80137114) into game/sex.c, game/msgctrl.c,
 *        game/dbgMenu.c, game/pcbox.c, game/gamedata.c, game/gamedataBios.c,
 *        game/gamedatasaveBios.c, game/status.c, game/koukaBios.c,
 *        game/kouka.c, game/tenkouBios.c and game/tikeiBios.c.
 *
 * All functions declared here are asm-only/partially-decompiled
 * CodeCandidate units (not linked); this header exists purely so each
 * split TU can see the others' signatures without redeclaration
 * mismatches.
 */

#include "dolphin/types.h"

typedef struct EffectUtilByteEntry {
    u8 value;
    u8 pad[7];
} EffectUtilByteEntry;

extern EffectUtilByteEntry lbl_803635D8[];

extern u32 lbl_8047ADC8;

extern u32 lbl_8047ADCC;

extern u32 lbl_8047ADD0;

extern u32 lbl_8047ADE4;

extern u32 lbl_8047ADE8;

extern u32 lbl_8047ADEC;

extern u32 lbl_8047ADF0;

extern u32 lbl_8047ADF4;

extern u32 lbl_8047ADF8;

extern u32 lbl_8047ADFC;

extern u32 lbl_8047AE00;

extern u32 lbl_8047AE04;

extern u32 lbl_8047AE08;

extern u32 lbl_8047AE0C;

extern u32 lbl_8047AE20;

extern u32 lbl_8047AE24;

extern u32 lbl_8047AE28;

extern u32 lbl_8047AE2C;

extern u32 lbl_8047AE30;

extern u32 lbl_8047AE34;

extern u32 lbl_8047AE38;

extern u32 lbl_8047AE3C;

extern u32 lbl_8047AE40;

extern u32 lbl_8047AE5C;

extern u32 lbl_8047AE60;

extern u32 lbl_8047AE64;

extern u32 lbl_8047AE70;

extern u32 lbl_8047AE74;

extern u32 lbl_8047AE78;

extern u32 lbl_8047AE88;

extern u32 lbl_8047AE8C;

extern u32 lbl_8047AE98;

extern u32 lbl_8047AE9C;

extern u16 lbl_8047AEA2;

extern u8 lbl_8047AED0;

typedef struct EffectLinkedStatusRow {
    u16 links[8];
} EffectLinkedStatusRow;

typedef struct EffectTraceFxEntry {
    u8 kind;
    u8 pad_01[7];
    u32 effectId;
    u8 pad_0C[0xC];
} EffectTraceFxEntry;

typedef struct EffectTraceEntry {
    u8 kind;
    u8 pad_01;
    u16 value0;
    u16 value1;
    u8 pad_06[6];
} EffectTraceEntry;

typedef union EffectParamFirstWord {
    struct {
        u8 field_00;
        u8 field_01;
        u8 field_02;
        u8 field_03;
    } byte;
    u32 word;
} EffectParamFirstWord;

typedef struct EffectParamBlock {
    EffectParamFirstWord word0;
    u32 field_04;
    u32 field_08;
    u32 field_0C;
    u32 field_10;
    u32 field_14;
    f32 field_18;
    u32 field_1C;
    u8 field_20;
    u8 field_21;
    u8 field_22;
} EffectParamBlock;

typedef struct EffectUtilCommandObj {
    /* 0x00 */ u8 field_00;
    /* 0x01 */ u8 activeFlag;
    /* 0x02 */ u8 field_02;
    /* 0x03 */ u8 field_03;
    /* 0x04 */ f32 field_04;
    /* 0x08 */ f32 field_08;
    /* 0x0C */ f32 field_0C;
    /* 0x10 */ f32 field_10;
    /* 0x14 */ u8 pad_14[0x0C];
    /* 0x20 */ u16 commandValue;
    /* 0x22 */ u8 pad_22;
    /* 0x23 */ u8 field_23;
    /* 0x24 */ u32 colorRgba;
    /* 0x28 */ u8 pad_28[4];
    /* 0x2C */ u8* savedStream;
    /* 0x30 */ u8* stream;
    /* 0x34 */ u8 pad_34[0x0D];
    /* 0x41 */ u8 field_41;
    /* 0x42 */ u8 field_42;
    /* 0x43 */ u8 field_43;
    /* 0x44 */ u8 flags;
    /* 0x45 */ u8 pendingFlag;
    /* 0x46 */ u8 doneFlag;
    /* 0x47 */ u8 pad_47;
    /* 0x48 */ s16 waitCounter;
    /* 0x4A */ u8 alignMode;
    /* 0x4B */ u8 field_4B;
    /* 0x4C */ u8 pad_4C[0x18];
    /* 0x64 */ f32 field_64;
} EffectUtilCommandObj;

/* lbl_803635C0 (GSEffectGlobals) is only used by fn_8013151C, which was
 * merged into game/effect/gs_effect.c (where it is already declared as
 * `extern u8 lbl_803635C0[];` -- see that file). Not redeclared here to
 * avoid depending on gs_effect.h's GSEffectGlobals type in every TU. */

extern EffectTraceFxEntry lbl_80363B88[];  /* trace fx table (BSS) */

extern EffectTraceEntry lbl_80363C00[];  /* trace table (BSS) */

extern u32 lbl_80478B98;  /* effect count (SDA) */

extern u32 lbl_80478BA0;  /* trace count (SDA) */

extern u8   menuIsCheck(u32 objID);

extern void menuClose(u32 arg1);

void _msgctrlSideName__FP15FightOutPokemonUc(u32 arg1, u32 arg2);

s32 _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(void* obj, s32 offset);

extern u16  lbl_8047AEA0;

extern void fightTrainerKindDataBiosGetPtr(u16 handle);

extern void fightTrainerKindDataBiosGetPrefixName(void);

extern void gamedataAttestCreate(void*, u8, u8, u8, u8);

extern void gamedataAttestInit();

extern u32 gamedataGetStatus();

extern void gamedataSetStatus();

extern u32 statusGetStatus(u32, u32, u32, u32, u32);

extern u32 statusSetStatus(u32, u32, u32, u32, u32, u32);

extern void* windowSearchID(s32 key);

extern void* _dbgMenuGetWin__Fl(s32 offset);

extern s32 _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(void* obj, s32 offset);

extern s32 _dbgMenuGetIndex__FP14tagWINDOW_WORK(void* obj);

extern void* gamedatasaveBiosGetPtr(void* ptr);

extern u8 gamedatasaveBiosGetOptionNoVibration(void* ptr);

extern u32 gamedatasaveBiosGetFloorid(void* ptr);

extern u32 gamedatasaveBiosGetSavecount(void* ptr);

extern u8 gamedatasaveBiosGetFloorposindex(void* ptr);

extern u8 gamedatasaveBiosGetOptionAudio(void* ptr);

extern u32 gamedatasaveBiosGetSavernd(void* ptr);

extern u32 gamedatasaveBiosGetPrevfloorid(void* ptr);

extern u32 gamedataBiosGetGamedataAtttestPtr(void* ptr);

extern u8 gamedataAttestBiosGetVerId(void* ptr);

extern u8 gamedataAttestBiosGetGenId(void* ptr);

extern u8 gamedataAttestBiosGetAreaId(void* ptr);

extern u8 gamedataAttestBiosGetLangareaId(void* ptr);

extern u32 pcboxGetStatus();

extern u32 koukaLinkDataBiosGetKouka(u32 index, u32 sub);

extern u32 koukaDataBiosGetLink(u32 index);

extern u32 koukaDataBiosGetStatusKind(u32 index);

extern u32 koukaDataBiosGetVar(u32 index);

extern s32 koukaDataBiosGetValue(u32 index, u32 subIndex);

extern u32 koukaDataBiosGetStatus(u32 index);

extern s32 menuOpenCustom(u32, ...);

extern void* _msgctrlMakeDigit__FPUslUll(void* table, u32 stride, u32 count, u32 type);

extern void msgctrlSetValue(u32 id, u32 value);

extern s32 _dbgMenuSub__Fl(s32 offset);

extern u32 _dbgMenuGetMenuNum__FP14tagWINDOW_WORKPl(u32 arg0, u32* outMax);

extern u32 _dbgMenuCheckTerminate__FP14tagWINDOW_WORKl(void* obj, s32 offset);

extern s32 _dbgMenuGetMenuNo__Fl(s32 key);

extern u32 _dbgMenuGetMsgID__FP14tagWINDOW_WORKl(void* obj, s32 offset);

extern s32 _dbgMenuGetLink__Fl(s32 idx);

extern u32 dbgMenuGetRootMenu(void);

extern s32 dbgMenuGetLink__Fl(s32 idx);

extern u32 debugMenuGetNum__Fv(void);

extern void gamedataInit(void*);

extern void fn_801353C0(void*, u8, u8, u8, u8);

extern void fn_80135708(void*);

extern void gamedataAttestBiosSetLangareaId(void* ptr, u8 val);

extern void gamedataAttestBiosSetAreaId(void* ptr, u8 val);

extern void gamedataAttestBiosSetGenId(void* ptr, u8 val);

extern void gamedataAttestBiosSetVerId(void* ptr, u8 val);

extern void gamedataBiosSetGamedataAtttestPtr(u32* dst, u32* src);

extern void gamedatasaveBiosSetOptionAudio(void* ptr, u8 val);

extern void gamedatasaveBiosSetOptionNoVibration(void* ptr, u8 val);

extern void gamedatasaveBiosSetFloorposindex(void* ptr, u8 val);

extern void gamedatasaveBiosSetPrevfloorid(void* ptr, u32 val);

extern void gamedatasaveBiosSetPlaytime(void* ptr, f32 val);

extern void gamedatasaveBiosSetFloorid(void* ptr, u32 val);

extern void gamedatasaveBiosSetSavecount(void* ptr, u32 val);

extern void gamedatasaveBiosSetSavernd(void* ptr, u32 val);

extern f32 gamedatasaveBiosGetPlaytime(void* ptr);

extern void gamedatasaveBiosSetPtr(void* dst, void* src);

extern void gamedatasaveInit(void*);

extern void _koukaOneExec__FUlPvPvPl(u32 index, void* arg1, void* arg2, s32* out);

typedef s32 (*EffectUtilCountFunc)(void);

typedef s32 (*EffectUtilEntryCallback)(s32, s32);

typedef struct EffectUtilEntry {
    u8 flags;
    u8 pad_01;
    s16 link;
    u32 value;
    EffectUtilEntryCallback callback;
} EffectUtilEntry;

typedef EffectUtilEntry* (*EffectUtilEntryFunc)(s32);

typedef struct EffectStatusTableEntry {
    u8 statusKind;
    u8 mode;
    u16 statusSub;
    u16 linkedIndex;
    s16 amount;
    s16 divisor;
} EffectStatusTableEntry;

extern u8  lbl_80426FF0[];

extern u32 lbl_8047AE94;

extern u8  lbl_80427010[];

extern u32 lbl_8047AE68;

extern u16  lbl_8047AE90;

extern void pokemonDataBiosGetPtr(u16 handle);

extern void pokemonDataBiosGetName(void);

extern u32 lbl_8047AE84;

extern u8 lbl_80427030[];

extern u32 lbl_8047AEA8;

extern u8 lbl_80427050[];

extern u8 lbl_80427070[];

extern u32 lbl_8047AE80;

extern u8 lbl_80427090[];

extern u32 lbl_8047AE6C;

extern u8 lbl_804270B0[];

extern u8 lbl_804270D0[];

extern u8 lbl_804270F0[];

extern u8 lbl_80427110[];

extern u8 lbl_80427130[];

extern u32 lbl_8047AE4C;

extern u32 lbl_8047AE48;

extern u32 lbl_8047AE44;

extern u32 lbl_8047AE1C;

extern u32 lbl_8047AE18;

extern u32 lbl_8047AE14;

extern void fightFloorGetFightOutPokemonPtrToFightTrainerPtr(void);

extern void fn_801F18DC(void);

extern void fightTrainerGetNamePtr(void);

extern void fightOutPokemonGetNicknamePtr(void);

extern void GSmsgGetGSchar(void);

extern u32 lbl_8047AE10;

extern u32 lbl_8047ADE0;

extern u32 lbl_8047ADDC;

extern u32 lbl_8047ADD8;

extern u32 lbl_8047ADD4;

extern u16  lbl_8047AE7C;

extern void wazaDataBiosGetPtr(u16 handle);

extern void wazaDataBiosGetName(void);

extern u8 lbl_80427150[];

extern u8 lbl_80427170[];

extern u8  lbl_80427190[];

extern u32 lbl_8047AE58;

extern u8  lbl_804271B0[];

extern u32 lbl_8047AE54;

extern u16  lbl_8047AE52;

extern void itemDataBiosGetPtr(u16 handle);

extern u32  itemDataBiosGetName(void);

extern u16 lbl_8047AE50;

extern u32  savedataGetStatus(u32 side, u32 slotType);

extern u32  heroBiosGetHizukiNamePtr(void);

extern u32 heroBiosGetNamePtr(void);

extern void jumptable_80363630();

extern u16 lbl_8047AEA4;

extern u32 _toolentryAlloc__FUl(u32 size);

extern u32 fn_800E27B0(u32 handle);

extern u32 lbl_8047AEB4;

extern u32 lbl_8047AEC0;

extern u32 lbl_8047AECC;

extern u32 lbl_8047AEC8;

extern u16 lbl_8047AEB8;

extern u32 lbl_8047AEB0;

extern u16 lbl_8047AEC4;

extern u32 lbl_8047AEBC;

extern void fn_800D37D4(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);

extern void fn_800D88DC(u32);

extern void fn_800D888C(u32);

extern void fn_800D9B58(f32, f32, f32, f32);

extern void fn_800DA4C4(u32, u32, u32);

extern void fn_800DA2BC(u32, u32, u32);

extern void fn_800DA1E8(u32, u32, u32);

extern void fn_800DA028(u32);

extern void fn_800D6A00(u32);

extern void fn_800D7820(u32);

extern void fn_800D67BC(u32);

extern void fn_800D6680(f32, f32, f32);

extern void fn_800D5CB8(u32, u32, u32, u32, u32);

extern void fn_800D6728(void);

extern f32 lbl_8047D0F0;

extern f32 lbl_8047D0F4;

extern f32 lbl_8047D0F8;

extern u8 lbl_80478AC0[];

extern f32 lbl_8047D0FC;

extern u8 lbl_80478AC4[];

extern f32 lbl_8047D100;

extern f32 lbl_8047D104;

extern u32  fn_800E1544(void);

extern void GSlogWrite(const char* fmt, ...);

extern const char lbl_80272AB8[];

extern u8 fn_800E0E14(u32 a, u32 b);

extern const char lbl_80272AE0[];

extern const char lbl_80272AF0[];

extern void fn_800D3074(u32 mode);

extern void GSresGetResource(void);

extern u32 lbl_80478820;

extern u8   lbl_8047AED9;

extern void GSmodelSetShadowDebug(u32 val);

extern u8   lbl_8047AED8;

extern void fn_800D4610(u32 val);

extern u32  lbl_8047AED4;

extern void* windowGetKeyInfo(void);

extern u32 menuDataBiosGetType(u32 arg);

extern u32  lbl_80478848;

extern u8   lbl_8047AED1;

extern u32 lbl_80478F88;

extern u32 lbl_8047AEDC;

extern u32 lbl_80478F8C;

extern u32 GSmsgGetRect(u32 val);

extern void fn_800057A8(void);

extern void fn_80140A9C(void);

extern u8 pokemonCheckValid(u8* ptr);

extern void GScharCpy(void);

extern void pokemonInit(void);

extern void pokemonAllKaihuku(void);

extern void fn_800F96E4(void);

extern void pokemonInitAry(void);

extern void fn_80142A88(void);

extern f64 lbl_8047D108;

extern void jumptable_80363A9C();

extern f32 lbl_8047D110;

extern u32 heroSetStatus();

extern u32 fn_80142B24();

extern u32 pokemonSetStatus();

extern u32 wazaSetStatus();

extern u32 fightFloorSetStatus();

extern u32 fightSideSetStatus();

extern u32 fightTrainerSetStatus();

extern void jumptable_80363AC8();

extern u32 itemGetStatus();

extern u32 heroGetStatus();

extern u32 pokemonGetStatus();

extern u32 wazaGetStatus();

extern u32 fightFloorGetStatus();

extern u32 fightSideGetStatus();

extern u32 fightTrainerGetStatus();

extern void jumptable_80363AF0();

extern u32 lbl_80478B90;

extern EffectLinkedStatusRow lbl_80363B78[];

extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(void* arg, u16 handle);

extern void fn_800D37CC(void);

extern void fn_80137780(void);

extern void GSvecCopy(void);

extern void fn_800E2C04(void);

extern void GStextureLoad(void);

extern void fn_800EFD14(void);

extern void fn_8013757C(void);

extern void GStextureSetWrap(void);

extern void wazaSequenceSysGetResID(void);

extern void fn_8010147C(void);

extern void fn_801013A0(void);

extern void GSmodelSetVisibility(void);

extern void fn_8013735C(void);

extern void fn_8013D604(void);

extern void fn_80137114(void);

extern void* memset(void* dst, int val, u32 n);

extern u32 jumptable_80363C70[];

extern void* memcpy(void* dst, const void* src, u32 n);

extern f64 lbl_8047D128;

extern f32 lbl_8047D118;

extern u8 lbl_80314AE8[];

extern u8 lbl_80314638[];

extern f32 lbl_8047D11C;

extern f32 lbl_8047D120;

extern s32 fn_801666BC(u16);

extern void fn_80165A20(u16, u32, u32);

extern void GSmsgAdjustAlign(void);

extern void GSmsgSetColor(void);

extern void GSmsgInitRuby(void);

extern f64 lbl_8047D0E0;

extern void fn_801E1810(void);

extern void _threadSwitch(void);

extern u8 fn_801E1874(void);

extern s32 menuOpen(u32, u32);

extern void sprintf(u8*, u8*, ...);

extern void fn_801E189C(u8*, u32);

extern u8 lbl_80272AA8[];

extern u8 lbl_8047D0E8[4];

extern void mailChkReceiveMail(void);

extern BOOL mailAddMailbox(s32 mailId);

extern void mailMainReceiveStart(void);

extern void heroMoveIsMember(void);

extern void heroMoveDismissMember(void);

extern void fn_8012F1FC(void);

#endif /* GAME_EFFECT_EFFECT_UTIL_TYPES_H */
