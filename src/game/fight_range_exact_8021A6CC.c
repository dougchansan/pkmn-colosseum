/**
 * @file fight_range_exact_8021A6CC.c
 * @brief Strict target-order fight island, 0x8021A6CC - 0x8021B910.
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

/* 0x8021A6CC size 0x98: fn_8021A6CC (owner line 1445) */
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
void fn_8021A6CC(void) {
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    if (fn_802025B8(ctx, 0xf) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x45);
        lbl_80478D78[5] = 1;
    } else {
        fn_8020248C(ctx, 0xf, 0);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610++;
}
#undef fn_801F4C14
#undef fn_801F025C

/* 0x8021A764 size 0xA8: fn_8021A764 (owner line 1654) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
void fn_8021A764(void) {
    extern void* fn_801F025C();
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    u32 a = (u32)fn_801F025C(0x2, ctx1);
    if (fn_801F6E44(a, 0x4c) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x45);
        lbl_80478D78[5] = 1;
    } else {
        fn_801F6DF0(a, 0x4c, 0);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610++;
}
#undef fn_801F6DF0
#undef fn_801F4C14
#undef fn_801F6E44
#undef fn_801F025C

/* 0x8021A80C size 0x6C: fn_8021A80C (owner line 1360) */
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
#define fn_802653FC fightMenuFightOutPokemonRenewStatusMenu
void fn_8021A80C(void) {
    extern int fn_801F54A4();
    extern int fn_801F025C();
    extern void fn_802653FC(void*, u16, s32);
    u16 count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    u8 op = lbl_8047B610[1];
    void* ctx = (void*)fn_801F025C(op, 0);
    if (ctx != 0) {
        fn_802653FC(ctx, count, 1);
    }
    lbl_8047B610 += 2;
}
#undef fn_802653FC
#undef fn_801F025C
#undef fn_801F54A4

/* 0x8021A878 size 0x10C: fn_8021A878 (owner line 4214) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802016A4 fightOutPokemonGetSex
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
void fn_8021A878(void) {
    extern u8 fn_802016A4();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u32 fn_801F0134();
    extern u16 fn_80207BF4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u8 statusA = fn_802016A4(ctx1);
    u16 cand = fn_801F54A4(0, 0, 0x14, 0);
    u32 ctx2;
    u32 newPoke = fn_801F0134(ctx1, cand);
    u8 statusB;
    u16 res;
    ctx2 = fn_801F025C(0x12, 0);
    statusB = fn_802016A4(ctx2);
    res = fn_80207BF4(ctx2);

    if (res == 0xc) {
        lbl_8047B610 = (u8*)&lbl_8037984D;
        return;
    }
    if (statusA == statusB) {
        goto takeJump;
    }
    if (fn_802025B8(ctx2, 0xa) != 2) {
        goto takeJump;
    }
    if (statusA == 2) {
        goto takeJump;
    }
    if (statusB != 2) {
        goto doCall;
    }
takeJump:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
doCall:
    fn_8020248C(ctx2, 0xa, newPoke);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_80207BF4
#undef fn_801F0134
#undef fn_801F54A4
#undef fn_802016A4
#undef fn_801F025C

/* 0x8021A984 size 0x194: fn_8021A984 (owner line 14563) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_8011BBD8 wazaSetStatus
void fn_8021A984(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern u8 fn_801F453C();
    extern u8 fn_802026E4();
    extern u32 fn_80203B5C();
    extern u8 fn_802062FC();
    extern u8 fn_80207AE0();
    extern u32 fn_80207BF4();
  u32 uVar1;
  u32 uVar2;
  u32 sVar3;
  u32 uVar4;
  u8 cVar6;
  u32 cVar5;

  uVar4 = 0;
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  sVar3 = fn_80207BF4(uVar1);
  cVar5 = (int)fn_801F453C(0,1);
  cVar6 = fn_802062FC(uVar1);
  if (cVar6 == 0) {
    uVar4 = 0;
    goto LAB_00217ae0;
  }
  if (cVar5 == 3) {
    cVar6 = fn_80207AE0(uVar1,5);
    if (cVar6 == 0) {
      cVar6 = fn_80207AE0(uVar1,8);
      if ((cVar6 == 0) && (cVar6 = fn_80207AE0(uVar1,4), cVar6 == 0)) {
        if ((sVar3 & 0xffff) != 8) {
          cVar6 = fn_802026E4(uVar1,0x20);
          if ((cVar6 == 0) && (cVar6 = fn_802026E4(uVar1,0x21), cVar6 == 0)) {
            uVar4 = fn_80203B5C(uVar1,0x10) & 0xffff;
            goto LAB_00217a84;
          }
        }
      }
    }
    uVar4 = 0;
  }
LAB_00217a84:
  if (cVar5 == 4) {
    cVar5 = fn_80207AE0(uVar1,0xf);
    if (cVar5 == 0) {
      cVar5 = fn_802026E4(uVar1,0x20);
      if ((cVar5 == 0) && (cVar5 = fn_802026E4(uVar1,0x21), cVar5 == 0)) {
        uVar4 = fn_80203B5C(uVar1,0x10) & 0xffff;
        goto LAB_00217ae0;
      }
    }
    uVar4 = 0;
  }
LAB_00217ae0:
  fn_8011BBD8(uVar2,0,0x2d,0,uVar4);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_8011BBD8
#undef fn_80207BF4
#undef fn_80207AE0
#undef fn_802062FC
#undef fn_80203B5C
#undef fn_801F453C
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021AB18 size 0x84: WS_WEATHER_CHANGE (owner line 14636) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
void WS_WEATHER_CHANGE(void)

{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F4C14();
    extern u8 lbl_80478D78[1];
  u8 cVar1;

  cVar1 = fn_801F2988(0,0x55);
  if (cVar1 != 2) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    lbl_80478D78[5] = 2;
  }
  else {
    fn_801F2934(0,0x55,0);
    lbl_80478D78[5] = 3;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988

/* 0x8021AB9C size 0x80: fn_8021AB9C (owner line 1400) */
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
#define fn_80203ADC fightOutPokemonNowHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_8021AB9C(void) {
    extern u32 fn_801F025C();
    extern void* fn_8012640C();
    extern u32 fn_80203ADC();
    extern void fn_8011BBD8();
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx1, 0, 0xd9, 0);
    void* ctx2 = (void*)fn_801F025C(0x12, 0);
    u16 val = (u16)fn_80203ADC(ctx2, 2);
    fn_8011BBD8(resolved, 0, 0x2d, 0, val);
    lbl_8047B610++;
}
#undef fn_8011BBD8
#undef fn_80203ADC
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021AC1C size 0x390: fn_8021AC1C (owner line 14662) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation off
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8021AC1C(void)

