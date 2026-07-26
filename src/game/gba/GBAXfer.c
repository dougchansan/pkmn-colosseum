/**
 * @file GBAXfer.c
 * @brief gba/GBAXfer.c -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8025F70C-0x8025FA20, 5 fns.
 *
 * XD source unit: gba/GBAXfer.c
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
#pragma peephole off
/* Address: 0x8025F70C | Size: 0xDC | Ghidra import */
void __GBAHandler(int chan, u32 status, void* currentContext)

{
    extern void OSClearContext(void*);
    extern void OSSetCurrentContext(void*);
  u8* entry;
  void (*callback1)(int, u32);
  u8 context[0x2d0];

  entry = lbl_804783E0 + chan * 0x100;
  if (lbl_8047B670 != 0) {
    return;
  }
  if ((status & 0xf) != 0) {
    *(u32 *)(entry + 0x20) = 1;
  }
  else {
    *(u32 *)(entry + 0x20) = 0;
  }
  if (*(u32 *)(entry + 0x38) != 0) {
    void (*callback0)(int) = *(void (**)(int))(entry + 0x38);
    *(u32 *)(entry + 0x38) = 0;
    callback0(chan);
  }
  if (*(u32 *)(entry + 0x1c) != 0) {
    OSClearContext(context);
    OSSetCurrentContext(context);
    callback1 = *(void (**)(int, u32))(entry + 0x1c);
    *(u32 *)(entry + 0x1c) = 0;
    callback1(chan, *(u32 *)(entry + 0x20));
    OSClearContext(context);
    OSSetCurrentContext(currentContext);
  }
}
#pragma pop

/* Address: 0x8025F7E8 | Size: 0x34 | Ghidra import */
#pragma peephole off
void __GBASyncCallback(int r3)

{
    extern void OSWakeupThread(void *);
  OSWakeupThread(lbl_804783E0 + r3 * 0x100 + 0x24);
}
#pragma peephole on

/* Address: 0x8025F81C | Size: 0x6C | Ghidra import */
#pragma push
#pragma optimize_for_size off
u32 __GBASync(int r3)

{
  extern u32 OSDisableInterrupts(void);
  extern void OSRestoreInterrupts(u32);
  extern void OSSleepThread(void *);
  u8 *entry;
  u32 interrupts;
  u32 status;

  entry = lbl_804783E0 + r3 * 0x100;
  interrupts = OSDisableInterrupts();
  while (*(u32 *)(entry + 0x1c) != 0) {
    OSSleepThread(entry + 0x24);
  }
  status = *(u32 *)(entry + 0x20);
  OSRestoreInterrupts(interrupts);
  return status;
}
#pragma pop

/* Address: 0x8025F888 | Size: 0x124 | Ghidra import */
void TypeAndStatusCallback(int r3,u32 r4)

{
  extern void *OSGetCurrentContext(void);
  extern void OSClearContext(void *);
  extern void OSSetCurrentContext(void *);
  extern void __OSReschedule(void);
  extern int SITransfer(int, void *, u32, void *, u32, u32, u32, u32);
  u8 *entry;
  void (*callback1)(int, u32);
  void *currentContext;
  u8 context[0x2d0];

  entry = lbl_804783E0 + r3 * 0x100;
  if (lbl_8047B670 == 0) {
    if (((r4 & 0xff) != 0) || ((r4 & 0xffff0000) != 0x40000)) {
      *(u32 *)(entry + 0x20) = 1;
    }
    else {
      if (SITransfer(r3, entry, *(u32 *)(entry + 0xc),
                     entry + 0x5, *(u32 *)(entry + 0x10),
                     (u32)__GBAHandler, *(u32 *)(entry + 0x30),
                     *(u32 *)(entry + 0x34)) != 0) {
        return;
      }
      *(u32 *)(entry + 0x20) = 2;
    }
    if (*(u32 *)(entry + 0x38) != 0) {
      void (*callback0)(int) = *(void (**)(int))(entry + 0x38);
      *(u32 *)(entry + 0x38) = 0;
      callback0(r3);
    }
    if (*(u32 *)(entry + 0x1c) != 0) {
      currentContext = OSGetCurrentContext();
      OSClearContext(context);
      OSSetCurrentContext(context);
      callback1 = *(void (**)(int, u32))(entry + 0x1c);
      *(u32 *)(entry + 0x1c) = 0;
      callback1(r3, *(u32 *)(entry + 0x20));
      OSClearContext(context);
      OSSetCurrentContext(currentContext);
      __OSReschedule();
    }
  }
}

/* Address: 0x8025F9AC | Size: 0x74 | Ghidra import */
u32 __GBATransfer(int r3,u32 r4,u32 r5,u32 r6)

{
  extern u32 OSDisableInterrupts(void);
  extern void OSRestoreInterrupts(u32);
  extern int SIGetTypeAsync();
  u8 *entry;
  u32 interrupts;

  entry = lbl_804783E0 + r3 * 0x100;
  interrupts = OSDisableInterrupts();
  *(u32 *)(entry + 0x38) = r6;
  *(u32 *)(entry + 0xc) = r4;
  *(u32 *)(entry + 0x10) = r5;
  SIGetTypeAsync(r3,(u32)TypeAndStatusCallback);
  OSRestoreInterrupts(interrupts);
  return 0;
}
