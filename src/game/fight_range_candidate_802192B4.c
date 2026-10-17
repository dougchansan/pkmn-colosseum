/**
 * @file fight_range_candidate_802192B4.c
 * @brief Fight sequence commands, 0x802192B4 - 0x80219804 (wsCmdA7,
 *        wsCmdA6, wsCmdA5, wsCmdA4).
 *
 * wsCmdA6 picks a random entry of its stack type array as
 * `p = typeArray; p += index; *p`, written after the random call: that
 * creates the array base before the modulo operands, which gives retail's
 * volatile registers (random r4, count r3, base r5).
 *
 * wsCmdA4 needs fightOutPokemonSetPokemonJoutai08's third parameter prototyped as a full
 * word (s32): the slot is then sign-extended again at that call instead
 * of reusing the extension made for pokemonGetStatus.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 fn_802025B8();
extern void fightOutPokemonWritePokemonJoutaiDataId();

void wsCmdA7(void)
{
    extern int fightTargetGetPtrAsNowFightType();
    extern int fightFloorGetStatus();
    extern void* fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    void* attacker;
    void* target;
    u16 floor;
    void* targetId;

    attacker = (void*)fightTargetGetPtrAsNowFightType(0x11, 0);
    floor = fightFloorGetStatus(0, 0, 0x14, 0);
    targetId = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floor);
    target = (void*)fightTargetGetPtrAsNowFightType(0x12, 0);
    if (fn_802025B8(target, 0x1d) == 2) {
        fightOutPokemonWritePokemonJoutaiDataId(target, 0x1d, targetId);
    }
    lbl_8047B610++;
}

void wsCmdA6(void)
{
    extern u32 GSrand();
    extern u32 GSmsgGetGSchar();
    extern void fn_8010C4D4();
    extern u32 wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u32 fightOutPokemonGetTeikouZokuseiDataIdAry();
    extern u32 fightOutPokemonSetZokuseiDataId(u32, u8, u16);
    extern u32 pokemonGetStatus();
    u8 specialMove;
    u16 hostSide;
    u32 pokemon;
    u16 type;
    u16 sourceType;
    u32 target;
    s32 random;
    u32 count;
    u32 selected;
    u16 i;
    u32 typeArray[18];
    u32* p;

    hostSide = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    pokemon = fightTargetGetPtrAsNowFightType(0x11, 0);
    pokemonGetStatus(pokemon, 0, 0xd9, 0);
    type = (u16)pokemonGetStatus(pokemon, 0, 0xf3, 0);
    pokemonGetStatus(pokemon, 0, 0x102, 0);
    pokemonGetStatus(pokemon, 0, 0x104, 0);
    sourceType = (u16)pokemonGetStatus(pokemon, 0, 0xf4, 0);
    target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
        (u16)pokemonGetStatus(pokemon, 0, 0xf2, 0), hostSide);

    if (type == 0 || type == 0x165 || type == 0xffff || type == 0x164) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        if (target != 0) {
            type = (u16)wazaGetStatus(0, type, 9, 0);
            if (type == 0x91 || type == 0x27 || type == 0x4b ||
                type == 0x97 || type == 0x9b || type == 0x1a) {
                specialMove = 1;
            } else {
                specialMove = 0;
            }
            if (specialMove && fightOutPokemonIsPokemonJoutaiDataId(target, 0x22) == 1) {
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
                return;
            }
        }
        count = fightOutPokemonGetTeikouZokuseiDataIdAry(pokemon, sourceType, typeArray);
        if ((count & 0xffff) == 0) {
            lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        } else {
            random = GSrand();
            p = typeArray;
            p += (u16)((u16)random % (s32)(u16)count);
            random = *p;
            if (random < 0) {
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
            } else {
                selected = random & 0xffff;
                for (i = 0; i < 2; i++) {
                    fightOutPokemonSetZokuseiDataId(pokemon, (u8)i, selected);
                }
                fn_8010C4D4(selected);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                lbl_8047B610 += 5;
            }
        }
    }
}

void wsCmdA5(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void wazaSetStatus();
    extern void pokemonSetStatus();
    extern s32 lbl_8047B608;
    u32 attacker;
    u32 defender;
    u32 move;
    u32 attackerPokemon;
    s32 attackerHp;
    u32 defenderPokemon;
    s32 defenderHp;
    s32 average;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(attacker, 0, 0xD9, 0);
    attackerPokemon = fightOutPokemonGetPokemonPtr(attacker);
    attackerHp = (u16)pokemonGetStatus(attackerPokemon, 0, 0x83, 0);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    defenderPokemon = fightOutPokemonGetPokemonPtr(defender);
    defenderHp = (u16)pokemonGetStatus(defenderPokemon, 0, 0x83, 0);

    if (fightOutPokemonIsPokemonJoutaiDataId(defender, 0x14) == 0) {
        average = (attackerHp + defenderHp) / 2;
        lbl_8047B608 = defenderHp - average;
        wazaSetStatus(move, 0, 0x2d, 0, attackerHp - average);
        pokemonSetStatus(defender, 0, 0x11b, 0, 0xFFFF);
        lbl_8047B610 += 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}

void wsCmdA4(void)
{
    extern s8 pokemonSearchWazaDataId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u16 fightOutPokemonSetPokemonJoutai08(u32, u32, s32);
    extern void fightOutPokemonSetPokemonJoutai0C();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    u8 blocked;
    u32 target;
    u32 pokemon;
    u16 lastMove;
    s8 slot;
    u8 pp;
    u16 move;

    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    pokemon = fightOutPokemonGetPokemonPtr(target);
    lastMove = pokemonGetStatus(target, 0, 0xf0, 0);
    slot = pokemonSearchWazaDataId(pokemon, lastMove);
    if (slot < 0) {
        pp = 0;
    } else {
        pp = pokemonGetStatus(pokemon, 0, 0x80, slot);
    }
    if (lastMove == 0xa5 || lastMove == 0xe3 || lastMove == 0x77 || lastMove == 0xffff) {
        blocked = 1;
    } else {
        blocked = 0;
    }
    if (blocked == 1) {
        pp = 0;
        slot = -1;
    }
    if (fn_802025B8(target, 0x2a) == 2 && slot >= 0 && pp != 0) {
        fightOutPokemonWritePokemonJoutaiDataId(target, 0x2a, 0);
        move = pokemonGetStatus(pokemon, 0, 0x7f, slot);
        fightOutPokemonSetPokemonJoutai0C(target, 0x2a, move);
        fightOutPokemonSetPokemonJoutai08(target, 0x2a, slot);
        lbl_8047B610 += 5;
        return;
    }
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
}
