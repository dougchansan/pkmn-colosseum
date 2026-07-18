/**
 * @file fight_trainer_ai_waza_value_candidate_8024B474.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x8024B474 - 0x8024E578.
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

#if !defined(FIGHT_AI_VALUE_EXACT_8024DE8C_ONLY)

/* Address: 0x8024B474 | Size: 0x5D0 (1488 bytes) */
void fightTrainerAiWazaValueJisin(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023C370();
    u8 sp[0xF0];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r4;
    r28 = r5;
    r30 = r3;
    r31 = 0x0;
    r5 = r29;
    r4 = r28;
    fn_802395C8();
    r24 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x5c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r4 = r30;
    r5 = (u32)sp + 0x3c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r27 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r25 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r26 = 0x0;
    while (1) {
        r0 = r26 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r0 = *(u32*)(r25 + r23);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r25 + r23);
            r6 = r3;
            r3 = r30;
            r5 = r24;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x43) {
                r3 = r31;
                r4 = r30;
                r5 = 0xa9;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xa9;
                fn_80239EE8();
        }
        }
        r26 = r26 + 0x1;

    }
    r25 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r26 = 0x0;
    while (1) {
        r0 = r26 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r0 = *(u32*)(r25 + r23);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r25 + r23);
            r6 = r3;
            r3 = r30;
            r5 = r24;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x42) {
                r3 = r31;
                r4 = r30;
                r5 = 0xaa;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xaa;
                fn_80239EE8();
        }
        }
        r26 = r26 + 0x1;

    }
    r25 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r25 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r4 = r3 & 0xFFFF;
            r26 = r3;
            if (r29 != (u32)r4) {
                r3 = (u32)sp + 0x8;
                r23 = 0x0;
                while (1) {
                    r0 = r23 & 0xFFFF;
                    if (r0 >= (u32)r4) break;
                    r0 = *(u16*)(r3 + r0);
                    if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                        if (r0 == (u32)0xcb) {
                        }
                        r3 = r31;
                        r4 = r30;
                        r5 = 0xab;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r30;
                        r8 = r28;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0xab;
                        fn_80239EE8();
                        break;
                        }
                    r23 = r23 + 0x1;

                }

                r3 = r23 & 0xFFFF;
                r0 = r26 & 0xFFFF;
                if (r3 < r0) break;
        }
        }
        r24 = r24 + 0x1;

    }

    r4 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0xb6;
    r8 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0xac;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xac;
        fn_80239EE8();

    } else {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xc5;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0xac;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xac;
            fn_80239EE8();
            goto L_8024B878;
        }
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xcb;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0xac;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0xac;
            fn_80239EE8();
        }
    }
