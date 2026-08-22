#include "dolphin/types.h"

typedef struct {
    u16 fields[9];
} FieldTable9;

extern FieldTable9 lbl_80279CA4;
extern void* pokemonGetStatus(void* context, s32 arg1, s32 status, s32 arg3);

u32 fightOutPokemonCheckNoAttackFlag(void* context)
{
    FieldTable9 table;
    u8 i;

    table = lbl_80279CA4;
    if (context == NULL) {
        return 0;
    }
    for (i = 0; i < 9; i++) {
        if ((s32) pokemonGetStatus(context, 0, table.fields[i], 0) == 1) {
            return 1;
        }
    }
    return 0;
}
