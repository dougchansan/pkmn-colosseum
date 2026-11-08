/**
 * @file input_exact_800F75FC.c
 * @brief Exact input backend state setter at 0x800F75FC.
 */
#include "dolphin/types.h"

extern u8* lbl_80478B00;

s32 cms_PoolSetParam(u32 val) {
    *(u32*)(lbl_80478B00 + 0x10) = val;
    return 0;
}
