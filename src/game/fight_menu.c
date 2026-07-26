/**
 * @file fight_menu.c
 * @brief game/pxdvs/app/fight/fightMenu.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x80261B68-0x802658C8, 33 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightMenu.cpp
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

#if defined(FIGHT_MENU_CANDIDATE_80261B68)

/* =========================================================================
 * fightMenuFightTrainerGcHeroOpenMenu - FinalCleanup
 *
 * Large function near the end of the range (0xA5C = 2652 bytes).
 * One of the last significant functions before .ctors at 0x80266360.
 *
 * Likely performs comprehensive cleanup after a Colosseum session:
 *   - Reset temporary state
 *   - Save progress
 *   - Release loaded resources
 *   - Restore overworld state
 * ========================================================================= */
/* TODO: Decompile fightMenuFightTrainerGcHeroOpenMenu (2652 bytes) */
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2) {
    extern void menuFightCloseTop();
    extern void fn_80011A1C();
    extern void menuFightStatusSetActive();
    extern void menuCloseCustom();
    extern void menuIsCheck();
    extern void menuOpenCustom();
    extern void fn_801EF634();
    extern void fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern void fightTargetDataBiosGetBuff();
    extern void fightTargetDataBiosGetPtr();
    extern void fightTargetGetPtr();
    extern void fightFloorIsUseFightTimerCommand();
    extern void fn_801F18DC();
    extern void fightFloorSetMenuFightAction();
    extern void fightSideGetStatus();
    extern void fightTrainerIsUsedItem();
    extern void fightTrainerTimeOutSelectFightAction();
    extern void fightTrainerAllInitFightActionBuff();
    extern void fightTrainerGetValidFightOutPokemonPtr();
    extern void fightOutPokemonCheckFightActionSelect();
    extern void fightOutPokemonInitFightActionBuff();
    extern void fightTypeDataBiosGetFightoutPokemonNum();
    extern void fightTypeDataBiosGetPtr();
    extern u32 _fightMenuFightTrainerGcHeroOpenMenuSubMain__FP13FIGHT_TRAINERP15FightOutPokemonUsl();
    extern void fightTimerCommandIsOver();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f3 = 0.0f;
    f32 f5 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = 0;

    r31 = r4;
    r29 = r3;
    r25 = r5;
    r3 = r31;
    fightTypeDataBiosGetPtr();
    fightTypeDataBiosGetFightoutPokemonNum();
    r27 = r3 & 0xFF;
    r3 = 0x0;
    fn_801F18DC();
    r0 = r3 & 0xFF;
    if ((s32)r0 != (s32)0) {
    do {
        r4 = r29;
        r5 = r31;
        r24 = 0x100;
        r3 = 0x2;
        fightTargetGetPtr();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0x5;
        r6 = 0x0;
        fightSideGetStatus();
        r26 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
        r0 = r3 & 0xFFFF;
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        fightTargetDataBiosGetPtr();
        fightTargetDataBiosGetBuff();
        if ((s32)r3 < (s32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = r26;
        r6 = r3 & 0xFFFF;
        r3 = 0x0;
        r5 = 0x2;
        fightSideGetStatus();
    } while (0);
    do {
        if ((s32)r3 != (s32)0xf3) {
            if ((s32)r3 < (s32)0xf3) {
                if ((s32)r3 != (s32)0xf1) {
                    if ((s32)r3 < (s32)0xf1) {
                        break;
                    }
                    if ((s32)r3 >= (s32)0xf5) break;
                    continue;
                    }
                r24 = 0x100;
                break;
                    }
            r24 = 0x101;
            break;
        }
        r24 = 0x102;
        break;

        r24 = 0x103;
    } while (0);
        r3 = r24;
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        menuOpenCustom();
    }
    r30 = 0x0;
    r26 = 0x0;
while (1) {
        r0 = r30 & 0xFFFF;
        if (r0 >= r27) break;
    do {
        r3 = r29;
        r4 = r30;
        fightTrainerGetValidFightOutPokemonPtr();
        if ((s32)r3 == (s32)0xf5) {
            r26 = r30;
            break;
        }
        r4 = 0x1;
        fightOutPokemonCheckFightActionSelect();
        r0 = r3 & 0xFF;
        if ((s32)r3 == (s32)0xf5) {
            r26 = r30;
            break;
        }
        fn_801EF634();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x1) {
    while (1) {
        r3 = r29;
        fightTrainerAllInitFightActionBuff();
        break;
        }
        r3 = 0x0;
        fightFloorIsUseFightTimerCommand();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            fightTimerCommandIsOver();
            r0 = r3 & 0xFF;
        }
        if (r0 == (u32)0x1) {
    while (1) {
        r3 = r28;
        fightOutPokemonInitFightActionBuff();
        r3 = r29;
        r4 = r28;
        r5 = r31;
        fightTrainerTimeOutSelectFightAction();
        r26 = r30;
        break;
        }
        r3 = 0x0;
        fn_801F18DC();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
        do {
            r4 = r28;
            r5 = r31;
            r3 = 0x2;
            fightTargetGetPtr();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = 0x0;
            r5 = 0x5;
            r6 = 0x0;
            fightSideGetStatus();
            r24 = r3 & 0xFFFF;
            r3 = r28;
            r4 = r31;
            fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            fightTargetDataBiosGetPtr();
            fightTargetDataBiosGetBuff();
            if ((s32)r3 < (s32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = r24;
            r6 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x3;
            fightSideGetStatus();
        } while (0);
            r4 = 0x1;
            menuFightStatusSetActive();
        }
        r0 = 0x0;
        *(u32*)(sp + 0x8) = r0;
    while (1) {
            r3 = r28;
            fightOutPokemonInitFightActionBuff();
            r4 = r28;
            r5 = (u32)sp + 0xc;
            r3 = 0x0;
            fightFloorSetMenuFightAction();
            r3 = 0x0;
            fn_801F18DC();
            *(u8*)(sp + 0x23) = r3;
            r3 = (u32)sp + 0xc;
            r4 = (u32)sp + 0x8;
            r5 = 0x1;
            fn_80011A1C();
            r24 = r3;
            fn_801EF634();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x1) {
                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fightTargetGetPtr();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fightTargetDataBiosGetPtr();
                    fightTargetDataBiosGetBuff();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    menuFightStatusSetActive();
                }
                r3 = 0x1;
                menuFightCloseTop();
                continue;
            }
            r3 = 0x0;
            fightFloorIsUseFightTimerCommand();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                fightTimerCommandIsOver();
                r0 = r3 & 0xFF;
            }
            if (r0 == (u32)0x1 || (s32)r24 >= (s32)0x0) {

                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fightTargetGetPtr();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fightTargetDataBiosGetPtr();
                    fightTargetDataBiosGetBuff();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    menuFightStatusSetActive();
                }
                r3 = 0x1;
                menuFightCloseTop();
                continue;
            }
            if ((s32)r24 >= (s32)0x0) {
                r3 = 0x1;
                menuFightCloseTop();
            }
            r3 = r29;
            r4 = r28;
            r5 = r31;
            r6 = r24;
            _fightMenuFightTrainerGcHeroOpenMenuSubMain__FP13FIGHT_TRAINERP15FightOutPokemonUsl();
            r24 = r3;
            fn_801EF634();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x1) {
                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fightTargetGetPtr();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fightTargetDataBiosGetPtr();
                    fightTargetDataBiosGetBuff();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    menuFightStatusSetActive();
                }
                r3 = 0x1;
                menuFightCloseTop();
    }
            }
            r3 = 0x0;
            fightFloorIsUseFightTimerCommand();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                fightTimerCommandIsOver();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r0 = r24 & 0xFF;
            }
            }
            if (r0 != (u32)0x1) {
                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fightTargetGetPtr();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fightTargetDataBiosGetPtr();
                    fightTargetDataBiosGetBuff();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    menuFightStatusSetActive();
                }
                r3 = 0x1;
                menuFightCloseTop();
    }
            }
            r0 = r24 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r29;
                fightTrainerIsUsedItem();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) continue;
                r3 = r30 & 0xFFFF;
                if (r0 != (u32)0x1) {
                    r30 = r26;
                    r3 = 0x0;
                    fn_801F18DC();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                    do {
                        r4 = r28;
                        r5 = r31;
                        r3 = 0x2;
                        fightTargetGetPtr();
                        if (r3 == (u32)0x0) {
                            r3 = 0x0;
                            break;
                        }
                        r4 = 0x0;
                        r5 = 0x5;
                        r6 = 0x0;
                        fightSideGetStatus();
                        r24 = r3 & 0xFFFF;
                        r3 = r28;
                        r4 = r31;
                        fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
                        r0 = r3 & 0xFFFF;
                        if (r3 == (u32)0x0) {
                            r3 = 0x0;
                            break;
                        }
                        fightTargetDataBiosGetPtr();
                        fightTargetDataBiosGetBuff();
                        if ((s32)r3 < (s32)0x0) {
                            r3 = 0x0;
                            break;
                        }
                        r4 = r24;
                        r6 = r3 & 0xFFFF;
                        r3 = 0x0;
                        r5 = 0x3;
                        fightSideGetStatus();
                    } while (0);
                        r4 = 0x0;
                        menuFightStatusSetActive();
                    }
                    r3 = 0x1;
                    menuFightCloseTop();
                    continue;
                }
                r0 = r25 & 0xFF;
                if (r0 != (u32)0x1 || r3 != (u32)0x0) continue;

                r3 = 0x0;
                fn_801F18DC();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                do {
                    r4 = r28;
                    r5 = r31;
                    r3 = 0x2;
                    fightTargetGetPtr();
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = 0x0;
                    r5 = 0x5;
                    r6 = 0x0;
                    fightSideGetStatus();
                    r24 = r3 & 0xFFFF;
                    r3 = r28;
                    r4 = r31;
                    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
                    r0 = r3 & 0xFFFF;
                    if (r3 == (u32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    fightTargetDataBiosGetPtr();
                    fightTargetDataBiosGetBuff();
                    if ((s32)r3 < (s32)0x0) {
                        r3 = 0x0;
                        break;
                    }
                    r4 = r24;
                    r6 = r3 & 0xFFFF;
                    r3 = 0x0;
                    r5 = 0x3;
                    fightSideGetStatus();
                } while (0);
                    r4 = 0x0;
                    menuFightStatusSetActive();
                }
                r3 = 0x1;
                menuFightCloseTop();
                r24 = 0x0;
                while (1) {
                    r0 = r24 & 0xFFFF;
                    if (r0 >= (u32)r27) break;
                    r3 = r29;
                    r4 = r24;
                    fightTrainerGetValidFightOutPokemonPtr();
                    if (r3 != (u32)0x0) {
                        r4 = 0x0;
                        r5 = 0x120;
                        r6 = 0x0;
                        r7 = 0x0;
                        ((void(*)(void))pokemonSetStatus)();
                    }
                    r24 = r24 + 0x1;

                }
                r3 = 0x0;
                return r3;
            }
            if (r0 == (u32)0x2) continue;
        break;
    }
        r3 = 0x0;
        fn_801F18DC();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
        do {
            r4 = r28;
            r5 = r31;
            r3 = 0x2;
            fightTargetGetPtr();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = 0x0;
            r5 = 0x5;
            r6 = 0x0;
            fightSideGetStatus();
            r24 = r3 & 0xFFFF;
            r3 = r28;
            r4 = r31;
            fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            fightTargetDataBiosGetPtr();
            fightTargetDataBiosGetBuff();
            if ((s32)r3 < (s32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = r24;
            r6 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x3;
            fightSideGetStatus();
        } while (0);
            r4 = 0x0;
            menuFightStatusSetActive();
        }
        r26 = r30;
    } while (0);
        r30 = r30 + 0x1;
        r0 = r30 & 0xFFFF;
        if (r0 < r27) continue;
    break;
}

    r3 = 0x0;
    fn_801F18DC();
    r0 = r3 & 0xFF;
    if (r0 != (u32)r27) {
    do {
        r4 = r29;
        r5 = r31;
        r24 = 0x100;
        r3 = 0x2;
        fightTargetGetPtr();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = 0x0;
        r5 = 0x5;
        r6 = 0x0;
        fightSideGetStatus();
        r25 = r3 & 0xFFFF;
        r3 = r29;
        r4 = r31;
        fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
        r0 = r3 & 0xFFFF;
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            break;
        }
        fightTargetDataBiosGetPtr();
        fightTargetDataBiosGetBuff();
        if ((s32)r3 < (s32)0x0) {
            r3 = 0x0;
            break;
        }
        r4 = r25;
        r6 = r3 & 0xFFFF;
        r3 = 0x0;
        r5 = 0x2;
        fightSideGetStatus();
    } while (0);
    do {
        if ((s32)r3 != (s32)0xf3) {
            if ((s32)r3 < (s32)0xf3) {
                if ((s32)r3 != (s32)0xf1) {
                    if ((s32)r3 < (s32)0xf1) {
                        break;
                    }
                    if ((s32)r3 >= (s32)0xf5) break;
                    continue;
                    }
                r24 = 0x100;
                break;
                    }
            r24 = 0x101;
            break;
        }
        r24 = 0x102;
        break;

        r24 = 0x103;
    } while (0);
        r3 = r24;
        menuIsCheck();
        r0 = r3 & 0xFF;
        if ((s32)r3 != (s32)0xf5) {
        do {
            r4 = r29;
            r5 = r31;
            r24 = 0x100;
            r3 = 0x2;
            fightTargetGetPtr();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = 0x0;
            r5 = 0x5;
            r6 = 0x0;
            fightSideGetStatus();
            r25 = r3 & 0xFFFF;
            r3 = r29;
            r4 = r31;
            fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
            r0 = r3 & 0xFFFF;
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                break;
            }
            fightTargetDataBiosGetPtr();
            fightTargetDataBiosGetBuff();
            if ((s32)r3 < (s32)0x0) {
                r3 = 0x0;
                break;
            }
            r4 = r25;
            r6 = r3 & 0xFFFF;
            r3 = 0x0;
            r5 = 0x2;
            fightSideGetStatus();
        } while (0);
        do {
            if ((s32)r3 != (s32)0xf3) {
                if ((s32)r3 < (s32)0xf3) {
                    if ((s32)r3 != (s32)0xf1) {
                        if ((s32)r3 < (s32)0xf1) {
                            break;
                        }
                        if ((s32)r3 >= (s32)0xf5) break;
                        continue;
                        }
                    r24 = 0x100;
                    break;
                        }
                r24 = 0x101;
                break;
            }
            r24 = 0x102;
            break;

            r24 = 0x103;
        } while (0);
            r3 = r24;
            r4 = 0x0;
            r5 = 0x1;
            menuCloseCustom();
    }
    }
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r27) break;
        r3 = r29;
        r4 = r24;
        fightTrainerGetValidFightOutPokemonPtr();
        if (r3 != (u32)0x0) {
            r4 = 0x0;
            r5 = 0x120;
            r6 = 0x0;
            r7 = 0x0;
            ((void(*)(void))pokemonSetStatus)();
        }
        r24 = r24 + 0x1;

    }
    r3 = 0x1;

    return r3;
}

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

