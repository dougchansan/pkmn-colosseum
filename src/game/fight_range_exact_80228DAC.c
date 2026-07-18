/**
 * @file fight_range_exact_80228DAC.c
 * @brief Exact pure-C fight status-check island, 0x80228DAC - 0x80229704.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u32 fn_80077AF4(void);

#pragma opt_propagation reset
#define fn_8011BEB4 wazaGetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_802096E8 fightWazaIsHit
u32 fn_80228DAC(u32 r3, u32 r4, u32 r5)

{
    extern u16 fn_8011BEB4();
    extern u16 fn_801F0134();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u16 fn_80201D84();
    extern u8 fn_802026E4();
    extern u8 fn_802096E8();
    extern int fn_8022DCB8();
    extern u32 lbl_8047B618;
  u8 bVar1;
  u16 uVar8;
  u16 sVar9;
  u32 lateMoveType;
  u8 cVar11;
  u16 sVar10;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;

  uVar8 = fn_801F54A4(0,0,0x14,0);
  fn_8011BEB4(0,r5,9,0);
  fn_801F453C(0,1);
  sVar9 = fn_801F0134(r3,uVar8);
  cVar11 = fn_802026E4(r4,0x1d);
  if (cVar11 != 1) {
    goto first_check_done;
  }
  sVar10 = fn_80201D84(r4,0x1d);
  if (sVar10 == sVar9) {
    void* savedAttacker;
    savedAttacker = (void*)fn_801F025C(0x11,0);
    uVar3 = (0,(int)fn_8012640C(savedAttacker,0,0xd9,0));
    uVar4 = (0,fn_801F025C(0x12,0));
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar5 = fn_801F025C(2,fn_801F025C(0x11,0));
      uVar2 = 0;
      uVar2 += fn_801F025C(0x12,0);
      uVar3 = fn_801F025C(2,uVar2);
      cVar11 = fn_802026E4(uVar2,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar3)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8((u32)savedAttacker,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
first_check_done:
  if (((lbl_8047B618 & 0x10000) == 0) &&
     (cVar11 = fn_802026E4(r4,0x1f), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = 0;
    uVar4 += fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar3 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar3);
      uVar3 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar3);
      cVar11 = fn_802026E4(uVar3,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  uVar7 = lbl_8047B618;
  lbl_8047B618 = uVar7 & 0xfffeffff;
  if (((uVar7 & 0x20000) == 0) && (cVar11 = fn_802026E4(r4,0x20), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar3 = fn_801F025C(0x11,0);
    uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
    uVar2 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar4);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar2,0,0xf3,0,0);
    fn_801254B4((void*)uVar2,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar4 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar4);
      uVar4 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar4);
      cVar11 = fn_802026E4(uVar4,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar3,uVar2,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  uVar7 = lbl_8047B618;
  lbl_8047B618 = uVar7 & 0xfffdffff;
  if (((uVar7 & 0x40000) == 0) && (cVar11 = fn_802026E4(r4,0x21), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar3 = fn_801F025C(0x11,0);
    uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
    uVar2 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar4);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar2,0,0xf3,0,0);
    fn_801254B4((void*)uVar2,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar4 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar4);
      uVar4 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar4);
      cVar11 = fn_802026E4(uVar4,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar3,uVar2,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 & 0xfffbffff;
  lateMoveType = fn_8011BEB4(0,r5,9,0);
  lateMoveType = (u16)lateMoveType;
  cVar11 = (int)fn_801F453C(0,1);
  if ((cVar11 == 2) && (lateMoveType == 0x98)) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if (bVar1 == 1) {
    uVar3 = fn_801F025C(0x11,0);
    uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
    uVar2 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar4);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar2,0,0xf3,0,0);
    fn_801254B4((void*)uVar2,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar4 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar4);
      uVar4 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar4);
      cVar11 = fn_802026E4(uVar4,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar3,uVar2,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    uVar2 = 1;
  }
  else {
    lateMoveType = fn_8011BEB4(0,r5,9,0);
    if ((lateMoveType == 0x11) || (lateMoveType == 0x4e)) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1 == 1) {
      uVar3 = fn_801F025C(0x11,0);
      uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
      uVar2 = fn_801F025C(0x12,0);
      cVar11 = fn_802096E8(uVar4);
      if (cVar11 == 0) {
    fn_801254B4((void*)uVar2,0,0xf3,0,0);
    fn_801254B4((void*)uVar2,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
      }
      else {
        uVar4 = fn_801F025C(0x11,0);
        uVar5 = fn_801F025C(2,uVar4);
        uVar4 = fn_801F025C(0x12,0);
        uVar6 = fn_801F025C(2,uVar4);
        cVar11 = fn_802026E4(uVar4,0x15);
        if (((cVar11 == 1) && (uVar5 != uVar6)) &&
           ((lbl_8047B618 & 0x1000000) == 0)) {
          lbl_8047B618 = lbl_8047B618 | 0x40;
        }
        cVar11 = fn_8022DCB8(uVar3,uVar2,r5);
        if (cVar11 == 0) {
          lbl_8047B610 = lbl_8047B610 + 7;
        }
      }
      uVar2 = 1;
    }
    else {
      uVar2 = 0;
    }
  }
  return uVar2;
}
#undef fn_8011BEB4
#undef fn_801254B4
#undef fn_8012640C
#undef fn_801F0134
#undef fn_801F025C
#undef fn_801F453C
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_802096E8

#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_8011BEB4 wazaGetStatus
u8 fn_8022967C(u32 param2) {
    extern u16 fn_8011BEB4();
    extern int fn_801F54A4();
    u8 a = (u8)fn_801F54A4(0, 0, 0x34, 0);
    u32 b = fn_80077AF4();
    u16 c = (u16)fn_8011BEB4(0, param2, 9, 0);

    if (a == 1 && (u8)b == 1 && c == 7) {
        return 1;
    }
    return 0;
}
#undef fn_8011BEB4
#undef fn_801F54A4
#pragma optimize_for_size reset
