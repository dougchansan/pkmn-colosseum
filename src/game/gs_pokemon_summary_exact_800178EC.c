#include "dolphin/types.h"

typedef struct SummaryPageContext SummaryPageContext;

typedef struct SummaryDrawItem {
    u8 unk_00[0x06];
    s16 speciesId;
    u8 unk_08[0x5C];
    u8 color[4];
} SummaryDrawItem;

extern u32 lbl_8047A2E0;

s32 bagMenuDrawItem2(SummaryPageContext* unused, SummaryDrawItem* item)
{
    if ((s32)lbl_8047A2E0 != 3) {
        item->color[3] = 0;
    } else {
        item->color[3] = 0xCC;
    }
    return 0;
}

s32 bagMenuDrawItem3(SummaryPageContext* unused, SummaryDrawItem* item)
{
    if ((s32)lbl_8047A2E0 != 3) {
        item->color[3] = 0;
    } else {
        item->color[3] = 0xFF;
    }
    return 0;
}
