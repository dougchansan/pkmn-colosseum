/**
 * @file fight_menu_exact_80264ADC.c
 * @brief _fightMenuFightTrainerGcHeroOpenMenuSubBallSelectTargetPokemon, 0x80264ADC - 0x80264D58.
 *
 * Function-boundary carve of fightMenu.cpp (see fight_menu.c): a data-free
 * exact function between the GcHero menu candidates. GC/1.3 -O4,s, no
 * pragmas.
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
extern void fadeSet(f32 duration, u32 mode);
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
u32 fightMenuFightTrainerGcHeroOpenMenu(u32 trainer, u32 side, u32 canCancel);

/* Address: 0x80264ADC | Size: 0x27C | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubBallSelectTargetPokemon__FP15FightOutPokemonUsUs(u32 r3,u32 r4,u32 r5)

{
    extern int menuFightCloseTarget();
    extern int menuFightOpenTarget();
    extern int winMsgCloseFight();
    extern int winMsgOpenFight();
    extern int std082_flagGetValue();
    extern int fightMainWaitFrame();
  u16 uVar4;
  u32 uVar1;
  u8 cVar6;
  int iVar2;
  u16 sVar5;
  u32 uVar3;
  u32 found;
  u8 auStack_38 [0x24];
  u32 local_34;
  u32 local_2c;
  u32 local_24;
  u32 local_1c;
  u8 local_18;
  u8 local_17;
  
LAB_00261af4:
  uVar1 = fightTargetGetPtr(0xf,r3,r5);
    cVar6 = fightOutPokemonCheckFightOut();
    if (cVar6 == '\x01') {
    found = fightTargetGetPtr(2,uVar1,r5);
    if (found == 0) {
      uVar3 = 0;
    }
    else {
      uVar4 = fightSideGetStatus(found,0,5,0);
      sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1,r5);
      if (sVar5 == 0) {
        uVar3 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar3 = fightTargetDataBiosGetBuff();
        if ((int)uVar3 < 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
        }
      }
    }
  }
  else {
    uVar3 = 0;
  }
  *(u32 *)(auStack_38 + 4) = uVar3;
  {
    u32 uVar1b;
    u16 uVar4b;

    uVar1b = fightTargetGetPtr(0x10,r3,r5);
    cVar6 = fightOutPokemonCheckFightOut();
    if (cVar6 == '\x01') {
      found = fightTargetGetPtr(2,uVar1b,r5);
      if (found == 0) {
        uVar3 = 0;
      }
      else {
        uVar4b = fightSideGetStatus(found,0,5,0);
        sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1b,r5);
        if (sVar5 == 0) {
          uVar3 = 0;
        }
        else {
          fightTargetDataBiosGetPtr();
          uVar3 = fightTargetDataBiosGetBuff();
          if ((int)uVar3 < 0) {
            uVar3 = 0;
          }
          else {
            uVar3 = fightSideGetStatus(0,uVar4b,3,uVar3 & 0xffff);
          }
        }
      }
    }
    else {
      uVar3 = 0;
    }
  }
  *(u32 *)(auStack_38 + 0xc) = uVar3;
  *(u32 *)(auStack_38 + 0x14) = 0;
  *(u32 *)(auStack_38 + 0x1c) = 0;
  auStack_38[0x20] = 2;
  auStack_38[0x21] = fn_801F18DC(0);
  iVar2 = menuFightOpenTarget((int*)auStack_38,0,1);
  if (iVar2 < 0) {
    menuFightCloseTarget(1);
    return 0;
  }
  if (iVar2 != 0) goto LAB_00261cb0;
  uVar1 = fightTargetGetPtr(0xf,r3,r5);
  goto LAB_00261ccc;
LAB_00261cb0:
  if (iVar2 == 1) {
    uVar1 = fightTargetGetPtr(0x10,r3,r5);
LAB_00261ccc:
    cVar6 = fightOutPokemonCheckFightOut(uVar1);
    if (cVar6 != '\0') {
      fightOutPokemonGetPokemonPtr(uVar1);
      cVar6 = pokemonIsDarkPokemon();
      if (cVar6 == '\0') {
        uVar3 = std082_flagGetValue(0x99f);
        if (uVar3 == 0) {
          winMsgOpenFight(0x7716,1,1);
        }
        else {
          winMsgOpenFight(0x7702,1,1);
        }
        fightMainWaitFrame(0x40);
        winMsgCloseFight(0);
        goto LAB_00261af4;
      }
      menuFightCloseTarget(1);
      return uVar1;
    }
  }
  goto LAB_00261af4;
}
