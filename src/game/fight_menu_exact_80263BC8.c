/**
 * @file fight_menu_exact_80263BC8.c
 * @brief _fightMenuFightTrainerGcHeroOpenMenuSubMain, 0x80263BC8 - 0x80263DE4.
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

/* Address: 0x80263BC8 | Size: 0x21C | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubMain__FP13FIGHT_TRAINERP15FightOutPokemonUsl(u32 r3, u32 r4, u32 r5, s32 mode)
{
    extern u8 fightOutPokemonCheckFightActionWazaSelect(u32, u32);
    extern u8 fightFloorGetStatus(u32, u32, u32, u32);
    extern void msgctrlSetValue(u32, u32);
    extern void winMsgOpenFight(u32, u32, u32);
    extern void fightMainWaitFrame(u32);
    extern void winMsgCloseFight(u32);
    extern u8 _fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32, u32, u32);
    extern u8 _fightMenuFightTrainerGcHeroOpenMenuSubItem__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32, u32, u32);
    extern s32 fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(u32, u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void *, s32);
    extern char lbl_80375D30[];
    u8 flag;
    u32 result;

    result = 0;
    switch (mode) {
    case 0:
        flag = fightOutPokemonCheckFightActionWazaSelect(r4, 1);
        if (flag == 1) {
            msgctrlSetValue(0x11, r4);
            winMsgOpenFight(0x75fc, 1, 1);
            fightMainWaitFrame(0x40);
            winMsgCloseFight(0);
        }
        if (flag == 0) {
            if (_fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs(r3, r4, r5) == 0) {
                result = 2;
            }
        }
        break;
    case 1:
        if (fightFloorGetStatus(0, 0, 0x20, 0) == 0) {
            winMsgOpenFight(0x75f5, 1, 1);
            fightMainWaitFrame(0x40);
            winMsgCloseFight(0);
            result = 2;
        }
        else if (_fightMenuFightTrainerGcHeroOpenMenuSubItem__FP13FIGHT_TRAINERP15FightOutPokemonUs(r3, r4, r5) == 0) {
            result = 2;
        }
        break;
    case 2:
        r5 = fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(r3, r4, r5, 1, 1);
        if ((s16)r5 < 0) {
            flag = 0;
        }
        else {
            fightOutPokemonCreateFightAction(r4, 0, 9, 0, lbl_80375D30, (s16)(r5 & 0xFFFFFFFFFFFFFFFFu));
            flag = 1;
        }
        if (flag == 0) {
            result = 2;
        }
        break;
    case 3:
        if (fightFloorGetStatus(0, 0, 0x22, 0) == 1) {
            fightOutPokemonCreateFightAction(r4, 0, 8, 0, lbl_80375D30, 0);
        }
        else if (fightFloorGetStatus(0, 0, 0x21, 0) == 1) {
            fightOutPokemonCreateFightAction(r4, 0, 0xa, 0, lbl_80375D30, 0);
        }
        else {
            result = 2;
        }
        break;
    default:
        result = 1;
        break;
    }
    return result;
}
