#include "dolphin/types.h"

extern u8* lbl_8047B610;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimize_for_size on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimize_for_size on
void wsCmd9D(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern void wazaSetStatus();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 wazaGetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u8 fightOutPokemonUseHensinBuff();
    extern void fightOutPokemonSetPokemonJoutai08();
    extern u32 fightOutPokemonGetPokemonJoutai08();
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern void pokemonWazaCreate();
    extern void pokemonSetStatus();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern u16 lbl_80279FA0[];
    u32 attacker;
    u32 pokemon;
    u32 move;
    u16 entry;
    u32 index;
    u8 invalid;
    s8 slot;
    u8 pp;
    u32 flags;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    pokemon = pokemonGetStatus(attacker, 0, 0xd9, 0);
    move = pokemonGetStatus(
               fightTargetGetPtrAsNowFightType(0x12, 0), 0, 0xf0, 0) &
           0xffff;
    wazaSetStatus(pokemon, 0, 0x27, 0, 0xffff);

    if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x10) == 1) {
        goto advance_alt;
    }

    if (move == 0) {
        invalid = 1;
    } else if (move == 0x165) {
        invalid = 1;
    } else if (move == 0x163) {
        invalid = 1;
    } else {
        index = 0;
        while ((entry = lbl_80279FA0[index]) != 0xfffe) {
            if (move == entry) {
                break;
            }
            index++;
        }
        invalid = entry != 0xfffe;
    }

    if (invalid != 0 || move == 0 || move == 0xffff || move == 0x165 ||
        move == 0x163) {
advance_alt:
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (pokemonSearchWazaDataId(fightOutPokemonGetPokemonPtr(attacker), move) >=
        0) {
        goto advance_alt;
    }

    slot = (s8)wazaGetStatus(pokemon, 0, 0x26, 0);
    if (slot < 0) {
        goto advance_alt;
    }

    if (fightOutPokemonIsUseHensinBuff(attacker) == 0 &&
        fightOutPokemonUseHensinBuff(attacker) == 0) {
        goto advance_alt;
    }

    if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x31) == 0) {
        fightOutPokemonWritePokemonJoutaiDataId(attacker, 0x31, 0);
    }

    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    pokemonWazaCreate(pokemon, slot, move);
    pp = (u8)wazaGetStatus(0, move, 2, 0);
    if (pp > 5) {
        pp = 5;
    }
    pokemonSetStatus(pokemon, 0, 0x80, slot, pp);
    wazaGetStatus(0, move, 1, 0);
    msgctrlSetValue(0xd, GSmsgGetGSchar());
    flags = fightOutPokemonGetPokemonJoutai08(attacker, 0x31);
    flags |= 1 << slot;
    fightOutPokemonSetPokemonJoutai08(attacker, 0x31, flags);
    lbl_8047B610 += 5;
}
#pragma optimize_for_size reset
