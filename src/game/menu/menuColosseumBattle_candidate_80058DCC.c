/**
 * @file menuColosseumBattle_candidate_80058DCC.c
 * @brief menuColosseumBattle.cpp candidate range, 0x80058DCC - 0x80058F08.
 */
#include "dolphin/types.h"

typedef struct MenuWindow {
    u8 field_00;
    s8 state;
    s8 initialized;
    u8 field_03;
    s32 windowId;
    u8 field_08[0x58];
    void* partyState;
    u8 field_64[0x31];
    s8 cursor;
    u8 field_96[2];
    u8 accepted;
    u8 canceled;
} MenuWindow;

typedef struct MenuCursorItem {
    u8 field_00[2];
    s16 row;
    s16 column;
} MenuCursorItem;

extern void* heroGetStatus(s32, s32, u16);
extern u8 pokemonCheckValid(void*);
extern MenuCursorItem* windowGetCursorToItem(MenuWindow*);
extern s32 windowGetActiveID(void);
extern void fn_80057830(s32, s32, s32);
extern s32 fn_80058AF0(MenuWindow*);

#pragma peephole off
s32 fn_80058DCC(MenuWindow* window)
{
    MenuCursorItem* item;
    s32 slot;

    switch (window->state) {
    case 0:
        if (!window->initialized) {
            window->initialized = 1;
        }
        if (*(s32*)window->partyState != 0) {
            for (slot = 0; slot < 6; slot++) {
                if (!pokemonCheckValid(heroGetStatus(0, 3, (u16)slot))) {
                    break;
                }
            }
            if (slot < 6) {
                window->cursor = (s8)slot;
            }
        }
        item = windowGetCursorToItem(window);
        fn_80057830(item->row, item->column, 0);
        break;
    case 2:
        if (window->windowId == windowGetActiveID() &&
            fn_80058AF0(window) == 0) {
            window->accepted = 1;
            window->canceled = 1;
        }
        break;
    case 3:
        if (!window->initialized) {
            window->initialized = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
