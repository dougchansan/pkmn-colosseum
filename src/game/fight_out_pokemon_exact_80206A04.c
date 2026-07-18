/**
 * @file fight_out_pokemon_exact_80206A04.c
 * @brief Matching fightOutPokemon + fightPokemon range
 *        0x80206A04-0x80207C6C, 17 functions.
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

/* Retail marks every function in this physical island active. */
#pragma force_active on

/* Address: 0x80206A04 | Size: 0xe8 | Ghidra import */
#pragma push
#pragma peephole on
u32 fightPokemonCheckValid(void* ctx) {
    extern u8 pokemonCheckValid();
    extern u16 fn_801EF634();
    u32 uVar1;
    u16 sVar3;
    void* iVar2;
    u8 cVar4;
    if (ctx == 0) { return 0; }
    sVar3 = fn_801EF634();
    if (sVar3 == 1) { return 0; }
    iVar2 = pokemonGetStatus(ctx, 0, 0xcb, 0);
    if (iVar2 == 0) { return 0; }
    cVar4 = pokemonCheckValid();
    if (cVar4 == 0) { return 0; }
    if (ctx == 0) {
        iVar2 = 0;
    } else {
        iVar2 = pokemonGetStatus(ctx, 0, 0xcc, 0);
    }
    if (iVar2 == 0) { return 0; }
    cVar4 = pokemonCheckValid();
    if (cVar4 == 0) { return 0; }
    uVar1 = (u32)pokemonGetStatus(ctx, 0, 0xce, 0);
    return uVar1 >> 0x1f ^ 1;
}
#pragma pop

/* Address: 0x80206AEC | Size: 0x150 | Ghidra import */
#pragma push
#pragma peephole on
void fightPokemonCreate(void* p1, void* p2, s16 p3) {
    extern void fn_8011B950();
    extern void pokemonBiosCopy();
    extern void pokemonInit();
    u32 uVar1;

    if (p1 != 0 && p2 != 0) {
        if (p1 != 0) {
            pokemonSetStatus(p1, 0, 0xcb, 0, 0);
            pokemonGetStatus(p1, 0, 0xcc, 0);
            pokemonInit();
            uVar1 = (u32)pokemonGetStatus(p1, 0, 0xcd, 0);
            fn_8011B950(uVar1, 1);
            pokemonSetStatus(p1, 0, 0xce, 0, (void*)0xffffffff);
            pokemonSetStatus(p1, 0, 0xcf, 0, 0);
            pokemonSetStatus(p1, 0, 0xd0, 0, 0);
            pokemonSetStatus(p1, 0, 0xd1, 0, 0);
            pokemonSetStatus(p1, 0, 0xd2, 0, 0);
        }
        pokemonSetStatus(p1, 0, 0xcb, 0, p2);
        uVar1 = (u32)pokemonGetStatus(p1, 0, 0xcc, 0);
        pokemonBiosCopy(uVar1, p2);
        pokemonSetStatus(p1, 0, 0xce, 0, (s32)p3);
    }
}
#pragma pop

/* 0x80206C3C | size: 0x58 | small */
#pragma push
#pragma peephole on
void fightOutPokemonInitAry(u32 param_1, u16 param_2) {
    extern void fightOutPokemonInit(u32);
    u32 n;
    int iVar1;

    if (param_1 != 0) {
        iVar1 = 0;
        n = param_2;
        for (; (u16)iVar1 < n; iVar1 = iVar1 + 1) {
            fightOutPokemonInit(param_1 + (u32)(u16)iVar1 * 0x6E0);
        }
    }
}
#pragma pop

/* Address: 0x80206C94 | Size: 0x72c | Ghidra import */
#pragma push
#pragma peephole on
#pragma optimization_level 2
void fightOutPokemonInit(int r3)

