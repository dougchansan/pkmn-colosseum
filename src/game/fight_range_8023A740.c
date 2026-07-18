/**
 * @file fight_range_8023A740.c
 * @brief Strict target-order island, 0x8023A740 - 0x8023B498 (1 fn).
 */
#include "dolphin/types.h"

extern u32 fightOutPokemonGetPokemonPtr();
extern u16 fn_800E0C54(void);
extern s32 fightTrainerAiGetValueAryMaxBanme(s32*, u16, u8);
extern s32 fightTrainerAiAddValue(s32, s32);

#pragma optimize_for_size on
#pragma opt_lifetimes off
#define AI_SCORE(code)                                                        \
    do {                                                                      \
        score[i] = fn_80239984(score[i], trainer, (code));                    \
        fn_80239EE8(0xEC64, trainerPtr,                                       \
                    fightOutPokemonGetPokemonPtr(pokemon), 0, 0, move, 0,    \
                    (code));                                                  \
    } while (0)

#define AI_SCORE_VALUE(code, value)                                           \
    do {                                                                      \
        u8 scoreValue = (u8)(value);                                          \
        score[i] = fn_802398E4(score[i], scoreValue, trainer, (code));        \
        fn_80239A40(0xEC64, trainer,                                          \
                    fightOutPokemonGetPokemonPtr(pokemon), 0, 0, move, 0,    \
                    (code), scoreValue);                                      \
    } while (0)