/* Address: 0x80263DE4 | Size: 0x6A4 | Ghidra import */
s32 fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(u32 r3, u32 r4, u32 r5, u32 retry, u32 reopen)
{
    extern u16 fn_801EF634(void);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightFloorIsUseFightTimerAll(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern u8 fn_801F18DC(u32);
    extern u32 fightTargetGetPtr(u32, u32, u32);
    extern u32 fightSideGetStatus(u32, u32, u32, u32);
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightTargetDataBiosGetPtr(void);
    extern s32 fightTargetDataBiosGetBuff(void);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    extern void fightMenuCloseInfoMenu(u32);
    extern void fightMenuOpenInfoMenu(s8);
    extern s32 menuFightOpenPokemon(u32, u32, u32, u32, u32);
    extern void fn_801EFFC4(u32);
    extern void menuFightStatusSetActive(u32, u32);
    extern s32 fightTrainerTimeOutSelectIrekaeFightPokemon(u32, u32, u32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u8 fightPokemonCheckValid(u32);
    extern s16 pokemonGetStatus(u32, u32, u32, u32);
    u32 ctx;
    u32 actor;
    u32 param;
    u32 choice;
    u32 msg;
    u32 found;
    u32 status;
    u32 side;
    s32 index;
    u32 item;

    ctx = r3;
    actor = r4;
    param = r5;
    while (1) {
        if ((u16)fn_801EF634() == 1) {
            return -2;
        }
        if ((fightFloorIsUseFightTimerCommand(0) == 1) && (fightTimerCommandIsOver() == 1)) {
            if ((u8)reopen == 0) {
                return fightTrainerTimeOutSelectIrekaeFightPokemon(ctx, actor, param);
            }
            if ((u8)retry != 0) {
                return -1;
            }
            continue;
        }
        if ((fn_801F18DC(0) != 0) && ((u8)reopen == 0)) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param);
            if (found == 0) {
                status = 0;
            }
            else {
                side = (u16)fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    if (index < 0) {
                        status = 0;
                    }
                    else {
                        status = fightSideGetStatus(0, side, 2, index & 0xffff);
                    }
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            menuOpenCustom(msg, 0, 0, 0, 0, 0);
        }
        if (fn_801F18DC(0) == 0) {
            fightMenuCloseInfoMenu(1);
        }
        choice = menuFightOpenPokemon(ctx, actor, param, retry, fn_801F18DC(0));
        fn_801EFFC4(0xa);
        if ((fn_801F18DC(0) == 0) && ((u8)reopen == 1)) {
            fightMenuOpenInfoMenu(1);
            if ((actor != 0) && (fn_801F18DC(0) != 1)) {
                found = fightTargetGetPtr(2, actor, param);
                if (found == 0) {
                    status = 0;
                }
                else {
                    side = (u16)fightSideGetStatus(found, 0, 5, 0);
                    if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(actor, param) == 0) {
                        status = 0;
                    }
                    else {
                        fightTargetDataBiosGetPtr();
                        index = fightTargetDataBiosGetBuff();
                        if (index < 0) {
                            status = 0;
                        }
                        else {
                            status = fightSideGetStatus(0, side, 3, index & 0xffff);
                        }
                    }
                }
                menuFightStatusSetActive(status, 1);
            }
        }
        if ((fn_801F18DC(0) != 0) && ((u8)reopen == 0)) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param);
            if (found == 0) {
                status = 0;
            }
            else {
                side = (u16)fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    if (index < 0) {
                        status = 0;
                    }
                    else {
                        status = fightSideGetStatus(0, side, 2, index & 0xffff);
                    }
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            if (menuIsCheck(msg) != 0) {
                msg = 0x100;
                found = fightTargetGetPtr(2, ctx, param);
                if (found == 0) {
                    status = 0;
                }
                else {
                    side = (u16)fightSideGetStatus(found, 0, 5, 0);
                    if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                        status = 0;
                    }
                    else {
                        fightTargetDataBiosGetPtr();
                        index = fightTargetDataBiosGetBuff();
                        if (index < 0) {
                            status = 0;
                        }
                        else {
                            status = fightSideGetStatus(0, side, 2, index & 0xffff);
                        }
                    }
                }
                switch (status) {
                case 0xf1:
                    msg = 0x100;
                    break;
                case 0xf2:
                    msg = 0x101;
                    break;
                case 0xf3:
                    msg = 0x102;
                    break;
                case 0xf4:
                    msg = 0x103;
                    break;
                }
                menuCloseCustom(msg, 0, 1);
            }
        }
        if ((u16)fn_801EF634() == 1) {
            return -2;
        }
        if ((fightFloorIsUseFightTimerCommand(0) == 1) && (fightTimerCommandIsOver() == 1) && (choice < 0)) {
            if ((u8)reopen == 0) {
                return fightTrainerTimeOutSelectIrekaeFightPokemon(ctx, actor, param);
            }
            if ((u8)retry != 0) {
                return -1;
            }
            continue;
        }
        if (choice < 0) {
            if ((u8)retry == 0) {
                continue;
            }
            return -1;
        }
        item = fightTrainerGetStatus(ctx, 0, 0x45, choice & 0xffff);
        if (fightPokemonCheckValid(item) == 0) {
            continue;
        }
        if (((u8)pokemonGetStatus(item, 0, 0xd2, 0)) == 1) {
            continue;
        }
        return pokemonGetStatus(item, 0, 0xce, 0);
    }
}

