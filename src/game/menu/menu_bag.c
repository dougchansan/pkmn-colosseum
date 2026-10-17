/**
 * @file menu_bag.c
 * @brief Bag / item pocket management interface.
 *
 * Implements the bag screen where the player views and manages items across
 * different pockets (Items, Key Items, Poke Balls, TMs/HMs, Berries).
 * Handles item selection, use, give-to-Pokemon, and toss operations.
 *
 * Key behaviors:
 *   - Uses BSS lbl_803A6AB0 (0x2B38 = 11,064 bytes) as the bag state buffer.
 *     This is the largest BSS allocation in the gap -- it stores cached copies
 *     of all item data for fast display (553 species * 0x14 bytes = 0x2B24).
 *   - fn_8004EADC and fn_8004EC54 are initialization functions that copy
 *     item data from the save file (via fightTrainerAiValueAddsubDataBiosGetPtr) into the local cache,
 *     iterating 0x229 (553) times with 0x14-byte entries.
 *   - dbgMenuTrainerAiEditA, fn_8004F860, fn_8004FE3C and fn_80050844 share one
 *     template: reload the species cache, then run a scene-message loop
 *     (menuOpenCustom/menuGetCursorItemID) that dispatches per-message onto
 *     HANDLE_BAG_VALUE, which reads a numeric field from the on-screen input
 *     widget (fn_8001E224), clamps it to +-0xC8, and commits it
 *     (fightTrainerSetStatus). Each of the four owns a distinct scene model
 *     (0x87/0x8A/0x8B/0x8C) and a distinct set of message-ID -> field-slot
 *     pairs; the message IDs are mostly a contiguous per-pocket range with
 *     one or two special IDs (e.g. registered/key-item actions) spliced in.
 *   - Scene model indices 0x87, 0x89, 0x8A, 0x8B, 0x8C used for bag UI
 *     elements (one per pocket / sub-screen)
 *   - Checks scene model 0x44 for resource availability
 *
 * BSS usage:
 *   - lbl_803A6AB0 (0x2B38 bytes): Bag item cache (553 species * 20 bytes)
 *
 * Address range: 0x8004EADC - 0x80051710 (6 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  _threadSwitch(void);           /* GSthread yield */
extern u32   menuCloseCustom(u32 a, u32 b, u32 c); /* scene load */
extern s32   menuOpen(u32 a, u32 b);        /* scene query */
extern s32   menuOpenCustom(u32 sceneId, u32 a, u32 b, u32 c,
                         u32 d, u32 e, ...);
extern u32   menuGetCursorItemID(u32 a);                /* scene message get */
extern void  winSpriteSetDisp(u32 obj, u8 visible);  /* model visibility */
extern void  GSmsgPrintColor(u32 a, u32 b, s32 c, u32 d); /* sound trigger */
extern void  msgctrlSetValue(u32 effectId, u32 param);

/* ===== Pokemon data ===== */
extern void* fightTrainerAiValueAddsubDataBiosGetPtr(u32 species);    /* Species base data get */
extern void  fightTrainerSetStatus(u32 a, u32 b, u32 c, u32 d, s32 e);
extern u32   fightTrainerGetStatus(u32 a, u32 b, u32 c, u32 d);

/* ===== Text / Messages ===== */
extern u8    fn_8001E224(u32 msgBank, s32* out, u32 a, u32 b, u32 c, u32 d);
extern void  menuSubCloseNumberInput(void);

typedef struct BagCacheEntry {
    u32 word0;
    u32 word1;
    u32 word2;
    u32 word3;
    u32 word4;
} BagCacheEntry;

/* ===== BSS data ===== */
extern BagCacheEntry lbl_803A6AB0[];   /* Bag item cache (0x2B38 bytes) */

