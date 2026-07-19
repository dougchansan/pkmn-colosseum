/**
 * @file fight_range_exact_8022DCB8.c
 * @brief Strict target-order fight island, 0x8022DCB8 - 0x8022F2F8.
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

/* fn_8022DCB8 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_80205134 fightOutPokemonGetWazaZokuseiDataId
#define fn_8012640C pokemonGetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
u8 fn_8022DCB8(u32 p1, u32 p2, u32 extra) {
    extern u16 fn_80207BF4();
    extern s32 fn_8011BEB4();
    extern u16 fn_80205134();
    extern u32 fn_8012640C();
    extern u32 fn_802062FC();
    extern u8  fn_802026E4();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    extern u8  lbl_80379714[];
    extern u8  lbl_80379715[];
    extern u8  lbl_80379752[];
    extern u8  lbl_80379753[];
    extern u8  lbl_80379783[];
    extern u8  lbl_80379784[];
    extern u8  fn_80201704();
    extern void fn_8011BBD8();
    extern u16 fn_80203B5C();

    u16 val26 = fn_80207BF4(p2);
    u16 val25 = (u16)fn_8011BEB4(0, extra, 7, 0);
    u16 val27 = fn_80205134(p1);
    u8 val31 = (u8)fn_8012640C(p1, 0, 0x111, 0);
    u8 result = 0;
    u32 fieldD9 = fn_8012640C(p1, 0, 0xD9, 0);
    u16 val4;

    if ((u8)fn_802062FC(p2) == 0) {
        return 0;
    }
    if ((u16)extra != 0 && (u16)extra != 0x165) {
        switch (val26) {
        case 0xa:
            if (val27 == 0xd && val25 != 0) {
                lbl_8047B610 = (val31 != 0) ? (u8*)&lbl_80379715 : (u8*)&lbl_80379714;
                result = 1;
            }
            break;
        case 0xb:
            if (val27 == 0xb && val25 != 0) {
                lbl_8047B610 = (val31 != 0) ? (u8*)&lbl_80379715 : (u8*)&lbl_80379714;
                result = 1;
            }
            break;
        case 0x12:
            if (val27 == 0xa && (u8)fn_802026E4(p2, 7) == 0) {
                if ((u8)fn_802025B8(p2, 0x3a) == 2) {
                    fn_8020248C(p2, 0x3a, 0);
                    lbl_80478D78[5] = 0;
                } else {
                    lbl_80478D78[5] = 1;
                }
                lbl_8047B610 = (val31 != 0) ? (u8*)&lbl_80379784 : (u8*)&lbl_80379783;
                result = 2;
            }
            break;
        }

        if (result == 1) {
            if ((u8)fn_80201704(p2) == 1) {
                if (val31 != 0) {
                    lbl_8047B610 = (u8*)&lbl_80379753;
                } else {
                    lbl_8047B610 = (u8*)&lbl_80379752;
                }
                goto done;
            }
            fn_8011BBD8(fieldD9, 0, 0x2d, 0, -fn_80203B5C(p2, 4));
        }
    }
done:
    return result;
}
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_80201704
#undef fn_802062FC
#undef fn_8012640C
#undef fn_80205134
#undef fn_8011BEB4
#undef fn_80207BF4

/* fn_8022DF08 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_8022DF08(u32 ctx)
{
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightFloorGetNowTenkouDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern u8 fightOutPokemonIsHpMantan();
    extern u16 fightOutPokemonMaxHpWaruValue();
    extern void wazaSetStatus();
    extern u8 fightOutPokemonIsJoutaiNormal();
    extern u32 pokemonGetJoutaiMsgId();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightOutPokemonResetSeqStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void pokemonSetStatus();
    extern void fn_80211B94();
    extern u8 lbl_8037967E[];
    extern u8 lbl_8037969F[];
    extern u8 lbl_803796B5[];
    extern u8 lbl_80379F58[];
    extern void* lbl_8047B62C;
    u32 pokemon;
    u32 weather;
    u32 ability;
    u32 move;

    pokemon = fightOutPokemonGetPokemonPtr();
    {
        u32 callResult = fightFloorGetNowTenkouDataId(0, 1);
        weather = (u8)callResult;
    }
    if (ctx == 0) {
        return;
    }
    if (fightOutPokemonCheckFightOut(ctx) == 0) {
        return;
    }

    ability = fightOutPokemonGetTokuseiDataId(ctx);
    fightFloorSetStatus(0, 0, 0x36, 0, ctx);
    move = pokemonGetStatus(ctx, 0, 0xD9, 0);

    switch ((u16)ability) {
    case 0x2C:
        if (weather == 2 && fightOutPokemonIsHpMantan(ctx) == 0) {
            u32 amount = (u16)fightOutPokemonMaxHpWaruValue(ctx, 0x10);
            wazaSetStatus(move, 0, 0x2D, 0, -(s32)amount);
            fn_80211B94(lbl_8047B62C, lbl_8037967E, 0);
        }
        break;

    case 0x3D:
        if (fightOutPokemonIsJoutaiNormal(ctx) == 0) {
            u16 random = fn_800E0C54();
            s32 divisor = 3;
            if (random % divisor == 0) {
                fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
                msgctrlSetValue(0xD, GSmsgGetGSchar(pokemonGetJoutaiMsgId(pokemon)));
                pokemonInitJoutai(pokemon);
                fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
                fightOutPokemonResetSeqStatus(ctx, 0);
                if (fightOutPokemonIsUseHensinBuff(ctx) == 1) {
                    fightOutPokemonSetHensinPokemonStatusId(ctx, 0x7C, 0, 0);
                }
                fn_80211B94(lbl_8047B62C, lbl_8037969F, 0);
            }
        }
        break;

    case 3: {
        s8 count = pokemonGetStatus(ctx, 0, 0xEA, 0);
        if (count < 12 && (u16)pokemonGetStatus(ctx, 0, 0xED, 0) != 2) {
            fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
            pokemonSetStatus(ctx, 0, 0xEA, 0, (s8)(count + 1));
            lbl_80379F58[0x160A4] = 0x11;
            lbl_80379F58[0x160A5] = 0;
            fn_80211B94(lbl_8047B62C, lbl_803796B5, 0);
        }
        break;
    }

    case 0x36: {
        u32 value = pokemonGetStatus(ctx, 0, 0xF9, 0);
        value = __cntlzw(value & 0xFF);
        pokemonSetStatus(ctx, 0, 0xF9, 0, (value & 0x1FE0) >> 5);
        break;
    }
    }
}

/* fn_8022E1C4 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_8022E1C4(void) {
    extern void fn_801F37B0();
    fn_801F37B0(0, fn_8022E1F8, 0, 0);
}
#undef fn_801F37B0

/* fn_8022E1F8 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011CB54 pokemonTokuseiDataBiosGetName
#define fn_8011CB6C pokemonTokuseiDataBiosGetPtr
#define fn_80132A38 msgctrlSetValue
#define fn_801F2598 fightFloorGetFightOutPokemonPtrRandom
#define fn_801F4C14 fightFloorSetStatus
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207BC0 fightOutPokemonSetTokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
u32 fn_8022E1F8(int r3)

{
    extern u32 fn_800FA280();
    extern void fn_8011CB54();
    extern void fn_8011CB6C();
    extern void fn_80132A38();
    extern u32 fn_801F2598();
    extern void fn_801F4C14();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_802037DC();
    extern s8 fn_802062FC();
    extern void fn_80207BC0();
    extern u32 fn_80207BF4();
    extern void fn_80211B94();
    extern void* lbl_8047B62C;
    extern u8 lbl_80379667[];
  u32 local;
  u32 sVar3;
  u8 cVar4;
  u32 uVar1;
  u32 uVar2;

  local = r3;
  sVar3 = fn_80207BF4();
  cVar4 = fn_802062FC(local);
  if (cVar4 == 0) {
    return 1;
  }
  if (((sVar3 & 0xffff) == 0x24) && (cVar4 = fn_802026E4(local,0x3c), cVar4 == 1)) {
    uVar1 = fn_801F2598(0,1,2,local);
    cVar4 = fn_802062FC();
    if (cVar4 == 1) {
      fn_80202810(local,0x3c);
      uVar2 = fn_80207BF4(uVar1);
      fn_80207BC0(local,uVar2);
      fn_801F4C14(0,0,0x4b,0,local);
      uVar1 = fn_802037DC(uVar1);
      fn_80132A38(0xd,uVar1);
      fn_8011CB6C(uVar2);
      fn_8011CB54();
      uVar1 = fn_800FA280();
      fn_80132A38(0xe,uVar1);
      fn_80211B94(lbl_8047B62C,(u32)lbl_80379667,0);
    }
  }
  return 1;
}
#undef fn_80207BF4
#undef fn_80207BC0
#undef fn_802062FC
#undef fn_802037DC
#undef fn_80202810
#undef fn_801F4C14
#undef fn_801F2598
#undef fn_80132A38
#undef fn_8011CB6C
#undef fn_8011CB54
#undef fn_800FA280

/* fn_8022E314 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_8022E314(u8 param) {
    extern void fn_801F37B0();
    u8 local = param;
    fn_801F37B0(0, fn_8022E34C, &local, 0);
}
#undef fn_801F37B0

/* fn_8022E34C */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
u32 fn_8022E34C(u32 r3, u32 r4, char* r5)

