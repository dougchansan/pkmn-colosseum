/**
 * @file fight_trainer_ai_irekae.c
 * @brief game/pxdvs/app/fight/fightTrainerAiIrekae.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8024E578-0x80250980, 6 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTrainerAiIrekae.cpp
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
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

/* =========================================================================
 * fightTrainerAiSelectIrekaeDasuFightPokemon - PostBattleProcessing
 *
 * Large post-battle handler (0x1224 = 4644 bytes).
 *
 * Processes the results of a completed battle:
 *   - Experience calculation and distribution
 *   - Shadow Pokemon purification progress
 *   - Prize money / Poke Coupon rewards
 *   - Story flag updates
 *   - Team state restoration (PP, status)
 * ========================================================================= */
/* TODO: Decompile fightTrainerAiSelectIrekaeDasuFightPokemon (4644 bytes) */
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A434[];
    extern u32 lbl_80478B38;
    extern void fn_8000815C();
    extern void fn_800E0C54();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightFloorGetFightPokemonPtrToFightTrainerPtr();
    extern void fn_801F87CC();
    extern void fightTrainerCheckCanIrekaeFightPokemon();
    extern void fightTrainerGetStatus();
    extern void fn_801FCEC4();
    extern void fightOutPokemonIsFightActionAttackWazaOut();
    extern void fightPokemonGetPokemonPtr();
    extern void fightOutPokemonCheckFightOut();
    extern void fightOutPokemonCreate();
    extern void fn_80235B04();
    extern void fn_802367CC();
    extern void fn_802369B8();
    extern void fn_8023793C();
    extern void fn_80238538();
    extern void fn_80238600();
    extern void fn_802386C8();
    extern void fn_8023881C();
    extern void fn_802389D4();
    extern void fn_80238B0C();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_8023943C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_802397B8();
    extern void fn_802398E4();
    extern void fn_80239984();
    extern void fightTrainerAiAddValue();
    extern void fn_80239A40();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C530();
    extern void fn_8024FE80();
    u8 sp[0x860];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    f32 f8 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = 0x0;
    r5 = 0x43;
    r16 = r6;
    r15 = r3;
    r6 = 0x0;
    fightTrainerGetStatus();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fightTrainerGetStatus();
    r21 = r3 & 0xFFFF;
    r3 = 0x0;
    r4 = r21;
    r5 = 0x38;
    r6 = 0x0;
    fightTrainerGetStatus();
    r5 = (u32)lbl_8027A434;
    r0 = r3 & 0xFF;
    r5 = (u32)lbl_8027A434;
    r4 = 0x3;
    *(u8*)(sp + 0x7F0) = r0;
    r6 = (u32)sp + 0x14;
    r14 = 0x0;
    ctr_fn = (void(*)(void))r4;
    do {
        r3 = *(u32*)((u8*)r5 + 0x4);
        r0 = *(u32*)((u8*)r5 + 0x8);
        *(u32*)((u8*)r6 + 0x4) = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r5 + 0x4);
    r3 = r15;
    r4 = 0x0;
    r5 = 0x1;
    *(u32*)((u8*)r6 + 0x4) = r0;
    fn_80235B04();
    r3 = (u32)sp + 0x68;
    r5 = 0x0;
    r4 = 0x0;
    while (1) {
        r0 = r5 & 0xFFFF;
        if (r0 >= (u32)0x6) break;
        r5 = r5 + 0x1;
        *(u32*)(r3 + r0) = r4;

    }
    r4 = r16;
    r3 = (u32)sp + 0x110;
    fn_801FCEC4();
    r3 = r16;
    fightOutPokemonCheckFightOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r16;
        r4 = 0xe2;
        r5 = 0x0;
        fightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r14 = 0x1;
    }
    }
    r3 = r15;
    r4 = (u32)sp + 0x98;
    fn_801F87CC();
    r20 = r3;
    r4 = r15;
    r5 = (u32)sp + 0x34;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r4 = r15;
    r5 = (u32)sp + 0xb0;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r0 = r20 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r3 = -0x1;
        return;
    }
    r4 = r21;
    r3 = 0x0;
    r5 = 0x1f;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r0 = 0x0;
        r5 = (0x1 << 16);
        *(u32*)(sp + 0x8) = r0;
        r6 = r15;
        *(u32*)(sp + 0xC) = r0;
        r7 = 0x0;
        r8 = 0x0;
        *(u32*)(sp + 0x10) = r0;
        r9 = 0x0;
        r10 = 0x0;
        fn_8023A118();
        fn_800E0C54();
        r5 = r3 & 0xFFFF;
        r4 = r20 & 0xFFFF;
        r0 = (s32)r5 / (s32)r4;
        r3 = (u32)sp + 0x98;
        r0 = r0 * r4;
        r0 = r5 - r0;
        r17 = *(u32*)(r3 + r0);
        if (r17 != (u32)0x0) {
            r4 = r17;
            r3 = 0x0;
            fightFloorGetFightPokemonPtrToFightTrainerPtr();
            r0 = r3;
            r3 = r17;
            r14 = r0;
            fightPokemonGetPokemonPtr();
            r8 = 0x0;
            r5 = (0x1 << 16);
            r0 = 0x228;
            r7 = r3;
            r6 = r14;
            *(u32*)(sp + 0xC) = r0;
            r8 = 0x0;
            r9 = 0x0;
            r10 = 0x0;
            fn_8023A118();
            r3 = r17;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r3 = (s16)r3;
            return;
    }
    }
    r0 = r14 & 0xFF;
    if (r0 == (u32)0x1) {
        r14 = (u32)sp + 0x98;
        r17 = (u32)sp + 0x80;
        r18 = (u32)sp + 0x18;
        r19 = 0x0;
        r26 = 0x0;
        while (1) {
            r0 = r26 & 0xFFFF;
            if (r0 >= (u32)0xe) break;
            r4 = 0x0;
            r0 = r3 + 0x2;
            r23 = *(s16*)(r18 + r3);
            r24 = *(s16*)(r18 + r0);
            r3 = r4;
            while (1) {
                r0 = r4 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r4 = r4 + 0x1;
                *(u32*)(r17 + r0) = r3;

            }
            r22 = 0x0;
            r25 = r22;
            while (1) {
                r0 = r25 & 0xFFFF;
                if (r0 >= (u32)0x6) break;
                r19 = *(u32*)(r14 + r0);
                if (r19 != (u32)0x0 || (s32)r23 != (s32)r0 || (s32)r24 != (s32)r0) {
                    r0 = (s16)r23;
                    if (r19 >= (u32)0x0) {
                        r3 = r15;
                        r4 = r19;
                        fn_80238600();
                        r0 = r3 & 0xFF;

                    }
                    r0 = (s16)r24;
                    if ((s32)r23 >= (s32)r0) {
                        r3 = r15;
                        r4 = r19;
                        fn_80238538();
                        r0 = r3 & 0xFF;

                    }
                    r3 = (u32)sp + 0x80;
                    *(u32*)(r3 + r0) = r19;
                    r22 = r22 + 0x1;
                }
                r25 = r25 + 0x1;

            }
            r0 = r22 & 0xFFFF;
            r19 = r22;
            if (r0 != (u32)0x6) break;
            r26 = r26 + 0x2;

        }

        r0 = r19 & 0xFFFF;
        if (r0 != (u32)0xe) {
            fn_800E0C54();
            r5 = r3 & 0xFFFF;
            r4 = r19 & 0xFFFF;
            r0 = (s32)r5 / (s32)r4;
            r3 = (u32)sp + 0x80;
            r0 = r0 * r4;
            r0 = r5 - r0;
            r14 = *(u32*)(r3 + r0);
            if (r14 != (u32)0x0) {
                r4 = r14;
                r3 = 0x0;
                fightFloorGetFightPokemonPtrToFightTrainerPtr();
                r15 = r3;
                r3 = r14;
                fightPokemonGetPokemonPtr();
                r8 = 0x0;
                r5 = (0x1 << 16);
                r0 = 0x228;
                r7 = r3;
                r6 = r15;
                *(u32*)(sp + 0xC) = r0;
                r8 = 0x0;
                r9 = 0x0;
                r10 = 0x0;
                fn_8023A118();
                r3 = r14;
                r4 = 0x0;
                r5 = 0xce;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r3 = (s16)r3;
                return;
    }
    }
    }
    r4 = (0xffff << 16);
    r3 = (0x1 << 16);
    r18 = r4 + 0x1;
    r22 = (u32)sp + 0x98;
    r17 = r20 & 0xFFFF;
    r19 = 0x0;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r17) break;
        r24 = *(u32*)(r22 + r0);
        if (r24 != (u32)0x0) {
            r3 = r24;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r0 = (s16)r3;
            if (r24 >= (u32)0x0) {
                r3 = r15;
                r4 = r24;
                fn_802386C8();
                r0 = r3;
                r3 = r15;
                r25 = r0;
                r4 = r24;
                fn_802389D4();
                r0 = r3;
                r3 = r24;
                r24 = r0;
                fightPokemonGetPokemonPtr();
                r4 = 0x0;
                r5 = 0xc9;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r3 = r3 & 0xFFFF;
                if (r19 < r25) {
                    r19 = r25;
                }
                if ((s32)r18 < (s32)r24) {
                    r18 = r24;
                }
                r0 = r14 & 0xFFFF;
                if (r0 > r3) {
                    r14 = r3;
        }
        }
        }
        r23 = r23 + 0x1;

    }
    r4 = r21;
    r3 = 0x0;
    r5 = 0x21;
    r6 = 0x0;
    fightTrainerGetStatus();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r22 = (u32)sp + 0x98;
        r17 = r20 & 0xFFFF;
        r23 = 0x0;
        while (1) {
            r0 = r23 & 0xFFFF;
            if (r0 >= (u32)r17) break;
            r24 = *(u32*)(r22 + r0);
            if (r24 != (u32)0x0) {
                r3 = r24;
                r4 = 0x0;
                r5 = 0xce;
                r6 = 0x0;
                ((void(*)(void))pokemonGetStatus)();
                r0 = (s16)r3;
                if (r24 >= (u32)0x0) {
                    r4 = r24;
                    r3 = 0x0;
                    fightFloorGetFightPokemonPtrToFightTrainerPtr();
                    r3 = r24;
                    fightPokemonGetPokemonPtr();
                    r4 = 0x0;
                    r5 = 0xc9;
                    r6 = 0x0;
                    ((void(*)(void))pokemonGetStatus)();
                    r3 = r3 & 0xFFFF;
                    r0 = r14 & 0xFFFF;
                    if (r0 >= (u32)r3) {
                        r4 = r24;
                        r3 = 0x0;
                        fightFloorGetFightPokemonPtrToFightTrainerPtr();
                        r14 = r3;
                        r3 = r24;
                        fightPokemonGetPokemonPtr();
                        r8 = 0x0;
                        r5 = (0x1 << 16);
                        r0 = 0x228;
                        r7 = r3;
                        r6 = r14;
                        *(u32*)(sp + 0xC) = r0;
                        r8 = 0x0;
                        r9 = 0x0;
                        r10 = 0x0;
                        fn_8023A118();
                        r3 = r24;
                        r4 = 0x0;
                        r5 = 0xce;
                        r6 = 0x0;
                        ((void(*)(void))pokemonGetStatus)();
                        r3 = (s16)r3;
                        return;
            }
            }
            }
            r23 = r23 + 0x1;

        }
    }
    r0 = *(u8*)(sp + 0x7F0);
    r28 = 0x0;
    r0 = r3 + 0x1;
    *(u32*)(sp + 0x7F4) = r0;
    r0 = r20 & 0xFFFF;
    *(u32*)(sp + 0x808) = r0;
    while (1) {
        r0 = *(u32*)(sp + 0x808);
        r3 = r28 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = (u32)sp + 0x98;
        r27 = *(u32*)(r3 + r30);
        if (r27 != (u32)0x0) {
            r3 = r27;
            r4 = 0x0;
            r5 = 0xce;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r0 = (s16)r3;
            if (r27 >= (u32)0x0) {
                r4 = r27;
                r3 = 0x0;
                fightFloorGetFightPokemonPtrToFightTrainerPtr();
                r26 = r3;
                r3 = r16;
                r4 = r27;
                r5 = 0x0;
                fightOutPokemonCreate();
                r3 = r15;
                r4 = r16;
                r5 = (u32)sp + 0x54;
                r6 = 0x0;
                r7 = 0x1;
                fn_802367CC();
                r0 = *(u32*)(sp + 0x7FC);
                r23 = r3;
                r31 = (u32)sp + 0x34;
                r14 = 0x0;
                r17 = r0 & 0xFFFF;
                r25 = 0x0;
                while (1) {
                    r0 = r25 & 0xFFFF;
                    if (r0 >= (u32)r17) break;
                    r29 = *(u32*)(r31 + r0);
                    if (r29 != (u32)0x0) {
                        r22 = r23 & 0xFFFF;
                        r24 = 0x0;
                        while (1) {
                            r0 = r24 & 0xFFFF;
                            if (r0 >= (u32)r22) break;
                            r3 = (u32)sp + 0x54;
                            r5 = *(u16*)(r3 + r0);
                            if (r5 == (u32)0x0 || r5 == (u32)0x165 || r0 == (u32)0x1) {

                                r3 = r15;
                                r4 = r16;
                                r6 = r29;
                                fn_8023C530();
                                r0 = r3 & 0xFF;

                                r14 = 0x1;
                                break;
                            }
                            r24 = r24 + 0x1;

                        }

                        r0 = r14 & 0xFF;
                        if (r0 == (u32)0x1) break;
                    }
                    r25 = r25 + 0x1;

                }

                r3 = r16;
                r4 = (u32)sp + 0x110;
                fn_801FCEC4();
                r3 = r15;
                r4 = r27;
                r5 = (u32)sp + 0x54;
                r6 = 0x0;
                r7 = 0x1;
                fn_802369B8();
                r3 = r27;
                fightPokemonGetPokemonPtr();
                r0 = 0x0;
                r5 = (0x1 << 16);
                *(u32*)(sp + 0x8) = r0;
                r0 = 0x227;
                r7 = r3;
                r29 = (u32)sp + 0x68;
                *(u32*)(sp + 0xC) = r0;
                r6 = r26;
                r0 = *(u32*)(r29 + r30);
                r8 = 0x0;
                r9 = 0x0;
                *(u32*)(sp + 0x10) = r0;
                r10 = 0x0;
                fn_8023A118();
                r3 = r15;
                r4 = r27;
                fn_80238600();
                r17 = r3;
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1a;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1a;
                    fn_80239EE8();
                }
                r0 = r17 & 0xFF;
                if (r0 == (u32)0x2) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1b;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1b;
                    fn_80239EE8();
                }
                r0 = r17 & 0xFF;
                if (r0 == (u32)0x3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1c;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1c;
                    fn_80239EE8();
                }
                r17 = 0x0;
                while (1) {
                    r0 = r17 & 0xFFFF;
                    if (r0 >= (u32)0x2) break;
                    r4 = r21;
                    r6 = r17;
                    r3 = 0x0;
                    r5 = 0x39;
                    fightTrainerGetStatus();
                    r5 = r3 & 0xFFFF;
                    if (r5 != (u32)0x9) {
                        r3 = r15;
                        r4 = r27;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r4 = r21;
                            r6 = r17;
                            r3 = 0x0;
                            r5 = 0x3a;
                            fightTrainerGetStatus();
                            r22 = r3 & 0xFF;
                            r3 = *(u32*)(r29 + r30);
                            r4 = r22;
                            r5 = r15;
                            r6 = 0x1d;
                            fn_802398E4();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r6 = (0x1 << 16);
                            r5 = r3;
                            r4 = r15;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x1d;
                            fn_80239A40();
                    }
                    }
                    r17 = r17 + 0x1;

                }
                r3 = r15;
                r4 = r27;
                fn_8023881C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1e;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1e;
                    fn_80239EE8();
                }
                r0 = r14 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x1f;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x1f;
                    fn_80239EE8();
                }
                r3 = r15;
                r4 = r27;
                fn_802386C8();
                if (r19 <= (u32)r3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x21;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x21;
                    fn_80239EE8();
                }
                r3 = r15;
                r4 = r27;
                fn_802389D4();
                if ((s32)r18 <= (s32)r3) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x20;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x20;
                    fn_80239EE8();
                }
                r0 = *(u32*)(sp + 0x7FC);
                r24 = 0x0;
                r14 = r0 & 0xFFFF;
                while (1) {
                    r0 = r24 & 0xFFFF;
                    if (r0 >= (u32)r14) break;
                    r3 = (u32)sp + 0x34;
                    r23 = *(u32*)(r3 + r0);
                    if (r23 != (u32)0x0) {
                        r0 = *(u32*)(sp + 0x804);
                        r25 = 0x0;
                        r31 = r0 & 0xFFFF;
                        while (1) {
                            r0 = r25 & 0xFFFF;
                            if (r0 >= (u32)r31) break;
                            r3 = (u32)sp + 0x54;
                            r22 = *(u16*)(r3 + r0);
                            if (r22 != (u32)0x0) {
                                r3 = r15;
                                r4 = r22;
                                r5 = r16;
                                fn_802395C8();
                                r0 = r3 & 0xFFFF;
                                r17 = r3;
                                if (r0 != (u32)0x9) {
                                    r3 = r15;
                                    r4 = r22;
                                    r5 = 0x1;
                                    fn_8023943C();
                                    r0 = r3 & 0xFF;
                                    if (r0 != (u32)0x9) {
                                        r3 = r15;
                                        r4 = r22;
                                        fn_80239500();
                                        r6 = r3;
                                        r3 = r15;
                                        r4 = r23;
                                        r5 = r17;
                                        fn_8023793C();
                                        r0 = r3 & 0xFFFF;
                                        if (r0 == (u32)0x41) {
                                            r3 = *(u32*)(r29 + r30);
                                            r4 = r15;
                                            r5 = 0x22;
                                            fn_80239984();
                                            *(u32*)(r29 + r30) = r3;
                                            r3 = r27;
                                            fightPokemonGetPokemonPtr();
                                            r7 = (0x1 << 16);
                                            r5 = r3;
                                            r4 = r26;
                                            r6 = 0x0;
                                            r7 = 0x0;
                                            r8 = 0x0;
                                            r9 = 0x0;
                                            r10 = 0x22;
                                            fn_80239EE8();
                            }
                            }
                            }
                            }
                            r25 = r25 + 0x1;

                        }
                    }
                    r24 = r24 + 0x1;

                }
                r0 = *(u32*)(sp + 0x7FC);
                r22 = 0x0;
                r31 = r0 & 0xFFFF;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r31) break;
                    r3 = (u32)sp + 0x34;
                    r23 = *(u32*)(r3 + r0);
                    if (r23 != (u32)0x0) {
                        r3 = r15;
                        r4 = r23;
                        r5 = (u32)sp + 0x54;
                        r6 = 0x0;
                        r7 = 0x0;
                        fn_802367CC();
                        r14 = r3 & 0xFFFF;
                        r17 = 0x0;
                        while (1) {
                            r0 = r17 & 0xFFFF;
                            if (r0 >= (u32)r14) break;
                            r3 = (u32)sp + 0x54;
                            r24 = *(u16*)(r3 + r0);
                            if (r24 != (u32)0x0) {
                                r3 = r15;
                                r4 = r24;
                                r5 = r23;
                                fn_802395C8();
                                r0 = r3 & 0xFFFF;
                                r25 = r3;
                                if (r0 != (u32)0x9) {
                                    r3 = r15;
                                    r4 = r24;
                                    r5 = 0x1;
                                    fn_8023943C();
                                    r0 = r3 & 0xFF;
                                    if (r0 != (u32)0x9) {
                                        r3 = r15;
                                        r4 = r24;
                                        fn_80239500();
                                        r6 = r3;
                                        r3 = r15;
                                        r4 = r27;
                                        r5 = r25;
                                        fn_80238B0C();
                                        r0 = r3 & 0xFFFF;
                                        if (r0 == (u32)0x41) {
                                            r3 = *(u32*)(r29 + r30);
                                            r4 = r15;
                                            r5 = 0x23;
                                            fn_80239984();
                                            *(u32*)(r29 + r30) = r3;
                                            r3 = r27;
                                            fightPokemonGetPokemonPtr();
                                            r7 = (0x1 << 16);
                                            r5 = r3;
                                            r4 = r26;
                                            r6 = 0x0;
                                            r7 = 0x0;
                                            r8 = 0x0;
                                            r9 = 0x0;
                                            r10 = 0x23;
                                            fn_80239EE8();
                            }
                            }
                            }
                            }
                            r17 = r17 + 0x1;

                        }
                    }
                    r22 = r22 + 0x1;

                }
                r4 = r21;
                r3 = 0x0;
                r5 = 0x20;
                r6 = 0x0;
                fightTrainerGetStatus();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r17 = 0x0;
                    while (1) {
                        r0 = lbl_80478B38;
                        r3 = r17 & 0xFFFF;
                        if (r3 >= (u32)r0) break;
                        r0 = r17 & 0xFFFF;
                        if (r0 != (u32)0x9) {
                            r3 = r15;
                            r4 = r27;
                            r5 = r17;
                            r6 = 0x1;
                            fn_80238B0C();
                            r0 = r3 & 0xFFFF;
                            if (r0 == (u32)0x41) {
                                r0 = *(u32*)(sp + 0x800);
                                r14 = (u32)sp + 0xb0;
                                r22 = 0x0;
                                r23 = r0 & 0xFFFF;
                                while (1) {
                                    r0 = r22 & 0xFFFF;
                                    if (r0 >= (u32)r23) break;
                                    r24 = *(u32*)(r14 + r0);
                                    if (r24 != (u32)0x0) {
                                        r4 = r24;
                                        r3 = 0x0;
                                        fightFloorGetFightPokemonPtrToFightTrainerPtr();
                                        if (r3 != (u32)0x0) {
                                            r4 = r24;
                                            fightTrainerCheckCanIrekaeFightPokemon();
                                            r0 = r3 & 0xFF;
                                    }
                                    }
                                    if (r0 != (u32)0x1 || r0 != (u32)0x2 && r0 != (u32)0x3 || r0 != (u32)0x2 && r0 != (u32)0x3) {

                                        if (r0 == (u32)0x2 || r0 == (u32)0x3) {

                                            r3 = r15;
                                            r4 = r24;
                                            r5 = r17;
                                            r6 = 0x1;
                                            fn_80238B0C();
                                            r0 = r3 & 0xFFFF;
                                            if (r0 == (u32)0x41) {
                                                r3 = *(u32*)(r29 + r30);
                                                r4 = r15;
                                                r5 = 0x24;
                                                fn_80239984();
                                                *(u32*)(r29 + r30) = r3;
                                                r3 = r27;
                                                fightPokemonGetPokemonPtr();
                                                r7 = (0x1 << 16);
                                                r5 = r3;
                                                r4 = r26;
                                                r6 = 0x0;
                                                r7 = 0x0;
                                                r8 = 0x0;
                                                r9 = 0x0;
                                                r10 = 0x24;
                                                fn_80239EE8();
                            }
                                        }
                                    }
                                    r22 = r22 + 0x1;

                                }
                    }
                        }
                        r17 = r17 + 0x1;

                    }
                }
                r3 = r15;
                r4 = r27;
                fn_8024FE80();
                r0 = r3 & 0xFFFF;
                r14 = r3;
                if (r3 != (u32)r0) {
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = r14;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r10 = r14;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    fn_80239EE8();
                }
                r0 = *(u32*)(sp + 0x7F8);
                r0 = r0 & 0xFF;
                if (r0 == (u32)0x2) {
                    r3 = r15;
                    r4 = r27;
                    r5 = 0x21;
                    fn_80239058();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x1) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x2c;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) goto L_8024F710;
                    }
                    r3 = *(u32*)(r29 + r30);
                    r4 = r15;
                    r5 = 0x29;
                    fn_80239984();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r26;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x29;
                    fn_80239EE8();

                } else {
                    if (r0 == (u32)0x1) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x22;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2a;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2a;
                            fn_80239EE8();
                        }
                    } else {
                    if (r0 == (u32)0x3) {
                        r3 = r15;
                        r4 = r27;
                        r14 = 0x0;
                        r5 = 0x8;
                        fn_80239058();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r14 = 0x1;
                        }
                        r3 = r15;
                        r4 = r27;
                        r5 = 0x8;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r15;
                            r4 = r27;
                            r5 = 0x5;
                            fn_80238E30();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x1) {
                                r3 = r15;
                                r4 = r27;
                                r5 = 0x4;
                                fn_80238E30();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                        }
                            }
                            r14 = 0x1;
                                }
                        r0 = r14 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2b;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2b;
                            fn_80239EE8();
                        }
                        goto L_8024F710;
                    }
                    }
                    if (r0 == (u32)0x4) {
                        r3 = r15;
                        r4 = r27;
                        r5 = 0xf;
                        fn_80238E30();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r3 = *(u32*)(r29 + r30);
                            r4 = r15;
                            r5 = 0x2c;
                            fn_80239984();
                            *(u32*)(r29 + r30) = r3;
                            r3 = r27;
                            fightPokemonGetPokemonPtr();
                            r7 = (0x1 << 16);
                            r5 = r3;
                            r4 = r26;
                            r6 = 0x0;
                            r7 = 0x0;
                            r8 = 0x0;
                            r9 = 0x0;
                            r10 = 0x2c;
                            fn_80239EE8();
                }
                    }
                }
            L_8024F710:
                fn_8000815C();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    fn_800E0C54();
                    r0 = *(u32*)(sp + 0x7F4);
                    r5 = r3 & 0xFFFF;
                    r3 = *(u32*)(r29 + r30);
                    r4 = (s32)r5 / (s32)r0;
                    r0 = r4 * r0;
                    r4 = r5 - r0;
                    r0 = *(u8*)(sp + 0x7F0);
                    r14 = r4 - r0;
                    r4 = r14;
                    fightTrainerAiAddValue();
                    *(u32*)(r29 + r30) = r3;
                    r3 = r27;
                    fightPokemonGetPokemonPtr();
                    r0 = 0x0;
                    r5 = (0x1 << 16);
                    *(u32*)(sp + 0x8) = r0;
                    r0 = 0x225;
                    r7 = r3;
                    r6 = r26;
                    *(u32*)(sp + 0xC) = r0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x0;
                    fn_8023A118();
                }
                r3 = r27;
                fightPokemonGetPokemonPtr();
                r0 = 0x0;
                r5 = (0x1 << 16);
                *(u32*)(sp + 0x8) = r0;
                r0 = 0x226;
                r7 = r3;
                r6 = r26;
                *(u32*)(sp + 0xC) = r0;
                r0 = *(u32*)(r29 + r30);
                r8 = 0x0;
                r9 = 0x0;
                r10 = 0x0;
                *(u32*)(sp + 0x10) = r0;
                fn_8023A118();
            }
        }
        r28 = r28 + 0x1;

    }
    r4 = r20;
    r3 = (u32)sp + 0x68;
    r5 = 0x1;
    fn_802397B8();
    if ((s32)r3 < (s32)0x0) {
        r3 = -0x1;
        return;
    }
    r14 = r3 << 2;
    r3 = (u32)sp + 0x98;
    r15 = *(u32*)(r3 + r14);
    if (r15 == (u32)0x0) {
        r3 = -0x1;
        return;
    }
    r4 = r15;
    r3 = 0x0;
    fightFloorGetFightPokemonPtrToFightTrainerPtr();
    r16 = r3;
    r3 = r15;
    fightPokemonGetPokemonPtr();
    r0 = 0x0;
    r4 = (u32)sp + 0x68;
    *(u32*)(sp + 0x8) = r0;
    r0 = 0x228;
    r5 = (0x1 << 16);
    r7 = r3;
    *(u32*)(sp + 0xC) = r0;
    r6 = r16;
    r8 = 0x0;
    r0 = *(u32*)(r4 + r14);
    r9 = 0x0;
    *(u32*)(sp + 0x10) = r0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r15;
    r4 = 0x0;
    r5 = 0xce;
    r6 = 0x0;
    ((void(*)(void))pokemonGetStatus)();
    r3 = (s16)r3;

    return;
}

