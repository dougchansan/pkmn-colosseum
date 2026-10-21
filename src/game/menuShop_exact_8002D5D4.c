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
extern u32 shopFormatTextB(u8* buffer, s32 location, s32 field, s32 value, ...);
extern u32 shopFormatTextA(u8* buffer, s32 location, s32 field, s32 value, ...);
extern void shopMsgOpenA(u8* location, s32 message, s32 terminator, ...);
extern void shopMsgOpenB(u8* location, s32 message, s32 terminator, ...);
extern void shopPurchaseRun(u8* location, u8 type);
extern void shopBuyRun(s32 location, u8 type);
extern void shopCouponRun(s32 location, u32 type);
extern void shopBuyMenu(u32 location, u32 type);
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
        u32 message = shopFormatTextB(&text1, location, 0, 0x4B, value, -1);
        winMsgOpenWithSE(2, message, 1, 0, text1);
        while ((menu_result = shopQueryMenu(0x62),
                shopNormalizeMenu62(menu_result, &selection), selection != 2)) {
            winMsgClose(1);
            switch (selection) {
            case 0:
                shopBuyRun(location, type);
                break;
            case 1:
                bagMenuOpen(3, location, 0);
                break;
            }
            message = shopFormatTextB(&text1, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text1);
        }

        shopMsgOpenB((ShopLocationArgument*)location, 2, -1);
        break;
    }
    case 1:
        shopPurchaseRun((ShopLocationArgument*)location, type);
        break;
    default: {
        u32 message;

        done = 0;
        memory = shopMemoryAlloc();
        lbl_8047A3DC = (u32)GSmemLock(memory);
        message = shopFormatTextA(&text0, location, 0, -1);
        winMsgOpenWithSE(2, message, 1, 0, text0);
        while ((menu_result = shopNormalizeMenu83(shopQueryMenu(0x83))) != 3) {
            winMsgClose(1);
            switch (menu_result) {
            case 0:
                shopCouponRun(location, type);
                break;
            case 1:
                shopBuyMenu(location, type);
                break;
            case 2:
                shopMsgOpenA((ShopLocationArgument*)location, 0xB, -1);
                break;
            case 3:
                done = 1;
                break;
            }
            if (done != 0) {
                break;
            }
            message = shopFormatTextA(&text0, location, 1, -1);
            winMsgOpenWithSE(2, message, 1, 0, text0);
        }

        if (type != 3 && type != 2) {
            shopMsgOpenA((ShopLocationArgument*)location, 2, -1);
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
