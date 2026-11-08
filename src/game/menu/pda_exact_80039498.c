/**
 * @file pda_exact_80039498.c
 * @brief Byte-exact PDA list callbacks, 0x80039498 - 0x80039644.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4c];
    s32 messageId;
} PdaSprite;

extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4B0;
extern const s32 lbl_80267120[4];
extern void winSpriteSetDisp(void* sprite, s32 disp);

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 itemStorageSelectAction(s32 value)
{
    extern u32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menuId, ...);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 wait);
    s32 parameter;
    s32 choices[4];
    s32 choice;

    parameter = value;
    choices[0] = lbl_80267120[0];
    choices[1] = lbl_80267120[1];
    choices[2] = lbl_80267120[2];
    choices[3] = lbl_80267120[3];
    choice = menuOpenCustom(0x24, windowGetActiveID(), &parameter, 0, 1, 0);
    menuClose(0x24);
    menuCloseSync(0x24, 1);
    if (choice < 0 || choice >= 4) {
        return 4;
    }
    return choices[choice];
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 itemStorageListDrawItem(void* window, PdaSprite* sprite)
{
    s32 messageId;

    (void)window;
    if (lbl_8047A4B0 == 0) {
        messageId = 0x1b6d;
    } else {
        messageId = 0x1b6e;
    }
    sprite->messageId = messageId;
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 itemStorageListDrawSprite(void* window, void* sprite)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern void* pcboxGetItem(s32 box, s16 slot);
    extern u8 itemSlotIsValid(void* item);
    s32 count;
    s32 slot;
    s32 slotCount;
    u16 boundedSlotCount;
    s32 threshold;
    s32 display;

    (void)window;
    count = 0;
    threshold = lbl_8047A4A8 + 8;
    slotCount = pcboxGetNbItemSlot(0);
    boundedSlotCount = slotCount;
    for (slot = 0; slot < boundedSlotCount; slot++) {
        if (itemSlotIsValid(pcboxGetItem(0, slot))) {
            count++;
        }
    }
    if (threshold < count + 1) {
        display = 1;
    } else {
        display = 0;
    }
    winSpriteSetDisp(sprite, display);
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 itemStorageListDrawSprite2(void* window, void* sprite)
{
    s32 disp;

    if (lbl_8047A4A8 > 0) {
        disp = 1;
    } else {
        disp = 0;
    }
    winSpriteSetDisp(sprite, disp);
    return 0;
}
#pragma peephole reset
