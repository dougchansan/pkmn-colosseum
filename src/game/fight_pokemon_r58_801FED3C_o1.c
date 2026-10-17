/**
 * @file fight_pokemon_r58_801FED3C_o1.c
 * @brief fightOutPokemonSetJoutaiMigawariHp carve, 0x801FED3C - 0x801FEF74.
 *
 * Writer twin of fightOutPokemonGetJoutaiMigawariHp (the next carve): when
 * the substitute joutai (0x14) is set, store its hit points where the
 * reader looks for them.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);

/* Asks whether the joutai `id` is set. Moves 0x7C/0xC8/0xCD keep it in the
 * 0xD6 slot's data (0x7C/0xC8 through its 0xCC record), 0xD8 on the
 * Pokemon itself; any other kind is never set. */
static inline u8 fightPokemonIsJoutai(void* pokemon, u16 id) {
    extern u16 joutaiDataBiosGetStatusKind(u16 id);
    extern u8 fn_8011B67C(void* obj, u16 id);
    extern u8 pokemonIsJoutaiDataId(void* obj, u16 id);
    void* slot;
    void* data;

    if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8 || joutaiDataBiosGetStatusKind(id) == 0xCD) {
        slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8) {
            if (slot == NULL) {
                data = NULL;
            } else {
                data = pokemonGetStatus(slot, 0, 0xCC, 0);
            }
            return pokemonIsJoutaiDataId(data, id);
        } else if (joutaiDataBiosGetStatusKind(id) != 0xCD) {
            return 0;
        } else {
            return fn_8011B67C(slot, id);
        }
    } else if (joutaiDataBiosGetStatusKind(id) != 0xD8) {
        return 0;
    } else {
        return fn_8011B67C(pokemon, id);
    }
}

/* Stores `value` as the status word of the joutai `id`, where
 * fightPokemonIsJoutai looks for it. */
static inline void fightPokemonSetJoutaiStatus(void* pokemon, u16 id, u32 value) {
    extern u16 joutaiDataBiosGetStatusKind(u16 id);
    extern void pokemonSetJoutai08(void* obj, u16 id, u32 value);
    extern void statusSetJoutai08(void* obj, u16 id, u32 value);
    void* slot;
    void* data;

    if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8 || joutaiDataBiosGetStatusKind(id) == 0xCD) {
        slot = pokemonGetStatus(pokemon, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8) {
            if (slot == NULL) {
                data = NULL;
            } else {
                data = pokemonGetStatus(slot, 0, 0xCC, 0);
            }
            pokemonSetJoutai08(data, id, value);
        } else if (joutaiDataBiosGetStatusKind(id) == 0xCD) {
            statusSetJoutai08(slot, id, value);
        }
    } else if (joutaiDataBiosGetStatusKind(id) == 0xD8) {
        statusSetJoutai08(pokemon, id, value);
    }
}

void fightOutPokemonSetJoutaiMigawariHp(void* trainer, u32 hp) {
    if (trainer == NULL) {
        return;
    }
    if (fightPokemonIsJoutai(trainer, 0x14) == 1) {
        fightPokemonSetJoutaiStatus(trainer, 0x14, hp);
    }
}
