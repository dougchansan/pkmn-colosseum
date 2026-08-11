/**
 * @file fight_seq.c
 * @brief fightSeq module (head) -- split from colosseum_event.c (the fight
 *        engine bucket, 0x80202810-0x80211A00), address range
 *        0x80211170-0x80211A00, 7 fns.
 *
 * fightSeq's normal-waza damage calculator (hit chance / damage / crit
 * / normal-damage combined into one function here vs 4 in XD), plus
 * fightSeqInit/Post and the EffectAnimFlag accessors. Corresponds to
 * XD's fightSeq head (0x8020D858-0x8020E1A0).
 */

#include "game/colosseum.h"
#include "crt/string.h"
#include "game/fight_action.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

#if !defined(PR424_FIGHT_SEQ_SPLIT) || \
    defined(PR424_FIGHT_SEQ_80211170_802117FC)
/* Address: 0x80211170 | Size: 0x68c | Ghidra import */
u32
fightSeqGetNromalWazaDamage(u32 r3, u32 r4, u32 r5, u32 r6, u8 r7, u8 r8, void *r9, void *r10)

{
    extern u16 fn_800E0C54();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern void fn_801FCEC4();
    extern int fn_802026E4();
    extern u16 figthOutPokemonGetPokemonDataId();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fightWazaIsHit();
    extern void fightWazaCreate();
    extern void fightWazaInit();
    extern u8 fightWazaCriticalDataBiosGetBunbo();
    extern void fightWazaCriticalDataBiosGetPtr();
    extern void fightWazaBiosCopy();
    extern void fn_802271E0();
    extern void fn_802274F0();
    extern int fn_80232110();
    extern u32 lbl_80478D60;
    extern u8 lbl_80478D78[8];
    extern u32 lbl_8047B610;
    extern u32 lbl_8047B618;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar15;
  u32 uVar5;
  u32 uVar6;
  u16 sVar16;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  u32 uVar10;
  u32 uVar11;
  u32 uVar12;
  u8 cVar18;
  u32 uVar13;
  u32 uVar14;
  u16 uVar17;
  u32 uVar21;
  u32 uVar22;

  u32 uVar23;
  int iVar24;
  int iVar25;
  u8 local_ec8 [8];
  u8 auStack_ec0 [172];
  u8 auStack_734 [1760];
  u8 auStack_e14 [1760];
  u32 local_54;
  u32 local_50;
  
  local_54 = fightFloorGetStatus(0,0,0x36,0);
  uVar2 = fightFloorGetStatus(0,0,0x42,0);
  local_50 = lbl_8047B610;
  uVar23 = lbl_8047B618;
  for (iVar25 = 0; iVar25 < 8; iVar25++) {
    local_ec8[iVar25] = lbl_80478D78[iVar25];
  }
  fn_801FCEC4(auStack_734,r5);
  fn_801FCEC4(auStack_e14,r6);
  fightFloorSetStatus(0,0,0x36,0,r5);
  fightFloorSetStatus(0,0,0x42,0,r6);
  uVar3 = (int)pokemonGetStatus(r5,0,0xd9,0);
  fightWazaBiosCopy(auStack_ec0,uVar3);
  fightWazaInit(uVar3);
  fightWazaCreate(uVar3,0,r4,0,0);
  lbl_8047B618 = 0;
  for (iVar25 = 0; iVar25 < 8; iVar25++) {
    lbl_80478D78[iVar25] = 0;
  }
  if (r9 != (void *)0) {
    ((void (*)())r9)(r3,r4,r5,r6);
  }
  if (r7 != 1) goto LAB_0020e58c;
  uVar4 = fightTargetGetPtrAsNowFightType(0x11,0);
  fightOutPokemonGetTokuseiDataId();
  uVar15 = fightOutPokemonGetSoubiItemSoubiDataId(uVar4);
  uVar5 = (int)pokemonGetStatus(uVar4,0,0xd9,0);
  uVar6 = fightOutPokemonGetUseWazaDataId(uVar4);
  sVar16 = figthOutPokemonGetPokemonDataId(uVar4);
  uVar7 = fightTargetGetPtrAsNowFightType(0x12,0);
  iVar25 = 0;
  if (((uVar15 & 0xffff) == 0x3f) && (sVar16 == 0x71)) {
    iVar25 = 1;
  }
  iVar24 = 0;
  if (((uVar15 & 0xffff) == 0x42) && (sVar16 == 0x53)) {
    iVar24 = 1;
  }
  uVar8 = fn_802026E4(uVar4,0xf);
  uVar8 = __cntlzw(1 - (uVar8 & 0xff));
  uVar8 = uVar8 >> 4 & 0xffffffe;
  uVar9 = wazaGetStatus(0,uVar6,9,0);
  uVar9 = __cntlzw(0x2b - (uVar9 & 0xffff));
  uVar9 = uVar9 >> 5;
  uVar10 = wazaGetStatus(0,uVar6,9,0);
  uVar10 = __cntlzw(0x4b - (uVar10 & 0xffff));
  uVar8 = uVar8 + (uVar10 >> 5);
  uVar11 = wazaGetStatus(0,uVar6,9,0);
  uVar11 = __cntlzw(200 - (uVar11 & 0xffff));
  uVar8 = uVar8 + (uVar11 >> 5);
  uVar12 = wazaGetStatus(0,uVar6,9,0);
  uVar21 = __cntlzw(0xd1 - (uVar12 & 0xffff));
  uVar22 = (int)lbl_80478D60 - 1;
  uVar12 = __cntlzw(0x29 - (uVar15 & 0xffff));
  uVar8 = uVar8 + (uVar21 >> 5);
  uVar8 = uVar8 + (uVar12 >> 5);
  uVar8 = uVar8 + iVar25 * 2;
  uVar8 = uVar8 + iVar24 * 2;
  uVar8 = (uVar9 + uVar8) & 0xffff;
  if (uVar8 > uVar22) {
    uVar8 = uVar22 & 0xffff;
  }
  sVar16 = fightOutPokemonGetTokuseiDataId(uVar7);
  if ((sVar16 != 4) &&
      (sVar16 = fightOutPokemonGetTokuseiDataId(uVar7), sVar16 != 0x4b) &&
      (cVar18 = fightFloorGetStatus(0,0,0x29,0), cVar18 == 1)) {
    fightWazaCriticalDataBiosGetPtr(uVar8);
    uVar8 = fightWazaCriticalDataBiosGetBunbo();
    uVar9 = fn_800E0C54();
    if (((s32)(uVar9 & 0xffff) % (s32)(uVar8 & 0xff) == 0) ||
        (((u8)fn_802026E4(uVar4,0x3e) == 1) &&
         ((uVar6 & 0xffff) == 0x164) &&
         ((s32)(fn_800E0C54() & 0xffff) % 100 < 0x5a))) {
      wazaSetStatus(uVar5,0,0x2b,0,2);
    }
    else {
      wazaSetStatus(uVar5,0,0x2b,0,1);
    }
  }
  else {
    wazaSetStatus(uVar5,0,0x2b,0,1);
  }
  lbl_8047B610 = (int)lbl_8047B610 + 1;
LAB_0020e58c:
  uVar4 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar5 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar7 = fightTargetGetPtrAsNowFightType(2,uVar5);
  uVar13 = fightOutPokemonGetUseWazaDataId(uVar4);
  uVar14 = (int)pokemonGetStatus(uVar4,0,0xd9,0);
  uVar17 = wazaGetStatus(uVar14,0,0x2f,0);
  sVar16 = wazaGetStatus(uVar14,0,0x30,0);
  iVar25 = fn_80232110(uVar4,uVar5,uVar7,uVar13,uVar17,sVar16);
  uVar6 = wazaGetStatus(uVar14,0,0x2b,0);
  iVar25 = iVar25 * (uVar6 & 0xff);
  uVar8 = wazaGetStatus(uVar14,0,0x2c,0);
  iVar25 = iVar25 * (uVar8 & 0xff);
  cVar18 = fn_802026E4(uVar4,0x24);
  if ((cVar18 == 1) && (sVar16 == 0xd)) {
    iVar25 = iVar25 * 2;
  }
  cVar18 = fn_802026E4(uVar4,0x32);
  if (cVar18 == 1) {
    iVar25 = (iVar25 * 0xf) / 10;
  }
  wazaSetStatus(uVar14,0,0x2d,0,iVar25);
  lbl_8047B610 = (int)lbl_8047B610 + 1;
  fn_802274F0(1,1,1,0);
  if (r8 == 1) {
    fn_802271E0(1,1);
    lbl_8047B610 = (int)lbl_8047B610 + 1;
  }
  if (r10 != (void *)0) {
    ((void (*)())r10)(r3,r4,r5,r6);
  }
  cVar18 = fightWazaIsHit(uVar3);
  if (cVar18 == 1) {
    uVar4 = wazaGetStatus(uVar3,0,0x2d,0);
  }
  else {
    uVar4 = 0;
  }
  fightFloorSetStatus(0,0,0x36,0,local_54);
  fightFloorSetStatus(0,0,0x42,0,uVar2);
  fightWazaBiosCopy(uVar3,auStack_ec0);
  lbl_8047B610 = local_50;
  lbl_8047B618 = uVar23;
  for (iVar25 = 0; iVar25 < 8; iVar25++) {
    lbl_80478D78[iVar25] = local_ec8[iVar25];
  }
  fn_801FCEC4(r5,auStack_734);
  fn_801FCEC4(r6,auStack_e14);
  return uVar4;
}
#endif

