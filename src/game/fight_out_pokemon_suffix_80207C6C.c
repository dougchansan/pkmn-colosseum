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

/* fightPokemonGetPokemonPtr and fightOutPokemonGetPokemonPtr, expanded as in
 * fight_out_pokemon.c. */
static inline void* fightPokemonGetPokemonPtrInline(void* fp)
{
    void* p;

    if (fp == NULL) {
        p = NULL;
    } else {
        void* tmp = pokemonGetStatus(fp, 0, 0xCC, 0);
        p = tmp;
    }
    return p;
}

static inline void* fightOutPokemonGetPokemonPtrInline(void* fo)
{
    void* p;

    if (fo == NULL) {
        p = NULL;
    } else {
        void* tmp = fightPokemonGetPokemonPtrInline(pokemonGetStatus(fo, 0, 0xD6, 0));
        p = tmp;
    }
    return p;
}

static inline u8 fightPokemonCheckJoutaiInline(void* fp, u16 id)
{
    extern u16 joutaiDataBiosGetStatusKind();
    extern u8 pokemonIsJoutaiDataId();
    extern u8 fn_8011B67C();

    if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8) {
        return pokemonIsJoutaiDataId(fightPokemonGetPokemonPtrInline(fp), id);
    }
    if (joutaiDataBiosGetStatusKind(id) != 0xCD) {
        return 0;
    }
    return fn_8011B67C(fp, id);
}

static inline u8 fightOutPokemonCheckJoutaiInline(void* fo, u16 id)
{
    extern u16 joutaiDataBiosGetStatusKind();
    extern u8 fn_8011B67C();

    if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8 || joutaiDataBiosGetStatusKind(id) == 0xCD) {
        return fightPokemonCheckJoutaiInline(pokemonGetStatus(fo, 0, 0xD6, 0), id);
    }
    if (joutaiDataBiosGetStatusKind(id) != 0xD8) {
        return 0;
    }
    return fn_8011B67C(fo, id);
}

/* Address: 0x80207C6C | Size: 0x2f0 | Ghidra import */
/* 0x181 (Unown): the sequence follows the requested form. */
static inline u32 fightOutPokemonGetUnknownSeqInline(u16 form)
{
    switch (form) {
    case 1:
        return 0x19f;
    case 2:
        return 0x19e;
    case 4:
        return 0x1a0;
    default:
        return 0x181;
    }
}

#if !defined(FIGHT_OUT_POKEMON_EXACT_80207F5C_ONLY)
u32 fightOutPokemonCreateSequence(void* r3, u16 r4)

{
    extern void pokemonBiosCopy();
    extern void pokemonSetSequenceStatus();
    extern u32 pokemonCheckRare();
    extern u32 fn_801DE190();
    extern void fightOutPokemonGetRndStatus();
  u16 monsNo;
  u32 seqNo;
  u32 result;
  u32 rnd;
  /* RULE-EXCEPTION(user-approved): u16 form carrier and a widening u64 cast before the u16 cast, chosen for register allocation (permuter farm) — see docs/RULE_EXCEPTIONS.md */
  u16 form;
  u32 rare;
  u8 bios[320];

  pokemonBiosCopy(bios,fightOutPokemonGetPokemonPtrInline(r3));
  monsNo = (int)pokemonGetStatus(bios,0,0x6e,0);
  seqNo = (int)pokemonGetStatus(0,monsNo,0x66,0);
  if (monsNo == 0x181) {
    form = r4;
    seqNo = fightOutPokemonGetUnknownSeqInline(form);
  }
  if (fightOutPokemonCheckJoutaiInline(r3,0x14) == 1) {
    seqNo = 0x19d;
  }
  if (seqNo == 0) {
    return 0;
  }
  fightOutPokemonGetRndStatus(r3,&rnd,&rare);
  pokemonSetStatus(bios,0,0x6f,0,rnd);
  pokemonSetStatus(bios,0,0x75,0,rare);
  result = fn_801DE190((u16)(u64)seqNo,rnd,pokemonCheckRare(bios));
  pokemonSetSequenceStatus(bios,result);
  itemGetStatus(0,(u8)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtrInline(r3),0,0x73,0),0x10,0);
  return result;
}
#endif /* !FIGHT_OUT_POKEMON_EXACT_80207F5C_ONLY */

