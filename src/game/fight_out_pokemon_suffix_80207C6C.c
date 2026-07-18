/**
 * @file fight_out_pokemon_suffix_80207C6C.c
 * @brief fightOutPokemon + fightPokemon suffix, address range
 *        0x80207C6C-0x8020AE30, 84 functions.
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
/* Address: 0x80207C6C | Size: 0x2f0 | Ghidra import */
u32 fightOutPokemonCreateSequence(void)

{
    int r3;
    u16 r4;

    extern short fn_80119ED0();
    extern s8 fn_8011B67C();
    extern void pokemonBiosCopy();
    extern s8 fn_80121ADC();
    extern void pokemonSetSequenceStatus();
    extern u32 pokemonCheckRare();
    extern u32 fn_801DE190();
    extern void fightOutPokemonGetRndStatus();
  u32 uVar1;
  short sVar5;
  u32 uVar2;
  int iVar3;
  s8 cVar6;
  u32 uVar4;
  u8 uVar7;
  u32 local_158;
  u32 local_154;
  u8 auStack_150 [320];

  if (r3 == 0) {
    uVar1 = 0;
  }
  else {
    iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
    if (iVar3 == 0) {
      uVar1 = 0;
    }
    else {
      uVar1 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
    }
  }
  pokemonBiosCopy(auStack_150,uVar1);
  sVar5 = (int)pokemonGetStatus(auStack_150,0,0x6e,0);
  uVar2 = (int)pokemonGetStatus(0,sVar5,0x66,0);
  if (sVar5 != 0x181) goto LAB_00204d50;
  if (r4 != 3) {
    if (r4 < 3) {
      if (r4 == 1) {
        uVar2 = 0x19f;
        goto LAB_00204d50;
      }
      if (r4 != 0) {
        uVar2 = 0x19e;
        goto LAB_00204d50;
      }
    }
    else if (r4 < 5) {
      uVar2 = 0x1a0;
      goto LAB_00204d50;
    }
  }
  uVar2 = 0x181;
LAB_00204d50:
  sVar5 = fn_80119ED0(0x14);
  if (((sVar5 == 0x7c) || (sVar5 = fn_80119ED0(0x14), sVar5 == 200)) ||
     (sVar5 = fn_80119ED0(0x14), sVar5 == 0xcd)) {
    iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
    sVar5 = fn_80119ED0(0x14);
    if ((sVar5 == 0x7c) || (sVar5 = fn_80119ED0(0x14), sVar5 == 200)) {
      if (iVar3 == 0) {
        uVar1 = 0;
      }
      else {
        uVar1 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
      cVar6 = fn_80121ADC(uVar1,0x14);
    }
    else {
      sVar5 = fn_80119ED0(0x14);
      if (sVar5 == 0xcd) {
        cVar6 = fn_8011B67C(iVar3,0x14);
      }
      else {
        cVar6 = 0;
      }
    }
  }
  else {
    sVar5 = fn_80119ED0(0x14);
    if (sVar5 == 0xd8) {
      cVar6 = fn_8011B67C(r3,0x14);
    }
    else {
      cVar6 = 0;
    }
  }
  if (cVar6 == 1) {
    uVar2 = 0x19d;
  }
  if (uVar2 == 0) {
    uVar1 = 0;
  }
  else {
    fightOutPokemonGetRndStatus(r3,&local_154,&local_158);
    pokemonSetStatus(auStack_150,0,0x6f,0,local_154);
    pokemonSetStatus(auStack_150,0,0x75,0,local_158);
    uVar1 = pokemonCheckRare(auStack_150);
    uVar1 = fn_801DE190(uVar2 & 0xffff,local_154,uVar1);
    pokemonSetSequenceStatus(auStack_150,uVar1);
    if (r3 == 0) {
      uVar4 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar4 = 0;
      }
      else {
        uVar4 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
    }
    uVar7 = (int)pokemonGetStatus(uVar4,0,0x73,0);
    itemGetStatus(0,uVar7,0x10,0);
  }
  return uVar1;
}

/* Address: 0x80207F5C | Size: 0xcc */
#pragma push
#pragma peephole on
u32 _fightOutPokemonRegWzxFreeSub__FPvUsPv(void* pokemon, u16 unused, u32* state)
{
    void* currentPokemon;
    void* resolved;
    u32 excludedPokemon;
    u32 itemDataId;

    excludedPokemon = state[0];
    if ((currentPokemon = pokemon) == NULL) {
        resolved = NULL;
    } else {
        resolved = pokemonGetStatus(currentPokemon, 0, 0xD6, 0);
        if (resolved == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(resolved, 0, 0xCC, 0);
        }
    }

    if (currentPokemon == (void*)excludedPokemon) {
        return 1;
    }

    itemDataId = itemGetStatus(0, (u8)(u32)pokemonGetStatus(resolved, 0, 0x73, 0), 0x10, 0);
    if (itemDataId == 0) {
        return 1;
    }

    if (state[1] == itemDataId) {
        state[2]++;
    }
    return 1;
}
#pragma pop

/* 0x80208028 | size: 0x80 | small */
#pragma push
#pragma peephole on
void fightOutPokemonRegWzxLoad(void* param_1) {
    void* uVar1;
    void* iVar2;
    u8 uVar3;

    if (param_1 == NULL) {
        uVar1 = NULL;
    } else {
        iVar2 = pokemonGetStatus(param_1, 0, 0xD6, 0);
        if (iVar2 == NULL) {
            uVar1 = NULL;
        } else {
            uVar1 = pokemonGetStatus(iVar2, 0, 0xCC, 0);
        }
    }
    uVar3 = (u8)(u32)pokemonGetStatus(uVar1, 0, 0x73, 0);
    itemGetStatus(0, uVar3, 0x10, 0);
}
#pragma pop

/* Address: 0x802080A8 | Size: 0x35c | Ghidra import */
void fn_802080A8(void)

{
    int r3;
    char r4;
    char r5;
    u32 r6;
    char r7;

    extern void _threadSwitch();
    extern void fn_80166A50();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
    extern u32 fightFloorGetStatus();
    extern void fightMenuOpenMsg();
    extern void fightMenuFightOutPokemonRenewStatusMenu();
    u32 saved_r25 = 0;
  u16 uVar4;
  int iVar1;
  u32 uVar2;
  u16 uVar5;
  s8 cVar6;
  int iVar3;

  uVar4 = fightFloorGetStatus(0,0,0x14,0);
  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r7 == 0) {
      if (r4 == 1) {
        fn_801DDD28(iVar1,0xa3,4,0);
      }
      if (r5 == 1) {
        fn_801DDD28(iVar1,0x9f,4,0);
      }
      if ((r4 == 0) && (r5 == 0)) {
        fn_801DDD28(iVar1,0x57,4,0);
      }
    }
    else if (r7 == 1) {
      if (r4 == 1) {
        fn_801DA9E8(iVar1,0xa3,4);
        if (r3 == 0) {
          uVar2 = 0;
        }
        else {
          iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
          if (iVar3 == 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
          }
        }
        uVar5 = (int)pokemonGetStatus(uVar2,0,0x6e,0);
        uVar5 = (int)pokemonGetStatus(0,uVar5,0x61,0);
        fn_80166A50(uVar5,0,0xff,0);
        fightMenuOpenMsg(r6);
        if (r5 == 0) {
          fightMenuFightOutPokemonRenewStatusMenu(r3,uVar4,1);
        }
      }
      if (r5 == 1) {
        if (r4 == 1) {
          while (1) {
            cVar6 = fn_801DA94C(iVar1,0xa3,4);
            if (cVar6 == 0) break;
            _threadSwitch();
          }
        }
        fn_801DA9E8(iVar1,0x9f,4);
        if (r4 == 0) {
          if (r3 == 0) {
            uVar2 = 0;
          }
          else {
            iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
            if (iVar3 == 0) {
              uVar2 = 0;
            }
            else {
              uVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
            }
          }
          uVar5 = (int)pokemonGetStatus(uVar2,0,0x6e,0);
          uVar5 = (int)pokemonGetStatus(0,uVar5,0x61,0);
          fn_80166A50(uVar5,0,0xff,0);
          fightMenuOpenMsg(r6);
        }
        fightMenuFightOutPokemonRenewStatusMenu(r3,uVar4,1);
      }
      if ((r4 == 0) && (r5 == 0)) {
        fn_801DA9E8(iVar1,0x57,4);
        fightMenuOpenMsg(r6);
      }
    }
    else if (r7 == 2) {
      if (r4 == 1) {
        saved_r25 = 0xa3;
      }
      if (r5 == 1) {
        saved_r25 = 0x9f;
      }
      if ((r4 == 0) && (r5 == 0)) {
        saved_r25 = 0x57;
      }
      while (1) {
        cVar6 = fn_801DA94C(iVar1,saved_r25,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
    }
    else if (r7 == 3) {
      if (r4 == 1) {
        fn_801DA8C4(iVar1,0xa3,4);
      }
      if (r5 == 1) {
        fn_801DA8C4(iVar1,0x9f,4);
      }
      if ((r4 == 0) && (r5 == 0)) {
        fn_801DA8C4(iVar1,0x57,4);
      }
    }
  }
  return;
}

/* Address: 0x80208404 | Size: 0x150 | Ghidra import */
#pragma push
#pragma peephole on
void fightOutPokemonDarkPokemonEffect(void* ctx, u8 p4, u8 p5, u8 p6)
{
    extern void _threadSwitch();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9B4();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
    extern u32 fightFloorGetStatus();
    extern void fn_80265598();
    void* iVar1;
    u16 uVar2;
    u8 cVar3;
    u32 uVar4;

    uVar2 = fightFloorGetStatus(0, 0, 0x14, 0);
    iVar1 = pokemonGetStatus(ctx, 0, 0xee, 0);
    if (iVar1 != 0) {
        if (p5 == 0) {
            uVar4 = 0x3a;
        } else if (p5 == 1) {
            uVar4 = 0x88;
        } else if (p5 == 2) {
            uVar4 = 0x57;
        } else {
            uVar4 = 0xd9;
        }
        if (p6 == 0) {
            fn_801DDD28(iVar1, uVar4, 4, 0);
        } else if (p6 == 1) {
            fn_801DA9E8(iVar1, uVar4, 4);
            if (p4 == 1) {
                fn_80265598(ctx, uVar2, 1);
            }
        } else if (p6 == 2) {
            while (1) {
                cVar3 = fn_801DA94C(iVar1, uVar4, 4);
                if (cVar3 == 0) break;
                _threadSwitch();
            }
        } else if (p6 == 3) {
            fn_801DA8C4(iVar1, uVar4, 4);
        } else if (p6 == 4) {
            fn_801DA9B4(iVar1, uVar4, 4);
        }
    }
}
#pragma pop

/* Address: 0x80208554 | Size: 0x70 | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
void fn_80208554(void* r3, u32 r4, u32 r5, u32 r6)

{
    extern void _threadSwitch();
    extern u8 fn_801DA698();
  int iVar1;
  u8 cVar2;

  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    while (1) {
      cVar2 = fn_801DA698(iVar1,r4,r5,r6);
      if (cVar2 == 1) break;
      _threadSwitch();
    }
  }
  return;
}
#pragma pop

/* Address: 0x802085C4 | Size: 0xec | Ghidra import */
#pragma push
#pragma peephole on
void fightOutPokemonWazaEffect(u32 r3, u32 r4, u32 r5, u32 r6, int r7)

{
    extern void menuCloseCustom();
    extern void menuOpenCustom(int, ...);
    extern u32 fightFloorGetStatus();
    extern void fightOutPokemonToMenuPokemonStatus();
    extern void fightWazaDoEffect();
    extern int fightMenuGetFightOutPokemonPtrToStatusMenuId();
  int iVar1;
  u16 uVar2;
  u8 auStack_58 [44];

  uVar2 = fightFloorGetStatus(0,0,0x14,0);
  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    fightOutPokemonToMenuPokemonStatus(r3,auStack_58);
    if (r7 >= 0) {
      r7 = fightMenuGetFightOutPokemonPtrToStatusMenuId(r3,uVar2,1);
      menuOpenCustom(r7,0,0,0,0,1,auStack_58);
    }
    fightWazaDoEffect(iVar1,r4,r5,r6);
    if (((r6 & 0xff) == 1) && (r7 >= 0)) {
      menuCloseCustom(r7,0,0);
    }
  }
  return;
}
#pragma pop