/* Address: 0x8024E578 | Size: 0x118 (280 bytes) */
u32 fightTrainerAiSelectFightActionIrekae(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D30[];
    int new_var2;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void*, s32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern void fn_8023A118(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32);
    long long new_var;
    extern s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void*, u32, u32, u32);
    extern s32 fightTrainerAiGetFightPokemonIrekaeModosuValue();
    u32 choice;
    s32 score;
    u32 field;

    fightTrainerGetStatus(0, fightTrainerGetStatus((u32)ctx, 0, 0x43, 0) & 0xffff, 2, 0);
    field = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param1);
    score = fightTrainerAiGetFightPokemonIrekaeModosuValue(ctx, param1, param2);
    if (score <= 0) {
        return 0;
    }
    new_var = !ctx;
    if (new_var) {
    }
    new_var2 = 0;
    fn_8023A118(0xec63, 0xec04, 0xec05, field, fightOutPokemonGetPokemonPtr(param1), 0, new_var2, new_var2, 0,
                (((((0x228 & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF, score);
    choice = fightTrainerAiSelectIrekaeDasuFightPokemon(ctx, param2, 1, param1);
    if ((s16)choice < 0) {
        return 0;
    }
    new_var = choice;
    fightOutPokemonCreateFightAction(param1, new_var2, 9, 0, lbl_80375D30, (s16)new_var);
    return 1;
}

/* Address: 0x8024F8B4 | Size: 0x5CC (1484 bytes) */
s32 fightTrainerAiGetFightPokemonIrekaeModosuValue(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_8023565C();
    extern void fn_80235714();
    extern void fn_802367CC();
    extern void fn_80236E9C();
    extern void fn_80237288();
    extern void fn_8023753C();
    extern void fn_802376EC();
    extern void fn_8023785C();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C370();
    extern void fn_8023C530();
    extern s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId();
    u8 sp[0xA0];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r14 = 0;
    u32 r15 = 0;
    u32 r16 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r5 = 0x43;
    r6 = 0x0;
    r16 = r4;
    r15 = r3;
    r4 = 0x0;
    fightTrainerGetStatus();
    r4 = r3 & 0xFFFF;
    r3 = 0x0;
    r5 = 0x2;
    r6 = 0x0;
    fightTrainerGetStatus();
    r4 = r16;
    r24 = 0x0;
    r3 = 0x0;
    fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    r0 = r3;
    r3 = r15;
    r20 = r0;
    r4 = r16;
    fn_80236E9C();
    r26 = r3;
    r3 = r15;
    r4 = r16;
    r5 = (u32)sp + 0x38;
    r6 = 0x0;
    r7 = 0x1;
    fn_802367CC();
    r27 = r3;
    r4 = r15;
    r5 = (u32)sp + 0x18;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r28 = r3;
    r14 = (u32)sp + 0x18;
    r31 = r3 & 0xFFFF;
    r18 = 0x0;
    r17 = 0x0;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r19 = *(u32*)(r14 + r0);
        if (r19 != (u32)0x0) {
            r3 = r15;
            r4 = r19;
            fn_802376EC();
            r25 = r3 & 0xFFFF;
            r30 = r27 & 0xFFFF;
            r21 = 0x0;
            while (1) {
                r0 = r21 & 0xFFFF;
                if (r0 >= (u32)r30) break;
                r3 = (u32)sp + 0x38;
                r22 = *(u16*)(r3 + r0);
                if (r22 != (u32)0x0) {
                    if (r22 != (u32)0x165) {
                        r3 = r15;
                        r4 = r16;
                        r5 = r22;
                        r6 = r19;
                        fn_8023C530();
                        r29 = r3;
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r17 = 0x1;
                        }
                        r3 = r15;
                        r4 = r19;
                        fn_80237288();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r17 = 0x1;
                        }
                        r3 = r15;
                        r4 = r22;
                        r5 = 0x1;
                        fn_8023943C();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x1) {
                            r3 = r15;
                            r4 = r16;
                            r5 = r22;
                            r6 = r19;
                            r7 = 0x0;
                            fn_8023C370();
                            if ((s32)r25 < (s32)r3) {
                                r0 = r29 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r18 = 0x1;
                }
                }
                }
                }
                }
                r21 = r21 + 0x1;

            }
        }
        r23 = r23 + 0x1;

    }
    r3 = r16;
    fightOutPokemonGetPokemonPtr();
    r8 = 0x0;
    r5 = (0x1 << 16);
    r0 = 0x227;
    r7 = r3;
    r6 = r20;
    *(u32*)(sp + 0xC) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r15;
    r4 = r16;
    fn_8023785C();
    r0 = r3 & 0xFF;
    if (r0 != (u32)r31) {
        r4 = r15;
        r3 = 0x0;
        r5 = 0x1;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x1;
        fn_80239EE8();
    }
    r3 = r15;
    r4 = r16;
    fn_8023753C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)r31) {
        r3 = r24;
        r4 = r15;
        r5 = 0x2;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x2;
        fn_80239EE8();

    } else {
        r3 = r15;
        r4 = r16;
        fn_8023785C();
        r0 = r3 & 0xFF;
        if (r0 != (u32)r31) {
            r3 = r24;
            r4 = r15;
            r5 = 0x3;
            fn_80239984();
            r0 = r3;
            r3 = r16;
            r24 = r0;
            fightOutPokemonGetPokemonPtr();
            r7 = (0x1 << 16);
            r5 = r3;
            r4 = r20;
            r6 = 0x0;
            r7 = 0x0;
            r8 = 0x0;
            r9 = 0x0;
            r10 = 0x3;
            fn_80239EE8();
        }
    }
    r3 = r15;
    r4 = r16;
    fn_80235714();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x4;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x4;
        fn_80239EE8();
    }
    r3 = r15;
    r4 = r16;
    fn_8023565C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x5;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x5;
        fn_80239EE8();
    }
    r0 = r17 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x6;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x6;
        fn_80239EE8();
    }
    r0 = r18 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r24;
        r4 = r15;
        r5 = 0x7;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r7 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        r10 = 0x7;
        fn_80239EE8();
    }
    r3 = r15;
    r4 = r16;
    fn_8023753C();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r17 = (u32)sp + 0x18;
        r14 = r28 & 0xFFFF;
        r18 = 0x0;
        while (1) {
            r0 = r18 & 0xFFFF;
            if (r0 >= (u32)r14) break;
            r4 = *(u32*)(r17 + r0);
            if (r4 != (u32)0x0) {
                r3 = r15;
                fn_80236E9C();
                r3 = r3 & 0xFFFF;
                r0 = r26 & 0xFFFF;
                if (r3 > r0) {
                    r3 = r24;
                    r4 = r15;
                    r5 = 0x8;
                    fn_80239984();
                    r0 = r3;
                    r3 = r16;
                    r24 = r0;
                    fightOutPokemonGetPokemonPtr();
                    r7 = (0x1 << 16);
                    r5 = r3;
                    r4 = r20;
                    r6 = 0x0;
                    r7 = 0x0;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x8;
                    fn_80239EE8();
            }
            }
            r18 = r18 + 0x1;

        }
    }
    r3 = r15;
    r4 = r16;
    fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId();
    r0 = r3 & 0xFFFF;
    r14 = r3;
    if (r0 != (u32)r14) {
        r3 = r24;
        r4 = r15;
        r5 = r14;
        fn_80239984();
        r0 = r3;
        r3 = r16;
        r24 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r20;
        r10 = r14;
        r6 = 0x0;
        r7 = 0x0;
        r8 = 0x0;
        r9 = 0x0;
        fn_80239EE8();
    }
    r3 = r16;
    fightOutPokemonGetPokemonPtr();
    r0 = 0x0;
    r5 = (0x1 << 16);
    *(u32*)(sp + 0x8) = r0;
    r0 = 0x226;
    r7 = r3;
    r6 = r20;
    *(u32*)(sp + 0xC) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r24;
    return r3;
}

