/**
 * @file pda_candidate_80039A50.c
 * @brief PC item-list input callback, 0x80039A84 - 0x80039F44.
 *
 * RULE-EXCEPTION(user-approved): helper ordering, scalar reuse and local
 * compiler controls preserve retail allocation; see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

typedef struct PdaKeyInfo {
    u8 pad00[4];
    u16 trigger;
    u16 buttons;
} PdaKeyInfo;

extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4AC;
extern s32 lbl_8047A4B4;
extern s32 lbl_8047A4B8;
extern s32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern f32 lbl_8047BAB0;
extern void* pcboxGetItem(s32 box, s16 slot);
extern u8 fn_801429E8(void* item);

/* The data id / count of the target-th listed item in PC box 0. */
static inline s32 pdaBoxItemId(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    void* item;
    s32 i;
    s32 found;
    s32 slots;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)fn_801429E8(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

/* This call site needs a distinct local order to retain its inline registers. */
static inline s32 pdaBoxItemIdFirst(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    s32 slots;
    void* item;
    s32 i;
    s32 found;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)fn_801429E8(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

/* PC item list: cursor/page input, item swapping, and caption refresh. */
/* RULE-EXCEPTION(user-approved): allocation shaping and local compiler controls;
   see docs/RULE_EXCEPTIONS.md, itemStorageListKey. */
#pragma push
#pragma opt_lifetimes off
#pragma peephole off
s32 itemStorageListKey(void)
{
    extern PdaKeyInfo* windowGetKeyInfo(void);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern u16 itemBiosGetItemDataId(void* item);
    extern void* itemDataBiosGetPtr(u16 id);
    extern s32 itemDataBiosGetDoc(void* data);
    extern void pcboxSwapItemSlot(s32 box, s16 from, s16 to);
    extern void soundStart(s32 id, s32 a, s32 b, s32 c);
    extern f32 lbl_8047BAB4;
    extern f32 lbl_8047BAB8;
    PdaKeyInfo* keyInfo;
    s32 docSlots;
    s32 docFound;
    s32 docIndex;
    void* docItem;
    s32 docTarget;
    s32 countSlots;
    s32 countTotal;
    s32 countIndex;
    s32 slots;
    s32 found;
    s32 i;
    s32 target;
    s32 max;
    s32 cursor;
    s32 message;
    u16 keys;
    u16 itemId;
    s32 top;
    s32 previousPageTop;
    u8 moved;
    void* item;

    keyInfo = windowGetKeyInfo();
    moved = 0;
    if (lbl_8047BAB0 != lbl_8047A4C0) {
        return 0;
    }
    countTotal = pcboxGetNbItemSlot(0);
    for (target = 0; target < countTotal; target++) {
        fn_801429E8(pcboxGetItem(0, (s16)target));
    }
    target = lbl_8047A4A8 + lbl_8047A4AC;
    if (lbl_8047A4B8 < 0) {
        if ((keyInfo->trigger & 0xc0) != 0) {
            itemId = pdaBoxItemIdFirst(target);
            if (itemId != 0) {
                lbl_8047A4B8 = target;
                soundStart(0x3c6, 0, 0xff, 0);
            }
        }
    } else {
        keys = keyInfo->trigger;
        if ((keys & 0xd0) != 0) {
            itemId = pdaBoxItemId(target);
            if (itemId != 0) {
                pcboxSwapItemSlot(0, (s16)lbl_8047A4B8, (s16)target);
                soundStart(0x3c6, 0, 0xff, 0);
            } else {
                soundStart(0x3c7, 0, 0xff, 0);
            }
            lbl_8047A4B8 = -1;
        } else if ((keys & 0x20) != 0) {
            lbl_8047A4B8 = -1;
            soundStart(0x3c7, 0, 0xff, 0);
        }
    }
    countTotal = 0;
    countSlots = pcboxGetNbItemSlot(0);
    for (countIndex = 0; countIndex < countSlots; countIndex++) {
        if ((u8)fn_801429E8(pcboxGetItem(0, (s16)countIndex)) != 0) {
            countTotal++;
        }
    }
    max = countTotal + 1;
    if ((keyInfo->buttons & 2) != 0) {
        lbl_8047A4AC++;
        if (lbl_8047A4AC + lbl_8047A4A8 >= max) {
            lbl_8047A4AC = lbl_8047A4AC - 1;
        } else {
            if (lbl_8047A4AC >= 8) {
                lbl_8047A4A8 = lbl_8047A4A8 + 1;
                lbl_8047A4AC = lbl_8047A4AC - 1;
                lbl_8047A4BC = 1;
            } else {
                lbl_8047A4BC = 0;
            }
            lbl_8047A4C0 = lbl_8047BAB4;
            moved = 1;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 1) != 0 && moved == 0) {
        if (lbl_8047A4AC > 0 || lbl_8047A4A8 > 0) {
            cursor = lbl_8047A4AC - 1;
            lbl_8047A4AC = cursor;
            if (cursor < 0) {
                lbl_8047A4AC = 0;
                lbl_8047A4A8 = lbl_8047A4A8 - 1;
                lbl_8047A4BC = 1;
            } else {
                lbl_8047A4BC = 0;
            }
            lbl_8047A4C0 = lbl_8047BAB8;
            moved = 1;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 8) != 0 && moved == 0) {
        top = lbl_8047A4A8 + 8;
        if (top < max) {
            lbl_8047A4A8 = top;
            if (top + lbl_8047A4AC >= max) {
                lbl_8047A4AC = max - top - 1;
            }
            moved = 1;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if ((keyInfo->buttons & 4) != 0 && moved == 0) {
        previousPageTop = lbl_8047A4A8 - 8;
        if (previousPageTop >= 0) {
            lbl_8047A4A8 = previousPageTop;
            soundStart(0x3c5, 0, 0xff, 0);
        }
    }
    if (lbl_8047A4B8 >= 0) {
        message = 0x1b69;
    } else {
        docTarget = lbl_8047A4A8 + lbl_8047A4AC;
        itemId = pdaBoxItemId(docTarget);
        if (itemId != 0) {
            message = itemDataBiosGetDoc(itemDataBiosGetPtr(itemId));
        } else {
            message = 0x1b68;
        }
    }
    lbl_8047A4B4 = message;
    return 0;
}
#pragma pop