#if !defined(PR424_FIGHT_SEQ_SPLIT)
/* 0x802117FC | size: 0x14 | tiny */
u32 fightSeqGetEffectAminFlag(void) {
    extern u32 lbl_8047B618;
    return !(lbl_8047B618 & 0x80);
}
#endif

#if !defined(PR424_FIGHT_SEQ_SPLIT) || \
    defined(PR424_FIGHT_SEQ_80211810_802119D4)
/* fightSeqSetEffectAminFlag | Size: 0x20 | Set/clear bit 0x80 in flags */
void fightSeqSetEffectAminFlag(u8 enable) {
    extern u32 lbl_8047B618;
    u32 val;
    u32 result;
    enable = (u8)enable;
    val = lbl_8047B618;
    result = val | 0x80u;
    if (enable == 1) {
        result = val & ~0x80u;
    }
    lbl_8047B618 = result;
}

/* 0x80211830 | size: 0xCC */
void fightSeqInit(void) {
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B62C;
    extern void fightFloorLoopValidFightOutPokemon();
    extern void* fightFloorGetValidFightSidePtr();
    extern void fightSideInitJoutaiDataId();
    extern s32 _fightSeqInitSubFightOutPokemon__FPvUsPv();
    extern s32 fn_80213558();
    extern s32 fn_802136A4();
    u8 localBuf[0x10];
    u16 i;
    void* slotData;

    localBuf[0] = 0;
    fightFloorLoopValidFightOutPokemon(0, (u32)fn_80213558, &localBuf[0], 0);
    for (i = 0; i < 2; i++) {
        slotData = fightFloorGetValidFightSidePtr(0, i);
        if (slotData != NULL) {
            fightSideInitJoutaiDataId(slotData, 0x4D);
        }
    }
    fightFloorLoopValidFightOutPokemon(0, (u32)fn_802136A4, 0, 0);
    memset(lbl_80478D78, 0, 8);
    lbl_8047B62C = 0;
    fightFloorLoopValidFightOutPokemon(0, (u32)_fightSeqInitSubFightOutPokemon__FPvUsPv, 0, 0);
}