{
    extern StatusIdTable7 lbl_80279C60;
    extern void fn_8011B950();
    extern void pokemonInit();
    extern void fightActionInit();
    extern void fn_801FD830();
    extern void fightWazaInit();
    extern void fightItemInit();
    extern void fightOutPokemonEnemyInitAry();
  u32 uVar2;
  u32 *puVar3;
  u16 *puVar4;
  u8 bVar4;
  void* iVar1;
  StatusIdTable7 local_28;

  if (r3) {
    pokemonSetStatus(r3,0,0xd5,0,0);
    pokemonSetStatus(r3,0,0xd6,0,0);
    if ((iVar1 = pokemonGetStatus(r3,0,0xd7,0)) != NULL) {
      pokemonSetStatus(iVar1,0,0xcb,0,0);
      pokemonGetStatus(iVar1,0,0xcc,0);
      pokemonInit();
      uVar2 = (u32)pokemonGetStatus(iVar1,0,0xcd,0);
      fn_8011B950(uVar2,1);
      pokemonSetStatus(iVar1,0,0xce,0, (void*)0xffffffff);
      pokemonSetStatus(iVar1,0,0xcf,0,0);
      pokemonSetStatus(iVar1,0,0xd0,0,0);
      pokemonSetStatus(iVar1,0,0xd1,0,0);
      pokemonSetStatus(iVar1,0,0xd2,0,0);
    }
    uVar2 = (int)pokemonGetStatus(r3,0,0xd8,0);
    fn_8011B950(uVar2,0x34);
    local_28 = lbl_80279C60;
    puVar4 = local_28.id;
    for (bVar4 = 0; bVar4 < 7; bVar4++) {
      pokemonSetStatus(r3,0,puVar4[bVar4],0,6);
    }
    fn_801FD830(r3,0);
    pokemonSetStatus(r3,0,0xed,0,2);
    pokemonSetStatus(r3,0,0xee,0,0);
    for (bVar4 = 0; bVar4 < 0xc; bVar4++) {
      pokemonSetStatus(r3,0,0xfd,bVar4, (void*)0xffffffff);
    }
    iVar1 = pokemonGetStatus(r3,0,0xfe,0);
    if (iVar1 != NULL) {
      fightActionInit();
      pokemonGetStatus(r3,0,0xd9,0);
      fightWazaInit();
      pokemonGetStatus(r3,0,0xe5,0);
      fightItemInit();
    }
    pokemonGetStatus(r3,0,0xf8,0);
    fightWazaInit();
    for (bVar4 = 0; bVar4 < 2; bVar4++) {
      pokemonSetStatus(r3,0,0xff,bVar4,9);
    }
    pokemonSetStatus(r3,0,0x100,0,0);
    puVar3 = (u32 *)pokemonGetStatus(r3,0,0x101,0);
    if (puVar3 != (void *)0) {
      *puVar3 = 0;
    }
    pokemonSetStatus(r3,0,0xef,0,0);
    pokemonSetStatus(r3,0,0xf0,0,0);
    pokemonSetStatus(r3,0,0xf1,0,0);
    pokemonSetStatus(r3,0,0xf2,0,0);
    pokemonSetStatus(r3,0,0xf3,0,0);
    pokemonSetStatus(r3,0,0xf4,0,9);
    pokemonSetStatus(r3,0,0xf5,0,0);
    pokemonSetStatus(r3,0,0xf6,0,0);
    pokemonSetStatus(r3,0,0xf7,0,0);
    pokemonSetStatus(r3,0,0xf9,0,0);
    pokemonSetStatus(r3,0,0xfc,0,0);
    pokemonSetStatus(r3,0,0xfb,0,0);
    pokemonSetStatus(r3,0,0x102,0,0);
    pokemonSetStatus(r3,0,0x103,0,0);
    pokemonSetStatus(r3,0,0x104,0,0);
    pokemonSetStatus(r3,0,0x105,0,0);
    pokemonSetStatus(r3,0,0x106,0,0);
    pokemonSetStatus(r3,0,0x107,0,0);
    pokemonSetStatus(r3,0,0x108,0,0);
    pokemonSetStatus(r3,0,0x109,0,0);
    pokemonSetStatus(r3,0,0x10a,0,0);
    pokemonSetStatus(r3,0,0x10b,0,0);
    pokemonSetStatus(r3,0,0x10c,0,0);
    pokemonSetStatus(r3,0,0x10d,0,0);
    pokemonSetStatus(r3,0,0x10e,0,0);
    pokemonSetStatus(r3,0,0x10f,0,0);
    pokemonSetStatus(r3,0,0x110,0,0);
    pokemonSetStatus(r3,0,0x111,0,0);
    pokemonSetStatus(r3,0,0x112,0,0);
    pokemonSetStatus(r3,0,0x113,0,0);
    pokemonSetStatus(r3,0,0x114,0,0);
    pokemonSetStatus(r3,0,0x115,0,0);
    pokemonSetStatus(r3,0,0x116,0,0);
    pokemonSetStatus(r3,0,0x117,0,0);
    pokemonSetStatus(r3,0,0x118,0,0);
    pokemonSetStatus(r3,0,0x119,0,0);
    pokemonSetStatus(r3,0,0x11a,0,0);
    pokemonSetStatus(r3,0,0x11b,0,0);
    pokemonSetStatus(r3,0,0x11c,0,0);
    pokemonSetStatus(r3,0,0x11d,0,0);
    pokemonSetStatus(r3,0,0x11e,0,0);
    pokemonSetStatus(r3,0,0x11f,0,0);
    pokemonSetStatus(r3,0,0x120,0,0);
    pokemonSetStatus(r3,0,0x121,0, (void*)0xffffffff);
    uVar2 = (int)pokemonGetStatus(r3,0,0x122,0);
    fightOutPokemonEnemyInitAry(uVar2,4);
  }
  return;
}
#pragma pop