/* 0x802086B0 | size: 0x38 | small */
#pragma push
#pragma peephole on
void fightOutPokemonFreeAllSequenceWaza(void* ctx) {
    extern void fn_801DA83C();
    void* obj = pokemonGetStatus(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA83C(obj);
    }
}
#pragma pop

/* 0x802086E8 | size: 0x68 | small */
#pragma push
#pragma peephole on
void fightOutPokemonFreeWazaEffect(void* param_1, u32 param_2, u32 param_3) {
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    extern void fn_801DA8C4(void*, u16, u32);
    u32 uVar2;
    void* iVar1;

    uVar2 = wazaGetStatus(NULL, param_2, 0x1F, 0);
    iVar1 = pokemonGetStatus(param_1, 0, 0xEE, 0);
    if (iVar1 != NULL) {
        fn_801DA8C4(iVar1, (u16)uVar2, param_3);
    }
}
#pragma pop

/* 0x80208750 | size: 0x70 | small */
void fightOutPokemonLoadWazaEffect(void* param_1, u32 param_2, u32 param_3, u32 param_4) {
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    extern void fn_801DDD28(void*, u16, u32, u32);
    u32 uVar2;
    void* iVar1;

    uVar2 = wazaGetStatus(NULL, param_2, 0x1F, 0);
    iVar1 = pokemonGetStatus(param_1, 0, 0xEE, 0);
    if (iVar1 != NULL) {
        fn_801DDD28(iVar1, (u16)uVar2, param_3, param_4);
    }
}