{
    extern void fn_801F4C14();
    extern u16 fn_80207BF4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 lbl_803795FE[];
    extern u8 lbl_803795F5[];
    extern void* lbl_8047B62C;
  u16 sVar1;
  u32 uVar3;
  void* context;
  u8 cVar2;

  context = (void*)r3;
  sVar1 = fn_80207BF4();
  cVar2 = fn_802062FC(context);
  if (cVar2 == 0) {
    return 1;
  }
  uVar3 = (u32)((*r5 == 1) ? lbl_803795F5 : lbl_803795FE);
  if ((sVar1 == 0x16) &&
      (cVar2 = fn_802026E4(context,0x3b), cVar2 == 1)) {
      fn_80202810(context,0x3b);
      fn_801F4C14(0,0,0x4b,0,context);
      fn_80211B94(lbl_8047B62C,uVar3,0);
  }
  return 1;
}
#undef fn_801F4C14
#undef fn_80202810
#undef fn_802062FC
#undef fn_80207BF4

/* fn_8022E410 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_801F4C14 fightFloorSetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
void fn_8022E410(int r3)

{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F37B0();
    extern void fn_801F4C14();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 fn_8022EC40();
    extern void* lbl_8047B62C;
    extern u8 lbl_80379F58[];
    extern u8 lbl_803795BB[];
    extern u8 lbl_8037959E[];
    extern u8 lbl_803795D8[];
    extern u8 lbl_803796D7[];
    extern void fn_8022EB9C();
  u8 cVar3;
  int uVar2;

  u8 local_18 [12];

  if ((r3 != 0) && (cVar3 = fn_802062FC(r3), cVar3 != 0)) {
    uVar2 = (int)(u16)fn_80207BF4(r3);
    switch (uVar2) {
    case 0x2:
      cVar3 = fn_801F2988(0,0x50);
      if (cVar3 != 2) {
        return;
      }
      fn_801F2934(0,0x50,0);
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_8037959E,0);
      return;
    case 0x2d:
      cVar3 = fn_801F2988(0,0x51);
      if (cVar3 == 2) {
        fn_801F2934(0,0x51,0);
        fn_801F4C14(0,0,0x4b,0,r3);
        fn_80211B94(lbl_8047B62C,(u32)lbl_803795BB,0);
      }
      break;
    case 0x46:
      cVar3 = fn_801F2988(0,0x4f);
      if (cVar3 != 2) {
        return;
      }
      fn_801F2934(0,0x4f,0);
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_803795D8,0);
      return;
    case 0x16:
      if ((int)fn_8012640C(r3,0,0x116,0) != 0) {
        return;
      }
      cVar3 = fn_802025B8(r3,0x3b);
      if (cVar3 == 2) {
        fn_8020248C(r3,0x3b,0);
      }
      fn_801254B4((void*)r3,0,0x116,0,1);
      return;
    case 0x24:
      if ((int)fn_8012640C(r3,0,0x117,0) != 0) {
        return;
      }
      cVar3 = fn_802025B8(r3,0x3c);
      if (cVar3 == 2) {
        fn_8020248C(r3,0x3c,0);
      }
      fn_801254B4((void*)r3,0,0x117,0,1);
      return;
    case 0x3b:
      cVar3 = fn_8022EC40(r3);
      if (cVar3 == 0) {
        return;
      }
      lbl_80379F58[0x1609b] = cVar3 - 1;
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_803796D7,0);
      return;
    case 0xd:
    case 0x4d:
      local_18[0] = 0;
      fn_801F37B0(0,(u32)fn_8022EB9C,local_18,0);
      break;
    default:
      return;
    }
  }
  return;
}
#undef fn_80207BF4
#undef fn_802062FC
#undef fn_801F4C14
#undef fn_801F37B0
#undef fn_801F2988
#undef fn_801F2934
#undef fn_8012640C
#undef fn_801254B4

/* fn_8022E6F0 */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_8011F910 pokemonAddDpFormPokemonDpFilterId
#define fn_8011FC74 pokemonIsDarkPokemon
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801C3430 battleGridUpdate
#define fn_801F000C fightMainWaitFrame
#define fn_801F2A7C fightFloorGetGcHeroFightTrainerPtr
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FE710 fightOutPokemonSetHensinFightPokemonStatusId
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80208404 fightOutPokemonDarkPokemonEffect
#define fn_802085C4 fightOutPokemonWazaEffect
#define fn_802086E8 fightOutPokemonFreeWazaEffect
#define fn_80208750 fightOutPokemonLoadWazaEffect
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#define fn_802653FC fightMenuFightOutPokemonRenewStatusMenu
void fn_8022E6F0(u32 r3, u8 r4)

