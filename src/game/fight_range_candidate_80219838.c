/**
 * @file fight_range_candidate_80219838.c
 * @brief Fight sequence commands, 0x80219838 - 0x80219D98 (wsCmdA3,
 *        wsCmdA2, wsCmdA1, wsCmdA0).
 *
 * wsCmdA2 and wsCmdA1 are one body with different pokemon status
 * ids, so they share an inline. As an inline its locals are numbered in
 * reverse declaration order, which gives retail's register order (floor,
 * power, target id, move, attacker) and lets the move result coalesce
 * without an extra copy. The Follow Me redirect is the same inline as in
 * fightSeqSelectTarget (fight_range_8022B2CC.c).
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u16 GSrand(void);

extern u32 pokemonGetStatus();
extern void pokemonSetStatus();
extern s8 pokemonSearchWazaDataId();
extern void wazaGetStatus(u32, u16, u32, u32);
extern void wazaSetStatus();
extern u32 GSmsgGetGSchar();
extern void msgctrlSetValue();
extern u32 fightTargetGetPtrAsNowFightType();
extern int fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
extern u32 fightFloorGetStatus();
extern void fightFloorSetStatus();
extern u8 fightSideIsJoutaiDataId();
extern u32 fightSideGetJoutaiUserFightTargetId();
extern u32 fightOutPokemonGetPokemonPtr();
extern u8 fightOutPokemonIsAlly();
extern u8 fightOutPokemonCheckFightOut();
extern u8 figthOutPokemonGetLevel();
extern void fightOutPokemonSetPokemonJoutai0C();
extern void fightOutPokemonWritePokemonJoutaiDataId();
extern u8 fn_802025B8();

void wsCmdA3(void)
{
    u32 target;
    u32 pokemon;
    u16 lastMove;
    u16 move;
    s8 slot;
    u8 pp;

    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    pokemon = fightOutPokemonGetPokemonPtr(target);
    lastMove = pokemonGetStatus(target, 0, 0xf0, 0);
    slot = pokemonSearchWazaDataId(pokemon, lastMove);
    if (slot < 0) {
        pp = 0;
    } else {
        pp = pokemonGetStatus(pokemon, 0, 0x80, slot);
    }
    if (fn_802025B8(target, 0x29) == 2 && slot >= 0 && pp != 0) {
        move = pokemonGetStatus(pokemon, 0, 0x7f, slot);
        wazaGetStatus(0, move, 1, 0);
        msgctrlSetValue(0xd, GSmsgGetGSchar());
        fightOutPokemonWritePokemonJoutaiDataId(target, 0x29, 0);
        fightOutPokemonSetPokemonJoutai0C(target, 0x29, move);
        lbl_8047B610 += 5;
        return;
    }
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
}

/* The live Follow Me user (side joutai 0x4d) on the side that
 * fightTargetGetPtrAsNowFightType(3, attacker) returns, or 0. */
static inline u32 fightGetFollowMeTarget(u32 attacker)
{
    u32 target;
    u32 userId;
    u32 side;
    u32 redirected;
    u16 floor;

    side = fightTargetGetPtrAsNowFightType(3, attacker);
    floor = fightFloorGetStatus(0, 0, 0x14, 0);
    redirected = 0;
    if (fightSideIsJoutaiDataId(side, 0x4d) == 1) {
        userId = fightSideGetJoutaiUserFightTargetId(side, 0x4d);
        if ((userId & 0xffff) != 0) {
            target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(userId, floor);
            if (target != 0 && fightOutPokemonCheckFightOut(target) == 1) {
                redirected = target;
            }
        }
    }
    return redirected;
}

/*
 * Checks the target stored in the attacker's status (targetStatus) and, if
 * it is still in battle and not an ally, sets the move's power from
 * powerStatus and aims the move there, unless a Follow Me user redirects it.
 */
static inline void fightSeqAimStoredTarget(u32 targetStatus, u32 powerStatus)
{
    int target;
    u32 followMe;
    u32 attacker;
    u32 move;
    u16 targetId;
    s16 power;
    u16 floor;

    floor = fightFloorGetStatus(0, 0, 0x14, 0);
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(attacker, 0, 0xd9, 0);
    targetId = pokemonGetStatus(attacker, 0, targetStatus, 0);
    power = pokemonGetStatus(attacker, 0, powerStatus, 0);
    if (targetId == 0 || power == 0) {
        goto fail;
    }
    target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(targetId, floor);
    if (target == 0) {
        goto fail;
    }
    if (fightOutPokemonIsAlly(attacker, target) != 0) {
        goto fail;
    }
    if (fightOutPokemonCheckFightOut(target) == 1) {
        wazaSetStatus(move, 0, 0x2d, 0, power << 1);
        followMe = fightGetFollowMeTarget(attacker);
        if (followMe != 0) {
            target = followMe;
        }
        fightFloorSetStatus(0, 0, 0x43, 0, target);
        lbl_8047B610 += 5;
        return;
    }
fail:
    pokemonSetStatus(attacker, 0, 0x118, 0, 1);
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
}

void wsCmdA2(void)
{
    fightSeqAimStoredTarget(0x105, 0x104);
}

void wsCmdA1(void)
{
    fightSeqAimStoredTarget(0x103, 0x102);
}

/* Sets the move's power to level * (50 + 10 * (rand % 11)) / 100. */
void wsCmdA0(void)
{
    u32 attacker;
    u32 move;
    u32 level;
    u16 rng;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(attacker, 0, 0xd9, 0);
    level = figthOutPokemonGetLevel(attacker);
    rng = GSrand();
    wazaSetStatus(move, 0, 0x2d, 0, (s32)(level * ((rng % 11) * 10 + 50)) / 100);
    lbl_8047B610++;
}
