/**
 * @file fight_trainer_ai_waza_value_middle.c
 * @brief Candidate fightTrainerAiWazaValue.cpp range, 0x80243CD8 - 0x802451C0.
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

#if !defined(FIGHT_AI_VALUE_MIDDLE_EXACT_80244318) && \
    !defined(FIGHT_AI_VALUE_MIDDLE_SUFFIX_8024498C)

/* Address: 0x80243CD8 | Size: 0x640 (1600 bytes) */
void fightTrainerAiWazaValueKoraeru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern f32 lbl_8047E630;
    extern void wazaGetStatus();
    extern void fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235B04();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_80236BFC();
    extern void fn_802373B0();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80237CB8();
    extern void fn_8023831C();
    extern void fn_80239500();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fightTrainerAiWazaValueJisin();
    extern void fightTrainerAiWazaValueJibaku();
    extern void fightTrainerAiWazaValueNull();
    u8 sp[0x90];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
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
    f32 f1 = 0.0f;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r30 = r4;
    r28 = r5;
    r31 = r3;
    r16 = r6;
    r29 = 0x0;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r25 = r3;
    r4 = r31;
    r5 = (u32)sp + 0x20;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r15 = r3;
    r3 = r31;
    r4 = r16;
    r5 = (u32)sp + 0x8;
    fn_80237CB8();
    r16 = r3;
    r3 = r31;
    r4 = r30;
    fn_80236520();
    r27 = r3;
    r3 = r31;
    r4 = r30;
    fn_8023831C();
    r26 = r3;
    r17 = (u32)sp + 0x8;
    r16 = r16 & 0xFFFF;
    r18 = 0x0;
    r19 = 0x0;
    while (1) {
        r0 = r19 & 0xFFFF;
        if (r0 >= (u32)r16) break;
        r3 = r31;
        r4 = r28;
        fn_80239500();
        r6 = r3;
        r5 = *(u16*)(r17 + r0);
        r3 = r31;
        r4 = r30;
        fn_8023793C();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x41) {
            r18 = 0x1;
            break;
        }
        r19 = r19 + 0x1;

    }

    r0 = r18 & 0xFF;
    if (r0 == (u32)0x1) {
        r4 = r31;
        r3 = 0x0;
        r5 = 0x16a;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x16a;
        fn_80239EE8();
    }
    f1 = lbl_8047E630;
    r3 = r31;
    r4 = r30;
    r5 = -0x1;
    fn_802373B0();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r31;
        r5 = 0x16b;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x16b;
        fn_80239EE8();
    }
    r16 = (u32)sp + 0x20;
    r21 = r15 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r21) break;
        r4 = *(u32*)(r16 + r0);
        if (r30 != (u32)r4) {
            r3 = r31;
            r5 = (u32)sp + 0xc;
            r6 = 0x0;
            r7 = 0x1;
            fn_802367CC();
            r17 = r3 & 0xFFFF;
            r24 = r3;
            if (r30 != (u32)r4) {
                r4 = (u32)fightTrainerAiWazaValueJisin;
                r3 = (u32)fightTrainerAiWazaValueJibaku;
                r5 = (u32)fightTrainerAiWazaValueNull;
                r15 = (u32)sp + 0xc;
                r19 = (u32)fightTrainerAiWazaValueJisin;
                r20 = (u32)fightTrainerAiWazaValueJibaku;
                r18 = (u32)fightTrainerAiWazaValueNull;
                r22 = 0x0;
                while (1) {
                    r0 = r22 & 0xFFFF;
                    if (r0 >= (u32)r17) break;
                    r3 = 0x0;
                    r4 = *(u16*)(r15 + r0);
                    r5 = 0x1c;
                    r6 = 0x0;
                    wazaGetStatus();
                    if (r3 == (u32)0x0) {
                        r3 = r18;
                    }

                    if (r3 == (u32)r19 || r3 == (u32)r20) {

                        r3 = r29;
                        r4 = r31;
                        r5 = 0x16c;
                        fn_80239984();
                        r0 = r3;
                        r3 = r30;
                        r29 = r0;
                        fightOutPokemonGetPokemonPtr();
                        r6 = (0x1 << 16);
                        r5 = r3;
                        r4 = r31;
                        r8 = r28;
                        r6 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x16c;
                        fn_80239EE8();
                        break;
                    }
                    r22 = r22 + 0x1;

                }

                r3 = r22 & 0xFFFF;
                r0 = r24 & 0xFFFF;
                if (r3 < r0) break;
        }
        }
        r23 = r23 + 0x1;

    }

    r3 = (u32)fightTrainerAiWazaValueJisin;
    r15 = 0x0;
    r16 = (u32)fightTrainerAiWazaValueJisin;
    while (1) {
        r3 = lbl_80478DF8;
        r4 = r15 & 0xFFFF;
        r0 = *(u32*)((u8*)r3 + 0x0);
        if (r4 >= (u32)r0) break;
        r4 = r15;
        r3 = 0x0;
        r5 = 0x1c;
        r6 = 0x0;
        wazaGetStatus();
        if (r3 == (u32)0x0) {
            r3 = (u32)fightTrainerAiWazaValueNull;
            r3 = (u32)fightTrainerAiWazaValueNull;
        }
        if (r3 != (u32)r16) {
            r4 = (u32)fightTrainerAiWazaValueJibaku;
            r0 = (u32)fightTrainerAiWazaValueJibaku;
            if (r3 == (u32)r0) {
            }
            r4 = r31;
            r7 = r15;
            r3 = 0x0;
            r5 = 0x1;
            r6 = 0x1;
            r8 = 0x0;
            fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r29;
                r4 = r31;
                r5 = 0x16d;
                fn_80239984();
                r0 = r3;
                r3 = r30;
                r29 = r0;
                fightOutPokemonGetPokemonPtr();
                r6 = (0x1 << 16);
                r5 = r3;
                r4 = r31;
                r8 = r28;
                r6 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x16d;
                fn_80239EE8();
                break;
            }
            }
        r15 = r15 + 0x1;

    }

    r3 = r31;
    r4 = r30;
    r15 = 0x0;
    r5 = 0x6;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x3;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x4;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x5;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x18;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r3 = r31;
    r4 = r30;
    r5 = 0x1c;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r15 = 0x1;
    }
    r0 = r15 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r31;
        r5 = 0x16e;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x16e;
        fn_80239EE8();
    }
    r0 = r27 & 0xFFFF;
    if (r0 == (u32)0xcb) {
        r3 = r30;
        r4 = 0x0;
        r5 = 0xfc;
        r6 = 0x0;
        ((void(*)(void))pokemonGetStatus)();
        if ((s32)r3 != (s32)0x0) {
            r3 = r29;
            r4 = r31;
            r5 = 0x16f;
            fn_80239984();
            r0 = r3;
            r3 = r30;
            r29 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r31;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x16f;
            fn_80239EE8();
    }
    }
    r0 = r26 & 0xFFFF;
    if ((r0 != (u32)0x11) && (r0 != (u32)0xf)) {

        r3 = r29;
        r4 = r31;
        r5 = 0x170;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x170;
        fn_80239EE8();
    }
    r3 = r31;
    r4 = r30;
    fn_802376EC();
    r0 = r3 & 0xFFFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r31;
        r5 = 0x171;
        fn_80239984();
        r0 = r3;
        r3 = r30;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r31;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x171;
        fn_80239EE8();
    }
    r0 = r25 & 0xFF;
    if (r0 != (u32)0x4) {
        if (r0 != (u32)0x3) { r3 = r29; return; }
    }
    r3 = r29;
    r4 = r31;
    r5 = 0x172;
    fn_80239984();
    r0 = r3;
    r3 = r30;
    r29 = r0;
    fightOutPokemonGetPokemonPtr();
    r6 = (0x1 << 16);
    r5 = r3;
    r4 = r31;
    r8 = r28;
    r6 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x172;
    fn_80239EE8();

    r3 = r29;
    return;
}

