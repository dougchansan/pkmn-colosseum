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

static inline s32 battleGridGetMaxPokemonField(void)
{
    extern BattleGridGroupTable lbl_80466DE8;
    extern s32 fn_801DAC24(void*);
    u16 i;
    u16 j;
    BattleGridGroupEntry* group = lbl_80466DE8.entries;
    u8** pokemon;
    s32 maxField = -2;

    if (lbl_80466DE8.count == 0) {
        return 0;
    }
    for (i = 0; i < 4; i++, group++) {
        if (group->slot != NULL) {
            pokemon = group->pokemon;
            for (j = 0; j < 2; j++, pokemon++) {
                if (*pokemon != NULL) {
                    s32 field = fn_801DAC24(*pokemon);
                    if (field > maxField) {
                        maxField = field;
                    }
                }
            }
        }
    }
    return maxField;
}

void battleGridGetNormalisedScale(f32* scale)
{
    extern void set__5GSvecFfff(f32*, f32, f32, f32);
    extern const f32 lbl_8047DF64;
    extern const f32 lbl_8047DF68;
    extern const f32 lbl_8047DF6C;
    extern const f32 lbl_8047DF70;
    extern const f32 lbl_8047DF74;
    extern const f32 lbl_8047DFA0;
    s32 maxField;
    f32 scaleValue;

    maxField = battleGridGetMaxPokemonField();

    switch (maxField) {
    case -2:
    case -1:
        scaleValue = lbl_8047DF6C;
        break;
    case 1:
        scaleValue = lbl_8047DF64;
        break;
    case 2:
        scaleValue = lbl_8047DF70;
        break;
    case 3:
        scaleValue = lbl_8047DF74;
        break;
    default:
        scaleValue = lbl_8047DF68;
        break;
    }

    scaleValue *= lbl_8047DFA0;
    set__5GSvecFfff(scale, scaleValue, scaleValue, scaleValue);
}
