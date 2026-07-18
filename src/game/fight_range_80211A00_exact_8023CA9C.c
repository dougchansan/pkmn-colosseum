/**
 * @file fight_range_80211A00_exact_8023CA9C.c
 * @brief Strict target-order suffix, 0x8023CA9C - 0x802405C0 (33 fns).
 *
 * Materialized from clean blob 754061cf; executable owner bodies are
 * byte-for-byte source extracts and compiler state is re-established per
 * original owner context.
 */
#include "dolphin/types.h"

extern f32 lbl_8047E630;
extern u16 fn_800E0C54(void);
extern int fn_802373B0(u32, u32, int, f32);

/* 0x8023CA9C size 0xC4: fn_8023CA9C */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_8011BEB4 wazaGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8024E52C fightTrainerAiWazaValueNull
s32 fn_8023CA9C(void* ctx, u32 param1, u32 param2, u32 param3) {
    typedef s32 (*Handler)();
    extern Handler fn_8011BEB4(u32, u32, u32, u32);
    extern u32 fn_801FB1C0(void*, u32, u32, u32);
    extern s32 fn_8024E52C();
    Handler fp;
    u16 v;
    u8 v2;

    v = (u16)fn_801FB1C0(ctx, 0, 0x43, 0);
    v = (u16)fn_801FB1C0(0, v, 2, 0);
    v2 = (u8)fn_801FB1C0(0, v, 0x32, 0);
    if (v2 != 1) {
        goto ret0;
    }
    fp = fn_8011BEB4(0, param2, 0x1c, 0);
    if (fp != NULL) {
        goto call;
    }
    fp = fn_8024E52C;
    goto call;
ret0:
    return 0;
call:
    return fp(ctx, param1, param2, param3);
}
#undef fn_8024E52C
#undef fn_801FB1C0
#undef fn_8011BEB4

/* 0x8023CB60 size 0x140: fightTrainerAiWazaValueYumekui */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801F1990 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_802399FC fightTrainerAiAddValue
u32 fightTrainerAiWazaValueYumekui(u32 r3, u32 r4, u32 r5, u32 r6)

{
    extern u8 fn_801F1990();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_802376EC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
    extern int fn_8023C370();
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar5;
  u8 cVar6;

  uVar1 = fn_802376EC();
  uVar2 = fn_8023C370(r3,r4,r5,r6,1);
  iVar3 = fn_801FB1C0(0,0x223,0x3e,0);
  uVar2 = ((int)(((int)uVar2 / 2) * 100) /
          (int)(uVar1 & 0xffff)) / iVar3;
  {
    u32 callResult = fn_802399FC(0,uVar2);
    uVar1 = callResult;
  }
  uVar5 = fn_80205B8C(r4);
  fn_80239CCC(0xec64,r3,uVar5,0,0,r5,0,0x223,uVar2);
  cVar6 = fn_801F1990(0,r3,1,1,0x10e,r4);
  if (cVar6 == 1) {
    {
      u32 callResult = fn_80239984(uVar1,r3,0x224);
      uVar1 = callResult;
    }
    uVar5 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar5,0,0,r5,0,0x224);
  }
  return uVar1;
}
#undef fn_802399FC
#undef fn_80205B8C
#undef fn_801FB1C0
#undef fn_801F1990

/* 0x8023CCA0 size 0x12C: fightTrainerAiWazaValueKawarawari */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_801F1990 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut
u32 fightTrainerAiWazaValueKawarawari(u32 r3, u32 r4, u32 r5, u32 r6)

{
    extern u32 fn_801F025C();
    extern u8 fn_801F1990();
    extern u8 fn_801F6E98();
    extern u32 fn_80205B8C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u32 uVar3;
  u8 cVar2;
  u32 uVar1;

  uVar3 = 0;
  uVar1 = fn_801F025C(2,r6);
  cVar2 = fn_801F6E98(uVar1,0x49);
  if ((cVar2 == 1) || (cVar2 = fn_801F6E98(uVar1,0x48), cVar2 == 1)) {
    uVar3 = fn_80239984(0,r3,0x221);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x221);
  }
  cVar2 = fn_801F1990(0,r3,1,1,0x10e,r4);
  if (cVar2 == 1) {
    uVar3 = fn_80239984(uVar3,r3,0x222);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x222);
  }
  return uVar3;
}
#undef fn_801F1990
#undef fn_80205B8C
#undef fn_801F6E98
#undef fn_801F025C