{
    extern u32 fn_800E0C54();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 fn_80201D84();
    extern u8 fn_802026E4();
    extern u32 figthOutPokemonGetLevel();
    extern int figthOutPokemonGetSoubiItemBuff();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_803797A0[];
  u16 canSteal;
  u32 attacker;
  u16 floorTarget;
  u32 relativeTarget;
  u32 attackerMove;
  u16 remainingPP;
  u32 attackerStatus;
  u32 attackerLevel;
  u32 itemDataId;
  u32 itemSubDataId;
  int itemBuff;
  u32 defenderPokemon;
  u8 stealBonus;
  u32 ability;
  u32 defenderLevel;
  u16 randomValue;
  u8 condition;
  register u16 threshold;
  int levelDelta;

  attacker = fightTargetGetPtrAsNowFightType(0x11,0);
  floorTarget = fightFloorGetStatus(0,0,0x14,0);
  relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker,
      floorTarget);
  attackerMove = fightOutPokemonGetUseWazaDataId(attacker);
  attackerStatus = pokemonGetStatus(attacker,0,0xd9,0);
  attackerLevel = figthOutPokemonGetLevel(attacker);
  attacker = fightTargetGetPtrAsNowFightType(0x12,0);
  itemDataId = fightOutPokemonGetSoubiItemDataId(attacker);
  itemSubDataId = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
  itemBuff = figthOutPokemonGetSoubiItemBuff(attacker);
  defenderPokemon = fightOutPokemonGetPokemonPtr(attacker);
  remainingPP = pokemonGetStatus(defenderPokemon,0,0x83,0);
  ability = fightOutPokemonGetTokuseiDataId(attacker);
  defenderLevel = figthOutPokemonGetLevel(attacker);
  fightFloorSetStatus(0,0,0x49,0,attacker);
  if ((u16)itemSubDataId == 0x27) {
    randomValue = (u16)fn_800E0C54();
    if ((int)randomValue % 100 < itemBuff) {
      pokemonSetStatus(attacker,0,0x11a,0,1);
    }
  }
  if ((u16)ability == 5) {
    fightFloorSetStatus(0,0,0x3b,0,0x40);
    lbl_8047B610 = lbl_803797A0;
    return;
  }
  condition = fn_802026E4(attacker,0x1d);
  if ((condition == 1) &&
      ((u16)relativeTarget == (u16)fn_80201D84(attacker,0x1d))) {
    if ((u8)attackerLevel >= (u8)defenderLevel) {
      canSteal = 1;
    }
    else {
      canSteal = 0;
    }
  }
  else {
    stealBonus = wazaGetStatus(0,attackerMove,6,0);
    levelDelta = (u8)attackerLevel - (u8)defenderLevel;
    threshold = levelDelta + stealBonus;
    randomValue = (u16)fn_800E0C54();
    if (((int)randomValue % 100 + 1 < (int)threshold) &&
        ((u8)attackerLevel >= (u8)defenderLevel)) {
      canSteal = 1;
    }
    else {
      canSteal = 0;
    }
  }
  if (canSteal) {
    condition = fn_802026E4(attacker,0x2c);
    if (condition != 0) {
      wazaSetStatus(attackerStatus,0,0x2d,0,remainingPP - 1);
      fightFloorSetStatus(0,0,0x3b,0,0x46);
    }
    else {
      if ((int)pokemonGetStatus(attacker,0,0x11a,0) != 0) {
        wazaSetStatus(attackerStatus,0,0x2d,0,remainingPP - 1);
        fightFloorSetStatus(0,0,0x3b,0,0x47);
        fightFloorSetStatus(0,0,0x56,0,(u16)itemDataId);
      }
      else {
        wazaSetStatus(attackerStatus,0,0x2d,0,remainingPP);
        fightFloorSetStatus(0,0,0x3b,0,0x44);
      }
    }
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    fightFloorSetStatus(0,0,0x3b,0,0x40);
    if ((u8)attackerLevel >= (u8)defenderLevel) {
      lbl_80478D78[5] = 0;
    }
    else {
      lbl_80478D78[5] = 1;
    }
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
  }
}

