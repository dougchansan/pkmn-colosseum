#include "dolphin/types.h"

/* Retail stores this six-entry retry table in small uninitialized data. */
extern u16 lbl_8047A684[6] __attribute__((section(".sdata")));

s32 gbaCommWaitReadyA(s32 index)
{
    extern s32 gbaCmdWaitReady(s32);
    s32 status;

    status = gbaCmdWaitReady(index - 1);
    if (status < 0) {
        lbl_8047A684[index - 1] = 0;
    } else if (status == 1 || status == 2) {
        if (++lbl_8047A684[index - 1] <= 10) {
            status = -1;
        }
    }

    return status;
}
