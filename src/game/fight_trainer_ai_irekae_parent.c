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

#if !defined(FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY)
#if !defined(FIGHT_TRAINER_AI_IREKAE_ITEM_ONLY)
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
typedef struct IrekaeKindTable {
    s16 kinds[14];
} IrekaeKindTable;

/* RULE-EXCEPTION(user-approved): single-function inline helper; inlining it gives retail's
 * `li r22,0; mr r25,r22` for the candidate count and index (99.95% as an open-coded loop)
 * - see docs/RULE_EXCEPTIONS.md */
static inline u16 aiGatherKindCandidates(void* ctx, u32* pokemon, u32* candidates, s16 kindA, s16 kindB) {
    extern u8 fightTrainerAiGetPokemonPartData1C(void* ctx, u32 pokemon);
    extern u8 fightAiCheckSwitchFlag33(void* ctx, u32 pokemon);
    u16 n;
    u16 i;
    u32 p;

    for (i = 0; i < 6; i++) {
        candidates[i] = 0;
    }
    n = 0;
    for (i = 0; i < 6; i++) {
        p = pokemon[i];
        if (p == 0) {
            continue;
        }
        if (kindA >= 0 && kindA != fightTrainerAiGetPokemonPartData1C(ctx, p)) {
            continue;
        }
        if (kindB >= 0 && kindB != fightAiCheckSwitchFlag33(ctx, p)) {
            continue;
        }
        candidates[n] = p;
        n++;
    }
    return n;
}