/* _fightSeqInitSubFightOutPokemon__FPvUsPv | Size: 0x4C | Check state and optionally trigger event 0x11 */
#pragma push
#pragma peephole on
s32 _fightSeqInitSubFightOutPokemon__FPvUsPv(void* ctx) {
    extern u8 fightOutPokemonCheckFightOut(void);
    extern void fightOutPokemonWriteJoutaiDataId(void* ctx, u32 eventId);
    if (!fightOutPokemonCheckFightOut()) {
        return 1;
    }
    fightOutPokemonWriteJoutaiDataId(ctx, 0x11);
    return 1;
}
#pragma pop

/* Address: 0x80211948 | Size: 0x8c | Ghidra import */
void fightSeqFightActionCreateAndFlowFifo(
    void* motoAction, void* actorTarget, u32 kind, u32 buff,
    FightActionData* actionData, void* buffData)
{
  s32 result;
  FightAction action;
  
  result = fightActionCreate(&action, motoAction, actorTarget, kind, buff,
                             actionData);
  switch ((u32)result & 0xff) {
  case 1u:
    fightActionBiosSetBuffDataPtr(&action, buffData);
    result = 1;
    break;
  default:
    break;
  }
  switch ((u32)result & 0xff) {
  case 1u:
    fightActionFlowFifo(&action);
    break;
  default:
    break;
  }
  return;
}
#endif