L_8024B878:
    r4 = r30;
    r8 = r29;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0x10e;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0xad;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xad;
        fn_80239EE8();
    }
    r24 = (u32)sp + 0x3c;
    r23 = r27 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r4 = *(u32*)(r24 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_80236520();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                if (r0 == (u32)0xcb) {
                }
                r3 = r31;
                r4 = r30;
                r5 = 0xae;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xae;
                fn_80239EE8();
            }
                }
        r22 = r22 + 0x1;

    }
    r22 = (u32)sp + 0x3c;
    r23 = r27 & 0xFFFF;
    r25 = 0x0;
    while (1) {
        r0 = r25 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r4 = *(u32*)(r22 + r24);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_802376EC();
            r6 = *(u32*)(r22 + r24);
            r24 = r3;
            r3 = r30;
            r4 = r29;
            r5 = r28;
            r7 = 0x0;
            fn_8023C370();
            r0 = r24 & 0xFFFF;
            if ((s32)r0 <= (s32)r3) {
                r3 = r31;
                r4 = r30;
                r5 = 0xaf;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0xaf;
                fn_80239EE8();
        }
        }
        r25 = r25 + 0x1;

    }
    r3 = r31;
    return;
}
/* Address: 0x8024BA44 | Size: 0x438 (1080 bytes) */
u32 fightTrainerAiWazaValueSokubaku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F8A18(u32, u16*);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235BE4(void*, u32, u32, u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 tmp;
    u32 found;
    u32 handle;
    u32 pokemon;
    u32 status;

    handle = 0;
    status = fn_80235BE4(ctx, 0, param3, 0);
    pokemon = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    tmp = 0;
    found = fn_801F8A18(pokemon, &tmp);
    if (found == 0) {
        status = 1;
    }
    if ((u8)status == 0) {
        handle = fn_80239984(0, ctx, 0x9f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9f);
    }
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(handle, ctx, 0xa0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa0);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0xa1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa1);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0xa2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa2);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0xa4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa4);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0xa5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa5);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0xa6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa6);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0xa7);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa7);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0xa3);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa3);
    }
    if ((u8)status != 0) {
        handle = fn_80239984(handle, ctx, 0xa8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0xa8);
    }
    return handle;
}
/* Address: 0x8024BE7C | Size: 0x144 (324 bytes) */
u32 fightTrainerAiWazaValueKorogaru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 fn_802387C8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    s32 value;
    s32 quotient;

    handle = 0;
    if (fn_80236BFC(ctx, param1, 0x1a) == 1) {
        handle = fn_80239984(0, ctx, 0x9c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9c);
    }
    value = fn_802387C8(ctx, param1);
    quotient = value / fightTrainerGetStatus(0, 0x9d, 0x3e, 0);
    handle = fightTrainerAiAddValue(handle, quotient);
    fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9d, quotient);
    handle = fn_80239984(handle, ctx, 0x9e);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x9e);
    return handle;
}
/* Address: 0x8024BFC0 | Size: 0x5FC (1532 bytes) */
void fightTrainerAiWazaValueJibaku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8023C370();
    u8 sp[0xF0];
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
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r29 = r4;
    r28 = r5;
    r30 = r3;
    r31 = 0x0;
    r5 = r29;
    r4 = r28;
    fn_802395C8();
    r25 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x5c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r4 = r30;
    r5 = (u32)sp + 0x3c;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r27 = r3;
    r4 = r30;
    r5 = (u32)sp + 0x1c;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r26 = r3;
    r24 = (u32)sp + 0x3c;
    r21 = r27 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r0 = *(u32*)(r24 + r22);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r24 + r22);
            r6 = r3;
            r3 = r30;
            r5 = r25;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x43) {
                r3 = r31;
                r4 = r30;
                r5 = 0x94;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x94;
                fn_80239EE8();
        }
        }
        r23 = r23 + 0x1;

    }
    r24 = (u32)sp + 0x3c;
    r21 = r27 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r0 = *(u32*)(r24 + r22);
        if (r29 != (u32)r0) {
            r3 = r30;
            r4 = r28;
            fn_80239500();
            r4 = *(u32*)(r24 + r22);
            r6 = r3;
            r3 = r30;
            r5 = r25;
            fn_8023793C();
            r0 = r3 & 0xFFFF;
            if (r0 == (u32)0x42) {
                r3 = r31;
                r4 = r30;
                r5 = 0x95;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x95;
                fn_80239EE8();
        }
        }
        r23 = r23 + 0x1;

    }
    r24 = (u32)sp + 0x3c;
    r21 = r27 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r4 = *(u32*)(r24 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            r5 = (u32)sp + 0x8;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r4 = r3 & 0xFFFF;
            r25 = r3;
            if (r29 != (u32)r4) {
                r3 = (u32)sp + 0x8;
                r22 = 0x0;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r4) break;
                    r0 = *(u16*)(r3 + r0);
                    if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                        if (r0 == (u32)0xcb) {
                        }
                        r3 = r31;
                        r4 = r30;
                        r5 = 0x96;
                        fn_80239984();
                        r0 = r3;
                        r3 = r29;
                        r31 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r30;
                        r8 = r28;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x96;
                        fn_80239EE8();
                        break;
                        }
                    r22 = r22 + 0x1;

                }

                r3 = r22 & 0xFFFF;
                r0 = r25 & 0xFFFF;
                if (r3 < r0) break;
        }
        }
        r23 = r23 + 0x1;

    }

    r4 = r30;
    r3 = 0x0;
    r5 = 0x1;
    r6 = 0x1;
    r7 = 0xb6;
    r8 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r31;
        r4 = r30;
        r5 = 0x97;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r31 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r30;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x97;
        fn_80239EE8();

    } else {
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xc5;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0x97;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x97;
            fn_80239EE8();
            goto L_8024C3C8;
        }
        r4 = r30;
        r3 = 0x0;
        r5 = 0x1;
        r6 = 0x1;
        r7 = 0xcb;
        r8 = 0x0;
        fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = r31;
            r4 = r30;
            r5 = 0x97;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x97;
            fn_80239EE8();
        }
    }
