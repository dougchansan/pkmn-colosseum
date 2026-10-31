/**
 * @file pda_exact_800398D4.c
 * @brief Byte-exact PDA item-list callbacks, 0x800398D4 - 0x80039A50.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x50];
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
} PdaSprite;

extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4AC;
extern s32 lbl_8047A4B8;
extern s32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern void winSpriteSetDisp(void* sprite, s32 disp);

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 itemStorageDrawList(void* work, PdaSprite* sprite)
{
    extern void spriteSetScissor(s32 x1, s32 y1, s32 x2, s32 y2);
    extern void spriteClearScissor(void);
    extern s32 itemStorageDrawItem(void* window, void* sprite);
    s32 x;
    s32 y;

    x = sprite->field_50;
    y = sprite->field_52;
    spriteSetScissor(0x118 - x, 0x8b - y, 0x150, 0x10d);
    itemStorageDrawItem((u8*)work + 0x94, *(void**)((u8*)work + 0x88));
    spriteClearScissor();
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 itemStorageListDrawItem2(void* window, PdaSprite* sprite)
{
    s32 y;

    (void)window;
    y = lbl_8047A4AC * 31 + 0x9a;
    if (lbl_8047A4BC == 0) {
        y += (s32)lbl_8047A4C0;
    }
    sprite->field_52 = (s16)y;
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 itemStorageListDrawSprite3(void* window, PdaSprite* sprite)
{
    s32 y;
    s32 disp;

    (void)window;
    if (lbl_8047A4B8 >= 0) {
        y = (lbl_8047A4B8 - lbl_8047A4A8) * 31 + 0x97;
        if (lbl_8047A4BC != 0) {
            y -= (s32)lbl_8047A4C0;
        }
        if (y + sprite->y < 0x97 || y >= 0x18f) {
            disp = 0;
        } else {
            disp = 1;
        }
        winSpriteSetDisp(sprite, disp);
    } else {
        y = lbl_8047A4AC * 31 + 0x97;
        if (lbl_8047A4BC == 0) {
            y += (s32)lbl_8047A4C0;
        }
        winSpriteSetDisp(sprite, 1);
    }
    sprite->field_52 = (s16)y;
    return 0;
}
#pragma peephole reset
