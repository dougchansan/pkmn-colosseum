/**
 * @file fight_pokemon_r58_801FEF74_middle.c
 * @brief fightOutPokemonGetJoutaiMigawariHp carve, 0x801FEF74 - 0x801FF1BC,
 *        linked.
 *
 * Standalone copy of the shared candidate body, built at -O4,s with
 * -schedule off (configure.py) in place of the shared file's
 * push/scheduling-off/pop pragmas.
 */
#include "dolphin/types.h"

extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);
extern u8 pokemonGetJoutai08(void* obj, s32 arg);
extern u8 statusGetJoutai08(void* obj, s32 arg);

u8 fightOutPokemonGetJoutaiMigawariHp(void* trainer) {
    extern u16 joutaiDataBiosGetStatusKind(s32 id);
    extern u8 fn_8011B67C(void* obj, s32 arg);
    extern u8 pokemonIsJoutaiDataId(void* obj, u32 field);
    void* obj;
    void* target;
    void* obj2;
    void* target2;
    u8 status;

    if (trainer == NULL) {
        return 0;
    }
    if (joutaiDataBiosGetStatusKind(0x14) == 0x7C || joutaiDataBiosGetStatusKind(0x14) == 0xC8 || joutaiDataBiosGetStatusKind(0x14) == 0xCD) {
        obj = pokemonGetStatus(trainer, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(0x14) == 0x7C || joutaiDataBiosGetStatusKind(0x14) == 0xC8) {
            if (obj == NULL) {
                target = NULL;
            } else {
                target = pokemonGetStatus(obj, 0, 0xCC, 0);
            }
            status = pokemonIsJoutaiDataId(target, 0x14);
        } else if (joutaiDataBiosGetStatusKind(0x14) != 0xCD) {
            status = 0;
        } else {
            status = fn_8011B67C(obj, 0x14);
        }
    } else if (joutaiDataBiosGetStatusKind(0x14) != 0xD8) {
        status = 0;
    } else {
        status = fn_8011B67C(trainer, 0x14);
    }
    if (status == 1) {
        if (joutaiDataBiosGetStatusKind(0x14) == 0x7C || joutaiDataBiosGetStatusKind(0x14) == 0xC8 || joutaiDataBiosGetStatusKind(0x14) == 0xCD) {
            obj2 = pokemonGetStatus(trainer, 0, 0xD6, 0);
            if (joutaiDataBiosGetStatusKind(0x14) == 0x7C || joutaiDataBiosGetStatusKind(0x14) == 0xC8) {
                if (obj2 == NULL) {
                    target2 = NULL;
                } else {
                    target2 = pokemonGetStatus(obj2, 0, 0xCC, 0);
                }
                return pokemonGetJoutai08(target2, 0x14);
            } else if (joutaiDataBiosGetStatusKind(0x14) != 0xCD) {
                return 0;
            } else {
                return statusGetJoutai08(obj2, 0x14);
            }
        } else if (joutaiDataBiosGetStatusKind(0x14) != 0xD8) {
            return 0;
        } else {
            return statusGetJoutai08(trainer, 0x14);
        }
    }
    return 0;
}