L_8024C3C8:
    r25 = (u32)sp + 0x1c;
    r22 = r26 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r3 = r30;
        r4 = *(u32*)(r25 + r0);
        r5 = 0x14;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r0 = r24 & 0xFFFF;
            r3 = r31;
            r4 = r30;
            r5 = r0 - r0; /* -borrow */;
            r23 = r5 + 0x99;
            r5 = r23;
            fn_80239984();
            r0 = r3;
            r3 = r29;
            r31 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r30;
            r8 = r28;
            r10 = r23;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            fn_80239EE8();
        }
        r24 = r24 + 0x1;

    }
    r23 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r21 = 0x0;
    while (1) {
        r0 = r21 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r23 + r0);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_80236520();
            r0 = r3 & 0xFFFF;
            if (r0 != (u32)0xb6 && r0 != (u32)0xc5) {

                if (r0 == (u32)0xcb) {
                }
                r3 = r31;
                r4 = r30;
                r5 = 0x9a;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x9a;
                fn_80239EE8();
            }
                }
        r21 = r21 + 0x1;

    }
    r21 = (u32)sp + 0x3c;
    r22 = r27 & 0xFFFF;
    r24 = 0x0;
    while (1) {
        r0 = r24 & 0xFFFF;
        if (r0 >= (u32)r22) break;
        r4 = *(u32*)(r21 + r23);
        if (r29 != (u32)r4) {
            r3 = r30;
            fn_802376EC();
            r6 = *(u32*)(r21 + r23);
            r23 = r3;
            r3 = r30;
            r4 = r29;
            r5 = r28;
            r7 = 0x0;
            fn_8023C370();
            r0 = r23 & 0xFFFF;
            if ((s32)r0 <= (s32)r3) {
                r3 = r31;
                r4 = r30;
                r5 = 0x9b;
                fn_80239984();
                r0 = r3;
                r3 = r29;
                r31 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r30;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x9b;
                fn_80239EE8();
        }
        }
        r24 = r24 + 0x1;

    }
    r3 = r31;
    return;
}
/* Address: 0x8024C5BC | Size: 0x91C (2332 bytes) */
void fightTrainerAiWazaValueTobihaneru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern s32 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
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
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r7 = 0x1;
    r31 = r3;
    r30 = r4;
    r29 = r5;
    r27 = r6;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r26 = 0x0;
    r3 = 0x0;
    r6 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r28 = r3;
    r3 = r31;
    r4 = r27;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x7e;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x7f;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7f;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x80;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x80;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x81;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x81;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x82;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x82;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x83;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x83;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x84;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x84;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x85;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x85;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r27;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r26;
        r4 = r31;
        r5 = 0x86;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x86;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = r27;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r3 = r26;
        r4 = r31;
        r5 = 0x87;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r26 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x87;
        fn_80239EE8();
    }
    r3 = r26;
    r4 = r31;
    r5 = 0x88;
    fn_80239984();
    r27 = r3;
    r3 = r30;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r29;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x88;
    fn_80239EE8();
    r3 = r31;
    r4 = r30;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x89;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x89;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8a;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8a;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8b;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8b;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8c;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8c;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8d;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8d;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8e;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x8f;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x8f;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x90;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x90;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x91;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x91;
        fn_80239EE8();
    }
    r26 = (u32)sp + 0x1c;
    r25 = r28 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r3 = r31;
        r4 = *(u32*)(r26 + r0);
        r5 = (u32)sp + 0x8;
        r6 = 0x0;
        r7 = 0x0;
        fn_802367CC();
        r23 = r3 & 0xFFFF;
        if (r0 != (u32)0x1) {
            r24 = (u32)sp + 0x8;
            r21 = 0x0;
            r22 = 0x0;
            while (1) {
                r0 = r22 & 0xFFFF;
                if (r0 >= (u32)r23) break;
                r3 = r31;
                r5 = *(u16*)(r24 + r0);
                r4 = 0x1f;
                fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = r27;
                    r4 = r31;
                    r5 = 0x92;
                    fn_80239984();
                    r27 = r3;
                    r3 = r30;
                    fightOutPokemonGetPokemonPtr();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r31;
                    r8 = r29;
                    r6 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x92;
                    fn_80239EE8();
                    r21 = 0x1;
                    break;
                }
                r22 = r22 + 0x1;

            }

            r0 = r21 & 0xFF;
            if (r0 == (u32)0x1) break;
        }
        r28 = r28 + 0x1;

    }

    r3 = r31;
    r4 = r30;
    r5 = 0x1d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r27;
        r4 = r31;
        r5 = 0x93;
        fn_80239984();
        r27 = r3;
        r3 = r30;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r29;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x93;
        fn_80239EE8();
    }
    r3 = r27;
    return;
}
/* Address: 0x8024CED8 | Size: 0x940 (2368 bytes) */
void fightTrainerAiWazaValueSorawotobu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_80236D60();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern s32 fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
    u8 sp[0x70];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r7 = 0x1;
    r31 = r3;
    r29 = r4;
    r28 = r5;
    r25 = r6;
    r4 = r31;
    r5 = (u32)sp + 0x1c;
    r30 = 0x0;
    r27 = 0x0;
    r3 = 0x0;
    r6 = 0x0;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r26 = r3;
    r3 = r31;
    r4 = r25;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x69;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x69;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6a;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6a;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6b;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6b;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6c;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6c;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6d;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6d;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6e;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6e;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x6f;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x6f;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x70;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x70;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r25;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x71;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x71;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = r25;
    fn_80236D60();
    if ((s32)r3 > (s32)0x0) {
        r3 = r30;
        r4 = r31;
        r5 = 0x72;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x72;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x73;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x73;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x74;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x74;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x75;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x75;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x76;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x76;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x9;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x77;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x77;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0xa;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x78;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x78;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x79;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x79;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x1e;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7a;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7a;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r29;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7b;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7b;
        fn_80239EE8();
    }
    r0 = r28 & 0xFFFF;

    if (r0 == (u32)0x13 || r0 == (u32)0x154) {

        r27 = 0x1f;

    } else {
        if (r0 == (u32)0x5b) {
            r27 = 0x20;
            goto L_8024D6BC;
        }
        if (r0 == (u32)0x123) {
            r27 = 0x21;
        }
    }
