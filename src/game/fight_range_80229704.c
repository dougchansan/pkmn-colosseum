/**
 * @file fight_range_80229704.c
 * @brief Status-move usability checks, 0x80229704 - 0x80229B70
 *        (fightAiCheckRule0E0F, fightCheckAccuracyRules).
 *
 * Standalone source at GC/1.3 -O4,s. Both functions count the valid, fought-in
 * Pokemon on the target's side that can take a status through one shared
 * inline, fightCountJoutai: fightAiCheckRule0E0F expands it twice (the status id, then
 * status 7), fightCheckAccuracyRules once (status 8), and fightSeqSetTsuikaKouka expands the same
 * sequence for statuses 7 and 8. Its locals are declared pokemon, side,
 * trainerCount, found, trainer, trainerIndex, pokemonIndex, which gives
 * retail's register order in every expansion.
 */
#include "dolphin/types.h"

extern u8 fn_80077B3C(void);
extern u8 fn_80077B60(void);
extern u32 fightTargetGetPtrAsNowFightType();
extern u32 fightFloorGetStatus();
extern u32 fightSideGetValidFightTrainerPtr();
extern u32 fightTrainerGetValidFightPokemonPtr();
extern u8 fightPokemonCheckWriteJoutaiDataId();
extern u8 fightPokemonCheckFightOut();

static inline u16 fightCountJoutai(u32 target, u32 statusId)
{
    u32 pokemon;
    u32 side;
    u16 trainerCount;
    u16 found;
    u32 trainer;
    u16 trainerIndex;
    u32 pokemonIndex;

    found = 0;
    side = fightTargetGetPtrAsNowFightType(2, target);
    trainerCount = fightFloorGetStatus(0, 0, 0x16, 0);
    fightFloorGetStatus(0, 0, 0x17, 0);
    for (trainerIndex = 0; trainerIndex < trainerCount; trainerIndex++) {
        trainer = fightSideGetValidFightTrainerPtr(side, trainerIndex);
        if (trainer != 0) {
            for (pokemonIndex = 0; (u16)pokemonIndex < 6; pokemonIndex++) {
                pokemon = fightTrainerGetValidFightPokemonPtr(trainer, pokemonIndex);
                if (pokemon != 0 && fightPokemonCheckFightOut(pokemon) != 0 &&
                    fightPokemonCheckWriteJoutaiDataId(pokemon, statusId) == 1) {
                    found++;
                }
            }
        }
    }
    return found;
}

u32 fightAiCheckRule0E0F(u32 statusId, u32 target)
{
    u8 enabled;
    u8 disabled8;
    u8 disabled7;

    enabled = fightFloorGetStatus(0, 0, 0x34, 0);
    disabled8 = fn_80077B60();
    disabled7 = fn_80077B3C();
    if (enabled == 1) {
        if ((u16)statusId == 8) {
            if (disabled8 != 1 && fightCountJoutai(target, statusId) >= 1) {
                return 1;
            }
        } else if ((u16)statusId == 7 && disabled7 != 1) {
            if (fightCountJoutai(target, 7) >= 1) {
                return 1;
            }
        }
    }
    return 0;
}

u8 fightCheckAccuracyRules(u32 move, u32 attacker, u32 target)
{
    extern u8 fn_80077AAC();
    extern u8 fn_80077AD0();
    extern u8 fn_80077B18();
    extern u16 wazaGetStatus();
    extern u32 fightSideGetFightPokemonNum();
    u8 enabled;
    u8 disabled11D;
    u8 disabledC2;
    u8 disabled52;
    u8 disabled8;
    u32 side;
    u16 trainerCount;
    u16 floor17;
    u16 pokemonNum;
    u16 kind;
    u32 id;

    enabled = fightFloorGetStatus(0, 0, 0x34, 0);
    disabled11D = fn_80077B18();
    disabledC2 = fn_80077AD0();
    disabled52 = fn_80077AAC();
    disabled8 = fn_80077B60();
    fn_80077B3C();
    if (enabled == 1) {
        id = move & 0xffff;
        if (id == 0x11d) {
            if (disabled11D != 1) {
                return 1;
            }
        } else if (id == 0xc3 || id == 0xc2) {
            side = fightTargetGetPtrAsNowFightType(2, attacker);
            trainerCount = fightFloorGetStatus(0, 0, 0x16, 0);
            floor17 = fightFloorGetStatus(0, 0, 0x17, 0);
            pokemonNum = fightSideGetFightPokemonNum(side, trainerCount, floor17);
            if (pokemonNum <= 1 && disabledC2 != 1) {
                return 1;
            }
        } else if (id == 0x52 || id == 0x31) {
            if (disabled52 != 1) {
                return 1;
            }
        } else {
            kind = wazaGetStatus(0, move, 9, 0);
            if (kind == 1) {
                u16 found = fightCountJoutai(target, 8);
                if (disabled8 != 1 && found >= 1) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
