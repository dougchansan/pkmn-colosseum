/**
 * @file fight_range_exact_802279F8.c
 * @brief Exact island 0x802279F8 - 0x80227DB8 (_fightSeqApplyAisyouToDamage, wsCmd05),
 *        moved out of fight_range_80211A00.c in address order.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_80478D78[8];
extern u32 lbl_8047B618;
extern void* lbl_8047B62C;
extern void wsRunScript(void*, void*, u8);

#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_802096E8 fightWazaIsHit
#define fn_802097C8 fightWazaWriteJoutaiDataId
#define fn_8020981C fightWazaCheckWriteJoutaiDataId
#define fn_8020990C fightWazaIsJoutaiDataId
#define fn_80209960 fightWazaInitJoutaiDataId
void _fightSeqApplyAisyouToDamage(u32 r3, u32 r4, u32 r5, u8 r6)

{
    extern void fn_8011BBD8();
    extern int fn_8011BEB4();
    extern s8 fn_802096E8();
    extern void fn_802097C8();
    extern s8 fn_8020981C();
    extern s8 fn_8020990C();
    extern void fn_80209960();
  int iVar1;
  u16 sVar2;
  u8 cVar3;
  u32 uVar4;

  iVar1 = fn_8011BEB4(r3,0,0x2d,0);
  sVar2 = fn_8011BEB4(0,r5,7,0);
  cVar3 = fn_8020990C(r3,0x43);
  if (((cVar3 != 1) || (r6 != 0)) && (uVar4 = r4 & 0xffff, uVar4 != 0x3f)) {
    if (uVar4 == 0x43) {
      uVar4 = 0;
    }
    else if (uVar4 == 0x42) {
      uVar4 = 5;
    }
    else if (uVar4 == 0x41) {
      uVar4 = 0x14;
    }
    else {
      return;
    }
    if (r6 == 1) {
      iVar1 = (int)(iVar1 * (u8)uVar4) / 10;
      if ((iVar1 == 0) && ((u8)uVar4 != 0)) {
        iVar1 = 1;
      }
      fn_8011BBD8(r3,0,0x2d,0,iVar1);
    }
    switch ((u8)uVar4) {
    case 0:
      cVar3 = fn_8020981C(r3,r4);
      if (cVar3 == 2) {
        fn_802097C8(r3,r4,0);
      }
      fn_80209960(r3,0x42);
      fn_80209960(r3,0x41);
      break;
    case 5:
      if ((sVar2 != 0) && (cVar3 = fn_802096E8(r3), cVar3 == 1)) {
        cVar3 = fn_8020990C(r3,0x41);
        if (cVar3 == 1) {
          fn_80209960(r3,0x41);
        }
        else {
          cVar3 = fn_8020981C(r3,r4);
          if (cVar3 == 2) {
            fn_802097C8(r3,r4,0);
          }
        }
      }
      break;
    case 0x14:
      if ((sVar2 != 0) && (cVar3 = fn_802096E8(r3), cVar3 == 1)) {
        cVar3 = fn_8020990C(r3,0x42);
        if (cVar3 == 1) {
          fn_80209960(r3,0x42);
        }
        else {
          cVar3 = fn_8020981C(r3,r4);
          if (cVar3 == 2) {
            fn_802097C8(r3,r4,0);
          }
        }
      }
      break;
    }
  }
  return;
}

/*
 * wsCmd05 (0x80227C40)
 *
 * Move-power modifier: derives a base power via fightSeqCalcDamage(ctx1, ctx2,
 * sub, moveId, val2f, val30) then scales it by the field-0xD9 object's
 * 0x2b/0x2c byte multipliers; doubles it when flag 0x24 is set and
 * val30==0xd, then applies a 1.5x (*15/10) scale when flag 0x32 is
 * set. Result stored to field 0x2d. PC always advances by 1.
 */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void wsCmd05(void) {
    extern u32 fn_80205184();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_8011BEB4();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    u32 ctx2;
    u32 ctx1;
    u32 sub;
    u32 moveId;
    u16 val2f;
    u16 val30;
    u32 fieldD9;
    s32 power;
    u8 mult1;
    u8 mult2;

    ctx1 = fn_801F025C(0x11, 0);
    ctx2 = fn_801F025C(0x12, 0);
    sub = fn_801F025C(2, ctx2);
    moveId = fn_80205184(ctx1);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    val2f = (u16)fn_8011BEB4(fieldD9, 0, 0x2f, 0);
    val30 = (u16)fn_8011BEB4(fieldD9, 0, 0x30, 0);
    power = fightSeqCalcDamage(ctx1, ctx2, sub, moveId, val2f, val30);
    mult1 = (u8)fn_8011BEB4(fieldD9, 0, 0x2b, 0);

    power = power * mult1;
    mult2 = (u8)fn_8011BEB4(fieldD9, 0, 0x2c, 0);
    power = power * mult2;

    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(ctx1, 0x24) == 1 && val30 == 0xd) {
        power = power * 2;
    }
    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(ctx1, 0x32) == 1) {
        power = power * 15 / 10;
    }
    fn_8011BBD8(fieldD9, 0, 0x2d, 0, power);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_80205184
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_8011BBD8