L_8024D6BC:
    r25 = (u32)sp + 0x1c;
    r24 = r26 & 0xFFFF;
    r26 = 0x0;
    while (1) {
        r0 = r26 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = r31;
        r4 = *(u32*)(r25 + r0);
        r5 = (u32)sp + 0x8;
        r6 = 0x0;
        r7 = 0x0;
        fn_802367CC();
        r22 = r3 & 0xFFFF;
        if (r0 != (u32)0x123) {
            r23 = (u32)sp + 0x8;
            r20 = 0x0;
            r21 = 0x0;
            while (1) {
                r0 = r21 & 0xFFFF;
                if (r0 >= (u32)r22) break;
                r3 = r31;
                r5 = *(u16*)(r23 + r0);
                r4 = r27;
                fightTrainerAiCheckJoutaiKieWazaHitWazaDataId();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
                    r3 = r30;
                    r4 = r31;
                    r5 = 0x7c;
                    fn_80239984();
                    r0 = r3;
                    r3 = r29;
                    r30 = r0;
                    fightOutPokemonGetPokemonPtr();
                    r6 = (0x1 << 16);
                    r5 = r3;
                    r4 = r31;
                    r8 = r28;
                    r6 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x7c;
                    fn_80239EE8();
                    r20 = 0x1;
                    break;
                }
                r21 = r21 + 0x1;

            }

            r0 = r20 & 0xFF;
            if (r0 == (u32)0x1) break;
        }
        r26 = r26 + 0x1;

    }

    r3 = r31;
    r4 = r29;
    r5 = 0x1d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r30;
        r4 = r31;
        r5 = 0x7d;
        fn_80239984();
        r0 = r3;
        r3 = r29;
        r30 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x7d;
        fn_80239EE8();
    }
    r3 = r30;
    return;
}
/* -------------------------------------------------------------------
 * Experience & Level Processing (0x8024D000-0x80254000)
 * 136 functions
 * ------------------------------------------------------------------- */

