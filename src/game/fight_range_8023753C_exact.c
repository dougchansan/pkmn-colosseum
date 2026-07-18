/**
 * @file fight_range_8023753C_exact.c
 * @brief Exact pure-C fight-engine island.
 */
#include "dolphin/types.h"

#define fn_8011BEB4 wazaGetStatus
#define fn_80122DDC pokemonIsJoutaiNormal
#define fn_8012640C pokemonGetStatus
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F8424 fightTrainerIsAllyFightTargetPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FFEC8 fightOutPokemonCheckCanOutOkWazaBanme
#define fn_802010C8 fightOutPokemonIsJoutaiKie
#define fn_8020156C fightOutPokemonGetAllAbiCnt
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_80203DAC figthPokemonGetPokemonDataId
#define fn_80203E7C figthPokemonGetLevel
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80207B8C fightOutPokemonGetZokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId

extern u32 fightTrainerGetStatus();
extern u32 fightFloorGetStatus();
extern u8 fightTrainerIsAllyFightTargetPtr(u32, u32, u16);
extern u16 fightOutPokemonGetZokuseiDataId(u32, u8);
extern u16 fightOutPokemonGetTokuseiDataId(u32);
extern u32 fn_8010C650(u16, u16*, u16);
extern int fn_802026E4();

#pragma optimize_for_size on
u32 fn_8023753C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
    extern u32 fn_8012640C();
  u16 uVar3;
  u32 uVar1;
  u16 uVar6;
  u16 uVar7;
  u32 uVar2;
  u16 uVar4;
  u8 uVar5;

  uVar1 = fn_8012640C(r4,0,0xd6,0);
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  uVar6 = fn_801FB1C0(0,uVar6,2,0);
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  uVar2 = fn_80205BE8(uVar1);
  uVar3 = fn_8012640C(uVar2,0,0x83,0);
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar4 = fn_8012640C(uVar1,0,0x87,0);
  uVar5 = fn_801FB1C0(0,uVar6,0x2c,0);
  return (int)((int)uVar3 * 100) / (int)uVar4 <= (int)uVar5;
}
#pragma optimize_for_size reset

u16 fn_80237664(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x87, 0);
}

u16 fn_802376EC(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x83, 0);
}

