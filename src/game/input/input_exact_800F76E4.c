/**
 * @file input_exact_800F76E4.c
 * @brief Exact input resource registration helper at 0x800F76E4.
 */
#include "dolphin/types.h"

extern u8* lbl_80478B00;

void cms_RegisterScript(u8* arg) {
    u8* head;
    u32 off;
    u32* tbl;
    s32 i;

    *(u32*)(arg + 0x14) = 0;
    head = (u8*)*(u32*)(lbl_80478B00 + 0x8);
    if (head == NULL) {
        *(u32*)(lbl_80478B00 + 0x8) = (u32)arg;
    } else {
        *(u32*)(lbl_80478B00 + 0x8) = (u32)arg;
        *(u32*)(arg + 0x14) = (u32)head;
    }
    if (*(u8*)(arg + 0xa) != 0) {
        return;
    }
    tbl = (u32*)(arg + *(u32*)(arg + 0xc));
    i = 0;
    while (i < (s32)*(u16*)(arg + 0x6)) {
        off = *tbl;
        tbl += 1;
        i += 1;
        *(u32*)(arg + off) += (u32)arg;
    }
    *(u8*)(arg + 0xa) = 1;
}
