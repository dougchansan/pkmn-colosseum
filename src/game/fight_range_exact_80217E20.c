/**
 * @file fight_range_exact_80217E20.c
 * @brief Strict target-order fight island, 0x80217E20 - 0x80218BD4.
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

/* 0x80217E20 size 0x1F8: fn_80217E20 (owner line 13060) */
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
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_802096E8 fightWazaIsHit
#define fn_80200B10 fightOutPokemonInitJoutaiKeep
#define fn_8020A2B8 fightWazaBiosCopy
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80217E20(void)

{
    extern u32 fn_80119DD0();
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_80200B10();
    extern u32 fn_80202360();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802096E8();
    extern void fn_8020A2B8();
  u32 uVar3;
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  u8 cVar8;
  u8 uVar5;
  u32 uVar6;
  u16 sVar7;
  u32 uVar9;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xf8,0);
  uVar4 = fn_80205184((void*)uVar1);
  cVar8 = fn_802096E8(uVar2);
  if (cVar8 == 0) {
    fn_80200B10(uVar1);
    lbl_8047B610 = lbl_80375FDF;
  }
  else {
    cVar8 = fn_802025B8(uVar1,0x2f);
    if (cVar8 == 2) {
      fn_8020248C(uVar1,0x2f,0);
      cVar8 = fn_802026E4(uVar1,0x22);
      if (cVar8 == 0) {
        fn_8020248C(uVar1,0x22,0);
        fn_8020A2B8(uVar3,uVar2);
      }
    }
    uVar6 = 1;
    cVar8 = fn_802026E4(uVar1,0x2f);
    if (cVar8 == 1) {
      uVar5 = fn_80119DD0(0x2f);
      uVar6 = fn_80202360(uVar1,0x2f);
      if ((short)uVar6 == uVar5) {
        fn_80202810(uVar1,0x2f);
        fn_80202810(uVar1,0x22);
      }
    }
    sVar7 = fn_8011BEB4(0,uVar4,7,0);
    for (uVar9 = 1; (int)(uVar9 & 0xffff) < (int)(short)uVar6; uVar9 = uVar9 + 1) {
      sVar7 = sVar7 << 1;
    }
    fn_8011BBD8(uVar2,0,0x2f,0,sVar7);
    cVar8 = fn_802026E4(uVar1,0x1a);
    if (cVar8 == 1) {
      uVar9 = (u16)fn_8011BEB4(uVar2,0,0x2f,0);
      fn_8011BBD8(uVar2,0,0x2f,0,(uVar9 << 1) & 0xfffe);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  return;
}
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_80202810
#undef fn_8020A2B8
#undef fn_80200B10
#undef fn_802096E8
#undef fn_80205184
#undef fn_8012640C
#undef fn_801F025C

/* 0x80218018 size 0x1C0: fn_80218018 (owner line 13139) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_80218018(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u8 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u32 _fightSeqWsHorobinoutaSub__FPvUsPv();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    u32 attacker;
    u32 pokemon;
    s8 slot;
    u8 slot8;
    int slotIndex;
    u32 move;
    u32 target;
    u32 ability;
    u8 pp;
    u8 newPp;
    u32 flags;
    int callbackResult;
    u8* nextPc;

    callbackResult = 0;
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    if (fn_80229934(move, attacker, target) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fightFloorLoopValidFightOutPokemon(
            0, _fightSeqWsHorobinoutaSub__FPvUsPv, &callbackResult, 0);
        if (attacker != 0) {
            ability = (u8)fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
                0, 0x2e, 0, attacker);
            if (ability != 0) {
                pokemon = fightOutPokemonGetPokemonPtr(attacker);
                slot = pokemonSearchWazaDataId(pokemon, 0xc3);
                if (slot >= 0) {
                    slotIndex = slot;
                    pp = (u8)pokemonGetStatus(pokemon, 0, 0x80, slotIndex);
                    newPp = ability < pp ? (u8)(pp - ability) : 0;
                    pokemonSetStatus(pokemon, 0, 0x80, slotIndex, newPp);
                    slot8 = (u8)slot;
                    if (fn_802026E4(attacker, 0x10) == 0 &&
                        fn_802026E4(attacker, 0x31) == 1) {
                        flags = fn_80201890(attacker, 0x31);
                        if ((flags & (1 << slot8)) == 0 &&
                            fightOutPokemonIsUseHensinBuff(attacker) == 1) {
                            fightOutPokemonSetHensinPokemonStatusId(
                                attacker, 0x80, slot8, 0);
                        }
                    }
                }
            }
        }
        if (callbackResult == 0) {
            nextPc = *(u8**)(lbl_8047B610 + 1);
        } else {
            nextPc = lbl_8047B610 + 5;
        }
        lbl_8047B610 = nextPc;
    }
}

/* 0x802181D8 size 0x98: _fightSeqWsHorobinoutaSub__FPvUsPv (owner line 13214) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
u32 _fightSeqWsHorobinoutaSub__FPvUsPv(void* r3, u16 r4, void* r5)

{
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802062FC();
    extern u32 fn_80207BF4();
  u32 pokemon;
  int* count;
  u8 cVar2;
  u32 sVar1;

  count = (int*)r5;
  pokemon = (u32)r3;
  cVar2 = fn_802062FC();
  if (cVar2 == 0) {
    return 1;
  }
  sVar1 = fn_80207BF4(pokemon);
  cVar2 = fn_802025B8(pokemon,0x1e);
  if (cVar2 != 2 || (u16)sVar1 == 0x2b) {
    return 1;
  }
  fn_8020248C(pokemon,0x1e,0);
  *count = *count + 1;
  return 1;
}
#undef fn_80207BF4
#undef fn_802062FC

/* 0x80218270 size 0x64: WS_MIYABURU (owner line 13245) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_MIYABURU(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x12,0);
  cVar2 = fn_802025B8(uVar1,0x19);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x19,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F025C

/* 0x802182D4 size 0xE8: fn_802182D4 (owner line 4398) */
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
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6D9C fightSideGetCountAsJoutaiDataId
#define fn_801254B4 pokemonSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
void fn_802182D4(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801254B4();
    extern u8  fn_801F6E98();
    extern s16 fn_801F6D9C();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 tmp = fn_801F025C(3, ctx1);
    s16 val;

    if (fn_801F6E44(tmp, 0x4a) != 2) {
        goto matched;
    }
    if (fn_801F6E98(tmp, 0x4a) == 1) {
        val = fn_801F6D9C(tmp, 0x4a);
    } else {
        val = 0;
    }
    if (val != fn_80119DD0(0x4a)) {
        goto notmatched;
    }
matched:
    fn_801254B4(ctx1, 0, 0x118, 0, 1);
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
notmatched:
    fn_801F6DF0(tmp, 0x4a, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F6DF0
#undef fn_801254B4
#undef fn_801F6D9C
#undef fn_801F6E98
#undef fn_801F6E44
#undef fn_801F025C

/* 0x802183BC size 0xC0: fn_802183BC (owner line 1583) */
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
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_802183BC(void) {
    extern void* fn_801F025C();
    extern u16 fn_80203B5C();
    extern void fn_8011BBD8();
    extern void* fn_8012640C();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    u32 ctx2;
    u32 resolved = (u32)fn_8012640C(ctx, 0, 0xd9, 0);
    ctx2 = (u32)fn_801F025C(0x12, 0);
    if (fn_802025B8(ctx2, 0x18) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx2, 0x18, 0);
        fn_8011BBD8(resolved, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 2));
        lbl_8047B610 += 5;
    }
}
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_8012640C
#undef fn_801F025C

/* 0x8021847C size 0x3A8: fn_8021847C (owner line 13282) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801248C4 pokemonGetTokuseiDataId
#define fn_801252E0 pokemonInitJoutai
#define fn_8012A5B0 heroGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F7258 fightSideGetValidFightTrainerPtr
#define fn_801F9930 fightTrainerCheckTemotiPokemonFightEntry
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80206780 fightOutPokemonCheckValid
#define fn_80206A04 fightPokemonCheckValid
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
void fn_8021847C(void)

{
    extern u16 fn_801248C4();
    extern void fn_801252E0();
    extern u32 fn_8012A5B0();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_801F7258();
    extern int fn_801F9930();
    extern u32 fn_801FB1C0();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_80202810();
    extern void fn_80202998();
    extern void fn_80205B8C();
    extern u32 fn_80205BE8();
    extern u8 fn_80206780();
    extern u8 fn_80206A04();
  u32 uVar1;
  u32 sVar7;
  u32 iVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar8;
  u32 uVar5;
  u32 trainerStatus;
  int iVar6;
  u32 uVar9;
  u32 uVar10;

  uVar1 = fn_801F025C(0x11,0);
  sVar7 = fn_80205184((void*)uVar1);
  iVar2 = fn_801F025C(0xe,uVar1);
  uVar3 = fn_801F025C(2,uVar1);
  uVar4 = (u16)fn_801F54A4(0,0,0x16,0);
  if ((sVar7 & 0xffff) == 0xd7) {
    lbl_80478D78[5] = 0;
    sVar7 = fn_80207BF4(uVar1);
    if ((sVar7 & 0xffff) != 0x2b) {
      fn_80205B8C(uVar1);
      fn_801252E0();
      fn_80202998(uVar1,0);
      fn_80202810(uVar1,0x17);
      cVar8 = fn_801FECD4(uVar1);
      if (cVar8 == 1) {
        fn_801FE7EC(uVar1,0x7c,0,0);
      }
    }
    else {
      lbl_80478D78[5] |= 1;
    }
    if ((iVar2 != 0) && (cVar8 = fn_80206780(iVar2), cVar8 == 1)) {
      fn_801F4C14(0,0,0x4b,0,iVar2);
      sVar7 = fn_80207BF4(iVar2);
      if ((sVar7 & 0xffff) != 0x2b) {
        fn_80205B8C(iVar2);
        fn_801252E0();
        fn_80202998(iVar2,0);
        fn_80202810(iVar2,0x17);
        cVar8 = fn_801FECD4(iVar2);
        if (cVar8 == 1) {
          fn_801FE7EC(iVar2,0x7c,0,0);
        }
      }
      else {
        lbl_80478D78[5] |= 2;
      }
    }
    for (iVar6 = 0; iVar6 < uVar4; iVar6 = iVar6 + 1) {
      iVar2 = fn_801F7258(uVar3,iVar6 & 0xffff);
      if (iVar2 != 0) {
        uVar10 = fn_801FB1C0(iVar2,0,0x44,0);
        uVar9 = 0;
        do {
          trainerStatus = fn_801F9930(iVar2,
              fn_8012A5B0(uVar10,3,uVar9 & 0xffff));
          if ((trainerStatus != 0) && (cVar8 = fn_80206A04(), cVar8 != 0)) {
            uVar1 = fn_80205BE8(trainerStatus);
            sVar7 = fn_801248C4();
            if ((sVar7 & 0xffff) != 0x2b) {
              fn_801252E0(uVar1);
            }
          }
          uVar9 = uVar9 + 1;
        } while (uVar9 < 6);
      }
    }
  }
  else {
    lbl_80478D78[5] = 4;
    fn_80205B8C(uVar1);
    fn_801252E0();
    fn_80202810(uVar1,0x17);
    cVar8 = fn_801FECD4(uVar1);
    if (cVar8 == 1) {
      fn_801FE7EC(uVar1,0x7c,0,0);
    }
    if ((iVar2 != 0) && (cVar8 = fn_80206780(iVar2), cVar8 == 1)) {
      fn_801F4C14(0,0,0x4b,0,iVar2);
      fn_80205B8C(iVar2);
      fn_801252E0();
      fn_80202998(iVar2,0);
      fn_80202810(iVar2,0x17);
      cVar8 = fn_801FECD4(iVar2);
      if (cVar8 == 1) {
        fn_801FE7EC(iVar2,0x7c,0,0);
      }
    }
    for (uVar1 = 0; uVar1 < uVar4; uVar1 = uVar1 + 1) {
      iVar2 = fn_801F7258(uVar3,uVar1 & 0xffff);
      if (iVar2 != 0) {
        iVar6 = fn_801FB1C0(iVar2,0,0x44,0);
        uVar10 = 0;
        do {
          uVar9 = fn_801F9930(iVar2,
              fn_8012A5B0(iVar6,3,uVar10 & 0xffff));
          if ((uVar9 != 0) && (cVar8 = fn_80206A04(), cVar8 != 0)) {
            fn_80205BE8(uVar9);
            fn_801252E0();
          }
          uVar10 = uVar10 + 1;
        } while (uVar10 < 6);
      }
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_80207BF4
#undef fn_80206A04
#undef fn_80206780
#undef fn_80205BE8
#undef fn_80205B8C
#undef fn_80205184
#undef fn_80202998
#undef fn_80202810
#undef fn_801FECD4
#undef fn_801FE7EC
#undef fn_801FB1C0
#undef fn_801F9930
#undef fn_801F7258
#undef fn_801F54A4
#undef fn_801F4C14
#undef fn_801F025C
#undef fn_8012A5B0
#undef fn_801252E0
#undef fn_801248C4

/* 0x80218824 size 0x248: fn_80218824 (owner line 13431) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_80218824(void)
{
    extern u16 fn_800E0C54();
    extern u32 GSmsgGetGSchar();
    extern u32 wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus(u32, u32, u32, u32, u32);
    extern u8 pokemonWazaCheckValid();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonInitJoutaiKeep();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    u32 outPokemon;
    u32 pokemon;
    u16 slotMove;
    u16 move;
    u8 i;
    u8 pp;
    u8 amount;
    u32 mask;

    fightTargetGetPtrAsNowFightType(0x11, 0);
    outPokemon = fightTargetGetPtrAsNowFightType(0x12, 0);
    pokemon = fightOutPokemonGetPokemonPtr(outPokemon);
    move = pokemonGetStatus(outPokemon, 0, 0xF0, 0);

    if (move != 0 && move != 0x165 && move != 0xFFFF && move != 0x164) {
        pp = 0;
        for (i = 0; i < 4; i++) {
            if (pokemonWazaCheckValid(pokemon, i) != 0) {
                slotMove = pokemonGetStatus(pokemon, 0, 0x7F, i);
                if (move == slotMove) {
                    pp = pokemonGetStatus(pokemon, 0, 0x80, i);
                    break;
                }
            }
        }

        if (i >= 4) {
            lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        } else {
            if (pp < 2) {
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
            } else {
                amount = (u8)((fn_800E0C54() % 4) + 2);
                if (pp < amount) {
                    amount = pp;
                }

                msgctrlSetValue(0xD, GSmsgGetGSchar(wazaGetStatus(0, move, 1, 0)));
                msgctrlSetValue(0x2F, amount);
                pp -= amount;
                pokemonSetStatus(pokemon, 0, 0x80, i, pp);

                if (fn_802026E4(outPokemon, 0x10) == 0 &&
                    fn_802026E4(outPokemon, 0x31) == 1) {
                    mask = fn_80201890(outPokemon, 0x31);
                    if ((mask & (1 << i)) == 0 &&
                        fightOutPokemonIsUseHensinBuff(outPokemon) == 1) {
                        fightOutPokemonSetHensinPokemonStatusId(outPokemon, 0x80, i, 0);
                    }
                }

                if (pp == 0) {
                    fightOutPokemonInitJoutaiKeep(outPokemon);
                }
                lbl_8047B610 += 5;
            }
        }
        return;
    }
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
}

/* 0x80218A6C size 0x100: fn_80218A6C (owner line 13509) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_80218A6C(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void wazaSetStatus();
    u32 ctx;
    u32 move;
    u32 pokemon;
    u16 currentHp;
    u16 maxHp;
    u8 key;
    s32 offset;
    s32 i;

    ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(ctx, 0, 0xd9, 0);
    pokemon = fightOutPokemonGetPokemonPtr(ctx);
    currentHp = (u16)pokemonGetStatus(pokemon, 0, 0x83, 0);
    maxHp = (u16)pokemonGetStatus(pokemon, 0, 0x87, 0);
    key = (u8)((currentHp * 0x30) / maxHp);
    if (key == 0 && currentHp != 0) {
        key = 1;
    }

    offset = 0;
    for (i = 6; i > 0; i--) {
        if (key <= lbl_80279F7C[offset]) {
            break;
        }
        offset += 2;
    }

    key = *(volatile u8*)((u32)lbl_80279F7C + offset + 1);
    wazaSetStatus(move, 0, 0x2f, 0, key);
    lbl_8047B610++;
}

/* 0x80218B6C size 0x68: fn_80218B6C (owner line 1323) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u8 fn_80218B6C(u16 a, u16 b) {
    u8 key = (u8)((a * 0x30) / b);
    s32 offset;
    s32 i;

    if (key == 0 && a != 0) {
        key = 1;
    }
    offset = 0;
    for (i = 6; i > 0; i--) {
        if (key <= lbl_80279F7C[offset]) {
            break;
        }
        offset += 2;
    }
    return lbl_80279F7C[offset + 1];
}

#pragma switch_tables reset
#pragma opt_common_subs reset
#pragma opt_propagation reset
#pragma opt_lifetimes reset
#pragma dont_inline reset
#pragma scheduling reset
#pragma optimize_for_size reset
#pragma optimization_level reset
