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

u8 darkPokemonCheckFlag12(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag12 != 0) {
            return std081_flagCheckTrue(def->flag12);
        }
        return 0;
    }
    return 0;
}
