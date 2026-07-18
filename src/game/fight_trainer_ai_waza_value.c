/**
 * @file fight_trainer_ai_waza_value.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x8024A170 - 0x8024A664.
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

/* Address: 0x8024A170 | Size: 0x2B8 (696 bytes) */
void fightTrainerAiWazaValueKogoerukaze(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80236D60();
    extern void fn_80237F74();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r7 = 0x1;
    r24 = r3;
    r25 = r4;
    r26 = r5;
    r27 = r6;
    r4 = r24;
    r5 = (u32)sp + 0x28;
    r29 = 0x0;
    r3 = 0x0;
    r6 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r30 = r3;
    r4 = r24;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r31 = (u32)sp + 0x8;
    r23 = r3 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r19 = *(u32*)(r31 + r0);
        if (r19 != (u32)0x0) {
            r21 = (u32)sp + 0x28;
            r22 = r30 & 0xFFFF;
            r20 = 0x0;
            while (1) {
                r0 = r20 & 0xFFFF;
            if (r0 >= (u32)r22) break;
                r5 = *(u32*)(r21 + r0);
                if (r5 != (u32)0x0) {
                    r3 = r24;
                    r4 = r19;
                    fn_80236D60();
                    if ((s32)r3 > (s32)0x0) {
                        r3 = r29;
                        r4 = r24;
                        r5 = 0xcf;
                        fn_80239984();
                        r0 = r3;
                        r3 = r25;
                        r29 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r24;
                        r8 = r26;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0xcf;
                        fn_80239EE8();
                        break;
                }
                }
                r20 = r20 + 0x1;

            }
        }
        r28 = r28 + 0x1;

    }
    if (r23 >= (u32)0x2) {
        r3 = r29;
        r4 = r24;
        r5 = 0xd0;
        fn_80239984();
        r0 = r3;
        r3 = r25;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r24;
        r8 = r26;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd0;
        fn_80239EE8();
    }
    r31 = (u32)sp + 0x28;
    r28 = r30 & 0xFFFF;
    r21 = 0x0;
    while (1) {
        r0 = r21 & 0xFFFF;
        if (r0 >= (u32)r28) break;
        r5 = *(u32*)(r31 + r0);
        if (r5 != (u32)0x0) {
            r3 = r24;
            r4 = r27;
            fn_80236D60();
            if ((s32)r3 < (s32)0x0) break;
        }
        r21 = r21 + 0x1;

    }

    r3 = r21 & 0xFFFF;
    r0 = r30 & 0xFFFF;
    if (r3 < r0) {
        r3 = r29;
        r4 = r24;
        r5 = 0xd1;
        fn_80239984();
        r0 = r3;
        r3 = r25;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r24;
        r8 = r26;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xd1;
        fn_80239EE8();
    }
    r3 = r24;
    r4 = r27;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r24;
        r4 = r27;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r24;
            r4 = r27;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r29; return; }
    }
    }
    r3 = r29;
    r4 = r24;
    r5 = 0xd2;
    fn_80239984();
    r0 = r3;
    r3 = r25;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r24;
    r8 = r26;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xd2;
    fn_80239EE8();

    r3 = r29;
    return;
}
/* Address: 0x8024A428 | Size: 0x23C (572 bytes) */
void fightTrainerAiWazaValueKanarazuSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235910();
    extern void fn_80236D60();
    extern void fn_80237F74();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x80];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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

    r7 = 0x1;
    r24 = r3;
    r25 = r4;
    r26 = r5;
    r27 = r6;
    r4 = r24;
    r5 = (u32)sp + 0x28;
    r29 = 0x0;
    r3 = 0x0;
    r6 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r30 = r3;
    r4 = r24;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r31 = (u32)sp + 0x8;
    r23 = r3 & 0xFFFF;
    r28 = 0x0;
    while (1) {
        r0 = r28 & 0xFFFF;
        if (r0 >= (u32)r23) break;
        r19 = *(u32*)(r31 + r0);
        if (r19 != (u32)0x0) {
            r21 = (u32)sp + 0x28;
            r22 = r30 & 0xFFFF;
            r18 = 0x0;
            r20 = 0x0;
            while (1) {
                r0 = r20 & 0xFFFF;
                if (r0 >= (u32)r22) break;
                r5 = *(u32*)(r21 + r0);
                if (r5 != (u32)0x0) {
                    r3 = r24;
                    r4 = r19;
                    fn_80236D60();
                    if ((s32)r3 > (s32)0x0) {
                        r3 = r29;
                        r4 = r24;
                        r5 = 0xcc;
                        fn_80239984();
                        r0 = r3;
                        r3 = r25;
                        r29 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r24;
                        r8 = r26;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0xcc;
                        fn_80239EE8();
                        r18 = 0x1;
                        break;
                }
                }
                r20 = r20 + 0x1;

            }

            r0 = r18 & 0xFF;
            if (r0 == (u32)0x1) break;
        }
        r28 = r28 + 0x1;

    }

    r3 = r24;
    r4 = r27;
    fn_80235910();
    r0 = r3 & 0xFF;
    if (r0 == (u32)r23) {
        r3 = r29;
        r4 = r24;
        r5 = 0xcd;
        fn_80239984();
        r0 = r3;
        r3 = r25;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r24;
        r8 = r26;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0xcd;
        fn_80239EE8();
    }
    r3 = r24;
    r4 = r27;
    r5 = 0x1d;
    fn_80237F74();
    r0 = r3 & 0xFF;
    if (r0 != (u32)0x1) {
        r3 = r24;
        r4 = r27;
        r5 = 0x13;
        fn_80237F74();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r3 = r24;
            r4 = r27;
            r5 = 0x49;
            fn_80237F74();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) { r3 = r29; return; }
    }
    }
    r3 = r29;
    r4 = r24;
    r5 = 0xce;
    fn_80239984();
    r0 = r3;
    r3 = r25;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r24;
    r8 = r26;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0xce;
    fn_80239EE8();

    r3 = r29;
    return;
}
