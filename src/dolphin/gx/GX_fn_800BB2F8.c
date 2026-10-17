#include "dolphin/types.h"

typedef struct GXData_800BB2F8 {
    u8 pad_000[0x414];
    u32 field_414;
} GXData_800BB2F8;

extern GXData_800BB2F8* gx;

u32 GXSetTlutRegionCallback(u32 value) {
    u32 old = gx->field_414;
    gx->field_414 = value;
    return old;
}
