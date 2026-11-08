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
extern int  wazaSequenceSysInitEx();
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
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
u8 fightTrainerAiIsJoutaiNormal(void*, u32);
u8 fightTrainerAiIsTokusei(void*, u32, u32);
u8 fightTrainerAiCheckWriteJoutai(void*, u32, u32);
u8 fightTrainerAiGetWazaTsuikaAvg(void*, u32);
u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
u16 fightTrainerAiGetPokemonDataId2(void*, u32);
u8 fightTrainerAiGetPokemonZokusei(void*, u32, u32);
u8 fightTrainerAiIsPokemonTokusei(void*, u32, u32);
u32 fightTrainerAiAddValueClamped(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fightTrainerAiLogValue(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);

/* Address: 0x80241B70 | Size: 0x278 (632 bytes) */
u32 fightTrainerAiWazaValueKyouseikoutai(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern u32 fightSideGetCountAsJoutaiDataId(u32, u32);
    extern u8 fightSideIsJoutaiDataId(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetAbicntAvoid(void*, u32);
    extern u8 fightTrainerAiGetAbicntAverage(void*, u32);
    extern u8 fightTrainerAiGetAbicntNimbleness(void*, u32);
    extern u8 fightTrainerAiGetAbicntSpeDef(void*, u32);
    extern u8 fightTrainerAiGetAbicntSpeAtk(void*, u32);
    extern u8 fightTrainerAiGetAbicntPhyDef(void*, u32);
    extern u8 fightTrainerAiGetAbicntPhyAtk(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
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
    stats[0] = fightTrainerAiGetAbicntPhyAtk(ctx, param3);
    stats[1] = fightTrainerAiGetAbicntPhyDef(ctx, param3);
    stats[2] = fightTrainerAiGetAbicntSpeAtk(ctx, param3);
    stats[3] = fightTrainerAiGetAbicntSpeDef(ctx, param3);
    stats[4] = fightTrainerAiGetAbicntNimbleness(ctx, param3);
    stats[5] = fightTrainerAiGetAbicntAverage(ctx, param3);
    stats[6] = fightTrainerAiGetAbicntAvoid(ctx, param3);

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
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x1ac);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ac);
    }
    if ((s16)mode == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1ad);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ad);
    } else if ((s16)mode == 2) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1ae);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ae);
    } else if ((s16)mode == 3) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1af);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1af);
    }
    return handle;
}
/* Address: 0x80241DE8 | Size: 0x1FC (508 bytes) */
s32 fightTrainerAiWazaValueYokodori(void* ctx, void* param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u16 fightTrainerAiGetLastSelectWaza();
    extern u32 fightTrainerAiGetLastUseWaza();
    extern u16 fightTrainerAiGetPokemonDataId();
    extern u8 fightTrainerAiIsWazaTypeId();
    extern void* fightTrainerAiAddValueClamped();
    extern void fightTrainerAiAddValueLog2();
    u32 buf[8];
    s32 handle = 0;
    u32 r1v;
    u16 r2v;
    u16 count;
    u16 i;
    u32 r3v;

    r1v = fightTrainerAiGetLastUseWaza(ctx, param1);
    r2v = fightTrainerAiGetLastSelectWaza(ctx, param1);
    r3v = fightTrainerAiGetLastUseWaza(ctx, param3);
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf, 0, 1);

    if ((u16)r3v != 0 && (u16)r3v != 0xffff && (u16)r3v != 0x165 && (u16)r3v != 0x163 &&
        fightTrainerAiIsWazaTypeId(ctx, r3v, 4) == 1) {
        handle = (s32)fightTrainerAiAddValueClamped(0, ctx, 0x1a9);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a9);
    }

    for (i = 0; i < count; i++) {
        u16 v = fightTrainerAiGetPokemonDataId(ctx, buf[i]);
        if (v == 0x12e || v == 0xd4 || v == 0x177) {
            handle = (s32)fightTrainerAiAddValueClamped(handle, ctx, 0x1aa);
            fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1aa);
            break;
        }
    }

    if ((u16)r1v == 0x121 || r2v == 0x121) {
        handle = (s32)fightTrainerAiAddValueClamped(handle, ctx, 0x1ab);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1ab);
    }
    return handle;
}
/* Address: 0x80241FE4 | Size: 0x2A8 (680 bytes) */
s32 fightTrainerAiWazaValueKonoyubitomare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightOutPokemonGetPokemonPtr();
    extern u16 fightTrainerAiGetPokemonDataId();
    extern u8 fightTrainerAiGetPartData1C();
    extern u16 fightTrainerAiGetAisyou();
    extern u16 fightAiCheckAllyFlag2A();
    extern s32 fightTrainerAiGetWazaIryoku();
    extern void* fightTrainerAiAddValueClamped();
    extern void fightTrainerAiAddValueLog2();
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
        countC = fightAiCheckAllyFlag2A(ctx, listB[i], listC);
        for (j = 0; j < countC; j++) {
            for (k = 0; k < countA; k++) {
                if (param1 != listA[k] &&
                    fightTrainerAiGetAisyou(ctx, listA[k], listC[j], fightTrainerAiGetWazaIryoku(ctx, param2)) == 0x41) {
                    found = 1;
                }
            }
        }
    }

    if (found == 1) {
        handle = (s32)fightTrainerAiAddValueClamped(0, ctx, 0x1A6);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A6);
    }

    for (i = 0; i < countA; i++) {
        if (param1 != listA[i] && fightTrainerAiGetPartData1C(ctx, listA[i]) == 2) {
            handle = (s32)fightTrainerAiAddValueClamped(handle, ctx, 0x1A7);
            fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A7);
            break;
        }
    }

    for (i = 0; i < countB; i++) {
        u16 v = fightTrainerAiGetPokemonDataId(ctx, listB[i]);
        if (v == 0x12E || v == 0xD4 || v == 0x177) {
            handle = (s32)fightTrainerAiAddValueClamped(handle, ctx, 0x1A8);
            fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1A8);
            break;
        }
    }

    return handle;
}
/* Address: 0x8024228C | Size: 0x3BC (956 bytes) */
u32 fightTrainerAiWazaValueKoukanKinsi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 3) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x19d);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19d);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 4) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x19e);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19e);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 6) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x19f);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19f);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 5) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1a0);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a0);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 9) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1a1);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a1);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0xa) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1a2);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a2);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0x18) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1a3);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a3);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1e) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1a4);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a4);
    }
    if ((u8)fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1c) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x1a5);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x1a5);
    }
    return handle;
}
/* Address: 0x80242648 | Size: 0xE8 (232 bytes) */
u32 fightTrainerAiWazaValueKanasibari(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetLastUseWaza(void*, u32);
    extern s32 fightTrainerAiIsFasterBasis(void*, u32, u32);
    extern u8 fightTrainerAiIsWazaTypeId(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;

    handle = 0;
    move = fightTrainerAiGetLastUseWaza(ctx, param3);
    if (fightTrainerAiIsFasterBasis(ctx, param1, param3) > 0) {
        if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
            (move & 0xFFFF) != 0x163) {
            if (fightTrainerAiIsWazaTypeId(ctx, move, 1) == 1) {
                handle = fightTrainerAiAddValueClamped(0, ctx, 0x19c);
                fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19c);
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
    extern u8 fightTrainerAiGetHpPercent(void*, u32, s32, f32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fightTrainerAiGetHpPercent(ctx, param1, -1, lbl_8047E638) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x19b);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19b);
    } else if (fightTrainerAiGetHpPercent(ctx, param1, -1, lbl_8047E63C) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x19a);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x19a);
    } else if (fightTrainerAiGetHpPercent(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x199);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x199);
    }
    return handle;
}
/* Address: 0x802428A0 | Size: 0x134 (308 bytes) */
u32 fightTrainerAiWazaValueUrami(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetLastUseWaza(void*, u32);
    extern s32 fightTrainerAiIsFasterBasis(void*, u32, u32);
    extern u8 fightAiGetWazaPp(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;
    u32 value;

    handle = 0;
    move = fightTrainerAiGetLastUseWaza(ctx, param3);
    if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
        (move & 0xFFFF) != 0x163 && fightTrainerAiIsFasterBasis(ctx, param1, param3) > 0) {
        value = fightAiGetWazaPp(ctx, move);
        if ((value & 0xFF) <= 5) {
            handle = fightTrainerAiAddValueClamped(0, ctx, 0x197);
            fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x197);
        } else if ((value & 0xFF) <= 0xa) {
            handle = fightTrainerAiAddValueClamped(0, ctx, 0x198);
            fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x198);
        }
    }
    return handle;
}
/* Address: 0x802429D4 | Size: 0x17C (380 bytes) */
u32 fightTrainerAiWazaValueOdareru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetBasisSpeAtk(void*, u32);
    extern u32 fightTrainerAiGetBasisPhyDef(void*, u32);
    extern u32 fightTrainerAiGetBasisPhyAtk(void*, u32);
    extern u8 fightTrainerAiIsTokusei(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 statLimit;
    u32 statCurrent;
    u32 statNext;
    u32 handle;

    handle = 0;
    statLimit = fightTrainerAiGetBasisSpeAtk(ctx, param3);
    statCurrent = fightTrainerAiGetBasisPhyAtk(ctx, param3);
    statNext = fightTrainerAiGetBasisPhyDef(ctx, param3);
    if ((u16)statCurrent > (u16)statLimit) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x194);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x194);
    }
    if ((u16)statCurrent > (u16)statNext) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x195);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x195);
    }
    if (fightTrainerAiIsTokusei(ctx, param3, 0x14) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x196);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x196);
    }
    return handle;
}
/* Address: 0x80242B50 | Size: 0x17C (380 bytes) */
u32 fightTrainerAiWazaValueIbaru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetBasisSpeAtk(void*, u32);
    extern u32 fightTrainerAiGetBasisPhyDef(void*, u32);
    extern u32 fightTrainerAiGetBasisPhyAtk(void*, u32);
    extern u8 fightTrainerAiIsTokusei(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 statLimit;
    u32 statCurrent;
    u32 statNext;
    u32 handle;

    handle = 0;
    statLimit = fightTrainerAiGetBasisSpeAtk(ctx, param3);
    statCurrent = fightTrainerAiGetBasisPhyAtk(ctx, param3);
    statNext = fightTrainerAiGetBasisPhyDef(ctx, param3);
    if ((u16)statLimit > (u16)statCurrent) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x191);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x191);
    }
    if ((u16)statCurrent > (u16)statNext) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x192);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x192);
    }
    if (fightTrainerAiIsTokusei(ctx, param3, 0x14) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x193);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x193);
    }
    return handle;
}
/* Address: 0x80242CCC | Size: 0xE4 (228 bytes) */
u32 fightTrainerAiWazaValueAnkooru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetLastSelectWaza(void*, u32);
    extern s32 fightTrainerAiIsFasterBasis(void*, u32, u32);
    extern u8 fightTrainerAiIsWazaTypeId(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 move;

    handle = 0;
    move = fightTrainerAiGetLastSelectWaza(ctx, param3);
    if (fightTrainerAiIsFasterBasis(ctx, param1, param3) > 0) {
        if ((move & 0xFFFF) != 0 && (move & 0xFFFF) != 0xFFFF && (move & 0xFFFF) != 0x165 &&
            (move & 0xFFFF) != 0x163) {
            if (fightTrainerAiIsWazaTypeId(ctx, move, 1) == 0) {
                handle = fightTrainerAiAddValueClamped(0, ctx, 0x190);
                fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x190);
            }
        }
    }
    return handle;
}
/* Address: 0x80242DB0 | Size: 0x9C */
u32 fightTrainerAiWazaValueIkarinomaeba(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern const f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetHpPercent(void*, u32, s32, f32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fightTrainerAiGetHpPercent(ctx, extra, 1, lbl_8047E630) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x18f);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18f);
    }
    return handle;
}
/* Address: 0x80242E4C | Size: 0x1A0 (416 bytes) */
u32 fightTrainerAiWazaValueKusuguruDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetAbicntPhyDef(void*, u32);
    extern u8 fightTrainerAiGetAbicntPhyAtk(void*, u32);
    extern u32 fightTrainerAiGetBasisSpeDef(void*, u32);
    extern u32 fightTrainerAiGetBasisSpeAtk(void*, u32);
    extern u32 fightTrainerAiGetBasisPhyDef(void*, u32);
    extern u32 fightTrainerAiGetBasisPhyAtk(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 currentAtk;
    u32 limitAtk;
    u32 currentDef;
    u32 limitDef;
    u32 handle;

    handle = 0;
    currentAtk = fightTrainerAiGetBasisPhyAtk(ctx, param3);
    limitAtk = fightTrainerAiGetBasisSpeAtk(ctx, param3);
    currentDef = fightTrainerAiGetBasisPhyDef(ctx, param3);
    limitDef = fightTrainerAiGetBasisSpeDef(ctx, param3);
    if ((u16)currentAtk > (u16)limitAtk) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x18c);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18c);
    }
    if ((u16)currentDef > (u16)limitDef) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x18d);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18d);
    }
    if (fightTrainerAiGetAbicntPhyAtk(ctx, param3) <= 4 && fightTrainerAiGetAbicntPhyDef(ctx, param3) <= 4) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x18e);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18e);
    }
    return handle;
}
/* Address: 0x80242FEC | Size: 0xF8 (248 bytes) */
u32 fightTrainerAiWazaValueKaihiDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightTrainerAiGetAbicntAvoid();
    extern s32 fightTrainerAiAddValueClamped();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValueLog2();
    s32 handle = 0;

    if ((u8)fightTrainerAiGetAbicntAvoid(ctx, param3) >= 7) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x18A);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18A);
    }
    if ((u8)fightTrainerAiGetAbicntAvoid(ctx, param3) <= 4) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x18B);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x18B);
    }
    return handle;
}
/* Address: 0x802430E4 | Size: 0x94 */
u32 fightTrainerAiWazaValueMeityuuDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetAbicntAverage(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;

    setup = 0;
    if (fightTrainerAiGetAbicntAverage(ctx, param3) <= 4U) {
        setup = fightTrainerAiAddValueClamped(0, ctx, 0x189);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x189);
    }
    return setup;
}
/* Address: 0x80243178 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueTokubouDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fightTrainerAiGetBasisPhyDef();
    extern u32 fightTrainerAiGetBasisSpeDef();
    extern u32 fightTrainerAiGetAbicntSpeDef();
    extern s32 fightTrainerAiAddValueClamped();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValueLog2();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fightTrainerAiGetBasisPhyDef(arg0, arg3);
    limit = fightTrainerAiGetBasisSpeDef(arg0, arg3);
    if ((u16)count < (u16)limit) {
        handle = fightTrainerAiAddValueClamped(0, arg0, 0x187);
        fightTrainerAiAddValueLog2(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x187);
    }
    if ((u8)fightTrainerAiGetAbicntSpeDef(arg0, arg3) <= 4) {
        handle = fightTrainerAiAddValueClamped(handle, arg0, 0x188);
        fightTrainerAiAddValueLog2(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x188);
    }
    return handle;
}
/* Address: 0x80243284 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueBougyoDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fightTrainerAiGetBasisPhyDef();
    extern u32 fightTrainerAiGetBasisSpeDef();
    extern u32 fightTrainerAiGetAbicntPhyDef();
    extern s32 fightTrainerAiAddValueClamped();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValueLog2();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fightTrainerAiGetBasisPhyDef(arg0, arg3);
    limit = fightTrainerAiGetBasisSpeDef(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fightTrainerAiAddValueClamped(0, arg0, 0x185);
        fightTrainerAiAddValueLog2(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x185);
    }
    if ((u8)fightTrainerAiGetAbicntPhyDef(arg0, arg3) <= 4) {
        handle = fightTrainerAiAddValueClamped(handle, arg0, 0x186);
        fightTrainerAiAddValueLog2(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x186);
    }
    return handle;
}
/* Address: 0x80243390 | Size: 0x10C (268 bytes) */
u32 fightTrainerAiWazaValueKougekiDaun(void* arg0, void* arg1, u32 arg2, void* arg3) {
    extern u32 fightTrainerAiGetBasisPhyAtk();
    extern u32 fightTrainerAiGetBasisSpeAtk();
    extern u32 fightTrainerAiGetAbicntPhyAtk();
    extern s32 fightTrainerAiAddValueClamped();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightTrainerAiAddValueLog2();
    s32 handle;
    s32 count;
    s32 limit;

    handle = 0;
    count = fightTrainerAiGetBasisPhyAtk(arg0, arg3);
    limit = fightTrainerAiGetBasisSpeAtk(arg0, arg3);
    if ((u16)count > (u16)limit) {
        handle = fightTrainerAiAddValueClamped(0, arg0, 0x183);
        fightTrainerAiAddValueLog2(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x183);
    }
    if ((u8)fightTrainerAiGetAbicntPhyAtk(arg0, arg3) <= 4) {
        handle = fightTrainerAiAddValueClamped(handle, arg0, 0x184);
        fightTrainerAiAddValueLog2(0xEC64, arg0, fightOutPokemonGetPokemonPtr(arg1), 0, 0, arg2, 0, 0x184);
    }
    return handle;
}
/* Address: 0x8024349C | Size: 0x138 (312 bytes) */
u32 fightTrainerAiWazaValueSubayasaDaun(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fightTrainerAiIsFasterBasis(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[10];
    u32 setup;
    u16 count;
    u8 found;
    u16 index;

    found = 0;
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, entries, 1, 1);
    index = 0;
    while (index < count) {
        if (fightTrainerAiIsFasterBasis(ctx, param3, entries[index]) > 0) {
            found = 1;
            break;
        }
        index++;
    }

    if (found == 1) {
        setup = fightTrainerAiAddValueClamped(0, ctx, 0x181);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x181);
    } else {
        setup = fightTrainerAiAddValueClamped(0, ctx, 0x182);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x182);
    }
    return setup;
}
/* Address: 0x802435D4 | Size: 0xB8 */
u32 fightTrainerAiWazaValueGamusyara(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiGetHp(void*, u32);
    extern u32 fightTrainerAiAddValue(u32, u32);
    extern void fightTrainerAiLogValue(u32, void*, u32, u32, u32, u32, u32, u32, u32);
    u32 baseHp;
    u32 handle;

    baseHp = fightTrainerAiGetHp(ctx, param1);
    extra = (s32)(fightTrainerAiGetHp(ctx, extra) & 0xFFFF) / (s32)(baseHp & 0xFFFF);
    extra *= fightTrainerGetStatus(0, 0x180, 0x3e, 0);
    handle = fightTrainerAiAddValue(0, extra);
    fightTrainerAiLogValue(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x180, extra);
    return handle;
}
/* Address: 0x8024368C | Size: 0x1AC (428 bytes) */
u32 fightTrainerAiWazaValueItamiwake(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern s32 fightAiGetHpPercent(void*, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 ownHp;
    s32 targetHp;
    u32 handle;

    handle = 0;
    ownHp = fightAiGetHpPercent(ctx, param1);
    targetHp = fightAiGetHpPercent(ctx, param3);
    if (ownHp * 3 <= targetHp) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x17d);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17d);
    } else if (ownHp * 2 <= targetHp) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x17c);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17c);
    } else if (ownHp >= targetHp * 3) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x17f);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17f);
    } else if (ownHp >= targetHp * 2) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x17e);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17e);
    }
    return handle;
}
/* Address: 0x80243838 | Size: 0x94 */
s32 fightTrainerAiWazaValueAkumu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightTrainerAiIsPokemonJoutai();
    extern void* fightOutPokemonGetPokemonPtr();
    extern void* fightTrainerAiAddValueClamped();
    extern void fightTrainerAiAddValueLog2();
    s32 ret = 0;

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x17) == 0) {
        ret = (s32)fightTrainerAiAddValueClamped(0, ctx, 0x17b);
        fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17b);
    }
    return ret;
}
/* Address: 0x802438CC | Size: 0x140 (320 bytes) */
u32 fightTrainerAiWazaValueRokkuon(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u16);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u16 fightOutPokemonGetJoutaiParam(u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 setup;
    u16 current;

    setup = 0;
    current = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, (u16)fightFloorGetStatus(0, 0, 0x14, 0));
    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1d) == 0) {
        setup = fightTrainerAiAddValueClamped(0, ctx, 0x179);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x179);
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1d) == 1) {
        if (current == fightOutPokemonGetJoutaiParam(param3, 0x1d)) {
            setup = fightTrainerAiAddValueClamped(setup, ctx, 0x17a);
            fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x17a);
        }
    }
    return setup;
}
/* Address: 0x80243A0C | Size: 0x250 (592 bytes) */
u32 fightTrainerAiWazaValueNoroi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern f32 lbl_8047E640;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fightTrainerAiIsAbiCntAbove(void*, u32);
    extern u32 fightTrainerAiGetHpPercent(void*, u32, s32, f32);
    extern u32 fightTrainerAiGetZokusei(void*, u32, u32);
    extern u32 fightTrainerAiAddValueClamped(u32, void*, u32);
    extern void fightTrainerAiAddValueLog2(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fightTrainerAiGetZokusei(ctx, param1, 7);
    if ((u8)state == 1 && (u8)fightTrainerAiGetHpPercent(ctx, param1, 1, lbl_8047E630) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x174);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x174);
    } else if ((u8)state == 1 && (u8)fightTrainerAiGetHpPercent(ctx, param1, -1, lbl_8047E640) == 1) {
        handle = fightTrainerAiAddValueClamped(0, ctx, 0x175);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x175);
    }
    if ((u8)state == 0 && (u8)fightTrainerAiIsAbiCntAbove(ctx, param1) == 0) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x176);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x176);
    }
    if ((u8)state == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x177);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x177);
    }
    if ((u8)state == 0 && (u8)fightTrainerAiIsAbiCntAbove(ctx, param1) == 1) {
        handle = fightTrainerAiAddValueClamped(handle, ctx, 0x178);
        fightTrainerAiAddValueLog2(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x178);
    }
    return handle;
}
/* Address: 0x80243C5C | Size: 0x7C | Pattern: field_accessor */
s32 fightTrainerAiWazaValueToriaezutukae(void* ctx, u32 slot, u32 param) {
    extern s32 fightOutPokemonGetPokemonPtr();
    extern s32 fightTrainerAiAddValueClamped();
    extern void fightTrainerAiAddValueLog2();
    s32 handle = fightTrainerAiAddValueClamped(0, ctx, 0x173);
    fightTrainerAiAddValueLog2(0xEC64, ctx, fightOutPokemonGetPokemonPtr(slot), 0, 0, param, 0, 0x173);
    return handle;
}
