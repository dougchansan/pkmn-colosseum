/**
 * @file fight_range_exact_80217018.c
 * @brief Strict target-order fight island, 0x80217018 - 0x80217D34.
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

/* 0x80217018 size 0x9C: fn_80217018 (owner line 2495) */
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
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F6DF0 fightSideWriteJoutaiDataId
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80120B00 pokemonGetMezamerupower
#define fn_8011BBD8 wazaSetStatus
void fn_80217018(void) {
    extern u32 fn_80205B8C();
    extern void fn_80120B00();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    u32 ctx = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    u32 poke = fn_80205B8C(ctx);
    u16 outA, outB;

    fn_80120B00(poke, &outA, &outB);
    fn_8011BBD8(fieldD9, 0, 0x2f, 0, outA);
    fn_8011BBD8(fieldD9, 0, 0x30, 0, outB);
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

/* 0x802170B4 size 0x108: fn_802170B4 (owner line 4949) */
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
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_802170B4(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_801F4C14();
    extern u8 fn_801F453C();
    extern u16 fn_80203B5C();
    extern u8 fn_80201704();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8 sel;
    s32 val;

    fn_801F4C14(0, 0, 0x43, 0, ctx1);
    sel = (u8)fn_801F453C(0, 1);

    if (!fn_80201704(ctx1)) {
        if (sel == 0) {
            val = (u16)fn_80203B5C(ctx1, 2);
        } else if (sel == 1) {
            val = (fn_80203B5C(ctx1, 1) * 20) / 30;
        } else {
            val = (u16)fn_80203B5C(ctx1, 4);
        }
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(s32)val);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_80201704
#undef fn_801F453C
#undef fn_801F4C14
#undef fn_8012640C
#undef fn_801F025C

/* 0x802171BC size 0x64: WS_MARUKUNARU (owner line 12759) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_MARUKUNARU(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0x1a);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x1a,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F025C

/* 0x80217220 size 0x1B4: fn_80217220 (owner line 9299) */
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
#define fn_801F4C14 fightFloorSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_800FA280 GSmsgGetGSchar
#define fn_80132A38 msgctrlSetValue
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6EEC fightSideInitJoutaiDataId
void fn_80217220(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u8  fn_802026E4();
    extern void fn_801F4C14();
    extern s32 fn_8011BEB4();
    extern u8  fn_801F6E98();
    extern void fn_80202810();
    extern u32 fn_80201D84();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fn_80201C58();
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern void fn_80211B94();
    extern u8  fn_801F6EEC();
    extern u8  lbl_80378EFD[];
    extern u8  lbl_80378F11[];
    extern u8  lbl_80378F25[];
    u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 sub = fn_801F025C(2, ctx1);
    u32 ctx2 = fn_801F025C(0x12, 0);
    u32 tmp;
    u32 target;
    u16 tmpNarrow;

    if (fn_802026E4(ctx1, 0xe) == 1) {
        tmp = fn_80201D84(ctx1, 0xe);
        tmpNarrow = (u16)tmp;
        if (tmpNarrow != 0) {
            target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(tmp, val);
            if (target != 0) {
                fn_801F4C14(0, 0, 0x42, 0, target);
                {
                    u32 t = fn_80201C58(ctx1, 0xe);
                    fn_80132A38(0xd, fn_800FA280(fn_8011BEB4(0, t, 1, 0)));
                }
                fn_80202810(ctx1, 0xe);
                fn_80211B94(lbl_8047B62C, (void*)lbl_80378EFD, 0);
                fn_801F4C14(0, 0, 0x42, 0, ctx2);
            }
        }
    }

    if (fn_802026E4(ctx1, 0x1c) == 1) {
        fn_80202810(ctx1, 0x1c);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80378F11, 0);
    }

    if (fn_801F6E98(sub, 0x4a) == 1) {
        fn_801F6EEC(sub, 0x4a);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80378F25, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F6EEC
#undef fn_801F6E98
#undef fn_80202810
#undef fn_80132A38
#undef fn_800FA280
#undef fn_8011BEB4
#undef fn_801F4C14
#undef fn_801F025C
#undef fn_801F54A4

/* 0x802173D4 size 0x60: WS_JIKOANJI (owner line 12779) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80201600 fightOutPokemonCopyAllAbiCnt
void WS_JIKOANJI(void)

{
    extern u32 fn_801F025C();
    extern void fn_80201600();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(0x12,0);
  fn_80201600(uVar1,uVar2);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
#undef fn_80201600
#undef fn_801F025C

/* 0x80217434 size 0xF0: fn_80217434 (owner line 2876) */
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
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_801254B4 pokemonSetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80217434(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern u16 fn_80203B5C();
    extern u32 fn_801254B4();
    extern void fn_8011BBD8();
    u32 ctx = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    u16 statA = (u16)fn_8012640C(fn_80205B8C(ctx), 0, 0x83, 0);
    u16 val2 = fn_80203B5C(ctx, 2);
    s8 e6field = (s8)fn_8012640C(ctx, 0, 0xe6, 0);

    if (e6field < 0xc && statA > val2) {
        fn_801254B4(ctx, 0, 0xe6, 0, 0xc);
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, val2);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_8011BBD8
#undef fn_801254B4
#undef fn_80203B5C
#undef fn_80205B8C
#undef fn_8012640C
#undef fn_801F025C

/* 0x80217524 size 0x84: fn_80217524 (owner line 4917) */
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
void fn_80217524(void) {
    extern u32 fn_801F2988();
    extern void fn_801F2934();
    extern void fn_801F4C14();
    u8 val = (u8)fn_801F2988(0, 0x53);

    if (val != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x53, 0);
        lbl_80478D78[5] = 4;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988

/* 0x802175A8 size 0x23C: fn_802175A8 (owner line 12795) */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_802175A8(void)

{
    extern u16 wazaGetStatus();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u16 fightActionGetKindDataId();
    extern u8 fightActionCheckValid();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonIsAlly();
    extern u8 fightOutPokemonCheckFightOut();
    extern u16 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_80379F58[];
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  u32 context;
  u32 relativeTarget;
  u32 candidate;
  u32 action;
  u32 waza;
  u32 selected;

  context = fightTargetGetPtrAsNowFightType(0x14,0);
  relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(
      context,(u16)fightFloorGetStatus(0,0,0x14,0));
  if (fightOutPokemonCheckFightOut(context) == 0) {
    lbl_8047B648 = lbl_8047B649;
  } else {
    candidate = 0;
    selected = 0;
    while (lbl_8047B648 < lbl_8047B649) {
      candidate = fightFloorGetStatus(0,0,0x5d,lbl_8047B648);
      if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 &&
          context != candidate && fightOutPokemonIsAlly(context,candidate) != 1 &&
          (action = pokemonGetStatus(candidate,0,0xfe,0)) != 0 &&
          fightActionCheckValid() != 0 &&
          fightActionGetKindDataId(action) == 0x13) {
        waza = pokemonGetStatus(candidate,0,0xd9,0);
        if (fightOutPokemonGetUseWazaDataId(candidate) == 0xe4 &&
            (u16)relativeTarget == wazaGetStatus(waza,0,0x29,0) &&
            fn_802026E4(candidate,8) != 1 &&
            fn_802026E4(candidate,7) != 1 &&
            (s32)pokemonGetStatus(candidate,0,0xf9,0) != 1) {
          selected = candidate;
          break;
        }
      }
      lbl_8047B648++;
    }
    if (selected != 0) {
      fightFloorSetStatus(0,0,0x42,0,selected);
      lbl_80379F58[0x16002] = 1;
      lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
      pokemonSetStatus(candidate,0,0x112,0,1);
      lbl_8047B610 = lbl_8047B610 + 5;
      return;
    }
  }
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}

/* 0x802177E4 size 0x110: fn_802177E4 (owner line 4053) */
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
#define fn_8011BBD8 wazaSetStatus
#define fn_80132A38 msgctrlSetValue
void fn_802177E4(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern void fn_80132A38();
    u32 ctx = fn_801F025C(0x11, 0);
    u8 idx;
    u32 fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    u8 v = fn_800E0C54() % 100;
    u16 result;

    if (v < 5) {
        result = 10;
        idx = 4;
    } else if (v < 15) {
        result = 30;
        idx = 5;
    } else if (v < 35) {
        result = 50;
        idx = 6;
    } else if (v < 65) {
        result = 70;
        idx = 7;
    } else if (v < 85) {
        result = 90;
        idx = 8;
    } else if (v < 95) {
        result = 110;
        idx = 9;
    } else {
        result = 150;
        idx = 10;
    }
    fn_8011BBD8(fieldD9, 0, 0x2f, 0, result);
    fn_80132A38(0x2f, idx);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_80132A38
#undef fn_8011BBD8
#undef fn_8012640C
#undef fn_801F025C

/* 0x802178F4 size 0xA8: fn_802178F4 (owner line 1491) */
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
void fn_802178F4(void) {
    extern void* fn_801F025C();
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    u32 a = (u32)fn_801F025C(0x2, ctx1);
    if (fn_801F6E44(a, 0x4b) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 0;
    } else {
        fn_801F6DF0(a, 0x4b, 0);
        lbl_80478D78[5] = 5;
    }
    lbl_8047B610++;
}
#undef fn_801F6DF0
#undef fn_801F4C14
#undef fn_801F6E44
#undef fn_801F025C

/* 0x8021799C size 0x148: fn_8021799C (owner line 5000) */
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
#define fn_8011BBD8 wazaSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80209960 fightWazaInitJoutaiDataId
void fn_8021799C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern u16 fn_800E0C54(void);
    extern u16 fn_80203B5C();
    extern u8 fn_80201704();
    extern void fn_80209960();
    extern u8 lbl_80375F98[16];
    extern u8 lbl_80377AD9[0x2A];
    extern u8 lbl_80377AB8[0x21];
    u32 ctx2;
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8 val;
    ctx2 = fn_801F025C(0x12, 0);
    val = (u8)fn_800E0C54();

    if (val < 0x66) {
        fn_8011BBD8(fieldD9, 0, 0x2f, 0, 0x28);
    } else if (val < 0xb2) {
        fn_8011BBD8(fieldD9, 0, 0x2f, 0, 0x50);
    } else if (val < 0xcc) {
        fn_8011BBD8(fieldD9, 0, 0x2f, 0, 0x78);
    } else {
        u16 v4 = (u16)fn_80203B5C(ctx2, 4);
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(s32)v4);
    }

    if (val < 0xcc) {
        lbl_8047B610 = (u8*)lbl_80375F98;
    } else if (fn_80201704(ctx2) == 1) {
        lbl_8047B610 = (u8*)lbl_80377AD9;
    } else {
        fn_80209960(fieldD9, 0x43);
        lbl_8047B610 = (u8*)lbl_80377AB8;
    }
}
#undef fn_80209960
#undef fn_80201704
#undef fn_80203B5C
#undef fn_8011BBD8
#undef fn_8012640C
#undef fn_801F025C

/* 0x80217AE4 size 0xEC: fn_80217AE4 (owner line 12868) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
void fn_80217AE4(void)

{
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 moveId;
  u32 uVar3;
  u16 sVar5;
  u32 uVar2;
  u16 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  uVar3 = (int)fn_8012640C(uVar1,0,0xd9,0);
  moveId = fn_80205184((void*)uVar1);
  sVar5 = fn_8011BEB4(0,moveId,9,0);
  uVar4 = (int)fn_8012640C(uVar2,0,0x99,0);
  if (sVar5 == 0x79) {
    uVar4 = (uVar4 * 10) / 0x19;
  }
  else {
    uVar4 = ((0xff - uVar4) * 10) / 0x19;
  }
  fn_8011BBD8(uVar3,0,0x2f,0,uVar4);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_80205B8C
#undef fn_80205184
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_8011BBD8

/* 0x80217BD0 size 0x1C: fn_80217BD0 (owner line 9421) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u16 fn_80217BD0(u16 x) { return (u16)(((0xFF - x) * 10) / 25); }

/* 0x80217BEC size 0x18: fn_80217BEC (owner line 9420) */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u16 fn_80217BEC(u16 x) { return (u16)((x * 10) / 25); }

/* 0x80217C04 size 0x130: fn_80217C04 (owner line 4176) */
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
#define fn_802096E8 fightWazaIsHit
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80217C04(void) {
    extern void fn_80202810();
    extern void fn_8011BBD8();
    extern u32 fn_80205184();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    extern u8 fn_802096E8();
    extern s32 fn_8011BEB4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 poke1 = fn_80205184(ctx1);
    u8 flag = fn_802096E8(fieldD9);
    s16 field2e;
    u16 n;
    u32 result;
    s32 i;

    if (flag == 0) {
        if (fn_802026E4(ctx1, 0x2e) == 1) {
            fn_80202810(ctx1, 0x2e);
        }
        lbl_8047B610 = (u8*)lbl_80375FDF;
        return;
    }
    if (fn_802025B8(ctx1, 0x2e) == 2) {
        fn_8020248C(ctx1, 0x2e, 0);
    }
    field2e = fn_80202360(ctx1, 0x2e);
    n = fn_8011BEB4(0, poke1, 7, 0);
    result = n;
    for (i = 1; i < field2e; i++) {
        result = result * 2;
    }
    fn_8011BBD8(fieldD9, 0, 0x2f, 0, result);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_80202810
#undef fn_802096E8
#undef fn_80205184
#undef fn_8012640C
#undef fn_801F025C

#pragma switch_tables reset
#pragma opt_common_subs reset
#pragma opt_propagation reset
#pragma opt_lifetimes reset
#pragma dont_inline reset
#pragma scheduling reset
#pragma optimize_for_size reset
#pragma optimization_level reset