/* Address: 0x80262D34 | Size: 0x8 | Pattern: return_constant */
u32 _fightMenuFightTrainerAgbHeroSelectDefensePokemon__FP15FightOutPokemonUsUs(void) { return 0; }

/* Address: 0x80261B68 | Size: 0x84 | Ghidra import */
void fightMenuAllFightTrainerCloseStatusMenu(u32 wait)
{
    extern void fightFloorLoopValidFightTrainer();
    extern void _threadSwitch(void);
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv();
    u32 r30;
    u8 done;

    fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv, 0, 0);
    if ((u8)wait == 1) {
        r30 = 1;
        do {
            done = r30;
            fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv, &done, 0);
            if (done == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
}

/* Address: 0x80261BEC | Size: 0xD0 */
u32 _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv(u32 r3,u32 r4,u8 *r5)

{
  u32 iVar1;
  u32 uVar2;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  u8 result;
  int new_var;
  u32 found;
  u8 cVar6;

  new_var = r3;
  iVar1 = fightTargetGetPtr(2,new_var,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(new_var,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar3 = fightTargetDataBiosGetBuff();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
      }
    }
  }
  cVar6 = menuIsCheck(uVar2);
  if ((cVar6 == '\x01') && (r5 != (u8 *)0x0)) {
    *r5 = 0;
  }
  return 1;
}

/* Address: 0x80261CBC | Size: 0xD0 | Ghidra import */
u32 _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv(void* trainer, u32 slot, void* userData)
{
    u32 target;
    u16 side;
    u16 relativeTarget;
    u32 bufferIndex;
    u8 isOpen;
    u32 menuId;
    u32 context;

    context = (u32)trainer;
    target = fightTargetGetPtr(2, context, slot);
    if (target == 0) {
        menuId = 0;
    } else {
        side = fightSideGetStatus(target, 0, 5, 0);
        relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(context, slot);
        if (relativeTarget == 0) {
            menuId = 0;
        } else {
            fightTargetDataBiosGetPtr();
            bufferIndex = fightTargetDataBiosGetBuff();
            if ((int)bufferIndex < 0) {
                menuId = 0;
            } else {
                menuId = fightSideGetStatus(0, side, 2, bufferIndex & 0xffff);
            }
        }
    }
    isOpen = menuIsCheck(menuId);
    if (isOpen != 0) {
        menuCloseCustom(menuId, 0, 0);
    }
    return 1;
}

/* Address: 0x80261D8C | Size: 0xF0 | Ghidra import */
u32 _fightMenuAllFightTrainerOpenStatusMenuSub__FPvUsPv(u32 r3, u32 r4)
{
    extern int fightTrainerToMenuBallStatus();
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    u32 iVar1;
    u16 uVar3;
    u16 sVar4;
    u32 uVar2;
    u32 uVar5;
    u32 local_20[2];
    u32 local_28[2];
    int context;

    context = r3;
    iVar1 = fightTargetGetPtr(2, context, r4);
    if (iVar1 == 0) {
        uVar5 = 0;
    } else {
        uVar3 = fightSideGetStatus(iVar1, 0, 5, 0);
        sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(context, r4);
        if (sVar4 == 0) {
            uVar5 = 0;
        } else {
            fightTargetDataBiosGetPtr();
            uVar2 = fightTargetDataBiosGetBuff();
            if ((int)uVar2 < 0) {
                uVar5 = 0;
            } else {
                uVar5 = fightSideGetStatus(0, uVar3, 2, uVar2 & 0xffff);
            }
        }
    }
    fightTrainerToMenuBallStatus(context, local_28);
    local_20[0] = local_28[0];
    *(u16 *)&local_20[1] = *(u16 *)&local_28[1];
    menuOpenCustom(uVar5, 0, 0, 0, 0, 1, local_20);
  return 1;
}

/* Address: 0x80261E7C | Size: 0x7C | Ghidra import */
void fightMenuAllFightOutPokemonCloseStatusMenu(u32 wait)
{
    extern u32 fightFloorLoopValidFightOutPokemon();
    extern void _threadSwitch(void);
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv();
    fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv, 0, 0);
    if ((u8)wait == 1) {
        do {
            if ((u8)fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv, 0, 0) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
}

/* Address: 0x80261EF8 | Size: 0xBC | Ghidra import */
int _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv(u32 r3,u32 r4)

{
  u32 iVar1;
  u32 uVar2;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  u8 result;
  
  iVar1 = fightTargetGetPtr(2,r3,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar3 = fightTargetDataBiosGetBuff();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
      }
    }
  }
  uVar3 = menuIsCheck(uVar2);
  result = (uVar3 & 0xff) != 1;
  return result;
}

/* Address: 0x80261FB4 | Size: 0xD0 | Ghidra import */
u32 _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv(u32 r3,u32 r4)

{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u8 cVar5;
  u32 uVar6;
  long new_var;

  new_var = r3;
  iVar1 = fightTargetGetPtr(2,new_var,r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1,0,5,0);
    sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(new_var,r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar2 = fightTargetDataBiosGetBuff();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0,uVar3,3,uVar2 & 0xffff);
      }
    }
  }
  cVar5 = menuIsCheck(uVar6);
  if (cVar5 != '\0') {
    menuCloseCustom(uVar6,0,0);
  }
  return 1;
}