/* Address: 0x8024D818 | Size: 0x140 (320 bytes) */
u32 fightTrainerAiWazaValueAbareru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74();
    extern u16 fn_8023831C();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle = 0;
    u16 v;

    if (fn_80237F74(ctx, param1, 0x14) == 1) {
        handle = fn_80239984(0, ctx, 0x66);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x66);
    }
    v = fn_8023831C(ctx, param1);
    if (v == 8 || v == 9) {
        handle = fn_80239984(handle, ctx, 0x67);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x67);
    }
    handle = fn_80239984(handle, ctx, 0x68);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x68);
    return handle;
}
/* Address: 0x8024D958 | Size: 0x1A4 (420 bytes) */
u32 fightTrainerAiWazaValueKyuusyuu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802376EC();
    extern u32 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern s32 fn_8023C370(void*, u32, u32, u32, u32);
    u32 handle;
    s32 score;
    s32 scale;

    handle = fn_802376EC();
    score = fn_8023C370(ctx, param1, param2, param3, 1);
    scale = fightTrainerGetStatus(0, 0x63, 0x3e, 0);
    score = (((score / 2) * 100) / (u16)handle) / scale;
    {
        u32 nextHandle = fightTrainerAiAddValue(0, score);
        handle = nextHandle;
    }
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x63, score);
    if ((fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, param1) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x64);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x64);
    }
    if ((fn_80237F74(ctx, param3, 0x40) & 0xFF) == 1) {
        {
            u32 nextHandle = fn_80239984(handle, ctx, 0x65);
            handle = nextHandle;
        }
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x65);
    }
    return handle;
}
/* Address: 0x8024DAFC | Size: 0xC0 */
u32 fightTrainerAiWazaValueRokettoZutuki(void* ctx, u32 slot, u32 param) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x61);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x61);
    handle = fn_80239984(handle, ctx, 0x62);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x62);
    return handle;
}
/* Address: 0x8024DBBC | Size: 0xC0 */
u32 fightTrainerAiWazaValueKamaitati(void* ctx, u32 slot, u32 param) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x5f);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x5f);
    handle = fn_80239984(handle, ctx, 0x60);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x60);
    return handle;
}
/* Address: 0x8024DC7C | Size: 0x210 (528 bytes) */
u32 fightTrainerAiWazaValueKiaipanti(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[8];
    void* battleCtx;
    u32 trainer;
    u32 sequenceArg;
    u32 handle;
    u32 rawCount;
    u32* entriesPtr;
    u16 count;
    u32 index;

    battleCtx = ctx;
    trainer = param1;
    sequenceArg = param2;
    handle = 0;
    rawCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, battleCtx, entries, 0, 1);
    if ((fn_80236520(battleCtx, trainer) & 0xFFFF) == 0x117) {
        handle = fn_80239984(0, battleCtx, 0x5B);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5B);
    }

    entriesPtr = entries;
    count = rawCount;
    index = 0;
    while ((u16)index < count) {
        if (fn_80236BFC(battleCtx, entriesPtr[(u16)index], 8) == 1) {
            handle = fn_80239984(handle, battleCtx, 0x5C);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5C);
            break;
        }
        index++;
    }

    entriesPtr = entries;
    rawCount = rawCount & 0xFFFF;
    index = 0;
    while ((u16)index < rawCount) {
        if (fn_80236BFC(battleCtx, entriesPtr[(u16)index], 7) == 1) {
            handle = fn_80239984(handle, battleCtx, 0x5D);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5D);
            break;
        }
        index++;
    }

    handle = fn_80239984(handle, battleCtx, 0x5E);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5E);
    return handle;
}

