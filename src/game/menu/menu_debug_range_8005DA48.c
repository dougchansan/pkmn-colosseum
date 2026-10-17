/**
 * @file menu_debug_range_8005DA48.c
 * @brief debug menu TU, 0x8005DA48 - 0x8005DFC8.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. dbgMenuLog-family / dbgMenuFieldCamera-family /
 * menuDbgItem-family cluster in XD's dbg menu source (XD 0x8000DEE8-0x8000E53C). 2 XD anchors
 * (dbgMenuFieldCameraChangeDisp, menuDbgItemCreate), monotonic with exact
 * size matches. All functions asm-only.
 */
#include "dolphin/types.h"

#if !defined(MENU_DEBUG_RANGE_SPLIT) || defined(MENU_DEBUG_LOG_CHANGE_DISP)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 dbgMenuLogChangeDisp(void)
{
    extern u8 menuIsCheck(s32 menuId);
    extern void menuClose(s32 menuId);
    extern s32 menuOpenCustom(s32 menuId, ...);

    if (menuIsCheck(0xBB)) {
        menuClose(0xBB);
    } else {
        menuOpenCustom(0xBB, 0, 0, 0, 1, 0);
    }

    return 0;
}
#pragma peephole reset
#endif

typedef struct DebugMenuKeyInfo {
    u8 _0[4];
    u16 trigger;
    u16 repeat;
} DebugMenuKeyInfo;

#if !defined(MENU_DEBUG_RANGE_SPLIT) || defined(MENU_DEBUG_LOG_DRAW)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 dbgMenuLogDraw(void* window)
{
    extern DebugMenuKeyInfo* windowGetKeyInfo(void);
    extern s32 windowGetParam(void*, s32);
    extern void windowSetParam(void*, s32, s32);
    extern void gfxSetZMode(s32, s32, s32);
    extern void menuDrawRectOutline(s32, s32, s32, s32, u32*);
    extern s32 GSlogGetLineCount(void);
    extern char* GSlogGetLine(s32);
    extern void GSmsgPrintf(s32, s32, u32, char*, ...);
    extern u32 lbl_8047BF48;
    u32 color[2];
    s32 y;
    s32 i;
    s32 lines;
    s32 line;

    color[1] = lbl_8047BF48;
    if (windowGetKeyInfo()->trigger & 0x100) {
        windowSetParam(window, 0, (windowGetParam(window, 0) + 1) % 4);
    }

    {
        s32 width;

        switch (windowGetParam(window, 0)) {
        case 0:
        default:
            lines = 10;
            width = 0x82;
            y = 0x145;
            break;
        case 1:
            lines = 10;
            width = 0x82;
            y = 0x27;
            break;
        case 2:
            lines = 0x20;
            width = 0x1A0;
            y = 0x27;
            break;
        case 3:
            lines = 1;
            width = 0xD;
            y = 0x1BA;
            break;
        }

        gfxSetZMode(0, 7, 2);
        color[0] = color[1];
        menuDrawRectOutline(0xF, y - 5, 0x25D, width + 0x12, &color[0]);
    }
    for (i = 0; i < lines; i++) {
        line = i + GSlogGetLineCount() - lines;
        if (line >= 0) {
            GSmsgPrintf(0x14, y, 0xC0C0C0FF, GSlogGetLine(line));
        }
        y += 13;
    }
    return 0;
}
#pragma pop
#endif

