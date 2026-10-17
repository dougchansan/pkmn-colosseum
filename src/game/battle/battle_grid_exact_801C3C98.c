/**
 * @file battle_grid_exact_801C3C98.c
 * @brief Exact battle-grid Pokemon removal and replacement helpers.
 */
#include "dolphin/types.h"

typedef struct BattleGridGroupEntry {
    u8* slot;
    u8* pokemon[2];
    u16 memberCount;
    u8 arg1;
    u8 arg2;
} BattleGridGroupEntry;

typedef struct BattleGridGroupTable {
    BattleGridGroupEntry entries[4];
    u16 count;
    u16 pokemonCount;
} BattleGridGroupTable;

extern BattleGridGroupTable lbl_80466DE8;
extern const char lbl_80275808[];

extern void GSlogWrite(const char*, ...);
extern void wazaSequenceOwnerSetVisibility(void*, u32);

void battleGridRemovePokemon(u8* pokemon)
{
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u16 i;
    u16 j;

    if (pokemon != NULL) {
        for (i = 0; i < 4; i++, group++) {
            for (j = 0; j < 2; j++) {
                if (group->pokemon[j] == pokemon) {
                    group->pokemon[j] = NULL;
                    group->memberCount--;
                    lbl_80466DE8.pokemonCount--;
                    wazaSequenceOwnerSetVisibility(pokemon, 0);
                    pokemon[0x76] = 0;
                    return;
                }
            }
        }
    }
}

void battleGridReplacePokemon(u8* pokemon, u8* replacement)
{
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u16 i;
    u16 j;

    if (pokemon == NULL || replacement == NULL) {
        GSlogWrite(lbl_80275808);
        return;
    }

    for (i = 0; i < 4; i++, group++) {
        for (j = 0; j < 2; j++) {
            if (group->pokemon[j] == pokemon) {
                group->pokemon[j] = replacement;
                wazaSequenceOwnerSetVisibility(pokemon, 0);
                replacement[0x76] = pokemon[0x76];
                pokemon[0x76] = 0;
                return;
            }
        }
    }
}