/* 0x8023CDCC size 0x94: fightTrainerAiWazaValueTeiryouDameeji */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueTeiryouDameeji(u32 arg0, u32 arg1, u32 arg2, u32 arg3)

{
    extern u32 fn_80205B8C();
    extern u16 fn_802377E8();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  if (fn_802377E8(arg0, arg3) == 0xd5) {
    uVar3 = fn_80239984(0, arg0, 0x220);
    uVar1 = fn_80205B8C(arg1);
    fn_80239EE8(0xec64, arg0, uVar1, 0, 0, arg2, 0, 0x220);
  }
  return uVar3;
}
#undef fn_80205B8C

/* 0x8023CE60 size 0x17C: fightTrainerAiWazaValueHikarinokabe */
#pragma optimize_for_size reset
#pragma optimization_level 2
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueHikarinokabe(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80205B8C();
    extern u16 fn_80236520();
    extern u16 fn_80236FFC();
    extern u16 fn_8023715C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern int fn_8023C530();
  u32 uVar6;
  u16 uVar2;
  u16 uVar3;
  u16 sVar4;
  u8 cVar5;
  u32 uVar1;

  uVar6 = 0;
  uVar2 = fn_80236FFC(r3,r6);
  uVar3 = fn_8023715C(r3,r6);
  sVar4 = fn_80236520(r3,r6);
  cVar5 = fn_8023C530(r3,r4,r5,r6);
  if (cVar5 == 1) {
    uVar6 = fn_80239984(0,r3,0x21d);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21d);
  }
  if (uVar3 < uVar2) {
    uVar6 = fn_80239984(uVar6,r3,0x21e);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21e);
  }
  if (sVar4 == 0x118) {
    uVar6 = fn_80239984(uVar6,r3,0x21f);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21f);
  }
  return uVar6;
}

/* 0x8023CFDC size 0x17C: fightTrainerAiWazaValueRihurekutaa */
u32 fightTrainerAiWazaValueRihurekutaa(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80205B8C();
    extern u16 fn_80236520();
    extern u16 fn_80236FFC();
    extern u16 fn_8023715C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern int fn_8023C530();
  u16 uVar2;
  u16 uVar3;
  u16 sVar4;
  u8 cVar5;
  u32 uVar1;
  u32 uVar6;

  uVar6 = 0;
  uVar2 = fn_80236FFC(r3,r6);
  uVar3 = fn_8023715C(r3,r6);
  sVar4 = fn_80236520(r3,r6);
  cVar5 = fn_8023C530(r3,r4,r5,r6);
  if (cVar5 == 1) {
    uVar6 = fn_80239984(0,r3,0x21a);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21a);
  }
  if (uVar3 > uVar2) {
    uVar6 = fn_80239984(uVar6,r3,0x21b);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21b);
  }
  if (sVar4 == 0x118) {
    uVar6 = fn_80239984(uVar6,r3,0x21c);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21c);
  }
  return uVar6;
}
#undef fn_80205B8C

/* 0x8023D158 size 0x1CC: fightTrainerAiWazaValueOnibi */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueOnibi(void* ctx, u32 param1, u32 param2, u32 param3)
{
    extern u16 fn_80236FFC(void*, u32);
    extern u16 fn_8023715C(void*, u32);
    extern u16 fn_80236520(void*, u32);
    extern u16 fn_801F1C18(u32, void*, void*, u32, u32);
    extern u16 fn_802377E8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[8];
    u32 handle = 0;
    u16 a = fn_80236FFC(ctx, param3);
    u16 b = fn_8023715C(ctx, param3);
    u16 c = fn_80236520(ctx, param3);
    u16 count;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 0, 1);
    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, arr[i]);
        if (v == 0xca || v == 0x168 || v == 0x12f || v == 0xd5) {
            handle = fn_80239984(0, ctx, 0x217);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x217);
            break;
        }
    }
    if (b > a) {
        handle = fn_80239984(handle, ctx, 0x218);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x218);
    }
    if (c == 0x11f) {
        handle = fn_80239984(handle, ctx, 0x219);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x219);
    }
    return handle;
}
#undef fn_80205B8C
#undef fn_801F1C18