/* Address: 0x802087C0 | Size: 0x458 | Ghidra import */
void fightOutPokemonHokakuEffect(void)

{
    int r3;
    u8 r4;
    u32 r5;
    char r6;
    u8 *r7;

    extern void _threadSwitch();
    extern void battleGridRemovePokemon();
    extern void fn_801DA224();
    extern void fn_801DA2C4();
    extern u32 fn_801DA354();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern void fn_8026532C();
  int iVar1;
  u16 uVar4;
  u16 uVar5;
  u16 uVar6;
  u16 uVar7;
  u16 uVar8;
  u8 uVar9;
  s8 cVar10;
  u32 uVar2;
  int iVar3;
  u8 bVar11;
  int local_38;
  int local_34;
  u32 local_30;

  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    uVar4 = itemGetStatus(0,r5,0x17,0);
    uVar5 = itemGetStatus(0,r5,0x13,0);
    uVar6 = itemGetStatus(0,r5,0x16,0);
    uVar7 = itemGetStatus(0,r5,0x14,0);
    uVar8 = itemGetStatus(0,r5,0x15,0);
    if (r6 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
      fn_801DDD28(iVar1,uVar5,4,0);
      fn_801DDD28(iVar1,uVar6,4,0);
      fn_801DDD28(iVar1,uVar7,4,0);
      fn_801DDD28(iVar1,uVar8,4,0);
      if (r7 != (void *)0) {
        uVar9 = fn_801DA354(iVar1);
        *r7 = uVar9;
        fn_801DA2C4(iVar1);
      }
    }
    else if (r6 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
      while (1) {
        cVar10 = fn_801DA94C(iVar1,uVar4,4);
        if (cVar10 == 0) break;
        _threadSwitch();
      }
      fn_801DA9E8(iVar1,uVar5,4);
      while (1) {
        cVar10 = fn_801DA94C(iVar1,uVar5,4);
        if (cVar10 == 0) break;
        _threadSwitch();
      }
      bVar11 = 0;
      do {
        fn_801DA9E8(iVar1,uVar6,4);
        while (1) {
          cVar10 = fn_801DA94C(iVar1,uVar6,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
        bVar11 = bVar11 + 1;
      } while ((bVar11 < 3) && (bVar11 < r4));
      if (r4 < 4) {
        fn_801DA9E8(iVar1,uVar7,4);
        while (1) {
          cVar10 = fn_801DA94C(iVar1,uVar7,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r6 == 2) {
      if (r4 < 4) {
        fn_801DA9E8(iVar1,uVar8,4);
      }
    }
    else if (r6 == 3) {
      if (r4 < 4) {
        if (r7 != (void *)0) {
          fn_801DA224(iVar1,*r7);
        }
        while (1) {
          cVar10 = fn_801DA94C(iVar1,uVar8,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r6 == 4) {
      fn_801DA8C4(iVar1,uVar4,4);
      fn_801DA8C4(iVar1,uVar5,4);
      fn_801DA8C4(iVar1,uVar6,4);
      fn_801DA8C4(iVar1,uVar7,4);
      fn_801DA8C4(iVar1,uVar8,4);
      if (r4 < 4) {
        if (r7 != (void *)0) {
          fn_801DA224(iVar1,*r7);
        }
      }
      else {
        if (r3 == 0) {
          uVar2 = 0;
        }
        else {
          iVar1 = (int)pokemonGetStatus(r3,0,0xd6,0);
          if (iVar1 == 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = (int)pokemonGetStatus(iVar1,0,0xcc,0);
          }
        }
        uVar9 = (int)pokemonGetStatus(uVar2,0,0x73,0);
        iVar1 = itemGetStatus(0,uVar9,0x10,0);
        if (iVar1 != 0) {
          local_30 = 0;
          local_38 = r3;
          local_34 = iVar1;
          fightFloorLoopValidFightOutPokemon(0,0x80207f5c,&local_38,0);
        }
        iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
        if (iVar1 != 0) {
          iVar3 = (int)pokemonGetStatus(r3,0,0xee,0);
          if (iVar3 != 0) {
            fn_801DA4E8(iVar3,0);
          }
          pokemonSetStatus(r3,0,0xee,0,0);
          battleGridRemovePokemon(iVar1);
          fn_801DB100(iVar1);
        }
        uVar4 = fightFloorGetStatus(0,0,0x14,0);
        fn_8026532C(r3,uVar4,1);
      }
    }
  }
  return;
}

/* Address: 0x80208C18 | Size: 0x2b8 | Ghidra import */
void fightOutPokemonDasuEffect(void)

{
    int r3;
    char r4;

    extern void _threadSwitch();
    extern s8 pokemonCheckRare();
    extern void fn_80166A50();
    extern s8 fn_801DA5C4();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
  int iVar1;
  u8 uVar6;
  u16 uVar4;
  u16 uVar5;
  s8 cVar7;
  u32 uVar2;
  int iVar3;
  u32 uVar8;

  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar8 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar8 = 0;
      }
      else {
        uVar8 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
    }
    uVar6 = (int)pokemonGetStatus(uVar8,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar6,0xe,0);
    uVar5 = itemGetStatus(0,uVar6,0xf,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
      fn_801DDD28(iVar1,uVar5,4,0);
      fn_801DDD28(iVar1,0x67,4,0);
    }
    else if (r4 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (cVar7 = fn_801DA94C(iVar1,uVar4,4), cVar7 != 0) {
        _threadSwitch();
      }
    }
    else if (r4 == 3) {
      fn_801DA9E8(iVar1,uVar5,4);
    }
    else if (r4 == 4) {
      while (cVar7 = fn_801DA5C4(0), cVar7 != 1) {
        _threadSwitch();
      }
      if (r3 == 0) {
        uVar2 = 0;
      }
      else {
        iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
        if (iVar3 == 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
        }
      }
      uVar4 = (int)pokemonGetStatus(uVar2,0,0x6e,0);
      uVar4 = (int)pokemonGetStatus(0,uVar4,0x61,0);
      fn_80166A50(uVar4,0,0xff,0);
      while (cVar7 = fn_801DA94C(iVar1,uVar5,4), cVar7 != 0) {
        _threadSwitch();
      }
      cVar7 = pokemonCheckRare(uVar8);
      if (cVar7 == 1) {
        fn_801DA9E8(iVar1,0x67,4);
        while (cVar7 = fn_801DA94C(iVar1,0x67,4), cVar7 != 0) {
          _threadSwitch();
        }
      }
    }
    else if (r4 == 5) {
      fn_801DA8C4(iVar1,uVar4,4);
      fn_801DA8C4(iVar1,uVar5,4);
      fn_801DA8C4(iVar1,0x67,4);
    }
  }
  return;
}

/* Address: 0x80208ED0 | Size: 0x25c | Ghidra import */
void fightOutPokemonModosuEffect(void)

{
    int r3;
    char r4;

    extern void _threadSwitch();
    extern void battleGridRemovePokemon();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern void fn_8026532C();
  int iVar1;
  u32 uVar2;
  u8 uVar5;
  u16 uVar4;
  s8 cVar6;
  int iVar3;
  int local_28;
  int local_24;
  u32 local_20;

  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar2 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
    }
    uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar5,0xd,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
    }
    else if (r4 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar6 = fn_801DA94C(iVar1,uVar4,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
    }
    else if (r4 == 3) {
      fn_801DA8C4(iVar1,uVar4,4);
    }
    else if (r4 == 4) {
      if (r3 == 0) {
        uVar2 = 0;
      }
      else {
        iVar1 = (int)pokemonGetStatus(r3,0,0xd6,0);
        if (iVar1 == 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = (int)pokemonGetStatus(iVar1,0,0xcc,0);
        }
      }
      uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
      iVar1 = itemGetStatus(0,uVar5,0x10,0);
      if (iVar1 != 0) {
        local_20 = 0;
        local_28 = r3;
        local_24 = iVar1;
        fightFloorLoopValidFightOutPokemon(0,0x80207f5c,&local_28,0);
      }
      iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
      if (iVar1 != 0) {
        iVar3 = (int)pokemonGetStatus(r3,0,0xee,0);
        if (iVar3 != 0) {
          fn_801DA4E8(iVar3,0);
        }
        pokemonSetStatus(r3,0,0xee,0,0);
        battleGridRemovePokemon(iVar1);
        fn_801DB100(iVar1);
      }
      uVar4 = fightFloorGetStatus(0,0,0x14,0);
      fn_8026532C(r3,uVar4,1);
    }
  }
  return;
}

/* Address: 0x8020912C | Size: 0x254 | Ghidra import */
void fightOutPokemonKizetuEffect(void)

{
    int r3;
    char r4;

    extern void _threadSwitch();
    extern void battleGridRemovePokemon();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern void fn_8026532C();
  int iVar1;
  u32 uVar2;
  u8 uVar5;
  u16 uVar4;
  s8 cVar6;
  int iVar3;
  int local_28;
  int local_24;
  u32 local_20;

  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar2 = 0;
    }
    else {
      iVar3 = (int)pokemonGetStatus(r3,0,0xd6,0);
      if (iVar3 == 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = (int)pokemonGetStatus(iVar3,0,0xcc,0);
      }
    }
    uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar5,0x10,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
    }
    else if (r4 == 1) {
      fn_801DA9E8(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar6 = fn_801DA94C(iVar1,uVar4,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
      fn_801DA8C4(iVar1,uVar4,4);
      uVar4 = fightFloorGetStatus(0,0,0x14,0);
      fn_8026532C(r3,uVar4,1);
    }
    else if (r4 == 3) {
      if (r3 == 0) {
        uVar2 = 0;
      }
      else {
        iVar1 = (int)pokemonGetStatus(r3,0,0xd6,0);
        if (iVar1 == 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = (int)pokemonGetStatus(iVar1,0,0xcc,0);
        }
      }
      uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
      iVar1 = itemGetStatus(0,uVar5,0x10,0);
      if (iVar1 != 0) {
        local_20 = 0;
        local_28 = r3;
        local_24 = iVar1;
        fightFloorLoopValidFightOutPokemon(0,0x80207f5c,&local_28,0);
      }
      iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
      if (iVar1 != 0) {
        iVar3 = (int)pokemonGetStatus(r3,0,0xee,0);
        if (iVar3 != 0) {
          fn_801DA4E8(iVar3,0);
        }
        pokemonSetStatus(r3,0,0xee,0,0);
        battleGridRemovePokemon(iVar1);
        fn_801DB100(iVar1);
      }
    }
  }
  return;
}

/* 0x80209380 | size: 0x104 */
#pragma push
#pragma peephole on
void fightOutPokemonDamageEffect(void* ctx) {
    extern u8 GSmodelGetVisibility();
    extern void GSmodelSetVisibility();
    extern void fn_801DA4E8();
    extern void* fn_801DAC3C();
    extern void fightMainWaitFrame();
    void* eeData;
    void* resolved;
    u8 i;

    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    resolved = !eeData ? NULL : fn_801DAC3C(eeData);
    if (resolved == NULL) { return; }
    if ((u8)GSmodelGetVisibility(resolved) == 0) { return; }
    for (i = 0; i < 8; i++) {
        eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
        if (eeData != NULL) {
            fn_801DA4E8(eeData, 1);
        }
        GSmodelSetVisibility(resolved, 1);
        fightMainWaitFrame(3);
        eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
        if (eeData != NULL) {
            fn_801DA4E8(eeData, 0);
        }
        fightMainWaitFrame(2);
    }
    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    if (eeData != NULL) {
        fn_801DA4E8(eeData, 1);
    }
}
#pragma pop

/* 0x80209484 | size: 0x48 | small */
#pragma push
#pragma peephole on
void fightOutPokemonSetVisibility(void* ctx, u32 param) {
    extern void fn_801DA4E8();
    void* obj = pokemonGetStatus(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA4E8(obj, param);
    }
}
#pragma pop

/* 0x802094CC | size: 0x90 | medium */
#pragma push
#pragma peephole on
#pragma scheduling on
void fightWazaDoEffect(u32 param_1, u32 param_2, u32 param_3, u8 param_4) {
    extern void _threadSwitch(void);
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    extern void fn_801DA8C4(u32, u16, u32);
    extern u8 fn_801DA94C(u32, u16, u32);
    extern void fn_801DA9E8(u32, u16, u32);
    u32 uVar1;
    u8 cVar2;

    uVar1 = wazaGetStatus(NULL, param_2, 0x1F, 0);
    fn_801DA9E8(param_1, uVar1, param_3);
    if (param_4 == 1) {
        while (1) {
            cVar2 = fn_801DA94C(param_1, uVar1, param_3);
            if (cVar2 == 0) break;
            _threadSwitch();
        }
        fn_801DA8C4(param_1, uVar1, param_3);
    }
}
#pragma pop

/* Address: 0x8020955C | Size: 0xbc | Ghidra import */
#pragma push
#pragma peephole on
void fightWazaDoEffectFunc(u32 p1, u32 p2, u32 p3, u32 p4)
{
    extern int wazaGetStatus();
    extern void fightWazaWzxTypeFuncNull();
    extern u32 fightWazaWzxVariationFuncNull();
    void* pcVar1;
    void* pcVar2;
    u32 uVar3;

    pcVar1 = (void*)wazaGetStatus(0, p1, 0x20, 0);
    if (pcVar1 == NULL) {
        pcVar1 = (void*)&fightWazaWzxTypeFuncNull;
    }
    pcVar2 = (void*)wazaGetStatus(0, p1, 0x21, 0);
    if (pcVar2 == NULL) {
        pcVar2 = (void*)&fightWazaWzxVariationFuncNull;
    }
    uVar3 = ((u32 (*)(u32, u32, u32, u32))pcVar2)(p1, p2, p3, p4);
    ((void (*)(u32, u32, u32, u32, u32))pcVar1)(p1, p2, p3, p4, uVar3);
}
#pragma pop

/* Address: 0x80209618 | Size: 0xd0 | Ghidra import */
#pragma push
#pragma peephole on
char fightWazaIsMix(u32 ctx)
{
    extern StatusIdTable7 lbl_80279D08;
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    u16 i;
    u16 statusId;
    u16 mixCount;
    u16 statusType;
    u8 isMix;
    StatusIdTable7 statusIds;
    u16* idTable;

    mixCount = 0;
    statusIds = lbl_80279D08;
    idTable = statusIds.id;
    for (i = 0; i < 7; i++) {
        statusId = idTable[i];
        statusType = fn_80119ED0(statusId);
        if (statusType != 0x2A) {
            isMix = 0;
        } else {
            isMix = fn_8011B67C(ctx, statusId);
        }
        if (isMix == 1) {
            mixCount++;
        }
    }
    return mixCount >= 2;
}
#pragma pop

/* Address: 0x802096E8 | Size: 0xe0 | Ghidra import */
#pragma push
#pragma peephole off
u32 fightWazaIsHit(void* ctx)
{
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
  u16 sVar2;
  u32 uVar1;
  u8 cVar3;

  sVar2 = fn_80119ED0(0x40);
  if (sVar2 != 0x2a) {
    cVar3 = 0;
  }
  else {
    cVar3 = fn_8011B67C(ctx,0x40);
  }
  if (cVar3 == 1) {
    uVar1 = 0;
  }
  else {
    sVar2 = fn_80119ED0(0x43);
    if (sVar2 != 0x2a) {
      cVar3 = 0;
    }
    else {
      cVar3 = fn_8011B67C(ctx,0x43);
    }
    if (cVar3 == 1) {
      uVar1 = 0;
    }
    else {
      sVar2 = fn_80119ED0(0x45);
      if (sVar2 != 0x2a) {
        cVar3 = 0;
      }
      else {
        cVar3 = fn_8011B67C(ctx,0x45);
      }
      if (cVar3 == 1) {
        uVar1 = 0;
      }
      else {
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}
#pragma pop

/* 0x802097C8 | size: 0x54 | small */
#pragma push
#pragma peephole on
void fightWazaWriteJoutaiDataId(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B2C0(u32, u32, u32);
    if ((fn_80119ED0(param_2) & 0xFFFF) == 0x2A) {
        fn_8011B2C0(param_1, param_2, param_3);
    }
}
#pragma pop

/* Address: 0x8020981C | Size: 0x54 | Ghidra import */
#pragma push
#pragma scheduling on
#pragma peephole on
u32 fightWazaCheckWriteJoutaiDataId(void* ctx, u32 param)

{
    extern u16 fn_80119ED0();
    extern u32 fn_8011B444();
  u16 sVar2;
  u32 uVar1;

  sVar2 = fn_80119ED0(param);
  if (sVar2 != 0x2a) {
    uVar1 = 0;
  }
  else {
    uVar1 = fn_8011B444(ctx,param);
  }
  return uVar1;
}
#pragma pop

/* Address: 0x80209870 | Size: 0x9c | Ghidra import */
u32 fightWazaIsJoutaiSousai(void* ctx)

{
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
  u16 sVar1;
  u8 cVar2;

  sVar1 = fn_80119ED0(0x41);
  if (sVar1 != 0x2a) {
    cVar2 = 0;
  }
  else {
    cVar2 = fn_8011B67C(ctx,0x41);
  }
  if (cVar2 == 1) {
    sVar1 = fn_80119ED0(0x42);
    if (sVar1 != 0x2a) {
      cVar2 = 0;
    }
    else {
      cVar2 = fn_8011B67C(ctx,0x42);
    }
    if (cVar2 == 1) {
      return 1;
    }
  }
  return 0;
}

/* 0x8020990C | size: 0x54 */
#pragma push
#pragma peephole on
#pragma scheduling on
u32 fightWazaIsJoutaiDataId(void* ctx, u32 param) {
    extern u32 fn_80119ED0();
    extern u32 fn_8011B67C();
    if ((fn_80119ED0(param) & 0xFFFF) != 0x2A) {
        return 0;
    }
    return fn_8011B67C(ctx, param);
}
#pragma pop

/* 0x80209960 | size: 0x4C | small */
#pragma push
#pragma scheduling on
#pragma peephole on
void fightWazaInitJoutaiDataId(void* ctx, u32 param) {
    extern u32 fn_80119ED0();
    extern void fn_8011B788();
    if ((fn_80119ED0(param) & 0xFFFF) == 0x2a) {
        fn_8011B788(ctx, param);
    }
}
#pragma pop

/* Address: 0x802099AC | Size: 0x270 | Ghidra import */
#pragma push
#pragma peephole on
void fightWazaCreate(void* p1, s8 p2, u32 p3, u16 p4, u8 p5) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    u16 sVar2;

    if (p1 != 0) {
        if (p1) {
            wazaSetStatus(p1, 0, 0x26, 0, (void*)0xffffffff);
            wazaSetStatus(p1, 0, 0x27, 0, 0);
            wazaSetStatus(p1, 0, 0x28, 0, 0);
            wazaSetStatus(p1, 0, 0x29, 0, 0);
            fn_8011B950(wazaGetStatus(p1, 0, 0x2a, 0), 9);
            sVar2 = fn_80119ED0(0x3f);
            if (sVar2 == 0x2a) {
                fn_8011B2C0(p1, 0x3f, 0);
            }
            wazaSetStatus(p1, 0, 0x2b, 0, 1);
            wazaSetStatus(p1, 0, 0x2c, 0, 1);
            wazaSetStatus(p1, 0, 0x2d, 0, 0);
            wazaSetStatus(p1, 0, 0x2e, 0, 0);
            wazaSetStatus(p1, 0, 0x2f, 0, 0);
            wazaSetStatus(p1, 0, 0x30, 0, 9);
            wazaSetStatus(p1, 0, 0x31, 0, 0);
            wazaSetStatus(p1, 0, 0x32, 0, 0);
        }
        wazaSetStatus(p1, 0, 0x26, 0, (s32)p2);
        wazaSetStatus(p1, 0, 0x29, 0, p4);
        wazaSetStatus(p1, 0, 0x27, 0, p3 & 0xffff);
        wazaSetStatus(p1, 0, 0x28, 0, p3 & 0xffff);
        wazaSetStatus(p1, 0, 0x2f, 0, (u16)wazaGetStatus(0, p3, 7, 0));
        wazaSetStatus(p1, 0, 0x30, 0, (u16)wazaGetStatus(0, p3, 3, 0));
        wazaSetStatus(p1, 0, 0x32, 0, p5);
    }
}
#pragma pop

/* Address: 0x80209C1C | Size: 0x98 | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
void fightWazaSetUseWazaStatus(u32 r3, u32 r4)

{
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
  u16 uVar1;

  wazaSetStatus(r3,0,0x28,0,r4 & 0xffff);
  uVar1 = (u16)wazaGetStatus(0,r4,7,0);
  wazaSetStatus(r3,0,0x2f,0,uVar1);
  uVar1 = (u16)wazaGetStatus(0,r4,3,0);
  wazaSetStatus(r3,0,0x30,0,uVar1);
  return;
}
#pragma pop

/* Address: 0x80209CB4 | Size: 0xdc | Ghidra import */
#pragma push
#pragma peephole on
u32 fightWazaCheckValid(void* ctx) {
    extern s32 wazaGetStatus(void* ctx, u32 p1, u32 p2, u32 p3);
    s32 iVar1;
    if (ctx == 0) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x27, 0);
    if (iVar1 == 0) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x27, 0);
    if (iVar1 == 0x163) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x28, 0);
    if (iVar1 == 0) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x28, 0);
    if (iVar1 == 0x163) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x29, 0);
    return iVar1 != 0;
}
#pragma pop

