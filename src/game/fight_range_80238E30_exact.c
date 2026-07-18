/**
 * @file fight_range_80238E30_exact.c
 * @brief Exact pure-C fight-engine island, 0x80238E30 - 0x8023A308.
 */
#include "dolphin/types.h"

extern u8 lbl_80375CA8[];
extern u32 lbl_8047B618;
extern u16 fn_800E0C54(void);

#pragma optimize_for_size on
u8 fn_80238E30(u32 ctx, void *param2, u16 target) {
  extern u32 fightTrainerGetStatus();
  extern u32 fightFloorGetStatus();
  extern u32 pokemonGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  extern u8 fightTrainerIsAllyFightTargetPtr(u32 fieldA, void *floor, u16 idx);
  u16 val;
  u16 flag;
  u16 field6E;
  u16 temp;
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
  flag = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  field6E =
      (u16)pokemonGetStatus(fightPokemonGetPokemonPtr(param2), 0, 0x6E, 0);
  if ((u8)fightTrainerGetStatus(0, flag, 0x2A, 0) == 1) {
    if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
      temp = (u16)pokemonGetStatus(0, field6E, 0x16, 0);
    } else {
      temp = (u16)pokemonGetStatus(0, field6E, 0x16, 0);
    }
  } else {
    temp = 9;
  }
  if (target == temp) {
    goto found;
  }
  val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
  flag = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  field6E =
      (u16)pokemonGetStatus(fightPokemonGetPokemonPtr(param2), 0, 0x6E, 0);
  if ((u8)fightTrainerGetStatus(0, flag, 0x2A, 0) == 1) {
    if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
      temp = (u16)pokemonGetStatus(0, field6E, 0x16, 1);
    } else {
      temp = (u16)pokemonGetStatus(0, field6E, 0x16, 1);
    }
  } else {
    temp = 9;
  }
  if (target != temp) {
    goto not_found;
  }
found:
  return 1;
not_found:
  return 0;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_80239058(void *ctx, void *param2, u32 param3) {
  extern u32 fightFloorGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void *fightPokemonGetPokemonPtr(void *ctx);
  extern u32 pokemonGetTokuseiDataId(void *param);
  u16 val;
  u32 temp;
  void *resolved;
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  if ((u16)param3 == 0) {
    return 0;
  }
  fightFloorGetStatus(0, 0, 0x14, 0);
  val = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  resolved = fightPokemonGetPokemonPtr(param2);
  if ((u8)fightTrainerGetStatus(0, val, 0x2B, 0) == 1) {
    temp = pokemonGetTokuseiDataId(resolved);
  } else {
    temp = 0;
  }
  if ((u16)param3 == (u16)temp) {
    return 1;
  }
  return 0;
}
#pragma optimize_for_size reset

