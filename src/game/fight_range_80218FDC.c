/**
 * @file fight_range_80218FDC.c
 * @brief Move-copy script command (copies the target's last
 *        move into the user's empty slot), 0x8021908C - 0x80219270.
 *
 * Standalone source at GC/1.3. The user's 0xD5 and 0xD6 slots are read into
 * their own locals before the two fightPokemonGetPokemonPtr calls.
 */
#include "dolphin/types.h"
extern u8* lbl_8047B610;
void wsCmdA8(void)
{
    extern u32 GSmsgGetGSchar();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern s8 pokemonSearchWazaDataId();
    extern void pokemonWazaCreate();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    u32 status;
    u32 firstSlot;
    u32 secondSlot;
    u32 firstPokemon;
    u32 target;
    u32 secondPokemon;
    u32 invalid;
    u16 move;
    u32 attacker;
    s8 slot;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    status = pokemonGetStatus(attacker, 0, 0xd9, 0);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    move = (u16)pokemonGetStatus(target, 0, 0xef, 0);
    wazaSetStatus(status, 0, 0x27, 0, 0xffff);

    if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x10) == 1) {
        goto failed;
    }

    if (move == 0x164 || (u16)(move - 0xa5) <= 1 || move == 0xffff ||
        move == 0 || move == 0x165) {
        invalid = 1;
    } else {
        invalid = 0;
    }
    if ((u8)invalid == 1) {
failed:
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        goto done;
    }

    firstSlot = pokemonGetStatus(attacker, 0, 0xd5, 0);
    secondSlot = pokemonGetStatus(attacker, 0, 0xd6, 0);
    firstPokemon = fightPokemonGetPokemonPtr(firstSlot);
    secondPokemon = fightPokemonGetPokemonPtr(secondSlot);
    if (pokemonSearchWazaDataId(firstPokemon, move) >= 0) {
        goto failed;
    }

    slot = (s8)wazaGetStatus(status, 0, 0x26, 0);
    if (slot < 0) {
        goto failed;
    }

    pokemonWazaCreate(firstPokemon, slot, move);
    if (fightOutPokemonIsUseHensinBuff(attacker) == 1) {
        pokemonWazaCreate(secondPokemon, slot, move);
    }
    wazaGetStatus(0, move, 1, 0);
    msgctrlSetValue(0xd, GSmsgGetGSchar());
    lbl_8047B610 += 5;
done:
    return;
}
