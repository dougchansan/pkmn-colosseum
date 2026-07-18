/**
 * @file fight_range_8023565C_exact.c
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
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80207B8C fightOutPokemonGetZokuseiDataId


#pragma optimize_for_size on
u32 fn_8023565C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_8020156C();
  u16 uVar1;
  u16 uVar2;
  u32 uVar3;
  u16 uVar4;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar3 = fn_8020156C(r4);
  uVar4 = fn_801FB1C0(0,uVar1,0x2f,0);
  return (u8)((s32)(u8)uVar4 >= (s32)(uVar3 & 0xffff));
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u8 fn_80235714(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_8020156C();
  u16 uVar1;
  u16 uVar2;
  u32 uVar3;
  u16 uVar4;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar3 = fn_8020156C(r4);
  uVar4 = fn_801FB1C0(0,uVar1,0x2e,0);
  return (s32)(u8)uVar4 <= (s32)(u16)uVar3;
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u8 fn_802357CC(u32 ctx, u32 param1) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    extern u8  fn_802026E4();
    u16 slot1;
    u16 slot2;
    u8 val;
    u8 x;

    slot1 = (u16)fn_801FB1C0(ctx, 0, 0x43, 0);
    fn_801FB1C0(0, slot1, 2, 0);
    val = (u8)fn_8012640C(param1, 0, 0xec, 0);

    slot2 = (u16)fn_801FB1C0(ctx, 0, 0x43, 0);
    slot2 = (u16)fn_801FB1C0(0, slot2, 2, 0);

    if ((u8)fn_801FB1C0(0, slot2, 0x24, 0) == 1) {
        x = fn_802026E4(param1, 0x19);
    } else {
        x = 0;
    }

    if (x == 1) {
        if (val > 6) {
            val = 6;
        }
    }
    return val;
}
#pragma optimize_for_size reset

u8 fn_802358AC(void* param1, void* param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xeb, 0);
}

u8 fn_80235910(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xea, 0);
}

u8 fn_80235974(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe9, 0);
}

u8 fn_802359D8(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe8, 0);
}

u8 fn_80235A3C(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe7, 0);
}

u8 fn_80235AA0(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe6, 0);
}


#pragma optimize_for_size on
u8 fn_80235B04(void* ctx, u32 arg2, u32 arg3) {
    extern u8 fn_801F453C();
    extern u32 fn_801F37B0();
    extern void fn_80236268();
    u8 ret = fn_801F453C(arg2, 0);
    if ((u8)arg3 == 1) {
        u32 msg1[5];
        msg1[0] = 0xd;
        msg1[1] = 0;
        msg1[2] = 0;
        msg1[3] = 0;
        msg1[4] = (u32)ctx;
        fn_801F37B0(arg2, fn_80236268, &msg1[0], 0);
        if (msg1[1] != 0) {
            return 0;
        }
        {
            u32 msg2[5];
            msg2[0] = 0x4d;
            msg2[1] = 0;
            msg2[2] = 0;
            msg2[3] = 0;
            msg2[4] = (u32)ctx;
            fn_801F37B0(arg2, fn_80236268, &msg2[0], 0);
            if (msg2[1] != 0) {
                return 0;
            }
        }
    }
    return ret;
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u32 fn_80235BE4(u32 trainer, u32 floor, u32 fightOut, u32* outValue)

{
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern u8 fightTrainerIsAllyFightTargetPtr();
    extern u32 fightTrainerGetStatus();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonCheckValid();
    extern u32 fightOutPokemonGetZokuseiDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fn_80236268();
  u8 bVar1;
  u8 bVar2;
  u32 iVar3;
  u32 iVar4;
  u32 iVar5;
  u8 cVar10;
  u8 flag;
  u16 uVar7;
  u16 uVar8;
  u32 sVar9;
  u32 uVar6;
  u32 check17[5];
  u32 check47[5];
  u32 check2A[5];

  bVar1 = 0;
  bVar2 = 0;
  cVar10 = fightOutPokemonCheckValid(fightOut);
  if (cVar10 == 0) {
    return 0;
  }
  check17[0] = 0x17;
  check17[1] = 0;
  check17[2] = 2;
  check17[3] = fightOut;
  check17[4] = trainer;
  fightFloorLoopValidFightOutPokemon(floor, fn_80236268, check17, 0);
  iVar5 = check17[1];
  check47[0] = 0x47;
  check47[1] = 0;
  check47[2] = 2;
  check47[3] = fightOut;
  check47[4] = trainer;
  fightFloorLoopValidFightOutPokemon(floor, fn_80236268, check47, 0);
  iVar4 = check47[1];
  check2A[0] = 0x2a;
  check2A[1] = 0;
  check2A[2] = 0;
  check2A[3] = fightOut;
  check2A[4] = trainer;
  fightFloorLoopValidFightOutPokemon(floor, fn_80236268, check2A, 0);
  iVar3 = check2A[1];
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 == 2) {
    goto element_two_true;
  }
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 != 2) {
    goto element_two_false;
  }
element_two_true:
  flag = 1;
  goto element_two_done;
element_two_false:
  flag = 0;
element_two_done:
  if (flag == 1) {
    goto set_first_flag;
  }
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x2b,0);
  if (cVar10 == 1) {
    uVar6 = fightOutPokemonGetTokuseiDataId(fightOut);
  }
  else {
    uVar6 = 0;
  }
  uVar6 = __cntlzw(0x1a - (uVar6 & 0xffff));
  if ((uVar6 >> 5 & 0xff) != 1) {
    goto first_flag_done;
  }
set_first_flag:
  bVar1 = 1;
first_flag_done:
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 == 8) {
    goto element_eight_true;
  }
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 != 8) {
    goto element_eight_false;
  }
element_eight_true:
  flag = 1;
  goto element_eight_done;
element_eight_false:
  flag = 0;
element_eight_done:
  if (flag == 1) {
    bVar2 = 1;
  }
LAB_00233098:
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(fightOut,0x16);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 == 1) {
    goto return_one;
  }
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(fightOut,0xe);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 == 1) {
    goto return_one;
  }
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(fightOut,0x25);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 != 1) {
    goto check_outputs;
  }
return_one:
  return 1;

check_outputs:
  if (iVar5 != 0) {
    if (outValue != (void *)0) {
      *outValue = iVar5;
    }
    return 2;
  }
  if ((iVar4 != 0) && (!bVar1)) {
    if (outValue != (void *)0) {
      *outValue = iVar4;
    }
    return 2;
  }
  if ((iVar3 != 0) && (bVar2 == 1)) {
    if (outValue != (void *)0) {
      *outValue = iVar3;
    }
    return 2;
  }
  return 0;
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u32 fn_80236268(u32 attacker, u32 unused, u32* state)

{
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightOutPokemonGetTokuseiDataId();
    u32 originalAttacker = attacker;
    u32 reference = state[3];
    u32 trainer;
    u32 relative;
    u32 attackerSide;
    s32 mode;
    u16 trainerId;
    u32 actual;
    u8 matches;

    trainer = state[4];
    if (fightOutPokemonCheckFightOut(originalAttacker) == 0) {
        return 1;
    }

    if (reference == 0) {
        relative = 0;
    } else if ((s32)state[2] == 1) {
        relative = fightTargetGetPtrAsNowFightType(2, reference);
    } else if ((s32)state[2] == 2) {
        relative = fightTargetGetPtrAsNowFightType(3, reference);
    } else {
        relative = 0;
    }

    attackerSide = fightTargetGetPtrAsNowFightType(2, originalAttacker);
    mode = state[2];
    if ((mode == 1 || mode == 2) && relative == 0) {
        return 1;
    }
    if (mode == 0) {
        if (reference != 0 && reference == originalAttacker) {
            return 1;
        }
        goto mode_done;
    }
    if (mode == 1) {
        goto compare_side;
    }
    if (mode != 2) {
        goto invalid_mode;
    }
compare_side:
    if (relative == attackerSide) {
        goto mode_done;
    }
    return 1;
invalid_mode:
    return 1;
mode_done:

    attacker = (u16)state[0];
    trainerId = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    fightTrainerGetStatus(0, trainerId, 2, 0);
    if (attacker == 0) {
        matches = 0;
    } else {
        fightFloorGetStatus(0, 0, 0x14, 0);
        trainerId = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
        trainerId = (u16)fightTrainerGetStatus(0, trainerId, 2, 0);
        if ((u8)fightTrainerGetStatus(0, trainerId, 0x2b, 0) == 1) {
            actual = fightOutPokemonGetTokuseiDataId(originalAttacker);
        } else {
            actual = 0;
        }
        if (attacker == (u16)actual) {
            matches = 1;
        } else {
            matches = 0;
        }
    }

    if (matches == 1) {
        state[1] = originalAttacker;
        return 0;
    }
    return 1;
}
#pragma optimize_for_size reset

u16 fn_80236458(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xEF, 0);
}

u16 fn_802364BC(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xF0, 0);
}

u32 fn_80236520(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xF1, 0);
}


#pragma optimize_for_size on
u32 fn_80236584(u32 ctx, u32 fightOut, u16 wantedMove, s16* outSlot, u8 checkUsable)
{
    extern void fightFloorGetStatus();
    extern u16 fightTrainerGetStatus();
    extern u32 pokemonGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme();
    u16 moveIds[10];
    s16 slots[10];
    struct {
        u16 value;
    } count;
    u16 slot;
    struct {
        u16 value;
    } i;

    fightFloorGetStatus(0, 0, 0x14, 0);
    fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0), 2, 0);

    count.value = 0;
    for (i.value = count.value; i.value < 10; i.value++) {
        if (moveIds != 0) {
            moveIds[i.value] = count.value;
        }
        if (slots != 0) {
            slots[i.value] = -1;
        }
    }

    for (slot = 0; slot < 4; slot++) {
        struct {
            u32 value;
        } pokemon;
        u8 slot8;
        slot8 = (u8)slot;
        pokemon.value = pokemonGetStatus(fightOut, 0, 0xD6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0), 2, 0);
        fightPokemonGetPokemonPtr(pokemon.value);
        pokemon.value = fightPokemonGetPokemonPtr(pokemon.value);
        pokemon.value = (u16)pokemonGetStatus(pokemon.value, 0, 0x7F, slot8);
        if (pokemon.value == 0) {
            continue;
        }
        if (pokemon.value == 0x165) {
            if (pokemon.value == 0x163) {
                continue;
            }
        }
        if (checkUsable == 1) {
            fightFloorGetStatus(0, 0, 0x14, 0);
            fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0), 2, 0);
            if (fightOutPokemonCheckCanOutOkWazaBanme(fightOut, slot8, 0, 0) != 0) {
                continue;
            }
        }
        if (moveIds != 0) {
            moveIds[count.value] = pokemon.value;
        }
        if (slots != 0) {
            slots[count.value] = slot;
        }
        count.value++;
    }

    for (i.value = 0; i.value < count.value; i.value++) {
        if (wantedMove == moveIds[i.value]) {
            if (outSlot != 0) {
                *outSlot = slots[i.value];
            }
            return 1;
        }
    }
    return 0;
}
#pragma optimize_for_size reset

u32 fn_802367CC(void* ctx, u32 fightOut, void* moveIds, u32 slots,
                u32 checkUsable)
{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    extern u8 fn_801FFEC8();
    extern u32 fn_80205BE8();
    u16 trainer;
    u32 pokemon;
    u8 slot8;
    u16 slot;
    u32 count;

  fn_801F54A4(0,0,0x14,0);
  trainer = fn_801FB1C0(ctx,0,0x43,0);
  fn_801FB1C0(0,trainer,2,0);
  count = 0;
  for (slot = count; slot < 10; slot++) {
    if (moveIds != 0) {
      ((u16*)moveIds)[slot] = count;
    }
    if (slots != 0) {
      ((s16*)slots)[slot] = -1;
    }
  }

  for (slot = 0; slot < 4; slot++) {
    slot8 = (u8)slot;
    pokemon = (0, fn_8012640C(fightOut,0,0xd6,0));
    fn_801F54A4(0,0,0x14,0);
    trainer = fn_801FB1C0(ctx,0,0x43,0);
    fn_801FB1C0(0,trainer,2,0);
    fn_80205BE8(pokemon);
    pokemon = fn_80205BE8(pokemon);
    pokemon = (u16)fn_8012640C(pokemon,0,0x7f,slot8);
    if (pokemon == 0) {
      continue;
    }
    if (pokemon == 0x165) {
      if (pokemon == 0x163) {
        continue;
      }
    }
    if ((u8)checkUsable == 1) {
      fn_801F54A4(0,0,0x14,0);
      trainer = fn_801FB1C0(ctx,0,0x43,0);
      fn_801FB1C0(0,trainer,2,0);
      if (fn_801FFEC8(fightOut,slot8,0,0) != 0) {
        continue;
      }
    }
    if (moveIds != 0) {
      ((u16*)moveIds)[(u16)count] = pokemon;
    }
    if (slots != 0) {
      ((s16*)slots)[(u16)count] = slot;
    }
    count++;
  }
  return count;
}

u32 fn_802369B8(u32 trainer, u32 fightPokemon, u16* outMoves,
                s16* outSlots, u32 requirePp)
{
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    u16 species;
    u16 index;
    u32 move;
    u32 pokemon;
    u32 count;

    fightFloorGetStatus(0, 0, 0x14, 0);
    species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    fightTrainerGetStatus(0, species, 2, 0);

    count = 0;
    for (index = 0; index < 10; index++) {
        if (outMoves != 0) {
            outMoves[index] = 0;
        }
        if (outSlots != 0) {
            outSlots[index] = -1;
        }
    }

    for (index = 0; index < 4; index++) {
        fightFloorGetStatus(0, 0, 0x14, 0);
        species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
        fightTrainerGetStatus(0, species, 2, 0);
        fightPokemonGetPokemonPtr(fightPokemon);
        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
        move = (u16)pokemonGetStatus(pokemon, 0, 0x7f, (u8)index);

        if (move != 0 && (move != 0x165 || move != 0x163)) {
            if ((u8)requirePp == 1) {
                fightFloorGetStatus(0, 0, 0x14, 0);
                species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
                fightTrainerGetStatus(0, species, 2, 0);
                fightPokemonGetPokemonPtr(fightPokemon);
                pokemon = fightPokemonGetPokemonPtr(fightPokemon);
                if ((u8)pokemonGetStatus(pokemon, 0, 0x80, (u8)index) == 0) {
                    continue;
                }
            }
            if (outMoves != 0) {
                outMoves[(u16)count] = move;
            }
            if (outSlots != 0) {
                outSlots[(u16)count] = index;
            }
            count++;
        }
    }
    return count;
}

u16 fn_80236B98(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xFA, 0);
}


#pragma optimize_for_size on
u32 fn_80236BFC(void* ctx, void* param2, void* param3) {
    extern u32 fn_801FB1C0();
    extern u32 fn_802026E4();
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_802026E4(param2, param3);
    }
    return 0;
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u8 fn_80236C80(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val, val2;
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val2 = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0xC9, 0);
    if ((u8)fn_801FB1C0(0, val, 0x33, 0) == 1) {
        return (u8)fn_801FB1C0(0, val2, 0x1D, 0);
    }
    return 1;
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
int fn_80236D60(u32 r3, u32 r4, u32 r5)

{
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  struct {
    u32 value;
  } uVar1;
  u16 uVar4;
  u16 uVar2;
  u32 uVar3;

  uVar1.value = (int)pokemonGetStatus(r4,0,0xd6,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar4 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar4,2,0);
  uVar1.value = fightPokemonGetPokemonPtr(uVar1.value);
  uVar4 = (int)pokemonGetStatus(uVar1.value,0,0x6e,0);
  uVar2 = (int)pokemonGetStatus(0,uVar4,8,0);
  uVar1.value = (int)pokemonGetStatus(r5,0,0xd6,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar4 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar4,2,0);
  uVar1.value = fightPokemonGetPokemonPtr(uVar1.value);
  uVar4 = (int)pokemonGetStatus(uVar1.value,0,0x6e,0);
  uVar3 = (int)pokemonGetStatus(0,uVar4,8,0);
  return (uVar2 & 0xffff) - (uVar3 & 0xffff);
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u16 fn_80236E9C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x8, 0);
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u16 fn_80236F4C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x7, 0);
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u16 fn_80236FFC(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x6, 0);
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u16 fn_802370AC(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x5, 0);
}
#pragma optimize_for_size reset


#pragma optimize_for_size on
u16 fn_8023715C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x4, 0);
}
#pragma optimize_for_size reset

u32 fn_8023720C(void* ctx, void* param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_80203C5C(void* ctx);
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_80203C5C(param);
    }
    return 0;
}


#pragma optimize_for_size on
u32 fn_80237288(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern short fn_802010C8();
  u16 uVar1;
  u8 cVar3;
  u16 sVar2;
  u32 bVar4;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  cVar3 = fn_801FB1C0(0,uVar1,0x24,0);
  if (cVar3 == 1) {
    sVar2 = fn_802010C8(r4);
    bVar4 = sVar2 != 0;
  }
  else {
    bVar4 = 0;
  }
  return bVar4;
}
#pragma optimize_for_size reset

u32 fn_80237310(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_80122DDC(u8* ptr);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_80122DDC(fn_80205BE8(resolved));
    }
    return 1;
}