#if !defined(PR424_FIGHT_SEQ_SPLIT)
/* fightSeqPost | Size: 0x2C | Clear bit 20 in flags and call fn_80213270 */
void fightSeqPost(void) {
    extern u32 lbl_8047B618;
    extern void fn_80213270(void);
    lbl_8047B618 &= ~0x00100000u;
    fn_80213270();
}
#endif

#if !defined(PR424_FIGHT_SEQ_SPLIT)
/* Address: 0x80211A00 | Size: 0x78 | Ghidra import */
void fn_80211A00(void)

{
    extern u32 DAT_8038ff5a;
    extern u32 DAT_8038fff9;
    extern void fn_801DA7AC();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7C;
    extern u8 lbl_8047B625;
    extern void fn_80211A78();
    extern void fn_8022FE20();

  fightFloorLoopValidFightOutPokemon(0,(u32)fn_80211A78,0,1);
  fightFloorLoopValidFightOutPokemon(0,(u32)fn_8022FE20,0,1);
  lbl_80478D7B = 0;
  lbl_8047B625 = 0;
  lbl_80478D7C = 0;
  DAT_8038ff5a = 0;
  DAT_8038fff9 = 0;
  fn_801DA7AC();
  return;
}
/* 0x80211A78 | size: 0x11C */
#pragma push
#pragma peephole on
u32 fn_80211A78(void* ctx) {
    extern u8 lbl_80375D30[];
    extern u8 lbl_803791FE[];
    extern void* lbl_8047B62C;
    extern u8 fn_802026E4();
    extern u16 fightOutPokemonGetMotoWazaDataId();
    extern u8 fightOutPokemonCheckFightOut();
    u8 localBuf[0x30];
    void* feData;
    FightActionData* d920val;
    u8 result;

    if ((u8)fightOutPokemonCheckFightOut(ctx) == 0) { return 1; }
    feData = pokemonGetStatus(ctx, 0, 0xFE, 0);
    if (feData == NULL) { return 1; }
    if ((u8)fightActionCheckValid(feData) == 0) { return 1; }
    if (fightOutPokemonGetMotoWazaDataId(ctx) != 0x108) { goto done; }
    if ((u8)fn_802026E4(ctx, 8) != 0) { goto done; }
    if ((u8)(u32)pokemonGetStatus(ctx, 0, 0xF9, 0) != 0) { goto done; }
    d920val = fightActionBiosGetFightActionDataPtr((FightAction*)lbl_8047B62C);
    result = fightActionCreate((FightAction*)localBuf, d920val, ctx, 0xC, 0,
                               (FightActionData*)lbl_80375D30);
    if (result == 1) {
        fightActionBiosSetBuffDataPtr((FightAction*)localBuf, lbl_803791FE);
        result = 1;
    }
    if (result == 1) {
        fightActionFlowFifo(localBuf);
    }
done:
    return 1;
}
#pragma pop

