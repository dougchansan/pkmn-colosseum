/**
 * @file fight_out_pokemon_suffix_8020A8E0.c
 * @brief fightOutPokemon + fightPokemon final suffix, address range
 *        0x8020A8E0-0x8020AE30, 1 function.
 *
 * OutPokemon/Pokemon field accessors, sequence/status writers, and
 * damage-calc support the seq/waza layers call into (statusGetStatus,
 * fadeEffectGetRandom callers, etc). Corresponds to XD's
 * fight.cpp fightOutPokemon+fightPokemon cluster (0x80200644-0x80208288).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

typedef struct StatusIdTable7 {
    u16 id[7];
} StatusIdTable7;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */
/* Address: 0x8020A8E0 | Size: 0x424 | Ghidra import */

int fn_8020A8E0(u32 r3,u32 r4)

{
    extern int _fadeEffectGetRandom__FUl();
    extern u32 fn_80135E44();
    extern u32 fightTargetDataBiosGetStatusKid();
    extern void fightTargetDataBiosGetPtr();
    extern int fightTargetGetPtr();
    extern u32 fightFloorGetStatus();
    extern u32 fn_8020A500();
    extern u32 fn_8020A540();
    extern u32 fn_8020A580();
    extern s16 fn_8020A5C0();
    extern s16 fn_8020A630();
    extern u8 fn_8020A6A0();
    extern u16 fn_8020A710();
    extern u16 fn_8020A780();
    extern u8 fn_8020A7F0();
    extern u16 fn_8020A860();
    extern u8 fn_8020A8A0();
  u32 uVar1;
  int iVar2;
  u32 uVar3;
  u8 bVar12;
  u32 uVar4;
  u32 uVar5;
  short sVar9;
  short sVar10;
  s8 cVar13;
  u16 uVar11;
  int iVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar14;
  u32 local_48 [5];

  bVar14 = 0;
  do {
    if (1 < bVar14) {
      uVar1 = fn_8020A8A0(r3);
      if ((uVar1 & 0xff) < 7) {

        iVar2 = ((int (*)(void))**(void ***)((uVar1 & 0xff) * 4 + -0x7fc8a6ac))();
        return iVar2;
      }
      iVar2 = 0;
      uVar1 = fn_8020A860(r3);
      if ((uVar1 & 0xffff) == 0) {
        iVar2 = 0;
      }
      else {
        do {
          uVar3 = fn_8020A540(uVar1);
          for (bVar14 = 0; bVar14 < 2; bVar14 = bVar14 + 1) {
            uVar8 = 0;
            bVar12 = fn_8020A7F0(uVar3,bVar14);
            uVar4 = fn_8020A780(uVar3,bVar14);
            uVar5 = fn_8020A710(uVar3,bVar14);
            sVar9 = fn_8020A630(uVar3,bVar14);
            sVar10 = fn_8020A5C0(uVar3,bVar14);
            cVar13 = fn_8020A6A0(uVar3,bVar14);
            if (bVar12 == 2) {
              iVar6 = _fadeEffectGetRandom__FUl((uVar5 & 0xffff) - (uVar4 & 0xffff));
              uVar8 = (uVar4 & 0xffff) + iVar6;
LAB_00207c9c:
              if ((cVar13 == 1) && (uVar8 = uVar8 * (int)sVar9, sVar10 != 0)) {
                uVar8 = (int)uVar8 / (int)sVar10;
              }
            }
            else {
              if (bVar12 < 2) {
                if (bVar12 != 0) {
                  uVar8 = uVar4 & 0xffff;
                }
                goto LAB_00207c9c;
              }
              if (3 < bVar12) goto LAB_00207c9c;
              uVar11 = fightFloorGetStatus(0,0,0x14,0);
              iVar6 = fightTargetGetPtr(uVar4,r4,uVar11);
              if (iVar6 != 0) {
                fightTargetDataBiosGetPtr(uVar4);
                uVar7 = fightTargetDataBiosGetStatusKid();
                if (cVar13 == 0) {
                  uVar8 = fn_80135E44(uVar7,iVar6,sVar9,uVar5,sVar10);
                }
                else {
                  uVar8 = fn_80135E44(uVar7,iVar6,0,uVar5,0);
                }
                goto LAB_00207c9c;
              }
              uVar8 = 0;
            }
            local_48[bVar14] = uVar8;
          }
          uVar8 = fn_8020A8A0(uVar3);
          if ((uVar8 & 0xff) < 7) {

            iVar2 = ((int (*)(void))**(void ***)((uVar8 & 0xff) * 4 + -0x7fc8a6c8))();
            return iVar2;
          }
          bVar14 = fn_8020A580(uVar3);
          if (bVar14 == 2) {
            iVar2 = 0;
          }
          else if ((bVar14 < 2) && (bVar14 != 0)) {
            if (iVar2 == 0) {
              iVar2 = 0;
            }
            else {
              iVar2 = 1;
            }
          }
          uVar1 = fn_8020A500(uVar1);
        } while ((uVar1 & 0xffff) != 0);
      }
      return iVar2;
    }
    uVar1 = 0;
    bVar12 = fn_8020A7F0(r3,bVar14);
    uVar8 = fn_8020A780(r3,bVar14);
    uVar4 = fn_8020A710(r3,bVar14);
    sVar9 = fn_8020A630(r3,bVar14);
    sVar10 = fn_8020A5C0(r3,bVar14);
    cVar13 = fn_8020A6A0(r3,bVar14);
    if (bVar12 == 2) {
      iVar2 = _fadeEffectGetRandom__FUl((uVar4 & 0xffff) - (uVar8 & 0xffff));
      uVar1 = (uVar8 & 0xffff) + iVar2;
LAB_00207a34:
      if ((cVar13 == 1) && (uVar1 = uVar1 * (int)sVar9, sVar10 != 0)) {
        uVar1 = (int)uVar1 / (int)sVar10;
      }
    }
    else {
      if (bVar12 < 2) {
        if (bVar12 != 0) {
          uVar1 = uVar8 & 0xffff;
        }
        goto LAB_00207a34;
      }
      if (3 < bVar12) goto LAB_00207a34;
      uVar11 = fightFloorGetStatus(0,0,0x14,0);
      iVar2 = fightTargetGetPtr(uVar8,r4,uVar11);
      if (iVar2 != 0) {
        fightTargetDataBiosGetPtr(uVar8);
        uVar3 = fightTargetDataBiosGetStatusKid();
        if (cVar13 == 0) {
          uVar1 = fn_80135E44(uVar3,iVar2,sVar9,uVar4,sVar10);
        }
        else {
          uVar1 = fn_80135E44(uVar3,iVar2,0,uVar4,0);
        }
        goto LAB_00207a34;
      }
      uVar1 = 0;
    }
    uVar8 = (u32)bVar14;
    bVar14 = bVar14 + 1;
    local_48[uVar8 + 2] = uVar1;
  } while (1);
}