/* Address: 0x80262084 | Size: 0x140 | Ghidra import */
u32 _fightMenuAllFightOutPokemonOpenStatusMenuSub__FPvUsPv(u32 r3,u32 r4,char *r5)

{
    extern int fightOutPokemonToMenuPokemonStatus();
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 *puVar1;
  u32 uVar2;
  u8 cVar7;
  u32 iVar3;
  u16 uVar5;
  u16 sVar6;
  u32 uVar4;
  u32 *puVar8;
  u32 *puVar9;
  unsigned int context;
  u8 cVar10;
		  u32 uVar11;
		  u32 local_48[12];
		  u32 local_78[12];

	  context = r3;
	  if (r5 == (char *)0x0) {
	    cVar10 = '\x01';
	  }
	  else {
	    cVar10 = *r5;
	  }
	  cVar7 = fightOutPokemonCheckFightOut(context);
	  if (cVar7 == '\0') {
	    return 1;
	  }
	    iVar3 = fightTargetGetPtr(2,context,r4);
    if (iVar3 == 0) {
      uVar11 = 0;
    }
    else {
      uVar5 = fightSideGetStatus(iVar3,0,5,0);
		      sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(context,r4);
      if (sVar6 == 0) {
        uVar11 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar4 = fightTargetDataBiosGetBuff();
        if ((int)uVar4 < 0) {
          uVar11 = 0;
        }
        else {
          uVar11 = fightSideGetStatus(0,uVar5,3,uVar4 & 0xffff);
        }
      }
    }
		    fightOutPokemonToMenuPokemonStatus(context,local_78);
    if (cVar10 == '\0') {
      ((u8 *)local_78)[0x29] = 0;
    }
    puVar9 = local_48 - 1;
    puVar8 = local_78 - 1;
    iVar3 = 6;
    while (iVar3--) {
      puVar1 = puVar8 + 1;
      puVar8 = puVar8 + 2;
      uVar2 = *puVar1;
      puVar9[1] = uVar2;
      puVar9 = puVar9 + 2;
      *puVar9 = *puVar8;
    }
    menuOpenCustom(uVar11,0xffffffff,0,0,0,1,local_48);
  return 1;
}

/* Address: 0x802621C4 | Size: 0x30 | Ghidra import */
void fightMenuOpenLevelUpStatusMenu(u8 *dst, u8 value)
{
    extern void winMsgOpenLevelUpStatus(u8 *, u32);

    if (dst != NULL) {
        *dst = value;
        winMsgOpenLevelUpStatus(dst, 1);
    }
}

/* Address: 0x802621F4 | Size: 0x7C | Ghidra import */
void fightMenuSubMenuLvupStatus(s16 *current, s16 *previous, s16 *out)
{
    if (current == NULL) {
        return;
    }
    if (previous == NULL) {
        return;
    }
    if (out == NULL) {
        return;
    }
    out[1] = current[1] - previous[1];
    out[2] = current[2] - previous[2];
    out[3] = current[3] - previous[3];
    out[5] = current[5] - previous[5];
    out[6] = current[6] - previous[6];
    out[4] = current[4] - previous[4];
}

/* Address: 0x80262270 | Size: 0x74 | Ghidra import */
s32 fightMenuWazaWasure(u32 r3, u32 r4)
{
    extern f32 lbl_8047E6C8;
    extern s32 fn_80097A38(u32, u32);
    s32 result;

    fadeSet((double)lbl_8047E6C8, 3);
    fadeCheck(1);
    result = fn_80097A38(r3, r4);
    if (result == 4) {
        result = -1;
    }
    fadeSet((double)lbl_8047E6C8, 2);
    fadeCheck(1);
    return result;
}

/* Address: 0x802622E4 | Size: 0x24 | Ghidra import */
void fightMenuCloseLevelUpStatusMenu(void)
{
    extern void winMsgCloseLevelUpStatus(u32);

    winMsgCloseLevelUpStatus(1);
}

/* Address: 0x80262308 | Size: 0x2C | Ghidra import */
u32 fightMenuYesNo(void)
{
    extern s8 fn_8001E184(void);

    return (__cntlzw((s8)fn_8001E184()) >> 5) & 0xff;
}

/* Address: 0x80262334 | Size: 0x80 | Ghidra import */
u32 fightMenuWazaKoukaMsg(u32 msgId, u32 unused, u32 itemId)
{
    extern u32 itemGetStatus(u32, u32, u32, u32);
    extern u32 GSmsgGetGSchar(u32);
    u32 itemName;

    msgctrlSetValue(0x10);
    itemName = GSmsgGetGSchar(itemGetStatus(0, itemId, 1, 0));
    msgctrlSetValue(0x29, itemName);
    if (msgId != 0) {
        winMsgOpenFight(msgId, 1, 1);
        return 1;
    }
    return 0;
}

/* Address: 0x802623B4 | Size: 0xB8 | Ghidra import */
u32 fightMenuWazaOutMsg(u32 msgId, u32 pokemon)
{
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 GSmsgGetGSchar(u32);
    u32 name;

    msgctrlSetValue(0xf, msgId);
    name = GSmsgGetGSchar(wazaGetStatus(0, pokemon, 0xa, 0));
    msgctrlSetValue(0xd, name);
    msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, pokemon, 1, 0)));
    msgctrlSetValue(0xe, GSmsgGetGSchar(wazaGetStatus(0, pokemon, 0xb, 0)));
    winMsgOpenFight(0x768d, 1, 1);
    return 1;
}

/* Address: 0x8026246C | Size: 0x24 | Ghidra import */
void fightMenuCloseMsg(void)
{
    winMsgCloseFight(0);
}

/* Address: 0x80262490 | Size: 0x3C | Ghidra import */
u32 fightMenuOpenTrainerMsg(u32 msgId)
{
    if (msgId != 0) {
        winMsgOpenFightNoWait(msgId, 1, 1);
        return 1;
    }
    return 0;
}

/* Address: 0x802624CC | Size: 0x3C | Ghidra import */
u32 fightMenuOpenMsg(u32 msgId)
{
    if (msgId != 0) {
        winMsgOpenFight(msgId, 1, 1);
        return 1;
    }
    return 0;
}

