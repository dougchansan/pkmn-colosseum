/**
 * @file menuShop_exact_8002D5D4.c
 * @brief Run the world-map travel cancellation dialog.
 */
#include "dolphin/types.h"

typedef struct ShopLocationEntry {
    u8 field_0;
    u8 type;
    u8 field_2[2];
} ShopLocationEntry;

typedef u8 ShopLocationArgument;

extern u32 lbl_8047A3FC;
extern u32 lbl_80478E54;
extern u32 lbl_8047A3DC;
extern s32 menuOpen(s32 menu, s32 flag);
extern void menuClose(s32 menu);
extern void menuCloseSync(s32 menu, s32 flag);
extern u32 heroGetStatus(u8* ptr, u32 selector, u32 index);
extern u32 fn_8002A0B8(u8* buffer, s32 location, s32 field, s32 value, ...);
extern u32 fn_80029FAC(u8* buffer, s32 location, s32 field, s32 value, ...);
extern void fn_8002A1C4(u8* location, s32 message, s32 terminator, ...);
extern void fn_8002A2CC(u8* location, s32 message, s32 terminator, ...);
extern void fn_8002CE6C(u8* location, u8 type);
extern void fn_8002D154(s32 location, u8 type);
extern void fn_8002C408(s32 location, u32 type);
extern void fn_8002C284(u32 location, u32 type);
extern void winMsgOpenWithSE(s32 kind, u32 message, s32 arg2, s32 arg3,
                             u8 format);
extern void winMsgClose(s32 mode);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u32 handle);
extern void fn_800E24B0(u32 handle);
extern void GSmemFree(u32 handle);
extern void floorBattleEndTerminateScripts(void);
extern void floorSetFadeScript(s32 mode, u32 value);

static inline s32 shopQueryMenu(s32 menu)
{
    s32 result = menuOpen(menu, 1);

    menuClose(menu);
    menuCloseSync(menu, 1);
    return result;
}

static inline void shopNormalizeMenu62(s32 result, s32* selection)
{
    if (result == -1 || result == 2) {
        *selection = 2;
    } else if (result == 0) {
        *selection = 0;
    } else {
        *selection = 1;
    }
}

static inline s32 shopNormalizeMenu83(s32 result)
{
    switch (result) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    default:
        return 3;
    }
}

static inline u32 shopMemoryAlloc(void)
{
    return _toolentryAlloc__FUl(0x7198);
}

void std026_unkMenuSequence26(void)
{
    ShopLocationEntry* location_entry;
    u32 location_offset;
    s32 location;
    u8 type;
    s32 menu_result;
    s32 done;
    u32 memory;
    s32 selection;
    u8 text1;
    u8 text0;

    location_offset = lbl_8047A3FC;
    location = location_offset;
    location_entry = (ShopLocationEntry*)lbl_80478E54;
    location_entry += location_offset;
    type = location_entry->type;

    switch (type) {
    case 0: {
        u32 value = heroGetStatus(NULL, 0xC, 0);
        u32 message = fn_8002A0B8(&text1, location, 0, 0x4B, value, -1);
        winMsgOpenWithSE(2, message, 1, 0, text1);
        while ((menu_result = shopQueryMenu(0x62),
                shopNormalizeMenu62(menu_result, &selection), selection != 2)) {
            winMsgClose(1);
            switch (selection) {
            case 0:
                fn_8002D154(location, type);
                break;
            case 1:
                fn_80018F54(3, location, 0);
                break;
            }
            message = fn_8002A0B8(&text1, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text1);
        }

        fn_8002A2CC((ShopLocationArgument*)location, 2, -1);
        break;
    }
    case 1:
        fn_8002CE6C((ShopLocationArgument*)location, type);
        break;
    default: {
        u32 message;

        done = 0;
        memory = shopMemoryAlloc();
        lbl_8047A3DC = (u32)GSmemLock(memory);
        message = fn_80029FAC(&text0, location, 0, -1);
        winMsgOpenWithSE(2, message, 1, 0, text0);
        while ((menu_result = shopNormalizeMenu83(shopQueryMenu(0x83))) != 3) {
            winMsgClose(1);
            switch (menu_result) {
            case 0:
                fn_8002C408(location, type);
                break;
            case 1:
                fn_8002C284(location, type);
                break;
            case 2:
                fn_8002A1C4((ShopLocationArgument*)location, 0xB, -1);
                break;
            case 3:
                done = 1;
                break;
            }
            if (done != 0) {
                break;
            }
            message = fn_80029FAC(&text0, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text0);
        }

        if (type != 3 && type != 2) {
            fn_8002A1C4((ShopLocationArgument*)location, 2, -1);
        } else {
            winMsgClose(1);
        }
        fn_800E24B0(memory);
        GSmemFree(memory);
        break;
    }
    }

    if ((s32)*(&lbl_8047A3FC + 1) != 0) {
        floorBattleEndTerminateScripts();
        floorSetFadeScript(0, 0);
    }
}