/*
 * Clamp a numeric bag-value field (IV/EV-style stat, quantity, etc.) read via
 * the on-screen numeric input widget, then write it back.
 *   fightTrainerGetStatus(0, slot_, 0x3E, 0)      -- resolve the widget's message bank
 *   fn_8001E224(bank, &value_, 0,0x32,0x32,0) -- read+validate the entered value
 *   fightTrainerSetStatus(0, slot_, 0x3E, 0, value_)     -- commit the clamped value
 *   menuSubCloseNumberInput()                              -- close the widget
 */
#define HANDLE_BAG_VALUE(slot_, value_)                                           \
        if (fn_8001E224(fightTrainerGetStatus(0, (slot_), 0x3E, 0),                         \
                        &(value_), 0, 0x32, 0x32, 0) == 1) {                      \
            if ((value_) > 0xC8) {                                                \
                (value_) = 0xC8;                                                  \
            }                                                                     \
            if ((value_) < -0xC8) {                                               \
                (value_) = -0xC8;                                                 \
            }                                                                     \
            fightTrainerSetStatus(0, (slot_), 0x3E, 0, (value_));                           \
        }                                                                         \
        menuSubCloseNumberInput()

/*
 * Functions in this translation unit (6 total):
 *
 * fn_8004EADC  0x178  Bag init A (copy item data, 553 species loop, scene 0x89)
 * fn_8004EC54  0x178  Bag init B (copy item data, 553 species loop, scene 0x8A)
 * dbgMenuTrainerAiEditA  0xA94  Item sort/filter handler (2708 bytes)
 * fn_8004F860  0x5DC  Scroll position / item list display (1500 bytes)
 * fn_8004FE3C  0xA08  Item selection confirmation (2568 bytes)
 * fn_80050844  0xECC  Bag main update loop (3788 bytes, msg switch 0x5BC-0xFEC)
 */

/* 0x8004EADC | size: 0x178 */
s32 fn_8004EADC(void) {
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
    u32 index;
    BagCacheEntry* src;
    BagCacheEntry* dst;
    s32 result;

    index = 0;
    while ((u16)index < 0x229) {
        src = fightTrainerAiValueAddsubDataBiosGetPtr(index);
        dst = &lbl_803A6AB0[(u16)index];
        index++;
        *dst = *src;
    }

    while (1) {
        result = menuOpenCustom(0x89, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(0x89, 0, 1);

            index = 0;
            while ((u16)index < 0x229) {
                dst = fightTrainerAiValueAddsubDataBiosGetPtr(index);
                src = &lbl_803A6AB0[(u16)index];
                index++;
                *dst = *src;
            }

            return -1;
        }

        if (result == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
            } else {
                menuCloseCustom(0x44, 0, 1);
                menuCloseCustom(0x89, 0, 1);
                return 1;
            }
        }
    }
}

/* 0x8004EC54 | size: 0x178 */
s32 fn_8004EC54(void) {
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
    u32 index;
    BagCacheEntry* src;
    BagCacheEntry* dst;
    s32 result;

    index = 0;
    while ((u16)index < 0x229) {
        src = fightTrainerAiValueAddsubDataBiosGetPtr(index);
        dst = &lbl_803A6AB0[(u16)index];
        index++;
        *dst = *src;
    }

    while (1) {
        result = menuOpenCustom(0x88, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(0x88, 0, 1);

            index = 0;
            while ((u16)index < 0x229) {
                dst = fightTrainerAiValueAddsubDataBiosGetPtr(index);
                src = &lbl_803A6AB0[(u16)index];
                index++;
                *dst = *src;
            }

            return -1;
        }

        if (result == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
            } else {
                menuCloseCustom(0x44, 0, 1);
                menuCloseCustom(0x88, 0, 1);
                return 1;
            }
        }
    }
}

