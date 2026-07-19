/**
 * @file fight_range_exact_8022FE20.c
 * @brief Strict target-order fight island, 0x8022FE20 - 0x80230568.
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


/* fn_8022FE20 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_80205224 fightOutPokemonGetMotoWazaDataId
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
u8 fn_8022FE20(void* ctx) {
    extern u16 fn_80205224();
    extern void fn_80202810();
    extern u8 fn_802026E4();
    if ((u8)fn_802026E4(ctx, 0x13) == 1) {
        if ((u16)fn_80205224(ctx) != 0x63) {
            fn_80202810(ctx, 0x13);
        }
    }
    return 1;
}
#undef fn_80202810
#undef fn_80205224

/* fn_8022FE80 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation off
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8022FE80(void) {
    extern u8 lbl_8027A00C[];
    extern u8 lbl_80378798[];
    extern u8 lbl_8047B614;
    extern void* lbl_8047B62C;
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fn_8022E1F8();
    extern void fn_8022E34C();
    extern void fn_8022EB9C();
    extern void fn_80230088();
    extern void fn_8023011C();
    u8 sp[2];
    u32 saved610;
    u8 saved614;
    u32 saved62c;
    u8* start;
    u32 dispatch;

    fightFloorLoopValidFightOutPokemon(0, fn_8023011C, 0, 0);
    saved62c = (u32)lbl_8047B62C;
    start = lbl_80378798;
    saved614 = lbl_8047B614;
    dispatch = (u32)lbl_8027A00C;
    saved610 = (u32)lbl_8047B610;
    lbl_8047B614 = 0;
    lbl_8047B610 = start;
    lbl_8047B62C = (void*)saved62c;
    do {
        ((void (*)(void))*(u32*)(dispatch + (*lbl_8047B610 * 4)))();
    } while (lbl_8047B614 != 1 && lbl_8047B614 != 2);

    lbl_8047B62C = (void*)saved62c;
    lbl_8047B614 = saved614;
    lbl_8047B610 = (u8*)saved610;
    sp[1] = 1;
    fightFloorLoopValidFightOutPokemon(0, fn_8022E34C, &sp[1], 0);
    fightFloorLoopValidFightOutPokemon(0, fn_8022E1F8, 0, 0);
    fightFloorLoopValidFightOutPokemon(0, fn_80230088, 0, 0);
    sp[0] = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_8022EB9C, &sp[0], 0);
}

/* fn_8022FF90 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation off
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8022FF90(void) {
    extern u8 lbl_8027A00C[];
    extern u8 lbl_80378798[];
    extern u8 lbl_8047B614;
    extern void* lbl_8047B62C;
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fn_8022E1F8();
    extern void fn_8022E34C();
    extern void fn_8022EB9C();
    extern void fn_80230088();
    u8 sp[2];
    u32 saved610;
    u8 saved614;
    u32 saved62c;
    u8* start;
    u32 dispatch;

    start = lbl_80378798;
    dispatch = (u32)lbl_8027A00C;
    saved62c = (u32)lbl_8047B62C;
    saved614 = lbl_8047B614;
    saved610 = (u32)lbl_8047B610;
    lbl_8047B614 = 0;
    lbl_8047B610 = start;
    lbl_8047B62C = (void*)saved62c;
    do {
        ((void (*)(void))*(u32*)(dispatch + (*lbl_8047B610 * 4)))();
    } while (lbl_8047B614 != 1 && lbl_8047B614 != 2);

    lbl_8047B62C = (void*)saved62c;
    lbl_8047B614 = saved614;
    lbl_8047B610 = (u8*)saved610;
    sp[1] = 1;
    fightFloorLoopValidFightOutPokemon(0, fn_8022E34C, &sp[1], 0);
    fightFloorLoopValidFightOutPokemon(0, fn_8022E1F8, 0, 0);
    fightFloorLoopValidFightOutPokemon(0, fn_80230088, 0, 0);
    sp[0] = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_8022EB9C, &sp[0], 0);
}

/* fn_80230088 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F4C14 fightFloorSetStatus
u32 fn_80230088(int r3)

{
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern s8 fn_802062FC();
    extern u32 fn_8022BE2C();
  u32 local;
  u32 uVar1;
  u8 cVar2;

  local = r3;
  cVar2 = fn_802062FC();
  if (cVar2 == 0) {
    return 1;
  }
  uVar1 = fn_801F54A4(0,0,0x36,0);
  fn_801F4C14(0,0,0x36,0,local);
  fn_8022BE2C(local,1);
  fn_801F4C14(0,0,0x36,0,uVar1);
  return 1;
}
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_802062FC

/* fn_8023011C */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
u8 fn_8023011C(void* ctx) {
    extern void* fn_8012640C();
    extern void fn_801F4C14();
    if ((u8)(u32)fn_8012640C(ctx, 0, 0x120, 0) == 1) {
        fn_801F4C14(0, 0, 0x46, 0, (u32)ctx);
        fn_801F4C14(0, 0, 0x42, 0, (u32)ctx);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80378721, 0);
    }
    return 1;
}
#undef fn_801F4C14
#undef fn_8012640C

