#include "dolphin/types.h"

extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4AC;
extern s32 lbl_8047A4B0;
extern s32 lbl_8047A4B4;
extern void* pcboxGetItem(void*, s16);
extern u8 itemSlotIsValid(void* item);

/* RULE-EXCEPTION(user-approved): callsite-specific inline lookup layouts,
   local reuse and compiler controls; see docs/RULE_EXCEPTIONS.md, fn_8003A10C. */
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
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetItemDataId(item);
            }
        }
    }
    return 0;
}

static inline s32 pdaBoxItemNum(s32 target)
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
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetNum(item);
            }
        }
    }
    return 0;
}

/* RULE-EXCEPTION(user-approved): fn_8003A10C retains peephole off and
   opt_lifetimes off for inline allocation; see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
#pragma opt_lifetimes off
static inline s32 pdaBoxWithdrawItemId(s32 target)
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
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                i = itemBiosGetItemDataId(item);
                return i;
            }
        }
    }
    return 0;
}
static inline s32 pdaBoxDiscardItemId(s32 target)
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
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                found = itemBiosGetItemDataId(item);
                return found;
            }
        }
    }
    return 0;
}
static inline s32 pdaBoxItemNumFirst(s32 target)
{
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    s32 slots;
    s32 i;
    s32 found;
    void* item;

    slots = pcboxGetNbItemSlot(0);
    found = -1;
    for (i = 0; i < slots; i++) {
        item = pcboxGetItem(0, (s16)i);
        if ((u8)itemSlotIsValid(item) != 0) {
            found++;
            if (found >= target) {
                return itemBiosGetNum(item);
            }
        }
    }
    return 0;
}
/* PC item management: mode 0 withdraws; mode 1 confirms discarding. */
void itemStorageTransfer(s32 mode)
{
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void menuClose(s32 menu);
    extern void menuCloseSync(s32 menu, s32 sync);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern s32 itemBiosGetItemDataId(void* item);
    extern s32 itemBiosGetNum(void* item);
    extern void* itemDataBiosGetPtr(s32 id);
    extern u8 itemDataBiosGetKind(void* data);
    extern s32 heroItemCheckAddItemDataId(s32 bag, s32 id);
    extern void heroItemAddItemDataId(s32 bag, s32 id, u16 count, s32 slot);
    extern void pcboxAddItem(s32 box, s32 id, u16 count);
    extern void msgctrlSetValue(s32 id, s32 value);
    extern void winMsgOpen(s32 a, s32 message, s32 b, s32 c);
    extern void winMsgClose(s32 a);
    extern s32 itemStorageConfirm(void);
    extern s32 itemStorageSelectAmount(s32 a, s32 b, s32 c);
    s32 target;
    s32 itemId;
    s32 num;
    s32 count;
    u16 shortId;

    lbl_8047A4B0 = mode;
    while (1) {
        if (menuOpen(0x25, 1) == -1) {
            break;
        }
        target = lbl_8047A4A8 + lbl_8047A4AC;
        itemId = pdaBoxItemIdFirst(target);
        if ((u16)itemId == 0) {
            break;
        }
        switch (mode) {
        case 0:
            itemId = pdaBoxWithdrawItemId(target);
            lbl_8047A4B4 = 0x1b6a;
            if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) != 5) {
                num = pdaBoxItemNumFirst(target);
                count = itemStorageSelectAmount(1, 1, (u16)num);
            } else {
                count = 1;
            }
            if (count <= 0) {
                break;
            }
            if (heroItemCheckAddItemDataId(0, itemId) < count) {
                winMsgOpen(2, 0x1b6f, 1, 0);
                winMsgClose(1);
                break;
            }
            heroItemAddItemDataId(0, itemId, (u16)count, -1);
            pcboxAddItem(0, itemId, (u16)count);
            msgctrlSetValue(0x2d, (u16)itemId);
            msgctrlSetValue(0x2f, count);
            winMsgOpen(2, 0x1b70, 1, 0);
            winMsgClose(1);
            break;
        case 1:
            itemId = pdaBoxDiscardItemId(target);
            if (itemDataBiosGetKind(itemDataBiosGetPtr(itemId)) == 5) {
                winMsgOpen(2, 0x1b72, 1, 0);
                winMsgClose(1);
                break;
            }
            lbl_8047A4B4 = 0x1b6b;
            num = pdaBoxItemNum(target);
            count = itemStorageSelectAmount(1, 1, (u16)num);
            if (count <= 0) {
                break;
            }
            shortId = (u16)itemId;
            msgctrlSetValue(0x2d, shortId);
            msgctrlSetValue(0x2f, count);
            lbl_8047A4B4 = 0x1b6c;
            if (itemStorageConfirm() == 0) {
                break;
            }
            pcboxAddItem(0, itemId, (u16)count);
            msgctrlSetValue(0x2d, shortId);
            msgctrlSetValue(0x2f, count);
            winMsgOpen(2, 0x1b71, 1, 0);
            winMsgClose(1);
            break;
        }
    }
    menuClose(0x25);
    menuCloseSync(0x25, 1);
}
#pragma pop
