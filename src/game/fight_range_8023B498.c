/**
 * @file fight_range_8023B498.c
 * @brief Fight/battle-AI tail candidate, 0x8023B498 - 0x8023CA9C (4 fns).
 */
#include "dolphin/types.h"

typedef struct { u16 v[11]; } SpecialMoveList22;
typedef struct { u16 a, b; } U16Pair;

extern SpecialMoveList22 lbl_8027A408;
extern U16Pair lbl_8047E628;
extern U16Pair lbl_8047E62C;

#pragma optimize_for_size on
s32 fn_8023B498(void* ctx, u32 userPoke, u16 moveCount, u16* moveList,
                u16 allyCount, u32* allyArray, u32* outMoveId, s16* outValue) {
    extern u32 fightTrainerGetStatus();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(int a, void* ctx, u32* buf, int b, int c);
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32 context, u32 slot);
    extern u8 fightOutPokemonCheckFightOut(void* poke);
    extern u8 fn_80237F74(void* ctx, u32 poke, u32 fieldId);
    extern u32 fn_80236BFC(void* ctx, u32 poke, u32 param3);
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void pokemonGetMezamerupower();
    extern u8 fn_80236584(void* ctx, u32 poke, u32 moveId, s16* out, u32 flag);
    extern u32 fn_80237310(void* ctx, u32 param);
    extern u8 fn_8023785C(void* ctx, u32 param);
    extern u8 fn_80235714();
    extern u32 fn_8023831C(void* ctx, u32 param);
    extern s16 fightOutPokemonGetFightEntryId(void* ctx);
    extern u16 fn_802376EC(void* ctx, u32 param);
    extern u32 fn_8023C530();
    extern u8 fn_8023943C();
    extern u32 wazaGetStatus();
    extern u32 fn_80239984(u32 handle, void* ctx, u32 seq);
    extern u8 fn_80239EE8(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8);
    extern u8 fn_8023A118(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8,
                           u16 p9, u16 p10, u32 p11);
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern u8 fn_8000815C(void);
    extern u32 fightTrainerAiAddValue(s32 value, s32 delta);
    extern s32 fightTrainerAiGetValueAryMaxBanme(s32* valueAry, u16 count, u8 useRandom);
    extern s32 fightTrainerAiWazaDamageNull(void*, u32, u16, u32);
    extern u16 fn_800E0C54(void);

    typedef s32 (*Handler)();

    u8 flag_10c;
    s32 valueAry[8];
    s32 threshAry[8];
    s16 candArr[10];
    u32 buf98[8];
    u16 specialArr[11];
    u8 flagB[8];
    u8 flagA[8];
    u16 pairArr[4];
    u32 oppCount;
    u32 poke2Ctx;
    u32 allyCtx;
    u32 allyH;
    u16 i;
    u32 cur;
    u16 k;
    s16 val;
    u16 outA;
    u16 outB;
    u32 allyLimit;
    s32 bestThresh;
    s32 countWork;
    s32 scanWork;
    s32 bestIdx;
    u32 valueWork;

    valueWork = (u32)outValue;

    {
        u16 v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
        flag_10c = (u8)fightTrainerGetStatus(0, v2, 0x38, 0);
    }

    *(volatile SpecialMoveList22*)specialArr = lbl_8027A408;
    *(volatile U16Pair*)&pairArr[0] = lbl_8047E628;
    *(volatile U16Pair*)&pairArr[2] = lbl_8047E62C;

    cur = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf98, 1, 1);
    oppCount = cur;
    poke2Ctx = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, userPoke);

    for (i = 0; i < (u16)oppCount; i++) {
        cur = buf98[i];
        if (cur == 0) continue;
        if (!fightOutPokemonCheckFightOut((void*)cur)) continue;
        if (userPoke == cur) continue;

        if ((u8)fn_80237F74(ctx, cur, 0x12) == 1 &&
            (u8)fn_80236BFC(ctx, cur, 0x3a) == 0) {
            countWork = 0;
            for (scanWork = 0; (u16)scanWork < 11; scanWork++) {
                u16 mv = specialArr[(u16)scanWork];
                if (mv == 0xed) {
                    pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(userPoke), &outA, &outB);
                    if (outB != 10) {
                        continue;
                    }
                }
                if ((u8)fn_80236584(ctx, userPoke, mv, &val, 1) != 0 && val >= 0) {
                    candArr[(s16)countWork++] = val;
                }
            }
            if ((s16)countWork > 0) {
                val = candArr[fn_800E0C54() % (s16)countWork];
                if (outMoveId) *outMoveId = cur;
                if (valueWork) *(s16*)valueWork = val;
                return -1;
            }
        }

        if (((u8)fn_80237F74(ctx, cur, 0x3e) == 1 || (u8)fn_80237F74(ctx, cur, 0x3f) == 1) &&
            (u8)fn_80237310(ctx, cur) == 1) {
            scanWork = 0;
            for (countWork = 0; (u16)countWork < 4; countWork++) {
                u16 mv = pairArr[(u16)countWork];
                if ((u8)fn_80236584(ctx, userPoke, mv, &val, 1) != 0 && val >= 0) {
                    candArr[(s16)scanWork++] = val;
                }
            }
            if ((s16)scanWork > 0) {
                val = candArr[fn_800E0C54() % (s16)scanWork];
                if (outMoveId) *outMoveId = cur;
                if (valueWork) *(s16*)valueWork = val;
                return -1;
            }
        }

        {
            u8 flag23 = 0;
            if ((u8)fn_80237F74(ctx, cur, 0x36) == 1) flag23 = 1;
            if ((u8)fn_8023785C(ctx, cur) == 2) {
                if ((u8)fn_80237F74(ctx, cur, 0x25) == 1 || (u8)fn_80237F74(ctx, cur, 0x4a) == 1) {
                    flag23 = 1;
                }
            }
            if ((u8)fn_8023785C(ctx, cur) == 3) {
                if ((u8)fn_80237F74(ctx, userPoke, 0x25) == 1 || (u8)fn_80237F74(ctx, userPoke, 0x4a) == 1) {
                    flag23 = 1;
                }
            }
            if (flag23 == 1) {
                if ((u8)fn_80236584(ctx, userPoke, 0x11d, &val, 1) != 0 && val >= 0) {
                    if (outMoveId) *outMoveId = cur;
                    if (valueWork) *(s16*)valueWork = val;
                    return -1;
                }
            }
        }

        {
            u8 flag23 = 0;
            if ((u8)fn_80235714(ctx, cur) == 0) {
                u16 cat;
                if ((u8)fn_80237F74(ctx, cur, 0x14) == 1) flag23 = 1;
                cat = (u16)fn_8023831C(ctx, cur);
                if (cat == 8 || cat == 9) flag23 = 1;
            }
            if (flag23 == 1) {
                if ((u8)fn_80236584(ctx, userPoke, 0xcf, &val, 1) != 0 && val >= 0) {
                    if (outMoveId) *outMoveId = cur;
                    if (valueWork) *(s16*)valueWork = val;
                    return -1;
                }
            }
        }

        if ((u8)fn_80235714(ctx, cur) == 1 && (u8)fn_80235714(ctx, userPoke) == 0) {
            if ((u8)fn_80236584(ctx, userPoke, 0xf4, &val, 1) != 0 && val >= 0) {
                if (outMoveId) *outMoveId = cur;
                if (valueWork) *(s16*)valueWork = val;
                return -1;
            }
        }
    }

    for (k = 0; k < 8; k++) {
        valueAry[k] = 0;
        threshAry[k] = 0;
        flagB[k] = 0;
        flagA[k] = 0;
    }

    allyLimit = (u16)allyCount;
    bestThresh = 0;
    for (k = 0; k < allyLimit; k++) {
        allyH = allyArray[k];
        if (allyH == 0) continue;
        if (fightOutPokemonGetFightEntryId((void*)allyH) < 0) continue;

        {
            u32 allyMoveCount = (u16)fn_802376EC(ctx, allyH);
            s32 maxVal = 0;
            u16 j;
            for (j = 0; j < moveCount; j++) {
                u32 matched;
                Handler h;
                valueWork = moveList[j];
                if ((u16)valueWork == 0 || (u16)valueWork == 0x165) continue;
                matched = fn_8023C530(ctx, userPoke, (u16)valueWork, allyH);
                if ((u8)matched == 1) flagA[k] = 1;
                if ((u8)fn_8023943C(ctx, (u16)valueWork, 1) == 0) continue;

                {
                    s32 val;
                    u16 v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
                    u32 buf60[8];
                    u32 hitCount;
                    u8 category;
                    u16 c;

                    if ((u8)fightTrainerGetStatus(0, v2, 0x2d, 0) != 1) goto damage_one_disabled;
                    h = (Handler)wazaGetStatus(0, (u16)valueWork, 0x1e, 0);
                    if (h) goto damage_one_ready;
                    h = (Handler)fightTrainerAiWazaDamageNull;
                    goto damage_one_ready;
damage_one_disabled:
                    val = 0;
                    goto damage_one_done;
damage_one_ready:
                    hitCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf60, 0, 1);
                    category = (u8)wazaGetStatus(0, (u16)valueWork, 5, 0);
                    val = 0;
                    switch (category) {
                    case 0:
                    case 3:
                        val = h(ctx, userPoke, (u16)valueWork, allyH);
                        break;
                    case 2:
                    case 4:
                    case 6:
                        for (c = 0; c < (u16)hitCount; c++) {
                            val += h(ctx, userPoke, (u16)valueWork, allyH);
                        }
                        break;
                    case 1:
                    case 5:
                    case 7:
                        val = h(ctx, userPoke, (u16)valueWork, allyH);
                        break;
                    }
damage_one_done:
                    if (maxVal < val) maxVal = val;
                }

                {
                    u16 v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
                    s32 val2;
                    u32 buf40[8];

                    if ((u8)fightTrainerGetStatus(0, v2, 0x2d, 0) != 1) goto damage_two_disabled;
                    h = (Handler)wazaGetStatus(0, (u16)valueWork, 0x1e, 0);
                    if (h) goto damage_two_ready;
                    h = (Handler)fightTrainerAiWazaDamageNull;
                    goto damage_two_ready;
damage_two_disabled:
                    val2 = 0;
                    goto damage_two_done;
damage_two_ready:
                    fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf40, 0, 1);
                    (void)wazaGetStatus(0, (u16)valueWork, 5, 0);
                    val2 = h(ctx, userPoke, (u16)valueWork, allyH);
damage_two_done:
                    if ((s32)allyMoveCount < val2) {
                        if ((u8)matched == 1) flagB[k] = 1;
                    }
                }
            }
            threshAry[k] = maxVal;
            if (bestThresh < threshAry[k]) bestThresh = threshAry[k];
        }
    }

    countWork = (s32)flag_10c * 2 + 1;
    for (k = 0; k < allyCount; k++) {
        s16 valid;
        allyH = allyArray[k];
        if (allyH == 0) continue;
        valid = fightOutPokemonGetFightEntryId((void*)allyH);
        if (valid < 0) continue;

        {
            u16 species = (u16)valid;
            allyCtx = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allyH);

            fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                        allyCtx, fightOutPokemonGetPokemonPtr(allyH),
                        0, 0, 0x227, valueAry[k]);

            if ((u8)fightTrainerGetStatus(ctx, species, 0x52, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x34);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x34);
            }
            if ((u8)fightTrainerGetStatus(ctx, species, 0x53, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x35);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x35);
            }
            if ((u8)fightTrainerGetStatus(ctx, species, 0x55, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x36);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x36);
            }
            if (bestThresh <= threshAry[k]) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x37);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x37);
            }
            if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0, allyH) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x38);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x38);
            }
            if ((s32)fightTrainerGetStatus(ctx, species, 0x56, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x39);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x39);
            }
            if (flagA[k] == 0) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x3a);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x3a);
            }
            if (flagB[k] == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x3b);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x3b);
            }
            if ((u8)fn_8000815C() == 1) {
                u16 rngVal = fn_800E0C54();
                s32 delta = (s32)(rngVal % countWork) - flag_10c;
                valueAry[k] = fightTrainerAiAddValue(valueAry[k], delta);
                fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                            allyCtx, fightOutPokemonGetPokemonPtr(allyH),
                            0, 0, 0x225, delta);
            }
            fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                        allyCtx, fightOutPokemonGetPokemonPtr(allyH),
                        0, 0, 0x226, valueAry[k]);
        }
    }

    bestIdx = fightTrainerAiGetValueAryMaxBanme(valueAry, allyCount, 1);
    if (bestIdx < 0) {
        return -1;
    }
    {
        allyCtx = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allyArray[bestIdx]);
        fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                    allyCtx, fightOutPokemonGetPokemonPtr(allyArray[bestIdx]),
                    0, 0, 0x228, valueAry[bestIdx]);
    }
    return bestIdx;
}
#pragma optimize_for_size reset
/* Trivial constant return. */
s32 fn_8023C368(void) { return 0; }
u32 fn_8023C370(u32 trainer, u32 pokemon, u32 move, u32 target, u32 dispatch)
{
    typedef u32 (*DamageFunc)(u32, u32, u32, u32);
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
u32 fn_8023C530(s32 trainer, s32 pokemon, s32 move, s32 target) {
    u8 valid;
    typedef u32 (*HitFunc)(u32, u32, u32, u32);
    extern u32 fightTrainerGetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fn_80237F74();
    extern u8 fn_802026E4();
    extern s32 fn_80239500();
    extern u32 fn_802395C8();
    extern u32 fightTrainerAiWazaHitNull();
    u32 party[8];
    u32 trainerData;
    u32 partyCount;
    HitFunc hitFunc;
    u8 category;
    u32 result;

    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    if ((u8)fightTrainerGetStatus(0, trainerData, 0x34, 0) == 1) {
        hitFunc = (HitFunc)wazaGetStatus(0, move, 0x1D, 0);
        if (hitFunc == 0) {
            hitFunc = fightTrainerAiWazaHitNull;
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
    case 3: {
        u8 valid;
        s32 moveValue;
        u32 moveType;

        moveValue = fn_80239500(trainer, move);
        moveType = fn_802395C8(trainer, move, pokemon);
        valid = 1;
        if (fightOutPokemonCheckFightOut(target) == 0) {
            valid = 0;
        } else if ((u16)move != 0 && (u16)move != 0x165) {
            if (fn_80237F74(trainer, target, 0xA) == 1 &&
                (u16)moveType == 0xD && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0xB) == 1 &&
                (u16)moveType == 0xB && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x12) == 1 &&
                (u16)moveType == 0xA && fn_802026E4(target, 7) == 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x2B) == 1 &&
                (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
                valid = 0;
            }
        }
        if (valid == 0) {
            return 0;
        }
        return hitFunc(trainer, pokemon, move, target);
    }

    case 2:
    case 4:
    case 6: {
        u16 i;
        u16 count;
        u8 valid;
        s32 moveValue;
        u32 moveType;

        count = (u16)partyCount;
        for (i = 0; i < count; i++) {
            moveValue = fn_80239500(trainer, move);
            moveType = fn_802395C8(trainer, move, pokemon);
            valid = 1;
            if (fightOutPokemonCheckFightOut(target) == 0) {
                valid = 0;
            } else if ((u16)move != 0 && (u16)move != 0x165) {
                if (fn_80237F74(trainer, target, 0xA) == 1 &&
                    (u16)moveType == 0xD && (s16)moveValue != 0) {
                    valid = 0;
                }
                if (fn_80237F74(trainer, target, 0xB) == 1 &&
                    (u16)moveType == 0xB && (s16)moveValue != 0) {
                    valid = 0;
                }
                if (fn_80237F74(trainer, target, 0x12) == 1 &&
                    (u16)moveType == 0xA &&
                    fn_802026E4(target, 7) == 0) {
                    valid = 0;
                }
                if (fn_80237F74(trainer, target, 0x2B) == 1 &&
                    (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
                    valid = 0;
                }
            }
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
    }

    case 1:
    case 5:
    case 7: {
        s32 moveValue;
        u32 moveType;

        moveValue = fn_80239500(trainer, move);
        moveType = fn_802395C8(trainer, move, pokemon);
        valid = 1;
        if (fightOutPokemonCheckFightOut(target) == 0) {
            valid = 0;
        } else if ((u16)move != 0 && (u16)move != 0x165) {
            if (fn_80237F74(trainer, target, 0xA) == 1 &&
                (u16)moveType == 0xD && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0xB) == 1 &&
                (u16)moveType == 0xB && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x12) == 1 &&
                (u16)moveType == 0xA && fn_802026E4(target, 7) == 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x2B) == 1 &&
                (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
                valid = 0;
            }
        }
        if (valid == 0) {
            result = 0;
            break;
        }
        result = hitFunc(trainer, pokemon, move, target);
        break;
    }
    }
    return result;
}