/* 0x8004EDCC | size: 0xA94 */
s32 dbgMenuTrainerAiEditA(void) {
    s32 value16;
    s32 value15;
    s32 value14;
    s32 value13;
    s32 value12;
    s32 value11;
    s32 value10;
    s32 value9;
    s32 value8;
    s32 value7;
    s32 value6;
    s32 value5;
    s32 value4;
    s32 value3;
    s32 value2;
    s32 value1;
    s32 value0;
    u32 index;
    BagCacheEntry* src;
    BagCacheEntry* dst;
    s32 result;
    u32 msg;

    for (index = 0; (u16)index < 0x229; index++) {
        src = fightTrainerAiValueAddsubDataBiosGetPtr(index);
        dst = &lbl_803A6AB0[(u16)index];
        *dst = *src;
    }

    while (1) {
        result = menuOpenCustom(0x8C, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(0x8C, 0, 1);

            for (index = 0; (u16)index < 0x229; index++) {
                dst = fightTrainerAiValueAddsubDataBiosGetPtr(index);
                src = &lbl_803A6AB0[(u16)index];
                *dst = *src;
            }

            return -1;
        }

        if (result == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }

            menuCloseCustom(0x44, 0, 1);
            break;
        }

        msg = menuGetCursorItemID(0x8C);
        switch (msg) {
        case 0x5E8:
            HANDLE_BAG_VALUE(0x3C, value16);
            continue;
        case 0x5E9:
            HANDLE_BAG_VALUE(0x3D, value15);
            continue;
        case 0xEF0:
            HANDLE_BAG_VALUE(0x3E, value14);
            continue;
        case 0x5EA:
            HANDLE_BAG_VALUE(0x3F, value13);
            continue;
        case 0x5EB:
            HANDLE_BAG_VALUE(0x40, value12);
            continue;
        case 0x5EC:
            HANDLE_BAG_VALUE(0x44, value11);
            continue;
        case 0x5ED:
            HANDLE_BAG_VALUE(0x48, value10);
            continue;
        case 0x5EE:
            HANDLE_BAG_VALUE(0x49, value9);
            continue;
        case 0x5EF:
            HANDLE_BAG_VALUE(0x4B, value8);
            continue;
        case 0x5F0:
            HANDLE_BAG_VALUE(0x4C, value7);
            continue;
        case 0x5F1:
            HANDLE_BAG_VALUE(0x4A, value6);
            continue;
        case 0x5F2:
            HANDLE_BAG_VALUE(0x45, value5);
            continue;
        case 0x5F3:
            HANDLE_BAG_VALUE(0x46, value4);
            continue;
        case 0x5F4:
            HANDLE_BAG_VALUE(0x47, value3);
            continue;
        case 0x5F5:
            HANDLE_BAG_VALUE(0x41, value2);
            continue;
        case 0x5F6:
            HANDLE_BAG_VALUE(0x42, value1);
            continue;
        case 0x5F7:
            HANDLE_BAG_VALUE(0x43, value0);
            continue;
        default:
            continue;
        }
    }

    menuCloseCustom(0x8C, 0, 1);
    return 1;
}

/* 0x8004F860 | size: 0x5DC */
s32 fn_8004F860(void) {
    s32 value7;
    s32 value6;
    s32 value5;
    s32 value4;
    s32 value3;
    s32 value2;
    s32 value1;
    s32 value0;
    u32 index;
    BagCacheEntry* src;
    BagCacheEntry* dst;
    s32 result;
    u32 msg;

    for (index = 0; (u16)index < 0x229; index++) {
        src = fightTrainerAiValueAddsubDataBiosGetPtr(index);
        dst = &lbl_803A6AB0[(u16)index];
        *dst = *src;
    }

    while (1) {
        result = menuOpenCustom(0x87, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(0x87, 0, 1);

            for (index = 0; (u16)index < 0x229; index++) {
                dst = fightTrainerAiValueAddsubDataBiosGetPtr(index);
                src = &lbl_803A6AB0[(u16)index];
                *dst = *src;
            }

            return -1;
        }

        if (result == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }

            menuCloseCustom(0x44, 0, 1);
            break;
        }

        msg = menuGetCursorItemID(0x87);
        switch (msg) {
        case 0x5E3:
            HANDLE_BAG_VALUE(0x34, value7);
            continue;
        case 0x5E4:
            HANDLE_BAG_VALUE(0x35, value6);
            continue;
        case 0x5E5:
            HANDLE_BAG_VALUE(0x36, value5);
            continue;
        case 0x5E6:
            HANDLE_BAG_VALUE(0x37, value4);
            continue;
        case 0xFD5:
            HANDLE_BAG_VALUE(0x38, value3);
            continue;
        case 0xFE9:
            HANDLE_BAG_VALUE(0x39, value2);
            continue;
        case 0xFEA:
            HANDLE_BAG_VALUE(0x3A, value1);
            continue;
        case 0xFEB:
            HANDLE_BAG_VALUE(0x3B, value0);
            continue;
        default:
            continue;
        }
    }

    menuCloseCustom(0x87, 0, 1);
    return 1;
}