/* Address: 0x8024FE80 | Size: 0x1F0 (496 bytes) */
void fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A420[];
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fn_80235B04();
    extern void fn_80236FFC();
    extern void fn_8023715C();
    extern void fn_80237F74();
    extern void fn_80239058();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r5 = 0x1;
    r30 = r4;
    r29 = r3;
    r4 = 0x0;
    fn_80235B04();
    r4 = (u32)lbl_8027A420;
    r0 = 0x2;
    r4 = (u32)lbl_8027A420;
    r31 = r3;
    r6 = (u32)sp + 0x4;
    ctr_fn = (void(*)(void))r0;
    do {
        r3 = *(u32*)((u8*)r4 + 0x4);
        r0 = *(u32*)((u8*)r4 + 0x8);
        *(u32*)((u8*)r6 + 0x4) = r3;
        r6 += 8; *(u32*)r6 = r0;
    } while (--ctr != 0);
    r0 = *(u32*)((u8*)r4 + 0x4);
    r4 = r29;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    *(u32*)((u8*)r6 + 0x4) = r0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r28 = r3;
    r3 = r29;
    r4 = r30;
    r5 = 0x16;
    fn_80239058();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r25 = (u32)sp + 0x1c;
        r26 = r28 & 0xFFFF;
        r24 = 0x0;
        while (1) {
            r0 = r24 & 0xFFFF;
            if (r0 >= (u32)r26) break;
            r3 = r29;
            r4 = *(u32*)(r25 + r27);
            fn_8023715C();
            r4 = *(u32*)(r25 + r27);
            r27 = r3;
            r3 = r29;
            fn_80236FFC();
            r4 = r27 & 0xFFFF;
            r0 = r3 & 0xFFFF;
            if (r4 >= (u32)r0) {
                r3 = 0x25;
                return;
            }
            r24 = r24 + 0x1;

        }
    }
    r3 = r29;
    r4 = r30;
    r5 = 0x24;
    fn_80239058();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r27 = (u32)sp + 0x8;
        r25 = (u32)sp + 0x1c;
        r28 = r28 & 0xFFFF;
        r24 = 0x0;
        while (1) {
            r0 = r24 & 0xFFFF;
            if (r0 >= (u32)r28) break;
            r22 = 0x0;
            r23 = 0x0;
            while (1) {
                r0 = r23 & 0xFFFF;
                if (r0 >= (u32)0xa) break;
                r4 = *(u32*)(r25 + r26);
                r5 = *(u16*)(r27 + r0);
                r3 = r29;
                fn_80237F74();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r22 = 0x1;
                    break;
                }
                r23 = r23 + 0x1;

            }

            r0 = r22 & 0xFF;
            if (r0 != (u32)0x1) {
                r3 = 0x26;
                return;
            }
            r24 = r24 + 0x1;

        }
    }
    r0 = r31 & 0xFF;
    if (r0 != (u32)r28) {
        r3 = r29;
        r4 = r30;
        r5 = 0x4d;
        fn_80239058();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x27;
            return;
        }
        r3 = r29;
        r4 = r30;
        r5 = 0xd;
        fn_80239058();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x28;
            return;
    }
    }
    r3 = 0x0;

    return;
}

