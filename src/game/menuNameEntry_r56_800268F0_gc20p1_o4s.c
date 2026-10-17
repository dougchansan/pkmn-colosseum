/* Name-entry name line (layout 7), 0x800268F0-0x80026B44.
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
extern u8 lbl_80266DD8[];
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;

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

/* 6 when the letter differs from the current one, 0 otherwise. */
static inline s32 menuNameEntryGetLetterKind(u16 letter)
{
    u16* current = (u16*)GSmsgGetGSchar(0x2efc);
    if (letter == *current) {
        return 0;
    }
    return 6;
}
/* Draws the entered name and, while there is room, the blinking candidate letter,
 * when the session's name kind uses layout `type`. */
static inline void menuNameEntryDrawName(u8* self, u8* draw, s32 type)
{
    s32* types;
    u16* bufp;
    s32 x;
    u8* ctx;
    s32 count;
    s32 color;
    s32 width;
    s32 row;
    s32 column;
    u16 letter;
    u8 alpha;
    u16 next[2];
    u16 buf[2];
    u16* letters;
    s32 index;

    ctx = *(u8**)(self + 0x60);
    types = (s32*)(lbl_80266DD8 + 4);
    if (types[*(s32*)(ctx + 0x1c) * 4] != type) {
        draw[0x67] = 0;
    } else {
        letters = *(u16**)(ctx + 0x18);
        bufp = buf;
        count = 0;
        x = 0;
        while (*letters != 0) {
            color = self[0x8b] | -0x100;
            bufp[0] = *letters;
            bufp[1] = 0;
            msgctrlSetValue(0x37, bufp);
            width = 0x1b - (s16)(GSmsgGetRect(0xce) >> 16);
            GSmsgPrintColor(x + width / 2, 0, color, 0xce);
            x += 0x1a;
            count++;
            letters++;
        }
        if (count < types[*(s32*)(ctx + 0x1c) * 4] && self[0x98] == 0) {
            row = **(s32**)(ctx + 0x24);
            index = **(s32**)(ctx + 0x28);
            column = **(s32**)(ctx + 0x2c);
            letter = menuNameEntryGetLetter(row, index, column);
            if (letter != 0 && menuNameEntryGetLetterKind(letter) == 6) {
                alpha = (lbl_8047B934 - **(f32**)(ctx + 0x30)) * lbl_8047B938;
                color = alpha | 0xff0000;
                next[0] = letter;
                next[1] = 0;
                msgctrlSetValue(0x37, next);
                GSmsgPrintColor(count * 0x1a + (0x1b - (s16)(GSmsgGetRect(0xce) >> 16)) / 2, 0, color, 0xce);
            }
        }
        draw[0x67] = 0xff;
    }
}

s32 fn_800268F0(void* window, u8* draw)
{
    menuNameEntryDrawName(window, draw, 7);
    return 0;
}