u32 fn_80237774(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern u32 fn_80203E7C(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203E7C(resolved);
}

u16 fn_802377E8(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern u16 fn_80203DAC(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203DAC(resolved);
}


#pragma optimize_for_size on
u8 fn_8023785C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val, val2;
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val2 = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0xC9, 0);
    if ((u8)fn_801FB1C0(0, val, 0x23, 0) == 1) {
        return (u8)fn_801FB1C0(0, val2, 0x1C, 0);
    }
    return 0;
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u32 fn_8023793C(u32 ctx, u32 poke, u16 waza, s32 val) {
    u16 types[2];
    u16 tmp;
    u16 tk;
    struct {
        u16 value;
    } typeCount;
    u16 z;
    struct {
        u8 value;
    } i;
    u16 j;
    u16 floorVal;
    u32 result;
    u8 tkMatch;
    u32 valid;

    if (waza == 9) {
        return 0x3f;
    }

    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    fightTrainerGetStatus(0, tmp, 2, 0);
    fightFloorGetStatus(0, 0, 0x14, 0);

    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
    if ((u8)fightTrainerGetStatus(0, tmp, 0x2b, 0) == 1) {
        tk = fightOutPokemonGetTokuseiDataId(poke);
    } else {
        tk = 0;
    }
    tkMatch = (tk == 0x1a);
    if (tkMatch == 1 && waza == 4) {
        return 0x43;
    }

    typeCount.value = 0;
    i.value = typeCount.value;
    for (; i.value < 2; i.value++) {
        floorVal = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
        tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
        tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
        if ((u8)fightTrainerGetStatus(0, tmp, 0x2a, 0) == 1) {
            if ((u8)fightTrainerIsAllyFightTargetPtr(ctx, poke, floorVal) == 0) {
                z = fightOutPokemonGetZokuseiDataId(poke, i.value);
            } else {
                z = fightOutPokemonGetZokuseiDataId(poke, i.value);
            }
        } else {
            z = 9;
        }
        if (z != 9) {
            types[typeCount.value] = z;
            typeCount.value++;
        }
    }
    if (typeCount.value == 0) {
        return 0x3f;
    }

    result = fn_8010C650(waza, types, typeCount.value);
    if (waza == 0 || waza == 1) {
        tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
        tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
        if ((u8)fightTrainerGetStatus(0, tmp, 0x24, 0) == 1) {
            valid = fn_802026E4(poke, 0x19);
        } else {
            valid = 0;
        }
        if ((u8)valid == 1) {
            for (j = 0; j < typeCount.value; j++) {
                u16 v = types[j];
                if (v != 9 && v == 7) {
                    result = 0x3f;
                }
            }
        }
    }

    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    fightTrainerGetStatus(0, tmp, 2, 0);
    fightFloorGetStatus(0, 0, 0x14, 0);
    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
    if ((u8)fightTrainerGetStatus(0, tmp, 0x2b, 0) == 1) {
        tk = fightOutPokemonGetTokuseiDataId(poke);
    } else {
        tk = 0;
    }
    tkMatch = (tk == 0x19);
    if (tkMatch == 1 && (u16)result != 0x41 && (s16)val > 0) {
        return 0x43;
    }
    return result;
}
#pragma optimize_for_size reset

u32 fn_80237CB8(u32 r3, u32 r4, int r5)

{
    extern u16 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u16 fn_801FB1C0();
    extern u16 fn_80207B8C();
  u32 uVar1;
  u32 uVar2;
  u8 cVar5;
  u16 sVar4;
  u32 uVar7;
  u32 uVar6;

  uVar7 = 0;
  for (uVar6 = 0; (uVar6 & 0xff) < 2; uVar6 = uVar6 + 1) {
    uVar2 = fn_801F54A4(0,0,0x14,0);
    cVar5 = fn_801FB1C0(0,fn_801FB1C0(0,fn_801FB1C0(r3,0,0x43,0),2,0),0x2a,0);
    if (cVar5 == 1) {
      cVar5 = fn_801F8424(r3,r4,uVar2);
      if (cVar5 == 0) {
        sVar4 = fn_80207B8C(r4,uVar6);
      }
      else {
        sVar4 = fn_80207B8C(r4,uVar6);
      }
    }
    else {
      sVar4 = 9;
    }
    if (sVar4 != 9) {
      uVar1 = uVar7 & 0xffff;
      uVar7 = uVar7 + 1;
      *(short *)(r5 + uVar1 * 2) = sVar4;
    }
  }
  return uVar7;
}


#pragma optimize_for_size on
u8 fn_80237DBC(u32 ctx, void* param2, u16 target) {
    extern u32 fn_801FB1C0();
    extern u32 fn_801F54A4();
    extern u8 fightTrainerIsAllyFightTargetPtr(u32 fieldA, void* floor, u16 idx);
    extern u16 fn_80207B8C(void* context, u8 field);
    u16 val;
    u16 temp;
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    if ((u8)fn_801FB1C0(0, fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0) & 0xFFFF, 0x2A, 0) == 1) {
        if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
            temp = fn_80207B8C(param2, 0);
        } else {
            temp = fn_80207B8C(param2, 0);
        }
    } else {
        temp = 9;
    }
    if (target == temp) {
        goto found;
    }
    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    if ((u8)fn_801FB1C0(0, fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0) & 0xFFFF, 0x2A, 0) == 1) {
        if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
            temp = fn_80207B8C(param2, 1);
        } else {
            temp = fn_80207B8C(param2, 1);
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
u8 fn_80237F74(void* ctx, u32 param2, u32 param3) {
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80207BF4(void* param);
    u16 val;
    u32 temp;
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u16)param3 == 0) {
        return 0;
    }
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x2B, 0) == 1) {
        temp = fn_80207BF4((void*)param2);
    } else {
        temp = 0;
    }
    if ((u16)param3 == (u16)temp) {
        return 1;
    }
    return 0;
}
#pragma optimize_for_size reset
