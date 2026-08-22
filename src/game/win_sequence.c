#include "dolphin/types.h"

extern void* menuItemBiosGetPtr(s16 idx);
extern u32 fn_800D3088(void);
extern void _winSeqMoveSub(void* targetPtr, void* statePtr);
extern void* windowSearchID(s32 id);
extern u8 lbl_80404B68[];

void fn_80106F98(s32 windowId)
{
    u8* window = windowSearchID(windowId);
    u8* node;

    if (window == NULL) {
        return;
    }
    for (node = *(u8**)(window + 0x20); node != NULL; node = *(u8**)node) {
        u8* scratch = lbl_80404B68;
        u32 i;
        u8* item;

        *(u32*)(node + 0x0C) = 0;
        *(u16*)(node + 0x10) = 0;
        *(s16*)(scratch + 0x00) = *(s16*)(node + 0x50);
        *(s16*)(scratch + 0x02) = *(s16*)(node + 0x52);
        *(u32*)(scratch + 0x04) = *(u32*)(node + 0x64);
        *(f32*)(scratch + 0x0C) = *(f32*)(node + 0x68);
        *(f32*)(scratch + 0x10) = *(f32*)(node + 0x6C);
        scratch[0x20] = node[0x04];
        *(u32*)(scratch + 0x14) = *(u32*)(node + 0x58);
        *(s16*)(scratch + 0x18) = *(s16*)(node + 0x5C);
        *(s16*)(scratch + 0x1A) = *(s16*)(node + 0x5E);
        *(s16*)(scratch + 0x1C) = *(s16*)(node + 0x60);
        *(s16*)(scratch + 0x1E) = *(s16*)(node + 0x62);
        item = menuItemBiosGetPtr(*(s16*)(node + 0x06));
        *(s16*)(scratch + 0x08) = *(s16*)(item + 0x02);
        item = menuItemBiosGetPtr(*(s16*)(node + 0x06));
        *(s16*)(scratch + 0x0A) = *(s16*)(item + 0x04);
        for (i = 0; i < fn_800D3088(); i++) {
            _winSeqMoveSub(scratch, node + 0x0C);
        }
        *(s16*)(node + 0x50) = *(s16*)(scratch + 0x00);
        *(s16*)(node + 0x52) = *(s16*)(scratch + 0x02);
        *(u32*)(node + 0x64) = *(u32*)(scratch + 0x04);
        *(f32*)(node + 0x68) = *(f32*)(scratch + 0x0C);
        *(f32*)(node + 0x6C) = *(f32*)(scratch + 0x10);
        node[0x04] = scratch[0x20];
        *(u32*)(node + 0x58) = *(u32*)(scratch + 0x14);
        *(s16*)(node + 0x5C) = *(s16*)(scratch + 0x18);
        *(s16*)(node + 0x5E) = *(s16*)(scratch + 0x1A);
        *(s16*)(node + 0x60) = *(s16*)(scratch + 0x1C);
        *(s16*)(node + 0x62) = *(s16*)(scratch + 0x1E);
    }
}