/* 0x8021AFAC size 0x104: fn_8021AFAC (owner line 4060) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
#define fn_801F54A4 fightFloorGetStatus
void fn_8021AFAC(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2 = fn_801F025C(2, ctx1);
    u16 count = fightFloorGetValidFightOutPokemonCount(0, 1, ctx1, 1);

    if (fn_801F6E44(ctx2, 0x49) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 0;
    } else {
        u16 x;
        fn_801F6DF0(ctx2, 0x49, 0);
        x = fn_801F54A4(0, 0, 0x19, 0);
        if (x < 2) {
            goto setThree;
        }
        if (count < 2) {
            goto setThree;
        }
        lbl_80478D78[5] = 4;
        goto doneAfac;
    setThree:
        lbl_80478D78[5] = 3;
    doneAfac:;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F54A4
#undef fn_801F6DF0
#undef fn_801F4C14
#undef fn_801F6E44
#undef fn_801F025C

/* 0x8021B0B0 size 0xF4: fn_8021B0B0 (owner line 4338) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F2A7C fightFloorGetGcHeroFightTrainerPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_8012A5B0 heroGetStatus
void fn_8021B0B0(void) {
    extern void fn_80132A38();
    extern u8 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern s32 fn_801F2A7C();
    extern s32 fn_8012A5B0();
    u32 handle;
    u32 c;
    u8 b;
    u32 a;

    if (fn_801F54A4(0, 0, 0x25, 0) != 1) {
        goto skipB0B0;
    }
    handle = fn_801F2A7C(0);
    if (handle == 0) {
        goto skipB0B0;
    }
    a = fn_801FB1C0(handle, 0, 0x49, 0);
    if (a == 0) {
        goto skipB0B0;
    }
    b = fn_801FB1C0(handle, 0, 0x48, 0);
    a = a * b;
    c = fn_801FB1C0(handle, 0, 0x44, 0);
    if (c == 0) {
        goto skipB0B0;
    }
    heroAddPokedoru(c, a);
    fn_80132A38(0x2f, a);
    fn_80132A38(0x13, fn_8012A5B0(c, 1, 0));
    fn_80211B94(lbl_8047B62C, lbl_8037939C, 0);
skipB0B0:
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_8012A5B0
#undef fn_80132A38
#undef fn_801FB1C0
#undef fn_801F2A7C
#undef fn_801F54A4

/* 0x8021B1A4 size 0x1C0: fn_8021B1A4 (owner line 14787) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes off
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8021B1A4(void)

{
    extern u16 fn_800E0C54();
    extern u32 GSmsgGetGSchar();
    extern void fn_8010C4D4();
    extern void msgctrlSetValue();
    extern u32 wazaGetStatus();
    extern u8 pokemonWazaCheckValid();
    extern u32 pokemonGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightOutPokemonSetZokuseiDataId();
    u32 attacker;
    struct {
        u32 value;
    } pokemon;
    struct {
        u32 value;
    } type;
    struct {
        u32 value;
    } slot;
    u16 move;
    u32 selected;
    struct {
        u32 count;
        s32 i;
    } state;
    u32 index;
    u32 candidates[4];

    type.value = 0;
    slot.value = 0;
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    pokemon.value = fightOutPokemonGetPokemonPtr(attacker);
    {
        u32 init_i;
        for (init_i = 0; (u8)init_i < 4; init_i++) {
            candidates[(u8)init_i] = -1;
        }
    }

    state.count = 0;
    for (state.i = state.count; (u8)state.i < 4; state.i++) {
        slot.value = (u8)state.i;
        if (pokemonWazaCheckValid(pokemon.value, slot.value) == 1) {
            move = (u16)pokemonGetStatus(pokemon.value, 0, 0x7f, slot.value);
            if (move != 0xa5 && move != 0x164) {
                type.value = wazaGetStatus(0, move, 3, 0) & 0xffff;
                if (type.value == 9) {
                    if (fightOutPokemonIsZokuseiDataId(attacker, 7) == 1) {
                        type.value = 7;
                    } else {
                        type.value = 0;
                    }
                }
                if (fightOutPokemonIsZokuseiDataId(attacker, type.value) == 0) {
                    candidates[(u8)state.count] = (u16)type.value;
                    state.count++;
                }
            }
        }
    }

    if ((u8)state.count == 0) {
failed:
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        goto done;
    }

    selected = (pokemon.value,
        candidates[(u8)((s32)(u16)fn_800E0C54() % (s32)(u8)state.count)]);
    if ((s32)selected < 0) {
        goto failed;
    }

    state.count = selected & 0xffff;
    for (index = 0; (u8)index < 2; index++) {
        fightOutPokemonSetZokuseiDataId(attacker, index, state.count);
    }
    fn_8010C4D4(state.count);
    msgctrlSetValue(0xd, GSmsgGetGSchar());
    lbl_8047B610 += 5;
done:
    return;
}

/* 0x8021B364 size 0x120: fn_8021B364 (owner line 14882) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80123CD4 pokemonWazaCheckValid
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
u32 fn_8021B364(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u8 fn_80123CD4();
    extern s32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern u8 fn_80207AE0();
  u32 uVar6;
  u32 uVar1;
  u8 cVar4;
  u16 sVar3;
  u32 uVar2;
  int bVar5;

  uVar1 = fn_80205B8C();
  for (bVar5 = 0; (u8)bVar5 < 4; bVar5 = bVar5 + 1) {
    *(u32 *)(r4 + (u32)(u8)bVar5 * 4) = 0xffffffff;
  }
  uVar6 = 0;
  for (bVar5 = 0; (u8)bVar5 < 4; bVar5 = bVar5 + 1) {
    cVar4 = fn_80123CD4(uVar1,(u8)bVar5);
    if (cVar4 == 1) {
      sVar3 = (int)fn_8012640C(uVar1,0,0x7f,(u8)bVar5);
      if ((sVar3 != 0xa5) && (sVar3 != 0x164)) {
        uVar2 = fn_8011BEB4(0,sVar3,3,0);
        uVar2 = uVar2 & 0xffff;
        if (uVar2 == 9) {
          cVar4 = fn_80207AE0(r3,7);
          if (cVar4 == 1) {
            uVar2 = 7;
          }
          else {
            uVar2 = 0;
          }
        }
        cVar4 = fn_80207AE0(r3,uVar2);
        if (cVar4 == 0) {
          *(u32 *)(r4 + (uVar6 & 0xff) * 4) = uVar2 & 0xffff;
          uVar6 = uVar6 + 1;
        }
      }
    }
  }
  return uVar6;
}
#undef fn_80207AE0
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_80123CD4
#undef fn_80205B8C

/* 0x8021B484 size 0x18C: fn_8021B484 (owner line 14934) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8021B484(void)
{
    extern u32 fn_800E0C54();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorSetStatus();
    extern u32 fn_801F87CC();
    extern u32 figthOutPokemonGetLevel();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_80378CCE[];
    u32 attacker;
    u32 defender;
    u32 trainer;
    u32 count;
    u32 attackerLevel;
    u32 defenderLevel;
    u32 random;
    u32 selected;
    s16 status;
    u32 proceed;
    u32 choices[6];

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, defender);
    count = fn_801F87CC(trainer, choices);
    if ((u16)count != 0) {
        goto have_choices;
    }
failed:
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    goto done;

have_choices:
    attackerLevel = figthOutPokemonGetLevel(attacker);
    defenderLevel = figthOutPokemonGetLevel(defender);
    if ((u8)attackerLevel < (u8)defenderLevel) {
        goto random_check;
    }
success:
    proceed = 1;
    lbl_8047B610 = lbl_80378CCE;
    goto check_proceed;

random_check:
    random = fn_800E0C54();
    if ((u16)(((s32)(((u8)attackerLevel + (u8)defenderLevel) *
                     (u8)random) >> 8) + 1) >
        (s32)((defenderLevel >> 2) & 0x3f)) {
        goto success;
    }
    proceed = 0;
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);

check_proceed:
    if ((u8)proceed == 0) {
        goto done;
    }
    random = fn_800E0C54();
    selected = choices[(u16)((s32)(u16)random % (s32)(u16)count)];
    if (selected == 0) {
        goto failed;
    }
    status = (s16)pokemonGetStatus(selected, 0, 0xce, 0);
    if (status < 0) {
        goto failed;
    }
    fightFloorSetStatus(0, 0, 0x45, 0, defender);
    pokemonSetStatus(defender, 0, 0x121, 0, status);
    pokemonSetStatus(defender, 0, 0x119, 0, 1);
done:
    return;
}

/* 0x8021B610 size 0x18: fn_8021B610 (owner line 232) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u8* fn_8021B610(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B626 = 0;
    lbl_8047B610 = pc + 1;
    return pc;
}

/* 0x8021B628 size 0xE4: fn_8021B628 (owner line 3937) */
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
#define fn_8011BBD8 wazaSetStatus
void fn_8021B628(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8 flag = *(u8*)(lbl_8047B610 + 1);
    s32 roll;
    u8 val;

    if (flag != 0) {
        fn_8011BBD8(fieldD9, 0, 0x31, 0, flag);
    } else {
        roll = fn_800E0C54();
        val = roll % 4;
        if (val < 2) {
            val = val + 2;
        } else {
            roll = fn_800E0C54();
            val = roll % 4 + 2;
        }
        fn_8011BBD8(fieldD9, 0, 0x31, 0, val);
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_8011BBD8
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021B70C size 0x54: WS_ABARERU (owner line 15009) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_ABARERU(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802025B8();
    extern u8 lbl_80478D78[1];
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0xd);
  if (cVar2 == 2) {
    lbl_80478D78[3] = 0x75;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F025C

/* 0x8021B760 size 0xD0: fn_8021B760 (owner line 3978) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8020A2B8 fightWazaBiosCopy
#define fn_801254B4 pokemonSetStatus
void fn_8021B760(void) {
    extern void fn_8020A2B8();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_801254B4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 fieldF8 = fn_8012640C(ctx1, 0, 0xF8, 0);

    if (fn_802025B8(ctx1, 0xc) == 2) {
        fn_8020248C(ctx1, 0xc, 0);
        fn_8020248C(ctx1, 0x22, 0);
    }
    fn_8020A2B8(fieldF8, fieldD9);
    fn_801254B4(ctx1, 0, 0xf5, 0, 0);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801254B4
#undef fn_8020A2B8
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021B830 size 0x40: WS_KUROIKIRI (owner line 15029) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_8021B870 _fightSeqWsKuroikiriSub__FPvUsPv
void WS_KUROIKIRI(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8021B870();

  fn_801F37B0(0,fn_8021B870,0,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_8021B870
#undef fn_801F37B0

/* 0x8021B870 size 0x48: _fightSeqWsKuroikiriSub__FPvUsPv (owner line 8952) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_802062FC fightOutPokemonCheckFightOut
s32 _fightSeqWsKuroikiriSub__FPvUsPv(void* ctx, u16 param2, void* param3) {
    extern u8 fn_802062FC();
    extern void fightOutPokemonInitAbiCntAll();
    if (!fn_802062FC(ctx)) return 0;
    fightOutPokemonInitAbiCntAll(ctx);
    return 1;
}
#undef fn_802062FC

/* 0x8021B8B8 size 0x58: fn_8021B8B8 (owner line 1301) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8021B8B8(void) {
    u8 val = *(u8*)((u8*)lbl_80379F58 + (0x1 << 16) + 0x601e);
    if (fn_8021B910(val & 0xF0, val & 0x0F, lbl_8047B610[1],
                    *(u32*)(lbl_8047B610 + 2)) == 0) {
        lbl_8047B610 += 6;
    }
}

#pragma switch_tables reset
#pragma opt_common_subs reset
#pragma opt_propagation reset
#pragma opt_lifetimes reset
#pragma dont_inline reset
#pragma scheduling reset
#pragma optimize_for_size reset
#pragma optimization_level reset