/* Address: 0x80262508 | Size: 0x82C | Ghidra import */
u32 fightMenuFightTrainerAgbHeroOpenMenu(u32 r3, u32 r4)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u8 fn_801F18DC(u32);
    extern u32 fightTargetGetPtr(u32, u32, u32);
    extern u16 fightSideGetStatus(u32, u32, u32, u32);
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightTargetDataBiosGetPtr(void);
    extern s32 fightTargetDataBiosGetBuff(void);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u16 fightFloorGetStatus(u32, u32, u32, u32);
    extern void fightTypeDataBiosGetPtr(u32);
    extern u32 fightTypeDataBiosGetFightoutPokemonNum(void);
    extern u32 fightTrainerGetValidFightOutPokemonPtr(u32, u32);
    extern u8 fightOutPokemonCheckFightActionSelect(u32, u32);
    extern void fightOutPokemonInitFightActionBuff(u32);
    extern u16 fn_801EF634(void);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern u32 menuFightOpenGBAMain(u32, u32, u32, u32);
    extern s32 fn_80089F70(u32);
    extern s8 fn_80089F68(u32);
    extern u16 fn_80089F58(u32);
    extern u16 fn_80089F60(u32);
    extern u8 fightOutPokemonCheckFightActionWazaSelect(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s16 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 lbl_80478DF8;
    extern u32 fn_8022B2CC(u32, u32, u32, u32, u32, u32, s32);
    extern u32 fightFloorGetValidFightSidePtr(u32, u32);
    extern u32 fightSideGetValidFightTrainerPtr(u32, u32);
    extern void fightOutPokemonCreateFightActionAttackWaza(u32, u32, u32, u32, void *, u32, u32, s32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void *, s32);
    extern void fightTrainerTimeOutSelectFightAction(u32, u32, u32);
    extern void fightTrainerAllInitFightActionBuff(u32);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    extern char lbl_80375D30[];
    extern char lbl_80375CA8[];
    u32 ctx;
    u32 param;
    u32 battle;
    u32 found;
    u32 status;
    u32 side;
    s32 index;
    u32 msg;
    u16 optionCount;
    u32 count;
    u32 lastGood;
    u32 i;
    u32 slot;
    u32 entry;
    s32 kind;
    s32 moveSlot;
    u32 moveId;
    u32 selected;
    u32 targetIndex;
    u32 sideIndex;
    u32 optionIndex;
    u32 scanIndex;
    u32 sideObj;
    u32 optionObj;
    u32 done;

    ctx = r3;
    param = r4;
    battle = fightTrainerGetStatus(ctx, 0, 0x4b, 0);
    if (fn_801F18DC(0) != 0) {
        msg = 0x100;
        found = fightTargetGetPtr(2, ctx, param);
        if (found == 0) {
            status = 0;
        }
        else {
            side = fightSideGetStatus(found, 0, 5, 0);
            if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                status = 0;
            }
            else {
                fightTargetDataBiosGetPtr();
                index = fightTargetDataBiosGetBuff();
                if (index < 0) {
                    status = 0;
                }
                else {
                    status = fightSideGetStatus(0, side, 2, index & 0xffff);
                }
            }
        }
        switch (status) {
        case 0xf1:
            msg = 0x100;
            break;
        case 0xf2:
            msg = 0x101;
            break;
        case 0xf3:
            msg = 0x102;
            break;
        case 0xf4:
            msg = 0x103;
            break;
        }
        menuOpenCustom(msg, 0, 0, 0, 0, 0);
    }
    optionCount = fightFloorGetStatus(0, 0, 0x16, 0);
    fightTypeDataBiosGetPtr(param);
    count = fightTypeDataBiosGetFightoutPokemonNum() & 0xff;
    lastGood = 0;
    i = 0;
    while ((u32)(u16)i < count) {
        slot = fightTrainerGetValidFightOutPokemonPtr(ctx, i);
        if (slot == 0) {
            lastGood = i;
            i++;
            continue;
        }
        if (fightOutPokemonCheckFightActionSelect(slot, 1) == 0) {
            lastGood = i;
            i++;
            continue;
        }
        while (1) {
            fightOutPokemonInitFightActionBuff(slot);
            if ((u16)fn_801EF634() == 1) {
                fightTrainerAllInitFightActionBuff(ctx);
                if (fn_801F18DC(0) != 0) {
                    msg = 0x100;
                    found = fightTargetGetPtr(2, ctx, param);
                    if (found == 0) {
                        status = 0;
                    }
                    else {
                        side = fightSideGetStatus(found, 0, 5, 0);
                        if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                            status = 0;
                        }
                        else {
                            fightTargetDataBiosGetPtr();
                            index = fightTargetDataBiosGetBuff();
                            status = (index < 0) ? 0 : fightSideGetStatus(0, side, 2, index & 0xffff);
                        }
                    }
                    switch (status) {
                    case 0xf1:
                        msg = 0x100;
                        break;
                    case 0xf2:
                        msg = 0x101;
                        break;
                    case 0xf3:
                        msg = 0x102;
                        break;
                    case 0xf4:
                        msg = 0x103;
                        break;
                    }
                    if (menuIsCheck(msg) != 0) {
                        menuCloseCustom(msg, 0, 1);
                    }
                }
                return 0;
            }
            if ((fightFloorIsUseFightTimerCommand(0) == 1) && (fightTimerCommandIsOver() == 1)) {
                fightTrainerTimeOutSelectFightAction(ctx, slot, param);
                lastGood = i;
                break;
            }
            entry = menuFightOpenGBAMain(battle, ctx, (u16)i, param);
            kind = fn_80089F70(entry);
            if ((u16)fn_801EF634() == 1) {
                fightTrainerAllInitFightActionBuff(ctx);
                return 0;
            }
            if (kind == 3) {
                fightOutPokemonCreateFightAction(slot, 0, 8, 0, lbl_80375D30, 0);
                lastGood = i;
                break;
            }
            if (kind == 1) {
                if (fightOutPokemonCheckFightActionWazaSelect(slot, 1) != 0) {
                    lastGood = i;
                    break;
                }
                moveSlot = fn_80089F68(entry);
                if (moveSlot < 0) {
                    continue;
                }
                moveId = pokemonGetStatus(fightOutPokemonGetPokemonPtr(slot), 0, 0x7f, moveSlot);
                moveId &= 0xffff;
                if ((moveId == 0) || (moveId >= lbl_80478DF8) || (moveId == 0x165)) {
                    continue;
                }
                selected = fn_8022B2CC(slot, moveId, param, (u32)_fightMenuFightTrainerAgbHeroSelectDefensePokemon__FP15FightOutPokemonUsUs, 1, 0, -1);
                if (selected == 0) {
                    targetIndex = fn_80089F60(entry);
                    scanIndex = 0;
                    done = 0;
                    sideIndex = 0;
                    while ((sideIndex & 0xffff) < 2) {
                        sideObj = fightFloorGetValidFightSidePtr(0, sideIndex);
                        if (sideObj != 0) {
                            optionIndex = 0;
                            while ((optionIndex & 0xffff) < optionCount) {
                                optionObj = fightSideGetValidFightTrainerPtr(sideObj, optionIndex);
                                if (optionObj != 0) {
                                    selected = fightTrainerGetStatus(optionObj, 0, 0x46, scanIndex);
                                    if ((u16)scanIndex == targetIndex) {
                                        done = 1;
                                        break;
                                    }
                                    scanIndex++;
                                }
                                optionIndex++;
                            }
                            if (done == 1) {
                                break;
                            }
                        }
                        sideIndex++;
                    }
                }
                if (selected == 0) {
                    continue;
                }
                fightOutPokemonCreateFightActionAttackWaza(slot, 0, 0x13, 0, lbl_80375CA8, moveId,
                            fightTargetGetTragetPtrToRelativeHostSideFightTargetId(selected, param), moveSlot, 0);
                lastGood = i;
                break;
            }
            if (kind == 2) {
                moveId = fightTrainerGetStatus(ctx, 0, 0x45, fn_80089F58(entry));
                index = pokemonGetStatus(moveId, 0, 0xce, 0);
                if (index < 0) {
                    continue;
                }
                fightOutPokemonCreateFightAction(slot, 0, 9, 0, lbl_80375D30, index);
                lastGood = i;
                break;
            }
            if (kind == 4) {
                fightTrainerTimeOutSelectFightAction(ctx, slot, param);
                lastGood = i;
                break;
            }
            if (kind == 5) {
                fightTrainerAllInitFightActionBuff(ctx);
                return 0;
            }
            if (kind != 0) {
                fightTrainerTimeOutSelectFightAction(ctx, slot, param);
                lastGood = i;
                break;
            }
            if ((u16)i == 0) {
                break;
            }
            i = lastGood;
        }
        i++;
    }
    if (fn_801F18DC(0) != 0) {
        msg = 0x100;
        found = fightTargetGetPtr(2, ctx, param);
        if (found == 0) {
            status = 0;
        }
        else {
            side = fightSideGetStatus(found, 0, 5, 0);
            if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                status = 0;
            }
            else {
                fightTargetDataBiosGetPtr();
                index = fightTargetDataBiosGetBuff();
                status = (index < 0) ? 0 : fightSideGetStatus(0, side, 2, index & 0xffff);
            }
        }
        switch (status) {
        case 0xf1:
            msg = 0x100;
            break;
        case 0xf2:
            msg = 0x101;
            break;
        case 0xf3:
            msg = 0x102;
            break;
        case 0xf4:
            msg = 0x103;
            break;
        }
        if (menuIsCheck(msg) != 0) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param);
            if (found == 0) {
                status = 0;
            }
            else {
                side = fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    status = (index < 0) ? 0 : fightSideGetStatus(0, side, 2, index & 0xffff);
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            menuCloseCustom(msg, 0, 1);
        }
    }
    return 1;
}

