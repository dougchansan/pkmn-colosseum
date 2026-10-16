/**
 * @file fight_timer.c
 * @brief game/pxdvs/app/fight/fightTimer/fightTimer.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x802658C8-0x80265E34, 13 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTimer/fightTimer.cpp
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

typedef struct ColosseumBattleTimerState {
  u8 done;
  u8 forceDone;
  u8 pad[2];
  f32 elapsed;
  f32 limit;
  u32 thread;
} ColosseumBattleTimerState;

/* Address: 0x802658C8 | Size: 0x5C | Ghidra import */
void fightTimerCommandTerminate(void)
{
    extern ColosseumBattleTimerState lbl_80478800;
    extern void GSthreadTerminate(u32);
    ColosseumBattleTimerState *state;

    state = &lbl_80478800;
    if (state->thread != 0) {
        GSthreadTerminate(state->thread);
    }
    lbl_80478800.done = 0;
    state->thread = 0;
    lbl_80478800.elapsed = 0.0f;
    lbl_80478800.limit = 0.0f;
    lbl_80478800.forceDone = 0;
}

/* Address: 0x80265924 | Size: 0x38 | Ghidra import */
u32 fightTimerCommandIsOver(void)
{
    extern ColosseumBattleTimerState lbl_80478800;
    ColosseumBattleTimerState *state;

    state = &lbl_80478800;
    if (state->forceDone == 1) {
        return 0;
    }
    if (state->thread == 0) {
        return 1;
    }
    return state->done;
}

/* RULE-EXCEPTION(user-approved): reconstructed function the linker strips; its
 * body is inferred only from the .sdata2 pool layout (0.0f, 60.0f, then the
 * u32->f32 bias ahead of -3600.0f) — see docs/RULE_EXCEPTIONS.md */
f32 fightTimerFramesToSeconds(u32 frames)
{
    if (frames == 0) {
        return 0.0f;
    }
    return (f32)frames / 60.0f;
}

/* Address: 0x8026595C | Size: 0x48 | Ghidra import */
f32 fightTimerCommandGetNokoriTime(void)
{
    extern ColosseumBattleTimerState lbl_80478800;
    ColosseumBattleTimerState *state;

    state = &lbl_80478800;
    if (state->forceDone == 1) {
        return -3600.0f;
    }
    if (state->thread == 0) {
        return 0.0f;
    }
    return state->limit - state->elapsed / 60.0f;
}

/* Address: 0x802659A4 | Size: 0x54 | Ghidra import */
u32 fightTimerCommandBlock(void)
{
    extern ColosseumBattleTimerState lbl_80478800;
    extern void GSthreadBlock(u32);
    ColosseumBattleTimerState *state;

    state = &lbl_80478800;
    if (state->forceDone == 1) {
        return 0;
    }
    if (state->thread == 0) {
        return 0;
    }
    GSthreadBlock(state->thread);
    return 1;
}

/* Address: 0x802659F8 | Size: 0x74 | Ghidra import */
void fightTimerCommandStart(void)
{
    extern ColosseumBattleTimerState lbl_80478800;
    extern u32 floorGetCurrentContext(void);
    extern u32 GSthreadCreate(u32, u32, u32, u32, u32, u32);
    extern void GSthreadSetArgs(u32, u32, ...);
    extern void fightTimerThreadFunc(u8 *);
    u32 thread;

    if (lbl_80478800.forceDone != 1) {
        thread = GSthreadCreate(1, floorGetCurrentContext(), 0x4000, 1, 0, (u32)fightTimerThreadFunc);
        if (thread != 0) {
            lbl_80478800.thread = thread;
            GSthreadSetArgs(thread, 1);
        }
    }
}


/* Address: 0x80265A6C | Size: 0xD0 | Ghidra import */
void fightTimerCommandInit(void)
{
    extern ColosseumBattleTimerState lbl_80478800;
    extern s32 fn_80077B84(void);
    extern void GSthreadTerminate(u32);
    ColosseumBattleTimerState *state;
    f32 limit;

    limit = (f32)fn_80077B84();
    state = &lbl_80478800;
    if (state->thread != 0) {
        if (state->thread != 0) {
            GSthreadTerminate(state->thread);
        }
        lbl_80478800.done = 0;
        state->thread = 0;
        lbl_80478800.elapsed = 0.0f;
        lbl_80478800.limit = 0.0f;
        lbl_80478800.forceDone = 0;
    }
    lbl_80478800.done = 0;
    lbl_80478800.elapsed = 0.0f;
    if (limit < 0.0f) {
        lbl_80478800.forceDone = 1;
        lbl_80478800.limit = -3600.0f;
    }
    else {
        lbl_80478800.forceDone = 0;
        lbl_80478800.limit = limit;
    }
    state->thread = 0;
}