/* Address: 0x80211B94 | Size: 0x284 | Ghidra import */
void fn_80211B94(void)

{
    u32 r3;
    u32 r4;
    char r5;

    extern u32 DAT_80378798;
    extern u32 DAT_8038ff5a;
    extern u32 DAT_8038fff9;
    extern void wazaSetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7C;
    extern u32 lbl_8047B610;
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B625;
    extern u32 lbl_8047B62C;
  u8 uVar1;
  u8 uVar2;
  u32 uVar3;

  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;
  u8 local_38;
  u8 local_37 [15];
  
  uVar4 = lbl_8047B610;
  uVar5 = lbl_8047B62C;
  lbl_8047B610 = r4;
  uVar1 = lbl_8047B614;
  if (r5 == 0) {
    lbl_8047B614 = 0;
  }
  lbl_8047B62C = r3;
  do {
    while (((int (*)())*(u32 *)((u32)*(u8*)lbl_8047B610 * 4 + -0x7fd85ff4))(),
          r5 == 0) {
      if ((lbl_8047B614 == 1) || (lbl_8047B614 == 2))
      goto LAB_0020eddc;
    }
    if (lbl_8047B614 == 1) {
      fightFloorLoopValidFightOutPokemon(0,0x8023011c,0,0);
      uVar6 = lbl_8047B62C;
      uVar2 = lbl_8047B614;
      uVar7 = lbl_8047B610;
      lbl_8047B614 = 0;
      lbl_8047B610 = (u32)(u8 *)&DAT_80378798;
      lbl_8047B62C = uVar6;
      do {
        ((int (*)())*(u32 *)((u32)*(u8*)lbl_8047B610 * 4 + -0x7fd85ff4))();
        if (lbl_8047B614 == 1) break;
      } while ((char)lbl_8047B614 != 2);
      lbl_8047B62C = uVar6;
      lbl_8047B614 = uVar2;
      lbl_8047B610 = uVar7;
      local_37[0] = 1;
      fightFloorLoopValidFightOutPokemon(0,0x8022e34c,local_37,0);
      fightFloorLoopValidFightOutPokemon(0,0x8022e1f8,0,0);
      fightFloorLoopValidFightOutPokemon(0,0x80230088,0,0);
      local_38 = 0;
      fightFloorLoopValidFightOutPokemon(0,0x8022eb9c,&local_38,0);
      lbl_8047B614 = 2;
    }
  } while ((char)lbl_8047B614 != 2);
  uVar6 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar7 = (int)pokemonGetStatus(uVar6,0,0xd9,0);
  fightFloorLoopValidFightOutPokemon(0,0x802136a4,0,0);
  lbl_80478D7B = 0;
  lbl_8047B618 = lbl_8047B618 & 0xf1e892af;
  lbl_8047B625 = 0;
  lbl_80478D7C = 0;
  DAT_8038ff5a = 0;
  DAT_8038fff9 = 0;
  pokemonSetStatus(uVar6,0,0xf3,0,0);
  pokemonSetStatus(uVar6,0,0xf4,0,9);
  wazaSetStatus(uVar7,0,0x2d,0,0);
LAB_0020eddc:
  if (r5 != 0) {
    uVar3 = lbl_8047B618;
    lbl_8047B618 = uVar3 & 0xfffffdff;
    lbl_8047B618 = uVar3 & 0xfff7fdff;
  }
  lbl_8047B62C = uVar5;
  lbl_8047B614 = uVar1;
  lbl_8047B610 = uVar4;
  return;
}
/* Address: 0x80211E18 | Size: 0x8ac | Ghidra import */

void fn_80211E18(u32 r3,u32 r4)