/* 0x80209D90 | size: 0x188 */
#pragma push
#pragma peephole on
void fightWazaInit(void* r3) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void wazaSetStatus();
    extern void* wazaGetStatus();
    void* ctx;

    if ((ctx = r3) == NULL) { return; }
    wazaSetStatus(ctx, 0, 0x26, 0, (u32)-1);
    wazaSetStatus(ctx, 0, 0x27, 0, 0);
    wazaSetStatus(ctx, 0, 0x28, 0, 0);
    wazaSetStatus(ctx, 0, 0x29, 0, 0);
    fn_8011B950(wazaGetStatus(ctx, 0, 0x2A, 0), 9);
    if (fn_80119ED0(0x3F) == 0x2A) {
        fn_8011B2C0(ctx, 0x3F, 0);
    }
    wazaSetStatus(ctx, 0, 0x2B, 0, 1);
    wazaSetStatus(ctx, 0, 0x2C, 0, 1);
    wazaSetStatus(ctx, 0, 0x2D, 0, 0);
    wazaSetStatus(ctx, 0, 0x2E, 0, 0);
    wazaSetStatus(ctx, 0, 0x2F, 0, 0);
    wazaSetStatus(ctx, 0, 0x30, 0, 9);
    wazaSetStatus(ctx, 0, 0x31, 0, 0);
    wazaSetStatus(ctx, 0, 0x32, 0, 0);
}
#pragma pop