/* 0x8023D324 size 0x15C: fightTrainerAiWazaValueDoku */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueDoku(void* ctx, u32 param1, u32 param2, u32 param3)
{
    extern u16 fn_801F1C18(u32, void*, void*, u32, u32);
    extern u16 fn_80236520(void*, u32);
    extern u16 fn_802377E8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[10];
    u32 handle = 0;
    u16 count;
    u16 c;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 0, 1);
    c = fn_80236520(ctx, param3);
    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, arr[i]);
        if (v == 0xca || v == 0x168 || v == 0x12f || v == 0xd5) {
            handle = fn_80239984(0, ctx, 0x215);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x215);
            break;
        }
    }
    if (c == 0x11f) {
        handle = fn_80239984(handle, ctx, 0x216);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x216);
    }
    return handle;
}
#undef fn_80205B8C
#undef fn_801F1C18

/* 0x8023D480 size 0x90: fightTrainerAiWazaValueBatontatti */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueBatontatti(u32 r3, u32 r4, u32 r5)

{
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u8 cVar2;
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  cVar2 = fn_80235714();
  if (cVar2 == 1) {
    uVar3 = fn_80239984(0,r3,0x214);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x214);
  }
  return uVar3;
}
#undef fn_80205B8C

/* Non-prototype declarations visible to the original late AI block. */
extern u8 fn_80235714();
extern u32 fn_802367CC();
extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
extern u32 fightOutPokemonGetPokemonPtr();
extern u32 fn_80239984();
extern u8 fn_80239EE8();
extern u8 fn_80239CCC();
extern s32 fightTrainerAiAddValue();

/* 0x8023D510 size 0x23C: fightTrainerAiWazaValueHaradaiko */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
u32 fightTrainerAiWazaValueHaradaiko(u32 ctx, u32 poke, u32 msgArg) {
    u32 acc = 0;
    u16 scanBuf[10];
    u32 stackArr[8];
    u16 count16;
    u32* stackPtr;
    u16 i;
    u32 count;
    u8 found;
    u32 result;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x210);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x210);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    stackPtr = stackArr;
    count16 = (u16)count;
    for (i = 0; i < count16; i++) {
        if (stackPtr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackPtr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_haradaiko;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_haradaiko:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x211);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x211);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x212);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x212);
    }

    result = acc;
    if ((u8)fn_802373B0(ctx, poke, -1, lbl_8047E630) == 1) {
        result = fn_80239984(acc, ctx, 0x213);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x213);
    }

    return result;
}

/* 0x8023D74C size 0x200: fightTrainerAiWazaValueMarukunaru */
#pragma optimization_level 4
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueMarukunaru(u32 r3, u32 r4, u32 r5)

{
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u32 fn_80235A3C();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 uVar4;
  int uVar1;
  u8 cVar5;
  u32 uVar2;
  int iVar3;
  u16 uVar6;
  struct {
    u32 value;
  } uVar7;
  u16 local_30 [12];

  uVar4 = fn_802367CC(r3,r4,local_30,0,1);
  uVar1 = fn_80235A3C(r3,r4);
  uVar7.value = 0;
  cVar5 = fn_80235714(r3,r4);
  if (cVar5 == 0) {
    uVar7.value = fn_80239984(0,r3,0x20c);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20c);
  }
  cVar5 = fn_80235714(r3,r4);
  if (cVar5 == 1) {
    uVar7.value = fn_80239984(uVar7.value,r3,0x20e);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20e);
  }
  uVar1 = (uVar1 & 0xff) - 6;
  if (uVar1 < 0) {
    uVar1 = 0;
  }
  iVar3 = fightTrainerGetStatus(0,0x20f,0x3e,0);
  uVar1 = uVar1 * iVar3;
  uVar7.value = fightTrainerAiAddValue(uVar7.value,uVar1);
  uVar2 = fn_80205B8C(r4);
  fn_80239CCC(0xec64,r3,uVar2,0,0,r5,0,0x20f,uVar1);
  for (uVar6 = 0; uVar6 < (u16)uVar4; uVar6++) {
    if ((local_30[uVar6] == 0xcd) || (local_30[uVar6] == 0x12d)) {
      {
        u32 callResult = fn_80239984(uVar7.value,r3,0x20d);
        uVar7.value = callResult;
      }
      uVar2 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20d);
      break;
    }
  }
  return uVar7.value;
}
#undef fn_80205B8C

/* Earlier helper definitions supplied these return types in the original TU. */
extern u8 fn_80235910(u32, u32);
extern u8 fn_80235974(u32, u32);
extern u8 fn_802359D8(u32, u32);
extern u8 fn_80235A3C(u32, u32);
extern u8 fn_80235AA0(u32, u32);
extern u8 fn_802357CC(u32, u32);

