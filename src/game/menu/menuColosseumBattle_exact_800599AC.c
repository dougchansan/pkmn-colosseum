/**
 * @file menuColosseumBattle_exact_800599AC.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x800599AC - 0x80059BDC.
 */
#include "dolphin/types.h"
#include "game/menu/menu.h"

typedef struct ColosseumRosterRow {
    s32 mode;
    s32 menuId;
    s32 partyIndex;
    s32 action;
} ColosseumRosterRow;

extern s32 fn_80059034(s32);
extern void menuCloseCustom();
extern s32 menuCloseSync(s32, s32);

extern s32 lbl_8047A598;
extern s32 lbl_8047A59C;
extern const ColosseumRosterRow lbl_802677D0[7];

/* Address: 0x800599AC | Size: 0x230 */
#pragma push
#pragma peephole off
void fn_800599AC(s32 initialSlot) {
    s32 firstIndex;
    s32 secondIndex;
    const ColosseumRosterRow* table;
    const ColosseumRosterRow* row;
    s32 initialSlotValue;
    s32 nextSlotValue;
    s32 selectedPartyIndex;
    s32 done;
    s32* nextSlot;
    s32 result;
    s32 action;

    nextSlotValue = (firstIndex = 0);
    done = 0;
    table = lbl_802677D0;
    row = table;

    for (; firstIndex < 7; firstIndex++) {
        if (row->action == 0) {
            break;
        }
        row++;
    }
    lbl_8047A59C = firstIndex;

    for (secondIndex = 0; secondIndex < 7; secondIndex++) {
        if (table[secondIndex].action == 1) {
            break;
        }
    }
    lbl_8047A598 = secondIndex;

    nextSlot = &nextSlotValue;
    initialSlotValue = initialSlot;
    while (done == 0) {
        result = menuOpenCustom(MENU_ID(0x94), windowGetActiveID(), nextSlot,
                                0, MENU_CURSOR_CHECK(1), 1,
                                &initialSlotValue);
        nextSlot = NULL;
        if (result == -1) {
            done = 1;
            continue;
        }
        if (result < 0 || result >= 7) {
            action = 2;
        } else {
            selectedPartyIndex = lbl_802677D0[result].partyIndex;
            action = lbl_802677D0[result].menuId;
        }
        switch (action) {
        case 0:
            nextSlotValue = fn_80059034(selectedPartyIndex);
            nextSlot = &nextSlotValue;
            break;
        case 1:
            done = 1;
            break;
        }
    }

    menuCloseCustom(0x94, 2, 0);
    menuCloseSync(0x94, 1);
}
#pragma pop