u8 fn_80239154(void *ctx, u32 param) {
  extern u32 fightTrainerGetStatus();
  extern u32 wazaGetStatus();
  u16 val = (u16)fightTrainerGetStatus(
      0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
  if ((u8)fightTrainerGetStatus(0, val, 0x36, 0) == 1) {
    return (u8)wazaGetStatus(0, param, 0x1B, 0);
  }
  return 0;
}

u8 fn_802391E0(void *ctx, u32 param) {
  extern u32 fightTrainerGetStatus();
  extern u32 wazaGetStatus();
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return (u8)wazaGetStatus(0, param, 0x2, 0);
}

u8 fn_80239244(void *ctx, u32 param) {
  extern u32 fightTrainerGetStatus();
  extern u32 wazaGetStatus();
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return (u8)wazaGetStatus(0, param, 0x5, 0);
}

#pragma optimize_for_size on
char fn_802392A8(u32 r3, u32 r4) {
  extern int wazaGetStatus();
  extern u32 fightTrainerGetStatus();
  extern u32 fn_80229B70();
  extern s8 fn_80229BD8();
  u16 uVar2;
  u8 cVar3;
  u8 bVar4;
  u8 bVar5;
  u16 uVar1;
  uVar2 = fightTrainerGetStatus(r3, 0, 0x43, 0);
  uVar1 = fightTrainerGetStatus(0, uVar2, 2, 0);
  uVar2 = fightTrainerGetStatus(r3, 0, 0x43, 0);
  fightTrainerGetStatus(0, uVar2, 2, 0);
  cVar3 = fn_80229B70(r4);
  if (cVar3 == 1) {
    bVar4 = 100;
  } else {
    cVar3 = fn_80229BD8(r4);
    if (cVar3 == 1) {
      bVar4 = 100;
    } else {
      bVar4 = wazaGetStatus(0, r4, 6, 0);
    }
  }
  bVar5 = fightTrainerGetStatus(0, uVar1, 0x35, 0);
  bVar5 = bVar4 >= bVar5;
  return bVar5;
}
#pragma optimize_for_size reset

u8 fn_802393A0(void *ctx, u32 param) {
  extern u32 fightTrainerGetStatus();
  extern u8 fn_80229B70();
  extern u8 fn_80229BD8();
  extern u32 wazaGetStatus();
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  if (fn_80229B70(param) == 1) {
    return 100;
  }
  if (fn_80229BD8(param) == 1) {
    return 100;
  }
  return (u8)wazaGetStatus(0, param, 0x6, 0);
}

#pragma optimize_for_size on
u8 fn_8023943C(int arg1, int arg2, int arg3) {
  extern int fightTrainerGetStatus();
  extern u8 wazaIsWazaTypeId();
  u16 r = (u16)fightTrainerGetStatus(arg1, 0, 0x43, 0);
  fightTrainerGetStatus(0, r, 2, 0);
  return wazaIsWazaTypeId(arg2, arg3);
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_80239498(void *ctx, u32 param, u8 flag) {
  extern u32 fightTrainerGetStatus();
  extern u32 wazaGetStatus();
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return (u8)wazaGetStatus(0, param, 0x1A, flag);
}
#pragma optimize_for_size reset

s16 fn_80239500(void *ctx, u32 param) {
  extern u32 fightTrainerGetStatus();
  extern u32 wazaGetStatus();
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return (s16)wazaGetStatus(0, param, 0x7, 0);
}

u8 fn_80239564(void *ctx, u32 param) {
  extern u32 fightTrainerGetStatus();
  extern u32 wazaGetStatus();
  fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2,
                        0);
  return (u8)wazaGetStatus(0, param, 0xc, 0);
}

u32 fn_802395C8(u32 trainer, u32 move, u32 target) {
  extern u32 fightTrainerGetStatus();
  extern u8 fightFloorGetNowTenkouDataId();
  extern void fightFloorLoopValidFightOutPokemon();
  extern void fn_80236268();
  extern u32 fightOutPokemonGetPokemonPtr();
  extern void pokemonGetMezamerupower();
  extern u32 wazaGetStatus();
  u16 hiddenPower;
  u32 callbackD[5];
  u32 callback4D[5];
  u16 trainerData;
  struct {
    u8 value;
  } weather;
  trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
  trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
  weather.value = fightFloorGetNowTenkouDataId(0, 0);
  callbackD[0] = 0xd;
  callbackD[1] = 0;
  callbackD[2] = 0;
  callbackD[3] = 0;
  callbackD[4] = trainer;
  fightFloorLoopValidFightOutPokemon(0, fn_80236268, callbackD, 0);
  if (callbackD[1] != 0) {
    weather.value = 0;
  } else {
    callback4D[0] = 0x4d;
    callback4D[1] = 0;
    callback4D[2] = 0;
    callback4D[3] = 0;
    callback4D[4] = trainer;
    fightFloorLoopValidFightOutPokemon(0, fn_80236268, callback4D, 0);
    if (callback4D[1] != 0) {
      weather.value = 0;
    }
  }
  if ((u8)fightTrainerGetStatus(0, trainerData, 0x2a, 0) == 1) {
    if ((u16)move == 0xa5 || (u16)move == 0x164 || (u16)move == 0xf8 ||
        (u16)move == 0x161) {
      return 9;
    }
    if ((u16)move == 0xed) {
      pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(target), 0,
                              &hiddenPower);
      return hiddenPower;
    }
    if ((u16)move == 0x137) {
      if (weather.value == 2) {
        hiddenPower = 0xb;
      } else if (weather.value == 3) {
        hiddenPower = 5;
      } else if (weather.value == 1) {
        hiddenPower = 0xa;
      } else if (weather.value == 4) {
        hiddenPower = 0xf;
      } else {
        hiddenPower = 0;
      }
      return hiddenPower;
    }
    return (u16)wazaGetStatus(0, move, 3, 0);
  }
  return 9;
}

