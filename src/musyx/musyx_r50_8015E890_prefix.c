#include "dolphin/types.h"

extern u8 lbl_8047AF18;

u8 fn_8015E890(void* emitter)
{
    if (lbl_8047AF18 != 0) {
        return (((u32*)emitter)[4] >> 0x10) & 1;
    }

    return 0;
}
