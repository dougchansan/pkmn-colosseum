/**
 * @file people_data_exact_80142A88.c
 * @brief itemSlotsClear (0x80142A88 - 0x80142B24): resets both item-status
 *        fields (0x1B and 0x1C) of each entry in an item-record array.
 *
 * Function-boundary carve of the people/item data TU (see people_data.c),
 * built with the group's flags (GC/1.3 -O4,p); text only. fn_80142B24, the
 * status setter, stays in the candidate unit.
 */
#include "dolphin/types.h"

extern void fn_80142B24(void*, u32, u16, u32, u32);

/* 0x80142A88 | 0x9C */
void itemSlotsClear(u32* base, u16 count) {
    u32* ptr;
    u16 i;

    if (base == NULL) return;

    for (i = 0; i < count; i++) {
        ptr = base + i;
        if (ptr != NULL) {
            fn_80142B24((void*)ptr, 0, 0x1b, 0, 0);
            fn_80142B24((void*)ptr, 0, 0x1c, 0, 0);
        }
    }
}