s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern const IrekaeKindTable lbl_8027A434;
    extern u32 lbl_80478B38;
    extern s32 fightTrainerGetStatus(u32, u32, u32, u16);
    extern u8 fightTrainerAiGetNowTenkou(void* ctx, u32 zero, u32 one);
    extern void fightOutPokemonBiosCopy();
    extern s32 fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId();
    extern u8 fightOutPokemonCheckFightOut(u32 pokemon);
    extern u8 fightOutPokemonIsFightActionAttackWazaOut(u32 pokemon, u32 move, u32 zero);
    extern u16 fightTrainerGetIrekaeFightPokemonAry(void* ctx, u32* pokemon);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern void fightTrainerAiLogText(u32, u32, u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern u16 GSrand(void);
    extern void* fightFloorGetFightPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fightPokemonGetPokemonPtr(u32);
    extern u8 fightTrainerAiGetPokemonPartData1C(void* ctx, u32 pokemon);
    extern u32 fn_802386C8(void* ctx, u32 pokemon);
    extern s32 fightTrainerAiGetPokemonAisyouScore(void* ctx, u32 pokemon);
    extern void fightOutPokemonCreate(u32 out, u32 pokemon, u32 zero);
    extern u16 fightTrainerAiGetFightPokemonWazaAry(void* ctx, u32 pokemon, u16* moves, u32 zero, u32 one);
    extern u8 fightTrainerAiGetWazaHitValue(void* ctx, u32 pokemon, u32 move, u32 target);
    extern u16 fightTrainerAiGetFightPokemonWazaAry2(void* ctx, u32 pokemon, u16* moves, u32 zero, u32 one);
    extern s32 fn_80239984(s32 value, void* ctx, u32 id);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern u8 fightTrainerAiGetPokemonZokusei(void* ctx, u32 pokemon, u32 type);
    extern s32 fightTrainerAiAddValueScaled(s32 value, u32 amount, void* ctx, u32 id);
    extern void fightTrainerAiAddValueLog(u32, void*, u32, u32, u32, u32, u32, u32, u32);
    extern u8 fn_8023881C(void* ctx, u32 pokemon);
    extern u16 fightTrainerAiGetWazaTenkouValue(void* ctx, u32 move, u32 pokemon);
    extern u8 fn_8023943C(void* ctx, u32 move, u32 one);
    extern u32 fn_80239500(void* ctx, u32 move);
    extern u16 fightTrainerAiGetAisyou(void* ctx, u32 pokemon, u16 type, u32 power);
    extern u16 fightTrainerAiGetPokemonAisyou(void* ctx, u32 pokemon, u16 type, u32 power);
    extern u8 fightTrainerCheckCanIrekaeFightPokemon(void* trainer, u32 pokemon);
    extern u8 fn_80239058(void* ctx, u32 elem, u32 type);
    extern u8 fn_8000815C(void);
    extern s32 fightTrainerAiAddValue(s32 value, s32 add);
    extern s32 fightTrainerAiGetValueAryMaxBanme(s32* values, u16 count, u32 one);
    u8 work[0x6E0];
    u32 fightPokemon[24];
    u32 pokemon[6];
    u32 candidates[6];
    s32 values[6];
    u16 moves[10];
    u32 outPokemon[8];
    IrekaeKindTable kinds;
    u32 aiId;
    u8 attacked;
    u8 mode;
    u16 count;
    u16 outCount;
    u16 fightCount;
    u16 i;
    u16 n;
    u16 j;
    u16 k;
    u16 found;
    u32 p;
    u32 q;
    void* trainer;
    u32 maxA;
    s32 maxB;
    u16 minLevel;
    u16 moveCount;
    u16 moveCount2;
    u8 hit;
    u32 move;
    u16 type;
    u16 moveType;
    u8 kind;
    u8 amount;
    s32 id;
    s32 rangeSize;
    u8 randRange;
    s32 rnd;
    s32 best;

    aiId = fightTrainerGetStatus(0, (u16)fightTrainerGetStatus((u32)ctx, 0, 0x43, 0), 2, 0) & 0xFFFF;
    randRange = fightTrainerGetStatus(0, aiId, 0x38, 0);
    attacked = 0;
    kinds = lbl_8027A434;
    mode = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    for (i = 0; i < 6; i++) {
        values[i] = 0;
    }
    fightOutPokemonBiosCopy(work, param3);
    if (fightOutPokemonCheckFightOut(param3) == 1 &&
        fightOutPokemonIsFightActionAttackWazaOut(param3, 0xE2, 0) == 1) {
        attacked = 1;
    }
    count = fightTrainerGetIrekaeFightPokemonAry(ctx, pokemon);
    outCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, outPokemon, 0, 1);
    fightCount = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, fightPokemon, 1, 1);
    if (count == 0) {
        return -1;
    }

    if ((u8)fightTrainerGetStatus(0, aiId, 0x1F, 0) == 1) {
        fightTrainerAiLogText(0xEC62, 0xEC04, 0xEC1D, ctx, 0, 0, 0, 0, 0, 0, 0);
        p = pokemon[(u16)(GSrand() % count)];
        if (p != 0) {
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, p);
            fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                        0, 0, 0, 0, 0x228, 0);
            return (s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0);
        }
    }

    if (attacked == 1) {
        found = 0;
        for (k = 0; k < 14; k += 2) {
            n = aiGatherKindCandidates(ctx, pokemon, candidates, kinds.kinds[k], kinds.kinds[k + 1]);
            found = n;
            if (n != 0) {
                break;
            }
        }
        if (found != 0) {
            p = candidates[(u16)(GSrand() % found)];
            if (p != 0) {
                trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, p);
                fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                            0, 0, 0, 0, 0x228, 0);
                return (s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0);
            }
        }
    }

    maxA = 0;
    maxB = -0xFFFF;
    minLevel = 0xFFFF;
    for (i = 0; i < count; i++) {
        p = pokemon[i];
        if (p == 0) {
            continue;
        }
        if ((s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0) < 0) {
            continue;
        }
        {
            u32 a = fn_802386C8(ctx, p);
            s32 b = fightTrainerAiGetPokemonAisyouScore(ctx, p);
            u16 level = (u32)pokemonGetStatus(fightPokemonGetPokemonPtr(p), 0, 0xC9, 0);

            if (maxA < a) {
                maxA = a;
            }
            if (maxB < b) {
                maxB = b;
            }
            if (minLevel > level) {
                minLevel = level;
            }
        }
    }

    if ((u8)fightTrainerGetStatus(0, aiId, 0x21, 0) == 1) {
        for (i = 0; i < count; i++) {
            p = pokemon[i];
            if (p == 0) {
                continue;
            }
            if ((s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0) < 0) {
                continue;
            }
            fightFloorGetFightPokemonPtrToFightTrainerPtr(0, p);
            if (minLevel < (u16)(u32)pokemonGetStatus(fightPokemonGetPokemonPtr(p), 0, 0xC9, 0)) {
                continue;
            }
            trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, p);
            fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                        0, 0, 0, 0, 0x228, 0);
            return (s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0);
        }
    }

    rangeSize = randRange * 2 + 1;
    for (i = 0; i < count; i++) {
        p = pokemon[i];
        if (p == 0) {
            continue;
        }
        if ((s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0) < 0) {
            continue;
        }
        trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, p);
        fightOutPokemonCreate(param3, p, 0);
        moveCount = fightTrainerAiGetFightPokemonWazaAry(ctx, param3, moves, 0, 1);
        hit = 0;
        for (j = 0; j < outCount; j++) {
            q = outPokemon[j];
            if (q == 0) {
                continue;
            }
            for (k = 0; k < moveCount; k++) {
                move = moves[k];
                if (move == 0 || move == 0x165) {
                    continue;
                }
                if (fightTrainerAiGetWazaHitValue(ctx, param3, move, q) == 1) {
                    hit = 1;
                    break;
                }
            }
            if (hit == 1) {
                break;
            }
        }
        fightOutPokemonBiosCopy(param3, work);
        moveCount2 = fightTrainerAiGetFightPokemonWazaAry2(ctx, p, moves, 0, 1);
        fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                    0, 0, 0, 0, 0x227, values[i]);

        kind = fightTrainerAiGetPokemonPartData1C(ctx, p);
        if (kind == 1) {
            values[i] = fn_80239984(values[i], ctx, 0x1A);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x1A);
        }
        if (kind == 2) {
            values[i] = fn_80239984(values[i], ctx, 0x1B);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x1B);
        }
        if (kind == 3) {
            values[i] = fn_80239984(values[i], ctx, 0x1C);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x1C);
        }
        for (k = 0; k < 2; k++) {
            type = fightTrainerGetStatus(0, aiId, 0x39, k);
            if (type == 9) {
                continue;
            }
            if (fightTrainerAiGetPokemonZokusei(ctx, p, type) == 1) {
                amount = fightTrainerGetStatus(0, aiId, 0x3A, k);
                values[i] = fightTrainerAiAddValueScaled(values[i], amount, ctx, 0x1D);
                fightTrainerAiAddValueLog(0xEC63, ctx, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x1D, amount);
            }
        }
        if (fn_8023881C(ctx, p) == 1) {
            values[i] = fn_80239984(values[i], ctx, 0x1E);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x1E);
        }
        if (!hit) {
            values[i] = fn_80239984(values[i], ctx, 0x1F);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x1F);
        }
        if (maxA <= fn_802386C8(ctx, p)) {
            values[i] = fn_80239984(values[i], ctx, 0x21);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x21);
        }
        if (maxB <= fightTrainerAiGetPokemonAisyouScore(ctx, p)) {
            values[i] = fn_80239984(values[i], ctx, 0x20);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x20);
        }
        for (j = 0; j < outCount; j++) {
            q = outPokemon[j];
            if (q == 0) {
                continue;
            }
            for (k = 0; k < moveCount2; k++) {
                move = moves[k];
                if (move == 0) {
                    continue;
                }
                moveType = fightTrainerAiGetWazaTenkouValue(ctx, move, param3);
                if (moveType == 9) {
                    continue;
                }
                if (!fn_8023943C(ctx, move, 1)) {
                    continue;
                }
                if (fightTrainerAiGetAisyou(ctx, q, moveType, fn_80239500(ctx, move)) == 0x41) {
                    values[i] = fn_80239984(values[i], ctx, 0x22);
                    fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x22);
                }
            }
        }
        for (j = 0; j < outCount; j++) {
            q = outPokemon[j];
            if (q == 0) {
                continue;
            }
            n = fightTrainerAiGetFightPokemonWazaAry(ctx, q, moves, 0, 0);
            for (k = 0; k < n; k++) {
                move = moves[k];
                if (move == 0) {
                    continue;
                }
                moveType = fightTrainerAiGetWazaTenkouValue(ctx, move, q);
                if (moveType == 9) {
                    continue;
                }
                if (!fn_8023943C(ctx, move, 1)) {
                    continue;
                }
                if (fightTrainerAiGetPokemonAisyou(ctx, p, moveType, fn_80239500(ctx, move)) == 0x41) {
                    values[i] = fn_80239984(values[i], ctx, 0x23);
                    fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x23);
                }
            }
        }
        if ((u8)fightTrainerGetStatus(0, aiId, 0x20, 0) == 1) {
            for (k = 0; k < lbl_80478B38; k++) {
                if (k == 9) {
                    continue;
                }
                if (fightTrainerAiGetPokemonAisyou(ctx, p, k, 1) != 0x41) {
                    continue;
                }
                for (j = 0; j < fightCount; j++) {
                    void* other;

                    q = fightPokemon[j];
                    if (q == 0) {
                        continue;
                    }
                    other = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, q);
                    if (other == 0) {
                        continue;
                    }
                    kind = fightTrainerCheckCanIrekaeFightPokemon(other, q);
                    if (kind == 1) {
                        continue;
                    }
                    if (kind != 2 && kind != 3) {
                        continue;
                    }
                    if (fightTrainerAiGetPokemonAisyou(ctx, q, k, 1) == 0x41) {
                        values[i] = fn_80239984(values[i], ctx, 0x24);
                        fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x24);
                    }
                }
            }
        }
        id = fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId(ctx, p);
        if ((u16)id != 0) {
            values[i] = fn_80239984(values[i], ctx, id);
            fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, id);
        }
        if (mode == 2) {
            if (fn_80239058(ctx, p, 0x21) == 1 || fn_80239058(ctx, p, 0x2C) == 1) {
                values[i] = fn_80239984(values[i], ctx, 0x29);
                fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x29);
            }
        } else if (mode == 1) {
            if (fn_80239058(ctx, p, 0x22) == 1) {
                values[i] = fn_80239984(values[i], ctx, 0x2A);
                fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x2A);
            }
        } else if (mode == 3) {
            hit = 0;
            if (fn_80239058(ctx, p, 8) == 1) {
                hit = 1;
            }
            if (fightTrainerAiGetPokemonZokusei(ctx, p, 8) == 1 || fightTrainerAiGetPokemonZokusei(ctx, p, 5) == 1 ||
                fightTrainerAiGetPokemonZokusei(ctx, p, 4) == 1) {
                hit = 1;
            }
            if (hit == 1) {
                values[i] = fn_80239984(values[i], ctx, 0x2B);
                fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x2B);
            }
        } else if (mode == 4) {
            if (fightTrainerAiGetPokemonZokusei(ctx, p, 0xF) == 1) {
                values[i] = fn_80239984(values[i], ctx, 0x2C);
                fn_80239EE8(0xEC63, trainer, fightPokemonGetPokemonPtr(p), 0, 0, 0, 0, 0x2C);
            }
        }
        if (fn_8000815C() == 1) {
            rnd = GSrand() % rangeSize - randRange;
            values[i] = fightTrainerAiAddValue(values[i], rnd);
            fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                        0, 0, 0, 0, 0x225, rnd);
        }
        fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                    0, 0, 0, 0, 0x226, values[i]);
    }

    best = fightTrainerAiGetValueAryMaxBanme(values, count, 1);
    if (best < 0) {
        return -1;
    }
    p = pokemon[best];
    if (p == 0) {
        return -1;
    }
    trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, p);
    fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC1D, trainer, fightPokemonGetPokemonPtr(p),
                0, 0, 0, 0, 0x228, values[best]);
    return (s16)(u32)pokemonGetStatus(p, 0, 0xCE, 0);
}

