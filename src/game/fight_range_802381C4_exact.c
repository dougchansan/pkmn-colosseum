/**
 * @file fight_range_802381C4_exact.c
 * @brief Exact pure-C fight-engine island, 0x802381C4 - 0x80238B0C.
 */
#include "dolphin/types.h"

extern u8 lbl_80375CA8[];
extern u32 lbl_8047B618;

#pragma optimize_for_size on
u8 fn_802381C4(void *ctx, void *param2, u32 param3) {
  extern void *pokemonGetStatus();
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  void *resolved = pokemonGetStatus(param2, 0, 0xD6, 0);
  fightFloorGetStatus(0, 0, 0x14, 0);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  fightPokemonGetPokemonPtr(resolved);
  return (u8)pokemonGetStatus(fightPokemonGetPokemonPtr(resolved), 0, 0x80,
                              (u8)param3);
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u16 fn_80238270(void *ctx, void *param2, u32 param3) {
  extern void *pokemonGetStatus();
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  void *resolved = pokemonGetStatus(param2, 0, 0xD6, 0);
  fightFloorGetStatus(0, 0, 0x14, 0);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  fightPokemonGetPokemonPtr(resolved);
  return (u16)pokemonGetStatus(fightPokemonGetPokemonPtr(resolved), 0, 0x7F,
                               (u8)param3);
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u32 fn_8023831C(ctx, param) void *ctx;
u32 param;
{
  extern void *pokemonGetStatus();
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern u32 fightPokemonGetSoubiItemSoubiDataId(void *ctx);
  void *resolved = pokemonGetStatus(param, 0, 0xD6, 0);
  fightFloorGetStatus(0, 0, 0x14, 0);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return fightPokemonGetSoubiItemSoubiDataId(resolved);
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u32 fn_802383A4(void *ctx, u32 param) {
  extern void *pokemonGetStatus();
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern u32 fightPokemonGetSoubiItemDataId(void *ctx);
  void *resolved = pokemonGetStatus(param, 0, 0xD6, 0);
  fightFloorGetStatus(0, 0, 0x14, 0);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return fightPokemonGetSoubiItemDataId(resolved);
}
#pragma optimize_for_size reset

u16 fn_8023842C(void *ctx, u32 param) {
  extern void *pokemonGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  void *resolved = pokemonGetStatus(param, 0, 0xD6, 0);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return (u16)pokemonGetStatus(fightPokemonGetPokemonPtr(resolved), 0, 0x99, 0);
}

#pragma optimize_for_size on
u32 fn_802384B4(void *ctx, void *param2, void *param3) {
  extern u32 fightTrainerGetStatus();
  extern u32 fightPokemonCheckWriteJoutaiDataId(void *ctx, void *typeObj);
  u16 val = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  if ((u8)fightTrainerGetStatus(0, val, 0x24, 0) == 1) {
    return fightPokemonCheckWriteJoutaiDataId(param2, param3);
  }
  return 0;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_80238538(void *ctx, void *param2) {
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  extern u32 pokemonGetStatus();
  u16 val, val2;
  fightFloorGetStatus(0, 0, 0x14, 0);
  val = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  val2 = (u16)pokemonGetStatus(fightPokemonGetPokemonPtr(param2), 0, 0xC9, 0);
  if ((u8)fightTrainerGetStatus(0, val, 0x33, 0) == 1) {
    return (u8)fightTrainerGetStatus(0, val2, 0x1D, 0);
  }
  return 1;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_80238600(void *ctx, void *param2) {
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  extern u32 pokemonGetStatus();
  u16 val, val2;
  fightFloorGetStatus(0, 0, 0x14, 0);
  val = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  val2 = (u16)pokemonGetStatus(fightPokemonGetPokemonPtr(param2), 0, 0xC9, 0);
  if ((u8)fightTrainerGetStatus(0, val, 0x23, 0) == 1) {
    return (u8)fightTrainerGetStatus(0, val2, 0x1C, 0);
  }
  return 0;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u32 fn_802386C8(void *ctx, void *param) {
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  extern u32 pokemonGetStatus();
  fightFloorGetStatus(0, 0, 0x14, 0);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return pokemonGetStatus(fightPokemonGetPokemonPtr(param), 0, 0x8C, 0);
}
#pragma optimize_for_size reset

u32 fn_80238748(void *ctx, void *param) {
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  extern u32 pokemonIsJoutaiNormal(u8 * ptr);
  u16 val = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  if ((u8)fightTrainerGetStatus(0, val, 0x24, 0) == 1) {
    return pokemonIsJoutaiNormal(fightPokemonGetPokemonPtr(param));
  }
  return 1;
}

u32 fn_802387C8(void *ctx, void *param) {
  extern u32 fightTrainerGetStatus();
  extern u32 fightOutPokemonGetNowHpPercentage(void *ctx);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return fightOutPokemonGetNowHpPercentage(param);
}

#pragma optimize_for_size on
u32 fn_8023881C(u32 r3, u32 r4) {
  u16 statusValues[2];
  extern u32 fightTrainerGetStatus();
  extern u32 fightPokemonGetPokemonPtr();
  extern u32 pokemonGetStatus();
  u16 uVar5;
  u16 uVar6;
  u32 uVar1;
  u8 uVar4;
  u8 bVar7;
  uVar5 = fightTrainerGetStatus(r3, 0, 0x43, 0);
  uVar5 = fightTrainerGetStatus(0, uVar5, 2, 0);
  uVar6 = fightTrainerGetStatus(r3, 0, 0x43, 0);
  fightTrainerGetStatus(0, uVar6, 2, 0);
  uVar1 = fightPokemonGetPokemonPtr(r4);
  statusValues[0] = (int)pokemonGetStatus(uVar1, 0, 0x83, 0);
  uVar6 = fightTrainerGetStatus(r3, 0, 0x43, 0);
  fightTrainerGetStatus(0, uVar6, 2, 0);
  uVar1 = fightPokemonGetPokemonPtr(r4);
  statusValues[1] = (int)pokemonGetStatus(uVar1, 0, 0x87, 0);
  uVar4 = fightTrainerGetStatus(0, uVar5, 0x2c, 0);
  bVar7 = (statusValues[0] * 100) / statusValues[1] <= uVar4;
  return bVar7;
}
#pragma optimize_for_size reset

u32 fn_8023892C(void *ctx, void *param) {
  extern u32 fightTrainerGetStatus();
  extern u32 figthPokemonGetLevel(u32 ctx);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return figthPokemonGetLevel((u32)param);
}

u16 fn_80238980(void *ctx, void *param) {
  extern u32 fightTrainerGetStatus();
  extern u16 figthPokemonGetPokemonDataId(void *ctx);
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return figthPokemonGetPokemonDataId(param);
}

int fn_802389D4(u32 r3, u32 r4) {
  extern int fn_8010C54C();
  extern u32 pokemonGetStatus(u32, u32, u32, u8);
  extern u32 fightFloorGetStatus();
  extern u8 fightTrainerIsAllyFightTargetPtr();
  extern u32 fightTrainerGetStatus();
  extern u32 fightPokemonGetPokemonPtr();
  u16 uVar5;
  u16 uVar4;
  u16 uVar3;
  u32 uVar1;
  u8 cVar7;
  u32 sVar6;
  int iVar2;
  u32 bVar9;
  int iVar8;
  iVar8 = 0;
  for (bVar9 = 0; (bVar9 & 0xff) < 2; bVar9 = bVar9 + 1) {
    uVar3 = fightFloorGetStatus(0, 0, 0x14, 0);
    uVar4 = fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(r3, 0, 0x43, 0),
                                  2, 0);
    uVar1 = fightPokemonGetPokemonPtr(r4);
    uVar5 = (int)pokemonGetStatus(uVar1, 0, 0x6e, 0);
    cVar7 = fightTrainerGetStatus(0, uVar4, 0x2a, 0);
    if (cVar7 == 1) {
      cVar7 = fightTrainerIsAllyFightTargetPtr(r3, r4, uVar3);
      if (cVar7 == 0) {
        sVar6 = pokemonGetStatus(0, uVar5, 0x16, bVar9) & 0xffff;
      } else {
        sVar6 = pokemonGetStatus(0, uVar5, 0x16, bVar9) & 0xffff;
      }
    } else {
      sVar6 = 9;
    }
    if ((u16)sVar6 != 9) {
      iVar2 = fn_8010C54C(sVar6, 0);
      iVar8 = iVar8 + iVar2;
    }
  }
  return iVar8;
}