/* 0x80209F18 | size: 0xa8 */
#pragma push
#pragma peephole on
void fightWazaInitLoop(void* ctx) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    u32 val;
    u16 typeId;
    val = wazaGetStatus(ctx, 0, 0x2a, 0);
    fn_8011B950(val, 9);
    typeId = fn_80119ED0(0x3f);
    if (typeId == 0x2a) {
        fn_8011B2C0(ctx, 0x3f, 0);
    }
    wazaSetStatus(ctx, 0, 0x2b, 0, 1);
    wazaSetStatus(ctx, 0, 0x2c, 0, 1);
}
#pragma pop

/* 0x80209FAC | size: 0x64 */
#pragma push
#pragma scheduling on
#pragma peephole on
void fightWazaInitJoutai(void* ctx) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern u32 fn_8011B950();
    extern u32 wazaGetStatus();
    u32 val = wazaGetStatus(ctx, 0, 0x2A, 0);
    fn_8011B950(val, 9);
    if (fn_80119ED0(0x3F) == 0x2A) {
        fn_8011B2C0(ctx, 0x3F, 0);
    }
}
#pragma pop

/* 0x8020A010 | size: 0x18 */
u32 fightWazaHitKakurituDataBiosGetWaru(u8* ptr) {
    if (ptr == NULL) { return 1; }
    return ptr[0x1];
}

