/**
 * @file fight_range_80211A00_exact_80214450.c
 * @brief Strict target-order fight island, 0x80214450 - 0x80216A58.
 *
 * Bodies are mechanically extracted from the shared pure-C candidate.
 * Compiler state and active symbol aliases are re-established per body.
 */
#include "dolphin/types.h"

typedef struct FightSeqOpU8Operand {
    u8 opcode;
    u8 operand;
} FightSeqOpU8Operand;
typedef struct { u16 v[11]; } SpecialMoveList22;
typedef struct { u16 a, b; } U16Pair;

extern f32 lbl_8047E630;
extern u8 lbl_80378724[];
extern u8 lbl_80378B30[];
extern u8 lbl_80378B5B[];
extern u8 lbl_80378A5F[];
extern u8 lbl_80378968[];
extern u8 lbl_80378A4D[];
extern u8 lbl_80378A7C[];
extern u8 lbl_80378A8E[];
extern u32 fn_80232024();
extern void fn_80234A0C();
extern u32 fightOutPokemonGetPokemonPtr();
extern u16 fn_800E0C54(void);
extern u8* fn_801440A0(u16 idx);
extern u8* itemDataBiosGetPtr(u16 idx);
extern u8   fn_80143DFC(u8* p);
extern u8   itemDataBiosGetItemEffectParam(u8* p);
extern u8*  fn_80143A94(u8 idx);
extern u8*  itemParamGetPtr(u8 idx);
extern u8   fn_801437E0(u8* p);
extern u8   itemParamGetHPUp(u8* p);
extern u8   fn_80143940(u8* p);
extern u8   itemParamGetSleepFlag(u8* p);
extern u8   fn_80143918(u8* p);
extern u8   itemParamGetPoisonFlag(u8* p);
extern u8   fn_801438F0(u8* p);
extern u8   itemParamGetBurnFlag(u8* p);
extern u8   fn_801438C8(u8* p);
extern u8   itemParamGetFreezeFlag(u8* p);
extern u8   fn_801438A0(u8* p);
extern u8   itemParamGetParalyzeFlag(u8* p);
extern u8   fn_80143878(u8* p);
extern u8   itemParamGetConfuseFlag(u8* p);
extern u8   fn_80143A44(u8* p);
extern u8   itemParamGetCriticalFlag(u8* p);
extern u8   fn_80143A28(u8* p);
extern u8   itemParamGetAttackUp(u8* p);
extern u8   fn_80143A0C(u8* p);
extern u8   itemParamGetDefenceUp(u8* p);
extern u8   fn_801439F0(u8* p);
extern u8   itemParamGetQuickUp(u8* p);
extern u8   fn_801439D4(u8* p);
extern u8   itemParamGetHitUp(u8* p);
extern u8   fn_801439B8(u8* p);
extern u8   itemParamGetSpAttackUp(u8* p);
extern u8   fn_80143990(u8* p);
extern u8   itemParamGetGuardFlag(u8* p);
extern u8* lbl_8047B610;
extern u8  lbl_8047B614;
extern u8  lbl_8047B626;
extern void fn_80207448(void* p);
extern void fightOutPokemonInitOneSelfTurn(void* p);
extern void fn_802249B8();
extern void fn_802271E0(char, char);
extern void fn_802274F0(u32, char, char, char);
extern void* fn_801F025C();
extern u8 fn_802624CC();
extern u8 lbl_80478D78[1];
extern void fn_801F37B0();
extern u32 fn_8022E1F8();
extern s32 _fightSeqWsKuroikiriSub__FPvUsPv(void*, u16, void*);
extern u8  fn_80136468();
extern u8  fn_802025B8();
extern u32 fn_80214CFC();
extern int  fn_801F000C();
extern int  fightMainWaitFrame();
extern void fn_8026246C();
extern void fightMenuCloseMsg();
extern void fn_801F2598();
extern u8 lbl_80379F58[];
extern void fn_80201600();
extern void* lbl_8047B64C;
extern u8    lbl_80478278[0x10];
extern void  fn_801DA36C(void* obj, u32 val);
extern void  fn_80209484(void* ctx, u32 param);
extern u16   fn_8020147C(void* context, u16 moveId, u8 slot, u8 updateFlag);
extern void  fn_801FE468(void* context, u8* dest);
extern void  fightMenuSubMenuLvupStatus(void* a, void* b, void* c);
extern void  fightMenuOpenLevelUpStatusMenu(void* a, u32 flag);
extern void  fn_802622E4(void);
extern u32 fn_80211A78();
extern void fn_801DA7AC();
extern u8   lbl_8047B625;
extern void fn_8020248C();
extern void* lbl_8047B62C;
extern void  fn_80211B94(void* a, void* b, u8 c);
extern u8    fn_80207AE0(void* obj, u8 v);
extern void* fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(u32 a, u8 b, u32 c, u32 d, u32 e);
extern void  fn_801254B4(void* a, u32 b, u32 c, u32 d, u32 e);
extern u32  fn_80239984();
extern u32  fn_80205B8C(u32);
extern u8 fn_80239EE8();
extern u8 fn_80239CCC();
extern u16  fn_802377E8(void*, u32);
extern u16  fn_801F1990(u32, void*, u32, u32, u32, u32);
extern u16  fn_801F1C18(u32, void*, void*, u32, u32);
extern u8   fn_801F4C14(u32, u16, u32, u16, u32);
extern u32  fn_802096E8(void*);
extern u32  fn_80077AF4(void);
extern u8    fn_8021B910();
extern u8    lbl_80279F7C[12];
extern void  fn_802653FC(void* ptr, u16 a, s32 b);
extern u8    fn_80203E0C(void* ctx);
extern void  fn_8011BBD8();
extern u32   fn_80203ADC(void* ctx, u32 param);
extern u8    fn_801F2988(u32 param1, u32 param2);
extern void  fn_801F2934(u32 param1, u32 param2, u32 param3);
extern void* fn_801F0134();
extern u8    fn_801F6E44(u32 param1, u32 param2);
extern void  fn_801F6DF0(u32 param1, u32 param2, u32 param3);
extern u32   fn_80203B5C(void* ctx, u32 param);
extern u32   fn_80205184(void* ctx);
extern u8    fn_80229934(u32 param2, u32 param1, u32 param3);
extern u16  fn_80205224();
extern void fn_80202810();
extern u8   fn_802062FC();
extern u8   lbl_80378721[];
extern u8   lbl_80379249[];
extern u32 lbl_8047B618;
extern u8   fn_80203CCC();
extern u8   fn_801F221C();
extern void fn_80120B00();
extern void statusSetStatus();
extern u32  statusGetStatus();
extern void fn_801252E0();
extern void fn_80202998();
extern u8   fn_80204A10();
extern void* fn_802037DC(void* ctx);
extern void fn_801EF8F4();
extern void fightMenuAllFightTrainerCloseStatusMenu();
extern void fightMenuAllFightOutPokemonCloseStatusMenu();
extern u8   lbl_8037889D[0x23];
extern void fn_80132A38();
extern void fn_80165668(s32, s32, s32);
extern void fn_80166A50(s32, s32, s32, s32);
extern void fn_801F22D8(u32 obj);
extern u32  fn_80262308(void);
extern u8   lbl_8047B642;
extern u32  fn_802036D4();
extern u32  fn_801F4354();
extern void fn_801FCEC4();
extern u8   fn_801F2020();
extern u32  fn_801F8A18();
extern void _threadSwitch(void);
extern u32  fn_800FA280();
extern u32  fn_80203848();
extern u16  lbl_8047B61C;
extern s32  fn_801FEF74();
extern void fn_802086B0();
extern void fn_8020F108();
extern s8   fn_801DA5C4();
extern u8   lbl_80379A22[0x14];
extern u32  fn_8020912C();
extern int  fn_802656AC();
extern s32  fn_80102620();
extern u32  fn_80011C78();
extern u8   fn_802038A4();
extern void fn_80207C24();
extern void fn_80201764();
extern s32  fn_80232110();
extern u32 fn_8022E34C();
extern u8   fn_801F453C();
extern u8   lbl_8047B628;
extern u8   lbl_80379945[];
extern u16  fn_802040E8();
extern u16  fn_80203FE4();
extern void fn_80203EDC();
extern void fn_801FAA58();
extern u16  lbl_80279FD0[8];
extern u8   lbl_80379B06[];
extern u32 fn_80232FE4();
extern void fn_8020A2B8();
extern u8   fn_80119DD0();
extern u16  fightFloorGetValidFightOutPokemonCount(u32, u8, u32, u8);
extern u8   fn_802016A4();
extern u8   lbl_8037984D[27];
extern u8   lbl_80375FDF[9];
extern void heroAddPokedoru(u32, s32);
extern u8   lbl_8037939C[9];
extern u8  fn_801DDD28();
extern void fn_801DA9E8();
extern u16 lbl_8047B60C;
extern int fn_80215008();
extern u32 fn_8022B2CC();
extern void fn_801F6EEC();
extern void fightSideInitJoutaiDataId();
extern void fn_801FBC20();
extern void fn_80208C18();
extern void fn_80205AD4();
extern void fn_80205A7C();
extern void fn_80206C94();
extern SpecialMoveList22 lbl_8027A408;
extern U16Pair lbl_8047E628;
extern U16Pair lbl_8047E62C;
extern u8  fightTrainerIsAllyFightTargetPtr(u32 ctx, u32 poke, u16 floorVal);
extern u16 fightOutPokemonGetZokuseiDataId(u32 poke, u8 idx);
extern u16 fightOutPokemonGetTokuseiDataId(u32 poke);
extern u32 fn_8010C650(u16 waza, u16* types, u16 typeCount);
extern int fn_802026E4();
extern u8 fn_80235714();
extern u32 fn_802367CC();
extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
extern u32 fn_80239984();
extern s32 fightTrainerAiAddValue();
extern u32* lbl_80478DF8;
extern int wazaGetStatus();
extern int pokemonGetStatus();
extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
extern s32 fightTrainerAiWazaValueJisin();
extern s32 fightTrainerAiWazaValueJibaku();
extern s32 fightTrainerAiWazaValueNull();