{
    extern void _threadSwitch();
    extern void fn_8011F910();
    extern u8 fn_8011FC74();
    extern int fn_801906A0();
    extern void fn_801C3430();
    extern void battleGridReplaceTrainer();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern u32 fn_801DE418();
    extern void fn_801EF7C4();
    extern void fn_801F000C();
    extern u32 fn_801F2A7C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u8 fn_801F7B70();
    extern int fn_801FB1C0();
    extern void fn_801FBA24();
    extern void fn_801FE710();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u8 fn_80204A10();
    extern u32 fn_80205B8C();
    extern u8 fn_802062FC();
    extern void fn_80208404();
    extern void fn_80208554();
    extern void fn_802085C4();
    extern void fn_802086E8();
    extern void fn_80208750();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_802653FC();
  u8 cVar8;
  u32 uVar1;
  u16 uVar6;
  u8 cVar9;

  fn_801F54A4(0,0,0x14,0);
  if (r3 != 0) {
    cVar8 = fn_802062FC(r3);
    if (cVar8 != 0) {
      fn_80205B8C(r3);
      cVar8 = fn_8011FC74();
      if (cVar8 != 0) {
        u32 uVar4;
        u32 uVar5;
        uVar1 = (int)fn_8012640C(r3,0,0xd6,0);
        cVar8 = fn_80204A10(r3);
        if (cVar8 == 1) {
          cVar8 = (int)fn_8012640C(uVar1,0,0xd1,0);
          if (cVar8 == 0) {
            u32 uVar2;
            uVar2 = fn_80205B8C(r3);
            fn_8011F910(uVar2,0,0);
            cVar8 = fn_801FECD4(r3);
            if (cVar8 == 1) {
              fn_801FE7EC(r3,0xc5,0,0);
            }
            uVar6 = fn_801F54A4(0,0,0x14,0);
            fn_802653FC(r3,uVar6,1);
            fn_801254B4((void*)uVar1,0,0xd1,0,1);
            cVar8 = fn_801FECD4(r3);
            if (cVar8 == 1) {
              fn_801FE710(r3,0xd1,0);
            }
          }
        }
        else if (r4 == 0) {
          u32 uVar3;
          u32 uVar2;
          uVar2 = fn_801F2A7C(0);
          uVar3 = fn_801FB1C0(uVar2,0,0x4c,0);
          cVar8 = fn_801F7B70(uVar2);
          cVar9 = (int)fn_8012640C(uVar1,0,0xd1,0);
          if (((cVar9 == 0) && (cVar9 = fn_801F54A4(0,0,0x2b,0), cVar9 == 1)) &&
             (uVar3 != 0)) {
            u16 sVar7;
            u32 uVar10;
            sVar7 = fn_801F54A4(0,0,0xd,0);
            if (sVar7 != 0x11) {
              uVar10 = 0;
            }
            else {
              uVar10 = 3;
              fn_80208750(r3,1,1,0);
              fn_801FBA24(uVar2,0);
            }
            uVar4 = fn_801F54A4(0,0,0x36,0);
            fn_801F4C14(0,0,0x36,0,r3);
            uVar5 = fn_801DE418(0x32);
            fn_801DDD28(uVar5,0xa0,4,0);
            fn_80208404(r3,0,uVar10,0);
            battleGridReplaceTrainer(uVar3,uVar5);
            fn_801C3430();
            fn_801DA9E8(uVar5,0xa0,4);
            fn_801EF7C4(0);
            fn_801DA4E8(uVar5,1);
            while (1) {
              cVar9 = fn_801DA94C(uVar5,0xa0,4);
              if (cVar9 == 0) break;
              _threadSwitch();
            }
            battleGridReplaceTrainer(uVar5,uVar3);
            fn_801C3430();
            fn_801EF7C4(1);
            fn_801DA4E8(uVar5,0);
            fn_80208404(r3,0,uVar10,1);
            if (sVar7 == 0x11) {
              fn_802624CC(0x7729);
              fn_8026246C();
              fn_80208404(r3,0,uVar10,4);
              fn_802085C4(r3,1,1,0, (void*)0xffffffff);
              fn_80208554(r3,1,1,6);
              fn_801FBA24(uVar2,1);
              fn_801FBA24(uVar2,2);
              fn_80208404(r3,0,uVar10,1);
              fn_802624CC(0x772a);
              if (cVar8 == 0) {
                fn_8026246C();
                fn_801F000C(0x40);
                fn_802624CC(0x772b);
              }
              fn_8026246C();
              fn_80208404(r3,0,uVar10,4);
            }
            else {
              uVar3 = fn_801906A0(0x9a0);
              if (uVar3 == 0) {
                fn_802624CC(0x7717);
              }
              else {
                fn_802624CC(0x770a);
              }
              fn_80208404(r3,0,uVar10,2);
              fn_8026246C();
            }
            fn_801254B4((void*)uVar1,0,0xd1,0,1);
            cVar8 = fn_801FECD4(r3);
            if (cVar8 == 1) {
              fn_801FE710(r3,0xd1,0);
            }
            fn_801F4C14(0,0,0x36,0,uVar4);
            fn_80208404(r3,0,uVar10,3);
            if (sVar7 == 0x11) {
              fn_802086E8(r3,1,1);
              fn_801FBA24(uVar2,3);
            }
            fn_801DA8C4(uVar5,0xa0,4);
            fn_801DB100(uVar5);
          }
        }
      }
    }
  }
  return;
}
#undef fn_802653FC
#undef fn_802624CC
#undef fn_8026246C
#undef fn_80208750
#undef fn_802086E8
#undef fn_802085C4
#undef fn_80208404
#undef fn_802062FC
#undef fn_80205B8C
#undef fn_80204A10
#undef fn_801FECD4
#undef fn_801FE7EC
#undef fn_801FE710
#undef fn_801FB1C0
#undef fn_801F54A4
#undef fn_801F4C14
#undef fn_801F2A7C
#undef fn_801F000C
#undef fn_801C3430
#undef fn_8012640C
#undef fn_801254B4
#undef fn_8011FC74
#undef fn_8011F910