#endif /* !ITEM_ONLY */

#if !defined(FIGHT_TRAINER_AI_IREKAE_DASU_ONLY) && !defined(FIGHT_TRAINER_AI_IREKAE_ITEM_ONLY)
/* Address: 0x8024E578 | Size: 0x118 (280 bytes) */
u32 fightTrainerAiSelectFightActionIrekae(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_80375D30[];
    int new_var2;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void*, s32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern void fightTrainerAiLogText(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32);
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
    fightTrainerAiLogText(0xec63, 0xec04, 0xec05, field, fightOutPokemonGetPokemonPtr(param1), 0, new_var2, new_var2, 0,
                (((((0x228 & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF) & 0xFFFF, score);
    choice = fightTrainerAiSelectIrekaeDasuFightPokemon(ctx, param2, 1, param1);
    if ((s16)choice < 0) {
        return 0;
    }
    new_var = choice;
    fightOutPokemonCreateFightAction(param1, new_var2, 9, 0, lbl_80375D30, (s16)new_var);
    return 1;
}

#endif /* !DASU_ONLY && !ITEM_ONLY */
#endif /* !FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY */

#if !defined(FIGHT_TRAINER_AI_IREKAE_DASU_ONLY) && !defined(FIGHT_TRAINER_AI_IREKAE_ITEM_ONLY)
/* Address: 0x8024F8B4 | Size: 0x5CC (1484 bytes) */
s32 fightTrainerAiGetFightPokemonIrekaeModosuValue(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u16 fn_80236E9C(void* ctx, u32 pokemon);
    extern u16 fightTrainerAiGetFightPokemonWazaAry(void* ctx, u32 pokemon, u16* moves, u32 zero, u32 one);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u16 fn_802376EC(void* ctx, u32 pokemon);
    extern u8 fightTrainerAiGetWazaHitValue(void* ctx, u32 pokemon, u32 move, u32 target);
    extern u8 fn_80237288(void* ctx, u32 pokemon);
    extern u8 fn_8023943C(void* ctx, u32 move, u32 one);
    extern s32 fightTrainerAiGetWazaDamageValue(void* ctx, u32 pokemon, u32 move, u32 target, u32 zero);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern void fightTrainerAiLogText(u32, u32, u32, void*, u32, u32, u32, u32, u32, u32, s32);
    extern u8 fightTrainerAiGetPartData1C(void* ctx, u32 pokemon);
    extern s32 fn_80239984(s32 value, void* ctx, u32 id);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    extern u8 fightTrainerAiGetHpRatio(void* ctx, u32 pokemon);
    extern u8 fn_80235714(void* ctx, u32 pokemon);
    extern u8 fn_8023565C(void* ctx, u32 pokemon);
    extern s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId();
    u16 moves[16];
    u32 pokemon[8];
    u8 canHit;
    u16 count;
    u16 moveCount;
    u16 level;
    u16 speed;
    s32 value;
    u16 i;
    u32 move;
    u16 j;
    void* trainer;
    u32 other;
    u8 faster;
    u8 hittable;
    s32 id;

    fightTrainerGetStatus(0, (u16)fightTrainerGetStatus((u32)ctx, 0, 0x43, 0), 2, 0);
    value = 0;
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param1);
    level = fn_80236E9C(ctx, param1);
    moveCount = fightTrainerAiGetFightPokemonWazaAry(ctx, param1, moves, 0, 1);
    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, pokemon, 0, 1);

    faster = 0;
    hittable = 0;
    for (i = 0; i < count; i++) {
        other = pokemon[i];
        if (other == 0) {
            continue;
        }
        speed = fn_802376EC(ctx, other);
        for (j = 0; j < moveCount; j++) {
            move = moves[j];
            if (move == 0 || move == 0x165) {
                continue;
            }
            canHit = fightTrainerAiGetWazaHitValue(ctx, param1, move, other);
            if (canHit == 1) {
                hittable = 1;
            }
            if (fn_80237288(ctx, other) == 1) {
                hittable = 1;
            }
            if (fn_8023943C(ctx, move, 1) == 0) {
                continue;
            }
            if (speed >= fightTrainerAiGetWazaDamageValue(ctx, param1, move, other, 0)) {
                continue;
            }
            if (canHit == 1) {
                faster = 1;
            }
        }
    }

    fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC05, trainer, fightOutPokemonGetPokemonPtr(param1),
                0, 0, 0, 0, 0x227, 0);

    if (fightTrainerAiGetPartData1C(ctx, param1)) {
        value = fn_80239984(value, ctx, 1);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 1);
    }
    if (!fightTrainerAiGetHpRatio(ctx, param1)) {
        value = fn_80239984(value, ctx, 2);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 2);
    } else if (fightTrainerAiGetPartData1C(ctx, param1)) {
        value = fn_80239984(value, ctx, 3);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 3);
    }
    if (fn_80235714(ctx, param1) == 1) {
        value = fn_80239984(value, ctx, 4);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 4);
    }
    if (fn_8023565C(ctx, param1) == 1) {
        value = fn_80239984(value, ctx, 5);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 5);
    }
    if (!hittable) {
        value = fn_80239984(value, ctx, 6);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 6);
    }
    if (faster == 1) {
        value = fn_80239984(value, ctx, 7);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 7);
    }
    if (fightTrainerAiGetHpRatio(ctx, param1) == 1) {
        for (i = 0; i < count; i++) {
            if (pokemon[i] == 0) {
                continue;
            }
            if (fn_80236E9C(ctx, pokemon[i]) > level) {
                value = fn_80239984(value, ctx, 8);
                fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, 8);
            }
        }
    }
    id = fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(ctx, param1);
    if ((u16)id != 0) {
        value = fn_80239984(value, ctx, id);
        fn_80239EE8(0xEC63, trainer, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, 0, id);
    }

    fightTrainerAiLogText(0xEC63, 0xEC04, 0xEC05, trainer, fightOutPokemonGetPokemonPtr(param1),
                0, 0, 0, 0, 0x226, value);
    return value;
}

