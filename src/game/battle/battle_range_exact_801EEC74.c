#include "dolphin/types.h"

typedef struct BattleRangeDef {
    u8 type;
    u8 pad01;
    u16 field02;
    u16 field04;
    u16 field06;
    u16 field08;
    u16 runtimeSlot;
    u16 flag0C;
    u16 flag0E;
    u16 pad10;
    u16 flag12;
    u16 flag14;
    u16 flag16;
    u16 pad18;
    u16 pad1A;
    u16 flag1C;
    u16 actionListId;
    u16 pad20;
    u16 pad22;
    u16 indexedFlags[10];
} BattleRangeDef;

extern BattleRangeDef* lbl_80478F6C;
extern u8 std081_flagCheckTrue(u16 flag);
extern void _flagSet(u16 flag, s32 value);

u8 darkPokemonCheckFlag14(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag14 != 0) {
            return std081_flagCheckTrue(def->flag14);
        }
        return 0;
    }
    return 0;
}

void darkPokemonSetFlag14(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag14 != 0) {
        _flagSet(def->flag14, value);
    }
}

void darkPokemonSetFlag0E(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag0E != 0) {
        _flagSet(def->flag0E, value);
    }
}

u8 darkPokemonCheckFlag0C(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag0C != 0) {
            return std081_flagCheckTrue(def->flag0C);
        }
        return 0;
    }
    return 0;
}

void darkPokemonSetFlag0C(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag0C != 0) {
        _flagSet(def->flag0C, value);
    }
}

u8 fn_801EEE44(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    return def->type;
}