/* 0x80214450 size 0x178: fn_80214450 (owner line 5506) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6EEC fightSideInitJoutaiDataId
void fn_80214450(void) {
    extern u32 fn_801F025C();
    extern u8 fn_801F6E98();
    u32 ctxA = fn_801F025C(0x11, 0);
    u32 ctx = fn_801F025C(3, ctxA);

    if (fn_801F6E98(ctx, 0x49) == 1) {
        goto L_check49_recheck;
    }
    if (fn_801F6E98(ctx, 0x48) != 1) {
        goto L_zero;
    }

L_check49_recheck:
    if (fn_801F6E98(ctx, 0x49) != 1) {
        goto L_check48_only;
    }
    if (fn_801F6E98(ctx, 0x48) != 1) {
        goto L_check48_only;
    }
    fn_801F6EEC(ctx, 0x49);
    fn_801F6EEC(ctx, 0x48);
    lbl_80379F58[0x16002] = 3;
    lbl_80379F58[0x160a1] = 3;
    goto L_done;

L_check48_only:
    if (fn_801F6E98(ctx, 0x49) == 1) {
        fn_801F6EEC(ctx, 0x49);
        lbl_80379F58[0x16002] = 1;
        lbl_80379F58[0x160a1] = 1;
        goto L_done;
    }
    if (fn_801F6E98(ctx, 0x48) == 1) {
        fn_801F6EEC(ctx, 0x48);
        lbl_80379F58[0x16002] = 2;
        lbl_80379F58[0x160a1] = 2;
    }
    goto L_done;

L_zero:
    lbl_80379F58[0x16002] = 0;
    lbl_80379F58[0x160a1] = 0;

L_done:
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F6EEC
#undef fn_801F6E98
#undef fn_801F025C

/* 0x802145C8 size 0xEC: fn_802145C8 (owner line 13017) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
void fn_802145C8(void)

{
    extern int fn_801F025C();
    extern void fn_801F4C14();
  u32 uVar1;
  int iVar2;
  int iVar3;
  int originalTarget;
  int iVar4;
  struct {
    int value;
  } status43;

  iVar2 = fn_801F025C(0x11,0);
  iVar3 = fn_801F025C(0x12,0);
  iVar4 = fn_801F025C(0x19,0);
  originalTarget = iVar2;
  fn_801F4C14(0,0,0x47,0,iVar2);
  uVar1 = __cntlzw(iVar3 - iVar2);
  if (uVar1 >> 5 != 0) {
    status43.value = iVar4;
    iVar2 = iVar4;
  }
  if ((s32)(uVar1 >> 5) == 0) {
    status43.value = iVar4;
  }
  fn_801F4C14(0,0,0x4b,0,originalTarget);
  fn_801F4C14(0,0,0x36,0,iVar2);
  fn_801F4C14(0,0,0x43,0,status43.value);
  lbl_8047B610 += 1;
  return;
}
#undef fn_801F4C14
#undef fn_801F025C

/* 0x802146B4 size 0x10: fn_802146B4 (owner line 212) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u8* fn_802146B4(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

/* 0x802146C4 size 0xD0: fn_802146C4 (owner line 13061) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80136428 tikeiDataBiosGetZokuseiDataId
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_80207B5C fightOutPokemonSetZokuseiDataId
#define fn_800FA280 GSmsgGetGSchar
#define fn_80132A38 msgctrlSetValue
void fn_802146C4(void)

{
    extern u32 fn_800FA280();
    extern void fn_8010C4D4();
    extern void fn_80132A38();
    extern u8 fn_80136428();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_80207AE0();
    extern void fn_80207B5C();
  u16 uVar2;
  u32 uVar1;
  u8 uVar3;
  u8 cVar4;

  u32 uVar5;

  uVar2 = fn_801F54A4(0,0,0xf,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar3 = fn_80136428(uVar2);
  cVar4 = fn_80207AE0(uVar1,uVar3);
  if (cVar4 == 0) {
    for (uVar5 = 0; (uVar5 & 0xff) < 2; uVar5 = uVar5 + 1) {
      fn_80207B5C(uVar1,uVar5,uVar3);
    }
    fn_8010C4D4(uVar3);
    uVar1 = fn_800FA280();
    fn_80132A38(0xd,uVar1);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
#undef fn_80132A38
#undef fn_800FA280
#undef fn_80207B5C
#undef fn_80207AE0
#undef fn_80136428
#undef fn_801F025C
#undef fn_801F54A4

/* 0x80214794 size 0xD0: fn_80214794 (owner line 9863) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_8020147C fightOutPokemonDoItemSoubi
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_80214794(void) {
    extern u8* lbl_8047B610;
    extern void* fn_801F025C();
    extern u32 fn_802040E8();
    extern u32 fn_8012640C();
    extern u8 fn_801FECD4(void* trainer);
    extern void fn_801FE7EC(void* trainer, u32 eventId, u32 param1, u32 param2);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern u16 fn_8020147C(void* context, u16 moveId, u8 slot, u8 updateFlag);
    void* ctx;
    u32 val2;
    u16 val3;
    ctx = fn_801F025C(0x11, 0);
    val2 = fn_802040E8(ctx);
    val3 = (u16)fn_8012640C((u32)ctx, 0, 0xfa, 0);
    if (val3 != 0 && (u16)val2 == 0) {
        fn_801F4C14(0, 0, 0x56, 0, val3);
        fn_8020147C(ctx, val3, 1, 1);
        if (fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x82, 0, 0);
        }
        lbl_8047B610 += 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_8020147C
#undef fn_801F4C14
#undef fn_8012640C
#undef fn_802040E8
#undef fn_801F025C

/* 0x80214864 size 0x154: fn_80214864 (owner line 5349) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80214864(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern u8 fn_801F453C();
    u32 fieldD9;
    u8 sel = (u8)fn_801F453C(0, 1);
    u32 ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);

    if (sel == 2) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0xb);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else if (sel == 3) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 5);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else if (sel == 1) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0xa);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else if (sel == 4) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0xf);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_8011BBD8
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F453C

/* 0x802149B8 size 0xFC: fn_802149B8 (owner line 9904) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
void fn_802149B8(void) {
    extern u8* lbl_8047B610;
    extern u8  lbl_80478D78[1];
    extern void* fn_801F025C();
    extern u16 fn_80207BF4(void*);
    extern u32 fn_80205184(void*);
    extern u32 fn_8011BEB4();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    void* ctx;
    u32 val;
    u16 result;
    u8 flag;
    ctx = fn_801F025C(0x11, 0);
    fn_80207BF4(ctx);
    val = fn_80205184(ctx);
    result = (u16)fn_8011BEB4(0, val, 9, 0);
    flag = 0;
    if (result == 0xc9) {
        if ((u8)fn_802025B8(ctx, 0x38) == 2) {
            fn_8020248C(ctx, 0x38, 0);
            lbl_80478D78[5] = 0;
            flag = 1;
        }
    } else {
        if ((u8)fn_802025B8(ctx, 0x39) == 2) {
            fn_8020248C(ctx, 0x39, 0);
            lbl_80478D78[5] = 1;
            flag = 1;
        }
    }
    if (flag != 0) {
        lbl_8047B610 += 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_8011BEB4
#undef fn_80205184
#undef fn_80207BF4
#undef fn_801F025C

/* 0x80214AB4 size 0x48: fn_80214AB4 (owner line 2776) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_80214AB4(void) {
    extern void fn_801F37B0();
    extern void fn_8022EB9C();
    u8 local = 0;
    fn_801F37B0(0, fn_8022EB9C, &local, 0);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F37B0

/* 0x80214AFC size 0x5C: fn_80214AFC (owner line 498) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void fn_80214AFC(void) {
    extern u32 fn_801F025C();
    extern u32 fn_802026E4(u32, u16);
    if ((u8)fn_802026E4((u32)fn_801F025C(0x19, 0), 0x14) == 0) {
        lbl_80379F58[0x1609B] |= 0x80;
    }
    lbl_8047B610++;
}
#undef fn_801F025C

/* 0x80214B58 size 0x10: fn_80214B58 (owner line 202) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u8* fn_80214B58(void) { return lbl_8047B610++; }

/* 0x80214B68 size 0x4C: WS_HIMITUNOTIKARA (owner line 13108) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
#pragma opt_propagation off
#define fn_801F54A4 fightFloorGetStatus
#define fn_80136468 tikeiDataBiosGetFightKoukaId
void WS_HIMITUNOTIKARA(void)

{
    extern u32 fn_80136468();
    extern u32 fn_801F54A4();
    extern u8 lbl_80478D78[1];
  u16 uVar1;
  u8 uVar2;
  u32 pc;
  u8* status;
  uVar1 = fn_801F54A4(0,0,0xf,0);
  uVar2 = fn_80136468(uVar1);
  status = lbl_80478D78;
  pc = *(volatile u32*)&lbl_8047B610;
  status[3] = uVar2;
  lbl_8047B610 = (u8*)(pc + 1);
  return;
}
#undef fn_80136468
#undef fn_801F54A4

/* 0x80214BB4 size 0x50: fn_80214BB4 (owner line 2789) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802062FC fightOutPokemonCheckFightOut
void fn_80214BB4(void) {
    extern u32 fn_801F025C();
    extern u8 fn_802062FC();
    u32 ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    if (fn_802062FC(ctx) == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 2);
    } else {
        lbl_8047B610 = lbl_8047B610 + 6;
    }
}
#undef fn_802062FC
#undef fn_801F025C

/* 0x80214C04 size 0xAC: fn_80214C04 (owner line 2948) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801252E0 pokemonInitJoutai
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_80214C04(void) {
    extern u32 fn_80205B8C();
    extern void fn_801252E0();
    extern void fn_80202998();
    extern void fn_80202810();
    extern u32 fn_801F025C();
    extern u32 fn_80207BF4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    u32 tmp = fn_80205B8C(ctx);
    if ((u16)fn_80207BF4(ctx) == 0x1e) {
        fn_801252E0(tmp);
        fn_80202998(ctx, 0);
        fn_80202810(ctx, 0x17);
        if (fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x7c, 0, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80202810
#undef fn_80202998
#undef fn_801252E0
#undef fn_80207BF4
#undef fn_80205B8C
#undef fn_801F025C

/* 0x80214CB0 size 0x4C: fn_80214CB0 (owner line 2806) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_80214CB0(void) {
    extern u32 fn_801F025C();
    extern int fn_801F37B0();
    u32 ctx = fn_801F025C(0x19, 0);
    fn_801F37B0(0, fn_80214CFC, ctx, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F37B0
#undef fn_801F025C

/* 0x80214CFC size 0xB4: fn_80214CFC (owner line 13138) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
#pragma opt_propagation off
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011CB54 pokemonTokuseiDataBiosGetName
#define fn_8011CB6C pokemonTokuseiDataBiosGetPtr
#define fn_80132A38 msgctrlSetValue
#define fn_801F4C14 fightFloorSetStatus
#define fn_80202B88 fightOutPokemonIsAlly
#define fn_802062FC fightOutPokemonCheckFightOut
u32 fn_80214CFC(u32 r3, u32 r4, u32 r5)

{
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fn_800FA280();
    extern void fn_8011CB54();
    extern void fn_8011CB6C();
    extern void fn_80132A38();
    extern void fn_801F4C14();
    extern u8 fn_80202B88();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
  extern u8 lbl_8037960A[];
  extern void* lbl_8047B62C;
  u32 ctx;
  u32 other;

  ctx = r3;
  other = r5;
  {
    u32 value;

    value = fightOutPokemonGetTokuseiDataId(other);
    if (fn_802062FC(ctx) == 0) {
      return 1;
    }
    if (fn_80202B88(ctx,other) == 0) {
      fn_801F4C14(0,0,0x42,0,ctx);
      fn_8011CB6C(value);
      fn_8011CB54();
      value = fn_800FA280();
      fn_80132A38(0xd,value);
      fn_80211B94(lbl_8047B62C,lbl_8037960A,0);
    }
  }
  return 1;
}
#undef fn_802062FC
#undef fn_80202B88
#undef fn_801F4C14
#undef fn_80132A38
#undef fn_8011CB6C
#undef fn_8011CB54
#undef fn_800FA280

/* 0x80214DB0 size 0xA0: fn_80214DB0 (owner line 2610) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801254B4 pokemonSetStatus
#define fn_801F221C fightFloorIsLastActionFightOutPokemon
void fn_80214DB0(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801254B4();
    extern u8 fn_801F221C();
    u32 ctx = fn_801F025C(0x11, 0);
    fn_801254B4(ctx, 0, 0x118, 0, 1);

    if (fn_801F221C(0) != 1) {
        goto check2;
    }
deref:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
check2:
    if (fn_802025B8(ctx, 0x33) != 2) {
        goto deref;
    }
    fn_8020248C(ctx, 0x33, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F221C
#undef fn_801254B4
#undef fn_801F025C

/* 0x80214E50 size 0xC0: fn_80214E50 (owner line 2638) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_801F221C fightFloorIsLastActionFightOutPokemon
void fn_80214E50(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801254B4();
    extern void fn_801F4C14();
    extern u8 fn_801F221C();
    u32 ctx = fn_801F025C(0x11, 0);
    fn_801F4C14(0, 0, 0x43, 0, ctx);
    fn_801254B4(ctx, 0, 0x118, 0, 1);

    if (fn_801F221C(0) != 1) {
        goto check2;
    }
deref:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
check2:
    if (fn_802025B8(ctx, 0x37) != 2) {
        goto deref;
    }
    fn_8020248C(ctx, 0x37, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F221C
#undef fn_801254B4
#undef fn_801F4C14
#undef fn_801F025C

/* 0x80214F10 size 0xF8: fn_80214F10 (owner line 5462) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F4C14 fightFloorSetStatus
void fn_80214F10(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    u32 ctx11;
    u16 id;
    u32 trainerCtx;
    s32 divisor;
    u16 val;
    u32 result;
    u16 buf[0x1C];

    id = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx11 = fn_801F025C(0x11, 0);
    trainerCtx = fn_801F4354(0, ctx11);
    divisor = fn_80215008(trainerCtx, buf, 0x18, ctx11);
    if (divisor != 0) {
        val = buf[(u16)fn_800E0C54() % divisor];
        lbl_8047B618 = lbl_8047B618 & ~0x400;
        lbl_8047B60C = val;
        result = fn_8022B2CC(ctx11, val, id, 0, 1, 1, -1);
        fn_801F4C14(0, 0, 0x43, 0, result);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F4C14
#undef fn_801F4354
#undef fn_801F025C
#undef fn_801F54A4

/* 0x80215008 size 0x1B8: fn_80215008 (owner line 13185) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
int fn_80215008(u32 trainer, u16* buffer, u32 maxCount, u32 context)

{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u16);
    extern u32 heroGetStatus(u32, u32, u16);
    extern u32 fightTrainerCheckTemotiPokemonFightEntry(u32, u32);
    extern u8 fightPokemonCheckFightOut(u32);
    extern u32 fightPokemonGetPokemonPtr(u32);
    extern u8 pokemonWazaCheckValid(u32, u16);
    extern u16 lbl_80279FA0[];
    u32 excludedPokemon;
    u32 fightPokemon;
    u32 heroPokemon;
    u32 pokemon;
    u16 moveId;
    s32 moveIndex;
    u16 tableMove;
    u8 blocked;
    s32 capacity;
    s32 heroIndex;
    u32 trainerData;
    s32 clearIndex;
    s32 count;
    u32 tableOffset;

    trainerData = fightTrainerGetStatus(trainer, 0, 0x44, 0);
    excludedPokemon = pokemonGetStatus(context, 0, 0xd5, 0);
    capacity = maxCount & 0xffff;
    for (clearIndex = 0; clearIndex < capacity; clearIndex++) {
        buffer[clearIndex] = 0;
    }

    count = 0;
    heroIndex = 0;
    do {
        heroPokemon = heroGetStatus(trainerData, 3, heroIndex);
        fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(trainer, heroPokemon);
        if (fightPokemon != 0 && fightPokemonCheckFightOut(fightPokemon) != 0 &&
            excludedPokemon != fightPokemon) {
            pokemon = fightPokemonGetPokemonPtr(fightPokemon);
            moveIndex = 0;
            do {
                if (pokemonWazaCheckValid(pokemon, moveIndex) != 0) {
                    moveId = pokemonGetStatus((unsigned long)pokemon, 0, 0x7f, moveIndex);
                    if (moveId == 0 || moveId == 0x165 || moveId == 0xd6 ||
                        moveId == 0x112 || moveId == 0x77 || moveId == 0x76) {
                        blocked = 1;
                    } else {
                        blocked = 0;
                    }
                    if (blocked == 0) {
                        tableOffset = 0;
                        while ((tableMove = lbl_80279FA0[tableOffset]) != 0xffff) {
                            if (moveId == tableMove) {
                                break;
                            }
                            tableOffset++;
                        }
                        if (tableMove == 0xffff && moveId != 0 && moveId != 0x165 &&
                            count < capacity) {
                            buffer[count] = moveId;
                            count++;
                        }
                    }
                }
                moveIndex++;
            } while (moveIndex < 4);
        }
        heroIndex++;
    } while (heroIndex < 6);
    return count;
}

/* 0x802151C0 size 0xE8: fn_802151C0 (owner line 10061) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80203D3C figthOutPokemonGetPokemonDataId
#define fn_8011BBD8 wazaSetStatus
void fn_802151C0(void) {
    extern void* fn_801F025C();
    extern void* fn_8012640C();
    extern u32 fn_80203D3C(void*);
    extern void fn_8011BBD8();
    extern u16 lbl_80279F88[];
    void* ctx1 = fn_801F025C(0x11, 0);
    void* obj = fn_8012640C(ctx1, 0, 0xD9, 0);
    void* ctx2 = fn_801F025C(0x12, 0);
    u32 level = fn_80203D3C(ctx2);
    u16 result = (u16)(u32)fn_8012640C(0, level, 0x5F, 0);
    u16 i;
    u16 thr;
    u16 val;

    for (i = 0; (thr = lbl_80279F88[i]) != 0xFFFF; i += 2) {
        if (thr > result) break;
    }
    val = (thr != 0xFFFF) ? lbl_80279F88[i + 1] : 0x78;
    fn_8011BBD8(obj, 0, 0x2f, 0, val);
    lbl_8047B610 += 1;
}
#undef fn_8011BBD8
#undef fn_80203D3C
#undef fn_8012640C
#undef fn_801F025C

/* 0x802152A8 size 0x58: fightSeqGetKetaguriIryoku (owner line 9237) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u16 fightSeqGetKetaguriIryoku(u16 weight) {
    extern u16 lbl_80279F88[];
    u16 i;
    u16 thr;
    for (i = 0; (thr = lbl_80279F88[i]) != 0xFFFF; i += 2) {
        if (thr > weight) break;
    }
    if (thr != 0xFFFF) return lbl_80279F88[i + 1];
    return 0x78;
}

/* 0x80215300 size 0x74: WS_ONNEN (owner line 10091) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_ONNEN(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);

    if (fn_802025B8(ctx, 0x28) == 2) {
        fn_8020248C(ctx, 0x28, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    }
}

/* 0x80215374 size 0x1B4: fn_80215374 (owner line 13260) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
void fn_80215374(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fn_80201890();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern u32 fn_80215528();
    u32 pokemon;
    u32 stateMask;
    u32 amount;
    u32 pokemonData;
    u32 newValue;
    u8 currentValue;
    u8 slot;
    s8 moveSlot;
    s32 signedMoveSlot;
    s32 loopState[2];

    pokemon = fightTargetGetPtrAsNowFightType(0x11, 0);
    if (fn_802025B8(pokemon, 0x27) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (pokemon != 0) {
        amount = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
            0, 0x2e, 2, pokemon);
        if (amount != 0) {
            pokemonData = fightOutPokemonGetPokemonPtr(pokemon);
            moveSlot = pokemonSearchWazaDataId(pokemonData, 0x11e);
            if (moveSlot >= 0) {
                signedMoveSlot = moveSlot;
                currentValue = pokemonGetStatus(pokemonData, 0, 0x80, signedMoveSlot);
                newValue = amount < currentValue ? (u8)(currentValue - amount) : 0;
                pokemonSetStatus(pokemonData, 0, 0x80, signedMoveSlot, (u8)newValue);
                slot = moveSlot;
                if (fn_802026E4(pokemon, 0x10) == 0 &&
                    fn_802026E4(pokemon, 0x31) == 1) {
                    stateMask = fn_80201890(pokemon, 0x31);
                    if ((stateMask & (1 << slot)) == 0 &&
                        fightOutPokemonIsUseHensinBuff(pokemon) == 1) {
                        fightOutPokemonSetHensinPokemonStatusId(
                            pokemon, 0x80, slot, 0);
                    }
                }
            }
        }
    }

    loopState[0] = pokemon;
    loopState[1] = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_80215528, loopState, 0);
    if (loopState[1] == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(pokemon, 0x27, 0);
        lbl_8047B610 += 5;
    }
}

/* 0x80215528 size 0xEC: fn_80215528 (owner line 13331) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
u32 fn_80215528(u32 r3, u32 r4, u32* r5)

{
    extern s8 pokemonSearchWazaDataId();
    extern u8 pokemonWazaCheckValid();
    extern u8 fightOutPokemonIsAlly();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 pokemonGetStatus();
  u32 otherPokemon;
  u32 other;
  s32 value;
  u32 pokemon;
  s8 slot;
  s8 moveSlot;
  u16 move;

  other = *r5;
  value = r3;
  if (fightOutPokemonCheckFightOut() == 0) {
    return 1;
  }
  if (fightOutPokemonIsAlly(value,other) == 1) {
    return 1;
  }
  otherPokemon = fightOutPokemonGetPokemonPtr(other);
  pokemon = fightOutPokemonGetPokemonPtr(value);
  for (slot = 0; slot < 4; slot++) {
    value = (int)slot;
    if (pokemonWazaCheckValid(otherPokemon,value) == 0) {
      continue;
    }
    move = (int)pokemonGetStatus(otherPokemon,0,0x7f,value);
    moveSlot = pokemonSearchWazaDataId(pokemon,move);
    if (moveSlot < 0) {
      continue;
    }
    r5[1] = 1;
    return 0;
  }
  return 1;
}

/* 0x80215614 size 0x10C: fn_80215614 (owner line 13380) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_802096E8 fightWazaIsHit
#define fn_80207BC0 fightOutPokemonSetTokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
void fn_80215614(void)

{
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern u32 fn_80205184();
    extern void fn_80207BC0();
    extern u8 fn_802096E8();
    extern u8 fn_80229934();
    extern u32 fn_80207BF4();
  u32 uVar1;
  u32 uVar2;
  u32 uVar5;
  u32 uVar3;
  u32 uVar6;
  u32 uVar4;
  u8 cVar7;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80207BF4(uVar1);
  uVar4 = fn_80205184((void*)uVar1);
  uVar5 = fn_801F025C(0x12,0);
  uVar6 = fn_80207BF4();
  if (((((uVar3 & 0xffff) == 0) && ((uVar6 & 0xffff) == 0)) || ((u16)uVar3 == 0x19)) ||
     ((((uVar6 & 0xffff) == 0x19 || (cVar7 = fn_802096E8(uVar2), cVar7 == 0)) ||
      (cVar7 = fn_80229934(uVar4,uVar1,uVar5), cVar7 == 1)))) {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    return;
  }
  fn_80207BC0(uVar1,uVar6);
  fn_80207BC0(uVar5,uVar3);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
#undef fn_80207BF4
#undef fn_80207BC0
#undef fn_802096E8
#undef fn_80205184
#undef fn_8012640C
#undef fn_801F025C

/* 0x80215720 size 0xE8: fn_80215720 (owner line 13428) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
void fn_80215720(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern u32 fn_80205184();
    extern u32 fn_80205B8C();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  u32 uVar4;
  u16 uVar5;
  u16 uVar6;

  if ((lbl_8047B618 & 0x200) == 0) {
    uVar1 = fn_801F025C(0x11,0);
    uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
    fn_80205184((void*)uVar1);
    uVar3 = fn_8011BEB4(uVar2,0,0x2f,0);
    uVar4 = fn_80205B8C(uVar1);
    uVar5 = (int)fn_8012640C(uVar4,0,0x83,0);
    uVar1 = fn_80205B8C(uVar1);
    uVar6 = (int)fn_8012640C(uVar1,0,0x87,0);
    uVar3 = (u16)((s32)(uVar3 * uVar5) / (s32)uVar6);
    if (uVar3 == 0) {
      uVar3 = 1;
    }
    fn_8011BBD8(uVar2,0,0x2f,0,uVar3);
  }
  *(u32*)&lbl_8047B610 = *(u32*)&lbl_8047B610 + 1;
  return;
}
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_8011BEB4
#undef fn_80205184
#undef fn_8012640C
#undef fn_801F025C

/* 0x80215808 size 0xC8: fn_80215808 (owner line 2670) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
void fn_80215808(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 poke1 = fn_80205B8C(ctx1);
    s32 statA = (u16)fn_8012640C(poke1, 0, 0x83, 0);

    u32 ctx2 = fn_801F025C(0x12, 0);
    u32 poke2 = fn_80205B8C(ctx2);
    s32 statB = (u16)fn_8012640C(poke2, 0, 0x83, 0);

    if (statB <= statA) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, statB - statA);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_8012640C
#undef fn_801F025C

/* 0x802158D0 size 0x84: WS_AKUBI (owner line 10135) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_AKUBI(void) {
    extern u8 fightOutPokemonIsJoutaiNormal();
    u32 ctx = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fn_802025B8(ctx, 0x26) != 2) {
        goto deref;
    }
    if (fightOutPokemonIsJoutaiNormal(ctx) != 0) {
        goto plus5;
    }
deref:
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    return;
plus5:
    fn_8020248C(ctx, 0x26, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}

/* 0x80215954 size 0x124: fn_80215954 (owner line 13466) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F54A4 fightFloorGetStatus
void fn_80215954(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
  u32 uVar1;
  u32 uVar2;
  u16 uVar5;
  short sVar4;
  short sVar6;
  u16 uVar7;
  u32 uVar8;
  u16 uVar3;

  uVar3 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (u32)fn_8012640C(uVar1,0,0xd9,0);
  sVar4 = (int)fn_8012640C(uVar1,0,0x102,0);
  uVar5 = (int)fn_8012640C(uVar1,0,0x103,0);
  sVar6 = (int)fn_8012640C(uVar1,0,0x104,0);
  uVar7 = (int)fn_8012640C(uVar1,0,0x105,0);
  uVar1 = fn_801F025C(0x12,0);
  uVar8 = fn_801F0134(uVar1,uVar3);
  if (((sVar4 != 0) && (uVar5 == (u16)uVar8)) ||
      ((sVar6 != 0) && (uVar7 == (u16)uVar8))) {
    fn_8011BBD8(uVar2,0,0x2c,0,2);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F54A4
#undef fn_801F0134
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_8011BEB4
#undef fn_80205184
#undef fn_8012640C
#undef fn_801F025C

/* 0x80215A78 size 0x74: WS_NEWOHARU (owner line 10124) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_NEWOHARU(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);

    if (fn_802025B8(ctx, 0x25) != 2) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x25, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

/* 0x80215AEC size 0x184: fn_80215AEC (owner line 13507) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#pragma dont_inline on
void fn_80215AEC(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern s32 pokemonGetStatus();
    extern u8 fn_802025B8();
    extern void fn_8020248C();
    extern void fn_80201764();
    extern u32 fn_80201890();
    extern void msgctrlSetValue();
    extern u16 fightOutPokemonMaxHpWaruValue();
    extern void wazaSetStatus();
    extern u8 fightOutPokemonIsHpMantan();

    switch (lbl_8047B610[1]) {
    case 0: {
        u32 value;
        u32 ctx;
        u32 finalValue;

        ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
        value = pokemonGetStatus(ctx, 0, 0xd5, 0);
        value = pokemonGetStatus(value, 0, 0xcb, 0);
        finalValue = pokemonGetStatus(value, 0, 0x77, 0);
        if (fn_802025B8(ctx, 0x35) != 2) {
            lbl_8047B610 = *(u8 **)(lbl_8047B610 + 2);
        } else {
            fn_8020248C(ctx, 0x35, 0);
            fn_80201764(ctx, 0x35, finalValue);
            lbl_8047B610 += 6;
        }
        break;
    }
    case 1: {
        u32 pokemon;
        u32 defender;
        u32 attacker;

        attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
        pokemon = pokemonGetStatus(attacker, 0, 0xd9, 0);
        defender = fightTargetGetPtrAsNowFightType(0x12, 0);
        msgctrlSetValue(0xd, fn_80201890(defender, 0x35));
        wazaSetStatus(pokemon, 0, 0x2d, 0,
                      -(s32)(u16)fightOutPokemonMaxHpWaruValue(defender, 2));
        if (fightOutPokemonIsHpMantan(defender) == 1) {
            lbl_8047B610 = *(u8 **)(lbl_8047B610 + 2);
        } else {
            lbl_8047B610 += 6;
        }
        break;
    }
    }
}

/* 0x80215C70 size 0x80: WS_NARIKIRI (owner line 13563) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_80207BC0 fightOutPokemonSetTokuseiDataId
void WS_NARIKIRI(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_80207BF4();
    extern void fn_80207BC0();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(0x11,0);
  fn_80207BF4();
  fn_801F025C(0x12,0);
  uVar2 = fn_80207BF4();
  if (((uVar2 & 0xffff) == 0) || ((uVar2 & 0xffff) == 0x19)) {
    goto jump;
  }
  fn_80207BC0(uVar1,uVar2);
  lbl_8047B610 += 5;
  return;
jump:
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}
#undef fn_80207BC0
#undef fn_80207BF4
#undef fn_801F025C

/* 0x80215CF0 size 0x358: fn_80215CF0 (owner line 13589) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
void fn_80215CF0(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern s32 fightFloorGetStatus();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u8 fn_802026E4();
    extern u8 fn_80142984();
    extern void fightOutPokemonDoItemSoubi();
    extern void pokemonSetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 itemGetStatus();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern u8 lbl_803798D8[];
    u32 attacker;
    u32 defender;
    u32 attackerItem;
    u32 ability;
    u32 defenderItem;
    u8 proceed;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    attackerItem = fightOutPokemonGetSoubiItemDataId(attacker);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    ability = fightOutPokemonGetTokuseiDataId(defender);
    defenderItem = fightOutPokemonGetSoubiItemDataId(defender);

    if ((u8)fightFloorGetStatus(0, 0, 0x2f, 0) == 1 &&
        fightOutPokemonIsGcHeroFightOutPokemon(attacker) == 0) {
        proceed = 0;
    } else if ((u8)fightFloorGetStatus(0, 0, 0x2f, 0) == 1 &&
               fn_802026E4(attacker, 0x3d) == 1) {
        proceed = 0;
    } else {
        proceed = 1;
    }

    if (proceed == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (fightFloorGetStatus(0, 0, 0x2f, 0) == 0) {
        goto second_check_true;
    }
    if (fn_802026E4(attacker, 0x3d) != 0) {
        goto second_check_false;
    }
    if (fn_802026E4(defender, 0x3d) == 0) {
        goto second_check_true;
    }
second_check_false:
    proceed = 0;
    goto second_check_done;
second_check_true:
    proceed = 1;
second_check_done:

    if (proceed == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (!(((u16)attackerItem != 0 || (u16)defenderItem != 0) &&
          (u16)attackerItem != 0xaf && (u16)defenderItem != 0xaf &&
          ((u16)attackerItem == 0 || fn_80142984(attackerItem) != 0) &&
          ((u16)defenderItem == 0 || fn_80142984(defenderItem) != 0))) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    {
        if ((u16)ability == 0x3c) {
            lbl_8047B610 = lbl_803798D8;
            return;
        }

        if ((u16)attackerItem != 0) {
            fightOutPokemonDoItemSoubi(attacker, 0, 0, 0);
        }
        if ((u16)defenderItem != 0) {
            fightOutPokemonDoItemSoubi(defender, 0, 0, 0);
        }
        if ((u16)attackerItem != 0) {
            fightOutPokemonDoItemSoubi(defender, attackerItem, 1, 0);
        }
        if ((u16)defenderItem != 0) {
            pokemonSetStatus(attacker, 0, 0xfb, 0, (u16)defenderItem);
        }

        if (fightOutPokemonIsUseHensinBuff(attacker) == 1) {
            fightOutPokemonSetHensinPokemonStatusId(attacker, 0x82, 0, 0);
        }
        if (fightOutPokemonIsUseHensinBuff(defender) == 1) {
            fightOutPokemonSetHensinPokemonStatusId(defender, 0x82, 0, 0);
        }
        fightOutPokemonWriteJoutaiDataId(attacker, 0x36);
        fightOutPokemonWriteJoutaiDataId(defender, 0x36);

        itemGetStatus(0, defenderItem, 1, 0);
        msgctrlSetValue(0xd, GSmsgGetGSchar());
        itemGetStatus(0, attackerItem, 1, 0);
        msgctrlSetValue(0xe, GSmsgGetGSchar());

        if ((u16)attackerItem == 0) {
            goto one_item_missing;
        }
        if ((u16)defenderItem == 0) {
            goto one_item_missing;
        }
        lbl_80478D78[5] = 2;
        goto item_state_done;
one_item_missing:
        if ((u16)defenderItem == 0) {
            goto defender_item_missing;
        }
        lbl_80478D78[5] = 0;
        goto item_state_done;
defender_item_missing:
        lbl_80478D78[5] = 1;
item_state_done:
        lbl_8047B610 += 5;
        return;
    }
}

/* 0x80216048 size 0xA4: fn_80216048 (owner line 3025) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
u8 fn_80216048(u32 elem) {
    extern u32 fn_801F54A4();
    extern u8 fn_80204A10();
    extern u8 fn_802026E4();
    if ((u8)fn_801F54A4(0, 0, 0x2f, 0) == 1) {
        if (fn_80204A10(elem) == 0) {
            return 0;
        }
    }
    if ((u8)fn_801F54A4(0, 0, 0x2f, 0) == 1) {
        if (fn_802026E4(elem, 0x3d) == 1) {
            return 0;
        }
    }
    return 1;
}
#undef fn_80204A10
#undef fn_801F54A4

/* 0x802160EC size 0x104: fn_802160EC (owner line 3080) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F54A4 fightFloorGetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
void fn_802160EC(void) {
    extern u8 fn_802062FC();
    extern u8 fn_801F4C14();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2 = fn_801F025C(0xe, ctx1);

    if (ctx2 != 0 &&
        (u16)fn_801F54A4(0, 0, 0x19, 0) >= 2 &&
        fn_802062FC(ctx2) == 1 &&
        fn_802026E4(ctx1, 0x32) == 0 &&
        fn_802026E4(ctx2, 0x32) == 0) {
        fn_801F4C14(0, 0, 0x43, 0, ctx2);
        if (fn_802025B8(ctx2, 0x32) == 2) {
            fn_8020248C(ctx2, 0x32, 0);
        }
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F4C14
#undef fn_802062FC
#undef fn_801F54A4
#undef fn_801F025C

/* 0x802161F0 size 0x74: WS_CHOUHATSU (owner line 10102) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_CHOUHATSU(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fn_802025B8(ctx, 0x30) != 2) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x30, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

/* 0x80216264 size 0x8C: fn_80216264 (owner line 2921) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void fn_80216264(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    u32 ctx = fn_801F025C(0x11, 0);
    s16 val1 = (s16)fn_8012640C(ctx, 0, 0x102, 0);
    s16 val2 = (s16)fn_8012640C(ctx, 0, 0x104, 0);
    if (val1 != 0 || val2 != 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_8012640C
#undef fn_801F025C

/* 0x802162F0 size 0x74: WS_ICHAMON (owner line 10113) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_ICHAMON(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fn_802025B8(ctx, 0x1b) != 2) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x1b, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

/* 0x80216364 size 0xAC: fn_80216364 (owner line 2990) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_801252E0 pokemonInitJoutai
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_80216364(void) {
    extern u32 fn_80205B8C();
    extern void fn_801252E0();
    extern void fn_80202998();
    extern u32 fn_801F025C();
    extern u8 fn_80203C5C();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx = fn_801F025C(0x11, 0);
    u32 tmp = fn_80205B8C(ctx);
    if (fn_80203C5C(ctx) == 1) {
        fn_801252E0(tmp);
        fn_80202998(ctx, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
        if (fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x7c, 0, 0);
        }
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80202998
#undef fn_801252E0
#undef fn_80203C5C
#undef fn_80205B8C
#undef fn_801F025C

/* 0x80216410 size 0x140: fn_80216410 (owner line 13727) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801363E8 tikeiDataBiosGetWazaId
#define fn_8011BEB4 wazaGetStatus
#define fn_80209C1C fightWazaSetUseWazaStatus
#define fn_800FA280 GSmsgGetGSchar
#define fn_80132A38 msgctrlSetValue
#define fn_801F4C14 fightFloorSetStatus
void fn_80216410(void)

{
    extern u32 fn_800FA280();
    extern u16 fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801363E8();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_80209C1C();
    extern void fn_80211B94();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
    extern u8 lbl_80377EA4[];
    extern u32 lbl_80379BFF[];
  u16 uVar5;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar6;
  u32 uVar7;

  uVar5 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar6 = fn_801F54A4(0,0,0xf,0);
  uVar3 = fn_801363E8(uVar6);
  uVar4 = fn_8011BEB4(0,uVar3,9,0);
  fn_80209C1C(uVar2,uVar3);
  fn_8011BEB4(0,uVar3,1,0);
  uVar2 = fn_800FA280();
  fn_80132A38(0x28,uVar2);
  uVar7 = fn_8022B2CC(uVar1,uVar3,uVar5,0,1,1, (void*)0xffffffff);
  fn_801F4C14(0,0,0x43,0,uVar7);
  lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
  fn_80211B94(lbl_8047B62C,(u32)lbl_80377EA4,0);
      lbl_8047B610 = (u8*)lbl_80379BFF[(uVar4 & 0xffff)];
  return;
}
#undef fn_801F4C14
#undef fn_80132A38
#undef fn_800FA280
#undef fn_80209C1C
#undef fn_8011BEB4
#undef fn_801363E8
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F54A4

/* 0x80216550 size 0x64: WS_JUUDEN (owner line 13780) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma dont_inline on
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_JUUDEN(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0x24);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x24,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F025C

/* 0x802165B4 size 0x9C: fn_802165B4 (owner line 2563) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F6DF0 fightSideWriteJoutaiDataId
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80120B00 pokemonGetMezamerupower
#define fn_8011BBD8 wazaSetStatus
void fn_802165B4(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u32 fn_801F0134();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 tmp = fn_801F025C(2, ctx1);

    if (fn_801F6E44(tmp, 0x4d) == 2) {
        u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
        u32 val2 = fn_801F0134(ctx1, val);
        fn_801F6DF0(tmp, 0x4d, val2);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_8011BBD8
#undef fn_80120B00
#undef fn_80205B8C
#undef fn_8012640C
#undef fn_801F6DF0
#undef fn_801F0134
#undef fn_801F54A4
#undef fn_801F6E44
#undef fn_801F025C

/* 0x80216650 size 0x130: fn_80216650 (owner line 9675) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_802656AC fightMenuGetFightOutPokemonPtrToStatusMenuId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
void fn_80216650(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_802656AC();
    extern void fn_8011BBD8();
    extern u32 fn_80205B8C();
    extern void menuFightStatusSetHP();
    u32 ctx1;
    u32 fieldD9;
    u32 ctx2;
    u8 flagA;
    u32 result;
    u8 flagB;
    u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);

    ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    result = fn_802656AC(ctx1, val, 1);
    ctx2 = fn_801F025C(0x12, 0);
    flagA = (u8)fn_8012640C(ctx2, 0, 0xE6, 0);
    flagB = (u8)fn_8012640C(ctx2, 0, 0xE8, 0);

    if (flagA == 0 && flagB == 0 && lbl_80478D78[6] != 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        u16 poke_val = (u16)fn_8012640C(fn_80205B8C(ctx1), 0, 0x83, 0);
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, poke_val);
        menuFightStatusSetHP(result, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_802656AC
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F54A4

/* 0x80216780 size 0x84: fn_80216780 (owner line 5186) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
void fn_80216780(void) {
    extern u32 fn_801F2988();
    extern void fn_801F2934();
    extern void fn_801F4C14();
    u8 val = (u8)fn_801F2988(0, 0x52);

    if (val != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x52, 0);
        lbl_80478D78[5] = 5;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988

/* 0x80216804 size 0x70: WS_CHIISAKUNARU (owner line 10153) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_CHIISAKUNARU(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);

    if ((lbl_8047B618 & 0x2000000) != 0) {
        if (fn_802025B8(ctx, 0x23) == 2) {
            fn_8020248C(ctx, 0x23, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

/* 0x80216874 size 0xEC: WS_KIERUTAME_AFTAR (owner line 10164) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_KIERUTAME_AFTAR(void) {
    extern u8 fn_802026E4();
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    u16 moveId = fightOutPokemonGetUseWazaDataId(ctx);

    switch (moveId) {
    case 0x13:
    case 0x154:
        if (fn_802026E4(ctx, 0x1f) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x1f);
        }
        break;
    case 0x5b:
        if (fn_802026E4(ctx, 0x20) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x20);
        }
        break;
    case 0x123:
        if (fn_802026E4(ctx, 0x21) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x21);
        }
        break;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

/* 0x80216960 size 0xF8: WS_KIERUTAME (owner line 10190) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void WS_KIERUTAME(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    u16 moveId = fightOutPokemonGetUseWazaDataId(ctx);

    switch (moveId) {
    case 0x13:
    case 0x154:
        if (fn_802025B8(ctx, 0x1f) == 2) {
            fn_8020248C(ctx, 0x1f, 0);
        }
        break;
    case 0x5b:
        if (fn_802025B8(ctx, 0x20) == 2) {
            fn_8020248C(ctx, 0x20, 0);
        }
        break;
    case 0x123:
        if (fn_802025B8(ctx, 0x21) == 2) {
            fn_8020248C(ctx, 0x21, 0);
        }
        break;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
