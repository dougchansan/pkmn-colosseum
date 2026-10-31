/**
 * @file menu_middle_r50_8006EE7C_prefix.c
 * @brief menu_middle carve, 0x8006EE7C - 0x8006EFF8: menuTaisenButton2 (toggle the
 *        flag under the cursor on the trigger key) and menuTaisenCursor (step the
 *        cursor with the repeat keys at scroll 0 or 10).
 *
 * Text only, GC/1.3 -O4,p with the peephole pass off for the whole unit
 * (retail keeps `beq; b` pairs the pass would fold; the range file wraps
 * both functions in `#pragma peephole off`). Both are menu state handlers:
 * in state 2 they act on the key, otherwise they fall through to the
 * default button/cursor handler.
 */
#include "dolphin/types.h"

typedef struct MenuKeyInfo {
    u32 pad0;
    u16 trigger;
    u16 repeat;
} MenuKeyInfo;

typedef struct MenuMiddleState {
    u8 pad0;
    s8 state;
    u8 pad2[0x92];
    s8 cursor;
    s8 scroll;
    u8 pad96[2];
    u8 dirty;
} MenuMiddleState;

extern MenuKeyInfo* windowGetKeyInfo(void);
extern u8* windowGetParam(void* window, s32 index);
extern void menuButtonNormal(void* menu);
extern void menuCursorNormal(void* menu);

void menuTaisenButton2(void* menu)
{
    MenuMiddleState* state = menu;
    u8* params;
    s32 index;

    switch (state->state) {
    case 2:
        if (windowGetKeyInfo()->trigger & 0x10) {
            index = state->scroll + state->cursor;
            if (index < 60) {
                params = windowGetParam(menu, 0);
                params[index] = params[index] == 0;
                state->dirty = 0;
                return;
            }
        }
        break;
    }
    menuButtonNormal(menu);
}

void menuTaisenCursor(void* menu)
{
    MenuMiddleState* state = menu;

    switch (state->state) {
    case 2:
        if (state->scroll == 0) {
            if (windowGetKeyInfo()->repeat & 1) {
                state->cursor--;
                if (state->cursor < 0) {
                    state->cursor = 0;
                }
                return;
            }
        } else if (state->scroll == 10) {
            if (windowGetKeyInfo()->repeat & 2) {
                state->cursor++;
                if (state->cursor > 50) {
                    state->cursor = 50;
                }
                return;
            }
        }
        break;
    }
    menuCursorNormal(menu);
}