/* Address: 0x802073C0 | Size: 0x88 | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
void fightOutPokemonInitAbiCntAll(u32 r3)

{
    extern StatusIdTable7 lbl_80279C60;
  u8 bVar1;
  StatusIdTable7 local_28;

  local_28 = lbl_80279C60;
  for (bVar1 = 0; bVar1 < 7; bVar1++) {
    pokemonSetStatus(r3,0,local_28.id[bVar1],0,6);
  }
  return;
}
#pragma pop

/* 0x80207448 | size: 0x15C | medium */
#pragma push
#pragma peephole on
void fightOutPokemonInitOneSelfTurn(void* param_1) {
    pokemonSetStatus(param_1, 0, 0x113, 0, 0);
    pokemonSetStatus(param_1, 0, 0x114, 0, 0);
    pokemonSetStatus(param_1, 0, 0x115, 0, 0);
    pokemonSetStatus(param_1, 0, 0x116, 0, 0);
    pokemonSetStatus(param_1, 0, 0x117, 0, 0);
    pokemonSetStatus(param_1, 0, 0x118, 0, 0);
    pokemonSetStatus(param_1, 0, 0x119, 0, 0);
    pokemonSetStatus(param_1, 0, 0x11A, 0, 0);
    pokemonSetStatus(param_1, 0, 0x11B, 0, 0);
    pokemonSetStatus(param_1, 0, 0x11C, 0, 0);
    pokemonSetStatus(param_1, 0, 0x11D, 0, 0);
    pokemonSetStatus(param_1, 0, 0x11E, 0, 0);
    pokemonSetStatus(param_1, 0, 0x11F, 0, 0);
}
#pragma pop

/* 0x802075A4 | size: 0x1BC | medium */
#pragma push
#pragma peephole on
void fightOutPokemonInitOneTurn(void* param_1) {
    pokemonSetStatus(param_1, 0, 0x102, 0, 0);
    pokemonSetStatus(param_1, 0, 0x103, 0, 0);
    pokemonSetStatus(param_1, 0, 0x104, 0, 0);
    pokemonSetStatus(param_1, 0, 0x105, 0, 0);
    pokemonSetStatus(param_1, 0, 0x106, 0, 0);
    pokemonSetStatus(param_1, 0, 0x107, 0, 0);
    pokemonSetStatus(param_1, 0, 0x108, 0, 0);
    pokemonSetStatus(param_1, 0, 0x109, 0, 0);
    pokemonSetStatus(param_1, 0, 0x10A, 0, 0);
    pokemonSetStatus(param_1, 0, 0x10B, 0, 0);
    pokemonSetStatus(param_1, 0, 0x10C, 0, 0);
    pokemonSetStatus(param_1, 0, 0x10D, 0, 0);
    pokemonSetStatus(param_1, 0, 0x10E, 0, 0);
    pokemonSetStatus(param_1, 0, 0x10F, 0, 0);
    pokemonSetStatus(param_1, 0, 0x110, 0, 0);
    pokemonSetStatus(param_1, 0, 0x111, 0, 0);
    pokemonSetStatus(param_1, 0, 0x112, 0, 0);
}
#pragma pop

/* 0x80207760 | size: 0x74 | small */
#pragma push
#pragma peephole on
#pragma scheduling on
void fightOutPokemonInitFightActionBuff(void* param_1) {
    extern void fightActionInit(void*);
    extern void fightWazaInit(void*);
    extern void fightItemInit(void*);
    void* iVar1;

    iVar1 = pokemonGetStatus(param_1, 0, 0xFE, 0);
    if (iVar1 != NULL) {
        fightActionInit(iVar1);
        fightWazaInit(pokemonGetStatus(param_1, 0, 0xD9, 0));
        fightItemInit(pokemonGetStatus(param_1, 0, 0xE5, 0));
    }
}
#pragma pop

/* 0x802077D4 | size: 0x11C */
#pragma push
#pragma peephole on
void fightPokemonInitAry(void* basePtr, u16 count) {
    extern void fn_8011B950();
    extern void pokemonInit();
    void* entry;
    u16 i;

    if (basePtr == NULL) { return; }
    for (i = 0; i < count; i++) {
        entry = (void*)((u32)basePtr + i * 0x154);
        if (entry == NULL) { continue; }
        pokemonSetStatus(entry, 0, 0xCB, 0, 0);
        pokemonInit(pokemonGetStatus(entry, 0, 0xCC, 0));
        fn_8011B950(pokemonGetStatus(entry, 0, 0xCD, 0), 1);
        pokemonSetStatus(entry, 0, 0xCE, 0, (u32)-1);
        pokemonSetStatus(entry, 0, 0xCF, 0, 0);
        pokemonSetStatus(entry, 0, 0xD0, 0, 0);
        pokemonSetStatus(entry, 0, 0xD1, 0, 0);
        pokemonSetStatus(entry, 0, 0xD2, 0, 0);
    }
}
#pragma pop

