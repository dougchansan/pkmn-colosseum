/**
 * @file fight_range_exact_8022808C.c
 * @brief Exact island 0x8022808C - 0x802282D8 (wsCmd03), moved out of
 *        fight_range_80211A00.c.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_80478D78[8];
extern u32 lbl_8047B618;
extern void* lbl_8047B62C;
extern void wsRunScript(void*, void*, u8);

void wsCmd03(void)

{
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u32 fightOutPokemonGetPokemonJoutai08();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern s32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 lbl_8047B618;
    u32 attacker;
    u32 statusSlot;
    u32 target;
    u32 ability;
    u32 move;
    u32 usedFlags;
    u8 deduction;
    u32 pokemon;
    u8 pp;
    u8 moveField;

    deduction = 1;
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    statusSlot = pokemonGetStatus(attacker, 0, 0xd9, 0);
    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    ability = fightOutPokemonGetTokuseiDataId(target);

    if ((lbl_8047B618 & 0xa00) == 0) {
        move = fightOutPokemonGetUseWazaDataId(attacker);
        moveField = (u8)wazaGetStatus(0, move, 5, 0);
        statusSlot = (s8)wazaGetStatus(statusSlot, 0, 0x26, 0);
        pp = (u8)pokemonGetStatus(pokemon, 0, 0x80, (s8)statusSlot);

        if ((s32)pokemonGetStatus(attacker, 0, 0x118, 0) == 0) {
            switch (moveField) {
            case 6:
                deduction = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
                    0, 0x2e, 0, attacker) + 1;
                break;
            case 4:
            case 7:
                deduction = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
                    0, 0x2e, 2, attacker) + 1;
                break;
            default:
                if (attacker != target && (u16)ability == 0x2e) {
                    deduction = 2;
                }
                break;
            }
        }

        if (pp != 0) {
            pokemonSetStatus((void*)attacker, 0, 0x111, 0, 1);
            if (deduction < pp) {
                pp -= deduction;
            } else {
                pp = 0;
            }
            pokemonSetStatus((void*)pokemon, 0, 0x80, (s8)statusSlot, pp);

            if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x10) == 0 &&
                fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x31) == 1) {
                usedFlags = fightOutPokemonGetPokemonJoutai08(attacker, 0x31);
                if ((usedFlags & (1 << (u8)statusSlot)) == 0 &&
                    fightOutPokemonIsUseHensinBuff(attacker) == 1) {
                    fightOutPokemonSetHensinPokemonStatusId(
                        attacker, 0x80, (u8)statusSlot, 0);
                }
            }
        }
    }

    lbl_8047B618 &= 0xfffff7ff;
    lbl_8047B610++;
}