/* 0x8020A028 | size: 0x18 */
u32 fightWazaHitKakurituDataBiosGetKake(u8* ptr) {
    if (ptr == NULL) { return 1; }
    return ptr[0x0];
}

/* fightWazaHitKakurituDataBiosGetPtr | Size: 0x28 | Look up 2-byte entry in table */
#pragma push
#pragma peephole on
u16* fightWazaHitKakurituDataBiosGetPtr(u16 index) {
    extern u8 lbl_80375DD0[];
    extern u32 lbl_80478D70;
    u16* result = (u16*)&lbl_80375DD0[index * 2];
    if (index < lbl_80478D70) {
        return result;
    }
    return NULL;
}
#pragma pop

/* Address: 0x8020A068 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaCriticalDataBiosGetBunbo(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* fightWazaCriticalDataBiosGetPtr | Size: 0x24 | Look up byte in table with bounds check */
#pragma push
#pragma peephole on
u8* fightWazaCriticalDataBiosGetPtr(u16 index) {
    extern u8 lbl_80478D58[];
    extern u32 lbl_80478D60;
    u8* result = &lbl_80478D58[index];
    if (index < lbl_80478D60) {
        return result;
    }
    return NULL;
}
#pragma pop

/* Address: 0x8020A0A4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetAutoMakeFlag(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA9]) = val;
}

/* Address: 0x8020A0B4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetKaisuu(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA8]) = val;
}

/* Address: 0x8020A0C4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetZokusei(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA6]) = val;
}

/* Address: 0x8020A0D4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetIryoku(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA4]) = val;
}

/* Address: 0x8020A0E4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetHitDamage(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xA0]) = val;
}

/* Address: 0x8020A0F4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetDamage(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x9C]) = val;
}

/* Address: 0x8020A104 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetDamageValue(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x99]) = val;
}

/* Address: 0x8020A114 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetCritical(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x98]) = val;
}

/* Address: 0x8020A124 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetTargetDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6]) = val;
}

/* Address: 0x8020A134 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetUseWazaDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x4]) = val;
}

/* Address: 0x8020A144 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetMotoWazaDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x8020A154 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetWazaBanme(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x8020A164 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetAutoMakeFlag(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA9]);
}

/* Address: 0x8020A17C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetKaisuu(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA8]);
}

/* Address: 0x8020A194 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetZokusei(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA6]);
}

/* Address: 0x8020A1AC | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetIryoku(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA4]);
}

/* Address: 0x8020A1C4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightWazaBiosGetHitDamage(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xA0]);
}

/* Address: 0x8020A1DC | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightWazaBiosGetDamage(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x9C]);
}

/* Address: 0x8020A1F4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetDamageValue(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x99]);
}

/* Address: 0x8020A20C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetCritical(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x98]);
}

/* 0x8020A224 | size: 0x34 | small */
#pragma push
#pragma peephole on
void* fightWazaBiosGetJoutaiPtr(void* base, u16 index) {
    if (base == 0) return 0;
    if (index >= 9) return 0;
    return (u8*)base + 0x8 + index * 16;
}
#pragma pop

