/**
 * @file pda_exact_80038250.c
 * @brief Byte-exact PDA menu callbacks, 0x80038250 - 0x8003842C.
 */
#include "dolphin/types.h"

typedef struct PdaMenuState {
    u8 pad00;
    s8 mode;
    s8 menuSet;
} PdaMenuState;

typedef struct PdaSelectionWork {
    u8 pad00[4];
    void* menu;
    u8 pad08[0x8d];
    s8 selectedIndex;
} PdaSelectionWork;

typedef struct PdaKeyInfo {
    u8 pad00[4];
    u16 trigger;
    u16 buttons;
} PdaKeyInfo;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 pcMenuListCtrl(PdaMenuState* state)
{
    extern void winSeqSetMenu(s32 sequence, s32 menu);

    switch (state->mode) {
    case 0:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1b, 0xc4);
            state->menuSet = 1;
        }
        break;
    case 3:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1b, 0xc8);
            state->menuSet = 1;
        }
        break;
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 pcMenuListCtrl2(PdaMenuState* state)
{
    switch (state->mode) {
    case 0:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1a, 0xbc);
            state->menuSet = 1;
        }
        break;
    case 3:
        if (state->menuSet == 0) {
            winSeqSetMenu(0x1a, 0xc0);
            state->menuSet = 1;
        }
        break;
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 pcMenuCursor(PdaSelectionWork* work)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern s32 menuGetSelectItemNum();
    PdaKeyInfo* keyInfo;
    s32 itemCount;

    keyInfo = windowGetKeyInfo();
    if (keyInfo->buttons & 2) {
        itemCount = menuGetSelectItemNum(work->menu);
        itemCount = (s8)itemCount;
        if ((s8)++work->selectedIndex >= itemCount) {
            work->selectedIndex = itemCount - 1;
        }
    }
    if (keyInfo->buttons & 1) {
        if ((s8)--work->selectedIndex < 0) {
            work->selectedIndex = 0;
        }
    }
    return 0;
}
#pragma peephole reset
