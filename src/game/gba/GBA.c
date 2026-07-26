/**
 * @file GBA.c
 * @brief gba/GBA.c -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8025F2FC-0x8025F524, 5 fns.
 *
 * XD source unit: gba/GBA.c
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void fn_801EF8F4();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void fn_80165A20();     /* Fade out music */
extern void fn_801659FC();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void ShortCommandProc(void) {
#include "src/game/colosseum_battle_fn_8025F2FC.inc"
}
#endif
#pragma pop
void ShortCommandProc(int r3) {
  u8 *entry;
  u8 b7;
  entry = lbl_804783E0 + (r3 * 0x100);
  if (*(s32 *)(entry + 0x20) != 0) return;
  if ((*(u8 *)(entry + 0x5) != 0) || (*(u8 *)(entry + 0x6) != 4)) {
    *(s32 *)(entry + 0x20) = 1;
    return;
  }
  b7 = *(u8 *)(entry + 0x7);
  *(u8 *)(*(u32 *)(entry + 0x14)) = b7 & 0x3a;
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8025F350(void) {
#include "src/game/colosseum_battle_fn_8025F350.inc"
}
#endif
#pragma pop
void GBAInit(void) {
  u32 r30;
  u8 *r29;
  u8 *r28;
  int i;
  u32 r0;
  r0 = *(u32 *)0x800000F8;
  r0 = r0 >> 2;
  r0 = __mulhwu(0x431BDE83u, r0);
  r0 = r0 >> 15;
  r0 = r0 * 60;
  r30 = r0 >> 3;
  r29 = lbl_804783E0;
  r28 = lbl_804782E0;
  for (i = 0; i < 4; i++) {
    *(u32 *)(r29 + 0x34) = r30;
    *(u32 *)(r29 + 0x30) = 0;
    OSInitThreadQueue((void *)(r29 + 0x24));
    *(u32 *)(r29 + 0xF8) = (u32)r28;
    r29 += 0x100;
    r28 += 0x40;
  }
  OSInitAlarm();
  DSPInit();
  lbl_8047B670 = 0;
  OSRegisterResetFunction(lbl_8039A6B8);
}

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GBAGetStatus(void) {
#include "src/game/colosseum_battle_fn_8025F3F4.inc"
}
#endif
#pragma pop
static inline s32 GBAGetStatusAsync(s32 chan, u32 status, u32 callback) {
  u8 *entry = lbl_804783E0 + chan * 0x100;
  if (*(u32 *)(entry + 0x1C) != 0) {
    return 2;
  }
  *(u8 *)(entry + 0x0) = 0;
  *(u32 *)(entry + 0x14) = status;
  *(u32 *)(entry + 0x1C) = callback;
  return __GBATransfer(chan, 1, 3, (u32)ShortCommandProc);
}

#pragma push
#pragma peephole off
u32 GBAGetStatus(int r3, u32 r4) {
  s32 result;
  result = GBAGetStatusAsync(r3, r4, (u32)__GBASyncCallback);
  if (result == 0) {
    result = __GBASync(r3);
  }
  return result;
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GBAReset(void) {
#include "src/game/colosseum_battle_fn_8025F484.inc"
}
#endif
#pragma pop
static inline s32 GBAResetAsync(s32 chan, u32 status, u32 callback) {
  u8 *entry = lbl_804783E0 + chan * 0x100;
  if (*(u32 *)(entry + 0x1C) != 0) {
    return 2;
  }
  *(u8 *)(entry + 0x0) = 0xFF;
  *(u32 *)(entry + 0x14) = status;
  *(u32 *)(entry + 0x1C) = callback;
  return __GBATransfer(chan, 1, 3, (u32)ShortCommandProc);
}

#pragma push
#pragma peephole off
u32 GBAReset(int r3, u32 r4) {
  s32 result;
  result = GBAResetAsync(r3, r4, (u32)__GBASyncCallback);
  if (result == 0) {
    result = __GBASync(r3);
  }
  return result;
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void OnReset(void) {
#include "src/game/colosseum_battle_fn_8025F514.inc"
}
#endif
#pragma pop
#pragma scheduling off
u32 OnReset(void) {
  lbl_8047B670 = 1;
  return 1;
}
