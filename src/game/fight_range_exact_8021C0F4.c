/**
 * @file fight_range_exact_8021C0F4.c
 * @brief Strict target-order fight island, 0x8021C0F4 - 0x8021CA00.
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

/* 0x8021C0F4 size 0x9C: fn_8021C0F4 (owner line 1378) */
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
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_8021C0F4(void) {
    extern u32 fn_801F025C();
    extern void* fn_8012640C();
    extern s32 fn_8011BEB4();
    extern void fn_8011BBD8();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx, 0, 0xd9, 0);
    s32 v2;
    s32 neg;

    fn_8011BEB4(resolved, 0, 0x2d, 0);
    v2 = fn_8011BEB4(resolved, 0, 0x2e, 0);
    neg = -(v2 / 2);
    if (neg == 0) {
        neg = -1;
    }
    fn_8011BBD8(resolved, 0, 0x2d, 0, neg);
    lbl_8047B610++;
}
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021C190 size 0x178: fn_8021C190 (owner line 5486) */
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
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_8021C190(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    extern u8 fn_80201704();
    extern void fn_80202810();
    extern void fn_801F4C14();
    extern u8 fn_80119DD0();
    extern u16 fn_80203B5C();
    extern void fn_8011BBD8();

    s16 val;
    u32 ctx1;
    u32 fieldD9;
    u8* jumpTarget;
    s16 diff;

    jumpTarget = *(u8**)(lbl_8047B610 + 1);
    ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);

    if (fn_802026E4(ctx1, 0x2d) == 0) {
        val = 0;
    } else {
        val = fn_80202360(ctx1, 0x2d);
    }
    if (val <= 0) {
        lbl_80478D78[5] = 0;
        lbl_8047B610 = jumpTarget;
        return;
    }
    if (fn_80201704(ctx1) == 1) {
        fn_80202810(ctx1, 0x2d);
        fn_801F4C14(0, 0, 0x43, 0, ctx1);
        lbl_80478D78[5] = 1;
        lbl_8047B610 = jumpTarget;
        return;
    }
    diff = (s16)fn_80119DD0(0x2d) - val;
    if (diff < 0) {
        diff = 0;
    }
    fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(s32)(u16)fn_80203B5C(ctx1, (u16)(1 << diff)));
    lbl_80379F58[0x16002] = (u8)val;
    fn_80202810(ctx1, 0x2d);
    fn_801F4C14(0, 0, 0x43, 0, ctx1);

    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_801F4C14