#pragma optimize_for_size on
s32 fightTrainerAiGetValueAryMaxBanme(s32 *valueAry, u16 count, u8 useRandom) {
  s32 banmeAry[16];
  s32 max = -0xC8;
  u16 tieCount = 0;
  u16 i;
  u16 idx;
  for (i = 0; i < 16; i++) {
    banmeAry[i] = -1;
  }
  for (i = 0; i < count; i++) {
    if (max < valueAry[i]) {
      max = valueAry[i];
    }
  }
  for (i = 0; i < count; i++) {
    if (max <= valueAry[i]) {
      banmeAry[tieCount] = i;
      tieCount++;
    }
    if (tieCount >= 16) {
      break;
    }
  }
  if (tieCount == 0) {
    return -1;
  }
  if (tieCount == 1) {
    idx = 0;
  } else if (useRandom == 1) {
    idx = fn_800E0C54() % tieCount;
  } else {
    idx = 0;
  }
  return banmeAry[idx];
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
int fn_802398E4(int r3, int r4, u32 r5, u32 r6) {
  extern int fightTrainerGetStatus();
  int iVar1;
  int result;
  struct {
    int value;
  } factor;
  struct {
    int value;
  } status;
  status.value = fightTrainerGetStatus(0, r6, 0x3e, 0);
  factor.value = (short)(((r4 & 0xff) * 100) / 0xff) + -0x32;
  iVar1 = (int)(short)((factor.value * status.value) / 0x32);
  result = r3;
  if (iVar1 > 0) {
    result = r3 + iVar1;
    if (result > 200) {
      result = 200;
    } else {
      result = r3 + iVar1;
    }
  } else {
    if (iVar1 < 0) {
      result = r3 + iVar1;
      if (result < -200) {
        result = -200;
      }
    }
  }
  return result;
}
#pragma optimize_for_size reset

u32 fn_80239984(int r3, void *r4, u32 r5) {
  extern int fightTrainerGetStatus();
  int iVar1;
  int result;
  iVar1 = fightTrainerGetStatus(0, r5, 0x3e, 0);
  result = r3;
  if (iVar1 > 0) {
    result = r3 + iVar1;
    if (result > 200) {
      result = 200;
    } else {
      result = r3 + iVar1;
    }
  } else {
    if (iVar1 < 0) {
      result = r3 + iVar1;
      if (result < -200) {
        result = -200;
      }
    }
  }
  return result;
}

#pragma optimize_for_size on
#pragma scheduling 601
#pragma push
#pragma optimization_level 2
#pragma opt_common_subs off
s32 fightTrainerAiAddValue(s32 value, s32 delta) {
  if (delta > 0) {
    if (value + delta > 200) {
      return 200;
    }
    value += delta;
    return value;
  } else {
    if (delta >= 0) {
      return value;
    }
    if (value + delta < -200) {
      return -200;
    }
    value += delta;
    return value;
  }
}
#pragma pop
#pragma scheduling reset
#pragma optimize_for_size reset

#pragma optimize_for_size on
u32 fn_80239A40(u32 r3, u32 r4, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9, u32 r10,
                u8 param_9) {
  extern u8 fn_80008164();
  extern u32 GSmsgGetGSchar();
  extern void menuGetKeyInfo();
  extern void wazaGetStatus();
  extern void msgctrlSetValue();
  extern u32 fightTrainerGetNamePtr();
  extern int fightTrainerGetStatus();
  extern void fightMenuCloseMsg();
  extern u8 fightMenuOpenMsg();
  int iVar6;
  u32 iVar1;
  u32 iVar2;
  int iVar3;
  int factor;
  u32 uVar4;
  u8 cVar5;
  u16 local_58[14];
  iVar6 = param_9;
  iVar1 = fightTrainerGetStatus(0, r10, 0x40, 0);
  iVar2 = fightTrainerGetStatus(0, r10, 0x41, 0);
  iVar3 = fightTrainerGetStatus(0, r10, 0x3e, 0);
  factor = (short)((iVar6 * 100) / 0xff) + -0x32;
  iVar3 = (int)(short)((factor * iVar3) / 0x32);
  iVar6 = 0;
  if (iVar3 > 0) {
    if (iVar3 > 200) {
      iVar6 = 200;
    } else {
      iVar6 = iVar3;
    }
  } else {
    if (iVar3 < 0) {
      if (iVar3 < -200) {
        iVar6 = -200;
      } else {
        iVar6 = iVar3;
      }
    }
  }
  if (iVar1 != 0) {
    msgctrlSetValue(0xd, GSmsgGetGSchar(iVar1));
  }
  if (iVar2 != 0) {
    msgctrlSetValue(0xe, GSmsgGetGSchar(iVar2));
  }
  if (r4 != 0) {
    msgctrlSetValue(0x13, fightTrainerGetNamePtr(r4));
  }
  if (r5 != 0) {
    msgctrlSetValue(0x14, (int)pokemonGetStatus(r5, 0, 0x77, 0));
  }
  if (r6 != 0) {
    msgctrlSetValue(0x23, fightTrainerGetNamePtr(r6));
  }
  if (r7 != 0) {
    msgctrlSetValue(0x15, (int)pokemonGetStatus(r7, 0, 0x77, 0));
  }
  if ((r8 & 0xffff) != 0) {
    wazaGetStatus(0, r8, 1, 0);
    msgctrlSetValue(0x28, GSmsgGetGSchar());
  }
  if ((r9 & 0xffff) != 0) {
    itemGetStatus(0, r9, 1, 0);
    msgctrlSetValue(0x29, GSmsgGetGSchar());
  }
  if ((r10 & 0xffff) != 0) {
    fightTrainerGetStatus(0, r10, 0x3f, 0);
    msgctrlSetValue(0x41, GSmsgGetGSchar());
  }
  msgctrlSetValue(0x2f, iVar6);
  cVar5 = fn_80008164();
  if (cVar5 == 1) {
    menuGetKeyInfo(local_58, 1);
    if ((local_58[0] & 0x800) != 0) {
      return 0;
    }
    cVar5 = fightMenuOpenMsg(r3);
    if (cVar5 == 1) {
      fightMenuCloseMsg();
      return 0;
    }
  }
  return 0;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_80239CCC(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8,
               s32 p9) {
  extern u32 fightTrainerGetStatus();
  extern u32 GSmsgGetGSchar();
  extern void msgctrlSetValue();
  extern u32 fightTrainerGetNamePtr();
  extern u32 pokemonGetStatus();
  extern u32 wazaGetStatus();
  extern u32 itemGetStatus();
  extern void menuGetKeyInfo();
  extern u32 fn_80008164();
  extern u32 fightMenuOpenMsg();
  extern void fightMenuCloseMsg();
  u32 v40;
  u32 v41;
  u8 buf[0x20];
  v40 = fightTrainerGetStatus(0, p8, 0x40, 0);
  v41 = fightTrainerGetStatus(0, p8, 0x41, 0);
  if (v40)
    msgctrlSetValue(0xD, GSmsgGetGSchar(v40));
  if (v41)
    msgctrlSetValue(0xE, GSmsgGetGSchar(v41));
  if (p2)
    msgctrlSetValue(0x13, fightTrainerGetNamePtr(p2));
  if (p3)
    msgctrlSetValue(0x14, pokemonGetStatus(p3, 0, 0x77, 0));
  if (p4)
    msgctrlSetValue(0x23, fightTrainerGetNamePtr(p4));
  if (p5)
    msgctrlSetValue(0x15, pokemonGetStatus(p5, 0, 0x77, 0));
  if ((u16)p6)
    msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, p6, 1, 0)));
  if ((u16)p7)
    msgctrlSetValue(0x29, GSmsgGetGSchar(itemGetStatus(0, p7, 1, 0)));
  if ((u16)p8)
    msgctrlSetValue(0x41,
                    GSmsgGetGSchar(fightTrainerGetStatus(0, p8, 0x3f, 0)));
  msgctrlSetValue(0x2F, p9);
  if ((u8)fn_80008164() == 1) {
    menuGetKeyInfo(buf, 1);
    if ((*(u16 *)buf & 0x800) == 0) {
      goto do_check;
    }
    return 0;
  do_check:
    if ((u8)fightMenuOpenMsg(ctx) == 1) {
      fightMenuCloseMsg();
      return 0;
    }
  }
  return 0;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_80239EE8(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7,
               u32 p8) {
  extern u32 fightTrainerGetStatus();
  extern u32 GSmsgGetGSchar();
  extern void msgctrlSetValue();
  extern u32 fightTrainerGetNamePtr();
  extern u32 pokemonGetStatus();
  extern u32 wazaGetStatus();
  extern u32 itemGetStatus();
  extern void menuGetKeyInfo();
  extern u32 fn_80008164();
  extern u32 fightMenuOpenMsg();
  extern void fightMenuCloseMsg();
  u32 v3e;
  u32 v40;
  u32 v41;
  u8 buf[0x20];
  v40 = fightTrainerGetStatus(0, p8, 0x40, 0);
  v41 = fightTrainerGetStatus(0, p8, 0x41, 0);
  v3e = fightTrainerGetStatus(0, p8, 0x3e, 0);
  if (v40)
    msgctrlSetValue(0xD, GSmsgGetGSchar(v40));
  if (v41)
    msgctrlSetValue(0xE, GSmsgGetGSchar(v41));
  if (p2)
    msgctrlSetValue(0x13, fightTrainerGetNamePtr(p2));
  if (p3)
    msgctrlSetValue(0x14, pokemonGetStatus(p3, 0, 0x77, 0));
  if (p4)
    msgctrlSetValue(0x23, fightTrainerGetNamePtr(p4));
  if (p5)
    msgctrlSetValue(0x15, pokemonGetStatus(p5, 0, 0x77, 0));
  if ((u16)p6)
    msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, p6, 1, 0)));
  if ((u16)p7)
    msgctrlSetValue(0x29, GSmsgGetGSchar(itemGetStatus(0, p7, 1, 0)));
  if ((u16)p8)
    msgctrlSetValue(0x41,
                    GSmsgGetGSchar(fightTrainerGetStatus(0, p8, 0x3f, 0)));
  msgctrlSetValue(0x2F, v3e);
  if ((u8)fn_80008164() == 1) {
    menuGetKeyInfo(buf, 1);
    if ((*(u16 *)buf & 0x800) == 0) {
      goto do_check;
    }
    return 0;
  do_check:
    if ((u8)fightMenuOpenMsg(ctx) == 1) {
      fightMenuCloseMsg();
      return 0;
    }
  }
  return 0;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
