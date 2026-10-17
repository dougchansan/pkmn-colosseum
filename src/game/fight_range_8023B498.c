/**
 * @file fight_range_8023B498.c
 * @brief Fight/battle-AI tail candidate, 0x8023B498 - 0x8023CA9C (4 fns).
 *
 * The unit builds with -inline auto,deferred, which emits functions in
 * reverse source order. The functions are therefore written last-first
 * (fightTrainerAiGetWazaHitValue, fightTrainerAiGetWazaDamageValue, fn_8023C368, fn_8023B498), and fn_8023B498
 * inlines both of its fightTrainerAiGetWazaDamageValue calls as retail does (its two party
 * buffers at 0x40 and 0x60 are the two inlined copies of `party`).
 */
#include "dolphin/types.h"

typedef struct { u16 v[11]; } SpecialMoveList22;
typedef struct { u16 a, b; } U16Pair;

extern SpecialMoveList22 lbl_8027A408;
extern U16Pair lbl_8047E628;
extern U16Pair lbl_8047E62C;

extern u32 fightTrainerGetStatus();
extern u32 wazaGetStatus();
extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
extern u8 fightOutPokemonCheckFightOut();
extern u8 fn_80237F74();
extern u8 fightOutPokemonIsPokemonJoutaiDataId();
extern s32 fn_80239500();
extern u32 fightTrainerAiGetWazaTenkouValue();
extern u32 fightTrainerAiWazaHitNull();

/* The move/target validity check that all three range cases run before the
 * hit callback (absorb abilities, Flash Fire, Soundproof). A target that is
 * not out fails early; its return value shares the register of `valid`. */