#if !defined(MENU_DEBUG_RANGE_SPLIT) || defined(MENU_DEBUG_ITEM_CURSOR)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 menuDbgItemCreateCursor(u8* window)
{
    extern DebugMenuKeyInfo* windowGetKeyInfo(void);
    extern s32 lbl_8047A5C4;
    extern s32 lbl_8047A5C8;
    extern s32 lbl_80478BD8;
    DebugMenuKeyInfo* keys;

    keys = windowGetKeyInfo();
    if (keys->repeat & 1) {
        if ((s8)--window[0x95] < 0) {
            window[0x95] = 0;
        }
    }
    if (keys->repeat & 2) {
        if ((s8)++window[0x95] > 1) {
            window[0x95] = 1;
        }
    }

    switch ((s8)window[0x95]) {
    case 0:
        keys = windowGetKeyInfo();
        if (keys->repeat & 8) {
            lbl_8047A5C8++;
            if (lbl_8047A5C8 >= lbl_80478BD8) lbl_8047A5C8 = lbl_80478BD8 - 1;
        }
        if (keys->repeat & 0x400) {
            lbl_8047A5C8 += 10;
            if (lbl_8047A5C8 >= lbl_80478BD8) lbl_8047A5C8 = lbl_80478BD8 - 1;
        }
        if (keys->repeat & 4) {
            lbl_8047A5C8--;
            if (lbl_8047A5C8 < 0) lbl_8047A5C8 = 0;
        }
        if (keys->repeat & 0x200) {
            lbl_8047A5C8 -= 10;
            if (lbl_8047A5C8 < 0) lbl_8047A5C8 = 0;
        }
        break;
    case 1:
        keys = windowGetKeyInfo();
        if (keys->repeat & 8) {
            lbl_8047A5C4++;
            if (lbl_8047A5C4 > 999) lbl_8047A5C4 = 999;
        }
        if (keys->repeat & 0x400) {
            lbl_8047A5C4 += 10;
            if (lbl_8047A5C4 > 999) lbl_8047A5C4 = 999;
        }
        if (keys->repeat & 4) {
            lbl_8047A5C4--;
            if (lbl_8047A5C4 < 0) lbl_8047A5C4 = 0;
        }
        if (keys->repeat & 0x200) {
            lbl_8047A5C4 -= 10;
            if (lbl_8047A5C4 < 0) lbl_8047A5C4 = 0;
        }
        break;
    }
    return 0;
}
#pragma pop
#endif

#if !defined(MENU_DEBUG_RANGE_SPLIT) || defined(MENU_DEBUG_FIELD_CAMERA)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 dbgMenuFieldCameraChangeDisp(void)
{
    extern u8 menuIsCheck(s32 menuId);
    extern s32 menuOpenCustom(s32 menuId, ...);
    extern void menuSetPosition(s32 menuId, s32 x, s32 y);
    extern u32 std092_getCurrentRoomID(void);
    extern void fn_801176C8(u32 floorId);
    extern u32 fn_80117AD4(void);
    extern u32 lbl_8047A5B0;
    extern u32 lbl_8047A5B4;
    extern u32 lbl_8047A5B8;
    extern u8 lbl_8047A5BC;
    u32 floorId;

    if (!menuIsCheck(0xCA)) {
        floorId = std092_getCurrentRoomID();
        if (floorId != fn_80117AD4()) {
            fn_801176C8(floorId);
            lbl_8047A5B0 = 0;
            lbl_8047A5B4 = 0;
            lbl_8047A5B8 = 0;
        }

        lbl_8047A5BC = 0;
        menuOpenCustom(0xCA, 0, 0, 0, 1, 0);
        menuSetPosition(0xCA, 0xC, 0xA);
    }

    return 0;
}
#pragma peephole reset
#endif

#if !defined(MENU_DEBUG_RANGE_SPLIT) || defined(MENU_DEBUG_ITEM_CREATE)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void menuDbgItemCreate(void)
{
    extern s32 menuOpen(s32 menuId, s32 mode);
    extern u8 fn_80142984(u16 itemId);
    extern void menuCloseCustom(s32 menuId, s32 mode, s32 wait);
    extern s32 heroItemAddItemDataId(void* hero, u16 itemId, u16 count,
                                     s32 slot);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 wait);
    extern s32 lbl_8047A5C0;
    extern s32 lbl_8047A5C4;
    extern s32 lbl_8047A5C8;
    s32 result;

    if (lbl_8047A5C0 == 0) {
        lbl_8047A5C8 = 1;
        lbl_8047A5C0 = 1;
    }

    lbl_8047A5C4 = 1;
    for (;;) {
        result = menuOpen(0xCB, 1);
        if (result == -1) {
            break;
        }
        if (fn_80142984((u16)lbl_8047A5C8) == 0) {
            continue;
        }
        if (lbl_8047A5C4 < 1 || lbl_8047A5C4 > 999) {
            continue;
        }

        result = menuOpen(0x44, 1);
        menuCloseCustom(0x44, 0, 1);
        if (result == 0) {
            heroItemAddItemDataId(0, (u16)lbl_8047A5C8,
                                  (u16)lbl_8047A5C4, -1);
        }
    }

    menuClose(0xCB);
    menuCloseSync(0xCB, 1);
}
#pragma peephole reset
#endif