/* 0x8023D94C size 0x24C: fightTrainerAiWazaValueMeisou */
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma scheduling on
#pragma dont_inline on
#pragma opt_lifetimes reset
u32 fightTrainerAiWazaValueMeisou(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235974(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x208);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x208);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan;
                    }
                }
            }
        }
    }
    found = 0;
after_scan:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x209);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x209);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x20a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x20a);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x20b, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x20b, count2);
        return result;
    }
}

/* 0x8023DB98 size 0x24C: fightTrainerAiWazaValueRyuunomai */
u32 fightTrainerAiWazaValueRyuunomai(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235910(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x204);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x204);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_ryuunomai;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_ryuunomai:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x205);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x205);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x206);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x206);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x207, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x207, count2);
        return result;
    }
}

/* 0x8023DDE4 size 0x24C: fightTrainerAiWazaValueBirudoAppu */
u32 fightTrainerAiWazaValueBirudoAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235AA0(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x200);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x200);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_birudoappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_birudoappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x201);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x201);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x202);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x202);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x203, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x203, count2);
        return result;
    }
}

/* 0x8023E030 size 0x24C: fightTrainerAiWazaValueKosumopawaa */
u32 fightTrainerAiWazaValueKosumopawaa(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235974(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fc);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fc);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kosumopawaa;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kosumopawaa:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fd);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fd);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fe);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fe);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1ff, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ff, count2);
        return result;
    }
}

/* 0x8023E27C size 0x24C: fightTrainerAiWazaValueKaihirituAppu */
u32 fightTrainerAiWazaValueKaihirituAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_802357CC(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f8);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kaihirituappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kaihirituappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f9);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fa);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fa);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1fb, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fb, count2);
        return result;
    }
}

/* 0x8023E4C8 size 0x24C: fightTrainerAiWazaValueDowasure */
u32 fightTrainerAiWazaValueDowasure(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235974(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f4);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_dowasure;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_dowasure:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f5);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f6);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1f7, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f7, count2);
        return result;
    }
}

/* 0x8023E714 size 0x24C: fightTrainerAiWazaValueTokukouAppu */
u32 fightTrainerAiWazaValueTokukouAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_802359D8(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f0);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_tokukouappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_tokukouappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f1);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f2);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1f3, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f3, count2);
        return result;
    }
}

/* 0x8023E960 size 0x24C: fightTrainerAiWazaValueKousokuidou */
u32 fightTrainerAiWazaValueKousokuidou(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235910(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ec);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ec);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kousokuidou;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kousokuidou:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ed);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ed);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ee);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ee);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1ef, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ef, count2);
        return result;
    }
}

/* 0x8023EBAC size 0x24C: fightTrainerAiWazaValueBougyoAppu */
u32 fightTrainerAiWazaValueBougyoAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235A3C(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e8);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_bougyoappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_bougyoappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e9);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ea);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ea);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1eb, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1eb, count2);
        return result;
    }
}

/* 0x8023EDF8 size 0x24C: fightTrainerAiWazaValueKougekiAppu */
u32 fightTrainerAiWazaValueKougekiAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235AA0(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e4);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kougekiappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kougekiappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e5);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e6);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1e7, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e7, count2);
        return result;
    }
}

/* 0x8023F044 size 0x100: fightTrainerAiWazaValuetorikku */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValuetorikku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_8023831C(void*);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80205B8C();
    u32 handle = 0;
    u16 v = fn_8023831C(ctx);

    if (v == 0x1d || v == 0x18) {
        handle = fn_80239984(0, ctx, 0x1e2);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e2);
    }
    if (fn_80237F74(ctx, param3, 0x3c) == 1) {
        handle = fn_80239984(handle, ctx, 0x1e3);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e3);
    }
    return handle;
}
#undef fn_80205B8C

/* 0x8023F144 size 0x134: fightTrainerAiWazaValueSukirusuwappu */
#pragma optimization_level 4
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueSukirusuwappu(u32 arg0, u32 arg1, u32 arg2, u32 arg3)

