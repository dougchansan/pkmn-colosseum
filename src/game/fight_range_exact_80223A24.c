/**
 * @file fight_range_exact_80223A24.c
 * @brief Strict target-order fight island with compiler-owned jump table.
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



/* WS_CHECK_TYPE */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
void WS_CHECK_TYPE(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_80207AE0();
  u32 uVar1;
  u8 cVar2;
  u8 typeId;
  u8* pc;
  u8* jumpTarget;
  u8* nextPc;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  pc = lbl_8047B610;
  cVar2 = fn_80207AE0(uVar1,
      (typeId = *(u8 *)(pc + 2),
       jumpTarget = (u8*)*(u32 *)(pc + 3), typeId));
  if (cVar2 == 1) {
    nextPc = jumpTarget;
  }
  else {
    nextPc = lbl_8047B610 + 7;
  }
  lbl_8047B610 = nextPc;
  return;
}
#undef fn_80207AE0
#undef fn_801F025C

/* WS_WAZAKOUKA_CHECK */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_WAZAKOUKA_CHECK(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
  u32 uVar1;
  u8 cVar2;

  u8* jumpTarget;
  u8* nextPc;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  jumpTarget = (u8*)*(u32 *)(lbl_8047B610 + 7);
  cVar2 = fn_802026E4(uVar1,*(u32 *)(lbl_8047B610 + 2) & 0xffff);
  if (*(u8 *)(lbl_8047B610 + 6) == cVar2) {
    nextPc = jumpTarget;
  }
  else {
    nextPc = lbl_8047B610 + 0xb;
  }
  lbl_8047B610 = nextPc;
  return;
}
#undef fn_801F025C

/* fn_80223AF4 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_80223AF4(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern s32 pokemonGetStatus();
    u32 ctx;
    u32 statusId;
    u8 result;
    s16 mode;
    s16 expected;
    u8 status;
    u8 *pc;

    result = 0;
    ctx = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
    mode = lbl_8047B610[2];

    switch (lbl_8047B610[3]) {
    case 1:
        statusId = 0xe6;
        break;
    case 2:
        statusId = 0xe7;
        break;
    case 3:
        statusId = 0xea;
        break;
    case 4:
        statusId = 0xe8;
        break;
    case 5:
        statusId = 0xe9;
        break;
    case 6:
        statusId = 0xeb;
        break;
    case 7:
        statusId = 0xec;
        break;
    default:
        statusId = 0;
        break;
    }

    status = (u8)pokemonGetStatus(ctx, 0, statusId, 0);
    pc = lbl_8047B610;
    expected = (u8)pc[4];
    switch ((u8)(s8)mode) {
    case 0:
        if ((u8)status == (u8)expected) {
            result = 1;
        }
        break;
    case 1:
        if ((u8)status != (u8)expected) {
            result = 1;
        }
        break;
    case 2:
        if ((u8)status > (u8)expected) {
            result = 1;
        }
        break;
    case 3:
        if ((u8)status < (u8)expected) {
            result = 1;
        }
        break;
    case 4:
        if (((u8)status & (u8)expected) != 0) {
            result = 1;
        }
        break;
    case 5:
        if (((u8)status & (u8)expected) == 0) {
            result = 1;
        }
        break;
    }

    if (result != 0) {
        lbl_8047B610 = *(u8 **)(pc + 5);
    } else {
        lbl_8047B610 += 9;
    }
}

/* WS_SIDECONDITION_CHECK */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
void WS_SIDECONDITION_CHECK(void)
{
    extern u32 fn_801F025C();
    extern u8 fn_801F6E98();
  u32 uVar1;
  u8 cVar2;
  u16 conditionId;
  u8* pc;
  u8* jumpTarget;
  u8* nextPc;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar1 = fn_801F025C(2,uVar1);
  pc = lbl_8047B610;
  cVar2 = fn_801F6E98(uVar1,
      (conditionId = *(u16 *)(pc + 2),
       jumpTarget = (u8*)*(u32 *)(pc + 4), conditionId));
  if (cVar2 == 1) {
    nextPc = jumpTarget;
  }
  else {
    nextPc = lbl_8047B610 + 8;
  }
  lbl_8047B610 = nextPc;
  return;
}
#undef fn_801F025C
#undef fn_801F6E98