/* 0x8004FE3C | size: 0xA08 */
s32 fn_8004FE3C(void) {
    s32 value15;
    s32 value14;
    s32 value13;
    s32 value12;
    s32 value11;
    s32 value10;
    s32 value9;
    s32 value8;
    s32 value7;
    s32 value6;
    s32 value5;
    s32 value4;
    s32 value3;
    s32 value2;
    s32 value1;
    s32 value0;
    u32 index;
    BagCacheEntry* src;
    BagCacheEntry* dst;
    s32 result;
    u32 msg;

    for (index = 0; (u16)index < 0x229; index++) {
        src = fightTrainerAiValueAddsubDataBiosGetPtr(index);
        dst = &lbl_803A6AB0[(u16)index];
        *dst = *src;
    }

    while (1) {
        result = menuOpenCustom(0x8B, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(0x8B, 0, 1);

            for (index = 0; (u16)index < 0x229; index++) {
                dst = fightTrainerAiValueAddsubDataBiosGetPtr(index);
                src = &lbl_803A6AB0[(u16)index];
                *dst = *src;
            }

            return -1;
        }

        if (result == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }

            menuCloseCustom(0x44, 0, 1);
            break;
        }

        msg = menuGetCursorItemID(0x8B);
        switch (msg) {
        case 0x5D4:
            HANDLE_BAG_VALUE(0x1A, value15);
            continue;
        case 0xFD3:
            HANDLE_BAG_VALUE(0x1B, value14);
            continue;
        case 0xFD4:
            HANDLE_BAG_VALUE(0x1C, value13);
            continue;
        case 0x5D5:
            HANDLE_BAG_VALUE(0x1E, value12);
            continue;
        case 0x5D6:
            HANDLE_BAG_VALUE(0x1F, value11);
            continue;
        case 0x5D7:
            HANDLE_BAG_VALUE(0x20, value10);
            continue;
        case 0x5D8:
            HANDLE_BAG_VALUE(0x21, value9);
            continue;
        case 0x5D9:
            HANDLE_BAG_VALUE(0x22, value8);
            continue;
        case 0x5DA:
            HANDLE_BAG_VALUE(0x23, value7);
            continue;
        case 0x5DB:
            HANDLE_BAG_VALUE(0x24, value6);
            continue;
        case 0x5DC:
            HANDLE_BAG_VALUE(0x1D, value5);
            continue;
        case 0x5DD:
            HANDLE_BAG_VALUE(0x2D, value4);
            continue;
        case 0x5DE:
            HANDLE_BAG_VALUE(0x25, value3);
            continue;
        case 0x5DF:
            HANDLE_BAG_VALUE(0x26, value2);
            continue;
        case 0x5E0:
            HANDLE_BAG_VALUE(0x27, value1);
            continue;
        case 0x5E1:
            HANDLE_BAG_VALUE(0x28, value0);
            continue;
        default:
            continue;
        }
    }

    menuCloseCustom(0x8B, 0, 1);
    return 1;
}