{
    extern u32 fn_80205B8C();
    extern u8 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u8 cVar2;
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  cVar2 = fn_80237F74(arg0,arg1,0x36);
  if ((cVar2 == 1) || (cVar2 = fn_80237F74(arg0,arg1,0x10), cVar2 == 1)) {
    uVar3 = fn_80239984(0,arg0,0x1e0);
    uVar1 = fn_80205B8C(arg1);
    fn_80239EE8(0xec64,arg0,uVar1,0,0,arg2,0,0x1e0);
  }
  cVar2 = fn_80237F74(arg0,arg3,0x36);
  if ((cVar2 == 1) || (cVar2 = fn_80237F74(arg0,arg3,0x10), cVar2 == 1)) {
    uVar3 = fn_80239984(uVar3,arg0,0x1e1);
    uVar1 = fn_80205B8C(arg1);
    fn_80239EE8(0xec64,arg0,uVar1,0,0,arg2,0,0x1e1);
  }
  return uVar3;
}
#undef fn_80205B8C

extern u32* lbl_80478DF8;
extern int wazaGetStatus();
extern int pokemonGetStatus();
extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
extern s32 fightTrainerAiWazaValueJisin();
extern s32 fightTrainerAiWazaValueJibaku();
extern s32 fightTrainerAiWazaValueNull();
extern u8 fn_8023943C(int, int, int);

/* 0x8023F278 size 0x648: fightTrainerAiWazaValueMamoru */
u32 fightTrainerAiWazaValueMamoru(u32 ctx, u32 poke, u32 msgArg,
                                  u32 otherPoke)
{
    typedef s32 (*WazaValueFunc)();
    u32 party[8];
    u16 moveBuf[10];
    u32 moveCount;
    u32 partyCount;
    u32 finalMoveCount;
    u32 score;
    u16 i;
    u16 j;
    u32 waza;
    WazaValueFunc valueFunc;
    u8 allyJisin;
    u8 allyJibaku;
    u8 queuedJisin;
    u8 queuedJibaku;

    score = 0;
    allyJisin = 0;
    allyJibaku = 0;
    queuedJisin = 0;
    queuedJibaku = 0;

    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(
        0, ctx, party, 1, 1);
    waza = fn_80236520((void*)ctx, poke);
    fn_80236520((void*)ctx, otherPoke);

    for (i = 0; i < (u16)partyCount; i++) {
        if (party[i] == poke) {
            continue;
        }
        moveCount = (u16)fn_802367CC((void*)ctx, party[i], moveBuf, 0, 1);
        if (moveCount == 0) {
            continue;
        }
        for (j = 0; j < moveCount; j++) {
            valueFunc = (WazaValueFunc)wazaGetStatus(0, moveBuf[j], 0x1C, 0);
            if (valueFunc == 0) {
                valueFunc = fightTrainerAiWazaValueNull;
            }
            if (valueFunc == fightTrainerAiWazaValueJisin) {
                allyJisin = 1;
            }
            if (valueFunc == fightTrainerAiWazaValueJibaku) {
                allyJibaku = 1;
            }
        }
    }

    for (moveCount = 0; (moveCount & 0xFFFF) < *lbl_80478DF8; moveCount++) {
        valueFunc = (WazaValueFunc)wazaGetStatus(0, moveCount, 0x1C, 0);
        if (valueFunc == 0) {
            valueFunc = fightTrainerAiWazaValueNull;
        }
        if (valueFunc == fightTrainerAiWazaValueJisin ||
            valueFunc == fightTrainerAiWazaValueJibaku) {
            if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                    0, ctx, 1, 1, moveCount, 0) == 1) {
                if (valueFunc == fightTrainerAiWazaValueJisin) {
                    queuedJisin = 1;
                }
                if (valueFunc == fightTrainerAiWazaValueJibaku) {
                    queuedJibaku = 1;
                }
            }
        }
    }

    if (allyJisin == 1 || allyJibaku == 1) {
        score = fn_80239984(score, (void*)ctx, 0x1D7);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1D7);
    }
    if (queuedJisin == 1 || queuedJibaku == 1) {
        score = fn_80239984(score, (void*)ctx, 0x1D8);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1D8);
    }

    if ((u8)fn_80237F74((void*)ctx, poke, 3) == 1 &&
        fn_8023943C(ctx, waza, 4) == 1) {
        score = fn_80239984(score, (void*)ctx, 0x1D9);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1D9);
    }

    if (((u16)waza == 0xB6 || (u16)waza == 0xC5) &&
        pokemonGetStatus(poke, 0, 0xFC, 0) != 0) {
        score = fn_80239984(score, (void*)ctx, 0x1DA);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1DA);
    }

    if ((u8)fn_80237DBC(ctx, (void*)poke, 7) == 1) {
        if (allyJibaku == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DB);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DB);
        }
        if (queuedJibaku == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DC);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DC);
        }
    }

    if ((u8)fn_80237DBC(ctx, (void*)poke, 2) == 1 ||
        (u8)fn_80237F74((void*)ctx, poke, 0x1A) == 1 ||
        (u8)fn_80237F74((void*)ctx, poke, 0x19) == 1) {
        if (allyJisin == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DD);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DD);
        }
        if (queuedJisin == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DE);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DE);
        }
    }

    for (i = 0; i < (u16)partyCount; i++) {
        if (party[i] == poke) {
            continue;
        }
        finalMoveCount = fn_802367CC((void*)ctx, party[i], moveBuf, 0, 1);
        if ((u16)finalMoveCount == 0) {
            continue;
        }
        for (j = 0; j < (u16)finalMoveCount; j++) {
            if (moveBuf[j] == 0x10A) {
                score = fn_80239984(score, (void*)ctx, 0x1DF);
                fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke),
                            0, 0, msgArg, 0, 0x1DF);
                break;
            }
        }
        if ((u16)j < (u16)finalMoveCount) {
            break;
        }
    }
    return score;
}

