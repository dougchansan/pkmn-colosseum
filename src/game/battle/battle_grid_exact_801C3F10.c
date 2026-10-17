/**
 * @file battle_grid_exact_801C3F10.c
 * @brief Exact battle-grid trainer replacement and insertion helpers.
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
extern const char lbl_80275830[];

extern void GSlogWrite(const char*, ...);
extern void wazaSequenceOwnerSetVisibility(void*, u32);
extern void* memset(void*, int, u32);

void battleGridReplaceTrainer(u8* trainer, u8* replacement)
{
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u16 i;

    if (trainer == NULL || replacement == NULL) {
        GSlogWrite(lbl_80275830);
        return;
    }

    for (i = 0; i < 4; i++, group++) {
        if (group->slot == trainer) {
            group->slot = replacement;
            wazaSequenceOwnerSetVisibility(trainer, 0);
            replacement[0x76] = trainer[0x76];
            trainer[0x76] = 0;
            return;
        }
    }
}

void battleGridAddTrainer(u8* slot, u8 arg1, u8 arg2)
{
    BattleGridGroupEntry* group;
    s8 state;

    if (lbl_80466DE8.count < 4) {
        group = &lbl_80466DE8.entries[0];
        if (group->slot != NULL) {
            group = &lbl_80466DE8.entries[1];
            if (group->slot != NULL) {
                group++;
                if (group->slot != NULL) {
                    group++;
                    if (group->slot != NULL) {
                        group++;
                    }
                }
            }
        }
        memset(group, 0, sizeof(*group));
        group->slot = slot;
        state = 1;
        group->arg1 = arg1;
        group->arg2 = arg2;
        if (arg1 != 0) {
            state = -1;
        }
        slot[0x76] = state;
        lbl_80466DE8.count = lbl_80466DE8.count + 1;
    }
}
