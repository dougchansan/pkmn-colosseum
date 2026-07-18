/**
 * @file fight_trainer_ai_waza_value_exact_80241B70.c
 * @brief Byte-exact fightTrainerAiWazaValue.cpp range, 0x80241B70 - 0x80243CD8.
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

/* Address: 0x80241B70 | Size: 0x278 (632 bytes) */
u32 fightTrainerAiWazaValueKyouseikoutai(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern u32 fightSideGetCountAsJoutaiDataId(u32, u32);
    extern u8 fightSideIsJoutaiDataId(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802357CC(void*, u32);
    extern u8 fn_802358AC(void*, u32);
    extern u8 fn_80235910(void*, u32);
    extern u8 fn_80235974(void*, u32);
    extern u8 fn_802359D8(void*, u32);
    extern u8 fn_80235A3C(void*, u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u8 stats[7];
    u32 handle;
    u32 mode;
    u8 found;
    u8 i;

    handle = 0;
    mode = fightTargetGetPtrAsNowFightType(2, param3);
    if (fightSideIsJoutaiDataId(mode, 0x4a) == 1) {
        mode = fightSideGetCountAsJoutaiDataId(mode, 0x4a);
    } else {
        mode = 0;
    }
    stats[0] = fn_80235AA0(ctx, param3);
    stats[1] = fn_80235A3C(ctx, param3);
    stats[2] = fn_802359D8(ctx, param3);
    stats[3] = fn_80235974(ctx, param3);
    stats[4] = fn_80235910(ctx, param3);
    stats[5] = fn_802358AC(ctx, param3);
    stats[6] = fn_802357CC(ctx, param3);

    i = 0;
    goto check_stats;
check_stat_value:
    if (stats[i] >= 8 && stats[i] <= 0xc) {
        found = 1;
        goto done_stats;
    }
    i++;
check_stats:
    if (i < 7) {
        goto check_stat_value;
    }
    found = 0;
done_stats:
    if (found == 1) {
        handle = fn_80239984(0, ctx, 0x1ac);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ac);
    }
    if ((s16)mode == 1) {
        handle = fn_80239984(handle, ctx, 0x1ad);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ad);
    } else if ((s16)mode == 2) {
        handle = fn_80239984(handle, ctx, 0x1ae);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ae);
    } else if ((s16)mode == 3) {
        handle = fn_80239984(handle, ctx, 0x1af);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1af);
    }
    return handle;
}
/* Address: 0x80241DE8 | Size: 0x1FC (508 bytes) */
s32 fightTrainerAiWazaValueYokodori(void* ctx, void* param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u16 fn_802364BC();
    extern u32 fn_80236520();
    extern u16 fn_802377E8();
    extern u8 fn_8023943C();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    u32 buf[8];
    s32 handle = 0;
    u32 r1v;
    u16 r2v;
    u16 count;
    u16 i;
    u32 r3v;

    r1v = fn_80236520(ctx, param1);
    r2v = fn_802364BC(ctx, param1);
    r3v = fn_80236520(ctx, param3);
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf, 0, 1);

    if ((u16)r3v != 0 && (u16)r3v != 0xffff && (u16)r3v != 0x165 && (u16)r3v != 0x163 &&
        fn_8023943C(ctx, r3v, 4) == 1) {
        handle = (s32)fn_80239984(0, ctx, 0x1a9);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a9);
    }

    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, buf[i]);
        if (v == 0x12e || v == 0xd4 || v == 0x177) {
            handle = (s32)fn_80239984(handle, ctx, 0x1aa);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1aa);
            break;
        }
    }

    if ((u16)r1v == 0x121 || r2v == 0x121) {
        handle = (s32)fn_80239984(handle, ctx, 0x1ab);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ab);
    }
    return handle;
}
/* Address: 0x80241FE4 | Size: 0x2A8 (680 bytes) */
s32 fightTrainerAiWazaValueKonoyubitomare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u16 fn_802377E8();
    extern u8 fn_8023785C();
    extern u16 fn_8023793C();
    extern u16 fn_80237CB8();
    extern s32 fn_80239500();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    u32 listA[8];
    u32 listB[8];
    u16 listC[2];
    s32 handle = 0;
    u16 countA;
    u16 countB;
    u16 countC;
    u8 found;
    u16 i;
    u16 j;
    u16 k;

    countA = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, listA, 1, 1);
    countB = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, listB, 0, 1);

    found = 0;
    for (i = 0; i < countB; i++) {
        countC = fn_80237CB8(ctx, listB[i], listC);
        for (j = 0; j < countC; j++) {
            for (k = 0; k < countA; k++) {
                if (param1 != listA[k] &&
                    fn_8023793C(ctx, listA[k], listC[j], fn_80239500(ctx, param2)) == 0x41) {
                    found = 1;
                }
            }
        }
    }

    if (found == 1) {
        handle = (s32)fn_80239984(0, ctx, 0x1A6);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A6);
    }

    for (i = 0; i < countA; i++) {
        if (param1 != listA[i] && fn_8023785C(ctx, listA[i]) == 2) {
            handle = (s32)fn_80239984(handle, ctx, 0x1A7);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A7);
            break;
        }
    }

    for (i = 0; i < countB; i++) {
        u16 v = fn_802377E8(ctx, listB[i]);
        if (v == 0x12E || v == 0xD4 || v == 0x177) {
            handle = (s32)fn_80239984(handle, ctx, 0x1A8);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A8);
            break;
        }
    }

    return handle;
}
/* Address: 0x8024228C | Size: 0x3BC (956 bytes) */
u32 fightTrainerAiWazaValueKoukanKinsi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(0, ctx, 0x19d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19d);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x19e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19e);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x19f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19f);
    }
    if ((u8)fn_80236BFC(ctx, param3, 5) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a0);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a1);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a2);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a3);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a3);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a4);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x1a5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a5);
    }
    return handle;
}
/* Address: 0x80242648 | Size: 0xE8 (232 bytes) */
u32 fightTrainerAiWazaValueKanasibari(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u8 fn_8023943C(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;

    handle = 0;
    move = fn_80236520(ctx, param3);
    if (fn_80236D60(ctx, param1, param3) > 0) {
        if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
            (move & 0xFFFF) != 0x163) {
            if (fn_8023943C(ctx, move, 1) == 1) {
                handle = fn_80239984(0, ctx, 0x19c);
                fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19c);
            }
        }
    }
    return handle;
}
/* Address: 0x80242730 | Size: 0x170 (368 bytes) */
u32 fightTrainerAiWazaValueMitizure(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern const f32 lbl_8047E630;
    extern const f32 lbl_8047E638;
    extern const f32 lbl_8047E63C;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_802373B0(ctx, param1, -1, lbl_8047E638) == 1) {
        handle = fn_80239984(0, ctx, 0x19b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19b);
    } else if (fn_802373B0(ctx, param1, -1, lbl_8047E63C) == 1) {
        handle = fn_80239984(0, ctx, 0x19a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19a);
    } else if (fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x199);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x199);
    }
    return handle;
}
/* Address: 0x802428A0 | Size: 0x134 (308 bytes) */
u32 fightTrainerAiWazaValueUrami(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236520(void*, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u8 fn_802391E0(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;
    u32 value;

    handle = 0;
    move = fn_80236520(ctx, param3);
    if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
        (move & 0xFFFF) != 0x163 && fn_80236D60(ctx, param1, param3) > 0) {
        value = fn_802391E0(ctx, move);
        if ((value & 0xFF) <= 5) {
            handle = fn_80239984(0, ctx, 0x197);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x197);
        } else if ((value & 0xFF) <= 0xa) {
            handle = fn_80239984(0, ctx, 0x198);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x198);
        }
    }
    return handle;
}
/* Address: 0x802429D4 | Size: 0x17C (380 bytes) */
u32 fightTrainerAiWazaValueOdareru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_802370AC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 statLimit;
    u32 statCurrent;
    u32 statNext;
    u32 handle;

    handle = 0;
    statLimit = fn_80236FFC(ctx, param3);
    statCurrent = fn_8023715C(ctx, param3);
    statNext = fn_802370AC(ctx, param3);
    if ((u16)statCurrent > (u16)statLimit) {
        handle = fn_80239984(0, ctx, 0x194);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x194);
    }
    if ((u16)statCurrent > (u16)statNext) {
        handle = fn_80239984(handle, ctx, 0x195);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x195);
    }
    if (fn_80237F74(ctx, param3, 0x14) == 1) {
        handle = fn_80239984(handle, ctx, 0x196);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x196);
    }
    return handle;
}
/* Address: 0x80242B50 | Size: 0x17C (380 bytes) */
u32 fightTrainerAiWazaValueIbaru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_802370AC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 statLimit;
    u32 statCurrent;
    u32 statNext;
    u32 handle;

    handle = 0;
    statLimit = fn_80236FFC(ctx, param3);
    statCurrent = fn_8023715C(ctx, param3);
    statNext = fn_802370AC(ctx, param3);
    if ((u16)statLimit > (u16)statCurrent) {
        handle = fn_80239984(0, ctx, 0x191);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x191);
    }
    if ((u16)statCurrent > (u16)statNext) {
        handle = fn_80239984(handle, ctx, 0x192);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x192);
    }
    if (fn_80237F74(ctx, param3, 0x14) == 1) {
        handle = fn_80239984(handle, ctx, 0x193);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x193);
    }
    return handle;
}
/* Address: 0x80242CCC | Size: 0xE4 (228 bytes) */
u32 fightTrainerAiWazaValueAnkooru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802364BC(void*, u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u8 fn_8023943C(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;

    handle = 0;
    move = fn_802364BC(ctx, param3);
    if (fn_80236D60(ctx, param1, param3) > 0) {
        if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
            (move & 0xFFFF) != 0x163) {
            if (fn_8023943C(ctx, move, 1) == 0) {
                handle = fn_80239984(0, ctx, 0x190);
                fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x190);
            }
        }
    }
    return handle;
}
/* Address: 0x80242DB0 | Size: 0x9C */
u32 fightTrainerAiWazaValueIkarinomaeba(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern const f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_802373B0(ctx, extra, 1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x18f);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18f);
    }
    return handle;
}
/* Address: 0x80242E4C | Size: 0x1A0 (416 bytes) */
u32 fightTrainerAiWazaValueKusuguruDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80235A3C(void*, u32);
    extern u8 fn_80235AA0(void*, u32);
    extern u32 fn_80236F4C(void*, u32);
    extern u32 fn_80236FFC(void*, u32);
    extern u32 fn_802370AC(void*, u32);
    extern u32 fn_8023715C(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 currentAtk;
    u32 limitAtk;
    u32 currentDef;
    u32 limitDef;
    u32 handle;

    handle = 0;
    currentAtk = fn_8023715C(ctx, param3);
    limitAtk = fn_80236FFC(ctx, param3);
    currentDef = fn_802370AC(ctx, param3);
    limitDef = fn_80236F4C(ctx, param3);
    if ((u16)currentAtk > (u16)limitAtk) {
        handle = fn_80239984(0, ctx, 0x18c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18c);
    }
    if ((u16)currentDef > (u16)limitDef) {
        handle = fn_80239984(handle, ctx, 0x18d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18d);
    }
    if (fn_80235AA0(ctx, param3) <= 4 && fn_80235A3C(ctx, param3) <= 4) {
        handle = fn_80239984(handle, ctx, 0x18e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18e);
    }
    return handle;
}
/* Address: 0x80242FEC | Size: 0xF8 (248 bytes) */
u32 fightTrainerAiWazaValueKaihiDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_802357CC();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle = 0;

    if ((u8)fn_802357CC(ctx, param3) >= 7) {
        handle = fn_80239984(0, ctx, 0x18A);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18A);
    }
    if ((u8)fn_802357CC(ctx, param3) <= 4) {
        handle = fn_80239984(handle, ctx, 0x18B);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18B);
    }
    return handle;
}
/* Address: 0x802430E4 | Size: 0x94 */
u32 fightTrainerAiWazaValueMeityuuDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802358AC(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;

    setup = 0;
    if (fn_802358AC(ctx, param3) <= 4U) {
        setup = fn_80239984(0, ctx, 0x189);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x189);
    }
    return setup;
}
/* Address: 0x80243178 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueTokubouDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_802370AC();
    extern u32 fn_80236F4C();
    extern u32 fn_80235974();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_802370AC(arg0, arg3);
    limit = fn_80236F4C(arg0, arg3);
    if ((u16)count < (u16)limit) {
        handle = fn_80239984(0, arg0, 0x187);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x187);
    }
    if ((u8)fn_80235974(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x188);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x188);
    }
    return handle;
}
/* Address: 0x80243284 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueBougyoDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_802370AC();
    extern u32 fn_80236F4C();
    extern u32 fn_80235A3C();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_802370AC(arg0, arg3);
    limit = fn_80236F4C(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fn_80239984(0, arg0, 0x185);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x185);
    }
    if ((u8)fn_80235A3C(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x186);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x186);
    }
    return handle;
}
/* Address: 0x80243390 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueKougekiDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fn_8023715C();
    extern u32 fn_80236FFC();
    extern u32 fn_80235AA0();
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fn_8023715C(arg0, arg3);
    limit = fn_80236FFC(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fn_80239984(0, arg0, 0x183);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x183);
    }
    if ((u8)fn_80235AA0(arg0, arg3) <= 4) {
        handle = fn_80239984(handle, arg0, 0x184);
        fn_80239EE8(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x184);
    }
    return handle;
}
/* Address: 0x8024349C | Size: 0x138 (312 bytes) */
u32 fightTrainerAiWazaValueSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fn_80236D60(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[10];
    u32 setup;
    u16 count;
    u8 found;
    u16 index;

    found = 0;
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 1, 1);
    index = 0;
    while (index < count) {
        if (fn_80236D60(ctx, param3, entries[index]) > 0) {
            found = 1;
            break;
        }
        index++;
    }

    if (found == 1) {
        setup = fn_80239984(0, ctx, 0x181);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x181);
    } else {
        setup = fn_80239984(0, ctx, 0x182);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x182);
    }
    return setup;
}
/* Address: 0x802435D4 | Size: 0xB8 */
u32 fightTrainerAiWazaValueGamusyara(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_802376EC(void*, u32);
    extern u32 fightTrainerAiAddValue(u32, u32);
    extern void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, u32);
    u32 baseHp;
    u32 handle;

    baseHp = fn_802376EC(ctx, param1);
    extra = (s32)(fn_802376EC(ctx, extra) & 0xFFFF) / (s32)(baseHp & 0xFFFF);
    extra *= fightTrainerGetStatus(0, 0x180, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, extra);
    fn_80239CCC(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x180, extra);
    return handle;
}
/* Address: 0x8024368C | Size: 0x1AC (428 bytes) */
u32 fightTrainerAiWazaValueItamiwake(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fn_802387C8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 ownHp;
    s32 targetHp;
    u32 handle;

    handle = 0;
    ownHp = fn_802387C8(ctx, param1);
    targetHp = fn_802387C8(ctx, param3);
    if (ownHp * 3 <= targetHp) {
        handle = fn_80239984(0, ctx, 0x17d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17d);
    } else if (ownHp * 2 <= targetHp) {
        handle = fn_80239984(0, ctx, 0x17c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17c);
    } else if (ownHp >= targetHp * 3) {
        handle = fn_80239984(0, ctx, 0x17f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17f);
    } else if (ownHp >= targetHp * 2) {
        handle = fn_80239984(0, ctx, 0x17e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17e);
    }
    return handle;
}
/* Address: 0x80243838 | Size: 0x94 */
s32 fightTrainerAiWazaValueAkumu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern void* fightOutPokemonGetPokemonPtr();
    extern void* fn_80239984();
    extern void fn_80239EE8();
    s32 ret = 0;

    if (fn_80236BFC(ctx, param3, 0x17) == 0) {
        ret = (s32)fn_80239984(0, ctx, 0x17b);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17b);
    }
    return ret;
}
/* Address: 0x802438CC | Size: 0x140 (320 bytes) */
u32 fightTrainerAiWazaValueRokkuon(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u16);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u16 fn_80201D84(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;
    u16 current;

    setup = 0;
    current = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, (u16)fightFloorGetStatus(0, 0, 0x14, 0));
    if (fn_80236BFC(ctx, param3, 0x1d) == 0) {
        setup = fn_80239984(0, ctx, 0x179);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x179);
    }
    if (fn_80236BFC(ctx, param3, 0x1d) == 1) {
        if (current == fn_80201D84(param3, 0x1d)) {
            setup = fn_80239984(setup, ctx, 0x17a);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17a);
        }
    }
    return setup;
}
/* Address: 0x80243A0C | Size: 0x250 (592 bytes) */
u32 fightTrainerAiWazaValueNoroi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern f32 lbl_8047E640;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235714(void*, u32);
    extern u32 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80237DBC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fn_80237DBC(ctx, param1, 7);
    if ((u8)state == 1 && (u8)fn_802373B0(ctx, param1, 1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x174);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x174);
    } else if ((u8)state == 1 && (u8)fn_802373B0(ctx, param1, -1, lbl_8047E640) == 1) {
        handle = fn_80239984(0, ctx, 0x175);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x175);
    }
    if ((u8)state == 0 && (u8)fn_80235714(ctx, param1) == 0) {
        handle = fn_80239984(handle, ctx, 0x176);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x176);
    }
    if ((u8)state == 1) {
        handle = fn_80239984(handle, ctx, 0x177);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x177);
    }
    if ((u8)state == 0 && (u8)fn_80235714(ctx, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x178);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x178);
    }
    return handle;
}
/* Address: 0x80243C5C | Size: 0x7C | Pattern: field_accessor */
s32 fightTrainerAiWazaValueToriaezutukae(void* ctx, u32 slot, u32 param) {
    extern s32 fightOutPokemonGetPokemonPtr();
    extern s32 fn_80239984();
    extern void fn_80239EE8();
    s32 handle = fn_80239984(0, ctx, 0x173);
    fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x173);
    return handle;
}