/* fn_80223CE8 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F4C14 fightFloorSetStatus
void fn_80223CE8(void) {
    extern void* fn_801F025C();
    extern u16 fn_80207BF4(void*);
    extern void fn_801F4C14();
    u8* pc = lbl_8047B610;
    void* ctx = fn_801F025C(pc[1], 0);
    u16 ret = fn_80207BF4(ctx);
    u8* target;

    pc = lbl_8047B610;
    target = *(u8**)(pc + 3);
    if (ret == pc[2]) {
        fn_801F4C14(0, 0, 0x48, 0, (u32)ctx);
        lbl_8047B610 = target;
    } else {
        lbl_8047B610 = lbl_8047B610 + 7;
    }
}
#undef fn_801F025C
#undef fn_80207BF4
#undef fn_801F4C14

/* fn_80223D64 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80203CCC fightOutPokemonIsJoutaiNormal
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_802062FC fightOutPokemonCheckFightOut
void fn_80223D64(void) {
    extern u32 fn_801F025C();
    extern u8 fn_80203CCC();
    extern u8 fn_80203C5C();
    extern u8 fn_802026E4();
    extern u32 fn_802062FC();
    u8* target;
    u32 ctx;
    u16 kind;

    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    kind = *(u32*)(lbl_8047B610 + 2);
    target = *(u8**)(lbl_8047B610 + 6);

    if (kind == 1) {
        if ((u8)fn_80203CCC(ctx) == 0) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    } else if (kind == 2) {
        if ((u8)fn_80203C5C(ctx) == 1) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    } else {
        if ((u8)fn_802026E4(ctx, kind) == 1 && (u8)fn_802062FC(ctx) == 1) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    }
}
#undef fn_802062FC
#undef fn_80203C5C
#undef fn_80203CCC
#undef fn_801F025C

/* WS_CONDITION_CHECK */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80203CCC fightOutPokemonIsJoutaiNormal
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_802062FC fightOutPokemonCheckFightOut
void WS_CONDITION_CHECK(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern u8 fn_80203C5C();
    extern u8 fn_80203CCC();
    extern u32 fn_802062FC();
  u32 ctx;
  u8* target;
  u16 kind;

  ctx = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  kind = *(u32 *)(lbl_8047B610 + 2);
  target = *(u8 **)(lbl_8047B610 + 6);
  if (kind == 1) {
    if ((u8)fn_80203CCC(ctx) == 0) {
      lbl_8047B610 = target;
    }
    else {
      lbl_8047B610 += 10;
    }
  }
  else if (kind == 2) {
    if ((u8)fn_80203C5C(ctx) == 1) {
      lbl_8047B610 = target;
    }
    else {
      lbl_8047B610 += 10;
    }
  }
  else {
    if ((u8)fn_802026E4(ctx, kind) == 1 &&
        (u8)fn_802062FC(ctx) == 1) {
      lbl_8047B610 = target;
    }
    else {
      lbl_8047B610 += 10;
    }
  }
  return;
}
#undef fn_802062FC
#undef fn_80203C5C
#undef fn_80203CCC
#undef fn_801F025C

/* fn_80223F1C */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801252E0 pokemonInitJoutai
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8020912C fightOutPokemonKizetuEffect
void fn_80223F1C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx;
    u16 count;

    count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);

    fn_801F4354(0, ctx);
    fn_8020912C(ctx, 2);
    fn_8026532C(ctx, count, 0);
    fn_8020912C(ctx, 3);
    fn_801F37B0(0, fn_80232FE4, ctx, 0);
    fn_801252E0(fn_80205B8C(ctx));
    fn_80202998(ctx, 0);
    fn_80202810(ctx, 0x17);

    if ((u8)fn_801FECD4(ctx) == 1) {
        fn_801FE7EC(ctx, 0x7c, 0, 0);
    }
    if ((u8)fn_802026E4(ctx, 0x3e) == 1) {
        fn_80202810(ctx, 0x3e);
        if ((u8)fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0xc8, 0, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801252E0
#undef fn_801F025C
#undef fn_801F37B0
#undef fn_801F4354
#undef fn_801F54A4
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80202810
#undef fn_80202998
#undef fn_80205B8C
#undef fn_8020912C

/* fn_80224060 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_802086B0 fightOutPokemonFreeAllSequenceWaza
#define fn_8020F108 fightWazaWzxTypeFuncMigawari
#define fn_8020912C fightOutPokemonKizetuEffect
void fn_80224060(void) {
    extern s32 fn_801FEF74();
    extern void fn_80202810();
    extern void fn_802086B0();
    extern void fn_8020F108();
    extern u32 fn_8020912C();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    u32 ctx;
    u16 count;

    count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);

    if ((u8)fn_802026E4(ctx, 0x14) == 1 && fn_801FEF74(ctx) > 0) {
        fn_80202810(ctx, 0x14);
        fn_802086B0(ctx);
        fn_8020F108(0xa4, ctx, ctx, 0, 0);
        do {
            if ((u8)fn_801DA5C4(6) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
    fn_802086B0(ctx);
    fn_8020912C(ctx, 0);
    fn_8020912C(ctx, 1);
    fn_80265598(ctx, count, 1);
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_801FEF74
#undef fn_80202810
#undef fn_802086B0
#undef fn_8020F108
#undef fn_8020912C
