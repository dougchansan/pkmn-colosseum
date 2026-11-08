/**
 * @file pda_exact_80039004.c
 * @brief Byte-exact PDA row callbacks, 0x80039004 - 0x80039128.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x50];
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

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
s32 itemStorageMenuDrawItem(PdaSprite* context, PdaSprite* sprite)
{
    extern f32 lbl_803A65B0[][3];
    s32 index;

    index = context->selectedIndex;
    if (index < 0 || index >= 8) {
        index = 0;
    }
    sprite->field_50 = (s16)lbl_803A65B0[index][0];
    sprite->field_52 = (s16)lbl_803A65B0[index][1];
    *(s8*)((u8*)sprite + 0x67) = lbl_803A65B0[index][2];
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 itemStorageMenuCursor(PdaSelectionWork* work)
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
