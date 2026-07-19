/**
 * @file fight_range_exact_8021CB58.c
 * @brief Strict target-order fight island, 0x8021CB58 - 0x8021D40C.
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


/* fn_8021CB58 */
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
void fn_8021CB58(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2 = fn_801F025C(2, ctx1);
    u16 count = fightFloorGetValidFightOutPokemonCount(0, 1, ctx1, 1);

    if (fn_801F6E44(ctx2, 0x48) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 0;
    } else {
        u16 x;
        fn_801F6DF0(ctx2, 0x48, 0);
        x = fn_801F54A4(0, 0, 0x19, 0);
        if (x < 2) {
            goto setOne;
        }
        if (count < 2) {
            goto setOne;
        }
        lbl_80478D78[5] = 2;
        goto doneCb58;
    setOne:
        lbl_80478D78[5] = 1;
    doneCb58:;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F54A4
#undef fn_801F6DF0
#undef fn_801F4C14
#undef fn_801F6E44
#undef fn_801F025C

/* fn_8021CC5C */
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
void fn_8021CC5C(void) {
    extern u8 fn_801F2988(u32, u32);
    extern void fn_801F4C14();
    extern void fn_801F2934(u32, u32, u32);
    if (fn_801F2988(0, 0x54) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x54, 0);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610++;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988

/* fn_8021CCE0 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80209C1C fightWazaSetUseWazaStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_80201248 fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry
#define fn_801254B4 pokemonSetStatus
void fn_8021CCE0(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern void fn_801254B4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_80201248();
    extern void fn_80209C1C();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern u32 lbl_80379BFF[];
  u32 uVar5;
  u16 uVar4;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  int bVar6;

  u16 auStack_18 [4];

  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar5 = (u32)fn_8012640C(uVar1,0,0xf7,0) & 0xffff;
  if (uVar5 == 0) goto LAB_else;
  if (uVar5 == 0x165) goto LAB_else;
  if (uVar5 == 0xffff) goto LAB_else;
LAB_main:
  lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
  fn_80209C1C(uVar2,uVar5);
  uVar3 = fn_8022B2CC(uVar1,uVar5,uVar4,0,1,1, (void*)0xffffffff);
  fn_801F4C14(0,0,0x43,0,uVar3);
  uVar3 = fn_8011BEB4(0,uVar5,9,0);
    lbl_8047B610 = (u8*)lbl_80379BFF[(u16)uVar3];
  return;
LAB_else:
  bVar6 = fn_80201248(uVar1,auStack_18);
  if ((u8)bVar6 != 0) {
    uVar3 = (u16)fn_800E0C54();
    uVar5 = auStack_18[(u8)((int)uVar3 % (int)(u8)bVar6)];
    if (uVar5 == 0) goto LAB_217848;
    if (uVar5 != 0x165) goto LAB_main;
  }
LAB_217848:
    fn_801254B4((void*)uVar1,0,0x118,0,1);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801254B4
#undef fn_80201248
#undef fn_8011BEB4
#undef fn_801F4C14
#undef fn_80209C1C
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F54A4

/* fn_8021CE60 */
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
#define fn_801F4C14 fightFloorSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
#define fn_80201704 fightOutPokemonIsHpMantan
void fn_8021CE60(void) {
    extern void* fn_801F025C();
    extern void* fn_8012640C();
    extern void fn_801F4C14();
    extern u16 fn_80203B5C();
    extern void fn_8011BBD8();
    extern u8 fn_80201704();
    void* ctx1 = fn_801F025C(0x11, 0);
    void* ctx2;
    void* obj = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8* pc;
    u8 sel;
    u8* target;
    u16 result;

    ctx2 = fn_801F025C(0x12, 0);
    pc = lbl_8047B610;
    sel = pc[5];
    target = *(u8**)(pc + 1);

    if (sel == 0x11) {
        ctx2 = ctx1;
        fn_801F4C14(0, 0, 0x43, 0, (u32)ctx1);
    }
    result = (u16)fn_80203B5C(ctx2, 2);
    fn_8011BBD8(obj, 0, 0x2d, 0, -(s32)result);
    if ((u8)fn_80201704(ctx2) == 1) {
        lbl_8047B610 = target;
    } else {
        lbl_8047B610 = lbl_8047B610 + 6;
    }
}
#undef fn_80201704
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_801F4C14
#undef fn_8012640C
#undef fn_801F025C

/* fn_8021CF3C */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8021CF3C(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
    u32 attacker;
    u32 candidate;
    u32 target;
    u32 branch;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    branch = *(u32*)(lbl_8047B610 + 1);
    lbl_8047B648++;
    target = 0;
    while (lbl_8047B648 < lbl_8047B649) {
        candidate = fightFloorGetStatus(0, 0, 0x5d, lbl_8047B648);
        if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 &&
            attacker != candidate) {
            target = candidate;
            break;
        }
        lbl_8047B648++;
    }
    if (target != 0) {
        fightFloorSetStatus(0, 0, 0x43, 0, target);
        lbl_8047B610 = (u8*)branch;
    } else {
        lbl_8047B610 += 5;
    }
}

/* fn_8021D010 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_801254B4 pokemonSetStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_8021D010(void) {
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
    extern u32 fn_801254B4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    fn_801254B4((void*)fn_80205B8C((u32)ctx), 0, 0x83, 0, 0);
    if (fn_801FECD4(ctx) == 1) {
        fn_801FE7EC(ctx, 0x83, 0, 0);
    }
    lbl_8047B610++;
}
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_801254B4
#undef fn_80205B8C
#undef fn_801F025C

/* fn_8021D090 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_802656AC fightMenuGetFightOutPokemonPtrToStatusMenuId
#define fn_801F349C fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
#define fn_80011E68 menuFightStatusSetHP
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
void fn_8021D090(void)
{
    extern void fn_80011E68();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_801F349C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_80205B8C();
    extern u8 fn_802062FC();
    extern u32 fn_802656AC();
    extern u8 lbl_803797BB[];
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
    u16 floorStatus;
    u32 attacker;
    u32 wazaData;
    u32 statusMenu;
    u32 special;
    u32 pokemon;
    u32 candidate;
    u32 current;

  floorStatus = fn_801F54A4(0,0,0x14,0);
  attacker = fn_801F025C(0x11,0);
  wazaData = (int)fn_8012640C(attacker,0,0xd9,0);
  statusMenu = fn_802656AC(attacker,floorStatus,1);
  special = fn_801F349C(0,6,0,0,0);
  if (special == 0) {
    pokemon = fn_80205B8C(attacker);
    fn_8011BBD8(wazaData,0,0x2d,0,
                 (u16)fn_8012640C(pokemon,0,0x83,0));
    fn_80011E68(statusMenu,0);
    lbl_8047B649 = 8;
    candidate = 0;
    lbl_8047B648 = 0;
    while (lbl_8047B648 < lbl_8047B649) {
      current = fn_801F54A4(0,0,0x5d,lbl_8047B648);
      if (current != 0 && fn_802062FC(current) != 0 &&
          attacker != current) {
        candidate = current;
        break;
      }
      lbl_8047B648++;
    }
    if (candidate != 0) {
      fn_801F4C14(0,0,0x43,0,candidate);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  else {
    fn_801F4C14(0,0,0x42,0,special);
    lbl_8047B610 = lbl_803797BB;
  }
}
#undef fn_801F4C14
#undef fn_802062FC
#undef fn_80011E68
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_801F349C
#undef fn_802656AC
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F54A4

/* fn_8021D224 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_801F221C fightFloorIsLastActionFightOutPokemon
#define fn_801F4C14 fightFloorSetStatus
void fn_8021D224(void) {
    extern u16 lbl_8047E5F8[4];
    extern u32 fn_801F025C();
    extern u16 fn_800E0C54(void);
    extern u16 fn_80205184();
    extern u32 fn_8012640C();
    extern s32 fn_8011BEB4(void*, u16, u32, u32);
    extern u32 fn_801254B4();
    extern u8  fn_801F221C();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    extern void fn_801F4C14();
    u8 canRoll = 1;
    u32 ctx1 = fn_801F025C(0x11, 0);
    u16 moveId = fn_80205184(ctx1);
    u16 fieldF1;
    u8 fieldFC;
    u16 effectId;
    u16 roll;

    effectId = (u16)fn_8011BEB4(0, moveId, 9, 0);
    fieldF1 = (u16)fn_8012640C(ctx1, 0, 0xf1, 0);
    fieldFC = (u8)fn_8012640C(ctx1, 0, 0xfc, 0);

    if (fieldF1 != 0xb6 && fieldF1 != 0xc5 && fieldF1 != 0xcb) {
        fieldFC = 0;
        fn_801254B4(ctx1, 0, 0xfc, 0, 0);
    }

    if (fn_801F221C(0) == 1) {
        canRoll = 0;
    }

    roll = (u16)fn_800E0C54();
    if (lbl_8047E5F8[fieldFC] > roll && canRoll != 0) {
        if (effectId == 0x6f) {
            if (fn_802025B8(ctx1, 0x2b) == 2) {
                fn_8020248C(ctx1, 0x2b, 0);
            }
            lbl_80478D78[5] = 0;
        }
        if (effectId == 0x74) {
            if (fn_802025B8(ctx1, 0x2c) == 2) {
                fn_8020248C(ctx1, 0x2c, 0);
            }
            lbl_80478D78[5] = 1;
        }
        fieldFC++;
        if (fieldFC > 3) {
            fieldFC = 3;
        }
        fn_801254B4(ctx1, 0, 0xfc, 0, fieldFC);
    } else {
        fn_801254B4(ctx1, 0, 0xfc, 0, 0);
        lbl_80478D78[5] = 2;
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F4C14
#undef fn_801F221C
#undef fn_801254B4
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_80205184
#undef fn_801F025C
