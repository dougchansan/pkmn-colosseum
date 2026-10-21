#include "dolphin/types.h"

/* Retail stores this six-entry retry table in small uninitialized data. */
extern u16 lbl_8047A684[6] __attribute__((section(".sdata")));

/*
 * Runs gbaCmdUpload22 for a 1-based port and clears the port's retry
 * counter; returns gbaCmdUpload22's result.
 */
s32 gbaPortUpload22(s32 index)
{
    extern s32 gbaCmdUpload22(s32);
    s32 result;

    result = gbaCmdUpload22(index - 1);
    lbl_8047A684[index - 1] = 0;
    return result;
}

void fn_80089D74(s32 index)
{
    extern void gbaCmdHandshakeB(s32);
    gbaCmdHandshakeB(index - 1);
}