/* 0x8023F8C0 size 0x15C: fightTrainerAiWazaValueTakuwaeru */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueTakuwaeru(void* ctx, u32 param1, u32 param2)
{
    extern s16 fn_80202360(u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    s16 val;
    u32 handle = 0;

    if (fn_80236BFC(ctx, param1, 0x2d) == 1) {
        val = fn_80202360(param1, 0x2d);
    } else {
        val = 0;
    }
    if (val == 0) {
        handle = fn_80239984(0, ctx, 0x1d4);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d4);
    } else if (val == 1) {
        handle = fn_80239984(0, ctx, 0x1d5);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d5);
    } else if (val == 2) {
        handle = fn_80239984(0, ctx, 0x1d6);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d6);
    }
    return handle;
}
#undef fn_80205B8C

/* 0x8023FA1C size 0x140: fightTrainerAiWazaValueJyuuden */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueJyuuden(void* ctx, u32 param1, u32 param2)
{
    extern u16 fn_802367CC(void*, u32, void*, u32, u32);
    extern u16 fn_802395C8(void*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 buf[10];
    u32 handle = 0;
    u16 count;
    u16 i;

    count = fn_802367CC(ctx, param1, buf, 0, 1);
    for (i = 0; i < count; i++) {
        if (buf[i] == 0x10c) {
            continue;
        }
        if (fn_802395C8(ctx, buf[i], param1) == 0xd) {
            handle = fn_80239984(0, ctx, 0x1d2);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d2);
            break;
        }
    }
    if (fn_80236BFC(ctx, param1, 0x24) == 1) {
        handle = fn_80239984(handle, ctx, 0x1d3);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d3);
    }
    return handle;
}
#undef fn_80205B8C

/* 0x8023FB5C size 0x1E8: fightTrainerAiWazaValueNemuriKoudou */
#pragma optimization_level 4
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueNemuriKoudou(u32 r3, u32 r4, u32 r5)

{
    extern int fn_80202108();
    extern s8 fn_80202234();
    extern u32 fn_80205B8C();
    extern u32 fn_80236BFC();
    extern u32 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u8 cVar3;
  u32 uVar1;
  u32 uVar2;
  s8 cVar4;
  s8 cVar6;
  int iVar7;
  int iVar8;
  u32 uVar5;

  uVar5 = 0;
  cVar3 = fn_80236BFC(r3,r4,8);
  if (cVar3 == 1) {
    cVar3 = fn_80236BFC(r3,r4,8);
    if (cVar3 == 0) {
      cVar6 = -1;
    }
    else {
      uVar2 = fn_80237F74(r3,r4,0x30);
      iVar7 = ((u32)__cntlzw(1 - (uVar2 & 0xff)) >> 5) + 1;
      iVar8 = fn_80202108(r4,8);
      iVar7 = iVar8 + iVar7;
      if ((s8)iVar7 >= (cVar4 = fn_80202234(r4,8))) {
        cVar6 = 1;
      }
      else {
        cVar6 = 0;
      }
    }
    if (cVar6 == 0) {
      uVar5 = fn_80239984(0,r3,0x1d0);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d0);
    }
    cVar3 = fn_80236BFC(r3,r4,8);
    if (cVar3 == 0) {
      cVar6 = -1;
    }
    else {
      uVar2 = fn_80237F74(r3,r4,0x30);
      iVar7 = ((u32)__cntlzw(1 - (uVar2 & 0xff)) >> 5) + 1;
      iVar8 = fn_80202108(r4,8);
      iVar7 = iVar8 + iVar7;
      if ((s8)iVar7 >= (cVar4 = fn_80202234(r4,8))) {
        cVar6 = 1;
      }
      else {
        cVar6 = 0;
      }
    }
    if (cVar6 == 1) {
      uVar5 = fn_80239984(uVar5,r3,0x1d1);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d1);
    }
  }
  return uVar5;
}
#undef fn_80205B8C