#endif

#if defined(FIGHT_AI_VALUE_MIDDLE_EXACT_80244318)

/* Address: 0x80244318 | Size: 0x160 (352 bytes) */
u32 fightTrainerAiWazaValueMiyaburu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802357CC(void*, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_80237DBC(ctx, param3, 7) == 1) {
        handle = fn_80239984(0, ctx, 0x167);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x167);
    }
    if ((fn_802357CC(ctx, param3) & 0xff) >= 8) {
        handle = fn_80239984(handle, ctx, 0x168);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x168);
    }
    if (fn_80236BFC(ctx, param3, 0x19) == 1) {
        handle = fn_80239984(handle, ctx, 0x169);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x169);
    }
    return handle;
}
/* Address: 0x80244478 | Size: 0x9C */
u32 fightTrainerAiWazaValueMajikkukooto(void* ctx, u32 param1, u32 param2) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((pokemonGetStatus(param1, 0, 0xed, 0) & 0xFFFF) != 0) {
        handle = fn_80239984(0, ctx, 0x166);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x166);
    }
    return handle;
}
static inline u16 lowHalf(u32 value) {
    return (u16)value;
}
/* Address: 0x80244514 | Size: 0x18C (396 bytes) */
u32 fightTrainerAiWazaValueMiraakooto(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_8010C4A0(u32);
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u32 fn_802395C8(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[8];
    u32 rawCount;
    u32 count;
    u32* entriesPtr;
    u32 move;
    u32 index;
    u32 current;
    u32 handle;

    handle = 0;
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 0, 1);
    rawCount = count;
    move = fn_80236520(ctx, param3);
    entriesPtr = entries;
    count = rawCount & 0xffff;
    index = 0;
    while (lowHalf(index) < count) {
        current = fn_8023715C(ctx, entriesPtr[lowHalf(index)]);
        if (lowHalf(current) < lowHalf(fn_80236FFC(ctx, entriesPtr[lowHalf(index)]))) {
            handle = fn_80239984(0, ctx, 0x164);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x164);
            break;
        }
        index++;
    }
    if (lowHalf(move) != 0 && lowHalf(move) != 0xffff && lowHalf(move) != 0x165 && lowHalf(move) != 0x163) {
        if ((u8)fn_8010C4A0(fn_802395C8(ctx, move, param3)) == 2) {
            handle = fn_80239984(handle, ctx, 0x165);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x165);
        }
    }
    return handle;
}
/* Address: 0x802446A0 | Size: 0x18C (396 bytes) */
u32 fightTrainerAiWazaValueKauntaa(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_8010C4A0(u32);
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u32 fn_802395C8(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[8];
    u32 rawCount;
    u32 count;
    u32* entriesPtr;
    u32 move;
    u32 index;
    u32 current;
    u32 handle;

    handle = 0;
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 0, 1);
    rawCount = count;
    move = fn_80236520(ctx, param3);
    entriesPtr = entries;
    count = rawCount & 0xffff;
    index = 0;
    while (lowHalf(index) < count) {
        current = fn_8023715C(ctx, entriesPtr[lowHalf(index)]);
        if (lowHalf(current) > lowHalf(fn_80236FFC(ctx, entriesPtr[lowHalf(index)]))) {
            handle = fn_80239984(0, ctx, 0x162);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x162);
            break;
        }
        index++;
    }
    if (lowHalf(move) != 0 && lowHalf(move) != 0xffff && lowHalf(move) != 0x165 && lowHalf(move) != 0x163) {
        if ((u8)fn_8010C4A0(fn_802395C8(ctx, move, param3)) == 1) {
            handle = fn_80239984(handle, ctx, 0x163);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x163);
        }
    }
    return handle;
}
/* Address: 0x8024482C | Size: 0xD4 (212 bytes) */
u32 fightTrainerAiWazaValueAroma(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80238748(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 list[23];
    void* battleCtx = ctx;
    u32 trainer = param1;
    u32 sequenceArg = param2;
    u32* listPtr;
    u32 handle = 0;
    u32 i;
    u32 count;

    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, battleCtx, list, 1, 1);
    listPtr = list;
    count &= 0xFFFF;
    for (i = 0; (u16)i < count; i++) {
        if (fn_80238748(battleCtx, listPtr[(u16)i]) == 0) {
            handle = fn_80239984(0, battleCtx, 0x161);
            fn_80239EE8(0xEC64, battleCtx, fightOutPokemonGetPokemonPtr(trainer), 0, 0, sequenceArg, 0, 0x161);
            break;
        }
    }
    return handle;
}
/* Address: 0x80244900 | Size: 0x8C */
u32 fightTrainerAiWazaValueRihuressyu(void* ctx, u32 param1, u32 param2) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void* ctx);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80237310(ctx) == 0) {
        u32 tmp = fn_80239984(0, ctx, 0x160);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x160);
    }
    return handle;
}