/* The rest of the range is the exact island 0x80207F5C-0x802096E8, built
 * through fight_out_pokemon_exact_80207F5C.c; this unit owns only
 * fightOutPokemonCreateSequence. */
#if defined(FIGHT_OUT_POKEMON_EXACT_80207F5C_ONLY)

/* Address: 0x80207F5C | Size: 0xcc */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
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
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* 0x80208028 | size: 0x80 | small */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
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
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* Address: 0x802080A8 | Size: 0x35c | Ghidra import */
void fn_802080A8(void* r3, u8 r4, u8 r5, u32 r6, u8 r7)

{
    extern void _threadSwitch();
    extern void soundStart();
    extern void wazaSequenceSysStopWaza();
    extern u8 wazaSequenceSysIsPlayingWaza();
    extern void wazaSequenceSysStartWaza();
    extern void fn_801DDD28();
    extern u32 fightFloorGetStatus();
    extern void fightMenuOpenMsg();
    extern void fightMenuFightOutPokemonRenewStatusMenu();
  u32 saved_r25;
  int iVar1;
  u16 uVar4;
  u32 uVar2;
  u16 uVar5;
  u8 cVar6;

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
        wazaSequenceSysStartWaza(iVar1,0xa3,4);
        uVar2 = (u32)fightOutPokemonGetPokemonPtrInline(r3);
        uVar5 = (int)pokemonGetStatus(uVar2,0,0x6e,0);
        uVar5 = (int)pokemonGetStatus(0,uVar5,0x61,0);
        soundStart(uVar5,0,0xff,0);
        fightMenuOpenMsg(r6);
        if (r5 == 0) {
          fightMenuFightOutPokemonRenewStatusMenu(r3,uVar4,1);
        }
      }
      if (r5 == 1) {
        if (r4 == 1) {
          while (1) {
            cVar6 = wazaSequenceSysIsPlayingWaza(iVar1,0xa3,4);
            if (cVar6 == 0) break;
            _threadSwitch();
          }
        }
        wazaSequenceSysStartWaza(iVar1,0x9f,4);
        if (r4 == 0) {
          uVar2 = (u32)fightOutPokemonGetPokemonPtrInline(r3);
          uVar5 = (int)pokemonGetStatus(uVar2,0,0x6e,0);
          uVar5 = (int)pokemonGetStatus(0,uVar5,0x61,0);
          soundStart(uVar5,0,0xff,0);
          fightMenuOpenMsg(r6);
        }
        fightMenuFightOutPokemonRenewStatusMenu(r3,uVar4,1);
      }
      if ((r4 == 0) && (r5 == 0)) {
        wazaSequenceSysStartWaza(iVar1,0x57,4);
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
        cVar6 = wazaSequenceSysIsPlayingWaza(iVar1,saved_r25,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
    }
    else if (r7 == 3) {
      if (r4 == 1) {
        wazaSequenceSysStopWaza(iVar1,0xa3,4);
      }
      if (r5 == 1) {
        wazaSequenceSysStopWaza(iVar1,0x9f,4);
      }
      if ((r4 == 0) && (r5 == 0)) {
        wazaSequenceSysStopWaza(iVar1,0x57,4);
      }
    }
  }
  return;
}

/* Address: 0x80208404 | Size: 0x150 | Ghidra import */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fightOutPokemonDarkPokemonEffect(void* ctx, u8 p4, u8 p5, u8 p6)
{
    extern void _threadSwitch();
    extern void wazaSequenceSysStopWaza();
    extern u8 wazaSequenceSysIsPlayingWaza();
    extern void fn_801DA9B4();
    extern void wazaSequenceSysStartWaza();
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
            wazaSequenceSysStartWaza(iVar1, uVar4, 4);
            if (p4 == 1) {
                fn_80265598(ctx, uVar2, 1);
            }
        } else if (p6 == 2) {
            while (1) {
                cVar3 = wazaSequenceSysIsPlayingWaza(iVar1, uVar4, 4);
                if (cVar3 == 0) break;
                _threadSwitch();
            }
        } else if (p6 == 3) {
            wazaSequenceSysStopWaza(iVar1, uVar4, 4);
        } else if (p6 == 4) {
            fn_801DA9B4(iVar1, uVar4, 4);
        }
    }
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* Address: 0x80208554 | Size: 0x70 | Ghidra import */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fn_80208554(void* r3, u32 r4, u32 r5, u32 r6)

