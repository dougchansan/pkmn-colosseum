/**
 * @file memo.c
 * @brief game/pxdvs/app/memo/memo.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8025FA20-0x802600E4, 9 fns.
 *
 * XD source unit: game/pxdvs/app/memo/memo.cpp
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

/* Single-function carves: memo_r57b_8025FA20_prefix.c (memoGetScaleAngle)
 * and memo_r57b_8025FBCC_o2.c (memoInitDebug). */
#if defined(MEMO_8025FA20_ONLY) || defined(MEMO_8025FBCC_ONLY)
#define PR424_MEMO_SPLIT
#define PR424_MEMO_8025FA20_8025FEE4
#endif

#if !defined(PR424_MEMO_SPLIT) || \
    defined(PR424_MEMO_8025FA20_8025FEE4)
#if !defined(MEMO_8025FBCC_ONLY)
/* Address: 0x8025FA20 | Size: 0x1AC */
void memoGetScaleAngle(u16 id, f32* scaleOut, f32* angleOut)
{
    extern f32 lbl_8047E690;
    extern f32 lbl_8047E694;
    extern f32 lbl_8047E698;
    extern f32 lbl_8047E69C;
    extern f32 lbl_8047E6A0;
    extern f32 lbl_8047E6A4;
    extern f32 lbl_8047E6A8;
    extern f32 lbl_8047E6AC;
    f32 scale;

    scale = lbl_8047E690;
    switch (id) {
    case 0x26:
        scale = lbl_8047E694;
        break;
    case 0x4B:
        scale = lbl_8047E698;
        break;
    case 0x91:
        scale = lbl_8047E69C;
        break;
    case 0x12C:
        scale = lbl_8047E6A0;
        break;
    case 0x8E:
        scale = lbl_8047E69C;
        break;
    case 0x92:
        scale = lbl_8047E6A4;
        break;
    case 0x90:
        scale = lbl_8047E6A4;
        break;
    case 0xFA:
        scale = lbl_8047E698;
        break;
    case 0x197:
        scale = lbl_8047E6A4;
        break;
    case 0x198:
        scale = lbl_8047E6A4;
        break;
    case 0x1A:
        scale = lbl_8047E6A0;
        break;
    case 0x6:
        scale = lbl_8047E6A0;
        break;
    case 0xE2:
        scale = lbl_8047E69C;
        break;
    case 0xD9:
        scale = lbl_8047E69C;
        break;
    case 0x14B:
        scale = lbl_8047E6A8;
        break;
    case 0x44:
        scale = lbl_8047E6A8;
        break;
    case 0x136:
        scale = lbl_8047E69C;
        break;
    case 0x4A:
        scale = lbl_8047E698;
        break;
    case 0x16B:
        scale = lbl_8047E698;
        break;
    case 0x85:
        scale = lbl_8047E698;
        break;
    }
    if (angleOut != NULL) {
        *angleOut = lbl_8047E6AC;
    }
    if (scaleOut != NULL) {
        *scaleOut = scale;
    }
}

#endif

#if !defined(MEMO_8025FA20_ONLY)
/* Adds a debug memo record for one species id unless it is already there. */
static inline void memoDataSetDebugInline(u16 id)
{
    u16* memo;
    u16 i;
    u16 entryID;

    memo = (u16*)savedataGetStatus(0, 0xC);
    for (i = 0; i < *memo; i++) {
        entryID = memo[i * 6 + 2] & 0x3FFF;
        if (entryID == id) {
            return;
        }
    }
    memo[*memo * 6 + 2] = id | 0x8000;
    *(u32*)((u8*)memo + *memo * 12 + 0xC) = _fadeEffectGetRandom__FUl(-1);
    *memo = *memo + 1;
}

/* Address: 0x8025FBCC | Size: 0x168 */
void memoInitDebug(u16* block)
{
    u16 id;

    if (block == NULL) {
        block = (u16*)savedataGetStatus(0, 0xC);
    }
    for (id = 1; id <= 0xFB; id++) {
        memoDataSetDebugInline(id);
    }
    for (id = 0x115; id <= 0x19B; id++) {
        memoDataSetDebugInline(id);
    }
}
#endif

#if !defined(MEMO_8025FA20_ONLY) && !defined(MEMO_8025FBCC_ONLY)
#if 0
asm void memoDataGetPokemonTrainerRndFromID(void) {
#include "src/game/colosseum_battle_fn_8025FD34.inc"
}
#endif
u32 memoDataGetPokemonTrainerRndFromID(u16 *r3, u16 r4) {
	  u16 *queue;
	  u16 *countQueue;
	  u16 count;
	  u32 r5;
	  u32 i;
	  s32 r0;
  queue = r3;
  if (queue == (u16 *)0) {
    queue = (u16 *)savedataGetStatus(0, 0xC);
  }
  countQueue = queue;
	  if (queue == (u16 *)0) {
	    countQueue = (u16 *)savedataGetStatus(0, 0xC);
	  }
	  count = (u32)*countQueue;
	  for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
    r5 = (i & 0xffff) * 12;
    r0 = r5 + 4;
    r0 = (u32)*(u16 *)((u8 *)queue + r0) & 0x3FFF;
			    if (r0 == (u16)r4) {
		      queue = (u16 *)((u8 *)queue + r5);
		      return *(u32 *)((u8 *)queue + 8);
		    }
  }
  return 0;
}