#else

/* Address: 0x8024DE8C | Size: 0x138 (312 bytes) */
u32 fightTrainerAiWazaValueAteminage(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802357CC(void*, u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if (fn_802357CC(battleCtx, param3) > 6U) {
        handle = fn_80239984(0, battleCtx, 0x58);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x58);
    }
    if (fn_802358AC(battleCtx, trainer) < 6U) {
        handle = fn_80239984(handle, battleCtx, 0x59);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x59);
    }
    handle = fn_80239984(handle, battleCtx, 0x5a);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x5a);
    return handle;
}
/* Address: 0x8024DFC4 | Size: 0x108 (264 bytes) */
u32 fightTrainerAiWazaValueRibenji(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u8 fn_8023943C(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;
    u32 move = fn_80236520(battleCtx, param3);

    if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
        (move & 0xFFFF) != 0x163) {
        if (fn_8023943C(battleCtx, move, 1) == 1) {
            handle = fn_80239984(0, battleCtx, 0x56);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x56);
        }
    }
    handle = fn_80239984(handle, battleCtx, 0x57);
    fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x57);
    return handle;
}
/* Address: 0x8024E0CC | Size: 0x7C | Pattern: field_accessor */
u32 fightTrainerAiWazaValueTokubetuYuusen(void* ctx, u32 slot, u32 param) {
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    u32 handle;

    handle = fn_80239984(0, ctx, 0x55);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x55);
    return handle;
}
/* Address: 0x8024E148 | Size: 0xEC (236 bytes) */
u32 fightTrainerAiWazaValueKouPuraioritii(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x53);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x53);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(handle, ctx, 0x54);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x54);
    }
    return handle;
}
/* Address: 0x8024E234 | Size: 0xEC (236 bytes) */
u32 fn_8024E234(void* ctx, u32 slot, u32 param, u32 unused) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = fn_80239984(0, ctx, 0x51);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x51);
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(handle, ctx, 0x52);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x52);
    }
    return handle;
}
/* Address: 0x8024E320 | Size: 0x164 (356 bytes) */
u32 fightTrainerAiWazaValueHittyuu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802357CC(void*, u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32 handle = 0;

    if (fn_802357CC(battleCtx, param3) > 6U) {
        handle = fn_80239984(0, battleCtx, 0x4E);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x4E);
    }
    if (fn_802358AC(battleCtx, trainer) < 6U) {
        handle = fn_80239984(handle, battleCtx, 0x4F);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x4F);
    }
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, battleCtx, 1, 1, 0x10E, trainer) == 1) {
        handle = fn_80239984(handle, battleCtx, 0x50);
        fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x50);
    }
    return handle;
}
/* Address: 0x8024E484 | Size: 0xA8 */
u32 fightTrainerAiWazaValueTuujouIryoku(void* ctx, u32 slot, u32 param) {
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0x10e, slot) == 1) {
        handle = fn_80239984(0, ctx, 0x4d);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x4d);
    }
    return handle;
}
/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 120 functions matched
 * =================================================================== */

/* Address: 0x8024E52C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaValueNull(void) { return 0; }
/* Address: 0x8024E534 | Size: 0x44 | Pattern: field_accessor */
u32 fn_8024E534(void* ctx, u32 slot, u32 param) {
    extern u32 fightTrainerGetStatus();
    fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return 0;
}

#endif