{
    extern u32 DAT_80375e24;
    extern u32 DAT_8038ff76;
    extern u32 DAT_8038fffc;
    extern u32 _DAT_80279e80;
    extern u32 _DAT_80279e84;
    extern u32 _DAT_80279e88;
    extern u32 _DAT_80279e8c;
    extern u32 _DAT_80279e94;
    extern u32 _DAT_80375e24;
    extern u32 _DAT_80375e44;
    extern u32 GSmsgGetGSchar();
    extern void heroItemDecItemDataId();
    extern void msgctrlSetValue();
    extern s8 itemParamGetHPUp();
    extern s8 itemParamGetConfuseFlag();
    extern s8 itemParamGetParalyzeFlag();
    extern s8 itemParamGetFreezeFlag();
    extern s8 itemParamGetBurnFlag();
    extern s8 itemParamGetPoisonFlag();
    extern s8 itemParamGetSleepFlag();
    extern s8 itemParamGetGuardFlag();
    extern s8 itemParamGetSpAttackUp();
    extern s8 itemParamGetHitUp();
    extern s8 itemParamGetQuickUp();
    extern s8 itemParamGetDefenceUp();
    extern s8 itemParamGetAttackUp();
    extern s8 itemParamGetCriticalFlag();
    extern u32 itemParamGetPtr();
    extern void itemDataBiosGetItemEffectParam();
    extern void itemDataBiosGetPtr();
    extern void fn_801DA7AC();
    extern void fn_801EF8F4();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 fn_801F8000();
    extern u32 fightTrainerGetNamePtr();
    extern u32 fightTrainerGetStatus();
    extern int fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fn_80211B94();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern void fn_80265598();
    extern u8 lbl_80478D7D;
    extern u8 lbl_8047B614;
  u8 bVar1;
  s8 cVar2;
  u16 uVar9;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  int iVar7;
  short sVar10;
  s8 cVar12;
  u32 uVar8;
  s8 cVar13;
  u16 uVar11;

  u32 uVar14;
  u32 uVar15;
  
  uVar9 = fightFloorGetStatus(0,0,0x14,0);
  uVar3 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar5 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar3);
  uVar6 = fightTrainerGetStatus(uVar5,0,0x44,0);
  iVar7 = (int)pokemonGetStatus(uVar3,0,0xe5,0);
  if (iVar7 != 0) {
    itemGetStatus(iVar7,0,0x1f,0);
    sVar10 = itemGetStatus(iVar7,0,0x20,0);
    cVar12 = itemGetStatus(iVar7,0,0x21,0);
    itemDataBiosGetPtr(r4);
    itemDataBiosGetItemEffectParam();
    uVar8 = itemParamGetPtr();
    cVar2 = DAT_8038fffc;
    bVar1 = DAT_8038ff76;
    cVar13 = fn_802026E4(uVar3,0x2e);
    if (cVar13 == 1) {
      fightOutPokemonWriteJoutaiDataId(uVar3,0x2e);
    }
    cVar13 = fn_802026E4(uVar3,0x15);
    if (cVar13 == 1) {
      fightOutPokemonWriteJoutaiDataId(uVar3,0x15);
    }
    cVar13 = fn_802026E4(uVar3,0x28);
    if (cVar13 == 1) {
      fightOutPokemonWriteJoutaiDataId(uVar3,0x28);
    }
    uVar14 = _DAT_80375e24;
    if (cVar12 != 1) {
      cVar13 = itemGetStatus(0,r4,2,0);
      if (cVar13 == 1) {
        uVar14 = *(u32 *)((r4 & 0xffff) * 4 + -0x7fc8a210);
      }
      else {
        uVar14 = _DAT_80375e44;
        if (((r4 & 0xffff) != 0x50) && ((r4 & 0xffff) != 0x51)) {
          itemDataBiosGetPtr(r4);
          itemDataBiosGetItemEffectParam();
          iVar7 = itemParamGetPtr();
          if (iVar7 == 0) {
            uVar15 = 7;
          }
          else if ((r4 & 0xffff) == 0x13) {
            uVar15 = 1;
          }
          else {
            cVar13 = itemParamGetHPUp();
            if (cVar13 == 0) {
              cVar13 = itemParamGetSleepFlag(iVar7);
              if ((((cVar13 == 1) || (cVar13 = itemParamGetPoisonFlag(iVar7), cVar13 == 1)) ||
                  (cVar13 = itemParamGetBurnFlag(iVar7), cVar13 == 1)) ||
                 (((cVar13 = itemParamGetFreezeFlag(iVar7), cVar13 == 1 ||
                   (cVar13 = itemParamGetParalyzeFlag(iVar7), cVar13 == 1)) ||
                  (cVar13 = itemParamGetConfuseFlag(iVar7), cVar13 == 1)))) {
                uVar15 = 3;
              }
              else {
                cVar13 = itemParamGetCriticalFlag(iVar7);
                if (cVar13 == 1) {
                  uVar15 = 4;
                }
                else {
                  cVar13 = itemParamGetAttackUp(iVar7);
                  if (((cVar13 == 0) && (cVar13 = itemParamGetDefenceUp(iVar7), cVar13 == 0)) &&
                     ((cVar13 = itemParamGetQuickUp(iVar7), cVar13 == 0 &&
                      ((cVar13 = itemParamGetHitUp(iVar7), cVar13 == 0 &&
                       (cVar13 = itemParamGetSpAttackUp(iVar7), cVar13 == 0)))))) {
                    cVar13 = itemParamGetGuardFlag(iVar7);
                    if (cVar13 == 1) {
                      uVar15 = 6;
                    }
                    else {
                      uVar15 = 7;
                    }
                  }
                  else {
                    uVar15 = 5;
                  }
                }
              }
            }
            else {
              uVar15 = 2;
            }
          }
          uVar14 = _DAT_80375e24;
          if (uVar15 != 7) {
            fightFloorSetStatus(0,0,0x4b,0,uVar3);
            lbl_80478D7D = 0;
            if (uVar15 == 4) {
              lbl_80478D7D = 5;
            }
            else if (uVar15 < 4) {
              if (2 < uVar15) {
                cVar13 = itemParamGetSleepFlag(uVar8);
                if ((((cVar13 == 1) && (cVar13 = itemParamGetPoisonFlag(uVar8), cVar13 == 1)) &&
                    (cVar13 = itemParamGetBurnFlag(uVar8), cVar13 == 1)) &&
                   ((cVar13 = itemParamGetFreezeFlag(uVar8), cVar13 == 1 &&
                    (cVar13 = itemParamGetParalyzeFlag(uVar8), cVar13 == 1)))) {
                  cVar13 = fn_802026E4(uVar3,8);
                  if (cVar13 == 1) {
                    lbl_80478D7D = 5;
                  }
                  else {
                    cVar13 = fn_802026E4(uVar3,3);
                    if (cVar13 == 1) {
                      lbl_80478D7D = 4;
                    }
                    else {
                      cVar13 = fn_802026E4(uVar3,4);
                      if (cVar13 == 1) {
                        lbl_80478D7D = 4;
                      }
                      else {
                        cVar13 = fn_802026E4(uVar3,6);
                        if (cVar13 == 1) {
                          lbl_80478D7D = 3;
                        }
                        else {
                          cVar13 = fn_802026E4(uVar3,7);
                          if (cVar13 == 1) {
                            lbl_80478D7D = 2;
                          }
                          else {
                            cVar13 = fn_802026E4(uVar3,5);
                            if (cVar13 == 1) {
                              lbl_80478D7D = 1;
                            }
                          }
                        }
                      }
                    }
                  }
                }
                else {
                  cVar13 = itemParamGetSleepFlag(uVar8);
                  if (cVar13 == 1) {
                    lbl_80478D7D = 5;
                  }
                  else {
                    cVar13 = itemParamGetPoisonFlag(uVar8);
                    if (cVar13 == 1) {
                      lbl_80478D7D = 4;
                    }
                    else {
                      cVar13 = itemParamGetBurnFlag(uVar8);
                      if (cVar13 == 1) {
                        lbl_80478D7D = 3;
                      }
                      else {
                        cVar13 = itemParamGetFreezeFlag(uVar8);
                        if (cVar13 == 1) {
                          lbl_80478D7D = 2;
                        }
                        else {
                          cVar13 = itemParamGetParalyzeFlag(uVar8);
                          if (cVar13 == 1) {
                            lbl_80478D7D = 1;
                          }
                          else {
                            cVar13 = itemParamGetConfuseFlag(uVar8);
                            if (cVar13 == 1) {
                              lbl_80478D7D = 0;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            else if ((uVar15 != 6) && (uVar15 < 6)) {
              lbl_80478D7D = 4;
              uVar11 = itemParamGetAttackUp(uVar8);
              uVar11 = uVar11 & 0xff;
              if (uVar11 == 0) {
                uVar11 = itemParamGetDefenceUp(uVar8);
                uVar11 = uVar11 & 0xff;
                if (uVar11 == 0) {
                  uVar11 = itemParamGetQuickUp(uVar8);
                  uVar11 = uVar11 & 0xff;
                  if (uVar11 == 0) {
                    uVar11 = itemParamGetHitUp(uVar8);
                    uVar11 = uVar11 & 0xff;
                    if (uVar11 == 0) {
                      uVar11 = itemParamGetSpAttackUp(uVar8);
                      uVar11 = uVar11 & 0xff;
                      if (uVar11 != 0) {
                        uVar3 = GSmsgGetGSchar(_DAT_80279e8c);
                        msgctrlSetValue(0xd,uVar3);
                        DAT_8038ff76 = 4;
                      }
                    }
                    else {
                      uVar3 = GSmsgGetGSchar(_DAT_80279e94);
                      msgctrlSetValue(0xd,uVar3);
                      DAT_8038ff76 = 6;
                    }
                  }
                  else {
                    uVar3 = GSmsgGetGSchar(_DAT_80279e88);
                    msgctrlSetValue(0xd,uVar3);
                    DAT_8038ff76 = 3;
                  }
                }
                else {
                  uVar3 = GSmsgGetGSchar(_DAT_80279e84);
                  msgctrlSetValue(0xd,uVar3);
                  DAT_8038ff76 = 2;
                }
              }
              else {
                uVar3 = GSmsgGetGSchar(_DAT_80279e80);
                msgctrlSetValue(0xd,uVar3);
                DAT_8038ff76 = 1;
              }
              if ((uVar11 == 1) || (uVar11 == 0xffff)) {
                uVar3 = GSmsgGetGSchar(0x76bd);
                msgctrlSetValue(0xe,uVar3);
                DAT_8038fffc = (DAT_8038ff76 & 0xf) + 0xe;
              }
              else {
                uVar3 = GSmsgGetGSchar(0x7626);
                msgctrlSetValue(0xe,uVar3);
                DAT_8038fffc = (DAT_8038ff76 & 0xf) + 0x26;
              }
              uVar3 = GSmsgGetGSchar(0x7627);
              msgctrlSetValue(0x41,uVar3);
            }
            uVar14 = *(u32 *)(&DAT_80375e24 + uVar15 * 4);
          }
        }
      }
    }
    lbl_8047B614 = 0;
    uVar3 = fn_801F8000(uVar5);
    msgctrlSetValue(0x22,uVar3);
    uVar3 = fightTrainerGetNamePtr(uVar5);
    msgctrlSetValue(0x23,uVar3);
    uVar3 = fightTrainerGetNamePtr(uVar5);
    msgctrlSetValue(0x13,uVar3);
    itemGetStatus(0,r4 & 0xffff,1,0);
    uVar3 = GSmsgGetGSchar();
    msgctrlSetValue(0x29,uVar3);
    fn_801EF8F4(1);
    if (cVar12 == 0) {
      fn_80265598(uVar4,uVar9,1);
    }
    fn_80211B94(r3,uVar14,1);
    if (cVar12 == 0) {
      heroItemDecItemDataId(uVar6,r4,1,(int)sVar10);
    }
    DAT_8038ff76 = bVar1;
    DAT_8038fffc = cVar2;
    fn_801DA7AC();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
  }
  return;
}
#endif