/* fn_802301A8 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u32 fn_802301A8(u32 r3)
{
    extern void wazaSetStatus();
    extern void msgctrlSetValue();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fn_80211B94();
    extern void fn_8022FE80();
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
    extern u8 lbl_80378E1E[];
    extern u8 lbl_80378E46[];
    s8 threshold;
    s8 count;
    s32 signedCount;
    void* context;
    u32 moveStatus;
    u32 pokemon;
    u16 hp;
    u8* script;

    context = (void*)r3;
    if (fightOutPokemonCheckFightOut(context) == 0) {
        return 1;
    }

    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if (fn_802026E4(context, 0x1E) == 1) {
        threshold = fn_80202234(context, 0x1E);
        count = fn_80202108(context, 0x1E);
        fightFloorSetStatus(0, 0, 0x36, 0, context);
        moveStatus = pokemonGetStatus(context, 0, 0xD9, 0);
        signedCount = count;
        msgctrlSetValue(0x2F, (s8)threshold - signedCount);

        if ((int)count < threshold) {
            goto increment_counter;
        }
        pokemon = fightOutPokemonGetPokemonPtr(context);
        hp = pokemonGetStatus(pokemon, 0, 0x83, 0);
        wazaSetStatus(moveStatus, 0, 0x2D, 0, hp);
        fightOutPokemonWriteJoutaiDataId(context, 0x1E);
        script = lbl_80378E1E;
        goto show_message;
increment_counter:
        fn_80201FDC(context, 0x1E, (s8)(signedCount + 1));
        script = lbl_80378E46;
show_message:
        fn_80211B94(lbl_8047B62C, script, 0);
        fn_8022FE80();
    }
    lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
    return 1;
}

/* fn_80230318 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
u32 fn_80230318(u32 input, u32 r4)

{
    extern u32 GSmsgGetGSchar();
    extern void wazaSetStatus();
    extern void wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fn_80201890();
    extern u32 fn_80201C58();
    extern u32 fn_80201D84();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightWazaCreate();
    extern void fightWazaBiosCopy();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fn_80211B94();
    extern void fn_8022FE80();
    extern u8 lbl_80378F39[];
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
  s8 cVar5;
  s8 cVar6;
  u8 hasState;
  void* r3;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;

  u8 auStack_c8 [172];

  r3 = (void*)input;
  if (fightOutPokemonCheckFightOut() == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  hasState = fn_802026E4(r3,0x34);
  if (hasState == 1) {
    cVar5 = fn_80202234(r3,0x34);
    cVar6 = fn_80202108(r3,0x34);
    if (cVar6 < cVar5) {
      goto increment_state;
    }
        uVar1 = fn_80201890(r3,0x34);
        uVar2 = fn_80201C58(r3,0x34);
        uVar3 = fn_80201D84(r3,0x34);
        uVar3 = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(uVar3,r4);
        if (fightOutPokemonCheckFightOut() == 0) {
          uVar3 = fightTargetGetPtrAsNowFightType(0xe,uVar3);
        }
        if ((uVar2 & 0xffff) == 0xf8) {
          lbl_80478D78[5] = 0;
        }
        else {
          lbl_80478D78[5] = 1;
        }
        wazaGetStatus(0,uVar2,1,0);
        msgctrlSetValue(0xd,GSmsgGetGSchar());
        fightFloorSetStatus(0,0,0x42,0,r3);
        r4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
        fightFloorSetStatus(0,0,0x36,0,uVar3);
        {
          u32 callResult = pokemonGetStatus(uVar3,0,0xd9,0);
          uVar3 = callResult;
        }
        fightWazaBiosCopy(auStack_c8,uVar3);
        fightWazaCreate(uVar3,0xffffffff,uVar2,r4,0);
        wazaSetStatus(uVar3,0,0x2d,0,uVar1);
        pokemonSetStatus((void*)r3,0,0x11b,0,0xffff);
        fightOutPokemonWriteJoutaiDataId(r3,0x34);
        fn_80211B94(lbl_8047B62C,lbl_80378F39,0);
        fightWazaBiosCopy(uVar3,auStack_c8);
        fn_8022FE80();
        goto clear_flags;
increment_state:
    fn_80201FDC(r3,0x34,(s8)(cVar6 + 1));
  }
clear_flags:
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  return 1;
}