/* 0x8023FD44 size 0x98: fightTrainerAiWazaValueMigawari */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueMigawari(u32 r3, u32 r4, u32 r5)

{
    extern u32 fn_80205B8C();
    extern u32 fn_802373B0();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern const f32 lbl_8047E634;

  u8 cVar2;
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  cVar2 = fn_802373B0((double)lbl_8047E634,r3,r4,1);
  if (cVar2 == 1) {
    uVar3 = fn_80239984(0,r3,0x1cf);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1cf);
  }
  return uVar3;
}
#undef fn_80205B8C

/* 0x8023FDDC size 0x2FC: fightTrainerAiWazaValueSawagu */
#pragma optimization_level 4
#pragma optimize_for_size reset
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes off
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueSawagu(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_801F1C18();
    extern int fn_80202108();
    extern s8 fn_80202234();
    extern u32 fn_80205B8C();
    extern u32 fn_802367CC();
    extern u8 fn_80236BFC();
    extern u8 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u32 uVar3;
  u32 uVar4;
  s8 cVar5;
  u8 cVar11;
  u32 uVar1;
  s8 cVar6;
  u32 uVar2;
  int iVar8;
  int iVar7;
  u32 uVar10;
  u32 uVar9;
  u32 uVar8;
  u32 local_54 [8];
  u16 local_68 [10];

  uVar9 = 0;
  uVar3 = fn_801F1C18(0,r3,local_54,1,1);
  for (uVar10 = 0; (u16)uVar10 < (u16)uVar3; uVar10 = uVar10 + 1) {
    iVar7 = local_54[(u16)uVar10];
    if (r4 != iVar7) {
      cVar11 = fn_80236BFC(r3,iVar7,8);
      if (cVar11 == 0) {
        cVar5 = -1;
      }
      else {
        uVar1 = fn_80237F74(r3,iVar7,0x30);
        uVar8 = ((u32)__cntlzw(1 - (uVar1 & 0xff)) >> 5) + 1;
        iVar8 = fn_80202108(iVar7,8);
        uVar8 = iVar8 + uVar8;
        if ((s8)uVar8 >= (cVar6 = fn_80202234(iVar7,8))) {
          cVar5 = 1;
        }
        else {
          cVar5 = 0;
        }
      }
      if (cVar5 == 0) {
        uVar9 = fn_80239984(0,r3,0x1cc);
        uVar2 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1cc);
        break;
      }
    }
  }
  for (uVar10 = 0; (u16)uVar10 < (u16)uVar3; uVar10 = uVar10 + 1) {
    if (r4 != local_54[(u16)uVar10]) {
      uVar4 = fn_802367CC(r3,local_54[(u16)uVar10],local_68,0,1);
      if ((u16)uVar4 != 0) {
        for (uVar8 = 0; (u16)uVar8 < (u16)uVar4; uVar8 = uVar8 + 1) {
          if (local_68[(u16)uVar8] == 0x9c) {
            uVar9 = fn_80239984(uVar9,r3,0x1cd);
            uVar2 = fn_80205B8C(r4);
            fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1cd);
            break;
          }
        }
        if ((u16)uVar8 < (u16)uVar4) break;
      }
    }
  }
  cVar11 = fn_80236BFC(r3,r6,8);
  if (cVar11 == 0) {
    cVar5 = -1;
  }
  else {
    uVar1 = fn_80237F74(r3,r6,0x30);
    uVar8 = ((u32)__cntlzw(1 - (uVar1 & 0xff)) >> 5) + 1;
    iVar8 = fn_80202108(r6,8);
    uVar4 = iVar8;
    uVar4 += uVar8;
    if ((s8)uVar4 >= (cVar6 = fn_80202234(r6,8))) {
      cVar5 = 1;
    }
    else {
      cVar5 = 0;
    }
  }
  if (cVar5 == 0) {
    uVar9 = fn_80239984(uVar9,r3,0x1ce);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1ce);
  }
  return uVar9;
}
#undef fn_80205B8C
#undef fn_801F1C18

