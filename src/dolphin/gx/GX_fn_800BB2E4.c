#include "dolphin/types.h"

typedef struct GXData_800BB2E4 {
    u8 pad_000[0x410];
    u32 field_410;
} GXData_800BB2E4;

extern GXData_800BB2E4* gx;

u32 GXSetTexRegionCallback(u32 value) {
    u32 old = gx->field_410;
    gx->field_410 = value;
    return old;
}
