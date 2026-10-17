/**
 * @file fight_out_pokemon_candidate_802038A4.c
 * @brief Residual Ghidra-shaped validity-check candidate
 *        0x802038A4-0x80203A6C, 1 function.
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
extern void  joutaiDataBiosGetStatusKind(void);
extern void  pokemonIsJoutaiDataId(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

/* Address: 0x802038A4 | Size: 0x1c8 | Ghidra import */

u8 fightOutPokemonIsHinsi(int r3)

{
    extern s8 pokemonCheckValid();
    extern short std103_battleGetResult();
  u8 bVar1;
  u16 sVar4;
  u32 iVar2;
  u8 cVar5;
  u32 iVar3;
  u8 uVar6;

  if (r3 == 0) {
    bVar1 = 0;
  }
  else {
    sVar4 = std103_battleGetResult();
    if (sVar4 == 1) {
      bVar1 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        bVar1 = 0;
      }
      else {
        if (iVar3 == 0) {
          bVar1 = 0;
        }
        else {
        sVar4 = std103_battleGetResult();
        if (sVar4 == 1) {
          bVar1 = 0;
        }
        else {
          iVar2 = (int)pokemonGetStatus(iVar3,0,0xcb,0);
          if (iVar2 == 0) {
            bVar1 = 0;
          }
          else {
            cVar5 = pokemonCheckValid();
            if (cVar5 == 0) {
              bVar1 = 0;
            }
            else {
              if (iVar3 == 0) {
                iVar2 = 0;
              }
              else {
                iVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
              }
              if (iVar2 == 0) {
                bVar1 = 0;
              }
              else {
                cVar5 = pokemonCheckValid();
                if (cVar5 == 0) {
                  bVar1 = 0;
                }
                else {
                  iVar3 = (int)pokemonGetStatus(iVar3,0,0xce,0);
                  if ((int)iVar3 < 0) {
                    bVar1 = 0;
                  }
                  else {
                    bVar1 = 1;
                  }
                }
              }
            }
          }
        }
        }
        if (bVar1 == 0) {
          bVar1 = 0;
        }
        else {
          bVar1 = 1;
        }
      }
    }
  }
  if (bVar1 == 0) {
    uVar6 = 1;
  }
  else {
    if ((u32)r3 == 0) {
      iVar3 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        iVar3 = 0;
      }
      else {
        iVar3 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
    }
    if (iVar3 == 0) {
      uVar6 = 1;
    }
    else {
      uVar6 = (int)pokemonGetStatus(iVar3,0,0x7b,0);
    }
  }
  return uVar6;
}
