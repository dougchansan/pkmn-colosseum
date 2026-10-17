/**
 * @file char_name_bios.c
 * @brief game/pxdvs/app/charName/charNameBios.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x80261388-0x802614B4, 3 fns.
 *
 * XD source unit: game/pxdvs/app/charName/charNameBios.cpp
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

/* Address: 0x80261388 | Size: 0x4C | Ghidra import */
u32 charNameBiosSearchIndex(u32 value)
{
    extern u32 *lbl_80478F80;
    extern u32 *lbl_80478F84;
    u32 *table;
    u32 count;
    u32 i;
    u32 offset;

    table = lbl_80478F84;
    count = *lbl_80478F80;
    for (i = 0; (u16)i < count; i++) {
        offset = ((u16)i) << 3;
        if (*(u32 *)((u8 *)table + offset + 4) == value) {
            return i;
        }
    }
    return 0;
}

/* Address: 0x802613D4 | Size: 0x70 | Ghidra import */
u32 charNameBiosGetNameID(u32 idx) {
    extern void* lbl_80478F80;
    extern void* lbl_80478F84;
    extern void GSlogWrite(char*, char*, ...);
    extern char lbl_8027A4C8[];
    extern char lbl_8039A6C8[];
    u8* entry;
    u16 i = (u16)idx;

    if (i >= *(u32*)lbl_80478F80) {
        GSlogWrite(lbl_8027A4C8, lbl_8039A6C8);
        entry = NULL;
    } else {
        entry = (u8*)lbl_80478F84 + i * 8;
    }
    if (entry == NULL) {
        return 0;
    }
    return *(u32*)(entry + 4);
}
/* Address: 0x80261444 | Size: 0x70 | Ghidra import */
u32 charNameBiosGetHearFlag(u32 idx)
{
    extern void* lbl_80478F80;
    extern void* lbl_80478F84;
    extern void GSlogWrite(char*, char*, ...);
    extern char lbl_8027A4C8[];
    extern char lbl_8039A6C8[];
    u8* entry;
    u16 i = (u16)idx;

    if (i >= *(u32*)lbl_80478F80) {
        GSlogWrite(lbl_8027A4C8, lbl_8039A6C8);
        entry = NULL;
    } else {
        entry = (u8*)lbl_80478F84 + i * 8;
    }
    if (entry == NULL) {
        return 0;
    }
    return *(u16*)entry;
}