/* Address: 0x8024FE80 | Size: 0x1F0 (496 bytes) */
typedef struct IrekaeTypeTable {
    u16 ids[10];
} IrekaeTypeTable;

s32 fightTrainerAiGetFightPokemonIrekaeDasuTokuseiAddsubDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightTrainerAiGetNowTenkou(void* ctx, u32 zero, u32 one);
    extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u8 fn_80239058(void* ctx, u32 elem, u32 type);
    extern u32 fn_8023715C(void* ctx, u32 pokemon);
    extern u32 fn_80236FFC(void* ctx, u32 pokemon);
    extern u8 fn_80237F74(void* ctx, u32 pokemon, u32 type);
    /* {0x01, 0x02, 0x23, 0x24, 0x2D, 0x32, 0x35, 0x36, 0x3B, 0x46} */
    extern const IrekaeTypeTable lbl_8027A420;
    u8 notFirst = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    IrekaeTypeTable types = lbl_8027A420;
    u32 pokemon[11];
    u16 count;
    u16 i;
    u16 j;
    u8 found;
    u32 hp;
    u32 maxHp;

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, pokemon, 0, 1);

    if (fn_80239058(ctx, param1, 0x16) == 1) {
        for (i = 0; i < count; i++) {
            hp = fn_8023715C(ctx, pokemon[i]);
            maxHp = fn_80236FFC(ctx, pokemon[i]);
            if ((u16)hp >= (u16)maxHp) {
                return 0x25;
            }
        }
    }

    if (fn_80239058(ctx, param1, 0x24) == 1) {
        for (i = 0; i < count; i++) {
            found = 0;
            for (j = 0; j < 10; j++) {
                if (fn_80237F74(ctx, pokemon[i], types.ids[j]) == 1) {
                    found = 1;
                    break;
                }
            }
            if (found != 1) {
                return 0x26;
            }
        }
    }

    if (notFirst) {
        if (fn_80239058(ctx, param1, 0x4D) == 1) {
            return 0x27;
        }
        if (fn_80239058(ctx, param1, 0xD) == 1) {
            return 0x28;
        }
    }
    return 0;
}