/* fn_8022EB9C */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation off
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
u32 fn_8022EB9C(u32 r3, u32 r4, u8* r5)

{
    extern u8 lbl_80379F58[];
    extern u8 lbl_803796D7[];
    extern void fn_801F4C14();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 fn_8022EC40();
    extern void* lbl_8047B62C;
  u8 cVar1;
  register u32 id = r3;
  u8* out = r5;

  cVar1 = fn_802062FC(id);
  if (cVar1 == 0) {
    return 1;
  }
  cVar1 = fn_8022EC40(id);
  if (cVar1 != 0) {
    lbl_80379F58[0x1609b] = cVar1 + -1;
    fn_801F4C14(0,0,0x4b,0,id);
    fn_80211B94(lbl_8047B62C,lbl_803796D7,0);
    if (out != (void *)0) {
      *out = 1;
    }
  }
  return 1;
}
#undef fn_801F4C14
#undef fn_802062FC

/* fn_8022EC40 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80203D3C figthOutPokemonGetPokemonDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_80207B5C fightOutPokemonSetZokuseiDataId
u8 fn_8022EC40(void* ctx) {
    extern u32 fn_802062FC();
    extern u16 fn_80203D3C();
    extern u16 fn_80207BF4();
    extern u8  fn_801F453C();
    extern u32 fn_80207AE0();
    extern u32 fn_80207B5C(void*, u8, u32);

    u16 x;
    u16 y;
    u8 kind;
    u8 count;

    if ((u8)fn_802062FC(ctx) == 0) {
        return 0;
    }
    x = fn_80203D3C(ctx);
    y = fn_80207BF4(ctx);
    if (x != 0x181 || y != 0x3b) {
        return 0;
    }

    kind = (u8)fn_801F453C(0, 1);
    count = 0;

    if (kind == 0 || kind == 3) {
        if ((u8)fn_80207AE0(ctx, 0) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0);
            }
            count = 1;
        }
    }
    if (kind == 1) {
        if ((u8)fn_80207AE0(ctx, 0xa) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0xa);
            }
            count = 2;
        }
    }
    if (kind == 2) {
        if ((u8)fn_80207AE0(ctx, 0xb) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0xb);
            }
            count = 3;
        }
    }
    if (kind == 4) {
        if ((u8)fn_80207AE0(ctx, 0xf) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0xf);
            }
            count = 4;
        }
    }
    return count;
}
#undef fn_80207B5C
#undef fn_80207AE0
#undef fn_801F453C
#undef fn_80207BF4
#undef fn_80203D3C
#undef fn_802062FC

/* fn_8022EDEC */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_8011FC74 pokemonIsDarkPokemon
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80208404 fightOutPokemonDarkPokemonEffect
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#define fn_802653FC fightMenuFightOutPokemonRenewStatusMenu
u32 fn_8022EDEC(u32 r3, u8 r4)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011F6D8();
    extern u8 fn_8011FC74();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_80204A10();
    extern void fn_80205B8C();
    extern u8 fn_802062FC();
    extern void fn_802080A8();
    extern void fn_80208404();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_8026532C();
    extern void fn_802653FC();
    extern void fn_80265598();
  u16 uVar4;
  u32 uVar1;
  u8 cVar6;
  u32 uVar2;
  u32 sVar5;
  int uVar3;
  int randomResult;

  uVar4 = fn_801F54A4(0,0,0x14,0);
  if (r3 == 0) {
    uVar1 = 0;
  }
  else {
    cVar6 = fn_802062FC(r3);
    if (cVar6 == 0) {
      uVar1 = 0;
    }
    else {
      cVar6 = fn_80204A10(r3);
      if (cVar6 == 0) {
        uVar1 = 0;
      }
      else {
        cVar6 = fn_801F54A4(0,0,0x31,0);
        if (cVar6 == 0) {
          uVar1 = 0;
        }
        else {
          fn_80205B8C(r3);
          cVar6 = fn_8011FC74();
          if (cVar6 == 0) {
            uVar1 = 0;
          }
          else {
            fn_8012640C(r3,0,0xee,0);
            if (r4 == 0) {
              cVar6 = fn_802026E4(r3,0x3e);
              if (cVar6 == 1) {
                return 0;
              }
              cVar6 = fn_802026E4(r3,8);
              if (cVar6 == 1) {
                return 0;
              }
              fn_80205B8C(r3);
              uVar2 = fn_8011F6D8();
              sVar5 = fn_80205184((void*)r3);
              uVar3 = fn_800E0C54();
              if (((int)((uVar3 & 0xffff) % 100) < (int)(uVar2 & 0xff)) &&
                  ((sVar5 & 0xffff) == 0x164)) {
                fn_80208404(r3,1,1,0);
                uVar1 = fn_801F54A4(0,0,0x36,0);
                fn_801F4C14(0,0,0x36,0,r3);
                cVar6 = fn_802025B8(r3,0x3e);
                if (cVar6 == 2) {
                  fn_8020248C(r3,0x3e,0);
                }
                cVar6 = fn_801FECD4(r3);
                if (cVar6 == 1) {
                  fn_801FE7EC(r3,200,0,0);
                }
                fn_80208404(r3,1,1,1);
                fn_802653FC(r3,uVar4,1);
                fn_802624CC(0x771b);
                fn_80208404(r3,1,1,2);
                fn_8026246C();
                fn_8026532C(r3,uVar4,0);
                fn_801F4C14(0,0,0x36,0,uVar1);
                fn_80208404(r3,1,1,3);
              }
              else {
                return 0;
              }
            }
            else if (r4 == 1) {
              cVar6 = fn_802026E4(r3,0x3e);
              if (cVar6 == 0) {
                return 0;
              }
              fn_80208404(r3,0,2,0);
              uVar1 = fn_801F54A4(0,0,0x36,0);
              fn_801F4C14(0,0,0x36,0,r3);
              fn_80208404(r3,0,2,1);
              fn_80265598(r3,uVar4,1);
              fn_802624CC(0x771c);
              fn_80208404(r3,0,2,2);
              fn_8026246C();
              fn_8026532C(r3,uVar4,0);
              fn_801F4C14(0,0,0x36,0,uVar1);
              fn_80208404(r3,0,2,3);
            }
            else if (r4 == 2) {
              cVar6 = fn_802026E4(r3,0x3e);
              if (cVar6 == 0) {
                return 0;
              }
              randomResult = (u16)fn_800E0C54();
              if ((randomResult % 0x100) != 0) {
                return 0;
              }
              if (fn_802026E4(r3,8) == 1) {
                __memcpy((void*)r3,(void*)r3,0);
              }
              fn_802080A8(r3,1,1,0,0);
              uVar1 = fn_801F54A4(0,0,0x36,0);
              fn_801F4C14(0,0,0x36,0,r3);
              fn_80202810(r3,0x3e);
              fn_80202810(r3,8);
              fn_80202810(r3,0x17);
              cVar6 = fn_801FECD4(r3);
              if (cVar6 == 1) {
                fn_801FE7EC(r3,200,0,0);
              }
              cVar6 = fn_801FECD4(r3);
              if (cVar6 == 1) {
                fn_801FE7EC(r3,0x7c,0,0);
              }
              fn_802080A8(r3,1,1,0x771e,1);
              fn_802080A8(r3,1,1,0,2);
              fn_8026246C();
              fn_8026532C(r3,uVar4,0);
              fn_801F4C14(0,0,0x36,0,uVar1);
              fn_802080A8(r3,1,1,0,3);
            }
            uVar1 = 1;
          }
        }
      }
    }
  }
  return uVar1;
}
#undef fn_802653FC
#undef fn_802624CC
#undef fn_8026246C
#undef fn_80208404
#undef fn_802062FC
#undef fn_80205B8C
#undef fn_80205184
#undef fn_80204A10
#undef fn_80202810
#undef fn_801FECD4
#undef fn_801FE7EC
#undef fn_801F54A4
#undef fn_801F4C14
#undef fn_8012640C
#undef fn_8011FC74