/* Address: 0x8020A258 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetTargetDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6]);
}

/* Address: 0x8020A270 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetUseWazaDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x8020A288 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetMotoWazaDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020A2A0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetWazaBanme(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* 0x8020A2B8 | size: 0x40 -- copy 0xAC bytes (43 u32s) */
void fightWazaBiosCopy(u32* dst, u32* src) {
    struct CopyBlk8020A2B8 { u32 data[43]; };
    if (dst == 0) return;
    if (src == 0) return;
    *(struct CopyBlk8020A2B8*)dst = *(struct CopyBlk8020A2B8*)src;
}

/* Address: 0x8020A2F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetBuff(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x8020A308 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetCount(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x8020A318 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetTargetDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x8020A328 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetItemDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x8020A338 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightItemBiosGetBuff(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020A350 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightItemBiosGetCount(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020A368 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightItemBiosGetTargetDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020A380 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightItemBiosGetItemDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

#pragma push
#pragma peephole on
static inline void fightItemInitInline(u8* item)
{
    extern void fn_80142B24();

    if (item != NULL) {
        fn_80142B24(item, 0, 0x1E, 0, 0);
        fn_80142B24(item, 0, 0x1F, 0, 0);
        fn_80142B24(item, 0, 0x20, 0, (void*)-1);
        fn_80142B24(item, 0, 0x21, 0, 0);
    }
}

/* Address: 0x8020A398 | Size: 0xe0 */
void fightItemCreate(u8* item, u16 itemDataId, u16 targetDataId, u32 count)
{
    extern void fn_80142B24();
    u8* ctx;

    if ((ctx = item) != NULL) {
        fightItemInitInline(ctx);
        fn_80142B24(ctx, 0, 0x1E, 0, itemDataId);
        fn_80142B24(ctx, 0, 0x1F, 0, targetDataId);
        fn_80142B24(ctx, 0, 0x20, 0, count);
    }
}
#pragma pop

