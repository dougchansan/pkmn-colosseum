/** Exact battle-grid visibility accessors, 0x801C2AE8 - 0x801C2BE0. */
#include "dolphin/types.h"

typedef struct BattleGridGroupEntry {
    u8* slot;
    u8* pokemon[2];
    u16 memberCount;
    u8 arg1;
    u8 arg2;
} BattleGridGroupEntry;

extern BattleGridGroupEntry lbl_80466DE8[];

u16 battleGridGetNumPokemonsForTrainer(u32 id)
{
    BattleGridGroupEntry* group;
    u16 i;

    group = lbl_80466DE8;
    for (i = 0; i < 4; i++, group++) {
        if ((u32)group->slot == id) {
            return group->memberCount;
        }
    }
    return 0;
}

void battleGridResetModelVisibilityFlags(void)
{
    extern u8 lbl_8047B39A;
    extern u8 lbl_8047B39C[12] __attribute__((section(".sdata")));
    extern void wazaSequenceOwnerSetVisibility(void*, u32);
    BattleGridGroupEntry* group;
    u16 i;
    u16 j;
    u16 visibilityIndex;

    group = lbl_80466DE8;
    visibilityIndex = 0;
    if (lbl_8047B39A != 0) {
        for (i = 0; i < 4; i++, group++) {
            wazaSequenceOwnerSetVisibility(group->slot, lbl_8047B39C[visibilityIndex++]);
            for (j = 0; j < 2; j++) {
                wazaSequenceOwnerSetVisibility(group->pokemon[j],
                            lbl_8047B39C[visibilityIndex++]);
            }
        }
        lbl_8047B39A = 0;
    }
}
