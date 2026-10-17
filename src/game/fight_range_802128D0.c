/**
 * @file fight_range_802128D0.c
 * @brief Fight turn-end carve, 0x80213270 - 0x802134D4 (fightSeqTurnEnd), linked.
 *
 * Standalone copy of the shared candidate body. The four flag clears are
 * plain read-modify-writes of lbl_8047B618 (retail loads it once and
 * stores after each mask); no pragmas are needed.
 */
#include "dolphin/types.h"

extern u8 _fightSeqTurnEndJoutaiSub(void* ctx, u32 side);
extern s32 _fightSeqTurnCheckSubFightOutPokemon__FPvUsPv(void* ctx, u16 param2, void* param3);

void fightSeqTurnEnd(void)

{
    extern u32 GSrand();
    extern void wazaSequenceSysFreeAll();
    extern u16 std103_battleGetResult();
    extern void fightFloorInitFightOutPokemonPtrAryFightWazaJoutai();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetValidFightSidePtr();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern void fightSideInitJoutaiDataId();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern u32 _fightSeqTurnEndSubFightOutPokemon();
    extern u32 _fightJoutai1ETurnEndSub();
    extern u32 fn_80230318();
    extern void wsRunEndOfTurnScript();
    extern void fightSeqTurnEndAll();
    extern u8 lbl_80379F58[];
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B618;
  u32 side;
  u16 sVar2;
  u16 uVar4;

  u32 uVar6;
  u32 bVar5;
  u8 local_17;
  u8 local_18;

  fightMenuAllFightTrainerCloseStatusMenu(0);
  fightMenuAllFightOutPokemonCloseStatusMenu(0);
  fightMenuCloseMsg();
  for (bVar5 = 0; (bVar5 & 0xff) < 8; bVar5 = bVar5 + 1) {
    *(u8 *)(lbl_80478D78 + (u8)bVar5) = 0;
  }
  fightFloorLoopValidFightOutPokemon(0,_fightSeqTurnCheckSubFightOutPokemon__FPvUsPv,0,0);
  fightFloorInitFightOutPokemonPtrAryFightWazaJoutai(0);
  local_17 = 1;
  fightFloorLoopValidFightOutPokemon(0,_fightSeqTurnEndSubFightOutPokemon,&local_17,0);
  for (uVar6 = 0; (uVar6 & 0xffff) < 2; uVar6 = uVar6 + 1) {
    side = fightFloorGetValidFightSidePtr(0,uVar6);
    if (side != 0) {
      fightSideInitJoutaiDataId(side,0x4d);
    }
  }
  fightSeqTurnEndAll();
  fightFloorLoopValidFightOutPokemon(0,_fightSeqTurnEndJoutaiSub,0,1);
  wsRunEndOfTurnScript();
  fightFloorLoopValidFightOutPokemon(0,fn_80230318,0,0);
  wazaSequenceSysFreeAll();
  fightFloorLoopValidFightOutPokemon(0,_fightJoutai1ETurnEndSub,0,1);
  wazaSequenceSysFreeAll();
  local_18 = 0;
  fightFloorLoopValidFightOutPokemon(0,_fightSeqTurnEndSubFightOutPokemon,&local_18,0);
  for (uVar6 = 0; (uVar6 & 0xffff) < 2; uVar6 = uVar6 + 1) {
    side = fightFloorGetValidFightSidePtr(0,uVar6);
    if (side != 0) {
      fightSideInitJoutaiDataId(side,0x4d);
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfffffdff;
  lbl_8047B618 = lbl_8047B618 & 0xfff7ffff;
  lbl_8047B618 = lbl_8047B618 & 0xffbfffff;
  lbl_8047B618 = lbl_8047B618 & 0xffefffff;
  lbl_80379F58[0x16002] = 0;
  lbl_80379F58[0x160a1] = 0;
  for (bVar5 = 0; (bVar5 & 0xff) < 5; bVar5 = bVar5 + 1) {
    lbl_80478D78[(u8)bVar5] = 0;
  }
  sVar2 = std103_battleGetResult();
  if (sVar2 == 0) {
    sVar2 = fightFloorGetStatus(0,0,0xc,0);
    ++sVar2;
    if (0xff < sVar2) {
      sVar2 = 0xff;
    }
    fightFloorSetStatus(0,0,0xc,0,sVar2);
    uVar4 = GSrand();
    fightFloorSetStatus(0,0,0x5b,0,uVar4);
    wazaSequenceSysFreeAll();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
  }
  return;
}