/* Address: 0x80262D3C | Size: 0x430 | Ghidra import */
s32 fightMenuFightTrainerAgbHeroSelectIrekaeFightPokemon(u32 r3, u32 r4, u32 r5, s32 r6)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightTypeDataBiosGetPtr(u32);
    extern u32 fightTypeDataBiosGetFightoutPokemonNum(void);
    extern u16 fn_801EF634(void);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern u8 fn_801F18DC(u32);
    extern u32 fightTargetGetPtr(u32, u32, u32);
    extern u32 fightSideGetStatus(u32, u32, u32, u32);
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightTargetDataBiosGetPtr(void);
    extern s32 fightTargetDataBiosGetBuff(void);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u32 menuFightOpenGBAIrekae(u32, u32, u32, u32);
    extern s32 fn_80089F70(u32);
    extern u16 fn_80089F58(u32);
    extern s16 pokemonGetStatus(u32, u32, u32, u32);
    extern s32 fightTrainerTimeOutSelectIrekaeFightPokemon(u32, s32, u32);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    u32 ctx;
    u32 param1;
    u32 param2;
    s32 target;
    u32 battle;
    u32 count;
    u32 i;
    u32 found;
    u32 status;
    u16 side;
    s32 index;
    u32 msg;
    u32 entry;
    s32 kind;

    ctx = r3;
    param1 = r4;
    param2 = r5;
    target = r6;
    battle = fightTrainerGetStatus(ctx, 0, 0x4b, 0);
    fightTypeDataBiosGetPtr(param1);
    count = fightTypeDataBiosGetFightoutPokemonNum() & 0xff;
    i = 0;
    while ((u32)(u16)i < count) {
        found = fightTrainerGetStatus(ctx, 0, 0x46, i);
        if (found == (u32)target) {
            break;
        }
        i++;
    }
    if ((u32)(u16)i >= count) {
        return fightTrainerAiSelectIrekaeDasuFightPokemon((void *)ctx, param1, param2, target);
    }

    if ((u16)fn_801EF634() == 1) {
        goto set_cancel;
    }
    if ((fightFloorIsUseFightTimerCommand(0) == 1) && (fightTimerCommandIsOver() == 1)) {
        goto set_select;
    }
    {
        if (fn_801F18DC(0) != 0) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param1);
            if (found == 0) {
                status = 0;
            }
            else {
                side = (u16)fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param1) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    if (index < 0) {
                        status = 0;
                    }
                    else {
                        status = fightSideGetStatus(0, side, 2, index & 0xffff);
                    }
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            menuOpenCustom(msg, 0, 0, 0, 0, 0);
        }
        entry = menuFightOpenGBAIrekae(battle, ctx, (u16)i, param1);
        kind = fn_80089F70(entry);
        if ((u16)fn_801EF634() == 1) {
            goto set_cancel;
        }
        if (kind == 2) {
            found = fightTrainerGetStatus(ctx, 0, 0x45, fn_80089F58(entry));
            target = pokemonGetStatus(found, 0, 0xce, 0);
            goto after_select;
        }
	        if (kind == 4) {
	set_select:
	            target = fightTrainerTimeOutSelectIrekaeFightPokemon(ctx, target, param1);
	            goto after_select;
	        }
	        if (kind != 5) {
	            goto set_select;
	        }
	    }
set_cancel:
    target = -2;
after_select:

    if (fn_801F18DC(0) != 0) {
        msg = 0x100;
        found = fightTargetGetPtr(2, ctx, param1);
        if (found == 0) {
            status = 0;
        }
        else {
            side = (u16)fightSideGetStatus(found, 0, 5, 0);
            if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param1) == 0) {
                status = 0;
            }
            else {
                fightTargetDataBiosGetPtr();
                index = fightTargetDataBiosGetBuff();
                if (index < 0) {
                    status = 0;
                }
                else {
                    status = fightSideGetStatus(0, side, 2, index & 0xffff);
                }
            }
        }
        switch (status) {
        case 0xf1:
            msg = 0x100;
            break;
        case 0xf2:
            msg = 0x101;
            break;
        case 0xf3:
            msg = 0x102;
            break;
        case 0xf4:
            msg = 0x103;
            break;
        }
        if (menuIsCheck(msg) != 0) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param1);
            if (found == 0) {
                status = 0;
            }
            else {
                side = (u16)fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param1) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    if (index < 0) {
                        status = 0;
                    }
                    else {
                        status = fightSideGetStatus(0, side, 2, index & 0xffff);
                    }
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            menuCloseCustom(msg, 0, 1);
        }
    }
    return target;
}

/* Address: 0x80264ADC | Size: 0x27C | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubBallSelectTargetPokemon__FP15FightOutPokemonUsUs(u32 r3,u32 r4,u32 r5)

{
    extern int menuFightCloseTarget();
    extern int menuFightOpenTarget();
    extern int winMsgCloseFight();
    extern int winMsgOpenFight();
    extern int fn_801906A0();
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
        uVar3 = fn_801906A0(0x99f);
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

#endif

#if defined(FIGHT_MENU_EXACT_8026503C)

/* Address: 0x8026503C | Size: 0x2F0 | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs(u32 r3,u32 r4,u32 r5)

{
    extern u8 lbl_8047B678;
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
  
LAB_00262054:
  do {
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
    {
      u32 uVar1c;
      u16 uVar4c;

      uVar1c = fightTargetGetPtr(0xe,r3,r5);
      cVar6 = fightOutPokemonCheckFightOut();
      if (cVar6 == '\x01') {
        found = fightTargetGetPtr(2,uVar1c,r5);
        if (found == 0) {
          uVar3 = 0;
        }
        else {
          uVar4c = fightSideGetStatus(found,0,5,0);
          sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1c,r5);
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
              uVar3 = fightSideGetStatus(0,uVar4c,3,uVar3 & 0xffff);
            }
          }
        }
      }
      else {
        uVar3 = 0;
      }
    }
    *(u32 *)(auStack_38 + 0x14) = uVar3;
    *(u32 *)(auStack_38 + 0x1c) = 0;
    auStack_38[0x20] = 3;
    auStack_38[0x21] = fn_801F18DC(0);
    iVar2 = menuFightOpenTarget((int*)auStack_38,0,1);
    lbl_8047B678 = 1;
    if (iVar2 < 0) {
      menuFightCloseTarget(1);
      return 0;
    }
    if (iVar2 == 0) {
      uVar1 = fightTargetGetPtr(0xf,r3,r5);
    }
    else if (iVar2 == 1) {
      uVar1 = fightTargetGetPtr(0x10,r3,r5);
    }
    else {
      if (iVar2 != 2) goto LAB_00262054;
      uVar1 = fightTargetGetPtr(0xe,r3,r5);
    }
    cVar6 = fightOutPokemonCheckFightOut(uVar1);
    if (cVar6 != '\0') {
      menuFightCloseTarget(1);
      return uVar1;
    }
  } while (1);
}

/* Address: 0x8026532C | Size: 0xD0 | Ghidra import */
void fn_8026532C(u32 r3, u32 r4, u32 r5)
{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u32 uVar6;

  iVar1 = fightTargetGetPtr(2, r3, r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1, 0, 5, 0);
    sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3, r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar2 = fightTargetDataBiosGetBuff();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0, uVar3, 3, uVar2 & 0xffff);
      }
    }
  }
  if ((u8)menuIsCheck(uVar6) != 0) {
    menuCloseCustom(uVar6, 0, r5);
  }
}

/* Address: 0x802653FC | Size: 0x19C | Ghidra import */
void fightMenuFightOutPokemonRenewStatusMenu(u32 r3, u32 r4, u32 r5)
{
  typedef struct StatusMenuCopy {
    u32 word[12];
  } StatusMenuCopy;
  typedef struct StatusMenuOut {
    StatusMenuCopy copy;
    u32 pad[2];
  } StatusMenuOut;
  extern int fightOutPokemonToMenuPokemonStatus();
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 r28;
  u32 r29;
  u32 r30;
  u32 iVar3;
  u16 uVar5;
  u16 sVar6;
  u32 uVar4;
  u32 checkStatus;
  u32 uVar11;
  StatusMenuOut local_48;
  StatusMenuCopy local_78;

  r28 = r3;
  r29 = r4;
  r30 = r5;
  iVar3 = fightTargetGetPtr(2, r28, r29);
  if (iVar3 == 0) {
    checkStatus = 0;
  }
  else {
    uVar5 = fightSideGetStatus(iVar3, 0, 5, 0);
    sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r28, r29);
    if (sVar6 == 0) {
      checkStatus = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar4 = fightTargetDataBiosGetBuff();
      if ((int)uVar4 < 0) {
        checkStatus = 0;
      }
      else {
        checkStatus = fightSideGetStatus(0, uVar5, 3, uVar4 & 0xffff);
      }
    }
  }
  if ((u8)menuIsCheck(checkStatus) != 0) {
    iVar3 = fightTargetGetPtr(2, r28, r29);
    if (iVar3 == 0) {
      uVar11 = 0;
    }
    else {
      uVar5 = fightSideGetStatus(iVar3, 0, 5, 0);
      sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r28, r29);
      if (sVar6 == 0) {
        uVar11 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar4 = fightTargetDataBiosGetBuff();
        if ((int)uVar4 < 0) {
          uVar11 = 0;
        }
        else {
          uVar11 = fightSideGetStatus(0, uVar5, 3, uVar4 & 0xffff);
        }
      }
    }
    fightOutPokemonToMenuPokemonStatus(r28, &local_78);
    if ((u8)r30 == 0) {
      *(u8 *)((u8 *)&local_78 + 0x29) = 0;
    }
    local_48.copy = local_78;
    menuOpenCustom(uVar11, -1, 0, 0, 0, 1, &local_48.copy);
  }
}

