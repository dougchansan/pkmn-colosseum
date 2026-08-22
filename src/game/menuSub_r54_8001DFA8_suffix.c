#include "dolphin/types.h"

/* 0x64 | fn_8001DFA8 | generic_call_check_store */
#pragma push
#pragma peephole off
s32 fn_8001DFA8(u32 arg1, u8* arg2) {
    extern u8 menuItemBiosGetSelectFlag(s16);
    if (menuItemBiosGetSelectFlag(*(s16*)(arg2 + 0x6)) != 0) {
        *(u8*)(arg2 + 0x66) = 0xff;
        *(u8*)(arg2 + 0x65) = 0xff;
        *(u8*)(arg2 + 0x64) = 0xff;
    } else {
        *(u8*)(arg2 + 0x66) = 0x80;
        *(u8*)(arg2 + 0x65) = 0x80;
        *(u8*)(arg2 + 0x64) = 0x80;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_8001E00C(u32 sp8) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom(s32, ...);
    extern void menuCloseCustom(s32, s32, s32);
    s32 r31;
    r31 = menuOpenCustom(0x43, windowGetActiveID(), &sp8, 0, 1, 0);
    menuCloseCustom(0x43, 0, 1);
    return r31;
}
#pragma pop

#pragma optimization_level 4
#pragma push
#pragma peephole off
s8 menuSubOpenYesNo(u8 menuType, s16 x, s16 y, s32 initialValue) {
    extern void* windowGetActiveID();
    extern s32 menuOpenCustom(s32, ...);
    extern void menuSetPosition(s32, s16, s16);
    extern void windowCheckCursor(s32, s32);
    extern u32 windowGetValue(s32);
    extern void menuCloseCustom(s32, s32, s32);
    s8 result;
    s16 activeWindowId;
    s16 windowId;

    if (initialValue != 0) initialValue = 1;
    switch (menuType) {
    case 0:
        windowId = 0x11;
        break;
    case 1:
        windowId = 0x12;
        break;
    case 0x7f:
    default:
        windowId = 0x44;
        break;
    }
    activeWindowId = windowId;
    menuOpenCustom((s32)activeWindowId, windowGetActiveID(), &initialValue, 0, 0, 0);
    if (x >= 0 && y >= 0) menuSetPosition((s32)activeWindowId, x, y);
    windowCheckCursor((s32)activeWindowId, 1);
    result = (s8)windowGetValue((s32)activeWindowId);
    menuCloseCustom((s32)activeWindowId, 0, 1);
    return result;
}
#pragma pop

#pragma optimization_level 4
#pragma push
#pragma peephole off
s32 fn_8001E184(void) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void windowCheckCursor();
    extern u32 windowGetValue();
    extern void menuCloseCustom();
    u32 sp8;
    s8 r31;
    sp8 = 0;
    menuOpenCustom(0x12, windowGetActiveID(), &sp8, 0, 0, 0);
    windowCheckCursor(0x12, 0x1);
    r31 = (s8)windowGetValue(0x12);
    menuCloseCustom(0x12, 0x0, 0x1);
    return r31;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma scheduling off
void menuSubCloseNumberInput(void) {
    extern void menuClose();
    menuClose(0x2);
}
#pragma pop

#pragma peephole off
#pragma optimization_level 4
s32 fn_8001E224(void* a, u32* b, u8 c, void* d, void* e, u8 f) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void menuSetPosition();
    extern void windowCheckCursor();
    extern void windowGetValue();
    extern u8* windowSearchID();
    extern void menuClose();
    void* r4;
    u8* r3;
    s32 r31;
    u8 c_val;
    r31 = 0;
    r4 = windowGetActiveID();
    c_val = c;
    menuOpenCustom(0x2, r4, 0, 0, 0, 0x3, a, c_val, 0);
    menuSetPosition(0x2, d, e);
    windowCheckCursor(0x2, 0x1);
    windowGetValue(0x2);
    r3 = windowSearchID(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        if (f != 0) menuClose(0x2);
    }
    return r31;
}
#pragma peephole reset

#pragma peephole off
#pragma optimization_level 4
s32 menuSubOpenNumberInputSub__FUlPUlUcssbPFUl_PUs(void* a, u32* b, void* c) {
    extern void* windowGetActiveID();
    extern void menuOpenCustom(s32, ...);
    extern void menuSetPosition();
    extern void windowCheckCursor();
    extern void windowGetValue();
    extern u8* windowSearchID();
    extern void menuClose();
    void* r4_tmp;
    u8* r3;
    s32 r31;
    r31 = 0;
    r4_tmp = windowGetActiveID();
    menuOpenCustom(0x2, r4_tmp, 0, 0, 0, 0x3, a, 0x1, c);
    menuSetPosition(0x2, 0x32, 0x3c);
    windowCheckCursor(0x2, 0x1);
    windowGetValue(0x2);
    r3 = windowSearchID(0x2);
    if (r3 != 0) {
        if (b != 0) *b = *(u32*)(r3 + 0x80);
        if (*(u8*)(r3 + 0x99) == 0) r31 = 1;
        menuClose(0x2);
    }
    return r31;
}
#pragma peephole reset
