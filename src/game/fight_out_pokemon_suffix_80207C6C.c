/**
 * @file fight_out_pokemon_suffix_80207C6C.c
 * @brief fightOutPokemon + fightPokemon suffix prefix, address range
 *        0x80207C6C-0x802096E8, 19 functions.
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
u32 fightOutPokemonCreateSequence(void* r3, u16 r4)

{
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern void pokemonBiosCopy();
    extern u8 fn_80121ADC();
    extern void pokemonSetSequenceStatus();
    extern u32 pokemonCheckRare();
    extern u32 fn_801DE190();
    extern void fightOutPokemonGetRndStatus();
  u32 uVar1;
  u16 sVar5;
  u32 uVar2;
  int iVar3;
  u8 cVar6;
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
void fn_802080A8(void* r3, u8 r4, u8 r5, u32 r6, u8 r7)

{
    extern void _threadSwitch();
    extern void fn_80166A50();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
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
  u8 cVar6;
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
void fightOutPokemonHokakuEffect(void* r3, u8 r4, u32 r5, u8 r6, u8* r7)

{
    extern void _threadSwitch();
    extern void battleGridRemovePokemon();
    extern void fn_801DA224();
    extern void fn_801DA2C4();
    extern u32 fn_801DA354();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
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
  u8 cVar10;
  u32 uVar2;
  int iVar3;
  u8 bVar11;
  void* local_38;
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
void fightOutPokemonDasuEffect(void* r3, u8 r4)

{
    extern void _threadSwitch();
    extern u8 pokemonCheckRare();
    extern void fn_80166A50();
    extern u8 fn_801DA5C4();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DDD28();
  int iVar1;
  u8 uVar6;
  u16 uVar4;
  u16 uVar5;
  u8 cVar7;
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