/* Address: 0x80265598 | Size: 0x114 | Ghidra import */
void fn_80265598(u32 r3, u32 r4, u32 r5)
{
  typedef struct StatusMenuCopy {
    u32 word[12];
  } StatusMenuCopy;
  typedef struct StatusMenuOut {
    StatusMenuCopy copy;
    u32 pad[2];
  } StatusMenuOut;
  extern int fightOutPokemonToMenuPokemonStatus();
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 r28;
  u32 r29;
  u32 r30;
  u32 iVar3;
  u16 uVar5;
  u16 sVar6;
  u32 uVar4;
  u32 uVar11;
  StatusMenuOut local_48;
  StatusMenuCopy local_78;

  r28 = r3;
  r29 = r4;
  r30 = r5;
  iVar3 = fightTargetGetPtr(2, r28, r29);
  if (iVar3 == 0) {
    uVar11 = 0;
  }
  else {
    uVar5 = fightSideGetStatus(iVar3, 0, 5, 0);
    sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r28, r29);
    if (sVar6 == 0) {
      uVar11 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar4 = fightTargetDataBiosGetBuff();
      if ((int)uVar4 < 0) {
        uVar11 = 0;
      }
      else {
        uVar11 = fightSideGetStatus(0, uVar5, 3, uVar4 & 0xffff);
      }
    }
  }
  fightOutPokemonToMenuPokemonStatus(r28, &local_78);
  if ((u8)r30 == 0) {
    *(u8 *)((u8 *)&local_78 + 0x29) = 0;
  }
  local_48.copy = local_78;
  menuOpenCustom(uVar11, -1, 0, 0, 0, 1, &local_48.copy);
}

/* Address: 0x802656AC | Size: 0xA8 | Ghidra import */
u32 fightMenuGetFightOutPokemonPtrToStatusMenuId(u32 r3, u32 r4)
{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u32 uVar6;

  iVar1 = fightTargetGetPtr(2, r3, r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1, 0, 5, 0);
    sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3, r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar2 = fightTargetDataBiosGetBuff();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0, uVar3, 3, uVar2 & 0xffff);
      }
    }
  }
  return uVar6;
}

/* Address: 0x80265754 | Size: 0x174 | Ghidra import */
void fightMenuFightTrainerRenewStatusMenu(u32 r3,u32 r4)

{
  typedef struct BattleStatusPair {
      u32 unk0;
      u16 unk4;
  } BattleStatusPair;
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 iVar1;
  u32 uVar2;
  u8 cVar6;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  BattleStatusPair local_20;
  BattleStatusPair local_28;
  
  iVar1 = fightTargetGetPtr(2,r3,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar3 = fightTargetDataBiosGetBuff();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
      }
    }
  }
  cVar6 = menuIsCheck(uVar2);
  if (cVar6 != '\0') {
    iVar1 = fightTargetGetPtr(2,r3,r4);
    if (iVar1 == 0) {
      uVar2 = 0;
    }
    else {
      uVar4 = fightSideGetStatus(iVar1,0,5,0);
      sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
      if (sVar5 == 0) {
        uVar2 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar3 = fightTargetDataBiosGetBuff();
        if ((int)uVar3 < 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
        }
      }
    }
    fightTrainerToMenuBallStatus(r3,&local_28);
    local_20.unk0 = local_28.unk0;
    local_20.unk4 = local_28.unk4;
    menuOpenCustom(uVar2,0,0,0,0,1,&local_20);
  }
  return;
}

#endif

#if defined(FIGHT_MENU_CANDIDATE_80261B68)

/* Address: 0x80264D58 | Size: 0x2E4 | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32 r3,u32 r4,u32 r5)

{
    extern int menuFightCloseWaza();
    extern u32 menuFightOpenWaza();
    extern int fightFloorSetStatus();
    extern int fightOutPokemonToMenuWazaStatus();
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme();
    extern u16 fightOutPokemonGetSoubiItemDataId();
    extern int fightOutPokemonCreateFightActionAttackWaza();
    extern int fn_8022B2CC();
    extern u32 _fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs();
    extern char lbl_80375CA8[];
    extern u32 *lbl_80478DF8;
    extern u8 lbl_8047B678;
    u32 saved_r27;
  u8 bVar8;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar9;
  u16 uVar7;
  u32 uVar4;
  u32 iVar5;
  u32 iVar6;

  u16 local_70 [2];
  u8 auStack_6c [69];
  
  fightTypeDataBiosGetPtr(r5);
  bVar8 = fightTypeDataBiosGetFightoutPokemonNum();
  fightOutPokemonToMenuWazaStatus(r4,auStack_6c);
  auStack_6c[68] = fn_801F18DC(0);
  uVar1 = fightOutPokemonGetPokemonPtr(r4);
  do {
    do {
      while (1) {
        uVar2 = (int)pokemonGetStatus(r4,0,0x101,0);
        uVar3 = menuFightOpenWaza((int*)auStack_6c,uVar2,1);
        if ((int)uVar3 < 0) {
          menuFightCloseWaza(1);
          return 0;
        }
        cVar9 = fightOutPokemonCheckCanOutOkWazaBanme(r4,(u16)uVar3,1,local_70);
        uVar7 = (int)pokemonGetStatus(uVar1,0,0x7f,uVar3 & 0xffff);
        if (cVar9 != '\0') {
          msgctrlSetValue(0x11,r4);
          wazaGetStatus(0,uVar7,1,0);
          uVar2 = GSmsgGetGSchar();
          msgctrlSetValue(0x28,uVar2);
          uVar7 = fightOutPokemonGetSoubiItemDataId(r4);
          fightFloorSetStatus(0,0,0x56,0,uVar7);
        }
        if (cVar9 == '\x06') {
          saved_r27 = 0x7661;
        }
        else if (cVar9 == '\x05') {
          wazaGetStatus(0,local_70[0],1,0);
          uVar2 = GSmsgGetGSchar();
          msgctrlSetValue(0x28,uVar2);
          saved_r27 = 0x76bb;
        }
        else if (cVar9 == '\x04') {
          saved_r27 = 0x7600;
        }
        else if (cVar9 == '\x03') {
          saved_r27 = 0x75ff;
        }
        else if (cVar9 == '\x02') {
          saved_r27 = 0x75fe;
        }
        else if (cVar9 == '\x01') {
          saved_r27 = 0x75fd;
        }
        if (cVar9 == '\0') break;
        if (saved_r27 != 0) {
          winMsgOpenFight(saved_r27,1,1);
        }
        fightMainWaitFrame(0x40);
        winMsgCloseFight(0);
      }
      uVar4 = (int)pokemonGetStatus(uVar1,0,0x7f,(u16)uVar3);
      uVar4 = uVar4 & 0xffff;
    } while (((uVar4 == 0) || (uVar4 >= *lbl_80478DF8)) || (uVar4 == 0x165));
    lbl_8047B678 = 0;
    iVar5 = fn_8022B2CC(r4,uVar4,r5,_fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs,1,0,0xffffffff);
    cVar9 = fn_801F18DC(0);
  } while ((((cVar9 == '\x01') && (lbl_8047B678 == '\0')) &&
           ((bVar8 >= 2 && (iVar6 = _fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs(r4,uVar4,r5), iVar6 == 0)))) ||
          (iVar5 == 0));
  uVar1 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(iVar5,r5);
  menuFightCloseWaza(1);
  fightOutPokemonCreateFightActionAttackWaza(r4,0,0x13,0,lbl_80375CA8,uVar4,uVar1,(int)(s8)uVar3,0);
  return 1;
}

/* Address: 0x80264488 | Size: 0x654 | Ghidra import (PSQ removed) */


u32 _fightMenuFightTrainerGcHeroOpenMenuSubItem__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32 r3,int r4,u16 r5)