/* Address: 0x802078F0 | Size: 0xec | Ghidra import */
#pragma push
#pragma peephole on
void fightPokemonInit(void* r3)
{
    extern void fn_8011B950();
    extern void pokemonInit();
    void* ctx;
    u32 uVar1;

    if ((ctx = r3) != NULL) {
        pokemonSetStatus(ctx, 0, 0xcb, 0, 0);
        pokemonGetStatus(ctx, 0, 0xcc, 0);
        pokemonInit();
        uVar1 = (u32)pokemonGetStatus(ctx, 0, 0xcd, 0);
        fn_8011B950(uVar1, 1);
        pokemonSetStatus(ctx, 0, 0xce, 0, (void*)0xffffffff);
        pokemonSetStatus(ctx, 0, 0xcf, 0, 0);
        pokemonSetStatus(ctx, 0, 0xd0, 0, 0);
        pokemonSetStatus(ctx, 0, 0xd1, 0, 0);
        pokemonSetStatus(ctx, 0, 0xd2, 0, 0);
    }
}
#pragma pop

/* 0x802079DC | size: 0x104 */
#pragma push
#pragma peephole on
u32 fightOutPokemonGetTeikouZokuseiDataIdAry(void* ctx, void* battleCtx, u32* outSlots) {
    extern u16 zokuseiGetWazaJoutai(void*, u16);
    u16 i;
    int outCount;
    u16 slot0;
    u16 slot1;
    u16 result;
    u8 isPlayerSlot;

    for (i = 0; i < 0x12; i++) {
        outSlots[i] = (u32)-1;
    }
    outCount = 0;
    for (i = 0; i < 0x12; i++) {
        if (i == (u16)(u32)pokemonGetStatus(ctx, 0, 0xFF, 0)) {
            goto _set1;
        }
        if (i == (u16)(u32)pokemonGetStatus(ctx, 0, 0xFF, 1)) {
        _set1:
            isPlayerSlot = 1;
        } else {
            isPlayerSlot = 0;
        }
        if (isPlayerSlot == 1) { continue; }
        result = zokuseiGetWazaJoutai(battleCtx, i);
        if (result == 0x42 || result == 0x43) {
            outSlots[(u16)outCount] = i;
            outCount++;
        }
    }
    return outCount;
}
#pragma pop

/* Address: 0x80207AE0 | Size: 0x7c | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
u32 fightOutPokemonIsZokuseiDataId(void* r3, u16 r4)

{
  u16 sVar2;
  u32 uVar1;

  sVar2 = (u16)(u32)pokemonGetStatus(r3,0,0xff,0);
  if ((r4 == sVar2) || (sVar2 = (u16)(u32)pokemonGetStatus(r3,0,0xff,1), r4 == sVar2)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
#pragma pop

/* 0x80207B5C | size: 0x30 */
#pragma scheduling on
#pragma peephole on
u32 fightOutPokemonSetZokuseiDataId(void* context, u8 flags, u16 value) {
    return pokemonSetStatus(context, 0, 0xFF, flags, value);
}
#pragma peephole reset
#pragma scheduling reset

/* 0x80207B8C | size: 0x34 */
#pragma push
#pragma scheduling on
#pragma peephole on
u16 fightOutPokemonGetZokuseiDataId(void* context, u8 field) {
    return (u16)(u32)pokemonGetStatus(context, 0, 0xFF, field);
}
#pragma pop

/* 0x80207BC0 | size: 0x34 */
#pragma push
#pragma scheduling on
#pragma peephole on
u32 fightOutPokemonSetTokuseiDataId(void* context, u16 value) {
    return pokemonSetStatus(context, 0, 0x100, 0, value);
}
#pragma pop

/* 0x80207BF4 | size: 0x30 */
#pragma push
#pragma scheduling on
#pragma peephole on
u16 fightOutPokemonGetTokuseiDataId(void* context) {
    return (u16)(u32)pokemonGetStatus(context, 0, 0x100, 0);
}
#pragma pop

/* 0x80207C24 | size: 0x48 | small */
#pragma push
#pragma scheduling on
#pragma peephole on
void fightOutPokemonSetWazaEffectDownFlag(void* ctx, u32 param) {
    extern void fn_801DA5AC();
    void* obj = pokemonGetStatus(ctx, 0, 0xee, 0);
    if (obj != 0) {
        fn_801DA5AC(obj, param);
    }
}
#pragma pop
