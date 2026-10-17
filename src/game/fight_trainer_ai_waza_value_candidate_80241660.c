/**
 * @file fight_trainer_ai_waza_value_candidate_80241660.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x80241660 - 0x80241B70.
 *
 * Physically split from fight_trainer_ai_waza_value.c so this
 * translation unit owns only the functions in the stated range.
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
extern void battleCameraSelect();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

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
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
u8 fightTrainerAiIsJoutaiNormal(void*, u32);
u8 fn_80237F74(void*, u32, u32);
u8 fn_802384B4(void*, u32, u32);
u8 fn_80239564(void*, u32);
u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
u16 fightTrainerAiGetPokemonDataId2(void*, u32);
u8 fightTrainerAiGetPokemonZokusei(void*, u32, u32);
u8 fn_80239058(void*, u32, u32);
u32 fn_80239984(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);


/* Address: 0x80241660 | Size: 0x510 (1296 bytes) */
u32 fightTrainerAiWazaValueTedasuke(void *ctx, u32 param1, u32 param2, u32 param3)
{
  extern u32 *lbl_80478DF8;
  extern u8 fightActionCheckValid(void *);
  extern u16 fightActionGetKindDataId(void *);
  extern u32 fightOutPokemonGetUseWazaDataId(u32);
  extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void *, u32 *, u32, u32);
  extern s32 fn_80202108(u32, u32);
  extern s32 fn_80202234(u32, u32);
  extern u16 fightTrainerAiGetFightPokemonWazaAry(void *, u32, u16 *, u32, u32);
  extern u8 fightTrainerAiIsPokemonJoutai(void *, u32, u32);
  extern u8 fn_8023943C(void *, u32, u32);
  unsigned int new_var2;
  u32 enemy[6];
  u16 waza[10];
  u32 handle;
  u16 enemyCount;
  u16 wazaCount;
  u16 i;
  u16 j;
  u8 supportWazaOut;
  u8 allEnemySupport;
  void *action;
  u32 pokemon;
  s32 need;
  s8 enough;
  /* RULE-EXCEPTION(user-approved): chained zeroing, need reused as the move loop counter and a split need computation, chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
  allEnemySupport = (handle = 0);
  enemyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, enemy, 1, 1);
  supportWazaOut = 0;
  for (need = 0; (need & 0xFFFF) < (*lbl_80478DF8); need++)
  {
    if (((((u16) need) == 0) || (((u16) need) == 0x165)) || (((u16) need) == 0x163))
    {
      continue;
    }
    if (fn_8023943C(ctx, need, 1) == 0)
    {
      continue;
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, need, 0) == 1)
    {
      supportWazaOut = 1;
    }
  }

  if (supportWazaOut == 1)
  {
    handle = fn_80239984(0, ctx, 0x1b0);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, allEnemySupport, 0x1b0);
  }
  for (i = 0; i < enemyCount; i++)
  {
    if (param1 == enemy[i])
    {
      continue;
    }
    wazaCount = fightTrainerAiGetFightPokemonWazaAry(ctx, enemy[i], waza, 0, 1);
    if (wazaCount == 0)
    {
      continue;
    }
    for (j = 0; j < wazaCount; j++)
    {
      if (fn_8023943C(ctx, waza[j], 1) != 0)
      {
        handle = fn_80239984(handle, ctx, 0x1b1);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b1);
        break;
      }
    }

  }

  allEnemySupport = 1;
  for (i = 0; i < enemyCount; i++)
  {
    if (param1 == enemy[i])
    {
      continue;
    }
    action = pokemonGetStatus(enemy[i], 0, 0xfe, 0);
    if (action == ((void *) 0))
    {
      continue;
    }
    if (fightActionCheckValid(action) == 0)
    {
      continue;
    }
    if (fightActionGetKindDataId(action) != 0x13)
    {
      allEnemySupport = 0;
      break;
    }
    pokemon = fightOutPokemonGetUseWazaDataId(enemy[i]);
    if (fn_8023943C(ctx, pokemon, 1) == 0)
    {
      allEnemySupport = 0;
      break;
    }
  }

  if (allEnemySupport == 0)
  {
    handle = fn_80239984(handle, ctx, 0x1b2);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b2);
  }
  for (i = 0; i < enemyCount; i++)
  {
    if (param1 == enemy[i])
    {
      continue;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, enemy[i], 0x12) != 1)
    {
      continue;
    }
    handle = fn_80239984(handle, ctx, 0x1b3);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b3);
    break;
  }

  for (i = 0; i < enemyCount; i++)
  {
    if (param1 == enemy[i])
    {
      continue;
    }
    if (((u8) ((u32) pokemonGetStatus(enemy[i], 0, 0xf9, 0))) == 0)
    {
      continue;
    }
    handle = fn_80239984(handle, ctx, 0x1b4);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b4);
    break;
  }

  for (i = 0; i < enemyCount; i++)
  {
    pokemon = enemy[i];
    if (param1 == pokemon)
    {
      continue;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, pokemon, 8) == 0)
    {
      enough = -1;
    }
    else
    {
      need = fn_80237F74(ctx, pokemon, 0x30);
      need = (need == 1) ? 2 : 1;
      new_var2 = fn_80202108(pokemon, 8);
      need = new_var2 + need;
      if (((s8) need) >= ((s8) fn_80202234(pokemon, 8)))
      {
        enough = 1;
      }
      else
      {
        enough = 0;
      }
    }
    if (enough != 0)
    {
      continue;
    }
    handle = fn_80239984(handle, ctx, 0x1b5);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1b5);
    break;
  }

  return handle;
}
