#include "dolphin/types.h"

extern s32 lbl_8047A4A8;
extern s32 lbl_8047A4AC;
extern s32 lbl_8047A4B4;
extern s32 lbl_8047A4B8;
extern s32 lbl_8047A4BC;
extern f32 lbl_8047A4C0;
extern f32 lbl_8047BABC;
extern void* pcboxGetItem(void* box, s16 slot);

/* RULE-EXCEPTION(user-approved): local peephole-control pragma; see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
s32 itemStorageListSeq(u8* ctx)
{
    extern void winSeqSetMenu(s32 sequence, s32 menu);
    extern u8 fn_801429E8(void* item);
    extern u16 pcboxGetNbItemSlot(s32 box);
    extern u16 itemBiosGetItemDataId(void* item);
    extern void* itemDataBiosGetPtr(s32 id);
    extern s32 itemDataBiosGetDoc(void* data);
    s8 state;
    s32 found;
    s32 i;
    void* item;
    s32 target;
    u16 itemId;
    s32 message;
    s32 slots;
    f32 f0;
    f32 f1;
    f32 f2;

    state = (s8)ctx[1];
    switch (state) {
    case 0:
        if ((s8)ctx[2] == 0) {
            winSeqSetMenu(0x25, 0xb4);
            target = lbl_8047A4A8 + lbl_8047A4AC;
            slots = pcboxGetNbItemSlot(0);
            found = -1;
            for (i = 0; i < slots; i++) {
                item = pcboxGetItem(0, (s16)i);
                if ((u8)fn_801429E8(item) != 0) {
                    found++;
                    if (found >= target) {
                        itemId = itemBiosGetItemDataId(item);
                        goto haveDoc;
                    }
                }
            }
            itemId = 0;
        haveDoc:
            if (itemId != 0) {
                message = itemDataBiosGetDoc(itemDataBiosGetPtr(itemId));
            } else {
                message = 0x1b68;
            }
            lbl_8047A4B4 = message;
            lbl_8047A4C0 = 0.0f;
            lbl_8047A4BC = 0;
            lbl_8047A4B8 = -1;
            ctx[2] = 1;
        }
        break;
    case 2:
        f2 = lbl_8047A4C0;
        f1 = 0.0f;
        if (f2 > f1) {
            f0 = f2 - lbl_8047BABC;
            lbl_8047A4C0 = f0;
            if (f0 < f1) {
                lbl_8047A4C0 = f1;
            }
        }
        f2 = lbl_8047A4C0;
        f1 = 0.0f;
        if (f2 < f1) {
            f0 = f2 + lbl_8047BABC;
            lbl_8047A4C0 = f0;
            if (f0 > f1) {
                lbl_8047A4C0 = f1;
            }
        }
        break;
    case 3:
        if ((s8)ctx[2] == 0) {
            winSeqSetMenu(0x25, 0xb8);
            ctx[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma pop
