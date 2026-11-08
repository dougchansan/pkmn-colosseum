/** Exact PDA-mail cursor and animation callbacks, 0x8004CF78 - 0x8004D34C. */
#include "dolphin/types.h"
#include "game/cursor_bios.h"

typedef struct PdaMailWindowA {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
    u8 pad08[0x58];
    s32** field_0x60;
} PdaMailWindowA;

extern u8* windowGetKeyInfo(void);
extern s32 mailGetNbMailInMailbox(void);
extern void winSeqSetMenu(s32, s32);
extern f32 lbl_8047BE18;
extern f32 lbl_8047BE1C;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 pdaMailListKey(u8* window)
{
    typedef union PdaMailCursorState {
        u32 packed;
        struct {
            s8 page;
            s8 row;
        } position;
    } PdaMailCursorState;
    PdaMailCursorState cursor;
    u16 persistedPosition;
    u8* input;
    s32 remaining;

    input = windowGetKeyInfo();
    cursorBiosGetPos(10);
    *(u16*) &cursor.position.page = *(u16*) (window + 0x94);

    if ((*(u16*) (input + 6) & 2) != 0) {
        if (++cursor.position.row > 11) {
            cursor.position.row = 0;
        }
        remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
        if (remaining > 10) {
            remaining = 10;
        } else if (remaining < 0) {
            remaining = 0;
        }
        if (cursor.position.row < 10 && cursor.position.row >= remaining) {
            cursor.position.row = 10;
        }
    }
    if ((*(u16*) (input + 6) & 1) != 0) {
        if (--cursor.position.row < 0) {
            cursor.position.row = 11;
        }
        remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
        if (remaining > 10) {
            remaining = 10;
        } else if (remaining < 0) {
            remaining = 0;
        }
        if (cursor.position.row < 10 && cursor.position.row >= remaining) {
            if (remaining > 0) {
                cursor.position.row = remaining - 1;
            } else {
                cursor.position.row = 11;
            }
        }
    }
    if ((*(u16*) (input + 6) & 8) != 0) {
        if (cursor.position.row < 10) {
            remaining = mailGetNbMailInMailbox();
            remaining = (remaining + 9) / 10;
            if (++cursor.position.page >= remaining) {
                cursor.position.page = 0;
            }
            remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
            if (remaining > 10) {
                remaining = 10;
            } else if (remaining < 0) {
                remaining = 0;
            }
            if (cursor.position.row >= remaining) {
                cursor.position.row = remaining - 1;
            }
        } else {
            cursor.position.row = 11;
        }
    }
    if ((*(u16*) (input + 6) & 4) != 0) {
        if (cursor.position.row < 10) {
            if (--cursor.position.page < 0) {
                remaining = mailGetNbMailInMailbox();
                cursor.position.page = (remaining + 9) / 10 - 1;
            }
            remaining = mailGetNbMailInMailbox() - cursor.position.page * 10;
            if (remaining > 10) {
                remaining = 10;
            } else if (remaining < 0) {
                remaining = 0;
            }
            if (cursor.position.row >= remaining) {
                cursor.position.row = remaining - 1;
            }
        } else {
            cursor.position.row = 10;
        }
    }

    persistedPosition = *(u16*) &cursor.position.page;
    cursorBiosSetPos(10, &persistedPosition);
    *(u16*) (window + 0x94) = *(u16*) &cursor.position.page;
    return 0;
}
#pragma peephole reset

/* RULE-EXCEPTION(user-approved): local peephole control and a no-op scalar
 * copy preserve retail FP allocation; see docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
s32 pdaMailListCtrl(PdaMailWindowA* window)
{
    s32** field = window->field_0x60;
    switch (window->phase) {
    case 0:
        if (window->guard == 0) {
            winSeqSetMenu(window->msgObj, 0x1c2);
            window->guard = 1;
        }
        break;
    case 2: {
        f32 result;
        f32 thresh = lbl_8047BE1C;
        f32 val = *(f32*)*field;
        val = val;
        result = val + lbl_8047BE18;
        *(f32*)*field = result;
        if (result >= thresh) {
            *(f32*)*field -= thresh;
        }
        break;
    }
    case 3:
        if (window->guard == 0) {
            winSeqSetMenu(window->msgObj, 0x1c6);
            window->guard = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