{
    extern void _threadSwitch();
    extern u8 wazaSequenceSysIsWazaEnd();
  int iVar1;
  u8 cVar2;

  iVar1 = (int)pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    while (1) {
      cVar2 = wazaSequenceSysIsWazaEnd(iVar1,r4,r5,r6);
      if (cVar2 == 1) break;
      _threadSwitch();
    }
  }
  return;
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* Address: 0x802085C4 | Size: 0xec | Ghidra import */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
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
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* 0x802086B0 | size: 0x38 | small */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fightOutPokemonFreeAllSequenceWaza(void* ctx) {
    extern void fn_801DA83C();
    void* obj = pokemonGetStatus(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA83C(obj);
    }
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* 0x802086E8 | size: 0x68 | small */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fightOutPokemonFreeWazaEffect(void* param_1, u32 param_2, u32 param_3) {
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    extern void wazaSequenceSysStopWaza(void*, u16, u32);
    u32 uVar2;
    void* iVar1;

    uVar2 = wazaGetStatus(NULL, param_2, 0x1F, 0);
    iVar1 = pokemonGetStatus(param_1, 0, 0xEE, 0);
    if (iVar1 != NULL) {
        wazaSequenceSysStopWaza(iVar1, (u16)uVar2, param_3);
    }
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

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
    extern u32 wazaSequenceOwnerGetFlags();
    extern void wazaSequenceOwnerSetVisibility();
    extern void wazaSequenceSysStopWaza(void*, u16, u16);
    extern u8 wazaSequenceSysIsPlayingWaza(void*, u16, u16);
    extern void wazaSequenceSysStartWaza(void*, u16, u16);
    extern void wazaSequenceOwnerFree();
    extern void fn_801DDD28(void*, u16, u16, u32);
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern void fightMenuStatusMenuClose();
  void* iVar1;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 uVar9;
  u8 cVar10;
  u32 uVar2;
  void* iVar3;
  u32 itemStatus;
  u32 bVar11;
  u32 callbackState[3];

  iVar1 = pokemonGetStatus(r3,0,0xee,0);
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
        uVar9 = wazaSequenceOwnerGetFlags(iVar1);
        *r7 = uVar9;
        fn_801DA2C4(iVar1);
      }
    }
    else if (r6 == 1) {
      wazaSequenceSysStartWaza(iVar1,uVar4,4);
      while (1) {
        cVar10 = wazaSequenceSysIsPlayingWaza(iVar1,uVar4,4);
        if (cVar10 == 0) break;
        _threadSwitch();
      }
      wazaSequenceSysStartWaza(iVar1,uVar5,4);
      while (1) {
        cVar10 = wazaSequenceSysIsPlayingWaza(iVar1,uVar5,4);
        if (cVar10 == 0) break;
        _threadSwitch();
      }
      bVar11 = 0;
      do {
        wazaSequenceSysStartWaza(iVar1,uVar6,4);
        while (1) {
          cVar10 = wazaSequenceSysIsPlayingWaza(iVar1,uVar6,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
        bVar11 = bVar11 + 1;
      } while (((u8)bVar11 < 3) && ((u8)bVar11 < r4));
      if (r4 < 4) {
        wazaSequenceSysStartWaza(iVar1,uVar7,4);
        while (1) {
          cVar10 = wazaSequenceSysIsPlayingWaza(iVar1,uVar7,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r6 == 2) {
      if (r4 < 4) {
        wazaSequenceSysStartWaza(iVar1,uVar8,4);
      }
    }
    else if (r6 == 3) {
      if (r4 < 4) {
        if (r7 != (void *)0) {
          fn_801DA224(iVar1,*r7);
        }
        while (1) {
          cVar10 = wazaSequenceSysIsPlayingWaza(iVar1,uVar8,4);
          if (cVar10 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r6 == 4) {
      wazaSequenceSysStopWaza(iVar1,uVar4,4);
      wazaSequenceSysStopWaza(iVar1,uVar5,4);
      wazaSequenceSysStopWaza(iVar1,uVar6,4);
      wazaSequenceSysStopWaza(iVar1,uVar7,4);
      wazaSequenceSysStopWaza(iVar1,uVar8,4);
      if (r4 >= 4) {
        if (r3 == 0) {
          uVar2 = 0;
        }
        else {
          iVar1 = pokemonGetStatus(r3,0,0xd6,0);
          if (iVar1 == 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = (int)pokemonGetStatus(iVar1,0,0xcc,0);
          }
        }
        uVar9 = (int)pokemonGetStatus(uVar2,0,0x73,0);
        itemStatus = itemGetStatus(0,uVar9,0x10,0);
        if (itemStatus != 0) {
          callbackState[0] = (u32)r3;
          callbackState[1] = itemStatus;
          callbackState[2] = 0;
          fightFloorLoopValidFightOutPokemon(
              0, _fightOutPokemonRegWzxFreeSub__FPvUsPv, callbackState, 0);
        }
        iVar1 = pokemonGetStatus(r3,0,0xee,0);
        if (iVar1 != 0) {
          iVar3 = pokemonGetStatus(r3,0,0xee,0);
          if (iVar3 != 0) {
            wazaSequenceOwnerSetVisibility(iVar3,0);
          }
          pokemonSetStatus(r3,0,0xee,0,0);
          battleGridRemovePokemon(iVar1);
          wazaSequenceOwnerFree(iVar1);
        }
        fightMenuStatusMenuClose(r3,(u16)fightFloorGetStatus(0,0,0x14,0),1);
      }
      else if (r7 != (void *)0) {
        fn_801DA224(iVar1,*r7);
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
    extern void soundStart();
    extern u8 wazaSequenceSysIsAllEnd();
    extern void wazaSequenceSysStopWaza(void*, u16, u32);
    extern u8 wazaSequenceSysIsPlayingWaza(void*, u16, u32);
    extern void wazaSequenceSysStartWaza(void*, u16, u32);
    extern void fn_801DDD28(void*, u16, u32, u32);
  void* iVar1;
  u8 uVar6;
  u32 uVar4;
  u32 uVar5;
  u8 cVar7;
  u32 uVar2;
  u32 uVar8;
  u16 uVar9;

  iVar1 = pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar8 = 0;
    }
    else {
      uVar8 = (u32)fightPokemonGetPokemonPtrInline(pokemonGetStatus(r3,0,0xd6,0));
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
      wazaSequenceSysStartWaza(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar7 = wazaSequenceSysIsPlayingWaza(iVar1,uVar4,4);
        if (cVar7 == 0) break;
        _threadSwitch();
      }
    }
    else if (r4 == 3) {
      wazaSequenceSysStartWaza(iVar1,uVar5,4);
    }
    else if (r4 == 4) {
      while (1) {
        cVar7 = wazaSequenceSysIsAllEnd(0);
        if (cVar7 == 1) break;
        _threadSwitch();
      }
      uVar2 = (u32)fightOutPokemonGetPokemonPtrInline(r3);
      uVar9 = (int)pokemonGetStatus(uVar2,0,0x6e,0);
      uVar9 = (int)pokemonGetStatus(0,uVar9,0x61,0);
      soundStart(uVar9,0,0xff,0);
      while (1) {
        cVar7 = wazaSequenceSysIsPlayingWaza(iVar1,uVar5,4);
        if (cVar7 == 0) break;
        _threadSwitch();
      }
      cVar7 = pokemonCheckRare(uVar8);
      if (cVar7 == 1) {
        wazaSequenceSysStartWaza(iVar1,0x67,4);
        while (1) {
          cVar7 = wazaSequenceSysIsPlayingWaza(iVar1,0x67,4);
          if (cVar7 == 0) break;
          _threadSwitch();
        }
      }
    }
    else if (r4 == 5) {
      wazaSequenceSysStopWaza(iVar1,uVar4,4);
      wazaSequenceSysStopWaza(iVar1,uVar5,4);
      wazaSequenceSysStopWaza(iVar1,0x67,4);
    }
  }
  return;
}

/* Address: 0x80208ED0 | Size: 0x25c | Ghidra import */
void fightOutPokemonModosuEffect(void* r3, u8 r4)

{
    extern void _threadSwitch();
    extern void battleGridRemovePokemon();
    extern void wazaSequenceOwnerSetVisibility();
    extern void wazaSequenceSysStopWaza(void*, u16, u32);
    extern u8 wazaSequenceSysIsPlayingWaza(void*, u16, u32);
    extern void wazaSequenceSysStartWaza(void*, u16, u32);
    extern void wazaSequenceOwnerFree();
    extern void fn_801DDD28(void*, u16, u32, u32);
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern void fightMenuStatusMenuClose();
  void* iVar1;
  u32 uVar2;
  u8 uVar5;
  u32 uVar4;
  u8 cVar6;
  void* iVar3;
  u32 itemStatus;
  u32 callbackState[3];

  iVar1 = pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar2 = 0;
    }
    else {
      uVar2 = (u32)fightPokemonGetPokemonPtrInline(pokemonGetStatus(r3,0,0xd6,0));
    }
    uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar5,0xd,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
    }
    else if (r4 == 1) {
      wazaSequenceSysStartWaza(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar6 = wazaSequenceSysIsPlayingWaza(iVar1,uVar4,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
    }
    else if (r4 == 3) {
      wazaSequenceSysStopWaza(iVar1,uVar4,4);
    }
    else if (r4 == 4) {
      uVar2 = (u32)fightOutPokemonGetPokemonPtrInline(r3);
      uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
      itemStatus = itemGetStatus(0,uVar5,0x10,0);
      if (itemStatus != 0) {
        callbackState[0] = (u32)r3;
        callbackState[1] = itemStatus;
        callbackState[2] = 0;
        fightFloorLoopValidFightOutPokemon(
            0, _fightOutPokemonRegWzxFreeSub__FPvUsPv, callbackState, 0);
      }
      iVar1 = pokemonGetStatus(r3,0,0xee,0);
      if (iVar1 != 0) {
        iVar3 = pokemonGetStatus(r3,0,0xee,0);
        if (iVar3 != 0) {
          wazaSequenceOwnerSetVisibility(iVar3,0);
        }
        pokemonSetStatus(r3,0,0xee,0,0);
        battleGridRemovePokemon(iVar1);
        wazaSequenceOwnerFree(iVar1);
      }
      fightMenuStatusMenuClose(r3,(u16)fightFloorGetStatus(0,0,0x14,0),1);
    }
  }
  return;
}

/* Address: 0x8020912C | Size: 0x254 | Ghidra import */
void fightOutPokemonKizetuEffect(void* r3, u8 r4)

{
    extern void _threadSwitch();
    extern void battleGridRemovePokemon();
    extern void wazaSequenceOwnerSetVisibility();
    extern void wazaSequenceSysStopWaza(void*, u16, u32);
    extern u8 wazaSequenceSysIsPlayingWaza(void*, u16, u32);
    extern void wazaSequenceSysStartWaza(void*, u16, u32);
    extern void wazaSequenceOwnerFree();
    extern void fn_801DDD28(void*, u16, u32, u32);
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern void fightMenuStatusMenuClose();
  void* iVar1;
  u32 uVar2;
  u8 uVar5;
  u32 uVar4;
  u8 cVar6;
  void* iVar3;
  u32 itemStatus;
  u32 callbackState[3];

  iVar1 = pokemonGetStatus(r3,0,0xee,0);
  if (iVar1 != 0) {
    if (r3 == 0) {
      uVar2 = 0;
    }
    else {
      uVar2 = (u32)fightPokemonGetPokemonPtrInline(pokemonGetStatus(r3,0,0xd6,0));
    }
    uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
    uVar4 = itemGetStatus(0,uVar5,0x10,0);
    if (r4 == 0) {
      fn_801DDD28(iVar1,uVar4,4,0);
    }
    else if (r4 == 1) {
      wazaSequenceSysStartWaza(iVar1,uVar4,4);
    }
    else if (r4 == 2) {
      while (1) {
        cVar6 = wazaSequenceSysIsPlayingWaza(iVar1,uVar4,4);
        if (cVar6 == 0) break;
        _threadSwitch();
      }
      wazaSequenceSysStopWaza(iVar1,uVar4,4);
      fightMenuStatusMenuClose(r3,(u16)fightFloorGetStatus(0,0,0x14,0),1);
    }
    else if (r4 == 3) {
      uVar2 = (u32)fightOutPokemonGetPokemonPtrInline(r3);
      uVar5 = (int)pokemonGetStatus(uVar2,0,0x73,0);
      itemStatus = itemGetStatus(0,uVar5,0x10,0);
      if (itemStatus != 0) {
        callbackState[0] = (u32)r3;
        callbackState[1] = itemStatus;
        callbackState[2] = 0;
        fightFloorLoopValidFightOutPokemon(
            0, _fightOutPokemonRegWzxFreeSub__FPvUsPv, callbackState, 0);
      }
      iVar1 = pokemonGetStatus(r3,0,0xee,0);
      if (iVar1 != 0) {
        iVar3 = pokemonGetStatus(r3,0,0xee,0);
        if (iVar3 != 0) {
          wazaSequenceOwnerSetVisibility(iVar3,0);
        }
        pokemonSetStatus(r3,0,0xee,0,0);
        battleGridRemovePokemon(iVar1);
        wazaSequenceOwnerFree(iVar1);
      }
    }
  }
  return;
}

/* 0x80209380 | size: 0x104 */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fightOutPokemonDamageEffect(void* ctx) {
    extern u8 GSmodelGetVisibility();
    extern void GSmodelSetVisibility();
    extern void wazaSequenceOwnerSetVisibility();
    extern void* wazaSequenceOwnerGetModel();
    extern void fightMainWaitFrame();
    void* eeData;
    void* resolved;
    u8 i;

    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    resolved = !eeData ? NULL : wazaSequenceOwnerGetModel(eeData);
    if (resolved == NULL) { return; }
    if ((u8)GSmodelGetVisibility(resolved) == 0) { return; }
    for (i = 0; i < 8; i++) {
        eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
        if (eeData != NULL) {
            wazaSequenceOwnerSetVisibility(eeData, 1);
        }
        GSmodelSetVisibility(resolved, 1);
        fightMainWaitFrame(3);
        eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
        if (eeData != NULL) {
            wazaSequenceOwnerSetVisibility(eeData, 0);
        }
        fightMainWaitFrame(2);
    }
    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    if (eeData != NULL) {
        wazaSequenceOwnerSetVisibility(eeData, 1);
    }
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* 0x80209484 | size: 0x48 | small */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fightOutPokemonSetVisibility(void* ctx, u32 param) {
    extern void wazaSequenceOwnerSetVisibility();
    void* obj = pokemonGetStatus(ctx, 0, 0xee, 0);
    if (obj != 0) {
        wazaSequenceOwnerSetVisibility(obj, param);
    }
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* 0x802094CC | size: 0x90 | medium */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
void fightWazaDoEffect(u32 param_1, u32 param_2, u32 param_3, u8 param_4) {
    extern void _threadSwitch(void);
    extern u32 wazaGetStatus(void*, u32, u16, u32);
    extern void wazaSequenceSysStopWaza(u32, u16, u32);
    extern u8 wazaSequenceSysIsPlayingWaza(u32, u16, u32);
    extern void wazaSequenceSysStartWaza(u32, u16, u32);
    u32 uVar1;
    u8 cVar2;

    uVar1 = wazaGetStatus(NULL, param_2, 0x1F, 0);
    wazaSequenceSysStartWaza(param_1, uVar1, param_3);
    if (param_4 == 1) {
        while (1) {
            cVar2 = wazaSequenceSysIsPlayingWaza(param_1, uVar1, param_3);
            if (cVar2 == 0) break;
            _threadSwitch();
        }
        wazaSequenceSysStopWaza(param_1, uVar1, param_3);
    }
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* Address: 0x8020955C | Size: 0xbc | Ghidra import */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
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
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif

/* Address: 0x80209618 | Size: 0xd0 | Ghidra import */
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
u8 fightWazaIsMix(u32 ctx)
{
    extern StatusIdTable7 lbl_80279D08;
    extern u16 joutaiDataBiosGetStatusKind();
    extern u8 fn_8011B67C();
    u16 i;
    u16 statusId;
    u16 mixCount;
    u16 statusType;
    u8 isMix;
    StatusIdTable7 statusIds;

    mixCount = 0;
    statusIds = lbl_80279D08;
    for (i = 0; i < 7; i++) {
        statusId = statusIds.id[i];
        statusType = joutaiDataBiosGetStatusKind(statusId);
        if (statusType != 0x2A) {
            isMix = 0;
        } else {
            isMix = fn_8011B67C(ctx, statusId);
        }
        if (isMix == 1) {
            mixCount++;
        }
    }
    if (mixCount >= 2) {
        return 1;
    }
    return 0;
}
#if !defined(NO_LOCAL_PRAGMAS_FIGHT_OUT_POKEMON_EXACT_80207F5C)
#endif
#endif /* FIGHT_OUT_POKEMON_EXACT_80207F5C_ONLY */