u8 fn_8023A118(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8,
               u16 p9, u16 p10, u32 p11) {
  extern u32 GSmsgGetGSchar();
  extern void msgctrlSetValue();
  extern u32 fightTrainerGetNamePtr();
  extern u32 pokemonGetStatus();
  extern u32 wazaGetStatus();
  extern u32 itemGetStatus();
  extern u32 fightTrainerGetStatus();
  extern void menuGetKeyInfo();
  extern u32 fn_80008164();
  extern u32 fightMenuOpenMsg();
  extern void fightMenuCloseMsg();
  u8 buf[0x20];
  if (p2)
    msgctrlSetValue(0xD, GSmsgGetGSchar(p2));
  if (p3)
    msgctrlSetValue(0xE, GSmsgGetGSchar(p3));
  if (p4)
    msgctrlSetValue(0x13, fightTrainerGetNamePtr(p4));
  if (p5)
    msgctrlSetValue(0x14, pokemonGetStatus(p5, 0, 0x77, 0));
  if (p6)
    msgctrlSetValue(0x23, fightTrainerGetNamePtr(p6));
  if (p7)
    msgctrlSetValue(0x15, pokemonGetStatus(p7, 0, 0x77, 0));
  if ((u16)p8)
    msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, p8, 1, 0)));
  if (p9)
    msgctrlSetValue(0x29, GSmsgGetGSchar(itemGetStatus(0, p9, 1, 0)));
  if (p10)
    msgctrlSetValue(0x41,
                    GSmsgGetGSchar(fightTrainerGetStatus(0, p10, 0x3f, 0)));
  msgctrlSetValue(0x2F, p11);
  if ((u8)fn_80008164() == 1) {
    menuGetKeyInfo(buf, 1);
    if ((*(u16 *)buf & 0x800) == 0) {
      goto do_check;
    }
    return 0;
  do_check:
    if ((u8)fightMenuOpenMsg(ctx) == 1) {
      fightMenuCloseMsg();
      return 0;
    }
  }
  return 0;
}
#pragma optimize_for_size reset
