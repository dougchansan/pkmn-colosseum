/**
 * @file people_data_r49_80140A9C_suffix.c
 * @brief itemSwapSlots (0x80140A9C - 0x80140ACC): swaps two item records.
 *
 * Function-boundary carve of the item/people data TU (people_data.c), GC/1.3
 * -O4,p, text only. The record is the 4-byte {itemDataId, num} pair the
 * itemBios accessors read (people_item_getters_exact_80143C50.c); copying it
 * as a struct gives retail's stack copy of the first record.
 */
#include "dolphin/types.h"

typedef struct ItemBiosData {
    u16 itemDataId;
    u16 num;
} ItemBiosData;

void itemSwapSlots(ItemBiosData* a, ItemBiosData* b)
{
    ItemBiosData tmp;

    if (a == NULL) {
        return;
    }
    if (b == NULL) {
        return;
    }
    tmp = *a;
    *a = *b;
    *b = tmp;
}