#undef fn_80202810
#undef fn_80201704
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021C308 size 0x188: fn_8021C308 (owner line 5555) */
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
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
void fn_8021C308(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205184();
    extern s32 fn_8011BEB4();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    extern s32 fn_80232110();
    extern void fn_8011BBD8();
    extern void fn_80202810();

    u32 ctx2;
    u8* jumpTarget;
    u32 aux;
    u16 v1;
    u16 v2;
    u32 ctx1;
    u32 fieldD9;
    s16 val;
    u32 obj;

    jumpTarget = *(u8**)(lbl_8047B610 + 1);
    ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    aux = fn_80205184(ctx1);
    v1 = (u16)fn_8011BEB4(fieldD9, 0, 0x2f, 0);
    v2 = (u16)fn_8011BEB4(fieldD9, 0, 0x30, 0);

    if (fn_802026E4(ctx1, 0x2d) == 0) {
        val = 0;
    } else {
        val = fn_80202360(ctx1, 0x2d);
    }
    ctx2 = fn_801F025C(0x12, 0);
    obj = fn_801F025C(2, ctx2);
    if (val <= 0) {
        lbl_8047B610 = jumpTarget;
        return;
    }
    if (lbl_80478D78[6] != 1) {
        s32 result = fn_80232110(ctx1, ctx2, obj, aux, v1, v2);
        ctx2 = (s32)val * result;
        lbl_80379F58[0x16002] = (u8)val;
        if (fn_802026E4(ctx1, 0x32) == 1) {
            ctx2 = ((s32)ctx2 * 15) / 10;
        }
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, ctx2);
    }
    fn_80202810(ctx1, 0x2d);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_80202810
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_80205184
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021C490 size 0xF8: fn_8021C490 (owner line 3824) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_80132A38 msgctrlSetValue
void fn_8021C490(void) {
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_80132A38();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    u32 ctx1 = fn_801F025C(0x11, 0);
    s16 field2d;

    if (fn_802026E4(ctx1, 0x2d) == 0) {
        field2d = 0;
    } else {
        field2d = fn_80202360(ctx1, 0x2d);
    }
    if (field2d == fn_80119DD0(0x2d)) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 1;
    } else {
        if (fn_802025B8(ctx1, 0x2d) == 2) {
            fn_8020248C(ctx1, 0x2d, 0);
        }
        field2d = fn_80202360(ctx1, 0x2d);
        fn_80132A38(0x2f, field2d);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_80132A38
#undef fn_801F4C14
#undef fn_801F025C

/* 0x8021C588 size 0xB0: fn_8021C588 (owner line 9347) */
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
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_8021C588(void) {
    extern void* fn_801F025C(u32, u32);
    extern u16   fn_80207BF4(void*);
    extern void  fn_8021C638();
    extern u8    fn_801F37B0(u32, void*, void*, u32);
    void* ctx = fn_801F025C(0x12, 0);
    u16 moveId = fn_80207BF4(ctx);
    u32 target = *(u32*)(lbl_8047B610 + 1);
    u8 found = (u8)fn_801F37B0(0, fn_8021C638, ctx, 1) != 1;
    if (found != 0) {
        lbl_8047B610 = (u8*)target;
        return;
    }
    if (moveId == 0xf || moveId == 0x48) {
        lbl_80478D78[5] = 2;
        lbl_8047B610 = (u8*)target;
        return;
    }
    lbl_8047B610 += 5;
}
#undef fn_801F37B0
#undef fn_80207BF4
#undef fn_801F025C

/* 0x8021C638 size 0xBC: fn_8021C638 (owner line 15056) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u32 fn_8021C638(u32 r3, u32 r4, u32 r5)

{
    extern void fightFloorSetStatus();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
  u8 cVar3;
  u32 originalR3 = r3;

  r3 = fightOutPokemonGetTokuseiDataId(r5);
  cVar3 = fightOutPokemonCheckFightOut(originalR3);
  if (cVar3 == 0) {
    return 1;
  }
  cVar3 = fn_802026E4(originalR3,0xb);
  if ((cVar3 == 1) && ((u16)r3 != 0x2b)) {
    fightFloorSetStatus(0,0,0x4b,0,originalR3);
    if (originalR3 == r5) {
      lbl_80478D78[5] = 0;
    }
    else {
      lbl_80478D78[5] = 1;
    }
    return 0;
  }
  return 1;
}

/* 0x8021C6F4 size 0x10: fn_8021C6F4 (owner line 204) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u8* fn_8021C6F4(void) { return lbl_8047B610++; }

/* 0x8021C704 size 0x58: WS_NEKODAMASHI (owner line 15086) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void WS_NEKODAMASHI(void)

{
    extern u32 fn_801F025C();
    extern s32 fn_8012640C();
  u32 uVar2;
  u16 sVar3;

  uVar2 = fn_801F025C(0x11,0);
  sVar3 = (int)fn_8012640C(uVar2,0,0xed,0);
  if (sVar3 != 0) {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021C75C size 0x1A4: fn_8021C75C (owner line 8827) */
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
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_8021C75C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern void fn_801F4C14();
    extern u16 fn_80203B5C();
    extern u8  fn_80201704();
    extern u8  fn_802026E4();
    extern void fn_8020248C();
    extern void fn_80201EB0();
    extern u8  fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xd9, 0);
    u16 val;

    fn_801F025C(0x12, 0);
    fn_801F4C14(0, 0, 0x43, 0, ctx1);

    val = (u16)fn_80203B5C(ctx1, 1);
    fn_8011BBD8(fieldD9, 0, 0x2d, 0, -val);

    if ((u8)fn_80201704(ctx1) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (fn_802026E4(ctx1, 3) == 1 ||
        fn_802026E4(ctx1, 4) == 1 ||
        fn_802026E4(ctx1, 5) == 1 ||
        fn_802026E4(ctx1, 6) == 1 ||
        fn_802026E4(ctx1, 7) == 1) {
        lbl_80478D78[5] = 1;
    } else {
        lbl_80478D78[5] = 0;
    }

    fn_8020248C(ctx1, 8, 0);
    fn_80201EB0(ctx1, 8, 3);

    if (fn_801FECD4(ctx1) == 1) {
        fn_801FE7EC(ctx1, 0x7c, 0, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80201704
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_801F4C14
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021C900 size 0x100: fn_8021C900 (owner line 9464) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8021C900(void) {
    u32 ctx1 = fightTargetGetPtrAsNowFightType(0x11, 0);
    u32 fieldD9 = pokemonGetStatus(ctx1, 0, 0xD9, 0);
    s32 val = wazaGetStatus(fieldD9, 0, 0x2d, 0);
    u32 ctx2 = fightTargetGetPtrAsNowFightType(0x12, 0);
    u32 poke2 = fightOutPokemonGetPokemonPtr(ctx2);
    s32 stat = pokemonGetStatus(poke2, 0, 0x87, 0);
    u8 op = ((FightSeqOpU8Operand*)lbl_8047B610)->operand;

    switch (op) {
    case 0:
        val *= -1;
        break;
    case 1:
        val = val / 2;
        if (val == 0) {
            val = 1;
        }
        if (stat / 2 < val) {
            val = stat / 2;
        }
        break;
    case 2:
        val = val << 1;
        break;
    }
    wazaSetStatus(fieldD9, 0, 0x2d, 0, val);
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma switch_tables reset
#pragma opt_common_subs reset
#pragma opt_propagation reset
#pragma opt_lifetimes reset
#pragma dont_inline reset
#pragma scheduling reset
#pragma optimize_for_size reset
#pragma optimization_level reset
