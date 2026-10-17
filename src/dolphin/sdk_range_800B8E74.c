/**
 * @file sdk_range_800B8E74.c
 * @brief GXPixModeSync, 0x800B8E74 - 0x800B8E98.
 */

#include "dolphin/types.h"

typedef struct GXData_800B8E74 {
    /* 0x000 */ u16 field_000;
    /* 0x002 */ u16 bpSent;
    /* 0x004 */ u8 pad_004[0x1D8];
    /* 0x1DC */ u32 peCtrl;
} GXData_800B8E74;

typedef union PPCWGPipe_800B8E74 {
    u8 u8;
    u16 u16;
    u32 u32;
} PPCWGPipe_800B8E74;

extern GXData_800B8E74* gx;
volatile PPCWGPipe_800B8E74 GXWGFifo_800B8E74 : 0xCC008000;

#define GX_WRITE_U8(value) (GXWGFifo_800B8E74.u8 = (u8)(value))
#define GX_WRITE_U32(value) (GXWGFifo_800B8E74.u32 = (u32)(value))
#define GX_WRITE_RAS_REG(value) \
    do {                         \
        GX_WRITE_U8(0x61);       \
        GX_WRITE_U32(value);     \
    } while (0)

void GXPixModeSync(void)
{
    GX_WRITE_RAS_REG(gx->peCtrl);
    gx->bpSent = 0;
}
