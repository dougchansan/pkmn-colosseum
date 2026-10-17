/* Name-entry 50-sound letter grid, 0x800275F4-0x80027740.
 * RULE-EXCEPTION(user-approved): unit-scope GC/2.0 -opt nopeephole (the shared
 * file's local `peephole off`); see docs/RULE_EXCEPTIONS.md.
 * Source mirrors src/game/menuNameEntry.c. */
#include "dolphin/types.h"

/* Name-entry row table: colour, X-button label and the four letter lists. */
typedef struct NameEntryRgb {
    u8 r;
    u8 g;
    u8 b;
} NameEntryRgb;

typedef struct NameEntryModeEntry {
    NameEntryRgb color;
    u8 pad03;
    u32 xButtonMessage;
    u32 messages[4];
} NameEntryModeEntry;
extern NameEntryModeEntry lbl_80266E18[];

extern void msgctrlSetValue(s32, void*);
extern u32 GSmsgGetRect(u32);
extern void GSmsgPrintColor(s32, s32, s32, u32);
extern s32 GSmsgGetLength(void*);
extern void* GSmsgGetGSchar(u32);

/* Letter `index` of the message list for (row, column), or 0 when out of range. */
static inline u16 menuNameEntryGetLetter(s32 row, s32 index, s32 column)
{
    u32 message;
    s32 length;

    if (row < 0 || row >= 2) {
        return 0;
    }
    if (column < 0 || column >= 4) {
        return 0;
    }
    message = lbl_80266E18[row].messages[column];
    length = GSmsgGetLength((void*)message);
    if (index < 0 || index >= length) {
        return 0;
    }
    return ((u16*)GSmsgGetGSchar(message))[index];
}

/* Draws the 50-sound grid: four columns of letters for the current row. */
s32 menuNameEntryDraw50Text(void* window) {
    u8* self;
    u8* ctx;
    u16* bufp;
    s32 index;
    s32 column;
    s32 row;
    u16 buf[2];
    s32 color;
    u16 letter;

    self = window;
    ctx = *(u8**)(self + 0x60);
    row = **(s32**)(ctx + 0x24);
    bufp = buf;
    for (column = 0; column < 4; column++) {
        index = 0;
        while ((letter = menuNameEntryGetLetter(row, index, column)) != 0) {
            color = self[0x8b] | -0x100;
            bufp[0] = letter;
            bufp[1] = 0;
            msgctrlSetValue(0x37, bufp);
            GSmsgPrintColor(index * 0x1b + (0x1b - (s16)(GSmsgGetRect(0xce) >> 16)) / 2, column * 0x23, color, 0xce);
            index++;
        }
    }
    return 0;
}