/* 0x802400D8 size 0x6C: fightTrainerAiWazaValueOomugaesi */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_8025CB3C fightTrainerAiCheckOumu
u32 fightTrainerAiWazaValueOomugaesi(void* ctx, u32 slot, u16 species, u32 extra) {
    extern u32 fn_8023CA9C();
    extern u32 fn_8025CB3C();
    u32 currentSpecies;
    u32 result;
    currentSpecies = fn_8025CB3C(ctx);
    if ((u16)currentSpecies == species) {
        goto ret_zero;
    }
    if ((u16)currentSpecies == 0) {
        goto ret_zero;
    }
    result = fn_8023CA9C(ctx, slot, currentSpecies, extra);
    goto done;
ret_zero:
    result = 0;
done:
    return result;
}
#undef fn_8025CB3C

/* 0x80240144 size 0xAC: fightTrainerAiWazaValueRandamuSentaku */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
s32 fightTrainerAiWazaValueRandamuSentaku(void* ctx, u32 param1, u32 param2) {
    extern s32 fn_801FB1C0(u32, u32, u32, u32);
    extern u32 fn_80205B8C();
    extern u32 fightTrainerAiAddValue(u32, s32);
    s32 handle;
    s32 scale;
    s32 mod;
    u16 rng;

    scale = fn_801FB1C0(0, 0x1cb, 0x3e, 0);
    rng = fn_800E0C54();
    mod = rng % (scale + 1);
    handle = fightTrainerAiAddValue(0, mod);
    fn_80239CCC(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1cb, mod);
    return handle;
}
#undef fn_80205B8C
#undef fn_801FB1C0

/* 0x802401F0 size 0x264: fightTrainerAiWazaValueHurahuradansu */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_801F54A4 fightFloorGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueHurahuradansu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_801F1C18(u32, void*, u32*, u32, u32);
    extern u16 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_8023831C(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[10];
    u32 handle = 0;
    u32 count;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 1, 1);
    if (fn_801F54A4(0, 0, 0x18, 0) >= 2) {
        handle = fn_80239984(0, ctx, 0x1c7);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c7);
    }
    for (i = 0; i < (u16)count; i++) {
        if (param1 != arr[i]) {
            u16 v = fn_8023831C(ctx, arr[i]);
            if (v == 8 || v == 9) {
                handle = fn_80239984(handle, ctx, 0x1c8);
                fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c8);
                break;
            }
        }
    }
    for (i = 0; i < (u16)count; i++) {
        if (arr[i] != param1 && fn_80237F74(ctx, arr[i], 0x14) == 1) {
            handle = fn_80239984(handle, ctx, 0x1c9);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c9);
            break;
        }
    }
    for (i = 0; i < (u16)count; i++) {
        if (arr[i] != param1) {
            handle = fn_80239984(handle, ctx, 0x1ca);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1ca);
            break;
        }
    }
    return handle;
}
#undef fn_80205B8C
#undef fn_801F54A4
#undef fn_801F1C18

/* 0x80240454 size 0x16C: fightTrainerAiWazaValueMakibisi */
#pragma optimization_level 4
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline on
#pragma opt_lifetimes reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6D9C fightSideGetCountAsJoutaiDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueMakibisi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void* fn_801F025C();
    extern u32 fn_80205B8C();
    extern u8 fn_801F6E98(void*, u32);
    extern s16 fn_801F6D9C(void*, u32);
    u32 handle = 0;
    void* obj = fn_801F025C(2, param3);
    s16 val;

    if (fn_801F6E98(obj, 0x4a) == 1) {
        val = fn_801F6D9C(obj, 0x4a);
    } else {
        val = 0;
    }
    if (val == 0) {
        handle = fn_80239984(0, ctx, 0x1c4);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c4);
    } else if (val == 1) {
        handle = fn_80239984(0, ctx, 0x1c5);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c5);
    } else if (val == 2) {
        handle = fn_80239984(0, ctx, 0x1c6);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c6);
    }
    return handle;
}
#undef fn_80205B8C
#undef fn_801F6D9C
#undef fn_801F6E98
#undef fn_801F025C

#pragma opt_lifetimes reset
#pragma scheduling reset
#pragma dont_inline reset
#pragma optimize_for_size reset
#pragma optimization_level reset
