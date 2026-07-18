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
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
u8 fn_80237310(void*, u32);
u8 fn_80237F74(void*, u32, u32);
u8 fn_802384B4(void*, u32, u32);
u8 fn_80239564(void*, u32);
u8 fn_80235B04(void*, u32, u32);
u16 fn_80238980(void*, u32);
u8 fn_80238E30(void*, u32, u32);
u8 fn_80239058(void*, u32, u32);
u32 fn_80239984(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);


/* Address: 0x80241660 | Size: 0x510 (1296 bytes) */
void fightTrainerAiWazaValueTedasuke(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern void fightActionGetKindDataId();
    extern void fightActionCheckValid();
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fn_80202108();
    extern void fn_80202234();
    extern void fightOutPokemonGetUseWazaDataId();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80237F74();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    f32 f9 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r6 = 0x1;
    r7 = 0x1;
    r28 = r3;
    r29 = r4;
    r30 = r5;
    r5 = (u32)sp + 0x1c;
    r4 = r28;
    r31 = 0x0;
    r3 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r26 = r3;
    r21 = 0x0;
    r22 = 0x0;
    while (1) {
        r3 = lbl_80478DF8;
        r4 = r22 & 0xFFFF;
        r0 = *(u32*)((u8*)r3 + 0x0);
        if (r4 >= (u32)r0) break;
        r0 = r22 & 0xFFFF;
        if ((s32)r0 != (s32)0) {
            if (r0 != (u32)0x165) {
                if (r0 != (u32)0x163) {
                    r3 = r28;
                    r4 = r22;
                    r5 = 0x1;
                    fn_8023943C();
                    r0 = r3 & 0xFF;
                    if (r0 != (u32)0x163) {
                        r4 = r28;
                        r7 = r22;
                        r3 = 0x0;
                        r5 = 0x1;
                        r6 = 0x1;
                        r8 = 0x0;
                        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
                        r0 = r3 & 0xFF;
                        if (r0 == (u32)0x1) {
                            r21 = 0x1;
        }
        }
        }
        }
        }
        r22 = r22 + 0x1;

    }
    r0 = r21 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r28;
        r3 = 0x0;
        r5 = 0x1b0;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1b0;
        fn_80239EE8();
    }
    r27 = (u32)sp + 0x1c;
    r22 = r26 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r27 + r0);
        if (r29 != (u32)r4) {
            r3 = r28;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r25 = r3 & 0xFFFF;
            if (r29 != (u32)r4) {
                r23 = (u32)sp + 0x8;
                r21 = 0x0;
                while (1) {
                    r0 = r21 & 0xFFFF;
                    if (r0 >= (u32)r25) break;
                    r3 = r28;
                    r4 = *(u16*)(r23 + r0);
                    r5 = 0x1;
                    fn_8023943C();
                    r0 = r3 & 0xFF;
                    if (r29 != (u32)r4) {
                        r3 = r31;
                        r4 = r28;
                        r5 = 0x1b1;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r28;
                        r8 = r30;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x1b1;
                        fn_80239EE8();
                        break;
                    }
                    r21 = r21 + 0x1;

                }
        }
        }
        r24 = r24 + 0x1;

    }
    r27 = (u32)sp + 0x1c;
    r23 = r26 & 0xFFFF;
    r25 = 0x1;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r3 = *(u32*)(r27 + r22);
        if (r29 == (u32)r3 || r29 == (u32)r3 || r29 == (u32)r3 || r0 == (u32)0x13) {
            r4 = 0x0;
            r5 = 0xfe;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();

            fightActionCheckValid();
            r0 = r3 & 0xFF;

            r3 = r21;
            fightActionGetKindDataId();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0x13) {
                r25 = 0x0;
                break;
            }
            r3 = *(u32*)(r27 + r22);
            fightOutPokemonGetUseWazaDataId();
            r0 = r3;
            r3 = r28;
            r4 = r0;
            r5 = 0x1;
            fn_8023943C();
            r0 = r3 & 0xFF;

            r25 = 0x0;
            break;
        }
        r24 = r24 + 0x1;

    }

    r0 = r25 & 0xFF;
    if (r0 == (u32)r23) {
        r3 = r31;
        r4 = r28;
        r5 = 0x1b2;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r28;
        r8 = r30;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1b2;
        fn_80239EE8();
    }
    r25 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r4 = *(u32*)(r25 + r0);
        if (r29 != (u32)r4) {
            r3 = r28;
            r5 = 0x12;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b3;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r28;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1b3;
                fn_80239EE8();
                break;
        }
        }
        r22 = r22 + 0x1;

    }

    r25 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = *(u32*)(r25 + r0);
        if (r29 != (u32)r3) {
            r4 = 0x0;
            r5 = 0xf9;
            r6 = 0x0;
            ((void(*)(void))pokemonGetStatus)();
            r0 = r3 & 0xFF;
            if (r29 != (u32)r3) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b4;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r28;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1b4;
                fn_80239EE8();
                break;
        }
        }
        r22 = r22 + 0x1;

    }

    r27 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r23 = *(u32*)(r27 + r0);
        if (r29 != (u32)r23) {
            r3 = r28;
            r4 = r23;
            r5 = 0x8;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r29 == (u32)r23) {
                r0 = -0x1;

            } else {
                r3 = r28;
                r4 = r23;
                r5 = 0x30;
                fn_80237F74();
                r0 = r3 & 0xFF;
                r3 = r23;
                r0 = 0x1 - r0;
                r4 = 0x8;
                r0 = __cntlzw(r0);
                r5 = (u32)r0 >> 5;
                r26 = r5 + 0x1;
                fn_80202108();
                r22 = r3 + r26;
                r3 = r23;
                r4 = 0x8;
                fn_80202234();
                r3 = (s8)r3;
                r0 = (s8)r22;
                if ((s32)r0 >= (s32)r3) {
                    r0 = 0x1;
                    goto L_80241AF4;
                }
                r0 = 0x0;
            }
        L_80241AF4:
            r0 = (s8)r0;
            if ((s32)r0 == (s32)r3) {
                r3 = r31;
                r4 = r28;
                r5 = 0x1b5;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r28;
                r8 = r30;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1b5;
                fn_80239EE8();
                r3 = r31;
                return;
        }
        }
        r25 = r25 + 0x1;

    }

    r3 = r31;
    return;
}