{
    extern s8 menuFightCloseCheckTotalTimer();
    extern int menuFightCloseTotalTimer();
    extern int menuFightOpenTotalTimer();
    extern s8 menuFightCloseCheckCountDown();
    extern int menuFightCloseCountDown();
    extern int menuFightOpenCountDown();
    extern u32 fn_80018F88();
    extern u8 fn_80019064();
    extern u32 fn_800D37CC();
    extern int menuReleaseOffScreen();
    extern int menuCreateOffScreen();
    extern s8 fn_80142CF4();
    extern int fn_801DA36C();
    extern int fn_801EFFC4();
    extern s8 fightFloorIsUseFightTimerAll();
    extern s8 fightTrainerIsSelectedItemBall();
    extern u16 fightTrainerGetFightOutPokemonToTemotiBanme();
    extern short fightTrainerGetFightPokemonToTemotiBanme();
    extern int fn_8022FF90();
    extern f32 lbl_8047E6CC;
    extern f64 lbl_8047E6D0;

  int iVar1;
  u32 uVar2;
  short sVar4;
  s8 cVar6;
  u8 uVar7;
  int iVar3;
  u16 uVar5;
  u16 uVar8;
  u32 uVar9;
  u32 uVar10;

  u64 f30;
  double dVar11;
  u64 f31;
  double dVar12;
  u8 local_80;
  char local_7f [3];
  char local_7c [4];
  char local_78 [4];
  char local_74 [4];
  u32 local_70 [2];
  u32 local_68;
  u32 uStack_64;
  u8 auStack_18 [16];
  u8 auStack_8 [8];

  local_70[0] = 0;
  for (uVar8 = 0; uVar8 < 2; uVar8 = uVar8 + 1) {
    local_74[uVar8] = '\0';
    local_78[uVar8] = '\0';
    local_7c[uVar8] = '\0';
  }
  for (uVar9 = 0; (uVar9 & 0xffff) < 2; uVar9 = uVar9 + 1) {
    iVar1 = fightTrainerGetStatus(r3,0,0x46,uVar9);
    if (iVar1 != 0) {
      uVar5 = fightTrainerGetFightOutPokemonToTemotiBanme(r3,iVar1);
      fightTrainerGetStatus(r3,0,0x45,uVar5);
      cVar6 = fightPokemonCheckFightOut();
      uVar10 = uVar9 & 0xffff;
      local_74[uVar10] = cVar6;
      cVar6 = fn_802026E4(iVar1,8);
      local_78[uVar10] = cVar6;
      cVar6 = fn_802026E4(iVar1,7);
      local_7c[uVar10] = cVar6;
    }
  }
  iVar1 = fightTrainerGetStatus(r3,0,0x44,0);
  if (iVar1 != 0) {
    uVar2 = (int)pokemonGetStatus(r4,0,0xd6,0);
    sVar4 = fightTrainerGetFightPokemonToTemotiBanme(r3,uVar2);
    local_70[0] = (u32)sVar4;
    dVar11 = lbl_8047E6D0;
    dVar12 = (double)lbl_8047E6CC;
LAB_0026160c:
    fightFloorLoopValidFightTrainer(0,0x80261cbc,0,0);
    fightFloorLoopValidFightOutPokemon(0,0x80261fb4,0,0);
    cVar6 = fightFloorIsUseFightTimerCommand(0);
    if (cVar6 == '\x01') {
      menuFightCloseCountDown();
    }
    cVar6 = fightFloorIsUseFightTimerAll(0);
    if (cVar6 == '\x01') {
      menuFightCloseTotalTimer();
    }
    fightFloorLoopValidFightTrainer(0,0x80261cbc,0,0);
    while (1) {
      local_7f[0] = '\x01';
      fightFloorLoopValidFightTrainer(0,0x80261bec,local_7f,0);
      if (local_7f[0] == '\x01') break;
      _threadSwitch();
    }
    fightFloorLoopValidFightOutPokemon(0,0x80261fb4,0,0);
    while (cVar6 = fightFloorLoopValidFightOutPokemon(0,0x80261ef8,0,0), cVar6 != '\x01') {
      _threadSwitch();
    }
    cVar6 = fightFloorIsUseFightTimerCommand(0);
    if (cVar6 == '\x01') {
      while (cVar6 = menuFightCloseCheckCountDown(), cVar6 != '\0') {
        _threadSwitch();
      }
    }
    cVar6 = fightFloorIsUseFightTimerAll(0);
    if (cVar6 == '\x01') {
      while (cVar6 = menuFightCloseCheckTotalTimer(), cVar6 != '\0') {
        _threadSwitch();
      }
    }
    uStack_64 = fn_800D37CC();
    uStack_64 = uStack_64 ^ 0x80000000;
    local_68 = 0x43300000;
    menuCreateOffScreen((double)(float)(dVar12 / (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uStack_64)) -
                                                         dVar11)));
    uVar9 = fn_80018F88(1,local_70,r3);
    fn_801EFFC4(10);
    uVar7 = fn_80019064();
    if (((uVar9 & 0xffff) != 0) && (cVar6 = itemGetStatus(0,uVar9,2,0), cVar6 == '\x02')) {
      for (uVar10 = 0; (uVar10 & 0xffff) < 2; uVar10 = uVar10 + 1) {
        iVar1 = fightTrainerGetStatus(r3,0,0x46,uVar10);
        if ((iVar1 != 0) && (iVar3 = (int)pokemonGetStatus(iVar1,0,0xee,0), iVar3 != 0)) {
          if ((local_78[uVar10 & 0xffff] == '\x01') &&
             (cVar6 = fn_802026E4(iVar1,8), cVar6 == '\0')) {
            fn_801DA36C(iVar3,1);
          }
          if ((local_7c[uVar10 & 0xffff] == '\x01') &&
             (cVar6 = fn_802026E4(iVar1,7), cVar6 == '\0')) {
            fn_801DA36C(iVar3,2);
          }
        }
      }
    }
    fightFloorLoopValidFightTrainer(0,0x80261d8c,0,0);
    local_80 = 1;
    fightFloorLoopValidFightOutPokemon(0,0x80262084,&local_80,0);
    cVar6 = fightFloorIsUseFightTimerCommand(0);
    if (cVar6 == '\x01') {
      menuFightOpenCountDown();
    }
    cVar6 = fightFloorIsUseFightTimerAll(0);
    if (cVar6 == '\x01') {
      menuFightOpenTotalTimer();
    }
    if ((r4 != 0) && (cVar6 = fn_801F18DC(0), cVar6 != '\x01')) {
      iVar1 = fightTargetGetPtr(2,r4,r5);
      if (iVar1 == 0) {
        uVar2 = 0;
      }
      else {
        uVar5 = fightSideGetStatus(iVar1,0,5,0);
        sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r4,r5);
        if (sVar4 == 0) {
          uVar2 = 0;
        }
        else {
          fightTargetDataBiosGetPtr();
          uVar10 = fightTargetDataBiosGetBuff();
          if ((int)uVar10 < 0) {
            uVar2 = 0;
          }
          else {
            uVar2 = fightSideGetStatus(0,uVar5,3,uVar10 & 0xffff);
          }
        }
      }
      menuFightStatusSetActive(uVar2,1);
    }
    uStack_64 = fn_800D37CC();
    uStack_64 = uStack_64 ^ 0x80000000;
    local_68 = 0x43300000;
    menuReleaseOffScreen((double)(float)(dVar12 / (double)(float)((double)(((u64)(0x43300000) << 32) | (u32)(uStack_64)) -
                                                         dVar11)));
    fn_801EFFC4(10);
    if ((uVar9 & 0xffff) != 0) {
      cVar6 = itemGetStatus(0,uVar9,2,0);
      if (cVar6 == '\x01') goto code_r0x002619ac;
      cVar6 = itemGetStatus(0,uVar9,2,0);
      if (cVar6 == '\x02') {
        fn_8022FF90();
        for (uVar10 = 0; (uVar10 & 0xffff) < 2; uVar10 = uVar10 + 1) {
          iVar1 = fightTrainerGetStatus(r3,0,0x46,uVar10);
          if (((iVar1 != 0) && (local_74[uVar10 & 0xffff] == '\0')) &&
             (cVar6 = fightOutPokemonCheckFightOut(), cVar6 == '\x01')) {
            pokemonSetStatus(iVar1,0,0x120,0,1);
          }
        }
      }
      uVar2 = 1;
      uVar10 = local_70[0] & 0xffff;
      goto LAB_00261a8c;
    }
    uVar2 = 0;
    goto LAB_00261ab8;
  }
  uVar2 = 0;
LAB_00261ab8:

  return uVar2;
code_r0x002619ac:
  cVar6 = fightTrainerIsSelectedItemBall(r3);
  if ((cVar6 == '\x01') || (iVar1 = _fightMenuFightTrainerGcHeroOpenMenuSubBallSelectTargetPokemon__FP15FightOutPokemonUsUs(r4,uVar9,r5), iVar1 == 0))
  goto LAB_0026160c;
  uVar10 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(iVar1,r5);
  uVar2 = 0;
LAB_00261a8c:
  fightOutPokemonCreateFightActionUseItem(r4,0,0x12,0,0x80375d70,uVar9 & 0xffff,uVar10,uVar7,uVar2);
  uVar2 = 1;
  goto LAB_00261ab8;
}

#endif
