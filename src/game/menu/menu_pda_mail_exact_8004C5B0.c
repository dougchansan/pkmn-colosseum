/** Exact PDA-mail page-row widget, 0x8004C5B0 - 0x8004C6C0. */
#include "dolphin/types.h"
#include "game/cursor_bios.h"

typedef struct PdaMailSpriteField {
    u8 pad00[6];
    s16 msgId;
} PdaMailSpriteField;

extern void winSpriteSetDisp(void*, s32);
extern const s32 lbl_802671D0[12];

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 pdaMailDrawCursor(void* unused, PdaMailSpriteField* field)
{
    typedef union PdaMailCursorPosition {
        u32 storage;
        u16 packed;
        struct {
            s8 page;
            s8 row;
        } position;
    } PdaMailCursorPosition;
    PdaMailCursorPosition cursors[2];
    s32 i;

    cursors[1].packed = cursors[0].packed =
        (u16) (cursorBiosGetPos(10) >> 16);
    for (i = 0; i < 12; i++) {
        if (field->msgId == lbl_802671D0[i]) {
            break;
        }
    }
    if (i >= 12) {
        return 0;
    }
    if (i == cursors[1].position.row) {
        winSpriteSetDisp(field, 1);
    } else {
        winSpriteSetDisp(field, 0);
    }
    return 0;
}
#pragma peephole reset