#endif /* !DASU_ONLY && !ITEM_ONLY (0x8024F8B4 range) */

#if !defined(FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY)
#if !defined(FIGHT_TRAINER_AI_IREKAE_DASU_ONLY) && !defined(FIGHT_TRAINER_AI_IREKAE_ITEM_ONLY)
/* Address: 0x80250070 | Size: 0x27C (636 bytes) */
s32 fightTrainerAiGetFightOutPokemonIrekaeJoutaiBadJoutaiAddsbuDataId(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightTrainerAiIsPokemonJoutai();
    extern u8 fightTrainerAiGetHpRatio();

    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x9) == 1) {
        return 0x9;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0xA) == 1) {
        return 0xA;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x1E) == 1 && fightTrainerAiGetHpRatio(ctx, param1) == 0) {
        return 0xB;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0xE) == 1) {
        return 0xC;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x17) == 1) {
        return 0xD;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x18) == 1) {
        return 0xE;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x19) == 1) {
        return 0xF;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x1B) == 1) {
        return 0x10;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x1C) == 1) {
        return 0x11;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x1D) == 1) {
        return 0x12;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x26) == 1) {
        return 0x13;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x27) == 1) {
        return 0x14;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x28) == 1) {
        return 0x15;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x29) == 1) {
        return 0x16;
    }
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x2A) == 1) {
        return 0x17;
    }
    return fightTrainerAiIsPokemonJoutai(ctx, param1, 0x30) == 1 ? 0x18 : 0;
}