/* Address: 0x80265B3C | Size: 0x38 | Ghidra import */
u32 fightTimerAllIsOver(void)
{
    extern ColosseumBattleTimerState lbl_80478810;
    ColosseumBattleTimerState *state;

    state = &lbl_80478810;
    if (state->forceDone == 1) {
        return 0;
    }
    if (state->thread == 0) {
        return 1;
    }
    return state->done;
}

/* Address: 0x80265B74 | Size: 0x48 | Ghidra import */
f32 fightTimerAllGetNokoriTime(void)
{
    extern ColosseumBattleTimerState lbl_80478810;
    ColosseumBattleTimerState *state;

    state = &lbl_80478810;
    if (state->forceDone == 1) {
        return -3600.0f;
    }
    if (state->thread == 0) {
        return 0.0f;
    }
    return state->limit - state->elapsed / 60.0f;
}

/* Address: 0x80265BBC | Size: 0x54 | Ghidra import */
u32 fightTimerAllBlock(void)
{
    extern ColosseumBattleTimerState lbl_80478810;
    extern void GSthreadBlock(u32);
    ColosseumBattleTimerState *state;

    state = &lbl_80478810;
    if (state->forceDone == 1) {
        return 0;
    }
    if (state->thread == 0) {
        return 0;
    }
    GSthreadBlock(state->thread);
    return 1;
}

/* Address: 0x80265C10 | Size: 0x74 | Ghidra import */
void fightTimerAllStart(void)
{
    extern ColosseumBattleTimerState lbl_80478810;
    extern u32 floorGetCurrentContext(void);
    extern u32 GSthreadCreate(u32, u32, u32, u32, u32, u32);
    extern void GSthreadSetArgs(u32, u32, ...);
    extern void fightTimerThreadFunc(u8 *);
    u32 thread;

    if (lbl_80478810.forceDone != 1) {
        thread = GSthreadCreate(1, floorGetCurrentContext(), 0x4000, 1, 0, (u32)fightTimerThreadFunc);
        if (thread != 0) {
            lbl_80478810.thread = thread;
            GSthreadSetArgs(thread, 1);
        }
    }
}


/* Address: 0x80265C84 | Size: 0xD0 | Ghidra import */
void fightTimerAllInit(void)
{
    extern ColosseumBattleTimerState lbl_80478810;
    extern s32 menuCBRule_GetBattleTimeLimit(void);
    extern void GSthreadTerminate(u32);
    ColosseumBattleTimerState *state;
    f32 limit;

    limit = (f32)menuCBRule_GetBattleTimeLimit();
    state = &lbl_80478810;
    if (state->thread != 0) {
        if (state->thread != 0) {
            GSthreadTerminate(state->thread);
        }
        lbl_80478810.done = 0;
        state->thread = 0;
        lbl_80478810.elapsed = 0.0f;
        lbl_80478810.limit = 0.0f;
        lbl_80478810.forceDone = 0;
    }
    lbl_80478810.done = 0;
    lbl_80478810.elapsed = 0.0f;
    if (limit < 0.0f) {
        lbl_80478810.forceDone = 1;
        lbl_80478810.limit = -3600.0f;
    }
    else {
        lbl_80478810.forceDone = 0;
        lbl_80478810.limit = limit;
    }
    state->thread = 0;
}


/* Address: 0x80265D54 | Size: 0x5C | Ghidra import */
void fightTimerAllTerminate(void)
{
    extern ColosseumBattleTimerState lbl_80478810;
    extern void GSthreadTerminate(u32);
    ColosseumBattleTimerState *state;

    state = &lbl_80478810;
    if (state->thread != 0) {
        GSthreadTerminate(state->thread);
    }
    lbl_80478810.done = 0;
    state->thread = 0;
    lbl_80478810.elapsed = 0.0f;
    lbl_80478810.limit = 0.0f;
    lbl_80478810.forceDone = 0;
}


/* Address: 0x80265DB0 | Size: 0x84 | Ghidra import (PSQ removed) */
void fightTimerThreadFunc(ColosseumBattleTimerState *r3)
{
    extern u32 GSgetFrameStep(void);
    extern void _threadSwitch(void);

  r3->elapsed = 0.0f;
  while (r3->elapsed < 60.0f * r3->limit) {
    _threadSwitch();
    r3->elapsed = (f32)GSgetFrameStep() + r3->elapsed;
  }
  r3->done = 1;
  do {
    _threadSwitch();
  } while (1);
}