#pragma opt_propagation off
s32 fn_8023A740(u32 trainer, u32 pokemon, u32 moveCount,
                 register u16* moveList,
                 s16* volatile moveData, u32 target, u32 unused) {
    u16 i;
    u32 move;
    u16 k;
    u8 sharedValue;
    u16 new_var;
    s32 data;
    u32 j;
    s32 maxDamageFirst;
    s32 maxMetric;
    u32 trainerPtr;
    typedef s32 (*AiMoveFunc)(u32, u32, u32, u32);
    extern s32 fightTrainerAiGetValueAryMaxBanme();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u16 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus(u32, u16, u32, u32);
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fn_80236C80();
    extern u16 fn_802376EC();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u32 wazaGetStatus();
    extern s32 fightTrainerAiWazaDamageNull();
    extern s32 fightTrainerAiWazaValueNull();
    extern s32 fightTrainerAiWazaValueToriaezutukae();
    extern s32 fn_802395C8();
    extern u32 fn_8023793C();
    extern u8 fn_8023943C();
    extern u32 fn_80239244();
    extern u8 fn_802393A0(u32, u32);
    extern u8 fn_80239498();
    extern s32 fn_80239500();
    extern u8 fn_8023C530(s32, u32, u32, u32);
    extern u8 fn_8023753C();
    extern u8 fn_802392A8();
    extern u8 fn_80238060(u32, u32, u8);
    extern u8 fn_80239154();
    extern u8 fn_8000815C();
    extern u16 fightFloorGetValidFightOutPokemonCount();
    extern s32 fn_802398E4();
    extern s32 fn_80239984();
    extern void fn_80239A40();
    extern void fn_80239EE8();
    extern void fn_8023A118();

    s32 score[10];
    s32 damage[10];
    s32 difference[10];
    s32 moveMetric[10];
    u32 scratch[8];
    u16 trainerValue16;
    u32 side;
    u16 reserve16;
    u16 reserve17;
    u16 reserve18;
    u32 trainerData;
    u32 trainerValue;
    u16 baseDamage;
    struct {
        s32 value;
    } maxDamage;
    u16 damageIndex;
    s32 maxDifference;
    u16 initIndex;
    u32 moveType;
    u32 moveKind;
    struct {
        AiMoveFunc value;
    } damageFunc;
    AiMoveFunc valueFunc;
    AiMoveFunc checkFunc;
    s32 damageValue;
    s32 scoreValueTemp;
    s32 delta;
    s32 selected;
    s32 randomRange;
    u8 randomBase;

    (void)unused;
    side = fightTargetGetPtrAsNowFightType(3, trainer);
    reserve16 = (u16)fightFloorGetStatus(0, 0, 0x16, 0);
    reserve17 = (u16)fightFloorGetStatus(0, 0, 0x17, 0);
    reserve18 = (u16)fightFloorGetStatus(0, 0, 0x18, 0);
    new_var = (u16)fightTrainerGetStatus(
        0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
    trainerData = new_var;
    randomBase = (u8)fightTrainerGetStatus(0, trainerData, 0x38, 0);
    trainerPtr = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);
    trainerValue = fn_80236C80(trainer, pokemon);
    fightSideGetHikaeFightPokemonNum(side, reserve16, reserve17, reserve18);

    for (initIndex = 0; initIndex < 10; initIndex++) {
        score[initIndex] = 0;
        damage[initIndex] = 0;
        difference[initIndex] = 0;
        moveMetric[initIndex] = 0;
    }

    baseDamage = fn_802376EC(trainer, target);
    maxDamageFirst = -0xFFFF;
    maxDifference = -0xFFFF;
    maxMetric = 0;

    for (damageIndex = 0; damageIndex < (u16)moveCount; damageIndex++) {
        i = moveList[damageIndex];
        if (i == 0 || i == 0x165) {
            continue;
        }

        if ((u8)fightTrainerGetStatus(
                0,
                (u16)fightTrainerGetStatus(
                    0,
                    (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0),
                    2, 0),
                0x2D, 0) != 1) {
            goto damage_zero;
        }
        damageFunc.value = (AiMoveFunc)wazaGetStatus(0, i, 0x1E, 0);
        if (damageFunc.value != 0) {
            goto damage_call;
        }
        damageFunc.value = fightTrainerAiWazaDamageNull;
        goto damage_call;
damage_zero:
        damageValue = 0;
        goto damage_done;
damage_call:
        fightFloorGetFightTrainerFightOutPokemonPtrAry(
            0, trainer, scratch, 0, 1);
        wazaGetStatus(0, i, 5, 0);
        damageValue = damageFunc.value(trainer, pokemon, i, target);
damage_done:
        damage[damageIndex] = damageValue;

        difference[damageIndex] =
            (s32)baseDamage - damage[damageIndex];
        if (fn_8023943C(trainer, i, 1) == 1) {
            if (maxDamageFirst < damage[damageIndex]) {
                maxDamageFirst = damage[damageIndex];
            }
            if (difference[damageIndex] <= 0) {
                if (maxDifference < difference[damageIndex]) {
                    maxDifference = difference[damageIndex];
                }
                moveMetric[damageIndex] = fn_802393A0(trainer, i);
                if (maxMetric < moveMetric[damageIndex]) {
                    maxMetric = moveMetric[damageIndex];
                }
            }
        }
    }

    maxDamage.value = maxDamageFirst;
    randomRange = randomBase * 2 + 1;
    trainerValue16 = (u8)trainerValue;

    for (i = 0; i < (u16)moveCount; i++) {
        move = moveList[i];
        if (move == 0) {
            continue;
        }
        data = moveData[i];
        if (data < 0) {
            continue;
        }

        moveType = fn_802395C8(trainer, move, pokemon);
        moveKind = fn_80239244(trainer, move);
        checkFunc = (AiMoveFunc)wazaGetStatus(0, move, 0x1C, 0);
        if ((u8)fightTrainerGetStatus(
                0,
                (u16)fightTrainerGetStatus(
                    0,
                    (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0),
                    2, 0),
                0x32, 0) != 1) {
            goto value_zero;
        }
        valueFunc = (AiMoveFunc)wazaGetStatus(0, move, 0x1C, 0);
        if (valueFunc != 0) {
            goto value_call;
        }
        valueFunc = fightTrainerAiWazaValueNull;
        goto value_call;
value_zero:
        scoreValueTemp = 0;
        goto value_done;
value_call:
        scoreValueTemp = valueFunc(trainer, pokemon, move, target);
value_done:
        score[i] = scoreValueTemp;

        fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                    trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0, 0,
                    move, 0, 0x227, score[i]);

        if (fn_8023C530(trainer, pokemon, move, target) == 1) {
            if (checkFunc == fightTrainerAiWazaValueToriaezutukae) {
                AI_SCORE(0x3E);
            } else {
                AI_SCORE(0x3C);
            }
        } else {
            AI_SCORE(0x3D);
        }

        if ((u16)moveType != 9 && fn_8023943C(trainer, move, 1) == 1) {
            if ((u16)fn_8023793C(trainer, target, moveType,
                                  fn_80239500(trainer, move)) == 0x42) {
                AI_SCORE(0x3F);
            }
            if ((u16)fn_8023793C(trainer, target, moveType,
                                  fn_80239500(trainer, move)) == 0x43) {
                AI_SCORE(0x40);
            }
        }

        for (j = 0; (u16)j < 3; j++) {
            u8 effect = fn_80239498(trainer, move, j & 0xFF);
            if (effect != 0) {
                extern u32 fightTrainerGetStatus(u32, u16, u32, u32);
                sharedValue = (u8)fightTrainerGetStatus(
                    0, trainerValue16, 0xC, effect);
                AI_SCORE_VALUE(0x41, sharedValue);
            }
        }

        if ((u16)moveType != 9) {
            for (j = 0; (u16)j < 2; j++) {
                u16 type = (u16)fightTrainerGetStatus(
                    0, trainerData, 0x39, j);
                if (type != 9 && (u16)moveType == type) {
                    sharedValue = (u8)fightTrainerGetStatus(
                        0, trainerData, 0x3A, j);
                    AI_SCORE_VALUE(0x42, sharedValue);
                }
            }
        }

        for (k = 0; k < 3; k++) {
            u8 effect = fn_80239498(trainer, move, k & 0xFF);
            if (effect != 0) {
                for (j = 0; (u16)j < 2; j++) {
                    u8 type = (u8)fightTrainerGetStatus(
                        0, trainerData, 0x3B, j);
                    if (type != 0 && effect == type) {
                        sharedValue = (u8)fightTrainerGetStatus(
                            0, trainerData, 0x3C, j);
                        AI_SCORE_VALUE(0x43, sharedValue);
                    }
                }
            }
        }

        if (fn_8023943C(trainer, move, 1) == 1) {
            if (maxDamage.value <= damage[i]) {
                AI_SCORE(0x44);
            }
            delta = difference[i];
            if (delta < 0) {
                AI_SCORE(0x45);
            }
            if (delta <= 0) {
                AI_SCORE(0x46);
            }
            if (delta <= 0 && maxMetric <= moveMetric[i]) {
                AI_SCORE(0x47);
            }
        }

        if (fn_8023943C(trainer, move, 2) == 1 &&
            fn_8023753C(trainer, pokemon) == 1) {
            AI_SCORE(0x48);
        }
        if (fn_802392A8(trainer, move) == 1) {
            AI_SCORE(0x49);
        }
        if ((u8)moveKind == 4 &&
            fightFloorGetValidFightOutPokemonCount(0, 1, target, 1) >= 2) {
            AI_SCORE(0x4A);
        }
        if (fn_80238060(trainer, pokemon, (u8)data) == 1) {
            AI_SCORE(0x4B);
        }
        if (fn_80239154(trainer, move) == 1) {
            AI_SCORE(0x4C);
        }

        if (fn_8000815C() == 1) {
            delta = (u16)fn_800E0C54() % randomRange - randomBase;
            score[i] = fightTrainerAiAddValue(score[i], delta);
            fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                        trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0,
                        0, move, 0, 0x225, delta);
        }

        fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                    trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0, 0,
                    move, 0, 0x226, score[i]);
    }

    selected = fightTrainerAiGetValueAryMaxBanme(score, moveCount, 1);
    if (selected < 0) {
        return -1;
    }
    fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0, 0,
                moveList[selected], 0, 0x228, score[selected]);
    return selected;
}
#pragma opt_propagation reset

#undef AI_SCORE_VALUE
#undef AI_SCORE
#pragma opt_lifetimes reset
#pragma optimize_for_size reset
