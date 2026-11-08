/**
 * @file pda_exact_8003F2DC.c
 * @brief PDA list shell sort by the u16 key at +2, 0x8003F2DC - 0x8003F464
 *        (ascending or descending). Copied from pda_range_80037158.c.
 */
#include "dolphin/types.h"

extern void* memcpy(void* dst, const void* src, u32 size);

void pdaMemoSortPairs(u8* arr, s32 count, s32 dir) {
    u8 tmp[4];

    if (dir == 0) {
        u8* a;
        s32 step;
        u8* b;
        s32 j;
        s32 i;
        s32 gap;

        gap = count / 2;
        while (gap > 0) {
            step = gap * 4;
            for (i = gap; i < count; i++) {
                j = i - gap;
                a = arr + j * 4;
                while (j >= 0 &&
                       *(u16*)(a + 2) > *(u16*)((b = arr + (j + gap) * 4) + 2)) {
                    memcpy(tmp, a, 4);
                    memcpy(a, b, 4);
                    memcpy(b, tmp, 4);
                    a -= step;
                    j -= gap;
                }
            }
            gap = gap / 2;
        }
    } else {
        s32 j2;
        s32 i2;
        s32 gap2;
        u8* b2;
        u8* a2;

        gap2 = count / 2;
        while (gap2 > 0) {
            for (i2 = gap2; i2 < count; i2++) {
                j2 = i2 - gap2;
                a2 = arr + j2 * 4;
                while (j2 >= 0 &&
                       *(u16*)(a2 + 2) < *(u16*)((b2 = arr + (j2 + gap2) * 4) + 2)) {
                    memcpy(tmp, a2, 4);
                    memcpy(a2, b2, 4);
                    memcpy(b2, tmp, 4);
                    a2 -= gap2 * 4;
                    j2 -= gap2;
                }
            }
            gap2 = gap2 / 2;
        }
    }
}