#endif

#if defined(FIGHT_AI_VALUE_MIDDLE_SUFFIX_8024498C)

/* Address: 0x8024498C | Size: 0x318 (792 bytes) */
void fightTrainerAiWazaValueSunaarasi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235B04();
    extern void fn_802377E8();
    extern void fn_80238980();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0xC0];
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

    r26 = r4;
    r27 = r5;
    r25 = r3;
    r28 = 0x0;
    r4 = 0x0;
    r5 = 0x0;
    fn_80235B04();
    r29 = r3;
    r4 = r25;
    r5 = (u32)sp + 0x28;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r30 = r3;
    r4 = r25;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x0;
    r7 = 0x1;
    fightFloorGetFightTrainerFightOutPokemonPtrAry();
    r0 = r29 & 0xFF;
    r31 = r3;
    if (r0 != (u32)0x3) {
        r4 = r25;
        r3 = 0x0;
        r5 = 0x15b;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r25;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x15b;
        fn_80239EE8();
    }
    r22 = (u32)sp + 0x28;
    r24 = r30 & 0xFFFF;
    r21 = 0x0;
    while (1) {
        r0 = r21 & 0xFFFF;
        if (r0 >= (u32)r24) break;
        r3 = r25;
        r4 = *(u32*)(r22 + r23);
        r5 = 0x5;
        fn_80238E30();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r4 = *(u32*)(r22 + r23);
            r3 = r25;
            r5 = 0x4;
            fn_80238E30();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r4 = *(u32*)(r22 + r23);
                r3 = r25;
                r5 = 0x8;
                fn_80238E30();
                r0 = r3 & 0xFF;
                if (r0 != (u32)0x1) {
                    r4 = *(u32*)(r22 + r23);
                    r3 = r25;
                    r5 = 0x8;
                    fn_80239058();
                    r0 = r3 & 0xFF;
                    if (r0 == (u32)0x1) {
            }
            }
            }
            r3 = r28;
            r4 = r25;
            r5 = 0x15c;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x15c;
            fn_80239EE8();
            break;
                    }
        r21 = r21 + 0x1;

    }

    r24 = (u32)sp + 0x8;
    r31 = r31 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r25;
        r4 = *(u32*)(r24 + r0);
        fn_802377E8();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x12f) {
            r3 = r28;
            r4 = r25;
            r5 = 0x15d;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x15d;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r31 = (u32)sp + 0x28;
    r30 = r30 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r30) break;
        r3 = r25;
        r4 = *(u32*)(r31 + r0);
        fn_80238980();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x181) {
            r3 = r28;
            r4 = r25;
            r5 = 0x15e;
            fn_80239984();
            r0 = r3;
            r3 = r26;
            r28 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r25;
            r8 = r27;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x15e;
            fn_80239EE8();
            break;
        }
        r22 = r22 + 0x1;

    }

    r0 = r29 & 0xFF;
    if (r0 == (u32)0x3) {
        r3 = r28;
        r4 = r25;
        r5 = 0x15f;
        fn_80239984();
        r0 = r3;
        r3 = r26;
        r28 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r25;
        r8 = r27;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x15f;
        fn_80239EE8();
    }
    r3 = r28;
    return;
}
/* Address: 0x80244CA4 | Size: 0x2C4 (708 bytes) */
u32 fightTrainerAiWazaValueArare(void* ctx, u32 param1, u32 param2, u32 param3) {
    u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    u16 fn_802377E8(void*, u32);
    u32 pokemon[23];
    u32 outPokemon[8];
    u32 handle;
    u8 weather;
    u16 pokemonCount;
    u16 outPokemonCount;
    u16 i;

    handle = 0;
    weather = fn_80235B04(ctx, 0, 0);
    pokemonCount = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 1, 1);
    outPokemonCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, outPokemon, 0, 1);

    if (weather != 4) {
        handle = fn_80239984(0, ctx, 0x156);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x156);
    }

    for (i = 0; i < pokemonCount; i++) {
        if (fn_80238E30(ctx, pokemon[i], 0xf) == 1) {
            handle = fn_80239984(handle, ctx, 0x157);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x157);
            break;
        }
    }

    for (i = 0; i < outPokemonCount; i++) {
        if (fn_802377E8(ctx, outPokemon[i]) == 0x12f) {
            handle = fn_80239984(handle, ctx, 0x158);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x158);
            break;
        }
    }

    for (i = 0; i < pokemonCount; i++) {
        if (fn_80238980(ctx, pokemon[i]) == 0x181) {
            handle = fn_80239984(handle, ctx, 0x159);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x159);
            break;
        }
    }

    if (weather == 4) {
        handle = fn_80239984(handle, ctx, 0x15a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x15a);
    }
    return handle;
}
/* Address: 0x80244F68 | Size: 0x258 (600 bytes) */
void fightTrainerAiWazaValueNihonbare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fightFloorGetFightTrainerFightPokemonPtrAry();
    extern void fightOutPokemonGetPokemonPtr();
    extern void fn_80235B04();
    extern void fn_80238980();
    extern void fn_80238E30();
    extern void fn_80239058();
    extern void fn_80239984();
    extern void fn_80239EE8();
    u8 sp[0x90];
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
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r27 = r4;
    r28 = r5;
    r26 = r3;
    r29 = 0x0;
    r4 = 0x0;
    r5 = 0x0;
    fn_80235B04();
    r30 = r3;
    r4 = r26;
    r5 = (u32)sp + 0x8;
    r3 = 0x0;
    r6 = 0x1;
    r7 = 0x1;
    fightFloorGetFightTrainerFightPokemonPtrAry();
    r0 = r30 & 0xFF;
    r31 = r3;
    if (r0 != (u32)0x1) {
        r4 = r26;
        r3 = 0x0;
        r5 = 0x152;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x152;
        fn_80239EE8();
    }
    r23 = (u32)sp + 0x8;
    r25 = r31 & 0xFFFF;
    r22 = 0x0;
    while (1) {
        r0 = r22 & 0xFFFF;
        if (r0 >= (u32)r25) break;
        r3 = r26;
        r4 = *(u32*)(r23 + r24);
        r5 = 0xa;
        fn_80238E30();
        r0 = r3 & 0xFF;
        if (r0 != (u32)0x1) {
            r4 = *(u32*)(r23 + r24);
            r3 = r26;
            r5 = 0xc;
            fn_80238E30();
            r0 = r3 & 0xFF;
            if (r0 != (u32)0x1) {
                r4 = *(u32*)(r23 + r24);
                r3 = r26;
                r5 = 0x22;
                fn_80239058();
                r0 = r3 & 0xFF;
                if (r0 == (u32)0x1) {
            }
            }
            r3 = r29;
            r4 = r26;
            r5 = 0x153;
            fn_80239984();
            r0 = r3;
            r3 = r27;
            r29 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r26;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x153;
            fn_80239EE8();
            break;
                }
        r22 = r22 + 0x1;

    }

    r25 = (u32)sp + 0x8;
    r31 = r31 & 0xFFFF;
    r23 = 0x0;
    while (1) {
        r0 = r23 & 0xFFFF;
        if (r0 >= (u32)r31) break;
        r3 = r26;
        r4 = *(u32*)(r25 + r0);
        fn_80238980();
        r0 = r3 & 0xFFFF;
        if (r0 == (u32)0x181) {
            r3 = r29;
            r4 = r26;
            r5 = 0x154;
            fn_80239984();
            r0 = r3;
            r3 = r27;
            r29 = r0;
            fightOutPokemonGetPokemonPtr();
            r6 = (0x1 << 16);
            r5 = r3;
            r4 = r26;
            r8 = r28;
            r6 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x154;
            fn_80239EE8();
            break;
        }
        r23 = r23 + 0x1;

    }

    r0 = r30 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r29;
        r4 = r26;
        r5 = 0x155;
        fn_80239984();
        r0 = r3;
        r3 = r27;
        r29 = r0;
        fightOutPokemonGetPokemonPtr();
        r6 = (0x1 << 16);
        r5 = r3;
        r4 = r26;
        r8 = r28;
        r6 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x155;
        fn_80239EE8();
    }
    r3 = r29;
    return;
}

#endif
