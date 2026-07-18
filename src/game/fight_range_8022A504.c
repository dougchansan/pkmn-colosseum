/**
 * @file fight_range_8022A504.c
 * @brief Exact pure-C fight island, 0x8022A504 - 0x8022B29C (2 fns).
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern void fn_8022B29C(s32);

#pragma dont_inline on
#pragma optimize_for_size on
u32 fn_8022A504(u32 r3, u32 r4, u32* r5)

{
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 wazaGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fn_802026E4();
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fn_80201890();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightFloorSetStatus();
    extern void fn_80211B94();
    extern void* lbl_8047B62C;
    extern u8 lbl_8037919D[];
    void* context;
    u32* state;
    u32 pokemon;
    u8 moveSlot;
    s32 slot;
    u32 iVar7;
    u32 uVar1;
    u8 cVar4;
    u8 cVar5;
    u16 sVar3;
    s8 bVar6;
    u32 uVar2;

    context = (void*)r3;
    state = r5;
    iVar7 = *state;
    uVar1 = fightOutPokemonGetUseWazaDataId(iVar7);
    cVar4 = wazaGetStatus(0, uVar1, 0x10, 0);
    cVar5 = fightOutPokemonCheckFightOut(context);
    if (cVar5 == 0) {
        return 1;
    }
    cVar5 = fn_802026E4(context, 0x33);
    if ((cVar5 == 1) && (cVar4 == 1)) {
        if ((iVar7 != 0) && (context != 0) &&
            (sVar3 = fightOutPokemonGetTokuseiDataId(iVar7), sVar3 == 0x2e)) {
            pokemon = fightOutPokemonGetPokemonPtr(context);
            bVar6 = pokemonSearchWazaDataId(pokemon, 0x121);
            if (bVar6 >= 0) {
                slot = bVar6;
                cVar4 = pokemonGetStatus(pokemon, 0, 0x80, slot);
                if (cVar4 != 0) {
                    cVar4--;
                }
                pokemonSetStatus((void*)pokemon, 0, 0x80, slot, cVar4);
                moveSlot = bVar6;
                cVar5 = fn_802026E4(context, 0x10);
                if ((cVar5 == 0) &&
                    (cVar5 = fn_802026E4(context, 0x31), cVar5 == 1) &&
                    (uVar2 = fn_80201890(context, 0x31),
                     (uVar2 & (1 << (u32)moveSlot)) == 0) &&
                    (cVar5 = fightOutPokemonIsUseHensinBuff(context), cVar5 == 1)) {
                    fightOutPokemonSetHensinPokemonStatusId(context, 0x80, moveSlot, 0);
                }
            }
        }
        fightOutPokemonWriteJoutaiDataId(context, 0x33);
        fightFloorSetStatus(0, 0, 0x4b, 0, context);
        fn_80211B94(lbl_8047B62C, lbl_8037919D, 0);
        state[1] = (u32)context;
    }
    return 1;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8011CE18 pokemonSeikakuDataBiosGetPtr
#define fn_8011FC74 pokemonIsDarkPokemon
#define fn_80123CD4 pokemonWazaCheckValid
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F8100 fightTrainerGetNamePtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FFEC8 fightOutPokemonCheckCanOutOkWazaBanme
#define fn_80203E0C figthOutPokemonGetLevel
#define fn_80203FE4 fightOutPokemonGetSoubiItemSoubiDataId
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80208404 fightOutPokemonDarkPokemonEffect
#define fn_802085C4 fightOutPokemonWazaEffect
#define fn_802086E8 fightOutPokemonFreeWazaEffect
#define fn_80208750 fightOutPokemonLoadWazaEffect
#define fn_80208ED0 fightOutPokemonModosuEffect
#define fn_802099AC fightWazaCreate
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#pragma opt_lifetimes off
u32 fn_8022A6C8(u32 r3)

{
    extern u16 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_8011CC54();
    extern u32 fn_8011CC6C();
    extern u32 fn_8011CC84();
    extern u32 fn_8011CC9C();
    extern u32 fn_8011CCB4();
    extern u32 fn_8011CCCC();
    extern u32 fn_8011CCE4();
    extern void fn_8011CE18();
    extern u32 fn_8011FC74();
    extern u32 fn_80123CD4();
    extern void fn_80132A38();
    extern u32 fn_80142984();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern int fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801F8100();
    extern int fn_801F8A18();
    extern void fn_801FB1C0();
    extern void fn_801FBA24();
    extern u32 fn_801FFEC8();
    extern u32 fn_802026E4();
    extern void fn_80203E0C();
    extern void fn_80203FE4();
    extern u32 fn_80205B8C();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern void fn_80208404();
    extern void fn_80208554();
    extern void fn_802085C4();
    extern void fn_802086E8();
    extern void fn_80208750();
    extern void fn_80208ED0();
    extern void fn_802099AC(u32, s8, u16, u32, u32);
    extern int fn_8022B2CC();
    extern int fn_8022BE2C();
    extern int fn_80232110();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern u16 lbl_8047B60C;
    extern u32 lbl_8047B618;
    extern u8 lbl_803799ED[];
    extern u8 lbl_803799EF[];
    extern u8 lbl_803799F4[];
    extern u8 lbl_803799FE[];
  u8 bVar1;
  u16 uVar11;
  int iVar5;
  u32 uVar6;
  u8 uVar14;
  s32 uVar17;
  u32 uVar3;
  u32 uVar4;
  u32 uVar7;
  u32 uVar2;
  u32 uVarMove;
  u32 activeMove;
  u32 uVar8;
  u8 cVar15;
  u32 uVar9;
  u32 uVar10;
  u16 local_48;
  short local_44[4];

  uVar11 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_80205B8C(r3);
  uVar3 = fn_801F025C(2,r3);
  uVar4 = (int)fn_8012640C(r3,0,0xd9,0);
  fn_80203E0C(r3);
  iVar5 = fn_801F4354(0,r3);
  if (iVar5 == 0) {
    return 0;
  }
  uVar6 = fn_801F025C(9,iVar5);
  fn_801FB1C0(iVar5,0,0x44,0);
  activeMove = 0;
  activeMove += fn_80205184((void*)r3);
  uVar7 = fightOutPokemonGetSoubiItemDataId(r3);
  fn_80203FE4(r3);
  uVar8 = fn_80205B8C(r3);
  uVar14 = (int)fn_8012640C(uVar8,0,0xbf,0);
  cVar15 = (u8)fn_8011BEB4(uVar4,0,0x32,0);
  if ((cVar15 != 1) && ((u8)fn_801F54A4(0,0,0x31,0) != 0)) {
    fn_80205B8C(r3);
    cVar15 = (u8)fn_8011FC74();
    if ((cVar15 != 0) && ((u8)fn_802026E4(r3,0x3e) != 0) &&
        ((u16)activeMove != 0x164)) {
      fn_80132A38(0x13,fn_801F8100(iVar5));
      uVar17 = fn_800E0C54() % 100;
      fn_8011CE18(uVar14);
      uVarMove = fn_8011CCE4();
      if (uVar17 < (uVarMove & 0xff)) {
        u8 initIndex;
        u32 bVar16;
        u32 sVar12;
        for (initIndex = 0; initIndex < 4; initIndex++) {
          local_44[initIndex] = -1;
        }
        for (bVar16 = 0, sVar12 = 0; (s16)sVar12 < 4; sVar12 = sVar12 + 1) {
          u16 moveId;
          activeMove = sVar12 & 0xffff;
          cVar15 = fn_80123CD4(uVar2,activeMove);
          if ((((cVar15 != 0) && (cVar15 = fn_801FFEC8(r3,activeMove,0,0), cVar15 == 0)) &&
              (moveId = (int)fn_8012640C(uVar2,0,0x7f,activeMove), moveId != 0)) &&
             ((moveId != 0x165 && (moveId != 0x163)))) {
            local_44[(u8)bVar16] = sVar12;
            bVar16 = bVar16 + 1;
          }
        }
        if ((u8)bVar16 != 0) {
          extern u32 fn_8012640C(u32, u32, u32, u16);
          u32 randomValue;
          s32 quotient;
          short selectedSlot;
          randomValue = fn_800E0C54();
          quotient = (s32)(randomValue & 0xffff) / (u8)bVar16;
          selectedSlot = *(short *)((int)local_44 +
                                   (((randomValue & 0xffff) - quotient * (u8)bVar16)
                                    * 2 & 0x1fe));
          if (((selectedSlot >= 0) && (uVar2 = (u16)fn_8012640C(uVar2,0,0x7f,selectedSlot), (u16)uVar2 != 0)) &&
             (((u16)uVar2 != 0x165 && ((u16)uVar2 != 0x163)))) {
            u32 targetOutPokemon;
            u32 flags;
            u32 relativeTarget;
            lbl_8047B60C = (u16)uVar2;
            flags = lbl_8047B618 & 0xfffffbff;
            *(volatile u32*)&lbl_8047B618 = flags;
            *(volatile u32*)&lbl_8047B618 = flags | 0x200000;
            targetOutPokemon = fn_8022B2CC(r3,(u16)uVar2,uVar11,(u32)fn_8022B29C,1,1, (void*)0xffffffff);
            relativeTarget = fn_801F0134(targetOutPokemon, uVar11);
            fn_802099AC(uVar4, selectedSlot, (u16)uVar2, relativeTarget, 0);
            fn_801F4C14(0,0,0x43,0,targetOutPokemon);
            fn_8011BEB4(0,(u16)uVar2,1,0);
            fn_80132A38(0x28,fn_800FA280());
            fn_80208404(r3,0,1,0);
            fn_80208404(r3,0,2,0);
            fn_80208404(r3,0,1,1);
            fn_80208404(r3,0,1,2);
            fn_80208404(r3,0,2,1);
            fn_802624CC(0x770c);
            fn_80208404(r3,0,2,2);
            fn_8026246C();
            fn_80208404(r3,0,1,3);
            fn_80208404(r3,0,2,3);
            lbl_8047B618 = lbl_8047B618 | 0x400;
            lbl_8047B610 = lbl_803799ED;
            return 2;
          }
        }
        goto fallback_7710;
      }
      else {
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CCCC();
        uVarMove = (uVarMove & 0xff) + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
          fn_80208404(r3,0,1,0);
          fn_80208750(r3,1,1,0);
          fn_801FBA24(uVar6,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_802085C4(r3,1,1,0, (void*)0xffffffff);
          fn_802624CC(0x770d);
          fn_80208554(r3,1,1,6);
          fn_8026246C();
          fn_801FBA24(uVar6,1);
          fn_801FBA24(uVar6,2);
          fn_80208404(r3,0,1,3);
          fn_802086E8(r3,1,1);
          fn_801FBA24(uVar6,3);
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CCB4();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
          fn_80208404(r3,0,1,0);
          fn_80208750(r3,1,1,0);
          fn_801FBA24(iVar5,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_802085C4(r3,1,1,0, (void*)0xffffffff);
          fn_802624CC(0x770e);
          fn_80208554(r3,1,1,6);
          fn_8026246C();
          fn_801FBA24(iVar5,1);
          fn_801FBA24(iVar5,2);
          fn_80208404(r3,0,1,3);
          fn_802086E8(r3,1,1);
          fn_801FBA24(iVar5,3);
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC9C();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
          fn_80208404(r3,0,1,0);
          fn_80208404(r3,0,2,0);
          fn_801F4C14(0,0,0x43,0,r3);
          uVar10 = fn_80232110(r3,r3,uVar3,1,0x28, (void*)0xffffffff);
          fn_8011BBD8(uVar4,0,0x2d,0,uVar10);
          fn_801254B4((void*)r3,0,0x107,0,1);
          lbl_8047B618 = lbl_8047B618 | 0x80000;
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_80208404(r3,0,2,1);
          fn_802624CC(0x770f);
          fn_80208404(r3,0,2,2);
          fn_8026246C();
          fn_80208404(r3,0,1,3);
          fn_80208404(r3,0,2,3);
          lbl_8047B610 = lbl_803799EF;
          return 2;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC84();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 >= uVarMove) {
          goto after_fallback_7710;
        }
fallback_7710:
        fn_80208404(r3,0,1,0);
        fn_80208750(r3,0x85,3,0);
        fn_80208404(r3,0,1,1);
        fn_80208404(r3,0,1,2);
        fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
        fn_802624CC(0x7710);
        fn_80208554(r3,0x85,3,6);
        fn_8026246C();
        fn_80208404(r3,0,1,3);
        fn_802086E8(r3,0x85,3);
        lbl_8047B610 = lbl_803799F4;
        return 1;
after_fallback_7710:
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC6C();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
          fn_80208404(r3,0,1,0);
          fn_80208750(r3,0x85,3,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          cVar15 = fn_80142984(uVar7);
          if (cVar15 == 0) {
            fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
            fn_802624CC(0x7712);
            fn_80208554(r3,0x85,3,6);
            fn_8026246C();
          }
          else {
            uVar9 = uVar7 & 0xffff;
            if ((((uVar9 == 0x2c) || ((uVar7 - 0x85 & 0xffff) <= 0xe)) ||
                ((uVar7 - 0xa8 & 0xffff) <= 6)) || ((uVar9 == 0xb4 || (uVar9 == 0xb9)))) {
              bVar1 = 1;
            }
            else {
              bVar1 = 0;
            }
            if ((bVar1 == 1) && (cVar15 = fn_8022BE2C(r3,2), cVar15 != 0)) {
              fn_80208404(r3,0,1,3);
              fn_802086E8(r3,0x85,3);
              lbl_8047B610 = lbl_803799F4;
              return 1;
            }
            fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
            fn_802624CC(0x7711);
            fn_80208554(r3,0x85,3,6);
            fn_8026246C();
          }
          fn_80208404(r3,0,1,3);
          fn_802086E8(r3,0x85,3);
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar7 = fn_8011CC54();
        if (uVar17 < uVarMove + (uVar7 & 0xff)) {
          fn_80208404(r3,0,1,0);
          fn_80208404(r3,0,0,0);
          fn_80208ED0(r3,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          local_48 = 0;
          uVar8 = fn_801F8A18(iVar5,&local_48);
          if (uVar8 != 0) {
            fn_80208ED0(r3,1);
            fn_802624CC(0x7713);
            fn_80208ED0(r3,2);
            fn_8026246C();
            fn_80208404(r3,0,1,3);
            fn_80208404(r3,0,0,3);
            fn_80208ED0(r3,3);
            fn_80208ED0(r3,4);
            lbl_8047B610 = lbl_803799FE;
            return 1;
          }
          fn_80208404(r3,0,0,1);
          fn_802624CC(0x7714);
          fn_80208404(r3,0,0,2);
          fn_8026246C();
          fn_80208404(r3,0,1,3);
          fn_80208404(r3,0,0,3);
          fn_80208ED0(r3,3);
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
      }
    }
  }
  return 0;
}
#pragma opt_lifetimes reset
#undef fn_800FA280
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_8011CE18
#undef fn_8011FC74
#undef fn_80123CD4
#undef fn_801254B4
#undef fn_8012640C
#undef fn_80132A38
#undef fn_801F0134
#undef fn_801F025C
#undef fn_801F4354
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_801F8100
#undef fn_801FB1C0
#undef fn_801FFEC8
#undef fn_80203E0C
#undef fn_80203FE4
#undef fn_80205184
#undef fn_80205B8C
#undef fn_80208404
#undef fn_802085C4
#undef fn_802086E8
#undef fn_80208750
#undef fn_80208ED0
#undef fn_802099AC
#undef fn_8026246C
#undef fn_802624CC
#pragma optimize_for_size reset
