/**
 * @file fight_range_8021A338.c
 * @brief Fight sequence carve, 0x8021A338 - 0x8021A6CC (wsCmd9C,
 *        wsCmd9B), linked.
 *
 * Built with GC/1.3.2 at -O4,s (configure.py). Under GC/1.3 the scheduler
 * issues the flag-byte store ahead of the ori/addi that feeds the following
 * word store, which the old candidate imitated with duplicate stores and
 * per-function pragmas; GC/1.3.2 schedules the plain source as retail does.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_80478D78[1];
extern u32 lbl_8047B618;

void wsCmd9C(void)

{
    extern void wazaSetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightOutPokemonSetPokemonJoutai08();
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern u8 fn_802025B8();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonMaxHpWaruValue();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B618;
  u32 attacker;
  u32 move;
  u32 pokemon;
  u16 hp;
  u16 amount;

  attacker = fightTargetGetPtrAsNowFightType(0x11,0);
  pokemon = fightOutPokemonGetPokemonPtr();
  hp = pokemonGetStatus(pokemon,0,0x83,0);
  move = pokemonGetStatus(attacker,0,0xd9,0);
  amount = fightOutPokemonMaxHpWaruValue(attacker,4);
  if ((s32)hp > (s32)amount) {
    goto check_status;
  }
failed:
  wazaSetStatus(move,0,0x2d,0,0);
  lbl_80478D78[5] = 1;
  goto done;
check_status:
  if (fn_802025B8(attacker,0x14) != 2) {
    goto failed;
  }
  wazaSetStatus(move,0,0x2d,0,amount);
  fightOutPokemonWritePokemonJoutaiDataId(attacker,0x14,0);
  fightOutPokemonSetPokemonJoutai08(attacker,0x14,amount);
  if (fightOutPokemonIsPokemonJoutaiDataId(attacker,0xe) == 1) {
    fightOutPokemonWriteJoutaiDataId(attacker,0xe);
  }
  lbl_80478D78[5] = 0;
  lbl_8047B618 |= 0x100;
done:
  lbl_8047B610++;
  return;
}

void wsCmd9B(void)

{
    extern void wazaSetStatus();
    extern u8 pokemonIsDarkPokemon();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern void fightOutPokemonCopyHensinStatus();
    extern u8 fightOutPokemonUseHensinBuff();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u16 fightOutPokemonIsJoutaiKie();
    extern void fightOutPokemonSetPokemonJoutai08();
    extern void fightOutPokemonSetPokemonJoutai0C(u32, u32, u16);
    extern void fightOutPokemonSetPokemonJoutaiKaisuu();
    extern void fn_80201FDC(u32, u32, s8);
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonGetNamePtr();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u8 cVar9;
  u16 sVar8;
  u32 *puVar7;
  u8 lowByte;

  uVar1 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar2 = pokemonGetStatus(uVar1,0,0xd9,0);
  uVar3 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar4 = pokemonGetStatus(fightOutPokemonGetPokemonPtr(),0,0x6f,0);
  uVar5 = fightOutPokemonGetPokemonPtr(uVar3);
  uVar6 = pokemonGetStatus(uVar5,0,0x75,0);
  wazaSetStatus(uVar2,0,0x27,0,0xffff);
  cVar9 = fightOutPokemonIsPokemonJoutaiDataId(uVar3,0x10);
  if (cVar9 == 1) {
    goto failed;
  }
  sVar8 = fightOutPokemonIsJoutaiKie(uVar3);
  if (sVar8 != 0) {
    goto failed;
  }
  fightOutPokemonGetPokemonPtr(uVar3);
  cVar9 = pokemonIsDarkPokemon();
  if (cVar9 != 1) {
    goto check_hensin;
  }

failed:
  fightFloorSetStatus(0,0,0x3b,0,0x45);
  lbl_80478D78[5] = 1;
  lbl_8047B610++;
  goto done;

check_hensin:
  cVar9 = fightOutPokemonIsUseHensinBuff(uVar1);
  if (cVar9 != 0) {
    goto apply_hensin;
  }
  cVar9 = fightOutPokemonUseHensinBuff(uVar1);
  if (cVar9 == 0) {
    goto failed;
  }

apply_hensin:
  cVar9 = fightOutPokemonIsPokemonJoutaiDataId(uVar1,0x10);
  if (cVar9 == 0) {
    fightOutPokemonWritePokemonJoutaiDataId(uVar1,0x10,0);
  }
  fightOutPokemonSetPokemonJoutai08(uVar1,0x10,uVar4);
  lowByte = (u8)uVar6;
  fightOutPokemonSetPokemonJoutaiKaisuu(uVar1,0x10,(s8)lowByte);
  fn_80201FDC(uVar1,0x10,(s8)(u8)(uVar6 >> 8));
  fightOutPokemonSetPokemonJoutai0C(uVar1,0x10,uVar6 >> 0x10);
  cVar9 = fightOutPokemonIsPokemonJoutaiDataId(uVar1,0x29);
  if (cVar9 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x29);
  }
  cVar9 = fightOutPokemonIsPokemonJoutaiDataId(uVar1,0x31);
  if (cVar9 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x31);
  }
  msgctrlSetValue(0xd,fightOutPokemonGetNamePtr(uVar3));
  lbl_80478D78[5] = 0;
  fightOutPokemonCopyHensinStatus(uVar3,uVar1);
  puVar7 = (u32 *)pokemonGetStatus(uVar1,0,0x101,0);
  if (puVar7 != (void *)0) {
    *puVar7 = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
done:
  return;
}
