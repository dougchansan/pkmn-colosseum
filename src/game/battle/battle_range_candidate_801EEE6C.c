#include "dolphin/types.h"

extern u8* fn_801EF1E4(u32 data);

void fn_801EEE6C(u16 id, u16 value)
{
    u8* data = fn_801EF1E4(0);

    if (data != NULL) {
        *(u16*)(data + id * 0x18 + 0x4A6) = value;
    }
}