/* Address: 0x8020A478 | Size: 0x88 | Ghidra import */
#pragma push
#pragma peephole on
void fightItemInit(void* r3)
{
    extern void fn_80142B24();
    void* ctx;
    if ((ctx = r3) != NULL) {
        fn_80142B24(ctx, 0, 0x1e, 0, 0);
        fn_80142B24(ctx, 0, 0x1f, 0, 0);
        fn_80142B24(ctx, 0, 0x20, 0, (void*)0xffffffff);
        fn_80142B24(ctx, 0, 0x21, 0, 0);
    }
}
#pragma pop

/* 0x8020A500 | size: 0x40 */
u32 fn_8020A500(u16 idx) {
    ColosseumEventRow6* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = &lbl_80478D30[idx];
    }
    if (entry == NULL) { return 0; }
    return entry->nextIndex;
}

/* 0x8020A540 | size: 0x40 */
u32 fn_8020A540(u16 idx) {
    ColosseumEventRow6* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = &lbl_80478D30[idx];
    }
    if (entry == NULL) { return 0; }
    return entry->eventIndex;
}

/* 0x8020A580 | size: 0x40 */
u32 fn_8020A580(u16 idx) {
    ColosseumEventRow6* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = &lbl_80478D30[idx];
    }
    if (entry == NULL) { return 0; }
    return entry->mode;
}

/* 0x8020A5C0 | size: 0x70 */
s16 fn_8020A5C0(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->scaleDenominator;
}

/* 0x8020A630 | size: 0x70 */
s16 fn_8020A630(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->scaleNumerator;
}

/* 0x8020A6A0 | size: 0x70 */
u8 fn_8020A6A0(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->scaleMode;
}

/* 0x8020A710 | size: 0x70 */
u16 fn_8020A710(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->maxValue;
}

/* 0x8020A780 | size: 0x70 */
u16 fn_8020A780(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->minValue;
}

/* 0x8020A7F0 | size: 0x70 */
u8 fn_8020A7F0(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->valueMode;
}

/* fn_8020A860 | Size: 0x40 | Look up u16 field at offset 2 in 0x18-byte table */
u16 fn_8020A860(u16 index) {
    ColosseumEventPairRow* entry;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry->firstLinkIndex;
}

/* fn_8020A8A0 | Size: 0x40 | Look up u8 field at offset 0 in 0x18-byte table */
u8 fn_8020A8A0(u16 index) {
    ColosseumEventPairRow* entry;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry->resultFuncId;
}

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
