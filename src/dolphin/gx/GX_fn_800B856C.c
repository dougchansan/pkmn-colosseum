#include "dolphin/types.h"

void GXInvalidateVtxCache(void) {
    *(volatile u8*)0xCC008000 = 0x48;
}