static inline u8 aiWazaHitCheck(void* trainer, s32 pokemon, u32 move, u32 target) {
    s32 moveValue;
    u32 moveType;
    u8 valid;

    moveValue = fn_80239500(trainer, move);
    moveType = fightTrainerAiGetWazaTenkouValue(trainer, move, pokemon);
    valid = 1;
    if (fightOutPokemonCheckFightOut(target) == 0) {
        return 0;
    }
    if ((u16)move != 0 && (u16)move != 0x165) {
        if (fn_80237F74(trainer, target, 0xA) == 1 &&
            (u16)moveType == 0xD && (s16)moveValue != 0) {
            valid = 0;
        }
        if (fn_80237F74(trainer, target, 0xB) == 1 &&
            (u16)moveType == 0xB && (s16)moveValue != 0) {
            valid = 0;
        }
        if (fn_80237F74(trainer, target, 0x12) == 1 &&
            (u16)moveType == 0xA && fightOutPokemonIsPokemonJoutaiDataId(target, 7) == 0) {
            valid = 0;
        }
        if (fn_80237F74(trainer, target, 0x2B) == 1 &&
            (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
            valid = 0;
        }
    }
    return valid;
}

u32 fightTrainerAiGetWazaHitValue(void* trainer, s32 pokemon, u32 move, u32 target) {
    typedef u32 (*HitFunc)(void*, s32, u32, u32);
    u32 party[8];
    u32 trainerData;
    u32 partyCount;
    HitFunc hitFunc;
    u8 category;
    u32 result;
    u16 i;
    u8 valid;

    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    if ((u8)fightTrainerGetStatus(0, trainerData, 0x34, 0) == 1) {
        hitFunc = (HitFunc)wazaGetStatus(0, move, 0x1D, 0);
        if (hitFunc == 0) {
            hitFunc = (HitFunc)fightTrainerAiWazaHitNull;
        }
    } else {
        return 1;
    }
    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(
        0, trainer, party, 0, 1);
    category = (u8)wazaGetStatus(0, move, 5, 0);
    result = 0;
    switch (category) {
    case 0:
    case 3:
        valid = aiWazaHitCheck(trainer, pokemon, move, target);
        if (valid == 0) {
            return 0;
        }
        return hitFunc(trainer, pokemon, move, target);

    case 2:
    case 4:
    case 6:
        for (i = 0; i < (u16)partyCount; i++) {
            valid = aiWazaHitCheck(trainer, pokemon, move, target);
            if (valid == 0) {
                result = 0;
            } else {
                result = hitFunc(trainer, pokemon, move, party[i]);
            }
            if ((u8)result == 1) {
                return result;
            }
        }
        return result;

    case 1:
    case 5:
    case 7:
        valid = aiWazaHitCheck(trainer, pokemon, move, target);
        if (valid == 0) {
            result = 0;
            break;
        }
        result = hitFunc(trainer, pokemon, move, target);
        break;
    }
    return result;
}

u32 fightTrainerAiGetWazaDamageValue(void* trainer, s32 pokemon, u32 move, u32 target, u32 dispatch)
{
    typedef u32 (*DamageFunc)(void*, s32, u32, u32);
    extern u32 fightTrainerGetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u32 fightTrainerAiWazaDamageNull();
    u32 party[8];
    u32 trainerData;
    DamageFunc damageFunc;
    u32 partyCount;
    u8 category;
    u16 count;
    u16 i;
    u32 result;

    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    if ((u8)fightTrainerGetStatus(0, trainerData, 0x2d, 0) == 1) {
        damageFunc = (DamageFunc)wazaGetStatus(0, move, 0x1e, 0);
        if (damageFunc == 0) {
            damageFunc = fightTrainerAiWazaDamageNull;
        }
    } else {
        return 0;
    }

    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(
        0, trainer, party, 0, 1);
    category = (u8)wazaGetStatus(0, move, 5, 0);
    result = 0;

    if ((u8)dispatch == 1) {
        switch (category) {
        case 0:
        case 3:
            result = damageFunc(trainer, pokemon, move, target);
            break;

        case 2:
        case 4:
        case 6:
            count = (u16)partyCount;
            for (i = 0; i < count; i++) {
                result += damageFunc(trainer, pokemon, move, target);
            }
            break;

        case 1:
        case 5:
        case 7:
            result = damageFunc(trainer, pokemon, move, target);
            break;
        }
    } else {
        result = damageFunc(trainer, pokemon, move, target);
    }

    return result;
}

/* Trivial constant return. */
s32 fn_8023C368(void) { return 0; }

s32 fn_8023B498(void* ctx, s32 userPoke, u16 moveCount, u16* moveList,
                u16 allyCount, u32* allyArray, u32* outMoveId, s16* outValue) {
    extern u32 fightTrainerGetStatus(void*, u16, u32, u32);
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fn_80237F74();
    extern u32 fightTrainerAiIsPokemonJoutai();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void pokemonGetMezamerupower();
    extern u8 fightAiCollectWazaToSide();
    extern u32 fightTrainerAiIsJoutaiNormal();
    extern u8 fightTrainerAiGetPartData1C();
    extern u8 fn_80235714();
    extern u32 fightTrainerAiGetSoubiItem();
    extern s16 fightOutPokemonGetFightEntryId();
    extern u16 fn_802376EC();
    extern u8 fn_8023943C();
    extern u32 fn_80239984();
    extern u8 fn_80239EE8();
    extern u8 fightTrainerAiLogText();
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern u8 fn_8000815C();
    extern u32 fightTrainerAiAddValue();
    extern s32 fightTrainerAiGetValueAryMaxBanme(s32* valueAry, u16 count, u8 useRandom);
    extern u16 GSrand();

    u8 randRange;
    s32 valueAry[8];
    s32 threshAry[8];
    s16 candArr[10];
    u32 oppAry[8];
    u16 specialArr[11];
    u8 flagB[8];
    u8 flagA[8];
    u16 pairArr[4];
    u16 j;
    s32 n;
    u32 oppCount;
    u8 foundB;
    s32 delta;
    u16 k2;
    u32 ally2;
    u32 allyTrainer;
    u16 k;
    u16 i;
    s32 candCount;
    u16 v2;
    u32 species;
    u16 cat;
    u8 foundA;
    s32 maxVal;
    u32 allyMoveCount;
    s32 bestThresh;
    u32 ally;
    u32 hit;
    s32 dmg;
    s16 valid;
    u32 move;
    u16 rngVal;
    u32 userTrainer;
    u16 mv;
    s32 bestIdx;
    u16 slot;
    u32 opp;
    s16 val;
    u16 outA;
    u16 outB;

    {
        v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
        randRange = (u8)fightTrainerGetStatus(0, v2, 0x38, 0);
    }

    /* RULE-EXCEPTION(user-approved): struct-copy casts stand in for local array initializers whose data belongs to other data units - see docs/RULE_EXCEPTIONS.md */
    *(SpecialMoveList22*)specialArr = lbl_8027A408;
    *(U16Pair*)&pairArr[0] = lbl_8047E628;
    *(U16Pair*)&pairArr[2] = lbl_8047E62C;

    oppCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, oppAry, 1, 1);
    userTrainer = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, userPoke);

    for (i = 0; i < (u16)oppCount; i++) {
        opp = oppAry[i];
        if (opp == 0) continue;
        if (!fightOutPokemonCheckFightOut((void*)opp)) continue;
        if (userPoke == opp) continue;

        if ((u8)fn_80237F74(ctx, opp, 0x12) == 1 &&
            (u8)fightTrainerAiIsPokemonJoutai(ctx, opp, 0x3a) == 0) {
            candCount = 0;
            for (n = 0; (u16)n < 11; n++) {
                mv = specialArr[(u16)n];
                if (mv == 0xed) {
                    pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(userPoke), &outA, &outB);
                    if (outB != 10) {
                        continue;
                    }
                }
                if ((u8)fightAiCollectWazaToSide(ctx, userPoke, mv, &val, 1) != 0 && val >= 0) {
                    candArr[(s16)candCount++] = val;
                }
            }
            if ((s16)candCount > 0) {
                val = candArr[GSrand() % (s16)candCount];
                if (outMoveId) *outMoveId = opp;
                if (outValue) *outValue = val;
                return -1;
            }
        }

        if (((u8)fn_80237F74(ctx, opp, 0x3e) == 1 || (u8)fn_80237F74(ctx, opp, 0x3f) == 1) &&
            (u8)fightTrainerAiIsJoutaiNormal(ctx, opp) == 1) {
            candCount = 0;
            for (n = 0; (u16)n < 4; n++) {
                u16 mv = pairArr[(u16)n];
                if ((u8)fightAiCollectWazaToSide(ctx, userPoke, mv, &val, 1) != 0 && val >= 0) {
                    candArr[(s16)candCount++] = val;
                }
            }
            if ((s16)candCount > 0) {
                val = candArr[GSrand() % (s16)candCount];
                if (outMoveId) *outMoveId = opp;
                if (outValue) *outValue = val;
                return -1;
            }
        }

        {
            foundA = 0;
            if ((u8)fn_80237F74(ctx, opp, 0x36) == 1) foundA = 1;
            if ((u8)fightTrainerAiGetPartData1C(ctx, opp) == 2) {
                if ((u8)fn_80237F74(ctx, opp, 0x25) == 1 || (u8)fn_80237F74(ctx, opp, 0x4a) == 1) {
                    foundA = 1;
                }
            }
            if ((u8)fightTrainerAiGetPartData1C(ctx, opp) == 3) {
                if ((u8)fn_80237F74(ctx, userPoke, 0x25) == 1 || (u8)fn_80237F74(ctx, userPoke, 0x4a) == 1) {
                    foundA = 1;
                }
            }
            if (foundA == 1) {
                if ((u8)fightAiCollectWazaToSide(ctx, userPoke, 0x11d, &val, 1) != 0 && val >= 0) {
                    if (outMoveId) *outMoveId = opp;
                    if (outValue) *outValue = val;
                    return -1;
                }
            }
        }

        {
            foundB = 0;
            if ((u8)fn_80235714(ctx, opp) == 0) {
                if ((u8)fn_80237F74(ctx, opp, 0x14) == 1) foundB = 1;
                cat = (u16)fightTrainerAiGetSoubiItem(ctx, opp);
                if (cat == 8 || cat == 9) foundB = 1;
            }
            if (foundB == 1) {
                if ((u8)fightAiCollectWazaToSide(ctx, userPoke, 0xcf, &val, 1) != 0 && val >= 0) {
                    if (outMoveId) *outMoveId = opp;
                    if (outValue) *outValue = val;
                    return -1;
                }
            }
        }

        if ((u8)fn_80235714(ctx, opp) == 1 && (u8)fn_80235714(ctx, userPoke) == 0) {
            if ((u8)fightAiCollectWazaToSide(ctx, userPoke, 0xf4, &val, 1) != 0 && val >= 0) {
                if (outMoveId) *outMoveId = opp;
                if (outValue) *outValue = val;
                return -1;
            }
        }
    }

    for (slot = 0; slot < 8; slot++) {
        valueAry[slot] = 0;
        threshAry[slot] = 0;
        flagB[slot] = 0;
        flagA[slot] = 0;
    }

    bestThresh = 0;
    for (k = 0; k < allyCount; k++) {
        ally = allyArray[k];
        if (ally == 0) continue;
        if (fightOutPokemonGetFightEntryId((void*)ally) < 0) continue;

        {
            allyMoveCount = (u16)fn_802376EC(ctx, ally);
            maxVal = 0;
            for (j = 0; j < moveCount; j++) {
                move = moveList[j];
                if (move == 0 || move == 0x165) continue;
                hit = fightTrainerAiGetWazaHitValue(ctx, userPoke, move, ally);
                if ((u8)hit == 1) flagA[k] = 1;
                if ((u8)fn_8023943C(ctx, move, 1) == 0) continue;

                dmg = fightTrainerAiGetWazaDamageValue(ctx, userPoke, move, ally, 1);
                if (maxVal < dmg) maxVal = dmg;
                if ((s32)allyMoveCount < (s32)fightTrainerAiGetWazaDamageValue(ctx, userPoke, move, ally, 0)) {
                    if ((u8)hit == 1) flagB[k] = 1;
                }
            }
            threshAry[k] = maxVal;
            if (bestThresh < threshAry[k]) bestThresh = threshAry[k];
        }
    }

    for (k2 = 0; k2 < allyCount; k2++) {
        ally2 = allyArray[k2];
        if (ally2 == 0) continue;
        valid = fightOutPokemonGetFightEntryId((void*)ally2);
        if (valid < 0) continue;

        {
            allyTrainer = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ally2);

            fightTrainerAiLogText(0xEC6C, 0xEC2C, 0xEC2D, userTrainer, fightOutPokemonGetPokemonPtr(userPoke),
                        allyTrainer, fightOutPokemonGetPokemonPtr(ally2),
                        0, 0, 0x227, valueAry[k2]);
            species = (u16)valid;

            if ((u8)fightTrainerGetStatus(ctx, species, 0x52, 0) == 1) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x34);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x34);
            }
            if ((u8)fightTrainerGetStatus(ctx, species, 0x53, 0) == 1) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x35);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x35);
            }
            if ((u8)fightTrainerGetStatus(ctx, species, 0x55, 0) == 1) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x36);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x36);
            }
            if (bestThresh <= threshAry[k2]) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x37);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x37);
            }
            if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0, ally2) == 1) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x38);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x38);
            }
            if ((s32)fightTrainerGetStatus(ctx, species, 0x56, 0) == 1) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x39);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x39);
            }
            if (flagA[k2] == 0) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x3a);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x3a);
            }
            if (flagB[k2] == 1) {
                valueAry[k2] = fn_80239984(valueAry[k2], ctx, 0x3b);
                fn_80239EE8(0xEC6C, userTrainer, fightOutPokemonGetPokemonPtr(userPoke), allyTrainer,
                            fightOutPokemonGetPokemonPtr(ally2), 0, 0, 0x3b);
            }
            if ((u8)fn_8000815C() == 1) {
                rngVal = GSrand();
                delta = (s32)(rngVal % ((s32)randRange * 2 + 1)) - randRange;
                valueAry[k2] = fightTrainerAiAddValue(valueAry[k2], delta);
                fightTrainerAiLogText(0xEC6C, 0xEC2C, 0xEC2D, userTrainer, fightOutPokemonGetPokemonPtr(userPoke),
                            allyTrainer, fightOutPokemonGetPokemonPtr(ally2),
                            0, 0, 0x225, delta);
            }
            fightTrainerAiLogText(0xEC6C, 0xEC2C, 0xEC2D, userTrainer, fightOutPokemonGetPokemonPtr(userPoke),
                        allyTrainer, fightOutPokemonGetPokemonPtr(ally2),
                        0, 0, 0x226, valueAry[k2]);
        }
    }

    bestIdx = fightTrainerAiGetValueAryMaxBanme(valueAry, allyCount, 1);
    if (bestIdx < 0) {
        return -1;
    }
    allyTrainer = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allyArray[bestIdx]);
    fightTrainerAiLogText(0xEC6C, 0xEC2C, 0xEC2D, userTrainer, fightOutPokemonGetPokemonPtr(userPoke),
                allyTrainer, fightOutPokemonGetPokemonPtr(allyArray[bestIdx]),
                0, 0, 0x228, valueAry[bestIdx]);
    return bestIdx;
}