/* Address: 0x80250070 | Size: 0x27C (636 bytes) */
s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern u8 fn_8023753C();

    if (fn_80236BFC(ctx, param1, 0x9) == 1) {
        return 0x9;
    }
    if (fn_80236BFC(ctx, param1, 0xA) == 1) {
        return 0xA;
    }
    if (fn_80236BFC(ctx, param1, 0x1E) == 1 && fn_8023753C(ctx, param1) == 0) {
        return 0xB;
    }
    if (fn_80236BFC(ctx, param1, 0xE) == 1) {
        return 0xC;
    }
    if (fn_80236BFC(ctx, param1, 0x17) == 1) {
        return 0xD;
    }
    if (fn_80236BFC(ctx, param1, 0x18) == 1) {
        return 0xE;
    }
    if (fn_80236BFC(ctx, param1, 0x19) == 1) {
        return 0xF;
    }
    if (fn_80236BFC(ctx, param1, 0x1B) == 1) {
        return 0x10;
    }
    if (fn_80236BFC(ctx, param1, 0x1C) == 1) {
        return 0x11;
    }
    if (fn_80236BFC(ctx, param1, 0x1D) == 1) {
        return 0x12;
    }
    if (fn_80236BFC(ctx, param1, 0x26) == 1) {
        return 0x13;
    }
    if (fn_80236BFC(ctx, param1, 0x27) == 1) {
        return 0x14;
    }
    if (fn_80236BFC(ctx, param1, 0x28) == 1) {
        return 0x15;
    }
    if (fn_80236BFC(ctx, param1, 0x29) == 1) {
        return 0x16;
    }
    if (fn_80236BFC(ctx, param1, 0x2A) == 1) {
        return 0x17;
    }
    return fn_80236BFC(ctx, param1, 0x30) == 1 ? 0x18 : 0;
}

