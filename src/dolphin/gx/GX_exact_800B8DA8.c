/**
 * @file GX_exact_800B8DA8.c
 * @brief Exact GX draw-done wait functions, 0x800B8DA8 - 0x800B8E74.
 */

#include "dolphin/types.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"

extern volatile u8 lbl_8047A9C8;
extern OSThreadQueue lbl_8047A9CC;

extern void GXFlush(void);

#define GX_FIFO_U8  (*(volatile u8*)0xCC008000)
#define GX_FIFO_U32 (*(volatile u32*)0xCC008000)

void fn_800B8DA8(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    while (lbl_8047A9C8 == 0) {
        OSSleepThread(&lbl_8047A9CC);
    }
    OSRestoreInterrupts(enabled);
}

#pragma dont_inline on
void GXDrawDone(void) {
    BOOL enabled;
    u32 scratch[2];

    enabled = OSDisableInterrupts();
    GX_FIFO_U8 = 0x61;
    GX_FIFO_U32 = 0x45000002;
    GXFlush();
    lbl_8047A9C8 = 0;
    OSRestoreInterrupts(enabled);

    enabled = OSDisableInterrupts();
    while (lbl_8047A9C8 == 0) {
        OSSleepThread(&lbl_8047A9CC);
    }
    OSRestoreInterrupts(enabled);
}
#pragma dont_inline reset