#endif /* !DASU_ONLY && !ITEM_ONLY */

#if !defined(FIGHT_TRAINER_AI_IREKAE_DASU_ONLY)
/* Address: 0x802502EC | Size: 0x694 (1684 bytes) */
s32 fightTrainerAiSelectFightActionItem(void *ctx, u32 param1, u32 param2, u32 param3)
{
  extern u8 lbl_80375D70[];
  extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
  extern u16 fightTrainerGetTemotiNormalItemDataIdAry(void *ctx, u16 *items, u32 max, u32 mode);
  extern u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32, void *, u32 *, u32, u32);
  extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void *, u32 *, u32, u32);
  extern u8 fightOutPokemonCheckFightOut(u32 pokemon);
  extern u8 fn_80235714(void *ctx, u32 pokemon);
  extern u8 fn_80142984(u32 item);
  extern u8 fightSeqGetItemType(u32 item, u32 pokemon);
  extern s16 itemUse2PokemonSimulation(void *work, u32 zero, void *status, u32 item, u32 flag);
  extern u8 fightTrainerAiGetHpRatio(void *ctx, u32 pokemon);
  extern u32 fightOutPokemonGetPokemonPtr(u32);
  extern u8 pokemonIsDarkPokemon(u32 pokemon);
  extern s32 fn_80239984(s32 value, void *ctx, u32 id);
  extern void fn_80239EE8(u32, void *, u32, u32, u32, u32, u32, u32);
  extern u8 fightTrainerAiIsJoutaiNormal(void *ctx, u32 pokemon);
  extern u8 fn_80236C80(void *ctx, u32 pokemon);
  extern u8 fightTrainerAiGetPartData1C(void *ctx, u32 pokemon);
  extern u8 fightPokemonCheckFightOut(u32 pokemon);
  extern void fightTrainerAiLogText(u32, u32, u32, void *, u32, u32, u32, u32, u32, u32, s32);
  extern s32 fightTrainerAiGetValueAryMaxBanme(s32 *values, u16 count, u32 one);
  extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32 pokemon, u32 target);
  void *statusPtr;
  extern void fightOutPokemonCreateFightActionUseItem(u32, u32, u32, u32, void *, u32, u32, s32, u32);
  u8 work[0xF4];
  s32 values[20];
  u32 fightPokemon[24];
  u16 items[20];
  u32 outPokemon[8];
  u32 other;
  s32 best;
  u16 itemIdx;
  u16 outCount;
  u16 outIdx;
  u8 usable;
  u32 item;
  u16 searchIdx;
  u16 fightIdx;
  u8 type;
  u16 itemCount;
  u16 fightCount;
  u8 flag;
  u16 zeroIdx;
  /* RULE-EXCEPTION(user-approved): carrier copy of ctx used only to shape register allocation (found by the permuter farm) - see docs/RULE_EXCEPTIONS.md */
  void *carrier;
  carrier = ctx;
  fightTrainerGetStatus(0, (u16) fightTrainerGetStatus((u32) carrier, 0, 0x43, 0), 2, 0);
  flag = 0;
  itemCount = fightTrainerGetTemotiNormalItemDataIdAry(carrier, items, 0x14, 1);
  if (itemCount == 0)
  {
    return 0;
  }
  outCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, carrier, outPokemon, 1, 1);
  fightCount = fightFloorGetFightTrainerFightPokemonPtrAry(0, carrier, fightPokemon, 1, 1);
  for (outIdx = 0; outIdx < outCount; outIdx++)
  {
    other = outPokemon[outIdx];
    if (other == 0)
    {
      continue;
    }
    if (!fightOutPokemonCheckFightOut(other))
    {
      continue;
    }
    if (fn_80235714(carrier, other) == 1)
    {
      flag = 0;
    }
  }

  for (zeroIdx = 0; zeroIdx < 20; zeroIdx++)
  {
    values[zeroIdx] = 0;
  }

  for (itemIdx = 0; itemIdx < itemCount; itemIdx++)
  {
    item = items[itemIdx];
    if (item == 0)
    {
      continue;
    }
    if (!fn_80142984(item))
    {
      continue;
    }
    type = fightSeqGetItemType(item, param1);
    if (type == 7)
    {
      continue;
    }
    statusPtr = pokemonGetStatus(param1, 0, 0xD5, 0);
    usable = itemUse2PokemonSimulation(work, 0, statusPtr, item, 0) > 0;
    if (!usable)
    {
      continue;
    }
    if ((type == 2) || (type == 1))
    {
      if ((fightTrainerAiGetHpRatio(carrier, param1) == 1) && (!pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(param1))))
      {
        values[itemIdx] = fn_80239984(values[itemIdx], carrier, 0x2E);
        fn_80239EE8(0xEC65, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x2E);
      }
    }
    if ((type == 3) || (type == 1))
    {
      if ((!fightTrainerAiIsJoutaiNormal(carrier, param1)) && (!pokemonIsDarkPokemon(fightOutPokemonGetPokemonPtr(param1))))
      {
        values[itemIdx] = fn_80239984(values[itemIdx], carrier, 0x2F);
        fn_80239EE8(0xEC65, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x2F);
      }
    }
    if ((type == 5) && (!fn_80235714(carrier, param1)))
    {
      values[itemIdx] = fn_80239984(values[itemIdx], carrier, 0x30);
      fn_80239EE8(0xEC65, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0 & 0xFFFF, 0, item, 0x30);
    }
    if ((type == 4) && (fn_80236C80(carrier, param1) == 2))
    {
      values[itemIdx] = fn_80239984(values[itemIdx], carrier, 0x31);
      fn_80239EE8(0xEC65, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x31);
    }
    if ((type == 6) && (flag == 1))
    {
      values[itemIdx] = fn_80239984(values[itemIdx], carrier, 0x32);
      fn_80239EE8(0xEC65, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x32);
    }
    if ((fightTrainerAiGetPartData1C(carrier, param1) != 2) && (fightTrainerAiGetPartData1C(carrier, param1) != 3))
    {
      for (fightIdx = 0; fightIdx < fightCount; fightIdx++)
      {
        if (pokemonGetStatus(param1, 0, 0xD5, 0) == ((void *) fightPokemon[fightIdx]))
        {
          continue;
        }
        if ((fightTrainerAiGetPartData1C(carrier, param1) == 2) || (fightTrainerAiGetPartData1C(carrier, param1) == 3))
        {
          if (fightPokemonCheckFightOut(fightPokemon[fightIdx]) == 1)
          {
            values[itemIdx] = fn_80239984(values[itemIdx], carrier, 0x33);
            fn_80239EE8(0xEC65, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x33);
            break;
          }
        }
      }

    }
    fightTrainerAiLogText(0xEC65, 0xEC46, 0xF238, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x226, values[itemIdx]);
  }

  for (searchIdx = 0; searchIdx < itemCount; searchIdx++)
  {
    if (values[searchIdx] > 0)
    {
      break;
    }
  }

  if (searchIdx >= itemCount)
  {
    return 0;
  }
  best = fightTrainerAiGetValueAryMaxBanme(values, itemCount, 1);
  if (best < 0)
  {
    return 0;
  }
  item = items[searchIdx];
  if (item == 0)
  {
    return 0;
  }
  fightTrainerAiLogText(0xEC65, 0xEC46, 0xF238, carrier, fightOutPokemonGetPokemonPtr(param1), 0, 0, 0, item, 0x228, values[best]);
  fightOutPokemonCreateFightActionUseItem(param1, 0, 0x12, 0, lbl_80375D70, item, fightTargetGetTragetPtrToRelativeHostSideFightTargetId(param1, param2), -1, 0);
  return 1;
}

#endif /* !DASU_ONLY */
#endif /* !FIGHT_TRAINER_AI_IREKAE_EXACT_8024F8B4_ONLY */