/* Address: 0x802502EC | Size: 0x694 (1684 bytes) */
void fightTrainerAiSelectFightActionItem(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D70[];
    extern void pokemonIsDarkPokemon();
    extern void fn_80142984();
    extern void itemUse2PokemonSimulation();
    extern void fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightTrainerGetTemotiNormalItemDataIdAry();
    extern void fightTrainerGetStatus();
    extern void fightOutPokemonCreateFightActionUseItem();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fightOutPokemonCheckFightOut();
    extern void fightPokemonCheckFightOut();
    extern void fn_802126C4();
    extern void fn_80235714();
    extern void fn_80236C80();
    extern void fn_80237310();
    extern void fn_8023753C();
    extern void fn_8023785C();
    extern void fn_802397B8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    u8 sp[0x250];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;
    u32 (*getTrainerStatus)(u32, u32, u32, u32);
    u16 (*getItemIds)(void*, u16*, u32, u32);
    u16 (*getOutPokemon)(u32, void*, u32, u32, u32);
    u16 (*getFightPokemon)(u32, void*, u32, u32, u32);
    u8 (*checkOutPokemon)(void*);
    u8 (*checkOutState)(void*, void*);

    r29 = r4;
    r22 = r5;
    r28 = r3;
    getTrainerStatus = (u32 (*)(u32, u32, u32, u32))fightTrainerGetStatus;
    getItemIds = (u16 (*)(void*, u16*, u32, u32))
        fightTrainerGetTemotiNormalItemDataIdAry;
    getOutPokemon = (u16 (*)(u32, void*, u32, u32, u32))
        fightFloorGetFightTrainerFightOutPokemonPtrAry;
    getFightPokemon = (u16 (*)(u32, void*, u32, u32, u32))
        fightFloorGetFightTrainerFightPokemonPtrAry;
    checkOutPokemon = (u8 (*)(void*))fightOutPokemonCheckFightOut;
    checkOutState = (u8 (*)(void*, void*))fn_80235714;
    r4 = getTrainerStatus(r28, 0, 0x43, 0) & 0xFFFF;
    r3 = getTrainerStatus(0, r4, 2, 0);
    r30 = 0x0;
    r31 = getItemIds((void*)r28, (u16*)(sp + 0x38), 0x14, 1);
    r0 = r31 & 0xFFFF;
    if ((s32)r0 == (s32)0) {
        r3 = 0x0;
        return;
    }
    r17 = getOutPokemon(0, (void*)(sp + 0x18), r28, 1, 1);
    r26 = getFightPokemon(0, (void*)(sp + 0x60), r28, 1, 1);
    r18 = (u32)sp + 0x18;
    r17 = r17 & 0xFFFF;
    r20 = 0x0;
    while (1) {
        r0 = r20 & 0xFFFF;
        if (r0 >= (u32)r17) break;
        r19 = *(u32*)(r18 + r0);
        if (r19 != (u32)0x0) {
            r3 = checkOutPokemon((void*)r19);
            r0 = r3 & 0xFF;
            if (r19 != (u32)0x0) {
                r3 = checkOutState((void*)r28, (void*)r19);
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r30 = 0x0;
        }
        }
        }
        r20 = r20 + 0x1;

    }
    r3 = (u32)sp + 0xc0;
    r5 = 0x0;
    r4 = 0x0;
    while (1) {
        r0 = r5 & 0xFFFF;
        if (r0 >= (u32)0x14) break;
        r5 = r5 + 0x1;
        *(u32*)(r3 + r0) = r4;

    }
    r20 = (u32)sp + 0x38;
    r27 = r31 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r27) break;
        r25 = r24 & 0xFFFF;
        r23 = *(u16*)(r20 + r0);
        if (r23 != (u32)0x0) {
            r3 = r23;
            fn_80142984();
            r0 = r3 & 0xFF;
            if (r23 != (u32)0x0) {
                r3 = r23;
                r4 = r29;
                fn_802126C4();
                r21 = r3;
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x7) {
                    r3 = r29;
                    r4 = 0x0;
                    r5 = 0xd5;
                    r6 = 0x0;
                    ((void(*)(void))pokemonGetStatus)();
                    r5 = r3;
                    r6 = r23;
                    r3 = (u32)sp + 0x110;
                    r4 = 0x0;
                    r7 = 0x0;
                    itemUse2PokemonSimulation();
                    r3 = (s16)r3;
                    r0 = -r3;
                    r0 = r0 & ~r3;
                    if (r0 != (u32)0x7) {
                        r0 = r21 & 0xFF;

                        if (r0 == (u32)0x2 || r0 == (u32)0x1) {

                            r3 = r28;
                            r4 = r29;
                            fn_8023753C();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r3 = r29;
                                fightOutPokemonGetPokemonPtr();
                                pokemonIsDarkPokemon();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r17 = r25 << 2;
                                    r18 = (u32)sp + 0xc0;
                                    r3 = *(u32*)(r18 + r17);
                                    r4 = r28;
                                    r5 = 0x2e;
                                    fn_80239984();
                                    *(u32*)(r18 + r17) = r3;
                                    r3 = r29;
                                    fightOutPokemonGetPokemonPtr();
                                    r6 = (0x1 << 16);
                                    r5 = r3;
                                    r4 = r28;
                                    r9 = r23;
                                    r6 = 0x0;
                                    r7 = 0x0;
                                    r8 = 0x0;
                                    r10 = 0x2e;
                                    fn_80239EE8();
        }
            }
                        }
                        r0 = r21 & 0xFF;

                        if (r0 == (u32)0x3 || r0 == (u32)0x1) {

                            r3 = r28;
                            r4 = r29;
                            fn_80237310();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r3 = r29;
                                fightOutPokemonGetPokemonPtr();
                                pokemonIsDarkPokemon();
                                r0 = r3 & 0xFF;
                                if (r0 == (u32)0x1) {
                                    r17 = r25 << 2;
                                    r18 = (u32)sp + 0xc0;
                                    r3 = *(u32*)(r18 + r17);
                                    r4 = r28;
                                    r5 = 0x2f;
                                    fn_80239984();
                                    *(u32*)(r18 + r17) = r3;
                                    r3 = r29;
                                    fightOutPokemonGetPokemonPtr();
                                    r6 = (0x1 << 16);
                                    r5 = r3;
                                    r4 = r28;
                                    r9 = r23;
                                    r6 = 0x0;
                                    r7 = 0x0;
                                    r8 = 0x0;
                                    r10 = 0x2f;
                                    fn_80239EE8();
        }
            }
                        }
                        r0 = r21 & 0xFF;
                        if (r0 == (u32)0x5) {
                            r3 = r28;
                            r4 = r29;
                            fn_80235714();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x5) {
                                r17 = r25 << 2;
                                r18 = (u32)sp + 0xc0;
                                r3 = *(u32*)(r18 + r17);
                                r4 = r28;
                                r5 = 0x30;
                                fn_80239984();
                                *(u32*)(r18 + r17) = r3;
                                r3 = r29;
                                fightOutPokemonGetPokemonPtr();
                                r6 = (0x1 << 16);
                                r5 = r3;
                                r4 = r28;
                                r9 = r23;
                                r6 = 0x0;
                                r7 = 0x0;
                                r8 = 0x0;
                                r10 = 0x30;
                                fn_80239EE8();
                        }
                        }
                        r0 = r21 & 0xFF;
                        if (r0 == (u32)0x4) {
                            r3 = r28;
                            r4 = r29;
                            fn_80236C80();
                            r0 = r3 & 0xFF;
                            if (r0 == (u32)0x2) {
                                r17 = r25 << 2;
                                r18 = (u32)sp + 0xc0;
                                r3 = *(u32*)(r18 + r17);
                                r4 = r28;
                                r5 = 0x31;
                                fn_80239984();
                                *(u32*)(r18 + r17) = r3;
                                r3 = r29;
                                fightOutPokemonGetPokemonPtr();
                                r6 = (0x1 << 16);
                                r5 = r3;
                                r4 = r28;
                                r9 = r23;
                                r6 = 0x0;
                                r7 = 0x0;
                                r8 = 0x0;
                                r10 = 0x31;
                                fn_80239EE8();
                        }
                        }
                        r0 = r21 & 0xFF;
                        if (r0 == (u32)0x6) {
                            r0 = r30 & 0xFF;
                            if (r0 == (u32)0x1) {
                                r17 = r25 << 2;
                                r18 = (u32)sp + 0xc0;
                                r3 = *(u32*)(r18 + r17);
                                r4 = r28;
                                r5 = 0x32;
                                fn_80239984();
                                *(u32*)(r18 + r17) = r3;
                                r3 = r29;
                                fightOutPokemonGetPokemonPtr();
                                r6 = (0x1 << 16);
                                r5 = r3;
                                r4 = r28;
                                r9 = r23;
                                r6 = 0x0;
                                r7 = 0x0;
                                r8 = 0x0;
                                r10 = 0x32;
                                fn_80239EE8();
                        }
                        }
                        r3 = r28;
                        r4 = r29;
                        fn_8023785C();
                        r0 = r3 & 0xFF;
                        if (r0 != (u32)0x2) {
                            r3 = r28;
                            r4 = r29;
                            fn_8023785C();
                            r0 = r3 & 0xFF;
                            if (r0 != (u32)0x3) {
                                r19 = (u32)sp + 0x60;
                                r17 = r26 & 0xFFFF;
                                r18 = 0x0;
                                while (1) {
                                    r0 = r18 & 0xFFFF;
                                    if (r0 >= (u32)r17) break;
                                    r3 = r29;
                                    r4 = 0x0;
                                    r5 = 0xd5;
                                    r6 = 0x0;
                                    ((void(*)(void))pokemonGetStatus)();
                                    r0 = *(u32*)(r19 + r21);
                                    if (r3 != (u32)r0) {
                                        r3 = r28;
                                        r4 = r29;
                                        fn_8023785C();
                                        r0 = r3 & 0xFF;
                                        if (r0 != (u32)0x2) {
                                            r3 = r28;
                                            r4 = r29;
                                            fn_8023785C();
                                            r0 = r3 & 0xFF;
                                            if (r0 == (u32)0x3) {
                                            }
                                            r3 = *(u32*)(r19 + r21);
                                            fightPokemonCheckFightOut();
                                            r0 = r3 & 0xFF;
                                            if (r0 == (u32)0x1) {
                                                r17 = r25 << 2;
                                                r18 = (u32)sp + 0xc0;
                                                r3 = *(u32*)(r18 + r17);
                                                r4 = r28;
                                                r5 = 0x33;
                                                fn_80239984();
                                                *(u32*)(r18 + r17) = r3;
                                                r3 = r29;
                                                fightOutPokemonGetPokemonPtr();
                                                r6 = (0x1 << 16);
                                                r5 = r3;
                                                r4 = r28;
                                                r9 = r23;
                                                r6 = 0x0;
                                                r7 = 0x0;
                                                r8 = 0x0;
                                                r10 = 0x33;
                                                fn_80239EE8();
                                                break;
                    }
                                        }
                                            }
                                    r18 = r18 + 0x1;

                                }
        }
                        }
                        r3 = r29;
                        fightOutPokemonGetPokemonPtr();
                        r0 = 0x226;
                        r4 = (u32)sp + 0xc0;
                        r5 = (0x1 << 16);
                        *(u32*)(sp + 0xC) = r0;
                        r0 = r25 << 2;
                        r7 = r3;
                        r6 = r28;
                        r0 = *(u32*)(r4 + r0);
                        *(u32*)(sp + 0x10) = r0;
                        r8 = 0x0;
                        r9 = 0x0;
                        r10 = 0x0;
                        fn_8023A118();
        }
        }
        }
        }
        r24 = r24 + 0x1;

    }
    r4 = (u32)sp + 0xc0;
    r0 = r31 & 0xFFFF;
    r17 = 0x0;
    while (1) {
        r3 = r17 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u32*)(r4 + r3);
        if ((s32)r3 > (s32)0x0) break;
        r17 = r17 + 0x1;

    }

    r3 = r17 & 0xFFFF;
    r0 = r31 & 0xFFFF;
    if (r3 >= (u32)r0) {
        r3 = 0x0;
        return;
    }
    r4 = r31;
    r3 = (u32)sp + 0xc0;
    r5 = 0x1;
    fn_802397B8();
    if (r3 < r0) {
        r3 = 0x0;
        return;
    }
    r3 = (u32)sp + 0x38;
    r17 = *(u16*)(r3 + r0);
    if (r17 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r3 = r29;
    fightOutPokemonGetPokemonPtr();
    r0 = 0x228;
    r4 = (u32)sp + 0xc0;
    r5 = (0x1 << 16);
    *(u32*)(sp + 0xC) = r0;
    r0 = r18 << 2;
    r7 = r3;
    r6 = r28;
    r0 = *(u32*)(r4 + r0);
    *(u32*)(sp + 0x10) = r0;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_8023A118();
    r3 = r29;
    r4 = r22;
    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    r0 = 0x0;
    r4 = (u32)lbl_80375D70;
    *(u32*)(sp + 0x8) = r0;
    r9 = r3;
    r7 = (u32)lbl_80375D70;
    r3 = r29;
    r8 = r17;
    r4 = 0x0;
    r5 = 0x12;
    r6 = 0x0;
    r10 = -0x1;
    fightOutPokemonCreateFightActionUseItem();
    r3 = 0x1;

    return;
}
