/**
 * @file menuColosseumBattle_tail_exact_8005D6A8.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x8005D6A8 - 0x8005D7F8.
 */
#include "dolphin/types.h"

typedef struct MenuKeyInfo {
    u8 pad_00[4];
    u16 buttons;
    u16 repeatButtons;
} MenuKeyInfo;

typedef struct MenuWindow {
    u8 pad_00[0x4C];
    s32 nextMenu;
    u8 pad_50[0x10];
    void* partyState;
    u8 pad_64[0x31];
    s8 cursor;
    u8 pad_96;
    s8 previousCursor;
    u8 accepted;
    u8 canceled;
} MenuWindow;

typedef struct MenuSeTable {
    u16 field_00;
    u16 field_02;
    u16 field_04;
    u16 field_06;
    u16 field_08;
} MenuSeTable;

typedef union ColosseumBitMasks {
    u32 word;
    u8 bytes[4];
} ColosseumBitMasks;

extern u8* windowGetKeyInfo(void);
extern void fn_80166A50(s32, s32, s32, s32);
extern void menuButtonNormal(void*);
extern s32 menuOpen(s32, s32);
extern void menuClose();
extern s32 menuCloseSync(s32, s32);

extern u8 lbl_8047A5A8;
extern const ColosseumBitMasks lbl_8047BF40;

/* Address: 0x8005D6A8 | Size: 0x90 */
#pragma push
#pragma peephole off
void fn_8005D6A8(MenuWindow* window) {
    MenuKeyInfo* keys = (MenuKeyInfo*)windowGetKeyInfo();
    ColosseumBitMasks masks;
    s32 cursor = window->cursor;

    masks.word = lbl_8047BF40.word;
    if (cursor >= 0 && cursor <= 3 && (keys->buttons & 0x10) != 0) {
        lbl_8047A5A8 ^= masks.bytes[cursor];
        fn_80166A50(0x3C6, 0, 0xFF, 0);
    } else {
        menuButtonNormal(window);
    }
}
#pragma pop

/* Address: 0x8005D738 | Size: 0x60 */
#pragma push
#pragma peephole off
u32 fn_8005D738(u8 flags) {
    s32 menuResult;

    lbl_8047A5A8 = flags;
    menuResult = menuOpen(0x9D, 1);
    menuClose(0x9D);
    menuCloseSync(0x9D, 1);
    if (menuResult == 0x73D) {
        return lbl_8047A5A8;
    } else {
        return 0xFF;
    }
}
#pragma pop

/* Address: 0x8005D798 | Size: 0x60 */
u32 fn_8005D798(MenuSeTable* table, u8 index) {
    u32 value = 0;

    switch (index) {
    case 1:
        value = table->field_04;
        break;
    case 2:
        value = table->field_06;
        break;
    case 3:
        value = table->field_08;
        break;
    case 4:
        value = table->field_00;
        break;
    case 5:
        value = table->field_02;
        break;
    }

    return value;
}