#if 0
asm void memoDataGetPokemonRndFromID(void) {
#include "src/game/colosseum_battle_fn_8025FDDC.inc"
}
#endif
u32 memoDataGetPokemonRndFromID(u16 *r3, u16 r4) {
  u16 *queue;
  u16 *countQueue;
  u32 r5;
  u32 i;
  u16 count;
  s32 r0;
  queue = r3;
  if (queue == (u16 *)0) {
    queue = (u16 *)savedataGetStatus(0, 0xC);
  }
  countQueue = queue;
  if (queue == (u16 *)0) {
    countQueue = (u16 *)savedataGetStatus(0, 0xC);
  }
  count = (u32)*countQueue;
	  for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
	    r5 = (i & 0xffff) * 12;
	    r0 = r5 + 4;
	    r0 = (u32)*(u16 *)((u8 *)queue + r0) & 0x3FFF;
		    if (r0 == (u16)r4) {
		      queue = (u16 *)((u8 *)queue + r5);
		      return *(u32 *)((u8 *)queue + 0xC);
		    }
  }
  return 0;
}

/* Address: 0x8025FE84 | Size: 0x60 | Ghidra import */
u16 memoDataGetPokemonID(u16 *r3, u32 r4)
{
    u16 res;
    extern void *savedataGetStatus();
    if (r3 == (u16 *)0) {
        r3 = (u16 *)savedataGetStatus(0, 0xc);
    }
    if (*r3 != 0) {
        r3 = (u16 *)((u8 *)r3 + (r4 & 0xFFFF) * 12);
        res = *(u16 *)((u8 *)r3 + 4);
    } else {
        res = 0;
    }
    return res;
}
#endif
#endif

#if !defined(PR424_MEMO_SPLIT)
/* Address: 0x8025FEE4 | Size: 0x34 | Ghidra import */
u16 memoDataGetCount(u16 *r3)

{
  if (r3 == (u16 *)0x0) {
    r3 = (u16 *)savedataGetStatus(0,0xc);
  }
  return *r3;
}
#endif

#if !defined(PR424_MEMO_SPLIT) || \
    defined(PR424_MEMO_8025FF18_802600E4)
/* Address: 0x8025FF18 | Size: 0x84 | Ghidra import */
u32 memoDataSetMemoFlag(u16 *r3)

{
  u32 uVar2;
  u32 uVar3;
  u32 uVar1;
  
  uVar3 = 0;
  if (r3 == (u16 *)0x0) {
    r3 = (u16 *)savedataGetStatus(0,0xc);
  }
  uVar2 = 0;
  while ((u32)(u16)uVar2 < (u32)*r3) {
    uVar1 = (uVar2 & 0xffff) * 12;
    if ((*(volatile u16 *)((u8 *)r3 + uVar1 + 4) & 0x8000) != 0) {
      uVar3 = 1;
    }
    uVar2 = uVar2 + 1;
    *(volatile u16 *)((u8 *)r3 + uVar1 + 4) = *(volatile u16 *)((u8 *)r3 + uVar1 + 4) & 0x3fff;
  }
  return uVar3;
}

#if 0
asm void fn_8025FF9C(void) {
#include "src/game/colosseum_battle_fn_8025FF9C.inc"
}
#endif
void memoDataSet(u16 *r3, u32 r4) {
  extern u32 pokemonBiosGetPokemonDataId();
  u16 *queue;
  u32 r30;
		  u32 count;
		  u32 rawLookup;
		  u16 lookup;
		  u32 r5;
		  u32 i;
		  u32 r0;

  queue = r3;
  r30 = r4;
  if (queue == (u16 *)0) {
    queue = (u16 *)savedataGetStatus(0, 0xC);
  }
  rawLookup = pokemonBiosGetPokemonDataId(r30);
	  count = *queue;
	  for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
	    lookup = rawLookup;
	    r5 = (i & 0xffff) * 12;
	    r0 = (u32)*(u16 *)((u8 *)queue + r5 + 4) & 0x3FFF;
	    if (r0 == lookup) {
	      return;
	    }
	  }
		  queue[(u32)count * 6 + 2] = (u16)(rawLookup | 0x8000);
	  r5 = pokemonBiosGetRnd(r30);
	  lookup = *queue;
	  *(u32 *)((u8 *)queue + ((u32)lookup * 12 + 0xC)) = r5;
	  r5 = pokemonBiosGetCatchTrainerRnd(r30);
	  *(u32 *)((u8 *)queue + ((u32)*queue * 12 + 0x8)) = r5;
  *queue = (u16)(*queue + 1);
}

#if 0
asm void memoInit(void) {
#include "src/game/colosseum_battle_fn_80260070.inc"
}
#endif
void memoInit(u16 *r3) {
  u32 r31;
  u32 r30;
  u16 *r29;
  u32 offsetMask;
  u8 *r28;
  s32 r27;
  r29 = r3;
  if (r29 == (u16 *)0) {
    r29 = (u16 *)savedataGetStatus(0, 0xC);
  }
  r31 = 0;
  r30 = r31;
  r27 = r31;
  *r29 = r30;
  r31 = r30;
  while (r27 < 0x1F4) {
    /* The full-width mask preserves the original MWCC register allocation. */
    offsetMask = ~0u;
    r28 = (u8 *)r29 + (r31 & offsetMask);
    *(u16 *)(r28 + 4) = r30;
    *(u32 *)(r28 + 0xC) = _fadeEffectGetRandom__FUl(-1);
    r27++;
    r31 += 12;
  }
}
#endif