/* 0x80050844 | size: 0xECC */
s32 fn_80050844(void) {
    s32 dummy1;
    s32 dummy0;
    s32 value24;
    s32 value23;
    s32 value22;
    s32 value21;
    s32 value20;
    s32 value19;
    s32 value18;
    s32 value17;
    s32 value16;
    s32 value15;
    s32 value14;
    s32 value13;
    s32 value12;
    s32 value11;
    s32 value10;
    s32 value9;
    s32 value8;
    s32 value7;
    s32 value6;
    s32 value5;
    s32 value4;
    s32 value3;
    s32 value2;
    s32 value1;
    s32 value0;
    u32 index;
    BagCacheEntry* src;
    BagCacheEntry* dst;
    s32 result;
    u32 msg;

    for (index = 0; (u16)index < 0x229; index++) {
        src = fightTrainerAiValueAddsubDataBiosGetPtr(index);
        dst = &lbl_803A6AB0[(u16)index];
        *dst = *src;
    }

    while (1) {
        result = menuOpenCustom(0x8A, 0, 0, 0, 1, 0);
        if (result == -1) {
            menuCloseCustom(0x8A, 0, 1);

            for (index = 0; (u16)index < 0x229; index++) {
                dst = fightTrainerAiValueAddsubDataBiosGetPtr(index);
                src = &lbl_803A6AB0[(u16)index];
                *dst = *src;
            }

            return -1;
        }

        if (result == -2) {
            if (menuOpen(0x44, 1) != 0) {
                menuCloseCustom(0x44, 0, 1);
                continue;
            }

            menuCloseCustom(0x44, 0, 1);
            break;
        }

        msg = menuGetCursorItemID(0x8A);
        switch (msg) {
        case 0x5BC:
            HANDLE_BAG_VALUE(1, value24);
            continue;
        case 0x5BD:
            HANDLE_BAG_VALUE(2, value23);
            continue;
        case 0x5BE:
            HANDLE_BAG_VALUE(3, value22);
            continue;
        case 0x5BF:
            HANDLE_BAG_VALUE(4, value21);
            continue;
        case 0x5C0:
            HANDLE_BAG_VALUE(5, value20);
            continue;
        case 0x5C1:
            HANDLE_BAG_VALUE(6, value19);
            continue;
        case 0xFEC:
            HANDLE_BAG_VALUE(7, value18);
            continue;
        case 0x766:
            HANDLE_BAG_VALUE(8, value17);
            continue;
        case 0x5C2:
            HANDLE_BAG_VALUE(0x19, value16);
            continue;
        case 0x5C3:
            HANDLE_BAG_VALUE(9, value15);
            continue;
        case 0x5C4:
            HANDLE_BAG_VALUE(0xA, value14);
            continue;
        case 0x5C5:
            HANDLE_BAG_VALUE(0xB, value13);
            continue;
        case 0x5C6:
            HANDLE_BAG_VALUE(0xC, value12);
            continue;
        case 0x5C7:
            HANDLE_BAG_VALUE(0xD, value11);
            continue;
        case 0x5C8:
            HANDLE_BAG_VALUE(0xE, value10);
            continue;
        case 0x5C9:
            HANDLE_BAG_VALUE(0xF, value9);
            continue;
        case 0x5CA:
            HANDLE_BAG_VALUE(0x10, value8);
            continue;
        case 0x5CB:
            HANDLE_BAG_VALUE(0x11, value7);
            continue;
        case 0x5CC:
            HANDLE_BAG_VALUE(0x12, value6);
            continue;
        case 0x5CD:
            HANDLE_BAG_VALUE(0x13, value5);
            continue;
        case 0x5CE:
            HANDLE_BAG_VALUE(0x14, value4);
            continue;
        case 0x5CF:
            HANDLE_BAG_VALUE(0x15, value3);
            continue;
        case 0x5D0:
            HANDLE_BAG_VALUE(0x16, value2);
            continue;
        case 0x5D1:
            HANDLE_BAG_VALUE(0x17, value1);
            continue;
        case 0x5D2:
            HANDLE_BAG_VALUE(0x18, value0);
            continue;
        default:
            continue;
        }
    }

    menuCloseCustom(0x8A, 0, 1);
    return 1;
}
